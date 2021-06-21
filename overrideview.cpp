#include "includes.h"
#include "Hooks.h"

#include "singleton.h"
#include "Thirdperson.h"

void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup)
{
	auto local = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	if (!local || !interfaces::engine->IsConnected() || !interfaces::engine->IsInGame())
		return;

	auto zoom = options::menu.visuals.OtherFOV.GetValue();

	if (local->IsScoped() && options::menu.visuals.RemoveZoom.getstate()) {
		zoom += 90.0f - pSetup->fov;
	}
	pSetup->fov += zoom;

	thirdperson::get().run();

	if (GetAsyncKeyState(options::menu.misc.fake_crouch_key.GetKey()) && options::menu.misc.fake_crouch_view.getstate())
	{
		pSetup->origin.z = hackManager.plocal()->GetAbsOrigin().z + 64.f;
	}

	oOverrideView(ecx, edx, pSetup);
}