#pragma once

#include "Loki/Threads.h"
#include "BM/HSEL.h"
#include "CEL/Session_Base.h"
#include "./Header/CEL_EL.h"

namespace CEL
{
	template< typename T_HEADER_TYPE >
	class PgSession
		:	public CSession_Base
	{
	public:
		typedef typename T_HEADER_TYPE HEADER_TYPE;
		
		PgSession(void);
		virtual ~PgSession(void);

	protected:
		virtual HRESULT VGetEncodeKey(std::vector< char > &rkKeyOut)const;
		virtual HRESULT VSetEncodeKey(std::vector< char > const& rkKeyIn);

		virtual bool VOpen();
		virtual bool VImprovePacket(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader);
		virtual HRESULT VDecodeRecved(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader);

		virtual bool IsAlive() const;

	protected:
		CEL::CEL_EL m_kSendEncoder;//서버기준으로 send 클라이언트 기준으로 Recv.
		CEL::CEL_EL m_kRecvEncoder;
		bool m_bIsAlive;

		bool m_bIsFirstSend;
		bool m_bIsFirstRecv;
	};

	template< typename T_HEADER_TYPE >
	class PgSession_NoneEnc
		:	public CSession_Base
	{
	public:
		typedef typename T_HEADER_TYPE HEADER_TYPE;

		PgSession_NoneEnc(void){m_bIsAlive = false;}
		virtual ~PgSession_NoneEnc(void){}

	protected:
		virtual HRESULT VGetEncodeKey(std::vector< char > &rkKeyOut)const{return S_OK;}
		virtual HRESULT VSetEncodeKey(std::vector< char > const& rkKeyIn){return S_OK;}

		virtual bool VOpen(){m_bIsAlive = true; return true;}
		virtual bool VImprovePacket(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader);
		virtual HRESULT VDecodeRecved(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader);

		virtual bool IsAlive() const{return m_bIsAlive;}
	
	protected:
		bool m_bIsAlive;
	};

	template< typename T_HEADER_TYPE >
	class PgSession_NoneEncWithZIP
		:	public CSession_Base
	{
	public:
		typedef typename T_HEADER_TYPE HEADER_TYPE;

		PgSession_NoneEncWithZIP(void)
		{
			m_bIsAlive = false;
			m_bIsFirstSend = true;
			m_bIsFirstRecv = true;
		}
		virtual ~PgSession_NoneEncWithZIP(void){}

	protected:
		virtual HRESULT VGetEncodeKey(std::vector< char > &rkKeyOut)const{return S_OK;}
		virtual HRESULT VSetEncodeKey(std::vector< char > const& rkKeyIn){return S_OK;}

		virtual bool VOpen(){m_bIsAlive = true; return true;}
		virtual bool VImprovePacket(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader);
		virtual HRESULT VDecodeRecved(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader);

		virtual bool IsAlive() const{return m_bIsAlive;}
	
	protected:
		bool m_bIsAlive;
		bool m_bIsFirstSend;
		bool m_bIsFirstRecv;
	};
}

#include "Session.inl"