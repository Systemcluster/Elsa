//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2015-2017
//

// TODO: reorder
#include <iostream>

#include <lua.hpp>
#include <elsa.hpp>

#include <utility>
#include <vector>


bool test_(elsa::state& state) {
    return false;
}

bool test_state_copy(elsa::state& state) {
    {
        elsa::state state2 { [&](){
            elsa::state state3 { state };
            return state3;
        }() };
        lua_State *s1 { state }, *s2 { state2 };
        if(s1 != s2) return false;
        if(state.references() != state2.references() ||
           state.references() != 2) return false;
    }
    return state.references() == 1;
}

bool test_state_assignment(elsa::state& state) {
    {
        elsa::state state2;
        state2 = [&](){
            elsa::state state3;
            state3 = state;
            return state3;
        }();
        elsa::state state3;
        
        lua_State *s1 { state }, *s2 { state2 };
        if(s1 != s2) return false;
        if(state.references() != state2.references() ||
           state.references() != 2)
           return false;
    }
    return state.references() == 1;
}

bool test_state_move_reassignment(elsa::state& state) {
    elsa::state state2 { state };
    elsa::state state3 { std::move(state2) };
    auto m_references = state.references();
    state2 = state;
    return state.references() == 3 && m_references == 2;
}

bool test_state_self_assignment(elsa::state& state) {
    state = state;
    return state.references() == 1;
}

bool test_state_copy_weak(elsa::state& state) {
    elsa::state state2 { state, false };
    return state.references() == 1;
}

bool test_state_compare(elsa::state& state) {
    lua_State* s { state };
    elsa::state state2 { s, false };
    elsa::state state3;
    return state == state2 && state != state3 && s == state2;
}

bool test_state_run_code(elsa::state& state) {
    state("x = 5");
    return true;
}

bool test_state_call_return_0(elsa::state& state) {
    state.call("x = 5");
    return true;
}

bool test_state_call_return_1(elsa::state& state) {
    int a = state.call<int>("return 5");
    return a == 5;
}

bool test_state_call_return_n(elsa::state& state) {
    int i; bool b; std::string s;
    std::tie(i, b, s) = state.call<int, bool, std::string>("return 5, true, 'test'");
    return i == 5 && b == true && s == "test";
}

bool test_state_call_return_tuple(elsa::state& state) {
    int a, b;
    std::tie(a, b) = state.call<std::tuple<int, int>>("return 5, 10");
    return a == 5 && b == 10;
}

bool test_state_call_return_tuple_nested(elsa::state& state) {
    int a, b, c;
    auto t = std::tie(a, b);
    std::tie(t, c) = state.call<std::tuple<std::tuple<int, int>, int>>("return 5, 10, 20");
    return a == 5 && b == 10 && c == 20;
}


bool test_selector_get(elsa::state& state) {
    state("a = 10; b = { c = 8; };");
    auto a = state["a"];
    auto c = state["b"]["c"];
    
    return a == 10 && c == 8;
}

bool test_selector_call_return_0(elsa::state& state) {
    state("a = function() end;");
    state("b = { c = function() end; };");
    state["a"]();
    state["b"]["c"]();
    return true;
}

bool test_selector_call_return_1(elsa::state& state) {
    state("a = function() return 5; end;");
    int a = state["a"].call<int>();
    return a == 5;
}

bool test_selector_call_return_n(elsa::state& state) {
    state("b = { c = function() return 10, 'test'; end; }");
    int a; std::string b;
    std::tie(a, b) = state["b"]["c"].call<int, std::string>();
    return a == 10 && b == "test";
}

bool test_selector_call_return_n_partial(elsa::state& state) {
    state("b = { c = function() return 10, 20, 30; end; }");
    int a, b;
    std::tie(a, b) = state["b"]["c"].call<int, int>();
    return a == 10 && b == 20;
}

bool test_state_select_delim(elsa::state& state) {
    state("a = { b = 5; c = function() end; d = { e = 5 } }");
    auto sel = state.select("a.d.e", '.');

    return sel == 5;
}

bool test_state_select_n(elsa::state& state) {
    state("a = { b = 5; c = function() end; d = { e = 5 } }");
    auto sel = state.select("a", "d", "e");
    
    return sel == 5;
}

bool test_utility_arity(elsa::state& state) {
    return
    elsa::utility::arity<>::value == 0 &&
    elsa::utility::arity<int, int, int>::value == 3 &&
    elsa::utility::arity<std::tuple<int, int, int>>::value == 3 &&
    elsa::utility::arity<std::tuple<int, int>, int>::value == 3 &&
    elsa::utility::arity<std::tuple<std::tuple<int, int>, int>>::value == 3 &&
    elsa::utility::arity<std::tuple<void, int>, void>::value == 1;
}

bool test_call_result(elsa::state& state) {
    state("a = function() return 10, 20; end");
    std::tuple<int, int> c  = state["a"]();
    return std::get<0>(c) == 10 && std::get<1>(c) == 20;
}

bool test_call_result_tie(elsa::state& state) {
    state("a = function() return 10, 20, 30; end");
    int a = 0, b = 0, c = 0;
    elsa::tie(a, b, c) = state["a"]();
    return a == 10 && b == 20 && c == 30;
}

bool test_call_result_args(elsa::state& state) {
    state("a = function(b) return b+2; end");
    int a = state["a"](10);
    return a == 12;
}

bool test_call_args(elsa::state& state) {
    state("a = function(b) return b+2; end");
    state["a"](10);
    return true;
}

bool test_call_nested_tuple(elsa::state& state) {
    state("a = function() return 1, 2, 3; end");
    std::tuple<int, std::tuple<int, int>> a = state["a"]();
    auto b = std::get<1>(a);
    return std::get<0>(a) == 1 && std::get<0>(b) == 2 && std::get<1>(b) == 3;
}

bool test_call_multiple_times(elsa::state& state) {
    state("a = 0; b = function() a=a+1; return a; end");
    auto b = state["b"];
    b();
    int t = b();
    b();
    return t == 2 && state["a"] == 3;
}



static const std::vector<std::pair<
const std::string, const std::function<bool(elsa::state&)>>> tests {
    { "test_state_copy", test_state_copy },
    { "test_state_assignment", test_state_assignment },
    { "test_state_self_assignment", test_state_self_assignment },
    { "test_state_move_reassignment", test_state_move_reassignment },
    { "test_state_copy_weak", test_state_copy_weak },
    { "test_state_compare", test_state_compare },
    
    { "test_state_run_code", test_state_run_code },
    { "test_state_call_return_0", test_state_call_return_0 },
    { "test_state_call_return_1", test_state_call_return_1 },
    { "test_state_call_return_n", test_state_call_return_n },
    { "test_state_call_return_tuple", test_state_call_return_tuple },
    { "test_state_call_return_tuple_nested", test_state_call_return_tuple_nested },
    
    { "test_selector_get", test_selector_get },
    { "test_selector_call_return_0", test_selector_call_return_0 },
    { "test_selector_call_return_1", test_selector_call_return_1 },
    { "test_selector_call_return_n", test_selector_call_return_n },
    { "test_selector_call_return_n_partial", test_selector_call_return_n_partial },
    
    { "test_state_select_delim", test_state_select_delim },
    { "test_state_select_n", test_state_select_n },
    
    { "test_utility_arity", test_utility_arity },
    
    { "test_call_result", test_call_result },
    { "test_call_result_args", test_call_result_args },
    { "test_call_result_tie", test_call_result_tie },
    { "test_call_args", test_call_args },
    
    { "test_call_nested_tuple", test_call_nested_tuple },
    { "test_call_multiple_times", test_call_multiple_times }
};

#if defined(COLOURED_OUTPUT)
#define S "\x1B[92m"
#define E "\x1B[31m"
#define R "\x1B[0m"
#else
#define S ""
#define E ""
#define R ""
#endif

int main(int argc, const char * argv[]) {
   
    std::cout << "Elsa Test Runner (" << elsa::version << ")" << std::endl;
    std::cout << "Lua version: " << elsa::lua_version << " (" << elsa::lua_release << ")" << std::endl;
    std::cout << std::endl;

    const unsigned long test_num { tests.size() };
    std::cout << "Running " << test_num << " tests:" << std::endl;

    std::vector<std::string> failures;
    std::vector<std::pair<const std::string, const int>> impacts;
    for(const auto& test: tests) {
        std::cout << test.first << "... ";
        elsa::state state { true };
        try {
            const bool test_result = test.second(state);
            if(test_result) {
                std::cout << S << "succeeded " << std::flush;
            }
            else {
                std::cout << E << "failed " << std::flush;
                failures.emplace_back(test.first);
            }
            const int stack = (int)state;
            if(stack) {
                std::cout << E << "with side effect on the stack: " << stack;
                impacts.emplace_back(test.first, stack);
            }
            else std::cout << "";
        }
        catch(const std::exception& e) {
            std::cout << E"failed with an exception being thrown: " << e.what();
            failures.emplace_back(test.first);
        }
        catch(...) {
            if(lua_gettop(state)) {
                if(auto e = lua_tostring(state, -1)) {
                    std::cout << E << "failed: " << e << " ";
                }
            }
            else {
                std::cout << E << "failed with an error";
            }
            failures.emplace_back(test.first);
        }

        std::cout << R << std::endl;
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
    
    std::cout << std::endl;
      
    return 0;
}
