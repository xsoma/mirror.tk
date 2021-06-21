#include "lin_extp.h"
#include "RageBot.h"
template<class T, class U>
T fine(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}
void LinearExtrapolations::run()
{
	auto m_local = hackManager.plocal();

	if (hackManager.plocal()->team() != 2 && hackManager.plocal()->team() != 3)
		return;

	if (m_local && m_local->IsAlive()) {
		for (int i = 1; i < interfaces::globals->max_clients; i++)
		{
			auto m_entity = interfaces::ent_list->get_client_entity(i);
			if (!ragebot->TargetMeetsRequirements(m_entity, m_local) )
				continue;

			float simtime_delta = m_entity->get_simulation_time() - m_entity->m_flOldSimulationTime();
			int choked_ticks = fine(TIME_TO_TICKS(simtime_delta), 1, 15);
			vector lastOrig;

			if (lastOrig.Length() != m_entity->GetOrigin().Length())
				lastOrig = m_entity->GetOrigin();

			float delta_distance = (m_entity->GetOrigin() - lastOrig).LengthSqr();
			if (delta_distance > 4096.f)
			{
				ragebot->enemy_breaking_lc = true;
				vector velocity_per_tick = m_entity->getvelocity() * interfaces::globals->interval_per_tick;
				auto new_origin = m_entity->GetOrigin() + (velocity_per_tick * choked_ticks);
				m_entity->SetAbsOriginal(new_origin);
			}
		}
	}
}