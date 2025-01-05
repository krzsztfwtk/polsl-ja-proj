/**
 * @file dllmain.cpp
 * @author krzsztfwtk
 * @brief Określa punkt wejścia dla aplikacji DLL.
 * @version 2.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2025 krzsztfwtk
 *
 */

#include "pch.h"
#include <immintrin.h>

extern "C" __declspec(dllexport) float HornerPolynomial(float* x, float a[], int n) {
    float result = 0.0f;

    float value = *x;

    for (int i = n - 1; i >= 0; i--) {
        result = result * value + a[i];
    }

    return result;
}

extern "C" __declspec(dllexport) __m256 HornerPolynomialAvx(float x[], float a[], int n) {
    __m256 points = _mm256_loadu_ps(&x[0]);
    __m256 result = _mm256_setzero_ps();
    for (int j = n - 1; j >= 0; --j) {
        __m256 coeff = _mm256_set1_ps(a[j]);
        result = _mm256_add_ps(_mm256_mul_ps(result, points), coeff);
    }
    return result;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

