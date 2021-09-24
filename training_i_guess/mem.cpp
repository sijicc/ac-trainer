#include "stdafx.h"
#include "mem.h"

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess) {
	DWORD oldProtect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	//hProcess - handle to process
	//dst - base address where we change rights
	//size - size of said region in bytes
	//PAGE_EXECUTE_READWRITE - memory protection constant, enables execute and read/write access
	//&oldProtect - pointer to oldProtect, which receives previous rights
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	//dst - memory address we change
	//src - what we are changing said address to
	//size - number of bytes written
	//nullptr - pointer to a variable receiving number of bytes
	//it is optional, also we cast it to null pointer
	VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
	//we are reversing first function
}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess) { //NOPing instruction of size
	BYTE* nopArray = new BYTE[size];
	//we create BYTE pointer called nopArray
	//then we reserve said size on HEAP memory
	memset(nopArray, 0x90, size);
	//at nopArray destination we write 0x90, which is NOP instruction, size times
	PatchEx(dst, nopArray, size, hProcess);
	//calling PatchEx to write nopArray at dst
	delete[] nopArray;
	//since it is HEAP memory we have to delete nopArray to avoid memory leaks
}