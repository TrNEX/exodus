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
#include "../FEATURES/FakeLag.h"

void CFakeLag::do_fakelag()
{
	GLOBAL::should_send_packet = true;
	int choke_amount;

	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());
	if (!local_player || local_player->GetHealth() <= 0)
		return;

	auto net_channel = mopvar::Engine->GetNetChannel();
	if (!net_channel)
		return;

	if (SETTINGS::settings.aa_type == 0 || SETTINGS::settings.aa_type == 5)
		return;

	//if u dont remember, 1 = no fakelag so right now there is no fakelag (fucks with shit, nice fakelag bolbi1!1!!!!)
	if (SETTINGS::settings.lag_bool)
	{
		//if (fake_walk)
			//choke_amount = 14;
		//else
		//{
			choke_amount = 1; // standing flag (1)
			if (!(local_player->GetFlags() & FL_ONGROUND))
			{
				choke_amount = SETTINGS::settings.jump_lag; // jumping flag (6)
			}
			else if (local_player->GetVelocity().Length2D() > 0.1)
			{
				choke_amount = SETTINGS::settings.move_lag; // moving flag (3)
			}
		//}
	}
	else
		choke_amount = 1;

	if (net_channel->m_nChokedPackets >= min(15, choke_amount))
		GLOBAL::should_send_packet = true;
	else
		GLOBAL::should_send_packet = false;
}

CFakeLag* fakelag = new CFakeLag();