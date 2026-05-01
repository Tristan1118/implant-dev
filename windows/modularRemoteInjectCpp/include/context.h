#pragma once
#include <windows.h>
#include <winternl.h>
#include "native.h"

typedef struct _INJECT_CTX {
    NT_APIS             api;
    PROCESS_INFORMATION pi;
    HANDLE              hProcess;
    HANDLE              hThread;
    DWORD               pid;

    PVOID               remoteBase;
    SIZE_T              payloadSize;
    PBYTE               payload;
    ULONG               oldProtect;

    HANDLE              hRemoteThread;

    HANDLE              hSection;

    NTSTATUS            lastStatus;
    DWORD               lastError;
} INJECT_CTX;
