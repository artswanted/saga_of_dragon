#pragma once

#include "BM/Variable_Parser.h"

#ifdef UNICODE
typedef std::wstring T_VARIANT_GROUPKEY;
typedef std::wstring T_VARIANT_KEY;
#else
typedef std::string T_VARIANT_GROUPKEY;
typedef std::string T_VARIANT_KEY;
#endif

namespace BM
{
	class PgVariableMgr_Wrapper
		:	public	TWrapper< PgVariableMgr< T_VARIANT_GROUPKEY, T_VARIANT_KEY > >
	{
	public:
		PgVariableMgr_Wrapper(void){}
		~PgVariableMgr_Wrapper(void){}

		template< typename T >
		HRESULT Set( T_VARIANT_GROUPKEY const &kGroupKey, T_VARIANT_KEY const &kKey, T const &kElement );

		template< typename T >
		HRESULT Get( T_VARIANT_GROUPKEY const &kGroupKey, T_VARIANT_KEY const &kKey, T &rkOut )const;

		void Swap( PgVariableMgr< T_VARIANT_GROUPKEY, T_VARIANT_KEY > &rhs );

		void WriteToPacket( BM::Stream &rkStream )const;
		bool ReadFromPacket( BM::Stream &rkStream );

		HRESULT Load( std::wstring const &kPath );
	};

#include "Variable_Wrapper.inl"

};// <-- namespace BM

#define g_kVariableMgr SINGLETON_STATIC( BM::PgVariableMgr_Wrapper )
