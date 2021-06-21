#include "includes.h"
#include "Hooks.h"

HRESULT __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	auto hr = o_Reset(pDevice, pPresentationParameters);
	if (hr >= 0)
	{
		bool gey;
		gey = true;
	}
	return hr;
}