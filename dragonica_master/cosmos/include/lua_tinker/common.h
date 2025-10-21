// lua_tinker.h
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
// 
// please check Licence.txt file for licence and legal issues. 
// 0.5b

#pragma once 

#include <new>
#include <iostream>

extern "C"
{
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}

#include "lua_tinker/lua_tinker_ext.h"
#include "lua_tinker/lua_wrapper.h"


#ifdef _MT_
	#pragma comment(lib, "lua_tinker_MT.lib")
#endif

#ifdef _MTd_
	#pragma comment(lib, "lua_tinker_MTd.lib")
#endif

#ifdef _MTo_
	#pragma comment(lib, "lua_tinker_MTo.lib")
#endif

#ifdef _MD_
	#pragma comment(lib, "lua_tinker_MD.lib")
#endif

#ifdef _MDd_
	#pragma comment(lib, "lua_tinker_MDd.lib")
#endif

#ifdef _MDo_
	#pragma comment(lib, "lua_tinker_MDo.lib")
#endif