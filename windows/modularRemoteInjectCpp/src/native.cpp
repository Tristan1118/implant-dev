#include "native.h"

BOOL NtapiInit(NT_APIS *api) {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return FALSE;

    // Add GetProcAddress resolution here as phases are implemented.
    // Guard each block with the corresponding USE_* ifdef. Example:
    //
    // #ifdef USE_ALLOCATE_SECTION
    //     api->NtCreateSection = (fnNtCreateSection)GetProcAddress(hNtdll, "NtCreateSection");
    // #endif

    return TRUE;
}
