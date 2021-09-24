#include "proc.h"


DWORD GetProcId(const wchar_t* procName) { //get PID of procName
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //takes a snapshot of the specified processes
	if (hSnap != INVALID_HANDLE_VALUE) { //if the function doesn't fail
		PROCESSENTRY32 procEntry; //entry from a list of processes
		procEntry.dwSize = sizeof(procEntry); //size of structure = size of procEntry

		if (Process32First(hSnap, &procEntry)) { //if the first entry in the process list has been copied to buffer
			do {
				if (!_wcsicmp(procEntry.szExeFile, procName)) { //compare strings
					procId = procEntry.th32ProcessID; 
					break;
				}
			} while (Process32Next(hSnap, &procEntry)); //while the next process entry exists
		}
	}
	CloseHandle(hSnap); //close handle to avoid memory leaks
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId); //takes a snapshot of all modules, second of all 32 bit modules when called from 64-bit process
	if (hSnap != INVALID_HANDLE_VALUE) { //same as in GetProcId
		MODULEENTRY32 modEntry; 
		modEntry.dwSize = sizeof(modEntry); 
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!_wcsicmp(modEntry.szModule, modName)) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) { //resolve final address after adding all the offsets
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i) {
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0); 
		addr += offsets[i]; 
		//read addr, store it there, add the offset
	}
	return addr;
}