// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
using namespace std;

DWORD WINAPI NotifyAttached( LPVOID param ) 
{
    DWORD pID = GetCurrentProcessId();
    wstring message = TEXT("Hello, I've been injected into process: ") + std::to_wstring(pID);
    MessageBox(NULL, message.c_str(), TEXT("Injection success"), MB_ICONEXCLAMATION | MB_DEFAULT_DESKTOP_ONLY);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, NotifyAttached, NULL, NULL, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

