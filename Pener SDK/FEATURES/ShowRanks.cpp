#include "../includes.h"
#include "../UTILS/mopvar.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/ShowRanks.h"

/*void CReveal::CreateMove(SDK::CUserCmd* cmd) //not pasted
{
	if (!SETTINGS::settings.esp_bool)
		return;

	if (!(cmd->buttons & IN_SCORE))
		return;

	float input[3] = { 0.f };
	//MsgFunc_ServerRankRevealAll(input);
}*/