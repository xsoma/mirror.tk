#include "Entities.h"
#include "Offsets.h"
#include "Interfaces.h"
#include "Hooks.h"
 float intervalPerTick;
#define TICK_INTERVAL			(  )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / intervalPerTick ) )


C_BaseCombatWeapon* IClientEntity::weapon()
{
	static int iOffset = GET_NETVAR("DT_BaseCombatCharacter", "m_hActiveWeapon");
	ULONG pWeepEhandle = *(PULONG)((DWORD)this + iOffset);
	return (C_BaseCombatWeapon*)(interfaces::ent_list->GetClientEntityFromHandle((HANDLE)pWeepEhandle));
}

const vector& IClientEntity::world_space_center() {
	vector vecOrigin = GetVecOrigin_2();

	vector min = this->GetCollideable()->OBBMins() + vecOrigin;
	vector max = this->GetCollideable()->OBBMaxs() + vecOrigin;

	vector size = max - min;
	size /= 2.f;
	size += min;

	return size;
}

bool IClientEntity::physics_run_think(int smthn) {
	static auto impl_PhysicsRunThink = reinterpret_cast< bool(__thiscall *)(void*, int) >(
		game_utils::pattern_scan(GetModuleHandle("client.dll"),
			"55 8B EC 83 EC 10 53 56 57 8B F9 8B 87 ? ? ? ? C1 E8 16")
		);

	return impl_PhysicsRunThink(this, smthn);
}

float IClientEntity::spawntime() {
	return *(float_t*)((uintptr_t)this + 0xA360);
}

float IClientEntity::FireRate()
{
	if (!this)
		return 0.f;
	if (!this->IsAlive())
		return 0.f;
	if (this->IsKnifeorNade())
		return 0.f;

	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)this->weapon();

	if (!pWeapon)
		return false;

	std::string WeaponName = pWeapon->GetName();

	if (WeaponName == "weapon_glock")
		return 0.15f;
	else if (WeaponName == "weapon_hkp2000")
		return 0.169f;
	else if (WeaponName == "weapon_usp_silencer")
		return 0.169f;
	else if (WeaponName == "weapon_p250")//the cz and p250 have the same name idky same with other guns
		return 0.15f;
	else if (WeaponName == "weapon_tec9")
		return 0.12f;
	else if (WeaponName == "weapon_elite")
		return 0.12f;
	else if (WeaponName == "weapon_fiveseven")
		return 0.15f;
	else if (WeaponName == "weapon_deagle")
		return 0.224f;
	else if (WeaponName == "weapon_nova")
		return 0.882f;
	else if (WeaponName == "weapon_sawedoff")
		return 0.845f;
	else if (WeaponName == "weapon_mag7")
		return 0.845f;
	else if (WeaponName == "weapon_xm1014")
		return 0.35f;
	else if (WeaponName == "weapon_mac10")
		return 0.075f;
	else if (WeaponName == "weapon_ump45")
		return 0.089f;
	else if (WeaponName == "weapon_mp9")
		return 0.070f;
	else if (WeaponName == "weapon_bizon")
		return 0.08f;
	else if (WeaponName == "weapon_mp7")
		return 0.08f;
	else if (WeaponName == "weapon_p90")
		return 0.070f;
	else if (WeaponName == "weapon_galilar")
		return 0.089f;
	else if (WeaponName == "weapon_ak47")
		return 0.1f;
	else if (WeaponName == "weapon_sg556")
		return 0.089f;
	else if (WeaponName == "weapon_m4a1_silencer")
		return 0.089f;
	else if (WeaponName == "weapon_m4a1")
		return 0.089f;
	else if (WeaponName == "weapon_aug")
		return 0.089f;
	else if (WeaponName == "weapon_m249")
		return 0.08f;
	else if (WeaponName == "weapon_negev")
		return 0.0008f;
	else if (WeaponName == "weapon_ssg08")
		return 1.25f;
	else if (WeaponName == "weapon_awp")
		return 1.463f;
	else if (WeaponName == "weapon_g3sg1")
		return 0.25f;
	else if (WeaponName == "weapon_scar20")
		return 0.25f;
	else if (WeaponName == "weapon_mp5sd")
		return 0.08f;
	else
		return .0f;

}

int IClientEntity::get_choked_ticks()
{
	if (!this || !this->IsAlive()) 
		return 0;

	float flSimulationTime = this->GetSimulationTime();
	float flOldSimulationTime = this->m_flOldSimulationTime();
	float flSimDiff = flSimulationTime - flOldSimulationTime;

	return TIME_TO_TICKS(max(0, flSimDiff /*- latency*/));
}

void IClientEntity::invalidate_bone_cache(void) 
{
	static auto invalidate_bone_bache_fn = game_utils::FindPattern1("client.dll", "80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81");

	*(uintptr_t*)((uintptr_t)this + 0x2924) = 0xFF7FFFFF;
	*(uintptr_t*)((uintptr_t)this + 0x2690) = **(uintptr_t**)((uintptr_t)invalidate_bone_bache_fn + 10) - 1;
}

bool IClientEntity::IsKnifeorNade()
{
	if (!this)
		return false;
	if (!this->IsAlive())
		return false;

	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)this->weapon();

	if (!pWeapon)
		return false;

	std::string WeaponName = pWeapon->GetName();

	if (WeaponName == "weapon_knife")
		return true;
	else if (WeaponName == "weapon_incgrenade")
		return true;
	else if (WeaponName == "weapon_decoy")
		return true;
	else if (WeaponName == "weapon_flashbang")
		return true;
	else if (WeaponName == "weapon_hegrenade")
		return true;
	else if (WeaponName == "weapon_smokegrenade")
		return true;
	else if (WeaponName == "weapon_molotov")
		return true;

	return false;
}

float &IClientEntity::m_flFlashDuration()
{
	static auto m_flFlashDuration = GET_NETVAR("DT_CSPlayer", "m_flFlashDuration");

	return *(float*)((DWORD)this + m_flFlashDuration);
}

std::array<float, 24> &IClientEntity::get_pose_paramaters()
{
	return *(std::array<float, 24>*)((DWORD)this + GET_NETVAR("DT_BaseEntity", "m_flPoseParameter"));
}
int IClientEntity::num_anim_overlays()
{
	if (this == nullptr)
		return 13;

	return *(int*)((DWORD)this + 0x298C);
}

bool IClientEntity::SetupBonesEx() {
	auto result = false;

	static auto r_jiggle_bones = interfaces::cvar->FindVar("r_jiggle_bones");

	const auto penis = r_jiggle_bones->GetInt();
	const auto old_origin = this->GetAbsOrigin();
	const auto effects = *(int*)((uintptr_t)this + 0xF0);
	const auto clientsideanim = client_side_animation();

	r_jiggle_bones->SetValue(0);

	const auto simtime_1 = get_simulation_time();
	const auto effects_backup = *(DWORD*)((uintptr_t)this + 0xF0);
	*(DWORD*)((uintptr_t)this + 0xA28) = 0;
	*(DWORD*)((uintptr_t)this + 0xA30) = 0;
	*(DWORD*)((uintptr_t)this + 0xF0) |= 8u;
//	cheat::main::setuping_bones = true;
	result = SetupBones(0, -1, 0x3FF00, simtime_1);                     // SetupBones
//	cheat::main::setuping_bones = false;
	*(DWORD*)((uintptr_t)this + 0xF0) = effects_backup;

	r_jiggle_bones->SetValue(penis);

	return result;
}

void IClientEntity::create_animation_state(CBaseAnimState* state) {
	using create_anim_state_t = void(__thiscall*)(CBaseAnimState*, IClientEntity*);
	static auto create_anim_state = (create_anim_state_t)game_utils::pattern_scan(
		GetModuleHandle("client.dll"), "55 8B EC 56 8B F1 B9 ?? ?? ?? ?? C7 46");
	if (!create_anim_state)
		return;

	create_anim_state(state, this);
}

inline vector ExtrapolateTick(vector p0, vector v0)
{
	return p0 + (v0 *intervalPerTick);
}
vector IClientEntity::GetPredicted(vector p0)
{
	return ExtrapolateTick(p0, this->getvelocity());
}
void IClientEntity::CopyPoseParameters(float* dest)
{
	float* flPose = (float*)((DWORD)this + 0x2764);
	memcpy(dest, flPose, sizeof(float) * 24);
}

void IClientEntity::CopyAnimLayers(CAnimationLayer* dest)
{
	int count = 15;
	for (int i = 0; i < count; i++)
	{
		dest[i] = GetAnimOverlayX(i);
	}
}

bool IClientEntity::has_gungame_immunity()
{
	static int m_bGunGameImmunity = GET_NETVAR("DT_CSPlayer", "m_bGunGameImmunity");
	return *(bool*)((DWORD)this + m_bGunGameImmunity);
}
void IClientEntity::pre_think() {
	typedef void(__thiscall *o_pregay)(void*);
	call_vfunc< o_pregay >(this, 317)(this);
}

void IClientEntity::think() {
	typedef void(__thiscall *o_pregay)(void*);
	call_vfunc< o_pregay >(this, 138)(this);
}
CBaseAnimState * IClientEntity::get_animation_state() 
{
	//	return *reinterpret_cast< CBaseAnimState ** >(reinterpret_cast< void * >(uintptr_t(this) + 0x3900));
	static auto animstate_offset = *(uintptr_t*)((uintptr_t)game_utils::FindPattern1("client.dll", "8B 8E ? ? ? ? F3 0F 10 48 04 E8 ? ? ? ? E9") + 0x2);
	return *(CBaseAnimState**)((uintptr_t)this + animstate_offset);
}

void IClientEntityList::for_each_player(const std::function<void(IClientEntity*)> fn)
{
	for (auto i = 0; i <= static_cast<int32_t>(interfaces::engine->GetMaxClients()); ++i) {
		const auto entity = get_client_entity(i);
		if (entity && entity->IsAlive())
			fn(reinterpret_cast<IClientEntity*>(entity));
	}
}