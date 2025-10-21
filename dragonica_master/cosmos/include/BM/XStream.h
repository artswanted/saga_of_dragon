#pragma once

#include "vstring.h"

namespace BM
{
	class PgXStream
	{
	public:
		typedef char	data_type;
		typedef size_t	size_type;

	public:
		PgXStream(void);
		virtual ~PgXStream(void){};

	public:
		virtual void Release(void) = 0;

		// Push
		virtual bool Push( void const *pData, size_type const iSize ) = 0;

		// Pop
		virtual bool Pop( void *pData, size_type const iSize ) = 0;

		virtual size_type GetMaxSize(void)const = 0;
		virtual size_type GetRemainWriteSize(void)const{return this->GetMaxSize() - GetWritePos();}
		virtual size_type GetRemainReadSize(void)const{return this->GetMaxSize() - GetReadPos();}

		virtual size_type GetWritePos(void)const{return m_iWritePos;}
		virtual size_type GetReadPos(void)const{return m_iReadPos;}

		virtual bool SetWritePos( size_type const iPos );
		virtual bool SetReadPos( size_type const iPos );
	protected:
		size_type	m_iWritePos;
		size_type	m_iReadPos;
	};
	
	class  PgXStream_FMMView
		:	public	PgXStream
	{
	public:
		PgXStream_FMMView(void);
		virtual ~PgXStream_FMMView(void);

	public:
		virtual bool Push( void const *pData, size_type const iSize );
		virtual bool Pop( void *pData, size_type const iSize );

		HRESULT Init( BM::GUID const &kID, data_type *pMem, size_type const iPosStart, size_type const iPosEnd );
		virtual void Release(void);

		BM::GUID const &GetID(void)const{return m_kID;}

		virtual size_type GetMaxSize(void)const{return m_iPosEnd - m_iPosStart;}
	protected:
		BM::GUID	m_kID;
		data_type*	m_pMem;
		size_type	m_iPosStart;
		size_type	m_iPosEnd;
	};

};//<- namespace BM
