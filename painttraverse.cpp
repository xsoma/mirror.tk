#include "includes.h"
#include "Hooks.h"

#include "Visuals.h"
backup_visuals* c_visuals = new backup_visuals(); // related to painttraverse.cpp 

void __fastcall Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	if (options::menu.visuals.Active.getstate() && options::menu.visuals.OtherNoScope.getstate() && strcmp("HudZoom", interfaces::panels->GetName(vguiPanel)) == 0)
		return;

	int w, h;
	int centerW, centerh, topH;
	interfaces::engine->GetScreenSize(w, h);

	centerW = w / 2;
	centerh = h / 2;

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;

	IClientEntity* pLocal = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (!FoundPanel)
	{
		PCHAR szPanelName = (PCHAR)interfaces::panels->GetName(vguiPanel);
		if (strstr(szPanelName, XorStr("MatSystemTopPanel")))
		{
			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}
	}
	else if (FocusOverlayPanel == vguiPanel)
	{
		interfaces::m_iInputSys->EnableInput(!options::menu.m_bIsOpen);
		if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame() && pLocal->isValidPlayer())
		{
			/*
			if (pLocal->GetWeapon2()) {

				interfaces::engine->GetScreenSize(w, h); w /= 2; h /= 2;
				if (interfaces::engine->IsInGame() && interfaces::engine->IsConnected())
				{
					int w, h;
					static float rot = 0.f;
					interfaces::engine->GetScreenSize(w, h); w /= 2; h /= 2;
					C_BaseCombatWeapon* pWeapon = pLocal->GetWeapon2();
					if (pWeapon)
					{
						short Index = (int)pWeapon->GetItemDefinitionIndex();

						if (Index != 42 && Index != 59 && Index != 500)
						{
							if (options::menu.visuals.SpreadCrosshair.getindex() != 0 && !options::menu.m_bIsOpen)
							{
								auto accuracy = pWeapon->GetInaccuracy() * (90 * 6.5);

								switch (options::menu.visuals.SpreadCrosshair.getindex())
								{
								case 1:
								{
									greyone(w, h, accuracy, 0, 1, 50, pDevice);
								}
								break;

								case 2:
								{
									colorboy69(w, h, accuracy, 0, 1, 50, pDevice);
								}
								break;

								case 3:
								{
									CircleFilledRainbowColor(w, h, accuracy, 0, 1, 50, pDevice);
								}
								break;

								case 4:
								{
									CircleFilledRainbowColor(w, h, accuracy, rot, 1, 50, pDevice);
								}
								break;
								}

								rot += 1.f;
								if (rot > 360.f)
									rot = 0.f;
							}
						}
					}
				}
			}
			*/

			RECT scrn = Render::GetViewport();
			if (options::menu.misc.Radar.getstate())
				DrawRadar();


			{

				switch (options::menu.misc.onetapsu.getstate())
				{
				case true:
				{
					if (ragebot->can_shoot || ragebot->can_autowall)
					{
						Render::Text(9, scrn.bottom - 124, Color(120 + rand() % 125, 0 + rand() % 100, 240, 255), Render::Fonts::LBY, "CAN'T MISS = CAN SHOOT");
					}

					else
					{
						Render::Text(9, scrn.bottom - 124, Color(190, 190, 190, 255), Render::Fonts::LBY, "CAN MISS = CAN'T SHOOT :(");
					}
				}
				break;
				}

				switch (options::menu.misc.muslims.getstate())
				{
				case true:
				{
					C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(hackManager.plocal()->GetActiveWeaponHandle());

					if (pWeapon)
					{
						if (game_utils::IsGrenade(pWeapon))
						{
							if (interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::MOUSE_LEFT))
								Render::Text(9, scrn.bottom - 139, Color(120 + rand() % 125, 0 + rand() % 100, 120, 255), Render::Fonts::LBY, "SUICIDE-BOMBING IN PROGRESS");
							else
								Render::Text(9, scrn.bottom - 139, Color(250, 100, 100, 255), Render::Fonts::LBY, "ISLAM DETECTED");

						}
					}
				}
				break;
				}

				RECT TextSize_2 = Render::GetTextSize(Render::Fonts::LBYIndicator, " Lag Comp Status:");
				bool breaklagcomp = false;
				auto last_origin = pLocal->GetAbsOrigin2();
				if (pLocal->GetAbsOrigin2() != last_origin)
				{
					if (!(pLocal->GetFlags() & FL_ONGROUND) && pLocal->GetAbsOrigin2().Length2DSqr() > 4096) {
						breaklagcomp = true;
						last_origin = pLocal->GetAbsOrigin2();
					}
				}

				else if (!(pLocal->GetFlags() & FL_ONGROUND) && pLocal->getvelocity().length2d() > 125 && overall::bSendPacket && overall::ChokeAmount == c_fakelag->break_lagcomp_mm_2() || overall::ChokeAmount == 5) {
					breaklagcomp = true;

				}
				else {
					breaklagcomp = false;
				}
				//	Render::Text(6, scrn.bottom - 88, breaklagcomp ? Color(0, 255, 30, 255) : Color(255, 0, 30, 255), Render::Fonts::LBY, "LC");

				switch (options::menu.visuals.LCIndicator.getindex())
				{
				case 1:
				{
					Render::Text(9, scrn.bottom - 88, breaklagcomp ? Color(120, 0, 240, 255) : Color(255, 0, 30, 0), Render::Fonts::LBY, "LC");
				}
				break;

				case 2:
				{
					if (breaklagcomp)
					{
						Render::Text(9, centerh + 115, Color(255, 255, 255, 255), Render::Fonts::xd, " Lag Comp Status:");
						Render::Text(TextSize_2.left + 11, centerh + 115, Color(0, 90, 250, 255), Render::Fonts::xd, "Active");
					}

					if (!breaklagcomp)
					{
						Render::Text(9, centerh + 115, Color(255, 255, 255, 255), Render::Fonts::xd, " Lag Comp Status:");
						Render::Text(TextSize_2.left + 11, centerh + 115, Color(255, 0, 80, 255), Render::Fonts::xd, "Normal");
					}
				}
				break;
				}

			}

			if (c_antiaim->_do())
			{
				if (options::menu.misc.banana.getstate())
					Render::Text(9, scrn.bottom - 184, Color(255, 225, 53, 255), Render::Fonts::LBY, "BANANA AA");

				RECT View = Render::GetViewport();
				int MidX = View.right / 2;
				int MidY = View.bottom / 2;
				RECT TextSize = Render::GetTextSize(Render::Fonts::xd, "--->");  int n = MidX - (TextSize.left / 2);
				RECT TextSize2 = Render::GetTextSize(Render::Fonts::xd, "Fake"); int nn = MidX - (TextSize.left / 2);
				if (bigboi::indicator == 1) {
					Render::Text(n, MidY + 20, Color(255, 255, 255, 255), Render::Fonts::xd, " --->");
					Render::Text(nn, MidY + 35, Color(255, 255, 255, 255), Render::Fonts::xd, "Fake");
				}
				else if (bigboi::indicator == 2) {
					Render::Text(n, MidY + 20, Color(255, 255, 255, 255), Render::Fonts::xd, "<---");
					Render::Text(nn, MidY + 35, Color(255, 255, 255, 255), Render::Fonts::xd, "Fake");
				}
			}

			switch (options::menu.visuals.FakeDuckIndicator.getindex())
			{
			case 1:
			{
				if (GetAsyncKeyState(options::menu.misc.fake_crouch_key.GetKey()))
				{
					Render::Text(9, scrn.bottom - 54, Color(120, 0, 240, 255), Render::Fonts::LBY, "FD");
				}

			}
			break;

			case 2:
			{
				RECT TextSize_2 = Render::GetTextSize(Render::Fonts::LBYIndicator, " Fake Duck Status:");
				if (GetAsyncKeyState(options::menu.misc.fake_crouch_key.GetKey()) && !c_fakelag->shot)
				{
					Render::Text(9, centerh + 130, Color(255, 255, 255, 255), Render::Fonts::xd, " Fake Duck Status:");
					Render::Text(TextSize_2.left + 11, centerh + 130, Color(0, 90, 250, 255), Render::Fonts::xd, "Active");
				}
				else
				{
					Render::Text(9, centerh + 130, Color(255, 255, 255, 255), Render::Fonts::xd, " Fake Duck Status:");
					Render::Text(TextSize_2.left + 11, centerh + 130, Color(255, 0, 80, 255), Render::Fonts::xd, "Inactive");
				}
			}
			break;
			}

			if (options::menu.visuals.LBYIndicator.getindex() != 0)
			{

				RECT TextSize = Render::GetTextSize(Render::Fonts::LBY, "LBY");
				RECT TextSize_2 = Render::GetTextSize(Render::Fonts::LBYIndicator, " LBY Status:");

				bool invalid_lby = (LastAngleAAReal.y - pLocal->get_lowerbody() >= -35 && LastAngleAAReal.y - pLocal->get_lowerbody() <= 35) || pLocal->IsMoving();
				switch (options::menu.visuals.LBYIndicator.getindex())
				{
				case 1:
				{
					if (!invalid_lby)
						Render::Text(9, scrn.bottom - 71, Color(120, 0, 240, 255), Render::Fonts::LBY, "LBY");
				}
				break;

				case 2:
				{
					if (invalid_lby)
					{
						Render::Text(9, centerh + 100, Color(255, 255, 255, 255), Render::Fonts::xd, " Lby Status:");
						Render::Text(TextSize_2.left + 11, centerh + 100, Color(255, 0, 80, 255), Render::Fonts::xd, "Normal");
					}
					else
					{
						Render::Text(9, centerh + 100, Color(255, 255, 255, 255), Render::Fonts::xd, " Lby Status:");
						Render::Text(TextSize_2.left + 11, centerh + 100, Color(0, 90, 250, 255), Render::Fonts::xd, "Broken");
					}
				}
				break;
				}
			}


			float yaw_difference = c_beam->visual_angle - LastAngleAAFake.y;
			bool fake_green = yaw_difference >= 45.f || yaw_difference <= -45.f;
			bool fake_orange = (yaw_difference < 45.f && yaw_difference > 20.f) || (yaw_difference > -45.f && yaw_difference < -45.f);

			switch (options::menu.visuals.fake_indicator.getindex())
			{
			case 1:
			{
				RECT TextSize = Render::GetTextSize(Render::Fonts::LBY, "FAKE");

				if (fake_green)
				{
					Render::Text(9, scrn.bottom - 105, Color(120, 0, 240, 255), Render::Fonts::LBY, "FAKE");
				}

				else if (fake_orange)
				{
					Render::Text(9, scrn.bottom - 105, Color(255, 150, 10, 255), Render::Fonts::LBY, "FAKE");
				}

			}
			break;

			case 2:
			{
				RECT TextSize_2 = Render::GetTextSize(Render::Fonts::LBYIndicator, " Desync Status:");
				switch (fake_green)
				{
				case true:
				{
					Render::Text(9, centerh + 145, Color(255, 255, 255, 255), Render::Fonts::xd, " Fake Status:");
					Render::Text(TextSize_2.left + 11, centerh + 145, Color(0, 90, 250, 255), Render::Fonts::xd, "Optimal");
				}
				break;

				case false:
				{
					Render::Text(9, centerh + 145, Color(255, 255, 255, 255), Render::Fonts::xd, " Fake Status:");
					Render::Text(TextSize_2.left + 11, centerh + 145, Color(255, 0, 80, 255), Render::Fonts::xd, "Minimal");
				}
				break;
				}
			}
			break;
			}



			switch (options::menu.visuals.manualaa_type.getindex())
			{
			case 1:
			{
				c_visuals->single_arrow();
			}
			break;

			case 2:
			{
				c_visuals->all_arrows();
			}
			break;
			}


		}

		if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame())
		{
			Hacks::DrawHacks();
			if (options::menu.visuals.OtherHitmarker.getstate() && pLocal && pLocal->IsAlive())
				hitmarker::singleton()->on_paint();
		}

		if (options::menu.m_bIsOpen && options::menu.ColorsTab.menu_backdrop.getstate())
		{
			Drop::DrawBackDrop();
		}

		if (options::menu.m_bIsOpen && options::menu.ColorsTab.menu_healthshot.getstate()) {
			int m = 0;
			if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame()) {
				if (m < 9) {
					hackManager.plocal()->health_boost_time() = interfaces::globals->curtime + 0.4f;
					m++;
				}
				else
					m = 0.f;
			}
		}

		options::DoUIFrame();
	}

	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);
}