/**
 * @file config.h
 * @author krzsztfwtk
 * @brief Obługa konfiguracji globalnej lub tymczasowej
 * @version 2.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2025 krzsztfwtk
 * 
 */

#pragma once

#include <string>
#include <map>

/**
 * @brief Konfiguracja aplikacji, która musi być ustawiona, aby połączenie z GUI było możliwe.
 * 
 */
namespace configuration {
    extern const char* const EVENT_NAME;
    extern const char* const COMPLETION_EVENT_NAME;
    extern const char* const CONFIG_FILE;
}

/**
 * @brief Struktura przechowująca ustawienia aplikacji wczytywane przy każdym wywołaniem z GUI.
 * 
 */
struct Settings {
    std::string implementation;
    bool use_multithreading;
    int number_of_threads;
    bool use_avx;
    bool save_results;
    bool generate_chart;
    std::string output_file;
    std::string input_coeffs_file;
    std::string input_points_file;
    std::string progress_file;
    std::string computation_time_file;
};

/**
 * @brief Wczytuje ustawienia aplikacji z pliku konfiguracyjnego.
 * 
 * @param filename Nazwa pliku konfiguracyjnego.
 * @param settings Struktura przechowująca wczytane ustawienia.
 * @return true Jeśli udało się wczytać ustawienia.
 * @return false Jeśli nie udało się wczytać ustawień.
 */
bool readConfigINI(const std::string& filename, Settings& settings);

