#include "phases/create/suspended.h"
#include "config.h"
#include "debug.h"
#include <cstdio>

BOOL CreateTarget_Suspended(INJECT_CTX *ctx) {
    wchar_t cmd[] = INJECTION_TARGET_PATH;
    LPSTARTUPINFOW startup_info = new STARTUPINFOW();
    startup_info->cb = sizeof(STARTUPINFOW);
    startup_info->dwFlags = STARTF_USESHOWWINDOW;

    ctx->processInfo = PROCESS_INFORMATION();
    bool success = CreateProcessW(
        NULL,
        cmd,
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW | CREATE_SUSPENDED,
        NULL,
        NULL,
        startup_info,
        &ctx->processInfo);

    if (success) {
        ctx->isSuspended = true;
        ctx->hProcess = ctx->processInfo.hProcess;
        ctx->hThread = ctx->processInfo.hThread;
        ctx->pid = ctx->processInfo.dwProcessId;
        DBG("Process created with pid %lu", ctx->pid);
        return TRUE;
    }
    else {
        DBG("Process creation failed.");
        return FALSE;
    }
}
