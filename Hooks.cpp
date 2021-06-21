
#include "includes.h"

#include "VMT.h"

EndScene_t o_EndScene;
SceneEnd_t pSceneEnd;
Reset_t o_Reset;
DoPostScreenEffects_t o_DoPostScreenEffects;
PaintTraverse_ oPaintTraverse;
DrawModelEx_ oDrawModelExecute;
FrameStageNotifyFn oFrameStageNotify;
OverrideViewFn oOverrideView;
FireEventClientSideFn oFireEventClientSide;
RenderViewFn oRenderView;
WriteUsercmdDeltaToBufferFn o_buffer;
LockCursor oLockCursor;
EyeAnglesFn original_eyeangles;

namespace Hooks
{
	Utilities::Memory::VMTManager VMTPanel;
	Utilities::Memory::VMTManager VMTClient;
	Utilities::Memory::VMTManager VMTClientMode;
	Utilities::Memory::VMTManager VMTModelRender;
	Utilities::Memory::VMTManager VMTPrediction;
	Utilities::Memory::VMTManager VMTRenderView;
	Utilities::Memory::VMTManager VMTEventManager;
	Utilities::Memory::VMTManager VMTDIRECTX;
	Utilities::Memory::VMTManager VMTSurface;
	Utilities::Memory::VMTManager VMTccsplayer_b;

	VMT::VMTHookManager ccsplayer_b; // jono hook test

	RecvVarProxyFn g_fnSequenceProxyFn = NULL;
};

void Hooks::UndoHooks()
{
	VMTPanel.RestoreOriginal();
	VMTPrediction.RestoreOriginal();
	VMTModelRender.RestoreOriginal();
	VMTClientMode.RestoreOriginal();
	VMTDIRECTX.RestoreOriginal();
	VMTEventManager.RestoreOriginal();
	VMTSurface.RestoreOriginal();
	VMTRenderView.RestoreOriginal();
	VMTClient.RestoreOriginal();
}

void Hooks::Initialise()
{
	interfaces::engine->ExecuteClientCmd("clear"); // why?

	VMTDIRECTX.Initialise((DWORD*)interfaces::g_pD3DDevice9);
	VMTDIRECTX.Initialise((DWORD*)interfaces::g_pD3DDevice9);
	o_Reset = (Reset_t)VMTDIRECTX.HookMethod((DWORD)&Hooked_Reset, 16);

	VMTClientMode.Initialise((DWORD*)interfaces::ClientMode);
	VMTClientMode.HookMethod((DWORD)&Hooked_GetViewModelFOV, 35);
	VMTClientMode.HookMethod((DWORD)Hooked_CreateMoveClient, 24);

	o_DoPostScreenEffects = (DoPostScreenEffects_t)VMTClientMode.HookMethod((DWORD)Hooked_DoPostScreenEffects, 44);
	oOverrideView = (OverrideViewFn)VMTClientMode.HookMethod((DWORD)&Hooked_OverrideView, 18);

	VMTClient.Initialise((DWORD*)interfaces::client);
	oFrameStageNotify = (FrameStageNotifyFn)VMTClient.HookMethod((DWORD)&Hooked_FrameStageNotify, 37);

	VMTPanel.Initialise((DWORD*)interfaces::panels);
	oPaintTraverse = (PaintTraverse_)VMTPanel.HookMethod((DWORD)&Hooked_PaintTraverse, Offsets::VMT::Panel_PaintTraverse);

	VMTPrediction.Initialise((DWORD*)interfaces::prediction_dword);
	VMTPrediction.HookMethod((DWORD)&Hooked_InPrediction, 14);

	VMTModelRender.Initialise((DWORD*)interfaces::model_render);
	oDrawModelExecute = (DrawModelEx_)VMTModelRender.HookMethod((DWORD)&Hooked_DrawModelExecute, Offsets::VMT::ModelRender_DrawModelExecute);


	VMTEventManager.Initialise((DWORD*)interfaces::event_manager);
	oFireEventClientSide = (FireEventClientSideFn)VMTEventManager.HookMethod((DWORD)&Hooked_FireEventClientSide, 9);

	VMTRenderView.Initialise((DWORD*)interfaces::render_view);
	pSceneEnd = (SceneEnd_t)VMTRenderView.HookMethod((DWORD)&Hooked_SceneEnd, 9);

	VMTSurface.Initialise((DWORD*)interfaces::surface);
	oLockCursor = (LockCursor)VMTSurface.HookMethod((DWORD)Hooked_LockCursor, 67);

//	VMTccsplayer_b.Initialise((DWORD*)game_utils::FindPattern1("client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C") + 0x47);

	uintptr_t player = game_utils::FindPattern1("client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C") + 0x47;
	ccsplayer_b.Init(*reinterpret_cast<uintptr_t**> (player));
	original_eyeangles = ccsplayer_b.HookFunction<EyeAnglesFn>(169, Hooked_EyeAngles);

	// ccsplayer_b = new c_bphook( *reinterpret_cast<uintptr_t**> ( OFFSETS::ccsplayer_vtable ) ); // ccsplayer_vtable = UTILS::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C") + 0x47;
	// original_eyeangles = ccsplayer_b->apply<EyeAnglesFn> ( 169, EyeAngles ); // convert these pls

	// this will set thirdperson angle, this is how you do it, also im lazy

	/// shit
	ConVar* nameVar = interfaces::cvar->FindVar("name");
	//--------------- NAME CVAR ---------------//
	if (nameVar)
	{
		*(int*)((DWORD)&nameVar->fnChangeCallback + 0xC) = 0;
	}
	//--------------- EVENT LOG ---------------//

	static auto developer = interfaces::cvar->FindVar("developer");
	developer->SetValue(1);
	static auto con_filter_enable = interfaces::cvar->FindVar("con_filter_enable");
	static auto con_filter_text = interfaces::cvar->FindVar("con_filter_text");
	static auto dogstfu = interfaces::cvar->FindVar("con_notifytime");
	dogstfu->SetValue(3);
	con_filter_text->SetValue(".     ");
	con_filter_enable->SetValue(2);

	for (ClientClass* pClass = interfaces::client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
	{
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			RecvTable* pClassTable = pClass->m_pRecvTable;
			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];
				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;

				// Store the original proxy function.
				Hooks::g_fnSequenceProxyFn = (RecvVarProxyFn)pProp->m_ProxyFn;

				// Replace the proxy function with our sequence changer.
				pProp->m_ProxyFn = Hooks::SetViewModelSequence;

				break;
			}

			break;
		}
	}
}






std::vector<impact_info> impacts;
std::vector<hitmarker_info> Xhitmarkers;
static CUserCmd * get_cmd = new CUserCmd();
beam * c_beam = new beam();

CLagcompensation lagcompensation;
HANDLE worldmodel_handle;
C_BaseCombatWeapon* worldmodel;

vector Globals::aim_point;
int bigboi::indicator;
int bigboi::desyncside;
bool bigboi::freestand;
bool round_change;
int bigboi::freestandval;
std::vector<trace_info> trace_logs;
int currentfov;
vector LastAngleAA;
extern vector LastAngleAA2;
vector LastAngleAAFake;
vector last_fake;
bool Resolver::didhitHS;
CUserCmd* Globals::UserCmd;
IClientEntity* Globals::Target;
int Globals::Shots;
int Globals::Hits;
int Globals::Missed;
bool Globals::change;
int Globals::TargetID;
bool Resolver::hitbaim;
bool Globals::Up2date;
bool Globals::shot[65];
bool Globals::hit[65];
int Globals::fired[65];

int Globals::m_nTickbaseShift;
bool Globals::bInSendMove;
int32_t Globals::nSinceUse;
bool Globals::bFirstSendMovePack;

vector LastAngleAAReal;
vector LBYThirdpersonAngle;

float bigboi::current_yaw;
#define STUDIO_RENDER					0x00000001
std::map<int, QAngle>Globals::storedshit;
int Globals::missedshots[65] = { 0 };
int m_nTickbaseShift;
static int missedLogHits[65];
float fakeangle;

namespace overall
{
	bool bVisualAimbotting = false;
	QAngle vecVisualAimbotAngs = QAngle(0.f, 0.f, 0.f);
	int ChokeAmount = 0;
	float flFakewalked = 0.f;
	bool NewRound = false;
	bool WeaponFire = false;
	QAngle fakeangleslocal;
	bool bRainbowCross = true;
	bool dohitmarker;
	float LastTimeWeFired = 0;
	int ShotsFiredLocally = 0;
	int ShotsHitPerEntity[65];
	bool HeadShottedEntity[65] = { false };
	float curFov = 0;
	bool bUsingFakeAngles[65];
	float HitMarkerAlpha = 0.f;
	int TicksOnGround = 0;
	int fixed_tickbase = 0.f;
	int ticks_alive = 0.f;
	int ticks_while_unducked = 0;
	char* breakmode;
	int AnimationPitchFix = 0;
	float hitchance;
	int NextPredictedLBYUpdate = 0;
	int breakangle;
	int prevChoked = 0;
	bool AAFlip = false;
	char my_documents_folder[MAX_PATH];
	float smt = 0.f;
	QAngle visualAngles = QAngle(0.f, 0.f, 0.f);
	bool bSendPacket = false;
	bool bAimbotting = false;
	CUserCmd* userCMD = nullptr;
	char* szLastFunction = "<No function was called>";
	HMODULE hmDll = nullptr;
	bool bFakewalking = false;
	vector vecUnpredictedVel = vector(0, 0, 0);
	float flFakeLatencyAmount = 0.f;
	float flEstFakeLatencyOnServer = 0.f;
	matrix3x4_t traceHitboxbones[128];
	std::array<std::string, 64> resolverModes;
	int shifted;
	vector bones_sent[128];
	matrix3x4 last_sent_matrix[128];
}

int ground_tick;
vector OldOrigin;

AnimatedClanTag *animatedClanTag = new AnimatedClanTag();


struct CUSTOMVERTEX {
	FLOAT x, y, z;
	FLOAT rhw;
	DWORD color;
};
#define M_PI 3.14159265358979323846
void CircleFilledRainbowColor(float x, float y, float rad, float rotate, int type, int resolution, IDirect3DDevice9* m_device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2;
	std::vector<CUSTOMVERTEX> circle(resolution + 2);
	float angle = rotate * M_PI / 180, pi = M_PI;
	if (type == 1)
		pi = M_PI; // Full circle
	if (type == 2)
		pi = M_PI / 2; // 1/2 circle
	if (type == 3)
		pi = M_PI / 4; // 1/4 circle
	pi = M_PI / type; // 1/4 circle
	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = D3DCOLOR_RGBA(0, 0, 0, 0);
	float hue = 0.f;
	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad * cos(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad * sin(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;
		auto clr = Color::FromHSB(hue, 1.f, 1.f);
		circle[i].color = D3DCOLOR_RGBA(clr.r(), clr.g(), clr.b(), clr.a() - 175);
		hue += 0.02;
	}
	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		float Vx1 = x + (cosf(angle) * (circle[i].x - x) - sinf(angle) * (circle[i].y - y));
		float Vy1 = y + (sinf(angle) * (circle[i].x - x) + cosf(angle) * (circle[i].y - y));
		circle[i].x = Vx1;
		circle[i].y = Vy1;
	}
	m_device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, NULL);
	VOID* pVertices;
	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX));
	g_pVB2->Unlock();
	m_device->SetTexture(0, NULL);
	m_device->SetPixelShader(NULL);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	if (g_pVB2 != NULL)
		g_pVB2->Release();
}
#define M_PI 3.14159265358979323846


void greyone(float x, float y, float rad, float rotate, int type, int resolution, IDirect3DDevice9* m_device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2;
	std::vector<CUSTOMVERTEX> circle(resolution + 2);
	float angle = rotate * M_PI / 180, pi = M_PI;
	if (type == 1)
		pi = M_PI; // Full circle
	if (type == 2)
		pi = M_PI / 2; // 1/2 circle
	if (type == 3)
		pi = M_PI / 4; // 1/4 circle
	pi = M_PI / type; // 1/4 circle
	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = D3DCOLOR_RGBA(0, 0, 0, 0);
	float hue = 0.f;
	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad * cos(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad * sin(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;
		auto clr = Color(15, 15, 15);
		circle[i].color = D3DCOLOR_RGBA(clr.r(), clr.g(), clr.b(), clr.a() - 175);
		hue += 0.02;
	}
	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		float Vx1 = x + (cosf(angle) * (circle[i].x - x) - sinf(angle) * (circle[i].y - y));
		float Vy1 = y + (sinf(angle) * (circle[i].x - x) + cosf(angle) * (circle[i].y - y));
		circle[i].x = Vx1;
		circle[i].y = Vy1;
	}
	m_device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, NULL);
	VOID* pVertices;
	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX));
	g_pVB2->Unlock();
	m_device->SetTexture(0, NULL);
	m_device->SetPixelShader(NULL);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	if (g_pVB2 != NULL)
		g_pVB2->Release();
}
void colorboy69(float x, float y, float rad, float rotate, int type, int resolution, IDirect3DDevice9* m_device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2;
	std::vector<CUSTOMVERTEX> circle(resolution + 2);
	float angle = rotate * M_PI / 180, pi = M_PI;
	if (type == 1)
		pi = M_PI; // Full circle
	if (type == 2)
		pi = M_PI / 2; // 1/2 circle
	if (type == 3)
		pi = M_PI / 4; // 1/4 circle
	pi = M_PI / type; // 1/4 circle
	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = D3DCOLOR_RGBA(0, 0, 0, 0);
	float hue = 0.f;
	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad * cos(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad * sin(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;
		auto clr = (Color)options::menu.ColorsTab.spreadcrosscol.GetValue();
		circle[i].color = D3DCOLOR_RGBA(clr.r(), clr.g(), clr.b(), clr.a() - 175);
		hue += 0.02;
	}
	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		float Vx1 = x + (cosf(angle) * (circle[i].x - x) - sinf(angle) * (circle[i].y - y));
		float Vy1 = y + (sinf(angle) * (circle[i].x - x) + cosf(angle) * (circle[i].y - y));
		circle[i].x = Vx1;
		circle[i].y = Vy1;
	}
	m_device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, NULL);
	VOID* pVertices;
	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX));
	g_pVB2->Unlock();
	m_device->SetTexture(0, NULL);
	m_device->SetPixelShader(NULL);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	if (g_pVB2 != NULL)
		g_pVB2->Release();
}
/*
HRESULT __stdcall EndScene_hooked(IDirect3DDevice9 *pDevice)
{
	//this will probably get drawn even over the console and other CSGO hud elements, but whatever
	//this will also draw over the menu so we should disable it if the menu is open



	return o_EndScene(pDevice);
}
*/

float clip(float n, float lower, float upper)
{
	return (std::max)(lower, (std::min)(n, upper));
}
int kek = 0;
int autism = 0;
int speed = 0;
static float testtimeToTick;
static float testServerTick;
static float testTickCount64 = 1;

float NormalizeYaw(float value)
{
	while (value > 180)
		value -= 360.f;
	while (value < -180)
		value += 360.f;
	return value;
}
float random_float(float min, float max)
{
	typedef float(*RandomFloat_t)(float, float);
	static RandomFloat_t m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle(("vstdlib.dll")), ("RandomFloat"));
	return m_RandomFloat(min, max);
}
LinearExtrapolations linear_extraps;

template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

float NormalizeAngle(float angle)
{
	if (angle > 180.f || angle < -180.f)
	{
		auto revolutions = round(abs(angle / 360.f));
		if (angle < 0.f)
			angle = angle + 360.f * revolutions;
		else
			angle = angle - 360.f * revolutions;
	}
	return angle;
}

void setupvelocity_gfy(IClientEntity * plocal)
{
	if (!plocal || !plocal->IsAlive())
		return;
	auto state = plocal->get_animation_state();

	if (!state)
		return;

	auto eye_y = state->m_flEyeYaw;
	auto gfy = state->goal_feet_yaw;
	gfy = clamp(gfy, -360.0f, 360.0f);

	gfy = clamp(gfy, eye_y + c_antiaim->maxdelta(plocal), eye_y - c_antiaim->maxdelta(plocal));

	NormalizeAngle(gfy);

	if (state->speed > 0.1f || plocal->get_abs_velocity().z > 100.0f) {

		state->goal_feet_yaw = eye_y;
	}
	else {

		state->goal_feet_yaw = plocal->get_lowerbody();
	}
}
struct CIncomingSequence
{
	CIncomingSequence::CIncomingSequence(int instate, int outstate, int seqnr, float time)
	{
		inreliablestate = instate;
		outreliablestate = outstate;
		sequencenr = seqnr;
		curtime = time;
	}
	int inreliablestate;
	int outreliablestate;
	int sequencenr;
	float curtime;
};
std::deque<CIncomingSequence> sequences;
int32_t lastincomingsequencenumber;

/*
static void drawThiccLine(int x1, int y1, int x2, int y2, int type, Color color) {
	if (type > 1) {
		Render::Line(x1, y1 - 1, x2, y2 - 1, color);
		Render::Line(x1, y1, x2, y2, color);
		Render::Line(x1, y1 + 1, x2, y2 + 1, color);
		Render::Line(x1, y1 - 2, x2, y2 - 2, color);
		Render::Line(x1, y1 + 2, x2, y2 + 2, color);
	}
	else {
		Render::Line(x1 - 1, y1, x2 - 1, y2, color);
		Render::Line(x1, y1, x2, y2, color);
		Render::Line(x1 + 1, y1, x2 + 1, y2, color);
		Render::Line(x1 - 2, y1, x2 - 2, y2, color);
		Render::Line(x1 + 2, y1, x2 + 2, y2, color);
	}
}
*/
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y | %d | %X", &tstruct);
	return buf;
}
Color urmamasuckmylargegenetalia(int speed, int offset)
{
	float hue = (float)((GetCurrentTime() + offset) % speed);
	hue /= speed;
	return Color::FromHSB(hue, 1.0F, 1.0F);
}

static DWORD* deathNotice;
static void(__thiscall *ClearDeathNotices)(DWORD);


void things(IGameEvent* pEvent)
{
	if (!strcmp(pEvent->GetName(), "round_prestart") && interfaces::engine->IsInGame() && interfaces::engine->IsConnected() && hackManager.plocal())
	{
		deathNotice = nullptr;
	
		c_misc->anotherpcheck = true;
	
	}
}
template<class T>
static T* FindHudElementX(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(game_utils::FindPattern1(("client.dll"), ("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(game_utils::FindPattern1(("client.dll"), ("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
	return (T*)find_hud_element(pThis, name);
}


bool warmup = false;

void angle_correction::ConColorMsg(Color const &color, const char* buf, ...)
{
	using ConColFn = void(__stdcall*)(Color const &, const char*, ...);
	auto ConCol = reinterpret_cast<ConColFn>((GetProcAddress(GetModuleHandle("tier0.dll"), "?ConColorMsg@@YAXABVColor@@PBDZZ")));
	ConCol(color, buf);
}

struct bullet_impact_log
{
	bullet_impact_log(int userid, vector fire_pos, vector impact_pos, float impact_time) {
		this->uid = userid;
		this->fire_posit = fire_pos;
		this->impact_posit = impact_pos;
		this->impac_time = impact_time;
	}

	int uid;
	vector fire_posit;
	vector impact_posit;
	float impac_time;
};

std::vector< bullet_impact_log > bullet_logs;

void DrawBeam(vector src, vector end, Color Color) {
	int r, g, b, a;
	Color.GetColor(r, g, b, a);
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;

	beamInfo.m_pszModelName = "sprites/blueglow1.vmt";

	beamInfo.m_nModelIndex = -1; // will be set by CreateBeamPoints if its -1
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = 3.0;
	beamInfo.m_flWidth = 1.f;
	beamInfo.m_flEndWidth = 1.f;
	beamInfo.m_flFadeLength = 0.0f;
	beamInfo.m_flAmplitude = 1.f;
	beamInfo.m_flBrightness = a;
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = r;
	beamInfo.m_flGreen = g;
	beamInfo.m_flBlue = b;
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = 0;

	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;

	Beam_t* myBeam = interfaces::render_beams->CreateBeamPoints(beamInfo);

	if (myBeam)
		interfaces::render_beams->DrawBeam(myBeam);
}

class CBaseAnimating
{
public:
	std::array<float, 24>* m_flPoseParameter()
	{
		static int offset = 0;
		if (!offset)
			offset = 0x2764;
		return (std::array<float, 24>*)((uintptr_t)this + offset);
	}
	model_t* GetModel()
	{
		void* pRenderable = reinterpret_cast<void*>(uintptr_t(this) + 0x4);
		typedef model_t* (__thiscall* fnGetModel)(void*);
		return call_vfunc<fnGetModel>(pRenderable, 8)(pRenderable);
	}

	void SetBoneMatrix(matrix3x4_t* boneMatrix)
	{
		//Offset found in C_BaseAnimating::GetBoneTransform, string search ankle_L and a function below is the right one
		const auto model = this->GetModel();
		if (!model)
			return;
		matrix3x4_t* matrix = *(matrix3x4_t**)((DWORD)this + 9880);
		studiohdr_t *hdr = interfaces::model_info->GetStudiomodel(model);
		if (!hdr)
			return;
		int size = hdr->numbones;
		if (matrix) {
			for (int i = 0; i < size; i++)
				memcpy(matrix + i, boneMatrix + i, sizeof(matrix3x4_t));
		}
	}
	void GetDirectBoneMatrix(matrix3x4_t* boneMatrix)
	{
		const auto model = this->GetModel();
		if (!model)
			return;
		matrix3x4_t* matrix = *(matrix3x4_t**)((DWORD)this + 9880);
		studiohdr_t *hdr = interfaces::model_info->GetStudiomodel(model);
		if (!hdr)
			return;
		int size = hdr->numbones;
		if (matrix) {
			for (int i = 0; i < size; i++)
				memcpy(boneMatrix + i, matrix + i, sizeof(matrix3x4_t));
		}
	}
};

void UpdateIncomingSequences()
{
	auto clientState = interfaces::client_state; //DONT HARDCODE OFFESTS

	if (!clientState)
		return;

	auto intnetchan = clientState->m_NetChannel; //Can optimise, already done in CM hook, make a global

	INetChannel* netchan = reinterpret_cast<INetChannel*>(intnetchan);
	if (netchan)
	{
		if (netchan->m_nInSequenceNr > lastincomingsequencenumber)
		{
			//sequences.push_front(netchan->m_nInSequenceNr);
			lastincomingsequencenumber = netchan->m_nInSequenceNr;

			sequences.push_front(CIncomingSequence(netchan->m_nInReliableState, netchan->m_nOutReliableState, netchan->m_nInSequenceNr, interfaces::globals->realtime));
		}
		if (sequences.size() > 2048)
			sequences.pop_back();
	}
}


