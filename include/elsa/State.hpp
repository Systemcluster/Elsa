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
#include "Definitions.hpp"
#include "BaseState.hpp"
#include "Result.hpp"
#include "Selector.hpp"



namespace elsa {

#if defined(LUAJIT_VERSION) && defined(DEBUG)
namespace utility {
static int wrap_exceptions(lua_State*, lua_CFunction);
}
#endif

class state: public base_state {
public:
    
    state(bool open_libs = false): base_state(luaL_newstate(), true) {
        if(!lstate) throw std::runtime_error("Could not create Lua lstate");
        if(open_libs) luaL_openlibs(lstate);
#if defined(LUAJIT_VERSION) && defined(DEBUG)
        lua_pushlightuserdata(lstate, (void*)utility::wrap_exceptions);
        luaJIT_setmode(lstate, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
        lua_pop(lstate, 1);
#endif
    }
    
    using base_state::base_state;
    using base_state::operator=;
    
    
    void collect_garbage() {
        lua_gc(lstate, LUA_GCCOLLECT, 0);
    }
    void clear_stack() {
        lua_settop(lstate, 0);
    }
    
    void operator()(const std::string& code) {
        utility::stack_guard guard {*this};
        int status = luaL_loadstring(lstate, code.c_str()) || lua_pcall(lstate, 0, LUA_MULTRET, 0);
        if(status != 0) {
            std::string error = lua_tostring(lstate, -1);
            throw std::runtime_error("Could not load string: " + error);
        }
    }
    template<typename... Ret>
    auto call(const std::string& code) {
        utility::stack_guard guard {*this};
        int status = luaL_loadstring(lstate, code.c_str()) || lua_pcall(lstate, 0, utility::arity<Ret...>::value, 0);
        if(status != 0) {
            std::string error = lua_tostring(lstate, -1);
            lua_settop(lstate, 0);
            throw std::runtime_error("Could not load string: " + error);
        }
        return utility::get<Ret...>(lstate);
        //return utility::pop<Ret...>(lstate);
    }
    
    void load(const std::string& file) {
        utility::stack_guard guard {*this};
        int status = luaL_loadfile(lstate, file.c_str()) || lua_pcall(lstate, 0, LUA_MULTRET, 0);
        if(status != 0) {
            std::string error = lua_tostring(lstate, -1);
            lua_settop(lstate, 0);
            throw std::runtime_error("Could not load file " + file + ": " + error);
        }
    }
    
    template<typename T>
    selector operator[](T&& name) {
        return selector {*this, name};
    }
    
    template<typename... T>
    selector select(T&&... name) {
        selector s {*this};
        ((s = s[name]), ...);
        return s;
    }
    template<typename T>
    selector select(T&& name, const char delim = '.') {
        std::istringstream str(name);
        std::string buf;
        selector s {*this};
        while(std::getline(str, buf, delim)) {
            s = s[buf];
        }
        return s;
    }
    
};


#if defined(LUAJIT_VERSION) && defined(DEBUG)
namespace utility {
static int wrap_exceptions(lua_State* lstate, lua_CFunction f)
{
    try {
        return f(lstate);
    } catch (const char *s) {
        lua_pushstring(lstate, s);
    } catch (std::exception& e) {
        lua_pushstring(lstate, e.what());
    } catch (...) {
        lua_pushliteral(lstate, "caught (...)");
    }
    return lua_error(lstate);
}
}
#endif

}
