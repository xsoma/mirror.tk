// fixed and updated for 9/8 update by I am a dook.
// change all client_panorama variables to updated state
// updated globalvars; clientobs; and getanimstate
// also fixed crashing on menu/game load











#include "Interfaces.h"
#include "glow_outline_effect.h"
#include "Utilities.h"
#include <d3d9.h>
typedef void* (__cdecl* CreateInterface_t)(const char*, int*);
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
CreateInterface_t EngineFactory = NULL;
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUISurfaceFactory = NULL;
CreateInterface_t VGUI2Factory = NULL;
CreateInterface_t MatFactory = NULL;
CreateInterface_t InputSys = NULL;
CreateInterface_t PhysFactory = NULL;
CreateInterface_t StdFactory = NULL;

#define enc_str(s) std::string(s)
#define enc_char(s) enc_str(s).c_str()
void* FindInterface(const char* Module, const char* InterfaceName)
{
	void* Interface = nullptr;
	auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(
		GetModuleHandleA(Module), enc_char("CreateInterface")));

	char PossibleInterfaceName[1024];
	for (int i = 1; i < 100; i++)
	{
		sprintf(PossibleInterfaceName, "%s0%i", InterfaceName, i);
		Interface = CreateInterface(PossibleInterfaceName, 0);
		if (Interface)
			break;

		sprintf(PossibleInterfaceName, "%s00%i", InterfaceName, i);
		Interface = CreateInterface(PossibleInterfaceName, 0);
		if (Interface)
			break;
	}

	//		if (!Interface)
	//			std::cout << (enc_str("Failed to find interface - ") + PossibleInterfaceName) << std::endl;
	//		else
	//			std::cout <<  (enc_str("Successfully found interface - ") + PossibleInterfaceName) << std::endl;

	return Interface;
}

static auto FindPatternIDA = [](HMODULE hModule, const char* szSignature) -> uint8_t* {
	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto dosHeader = (PIMAGE_DOS_HEADER)hModule;
	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)hModule + dosHeader->e_lfanew);

	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = pattern_to_byte(szSignature);
	auto scanBytes = reinterpret_cast<std::uint8_t*>(hModule);

	auto s = patternBytes.size();
	auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
		bool found = true;
		for (auto j = 0ul; j < s; ++j) {
			if (scanBytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found) {

			return &scanBytes[i];
		}
	}

	return nullptr;
};


void interfaces::Initialise()
{
	EngineFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Engine, "CreateInterface");
	ClientFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Client, "CreateInterface");
	VGUI2Factory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VGUI2, "CreateInterface");
	VGUISurfaceFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VGUISurface, "CreateInterface");
	MatFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Material, "CreateInterface");
	PhysFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VPhysics, "CreateInterface");
	InputSys = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::InputSys, "CreateInterface");
	StdFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Stdlib, "CreateInterface");
	char* CHLClientInterfaceName = (char*)Utilities::Memory::FindTextPattern("client.dll", "VClient0");
	char* VGUI2PanelsInterfaceName = (char*)Utilities::Memory::FindTextPattern("vgui2.dll", "VGUI_Panel0");
	char* VGUISurfaceInterfaceName = (char*)Utilities::Memory::FindTextPattern("vguimatsurface.dll", "VGUI_Surface0");
	char* EntityListInterfaceName = (char*)Utilities::Memory::FindTextPattern("client.dll", "VClientEntityList0");
	char* EngineDebugThingInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VDebugOverlay0");
	char* EngineClientInterfaceName = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineClient0");
	char* ClientPredictionInterface = (char*)Utilities::Memory::FindTextPattern("client.dll", "VClientPrediction0");
	char* MatSystemInterfaceName = (char*)Utilities::Memory::FindTextPattern("materialsystem.dll", "VMaterialSystem0");
	char* EngineRenderViewInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineRenderView0");
	char* EngineModelRenderInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineModel0");
	char* EngineModelInfoInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VModelInfoClient0");
	char* EngineTraceInterfaceName = (char*)Utilities::Memory::FindTextPattern("engine.dll", "EngineTraceClient0");
	char* PhysPropsInterfaces = (char*)Utilities::Memory::FindTextPattern("client.dll", "VPhysicsSurfaceProps0");
	char* VEngineCvarName = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineCvar00");
	char* effectthing = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineEffects001");


	client = (IBaseClientDLL*)ClientFactory(CHLClientInterfaceName, NULL);
	engine = (IVEngineClient*)EngineFactory(EngineClientInterfaceName, NULL);
	panels = (IPanel*)VGUI2Factory(VGUI2PanelsInterfaceName, NULL);
	window = FindWindow("Valve001", NULL);
	surface = (ISurface*)VGUISurfaceFactory(VGUISurfaceInterfaceName, NULL);
	ent_list = (IClientEntityList*)ClientFactory(EntityListInterfaceName, NULL);
	DebugOverlay = (IVDebugOverlay*)EngineFactory(EngineDebugThingInterface, NULL);
	prediction_dword = (DWORD*)ClientFactory(ClientPredictionInterface, NULL);
	materialsystem = (CMaterialSystem*)MatFactory(MatSystemInterfaceName, NULL);
	render_view = (CVRenderView*)EngineFactory(EngineRenderViewInterface, NULL);
	model_render = (IVModelRender*)EngineFactory(EngineModelRenderInterface, NULL);
	model_info = (CModelInfo*)EngineFactory(EngineModelInfoInterface, NULL);
	trace = (IEngineTrace*)EngineFactory(EngineTraceInterfaceName, NULL);
	trace_2 = (Trace2*)EngineFactory(EngineTraceInterfaceName, NULL);
	m_iInputSys = (IInputSystem*)InputSys("InputSystemVersion001", NULL);
	phys_props = (IPhysicsSurfaceProps*)PhysFactory(PhysPropsInterfaces, NULL);
	cvar = (ICVar*)StdFactory(VEngineCvarName, NULL);
	event_manager = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", NULL);
	g_pD3DDevice9 = **(IDirect3DDevice9***)(Utilities::Memory::FindPatternV2("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
	effects = (IVEffects*)EngineFactory(effectthing, NULL);
	ClientMode = **(IClientModeShared***)((*(uintptr_t**)client)[10] + 0x5);
	globals = **reinterpret_cast< CGlobalVarsBase*** >((*reinterpret_cast< DWORD** >(client))[0] + 0x1F);
	render_beams = *(IViewRenderBeams**)(Utilities::Memory::FindPatternV2("client.dll", "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9") + 1);
	movehelper = **(IMoveHelper***)(game_utils::pattern_scan(GetModuleHandleA("client.dll"), "8B 0D ? ? ? ? 8B 46 08 68") + 2); //test
	PDWORD pdwClientVMT = *(PDWORD*)client;
	glow_manager = *(CGlowObjectManager**)(Utilities::Memory::FindPatternV2("client.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 3);

	client_state = **reinterpret_cast<CClientState***>(v_func<std::uintptr_t>(engine, 12) + 16);
	
	g_ClientSideAnimationList = (CUtlVectorSimple*)*(DWORD*)(Utilities::Memory::FindPatternV2("client.dll", "A1 ? ? ? ? F6 44 F0 04 01 74 0B") + 1);
	pinput = *(CInput**)(Utilities::Memory::FindPatternV2("client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 1);
	mem_alloc = *(IMemAlloc**)(GetProcAddress(GetModuleHandle("tier0.dll"), "g_pMemAlloc"));
	c_gamemove = reinterpret_cast<CGameMovement*>(Utilities::Memory::capture_interface("client.dll", "GameMovement001"));
	c_prediction = reinterpret_cast<IPrediction*>(Utilities::Memory::capture_interface("client.dll", "VClientPrediction001"));
}
namespace interfaces
{
	IBaseClientDLL* client;
	IVEngineClient* engine;
	IPanel* panels;
	IClientEntityList* ent_list;
	ISurface* surface;
	IVDebugOverlay* DebugOverlay;
	IClientModeShared *ClientMode;
	CGlobalVarsBase *globals;
	DWORD *prediction_dword;
	CGlowObjectManager* glow_manager;
	CMaterialSystem* materialsystem;
	CVRenderView* render_view;
	IVModelRender* model_render;
	IInputSystem* m_iInputSys;
	CModelInfo* model_info;
	IEngineTrace* trace;
	IPhysicsSurfaceProps* phys_props;
	CClientState* client_state;
	IVEffects* effects;
	Trace2 *trace_2;
	HWND window;
	ICVar *cvar;
	CInput* pinput;
	CUtlVectorSimple *g_ClientSideAnimationList;
	IDirect3DDevice9        *g_pD3DDevice9;
	IGameEventManager2 *event_manager;
	HANDLE __FNTHANDLE;
	IMoveHelper* movehelper;
	IPrediction *c_prediction;
	IGameMovement* gamemovement;
	IViewRenderBeams* render_beams;
	CGameMovement * c_gamemove;
	IMemAlloc* mem_alloc;
};
