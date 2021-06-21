#include "SkinChanger.h"
#include "Hacks.h"
SkinChanger skinchanger;
std::unordered_map<int, SkinStruct>  m_skins;
std::unordered_map<int, const char*> g_ViewModelCFG;
std::unordered_map<const char*, const char*> g_KillIconCfg;

#define iKnife options::menu.skin.t_knife_index.getindex() 

int SkinChanger::get_skin_id_knife()
{
	int id = options::menu.skin.t_knife_skin_id.getindex();

	if (id != 0)
	{
		switch (id)
		{
			case 1: return 415;
				break;
			case 2: return 416;
				break;
			case 3: return 417;
				break;
			case 4: return 420;
				break;
			case 5: return 38;
				break;
			case 6: return 413;
				break;
			case 7: return 570;
				break;
			case 8: return 568;
				break;
			case 9: return 59;
				break;
			case 10: return 102;
				break;
			case 11: return 98;
				break;
			case 12: return 562;
				break;
		}
	}
}

#define iKnifePaintkit get_skin_id_knife()//listbox with knife paintkits tigertooth,doppler,fade etc.//combobox for the knifemodel, you can add the new ones if you wish to as well
#define flKnifeWear options::menu.skin.t_knife_wear.GetValue() / 1000 //slider for the wear
#define iKnifeQuality 1 //combo for quality

void SkinChanger::update_settings()
{

	IClientEntity * m_local = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	if (!m_local->isValidPlayer())
		return;

	if (!m_local->weapon())
		return;
//	if (interfaces::pinput->m_fCameraInThirdPerson)
//		return;

	static int last_model;
	static int last_paintkit;
	static float last_wear;
	static int last_quality;
	if (last_paintkit != iKnifePaintkit)
	{
		m_skins[WEAPON_KNIFE].nFallbackPaintKit = iKnifePaintkit;
		m_skins[WEAPON_KNIFE_T].nFallbackPaintKit = iKnifePaintkit;
		last_paintkit = iKnifePaintkit;
	}
	/*
	if (scar20_paint != m_skins[WEAPON_SCAR20].nFallbackPaintKit)
	{
		get_skin_id_sniperScar();
		scar20_paint = m_skins[WEAPON_SCAR20].nFallbackPaintKit;
	}

	if (awp_paint != m_skins[WEAPON_AWP].nFallbackPaintKit)
	{
		get_skin_id_sniperAWP();
		awp_paint = m_skins[WEAPON_AWP].nFallbackPaintKit;
	}
	*/
	if (last_model != iKnife)
	{
		std::vector<char*> icons;
		icons.push_back("bayonet");
		icons.push_back("knife_m9_bayonet");
		icons.push_back("knife_butterfly");
		icons.push_back("knife_flip");
		icons.push_back("knife_gut");
		icons.push_back("knife_karambit");
		icons.push_back("knife_tactical");
		icons.push_back("knife_falchion");
		icons.push_back("knife_survival_bowie");
		icons.push_back("knife_push");
		icons.push_back("knife_widowmaker");
		icons.push_back("knife_stiletto");
		icons.push_back("knife_ursus");
		g_KillIconCfg["knife_default_ct"] = icons[iKnife - 1];
		g_KillIconCfg["knife_t"] = icons[iKnife - 1];
		int nOriginalKnifeCT = interfaces::model_info->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
		int nOriginalKnifeT = interfaces::model_info->GetModelIndex("models/weapons/v_knife_default_t.mdl");
		std::vector<char*> models;
		models.push_back("models/weapons/v_knife_bayonet.mdl");
		models.push_back("models/weapons/v_knife_m9_bay.mdl");
		models.push_back("models/weapons/v_knife_butterfly.mdl");
		models.push_back("models/weapons/v_knife_flip.mdl");
		models.push_back("models/weapons/v_knife_gut.mdl");
		models.push_back("models/weapons/v_knife_karam.mdl");
		models.push_back("models/weapons/v_knife_tactical.mdl");
		models.push_back("models/weapons/v_knife_falchion_advanced.mdl");
		models.push_back("models/weapons/v_knife_survival_bowie.mdl");
		models.push_back("models/weapons/v_knife_push.mdl");
		models.push_back("models/weapons/v_knife_widowmaker.mdl"); 
		models.push_back("models/weapons/v_knife_stiletto.mdl");
		models.push_back("models/weapons/v_knife_ursus.mdl");
		g_ViewModelCFG[nOriginalKnifeCT] = models[iKnife - 1];
		g_ViewModelCFG[nOriginalKnifeT] = models[iKnife - 1];

		switch (iKnife)
		{
		case 1:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_BAYONET;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_BAYONET;
		}
		break;

		case 2:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_M9_BAYONET;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_M9_BAYONET;
		}
		break;

		case 3:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_BUTTERFLY;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_BUTTERFLY;
		}
		break;

		case 4:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_FLIP;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_FLIP;
		}
		break;

		case 5:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_GUT;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_GUT;
		}
		break;

		case 6:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_KARAMBIT;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_KARAMBIT;
		}
		break;

		case 7:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_TACTICAL;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_TACTICAL;
		}
		break;

		case 8:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_FALCHION;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_FALCHION;
		}
		break;

		case 9:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_SURVIVAL_BOWIE;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_SURVIVAL_BOWIE;
		}
		break;

		case 10:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_PUSH;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_PUSH;
		}
		break;


		case 11:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_WIDOWMAKER;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_WIDOWMAKER;
		}
		break;

		case 12:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_STILETTO;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_STILETTO;
		}
		break;

		case 13:
		{
			m_skins[WEAPON_KNIFE].nItemDefinitionIndex = WEAPON_KNIFE_URSUS;
			m_skins[WEAPON_KNIFE_T].nItemDefinitionIndex = WEAPON_KNIFE_URSUS;
		}
		break;
		}

		last_model = iKnife;
		if (last_wear != flKnifeWear)
		{
			m_skins[WEAPON_KNIFE].flFallbackWear = flKnifeWear;
			m_skins[WEAPON_KNIFE_T].flFallbackWear = flKnifeWear;
			last_wear = flKnifeWear;
		}
		if (last_quality != iKnifeQuality && iKnifeQuality)
		{
			std::vector<int> qualities;
			qualities.push_back(0);
			qualities.push_back(1);
			qualities.push_back(2);
			qualities.push_back(3);
			qualities.push_back(5);
			qualities.push_back(6);
			qualities.push_back(7);
			qualities.push_back(8);
			qualities.push_back(9);
			qualities.push_back(10);
			qualities.push_back(12);
			m_skins[WEAPON_KNIFE].nEntityQuality = qualities[iKnifeQuality];
			m_skins[WEAPON_KNIFE_T].nEntityQuality = qualities[iKnifeQuality];
			interfaces::engine->ClientCmd_Unrestricted("clear");
			last_quality = iKnifeQuality;
		}
		skinchanger.set_viewmodel();
	}

}
void SkinChanger::set_skins()
{
//	get_skin_id_sniperAWP();
//	get_skin_id_sniperScar();

	/*
	m_skins[WEAPON_AK47].nFallbackPaintKit = 707;
	m_skins[WEAPON_M4A4].nFallbackPaintKit = 695;
	m_skins[WEAPON_M4A1S].nFallbackPaintKit = 714;
	m_skins[WEAPON_ELITE].nFallbackPaintKit = 396;
	m_skins[WEAPON_FIVESEVEN].nFallbackPaintKit = 464;
	m_skins[WEAPON_FAMAS].nFallbackPaintKit = 604;
	m_skins[WEAPON_GALIL].nFallbackPaintKit = 478;
	m_skins[WEAPON_M249].nFallbackPaintKit = 547;
	m_skins[WEAPON_MAC10].nFallbackPaintKit = 246;
	m_skins[WEAPON_P90].nFallbackPaintKit = 359;
	m_skins[WEAPON_UMP45].nFallbackPaintKit = 436;
	m_skins[WEAPON_XM1014].nFallbackPaintKit = 314;
	m_skins[WEAPON_BIZON].nFallbackPaintKit = 508;
	m_skins[WEAPON_MAG7].nFallbackPaintKit = 462;
	m_skins[WEAPON_NEGEV].nFallbackPaintKit = 369;
	m_skins[WEAPON_SAWEDOFF].nFallbackPaintKit = 390;
	m_skins[WEAPON_TEC9].nFallbackPaintKit = 459;
	m_skins[WEAPON_P2000].nFallbackPaintKit = 246;
	m_skins[WEAPON_MP7].nFallbackPaintKit = 481;
	m_skins[WEAPON_MP9].nFallbackPaintKit = 482;
	m_skins[WEAPON_NOVA].nFallbackPaintKit = 158;
	m_skins[WEAPON_DEAGLE].nFallbackPaintKit = 37;
	m_skins[WEAPON_DEAGLE].nFallbackStatTrak = 420;
	m_skins[WEAPON_P250].nFallbackPaintKit = 388;
	m_skins[WEAPON_REVOLVER].nFallbackPaintKit = 102;
	m_skins[WEAPON_REVOLVER].nFallbackStatTrak = 666;
	m_skins[WEAPON_GLOCK].nFallbackPaintKit = 38;
	m_skins[WEAPON_G3SG1].nFallbackPaintKit = 511;
	m_skins[WEAPON_G3SG1].nFallbackStatTrak = 1337;
	m_skins[WEAPON_USPS].nFallbackPaintKit = 504;
	m_skins[WEAPON_USPS].nFallbackStatTrak = 666;
	m_skins[WEAPON_SCAR20].nFallbackPaintKit = 312;
	m_skins[WEAPON_SSG08].nFallbackPaintKit = 269;
	m_skins[WEAPON_SSG08].flFallbackWear = 0.0001;
	m_skins[WEAPON_SG553].nFallbackPaintKit = 39;
	m_skins[WEAPON_AUG].nFallbackPaintKit = 455;
	*/
	interfaces::engine->ClientCmd_Unrestricted("clear");
}


void SkinChanger::set_viewmodel()
{
	bool has_model = false;
	if (iKnife)
		has_model = true;

	knifemodel = iKnife;

	if (!has_model)
	{
		int nOriginalKnifeCT = interfaces::model_info->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
		int nOriginalKnifeT = interfaces::model_info->GetModelIndex("models/weapons/v_knife_default_t.mdl");
		g_ViewModelCFG[nOriginalKnifeCT] = "models/weapons/v_knife_default_ct.mdl";
		g_ViewModelCFG[nOriginalKnifeT] = "models/weapons/v_knife_default_t.mdl";
		interfaces::engine->ClientCmd_Unrestricted("clear");
	}
	else
	{
		int nOriginalKnifeCT = interfaces::model_info->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
		int nOriginalKnifeT = interfaces::model_info->GetModelIndex("models/weapons/v_knife_default_t.mdl");
		std::vector<char*> models;
		models.push_back("models/weapons/v_knife_bayonet.mdl");
		models.push_back("models/weapons/v_knife_m9_bay.mdl");
		models.push_back("models/weapons/v_knife_butterfly.mdl");
		models.push_back("models/weapons/v_knife_flip.mdl");
		models.push_back("models/weapons/v_knife_gut.mdl");
		models.push_back("models/weapons/v_knife_karam.mdl");
		models.push_back("models/weapons/v_knife_tactical.mdl");
		models.push_back("models/weapons/v_knife_falchion_advanced.mdl");
		models.push_back("models/weapons/v_knife_survival_bowie.mdl");
		models.push_back("models/weapons/v_knife_push.mdl");
		models.push_back("models/weapons/v_knife_widowmaker.mdl");
		models.push_back("models/weapons/v_knife_stiletto.mdl");
		models.push_back("models/weapons/v_knife_ursus.mdl");
		g_ViewModelCFG[nOriginalKnifeCT] = models[iKnife - 1];
		g_ViewModelCFG[nOriginalKnifeT] = models[iKnife - 1];
	}
}
bool SkinChanger::apply_skins(CBaseAttributableItem* pWeapon, int nWeaponIndex)
{
	if (m_skins.find(nWeaponIndex) == m_skins.end())
		return false;
	if (!game_utils::IsKnife(pWeapon))
		return false;

	*pWeapon->GetFallbackPaintKit() = m_skins[nWeaponIndex].nFallbackPaintKit;
	*pWeapon->GetEntityQuality() = m_skins[nWeaponIndex].nEntityQuality;
	*pWeapon->GetFallbackSeed() = m_skins[nWeaponIndex].nFallbackSeed;
	*pWeapon->GetFallbackStatTrak() = m_skins[nWeaponIndex].nFallbackStatTrak;
	*pWeapon->GetFallbackWear() = m_skins[nWeaponIndex].flFallbackWear;
	if (m_skins[nWeaponIndex].nItemDefinitionIndex)
		*pWeapon->GetItemDefinitionIndex() = m_skins[nWeaponIndex].nItemDefinitionIndex;
	if (m_skins[nWeaponIndex].szWeaponName) {
		sprintf_s(pWeapon->GetCustomName(), 32, "%s", m_skins[nWeaponIndex].szWeaponName);
	}
	*pWeapon->GetItemIDHigh() = -1;
	return true;
}
bool SkinChanger::apply_viewmodel(IClientEntity* pLocal, CBaseAttributableItem* pWeapon, int nWeaponIndex)
{
	CBaseViewModel* pViewModel = (CBaseViewModel*)interfaces::ent_list->GetClientEntityFromHandle(*(HANDLE*)((DWORD)pLocal + 0x32F8));
	if (!pViewModel)
		return false;
	DWORD hViewModelWeapon = pViewModel->GetWeapon();
	CBaseAttributableItem* pViewModelWeapon = (CBaseAttributableItem*)interfaces::ent_list->GetClientEntityFromHandle((HANDLE)hViewModelWeapon);
	if (pViewModelWeapon != pWeapon)
		return false;

	int nViewModelIndex = pViewModel->GetModelIndex();

	if (g_ViewModelCFG.find(nViewModelIndex) == g_ViewModelCFG.end())
		return false;
	pViewModel->SetWeaponModel(g_ViewModelCFG[nViewModelIndex], pWeapon);

	return true;
}
bool SkinChanger::apply_killcon(IGameEvent* pEvent)
{
	int nUserID = pEvent->GetInt("attacker");
	if (!nUserID)
		return false;
	if (interfaces::engine->GetPlayerForUserID(nUserID) != interfaces::engine->get_localplayer())
		return false;
	bool head = pEvent->GetBool("headshot");
	bool hitgroup = pEvent->GetInt("hitgroup");
	const char* szWeapon = pEvent->GetString("weapon");
	for (auto ReplacementIcon : g_KillIconCfg)
	{
		if (!strcmp(szWeapon, ReplacementIcon.first))
		{
			pEvent->SetString("weapon", ReplacementIcon.second);
			break;
		}
	}
	return true;
}

// reworked

void knifeidx(int knife, char * con)
{
	switch (iKnife)
	{
	case 1:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
		con = "bayonet";
	}
	break;

	case 2:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
		con = "knife_m9_bayonet";
	}
	break;

	case 3:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
		con = "knife_butterfly";
	}
	break;

	case 4:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_flip.mdl");
		con = "knife_flip";
	}
	break;

	case 5:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_gut.mdl");
		con = "knife_gut";
	}
	break;

	case 6:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_karam.mdl");
		con = "knife_karambit";
	}
	break;

	case 7:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_tactical.mdl");
		con = "knife_tactical";
	}
	break;

	case 8:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
		con = "knife_falchion";
	}
	break;

	case 9:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
		con = "knife_survival_bowie";
	}
	break;

	case 10:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_push.mdl");
		con = "knife_push";
	}
	break;

	case 11:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
		con = "knife_widowmaker";
	}
	break;

	case 12:
	{
		knife = interfaces::model_info->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
		con = "knife_stiletto";
	}
	break;
	}
}

#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
void SkinChanger::_do(IClientEntity * local)
{
	if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame() && local->valid_entity()) {
		auto weapons = local->m_hMyWeapons();
		if (!weapons)
			return;
		for (size_t i = 0; weapons[i] != INVALID_EHANDLE_INDEX; i++)
		{
			if (weapons[i]) {
				IClientEntity* e = (IClientEntity*)interfaces::ent_list->GetClientEntityFromHandle(weapons);
				if (!e)
					continue;
				C_BaseCombatWeapon* worldmodle;
				C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)e;
				HANDLE h_worldmodel = weapon->m_hWeaponWorldModel();
				if (h_worldmodel)
					worldmodle = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(weapon->m_hWeaponWorldModel());

				int selected; char * con; const char * name = "mirror gang";
				knifeidx(selected, con);

				*weapon->m_nModelIndex() = selected;
				*weapon->ViewModelIndex() = selected;
				if (worldmodle)
					*worldmodle->m_nModelIndex() = selected + 1;	
				*weapon->fixskins() = 500;
				*weapon->GetEntityQuality() = 3;
				g_KillIconCfg.clear();
				g_KillIconCfg["knife_t"] = con;
				g_KillIconCfg["knife_default_ct"] = con;
				*weapon->FallbackPaintKit() = 0;
				*weapon->FallbackWear() = 0;
				*weapon->szCustomName() = *name;

				*weapon->OwnerXuidLow() = 0;
				*weapon->OwnerXuidHigh() = 0;
			}
		}
	}
}

