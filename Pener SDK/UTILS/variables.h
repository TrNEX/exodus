#pragma once

/*
global variables and other stuff that is probably too small to put in another 
file (and i'm too lazy to make files for all of them) would go in here.
Stuff like fonts and shit
*/

namespace GLOBAL
{
	extern HWND csgo_hwnd;

	extern bool should_send_packet;
	extern bool is_fakewalking;
	extern int choke_amount;

	extern Vector real_angles;
	extern Vector fake_angles;
}

inline void setclantag(const char* tag)
{
	static auto ClanTagOffset = UTILS::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx");
	if (ClanTagOffset)
	{
		auto tag_ = std::string(tag);
		if (strlen(tag) > 0) {
			auto newline = tag_.find("\\n");
			auto tab = tag_.find("\\t");
			if (newline != std::string::npos) {
				tag_.replace(newline, newline + 2, "\n");
			}
			if (tab != std::string::npos) {
				tag_.replace(tab, tab + 2, "\t");
			}
		}
		static auto dankesttSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(ClanTagOffset);
		dankesttSetClanTag(tag_.data(), tag_.data());
	}
}

namespace FONTS
{
	extern unsigned int menu_tab_font;
	extern unsigned int menu_checkbox_font;
	extern unsigned int menu_slider_font;
	extern unsigned int menu_groupbox_font;
	extern unsigned int menu_combobox_font;
	extern unsigned int menu_window_font;
	extern unsigned int menu_window2_font;
	extern unsigned int numpad_menu_font;
	extern unsigned int visuals_esp_font;
	extern unsigned int visuals_xhair_font;
	extern unsigned int visuals_side_font;
	extern unsigned int visuals_name_font;
	extern unsigned int visuals_lby_font;
	
	bool ShouldReloadFonts();
	void InitFonts();
}
namespace SETTINGS
{
	class CSettings
	{
	public:
		// returns true/false whether the function succeeds, usually returns false if file doesn't exist
		bool Save(std::string file_name);
		bool Load(std::string file_name);

		void CreateConfig(); // creates a blank config

		std::vector<std::string> GetConfigs();

		bool bhop_bool;
		bool strafe_bool;
		bool esp_bool;
		int chams_type;
		bool ragdoll;
		float fov = 90;
		bool clantag;
		float NightStrength = 0.50;
		bool recoil_circle;
		int xhair_type;
		bool tp_bool;
		bool aim_bool;
		int aim_type;
		bool aa_bool;
		int aa_type;
		int acc_type;
		bool up_bool;
		bool misc_bool;
		int config_sel;
		bool beam_bool;
		bool stop_bool;
		bool night_bool;
		bool box_bool;
		bool name_bool;
		bool weap_bool;
		bool health_bool;
		bool info_bool;
		bool back_bool;
		bool lag_bool;
		int box_type;
		bool reverse_bool;
		bool multi_bool;
		bool fakefix_bool;

		float stand_lag;
		float move_lag;
		float jump_lag;

		bool debug_bool;

		CColor vmodel_col;
		CColor imodel_col;
		CColor box_col;
		CColor spread_col;
		CColor menu_col = CColor(0, 255, 97);

		float chance_val;
		float damage_val;
		float delta_val;
		float point_val;
		float body_val;

		bool flip_bool;
		int aa_side;

	private:
	}; extern CSettings settings;
}

/*extern bool bhop_bool;
extern bool strafe_bool;
extern bool esp_bool;
extern int chams_type;
extern int xhair_type;
extern bool tp_bool;
extern bool aim_bool;
extern int aim_type;
extern bool aa_bool;
extern int aa_type;
extern int acc_type;
extern bool up_bool;
extern bool misc_bool;

extern float chance_val;
extern float damage_val;
extern float delta_val;

extern bool flip_bool;
extern int aa_side;
extern bool in_tp;*/

extern bool using_fake_angles[65];

extern bool in_tp;
extern bool fake_walk;

extern int resolve_type[65];

extern int shots_fired[65];
extern int shots_hit[65];
extern int shots_missed[65];

extern float tick_to_back[65];
extern float lby_to_back[65];
extern bool backtrack_tick[65];

extern float lby_delta;
extern float update_time[65];

extern int hitmarker_time;

extern bool menu_hide;

extern int oldest_tick[65];
extern float compensate[65][12];
extern Vector backtrack_hitbox[65][20][12];
extern float backtrack_simtime[65][12];