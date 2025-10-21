#include "stdafx.h"
#include "PgNetModule.h"

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::PgNetModule(void)
:	m_kID( T_ID() )
,	m_kNet( T_NET() )
,	m_bReady( true )
{
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::PgNetModule( bool const bReady )
:	m_kID( T_ID() )
,	m_kNet( T_NET() )
,	m_bReady( bReady )
{
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::~PgNetModule(void)
{
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::PgNetModule( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & rhs )
:	m_kID(rhs.m_kID)
,	m_kNet(rhs.m_kNet)
,	m_bReady(rhs.m_bReady)
,	m_kPacketBuffer(rhs.m_kPacketBuffer)
{
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >& PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::operator = ( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & rhs )
{
	m_kID = rhs.m_kID;
	m_kNet = rhs.m_kNet;
	m_bReady = rhs.m_bReady;
	m_kPacketBuffer = rhs.m_kPacketBuffer;
	return *this;
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
bool PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::operator < ( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & rhs ) const
{
	if ( m_kNet == rhs.m_kNet )
	{
		return m_kID < rhs.m_kID;
	}
	return m_kNet < rhs.m_kNet;
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
bool PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::operator > ( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & rhs ) const
{
	return rhs < (*this);
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
bool PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::operator ==( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & rhs ) const
{
	if ( rhs < (*this) ){return false;}
	if ( (*this) < rhs ){return false;}
	return true;
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
void PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::Set( T_ID const &kID, T_NET const &kNet )
{
	SetID( kID );
	SetNet( kNet );
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
bool PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::Send( T_PACKET const &kPacket, bool const bMustNowSend, bool const bMustSend ) const
{
	static size_t iSize = 1;

	if (	(true == m_bReady)
		||	(true == bMustNowSend)
	)
	{
		T_PACKET kWrappedPacket( PT_M_S_WRAPPED_USER_PACKET );
		kWrappedPacket.Push( iSize );
		kWrappedPacket.Push( m_kID );
		BM::Stream kPurePacket;
		kPurePacket.Push(kPacket);
		kWrappedPacket.Push( kPurePacket.Data() );//rd_pos 가 바뀌거나 했을 수 있으므로
		if ( true == g_kProcessCfg.Locked_SendToServer( m_kNet, kWrappedPacket) )
		{
			return true;
		}
	}
	else if ( true == bMustSend )
	{
		m_kPacketBuffer.push_back( kPacket );
		if ( m_kPacketBuffer.size() > packet_buffer_max_size )
		{
			// 버퍼를 오버하면 어떻게 하지... 
			// 보내긴 해야 하지 않을까?
			Send( m_kPacketBuffer.front(), true, false );// 무한 루프 조심!!!!
			m_kPacketBuffer.pop_front();
		}
		return true;
	}
	return false;
}

template< typename T_ID, typename T_NET, typename T_PACKET, size_t packet_buffer_max_size >
void PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >::SetReady( bool const bReady )
{
	m_bReady = bReady;
	if ( true == m_bReady )
	{
		CONT_PACKET_BUFFER::iterator itr = m_kPacketBuffer.begin();
		for ( ; itr != m_kPacketBuffer.end() ; ++itr )
		{
			Send( *itr, true, false );
		}
		m_kPacketBuffer.clear();
	}
}
