#include "stdafx.h"
#include "XFStream.h"

namespace BM
{
	DWORD PgXFStream_FMM::ms_dwAllocationGranularity = 0;

	PgXFStream_FMM::PgXFStream_FMM( DWORD const dwGrowSize )
		:	m_kID(BM::GUID::Create())
		,	m_hFileHandle(INVALID_HANDLE_VALUE)
		,	m_hFileMapHandle(INVALID_HANDLE_VALUE)
		,	m_iSize(0)
		,	m_dwGrowSize(dwGrowSize)
	{
		SYSTEM_INFO kSystemInfo;
		::GetSystemInfo( &kSystemInfo );
		if ( kSystemInfo.dwAllocationGranularity )
		{
			ms_dwAllocationGranularity = kSystemInfo.dwAllocationGranularity;
		}
	}

	PgXFStream_FMM::~PgXFStream_FMM(void)
	{
		PgXFStream_FMM::Close();// virtual 함수 호출하지 않도록
	}

	HRESULT PgXFStream_FMM::Open( std::wstring const &wstrFileName, size_type iSize )
	{
		if ( 0 == iSize )
		{
			iSize = static_cast<size_type>(m_dwGrowSize);
		}

		if ( 0 == iSize )
		{
			return E_FAIL;
		}

		m_wstrFileName = wstrFileName;
		HRESULT const hRet = Reserve( static_cast<DWORD>(iSize) );
		if ( NO_ERROR == hRet )
		{
			m_iWritePos = 0;
		}

		return hRet;
	}

	HRESULT PgXFStream_FMM::Reserve( DWORD const dwMaximumSizeLow )
	{
		try
		{
			HANDLE hFile = m_hFileHandle;
			if ( INVALID_HANDLE_VALUE == hFile )
			{
				hFile = ::CreateFileW(	m_wstrFileName.c_str()
									,	GENERIC_READ|GENERIC_WRITE
									,	FILE_SHARE_READ
									,	NULL
									,	CREATE_ALWAYS
									,	FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS
									,	NULL
									);
			}
			
			if ( INVALID_HANDLE_VALUE == hFile || !hFile )
			{
				throw GetLastError();
			}

			if ( INVALID_HANDLE_VALUE != m_hFileMapHandle )
			{
				::CloseHandle(m_hFileMapHandle);
				m_hFileMapHandle = INVALID_HANDLE_VALUE;
			}

			HANDLE hWriteMapFileMap = ::CreateFileMappingW( hFile
														,	NULL
														,	PAGE_READWRITE
														,	0
														,	dwMaximumSizeLow
														,	m_kID.str().c_str()
														);

			if ( INVALID_HANDLE_VALUE == hWriteMapFileMap || !hWriteMapFileMap )
			{
				throw GetLastError();
			}

			m_hFileHandle = hFile;
			m_hFileMapHandle = hWriteMapFileMap;
			m_iSize = static_cast<size_t>(dwMaximumSizeLow);
		}
		catch( DWORD dwError )
		{
			return static_cast<HRESULT>(dwError);
		}
		return NO_ERROR;
	}

	void PgXFStream_FMM::Close( void )
	{
		if ( INVALID_HANDLE_VALUE != m_hFileMapHandle )
		{
			::CloseHandle( m_hFileMapHandle );
			m_hFileMapHandle = INVALID_HANDLE_VALUE;
		}

		if ( INVALID_HANDLE_VALUE != m_hFileHandle )
		{
			::CloseHandle( m_hFileHandle );
			m_hFileHandle = INVALID_HANDLE_VALUE;
		}
	}

	bool PgXFStream_FMM::GetView( PgXStream_FMMView &rkOutView, size_type const iSeek, size_type const iSize )
	{
		if ( true == IsOpen() )
		{
			try
			{
				if ( iSeek < m_iSize )
				{
					size_type const iRemainSize = m_iSize - iSeek;
					if ( iSize > iRemainSize )
					{
						throw static_cast<DWORD>(iSize);
					}

					DWORD dwOffSetLow = ULONG_MAX;
					DWORD dwOffSetHigh = 0;
					size_type iAdjust = 0;
	//				if ( i64Seek < dwOffSetLow )
					{
						dwOffSetLow = static_cast<DWORD>(iSeek);
						iAdjust = static_cast<size_type>(dwOffSetLow % ms_dwAllocationGranularity);
						dwOffSetLow = (dwOffSetLow / ms_dwAllocationGranularity) * ms_dwAllocationGranularity;
					}
	// 				else
	// 				{
	// 					dwOffSetHigh = static_cast<DWORD>(i64Seek >> 32);
	// 					iAdjust = static_cast<PgIFMMView::size_type>(dwOffSetHigh % ms_dwAllocationGranularity);
	// 					dwOffSetHigh = (dwOffSetHigh / ms_dwAllocationGranularity) * ms_dwAllocationGranularity;
	// 				}

					size_type const iNumberOfBytesToMap = iAdjust + iSize;
					if ( iNumberOfBytesToMap < iSize )
					{
						return false;
					}

					LPVOID lpData = ::MapViewOfFile( m_hFileMapHandle, FILE_MAP_WRITE, dwOffSetHigh, dwOffSetLow, iNumberOfBytesToMap );
					if ( lpData )
					{
						return SUCCEEDED(rkOutView.Init( m_kID, reinterpret_cast<data_type*>(lpData), iAdjust, iNumberOfBytesToMap ));
					}
				}
				else
				{
					throw static_cast<DWORD>(iSize);;
				}
			}
			catch( DWORD dwNeedSize )
			{
				if ( dwNeedSize && m_dwGrowSize )
				{
					DWORD const dwSize = static_cast<DWORD>(m_iSize);
					DWORD const dwRemainSize = ULONG_MAX - dwSize;
					if ( dwRemainSize < dwNeedSize )
					{
						return false;
					}

					DWORD dwAddSize = m_dwGrowSize * ( dwNeedSize / m_dwGrowSize );
					if ( dwNeedSize % m_dwGrowSize )
					{
						dwAddSize += m_dwGrowSize;
					}

					DWORD dwMaximumSizeLow = dwSize + dwAddSize;
					if ( dwMaximumSizeLow < dwSize )
					{// over났음.
						dwMaximumSizeLow = ULONG_MAX;
					}

					if ( NO_ERROR == Reserve( dwMaximumSizeLow ) )
					{
						return GetView( rkOutView, iSeek, iSize );
					}
				}
			}
		}
		return false;
	}

	bool PgXFStream_FMM::Push( void const *pData, size_type const iSize )
	{
		PgXStream_FMMView kView;
		if ( true == GetView( kView, m_iWritePos, iSize ) )
		{
			if ( true == kView.Push( pData, iSize ) )
			{
				m_iWritePos += iSize;
				return true;
			}
		}

		return false;
	}

	bool PgXFStream_FMM::Pop( void *pData, size_type const iSize )
	{
		PgXStream_FMMView kView;
		if ( true == GetView( kView, m_iReadPos, iSize ) )
		{
			if ( true == kView.Pop( pData, iSize ) )
			{
				m_iReadPos += iSize;
				return true;
			}
		}
		return false;
	}

};//<-- namespace BM
