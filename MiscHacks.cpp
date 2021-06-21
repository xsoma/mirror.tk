#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "IClientMode.h"
#include <chrono>
#include <algorithm>
#include <time.h>
#include "Hooks.h"
#include "position_adjust.h"
#include "RageBot.h"
#include "Autowall.h"
#include "Resolver.h"
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <stdexcept>

CMiscHacks* c_misc = new CMiscHacks;
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
inline float bitsToFloat(unsigned long i)
{
	return *reinterpret_cast<float*>(&i);
}
inline float FloatNegate(float f)
{
	return bitsToFloat(FloatBits(f) ^ 0x80000000);
}
vector AutoStrafeView;
void CMiscHacks::Init()
{
}
void CMiscHacks::Draw()
{
	if (!interfaces::engine->IsConnected() || !interfaces::engine->IsInGame())
		return;

}

void set_name(const char* name)
{
	ConVar* nameConvar = interfaces::cvar->FindVar(("name"));
	*(int*)((DWORD)&nameConvar->fnChangeCallback + 0xC) = NULL;
	nameConvar->SetValueChar(name);
}

inline float FastSqrt(float x)
{
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}
#define square( x ) ( x * x )

void angleVectors(const vector& angles, vector& forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}
#include "experimental.h"
void CMiscHacks::zeusbot(CUserCmd *m_pcmd, IClientEntity*  player)
{
	auto m_local = hackManager.plocal();

	if (m_local->GetFlags() & FL_FROZEN)
		return;
	if (!m_local->IsAlive())
		return;
	if (!m_local->weapon())
		return;
	if (!m_local->weapon()->isZeus())
		return;
	do_zeus = false;

	for (int y = 0; y <= 360; y += 360.f / 6.f) {
		for (int x = -89; x <= 89; x += 179.f / 6.f) {
			vector ang = vector(x, y, 0);
			vector dir;
			angleVectors(ang, dir);
			trace_t trace;
			UTIL_TraceLine(m_local->GetEyePosition(), m_local->GetEyePosition() + (dir * 554), MASK_SHOT, m_local, 0, &trace);

			if (trace.m_pEnt == nullptr)
				continue;
			if (trace.m_pEnt == m_local)
				continue;
			if (!player || trace.m_pEnt != player)
				continue;
			if (!trace.m_pEnt->IsAlive())
				continue;
		
			if (trace.m_pEnt->team() == m_local->team())
				continue;
			if (trace.m_pEnt->IsDormant())
				continue;

		
			do_zeus = true;

			m_pcmd->viewangles = vector(x, y, 0);
			m_pcmd->buttons |= IN_ATTACK;
			m_pcmd->tick_count = TIME_TO_TICKS(C_LagCompensation::get().player_lag_records[player->GetIndex()].simtime + backtracking->GetLerpTime());
			do_zeus = false;

			return;
		}
	}
}

void airstuck(CUserCmd * cmd)
{
	int key;
	if (key) {
		cmd->tick_count = INT_MAX;
		cmd->command_number = INT_MAX;
	}
}

void CMiscHacks::namespam()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < .5)
		return;
	const char* result;
	std::vector <std::string> names;
	if (interfaces::engine->IsInGame() && interfaces::engine->IsConnected()) {
		for (int i = 1; i < interfaces::globals->max_clients; i++)
		{
			IClientEntity *entity = interfaces::ent_list->get_client_entity(i);

			player_info_t pInfo;

			if (entity && hackManager.plocal()->team() == entity->team())
			{
				ClientClass* cClass = (ClientClass*)entity->GetClientClass();

				if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
				{
					if (interfaces::engine->GetPlayerInfo(i, &pInfo))
					{
						if (!strstr(pInfo.name, "GOTV"))
							names.push_back(pInfo.name);
					}
				}
			}
		}
	}

	set_name("\n\xAD\xAD\xAD");
	int randomIndex = rand() % names.size();
	char buffer[128];
	sprintf_s(buffer, "%s ", names[randomIndex].c_str());
	result = buffer;



	set_name(result);
	start_t = clock();

}
/*
template<class T>
static T* FindHudElement(const char* name)
{

static auto pThis = *reinterpret_cast<DWORD**>(Utilities::Memory::FindPatternV2("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 85 C0 0F 84 ? ? ? ? 8D 58") + 1);

static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(Utilities::Memory::FindPatternV2("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39"));

if (find_hud_element != nullptr)
{
return (T*)find_hud_element(pThis, name);
}

}
void preservefeed(IGameEvent* Event)
{
if (hackManager.pLocal()->IsAlive())
{
static DWORD* _death_notice = FindHudElement<DWORD>("CCSGO_HudDeathNotice");

if (_death_notice == nullptr)
return;

static void(__thiscall *_clear_notices)(DWORD) = (void(__thiscall*)(DWORD))Utilities::Memory::FindPatternV2("client_panorama.dll", "55 8B EC 83 EC 0C 53 56 8B 71 58");

if (round_change)
{

_death_notice = FindHudElement<DWORD>("CCSGO_HudDeathNotice");
if (_death_notice - 20)
_clear_notices(((DWORD)_death_notice - 20));
round_change = false;
}

if (_death_notice)
*(float*)((DWORD)_death_notice + 0x50) = Options::Menu.VisualsTab.killfeed.GetState() ? 100 : 1;
}
}*/
void CMiscHacks::buybot_primary()
{
	bool is_ct = hackManager.plocal()->team() == TEAM_CS_CT;
	switch (options::menu.misc.buybot_primary.getindex())
	{
	case 1: is_ct ? (interfaces::engine->ExecuteClientCmd("buy scar20;")) : (interfaces::engine->ExecuteClientCmd("buy g3sg1;"));
		break;
	case 2: interfaces::engine->ExecuteClientCmd("buy ssg08;");
		break;
	case 3: interfaces::engine->ExecuteClientCmd("buy awp;");
		break;
	case 4: is_ct ? (interfaces::engine->ExecuteClientCmd("buy m4a1; buy m4a1_silencer")) : (interfaces::engine->ExecuteClientCmd("buy ak47;"));
		break;
	case 5: is_ct ? (interfaces::engine->ExecuteClientCmd("buy aug;")) : (interfaces::engine->ExecuteClientCmd("buy sg556"));
		break;
	case 6: is_ct ? (interfaces::engine->ExecuteClientCmd("buy mp9")) : (interfaces::engine->ExecuteClientCmd("buy mac-10;"));
		break;
	}

}

void CMiscHacks::buybot_secondary()
{
	switch (options::menu.misc.buybot_secondary.getindex())
	{
	case 1: interfaces::engine->ExecuteClientCmd("buy elite;");
		break;
	case 2: interfaces::engine->ExecuteClientCmd("buy deagle;");
		break;
	case 3: interfaces::engine->ExecuteClientCmd("buy fn57");
		break;
	}

}

void CMiscHacks::buybot_otr()
{
	std::vector<dropdownboxitem> otr_list = options::menu.misc.buybot_otr.items;

	if (otr_list[0].GetSelected)
	{
		interfaces::engine->ExecuteClientCmd("buy vest; buy vesthelm;");
	}

	if (otr_list[1].GetSelected)
	{
		interfaces::engine->ExecuteClientCmd("buy hegrenade;");
	}

	if (otr_list[2].GetSelected)
	{
		interfaces::engine->ExecuteClientCmd("buy flashbang;");
	}

	if (otr_list[3].GetSelected)
	{
		interfaces::engine->ExecuteClientCmd("buy smokegrenade;");
	}

	if (otr_list[4].GetSelected)
	{
		interfaces::engine->ExecuteClientCmd("buy molotov;");
	}

	if (otr_list[5].GetSelected)
	{
		interfaces::engine->ExecuteClientCmd("buy taser;");
	}

	if (otr_list[6].GetSelected)
	{
		interfaces::engine->ExecuteClientCmd("buy defuser;");
	}

}

void CMiscHacks::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity *pLocal = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	IGameEvent* Event;

	static bool yes;
	float log = -1;
	switch (options::menu.ColorsTab.toggle_aspectratio.getstate())
	{
	case true:
	{
		float cur = options::menu.ColorsTab.aspectratio.GetValue() / 100;
		static float old;

		if (cur != old)
		{
			auto aspect = interfaces::cvar->FindVar("r_aspectratio");

			log = aspect->GetFloat();

			aspect->SetValue(cur);
			old = cur;
		}
		yes = true;
	}
	break;

	case false:
	{
		if (yes == true)
		{
			auto aspect = interfaces::cvar->FindVar("r_aspectratio");
			log != -1 ? aspect->SetValue(log) : aspect->SetValue(1);
			yes = false;
		}
	}
	break;
	}

	static auto plague_marker = interfaces::cvar->FindVar("r_modelwireframedecal");
	*(int*)((DWORD)&plague_marker->fnChangeCallback + 0xC) = 0;
	plague_marker->SetValue(options::menu.misc.plague.getstate());

	if (options::menu.visuals.fogui.getstate())
	{
		ConVar* sv_cheats = interfaces::cvar->FindVar("sv_cheats");
		SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
		sv_cheats_spoofed->SetInt(1);

		options::menu.m_bIsOpen = false;

		interfaces::engine->ExecuteClientCmd("fog_enable 1");
		interfaces::engine->ExecuteClientCmd("fog_override 2"); 
		interfaces::engine->ExecuteClientCmd("fog_color 255 255 255");
		interfaces::engine->ExecuteClientCmd("fog_start -200");
		interfaces::engine->ExecuteClientCmd("fog_end -1");
		interfaces::engine->ExecuteClientCmd("fogui 1");
		options::menu.visuals.fogui.SetState(false);
	}

	if (!hackManager.plocal()->IsAlive())
	{
		_done = false;
		return;
	}

	// ------- Oi thundercunt, this is needed for the weapon configs ------- //

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

//	preset_cfg();
	if (pWeapon != nullptr)
	{

		if (options::menu.misc.fakeangle_compaitibility.getstate())
		{
			if ((GetAsyncKeyState(options::menu.misc.minimal_walk.GetKey())))
			{
				FakeWalk0(pCmd, bSendPacket);
			}

		}
		else
		{
			if (GetAsyncKeyState(options::menu.misc.minimal_walk.GetKey()))
			{
				MinimalWalk(pCmd, get_gun(pWeapon));
			}

		}		
		
		if (game_utils::AutoSniper(pWeapon))
		{
			switch (options::menu.aimbot.automatic_cfg.getstate()) {
				case true:
				{
					int ticks_standing = 0;
					if (hackManager.plocal()->getvelocity().length2d() < 4.f)
						ticks_standing++;
					else
						ticks_standing = 0;

					if (hackManager.plocal()->getvelocity().length2d() < 75.f) {

						resolver->auto_config(hackManager.plocal(), ragebot->pTarget, head_first);
						if (pWeapon->GetAmmoInClip() > 19.f && ticks_standing > 4)
							head_first ? options::menu.aimbot.AccuracyHitchance.SetValue(34.f) : options::menu.aimbot.AccuracyHitchance.SetValue(20.f);
						else
							head_first ? options::menu.aimbot.AccuracyHitchance.SetValue(60.f) : options::menu.aimbot.AccuracyHitchance.SetValue(49.f);

						if (backup_awall->wall_damage > 54.f) // average damage with an auto
							options::menu.aimbot.AccuracyMinimumDamage.SetValue(54.f);
						else options::menu.aimbot.AccuracyMinimumDamage.SetValue(29.f);
					}
					else
						options::menu.aimbot.AccuracyHitchance.SetValue((float)options::menu.aimbot.hc_auto.GetValue()), head_first = false;
				}
				break;

				case false:
				{
					options::menu.aimbot.AccuracyHitchance.SetValue((float)options::menu.aimbot.hc_auto.GetValue());
					if (GetAsyncKeyState(options::menu.aimbot.toggle_override.GetKey()))
					{
						options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.mindmg_override.GetValue());
					}
					else
						options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.md_auto.GetValue());
				}
				break;
			}

		}

		if (game_utils::IsPistol(pWeapon))
		{

			options::menu.aimbot.AccuracyHitchance.SetValue((float)options::menu.aimbot.hc_pistol.GetValue());

			if (GetAsyncKeyState(options::menu.aimbot.toggle_override.GetKey()))
			{
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.mindmg_override.GetValue());
			}
			else
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.md_pistol.GetValue());

		}

		if (pWeapon->is_scout())
		{

			options::menu.aimbot.AccuracyHitchance.SetValue((float)options::menu.aimbot.hc_scout.GetValue());

			if (GetAsyncKeyState(options::menu.aimbot.toggle_override.GetKey()))
			{
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.mindmg_override.GetValue());
			}
			else
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.md_scout.GetValue());

		}

		if (pWeapon->is_awp())
		{
			options::menu.aimbot.AccuracyHitchance.SetValue((float)options::menu.aimbot.hc_awp.GetValue());

			if (GetAsyncKeyState(options::menu.aimbot.toggle_override.GetKey()))
			{
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.mindmg_override.GetValue());
			}
			else
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.md_awp.GetValue());

		}

		if (game_utils::IsRifle(pWeapon) || game_utils::IsShotgun(pWeapon) || game_utils::IsMachinegun(pWeapon))
		{

			options::menu.aimbot.AccuracyHitchance.SetValue((float)options::menu.aimbot.hc_otr.GetValue());

			if (GetAsyncKeyState(options::menu.aimbot.toggle_override.GetKey()))
			{
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.mindmg_override.GetValue());
			}
			else
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.md_otr.GetValue());

		}

		if (game_utils::IsMP(pWeapon))
		{
			options::menu.aimbot.AccuracyHitchance.SetValue((float)options::menu.aimbot.hc_smg.GetValue());

			if (GetAsyncKeyState(options::menu.aimbot.toggle_override.GetKey()))
			{
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.mindmg_override.GetValue());
			}
			else
				options::menu.aimbot.AccuracyMinimumDamage.SetValue((float)options::menu.aimbot.md_smg.GetValue());
		}

		if (game_utils::IsZeus(pWeapon))
		{
			options::menu.aimbot.AccuracyHitchance.SetValue(10);
			options::menu.aimbot.AccuracyMinimumDamage.SetValue(10);
		} 
	}

	if (options::menu.misc.infinite_duck.getstate())
	{
		pCmd->buttons |= IN_BULLRUSH;
	}

	if (options::menu.visuals.override_viewmodel.getstate())
		viewmodel_x_y_z();


	if (pLocal->movetype() == MOVETYPE_LADDER || pLocal->movetype() == MOVETYPE_NOCLIP)
		return;

	if (options::menu.misc.OtherAutoJump.getstate())
		AutoJump(pCmd);
	if (options::menu.misc.airduck_type.getindex() != 0)
	{
		airduck(pCmd);
	}
	interfaces::engine->get_viewangles(AutoStrafeView);
	if (options::menu.misc.OtherAutoStrafe.getstate())
	{
		strafer(pCmd);
	}

	fake_crouch(pCmd, bSendPacket, pLocal);

/*
	if (!_done && hackManager.pLocal()->IsAlive())
	{
		if (options::menu.misc.buybot_primary.getindex() != 0)
			buybot_primary();

		if (options::menu.misc.buybot_secondary.getindex() != 0)
			buybot_secondary();

		buybot_otr();

		_done = true;
	}
*/
	if (options::menu.misc.NameChanger.getstate())
	{
		namespam();
	}

}

void CMiscHacks::fog()
{
	if (options::menu.visuals.fog_update.getstate())
	{
		float start = -(options::menu.visuals.fog_start.GetValue() * 10);
		float end = -((30000 * -(options::menu.visuals.fog_end.GetValue() / 10)) - 5000);

		auto cheats = interfaces::cvar->FindVar("sv_cheats");
		cheats->SetValue(1);

		interfaces::engine->ExecuteClientCmd("fog_override 1");

		interfaces::engine->ExecuteClientCmd("fog_enable 1");

		auto f_start = interfaces::cvar->FindVar("fog_start");
		f_start->SetValue(start);

		auto f_end = interfaces::cvar->FindVar("fog_end");
		f_end->SetValue(end);

		interfaces::engine->ExecuteClientCmd("fog_color 60, 0, 90");
		options::menu.visuals.fog_update.SetState(false);
	}
}

int CMiscHacks::GetFPS()
{
	static int fps = 0;
	static int count = 0;
	using namespace std::chrono;
	auto now = high_resolution_clock::now();
	static auto last = high_resolution_clock::now();
	count++;
	if (duration_cast<milliseconds>(now - last).count() > 1000)
	{
		fps = count;
		count = 0;
		last = now;
	}
	return fps;
}
float curtime_fixedx(CUserCmd* ucmd) {
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

void VectorAnglesXXX(vector forward, vector &angles)
{
	float tmp, yaw, pitch;

	if (forward[2] == 0 && forward[0] == 0)
	{
		yaw = 0;

		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / PI);

		if (yaw < 0)
			yaw += 360;
		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / PI);

		if (pitch < 0)
			pitch += 360;
	}

	if (pitch > 180)
		pitch -= 360;
	else if (pitch < -180)
		pitch += 360;

	if (yaw > 180)
		yaw -= 360;
	else if (yaw < -180)
		yaw += 360;

	if (pitch > 89)
		pitch = 89;
	else if (pitch < -89)
		pitch = -89;

	if (yaw > 180)
		yaw = 180;
	else if (yaw < -180)
		yaw = -180;

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}
vector CalcAngleFakewalk(vector src, vector dst)
{
	vector ret;
	VectorAnglesXXX(dst - src, ret);
	return ret;
}

void rotate_movement(float yaw, CUserCmd* cmd)
{
	vector viewangles;
	QAngle yamom;
	interfaces::engine->get_viewangles(viewangles);
	float rotation = DEG2RAD(viewangles.y - yaw);
	float cos_rot = cos(rotation);
	float sin_rot = sin(rotation);
	float new_forwardmove = (cos_rot * cmd->forwardmove) - (sin_rot * cmd->sidemove);
	float new_sidemove = (sin_rot * cmd->forwardmove) + (cos_rot * cmd->sidemove);
	cmd->forwardmove = new_forwardmove;
	cmd->sidemove = new_sidemove;
}

float fakewalk_curtime(CUserCmd* ucmd)
{
	auto local_player = hackManager.plocal();

	if (!local_player)
		return 0;

	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted)
	{
		g_tick = (float)local_player->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * interfaces::globals->interval_per_tick;
	return curtime;
}
void CMiscHacks::FakeWalk0(CUserCmd* pCmd, bool &bSendPacket)
{
	IClientEntity* pLocal = hackManager.plocal();

	globalsh.fakewalk = true;
	static int iChoked = -1;
	iChoked++;
	if (pCmd->forwardmove > 0)
	{
		pCmd->buttons |= IN_BACK;
		pCmd->buttons &= ~IN_FORWARD;
	}
	if (pCmd->forwardmove < 0)
	{
		pCmd->buttons |= IN_FORWARD;
		pCmd->buttons &= ~IN_BACK;
	}
	if (pCmd->sidemove < 0)
	{
		pCmd->buttons |= IN_MOVERIGHT;
		pCmd->buttons &= ~IN_MOVELEFT;
	}
	if (pCmd->sidemove > 0)
	{
		pCmd->buttons |= IN_MOVELEFT;
		pCmd->buttons &= ~IN_MOVERIGHT;
	}
	static int choked = 0;
	choked = choked > 14 ? 0 : choked + 1;

	float nani = 40 / 14;

	pCmd->forwardmove = choked < nani || choked > 14 ? 0 : pCmd->forwardmove;
	pCmd->sidemove = choked < nani || choked > 14 ? 0 : pCmd->sidemove; //100:6 are about 16,6, quick maths
	bSendPacket = choked < 1;
}

static __declspec(naked) void __cdecl Invoke_NET_SetConVar(void* pfn, const char* cvar, const char* value)
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		and     esp, 0FFFFFFF8h
		sub     esp, 44h
		push    ebx
		push    esi
		push    edi
		mov     edi, cvar
		mov     esi, value
		jmp     pfn
	}
}

void CMiscHacks::AutoPistol(CUserCmd* pCmd)
{
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());
	static bool WasFiring = false;
	if (game_utils::IsPistol(pWeapon) && !game_utils::IsBomb(pWeapon))
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
				ragebot->was_firing = true;
			}
		}
		else
			ragebot->was_firing = false;

		WasFiring = pCmd->buttons & IN_ATTACK ? true : false;
	}
	else
		return;
}
void CMiscHacks::AutoJump(CUserCmd *pCmd)
{
	auto g_LocalPlayer = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	auto userCMD = pCmd;
	if (options::menu.misc.autojump_type.getindex() < 1)
	{
		if (!g_LocalPlayer)
			return;
		if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER) 
			return;
		if (userCMD->buttons & IN_JUMP && !(g_LocalPlayer->GetFlags() & FL_ONGROUND))
		{
			userCMD->buttons &= ~IN_JUMP;
		}
	}
	if (options::menu.misc.autojump_type.getindex() > 0)
	{
		if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
			return;
		userCMD->buttons |= IN_JUMP;
	}
}
void CMiscHacks::airduck(CUserCmd *pCmd) // quack
{
	auto local = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (options::menu.misc.airduck_type.getindex() == 1)
	{
		if (!(local->GetFlags() & FL_ONGROUND))
		{
			pCmd->buttons |= IN_DUCK;
		}
	}
	else if (options::menu.misc.airduck_type.getindex() == 2)
	{
		if (!(local->GetFlags() & FL_ONGROUND))
		{
			static bool counter = false;
			static int counters = 0;
			if (counters == 9)
			{
				counters = 0;
				counter = !counter;
			}
			counters++;
			if (counter)
			{
				pCmd->buttons |= IN_DUCK;
			}
			else
				pCmd->buttons &= ~IN_DUCK;
		}
	}
}
template<class T, class U>
inline T clampangle(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

#define nn(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
bool bHasGroundSurface(IClientEntity* pLocalBaseEntity, const vector& vPosition)
{
	trace_t pTrace;
	vector vMins, vMaxs; pLocalBaseEntity->GetRenderBounds(vMins, vMaxs);

	UTIL_TraceLine(vPosition, { vPosition.x, vPosition.y, vPosition.z - 32.f }, MASK_PLAYERSOLID_BRUSHONLY, pLocalBaseEntity, 0, &pTrace);

	return pTrace.fraction != 1.f;
}

void CMiscHacks::strafe_2(CUserCmd * cmd)
{
	auto local = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (local->GetMoveType() == MOVETYPE_NOCLIP || local->GetMoveType() == MOVETYPE_LADDER  || !local || !local->IsAlive())
		return;

	if (interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::KEY_A) || interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::KEY_D) || interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::KEY_S) || interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::KEY_W) || interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::KEY_LSHIFT))
		return;

	if (!(local->GetFlags() & FL_ONGROUND)) {
		if (cmd->mousedx > 1 || cmd->mousedx < -1) {
			cmd->sidemove = clamp(cmd->mousedx < 0.f ? -450.0f : 450.0f, -450.0f, 450.0f);
		}
		else {
			cmd->forwardmove = 10000.f / local->getvelocity().Length();
			cmd->sidemove = (cmd->command_number % 2) == 0 ? -450.0f : 450.0f;
			if (cmd->forwardmove > 450.0f)
				cmd->forwardmove = 450.0f;
		}
	}
}

vector GetAutostrafeView()
{
	return AutoStrafeView;
}

void CMiscHacks::PostProcces()
{
	ConVar* Meme = interfaces::cvar->FindVar("mat_postprocess_enable");
	SpoofedConvar* meme_spoofed = new SpoofedConvar(Meme);
	meme_spoofed->SetString("mat_postprocess_enable 0");
}

void CMiscHacks::MinimalWalk(CUserCmd* cmd, float speed)
{
	if (speed <= 0.f)
		return;

	float fSpeed = (float)(FastSqrt(square(cmd->forwardmove) + square(cmd->sidemove) + square(cmd->upmove)));

	if (fSpeed <= 0.f)
		return;

	if (cmd->buttons & IN_DUCK)
		speed *= 2.8;

	if (fSpeed <= speed)
		return;

	float fRatio = speed / fSpeed;

	cmd->forwardmove *= fRatio;
	cmd->sidemove *= fRatio;
	cmd->upmove *= fRatio;

	interfaces::globals->frametime *= (hackManager.plocal()->getvelocity().length2d()) / 1.5;
}
void CMiscHacks::fake_crouch(CUserCmd * cmd, bool &packet, IClientEntity * local)  // appears pasted
{
	if (GetAsyncKeyState(options::menu.misc.fake_crouch_key.GetKey()))
	{
		cmd->buttons |= IN_BULLRUSH;
		unsigned int chokegoal = options::menu.misc.fake_crouch_fakelag.GetValue() / 2;
		auto choke = interfaces::client_state->chokedcommands; // yea i don't get why ayy doesn't recognize the normal chokedcommands. i even checked to see if it was updated :c
		bool mexican_tryhard = choke >= chokegoal;

		if (local->GetFlags() & FL_ONGROUND)
		{
			if (mexican_tryhard)
				cmd->buttons |= IN_DUCK;
			else
				cmd->buttons &= ~IN_DUCK;
		}
	}
	else
		Sleep(2);
}
float CMiscHacks::get_gun(C_BaseCombatWeapon* weapon)
{

	if (!weapon)
		return 0.f;

	if (weapon->isAuto())
		return 38.f;

	else if (weapon->is_scout())
		return 69.f;

	else if (weapon->is_awp())
		return 29.f;

	else
		return 33.f;
}

void CMiscHacks::strafer(CUserCmd* cmd) {

	if (!GetAsyncKeyState(VK_SPACE) || hackManager.plocal()->getvelocity().length2d() < 0.5) {
		return;
	}

	if (!(hackManager.plocal()->GetFlags() & FL_ONGROUND))
	{
		static float cl_sidespeed = interfaces::cvar->FindVar("cl_sidespeed")->GetFloat();
		if (fabsf(cmd->mousedx > 2)) {
			cmd->sidemove = (cmd->mousedx < 0.f) ? -cl_sidespeed : cl_sidespeed;
			return;
		}

		/*
		if (GetAsyncKeyState('S')) {
			cmd->viewangles.y -= 180;
		}
		else if (GetAsyncKeyState('D')) {
			cmd->viewangles.y += 90;
		}
		else if (GetAsyncKeyState('A')) {
			cmd->viewangles.y -= 90;
		}
		*/

		if (!hackManager.plocal()->getvelocity().length2d() > 0.5 || hackManager.plocal()->getvelocity().length2d() == NAN || hackManager.plocal()->getvelocity().length2d() == INFINITE)
		{
			cmd->forwardmove = 400;
			return;
		}

		cmd->forwardmove = clamp(5850.f / hackManager.plocal()->getvelocity().length2d(), -400, 400);
		if ((cmd->forwardmove < -400 || cmd->forwardmove > 400))
			cmd->forwardmove = 0;

		const auto vel = hackManager.plocal()->getvelocity();
		const float y_vel = RAD2DEG(atan2(vel.y, vel.x));
		const float diff_ang = normalize_yaw(cmd->viewangles.y - y_vel);

		cmd->sidemove = (diff_ang > 0.0) ? -cl_sidespeed : cl_sidespeed;
		cmd->viewangles.y = normalize_yaw(cmd->viewangles.y - diff_ang);

	}
}

void CMiscHacks::viewmodel_x_y_z()
{
	bool sex = false; // my life

	static int vx, vy, vz;
	static ConVar* view_x = interfaces::cvar->FindVar("viewmodel_offset_x");
	static ConVar* view_y = interfaces::cvar->FindVar("viewmodel_offset_y");
	static ConVar* view_z = interfaces::cvar->FindVar("viewmodel_offset_z");

	static ConVar* bob = interfaces::cvar->FindVar("cl_bobcycle"); 

	if (!sex)
	{
		ConVar* sv_cheats = interfaces::cvar->FindVar("sv_cheats");
		SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
		sv_cheats_spoofed->SetInt(1);

		ConVar* sv_minspec = interfaces::cvar->FindVar("sv_competitive_minspec");
		SpoofedConvar* sv_minspec_spoofed = new SpoofedConvar(sv_minspec);
		sv_minspec_spoofed->SetInt(0);
	}

	view_x->nFlags &= ~FCVAR_CHEAT;
	view_y->nFlags &= ~FCVAR_CHEAT;
	view_z->nFlags &= ~FCVAR_CHEAT;

	vx = options::menu.visuals.offset_x.GetValue();
	vy = options::menu.visuals.offset_y.GetValue();
	vz = options::menu.visuals.offset_z.GetValue();

	view_x->SetValue(vx);
	view_y->SetValue(vy);
	view_z->SetValue(vz);

	if (!paste)
	{
		interfaces::engine->ExecuteClientCmd("cl_bobcycle 0.98"); // yeah no jittery hands thanks
		interfaces::engine->ExecuteClientCmd("rate 196608");
		paste = true;
	}
}

void CMiscHacks::optimize()
{
	bool done = false;

	if (!interfaces::engine->IsConnected() || !interfaces::engine->IsInGame())
	{
		done = false;
		return;
	}


	if (!done)
	{
		ConVar* sv_cheats = interfaces::cvar->FindVar("sv_cheats");
		SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
		sv_cheats_spoofed->SetInt(1);

		auto da2 = interfaces::cvar->FindVar("cl_disable_ragdolls"); 
		da2->SetValue(1);

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] cl_disable_ragdolls was set to 1.     \n");

		auto da3 = interfaces::cvar->FindVar("dsp_slow_cpu"); 
		da3->SetValue(1);

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] dsp_slow_cpu was set to 1.     \n");

		auto da5 = interfaces::cvar->FindVar("mat_disable_bloom");
		da5->SetValue(1);

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] mat_disable_bloom was set to 1.     \n");

//		auto da6 = interfaces::cvar->FindVar("r_drawparticles"); 
//		da6->SetValue(0);
		auto da7 = interfaces::cvar->FindVar("func_break_max_pieces");
		da7->SetValue(0);

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] func_break_max_pieces was set to 0.     \n");

		auto da8 = interfaces::cvar->FindVar("muzzleflash_light");
		da8->SetValue(0);

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] muzzleflash_light was set to 0.     \n");

		auto da9 = interfaces::cvar->FindVar("r_eyemove"); 
		da9->SetValue(0);

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] r_eyemove was set to 0.     \n");

		auto da10 = interfaces::cvar->FindVar("r_eyegloss");
		da10->SetValue(0);

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] r_eyegloss was set to 0.     \n");

		auto da11 = interfaces::cvar->FindVar("mat_queue_mode"); 
		da11->SetValue(2);

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] mat_queue_mode was set to 2.     \n");

		ConVar* Meme = interfaces::cvar->FindVar("mat_postprocess_enable");
		SpoofedConvar* meme_spoofed = new SpoofedConvar(Meme);
		meme_spoofed->SetString("mat_postprocess_enable 0");

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [info] post process was set to 0.     \n");

		a_c->mirror_aesthetic_console();
		interfaces::cvar->ConsoleColorPrintf(Color(10, 200, 250, 255), " [info] game graphics have been optimized.     \n");


//		auto xd80 = interfaces::cvar->FindVar("r_showenvcubemap"); 
//		xd80->SetValue(0);
//		auto xd81 = interfaces::cvar->FindVar("r_drawtranslucentrenderables");// <----
//		xd81->SetValue(0);
		done = true;

		options::menu.visuals.optimize.SetState(false);
	}
	//	Interfaces::Engine->ExecuteClientCmd("r_drawtranslucentrenderables 0");
}

void CMiscHacks::LoadNamedSky(const char *sky_name)
{
	static auto fnLoadNamedSkys = (void(__fastcall*)(const char*))game_utils::FindPattern1("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45");
	fnLoadNamedSkys(sky_name);
	anotherpcheck = false;
}
void CMiscHacks::colour_modulation()
{
	static bool freakware = false;

	bool once = false;
	if (options::menu.visuals.colmodupdate.getstate())
	{

		ConVar* staticdrop = interfaces::cvar->FindVar("r_DrawSpecificStaticProp");
		SpoofedConvar* staticdrop_spoofed = new SpoofedConvar(staticdrop);
		staticdrop_spoofed->SetInt(0);
		ConVar* NightSkybox1 = interfaces::cvar->FindVar("sv_skyname");
		*(float*)((DWORD)&NightSkybox1->fnChangeCallback + 0xC) = NULL;

		for (MaterialHandle_t i = interfaces::materialsystem->FirstMaterial(); i != interfaces::materialsystem->InvalidMaterial(); i = interfaces::materialsystem->NextMaterial(i))
		{
			IMaterial *pMaterial = interfaces::materialsystem->GetMaterial(i);
			interfaces::engine->ExecuteClientCmd("mat_queue_mode 2"); // rate 196608 dsp_slow_cpu

			if (!pMaterial)
				continue;
			if (pMaterial->IsErrorMaterial())
				continue;

			pMaterial->IncrementReferenceCount();

			float sky_r = options::menu.visuals.sky_r.GetValue() / 10;
			float sky_g = options::menu.visuals.sky_g.GetValue() / 10;
			float sky_b = options::menu.visuals.sky_b.GetValue() / 10;

			float test = options::menu.visuals.asusamount.GetValue() / 100;
			float amountr = options::menu.visuals.colmod.GetValue() / 100;

			switch (options::menu.visuals.customskies.getindex())
			{
			case 1:
			{
				NightSkybox1->SetValue("sky_csgo_night02b");
			}
			break;

			case 2:
			{
				NightSkybox1->SetValue("sky_l4d_rural02_ldr");
			}
			break;

			case 3:
			{
				LoadNamedSky("sky_descent");
			}
			break;
			}

			if (options::menu.visuals.ModulateSkyBox.getstate() && strstr(pMaterial->GetTextureGroupName(), ("SkyBox"))) {
				pMaterial->ColorModulate(sky_r, sky_g, sky_b);
			}

			if (!strcmp(pMaterial->GetTextureGroupName(), "World textures")) {
				options::menu.ColorsTab.asus_type.getindex() ? pMaterial->ColorModulation(amountr, amountr, amountr) : pMaterial->ColorModulation(amountr, amountr, amountr);
			}
			if (!strcmp(pMaterial->GetTextureGroupName(), "StaticProp textures")) {
				pMaterial->AlphaModulate(test);
				pMaterial->ColorModulation(amountr, amountr, amountr);
			}
			pMaterial->IncrementReferenceCount(); once = true;
		}
		options::menu.visuals.colmodupdate.SetState(false);
	}

	if (once && options::menu.visuals.colmod.GetValue() < 20.f)
	{

		static float storedCurtime = interfaces::globals->curtime;

		double timeSoFar = abs(interfaces::globals->curtime - storedCurtime);

		static int choose = 1;

		srand(time(NULL));

		if (timeSoFar > 19.0)
		{
			switch (choose)	{
			case 1: interfaces::engine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder4.wav"); 
				break;
			case 2: interfaces::engine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder5.wav"); 
				break;
			case 3: interfaces::engine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder6.wav"); 
				break;
			case 4: { interfaces::engine->ExecuteClientCmd("play ambient\\playonce\weather\\thunder_distant_01.wav"); 
				interfaces::cvar->ConsoleColorPrintf(Color(200, 200, 200, 255), " [easter egg] you have been ratted >:)     \n"); 
			} break;
			case 5: 
				interfaces::engine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_02.wav");
				break;
			case 6: 
				interfaces::engine->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_06.wav");
				break;
			}
			storedCurtime = interfaces::globals->curtime;
		}
		else {
			int randomnum = 1 + (rand() % 6);
			if (randomnum == choose)
				choose = 1 + (rand() % 6); // could still be the same number but less likely
			else
				choose = randomnum;
		}
	}
}

typedef void(*RevealAllFn)(int);
RevealAllFn fnReveal;
void CMiscHacks::RankReveal(CUserCmd * cmd)
{
	// re-do
}

// as i type this i just created the base functions, no code, and i can tell i'm gonna fall asleep right here
bool IBaseClientDLL::WriteUsercmdDeltaToBuffer(int nSlot, void* buf,
	int from, int to, bool isNewCmd)
{
	using Fn = bool(__thiscall*)(void*, int, void*, int, int, bool);
	return call_vfunc<Fn>(this, 24)(this, nSlot, buf, from, to, isNewCmd);
}

