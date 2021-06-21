#include "includes.h"
#include "Hooks.h"

void hitsound()
{
	const char* _custom = "csgo\\sound\\mirror_custom.wav";
	switch (options::menu.misc.hitmarker_sound.getindex())
	{
	case 1: PlaySoundA(cod, NULL, SND_MEMORY | SND_ASYNC);
		break;
	case 2: interfaces::surface->PlaySound("buttons\\arena_switch_press_02.wav");
		break;
	case 3: PlaySoundA(bubble, NULL, SND_MEMORY | SND_ASYNC);
		break;
	case 4: PlaySoundA(bameware_sound, NULL, SND_MEMORY | SND_ASYNC);
		break;
	case 5: PlaySoundA(anime, NULL, SND_MEMORY | SND_ASYNC);
		break;
	case 6: PlaySoundA(hitler_wav, NULL, SND_MEMORY | SND_ASYNC);
		break;
	case 7: interfaces::surface->PlaySound("training\\bell_impact");
		break;
	case 8: interfaces::surface->PlaySound("training\\timer_bell.wav");
		break;
	case 9: interfaces::surface->PlaySound("play buttons\\button22.wav");
		break; // training\\bell_impact.wav
	case 10: interfaces::surface->PlaySound("play items\\flashlight1.wav");
		break;
	case 11: interfaces::surface->PlaySound("play physics\metal\metal_chainlink_impact_soft3.wav");
		break;
	case 12: PlaySoundA(_custom, NULL, SND_ASYNC);
		break;
	}
}

player_info_t GetInfo(int Index) 
{
	player_info_t Info;
	interfaces::engine->GetPlayerInfo(Index, &Info);
	return Info;
}

auto HitgroupToString = [](int hitgroup) -> std::string
{
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return "HEAD";
	case HITGROUP_CHEST:
		return "CHEST";
	case HITGROUP_STOMACH:
		return "STOMACH";
	case HITGROUP_LEFTARM:
		return "LEFT ARM";
	case HITGROUP_RIGHTARM:
		return "RIGHT ARM";
	case HITGROUP_LEFTLEG:
		return "LEFT LEG";
	case HITGROUP_RIGHTLEG:
		return "RIGHT LEG";
	default:
		return "BODY";
	}
};

void Msg(const char* msg, ...)
{
	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg"); char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list); //Calls the function, we got the address above.
}

void draw_hitboxes(IClientEntity* pEntity, int r, int g, int b, int a, float duration, float diameter) 
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

		interfaces::DebugOverlay->DrawPill(vMin, vMax, hitbox->m_flRadius, r, g, b, a, duration);
	}
}

bool __fastcall Hooked_FireEventClientSide(PVOID ECX, PVOID EDX, IGameEvent* Event)
{
	CBulletListener::singleton()->OnStudioRender();
	IClientEntity* localplayer = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame())
	{
		std::vector<dropdownboxitem> spike = options::menu.misc.killsay.items;

		std::string event_name = Event->GetName();

		bool z = false;
		bool _done = false;
		if (!strcmp(Event->GetName(), "round_prestart") || (!strcmp(Event->GetName(), "game_end")))
		{
			c_misc->anotherpcheck = true;
			if (!z)
			{
				C_LagCompensation::get().reset();
				z = true;
			}

		}

		if (!strcmp(Event->GetName(), "round_start"))
		{
			z = false;
			c_misc->anotherpcheck = false;

		} // 	_done = false;

		if (!strcmp(Event->GetName(), "round_end"))
		{
			_done = false;
		}

		if (!_done && localplayer && localplayer->IsAlive())
		{
			C_LagCompensation::get().reset();
			_done = true;
		}

		if (!strcmp(Event->GetName(), "player_death"))
		{
			//	skinchanger.apply_killcon(Event);	

			int deadfag = Event->GetInt("userid");
			int attackingfag = Event->GetInt("attacker");

			if (interfaces::engine->GetPlayerForUserID(deadfag) == interfaces::engine->get_localplayer() && interfaces::engine->GetPlayerForUserID(attackingfag) != interfaces::engine->get_localplayer())
			{
				if (spike[1].GetSelected)
				{
					/*		if (!deathmsg.empty()) {
								std::string msg = deathmsg[rand() % deathmsg.size()];
								std::string str;
								str.append("say ");
								str.append(msg);
								interfaces::engine->ClientCmd_Unrestricted(str.c_str());
							}*/
				}

				if (options::menu.misc.excuses.getstate())
				{
					/*	if (!excuse_msg.empty()) {
							interfaces::cvar->ConsoleColorPrintf(Color(220, 5, 250, 255), "Mirror: ");
							std::string One = "you died because: ";
							std::string Two = excuse_msg[rand() % excuse_msg.size()];

							std::string newline = ".     \n";
							std::string uremam = One + Two + newline;

							a_c->ConColorMsg(Color(250, 200, 10, 255), uremam.c_str());

						}*/
				}
			}
			if (spike[0].GetSelected && interfaces::engine->GetPlayerForUserID(deadfag) != interfaces::engine->get_localplayer() && interfaces::engine->GetPlayerForUserID(attackingfag) == interfaces::engine->get_localplayer())
			{
				/*		if (!killmsg.empty())
						{
							std::string msg = killmsg[rand() % killmsg.size()];
							std::string str;
							str.append("say ");
							str.append(msg);
							interfaces::engine->ClientCmd_Unrestricted(str.c_str());
						}*/
			}
		}

		if (!strcmp(Event->GetName(), "game_newmap"))
		{

			if (!z)
			{
				C_LagCompensation::get().reset();

				options::menu.visuals.colmodupdate.SetState(true);

				z = true;
			}

			a_c->mirror_aesthetic_console();
			interfaces::cvar->ConsoleColorPrintf(Color(10, 250, 200, 255), " [info] changing map.     \n");

			if (options::menu.visuals.colmod.GetValue() < 100.f)
			{
				options::menu.visuals.colmodupdate.SetState(true);
				a_c->mirror_aesthetic_console();
				interfaces::cvar->ConsoleColorPrintf(Color(250, 0, 200, 255), " [info] refreshed world modulation.     \n");
			}

		}

		if (!strcmp(Event->GetName(), "player_hurt"))
		{
			int attackerid = Event->GetInt("attacker");
			int entityid = interfaces::engine->GetPlayerForUserID(attackerid);
			if (entityid == interfaces::engine->get_localplayer())
			{
				hitsound();
				Globals::hit[entityid] = true;
				int nUserID = Event->GetInt("attacker");
				int nDead = Event->GetInt("userid");
				int gaylol = Event->GetInt("hitgroup");

				if ((nUserID || nDead) && nUserID != nDead)
				{
					player_info_t killed_info = GetInfo(interfaces::engine->GetPlayerForUserID(nDead));
					player_info_t killer_info = GetInfo(interfaces::engine->GetPlayerForUserID(nUserID));
					IClientEntity* hurt = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->GetPlayerForUserID(Event->GetInt("userid")));

					if (options::menu.ColorsTab.DebugLagComp.getstate())
					{
						studiohdr_t* studio_hdr = interfaces::model_info->GetStudiomodel(hurt->GetModel());
						mstudiohitboxset_t* set = studio_hdr->GetHitboxSet(0);
						for (int i = 0; i < set->numhitboxes; i++)
						{
							mstudiobbox_t* hitbox = set->GetHitbox(i);
							if (!hitbox)
								continue;

							draw_hitboxes(hurt, 220, 220, 220, 250, interfaces::globals->interval_per_tick * 2, hitbox->m_flRadius);
						}
					}
					auto remaining_health = Event->GetString("health");
					int remainaing = Event->GetInt("health");
					auto dmg_to_health = Event->GetString("dmg_health");
					std::string szHitgroup = HitgroupToString(gaylol);
					interfaces::cvar->ConsoleColorPrintf(Color(220, 5, 250, 255), "Mirror: ");
					std::string One = "-";
					std::string Two = dmg_to_health;
					std::string Three = " in the ";
					std::string Four = szHitgroup;
					std::string gey = " of ";
					std::string yes = killed_info.name;
					std::string yyes = " [";
					std::string yyyes = " hp: ";
					std::string yyyyes = remaining_health;
					std::string yyyyyes = " ]";
					std::string newline = ".     \n";//no,i'm not retarded, i tried with stringstream but it didn't work.
					std::string uremam = One + Two + Three + Four + gey + yes + yyes + yyyes + yyyyes + yyyyyes + newline;


					switch (options::menu.visuals.logs.getindex())
					{
					case 1: Msg(uremam.c_str());
						break;
					case 2: a_c->ConColorMsg(Color(options::menu.ColorsTab.console_colour.GetValue()), uremam.c_str());
						break;
					}

				}
			}
		}

		if (options::menu.aimbot.resolver.getindex() > 0)
		{
			auto entity = interfaces::ent_list->get_client_entity(interfaces::engine->GetPlayerForUserID(Event->GetInt("userid")));

			IClientEntity* pLocal = hackManager.plocal();
			a_c->missed_due_to_desync(Event);
			if ((!strcmp(Event->GetName(), "weapon_fire")) && !interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::MOUSE_LEFT))
			{
				auto userID = Event->GetInt("userid");
				auto attacker = interfaces::engine->GetPlayerForUserID(userID);
				if (attacker)
				{
					if (attacker == interfaces::engine->get_localplayer() && entity)
					{
						a_c->mirror_aesthetic_console();
						interfaces::cvar->ConsoleColorPrintf(Color(10, 250, 200, 255), " [debug] weapon fired.     \n");

						if (overall::userCMD && (overall::userCMD->buttons & IN_ATTACK))
							Globals::Shots += 1;
					}
				}

				if (!strcmp(Event->GetName(), "player_hurt"))
				{
					auto player_hit = Event->GetInt("userid");
					int shooter = Event->GetInt("attacker");

					if (entity && attacker && shooter && shooter == attacker && shooter != player_hit)
					{
						a_c->mirror_aesthetic_console();
						interfaces::cvar->ConsoleColorPrintf(Color(10, 250, 200, 255), " [debug] player hit.     \n");
						Globals::Hits += 1;
					}
				}
			}
		}
	}
	return oFireEventClientSide(ECX, Event);
}