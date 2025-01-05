/**
 * @file dlls.cpp
 * @author krzsztfwtk
 * @brief Obługa dynamicznego ładowania bibliotek
 * @version 2.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2025 krzsztfwtk
 *
 */

#include <windows.h>
#include <windef.h>
#include <Winuser.h>
#include <tchar.h>
#include <sstream>

#include "dlls.h"

namespace dlls {
    HINSTANCE h_cpp = nullptr;
    HINSTANCE h_asm = nullptr;
}

namespace dll_functions {
    PolynomialFunc hornerCpp = nullptr;
    PolynomialFunc hornerAsm = nullptr;
    PolynomialFuncAvx hornerCppAvx = nullptr;
    PolynomialFuncAvx hornerAsmAvx = nullptr;
}

static void ResultMessageBox(float x, float result) {
    std::wstringstream ss;

    ss << L"W(" << x << L") = " << result;
    MessageBox(nullptr, ss.str().c_str(), L"Result", MB_OK | MB_ICONINFORMATION);
}

static std::wstring ChooseDll(const wchar_t* title) {
    wchar_t fileName[MAX_PATH] = { 0 };

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = L"Dynamic-Link Libraries (*.dll)\0*.dll\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = title;

    if (GetOpenFileName(&ofn)) {
        return fileName;
    }

    MessageBox(nullptr, L"DLL file has not been choosen.", L"Error", MB_OK | MB_ICONERROR);
    return L"";
}

void loadDLLFunctions() {
    dll_functions::hornerCpp = (PolynomialFunc)GetProcAddress(dlls::h_cpp, "HornerPolynomial");
    dll_functions::hornerAsm = (PolynomialFunc)GetProcAddress(dlls::h_asm, "HornerPolynomial");
    dll_functions::hornerCppAvx = (PolynomialFuncAvx)GetProcAddress(dlls::h_cpp, "HornerPolynomialAvx");
    dll_functions::hornerAsmAvx = (PolynomialFuncAvx)GetProcAddress(dlls::h_asm, "HornerPolynomialAvx");
}

void LoadDlls() {
    // Wybór biblioteki CPP
    std::wstring cppDllPath = ChooseDll(L"Choose CPP DLL");
    if (cppDllPath.empty()) {
        return;
    }

    // Załaduj CPP DLL
    dlls::h_cpp = LoadLibraryW(cppDllPath.c_str());
    if (!dlls::h_cpp) {
        MessageBox(nullptr, L"Failed to load CPP DLL.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Wybór biblioteki ASM
    std::wstring asmDllPath = ChooseDll(L"Choose ASM DLL");
    if (asmDllPath.empty()) {
        return;
    }

    // Załaduj ASM DLL
    dlls::h_asm = LoadLibraryW(asmDllPath.c_str());
    if (!dlls::h_asm) {
        MessageBox(nullptr, L"Failed to load ASM DLL.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    loadDLLFunctions();

    //FreeLibrary(dlls::h_asm);
    //FreeLibrary(dlls::h_cpp);
}