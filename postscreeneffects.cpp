#include "includes.h"
#include "Hooks.h"

int __stdcall Hooked_DoPostScreenEffects(int a1)
{
	/*	auto m_local = hackManager.pLocal();

		for (auto i = 0; i < interfaces::glow_manager->size; i++)
		{
			auto glow_object = &interfaces::glow_manager->m_GlowObjectDefinitions[i];
			auto entity = reinterpret_cast<IClientEntity*>(glow_object->m_pEntity);


			if (!entity || glow_object->IsUnused() || !m_local)
				continue;


			if (strstr(entity->GetClientClass()->m_pNetworkName, "Weapon"))
			{
				if (options::menu.visuals.OtherEntityGlow.getstate())
				{
					float m_flRed = options::menu.ColorsTab.GlowOtherEnt.GetValue()[0], m_flGreen = options::menu.ColorsTab.GlowOtherEnt.GetValue()[1], m_flBlue = options::menu.ColorsTab.GlowOtherEnt.GetValue()[2];

					glow_object->m_bPulsatingChams = 3;
					glow_object->m_vGlowColor = Vector(m_flRed / 255, m_flGreen / 255, m_flBlue / 255);
					glow_object->m_flGlowAlpha = 1.f;
					glow_object->m_bRenderWhenOccluded = true;
					glow_object->m_bRenderWhenUnoccluded = false;

					c_beam->glow = true;
				}
			}

			if (entity->isValidPlayer())
			{
				if (entity == m_local && options::menu.visuals.Glowz_lcl.GetValue() > 0)
				{
					if (m_local->IsAlive() && options::menu.visuals.localmaterial.getindex() < 6)
					{
						float m_flRed = options::menu.ColorsTab.GlowLocal.GetValue()[0], m_flGreen = options::menu.ColorsTab.GlowLocal.GetValue()[1], m_flBlue = options::menu.ColorsTab.GlowLocal.GetValue()[2];
						glow_object->m_bPulsatingChams = 3;
						glow_object->m_vGlowColor = Vector(m_flRed / 255, m_flGreen / 255, m_flBlue / 255);
						glow_object->m_flGlowAlpha = options::menu.visuals.Glowz_lcl.GetValue() / 100;
						glow_object->m_bRenderWhenOccluded = true;
						glow_object->m_bRenderWhenUnoccluded = false;

					}

				}

				if (entity->team() != m_local->team() && options::menu.visuals.GlowZ.GetValue() > 0)
				{
					float m_flRed = options::menu.ColorsTab.GlowEnemy.GetValue()[0], m_flGreen = options::menu.ColorsTab.GlowEnemy.GetValue()[1], m_flBlue = options::menu.ColorsTab.GlowEnemy.GetValue()[2];

					glow_object->m_vGlowColor = Vector(m_flRed / 255, m_flGreen / 255, m_flBlue / 255);
					glow_object->m_bPulsatingChams = 3;
					glow_object->m_flGlowAlpha = options::menu.visuals.GlowZ.GetValue() / 100;
					glow_object->m_bRenderWhenOccluded = true;
					glow_object->m_bRenderWhenUnoccluded = false;
					//	glow_object->m_bPulsatingChams = 2;
				}

				if (entity->team() == m_local->team() && entity != m_local && options::menu.visuals.team_glow.GetValue() > 0)
				{
					float m_flRed = options::menu.ColorsTab.GlowTeam.GetValue()[0], m_flGreen = options::menu.ColorsTab.GlowTeam.GetValue()[1], m_flBlue = options::menu.ColorsTab.GlowTeam.GetValue()[2];
					glow_object->m_bPulsatingChams = 3;
					glow_object->m_vGlowColor = Vector(m_flRed / 255, m_flGreen / 255, m_flBlue / 255);
					glow_object->m_flGlowAlpha = options::menu.visuals.team_glow.GetValue() / 100;
					glow_object->m_bRenderWhenOccluded = true;
					glow_object->m_bRenderWhenUnoccluded = false;
					//	glow_object->m_bPulsatingChams = 1;
				}

			}

		}
		*/

	return o_DoPostScreenEffects(interfaces::ClientMode, a1);
}