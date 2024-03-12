#pragma once
#include <Windows.h>
#include "..\ini.h"

struct Config
{
    // Status
    bool Run = false;
    RECT GameRect;
    POINT GamePoint;
    bool ShowMenu = false;

    // AimBot
    bool AimBot = true;
    bool AimAtTeam = false;
    bool VisCheck = true;
    bool NoSway = true;
    int AimTargetBone = 1;
    bool DrawFov = true;
    bool RainbowFov = false;
    bool FovFilled = false;
    float AimFov = 150.f;
    float Smooth = 1.f;
    float MaxAimDistance = 150.f;
    int PredictVal = 450;
    int AimType = 0;
    int AimKeyType = 1;

    // Visual
    bool PlayerESP = true;
    bool TeamESP = false;
    bool DummyESP = true;
    bool GrenadeESP = true;
    bool PlayerGlow = true;

    bool ESP_Box = true;
    bool ESP_BoxFilled = false;
    bool ESP_Line = false;
    bool ESP_Distance = true;
    bool ESP_HealthBar = true;
    float ESP_MaxDistance = 750.f;
    int ESP_BoxType = 1;

    // System
    bool StreamProof = false;
    bool Crosshair = false;
    int CrosshairType = 0;
    int RainbowRate = 25;

    // Misc
    bool Rapidfire = false;
    bool NoRecoil = false;
    float NoRecoilVal = 1.f;
    bool ViewModelGlow = true;
    bool RainbowViewModel = true;

    // SpeedHack
    bool SpeedHack = false;
    float Speed = 6.f;
    int DownTime = 0;

    // Key
    int MenuKey = VK_END;
    int AimKey0 = VK_RBUTTON;
    int AimKey1 = VK_LBUTTON;

    // KeyBinder
    bool KeyBinding = false;
    int BindingID = 0;
};

// Using mINI
// https://github.com/metayeti/mINI
class ConfigManager
{
private:
    mINI::INIStructure ini;
public:
    bool CheckDir(char* dir);
    std::vector<std::string> GetFileList(const std::string& directoryPath);
    void SaveSetting(std::string filename);
    void LoadSetting(std::string filename);
};

extern Config cfg;
extern bool IsKeyDown(int VK);