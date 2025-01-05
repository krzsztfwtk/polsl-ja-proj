/**
 * @file events.cpp
 * @author krzsztfwtk
 * @brief Obługa zdarzeń systemowych Windows
 * @version 2.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2025 krzsztfwtk
 *
 */

#include "events.h"

HANDLE createEvent(const char* eventName, BOOL manualReset) {
    HANDLE hEvent = CreateEventA(
        NULL,        // domyślne atrybuty zabezpieczeń
        manualReset, // TRUE dla manual-reset, FALSE dla auto-reset
        FALSE,       // początkowy stan niesygnalizowany
        eventName    // nazwa zdarzenia
    );
    return hEvent;
}