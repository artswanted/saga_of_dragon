#include "stdafx.h"
#include "Log.h"

//////////////////////////////////////////////////////
//		PgLog

PgLog::PgLog(EOrderMainType eMainType, EOrderSubType eSubType)
: m_kOrderMainType(eMainType), m_kOrderSubType(eSubType)
{
	m_kContInt.resize(E_INT_SIZE, 0);
	m_kContInt64.resize(E_I64_SIZE, 0);
}

PgLog::~PgLog()
{
}

PgLog& PgLog::operator = (PgLog const &rhs)
{
	OrderMainType(rhs.OrderMainType());
	OrderSubType(rhs.OrderSubType());

	m_kContInt = rhs.m_kContInt;
	m_kContInt64 = rhs.m_kContInt64;
	//m_kContGuid = rhs.m_kContGuid;
	m_kContString = rhs.m_kContString;
	return *this;
}

void PgLog::WriteToPacket(BM::Stream &kPacket)const
{
	kPacket.Push(OrderMainType());
	kPacket.Push(OrderSubType());

	PU::TWriteArray_A(kPacket, m_kContInt);
	PU::TWriteArray_A(kPacket, m_kContInt64);
	//PU::TWriteArray_A(kPacket, m_kContGuid);

	kPacket.Push( m_kContString.size() );
	CONT_LOG_STRING::const_iterator itr = m_kContString.begin();
	for ( ; itr!=m_kContString.end() ; ++itr )
	{
		kPacket.Push( itr->first );
		kPacket.Push( itr->second );
	}
}

void PgLog::ReadFromPacket(BM::Stream &kPacket)
{
	kPacket.Pop(m_kOrderMainType);
	kPacket.Pop(m_kOrderSubType);

	m_kContInt.clear();
	m_kContInt64.clear();
	m_kContString.clear();
	PU::TLoadArray_A(kPacket, m_kContInt);
	PU::TLoadArray_A(kPacket, m_kContInt64);
	//PU::TLoadArray_A(kPacket, m_kContGuid);

	size_t iSize = 0;
	CONT_LOG_STRING::mapped_type kElement;
	kPacket.Pop( iSize );
	while ( iSize-- )
	{
		size_t iIndex = 0;
		kPacket.Pop( iIndex );
		if ( true == kPacket.Pop( kElement ) )
		{
			m_kContString.insert( std::make_pair( iIndex, kElement ) );
		}
	}
}

void PgLog::Set( size_t const iIndex, int const iValue )
{
	if ( iIndex < m_kContInt.size() )
	{
		m_kContInt.at(iIndex) = iValue;
	}
	else
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("int Index error =") << iIndex << _T(", OrderMainType = ") << int(OrderMainType()) << _T(", OrderSubType = ") << int(OrderSubType()));
	}
}

void PgLog::Set( size_t const iIndex, __int64 const i64Value )
{
	if ( iIndex < m_kContInt64.size() )
	{
		m_kContInt64.at(iIndex) = i64Value;
	}
	else
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("__int64 Index error =") << iIndex << _T(", OrderMainType = ") << int(OrderMainType()) << _T(", OrderSubType = ") << int(OrderSubType()));
	}
}

/*
void PgLog::Set( size_t const iIndex, BM::GUID const& rkGuid)
{
	if ( iIndex < m_kContGuid.size() )
	{
		m_kContGuid.at(iIndex) = rkGuid;
	}
	else
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Index error =") << iIndex );
	}
}
*/

void PgLog::Set( size_t const iIndex, std::wstring const& rkString)
{
	if ( iIndex < E_STR_SIZE )
	{
		auto kPair = m_kContString.insert( std::make_pair(iIndex, rkString) );
		if ( !kPair.second )
		{
			kPair.first->second = rkString;
		}
	}
	else
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("std::wstring Index error =") << iIndex << _T(", OrderMainType = ") << int(OrderMainType()) << _T(", OrderSubType = ") << int(OrderSubType()));
	}	
}

bool PgLog::Get( size_t const iIndex, int& iValue  ) const
{
	if ( iIndex < m_kContInt.size() )
	{
		iValue = m_kContInt.at(iIndex);
		return true;
	}

	CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("int Index error =") << iIndex << _T(", OrderMainType = ") << int(OrderMainType()) << _T(", OrderSubType = ") << int(OrderSubType()));
	return false;
}

bool PgLog::Get( size_t const iIndex, __int64 &i64Value ) const
{
	if ( iIndex < m_kContInt64.size() )
	{
		i64Value = m_kContInt64.at(iIndex);
		return true;
	}

	CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("__int64 Index error =") << iIndex << _T(", OrderMainType = ") << int(OrderMainType()) << _T(", OrderSubType = ") << int(OrderSubType()));
	return false;
}

/*
bool PgLog::Get( size_t const iIndex, BM::GUID &rkGuid ) const
{
	if ( iIndex < m_kContGuid.size() )
	{
		rkGuid = m_kContGuid.at(iIndex);
		return true;
	}

	CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Index error =") << iIndex);
	return false;
}
*/

bool PgLog::Get( size_t const iIndex, std::wstring &rkString ) const
{
	CONT_LOG_STRING::const_iterator itr = m_kContString.find( iIndex );
	if ( itr != m_kContString.end() )
	{
		rkString = itr->second;
		return true;
	}

	CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("std::wstring Index error =") << iIndex << _T(", OrderMainType = ") << int(OrderMainType()) << _T(", OrderSubType = ") << int(OrderSubType()));
	return false;
}

bool PgLog::BuildQuery(CEL::DB_QUERY& rkQuery) const
{
	rkQuery.PushStrParam(static_cast<short>(OrderMainType()));
	rkQuery.PushStrParam(static_cast<short>(OrderSubType()));
	BuildQuery_Sub( m_kContInt, rkQuery );
	BuildQuery_Sub( m_kContInt64, rkQuery );
	//BuildQuery_Sub( m_kContGuid, rkQuery );
	BuildQuery_Sub( m_kContString, rkQuery, static_cast<size_t>(E_STR_SIZE) );

	return true;
}



//////////////////////////////////////////////////////
//		PgLogTran

PgLogCont::PgLogCont(ELogMainType const eMainType, ELogSubType const eSubType, BM::GUID const& rkMemberGuid, BM::GUID const& rkCharGuid,
		std::wstring const& rkID, std::wstring const& rkName, short const sClass, short const sLevel, int const iGroundNo, EUnitGender eGender)
:	m_kLogMainType(eMainType), m_kLogSubType(eSubType),
	m_kLogKey(BM::GUID::Create()), m_kMemberKey(rkMemberGuid), m_kCharacterKey(rkCharGuid),
	m_kID(rkID), m_kName(rkName), m_kClass(sClass), m_kLevel(sLevel), m_kGroundNo(iGroundNo), m_kUnitGender(eGender)
,	m_kUID(0)
,	m_kChannelNo(0)
{
	RealmNo(g_kProcessCfg.RealmNo());
	ChannelNo(g_kProcessCfg.ChannelNo());
}

PgLogCont::~PgLogCont()
{
}

void PgLogCont::WriteToPacket(BM::Stream &kPacket)const
{
	size_t iLogSize = m_kLog.size();
	kPacket.Push(iLogSize);
	if( iLogSize <= 0 )
	{
		return;
	}
	kPacket.Push(LogKey());
	kPacket.Push(LogMainType());
	kPacket.Push(LogSubType());
	kPacket.Push(MemberKey());
	kPacket.Push(CharacterKey());
	kPacket.Push(ID());
	kPacket.Push(UID());
	kPacket.Push(Name());
	kPacket.Push(RealmNo());
	kPacket.Push(ChannelNo());
	kPacket.Push(GroundNo());
	kPacket.Push(Class());
	kPacket.Push(Level());

	for (size_t i=0; i<iLogSize; i++)
	{
		m_kLog.at(i).WriteToPacket(kPacket);
	}
}

void PgLogCont::ReadFromPacket(BM::Stream &kPacket)
{
	size_t iLogSize = 0;
	kPacket.Pop(iLogSize);
	if( iLogSize <= 0 )
	{
		return;
	}
	kPacket.Pop(m_kLogKey);
	kPacket.Pop(m_kLogMainType);
	kPacket.Pop(m_kLogSubType);
	kPacket.Pop(m_kMemberKey);
	kPacket.Pop(m_kCharacterKey);
	kPacket.Pop(m_kID);
	kPacket.Pop(m_kUID);
	kPacket.Pop(m_kName);
	kPacket.Pop(m_kRealmNo);
	kPacket.Pop(m_kChannelNo);
	kPacket.Pop(m_kGroundNo);
	kPacket.Pop(m_kClass);
	kPacket.Pop(m_kLevel);

	m_kLog.resize(iLogSize);
	for (size_t i=0; i<iLogSize; i++)
	{
		m_kLog.at(i).ReadFromPacket(kPacket);
	}
}

void PgLogCont::Add(PgLog const& kLog)
{
	m_kLog.push_back(kLog);
}

bool PgLogCont::Commit() const
{
	if( m_kLog.empty() )
	{
		// 찍을게 없으면 찍지 말자
		return false;
	}

	if( m_kLogMainType == ELogMain_None )
	{
		//	CAUTION_LOG(BM::LOG_LV1, _T("LogMainType[") << static_cast< int >(m_kLogMainType) << _T("] LogSubType[") << static_cast< int >(m_kLogSubType) << _T("] is empty log Data"));

		// LogMainType or LogSubType이 0인 경우에는 로그도 남기지 않고 로그 서버로 패킷도 보내지 않도록 수정.
		// 2011.07.18, 김종수.
		return false;
	}

	BM::Stream kPacket( PT_A_GAME_LOG );
	WriteToPacket(kPacket);
	return g_kProcessCfg.Locked_SendToServerType(CEL::ST_LOG, kPacket);
}

bool PgLogCont::BuildQuery(CEL::DB_QUERY& rkQuery) const
{
	rkQuery.PushStrParam(LogKey());
	rkQuery.PushStrParam(static_cast<short>(LogMainType()));
	rkQuery.PushStrParam(static_cast<short>(LogSubType()));
	rkQuery.PushStrParam(MemberKey());
	rkQuery.PushStrParam(CharacterKey());
	rkQuery.PushStrParam(ID());
	rkQuery.PushStrParam(Name());
	rkQuery.PushStrParam(ChannelNo());
	rkQuery.PushStrParam(GroundNo());
	rkQuery.PushStrParam(Class());
	rkQuery.PushStrParam(Level());
	return true;
}

bool PgLogCont::BuildQuery(size_t iIndex, CEL::DB_QUERY& rkQuery) const
{
	if (m_kLog.size() <= iIndex)
	{
		return false;
	}
	rkQuery.PushStrParam(LogKey());
	m_kLog.at(iIndex).BuildQuery(rkQuery);
	return true;
}

size_t PgLogCont::LogCount() const
{
	return m_kLog.size();
}

PgLog const* const PgLogCont::GetLog(int const iIndex) const
{
	if (static_cast<int>(m_kLog.size()) <= iIndex)
	{
		return NULL;
	}
	return &m_kLog.at(iIndex);
}

PgLog const* const PgLogCont::GetLog(EOrderMainType eMainType, EOrderSubType eSubType)
{
	VECTOR_LOG::const_iterator log_itor = m_kLog.begin();
	while (log_itor != m_kLog.end())
	{
		if ((*log_itor).OrderMainType() == eMainType && (*log_itor).OrderSubType() == eSubType)
		{
			return &(*log_itor);
		}
		++log_itor;
	}
	return NULL;
}

void PgLogCont::UpdateLog(PgLog const& rkNewLog)
{
	VECTOR_LOG::iterator log_itor = m_kLog.begin();
	while (log_itor != m_kLog.end())
	{
		if ((*log_itor).OrderMainType() == rkNewLog.OrderMainType() && (*log_itor).OrderSubType() == rkNewLog.OrderSubType())
		{
			(*log_itor) = rkNewLog;
			return;
		}
		++log_itor;
	}
}

PgLogCont const& PgLogCont::operator =(PgLogCont const& rhs)
{
	LogMainType(rhs.LogMainType());
	LogSubType(rhs.LogSubType());
	LogKey(rhs.LogKey());
	MemberKey(rhs.MemberKey());
	CharacterKey(rhs.CharacterKey());
	m_kID = rhs.m_kID;
	UID(rhs.UID());
	m_kName = rhs.m_kName;
	RealmNo(rhs.RealmNo());
	ChannelNo(rhs.ChannelNo());
	Class(rhs.Class());
	Level(rhs.Level());
	GroundNo(rhs.GroundNo());

	m_kLog = rhs.m_kLog;
	return (*this);
}


//////////////////////////////////////////////////////
//		PgContLogMgr

PgContLogMgr::PgContLogMgr()
{
	m_kGuid.Generate();
}

bool const PgContLogMgr::AddContLog(PgLogCont const & kLog)
{
	auto kInsert = m_kContLog.insert(std::make_pair(kLog.CharacterKey(),kLog));
	if(kInsert.second)
	{
		//kInsert.first->second.LogKey(Guid());
		return true;
	}
	return false;
}

bool const PgContLogMgr::AddLog(BM::GUID const & kCharGuid,PgLog const & kLog)
{
	CONT_LOG_CONT::iterator iter = m_kContLog.find(kCharGuid);
	if(m_kContLog.end() == iter)
	{
		return false;
	}

	(*iter).second.Add(kLog);
	return true;
}

bool const PgContLogMgr::AddUpdateLog(BM::GUID const & kCharGuid,PgLog const & kLog)
{
	CONT_LOG_CONT::iterator iter = m_kContLog.find(kCharGuid);
	if(m_kContLog.end() == iter)
	{
		return false;
	}
	if( ELOrderMain_Exp == kLog.OrderMainType() ) // Exp와 Fran을 하나의 로그로 합친다.
	{
		PgLog const *kOldLog = (*iter).second.GetLog(kLog.OrderMainType(), ELOrderSub_Modify);

		if( NULL != kOldLog )
		{
			__int64 i64Value0 = 0;
			__int64 i64Value1 = 0;
			__int64 i64Value2 = 0;
			__int64 i64Value3 = 0;
			__int64 i64Value4 = 0;

			kOldLog->Get(0, i64Value0);	// 경험치
			kOldLog->Get(1, i64Value1);
			kOldLog->Get(2, i64Value2);
			kLog.Get(3, i64Value3);		// Fran
			kLog.Get(4, i64Value4);

			PgLog kNewLog(kLog.OrderMainType(), ELOrderSub_Modify);
			kNewLog.Set(0, i64Value0);
			kNewLog.Set(1, i64Value1);
			kNewLog.Set(2, i64Value2);
			kNewLog.Set(3, i64Value3);
			kNewLog.Set(4, i64Value4);

			(*iter).second.UpdateLog(kNewLog);
			return true;
		}
	}

	(*iter).second.Add(kLog);
	return true;
}

void PgContLogMgr::SendLog()const
{
	for(CONT_LOG_CONT::const_iterator iter = m_kContLog.begin();iter != m_kContLog.end();++iter)
	{
		(*iter).second.Commit();
	}
}

PgLogCont * const PgContLogMgr::GetLogCont(BM::GUID const& rkGuid)
{
	CONT_LOG_CONT::iterator iter = m_kContLog.find(rkGuid);
	if(m_kContLog.end() == iter)
	{
		return NULL;
	}
	return &(iter->second);
}

void PgContLogMgr::GetLogCont(VEC_GUID& rkGuid) const
{
	for(CONT_LOG_CONT::const_iterator iter = m_kContLog.begin();iter != m_kContLog.end();++iter)
	{
		rkGuid.push_back(iter->first);
	}
}

void PgContLogMgr::MergeLogCont(PgContLogMgr const& rkCont)
{
	for(CONT_LOG_CONT::const_iterator c_it = rkCont.m_kContLog.begin(); c_it != rkCont.m_kContLog.end(); ++c_it)
	{
		size_t const iCount = (*c_it).second.LogCount();
		for(int i = 0; i < iCount; ++i)
		{
			if(PgLog const* pkLog = (*c_it).second.GetLog(i))
			{
				AddLog((*c_it).first, *pkLog);
			}
		}
	}
}
