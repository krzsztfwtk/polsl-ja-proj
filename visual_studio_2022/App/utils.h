/**
 * @file utils.h
 * @author krzsztfwtk
 * @brief Funkcje pomocnicze wykorzystywane w r�nych modu�ach
 * @version 2.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2025 krzsztfwtk
 *
 */

#pragma once

#include <vector>
#include <string>

/**
 * @brief Wczytuje wsp�czynniki wielomianu z pliku.
 *
 * @param filename Nazwa pliku zawieraj�cego wsp�czynniki.
 * @param coefficients Wektor, do kt�rego zostan� zapisane wsp�czynniki.
 * @return true Je�li odczyt zako�czy� si� sukcesem.
 * @return false Je�li odczyt nie powi�d� si� (np. plik nie istnieje lub ma z�y format).
 */
bool readCoefficients(const std::string& filename, std::vector<float>& coefficients);


/**
 * @brief Wczytuje punkty, dla kt�rych b�dzie obliczany wielomian, z pliku.
 *
 * @param filename Nazwa pliku zawieraj�cego punkty.
 * @param points Wektor, do kt�rego zostan� zapisane punkty.
 * @return true Je�li operacja zako�czy�a si� powodzeniem.
 * @return false Je�li operacja nie powiod�a si�.
 */
bool readPoints(const std::string& filename, std::vector<float>& points);

/**
 * @brief Zapisuje wyniki oblicze� do pliku.
 *
 * @param filename Nazwa pliku, do kt�rego zostan� zapisane wyniki.
 * @param results Wektor zawieraj�cy wyniki oblicze�.
 * @return true Je�li operacja zako�czy�a si� powodzeniem.
 * @return false Je�li operacja nie powiod�a si�.
 */
bool writeResults(const std::string& filename, const std::vector<float>& results);

/**
 * @brief Zapisuje czas oblicze� do pliku.
 *
 * @param filename Nazwa pliku, do kt�rego zostanie zapisany czas oblicze�.
 * @param computationTime Czas oblicze� w sekundach.
 * @return true Je�li operacja zako�czy�a si� powodzeniem.
 * @return false Je�li operacja nie powiod�a si�.
 */
bool writeComputationTime(const std::string& filename, double computationTime);

/**
 * @brief Zapisuje post�p oblicze� do pliku.
 *
 * @param filename Nazwa pliku, do kt�rego zostanie zapisany post�p oblicze�.
 * @param progress Aktualny post�p w procentach.
 * @return true Je�li operacja zako�czy�a si� powodzeniem.
 * @return false Je�li operacja nie powiod�a si�.
 */
bool writeProgress(const std::string& filename, int progress);