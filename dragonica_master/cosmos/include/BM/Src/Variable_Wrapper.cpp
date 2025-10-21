#include "stdafx.h"
#include "Variable_Wrapper.h"
#include "Stream_Traits.h"

namespace BM
{
	void PgVariableMgr_Wrapper::Swap( PgVariableMgr< T_VARIANT_KEY, T_VARIANT_KEY > &rhs )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
		Instance()->Swap( rhs );
	}

	void PgVariableMgr_Wrapper::WriteToPacket( BM::Stream &rkPacket )const
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
		Instance()->WriteToPacket( rkPacket );
	}

	bool PgVariableMgr_Wrapper::ReadFromPacket( BM::Stream &rkPacket )
	{
		BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
		return Instance()->ReadFromPacket( rkPacket );
	}

	HRESULT PgVariableMgr_Wrapper::Load( std::wstring const &kPath )
	{
		PgVariable_ParserINI< T_VARIANT_GROUPKEY, T_VARIANT_KEY > kParseINI;
		if ( SUCCEEDED(kParseINI.Load( kPath )) )
		{
			BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
			Instance()->Swap( kParseINI );
			return S_OK;
		}
		return E_FAIL;
	}

};//<-- namespace BM
