//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//
//  Selector.hpp
//  Created 2015-05-15
//

#pragma once


namespace elsa {

class Selector {
    friend class BaseState;
   
    BaseState state;
    std::string name;
    
public:

    Selector(BaseState state, const std::string name):
    state(state), name(name) {
        
    }

};

}
