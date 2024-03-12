#pragma once
#include "..\Utils\SimpleMath\SimpleMath.h"
#include "..\Utils\Memory\Memory.h"
#include <string>
#include <cstdint>
#include <vector>
using namespace DirectX::SimpleMath;

// offsets
namespace offset
{
	// Main
	constexpr auto dwEntityList = 0x1F33F78;
	constexpr auto dwLocalPlayer = 0x22E93F8;
	constexpr auto ViewMatrix = 0x1A93D0;
	constexpr auto ViewRender = 0xD4138F0;

	// Player
	constexpr auto m_vecAbsVelocity = 0x140;	// Vector3
	constexpr auto m_localOrigin = 0x14C;	// Vector3
	constexpr auto m_shieldHealth = 0x170;	// int
	constexpr auto m_shieldHealthMax = 0x174;	// int
	constexpr auto m_iHealth = 0x3E0;	// int
	constexpr auto m_iTeamNum = 0x3F0;	// int
	constexpr auto m_localAngle = 0x428;	// Vector3
	constexpr auto m_iMaxHealth = 0x510;	// int
	constexpr auto m_lifeState = 0x730;	// int
	constexpr auto m_lastvisibletime = 0x1754;		// float
	constexpr auto m_lastcrosshairtime = m_lastvisibletime + 0x8;		// float
	constexpr auto camera_origin = 0x1b68;			// Vector3
	constexpr auto m_SwayAngle = 0x2178;
	constexpr auto m_ViewAngle = 0x2188;

	// Misc
	constexpr auto s_BoneMatrix = 0xEE0;
	constexpr auto m_nSkin = 0xe48;

	// NoRecoil
	constexpr auto m_vecPunchWeapon_Angle = 0x20bc;
}

class CBaseEntity
{
public:
	uint64_t ptr;

	int Team;
	int Health;
	int MaxHealth;
	int Shield;
	int MaxShield;
	float VisTime;
	float CrossTime;
	Vector3 Position;
	Vector3 AbsVelocity;
	Vector3 CameraPosition;
	Vector3 SwayAngle;
	Vector3 ViewAngle;

	bool IsAlive()
	{
		return Position != Vector3(0.f, 0.f, 0.f) && Health > 0;
	}
	bool Update()
	{
		Position = m.Read<Vector3>(ptr + offset::m_localOrigin);
		Health = m.Read<int>(ptr + offset::m_iHealth);
		MaxHealth = m.Read<int>(ptr + offset::m_iMaxHealth);
		Shield = m.Read<int>(ptr + offset::m_shieldHealth);
		MaxShield = m.Read<int>(ptr + offset::m_shieldHealthMax);
		Team = m.Read<int>(ptr + offset::m_iTeamNum);
		VisTime = m.Read<float>(ptr + offset::m_lastvisibletime);

		return true;
	}
	void AimUpdate()
	{
		AbsVelocity = m.Read<Vector3>(ptr + offset::m_vecAbsVelocity);
		CameraPosition = m.Read<Vector3>(ptr + offset::camera_origin);

		SwayAngle = m.Read<Vector3>(ptr + 0x2178);
		ViewAngle = m.Read<Vector3>(ptr + 0x2188);
	}
	void EnableGlow()
	{
		m.Write<int>(ptr + 0x310, 1);
		m.Write<int>(ptr + 0x320, 2);
	}
	void DisableGlow()
	{
		m.Write<int>(ptr + 0x310, 0);
		m.Write<int>(ptr + 0x320, 0);
	}
};

struct GlowColor
{
	float R, G, B;
};

struct GlowMode
{
	int8_t GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};

struct ClientClass
{
	uint64_t pCreateFn;
	uint64_t pCreateEventFn;
	uint64_t pNetworkName;
	uint64_t pRecvTable;
	uint64_t pNext;
	uint32_t ClassID;
	uint32_t ClassSize;
};

extern void get_class_name(uint64_t entity_ptr, char* out_str);
extern bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to);
extern void NormalizeAngles(Vector3& angle);
extern Vector3 CalcAngle(const Vector3& src, const Vector3& dst);
extern uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent);
extern Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition);
extern Vector3 GetPredict(uint64_t Entity, float dis, float BulletSpeed);