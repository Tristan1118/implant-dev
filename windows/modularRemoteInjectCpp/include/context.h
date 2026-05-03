#pragma once
#include <windows.h>
#include <winternl.h>
#include <vector>
#include "native.h"

typedef struct _INJECT_CTX {
    NT_APIS             api;
    PROCESS_INFORMATION processInfo;
    HANDLE              hProcess;
    HANDLE              hThread;
    BOOL                isSuspended;
    DWORD               pid;

    PVOID               localBase; // local shellcode address
    PVOID               remoteBase; // remote shellcode address
    SIZE_T              shellcodeSize;
    std::vector<BYTE>   shellcode;
    ULONG               oldProtect;

    HANDLE              hRemoteThread;

    HANDLE              hSection;

    NTSTATUS            lastStatus;
    DWORD               lastError;
} INJECT_CTX;
