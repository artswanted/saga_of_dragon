#include "stdafx.h"
#include "PgPlayer.h"
#include "PgParty.h"
#include "PgPartyContents.h"

////////////////////////////////////////////////////////////////////
// Party Option

tagPartyOption::tagPartyOption()
{
	Default();
}

tagPartyOption::tagPartyOption(const EPartyOptionExp eOptionExp, const EPartyOptionItem eOptionItem, const EPartyOptionPublicTitle eOptionPublicTitle)
{
	Clear();
	SetOptionExp(eOptionExp);
	SetOptionItem(eOptionItem);
	SetOptionPublicTitle(eOptionPublicTitle);
}

void tagPartyOption::Clear()
{
	iPartyOption = 0;
	iPartyLevel = 0;
	m_kPartySubName.clear();
	iPartyAttribute = 0;;
	iPartyContinent = 0;
	PartyMaxMember	= PV_MAX_MEMBER_CNT;
	iPartyArea_NameNo = 0;
	byPartyState = 0;
}

void tagPartyOption::Default()
{
	SetOptionExp(POE_CalcShare);
	SetOptionItem(POI_Order);
	SetOptionPublicTitle(POT_Public);
	SetOptionLevel(90);			// Default -10, +10 Level
	m_kPartySubName.clear();
	SetOptionAttribute(POA_Field);
	SetOptionContinent(POC_Area1);
	SetOptionArea_NameNo(0);
	SetOptionState(0);
	SetOptionMaxMember(PV_MAX_MEMBER_CNT);
}

void tagPartyOption::SetOptionExp(const EPartyOptionExp eOptionExp)
{
	if( !eOptionExp )
	{
		return;
	}

	if( POE_SafeMask == eOptionExp )
	{
		return;
	}

	iPartyOption = SetPartyBit(iPartyOption, eOptionExp, POE_SafeMask);
}

void tagPartyOption::SetOptionItem(const EPartyOptionItem eOptionItem)
{
	if( !eOptionItem )
	{
		return ;
	}

	if( POI_SafeMask == eOptionItem )
	{
		return;
	}

	iPartyOption = SetPartyBit(iPartyOption, eOptionItem, POI_SafeMask);
}

void tagPartyOption::SetOptionPublicTitle(const EPartyOptionPublicTitle eOptionPublicTitle)
{
	if( !eOptionPublicTitle )
	{
		return ;
	}

	if( POT_SafeMask == eOptionPublicTitle )
	{
		return;
	}

	iPartyOption = SetPartyBit(iPartyOption, eOptionPublicTitle, POT_SafeMask);
}

void tagPartyOption::SetOptionLevel(int const iLevel)
{
	iPartyLevel = iLevel;
}

void tagPartyOption::SetOptionOption(const int iOption)
{
	iPartyOption = iOption;
}

void tagPartyOption::SetOptionAttribute(const int iAttribute)
{
	iPartyAttribute = iAttribute;
}

void tagPartyOption::SetOptionContinent(const int iContinent)
{
	iPartyContinent = iContinent;
}

void tagPartyOption::SetOptionMaxMember(const int MaxMember)
{
	PartyMaxMember = MaxMember;
}

void tagPartyOption::SetOptionArea_NameNo(const int iArea_NameNo)
{
	iPartyArea_NameNo = iArea_NameNo;
}

void tagPartyOption::SetOptionState(const BYTE byState)
{
	byPartyState = byState;
}

void tagPartyOption::AddOptionState(const BYTE bState)
{
	byPartyState |= bState;
}

void tagPartyOption::RemoveOptionState(const BYTE bState)
{
	byPartyState &= (~bState);
}

std::wstring const& tagPartyOption::GetOptionPartySubName() const
{
	return PartySubName();
}

EPartyOptionAttribute tagPartyOption::GetOptionAttribute() const
{
	return static_cast<EPartyOptionAttribute>(iPartyAttribute);
}

EPartyOptionContinent tagPartyOption::GetOptionContinent() const
{
	return static_cast<EPartyOptionContinent>(iPartyContinent);
}

int tagPartyOption::GetOptionMaxMember() const
{
	return PartyMaxMember;
}

int tagPartyOption::GetOptionArea_NameNo() const
{
	return iPartyArea_NameNo;
}

BYTE tagPartyOption::GetOptionState() const
{
	return byPartyState;
}

EPartyOptionExp tagPartyOption::GetOptionExp() const
{
	int const iFilter = POE_SafeMask ^ (POE_SafeMask | POI_SafeMask);
	return (EPartyOptionExp)(iPartyOption & iFilter);
}

EPartyOptionItem tagPartyOption::GetOptionItem() const
{
	int const iFilter = POI_SafeMask ^ (POE_SafeMask | POI_SafeMask);
	return (EPartyOptionItem)(iPartyOption & iFilter);
}

ePartyOptionPublicTitle tagPartyOption::GetOptionPublicTitle() const
{
	int const iFilter = POT_SafeMask ^ (POE_SafeMask | POI_SafeMask);
	return (ePartyOptionPublicTitle)(iPartyOption & iFilter);
}

int tagPartyOption::GetOptionLevel() const
{
	return iPartyLevel;
}


void tagPartyOption::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push(iPartyOption);
	rkPacket.Push(iPartyLevel);
	rkPacket.Push(m_kPartySubName);
	rkPacket.Push(iPartyAttribute);
	rkPacket.Push(iPartyContinent);
	rkPacket.Push(PartyMaxMember);
	rkPacket.Push(iPartyArea_NameNo);
	rkPacket.Push(byPartyState);
}

bool tagPartyOption::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop(iPartyOption);
	rkPacket.Pop(iPartyLevel);
	rkPacket.Pop(m_kPartySubName);
	rkPacket.Pop(iPartyAttribute);
	rkPacket.Pop(iPartyContinent);
	rkPacket.Pop(PartyMaxMember);
	rkPacket.Pop(iPartyArea_NameNo);
	rkPacket.Pop(byPartyState);
	return rkPacket.RemainSize()==0;
}

tagPartyOption::tagPartyOption(const tagPartyOption &rkInfo)
{
	*this = rkInfo;
}

void tagPartyOption::operator = (const tagPartyOption &rkInfo)
{
	iPartyOption = rkInfo.iPartyOption;
	iPartyLevel = rkInfo.iPartyLevel;
	m_kPartySubName = rkInfo.m_kPartySubName;
	iPartyAttribute = rkInfo.iPartyAttribute;
	iPartyContinent = rkInfo.iPartyContinent;
	PartyMaxMember = rkInfo.PartyMaxMember;
	iPartyArea_NameNo = rkInfo.iPartyArea_NameNo;
	byPartyState = rkInfo.byPartyState;
}

////////////////////////////////////////////////////////////////////
// PgParty_Release

void PgParty_Release::DoAction( PgPlayer *pkPlayer, bool const bOnlyEffect )
{
	if ( pkPlayer && pkPlayer->HaveParty() )
	{
		GET_DEF(PgClassDefMgr, kClassDefMgr);

		int iClass = 1;
		while ( iClass < UCLASS_MAX )
		{
			int iPartyBuffNo = kClassDefMgr.GetAbil( SClassKey(iClass++, 1), AT_CLASS_PARTY_BUFF_01 );
			if ( 0 == iPartyBuffNo )
			{
				break;
			}

			pkPlayer->DeleteEffect( iPartyBuffNo );
		}

		if ( !bOnlyEffect )
		{
			pkPlayer->PartyGuid( BM::GUID::NullData() );
		}
	}
}

////////////////////////////////////////////////////////////////////
// Party Base

PgParty::PgParty()
:	m_pkPartyContents(NULL)
{
	Clear();
}

PgParty::~PgParty()
{
	DetachPartyContents();
}

void PgParty::Init(BM::GUID const &rkNewGuid, std::wstring const &rkNewName, SPartyOption const& kNewOption)
{
	BM::CAutoMutex kLock(m_kMutex);
	assert(BM::GUID::NullData() != rkNewGuid);
	//assert(!rkNewName.empty());

	Clear();

	m_kPartyGuid = rkNewGuid;
	m_kPartyName = rkNewName;
	m_kOption = kNewOption;

	Expedition(false);
}

void PgParty::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	//----Party Property
	m_kPartyGuid.Clear();
	m_kPartyName.clear();
	m_kMasterCharGuid.Clear();
	m_kOption.Default();

	//----Member
	m_kMemberArr.clear();

	//----Contents
	DetachPartyContents();
}

HRESULT PgParty::SetFriendState(BM::GUID const &rkCharGuid, bool bState)
{
	BM::CAutoMutex kLock(m_kMutex);

	ContPartyMember::iterator iter = m_kMemberArr.find(rkCharGuid);
	if( m_kMemberArr.end() != iter )
	{
		(bState) ? (*iter).second.iFriend++ : (*iter).second.iFriend--;		

		if( (*iter).second.iFriend > PV_MAX_MEMBER_CNT ) 
		{
			(*iter).second.iFriend = PV_MAX_MEMBER_CNT;
		}
		else if( (*iter).second.iFriend < 0 ) 
		{
			(*iter).second.iFriend = 0;
		}

		return PRC_Success;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMember"));
	return PRC_Fail_NotMember;
}

bool PgParty::SetFriendHomeAddr(BM::GUID const &rkCharGuid, SHOMEADDR const &rkHomeAddr)
{
	BM::CAutoMutex kLock(m_kMutex);

	ContPartyMember::iterator iter = m_kMemberArr.find(rkCharGuid);
	if( m_kMemberArr.end() != iter )
	{
		(*iter).second.kHomeAddr = rkHomeAddr;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int PgParty::GetFriendCount(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	ContPartyMember::iterator iter = m_kMemberArr.find(rkCharGuid);
	if( m_kMemberArr.end() != iter )
	{
		return (*iter).second.iFriend;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}	
}

bool PgParty::PartyFriendCheck(SPartyUserInfo& kMember)
{
	BM::CAutoMutex kLock(m_kMutex);

	ContPartyMember::iterator iter = m_kMemberArr.find(kMember.kCharGuid);
	if( m_kMemberArr.end() != iter )
	{
		(*iter).second.iFriend = kMember.iFriend;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgParty::Add(SPartyUserInfo const& rkPartyMember, bool const bMaster)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	if ( m_pkPartyContents )
	{
		HRESULT const hRet = m_pkPartyContents->IsJoin();
		if ( PRC_Success != hRet )
		{
			return hRet;
		}
	}

	if( PV_MAX_MEMBER_CNT == m_kMemberArr.size() )//파티 최대 인원수 체크
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount"));
		return PRC_Fail_MemberCount;//최대 인원 초과
	}

	auto kRet = m_kMemberArr.insert( std::make_pair(rkPartyMember.kCharGuid, rkPartyMember) );
	if( !kRet.second )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Duplicate"));
		return PRC_Fail_Duplicate;//중복
	}

	if( bMaster
	&&	BM::GUID::NullData() == MasterCharGuid() )
	{
		m_kMasterCharGuid = rkPartyMember.kCharGuid;//예가 마스터
	}
	
	return PRC_Success;
}

bool PgParty::Get(BM::GUID const &rkCharGuid, SPartyUserInfo* &pkOut)
{
	BM::CAutoMutex kLock(m_kMutex);
	ContPartyMember::iterator iter = m_kMemberArr.find(rkCharGuid);
	if( m_kMemberArr.end() != iter )
	{
		pkOut = &(iter->second);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgParty::Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool const bIsMaster = IsMaster(rkCharGuid);

	if ( m_pkPartyContents )
	{
		if ( true == m_pkPartyContents->Leave( rkCharGuid, bIsMaster ) )
		{
			DetachPartyContents();
		}
	}

	m_kMemberArr.erase(rkCharGuid);//무조건 삭제
	
	HRESULT hRet = PRC_Success;
	if( bIsMaster
	&&	MemberCount() )//삭제된 이후 카운트
	{
		bool bChangeRet = ChangeMaster(rkNewMaster);
		if( !bChangeRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bChangeRet is false"));
			assert(NULL);
		}
		hRet = PRC_Success_Master;
	}

	if( 0 == MemberCount() )
	{
		m_kMasterCharGuid = BM::GUID::NullData();	// 원정대 파티일 경우 삭제 되지 않고 남아있기 때문에 파티장의 GUID를 비워줘야 함.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Success_Destroy"));
		return PRC_Success_Destroy;//날 지워라
	}
	return hRet;
}

bool PgParty::GetRand(SPartyUserInfo* &pkOut)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !m_kMemberArr.size() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Party member is null") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iRand = BM::Rand_Index( static_cast<int>(m_kMemberArr.size()) );

	ContPartyMember::iterator iter = m_kMemberArr.begin();
	int iCur = 0;
	while(m_kMemberArr.end() != iter)
	{
		if( iCur == iRand )
		{
			ContPartyMember::mapped_type& rkElement = (*iter).second;
			pkOut = &rkElement;
			return true;
		}
		++iCur;
		++iter;
	}
	return true;
}

bool PgParty::Find(BM::GUID const &rkCharGuid) const
{
	ContPartyMember::const_iterator iter = m_kMemberArr.find(rkCharGuid);
	return (m_kMemberArr.end() != iter);
}

bool PgParty::ChangeMaster(BM::GUID const &rkNewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);
	bool const bRndChange = (BM::GUID::NullData() == rkNewMaster);

	if( !MemberCount() )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Cannot Party ChangeMaster because MemberCount == 0 Party[") << PartyGuid().str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SPartyUserInfo* pkNewMaster = NULL;
	if( !bRndChange )
	{
		bool const bFindMember = Get(rkNewMaster, pkNewMaster);//지정한 사람을
		if( !bFindMember )
		{
			GetRand(pkNewMaster);
		}
	}
	else//새로운 마스터를 지정하지 않으면
	{
		GetRand(pkNewMaster);//GUID정렬 순서대로 처음 유저
	}

	assert(pkNewMaster);
	if( pkNewMaster )
	{
		//pkNewMaster->bMaster = true;
		m_kMasterCharGuid = pkNewMaster->kCharGuid;
		return true;//성공
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;//실패
}


void PgParty::ReadFromPacket(BM::Stream& rkPacket)
{//맴버만 로딩한다.
	BM::CAutoMutex kLock(m_kMutex);
	m_kMemberArr.clear();//정상 맴버 초기화

	BYTE cSize = 0;
	rkPacket.Pop(cSize);

	for( BYTE i=0; cSize>i; ++i )
	{
		SPartyUserInfo kTemp;
		kTemp.ReadFromPacket(rkPacket);

		auto kRet = m_kMemberArr.insert(std::make_pair(kTemp.kCharGuid, kTemp));//강제 추가
		assert(kRet.second);
	}
	rkPacket.Pop(m_kPartyName);
	rkPacket.Pop(m_kMasterCharGuid);
	m_kOption.ReadFromPacket(rkPacket);
	rkPacket.Pop(m_kExpedition);
	rkPacket.Pop(m_kAllowEmpty);
}

void PgParty::WriteToPacket(BM::Stream& rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	rkPacket.Push( static_cast<BYTE>(m_kMemberArr.size()) );
	ContPartyMember::const_iterator iter = m_kMemberArr.begin();
	while(m_kMemberArr.end() != iter)
	{
		ContPartyMember::mapped_type const &rkElement = iter->second;
		rkElement.WriteToPacket(rkPacket);
		++iter;
	}
	rkPacket.Push(m_kPartyName);
	rkPacket.Push(m_kMasterCharGuid);
	m_kOption.WriteToPacket(rkPacket);
	rkPacket.Push(m_kExpedition);
	rkPacket.Push(m_kAllowEmpty);
}

void PgParty::GetMemberCharGuidList( VEC_GUID& rkOutCharGuid, BM::GUID const &rkIgnore ) const
{
	BM::CAutoMutex kLock(m_kMutex);

	rkOutCharGuid.clear();
	ContPartyMember::const_iterator iter = m_kMemberArr.begin();
	while(m_kMemberArr.end() != iter)
	{
		const ContPartyMember::mapped_type &rkElement = (*iter).second;
		if(rkIgnore != rkElement.kCharGuid)
		{
			rkOutCharGuid.push_back(rkElement.kCharGuid);
		}
		++iter;
	}
}

void PgParty::GetMemberGuidList( VEC_GUID& rkOutCharGuid, bool const bIsMemberGuid, short const nChannelNo, BM::GUID const &rkIgnoreCharGuid ) const
{
	BM::CAutoMutex kLock(m_kMutex);

	rkOutCharGuid.clear();
	ContPartyMember::const_iterator itr = m_kMemberArr.begin();
	for ( ; itr != m_kMemberArr.end() ; ++itr )
	{
		ContPartyMember::mapped_type const  &rkElement = itr->second;
		if ( nChannelNo == rkElement.kChnGndKey.Channel() )
		{
			if( rkIgnoreCharGuid != rkElement.kCharGuid)
			{
				if ( true == bIsMemberGuid )
				{
					rkOutCharGuid.push_back(rkElement.kMemberGuid);
				}
				else
				{
					rkOutCharGuid.push_back(rkElement.kCharGuid);
				}
			}
		}
	}
}

bool PgParty::GetMemberInfo(BM::GUID const &rkCharGuid, SPartyUserInfo& rkOutMember) const
{
	BM::CAutoMutex kLock(m_kMutex);
	ContPartyMember::const_iterator kIter = m_kMemberArr.find(rkCharGuid);
	if(m_kMemberArr.end() != kIter)
	{
		rkOutMember = kIter->second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgParty::GetMemberChnGndInfo( BM::GUID const &rkCharGuid, SChnGroundKey& rkOutChnGnd ) const
{
	BM::CAutoMutex kLock(m_kMutex);
	ContPartyMember::const_iterator member_itr = m_kMemberArr.find(rkCharGuid);
	if( member_itr != m_kMemberArr.end() )
	{
		rkOutChnGnd = member_itr->second.kChnGndKey;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgParty::WriteToMemberInfoList(BM::Stream& rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);

	VEC_UserInfo kVec_UserInfo;

	ContPartyMember::const_iterator iter = m_kMemberArr.begin();
	while(m_kMemberArr.end() != iter)
	{
		const ContPartyMember::mapped_type &rkElement = (*iter).second;
		kVec_UserInfo.push_back(rkElement);

		++iter;
	}
	PU::TWriteArray_M(rkPacket, kVec_UserInfo);//rkPacket.Push(kVec_UserInfo);
}

bool PgParty::GetMasterInfo(SPartyUserInfo& rkOutMaster)
{
	BM::CAutoMutex kLock(m_kMutex);
	return GetMemberInfo(MasterCharGuid(), rkOutMaster);
}

bool PgParty::IsMaster(BM::GUID const &rkCharGuid) const
{
	return (MasterCharGuid() == rkCharGuid);
}

bool PgParty::IsMember(BM::GUID const &rkCharGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return Find(rkCharGuid);
}

size_t PgParty::MemberCount() const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kMemberArr.size();
}

size_t PgParty::MaxMemberCount() const
{
	BM::CAutoMutex kLock(m_kMutex);
	int const MaxMember = m_kOption.GetOptionMaxMember();
	if( MaxMember <= 0 || MaxMember > PV_MAX_MEMBER_CNT )
	{
		return PV_MAX_MEMBER_CNT;
	}
	return MaxMember;
}

HRESULT PgParty::IsChangeMaster()const
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( m_pkPartyContents )
	{
		return m_pkPartyContents->IsChangeMaster();
	}
	return PRC_Success;
}

HRESULT PgParty::IsChangeOption()const
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( m_pkPartyContents )
	{
		return m_pkPartyContents->IsChangeOption();
	}
	return PRC_Success;
}

bool PgParty::MovedGnd( BM::GUID const &rkCharGuid, SChnGroundKey const &rkKey )
{
	BM::CAutoMutex kLock(m_kMutex);
	SPartyUserInfo* pkPartyMember = NULL;
	bool const bRet = Get( rkCharGuid, pkPartyMember );
	if( bRet )
	{
		pkPartyMember->kChnGndKey = rkKey;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

BYTE PgParty::ChangeAbility(BM::GUID const &rkCharGuid, BYTE const cType, BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	SPartyUserInfo* pkUserInfo = NULL;
	bool const bGetUser = Get(rkCharGuid, pkUserInfo);
	if( !bGetUser )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	SPartyUserInfo &rkMember = *pkUserInfo;
	BYTE cChangedAbil = 0;

	unsigned short sVal = 0;
	POINT3 ptPos;
	ptPos.Clear();

	if( 0 != (cType & PMCAT_Class) )
	{
		rkPacket.Pop( sVal );
		cChangedAbil |= (rkMember.iClass != sVal)? PMCAT_Class: 0;
		rkMember.iClass = sVal;
	}
	if( 0 != (cType & PMCAT_Level) )
	{
		rkPacket.Pop( sVal );
		cChangedAbil |= (rkMember.sLevel != sVal)? PMCAT_Level: 0;
		rkMember.sLevel = sVal;
	}
	if( 0 != (cType & PMCAT_HP) )
	{
		rkPacket.Pop( sVal );
		cChangedAbil |= (PMCA_SameGround <= abs(rkMember.sHP - sVal))? PMCAT_HP: 0;
		if( (PMCA_MAX_PERCENT != rkMember.sHP)//이전 HP가 Full이 아니면서
		&&	(PMCA_MAX_PERCENT == sVal) )//피가 꽉차면
		{
			cChangedAbil |= PMCAT_HP;
		}
		else if( (0 != rkMember.sHP)//이전 HP가 0이 아니면서
			  && (0 == sVal) )//HP가 0(죽음)이면
		{
			cChangedAbil |= PMCAT_HP;
		}
		//bChanged = PMCA_SameGround <= abs(rkMember.sHP - sVal);
		rkMember.sHP = sVal;
	}
	if( 0 != (cType & PMCAT_MP) )
	{
		rkPacket.Pop( sVal );
		cChangedAbil |= (PMCA_SameGround <= abs(rkMember.sMP - sVal))? PMCAT_MP: 0;
		if( (PMCA_MAX_PERCENT != rkMember.sMP)//이전이 Full이 아니면서
		&&	(PMCA_MAX_PERCENT == sVal) )//MP꽉
		{
			cChangedAbil |= PMCAT_MP;
		}
		else if( (0 != rkMember.sMP)//이전 MP가 0이 아니면서
			  && (0 == sVal) )//MP가 0
		{
			cChangedAbil |= PMCAT_MP;
		}
		rkMember.sMP = sVal;
	}
	if( 0 != (cType & PMCAT_POS) )
	{
		rkPacket.Pop( ptPos );	

		POINT3 const &rkCurPos = rkMember.ptPos;
		float const fDistance = POINT3::Distance(ptPos, rkCurPos);

		cChangedAbil |= (PMCA_SameParty <= abs(fDistance))? PMCAT_POS: 0;
		rkMember.ptPos = ptPos;
	}

	return cChangedAbil;
}

bool PgParty::WriteToChangedAbil(BM::GUID const &rkCharGuid, BYTE const cChangedAbil, BM::Stream &rkOut)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( 0 == cChangedAbil )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SPartyUserInfo* pkUserInfo = NULL;
	bool const bGetUser = Get(rkCharGuid, pkUserInfo);
	if( !bGetUser )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SPartyUserInfo &rkMember = *pkUserInfo;

	rkOut.Push( cChangedAbil );
	if( 0 != (cChangedAbil & PMCAT_Class) ) { rkOut.Push( (unsigned short)rkMember.iClass ); }
	if( 0 != (cChangedAbil & PMCAT_Level) ) { rkOut.Push( rkMember.sLevel ); }
	if( 0 != (cChangedAbil & PMCAT_HP) )	{ rkOut.Push( rkMember.sHP ); }
	if( 0 != (cChangedAbil & PMCAT_MP) )	{ rkOut.Push( rkMember.sMP ); }
	if( 0 != (cChangedAbil & PMCAT_POS) )	{ rkOut.Push( rkMember.ptPos ); }
	return true;
}

bool PgParty::AttachPartyContents( PgPartyContents * pkPartyContents )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( !m_pkPartyContents && pkPartyContents )
	{
		pkPartyContents->Init( *this );
		m_pkPartyContents = pkPartyContents;
		return true;
	}
	return false;
}

bool PgParty::DetachPartyContents()
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_pkPartyContents )
	{
		delete m_pkPartyContents;
		m_pkPartyContents = NULL;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////
// Global(Contents server) Party

PgGlobalParty::PgGlobalParty()
{
	Clear();
}

PgGlobalParty::~PgGlobalParty()
{
}

void PgGlobalParty::Init(BM::GUID const &rkNewGuid, std::wstring const &rkNewName, SPartyOption const& rkOption)
{
	BM::CAutoMutex kLock(m_kMutex);
	Clear();
	PgParty::Init(rkNewGuid, rkNewName, rkOption);
}

void PgGlobalParty::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	PgParty::Clear();

	//----Member
	m_kContWaitJoinList.clear();
}

HRESULT PgGlobalParty::AddWait(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( BM::GUID::IsNull(rkCharGuid) )
	{
		return PRC_Fail_MemberCount;
	}

	bool bMine = IsMember(rkCharGuid );
	if( bMine )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MineMember"));
		return PRC_Fail_MineMember;
	}

	if ( m_pkPartyContents )
	{
		HRESULT const hRet = m_pkPartyContents->IsInvite();
		if ( PRC_Success != hRet )
		{
			return hRet;
		}
	}

	if( MaxMemberCount() == MemberCount() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount"));
		return PRC_Fail_MemberCount;//인원수 제한으로 등록 불가
	}

	auto kRet = m_kContWaitJoinList.insert(rkCharGuid);// 파티에 가입하려는 유저를 대기자 명단에 올린다.
	if(kRet.second)
	{
		return PRC_Success;//새로운 추가
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Waiter"));
	return PRC_Fail_Waiter;//중복
}

bool PgGlobalParty::IsWait(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( BM::GUID::IsNull(rkCharGuid) )
	{
		return false;
	}

	ContWaitJoinList::iterator itor = m_kContWaitJoinList.find(rkCharGuid);
	if(m_kContWaitJoinList.end() != itor)
	{
		return true;// 등록되있다
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGlobalParty::DelWait(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	if(m_kContWaitJoinList.end() == m_kContWaitJoinList.find(rkCharGuid))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kContWaitJoinList.erase(rkCharGuid);// 파티에 가입하려는 유저를 대기자 명단에서 삭제
	//INFO_LOG(BM::LOG_LV7, _T("[%s] Party WaitingUser Deleted Party[%s], CharGuid[%s]"), __FUNCTIONW__, PartyGuid().str().c_str(),
	//	rkCharGuid.str().c_str());
	return true;
}

void PgGlobalParty::GetWaitCharGuidList(VEC_GUID& rkOutWaitGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	rkOutWaitGuid.clear();
	ContWaitJoinList::iterator itor = m_kContWaitJoinList.begin();
	while(m_kContWaitJoinList.end() != itor)
	{
		rkOutWaitGuid.push_back((*itor));
		++itor;
	}
}

bool PgGlobalParty::IsDestroy() const
{
	BM::CAutoMutex kLock(m_kMutex);
	size_t const iMemberCount = MemberCount();
	size_t const iWaiterCount = m_kContWaitJoinList.size();
	if( 0 == iWaiterCount//가입 대기자가 없고
	&&	1 == iMemberCount )//맴버가 1명이면
	{
		return true;//폭파
	}
	return false;//폭파 하지 마라
}

BYTE PgGlobalParty::ChangeAbility(BM::GUID const &rkCharGuid, BYTE const cType, BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	BYTE const cChanged = PgParty::ChangeAbility(rkCharGuid, cType, rkPacket);
	//if( 0 != (cChanged & PMCAT_???) )
	//{
	//	Some thing do;
	//}
	return cChanged;
}

HRESULT PgGlobalParty::SetFriendState(BM::GUID const &rkCharGuid, bool bState)
{
	BM::CAutoMutex kLock(m_kMutex);

	HRESULT const hAddRet = PgParty::SetFriendState(rkCharGuid, bState);

	return hAddRet;
}

int PgGlobalParty::GetFriendCount(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	return PgParty::GetFriendCount(rkCharGuid);
}

bool PgGlobalParty::PartyFriendCheck(SPartyUserInfo& kMember)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool bRet = PgParty::PartyFriendCheck(kMember);

	return bRet;
}

HRESULT PgGlobalParty::Add(SPartyUserInfo const& rkPartyMember, bool const bMaster)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( !bMaster//마스터가 아니면
	&&	!IsWait(rkPartyMember.kCharGuid) )//대기열에 없을 때 에러
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("PRC_Fail_NoWaiter"));
		return PRC_Fail_NoWaiter;
	}
	
	HRESULT const hAddRet = PgParty::Add(rkPartyMember, bMaster);
	if( PRC_Success == hAddRet )
	{
		DelWait(rkPartyMember.kCharGuid);
	}

	return PRC_Success;
}

HRESULT PgGlobalParty::Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( DelWait(rkCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("PRC_Success_Waiter"));
		return PRC_Success_Waiter;
	}
	HRESULT const hResult = PgParty::Del(rkCharGuid, rkNewMaster);
	return hResult;
}

bool PgGlobalParty::SetPartyMaster(BM::GUID const &rkCharGuid, BM::GUID &rkNewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);

	SPartyUserInfo rkOutMaster;

	bool bRet = GetMasterInfo(rkOutMaster);
	if( !bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iMasterGroundNo = rkOutMaster.GroundNo();

	if( m_kMemberArr.size() > 1 )
	{
		ContPartyMember::iterator iter = m_kMemberArr.begin();
		int const iMemberGroundNo = (*iter).second.GroundNo();
		while(m_kMemberArr.end() != iter)
		{
			if( iMasterGroundNo == iMemberGroundNo )
			{
				const ContPartyMember::mapped_type &rkElement = (*iter).second;

				rkNewMaster = rkElement.kCharGuid;
				if( rkNewMaster != rkCharGuid )
				{
					return true;
				}
			}
			++iter;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

////////////////////////////////////////////////////////////////////
// Local(Map server) Party
PgLocalParty::PgLocalParty()
{
	Clear();
}

PgLocalParty::~PgLocalParty()
{
}

void PgLocalParty::Init(BM::GUID const &rkNewGuid, std::wstring const &rkNewName, SPartyOption const& rkOption)
{
	BM::CAutoMutex kLock(m_kMutex);
	Clear();
	PgParty::Init(rkNewGuid, rkNewName, rkOption);
}

void PgLocalParty::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	PgParty::Clear();

	//----Indun
//	m_kIsWaitToPortal = false;// 인던으로 파티포탈을 기다리는중인가?
//	m_kIsNfyWaitToPortal = false;// 인던으로 파티포탈을 기다리라고 파티원들에게 통보했나?
//	m_kReqMapMoveInfo.Clear();
//	m_kPortalCancelUser.Clear();// 인던으로 파티포탈 취소한 유저의 캐릭터GUID
//	m_kReadyPortalTime = 0;// 인던포탈준비를 시작한 시간
}


void PgLocalParty::RefreshTotalLevel()
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kGndTotalLevel.clear();

	ContPartyMember::const_iterator iter = m_kMemberArr.begin();
	while(m_kMemberArr.end() != iter)
	{
		ContPartyMember::mapped_type const &rkElement = iter->second;
		auto kPair = m_kGndTotalLevel.insert( std::make_pair(rkElement.kChnGndKey, rkElement.sLevel) );
		if( !kPair.second )
		{
			kPair.first->second += rkElement.sLevel;//누적
		}

		++iter;
	}
}

bool PgLocalParty::GetGndTotalLevel( SChnGroundKey const &rkGndKey, unsigned int& iOutTotalLevel)const
{
	BM::CAutoMutex kLock(m_kMutex);
	ContGndTotalLevel::const_iterator iter = m_kGndTotalLevel.find(rkGndKey);
	if(m_kGndTotalLevel.end() != iter)
	{
		iOutTotalLevel = (*iter).second;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

// void PgLocalParty::WaitToPortal(bool const bWait, SReqMapMove_MT const& rkRMM)
// {	
// 	m_kIsWaitToPortal = bWait;
// 	if(!m_kIsWaitToPortal)
// 	{
// 		m_kIsNfyWaitToPortal = false;
// 		m_kReadyPortalTime = 0;
// 		m_kReqMapMoveInfo.Clear();
// 		return;
// 	}
// 	m_kReadyPortalTime = BM::GetTime32();
// 	m_kReqMapMoveInfo = rkRMM;
// 	m_kPortalCancelUser = BM::GUID::NullData();
// }

bool PgLocalParty::MovedGnd(BM::GUID const &rkCharGuid, SChnGroundKey const &rkKey)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( true == PgParty::MovedGnd( rkCharGuid, rkKey ) )
	{
		RefreshTotalLevel();//맵 이동시에도 각 맵당 전체 레벨
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgLocalParty::GetLocalMemberList( SChnGroundKey const &rkGndKey, VEC_GUID& rkOutVec, BM::GUID const &rkIgnoreGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);
	ContPartyMember::const_iterator iter = m_kMemberArr.begin();
	while(m_kMemberArr.end() != iter)
	{
		ContPartyMember::mapped_type const &rkElement = iter->second;
		if( rkGndKey == rkElement.kChnGndKey
		&& rkIgnoreGuid != rkElement.kCharGuid )
		{
			rkOutVec.push_back(rkElement.kCharGuid);
		}
		++iter;
	}
	if(0 != rkOutVec.size())
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int PgLocalParty::GetLocalMemberFriend(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	ContPartyMember::const_iterator iter = m_kMemberArr.find(rkCharGuid);
	if( m_kMemberArr.end() != iter )
	{
		return (*iter).second.iFriend;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

BYTE PgLocalParty::ChangeAbility(BM::GUID const &rkCharGuid, BYTE const cType, BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	BYTE const cChanged = PgParty::ChangeAbility(rkCharGuid, cType, rkPacket);
	if( 0 != (cChanged & PMCAT_Level) )
	{
		RefreshTotalLevel();//파티원 레벨이 변할때 Refresh
	}
	return cChanged;
}

HRESULT PgLocalParty::Add(SPartyUserInfo const& rkPartyMember, bool const bMaster)
{
	BM::CAutoMutex kLock(m_kMutex);

	HRESULT const hAddRet = PgParty::Add(rkPartyMember, bMaster);
	if( PRC_Success == hAddRet )
	{
		RefreshTotalLevel();//총합 레벨 업데이트
	}
	return hAddRet;
}

HRESULT PgLocalParty::Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster)
{
	BM::CAutoMutex kLock(m_kMutex);
	HRESULT const hResult = PgParty::Del(rkCharGuid, rkNewMaster);
	RefreshTotalLevel();//총합 레벨 업데이트
	return hResult;
}

void PgLocalParty::ReadFromPacket(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	PgParty::ReadFromPacket(rkPacket);
	RefreshTotalLevel();
}
