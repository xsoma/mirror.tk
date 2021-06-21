#include "includes.h"
#include "Hooks.h"

#include "Global.h"
#include "Thirdperson.h"

auto smoke_count = *(DWORD*)(Utilities::Memory::FindPatternV2("client", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0") + 0x8);

std::vector<const char*> vistasmoke_mats =
{
	"particle/beam_smoke_01",
	"particle/particle_smokegrenade",
	"particle/particle_smokegrenade1",
	"particle/particle_smokegrenade2",
	"particle/particle_smokegrenade3",
	"particle/particle_smokegrenade_sc",
	"particle/smoke1/smoke1",
	"particle/smoke1/smoke1_ash",
	"particle/smoke1/smoke1_nearcull",
	"particle/smoke1/smoke1_nearcull2",
	"particle/smoke1/smoke1_snow",
	"particle/smokesprites_0001",
	"particle/smokestack",
	"particle/vistasmokev1/vistasmokev1",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_nearcull",
	"particle/vistasmokev1/vistasmokev1_nearcull_fog",
	"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev4_emods_nocull",
	"particle/vistasmokev1/vistasmokev4_nearcull",
	"particle/vistasmokev1/vistasmokev4_nocull"
};

void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	interfaces::cvar->FindVar("cl_extrapolate")->SetValue("0");

	if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame())
	{
		global::local = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer()); // cause life

		for (int i = 0; i < interfaces::globals->max_clients; i++)
		{
			if (!options::menu.aimbot.enable.getstate())
				continue;

			auto m_entity = interfaces::ent_list->get_client_entity(i);

			if (!m_entity)
				continue;

			if (m_entity->team() == global::local->team())
				continue;

			if (options::menu.visuals.OtherNoFlash.getstate())
			{
				if (global::local)
					global::local->m_flFlashDuration() = options::menu.visuals.flashAlpha.GetValue() / 100;;
			}

			if (curStage == FRAME_NET_UPDATE_END) { // Freak just no
				static auto sv_unlag = interfaces::cvar->FindVar("sv_unlag");
				if (!sv_unlag)
					C_LagCompensation::get().player_lag_records[i].records->clear();
					C_LagCompensation::get().frame_net_update_end();
			}

		}


		if (interfaces::pinput->m_fCameraInThirdPerson)
		{
		//	*reinterpret_cast<vector*>(reinterpret_cast<DWORD>(global::local) + 0x31D8) = options::menu.misc.fakeangle_compaitibility.getstate() ? LastAngleAAReal : vector(LastAngleAAFake.x, c_beam->visual_angle, 0);
		//	global::local->SetAbsAngles(vector(0, c_beam->visual_angle, 0));

			//	Interfaces::Prediction1->set_local_viewangles_rebuilt(LastAngleAAReal);
		//	vector viewangs = *(vector*)(reinterpret_cast<uintptr_t>(global::local) + 0x31D8);
		//	viewangs = options::menu.misc.fakeangle_compaitibility.getstate() ? LastAngleAAReal : vector(LastAngleAAFake.x, c_beam->visual_angle, 0); // c_beam->visual_angle

			global::local->get_visual_angles() = thirdperson::get().get_tp_angle();

			interfaces::c_prediction->set_local_viewangles_rebuilt(global::real_angles);
		}

		if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
			if (global::local && global::local->IsAlive())
			{
				//	skinchanger._do(local_player);
				static  constexpr const char* models[]{
					"models/player/custom_player/legacy/ctm_fbi_variantb.mdl",
					"models/player/custom_player/legacy/ctm_fbi_variantf.mdl",
					"models/player/custom_player/legacy/ctm_fbi_variantg.mdl",
					"models/player/custom_player/legacy/ctm_fbi_varianth.mdl",
					"models/player/custom_player/legacy/ctm_sas_variantf.mdl",
					"models/player/custom_player/legacy/ctm_st6_variante.mdl",
					"models/player/custom_player/legacy/ctm_st6_variantg.mdl",
					"models/player/custom_player/legacy/ctm_st6_varianti.mdl",
					"models/player/custom_player/legacy/ctm_st6_variantk.mdl",
					"models/player/custom_player/legacy/ctm_st6_variantm.mdl",
					"models/player/custom_player/legacy/tm_balkan_variantf.mdl",
					"models/player/custom_player/legacy/tm_balkan_variantg.mdl",
					"models/player/custom_player/legacy/tm_balkan_varianth.mdl",
					"models/player/custom_player/legacy/tm_balkan_varianti.mdl",
					"models/player/custom_player/legacy/tm_balkan_variantj.mdl",
					"models/player/custom_player/legacy/tm_leet_variantf.mdl",
					"models/player/custom_player/legacy/tm_leet_variantg.mdl",
					"models/player/custom_player/legacy/tm_leet_varianth.mdl",
					"models/player/custom_player/legacy/tm_leet_varianti.mdl",
					"models/player/custom_player/legacy/tm_phoenix_variantf.mdl",
					"models/player/custom_player/legacy/tm_phoenix_variantg.mdl",
					"models/player/custom_player/legacy/tm_phoenix_varianth.mdl"
				};
				if (options::menu.misc.model.getindex() > 0) {
					global::local->setmodel(interfaces::model_info->GetModelIndex(models[options::menu.misc.model.getindex() - 1]));
				} IClientEntity* pEntity{};

				C_LagCompensation::get().start_position_adjustment();
				resolver_setup::GetInst().FSN(pEntity, curStage);
			}
		}

		if (curStage == FRAME_RENDER_START)
		{
	//		animfix->fix_local_player_animations(overall::userCMD);

			bool yes = false;
			if (options::menu.visuals.nosmoke.getstate())
			{
				for (auto matName : vistasmoke_mats)
				{
					IMaterial* mat = interfaces::materialsystem->FindMaterial(matName, "Other textures");
					if (!mat || mat->IsErrorMaterial())
						continue;
					mat->IncrementReferenceCount();
					mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

					*(int*)smoke_count = 0;
					yes - true;
				}
			}

			if (yes && !options::menu.visuals.nosmoke.getstate())
			{
				for (auto matName : vistasmoke_mats)
				{
					IMaterial* mat = interfaces::materialsystem->FindMaterial(matName, "Other textures");

					mat->IncrementReferenceCount();
					mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

					yes = false;
				}
			}
		}

		c_misc->colour_modulation();
	}

	oFrameStageNotify(curStage);
}