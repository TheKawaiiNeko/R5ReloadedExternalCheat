#include "Cheat.h"
#include "SDK.h"
#include "..\Overlay\Overlay.h"
#include "..\Utils\Image\NaziRussia.h"
#include <filesystem>
#include <time.h>
#include <D3DX11.h>
#pragma comment(lib, "d3dx11.lib")

float vInfo[15000] = {};
const char* BoxList[] = { "2D Box", "2D Corner Box" };
const char* BoneList[] = { "Head", "Chest" };
const char* bAimTupeText[] = { "Crosshair", "Distance" };
const char* CrosshairList[] = { "Cross", "Apex" };
const char* AimKeyTypeList[] = { "and", "or" };

ImFont* Light = nullptr;
ImFont* Medium = nullptr;
ID3D11ShaderResourceView* NaziImage = nullptr;
void HelpMarker(const char* desc);
bool LoadImageByMemory(ID3D11Device* device, unsigned char* image, size_t image_size, ID3D11ShaderResourceView** result);

// Config
ConfigManager cfmg;
char ConfigPath[] = ".\\Config\\";

bool Cheat::Init()
{
    if (!LoadImageByMemory(g_pd3dDevice, NaziRawData, sizeof(NaziRawData), &NaziImage))
        return false;

    this->game = FindWindowA(nullptr, "Apex Legends");

    if (!cfmg.CheckDir(ConfigPath))
        system("mkdir Config");

    return true;
}

void Cheat::RenderInfo()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)cfg.GameRect.right, (float)cfg.GameRect.bottom));
    ImGui::Begin("##Info", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "%.f FPS", ImGui::GetIO().Framerate);

    // AimFov
    if (cfg.AimBot && cfg.DrawFov)
    {
        ImColor fovcol = cfg.RainbowFov ? Rainbow : FOV_User;

        Circle((float)cfg.GameRect.right, (float)cfg.GameRect.bottom, cfg.AimFov + 1.f, fovcol, 1.0f);
        if (cfg.FovFilled)
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f), cfg.AimFov, ImColor(fovcol.Value.x, fovcol.Value.y, fovcol.Value.z, 0.1f), NULL);
    }

    // Crosshair
    if (cfg.Crosshair)
    {
        switch (cfg.CrosshairType)
        {
        case 0:
            DrawLine(ImVec2(((float)cfg.GameRect.right / 2.f + 4), ((float)cfg.GameRect.bottom / 2.f)), ImVec2(((float)cfg.GameRect.right / 2.f + 10), ((float)cfg.GameRect.bottom / 2.f)), ImColor(1.f, 1.f, 1.f, 1.f), 1);
            DrawLine(ImVec2(((float)cfg.GameRect.right / 2.f - 4), ((float)cfg.GameRect.bottom / 2.f)), ImVec2(((float)cfg.GameRect.right / 2.f - 10), ((float)cfg.GameRect.bottom / 2.f)), ImColor(1.f, 1.f, 1.f, 1.f), 1);
            DrawLine(ImVec2(((float)cfg.GameRect.right / 2.f), ((float)cfg.GameRect.bottom / 2.f + 4)), ImVec2(((float)cfg.GameRect.right / 2.f), ((float)cfg.GameRect.bottom / 2.f + 10)), ImColor(1.f, 1.f, 1.f, 1.f), 1);
            DrawLine(ImVec2(((float)cfg.GameRect.right / 2.f), ((float)cfg.GameRect.bottom / 2.f - 4)), ImVec2(((float)cfg.GameRect.right / 2.f), ((float)cfg.GameRect.bottom / 2.f - 10)), ImColor(1.f, 1.f, 1.f, 1.f), 1);
            break;
        case 1:
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f), 3, ImColor(0.f, 0.f, 0.f, 1.f), NULL);
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f), 2, ImColor(1.f, 1.f, 1.f, 1.f), NULL);
            break;
        default:
            break;
        }
    }

    ImGui::End();
}

void Cheat::RenderMenu()
{
    static int menu = 0;

    ImGui::SetNextWindowSize(ImVec2(750.f, 500.f));
    ImGui::Begin("EastWare  -  R5Reloaded", &cfg.ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    //---// Clild 0 //-----------------------------------//
    ImGui::BeginChild("##BaseChild", ImVec2(150.f, ImGui::GetContentRegionAvail().y), false);

    // Icon
    ImGui::BeginChild("##TitleChild", ImVec2(ImGui::GetContentRegionAvail().x, 150.f), true);
    ImGui::Image((void*)NaziImage, ImGui::GetContentRegionAvail()); // size : 134 * 134
    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::BeginChild("##SomeChild", ImVec2(ImGui::GetContentRegionAvail()), true);

    ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y - 130.f);

    /*---| Config |----------------------------------*/ 
    ImGui::SeparatorText("Config");
    
    static int FileNum = 0;
    static char InputName[12];

    // Configフォルダ内の.iniファイルを取得、リスト化
    auto vec = cfmg.GetFileList(ConfigPath); 
    const char** FileList = new const char* [vec.size()];

    for (size_t j = 0; j < vec.size(); j++)
        FileList[j] = vec[j].c_str();

    ImGui::InputText("Name", InputName, IM_ARRAYSIZE(InputName));
    ImGui::Combo("##CfgList", &FileNum, FileList, vec.size());

    // Button
    if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 4.f, 20.f)))
    {
        if (vec.size() == 0 && InputName[0] != NULL || vec.size() != 0)
            std::thread([&]() {cfmg.SaveSetting(InputName[0] != NULL ? InputName : FileList[FileNum]); }).join();

        ZeroMemory(InputName, sizeof(InputName));
    }
    ImGui::SameLine();
    if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)) && vec.size() != 0)
        std::thread([&]() {cfmg.LoadSetting(FileList[FileNum]); }).join();

    delete[] FileList;
    /*-----------------------------------------------*/

    // Exit
    ImGui::Separator();
    ImGui::Spacing();
    if (ImGui::Button("Exit", ImVec2(ImGui::GetContentRegionAvail().x, 30.f)))
        cfg.Run = false;

    ImGui::EndChild();
    ImGui::EndChild();
    //---// Clild 0 //-----------------------------------//

    ImGui::SameLine();

    //---// Clild 1 //-----------------------------------//
    ImGui::BeginChild("BaseChild##2", ImVec2(ImGui::GetContentRegionAvail()));

    ImGuiStyle& style = ImGui::GetStyle();
    auto FramePadding = style.FramePadding;
    style.FramePadding = ImVec2(40, 8);

    if (ImGui::BeginTabBar("##ContextTabBar"))
    {
        style.FramePadding = ImVec2(40, 8);
        if (ImGui::BeginTabItem("   AimBot   "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftAimBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  AimBot");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("AimBot", &cfg.AimBot);
            ImGui::Checkbox("Aim at Team", &cfg.AimAtTeam);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  AimBot Config");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Visibility Check", &cfg.VisCheck);
            ImGui::Checkbox("NoSway", &cfg.NoSway);
            ImGui::Combo("AimBone", &cfg.AimTargetBone, BoneList, IM_ARRAYSIZE(BoneList));

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  FOV");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Draw FOV", &cfg.DrawFov);
            ImGui::Checkbox("Rainbow FOV", &cfg.RainbowFov);
            ImGui::Checkbox("Fov Filled", &cfg.FovFilled);
            ImGui::SliderFloat("FOV", &cfg.AimFov, 25.f, 650.f);
            ImGui::ColorEdit4("FOV Color", &FOV_User.Value.x);

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightAimBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  AimBot Setting");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SliderFloat("FOV", &cfg.AimFov, 25.f, 650.f);
            ImGui::SliderFloat("Smooth", &cfg.Smooth, 1.f, 50.f);
            ImGui::SliderFloat("Distance", &cfg.MaxAimDistance, 15.f, 300.f);
            ImGui::SliderInt("Prediction", &cfg.PredictVal, 200, 1200);
            ImGui::Combo("AimType", &cfg.AimType, bAimTupeText, IM_ARRAYSIZE(bAimTupeText));

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  KeyBind");
            ImGui::Separator();
            ImGui::Spacing();

            const char* text1 = KeyNames[cfg.AimKey0];
            const char* text2 = KeyNames[cfg.AimKey1];

            if (cfg.KeyBinding)
            {
                switch (cfg.BindingID)
                {
                case 1:
                    text1 = "< Press Any Key >";
                    break;
                case 2:
                    text2 = "< Press Any Key >";
                    break;
                default:
                    break;
                }
            }

            if (ImGui::Button(text1, ImVec2(215.f, 20.f)))
            {
                cfg.KeyBinding = true;
                cfg.BindingID = 1;
                std::thread([&]() {this->KeyBinder(cfg.AimKey0); }).detach();
            }

            ImGui::PushItemWidth(215.f);
            ImGui::Combo("##KeyType", &cfg.AimKeyType, AimKeyTypeList, IM_ARRAYSIZE(AimKeyTypeList));
            ImGui::PopItemWidth();

            if (ImGui::Button(text2, ImVec2(215.f, 20.f)))
            {
                cfg.KeyBinding = true;
                cfg.BindingID = 2;
                std::thread([&]() {this->KeyBinder(cfg.AimKey1); }).detach();
            }

            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(40, 8);
        if (ImGui::BeginTabItem("   Visual   "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftVisualBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  Visual");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Player ESP", &cfg.PlayerESP);
            ImGui::Checkbox("Team ESP", &cfg.TeamESP);
            ImGui::Checkbox("Dummy ESP", &cfg.DummyESP);
            ImGui::Checkbox("Grenade ESP", &cfg.GrenadeESP);
            ImGui::Checkbox("Glow", &cfg.PlayerGlow);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  ESP Options");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Box", &cfg.ESP_Box);
            ImGui::Checkbox("BoxFilled", &cfg.ESP_BoxFilled);
            ImGui::Checkbox("Line", &cfg.ESP_Line);
            ImGui::Checkbox("Distance", &cfg.ESP_Distance);
            ImGui::Checkbox("HealthBar", &cfg.ESP_HealthBar);

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightVisualBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  ESP Setting");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SliderFloat("Distance", &cfg.ESP_MaxDistance, 25.f, 2000.f);
            ImGui::Combo("Box Style", &cfg.ESP_BoxType, BoxList, IM_ARRAYSIZE(BoxList));

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  ESP Colors");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::ColorEdit4("Normal ", &ESP_Normal.Value.x);
            ImGui::ColorEdit4("Visible", &ESP_Visible.Value.x);
            ImGui::ColorEdit4("Team", &ESP_Team.Value.x);
            ImGui::ColorEdit4("BoxFilled", &ESP_Filled.Value.x);

            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(40, 8);
        if (ImGui::BeginTabItem("    Misc    "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftMiscBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 12.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  System");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("StreamProof", &cfg.StreamProof);
            ImGui::Checkbox("Crosshair", &cfg.Crosshair);
            ImGui::Combo("Type", &cfg.CrosshairType, CrosshairList, IM_ARRAYSIZE(CrosshairList));
            ImGui::SliderInt("RainbowRate", &cfg.RainbowRate, 1, 200);

            ImGui::Text("  Game");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("ViewModelGlow", &cfg.ViewModelGlow);
            ImGui::Checkbox("Rapidfire", &cfg.Rapidfire);
            ImGui::Checkbox("NoRecoil", &cfg.NoRecoil);
            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightMiscBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("   ViewModel");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("ViewModelGlow", &cfg.ViewModelGlow);
            ImGui::Checkbox("Rainbow##ViewModel", &cfg.RainbowViewModel);
            ImGui::ColorEdit4("Color##ViewModel", &ViewModel_User.Value.x);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("   NoRecoil");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("NoRecoil", &cfg.NoRecoil);
            ImGui::SliderFloat("Recoil", &cfg.NoRecoilVal, 0.f, 1.f);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("Name Spoofer"); ImGui::SameLine();
            HelpMarker("Works only offline or if client and server are [ onlineAuthEnable 0 ] \nIf server or client are [ onlineAuthEnable 1 ], you can't join server.");
            ImGui::Separator();
            ImGui::Spacing();
            static char WriteName[16];
            ImGui::InputText("Name", WriteName, IM_ARRAYSIZE(WriteName));

            if (ImGui::Button("Apply") && WriteName[3] != NULL)
            {
                for (int i = 0; i < sizeof(WriteName); i++)
                    m.Write<char>(m.BaseAddress + (0x234F35D8 + i), WriteName[i]);
            }

            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(40, 8);
        if (ImGui::BeginTabItem("Developer"))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftDevBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  Process");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("PID : %d", m.PID);
            ImGui::Text("r5apex.exe  : 0x%I64x", m.BaseAddress);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  Contact Developer");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Twitter : @WallHax_Ena");
            ImGui::Text("Discord : wallhax_ena");
            ImGui::NewLine();
            if (ImGui::Button("Open Twitter"))
                ShellExecute(NULL, "open", "https://twitter.com/WallHax_Ena", NULL, NULL, SW_SHOWDEFAULT);

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightDevBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);



            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = FramePadding;
        ImGui::EndTabBar();
    }

    ImGui::EndChild();
    //---// Clild 1 //-----------------------------------//

    ImGui::End();
}

void Cheat::RenderESP()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)cfg.GameRect.right, (float)cfg.GameRect.bottom));
    ImGui::Begin("##ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // GetLocal
    CBaseEntity lp, * pLocal = &lp;
    CBaseEntity ent, * pEntity = &ent;
    lp.ptr = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
    pLocal->Update();

    // W2S用
    uint64_t ViewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
    uint64_t tmpvmx = m.Read<uint64_t>(ViewRenderer + offset::ViewMatrix);
    Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);

    for (int i = 0; i < entitylist.size(); i++)
    {
        // LocalCheck
        if (!pLocal->IsAlive())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            break;
        }

        // Main
        char cls_name[256];
        get_class_name(entitylist[i], cls_name);
        ent.ptr = entitylist[i];
        pEntity->Update();
        std::string ClassName = cls_name;

        if (ClassName.find("CPlayer") != std::string::basic_string::npos || ClassName.find("CAI_Base") != std::string::basic_string::npos)
        {
            if (!cfg.DummyESP && ClassName.find("CAI_Base") != std::string::basic_string::npos)
                continue;
            else if (!pEntity->IsAlive())
                continue;

            // WorldToScreen
            Vector2 ScreenPosition = Vector2(0.f, 0.f);
            if (!WorldToScreen(pEntity->Position, &ViewMatrix._11, (float)cfg.GameRect.right, (float)cfg.GameRect.bottom, ScreenPosition))
                continue;

            // W2Sが有効であったらレンダリングする
            if (ScreenPosition != Vector2(0.f, 0.f))
            {
                if (!cfg.TeamESP && pEntity->Team == pLocal->Team)
                    continue;

                // Distance
                float distance = ((pEntity->Position - pLocal->Position).Length() * 0.01905f);

                if (distance < cfg.ESP_MaxDistance)
                {
                    // Head ScreenPosision
                    Vector3 HeadPosition = GetEntityBonePosition(pEntity->ptr, 8, pEntity->Position);
                    HeadPosition += Vector3(0.f, 0.f, 12.f);
                    Vector2 ScreenHeadPosition = {};
                    if (!WorldToScreen(HeadPosition, &ViewMatrix._11, (float)cfg.GameRect.right, (float)cfg.GameRect.bottom, ScreenHeadPosition))
                        continue;

                    // Box
                    float Height = abs(abs(ScreenHeadPosition.y) - abs(ScreenPosition.y));
                    float Width = Height / 1.5f;

                    // Vis check
                    bool visible = pEntity->VisTime > 0.f && pEntity->VisTime > vInfo[i];
                    vInfo[i] = pEntity->VisTime;

                    // Set ESP Color
                    ImColor ESP_Color = visible ? ESP_Visible : ESP_Normal;

                    if (cfg.TeamESP && pEntity->Team == pLocal->Team)
                        ESP_Color = ESP_Team;

                    // PlayerGlow
                    if (cfg.PlayerGlow)
                    {
                        pEntity->EnableGlow();
                        m.Write<GlowMode>(pEntity->ptr + 0x27C, { 101, 6, 85, 96 });

                        GlowColor gcol = visible ? GlowColor{ ESP_Visible.Value.x, ESP_Visible.Value.y, ESP_Visible.Value.z } : GlowColor{ ESP_Normal.Value.x, ESP_Normal.Value.y, ESP_Normal.Value.z };

                        // Team?
                        if (cfg.TeamESP && pEntity->Team == pLocal->Team)
                            gcol = { ESP_Team.Value.x, ESP_Team.Value.y, ESP_Team.Value.z };

                        m.Write<GlowColor>(pEntity->ptr + 0x1D0, { gcol });
                    }
                    else
                    {
                        // Disable Glow
                        if (m.Read<int>(pEntity->ptr + 0x310) == 1)
                            pEntity->DisableGlow();
                    }

                    // Line
                    if (cfg.ESP_Line)
                        DrawLine(ImVec2((float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom), ImVec2(ScreenPosition.x, ScreenPosition.y), ESP_Color, 1.f);

                    // ESP Box
                    if (cfg.ESP_Box)
                    {
                        switch (cfg.ESP_BoxType)
                        {
                        case 0:
                            // Normal 2D Box
                            DrawBox(ScreenPosition.x - Width / 1.85f, ScreenPosition.y, Width, -Height, ESP_Color);

                            if (cfg.ESP_BoxFilled)
                                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(ScreenPosition.x - (Width / 1.85f), ScreenPosition.y), ImVec2(ScreenPosition.x + (Width / 2.2f), ScreenPosition.y - Height), ESP_Filled, NULL);
                            break;
                        case 1:
                            // Corner ESP Box
                            DrawLine(ImVec2((ScreenPosition.x + Width / 1.85f + 1), ScreenHeadPosition.y), ImVec2((ScreenPosition.x + Width / 4.5), ScreenHeadPosition.y), ESP_Color, 2); // 右上
                            DrawLine(ImVec2((ScreenPosition.x - Width / 1.85f - 1), ScreenHeadPosition.y), ImVec2((ScreenPosition.x - Width / 4.5), ScreenHeadPosition.y), ESP_Color, 2); // 左上
                            DrawLine(ImVec2((ScreenPosition.x + Width / 1.85f), ScreenHeadPosition.y), ImVec2((ScreenPosition.x + Width / 1.85f), ScreenHeadPosition.y + Height / 4), ESP_Color, 2); // 右上横
                            DrawLine(ImVec2((ScreenPosition.x - Width / 1.85f), ScreenHeadPosition.y), ImVec2((ScreenPosition.x - Width / 1.85f), ScreenHeadPosition.y + Height / 4), ESP_Color, 2); // 左上横

                            DrawLine(ImVec2((ScreenPosition.x + Width / 1.85f), ScreenPosition.y), ImVec2((ScreenPosition.x + Width / 1.85f), ScreenPosition.y - (Height / 4)), ESP_Color, 2);
                            DrawLine(ImVec2((ScreenPosition.x - Width / 1.85f), ScreenPosition.y), ImVec2((ScreenPosition.x - Width / 1.85f), ScreenPosition.y - (Height / 4)), ESP_Color, 2);
                            DrawLine(ImVec2((ScreenPosition.x + Width / 1.85f + 1), ScreenPosition.y), ImVec2((ScreenPosition.x + Width / 4.5), ScreenPosition.y), ESP_Color, 2);
                            DrawLine(ImVec2((ScreenPosition.x - Width / 1.85f - 1), ScreenPosition.y), ImVec2((ScreenPosition.x - Width / 4.5), ScreenPosition.y), ESP_Color, 2);

                            if (cfg.ESP_BoxFilled)
                                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(ScreenPosition.x - (Width / 1.85f), ScreenPosition.y), ImVec2(ScreenPosition.x + (Width / 1.85f), ScreenPosition.y - Height), ESP_Filled, NULL);
                            break;
                        default:
                            break;
                        }
                    }

                    // Healthbar
                    if (cfg.ESP_HealthBar)
                    {
                        HealthBar((ScreenPosition.x - (Width / 1.8f) - 6.f), ScreenPosition.y, 2, -Height, pEntity->Health, pEntity->MaxHealth, true);

                        if (pEntity->Shield != 0)
                            ShieldBar((ScreenPosition.x - (Width / 1.8f) - 11.f), ScreenPosition.y, 2, -Height, pEntity->Shield, pEntity->MaxShield, true);
                    }

                    // Distance
                    if (cfg.ESP_Distance)
                    {
                        std::string m_text = std::to_string((int)distance) + "m";

                        // テキストの真ん中を取得
                        ImVec2 textSize = ImGui::CalcTextSize(m_text.c_str());
                        float TextCentor = textSize.x / 2.f;
                        String(ImVec2(ScreenPosition.x - TextCentor, ScreenPosition.y + 1.f), ImColor(1.f, 1.f, 1.f, 1.f), m_text.c_str());
                    }
                }
            }
        }
        else if (cfg.GrenadeESP && ClassName.find("CBaseGre") != std::string::basic_string::npos)
        {
            ImGui::PushFont(Medium);

            Vector2 GrenadeScreenPos = Vector2(0.f, 0.f);
            if (!WorldToScreen(pEntity->Position, &ViewMatrix._11, (float)cfg.GameRect.right, (float)cfg.GameRect.bottom, GrenadeScreenPos))
                continue;

            if (GrenadeScreenPos == Vector2(0.f, 0.f))
                continue;

            String(ImVec2(GrenadeScreenPos.x - (ImGui::CalcTextSize("[ Grenade! ]").x / 2.f), GrenadeScreenPos.y), ImColor(1.f, 0.f, 0.f, 1.f), "[ Grenade! ]");

            ImGui::PopFont();
        }
    }

    ImGui::End();
}

bool LoadImageByMemory(ID3D11Device* device, unsigned char* image, size_t image_size, ID3D11ShaderResourceView** result)
{
    D3DX11_IMAGE_LOAD_INFO Info;
    ID3DX11ThreadPump* thread = nullptr;

    auto hres = D3DX11CreateShaderResourceViewFromMemory(device, image, image_size, &Info, thread, result, 0);

    return (hres == S_OK);
}

void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}