#pragma once
#include "Hooks.h"
#include "Hacks.h"
#include "Singleton.hpp"

struct player_anims
{
	CBaseHandle handle;
	float spawn_time;
	CBaseAnimState* animstate;
	float animvel;
};
class c_animfix : public Singleton< c_animfix > {
public:

	bool           shoot[65];
	bool           hit[65];

	CBaseAnimState *animstate[65];
//	std::array< bool, 64 > on_ground{ false };
//	std::array< bool, 64 > last_on_ground{ false };

//	std::array< vec_t, 64 > speed{ vec_t() };
//	std::array< vec_t, 64 > last_speed{ vec_t() };

	void update_animations(IClientEntity* player);
	float get_pitch(IClientEntity * player);
	void clear();
	void re_work(ClientFrameStage_t stage);
	void fix_local_player_animations(CUserCmd * pcmd);
	float proper = 0.f;
	player_anims player_data[64];

};

extern std::unique_ptr< c_animfix > animfix;