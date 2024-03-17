#include "SDK.h"

void get_class_name(uint64_t entity_ptr, char* out_str)
{
	uint64_t client_networkable_vtable = m.Read<uint64_t>(entity_ptr + 8 * 3);

	uint64_t get_client_class = m.Read<uint64_t>(client_networkable_vtable + 8 * 3);

	uint32_t disp = m.Read<uint32_t>(get_client_class + 3);
	const uint64_t client_class_ptr = get_client_class + disp + 7;

	ClientClass client_class = m.Read<ClientClass>(client_class_ptr);

	m.ReadString(client_class.pNetworkName, out_str, sizeof(out_str));
}

bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to)
{
	float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

	if (w < 0.01f)
		return false;

	to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
	to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float x = targetWidth / 2;
	float y = targetHeight / 2;

	x += 0.5 * to.x * targetWidth + 0.5;
	y -= 0.5 * to.y * targetHeight + 0.5;

	to.x = x;
	to.y = y;

	return true;
}

void NormalizeAngles(Vector3& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

Vector3 CalcAngle(const Vector3& src, const Vector3& dst)
{
	Vector3 angle = Vector3();
	Vector3 delta = Vector3((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	angle.x = atan(delta.z / hyp) * (180.0f / 3.1415926535);
	angle.y = atan(delta.y / delta.x) * (180.0f / 3.1415926535);
	angle.z = 0;
	if (delta.x >= 0.0) angle.y += 180.0f;

	return angle;
}

uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent)
{
	uint64_t EntityList = GameBaseAddr + offset::dwEntityList;
	uint64_t BaseEntity = m.Read<uint64_t>(EntityList + (Ent << 5));
	if (!BaseEntity)
		return 0;
	return BaseEntity;
}

Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition)
{
	uint64_t pBoneArray = m.Read<uint64_t>(ent + offset::s_BoneMatrix);

	Vector3 EntityBone = Vector3();

	EntityBone.x = m.Read<float>(pBoneArray + 0xCC + (BoneId * 0x30)) + BasePosition.x;
	EntityBone.y = m.Read<float>(pBoneArray + 0xDC + (BoneId * 0x30)) + BasePosition.y;
	EntityBone.z = m.Read<float>(pBoneArray + 0xEC + (BoneId * 0x30)) + BasePosition.z;

	return EntityBone;
}

// 基本的に m_latestPrimaryWeapons からデータを取得できないと見た方がいいので、それっぽい値を入れてユーザーに調節させる
Vector3 GetPredict(uint64_t Entity, float dis, float BulletSpeed)
{
	Vector3 predict = Vector3(0.f, 0.f, 0.f);

	float bulletTime = dis / BulletSpeed;
	Vector3 pd = m.Read<Vector3>(Entity + offset::m_vecAbsVelocity);
	predict.x = pd.x * bulletTime;
	predict.y = pd.y * bulletTime;
	predict.z = (142.5f * 0.5f * (bulletTime * bulletTime));

	return predict;
}