#pragma once

#include "SDK.h"
#include <Psapi.h>

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

using ServerRankRevealAllFn = bool(__cdecl*)(float*);

struct Hitbox
{
	Hitbox(void)
	{
		hitbox = -1;
	}

	Hitbox(int newHitBox)
	{
		hitbox = newHitBox;
	}

	int  hitbox;
	vector points[9];
};

enum
{
	FL_HIGH = (1 << 0),
	FL_LOW = (1 << 1),
	FL_SPECIAL = (1 << 2)
};

struct BestPoint
{
	BestPoint(void)
	{
		hitbox = -1;
		point = vector(0, 0, 0);
		index = -1;
		dmg = -1;
		flags = 0;
	}

	BestPoint(int newHitBox)
	{
		hitbox = newHitBox;
		point = vector(0, 0, 0);
		index = -1;
		dmg = -1;
		flags = 0;
	}

	vector point;
	int  index;
	int  dmg;
	int  flags;
	int  hitbox;
};


namespace game_utils
{
	extern DWORD FindPattern1(std::string moduleName, std::string pattern);

	extern DWORD find_pattern_xy0(const char * module_name, const BYTE * mask, const char * mask_string);

	std::uint8_t * pattern_scan(void * module, const char * signature);
	extern ServerRankRevealAllFn ServerRankRevealAllEx;
	void NormaliseViewAngle(vector &angle);
	void CL_FixMove(CUserCmd* cmd, vector viewangles);
	bool is_visible(IClientEntity* pLocal, IClientEntity* pEntity, int BoneID);
	
	bool World2Screen(const vector & origin, vector & screen);

//	bool IsBallisticWeapon(void* weapon);
//	bool IsKnife(void * weapon);

	
	bool IsPistol(void* weapon);
	bool IsKnife(void * weapon);
	bool AutoSniper(void * weapon);
	bool IsMachinegun(void * weapon);
	bool IsMP(void * weapon);
	bool AWP(void* weapon);

	bool IsBomb(void * weapon);
	bool IsGrenade(void * weapon);
	bool IsRevolver(void * weapon);
	bool IsSniper(void* weapon);
	bool LightSniper(void * weapon);
	bool IsRifle(void * weapon);
	bool IsShotgun(void * weapon);
	bool IsZeus(void * weapon);
	bool IsScout(void * weapon);
	bool IsScopedWeapon(void* weapon);
	int GetPlayerCompRank(IClientEntity* pEntity);
	extern void ServerRankRevealAll();
	unsigned short UTIL_GetAchievementEventMask();
};

void UTIL_TraceLine(const vector& vecAbsStart, const vector& vecAbsEnd, unsigned int mask, const IClientEntity *ignore, int collisionGroup, trace_t *ptr);

void UTIL_ClipTraceToPlayers(const vector& vecAbsStart, const vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr);
void ClipToPlayer(const vector& vecAbsStart, const vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr);
bool IsBreakableEntity(IClientEntity* ent);

bool TraceToExit(vector& end, trace_t& tr, vector start, vector vEnd, trace_t* trace);

void SayInChat(const char *text);

void SayInTeamChat(const char *text);

void SetName(std::string name);

vector GetHitpointPosition(IClientEntity* pEntity, int Hitbox, vector *Point);

//bool GetBestPoint(IClientEntity* pEntity, Hitbox* hitbox, Vector *point);

vector GetHitboxPosition(IClientEntity* pEntity, int Hitbox, float this_sim);
vector hitbox_location(IClientEntity* obj, int hitbox_id);
//Vector GetHitscan(IClientEntity* pEntity, int Hitbox); hitbox_location
QAngle CalcAngleA(vector src, vector dst);
vector GetEyePosition(IClientEntity* pEntity);

//unsigned short UTIL_GetAchievementEventMask();