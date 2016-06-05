//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//
//  BaseState.hpp
//  Created 2015-05-16
//

#pragma once

#include<iostream>
namespace elsa {
    
class BaseState {
protected:
    
    lua_State* state;
    
    bool ownership { false };
    std::atomic<unsigned int>* references { new std::atomic<unsigned int> { 0 }};
    
    
public:
    
    BaseState(lua_State* state, bool take_ownership):
        state(state), ownership(take_ownership) {
        ++(*references);
    };
    BaseState(const BaseState& rhs):
    state(rhs.state), ownership(rhs.ownership), references(rhs.references) {
        ++(*references);
    }
    BaseState(BaseState&& rhs):
    state(rhs.state), ownership(std::move(rhs.ownership)), references(rhs.references) {
        rhs.state = nullptr;
        rhs.references = nullptr;
    }
    BaseState& operator=(BaseState rhs) {
        swap(*this, rhs);
        return *this;
    }
    ~BaseState() {
        if(!state) return;
        if(!--(*references) && ownership) {
            lua_close(state);
            delete references;
        }
    }
    
    inline const unsigned int GetReferences() const {
        return *references;
    }
    
    inline operator lua_State*() const {
        return state;
    }
    inline operator const int() const {
        return lua_gettop(state);
    }
    
    friend void swap(BaseState& lhs, BaseState& rhs) noexcept {
        std::swap(lhs.references, rhs.references);
        std::swap(lhs.ownership, rhs.ownership);
        std::swap(lhs.state, rhs.state);
    }
    
    inline friend bool operator==(const BaseState& lhs, const BaseState& rhs) {
        return lhs.state == rhs.state;
    }
    inline friend bool operator!=(const BaseState& lhs, const BaseState& rhs) {
        return lhs.state != rhs.state;
    }
    inline friend bool operator==(const BaseState& lhs, lua_State* rhs) {
        return lhs.state == rhs;
    }
    inline friend bool operator==(lua_State* lhs, const BaseState& rhs) {
        return lhs == rhs.state;
    }
    inline friend bool operator!=(const BaseState& lhs, lua_State* rhs) {
        return lhs.state == rhs;
    }
    inline friend bool operator!=(lua_State* lhs, const BaseState& rhs) {
        return lhs != rhs.state;
    }
};
    
}
