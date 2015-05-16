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
    bool ownership;
    unsigned int* references { new unsigned int { 0 }};
    
public:
    
    BaseState(lua_State* state, bool take_ownership):
        state(state), ownership(take_ownership) {
        if(ownership) ++(*references);
    };
    BaseState(const BaseState& rhs, bool take_ownership = true):
        state(rhs.state), ownership(take_ownership && *rhs.references),
        references(rhs.references) {
        if(ownership) ++(*references);
    }
    BaseState(BaseState&& rhs):
        state(rhs.state), ownership(rhs.ownership), references(rhs.references) {
        rhs.state = nullptr;
        rhs.ownership = false;
        rhs.references = nullptr;
    }
    BaseState& operator=(const BaseState& rhs) {
        state = rhs.state;
        ownership = rhs.ownership;
        references = rhs.references;
        if(ownership) ++(*references);
        return *this;
    }
    BaseState& operator=(BaseState&& rhs) {
        if(ownership) {
            --(*references);
            if(state && !(*references)) {
                lua_close(state);
                delete references;
            }
        }
        state = rhs.state;
        ownership = rhs.ownership;
        references = rhs.references;
        rhs.state = nullptr;
        rhs.ownership = false;
        rhs.references = nullptr;
        return *this;
    }
    ~BaseState() {
        if(references && ownership) {
            --(*references);
            if(state && !(*references)) {
                lua_close(state);
                delete references;
            }
        }
    }
    
    //!
    //! Get count of instances with ownership of state
    //!
    inline const unsigned int GetReferences() {
        return *references;
    }
    
    inline operator lua_State*() {
        return state;
    }
    inline operator const int() {
        return lua_gettop(state);
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
