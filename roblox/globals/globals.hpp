#pragma once

#include "../classes/classes.hpp"

#include <cstdint>
#include "../../utils/overlay/ckeybind/keybind.hpp"
// lua-env
#include "../../utils/overlay/imgui/TextEditor.h"
#include "../../lua_env/LuaVM.hpp"


struct globals
{
	static silence::roblox::instance_t datamodel;
	static silence::roblox::instance_t visualengine;
	static silence::roblox::instance_t players;
	static silence::roblox::instance_t placeid;
//	static silence::roblox::instance_t camera;
	//static silence::roblox::instance_t pf;

	static bool esp;
	static bool box;
	static bool outline;
	static bool filledbox;
	static bool distance_esp;
	static bool rainbowbox;
	static bool local_box;
	static bool fov_filled;
	static bool deadzone_fov_filled;
	static bool name_esp;
	static bool tracers;
	static bool healthinfo;
	static bool healthbar;
	static bool chams;

	static float box_color[3];
	static float deadzone_color[3];
	static float deadzone_fill_color[3];
	static float name_color[3];
	static float fov_fill_color[3];
	static float fov_color[3];
	static float fill_color[3];
	static float tracers_color[3];

	static bool shake;
	static bool aimbot;
	static bool triggerbot;
	static bool stabilizer;
	static bool deadzone;
	static bool show_deadzone;
	static bool sticky_aim;
	static bool resolver;
	static bool autopred;

	static bool prediction;
	static bool vsync;
	static bool streamproof;

	static bool autologin;

	static std::string game;

	static int aimpart;
	static int boxtype;

	static int x_offset;
	static int y_offset;

	static int max_dist;

	static float smoothness_x;
	static float smoothness_y;

	static float stabilizer_x;
	static float stabilizer_y;

	static float prediction_x;
	static float prediction_y;

	static float shake_value;

	static float range;
	static float range_mult;

	static bool show_auth;

	static float sensitivity;
	static int fov;
	static int deadzone_value;
	static bool fov_on;
	static bool disable_outside_fov;

	static bool healthcheck;
	static bool knock_check;
	static bool team_check;
	static bool team_check_esp;

	static CKeybind aimbotkey;
	static TextEditor lua_editor;
	static silence::LuaVM luaVM; // lua env
	static bool crosshair_on;
};