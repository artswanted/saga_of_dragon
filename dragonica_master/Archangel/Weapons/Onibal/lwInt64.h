#pragma once

class lwInt64
{
public:
	lwInt64(void);
	lwInt64(__int64 iValue);

	static bool RegisterWrapper(lua_State *pkState);
public:
	__int64 GetValue() { return m_iValue; }
	void Set(int iLow, int iHigh);

	bool IsBig( lwInt64 i64Value );

private:
	__int64 m_iValue;
};