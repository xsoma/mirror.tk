#pragma once

extern float last_real;
extern float lineLBY;
extern float lineRealAngle;
extern float lineFakeAngle;
extern float lspeed;
extern float pitchmeme;
extern float lby2;
extern bool DoUnload;
extern bool bGlovesNeedUpdate;
extern int missedLogHits[65];

extern float lineLBY2;
extern float current_desync;
extern float inaccuracy;

extern bool round_change;

#include "Vector.h"
extern vector LastAngleAAReal;
extern vector LastAngleAAFake;

#include "Utilities.h"
#include "Interfaces.h"

namespace Hooks
{
	void Initialise();

	void UndoHooks();


	extern Utilities::Memory::VMTManager VMTPanel; 
	extern Utilities::Memory::VMTManager VMTClient; 
	extern Utilities::Memory::VMTManager VMTClientMode;
	extern Utilities::Memory::VMTManager VMTModelRender;
	extern Utilities::Memory::VMTManager VMTPrediction; 
	extern Utilities::Memory::VMTManager VMTRenderView;
	extern Utilities::Memory::VMTManager VMTEventManager;
	extern Utilities::Memory::VMTManager VMTCreateMove;
	extern RecvVarProxyFn g_fnSequenceProxyFn;
	void SetViewModelSequence(const CRecvProxyData *pDataConst, void *pStruct, void *pOut);

};

// put this here so we can sperate the hooks into it's own file
extern void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld);
extern void __fastcall Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
extern bool __fastcall Hooked_FireEventClientSide(PVOID ECX, PVOID EDX, IGameEvent* Event);
extern void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld);
extern void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup);
extern void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup& setup, CViewSetup& hudViewSetup, int nClearFlags, int whatToDraw);
extern void __fastcall	Hooked_SceneEnd(void* pEcx, void* pEdx);
extern vector* _fastcall Hooked_EyeAngles(void* ecx, void* edx);

extern void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
extern bool __stdcall Hooked_InPrediction();
extern bool __stdcall Hooked_CreateMoveClient(float frametime, CUserCmd* pCmd);
extern int __stdcall Hooked_DoPostScreenEffects(int a1);
HRESULT __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
extern float __stdcall Hooked_GetViewModelFOV();
extern void __stdcall Hooked_LockCursor();

// hooking def shit
typedef void(__stdcall* FrameStageNotifyFn)(ClientFrameStage_t);
extern FrameStageNotifyFn oFrameStageNotify;

typedef void(__thiscall* DrawModelEx_)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);
extern DrawModelEx_ oDrawModelExecute;

typedef void(__thiscall* PaintTraverse_)(PVOID, unsigned int, bool, bool);
extern PaintTraverse_ oPaintTraverse;

typedef bool(__thiscall* WriteUsercmdDeltaToBufferFn) (void*, int, void*, int, int, bool);
extern WriteUsercmdDeltaToBufferFn o_buffer;

typedef bool(__thiscall* InPrediction_)(PVOID);

typedef long(__stdcall* EndScene_t)(IDirect3DDevice9*);
extern EndScene_t o_EndScene;

typedef int(__thiscall* DoPostScreenEffects_t)(IClientModeShared*, int);
extern DoPostScreenEffects_t o_DoPostScreenEffects;

typedef bool(__thiscall* FireEventClientSideFn)(PVOID, IGameEvent*);
extern FireEventClientSideFn oFireEventClientSide;

typedef long(__stdcall* Reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
extern Reset_t o_Reset;

typedef void(__thiscall* RenderViewFn)(void*, CViewSetup&, CViewSetup&, int, int);
extern RenderViewFn oRenderView;

using OverrideViewFn = void(__fastcall*)(void*, void*, CViewSetup*);
extern OverrideViewFn oOverrideView;

typedef void(__thiscall* SceneEnd_t)(void* pEcx);
extern SceneEnd_t pSceneEnd;

typedef void(__thiscall* LockCursor)(void*);
extern LockCursor oLockCursor;

typedef float(__stdcall* oGetViewModelFOV)();

using EyeAnglesFn = vector * (__thiscall*)(void*);
extern EyeAnglesFn original_eyeangles;

#include "EnginePrediction.h"
static CPredictionSystem* Prediction = new CPredictionSystem();

typedef void(__cdecl* MsgFn)(const char* msg, va_list); // msg related in fireeventclientside.cpp
















namespace Resolver
{
	extern bool didhitHS;
	extern bool hitbaim;
	extern bool lbyUpdate;
}

class lag_fix
{
public:
	void lag_fix_main(IClientEntity* entity);
};

namespace bigboi
{
	extern bool freestand;
	extern int indicator;	
	extern int desyncside;
	extern int freestandval;
	extern float current_yaw;
}
extern bool warmup;

class AnimatedClanTag
{
public:
//	void Tick();
};

extern AnimatedClanTag *animatedClanTag;

class beam
{
	struct BulletImpact_t
	{
		float			flImpactTime;
		vector			vecImpactPos;
		Color			color;
		IClientEntity*	pPlayer;

		__forceinline BulletImpact_t()
		{
			vecImpactPos = { 0.0f, 0.0f, 0.0f };
			flImpactTime = 0.0f;
			color = (255, 255, 255);
			pPlayer = nullptr;
		}

		__forceinline BulletImpact_t(IClientEntity* player, vector pos, float time, Color col = (255, 255, 255))
		{
			pPlayer = player;
			flImpactTime = time;
			vecImpactPos = pos;
			color = col;
		}
	};
	std::vector<std::pair<int, float>>		Entities;
	std::deque<BulletImpact_t>		Impacts;
public:

	void DrawBulletBeams();
	
	bool glow = false;

	float real = 0.f;
	float lby = 0.f;
	float duck = 0.f;

	float visual_angle = 0.f;

	vector fake;
	vector cham_origin;
	vector cham_angle;
};
extern beam *c_beam;


