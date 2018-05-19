#pragma once

namespace SDK
{
	class CUserCmd;
}

class CMovement
{
public:
	void bunnyhop(SDK::CUserCmd* cmd);
	void autostrafer(SDK::CUserCmd* cmd);
};

extern CMovement* movement;