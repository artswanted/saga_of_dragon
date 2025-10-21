#pragma once
#include "CEL/Common.h"

namespace CEL
{
	typedef struct tagConnectInfo
	{
		BM::GUID guidOrder;
		
		BM::GUID guidConnector;
		ADDR_INFO addr;

		BM::GUID guidSession;
	}SConnectInfo;

	class CCoreCenter;

	class CCoreConnector_Base//얘는 CoreCenter와 별개로 간다. 단 CoreCenter에서 CoreWorker 를 접근 할 수 없어야 한다.
	{	//원칙
		//CoreWorker 가 CoreCenter 로의 접근 ==> O
		//CoreCenter 가 CoreWorker 로의 접근 ==> X
		friend class CCoreCenter;
	public:
		CCoreConnector_Base()
		{
		}
		virtual ~CCoreConnector_Base(){}
	public:
		virtual bool VActivate() = 0;
		virtual bool VDeactivate() = 0;

		virtual bool Connect(ADDR_INFO const& rAddr, SESSION_KEY &kSessionKey, BM::GUID &kConnectObj) = 0;//! 등록된 결과 GUID를 반환 //바로 스레드가 시작. Init 에 스레드 값을 셋팅하고 SvcStart 를 하는게 정석.
	};
}