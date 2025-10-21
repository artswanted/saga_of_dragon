#pragma once

namespace lwBase
{
	void RegisterWrapper(lua_State *pkState);
	void lwDoFile(char const *pcFileName);
}