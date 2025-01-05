/**
 * @file utils.cpp
 * @author krzsztfwtk
 * @brief Funkcje pomocnicze wykorzystywane w różnych modułach
 * @version 2.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2025 krzsztfwtk
 *
 */

#include "utils.h"

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>

// Funkcja do odczytu współczynników z pliku
bool readCoefficients(const std::string& filename, std::vector<float>& coefficients) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open coefficients file: " << filename << std::endl;
        return false;
    }
    coefficients.clear();
    float value;
    std::string line;
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        if (ss >> value) {
            coefficients.push_back(value);
        }
        else {
            std::cerr << "Invalid coefficient value in file: " << line << std::endl;
            inputFile.close();
            return false;
        }
    }
    inputFile.close();

    if (coefficients.empty()) {
        std::cerr << "No coefficients found in file: " << filename << std::endl;
        return false;
    }

    return true;
}

// Funkcja do odczytu punktów z pliku (czyta start, end, step i generuje punkty)
bool readPoints(const std::string& filename, std::vector<float>& points) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open points file: " << filename << std::endl;
        return false;
    }
    float start, end, step;
    inputFile >> start >> end >> step;
    inputFile.close();

    if (step <= 0) {
        std::cerr << "Invalid step size in points file: " << step << std::endl;
        return false;
    }
    if (start > end) {
        std::cerr << "Start value is greater than end value in points file." << std::endl;
        return false;
    }

    points.clear();
    for (float current = start; current <= end; current += step) {
        points.push_back(current);
    }

    if (points.empty()) {
        std::cerr << "No points generated from file: " << filename << std::endl;
        return false;
    }

    return true;
}

// Funkcja do zapisu wyników do pliku
bool writeResults(const std::string& filename, const std::vector<float>& results) {
    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open results file: " << filename << std::endl;
        return false;
    }
    for (const auto& value : results) {
        outputFile << value << std::endl;
    }
    outputFile.close();
    return true;
}

// Funkcja do zapisu czasu obliczeń do pliku
bool writeComputationTime(const std::string& filename, double computationTime) {
    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open computation time file: " << filename << std::endl;
        return false;
    }
    outputFile << computationTime << std::endl;
    outputFile.close();
    return true;
}

// Funkcja do zapisywania postępu (już zaimplementowana wcześniej)
bool writeProgress(const std::string& filename, int progress) {
    static std::mutex progressMutex;
    std::lock_guard<std::mutex> lock(progressMutex);

    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open progress file: " << filename << std::endl;
        return false;
    }
    outputFile << progress << std::endl;
    outputFile.close();
    return true;
}