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


namespace elsa {
    
class BaseState {
protected:
    
    lua_State* state;
    
    bool ownership { false };
    unsigned int* references { new unsigned int { 0 }};

    std::shared_ptr<std::mutex> references_mutex { new std::mutex };
    std::shared_ptr<std::mutex> state_mutex { new std::mutex };
    
    
public:
    
    BaseState(lua_State* state, bool take_ownership):
        state(state), ownership(take_ownership) {
        ++(*references);
    };
    BaseState(const BaseState& rhs):
    state(rhs.state), ownership(rhs.ownership), references(rhs.references),
    state_mutex(rhs.state_mutex), references_mutex(rhs.references_mutex) {
        std::lock_guard<std::mutex> lock(*references_mutex);
        ++(*references);
    }
    BaseState(BaseState&& rhs):
    state(rhs.state), ownership(std::move(rhs.ownership)), references(rhs.references),
    state_mutex(std::move(rhs.state_mutex)), references_mutex(std::move(rhs.references_mutex)) {
        rhs.state = nullptr;
        rhs.references = nullptr;
    }
    BaseState& operator=(BaseState rhs) {
        swap(*this, rhs);
        return *this;
    }
    ~BaseState() {
        if(!state) return;
        std::lock_guard<std::mutex> lock(*references_mutex);
        --(*references);
        if(ownership && !(*references)) {
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
        std::swap(lhs.state_mutex, rhs.state_mutex);
        std::swap(lhs.references_mutex, rhs.references_mutex);
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
