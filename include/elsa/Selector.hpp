//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//
//  Selector.hpp
//  Created 2015-05-15
//

#pragma once



namespace elsa {

class selector {
    friend class state;
   
    base_state state;
    std::vector<std::string> path {};
    
    selector(base_state state):
    state(state) {
    }
    selector(base_state state, std::string name, std::vector<std::string> path):
    state(state), path(path) {
        path.push_back(name);
    }
    
    void traverse_root(const std::size_t v_index = 0, const int s_index = LUA_GLOBALSINDEX) const {
        if(v_index < path.size() - 1) {
            lua_pushstring(state, path.at(v_index).c_str());
            lua_rawget(state, s_index);
            traverse_root(v_index + 1, -2);
        }
    }
    void traverse(const std::size_t v_index = 0, const int s_index = LUA_GLOBALSINDEX) const {
        if(v_index < path.size()) {
            lua_pushstring(state, path.at(v_index).c_str());
            lua_rawget(state, s_index);
            traverse(v_index + 1, -2);
        }
    }

    
public:

    selector(base_state state, std::string name):
    state(state) {
        path.push_back(name);
    }

    inline selector operator[](std::string name) & {
        return selector {state, name, path};
    }
    inline selector&& operator[](std::string name) && {
        path.push_back(name);
        return std::move(*this);
    }


    template<typename... Arg>
    selector& operator()(Arg&&... args) {
        utility::stack_guard guard {state};
        traverse();
        utility::push(state, std::forward(args)...);
        if(lua_pcall(state, utility::arity<Arg...>::value, LUA_MULTRET, 0)) {
            std::string error = lua_tostring(state, -1);
            lua_pop(state, 1);
            throw std::runtime_error("Could not call: " + error);
        }
        return *this;
    }
    
    template<typename... Ret, typename... Arg>
    auto call(Arg&&... args) {
        utility::stack_guard guard {state};
        traverse();
        utility::push(state, std::forward(args)...);
        if(lua_pcall(state, utility::arity<Arg...>::value, utility::arity<Ret...>::value, 0)) {
            std::string error = lua_tostring(state, -1);
            lua_pop(state, 1);
            throw std::runtime_error("Could not call: " + error);
        }
        return utility::get<Ret...>(state);
        //auto ret = utility::pop<Ret...>(state);
        //if(path.size()) lua_pop(state, (int)path.size() - 1);
        //return ret;
    }
    template<typename... Ret>
    auto call() {
        utility::stack_guard guard {state};
        traverse();
        if(lua_pcall(state, 0, utility::arity<Ret...>::value, 0)) {
            std::string error = lua_tostring(state, -1);
            lua_pop(state, 1);
            throw std::runtime_error("Could not call: " + error);
        }
        return utility::get<Ret...>(state);
        //auto ret = utility::pop<Ret...>(state);
        //if(path.size()) lua_pop(state, (int)path.size() - 1);
        //return ret;
    }
    
    
    auto call2() {
        return result(state);
    }
    
    
    template<typename T>
    explicit operator T() const {
        utility::stack_guard guard {state};
        traverse();
        return utility::get<T>(state);
    }
    

};

}
