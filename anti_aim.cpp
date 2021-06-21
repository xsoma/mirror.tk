#include "antiaim.h"
#include "Hooks.h"
#include "MathFunctions.h"
#include "RageBot.h"
#include "fakelag.h"
#include "MiscHacks.h"
anti_aim * c_antiaim = new anti_aim();
antiaim_helper * c_helper = new antiaim_helper();

static bool dir = false;
static bool back = false;
static bool up = false;
static bool d_right = false;
static bool jitter = false;
static bool jitter2 = false;
inline float RandomFloat(float min, float max)
{
	static auto fn = (decltype(&RandomFloat))(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
	return fn(min, max);
}


#define MASK_SHOT_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)

void anti_aim::run_manual_inputs()
{
	
	if (GetAsyncKeyState(options::menu.misc.manual_right.GetKey())) // right
	{
	//	dir = true;
	//	back = false;
	//	up = false;
		manual_index = 1;
		bigboi::indicator = 1;
	}

	if (GetAsyncKeyState(options::menu.misc.manual_left.GetKey())) // left
	{
	//	dir = false;
	//	back = false;
	//	up = false;
		manual_index = 2;
		bigboi::indicator = 2;
	}

	if (GetAsyncKeyState(options::menu.misc.manualback.GetKey()))
	{
	//	dir = false;
	//	back = true;
	//	up = false;
		manual_index = 3;
		bigboi::indicator = 3;
	}

	if (GetAsyncKeyState(options::menu.misc.manualfront.GetKey()))
	{
	//	dir = false;
	//	back = false;
	//  up = true;
		manual_index = 4;
		bigboi::indicator = 4;
	}

}

bool anti_aim::extent_moving_desync(CUserCmd* cmd)
{
	auto local_player = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = interfaces::globals->curtime;

	auto animstate = local_player->get_animation_state();
	if (!animstate)
		return false;

	if (!(local_player->GetFlags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d < 0.1f) {
		next_lby_update_time = curtime + 0.5f;
		return false;
	}

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}

bool anti_aim::next_lby_update(const float yaw_to_break, CUserCmd* cmd)
{
	auto local_player = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = interfaces::globals->curtime;

	auto animstate = local_player->get_animation_state();
	if (!animstate)
		return false;

	if (!(local_player->GetFlags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d > 0.1f)
		next_lby_update_time = curtime + 0.22f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------
void anti_aim::DoYaw(CUserCmd* pCmd, IClientEntity* plocal, bool &bSendPacket)
{

	if ((pCmd->buttons & IN_ATTACK) && ragebot->CanOpenFire(plocal))
		return;

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

	bool standing = !plocal->IsMoving();
	bool on_ground = (plocal->GetFlags() & FL_ONGROUND);

	switch (on_ground)
	{
		case true:
		{
			switch (standing)
			{
				case true: selection(pCmd, false, false, plocal);
					break;
				case false: selection(pCmd, true, false, plocal);
					break;
			}
		}
		break;

		case false:
		{
			selection(pCmd, true, true, plocal);
		}
		break;
	}

}

float anti_aim::yaw_mod(int m, int a)
{
	if (!m || m == 0.f)
		return 0.f;

	switch (options::menu.misc.modifier.getindex())
	{
		case 0:
		{
			return m;
		}
		break;

		case 1:
		{
			return RandomFloat(m, -m);
		}
		break;

		case 2:
		{
			return (m < 0 ? -a : a) + fmodf(interfaces::globals->tickcount * 4, m * 2);
		}
		break;

		case 3:
		{
			static bool b = false;
			b = !b;

			return b ? m : 0;
		}
		break;

		case 4:
		{
			static bool b = false;
			b = !b;

			return b ? m : -m;
		}
		break;
	}
}

void anti_aim::selection(CUserCmd * pcmd, bool moving, bool air, IClientEntity * plocal) // if (is_oxygen)
{
	auto l = pcmd->tick_count % 2;
	int m = plocal->IsMoving() ? options::menu.misc.move_jitter.GetValue() : options::menu.misc.stand_jitter.GetValue();

	float view = 0.f;
	if (options::menu.misc.at_targets.getstate()) {
		int index = c_helper->closest();
		IClientEntity* entity = interfaces::ent_list->get_client_entity(index);

		if (entity->isValidPlayer())
			view = Math_trash::CalcAngleTrash(entity->GetAbsOrigin(), plocal->GetVecOrigin_2()).y;
		else view = 0;
	}

	switch (air)
	{
		case true:
		{
			switch (options::menu.misc.antiaim_air.getindex())
			{
				case 1:
				{
					view = 179.9f + yaw_mod(m, -m); 
				}
				break;

				case 2:
				{
					switch (manual_index)
					{
					case 1: view = 90.f + yaw_mod(m, -m);
						break;
					case 2: view = -90.f + yaw_mod(m, -m);
						break;
					case 3: view = 180.f + yaw_mod(m, -m);
						break;
					case 4: view = 0.f + yaw_mod(m, -m);
						break;
					}
				}
				break;

				case 3:
				{
					freestanding(pcmd);	
				}
				break;

				case 4:
				{
					float smh = view -= 145;
					view = smh - fmodf(interfaces::globals->tickcount * (rand() % 10), 70.f) + yaw_mod(m, -m);;
			
				}
				break;
			}
		}
		break;

		case false:
		{
			switch (moving)
			{
				case true:
				{
					switch (options::menu.misc.antiaim_move.getindex())
					{
					case 1:
					{
						view = 179.9f + yaw_mod(m, -m);
					}
					break;

					case 2:
					{
						switch (manual_index)
						{
						case 1: view = 90.f + yaw_mod(m, -m);
							break;
						case 2: view = -90.f + yaw_mod(m, -m);
							break;
						case 3: view = 180.f + yaw_mod(m, -m);
							break;
						case 4: view = 0.f + yaw_mod(m, -m);
							break;
						}
					}
					break;

					case 3:
					{
						freestanding(pcmd);
					}
					break;

					case 4:
					{
						float smh = view -= 145;
						view = smh - fmodf(interfaces::globals->tickcount * (rand() % 10), 70.f) + yaw_mod(m, -m);;

					}
					break;
					}
				}
				break;

				case false:
				{
					switch (options::menu.misc.antiaim_stand.getindex())
					{
					case 1:
					{
						view = 179.9f + yaw_mod(m, -m);
					}
					break;

					case 2:
					{
						switch (manual_index)
						{
						case 1: view = 90.f + yaw_mod(m, -m);
							break;
						case 2: view = -90.f + yaw_mod(m, -m);
							break;
						case 3: view = 180.f + yaw_mod(m, -m);
							break;
						case 4: view = 0.f + yaw_mod(m, -m);
							break;
						}
				
					}
					break;

					case 3:
					{
						freestanding(pcmd);
					}
					break;

					case 4:
					{
						float smh = view -= 145;
						view = smh - fmodf(interfaces::globals->tickcount * (rand() % 10), 70.f) + yaw_mod(m, -m);;

					}
					break;
					}
				}
				break;
			}
		}
		break;
	}

	pcmd->viewangles.y -= view;
	c_beam->visual_angle = pcmd->viewangles.y;
}

void anti_aim::DoPitch(CUserCmd * pCmd)
{
	IClientEntity* pLocal = hackManager.plocal();

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

	if (pCmd->buttons & IN_ATTACK)
		return;

	switch (options::menu.misc.AntiAimPitch.getindex())
	{
		case 1: 
		{
			if (pCmd->buttons & IN_ATTACK)
				return;

			pCmd->viewangles.x = 89.9f;
		}
		break;
		case 2: 
		{
			if (pCmd->buttons & IN_ATTACK)
				return;

			pCmd->viewangles.x = -89.f;
		}
		break;
		case 3: 
		{
			pCmd->viewangles.x = 179.f;
		}
		break;

		case 4: 
		{
			if (pCmd->buttons & IN_ATTACK)
				return;

			pCmd->viewangles.x = RandomFloat(89.f, -89.f);
		}
		break;
	}
}

#include "Autowall.h"
bool anti_aim::_do()
{
	std::vector<dropdownboxitem> condition = options::menu.misc.backup_aa_fac.items;	
	if (condition[0].GetSelected && GetAsyncKeyState(options::menu.misc.backup_aa.GetKey()))
		return true;
	if (condition[1].GetSelected && overall::ticks_while_unducked <= 1)
		return true;
	if (condition[2].GetSelected && overall::ticks_while_unducked != 0 && overall::ticks_while_unducked < 9)
		return true;
	if (condition[3].GetSelected && backup_awall->wall_damage >= 1)
		return true;
	if (condition[4].GetSelected)
		return true;

	return false;
}

void anti_aim::DoAntiAim(CUserCmd *pCmd, bool &packet)
{
	IClientEntity* pLocal = hackManager.plocal();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

	if (!pLocal || !pWeapon)
		return;

	if (c_misc->anotherpcheck) {
		c_beam->visual_angle = pCmd->viewangles.y;
		return;
	}

	if (pLocal->movetype() == MOVETYPE_LADDER || pLocal->movetype() == MOVETYPE_NOCLIP)
		return;

	if (pCmd->buttons & IN_USE)
	{
		static bool m = false;
		m = !m;
		c_beam->visual_angle = pCmd->viewangles.y;
		packet =  m ? true : false;
		return;
	}

	if (game_utils::IsGrenade(pWeapon) && pWeapon->GetThrowTime() > 0)
		return;

	if (options::menu.misc.disable_on_dormant.getstate())
	{
		if (c_helper->closest() == -1)
			return;
	}

	if (interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::MOUSE_LEFT) || (pWeapon->knife() && interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::MOUSE_RIGHT))) {
		c_beam->visual_angle = pCmd->viewangles.y;
		return;
	}

	if (pWeapon->isZeus27() && c_misc->do_zeus == true)
		return;

	bool duck = (pLocal->GetFlags() & FL_DUCKING);

	float last = pCmd->viewangles.y;
	auto max_rotation = maxdelta(pLocal);

	run_manual_inputs();
	if (_do())
	{
		if ( (pCmd->buttons & IN_ATTACK) && ragebot->CanOpenFire(pLocal) )
			return;

		switch (options::menu.misc.fakeangle_compaitibility.getstate())
		{
			case true:
			{
				pCmd->viewangles.x = 89.f;
				if (packet)
					pCmd->viewangles.y = c_beam->real - 179.9f + RandomFloat(max_rotation, -max_rotation);
				else
					freestanding(pCmd);
			}
			break;

			case false:
			{
				int paste = -1;
				switch (bigboi::indicator)
				{
					case 1: paste = 1; // right
						break;
					case 2: paste = -1; // left
						break;
				}
				pCmd->viewangles.x = 89.f;

				auto flick = false;
				if (interfaces::client_state->chokedcommands == 1) {
					packet = 1; flick = true;
				}

				if (max_rotation != 0.f)
					max_rotation -= 0.5f;
				if (pCmd->command_number % 4 < 2)
					max_rotation *= 0.6f;

				static bool invert_jitter = false;
				if (invert_jitter) {
					pCmd->viewangles.y = last - 180;
					last = pCmd->viewangles.y;
				}

				auto align = options::menu.misc.real_align.GetValue() / 10; // from 5 to 9. base is and should be 5 tbh
				auto body_flick = 90.f;
				if (packet) {
					invert_jitter = !invert_jitter;
					if (!invert_jitter)
						pCmd->viewangles.y = (last + (max_rotation * align) * paste);
					else
						pCmd->viewangles.y = (last - (max_rotation * align) * paste);
					c_beam->visual_angle = pCmd->viewangles.y;
				}
				else if (!flick) {
					if (invert_jitter)
						pCmd->viewangles.y = (last - (max_rotation + 5.0f) * paste);
					else
						pCmd->viewangles.y = (last + (max_rotation + 5.0f) * paste);
				}
				else {
					if (invert_jitter) {
						pCmd->viewangles.y = (last + body_flick * paste);
					}
					else {
						pCmd->viewangles.y = (last - body_flick * paste);
					}
				}
			}
			break;
		}
		
	}
	else
	{
		DoPitch(pCmd);
		do_yawangle(pCmd, packet, hackManager.plocal()->IsMoving(), hackManager.plocal());
		switch (options::menu.misc.antilby.getindex())
		{
			case 1:
			{
				if (options::menu.misc.fakeangle_compaitibility.getstate())
					anti_lby2(pCmd, packet);
				else
				{
					if (next_lby_update(0, pCmd)) {
						packet = false;
						pCmd->viewangles.y = c_beam->fake.y - (58 + (1.35 * options::menu.misc.antilbymod.GetValue()));
					}
				}
			}
			break;
			case 2:
			{
				bool pause = pCmd->command_number % 3 == 2;
				float c = duck ? (3.25 + (options::menu.misc.antilbymod.GetValue() / 100)) : (1.01 + (options::menu.misc.antilbymod.GetValue() / 100));

				if (!(GetAsyncKeyState(VK_SPACE)) && !(pCmd->buttons & IN_FORWARD) && !(pCmd->buttons & IN_BACK) && !pause) { // congrats, you not have micro movements, oh no wait "jitter walk". Smh ot... b1g name
					pCmd->forwardmove = pause ? 0 : pCmd->tick_count % 2 ? c : -c;
				}
			}
			break;
		}

		if (options::menu.misc.extend.getstate()) {
			if (extent_moving_desync(pCmd))
				pCmd->viewangles.y = c_beam->real - 165;
		}
	}
}

static bool lbyupdate = false;
static bool wasMoving = true;
static bool preBreak = false;
static bool shouldBreak = false;
static bool brokeThisTick = false;
static bool fake_walk = false;
static int chocked = 0;
static bool gaymode = false;
static bool doubleflick = false;
static bool has_broken = false;
bool is_brokenx;
float NextPredictedLBYUpdate = 0.f;

#include "chrono"

int anti_aim::GetFPS()
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
void anti_aim::anti_lby2(CUserCmd* cmd, bool& bSendPacket)
{
	IClientEntity* local_player = hackManager.plocal();
	if (!local_player)
		return;

	auto local_weapon = local_player->GetActiveWeaponHandle();

	if (!local_weapon)
		return;

	float b = rand() % 4;

	static float oldCurtime = interfaces::globals->curtime;

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

	if (!pWeapon)
		return;

	if (bSendPacket)
	{
		brokeThisTick = false;
		chocked = *(int*)(uintptr_t(interfaces::client_state) + 0x4D28);

		if (local_player->getvelocity().length2d() >= 0.1f && (local_player->GetFlags() & FL_ONGROUND))
		{
			if (GetAsyncKeyState(VK_SHIFT))
			{
				wasMoving = false;
				oldCurtime = interfaces::globals->curtime;
				if (interfaces::globals->curtime - oldCurtime >= 1.1f)
				{
					lbyupdate = true;
					shouldBreak = true;
					NextPredictedLBYUpdate = interfaces::globals->curtime;
				}
			}
			else
			{
				oldCurtime = interfaces::globals->curtime;
				wasMoving = true;
				has_broken = false;
				is_brokenx = false;
			}
		}

		else
		{
			if (wasMoving &&interfaces::globals->curtime - oldCurtime > 0.22f)
			{
				wasMoving = false;
				lbyupdate = true;
				shouldBreak = true;
				NextPredictedLBYUpdate = interfaces::globals->curtime;
			}

			else if (interfaces::globals->curtime - oldCurtime > 1.1f)
			{
				shouldBreak = true;
				NextPredictedLBYUpdate = interfaces::globals->curtime;
			}

			else if (interfaces::globals->curtime - oldCurtime > 1.1f - TICKS_TO_TIME(chocked) - TICKS_TO_TIME(2))
			{
				lbyupdate = true;
				preBreak = true;
			}

			else if (interfaces::globals->curtime - oldCurtime > 1.f - TICKS_TO_TIME(chocked + 12))
				doubleflick = true;

			else
			{
				lbyupdate = false;
			}
		}
	}
	else if (shouldBreak)
	{
		static int choked = 0;

		if (!(local_player->GetFlags() & FL_DUCKING))
		{
			oldCurtime = interfaces::globals->curtime;
			cmd->viewangles.x = -70.f + RandomFloat(-15, 35);
			shouldBreak = false;
		}

		float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.9 * max(choked, overall::prevChoked) + 100) : 145.f;

		if (is_brokenx)
		{
			brokeThisTick = true;
			oldCurtime = interfaces::globals->curtime;
			cmd->viewangles.y = cmd->viewangles.y - 90 - addAngle;
			shouldBreak = false;
		}
		else
		{
			brokeThisTick = true;
			oldCurtime = interfaces::globals->curtime;
			cmd->viewangles.y = cmd->viewangles.y + 90 + addAngle;
			is_brokenx = true;
			shouldBreak = false;
		}

	}

	else if (preBreak)
	{
		brokeThisTick = true;
		float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.3789 * max(chocked, overall::prevChoked) + 90) : 145;
		cmd->viewangles.y = cmd->viewangles.y + 135;
		preBreak = false;
	}

	
}

float normalize_yaw180(float yaw)
{
	if (yaw > 180)
		yaw -= (round(yaw / 360) * 360.f);
	else if (yaw < -180)
		yaw += (round(yaw / 360) * -360.f);

	return yaw;
}

float anti_aim::maxdelta(IClientEntity * player)
{
	if (player != hackManager.plocal())
	{
		auto animstate = player->GetBasePlayerAnimState();

		if (!animstate)
			return 1.f;
		float v1; // xmm0_4
		float v2; // xmm1_4
		float v3; // xmm0_4
		int v4; // eax
		float v5; // xmm4_4
		float v6; // xmm2_4
		float v7; // xmm0_4
		int v8; // eax
		float v10; // [esp+0h] [ebp-Ch]
		float v11; // [esp+4h] [ebp-8h]

		v1 = animstate->m_flFeetSpeedForwardsOrSideWays;
		v2 = 1.0;
		v10 = 0.0;
		v11 = v1;
		if (v1 <= 1.f)
		{
			v4 = v10;
			if (v1 >= 0.0)
				v4 = v11;
			v3 = v4;
		}
		else
			v3 = 1.f;

		v5 = animstate->m_fDuckAmount;
		v6 = ((animstate->m_flStopToFullRunningFraction * -0.30000001f) - 0.19999999f) * v3 + 1.0f;
		if (v5 > 0.0)
		{
			v7 = animstate->m_flFeetSpeedUnknownForwardOrSideways;
			v11 = 0.0;
			v10 = v7;
			if (v7 <= 1.0)
			{
				v8 = v11;
				if (v7 >= 0.0)
					v8 = v10;
				v2 = v8;
			}
			v6 = v6 + (float)((float)(v2 * v5) * (float)(0.5f - v6));
		}

		return (animstate->velocity_subtract_y * v6);
	}
	else
		return (0.58f * ((player->IsMoving() ? options::menu.misc.desync_range_move.GetValue() : options::menu.misc.desync_range_stand.GetValue()) - .1f));
}
		

void anti_aim::do_yawangle(CUserCmd* cmd, bool & packet, bool moving, IClientEntity * local)
{
	vector real;

	int choke = *(int*)(uintptr_t(interfaces::client_state) + 0x4D28);
	bool fakeangles = options::menu.misc.fakeangle_compaitibility.getstate();

	static auto sw = false;
//	float move = options::menu.misc.desync_range_move.GetValue();
//	float stand = options::menu.misc.desync_range_stand.GetValue();
	float desync = moving ? options::menu.misc.desync_range_move.GetValue() : options::menu.misc.desync_range_stand.GetValue();

	/*
	IClientEntity * ent = options::menu.misc.desync_swap.getindex() > 1 ? local : ragebot->pTarget;
	if (options::menu.misc.desync_swap.getindex() > 0)
	{
		if (ent->isValidPlayer() && ent->weapon()) {
			int idx = ent->GetIndex();
			int cur[65] = { ent->weapon()->GetAmmoInClip() };
			static int ammo[65];
			static int next[65];

			if (ammo[idx] != cur[idx])
			{
				next[idx] = ent->weapon()->GetAmmoInClip();

				if (cur[idx] - next[idx] != 0) {
					desync = -desync;
					if (cur[idx] - next[idx] >= 2)
						desync = -desync * -1;
				}

			}
		}
	}
	*/

	float getmax = moving ? maxdelta(local) * (options::menu.misc.desync_range_move.GetValue() / 100) : maxdelta(local) * (options::menu.misc.desync_range_stand.GetValue() / 100); // there's a nasty bug ok

	real.y = c_beam->real;
	if (options::menu.misc.fakeangle_compaitibility.getstate() && packet == false) // don't @ me 
		DoYaw(cmd, local, packet);
	else
	{
		auto flick = false;
		static bool invert_jitter = false;
		
		if ((fakeangles ? !packet : packet))
			DoYaw(cmd, local, packet);
		else {
			switch (moving)
			{
			case true:
			{
				switch (options::menu.misc.desync_type_move.getindex())
				{
				case 0:
				{
					fakeangles ? 0 : DoYaw(cmd, local, packet);
				}
				break;

				case 1:
				{
					fakeangles ? cmd->viewangles.y = real.y - 135 : cmd->viewangles.y = c_beam->visual_angle - (getmax);
				}
				break;

				case 2:
				{
					const auto y = real.y - 120.f;
					fakeangles ? cmd->viewangles.y = real.y + y + fmodf(interfaces::globals->tickcount * 10, 120.f) : cmd->viewangles.y = (real.y + getmax) + RandomFloat(-getmax, 20);
				}
				break;

				case 3:
				{
					if (fakeangles) {
						cmd->viewangles.y = (real.y - 180.f) + RandomFloat(-90, 90);
					}
					else {
						auto max_rotation = maxdelta(local);
						if (max_rotation != 0.f)
							max_rotation -= 0.5f;
						if (cmd->command_number % 4 < 2)
							max_rotation *= 0.6f;

						float last = real.y;

						if (invert_jitter) {
							cmd->viewangles.y = last - 180;
							last = real.y;
						}

						if (packet) {
							invert_jitter = !invert_jitter;
							if (!invert_jitter)
								cmd->viewangles.y = (last + (max_rotation * 0.5f) * -1.1f);
							else
								cmd->viewangles.y = (last - (max_rotation * 0.5f) * -1.1f);
						}
						if (!flick) {
							if (invert_jitter)
								cmd->viewangles.y = (last - (max_rotation + 5.0f) * -1.f);
							else
								cmd->viewangles.y = (last + (max_rotation + 5.0f) * -1.f);
						}
						else {
							if (invert_jitter) {
								cmd->viewangles.y = (last + 90.0f * -1);
							}
							else {
								cmd->viewangles.y = (last - 90.0f * -1);
							}
						}
					}
				}
				break;

				}
			}
			break;

			case false:
			{
				switch (options::menu.misc.desync_type_stand.getindex()) {
				case 0:
				{
					fakeangles ? 0 : DoYaw(cmd, local, packet);
				}
				break;

				case 1:
				{
					fakeangles ? cmd->viewangles.y = real.y - 135 : cmd->viewangles.y = c_beam->visual_angle - (getmax + 30);
				}
				break;

				case 2:
				{
					const auto y = real.y - 120.f;
					fakeangles ? cmd->viewangles.y = real.y + y + fmodf(interfaces::globals->tickcount * 10, 120.f) : cmd->viewangles.y = (real.y + getmax) + RandomFloat(-getmax, 20);
				}
				break;

				case 3:
				{
					if (fakeangles) {
						cmd->viewangles.y = (real.y - 180.f) + RandomFloat(-90, 90);
					}
					else {
						auto max_rotation = maxdelta(local);
						if (max_rotation != 0.f)
							max_rotation -= 0.5f;
						if (cmd->command_number % 4 < 2)
							max_rotation *= 0.6f;

						float last = real.y;

						if (invert_jitter) {
							cmd->viewangles.y = last - 180;
							last = real.y;
						}

						if (packet) {
							invert_jitter = !invert_jitter;
							if (!invert_jitter)
								cmd->viewangles.y = (last + (max_rotation * 0.5f) * -1.1f);
							else
								cmd->viewangles.y = (last - (max_rotation * 0.5f) * -1.1f);
						}
						if (!flick) {
							if (invert_jitter)
								cmd->viewangles.y = (last - (max_rotation + 5.0f) * -1.f);
							else
								cmd->viewangles.y = (last + (max_rotation + 5.0f) * -1.f);
						}
						else {
							if (invert_jitter) {
								cmd->viewangles.y = (last + 90.0f * -1);
							}
							else {
								cmd->viewangles.y = (last - 90.0f * -1);
							}
						}
					}
				}
				break;

				}
			}

			break;
			}
		}
	}

	if ((fakeangles ? !packet : packet) && c_beam->real != real.y)
		real.y = c_beam->real;
}

void anti_aim::freestanding(CUserCmd* cmd)
{
	IClientEntity* GetLocalPlayer = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	static int Ticks = 0;

	bool no_active = true;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	static float hold = 0.f;
	vector besthead;
	float opposite = 0.f;

	auto leyepos = hackManager.plocal()->GetOrigin_likeajew() + hackManager.plocal()->GetViewOffset();
	auto origin = hackManager.plocal()->GetOrigin_likeajew();

	auto checkWallThickness = [&](IClientEntity* pPlayer, vector newhead) -> float
	{

		vector endpos1, endpos2;

		vector eyepos = pPlayer->GetOrigin_likeajew() + pPlayer->GetViewOffset();
		Ray_t ray;
		ray.Init(newhead, eyepos);
		CTraceFilterSkipTwoEntities filter(pPlayer, hackManager.plocal());

		trace_t trace1, trace2;
		interfaces::trace->TraceRay(ray, MASK_SHOT, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.endpos;
		else
			return 0.f;

		ray.Init(eyepos, newhead);
		interfaces::trace->TraceRay(ray, MASK_SHOT, &filter, &trace2);

		//	UTIL_TraceLine(data.src, End_Point, 0x4600400B, local, 0, &data.enter_trace);

		if (trace2.DidHit())
			endpos2 = trace2.endpos;

		float add = newhead.Dist(eyepos) - leyepos.Dist(eyepos) + 3.f;
		return endpos1.Dist(endpos2) + add / 3;

	};

	int index = c_helper->closest();
	static IClientEntity* entity;

	if (index != -1)
		entity = interfaces::ent_list->get_client_entity(index); // maybe?

	vector headpos;
	if (!entity->isValidPlayer())
	{
		cmd->viewangles.y -= 180.f;
		return;
	}
	headpos = hitbox_location(GetLocalPlayer, 0);
	float radius = vector(headpos - origin).length2d();

	if (index < 0)
	{
		no_active = true;
	}
	else
	{
		for (float besthead = 0; besthead < 7; besthead += 0.1)
		{
			vector newhead(radius * cos(besthead) + leyepos.x, radius * sin(besthead) + leyepos.y, leyepos.z);
			float totalthickness = 0.f;
			no_active = false;
			totalthickness += checkWallThickness(entity, newhead);
			if (totalthickness > highestthickness)
			{
				highestthickness = totalthickness;
				opposite = besthead - 180;
				bestrotation = besthead;
			}
		}
	}

	if (no_active)
	{
		cmd->viewangles.y -= 180.f;
		c_beam->visual_angle = cmd->viewangles.y;
	}

	else
	{
		float best = RAD2DEG(bestrotation);
		static float lol = FLT_MAX;
		if (lol - best < -60.f || lol - best > 60.f)
			lol = best;
		if (lol == FLT_MAX)
			lol = best;
		static bool l = false;	 l = !l;
		float m = hackManager.plocal()->IsMoving() ? options::menu.misc.move_jitter.GetValue() : options::menu.misc.stand_jitter.GetValue();

		cmd->viewangles.y = lol + yaw_mod(m, -m);
		game_utils::NormaliseViewAngle(cmd->viewangles);
	}
	c_beam->visual_angle = cmd->viewangles.y;
}
