#pragma once
#include "ace/Task_ex_t.h"
#include "CEL/DBWorker_Base.h"

namespace CEL
{
	class CDBWorker_Eraser//쿼리 결과, 디비 커넥션등을 지워주는 기능
		:	public	CDBWorker_Base
		,	public	ACE_Task_Ex< ACE_MT_SYNCH , DB_RESULT* >
	{
		typedef enum eValue
		{
			E_WATER_MARK	= 500 * 1024 * 1024,//! 5000K 까지 버텨준다.
		}E_VALUE;
	public:
		CDBWorker_Eraser();
		virtual ~CDBWorker_Eraser();

	public:
		virtual bool VInit();
		virtual bool VActivate(int const iForceThreadCount = 0);
		virtual bool VDeactivate();

		HRESULT VPush(DB_RESULT &rQuery);//쿼리 결과 버리기.
		
		virtual HRESULT VPush(DB_QUERY_TRAN const& rkQuery){return S_OK;}//얘는 리얼 쿼리 처리할 일이 없음.
		
		virtual int svc (void);

		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType);
		
		virtual std::wstring const&  ConnString()const{static std::wstring kStr; return kStr;};
	protected:
		virtual HRESULT ExecuteQuery(CDBConnection &rkConn, DB_QUERY const& rkQuery, CEL::DB_RESULT& rkRet){return E_FAIL;}
		virtual HRESULT VProcess(DB_QUERY_TRAN const& rkQuery){return S_OK;}//! 셋팅된 쿼리 넣기
		
		virtual size_t GetQueryCount()
		{
			return msg_queue()->message_count();
		}

		CLASS_DECLARATION( bool, m_bIsStop, IsStop );
	};

}