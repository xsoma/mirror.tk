#include "UTIL Functions.h"
#include "Utilities.h"
#include "Menu.h"
#include "Hacks.h"
#include "Autowall.h"
#include "experimental.h"
#include "RenderManager.h"

ServerRankRevealAllFn game_utils::ServerRankRevealAllEx;

DWORD game_utils::FindPattern1(std::string moduleName, std::string pattern)
{
	const char* pat = pattern.c_str();
	DWORD firstMatch = 0;
	DWORD rangeStart = (DWORD)GetModuleHandleA(moduleName.c_str());
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else
				pat += 2; 
		}
		else
		{
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}
	return NULL;
}

DWORD game_utils::find_pattern_xy0(const char* module_name, const BYTE* mask, const char* mask_string)
{
	/// Get module address
	const unsigned int module_address = reinterpret_cast<unsigned int>(GetModuleHandle(module_name));

	/// Get module information to the size
	MODULEINFO module_info;
	GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(module_address), &module_info, sizeof(MODULEINFO));

	auto IsCorrectMask = [](const unsigned char* data, const unsigned char* mask, const char* mask_string) -> bool
	{
		for (; *mask_string; ++mask_string, ++mask, ++data)
			if (*mask_string == 'x' && *mask != *data)
				return false;

		return (*mask_string) == 0;
	};

	/// Iterate until we find a matching mask
	for (unsigned int c = 0; c < module_info.SizeOfImage; c += 1)
	{
		/// does it match?
		if (IsCorrectMask(reinterpret_cast<unsigned char*>(module_address + c), mask, mask_string))
			return (module_address + c);
	}

	return 0;
}

std::uint8_t* game_utils::pattern_scan(void* module, const char* signature)
{
	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector< int >{};
		auto start = const_cast< char* >(pattern);
		auto end = const_cast< char* >(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto dosHeader = (PIMAGE_DOS_HEADER)module;
	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = pattern_to_byte(signature);
	auto scanBytes = reinterpret_cast< std::uint8_t* >(module);

	auto s = patternBytes.size();
	auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
		bool found = true;
		for (auto j = 0ul; j < s; ++j) {
			if (scanBytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found) {
			return &scanBytes[i];
		}
	}

	// Afterwards call server to stop dispatch of cheat and to alert us of update.
	//ConsolePrint(true, "A pattern has outtdated: %s", signature);
	return nullptr;
}


void UTIL_TraceLine(const vector& vecAbsStart, const vector& vecAbsEnd, unsigned int mask,const IClientEntity *ignore, int collisionGroup, trace_t *ptr)
{
	typedef int(__fastcall* UTIL_TraceLine_t)(const vector&, const vector&, unsigned int, const IClientEntity*, int, trace_t*);
	static UTIL_TraceLine_t TraceLine = (UTIL_TraceLine_t)Utilities::Memory::FindPatternV2("client.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52");
	TraceLine(vecAbsStart, vecAbsEnd, mask, ignore, collisionGroup, ptr);
}
/*
void UTIL_trace(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, const IClientEntity *ignore, int collisionGroup, trace_t *ptr)
{
	typedef int(__fastcall* UTIL_TraceLine_t)(const Vector&, const Vector&, unsigned int, const IClientEntity*, int, trace_t*);
	static UTIL_TraceLine_t TraceLine = (UTIL_TraceLine_t)Utilities::Memory::FindPatternV2("client_panorama.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52");
	TraceLine(vecAbsStart, vecAbsEnd, mask, ignore, collisionGroup, ptr);
}
*/
void ClipToPlayer(const vector& vecAbsStart, const vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr)
{
	static DWORD dwAddress = Utilities::Memory::FindPattern("client.dll", (BYTE*)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x10", "xxxxxxxxxxxxxxxxxxxxxxxx????xxx");
	if (!dwAddress)
		return; 
	_asm
	{
		MOV		EAX, filter
		LEA		ECX, tr
		PUSH	ECX
		PUSH	EAX
		PUSH	mask
		LEA		EDX, vecAbsEnd
		LEA		ECX, vecAbsStart
		CALL	dwAddress
		ADD		ESP, 0xC
	}
}
void UTIL_ClipTraceToPlayers(const vector& vecAbsStart, const vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr)
{

		static DWORD dwAddress = Utilities::Memory::FindPatternV2("client.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10");

		if (!dwAddress)
			return;

		_asm
		{
			MOV		EAX, filter
			LEA		ECX, tr
			PUSH	ECX
			PUSH	EAX
			PUSH	mask
			LEA		EDX, vecAbsEnd
			LEA		ECX, vecAbsStart
			CALL	dwAddress
			ADD		ESP, 0xC
		}
	
}

bool IsBreakableEntity(IClientEntity* ent)
{
	typedef bool(__thiscall* IsBreakbaleEntity_t)(IClientEntity*);
	IsBreakbaleEntity_t IsBreakbaleEntityFn = (IsBreakbaleEntity_t)Utilities::Memory::FindPatternV2("client.dll", "55 8B EC 51 56 8B F1 85 F6 74 68");
	if (IsBreakbaleEntityFn)
		return IsBreakbaleEntityFn(ent);
	else
		return false;
}

bool TraceToExit(vector& end, trace_t& tr, vector start, vector vEnd, trace_t* trace)
{
	typedef bool(__fastcall* TraceToExitFn)(vector&, trace_t&, float, float, float, float, float, float, trace_t*);
	static TraceToExitFn TraceToExit = (TraceToExitFn)Utilities::Memory::FindPatternV2(("client.dll"), ("55 8B EC 83 EC 30 F3 0F 10 75"));

	if (!TraceToExit)
		return false;

	return TraceToExit(end, tr, start.x, start.y, start.z, vEnd.x, vEnd.y, vEnd.z, trace);
}

bool trace_to_exit(vector& end, trace_t& tr, vector start, vector vEnd, trace_t* trace)
{
	typedef bool(__fastcall* TraceToExitFn)(vector&, trace_t&, float, float, float, float, float, float, trace_t*);
	static TraceToExitFn TraceToExit = (TraceToExitFn)Utilities::Memory::FindPatternV2(("client.dll"), ("55 8B EC 83 EC 30 F3 0F 10 75"));

	if (!TraceToExit)
		return false;

	return TraceToExit(end, tr, start.x, start.y, start.z, vEnd.x, vEnd.y, vEnd.z, trace);
}

void game_utils::NormaliseViewAngle(vector &angle)
{
	
		while (angle.y <= -180) angle.y += 360;
		while (angle.y > 180) angle.y -= 360;
		while (angle.x <= -180) angle.x += 360;
		while (angle.x > 180) angle.x -= 360;


		if (angle.x > 89) angle.x = 89;
		if (angle.x < -89) angle.x = -89;
		if (angle.y < -180) angle.y = -179.999;
		if (angle.y > 180) angle.y = 179.999;

		angle.z = 0;
	
}

void game_utils::CL_FixMove(CUserCmd* pCmd, vector viewangles)
{




}

char shit[16];
trace_t Trace;
char shit2[16];
IClientEntity* entCopy;

bool game_utils::is_visible(IClientEntity* pLocal, IClientEntity* pEntity, int BoneID)
{
	return false;
}

bool screen_transform(const vector& point, vector& screen)
{
	const matrix3x4& w2sMatrix = interfaces::engine->WorldToScreenMatrix();
	screen.x = w2sMatrix[0][0] * point.x + w2sMatrix[0][1] * point.y + w2sMatrix[0][2] * point.z + w2sMatrix[0][3];
	screen.y = w2sMatrix[1][0] * point.x + w2sMatrix[1][1] * point.y + w2sMatrix[1][2] * point.z + w2sMatrix[1][3];
	screen.z = 0.0f;

	float w = w2sMatrix[3][0] * point.x + w2sMatrix[3][1] * point.y + w2sMatrix[3][2] * point.z + w2sMatrix[3][3];

	if (w < 0.001f) {
		screen.x *= 100000;
		screen.y *= 100000;
		return true;
	}

	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	return false;
}
bool game_utils::World2Screen(const vector & origin, vector & screen)
{
	if (!screen_transform(origin, screen)) {
		int iScreenWidth, iScreenHeight;
		interfaces::engine->GetScreenSize(iScreenWidth, iScreenHeight);
		screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
		screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;

		return true;
	}
	return false;
}

bool game_utils::IsBomb(void* weapon)
{
	if (weapon == nullptr) return false;
	IClientEntity* weaponEnt = (IClientEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)CSGOClassID::CC4)
		return true;
	else
		return false;
}

bool game_utils::IsGrenade(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_FLASHBANG,WEAPON_HEGRENADE,WEAPON_SMOKEGRENADE,WEAPON_MOLOTOV,WEAPON_DECOY,WEAPON_INC };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool game_utils::IsRevolver(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_REVOLVER };
	return (std::find(v.begin(), v.end(), id) != v.end());
}


void vector_transform(const vector in1, float in2[3][4], vector &out)
{
	out[0] = DotProduct(in1, vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}

bool game_utils::IsPistol(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_DEAGLE,WEAPON_CZ75,WEAPON_ELITE,WEAPON_USPS,WEAPON_P250,WEAPON_P2000, WEAPON_TEC9,WEAPON_REVOLVER,WEAPON_FIVESEVEN,WEAPON_GLOCK };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool game_utils::IsKnife(void * weapon)
{
	IClientEntity* weaponEnt = (IClientEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)CSGOClassID::CKnife)
		return true;
	else
		return false;
} //WEAPON_KNIFE_BAYONET


bool game_utils::AutoSniper(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_G3SG1,WEAPON_SCAR20 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool game_utils::IsSniper(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_AWP,WEAPON_SSG08,WEAPON_G3SG1,WEAPON_SCAR20 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}


bool game_utils::LightSniper(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_AWP,WEAPON_SSG08 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool game_utils::IsRifle(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_M4A4, WEAPON_AK47, WEAPON_AUG,  WEAPON_FAMAS,  WEAPON_GALIL, WEAPON_M4A1S, WEAPON_SG553 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}


bool game_utils::IsShotgun(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_NOVA, WEAPON_SAWEDOFF, WEAPON_XM1014, WEAPON_MAG7 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool game_utils::IsMachinegun(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_NEGEV, WEAPON_M249 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool game_utils::IsMP(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_MP5SD, WEAPON_BIZON, WEAPON_P90, WEAPON_MAC10, WEAPON_MP7, WEAPON_MP9, WEAPON_UMP45 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool game_utils::AWP(void* weapon)
{
	if (weapon == nullptr) return false;
	IClientEntity* weaponEnt = (IClientEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)CSGOClassID::CWeaponAWP)
		return true;
	else
		return false;
}

bool game_utils::IsZeus(void* weapon)
{
	if (weapon == nullptr) return false;
	IClientEntity* weaponEnt = (IClientEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)CSGOClassID::CWeaponTaser)
		return true;
	else
		return false;
}

bool game_utils::IsScout(void* weapon)
{
	if (weapon == nullptr) return false;
	IClientEntity* weaponEnt = (IClientEntity*)weapon;
	ClientClass* pWeaponClass = weaponEnt->GetClientClass();

	if (pWeaponClass->m_ClassID == (int)CSGOClassID::CWeaponSSG08)
		return true;
	else
		return false;
}
bool game_utils::IsScopedWeapon(void* weapon)
{
	if (weapon == nullptr) return false;
	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_AWP,WEAPON_SSG08,WEAPON_G3SG1,WEAPON_SCAR20, WEAPON_AUG, WEAPON_SG553 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

void SayInChat(const char *text)
{
	char buffer[250];
	sprintf_s(buffer, "say \"%s\"", text);
	interfaces::engine->ClientCmd_Unrestricted(buffer);
}

QAngle CalcAngleA(vector src, vector dst)
{
	QAngle angles;
	vector delta = src - dst;
	VectorAngles(delta, angles);
	angles.NormalizeX();
	return angles;
}
void AngleVectorsA(const QAngle &angles, vector& forward)
{
	float	sp, sy, cp, cy;

	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}
vector MultipointVectors[] = { vector(0,0,0), vector(0,0,1.5),vector(0,0,3),vector(0,0,4), vector(0,0,-2), vector(0,0,-4), vector(0,0,4.8), vector(0,0,5), vector(0,0,5.4), vector(0,3,0), vector(3,0,0),vector(-3,0,0),vector(0,-3,0), vector(0,2,4.2), vector(0,-2,4.2), vector(2,0,4.2), vector(-2,0,4.2),  vector(3.8,0,0), vector(-3.8,0,0),vector(0,3.6,0), vector(0,-3.6,0), vector(0,1.2,3.2), vector(0,0.6,1.4), vector(0,3.1,5.1), vector(0,0,6.2), vector(0,2.5,0), vector(2.1,2.1,2.1) };


vector hitbox_location(IClientEntity* obj, int hitbox_id)
{
	matrix3x4 bone_matrix[128];

	if (obj->SetupBones(bone_matrix, 128, 0x00000100, 0.0f)) {
		if (obj->GetModel()) {
			auto studio_model = interfaces::model_info->GetStudiomodel(obj->GetModel());
			if (studio_model) {
				auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
				if (hitbox) {
					auto min = vector{}, max = vector{};

					VectorTransform(hitbox->bbmin, bone_matrix[hitbox->bone], min);
					VectorTransform(hitbox->bbmax, bone_matrix[hitbox->bone], max);

					return (min + max) / 2.0f;
				}
			}
		}
	}
	return vector{};
}
vector GetHitboxPosition(IClientEntity* obj, int Hitbox, float this_sim)
{
	matrix3x4 bone_matrix[128];

	if (obj->SetupBones(bone_matrix, 128, 0x00000100, this_sim)) {
		if (obj->GetModel())
		{
			auto studio_model = interfaces::model_info->GetStudiomodel(obj->GetModel());
			if (studio_model)
			{
				auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(Hitbox);
				vector Point[] =
				{
					vector(hitbox->bbmin.x, hitbox->bbmin.y, hitbox->bbmin.z),
					vector(hitbox->bbmin.x, hitbox->bbmax.y, hitbox->bbmin.z),
					vector(hitbox->bbmax.x, hitbox->bbmax.y, hitbox->bbmin.z),
					vector(hitbox->bbmax.x, hitbox->bbmin.y, hitbox->bbmin.z),
					vector(hitbox->bbmax.x, hitbox->bbmax.y, hitbox->bbmax.z),
					vector(hitbox->bbmin.x, hitbox->bbmax.y, hitbox->bbmax.z),
					vector(hitbox->bbmin.x, hitbox->bbmin.y, hitbox->bbmax.z),
					vector(hitbox->bbmax.x, hitbox->bbmin.y, hitbox->bbmax.z)
				};
				vector vMin, vMax, vCenter, sCenter;

				int head = options::menu.aimbot.Multival2.GetValue();
				int chest = options::menu.aimbot.Multival4.GetValue();
				int body = options::menu.aimbot.Multival.GetValue();
				int legs = options::menu.aimbot.MultiVal3.GetValue();

				VectorTransform(hitbox->bbmin, bone_matrix[hitbox->bone], vMin);
				VectorTransform(hitbox->bbmax, bone_matrix[hitbox->bone], vMax);

				vCenter = ((vMin + vMax) *0.5f);
				int iCount = 7;

				for (int i = 0; i <= iCount; i++)
				{
					vector vTargetPos;
					switch (i)
					{
					case 0:
					default:
						vTargetPos = vCenter; break;
						
					case 1:
					{
						vTargetPos = (Point[7] + Point[1]) * (head / 100);
					}
					break;

					case 2:
					{
						vTargetPos = (Point[3] + Point[4]) * (chest / 100);
					}
					break;

					case 3:
					{
						vTargetPos = (Point[4] + Point[0]) * (body / 100);
					}
					break;

					case 4:
					{
						vTargetPos = (Point[2] + Point[7]) * (35 / 100);
					}
					break; 

					case 5:
					{
						vTargetPos = (Point[4] + Point[0]) * (legs / 100);
					}
					break;
					case 6:
						vTargetPos = (Point[5] + Point[3]) * 0.2; 
						break;
					case 7:
						vTargetPos = (Point[1] + Point[2]) * 0.2; 
						break;
				//	default: vTargetPos = vCenter;
					}
					return vTargetPos;
				}

			}

		}
	}
	return vector{};
}



vector GetEyePosition(IClientEntity* pEntity)
{
	vector vecViewOffset = *reinterpret_cast<vector*>(reinterpret_cast<int>(pEntity) + 0x104);

	return pEntity->GetOrigin() + vecViewOffset;
}

int game_utils::GetPlayerCompRank(IClientEntity* pEntity)
{
	DWORD m_iCompetitiveRanking = NetVar.GetNetVar(0x75671F7F); 
	DWORD GameResources = *(DWORD*)(Utilities::Memory::FindPatternV2("client.dll", "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7") + 0x2);
	
	return *(int*)((DWORD)GameResources + 0x1A44 + (int)pEntity->GetIndex() * 4);
}

extern void game_utils::ServerRankRevealAll()
{
	static float fArray[3] = { 0.f, 0.f, 0.f };

	game_utils::ServerRankRevealAllEx = (ServerRankRevealAllFn)(Offsets::Functions::dwGetPlayerCompRank);
	//GameUtils::ServerRankRevealAllEx = (ServerRankRevealAllFn)(offsets.ServerRankRevealAllEx);
	game_utils::ServerRankRevealAllEx(fArray);
}


