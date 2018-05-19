#include "../includes.h"
#include "../UTILS/mopvar.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Fakewalk.h"
#include "../FEATURES/Aimbot.h"

#include <time.h>
#include <iostream>

void CFakewalk::do_fakewalk(SDK::CUserCmd* cmd)
{
	if (GetAsyncKeyState(VK_SHIFT)) //make sure fakelag is set to max when u trigger fakewalk!
	{
		auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

		if (!local_player || local_player->GetHealth() <= 0)
			return;

		auto net_channel = mopvar::Engine->GetNetChannel();
		if (!net_channel)
			return;

		auto animstate = local_player->GetAnimState();
		if (!animstate)
			return;

		const int choked_ticks = net_channel->m_nChokedPackets;

		fake_walk = true;

		if (!choked_ticks || choked_ticks > 8)
		{
			cmd->sidemove = 0;
			cmd->forwardmove = 450;

			if (!choked_ticks || animstate->speed_2d < 20.f)
				cmd->forwardmove = 0;

			aimbot->rotate_movement(UTILS::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f, cmd);
		}
	}
	else
		fake_walk = false;
}

CFakewalk* slidebitch = new CFakewalk();