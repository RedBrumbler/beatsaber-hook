#include "../../shared/utils/capstone-utils.hpp"
#include <android/log.h>

csh handle;
bool valid = false;

#ifndef VERSION
#define VERSION "0.0.0"
#endif

#ifndef MOD_ID
#define MOD_ID "beatsaber-hook"
#endif

namespace cs {
void __attribute__((constructor)) init_capstone() {
    cs_err e1 = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);
    cs_option(handle, CS_OPT_DETAIL, 1);
    if (e1) {
        __android_log_print(Logging::CRITICAL, "QuestHook[" MOD_ID "|" VERSION "] capstone", "Capstone initialization failed! %u", e1);
        SAFE_ABORT();
    }
    __android_log_print(Logging::INFO, "QuestHook[" MOD_ID "|" VERSION "] capstone", "Capstone initialized!");
    valid = true;
}

csh getHandle() {
    return handle;
}

uint32_t* readb(const uint32_t* addr) {
    cs_insn* insns;
    // Read from addr, 1 instruction, with pc at addr, into insns.
    // TODO: consider using cs_disasm_iter
    auto count = cs_disasm(handle, reinterpret_cast<const uint8_t*>(addr), sizeof(uint32_t), reinterpret_cast<uint64_t>(addr), 1, &insns);
    CRASH_UNLESS(count == 1);
    auto inst = insns[0];
    // Thunks have a single b
    CRASH_UNLESS(inst.id == ARM64_INS_B);
    auto platinsn = inst.detail->arm64;
    CRASH_UNLESS(platinsn.op_count == 1);
    auto op = platinsn.operands[0];
    CRASH_UNLESS(op.type == ARM64_OP_IMM);
    // Our b dest is addr + (imm << 2), except capstone does this for us.
    auto dst = reinterpret_cast<uint32_t*>(op.imm);
    cs_free(insns, 1);
    return dst;
}

std::optional<uint32_t*> blConv(cs_insn* insn) {
    if (insn->id == ARM64_INS_BL) {
        // BL is pc + (imm << 2), capstone handles this
        return reinterpret_cast<uint32_t*>(insn->detail->arm64.operands[0].imm);
    }
    return std::nullopt;
}

std::optional<uint32_t*> bConv(cs_insn* insn) {
    if (insn->id == ARM64_INS_B) {
        // B is pc + (imm << 2), capstone handles this
        return reinterpret_cast<uint32_t*>(insn->detail->arm64.operands[0].imm);
    }
    return std::nullopt;
}

std::optional<std::tuple<uint32_t*, arm64_reg, uint32_t*>> pcRelConv(cs_insn* insn) {
    using tup = std::tuple<uint32_t*, arm64_reg, uint32_t*>;
    switch (insn->id) {
        case ARM64_INS_ADR:
        // ADR is just pc + imm, capstone handles this
        case ARM64_INS_ADRP:
            // ADRP is (pc & 1:12(0)) + (imm << 12), capstone handles this
            return tup{ reinterpret_cast<uint32_t*>(insn->address), insn->detail->arm64.operands[0].reg, reinterpret_cast<uint32_t*>(insn->detail->arm64.operands[1].imm) };
        default:
            return std::nullopt;
    }
}

std::optional<std::tuple<uint32_t*, arm64_reg, int64_t>> regMatchConv(cs_insn* match, arm64_reg toMatch) {
    // We need 1 to 2 operands, match 1 to 2 to register, determine dst reg from incoming instruction
    // For now, it's pretty common for add immediates, which have dst as first op, src 2nd, imm third
    auto& arm = match->detail->arm64;
    using tup = std::tuple<uint32_t*, arm64_reg, int64_t>;
    switch (match->id) {
        case ARM64_INS_ADD:
            if (arm.operands[1].reg != toMatch) return std::nullopt;
            return tup{ reinterpret_cast<uint32_t*>(match->address), arm.operands[0].reg, arm.operands[2].imm };
        case ARM64_INS_LDR:
            if (arm.operands[1].mem.base != toMatch) return std::nullopt;
            return tup{ reinterpret_cast<uint32_t*>(match->address), arm.operands[0].reg, arm.operands[1].mem.disp };
        // TODO: Add more conversions for instructions!
        default:
            return std::nullopt;
    }
}
}  // namespace cs