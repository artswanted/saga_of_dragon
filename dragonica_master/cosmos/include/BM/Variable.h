#pragma once

#include "BM/vstring.h"
#include "BM/TWrapper.h"
#include "BM/Stream.h"

namespace BM
{
	template< typename T_KEY >
	class PgVariables
	{
	public:
		typedef BM::vstring								value_type;
		typedef std::map< typename T_KEY, value_type >	CONT_VARIABLE;

	public:
		PgVariables(void){}
		~PgVariables(void){}

		template< typename T >
		HRESULT Set( T_KEY const &kKey, T const &kElement );

		template< >
		HRESULT Set( T_KEY const &kKey, value_type const &kElement )
		{
			auto kPair = m_kContVariable.insert( std::make_pair( kKey, kElement ) );
			if ( !kPair.second )
			{
				kPair.first->second = kElement;
				return S_FALSE;
			}
			return S_OK;
		}

		template< typename T >
		HRESULT Get( T_KEY const &kKey, T &rkOut )const;

		template< >
		HRESULT Get( T_KEY const &kKey, value_type &rkOut )const
		{
			CONT_VARIABLE::const_iterator itr = m_kContVariable.find( kKey );
			if ( itr != m_kContVariable.end() )
			{
				rkOut = itr->second;
				return S_OK;
			}
			return E_FAIL;
		}

		void Swap( PgVariables< T_KEY > &rhs );

		void Add( PgVariables< T_KEY > const &rhs );

		void WriteToPacket( Stream &kStream )const;
		bool ReadFromPacket( Stream &kStream );
		size_t min_size(void)const{return sizeof(size_t);}

	protected:
		CONT_VARIABLE		m_kContVariable;
	};

	template< typename T_GROUPKEY, typename T_KEY >
	class PgVariableMgr
	{
	public:
		typedef std::map< typename T_GROUPKEY, PgVariables< T_KEY > >		CONT_VARIABLES;

	public:
		PgVariableMgr(void){}
		~PgVariableMgr(void){}

		PgVariableMgr( PgVariableMgr const &rhs );
		PgVariableMgr& operator = ( PgVariableMgr const &rhs );

		template< typename T >
		HRESULT Set( T_GROUPKEY const &kGroupKey, T_KEY const &kKey, T const &kElement );

		template< typename T >
		HRESULT Get( T_GROUPKEY const &kGroupKey, T_KEY const &kKey, T &rkOut )const;

		void Swap( PgVariableMgr< T_GROUPKEY, T_KEY > &rhs );

		void Add( PgVariableMgr< T_GROUPKEY, T_KEY > const &rhs );
		HRESULT Add( PgVariableMgr< T_GROUPKEY, T_KEY > const &rhs, T_GROUPKEY const &kGroupKey );

		void WriteToPacket( Stream &kStream )const;
		bool ReadFromPacket( Stream &kStream );
		size_t min_size(void)const{return sizeof(size_t);}

	protected:
		CONT_VARIABLES		m_kContVariables;
	};


#include "Variable.inl"

};//<- namespace BM

