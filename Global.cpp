#include "Global.h"

#include "includes.h"

#include "Entities.h"

namespace global
{
	IClientEntity* local;
	CUserCmd* cmd;

	vector real_angles;

	int m_shot_command_number;

	/// global local weapon vars
	C_BaseCombatWeapon* m_weapon;
	CSWeaponInfo* m_weapon_info;
	int         m_weapon_id;
	int         m_weapon_type;
	bool		m_weapon_fire;
	bool		m_player_fire;


	/// Packet related
	bool* should_send_packet;
}