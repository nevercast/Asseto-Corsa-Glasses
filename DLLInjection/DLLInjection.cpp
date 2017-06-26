//
// Code is composed of lessons learnt from various sources.
// Some is copy paste, some is my own implementation.
//
#include "stdafx.h"
#include "DLLInjection.h"
#include <string>

using namespace std;

#define INJECT_THREAD_TIMEOUT	10000
#define KERNEL32_DLL			TEXT("kernel32.dll")
#define LOAD_LIBRARY_A			("LoadLibraryA")

#define DECL_INJECTION_RESULT InjectionResult _result = { CRITICAL_FAILURE }
#define INJECTION_FINALIZE()    \
	if(remote_allocation)       \
		VirtualFreeEx(process,  \
			remote_allocation,  \
			DLL_PATH.size(),    \
			MEM_RELEASE);       \
	if(process)                 \
		CloseHandle(process);   \
	return _result
#define INJECTION_SUCCESS() { _result.result = SUCCESS; INJECTION_FINALIZE(); }
#define INJECTION_ERROR(reason) { _result.result = reason; _result.error = GetLastError(); INJECTION_FINALIZE(); }
#define INJECTION_VERIFY(reason, bool_expr) { if(!bool_expr) { INJECTION_ERROR(reason); } }

DLLINJECTION_API InjectionResult fnInject(const DWORD PROCESS_ID, string DLL_PATH) {
    HANDLE	process = NULL;
    HANDLE	thread = NULL;
    LPVOID	remote_allocation = NULL;
    LPVOID	LoadLibraryAPtr = NULL;
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    DECL_INJECTION_RESULT;
    INJECTION_VERIFY(EPID_INVALID, PROCESS_ID);
    INJECTION_VERIFY(EDLL_PATH_INVALID, DLL_PATH.c_str());

    INJECTION_VERIFY(EDLL_PATH_INVALID, DLL_PATH.size());

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, 0, &tkp, sizeof(tkp), NULL, NULL);
    }

    process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, PROCESS_ID);
    INJECTION_VERIFY(EOPEN_PROCESS_FAILED, process);

    LoadLibraryAPtr = (LPVOID)GetProcAddress(GetModuleHandle(KERNEL32_DLL), LOAD_LIBRARY_A);
    INJECTION_VERIFY(EFIND_LOAD_LIBRARY, LoadLibraryAPtr);

    DLL_PATH += '\0';
    remote_allocation = (LPVOID)VirtualAllocEx(process, NULL, DLL_PATH.size(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    INJECTION_VERIFY(EALLOC_MEMORY, remote_allocation);
    INJECTION_VERIFY(EWRITE_MEMORY, WriteProcessMemory(process, remote_allocation, DLL_PATH.c_str(), DLL_PATH.size(), NULL));

    thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAPtr, remote_allocation, NULL, NULL);
    INJECTION_VERIFY(ECREATE_THREAD, thread);

    switch (WaitForSingleObject(thread, INJECT_THREAD_TIMEOUT)) {
    case WAIT_ABANDONED:
        INJECTION_ERROR(ETHREAD_ABANDON);
        break;
    case WAIT_OBJECT_0:
        INJECTION_SUCCESS();
        break;
    case WAIT_FAILED:
        INJECTION_ERROR(ETHREAD_FAILURE);
        break;
    case WAIT_TIMEOUT:
        INJECTION_ERROR(ETHREAD_TIMEOUT);
        break;
    default:
        INJECTION_ERROR(CRITICAL_FAILURE);
    }    
}
