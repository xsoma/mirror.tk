#pragma once
#include "Entities.h"
#include "Interfaces.h"

struct DamageIndicator_t {
	int iDamage;
	bool bInitialized;
	float flEraseTime;
	float flLastUpdate;
	IClientEntity * Player;
	vector Position;
};

class DamageIndicators {
public:

	void paint();
};

extern DamageIndicators damage_indicators;