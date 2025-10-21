#pragma once

#include "BM/XStream.h"

namespace BM
{

	class PgXFStream
		:	public PgXStream
	{
	public:
		PgXFStream(void){};
		virtual ~PgXFStream(void){}

	public:
		virtual HRESULT Open( std::wstring const &wstrFileName, size_type iSize = 0 ) = 0;
		virtual void Close(void) = 0;
		virtual bool IsOpen(void)const = 0;

	protected:
		std::wstring	m_wstrFileName;
	};

	class PgXFStream_FMM
		:	public PgXFStream
	{
	public:
		PgXFStream_FMM( DWORD const dwGrowSize = 32 );
		virtual ~PgXFStream_FMM(void);

	public:
		virtual void Release(void){Close();}

		virtual HRESULT Open( std::wstring const &wstrFileName, size_type iSize = 0 );
		virtual void Close(void);
		virtual bool IsOpen(void)const{return INVALID_HANDLE_VALUE != m_hFileMapHandle;}

		virtual size_type GetMaxSize(void)const{return m_iSize;}

		BM::GUID const &GetID(void)const{return m_kID;}

		virtual bool Push( void const *pData, size_type const iSize );

		template< typename T >
		bool Push( T const & rhs );

		template<>
		bool Push( std::string const &str );

		template<>
		bool Push( std::wstring const &wstr );

		template<>
		bool Push( vstring const &vstr );

		virtual bool Pop( void *pData, size_type const iSize );

		template< typename T >
		bool Pop( T & rhs );

	private:
		bool GetView( PgXStream_FMMView &rkOutView, size_type const iSeek, size_type const iSize );
		HRESULT Reserve( DWORD const dwMaximumSizeLow );

	protected:
		static DWORD	ms_dwAllocationGranularity;
		BM::GUID const	m_kID;
		HANDLE			m_hFileHandle;
		HANDLE			m_hFileMapHandle;

		DWORD			m_dwGrowSize;
		size_type		m_iSize;
	};

#include "XFStream.inl"

};//<-- namespace BM
