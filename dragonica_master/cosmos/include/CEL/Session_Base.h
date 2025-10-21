#pragma once
#include <vector>
#include "CEL/Common.h"

namespace CEL
{	
	class CSession_Base
	{
	protected:
		typedef BM::Stream PACKET_TYPE;
			
	public:
		CSession_Base(){}
		virtual ~CSession_Base(){};
	
	public://핸들러 레벨 재정의
		virtual bool VSend(PACKET_TYPE const& rPacket, bool const bIsEncryption = true)=0;
		virtual bool VTerminate()=0;
		virtual bool IsAlive() const=0;

	public://세션 레벨에서 재정의.
		virtual bool VOpen()=0;
		virtual bool VImprovePacket(PACKET_TYPE &rkStream, void * const pkStreamHeader)=0;
		virtual HRESULT VDecodeRecved(PACKET_TYPE &rPacket, void * const pkStreamHeader)=0;
		virtual HRESULT VGetEncodeKey(std::vector< char > &rkKeyOut)const=0;
		virtual HRESULT VSetEncodeKey(std::vector< char > const& rkKeyIn)=0;
	
		virtual HRESULT Statistics(SSessionStatistics &rkOutResult)const=0;
	protected:
		CLASS_DECLARATION_S(ADDR_INFO, Addr);
		CLASS_DECLARATION_S(CEL::SESSION_KEY, SessionKey);
		CLASS_DECLARATION_S(BM::GUID, CustomKey);//For Contents Control

	public:
		PACKET_TYPE m_kSessionData;//public
	};
}