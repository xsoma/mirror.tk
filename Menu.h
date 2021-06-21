#pragma once
#include "GUI.h"
#include "Controls.h"
class CAimbotTab : public CTab
{
public:
	void Setup();
	CLabel ActiveLabel;
	CCheckBox Active;
	CCheckBox freak_hvh_preset;
	CComboBoxYeti psilent;
	CGroupBox AimbotGroup;
	CCheckBox enable;
	CComboBoxYeti lag_pred;
	CCheckBox backtrack;
	CCheckBox AimbotAutoFire;
	CCheckBox rage_chokeshot;
	CCheckBox rage_silent;
	CCheckBox legit_trigger;
	CKeyBind legit_trigger_key;

	CComboBoxYeti AimbotResolver;
	CSlider AimbotFov;
	//-------------------------
	//--------------------------

	CCheckBox PreferBodyAim;
	CCheckBox AWPAtBody;
	CSlider BaimIfUnderXHealth;
	CSlider XShots;
	CCheckBox EnginePrediction;
	CCheckBox AimbotAimStep;
	CCheckBox pointscaleyes;
	CCheckBox Multienable;
	CSlider pointscaleval;
	CSlider Multival;
	CSlider Multival2;
	CSlider MultiVal3;
	CSlider Multival4;
	CCheckBox fakeduck_accuracy;
	CCheckBox nospread;
	CSlider AimbotAimStep2;
	CSlider shotlimit;
	CCheckBox AimbotKeyPress;

	CCheckBox baim_fake;
	CCheckBox baim_inair;
	CCheckBox baim_fakewalk;
	CCheckBox baim_lethal;
	CCheckBox baim_muslim;

	CKeyBind  AimbotKeyBind;
	CKeyBind  AimbotStopKey;
	CGroupBox TargetGroup;
	CComboBoxYeti TargetSelection;
	CCheckBox TargetFriendlyFire;
	CComboBoxYeti TargetHitbox;
	CComboBoxYeti TargetHitscan;
	CComboBoxYeti delay_shot;
	CCheckBox TargetMultipoint;
	CSlider   TargetPointscale;
	CSlider   baim;

	//--------------//
	CGroupBox weapongroup;

	CSlider hc_auto;
	CSlider hc_scout;
	CSlider hc_awp;
	CSlider hc_pistol;
	CSlider hc_smg;
	CSlider hc_otr;

	CSlider md_auto;
	CSlider md_scout;
	CSlider md_awp;
	CSlider md_pistol;
	CSlider md_smg;
	CSlider md_otr;

	CCheckBox automatic_cfg;

	// CComboBoxYeti target_auto;
	/*	CDropBox target_auto;
	CDropBox target_scout;
	CDropBox target_awp;
	CDropBox target_pistol;
	CDropBox target_smg;
	CDropBox target_otr;
	*/

	CComboBoxYeti target_auto;
	CComboBoxYeti target_scout;
	CComboBoxYeti target_awp;
	CComboBoxYeti target_pistol;
	CComboBoxYeti target_smg;
	CComboBoxYeti target_otr;

	CComboBoxYeti target_auto2;
	CComboBoxYeti target_scout2;
	CComboBoxYeti target_awp2;
	CComboBoxYeti target_pistol2;
	CComboBoxYeti target_smg2;
	CComboBoxYeti target_otr2;

	CCheckBox enemyhp_auto;
	CCheckBox enemyhp_scout;
	CCheckBox enemyhp_awp;
	CCheckBox enemyhp_pistol;

	CCheckBox min_damage_enemy_hp_awp;

	//--------------//

	CGroupBox AccuracyGroup;
	CCheckBox AccuracyRecoil;

	CCheckBox AccuracyAutoWall;
	CSlider AccuracyMinimumDamage;

	CKeyBind toggle_override;
	CSlider mindmg_override;

	CSlider lc_ticks;

	CCheckBox AccuracyAutoStop;

	CCheckBox AccuracyAutoCrouch;
	CCheckBox AccuracyAutoScope;
	CSlider   AccuracyHitchance;
	CCheckBox resolver_info;
	CComboBoxYeti resolver;

	CComboBoxYeti preso;

	CCheckBox ignorelimbs;
	CKeyBind bigbaim;
	CKeyBind doubletap;
	CComboBoxYeti extrapolation;
	CCheckBox onshot;
	CCheckBox blacklist_if_breaklc;
	CCheckBox lowfps;

	CDropBox prefer_head;
	CCheckBox toggledebug;
	CDropBox debug;
};
class CLegitBotTab : public CTab
{
public:
	void Setup();
	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;
	CGroupBox AimbotGroup;
	CCheckBox AimbotEnable;
	CComboBoxYeti aimbotfiremode;
	CCheckBox AimbotFriendlyFire;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CCheckBox AimbotSmokeCheck;
	CCheckBox BackTrack;
	CCheckBox legitresolver;
	CGroupBox TriggerGroup;
	CComboBoxYeti triggertype;
	CCheckBox TriggerEnable;
	CKeyBind  TriggerKeyBind;
	CSlider   TriggerDelay;
	CSlider   TriggerBurst;
	CSlider   TriggerBreak;
	CSlider TriggerRecoil; CSlider   TriggerHitChanceAmmount;
	CGroupBox TriggerFilterGroup;
	CCheckBox TriggerHead;
	CCheckBox TriggerChest;
	CCheckBox TriggerStomach;
	CCheckBox TriggerArms;
	CCheckBox TriggerLegs;
	CCheckBox TriggerTeammates;
	CCheckBox TriggerFlashCheck;
	CCheckBox TriggerSmokeCheck;
	CCheckBox aaenable;
	CComboBoxYeti aatyp;
	CComboBoxYeti aatyp2;
	CSlider aatyp3;
	CSlider aimhp;
	CSlider aafl;
	CSlider trighp;
	CGroupBox weapongroup;
	CSlider   WeaponMainSpeed;
	CSlider   WeaponMainFoV;
	CSlider WeaponMainRecoil;
	CComboBoxYeti WeaponMainHitbox;
	CSlider WeaponMainAimtime;
	CSlider WeaoponMainStartAimtime;
	CGroupBox WeaponPistGroup;
	CSlider   WeaponPistSpeed;
	CSlider   WeaponPistFoV;
	CSlider WeaponPistRecoil;
	CComboBoxYeti WeaponPistHitbox;
	CSlider WeaponPistAimtime;
	CSlider WeaoponPistStartAimtime;
	CGroupBox WeaponSnipGroup;
	CSlider   WeaponSnipSpeed;
	CSlider   WeaponSnipFoV;
	CSlider WeaponSnipRecoil;
	CComboBoxYeti WeaponSnipHitbox;
	CSlider WeaponSnipAimtime;
	CSlider WeaoponSnipStartAimtime;
	CGroupBox WeaponMpGroup;
	CSlider   WeaponMpSpeed;
	CSlider   WeaponMpFoV;
	CSlider WeaponMpRecoil;
	CComboBoxYeti WeaponMpHitbox;
	CSlider WeaponMpAimtime;
	CSlider WeaoponMpStartAimtime;
	CGroupBox WeaponShotgunGroup;
	CSlider   WeaponShotgunSpeed;
	CSlider   WeaponShotgunFoV;
	CSlider WeaponShotgunRecoil;
	CComboBoxYeti WeaponShotgunHitbox;
	CSlider WeaponShotgunAimtime;
	CSlider WeaoponShotgunStartAimtime;
	CGroupBox WeaponMGGroup;
	CSlider   WeaponMGSpeed;
	CSlider   WeaponMGFoV;
	CSlider WeaponMGRecoil;
	CComboBoxYeti WeaponMGHitbox;
	CSlider WeaponMGAimtime;
	CSlider WeaoponMGStartAimtime;




};
class CVisualTab : public CTab
{
public:
	void Setup();
	CLabel ActiveLabel;
	CCheckBox Active;
	CCheckBox visualize_desync;
	CSlider flashAlpha;
	CGroupBox OptionsGroup;
	CCheckBox OptionsWeapone;
	CComboBoxYeti OptionsBox;
	CComboBoxYeti OptionsName;
	CCheckBox HitmarkerSound;
	CCheckBox OtherHitmarker;
	CCheckBox healthshot;

	CCheckBox ChamsLocal;
	CCheckBox aa_helper;
	CComboBoxYeti ChamsEnemy;
	CComboBoxYeti helpermat;
	CCheckBox OffscreenESP;
	CComboBoxYeti chamstype;
	CColorSelector urdadyoufaggot;

	CSlider Glowz_lcl;
	CCheckBox BlendIfScoped;
	CSlider BlendValue;

	CComboBoxYeti logs;
	CComboBoxYeti OptionsChams;
	CComboBoxYeti OptionsArmor; CColorSelector armor_col;
	CComboBoxYeti OptionsHealth; CColorSelector health_col;
	CComboBoxYeti OptionsWeapon;
	CComboBoxYeti manualaa_type;
	CComboBoxYeti HandCHAMS;
	CComboBoxYeti GunCHAMS;
	CComboBoxYeti LBYIndicator;
	CComboBoxYeti LCIndicator;
	CComboBoxYeti fake_indicator;
	CComboBoxYeti FakeDuckIndicator;
	CComboBoxYeti OtherThirdpersonAngle;
	CComboBoxYeti WeaponChams;
	CComboBoxYeti visible_chams_type;
	CComboBoxYeti invisible_chams_type;
	CComboBoxYeti SleeveChams;
	CComboBoxYeti ChamsTeamVis;

	CCheckBox fakelag_ghost;
	CCheckBox Weapons;
	CCheckBox ChamsTeamNoVis;
	CCheckBox OptionsGlow_lcl;
	CCheckBox NoGlowIfScoped;
	CCheckBox NoChamsIfScoped;
	CCheckBox armorbar;
	CCheckBox OptionsInfo;
	CCheckBox BulletTrace;
	CCheckBox BulletTrace_enemy;
	CCheckBox ChamsVisibleOnly;
	CCheckBox Ammo;
	CCheckBox GrenadePrediction;
	CCheckBox LocalPlayerESP;
	CCheckBox CanHit;
	CCheckBox OptionsHelmet;
	CCheckBox NightMode;
	CCheckBox OptionsKit;

	CCheckBox IsScoped;
	CCheckBox CompRank;
	CCheckBox HasDefuser;
	CCheckBox GrenadeTrace;
	CCheckBox show_hostage;
	CCheckBox OptionsDefuse;
	CCheckBox OptionsDefusing;
	CCheckBox SniperCrosshair;
	CCheckBox OptionsSkeleton;
	CCheckBox NightSky;
	CCheckBox hitbone;
	CCheckBox OptionsAimSpot;
	CCheckBox OtherNoScope;
	CCheckBox OptionsCompRank;
	CCheckBox resoinfo;
	CCheckBox killfeed;
	CCheckBox override_viewmodel;
	CCheckBox selfglow;
	CGroupBox FiltersGroup;
	CCheckBox FiltersAll;
	CCheckBox show_players;
	CCheckBox show_team;
	CCheckBox show_local;
	CCheckBox fogui;
	CComboBoxYeti FiltersWeapons;
	CCheckBox WeaponFilterName;
	CKeyBind flashlight;
	CCheckBox FiltersChickens;
	CCheckBox FiltersNades;
	CCheckBox FiltersC4;
	CCheckBox FiltersChicken;
	CGroupBox ChamsGroup;
	CComboBoxYeti asus_type;
	CCheckBox ChamsThruWalls;
	CCheckBox ModulateSkyBox;
	CCheckBox debug_esp;

	CSlider sky_r;
	CSlider sky_g;
	CSlider sky_b;

	CCheckBox fog_update;
	CComboBoxYeti fog_tint;
	CSlider fog_start;
	CSlider fog_end;

	CSlider  GlowZ;
	CSlider  team_glow;
	CSlider enemy_blend;
	CSlider enemy_blend_invis;
	CSlider offset_x;
	CSlider offset_y;
	CSlider offset_z;
	CCheckBox ChamsPlayers;
	CCheckBox ChamsEnemyOnly;
	CSlider blend_local;
	CGroupBox OtherGroup;
	CCheckBox bulletbeam;
	CComboBoxYeti OtherCrosshair;
	CComboBoxYeti OtherRecoilCrosshair;
	CCheckBox crosshair;
	CCheckBox OptionsGlow;
	CCheckBox EntityGlow;
	CCheckBox OtherEntityGlow;
	CCheckBox DisablePostProcess;
	CCheckBox OtherRadar;
	CCheckBox OtherNoVisualRecoil;
	CCheckBox OtherNoSky;
	CCheckBox OtherNoFlash;
	CCheckBox nosmoke;
	CSlider nosmoke_slider;
	CCheckBox OtherAsusWalls;
	CComboBoxYeti OtherNoHands;
	CCheckBox AAIndicators;
	CCheckBox BulletTracers;
	CComboBoxYeti localmaterial;
	CComboBoxYeti scopemat;
	CSlider transparency;
	CSlider hand_transparency;
	CSlider gun_transparency;
	CSlider sleeve_transparency;
	CSlider visible_transparency;
	CSlider invisible_transparency;
	CSlider asusamount;
	CComboBoxYeti asustype;
	CSlider beamtime;
	CCheckBox watermark;
	CSlider beamsize;
	CCheckBox cheatinfo;
	CCheckBox RemoveZoom;
	CComboBoxYeti sound;
	CCheckBox SpreadCross;
	CSlider SpreadCrossSize;
	CCheckBox DamageIndicator;
	CSlider OtherViewmodelFOV;
	CComboBoxYeti SpreadCrosshair;
	CSlider OtherFOV;
	CGroupBox worldgroup;
	CCheckBox colmodupdate;
	CSlider colmod;
	CCheckBox customskies2;
	CComboBoxYeti customskies;
	CCheckBox optimize;
};
class CColorTab : public CTab
{
public:
	void Setup();
	//you pasted the fucking colorselector but didn't use it... man man freak no baka
	//you have no fucking idea how many errors and how much cancer this gave me
	CGroupBox ColorsGroup;
	CComboBoxYeti beam_type;
	CGroupBox ConfigGroup;
	CGroupBox OtherOptions;
	CGroupBox OtherOptions2;
	CCheckBox ClanTag;
	CSlider radar_alpha;
	CSlider owo_slider;
	CListBox ConfigListBox;
	CButton SaveConfig;
	CButton LoadConfig;
	CButton RemoveConfig;
	CTextField NewConfigName;
	CComboBoxYeti MenuBar;
	CButton AddConfig;

	CCheckBox toggle_aspectratio;
	CSlider aspectratio;

	CColorSelector NameCol;
	CColorSelector HandChamsCol;
	CColorSelector GunChamsCol;
	CColorSelector BoxCol;
	CColorSelector BoxColteam;
	CColorSelector Skeleton;
	CColorSelector Skeletonteam;
	CColorSelector GlowEnemy; //no one has fucking team esp on so i'm not even gonna bother making a color selector for that
	CColorSelector GlowTeam;
	CColorSelector GlowLocal;
	CColorSelector GlowOtherEnt;
	CColorSelector Skyboxcolor;
	CColorSelector spreadcrosscol;
	CColorSelector Weapons;
	CColorSelector Weaponsteam;
	CColorSelector Ammo;
	CColorSelector Ammoteam;
	CColorSelector Money; //we don't really need to be able to change the color for all the flags it's just retarded
	CColorSelector Offscreen;
	CColorSelector ChamsLocal;
	CColorSelector ChamsEnemyVis;
	CColorSelector ChamsEnemyNotVis;
	CColorSelector ChamsTeamVis;
	CColorSelector ChamsTeamNotVis;
	CColorSelector bullet_tracer;
	CColorSelector Bullettracer_enemy;
	CColorSelector Menu;
	CColorSelector bomb_timer;
	CColorSelector SleeveChams_col;
	CColorSelector scoped_c;
	CColorSelector misc_backtrackchams;
	CColorSelector misc_lagcomp;
	CColorSelector misc_lagcompChams;
	CColorSelector misc_lagcompBones;
	CColorSelector fakelag_ghost;
	CColorSelector console_colour;

	CComboBoxYeti chamcount;
	CComboBoxYeti bonecount;
	CComboBoxYeti asus_type;

	CCheckBox DebugLagComp;
	CCheckBox BackTrackBones2;
	CCheckBox BackTrackBones;

	CCheckBox menu_backdrop;
	CCheckBox menu_healthshot;

	CSlider AmbientRed;
	CSlider AmbientGreen;
	CSlider AmbientBlue;


	CGroupBox gcol;
	CSlider outl_r;
	CSlider outl_g;
	CSlider outl_b;

	CSlider inl_r;
	CSlider inl_g;
	CSlider inl_b;

	CSlider inr_r;
	CSlider inr_g;
	CSlider inr_b;


	CSlider outr_r;
	CSlider outr_g;
	CSlider outr_b;

	CSlider cr;
	CSlider cg;
	CSlider cb;

	CCheckBox experimental_backtrack;
	CSlider quickstop_speed;

};
class CMiscTab : public CTab
{
public:
	void Setup();
	CCheckBox DisablePostProcess;
	CComboBoxYeti hitmarker_sound;

	CComboBoxYeti model;
	CCheckBox onetapsu;
	CCheckBox excuses;
	CCheckBox muslims;
	CCheckBox plague;
	CCheckBox banana;

	CComboBoxYeti autojump_type;
	CComboBoxYeti airduck_type;
	CGroupBox OtherGroup;
	CCheckBox OtherAutoJump;
	CCheckBox meme;
	CCheckBox OtherAutoStrafe;
	CKeyBind Zstrafe;
	CComboBoxYeti OtherSafeMode;
	CCheckBox freestandtype;
	CKeyBind OtherCircleButton;
	CCheckBox AutoDefuse;
	CCheckBox OtherCircleStrafe;
	CComboBoxYeti QuickStop;
	CCheckBox QuickCrouch;
	CCheckBox CircleStrafe;
	CKeyBind CircleStrafeKey;
	CKeyBind minimal_walk;
	CKeyBind OtherCircle;
	CCheckBox preset_aa;
	CTextField CustomClantag;
	CCheckBox CheatsByPass;
	CCheckBox DebugHitbones;
	CSlider freerange;
	CCheckBox aa_helper;
	CCheckBox infinite_duck;
	CComboBoxYeti buybot_primary;
	CComboBoxYeti buybot_secondary;
	CDropBox buybot_otr;
	CCheckBox AutoAccept;
	CSlider custom_pitch;
	CCheckBox SniperCrosshair;
	CSlider CircleAmount;
	CKeyBind OtherSlowMotion;
	CKeyBind plugwalk;
	CComboBoxYeti FakeLagTyp;
	CCheckBox autofw;
	CKeyBind fw;
	CComboBoxYeti FakeYaw;
	CComboBoxYeti FakeYaw2;
	CComboBoxYeti FakeYaw3;
	CKeyBind manual_right;
	CKeyBind manual_left;
	CKeyBind manualback;
	CKeyBind manualfront;

	CKeyBind desyncleft;
	CKeyBind desyncright;
	CCheckBox fakeangle_compaitibility;
	CKeyBind backup_aa;
	CCheckBox at_targets;
	CCheckBox disable_on_dormant;
	CCheckBox fakelag_unlock;
	CCheckBox fake_crouch;
	CKeyBind fake_crouch_key;
	CCheckBox fake_crouch_view;
	CSlider fake_crouch_fakelag;
	CKeyBind fake_stand_key;
	CGroupBox AntiAimGroup;

	CCheckBox AntiAimEnable;
	CComboBoxYeti AntiAimPitch;
	CComboBoxYeti antiaim_stand;
	CComboBoxYeti antiaimtype_stand;
	CComboBoxYeti antiaimtype_move;
	CCheckBox desync_aa_stand;
	CCheckBox desync_aa_move;

	CSlider staticr;
	CSlider staticf;
	CSlider ThirdPersonVal;
	CComboBoxYeti antiaim_air;
	CComboBoxYeti antiaim_move;
	CCheckBox Radar;
	CCheckBox OtherTeamChat;
	CSlider ClanTagSpeed;
	CSlider	  OtherChatDelay;
	CCheckBox NameChanger;
	CCheckBox AutoPistol;
	CCheckBox ClanTag;
	CDropBox killsay;
	CKeyBind  OtherAirStuck;
	CCheckBox OtherSpectators;
	CCheckBox OtherThirdperson;

	CComboBoxYeti desync_type_stand;
	CSlider desync_range_stand;
	CCheckBox desync_swapsides_stand;

	CComboBoxYeti desync_type_move;
	CSlider desync_range_move;
	CComboBoxYeti desync_swap;

	CKeyBind fakelag_key;
	CKeyBind ThirdPersonKeyBind;
	CSlider FakeWalkSpeed;
	CSlider ThirdPersonValue;
	CCheckBox standing_desync;
	CCheckBox moving_desync;
	CCheckBox air_desync;
	CSlider twitchf;
	CSlider spinf;
	CSlider randlbyr;
	CSlider randlbyf;
	CSlider spinspeed;
	CSlider lby1;
	CSlider stand_jitter;
	CSlider move_jitter;
	CCheckBox squaredance;
	//	CComboBoxYeti antilby;
	CComboBoxYeti antilby;
	CSlider antilbymod;
	CDropBox backup_aa_fac;
	CSlider real_align;
	CSlider backup_real;
	CCheckBox extend;
	CCheckBox pitch_up;
	CCheckBox experimental;

	CGroupBox FakeLagGroup;
	CSlider RadarX;
	CSlider RadarY;
	CCheckBox FakeLagEnable;
	CSlider Fakelagjump;
	CCheckBox PingSpike;
	CKeyBind LagSpikeKey;
	CComboBoxYeti KnifeModel;
	CButton   SkinApply;
	CCheckBox SkinEnable;
	CSlider FakelagMove;
	CSlider fakelag_factor;
	CCheckBox fl_onshot;
	CCheckBox FakelagBreakLC;
	CComboBoxYeti modifier;
	CSlider   FakeLagChoke;
	CCheckBox FakelagOnground;
	CCheckBox antilby2;
	CSlider FakeLagChoke2;
	CKeyBind lagkey;
	CComboBoxYeti ConfigBox;
	CGroupBox ConfigGroup;

	CSlider pingspike_val;
	CKeyBind pingspike_key;

	CComboBoxYeti fl_spike;
};

class CSkinTab : public CTab
{
public:
	void Setup();
	//--------------------------//
	CGroupBox knifegroup;
	CGroupBox snipergroup;

	//--------------------------//
	CComboBoxYeti t_knife_index;
	CComboBoxYeti ct_knife_index;

	//--------------------------//
	CSlider t_knife_wear;
	CSlider t_sniperSCAR_wear;
	CSlider t_sniperAWP_wear;

	//--------------------------//
	CComboBoxYeti t_knife_skin_id;
	CComboBoxYeti t_sniperAWP_skin_id;
	CComboBoxYeti t_sniperSCAR_skin_id;

	CCheckBox refresh;
};

class mirror_window : public CWindow
{
public:
	void Setup();
	CAimbotTab aimbot;
	CLegitBotTab LegitBotTab;
	CVisualTab visuals;
	CMiscTab misc;
	CSkinTab skin;

	CColorTab ColorsTab;
	CButton   SkinApply;
	CButton SaveButton1;
	CButton LoadButton1;
	CButton SaveButton;
	CButton LoadButton;
	CButton UnloadButton;
	CButton PanicButton;
	CButton ThirdButton;
	CButton ByPass;
	CButton FirstButton;
	CComboBox ConfigBox;
};
namespace options
{
	void SetupMenu();
	void DoUIFrame();
	extern mirror_window menu;
};