#include "includes.h"
#include "Hooks.h"

#include "Global.h"
#include "Thirdperson.h"

vector* _fastcall Hooked_EyeAngles(void* ecx, void* edx)
{
	if (global::local && ecx == global::local && interfaces::pinput->m_fCameraInThirdPerson)
		return &thirdperson::get().get_tp_angle();

	return original_eyeangles(ecx);
}