#define _CRT_SECURE_NO_WARNINGS

#include "Hacks.h"
#include "Interfaces.h"
#include "RenderManager.h"

#include "ESP.h"
#include "Visuals.h"
#include "RageBot.h"
#include "MiscHacks.h"

CEsp Esp;
CVisuals Visuals;
CMiscHacks MiscHacks;
CAimbot RageBot;

void Hacks::SetupHacks()
{
	Esp.Init();
	Visuals.Init();
	MiscHacks.Init();
	RageBot.Init();

	hackManager.RegisterHack(&Esp);
	hackManager.RegisterHack(&Visuals);
	hackManager.RegisterHack(&MiscHacks);
	hackManager.RegisterHack(&RageBot);

	hackManager.Ready();
}


void SpecList()
{


}


void Hacks::DrawHacks()
{
	if (!options::menu.visuals.Active.getstate())
		return;

	hackManager.Draw();		
}

void Hacks::MoveHacks(CUserCmd * pCmd, bool & bSendPacket)
{
	vector origView = pCmd->viewangles;
	IClientEntity *pLocal = hackManager.plocal();
	hackManager.Move(pCmd, bSendPacket);

}

HackManager hackManager;

void HackManager::RegisterHack(CHack* hake)
{
	Hacks.push_back(hake);
	hake->Init();
}

void HackManager::Draw()
{
	if (!IsReady)
		return;

	pLocalInstance = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	if (!pLocalInstance) return;

	for (auto &hack : Hacks)
	{
		hack->Draw();
	}
}

void HackManager::Move(CUserCmd *pCmd,bool &bSendPacket)
{
	if (!IsReady)
		return;

	pLocalInstance = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	if (!pLocalInstance) return;

	for (auto &hack : Hacks)
	{
		hack->Move(pCmd,bSendPacket); 
	}
}

IClientEntity* HackManager::plocal()
{
	return pLocalInstance;
}

void HackManager::Ready()
{
	IsReady = true;
}