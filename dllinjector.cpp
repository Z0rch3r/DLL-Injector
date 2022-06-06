#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>

using namespace std;

DWORD GetPid(const string& ProcessName){
	
    PROCESSENTRY32 pinfo;
    pinfo.dwSize = sizeof(PROCESSENTRY32);
    
    HANDLE ProcessesShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(ProcessesShot == INVALID_HANDLE_VALUE){
        GetLastError();
        return -1;
    }
    
    Process32First(ProcessesShot, &pinfo);
    
    if(!ProcessName.compare(pinfo.szExeFile)){
        CloseHandle(ProcessesShot);
        return pinfo.th32ProcessID;
    }
    
    while(Process32Next(ProcessesShot, &pinfo)){
        if(!ProcessName.compare(pinfo.szExeFile)){
            CloseHandle(ProcessesShot);
            return pinfo.th32ProcessID;
        }
    }
    CloseHandle(ProcessesShot);
    return 0;
}

int main(void){
		HANDLE ProcessH;
		PVOID RemoteBuff;
		char svProcname[400];
		char dllpath[400];
		
		cout << "process name : ";
		cin >> svProcname;
		cout << "dll path : ";
		cin >> dllpath;
		
		size_t dwSize = sizeof(dllpath);
		ProcessH = OpenProcess(PROCESS_ALL_ACCESS, 0, GetPid(svProcname));
		if(ProcessH == INVALID_HANDLE_VALUE){
				GetLastError();
				return -1;
		}
		
		RemoteBuff = VirtualAllocEx(ProcessH, 0, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if(RemoteBuff == 0){
				GetLastError();
				return -1;
		}
		
		BOOL WSR = WriteProcessMemory(ProcessH, RemoteBuff, (LPVOID)dllpath, dwSize, 0);
		if(!WSR){
				CloseHandle(ProcessH);
				return -1;
		}
		cout << "DLL injected successfully\n";
		
		
		HANDLE CRthread = CreateRemoteThread(ProcessH, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, RemoteBuff, 0, 0);
		if(!CRthread){
				CloseHandle(ProcessH);
				return -1;
		}
		CloseHandle(CRthread);
		return 0;
}
