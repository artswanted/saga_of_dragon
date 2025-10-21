#include "stdafx.h"
#include <tchar.h>
#include "BM/vstring.h"
#include "../Stream.h"

using namespace BM;

void Stream::CommonInit()
{
	m_vecData.reserve(256);//reserve
	Reset();
	m_bIsStopSignal = false;
}

Stream::Stream()
{
	CommonInit();
}

Stream::Stream(DEF_STREAM_TYPE const wStreamType)
{
	CommonInit();

	Reserve( sizeof(wStreamType) );
	Push(wStreamType);
}

Stream::Stream(Stream const& kStream)
{
	*this = kStream;
}

void Stream::operator = (Stream const&  rhs)
{
	if( this == &rhs)
	{
		return;
	}

	size_t const rhs_data_size = rhs.m_vecData.size();
	if(rhs_data_size)
	{
		m_vecData.reserve(rhs_data_size);
		m_vecData.resize(rhs_data_size);

		::memcpy(&m_vecData.at(0), &rhs.m_vecData.at(0), rhs.m_vecData.size());
	}

	m_bIsStopSignal = rhs.m_bIsStopSignal;
	rd_pos_ = rhs.rd_pos_;
	wr_pos_ = rhs.wr_pos_;
}

Stream::~Stream()
{
//	assert(rd_pos_ == wr_pos_);
}

void Stream::Swap(BM::Stream &rhs)
{
	if( this == &rhs)
	{
		return;
	}

	m_vecData.swap( rhs.m_vecData );
	std::swap( m_bIsStopSignal, rhs.m_bIsStopSignal );
	std::swap( rd_pos_, rhs.rd_pos_ );
	std::swap( wr_pos_, rhs.wr_pos_ );
}

void Stream::Reset()
{
	m_vecData.resize(0);
	rd_pos_ = 0;
	wr_pos_ = 0;
}

void Stream::Reset(DEF_STREAM_TYPE wPacketType)
{
	Reset();
	m_bIsStopSignal = false;
	Reserve( sizeof(wPacketType) );
	Push(wPacketType);
}

void Stream::Reserve(size_t const reserve_size)
{
	if( 0 < reserve_size )
	{
		m_vecData.reserve( reserve_size );
	}
}

void Stream::Resize(size_t const resize_size)
{
	if( 0 < resize_size )
	{
		m_vecData.reserve( resize_size );
		m_vecData.resize( resize_size );
	}
}

void Stream::PosAdjust()
{
	rd_pos_ = 0;
	wr_pos_ = m_vecData.size();
}

bool Stream::IsEmpty()const
{
	if(m_vecData.size())
	{
		return false;
	}
	return true;
}

void Stream::Push( void const *pData, size_t const in_size )
{
	if( 0 < in_size )
	{
		m_vecData.resize( wr_pos_ + in_size );
		::memcpy( &m_vecData.at(wr_pos_), pData, in_size );
		
		wr_pos_ += in_size;
	}
}

size_t Stream::Size() const//���� ��ü�� ������
{
	return m_vecData.size();
}

size_t Stream::RemainSize() const//���� �� ���� ������
{
	if(wr_pos_ > rd_pos_)
	{
		return wr_pos_ - rd_pos_;
	}

	return 0;
}

bool Stream::PopMemory(void *pBuffer, size_t const out_size)
{
	if( out_size
	&&	RemainSize() >= out_size)
	{
		::memcpy( pBuffer, &m_vecData.at(rd_pos_), out_size );
		rd_pos_ += out_size;
//! 	STREAM_DATA::iterator start = m_vecData.begin();
//! 	STREAM_DATA::iterator end = start + out_size;
//! 	m_vecData.erase(start, end);
		return true;
	}
	//assert(NULL && "Stream::PopMemory Failed");
	return false;
}

bool Stream::Pop(std::string &rElement, size_t const max_count)
{
	rElement.clear();

	size_t string_size = 0;
	if( !Pop( string_size ) ){return true;}
	if( !string_size ){return true;}
	if( string_size > max_count){return false;}
	if( string_size > RemainSize() ){return false;}
	
	rElement.clear();
	rElement.append( &m_vecData.at(rd_pos_), string_size );

	rd_pos_ += string_size;
	return true;
}

bool Stream::Pop(std::wstring &rElement, size_t const max_count)
{
	rElement.clear();

	size_t string_size = 0;
	if( !Pop( string_size ) ){return true;}
	if( !string_size ){return true;}
	if( string_size > max_count){return false;}
	if( string_size*sizeof(std::wstring::value_type) > RemainSize() ){return false;}
	
	rElement.clear();
	rElement.append( (std::wstring::value_type*)&m_vecData.at(rd_pos_), string_size );
	
	size_t const out_size = string_size*sizeof(std::wstring::value_type);
	rd_pos_ += out_size;
	return true;
}

bool Stream::Pop(BM::vstring &rElement, size_t const max_count )
{
	std::wstring wstr;
	if ( true == Pop( wstr, max_count ) )
	{
		rElement = wstr;
		return true;
	}
	return false;
}

void Stream::Clear()
{
	m_bIsStopSignal = false;
	Reset();
}

void Stream::SetStopSignal( bool const bIsStopSignal )
{
	m_bIsStopSignal = bIsStopSignal;
}

bool Stream::IsStopSignal() const 
{
	return m_bIsStopSignal;
}

Stream::STREAM_DATA& Stream::Data()
{
	return m_vecData;
}

 Stream::STREAM_DATA const&  Stream::Data()const
{
	return m_vecData;
}

HRESULT Stream::ModifyData(size_t const pos, void const *pData, size_t const in_size)
{
	if(Size() >= pos+in_size)//����� �¾ƾ���.
	{
		memcpy(&m_vecData.at(pos), pData, in_size);
		return S_OK;
	}
	return E_FAIL;
}