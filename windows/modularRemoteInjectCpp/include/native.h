#pragma once
#include <windows.h>
#include <winternl.h>

// Function pointer typedefs go here as phases are implemented.
using fnNtClose = NTSTATUS(NTAPI*)(
	IN HANDLE handle
	);
#if defined(USE_ALLOCATE_SECTION)
using fnNtCreateSection = NTSTATUS(NTAPI*)(
	OUT PHANDLE SectionHandle,
	IN ULONG DesiredAccess,
	IN OPTIONAL POBJECT_ATTRIBUTES ObjectAttributes,
	IN OPTIONAL PLARGE_INTEGER MaximumSize,
	IN ULONG PageAttributess,
	IN ULONG SectionAttributes,
	IN OPTIONAL HANDLE FileHandle);

using fnNtMapViewOfSection = NTSTATUS(NTAPI*)(
	IN HANDLE SectionHandle,
	IN HANDLE ProcessHandle,
	IN OUT PVOID* BaseAddress,
	IN ULONG_PTR ZeroBits,
	IN SIZE_T CommitSize,
	IN OUT OPTIONAL PLARGE_INTEGER SectionOffset,
	IN OUT PSIZE_T ViewSize,
	IN DWORD InheritDisposition,
	IN ULONG AllocationType,
	IN ULONG Win32Protect);

using fnNtUnmapViewOfSection = NTSTATUS(NTAPI*)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress OPTIONAL);

typedef enum _SECTION_INHERIT : DWORD {
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT, * PSECTION_INHERIT;
#endif

#if defined(USE_EXECUTE_HIJACK)
using fnNtGetContextThread = NTSTATUS(NTAPI*)(
	IN HANDLE ThreadHandle,
	IN OUT PCONTEXT ThreadContext);

using fnNtSetContextThread = NTSTATUS(NTAPI*)(
	IN HANDLE ThreadHandle,
	IN PCONTEXT ThreadContext);

using fnNtResumeThread = NTSTATUS(NTAPI*)(
	IN HANDLE ThreadHandle,
	OUT OPTIONAL PULONG PreviousSuspendCount);
#endif

typedef struct _NT_APIS {
    // Function pointers go here as phases are implemented.
	fnNtClose ntClose;
#if defined(USE_ALLOCATE_SECTION)
    fnNtCreateSection ntCreateSection;
	fnNtMapViewOfSection ntMapViewOfSection;
	fnNtUnmapViewOfSection ntUnmapViewOfSection;
#endif
#if defined(USE_EXECUTE_HIJACK)
	fnNtGetContextThread ntGetContextThread;
	fnNtSetContextThread ntSetContextThread;
	fnNtResumeThread ntResumeThread;
#endif
} NT_APIS;

// Resolves all NTAPI function pointers. Add GetProcAddress calls
// here as new Nt* functions are needed by phase modules.
// Only resolve functions that are actually used — guard with
// the corresponding USE_* ifdef to avoid unnecessary strings in the binary.
BOOL NtapiInit(NT_APIS *api);
