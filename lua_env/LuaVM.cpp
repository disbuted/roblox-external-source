/*
CUSTOM LUA ENV MAIN CODE - mogus
*/
#include "LuaVM.hpp"
#include <sol/sol.hpp>
#include "../roblox/classes/classes.hpp"
#include "../roblox/globals/globals.hpp"

namespace silence
{
	// abstract class to represent and initialize our lua environnement
	void LuaVM::init()
	{
		// normally io/os packages should be disabled , thus preventing any rce
		luaState.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);

		// register our instance type
		sol::usertype<roblox::instance_t> instance_lua = luaState.new_usertype<roblox::instance_t>(
			"Instance",
			sol::constructors<roblox::instance_t(), roblox::instance_t(uint64_t)>()
		);

		instance_lua["Name"] = sol::property(&roblox::instance_t::name);
		instance_lua["FindFirstChild"] = &roblox::instance_t::find_first_child;
		instance_lua["GetChildren"] = &roblox::instance_t::children;


		luaState["Game"] = globals::datamodel;
	}

	void LuaVM::RunScriptSafe(std::string script)
	{
		luaState.script(script);
	}
}