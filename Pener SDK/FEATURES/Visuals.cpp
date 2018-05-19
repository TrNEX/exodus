#include "../includes.h"
#include "../UTILS/mopvar.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/IViewRenderBeams.h"
#include "../FEATURES/Backtracking.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CTrace.h"	
#include "../FEATURES/Resolver.h"
#include "../SDK/CGlobalVars.h"
#include "../FEATURES/Visuals.h"
#include "../UTILS/render.h"
#include "../SDK/IVDebugOverlay.h"
#include <string.h>

//--- Misc Variable Initalization ---//
int alpha[65];
CColor breaking;
CColor backtrack;
static bool bPerformed = false, bLastSetting;
float fade_alpha[65];
float dormant_time[65];
CColor main_color;
CColor ammo;

inline void CrossProduct(const Vector& a, const Vector& b, Vector& result)
{
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
}

__forceinline float VecDotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void CVisuals::Draw()
{
	for (int i = 1; i < 65; i++)
	{
		auto entity = mopvar::ClientEntityList->GetClientEntity(i);
		auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

		if (!entity)
			continue;

		if (!local_player)
			continue;

		bool is_local_player = entity == local_player;
		bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

		if (is_local_player)
			continue;

		if (is_teammate) 
			continue;

		if (entity->GetHealth() <= 0)
			continue;

		//--- Colors ---//
		int enemy_hp = entity->GetHealth();
		int hp_red = 255 - (enemy_hp * 2.55);
		int hp_green = enemy_hp * 2.55;
		CColor health_color = CColor(hp_red, hp_green, 1, alpha[entity->GetIndex()]);
		CColor dormant_color = CColor(100, 100, 100, alpha[entity->GetIndex()]);
		CColor box_color;
		CColor spread_color = SETTINGS::settings.spread_col;
		CColor still_health;
		CColor alt_color;
		CColor zoom_color;
		CColor bomb_color;

		//--- Domant ESP Checks ---//
		if (entity->GetIsDormant())
		{
			if (alpha[entity->GetIndex()] > 0)
				alpha[entity->GetIndex()] -= 0.4;
			main_color = dormant_color;
			still_health = dormant_color;
			alt_color = dormant_color;
			zoom_color = dormant_color;
			breaking = dormant_color;
			backtrack = dormant_color;
			box_color = dormant_color;
			bomb_color = dormant_color;
			ammo = dormant_color;
		}
		else if (!entity->GetIsDormant())
		{
			alpha[entity->GetIndex()] = 255;
			main_color = CColor(255, 255, 255, alpha[entity->GetIndex()]); //heath_color
			still_health = health_color;
			alt_color = BLACK;
			zoom_color = CColor(150, 150, 220, 255);
			breaking = CColor(220, 150, 150);
			backtrack = CColor(155, 220, 150);
			box_color = SETTINGS::settings.box_col;
			bomb_color = CColor(244, 66, 66, 150);
			ammo = CColor(61, 135, 255);
		}

		//--- Entity Related Rendering ---///
		switch (SETTINGS::settings.box_type)
		{
		case 0: break;
		case 1: DrawBox(entity, box_color); break;
		}
		if (SETTINGS::settings.recoil_circle)
		{
			auto pLocal = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

			if (!pLocal)
				return;
			auto pWeapon = reinterpret_cast<SDK::CBaseWeapon*>(mopvar::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
			if (!(pLocal->GetHealth()))
				return;


			int xs;
			int ys;
			mopvar::Engine->GetScreenSize(xs, ys);
			xs /= 2; ys /= 2;

			auto accuracy = pWeapon->GetInaccuracy() * 700.f; //3000

			RENDER::DrawCircle(xs, ys, accuracy, 60, spread_color);


		}

		if (SETTINGS::settings.name_bool) DrawName(entity, main_color, i);
		if (SETTINGS::settings.weap_bool) DrawWeapon(entity, main_color, i);
		if (SETTINGS::settings.info_bool) DrawInfo(entity, main_color, zoom_color); DrawDistance(entity, main_color);
		if (SETTINGS::settings.health_bool) DrawHealth(entity, still_health, alt_color);
		//DrawDirection(entity->GetVecOrigin());
		//DrawHitbox(entity);
		//DrawSkeleton(entity, main_color);
		//DrawBarrel(entity);
	}
}

void CVisuals::ClientDraw()
{
	DrawBorderLines();

	DrawIndicator();
	ModulateWorld();
	DrawHitmarker();
}

std::string str_to_upper(std::string strToConvert)
{
	std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

	return strToConvert;
}

void CVisuals::DrawBox(SDK::CBaseEntity* entity, CColor color)
{
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		int width = height / 2;

		RENDER::DrawEmptyRect(pos.x - width / 2, top.y, (pos.x - width / 2) + width, top.y + height, color);
		RENDER::DrawEmptyRect((pos.x - width / 2) + 1, top.y + 1, (pos.x - width / 2) + width - 1, top.y + height - 1, CColor(20, 20, 20, alpha[entity->GetIndex()]));
		RENDER::DrawEmptyRect((pos.x - width / 2) - 1, top.y - 1, (pos.x - width / 2) + width + 1, top.y + height + 1, CColor(20, 20, 20, alpha[entity->GetIndex()]));
	}

	/*Vector bbmin; //another way of doing it, doesn't look gae tho
	Vector bbmax;
	Vector screen1;
	Vector screen2;
	entity->GetRenderBounds(bbmin, bbmax);
	Vector pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	Vector top3D = pos3D + Vector(0, 0, bbmax.z + 10);

	if (RENDER::WorldToScreen(pos3D, screen1) && RENDER::WorldToScreen(top3D, screen2))
	{
	float height = screen1.y - screen2.y;
	float width = height / 4;
	RENDER::DrawEmptyRect(screen2.x - width, screen2.y, screen1.x + width, screen1.y, color); //main
	RENDER::DrawEmptyRect(screen2.x - width - 1, screen2.y - 1, screen1.x + width + 1, screen1.y + 1, CColor(20, 20, 20, alpha[entity->GetIndex()])); //outline
	RENDER::DrawEmptyRect(screen2.x - width + 1, screen2.y + 1, screen1.x + width - 1, screen1.y - 1, CColor(20, 20, 20, alpha[entity->GetIndex()])); //inline
	}*/
}



void CVisuals::DrawName(SDK::CBaseEntity* entity, CColor color, int index)
{
	SDK::player_info_t ent_info;
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);
	mopvar::Engine->GetPlayerInfo(index, &ent_info);

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		int width = height / 2;
		RENDER::DrawF(pos.x, top.y - 7, FONTS::visuals_name_font, true, true, color, ent_info.name); //numpad_menu_font
	}
/*	SDK::player_info_t ent_info;
	Vector bbmin;
	Vector bbmax;
	Vector screen1;
	Vector screen2;
	entity->GetRenderBounds(bbmin, bbmax);
	Vector bottom = entity->GetAbsOrigin();
	Vector top = entity->GetAbsOrigin() + Vector(0, 0, bbmax.z);
	mopvar::Engine->GetPlayerInfo(index, &ent_info);

	if (RENDER::WorldToScreen(bottom, screen1) && RENDER::WorldToScreen(top, screen2))
	{
		float height = screen1.y - screen2.y;
		float width = height / 4;
		RENDER::DrawF(screen2.x, screen2.y - 6, FONTS::numpad_menu_font, true, true, color, ent_info.name);
	}*/
}

float CVisuals::resolve_distance(Vector src, Vector dest)
{
	Vector delta = src - dest;

	float fl_dist = ::sqrtf((delta.Length()));

	if (fl_dist < 1.0f)
		return 1.0f;

	return fl_dist;
}

void CVisuals::DrawDistance(SDK::CBaseEntity* entity, CColor color)
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	SDK::player_info_t ent_info;
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);
	Vector vecOrigin = entity->GetVecOrigin();
	Vector vecOriginLocal = local_player->GetVecOrigin();

	char dist_to[32];
	sprintf_s(dist_to, "%.0f ft", resolve_distance(vecOriginLocal, vecOrigin));

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		int width = height / 2;
		RENDER::DrawF(pos.x, pos.y + 8, FONTS::visuals_esp_font, true, true, color, dist_to);
	}
}

std::string fix_item_name(std::string name)
{
	std::string cname = name;

	if (cname[0] == 'C')
		cname.erase(cname.begin());

	auto startOfWeap = cname.find("Weapon");
	if (startOfWeap != std::string::npos)
		cname.erase(cname.begin() + startOfWeap, cname.begin() + startOfWeap + 6);

	return cname;
}

void CVisuals::DrawWeapon(SDK::CBaseEntity* entity, CColor color, int index)
{
	SDK::player_info_t ent_info;
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);
	mopvar::Engine->GetPlayerInfo(index, &ent_info);

	auto weapon = mopvar::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(mopvar::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));

	if (!c_baseweapon)
		return;

	if (!weapon)
		return;

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		int width = height / 2;
		RENDER::DrawF(pos.x, pos.y + 18, FONTS::visuals_esp_font, true, true, color, fix_item_name(weapon->GetClientClass()->m_pNetworkName) + " (" + std::to_string(c_baseweapon->GetLoadedAmmo()) + ")");
	}
}

void CVisuals::DrawHealth(SDK::CBaseEntity* entity, CColor color, CColor dormant)
{
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);

	int enemy_hp = entity->GetHealth();
	int hp_red = 255 - (enemy_hp * 2.55);
	int hp_green = enemy_hp * 2.55;
	CColor health_color = CColor(hp_red, hp_green, 1, alpha[entity->GetIndex()]);

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		int width = height / 2;

		float offset = (height / 4.f) + 5;
		UINT hp = height - (UINT)((height * enemy_hp) / 100);

		RENDER::DrawEmptyRect((pos.x - width / 2) - 6, top.y - 1, (pos.x - width / 2) - 3, top.y + height + 1, dormant); //intense maths
		RENDER::DrawLine((pos.x - width / 2) - 4, top.y + hp, (pos.x - width / 2) - 4, top.y + height, color); //could have done a rect here,
		RENDER::DrawLine((pos.x - width / 2) - 5, top.y + hp, (pos.x - width / 2) - 5, top.y + height, color); //but fuck it

		if (entity->GetHealth() < 100)
			RENDER::DrawF((pos.x - width / 2) - 4, top.y + hp, FONTS::visuals_esp_font, true, true, main_color, std::to_string(enemy_hp));
	}
}

void CVisuals::DrawAmmo(SDK::CBaseEntity* entity, CColor color, CColor dormant)
{
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);

	int enemy_hp = entity->GetHealth();
	int hp_red = 255 - (enemy_hp * 2.55);
	int hp_green = enemy_hp * 2.55;
	CColor health_color = CColor(hp_red, hp_green, 1, alpha[entity->GetIndex()]);

	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	bool is_local_player = entity == local_player;
	bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

	if (is_local_player)
		return;

	if (is_teammate)
		return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(mopvar::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));

	if (!c_baseweapon)
		return;

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		//int width = height / 2;

		float offset = (height / 4.f) + 5;
		UINT hp = height - (UINT)((height * 3) / 100);

		auto animLayer = local_player->GetAnimOverlay(1);
		if (!animLayer.m_pOwner)
			return;

		auto activity = local_player->GetSequenceActivity(animLayer.m_nSequence);

		int iClip = c_baseweapon->GetLoadedAmmo();
		int iClipMax = c_baseweapon->get_full_info()->iMaxClip1;

		float box_w = (float)fabs(height / 2);
		float width;
		if (activity == 967 && animLayer.m_flWeight != 0.f)
		{
			float cycle = animLayer.m_flCycle; // 1 = finished 0 = just started
			width = (((box_w * cycle) / 1.f));
		}
		else
			width = (((box_w * iClip) / iClipMax));

		RENDER::DrawFilledRect((pos.x - box_w / 2) - 6, top.y - 1, (pos.x - box_w / 2) - 3, top.y + height + 1, dormant);
		RENDER::DrawFilledRect((pos.x - width / 2) - 6, top.y, (pos.x - width / 2) - 3, top.y + height, ammo);
	}
}

void CVisuals::DrawInfo(SDK::CBaseEntity* entity, CColor color, CColor alt)
{
	std::vector<std::pair<std::string, CColor>> stored_info;
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);

	//if (entity->GetHealth() > 0)
	//	stored_info.push_back(std::pair<std::string, CColor>("hp: " + std::to_string(entity->GetHealth()), color));

	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	auto weapon = mopvar::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(mopvar::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));

	if (!weapon)
		return;

	if (!c_baseweapon)
		return;

	//stored_info.push_back(std::pair<std::string, CColor>(weapon->GetClientClass()->m_pNetworkName, color));
//	stored_info.push_back(std::pair<std::string, CColor>("ammo: " + std::to_string(c_baseweapon->GetLoadedAmmo()), color));

	//if (entity->GetArmor() > 0)
	//	stored_info.push_back(std::pair<std::string, CColor>(entity->GetArmorName(), color));

	//if (entity->GetIsScoped())
	//	stored_info.push_back(std::pair<std::string, CColor>("zoom", alt));

	//stored_info.push_back(std::pair<std::string, CColor>("$" + std::to_string(entity->GetMoney()), backtrack));

	//if (!using_fake_angles[entity->GetIndex()])
	//	stored_info.push_back(std::pair<std::string, CColor>("fake", color));

	//if (entity->HasC4())
		//stored_info.push_back(std::pair<std::string, CColor>("B", backtrack));

	/*if (!SETTINGS::settings.aim_type == 2)
		stored_info.push_back(std::pair<std::string, CColor>("null", color));
	else
	{
		if (local_player->GetHealth() == 0)
			stored_info.push_back(std::pair<std::string, CColor>("null", color)); //not resolving
		else if (resolve_type[entity->GetIndex()] == 1)
			stored_info.push_back(std::pair<std::string, CColor>("fake 2", color)); //moving
		else if (resolve_type[entity->GetIndex()] == 2)
			stored_info.push_back(std::pair<std::string, CColor>("shuffle correct", color));
		else if (resolve_type[entity->GetIndex()] == 3)
			stored_info.push_back(std::pair<std::string, CColor>("fake 1", color)); //lby updates
		else if (resolve_type[entity->GetIndex()] == 4)
			stored_info.push_back(std::pair<std::string, CColor>("fake 4", color)); //bruteforce
		else if (resolve_type[entity->GetIndex()] == 5)
			stored_info.push_back(std::pair<std::string, CColor>("fake 3", color)); //logged angle
		else if (resolve_type[entity->GetIndex()] == 6)
			stored_info.push_back(std::pair<std::string, CColor>("lby inverse", color));
		else if (resolve_type[entity->GetIndex()] == 7)
			stored_info.push_back(std::pair<std::string, CColor>("fake 5", color)); //backtracking lby
		else
			stored_info.push_back(std::pair<std::string, CColor>("not resolving", color));

	}*/

	//stored_info.push_back(std::pair<std::string, CColor>("update: " + std::to_string(mopvar::Globals->curtime - update_time[entity->GetIndex()]), backtrack));

	/*if (entity->GetVelocity().Length2D() > 36)
		stored_info.push_back(std::pair<std::string, CColor>("velocity: " + std::to_string(entity->GetVelocity().Length2D()), CColor(0, 255, 0, alpha[entity->GetIndex()])));
	else if (entity->GetVelocity().Length2D() < 36 && entity->GetVelocity().Length2D() > 0.1)
		stored_info.push_back(std::pair<std::string, CColor>("velocity: " + std::to_string(entity->GetVelocity().Length2D()), CColor(255, 255, 0, alpha[entity->GetIndex()])));
	else
		stored_info.push_back(std::pair<std::string, CColor>("velocity: " + std::to_string(entity->GetVelocity().Length2D()), CColor(255, 0, 0, alpha[entity->GetIndex()])));*/

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		int width = height / 2;
		int i = 0;
		for (auto Text : stored_info)
		{
			RENDER::DrawF((pos.x + width / 2) + 5, top.y + i, FONTS::visuals_esp_font, false, false, Text.second, Text.first); //numpad_menu_font
			i += 8;
		}
	}
}

void CVisuals::DrawSkeleton(SDK::CBaseEntity* entity, CColor color) //wut hek
{
	matrix3x4_t bone_matrix[128];
	if (!entity->SetupBones(bone_matrix, 128, BONE_USED_BY_ANYTHING, 0))
		return;

	auto studio_model = mopvar::ModelInfo->GetStudioModel(entity->GetModel());
	if (!studio_model)
		return;

	for (int i = 0; i < studio_model->numbones; i++)
	{
		auto bone = studio_model->GetBone(i);
		if (!bone || bone->parent < 0 || !(bone->flags & BONE_USED_BY_HITBOX))
			continue;

		/// 2 bone matrices, 1 for the child and 1 for the parent bone
		matrix3x4_t bone_matrix_1, bone_matrix_2;
		if (bone_matrix)
		{
			bone_matrix_1 = bone_matrix[i];
			bone_matrix_2 = bone_matrix[bone->parent];
		}
		else
		{
			bone_matrix_1 = entity->GetBoneMatrix(i);
			bone_matrix_2 = entity->GetBoneMatrix(bone->parent);
		}

		Vector bone_position_1 = Vector(bone_matrix_1[0][3], bone_matrix_1[1][3], bone_matrix_1[2][3]),
			bone_position_2 = Vector(bone_matrix_2[0][3], bone_matrix_2[1][3], bone_matrix_2[2][3]);

		Vector screen1, screen2;
		if (RENDER::WorldToScreen(bone_position_1, screen1) && RENDER::WorldToScreen(bone_position_2, screen2))
			RENDER::DrawLine(screen1.x, screen1.y, screen2.x, screen2.y, color);
	}
	/*SDK::studiohdr_t* pStudioModel = mopvar::ModelInfo->GetStudioModel(entity->GetModel());
	if (!pStudioModel)
		return;

	matrix3x4_t pBoneToWorldOut[128];
	if (!entity->SetupBones(pBoneToWorldOut, 128, BONE_USED_BY_ANYTHING, 0))
		return;

	SDK::mstudiohitboxset_t* pHitboxSet = pStudioModel->GetHitboxSet(0);
	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioModel->numbones; j++)
	{
		SDK::mstudiobone_t* pBone = pStudioModel->GetBone(j);

		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			vChild = entity->GetBonePosition(j);
			vParent = entity->GetBonePosition(pBone->parent);

			int iChestBone = 6;  // Parameter of relevant Bone number
			Vector vBreastBone; // New reference Point for connecting many bones
			Vector vUpperDirection = entity->GetBonePosition(iChestBone + 1) - entity->GetBonePosition(iChestBone); // direction vector from chest to neck
			vBreastBone = entity->GetBonePosition(iChestBone) + vUpperDirection / 2;
			Vector vDeltaChild = vChild - vBreastBone; // Used to determine close bones to the reference point
			Vector vDeltaParent = vParent - vBreastBone;

			// Eliminating / Converting all disturbing bone positions in three steps.
			if ((vDeltaParent.Length() < 9 && vDeltaChild.Length() < 9))
				vParent = vBreastBone;

			if (j == iChestBone - 1)
				vChild = vBreastBone;

			if (abs(vDeltaChild.z) < 5 && (vDeltaParent.Length() < 5 && vDeltaChild.Length() < 5) || j == iChestBone)
				continue;

			mopvar::DebugOverlay->ScreenPosition(vParent, sParent);
			mopvar::DebugOverlay->ScreenPosition(vChild, sChild);

			mopvar::Surface->DrawSetColor(CColor(255, 255, 255, 255));
			mopvar::Surface->DrawLine(sParent[0], sParent[1], sChild[0], sChild[1]);
		}
	}*/
	/*for (int i = 0; i < pStudioModel->numbones; i++)
	{
		SDK::mstudiobone_t* pBone = pStudioModel->GetBone(i);
		if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
			continue;

		Vector vBonePos1;
		if (!RENDER::WorldToScreen(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
			continue;

		Vector vBonePos2;
		if (!RENDER::WorldToScreen(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
			continue;

		RENDER::DrawLine((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, main_color);
	}*/
}

void CVisuals::DrawBulletBeams()
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (!mopvar::Engine->IsInGame() || !local_player)
	{
		Impacts.clear();
		return;
	}

	if (Impacts.size() > 30)
		Impacts.pop_back();

	for (int i = 0; i < Impacts.size(); i++)
	{
		auto current = Impacts.at(i);

		if (!current.pPlayer)
			continue;

		if (current.pPlayer->GetIsDormant())
			continue;

		bool is_local_player = current.pPlayer == local_player;
		bool is_teammate = local_player->GetTeam() == current.pPlayer->GetTeam() && !is_local_player;

		if (current.pPlayer == local_player)
			current.color = { 0, 255, 0 };
		else if (current.pPlayer != local_player && !is_teammate)
			current.color = { 255, 0, 0 };
		else if (current.pPlayer != local_player && is_teammate)
			current.color = { 0, 0, 255};

		SDK::BeamInfo_t beamInfo;
		beamInfo.m_nType = SDK::TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = 3.0f;
		beamInfo.m_flWidth = 2.0f;
		beamInfo.m_flEndWidth = 2.0f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f; 
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current.color.RGBA[0];
		beamInfo.m_flGreen = current.color.RGBA[1];
		beamInfo.m_flBlue = current.color.RGBA[2];
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = SDK::FBEAM_ONLYNOISEONCE | SDK::FBEAM_NOTILE | SDK::FBEAM_HALOBEAM;

		beamInfo.m_vecStart = current.pPlayer->GetVecOrigin() + current.pPlayer->GetViewOffset();
		beamInfo.m_vecEnd = current.vecImpactPos;

		auto beam = mopvar::ViewRenderBeams->CreateBeamPoints(beamInfo);
		if (beam)
			mopvar::ViewRenderBeams->DrawBeam(beam);

		//g_pVDebugOverlay->AddBoxOverlay( current.vecImpactPos, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ), Vector( 0, 0, 0 ), current.color.r(), current.color.g(), current.color.b(), 125, 0.8f );

		Impacts.erase(Impacts.begin() + i);
	}
}

void CVisuals::DrawCrosshair()
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	auto weapon = mopvar::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex());

	if (!weapon)
		return;

	//--- Screen Positioning ---//
	static int screen_x;
	static int screen_y;
	if (screen_x == 0)
	{
		mopvar::Engine->GetScreenSize(screen_x, screen_y);
		screen_x /= 2; screen_y /= 2;
	}

	//--- Calculating Recoil ---//
	Vector view_angles;
	mopvar::Engine->GetViewAngles(view_angles);
	view_angles += local_player->GetPunchAngles() * 2.f;
	Vector factor_vec;
	MATH::AngleVectors(view_angles, &factor_vec);
	factor_vec = factor_vec * 10000.f;
	Vector start = local_player->GetVecOrigin() + local_player->GetViewOffset();
	Vector end = start + factor_vec, recoil_to_screen;

	//--- Calculating Spread (unused) ---//
	//auto cone = weapon->GetSpread() + weapon->GetInaccuracy();// *500.f;

	//if (!cone)
		//return;

	//RENDER::DrawFilledCircle(recoil_to_screen.x, recoil_to_screen.y, spread, 50, CColor(0, 0, 0, 100));

	//--- Rendering Crosshairs ---//
	switch (SETTINGS::settings.xhair_type % 3)
	{
	case 0:
		break;
	case 1:
		RENDER::DrawF(screen_x, screen_y, FONTS::visuals_xhair_font, true, true, WHITE, "+");
		break;
	case 2:
		if (RENDER::WorldToScreen(end, recoil_to_screen) && local_player->GetHealth() > 0)
			RENDER::DrawF(recoil_to_screen.x, recoil_to_screen.y, FONTS::visuals_xhair_font, true, true, WHITE, "+");
		break;
	}
}

void CVisuals::DrawIndicator()
{
	//--- LBY Indication Stuff ---//
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	if (menu_hide)
		return;

	float breaking_lby_fraction = fabs(MATH::NormalizeYaw(GLOBAL::real_angles.y - local_player->GetLowerBodyYaw())) / 180.f;
	float lby_delta = abs(MATH::NormalizeYaw(GLOBAL::real_angles.y - local_player->GetLowerBodyYaw()));

	int screen_width, screen_height;
	mopvar::Engine->GetScreenSize(screen_width, screen_height);

	//RENDER::DrawF(10, screen_height / 2, FONTS::numpad_menu_font, false, false, CColor((1.f - breaking_lby_fraction) * 255.f, breaking_lby_fraction * 255.f, 0), "lby delta: " + std::to_string(lby_delta));
	RENDER::DrawF(10, screen_height - 88, FONTS::visuals_lby_font, false, false, CColor((1.f - breaking_lby_fraction) * 255.f, breaking_lby_fraction * 255.f, 0), "LBY");

	if (!SETTINGS::settings.aa_bool)
		return;

	if (SETTINGS::settings.aa_type == 2 || SETTINGS::settings.aa_type == 4)
	{

		if (SETTINGS::settings.flip_bool)
		{
			RENDER::DrawF((screen_width / 2) + 40, screen_height / 2, FONTS::visuals_side_font, true, true, CColor(10, 145, 190, 255), ">"); //green
			RENDER::DrawF((screen_width / 2) - 40, screen_height / 2, FONTS::visuals_side_font, true, true, CColor(255, 255, 255, 255), "<");
		}
		else
		{
			RENDER::DrawF((screen_width / 2) - 40, screen_height / 2, FONTS::visuals_side_font, true, true, CColor(10, 145, 190, 255), "<"); //green
			RENDER::DrawF((screen_width / 2) + 40, screen_height / 2, FONTS::visuals_side_font, true, true, CColor(255, 255, 255, 255), ">");
		}
	}
}

/*void CVisuals::ModulateWorld()
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	static SDK::ConVar* sv_skyname = mopvar::cvar->FindVar("sv_skyname");
	sv_skyname->nFlags &= ~FCVAR_CHEAT;

	if (!local_player || !mopvar::Engine->IsConnected() || !mopvar::Engine->IsInGame())
		return;

	if (!bPerformed)
	{
		for (auto i = mopvar::MaterialSystem->FirstMaterial(); i != mopvar::MaterialSystem->InvalidMaterial(); i = mopvar::MaterialSystem->NextMaterial(i))
		{
			static SDK::IMaterial* pMaterial = mopvar::MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				if (bLastSetting)
				{
					sv_skyname->SetValue("sky_csgo_night02");
					pMaterial->SetMaterialVarFlag(SDK::MATERIAL_VAR_TRANSLUCENT, false);
					pMaterial->ColorModulate(0.15, 0.15, 0.15);
				}
				else
				{
					sv_skyname->SetValue("vertigoblue_hdr");
					pMaterial->ColorModulate(1.00, 1.00, 1.00);
				}
			}
		}
	}
	if (bLastSetting != esp_bool)
	{
		bLastSetting = esp_bool;
	}
}*/

void CVisuals::DrawDirection(const Vector& origin) //monarch is the NIGGA
{
	constexpr float radius = 360.0f;
	int width, height;
	mopvar::Engine->GetScreenSize(width, height);

	Vector vRealAngles;
	mopvar::Engine->GetViewAngles(vRealAngles);

	Vector vForward, vRight, vUp(0.0f, 0.0f, 1.0f);

	MATH::AngleVectors(vRealAngles, &vForward);

	vForward.z = 0.0f;
	MATH::NormalizeAngle(vForward);
	CrossProduct(vUp, vForward, vRight);

	float flFront = VecDotProduct(origin, vForward);
	float flSide = VecDotProduct(origin, vRight);
	float flXPosition = radius * -flSide;
	float flYPosition = radius * -flFront;

	float rotation = mopvar::Globals->user_cmd->viewangles.y + 180;

	rotation = atan2(flXPosition, flYPosition) + M_PI;
	rotation *= 180.0f / M_PI;

	float flYawRadians = -(rotation)* M_PI / 180.0f;
	float flCosYaw = cos(flYawRadians);
	float flSinYaw = sin(flYawRadians);

	flXPosition = (int)((width / 2.0f) + (radius * flSinYaw));
	flYPosition = (int)((height / 2.0f) - (radius * flCosYaw));

	RENDER::DrawFilledCircle(flXPosition, flYPosition, 10, 50, CColor(255, 0, 255, 120));
}

void CVisuals::ModulateWorld() //credits to my nigga monarch
{
	static bool nightmode_performed = false, nightmode_lastsetting;
	if (!mopvar::Engine->IsConnected() || !mopvar::Engine->IsInGame())
	{
		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (!local_player)
	{
		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	if (nightmode_lastsetting != SETTINGS::settings.night_bool)
	{
		nightmode_lastsetting = SETTINGS::settings.night_bool;
		nightmode_performed = false;
	}

	if (!nightmode_performed)
	{
		//static SDK::ConVar* r_DrawSpecificStaticProp = mopvar::cvar->FindVar("r_DrawSpecificStaticProp");
		//r_DrawSpecificStaticProp->nFlags &= ~FCVAR_CHEAT;

		static SDK::ConVar* sv_skyname = mopvar::cvar->FindVar("sv_skyname");
		sv_skyname->nFlags &= ~FCVAR_CHEAT;

		for (SDK::MaterialHandle_t i = mopvar::MaterialSystem->FirstMaterial(); i != mopvar::MaterialSystem->InvalidMaterial(); i = mopvar::MaterialSystem->NextMaterial(i))
		{
			SDK::IMaterial *pMaterial = mopvar::MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;




			if (strstr(pMaterial->GetTextureGroupName(), "World"))
			{
				if (SETTINGS::settings.night_bool)
					pMaterial->ColorModulate(SETTINGS::settings.NightStrength, SETTINGS::settings.NightStrength, SETTINGS::settings.NightStrength);
				else
					pMaterial->ColorModulate(1, 1, 1);

				if (SETTINGS::settings.night_bool)
				{
					sv_skyname->SetValue("sky_csgo_night02");
					pMaterial->SetMaterialVarFlag(SDK::MATERIAL_VAR_TRANSLUCENT, false);
					pMaterial->ColorModulate(SETTINGS::settings.NightStrength, SETTINGS::settings.NightStrength, SETTINGS::settings.NightStrength);
				}
				else
				{
					sv_skyname->SetValue("vertigoblue_hdr");
					pMaterial->ColorModulate(1.00, 1.00, 1.00);
				}
			}
			else if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				if (SETTINGS::settings.night_bool)
					pMaterial->ColorModulate(SETTINGS::settings.NightStrength, SETTINGS::settings.NightStrength, SETTINGS::settings.NightStrength);
				else
					pMaterial->ColorModulate(1, 1, 1);
			}
		}
		nightmode_performed = true;
	}
}


void CVisuals::DrawHitmarker()
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	int center_width;
	int center_height;
	mopvar::Engine->GetScreenSize(center_width, center_height);
	center_width / 2, center_height / 2;

	if (hitmarker_time > 0)
	{
		auto line_size = 10;

		RENDER::DrawLine(center_width - line_size, center_height - line_size, center_width - (line_size / 2), center_height - (line_size / 2), CColor(255, 255, 255, hitmarker_time));
		RENDER::DrawLine(center_width - line_size, center_height + line_size, center_width - (line_size / 2), center_height + (line_size / 2), CColor(255, 255, 255, hitmarker_time));
		RENDER::DrawLine(center_width + line_size, center_height + line_size, center_width + (line_size / 2), center_height + (line_size / 2), CColor(255, 255, 255, hitmarker_time));
		RENDER::DrawLine(center_width + line_size, center_height - line_size, center_width + (line_size / 2), center_height - (line_size / 2), CColor(255, 255, 255, hitmarker_time));
		hitmarker_time -= 4;

		/*for (int i = 0; i < 10; i++) { //pixel by pixel method
			RENDER::FillRectangle(width / 2 - 8 - i, height / 2 - 8 - i, 3, 3, CColor(0, 0, 0, 255));
			RENDER::FillRectangle(width / 2 - 7 - i, height / 2 - 7 - i, 1, 1, CColor(255, 255, 255, 255));

			RENDER::FillRectangle(width / 2 + 8 + i, height / 2 + 8 + i, 3, 3, CColor(0, 0, 0, 255));
			RENDER::FillRectangle(width / 2 + 7 + i, height / 2 + 7 + i, 1, 1, CColor(255, 255, 255, 255));

			RENDER::FillRectangle(width / 2 - 8 - i, height / 2 + 8 + i, 3, 3, CColor(0, 0, 0, 255));
			RENDER::FillRectangle(width / 2 - 7 - i, height / 2 + 7 + i, 1, 1, CColor(255, 255, 255, 255));

			RENDER::FillRectangle(width / 2 + 8 + i, height / 2 - 8 - i, 3, 3, CColor(0, 0, 0, 255));
			RENDER::FillRectangle(width / 2 + 7 + i, height / 2 - 7 - i, 1, 1, CColor(255, 255, 255, 255));
		}*/
	}
}

void CVisuals::DrawAntiAimSides()
{
	int screen_x;
	int screen_y;
	mopvar::Engine->GetScreenSize(screen_x, screen_y);
	screen_x /= 2; screen_y /= 2;

	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());
	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	if (!SETTINGS::settings.aa_type == 2 || !SETTINGS::settings.aa_type == 4)
		return;

	if (SETTINGS::settings.aa_side == 0)
		RENDER::DrawWF(screen_x - 25, screen_y, FONTS::visuals_side_font, CColor(0, 255, 0, 255), L"\u25c4");
	if (SETTINGS::settings.aa_side == 1)
		RENDER::DrawWF(screen_x + 25, screen_y, FONTS::visuals_side_font, CColor(0, 255, 0, 255), L"\u25ba");
}

void CVisuals::DrawBorderLines()
{
	auto local_player = mopvar::ClientEntityList->GetClientEntity(mopvar::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	auto weapon = mopvar::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex());

	if (!weapon)
		return;

	//--- Screen Positioning ---//
	int screen_x;
	int screen_y;
	int center_x;
	int center_y;
	mopvar::Engine->GetScreenSize(screen_x, screen_y);
	mopvar::Engine->GetScreenSize(center_x, center_y);
	center_x /= 2; center_y /= 2;

	//--- Rendering Scope Lines ---//
	if (local_player->GetIsScoped())
	{
		RENDER::DrawLine(0, center_y, screen_x, center_y, CColor(0, 0, 0, 255));
		RENDER::DrawLine(center_x, 0, center_x, screen_y, CColor(0, 0, 0, 255));
	}
}

void CVisuals::DrawBarrel(SDK::CBaseEntity* entity)
{
	//uh fuck idk yet
}

void CVisuals::DrawBomb(SDK::CBaseEntity* entity)
{
	//:thinking:
}

CVisuals* visuals = new CVisuals();