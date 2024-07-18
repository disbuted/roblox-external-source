#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace silence
{
	namespace offsets
	{
		constexpr std::uint32_t size = 0x8; /* 8 bytes */
		constexpr std::uint32_t name = 0x48;
		constexpr std::uint32_t children = 0x50;
		constexpr std::uint32_t parent = 0x60;
		constexpr std::uint32_t local_player = 0xC8;
		constexpr std::uint32_t model_instance = 0x2B0;
		constexpr std::uint32_t primitive = 0x158;
		constexpr std::uint32_t position = 0x148;
		constexpr std::uint32_t dimensions = 0x718;
		constexpr std::uint32_t viewmatrix = 0x4B0;
		constexpr std::uint32_t camera_pos = 0xA0;
		constexpr std::uint32_t classname = 0x18;
		constexpr std::uint32_t health = 0x258;
		constexpr std::uint32_t max_health = 0x2A8;
		constexpr std::uint32_t walkspeed = 0x340;
		constexpr std::uint32_t jumpspeed = 0x370;
		constexpr std::uint32_t team = 0x1D8;
		constexpr std::uint32_t gameid = 0x878;
		constexpr std::uint32_t velocity = 0x154;
	}

	namespace roblox
	{
		bool init();

		struct vector2_t final { float x, y; };
		struct vector3_t final { float x, y, z; };
		struct quaternion final { float x, y, z, w; };
		struct matrix4_t final { float data[16]; };

		class instance_t final
		{
		public:

			std::uint64_t self;

			std::string name();
			std::string class_name();
			std::vector<silence::roblox::instance_t> children();
			silence::roblox::instance_t find_first_child(std::string child);

			/* decided to have everything in one class rather than have multiple, it will be harder to manage (im lazy) */

			silence::roblox::instance_t get_local_player();
			silence::roblox::instance_t get_model_instance();
			silence::roblox::instance_t get_team();
			silence::roblox::instance_t get_cframe();
			std::uintptr_t get_gameid();

			silence::roblox::instance_t get_workspace();
			silence::roblox::instance_t get_current_camera();

			silence::roblox::vector2_t get_dimensions();
			silence::roblox::matrix4_t get_view_matrix();
			silence::roblox::vector3_t get_camera_pos();
			silence::roblox::vector3_t get_part_pos();
			silence::roblox::vector3_t get_part_velocity();

			float get_health();
			float get_max_health();

			void set_humanoid_walkspeed(float value);

			float get_ping();
		};

		silence::roblox::vector2_t world_to_screen(silence::roblox::vector3_t world, silence::roblox::vector2_t dimensions, silence::roblox::matrix4_t viewmatrix);
	}
}