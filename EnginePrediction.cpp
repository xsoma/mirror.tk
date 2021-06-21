#include "EnginePrediction.h"
#include "NetVars.h"
#include "XorStr.hpp"
#include "MathFunctions.h"
#include "Hacks.h"
#include "EnginePrediction.h"

int *m_nPredictionRandomSeed;
int *m_pSetPredictionPlayer;

float flOldCurtime;
float flOldFrametime;

CPredictionSystem * e_prediction = new CPredictionSystem();

void SetCurrentCommand(CUserCmd *cmd, IClientEntity * p)
{
	static int m_hConstraintEntity = NetvarManager::Instance()->GetOffset("DT_BasePlayer", "localdata", "m_hConstraintEntity");
	*reinterpret_cast<CUserCmd**>(uintptr_t(p) + m_hConstraintEntity - 0xC) = cmd;
}
void CPredictionSystem::StartPrediction(CUserCmd* pCmd)
{
	static int flTickBase;

	auto player = reinterpret_cast<IClientEntity*>(interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer()));

	if (!player || !pCmd)
		return;

	static bool bInit = false;
	if (!bInit) {
		m_nPredictionRandomSeed = *(int**)(Utilities::Memory::FindPatternV2("client.dll", "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);
		bInit = true;
	}

	memset(&m_MoveData, 0, sizeof(m_MoveData));
	flOldCurtime = interfaces::globals->curtime;
	flOldFrametime = interfaces::globals->frametime;

	SetCurrentCommand(pCmd, player);
	interfaces::globals->curtime = player->GetTickBase() * interfaces::globals->interval_per_tick;
	interfaces::globals->frametime = interfaces::globals->interval_per_tick;

	player->set_local_view_angles(pCmd->viewangles);

	//	if (player->physics_run_think(0)) {
	//		player->pre_think();
	//	}
	interfaces::c_gamemove->StartTrackPredictionErrors(player);
	interfaces::movehelper->SetHost(player);
	interfaces::c_prediction->SetupMove(player, pCmd, interfaces::movehelper, &m_MoveData);

	if (!(pCmd->buttons & IN_JUMP))
		interfaces::c_gamemove->ProcessMovement(player, &m_MoveData);

	interfaces::c_prediction->FinishMove(player, pCmd, &m_MoveData);
	interfaces::c_gamemove->FinishTrackPredictionErrors(player);

	interfaces::movehelper->SetHost(nullptr);

	*m_nPredictionRandomSeed = -1;
	SetCurrentCommand(nullptr, player);

}

void CPredictionSystem::EndPrediction(CUserCmd* pCmd)
{
	auto player = reinterpret_cast< IClientEntity* >(interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer()));

	if (!player)
		return;

	interfaces::globals->curtime = flOldCurtime;
	interfaces::globals->frametime = flOldFrametime;

}


