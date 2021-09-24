#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

int main()
{
	HANDLE hProcess = 0;
	uintptr_t moduleBase = 0;
	uintptr_t localPlayerPtr = 0;
	uintptr_t healthAddr = 0;

	bool bHealth = false;
	bool bAmmo = false;
	bool bRecoil = false;

	const int newValue = 1000;
	DWORD procId = GetProcId(L"ac_client.exe"); //L converts to unicode

	if (procId) {
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId); //open a process of procId with all privilages without inheriting handle
		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");
		localPlayerPtr = moduleBase + 0x10f4f4; //offset from CE
		healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xf8 });
	}
	else {
		std::cerr << "Process not found.\n Press any key to quit.";
		getchar();
		return 0;
	}

	DWORD dwExit = 0;
	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) { // & is bit and; we do it to avoid detecting holding down button as countless instructions
			bHealth = !bHealth;
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			bAmmo = !bAmmo;

			if (bAmmo) {
				//FF 06 = inc [esi]
				// \x means that windows will take raw bytes
				//we got 0x6367e9 from seeing what acceses currentAmmo in cheat engine, it's decrement function
				//so to have infinite ammo we change it to increment
				mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x06", 2, hProcess);
			}
			else {
				//FF 0E = dec [esi]
				mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x0E", 2, hProcess);
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			bRecoil = !bRecoil;

			if (bRecoil) {
				//we got 0x63786 from seeing what acceses recoil in cheat engine
				//we NOP it for 10 bytes
				mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
			}
			else {
				//this weird bit value is 10 bytes we nop from said address
				//we put it back by using opCode
				//that is BYTES followed by \x from CE
				mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
			}
		}
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			return 0;
		}

		if (bHealth) {
			mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
		}

		Sleep(10);
	}

	std::cout << "Process not found, press a key to exit.\n";
	getchar();
	return 0;
}

