#include "Visuals.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "Autowall.h"
#include "Hooks.h"
void CVisuals::Init()
{
}
void CVisuals::Move(CUserCmd *pCmd, bool &bSendPacket) {}

bool screen_transformxx(const vector& point, vector& screen)
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
bool world_to_screenxx(const vector &origin, vector &screen)
{
	if (!screen_transformxx(origin, screen)) {
		int iScreenWidth, iScreenHeight;
		interfaces::engine->GetScreenSize(iScreenWidth, iScreenHeight);
		screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
		screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;
		return true;
	}
	return false;
}
void CVisuals::Draw()
{
	IClientEntity* pLocal = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	if (options::menu.visuals.OtherNoScope.getstate() && pLocal->IsAlive() && pLocal->IsScoped())
		NoScopeCrosshair();
	if (options::menu.visuals.crosshair.getstate())
	{
		AutowallCrosshair();
		interfaces::engine->ClientCmd_Unrestricted("crosshair 0");
	}
	else
	{
		interfaces::engine->ClientCmd_Unrestricted("crosshair 1");
	}
}
void CVisuals::NoScopeCrosshair()
{
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;
	IClientEntity* pLocal = hackManager.plocal();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (game_utils::IsSniper(pWeapon))
	{
		Render::Line(MidX - 1000, MidY, MidX + 1000, MidY, Color(0, 0, 0, 255));
		Render::Line(MidX, MidY - 1000, MidX, MidY + 1000, Color(0, 0, 0, 255));
	}
}
void CVisuals::DrawCrosshair()
{
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;
	Render::Line(MidX - 10, MidY, MidX + 10, MidY, Color(0, 255, 0, 255));
	Render::Line(MidX, MidY - 10, MidX, MidY + 10, Color(0, 255, 0, 255));
}
void CVisuals::DrawRecoilCrosshair()
{
	IClientEntity *pLocal = hackManager.plocal();
	vector ViewAngles;
	interfaces::engine->get_viewangles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;
	vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec *= 10000;
	vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	vector end = start + fowardVec, endScreen;
	if (Render::TransformScreen(end, endScreen) && pLocal->IsAlive())
	{
		Render::Line(endScreen.x - 10, endScreen.y, endScreen.x + 10, endScreen.y, Color(0, 255, 0, 255));
		Render::Line(endScreen.x, endScreen.y - 10, endScreen.x, endScreen.y + 10, Color(0, 255, 0, 255));
	}
}
void CVisuals::SpreadCrosshair()
{
	IClientEntity *pLocal = hackManager.plocal();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	IClientEntity* WeaponEnt = interfaces::ent_list->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (!hackManager.plocal()->IsAlive())
		return;
//	if (!GameUtils::IsBallisticWeapon(pWeapon))
//		return;
	if (pWeapon == nullptr)
		return;
	int xs;
	int ys;
	interfaces::engine->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;
	float inaccuracy = pWeapon->GetInaccuracy() * 1000;
	char buffer4[64];
	sprintf_s(buffer4, "%.00f", inaccuracy);
	Render::DrawFilledCircle(Vector2D(xs, ys), Color(20, 20, 20, 124), inaccuracy, 60);
}
void CVisuals::AutowallCrosshair() //this was disgusting freaK
{
	IClientEntity *pLocal = hackManager.plocal();
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;
	vector ViewAngles;
	interfaces::engine->get_viewangles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;
	vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec *= 10000;
	vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	vector end = start + fowardVec, endScreen;
	int xs, ys;
	interfaces::engine->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;
	if (Render::TransformScreen(end, endScreen) && pLocal->IsAlive())
	{
		float damage = 0.f;
		Render::OutlinedRect(xs - 2, ys - 2, 5, 5, Color(27, 27, 27), Color(options::menu.ColorsTab.Menu.GetValue()));
	}
}


static void drawThiccLine(int x1, int y1, int x2, int y2, int type, Color color) {
	if (type > 1) {
		Render::Line(x1, y1 - 1, x2, y2 - 1, color);
		Render::Line(x1, y1, x2, y2, color);
		Render::Line(x1, y1 + 1, x2, y2 + 1, color);
		Render::Line(x1, y1 - 2, x2, y2 - 2, color);
		Render::Line(x1, y1 + 2, x2, y2 + 2, color);
	}
	else {
		Render::Line(x1 - 1, y1, x2 - 1, y2, color);
		Render::Line(x1, y1, x2, y2, color);
		Render::Line(x1 + 1, y1, x2 + 1, y2, color);
		Render::Line(x1 - 2, y1, x2 - 2, y2, color);
		Render::Line(x1 + 2, y1, x2 + 2, y2, color);
	}
}

void backup_visuals::single_arrow()
{
	int W, H, cW, cH;
	interfaces::engine->GetScreenSize(W, H);
	cW = W / 2;
	cH = H / 2;
	int rounding = 3;

	switch (bigboi::indicator)
	{
	case 1:
	{
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH - 10, 1, Color(options::menu.ColorsTab.Menu.GetValue()));// RIGHT | UP
																												   //RIGHT TARROW
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH + 10, 1, Color(options::menu.ColorsTab.Menu.GetValue())); //RIGHT DOWN
	}
	break;

	case 2:
	{
		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH - 10, 0, Color(options::menu.ColorsTab.Menu.GetValue())); //LEFT | UP 
																													//LEFT ARROW
		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH + 10, 0, Color(options::menu.ColorsTab.Menu.GetValue())); //LEFT | DOWN 
	}
	break;

	case 3:
	{
		drawThiccLine(cW, cH + 45, cW - 10, cH + 35 - rounding, 2, Color(options::menu.ColorsTab.Menu.GetValue()));// DOWN | LEFT
																												   //DOWN ARROW
		drawThiccLine(cW, cH + 45, cW + 10, cH + 35 - rounding, 2, Color(options::menu.ColorsTab.Menu.GetValue())); // DOWN | RIGHT
	}
	break;

	case 4:
	{
		drawThiccLine(cW, cH - 45, cW + 10, cH - 35 + rounding, 2, Color(options::menu.ColorsTab.Menu.GetValue()));// UP | LEFT
																												   //UP ARROW
		drawThiccLine(cW, cH - 45, cW - 10, cH - 35 + rounding, 2, Color(options::menu.ColorsTab.Menu.GetValue())); // UP| RIGHT
	}
	break;
	}
}

void backup_visuals::all_arrows()
{
	int W, H, cW, cH;
	interfaces::engine->GetScreenSize(W, H);
	cW = W / 2;
	cH = H / 2;
	int rounding = 3;

	switch (bigboi::indicator)
	{
	case 1:
	{
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH - 10, 1, Color(options::menu.ColorsTab.Menu.GetValue()));// RIGHT | UP
																												   //RIGHT TARROW
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH + 10, 1, Color(options::menu.ColorsTab.Menu.GetValue())); //RIGHT DOWN

																													//------ Other arrows ------//
		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH - 10, 0, Color(200, 200, 200, 10)); //LEFT | UP 
																							  //LEFT ARROW
		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH + 10, 0, Color(200, 200, 200, 10)); //LEFT | DOWN 

		drawThiccLine(cW, cH + 45, cW - 10, cH + 35 - rounding, 2, Color(200, 200, 200, 10));// DOWN | LEFT
																							 //DOWN ARROW
		drawThiccLine(cW, cH + 45, cW + 10, cH + 35 - rounding, 2, Color(200, 200, 200, 10));// DOWN | RIGHT

		drawThiccLine(cW, cH - 45, cW + 10, cH - 35 + rounding, 2, Color(200, 200, 200, 10));// UP | LEFT
																							 //UP ARROW
		drawThiccLine(cW, cH - 45, cW - 10, cH - 35 + rounding, 2, Color(200, 200, 200, 10)); // UP| RIGHT
	}
	break;

	case 2:
	{
		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH - 10, 0, Color(options::menu.ColorsTab.Menu.GetValue())); //LEFT | UP 
																													//LEFT ARROW
		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH + 10, 0, Color(options::menu.ColorsTab.Menu.GetValue())); //LEFT | DOWN 

																													//------ Other arrows ------//
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH - 10, 1, Color(200, 200, 200, 10));// RIGHT | UP
																							 //RIGHT TARROW
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH + 10, 1, Color(200, 200, 200, 10));//RIGHT DOWN

		drawThiccLine(cW, cH + 45, cW - 10, cH + 35 - rounding, 2, Color(200, 200, 200, 10));// DOWN | LEFT
																							 //DOWN ARROW
		drawThiccLine(cW, cH + 45, cW + 10, cH + 35 - rounding, 2, Color(200, 200, 200, 10)); // DOWN | RIGHT

		drawThiccLine(cW, cH - 45, cW + 10, cH - 35 + rounding, 2, Color(200, 200, 200, 10));// UP | LEFT
																							 //UP ARROW
		drawThiccLine(cW, cH - 45, cW - 10, cH - 35 + rounding, 2, Color(200, 200, 200, 10)); // UP| RIGHT
	}
	break;

	case 3:
	{
		drawThiccLine(cW, cH + 45, cW - 10, cH + 35 - rounding, 2, Color(options::menu.ColorsTab.Menu.GetValue()));// DOWN | LEFT
																												   //DOWN ARROW
		drawThiccLine(cW, cH + 45, cW + 10, cH + 35 - rounding, 2, Color(options::menu.ColorsTab.Menu.GetValue())); // DOWN | RIGHT

																													//------ Other arrows ------//
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH - 10, 1, Color(200, 200, 200, 10));// RIGHT | UP
																							 //RIGHT TARROW
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH + 10, 1, Color(200, 200, 200, 10));//RIGHT DOWN

		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH - 10, 0, Color(200, 200, 200, 10)); //LEFT | UP 
																							  //LEFT ARROW
		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH + 10, 0, Color(200, 200, 200, 10)); //LEFT | DOWN 

		drawThiccLine(cW, cH - 45, cW + 10, cH - 35 + rounding, 2, Color(200, 200, 200, 10));// UP | LEFT
																							 //UP ARROW
		drawThiccLine(cW, cH - 45, cW - 10, cH - 35 + rounding, 2, Color(200, 200, 200, 10)); // UP| RIGHT
	}
	break;

	case 4:
	{
		drawThiccLine(cW, cH - 45, cW + 10, cH - 35 + rounding, 2, Color(options::menu.ColorsTab.Menu.GetValue()));// UP | LEFT
																												   //UP ARROW
		drawThiccLine(cW, cH - 45, cW - 10, cH - 35 + rounding, 2, Color(options::menu.ColorsTab.Menu.GetValue())); // UP| RIGHT

																													//------ Other arrows ------//
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH - 10, 1, Color(200, 200, 200, 10));// RIGHT | UP
																							 //RIGHT TARROW
		drawThiccLine(cW + 45, cH, cW + 35 - rounding, cH + 10, 1, Color(200, 200, 200, 10));//RIGHT DOWN

		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH - 10, 0, Color(200, 200, 200, 10)); //LEFT | UP 
																							  //LEFT ARROW
		drawThiccLine(cW - 45, cH, cW - 35 + rounding, cH + 10, 0, Color(200, 200, 200, 10)); //LEFT | DOWN 

		drawThiccLine(cW, cH + 45, cW - 10, cH + 35 - rounding, 2, Color(200, 200, 200, 10));// DOWN | LEFT
																							 //DOWN ARROW
		drawThiccLine(cW, cH + 45, cW + 10, cH + 35 - rounding, 2, Color(200, 200, 200, 10)); // DOWN | RIGHT
	}
	break;
	}
}