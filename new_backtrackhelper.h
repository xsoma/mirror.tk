#pragma once
#include "SDK.h"
#include "Hooks.h"
#include "newbacktrack.h"
struct tick_record
{
	vector m_vecOrigin;//0 //4 // 8
	bool bLowerBodyYawUpdated;
	bool bFakelagging;
	float m_flSimulationTime; //12
	float m_flAnimTime;
	int m_nSequence;//16
	float m_flCycle; //20
	vector m_angEyeAngles;//x->24
	float m_flUpdateTime;//36
	vector m_vecVelocity;//40 //44 //48
	float m_flPoseParameter[24];//52
	float backtrack_time;
	bool shot_in_that_record = false;
	bool needs_extrapolation = false;
	/*my addition*/
	matrix3x4 boneMatrix[128];
};
struct player_record
{
	//std::vector<tick_record> records;
	std::vector<tick_record> records;
	vector EyeAngles;
	float LowerBodyYawTarget;
	int Flags;
	float unknown;
};

struct simulation_record
{
	vector origin;
	vector velocity;
	vector acceleration;
	float simulation_time;
	float simulation_time_increasment_per_tick;
	float update_time;

};
class CBacktrackHelper
{
public:
	player_record PlayerRecord[64];
	simulation_record SimRecord[64][7];
	float GetLerpTime();
	int GetDesiredTickCount(float flTargetTime);
	float GetNetworkLatency();
	int GetLatencyTicks();
	int GetEstimateServerTickCount();
	float GetEstimateServerTime();
	void UpdateBacktrackRecords(IClientEntity* pPlayer);
	void UpdateExtrapolationRecords(IClientEntity* pPlayer);
	void ExtrapolatePosition(IClientEntity* pPlayer, vector& position, float &simtime, vector velocity);
//	void LagCompensate(IClientEntity* pPlayer);
	bool IsTickValid(tick_record record);


}; extern CBacktrackHelper* g_BacktrackHelper;