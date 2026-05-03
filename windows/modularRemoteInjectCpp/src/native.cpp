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
#if defined(USE_ALLOCATE_SECTION)
    api->ntCreateSection = (NtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
    api->ntMapViewOfSection = (NtMapViewOfSection)GetProcAddress(hNtdll, "NtMapViewOfSection");
    api->ntUnmapViewOfSection = (NtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");
#endif
#if defined(USE_EXECUTE_HIJACK)
    api->ntGetContextThread = (NtGetContextThread)GetProcAddress(hNtdll, "NtGetContextThread");
    api->ntSetContextThread = (NtSetContextThread)GetProcAddress(hNtdll, "NtSetContextThread");
    api->ntResumeThread = (NtResumeThread)GetProcAddress(hNtdll, "NtResumeThread");
#endif

    return TRUE;
}
