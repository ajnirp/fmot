#include "matchWithLua.hpp"

bool match( string test_string, string pattern ){

	// Create a new Lua state.
	lua_State *lua_state;
	lua_state = luaL_newstate( );

	// Loading Lua libraries.
	static const luaL_Reg lualibs[ ] =
	{
		{ "string", luaopen_string },
		{ NULL, NULL }
	};

	const luaL_Reg *lib = lualibs;
	for( ; lib->func != NULL; lib++ ){
		luaL_requiref( lua_state, lib->name, lib->func, 1 );
		lua_settop( lua_state, 0 );
	}

	// Start the arg table in Lua.
	lua_createtable( lua_state, 2, 0 );

	lua_pushnumber( lua_state, 1 );
	lua_pushstring( lua_state, test_string.c_str( ) );
	lua_settable( lua_state, -3 );

	lua_pushnumber( lua_state, 2 );
	lua_pushstring( lua_state, pattern.c_str( ) );
	lua_settable( lua_state, -3 );

	lua_setglobal( lua_state, "arg" );

	// Loading the Lua script.
	int status = luaL_loadfile( lua_state, "script.lua" );
	int result = 0;

	if( status == LUA_OK )
		result = lua_pcall( lua_state, 0, LUA_MULTRET, 0 );

	bool patternMatchResult;

	while( lua_gettop( lua_state ) ){
		patternMatchResult = lua_toboolean( lua_state, lua_gettop( lua_state ) );
		lua_pop( lua_state, 1 );
	}

	// Closing the Lua state.
	lua_close( lua_state );

	return patternMatchResult;
}
