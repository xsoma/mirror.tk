#include "includes.h"
#include "Hooks.h"

#include "Global.h"
#include "thirdperson.h"

std::string Tag = " mirror beta";
std::string Tag2 = "";

void set_clan_tag(const char* tag, const char* clan_name)
{
	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(Utilities::Memory::FindPatternV2(XorStr("engine.dll"), XorStr("53 56 57 8B DA 8B F9 FF 15")));
	pSetClanTag(tag, clan_name);
}

void clan_changer()
{
	auto m_local = hackManager.plocal();
	bool OOF = false;
	if (options::menu.ColorsTab.ClanTag.getstate())
	{
		if (!m_local || !m_local->IsAlive() || !interfaces::engine->IsInGame() || !interfaces::engine->connected())
		{
			if (!OOF)
			{
				Tag2 += Tag.at(0);
				Tag2.erase(0, 1);
				set_clan_tag(Tag2.c_str(), "FreaKratsKids");
			}
			else
			{
				OOF = true;
			}
		}
		static size_t lastTime = 0;

		if (GetTickCount() > lastTime)
		{
			OOF = false;
			Tag += Tag.at(0);
			Tag.erase(0, 1);
			set_clan_tag(Tag.c_str(), "FreaKratsKids");
			lastTime = GetTickCount() + 650;
		}
	}
}

struct BoneAccess
{
	BoneAccess()
	{
		bAllowBoneAccessForNormalModels = false;
		bAllowBoneAccessForViewModels = false;
		tag = nullptr;
	}

	bool bAllowBoneAccessForNormalModels;
	bool bAllowBoneAccessForViewModels;
	char const* tag;
};

BoneAccess* g_BoneAcessBase = nullptr;

void build_absyaw(IClientEntity* localplayer) 
{
	auto animstate = localplayer->get_animation_state();

	if (!animstate)
		return;

	float speedfraction = 0.0;
	if (animstate->m_flFeetSpeedForwardsOrSideWays < 0.0)
		speedfraction = 0.0;
	else
		speedfraction = animstate->m_flFeetSpeedForwardsOrSideWays;

	float v2 = (animstate->m_flStopToFullRunningFraction * -0.30000001 - 0.19999999) * speedfraction;
	float v18 = v2;
	float v3 = v2 + 1.0;
	float v23 = v3;
	float v8 = 0;
	float v7 = 0;
	float v6 = 0;

	float v20 = (animstate->velocity_subtract_z) * v23;
	float a1 = (animstate->velocity_subtract_y) * v23;
	float v30 = 0.0;
	float eye_angles_y = animstate->m_flEyeYaw;
	float goal_feet_yaw = animstate->goal_feet_yaw;
	float v22 = fabs(eye_angles_y - goal_feet_yaw);
	if (v20 < v22)
	{
		float v11 = fabs(v20);
		v30 = eye_angles_y - v11;
	}
	else if (a1 > v22)
	{
		float v12 = fabs(a1);
		v30 = v12 + eye_angles_y;
	}
	float v36 = std::fmodf((v30), 360.0);
	if (v36 > 180.0)
		v36 = v36 - 360.0;
	if (v36 < 180.0)
		v36 = v36 + 360.0;

	animstate->goal_feet_yaw = v36;
}

void MovementCorrection(CUserCmd* userCMD, IClientEntity* local)
{
	if (!local)
		return;

	if (userCMD->forwardmove) {
		userCMD->buttons &= ~(userCMD->forwardmove < 0 ? IN_FORWARD : IN_BACK);
		userCMD->buttons |= (userCMD->forwardmove > 0 ? IN_FORWARD : IN_BACK);
	}
	if (userCMD->sidemove) {
		userCMD->buttons &= ~(userCMD->sidemove < 0 ? IN_MOVERIGHT : IN_MOVELEFT);
		userCMD->buttons |= (userCMD->sidemove > 0 ? IN_MOVERIGHT : IN_MOVELEFT);
	}
}

bool CanFireWeapon(float curtime) {
	// the player cant fire.
	if (!global::m_player_fire || !global::m_weapon)
		return false;

	if (global::m_weapon->IsGrenade())
		return false;

	// if we have no bullets, we cant shoot.
	if (global::m_weapon_type != WEAPONTYPE_KNIFE && global::m_weapon->GetAmmoInClip() < 1)
		return false;

	// do we have any burst shots to handle?
	if ((global::m_weapon_id == WEAPON_GLOCK || global::m_weapon_id == WEAPON_FAMAS) && global::m_weapon->m_iBurstShotsRemaining() > 0) {
		// new burst shot is coming out.
		if (curtime >= global::m_weapon->m_fNextBurstShot())
			return true;
	}

	// r8 revolver. 
/*	if (global::m_weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER) {
		// mouse1.
		if (global::m_revolver_fire) {
			return true;
		}
		else {
			return false;
		}
	}	*/

	// yeez we have a normal gun.
	if (curtime >= global::m_weapon->GetNextPrimaryAttack())
		return true;

	return false;
}

bool IsFiring(float curtime) {
	const auto weapon = global::m_weapon;
	if (!weapon)
		return false;

	const auto IsZeus = global::m_weapon_id == WEAPON_ZEUS;
	const auto IsKnife = !IsZeus && global::m_weapon_type == WEAPONTYPE_KNIFE;

	if (weapon->IsGrenade())
		return !weapon->IsPinPulled() && weapon->GetThrowTime() > 0.f && weapon->GetThrowTime() < curtime;
	else if (IsKnife)
		return (global::cmd->buttons & (IN_ATTACK) || global::cmd->buttons & (IN_ATTACK2)) && CanFireWeapon(curtime);
	else
		return global::cmd->buttons & (IN_ATTACK) && CanFireWeapon(curtime);
}

C_BaseCombatWeapon* GetWeapon(IClientEntity* entity)
{
	return reinterpret_cast<C_BaseCombatWeapon*>(interfaces::ent_list->GetClientEntityFromHandle(global::local->GetActiveWeaponHandle()));
}

bool __stdcall Hooked_CreateMoveClient(float frametime, CUserCmd* pCmd)
{
	if (!pCmd || !pCmd->command_number)
		return true;

	global::cmd = pCmd;

	IClientEntity* plocal = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(plocal->GetActiveWeaponHandle());

	global::local = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer()); // cause life

	if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame())
	{

		pCmd->sidemove = pCmd->sidemove;
		pCmd->upmove = pCmd->upmove;
		pCmd->forwardmove = pCmd->forwardmove;

		if (GetAsyncKeyState(options::menu.misc.manual_right.GetKey()))
		{
			bigboi::indicator = 1;
		}
		else if (GetAsyncKeyState(options::menu.misc.manual_left.GetKey()))
		{
			bigboi::indicator = 2;
		}
		else if (GetAsyncKeyState(options::menu.misc.manualback.GetKey()))
		{
			bigboi::indicator = 3;
		}
		else if (GetAsyncKeyState(options::menu.misc.manualfront.GetKey()))
		{
			bigboi::indicator = 4;
		}

		if (GetAsyncKeyState(options::menu.misc.desyncleft.GetKey()))
		{
			bigboi::desyncside = 1;
		}
		else if (GetAsyncKeyState(options::menu.misc.desyncright.GetKey()))
		{
			bigboi::desyncside = 2;
		}

		overall::smt = frametime;
		overall::userCMD = pCmd;
		overall::vecUnpredictedVel = plocal->getvelocity();

		clan_changer();

	//	PVOID pebp;
	//	__asm mov pebp, ebp;
	//	bool* pbSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);
	//	bool& bSendPacket = *pbSendPacket;
	//	uintptr_t* framePtr;
	//	__asm mov framePtr, ebp;
	//	overall::bSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);

		uintptr_t* frame_pointer;
		__asm mov frame_pointer, ebp;
		 global::should_send_packet = reinterpret_cast<bool*>(*frame_pointer - 0x1C);
	//	*global::should_send_packet = true;

		if(options::menu.misc.FakelagBreakLC.getstate())
			*global::should_send_packet = interfaces::client_state->chokedcommands >= 14;

		if (plocal->GetFlags() & FL_ONGROUND)
			overall::TicksOnGround++;
		else
			overall::TicksOnGround = 0;

		// if ()  SHIFT TICKBASE
		// fixed_tickbase = local player tickbase - tick base shift amount
		// else
		overall::fixed_tickbase = plocal->GetTickBase();

		vector origView = pCmd->viewangles;
		vector viewforward, viewright, viewup, aimforward, aimright, aimup;
		vector qAimAngles;
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);
		if (globalsh.bSendPaket)
			globalsh.prevChoked = interfaces::client_state->chokedcommands;

		IClientEntity* pEntity;
		//	Vector ClientAngles; interfaces::engine->get_viewangles(ClientAngles);

		if (options::menu.misc.SniperCrosshair.getstate() && plocal->IsAlive() && !plocal->IsScoped())
		{
			if (plocal->weapon() && plocal->weapon()->m_bIsSniper()) {
				ConVar* cross = interfaces::cvar->FindVar("weapon_debug_spread_show");
				cross->nFlags &= ~FCVAR_CHEAT;
				cross->SetValue(3);
			}
		}
		else {
			ConVar* cross = interfaces::cvar->FindVar("weapon_debug_spread_show");
			cross->nFlags &= ~FCVAR_CHEAT;
			cross->SetValue(0);
		}
		if (plocal) // isconnected and is ingame original check location
		{
			if (plocal->IsAlive())
				overall::ticks_alive++;
			else
				overall::ticks_alive = 0.f;

			IBaseClientDLL* client = interfaces::client;
			uintptr_t createmove_client = (*(uintptr_t**)client)[21];
			g_BoneAcessBase = *reinterpret_cast<BoneAccess**>(createmove_client + 0x11);

			//	setupvelocity_gfy(plocal);
			build_absyaw(plocal);
			Prediction->StartPrediction(pCmd);
			{
				// reset everything 
				global::m_weapon = nullptr;
				global::m_weapon_info = nullptr;
				global::m_player_fire = global::m_weapon_fire = false;

				// store // Fix this and it won't crash!
		//		global::m_weapon = GetWeapon(global::local); //(C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

				if (global::m_weapon) // so we won't crash if it errors 
				{
				//	global::m_weapon_info = global::m_weapon->GetCSWpnData();
				//	global::m_weapon_id = global::m_weapon->test();
				//	global::m_weapon_type = global::m_weapon->get_weapon_type();

					// ensure weapon spread values / etc are up to date.
				//	if (!global::m_weapon->IsGrenade())
				//		global::m_weapon->UpdateAccPenalty();

				//	global::m_player_fire = TICKS_TO_TIME(global::local->m_nTickBase()) >= global::local->m_flNextAttack() && !(global::local->GetFlags() & FL_FROZEN);

				//	global::m_weapon_fire = CanFireWeapon(TICKS_TO_TIME(global::local->m_nTickBase()));
				}   // global variables kinda pog for this dog shit 

				//	linear_extraps.run();
				Hacks::MoveHacks(pCmd, *global::should_send_packet);

				// store command number at the time we fired.
			//	if (IsFiring(interfaces::globals->curtime)) 
				if(pCmd->buttons & IN_ATTACK)
					global::m_shot_command_number = pCmd->command_number;				
			}
			Prediction->EndPrediction(pCmd);


			// make sure to update our animations with the right angles. // this also crashes oof dog
		//	if (pCmd->command_number >= global::m_shot_command_number && global::m_shot_command_number >= pCmd->command_number - interfaces::client_state->chokedcommands)
		//		global::real_angles = interfaces::pinput->m_pCommands[global::m_shot_command_number % 150].viewangles;
		//	else
				global::real_angles = pCmd->viewangles;

			// set thirdperson angle // this is also how p2cs do it, not sup but how everything else did.
			if (!interfaces::client_state->chokedcommands) // also need to hook eye angles cause that's pog 
				thirdperson::get().set_tp_angle(global::real_angles); 

			if (pCmd->forwardmove)
			{
				pCmd->buttons &= ~(pCmd->forwardmove < 0 ? IN_FORWARD : IN_BACK);
				pCmd->buttons |= (pCmd->forwardmove > 0 ? IN_FORWARD : IN_BACK);
			}
			if (pCmd->sidemove) {
				pCmd->buttons &= ~(pCmd->sidemove < 0 ? IN_MOVERIGHT : IN_MOVELEFT);
				pCmd->buttons |= (pCmd->sidemove > 0 ? IN_MOVERIGHT : IN_MOVELEFT);
			}

			float flServerTime = (float)(plocal->GetTickBase() * interfaces::globals->interval_per_tick);
			static float next_time = 0;
			MovementCorrection(pCmd, plocal);
			qAimAngles.Init(0.0f, GetAutostrafeView().y, 0.0f);
			AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);
			qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
			AngleVectors(qAimAngles, &aimforward, &aimright, &aimup);
			vector vForwardNorm;		Normalize(viewforward, vForwardNorm);
			vector vRightNorm;			Normalize(viewright, vRightNorm);
			vector vUpNorm;				Normalize(viewup, vUpNorm);
			float forward = pCmd->forwardmove;
			float right = pCmd->sidemove;
			float up = pCmd->upmove;
			if (plocal->IsAlive())
			{
				if (forward > 450) forward = 450;
				if (right > 450) right = 450;
				if (up > 450) up = 450;
				if (forward < -450) forward = -450;
				if (right < -450) right = -450;
				if (up < -450) up = -450;
				pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
				pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
				pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);
			}

			if (options::menu.misc.OtherSafeMode.getindex() < 1)
			{
				game_utils::NormaliseViewAngle(pCmd->viewangles);
				if (pCmd->viewangles.z != 0.0f)
				{
					pCmd->viewangles.z = 0.00;
				}
				if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
				{
					Utilities::Log(" angles doing a nae nae hold up");
					game_utils::NormaliseViewAngle(pCmd->viewangles);

					pCmd->viewangles = origView;
					pCmd->sidemove = right;
					pCmd->forwardmove = forward;
				}
			}

			if (pCmd->viewangles.x > 90)
			{
				pCmd->forwardmove = -pCmd->forwardmove;
			}
			if (pCmd->viewangles.x < -90)
			{
				pCmd->forwardmove = -pCmd->forwardmove;
			}

			if (global::should_send_packet)
			{
				c_beam->duck = plocal->duck(); // quack
				c_beam->cham_origin = plocal->GetAbsOrigin();
				LastAngleAAFake = pCmd->viewangles;

				c_beam->fake = LastAngleAAFake;
			}
		}

		if (!global::should_send_packet)
		{
			LastAngleAAReal = pCmd->viewangles;
			c_beam->real = pCmd->viewangles.y;
		}

		lineLBY = plocal->get_lowerbody();
		lineLBY2 = LastAngleAAReal.y - plocal->get_lowerbody();

		switch (options::menu.visuals.optimize.getstate())
		{
		case true:
			c_misc->optimize();
			break;
		}

		if (plocal && plocal->IsAlive() && pWeapon != nullptr && !game_utils::IsGrenade(pWeapon) && !(pWeapon->isZeus() || pWeapon->IsC4()))
		{
			inaccuracy = pWeapon->GetInaccuracy() * 1000;
			lspeed = plocal->getvelocity().length2d();
			pitchmeme = pCmd->viewangles.x;
		}

		vector fl = plocal->GetAbsAngles();
		if (hackManager.plocal()->GetBasePlayerAnimState())
		{
			fl.y = LastAngleAAFake.y;
		}
		fl.z = 0.f;


		c_beam->cham_angle = fl;
	}
	return false;
}