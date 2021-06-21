#include "fakelag.h"
#include "Autowall.h"
#include "RageBot.h"
#include <array>
#include <filesystem>
#include <cmath>
#include "MiscHacks.h"
fakelag * c_fakelag = new fakelag();

int random_int(int min, int max)
{
	return rand() % max + min;
}
void fakelag::break_lagcomp(bool & send_packet, int ammount)
{
	static int factor = random_int(1, ammount);
	static int tick; tick++;
	bool m_switch = false;
	int m_stepdown;
	send_packet = !(tick % factor);

	if (send_packet)
	{
		int m_choke = tick % ammount + 1;

		m_stepdown = 15 - m_choke;
		if (m_choke >= ammount)
			m_switch = !m_switch;

		factor = m_switch ? m_stepdown : m_choke;

		if (factor >= 15)
			factor = 14;
	}

}

int fakelag::break_lagcomp_mm(float ammount)
{
	float speed = hackManager.plocal()->getvelocity().length2d();
	bool in_air = !(hackManager.plocal()->GetFlags() & FL_ONGROUND);

	if (speed > 0.f)
	{
		auto distance_per_tick = speed * interfaces::globals->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, ammount);
	}
}
int fakelag::break_lagcomp_mm_2()
{
	float speed = hackManager.plocal()->getvelocity().length2d();
	bool in_air = !(hackManager.plocal()->GetFlags() & FL_ONGROUND);

	if (speed > 0.f)
	{
		auto distance_per_tick = speed * interfaces::globals->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, 14);
	}
}
int random_integer(int min, int max)
{
	return rand() % max + min;
}
namespace i_hate_visual_studio
{
	template<class T, class U>
	T clamp(T in, U low, U high)
	{
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}
	
}
static bool whatever = false;
void fakelag::when_enemy_sight(bool &bSendPacket, IClientEntity * local, float ammount, bool &break_lc)
{
	float s = break_lc ? break_lagcomp_mm(ammount) : ammount;

	if (ragebot->can_autowall)
	{
		bSendPacket = ( s <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
	}
	else
	{
		bSendPacket = (1 <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
	}		
}

void fakelag::mirror_adaptive(bool &packet, IClientEntity * local, int ammount, bool break_lc, CUserCmd* cmd)
{
	bool done = false;
	int choked;
	auto anim_state = local->GetBasePlayerAnimState();

	if (cmd->buttons & IN_ATTACK)
	{
		packet = (1 <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
		return;
	}

	if (local->getvelocity().length2d() >= 20.f && local->getvelocity().length2d() < 200.f)
	{
		if (ragebot->can_autowall)
		{
			packet = (break_lc ? break_lagcomp_mm(ammount) : 14 <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
		}
		else
		{
			if (break_lc)
				packet = (break_lagcomp_mm(ammount) <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
			else
				packet = (1 <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
		}
	}

	else if (local->getvelocity().length2d() >= 200.f)
	{
		if (ragebot->can_autowall)
		{
			packet = (break_lc ? break_lagcomp_mm_2() : ammount) <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28);
		}
		else
			packet = (break_lagcomp_mm(3) <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
	}

	else
	{		
		packet = (ammount <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));
	}

}


void fakelag::aimware_adaptive(bool &send_packet, IClientEntity * local, int ammount)
{
	if (!local)
		return;

	float ExtrapolatedSpeed;
	int WishTicks;
	float VelocityY;
	float VelocityX;
	int WishTicks_1;
	signed int AdaptTicks;
	WishTicks = (ammount + 0.5) - 1;

	VelocityY = local->getvelocity().y;
	VelocityX = local->getvelocity().x;
	WishTicks_1 = 0;
	AdaptTicks = 2;
	ExtrapolatedSpeed = sqrt((VelocityX * VelocityX) + (VelocityY * VelocityY))
		* interfaces::globals->interval_per_tick;

	while ((WishTicks_1 * ExtrapolatedSpeed) <= 68.0)
	{
		if (((AdaptTicks - 1) * ExtrapolatedSpeed) > 68.0)
		{
			++WishTicks_1;
			break;
		}
		if ((AdaptTicks * ExtrapolatedSpeed) > 68.0)
		{
			WishTicks_1 += 2;
			break;
		}
		if (((AdaptTicks + 1) * ExtrapolatedSpeed) > 68.0)
		{
			WishTicks_1 += 3;
			break;
		}
		if (((AdaptTicks + 2) * ExtrapolatedSpeed) > 68.0)
		{
			WishTicks_1 += 4;
			break;
		}
		AdaptTicks += 5;
		WishTicks_1 += 5;
		if (AdaptTicks > 15) // originally 16
			break;
	}

	bool should_choke;

	if (*(int*)(uintptr_t(interfaces::client_state) + 0x4D28) < WishTicks_1 && *(int*)(uintptr_t(interfaces::client_state) + 0x4D28) < 15)
		should_choke = true;
	else
		should_choke = false;

	if (should_choke)
		send_packet = (WishTicks_1 <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28));

	else
		send_packet = (2 <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28)); // originally "4"
}

void fakelag::velocity(bool &packet, IClientEntity * local, int ammount, bool break_lc)
{
	static int factor = random_integer(ammount, 15);
	static int tick; tick++;
	static bool m = false;

	if (local->getvelocity().length2d() > 2.0f)
	{	
			packet = factor;
	}
	else
	{
		m = !m;
		packet = m ? true: false;
	}
}

void fakelag::mirror_adaptive_reworked(bool &packet, IClientEntity * local, int ammount, bool break_lc)
{
	static bool s = false; s = !s;

	if (ragebot->pTarget != nullptr)
	{
		auto lag = *(int*)(uintptr_t(interfaces::client_state) + 0x4D28);
		bool localmove = local->getvelocity().length2d() > 2;
		bool localjump = !(local->GetFlags() & FL_ONGROUND); // hippedy hoppedy communists are my private property

		switch (localmove)
		{
			case true:
			{
				if (localjump)
					if (backup_awall->wall_damage >= 1)
						packet = break_lc ? break_lagcomp_mm(ammount) < lag : ammount <= lag;
					else
						if (ammount > 1)
							packet = break_lagcomp_mm(ammount / 2) < lag;
						else
							packet = 3 < lag;
				else
					if (backup_awall->wall_damage >= 1)
						packet = (ammount > 3) ? (ammount <= lag) : (3 <= lag);
					else
						packet = break_lc ? break_lagcomp_mm(3) < lag : ammount <= lag;

			}
			break;

			case false:
			{
				if (overall::ticks_while_unducked > 0 && overall::ticks_while_unducked < 12)
					packet = 10 <= lag;
				else
					packet = s ? true : false;
			}
			break;
		}
	}
	else
		packet = s ? true : false;
}

#include "Global.h"
void fakelag::Fakelag(CUserCmd *pCmd, bool &bSendPacket) // terrible code do not look please :(
{
	IClientEntity* pLocal = (IClientEntity*)interfaces::ent_list->get_client_entity(interfaces::engine->get_localplayer());
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)interfaces::ent_list->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!interfaces::engine->IsConnected() || !interfaces::engine->IsInGame())
		return;

	static bool b = false;
	b = !b;

	if (interfaces::client_state->chokedcommands > 14)
		bSendPacket = true;

	if (GetAsyncKeyState(options::menu.aimbot.doubletap.GetKey()))
	{
		bSendPacket = b ? true : false;
		return;
	}

	if ((pCmd->buttons & IN_USE))
	{
		bSendPacket = b ? true : false;
		return;
	}

	if (options::menu.misc.fake_crouch_fakelag.GetValue() >= 10.f && options::menu.misc.fake_crouch_fakelag.GetValue() < 11.f)
		options::menu.misc.fake_crouch_fakelag.SetValue(10.f); // fist me
	if (options::menu.misc.fake_crouch_fakelag.GetValue() >= 11.f && options::menu.misc.fake_crouch_fakelag.GetValue() < 12.f)
		options::menu.misc.fake_crouch_fakelag.SetValue(12.f);
	if (options::menu.misc.fake_crouch_fakelag.GetValue() >= 12.f && options::menu.misc.fake_crouch_fakelag.GetValue() < 13.f)
		options::menu.misc.fake_crouch_fakelag.SetValue(12.f);
	if (options::menu.misc.fake_crouch_fakelag.GetValue() >= 13.f && options::menu.misc.fake_crouch_fakelag.GetValue() < 14.f)
		options::menu.misc.fake_crouch_fakelag.SetValue(14.f);

//	if (GetAsyncKeyState(options::menu.aimbot.doubletap.GetKey()))
//		return;

	if ((game_utils::IsGrenade(pWeapon) && pWeapon->GetThrowTime() > 0) || interfaces::m_iInputSys->IsButtonDown(ButtonCode_t::MOUSE_LEFT) || game_utils::IsRevolver(pWeapon))
	{
		bSendPacket = b ? true : false;
		return;
	}

	float flVelocity = pLocal->getvelocity().length2d() * interfaces::globals->interval_per_tick;
	bool in_air = !(hackManager.plocal()->GetFlags() & FL_ONGROUND);
	bool break_lc = options::menu.misc.FakelagBreakLC.getstate();
	static auto choked = 0;

	int type = options::menu.misc.fl_spike.getindex();
	int factor = options::menu.misc.fakelag_factor.GetValue();
	int lag = *(int*)(uintptr_t(interfaces::client_state) + 0x4D28);

	if (pCmd->buttons & IN_ATTACK) {
		*global::should_send_packet = options::menu.aimbot.rage_chokeshot.getstate() ? false : true;
		return;
	}

	if (GetAsyncKeyState(options::menu.misc.fake_crouch_key.GetKey())) {
		*global::should_send_packet = options::menu.misc.fake_crouch_fakelag.GetValue() <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28);
	}
	else if (GetAsyncKeyState(options::menu.misc.fakelag_key.GetKey()))
		*global::should_send_packet = lag <= *(int*)(uintptr_t(interfaces::client_state) + 0x4D28);
	
	else
	{
	
		switch (type)
		{
		case 0:
		{
			*global::should_send_packet = break_lc ? break_lagcomp_mm(factor) <= lag : factor <= lag;
			// FreaK is a snek
			// no step on snek or i pull an american revolution on yo ass
		}
		break;

		case 1: // enemy visible
		{
			when_enemy_sight(bSendPacket, pLocal, factor, break_lc);
		}
		break;

		case 2: // Mirror Adaptive
		{
			mirror_adaptive(bSendPacket, pLocal, factor, break_lc, pCmd);
		}
		break;

		case 3: // Aimware Adaptive
		{
			aimware_adaptive(bSendPacket, pLocal, factor);
		}
		break;

		case 4: // Velocity
		{
			velocity(bSendPacket, pLocal, factor, break_lc);
		}
		break;

		case 5: // fucking autistic shit that works against onetap for whatever the reason. I guess onetap can't handle much nowadays
		{
			mirror_adaptive_reworked(bSendPacket, pLocal, factor, break_lc);
		}
		break;

		case 6:
		{
			bSendPacket = (pCmd->command_number % 2 == 1) ? true : false;
		}
		break;

		}
	}
}
