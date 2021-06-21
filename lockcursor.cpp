#include "includes.h"
#include "Hooks.h"

void __stdcall Hooked_LockCursor()
{
	bool xd = options::menu.m_bIsOpen;
	if (xd) {
		interfaces::surface->unlockcursor();
		return;
	}
	oLockCursor(interfaces::surface);
}