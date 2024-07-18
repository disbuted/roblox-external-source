#pragma once
#include "../logs/logs.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

namespace silence
{
	namespace utils
	{
		namespace datamodel
		{
			std::uint64_t get_game_address();
			std::uint64_t get_place_id();
		}
	}
}