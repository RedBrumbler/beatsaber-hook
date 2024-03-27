#ifndef CONFIG_DEFINED_H
#define CONFIG_DEFINED_H

#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include "../../shared/config/config-utils.hpp"
#include "scotland2/shared/loader.hpp"
#include "scotland2/shared/modloader.h"
#include "utils/logging.hpp"

// CONFIG

bool readJson = false;
decltype(Configuration::configDir) Configuration::configDir;

using namespace rapidjson;

bool Configuration::ensureObject() {
    if (!config.IsObject()) {
        il2cpp_utils::Logger.warn("Config data for mod was invalid! Clearing.");
        config.SetObject();
        return false;
    }
    return true;
}

// Loads the config for the given mod, if it doesn't exist, will leave it as an empty object.
void Configuration::Load() {
    if (readJson) {
        return;
    }

    if (!fileexists(filePath)) {
        writefile(filePath, "{}");
    }
    Configuration::Reload();
}

void Configuration::Reload() {
    readJson = parsejsonfile(config, filePath);
    ensureObject();
}

void Configuration::Write() {
    ensureObject();

    StringBuffer buf;
    PrettyWriter<StringBuffer> writer(buf);
    config.Accept(writer);
    writefile(filePath, buf.GetString());
}

bool parsejsonfile(ConfigDocument& doc, std::string_view filename) {
    if (!fileexists(filename.data())) {
        return false;
    }
    std::ifstream is;
    is.open(filename.data());

    IStreamWrapper wrapper{ is };

    return !doc.ParseStream(wrapper).HasParseError();
}

bool parsejson(ConfigDocument& doc, std::string_view js) {
    char temp[js.length() + 1];
    memcpy(temp, js.data(), js.length());
    temp[js.length()] = '\0';

    if (doc.ParseInsitu(temp).HasParseError()) {
        return false;
    }
    return true;
}

std::string Configuration::getConfigFilePath(const modloader::ModInfo& info) {
    if (!Configuration::configDir) {
        Configuration::configDir = fmt::format(CONFIG_PATH_FORMAT, modloader_get_application_id());
        if (!direxists(Configuration::configDir->c_str())) {
            mkpath(Configuration::configDir->c_str());
        }
    }
    return *Configuration::configDir + info.id + ".json";
}

static std::optional<std::string> dataDir;
std::string getDataDir(modloader::ModInfo const& info) {
    if (!dataDir) {
        dataDir = fmt::format(PERSISTENT_DIR, modloader_get_application_id());
        if (!direxists(*dataDir)) {
            mkpath(*dataDir);
        }
    }
    return *dataDir + info.id + "/";
}

std::string getDataDir(std::string_view id) {
    if (!dataDir) {
        dataDir = fmt::format(PERSISTENT_DIR, modloader_get_application_id());
        if (!direxists(*dataDir)) {
            mkpath(*dataDir);
        }
    }
    return *dataDir + id.data() + "/";
}

#endif /* CONFIG_DEFINED_H */