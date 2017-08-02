//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//
//  Selector.hpp
//  Created 2016-07-28
//

#pragma once



namespace elsa {
    
class result {
    friend class selector;

    base_state state;

    explicit result(base_state state): state(state) {
    }
    result(result&& rhs): state(std::move(rhs.state)) {
    }
    result(const result&) = delete;
    result& operator=(result) = delete;
public:
    
    template<typename T,
        typename = typename std::enable_if<!std::is_reference<T>::value>::type,
        typename = typename std::enable_if<utility::arity<T>::value == 1>::type
    >
    operator T() const {
        return utility::get<T>(state);
    }
    
    template<typename... T,
    typename = typename std::enable_if<utility::none<typename std::is_reference<T>::value>::type...>>
    operator std::tuple<T...>() const {
        return utility::get<T...>(state);
    }
    
    
    
    
    
    
};

}
