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
    
    bool ownership;
    std::atomic<unsigned int>* refcount { new std::atomic<unsigned int> { 0 }};
    
    
public:
    
    BaseState(lua_State* state, bool take_ownership):
        state(state), ownership(take_ownership) {
        std::atomic_fetch_add_explicit(refcount, 1u, std::memory_order_relaxed);
    };
    BaseState(const BaseState& rhs):
    state(rhs.state), ownership(rhs.ownership), refcount(rhs.refcount) {
        std::atomic_fetch_add_explicit(refcount, 1u, std::memory_order_relaxed);
    }
    BaseState(BaseState&& rhs):
    state(rhs.state), ownership(rhs.ownership), refcount(rhs.refcount) {
        rhs.state = nullptr;
        rhs.ownership = false;
        rhs.refcount = nullptr;
    }
    BaseState& operator=(BaseState rhs) {
        swap(*this, rhs);
        return *this;
    }
    ~BaseState() {
        if(!state) return;
        if(std::atomic_fetch_sub_explicit(refcount, 1u, std::memory_order_release) == 1u && ownership) {
            std::atomic_thread_fence(std::memory_order_acquire);
            delete refcount;
            lua_close(state);
        }
    }
    
    inline const unsigned int GetReferences() const {
        return std::atomic_load_explicit(refcount, std::memory_order_relaxed);
    }
    
    inline operator lua_State*const() const {
        return state;
    }
    inline operator const int() const {
        return lua_gettop(state);
    }
    
    friend void swap(BaseState& lhs, BaseState& rhs) noexcept {
        std::swap(lhs.refcount, rhs.refcount);
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
