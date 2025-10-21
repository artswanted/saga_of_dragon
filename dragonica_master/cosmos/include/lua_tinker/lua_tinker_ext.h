#pragma once

namespace lua_tinker
{
	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			if(lua_pcall(L, 4, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			if(lua_pcall(L, 5, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}


	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			if(lua_pcall(L, 6, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,  typename T7>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			push(L, arg7);
			if(lua_pcall(L, 7, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,  typename T7, typename T8>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			push(L, arg7);
			push(L, arg8);
			
			if(lua_pcall(L, 8, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,  typename T7, typename T8, typename T9>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			push(L, arg7);
			push(L, arg8);
			push(L, arg9);
			
			if(lua_pcall(L, 9, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,  typename T7, typename T8, typename T9, typename T10>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			push(L, arg7);
			push(L, arg8);
			push(L, arg9);
			push(L, arg10);
			
			if(lua_pcall(L, 10, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6))
	{ 
		lua_pushcclosure(L, functor<T1,T2,T3,T4,T5,T6>::invoke<RVal>, 1);
	}

/*	lua_tinker 원본에서 아래 구문 제거
	// functor
	template<typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void>
	struct functor
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5)); return 0; }
	};
*/
	// functor
	template<typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void, typename T7=void>
	struct functor
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6,T7)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct functor<T1,T2,T3,T4,T5,T6> 
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	struct functor<T1,T2,T3,T4,T5> 
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5)); return 0; }
	};

/*	lua_tinker 원본에서 아래 구문 제거
	// member function
	template<typename T, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void>
	struct mem_functor
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)));; return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)); return 0; }
	};
*/
	template<typename T, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void, typename T7=void>
	struct mem_functor
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8)));; return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
	struct mem_functor<T,T1,T2,T3,T4,T5,T6>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7))); return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5> 
	struct mem_functor<T,T1,T2,T3,T4,T5>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6))); return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)); return 0; }
	};
//*/
	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6,T7>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6,T7>::invoke<RVal>, 1); 
	}
//*/
}