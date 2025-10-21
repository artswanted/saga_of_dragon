// LuaHelper.h: interface for the CLuaHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LUAHELPER_H__FE39E254_5793_42CE_B1CA_C38E7437E9AD__INCLUDED_)
#define AFX_LUAHELPER_H__FE39E254_5793_42CE_B1CA_C38E7437E9AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct lua_State;
struct Proto;
struct lua_Debug;

class CLuaHelper  
{
public:
	void RestoreGlobals();
	void CoverGlobals();
	void Describe(char* szRet, int nIndex);
	BOOL Eval(const char* szCode, char* szRet);
	BOOL GetCalltip(const char *szWord, char *szCalltip);
	void DrawGlobalVariables();
	void DrawLocalVariables();
	const char* GetSource();

	static BOOL ErrorStringToFileLine(CString strError, CString &strPathName, int &nLine);
	static BOOL LoadDebugLines(CProjectFile* pPF);

	CLuaHelper();
	virtual ~CLuaHelper();

// debugger functions
	BOOL PrepareDebugger();
	BOOL StartDebugger();	
	void StopDebugger();

	void DrawStackTrace();

	lua_State* GetState() { return L; };
protected:
	void Free();
	static CLuaHelper* m_pThis;

	static int OutputTop(lua_State* L);
	static int errormessage(lua_State* L);
	static int lua_loadlib(lua_State* L);
	static void line_hook (lua_State *L, lua_Debug *ar);
	static void func_hook (lua_State *L, lua_Debug *ar);
	static void hook (lua_State *L, lua_Debug *ar);

	lua_State* L;
	lua_Debug* m_pAr;
	HMODULE m_hLoaded[8192];
	int m_nLoaded;
};

#endif // !defined(AFX_LUAHELPER_H__FE39E254_5793_42CE_B1CA_C38E7437E9AD__INCLUDED_)
