#ifndef WEAPON_VARIANT_NETWORK_PGNETMODULE_H
#define WEAPON_VARIANT_NETWORK_PGNETMODULE_H

template< typename T_ID=BM::GUID, typename T_NET=SERVER_IDENTITY, typename T_PACKET=BM::Stream, size_t packet_buffer_max_size = 200 >
class PgNetModule
{
public:
	typedef	std::list< T_PACKET >		CONT_PACKET_BUFFER;

public:
	typedef typename std::set< PgNetModule< typename T_ID, typename T_NET, typename T_PACKET, packet_buffer_max_size > >	CONT_NETMODULE;

public:
	PgNetModule(void);
	explicit PgNetModule( bool const bReady );
	virtual ~PgNetModule(void);

	explicit PgNetModule( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & );
	PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size >& operator = ( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & );

	bool operator < ( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & ) const;
	bool operator > ( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & ) const;
	bool operator ==( PgNetModule< T_ID, T_NET, T_PACKET, packet_buffer_max_size > const & ) const;

	void SetID( T_ID const &kID ){m_kID=kID;}
	void SetNet( T_NET const &kNet ){m_kNet=kNet;}
	void Set( T_ID const &, T_NET const & );
	void SetReady( bool const bReady );

	T_ID const& GetID()const{return m_kID;}
	T_NET const& GetNet()const{return m_kNet;}
	bool IsReady(void)const{return m_bReady;}

	virtual bool Send( T_PACKET const &, bool const bMustNowSend, bool const bMustSend )const;

protected:
	T_ID	m_kID;
	T_NET	m_kNet;
	bool	m_bReady;

	mutable CONT_PACKET_BUFFER	m_kPacketBuffer;// mutable
};

#endif // WEAPON_VARIANT_NETWORK_PGNETMODULE_H