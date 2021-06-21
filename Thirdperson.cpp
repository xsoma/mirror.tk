#include "includes.h"

#include "Thirdperson.h"
#include "Vector.h"
#include "CUtlVector.h"

#include "Global.h"

#include <algorithm>
#include <cctype>
#include <cstring>

template <class T>
constexpr const T& Clamp(const T& v, const T& lo, const T& hi)
{
	return (v >= lo && v <= hi) ? v : (v < lo ? lo : hi);
}

void anglevectors(const vector& angles, vector* forward, vector* right, vector* up)
{
	float sr, sp, sy, cr, cp, cy;
	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);
	SinCos(DEG2RAD(angles[2]), &sr, &cr);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}
	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void thirdperson::run()
{
	// Side note ~Hunter: I've had this in my cheat since 2019 and since my source got leaked (2018 ver) it's public anyways and imma be honest, don't even 
	// remember where I got this from lmfaoooo, zeus maybe? No fucking clue.
	//
	// Also this engine trace horrible lmfaooo, ik it's old asf. will replace it later and that'll fix this dog shit with it's randomly going back into the model

	if (!global::local)
		return;

	vector								offset;
	vector								origin, forward;
	static CTraceFilterOneEntity		filter{ };
	trace_t								tr;

	if (!interfaces::engine->IsInGame())
		return;

	bool alive = global::local && global::local->IsAlive();

	static bool enabledtp = false, check = false;

	if (GetAsyncKeyState(options::menu.misc.ThirdPersonKeyBind.GetKey()) && hackManager.plocal()->IsAlive())
	{
		if (!check)
			enabledtp = !enabledtp;
		check = true;
	}
	else
		check = false;

	if (!global::local->IsAlive())
		enabledtp = false;

	if (enabledtp)
	{
		// if alive and not in thirdperson already switch to thirdperson.
		if (alive && !interfaces::pinput->m_fCameraInThirdPerson)
			interfaces::pinput->m_fCameraInThirdPerson = true;
		else if (global::local->GetObserverMode() == 4) {

			// if in thirdperson, switch to firstperson.
			// we need to disable thirdperson to spectate properly.
			if (interfaces::pinput->m_fCameraInThirdPerson) {
				interfaces::pinput->m_fCameraInThirdPerson = false;
				interfaces::pinput->m_vecCameraOffset.z = 0.f;
			}

			global::local->SetObserverMode(5);
		}
	}
	// camera should be in firstperson.
	else if (interfaces::pinput->m_fCameraInThirdPerson) {
		interfaces::pinput->m_fCameraInThirdPerson = false;
		interfaces::pinput->m_vecCameraOffset.z = 0.f;
	}

	// if after all of this we are still in thirdperson.
	if (interfaces::pinput->m_fCameraInThirdPerson) {
		// get camera angles.
		interfaces::engine->get_viewangles(offset);

		// get our viewangle's forward directional vector.
		anglevectors(offset, &forward, nullptr, nullptr);

		// cam_idealdist convar.
		offset.z = 130.f;

		// start pos.
		origin = global::local->GetEyePosition();

		// setup trace filter and trace.
		filter.SetPassEntity(hackManager.plocal());

		Ray_t ray;
		ray.Init(origin, origin - (forward * offset.z), { -16.f, -16.f, -16.f }, { 16.f, 16.f, 16.f });
		interfaces::trace->TraceRay(ray, MASK_NPCWORLDSTATIC, (ITraceFilter*)&filter, &tr);

		// adapt distance to travel time.
		Clamp(tr.fraction, 0.f, 1.f);
		offset.z *= tr.fraction;

		// override camera angles.
		interfaces::pinput->m_vecCameraOffset = { offset.x, offset.y, offset.z };
	}
}

void thirdperson::set_tp_angle(const vector tp_angle) const
{
	this->tp_angle = tp_angle;
}