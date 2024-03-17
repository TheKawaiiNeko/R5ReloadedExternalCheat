#include "Config.h"
#include <thread>
#include <filesystem>>

Config cfg;

bool IsKeyDown(int VK)
{
    return (GetAsyncKeyState(VK) & 0x8000) != 0;
}

bool ConfigManager::CheckDir(char* dir)
{
    return std::filesystem::exists(dir) && std::filesystem::is_directory(dir);
}

std::vector<std::string> ConfigManager::GetFileList(const std::string& directoryPath)
{
    std::vector<std::string> iniFileList;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.is_regular_file())
        {
            auto path = entry.path();
            if (path.extension() == ".ini")
                iniFileList.push_back(path.filename().string());
        }
    }

    return iniFileList;
}

void ConfigManager::SaveSetting(std::string filename)
{
    if (filename.empty())
        return;

    std::string path = ".\\Config\\" + filename;

    if (filename.find(".ini") == std::string::npos)
        path += ".ini";

    mINI::INIFile file(path.c_str());

    // config.iniの存在をチェック
    if (!std::filesystem::is_regular_file(path.c_str()))
        file.generate(ini, true);
    else
        file.read(ini);

    // AimBot
    ini["aimbot"]["enable"] = std::to_string(cfg.AimBot).c_str();
    ini["aimbot"]["aimatteam"] = std::to_string(cfg.AimAtTeam).c_str();
    ini["aimbot"]["vischeck"] = std::to_string(cfg.VisCheck).c_str();
    ini["aimbot"]["nosway"] = std::to_string(cfg.NoSway).c_str();
    ini["aimbot"]["targetbone"] = std::to_string(cfg.AimTargetBone).c_str();
    ini["aimbot"]["drawfov"] = std::to_string(cfg.DrawFov).c_str();
    ini["aimbot"]["rainbowfov"] = std::to_string(cfg.RainbowFov).c_str();
    ini["aimbot"]["fovfilled"] = std::to_string(cfg.FovFilled).c_str();
    ini["aimbot"]["fov"] = std::to_string(cfg.AimFov).c_str();
    ini["aimbot"]["smooth"] = std::to_string(cfg.Smooth).c_str();
    ini["aimbot"]["distance"] = std::to_string(cfg.MaxAimDistance).c_str();
    ini["aimbot"]["prediction"] = std::to_string(cfg.PredictVal).c_str();
    ini["aimbot"]["aimtype"] = std::to_string(cfg.AimType).c_str();
    ini["aimbot"]["aimkeytyle"] = std::to_string(cfg.AimKeyType).c_str();

    // Visual
    ini["visual"]["enable"] = std::to_string(cfg.PlayerESP).c_str();
    ini["visual"]["team"] = std::to_string(cfg.TeamESP).c_str();
    ini["visual"]["dummy"] = std::to_string(cfg.DummyESP).c_str();
    ini["visual"]["grenade"] = std::to_string(cfg.GrenadeESP).c_str();
    ini["visual"]["glow"] = std::to_string(cfg.PlayerGlow).c_str();
    ini["visual"]["box"] = std::to_string(cfg.ESP_Box).c_str();
    ini["visual"]["boxfilled"] = std::to_string(cfg.ESP_BoxFilled).c_str();
    ini["visual"]["line"] = std::to_string(cfg.ESP_Line).c_str();
    ini["visual"]["distance"] = std::to_string(cfg.ESP_Distance).c_str();
    ini["visual"]["healthbar"] = std::to_string(cfg.ESP_HealthBar).c_str();
    ini["visual"]["maxdistance"] = std::to_string(cfg.ESP_MaxDistance).c_str();
    ini["visual"]["boxtype"] = std::to_string(cfg.ESP_BoxType).c_str();

    // System
    ini["system"]["streamproof"] = std::to_string(cfg.StreamProof).c_str();
    ini["system"]["rapidfire"] = std::to_string(cfg.Rapidfire).c_str();
    ini["system"]["norecoil"] = std::to_string(cfg.NoRecoil).c_str();
    ini["system"]["recoilvalue"] = std::to_string(cfg.NoRecoilVal).c_str();
    ini["system"]["viewmodelglow"] = std::to_string(cfg.ViewModelGlow).c_str();
    ini["system"]["viewmodelrainbow"] = std::to_string(cfg.RainbowViewModel).c_str();

    // Keys
    ini["key"]["aim0"] = std::to_string(cfg.AimKey0).c_str();
    ini["key"]["aim1"] = std::to_string(cfg.AimKey1).c_str();

    file.write(ini);
}

void ConfigManager::LoadSetting(std::string filename)
{
    if (filename.empty())
        return;

    std::string path = ".\\Config\\" + filename;
    mINI::INIFile file(path.c_str());

    // config.iniの存在をチェック
    if (!std::filesystem::is_regular_file(path.c_str()))
        file.generate(ini, true);
    else
        file.read(ini);

    // AimBot
    std::istringstream(ini["aimbot"]["enable"]) >> cfg.AimBot;
    std::istringstream(ini["aimbot"]["aimatteam"]) >> cfg.AimAtTeam;
    std::istringstream(ini["aimbot"]["vischeck"]) >> cfg.VisCheck;
    std::istringstream(ini["aimbot"]["nosway"]) >> cfg.NoSway;
    std::istringstream(ini["aimbot"]["targetbone"]) >> cfg.AimTargetBone;
    std::istringstream(ini["aimbot"]["drawfov"]) >> cfg.DrawFov;
    std::istringstream(ini["aimbot"]["rainbowfov"]) >> cfg.RainbowFov;
    std::istringstream(ini["aimbot"]["fovfilled"]) >> cfg.FovFilled;
    std::istringstream(ini["aimbot"]["fov"]) >> cfg.AimFov;
    std::istringstream(ini["aimbot"]["smooth"]) >> cfg.Smooth;
    std::istringstream(ini["aimbot"]["distance"]) >> cfg.MaxAimDistance;
    std::istringstream(ini["aimbot"]["prediction"]) >> cfg.PredictVal;
    std::istringstream(ini["aimbot"]["aimtype"]) >> cfg.AimType;
    std::istringstream(ini["aimbot"]["aimkeytyle"]) >> cfg.AimKeyType;

    // Visual
    std::istringstream(ini["visual"]["enable"]) >> cfg.PlayerESP;
    std::istringstream(ini["visual"]["team"]) >> cfg.TeamESP;
    std::istringstream(ini["visual"]["dummy"]) >> cfg.DummyESP;
    std::istringstream(ini["visual"]["grenade"]) >> cfg.GrenadeESP;
    std::istringstream(ini["visual"]["glow"]) >> cfg.PlayerGlow;
    std::istringstream(ini["visual"]["box"]) >> cfg.ESP_Box;
    std::istringstream(ini["visual"]["boxfilled"]) >> cfg.ESP_BoxFilled;
    std::istringstream(ini["visual"]["line"]) >> cfg.ESP_Line;
    std::istringstream(ini["visual"]["distance"]) >> cfg.ESP_Distance;
    std::istringstream(ini["visual"]["healthbar"]) >> cfg.ESP_HealthBar;
    std::istringstream(ini["visual"]["maxdistance"]) >> cfg.ESP_MaxDistance;
    std::istringstream(ini["visual"]["boxtype"]) >> cfg.ESP_BoxType;

    // System
    std::istringstream(ini["system"]["streamproof"]) >> cfg.StreamProof;
    std::istringstream(ini["system"]["rapidfire"]) >> cfg.Rapidfire;
    std::istringstream(ini["system"]["norecoil"]) >> cfg.NoRecoil;
    std::istringstream(ini["system"]["recoilvalue"]) >> cfg.NoRecoilVal;
    std::istringstream(ini["system"]["viewmodelglow"]) >> cfg.ViewModelGlow;
    std::istringstream(ini["system"]["viewmodelrainbow"]) >> cfg.RainbowViewModel;

    // Keys
    std::istringstream(ini["key"]["aim0"]) >> cfg.AimKey0;
    std::istringstream(ini["key"]["aim1"]) >> cfg.AimKey1;
}