#pragma once

#include "CommonIncludes.h"
#include "SDK.h"
#include "CClientState.h"
#include "glow_outline_effect.h"
#include "IClientMode.h"
#include <d3d9.h>
#include "CUtlVector.h"

template < typename t >__forceinline t v_func(void* thisptr, std::uintptr_t v_index) {
	if (!thisptr) {
#if defined(_DEBUG)
		std::cout << "[ERROR] - couldn't find vfunc with index" << v_index << "\n";
		throw std::exception(std::string("couldn't find vfunc with index " + std::to_string(v_index)).data());
#endif
		return *(t*)nullptr;
	}

	return t((*(std::uintptr_t**) thisptr)[v_index]);
}

class IMoveHelper : public Interface
{
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	int m_nPlayerHandle; // edict index on server, client entity handle on client=
	int m_nImpulseCommand; // Impulse command issued.
	QAngle m_vecViewAngles; // Command view angles (local space)
	QAngle m_vecAbsViewAngles; // Command view angles (world space)
	int m_nButtons; // Attack buttons.
	int m_nOldButtons; // From host_client->oldbuttons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;
	float m_flMaxSpeed;
	float m_flClientMaxSpeed;
	vector m_vecVelocity; // edict::velocity // Current movement direction.
	QAngle m_vecAngles; // edict::angles
	QAngle m_vecOldAngles;
	float m_outStepHeight; // how much you climbed this move
	vector m_outWishVel; // This is where you tried 
	vector m_outJumpVel; // This is your jump velocity
	vector m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	float m_flUnknown[5];
	vector m_vecAbsOrigin;

	virtual	void _vpad() = 0;
	virtual void SetHost(IClientEntity* host) = 0;
};

class CMoveData {
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	bool m_bNoAirControl : 1;

	unsigned long m_nPlayerHandle;
	int m_nImpulseCommand;
	QAngle m_vecViewAngles;
	QAngle m_vecAbsViewAngles;
	int m_nButtons;
	int m_nOldButtons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	vector m_vecVelocity;
	vector m_vecOldVelocity;
	float somefloat;
	QAngle m_vecAngles;
	QAngle m_vecOldAngles;

	float m_outStepHeight;
	vector m_outWishVel;
	vector m_outJumpVel;

	vector m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	bool m_bConstraintPastRadius;

	void SetAbsOrigin(const vector& vec);
	const vector& GetOrigin() const;

private:
	vector m_vecAbsOrigin;
};

class IPrediction : public Interface
{
public:

void SetupMove(IClientEntity *player, CUserCmd *pCmd, IMoveHelper *pHelper, void *move)
	{
		typedef void(__thiscall* SetupMoveFn)(void*, IClientEntity*, CUserCmd*, IMoveHelper*, void*);
		return call_virtual_test<SetupMoveFn>(this, 20)(this, player, pCmd, pHelper, move);
	}	

	void	FinishMove(IClientEntity *player, CUserCmd *pCmd, void *move)
	{
		typedef void(__thiscall* FinishMoveFn)(void*, IClientEntity*, CUserCmd*, void*);
		return call_virtual_test<FinishMoveFn>(this, 21)(this, player, pCmd, move);
	}

	void set_local_viewangles_rebuilt(vector& angle)
	{
		typedef void(__thiscall *o_updateClientSideAnimation)(void*, vector&);
		call_vfunc<o_updateClientSideAnimation>(this, 13)(this, angle);
	}

	void CheckMovingGround(IClientEntity *player, double frametime)
	{
		using fnCheckMovingGround = void(__thiscall *)(void *, IClientEntity *, double);
		call_vfunc< fnCheckMovingGround >(this, 18)(this, player, frametime);
	}

	char pad00[8]; 					// 0x0000
	bool m_bInPrediction;				// 0x0008
	char pad01[1];					// 0x0009
	bool m_bEnginePaused;				// 0x000A
	char pad02[13];					// 0x000B
	bool m_bIsFirstTimePredicted;		// 0x0018
};

class IGameMovement : public Interface
{
public:
	virtual			~IGameMovement(void) {}

	virtual void	ProcessMovement(IClientEntity *pPlayer, CMoveData *pMove) = 0;
	virtual void	Reset(void) = 0;
	virtual void	StartTrackPredictionErrors(IClientEntity *pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(IClientEntity *pPlayer) = 0;
	virtual void	DiffPrint(char const *fmt, ...) = 0;

	virtual vector const&	GetPlayerMins(bool ducked) const = 0;
	virtual vector const&	GetPlayerMaxs(bool ducked) const = 0;
	virtual vector const&   GetPlayerViewOffset(bool ducked) const = 0;

	virtual bool			IsMovingPlayerStuck(void) const = 0;
	virtual IClientEntity*	GetMovingPlayer(void) const = 0;
	virtual void			UnblockPusher(IClientEntity* pPlayer, IClientEntity *pPusher) = 0;
};

class CGameMovement
{
public:
	virtual			~CGameMovement(void) {}

	virtual void	ProcessMovement(IClientEntity *pPlayer, CMoveData *pMove) = 0;
	virtual void	Reset(void) = 0;
	virtual void	StartTrackPredictionErrors(IClientEntity *pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(IClientEntity *pPlayer) = 0;
	virtual void	DiffPrint(char const *fmt, ...) = 0;

	virtual vector const&	GetPlayerMins(bool ducked) const = 0;
	virtual vector const&	GetPlayerMaxs(bool ducked) const = 0;
	virtual vector const&   GetPlayerViewOffset(bool ducked) const = 0;

	virtual bool			IsMovingPlayerStuck(void) const = 0;
	virtual IClientEntity*	GetMovingPlayer(void) const = 0;
	virtual void			UnblockPusher(IClientEntity* pPlayer, IClientEntity *pPusher) = 0;

	virtual void    SetupMovementBounds(CMoveData *pMove) = 0;
};

class IMemAlloc
{
public:
	// Release versions
	virtual void *alloc(size_t nSize) = 0;
	virtual void *realloc(void *pMem, size_t nSize) = 0;
	virtual void  free(void *pMem) = 0;
	virtual void *Expand_NoLongerSupported(void *pMem, size_t nSize) = 0;

	// Debug versions
	virtual void *alloc(size_t nSize, const char *pFileName, int nLine) = 0;
	virtual void *realloc(void *pMem, size_t nSize, const char *pFileName, int nLine) = 0;
	virtual void  free(void *pMem, const char *pFileName, int nLine) = 0;
	virtual void *Expand_NoLongerSupported(void *pMem, size_t nSize, const char *pFileName, int nLine) = 0;

	// Returns size of a particular allocation
	virtual size_t get_size(void *pMem) = 0;

	// Force file + line information for an allocation
	virtual void push_alloc_dbg_info(const char *pFileName, int nLine) = 0;
	virtual void pop_alloc_dbg_info() = 0;

	virtual long crt_set_break_alloc(long lNewBreakAlloc) = 0;
	virtual	int crt_set_report_mode(int nReportType, int nReportMode) = 0;
	virtual int crt_is_valid_heap_pointer(const void *pMem) = 0;
	virtual int crt_is_valid_pointer(const void *pMem, unsigned int size, int access) = 0;
	virtual int crt_check_memory(void) = 0;
	virtual int crt_set_dbg_flag(int nNewFlag) = 0;
	virtual void crt_mem_checkpoint(_CrtMemState *pState) = 0;
	virtual int heapchk() = 0;

	virtual void dump_stats() = 0;
	virtual void dump_stats_file_base(char const *pchFileBase) = 0;
};

namespace interfaces
{
	void Initialise();
	extern IBaseClientDLL* client;
	extern IVEngineClient* engine;
	extern IPanel* panels;
	extern IClientEntityList* ent_list;
	extern ISurface* surface;
	extern IVDebugOverlay* DebugOverlay;
	extern IClientModeShared *ClientMode;
	extern CGlobalVarsBase *globals;
	extern DWORD *prediction_dword;
	extern CMaterialSystem* materialsystem;
	extern CVRenderView* render_view;
	extern IDirect3DDevice9 *g_pD3DDevice9;
	extern IVModelRender* model_render;
	extern CGlowObjectManager* glow_manager;
	extern CModelInfo* model_info;
	extern HWND window;
	extern IInputSystem* m_iInputSys;
	extern CUtlVectorSimple *g_ClientSideAnimationList;
	extern IEngineTrace* trace;
	extern Trace2* trace_2;
	extern IPhysicsSurfaceProps* phys_props;
	extern ICVar *cvar;
	extern CInput* pinput;
	extern IGameEventManager2 *event_manager;
	extern HANDLE __FNTHANDLE;
	extern IVEffects* effects;
	extern IVEffects* effects;
	extern IMoveHelper* movehelper;
	extern IPrediction *c_prediction;
	extern CClientState* client_state;
	extern IGameMovement* gamemovement;
	extern IViewRenderBeams* render_beams;
	extern	CGameMovement * c_gamemove;
	extern IMemAlloc* mem_alloc;
};
