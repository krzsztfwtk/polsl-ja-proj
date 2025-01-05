/**
 * @file horner.cpp
 * @author krzsztfwtk
 * @brief Plik z implementacją funkcji obliczających wartości wielomianu.
 * @version 2.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2025 krzsztfwtk
 * 
 */

#include "horner.h"
#include "utils.h"

#include <fstream>
#include <iostream>
#include <thread>
#include <immintrin.h> // For AVX instructions
#include <atomic>
#include <mutex>

void computePolynomial(
    const Settings& settings,
    const std::vector<float>& coefficients,
    const std::vector<float>& points
) {
    int numCoeffs = (int)coefficients.size();
    int numPoints = (int)points.size();
    int numThreads = settings.number_of_threads;

    std::vector<float> results(numPoints);
    float* coeffsArray = const_cast<float*>(coefficients.data());
    float* pointsArray = const_cast<float*>(points.data());
    float* resultsArray = results.data();

    // Progress
    std::atomic<int> progressCounter(0);
    int progressUpdateInterval = std::max(2, (2 * numPoints) / 100); // update progresss by ~2%

    auto start = std::chrono::high_resolution_clock::now();


    if (settings.implementation == "cpp") {
        if (settings.use_avx) {
            if (settings.use_multithreading) {
                hornerAvxMultithreaded(coeffsArray, numCoeffs, pointsArray, numPoints,
                    resultsArray, numThreads, progressUpdateInterval, settings.progress_file,
                    dll_functions::hornerCppAvx, dll_functions::hornerCpp);
            }
            else {
                hornerAvx(coeffsArray, numCoeffs, pointsArray, numPoints,
                    resultsArray, 0, numPoints, progressCounter,
                    progressUpdateInterval, settings.progress_file,
                    dll_functions::hornerCppAvx, dll_functions::hornerCpp);
            }
        }
        else {
            if (settings.use_multithreading) {
                hornerScalarMultithreaded(coeffsArray, numCoeffs, pointsArray, numPoints,
                    resultsArray, numThreads, progressUpdateInterval, settings.progress_file,
                    dll_functions::hornerCpp);
            }
            else {
                hornerScalar(coeffsArray, numCoeffs, pointsArray, numPoints, 
                    resultsArray, 0, numPoints, progressCounter, 
                    progressUpdateInterval, settings.progress_file, dll_functions::hornerCpp);
            }
        }
    }
    else if (settings.implementation == "asm") {
        if (settings.use_avx) {
            if (settings.use_multithreading) {
                hornerAvxMultithreaded(coeffsArray, numCoeffs, pointsArray, numPoints,
                    resultsArray, numThreads, progressUpdateInterval, settings.progress_file,
                    dll_functions::hornerAsmAvx, dll_functions::hornerAsm);
            }
            else {
                hornerAvx(coeffsArray, numCoeffs, pointsArray, numPoints,
                    resultsArray, 0, numPoints, progressCounter,
                    progressUpdateInterval, settings.progress_file,
                    dll_functions::hornerAsmAvx, dll_functions::hornerAsm);
            }
        }
        else {
            if (settings.use_multithreading) {
                hornerScalarMultithreaded(coeffsArray, numCoeffs, pointsArray, numPoints,
                    resultsArray, numThreads, progressUpdateInterval, settings.progress_file,
                    dll_functions::hornerAsm);
            }
            else {
                hornerScalar(coeffsArray, numCoeffs, pointsArray, numPoints,
                    resultsArray, 0, numPoints, progressCounter,
                    progressUpdateInterval, settings.progress_file, dll_functions::hornerAsm);
            }
        }
    }
    else {
        std::cout << "wrong implementation choosen, there are only cpp and asm..." << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> computationTime = end - start;

    std::cout << "Computation completed in " << computationTime.count() << " seconds." << std::endl;

    // Save computation time
    if (!writeComputationTime(settings.computation_time_file, computationTime.count())) {
        std::cerr << "Failed to write computation time to file." << std::endl;
        return;
    }

    // Save results if output is in setting
    if (settings.save_results) {
        if (!writeResults(settings.output_file, results)) {
            std::cerr << "Failed to write results to file." << std::endl;
            return;
        }
        std::cout << "Results written to " << settings.output_file << std::endl;
    }
    else {
        std::cout << "Results not saved to file as per configuration." << std::endl;
    }
}

void hornerScalar(
    float coeffs[], int n, float points[], int numPoints,
    float results[], int start, int end,
    std::atomic<int>& progressCounter, int progressUpdateInterval, 
    const std::string& progressFile,
    PolynomialFunc polynomialFunc) {

    for (int i = start; i < end; ++i) {
        float x = points[i];

        results[i] = polynomialFunc(&x, coeffs, n);

        // Update progress
        int localCounter = progressCounter.fetch_add(1) + 1;
        if (localCounter % progressUpdateInterval == 0) {
            int progress = (localCounter * 100) / numPoints;
            writeProgress(progressFile, progress);
        }
    }
}


void hornerAvx(
    float coeffs[], int n, float points[], int numPoints,
    float results[], int start, int end,
    std::atomic<int>& progressCounter, int progressUpdateInterval,
    const std::string& progressFile,
    PolynomialFuncAvx polynomialFuncAvx,
    PolynomialFunc polynomialFunc) {

    int avxBlockSize = 8; // Process 8 points at a time
    int alignedEnd = start + ((end - start) / avxBlockSize) * avxBlockSize; // Align to block size

    for (int i = start; i < alignedEnd; i += avxBlockSize) {
        // Load 8 points into an AVX register
        float xBlock[8];
        std::copy(&points[i], &points[i + avxBlockSize], xBlock);

        // Compute the polynomial for the block of points
        __m256 resultsAvx = polynomialFuncAvx(xBlock, coeffs, n);

        // Store the AVX results back into the results array
        _mm256_storeu_ps(&results[i], resultsAvx);

        // Update progress
        int localCounter = progressCounter.fetch_add(avxBlockSize) + avxBlockSize;
        if (localCounter % progressUpdateInterval == 0) {
            int progress = (localCounter * 100) / numPoints;
            writeProgress(progressFile, progress);
        }
    }

    // Process any remaining points individually using scalar function
    for (int i = alignedEnd; i < end; ++i) {
        float x = points[i];
        results[i] = polynomialFunc(&x, coeffs, n); // Fallback to scalar function

        // Update progress
        int localCounter = progressCounter.fetch_add(1) + 1;
        if (localCounter % progressUpdateInterval == 0) {
            int progress = (localCounter * 100) / numPoints;
            writeProgress(progressFile, progress);
        }
    }
}


void hornerScalarMultithreaded(
    float coeffs[], int n, float points[], int numPoints,
    float results[], int numThreads, int progressUpdateInterval,
    const std::string& progressFile,
    PolynomialFunc polynomialFunc) {

    std::atomic<int> progressCounter(0);
    std::vector<std::thread> threads;

    int pointsPerThread = (numPoints + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; ++t) {
        int start = t * pointsPerThread;
        int end = std::min(start + pointsPerThread, numPoints);

        threads.emplace_back([=, &progressCounter]() {
            hornerScalar(
                coeffs, n, points, numPoints, results,
                start, end, progressCounter, progressUpdateInterval, progressFile,
                polynomialFunc);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void hornerAvxMultithreaded(
    float coeffs[], int n, float points[], int numPoints,
    float results[], int numThreads, int progressUpdateInterval,
    const std::string& progressFile,
    PolynomialFuncAvx polynomialFuncAvx,
    PolynomialFunc polynomialFunc) {

    std::atomic<int> progressCounter(0);
    std::vector<std::thread> threads;

    int pointsPerThread = (numPoints + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; ++t) {
        int start = t * pointsPerThread;
        int end = std::min(start + pointsPerThread, numPoints);

        threads.emplace_back([=, &progressCounter]() {
            hornerAvx(
                coeffs, n, points, numPoints, results,
                start, end, progressCounter, progressUpdateInterval, progressFile,
                polynomialFuncAvx, polynomialFunc);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}
