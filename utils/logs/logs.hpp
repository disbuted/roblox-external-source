#pragma once

#include <vector>
#include <filesystem>
#include <cstdint>
#include <string>

namespace silence
{
	namespace utils
	{
		namespace log
		{
			void debug_log(const std::uint64_t address, const char *name);

			/* roblox file logs */

			std::wstring get_username();

			std::vector<std::filesystem::path> get_roblox_file_logs();
			std::filesystem::path get_latest_log();
		}
	}
}