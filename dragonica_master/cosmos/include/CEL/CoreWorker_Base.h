#pragma once
#include "Common.h"

namespace CEL
{
	class CCoreCenter;

	class CCoreWorker_Base//얘는 CoreCenter와 별개로 간다. 단 CoreCenter에서 CoreWorker 를 접근 할 수 없어야 한다.
	{	//원칙
		//CoreWorker 가 CoreCenter 로의 접근 ==> O
		//CoreCenter 가 CoreWorker 로의 접근 ==> X
		friend class CCoreCenter;
	public:
		CCoreWorker_Base()
		{
		}
		virtual ~CCoreWorker_Base(){}
	public:
		virtual bool VActivate(int const iForceThreadCount) = 0;
		virtual bool VDeactivate() = 0;

		virtual bool Send(SESSION_KEY const& kSessionKey,  BM::Stream const& rPacket) = 0;//소유자와 child 의 키
	};
}