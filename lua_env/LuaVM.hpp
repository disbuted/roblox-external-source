#ifndef LUAVM_HPP // fix redfinition
#define LUAVM_HPP
#include "sol/sol.hpp"

namespace silence
{
	class LuaVM
	{
	public:
		sol::state luaState;

		void init();
		void RunScriptSafe(std::string script); // prevent rce and allow to return error type & numbers
	};
}

#endif