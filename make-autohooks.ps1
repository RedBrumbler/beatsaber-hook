# This script reads the hooking.hpp file, finds all MAKE_HOOK definitions, and generates corresponding MAKE_HOOK_AUTO definitions.
# The output is saved to autohooks.hpp.

# Define the input and output file paths
$inputFilePath = "./shared/utils/hooking.hpp"
$outputFilePath = "./shared/utils/autohooks.hpp"

# Define the regex pattern
$pattern = '(?ms)(?:^[ \t]*//[^\r\n]*\r?\n)+?^\s*#define\s+(MAKE_HOOK(?:_\w+)?)\(([^)]*)\)((?:[^\n]*\\\r?\n)+)([^\n]+)?'

# Read the file content
$fileContent = Get-Content -Path $inputFilePath -Raw

# Match the pattern and extract to an array
$matches = [regex]::Matches($fileContent, $pattern)

# Create an array to store the results
$resultArray = @()

# Iterate over the matches and add them to the result array
foreach ($match in $matches) {
    $resultArray += $match.Value
}

# Function to replace regex in a copy of the result values
function ReplaceMakeWithAuto {
    param (
            [array]$inputArray,
            [string]$modifier,
            [string]$macroPrefix = "HOOK_AUTO_REGISTER",
            [string]$macroSuffix = "",
            [string]$macroKeyword = "AUTO"
        )

    $outputArray = @()
    foreach ($item in $inputArray) {
        $lines = ([regex]::Replace($item, "(?ms)^(\s*#define\s+MAKE_)(HOOK)([^(]+)?", "`$1`$2_$macroKeyword$modifier`$3")) -split "`n"
        $lastLine = $lines[-1]
        $lines[-1] = "$macroPrefix$modifier$macroSuffix(name_); \"
        $lines += $lastLine
        $newItem = $lines -join "`n"
        $outputArray += $newItem
    }
    return $outputArray
}

# Call the function and print the modified array
$modifiedArray = ReplaceMakeWithAuto -inputArray $resultArray
$modifiedArray += ReplaceMakeWithAuto -inputArray $resultArray -modifier "_ORIG"
$modifiedArray += ReplaceMakeWithAuto -inputArray $resultArray -macroPrefix "INSTALL_HOOK" -macroSuffix "_ON_DLOPEN" -macroKeyword "DLOPEN"
$modifiedArray += ReplaceMakeWithAuto -inputArray $resultArray -modifier "_ORIG" -macroPrefix "INSTALL_HOOK" -macroSuffix "_ON_DLOPEN" -macroKeyword "DLOPEN"

# Write the output to the specified file
$outputContent = "/// @file autohooks.hpp`n" +
                 "/// @brief Contains convenience macros to create hooks and register for user-initiated`n"+
                 "/// installation with the INSTALL_HOOKS macro or automatically installed on dlopen.`n" +
                 "///`n" +
                 "/// IMPORTANT: This file is automatically generated.`n" +
                 "/// Do not edit this file directly. Any changes made to this file will be overwritten.`n" +
                 "/// Please modify the make-autohooks.ps1 script that generates this file instead.`n" +
                 "`n" +
                 "#pragma once`n" +
                 "#include `"hooking.hpp`"`n" +
                 "`n" +
                 ($modifiedArray -join "`n`n")

Set-Content -Path $outputFilePath -Value $outputContent