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
    
// TODO: THREAD SAFETY ("const")
class BaseState {
protected:
    
    lua_State* state;
    bool ownership { false };
    unsigned int* references { new unsigned int { 0 }};
    
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
        state(rhs.state), ownership(rhs.ownership), references(rhs.references) {
        rhs.state = nullptr;
        rhs.ownership = false;
        rhs.references = nullptr;
    }
    BaseState& operator=(const BaseState& rhs) {
        state = rhs.state;
        ownership = rhs.ownership;
        references = rhs.references;
        ++(*references);
        return *this;
    }
    BaseState& operator=(BaseState&& rhs) {
        --(*references);
        if(ownership && state && !(*references)) {
            lua_close(state);
            delete references;
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
        if(references) {
            --(*references);
            if(ownership && state && !(*references)) {
                lua_close(state);
                delete references;
            }
        }
    }
    
    inline const unsigned int GetReferences() const {
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
