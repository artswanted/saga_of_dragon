
inline __int64 MakeInt64(DWORD const dwLow, DWORD const dwHigh)
{
	return (static_cast<__int64>(dwHigh) << 32) + dwLow;
}

inline void SplitInt64(__int64 const i64Value, DWORD& dwLow, DWORD& dwHigh)
{
	dwLow = static_cast<DWORD>(0x00000000FFFFFFFF & i64Value);
	dwHigh = static_cast<DWORD>(i64Value >> 32);
}

