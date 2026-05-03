#include "native.h"
#include "debug.h"

BOOL NtapiInit(NT_APIS *api) {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        DBG("Failed to load ntdll");
        return FALSE;
    }

    // Add GetProcAddress resolution here as phases are implemented.
    // Guard each block with the corresponding USE_* ifdef
    api->ntClose = (fnNtClose)GetProcAddress(hNtdll, "NtClose");
#if defined(USE_ALLOCATE_SECTION)
    api->ntCreateSection = (fnNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
    api->ntMapViewOfSection = (fnNtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection");
    api->ntUnmapViewOfSection = (fnNtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");
#endif
#if defined(USE_EXECUTE_HIJACK)
    api->ntGetContextThread = (fnNtGetContextThread)GetProcAddress(hNtdll, "NtGetContextThread");
    api->ntSetContextThread = (fnNtSetContextThread)GetProcAddress(hNtdll, "NtSetContextThread");
    api->ntResumeThread = (fnNtResumeThread)GetProcAddress(hNtdll, "NtResumeThread");
#endif

    return TRUE;
}
