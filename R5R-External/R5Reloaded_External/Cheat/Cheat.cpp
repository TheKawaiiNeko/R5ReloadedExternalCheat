#include "Cheat.h"

float vAInfo[15000];

void Cheat::AimBot()
{
    while (g.Run)
    {
        // AimBotが無効だったら
        if (!g.AimBot)
        {
            Sleep(100);
            continue;
        }
        else if (!IsKeyDown(g.AimKey0) && !IsKeyDown(g.AimKey1))
        {
            Sleep(10);
            continue;
        }

        // LocalPlayer
        CBaseEntity LPlayer, * pLocal = &LPlayer;
        uint64_t LocalPlayer = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
        LPlayer = m.Read<CBaseEntity >(LocalPlayer + 0x140);

        // WorldToScreen用
        uint64_t viewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
        uint64_t tmpvmx = m.Read<uint64_t>(viewRenderer + offset::ViewMatrix);
        Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);

        // AimBot用
        float fov = 0.f;
        uint64_t TargetEntity = 0;
        Vector2 ScreenMiddle = { (float)g.GameSize.right / 2.f, (float)g.GameSize.bottom / 2.f };

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

            // Get distance
            float distance = ((pEntity->m_localOrigin - pLocal->m_localOrigin).Length() * 0.01905f);

            if (distance < g.Aim_MaxDistance)
            {
                // Fovの中に対象がいるかをチェック
                for (int i = 0; i < 110; i++) // -> マジで効率悪いだろこれ
                {
                    // Bone BlackList - BoneベースじゃなくてHitboxベースがいい。絶対に。
                    if (i == 4 || i == 46)
                        continue;

                    // 110回Boneを取得していずれかがFOV内だったらAngleに書き込む
                    Vector2 TargetBone = Vector2(0.f, 0.f);
                    Vector3 BodyPosition = GetEntityBonePosition(Entity, i, pEntity->m_localOrigin);
                    if (BodyPosition == Vector3(0.f, 0.f, 0.f))
                        continue;

                    Vector2 ScreenBodyPosition = {};
                    WorldToScreen(BodyPosition, &ViewMatrix._11, (float)g.GameSize.right, (float)g.GameSize.bottom, TargetBone);
                    fov = abs((ScreenMiddle - TargetBone).Length());

                    // AimFovの内側に敵がいたら
                    if (fov < g.AimFov)
                    {
                        // Vis check
                        bool visible = pEntity->m_lastvisibletime > 0.f && pEntity->m_lastvisibletime > vAInfo[i];
                        vAInfo[i] = pEntity->m_lastvisibletime;

                        if (g.VisCheck && !visible)
                        {
                            TargetEntity = NULL;
                            break;
                        }
                            
                        TargetEntity = Entity;
                        break;
                    }
                }
            }
        }

        // AimBot READY!
        if (TargetEntity != NULL)
        {
            // TargetEntity
            CBaseEntity ent, *Target = &ent;
            ent = m.Read<CBaseEntity>(TargetEntity + 0x140);

            // Get Head(Camera)Position

            Vector3 TargetBone = GetEntityBonePosition(TargetEntity, 3, Target->m_localOrigin);

            // Prediction
            float distance = ((Target->m_localOrigin - pLocal->m_localOrigin).Length() * 0.01905f);
            TargetBone += GetPredict(TargetEntity, distance, g.PredictVal);

            // W2S
            uint64_t viewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
            uint64_t tmpvmx = m.Read<uint64_t>(viewRenderer + offset::ViewMatrix);
            Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);
            Vector2 ScreenPosition = {};
            Vector3 EntityOrigin = m.Read<Vector3>(TargetEntity + offset::m_localOrigin);
            if (!WorldToScreen(EntityOrigin, &ViewMatrix._11, (float)g.GameSize.right, (float)g.GameSize.bottom, ScreenPosition))
                continue;

            // W2Sに失敗していなかったら
            if (ScreenPosition != Vector2(0.f, 0.f))
            {
                // Angle
                Vector3 AimAngle = CalcAngle(pLocal->camera_origin, TargetBone);
                Vector3 TempViewAngles = m.Read<Vector3>(LocalPlayer + 0x2188);
                Vector3 Delta = (AimAngle - TempViewAngles);

                // NoRecoil, NoSway (Aiming Only
                if (g.NoSway)
                {
                    Vector3 SwayAngle = m.Read<Vector3>(LocalPlayer + 0x2178);
                    Vector3 Breath = SwayAngle - TempViewAngles;

                    if (Breath.x != 0.f || Breath.y != 0.f)
                        Delta = (AimAngle - TempViewAngles) - Breath;
                }

                NormalizeAngles(Delta);
                Vector3 SmoothedAngle = TempViewAngles + Delta / g.Smooth;
                NormalizeAngles(SmoothedAngle);

                if (SmoothedAngle.x != 0.f || SmoothedAngle.y != 0.f)
                {
                    // Write Angle
                    m.Write<Vector3>(LocalPlayer + 0x2188, SmoothedAngle);
                }
            }
        }

        if (!g.DummyESP)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Cheat::KeyBinder(int &target_key)
{
    // チェック用変数
    bool flag = false;

    // KeyBinder
    while (true)
    {
        for (int i = 0; i < 0x87; i++)
        {
            if (i == VK_LWIN || i == VK_RWIN)
                continue;

            if (IsKeyDown(i))
            {
                if (i == VK_ESCAPE)
                {
                    target_key = NULL;
                    flag = true;
                }
                else
                {
                    target_key = i;
                    flag = true;
                }

                break;
            }
        }

        if (flag)
            break;
    }

    // Check
    if (g.AimKey0 == g.AimKey1)
        g.AimKey1 = 0;

    g.KeyBinding = false;
    g.BindID = 0;
}