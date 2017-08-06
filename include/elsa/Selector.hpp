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
    selector(base_state state, std::string name, std::vector<std::string> path_):
    state(state), path(path_) {
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

    inline auto operator[](std::string name) & {
        return selector {state, name, path};
    }
    inline auto operator[](std::string name) && {
        path.push_back(name);
        return *this;
    }

    
    template<typename... Ret, typename... Arg>
    auto call(Arg&&... args) {
        utility::stack_guard guard {state};
        traverse();
        utility::push(state, std::forward<Arg>(args)...);
        if(lua_pcall(state, utility::arity<Arg...>::value, utility::arity<Ret...>::value, 0)) {
            std::string error = lua_tostring(state, -1);
            lua_pop(state, 1);
            throw std::runtime_error("Could not call: " + error);
        }
        return utility::get<Ret...>(state);
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
    }
    
    template<typename... Arg>
    class result {
        friend class selector;
        
        selector* sel;
        bool called {false};
        std::tuple<Arg...> args;
        
        explicit result(selector* sel, Arg&&... args):
            sel(sel), args(args...) {}
        
        result(const result&) = delete;
        result& operator=(result) = delete;
    public:
        ~result() noexcept(false) {
            if(!called) try {
                std::apply([&](auto&&... args) -> auto {
                    return sel->call<>(std::forward<Arg>(args)...);
                }, args);
            }
            catch(const std::exception& e) {
                // TODO: change the way errors are handled
                std::cerr << e.what() << std::endl;
            }
        }
        
        template<typename T,
            typename = std::enable_if_t<!std::is_reference<T>::value>,
            typename = std::enable_if_t<utility::arity<T>::value == 1>
        >
        inline operator const T() && {
            called = true;
            return std::apply([&](auto&&... args) -> auto {
                return sel->call<T>(std::forward<Arg>(args)...);
            }, args);
        }

        template<typename... T,
            typename = std::enable_if_t<utility::none<std::is_reference<T>::value...>::value>
        >
        inline operator const std::tuple<T...>() && {
            called = true;
            return std::apply([&](auto&&... args) -> auto {
                return sel->call<T...>(std::forward<Arg>(args)...);
            }, args);
        }

    };
    
    template<typename... Arg>
    auto operator()(Arg&&... args) {
        return result<Arg...>(this, std::forward<Arg>(args)...);
    }
    
    
    template<typename T>
    explicit operator T() const {
        utility::stack_guard guard {state};
        traverse();
        return utility::get<T>(state);
    }
    
    bool operator==(selector&& rhs) const {
        return state == state && path == path;
    }
                
    template<typename T>
    bool operator==(T&& rhs) const {
        utility::stack_guard guard {state};
        traverse();
        return utility::get<T>(state);
    }

};


}
