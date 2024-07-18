#pragma once

#include "../classes/classes.hpp"

namespace silence
{
	namespace roblox
	{

		silence::roblox::instance_t get_closest_player_to_cursor();
		silence::roblox::vector2_t get_relative_player_to_pos(silence::roblox::vector2_t closest_player);
		void hook_aimbot();
	}
}