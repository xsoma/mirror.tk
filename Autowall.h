#pragma once
#include "Interfaces.h"
#include "Singleton.hpp"
struct FireBulletData
{
	FireBulletData(const vector &eye_pos)
		: src(eye_pos)
	{
	}

	vector           src;
	trace_t          enter_trace;
	vector           direction;
	CTraceFilter    filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
};



class autowall_2
{
public:

	
	bool handle_penetration;

	float damage_reduction;
	float damage_output;
	bool did_hit_world(IClientEntity * ent);

	bool did_hit_non_world_entity(IClientEntity * ent);

	void ClipTraceToPlayers(vector & absStart, vector absEnd, unsigned int mask, ITraceFilter * filter, CGameTrace * tr, float minDistanceToRay);

	void GetBulletTypeParameters(float & maxRange, float & maxDistance, bool sv_penetration_type);

	bool BreakableEntity(IClientEntity * entity);

	void ScaleDamage(trace_t & enterTrace, CSWeaponInfo * weaponData, float & currentDamage);



	bool TraceToExit(vector & vecEnd, CGameTrace * pEnterTrace, vector vecStart, vector vecDir, CGameTrace * pExitTrace);

	bool HandleBulletPenetration(CSWeaponInfo * weaponData, trace_t & enterTrace, vector & eyePosition, vector direction, int & possibleHitsRemaining, float & currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration);

	bool FireBullet(C_BaseCombatWeapon * pWeapon, vector & direction, float & currentDamage);

	bool CanHit(vector & point);

	bool PenetrateWall(IClientEntity * pBaseEntity, vector & vecPoint);

};
extern autowall_2 * new_autowall;


class backup_autowall
{
public:

	bool can_hit(const vector &point, float *damage_given, IClientEntity * player);
//	bool trace_autowallable_2(float& dmg);
	bool SimulateFireBullet_2(IClientEntity *local, C_BaseCombatWeapon *weapon, FireBulletData &data, IClientEntity * player);

	void ClipTraceToPlayers(vector & absStart, vector absEnd, unsigned int mask, ITraceFilter * filter, trace_t * tr, float minDistanceToRay);

	float GetHitgroupDamageMult_2(int iHitGroup);

	void ScaleDamage_2(int hitgroup, IClientEntity * enemy, float weapon_armor_ratio, float & current_damage);

	bool HandleBulletPenetration_2(CSWeaponInfo * wpn_data, FireBulletData & data);

	float wall_damage = 0.0f;

};
extern backup_autowall * backup_awall;
