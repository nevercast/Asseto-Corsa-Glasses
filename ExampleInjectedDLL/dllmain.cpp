// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

void NotifyAttached(void) 
{
    DWORD pID = GetCurrentProcessId();
    std::string message = "Hello, I've been injected into process: " + std::to_string(pID);
    MessageBoxA(NULL, message.c_str(), "Injection success", MB_OK);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&NotifyAttached, NULL, NULL, NULL);
        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

