#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include "position_adjust.h"
#include <iostream>
#include <time.h>
#include "UTIL Functions.h"
#include "xostr.h"
#include <chrono>
#include "Hooks.h"
#include "global_count.h"
#include "laggycompensation.h"
#include "MD5.cpp"
//#include "otr_awall.h"
#include "antiaim.h"
#include "fakelag.h"
#include "experimental.h"
#include "lin_extp.h"
#include "MiscHacks.h"
#include "newbacktrack.h"
//float bigboi::current_yaw;

float current_desync;
vector LastAngleAA2;
static bool dir = false;
static bool back = false;
static bool up = false;
static bool jitter = false;

static bool backup = false;
static bool default_aa = true;
static bool panic = false;
float hitchance_custom;
#define TICK_INTERVAL			(interfaces::globals->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
CAimbot * ragebot = new CAimbot;
c_newbacktrack * new_backtrack = new c_newbacktrack;
extra * ext = new extra;

/*
 Hey Hunter if you happen to find somehing not being used or duplicated feel free to delete it.
 I legit can't even navigate this anymore. Mirror recode got me used to actual organization lmfao

 Also, if you end up using obb_mins and obb_maxs then use entity->obbs_mins, ...
 this is because the default ayyware one is trash and doesn't recognize it if you try to force a value on it.

  If you wanna set a fixed tickbase, it's -->   overall::fixed_tickbase
*/


void CAimbot::Init()
{
	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CAimbot::Draw()
{
}
float curtime_fixed(CUserCmd* ucmd) {
	auto local_player = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = local_player->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * interfaces::globals->interval_per_tick;
	return curtime;
}

void RandomSeed(UINT seed)
{
	typedef void(*RandomSeed_t)(UINT);
	static RandomSeed_t m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed");
	m_RandomSeed(seed);
	return;
}

void CAimbot::auto_revolver(CUserCmd* m_pcmd) // credits: https://steamcommunity.com/id/x-87
{
	auto m_local = hackManager.plocal();
	auto m_weapon = m_local->weapon();
	if (!m_weapon)
		return;

	if (!shot_this_tick && *m_weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
	{

		float flPostponeFireReady = m_weapon->GetFireReadyTime();
		if (flPostponeFireReady > 0 && flPostponeFireReady - 1 < interfaces::globals->curtime)
		{
			m_pcmd->buttons &= ~IN_ATTACK;
		}
		static int delay = 0;
		delay++;

		if (delay <= 15) {
			m_pcmd->buttons |= IN_ATTACK;

			if (pTarget->isValidPlayer())
				m_pcmd->tick_count = C_LagCompensation::get().player_lag_records[pTarget->GetIndex()].tick_count;
		}
		else
			delay = 0;

	}
	else
	{
		if (*m_weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
		{
			static int delay = 0;
			delay++;

			if (delay <= 15)
				m_pcmd->buttons |= IN_ATTACK;
			else
				delay = 0;
		}
	}
}


bool IsAbleToShoot(IClientEntity* pLocal)
{
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (!pLocal)return false;
	if (!pWeapon)return false;
	float flServerTime = pLocal->GetTickBase() * interfaces::globals->interval_per_tick;
	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}
float CAimbot::hitchance()
{
	float hitchance = 101;
	auto m_local = hackManager.plocal();
	auto pWeapon = m_local->weapon();
	if (pWeapon)
	{
		if (options::menu.aimbot.AccuracyHitchance.GetValue() > 0)
		{
			float inaccuracy = pWeapon->GetInaccuracy();
			if (inaccuracy == 0) inaccuracy = 0.0000001;
			inaccuracy = 1 / inaccuracy;
			hitchance = inaccuracy;
		}
		return hitchance;
	}
}
bool CAimbot::CanOpenFire(IClientEntity * local)
{

	C_BaseCombatWeapon* entwep = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(local->GetActiveWeaponHandle());

	if (!entwep)
		return false;

	float flServerTime = (float)local->GetTickBase() * interfaces::globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();
	std::cout << flServerTime << " " << flNextPrimaryAttack << std::endl;
	return !(flNextPrimaryAttack > flServerTime);
}

template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

void CAimbot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocalEntity = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

	if (!interfaces::engine->IsConnected() || !interfaces::engine->IsInGame() || !pLocalEntity->isValidPlayer())
	{
		C_LagCompensation::get().reset();
		//i just wanted to get your aattention to tell you that unless you are a communist, i appreciate you :)
		return; 
	}

	if (!pWeapon)
		return; 
	if (options::menu.misc.AntiAimEnable.getstate()) {
		if (pLocalEntity->IsAlive() && pLocalEntity->GetMoveType() != MOVETYPE_LADDER)
		{
			c_fakelag->Fakelag(pCmd, bSendPacket);
			c_antiaim->DoAntiAim(pCmd, bSendPacket);
		}
	}
	else
		c_beam->visual_angle = pCmd->viewangles.y;
	
	if (options::menu.aimbot.enable.getstate())
	{
		DoAimbot(pCmd, bSendPacket);
		auto_revolver(pCmd);
		DoNoRecoil(pCmd);
	}
	LastAngle = pCmd->viewangles;
}
inline float FastSqrt(float x)
{
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}
#define square( x ) ( x * x )
void ClampMovement(CUserCmd* pCommand, float fMaxSpeed)
{
	if (fMaxSpeed <= 0.f)
		return;
	float fSpeed = (float)(FastSqrt(square(pCommand->forwardmove) + square(pCommand->sidemove) + square(pCommand->upmove)));
	if (fSpeed <= 0.f)
		return;
	if (pCommand->buttons & IN_DUCK)
		fMaxSpeed *= 2.8;
	if (fSpeed <= fMaxSpeed)
		return;
	float fRatio = fMaxSpeed / fSpeed;
	pCommand->forwardmove *= fRatio;
	pCommand->sidemove *= fRatio;
	pCommand->upmove *= fRatio;
}

void CAimbot::sim_time_delay(IClientEntity* entity)
{
	float old_sim[65] = { 0.f };
	float current_sim[65] = { entity->GetSimulationTime() };

	if (entity->m_flOldSimulationTime() != current_sim[entity->GetIndex()])
	{
		can_shoot = true;
		shot_refined = true;
	//	old_sim[entity->GetIndex()] = current_sim[entity->GetIndex()];
	}
	else
	{
		can_shoot = false;
		shot_refined = false;
	}
}

static float spent = 0.00f;

bool CAimbot::allow(IClientEntity* player, CUserCmd* pcmd, IClientEntity * local)
{
	if (C_LagCompensation::get().player_lag_records[pTarget->GetIndex()].tick_count == -1) // very bad tick bro
		return false;

	auto net = interfaces::engine->GetNetChannelInfo();
	if (net && ((((pcmd->tick_count + 1) * interfaces::globals->interval_per_tick) + net->GetLatency(FLOW_OUTGOING)) - net->GetLatency(FLOW_OUTGOING)) <= player->get_simulation_time())
		return true;

	if ((!(player->GetFlags() & FL_ONGROUND) && Globals::missedshots[player->GetIndex()] < 1))
	{
		return true;
	}

	if ((!(player->GetFlags() & FL_ONGROUND) && Globals::missedshots[player->GetIndex()] >= 1))
	{
		return spent >= 0.015;
	}

	if (backup_awall->wall_damage <= player->GetHealth() + 9)
		return spent >= 0.0156;

	return spent >= 0.010;
}

void CAimbot::delay_shot(IClientEntity* entity, CUserCmd* pcmd)
{
	float old_sim[65] = { 0.f };
	float current_sim[65] = { entity->GetSimulationTime() };
	vector vec_position[65], origin_delta[65];
	bool lag_comp = false;

	int index = options::menu.aimbot.delay_shot.getindex();

	switch (index)
	{
	case 0: can_shoot = true;
		break;
	case 1:
	{
		sim_time_delay(entity);
	}
	break;

	case 2: // bameware
	{

		if (entity->m_VecORIGIN() != vec_position[entity->GetIndex()])
		{
			origin_delta[entity->GetIndex()] = entity->m_VecORIGIN() - vec_position[entity->GetIndex()];
			vec_position[entity->GetIndex()] = entity->m_VecORIGIN();

			lag_comp = fabs(origin_delta[entity->GetIndex()].Length()) > 64;
		}

		if (lag_comp && entity->getvelocity().length2d() > 300)
		{
			can_shoot = false;
		}
		else
			can_shoot = true;
	}
	break;

	case 3:
	{	
		if (allow(entity, pcmd, hackManager.plocal()))
		{
			sim_time_delay(entity);
		}
	}
	break;
	}

}

namespace debug_helpers // will not be used for now
{
	auto hitbox_to_String = [](int hitgroup) -> std::string
	{
		switch (hitgroup)
		{
		case 0:
		{
			return "HEAD";
		}
		case 1:
			return "NECK";
		case 2:
			return "PELVIS";
		case 3:
			return "STOMACH";
		case 5:
			return "LOWER CHEST";
		case 6:
			return "UPPER CHEST";
		case 7:
			return "THIGHS";
		case 8:
			return" THIGHS";
		case 11:
			return "LEGS";
		case 12:
			return "LEGS";
		case 16:
			return "ARMS";
		case 17:
			return "ARMS";
		default:
			return "BODY";
		}
	};


}

void CAimbot::faxzee_extrapolation(IClientEntity * pTarget, vector AP)
{
	
}


typedef void(__cdecl* MsgFn)(const char* msg, va_list);

void gamer_message(const char* msg, ...)
{
	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg"); char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list); //Calls the function, we got the address above.
}

inline int time_to_ticks(float time) {
	return static_cast< int >(time / interfaces::globals->interval_per_tick + 0.5f);
}


void CAimbot::DoAimbot(CUserCmd* pCmd, bool& bSendPacket)
{

	IClientEntity* pLocal = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	bool FindNewTarget = true;
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pLocal)
		return;

	if (pWeapon)
	{
		if ((!pWeapon->isZeus() && pWeapon->GetAmmoInClip() < 1) || pWeapon->IsC4() || pWeapon->IsGrenade() || pWeapon->isZeus()) {

			return;
		}
	}
	else
		return;

	if (IsLocked && TargetID > -0 && HitBox >= 0)
	{
		pTarget = interfaces::ent_list->get_client_entity(TargetID);
		if (pTarget && TargetMeetsRequirements(pTarget, pLocal))
		{
			HitBox = HitScan(pTarget, pLocal);
			if (HitBox >= 0)
			{
				vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				vector View; interfaces::engine->get_viewangles(View);
				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);

				if (FoV < 180.f)
					FindNewTarget = false;
			}
		}
	}

	if (FindNewTarget)
	{
		spent = 0.00f;

		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;
		TargetID = get_target_fov(pLocal);
		if (TargetID >= 0)
		{
			pTarget = interfaces::ent_list->get_client_entity(TargetID);
		}
	}

	if (TargetID >= 0 && pTarget)
	{
		spent += interfaces::globals->interval_per_tick;
		HitBox = HitScan(pTarget, pLocal);
		//	hb = options::menu.aimbot.Multienable.getstate() ? GetHitboxPosition(pTarget, HitBox, C_LagCompensation::instance().player_lag_records[pTarget->GetIndex()].simtime) : hitbox_location(pTarget, HitBox);

		switch (options::menu.aimbot.extrapolation.getindex())
		{
		case 1: pTarget->GetPredicted(hb);
			break;
		}

		if (!CanOpenFire(pLocal)) {
			return;
		}

		shot_this_tick = false;
		switch (options::menu.misc.QuickStop.getindex())
		{
		case 1:
		{
			if ((pLocal->GetFlags() & FL_ONGROUND) && pLocal->getvelocity().length2d() > c_misc->get_gun(pWeapon) - 5)
			{
				pCmd->buttons |= IN_SPEED;
				ClampMovement(pCmd, (c_misc->get_gun(pWeapon) - (4 - rand() % 4)));
			}
		}
		break;

		case 2:
		{
			if ((pLocal->GetFlags() & FL_ONGROUND) && pLocal->getvelocity().length2d() > c_misc->get_gun(pWeapon) - 5)
			{
				c_misc->MinimalWalk(pCmd, c_misc->get_gun(pWeapon) - (2 + rand() % 5)); // huge memes
			}
		}
		break;
		}

		float hc = hitchance();
		if (game_utils::IsScopedWeapon(pWeapon) && !pWeapon->IsScoped() && options::menu.aimbot.AccuracyAutoScope.getstate())
		{
			pCmd->buttons |= IN_ATTACK2;
		}

		if (options::menu.misc.QuickCrouch.getstate() && !(GetAsyncKeyState(options::menu.misc.fake_crouch_key.GetKey())))
			pCmd->buttons |= IN_DUCK;

		C_LagCompensation::get().finish_position_adjustment();
		if (hc >= options::menu.aimbot.AccuracyHitchance.GetValue() * 1.5)
		{
			float this_sim = 0.f;

			this_sim = C_LagCompensation::get().player_lag_records[pTarget->GetIndex()].restore_record.simulation_time + C_LagCompensation::get().get_interpolation();
			auto rec = &C_LagCompensation::get().player_lag_records[pTarget->GetIndex()];
			if (AimAtPoint(pLocal, hb, pCmd, bSendPacket))
			{
				Globals::aim_point = hb;
				if (options::menu.aimbot.AimbotAutoFire.getstate() && !(pCmd->buttons & IN_ATTACK))
				{

					delay_shot(pTarget, pCmd);
					switch (can_shoot)
					{
					case true:
					{
						Globals::shot[pTarget->GetIndex()] = true;
						c_fakelag->shot = true;

						pCmd->buttons |= IN_ATTACK; // bang		
						pCmd->tick_count = (options::menu.aimbot.delay_shot.getindex() > 1) ?
							TIME_TO_TICKS(this_sim) : TIME_TO_TICKS(rec->simtime + C_LagCompensation::get().get_interpolation());

						if (!(GetAsyncKeyState(options::menu.misc.fake_crouch_key.GetKey()))) {
							!options::menu.aimbot.rage_chokeshot.getstate() ? bSendPacket = true : bSendPacket = (1 <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
						}

						c_fakelag->shot = false;
						shot_this_tick = true;

					}
					break;
					}

					if (!(pCmd->buttons |= IN_ATTACK))
					{
						shot_this_tick = false;
					}

				}
			}
		}
	}

}

float VectorDistance(const vector& v1, const vector& v2)
{
	return FastSqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

bool CAimbot::TargetMeetsRequirements(IClientEntity* pEntity, IClientEntity* local)
{
	//	auto local = interfaces::ent_list->get_client_entity(interfaces::engine->GetLocalPlayer());
//	ClientClass *pClientClass = pEntity->GetClientClass();

	if (pEntity->isValidPlayer())
	{
		if ( pEntity->cs_player() )
		{
			if (!pEntity->is_dormant())
			{
				// Team Check
				if (pEntity->team() != local->team())
				{
			//		if (!pEntity->has_gungame_immunity())
					{
						if (!(pEntity->GetFlags() & FL_FROZEN)) // ice age niggas
						{
							if (pEntity->GetOrigin() != vector(0,0,0))
								return true;
						}
					}

				}
			}			
		}
	
	}
	
	return false;
}

float CAimbot::FovToPlayer(vector ViewOffSet, vector View, IClientEntity* pEntity, int aHitBox)
{

	CONST FLOAT MaxDegrees = 180.0f;

	vector Angles = View;

	vector Origin = ViewOffSet;

	vector Delta(0, 0, 0);

	vector Forward(0, 0, 0);

	AngleVectors(Angles, &Forward);

	vector AimPos = GetHitboxPosition(pEntity, aHitBox, C_LagCompensation::get().player_lag_records[pEntity->GetIndex()].simtime);

	VectorSubtract(AimPos, Origin, Delta);

	Normalize(Delta, Delta);

	FLOAT DotProduct = Forward.Dot(Delta);

	return (acos(DotProduct) * (MaxDegrees / PI));
}
int CAimbot::get_target_fov(IClientEntity* pLocal)
{
	int target = -1;
	float min_fov = 180.f;

	vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	vector View; interfaces::engine->get_viewangles(View);

	for (int i = 0; i < 65; i++)
	{
		IClientEntity *pEntity = interfaces::ent_list->get_client_entity(i);
		
		if (TargetMeetsRequirements(pEntity, pLocal))
		{
			if (pEntity->GetOrigin() == vector(0, 0, 0))
				continue;

			int NewHitBox = HitScan(pEntity, pLocal);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov <= min_fov && fov <= 180.f)
				{
					min_fov = fov;
					target = i;
				}
			}
		}
	}

	return target;
}
float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	vector ang, aim;
	AngleVectors(viewAngle, &aim);
	AngleVectors(aimAngle, &ang);
	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}
bool CAimbot::should_baim(IClientEntity* pEntity)
{
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(pEntity->GetActiveWeaponHandle());

	if (!pWeapon)
		return false;

	int health = options::menu.aimbot.BaimIfUnderXHealth.GetValue();
	bool fake = resolver->enemy_fake[pEntity->GetIndex()];

	if ((GetAsyncKeyState(options::menu.aimbot.bigbaim.GetKey())
		|| (options::menu.aimbot.baim_fake.getstate() && fake)
		|| (options::menu.aimbot.blacklist_if_breaklc.getstate() && enemy_breaking_lc && pEntity->getvelocity().length2d() >= 39.f)
		|| (options::menu.aimbot.baim_inair.getstate() && !(pEntity->GetFlags() & FL_ONGROUND))
		|| pEntity->GetHealth() <= health)
		|| game_utils::IsZeus(pWeapon))
	{
		return true;
	}
	else
		return false;

	return false;
}


std::vector<int> CAimbot::head_hitscan()
{
	std::vector<int> hitbox;
	hitbox.push_back((int)csgo_hitboxes::head);
	//	hitbox.push_back((int)csgo_hitboxes::neck);

	return hitbox;
}



std::vector<int> CAimbot::body_hitscan()
{
	std::vector<int> hitbox;
	hitbox.push_back((int)csgo_hitboxes::upper_chest);
	hitbox.push_back((int)csgo_hitboxes::pelvis);
	hitbox.push_back((int)csgo_hitboxes::stomach);
	hitbox.push_back((int)csgo_hitboxes::left_thigh);
	hitbox.push_back((int)csgo_hitboxes::right_thigh);

	return hitbox;
}

std::vector<int> CAimbot::lowerbody_hitscan(IClientEntity * player)
{
	std::vector<int> hitbox;

	hitbox.push_back((int)csgo_hitboxes::pelvis);
	hitbox.push_back((int)csgo_hitboxes::stomach);

	if (!resolver->enemy_fake[player->GetIndex()])
	{
		hitbox.push_back((int)csgo_hitboxes::left_thigh);
		hitbox.push_back((int)csgo_hitboxes::right_thigh);
	}

	return hitbox;
}

std::vector<int> CAimbot::awp_minimal_hitscan()
{
	std::vector<int> scan_hitboxes;
	scan_hitboxes.push_back((int)csgo_hitboxes::stomach);
	scan_hitboxes.push_back((int)csgo_hitboxes::lower_chest);
	scan_hitboxes.push_back((int)csgo_hitboxes::pelvis);
	scan_hitboxes.push_back((int)csgo_hitboxes::right_foot);
	scan_hitboxes.push_back((int)csgo_hitboxes::left_foot);

	return scan_hitboxes;
}

std::vector<int> CAimbot::minimal_hitscan(IClientEntity * player)
{
	std::vector<int> scan_hitboxes;
	scan_hitboxes.push_back((int)csgo_hitboxes::head);

	if (!resolver->enemy_fake[player->GetIndex()])
	{
		scan_hitboxes.push_back((int)csgo_hitboxes::upper_chest);
	}

	scan_hitboxes.push_back((int)csgo_hitboxes::pelvis);

	if ((player->getvelocity().length2d() > 20 && options::menu.aimbot.ignorelimbs.getstate()) || !resolver->enemy_fake[player->GetIndex()])
	{
		scan_hitboxes.push_back((int)csgo_hitboxes::right_foot);
		scan_hitboxes.push_back((int)csgo_hitboxes::left_foot);
	}

	scan_hitboxes.push_back((int)csgo_hitboxes::head);

	return scan_hitboxes;
}

std::vector<int> CAimbot::essential_hitscan(IClientEntity * player)
{
	std::vector<int> scan_hitboxes;
	scan_hitboxes.push_back((int)csgo_hitboxes::head);
	scan_hitboxes.push_back((int)csgo_hitboxes::upper_chest);
	scan_hitboxes.push_back((int)csgo_hitboxes::lower_chest);

	if ((player->getvelocity().length2d() > 20 && options::menu.aimbot.ignorelimbs.getstate()) || !resolver->enemy_fake[player->GetIndex()])
	{
		scan_hitboxes.push_back((int)csgo_hitboxes::left_upper_arm);
		scan_hitboxes.push_back((int)csgo_hitboxes::right_upper_arm);
		scan_hitboxes.push_back((int)csgo_hitboxes::right_foot);
		scan_hitboxes.push_back((int)csgo_hitboxes::left_foot);
	}
	scan_hitboxes.push_back((int)csgo_hitboxes::pelvis);
	scan_hitboxes.push_back((int)csgo_hitboxes::left_thigh);
	scan_hitboxes.push_back((int)csgo_hitboxes::right_thigh);

	scan_hitboxes.push_back((int)csgo_hitboxes::head);
	return scan_hitboxes;
}

std::vector<int> CAimbot::maximal_hitscan(IClientEntity * player)
{
	std::vector<int> scan_hitboxes; 
	scan_hitboxes.push_back((int)csgo_hitboxes::head);
	
	scan_hitboxes.push_back((int)csgo_hitboxes::upper_chest);
	scan_hitboxes.push_back((int)csgo_hitboxes::lower_chest);
	if ((player->getvelocity().length2d() > 34 && options::menu.aimbot.ignorelimbs.getstate()) || !resolver->enemy_fake[player->GetIndex()])
	{
		scan_hitboxes.push_back((int)csgo_hitboxes::left_upper_arm);
		scan_hitboxes.push_back((int)csgo_hitboxes::right_upper_arm);
		scan_hitboxes.push_back((int)csgo_hitboxes::left_lower_arm);
		scan_hitboxes.push_back((int)csgo_hitboxes::right_lower_arm);
		scan_hitboxes.push_back((int)csgo_hitboxes::right_foot);
		scan_hitboxes.push_back((int)csgo_hitboxes::left_foot);
	}
	scan_hitboxes.push_back((int)csgo_hitboxes::stomach);
	scan_hitboxes.push_back((int)csgo_hitboxes::pelvis);

	if (!resolver->enemy_fake[player->GetIndex()])
	{
		scan_hitboxes.push_back((int)csgo_hitboxes::left_thigh);
		scan_hitboxes.push_back((int)csgo_hitboxes::right_thigh);
	}
	scan_hitboxes.push_back((int)csgo_hitboxes::head);
	return scan_hitboxes;
}

int CAimbot::HitScan(IClientEntity* player, IClientEntity* pLocal)
{

	float health = options::menu.aimbot.BaimIfUnderXHealth.GetValue();

	std::vector<int> scan_hitboxes;
	//	std::vector<dropdownboxitem> auto_list = options::menu.aimbot.target_auto.items;
	//	std::vector<dropdownboxitem> scout_list = options::menu.aimbot.target_scout.items;
	//	std::vector<dropdownboxitem> awp_list = options::menu.aimbot.target_awp.items;
	//	std::vector<dropdownboxitem> pistol_list = options::menu.aimbot.target_pistol.items;
	//	std::vector<dropdownboxitem> smg_list = options::menu.aimbot.target_smg.items;
	//	std::vector<dropdownboxitem> otr_list = options::menu.aimbot.target_otr.items;

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

	if (pWeapon != nullptr)
	{
		switch (should_baim(player))
		{
			case true:
			{
				scan_hitboxes = lowerbody_hitscan(player);
			}
			break;

			case false:
			{
				if (pWeapon->isAuto())
				{

					switch (options::menu.aimbot.target_auto2.getindex())
					{
					case 0:
						scan_hitboxes = head_hitscan();
						break;
					case 1:
						scan_hitboxes = minimal_hitscan(player);
						break;
					case 2:
						scan_hitboxes = essential_hitscan(player);
						break;
					case 3:
						scan_hitboxes = maximal_hitscan(player);
						break;
					case 4:
						scan_hitboxes = lowerbody_hitscan(player);
						break;
					
					}

				}
				if (pWeapon->is_scout())
				{
					switch (options::menu.aimbot.target_scout2.getindex())
					{
					case 0:
						scan_hitboxes = head_hitscan();
						break;
					case 1:
						scan_hitboxes = minimal_hitscan(player);
						break;
					case 2:
						scan_hitboxes = essential_hitscan(player);
						break;
					case 3:
						scan_hitboxes = maximal_hitscan(player);
						break;
					case 4:
						scan_hitboxes = lowerbody_hitscan(player);
						break;
					}

				}

				if (pWeapon->is_awp())
				{
					switch (options::menu.aimbot.target_awp2.getindex())
					{
					case 0:
						scan_hitboxes = head_hitscan();
						break;
					case 1:
						scan_hitboxes = minimal_hitscan(player);
						break;
					case 2:
						scan_hitboxes = essential_hitscan(player);
						break;
					case 3:
						scan_hitboxes = maximal_hitscan(player);
						break;
					case 4:
						scan_hitboxes = lowerbody_hitscan(player);
						break;
					}

				}

				if (pWeapon->isPistol()) // head
				{

					switch (options::menu.aimbot.target_pistol2.getindex())
					{
					case 0:
						scan_hitboxes = head_hitscan();
						break;
					case 1:
						scan_hitboxes = minimal_hitscan(player);
						break;
					case 2:
						scan_hitboxes = essential_hitscan(player);
						break;
					case 3:
						scan_hitboxes = maximal_hitscan(player);
						break;
					case 4:
						scan_hitboxes = lowerbody_hitscan(player);
						break;
					}
				}

				if (game_utils::IsMP(pWeapon))
				{

					switch (options::menu.aimbot.target_smg2.getindex())
					{
					case 0:
						scan_hitboxes = head_hitscan();
						break;
					case 1:
						scan_hitboxes = minimal_hitscan(player);
						break;
					case 2:
						scan_hitboxes = essential_hitscan(player);
						break;
					case 3:
						scan_hitboxes = maximal_hitscan(player);
						break;
					case 4:
						scan_hitboxes = lowerbody_hitscan(player);
						break;
					}

				}

				if (game_utils::IsRifle(pWeapon) || game_utils::IsShotgun(pWeapon) || game_utils::IsMachinegun(pWeapon))
				{

					switch (options::menu.aimbot.target_otr2.getindex())
					{
					case 0:
						scan_hitboxes = head_hitscan();
						break;
					case 1:
						scan_hitboxes = minimal_hitscan(player);
						break;
					case 2:
						scan_hitboxes = essential_hitscan(player);
						break;
					case 3:
						scan_hitboxes = maximal_hitscan(player);
						break;
					case 4:
						scan_hitboxes = lowerbody_hitscan(player);
						break;
					}
				}

				if (game_utils::IsZeus(pWeapon))
				{
					scan_hitboxes = body_hitscan();
				}
			}
			break;
		}	

		for (auto HitBoxID : scan_hitboxes)
		{
			vector Point, lol;
			hb = GetHitboxPosition(player, HitBoxID, C_LagCompensation::get().player_lag_records[player->GetIndex()].simtime);

			float dmg = 0.f;

			if (backup_awall->can_hit(hb, &dmg, player))
			{
				ragebot->can_autowall = true;
				return HitBoxID;
			}

		}
		return -1;
	}
}

void CAimbot::DoNoRecoil(CUserCmd *pCmd)
{
	vector AimPunch = hackManager.plocal()->localPlayerExclusive()->GetAimPunchAngle();
	if (AimPunch.length2d() > 0 && AimPunch.length2d() < 150)
	{
		pCmd->viewangles -= AimPunch * 2.00;
		game_utils::NormaliseViewAngle(pCmd->viewangles);
	}
}

bool CAimbot::AimAtPoint(IClientEntity* pLocal, vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	if (point.Length() == 0) 
		return false;
	vector angles;
	vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();
	CalcAngle(src, point, angles);
	game_utils::NormaliseViewAngle(angles);
	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return false;
	}
	IsLocked = true;

	vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle;

	float fovLeft = FovToPlayer(ViewOffset, LastAimstepAngle, interfaces::ent_list->get_client_entity(TargetID), HitBox);
	vector AddAngs = angles - LastAimstepAngle;
	
	options::menu.aimbot.rage_silent.getstate() ? pCmd->viewangles = angles : interfaces::engine->SetViewAngles(angles);
	pCmd->viewangles = angles;
	return true;
}
