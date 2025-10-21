#include "stdafx.h"
#include "vstring.h"
#include "Point.h"

// char, unsigned char만 특수화 (그냥 출력하면, 문자로 출력된다)
template<>
BM::vstring& operator <<(BM::vstring& lhs, T_PT3< char > const& rhs)
{
	lhs += BM::vstring(L" POINT3[X:")<<static_cast<int>(rhs.x)<<L",Y:"<<static_cast<int>(rhs.y)<<L",Z:"<<static_cast<int>(rhs.z)<<L"]";
	return lhs;
}
template<>
BM::vstring& operator <<(BM::vstring& lhs, T_PT3< unsigned char > const& rhs)
{
	lhs += BM::vstring(L" POINT3[X:")<<static_cast<int>(rhs.x)<<L",Y:"<<static_cast<int>(rhs.y)<<L",Z:"<<static_cast<int>(rhs.z)<<L"]";
	return lhs;
}