// WSAENOBUFS -> 메세지 블럭에 내용이 클 경우. 나옴.
//-> 한번에 보낼 수 있는 최대 크기 지정 할 수 있어야 하겠음.


#include "stdafx.h"
#include "../CoreCenter.h"
#include "objbase.h"

#include "BM/LocalMgr.h"

#include "ACE/init_Ace.h"

#include "ace/ace.h"
#include "ace/OS_NS_time.h"
#include "ace/Asynch_Acceptor.h"
#include "ace/Asynch_Connector.h"
#include "ace/INET_Addr.h"
#include "ace/Message_Block.h"
#include "ace/Proactor.h"
#include "ace/SOCK_Stream.h"//! Peer 의 객체.

#include "BM/Stream.h"

#include "../Header/Acceptor.h"
#include "../Header/Connector.h"
#include "../Header/Service_Handler.h"
#include "../Header/Service_Handler_NC.h"
#include "../Header/Service_Handler_Gravity.h"
#include "../Header/Service_Handler_NoHeader.h"
#include "../Header/Session.h"
#include "../Header/Session_manager.h"
#include "../Header/Packet_handler.h"

#include "../Header/Timer.h"
#include "../Header/DBWorker_Dispatcher.h"
#include "../Header/DBWorker_Null.h"

#include "../Header/CoreWorker.h"
#include "../Header/CoreRegister.h"
#include "../Header/CoreConnector.h"
#include "../header/PacketHeader.h"
#include "Cel_Log.h"

using namespace CEL;

#define IG_LOG  /##/

int const NULL_DB_INDEX = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CAcceptor< CSession_Manager< Service_Handler< CEL::PgSession< SCorePacketHeader > > > >CEL_ACCEPTOR;
typedef CConnector< CSession_Manager< Service_Handler< CEL::PgSession< SCorePacketHeader > > > > CEL_CONNECTOR;

typedef CAcceptor< CSession_Manager< Service_Handler_NC< CEL::PgSession_NoneEnc< SCorePacketHeader_NC > > > > CEL_NC_ACCEPTOR;//NC 특화.
typedef CConnector< CSession_Manager< Service_Handler_NC< CEL::PgSession_NoneEnc< SCorePacketHeader_NC > > > > CEL_CONNECTOR_NC;

typedef CAcceptor< CSession_Manager< Service_Handler< CEL::PgSession_NoneEnc< tagCorePacketHeader_ServerToServer > > > > CEL_ACCEPTOR_SERVER;//서버간 특화
typedef CConnector< CSession_Manager< Service_Handler< CEL::PgSession_NoneEnc< tagCorePacketHeader_ServerToServer > > > > CEL_CONNECTOR_SERVER;//커넥터.

typedef CAcceptor< CSession_Manager< Service_Handler< CEL::PgSession_NoneEncWithZIP< tagCorePacketHeader_FileServer > > > > CEL_ACCEPTOR_FILE_SERVER;//서버간 특화
typedef CConnector< CSession_Manager< Service_Handler< CEL::PgSession_NoneEncWithZIP< tagCorePacketHeader_FileServer > > > > CEL_CONNECTOR_FILE_SERVER;//커넥터.

typedef CAcceptor< CSession_Manager< Service_Handler< CEL::PgSession_NoneEnc< SCorePacketHeader_GF > > > > CEL_ACCEPTOR_GF;// GF(SoftWorld) 특화
typedef CConnector< CSession_Manager< Service_Handler< CEL::PgSession_NoneEnc< SCorePacketHeader_GF > > > > CEL_CONNECTOR_GF;

typedef CAcceptor< CSession_Manager< Service_Handler_NoHeader< CEL::PgSession_NoneEnc< SCorePacketHeader_NoHeader > > > > CEL_ACCEPTOR_NOHEADER;// Header 없는 단순한 Session
typedef CConnector< CSession_Manager< Service_Handler_NoHeader< CEL::PgSession_NoneEnc< SCorePacketHeader_NoHeader > > > > CEL_CONNECTOR_NOHEADER;

typedef CAcceptor< CSession_Manager< Service_Handler< CEL::PgSession_NoneEnc< SCorePacketHeader_JAPAN > > > > CEL_ACCEPTOR_JAPAN;// GF(SoftWorld) 특화
typedef CConnector< CSession_Manager< Service_Handler< CEL::PgSession_NoneEnc< SCorePacketHeader_JAPAN > > > > CEL_CONNECTOR_JAPAN;

typedef CAcceptor< CSession_Manager< Service_Handler_Gravity< CEL::PgSession_NoneEnc< SCorePacketHeader_GRAVITY > > > > CEL_ACCEPTOR_GRAVITY;// Gravity 특화
typedef CConnector< CSession_Manager< Service_Handler_Gravity< CEL::PgSession_NoneEnc< SCorePacketHeader_GRAVITY > > > > CEL_CONNECTOR_GRAVITY;


CCoreCenter::CCoreCenter()
{
	InitEnv();

	ACE::init();
//	m_pCoreWorker = NULL;
	m_pCoreRegister = NULL;
	m_pCoreConnector = NULL;
	m_bIsClose = false;
}

CCoreCenter::~CCoreCenter()
{
	Close();

//	ACE_Proactor::instance()->proactor_end_event_loop();//! 엑셉터정지.
//	ACE_Proactor::instance()->close();

	ACE_Thread_Manager::instance()->wait();

	ACE::fini();

	CONT_TIMER::iterator timer_itor = m_kContTimer.begin();
	while(timer_itor != m_kContTimer.end())
	{
		SAFE_DELETE((*timer_itor).second);
		++timer_itor;
	}
	m_kContTimer.clear();

	SAFE_DELETE(m_pCoreRegister);
	SAFE_DELETE(m_pCoreConnector);

	CONT_WORKER::iterator con_iter = m_kContWorker.begin();
	while(m_kContWorker.end() !=  con_iter)
	{
		SAFE_DELETE((*con_iter).second );
		++con_iter;
	}
	m_kContWorker.clear();

	CONT_DB_DISPATCHER_WORKER::iterator db_iter = m_kContDBWorker.begin();
	while (m_kContDBWorker.end() != db_iter)
	{
		SAFE_DELETE((*db_iter).second );
		++db_iter;
	}
	m_kContDBWorker.clear();

	ClearEnv();
}

bool CCoreCenter::Init(INIT_CENTER_DESC const& kInit)
{
	if(!g_kCelLog.Init(kInit.eOT, BM::LFC_WHITE_I, kInit.m_kCelLogFolder, _T("CEL"), g_kLocal.GetLocale()) )
	{
		std::cout<< "CEL Log Init Failed" << std::endl;
		return false;
	}

	if(S_OK != BM::LZOInit())//네트웍에서 Lzo 를 쓰기 때문에
	{
		CEL_LOG(BM::LOG_LV0, __FL__ << _T("Lzo Init Failed"));
		__asm int 3;
		return false;
	}

	m_kInit = kInit;
	m_pCoreRegister		= new CCoreRegister;
	m_pCoreConnector	= new CCoreConnector;

	if(	!m_pCoreRegister
	||	!m_pCoreConnector )
	{
		CEL_LOG(BM::LOG_LV0, __FL__ << _T("CCore Creation failed"));
		__asm int 3;
		return false;
	}

	m_pCoreRegister->VActivate();//선형으로 돌도록 스레드는 1개만 등록
	m_pCoreConnector->VActivate();

	if(kInit.bIsUseDBWorker)
	{//디비 결과 버리는 null 워커.
//		INIT_DB_DESC kInitDbDesc;
//		kInitDbDesc.dwThreadCount = 1;
		CDBWorker_Base *pkNullWorker = new CDBWorker_Eraser();
		if(!pkNullWorker->VInit())
		{
			SAFE_DELETE(pkNullWorker);
			CEL_LOG(BM::LOG_LV0, __FL__ << _T("DBWorker Eraser Init Failed"));
			__asm int 3;
			return false;
		}

		pkNullWorker->VActivate(1);

		auto ret = m_kContDBWorker.insert(std::make_pair(NULL_DB_INDEX, pkNullWorker));
		if(!ret.second)
		{
			SAFE_DELETE(pkNullWorker);
			CEL_LOG(BM::LOG_LV0, __FL__ << _T("DBWorker Null Worker Init Failed"));
			__asm int 3;
			return false;
		}
	}
	return true;
}

void CCoreCenter::Close()
{	//! m_pDBWorker-> 지우기전에 멈춤 시켜야 된다.
	//! m_pTimer-> 지우기전에 멈춤 시켜야 된다.
	
	if(!m_bIsClose)
	{
		m_bIsClose = true;
	}
	else
	{
		return;
	}
	
	if( m_pCoreRegister){ if(!m_pCoreRegister->VDeactivate()){assert(NULL);}}
	if( m_pCoreConnector){ if(!m_pCoreConnector->VDeactivate()){assert(NULL);}}

	CONT_TIMER::iterator timer_itor = m_kContTimer.begin();
	while(timer_itor != m_kContTimer.end())
	{
		(*timer_itor).second->VDeactivate();
		++timer_itor;
	}

	CONT_WORKER::iterator con_iter = m_kContWorker.begin();
	while(m_kContWorker.end() !=  con_iter)
	{
		(*con_iter).second->VClose();
//		SAFE_DELETE((*con_iter).second );
		++con_iter;
	}

	CONT_DB_DISPATCHER_WORKER::iterator db_iter = m_kContDBWorker.begin();
	while (m_kContDBWorker.end() != db_iter)
	{
		(*db_iter).second->VDeactivate();
		++db_iter;
	}

	ACE_Proactor* pProactor = ACE_Proactor::instance();
	pProactor->proactor_end_event_loop();

	int const iRet = ACE_Thread_Manager::instance()->wait_task( this );
//	{
//	}
	ACE_Proactor::close_singleton();
//	기다리기.
//	wait ... 
//	pProactor->end_event_loop(
}

E_CORE_RETURN_VALUE CCoreCenter::RegistAccpetor(INIT_CORE_DESC const& rInit, BM::GUID &rOutGuid)
{//패킷을 받아서 Acceptor 에 등록 시키는거라면 문제가 있다는것이지.
	IG_LOG LockCheck( m_CoreLog, fm );

	BM::CAutoMutex kLock(m_core_mutex, true);

	E_CORE_RETURN_VALUE eCRV = CRV_NONE;

	CWorker_Base *pAcceptor = NULL;
	switch( rInit.ServiceHandlerType() )
	{
	case SHT_NC:
		{
			pAcceptor = new CEL_NC_ACCEPTOR(rInit);
		}break;
	case SHT_SERVER:
		{
			pAcceptor = new CEL_ACCEPTOR_SERVER(rInit);
		}break;
	case SHT_FILE_SERVER:
		{
			pAcceptor = new CEL_ACCEPTOR_FILE_SERVER(rInit);
		}break;
	case SHT_GF:
		{
			pAcceptor = new CEL_ACCEPTOR_GF(rInit);
		}break;
	case SHT_JAPAN:
		{
			pAcceptor = new CEL_ACCEPTOR_JAPAN(rInit);
		}break;
	case SHT_NOHEADER:
		{
			pAcceptor = new CEL_ACCEPTOR_NOHEADER(rInit);
		}break;
	case SHT_GRAVITY:
		{
			pAcceptor = new CEL_ACCEPTOR_GRAVITY(rInit);
		}break;
	default:
		{
			pAcceptor = new CEL_ACCEPTOR(rInit);
		}break;
	}

	if(!rInit.IsCorrect() )
	{
		eCRV = CRV_INCORRECT_INIT_INFO;	goto __FAILED;
	}

	if( !pAcceptor )
	{
		eCRV = CRV_ALLOC_FAILED;	goto __FAILED;
	}

	if( !rOutGuid.Generate() )
	{
		eCRV = CRV_CREATE_GUID_FAILED;	goto __FAILED;
	}
	
	if(!pAcceptor->VInit(rOutGuid, rInit.kBindAddr, rInit.kNATAddr, rInit.bIsAllow) )
	{
		eCRV = CRV_ACCEPTOR_INIT_FAILED;	goto __FAILED;
	}
	
	auto ret = m_kContWorker.insert( CONT_WORKER::value_type( rOutGuid, pAcceptor) );
	if(! ret.second )
	{
		eCRV = CRV_REGIST_FAILED;	goto __FAILED;
	}

	CEL_LOG( BM::LOG_LV6, L"Regist Acceptor Success" << rInit.kBindAddr.ToString() << _T(" ") << rOutGuid );
	return CRV_SUCCESS;
__FAILED:
	{
		CEL_LOG( BM::LOG_LV0, L"Regist Acceptor Failed Cause:" << (int)eCRV<< _T(" ") << rInit.kBindAddr.ToString() << _T(" ") << rOutGuid );
		SAFE_DELETE( pAcceptor );
		return eCRV;
	}
}

E_CORE_RETURN_VALUE CCoreCenter::RegistConnector(INIT_CORE_DESC const& rInit, BM::GUID &rOutGuid)
{
	IG_LOG LockCheck( m_CoreLog, fm );
	BM::CAutoMutex kLock(m_core_mutex, true);
	
	E_CORE_RETURN_VALUE eCRV = CRV_NONE;

	CWorker_Base *pConnector = NULL;
	switch(rInit.ServiceHandlerType())
	{
	case SHT_NC:
		{
			pConnector = new CEL_CONNECTOR_NC(rInit);
		}break;
	case SHT_SERVER:
		{
			pConnector = new CEL_CONNECTOR_SERVER(rInit);
		}break;
	case SHT_FILE_SERVER:
		{
			pConnector = new CEL_CONNECTOR_FILE_SERVER(rInit);
		}break;
	case SHT_GF:
		{
			pConnector = new CEL_CONNECTOR_GF(rInit);
		}break;
	case SHT_JAPAN:
		{
			pConnector = new CEL_CONNECTOR_JAPAN(rInit);
		}break;
	case SHT_NOHEADER:
		{
			pConnector = new CEL_CONNECTOR_NOHEADER(rInit);
		}break;
	case SHT_GRAVITY:
		{
			pConnector = new CEL_CONNECTOR_GRAVITY(rInit);
		}break;
	default:
		{
			pConnector = new CEL_CONNECTOR(rInit);
		}break;
	}

	if(!rInit.IsCorrect() )
	{
		eCRV = CRV_INCORRECT_INIT_INFO;	goto __FAILED;
	}

	if( !pConnector )
	{
		eCRV = CRV_ALLOC_FAILED;	goto __FAILED;
	}

	if( !rOutGuid.Generate() )
	{
		eCRV = CRV_CREATE_GUID_FAILED;	goto __FAILED;
	}
	
	if(!pConnector->VInit(rOutGuid) )
	{
		eCRV = CRV_ACCEPTOR_INIT_FAILED;	goto __FAILED;
	}
	
	auto ret = m_kContWorker.insert( CONT_WORKER::value_type(rOutGuid, pConnector) );

	if( !ret.second )
	{
		eCRV = CRV_REGIST_FAILED;	goto __FAILED;
	}

	CEL_LOG( BM::LOG_LV6, L"Regist Connector Success " << rOutGuid);
	return CRV_SUCCESS;
__FAILED:
	{
		CEL_LOG( BM::LOG_LV1, L"Regist Connector Failed Cause:" << (int)eCRV << _T(" ") << rOutGuid);
		SAFE_DELETE( pConnector );
		return eCRV;
	}
}

E_CORE_RETURN_VALUE CCoreCenter::RegistTimerFunc(REGIST_TIMER_DESC const& rInit, BM::GUID &rOutGuid)
{
	IG_LOG LockCheck( m_CoreLog, fm );
	BM::CAutoMutex kLock(m_core_mutex, true);

	bool bIsNewTimer = false;//새로 만든거냐.
	
	BM::GUID kTimerGuid = rInit.kTimerGuid;
	if(kTimerGuid == BM::GUID::NullData())
	{//NULL 은 타이머 따로 쓰라는 것
		kTimerGuid.Generate();	
	}

	CONT_TIMER::iterator timer_itor = m_kContTimer.find(kTimerGuid);//기존 타이머를 찾고

	CTimer_Base *pkTimer = NULL;
	if(timer_itor != m_kContTimer.end())
	{
		pkTimer = (*timer_itor).second;
	}
	else
	{//없으면 만들고
		bIsNewTimer = true;
		pkTimer = new CTimer(rInit.dwThreadCount);
	}

	if( 0 != pkTimer->Regist( rInit.pFunc, rInit.dwUserData, rInit.dwInterval ) )//등록
	{
		if(bIsNewTimer)
		{
			auto ret = m_kContTimer.insert(std::make_pair(kTimerGuid, pkTimer));
			if(!ret.second)
			{
				goto __SAFE_RETURN;
			}
		}

		if(rInit.bIsImmidiate)
		{
			pkTimer->SvcStart();
		}
		return CRV_SUCCESS;	
	}
	
__SAFE_RETURN:
	{
		if(bIsNewTimer)
		{
			SAFE_DELETE(pkTimer);
		}
	}

	return CRV_REGIST_TIMER_FAILED;
}

bool CCoreCenter::SvcStart()
{
	static bool bIsCalled = false;

	if( bIsCalled ){return false;}//두번 부를 수 없도록.

	bIsCalled = true;

	IG_LOG LockCheck( m_CoreLog, fm );
	BM::CAutoMutex kLock(m_core_mutex, true);

	CONT_TIMER::iterator timer_itor = m_kContTimer.begin();
	while( m_kContTimer.end() != timer_itor)
	{
		(*timer_itor).second->SvcStart();
		++timer_itor;
	}	
	
	BM::CAutoMutex kDBLock(m_db_mutex);
	CONT_DB_DISPATCHER_WORKER::iterator db_itor = m_kContDBWorker.begin();
	while( m_kContDBWorker.end() != db_itor)
	{
		(*db_itor).second->VActivate();
		++db_itor;
	}	

	SvcStartProactor();

	return true;
}

void CCoreCenter::SvcStartProactor()
{
	static bool bIsCalled = false;

	if( bIsCalled ){return;}//두번 부를 수 없도록.

	bIsCalled = true;
	
	if(-1 == this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, m_kInit.dwProactorThreadCount, 0, THREAD_BASE_PRIORITY_MAX))//THREAD_BASE_PRIORITY_MAX
	{
		__asm int 3;
		return;
	}
	return;
}

void CCoreCenter::InitEnv()
{
	WSADATA wsd = {0,};
	if( 0 != ::WSAStartup(WINSOCK_VERSION, &wsd) )
	{
	}

	::CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
}
	
void CCoreCenter::ClearEnv()
{
//	if( 0 != ::WSACleanup() )
//	{
//	}
	::CoUninitialize();
}

size_t CCoreCenter::GetQueryCount(int const iDBIndex)const
{
	BM::CAutoMutex kLock(m_db_mutex);
	CONT_DB_DISPATCHER_WORKER::const_iterator db_itor = m_kContDBWorker.find(iDBIndex);

	if(db_itor != m_kContDBWorker.end())
	{
		return (*db_itor).second->GetQueryCount();
	}
	return 0;
}

HRESULT CCoreCenter::PushQuery( DB_QUERY &rkQuery, bool const bIsImmidiate)
{
	DB_QUERY_TRAN kTmpQuery;
	kTmpQuery.QueryOwner(rkQuery.QueryOwner());
	kTmpQuery.DBIndex(rkQuery.DBIndex());
	kTmpQuery.QueryType(rkQuery.QueryType());
	kTmpQuery.push_back(rkQuery);
	return PushQuery( kTmpQuery, bIsImmidiate);
}

HRESULT CCoreCenter::PushQuery( DB_QUERY_TRAN &rkQuery, bool const bIsImmidiate)
{
	rkQuery.BuildQueryTarget();//쿼리 타겟을 반드시!.

	BM::CAutoMutex kLock(m_db_mutex);

	CONT_DB_DISPATCHER_WORKER::iterator db_itor = m_kContDBWorker.find(rkQuery.DBIndex());

	if(db_itor != m_kContDBWorker.end())
	{
		if( bIsImmidiate )
		{
			CDBWorker_Dispatcher* pkDBDispatcher = dynamic_cast<CDBWorker_Dispatcher*>((*db_itor).second);
			if ( pkDBDispatcher )
			{
				HRESULT const hRet = pkDBDispatcher->ImmidiateProcess( rkQuery );
				return hRet;
			}
			return E_FAIL;
		}
		else
		{
			HRESULT const hRet = (*db_itor).second->VPush(rkQuery);
				
			if(hRet != S_OK)
			{//처리 실패.
				if(rkQuery.size() == 1)
				{
					CEL::DB_RESULT kRet(rkQuery.front());
					kRet.eRet = DR_ERR_PUSH;
					if (!kRet.QueryResultCallback())
						(*db_itor).second->m_kInitInfo.OnDBExecute( kRet );//.
					else
						kRet.QueryResultCallback()(kRet);
				}
				else
				{
					DB_RESULT_TRAN kRetTran(rkQuery);
					for(DB_QUERY_TRAN::const_iterator citer = rkQuery.begin();citer != rkQuery.end();++citer)
					{
						CEL::DB_RESULT kRet((*citer));
						kRet.eRet = DR_ERR_PUSH;
						kRetTran.push_back(kRet);
					}
					(*db_itor).second->m_kInitInfo.OnDBExecuteTran( kRetTran );//큐에 쌓지 못했다.
				}
			}
			return hRet;

		}
	}
	return E_FAIL;
}

HRESULT CCoreCenter::ExecDBResult( DB_RESULT &rQuery )
{
	CONT_DB_DISPATCHER_WORKER::iterator db_itor = m_kContDBWorker.find(rQuery.DBIndex());
	if (db_itor == m_kContDBWorker.end())
	{
		return E_FAIL;
	}
	return (*db_itor).second->m_kInitInfo.OnDBExecute(rQuery);
}

HRESULT CCoreCenter::ClearQueryResult( DB_RESULT &rQuery )
{
	CONT_DB_DISPATCHER_WORKER::iterator db_itor = m_kContDBWorker.find(NULL_DB_INDEX);
	if(db_itor != m_kContDBWorker.end())
	{
		CDBWorker_Eraser *pkWorker = dynamic_cast<CDBWorker_Eraser*>((*db_itor).second);

		if(pkWorker)
		{
			pkWorker->VPush( rQuery );
		}
		return S_OK;
	}
	return E_FAIL;
}

bool CCoreCenter::Connect(ADDR_INFO const& rAddr, SESSION_KEY &kSessionKey, BM::GUID &kConnectObj)
{
	if(m_pCoreConnector)
	{
		if(rAddr.IsCorrect())
		{
			return m_pCoreConnector->Connect(rAddr, kSessionKey, kConnectObj);
		}
//		__asm int 3;
	}
	return false;
}

bool CCoreCenter::ConnectSub(SConnectInfo const& rkObj)
{
	IG_LOG LockCheck( m_CoreLog, fm );
	BM::CAutoMutex kLock(m_core_mutex);

	CONT_WORKER::const_iterator itor = m_kContWorker.find(rkObj.guidConnector);

	while( itor != m_kContWorker.end() ) 
	{
		const CONT_WORKER::mapped_type &rElement = (*itor).second;

		SConnectInfo *pkConnectionInfo = new SConnectInfo;//이게 SessionMgr::OnConnectFail  ServiceHandler::act 에서 지워줘야됨.
		if(pkConnectionInfo)
		{
			*pkConnectionInfo = rkObj;
			rElement->VConnect(rkObj.addr, pkConnectionInfo);
			return true;
		}
	}
	return false;
}

bool CCoreCenter::Send(SESSION_KEY const& kSessionKey,  BM::Stream const& rkPacket)
{
	IG_LOG LockCheck( m_CoreLog, fm );
	BM::CAutoMutex kLock(m_core_mutex);

	CONT_WORKER::const_iterator conn_itor = m_kContWorker.find(kSessionKey.WorkerGuid());

	if( conn_itor != m_kContWorker.end() ) 
	{
		IG_LOG LockCheck( m_CoreLog, fm2 );
		bool bRet = (*conn_itor).second->VSend(kSessionKey.SessionGuid(), rkPacket);
		return bRet;
	}
	return false;
}

HRESULT CCoreCenter::GetDBInitInfo(std::list<INIT_DB_DESC> &kContOut)const
{
	BM::CAutoMutex kLock(m_db_mutex);

	if(!m_kContDBWorker.size())
	{
		return E_FAIL;
	}

	CONT_DB_DISPATCHER_WORKER::const_iterator db_itor = m_kContDBWorker.begin();
	while(m_kContDBWorker.end() != db_itor)
	{
		if((*db_itor).first != NULL_DB_INDEX)
		{
			kContOut.push_back((*db_itor).second->InitInfo());
		}
		++db_itor;
	}
	return S_OK; 
}

bool CCoreCenter::IsExistAcceptor(ADDR_INFO const& rUseAddr)const
{
	BM::CAutoMutex kLock(m_core_mutex);

	CONT_WORKER::const_iterator con_iter = m_kContWorker.begin();
	while(m_kContWorker.end() !=  con_iter)
	{
		if((*con_iter).second->m_kInitInfo.kBindAddr == rUseAddr)
		{
			return true;
		}
		++con_iter;
	}
	return false;
}

bool CCoreCenter::IsExistDBWorker(int const iDBIndex)const
{
	BM::CAutoMutex kLock(m_core_mutex);

	CONT_DB_DISPATCHER_WORKER::const_iterator db_worker_itor = m_kContDBWorker.find(iDBIndex);

	if(db_worker_itor != m_kContDBWorker.end())
	{
		return true;
	}
	return false;
}

BM::GUID CCoreCenter::Regist(ERegistType const eRT, tagRegist_Base const *pInitInfo)
{
	IG_LOG LockCheck( m_CoreLog, fm );
	if(m_pCoreRegister)
	{
		return m_pCoreRegister->Regist(eRT, pInitInfo);
	}
	return BM::GUID::NullData();
}

bool CCoreCenter::RegistSub(SRegistInfo * const pObj)
{
	ERegistType const eRT =  pObj->eType;
	BM::GUID const& guidOrder = pObj->guidOrder;
	BM::Stream &kPacket = pObj->kPacket;

	SRegistResult RR;
	RR.guidOrder = guidOrder;
	RR.eType = eRT;
	RR.iIdentityValue = 0;

	switch( eRT )
	{
	case RT_ACCEPTOR:
		{
			INIT_CORE_DESC kInit;
			kInit.ReadFromPacket(kPacket);
			
			RR.iRet = RegistAccpetor(kInit, RR.guidObj);
			RR.iIdentityValue = kInit.IdentityValue();
		}break;
	case RT_CONNECTOR:
		{
			INIT_CORE_DESC kInit;
			kInit.ReadFromPacket(kPacket);
			
			RR.iRet = RegistConnector(kInit, RR.guidObj);
			RR.iIdentityValue = kInit.IdentityValue();
		}break;
	case RT_DB_WORKER:
		{
			INIT_DB_DESC kInit;
			kInit.ReadFromPacket(kPacket);
			
			RR.iRet = RegistDBWorker(kInit, RR.guidObj);
			RR.iIdentityValue = static_cast<int>(kInit.kDBInfo.DBIndex());
		}break;
	case RT_TIMER_FUNC:
		{
			REGIST_TIMER_DESC kInit;
			kInit.ReadFromPacket(kPacket);
			
			RR.iRet = RegistTimerFunc(kInit, RR.guidObj);
		}break;
	case RT_FINAL_SIGNAL:
		{
			INIT_FINAL_SIGNAL kInit;
			kInit.ReadFromPacket(kPacket);

			RR.iRet = CEL::CRV_SUCCESS;
			RR.iIdentityValue = kInit.kIdentity;
		}break;
	}

	m_kInit.pOnRegist(RR);
	return true;
}

// DO Disconnect !!!
// [PARAMETER]
//	bAccepted : accepted connection ?
//	rkParentGuid : == Guid of Connector or Acceptor
//	rkChildGuid = Guid of session
bool CCoreCenter::Terminate(SESSION_KEY const& kSessionKey)
{
	CONT_WORKER::iterator itor = m_kContWorker.find(kSessionKey.WorkerGuid());
	if (m_kContWorker.end() == itor)
	{
//		CEL_LOG( BM::LOG_LV7, L"Disconnecting failure Guid[%s]", rkChildGuid.str().c_str() );
		return false;
	}
	return itor->second->VTerminate(kSessionKey.SessionGuid());
}

HRESULT CCoreCenter::Statistics(SESSION_KEY const& kSessionKey,  SSessionStatistics &rkOutResult)//소유자와 child 의 키.
{
	CONT_WORKER::iterator itor = m_kContWorker.find(kSessionKey.WorkerGuid());
	if (m_kContWorker.end() == itor)
	{
		return E_FAIL;
	}
	return itor->second->Statistics(kSessionKey.SessionGuid(), rkOutResult);
}

E_CORE_RETURN_VALUE CCoreCenter::RegistDBWorker(INIT_DB_DESC const& rInit, BM::GUID &rOutGuid)//! 등록된 결과 GUID를 반환
{
	IG_LOG LockCheck( m_CoreLog, fm );
	BM::CAutoMutex kLock(m_db_mutex, true);

	if(0 >= rInit.kDBInfo.DBIndex())
	{
		return CRV_INCORRECT_DB_INDEX;
	}

	if(!rInit.dwWorkerCount)
	{
		return CRV_NO_DB_WORKER_COUNT;
	}

	if(!rInit.OnDBExecute)
	{
		return CRV_NO_DB_EXECUTE_FUNC;
	}

	if(!rInit.OnDBExecuteTran)
	{
		return CRV_NO_DB_EXECUTE_TRAN_FUNC;
	}
{
	CDBWorker_Base *pkWorker = new CDBWorker_Dispatcher( rInit );

	if(!pkWorker)
	{
		SAFE_DELETE( pkWorker );
		return CRV_NONE;
	}

	if(	!pkWorker->VInit())
	{
		return CRV_NONE;
	}

	if(rInit.bIsImmidiate)
	{
		pkWorker->VActivate();
	}

	auto ret = m_kContDBWorker.insert(std::make_pair(rInit.kDBInfo.DBIndex(), pkWorker));
	if(!ret.second)
	{
		SAFE_DELETE(pkWorker);
		return CRV_ALREADY_USE_DB_INDEX;
	}
}
	return CRV_SUCCESS;
}

int CCoreCenter::svc(void)
{//! Run 전에 PacketHandler는 모두 등록이 되어 있어야 한다.//! 	MAIN_LOG( LOG_LV1, "Try to Connector Launch.\n"));
	ACE_Proactor* pProactor = ACE_Proactor::instance();//! proactor 를 다른 참조 포인터에 셋팅 해 두고 쓴다.

//	ACE_Time_Value tv = ACE_Time_Value(1, 100);
	while(!pProactor->proactor_event_loop_done())
	{
		pProactor->handle_events(ACE_Time_Value(1, 100));
	}
	return 1;
}

void CCoreCenter::DisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const
{
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, _T("\n===== CoreCenter State Start=====\n")));

	IG_LOG LockCheck( m_CoreLog, fm );
	BM::CAutoMutex kLock(m_core_mutex);
	BM::CAutoMutex kDBLock(m_db_mutex);

	CONT_WORKER::const_iterator con_itor = m_kContWorker.begin();
	while( m_kContWorker.end() != con_itor )
	{
		(*con_itor).second->VDisplayState(rkLogWorker, iLogType);
		++con_itor;
	}	
	
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring) _T("Total Worker[") << m_kContWorker.size()<<  _T("]")));

	CONT_TIMER::const_iterator timer_itor = m_kContTimer.begin();
	while( m_kContTimer.end() != timer_itor)
	{
		(*timer_itor).second->VDisplayState(rkLogWorker, iLogType);
		++timer_itor;
	}

	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring)_T("Total Timer[") << m_kContTimer.size() <<  _T("]")));
	
	CONT_DB_DISPATCHER_WORKER::const_iterator db_itor = m_kContDBWorker.begin();
	while( m_kContDBWorker.end() != db_itor)
	{
		(*db_itor).second->VDisplayState(rkLogWorker, iLogType);
		++db_itor;
	}	

	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring) _T("Total DBWoker[") << m_kContDBWorker.size() <<  _T("]")));

	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, _T("\n===== CoreCenter State End =====\n")));
}

bool CCoreCenter::VReadyToStop(PgLogWorker_Base &rkLogWorker, DWORD const iLogType) const
{
	BM::CAutoMutex kLock(m_core_mutex);
	BM::CAutoMutex kDBLock(m_db_mutex);

	CONT_DB_DISPATCHER_WORKER::const_iterator db_itor = m_kContDBWorker.begin();
	while( m_kContDBWorker.end() != db_itor)
	{
		if (false == (*db_itor).second->VReadyToStop(rkLogWorker, iLogType))
		{
			return false;
		}
		++db_itor;
	}	
	return true;
}

void CCoreCenter::DisplayStateDBRecord(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const
{
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, _T("\n===== CoreCenter State Start=====\n")));
	
	CONT_DB_DISPATCHER_WORKER::const_iterator db_itor = m_kContDBWorker.begin();
	while( m_kContDBWorker.end() != db_itor)
	{
		(*db_itor).second->VDisplayStateDBRecord(rkLogWorker, iLogType);
		++db_itor;
	}	

	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring) _T("Total DBWoker[") << m_kContDBWorker.size() <<  _T("]")));

	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, _T("\n===== CoreCenter State End =====\n")));
}