#include "../includes.h"
#include "../UTILS/mopvar.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CBaseWeapon.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/AntiAim.h"

float randnum(float Min, float Max)
{
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

float get_curtime(SDK::CUserCmd* ucmd) {
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return 0;

	int g_tick = 0;
	SDK::CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = (float)local_player->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * mopvar::Globals->interval_per_tick;
	return curtime;
}

float anim_velocity(SDK::CBaseEntity* LocalPlayer)
{
	if (LocalPlayer->GetAnimState() == nullptr)
		return false;

	int vel = LocalPlayer->GetAnimState()->speed_2d;
	return vel;
}

bool first_supress(const float yaw_to_break, SDK::CUserCmd* cmd)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime(cmd);

	auto animstate = local_player->GetAnimState();
	if (!animstate)
		return false;

	if (!(local_player->GetFlags() & FL_ONGROUND))
		return false;

	if (SETTINGS::settings.delta_val < 120)
		return false;

	if (animstate->speed_2d > 0.1)
		next_lby_update_time = curtime + 0.22 - TICKS_TO_TIME(1);

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1;
		return true;
	}

	return false;
}

bool second_supress(const float yaw_to_break, SDK::CUserCmd* cmd)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime(cmd);

	auto animstate = local_player->GetAnimState();
	if (!animstate)
		return false;

	if (!(local_player->GetFlags() & FL_ONGROUND))
		return false;

	if (SETTINGS::settings.delta_val < 120)
		return false;

	if (animstate->speed_2d > 0.1)
		next_lby_update_time = curtime + 0.22 + TICKS_TO_TIME(1);


	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1;
		return true;
	}

	return false;
}

bool next_lby_update(const float yaw_to_break, SDK::CUserCmd* cmd)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime(cmd);

	auto animstate = local_player->GetAnimState();
	if (!animstate)
		return false;

	if (!(local_player->GetFlags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d > 0.1)
		next_lby_update_time = curtime + 0.22f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}

void CAntiAim::do_antiaim(SDK::CUserCmd* cmd)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(mopvar::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));

	if (!weapon)
		return;

	if (cmd->buttons & IN_USE)
		return;

	if (cmd->buttons & IN_ATTACK && aimbot->can_shoot())
		return;

	if (weapon->get_full_info()->WeaponType == 9)
		return;

	if (!SETTINGS::settings.aa_bool)
		return;

	//float delta = abs(MATH::NormalizeYaw(GLOBAL::real_angles.y - local_player->GetLowerBodyYaw()));
	//std::cout << std::to_string(delta) << std::endl;

	switch (SETTINGS::settings.aa_type % 6)
	{
	case 0:
		break;
	case 1:
		cmd->viewangles.x = 89.000000;
		backwards(cmd);
		break;
	case 2:
		cmd->viewangles.x = 89.000000;
		sideways(cmd);
		break;
	case 3:
		cmd->viewangles.x = 89.000000;
		backjitter(cmd);
		break;
	case 4:
		cmd->viewangles.x = 89.000000;
		lowerbody(cmd);
		break;
	case 5:
		legit(cmd);
		break;
		}
	}

void CAntiAim::fix_movement(SDK::CUserCmd* cmd)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	Vector real_viewangles;
	mopvar::Engine->GetViewAngles(real_viewangles);

	Vector vecMove(cmd->forwardmove, cmd->sidemove, cmd->upmove);
	float speed = sqrt(vecMove.x * vecMove.x + vecMove.y * vecMove.y);

	Vector angMove;
	MATH::VectorAngles(vecMove, angMove);

	float yaw = DEG2RAD(cmd->viewangles.y - real_viewangles.y + angMove.y);

	cmd->forwardmove = cos(yaw) * speed;
	cmd->sidemove = sin(yaw) * speed;

	cmd->viewangles = MATH::NormalizeAngle(cmd->viewangles);
}

void CAntiAim::backwards(SDK::CUserCmd* cmd)
{
	if (GLOBAL::should_send_packet)
		cmd->viewangles.y += randnum(-180, 180);
	else
		cmd->viewangles.y += 180.000000;
}

void CAntiAim::legit(SDK::CUserCmd* cmd)
{
	if (GLOBAL::should_send_packet)
		cmd->viewangles.y += 0;
	else
		cmd->viewangles.y += 90;
}

void CAntiAim::sideways(SDK::CUserCmd* cmd)
{
	if (SETTINGS::settings.flip_bool)
	{
		if (GLOBAL::should_send_packet)
			cmd->viewangles.y += 90;
		else
			cmd->viewangles.y -= 90;
	}
	else
	{
		if (GLOBAL::should_send_packet)
			cmd->viewangles.y -= 90;
		else
			cmd->viewangles.y += 90;
	}
}

void CAntiAim::backjitter(SDK::CUserCmd* cmd)
{
	if (GLOBAL::should_send_packet)
		cmd->viewangles.y += randnum(-180, 180);
	else
		cmd->viewangles.y += 180 + ((rand() % 15) - (15 * 0.5f));
}

void CAntiAim::lowerbody(SDK::CUserCmd* cmd)
{
	static float last_real;

	if (SETTINGS::settings.flip_bool)
	{
		if (GLOBAL::should_send_packet)
			cmd->viewangles.y += randnum(-180, 180);
		else
		{
			//if (first_supress(cmd->viewangles.y + 110, cmd))
				//cmd->viewangles.y = last_real + 110;
			if (next_lby_update(cmd->viewangles.y + SETTINGS::settings.delta_val, cmd)) //else if
				cmd->viewangles.y = last_real + SETTINGS::settings.delta_val;
			//else if (second_supress(cmd->viewangles.y - 110, cmd))
			//	cmd->viewangles.y = last_real - 110;
			else
			{
				cmd->viewangles.y -= 90;
				last_real = cmd->viewangles.y;
			}
		}
	}
	else
	{
		if (GLOBAL::should_send_packet)
			cmd->viewangles.y += randnum(-180, 180);
		else
		{
			if (next_lby_update(cmd->viewangles.y - SETTINGS::settings.delta_val, cmd))
				cmd->viewangles.y = last_real - SETTINGS::settings.delta_val;
			else
			{
				cmd->viewangles.y += 90;
				last_real = cmd->viewangles.y;
			}
		}
	}
}

CAntiAim* antiaim = new CAntiAim();