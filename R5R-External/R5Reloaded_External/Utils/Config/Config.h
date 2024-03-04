#pragma once
#include <Windows.h>

// ê›íËÇÃï€ë∂Ç‚ì«Ç›çûÇ›
class ConfigManager
{
private:
    // Removed
public:
    // Removed
};

// ñ˘âıÇ»íáä‘ÇΩÇø
struct cfg
{
    // System
    bool Run = false;
    int MenuKey = VK_END;
    RECT GameSize;
    POINT GamePoint;
    
    // AimBot
    bool AimBot = true;
    bool AimAtTeam = false;
    bool VisCheck = true;
    bool NoSway = true;
    float AimFov = 150.f;
    bool DrawFov = true;
    float Smooth = 1.f;
    int PredictVal = 465;
    int AimType = 0;
    float Aim_MaxDistance = 300.f;

    // Visual
    bool ESP = true;
    bool DummyESP = false;
    bool ESP_Box = true;
    bool ESP_Line = false;
    bool ESP_Distance = true;
    bool ESP_HealthBar = true;
    float ESP_MaxDistance = 500.f;

    // Keys
    int AimKey0 = 2;
    int AimKey1 = 0;

    // KeyBind status
    bool KeyBinding = false;
    int BindID = 0;
};

extern cfg g;
extern bool IsKeyDown(int VK);
