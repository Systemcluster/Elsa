//
//  Elsa Lua Interface
//
//
//  Copyright (c) Christian Sdunek, 2016
//
//  Definitions.hpp
//  Created 2016-07-15
//

#pragma once



namespace elsa {

static const std::string version { "Elsa 0.0.1-dev" };

#if defined (LUAJIT_VERSION)
static const std::string lua_version { LUAJIT_VERSION };
#elif defined (LUA_RELEASE)
static const std::string lua_version { LUA_RELEASE };
#else
static const std::string lua_version { "Unknown" };
#endif

static const std::string lua_release { LUA_RELEASE };
static const int lua_version_num { LUA_VERSION_NUM };

}
