#include "debug.h"
#include "phases/cleanup/cleanup.h"
#include <cstdio>

BOOL Cleanup(INJECT_CTX *ctx) {
    DBG("Starting cleanup");

#if defined(USE_ALLOCATE_SECTION)
    if (ctx->localBase) {
        ctx->lastStatus = ctx->api.ntUnmapViewOfSection(GetCurrentProcess(), ctx->localBase);
        if (!NT_SUCCESS(ctx->lastStatus)) {
            DBG("Failed to unmap local view. NTSTATUS: 0x%08X", ctx->lastStatus);
        }
        else {
            DBG("Unmapped local view");
        }
        ctx->localBase = NULL;
    }
#endif

    if (ctx->hSection) {
        ctx->lastStatus = ctx->api.ntClose(ctx->hSection);
        if (!NT_SUCCESS(ctx->lastStatus)) {
            DBG("Failed to close section handle. NTSTATUS: 0x%08X", ctx->lastStatus);
        }
        else {
            DBG("Closed section handle");
        }
        ctx->hSection = NULL;
    }

    if (ctx->hRemoteThread) {
        ctx->lastStatus = ctx->api.ntClose(ctx->hRemoteThread);
        if (!NT_SUCCESS(ctx->lastStatus)) {
            DBG("Failed to close remote thread handle. NTSTATUS: 0x%08X", ctx->lastStatus);
        }
        else {
            DBG("Closed remote thread handle");
        }
        ctx->hRemoteThread = NULL;
    }

    if (ctx->processInfo.hThread) {
        ctx->lastStatus = ctx->api.ntClose(ctx->processInfo.hThread);
        if (!NT_SUCCESS(ctx->lastStatus)) {
            DBG("Failed to close thread handle. NTSTATUS: 0x%08X", ctx->lastStatus);
        }
        else {
            DBG("Closed thread handle");
        }
        ctx->processInfo.hThread = NULL;
    }

    if (ctx->processInfo.hProcess) {
        ctx->lastStatus = ctx->api.ntClose(ctx->processInfo.hProcess);
        if (!NT_SUCCESS(ctx->lastStatus)) {
            DBG("Failed to close process handle. NTSTATUS: 0x%08X", ctx->lastStatus);
        }
        else {
            DBG("Closed process handle");
        }
        ctx->processInfo.hProcess = NULL;
    }

    if (!ctx->shellcode.empty()) {
        SecureZeroMemory(ctx->shellcode.data(), ctx->shellcode.size());
        ctx->shellcode.clear();
        DBG("Cleared shellcode from memory");
    }

    DBG("Cleanup complete");
    return TRUE;
}
