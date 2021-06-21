#pragma once

class IClientEntity;
class C_BaseCombatWeapon;
class CSWeaponInfo;
class CUserCmd;
class vector;

namespace global
{
	extern IClientEntity* local;
	extern CUserCmd* cmd;

	extern vector real_angles;

	extern int m_shot_command_number;

	///global weapon vars
	extern C_BaseCombatWeapon*	m_weapon;
	extern CSWeaponInfo*		m_weapon_info;
	extern int					m_weapon_id;
	extern int					m_weapon_type;
	extern bool					m_weapon_fire;
	extern bool					m_player_fire;

	///packet related
	extern bool* should_send_packet;
}