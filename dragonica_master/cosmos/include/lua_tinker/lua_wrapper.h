#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "BM/ThreadObject.h"
#include "lua_tinker.h"

class lua_wrapper_user;

class lua_wrapper
{
	friend class lua_wrapper_user;
public:
	lua_wrapper();
	~lua_wrapper();

public:
	HRESULT open();
	void close();

protected:
	Loki::Mutex m_kMutex;//Lock
	lua_State *m_pkState;
};

class lua_wrapper_user
{
public:
	lua_wrapper_user(lua_wrapper& rkWrapper)
	{
		m_pkWrapper = &rkWrapper;
		m_pkWrapper->m_kMutex.Lock();
	}

	~lua_wrapper_user()
	{
		m_pkWrapper->m_kMutex.Unlock();
	}

public:
	lua_State* operator *(){return m_pkWrapper->m_pkState;}

protected:
	lua_wrapper* m_pkWrapper;
};

#define g_kLuaTinker SINGLETON_STATIC(lua_wrapper)

namespace lua_tinker
{
	int _exception(lua_State *L);

	struct lua_obj
	{
		lua_obj& operator,(const lua_obj& /* obj */) { return *this; }
	};

	template<typename T>
	struct class_: lua_obj
	{
		// initialize
		class_(lua_State* L, const char* name) 
		{ 
			lua_tinker::class_add<T>(L, name);
		}

		// Tinker Class Inheritence
		template<typename P>
		class_<T>& inh(lua_State* L)
		{
			lua_tinker::class_inh<T, P>(L);
			return *this;
		}

		// constructor
		template<typename CONSTRUCTOR>
		class_<T>& def(lua_State* L, CONSTRUCTOR)
		{
			lua_tinker::class_con<T, CONSTRUCTOR>(L, CONSTRUCTOR());
			return *this;
		}

		// Tinker Class Functions
		template<typename F>
		class_<T>& def(lua_State* L, const char* name, F func) 
		{ 
			lua_tinker::class_def<T>(L, name, func);
			return *this; 
		}

		static const char* _name(const char* szName = NULL)
		{
			static char std::string kStaticName= "";
			if(szName)
			{
				kStaticName = szName;
			}
			return szName.c_str();
		}
	};

	template<typename RVal>
	RVal call(const char* name)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal>(*kLua, name);
	}

	template<typename RVal, typename T1>
	RVal call(const char* name, T1 arg)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1>(*kLua, name, arg);
	}

	template<typename RVal, typename T1, typename T2>
	RVal call(const char* name, T1 arg1, T2 arg2)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2>(*kLua, name, arg1, arg2);
	}

	template<typename RVal, typename T1, typename T2, typename T3>
	RVal call(const char* name, T1 arg1, T2 arg2, T3 arg3)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2, T3>(*kLua, name, arg1, arg2, arg3);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	RVal call(const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2, T3, T4>(*kLua, name, arg1, arg2, arg3, arg4);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	RVal call(const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2, T3, T4, T5>(*kLua, name, arg1, arg2, arg3, arg4, arg5);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	RVal call(const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2, T3, T4, T5, T6>(*kLua, name, arg1, arg2, arg3, arg4, arg5, arg6);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	RVal call(const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2, T3, T4, T5, T6, T7>(*kLua, name, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	RVal call(const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2, T3, T4, T5, T6, T7, T8>(*kLua, name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	RVal call(const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2, T3, T4, T5, T6, T7, T8, T9>(*kLua, name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	RVal call(const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10)
	{
		lua_wrapper_user kLua(g_kLuaTinker);
		return lua_tinker::call<RVal, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(*kLua, name, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	}
}