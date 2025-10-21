#include "stdafx.h"
#include "XStream.h"

namespace BM
{
	PgXStream::PgXStream(void)
		:	m_iWritePos(0)
		,	m_iReadPos(0)
	{

	}

	bool PgXStream::SetWritePos( size_type const iPos )
	{
		if ( this->GetMaxSize() >= iPos )
		{
			m_iWritePos = iPos;
			return true;
		}
		return false;
	}

	bool PgXStream::SetReadPos( size_type const iPos )
	{
		if ( this->GetMaxSize() >= iPos )
		{
			m_iReadPos = iPos;
			return true;
		}
		return false;
	}

	//
	PgXStream_FMMView::PgXStream_FMMView(void)
		:	m_pMem(NULL)
		,	m_iPosStart(0)
		,	m_iPosEnd(0)
	{
	}

	PgXStream_FMMView::~PgXStream_FMMView(void)
	{
		PgXStream_FMMView::Release();// virtual 함수 호출 금지
	}

	HRESULT PgXStream_FMMView::Init( BM::GUID const &kID, data_type* pMem, size_type const iPosStart, size_type const iPosEnd )
	{
		if ( iPosStart < iPosEnd )
		{
			Release();
			m_kID = kID;
			m_pMem = pMem;
			m_iPosStart = iPosStart;
			m_iPosEnd = iPosEnd;
			m_iWritePos = m_iReadPos = 0;
			return S_OK;
		}
		return E_FAIL;
	}

	void PgXStream_FMMView::Release(void)
	{
		if ( m_pMem )
		{
			::UnmapViewOfFile( m_pMem );
		}

		m_kID.Clear();
		m_pMem = NULL;
		m_iPosStart = m_iPosEnd = m_iWritePos = m_iReadPos = 0;
	}

	bool PgXStream_FMMView::Push( void const *pData, size_type const iSize )
	{
		if ( GetRemainWriteSize() < iSize )
		{
			return false;
		}

		char *pMem = m_pMem + m_iPosStart + m_iWritePos;
		::memcpy( pMem, pData, iSize );
		m_iWritePos += iSize;
		return true;
	}

	bool PgXStream_FMMView::Pop( void *pData, size_type const iSize )
	{
		if ( GetRemainReadSize() < iSize )
		{
			return false;
		}

		char *pMem = m_pMem + m_iReadPos;
		::memcpy( pData, pMem, iSize );
		m_iReadPos += iSize;
		return true;
	}

};//<- namespace BM