#include "includes.h"
#include "Hooks.h"

void GetViewModelFOV(float& fov)
{
	IClientEntity* localplayer = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame())
	{
		if (!localplayer)
			return;
		if (options::menu.visuals.Active.getstate())
			fov = options::menu.visuals.OtherViewmodelFOV.GetValue();
	}
}

float __stdcall Hooked_GetViewModelFOV()
{
	float fov = Hooks::VMTClientMode.GetMethod<oGetViewModelFOV>(35)();
	GetViewModelFOV(fov);
	return fov;
}