/**
 * @file dlls.h
 * @author krzsztfwtk
 * @brief Obługa dynamicznego ładowania bibliotek
 * @version 2.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2025 krzsztfwtk
 *
 */

#pragma once

#include <windows.h>
#include <immintrin.h>

/**
 * @brief Typedef dla funkcji wielomianowej, która może być zaimplementowana w ASM DLL lub CPP DLL.
 * 
 */
typedef float(__stdcall* PolynomialFunc)(float* x, float* a, int n);
typedef __m256(__stdcall* PolynomialFuncAvx)(float* x, float* a, int n);

/**
 * @brief Globalne uchwyty do bibliotek DLL.
 * 
 */
namespace dlls {
    extern HINSTANCE h_cpp;
    extern HINSTANCE h_asm;
}

/**
 * @brief Funkcje wielomianowe zaimplementowane w bibliotekach DLL.
 * 
 */
namespace dll_functions {
    extern PolynomialFunc hornerCpp;
    extern PolynomialFunc hornerAsm;
    extern PolynomialFuncAvx hornerCppAvx;
    extern PolynomialFuncAvx hornerAsmAvx;
}

/** 
 * @brief Funkcja do ładowania bibliotek DLL
 */
void LoadDlls();