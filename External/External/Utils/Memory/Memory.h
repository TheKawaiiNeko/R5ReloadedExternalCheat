#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>

class Memory
{
private:
	char TargetName[64] = "Apex Legends";
	HWND TargetHwnd;
public:
	DWORD PID;
	uint64_t BaseAddress;
	HANDLE pHandle;

	bool Init();
	uintptr_t GetModuleBase(const std::string moduleName);

	bool isProcessRunning(const char* processName) 
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot) {
			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if (Process32First(hSnapshot, &pe32)) {
				do {
					if (strcmp(pe32.szExeFile, processName) == 0) {
						CloseHandle(hSnapshot);
						return true;
					}
				} while (Process32Next(hSnapshot, &pe32));
			}
			CloseHandle(hSnapshot);
		}
		return false;
	}

	template <typename T>
	constexpr const T Read(const uintptr_t& address) const noexcept
	{
		T value = { };
		ReadProcessMemory(pHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}

	template <typename T>
	constexpr void Write(const uintptr_t& address, const T& value) const noexcept
	{
		WriteProcessMemory(pHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
	}

	bool ReadString(uintptr_t address, LPVOID buffer, SIZE_T size) const
	{
		SIZE_T size_read;
		return !!::ReadProcessMemory(pHandle, LPCVOID(address), buffer, size, &size_read) && size_read > 0;
	}
};

extern Memory m;