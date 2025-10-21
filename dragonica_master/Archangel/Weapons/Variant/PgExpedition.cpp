#include "stdafx.h"
#include "PgPlayer.h"
#include "PgParty.h"
#include "PgExpedition.h"

////////////////////////////////////////////////////////////////////
// tagExpeditionOption 원정대 옵션

tagExpeditionOption::tagExpeditionOption()
{
	Default();
}

tagExpeditionOption::tagExpeditionOption(const EExpeditionOptionExp eOptionExp, const EExpeditionOptionItem eOptionItem, const EExpeditionOptionPublicTitle eOptionPublicTitle)
{
	Clear();
	SetOptionExp(eOptionExp);
	SetOptionItem(eOptionItem);
	SetOptionPublicTitle(eOptionPublicTitle);
}

void tagExpeditionOption::Clear()
{
	iExpeditionOption	= 0;
	iExpeditionLevel	= 0;
	iExpeditionArea_NameNo	= 0;
	iExpeditionContinent = 0;
	bExpeditionState = true;
}

void tagExpeditionOption::Default()
{
	SetOptionExp(EOE_CalcShare);
	SetOptionItem(EOI_Order);
	SetOptionPublicTitle(EOT_Public);
	SetOptionLevel(90);
	SetOptionArea_NameNo(0);
	SetOptionContinent(0);
	SetOptionState(true);
}

void tagExpeditionOption::SetOptionOption(const int iOption)
{
	iExpeditionOption = iOption;
}

void tagExpeditionOption::SetOptionExp(const EExpeditionOptionExp eOptionExp)
{
	if( !eOptionExp )
	{
		return;
	}

	if( EOE_SafeMask == eOptionExp )
	{
		return;
	}

	iExpeditionOption = SetExpeditionBit(iExpeditionOption, eOptionExp, EOE_SafeMask);
}

void tagExpeditionOption::SetOptionItem(const EExpeditionOptionItem eOptionItem)
{
	if( !eOptionItem )
	{
		return;
	}

	if( EOI_SafeMask == eOptionItem )
	{
		return;
	}

	iExpeditionOption = SetExpeditionBit(iExpeditionOption, eOptionItem, EOI_SafeMask);
}

void tagExpeditionOption::SetOptionPublicTitle(const EExpeditionOptionPublicTitle eOptionPublicTitle)
{
	if( !eOptionPublicTitle )
	{
		return;
	}

	if( EOT_SafeMask == eOptionPublicTitle )
	{
		return;
	}

	iExpeditionOption = SetExpeditionBit(iExpeditionOption, eOptionPublicTitle, EOT_SafeMask);
}

void tagExpeditionOption::SetOptionLevel(const int iLevel)
{
	iExpeditionLevel = iLevel;
}

void tagExpeditionOption::SetOptionArea_NameNo(int const iArea_NameNo)
{
	iExpeditionArea_NameNo = iArea_NameNo;
}

void tagExpeditionOption::SetOptionContinent(int const iContinent)
{
	iExpeditionContinent = iContinent;
}

void tagExpeditionOption::SetOptionState(bool const bState)
{
	bExpeditionState = bState;
}

EExpeditionOptionExp tagExpeditionOption::GetOptionExp() const
{
	int const iFilter = EOE_SafeMask ^ (EOE_SafeMask | EOI_SafeMask);
	return (EExpeditionOptionExp)(iExpeditionOption & iFilter);
}

EExpeditionOptionItem tagExpeditionOption::GetOptionItem() const
{
	int const iFilter = EOI_SafeMask ^ (EOE_SafeMask | EOI_SafeMask);
	return (EExpeditionOptionItem)(iExpeditionOption & iFilter);
}

EExpeditionOptionPublicTitle tagExpeditionOption::GetOptionPublicTitle() const
{
	int const iFilter = EOT_SafeMask ^ (EOE_SafeMask | EOI_SafeMask);
	return (EExpeditionOptionPublicTitle)(iExpeditionOption & iFilter);
}

int tagExpeditionOption::GetOptionLevel() const
{
	return iExpeditionLevel;
}

int tagExpeditionOption::GetArea_NameNo() const
{
	return iExpeditionArea_NameNo;
}

int tagExpeditionOption::GetContinent() const
{
	return iExpeditionContinent;
}

bool tagExpeditionOption::GetState() const
{
	return bExpeditionState;
}

void tagExpeditionOption::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push(iExpeditionOption);
	rkPacket.Push(iExpeditionLevel);
	rkPacket.Push(iExpeditionArea_NameNo);
	rkPacket.Push(iExpeditionContinent);
	rkPacket.Push(bExpeditionState);
}

bool tagExpeditionOption::ReadFromPacket(BM::Stream &rkPacket)
{
	if( rkPacket.Pop(iExpeditionOption) )
	{
		if( rkPacket.Pop(iExpeditionLevel) )
		{
			if( rkPacket.Pop(iExpeditionArea_NameNo) )
			{
				if( rkPacket.Pop(iExpeditionContinent) )
				{
					if( rkPacket.Pop(bExpeditionState) )
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

tagExpeditionOption::tagExpeditionOption(const tagExpeditionOption &rkInfo)
{
	*this = rkInfo;
}

void tagExpeditionOption::operator =(tagExpeditionOption const & rkInfo)
{
	iExpeditionOption = rkInfo.iExpeditionOption;
	iExpeditionLevel = rkInfo.iExpeditionLevel;
	iExpeditionArea_NameNo = rkInfo.iExpeditionArea_NameNo;
	iExpeditionContinent = rkInfo.iExpeditionContinent;
	bExpeditionState = rkInfo.bExpeditionState;
}

////////////////////////////////////////////////////////////////////
// Expedition 원정대

template< class _T_Party >
PgExpedition< _T_Party >::PgExpedition()
{
	Clear();
}

template< class _T_Party >
PgExpedition< _T_Party >::~PgExpedition()
{
}

template< class _T_Party >
void PgExpedition< _T_Party >::Init(BM::GUID const & rkNewGuid, std::wstring const & rkNewName, SExpeditionOption const& rkOption)
{
	BM::CAutoMutex kLock(m_kMutex);

	Clear();

	ExpeditionGuid(rkNewGuid);
	ExpeditionName(rkNewName);

	m_kOption = rkOption;
}

template< class _T_Party >
void PgExpedition< _T_Party >::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kExpeditionGuid.Clear();
	m_kExpeditionName.clear();
	m_kMasterCharGuid.Clear();
	m_kOption.Default();

	m_kPartyList.clear();
	m_kPositionWaitList.clear();
}

template< class _T_Party >
void PgExpedition< _T_Party >::WriteToPacket(BM::Stream &rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);

	// 교체 대기 인원 정보.
	PU::TWriteArray_M(rkPacket, m_kPositionWaitList);

	// 기타 원정대 정보.
	rkPacket.Push(m_kExpeditionGuid);
	rkPacket.Push(m_kMasterCharGuid);
	rkPacket.Push(m_kExpeditionName);
	m_kOption.WriteToPacket(rkPacket);
	rkPacket.Push(m_kMaxMemberCount);
	rkPacket.Push(m_kMaxTeamCount);
	rkPacket.Push(m_kExpeditionNpc);

	rkPacket.Push(m_kUserInfo.size());
	ContExpeditionUserInfo::const_iterator iter = m_kUserInfo.begin();
	while( iter != m_kUserInfo.end() )
	{
		iter->second.WriteToPacket(rkPacket);

		++iter;
	}
}

template< class _T_Party >
void PgExpedition< _T_Party >::ReadFromPacket(BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	int nUserInfoSize = 0;
	VEC_CHANGEPOS_USER TempList;

	// 교체 대기 인원 정보.
	PU::TLoadArray_M(rkPacket, TempList);
	m_kPositionWaitList.swap(TempList);

	// 각 팀 정보.
	rkPacket.Pop(m_kExpeditionGuid);
	rkPacket.Pop(m_kMasterCharGuid);
	rkPacket.Pop(m_kExpeditionName);
	m_kOption.ReadFromPacket(rkPacket);
	rkPacket.Pop(m_kMaxMemberCount);
	rkPacket.Pop(m_kMaxTeamCount);
	rkPacket.Pop(m_kExpeditionNpc);

	rkPacket.Pop(nUserInfoSize);
	for(int i = 0 ; i < nUserInfoSize ; ++i)
	{
		SExpeditionUserInfo UserInfo;
		UserInfo.ReadFromPacket(rkPacket);
		auto Ret = m_kUserInfo.insert(std::make_pair(UserInfo.kCharGuid, UserInfo));	// 무조건 추가함.
		if( !Ret.second )
		{
			ContExpeditionUserInfo::mapped_type & Element = (*Ret.first).second;
			Element = UserInfo;
		}
	}
}

template< class _T_Party >
void PgExpedition< _T_Party >::WriteToPacketMemberInfo(BM::Stream & rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);

	rkPacket.Push(m_kExpeditionGuid);
	rkPacket.Push(m_kMasterCharGuid);
	m_kOption.WriteToPacket(rkPacket);
	rkPacket.Push(m_kExpeditionName);

	rkPacket.Push(m_kUserInfo.size());
	ContExpeditionUserInfo::const_iterator iter = m_kUserInfo.begin();
	while( iter != m_kUserInfo.end() )
	{
		iter->second.WriteToPacket(rkPacket);

		++iter;
	}
}

template< class _T_Party >
void PgExpedition< _T_Party >::ReadFromPacketMemberInfo(BM::Stream & rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	int nUserInfoSize = 0;

	rkPacket.Pop(m_kExpeditionGuid);
	rkPacket.Pop(m_kMasterCharGuid);
	m_kOption.ReadFromPacket(rkPacket);
	rkPacket.Pop(m_kExpeditionName);

	rkPacket.Pop(nUserInfoSize);
	for(int i = 0 ; i < nUserInfoSize ; ++i)
	{
		SExpeditionUserInfo UserInfo;
		rkPacket.Pop(UserInfo);
		m_kUserInfo.insert(std::make_pair(UserInfo.kCharGuid, UserInfo));	// 무조건 추가함.
	}

}

template< class _T_Party >
HRESULT PgExpedition< _T_Party >::Add(SContentsUser const & rkNewUserInfo, _T_Party * & OutParty, int & OutTeamNo)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool bIsMember = IsMember(rkNewUserInfo.kCharGuid);
	if( true == bIsMember )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
		return PRC_Fail_Party;
	}

	if( EV_MAX_MEMBER_CNT <= GetExpeditionMemberCount() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount"));
		return PRC_Fail_MemberCount;//인원수 제한으로 등록 불가
	}

	SContentsUser kUser;
	int iTeamCount = EET_TEAM_1;

	// 원정대는 첫번째 파티부터 채운다.
	ContParty::const_iterator party_iter = m_kPartyList.begin();
	while( party_iter != m_kPartyList.end() )
	{
		_T_Party * pParty = GetParty( *party_iter );
		if( NULL != pParty )
		{
			if( PV_MAX_MEMBER_CNT != pParty->MemberCount() )
			{
				bool bMaster = false;
				// 현재 비어있는 파티인 경우.
				if( 0 == pParty->MemberCount() )
				{
					bMaster = true;
				}

				HRESULT hAnsJoinRet = PRC_Success;

				SPartyUserInfo kUser(rkNewUserInfo);
				kUser.kChnGndKey.Channel(g_kProcessCfg.ChannelNo());
				hAnsJoinRet = pParty->PgParty::Add(kUser, bMaster);		// 파티에 바로 넣음. GlobalParty로 하면 대기자 검사를 하기 때문에 PgParty의 Add를 직접 호출함.

				OutParty = pParty;
				OutTeamNo = iTeamCount;

				return hAnsJoinRet;
			}			
		}
		iTeamCount += 1;
		++party_iter;
	}

	return PRC_Fail;
}

template< class _T_Party >
HRESULT PgExpedition< _T_Party >::Add(SPartyUserInfo const & UserInfo, _T_Party * pParty)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool bIsMember = IsMember(UserInfo.kCharGuid);
	if( true == bIsMember )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
		return PRC_Fail_Party;
	}

	if( NULL == pParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	bool bMaster = false;
	if( 0 == pParty->MemberCount() )		// 옮겨갈 파티가 빈파티라면 마스터로 해서 들어가도록 한다.
	{
		bMaster = true;
	}

	HRESULT hAnsJoinRet = pParty->PgParty::Add(UserInfo, bMaster);

	return hAnsJoinRet;
}

template< class _T_Party >
HRESULT PgExpedition< _T_Party >::Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	return PRC_Success;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::IsMember(BM::GUID const & rkCharGuid) const
{
	ContParty::const_iterator iter = m_kPartyList.begin();
	while( iter != m_kPartyList.end() )
	{
		_T_Party * pParty = GetParty( *iter );
		if( NULL != pParty )
		{
			if( true == pParty->IsMember(rkCharGuid) )
			{
				return true;
			}
		}
		++iter;
	}

	VEC_CHANGEPOS_USER::const_iterator user_iter = m_kPositionWaitList.begin();
	while( user_iter != m_kPositionWaitList.end() )
	{
		if( (*user_iter).kCharGuid == rkCharGuid )
		{
			return true;
		}
		++user_iter;
	}

	return false;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::IsMaster(BM::GUID const & rkCharGuid) const
{
	return (MasterCharGuid() == rkCharGuid);
}

template< class _T_Party >
size_t PgExpedition< _T_Party >::GetExpeditionPartyCount() const
{
	BM::CAutoMutex kLock(m_kMutex);

	return m_kPartyList.size();
}

template< class _T_Party >
bool PgExpedition< _T_Party >::AddParty(_T_Party *pkParty)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( m_kPartyList.end() == std::find(m_kPartyList.begin(), m_kPartyList.end(), pkParty->PartyGuid()) )
	{
		std::back_inserter(m_kPartyList) = pkParty->PartyGuid();
		pkParty->Expedition(true);
		return true;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return false;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::DelParty(_T_Party* pkParty)
{
	BM::CAutoMutex kLock(m_kMutex);

	return false;
}

template< class _T_Party >
void PgExpedition< _T_Party >::GetPartyList(VEC_GUID & rkOutParty) const
{
	BM::CAutoMutex kLock(m_kMutex);

	rkOutParty.clear();
	VEC_GUID::const_iterator iter = m_kPartyList.begin();
	while(m_kPartyList.end() != iter)
	{
		const ContParty::value_type &rkElement = (*iter);
		rkOutParty.push_back(rkElement);
		++iter;
	}
}

template< class _T_Party >
void PgExpedition< _T_Party >::GetPositionWaitList(VEC_CHANGEPOS_USER& rkOutList) const
{
	BM::CAutoMutex kLock(m_kMutex);
	
	rkOutList.clear();
	VEC_CHANGEPOS_USER::const_iterator iter = m_kPositionWaitList.begin();
	while( m_kPositionWaitList.end() != iter )
	{
		VEC_CHANGEPOS_USER::value_type const & rkElement = (*iter);
		rkOutList.push_back(rkElement);
		++iter;
	}
}

template< class _T_Party >
bool PgExpedition< _T_Party >::AddExpeditionMemberInfo(SContentsUser const & rkUserInfo, SExpeditionUserInfo & OutInfo, unsigned short const sTeam, bool bAlive)
{
	BM::CAutoMutex kLock(m_kMutex);

	SExpeditionUserInfo ExpeditionUserInfo(rkUserInfo, sTeam, bAlive);
	
	auto Ret = m_kUserInfo.insert(std::make_pair(rkUserInfo.kCharGuid, ExpeditionUserInfo));
	if( Ret.second)
	{
		OutInfo = Ret.first->second;
		return true;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return false;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::AddExpeditionMemberInfo(SPartyUserInfo* pkPartyUserInfo, SExpeditionUserInfo & OutInfo, unsigned short const sTeam, bool bAlive)
{
	BM::CAutoMutex kLock(m_kMutex);

	SExpeditionUserInfo ExpeditionUserInfo(*pkPartyUserInfo, sTeam, bAlive);
	
	auto Ret = m_kUserInfo.insert(std::make_pair(pkPartyUserInfo->kCharGuid, ExpeditionUserInfo));
	if( Ret.second)
	{
		OutInfo = Ret.first->second;
		return true;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return false;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::DelExpeditionMemberInfo(const BM::GUID &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( m_kUserInfo.end() == m_kUserInfo.find(rkCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kUserInfo.erase(rkCharGuid);
	return true;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::DisperseExpedition()
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kPartyList.clear();	

	m_kPositionWaitList.clear();				// 교체 대기 목록 삭제.

	m_kUserInfo.clear();						// 원정대원 정보 삭제.

	return true;
}

template< class _T_Party >
size_t PgExpedition< _T_Party >::GetExpeditionMemberCount() const
{
	BM::CAutoMutex kLock(m_kMutex);

	size_t iCount = 0;

	ContParty::const_iterator iter = m_kPartyList.begin();
	while( iter != m_kPartyList.end() )
	{
		_T_Party * pParty = GetParty( *iter );
		if( NULL != pParty )
		{
			iCount += pParty->MemberCount();
		}
		++iter;
	}

	iCount += m_kPositionWaitList.size();

	return iCount;
}

template< class _T_Party >
void PgExpedition< _T_Party >::GetMemberCharGuidList(VEC_GUID & OutCharGuid, BM::GUID const & IgnoreGuid ) const
{
	BM::CAutoMutex kLock(m_kMutex);

	OutCharGuid.clear();

	VEC_GUID GuidList;

	ContParty::const_iterator iter = m_kPartyList.begin();
	while( iter != m_kPartyList.end() )
	{
		_T_Party * pParty = GetParty( *iter );
		if( NULL != pParty )
		{
			GuidList.clear();
			pParty->GetMemberCharGuidList(GuidList, IgnoreGuid);
			if( !GuidList.empty() )
			{
				OutCharGuid.insert(OutCharGuid.end(), GuidList.begin(), GuidList.end());
			}
		}
		++iter;
	}

	VEC_CHANGEPOS_USER::const_iterator user_iter = m_kPositionWaitList.begin();
	while( user_iter != m_kPositionWaitList.end() )
	{
		if( (*user_iter).kCharGuid != IgnoreGuid )
		{
			OutCharGuid.push_back((*user_iter).kCharGuid);
		}
		++user_iter;
	}
}

template< class _T_Party >
bool PgExpedition< _T_Party >::GetMemberInfo(BM::GUID const & CharGuid, SPartyUserInfo& OutMember) const
{
	BM::CAutoMutex kLock(m_kMutex);

	ContParty::const_iterator iter = m_kPartyList.begin();
	while( iter != m_kPartyList.end() )
	{
		_T_Party * pParty = GetParty( *iter );
		if( NULL != pParty )
		{
			if( pParty->IsMember(CharGuid) )
			{
				pParty->GetMemberInfo(CharGuid, OutMember);
				return true;
			}
		}
		++iter;
	}

	VEC_CHANGEPOS_USER::const_iterator user_iter = m_kPositionWaitList.begin();
	while( user_iter != m_kPositionWaitList.end() )
	{
		if( (*user_iter).kCharGuid == CharGuid )
		{
			OutMember = (*user_iter);
			return true;
		}
		++user_iter;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::GetMemberInfo(BM::GUID const & CharGuid, SExpeditionUserInfo & OutMember) const
{
	BM::CAutoMutex kLock(m_kMutex);

	ContExpeditionUserInfo::const_iterator iter = m_kUserInfo.find(CharGuid);
	while( iter != m_kUserInfo.end() )
	{
		OutMember = iter->second;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

template< class _T_Party >
_T_Party* PgExpedition< _T_Party >::Find(BM::GUID const & PartyGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);

	ContParty::const_iterator iter = m_kPartyList.begin();
	while( iter != m_kPartyList.end() )
	{
		_T_Party * pParty = GetParty( *iter );
		if( pParty )
		{
			if( pParty->PartyGuid() == PartyGuid )
			{
				return pParty;
			}
		}
		++iter;
	}

	return NULL;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::AddPositionWait(SPartyUserInfo const & UserInfo)
{
	BM::CAutoMutex kLock(m_kMutex);

	VEC_CHANGEPOS_USER::const_iterator user_iter = m_kPositionWaitList.begin();
	while( user_iter != m_kPositionWaitList.end() )
	{
		if( (*user_iter).kCharGuid == UserInfo.kCharGuid )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		++user_iter;
	}

	m_kPositionWaitList.push_back(UserInfo);

	ContExpeditionUserInfo::iterator iter = m_kUserInfo.find(UserInfo.kCharGuid);
	if( iter != m_kUserInfo.end() )
	{
		ContExpeditionUserInfo::mapped_type & Element = iter->second;
		Element.sTeam = EET_TEAM_SHIFT;
	}

	return true;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::DelPositionWaitList(BM::GUID const & CharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	VEC_CHANGEPOS_USER::iterator iter = m_kPositionWaitList.begin();
	while( iter != m_kPositionWaitList.end() )
	{
		if( (*iter).kCharGuid == CharGuid )
		{
			m_kPositionWaitList.erase(iter);

			return true;
		}
		++iter;
	}

	return false;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::IsPositionWait(BM::GUID const & CharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	VEC_CHANGEPOS_USER::iterator iter = m_kPositionWaitList.begin();
	while( iter != m_kPositionWaitList.end() )
	{
		if( (*iter).kCharGuid == CharGuid )
		{
			return true;
		}
		++iter;
	}

	return false;
}

template< class _T_Party >
bool PgExpedition< _T_Party >::ChangeMaster(BM::GUID const & NewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);
	bool const bRandChange = (BM::GUID::NullData() == NewMaster); // GUID를 지정하지 않으면 임의 대상으로 변경함.

	if( 0 == GetExpeditionMemberCount() )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Cannot Expedition ChangeMaster because MemberCount == 0 Expedition[") << ExpeditionGuid().str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SPartyUserInfo NewMasterInfo;
	if( !bRandChange )
	{
		bool const bFindMember = GetMemberInfo(NewMaster, NewMasterInfo);
		if( !bFindMember )
		{
			GetRand(NewMasterInfo);
		}
	}
	else
	{
		GetRand(NewMasterInfo);
	}

	m_kMasterCharGuid = NewMasterInfo.kCharGuid;
	return true;//성공
}

template< class _T_Party >
bool PgExpedition< _T_Party >::GetRand(SPartyUserInfo & UserInfo)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( 0 == GetExpeditionMemberCount() )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Cannot Expedition ChangeMaster because MemberCount == 0 Expedition[") << ExpeditionGuid().str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iRand = BM::Rand_Index( static_cast<int>(m_kUserInfo.size()) );

	ContExpeditionUserInfo::iterator iter = m_kUserInfo.begin();
	int iCur = 0;
	while( iter != m_kUserInfo.end() )
	{
		if( iCur == iRand )
		{
			GetMemberInfo(iter->first, UserInfo);
		}
		++iCur;
		++iter;
	}
	return true;
}

template< class _T_Party >
_T_Party * PgExpedition< _T_Party >::GetParty(BM::GUID const & PartyGuid) const
{
	if( NULL == m_pPartyHash )
	{
		return NULL;
	}

	PgPartyMgr<_T_Party>::PartyHash::const_iterator itor = m_pPartyHash->find(PartyGuid);
	if( m_pPartyHash->end() != itor )
	{
		return itor->second;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Expedition

PgGlobalExpedition::PgGlobalExpedition()
{
}

PgGlobalExpedition::~PgGlobalExpedition()
{
}

HRESULT PgGlobalExpedition::Add(SContentsUser const & rkNewUserInfo, PgGlobalParty * & OutParty, int & OutTeamNo)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	if( false == IsWaitMember(rkNewUserInfo.kCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;	// 대기중인 캐릭터가아님.
	}

	HRESULT hRet = DelWaitMember(rkNewUserInfo.kCharGuid);
	if( hRet != PRC_Success )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hRet );
		return hRet;
	}

	return PgExpedition< PgGlobalParty >::Add(rkNewUserInfo, OutParty, OutTeamNo);
}

HRESULT PgGlobalExpedition::Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);

	return PgExpedition< PgGlobalParty >::Del(rkCharGuid, rkNewMaster);
}

HRESULT PgGlobalExpedition::AddWaitMember(BM::GUID const & rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( BM::GUID::IsNull(rkCharGuid) )
	{
		return PRC_Fail_MemberCount;
	}

	bool bMine = IsMember(rkCharGuid);
	if( true == bMine )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MineMember"));
		return PRC_Fail_MineMember;	// 이미 추가된 멤버.
	}

	// 원정대 빈자리 확인.
	if( EV_MAX_MEMBER_CNT == GetExpeditionMemberCount() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount") );
		return PRC_Fail_MemberCount; // 원정대 멤버수가 최대치.
	}

	DWORD const dwCurTime = BM::GetTime32() + 60000; // 가입신청 수락 이후 지정된 시간이 경과 되면 대기자 리스트에서 삭제.
	auto Ret = m_kJoinWaitList.insert( std::make_pair(rkCharGuid, dwCurTime) ); // 원정대 가입 대기자열에 등록.
	if( Ret.second )
	{
		return PRC_Success;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Waiter"));
	return PRC_Fail_Waiter;	// 이미 대기 중.
}

HRESULT PgGlobalExpedition::DelWaitMember(BM::GUID const & rkCharGuid)
{
	ContWaitJoinList::iterator iter = m_kJoinWaitList.find(rkCharGuid);
	if( iter != m_kJoinWaitList.end() )
	{
		m_kJoinWaitList.erase(iter);
		return PRC_Success;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail!"));
	return PRC_Fail;
}

bool PgGlobalExpedition::IsWaitMember(BM::GUID const & rkCharGuid)
{
	ContWaitJoinList::const_iterator iter = m_kJoinWaitList.find(rkCharGuid);
	if( iter != m_kJoinWaitList.end() )
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false!"));
	return false;
}

void PgGlobalExpedition::GetJoinWaitList(ContWaitJoinList& rkOutList) const
{
	BM::CAutoMutex kLock(m_kMutex);

	rkOutList.clear();
	ContWaitJoinList::const_iterator iter = m_kJoinWaitList.begin();
	while( m_kJoinWaitList.end() != iter )
	{
		ContWaitJoinList::value_type const & rkElement = (*iter);
		rkOutList.insert(rkElement);
		++iter;
	}
}

HRESULT PgGlobalExpedition::AddToTeam(SPartyUserInfo const & UserInfo, int iTeam, PgGlobalParty * & pOutParty)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( iTeam < EET_TEAM_SHIFT )	// 1~4번만 원정대 팀. 5번은 교체 대기 리스트로 가야됨.
	{
		BM::GUID PartyGuid = m_kPartyList.at(iTeam - 1);	// 팀에 해당하는 파티.
		PgGlobalParty * pParty  = GetParty(PartyGuid);
		if( NULL == pParty )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail!"));
			return PRC_Fail;
		}
		if( PV_MAX_MEMBER_CNT <= pParty->MemberCount() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount!"));
			return PRC_Fail_MemberCount;		// 옮길 파티가 가득 찼다.
		}

		ContExpeditionUserInfo::iterator iter = m_kUserInfo.find(UserInfo.kCharGuid);
		if( iter != m_kUserInfo.end() )
		{
			ContExpeditionUserInfo::mapped_type & Element = iter->second;
			Element.sTeam = iTeam;
		}

		pOutParty = pParty;
		return PgExpedition::Add(UserInfo, pParty);
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail!"));
		return PRC_Fail;
	}

	return PRC_Success;
}

bool PgGlobalExpedition::CheckMovable(int iTeam)
{
	BM::CAutoMutex Lock(m_kMutex);

	if( iTeam <= EET_TEAM_0 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false!"));
		return false;
	}

	if( iTeam <= MaxTeamCount() )
	{
		BM::GUID PartyGuid = m_kPartyList.at(iTeam - 1); // -1 해야됨. 0부터.
		PgGlobalParty * pParty = GetParty(PartyGuid);
		if( NULL == pParty )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false!"));
			return false;
		}

		if( PV_MAX_MEMBER_CNT <= pParty->MemberCount() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false!"));
			return false;
		}
	}
	else if( iTeam == EET_TEAM_SHIFT )
	{
		if( PV_MAX_MEMBER_CNT <= m_kPositionWaitList.size() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false!"));
			return false;
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false!"));
		return false;
	}

	return true;
}

void PgGlobalExpedition::WrtieToPacketListInfo(BM::Stream & Packet)
{
	SPartyUserInfo MasterInfo;
	GetMemberInfo(MasterCharGuid(), MasterInfo);

	SExpeditionListInfo Info;
	Info.ExpeditionGuid = ExpeditionGuid();
	Info.MasterGuid = MasterInfo.kCharGuid;
	Info.MasterName = MasterInfo.kName;
	Info.MapNo = Option().GetArea_NameNo();
	Info.CurMemberCount = GetExpeditionMemberCount();
	Info.MaxMemberCount = GetMaxMemberCount();

	Info.WriteToPacket(Packet);
	Option().WriteToPacket(Packet);
}

void PgGlobalExpedition::ModifyMemberState(SExpeditionUserInfo & MemberInfo)
{
	ContExpeditionUserInfo::iterator iter = m_kUserInfo.find(MemberInfo.kCharGuid);
	if( iter != m_kUserInfo.end() )
	{
		(*iter).second.Set(MemberInfo);
	}
}

void PgGlobalExpedition::ClearExpeditionJoinWaitMember(DWORD Time)
{
	if( m_kJoinWaitList.empty() )
	{	// 비었으면 더 이상 진행할 필요가 없다.
		return;
	}

	ContWaitJoinList::iterator iter = m_kJoinWaitList.begin();
	for( ; iter != m_kJoinWaitList.end() ; ++iter )
	{
		ContWaitJoinList::mapped_type & Element = iter->second;
		if( Time > Element )
		{	// 삭제 대기시간을 지났다면, 가입 대기자 리스트에서 제거.
			m_kJoinWaitList.erase(iter);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local Expedition

PgLocalExpedition::PgLocalExpedition()
{
}

PgLocalExpedition::~PgLocalExpedition()
{
}

void PgLocalExpedition::Init(BM::GUID const & rkNewGuid, std::wstring const & rkNewName, SExpeditionOption const & rkOption)
{
	BM::CAutoMutex kLock(m_kMutex);

	PgExpedition< PgLocalParty >::Init(rkNewGuid, rkNewName, rkOption);
}

void PgLocalExpedition::Clear()
{
	BM::CAutoMutex kLock(m_kMutex); 

	PgExpedition< PgLocalParty >::Clear();
}

HRESULT PgLocalExpedition::Add(SContentsUser const & rkNewUserInfo, PgLocalParty * & OutParty)
{
	BM::CAutoMutex kLock(m_kMutex);

	return PgExpedition< PgLocalParty >::Add(rkNewUserInfo, OutParty);
}

HRESULT PgLocalExpedition::Del(BM::GUID const & rkCharGuid, BM::GUID const & rkNewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);

	return PgExpedition< PgLocalParty >::Del(rkCharGuid, rkNewMaster);
}

bool PgLocalExpedition::GetLocalMemberList( SChnGroundKey const & GndKey, VEC_GUID & OutVec, BM::GUID const & IgnoreGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);

	VEC_GUID GuidVec;

	ContParty::const_iterator iter = m_kPartyList.begin();
	while( m_kPartyList.end() != iter )
	{
		PgLocalParty * pParty = GetParty( *iter );
		if( pParty )
		{
			GuidVec.clear();
			pParty->GetLocalMemberList(GndKey, OutVec, IgnoreGuid);

			if( !OutVec.empty() )
			{
				OutVec.insert(OutVec.end(), GuidVec.begin(), GuidVec.end());
			}
		}
		++iter;
	}

	VEC_CHANGEPOS_USER::const_iterator user_iter = m_kPositionWaitList.begin();
	while( user_iter != m_kPositionWaitList.end() )
	{
		if( (*user_iter).kCharGuid != IgnoreGuid )
		{
			OutVec.push_back((*user_iter).kCharGuid);
		}
		++user_iter;
	}

	if(0 != OutVec.size())
	{
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
bool PgLocalExpedition::CleanUpPartyList(PgLocalParty * pParty)
{
	BM::CAutoMutex Lock(m_kMutex);

	if( NULL == pParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContParty::iterator iter = m_kPartyList.begin();
	while( iter != m_kPartyList.end() )
	{
		PgLocalParty * pExpeditionParty = GetParty( *iter );
		if( NULL != pExpeditionParty )
		{
			if( pExpeditionParty->PartyGuid() == pParty->PartyGuid() )
			{
				iter = m_kPartyList.erase(iter);
				break;
			}
		}
		++iter;
	}

	return true;
}

HRESULT PgLocalExpedition::IsStartable()
{
	BM::CAutoMutex Lock(m_kMutex);

	if( !m_kPositionWaitList.empty() )
	{	// 대기자가 있으면 출발 할 수없음.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_WaitPlayer"));
		return PRC_Fail_WaitPlayer;
	}

	return PRC_Success;
}

bool PgLocalExpedition::DeadExpeditionMember(BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_kMutex);

	ContExpeditionUserInfo::iterator itor = m_kUserInfo.find(CharGuid);
	if( itor != m_kUserInfo.end() )
	{
		ContExpeditionUserInfo::mapped_type & Element = itor->second;
		Element.bAlive = false; // 죽음.

		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

BM::GUID const & PgLocalExpedition::GetExpeditionNpcGuid()
{
	return ExpeditionNpc();
}

int PgLocalExpedition::GetMemberTeamNo(BM::GUID const & CharGuid)
{
	ContExpeditionUserInfo::const_iterator iter = m_kUserInfo.find(CharGuid);
	if( iter != m_kUserInfo.end() )
	{
		return iter->second.sTeam;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EET_TEAM_0"));
	return EET_TEAM_1;
}

void PgLocalExpedition::WrtieToPacketListInfo(BM::Stream & Packet)
{
	SPartyUserInfo MasterInfo;
	GetMemberInfo(MasterCharGuid(), MasterInfo);

	SExpeditionListInfo Info;
	Info.ExpeditionGuid = ExpeditionGuid();
	Info.MasterGuid = MasterInfo.kCharGuid;
	Info.MasterName = MasterInfo.kName;
	Info.MapNo = Option().GetArea_NameNo();
	Info.CurMemberCount = GetExpeditionMemberCount();
	Info.MaxMemberCount = GetMaxMemberCount();

	Info.WriteToPacket(Packet);
	Option().WriteToPacket(Packet);
}