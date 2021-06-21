#include "Resolver.h"
#include "Ragebot.h"
#include "Hooks.h"
#include "RenderManager.h"
#include "position_adjust.h"
#include "LagCompensation2.h"
#include "laggycompensation.h"
#include "global_count.h"
#include "position_adjust.h"
#include "Autowall.h"
#ifdef NDEBUG
#define XorStr( s ) ( XorCompileTime::XorString< sizeof( s ) - 1, __COUNTER__ >( s, std::make_index_sequence< sizeof( s ) - 1>() ).decrypt() )
#else
#define XorStr( s ) ( s )
#endif
#include "antiaim.h"
resolver_setup * resolver = new resolver_setup();

namespace global_count
{
	int hits[65] = { 0.f };
	int shots_fired[65] = { 0.f };
	int missed_shots[64] = { 0.f };
	bool didhit[64] = { 0.f };
	bool on_fire;

	int missed;
	int hit;
}

void calculate_angle(vector src, vector dst, vector &angles)
{
	vector delta = src - dst;
	vec_t hyp = delta.length2d();
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.x = (atan(delta.z / hyp) * 57.295779513082f);
	angles.x = (atan(delta.z / hyp) * 57.295779513082f);
	angles[2] = 0.0f;
	if (delta.x >= 0.0) angles.y += 180.0f;
}

QAngle calc(vector vecOrigin, vector vecOther)
{
	auto ret = QAngle();
	vector delta = vecOrigin - vecOther;
	double hyp = delta.length2d();
	ret.y = (atan(delta.y / delta.x) * 57.295779513082f);
	ret.x = (atan(delta.z / hyp) * 57.295779513082f);
	ret.z = 0.f;

	if (delta.x >= 0.f)
		ret.y += 180.f;
	return ret;
}

float NormalizeAngle_smh(float angle)
{
	if (angle > 180.f || angle < -180.f)
	{
		auto revolutions = round(abs(angle / 360.f));
		if (angle < 0.f)
			angle = angle + 360.f * revolutions;
		else
			angle = angle - 360.f * revolutions;
	}
	return angle;
}
void NormalizeNumX(vector &vIn, vector &vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0) {
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}

inline float RandomFloat(float min, float max)
{
	static auto fn = (decltype(&RandomFloat))(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
	return fn(min, max);
}
void resolver_setup::preso(IClientEntity * player)
{
	switch (options::menu.aimbot.preso.getindex())
	{
	case 1:
	{
		player->get_eyeangles()->x = 89;
		//	resolver->resolved_pitch = 89.f;
	}
	break;
	case 2:
	{
		player->get_eyeangles()->x = -89;
		//	resolver->resolved_pitch = -89.f;
	}
	break;
	case 3:
	{
		player->get_eyeangles()->x = 0;
		//	resolver->resolved_pitch = 0.f;
	}
	break;
	case 4:
	{

		auto index = player->GetIndex();

		static bool shoot[65];
		static bool untrusted[65];
		static float shoot_time[65];
		float base_pitch;

		const auto local = hackManager.plocal();
		if (!local)
			return;

		IGameEvent* g_event;

		for (auto i = 0; i < interfaces::engine->GetMaxClients(); ++i)
		{
			if (!g_event)
				continue;

			static float simtime[65];

			const auto eye = player->get_eyeangles();

			float pitch;

			if (player->weapon())
			{
				if ((!strcmp(g_event->GetName(), "weapon_fire")))
				{
					auto entity = interfaces::ent_list->get_client_entity(interfaces::engine->GetPlayerForUserID(g_event->GetInt("userid")));

					if (entity->GetIndex() == player->GetIndex() && shoot_time[index] != player->weapon()->last_shottime()) // check if they shot
					{
						pitch = eye->x;
						shoot[index] = true;

						shoot_time[index] = player->weapon()->last_shottime();
					}
					else
						pitch = eye->x;
				}
				else
				{
					if (eye->x > 80.f)
					{
						pitch = 89.f;
					}

					else if (eye->x < -80.f)
					{
						pitch = -89.f;
					}

					else
					{
						pitch = eye->x;
					}

					shoot[index] = false;
				}
			}
			else
			{
				shoot[index] = false;
				shoot_time[index] = 0.f; // reset
			}

			player->get_eyeangles()->x = pitch;
		}
	}
	break;

	}

}

player_info_t GetInfo2(int Index) {
	player_info_t Info;
	interfaces::engine->GetPlayerInfo(Index, &Info);
	return Info;
}

int IClientEntity::sequence_activity(IClientEntity* pEntity, int sequence)
{
	const model_t* pModel = pEntity->GetModel();
	if (!pModel)
		return 0;

	auto hdr = interfaces::model_info->GetStudiomodel(pEntity->GetModel());

	if (!hdr)
		return -1;

	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Utilities::Memory::FindPatternV2("client.dll", "55 8B EC 53 8B 5D 08 56 8B F1 83"));

	return get_sequence_activity(pEntity, hdr, sequence);
}

float NormalizeFloatToAngle(float input)
{
	for (auto i = 0; i < 3; i++) {
		while (input < -180.0f) input += 360.0f;
		while (input > 180.0f) input -= 360.0f;
	}
	return input;
}

float override_yaw(IClientEntity* player, IClientEntity* local) {
	vector eye_pos, pos_enemy;
	CalcAngle(player->GetEyePosition(), local->GetEyePosition(), eye_pos);

	if (Render::TransformScreen(player->GetOrigin(), pos_enemy))
	{
		if (GUI.GetMouse().x < pos_enemy.x)
			return (eye_pos.y - 90);
		else if (GUI.GetMouse().x > pos_enemy.x)
			return (eye_pos.y + 90);
	}

}

#define M_PI 3.14159265358979323846
void VectorAnglesBrute(const vector& forward, vector &angles)
{
	float tmp, yaw, pitch;
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0) pitch = 270; else pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0) yaw += 360; tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]); pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
		if (pitch < 0) pitch += 360;
	} angles[0] = pitch; angles[1] = yaw; angles[2] = 0;
}

vector calc_angle_trash(vector src, vector dst)
{
	vector ret;
	VectorAnglesBrute(dst - src, ret);
	return ret;
}

int total_missed[64];
int total_hit[64];
IGameEvent* event = nullptr;
extra s_extra;
HANDLE _out = NULL, _old_out = NULL;
bool ConsolePrint(const char* fmt, ...)
{
	if (!_out)
		return false;

	char buf[1024];
	va_list va;

	va_start(va, fmt);
	_vsnprintf_s(buf, 1024, fmt, va);
	va_end(va);//im missing 5 chromosomes

	return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
}
void angle_correction::missed_due_to_desync(IGameEvent* _event) {

	if (_event == nullptr)
		return;

	if ((!strcmp(_event->GetName(), "weapon_fire"))) {
		int user = _event->GetInt("userid");
		int attacker = _event->GetInt("attacker");
		bool player_hurt[64], hit_entity[64];

		static correction_flags stage = none;

		if (interfaces::engine->GetPlayerForUserID(user) != interfaces::engine->get_localplayer()
			&& interfaces::engine->GetPlayerForUserID(attacker) == interfaces::engine->get_localplayer()) {
			player_hurt[interfaces::engine->GetPlayerForUserID(user)] = true;

			vector bullet_impact_location = vector(_event->GetFloat("x"), _event->GetFloat("y"), _event->GetFloat("z"));
			if (Globals::aim_point != bullet_impact_location) {
				stage = precise_hit;
			}
		}

		if (interfaces::engine->GetPlayerForUserID(user) != interfaces::engine->get_localplayer()) {	
			hit_entity[interfaces::engine->GetPlayerForUserID(user)] = true;
		}

		if (interfaces::engine->GetPlayerForUserID(attacker) == interfaces::engine->get_localplayer()) {
			IClientEntity * e = interfaces::ent_list->get_client_entity(user); float speed = 0.f;
			if (e && e->getvelocity().length2d() > 270)
				stage = correction_flags::lag;

			vector bullet_impact_location = vector(_event->GetFloat("x"), _event->GetFloat("y"), _event->GetFloat("z"));
			if (Globals::aim_point != bullet_impact_location) {
				s_extra.current_flag[interfaces::engine->GetPlayerForUserID(user)] = correction_flags::spread;
				stage = correction_flags::spread;
			}
			else {
				s_extra.current_flag[interfaces::engine->GetPlayerForUserID(user)] = correction_flags::resolve;
				stage = correction_flags::resolve;
				++total_missed[interfaces::engine->GetPlayerForUserID(user)];
			}
		}

		std::string lag = "missed shot due to lagcomp";
		std::string spread = "missed shot due to inaccuracy";
		std::string resolve = "missed shot due to wrong resolve";
		std::string precise = "hit player despite inaccuracy";
		std::string newline = ".     \n";
		if (stage == correction_flags::resolve)
			ConColorMsg(Color(255, 255, 255, 255), (resolve + newline).c_str());
		else if (stage == correction_flags::spread)
			ConColorMsg(Color(255, 255, 255, 255), (spread + newline).c_str());
		else if (stage == correction_flags::spread)
			ConColorMsg(Color(255, 255, 255, 255), (lag + newline).c_str());
		else if (stage == correction_flags::precise_hit)
			ConColorMsg(Color(255, 255, 255, 255), (precise + newline).c_str());
	}
}

int IClientEntity::GetSequenceActivity(int sequence)
{
	auto hdr = interfaces::model_info->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	static auto getSequenceActivity = (DWORD)(Utilities::Memory::FindPatternV2("client.dll", "55 8B EC 83 7D 08 FF 56 8B F1 74"));
	static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(getSequenceActivity);

	return GetSequenceActivity(this, hdr, sequence);
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

float NormalizeYaw180(float yaw)
{
	if (yaw > 180)
		yaw -= (round(yaw / 360) * 360.f);
	else if (yaw < -180)
		yaw += (round(yaw / 360) * -360.f);

	return yaw;
}
bool delta_58(float first, float second)
{
	if (first - second < 58.f && first - second > -58.f)
	{
		return true;
	}
	return false;
}
vector CalcAngleToEnt(const vector& vecSource, const vector& vecDestination)
{
	vector qAngles;
	vector delta = vector((vecSource[0] - vecDestination[0]), (vecSource[1] - vecDestination[1]), (vecSource[2] - vecDestination[2]));
	float hyp = sqrtf(delta[0] * delta[0] + delta[1] * delta[1]);
	qAngles[0] = (float)(atan(delta[2] / hyp) * (180.0f / M_PI));
	qAngles[1] = (float)(atan(delta[1] / delta[0]) * (180.0f / M_PI));
	qAngles[2] = 0.f;
	if (delta[0] >= 0.f)
		qAngles[1] += 180.f;

	return qAngles;
}
float feet_yaw_delta(float first, float second)
{
	return first - second;
}

#define MASK_SHOT_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)

float NormalizeX(float yaw)
{
	if (yaw != yaw)
		yaw = 0.f;

	return fmod(yaw + 180.f, 360.f) - 180.f;
}

float approach(float cur, float target, float inc) {
	inc = abs(inc);

	if (cur < target)
		return min(cur + inc, target);
	if (cur > target)
		return max(cur - inc, target);

	return target;
}

float angle_difference(float a, float b) {
	auto diff = NormalizeYaw180(a - b);

	if (diff < 180)
		return diff;
	return diff - 360;
}

float approach_angle(float cur, float target, float inc) {
	auto diff = angle_difference(target, cur);
	return approach(cur, cur + diff, inc);
}

int IClientEntity::get_sequence_act(int sequence)
{
	if (!this->GetModel())
		return -1;

	auto hdr = interfaces::model_info->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	static auto get_sequence_activity = reinterpret_cast< int(__fastcall*)(void*, studiohdr_t*, int) >((DWORD)
		game_utils::pattern_scan(GetModuleHandle("client.dll"),
			"55 8B EC 53 8B 5D 08 56 8B F1 83"/*"55 8B EC 83 7D 08 FF 56 8B F1 74"*/));
	return get_sequence_activity(this, hdr, sequence);
}

bool angle_correction::breaking_lby_animations(IClientEntity* e)
{
	if (!e || e->is_dormant() || !e->IsAlive())
		return false;

	for (size_t i = 0; i < e->num_anim_overlays(); i++)
	{
		auto layer = e->get_anim_overlay_index(i);

		if (!layer )
			continue;
		if (e->get_sequence_act(layer->m_nSequence) == 979)
		{
			if (layer->m_flCycle != layer->m_flCycle || layer->m_flWeight == 1.f)
				return true;
		}
	}

	return false;
} // currentLayer->m_flWeight == 0.f && (previousLayer->m_flCycle > 0.92f && currentLayer->m_flCycle > 0.92f)

bool resolver_setup::solve_desync_simple(IClientEntity* e)
{
	if (!e || e->is_dormant() || !e->IsAlive())
		return false;

	for (size_t i = 0; i < e->num_anim_overlays(); i++)
	{
		auto layer = e->get_anim_overlay_index(i);

		if (!layer)
			continue;
		if (e->get_sequence_act(layer->m_nSequence) == 979)
		{
			if (layer->m_flWeight == 0.f && (layer->m_flPrevCycle > 0.92f && layer->m_flCycle > 0.92f))
				return true;
		}
	}

	return false;
}



float flAngleMod(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
}
float ApproachAngle(float target, float value, float speed)
{
	target = flAngleMod(target);
	value = flAngleMod(value);

	float delta = target - value;

	// Speed is assumed to be positive
	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}


float resolver_setup::mirror_release_basic(IClientEntity* player, IClientEntity* local)
{
	bool on_ground = (player->GetFlags() & FL_ONGROUND);
	bool crouched = (player->GetFlags() & FL_DUCKING);

	static float brute = 0.f;
	float resolved = 0.f;

	if (on_ground) {
		bool knife = (player->weapon() && player->weapon()->knife());
		bool max = false;

		float backwards_yaw = calc(local->GetVecOrigin_2(), player->GetVecOrigin_2()).y;

	//	bool forward = fabsf(NormalizeAngle_smh(player->get_eyeangles()->y - (NormalizeAngle_smh(backwards_yaw - 180.f)))) < 90.f;
		bool should_do_backwards = false;

		for (size_t i = 0; i < player->num_anim_overlays(); i++)
		{
			auto layer = player->get_anim_overlay_index(i);
			if (!layer)
				continue;

			max = (layer[3].m_flWeight == 0.f && layer[3].m_flCycle == 0.f);
		}

		if (solve_desync_simple(player)) {		
			if (player->get_pose_paramaters()[11] <= 0.f)
				brute = max ? -60.f : -35.f;
			else
				brute = max ? 60.f : 35.f; 
			resolver->enemy_fake[player->GetIndex()] = true;
		}
		else {
			if (delta_58(backwards_yaw, player->get_eyeangles()->y) && !delta_58(backwards_yaw - 180.f, player->get_eyeangles()->y)) {
				brute = 0.f;
				should_do_backwards = true;
			}
			else {
				if (player->get_pose_paramaters()[11] <= 0.f)
					brute = max ? 40.f : 29.f;
				else
					brute = max ? -40.f : -29.f; 
			}
			resolver->enemy_fake[player->GetIndex()] = false;
		}
		return should_do_backwards ? (backwards_yaw) : (player->GetBasePlayerAnimState()->m_flEyeYaw + brute);
	}
	else {
		player->get_pose_paramaters()[2] = crouched ? 1.f : 0.5;
		player->get_pose_paramaters()[11] = crouched ? 1.75f : 0.25;

		return player->get_eyeangles()->y;
	}

}

player_info_t GetInfo_x(int Index) {
	player_info_t Info;
	interfaces::engine->GetPlayerInfo(Index, &Info);
	return Info;
}


void angle_correction::mirror_aesthetic_console()
{
	interfaces::cvar->ConsoleColorPrintf(Color(250, 250, 250, 255), "[");
	interfaces::cvar->ConsoleColorPrintf(Color(210, 10, 250, 255), "mir");
	interfaces::cvar->ConsoleColorPrintf(Color(10, 190, 250, 255), "ror");
	interfaces::cvar->ConsoleColorPrintf(Color(250, 250, 250, 255), "]");
}

void resolver_setup::fakeangles(IClientEntity  * localplayer, IClientEntity  * enemy)
{
	float lby[65] = { enemy->get_lowerbody() };
	float speed[65] = { enemy->getvelocity().length2d() };

	float movinglby[65] = { FLT_MAX };
	float delta[65] = { FLT_MAX };
	float old[65] = { FLT_MAX };
	float old_m[65] = { FLT_MAX };

	bool high[65] = { false };
	bool low[65] = { false };
	bool was[65] = { false };
	static bool fw[65] = { false };

	Globals::missedshots[enemy->GetIndex()] = Globals::fired[enemy->GetIndex()] - Globals::hit[enemy->GetIndex()];

	if (Globals::missedshots[enemy->GetIndex()] > 5)
	{
		Globals::missedshots[enemy->GetIndex()] = 0;
	}
	
		if (speed[enemy->GetIndex()] > 29.f)
		{
			if (movinglby[enemy->GetIndex()] == FLT_MAX)
				movinglby[enemy->GetIndex()] = lby[enemy->GetIndex()];

			old[enemy->GetIndex()] = lby[enemy->GetIndex()];

			was[enemy->GetIndex()] = true;
		}
		else
		{
			if (speed[enemy->GetIndex()] < 0.1)
			{
	
				if (old[enemy->GetIndex()] != lby[enemy->GetIndex()])
				{
					if (old[enemy->GetIndex()] == FLT_MAX)
					{
						old[enemy->GetIndex()] = lby[enemy->GetIndex()];
					}
					else
						delta[enemy->GetIndex()] = lby[enemy->GetIndex()] - old[enemy->GetIndex()];
				}

				for (size_t i = 0; i < enemy->num_anim_overlays(); i++)
				{
					auto layer = enemy->get_anim_overlay_index(i);
					if (!layer)
					{
						continue;
					}

					if (layer->m_flPrevCycle > 0.92f && layer->m_flCycle > 0.92f && layer->m_flWeight == 0.f)
					{
						low[enemy->GetIndex()] = true;
						high[enemy->GetIndex()] = false;
					}
					else
					{
						low[enemy->GetIndex()] = false;
						if (layer->m_flPrevCycle != layer->m_flCycle || layer->m_flWeight == 1.f)
						{
							high[enemy->GetIndex()] = true;
						}
						else 
							high[enemy->GetIndex()] = false;
					}
				}

			}
		}

	if (speed[enemy->GetIndex()] >= 0.1 && speed[enemy->GetIndex()] <= 29.f)
	{
		for (size_t i = 0; i < enemy->num_anim_overlays(); i++)
		{
			auto layer = enemy->get_anim_overlay_index(i);
			if (!layer)
			{
				continue;
			}

			old_m[enemy->GetIndex()] = lby[enemy->GetIndex()];

			if (enemy->get_sequence_act(layer->m_nSequence) == 979)
			{
				if (was[enemy->GetIndex()])
				{
					switch (Globals::Missed % 4)
					{
					case 0: enemy->get_eyeangles()->y = movinglby[enemy->GetIndex()];
						break;
					case 1: enemy->get_eyeangles()->y = movinglby[enemy->GetIndex()] - 20;
						break;
					case 2: enemy->get_eyeangles()->y = movinglby[enemy->GetIndex()] + 40;
						break;
					case 3: enemy->get_eyeangles()->y = movinglby[enemy->GetIndex()] - 40;
						break;
					case 4: enemy->get_eyeangles()->y = movinglby[enemy->GetIndex()] + 15;
						break;
					}
				}
				else
				{
					switch (Globals::Missed % 4)
					{
					case 0: enemy->get_eyeangles()->y = enemy->get_lowerbody();
						break;
					case 1: enemy->get_eyeangles()->y = enemy->get_lowerbody() - 15;
						break;
					case 2: enemy->get_eyeangles()->y = enemy->get_lowerbody() - 40;
						break;
					case 3: enemy->get_eyeangles()->y = enemy->get_lowerbody() + 20;
						break;
					case 4: enemy->get_eyeangles()->y = enemy->get_lowerbody();
						break;
					}
				}
			}
			else
				enemy->get_eyeangles()->y = enemy->get_lowerbody();
		}
	}
	else
	{
		if (delta[enemy->GetIndex()] == FLT_MAX || old_m[enemy->GetIndex()] == FLT_MAX)
		{
			if (was[enemy->GetIndex()])
			{
				enemy->get_eyeangles()->y = movinglby[enemy->GetIndex()];
			}
			else if (low[enemy->GetIndex()])
				enemy->get_eyeangles()->y = enemy->get_lowerbody() + 100.f;
			else
				enemy->get_eyeangles()->y = enemy->get_lowerbody();
		}
		else
		{
			if (high[enemy->GetIndex()] || low[enemy->GetIndex()])
			{
				switch (Globals::Missed % 4)
				{
				case 0: enemy->get_eyeangles()->y = enemy->get_lowerbody() - delta[enemy->GetIndex()];
					break;
				case 1: enemy->get_eyeangles()->y = enemy->get_lowerbody() - delta[enemy->GetIndex()] - 35;
					break;
				case 2: enemy->get_eyeangles()->y = enemy->get_lowerbody() - delta[enemy->GetIndex()] + 30;
					break;
				case 3: enemy->get_eyeangles()->y = enemy->get_lowerbody();
					break;
				case 4: enemy->get_eyeangles()->y = enemy->get_lowerbody() - 110;
					break;
				}
			}
			else
			{
				switch (Globals::Missed % 4)
				{
				case 0: enemy->get_eyeangles()->y = enemy->get_lowerbody();
					break;
				case 1: enemy->get_eyeangles()->y = enemy->get_lowerbody() - 15;
					break;
				case 2: enemy->get_eyeangles()->y = enemy->get_lowerbody() - 40;
					break;
				case 3: enemy->get_eyeangles()->y = enemy->get_lowerbody() + 20;
					break;
				case 4: enemy->get_eyeangles()->y = enemy->get_lowerbody();
					break;
				}
			}
		}
	}
}


float maxdeltax(IClientEntity * player)
{
	auto animstate = player->GetBasePlayerAnimState();

	if (!animstate)
		return 0.f;

	float speedfactor = animstate->m_flFeetSpeedForwardsOrSideWays;

	if (animstate->m_flFeetSpeedForwardsOrSideWays > 1.f)
		speedfactor = 1.f;
	else if (animstate->m_flFeetSpeedForwardsOrSideWays < 0.f)
		speedfactor = 0.f;

	float unk1 = ((animstate->m_flStopToFullRunningFraction * -.3f) - .2f) * speedfactor;
	float unk2 = unk1 + 1.f;

	if (animstate->m_fDuckAmount > 0.0f)
	{
		float max_velocity = animstate->m_flFeetSpeedUnknownForwardOrSideways;

		if (animstate->m_flFeetSpeedUnknownForwardOrSideways > 1.f)
			max_velocity = 1.f;
		else if (animstate->m_flFeetSpeedUnknownForwardOrSideways < 0.f)
			max_velocity = 0.f;

		float duck_speed = animstate->m_fDuckAmount * max_velocity;
		unk2 += (duck_speed * (0.5f - unk2));
	}
	return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(animstate) + 0x334) * unk2;
}


void target_aim(IClientEntity * player, float grinch) { // fill my double cup up with some lean
	QAngle subscribe;
	float lowest = 99999999.f;

	vector eyepos = hackManager.plocal()->m_VecORIGIN();
	vector CurPos = player->m_VecORIGIN();

	auto dist = CurPos.Dist(eyepos);

	if (dist < lowest) {
		lowest = dist;
		subscribe = CalcAngleA(eyepos, CurPos);
	}
	grinch = subscribe.y;
}

// note to people seeing this, if you're gonna release a mirror paste, at least please follow this code style
// i know it's not the best but seriously, just give it a go okay?



void resolver_setup::auto_config(IClientEntity * local, IClientEntity * player, bool head)
{
	if (!player || player->IsDormant() || !player->IsAlive()) {
		head = false;
		return;
	}
	int idx = player->GetIndex();

	float yaw[64] = { player->get_eyeangles()->y };
	float grinch = Math_trash::CalcAngleTrash(local->GetVecOrigin_2(), player->GetAbsOrigin()).y - 180.f;
	float delta = grinch - yaw[idx];

	bool valid = false;

	bool negative = delta > -110.f && delta < -75.f;
	bool positive = delta > 75.f && delta < 110.f;

	if (negative || positive)
		head = true;
	else
		head = false;
}

float resolver_setup::mirror_release_brute(IClientEntity  * local, IClientEntity  * player)
{
	auto anim_state = player->get_animation_state();
	bool on_ground = (player->GetFlags() & FL_ONGROUND);

	if (!anim_state)
		return player->GetAbsAngles().y;

	if (on_ground) {
		bool knife = false;

		float base_yaw = player->get_eyeangles()->y;
		float inverted_yaw = -(player->get_eyeangles()->y);
		float backwards_yaw = calc(local->m_VecORIGIN(), player->m_VecORIGIN()).y;

		if (player->weapon()->knife())
			knife = true;
		else
			knife = false;

		anim_state->goal_feet_yaw = base_yaw - (inverted_yaw / 2);
		return backwards_yaw - (knife ? base_yaw : (base_yaw / 2));

	}
	else
		return player->get_eyeangles()->y;
}

angle_correction *a_c = new angle_correction();
#include "animations.h"
#include "MiscHacks.h"
void resolver_setup::FSN(IClientEntity* pEntity, ClientFrameStage_t stage)
{

	if (!interfaces::engine->IsConnected() || !interfaces::engine->IsInGame())
		return;

	if (!options::menu.aimbot.enable.getstate())
		return;

	if (hackManager.plocal()->team() != 2 && hackManager.plocal()->team() != 3)
		return;

	switch (stage)
	{
		case ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		{
			for (int i = 1; i < 65; i++)
			{
				pEntity = (IClientEntity*)interfaces::ent_list->get_client_entity(i);
				if (!pEntity || pEntity->team() == hackManager.plocal()->team() || pEntity->IsDormant())
					continue;

				if (!hackManager.plocal()->IsAlive()) {
		
					pEntity->UpdateClientSideAnimation();
					continue;
				}			

				if (pEntity->GetOrigin() == vector(0, 0, 0))
					continue;

				if (!pEntity->GetBasePlayerAnimState())
					continue;

				get_resolved[pEntity->GetIndex()].x = animfix->get_pitch(pEntity);

				if (pEntity->GetIndex() != c_helper->closest())
					get_resolved[pEntity->GetIndex()].y = pEntity->GetEyeAngles().y;

				switch (options::menu.aimbot.resolver.getindex())
				{
				case 0: get_resolved[pEntity->GetIndex()].y = pEntity->GetEyeAngles().y; 
					break;
				case 1: get_resolved[pEntity->GetIndex()].y = mirror_release_basic(pEntity, hackManager.plocal());
					break;
				case 2: get_resolved[pEntity->GetIndex()].y = mirror_release_brute(hackManager.plocal(), pEntity);
					break;
				case 3: fakeangles(hackManager.plocal(), pEntity);
					break;
				}
				pEntity->GetBasePlayerAnimState()->goal_feet_yaw = get_resolved[pEntity->GetIndex()].y;
				if (get_resolved[pEntity->GetIndex()] == vector(0, 0, 0))
					get_resolved[pEntity->GetIndex()] = pEntity->GetEyeAngles();

				get_resolved[pEntity->GetIndex()].z = 0.f;
			}
		}
		break;
	}

}


