/**
 * @file events.h
 * @author krzsztfwtk
 * @brief Obługa zdarzeń systemowych Windows
 * @version 2.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2025 krzsztfwtk
 *
 */

#pragma once

#include <windows.h>

/**
 * @brief Utowrzenie eventu WinAPI.
 * 
 * @param eventName Nazwa eventu.
 * @param manualReset TRUE dla manual-reset, FALSE dla auto-reset
 * @return HANDLE Uchwyt do zdarzenia systemowego.
 */
HANDLE createEvent(const char* eventName, BOOL manualReset);
