#include "includes.h"
#include "Hooks.h"

bool __stdcall Hooked_InPrediction()
{
	bool result;
	static InPrediction_ origFunc = (InPrediction_)Hooks::VMTPrediction.GetOriginalFunction(14);
	static DWORD* ecxVal = interfaces::prediction_dword;
	result = origFunc(ecxVal);
	if (options::menu.visuals.OtherNoVisualRecoil.getstate() && (DWORD)(_ReturnAddress()) == Offsets::Functions::dwCalcPlayerView)
	{
		IClientEntity* pLocalEntity = NULL;
		float* m_LocalViewAngles = NULL;
		__asm
		{
			MOV pLocalEntity, ESI
			MOV m_LocalViewAngles, EBX
		}
		vector viewPunch = pLocalEntity->localPlayerExclusive()->GetViewPunchAngle();
		vector aimPunch = pLocalEntity->localPlayerExclusive()->GetAimPunchAngle();
		m_LocalViewAngles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		m_LocalViewAngles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		m_LocalViewAngles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		return true;
	}
	return result;
}