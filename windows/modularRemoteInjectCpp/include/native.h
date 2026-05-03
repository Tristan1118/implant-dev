#pragma once
#include <windows.h>
#include <winternl.h>

// Function pointer typedefs go here as phases are implemented.
#if defined(USE_ALLOCATE_SECTION)
using NtCreateSection = NTSTATUS(NTAPI*)(
	OUT PHANDLE SectionHandle,
	IN ULONG DesiredAccess,
	IN OPTIONAL POBJECT_ATTRIBUTES ObjectAttributes,
	IN OPTIONAL PLARGE_INTEGER MaximumSize,
	IN ULONG PageAttributess,
	IN ULONG SectionAttributes,
	IN OPTIONAL HANDLE FileHandle);

using NtMapViewOfSection = NTSTATUS(NTAPI*)(
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

using NtUnmapViewOfSection = NTSTATUS(NTAPI*)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress OPTIONAL);

typedef enum _SECTION_INHERIT : DWORD {
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT, * PSECTION_INHERIT;
#endif

#if defined(USE_EXECUTE_HIJACK)
using NtGetContextThread = NTSTATUS(NTAPI*)(
	IN HANDLE ThreadHandle,
	IN OUT PCONTEXT ThreadContext);

using NtSetContextThread = NTSTATUS(NTAPI*)(
	IN HANDLE ThreadHandle,
	IN PCONTEXT ThreadContext);

using NtResumeThread = NTSTATUS(NTAPI*)(
	IN HANDLE ThreadHandle,
	OUT OPTIONAL PULONG PreviousSuspendCount);
#endif

typedef struct _NT_APIS {
    // Function pointers go here as phases are implemented.
#if defined(USE_ALLOCATE_SECTION)
    NtCreateSection ntCreateSection;
	NtMapViewOfSection ntMapViewOfSection;
	NtUnmapViewOfSection ntUnmapViewOfSection;
#endif
#if defined(USE_EXECUTE_HIJACK)
	NtGetContextThread ntGetContextThread;
	NtSetContextThread ntSetContextThread;
	NtResumeThread ntResumeThread;
#endif
} NT_APIS;

// Resolves all NTAPI function pointers. Add GetProcAddress calls
// here as new Nt* functions are needed by phase modules.
// Only resolve functions that are actually used — guard with
// the corresponding USE_* ifdef to avoid unnecessary strings in the binary.
BOOL NtapiInit(NT_APIS *api);
