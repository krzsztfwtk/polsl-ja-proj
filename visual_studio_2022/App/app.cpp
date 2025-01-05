/**
 * @file app.cpp
 * @author krzsztfwtk
 * @brief Głowny plik aplikacji serwera do obliczania wartości wielomianów
 * @version 2.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2025 krzsztfwtk
 * 
 */

/******************************************************************************
 * Temat projektu: Obliczanie wielomianu metodą Hornera
 *
 * Krótki opis algorytmu:
 * Obliczanie wartości wielomianu poprzez przekształcenie go,
 * w formę zagnieżdżoną co zmniejsza liczbę wymaganych operacji mnożenia.
 *
 * Autor: krzsztfwtk
 *
 * Wersja programu: 2.1
 * Historia zmian:
 * - Wersja 1.0: wersja początkowa
 * - Wersja 1.1: podstawowa funkcjonalność obliczania wielomian
 * - Wersja 1.2: implementacja funkcji wielomianowych w C++ w bibliotece DLL
 * - Wersja 1.3: implementacja funkcji wielomianowych w asemblerze w bibliotece DLL
 * - Wersja 1.4: komunikacja z interfejsem graficznym przez zdarzenia systemowe
 * - Wersja 2.0: pełna funkcjonalność, możliwość przeprowadzenia eksperymentu
 * - Wersja 2.1: poprawki i refaktowyzacja kodu
 *
 ******************************************************************************/

#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include "config.h"
#include "events.h"
#include "horner.h"
#include "utils.h"
#include "dlls.h"

/**
 * @brief Funckcja main ładuje biblioteki DLL, tworzy eventy i w nieskończonej pętli oczekuje na zdarzenie.
 * 
 * @param argc Liczba argumentów.
 * @param argv Opcjonalny wybór scenariusza testowego.
 * @return int Sukces: 0, Błąd: 1
 */
int main(int argc, char** argv) {
    LoadDlls();

    HANDLE hEvent = createEvent(configuration::EVENT_NAME, TRUE); // manual-reset event
    if (hEvent == NULL) {
        std::cerr << "Failed to create event: " << GetLastError() << std::endl;
        return 1;
    }

    HANDLE hCompletionEvent = createEvent(configuration::COMPLETION_EVENT_NAME, FALSE); // auto-reset event
    if (hCompletionEvent == NULL) {
        std::cerr << "Failed to create completion event: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Waiting for computation requests..." << std::endl;

    while (true) {
        DWORD waitResult = WaitForSingleObject(hEvent, INFINITE);
        if (waitResult == WAIT_OBJECT_0) {
            std::cout << "Received computation request." << std::endl;

            std::string configFilePath = configuration::CONFIG_FILE;
            std::cout << "Config file path: " << configFilePath << std::endl;

            ResetEvent(hEvent);

            Settings settings;
            if (!readConfigINI(configFilePath, settings)) {
                std::cerr << "Failed to read configuration file: " << configFilePath << std::endl;
                continue;
            }
            else {
                std::cout << "Configuration settings:" << std::endl;
                std::cout << "Implementation: " << settings.implementation << std::endl;
                std::cout << "Use multithreading: " << settings.use_multithreading << std::endl;
                std::cout << "Number of threads: " << settings.number_of_threads << std::endl;
                std::cout << "Use AVX: " << settings.use_avx << std::endl;
                std::cout << "Save results: " << settings.save_results << std::endl;
                std::cout << "Generate chart: " << settings.generate_chart << std::endl;
                std::cout << "Output file: " << settings.output_file << std::endl;
                std::cout << "Input coefficients file: " << settings.input_coeffs_file << std::endl;
                std::cout << "Input points file: " << settings.input_points_file << std::endl;
                std::cout << "Progress file: " << settings.progress_file << std::endl;
                std::cout << "Computation time file: " << settings.computation_time_file << std::endl;
            }

            // Wczytaj pliki wejściowe
            std::vector<float> coefficients;
            std::vector<float> points;

            if (!readCoefficients(settings.input_coeffs_file, coefficients)) {
                std::cerr << "Failed to read coefficients from file: " << settings.input_coeffs_file << std::endl;
                continue;
            }

            if (!readPoints(settings.input_points_file, points)) {
                std::cerr << "Failed to read points from file: " << settings.input_points_file << std::endl;
                continue;
            }

            // Wykonaj obliczenia
            computePolynomial(settings, coefficients, points);

            // Sygnał zakończenia
            SetEvent(hCompletionEvent);

        }
        else {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << std::endl;
            break;
        }
    }

    CloseHandle(hEvent);
    CloseHandle(hCompletionEvent);

    return 0;
}
