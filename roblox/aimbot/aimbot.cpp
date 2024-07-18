#include "aimbot.hpp"

#include <random>
#include <Windows.h>
#include <thread>
#include "../globals/globals.hpp"
#include "../../utils/xorstr/xorstr.hpp"

float calc_dist(silence::roblox::vector2_t first, silence::roblox::vector2_t sec)
{
	return sqrt((first.x - sec.x) * (first.x - sec.x) + (first.y - sec.y) * (first.y - sec.y));
}

bool is_within_deadzone(float x, float y) {
	float distance = sqrt(x * x + y * y);
	return distance <= globals::deadzone_value;
}

silence::roblox::vector2_t add_vector_2(silence::roblox::vector2_t first, silence::roblox::vector2_t sec)
{
	return
	{
		first.x + sec.x,
		first.y + sec.y
	};
}

silence::roblox::vector3_t add_vector_3(silence::roblox::vector3_t first, silence::roblox::vector3_t sec)
{
	return
	{
		first.x + sec.x,
		first.y + sec.y,
		first.z + sec.z
	};
}

silence::roblox::vector3_t div_vector_3(silence::roblox::vector3_t first, silence::roblox::vector3_t sec)
{
	return
	{
		first.x / sec.x,
		first.y / sec.y,
		first.z / sec.z
	};
}

silence::roblox::vector3_t sub_vector_3(silence::roblox::vector3_t first, silence::roblox::vector3_t sec)
{
	return
	{
		first.x - sec.x,
		first.y - sec.y,
		first.z - sec.z
	};
}

silence::roblox::vector2_t vector_sub(silence::roblox::vector2_t one, silence::roblox::vector2_t two)
{
	return { one.x - two.x, one.y - two.y };
}

silence::roblox::vector3_t to_vector3(float value)
{
	return { value, value , value };
}

silence::roblox::vector3_t calculate_velocity(silence::roblox::instance_t player)
{
	silence::roblox::vector3_t old_Position = player.get_part_pos();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	silence::roblox::vector3_t current_Position = player.get_part_pos();
	return div_vector_3((sub_vector_3(current_Position, old_Position)), to_vector3(0.100));
}


silence::roblox::instance_t silence::roblox::get_closest_player_to_cursor()
{
	POINT cursor_point;
	GetCursorPos(&cursor_point);
	ScreenToClient(FindWindowA(0, XorStr("Roblox")), &cursor_point);

	vector2_t cursor =
	{
		static_cast<float>(cursor_point.x),
		static_cast<float>(cursor_point.y)
	};

	auto players = globals::players;

	silence::roblox::instance_t closest_player;

	int min_dist = 9e9;
	std::string teamname;

	auto localplayer = players.get_local_player();
	auto localplayer_team = localplayer.get_team();

	auto dimensions = globals::visualengine.get_dimensions();
	auto viewmatrix = globals::visualengine.get_view_matrix();

	for (auto player : players.children())
	{
		
		if (player.self == localplayer.self)
			continue;



		auto character = player.get_model_instance();
		if (!character.self)
			continue;

		auto humanoid = character.find_first_child("Humanoid");
		if (!humanoid.self)
			continue;

		auto team = player.get_team();

		if (globals::team_check && (team.self == localplayer_team.self))
			continue;

		if (humanoid.get_health() <= 4 && globals::knock_check)
		{
			continue;
		}

		silence::roblox::instance_t head;

		switch (globals::aimpart)
		{
		case 0:
			head = character.find_first_child("Head");
			break;
		case 1:
			head = character.find_first_child("UpperTorso");
			break;
		case 2:
			head = character.find_first_child("HumanoidRootPart");
			break;
		case 3:
			head = character.find_first_child("LowerTorso");
			break;
		}

		if (!head.self)
			continue;

		vector3_t head_pos_3d = head.get_part_pos();

		auto head_pos = silence::roblox::world_to_screen(head_pos_3d, dimensions, viewmatrix);

		float dist = calc_dist(head_pos, cursor);

		if ((!globals::disable_outside_fov || dist < globals::fov) && (min_dist > dist)) {
			closest_player = head;
			min_dist = dist;
		}
	}
	return closest_player;
}

silence::roblox::vector2_t silence::roblox::get_relative_player_to_pos(vector2_t closest_player)
{
	vector2_t ret = { 0, 0 };

	POINT cursor_point;
	GetCursorPos(&cursor_point);
	ScreenToClient(FindWindowA(0, XorStr("Roblox")), &cursor_point);
	vector2_t cursor_pos = { static_cast<float>(cursor_point.x),static_cast<float>(cursor_point.y) };

	ret.x = (closest_player.x - cursor_pos.x) * globals::sensitivity / globals::smoothness_x;
	ret.y = (closest_player.y - cursor_pos.y) * globals::sensitivity / globals::smoothness_y;

	return ret;
}

void run(silence::roblox::instance_t player, silence::roblox::vector3_t resolvedVelocity)
{
	POINT cursor_pos;
	GetCursorPos(&cursor_pos);
	ScreenToClient(FindWindowA(0, "Roblox"), &cursor_pos);

	silence::roblox::vector3_t head_pos_3d;

	auto dimensions = globals::visualengine.get_dimensions();
	auto viewmatrix = globals::visualengine.get_view_matrix();

	if (globals::prediction)
	{
		silence::roblox::vector3_t velocity = globals::resolver ? resolvedVelocity : player.get_part_velocity();


		if (globals::autopred) {

			float ping = player.get_ping();

			std::cout << "Current Ping: " << ping << "ms\n"; 

			float predictionMultiplier = ((ping / 225) * 0.1f) + 0.1f;

			velocity = div_vector_3(velocity, { predictionMultiplier, predictionMultiplier, predictionMultiplier });


			head_pos_3d = add_vector_3(player.get_part_pos(), velocity);


		}

		auto velocity_vec = div_vector_3(velocity, { globals::prediction_x, globals::prediction_y, globals::prediction_x });
		head_pos_3d = add_vector_3(player.get_part_pos(), velocity_vec);
	}
	else
	{
		head_pos_3d = player.get_part_pos();
	}

	auto head_pos = silence::roblox::world_to_screen(head_pos_3d, dimensions, viewmatrix);

	if (head_pos.x == -1)
	{
		return;
	}

	float dist = calc_dist(head_pos, { static_cast<float>(cursor_pos.x), static_cast<float>(cursor_pos.y) });

	float sens = globals::sensitivity;

	silence::roblox::vector2_t relative = { 0, 0 };

	if (globals::shake) {
		int shake_range = static_cast<int>(globals::shake_value * 10);
		relative.x = (head_pos.x - cursor_pos.x + (rand() % shake_range - shake_range / 2)) * sens / globals::smoothness_x;
		relative.y = (head_pos.y - cursor_pos.y + (rand() % shake_range - shake_range / 2)) * sens / globals::smoothness_y;
	}
	else {
		relative.x = (head_pos.x - cursor_pos.x) * sens / globals::smoothness_x;
		relative.y = (head_pos.y - cursor_pos.y) * sens / globals::smoothness_y;
	}

	if (relative.x == -1 || relative.y == -1)
		return;

	float distance_to_deadzone = calc_dist({ static_cast<float>(cursor_pos.x), static_cast<float>(cursor_pos.y) },
		{ static_cast<float>(cursor_pos.x) + relative.x,
		  static_cast<float>(cursor_pos.y) + relative.y });

	if (globals::deadzone && distance_to_deadzone <= globals::deadzone_value) {
		return;
	}

	if (!globals::disable_outside_fov || dist <= globals::fov) {
		INPUT input;
		input.mi.time = 0;
		input.type = INPUT_MOUSE;
		input.mi.mouseData = 0;
		input.mi.dx = relative.x;
		input.mi.dy = relative.y;
		input.mi.dwFlags = MOUSEEVENTF_MOVE;
		SendInput(1, &input, sizeof(input));
	}

}

void silence::roblox::hook_aimbot()
{
	silence::roblox::instance_t saved_player;
	bool is_aimboting = false;
	silence::roblox::vector3_t velocity;
	silence::roblox::instance_t current_player;

	std::thread velocity_calculation([&]()
	{
		while (true)
		{
			__try
			{
				if (!current_player.self || !globals::resolver == true )
					continue;
					
				velocity = calculate_velocity(current_player);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});

	while (true)
	{
		globals::aimbotkey.update();

		if (GetForegroundWindow() == FindWindowA(0, XorStr("Roblox")))
		{
			if (globals::aimbotkey.enabled && globals::aimbot)
			{
				if (globals::sticky_aim && is_aimboting && saved_player.self != 0)
				{
					current_player = saved_player;
				}
				else
				{
					current_player = get_closest_player_to_cursor();
				}

				if (current_player.self != 0 && current_player.get_part_pos().y > 0.1f)
				{
					run(current_player, velocity);
					saved_player = current_player;
					is_aimboting = true;
				}
				else
				{
					is_aimboting = false;
				}
			}
			else
			{
				is_aimboting = false;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}