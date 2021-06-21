#include "includes.h"
#include "Hooks.h"

void draw_hitbox_bt(IClientEntity* pEntity, int r, int g, int b, int a, float duration, float diameter) 
{
	matrix3x4 matrix[128];

	if (!pEntity->SetupBones(matrix, 128, 0x00000100, pEntity->GetSimulationTime()))
		return;

	studiohdr_t* hdr = interfaces::model_info->GetStudiomodel(pEntity->GetModel());
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);

	for (int i = 0; i < set->numhitboxes; i++) {
		mstudiobbox_t* hitbox = set->GetHitbox(i);

		if (!hitbox)
			continue;

		vector vMin, vMax;
		auto VectorTransform_Wrapperx = [](const vector& in1, const matrix3x4& in2, vector& out)
		{
			auto VectorTransform = [](const float* in1, const matrix3x4& in2, float* out)
			{
				auto DotProducts = [](const float* v1, const float* v2)
				{
					return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
				};
				out[0] = DotProducts(in1, in2[0]) + in2[0][3];
				out[1] = DotProducts(in1, in2[1]) + in2[1][3];
				out[2] = DotProducts(in1, in2[2]) + in2[2][3];
			};

			VectorTransform(&in1.x, in2, &out.x);
		};

		VectorTransform_Wrapperx(hitbox->bbmin, matrix[hitbox->bone], vMin);
		VectorTransform_Wrapperx(hitbox->bbmax, matrix[hitbox->bone], vMax);

		interfaces::DebugOverlay->DrawPill(vMin, vMax, diameter, r, g, b, a, duration);
	}
}

void __fastcall  Hooked_SceneEnd(void* pEcx, void* pEdx)
{
	Hooks::VMTRenderView.GetMethod<SceneEnd_t>(9)(pEcx);

	IClientEntity* local_player = hackManager.plocal();

	if (!interfaces::engine->IsInGame() || !interfaces::engine->IsConnected())
		return pSceneEnd(pEcx);

	pSceneEnd(pEcx);

	float blend_vis = options::menu.visuals.enemy_blend.GetValue() / 100;
	float blend_invis = options::menu.visuals.enemy_blend_invis.GetValue() / 100;

	for (int i = 1; i <= interfaces::globals->max_clients; i++)
	{
		auto ent = interfaces::ent_list->get_client_entity(i);

		if (ent)
		{
			if (options::menu.ColorsTab.BackTrackBones2.getstate())
			{
				if (ent->getvelocity().length2d() > 24 && ent->cs_player() && !ent->IsDormant() && local_player->IsAlive() && ent->team() != local_player->team())
					draw_hitbox_bt(ent, options::menu.ColorsTab.misc_lagcompBones.GetValue()[0], options::menu.ColorsTab.misc_lagcompBones.GetValue()[1], options::menu.ColorsTab.misc_lagcompBones.GetValue()[2], 255, 0.2, 0);
			}
			pSceneEnd(pEcx);
		}

	}
}