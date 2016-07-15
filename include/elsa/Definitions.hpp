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

static const std::string Version { "Elsa 0.0.1-dev" };

#if defined (LUAJIT_VERSION)
static const std::string LuaVersion { LUAJIT_VERSION };
#elif defined (LUA_RELEASE)
static const std::string LuaVersion { LUA_RELEASE };
#else
static const std::string LuaVersion { "Unknown" };
#endif

static const std::string LuaRelease { LUA_RELEASE };
static const int LuaVersionNum { LUA_VERSION_NUM };

    
}
