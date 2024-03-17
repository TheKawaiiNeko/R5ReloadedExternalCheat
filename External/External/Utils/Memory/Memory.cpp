#include "Memory.h"

bool Memory::Init()
{
    if (isProcessRunning("EasyAntiCheat.exe"))
    {
        MessageBoxA(nullptr, "EasyAntiCheat is running.\nThis cheat is only for R5Reloaded!", "WARNING!", MB_TOPMOST | MB_ICONWARNING| MB_OK);
        return false;
    }

    TargetHwnd = FindWindowA(NULL, TargetName);

    if (!TargetHwnd)
    {
        MessageBoxA(nullptr, "Waiting R5Reloaded...", "Info", MB_TOPMOST | MB_OK);

        while (!TargetHwnd)
        {
            TargetHwnd = FindWindowA(NULL, TargetName);

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }

    GetWindowThreadProcessId(TargetHwnd, &PID);
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (!pHandle)
    {
        MessageBoxA(nullptr, "Failed to get process handle", "ERROR", MB_TOPMOST | MB_ICONERROR | MB_OK);

        return false;
    }

    BaseAddress = GetModuleBase("r5apex.exe");

    if (!BaseAddress)
    {
        MessageBoxA(nullptr, "Failed to get base address", "ERROR", MB_TOPMOST | MB_ICONERROR | MB_OK);

        return false;
    }

    return true;
}

uintptr_t Memory::GetModuleBase(const std::string moduleName)
{
    MODULEENTRY32 entry = { };
    entry.dwSize = sizeof(MODULEENTRY32);

    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);

    std::uintptr_t result = 0;

    while (Module32Next(snapShot, &entry))
    {
        if (!moduleName.compare(entry.szModule))
        {
            result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
            break;
        }
    }

    if (snapShot)
        CloseHandle(snapShot);

    return result;
}

Memory m;