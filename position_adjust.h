#pragma once
#include "Hooks.h"
#include "Hacks.h"
#include "ESP.h"
#include "Entities.h"

#include "SDK.h"
#include <deque>
#include <array>
#include "Singleton.hpp"

inline vector AngleVector(vector meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(PI));

	auto sp = sin(meme.x / 180.f * static_cast<float>(PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(PI));

	return vector(cp*cy, cp*sy, -sp);
}
struct legit_backtrackdata
{
	float simtime;
	vector hitboxPos;
};
inline float DistPointToLine(vector Point, vector LineOrigin, vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

/*

Most won't see it here anyways, but question from me to you.
Should i declare?

*/

struct lbyRecords
{
	int tick_count;
	float lby;
	vector headPosition;
	int32_t m_nFlags;
};
struct backtrackData
{
	float simtime;
	vector hitboxPos;
	vector origin;
	vector angs;
	int32_t flags;
	int meme;
	vector gownoPos;
};
struct urmomlol
{
	int32_t m_nFlags;
};

struct thiccboi
{
	matrix3x4* matrix;
	vector originX;
};

class BackTrack
{
public:
	int latest_tick;
	int bestTargetIndex = -1;
	float bestTargetSimTime;
	bool IsTickValid(int tick);
	bool good_tick(int tick);
	lbyRecords records[64];
	
	void run_legit(CUserCmd * cmd);

	void rageBackTrack(CUserCmd * cmd, IClientEntity * pLocal);

	void UpdateRecord(int i);
	bool RunLBYBackTrack(CUserCmd * cmd, vector & aimPoint, IClientEntity * ent);
	float point_to_line(vector Point, vector LineOrigin, vector Dir);
	void Update(int tick_count);
	float GetLerpTime();

};


struct CTickRecord {
	CTickRecord() {}
	CTickRecord(IClientEntity* ent)
	{
		m_flLowerBodyYawTarget = ent->get_lowerbody();
		m_angEyeAngles = ent->GetEyeAngles();
		m_flCycle = ent->GetCycle();
		m_flSimulationTime = ent->GetSimulationTime();
		m_nSequence = ent->GetSequence();
		m_vecOrigin = ent->GetOrigin();
		m_vecVelocity = ent->getvelocity();
		//m_flPoseParameter = ent->getPoseParams();
		m_angAbsAngles = ent->GetAbsAngles();
		m_vecAbsOrigin = ent->GetAbsOrigin();
		m_flDuckSpeed = ent->GetDuckSpeed();
		m_flDuckAmount = ent->GetDuckAmount();


	}

	explicit operator bool() const noexcept {
		return m_flSimulationTime > 0.f;
	}

	bool operator>(const CTickRecord& others) {
		return (m_flSimulationTime > others.m_flSimulationTime);
	}
	bool operator>=(const CTickRecord& others) {
		return (m_flSimulationTime >= others.m_flSimulationTime);
	}
	bool operator<(const CTickRecord& others) {
		return (m_flSimulationTime < others.m_flSimulationTime);
	}
	bool operator<=(const CTickRecord& others) {
		return (m_flSimulationTime <= others.m_flSimulationTime);
	}
	bool operator==(const CTickRecord& others) {
		return (m_flSimulationTime == others.m_flSimulationTime);
	}

	matrix3x4_t	matrix[128];
	bool matrixBuilt;
	float m_flDuckAmount = 0.f;
	float m_flDuckSpeed = 0.f;
	float m_flLowerBodyYawTarget = 0.f;
	QAngle m_angEyeAngles = QAngle(0, 0, 0);
	float m_flCycle = 0.f;
	float m_flSimulationTime = 0.f;
	int m_nSequence = 0;
	vector m_vecOrigin = vector(0, 0, 0);
	vector m_vecAbsOrigin = vector(0, 0, 0);
	vector m_vecVelocity = vector(0, 0, 0);
	//std::array<float, 24> m_flPoseParameter = {};
	QAngle m_angAbsAngles = QAngle(0, 0, 0);
	int tickcount = 0;
};

struct C_PlayerStored {
	C_BaseCombatWeapon* CurrentWeapon = nullptr;
	int BulletsLeft = 0;
	bool EyeAnglesUpdated = false;
	bool IsFiring = false;
	int TickCount = 0;
	float SimulationTime = 0;
	QAngle EyeAngles = QAngle(0, 0, 0);
	CTickRecord ValidTick = CTickRecord();
};

struct CBacktrack_Info {
	CBacktrack_Info() : shot(false), player(C_PlayerStored()), tick_count(0), temporaryRecord(CTickRecord()) {}

	bool shot = false;
	C_PlayerStored player = C_PlayerStored();
	int tick_count = 0;
	CTickRecord temporaryRecord = CTickRecord();


	IClientEntity* Entity;
	std::deque<CTickRecord> TicRkecords;
};

class cbacktracking : public Singleton<cbacktracking> {
	friend class CLagcompensation;
	friend class CResolver;


public:
	int ShotBackTrackedTick(IClientEntity* ent);
	void ShotBackTrackStoreFSN(IClientEntity* ent);
	void ShotBackTrackBeforeAimbot(IClientEntity* ent);
	void ShotBackTrackAimbotStart(IClientEntity* ent);
	void pasted_backTrack(CUserCmd * cmd);
	void ShotBackTrackAimbotEnd(IClientEntity* ent);
	void ClearRecord(IClientEntity* Entity);

	float sim;

public:
	bool bTickIsValid(int tick);
	void SaveTemporaryRecord(IClientEntity* ent, CTickRecord record = CTickRecord());
	void RestoreTemporaryRecord(IClientEntity* ent);
	std::array<CBacktrack_Info, 32> arr_infos;
	int ticks = 12;

private:
	void ApplyTickrecord(IClientEntity* ent, CTickRecord record);
	void ApplyTickRecordFSN(IClientEntity* pEntity, CTickRecord record);
	void ApplyTickRecordCM(IClientEntity* pEntity, CTickRecord record);
	int meme = 0;

};

extern backtrackData headPositions[64][12];
extern thiccboi hdfdunn[64][12];
extern urmomlol commitdie[64][12];
extern cbacktracking* bt_2;
extern BackTrack* backtracking;
extern CTickRecord * tickrec;

//extern legit_backtrackdata head_positions[64][12];


/*
#pragma once
#include "Hooks.h"
#include "Hacks.h"
#include "ESP.h"
#include "Entities.h"

#include "SDK.h"
#include <deque>
#include <array>
#include "Singleton.hpp"

inline vector AngleVector(vector meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(PI));

	auto sp = sin(meme.x / 180.f * static_cast<float>(PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(PI));

	return vector(cp*cy, cp*sy, -sp);
}
struct legit_backtrackdata
{
	float simtime;
	vector hitboxPos;
};
inline float DistPointToLine(vector Point, vector LineOrigin, vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

enum resolver_type
{
	ORIGINAL,
	BRUTEFORCE,
	LBY,
	TRACE,
	DIRECTIONAL
};

struct matrixes
{
	matrix3x4_t main[MAXSTUDIOBONES];
	matrix3x4_t zero[MAXSTUDIOBONES];
	matrix3x4_t first[MAXSTUDIOBONES];
	matrix3x4_t second[MAXSTUDIOBONES];
};


class adjust_data //-V730
{
public:
	IClientEntity* player;
	int i;

	AnimationLayer layers[15];
	matrixes matrixes_data;

	resolver_type type;


	bool invalid;
	bool immune;
	bool dormant;
	bool bot;

	int flags;
	int bone_count;

	float simulation_time;
	float duck_amount;
	float lby;

	vector angles;
	vector abs_angles;
	vector velocity;
	vector origin;
	vector mins;
	vector maxs;

	adjust_data() //-V730
	{
		reset();
	}

	void reset()
	{
		player = nullptr;
		i = -1;

		type = ORIGINAL;
	//	side = RESOLVER_ORIGINAL;

		invalid = false;
		immune = false;
		dormant = false;
		bot = false;

		flags = 0;
		bone_count = 0;

		simulation_time = 0.0f;
		duck_amount = 0.0f;
		lby = 0.0f;

		angles.Zero();
		abs_angles.Zero();
		velocity.Zero();
		origin.Zero();
		mins.Zero();
		maxs.Zero();
	}

	adjust_data(IClientEntity* e, bool store = true)
	{
		type = ORIGINAL;
	//	side = 0;

		invalid = false;
		store_data(e, store);
	}

	void store_data(IClientEntity* e, bool store = true)
	{
		if (!e->IsAlive())
			return;

		player = e;
		i = player->GetIndex();

		if (store)
		{
			memcpy(layers, e->AnimOverlays(), e->animlayer_count() * sizeof(AnimationLayer));
			memcpy(matrixes_data.main , player->get_bone_cache()->m_pCachedBones, player->get_bone_cache()->m_CachedBoneCount * sizeof(matrix3x4_t));
		}

		immune = player->has_gungame_immunity() || player->GetFlags() & FL_FROZEN;
		dormant = player->IsDormant();


		player_info_t player_info;
		interfaces::engine->GetPlayerInfo(i, &player_info);


		flags = player->getFlags();
		bone_count = player->get_bone_cache()->m_CachedBoneCount;

		simulation_time = player->get_simulation_time();
		duck_amount = player->m_flDuckAmount();
		lby = player->get_lowerbody();

		angles = player->GetEyeAngles();
		abs_angles = player->GetAbsAngles();
		velocity = player->getvelocity();
		origin = player->m_VecORIGIN();
		mins = player->get_obbmins();
		maxs = player->get_obbmaxs();
	}

	void adjust_player()
	{
		if (!valid(false))
			return;

		memcpy(player->AnimOverlays(), layers, player->animlayer_count() * sizeof(AnimationLayer));
		memcpy(player->get_bone_cache()->m_pCachedBones, matrixes_data.main, player->get_bone_cache()->m_CachedBoneCount * sizeof(matrix3x4_t)); //-V807

		player->getFlags() = flags;
	//	player->m_CachedBoneData().m_Size = bone_count;

		player->get_simulation_time() = simulation_time;
		player->m_flDuckAmount() = duck_amount;
		player->SetLowerBodyYaw(lby);

		player->GetEyeAngles() = angles;
		player->SetAbsAngles(abs_angles);
		player->get_velocity() = velocity;
		player->m_VecORIGIN() = origin;
		player->set_abs_origin(origin);
		player->get_obbmins() = mins;
		player->get_obbmaxs() = maxs;
	}

	bool valid(float interpolation)
	{
		if (!this) //-V704
			return false;

		if (i > 0)
			player = (IClientEntity*)interfaces::ent_list->get_client_entity(i);

		if (!player)
			return false;

		if (!player->IsAlive())
			return false;

		if (immune)
			return false;

		if (dormant)
			return false;

		if (invalid)
			return false;

		auto net_channel_info = interfaces::engine->GetNetChannelInfo();

		if (!net_channel_info)
			return false;

		static auto sv_maxunlag = interfaces::cvar->FindVar("sv_maxunlag");

		auto outgoing = net_channel_info->GetLatency(FLOW_OUTGOING);
		auto incoming = net_channel_info->GetLatency(FLOW_INCOMING);

		auto correct = clamp(outgoing + incoming + interpolation, 0.0f, sv_maxunlag->GetFloat());

		auto curtime = hackManager.plocal()->IsAlive() ? TICKS_TO_TIME(overall::fixed_tickbase) : interfaces::globals->curtime; //-V807
		auto delta_time = correct - (curtime - simulation_time);

		if (fabs(delta_time) > 0.2f)
			return false;

		auto extra_choke = 0;

		if (GetAsyncKeyState(options::menu.misc.fake_crouch_key.GetKey()))
			extra_choke = 14 - *(int*)(uintptr_t(interfaces::client_state) + 0x4D28);

		auto server_tickcount = extra_choke + interfaces::globals->tickcount + TIME_TO_TICKS(outgoing + incoming);
		auto dead_time = (int)(TICKS_TO_TIME(server_tickcount) - sv_maxunlag->GetFloat());

		if (simulation_time < (float)dead_time)
			return false;

		return true;
	}
};

class lagcompensation : public Singleton <lagcompensation>
{
public:
	void fsn(ClientFrameStage_t stage);
	bool valid(int i, IClientEntity* e);
	void update_player_animations(IClientEntity* e);

//	resolver player_resolver[65];

	bool is_dormant[65];
	float previous_goal_feet_yaw[65];
};


struct backtrackData
{
	float simtime;
	vector hitboxPos;
	vector origin;
	vector angs;
	int32_t flags;
	int meme;
	vector gownoPos;
};
struct urmomlol
{
	int32_t m_nFlags;
};

struct thiccboi
{
	matrix3x4* matrix;
	vector originX;
};



struct CTickRecord {
	CTickRecord() {}
	CTickRecord(IClientEntity* ent)
	{
		m_flLowerBodyYawTarget = ent->get_lowerbody();
		m_angEyeAngles = ent->GetEyeAngles();
		m_flCycle = ent->GetCycle();
		m_flSimulationTime = ent->GetSimulationTime();
		m_nSequence = ent->GetSequence();
		m_vecOrigin = ent->GetOrigin();
		m_vecVelocity = ent->getvelocity();
		//m_flPoseParameter = ent->getPoseParams();
		m_angAbsAngles = ent->GetAbsAngles();
		m_vecAbsOrigin = ent->GetAbsOrigin();
		m_flDuckSpeed = ent->GetDuckSpeed();
		m_flDuckAmount = ent->GetDuckAmount();


	}

	explicit operator bool() const noexcept {
		return m_flSimulationTime > 0.f;
	}

	bool operator>(const CTickRecord& others) {
		return (m_flSimulationTime > others.m_flSimulationTime);
	}
	bool operator>=(const CTickRecord& others) {
		return (m_flSimulationTime >= others.m_flSimulationTime);
	}
	bool operator<(const CTickRecord& others) {
		return (m_flSimulationTime < others.m_flSimulationTime);
	}
	bool operator<=(const CTickRecord& others) {
		return (m_flSimulationTime <= others.m_flSimulationTime);
	}
	bool operator==(const CTickRecord& others) {
		return (m_flSimulationTime == others.m_flSimulationTime);
	}

	matrix3x4_t	matrix[128];
	bool matrixBuilt;
	float m_flDuckAmount = 0.f;
	float m_flDuckSpeed = 0.f;
	float m_flLowerBodyYawTarget = 0.f;
	QAngle m_angEyeAngles = QAngle(0, 0, 0);
	float m_flCycle = 0.f;
	float m_flSimulationTime = 0.f;
	int m_nSequence = 0;
	vector m_vecOrigin = vector(0, 0, 0);
	vector m_vecAbsOrigin = vector(0, 0, 0);
	vector m_vecVelocity = vector(0, 0, 0);
	//std::array<float, 24> m_flPoseParameter = {};
	QAngle m_angAbsAngles = QAngle(0, 0, 0);
	int tickcount = 0;
};


extern backtrackData headPositions[64][12];
extern thiccboi hdfdunn[64][12];
extern urmomlol commitdie[64][12];


extern CTickRecord * tickrec;

//extern legit_backtrackdata head_positions[64][12];


*/