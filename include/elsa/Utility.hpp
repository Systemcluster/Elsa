//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//
//  Utility.hpp
//  Created 2015-05-14
//

#pragma once

#include <string>
#include <stdexcept>
#include <tuple>
#include <sstream>
#include <utility>
#include <atomic>
#include <vector>
#include <functional>



namespace elsa {
namespace utility {


struct stack_guard {
    explicit stack_guard(lua_State* state):
    state(state), stack_top(lua_gettop(state)) {}
    ~stack_guard() {
#if defined(__cpp_lib_uncaught_exceptions)
        if(!std::uncaught_exceptions())
            lua_settop(state, stack_top);
#else
        if(!std::uncaught_exception())
            lua_settop(state, stack_top);
#endif
        // TODO: remove this stupid test output ;)
        else {
            std::cout << "!unwinding! ";
            lua_settop(state, 0);
        }
    }
private:
    lua_State* state;
    int stack_top;
    stack_guard(const stack_guard&) = delete;
    stack_guard(stack_guard&&) = delete;
    stack_guard& operator=(const stack_guard&) = delete;
    stack_guard& operator=(stack_guard&&) = delete;
};


template<bool... T>
    struct all;
template <>
    struct all<>: std::true_type {};
template<bool... T>
    struct all<false, T...>: std::false_type {};
template<bool... T>
    struct all<true, T...>: all<T...>::type {};

template <bool... T>
    struct none;
template <>
    struct none<>: std::true_type {};
template<bool... T>
    struct none<true, T...>: std::false_type {};
template<bool... T>
    struct none<false, T...>: none<T...>::type {};

template<typename... T>
struct arity {
    static constexpr size_t value { (arity<T>::value + ...) };
};
template<typename T>
struct arity<T> {
    static constexpr size_t value { 1 };
};
template<typename... T>
struct arity<std::tuple<T...>> {
    static constexpr size_t value { (arity<T>::value + ...) };
};
template<>
struct arity<std::tuple<>> {
    static constexpr size_t value { 0 };
};
template<>
struct arity<void> {
    static constexpr size_t value { 0 };
};
template<>
struct arity<> {
    static constexpr size_t value { 0 };
};



//
// Push a value onto the stack.
//

inline void push(lua_State* state) {}
inline void push(lua_State* state, std::nullptr_t) {
    lua_pushnil(state);
}
inline void push(lua_State* state, int value) {
    lua_pushinteger(state, value);
}
inline void push(lua_State* state, unsigned int value) {
#if LUA_VERSION_NUM == 502
    lua_pushunsigned(state, value);
#else
    lua_pushinteger(state, value);
#endif
}
inline void push(lua_State* state, long value) {
    lua_pushinteger(state, value);
}
inline void push(lua_State* state, unsigned long value) {
#if LUA_VERSION_NUM == 502
    lua_pushunsigned(state, value);
#else
    lua_pushinteger(state, value);
#endif
}
inline void push(lua_State* state, float value) {
    lua_pushnumber(state, value);
}
inline void push(lua_State* state, double value) {
    lua_pushnumber(state, value);
}
inline void push(lua_State* state, bool value) {
    lua_pushboolean(state, value);
}
inline void push(lua_State* state, const char* value) {
    lua_pushstring(state, value);
}
inline void push(lua_State* state, std::string value) {
    lua_pushstring(state, value.c_str());
}

template<typename... T>
inline void push(lua_State* state, T&&... values) {
    (push(state, std::forward(values)), ...);
}

template<typename... T, std::size_t... N>
inline void push(lua_State* state, const std::tuple<T...>&& values, std::index_sequence<N...>) {
    (push(state, std::get<N>(std::forward(values))), ...);
}
template<typename... T>
inline void push(lua_State* state, const std::tuple<T...>&& values) {
   push(state, std::forward(values), std::make_index_sequence<sizeof...(T)>());
}



namespace detail {
    template<typename T> inline T get(lua_State* state, int index);
    
    template<> inline int get(lua_State* state, int index) {
        return static_cast<int>(lua_tointeger(state, index));
    }
    template<> inline unsigned int get(lua_State* state, int index) {
#if LUA_VERSION_NUM == 502
        return static_cast<unsigned int>(lua_tounsigned(state, index));
#else
        return static_cast<unsigned int>(lua_tointeger(state, index));
#endif
    }
    template<> inline long get(lua_State* state, int index) {
        return static_cast<long>(lua_tointeger(state, index));
    }
    template<> inline unsigned long get(lua_State* state, int index) {
#if LUA_VERSION_NUM == 502
        return static_cast<unsigned long>(lua_tounsigned(state, index));
#else
        return static_cast<unsigned long>(lua_tointeger(state, index));
#endif
    }
    template<> inline float get(lua_State* state, int index) {
        return static_cast<float>(lua_tonumber(state, index));
    }
    template<> inline double get(lua_State* state, int index) {
        return static_cast<double>(lua_tonumber(state, index));
    }
    template<> inline bool get(lua_State* state, int index) {
        return static_cast<bool>(lua_toboolean(state, index));
    }
    template<> inline std::string get(lua_State* state, int index) {
        return lua_tostring(state, index);
    }
    template<> inline const char* get(lua_State* state, int index) {
        return lua_tostring(state, index);
    }
    
}



template<size_t, typename... T>
struct stack_value {
    using type = std::tuple<T...>;
    static type pop(lua_State* state) {
        constexpr int arg_arity = static_cast<int>(arity<T...>::value);
        int bottom = 0 - arg_arity;
        auto ret = get(state, bottom);
        lua_pop(state, arg_arity);
        return ret;
    }
    static inline type get(lua_State* state, int& index) {
        return std::make_tuple(stack_value<1, T>::get(state, index)...);
    }
};
template<typename... T>
struct stack_value<1, std::tuple<T...>>: stack_value<sizeof...(T), T...> {
};
template<typename T>
struct stack_value<1, T> {
    using type = T;
    static type pop(lua_State* state) {
        auto ret = detail::get<T>(state, -1);
        lua_pop(state, 1);
        return ret;
    }
    static inline type get(lua_State* state, int& index) {
        return detail::get<T>(state, index++);
    }
};
template<typename... T>
struct stack_value<0, T...> {
    using type = void;
    constexpr static type pop(lua_State* state) {
        return;
    }
    constexpr static type get(lua_State* state, int& index) {
        return;
    }
};



//
// Read and return a value or values from the stack at @index.
//
template<typename... T>
inline auto get(lua_State* state, int index = 0 - static_cast<int>(arity<T...>::value)) {
    return stack_value<sizeof...(T), T...>::get(state, index);
}
//
// Pop and return a value or values at the top of the stack.
//
template<typename... T>
inline auto pop(lua_State* state) {
    return stack_value<sizeof...(T), T...>::pop(state);
}



}
}
