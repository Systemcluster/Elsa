//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//
//  Tuple.hpp
//  Created 2017-08-07
//

#pragma once


namespace elsa {
    template<typename... T>
    class tuple {
        std::tuple<T&...> values;
        tuple(const tuple&) = delete;
        tuple& operator=(tuple) = delete;
    public:
        tuple(tuple&&) = default;
        tuple(T&... values): values(values...) {}
        template<typename... Arg>
        void operator=(selector::result<Arg...>&& result) {
            std::tuple<T...> t = std::move(result);
            values = std::move(t);
        }
    };
    
    template<typename... T>
    inline tuple<T...> tie(T&... args) {
        return tuple<T...>(args...);
    }
}
