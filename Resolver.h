#include <array>
#include <string>
#include <deque>
#include <algorithm>
#include "Entities.h"
#include "CommonIncludes.h"
#include "Entities.h"
#include "Vector.h"
#include <map>
#include "Interfaces.h"
#include "Hacks.h"
#include "Hooks.h"

enum correction_flags
{
	resolve,
	spread,
	lag,
	precise_hit,
	none
};
struct extra
{
	int current_flag[64];
};
extern extra * ext;
struct resolver_infos
{
	AnimationLayer animation;

	bool jitter_desync = false, high_delta = false, low_delta = false, static_desync = false, lby_changed = false;

	float last_lby = FLT_MAX, lby_delta = FLT_MAX, last_eyes = FLT_MAX, eye_delta = FLT_MAX, eye_lby_delta = FLT_MAX, eyes = FLT_MAX;

};

struct resolver_record
{
	std::deque< resolver_infos > tick_records;

	resolver_infos current_tick;
	resolver_infos previous_tick;

};

class angle_correction
{
public:

	bool breaking_lby_animations(IClientEntity * e);

	bool solve_desync_simple(IClientEntity * e);

	void mirror_aesthetic_console();

	bool laurie_aa[65] = { false };
	bool breaking[65] = { false };

	int resolver_stage;
	int resolver_flag[65];
	bool yes = false;
	void missed_due_to_desync(IGameEvent* event);
	void ConColorMsg(Color const & color, const char * buf, ...);

private:
	resolver_record _player[64];

}; extern angle_correction * a_c;


class resolver_setup
{
public:
	bool didhit;

	//	void resolver_main(IClientEntity * pEntity);

	void fakeangles(IClientEntity * localplayer, IClientEntity * enemy);

	void side_detect(IClientEntity* player, int* side);

	bool solve_desync_simple(IClientEntity* e);

	float mirror_release_basic(IClientEntity * player, IClientEntity * local);

	void auto_config(IClientEntity * local, IClientEntity * player, bool head);

	float mirror_release_brute(IClientEntity * localplayer, IClientEntity  * enemy);

	void FSN(IClientEntity* pEntity, ClientFrameStage_t stage); //used to get the fresh THINGS and the resolve

	void preso(IClientEntity * pEntity);

	bool enemy_slowwalk = false;
	bool enemy_inair;
	bool enemy_fake[65] = { false };
	bool enemy_hitbox_blacklist[65] = { false };
	bool enemy_shot[65] = { false };
	bool has_desync[65] = { false };

	vector get_resolved[65] = { vector(0, 0, 0) };

	static resolver_setup GetInst()
	{
		static resolver_setup instance;
		return instance;
	}

	float lbyadjust[65] = { 0.f };
}; extern resolver_setup * resolver;

namespace Globals
{
	extern CUserCmd* UserCmd;
	extern IClientEntity* Target;
	extern int Shots;
	extern int Hits;
	extern int Missed;
	extern int fired[65];
	extern bool hit[65];
	extern bool shot[65];
	extern bool change;
	extern int TargetID;
	extern bool didhitHS;
	extern int missedshots[65];
	extern std::map<int, QAngle> storedshit;
	extern bool Up2date;
	extern vector aim_point;

	extern int m_nTickbaseShift;
	extern bool bInSendMove;
	extern int32_t nSinceUse;
	extern bool bFirstSendMovePack;
}