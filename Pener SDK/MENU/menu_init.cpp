#include "../includes.h"
#include "../UTILS/mopvar.h"
#include "../SDK/IEngine.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CInputSystem.h"
#include "../UTILS/render.h"
#include "Components.h"

#include "menu_framework.h"

namespace MENU
{
	void InitColors()
	{
		using namespace PPGUI_PP_GUI;



		colors[GAME_SNEEZE] = CColor(0, 255, 97);

		colors[WINDOW_BODY] = CColor(20, 20, 20);
		colors[WINDOW_TITLE_BAR] = CColor(21, 21, 21);
		colors[WINDOW_TEXT] = WHITE;

		colors[GROUPBOX_BODY] = CColor(21, 21, 21);
		colors[GROUPBOX_OUTLINE] = CColor(48, 48, 48);
		colors[GROUPBOX_TEXT] = WHITE;

		colors[SCROLLBAR_BODY] = CColor(30, 30, 30, 255);

		colors[SEPARATOR_TEXT] = WHITE;
		colors[SEPARATOR_LINE] = CColor(90, 90, 90, 255);

		colors[CHECKBOX_CLICKED] = SETTINGS::settings.menu_col; //HOTPINK
		colors[CHECKBOX_NOT_CLICKED] = CColor(32,32,32, 255);
		colors[CHECKBOX_TEXT] = WHITE;

		colors[BUTTON_BODY] = CColor(23, 23, 23, 255), CColor(16, 16, 16, 255);
		colors[BUTTON_TEXT] = WHITE;

		colors[COMBOBOX_TEXT] = WHITE;
		colors[COMBOBOX_SELECTED] = CColor(23, 23, 23, 255), CColor(16, 16, 16, 255);
		colors[COMBOBOX_SELECTED_TEXT] = WHITE;
		colors[COMBOBOX_ITEM] = CColor(23, 23, 23, 255), CColor(16, 16, 16, 255);
		colors[COMBOBOX_ITEM_TEXT] = WHITE;

		colors[SLIDER_BODY] = CColor(23, 23, 23, 255);
		colors[SLIDER_VALUE] = SETTINGS::settings.menu_col; //HOTPINK
		colors[SLIDER_TEXT] = WHITE;

		colors[TAB_BODY] = CColor(20, 20, 20, 120);
		colors[TAB_TEXT] = LIGHTGREY;
		colors[TAB_BODY_SELECTED] = CColor(40, 40, 40, 150); //HOTPINK
		colors[TAB_TEXT_SELECTED] = WHITE;

		colors[VERTICAL_TAB_BODY] = CColor(255, 40, 40, 255);
		colors[VERTICAL_TAB_TEXT] = WHITE;
		colors[VERTICAL_TAB_OUTLINE] = CColor(0, 0, 0, 100);
		colors[VERTICAL_TAB_BODY_SELECTED] = CColor(255, 20, 20, 255);

		colors[COLOR_PICKER_BODY] = CColor(23, 23, 23, 255);
		colors[COLOR_PICKER_TEXT] = WHITE;
		colors[COLOR_PICKER_OUTLINE] = CColor(0, 0, 0, 100);
	}
	void Do()
	{
		//all of this cool gui stuff made by bolbi, but i decide to just render text and make binds for every feature. sorry bolbi!

		static bool menu_open = false;

		if (UTILS::INPUT::input_handler.GetKeyState(VK_INSERT) & 1)
		{
			menu_open = !menu_open;
			mopvar::Engine->ClientCmd(menu_open ? "cl_mouseenable 0" :
				"cl_mouseenable 1");
			mopvar::InputSystem->EnableInput(!menu_open);
		}

		if (UTILS::INPUT::input_handler.GetKeyState(VK_END) & 1)
		{
			menu_hide = !menu_hide;
		}

		//--- Flip Bool Stuff ---//
		if (UTILS::INPUT::input_handler.GetKeyState(86) & 1)
			SETTINGS::settings.flip_bool = !SETTINGS::settings.flip_bool;

		//--- New Menu Rendering System --//
		if (menu_hide)
		{
			//menu is hidden entirely, you can do anything here i guess
		}
		else
		{
			if (menu_open)
			{
				using namespace PPGUI_PP_GUI;

				// if menu not open
				if (!menu_open)
					return;

				DrawMouse();

				SetFont(FONTS::menu_window_font);
				WindowBegin("exodos", Vector2D(200, 200), Vector2D(420, 375));

				std::vector<std::string> tabs = { "Aimbot", "Visuals", "Misc", "Anti-aim", "Configs" };
				std::vector<std::string> acc_mode = { "Head", "Body", "All" };
				std::vector<std::string> chams_mode = { "None", "Visible", "Invisible" };
				std::vector<std::string> aa_mode = { "none", "Backwards", "Sideways", "Backjitter", "Lowerbody", "Legit troll" };
				std::vector<std::string> configs = { "Default", "Legit", "Autos", "Scouts", "Pistols" };
				std::vector<std::string> box_style = { "none", "full" };
				std::string config;

				switch (Tab("main", tabs, OBJECT_FLAGS::FLAG_NONE))
				{
				case 0:
				{
					Checkbox("Activate", SETTINGS::settings.aim_bool);
					{
						GroupboxBegin("Main", 227.5, 1);
						{
							Combobox("Aimbot hitbox", acc_mode, SETTINGS::settings.acc_type);
							Checkbox("Reverse", SETTINGS::settings.reverse_bool);
							Checkbox("Fake-lag fix", SETTINGS::settings.fakefix_bool);;
							Slider("Hitchance", 0, 100, SETTINGS::settings.chance_val);
							Slider("Minimum damage", 1, 100, SETTINGS::settings.damage_val);
							GroupboxEnd();
						}
					}
				}
				break;
				case 1:
				{
					Checkbox("Activate", SETTINGS::settings.esp_bool);
					{
						GroupboxBegin("Player", 180, 1);
						Combobox("Draw box", box_style, SETTINGS::settings.box_type);
						if (SETTINGS::settings.box_type > 0)
							ColorPicker("box", SETTINGS::settings.box_col);
						Checkbox("Draw name", SETTINGS::settings.name_bool);
						Checkbox("Draw weapon", SETTINGS::settings.weap_bool);
						Checkbox("Draw info", SETTINGS::settings.info_bool);
						Checkbox("Draw health", SETTINGS::settings.health_bool);
						Combobox("Chams", chams_mode, SETTINGS::settings.chams_type);
						if (SETTINGS::settings.chams_type > 0)
							ColorPicker("Visible model", SETTINGS::settings.vmodel_col);
						if (SETTINGS::settings.chams_type > 1)
							ColorPicker("Invisible model", SETTINGS::settings.imodel_col);
						GroupboxEnd();

						GroupboxBegin("World", 145, 1);
						Checkbox("Night Mode", SETTINGS::settings.night_bool);
						Checkbox("Spread Crosshair", SETTINGS::settings.recoil_circle);
						if (SETTINGS::settings.recoil_circle > 0)
							ColorPicker("Color", SETTINGS::settings.spread_col);
						Checkbox("Bullet tracers", SETTINGS::settings.beam_bool);
						Checkbox("Thirdperson(mouse3)", SETTINGS::settings.tp_bool);
						Slider("Field Of View", 0, 130, SETTINGS::settings.fov);
						GroupboxEnd();
					}
				}
				break;
				case 2:
				{
				
					{
						GroupboxBegin("Movement", 90, 1);
						Checkbox("Auto bunnyhop", SETTINGS::settings.bhop_bool);
						Checkbox("Auto strafer", SETTINGS::settings.strafe_bool);
						GroupboxEnd();
						GroupboxBegin("Fakelag", 100, 1);
						Checkbox("Fakelag", SETTINGS::settings.lag_bool);
						Slider("Immobile", 1, 13, SETTINGS::settings.stand_lag);
						Slider("Moving", 1, 13, SETTINGS::settings.move_lag);
						Slider("In Air", 1, 13, SETTINGS::settings.jump_lag);
						GroupboxEnd();

						GroupboxBegin("Other", 65, 1);
						Checkbox("Gravity Ragdoll", SETTINGS::settings.ragdoll);
						Checkbox("Clantag", SETTINGS::settings.clantag);
						GroupboxEnd();
					}
				}
				break;
				case 3:
				{
					GroupboxBegin("Misc", 75, 1);
					Checkbox("Activate", SETTINGS::settings.aa_bool);
					{
							Combobox("Anti-aim type", aa_mode, SETTINGS::settings.aa_type);
						if (SETTINGS::settings.aa_type == 4)
							Slider("lby delta", 0, 180, SETTINGS::settings.delta_val);
						GroupboxEnd();
					}
				}
				break;
				case 4:
				{
					GroupboxBegin("Config", 40, 1);
					switch (Combobox("config", configs, SETTINGS::settings.config_sel))
					{
					case 0: config = "default"; break;
					case 1: config = "legit"; break;
					case 2: config = "auto_hvh"; break;
					case 3: config = "scout_hvh"; break;
					case 4: config = "pistol_hvh"; break;
					}
					GroupboxEnd();

					GroupboxBegin("Save/Load", 65, 1);
					if (Button("Load"))
						SETTINGS::settings.Load(config);

					if (Button("Save"))
						SETTINGS::settings.Save(config);
					GroupboxEnd();


					//Checkbox("debug", SETTINGS::settings.debug_bool);

					//ColorPicker("menu theme", SETTINGS::settings.menu_col);
				}
				break;
				}
				WindowEnd();



			}
		}
	}
}