#include "ESP.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include <stdarg.h>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <random>
#include <iostream>
#include <iomanip>
#include <random>
#include "RageBot.h"
#include "Autowall.h"
#include <stdio.h>
#include <stdlib.h>
#include <array>
#include <stdlib.h>
#include "position_adjust.h"
#include "Hooks.h"
#include "experimental.h"
#include "RageBot.h"
#include "Resolver.h"
#ifdef NDEBUG
#define strenc( s ) std::string( cx_make_encrypted_string( s ) )
#define charenc( s ) strenc( s ).c_str()
#define wstrenc( s ) std::wstring( strenc( s ).begin(), strenc( s ).end() )
#define wcharenc( s ) wstrenc( s ).c_str()
#else
#define strenc( s ) ( s )
#define charenc( s ) ( s )
#define wstrenc( s ) ( s )
#define wcharenc( s ) ( s )
#endif
#ifdef NDEBUG
#define XorStr( s ) ( XorCompileTime::XorString< sizeof( s ) - 1, __COUNTER__ >( s, std::make_index_sequence< sizeof( s ) - 1>() ).decrypt() )
#else
#define XorStr( s ) ( s )
#endif
float lineLBY;
float lineLBY2;
float lineRealAngle;
float lineFakeAngle;
float lby2;
float lspeed;
float pitchmeme;
CAimbot rageXbot;

float inaccuracy;
void CEsp::Init()
{
	BombCarrier = nullptr;
}
void CEsp::Move(CUserCmd *pCmd, bool &bSendPacket)
{
}
bool screen_transformx(const vector& point, vector& screen)
{
	const matrix3x4& w2sMatrix = interfaces::engine->WorldToScreenMatrix();
	screen.x = w2sMatrix[0][0] * point.x + w2sMatrix[0][1] * point.y + w2sMatrix[0][2] * point.z + w2sMatrix[0][3];
	screen.y = w2sMatrix[1][0] * point.x + w2sMatrix[1][1] * point.y + w2sMatrix[1][2] * point.z + w2sMatrix[1][3];
	screen.z = 0.0f;
	float w = w2sMatrix[3][0] * point.x + w2sMatrix[3][1] * point.y + w2sMatrix[3][2] * point.z + w2sMatrix[3][3];
	if (w < 0.001f) {
		screen.x *= 100000;
		screen.y *= 100000;
		return true;
	}
	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;
	return false;
}
bool world_to_screenx(const vector &origin, vector &screen)
{
	if (!screen_transformx(origin, screen)) {
		int iScreenWidth, iScreenHeight;
		interfaces::engine->GetScreenSize(iScreenWidth, iScreenHeight);
		screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
		screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;
		return true;
	}
	return false;
}
void CEsp::Draw()
{
	if (!interfaces::engine->IsConnected() || !interfaces::engine->IsInGame())
		return;

	IClientEntity *pLocal = hackManager.plocal();

	for (int i = 0; i < interfaces::ent_list->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = interfaces::ent_list->get_client_entity(i);
		player_info_t pinfo;
		if (pEntity &&  pEntity != pLocal && !pEntity->IsDormant())
		{
			if (options::menu.visuals.OtherRadar.getstate())
			{
				DWORD m_bSpotted = NetVar.GetNetVar(0x839EB159);
				*(char*)((DWORD)(pEntity)+m_bSpotted) = 1;
			}
			if (options::menu.visuals.show_players.getstate() && interfaces::engine->GetPlayerInfo(i, &pinfo) && pEntity->IsAlive())
			{
				DrawPlayer(pEntity, pinfo);
			}
		
			ClientClass* cClass = (ClientClass*)pEntity->GetClientClass();
			if (options::menu.visuals.FiltersNades.getstate() && strstr(cClass->m_pNetworkName, "Projectile"))
			{
				DrawThrowable(pEntity);
			}
			if ((options::menu.visuals.WeaponFilterName.getstate() || options::menu.visuals.FiltersWeapons.getindex() != 0) && cClass->m_ClassID != (int)CSGOClassID::CBaseWeaponWorldModel && ((strstr(cClass->m_pNetworkName, "Weapon") || cClass->m_ClassID == (int)CSGOClassID::CDEagle || cClass->m_ClassID == (int)CSGOClassID::CAK47)))
			{
				DrawDrop(pEntity, cClass);
			}
			if (options::menu.visuals.FiltersC4.getstate())
			{
				if (cClass->m_ClassID == (int)CSGOClassID::CPlantedC4)
					DrawBombus(pEntity);
				if (cClass->m_ClassID == (int)CSGOClassID::CC4)
					DrawBomb(pEntity, cClass);
			}
		}
	}

}
float damage;
char bombdamagestringdead[24];
char bombdamagestringalive[24];
inline float CSGO_Armor(float flDamage, int ArmorValue)
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;
		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}
		flDamage = flNew;
	}
	return flDamage;
}
void CEsp::DrawBombus(IClientEntity* pEntity) {
	BombCarrier = nullptr;
	auto entity = pEntity;
	vector vOrig; vector vScreen;
	vOrig = entity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)entity;
	float flBlow = Bomb->GetC4BlowTime();
	auto local = hackManager.plocal();
	float lifetime = flBlow - (interfaces::globals->interval_per_tick * local->GetTickBase());
	int width = 0;
	int height = 0;
	interfaces::engine->GetScreenSize(width, height);
	if (world_to_screenx(vOrig, vScreen))
	{
		Render::nonamegey(vScreen.x, vScreen.y, "C4", Render::Fonts::esp, Color(255, 255, 255));
	}
	if (interfaces::engine->IsConnected() && interfaces::engine->IsInGame())
	{
		if (lifetime > 0.01f && !Bomb->IsBombDefused())
		{
			int boomval = (lifetime * 300) / 40;
			Render::gradient_horizontal(0, 0, 12, boomval, Color(options::menu.ColorsTab.bomb_timer.GetValue()), Color(10, 10, 10, 55));
			Render::textT(0, boomval, Render::Fonts::esp, Color(255, 255, 255), "%.1f", lifetime);
			Render::textT(5, height / 2 - 140, Render::Fonts::niggerbomb, Color(250, 10, 50, 255), "%.1fs", lifetime);
		}

		float flDistance = local->GetEyePosition().DistTo(entity->GetEyePosition());
		float a = 450.7f;
		float b = 75.68f;
		float c = 789.2f;
		float d = ((flDistance - b) / c);
		float flDamage = a * exp(-d * d);
		damage = float((std::max)((int)ceilf(CSGO_Armor(flDamage, local->ArmorValue())), 0));
		sprintf_s(bombdamagestringdead, sizeof(bombdamagestringdead) - 1, "");
		if (lifetime > 0.01f && !Bomb->IsBombDefused() && local->IsAlive())
		{
			if (damage >= local->GetHealth() && lifetime > 0.01f)
			{
				Render::textT(5, height / 2 - 120, Render::Fonts::niggerbomb, Color(255, 10, 10, 255), "FATAL");
			}
			else {
				if (lifetime > 0.01f && local->IsAlive()) {
					std::string gey;
					gey += "-";
					gey += std::to_string((int)(damage));
					gey += "HP";
					Render::textT(5, height / 2 - 120, Render::Fonts::niggerbomb, Color(216, 215, 164, 255), gey.c_str());
				}
			}
		}

		if (Bomb->GetBombDefuser() > 0)
		{
			//IClientEntity *pDefuser = Interfaces::EntList->GetClientEntity(Bomb->GetBombDefuser());
			float countdown = Bomb->GetC4DefuseCountDown() - (local->GetTickBase() * interfaces::globals->interval_per_tick);
			//float maxdefuse = pDefuser->HasDefuser() ? 5.0f : 10.f;
			if (countdown > 0.01f)
			{
				if (lifetime > countdown)
				{
					Render::textT(5, height / 2 - 100, Render::Fonts::niggerbomb, Color(0, 140, 255, 255), "Defuse: %.1f", countdown);
				}
			}
		}
	}
}

bool World2Screen(const vector &origin, vector &screen)
{
	if (!screen_transformx(origin, screen)) {
		int iScreenWidth, iScreenHeight;
		interfaces::engine->GetScreenSize(iScreenWidth, iScreenHeight);
		screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
		screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;
		return true;
	}
	return false;
}
bool isOnScreen(vector origin, vector &screen)
{
	if (!World2Screen(origin, screen)) return false;
	int iScreenWidth, iScreenHeight;
	interfaces::engine->GetScreenSize(iScreenWidth, iScreenHeight);
	bool xOk = iScreenWidth > screen.x > 0, yOk = iScreenHeight > screen.y > 0;
	return xOk && yOk;
}
vector CalcAngle_vec(vector src, vector dst)
{
	vector ret;
	VectorAngles(dst - src, ret);
	return ret;
}

void CEsp::DrawWeapon(IClientEntity* pEntity, Box size)
{
	IClientEntity* pWeapon = interfaces::ent_list->GetClientEntityFromHandle((HANDLE)pEntity->GetActiveWeaponHandle());
	if (pWeapon)
	{
		if (options::menu.visuals.OptionsHealth.getindex() > 2 || options::menu.visuals.OptionsArmor.getindex() > 2)
		{
			RECT nameSize = Render::GetTextSize(Render::Fonts::esp, pWeapon->GetWeaponName());
			Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y + size.h + 13,
				Color(pEntity->team() == hackManager.plocal()->team() ? options::menu.ColorsTab.Weaponsteam.GetValue() : options::menu.ColorsTab.Weapons.GetValue()), Render::Fonts::esp, pWeapon->GetWeaponName());
		}

		else
		{
			RECT nameSize = Render::GetTextSize(Render::Fonts::esp, pWeapon->GetWeaponName());
			Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y + size.h + 5,
				Color(pEntity->team() == hackManager.plocal()->team() ? options::menu.ColorsTab.Weaponsteam.GetValue() : options::menu.ColorsTab.Weapons.GetValue()), Render::Fonts::esp, pWeapon->GetWeaponName());
		}
	}
}

void CEsp::DrawAmmo(IClientEntity* pEntity, Box size)
{
	C_BaseCombatWeapon* pWeapon = pEntity->weapon();
	IClientEntity* pLocal = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (!pEntity || !pLocal || !pLocal->IsAlive())
		return;

	if (!pWeapon)
		return;

	if (pWeapon->isZeus27() || pWeapon->IsMiscGAY())
		return;

	CSWeaponInfo* weapInfo = pWeapon->GetCSWpnData();

	if (weapInfo == nullptr)
		return;

	Color arc = Color(options::menu.ColorsTab.Ammo.GetValue());
	int ammoyes = pWeapon->GetAmmoInClip() * (size.w) / weapInfo->max_clip;
	Render::outlineyeti(size.x - 1, size.y + size.h + 1, size.w + 2, 4, Color(21, 21, 21, 255));
	Render::rect(size.x, size.y + size.h + 2, size.w, 2, Color(51, 51, 51, 255));
	Render::rect(size.x, size.y + size.h + 2, ammoyes, 2, arc);
}

void CEsp::ammo_text(IClientEntity* pEntity, Box size)
{
	C_BaseCombatWeapon* pWeapon = pEntity->weapon();
	IClientEntity* pLocal = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (!pEntity || !pLocal || !pLocal->IsAlive())
		return;

	if (!pWeapon)
		return;

	if (pWeapon->isZeus27())
		return;

	bool text_side = options::menu.visuals.OptionsName.getindex() > 1;
	CSWeaponInfo* weapInfo = pWeapon->GetCSWpnData();

	if (weapInfo == nullptr)
		return;

	Color arc = Color(options::menu.ColorsTab.Ammo.GetValue());

	vector vecOrigin = pEntity->GetOrigin();
	vector vecOriginLocal = pLocal->GetOrigin();
	float dist = DistanceTo(vecOriginLocal, vecOrigin) / 3;


	if (pWeapon->IsGrenade() && !pWeapon->IsSmoke())
	{
		static RECT Size = Render::GetTextSize(Render::Fonts::esp, "Hi");
		char ammoBuffer[512];
		sprintf_s(ammoBuffer, "muslim", 1);
		Render::Text(size.x + size.w + 5, (size.y + 14) + (1 * Size.bottom) - dist, Color(255, 50 + rand() % 100, 0, 255), Render::Fonts::esp, ammoBuffer);
	}
	else
	{
		static RECT Size = Render::GetTextSize(Render::Fonts::esp, "Hi");
		char ammoBuffer[512];
		sprintf_s(ammoBuffer, "%d bullets", pWeapon->GetAmmoInClip());
		Render::Text(size.x + size.w + 5, (size.y + 14) + (1 * Size.bottom) - dist, Color(255, 255, 255, 255), Render::Fonts::esp, ammoBuffer);
	}
}

bool CEsp::fake_ducking(IClientEntity* entity) {


	auto storedTick = 0;
	int crouchedTicks[64];
	auto duckamount = entity->m_flDuckAmount();
	if (!duckamount) return false;
	auto duckspeed = entity->m_flDuckSpeed();
	if (!duckspeed) return false;

	if (storedTick != interfaces::globals->tickcount) {
		crouchedTicks[entity->GetIndex()]++;
		storedTick = interfaces::globals->tickcount;
	}

	return duckspeed == 8 && duckamount <= 0.9 && duckamount > 0.01 && (entity->GetFlags() & FL_ONGROUND) && (
		crouchedTicks[entity->GetIndex()] >= 5);
}

void CEsp::fakeduck_info(IClientEntity * player, Box size)
{
	if (!player)
		return;

	vector vecOrigin = player->GetOrigin();
	vector vecOriginLocal = hackManager.plocal()->GetOrigin();
	float dist = DistanceTo(vecOriginLocal, vecOrigin) / 3;


	static RECT Size = Render::GetTextSize(Render::Fonts::esp, "Hi");
	char ammoBuffer[512];
	sprintf_s(ammoBuffer, "fake duck (?)", 1);
	Render::Text(size.x + size.w + 5, (size.y + 21) + (1 * Size.bottom) - dist, Color(255, 10, 190, 255), Render::Fonts::esp, ammoBuffer);
}

void CEsp::bomb_info(IClientEntity * player, Box size)
{
	//no muslim btw
	if (!player)
		return;

	vector vecOrigin = player->GetOrigin();
	vector vecOriginLocal = hackManager.plocal()->GetOrigin();
	float dist = DistanceTo(vecOriginLocal, vecOrigin) / 3;


	static RECT Size = Render::GetTextSize(Render::Fonts::esp, "Hi");
	char ammoBuffer[512];
	sprintf_s(ammoBuffer, "bomb", 1);
	Render::Text(size.x + size.w + 5, (size.y + 28) + (1 * Size.bottom) - dist, Color(255, 255, 255, 255), Render::Fonts::esp, ammoBuffer);
}

auto resolver_flag_to_string_x = [](int stage) -> std::string // because visual studio is a nigger
{
	switch (stage)
	{
	case 0:
		return "DEFAULT";
	case 1:
		return "LBY RELATIVE";
	case 2:
		return "DESYNC | LBY";
	case 3:
		return "DESYNC | RELATIVE";
	case 4:
		return "STATIC DESYNC";
	case 5:
		return "SLOW WALK (?)";
	case 6:
		return "LBY";
	}
};

void CEsp::debug_text(IClientEntity* pEntity, Box size)
{
	char buff[512];
	vector vecOrigin = pEntity->GetOrigin();
	vector vecOriginLocal = hackManager.plocal()->GetOrigin();
	float dist = DistanceTo(vecOriginLocal, vecOrigin) / 3;

	static RECT Size = Render::GetTextSize(Render::Fonts::esp, "Hi");
	//	Render::Text(size.x + size.w + 5, (size.y + (40)) + (1 * Size.bottom) - dist, Color(255, 255, 255, 255), Render::Fonts::esp, resolver_flag_to_string_x(a_c->resolver_flag[pEntity->GetIndex()]).c_str());

}



void CEsp::DrawPlayer(IClientEntity* pEntity, player_info_t pinfo)
{
	Box box;
	ESPBox esp_box;

//	Vector max = pEntity->GetCollideable()->OBBMaxs();

	vector pos, pos3D;
	vector top, top3D;
	pos3D = pEntity->GetOrigin();
//	top3D = pos3D + Vector(0, 0, max.z);

	auto local_player = interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());

	if (!local_player)
		return;

	bool teammate = pEntity->team() == local_player->team();

	if (!game_utils::World2Screen(pos3D, pos)) //|| !game_utils::World2Screen(top3D, top))
		return;

	if (get_box(pEntity, box, options::menu.visuals.Active.getstate()) && (!teammate))
	{

		if (options::menu.visuals.OptionsBox.getindex() != 0)
		{
			switch (options::menu.visuals.OptionsBox.getindex())
			{
			case 1: default_box(box, options::menu.ColorsTab.BoxCol.GetValue());
				break;
			case 2: bracket_box(box, options::menu.ColorsTab.BoxCol.GetValue());
				break;
			case 3: corner_box(box, options::menu.ColorsTab.BoxCol.GetValue(), pEntity);
				break;

			}
		}

		if (options::menu.visuals.OptionsName.getindex() != 0)
		{
			switch (options::menu.visuals.OptionsName.getindex())
			{
			case 1: DrawName(pinfo, box, pEntity);
				break;
			case 2: name_side(pinfo, box, pEntity);
				break;
			}
		}

		if (options::menu.visuals.OptionsHealth.getindex() != 0)
		{
			switch (options::menu.visuals.OptionsHealth.getindex())
			{
			case 1: hp_default(pEntity, box);
				break;
			case 2: hp_battery(pEntity, box);
				break;
			case 3: hp_bottom(pEntity, box);
				break;
			}
		}

		if (options::menu.visuals.OptionsInfo.getstate())
		{
			hp_text(pEntity, box);
			ar_text(pEntity, box);
			ammo_text(pEntity, box);

			if (fake_ducking(pEntity)) // ooo fak 
				fakeduck_info(pEntity, box);
		}


		switch (options::menu.visuals.OptionsArmor.getindex())
		{
		case 1: armor_default(pEntity, box);
			break;
		case 2: armor_battery(pEntity, box);
			break;
		case 3: armor_bottom(pEntity, box);
			break;
		}
		

		if (options::menu.visuals.OptionsInfo.getstate() || options::menu.visuals.OptionsWeapone.getstate())
		{
			DrawInfo(pEntity, box);
			DrawInfo2(pEntity, box);

			if (BombCarrier == pEntity)
				bomb_info(pEntity, box);
		}

		if (options::menu.visuals.OptionsSkeleton.getstate()) {
			auto player_index = pEntity->GetIndex();
				
		//	DrawSkeleton(pEntity, hdfdunn[64][12];
		}

		if (options::menu.visuals.Weapons.getstate()) {
			DrawWeapon(pEntity, box);
		}

		if (options::menu.visuals.Ammo.getstate())
		{
			DrawAmmo(pEntity, box);
		}

		if (options::menu.visuals.debug_esp.getstate())
		{
			debug_text(pEntity, box);
		}


	}

	if (options::menu.visuals.show_team.getstate())
	{
		if (get_box(pEntity, box, options::menu.visuals.Active.getstate()) && (teammate))
		{
			if (options::menu.visuals.OptionsBox.getindex() != 0)
			{
				switch (options::menu.visuals.OptionsBox.getindex())
				{
				case 1: default_box(box, options::menu.ColorsTab.BoxCol.GetValue());
					break;
				case 2: bracket_box(box, options::menu.ColorsTab.BoxCol.GetValue());
					break;
				case 3: corner_box(box, options::menu.ColorsTab.BoxCol.GetValue(), pEntity);
					break;

				}
			}

			if (options::menu.visuals.OptionsName.getindex() != 0)
			{
				switch (options::menu.visuals.OptionsName.getindex())
				{
				case 1: DrawName(pinfo, box, pEntity);
					break;
				case 2: name_side(pinfo, box, pEntity);
					break;
				}
			}

			if (options::menu.visuals.OptionsHealth.getindex() != 0)
			{
				switch (options::menu.visuals.OptionsHealth.getindex())
				{
				case 1: hp_default(pEntity, box);
					break;
				case 2: hp_battery(pEntity, box);
					break;
				case 3: hp_bottom(pEntity, box);
					break;
				}

				if (options::menu.visuals.OptionsInfo.getstate())
				{
					hp_text(pEntity, box);
				}
			}

			if (options::menu.visuals.OptionsArmor.getindex() != 0)
			{
				switch (options::menu.visuals.OptionsArmor.getindex())
				{
				case 1: armor_default(pEntity, box);
					break;
				case 2: armor_battery(pEntity, box);
					break;
				case 3: armor_bottom(pEntity, box);
					break;
				}

				if (options::menu.visuals.OptionsInfo.getstate())
				{
					ar_text(pEntity, box);
				}
			}

			if (options::menu.visuals.OptionsInfo.getstate() || options::menu.visuals.OptionsWeapone.getstate())
			{
				DrawInfo(pEntity, box);
				DrawInfo2(pEntity, box);
			}

			if (options::menu.visuals.OptionsSkeleton.getstate())
				DrawSkeleton_team(pEntity);

			if (options::menu.visuals.Weapons.getstate()) {
				DrawWeapon(pEntity, box);
			}

			if (options::menu.visuals.Ammo.getstate())
			{
				DrawAmmo(pEntity, box);

				if (options::menu.visuals.OptionsInfo.getstate())
				{
					ammo_text(pEntity, box);
				}
			}
		}
	}

}

float dot_product_t(const float* a, const float* b) {
	return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}
#define rad_pi 57.295779513082f
#define pi 3.14159265358979323846f
#define rad(a) a * 0.01745329251
#define deg(a) a * 57.295779513082
float degrees_to_radians(const float deg)
{
	return deg * (pi / 180.f);
}
float radians_to_degrees(const float rad)
{
	return rad * rad_pi;
}

void rotate_triangle(std::array<Vector2D, 3>& points, float rotation)
{
	const auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
	for (auto& point : points)
	{
		point -= points_center;
		const auto temp_x = point.x;
		const auto temp_y = point.y;
		const auto theta = degrees_to_radians(rotation);
		const auto c = cosf(theta);
		const auto s = sinf(theta);
		point.x = temp_x * c - temp_y * s;
		point.y = temp_x * s + temp_y * c;
		point += points_center;
	}
}
template<class T, class U>
inline T clampx(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}
void VectorAngles(vector &forward, QAngle &angles)
{
	Assert(s_bMathlibInitialized);
	float	tmp, yaw, pitch;
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / pi);
		if (yaw < 0)
			yaw += 360;
		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / pi);
		if (pitch < 0)
			pitch += 360;
	}
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void VectorAnglesXX(const vector& forward, vector &angles)
{
	float tmp, yaw, pitch;
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / PI);
		if (yaw < 0)
			yaw += 360;
		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / PI);
		if (pitch < 0)
			pitch += 360;
	}
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}
vector CalcAngleXX(vector src, vector dst)
{
	vector ret;
	VectorAnglesXX(dst - src, ret);
	return ret;
}
template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}
void vector_transform_a(const float *in1, const matrix3x4& in2, float *out) {
	out[0] = dot_product_t(in1, in2[0]) + in2[0][3];
	out[1] = dot_product_t(in1, in2[1]) + in2[1][3];
	out[2] = dot_product_t(in1, in2[2]) + in2[2][3];
}
inline void vector_transform_z(const vector& in1, const matrix3x4 &in2, vector &out) {
	vector_transform_a(&in1.x, in2, &out.x);
}
bool CEsp::get_box(IClientEntity* m_entity, Box& box, bool dynamic) {
	DWORD m_rgflCoordinateFrame = (DWORD)0x444; //(DWORD)0x470 - 0x30
	const matrix3x4& trnsf = *(matrix3x4*)((DWORD)m_entity + (DWORD)m_rgflCoordinateFrame);
	vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;
	vOrigin = m_entity->GetOrigin();
	min = m_entity->collisionProperty()->GetMins();
	max = m_entity->collisionProperty()->GetMaxs();
	if (!dynamic) {
		min += vOrigin;
		max += vOrigin;
	}
	vector points[] = { vector(min.x, min.y, min.z),
		vector(min.x, max.y, min.z),
		vector(max.x, max.y, min.z),
		vector(max.x, min.y, min.z),
		vector(max.x, max.y, max.z),
		vector(min.x, max.y, max.z),
		vector(min.x, min.y, max.z),
		vector(max.x, min.y, max.z) };
	vector vector_transformed[8];
	if (dynamic)
	{
		for (int i = 0; i < 8; i++)
		{
			vector_transform_z(points[i], trnsf, vector_transformed[i]);
			points[i] = vector_transformed[i];
		}
	}
	if (!Render::TransformScreen(points[3], flb) || !Render::TransformScreen(points[5], brt)
		|| !Render::TransformScreen(points[0], blb) || !Render::TransformScreen(points[4], frt)
		|| !Render::TransformScreen(points[2], frb) || !Render::TransformScreen(points[1], brb)
		|| !Render::TransformScreen(points[6], blt) || !Render::TransformScreen(points[7], flt))
		return false;
	vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;
	for (int i = 1; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}
	box.x = left;
	box.y = top;
	box.w = right - left;
	box.h = bottom - top;
	return true;
}
bool CEsp::GetBox(IClientEntity* pEntity, CEsp::ESPBox &result)
{
	vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;
	vOrigin = pEntity->GetOrigin();
	min = pEntity->collisionProperty()->GetMins() + vOrigin;
	max = pEntity->collisionProperty()->GetMaxs() + vOrigin;
	vector points[] = { vector(min.x, min.y, min.z),
		vector(min.x, max.y, min.z),
		vector(max.x, max.y, min.z),
		vector(max.x, min.y, min.z),
		vector(max.x, max.y, max.z),
		vector(min.x, max.y, max.z),
		vector(min.x, min.y, max.z),
		vector(max.x, min.y, max.z) };
	if (!Render::TransformScreen(points[3], flb) || !Render::TransformScreen(points[5], brt)
		|| !Render::TransformScreen(points[0], blb) || !Render::TransformScreen(points[4], frt)
		|| !Render::TransformScreen(points[2], frb) || !Render::TransformScreen(points[1], brb)
		|| !Render::TransformScreen(points[6], blt) || !Render::TransformScreen(points[7], flt))
		return false;
	vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;
	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}
	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;
	return true;
}
Color CEsp::GetPlayerColor(IClientEntity* pEntity)
{
	int TeamNum = pEntity->team();
	bool IsVis = game_utils::is_visible(hackManager.plocal(), pEntity, (int)csgo_hitboxes::head);
	Color color;
	if (TeamNum == TEAM_CS_T)
	{
	}
	else
	{
	}
	return color;
}
std::string CleanItemName(std::string name)
{
	std::string Name = name;
	if (Name[0] == 'C')
		Name.erase(Name.begin());
	auto startOfWeap = Name.find("Weapon");
	if (startOfWeap != std::string::npos)
		Name.erase(Name.begin() + startOfWeap, Name.begin() + startOfWeap + 6);
	return Name;
}
void CEsp::DrawGun(IClientEntity* pEntity, CEsp::ESPBox size)
{
	IClientEntity* pWeapon = interfaces::ent_list->GetClientEntityFromHandle((HANDLE)pEntity->GetActiveWeaponHandle());
	ClientClass* cClass = (ClientClass*)pWeapon->GetClientClass();
	if (cClass)
	{
		std::string meme = CleanItemName(cClass->m_pNetworkName);
		RECT nameSize = Render::GetTextSize(Render::Fonts::esp, meme.c_str());
		Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y + size.h + 1,
			Color(255, 255, 255, 255), Render::Fonts::esp, meme.c_str());
	}
}
void CEsp::corner_box(Box size, Color color, IClientEntity* pEntity)
{
	int VertLine = (((float)size.w) * (0.20f));
	int HorzLine = (((float)size.h) * (0.30f));
	Render::Clear(size.x, size.y - 1, VertLine, 1, Color(0, 0, 0, 255));
	Render::Clear(size.x + size.w - VertLine, size.y - 1, VertLine, 1, Color(0, 0, 0, 255));
	Render::Clear(size.x, size.y + size.h - 1, VertLine, 1, Color(0, 0, 0, 255));
	Render::Clear(size.x + size.w - VertLine, size.y + size.h - 1, VertLine, 1, Color(0, 0, 0, 255));
	Render::Clear(size.x - 1, size.y, 1, HorzLine, Color(0, 0, 0, 255));
	Render::Clear(size.x - 1, size.y + size.h - HorzLine, 1, HorzLine, Color(0, 0, 0, 255));
	Render::Clear(size.x + size.w - 1, size.y, 1, HorzLine, Color(0, 0, 0, 255));
	Render::Clear(size.x + size.w - 1, size.y + size.h - HorzLine, 1, HorzLine, Color(0, 0, 0, 255));
	Render::Clear(size.x, size.y, VertLine, 1, color);
	Render::Clear(size.x + size.w - VertLine, size.y, VertLine, 1, color);
	Render::Clear(size.x, size.y + size.h, VertLine, 1, color);
	Render::Clear(size.x + size.w - VertLine, size.y + size.h, VertLine, 1, color);
	Render::Clear(size.x, size.y, 1, HorzLine, color);
	Render::Clear(size.x, size.y + size.h - HorzLine, 1, HorzLine, color);
	Render::Clear(size.x + size.w, size.y, 1, HorzLine, color);
	Render::Clear(size.x + size.w, size.y + size.h - HorzLine, 1, HorzLine, color);
}
void CEsp::FilledBox(CEsp::ESPBox size, Color color)
{
	int VertLine = (((float)size.w) * (0.20f));
	int HorzLine = (((float)size.h) * (0.20f));
	Render::Clear(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(0, 0, 0, 40));
	Render::Clear(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(0, 0, 0, 40));
	Render::Clear(size.x, size.y, VertLine, 1, color);
	Render::Clear(size.x + size.w - VertLine, size.y, VertLine, 1, color);
	Render::Clear(size.x, size.y + size.h, VertLine, 1, color);
	Render::Clear(size.x + size.w - VertLine, size.y + size.h, VertLine, 1, color);
	Render::Clear(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(0, 0, 0, 40));
	Render::Clear(size.x, size.y, 1, HorzLine, color);
	Render::Clear(size.x, size.y + size.h - HorzLine, 1, HorzLine, color);
	Render::Clear(size.x + size.w, size.y, 1, HorzLine, color);
	Render::Clear(size.x + size.w, size.y + size.h - HorzLine, 1, HorzLine, color);
	Render::Clear(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(0, 0, 0, 40));
	CEsp::ESPBox box = size;
	Render::Outline(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(21, 21, 21, 150));
	Render::Outline(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(21, 21, 21, 150));
}
void CEsp::default_box(Box box, Color color)
{
	Render::Outline(box.x, box.y, box.w, box.h, color);
	Render::Outline(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(21, 21, 21, 150));
	Render::Outline(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(21, 21, 21, 150));
	Render::Outline(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(21, 21, 21, 150));
	Render::Outline(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(21, 21, 21, 150));
}
void CEsp::bracket_box(Box size, Color color)
{
	int VertLine = (((float)size.w) * (0.30f));
	int HorzLine = (((float)size.h) * (1.00f));
	Render::Clear(size.x, size.y, VertLine, 1.2, color);
	Render::Clear(size.x + size.w - VertLine, size.y, VertLine, 1.2, color);
	Render::Clear(size.x, size.y + size.h, VertLine, 1.2, color);
	Render::Clear(size.x + size.w - VertLine, size.y + size.h, VertLine, 1.2, color);
	Render::Clear(size.x, size.y, 1, HorzLine, color);
	Render::Clear(size.x, size.y + size.h - HorzLine, 1, HorzLine, color);
	Render::Clear(size.x + size.w, size.y, 1, HorzLine, color);
	Render::Clear(size.x + size.w, size.y + size.h - HorzLine, 1, HorzLine, color);
	Box box = size;
	Render::Outline(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(21, 21, 21, 150));
	Render::Outline(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(21, 21, 21, 150));
}
static wchar_t* CharToWideChar(const char* text)
{
	size_t size = strlen(text) + 1;
	wchar_t* wa = new wchar_t[size];
	mbstowcs_s(NULL, wa, size / 4, text, size);
	return wa;
}
//converting like that will fuck the unicode characters lmao who made that
void CEsp::DrawName(player_info_t pinfo, Box size, IClientEntity* pEntity)
{
	if (!pEntity || !pEntity->IsAlive() || !hackManager.plocal())
		return;
	wchar_t buffer[36];
	auto name = pinfo.name;
	if (MultiByteToWideChar(CP_UTF8, 0, pinfo.name, -1, buffer, 36) > 0)
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::esp, pinfo.name);
		Render::TEXTUNICODE(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 11, pinfo.name, Render::Fonts::esp, Color(options::menu.ColorsTab.NameCol.GetValue()));
	}
}

void CEsp::hp_battery(IClientEntity* pEntity, Box size)
{
	Box HealthBar = size;
	HealthBar.y += (HealthBar.h + 6);
	HealthBar.h = 4;

	float HealthValue = pEntity->GetHealth();
	float HealthPerc = HealthValue / 100.f;
	float Width = (size.w * HealthPerc);
	HealthBar.w = Width;

	// --  Main Bar -- //

	float flBoxes = std::ceil(pEntity->GetHealth() / 10);
	float flX = size.x - 7; float flY = size.y - 1;
	float flHeight = size.h / 10.f;
	float flHeight2 = size.h / 10.f;
	float flMultiplier = 10 / 360.f; flMultiplier *= flBoxes - 1;

	int rectHeight = flHeight * flBoxes + 1;

	Render::DrawRect(flX, flY, 4, size.h + 2, Color(60, 60, 60, 255));
	Render::Outline(flX, flY, 4, size.h + 2, Color(0, 0, 0, 255));

	Render::DrawRect(flX + 1, flY + size.h + (flMultiplier - rectHeight) + 1, 2, rectHeight, pEntity->IsDormant() ? Color(0, 0, 0, 140) : Color(options::menu.visuals.health_col.GetValue()[0], options::menu.visuals.health_col.GetValue()[1], options::menu.visuals.health_col.GetValue()[2], 255));

	for (int i = 0; i < 10; i++) // 
		Render::Line(flX, flY + i * flHeight2, flX + 4, flY + i * flHeight2, Color(0, 0, 0, 255));
}


void CEsp::armor_battery(IClientEntity* pEntity, Box size)
{
	Box ArmorBar = size;
	ArmorBar.y += (ArmorBar.h + 6);
	ArmorBar.h = 4;

	float ArmorValue = pEntity->ArmorValue();
	float ArmorPerc = ArmorValue / 100.f;
	float Width = (size.w * ArmorPerc);
	ArmorBar.w = Width;

	// --  Main Bar -- //

	float flBoxes = std::ceil(pEntity->ArmorValue() / 10);
	float flX = size.x - (options::menu.visuals.OptionsHealth.getindex() > 2 ? 7 : 12); float flY = size.y - 1;
	float flHeight = size.h / 10.f;
	float flHeight2 = size.h / 10.f;
	float flMultiplier = 10 / 360.f; flMultiplier *= flBoxes - 1;

	int rectHeight = flHeight * flBoxes + 1;


	Render::DrawRect(flX, flY, 4, size.h + 2, Color(60, 60, 60, 255));
	Render::Outline(flX, flY, 4, size.h + 2, Color(0, 0, 0, 255));

	Render::DrawRect(flX + 1, flY + size.h + (flMultiplier - rectHeight) + 1, 2, rectHeight, Color(options::menu.visuals.armor_col.GetValue()[0], options::menu.visuals.armor_col.GetValue()[1], options::menu.visuals.armor_col.GetValue()[2], 140));

	for (int i = 0; i < 10; i++) // 
		Render::Line(flX, flY + i * flHeight2, flX + 4, flY + i * flHeight2, Color(0, 0, 0, 255));


}

void CEsp::hp_bottom(IClientEntity* pEntity, Box size)
{
	Color arc = Color(10, 230, 75, 255);
	int hp = pEntity->GetHealth() * (size.w) / 100;

	Render::outlineyeti(size.x - 1, size.y + size.h + 5, size.w + 2, 4, Color(21, 21, 21, 255));
	Render::rect(size.x, size.y + size.h + 6, size.w, 2, Color(51, 51, 51, 255));
	Render::rect(size.x, size.y + size.h + 6, hp, 2, arc);
	//	Render::Text((size.x + 1) + hp, (size.y + 3.5 )+ size.h, Color(255, 255, 255), Render::Fonts::smallassfont, std::to_string(pEntity->GetHealth()).c_str());
}
void CEsp::armor_default(IClientEntity* pEntity, Box size)
{
	Box box = size;
	int player_ar = pEntity->ArmorValue();
	if (player_ar)
	{
		if (player_ar > 100) {
			player_ar = 100;
		}
		int color[3] = { options::menu.visuals.armor_col.GetValue()[0], options::menu.visuals.armor_col.GetValue()[1], options::menu.visuals.armor_col.GetValue()[2] };

		Render::Outline(box.x - 12, box.y - 1, 4, box.h + 2, Color(21, 21, 21, 255));
		int health_height = player_ar * box.h / 100;
		int add_space = box.h - health_height;
		Color hec = Color(color[0], color[1], color[2], 255);
		Render::rect(box.x - 11, box.y, 2, box.h, Color(21, 21, 21, 255));
		Render::rect(box.x - 11, box.y + add_space, 2, health_height, hec);

	}
} // DrawAmmo_vital(pEntity, box)

void CEsp::hp_default(IClientEntity* pEntity, Box size)
{
	Box box = size;
	int player_health = pEntity->GetHealth() > 100 ? 100 : pEntity->GetHealth();
	if (player_health) {

		int color[3] = { 0, 0, 0 };
		color[0] = options::menu.visuals.health_col.GetValue()[0]; color[1] = options::menu.visuals.health_col.GetValue()[1]; color[2] = options::menu.visuals.health_col.GetValue()[2];

		Render::Outline(box.x - 7, box.y - 1, 4, box.h + 2, Color(21, 21, 21, 255));
		int health_height = player_health * box.h / 100;
		int add_space = box.h - health_height;
		Color hec = Color(color[0], color[1], color[2], 255);
		Render::rect(box.x - 6, box.y, 2, box.h, Color(21, 21, 21, 255));
		Render::rect(box.x - 6, box.y + add_space, 2, health_height, hec);

	}
}

float CEsp::DistanceTo(vector vecSrc, vector vecDst)
{
	vector vDelta = vecDst - vecSrc;

	float fDistance = ::sqrtf((vDelta.Length()));

	if (fDistance < 1.0f)
		return 1.0f;

	return fDistance;
}
void CEsp::hp_text(IClientEntity* pEntity, Box size)
{
	vector vecOrigin = pEntity->GetOrigin();
	vector vecOriginLocal = hackManager.plocal()->GetOrigin();

	bool text_side = options::menu.visuals.OptionsName.getindex() > 1;

	float dist = DistanceTo(vecOriginLocal, vecOrigin) / 3;
	static RECT Size = Render::GetTextSize(Render::Fonts::esp, "Hi");
	char HPBuffer[512];
	sprintf_s(HPBuffer, "%d health", pEntity->GetHealth());
	Render::Text(size.x + size.w + 5, (size.y + (text_side ? 1 : 0)) + (1 * Size.bottom) - dist, Color(options::menu.visuals.health_col.GetValue()[0], options::menu.visuals.health_col.GetValue()[1], options::menu.visuals.health_col.GetValue()[2], 255), Render::Fonts::esp, HPBuffer);

}

void CEsp::ar_text(IClientEntity* pEntity, Box size)
{
	vector vecOrigin = pEntity->GetOrigin();
	vector vecOriginLocal = hackManager.plocal()->GetOrigin();

	bool text_side = options::menu.visuals.OptionsName.getindex() > 1;

	float dist = DistanceTo(vecOriginLocal, vecOrigin) / 3;
	static RECT Size = Render::GetTextSize(Render::Fonts::esp, "Hi");
	char armorbuffer[512];
	sprintf_s(armorbuffer, "%d armor", pEntity->ArmorValue());
	Render::Text(size.x + size.w + 5, (size.y + (text_side ? 21.5 : 7)) + (1 * Size.bottom) - dist, Color(options::menu.visuals.armor_col.GetValue()[0], options::menu.visuals.armor_col.GetValue()[1], options::menu.visuals.armor_col.GetValue()[2], 255), Render::Fonts::esp, armorbuffer);

}

void CEsp::name_side(player_info_t pinfo, Box size, IClientEntity* pEntity)
{
	vector vecOrigin = pEntity->GetOrigin();
	vector vecOriginLocal = hackManager.plocal()->GetOrigin();
	auto name = pinfo.name;
	float dist = DistanceTo(vecOriginLocal, vecOrigin) / 3;
	static RECT Size = Render::GetTextSize(Render::Fonts::esp, "Hi");
	wchar_t buffer[36];

	if (MultiByteToWideChar(CP_UTF8, 0, pinfo.name, -1, buffer, 36) > 0)
	{
		Render::TEXTUNICODE(size.x + size.w + 5, size.y + (1 * Size.bottom) - dist, pinfo.name, Render::Fonts::esp, Color(options::menu.ColorsTab.NameCol.GetValue()));
	}
}

void CEsp::armor_bottom(IClientEntity* pEntity, Box size)
{
	Color arc = Color(options::menu.visuals.armor_col.GetValue()[0], options::menu.visuals.armor_col.GetValue()[1], options::menu.visuals.armor_col.GetValue()[2], 255);
	int hp = pEntity->ArmorValue() * (size.w) / 100;

	if (options::menu.visuals.OptionsHealth.getindex() < 3)
	{
		Render::outlineyeti(size.x - 1, size.y + size.h + 5, size.w + 2, 4, Color(21, 21, 21, 255));
		Render::rect(size.x, size.y + size.h + 6, size.w, 2, Color(51, 51, 51, 255));
		Render::rect(size.x, size.y + size.h + 6, hp, 2, arc);
	}
	else
	{
		Render::outlineyeti(size.x - 1, size.y + size.h + 9, size.w + 2, 4, Color(21, 21, 21, 255));
		Render::rect(size.x, size.y + size.h + 10, size.w, 2, Color(51, 51, 51, 255));
		Render::rect(size.x, size.y + size.h + 10, hp, 2, arc);
	}
	//	Render::Text(size.x + hp, (size.y + 7 ) + size.h, Color(255, 255, 255), Render::Fonts::smallassfont, std::to_string(pEntity->GetHealth()).c_str());
}


void CEsp::DrawInfo(IClientEntity* pEntity, Box size)
{
	IClientEntity *g_LocalPlayer = hackManager.plocal();
	if (!g_LocalPlayer)
		return;
	std::vector<std::pair<std::string, Color>> stored_info;
	auto entity = pEntity;
	static float old_simtime[65];

	if (entity->GetFlashDuration() > 0)
		stored_info.push_back(std::pair<std::string, Color>("Flashed", Color(250, 250, 250, 250)));

	int i = 0;
	for (auto Text : stored_info)
	{
		RECT TextSize = Render::GetTextSize(Render::Fonts::esp, Text.first.c_str());
		Render::Text(size.x + size.w + 1, (size.y + 30) + i - ((TextSize.bottom / 2) - 5), Text.second, Render::Fonts::esp, Text.first.c_str());
		i += 8;
	}

}

void CEsp::DrawInfo2(IClientEntity* pEntity, Box size)
{
	RECT defSize = Render::GetTextSize(Render::Fonts::esp, "");
	std::vector<std::string> Info;

	// Player Weapon ESP
	static RECT Size = Render::GetTextSize(Render::Fonts::Default, "Hi");
	RECT named = Render::GetTextSize(Render::Fonts::esp, "Scoped");

	if (options::menu.visuals.OptionsInfo.getstate())
	{
		char hp[50];
		sprintf_s(hp, sizeof(hp), "%i", pEntity->ArmorValue());
		RECT nameSize = Render::GetTextSize(Render::Fonts::esp, hp);

		bool fuck = (options::menu.visuals.OptionsHealth.getindex() > 2 || options::menu.visuals.OptionsArmor.getindex() > 2);
		if (pEntity->ArmorValue() > 0)
		{
			if (pEntity->HasHelmet())
				Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y + (fuck ? size.h + 23 : size.h + 15), Color(255, 255, 255, 255), Render::Fonts::esp, "HK");
			else
				Render::Text(size.x + (size.w / 2) - ((nameSize.right / 2)), size.y + (fuck ? size.h + 23 : size.h + 15), Color(210, 210, 210, 255), Render::Fonts::esp, "Kev");
		}
		
	}
	if (options::menu.visuals.OptionsInfo.getstate())
	{
		if (pEntity->IsScoped())
		{
			Render::Text(size.x + (size.w / 2) - (named.right / 2), size.y - 20, Color(250, 250, 250, 255), Render::Fonts::esp, "Scoped");
		}
	}


	int i = 0;
	for (auto Text : Info)
	{
		Render::Text(size.x + size.w + 3, size.y + (i*(Size.bottom + 2)), Color(255, 255, 255, 255), Render::Fonts::esp, Text.c_str());
		i++;
	}
}


void CEsp::DrawCross(IClientEntity* pEntity)
{
	vector cross = pEntity->GetHeadPos(), screen;
	vec_t Scale = 2;
	if (Render::TransformScreen(cross, screen))
	{
		Render::Clear(screen.x - Scale, screen.y - (Scale * 2), (Scale * 2), (Scale * 4), Color(20, 20, 20, 160));
		Render::Clear(screen.x - (Scale * 2), screen.y - Scale, (Scale * 4), (Scale * 2), Color(20, 20, 20, 160));
		Render::Clear(screen.x - Scale - 1, screen.y - (Scale * 2) - 1, (Scale * 2) - 2, (Scale * 4) - 2, Color(250, 250, 250, 160));
		Render::Clear(screen.x - (Scale * 2) - 1, screen.y - Scale - 1, (Scale * 4) - 2, (Scale * 2) - 2, Color(250, 250, 250, 160));
	}
}
void CEsp::DrawDrop(IClientEntity* pEntity, ClientClass* cClass) //
{
	Color color;
	Box Box;
	IClientEntity* Weapon = (IClientEntity*)pEntity;

	IClientEntity* plr = interfaces::ent_list->GetClientEntityFromHandle((HANDLE)Weapon->GetOwnerHandle());

	if (Weapon && !plr)
	{

		if (get_box(pEntity, Box, options::menu.visuals.WeaponFilterName.getstate()))
		{
			RECT TextSize = Render::GetTextSize(Render::Fonts::nameaiz, Weapon->GetWeaponName());
			Render::Text(Box.x + (Box.w / 2) - (TextSize.right / 2), Box.y + Box.h + 5, Color(255, 255, 255, 255), Render::Fonts::nameaiz, Weapon->GetWeaponName());

		}

		if (get_box(pEntity, Box, options::menu.visuals.FiltersWeapons.getindex() != 0))
		{
			switch (options::menu.visuals.FiltersWeapons.getindex())
			{
			case 1: default_box(Box, Color(255, 255, 255, 255));
				break;
			case 2: bracket_box(Box, Color(255, 255, 255, 255));
				break;
			case 3: corner_box(Box, Color(255, 255, 255, 255), pEntity);
				break;

			}
		}
	}
}

void CEsp::DrawBombPlanted(IClientEntity* pEntity, ClientClass* cClass)
{
	BombCarrier = nullptr;
	vector vOrig; vector vScreen;
	vOrig = pEntity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)pEntity;
	float flBlow = Bomb->GetC4BlowTime();
	float TimeRemaining = flBlow - (interfaces::globals->interval_per_tick * hackManager.plocal()->GetTickBase());
	char buffer[64];
	sprintf_s(buffer, "%.1fs", TimeRemaining);
	float TimeRemaining2;
	bool exploded = true;
	if (TimeRemaining < 0)
	{
		!exploded;
		TimeRemaining2 = 0;
	}
	else
	{
		exploded = true;
		TimeRemaining2 = TimeRemaining;
	}
	if (exploded)
	{
		sprintf_s(buffer, " Bomb: %.1f", TimeRemaining2);
	}
	else
	{
		sprintf_s(buffer, " Bomb Undefusable", TimeRemaining2);
	}
	//Render::Text(10, 45, Color(0, 255, 0, 255), Render::Fonts::Clock, buffer);
}
void CEsp::DrawBomb(IClientEntity* pEntity, ClientClass* cClass)
{
	BombCarrier = nullptr;
	C_BaseCombatWeapon *BombWeapon = (C_BaseCombatWeapon *)pEntity;
	vector vOrig; vector vScreen;
	vOrig = pEntity->GetOrigin();
	bool adopted = true;
	HANDLE parent = BombWeapon->GetOwnerHandle();
	if (parent || (vOrig.x == 0 && vOrig.y == 0 && vOrig.z == 0))
	{
		IClientEntity* pParentEnt = (interfaces::ent_list->GetClientEntityFromHandle(parent));
		if (pParentEnt && pParentEnt->IsAlive())
		{
			BombCarrier = pParentEnt;
			adopted = false;
		}
	}
}

void CEsp::DrawSkeleton(IClientEntity* pEntity, matrix3x4_t *pBoneToWorldOut)
{
	auto model = pEntity->GetModel();
	if (!model) return;

	auto studio_model = interfaces::model_info->GetStudiomodel(pEntity->GetModel());
	if (!studio_model) return;

	for (int i = 0; i < studio_model->numbones; i++)
	{
		auto pBone = studio_model->GetBone(i);

		if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
			continue;

		if (vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]).IsZero() || vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]).IsZero())
			continue;

		vector vBonePos1;
		if (World2Screen(vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
			continue;

		vector vBonePos2;
		if (!World2Screen(vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
			continue;

		Render::Line((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, Color(options::menu.ColorsTab.Skeleton.GetValue()[0], options::menu.ColorsTab.Skeleton.GetValue()[1], options::menu.ColorsTab.Skeleton.GetValue()[2], options::menu.ColorsTab.Skeleton.GetValue()[3]));
	}
}

void CEsp::DrawSkeleton_team(IClientEntity* pEntity)
{
	studiohdr_t* pStudioHdr = interfaces::model_info->GetStudiomodel(pEntity->GetModel());
	if (!pStudioHdr)
		return;
	vector vParent, vChild, sParent, sChild;
	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->GetBone(j);
		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			vChild = pEntity->GetBonePos(j);
			vParent = pEntity->GetBonePos(pBone->parent);
			if (World2Screen(vParent, sParent) && World2Screen(vChild, sChild))
			{
				Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(options::menu.ColorsTab.Skeletonteam.GetValue()[0], options::menu.ColorsTab.Skeletonteam.GetValue()[1], options::menu.ColorsTab.Skeletonteam.GetValue()[2], options::menu.ColorsTab.Skeletonteam.GetValue()[3]));
			}
		}
	}
}

void CEsp::BoxAndText(IClientEntity* entity, std::string text)
{
	Box Box;
	std::vector<std::string> Info;
	RECT nameSize = Render::GetTextSize(Render::Fonts::esp, "");
	if (get_box(entity, Box, options::menu.visuals.FiltersNades.getstate()))
	{
		Info.push_back(text);
		if (options::menu.visuals.FiltersNades.getstate())
		{
			int i = 0;
			for (auto kek : Info)
			{
				default_box(Box, Color(255, 255, 255, 255));
				Render::Text(Box.x + (Box.w / 2) - (nameSize.right / 2), Box.y - 11, Color(255, 255, 255, 255), Render::Fonts::esp, kek.c_str());
				i++;
			}
		}
	}
}
void CEsp::DrawThrowable(IClientEntity* throwable)
{
	model_t* nadeModel = (model_t*)throwable->GetModel();
	if (!nadeModel)
		return;
	studiohdr_t* hdr = interfaces::model_info->GetStudiomodel(nadeModel);
	if (!hdr)
		return;
	if (!strstr(hdr->name, "thrown") && !strstr(hdr->name, "dropped"))
		return;
	std::string nadeName = "Unknown Grenade";
	IMaterial* mats[32];
	interfaces::model_info->GetModelMaterials(nadeModel, hdr->numtextures, mats);
	for (int i = 0; i < hdr->numtextures; i++)
	{
		IMaterial* mat = mats[i];
		if (!mat)
			continue;
		if (strstr(mat->GetName(), "flashbang"))
		{
			nadeName = "flash";
			break;
		}
		else if (strstr(mat->GetName(), "m67_grenade") || strstr(mat->GetName(), "hegrenade"))
		{
			nadeName = "he_grenade";
			break;
		}
		else if (strstr(mat->GetName(), "smoke"))
		{
			nadeName = "smoke";
			break;
		}
		else if (strstr(mat->GetName(), "decoy"))
		{
			nadeName = "decoy";
			break;
		}
		else if (strstr(mat->GetName(), "incendiary") || strstr(mat->GetName(), "molotov"))
		{
			nadeName = "fire";
			break;
		}
	}
	BoxAndText(throwable, nadeName);
}

// ---- thanks for b1g trapware code
void CEsp::draw_direction_arrow(Vector2D center, float flYaw, float flDistCenter, float flLength, Color clr)
{
	Vector2D  vSlope = { sin(DEG2RAD(-flYaw)), -cos(DEG2RAD(flYaw)) };
	Vector2D vDirection = vSlope * flLength;
	Vector2D vStart = center + (vSlope * flDistCenter);
	Vector2D vEnd = vStart + (vDirection);

	float flLeft = flYaw - 1.5f;
	Vector2D vLSlope = { sin(DEG2RAD(-flLeft)), -cos(DEG2RAD(flLeft)) };
	Vector2D vLStart = center + (vLSlope * flDistCenter);

	float flRight = flYaw + 1.5f;
	Vector2D vRSlope = { sin(DEG2RAD(-flRight)), -cos(DEG2RAD(flRight)) };
	Vector2D vRStart = center + (vRSlope * flDistCenter);

	interfaces::surface->DrawSetColor(clr);

	Vertex_t vert[3] =
	{

		{ vLStart, vLStart },
	{ vRStart, vRStart },
	{ vEnd, vEnd },

	};

	Render::Polygon(3, vert, clr);
}
