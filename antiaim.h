#pragma once
#include "Hacks.h"
class anti_aim
{
public:

	void DoPitch(CUserCmd * pCmd);

	void run_manual_inputs();

	float get_curtime(CUserCmd * ucmd, IClientEntity * get_local);

	bool extent_moving_desync(CUserCmd * cmd);

	bool next_lby_update(const float yaw_to_break, CUserCmd * cmd);

	void DoYaw(CUserCmd * pCmd, IClientEntity * pLocal, bool & bSendPacket);
	float yaw_mod(int m, int a);
//	float yaw_mod(float m, float a);
	void selection(CUserCmd * pcmd, bool moving, bool air, IClientEntity * plocal);
	bool _do();
	void DoAntiAim(CUserCmd *pCmd, bool&bSendPacket);


	//---- pitches ----//

	void pitchdown(CUserCmd * pcmd);

	void pitchup(CUserCmd * pcmd);

	void zero(CUserCmd * pcmd);

	void pitchjitter(CUserCmd * pcmd);

	void pitch_fakejitter(CUserCmd * pcmd);

	void pitchrandom(CUserCmd * pcmd);

	void pitch_fakerandom(CUserCmd * pcmd);

	void fakedown(CUserCmd * pcmd);

	void fakeup(CUserCmd * pcmd);

	//---- yaws ----//

	void backwards(CUserCmd * pcmd, bool moving);

	void lowerbody(CUserCmd * pcmd, bool moving);

	void rand_lowerbody(CUserCmd * pcmd, bool moving);

	void jitter_180(CUserCmd * pcmd, bool moving);

	void manual(CUserCmd * pCmd, bool moving);

	void freestanding_jitter(CUserCmd * pCmd, bool moving);

	int GetFPS();

	void anti_lby2(CUserCmd * cmd, bool & bSendPacket);

	float maxdelta(IClientEntity * local);

	void do_yawangle(CUserCmd * cmd, bool & packet, bool moving, IClientEntity * local);

	void freestanding(CUserCmd * cmd);

	void crooked(CUserCmd * pcmd);


	//---- desync ----//

	void desync_jitter(CUserCmd * pcmd, IClientEntity * g_local, bool send_packet, bool moving);


private:
	bool stand_done = false;
	bool move_done = false;

	//	bool swap_sides_stand = Options::Menu.MiscTab.desync_swapsides_stand.GetState();
	//	bool swap_sides_move = Options::Menu.MiscTab.desync_swapsides_move.GetState();

	float stand_range = options::menu.misc.desync_range_stand.GetValue();
	float move_range = options::menu.misc.desync_range_move.GetValue();

	float standjit = options::menu.misc.stand_jitter.GetValue();
	float movejit = options::menu.misc.move_jitter.GetValue();

	int manual_index;
};

class antiaim_helper
{
public:

	void anti_lby(CUserCmd * cmd, bool & bSendPacket);

	float fov_ent(vector ViewOffSet, vector View, IClientEntity * entity, int hitbox);

	int closest();

private:

	float next_lby_update = 0.f;
	int closest_entity = -1;
	float NextPredictedLBYUpdate = 0.f;

};

extern anti_aim * c_antiaim;
extern antiaim_helper * c_helper;