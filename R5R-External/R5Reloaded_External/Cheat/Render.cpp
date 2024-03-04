#include "Cheat.h"

// メニューとかで使う変数とかいろいろ
float vInfo[15000];
const char* KeyNames[];
const char* BoneList[] = { "Head", "Chest" };

// Info
void Cheat::RenderInfo()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)g.GameSize.right, (float)g.GameSize.bottom));
    ImGui::Begin("##Info", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImGui::Text("R5Reloaded External [%.1f FPS]", ImGui::GetIO().Framerate);

    // AimFov
    if (g.AimBot && g.DrawFov)
        Circle((float)g.GameSize.right, (float)g.GameSize.bottom, g.AimFov + 1.f, Col_FOV, 1.0f);

    ImGui::End();
}

// Menu
void Cheat::RenderMenu()
{
    ImGui::SetNextWindowSize(ImVec2(775.f, 520.f));
    ImGui::Begin("// Removed", (bool*)NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    //---// Clild 0 //-----------------------------------//
    ImGui::BeginChild("##BaseChild", ImVec2(150.f, ImGui::GetContentRegionAvail().y), false);
    ImGui::BeginChild("##TitleChild", ImVec2(ImGui::GetContentRegionAvail().x, 150.f), true);

    // I FUCKING HATE NAZI-RUSSIA
    //ImGui::Image((void*)NaziRussiaImage, ImGui::GetContentRegionAvail());
    ImGui::Text("// Removed");

    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::BeginChild("##SomeChild", ImVec2(ImGui::GetContentRegionAvail()), true);

    ImGui::Text("// Removed");

    // Exit
    ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y - 10.f);
    ImGui::Separator();
    if (ImGui::Button("Exit", ImVec2(135.f, 30.f)))
        g.Run = false;

    ImGui::EndChild();
    ImGui::EndChild();
    //---// Clild 0 //-----------------------------------//

    ImGui::SameLine();

    //---// Clild 1 //-----------------------------------//
    ImGui::BeginChild("BaseChild##2", ImVec2(ImGui::GetContentRegionAvail()));

    ImGuiStyle& style = ImGui::GetStyle();
    auto FramePadding = style.FramePadding;
    style.FramePadding = ImVec2(30, 8);

    if (ImGui::BeginTabBar("##ContextTabBar"))
    {
        style.FramePadding = ImVec2(30, 8);
        if (ImGui::BeginTabItem("   AimBot   "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftAimBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  AimBot");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("AimBot", &g.AimBot);
            ImGui::Checkbox("Aim at Team", &g.AimAtTeam);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  AimBot Config");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Visibility Check", &g.VisCheck);
            ImGui::Checkbox("NoSway", &g.NoSway);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  FOV");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Draw FOV", &g.DrawFov);
            ImGui::SliderFloat("FOV", &g.AimFov, 50.f, 500.f);
            ImGui::ColorEdit4("FOV Color", &Col_FOV.Value.x);

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightAimBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  AimBot Setting");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SliderFloat("FOV", &g.AimFov, 25.f, 650.f);
            ImGui::SliderFloat("Smooth", &g.Smooth, 1.f, 50.f);
            ImGui::SliderFloat("Distance", &g.Aim_MaxDistance, 15.f, 300.f);
            ImGui::SliderInt("Prediction", &g.PredictVal, 200, 1200);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  KeyBind");
            ImGui::Separator();
            ImGui::Spacing();

            const char* text1 = KeyNames[g.AimKey0];
            const char* text2 = KeyNames[g.AimKey1];

            if (g.KeyBinding)
            {
                switch (g.BindID)
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

            ImGui::Text("1st :");
            if (ImGui::Button(text1, ImVec2(215.f, 20.f)))
            {
                g.KeyBinding = true;
                g.BindID = 1;
                std::thread([&]() {this->KeyBinder(g.AimKey0); }).detach();
            }

            ImGui::Text("2nd :");
            if (ImGui::Button(text2, ImVec2(215.f, 20.f)))
            {
                g.KeyBinding = true;
                g.BindID = 2;
                std::thread([&]() {this->KeyBinder(g.AimKey1); }).detach();
            }
   
            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(30, 8);
        if (ImGui::BeginTabItem("   Visual   "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftVisualBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  Visual");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Player ESP", &g.ESP);
            ImGui::Checkbox("Dummy ESP", &g.DummyESP);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  ESP Options");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Box", &g.ESP_Box);
            ImGui::Checkbox("Line", &g.ESP_Line);
            ImGui::Checkbox("Distance", &g.ESP_Distance);
            ImGui::Checkbox("HealthBar", &g.ESP_HealthBar);

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightVisualBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  ESP Setting");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SliderFloat("Distance", &g.ESP_MaxDistance, 25, 500);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  ESP Colors");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::ColorEdit4("Normal ", &Col_ESP_Normal.Value.x);
            ImGui::ColorEdit4("Visible", &Col_ESP_Visible.Value.x);
            ImGui::ColorEdit4("Team", &Col_ESP_Team.Value.x);

            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(30, 8);
        if (ImGui::BeginTabItem("    Misc    "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftMiscBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 12.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("// Removed");

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightMiscBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("Name Spoofer");
            ImGui::Separator();
            ImGui::Spacing();
            static char name[16];
            ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

            if (ImGui::Button("Apply"))
                for (int i = 0; i < sizeof(name); i++)
                    m.Write<char>(m.BaseAddress + (0x234F35D8 + i), name[i]);

            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(30, 8);
        if (ImGui::BeginTabItem("  Developer  "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftDevBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("// Removed");

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

// ESP
void Cheat::RenderESP()
{
    // ImGui Window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)g.GameSize.right, (float)g.GameSize.bottom));
    ImGui::Begin("##ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // LocalPlayer
    CBaseEntity LPlayer, * pLocal = &LPlayer;
    uint64_t LocalPlayer = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
    LPlayer = m.Read<CBaseEntity >(LocalPlayer + 0x140);

    // WorldToScreen用
    uint64_t viewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
    uint64_t tmpvmx = m.Read<uint64_t>(viewRenderer + offset::ViewMatrix);
    Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);

    // ESP Main
    for (int i = 0; i < 15000; i++)
    {
        if (!g.DummyESP && i == 128)
            break;

        // Entity and Check
        uint64_t Entity = GetEntityById(m.BaseAddress, i);

        if (Entity == NULL || Entity == LocalPlayer)
            continue;

        CBaseEntity BaseEntity = m.Read<CBaseEntity>(Entity + 0x140), * pEntity = &BaseEntity;

        // Some checks
        if (pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;
        else if (pEntity->m_iTeamNum == 0)
            continue;
        else if (pEntity->m_iHealth <= 0.f || pEntity->m_iHealth == 0.f)
            continue;

        // WorldToScreen
        Vector2 ScreenPosition = Vector2(0.f, 0.f);
        if (!WorldToScreen(pEntity->m_localOrigin, &ViewMatrix._11, (float)g.GameSize.right, (float)g.GameSize.bottom, ScreenPosition))
            continue;

        // W2Sが有効であったらレンダリングする
        if (ScreenPosition == Vector2(0.f, 0.f));
        {
            // Distance
            float distance = ((pEntity->m_localOrigin - pLocal->m_localOrigin).Length() * 0.01905f);

            if (distance < g.ESP_MaxDistance)
            {
                // Head ScreenPosision
                Vector3 HeadPosition = GetEntityBonePosition(Entity, 8, pEntity->m_localOrigin);
                Vector2 ScreenHeadPosition = {};
                if (!WorldToScreen(HeadPosition, &ViewMatrix._11, (float)g.GameSize.right, (float)g.GameSize.bottom, ScreenHeadPosition))
                    continue;

                // Box
                float Height = abs(abs(ScreenHeadPosition.y) - abs(ScreenPosition.y));
                float Width = Height / 2.f;

                // Vis check
                bool visible = pEntity->m_lastvisibletime > 0.f && pEntity->m_lastvisibletime > vInfo[i];
                vInfo[i] = pEntity->m_lastvisibletime;

                // Set ESP Color
                ImVec4 ESP_Color = visible ? Col_ESP_Visible : Col_ESP_Normal;

                // Line
                if (g.ESP_Line)
                    ImGui::GetWindowDrawList()->AddLine(ImVec2((float)g.GameSize.right / 2.f, (float)g.GameSize.bottom), ImVec2(ScreenPosition.x, ScreenPosition.y), (ImColor)ESP_Color, 1.f);

                // ESP Box
                if (g.ESP_Box)
                    DrawBox(ScreenPosition.x - (Width / 2.f), ScreenPosition.y, Width, -Height, ESP_Color);

                // Healthbar
                if (g.ESP_HealthBar)
                {
                    HealthBar((ScreenPosition.x - (Width / 2.f) - 6), ScreenPosition.y, 2, -Height, pEntity->m_iHealth, pEntity->m_iMaxHealth);

                    if (pEntity->m_shieldHealth != 0)
                        ShieldBar((ScreenPosition.x - (Width / 2.f) - 12), ScreenPosition.y, 2, -Height, pEntity->m_shieldHealth, pEntity->m_shieldHealthMax);
                }

                // Distance
                if (g.ESP_Distance)
                {
                    std::string m_text = std::to_string((int)distance) + "m";

                    // テキストの真ん中を取得
                    ImVec2 textSize = ImGui::CalcTextSize(m_text.c_str());
                    float TextCentor = textSize.x / 2.f;
                    String(ImVec2(ScreenPosition.x - TextCentor, ScreenPosition.y), ImColor(1.f, 1.f, 1.f, 1.f), m_text.c_str());
                }
            }
        }
    }

    ImGui::End();
}

const char* KeyNames[] =
{
    "None",
    "Mouse Left",
    "Mouse Right",
    "Cancel",
    "Middle Center",
    "MouseSide1",
    "MouseSide2",
    "",
    "Backspace",
    "Tab",
    "",
    "",
    "Clear",
    "Enter",
    "",
    "",
    "Shift",
    "Ctrl",
    "Alt",
    "Pause",
    "CapsLock",
    "",
    "",
    "",
    "",
    "",
    "",
    "Escape",
    "",
    "",
    "",
    "",
    "Space",
    "Page Up",
    "Page Down",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "",
    "",
    "",
    "Print",
    "Insert",
    "Delete",
    "",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "",
    "",
    "",
    "",
    "",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3",
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "Multiply",
    "Add",
    "",
    "Subtract",
    "Decimal",
    "Divide",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
};