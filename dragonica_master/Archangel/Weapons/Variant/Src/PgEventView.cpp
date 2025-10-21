#include "stdafx.h"
#include <tchar.h>
#include "PgEventView.h"
#include "DefAbilType.h"
#include "PgPlayer.h"
#include "tabledatamanager.h"

PgEventAbil::PgEventAbil(void)
{

}

PgEventAbil& PgEventAbil::operator = ( PgEventAbil const &rhs )
{
	m_kAbil = rhs.m_kAbil;
	return *this;
}

int PgEventAbil::GetAbil( CAbilObject::DYN_ABIL::key_type const wAbilType )const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kAbil.GetAbil( wAbilType );
}

__int64 PgEventAbil::GetAbil64( CAbilObject::DYN_ABIL64::key_type const wAbilType )const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kAbil.GetAbil64(wAbilType);
}


bool PgEventAbil::AddAbil( CAbilObject::DYN_ABIL::key_type const wAbilType, CAbilObject::DYN_ABIL::mapped_type const iValue, bool const bZeroAutoDel = false )
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kAbil.AddAbil( wAbilType, iValue, bZeroAutoDel );
}

bool PgEventAbil::AddAbil64( CAbilObject::DYN_ABIL64::key_type const wAbilType, CAbilObject::DYN_ABIL64::mapped_type const i64Value, bool const bZeroAutoDel = false )
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kAbil.AddAbil64( wAbilType, i64Value, bZeroAutoDel );
}

bool PgEventAbil::SetAbil( CAbilObject::DYN_ABIL::key_type const wAbilType, CAbilObject::DYN_ABIL::mapped_type const iValue )
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kAbil.SetAbil( wAbilType, iValue );
}

bool PgEventAbil::SetAbil64( CAbilObject::DYN_ABIL64::key_type const wAbilType, CAbilObject::DYN_ABIL64::mapped_type const i64Value )
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kAbil.SetAbil64( wAbilType, i64Value );
}

void PgEventAbil::Clear(void)
{
	BM::CAutoMutex kLock( m_kMutex );
	m_kAbil.Clear();
}

bool PgEventAbil::IsEmpty()const
{
	BM::CAutoMutex kLock( m_kMutex );
	return m_kAbil.IsEmpty();
}

PgEventView::PgEventView(void)
:m_kCallbackStart(NULL),
m_kCallbackEnd(NULL),
m_kCallbackProcessEvent(NULL)
{
}

PgEventView::~PgEventView(void)
{
}

bool PgEventView::SetGameTime(LPSYSTEMTIME pkLocalTime, LPSYSTEMTIME pkGameTime)
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kGameTime.Begin(pkLocalTime, pkGameTime);
}

void PgEventView::SetGameTimeSpeed(float fSpeed)
{
	m_kGameTime.GameTimeSpeed(fSpeed);
}

bool PgEventView::ReadConfigFile(LPCTSTR lpszFileName)
{
	BM::CAutoMutex kLock(m_kMutex);

	float fSpeed = 1.0f;

	SYSTEMTIME kLocalTime;
	::GetLocalTime(&kLocalTime);

	SYSTEMTIME kGameTime = kLocalTime;

	if ( lpszFileName )
	{
		TCHAR chValue[100];
		DWORD dwLen = ::GetPrivateProfileString(_T("TIME"), _T("GameTime"), _T(""), chValue, 100, lpszFileName);
		if (dwLen > 0)
		{
			ZeroMemory(&kGameTime, sizeof(SYSTEMTIME));
			_stscanf_s(chValue, _T("%4hd-%2hd-%2hd %2hd:%2hd:%2hd"), &(kGameTime.wYear), &(kGameTime.wMonth), &(kGameTime.wDay),
				&(kGameTime.wHour), &(kGameTime.wMinute), &(kGameTime.wSecond));
		}
		else
		{
			::memcpy(&kGameTime ,&kLocalTime, sizeof(SYSTEMTIME));
		}
		dwLen = ::GetPrivateProfileString(_T("TIME"), _T("TimeSpeed"), _T(""), chValue, 100, lpszFileName);

		if (dwLen > 0)
		{
			_stscanf_s(chValue, _T("%f"), &fSpeed);
		}
	}

	SetGameTime( &kLocalTime, &kGameTime );
	SetGameTimeSpeed(fSpeed);
	return true;
}

HRESULT PgEventView::ReadFromPacket(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	HRESULT hReturn = m_kGameTime.ReadFromPacket(rkPacket);
	
	bool bOnlyGameTime;
	rkPacket.Pop(bOnlyGameTime);

	if (false == bOnlyGameTime)
	{
		m_kVariableCont.ReadFromPacket(rkPacket);
	}

	return hReturn;
}

void PgEventView::WriteToPacket(bool const bSendOnlyGameTime, BM::Stream& rkPacket)const
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kGameTime.WriteToPacket(rkPacket);
	rkPacket.Push(bSendOnlyGameTime);

	if (false == bSendOnlyGameTime)
	{
		m_kVariableCont.WriteToPacket(rkPacket);
	}
}

DWORD PgEventView::GetServerElapsedTime(void)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kGameTime.GetElapsedTime();
}

unsigned __int64 PgEventView::GetServerElapsedTime64(void)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kGameTime.GetElapsedTime64();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void PgEventView::ProcessEventTerminate(CONT_EVENT_STATE::mapped_type &element)
{
	BM::CAutoMutex kLock(m_kMutex);

	if(m_kCallbackEnd)
	{
		m_kCallbackEnd->DoAction(element);
	}

	const TBL_EVENT &kTbl = element.m_kTable;
	switch(kTbl.iEventType)
	{
	case ET_EXP_BONUS_RATE:
	case ET_MONEY_DROP_RATE:
	case ET_MONEY_GIVE_RATE:
	case ET_ITEM_DROP_RATE:
	case ET_MONSTER_MAX_HP_RATE:
	case ET_UNIT_EVENT:
	case ET_CONNECT_TIME_RECORD:
	case ET_NOTIFY_MSG:
		{
		}break;
	}
}

void PgEventView::ProcessEvent_Sub(CONT_EVENT_STATE::mapped_type &element, BM::DBTIMESTAMP_EX const &kNowTime)
{
	BM::CAutoMutex kLock(m_kMutex);

	if(m_kCallbackStart)
	{
		m_kCallbackStart->DoAction(element);
	}

	TBL_EVENT const &kTbl = element.m_kTable;
	switch(kTbl.iEventType)
	{
	case ET_EVENT_ITEM_CONTAINER:
		{
			if ( 0 == kTbl.aIntData[1] )
			{
				AddAbil(AT_EVENT_ITEM_CONTAINER, kTbl.aIntData[0]);
			}
		}break;
	case ET_EXP_BONUS_RATE:
		{
			if ( 0 == kTbl.aIntData[1] )
			{
				AddAbil(AT_ADD_EXP_PER, kTbl.aIntData[0]);
			}
		}break;
	case ET_MONEY_DROP_RATE:
		{
			if ( 0 == kTbl.aIntData[1] )
			{
				AddAbil(AT_ADD_MONEY_RATE, kTbl.aIntData[0]);
			}
		}break;
	case ET_MONEY_GIVE_RATE:
		{
			if ( 0 == kTbl.aIntData[1] )
			{
				AddAbil(AT_ADD_MONEY_PER, kTbl.aIntData[0]);
			}
		}break;
	case ET_ITEM_DROP_RATE:
		{
			if ( 0 == kTbl.aIntData[1] )
			{
				AddAbil(AT_ADD_ITEMDROP_RATE, kTbl.aIntData[0]); // 미션/인던에만 작동한다.
			}
		}break;
	case ET_MONSTER_MAX_HP_RATE:
		{
		}break;
	case ET_UNIT_EVENT:
		{
			//... UNIT_EVENT..
		}break;
	case ET_CONNECT_TIME_RECORD:
		{
//			m_kAbil.AddAbil(AT_GOALPOS_SET_TIME, 1);//시간 기록하겠다는 어빌.
//			m_kAbil.AddAbil(AT_1ST_AREA_TYPE, (long)BM::tagPackedTime(kTbl.timeStartDate));//시간 기록하겠다는 어빌.
//			m_kAbil.AddAbil(AT_2ND_AREA_TYPE, (long)BM::tagPackedTime(kTbl.timeEndDate));//시간 기록하겠다는 어빌.
		}break;
	case ET_CHANGE_LEVEL:
		{
			m_kChangeLevel[kTbl.aIntData[0]] = kTbl.iEventNo;
		}break;
	case ET_CHANGE_CLASS:
		{
			m_kChangeClass[kTbl.aIntData[0]] = kTbl.iEventNo;
		}break;
	case ET_NEW_CHARACTER:
		{
			AddAbil(AT_NEWCHARACTER, kTbl.iEventNo);
		}break;
	case ET_CONNECT_COUNT:
		{
            if( kTbl.aIntData[0] == AT_EVENTFUNC_CONNECT_EVENT11 )
            {
                SetAbil(kTbl.aIntData[0], kTbl.iEventNo);
            }
            else
            {
			    AddAbil(kTbl.aIntData[0], kTbl.iEventNo);
            }
		}break;
	}
//	element.iValue
}

bool PgEventView::ProcessEvent()//notify등등 처리.
{//이벤트는 그냥 덮어 써라?
//	어빌.
	BM::CAutoMutex kLock(m_kMutex);

	PgEventAbil::Clear();
	m_kChangeLevel.clear();
	m_kChangeClass.clear();

	BM::DBTIMESTAMP_EX const kTime(BM::PgPackedTime::LocalTime());

{
	CONT_EVENT_STATE::iterator state_itor = m_kContEventState.begin();
	while(state_itor != m_kContEventState.end())
	{//없어진 이벤트, 시간 지난 이벤트들 정리.
		if(!(*state_itor).second.IsCorrectTime(kTime))
		{//기간 지남 체크

			CONT_EVENT_STATE::mapped_type &element = (*state_itor).second;

			ProcessEventTerminate(element);

			m_kContEventState.erase(state_itor++);
			continue;
		}
		++state_itor;
	}
}

	CONT_EVENT_STATE::iterator my_itor = m_kContEventState.begin();
	while(my_itor != m_kContEventState.end())
	{
		CONT_EVENT_STATE::mapped_type &element = (*my_itor).second;
		ProcessEvent_Sub(element, kTime);
		++my_itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
	return true;
}

void PgEventView::RecvSync(BM::Stream &kPacket)
{//EventView 에서 ReadFrom 함.
	BM::CAutoMutex kLock(m_kMutex);

	CONT_TBL_EVENT kCont;
	PU::TLoadTable_MM(kPacket, kCont);

	BM::DBTIMESTAMP_EX const kTime(BM::PgPackedTime::LocalTime());

	CONT_TBL_EVENT::iterator itor = kCont.begin();
	while(itor != kCont.end())
	{
		EVENT_STATE kState((*itor).second);

		if(kState.IsCorrectTime(kTime))
		{
			CONT_EVENT_STATE::iterator state_itor = m_kContEventState.find((*itor).first);
			if(state_itor == m_kContEventState.end())
			{
				m_kContEventState.insert(std::make_pair((*itor).first, kState));
			}
			else
			{	
				if((*state_itor).second.m_kTable != (*itor).second)
				{
					ProcessEventTerminate((*state_itor).second);
					ProcessEvent_Sub(kState,kTime);
					m_kContEventState.erase(state_itor);
					m_kContEventState.insert(std::make_pair((*itor).first, kState));
				}
			}
		}
		else
		{
			CONT_EVENT_STATE::iterator state_itor = m_kContEventState.find((*itor).first);
			if(state_itor != m_kContEventState.end())
			{	
				ProcessEventTerminate((*state_itor).second);
				m_kContEventState.erase(state_itor);
			}
		}
		++itor;
	}

	CONT_EVENT_STATE::iterator state_itor = m_kContEventState.begin();

	while(state_itor != m_kContEventState.end())
	{//없어진 이벤트 정리.
		CONT_TBL_EVENT::iterator in_itor = kCont.find((*state_itor).first);
		if(in_itor == kCont.end())
		{
			ProcessEventTerminate((*state_itor).second);
			m_kContEventState.erase(state_itor++);
			continue;
		}
		++state_itor;
	}
}

void PgEventView::CallbackStart(PgAction_Callback * pkCallbackStart)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kCallbackStart = pkCallbackStart;
}

void PgEventView::CallbackEnd(PgAction_Callback * pkCallbackEnd)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kCallbackEnd = pkCallbackEnd;
}

void PgEventView::CallbackProcessEvent(PgAction_Callback * pkCallbackProcessEvent)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kCallbackProcessEvent = pkCallbackProcessEvent;
}

void PgEventView::GetContMonsterGenNfy(CONT_NFY_MONSTERGEN & kOutCont)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_EVENT_STATE::const_iterator my_itor = m_kContEventState.begin();
	while(my_itor != m_kContEventState.end())
	{
		CONT_EVENT_STATE::mapped_type const & element = (*my_itor).second;

		if(!element.kEventMonsterGen.IsNull())
		{
			kOutCont.push_back(element.kEventMonsterGen);
		}

		++my_itor;
	}
}

bool PgEventView::ProcessEvent(E_USER_EVENT_TYPE const kEventType,PgPlayer * pkPlayer)
{
	BM::CAutoMutex kLock(m_kMutex);

	if(NULL == pkPlayer)
	{
		return false;
	}

	switch(kEventType)
	{
	case ET_CHANGE_LEVEL:
		{
			CONT_EVENT_KEY_TABLE::const_iterator iter = m_kChangeLevel.find(pkPlayer->GetAbil(AT_LEVEL));
			if(iter == m_kChangeLevel.end())
			{
				return false;
			}

			CONT_EVENT_STATE::iterator eventiter = m_kContEventState.find((*iter).second);
			if(eventiter == m_kContEventState.end())
			{
				return false;
			}

			if(m_kCallbackProcessEvent)
			{
				m_kCallbackProcessEvent->DoAction((*eventiter).second,pkPlayer);
			}
		}break;
	case ET_CHANGE_CLASS:
		{
			CONT_EVENT_KEY_TABLE::const_iterator iter = m_kChangeClass.find(pkPlayer->GetAbil(AT_CLASS));
			if(iter == m_kChangeClass.end())
			{
				return false;
			}

			CONT_EVENT_STATE::iterator eventiter = m_kContEventState.find((*iter).second);
			if(eventiter == m_kContEventState.end())
			{
				return false;
			}

			if(m_kCallbackProcessEvent)
			{
				m_kCallbackProcessEvent->DoAction((*eventiter).second,pkPlayer);
			}
		}break;
	case ET_NEW_CHARACTER:
		{
			if(pkPlayer->GetTotalPlayTimeSec() > 0)
			{
				return false;
			}

			int const iEventNo = GetAbil(AT_NEWCHARACTER);

			CONT_EVENT_STATE::iterator eventiter = m_kContEventState.find(iEventNo);
			if(eventiter == m_kContEventState.end())
			{
				return false;
			}

			if(m_kCallbackProcessEvent)
			{
				m_kCallbackProcessEvent->DoAction((*eventiter).second,pkPlayer);
			}
		}break;
	case ET_CONNECT_COUNT:
		{
			for(int i = AT_EVENTFUNC_CONNECT_EVENT01;i <= AT_EVENTFUNC_CONNECT_EVENT13;++i)
			{
				CONT_EVENT_STATE::iterator eventiter = m_kContEventState.find(GetAbil(i));
				if(eventiter == m_kContEventState.end())
				{
					continue;
				}
				if(m_kCallbackProcessEvent)
				{
					m_kCallbackProcessEvent->DoAction((*eventiter).second,pkPlayer);
				}
			}
		}break;
	default:
		{
			return false;
		}break;
	}
	return true;
}

HRESULT PgEventView::ProcessManualEvent(int const iEventNo,PgPlayer * pkPlayer)
{
	BM::CAutoMutex kLock(m_kMutex);

	if(NULL == pkPlayer)
	{
		return E_ME_INVALID_ACTION;
	}

	CONT_EVENT_STATE::iterator eventiter = m_kContEventState.find(iEventNo);
	if(eventiter == m_kContEventState.end())
	{
		return E_ME_NOT_FOUND_EVENT;
	}

	TBL_EVENT const & kEvent = (*eventiter).second.m_kTable;

	switch(kEvent.iEventType)
	{
	case ET_PCROOMEVENT_COUPON:
	case ET_PCROOMEVENT_EFFECT:
		{
			CONT_PCROOMIP const * pkDef = NULL;
			g_kTblDataMgr.GetContDef(pkDef);
			if(NULL == pkDef)
			{
				return E_ME_NOT_FOUND_PCROOM;
			}

			CONT_PCROOMIP::const_iterator iter = pkDef->find(pkPlayer->addrRemote().ip.S_un.S_addr);
			if(iter == pkDef->end())
			{
				return E_ME_NOT_FOUND_PCROOM;
			}

			if((*iter).second != kEvent.aGuidData[0])
			{
				return E_ME_NOT_FOUND_PCROOM;
			}

			if(m_kCallbackProcessEvent)
			{
				m_kCallbackProcessEvent->DoAction((*eventiter).second,pkPlayer);
			}
		}break;
	default:
		{
			return E_ME_NOT_MANUAL_EVENT;
		}break;
	}
	return S_OK;
}

void PgEventView::WriteToPacketEventList(BM::Stream & kPacket)const
{
	BM::CAutoMutex kLock(m_kMutex);

	kPacket.Push(m_kContEventState.size());

	for(CONT_EVENT_STATE::const_iterator iter = m_kContEventState.begin();iter != m_kContEventState.end();++iter)
	{
		(*iter).second.m_kTable.WriteToPacket(kPacket);
	}
}