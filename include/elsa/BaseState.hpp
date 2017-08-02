//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//
//  base_state.hpp
//  Created 2015-05-16
//

#pragma once

#include<iostream>



namespace elsa {

class base_state {
protected:
    
    lua_State* lstate;
    
    bool ownership;
    std::atomic<unsigned int>* refcount { new std::atomic<unsigned int> { 0 }};
    
public:
    
    base_state(lua_State* lstate, bool take_ownership):
        lstate(lstate), ownership(take_ownership) {
        std::atomic_fetch_add_explicit(refcount, 1u, std::memory_order_relaxed);
    };
    base_state(const base_state& rhs):
    lstate(rhs.lstate), ownership(rhs.ownership), refcount(rhs.refcount) {
        std::atomic_fetch_add_explicit(refcount, 1u, std::memory_order_relaxed);
    }
    base_state(base_state&& rhs):
    lstate(rhs.lstate), ownership(rhs.ownership), refcount(rhs.refcount) {
        rhs.lstate = nullptr;
        rhs.ownership = false;
        rhs.refcount = nullptr;
    }
    // copy&swap assignment
    base_state& operator=(base_state rhs) {
        swap(*this, rhs);
        return *this;
    }
    
    ~base_state() {
        if(!lstate) return;
        if(std::atomic_fetch_sub_explicit(refcount, 1u, std::memory_order_release) == 1u && ownership) {
            std::atomic_thread_fence(std::memory_order_acquire);
            delete refcount;
            lua_close(lstate);
        }
    }
    
    inline const unsigned int references() const {
        if(refcount == nullptr) return 0;
        return std::atomic_load_explicit(refcount, std::memory_order_relaxed);
    }
    
    inline operator lua_State*const() const {
        return lstate;
    }
    inline operator const int() const {
        if(lstate == nullptr) return 0;
        return lua_gettop(lstate);
    }
    
    friend void swap(base_state& lhs, base_state& rhs) noexcept {
        std::swap(lhs.refcount, rhs.refcount);
        std::swap(lhs.ownership, rhs.ownership);
        std::swap(lhs.lstate, rhs.lstate);
    }
    
    inline friend bool operator==(const base_state& lhs, const base_state& rhs) {
        return lhs.lstate == rhs.lstate;
    }
    inline friend bool operator!=(const base_state& lhs, const base_state& rhs) {
        return lhs.lstate != rhs.lstate;
    }
    inline friend bool operator==(const base_state& lhs, lua_State* rhs) {
        return lhs.lstate == rhs;
    }
    inline friend bool operator==(lua_State* lhs, const base_state& rhs) {
        return lhs == rhs.lstate;
    }
    inline friend bool operator!=(const base_state& lhs, lua_State* rhs) {
        return lhs.lstate == rhs;
    }
    inline friend bool operator!=(lua_State* lhs, const base_state& rhs) {
        return lhs != rhs.lstate;
    }
};
    
}
