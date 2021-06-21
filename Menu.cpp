#include "Menu.h"
#include "Controls.h"
#include "Hooks.h" 
#include "Interfaces.h"
#include "CRC32.h"
#include <fstream>
#include "XorStr.hpp"
#define WINDOW_WIDTH 575
#define WINDOW_HEIGHT 510 // 507
mirror_window options::menu;
struct Config_t {
	int id;
	std::string name;
};
std::vector<Config_t> configs;
typedef void(__cdecl* MsgFn)(const char* msg, va_list);
void MsgX(const char* msg, ...)
{
	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg"); 	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list); //Calls the function, we got the address above.
}
void save_callback()
{
	int should_save = options::menu.ColorsTab.ConfigListBox.GetIndex();
	std::string config_directory = "miroawr\\cfg\\";
	config_directory += configs[should_save].name; config_directory += ".xml";
	GUI.SaveWindowState(&options::menu, XorStr(config_directory.c_str()));
	interfaces::cvar->ConsoleColorPrintf(Color(140, 10, 250, 255), "Mirror v6 ");
	std::string uremam = "Saved configuration.     \n";
	MsgX(uremam.c_str());
}
void load_callback()
{
	int should_load = options::menu.ColorsTab.ConfigListBox.GetIndex();
	std::string config_directory = "miroawr\\cfg\\";
	config_directory += configs[should_load].name; config_directory += ".xml";
	GUI.LoadWindowState(&options::menu, XorStr(config_directory.c_str()));
	interfaces::cvar->ConsoleColorPrintf(Color(140, 10, 250, 255), "Mirror v6 ");
	std::string uremam = "Loaded configuration.     \n";
	MsgX(uremam.c_str());
}

void list_configs() {
	configs.clear();
	options::menu.ColorsTab.ConfigListBox.ClearItems();
	std::ifstream file_in;
	file_in.open("miroawr\\cfg\\miroawr_configs.txt");
	if (file_in.fail()) {
		std::ofstream("miroawr\\cfg\\miroawr_configs.txt");
		file_in.open("miroawr\\cfg\\miroawr_configs.txt");
	}
	int line_count;
	while (!file_in.eof()) {
		Config_t config;
		file_in >> config.name;
		config.id = line_count;
		configs.push_back(config);
		line_count++;
		options::menu.ColorsTab.ConfigListBox.AddItem(config.name);
	}
	file_in.close();
	if (configs.size() > 7) options::menu.ColorsTab.ConfigListBox.AddItem(" ");
}

void add_config() {
	std::fstream file;
	file.open("miroawr\\cfg\\miroawr_configs.txt", std::fstream::app);
	if (file.fail()) {
		std::fstream("miroawr\\cfg\\miroawr_configs.txt");
		file.open("miroawr\\cfg\\miroawr_configs.txt", std::fstream::app);
	}
	file << std::endl << options::menu.ColorsTab.NewConfigName.getText();
	file.close();
	list_configs();
	int should_add = options::menu.ColorsTab.ConfigListBox.GetIndex();
	std::string config_directory = "miroawr\\cfg\\";
	config_directory += options::menu.ColorsTab.NewConfigName.getText(); config_directory += ".xml";
	GUI.SaveWindowState(&options::menu, XorStr(config_directory.c_str()));
	options::menu.ColorsTab.NewConfigName.SetText("");
}

void remove_config() {
	int should_remove = options::menu.ColorsTab.ConfigListBox.GetIndex();
	std::string config_directory = "miroawr\\cfg\\";
	config_directory += configs[should_remove].name; config_directory += ".xml";
	std::remove(config_directory.c_str());
	std::ofstream ofs("miroawr\\cfg\\miroawr_configs.txt", std::ios::out | std::ios::trunc);
	ofs.close();
	std::fstream file;
	file.open("miroawr\\cfg\\miroawr_configs.txt", std::fstream::app);
	if (file.fail()) {
		std::fstream("miroawr\\cfg\\miroawr_configs.txt");
		file.open("miroawr\\cfg\\miroawr_configs.txt", std::fstream::app);
	}
	for (int i = 0; i < configs.size(); i++) {
		if (i == should_remove) continue;
		Config_t config = configs[i];
		file << std::endl << config.name;
	}
	file.close();
	list_configs();
}

void KnifeApplyCallbk()
{
	static ConVar* Meme = interfaces::cvar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	interfaces::engine->ClientCmd_Unrestricted("cl_fullupdate");
}

void UnLoadCallbk()
{
	DoUnload = true;
}
void mirror_window::Setup()
{
	SetPosition(350, 50);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetTitle("");
	RegisterTab(&aimbot);
	//	RegisterTab(&LegitBotTab);
	RegisterTab(&visuals);
	RegisterTab(&misc);
	RegisterTab(&ColorsTab);
	RegisterTab(&skin);

	RECT Client = GetClientArea();
	Client.bottom -= 29;
	aimbot.Setup();
	//	LegitBotTab.Setup();
	visuals.Setup();
	misc.Setup();
	ColorsTab.Setup();
	skin.Setup();

#pragma endregion
}
void CAimbotTab::Setup()
{
	SetTitle("J");
#pragma region Aimbot
	AimbotGroup.SetPosition(4, 25); // 15, 25
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(550, 225);
	AimbotGroup.AddTab(CGroupTab("Main", 1));
	AimbotGroup.AddTab(CGroupTab("Accuracy", 2));
	AimbotGroup.AddTab(CGroupTab("BodyAim", 3));
	AimbotGroup.AddTab(CGroupTab("Lag Compensation", 4));
	//	AimbotGroup.AddTab(CGroupTab("Debug", 5));
	//	AimbotGroup.AddTab(CGroupTab("Resolver Mods", 4));
	RegisterControl(&AimbotGroup);

	enable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl(1, "Enable", this, &enable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotAutoFire.SetState(true);

	rage_silent.SetFileId("aim_silentaim");
	AimbotGroup.PlaceLabledControl(1, "Silent Aim", this, &rage_silent);

	rage_chokeshot.SetFileId("aim_silent");
	AimbotGroup.PlaceLabledControl(1, "Choke Shot", this, &rage_chokeshot);

	AccuracyAutoScope.SetFileId("acc_scope");
	AimbotGroup.PlaceLabledControl(1, "Auto Scope", this, &AccuracyAutoScope);

	AccuracyHitchance.SetFileId("base_hc");
	AccuracyHitchance.SetBoundaries(0.f, 100.f);
	AccuracyHitchance.SetValue(20);

	AccuracyMinimumDamage.SetFileId("base_md");
	AccuracyMinimumDamage.SetBoundaries(0.f, 100.f);
	AccuracyMinimumDamage.SetValue(20);

	preso.SetFileId("acc_zeusisgay");
	preso.AddItem("Default");
	preso.AddItem("Down");
	preso.AddItem("Up");
	preso.AddItem("Zero");
	preso.AddItem("Anti Invalid Pitch");
	AimbotGroup.PlaceLabledControl(1, "Pitch Adjustment", this, &preso);

	resolver.SetFileId("acc_aaa");
	resolver.AddItem("Off");
	resolver.AddItem("Desync Logic");
	resolver.AddItem("Desync Brute");
	resolver.AddItem("Legacy Fake Angles");
	AimbotGroup.PlaceLabledControl(1, "Yaw Adjustment", this, &resolver);

//	resolver_info.SetFileId("acc_resolverinfo");
//	AimbotGroup.PlaceLabledControl(1, "Print Resolver Info", this, &resolver_info);

	AimbotFov.SetFileId("aim_fov");
	AimbotFov.SetBoundaries(0.f, 180.f);
	AimbotFov.extension = XorStr("°");
	AimbotFov.SetValue(180.f);

	/*
	TargetPointscale.SetFileId("acc_hitbox_Scale");
	TargetPointscale.SetBoundaries(0, 100);
	TargetPointscale.SetValue(50);
	TargetPointscale.extension = ("%%");
	AimbotGroup.PlaceLabledControl(2, "Hitbox Scale", this, &TargetPointscale);
	*/

	doubletap.SetFileId("aim_doubletapkey");
	AimbotGroup.PlaceLabledControl(1, "Double Tap (not implemented yet)", this, &doubletap);

	Multienable.SetFileId("multipoint_enable");
	AimbotGroup.PlaceLabledControl(2, "Toggle Multipoint", this, &Multienable);

	Multival2.SetFileId("hitbox_scale_head");
	Multival2.SetBoundaries(0.1, 100);
	Multival2.SetValue(20);
	Multival2.extension = XorStr("%%");
	AimbotGroup.PlaceLabledControl(2, "Head Multipoint", this, &Multival2);

	Multival4.SetFileId("hitbox_scale_upperbody");
	Multival4.SetBoundaries(0.1, 100);
	Multival4.SetValue(20);
	Multival4.extension = XorStr("%%");
	AimbotGroup.PlaceLabledControl(2, "Upper Body Multipoint", this, &Multival4);

	Multival.SetFileId("hitbox_scale_body");
	Multival.SetBoundaries(0.1, 100);
	Multival.SetValue(20);
	Multival.extension = XorStr("%%");
	AimbotGroup.PlaceLabledControl(2, "Lower Body Multipoint", this, &Multival);

	MultiVal3.SetFileId("hitbox_scale_legs");
	MultiVal3.SetBoundaries(0.1, 100);
	MultiVal3.SetValue(20);
	MultiVal3.extension = XorStr("%%");
	AimbotGroup.PlaceLabledControl(2, "Leg Multipoint", this, &MultiVal3);

	delay_shot.SetFileId("delay_shot");
	delay_shot.AddItem("Off");
	delay_shot.AddItem("Sim-Time");
	delay_shot.AddItem("Lag Compensation");
	delay_shot.AddItem("Refine Shot");
	AimbotGroup.PlaceLabledControl(2, "Delay Shot", this, &delay_shot);

//	extrapolation.SetFileId("acc_extra_P_lation");
//	AimbotGroup.PlaceLabledControl(3, "Extrapolation", this, &extrapolation);

	baim_fake.SetFileId("bodyaim_fake");
	AimbotGroup.PlaceLabledControl(3, "If Fake", this, &baim_fake); // if we have to resort to a brute

//	baim_fakewalk.SetFileId("bodyaim_fakewalk");
//	AimbotGroup.PlaceLabledControl(3, "If Slow Walk", this, &baim_fakewalk);

	baim_inair.SetFileId("bodyaim_inair");
	AimbotGroup.PlaceLabledControl(3, "If In Air", this, &baim_inair); //if they be flyin like a plane

	BaimIfUnderXHealth.SetFileId("acc_BaimIfUnderXHealth");
	BaimIfUnderXHealth.SetBoundaries(0, 100);
	BaimIfUnderXHealth.extension = XorStr("HP");
	BaimIfUnderXHealth.SetValue(0);
	AimbotGroup.PlaceLabledControl(3, "If HP Lower Than", this, &BaimIfUnderXHealth);

	bigbaim.SetFileId("acc_bigbaim");
	AimbotGroup.PlaceLabledControl(3, "On Key", this, &bigbaim);

	ignorelimbs.SetFileId("acc_ignorelimbs");
	AimbotGroup.PlaceLabledControl(3, "Ignore Limbs If Moving", this, &ignorelimbs);

	toggle_override.SetFileId("acc_mindmg_override_toggle");
	AimbotGroup.PlaceLabledControl(3, "Minimum Damage Override", this, &toggle_override);

	mindmg_override.SetFileId("acc_min_damage_override_amt");
	mindmg_override.SetBoundaries(0, 100);
	mindmg_override.extension = XorStr("HP");
	mindmg_override.SetValue(20);
	AimbotGroup.PlaceLabledControl(3, "Minimum Damage Override", this, &mindmg_override);

	extrapolation.SetFileId("lc_extrapolation");
	extrapolation.AddItem("Default");
	extrapolation.AddItem("Velocity Add");
	AimbotGroup.PlaceLabledControl(4, "Extrapolation", this, &extrapolation);

	onshot.SetFileId("lc_onshot"); 
	AimbotGroup.PlaceLabledControl(4, "Target On Shot", this, &onshot);

	blacklist_if_breaklc.SetFileId("lc_blacklist_boxes_if_breaklc"); // if the enemy is breaking lc, we will blacklist upperbody
	AimbotGroup.PlaceLabledControl(4, "Body Only If Breaking LC", this, &blacklist_if_breaklc);

	lowfps.SetFileId("lc_lowfps"); 
	AimbotGroup.PlaceLabledControl(4, "Low FPS Mode", this, &lowfps);

	lc_ticks.SetFileId("lc_ticks");
	lc_ticks.SetBoundaries(2.f, 12.f); // i do the b1g
	lc_ticks.extension = XorStr(" ticks"); // issa meme
	lc_ticks.SetValue(4.f);
//	AimbotGroup.PlaceLabledControl(4, "Ticks To Compensate", this, &lc_ticks);

	/*
	prefer_head.SetFileId("acc_prefer_head");
	prefer_head.items.push_back(dropdownboxitem(false, XorStr("If Moving")));
	//	prefer_head.items.push_back(dropdownboxitem(false, XorStr("Lower body Is Unhittable")));
	prefer_head.items.push_back(dropdownboxitem(false, XorStr("No Fake")));
	//	prefer_head.items.push_back(dropdownboxitem(false, XorStr("Head Is Visible")));
	//	prefer_head.items.push_back(dropdownboxitem(false, XorStr("")));
	AimbotGroup.PlaceLabledControl(4, "Prefer Head Aim Factors", this, &prefer_head);
	*/
	//	toggledebug.SetFileId("debugtoggle");
	//	AimbotGroup.PlaceLabledControl(5, "Print Debug Info In Console", this, &toggledebug);


	// -<--------------------------------------------------------------->- //
	/*
	legit_mode.SetFileId("aim_legit_toggle");
	AimbotGroup.PlaceLabledControl(4, "Enable", this, &legit_mode);

	legit_trigger.SetFileId("aim_legit_trigger");
	AimbotGroup.PlaceLabledControl(4, "Trigger", this, &legit_trigger);

	legit_trigger_key.SetFileId("aim_legit_trigger_key");
	AimbotGroup.PlaceLabledControl(4, "Trigger Key", this, &legit_trigger_key);

	apply_smooth.SetFileId("aim_legit_apply_smooth");
	AimbotGroup.PlaceLabledControl(4, "Apply Smoothness", this, &apply_smooth); */
	// -<--------------------------------------------------------------->- //

	weapongroup.SetText("Weapon Configurations");
	weapongroup.SetPosition(4, 260); // 15, 230
	weapongroup.SetSize(550, 180);
	weapongroup.AddTab(CGroupTab("Auto Sniper", 1));
	weapongroup.AddTab(CGroupTab("Pistols", 2));
	weapongroup.AddTab(CGroupTab("Scout", 3));
	weapongroup.AddTab(CGroupTab("Awp", 4));
	weapongroup.AddTab(CGroupTab("SMG", 5));
	weapongroup.AddTab(CGroupTab("Others", 6));
	RegisterControl(&weapongroup);

	/*
	target_auto.SetFileId("tgt_hitbox_auto");
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_auto.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(1, "HitScan", this, &target_auto);

	custom_hitscan.SetFileId("auto_hitscan_auto");
	weapongroup.PlaceLabledControl(1, "Automatic HitScan", this, &custom_hitscan);

	hc_auto.SetFileId("auto_hitchance");
	hc_auto.SetBoundaries(0, 100);
	hc_auto.SetValue(25);
	hc_auto.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(1, "Hitchance", this, &hc_auto);

	md_auto.SetFileId("auto_minimumdamage");
	md_auto.SetBoundaries(0, 100);
	md_auto.SetValue(25);
	md_auto.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(1, "Minimum Damage", this, &md_auto);

	//	preset_auto.SetFileId("auto_automatic_cfg");
	//	weapongroup.PlaceLabledControl(1, "Automatic Auto Sniper Configuration", this, &preset_auto);
	//----------------------------------------------------------------------

	target_pistol.SetFileId("tgt_hitbox_pistol");
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_pistol.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(2, "HitScan", this, &target_pistol);

	hc_pistol.SetFileId("pistol_hitchance");
	hc_pistol.SetBoundaries(0, 100);
	hc_pistol.SetValue(25);
	hc_pistol.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(2, "Hitchance", this, &hc_pistol);

	md_pistol.SetFileId("pistol_minimumdamage");
	md_pistol.SetBoundaries(0, 100);
	md_pistol.SetValue(25);
	md_pistol.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(2, "Minimum Damage", this, &md_pistol);


	//	preset_pistol.SetFileId("pistol_automatic_cfg");
	//	weapongroup.PlaceLabledControl(2, "Automatic Pistol Configuration", this, &preset_pistol);

	//----------------------------------------------------------------------

	target_scout.SetFileId("tgt_hitbox_scout");
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_scout.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(3, "HitScan", this, &target_scout);

	hc_scout.SetFileId("scout_hitchance");
	hc_scout.SetBoundaries(0, 100);
	hc_scout.SetValue(25);
	hc_scout.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(3, "Hitchance", this, &hc_scout);

	md_scout.SetFileId("scout_minimumdamage");
	md_scout.SetBoundaries(0, 100);
	md_scout.SetValue(25);
	md_scout.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(3, "Minimum Damage", this, &md_scout);

	//	headonly_if_vis_scout.SetFileId("headonly_if_vis_scout");
	//	weapongroup.PlaceLabledControl(3, "Headshot Only If Hittable", this, &headonly_if_vis_scout);

	//	preset_scout.SetFileId("scout_automatic_cfg");
	//	weapongroup.PlaceLabledControl(3, "Automatic Scout Configuration", this, &preset_scout);
	//----------------------------------------------------------------------

	target_awp.SetFileId("tgt_hitbox_awp");
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_awp.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(4, "HitScan", this, &target_awp);

	hc_awp.SetFileId("awp_hitchance");
	hc_awp.SetBoundaries(0, 100);
	hc_awp.SetValue(25);
	hc_awp.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(4, "Hitchance", this, &hc_awp);

	md_awp.SetFileId("awp_minimumdamage");
	md_awp.SetBoundaries(0, 125);
	md_awp.SetValue(25);
	md_awp.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(4, "Minimum Damage", this, &md_awp);

	min_damage_enemy_hp_awp.SetFileId("tgt_min_damage_enemy_hp_awp");
	weapongroup.PlaceLabledControl(4, "Minimum Damage Based Off Enemy Health", this, &min_damage_enemy_hp_awp);

	//----------------------------------------------------------------------

	target_smg.SetFileId("tgt_hitbox_smg");
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_smg.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(5, "HitScan", this, &target_smg);

	hc_smg.SetFileId("smg_hitchance");
	hc_smg.SetBoundaries(0, 100);
	hc_smg.SetValue(25);
	hc_smg.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(5, "Hitchance", this, &hc_smg);

	md_smg.SetFileId("smg_minimumdamage");
	md_smg.SetBoundaries(0, 100);
	md_smg.SetValue(25);
	md_smg.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(5, "Minimum Damage", this, &md_smg);

	//----------------------------------------------------------------------

	target_otr.SetFileId("tgt_hitbox_otr");
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Head")));
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Upper Body")));
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Lower Body")));
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Arms")));
	target_otr.items.push_back(dropdownboxitem(false, XorStr("Legs")));
	weapongroup.PlaceLabledControl(6, "HitScan", this, &target_otr);

	hc_otr.SetFileId("otr_hitchance");
	hc_otr.SetBoundaries(0, 100);
	hc_otr.SetValue(25);
	hc_otr.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(6, "Hitchance", this, &hc_otr);

	md_otr.SetFileId("otr_minimumdamage");
	md_otr.SetBoundaries(0, 100);
	md_otr.SetValue(25);
	md_otr.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(6, "Minimum Damage", this, &md_otr);

	//	headonly_if_vis_otr.SetFileId("headonly_if_vis_otr");
	//	weapongroup.PlaceLabledControl(5, "Headshot Only If Hittable", this, &headonly_if_vis_otr);

	//	preset_otr.SetFileId("otr_automatic_cfg");
	//	weapongroup.PlaceLabledControl(5, "Automatic  Weapon Configuration", this, &preset_otr);

	*/

	/*		target_auto.SetFileId("tgt_hitbox_auto");
	target_auto.AddItem("Head");
	target_auto.AddItem("Neck");
	target_auto.AddItem("Chest");
	target_auto.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(1, "Hitbox Priority", this, &target_auto); */

	target_auto2.SetFileId("tgt_hitscan_autosniper");
	target_auto2.AddItem("Head Only");
	target_auto2.AddItem("Minimal");
	target_auto2.AddItem("Essential");
	target_auto2.AddItem("Maximal");
	target_auto2.AddItem("Body Only");
	weapongroup.PlaceLabledControl(1, "Hitscan", this, &target_auto2);

	hc_auto.SetFileId("auto_hitchance");
	hc_auto.SetBoundaries(0, 100);
	hc_auto.SetValue(25);
	hc_auto.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(1, "Hitchance", this, &hc_auto);

	md_auto.SetFileId("auto_minimumdamage");
	md_auto.SetBoundaries(0, 100);
	md_auto.SetValue(25);
	md_auto.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(1, "Minimum Damage", this, &md_auto);

	automatic_cfg.SetFileId("auto_automatic_auto_cfg");
	weapongroup.PlaceLabledControl(1, "Automatic Configuration", this, &automatic_cfg);
	//----------------------------------------------------------------------

	/*		target_pistol.SetFileId("tgt_hitbox_pistol");
	target_pistol.AddItem("Head");
	target_pistol.AddItem("Neck");
	target_pistol.AddItem("Chest");
	target_pistol.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(2, "Hitbox Priority", this, &target_pistol); */

	target_pistol2.SetFileId("tgt_hitscan_pistol");
	target_pistol2.AddItem("Head Only");
	target_pistol2.AddItem("Minimal");
	target_pistol2.AddItem("Essential");
	target_pistol2.AddItem("Maximal");
	target_pistol2.AddItem("Body Only");
	weapongroup.PlaceLabledControl(2, "Hitscan", this, &target_pistol2);

	hc_pistol.SetFileId("pistol_hitchance");
	hc_pistol.SetBoundaries(0, 100);
	hc_pistol.SetValue(25);
	hc_pistol.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(2, "Hitchance", this, &hc_pistol);

	md_pistol.SetFileId("pistol_minimumdamage");
	md_pistol.SetBoundaries(0, 100);
	md_pistol.SetValue(25);
	md_pistol.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(2, "Minimum Damage", this, &md_pistol);


	//----------------------------------------------------------------------

	/*		target_scout.SetFileId("tgt_hitbox_scout");
	target_scout.AddItem("Head");
	target_scout.AddItem("Neck");
	target_scout.AddItem("Chest");
	target_scout.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(3, "Hitbox Priority", this, &target_scout); */

	target_scout2.SetFileId("tgt_hitscan_scout");
	target_scout2.AddItem("Head Only");
	target_scout2.AddItem("Minimal");
	target_scout2.AddItem("Essential");
	target_scout2.AddItem("Maximal");
	target_scout2.AddItem("Body Only");
	weapongroup.PlaceLabledControl(3, "Hitscan", this, &target_scout2);

	hc_scout.SetFileId("scout_hitchance");
	hc_scout.SetBoundaries(0, 100);
	hc_scout.SetValue(25);
	hc_scout.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(3, "Hitchance", this, &hc_scout);

	md_scout.SetFileId("scout_minimumdamage");
	md_scout.SetBoundaries(0, 100);
	md_scout.SetValue(25);
	md_scout.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(3, "Minimum Damage", this, &md_scout);

	//----------------------------------------------------------------------

	/*		target_awp.SetFileId("tgt_hitbox_awp");
	target_awp.AddItem("Head");
	target_awp.AddItem("Neck");
	target_awp.AddItem("Chest");
	target_awp.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(4, "Hitbox Priority", this, &target_awp); */

	target_awp2.SetFileId("tgt_hitscan_awp");
	target_awp2.AddItem("Head Only");
	target_awp2.AddItem("Minimal");
	target_awp2.AddItem("Essential");
	target_awp2.AddItem("Maximal");
	target_awp2.AddItem("Body Only");
	weapongroup.PlaceLabledControl(4, "Hitscan", this, &target_awp2);

	hc_awp.SetFileId("awp_hitchance");
	hc_awp.SetBoundaries(0, 100);
	hc_awp.SetValue(25);
	hc_awp.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(4, "Hitchance", this, &hc_awp);

	md_awp.SetFileId("awp_minimumdamage");
	md_awp.SetBoundaries(0, 120);
	md_awp.SetValue(25);
	md_awp.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(4, "Minimum Damage", this, &md_awp);


	//----------------------------------------------------------------------

	/*		target_otr.SetFileId("tgt_hitbox_otr");
	target_otr.AddItem("Head");
	target_otr.AddItem("Neck");
	target_otr.AddItem("Chest");
	target_otr.AddItem("Pelvis");
	weapongroup.PlaceLabledControl(5, "Hitbox Priority", this, &target_otr); */

	target_smg2.SetFileId("tgt_hitscan_smg");
	target_smg2.AddItem("Head Only");
	target_smg2.AddItem("Minimal");
	target_smg2.AddItem("Essential");
	target_smg2.AddItem("Maximal");
	target_smg2.AddItem("Body Only");
	weapongroup.PlaceLabledControl(5, "Hitscan", this, &target_smg2);

	hc_smg.SetFileId("otr_hitchance");
	hc_smg.SetBoundaries(0, 100);
	hc_smg.SetValue(25);
	hc_smg.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(5, "Hitchance", this, &hc_smg);

	md_smg.SetFileId("otr_minimumdamage");
	md_smg.SetBoundaries(0, 100);
	md_smg.SetValue(25);
	md_smg.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(5, "Minimum Damage", this, &md_smg);


	target_otr2.SetFileId("tgt_hitscan_otr");
	target_otr2.AddItem("Head Only");
	target_otr2.AddItem("Minimal");
	target_otr2.AddItem("Essential");
	target_otr2.AddItem("Maximal");
	target_otr2.AddItem("Body Only");
	weapongroup.PlaceLabledControl(6, "Hitscan", this, &target_otr2);

	hc_otr.SetFileId("otr_hitchance");
	hc_otr.SetBoundaries(0, 100);
	hc_otr.SetValue(25);
	hc_otr.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(6, "Hitchance", this, &hc_otr);

	md_otr.SetFileId("otr_minimumdamage");
	md_otr.SetBoundaries(0, 100);
	md_otr.SetValue(25);
	md_otr.extension = XorStr("%%");
	weapongroup.PlaceLabledControl(6, "Minimum Damage", this, &md_otr);


#pragma endregion  AntiAim controls get setup in here
}
/*
void CLegitBotTab::Setup()
{	/*
_      ______ _____ _____ _______
| |    |  ____/ ____|_   _|__   __|
| |    | |__ | |  __  | |    | |
| |    |  __|| | |_ | | |    | |
| |____| |___| |__| |_| |_   | |
|______|______\_____|_____|  |_|


SetTitle("B");
AimbotGroup.SetText("Main");
AimbotGroup.SetPosition(4, 25);
AimbotGroup.SetSize(550, 222);
AimbotGroup.AddTab(CGroupTab("Aimbot", 1));
AimbotGroup.AddTab(CGroupTab("Triggerbot", 2));
AimbotGroup.AddTab(CGroupTab("Filters", 3));
RegisterControl(&AimbotGroup);
RegisterControl(&Active);
Active.SetFileId("active");
AimbotGroup.PlaceLabledControl(1, "Activate Legitbot", this, &Active);
AimbotEnable.SetFileId("l_aimbot");
AimbotGroup.PlaceLabledControl(1, "Aimbot Enable", this, &AimbotEnable);
aimbotfiremode.SetFileId("l_autoaimbot");
aimbotfiremode.AddItem("On Shot");
aimbotfiremode.AddItem("Automatic");
AimbotGroup.PlaceLabledControl(1, "Fire Mode", this, &aimbotfiremode);
AimbotKeyBind.SetFileId("l_aimkey");
AimbotGroup.PlaceLabledControl(1, "KeyBind", this, &AimbotKeyBind);
BackTrack.SetFileId("l_backtrack");
AimbotGroup.PlaceLabledControl(1, "Backtrack", this, &BackTrack);
AimbotSmokeCheck.SetFileId("l_smokeaimbot");
AimbotGroup.PlaceLabledControl(1, "Smoke Check", this, &AimbotSmokeCheck);

legitresolver.SetFileId("l_B1GresolverTappingSkeet");
AimbotGroup.PlaceLabledControl(1, "Resolver", this, &legitresolver);

//---- Trigger ---//
TriggerEnable.SetFileId("t_triggerbotenable");
AimbotGroup.PlaceLabledControl(2, "Activate Trigger", this, &TriggerEnable);
triggertype.SetFileId("t_triggerbottyp");
triggertype.AddItem("None");
triggertype.AddItem("Automatic");
AimbotGroup.PlaceLabledControl(2, "Trigger Mode", this, &triggertype);
TriggerHitChanceAmmount.SetFileId("l_trigHC");
TriggerHitChanceAmmount.SetBoundaries(0, 100);
TriggerHitChanceAmmount.extension = XorStr("%%");
TriggerHitChanceAmmount.SetValue(0);
AimbotGroup.PlaceLabledControl(2, "Hitchance", this, &TriggerHitChanceAmmount);
TriggerSmokeCheck.SetFileId("l_trigsmoke");
AimbotGroup.PlaceLabledControl(2, "Smoke Check", this, &TriggerSmokeCheck);
TriggerRecoil.SetFileId("l_trigRCS");
TriggerRecoil.SetBoundaries(0.f, 100.f);
TriggerRecoil.extension = XorStr("%%");
TriggerRecoil.SetValue(5.00f);
AimbotGroup.PlaceLabledControl(2, "Recoil", this, &TriggerRecoil);
TriggerKeyBind.SetFileId("l_trigkey");
AimbotGroup.PlaceLabledControl(2, "KeyBind", this, &TriggerKeyBind);
// ---- Hitboxes ---- //
TriggerHead.SetFileId("l_trighead");
AimbotGroup.PlaceLabledControl(3, "Head", this, &TriggerHead);
TriggerChest.SetFileId("l_trigchest");
AimbotGroup.PlaceLabledControl(3, "Chest", this, &TriggerChest);
TriggerStomach.SetFileId("l_trigstomach");
AimbotGroup.PlaceLabledControl(3, "Stomach", this, &TriggerStomach);
TriggerArms.SetFileId("l_trigarms");
AimbotGroup.PlaceLabledControl(3, "Arms", this, &TriggerArms);
TriggerLegs.SetFileId("l_triglegs");
AimbotGroup.PlaceLabledControl(3, "Legs", this, &TriggerLegs);

//--------------------------legitaa--------------------------((
/*	aaenable.SetFileId("AA_anable");
AimbotGroup.PlaceLabledControl(4, "Legit AA Enable", this, &aaenable);

aatyp.SetFileId("AA_aatyp");
aatyp.AddItem("Static");
aatyp.AddItem("Lowerbody");
aatyp.AddItem("Freestanding");
AimbotGroup.PlaceLabledControl(4, "Anti-Aim", this, &aatyp);

aatyp2.SetFileId("AA_aatyp2");
aatyp2.AddItem("Default");
aatyp2.AddItem("Jitter");
aatyp2.AddItem("Shuffle");
aatyp2.AddItem("Spin");
AimbotGroup.PlaceLabledControl(4, "Anti-Aim Type", this, &aatyp2);

aatyp3.SetFileId("AA_aatyp3");
aatyp3.SetBoundaries(0, 90);
aatyp3.SetValue(20);
AimbotGroup.PlaceLabledControl(4, "Anti-Aim Value", this, &aatyp3);

aafl.SetFileId("AA_aaFL");
aafl.SetBoundaries(1, 4);
aafl.SetValue(1);
AimbotGroup.PlaceLabledControl(4, "Legit FakeLag", this, &aafl); */
//----------------------solid kys---------------------//
/*
weapongroup.SetText("Main");
weapongroup.SetPosition(4, 256);
weapongroup.SetSize(550, 183);
weapongroup.AddTab(CGroupTab("Rifle", 1));
weapongroup.AddTab(CGroupTab("Pistol", 2));
weapongroup.AddTab(CGroupTab("Sniper", 3));
weapongroup.AddTab(CGroupTab("SMG", 4));
weapongroup.AddTab(CGroupTab("Heavy", 5));
RegisterControl(&weapongroup);
RegisterControl(&Active);
WeaponMainHitbox.SetFileId("l_rhitbox");
WeaponMainHitbox.AddItem("Head");
WeaponMainHitbox.AddItem("Neck");
WeaponMainHitbox.AddItem("Chest");
WeaponMainHitbox.AddItem("Stomach");
WeaponMainHitbox.AddItem("Nearest");
weapongroup.PlaceLabledControl(1, "Hitbox", this, &WeaponMainHitbox);
WeaponMainSpeed.SetFileId("l_rspeed");
WeaponMainSpeed.SetBoundaries(0, 75);
WeaponMainSpeed.SetValue(5);
weapongroup.PlaceLabledControl(1, "Speed", this, &WeaponMainSpeed);
WeaponMainRecoil.SetFileId("l_rRecoil");
WeaponMainRecoil.SetBoundaries(0, 200);
WeaponMainRecoil.SetValue(165);
weapongroup.PlaceLabledControl(1, "Recoil", this, &WeaponMainRecoil);
WeaponMainFoV.SetFileId("l_fov");
WeaponMainFoV.SetBoundaries(0, 45);
WeaponMainFoV.SetValue(10);
weapongroup.PlaceLabledControl(1, "Field Of View", this, &WeaponMainFoV);
// --- Pistols --- //
WeaponPistHitbox.SetFileId("l_phitbox");
WeaponPistHitbox.AddItem("Head");
WeaponPistHitbox.AddItem("Neck");
WeaponPistHitbox.AddItem("Chest");
WeaponPistHitbox.AddItem("Stomach");
WeaponPistHitbox.AddItem("Nearest");
weapongroup.PlaceLabledControl(2, "Hitbox", this, &WeaponPistHitbox);
WeaponPistSpeed.SetFileId("l_pspeed");
WeaponPistSpeed.SetBoundaries(0, 75);
WeaponPistSpeed.SetValue(5);
weapongroup.PlaceLabledControl(2, "Speed", this, &WeaponPistSpeed);
WeaponPistRecoil.SetFileId("l_pRecoil");
WeaponPistRecoil.SetBoundaries(0, 200);
WeaponPistRecoil.SetValue(165);
weapongroup.PlaceLabledControl(2, "Recoil", this, &WeaponPistRecoil);
WeaponPistFoV.SetFileId("l_pfov");
WeaponPistFoV.SetBoundaries(0, 45);
WeaponPistFoV.SetValue(10);
weapongroup.PlaceLabledControl(2, "Field Of View", this, &WeaponPistFoV);
// --- Sniper --- //
WeaponSnipHitbox.SetFileId("l_shitbox");
WeaponSnipHitbox.AddItem("Head");
WeaponSnipHitbox.AddItem("Neck");
WeaponSnipHitbox.AddItem("Chest");
WeaponSnipHitbox.AddItem("Stomach");
WeaponSnipHitbox.AddItem("Nearest");
weapongroup.PlaceLabledControl(3, "Hitbox", this, &WeaponSnipHitbox);
WeaponSnipSpeed.SetFileId("l_sspeed");
WeaponSnipSpeed.SetBoundaries(0, 75);
WeaponSnipSpeed.SetValue(5);
weapongroup.PlaceLabledControl(3, "Speed", this, &WeaponSnipSpeed);
WeaponSnipRecoil.SetFileId("l_sRecoil");
WeaponSnipRecoil.SetBoundaries(0, 200);
WeaponSnipRecoil.SetValue(165);
weapongroup.PlaceLabledControl(3, "Recoil", this, &WeaponSnipRecoil);
WeaponSnipFoV.SetFileId("l_sfov");
WeaponSnipFoV.SetBoundaries(0, 45);
WeaponSnipFoV.SetValue(10);
weapongroup.PlaceLabledControl(3, "Field Of View", this, &WeaponSnipFoV);
// --- SMG --- //
WeaponMpHitbox.SetFileId("l_sniphitbox");
WeaponMpHitbox.AddItem("Head");
WeaponMpHitbox.AddItem("Neck");
WeaponMpHitbox.AddItem("Chest");
WeaponMpHitbox.AddItem("Stomach");
WeaponMpHitbox.AddItem("Nearest");
weapongroup.PlaceLabledControl(4, "Hitbox", this, &WeaponMpHitbox);
WeaponMpSpeed.SetFileId("l_sspeed");
WeaponMpSpeed.SetBoundaries(0, 75);
WeaponMpSpeed.SetValue(5);
weapongroup.PlaceLabledControl(4, "Speed", this, &WeaponMpSpeed);
WeaponMpRecoil.SetFileId("l_sRecoil");
WeaponMpRecoil.SetBoundaries(0, 200);
WeaponMpRecoil.SetValue(165);
weapongroup.PlaceLabledControl(4, "Recoil", this, &WeaponMpRecoil);
WeaponMpFoV.SetFileId("l_sfov");
WeaponMpFoV.SetBoundaries(0, 45);
WeaponMpFoV.SetValue(10);
weapongroup.PlaceLabledControl(4, "Field Of View", this, &WeaponMpFoV);
// --- MachineGun --- //
WeaponMGHitbox.SetFileId("l_mghitbox");
WeaponMGHitbox.AddItem("Head");
WeaponMGHitbox.AddItem("Neck");
WeaponMGHitbox.AddItem("Chest");
WeaponMGHitbox.AddItem("Stomach");
WeaponMGHitbox.AddItem("Nearest");
weapongroup.PlaceLabledControl(5, "Hitbox", this, &WeaponMGHitbox);
WeaponMGSpeed.SetFileId("l_mgspeed");
WeaponMGSpeed.SetBoundaries(0, 75);
WeaponMGSpeed.SetValue(5);
weapongroup.PlaceLabledControl(5, "Speed", this, &WeaponMGSpeed);
WeaponMGRecoil.SetFileId("l_mgRecoil");
WeaponMGRecoil.SetBoundaries(0, 200);
WeaponMGRecoil.SetValue(165);
weapongroup.PlaceLabledControl(5, "Recoil", this, &WeaponMGRecoil);
WeaponMGFoV.SetFileId("l_mgfov");
WeaponMGFoV.SetBoundaries(0, 45);
WeaponMGFoV.SetValue(10);
weapongroup.PlaceLabledControl(5, "Field Of View", this, &WeaponMGFoV);
}
*/
void CVisualTab::Setup()
{

	SetTitle("D");
#pragma region Options
	OptionsGroup.SetText("Options");
	OptionsGroup.SetPosition(4, 30);
	OptionsGroup.SetSize(280, 412);
	OptionsGroup.AddTab(CGroupTab("Main", 1));
	OptionsGroup.AddTab(CGroupTab("Filters", 2));
	OptionsGroup.AddTab(CGroupTab("Misc", 3));
	OptionsGroup.AddTab(CGroupTab("Other", 4));
	RegisterControl(&OptionsGroup);
	RegisterControl(&Active);

	Active.SetFileId("active");
	OptionsGroup.PlaceLabledControl(1, "Activate Visuals", this, &Active);

	OptionsBox.SetFileId("opt_box");
	OptionsBox.AddItem("Off");
	OptionsBox.AddItem("Default");
	OptionsBox.AddItem("Genuine");
	OptionsBox.AddItem("Corners");
	OptionsGroup.PlaceLabledControl(1, "Box", this, &OptionsBox);

	OptionsName.SetFileId("opt_name");
	OptionsName.AddItem("Off");
	OptionsName.AddItem("Top");
	//	OptionsName.AddItem("Right");
	OptionsGroup.PlaceLabledControl(1, "Name", this, &OptionsName);

	OptionsHealth.SetFileId("opt_hp");
	OptionsHealth.AddItem("Off");
	OptionsHealth.AddItem("Default");
	OptionsHealth.AddItem("Battery");
	OptionsHealth.AddItem("Bottom");

	OptionsGroup.PlaceLabledControl(1, "Health", this, &OptionsHealth);

	health_col.SetFileId(XorStr("opt_colhp"));
	OptionsGroup.PlaceLabledControl(1, "", this, &health_col);

	OptionsArmor.SetFileId("otr_armor");
	OptionsArmor.AddItem("Off");
	OptionsArmor.AddItem("Default");
	OptionsArmor.AddItem("Battery");
	OptionsArmor.AddItem("Bottom");
	OptionsGroup.PlaceLabledControl(1, "Armor Bar", this, &OptionsArmor); // here

	armor_col.SetFileId(XorStr("opt_col_ar"));
	OptionsGroup.PlaceLabledControl(1, " ", this, &armor_col);

	OptionsInfo.SetFileId("opt_info");
	OptionsGroup.PlaceLabledControl(1, "Info", this, &OptionsInfo);

	OptionsSkeleton.SetFileId("opt_bone");
	OptionsGroup.PlaceLabledControl(1, "Skeleton", this, &OptionsSkeleton);

	Weapons.SetFileId("kysquecest");
	OptionsGroup.PlaceLabledControl(1, "Weapons", this, &Weapons);

	Ammo.SetFileId("urmomsucksass");
	OptionsGroup.PlaceLabledControl(1, "Ammo Bar", this, &Ammo);

	//	OffscreenESP.SetFileId("otr_offscreenESP");
	//	OptionsGroup.PlaceLabledControl(1, "Offscreen ESP", this, &OffscreenESP);

	/*
	GlowZ.SetFileId("opt_glowz");
	GlowZ.SetValue(0.f);
	GlowZ.SetBoundaries(0.f, 100.f);
	GlowZ.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(1, "Enemy Glow", this, &GlowZ);

	team_glow.SetFileId("opt_team_glow");
	team_glow.SetValue(0.f);
	team_glow.SetBoundaries(0.f, 100.f);
	team_glow.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(1, "Team Glow", this, &team_glow);

	Glowz_lcl.SetFileId("opt_glowz_lcl");
	Glowz_lcl.SetValue(0.f);
	Glowz_lcl.SetBoundaries(0.f, 100.f);
	Glowz_lcl.extension = XorStr("%%");
	OptionsGroup.PlaceLabledControl(1, "Local Glow", this, &Glowz_lcl);
	*/

	//	debug_esp.SetFileId("opt_debug_Esp");
	//	OptionsGroup.PlaceLabledControl(1, "Debug Esp", this, &debug_esp);

	//	FiltersAll.SetFileId("ftr_all");
	//	OptionsGroup.PlaceLabledControl(2, "All", this, &FiltersAll);

	show_players.SetFileId("ftr_players");
	show_players.SetState(true);
	OptionsGroup.PlaceLabledControl(2, "Players", this, &show_players);

	show_team.SetFileId("ftr_enemyonly");
	OptionsGroup.PlaceLabledControl(2, "Show Team", this, &show_team);

	FiltersNades.SetFileId("ftr_nades");
	OptionsGroup.PlaceLabledControl(2, "Nades", this, &FiltersNades);

	FiltersC4.SetFileId("ftr_c4");
	OptionsGroup.PlaceLabledControl(2, "C4", this, &FiltersC4);

	//	show_hostage.SetFileId("ftr_hostage");
	//	OptionsGroup.PlaceLabledControl(2, "Hostage", this, &show_hostage);

	optimize.SetFileId("ftr_optimize");
	OptionsGroup.PlaceLabledControl(2, "Optimize Graphics", this, &optimize);

	WeaponFilterName.SetFileId("ftr_weapon_toggle");
	OptionsGroup.PlaceLabledControl(2, "Dropped Weapon Name", this, &WeaponFilterName);

	FiltersWeapons.SetFileId("ftr_weaps");
	FiltersWeapons.AddItem("Off");
	FiltersWeapons.AddItem("Default");
	FiltersWeapons.AddItem("Genuine");
	FiltersWeapons.AddItem("Corners");
	OptionsGroup.PlaceLabledControl(2, "Dropped Weapon Box", this, &FiltersWeapons);

	fogui.SetFileId("ftr_fogui");
	OptionsGroup.PlaceLabledControl(2, "Open Fog UI", this, &fogui);
	//	asus_type.SetFileId("asus_wall_type");
	//	asus_type.AddItem("Props Only");
	//	asus_type.AddItem("Walls and Props");
	//	OptionsGroup.PlaceLabledControl(2, "Asus Type", this, &asus_type);


	//----------------------------------------------//


	SpreadCrosshair.SetFileId(XorStr("v_spreadcrosshair"));
	SpreadCrosshair.AddItem("Off");
	SpreadCrosshair.AddItem("Standard");
	SpreadCrosshair.AddItem("Colour");
	SpreadCrosshair.AddItem("Rainbow");
	SpreadCrosshair.AddItem("Rainbow Rotate");
	OptionsGroup.PlaceLabledControl(3, XorStr("Spread crosshair"), this, &SpreadCrosshair);

	/*	SpreadCrossSize.SetFileId("otr_spreadcross_size");
	SpreadCrossSize.SetBoundaries(1.f, 100.f); //we should take smth like 650 as max so i guess *6.5?
	SpreadCrossSize.extension = XorStr("%%");
	SpreadCrossSize.SetValue(45.f);
	OptionsGroup.PlaceLabledControl(3, "Size", this, &SpreadCrossSize); */

	crosshair.SetFileId("otr_crosshair");
	OptionsGroup.PlaceLabledControl(3, "Crosshair", this, &crosshair);

	//	DamageIndicator.SetFileId("otr_btracers");
	//	OptionsGroup.PlaceLabledControl(3, "Damage Indicator", this, &DamageIndicator);

	OtherNoScope.SetFileId("otr_noscope");
	OptionsGroup.PlaceLabledControl(3, "Remove scope", this, &OtherNoScope);

	RemoveZoom.SetFileId("otr_remv_zoom");
	OptionsGroup.PlaceLabledControl(3, "Remove zoom", this, &RemoveZoom);

	OtherNoFlash.SetFileId("otr_noflash");
	OptionsGroup.PlaceLabledControl(3, "Remove flash effect", this, &OtherNoFlash);

	flashAlpha.SetFileId("otr_stolen_from_punknown_muahahaha");
	flashAlpha.SetBoundaries(0, 100);
	flashAlpha.extension = XorStr("%%");
	flashAlpha.SetValue(10);
	OptionsGroup.PlaceLabledControl(3, "Flash Alpha", this, &flashAlpha);

	nosmoke.SetFileId("otr_nosmoke");
	OptionsGroup.PlaceLabledControl(3, "Remove smoke", this, &nosmoke);

	/*	nosmoke_slider.SetFileId("otr_nosmoke_alpha");
	nosmoke_slider.SetBoundaries(0, 100);
	nosmoke_slider.extension = ("%%");
	nosmoke_slider.SetValue(10);
	OptionsGroup.PlaceLabledControl(3, "Smoke Alpha", this, &nosmoke_slider); */


	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OptionsGroup.PlaceLabledControl(3, "No visual recoil", this, &OtherNoVisualRecoil);

	OtherViewmodelFOV.SetFileId("otr_viewfov");
	OtherViewmodelFOV.SetBoundaries(30.f, 120.f);
	OtherViewmodelFOV.SetValue(90.f);
	OptionsGroup.PlaceLabledControl(3, "Viewmodel fov", this, &OtherViewmodelFOV);

	OtherFOV.SetFileId("otr_fov");
	OtherFOV.SetBoundaries(0.f, 50.f);
	OtherFOV.SetValue(0.f);
	OptionsGroup.PlaceLabledControl(3, "Override fov", this, &OtherFOV);

	override_viewmodel.SetFileId("otr_override_viewmodel_offset");
	OptionsGroup.PlaceLabledControl(3, "Override Viewmodel Offset", this, &override_viewmodel);

	offset_x.SetFileId("otr_offset_x");
	offset_x.SetBoundaries(-12, 12);
	offset_x.SetValue(2.5);
	OptionsGroup.PlaceLabledControl(3, "Offset X", this, &offset_x);

	offset_y.SetFileId("otr_offset_Y");
	offset_y.SetBoundaries(-12, 12);
	offset_y.SetValue(2.0);
	OptionsGroup.PlaceLabledControl(3, "Offset Y", this, &offset_y);

	offset_z.SetFileId("otr_offset_z");
	offset_z.SetBoundaries(-12, 12);
	offset_z.SetValue(-2.0);
	OptionsGroup.PlaceLabledControl(3, "Offset z", this, &offset_z);


	/*
	beamtime.SetFileId("otr_beamtime");
	beamtime.SetBoundaries(1.f, 5.f);
	beamtime.SetValue(2.f);
	OptionsGroup.PlaceLabledControl(4, "Beam Time", this, &beamtime);
	*/
	LBYIndicator.SetFileId("otr_LBYIndicator");
	LBYIndicator.AddItem("Off");
	LBYIndicator.AddItem("Classic");
	LBYIndicator.AddItem("Genuine");
	OptionsGroup.PlaceLabledControl(4, "LBY Indicator", this, &LBYIndicator);

	LCIndicator.SetFileId("otr_LCIndicator");
	LCIndicator.AddItem("Off");
	LCIndicator.AddItem("Classic");
	LCIndicator.AddItem("Genuine");
	OptionsGroup.PlaceLabledControl(4, "LagComp Indicator", this, &LCIndicator);

	FakeDuckIndicator.SetFileId("otr_FakeDuckIndicator");
	FakeDuckIndicator.AddItem("Off");
	FakeDuckIndicator.AddItem("Classic");
	FakeDuckIndicator.AddItem("Genuine");
	OptionsGroup.PlaceLabledControl(4, "FakeDuck Indicator", this, &FakeDuckIndicator);

	fake_indicator.SetFileId("otr_desync_indicator");
	fake_indicator.AddItem("Off");
	fake_indicator.AddItem("Classic");
	fake_indicator.AddItem("Genuine");
	OptionsGroup.PlaceLabledControl(4, "Desync Indicator", this, &fake_indicator);

	manualaa_type.SetFileId("manualaa");
	manualaa_type.AddItem("Off");
	manualaa_type.AddItem("Single Arrow");
	manualaa_type.AddItem("All Arrows");
	OptionsGroup.PlaceLabledControl(4, "Manual Indicator", this, &manualaa_type); // requested by: https://steamcommunity.com/id/9a-

	killfeed.SetFileId("otr_killfeed");
	OptionsGroup.PlaceLabledControl(4, "Preserve Killfeed", this, &killfeed); // requested by: https://steamcommunity.com/id/B1GN1GNN

//	cheatinfo.SetFileId("cheatinfox");
//	OptionsGroup.PlaceLabledControl(4, "Debug Info", this, &cheatinfo);

	//	CompRank.SetFileId("otr_reveal__rank");
	//	OptionsGroup.PlaceLabledControl(4, "Rank Reveal", this, &CompRank);

	OtherEntityGlow.SetFileId("otr_world_ent_glow");
	OptionsGroup.PlaceLabledControl(4, "World Entity Glow", this, &OtherEntityGlow);

	OtherHitmarker.SetFileId("otr_hitmarker");
	OptionsGroup.PlaceLabledControl(4, "Hitmarker", this, &OtherHitmarker);

	healthshot.SetFileId("otr_healthshot_onhit");
	OptionsGroup.PlaceLabledControl(4, "Disable Health Shot", this, &healthshot);

	watermark.SetFileId("otr_watermark");
	watermark.SetState(true);
	OptionsGroup.PlaceLabledControl(4, "Watermark", this, &watermark);

//	bulletbeam.SetFileId("otr_bullet_beam");
//	OptionsGroup.PlaceLabledControl(4, "Bullet Beam", this, &bulletbeam);

	logs.SetFileId("otr_skeetpaste");
	logs.AddItem("Off");
	logs.AddItem("Default");
	logs.AddItem("Coloured");
	OptionsGroup.PlaceLabledControl(4, "Event Log", this, &logs);

	/*	BulletTrace.SetFileId("otr_bullet_tracers_local");
	OptionsGroup.PlaceLabledControl(4, "Local Bullet Tracers", this, &BulletTrace);

	BulletTrace_enemy.SetFileId("otr_bullet_tracers_enemy");
	OptionsGroup.PlaceLabledControl(4, "Enemy Bullet Tracers", this, &BulletTrace_enemy); */
	//----------------------------------------------//

	ChamsGroup.SetText("Chams");
	ChamsGroup.AddTab(CGroupTab("Player", 1));
	ChamsGroup.AddTab(CGroupTab("Viewmodel", 2));
	ChamsGroup.AddTab(CGroupTab("Mods", 3));
	ChamsGroup.SetPosition(294, 30); // 225, 30
	ChamsGroup.SetSize(260, 243);
	RegisterControl(&ChamsGroup);


	ChamsEnemy.SetFileId("chams_enenmy_selection");
	ChamsEnemy.AddItem("Off");
	ChamsEnemy.AddItem("Visible Only");
	ChamsEnemy.AddItem("Always");
	ChamsGroup.PlaceLabledControl(1, "Enemies", this, &ChamsEnemy); // *1

	ChamsTeamVis.SetFileId("chams_team_selection");
	ChamsTeamVis.AddItem("Off");
	ChamsTeamVis.AddItem("Visible Only");
	ChamsTeamVis.AddItem("Always");
	ChamsGroup.PlaceLabledControl(1, "Team", this, &ChamsTeamVis);

	visible_chams_type.SetFileId("otr_visiblechams_type");
	visible_chams_type.AddItem("Normal");
	visible_chams_type.AddItem("Flat"); // like my ex :)
	visible_chams_type.AddItem("Pulse");
	visible_chams_type.AddItem("Crystal");
	visible_chams_type.AddItem("Glass");
	visible_chams_type.AddItem("ArmsRace");
	visible_chams_type.AddItem("FBI");
	visible_chams_type.AddItem("Fishnet");
	visible_chams_type.AddItem("Metallic");
	visible_chams_type.AddItem("Shine");
//	visible_chams_type.AddItem("Overlay");

	ChamsGroup.PlaceLabledControl(1, "Visible Chams Type", this, &visible_chams_type);

	invisible_chams_type.SetFileId("otr_invisiblechams_type");
	invisible_chams_type.AddItem("Normal");
	invisible_chams_type.AddItem("Flat"); // i'm still sad about that :(
	invisible_chams_type.AddItem("Pulse");
	invisible_chams_type.AddItem("Crystal");
	invisible_chams_type.AddItem("Glass");
	invisible_chams_type.AddItem("ArmsRace");
	invisible_chams_type.AddItem("FBI");
	invisible_chams_type.AddItem("Fishnet");
	invisible_chams_type.AddItem("Metallic");
	invisible_chams_type.AddItem("Shine");

	ChamsGroup.PlaceLabledControl(1, "Invisible Chams Type", this, &invisible_chams_type);

	ChamsLocal.SetFileId("chams_local");
	ChamsGroup.PlaceLabledControl(1, "Apply Local Chams", this, &ChamsLocal);

//	fakelag_ghost.SetFileId("chams_desyncyaw");
//	ChamsGroup.PlaceLabledControl(1, "Server Angle Ghost", this, &fakelag_ghost);

	localmaterial.SetFileId("esp_localscopedmat");
	localmaterial.AddItem("Default");
	localmaterial.AddItem("Clear");
	localmaterial.AddItem("Cham");
	localmaterial.AddItem("Wireframe");
	ChamsGroup.PlaceLabledControl(2, "Scoped Materials", this, &localmaterial);

	transparency.SetFileId("esp_transparency");
	transparency.SetBoundaries(0, 100);
	transparency.SetValue(20);
	transparency.extension = XorStr("%%");
	ChamsGroup.PlaceLabledControl(2, "Scoped Transparency", this, &transparency);

	HandCHAMS.SetFileId("chams_local_hand");
	HandCHAMS.AddItem("off");
	HandCHAMS.AddItem("Simple");
	HandCHAMS.AddItem("Wireframe");
	HandCHAMS.AddItem("Golden");
	HandCHAMS.AddItem("Glass");
	HandCHAMS.AddItem("Crystal");
	HandCHAMS.AddItem("Pulse");
	HandCHAMS.AddItem("Shiny");
	HandCHAMS.AddItem("Metallic");
	ChamsGroup.PlaceLabledControl(2, "Hand Chams", this, &HandCHAMS);

	GunCHAMS.SetFileId("chams_local_weapon");
	GunCHAMS.AddItem("off");
	GunCHAMS.AddItem("Simple");
	GunCHAMS.AddItem("Wireframe");
	GunCHAMS.AddItem("Golden");
	GunCHAMS.AddItem("Glass");
	GunCHAMS.AddItem("Crystal");
	GunCHAMS.AddItem("Pulse");
	GunCHAMS.AddItem("Shiny");
	GunCHAMS.AddItem("Metallic");
	ChamsGroup.PlaceLabledControl(2, "Weapon Chams", this, &GunCHAMS);

	SleeveChams.SetFileId("remove_Sleeve");
	SleeveChams.AddItem("off");
	SleeveChams.AddItem("Simple");
	SleeveChams.AddItem("Wireframe");
	SleeveChams.AddItem("Golden");
	SleeveChams.AddItem("Glass");
	SleeveChams.AddItem("Crystal");
	SleeveChams.AddItem("Pulse");
	SleeveChams.AddItem("Shiny");
	SleeveChams.AddItem("Metallic");
	ChamsGroup.PlaceLabledControl(2, "Sleeve Chams", this, &SleeveChams);

	/*
	fakelag_ghost.SetFileId("otr_fakelag_ghost");
	fakelag_ghost.AddItem("Off");
	fakelag_ghost.AddItem("Default");
	fakelag_ghost.AddItem("Pulse");
	ChamsGroup.PlaceLabledControl(2, "Fake Lag Ghost", this, &fakelag_ghost);
	*/


	hand_transparency.SetFileId("esp_hand_transparency");
	hand_transparency.SetBoundaries(0, 100);
	hand_transparency.SetValue(20);
	hand_transparency.extension = XorStr("%%");
	ChamsGroup.PlaceLabledControl(3, "Arm Transparency", this, &hand_transparency);

	gun_transparency.SetFileId("esp_gun_transparency");
	gun_transparency.SetBoundaries(0, 100);
	gun_transparency.SetValue(20);
	gun_transparency.extension = XorStr("%%");
	ChamsGroup.PlaceLabledControl(3, "Gun Transparency", this, &gun_transparency);

	sleeve_transparency.SetFileId("esp_sleeve_transparency");
	sleeve_transparency.SetBoundaries(0, 100);
	sleeve_transparency.SetValue(20);
	sleeve_transparency.extension = XorStr("%%");
	ChamsGroup.PlaceLabledControl(3, "Sleeve Transparency", this, &sleeve_transparency);

	visible_transparency.SetFileId("esp_visible_transparency");
	visible_transparency.SetBoundaries(0, 100);
	visible_transparency.SetValue(100);
	visible_transparency.extension = XorStr("%%");
	ChamsGroup.PlaceLabledControl(3, "Visible Transparency", this, &visible_transparency);

	invisible_transparency.SetFileId("esp_invisible_transparency");
	invisible_transparency.SetBoundaries(0, 100);
	invisible_transparency.SetValue(85);
	invisible_transparency.extension = XorStr("%%");
	ChamsGroup.PlaceLabledControl(3, "Invisible Transparency", this, &invisible_transparency);

	blend_local.SetFileId("esp_teamblend");
	blend_local.SetBoundaries(0, 100);
	blend_local.SetValue(75);
	blend_local.extension = XorStr("%%");
	ChamsGroup.PlaceLabledControl(3, "Local Player Chams Blend", this, &blend_local);

	worldgroup.SetText("World");
	worldgroup.AddTab(CGroupTab("Main", 1));
	worldgroup.AddTab(CGroupTab("Sky", 2));
	worldgroup.SetPosition(294, 280);  
	worldgroup.SetSize(260, 162);
	RegisterControl(&worldgroup);
	colmodupdate.SetFileId("otr_night");
	worldgroup.PlaceLabledControl(1, "Force update Materials", this, &colmodupdate); //you could've just made this a button lol

	customskies.SetFileId("otr_skycustom");
	customskies.AddItem("Default");
	customskies.AddItem("Night");
	customskies.AddItem("NoSky");
	customskies.AddItem("Galaxy");

	worldgroup.PlaceLabledControl(1, "Change Sky", this, &customskies);

	colmod.SetFileId("night_amm");
	colmod.SetBoundaries(000.000f, 100.00f);
	colmod.extension = XorStr("%%");
	colmod.SetValue(020.0f);
	worldgroup.PlaceLabledControl(1, "Brightness percentage", this, &colmod);

	asusamount.SetFileId("otr_asusprops");
	asusamount.SetBoundaries(1.f, 100.f);
	asusamount.extension = XorStr("%%");
	asusamount.SetValue(95.f);
	worldgroup.PlaceLabledControl(1, "Asus percantage", this, &asusamount);

	ModulateSkyBox.SetFileId(XorStr("sky_box_color_enable"));
	worldgroup.PlaceLabledControl(2, XorStr("Enable Sky Color Changer"), this, &ModulateSkyBox);

	sky_r.SetFileId("sky_r");
	sky_r.SetBoundaries(0.f, 25.f);
	sky_r.SetValue(10.f);
	worldgroup.PlaceLabledControl(2, "Sky: Red", this, &sky_r);

	sky_g.SetFileId("sky_g");
	sky_g.SetBoundaries(0.f, 25.f);
	sky_g.SetValue(1.f);
	worldgroup.PlaceLabledControl(2, "Sky: Green", this, &sky_g);

	sky_b.SetFileId("sky_b");
	sky_b.SetBoundaries(0.f, 25.f);
	sky_b.SetValue(20.f);
	worldgroup.PlaceLabledControl(2, "Sky: Blue", this, &sky_b);

#pragma endregion Setting up the Other controls
}

void CMiscTab::Setup()
{
	/*
	__  __ _____  _____  _____
	|  \/  |_   _|/ ____|/ ____|
	| \  / | | | | (___ | |
	| |\/| | | |  \___ \| |
	| |  | |_| |_ ____) | |____
	|_|  |_|_____|_____/ \_____|


	*/
	SetTitle("C");
#pragma region Other

	OtherGroup.SetText("Other");
	OtherGroup.AddTab(CGroupTab("Main", 1));
	OtherGroup.AddTab(CGroupTab("BuyBot", 2));
	OtherGroup.AddTab(CGroupTab("Models", 3));
	OtherGroup.AddTab(CGroupTab("Meme Opt.", 4));
	OtherGroup.SetPosition(4, 30);
	OtherGroup.SetSize(270, 400);
	RegisterControl(&OtherGroup);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.AddItem("Default");
//	OtherSafeMode.AddItem("Anti VAC Kick");
//	OtherSafeMode.AddItem("Danger Zone");
	OtherSafeMode.AddItem("Unrestricted (!)");
	OtherGroup.PlaceLabledControl(1, "Safety Mode", this, &OtherSafeMode);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherGroup.PlaceLabledControl(1, "BunnyHop", this, &OtherAutoJump);

	OtherAutoStrafe.SetFileId("otr_strafe");
	OtherGroup.PlaceLabledControl(1, "Air Strafe", this, &OtherAutoStrafe);

	hitmarker_sound.SetFileId("hitmarker_sound");
	hitmarker_sound.AddItem("Off");
	hitmarker_sound.AddItem("Cod");
	hitmarker_sound.AddItem("ArenaSwitch");
	hitmarker_sound.AddItem("Bubble");
	hitmarker_sound.AddItem("Bameware");
	hitmarker_sound.AddItem("Anime");
	hitmarker_sound.AddItem("Hitler"); // this is for CruZZ and Crytec. Heil hitler
	hitmarker_sound.AddItem("Bell 1"); // diiiing	
	hitmarker_sound.AddItem("Bell 2");		
	hitmarker_sound.AddItem("Button");
	hitmarker_sound.AddItem("Light");
	hitmarker_sound.AddItem("Chain");
	hitmarker_sound.AddItem("User Custom (csgo sound folder)");
	OtherGroup.PlaceLabledControl(1, "Hitmarker Sound", this, &hitmarker_sound);

	OtherThirdperson.SetFileId("aa_1thirpbind");
	OtherGroup.PlaceLabledControl(1, "Thirdperson", this, &OtherThirdperson);
	ThirdPersonKeyBind.SetFileId("aa_thirpbind");
	OtherGroup.PlaceLabledControl(1, "", this, &ThirdPersonKeyBind);

	Radar.SetFileId("Radar");
	OtherGroup.PlaceLabledControl(1, "Draw Radar", this, &Radar);

	RadarX.SetFileId("misc_radar_xax1");
	RadarX.SetBoundaries(0, 1920);
	RadarX.SetValue(0);
	OtherGroup.PlaceLabledControl(1, "X-Axis", this, &RadarX);

	RadarY.SetFileId("misc_radar_yax2");
	RadarY.SetBoundaries(0, 1080);
	RadarY.SetValue(0);
	OtherGroup.PlaceLabledControl(1, "Y-Axis", this, &RadarY);

	autojump_type.SetFileId("misc_autojump_type");
	autojump_type.AddItem("Normal");
	autojump_type.AddItem("Always On");
	OtherGroup.PlaceLabledControl(1, "BunnyHop Type", this, &autojump_type);

	killsay.SetFileId("misc_killsay");
	killsay.items.push_back(dropdownboxitem(false, XorStr("On Kill")));
	killsay.items.push_back(dropdownboxitem(false, XorStr("On Death")));
	OtherGroup.PlaceLabledControl(1, "Trash Talk", this, &killsay);

	NameChanger.SetFileId("misc_NameChanger");
	OtherGroup.PlaceLabledControl(1, "Name Stealer", this, &NameChanger);

	infinite_duck.SetFileId("infinteduck");
	OtherGroup.PlaceLabledControl(1, "No Crouch Delay", this, &infinite_duck);

	buybot_primary.SetFileId("buybot_primary");
	buybot_primary.AddItem("Off");
	buybot_primary.AddItem("Auto-Sniper");
	buybot_primary.AddItem("Scout (ssg08)");
	buybot_primary.AddItem("Awp");
	buybot_primary.AddItem("Ak-47 / M4");
	buybot_primary.AddItem("Aug / Sg553");
	buybot_primary.AddItem("Mp9 / Mac10");
	OtherGroup.PlaceLabledControl(2, "Primary Weapon", this, &buybot_primary);

	buybot_secondary.SetFileId("buybot_secondary");
	buybot_secondary.AddItem("Off");
	buybot_secondary.AddItem("Dual Berretas");
	buybot_secondary.AddItem("Revolver / Desert Eagle");
	buybot_secondary.AddItem("Five-Seven / Cz75");
	OtherGroup.PlaceLabledControl(2, "Secondary Weapon", this, &buybot_secondary);

	buybot_otr.SetFileId("buybot_other");
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("kevlar")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("he-grenade")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("flashbang")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("smoke grenade")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("molotov")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("zeus")));
	buybot_otr.items.push_back(dropdownboxitem(false, XorStr("defuse-kit")));
	OtherGroup.PlaceLabledControl(2, "Others", this, &buybot_otr);

	model.SetFileId("misc_modelchanger");
	model.AddItem("default");
	model.AddItem("Agent Ava");
	model.AddItem("Operator");
	model.AddItem("Markus Delrow");
	model.AddItem("Michael Syfers");
	model.AddItem("Markus Delrow");
	model.AddItem("Officer");
	model.AddItem("Seal Team 6");
	model.AddItem("Buckshot");
	model.AddItem("Ricksaw");
	model.AddItem("Commando");
	model.AddItem("McCoy");
	model.AddItem("Dragomir");
	model.AddItem("Rezan");
	model.AddItem("Romanov");
	model.AddItem("Maximus");
	model.AddItem("Blackwolf");
	model.AddItem("Mr. Muhlik");
	model.AddItem("Ground Rebel");
	model.AddItem("Osiris");
	model.AddItem("Prof. Shahmat");
	model.AddItem("Enforcer");
	model.AddItem("Slingshot");
	model.AddItem("Phoenix Soldier");
	OtherGroup.PlaceLabledControl(3, "Model Options", this, &model);

	excuses.SetFileId("misc_meme_excuses");
	OtherGroup.PlaceLabledControl(4, "Log Death Reason", this, &excuses);

	onetapsu.SetFileId("misc_meme_onetap_cantmiss");
	OtherGroup.PlaceLabledControl(4, "Can't Miss If Can't Shoot", this, &onetapsu);

	muslims.SetFileId("misc_meme_muslims");
	OtherGroup.PlaceLabledControl(4, "Muslim Detection", this, &muslims);

	plague.SetFileId("misc_meme_plague");
	OtherGroup.PlaceLabledControl(4, "Wireframe Blood", this, &plague);

	banana.SetFileId("misc_meme_banana");
	OtherGroup.PlaceLabledControl(4, "Banana Indicator", this, &banana);
	// - next subtab

	AntiAimGroup.SetPosition(286, 30);
	AntiAimGroup.SetText("Anti-Aim");
	AntiAimGroup.SetSize(270, 400);
	AntiAimGroup.AddTab(CGroupTab("Main", 1));
	AntiAimGroup.AddTab(CGroupTab("Modifiers", 2));
	AntiAimGroup.AddTab(CGroupTab("Misc", 3));
	AntiAimGroup.AddTab(CGroupTab("Desync", 4));
	RegisterControl(&AntiAimGroup);
	AntiAimEnable.SetFileId("aa_enable");
	AntiAimGroup.PlaceLabledControl(1, "Enable", this, &AntiAimEnable);

	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("Off");
	AntiAimPitch.AddItem("Down");
	AntiAimPitch.AddItem("Up");
	AntiAimPitch.AddItem("Fake Up (!)");
	AntiAimPitch.AddItem("Random");
	AntiAimGroup.PlaceLabledControl(1, "Pitch", this, &AntiAimPitch);

	antiaim_stand.SetFileId("aa_y");
	antiaim_stand.AddItem("Off");
	antiaim_stand.AddItem("Backward");
	antiaim_stand.AddItem("Manual");
	antiaim_stand.AddItem("Freestanding");
	antiaim_stand.AddItem("Estrogen");
	AntiAimGroup.PlaceLabledControl(1, "Standing Yaw", this, &antiaim_stand);

	stand_jitter.SetFileId("c_addjitter_stand");
	stand_jitter.SetBoundaries(-90.f, 90.f);
	stand_jitter.SetValue(0.f);
	AntiAimGroup.PlaceLabledControl(1, "Modifier", this, &stand_jitter);

	antiaim_move.SetFileId("aa_y2");
	antiaim_move.AddItem("Off");
	antiaim_move.AddItem("Backward");
	antiaim_move.AddItem("Manual");
	antiaim_move.AddItem("Freestanding");
	antiaim_move.AddItem("Estrogen");
	AntiAimGroup.PlaceLabledControl(1, "Moving Yaw", this, &antiaim_move);

	move_jitter.SetFileId("c_addjitter_move");
	move_jitter.SetBoundaries(-90.f, 90.f);
	move_jitter.SetValue(0.f);
	AntiAimGroup.PlaceLabledControl(1, "Modifier", this, &move_jitter);

	antiaim_air.SetFileId("aa_y3");
	antiaim_air.AddItem("Off");
	antiaim_air.AddItem("Backward");
	antiaim_air.AddItem("Manual");
	antiaim_air.AddItem("Freestanding");	
	antiaim_air.AddItem("Estrogen");
	AntiAimGroup.PlaceLabledControl(1, "InAir Yaw", this, &antiaim_air);


	minimal_walk.SetFileId("minimal_walk");
	AntiAimGroup.PlaceLabledControl(1, "Minimal Walk Key", this, &minimal_walk);

	at_targets.SetFileId("at_targets");
	AntiAimGroup.PlaceLabledControl(1, "At Targets", this, &at_targets);

	disable_on_dormant.SetFileId("disable_on_dormant");
	AntiAimGroup.PlaceLabledControl(1, "Disable On Dormant", this, &disable_on_dormant);

	fakeangle_compaitibility.SetFileId("desyncfakeangle_compaitibility");
	AntiAimGroup.PlaceLabledControl(1, "Fake Angle Plugin Compatibility", this, &fakeangle_compaitibility);

	//	BreakLBYDelta2.SetFileId("b_antilby2");
	//	BreakLBYDelta2.SetBoundaries(-180, 180);
	//	BreakLBYDelta2.SetValue(90);
	//	AntiAimGroup.PlaceLabledControl(2, "Anti-LBY First Flick", this, &BreakLBYDelta2);

	//	BreakLBYDelta.SetFileId("b_antilby");
	//	BreakLBYDelta.SetBoundaries(-180, 180);
	//	BreakLBYDelta.SetValue(-90);
	//	AntiAimGroup.PlaceLabledControl(1, "Anti-LBY Range", this, &BreakLBYDelta);

	//	freerange.SetFileId("freestanding_range");
	//	freerange.SetBoundaries(0, 90);
	//	freerange.SetValue(35);
	//	AntiAimGroup.PlaceLabledControl(1, "Freestanding Value", this, &freerange);

	//preset_aa.SetFileId("preset_aa");
	//AntiAimGroup.PlaceLabledControl(1, "Pre-set AntiAim", this, &preset_aa);

	//choked_shot.SetFileId("choke_shot");
	//AntiAimGroup.PlaceLabledControl(1, "Choke Shot", this, &choked_shot);
	//-<------------------------------------->-//

	fakelag_factor.SetFileId("fakelag_fac_val");
	fakelag_factor.SetBoundaries(1, 14);
	fakelag_factor.SetValue(1);
	AntiAimGroup.PlaceLabledControl(2, "Fakelag Factor", this, &fakelag_factor);

	/*
	FakelagMove.SetFileId("fakelag_move_val");
	FakelagMove.SetBoundaries(1, 14);
	FakelagMove.SetValue(1);
	AntiAimGroup.PlaceLabledControl(2, "Fakelag Moving", this, &FakelagMove);

	Fakelagjump.SetFileId("fakelag_jump_val");
	Fakelagjump.SetBoundaries(1, 14);
	Fakelagjump.SetValue(1);
	AntiAimGroup.PlaceLabledControl(2, "Fakelag in Air", this, &Fakelagjump);
	*/

	fl_spike.SetFileId("fakelag_spike");
	fl_spike.AddItem("Default");
	fl_spike.AddItem("Enemy Sight");
	fl_spike.AddItem("Mirror Adaptive");
	fl_spike.AddItem("Aimware Adaptive");
	fl_spike.AddItem("Velocity");
	fl_spike.AddItem("Mirror New Adaptive");
	fl_spike.AddItem("Minimal");
	AntiAimGroup.PlaceLabledControl(2, "Fakelag Type", this, &fl_spike);

	fakelag_key.SetFileId("fakelag_onkey");
	AntiAimGroup.PlaceLabledControl(2, "Fakelag Spike Key", this, &fakelag_key);

	FakelagBreakLC.SetFileId("fakelag_breaklc");
	AntiAimGroup.PlaceLabledControl(2, "Break Lag Compensation", this, &FakelagBreakLC);

	modifier.SetFileId("aa_modifier");
	modifier.AddItem("Add");
	modifier.AddItem("Random");
	modifier.AddItem("Spin");
	modifier.AddItem("Jitter One Side");
	modifier.AddItem("Jitter Two Sides");
	AntiAimGroup.PlaceLabledControl(2, "Yaw Modifier", this, &modifier);

	manual_right.SetFileId("otr_keybasedleft");
	AntiAimGroup.PlaceLabledControl(3, "Manual Right", this, &manual_right);

	manual_left.SetFileId("otr_keybasedright");
	AntiAimGroup.PlaceLabledControl(3, "Manual Left", this, &manual_left);

	manualback.SetFileId("otr_keybasedback");
	AntiAimGroup.PlaceLabledControl(3, "Manual Back", this, &manualback);

	manualfront.SetFileId("otr_manualfrontk");
	AntiAimGroup.PlaceLabledControl(3, "Manual Front", this, &manualfront);


	fake_crouch.SetFileId("fake_crouch");
	AntiAimGroup.PlaceLabledControl(3, "Fake Crouch", this, &fake_crouch);

	fake_crouch_key.SetFileId("fake_crouch_key");
	AntiAimGroup.PlaceLabledControl(3, "Fake Crouch Key", this, &fake_crouch_key);	
	
	fake_crouch_view.SetFileId("fake_crouch_view");
	AntiAimGroup.PlaceLabledControl(3, "Fake Crouch View Fix", this, &fake_crouch_view);

	fake_crouch_fakelag.SetFileId("fake_crouch_fakelag");
	fake_crouch_fakelag.SetBoundaries(10.f, 14.f);
	fake_crouch_fakelag.SetValue(14.f);
	AntiAimGroup.PlaceLabledControl(3, "Fake Crouch Lag Factor", this, &fake_crouch_fakelag);

	QuickStop.SetFileId("acc_quickstop");
	QuickStop.AddItem("Off");
	QuickStop.AddItem("Default");
	QuickStop.AddItem("Slow Walk");
	AntiAimGroup.PlaceLabledControl(3, "Quickstop", this, &QuickStop);

	QuickCrouch.SetFileId("acc_quickcrouch");
	AntiAimGroup.PlaceLabledControl(3, "Quickcrouch", this, &QuickCrouch);
	//-<------------------------------------->-//

	desync_type_stand.SetFileId("desync_type_stand");
	desync_type_stand.AddItem("Off");
	desync_type_stand.AddItem("Static");
	desync_type_stand.AddItem("Random");
	desync_type_stand.AddItem("Jitter");
	AntiAimGroup.PlaceLabledControl(4, "Standing Type", this, &desync_type_stand);

	desync_range_stand.SetFileId("desync_range_standing"); //	mindmg_override.extension = XorStr("HP");
	desync_range_stand.SetBoundaries(0, 100);
	desync_range_stand.extension = XorStr("%%");
	desync_range_stand.SetValue(40);
	AntiAimGroup.PlaceLabledControl(4, "Desync Amount", this, &desync_range_stand);

	//	desync_swapsides_stand.SetFileId("desync_swapsides_stand");
	//	AntiAimGroup.PlaceLabledControl(4, "Swap Sides", this, &desync_swapsides_stand);

	desync_type_move.SetFileId("desync_type_moving");
	desync_type_move.AddItem("Off");
	desync_type_move.AddItem("Static");
	desync_type_move.AddItem("Random");
	desync_type_move.AddItem("Jitter");
	AntiAimGroup.PlaceLabledControl(4, "Moving Type", this, &desync_type_move);

	desync_range_move.SetFileId("desync_range_move");
	desync_range_move.SetBoundaries(0, 100);
	desync_range_move.extension = XorStr("%%");
	desync_range_move.SetValue(40);
	AntiAimGroup.PlaceLabledControl(4, "Desync Amount", this, &desync_range_move);

	/*
	desync_swap.SetFileId("desync_swapside");
	desync_swap.AddItem("Off");
	desync_swap.AddItem("Enemy Shot");
	desync_swap.AddItem("Client Shot");
//	desync_swap.AddItem("Both");
	AntiAimGroup.PlaceLabledControl(4, "Swap Side", this, &desync_swap);
	*/
/*
	desync_old.SetFileId("desync_old");
	AntiAimGroup.PlaceLabledControl(4, "Old Desync Method", this, &desync_old);

	desyncleft.SetFileId("desync_key_left");
	AntiAimGroup.PlaceLabledControl(4, "Desync Side Left", this, &desyncleft);

	desyncright.SetFileId("desync_key_right");
	AntiAimGroup.PlaceLabledControl(4, "Desync Side Right", this, &desyncright);
*/
//	extend.SetFileId("otr_extend_moving_desync");
//	AntiAimGroup.PlaceLabledControl(4, "Extend Moving Desync", this, &extend);

	antilby.SetFileId("otr_meh");
	antilby.AddItem("Off");
	antilby.AddItem("Break Lowerbody");
	antilby.AddItem("Microwalk");
	AntiAimGroup.PlaceLabledControl(4, "Anti Balance", this, &antilby);

	antilbymod.SetFileId("otr_antilbymod");
	antilbymod.SetBoundaries(1.f, 100.f);
	antilbymod.SetValue(20.f);
	antilbymod.extension = XorStr("%%");
	AntiAimGroup.PlaceLabledControl(4, "Anti Balance", this, &antilbymod);


	backup_aa_fac.SetFileId("backup_aa_factors");
	backup_aa_fac.items.push_back(dropdownboxitem(false, XorStr("key")));
	backup_aa_fac.items.push_back(dropdownboxitem(false, XorStr("duck")));
	backup_aa_fac.items.push_back(dropdownboxitem(false, XorStr("un-duck")));
	backup_aa_fac.items.push_back(dropdownboxitem(false, XorStr("peek")));
	backup_aa_fac.items.push_back(dropdownboxitem(false, XorStr("always on")));
	AntiAimGroup.PlaceLabledControl(4, "Backup AA Factors (Manual)", this, &backup_aa_fac);

	backup_aa.SetFileId("backup_aa");
	AntiAimGroup.PlaceLabledControl(4, "Backup Anti-Aim Key", this, &backup_aa);

	real_align.SetFileId("backup_real_align");
	real_align.SetBoundaries(5.f, 9.f);
	real_align.SetValue(5.f);
	AntiAimGroup.PlaceLabledControl(4, "Backup Align Multiplier", this, &real_align); 

//	showdesync.SetFileId("desync_showdesync");
//	AntiAimGroup.PlaceLabledControl(4, "False Packet Model", this, &showdesync);
}

void CSkinTab::Setup()
{
	SetTitle("B");
	knifegroup.SetText("someone please fix this for me it's 4 am, i cba");
	knifegroup.SetPosition(4, 30);
	//	knifegroup.AddTab(CGroupTab("Terrorist", 1));
	//	knifegroup.AddTab(CGroupTab("Counter-Terrorist", 2));
	knifegroup.SetSize(270, 170);
	RegisterControl(&knifegroup);

#pragma region Terrorist ( to be replaced with team based stuff )
	/*
	t_knife_index.SetFileId("t_knife_index");
	t_knife_index.AddItem("Off");
	t_knife_index.AddItem("Bayonet");
	t_knife_index.AddItem("M9 Bayonet");
	t_knife_index.AddItem("Butterfly");
	t_knife_index.AddItem("Flip");
	t_knife_index.AddItem("Gut");
	t_knife_index.AddItem("Karambit");
	t_knife_index.AddItem("Huntsman");
	t_knife_index.AddItem("Falchion");
	t_knife_index.AddItem("Bowie");
	t_knife_index.AddItem("Shadow");
	t_knife_index.AddItem("Talon");
	t_knife_index.AddItem("Stiletto");
	knifegroup.PlaceLabledControl(0, XorStr("Model"), this, &t_knife_index);

	t_knife_wear.SetFileId("t_knife_seed");
	t_knife_wear.SetBoundaries(1, 100);
	t_knife_wear.SetValue(1);
	t_knife_wear.extension = XorStr("%%");
	knifegroup.PlaceLabledControl(0, XorStr("Wear"), this, &t_knife_wear);

	t_knife_skin_id.SetFileId("t_knife_skin");
	t_knife_skin_id.AddItem("Default");
	t_knife_skin_id.AddItem("Ruby");
	t_knife_skin_id.AddItem("Sapphire");
	t_knife_skin_id.AddItem("Black Pearl");
	t_knife_skin_id.AddItem("Doppler");
	t_knife_skin_id.AddItem("Fade");
	t_knife_skin_id.AddItem("Marble Fade");
	t_knife_skin_id.AddItem("Gamma Doppler");
	t_knife_skin_id.AddItem("Emerald");
	t_knife_skin_id.AddItem("Slaughter");
	t_knife_skin_id.AddItem("Whiteout");
	t_knife_skin_id.AddItem("Ultraviolet");
	t_knife_skin_id.AddItem("Lore (M9)");
	knifegroup.PlaceLabledControl(0, XorStr("Skin"), this, &t_knife_skin_id);
	*/

	//-----------------
	/*
	snipergroup.SetText("Snipers");
	snipergroup.SetPosition(284, 30);
	snipergroup.AddTab(CGroupTab("Auto", 1));
	snipergroup.AddTab(CGroupTab("Bolt Action", 2));
	snipergroup.SetSize(270, 170);
	RegisterControl(&snipergroup);

	t_sniperSCAR_skin_id.SetFileId("t_scar20_skin");
	t_sniperSCAR_skin_id.AddItem("Default");
	t_sniperSCAR_skin_id.AddItem("Crimson Web");
	t_sniperSCAR_skin_id.AddItem("Splash Jam");
	t_sniperSCAR_skin_id.AddItem("Emerald");
	t_sniperSCAR_skin_id.AddItem("Cardiac");
	t_sniperSCAR_skin_id.AddItem("Cyrex");
	t_sniperSCAR_skin_id.AddItem("Whiteout");
	t_sniperSCAR_skin_id.AddItem("The Fuschia Is Now");
	snipergroup.PlaceLabledControl(1, XorStr("Scar20"), this, &t_sniperSCAR_skin_id);

	t_sniperSCAR_wear.SetFileId("t_sniperSCAR_wear");
	t_sniperSCAR_wear.SetBoundaries(1, 100);
	t_sniperSCAR_wear.SetValue(1);
	t_sniperSCAR_wear.extension = XorStr("%%");
	snipergroup.PlaceLabledControl(1, XorStr("Wear"), this, &t_sniperSCAR_wear);

	// --



	// --

	t_sniperAWP_skin_id.SetFileId("t_AWP_skin");
	t_sniperAWP_skin_id.AddItem("Default");
	t_sniperAWP_skin_id.AddItem("Dragon Lore");
	t_sniperAWP_skin_id.AddItem("Pink DDPAT");
	t_sniperAWP_skin_id.AddItem("Asiimov");
	t_sniperAWP_skin_id.AddItem("Redline");
	t_sniperAWP_skin_id.AddItem("Medusa");
	t_sniperAWP_skin_id.AddItem("Hyper Beast");
	t_sniperAWP_skin_id.AddItem("Whiteout");
	snipergroup.PlaceLabledControl(3, XorStr("Skin"), this, &t_sniperAWP_skin_id);

	t_sniperAWP_wear.SetFileId("t_sniperAWP_wear");
	t_sniperAWP_wear.SetBoundaries(1, 100);
	t_sniperAWP_wear.SetValue(1);
	t_sniperAWP_wear.extension = XorStr("%%");
	snipergroup.PlaceLabledControl(3, XorStr("Wear"), this, &t_sniperAWP_wear);
	*/
}

void CColorTab::Setup()
{
	SetTitle("G");
#pragma region Visual Colors
	ColorsGroup.SetText("Settings");
	ColorsGroup.SetPosition(4, 30);
	ColorsGroup.AddTab(CGroupTab("ESP", 1));
	ColorsGroup.AddTab(CGroupTab("ESP 2", 2));
	ColorsGroup.AddTab(CGroupTab("Menu 1", 3));
	ColorsGroup.AddTab(CGroupTab("Menu 2", 4));
	//	ColorsGroup.AddTab(CGroupTab("Misc", 6));
	ColorsGroup.SetSize(270, 335);
	RegisterControl(&ColorsGroup);

	/*---------------------- COL ----------------------*/
	/*---------------------- COL ----------------------*/
	/*---------------------- COL ----------------------*/

	NameCol.SetFileId(XorStr("player_espname_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Name"), this, &NameCol);

	BoxCol.SetFileId(XorStr("player_espbox_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Box"), this, &BoxCol);;

	Skeleton.SetFileId(XorStr("player_skeleton_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Skeleton Enemy"), this, &Skeleton);

	Skeletonteam.SetFileId(XorStr("player_skeletonteam_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Skeleton Team"), this, &Skeletonteam);

	GlowEnemy.SetFileId(XorStr("player_glowenemy_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Glow Enemy"), this, &GlowEnemy);

	GlowTeam.SetFileId(XorStr("player_glowteam_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Glow Team"), this, &GlowTeam);

	GlowOtherEnt.SetFileId(XorStr("player_glowother_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Glow World"), this, &GlowOtherEnt);

	GlowLocal.SetFileId(XorStr("player_glowlocal_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Glow Local"), this, &GlowLocal);

	Weapons.SetFileId(XorStr("player_weapons_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Weapons Enemy"), this, &Weapons);

	Weaponsteam.SetFileId(XorStr("player_weapons_color_team"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Weapons Team"), this, &Weaponsteam);

	Ammo.SetFileId(XorStr("player_ammo_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Ammo Enemy"), this, &Ammo);

	ChamsLocal.SetFileId(XorStr("player_chamslocal_color"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Local"), this, &ChamsLocal);
	ChamsEnemyVis.SetFileId(XorStr("player_chamsEVIS_color"));
	ChamsEnemyVis.SetColor(150, 200, 60, 255);
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Enemy"), this, &ChamsEnemyVis);
	ChamsEnemyNotVis.SetFileId(XorStr("player_chamsENVIS_color"));
	ChamsEnemyNotVis.SetColor(60, 120, 180, 178);
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Enemy (Behind Wall)"), this, &ChamsEnemyNotVis);

	ChamsTeamVis.SetFileId(XorStr("player_ChamsTeamVis"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Team"), this, &ChamsTeamVis);

	ChamsTeamNotVis.SetFileId(XorStr("player_ChamsTeamNotVis"));
	ColorsGroup.PlaceLabledControl(1, XorStr("Chams Team (Behind Wall)"), this, &ChamsTeamNotVis);

	//	Bullettracer.SetFileId(XorStr("player_beam_color"));
	//	ColorsGroup.PlaceLabledControl(2, XorStr("Bullet tracers"), this, &Bullettracer);

	scoped_c.SetFileId(XorStr("scope_c"));
	ColorsGroup.PlaceLabledControl(2, XorStr("Local Scoped Material Colour"), this, &scoped_c);

	misc_lagcomp.SetFileId(XorStr("misc_lagcomp"));
	misc_lagcomp.SetColor(250, 250, 250, 255);
	ColorsGroup.PlaceLabledControl(2, XorStr("Lag Compensation"), this, &misc_lagcomp);

	misc_lagcompBones.SetFileId(XorStr("misc_lagcompBones"));
	misc_lagcompBones.SetColor(250, 250, 250, 255);
	ColorsGroup.PlaceLabledControl(2, XorStr("Backtrack Bones"), this, &misc_lagcompBones);

	spreadcrosscol.SetFileId(XorStr("weapon_spreadcross_col"));
	ColorsGroup.PlaceLabledControl(2, XorStr("Spread Crosshair"), this, &spreadcrosscol);
	HandChamsCol.SetFileId(XorStr("handchams_col"));
	ColorsGroup.PlaceLabledControl(2, XorStr("Hand Chams"), this, &HandChamsCol);
	GunChamsCol.SetFileId(XorStr("gunchams_col"));
	ColorsGroup.PlaceLabledControl(2, XorStr("Weapon Chams"), this, &GunChamsCol);

//	bullet_tracer.SetFileId("beam_color");
//	ColorsGroup.PlaceLabledControl(2, XorStr("Bullet Impact"), this, &bullet_tracer);

	SleeveChams_col.SetFileId("player_chams_sleeves_color");
	ColorsGroup.PlaceLabledControl(2, XorStr("Sleeve Chams"), this, &SleeveChams_col);

	//	Offscreen.SetFileId(XorStr("player_offscreen_color"));
	//	ColorsGroup.PlaceLabledControl(2, XorStr("Offscreen"), this, &Offscreen);

	//	fakelag_ghost.SetFileId("player_fakelag_ghost");
	//	ColorsGroup.PlaceLabledControl(2, XorStr("Fakelag Ghost"), this, &fakelag_ghost);
	//---

	MenuBar.SetFileId(XorStr("menu_bar_mode"));
	MenuBar.AddItem("Static");
	MenuBar.AddItem("Animated");
	MenuBar.AddItem("Fade");
	ColorsGroup.PlaceLabledControl(3, XorStr("Menu Bar"), this, &MenuBar);
	outl_r.SetFileId("outlred");
	outl_r.SetBoundaries(0.f, 255.f);
	outl_r.SetValue(55.f);
	ColorsGroup.PlaceLabledControl(3, "Outer Left: Red", this, &outl_r);
	outl_g.SetFileId("outlgreen");
	outl_g.SetBoundaries(0.f, 255.f);
	outl_g.SetValue(15.f);
	ColorsGroup.PlaceLabledControl(3, "Outer Left: Green", this, &outl_g);
	outl_b.SetFileId("outlblue");
	outl_b.SetBoundaries(0.f, 255.f);
	outl_b.SetValue(210.f);
	ColorsGroup.PlaceLabledControl(3, "Outer Left: Blue", this, &outl_b);
	inl_r.SetFileId("inlred");
	inl_r.SetBoundaries(0.f, 255.f);
	inl_r.SetValue(185.f);
	ColorsGroup.PlaceLabledControl(3, "Inner Left: Red", this, &inl_r);
	inl_g.SetFileId("inlgreen");
	inl_g.SetBoundaries(0.f, 255.f);
	inl_g.SetValue(25.f);
	ColorsGroup.PlaceLabledControl(3, "Inner Left: Green", this, &inl_g);
	inl_b.SetFileId("inlblue");
	inl_b.SetBoundaries(0.f, 255.f);
	inl_b.SetValue(230.f);
	ColorsGroup.PlaceLabledControl(3, "Inner Left: Blue", this, &inl_b);
	inr_r.SetFileId("inrred");
	inr_r.SetBoundaries(0.f, 255.f);
	inr_r.SetValue(185.f);
	ColorsGroup.PlaceLabledControl(4, "Inner Right: Red", this, &inr_r);

	inr_g.SetFileId("inrgreen");
	inr_g.SetBoundaries(0.f, 255.f);
	inr_g.SetValue(25.f);
	ColorsGroup.PlaceLabledControl(4, "Inner Right: Green", this, &inr_g);

	inr_b.SetFileId("inrblue");
	inr_b.SetBoundaries(0.f, 255.f);
	inr_b.SetValue(230.f);
	ColorsGroup.PlaceLabledControl(4, "Inner Right: Blue", this, &inr_b);
	outr_r.SetFileId("outrred");
	outr_r.SetBoundaries(0.f, 255.f);
	outr_r.SetValue(55.f);
	ColorsGroup.PlaceLabledControl(4, "Outer Right: Red", this, &outr_r);
	outr_g.SetFileId("outrgreen");
	outr_g.SetBoundaries(0.f, 255.f);
	outr_g.SetValue(15.f);
	ColorsGroup.PlaceLabledControl(4, "Outer Right: Green", this, &outr_g);
	outr_b.SetFileId("outrblue");
	outr_b.SetBoundaries(0.f, 255.f);
	outr_b.SetValue(210.f);
	ColorsGroup.PlaceLabledControl(4, "Outer Right: Blue", this, &outr_b);

	bomb_timer.SetFileId(XorStr("bomb_timer"));
	bomb_timer.SetColor(250, 10, 90, 230);
	ColorsGroup.PlaceLabledControl(4, XorStr("Bomb Timer"), this, &bomb_timer);

	Menu.SetFileId(XorStr("menu_color"));
	Menu.SetColor(170, 20, 250, 255);
	ColorsGroup.PlaceLabledControl(4, XorStr("Controls"), this, &Menu);

	console_colour.SetFileId("eventlog_colour");
	ColorsGroup.PlaceLabledControl(4, XorStr("Event Log"), this, &console_colour);


	//	misc_backtrackchams.SetFileId(XorStr("misc_backtrackchams"));
	//	misc_backtrackchams.SetColor(250, 250, 250, 255);
	//	ColorsGroup.PlaceLabledControl(5, XorStr("Backtrack Chams"), this, &misc_backtrackchams);

	ConfigGroup.SetText("Configs");
	ConfigGroup.SetPosition(290, 30);
	ConfigGroup.SetSize(264, 335);
	RegisterControl(&ConfigGroup); ConfigListBox.SetHeightInItems(7);
	list_configs();
	ConfigGroup.PlaceLabledControl(0, XorStr(""), this, &ConfigListBox);
	LoadConfig.SetText(XorStr("Load"));
	LoadConfig.SetCallback(&load_callback);
	ConfigGroup.PlaceLabledControl(0, "", this, &LoadConfig);
	SaveConfig.SetText(XorStr("Save"));
	SaveConfig.SetCallback(&save_callback);
	ConfigGroup.PlaceLabledControl(0, "", this, &SaveConfig);
	RemoveConfig.SetText(XorStr("Remove"));
	RemoveConfig.SetCallback(&remove_config);
	ConfigGroup.PlaceLabledControl(0, "", this, &RemoveConfig);
	ConfigGroup.PlaceLabledControl(0, "", this, &NewConfigName);
	AddConfig.SetText(XorStr("Add"));
	AddConfig.SetCallback(&add_config);
	ConfigGroup.PlaceLabledControl(0, "", this, &AddConfig);

	toggle_aspectratio.SetFileId("aspectratio_toggle");
	ConfigGroup.PlaceLabledControl(0, "Change Aspect Ratio", this, &toggle_aspectratio);

	aspectratio.SetFileId("aspectratio_set");
	aspectratio.SetBoundaries(25, 275);
	aspectratio.SetValue(100);
	ConfigGroup.PlaceLabledControl(0, "Aspect Ratio Factor", this, &aspectratio);

	/*---------------------- OTHERS ----------------------*/
	/*---------------------- OTHERS ----------------------*/
	/*---------------------- OTHERS ----------------------*/

	OtherOptions.SetText("other");
	OtherOptions.SetPosition(4, 373);
	OtherOptions.SetSize(270, 70);
	RegisterControl(&OtherOptions);

	DebugLagComp.SetFileId(XorStr("lagcompensationyes"));
	OtherOptions.PlaceLabledControl(0, XorStr("Draw Lag Compensation"), this, &DebugLagComp);

	BackTrackBones2.SetFileId(XorStr("spookybonesOwOomg"));
	OtherOptions.PlaceLabledControl(0, XorStr("BackTrack Bones"), this, &BackTrackBones2);

	ClanTag.SetFileId("otr_clantg");
	//	ClanTag.AddItem("Off");
	//	ClanTag.AddItem("Default");
	//	ClanTag.AddItem("FreaK Rats Kids");
	OtherOptions.PlaceLabledControl(0, XorStr("Clan Tag"), this, &ClanTag); // requested by: https://steamcommunity.com/id/hitoridekun and https://steamcommunity.com/id/123x456x789

	//	quickstop_speed.SetFileId(XorStr("quickstop_speed"));
	//	quickstop_speed.SetBoundaries(1, 40);
	//	quickstop_speed.SetValue(30);
	//	OtherOptions.PlaceLabledControl(0, XorStr("Quick Stop Speed"), this, &quickstop_speed);
	//	BackTrackBones.SetFileId(XorStr("spookybonesOwO"));
	//	OtherOptions.PlaceLabledControl(0, XorStr("BackTrack Chams"), this, &BackTrackBones);
	// your fps will look beyond the gates of the next life and will raise their middle fingers for having caused their suicide

	OtherOptions2.SetText("other 2");
	OtherOptions2.SetPosition(290, 373);
	OtherOptions2.SetSize(264, 70);
	RegisterControl(&OtherOptions2);

	//	experimental_backtrack.SetFileId(XorStr("experimental_backtrack"));
	//	OtherOptions2.PlaceLabledControl(0, XorStr("Experimental Position Adjustment"), this, &experimental_backtrack);


	menu_backdrop.SetFileId("owo_backdrop");
	OtherOptions2.PlaceLabledControl(0, XorStr("Menu Backdrop"), this, &menu_backdrop);

	owo_slider.SetFileId("owo_slider");
	owo_slider.SetBoundaries(0, 100);
	owo_slider.SetValue(100);
	owo_slider.extension = XorStr("%%");
	OtherOptions2.PlaceLabledControl(0, XorStr("OwO"), this, &owo_slider);

	menu_healthshot.SetFileId("menu_healthshot");
	OtherOptions2.PlaceLabledControl(0, XorStr("Menu Toggle HealthShot"), this, &menu_healthshot);
}

void options::SetupMenu()
{
	menu.Setup();
	GUI.RegisterWindow(&menu);
	GUI.BindWindow(VK_INSERT, &menu);
}
void options::DoUIFrame()
{
	GUI.Update();
	GUI.Draw();
}

