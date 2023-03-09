#include <lua.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>

#include <new>
#include <vector>
#include <iostream>

int main(int argc, char **argv)
{
	const char *filename = argc > 1 ? argv[1] : "reality.lua";
	lua_State* L = luaL_newstate();
	luaopen_base(L);
	lua_settop(L, 0);
	int retcode;

	retcode = luaL_dofile(L, filename);
	if (retcode) {
		printf("Failed to execute %s\n", filename);
		return retcode;
	}
	
	lua_getglobal(L, "objects");
	if (!lua_istable(L, 1)) {
		printf("unable to found global table 'objects'\n");
		return 1;
	}
	
	int tsize = lua_rawlen(L, 1);
	printf("objects: %d\n", tsize);
	for (int i = 1; i <= tsize; ++i)
	{
		lua_rawgeti(L, 1, i);
		if (!lua_istable(L, 2)) {
			printf("(object %d isn't a table)\n", i);
			continue;
		}
		
		size_t objlen = lua_rawlen(L, 2);
		const char *warn = "";
		if (objlen < 4) {
			printf("(object %d hasn't enough fields)\n", i);
			continue;
		} else if (objlen != 4)
			warn = " (extra fields discarded)";
		
		lua_rawgeti(L, 2, 1);
		lua_rawgeti(L, 2, 2);
		lua_rawgeti(L, 2, 3);
		lua_rawgeti(L, 2, 4);
		
		printf("{\"%s\": (%f, %f, %f)}%s\n",
			lua_tostring(L, 3),
			lua_tonumber(L, 4),
			lua_tonumber(L, 5),
			lua_tonumber(L, 6),
			warn
		);
		lua_pop(L, 5);
	}
	lua_close(L);
}
