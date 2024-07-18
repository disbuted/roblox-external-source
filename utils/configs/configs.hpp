#pragma once

#include <ShlObj.h>
#include <string>

namespace silence
{
	namespace utils
	{
		static std::string appdata_path()
		{
			wchar_t path[260];
			if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path)))
			{
				std::wstring wstrPath(path);
				return std::string(wstrPath.begin(), wstrPath.end());
			}
			return "";
		}

		class configs {
		public:
			static void save(const char *name);
			static void load(const char *name);
		};
	}
}