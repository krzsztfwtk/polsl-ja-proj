/**
 * @file config.cpp
 * @author krzsztfwtk
 * @brief Obługa konfiguracji globalnej lub tymczasowej
 * @version 2.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2025 krzsztfwtk
 *
 */

#include "config.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace configuration {
    const char* const EVENT_NAME = "Global\\ComputeEvent";
    const char* const COMPLETION_EVENT_NAME = "Global\\CompletionEvent";
    const char* const CONFIG_FILE = "../../PythonGUI/config.ini";
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

int stringToInt(const std::string& str) {
    try {
        return std::stoi(str); // Converts string to int
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: cannot convert string to int\n";
        throw; // Optionally rethrow or handle
    }
    catch (const std::out_of_range& e) {
        std::cerr << "Out of range: value is too large for int\n";
        throw;
    }
}

bool readConfigINI(const std::string& filename, Settings& settings) {
    std::ifstream configFile(filename);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open configuration file: " << filename << std::endl;
        return false;
    }
    std::string line;
    std::string currentSection;
    std::map<std::string, std::string> configMap;
    while (std::getline(configFile, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue; // Skip empty lines and comments
        }
        if (line[0] == '[' && line.back() == ']') {
            // New section
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            if (!currentSection.empty()) {
                configMap[currentSection + "." + key] = value;
            }
            else {
                configMap[key] = value;
            }
        }
    }
    configFile.close();

    // Parse settings
    try {
        settings.number_of_threads = stringToInt(configMap.at("Settings.threads_number"));
    }
    catch (...) {
        settings.number_of_threads = 1; // Fallback or default value
    }

    settings.implementation = configMap["Settings.implementation"];
    settings.use_multithreading = configMap["Settings.multithreading"] == "1";
    settings.use_avx = configMap["Settings.avx"] == "1";
    settings.save_results = configMap["Settings.save_results"] == "1";
    settings.generate_chart = configMap["Settings.generate_chart"] == "1";
    settings.output_file = configMap["Settings.output_file"];
    settings.input_coeffs_file = configMap["Settings.input_coeffs_file"];
    settings.input_points_file = configMap["Settings.input_points_file"];
    settings.progress_file = configMap["Settings.progress_file"];
    settings.computation_time_file = configMap["Settings.computation_time_file"];

    return true;
}
