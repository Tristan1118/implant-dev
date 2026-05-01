#pragma once
#include <windows.h>
#include <winternl.h>

// Function pointer typedefs go here as phases are implemented.
// Example (do not add yet):
//   typedef NTSTATUS (NTAPI *fnNtCreateSection)(PHANDLE, ACCESS_MASK, ...);

typedef struct _NT_APIS {
    // Function pointers go here as phases are implemented.
    // Example (do not add yet):
    //   fnNtCreateSection NtCreateSection;
    int _placeholder; // remove once first real member is added
} NT_APIS;

// Resolves all NTAPI function pointers. Add GetProcAddress calls
// here as new Nt* functions are needed by phase modules.
// Only resolve functions that are actually used — guard with
// the corresponding USE_* ifdef to avoid unnecessary strings in the binary.
BOOL NtapiInit(NT_APIS *api);
