/**
 * @file horner.h
 * @author krzsztfwtk
 * @brief Plik nagłówkowy z funkcjami obliczające wartości wielomianu.
 * @version 2.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2025 krzsztfwtk
 * 
 */

#pragma once

#define NOMINMAX // aby uniknąć konfliktów nazw
#include <windows.h>
#include <vector>
#include <string>
#include <atomic>

#include "config.h"
#include "dlls.h"

/**
 * @brief Funkcja do obsługi różnych implementacji i konfiguracji obliczania wielomianu.
 * 
 * @param settings Konfiguracja wywołania funkcji obliczającej wielomian.
 * @param coefficients Lista współczynników wielomianu.
 * @param points Zbiór punktów, dla których obliczane są wartości wielomianu.
 */
void computePolynomial(
    const Settings& settings,
    const std::vector<float>& coefficients,
    const std::vector<float>& points
);

/**
 * @brief Zapisuje postęp obliczeń do pliku.
 * 
 * @param filename Plik, do którego zapisywany jest postęp obliczeń.
 * @param progress Procet postępu obliczeń.
 * @return true Poprawnie zapisano postęp obliczeń.
 * @return false Nieuadło się zapisać postępu obliczeń.
 */
bool writeProgress(const std::string& filename, int progress);

/**
 * @brief Oblicza wartości wielomianu stopnia n w punktach z tablicy points wykorzystując skalnarną implementację.
 * 
 * @param coeffs Lista współczynników wielomianu.
 * @param n Liczba współczynników.
 * @param points Zbiór punktów, dla których obliczane są wartości wielomianu.
 * @param numPoints Liczba punktów dla których obliczane są wartości wielomianu.
 * @param results Wartości wielomianu w punktach.
 * @param start Indeks początkowy punktów dla których obliczane są wartości wielomianu.
 * @param end Indeks końcowy punktów dla których obliczane są wartości wielomianu.
 * @param progressCounter Poziom postępu obliczeń.
 * @param progressUpdateInterval Przerwa między aktualizacjami postępu obliczeń.
 * @param progressFile Plik, do którego zapisywany jest poziom postępu obliczeń.
 * @param polynomialFunc Funckja skalarna obliczająca wartość wielomianu.
 */
void hornerScalar(
    float coeffs[], int n, float points[], int numPoints,
    float results[], int start, int end,
    std::atomic<int>& progressCounter, int progressUpdateInterval,
    const std::string& progressFile,
    PolynomialFunc polynomialFunc);

/**
 * @brief Oblicza wartości wielomianu stopnia n w punktach z tablicy points za pomocą implementacji AVX.
 * 
 * @param coeffs Lista współczynników wielomianu.
 * @param n Liczba współczynników.
 * @param points Zbiór punktów, dla których obliczane są wartości wielomianu.
 * @param numPoints Liczba punktów, dla których obliczane są wartości wielomianu.
 * @param results Wartości wielomianu w punktach.
 * @param start Początkowy indeks punktów, dla których obliczane są wartości wielomianu.
 * @param end Końcowy indeks punktów, dla których obliczane są wartości wielomianu.
 * @param progressCounter Poziom postępu obliczeń.
 * @param progressUpdateInterval Interwał pomiędzy aktualizacjami postępu.
 * @param progressFile Plik, do którego zapisywany jest poziom postępu obliczeń.
 * @param polynomialFuncAvx Funkcja AVX obliczająca wartość wielomianu.
 * @param polynomialFunc Funkcja skalarna obliczająca wartość wielomianu.
 */
void hornerAvx(
    float coeffs[], int n, float points[], int numPoints,
    float results[], int start, int end,
    std::atomic<int>& progressCounter, int progressUpdateInterval,
    const std::string& progressFile,
    PolynomialFuncAvx polynomialFuncAvx,
    PolynomialFunc polynomialFunc);

/**
 * @brief Oblicza wartości wielomianu stopnia n w punktach z tablicy points, korzystając ze skalarnych obliczeń wielowątkowych.
 * 
 * @param coeffs Lista współczynników wielomianu.
 * @param n Liczba współczynników wielomianu.
 * @param points Zbiór punktów, dla których obliczane są wartości wielomianu.
 * @param numPoints Liczba punktów, dla których obliczane są wartości wielomianu.
 * @param results Tablica, do której zapisywane są wyniki obliczeń.
 * @param numThreads Liczba wątków używanych w obliczeniach.
 * @param progressUpdateInterval Interwał pomiędzy aktualizacjami poziomu postępu.
 * @param progressFile Ścieżka do pliku, w którym zapisywany jest poziom postępu obliczeń.
 * @param polynomialFunc Funkcja skalarnie obliczająca wartość wielomianu.
 */
void hornerScalarMultithreaded(
    float coeffs[], int n, float points[], int numPoints,
    float results[], int numThreads, int progressUpdateInterval,
    const std::string& progressFile,
    PolynomialFunc polynomialFunc);

/**
 * @brief Oblicza wartości wielomianu stopnia n w punktach z tablicy points, wykorzystując implementację AVX oraz wielowątkowość.
 * 
 * @param coeffs Lista współczynników wielomianu.
 * @param n Liczba współczynników wielomianu.
 * @param points Zbiór punktów, dla których obliczane są wartości wielomianu.
 * @param numPoints Liczba punktów, dla których obliczane są wartości wielomianu.
 * @param results Tablica, do której zapisywane są wyniki obliczeń.
 * @param numThreads Liczba wątków używanych w obliczeniach.
 * @param progressUpdateInterval Interwał pomiędzy aktualizacjami poziomu postępu.
 * @param progressFile Ścieżka do pliku, w którym zapisywany jest poziom postępu obliczeń.
 * @param polynomialFuncAvx Funkcja AVX obliczająca wartość wielomianu.
 * @param polynomialFunc Funkcja skalarnie obliczająca wartość wielomianu.
 */
void hornerAvxMultithreaded(
    float coeffs[], int n, float points[], int numPoints,
    float results[], int numThreads, int progressUpdateInterval,
    const std::string& progressFile,
    PolynomialFuncAvx polynomialFuncAvx,
    PolynomialFunc polynomialFunc);