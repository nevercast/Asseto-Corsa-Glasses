// InjectorExample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tlhelp32.h> 
#include <string>
#include <iostream>

using namespace std;

#define INJECTION_DLL "D:\\Source\\Repos\\Asseto-Corsa-Glasses\\x64\\Debug\\ExampleInjectedDLL.dll"

DWORD FindTargetByProcessName(const wstring * processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    DWORD pid = NULL;
    PROCESSENTRY32 entry;
    ZeroMemory(&entry, sizeof(entry));
    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry)) {
        do {
            if (wstring(entry.szExeFile) == *processName) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return pid;
}


int wmain(int argc, wchar_t *argv[])
{
    wstring targetName;
    //WCHAR full_path[MAX_PATH];
    //GetFullPathName(TEXT(INJECTION_DLL), MAX_PATH, full_path, NULL);

    if (argc < 2) {
        cout << "Insufficient arguments. InjectorExample.exe <targetProcessName>\n";
        return 1;
    }
    targetName = wstring(argv[1]);


    DWORD pid = FindTargetByProcessName(&targetName);

    if (!pid) {
        cout << "Could not find process by name ";
        wcout << targetName;
        cout << "\n";
        return 2;
    }
    else {
        cout << "Found process\n";
        cout << "Injecting ";
        cout << INJECTION_DLL;
        cout << "...\n";
        InjectionResult result = fnInject(pid, string(INJECTION_DLL));
        printf("Result: %d. Error? %s. Err#: %d", result.result, result.error ? "Yes" : "No", result.error);
    }

    return 0;
}

