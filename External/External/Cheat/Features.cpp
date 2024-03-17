#include "Cheat.h"
#include "SDK.h"

int AimBone = 3;
float vAInfo[15000];

// Update EntityList - 有効なポインタ・エンティティのみを格納
void Cheat::UpdateList()
{
	while (cfg.Run)
	{
		std::vector<EntBox> temp_list;
        this->ViewModelList.clear();
		uint64_t local = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);

		for (int i = 0; i < 15000; i++)
		{
			uint64_t entity = GetEntityById(m.BaseAddress, i);

			if (entity == NULL || entity == local)
				continue;

            EntBox temp;

			// Check
			char pName[64];
            uint64_t client_networkable_vtable = m.Read<uint64_t>(entity + 8 * 3);
            uint64_t get_client_class = m.Read<uint64_t>(client_networkable_vtable + 8 * 3);
            uint32_t disp = m.Read<uint32_t>(get_client_class + 3);
            const uint64_t client_class_ptr = get_client_class + disp + 7;
            ClientClass client_class = m.Read<ClientClass>(client_class_ptr);
            ReadProcessMemory(m.pHandle, (void*)(client_class.pNetworkName), pName, sizeof(pName), nullptr);

            if (strcmp(pName, "CPlayer") == 0 || strcmp(pName, "CAI_BaseNPC") == 0 || strcmp(pName, "CBaseGrenade") == 0)
            {
                temp.ptr  = entity;
                temp.name = pName;
                temp_list.push_back(temp);
            }
                
            else if (strcmp(pName, "CBaseViewModel") == 0)
                ViewModelList.push_back(entity);

            ZeroMemory(pName, sizeof(pName));
		}

		this->newlist = temp_list;
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}
}

// AimBot
void Cheat::AimBot()
{
    while (cfg.Run)
    {
        /*----| SomeChecks |--------------------------------------------------------------------------------*/
        if (!cfg.AimBot)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        else if (!IsKeyDown(cfg.AimKey0) && !IsKeyDown(cfg.AimKey1))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        bool check = false;

        switch (cfg.AimKeyType)
        {
        case 0: // and
            if (cfg.AimKey1 != 0)
                if (!IsKeyDown(cfg.AimKey0) || !IsKeyDown(cfg.AimKey1)) // どちらかが押されていなければ
                    continue;
                else
                    if (!IsKeyDown(cfg.AimKey0))
                        continue;

            check = true;
            break;
        case 1: // or
            if (cfg.AimKey1 != 0)
                if (IsKeyDown(cfg.AimKey0) || IsKeyDown(cfg.AimKey1))
                    check = true;
                else
                    if (!IsKeyDown(cfg.AimKey0))
                        continue;

            check = true;
            break;
        default:
            break;
        }

        if (!check)
            continue;
        /*--------------------------------------------------------------------------------------------------*/

        float fov = 0.f;
        float MinFov = 0.f;
        float MinDistance = 0.f;
        Vector2 ScreenMiddle = { (float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f };

        CBaseEntity local, ent, target;
        CBaseEntity* pLocal = &local;
        CBaseEntity* pEntity = &ent;
        CBaseEntity* pTarget = &target;
        pLocal->ptr = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
        pLocal->Update();

        // LocalPlayer Check
        if (!pLocal->IsAlive())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // WorldToScreen用
        uint64_t ViewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
        uint64_t tmpvmx = m.Read<uint64_t>(ViewRenderer + offset::ViewMatrix);
        Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);

        for (int i = 0; i < newlist.size(); i++)
        {
            // Main
            ent.ptr = newlist[i].ptr;
            pEntity->Update();

            if (newlist[i].name == "CPlayer" || newlist[i].name == "CAI_BaseNPC")
            {
                if (!cfg.DummyESP && newlist[i].name == "CAI_BaseNPC")
                    continue;
                else if (!pEntity->IsAlive())
                    continue;
                else if (!cfg.AimAtTeam && pEntity->Team == pLocal->Team)
                    continue;

                // GetDistance
                float distance = ((pEntity->Position - pLocal->Position).Length() * 0.01905f);

                if (distance < cfg.MaxAimDistance)
                {
                    // Vis check
                    bool visible = pEntity->VisTime > 0.f && pEntity->VisTime > vAInfo[i];
                    vAInfo[i] = pEntity->VisTime;

                    // もうちょい最適化できる
                    for (int i = 0; i < 110; i++)
                    {
                        // Bone BlackList
                        if (i == 4 || i == 46)
                            continue;

                        // Get BonePosition
                        Vector3 BonePosition = GetEntityBonePosition(pEntity->ptr, i, pEntity->Position);

                        float B2Bdistance = ((pEntity->Position - BonePosition).Length() * 0.01905f);
                        if (BonePosition == Vector3(0.f, 0.f, 0.f) || BonePosition == pEntity->Position || B2Bdistance > 2.f)
                            continue;

                        // W2S
                        Vector2 ScreenBonePosition = Vector2(0.f, 0.f);
                        if (!WorldToScreen(BonePosition, &ViewMatrix._11, (float)cfg.GameRect.right, (float)cfg.GameRect.bottom, ScreenBonePosition))
                            continue;

                        fov = abs((ScreenMiddle - ScreenBonePosition).Length());

                        // AimFovの内側に敵がいるか - いたらループを終了、Angleに書き込む
                        if (fov < cfg.AimFov)
                        {
                            // ...
                            switch (cfg.AimType)
                            {
                            case 0: // Crosshair
                                if (cfg.VisCheck)
                                {
                                    if (visible)
                                    {
                                        if (MinFov == 0.f)
                                        {
                                            target = ent;
                                            MinFov = fov;
                                        }
                                        else if (MinFov > fov)
                                        {
                                            target = ent;
                                            MinFov = fov;
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                else
                                {
                                    if (MinFov == 0.f)
                                    {
                                        target = ent;
                                        MinFov = fov;
                                    }
                                    else if (MinFov > fov)
                                    {
                                        target = ent;
                                        MinFov = fov;
                                    }

                                    break;
                                }
                                break;
                            case 1: // Game Distance
                                if (cfg.VisCheck)
                                {
                                    if (visible)
                                    {
                                        if (MinDistance == 0.f)
                                        {
                                            target = ent;
                                            MinDistance = distance;
                                        }
                                        else if (MinDistance > distance)
                                        {
                                            target = ent;
                                            MinDistance = distance;
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                else
                                {
                                    if (MinDistance == 0.f)
                                    {
                                        target = ent;
                                        MinDistance = distance;
                                    }
                                    else if (MinDistance > distance)
                                    {
                                        target = ent;
                                        MinDistance = distance;
                                    }

                                    break;
                                }
                                break;
                            default:
                                break;
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }
        }

        // Write Angle
        if (target.ptr != NULL)
        {
            // SomeChecks
            if (GetForegroundWindow() != game)
                continue;

            // Set AimBot
            switch (cfg.AimTargetBone)
            {
            case 0:
                AimBone = 8;
                break;
            case 1:
                AimBone = 3;
                break;
            default:
                AimBone = 3;
                break;
            }

            if (!pTarget->IsAlive())
                continue;

            pLocal->AimUpdate();
            pTarget->AimUpdate();

            // Get TargetBone Position
            Vector3 TargetBone = GetEntityBonePosition(pTarget->ptr, AimBone, pTarget->Position); // Head / Neck

            if (TargetBone == Vector3(0.f, 0.f, 0.f))
                continue;

            // Prediction
            float distance = ((pTarget->Position - pLocal->Position).Length() * 0.01905f);
            TargetBone += GetPredict(pTarget->ptr, distance, cfg.PredictVal);

            // WorldToScreen
            Vector2 ScreenPosition = {};
            if (!WorldToScreen(pTarget->Position, &ViewMatrix._11, (float)cfg.GameRect.right, (float)cfg.GameRect.bottom, ScreenPosition))
                continue;

            if (ScreenPosition != Vector2(0.f, 0.f))
            {
                // Angle
                Vector3 AimAngle = CalcAngle(pLocal->CameraPosition, TargetBone);
                Vector3 TmpViewAngle = pLocal->ViewAngle;
                Vector3 Delta = (AimAngle - TmpViewAngle);

                // NoRecoil, NoSway (Aiming Only
                if (cfg.NoSway)
                {
                    Vector3 Breath = pLocal->SwayAngle - TmpViewAngle;

                    if (Breath.x != 0.f || Breath.y != 0.f)
                        Delta = (AimAngle - TmpViewAngle) - Breath;
                }

                NormalizeAngles(Delta);
                Vector3 SmoothedAngle = TmpViewAngle + Delta / cfg.Smooth;
                NormalizeAngles(SmoothedAngle);

                if (SmoothedAngle.x != 0.f || SmoothedAngle.y != 0.f)
                {
                    // Write Angle
                    m.Write<Vector3>(pLocal->ptr + 0x2188, SmoothedAngle);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(150));
    }
}

void Cheat::Misc()
{
    Vector3 OldPunch = Vector3(0.f, 0.f, 0.f);

    while (cfg.Run)
    {
        // ViewModel Glow
        if (cfg.ViewModelGlow)
        {
            ImColor vm_col = cfg.RainbowViewModel ? Rainbow : ViewModel_User;
            
            for (int i = 0; i < ViewModelList.size(); i++)
            {
                m.Write<int>(ViewModelList[i] + 0x310, 1);
                m.Write<int>(ViewModelList[i] + 0x320, 2);
                m.Write<GlowMode>(ViewModelList[i] + 0x27C, { 109, 6, 105, 0 });
                m.Write<GlowColor>(ViewModelList[i] + 0x1D0, { vm_col.Value.x, vm_col.Value.y, vm_col.Value.z });
            }
        }
        else if (!cfg.ViewModelGlow)
        {
            for (int j = 0; j < ViewModelList.size(); j++)
            {
                if (m.Read<int>(ViewModelList[j] + 0x310) == 0)
                    continue;

                m.Write<int>(ViewModelList[j] + 0x310, 0);
                m.Write<int>(ViewModelList[j] + 0x320, 0);
            }
        }

        if (cfg.NoRecoil)
        {
            uint64_t local = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
            if (!local)
                continue;

            Vector3 viewAngle = m.Read<Vector3>(local + offset::m_ViewAngle);
            Vector3 punchAngle = m.Read<Vector3>(local + offset::m_vecPunchWeapon_Angle);
            Vector3 NewAngle = viewAngle + (OldPunch - punchAngle) * cfg.NoRecoilVal;

            NormalizeAngles(NewAngle);

            if (IsKeyDown(VK_LBUTTON))
                m.Write<Vector3>(local + offset::m_ViewAngle, NewAngle);

            OldPunch = punchAngle;
        }

        if (cfg.Rapidfire)
        {
            // TriggerBot and more
            if (cfg.Rapidfire && IsKeyDown(VK_LBUTTON))
            {
                if (GetForegroundWindow() == this->game)
                {
                    m.Write<uint32_t>(m.BaseAddress + 0xd4150a8 + 0x8, 4);
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    m.Write<uint32_t>(m.BaseAddress + 0xd4150a8 + 0x8, 5);
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    m.Write<uint32_t>(m.BaseAddress + 0xd4150a8 + 0x8, 4);
                }
                else
                {
                    continue;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(500));
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
    if (cfg.AimKey0 == cfg.AimKey1)
        cfg.AimKey1 = 0;

    cfg.KeyBinding = false;
    cfg.BindingID = 0;
}

void Cheat::RainbowGenerator()
{
    while (cfg.Run)
    {
        static float rainbow;
        rainbow += 0.01f;
        if (rainbow > 1.f)
            rainbow = 0.f;

        constexpr float factor = 6.0f;
        int i = static_cast<int>(rainbow * factor);
        float f = rainbow * factor - i;
        float q = 1 - f;
        float t = 1 - (1 - f);

        Rainbow.Value.x = (i % 6 == 0) ? 1.f : (i % 6 == 1) ? q : (i % 6 == 2) ? 0 : (i % 6 == 3) ? 0 : (i % 6 == 4) ? t : 1.f;
        Rainbow.Value.y = (i % 6 == 0) ? t : (i % 6 == 1) ? 1.f : (i % 6 == 2) ? 1.f : (i % 6 == 3) ? q : (i % 6 == 4) ? 0 : 0;
        Rainbow.Value.z = (i % 6 == 0) ? 0 : (i % 6 == 1) ? 0 : (i % 6 == 2) ? t : (i % 6 == 3) ? 1.f : (i % 6 == 4) ? 1.f : q;

        std::this_thread::sleep_for(std::chrono::milliseconds(cfg.RainbowRate));
    }
}