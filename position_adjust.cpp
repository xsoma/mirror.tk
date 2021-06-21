#include "position_adjust.h"
#include "RageBot.h"
#include "laggycompensation.h"
#define TICK_INTERVAL			(interfaces::globals->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
legit_backtrackdata head_positions[64][12];
void BackTrack::Update(int tick_count)
{
	if (options::menu.aimbot.lag_pred.getindex() > 0)
	{
		if (interfaces::engine->IsConnected() || !interfaces::engine->IsInGame())
		{
			latest_tick = tick_count;
			for (int i = 0; i < 64; i++)
			{
				UpdateRecord(i);
			}
		}
	}
}
float BackTrack::GetLerpTime() {
	return 0;
}
template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	if (in >= high)
		return high;
	return in;
}

bool BackTrack::IsTickValid(int tick)
{
	return true;
}

bool BackTrack::good_tick(int tick)
{
	return true;
}

void BackTrack::UpdateRecord(int i)
{

}
bool BackTrack::RunLBYBackTrack(CUserCmd* cmd, vector& aimPoint, IClientEntity * ent)
{
	if (options::menu.aimbot.lag_pred.getindex() > 0)
	{
		for (int i = 0; i < interfaces::engine->GetMaxClients(); i++)
		{
			if (ent->getvelocity().length2d() > 1.f)
				continue;

			if (IsTickValid(records[i].tick_count))
			{
				aimPoint = records[i].headPosition;
				cmd->tick_count = records[i].tick_count;
				return true;
			}
		}
		return false;
	}
	else
		return false;
}

inline float BackTrack::point_to_line(vector Point, vector LineOrigin, vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

void BackTrack::run_legit(CUserCmd* cmd) //phook backtrack muahhahahahaaha
{

}
void BackTrack::rageBackTrack(CUserCmd* cmd, IClientEntity* pLocal)
{


}
bool cbacktracking::bTickIsValid(int tick)
{
	return false;
}
void cbacktracking::SaveTemporaryRecord(IClientEntity* ent, CTickRecord record)
{

}
void cbacktracking::RestoreTemporaryRecord(IClientEntity* ent)
{
	
}
void cbacktracking::ApplyTickrecord(IClientEntity* ent, CTickRecord record) {
	ApplyTickRecordFSN(ent, record);
	ApplyTickRecordCM(ent, record);
}
void cbacktracking::ApplyTickRecordFSN(IClientEntity* pEntity, CTickRecord record)
{
	*(QAngle*)((uintptr_t)pEntity + 0x0000B23C) = record.m_angEyeAngles;
	*(float*)((uintptr_t)pEntity + 0x00000264) = record.m_flSimulationTime;
	*(vector*)((uintptr_t)pEntity + 0x00000134) = record.m_vecOrigin;
	*(vector*)((uintptr_t)pEntity + 0x00000110) = record.m_vecVelocity;
	*(float*)((uintptr_t)pEntity + 0x00002F9C) = record.m_flDuckAmount;
	*(float*)((uintptr_t)pEntity + 0x00002FA0) = record.m_flDuckSpeed;
	//pEntity->SetPoseParameters(record.m_flPoseParameter);
	if (pEntity->GetAbsOrigin() == vector(0, 0, 0) || (record.m_vecOrigin - record.m_vecAbsOrigin).LengthSqr() > 100)
		pEntity->setAbsOriginal(record.m_vecOrigin);
	if (pEntity->GetAbsAngles_2() != pEntity->GetEyeAngles())
		pEntity->setAbsAechse(record.m_angEyeAngles);
}
void cbacktracking::ApplyTickRecordCM(IClientEntity* pEntity, CTickRecord record) {
	pEntity->setAbsAechse(record.m_angEyeAngles);
	pEntity->setAbsOriginal(record.m_vecOrigin);
	if (pEntity->GetAbsOrigin() == vector(0, 0, 0) || (record.m_vecOrigin - record.m_vecAbsOrigin).LengthSqr() > 20)
		pEntity->setAbsOriginal(record.m_vecOrigin);
	if (pEntity->GetAbsAngles_2() != pEntity->GetEyeAngles())
		pEntity->setAbsAechse(record.m_angEyeAngles);
	//	*(Vector*)((uintptr_t)pEntity + 0x00000134) = record.m_vecOrigin;  
	//pEntity->SetPoseParameters(record.m_flPoseParameter);
}
int cbacktracking::ShotBackTrackedTick(IClientEntity* ent) {
	if (!ent)
		return 0;
	C_PlayerStored* pCPlayer = &arr_infos.at(ent->GetIndex()).player;
	if (bTickIsValid(pCPlayer->TickCount))
		return pCPlayer->TickCount;
	return 0;
}
void cbacktracking::ShotBackTrackStoreFSN(IClientEntity* ent) {
	if (!ent)
		return;
	static std::array<float, 33> lastsimulationtimes;
	if (lastsimulationtimes[ent->GetIndex()] == ent->GetSimulationTime())
		return;
	lastsimulationtimes[ent->GetIndex()] = ent->GetSimulationTime();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());
	bool PlayerShot = false;
	C_PlayerStored* pCPlayer = &arr_infos[ent->GetIndex()].player;
	if (ent->GetEyeAngles() != pCPlayer->EyeAngles)
		pCPlayer->EyeAnglesUpdated = true;
	if (!pWeapon)
		return;
	if (pCPlayer->CurrentWeapon == pWeapon && pWeapon->IsGun()) {
		//don't count weapon changes
		if (pWeapon->GetAmmoInClip() == pCPlayer->BulletsLeft - 1) {
			if (pCPlayer->EyeAnglesUpdated) {
				//Don't count reloading
				pCPlayer->IsFiring = true;
				PlayerShot = true;
			}
		}
		else
			pCPlayer->IsFiring = false;
	}
	else {
		pCPlayer->IsFiring = false;
	}
	if (PlayerShot) {
		if (options::menu.aimbot.lag_pred.getindex() > 0)
			pCPlayer->TickCount = TIME_TO_TICKS(ent->GetSimulationTime() - interfaces::globals->interval_per_tick) + TIME_TO_TICKS(backtracking->GetLerpTime()) + 1;
		else
			pCPlayer->TickCount = interfaces::globals->tickcount;
		pCPlayer->ValidTick = CTickRecord(ent);
		pCPlayer->ValidTick.tickcount = pCPlayer->TickCount;
		pCPlayer->ValidTick.m_angEyeAngles = ent->GetEyeAngles();			//CURRENT angles yaw and pitch
																			//pCPlayer->ValidTick.m_flPoseParameter = std::array<float, 24>();
																			//if (ent->GetSimulationTime() - pCPlayer->SimulationTime > Interfaces::Globals->interval_per_tick) //if psilent or fake lag																//pCPlayer->ValidTick.m_angEyeAngles.x = pCPlayer->EyeAngles.x; //LAST angles pitch!
		arr_infos[ent->GetIndex()].shot = true;
	}
	pCPlayer->BulletsLeft = pWeapon->GetAmmoInClip();
	pCPlayer->EyeAngles = ent->GetEyeAngles();
	pCPlayer->CurrentWeapon = pWeapon;
	if (!bTickIsValid(pCPlayer->TickCount)) {
		pCPlayer->TickCount = 0;
		arr_infos[ent->GetIndex()].shot = false;
	}
}

void cbacktracking::ShotBackTrackBeforeAimbot(IClientEntity* ent) {
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());
	if (!ent || !hackManager.plocal() || !hackManager.plocal()->GetActiveWeaponHandle() || !interfaces::globals)
		return;
	bool can_shoot = (pWeapon->GetNextPrimaryAttack() <= (hackManager.plocal()->GetTickBase() * interfaces::globals->interval_per_tick));
	C_PlayerStored* pCPlayer = &arr_infos.at(ent->GetIndex()).player;
	if (interfaces::globals)
		interfaces::globals->tickcount = interfaces::globals->tickcount;
	if (can_shoot && bTickIsValid(pCPlayer->TickCount))
	{
		ApplyTickRecordFSN(ent, pCPlayer->ValidTick);
	}
}
void cbacktracking::ShotBackTrackAimbotStart(IClientEntity* ent) {
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());
	if (!ent || !hackManager.plocal() || !hackManager.plocal()->GetActiveWeaponHandle() || !interfaces::globals)
		return;
	bool can_shoot = (pWeapon->GetNextPrimaryAttack() <= (hackManager.plocal()->GetTickBase() * interfaces::globals->interval_per_tick));
	C_PlayerStored* pCPlayer = &arr_infos.at(ent->GetIndex()).player;
	if (interfaces::globals)
		interfaces::globals->tickcount = interfaces::globals->tickcount;
	if (bTickIsValid(pCPlayer->TickCount)) {
		SaveTemporaryRecord(ent);
		ApplyTickRecordCM(ent, pCPlayer->ValidTick);

		sim = ent->GetSimulationTime();
	}
}

void cbacktracking::pasted_backTrack(CUserCmd* cmd)
{

}


void cbacktracking::ShotBackTrackAimbotEnd(IClientEntity* ent) {
	return;
	if (!ent)
		return;
	C_PlayerStored* pCPlayer = &arr_infos.at(ent->GetIndex()).player;
	if (!bTickIsValid(pCPlayer->TickCount))
		return;
	RestoreTemporaryRecord(ent);
}
void cbacktracking::ClearRecord(IClientEntity* Entity) {
	arr_infos[Entity->GetIndex()] = CBacktrack_Info();
}
BackTrack* backtracking = new BackTrack();
backtrackData headPositions[64][12];
thiccboi hdfdunn[64][12];
cbacktracking* bt_2 = new cbacktracking();
CTickRecord * tickrec = new CTickRecord();
