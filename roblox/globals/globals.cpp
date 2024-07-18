#include "globals.hpp"
silence::roblox::instance_t globals::datamodel{};
silence::roblox::instance_t globals::players{};
silence::roblox::instance_t globals::visualengine{};
silence::roblox::instance_t globals::placeid{};
//silence::roblox::instance_t globals::camera{};
//silence::roblox::instance_t globals::pf{};

bool globals::sticky_aim = false;

bool globals::esp = false;
bool globals::box = false;
bool globals::filledbox = false;
bool globals::outline = false;
bool globals::distance_esp = false;
bool globals::local_box = false;
bool globals::rainbowbox = false;
bool globals::name_esp = false;
bool globals::tracers = false;
bool globals::healthinfo = false;
bool globals::healthbar = false;
bool globals::fov_filled = false;
bool globals::deadzone_fov_filled = false;
bool globals::crosshair_on = false;

float globals::box_color[3] = { 255, 255, 255 };
float globals::deadzone_color[3] = { 255, 255, 255 };
float globals::fill_color[3] = { 255, 255, 255 };
float globals::fov_fill_color[3] = { 255, 255, 255 };
float globals::fov_color[3] = { 255, 255, 255 };
float globals::name_color[3] = { 255, 255, 255 };
float globals::tracers_color[3] = { 255, 255, 255 };

bool globals::prediction = false;
bool globals::stabilizer = false;
bool globals::vsync = true;
bool globals::deadzone = false;
bool globals::show_deadzone = false;
bool globals::streamproof = false;
bool globals::aimbot = false;
bool globals::autologin = false;
bool globals::shake = false;
bool globals::triggerbot = false;
bool globals::resolver = false;
bool globals::autopred = false;

std::string globals::game = "Universal";

int globals::aimpart = 0;
int globals::boxtype = 0;

int globals::x_offset = 0;
int globals::y_offset = 0;

int globals::max_dist = 10000;

float globals::smoothness_x = 3;
float globals::smoothness_y = 1.5;

float globals::stabilizer_x = 0;
float globals::stabilizer_y = 0;

float globals::prediction_x = 5.5;
float globals::prediction_y = 6.5;

float globals::shake_value = 5;

float globals::range = 0;
float globals::range_mult = 0;

float globals::sensitivity = 1.0f;

int globals::fov = 100;
int globals::deadzone_value = 50;

bool globals::fov_on = false;
bool globals::disable_outside_fov = false;
bool globals::show_auth = false;

bool globals::healthcheck = false;
bool globals::knock_check = false;
bool globals::team_check = false;
bool globals::team_check_esp = false;

CKeybind globals::aimbotkey{ "aimkey" };
TextEditor globals::lua_editor = TextEditor();
silence::LuaVM globals::luaVM = silence::LuaVM();