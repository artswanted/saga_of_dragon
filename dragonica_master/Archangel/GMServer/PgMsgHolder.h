#ifndef GM_GMSERVER_PGMSGHOLDER_H
#define GM_GMSERVER_PGMSGHOLDER_H

#include "ace/Task_ex_t.h"
#include "ace/Message_Queue_t.h"

class PgMsgHolder;
//class PgMsgHolder;

class PgMsgQue
{
public:
	typedef BM::Stream MsgType;
	friend class PgMsgHolder;
public:
	PgMsgQue(std::wstring const &kAccountID, std::wstring const &strFunctionName)
		:	m_kAccountID(kAccountID)
		,	m_kFunctionName(strFunctionName)
	{
		m_kMsgGuid.Generate();
		m_kMsgQue.open();
		m_pkRetrunValue = NULL;
	}
	virtual ~PgMsgQue(){}

public:
	bool WaitMsg(DWORD const dwMSec, MsgType &kOutMsg)//밀리세컨 단위
	{
//		DWORD const dwNow = ::timeGetTime();
		
		DWORD const sec = dwMSec/1000;
		DWORD const micro_sec = (dwMSec%1000)*1000;

		MsgType *pkOutMsg  = NULL;

		ACE_Time_Value tv(sec, micro_sec);	tv += ACE_OS::gettimeofday();
		if( -1 != m_kMsgQue.dequeue( pkOutMsg, &tv ) )	
		{
			kOutMsg = *pkOutMsg;
			SAFE_DELETE(pkOutMsg);
//			DWORD const dwEnd = ::timeGetTime();
			return true;
		}
//		DWORD const dwEnd = ::timeGetTime();
		return false;
	}

	BM::GUID const& MsgGuid()const{return m_kMsgGuid;}
	std::wstring const& AccountID()const{return m_kAccountID;}
	std::wstring const& FunctionName()const{return m_kFunctionName;}
	
	void SetRetrunPtr(int * pkValue){m_pkRetrunValue = pkValue;}
	void SetRetrunValue(int const iValue)
	{
		if(m_pkRetrunValue)
		{
			*m_pkRetrunValue = iValue;
		}
	}
	
protected:	
	bool InputMsg(MsgType const &rkInMsg)//밀리세컨 단위
	{
		MsgType* pkMsg = new MsgType;
		if(pkMsg)
		{
			*pkMsg = rkInMsg;
			m_kMsgQue.enqueue(pkMsg);
			return true;
		}
		return false;		
	}

protected:
//	BM::TObjectPool<MsgType> m_kMsgPool;

	typedef ACE_Message_Queue_Ex< MsgType, ACE_MT_SYNCH > MsgQue;
	MsgQue m_kMsgQue;
	BM::GUID m_kMsgGuid;
	std::wstring const m_kAccountID;
	std::wstring const m_kFunctionName;
	int *m_pkRetrunValue;//GetPoint 함수를 위한.
};

class PgMsgHolder
{
protected:
	typedef std::map< BM::GUID, PgMsgQue* > CONT_MSG_QUE;
public:
	PgMsgHolder(){}
	virtual ~PgMsgHolder(){}
	
	bool MsgReady(BM::GUID const& rkOutGuid, PgMsgQue &kOutMsgQue)
	{
		BM::CAutoMutex kLock(m_kMutex);
		auto ret = m_kMsgHash.insert(std::make_pair(rkOutGuid, &kOutMsgQue));
		return (ret.second);
	}
	
	HRESULT SignalMsg(BM::GUID const& rkGuid, PgMsgQue::MsgType &rkMsg)
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_MSG_QUE::iterator itor =  m_kMsgHash.find(rkGuid);
		if(itor != m_kMsgHash.end())
		{
			(*itor).second->InputMsg(rkMsg);
			m_kMsgHash.erase(rkGuid);
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT MsgOwner(BM::GUID const& rkGuid, std::wstring &kOutStr)
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_MSG_QUE::iterator itor =  m_kMsgHash.find(rkGuid);
		if(itor != m_kMsgHash.end())
		{
			kOutStr = (*itor).second->AccountID();
			return S_OK;
		}
		return E_FAIL;
	}

protected:
	CONT_MSG_QUE m_kMsgHash;//-> Lock 을 못잡음.
	Loki::Mutex m_kMutex;
};

#define g_kRpcHolder SINGLETON_STATIC(PgMsgHolder)

/*
extern PgMsgHolder kMsgHolder;
extern BM::GUID g_kGuid;


	PgMsgHolder kMsgHolder;
	BM::GUID kGuid;
	PgMsgQue *pkMsgQue = NULL;
	if(kMsgHolder.MsgReady(kGuid,pkMsgQue) )//메세지 받을 상태로 전이
	{
		PgMsgQue::MsgType kMsg;
		pkMsgQue->WaitMsg(1000, kMsg);
	}
	
	
	{//어디선가 밀어주면 됨.
		BM::GUID kGuid;
		PgMsgQue::MsgType kMsg;
		kMsgHolder.SignalMsg(kGuid, kMsg);
	}
*/

#endif // GM_GMSERVER_PGMSGHOLDER_H