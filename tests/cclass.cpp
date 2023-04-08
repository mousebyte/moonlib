#include "tests.h"
extern "C" {

int CClass_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    return 0;
}

static const luaL_Reg CClass_funcs[] = {
    {"foo", foo        },
    {"new", CClass_init},
    {NULL,  NULL       }
};

int DCClass_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    lua_setfield(L, 1, "str");
    return 0;
}

static const luaL_Reg DCClass_funcs[] = {
    {"foo", foo         },
    {"new", DCClass_init},
    {NULL,  NULL        }
};
}

TEST_CASE("C Classes") {
    using doctest::String;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_moonaux(L);

    SUBCASE("Basic C Class") {
        luaC_newclass(L, "CClass", NULL, CClass_funcs);
        REQUIRE(luaC_isclass(L, -1));
        lua_pop(L, 1);

        lua_pushnumber(L, 7);
        luaC_construct(L, 1, "CClass");
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "CClass"));

        lua_pushnumber(L, 3);
        luaC_mcall(L, "foo", 1, 1);
        REQUIRE(lua_tonumber(L, -1) == 21);
        lua_pop(L, 2);
    }

    SUBCASE("Derived C Class") {
        moonL_dofile(L, "Base.moon");
        REQUIRE(luaC_registerclass(L, -1));
        lua_pop(L, 1);

        luaC_newclass(L, "DCClass", "Base", DCClass_funcs);
        REQUIRE(luaC_isclass(L, -1));

        REQUIRE(lua_getfield(L, -1, "var") == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Eek!");
        lua_pop(L, 2);

        lua_pushstring(L, "I have C functions!");
        lua_pushnumber(L, 12);
        luaC_construct(L, 2, "DCClass");
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "DCClass"));

        lua_pushnumber(L, 2);
        luaC_mcall(L, "foo", 1, 1);
        REQUIRE(lua_tonumber(L, -1) == 24);
        lua_pop(L, 1);

        lua_pushnumber(L, 11.2);
        luaC_mcall(L, "squeak", 1, 1);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "n is now 11.2, squeak!");
        lua_pop(L, 2);
    }

    lua_close(L);
}
