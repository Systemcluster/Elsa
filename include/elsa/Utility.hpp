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


namespace elsa {

static const std::string Version { "Elsa 0.0.1-dev" };

namespace utility {

#if defined (LUAJIT_VERSION)
static const std::string LuaVersion { LUAJIT_VERSION };
#elif defined (LUA_VERSION)
static const std::string LuaVersion { LUA_RELEASE };
#else
static const std::string LuaVersion { "Unknown" };
#endif
static const std::string LuaRelease { LUA_RELEASE };
static const int LuaVersionNum { LUA_VERSION_NUM };



template<typename... T>
struct Type {
};
template<typename T>
struct Type<T> {
    static constexpr const size_t arity { 1 };
};
template<>
struct Type<std::tuple<>> {
    static constexpr const size_t arity { 0 };
};
template<typename T1, typename... Tn>
struct Type<std::tuple<T1, Tn...>> {
    static constexpr const size_t arity { Type<T1>::arity + Type<Tn...>::arity };
};
template<>
struct Type<void> {
    static constexpr const size_t arity { 0 };
};
template<>
struct Type<> {
    static constexpr const size_t arity { 0 };
};
template<typename T1, typename T2, typename... Tn>
struct Type<T1, T2, Tn...> {
    static constexpr const size_t arity { Type<T1>::arity + Type<T2>::arity + Type<Tn...>::arity };
};



inline void Push(lua_State* state) { }
inline void Push(lua_State* state, std::nullptr_t) {
    lua_pushnil(state);
}
inline void Push(lua_State* state, int value) {
    lua_pushinteger(state, value);
}
inline void Push(lua_State* state, unsigned int value) {
#if LUA_VERSION_NUM == 502
    lua_pushunsigned(state, value);
#else
    lua_pushinteger(state, value);
#endif
}
inline void Push(lua_State* state, long value) {
    lua_pushinteger(state, value);
}
inline void Push(lua_State* state, unsigned long value) {
#if LUA_VERSION_NUM == 502
    lua_pushunsigned(state, value);
#else
    lua_pushinteger(state, value);
#endif
}
inline void Push(lua_State* state, float value) {
    lua_pushnumber(state, value);
}
inline void Push(lua_State* state, double value) {
    lua_pushnumber(state, value);
}
inline void Push(lua_State* state, bool value) {
    lua_pushboolean(state, value);
}
inline void Push(lua_State* state, const char* value) {
    lua_pushstring(state, value);
}
inline void Push(lua_State* state, std::string value) {
    lua_pushstring(state, value.c_str());
}
template<typename T1, typename T2, typename... Tn>
void Push(lua_State* state, T1& value1, T2& value2, Tn&... values) {
    Push(state, std::forward<T1>(value1));
    Push(state, std::forward<T2>(value2));
    Push(state, std::forward<Tn>(values)...);
}
namespace detail {
    template <std::size_t... Is>
    struct Indices {
    };
    template <std::size_t N, std::size_t... Is>
    struct IndicesBuilder: IndicesBuilder<N-1, N-1, Is...> {
    };
    template <std::size_t... Is>
    struct IndicesBuilder<0, Is...> {
        using type = Indices<Is...>;
    };
}
template<typename... T>
void Push(lua_State* state, std::tuple<T...> values) {
    Push(state, std::forward<std::tuple<T...>>(values), typename detail::IndicesBuilder<sizeof...(T)>::type());
}
template<typename... T, std::size_t... N>
void Push(lua_State* state, std::tuple<T...> values, detail::Indices<N...>) {
    Push(state, std::get<N>(std::forward<std::tuple<T...>>(values))...);
}



namespace detail {
    template<size_t, typename... T> struct StackValue;
}

template<typename... T>
inline auto Get(lua_State* state, int index) {
    return detail::StackValue<sizeof...(T), T...>::get(state, index);
}
template<typename... T>
inline auto Pop(lua_State* state) {
    return detail::StackValue<sizeof...(T), T...>::pop(state);
}

namespace detail {
    template<typename T> inline T Get(lua_State* state, int index);

    template<size_t, typename... T>
    struct StackValue {
        typedef std::tuple<T...> type;
        static type pop(lua_State* state) {
            int arity = static_cast<int>(Type<T...>::arity);
            int bottom = 0 - arity;
            auto ret = make_tuple<T...>(state, bottom);
            lua_pop(state, arity);
            return ret;
        }
        static type get(lua_State* state, int& index) {
            return make_tuple<T...>(state, index);
        }
    private:
        template<typename T1>
        static auto make_tuple(lua_State* state, int& index) {
            return std::make_tuple(StackValue<1, T1>::get(state, index));
        }
        template<typename T1, typename T2, typename... Tn>
        static auto make_tuple(lua_State* state, int& index) {
            return std::tuple_cat(make_tuple<T1>(state, index), make_tuple<T2, Tn...>(state, index));
        }
    };
    template<typename... T>
    struct StackValue<1, std::tuple<T...>>: StackValue<sizeof...(T), T...> {
    };
    template<typename T>
    struct StackValue<1, T> {
        typedef T type;
        static type pop(lua_State* state) {
            auto ret = Get<T>(state, -1);
            lua_pop(state, 1);
            return ret;
        }
        static type get(lua_State* state, int& index) {
            return detail::Get<T>(state, index++);
        }
    };
    template<typename... T>
    struct StackValue<0, T...> {
        typedef void type;
        static type pop(lua_State* state) {
            return;
        }
        static type get(lua_State* state, int& index) {
            return;
        }
    };
    
    template<> inline int Get(lua_State* state, int index) {
        return static_cast<int>(luaL_checkinteger(state, index));
    }
    template<> inline unsigned int Get(lua_State* state, int index) {
#if LUA_VERSION_NUM == 502
        return static_cast<unsigned int>(luaL_checkunsigned(state, index));
#else
        return static_cast<unsigned int>(luaL_checkinteger(state, index));
#endif
    }
    template<> inline long Get(lua_State* state, int index) {
        return static_cast<long>(luaL_checkinteger(state, index));
    }
    template<> inline unsigned long Get(lua_State* state, int index) {
#if LUA_VERSION_NUM == 502
        return static_cast<unsigned long>(luaL_checkunsigned(state, index));
#else
        return static_cast<unsigned long>(luaL_checkinteger(state, index));
#endif
    }
    template<> inline float Get(lua_State* state, int index) {
        return static_cast<float>(luaL_checknumber(state, index));
    }
    template<> inline double Get(lua_State* state, int index) {
        return static_cast<double>(luaL_checknumber(state, index));
    }
    template<> inline bool Get(lua_State* state, int index) {
        return static_cast<bool>(lua_toboolean(state, index));
    }
    template<> inline std::string Get(lua_State* state, int index) {
        return luaL_checkstring(state, index);
    }
    template<> inline const char* Get(lua_State* state, int index) {
        return luaL_checkstring(state, index);
    }

}

}
}
