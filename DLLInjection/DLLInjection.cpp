//
// Code is composed of lessons learnt from various sources.
// Some is copy paste, some is my own implementation.
//
#include "stdafx.h"
#include "DLLInjection.h"

#define INJECT_THREAD_TIMEOUT	10000
#define KERNEL32_DLL			TEXT("kernel32.dll")
#define LOAD_LIBRARY_A			("LoadLibraryA")

#define DECL_INJECTION_RESULT InjectionResult _result = {}
#define INJECTION_FINALIZE()    \
	if(remote_allocation)       \
		VirtualFreeEx(process,  \
			remote_allocation,  \
			strlen(DLL_PATH),   \
			MEM_RELEASE);       \
	if(process)                 \
		CloseHandle(process);   \
	return _result
#define INJECTION_SUCCESS() _result.result = SUCCESS; INJECTION_FINALIZE()
#define INJECTION_ERROR(reason) _result.result = reason; _result.error = GetLastError(); INJECTION_FINALIZE()
#define INJECTION_VERIFY(reason, bool_expr) if(!bool_expr) INJECTION_ERROR(reason)

DLLINJECTION_API InjectionResult fnInject(const DWORD PROCESS_ID, const char * DLL_PATH) {
    HANDLE	process = NULL;
    HANDLE	thread = NULL;
    LPVOID	remote_allocation = NULL;
    LPVOID	LoadLibraryAPtr = NULL;
    SIZE_T	dll_path_len = 0;

    DECL_INJECTION_RESULT;
    INJECTION_VERIFY(EPID_INVALID, PROCESS_ID);
    INJECTION_VERIFY(EDLL_PATH_INVALID, DLL_PATH);

    dll_path_len = strlen(DLL_PATH);
    INJECTION_VERIFY(EDLL_PATH_INVALID, dll_path_len);

    process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PROCESS_ID);
    INJECTION_VERIFY(EOPEN_PROCESS_FAILED, process);

    LoadLibraryAPtr = (LPVOID)GetProcAddress(GetModuleHandle(KERNEL32_DLL), LOAD_LIBRARY_A);
    INJECTION_VERIFY(EFIND_LOAD_LIBRARY, LoadLibraryAPtr);

    remote_allocation = (LPVOID)VirtualAllocEx(process, NULL, dll_path_len, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    INJECTION_VERIFY(EALLOC_MEMORY, remote_allocation);
    INJECTION_VERIFY(EWRITE_MEMORY, WriteProcessMemory(process, remote_allocation, DLL_PATH, dll_path_len, NULL));

    thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAPtr, remote_allocation, NULL, NULL);
    INJECTION_VERIFY(ECREATE_THREAD, thread);
    INJECTION_VERIFY(ETHREAD_TIMEOUT, WaitForSingleObject(thread, INJECT_THREAD_TIMEOUT));
    INJECTION_SUCCESS();
}
