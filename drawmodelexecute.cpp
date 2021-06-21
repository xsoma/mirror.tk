#include "includes.h"

void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld)
{
	if (!interfaces::engine->IsConnected() || !interfaces::engine->IsInGame())
		return;

	Color color;
	float flColor[3] = { 0.f };
	bool DontDraw = false;
	static IMaterial* mat = interfaces::materialsystem->FindMaterial("FlatChams", "Model textures", NULL);

	int HandsStyle = options::menu.visuals.HandCHAMS.getindex();
	int gunstyle = options::menu.visuals.GunCHAMS.getindex();

	const char* ModelName = interfaces::model_info->GetModelName((model_t*)pInfo.pModel);


	IClientEntity* pentity = (IClientEntity*)interfaces::ent_list->get_client_entity(pInfo.entity_index);
	IClientEntity* plocal = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	float blend_vis = options::menu.visuals.enemy_blend.GetValue() / 100;
	float blend_invis = options::menu.visuals.enemy_blend_invis.GetValue() / 100;

	float fl_color_x[3] = { 0.f };
	float fl_color1[4] = { 0.f };

	int v = options::menu.visuals.visible_chams_type.getindex();
	int iv = options::menu.visuals.invisible_chams_type.getindex();

	float blend = options::menu.visuals.transparency.GetValue() / 100;
	float hand_blend = options::menu.visuals.hand_transparency.GetValue() / 100;
	float gun_blend = options::menu.visuals.gun_transparency.GetValue() / 100;
	float sleeve_blend = options::menu.visuals.sleeve_transparency.GetValue() / 100;
	float blend_local = options::menu.visuals.blend_local.GetValue() / 100;


	static IMaterial* wire = CreateMaterial(true, false, true);
	static IMaterial* glass = interfaces::materialsystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER, true);
	static IMaterial* crystal = interfaces::materialsystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER, true);
	static IMaterial* usual = interfaces::materialsystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL, true);
	static IMaterial* gold = interfaces::materialsystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER);
	static IMaterial* dogtag = interfaces::materialsystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_OTHER, true);
	static IMaterial* devglow = interfaces::materialsystem->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_OTHER, true);
	static IMaterial* fbi = interfaces::materialsystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_OTHER, true);
	static IMaterial* tags = interfaces::materialsystem->FindMaterial("models/inventory_items/dogtags/dogtags_lightray", "Model textures");
	static IMaterial* fish = interfaces::materialsystem->FindMaterial("models/props_shacks/fishing_net01", TEXTURE_GROUP_OTHER, true);
	static IMaterial* hydra = interfaces::materialsystem->FindMaterial("models/inventory_items/hydra_crtysal/hydra_crystal_detail", TEXTURE_GROUP_OTHER, true); //hhhhhhhhhydra

	int ChamsStyle = options::menu.visuals.OptionsChams.getindex();
	int sleeves = options::menu.visuals.SleeveChams.getindex();
	//	IMaterial *covered = CoveredFlat;

	float fl_color[3] = { 0.f };

	int l = options::menu.visuals.localmaterial.getindex();

	IMaterial* visible; IMaterial* invisible; IMaterial* scopedlocal;
	IMaterial* overlay = interfaces::materialsystem->FindMaterial("Metallic", "Model textures", NULL);

	float visible_blend = options::menu.visuals.visible_transparency.GetValue() / 100;
	float invisible_blend = options::menu.visuals.invisible_transparency.GetValue() / 100;
	switch (l)
	{
	case 2: scopedlocal = interfaces::materialsystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL, true);
		break;
	case 3: scopedlocal = wire;
		break;
	}

	switch (v)
	{
	case 0: visible = interfaces::materialsystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL, true);
		break;
	case 1: visible = interfaces::materialsystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL, true);
		break;
	case 2: visible = interfaces::materialsystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_OTHER, true);
		break;
	case 3: visible = interfaces::materialsystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER, true);
		break;
	case 4:	visible = interfaces::materialsystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER, true);
		break;
	case 5: visible = interfaces::materialsystem->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_OTHER, true);
		break;
	case 6: visible = interfaces::materialsystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_OTHER, true);
		break;
	case 7: visible = interfaces::materialsystem->FindMaterial("models/props_shacks/fishing_net01", TEXTURE_GROUP_OTHER, true);
		break;
	case 8: visible = interfaces::materialsystem->FindMaterial("Metallic", "Model textures", NULL);
		break;
	case 9: visible = interfaces::materialsystem->FindMaterial("FlatChams", "Model textures", NULL);
		break;

	}

	switch (iv)
	{
	case 0: invisible = interfaces::materialsystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL, true);
		break;
	case 1: invisible = interfaces::materialsystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL, true);
		break;
	case 2: invisible = interfaces::materialsystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_OTHER, true);
		break;
	case 3: invisible = interfaces::materialsystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER, true);
		break;
	case 4:	invisible = interfaces::materialsystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER, true);
		break;
	case 5: invisible = interfaces::materialsystem->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_OTHER, true);
		break;
	case 6: invisible = interfaces::materialsystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_OTHER, true);
		break;
	case 7: invisible = interfaces::materialsystem->FindMaterial("models/props_shacks/fishing_net01", TEXTURE_GROUP_OTHER, true);
		break;
	case 8: invisible = interfaces::materialsystem->FindMaterial("Metallic", "Model textures", NULL);
		break;
	case 9: invisible = interfaces::materialsystem->FindMaterial("FlatChams", "Model textures", NULL);
		break;
		//	case 8: invisible = shiny();
		//		break;

	}

	bool do_team = options::menu.visuals.ChamsTeamVis.getindex() > 0;
	bool do_local = options::menu.visuals.ChamsLocal.getstate();
	if (options::menu.visuals.fakelag_ghost.getstate())
	{
		plocal->set_duck_amount(c_beam->duck);
		plocal->set_abs_origin(c_beam->cham_origin);
	}

	if (pentity->isValidPlayer() && strstr(ModelName, "models/player"))
	{
		if (pentity->team() != plocal->team() && !pentity->IsDormant() && plocal)
		{
			switch (options::menu.visuals.ChamsEnemy.getindex())
			{
			case 1:
			{
				if (!visible || visible->IsErrorMaterial())
					return;

				visible->IncrementReferenceCount();

				fl_color_x[0] = options::menu.ColorsTab.ChamsEnemyVis.GetValue()[0] / 255.f;
				fl_color_x[1] = options::menu.ColorsTab.ChamsEnemyVis.GetValue()[1] / 255.f;
				fl_color_x[2] = options::menu.ColorsTab.ChamsEnemyVis.GetValue()[2] / 255.f;
				fl_color_x[3] = visible_blend;
				interfaces::render_view->SetColorModulation(fl_color_x);
				interfaces::model_render->ForcedMaterialOverride(visible);
			}
			break;

			case 2:
			{
				if (!invisible || invisible->IsErrorMaterial())
					return;

				invisible->IncrementReferenceCount();
				fl_color_x[0] = options::menu.ColorsTab.ChamsEnemyNotVis.GetValue()[0] / 255.f;
				fl_color_x[1] = options::menu.ColorsTab.ChamsEnemyNotVis.GetValue()[1] / 255.f;
				fl_color_x[2] = options::menu.ColorsTab.ChamsEnemyNotVis.GetValue()[2] / 255.f;
				plocal->IsAlive() ? interfaces::render_view->SetColorModulation(fl_color_x) : invisible->ColorModulate(fl_color_x[0], fl_color_x[1], fl_color_x[2]);
				invisible->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				interfaces::render_view->SetBlend(invisible_blend);
				interfaces::model_render->ForcedMaterialOverride(invisible);

				oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

				if (!visible || visible->IsErrorMaterial())
					return;

				visible->IncrementReferenceCount();
				visible->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				fl_color1[0] = options::menu.ColorsTab.ChamsEnemyVis.GetValue()[0] / 255.f;
				fl_color1[1] = options::menu.ColorsTab.ChamsEnemyVis.GetValue()[1] / 255.f;
				fl_color1[2] = options::menu.ColorsTab.ChamsEnemyVis.GetValue()[2] / 255.f;
				interfaces::render_view->SetColorModulation(fl_color1);

				interfaces::render_view->SetBlend(visible_blend);
				interfaces::model_render->ForcedMaterialOverride(visible);
			}
			break;
			}
		}


		if (do_team && pentity->team() == plocal->team() && pentity != plocal && !do_local)
		{

			switch (options::menu.visuals.ChamsTeamVis.getindex())
			{
			case 1:
			{
				if (!visible || visible->IsErrorMaterial())
					return;

				visible->IncrementReferenceCount();
				fl_color_x[0] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[0] / 255.f;
				fl_color_x[1] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[1] / 255.f;
				fl_color_x[2] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[2] / 255.f;
				interfaces::render_view->SetColorModulation(fl_color_x);
				interfaces::render_view->SetBlend(visible_blend);
				interfaces::model_render->ForcedMaterialOverride(visible);
			}
			break;

			case 2:
			{
				if (!invisible || invisible->IsErrorMaterial())
					return;

				invisible->IncrementReferenceCount();
				fl_color_x[0] = options::menu.ColorsTab.ChamsTeamNotVis.GetValue()[0] / 255.f;
				fl_color_x[1] = options::menu.ColorsTab.ChamsTeamNotVis.GetValue()[1] / 255.f;
				fl_color_x[2] = options::menu.ColorsTab.ChamsTeamNotVis.GetValue()[2] / 255.f;
				interfaces::render_view->SetColorModulation(fl_color_x);
				interfaces::render_view->SetBlend(invisible_blend);
				invisible->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				interfaces::model_render->ForcedMaterialOverride(invisible);

				oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

				if (!visible || visible->IsErrorMaterial())
					return;

				visible->IncrementReferenceCount();
				fl_color1[0] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[0] / 255.f;
				fl_color1[1] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[1] / 255.f;
				fl_color1[2] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[2] / 255.f;
				interfaces::render_view->SetColorModulation(fl_color1);
				interfaces::render_view->SetBlend(visible_blend);
				visible->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				interfaces::model_render->ForcedMaterialOverride(visible);
			}
			break;
			}


		}

		if (pentity == plocal) {

		}

		if (do_local && pentity == plocal && !plocal->IsScoped())
		{
			switch (options::menu.visuals.ChamsTeamVis.getindex())
			{
			case 1:
			{
				if (!visible || visible->IsErrorMaterial())
					return;

				visible->IncrementReferenceCount();
				fl_color_x[0] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[0] / 255.f;
				fl_color_x[1] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[1] / 255.f;
				fl_color_x[2] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[2] / 255.f;
				interfaces::render_view->SetColorModulation(fl_color_x);
				interfaces::render_view->SetBlend(visible_blend);
				interfaces::model_render->ForcedMaterialOverride(visible);
			}
			break;

			case 2:
			{
				if (!invisible || invisible->IsErrorMaterial())
					return;

				invisible->IncrementReferenceCount();
				fl_color_x[0] = options::menu.ColorsTab.ChamsTeamNotVis.GetValue()[0] / 255.f;
				fl_color_x[1] = options::menu.ColorsTab.ChamsTeamNotVis.GetValue()[1] / 255.f;
				fl_color_x[2] = options::menu.ColorsTab.ChamsTeamNotVis.GetValue()[2] / 255.f;
				interfaces::render_view->SetColorModulation(fl_color_x);
				interfaces::render_view->SetBlend(invisible_blend);
				invisible->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				interfaces::model_render->ForcedMaterialOverride(invisible);

				oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

				if (!visible || visible->IsErrorMaterial())
					return;

				visible->IncrementReferenceCount();
				fl_color1[0] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[0] / 255.f;
				fl_color1[1] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[1] / 255.f;
				fl_color1[2] = options::menu.ColorsTab.ChamsTeamVis.GetValue()[2] / 255.f;
				interfaces::render_view->SetColorModulation(fl_color1);
				interfaces::render_view->SetBlend(visible_blend);
				visible->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				interfaces::model_render->ForcedMaterialOverride(visible);
			}
			break;
			}
		}

		if (plocal->IsScoped() && pentity == plocal)
		{
			switch (l)
			{

			case 1:
			{
				interfaces::render_view->SetBlend(blend);
			}
			break;

			case 2:
			{

				flColor[0] = options::menu.ColorsTab.scoped_c.GetValue()[0] / 255.f;
				flColor[1] = options::menu.ColorsTab.scoped_c.GetValue()[1] / 255.f;
				flColor[2] = options::menu.ColorsTab.scoped_c.GetValue()[2] / 255.f;

				interfaces::render_view->SetBlend(blend);
				interfaces::render_view->SetColorModulation(flColor);
				interfaces::model_render->ForcedMaterialOverride(scopedlocal);
				oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

			}
			break;

			case 3:
			{

				flColor[0] = options::menu.ColorsTab.scoped_c.GetValue()[0] / 255.f;
				flColor[1] = options::menu.ColorsTab.scoped_c.GetValue()[1] / 255.f;
				flColor[2] = options::menu.ColorsTab.scoped_c.GetValue()[2] / 255.f;
				interfaces::render_view->SetColorModulation(flColor);
				interfaces::render_view->SetBlend(blend);
				interfaces::model_render->ForcedMaterialOverride(scopedlocal);
				oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

			}
			break;

			}
		}
	}


	if (strstr(ModelName, "models/weapons/v_") && !strstr(ModelName, "arms"))
	{

		switch (gunstyle) // this shit was done in "else if"s with " == " instead of A > B or switch statements which is fucking HECKIN trash for optimization
		{
		case 1:
		{
			if (!usual || usual->IsErrorMaterial())
				return;
			usual->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			flColor[0] = options::menu.ColorsTab.GunChamsCol.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.GunChamsCol.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.GunChamsCol.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(gun_blend);
			interfaces::model_render->ForcedMaterialOverride(usual);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 2:
		{
			//	static IMaterial* mat_T = interfaces::materialsystem->FindMaterial("metal", TEXTURE_GROUP_MODEL);

			if (!mat || mat->IsErrorMaterial())
				return;
			float col[3] = { 0.f, 0.f, 0.f };
			col[0] = options::menu.ColorsTab.GunChamsCol.GetValue()[0] / 255.f;
			col[1] = options::menu.ColorsTab.GunChamsCol.GetValue()[1] / 255.f;
			col[2] = options::menu.ColorsTab.GunChamsCol.GetValue()[2] / 255.f;
			//	mat->AlphaModulate(1.0f);
			//	mat->ColorModulate(col[0], col[1], col[2]);
			interfaces::render_view->SetColorModulation(col);
			//	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, 1);

			interfaces::model_render->ForcedMaterialOverride(wire);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 3:
		{
			IMaterial* material = interfaces::materialsystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER);
			interfaces::render_view->SetBlend(gun_blend);
			material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			//		material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			//		material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
			interfaces::model_render->ForcedMaterialOverride(material);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 4:
		{
			interfaces::render_view->SetBlend(gun_blend);
			glass->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			interfaces::model_render->ForcedMaterialOverride(glass);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 5:
		{
			flColor[0] = 200 / 255.f;
			flColor[1] = 200 / 255.f;
			flColor[2] = 200 / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(0.2f);
			interfaces::model_render->ForcedMaterialOverride(crystal);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 6:
		{
			if (!dogtag || dogtag->IsErrorMaterial())
				return;

			float col[3] = { 0.f, 0.f, 0.f };
			col[0] = options::menu.ColorsTab.GunChamsCol.GetValue()[0] / 255.f;
			col[1] = options::menu.ColorsTab.GunChamsCol.GetValue()[1] / 255.f;
			col[2] = options::menu.ColorsTab.GunChamsCol.GetValue()[2] / 255.f;

			interfaces::render_view->SetBlend(gun_blend);
			dogtag->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			interfaces::render_view->SetColorModulation(col);
			interfaces::model_render->ForcedMaterialOverride(dogtag);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 7:
		{
			if (!mat || mat->IsErrorMaterial())
				return;
			mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			flColor[0] = options::menu.ColorsTab.GunChamsCol.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.GunChamsCol.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.GunChamsCol.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(gun_blend);
			interfaces::model_render->ForcedMaterialOverride(mat);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 8:
		{
			if (!overlay || overlay->IsErrorMaterial())
				return;
			overlay->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			flColor[0] = options::menu.ColorsTab.GunChamsCol.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.GunChamsCol.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.GunChamsCol.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(gun_blend);
			interfaces::model_render->ForcedMaterialOverride(overlay);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		}
	}



	if ((strstr(ModelName, "v_sleeve")))
	{

		switch (sleeves)
		{
		case 1:
		{
			if (!usual || usual->IsErrorMaterial())
				return;

			usual->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

			flColor[0] = options::menu.ColorsTab.SleeveChams_col.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.SleeveChams_col.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.SleeveChams_col.GetValue()[2] / 255.f;

			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(sleeve_blend);
			interfaces::model_render->ForcedMaterialOverride(usual);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 2:
		{
			if (!mat || mat->IsErrorMaterial())
				return;

			float col[3] = { 0.f, 0.f, 0.f };
			col[0] = options::menu.ColorsTab.SleeveChams_col.GetValue()[0] / 255.f;
			col[1] = options::menu.ColorsTab.SleeveChams_col.GetValue()[1] / 255.f;
			col[2] = options::menu.ColorsTab.SleeveChams_col.GetValue()[2] / 255.f;

			interfaces::render_view->SetColorModulation(col);
			interfaces::model_render->ForcedMaterialOverride(wire);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 3:
		{
			IMaterial* material = interfaces::materialsystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_OTHER);

			interfaces::render_view->SetBlend(sleeve_blend);

			material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			interfaces::model_render->ForcedMaterialOverride(material);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 4:
		{
			flColor[0] = options::menu.ColorsTab.SleeveChams_col.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.SleeveChams_col.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.SleeveChams_col.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(sleeve_blend);
			interfaces::model_render->ForcedMaterialOverride(glass);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

		}
		break;

		case 5:
		{
			flColor[0] = options::menu.ColorsTab.SleeveChams_col.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.SleeveChams_col.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.SleeveChams_col.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(sleeve_blend);
			interfaces::model_render->ForcedMaterialOverride(crystal);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 6:
		{
			if (!dogtag || dogtag->IsErrorMaterial())
				return;

			float col[3] = { 0.f, 0.f, 0.f };
			col[0] = options::menu.ColorsTab.SleeveChams_col.GetValue()[0] / 255.f;
			col[1] = options::menu.ColorsTab.SleeveChams_col.GetValue()[1] / 255.f;
			col[2] = options::menu.ColorsTab.SleeveChams_col.GetValue()[2] / 255.f;
			dogtag->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			interfaces::render_view->SetColorModulation(col);
			interfaces::model_render->ForcedMaterialOverride(dogtag);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;


		case 7:
		{
			if (!mat || mat->IsErrorMaterial())
				return;
			mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			flColor[0] = options::menu.ColorsTab.SleeveChams_col.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.SleeveChams_col.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.SleeveChams_col.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(sleeve_blend);
			interfaces::model_render->ForcedMaterialOverride(mat);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 8:
		{
			if (!overlay || overlay->IsErrorMaterial())
				return;
			overlay->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			flColor[0] = options::menu.ColorsTab.SleeveChams_col.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.SleeveChams_col.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.SleeveChams_col.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(sleeve_blend);
			interfaces::model_render->ForcedMaterialOverride(overlay);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		}

	}

	if (strstr(ModelName, XorStr("arms")) && plocal && plocal->IsAlive() && !strstr(ModelName, "v_sleeve"))
	{
		switch (HandsStyle)
		{
		case 1:
		{
			flColor[0] = options::menu.ColorsTab.HandChamsCol.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.HandChamsCol.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.HandChamsCol.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(hand_blend);

			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			interfaces::model_render->ForcedMaterialOverride(mat);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 2:
		{
			flColor[0] = options::menu.ColorsTab.HandChamsCol.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.HandChamsCol.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.HandChamsCol.GetValue()[2] / 255.f;
			//	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
			interfaces::render_view->SetBlend(hand_blend);
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::model_render->ForcedMaterialOverride(wire);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 3:
		{
			interfaces::render_view->SetBlend(1.f);
			gold->SetMaterialVarFlag(MATERIAL_VAR_ADDITIVE, true);
			interfaces::model_render->ForcedMaterialOverride(gold);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 4:
		{
			interfaces::render_view->SetBlend(1.f);
			glass->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			interfaces::model_render->ForcedMaterialOverride(glass);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 5:
		{
			flColor[0] = 5 / 255.f;
			flColor[1] = 5 / 255.f;
			flColor[2] = 5 / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(0.2f);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 6:
		{
			if (!dogtag || dogtag->IsErrorMaterial())
				return;

			flColor[0] = options::menu.ColorsTab.HandChamsCol.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.HandChamsCol.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.HandChamsCol.GetValue()[2] / 255.f;

			interfaces::render_view->SetBlend(hand_blend);
			dogtag->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::model_render->ForcedMaterialOverride(dogtag);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 7:
		{
			if (!mat || mat->IsErrorMaterial())
				return;
			mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			flColor[0] = options::menu.ColorsTab.HandChamsCol.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.HandChamsCol.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.HandChamsCol.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(hand_blend);
			interfaces::model_render->ForcedMaterialOverride(mat);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		case 8:
		{
			if (!overlay || overlay->IsErrorMaterial())
				return;
			overlay->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			flColor[0] = options::menu.ColorsTab.HandChamsCol.GetValue()[0] / 255.f;
			flColor[1] = options::menu.ColorsTab.HandChamsCol.GetValue()[1] / 255.f;
			flColor[2] = options::menu.ColorsTab.HandChamsCol.GetValue()[2] / 255.f;
			interfaces::render_view->SetColorModulation(flColor);
			interfaces::render_view->SetBlend(hand_blend);
			interfaces::model_render->ForcedMaterialOverride(overlay);
			oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
		}
		break;

		}

	}

	if (!DontDraw)
		oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	interfaces::model_render->ForcedMaterialOverride(NULL);
}
