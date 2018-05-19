#include "../includes.h"
#include "../UTILS/mopvar.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../FEATURES/Movement.h"

void CMovement::bunnyhop(SDK::CUserCmd* cmd)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (cmd->buttons & IN_JUMP)
	{
		int local_flags = local_player->GetFlags();
		if (!(local_flags & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;

		if (local_player->GetVelocity().Length() <= 50)
			cmd->forwardmove = 450.f;
	}
}

void CMovement::autostrafer(SDK::CUserCmd* cmd) //thanks toast
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	int local_flags = local_player->GetFlags();
	if (!(local_player->GetFlags() & local_flags & FL_ONGROUND))
	{
		if (cmd->mousedx > 1 || cmd->mousedx < -1)
			cmd->sidemove = cmd->mousedx < 0.0f ? -450 : 450;
		else 
		{
			cmd->forwardmove = 4200.0f / local_player->GetVelocity().Length();
			cmd->sidemove = (cmd->command_number % 2) == 0 ? -450 : 450;
			if (cmd->forwardmove >  450)
				cmd->forwardmove = 450;
		}
	}
	else if (cmd->buttons & IN_JUMP)
		cmd->forwardmove = 450;
}

CMovement* movement = new CMovement();