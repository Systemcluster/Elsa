//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//
//  State.hpp
//  Created 2015-05-14
//

#pragma once


#include "Utility.hpp"
#include "BaseState.hpp"
#include "Selector.hpp"


namespace elsa {

#if defined(LUAJIT_VERSION) && defined(DEBUG)
namespace utility {
static int wrap_exceptions(lua_State*, lua_CFunction);
}
#endif

class State: public BaseState {
public:
    
    State(bool open_libs = false): BaseState(luaL_newstate(), true) {
        if(!state) throw std::runtime_error("Could not create Lua state");
        if(open_libs) luaL_openlibs(state);
#if defined(LUAJIT_VERSION) && defined(DEBUG)
        lua_pushlightuserdata(state, (void*)utility::wrap_exceptions);
        luaJIT_setmode(state, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
        lua_pop(state, 1);
#endif
    }
    
    using BaseState::BaseState;
    using BaseState::operator=;
    
    void CollectGarbage() {
        lua_gc(state, LUA_GCCOLLECT, 0);
    }
    void ClearStack() {
        lua_settop(state, 0);
    }
    
    void operator()(const std::string& code) {
        int status = luaL_loadstring(state, code.c_str()) || lua_pcall(state, 0, LUA_MULTRET, 0);
        if(status != 0) {
            std::string error = lua_tostring(state, -1);
            lua_settop(state, 0);
            throw std::runtime_error("Could not load string: " + error);
        }
        lua_settop(state, 0);
    }
    template<typename... T>
    auto Call(const std::string& code) {
        int status = luaL_loadstring(state, code.c_str()) || lua_pcall(state, 0, utility::Type<T...>::arity, 0);
        if(status != 0) {
            std::string error = lua_tostring(state, -1);
            lua_settop(state, 0);
            throw std::runtime_error("Could not load string: " + error);
        }
        return utility::Pop<T...>(state);
    }
    
    void Load(const std::string& file) {
        int status = luaL_loadfile(state, file.c_str()) || lua_pcall(state, 0, LUA_MULTRET, 0);
        if(status != 0) {
            std::string error = lua_tostring(state, -1);
            lua_settop(state, 0);
            throw std::runtime_error("Could not load file " + file + ": " + error);
        }
        lua_settop(state, 0);
    }
    
    Selector operator[](const std::string& name) {
        return Selector(*this, name);
    }
    Selector Select(const std::string& name, const char delim = '.') {
        std::istringstream str(name);
        std::string buf;
        while(std::getline(str, buf, delim)) {
            // TODO
        }
        return Selector(*this, name);
    }
    template<typename... T>
    Selector Select(const std::string& name, const std::string& name2, T... name3) {
        // TODO
    }
    template<typename... T>
    Selector Select(const std::string& name, const int name2, T... name3) {
        // TODO
    }
};


#if defined(LUAJIT_VERSION) && defined(DEBUG)
namespace utility {
static int wrap_exceptions(lua_State* state, lua_CFunction f)
{
    try {
        return f(state);
    } catch (const char *s) {
        lua_pushstring(state, s);
    } catch (std::exception& e) {
        lua_pushstring(state, e.what());
    } catch (...) {
        lua_pushliteral(state, "caught (...)");
    }
    return lua_error(state);
}
}
#endif

}
