# Elsa

C++17 header-only interface to Lua 5.1+ and LuaJIT 2.0+. 

Work in progress. Not yet ready for general use!

## Usage

Install the files within `include` and include `elsa.hpp`. `lua.hpp` must exist in your include path and your project must link against a compatible lua library (Lua 5.1+, LuaJIT 2.0+ or similar). 

---

To build the tests, run `cmake` or manually compile `test/test.cpp`. In order to build the tests with `cmake`, execute the following command:

mkdir -p build && cd build && cmake .. && make

The tests can then be run by executing `./elsa_test`. 

### Accessing Lua states

    elsa::state state; // creates a new Lua state
    elsa::state state { true }; // creates a new Lua state and opens the standard libraries
    
`elsa::state` is reference counting and supports thread-safe copying and moving.

To access an existing Lua state, pass its handle as the first argument. `elsa::state` will take ownership by default and close the state when all its instances get destroyed. Pass `false` as the second argument if you intend to manage the lifetime of the Lua state yourself.
    
    lua_State* l = luaL_newstate();
    elsa::state state { l, false }; // accesses an existing Lua state without taking ownership

### Running Lua code

    elsa::state state;
    state("x = 5; b = false");
    int x, bool b;
    std::tie(x, b) = state.call<int, bool>("return x, b");
    


