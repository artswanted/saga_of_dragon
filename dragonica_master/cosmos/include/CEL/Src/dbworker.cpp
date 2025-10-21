#include "stdafx.h"
#include "ace/high_res_Timer.h"
#include "BM/Stream.h"
#include "BM/LocalMgr.h"
#include "../query.h"
#include "../Header/DBWorker.h"
#include "Cel_Log.h"
#include "BM/TImeCheck.h"

//#define DB_LOG_FOR_DEBUG(a,b)		{	BM::vstring kLogMsg; kLogMsg << b ; m_rkDBLog.LogNoArg(a, (std:: wstring const&)kLogMsg);}//ó����Ȳ �α׸� ���ϸ� Ǯ��.
#define DB_LOG_FOR_DEBUG(a,b)		{}
#define DB_LOG(a,b)		{SDebugLogMessage kMsg(m_kLogType, a, (BM::vstring() << b).operator std::wstring const&()); m_kLogWorker.PushLog(kMsg);}
//#define DB_LOG(a,b)		CEL_LOG(a,b)

using namespace CEL;

	class CAutoReleaseConnection
	{
	public:
		CAutoReleaseConnection(CDBSession* pkSession)
			:m_pkSession(pkSession)
		{	
		}
		~CAutoReleaseConnection()
		{
			SAFE_DELETE(m_pkSession);
//			m_rkWorker_Deleter.PushConnection(m_pkConnection);
		}

		CDBSession* m_pkSession;
	};


CDBWorker::CDBWorker(INIT_DB_DESC const& rInitInfo
					, std::wstring const &kConnStr, PgLogWorker_Base &rkLogWorker, DWORD const iLogType, DWORD const dwWorkerIndex)
	:	CDBWorker_Base( rInitInfo )
	,	m_kConnString(kConnStr)
	,	m_kLogWorker(rkLogWorker)
	,	m_kLogType(iLogType)
	,	m_dwWorkerIndex(dwWorkerIndex)
//	,	m_QueryPool(100, 100)
{
	m_kIsStop = false;
	m_eDBType = DBWT_DBWORKER;
	msg_queue()->high_water_mark(E_WATER_MARK);
	
	m_kAccProcessedQueryCount = 0;
	m_kAccQueuedQueryCount = 0;
//	CDBConnection::ConnString(strConn);
//	m_pkParentConnection = pkConn;
}

CDBWorker::~CDBWorker()
{
}

bool CDBWorker::VInit()
{
//	if(!RegistDBConn(m_kInitInfo.kDBInfo) )//Init���� OpenDB�� �ؼ�. Ŀ�ؼ� Ǯ�� ����� �غ� ��.
	{//! �ִ��Ŵ� ������� �Ǵµ�.
	//	assert(NULL && "RegistDBConn Failed");//
//		m_rkDBLog.Log(BM::LOG_LV6, _T("[%s] ConnectDB Failed [%s]"), __FUNCTIONW__, ConnString());
//		return false;
	}
	return true;
}

bool CDBWorker::VActivate(int const iForceThreadCount)
{//��� ��Ŀ�� ������ �����尡 1��.
	if(-1 == this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1, 0, THREAD_BASE_PRIORITY_MAX))//THREAD_BASE_PRIORITY_MAX
	{
		__asm int 3;
		return false;
	}
	return true;
}

bool CDBWorker::VDeactivate()
{
	m_kIsStop = true;
	msg_queue()->pulse();
	
	int const iRet = ACE_Thread_Manager::instance()->wait_task( this );
	if( 0 == iRet )
	{
		return true;
	}
	return false;
}
/*
bool CDBWorker::RegistDBConn( DB_INFO const& rDBInfo)
{
	bool const bRet = CDBConnection::Init(rDBInfo, m_kInitInfo.bUseConsoleLog);
	if( bRet )
	{
		if(m_kInitInfo.bUseConsoleLog)
		{
			m_rkDBLog.Log(BM::LOG_LV6, _T("[%s] DBConn Reg.....DBIndex[%d]"), __FUNCTIONW__, rDBInfo.DBIndex());
		}
		return true;
	}

	return false;
}
*/
void CDBWorker::OnBeginQuery(DB_QUERY_TRAN const& rkQuery)
{
	BM::CAutoMutex kLock(m_kWorkingtargetMutex, true);

	CONT_QUERY_TARGET::const_iterator target_itor = rkQuery.m_kContQueryTarget.begin();
	
	while(target_itor != rkQuery.m_kContQueryTarget.end())
	{
		auto ret = m_kContWorkingInfo.insert(std::make_pair((*target_itor), 1));
		if(!ret.second)
		{//�ִ°� ���и� ������ ���� ����.
			++((*ret.first).second);
		}
		++target_itor;
	}
}

void CDBWorker::OnCompleteQuery(DB_QUERY_TRAN const& rkQuery)
{
	BM::CAutoMutex kLock(m_kWorkingtargetMutex, true);

	CONT_QUERY_TARGET::const_iterator target_itor = rkQuery.m_kContQueryTarget.begin();
	
	while(target_itor != rkQuery.m_kContQueryTarget.end())
	{
		CONT_WORKING_TARGET_INFO::iterator working_target_itor = m_kContWorkingInfo.find((*target_itor));
		if(working_target_itor != m_kContWorkingInfo.end())
		{
			--((*working_target_itor).second);
			
			if(!(*working_target_itor).second)
			{//0 ī��Ʈ.
				m_kContWorkingInfo.erase(working_target_itor);
			}
		}
		else
		{
			__asm int 3;
		}

		++target_itor;
	}
}

HRESULT CDBWorker::VPush(DB_QUERY_TRAN const& rkQuery)
{//ī�� ���� �ʱ� ���� �����İ� new �ؿ°� �޴´�.
//	DB_QUERY_TRAN *pQuery = new DB_QUERY_TRAN(rkQuery);
	DB_QUERY_TRAN *pQuery = const_cast<DB_QUERY_TRAN *>(&rkQuery);
	if( pQuery )
	{
		DB_QUERY_TRAN** ppkMsg = (DB_QUERY_TRAN**)pQuery;

//		ACE_Time_Value tv(0, 1000);	tv += ACE_OS::gettimeofday();
		OnBeginQuery(*pQuery);
		if( -1 != msg_queue()->enqueue(ppkMsg) )
		{
			++m_kAccQueuedQueryCount;
			return S_OK;
		}
		else
		{//���н�.
			OnCompleteQuery(*pQuery);
		}
		SAFE_DELETE(pQuery);
	}
	return E_FAIL;
}//! ���õ� ���� �ֱ�

int CDBWorker::svc (void)
{
	while( !IsStop() )//! stop �ñ׳� �ɾ ���ٰ�.
	{
		DB_QUERY_TRAN **ppkMsg = NULL;
//		ACE_Time_Value tv(0, 1000);	tv += ACE_OS::gettimeofday();
		if( -1 != msg_queue()->dequeue( ppkMsg ) )
		{//����!
			DB_QUERY_TRAN *pkMsg = (DB_QUERY_TRAN*)ppkMsg;
			VProcess(*pkMsg);
//			Sleep(100);// -> �׽�Ʈ��.
			++m_kAccProcessedQueryCount;
			OnCompleteQuery(*pkMsg);
			
			SAFE_DELETE(pkMsg);
		}
	}
	return 1;
}

HRESULT CDBWorker::VProcess(DB_QUERY_TRAN const& rkQuery)
{
	BM::CAutoMutex kLock(m_kProcessMutex);

	HRESULT hResult = S_OK;

	CDBConnection kDataConnection(ConnString());
	if(!kDataConnection.OpenDB())
	{
		return E_FAIL;
	}
	
	DB_LOG_FOR_DEBUG(BM::LOG_LV1, m_dwWorkerIndex << _T(" OpenDB "));

//////////////////////////////////////
	CDBConnection *pkConn = &kDataConnection;

	CDBSession *pkSession = &pkConn->Session();

	pkSession->m_pkBufResult = m_kResultBuf;

	if(rkQuery.size() == 1)	// ������ �ϳ����̸� Ʈ������� Ÿ�� �ʴ´�.
	{
		DB_QUERY const&  kQuery = rkQuery.front();
		CEL::DB_RESULT kRet(kQuery);

		PgTimeCheck kTimeCheck( &m_kLogWorker, m_kLogType, _T("SQL_PROCESS SINGLE"), 0, 3500, kQuery.Command());//1�� ������. �α� ����.
		
		hResult = CallProcedureNoParam(pkSession, kQuery, kRet);//���и� �ص�. OnDBExeute �� ���ư�����.

		if(	kRet.eRet != DR_NO_RESULT 
		&&	kRet.eRet != DR_SUCCESS
		&&	m_kInitInfo.bUseConsoleLog)
		{
			DB_LOG(BM::LOG_LV5, m_dwWorkerIndex << _T(" ") << kQuery.Command() << _T(" Error=") << static_cast<int>(kRet.eRet) );
			assert(false);
		}

		DB_LOG_FOR_DEBUG(BM::LOG_LV1, m_dwWorkerIndex << _T(" Single QueryEnd "));
		if (kQuery.QueryResultCallback())
			return kQuery.QueryResultCallback()(kRet);
		return m_kInitInfo.OnDBExecute( kRet );
	}
	else
	{//������ �������̴�.
		DB_RESULT_TRAN kRetTran(rkQuery);

		hResult = pkSession->StartTransaction();
		
		DB_LOG_FOR_DEBUG(BM::LOG_LV1, m_dwWorkerIndex << _T(" Begin Tran"));
		if(FAILED(hResult))
		{
			for(DB_QUERY_TRAN::const_iterator citer = rkQuery.begin();citer != rkQuery.end();++citer)
			{
				CEL::DB_RESULT kRet((*citer));
				kRet.eRet = DR_ERR_BEGIN_TRAN;
				kRetTran.push_back(kRet);
				
				DB_LOG_FOR_DEBUG(BM::LOG_LV1, m_dwWorkerIndex << _T(" Tran Query ") << (*citer).Command());

				if( m_kInitInfo.bUseConsoleLog)
				{
					DB_LOG(BM::LOG_LV5, m_dwWorkerIndex << _T(" Begin Tran Query ") << (*citer).Command() << _T(" Error=")<< static_cast<int>(kRet.eRet) );
					assert(false);
				}
			}
			m_kInitInfo.OnDBExecuteTran( kRetTran );
			DB_LOG_FOR_DEBUG(BM::LOG_LV1, m_dwWorkerIndex << _T(" Tran Failed"));
			return hResult;
		}

		bool bIsRollback = false;

		int i = 0;
		for(DB_QUERY_TRAN::const_iterator citer = rkQuery.begin();citer != rkQuery.end();++citer, ++i)
		{
			PgTimeCheck kTimeCheck( &m_kLogWorker, m_kLogType, _T("SQL_PROCESS TRAN"), i, 3500, (*citer).Command());//500ms ������. �α� ����.

			CEL::DB_RESULT kRet((*citer));

			hResult = CallProcedureNoParam( pkSession, (*citer), kRet);//���и� �ص�. OnDBExeute �� ���ư�����.

			kRetTran.push_back(kRet);

			if(	kRet.eRet != DR_NO_RESULT 
			&&	kRet.eRet != DR_SUCCESS
			&&	m_kInitInfo.bUseConsoleLog)
			{
				DB_LOG(BM::LOG_LV5, m_dwWorkerIndex << _T(" Do Query ") << (*citer).Command() << _T(" Error=") << static_cast<int>(kRet.eRet));
				assert(false);
			}

			if(FAILED(hResult))	// ��� ó�� ����� ������ ���ϴ� ������ �ƴ϶� ���� �Ҷ��� Ʈ������ �ѹ��� �����ش�.
			{
				bIsRollback = true;
				break;
			}
		}

		HRESULT hUserResult = m_kInitInfo.OnDBExecuteTran(kRetTran);

		if(FAILED(hUserResult))	// ��� ó�� ����� ������ ���ϴ� ������ �ƴ϶� ���� �Ҷ��� Ʈ������ �ѹ��� �����ش�.
		{
			bIsRollback = true;
		}

		if(bIsRollback)
		{
			DB_LOG_FOR_DEBUG(BM::LOG_LV1, m_dwWorkerIndex << _T(" Rollback Tran"));
			hResult = pkSession->Abort();
		}
		else
		{
			DB_LOG_FOR_DEBUG(BM::LOG_LV1, m_dwWorkerIndex << _T(" Commit Tran"));
			hResult = pkSession->Commit();						// ��� ������ ���� �ϸ� ? Ŀ�� ���� �ش�.
		}

		return hResult;
	}
}

HRESULT CDBWorker::CallProcedureNoParam(CDBSession * pkSession,DB_QUERY const& rkQuery, CEL::DB_RESULT& rkRet)
{
	USES_CONVERSION;

	rkRet.eRet = DR_SUCCESS;

	CManualQuery kQuery;
	HRESULT hResult = kQuery.Open(*pkSession, rkQuery.Command().c_str());

	DB_LOG_FOR_DEBUG(BM::LOG_LV1, m_dwWorkerIndex << _T(" QueryOpen ") << rkQuery.Command());

	if(FAILED(hResult))
	{
		std::wstring wstrError = kQuery.GetErrorMsg();
		DB_LOG(BM::LOG_LV0, m_dwWorkerIndex << _T(" HRESULT = ") << HEX(hResult) << _T(" ErrorMsg1 =") << wstrError);
		rkRet.eRet = DR_ERR_QUERY;
		return E_FAIL;
	}

	int iQueryResultCount = 0;

	if(kQuery.GetInterface())
	{
		for(;;)
		{
			// Get the column information and bind the output columns.
			DBORDINAL				ulColumns = 0;
			DBCOLUMNINFO	*		pColumnInfo = NULL;
			OLECHAR			*		pColumnNames = NULL;
			DBBINDDATA		*		pkBindData = NULL;

			hResult = kQuery.GetColumnInfo(&ulColumns,&pColumnInfo,&pColumnNames);
			if(FAILED(hResult))
			{
				std::wstring wstrError = kQuery.GetErrorMsg();
				DB_LOG(BM::LOG_LV0, m_dwWorkerIndex << _T(" HRESULT = ") << HEX(hResult) << _T(" ErrorMsg2 =") << wstrError);
				rkRet.eRet = DR_ERR_CALL_BINDINFO;
				return E_FAIL;
			}

			if(ulColumns > 0)
			{
				pkSession->ReserveDBBind(ulColumns);
				pkBindData = pkSession->BindData();

				hResult = kQuery.CreateAccessor((int)ulColumns,pkBindData, sizeof(DBBINDDATA)*ulColumns);
				if(FAILED(hResult))
				{
					std::wstring wstrError = kQuery.GetErrorMsg();
					DB_LOG(BM::LOG_LV0, m_dwWorkerIndex << _T(" HRESULT = ") << hResult << _T(" ErrorMsg3 =") << wstrError);
					rkRet.eRet = DB_ERR_CREATE_ACCESSOR;
					return E_FAIL;
				}

				for (ULONG l=0; l<ulColumns; l++)
				{
					kQuery.AddBindEntry(l+1, pColumnInfo[l].wType,CManualQuery::GetBindSize(pColumnInfo[l].wType),pkBindData[l].kBindData.Data,&pkBindData[l].dwLength,&pkBindData[l].dwState);
				}
			}

			hResult = kQuery.Bind();
			if(FAILED(hResult))
			{
				CoTaskMemFree(pColumnInfo);
				CoTaskMemFree(pColumnNames);
				rkRet.eRet = DR_ERR_BIND_COLUMN;
				return E_FAIL;
			}

			hResult = S_OK;

			DBROWCOUNT kDBRowCount = 0;

			iQueryResultCount = 0;

			while(S_OK == kQuery.MoveNext())
			{
				for(ULONG l = 0;l < ulColumns;l++)
				{
					CEL::DB_DATA kDBData;
					kDBData.DataType = pColumnInfo[l].wType;
					
					/*//���� ���� üũ �Ҷ� �� ������.
					if(pColumnInfo[l].wType != 130)
					{
						if(	pkBindData[l].dwState != 3
						&&(	!pColumnInfo[l].wType
						||	!pkBindData[l].dwLength))
						{
							__asm int 3;
						}
					}
					*/

					kDBData.Push( pkBindData[l].kBindData.Data, pkBindData[l].dwLength );
					rkRet.vecArray.push_back( kDBData );
				}

				++iQueryResultCount;
			}

			rkRet.vecResultCount.push_back(iQueryResultCount);

			hResult = kQuery.GetNextResult(&kDBRowCount);

			CoTaskMemFree(pColumnInfo);
			CoTaskMemFree(pColumnNames);

			if(	FAILED(hResult) 
			||	(hResult == DB_S_NORESULT))
//			||	(SUCCEEDED(hResult) && !kQuery.GetInterface()))//�����ε� �ڿ� �������̽��� ����.(select �� ����)
			{
				break;
			}
		}

		/*=================================================================================
			����� ����� ���� ��찡 ������ ���� ó���� ����� �Ѵ�. ���� ���� ��� ����
			������ ���� ���� � ������ DR_NO_RESULT ���������� ���� ���Ѵ�.
			���� �Ұ�..
		==================================================================================*/
		rkRet.eRet = ((rkRet.vecArray.empty())?DR_NO_RESULT:rkRet.eRet);
	}
	else
	{
		rkRet.eRet = DR_NO_RESULT;
		rkRet.vecResultCount.push_back(iQueryResultCount);
	}

	return S_OK;
}

bool CDBWorker::IsEmpty()const
{
	BM::CAutoMutex kLock(m_kWorkingtargetMutex, false);

	return m_kContWorkingInfo.empty();
}

bool CDBWorker::IsInputAvailable(DB_QUERY_TRAN const& rkQuery)const
{
	BM::CAutoMutex kLock(m_kWorkingtargetMutex, false);

	CONT_QUERY_TARGET::const_iterator target_itor = rkQuery.m_kContQueryTarget.begin();
	
	while(target_itor != rkQuery.m_kContQueryTarget.end())
	{
		CONT_WORKING_TARGET_INFO::const_iterator working_target_itor = m_kContWorkingInfo.find(*target_itor);
		if(working_target_itor != m_kContWorkingInfo.end())
		{
			return true;
		}
		++target_itor;
	}

	return false;
}

void CDBWorker::VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)
{
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring)_T("===== CDBWorker State Display ===== IsRun[") << !IsStop() <<  _T("]")));
//	kLogger.Log(BM::LOG_LV1, _T("Remain Msg Count [%d], Thread Count [%d]"), msg_queue()->cur_count_, m_kInitInfo.dwThreadCount);
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("ConnInfo [") << m_kInitInfo.kDBInfo.DataToString() <<  _T("]")));
}