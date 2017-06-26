// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DLLINJECTION_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DLLINJECTION_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include "stdafx.h"
#include <string>

using namespace std;

#ifdef DLLINJECTION_EXPORTS
#define DLLINJECTION_API __declspec(dllexport)
#else
#define DLLINJECTION_API __declspec(dllimport)
#endif

enum InjectionResultNum {
    SUCCESS,
    EPID_INVALID,
    EDLL_PATH_INVALID,
    EOPEN_PROCESS_FAILED,
    EFIND_LOAD_LIBRARY,
    EALLOC_MEMORY,
    EWRITE_MEMORY,
    ECREATE_THREAD,
    ETHREAD_TIMEOUT,
    ETHREAD_ABANDON,
    ETHREAD_FAILURE,
    CRITICAL_FAILURE
};

typedef struct {
    InjectionResultNum result;
    DWORD error;
} InjectionResult;

DLLINJECTION_API InjectionResult fnInject(const DWORD PROCESS_ID, string DLL_PATH);
