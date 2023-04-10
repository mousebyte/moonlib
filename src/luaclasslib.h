/// @file moonaux.h

#ifndef LUACLASSLIB_H
#define LUACLASSLIB_H

#include <lauxlib.h>
#include <lua.h>

/**
 * @brief A user data class constructor. Implementations of this function should
 * push one value onto the stack, a userdata with at least one user value.
 *
 * @param L The Lua state.
 */
typedef void (*luaC_Constructor)(lua_State *L);

/**
 * @brief A user data class destructor. Implementations of this function should
 * perform any necessary resource cleanup for the user data object represented
 * by `p`. Do not free p, that is handled by the Lua garbage collector.
 *
 * @param p A pointer to the user data to destruct.
 */
typedef void (*luaC_Destructor)(void *p);

/// Header for luaC_Class objects.
#define LUAC_CLASS_HEADER                \
    /** The name of the class. */        \
    const char      *name;               \
    /** The name of the parent */        \
    const char      *parent;             \
    /** Whether to allow construction */ \
    /** by calling the class object */   \
    int              user_ctor;          \
    /** The class allocator */           \
    luaC_Constructor alloc;              \
    /** The class garbage collector */   \
    luaC_Destructor  gc;                 \
    /** The class methods */             \
    luaL_Reg        *methods;

/// Contains information about a user data class.
typedef struct {
    LUAC_CLASS_HEADER
} luaC_Class;

/**
 * @brief Pushes onto the stack the value `t[k]` where `t` is the table stored
 * in the given user value of the userdata at the given index, and `k` is the
 * value at the top of the stack. The access is raw (does not use the `__index`
 * metavalue).
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return The type of the pushed value.
 */
static inline int luaC_uvrawget(lua_State *L, int idx, int uv) {
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);        // put uv behind key
        ret = lua_rawget(L, -2);  // get the value
    } else lua_pushnil(L);        // otherwise push nil
    lua_remove(L, -2);            // remove the uv
    return ret;
}

/**
 * @brief Does the equivalent of `t[k] = v`, where `t` is the table stored in
 * the given user value of the userdata at the given index, `v` is the value on
 * top of the stack, and `k` is the value just below the top. The access is raw
 * (does not use the `__index` * metavalue).
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
static inline int luaC_uvrawset(lua_State *L, int idx, int uv) {
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -3);  // put uv before key and value
        lua_rawset(L, -3);  // set the value
        return 1;
    }
    lua_pop(L, 3);  // otherwise pop uv, key, and value
    return 0;
}

/**
 * @brief Pushes onto the stack the value `t[k]` where `t` is the table stored
 * in the given user value of the userdata at the given index, and `k` is the
 * pointer `p` represented as a light userdata. The access is raw (does not use
 * the `__index` * metavalue).
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return The type of the pushed value.
 */
static inline int luaC_uvrawgetp(lua_State *L, int idx, int uv, const void *p) {
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        ret = lua_rawgetp(L, -1, p);  // get the value
    } else lua_pushnil(L);            // otherwise push nil
    lua_remove(L, -2);                // remove the uv
    return ret;
}

/**
 * @brief Does the equivalent of `t[k] = v`, where `t` is the table stored in
 * the given user value of the userdata at the given index, `v` is the value on
 * top of the stack, and `k` is the pointer `p` represented as a light userdata.
 * The access is raw (does not use the `__index` * metavalue).
 *
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
static inline int luaC_uvrawsetp(lua_State *L, int idx, int uv, const void *p) {
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);      // put uv before value
        lua_rawsetp(L, -2, p);  // set the value
        return 1;
    }
    lua_pop(L, 2);  // otherwise pop uv and value
    return 0;
}

/**
 * @brief Pushes onto the stack the value `t[k]` where `t` is the table stored
 * in the given user value of the userdata at the given index, and `k` is the
 * value at the top of the stack.
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return The type of the pushed value.
 */
static inline int luaC_uvget(lua_State *L, int idx, int uv) {
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);          // put uv behind key
        ret = lua_gettable(L, -2);  // get the value
    } else lua_pushnil(L);          // otherwise push nil
    lua_remove(L, -2);              // remove the uv
    return ret;
}

/**
 * @brief Does the equivalent of `t[k] = v`, where `t` is the table stored in
 * the given user value of the userdata at the given index, `v` is the value on
 * top of the stack, and `k` is the value just below the top.
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
static inline int luaC_uvset(lua_State *L, int idx, int uv) {
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -3);    // put uv before key and value
        lua_settable(L, -3);  // set the value
        return 1;
    }
    lua_pop(L, 3);  // otherwise pop uv, key, and value
    return 0;
}

/**
 * @brief Pushes onto the stack the value `t[k]` where `t` is the table stored
 * in the given user value of the userdata at the given index.
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 * @param k The name of the field.
 *
 * @return The type of the pushed value.
 */
static inline int
luaC_getuvfield(lua_State *L, int idx, int uv, const char *k) {
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        ret = lua_getfield(L, -1, k);  // get value if uv is table
    } else lua_pushnil(L);             // otherwise push nil
    lua_remove(L, -2);                 // remove the uv
    return ret;
}

/**
 * @brief Does the equivalent of `t[k] = v`, where `t` is the table stored in
 * the given user value of the userdata at the given index, and `v` is the value
 * on top of the stack.
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 * @param k The name of the field.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
static inline int
luaC_setuvfield(lua_State *L, int idx, int uv, const char *k) {
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);       // put uv behind value
        lua_setfield(L, -2, k);  // set the value
        lua_pop(L, 1);           // pop the uv
        return 1;
    }
    lua_pop(L, 2);  // otherwise pop uv and value
    return 0;
}

/**
 * @brief Call a method of an object, passing the object as the first argument.
 *
 * @param L The Lua state.
 * @param method The name of the method to call.
 * @param nargs The number of arguments.
 * @param nresults The number of results.
 */
static inline void
luaC_mcall(lua_State *L, const char *method, int nargs, int nresults) {
    lua_getfield(L, -nargs - 1, method);  // get the method
    lua_pushvalue(L, -nargs - 2);         // push a copy of the object
    lua_rotate(L, -nargs - 2, 2);         // rotate args to top
    lua_call(L, nargs + 1, nresults);
}

/**
 * @brief Call a method of an object in protected mode, passing the object as
 * the first argument.
 *
 * @param L The Lua state.
 * @param method The name of the method to call.
 * @param nargs The number of arguments.
 * @param nresults The number of results.
 * @param msgh The stack index of the message handler, or 0 if none is to be
 * used.
 *
 * @return The pcall status code.
 */
static inline int luaC_pmcall(
    lua_State  *L,
    const char *method,
    int         nargs,
    int         nresults,
    int         msgh) {
    msgh = lua_absindex(L, msgh);
    lua_getfield(L, -nargs - 1, method);  // get the method
    lua_pushvalue(L, -nargs - 2);         // push a copy of the object
    lua_rotate(L, -nargs - 2, 2);         // rotate args to top
    return lua_pcall(L, nargs + 1, nresults, msgh);
}

/**
 * @brief Checks if the value at the given index is an instance of a class.
 *
 * @param L The Lua state.
 * @param index The stack index to check.
 *
 * @return 1 if the value is an instance of a class, and 0 otherwise.
 */
int luaC_isobject(lua_State *L, int index);

/**
 * @brief Checks if the value at the given index is a class.
 *
 * @param L The Lua state.
 * @param index The stack index to check.
 *
 * @return 1 if the value is a class, and 0 otherwise.
 */
int luaC_isclass(lua_State *L, int index);

/**
 * @brief Checks if the value at the given index is an instance of the class
 * named *name*.
 *
 * @param L The Lua state.
 * @param arg The stack index to check.
 * @param name The name of the class.
 *
 * @return 1 if the value is an instance of *name*, and 0 otherwise.
 */
int luaC_isinstance(lua_State *L, int arg, const char *name);

/**
 * @brief Checks if the function argument *arg* is an instance of the userdata
 * class named *name* and returns the userdata's memory-block address.
 *
 * @param L The Lua state.
 * @param arg The arg to check.
 * @param name The name of the class.
 *
 * @return A pointer to the userdata.
 */
void *luaC_checkuclass(lua_State *L, int arg, const char *name);

/**
 * @brief Pushes onto the stack the class registered under the given *name*.
 *
 * @param L The Lua state.
 * @param name The class name.
 *
 * @return The type of the pushed value.
 */
int luaC_getclass(lua_State *L, const char *name);

/**
 * @brief Gets a pointer to the user data class associated with the class at the
 * given stack index.
 *
 * @param L The Lua state.
 * @param index The stack index of the class.
 *
 * @return A pointer to the user data class, or NULL if none was found.
 */
luaC_Class *luaC_getuclass(lua_State *L, int index);

/**
 * @brief Construct an instance of a class.
 *
 * @param L The Lua state.
 * @param nargs The number of arguments on the stack to pass to the constructor.
 * @param name The name of the class.
 *
 * @return 1 if the object was successfully constructed, and 0 otherwise.
 */
int luaC_construct(lua_State *L, int nargs, const char *name);

/**
 * @brief Replaces a class method with a closure of the given C function *f*,
 * with the previous method as its only upvalue.
 *
 * @param L The Lua state.
 * @param index The index of the class object.
 * @param method The method to replace.
 * @param f The C function to replace the method with.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
int luaC_injectmethod(
    lua_State    *L,
    int           index,
    const char   *method,
    lua_CFunction f);

/**
 * @brief When called from an injected index function, calls (or indexes) the
 * original index and pushes the result onto the stack.
 *
 * @param L The Lua state.
 *
 * @return The type of the value pushed onto the stack.
 */
int luaC_deferindex(lua_State *L);

/**
 * @brief When called from an injected newindex function, calls the original
 * newindex if it exists.
 *
 * @param L The Lua state.
 */
void luaC_defernewindex(lua_State *L);

/**
 * @brief Pushes onto the stack the value of a field at the given depth up the
 * heirarchy. If *depth* is greater than the number of parents above the target
 * object, pushes nil.
 *
 * @param L The Lua state.
 * @param index The index of the object in the stack.
 * @param depth The parent depth.
 * @param name The name of the field.
 *
 * @return The type of the value pushed onto the stack.
 */
int luaC_getparentfield(lua_State *L, int index, int depth, const char *name);

/**
 * @brief Calls a parent class method, passing all values on the stack as
 * arguments. Leaves the stack in its previous state. Should only be used in C
 * class methods, in which the first stack index is the object on which the
 * method was invoked.
 *
 * @param L The Lua state.
 * @param name The name of the method.
 * @param nresults The number of results to return.
 */
void luaC_super(lua_State *L, const char *name, int nresults);

/**
 * @brief Adds the class at the given stack index to the class
 * registry, along with its parents, if not present already.
 *
 * @param L The Lua state.
 * @param index The stack index of the class.
 *
 * @return 1 if the class was successfully registered, and 0 otherwise.
 */
int luaC_registerclass(lua_State *L, int index);

/**
 * @brief Creates and registers the class defined by the luaC_Class at the
 * specified stack index.
 *
 * @param L The Lua state.
 * @param index The stack index of the luaC_Class.
 *
 * @return 1 if the class was successfully registered, and 0 otherwise.
 */
int luaC_registeruclass_fromstack(lua_State *L, int idx);

/**
 * @brief Helper method for registering a luaC_Class as a light userdata.
 *
 * @param L The Lua state.
 * @param c The class to register.
 *
 * @return 1 if the class was successfully registered, and 0 otherwise;
 */
static inline int luaC_registeruclass(lua_State *L, luaC_Class *c) {
    lua_pushlightuserdata(L, c);
    return luaC_registeruclass_fromstack(L, -1);
}

/**
 * @brief Helper method for creating and registering a simple luaC_Class as a
 * full userdata. Useful for when you're using stock classes and don't want to
 * define your luaC_Class with static linkage.
 *
 * @param L The Lua state.
 * @param name The class name.
 * @param parent The parent class name. Must be in the registry.
 * @param methods The class methods.
 *
 * @return 1 if the class was successfully created and registered, and 0
 * otherwise.
 */
static inline int luaC_newclass(
    lua_State  *L,
    const char *name,
    const char *parent,
    luaL_Reg   *methods) {
    luaC_Class *cls = (luaC_Class *)lua_newuserdatauv(L, sizeof(luaC_Class), 0);
    cls->name       = name;
    cls->parent     = parent;
    cls->user_ctor  = 1;
    cls->alloc      = NULL;
    cls->gc         = NULL;
    cls->methods    = methods;
    return luaC_registeruclass_fromstack(L, -1);
}

/**
 * @brief Loads the Lua class library user functions into the global namespace.
 *
 * @param L The Lua state.
 */
void luaopen_class(lua_State *L);

/**
 * @brief Calls the init function of the parent class.
 *
 * @param L The Lua state.
 */
#define luaC_superinit(L) luaC_super((L), "__init", 0);

/**
 * @brief Replaces the index method of a class with a closure of the given C
 * function *f*, with the previous index as its only upvalue.
 *
 * @param L The Lua state.
 * @param i The index of the class object.
 * @param f The C function to replace the method with.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
#define luaC_injectindex(L, i, f) luaC_injectmethod((L), (i), "__index", (f))

/**
 * @brief Replaces the newindex method of a class with a closure of the given C
 * function *f*, with the previous newindex as its only upvalue.
 *
 * @param L The Lua state.
 * @param i The index of the class object.
 * @param f The C function to replace the method with.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
#define luaC_injectnewindex(L, i, f) \
    luaC_injectmethod((L), (i), "__newindex", (f))

/**
 * @brief Pushes the class of the object at the given index onto the stack.
 *
 * @param L The Lua state.
 * @param index The index of the object on the stack.
 */
#define luaC_pushclass(L, index) lua_getfield((L), (index), "__class")

#endif
