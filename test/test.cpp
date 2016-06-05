//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015
//

#include <lua.hpp>
#include <elsa.hpp>

#include <iostream>
#include <utility>
#include <vector>


bool test_(elsa::State& state) {
    return false;
}

bool test_state_copy(elsa::State& state) {
    {
        elsa::State state2 { [&](){
            elsa::State state3 { state };
            return state3;
        }() };
        lua_State*s1 { state }, *s2 { state2 };
        if(s1 != s2) return false;
        if(state.GetReferences() != state2.GetReferences()
        || state.GetReferences() != 2) return false;
    }
    return state.GetReferences() == 1;
}

bool test_state_assignment(elsa::State& state) {
    {
        elsa::State state2;
        state2 = [&](){
            elsa::State state3;
            state3 = state;
            return state3;
        }();
        elsa::State state3;
        
        lua_State*s1 { state }, *s2 { state2 };
        if(s1 != s2) return false;
        if(state.GetReferences() != state2.GetReferences()
        || state.GetReferences() != 2) return false;
    }
    return state.GetReferences() == 1;
}

bool test_state_move_reassignment(elsa::State& state) {
    elsa::State state2 { state };
    elsa::State state3 { std::move(state2) };
    auto m_references = state.GetReferences();
    state2 = state;
    return state.GetReferences() == 3 && m_references == 2;
}

bool test_state_self_assignment(elsa::State& state) {
    state = state;
    return state.GetReferences() == 1;
}

bool test_state_compare(elsa::State& state) {
    lua_State* s { state };
    elsa::State state2 { s, false };
    elsa::State state3;
    return state == state2 && state != state3 && s == state2;
}

bool test_state_copy_weak(elsa::State& state) {
    elsa::State state2 { state, false };
    return state.GetReferences() == 1;
}

bool test_state_run_code(elsa::State& state) {
    state("x = 5");
    return true;
}

bool test_state_call_return_0(elsa::State& state) {
    state.Call<>("x = 5");
    return true;
}

bool test_state_call_return_1(elsa::State& state) {
    int var = state.Call<int>("return 5");
    return var == 5;
}

bool test_state_call_return_n(elsa::State& state) {
    auto var = state.Call<int, bool, std::string>("return 5, true, 'test'");
    return std::get<int>(var) == 5
        && std::get<bool>(var) == true
        && std::get<std::string>(var) == "test";
}

bool test_state_call_return_tuple(elsa::State& state) {
    auto var = state.Call<std::tuple<int, int>>("return 5, 10");
    return std::get<0>(var) == 5
        && std::get<1>(var) == 10;
}


bool test_selector(elsa::State& state) {
    state("a = { b = 5; c = function() end; d = { e = 5 } }");
    state.Select("a.b.e", '.');
    
    return false;
}

bool test_state_select(elsa::State& state) {
    return false;
}


static const std::vector<const std::pair<
const std::string, const std::function<bool(elsa::State&)>>> tests {
    { "test_state_copy", test_state_copy },
    { "test_state_assignment", test_state_assignment },
    { "test_state_self_assignment", test_state_self_assignment },
    { "test_state_move_reassignment", test_state_move_reassignment },
    { "test_state_compare", test_state_compare },
    { "test_state_copy_weak", test_state_copy_weak },
    
    { "test_state_run_code", test_state_run_code },
    { "test_state_call_return_0", test_state_call_return_0 },
    { "test_state_call_return_1", test_state_call_return_1 },
    { "test_state_call_return_n", test_state_call_return_n },
    { "test_state_call_return_tuple", test_state_call_return_tuple },
    
    { "test_selector", test_selector },
    
    { "test_state_select", test_state_select },
    
};

int main(int argc, const char * argv[]) {
   
    std::cout << "Elsa Test Runner (" << elsa::Version << ")" << std::endl;
    std::cout << "Lua version: " << elsa::utility::LuaVersion << " (" << elsa::utility::LuaRelease << ")" << std::endl;
    std::cout << std::endl;

    const unsigned long test_num { tests.size() };
    std::cout << "Running " << test_num << " tests:" << std::endl;

    std::vector<const std::string> failures;
    std::vector<std::pair<const std::string, const int>> impacts;
    for(const auto& test: tests) {
        std::cout << test.first << "... ";
        elsa::State state { true };
        try {
            const bool test_result = test.second(state);
            if(test_result) {
                std::cout << "succeeded";
            }
            else {
                std::cout << "failed";
                failures.emplace_back(test.first);
            }
            const int stack = (int)state;
            if(stack) {
                std::cout << " with effect on the stack: " << stack;
                impacts.emplace_back(test.first, stack);
            }
            else std::cout << "";
        }
        catch(std::exception& e) {
            std::cout << "failed with an exception being thrown: " << e.what();
            failures.emplace_back(test.first);
        }
        catch(...) {
            std::cout << "failed with an error! ";
            failures.emplace_back(test.first);
#if not IGNORE_ERRORS
            throw;
#endif
        }

        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << test_num - failures.size() << " out of " << test_num
        << " tests succeeded" << (failures.size()?".":"!") << std::endl;
    
    if(impacts.size()) {
        std::cout << std::endl;
        std::cout << impacts.size() << " test" << (impacts.size()>1?"s":"")
            << " had an undesired effect on the stack:" << std::endl;
        for(auto& test: impacts) {
            std::cout << test.first << ": " << test.second << std::endl;
        }
    }
    
    if(failures.size()) {
        std::cout << std::endl;
        std::cout << failures.size() << " test" << (failures.size()>1?"s":"")
            << " failed:" << std::endl;
        for(auto& test: failures) {
           std::cout << test << std::endl;
        }
    }
      
    return 0;
}
