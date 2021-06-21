#pragma once
//way shorter but still same effect lol
#include "SDK.h"

class CPredictionSystem
{
public:

	CMoveData m_MoveData;

	void StartPrediction(CUserCmd * pCmd);
	void EndPrediction(CUserCmd * pCmd);
//	int post_think(IClientEntity * player) const;

private:
	void* m_movedata_ = nullptr;
	int* m_prediction_player_ = nullptr;
	int* m_prediction_seed_ = nullptr;
	float m_flOldCurtime;
	float m_flOldFrametime;
	int ticks;
	int steps;
	bool jump;
	vector unpred_eyepos;

	int m_nServerCommandsAcknowledged;
	bool m_bInPrediction;

	struct globals_t
	{
		float frametime;
		float curtime;
		int tickcount;
	} m_old_globals_ = {};
	struct player_t
	{
		int flags;
		vector velocity;
	} m_old_player_ = {};
}; extern CPredictionSystem * e_prediction;