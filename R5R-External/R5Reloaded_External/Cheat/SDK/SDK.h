#pragma once
#include "..\..\Utils\Memory\Memory.h"
#include "..\..\Utils\SimpleMath\SimpleMath.h"
using namespace DirectX::SimpleMath;

// offsets
namespace offset
{
	constexpr auto dwEntityList = 0x1F33F78;
	constexpr auto dwLocalPlayer = 0x22E93F8;
	constexpr auto NameList = 0x023a2950;
	constexpr auto ViewMatrix = 0x1A93D0;
	constexpr auto ViewRender = 0xD4138F0;

	constexpr auto m_vecAbsVelocity = 0x140;
	constexpr auto m_localOrigin = 0x14C;
	constexpr auto m_shieldHealth = 0x170;
	constexpr auto m_shieldHealthMax = 0x174;
	constexpr auto m_iHealth = 0x3E0;
	constexpr auto m_iTeamNum = 0x3F0;
	constexpr auto m_localAngle = 0x428;
	constexpr auto m_iMaxHealth = 0x510;
	constexpr auto m_lifeState = 0x730;
	constexpr auto m_lastvisibletime = 0x1754;
	constexpr auto m_lastcrosshairtime = m_lastvisibletime + 0x8;
	constexpr auto camera_origin = 0x1b68;
	constexpr auto m_SwayAngle = 0x2178;
	constexpr auto m_ViewAngle = 0x2188;

	constexpr auto s_BoneMatrix = 0xEE0;
}

// 1RPMÇ≈ì«Ç›éÊÇÈópÅBÇ∆ÇËÇ†Ç¶Ç∏égÇ¶ÇÍÇŒÇ¢Ç¢
struct CBaseEntity
{
	Vector3 m_vecAbsVelocity;
	Vector3 m_localOrigin;
	char z1[offset::m_shieldHealth - (offset::m_localOrigin + sizeof(Vector3))];
	int m_shieldHealth;
	int m_shieldHealthMax;
	char z2[offset::m_iHealth - (offset::m_shieldHealthMax + sizeof(int))];
	int m_iHealth;
	char z3[offset::m_iTeamNum - (offset::m_iHealth + sizeof(int))];
	int m_iTeamNum;
	char z4[offset::m_localAngle - (offset::m_iTeamNum + sizeof(int))];
	Vector3 m_localAngle;
	char z5[offset::m_iMaxHealth - (offset::m_localAngle + sizeof(Vector3))];
	int m_iMaxHealth;
	char z7[offset::m_lifeState - (offset::m_iMaxHealth + sizeof(int))];
	int m_lifeState;
	char z8[offset::m_lastvisibletime - (offset::m_lifeState + sizeof(int))];
	float m_lastvisibletime;
	char z9[offset::m_lastcrosshairtime - (offset::m_lastvisibletime + sizeof(float))];
	float m_lastcrosshairtime;
	char z10[offset::camera_origin - (offset::m_lastcrosshairtime + sizeof(float))];
	Vector3 camera_origin;
	char z11[offset::m_SwayAngle - (offset::camera_origin + sizeof(Vector3))];
	Vector3 m_SwayAngle;
	Vector3 m_ViewAngle;
};

struct GlowColor
{
	float R, G, B;
};

// w
struct GlowMode
{
	uint8_t a, b, c, d;
};

extern uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent);
extern Vector3 GetPredict(uint64_t Entity, float dis, float BulletSpeed);
extern Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition);
extern void NormalizeAngles(Vector3& angle);
extern Vector3 CalcAngle(const Vector3& src, const Vector3& dst);
extern bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to);