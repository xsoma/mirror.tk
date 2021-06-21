#include "Autowall.h"
#include "Menu.h"
#include "XorStr.hpp"
#include "RageBot.h"
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <stdexcept>

inline bool CGameTrace::DidHitWorld() const
{
	return m_pEnt->GetIndex() == 0;
}
inline bool CGameTrace::DidHitNonWorldEntity() const
{
	return m_pEnt != NULL && !DidHitWorld();
}

bool autowall_2::did_hit_world(IClientEntity* ent) {
	return ent == interfaces::ent_list->get_client_entity(0);
}

bool autowall_2::did_hit_non_world_entity(IClientEntity* ent) {
	return ent != nullptr && !did_hit_world(ent);
}


backup_autowall * backup_awall = new backup_autowall;
autowall_2* new_autowall = new autowall_2;


#define DAMAGE_NO		0
#define DAMAGE_EVENTS_ONLY	1	
#define DAMAGE_YES		2
#define DAMAGE_AIM		3
#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define	CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' ///< the egg sacs in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' ///< wierd-looking jello effect for advisor shield.

/*
void ScaleDamage_1(int hitgroup, IClientEntity* enemy, float weapon_armor_ratio, float& current_damage)
{
	int armor = enemy->ArmorValue();
	float ratio;

	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		current_damage *= 4.f;
		break;
	case HITGROUP_STOMACH:
		current_damage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		current_damage *= 0.75f;
		break;
	}

	if (armor > 0)
	{
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			if (enemy->HasHelmet())
			{
				ratio = (weapon_armor_ratio * 0.5) * current_damage;
				if (((current_damage - ratio) * 0.5) > armor)
					ratio = current_damage - (armor * 2.0);
				current_damage = ratio;
			}
			break;
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			ratio = (weapon_armor_ratio * 0.5) * current_damage;
			if (((current_damage - ratio) * 0.5) > armor)
				ratio = current_damage - (armor * 2.0);
			current_damage = ratio;
			break;
		}
	}
}


void autowall_2::TraceLine(Vector& absStart, Vector& absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(absStart, absEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	interfaces::trace->TraceRay(ray, mask, &filter, ptr);
}
*/
/*
void autowall_2::ClipTraceToPlayers(const Vector& absStart, const Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr)
{

	IClientEntity *pLocal = interfaces::ent_list->get_client_entity(interfaces::engine->GetLocalPlayer());
	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

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
		LEA		EDX, absEnd
		LEA		ECX, absStart
		CALL	dwAddress
		ADD		ESP, 0xC
	}
}
*/
// void UTIL_ClipTraceToPlayers(const vector& vecAbsStart, const vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr)
void backup_autowall::ClipTraceToPlayers(vector& absStart, vector absEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr, float minDistanceToRay)
{
	CGameTrace playerTrace;
	Ray_t ray;
	auto smallestFraction = tr->fraction;
	const auto maxRange = 60.0f;

	ray.Init2(absStart, absEnd);

	for (auto i = 1; i <= interfaces::globals->max_clients; i++) {
		auto player = reinterpret_cast< IClientEntity* >(interfaces::ent_list->get_client_entity(i));

		if (!player || !player->IsAlive() || player->IsDormant())
			continue;

		if (filter && !filter->ShouldHitEntity(player, mask))
			continue;

		float a;
		vector *c;

		auto range = Math_trash::distance_to_ray( player->world_space_center( ), absStart, absEnd, &a, c );
		if (range < minDistanceToRay || range > maxRange)
			continue;

		interfaces::trace_2->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, player, &playerTrace);
		if (playerTrace.fraction < smallestFraction) {
			*tr = playerTrace;
			smallestFraction = playerTrace.fraction;
		}
	}
}

////////////////////////////////////// Legacy Functions //////////////////////////////////////
void autowall_2::GetBulletTypeParameters(float& maxRange, float& maxDistance, bool sv_penetration_type)
{
	if (sv_penetration_type)
	{
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else
	{

		IClientEntity* pLocal = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
		C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

		if (pWeapon->GetWeaponID2() == WEAPON_AWP)
		{
			maxRange = 45.0;
			maxDistance = 8000.0;
		}

		else if (pWeapon->GetWeaponID2() == WEAPON_SSG08 || pWeapon->GetWeaponID2() == WEAPON_SCAR20 || pWeapon->GetWeaponID2() == WEAPON_G3SG1)
		{
			maxRange = 39.0;
			maxDistance = 5000.0;
		}

		else if (pWeapon->GetWeaponID2() == WEAPON_DEAGLE)
		{
			maxRange = 30.0;
			maxDistance = 3000.0;
		}

		else if (pWeapon->GetWeaponID2() == WEAPONTYPE_RIFLE)
		{
			maxRange = 30.0;
			maxDistance = 2000.0;
		}

		else
		{
			maxRange = 21.0;
			maxDistance = 800.0;
		}
	}
}

////////////////////////////////////// Misc Functions //////////////////////////////////////
/*
bool autowall_2::BreakableEntity(IClientEntity* entity)
{

	ClientClass* pClass = (ClientClass*)entity->GetClientClass();

	if (!pClass)
		return false;

	if (pClass == nullptr)
		return false;

//	if (pClass->m_ClassID = (int)CSGOClassID::CChicken)
//	return false;

	return pClass->m_ClassID == (int)CSGOClassID::CBreakableProp || pClass->m_ClassID == (int)CSGOClassID::CBreakableSurface;

}
void autowall_2::ScaleDamage(trace_t &enterTrace, CSWeaponInfo *weaponData, float& currentDamage)
{
	//Cred. to N0xius for reversing this.
	//TODO: _xAE^; look into reversing this yourself sometime

	bool hasHeavyArmor = false;
	int armorValue = ((IClientEntity*)enterTrace.m_pEnt)->ArmorValue();
	int hitGroup = enterTrace.hitgroup;

	//Fuck making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
	auto IsArmored = [&enterTrace]()->bool
	{
		IClientEntity* targetEntity = (IClientEntity*)enterTrace.m_pEnt;
		switch (enterTrace.hitgroup)
		{
		case HITGROUP_HEAD:
			return !!(IClientEntity*)targetEntity->HasHelmet(); //Fuck compiler errors - force-convert it to a bool via (!!)
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (hitGroup)
	{
	case HITGROUP_HEAD:
		currentDamage *= 2.f; //Heavy Armor does 1/2 damage
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored())
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->armor_ratio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		currentDamage = NewDamage;
		damage_output = currentDamage;
	}
}
void tr_ln(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr) {
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	interfaces::trace->TraceRay(ray, mask, &filter, ptr);
}
////////////////////////////////////// Main Autowall Functions //////////////////////////////////////
bool autowall_2::TraceToExit(Vector& vecEnd, CGameTrace* pEnterTrace, Vector vecStart, Vector vecDir,
	CGameTrace* pExitTrace)
{
	auto dist = 0.f;

	while (dist <= 90.f) {
		dist += 4.f;
		vecEnd = vecStart + vecDir * dist;

		const auto point_contents = interfaces::trace->GetPointContents(
			vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);
		if (point_contents & MASK_SHOT_HULL && !(point_contents & CONTENTS_HITBOX))
			continue;

		auto a1 = vecEnd - vecDir * 4.f;
		Ray_t ray;
		ray.Init(vecEnd, a1);

		interfaces::trace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, pExitTrace);

		if (pExitTrace->startpos != Vector(0, 0, 0) && pExitTrace->surface.flags & SURF_HITBOX) {
			CTraceFilterSkipEntity filter(pExitTrace->m_pEnt);
			Ray_t ray2;
			ray2.Init(vecEnd, vecStart);
			interfaces::trace->TraceRay(ray2, MASK_SHOT_HULL, &filter, pExitTrace);

			if ((pExitTrace->fraction < 1.f || pExitTrace->allsolid) && !pExitTrace->startsolid) {
				vecEnd = pExitTrace->endpos;
				return true;
			}

			continue;
		}

		if (!pExitTrace->DidHit() || pExitTrace->startsolid) {
			if (pEnterTrace->m_pEnt && (pEnterTrace->m_pEnt != nullptr && pEnterTrace->m_pEnt !=
				interfaces::ent_list->get_client_entity(0))) {
				*pExitTrace = *pEnterTrace;
				pExitTrace->endpos = vecStart + vecDir;
				return true;
			}

			continue;
		}

		if (!pExitTrace->DidHit() || pExitTrace->startsolid) {
			if (pEnterTrace->m_pEnt != nullptr && !pEnterTrace->m_pEnt->GetIndex() == 0 &&
				IsBreakableEntity(
					reinterpret_cast< IClientEntity* >(pEnterTrace->m_pEnt))) {
				*pExitTrace = *pEnterTrace;
				pExitTrace->endpos = vecStart + vecDir;
				return true;
			}

			continue;
		}

		if (pExitTrace->surface.flags >> 7 & SURF_LIGHT && !(pEnterTrace->surface.flags >> 7 & SURF_LIGHT))
			continue;

		if (pExitTrace->plane.normal.Dot(vecDir) <= 1.f) {
			vecEnd = vecEnd - vecDir * (pExitTrace->fraction * 4.f);
			return true;
		}
	}

	return false;
}

bool autowall_2::HandleBulletPenetration(CSWeaponInfo* weaponData, trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration)
{
	auto local = reinterpret_cast< IClientEntity* >(interfaces::ent_list->get_client_entity(
		interfaces::engine->GetLocalPlayer()));
	//Because there's been issues regarding this- putting this here.
	if (&currentDamage == nullptr || !local || !weaponData || !local->IsAlive())
		throw std::invalid_argument("currentDamage is null!\n");

	Vector end;
	CGameTrace exitTrace;
	auto pEnemy = reinterpret_cast< IClientEntity* >(enterTrace.m_pEnt);
	if (!pEnemy || pEnemy->is_dormant() || !pEnemy->IsAlive()) return false;
	auto enterSurfaceData = interfaces::phys_props->GetSurfaceData(enterTrace.surface.surfaceProps);
	if (!enterSurfaceData) return false;
	int enterMaterial = enterSurfaceData->game.material;

	auto enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
	auto enterDamageModifier = enterSurfaceData->game.flDamageModifier;
	float thickness, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ((enterTrace.contents >> 3) & CONTENTS_SOLID);
	bool isLightSurf = ((enterTrace.surface.flags >> 7) & SURF_LIGHT);

	if (possibleHitsRemaining <= 0
		//Test for "DE_CACHE/DE_CACHE_TELA_03" as the entering surface and "CS_ITALY/CR_MISCWOOD2B" as the exiting surface.
		//Fixes a wall in de_cache which seems to be broken in some way. Although bullet penetration is not recorded to go through this wall
		//Decals can be seen of bullets within the glass behind of the enemy. Hacky method, but works.
		//You might want to have a check for this to only be activated on de_cache.
		|| (enterTrace.surface.name == (const char*)0x2227c261 && exitTrace.surface.name == (const char*)0x2227c868)
		|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial
			!= CHAR_TEX_GLASS)
		|| weaponData->penetration <= 0.f
		|| !TraceToExit(end, &enterTrace, enterTrace.endpos, direction, &exitTrace)
		&& !(interfaces::trace->GetPointContents(enterTrace.endpos, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL
			))
		return false;

	auto exitSurfaceData = interfaces::phys_props->
		GetSurfaceData(exitTrace.surface.surfaceProps);
	if (!exitSurfaceData) return false;
	int exitMaterial = exitSurfaceData->game.material;
	auto exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
	auto exitDamageModifier = exitSurfaceData->game.flDamageModifier;

	//Are we using the newer penetration system?
	if (sv_penetration_type) {
		if (enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS) {
			combinedPenetrationModifier = 3.f;
			finalDamageModifier = 0.05f;
		}
		else if (isSolidSurf || isLightSurf) {
			combinedPenetrationModifier = 1.f;
			finalDamageModifier = 0.16f;
		}
		else if (enterMaterial == CHAR_TEX_FLESH && (local->team() == pEnemy->team() &&
			ff_damage_reduction_bullets == 0.f)) {
			if (ff_damage_bullet_penetration == 0.f)
				return false;

			combinedPenetrationModifier = ff_damage_bullet_penetration;
			finalDamageModifier = 0.16f;
		}
		else {
			combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) / 2.f;
			finalDamageModifier = 0.16f;
		}

		//Do our materials line up?
		if (enterMaterial == exitMaterial) {
			if (exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD)
				combinedPenetrationModifier = 3.f;
			else if (exitMaterial == CHAR_TEX_PLASTIC)
				combinedPenetrationModifier = 2.f;
		}

		//Calculate thickness of the wall by getting the length of the range of the trace and squaring
		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();
		auto modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);

		//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
		auto lostDamage = fmaxf(
			((modifier * thickness) / 24.f)
			+ ((currentDamage * finalDamageModifier)
				+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

		//Did we loose too much damage?
		if (lostDamage > currentDamage)
			return false;

		//We can't use any of the damage that we've lost
		if (lostDamage > 0.f)
			currentDamage -= lostDamage;

		//Do we still have enough damage to deal?
		if (currentDamage < 1.f)
			return false;

		eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;

		return true;
	}
	else //Legacy penetration system
	{
		combinedPenetrationModifier = 1.f;

		if (isSolidSurf || isLightSurf)
			finalDamageModifier = 0.99f; //Good meme :^)
		else {
			finalDamageModifier = fminf(enterDamageModifier, exitDamageModifier);
			combinedPenetrationModifier = fminf(enterSurfPenetrationModifier, exitSurfPenetrationModifier);
		}

		if (enterMaterial == exitMaterial && (exitMaterial == CHAR_TEX_METAL || exitMaterial == CHAR_TEX_WOOD))
			combinedPenetrationModifier += combinedPenetrationModifier;

		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();

		if (sqrt(thickness) <= combinedPenetrationModifier * penetrationPower) {
			currentDamage *= finalDamageModifier;
			eyePosition = exitTrace.endpos;
			--possibleHitsRemaining;

			return true;
		}

		return false;
	}
}

bool autowall_2::FireBullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage)
{
	if (!pWeapon)
		return false;

	IClientEntity* local = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->GetLocalPlayer());

	auto data = FireBulletData(local->GetEyePosition());

	data.filter = CTraceFilter();
	data.filter.pSkip = local;

	bool sv_penetration_type;

	float currentDistance = 0.f, penetrationPower, penetrationDistance, maxRange, ff_damage_reduction_bullets, ff_damage_bullet_penetration, rayExtension = 40.f;
	Vector eyePosition = local->GetEyePosition();

	static ConVar* penetrationSystem = interfaces::cvar->FindVar(("sv_penetration_type"));
	static ConVar* damageReductionBullets = interfaces::cvar->FindVar(("ff_damage_reduction_bullets"));
	static ConVar* damageBulletPenetration = interfaces::cvar->FindVar(("ff_damage_bullet_penetration"));

	sv_penetration_type = penetrationSystem->GetBool();
	ff_damage_reduction_bullets = damageReductionBullets->GetFloat();
	ff_damage_bullet_penetration = damageBulletPenetration->GetFloat();

	damage_reduction = ff_damage_bullet_penetration;

	CSWeaponInfo* weaponData = pWeapon->GetCSWpnData();
	CGameTrace enterTrace;
	CTraceFilter filter;

	filter.pSkip = local;

	if (!weaponData)
		return false;

	maxRange = weaponData->range;

	GetBulletTypeParameters(penetrationPower, penetrationDistance, sv_penetration_type);

	if (sv_penetration_type)
		penetrationPower = weaponData->penetration;

	int possibleHitsRemaining = 4;

	currentDamage = weaponData->damage;

	while (possibleHitsRemaining > 0 && currentDamage >= 1.f)
	{
		maxRange -= currentDistance;

		Vector end = eyePosition + direction * maxRange;

		TraceLine(eyePosition, end, MASK_SHOT_HULL | CONTENTS_HITBOX, local, &enterTrace);
		ClipTraceToPlayers(eyePosition, end + direction * rayExtension, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter,
			&enterTrace, 1.f); //  | CONTENTS_HITBOX

		surfacedata_t *enterSurfaceData = interfaces::phys_props->GetSurfaceData(enterTrace.surface.surfaceProps);

		if (enterSurfaceData == nullptr)
			return false;

		float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;

		int enterMaterial = enterSurfaceData->game.material;

		if (enterTrace.fraction == 1.f)
			break;

		currentDistance += enterTrace.fraction * maxRange;

		currentDamage *= pow(weaponData->range_modifier, (currentDistance / 500.f));

		if (currentDistance > penetrationDistance && weaponData->penetration > 0.f || enterSurfPenetrationModifier < 0.1f)
			break;

		auto canDoDamage = (enterTrace.hitgroup != HITGROUP_GEAR && enterTrace.hitgroup != HITGROUP_GENERIC);
		auto isPlayer = ((enterTrace.m_pEnt)->cs_player());

		bool isEnemy = (((IClientEntity*)local)->team() != ((IClientEntity*)enterTrace.m_pEnt)->team());
		auto onTeam = ((reinterpret_cast< IClientEntity* >(enterTrace.m_pEnt)->team() == 3 || (
			reinterpret_cast< IClientEntity* >(enterTrace.m_pEnt)->team() == 2)));

		if ((canDoDamage && isPlayer && isEnemy) && onTeam) {

			ScaleDamage(enterTrace, weaponData, currentDamage);
			damage_output = currentDamage;
			return true;
		}
		if (!HandleBulletPenetration(weaponData, enterTrace, eyePosition, direction, possibleHitsRemaining, currentDamage, penetrationPower, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}
	return false;
}

////////////////////////////////////// Usage Calls //////////////////////////////////////
bool autowall_2::CanHit(Vector &point)
{

	auto player = reinterpret_cast< IClientEntity* >(interfaces::ent_list->get_client_entity(interfaces::engine->GetLocalPlayer()));
	if (!player || !player->IsAlive())
		return false;

	Vector angles, direction;
	auto tmp = point - player->GetEyePosition();
	float currentDamage = 0;

	VectorAngles(tmp, angles);
	AngleVectors(angles, &direction);
	direction.NormalizeInPlace();
	auto weapon = player->GetWeapon2();
	if (!weapon)
		return -1;
	if (FireBullet(weapon, direction, currentDamage))
	{
		if (damage_output >= options::menu.aimbot.AccuracyMinimumDamage.GetValue())
		{
			return true;
		}
	}

	return false; //That wall is just a bit too thick buddy
}

bool autowall_2::PenetrateWall(IClientEntity* pBaseEntity, Vector& vecPoint)
{
	float min_damage = options::menu.aimbot.AccuracyMinimumDamage.GetValue();
	if (pBaseEntity->GetHealth() <= min_damage)
		min_damage = pBaseEntity->GetHealth();

	if (CanHit(vecPoint) >= min_damage)
	{
		ragebot->can_autowall = true;
		return true;
	}
	else
		ragebot->can_autowall = false;

	return false;
}
*/


float backup_autowall::GetHitgroupDamageMult_2(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_HEAD:
		return 4.f;
	case HITGROUP_STOMACH:
		return 1.25f;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		return 0.75f;
	default:
		return 1.f;
	}
}

void backup_autowall::ScaleDamage_2(int hitgroup, IClientEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
	current_damage *= GetHitgroupDamageMult_2(hitgroup);
	int helmet = enemy->HasHelmet();
	int armor = enemy->ArmorValue();
	float ratio;
	if (armor > 0)
	{
		if (armor > 0)
		{
			switch (hitgroup)
			{
			case HITGROUP_HEAD:
				if (enemy->HasHelmet())
				{
					ratio = (weapon_armor_ratio * 0.5) * current_damage;
					if (((current_damage - ratio) * 0.5) > armor)
						ratio = current_damage - (armor * 2.0);
					current_damage = ratio;
				}
				break;
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
				ratio = (weapon_armor_ratio * 0.5) * current_damage;
				if (((current_damage - ratio) * 0.5) > armor)
					ratio = current_damage - (armor * 2.0);
				current_damage = ratio;
				break;
			}
		}
	}
}


void angle_vectors_2(const vector &angles, vector& forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

bool backup_autowall::HandleBulletPenetration_2(CSWeaponInfo *wpn_data, FireBulletData &data)
{
	if (&data.current_damage == nullptr)
		throw std::invalid_argument(" current damage is nullptr ");

	surfacedata_t *enter_surface_data = interfaces::phys_props->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow(wpn_data->range_modifier, (data.trace_length * 0.002));

	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;

	if (data.penetrate_count <= 0)
		return false;

	bool isSolidSurf = ((data.enter_trace.contents >> 3) & CONTENTS_SOLID);
	bool isLightSurf = ((data.enter_trace.surface.flags >> 7) & SURF_LIGHT);

	vector dummy;
	trace_t trace_exit;

	if (!TraceToExit(dummy, data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit)) 
		return false;

	surfacedata_t *exit_surface_data = interfaces::phys_props->GetSurfaceData(trace_exit.surface.surfaceProps);

	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71) || (enter_material == CHAR_TEX_GLASS))
	{ 
		combined_penetration_modifier = 3.0f; final_damage_modifier = 0.05f; 
	}
	else if (isSolidSurf || isLightSurf)
	{
		combined_penetration_modifier = 1.f;
		final_damage_modifier = 0.16f;
	}
	else { 
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f; 
	}
	
	if (enter_material == exit_material)
	{
		if (exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == CHAR_TEX_PLASTIC)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->penetration) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;

	float lost_damage = fmaxf(0.0f, v35 + thickness);
	if (lost_damage > data.current_damage)return false;
	if (lost_damage >= 0.0f)data.current_damage -= lost_damage;
	if (data.current_damage < 1.0f) return false;
	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool backup_autowall::SimulateFireBullet_2(IClientEntity *local, C_BaseCombatWeapon *weapon, FireBulletData &data, IClientEntity * player)
{

	data.penetrate_count = 4; // Max Amount Of Penitration
	data.trace_length = 0.0f; // wow what a meme

	if (!weapon)
		return false;

	if (!&data)
		return false;
	auto *wpn_data = weapon->GetCSWpnData(); // Get Weapon Info
	if (!wpn_data)
		return false; 

	data.current_damage = (float)wpn_data->damage;// Set Damage Memes
	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = wpn_data->range - data.trace_length;

		vector End_Point = data.src + data.direction * data.trace_length_remaining;
		UTIL_TraceLine(data.src, End_Point, MASK_SHOT_HULL | CONTENTS_HITBOX, local, 0, &data.enter_trace);
		UTIL_ClipTraceToPlayers( data.src, End_Point * 40.f, MASK_SHOT_HULL | CONTENTS_HITBOX, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
			break;
		if ((data.enter_trace.hitgroup != HITGROUP_GENERIC) && (data.enter_trace.hitgroup <= 7) && (local->team() != data.enter_trace.m_pEnt->team()) )
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= pow(wpn_data->range_modifier, data.trace_length * 0.002f);
			ScaleDamage_2(data.enter_trace.hitgroup, data.enter_trace.m_pEnt, wpn_data->armor_ratio, data.current_damage);
			return true;
		}
		if (!HandleBulletPenetration_2(wpn_data, data))
			break;
	}

	return false;
}

bool backup_autowall::can_hit(const vector &point, float *damage_given, IClientEntity * player)
{
	auto *local = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	auto data = FireBulletData(local->GetOrigin() + local->GetViewOffset());
	data.filter = CTraceFilter();
	data.filter.pSkip = local;

	vector angles;
	CalcAngle(data.src, point, angles);
	angle_vectors_2(angles, data.direction);
	VectorNormalize(data.direction);

	if (SimulateFireBullet_2(local, (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle((HANDLE)local->GetActiveWeaponHandle()), data, player))
	{
		*damage_given = data.current_damage;
		if (data.current_damage > 0.0f)
			ragebot->can_autowall = true;

		if (data.current_damage >= options::menu.aimbot.AccuracyMinimumDamage.GetValue())
		{
			return true;
		}
	}


	return false;
}
