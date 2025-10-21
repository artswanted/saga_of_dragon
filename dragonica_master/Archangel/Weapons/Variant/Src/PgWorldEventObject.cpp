#include "stdafx.h"
#include "PgStringUtil.h"
#include "PgEventView.h"
#include "PgWorldEventObject.h"

//
PgWEClientObjectState::PgWEClientObjectState()
	: m_kID(0), m_kState(0), m_kSyncTime(0), m_kShow(true)
{
}

PgWEClientObjectState::PgWEClientObjectState(PgWEClientObjectState const& rhs)
	: m_kID(rhs.m_kID), m_kState(rhs.m_kState), m_kSyncTime(rhs.m_kSyncTime), m_kShow(rhs.m_kShow)
{
}

PgWEClientObjectState::~PgWEClientObjectState()
{
}

void PgWEClientObjectState::operator =(PgWEClientObjectState const& rhs)
{
	// never try to copy (use Udpate() function)
}

bool PgWEClientObjectState::SetStatus(int const iNewState)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( m_kState != iNewState )
	{
		m_kState = iNewState;
		m_kSyncTime = g_kEventView.GetServerElapsedTime();
		OnStateChange();
		return true;
	}
	return false;
}

bool PgWEClientObjectState::SetShow(bool const bShow)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( m_kShow != bShow )
	{
		m_kShow = bShow;
		OnStateChange();
		return true;
	}
	return false;
}

void PgWEClientObjectState::OnStateChange()
{
}

bool PgWEClientObjectState::Parse(TiXmlElement const* pkNode)
{
	if( !pkNode )
	{
		return false;
	}

	std::string const kWEObjectElementName( "WORLDEVENT_OBJECT" );
	std::string const kWEObjectIDAttributeName( "ID" );
	std::string const kWEObjectInitStateAttributeName( "INIT_STATE" );
	std::string const kWEObjectInitShowAttributeName( "INIT_SHOW" );
	if( kWEObjectElementName != pkNode->Value() )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Unknown WorldEventClientObject ELEMENT Name[" << pkNode->Value() << L"]");
		return false;
	}

	TiXmlAttribute const* pkAttribute = pkNode->FirstAttribute();
	while( pkAttribute )
	{
		char const* szAttrName = pkAttribute->Name();
		char const* szAttrValue = pkAttribute->Value();

		if( kWEObjectIDAttributeName == szAttrName )
		{
			m_kID = static_cast< WORD >( PgStringUtil::SafeAtoi(szAttrValue) );
		}
		else if( kWEObjectInitStateAttributeName == szAttrName )
		{
			m_kState = PgStringUtil::SafeAtoi(szAttrValue);
		}
		else if( kWEObjectInitShowAttributeName == szAttrName )
		{
			m_kShow = PgStringUtil::SafeAtob(szAttrValue, true);
		}
		//else
		//{
		//	CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Unknown WorldEventClientObject Attribute[" << szAttrName << L"]");
		//	return false;
		//}
		pkAttribute = pkAttribute->Next();
	}
	return true;
}

void PgWEClientObjectState::Update(PgWEClientObjectState const& rhs)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kID = rhs.m_kID;
	m_kState = rhs.m_kState;
	m_kSyncTime = rhs.m_kSyncTime;
	m_kShow = rhs.m_kShow;

	OnStateChange();
}

void PgWEClientObjectState::WriteToPacket(BM::Stream& rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	rkPacket.Push( m_kID );
	rkPacket.Push( m_kState );
	rkPacket.Push( m_kSyncTime );
	rkPacket.Push( m_kShow );
}
bool PgWEClientObjectState::ReadFromPacket(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	return	rkPacket.Pop( m_kID )
		&&	rkPacket.Pop( m_kState )
		&&	rkPacket.Pop( m_kSyncTime )
		&&	rkPacket.Pop( m_kShow );
}



//
//
PgWEClientObjectServerMgr::PgWEClientObjectServerMgr()
{
}

PgWEClientObjectServerMgr::~PgWEClientObjectServerMgr()
{
}

bool PgWEClientObjectServerMgr::ParseWEClientObject(TiXmlElement const* pkWEObjectNode)
{
	if( !pkWEObjectNode )
	{
		return false;
	}

	PgWEClientObjectState kNewState;
	if( !kNewState.Parse(pkWEObjectNode) )
	{
		return false;
	}

	auto kRet = m_kContState.insert( std::make_pair(kNewState.ID(), kNewState) );
	if( !kRet.second )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Duplicate WorldEventClientObject ID[" << kNewState.ID() << L"]");
		return false;
	}
	return true;
}

bool PgWEClientObjectServerMgr::SetWEClientObjectState(WORD const kID, int const iNewState)
{
	ContWEObjectState::iterator find_iter = m_kContState.find( kID );
	if( m_kContState.end() == find_iter )
	{
		return false;
	}

	PgWEClientObjectState& rkState = (*find_iter).second;
	if( !rkState.SetStatus(iNewState) )
	{
		return true; // 상태 변화가 없다
	}

	NfyWEClientOjbectToGroundUser(rkState);
	return true;
}

bool PgWEClientObjectServerMgr::SetWEClientObjectShow(WORD const kID, bool const bShow)
{
	ContWEObjectState::iterator find_iter = m_kContState.find( kID );
	if( m_kContState.end() == find_iter )
	{
		return false;
	}

	PgWEClientObjectState& rkState = (*find_iter).second;
	if( !rkState.SetShow(bShow) )
	{
		return true; // 상태 변화가 없다
	}

	NfyWEClientOjbectToGroundUser(rkState);
	return true;
}

bool PgWEClientObjectServerMgr::GetWEClientObjectState(WORD const kID, PgWEClientObjectState const* &pkOut)
{
	ContWEObjectState::iterator find_iter = m_kContState.find( kID );
	if( m_kContState.end() == find_iter )
	{
		return false;
	}

	pkOut = &(*find_iter).second;
	return true;
}

void PgWEClientObjectServerMgr::NfyWEClientOjbectToGroundUser(PgWEClientObjectState const& rkState)
{
}

void PgWEClientObjectServerMgr::WEClientObjectWriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( m_kContState.size() );
	ContWEObjectState::const_iterator iter = m_kContState.begin();
	while( m_kContState.end() != iter )
	{
		((*iter).second).WriteToPacket(rkPacket);
		++iter;
	}
}

bool PgWEClientObjectServerMgr::ParseWEClientObjectList(TiXmlElement const* pkListElementNode)
{
	return _MyBaseType::ParseWEClientObjectList(pkListElementNode);
}

void PgWEClientObjectServerMgr::ClonseWEClientOjbect(PgWEClientObjectServerMgr& rkTo) const
{
	rkTo.m_kContState = m_kContState;
}