#include "animations.h"
#include "Hooks.h"
#include "fakelag.h"
std::unique_ptr< c_animfix > animfix = std::make_unique< c_animfix >();

void c_animfix::re_work(ClientFrameStage_t stage) // i'm really out of motivation to try, so, pasted from a friend.
{
	auto local_player = hackManager.plocal();
	if (!local_player || !local_player->get_animation_state())
		return;

	if (stage == ClientFrameStage_t::FRAME_RENDER_START)
		local_player->SetAbsAngles(vector(0.f, local_player->get_animation_state()->goal_feet_yaw, 0.f));


	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		IClientEntity * entity = (IClientEntity*)interfaces::ent_list->get_client_entity(i);

		if (!entity->isValidPlayer())
			continue;


		static auto set_interpolation_flags = [](IClientEntity* e, int flag) {
			const auto var_map = (uintptr_t)e + 36;
			const auto sz_var_map = *(int*)(var_map + 20);

			for (auto index = 0; index < sz_var_map; index++)
				*(uintptr_t*)((*(uintptr_t*)var_map) + index * 12) = flag;
		};

		if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END)
			set_interpolation_flags(entity, 0);

	}
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

#define TICK_INTERVAL			(interfaces::globals->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

void newtick(bool yes)
{
	static int ticks;
	if (interfaces::globals->tickcount != ticks) {
		yes = true;
		ticks = interfaces::globals->tickcount;
	}
	else
		yes = false;
}
void newsim(IClientEntity * local, bool yes)
{
	static float sim;
	if (local->get_simulation_time() != sim) {
		yes = true;
		sim = local->get_simulation_time();
	}
	else
		yes = false;
}
#include "antiaim.h"
void c_animfix::fix_local_player_animations(CUserCmd * pcmd)
{
	const auto local_player = hackManager.plocal();
	if (!local_player || !local_player->IsAlive())	
		return;

	bool inuse = (local_player->GetFlags() & IN_USE);

	auto state = local_player->get_animation_state();
	if (!state)
		return;
	
	auto& info = player_data[local_player->GetIndex()];
	if ( /*(local_player->spawntime() != info.spawn_time || local_player->GetRefEHandle() != info.handle) && overall::ticks_alive < 16 */ local_player->spawntime() != info.spawn_time)
	{
		info.spawn_time = local_player->spawntime();
		info.handle = local_player->GetRefEHandle();

		if (info.animstate)
		{
			interfaces::mem_alloc->free(info.animstate);
			info.animstate = nullptr;
		}
	}

	if (!info.animstate && overall::ticks_alive < 16)
	{
		info.animstate = reinterpret_cast< CBaseAnimState* > (interfaces::mem_alloc->alloc(860u));
		local_player->create_animation_state(info.animstate);
	} // fucks sake i was looking for this shit for ages and then i found out i had it the entire time in a lost file

	auto old_duck = local_player->duck();
	local_player->duck() = c_beam->duck; // quack

	static auto fl_proper_abs = state->goal_feet_yaw;
	static auto sent_pose_params = local_player->m_flPoseParameter();
	const auto backup_frametime = interfaces::globals->frametime;
	const auto backup_curtime = interfaces::globals->curtime;

	CAnimationLayer client_layers[13];

	static int ticks;
	static float sim;

	if (interfaces::globals->tickcount != ticks) 
	{
		interfaces::globals->curtime = local_player->get_simulation_time();
		interfaces::globals->frametime = interfaces::globals->interval_per_tick;
		if (local_player->get_simulation_time() != sim)
			std::memcpy(client_layers, local_player->get_anim_overlays(), sizeof(CAnimationLayer) * local_player->num_anim_overlays());

		local_player->client_side_animation() = true;
	//	interfaces::globals->curtime = local_player->m_flOldSimulationTime() + interfaces::globals->interval_per_tick;
	//	interfaces::globals->frametime = interfaces::globals->interval_per_tick;

		const auto backup_origin = local_player->GetOrigin();
		const auto backup_absvelocity = local_player->get_abs_velocity();
		
		state->m_onground = local_player->GetFlags() & FL_ONGROUND;

//		if (!interfaces::client_state->chokedcommands) {
//			local_player->get_pose_paramaters().at(6) = 0;
//		}
		if (state->m_last_csanim_framecount >= interfaces::globals->framecount) {
			state->m_last_csanim_framecount = interfaces::globals->framecount - 1;
		}
		if (local_player->get_simulation_time() != sim)
			local_player->UpdateClientSideAnimation();

		if (overall::bSendPacket) {
			fl_proper_abs = state->goal_feet_yaw; 
			sent_pose_params = local_player->m_flPoseParameter();
		}
		local_player->GetOrigin2() = backup_origin;
		local_player->get_abs_velocity() = backup_absvelocity; // tag me @ disc and tell me if this is in any way useful, i can't tell tbh

		state->m_flFeetYawRate = 0.f;
		state->m_flUnknownFraction = 0.f;
		state->m_flFeetCycle = client_layers[6].m_flCycle;

	//	interfaces::globals->curtime = local_player->m_flOldSimulationTime() + interfaces::globals->interval_per_tick;
	//	interfaces::globals->frametime = interfaces::globals->interval_per_tick;

		state->goal_feet_yaw = fl_proper_abs;
		ticks = interfaces::globals->tickcount;
	}
	local_player->duck() = old_duck;
	local_player->client_side_animation() = false;
	state->m_flFeetYawRate = 0.f;
	local_player->m_flPoseParameter() = sent_pose_params;

//	if (local_player->get_simulation_time() != sim)
//		std::memcpy(local_player->get_anim_overlays(), client_layers, (sizeof(CAnimationLayer) * local_player->GetNumAnimOverlays()));

	local_player->set_abs_angles_v2(vector(0, c_beam->visual_angle, 0.f));
	interfaces::globals->curtime = backup_curtime;
	interfaces::globals->frametime = backup_frametime;
	sim = local_player->get_simulation_time();

}

#include "Resolver.h"
void c_animfix::update_animations(IClientEntity* player) 
{
	if (!player->valid_entity())
		return;

	auto state = player->get_animation_state(); 
	
	if (!state) 
		return;

	auto idx = player->GetIndex(); // lazy.png
	static float sim_time[65];

	const auto origin = player->GetOrigin();
	const auto vel = player->getvelocity();
	const auto absvel = player->get_abs_velocity();
	const auto duck = player->duck();
	const auto lby = player->get_lowerbody();
	const auto absorigin = player->GetAbsOrigin();
	const auto gfy = state->goal_feet_yaw;

	auto& info = player_data[idx];

	if (player->spawntime() != info.spawn_time || player->GetRefEHandle() != info.handle)
	{
		info.spawn_time = player->spawntime();
		info.handle = player->GetRefEHandle();

		if (info.animstate)
		{
			interfaces::mem_alloc->free(info.animstate);
			info.animstate = nullptr;
		}
	}

	if (!info.animstate) 
	{
		info.animstate = reinterpret_cast< CBaseAnimState* > (interfaces::mem_alloc->alloc(860u));
		player->create_animation_state(info.animstate);
	}
	state->m_fDuckAmount = clamp(player->duck(), 0.f, 1.f);

	CAnimationLayer client_layers[13];
	state->m_flFeetYawRate = 0.f;
	state->m_flFeetCycle = client_layers[6].m_flCycle;

	//--- do resolved ang here!
	player->client_side_animation() = true;
	player->UpdateClientSideAnimation();
	player->client_side_animation() = false;

	std::memcpy(client_layers, player->get_anim_overlays(), 0x38 * player->num_anim_overlays());

	player->m_VecORIGIN() = origin;
	player->getvelocity() = vel;
	player->get_abs_velocity() = absvel;
	player->duck() = duck;
	player->SetLowerBodyYaw(lby);
	player->GetAbsOrigin() = absorigin;
}

float pick(float pt, bool d)
{
	if (pt < -80 || pt > 80)
		return -pt;

	else
		if (d)
			return -89.f;
		else
			return 89.f;
}

float c_animfix::get_pitch(IClientEntity * player)
{
	int index = player->GetIndex();

	float last[65] = { FLT_MAX };
	float adjusted[65] = { FLT_MAX };

	static int sim[65];

	if (sim[index] != player->get_simulation_time())
	{
		switch (options::menu.aimbot.preso.getindex())
		{
		case 0: adjusted[index] = player->get_eyeangles()->x;
			break;
		case 1: adjusted[index] = 89.9f;
			break;
		case 2: adjusted[index] = -89.9f;
			break;
		case 3: adjusted[index] = 0.f;
			break;
		case 4:
		{
			int cur[65] = { player->GetAmmoInClip() };
			int prev[65];
			float logged[65] = { FLT_MAX };

			if (prev[index] != cur[index] && logged[index] != FLT_MAX)
			{
				pick(player->get_eyeangles()->x, true);

				prev[index] = cur[index];
			}

			if (logged[index] == FLT_MAX)
				if (prev[index] != cur[index])
				{
					pick(player->get_eyeangles()->x, false);
					prev[index] = cur[index];
				}
		}
		break;
		}
		sim[index] = player->get_simulation_time();
	}

	if (adjusted[index] == FLT_MAX)
		return 89.f;
	else
		return adjusted[index];
}

void c_animfix::clear() // let's fix up some shit
{
	if (interfaces::engine->IsInGame())
		return;

	const auto local = hackManager.plocal();

	if (!local || !local->IsAlive())
	{
		interfaces::ent_list->for_each_player([](IClientEntity* player) -> void
		{
			player->client_side_animation() = true;
		});
		return;
	}

}