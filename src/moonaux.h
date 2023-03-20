#include <lauxlib.h>
#include <lua.h>

typedef void *(*moonL_Constructor)(lua_State *);
typedef void (*moonL_Destructor)(void *);

#define MOONL_UCLASS_HEADER  \
    const char       *name;  \
    moonL_Constructor alloc; \
    moonL_Destructor  gc;

typedef struct moonL_UClass {
    MOONL_UCLASS_HEADER
} moonL_UClass;

int                moonL_dofile(lua_State *, const char *);
static inline void moonL_print(lua_State *L, int index) {
    lua_pushvalue(L, index);
    luaL_dostring(L, "return require('moon')");
    lua_getfield(L, -2, "p");
    lua_insert(L, -4);
    lua_pop(L, 2);
    lua_call(L, 1, 0);
}
void          moonL_mcall(lua_State *, const char *, int, int);
int           moonL_pmcall(lua_State *, const char *, int, int, int);
int           moonL_isobject(lua_State *, int);
int           moonL_isclass(lua_State *, int);
int           moonL_isinstance(lua_State *, int, const char *);
void         *moonL_checkuclass(lua_State *, int, const char *);
int           moonL_getclass(lua_State *, const char *);
moonL_UClass *moonL_getuclass(lua_State *, int);
int           moonL_registerclass(lua_State *, int);
int           moonL_construct(lua_State *, int, const char *);
int           moonL_injectmethod(lua_State *, int, const char *, lua_CFunction);
int           moonL_deferindex(lua_State *);
void          moonL_defernewindex(lua_State *);
int           moonL_newclass(
              lua_State *,
              const char *,
              const char *,
              const luaL_Reg *,
              moonL_UClass *);
void luaopen_moonaux(lua_State *);

#define moonL_injectindex(L, i, f) moonL_injectmethod((L), (i), "__index", f)
#define moonL_injectnewindex(L, i, f) \
    moonL_injectmethod((L), (i), "__newindex", f)
#define moonL_pushclass(L, index) lua_getfield((L), (index), "__class")
