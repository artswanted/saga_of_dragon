#pragma once

#include "boost/algorithm/string.hpp"
#include "BM/Variable.h"


namespace BM
{
	template< typename T_GROUPKEY, typename T_KEY >
	class PgVariable_ParserINI
		:	public BM::PgVariableMgr< T_GROUPKEY, T_KEY >
	{
	public:
		PgVariable_ParserINI(void){}
		~PgVariable_ParserINI(void){}

	public:
		HRESULT Load( std::wstring const &kPath );
	};

#include "Variable_Parser.inl"

};//<-- namespace BM
