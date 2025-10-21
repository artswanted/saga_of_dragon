#include "stdafx.h"
#include "PgGuild.h"
#include "PgServerSetMgr.h"
#include "Item/PgPostManager.h"


PgGuild::PgGuild()
{
	Clear();
}

PgGuild::~PgGuild()
{
}

void PgGuild::Clear()
{
	m_kBasicInfo.Clear();
	m_kGrade.clear();
	m_kMember.clear();
	m_kMySkill.Init();
	m_kEmporiaInfo.Clear();
	m_kInv.Clear();

	m_kContGuildInventoryObserver.clear();
	m_kContFixingItemPos.clear();
}

HRESULT PgGuild::ChangePos(SContentsUser const &rkUser)
{
	ContGuildMember::iterator member_itr = m_kMember.find(rkUser.kCharGuid);
	if( member_itr == m_kMember.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ContGuildMember::mapped_type &rkMemberInfo = member_itr->second;

	if( rkMemberInfo.kGndKey == rkUser.kGndKey
	&&	rkMemberInfo.sChannel == rkUser.sChannel )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
		return S_FALSE;
	}

	rkMemberInfo.kGndKey = rkUser.kGndKey;
	rkMemberInfo.sChannel = rkUser.sChannel;
	rkMemberInfo.kCharName = rkUser.kName; // 이름 변경을 위해서
	return S_OK;
}

bool PgGuild::WriteMemberListToPacket( BM::Stream& rkPacket, BM::GUID const &rkCharGuid )const
{
	if ( rkCharGuid == BM::GUID::NullData() )
	{
		PU::TWriteTable_MM( rkPacket, m_kMember );
	}
	else
	{
		ContGuildMember::const_iterator kMember_itr = m_kMember.find(rkCharGuid);
		if ( m_kMember.end() == kMember_itr )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		rkPacket.Push((size_t)1);	
		kMember_itr->first.WriteToPacket(rkPacket);
		kMember_itr->second.WriteToPacket(rkPacket);
	}
	return true;
}

void PgGuild::BroadCast(BM::Stream const& rkPacket, BM::GUID const &rkIgnoreGuid, BYTE byGrade)const
{
	VEC_GUID kVec;
	ContGuildMember::const_iterator kMember_itr = m_kMember.begin();
	while(m_kMember.end() != kMember_itr)
	{
		const ContGuildMember::mapped_type &rkElement = (*kMember_itr).second;
		//if( 0 != rkElement.iNowPos
		if( rkElement.kCharGuid != rkIgnoreGuid
		&& rkElement.cGrade <= byGrade )
		{
			std::back_inserter(kVec) = rkElement.kCharGuid;
		}
		++kMember_itr;
	}

	if( !kVec.empty() )
	{
		g_kRealmUserMgr.Locked_SendToUser(kVec, rkPacket, false);
	}
}

void PgGuild::Set(const SGuildBasicInfo& rkBasicInfo)
{
	m_kBasicInfo = rkBasicInfo;
	m_kMySkill.Add(MAX_DB_GUILD_SKILL_SIZE, m_kBasicInfo.cGuildSkill);

	// 길드인벤
	if( 0 != m_kBasicInfo.abyInvExtern[1] 
	||	0 != m_kBasicInfo.abyInvExternIdx[1] )
	{
		m_kInv.Init( m_kBasicInfo.abyInvExtern , m_kBasicInfo.abyInvExternIdx );
		m_kInv.OwnerGuid(m_kBasicInfo.kGuildGuid);
		m_kInv.Money(m_kBasicInfo.i64Money);
	}
}
//void PgGuild::AddDBFlag(DWORD dwFlag)
//{
//	m_dwDBFlag |= dwFlag;
//}

//bool PgGuild::IsDBFlag(DWORD dwFlag)
//{
//	return ((dwFlag & m_dwDBFlag) == dwFlag);
//}

void PgGuild::WriteInfoToPacket(BM::Stream& rkPacket) const
{
	m_kBasicInfo.WriteToPacket(rkPacket);
	m_kSetMercenary.WriteToPacket(rkPacket);
	m_kSetGuildEntranceOpen.WriteToPacket(rkPacket);
	rkPacket.Push( m_kNotice );
	rkPacket.Push( m_kEmporiaInfo );
}

void PgGuild::WriteListToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push((BYTE)m_kGrade.size());
	ContGuildGrade::const_iterator kGrade_iter = m_kGrade.begin();
	while(m_kGrade.end() != kGrade_iter)
	{
		kGrade_iter->WriteToPacket(rkPacket);
		++kGrade_iter;
	}

	WriteMemberListToPacket(rkPacket);
}

bool PgGuild::AddGradeGroup(SGuildMemberGradeInfo const &rkGradeGroup)
{
	ContGuildGrade::iterator kGrade_iter = m_kGrade.begin();
	while(m_kGrade.end() != kGrade_iter)
	{
		if(kGrade_iter->cGradeGroupLevel == rkGradeGroup.cGradeGroupLevel)
		{
			//INFO_LOG(BM::LOG_LV5, _T("[%s]-[%d] [%s] Duplicate 'Guild Grade Name Group' in Guild"), __FUNCTIONW__, __LINE__, m_kBasicInfo.kGuildName.c_str());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		++kGrade_iter;
	}

	m_kGrade.push_back(rkGradeGroup);
	return true;
}

bool PgGuild::Add(SContentsUser const& rkUser, int iGrade)
{
	SGuildMemberInfo kMemberInfo(rkUser.Name(), rkUser.kCharGuid, (BYTE)iGrade);
	kMemberInfo.sLevel = rkUser.sLevel;
	kMemberInfo.cClass = (BYTE)rkUser.iClass;
	kMemberInfo.kGndKey = rkUser.kGndKey;
	kMemberInfo.kHomeAddr = rkUser.kHomeAddr;
	return Add(kMemberInfo);
}

bool PgGuild::Add(SGuildMemberInfo const& rkGuildMember)
{
	//BM::CAutoMutex kLock(m_kMutex);

	SGuildMemberInfo kTempInfo = rkGuildMember;

	if(GMG_Membmer<kTempInfo.cGrade || 0>kTempInfo.cGrade) {kTempInfo.cGrade = GMG_Membmer;};//범위를 벗어나면 일반 맴버
	if(m_kBasicInfo.kMasterGuid == kTempInfo.kCharGuid) {kTempInfo.cGrade = GMG_Owner;};//마스터일 경우

	auto ibRet = m_kMember.insert(std::make_pair(kTempInfo.kCharGuid, kTempInfo));
	if(ibRet.second)
	{
		//Success
		return true;
	}

	//INFO_LOG(BM::LOG_LV5, _T("[%s]-[%d] Can't add member in guild[%s]"), __FUNCTIONW__, __LINE__, Guid().str().c_str());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuild::UpdateAbil(BM::GUID const &rkCharGuid, WORD const wAbilType, int const iUpdatedVal)
{
	ContGuildMember::iterator kMember_iter = m_kMember.find(rkCharGuid);
	if(m_kMember.end() == kMember_iter)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Cannot find Guild[") << Guid().str().c_str() << _T("], Member[") << rkCharGuid.str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SGuildMemberInfo &rkMemberInfo = kMember_iter->second;

	switch( wAbilType )
	{
	case AT_LEVEL:
		{
			rkMemberInfo.sLevel = (unsigned short)iUpdatedVal;
		}break;
	case AT_CLASS:
		{
			rkMemberInfo.cClass = (BYTE)iUpdatedVal;
		}break;
	default:
		{
		}break;
	}
	return true;
}

bool PgGuild::IsMember(BM::GUID const &rkCharGuid) const
{
	ContGuildMember::const_iterator iter = m_kMember.find(rkCharGuid);
	return m_kMember.end() != iter;
}

bool PgGuild::WriteMemberPacket(BM::GUID const &rkCharGuid, BM::Stream& rkPacket) const
{
	SGuildMemberInfo const* pkMember = NULL;
	bool const bFindMemberRet = Get(rkCharGuid, pkMember);
	if( bFindMemberRet )
	{
		pkMember->WriteToPacket(rkPacket);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuild::Set(BM::GUID const &rkGuildGuid, BM::GUID const &rkMasterGuid, std::wstring const& rkGuildName, BM::PgPackedTime const& rkCreateTime, BYTE const cEmblem)
{
	//생성시 진입
	Clear();

	m_kBasicInfo.kGuildGuid = rkGuildGuid;
	m_kBasicInfo.kMasterGuid = rkMasterGuid;
	m_kBasicInfo.kGuildName = rkGuildName;
	m_kBasicInfo.kCreateDate = rkCreateTime;
	m_kBasicInfo.sLevel = 1;//기본 레벨1
	m_kBasicInfo.cEmblem = cEmblem;

	//기본 스킬 포인트 설정
	CONT_DEF_GUILD_LEVEL const *pkDefGuildLevel = NULL;
	g_kTblDataMgr.GetContDef(pkDefGuildLevel);
	if( pkDefGuildLevel )
	{
		CONT_DEF_GUILD_LEVEL::const_iterator def_iter = pkDefGuildLevel->find(GuildLevel());
		if( pkDefGuildLevel->end() != def_iter )
		{
			m_kBasicInfo.sSkillPoint += (*def_iter).second.iSkillPoint;
		}
	}

	//기본 스킬 설정 필요
	return true;
}

void PgGuild::GetGuildSkill(ContSkillNo &rkOut) const
{
	m_kMySkill.GetHaveSkills(SDT_Guild, rkOut);
}

void PgGuild::WriteSkillToPacket(BM::Stream &rkPacket)const
{
	ContSkillNo kSkillNo;
	GetGuildSkill(kSkillNo);
	rkPacket.Push(kSkillNo);
}

bool PgGuild::Get(BM::GUID const &rkCharGuid, SGuildMemberInfo &rkOut) const
{
	SGuildMemberInfo const *pkOut = NULL;
	if( Get(rkCharGuid, pkOut) )
	{
		rkOut = *pkOut;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuild::Get(BM::GUID const &rkCharGuid, SGuildMemberInfo* &pkOut)
{
	ContGuildMember::iterator kMember_iter = m_kMember.find(rkCharGuid);
	if(m_kMember.end() != kMember_iter)
	{
		ContGuildMember::mapped_type& rkElement = (*kMember_iter).second;
		pkOut = &rkElement;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuild::Get(BM::GUID const &rkCharGuid, SGuildMemberInfo const* &pkOut) const
{
	ContGuildMember::const_iterator kMember_iter = m_kMember.find(rkCharGuid);
	if(m_kMember.end() != kMember_iter)
	{
		const ContGuildMember::mapped_type& rkElement = (*kMember_iter).second;
		pkOut = &rkElement;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuild::Del(BM::GUID const &rkCharGuid)
{
	ContGuildMember::iterator kMember_iter = m_kMember.find(rkCharGuid);
	if(m_kMember.end() != kMember_iter)
	{
		m_kMember.erase(kMember_iter);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuild::IsOwner(BM::GUID const &rkCharGuid) const
{
	return Master() == rkCharGuid;
}

HRESULT PgGuild::SetGradeGroup(const SGuildMemberGradeInfo& rkGradeGroup)
{
	//BM::CAutoMutex kLock(m_kMutex);
	ContGuildGrade::iterator kGrade_iter = m_kGrade.begin();
	while(m_kGrade.end() != kGrade_iter)
	{
		if(kGrade_iter->cGradeGroupLevel == rkGradeGroup.cGradeGroupLevel)
		{
			//int const iDiffDay = kGrade_iter->kLastModifyDate.Day() - rkGradeGroup.kLastModifyDate.Day();
			//if( 7 < iDiffDay )
			{
				*kGrade_iter = rkGradeGroup;
				return S_OK;
			}
		}
		++kGrade_iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgGuild::SetMember(BM::GUID const &rkCharGuid, int iGrade)
{
	ContGuildMember::iterator kMember_iter = m_kMember.find(rkCharGuid);
	if(m_kMember.end() != kMember_iter)
	{
		ContGuildMember::mapped_type& rkElement = kMember_iter->second;
		rkElement.cGrade = (BYTE)iGrade;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuild::SetMemberHomeAddr(BM::GUID const &rkCharGuid, SHOMEADDR const kHomeAddr)
{
	ContGuildMember::iterator kMember_iter = m_kMember.find(rkCharGuid);
	if(m_kMember.end() != kMember_iter)
	{
		ContGuildMember::mapped_type& rkElement = kMember_iter->second;
		rkElement.kHomeAddr = kHomeAddr;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//bool PgGuild::SetTaxRate(int iPercent) // n/100
//{
//	if(GE_TaxRate_Min > iPercent) iPercent = GE_TaxRate_Min;
//	if(GE_TaxRate_Max < iPercent) iPercent = GE_TaxRate_Max;
//	m_kBasicInfo.sTaxRate = (unsigned short)iPercent;
//	return true;
//}

void PgGuild::SetExp( __int64 const iExp )
{
	m_kBasicInfo.iExperience = iExp;
}

bool PgGuild::UseExp( __int64 const iUseExp )
{
	if ( iUseExp > 0i64 )
	{
		if ( m_kBasicInfo.iExperience >= iUseExp )
		{
			m_kBasicInfo.iExperience -= iUseExp;
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

EGuildCommandRet PgGuild::AddExp(__int64 const iAddExp)
{
	if( !iAddExp )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_None"));
		return GCR_None;
	}

	const CONT_DEF_GUILD_LEVEL *pkDefGuildLevel = NULL;
	g_kTblDataMgr.GetContDef(pkDefGuildLevel);
	if( !pkDefGuildLevel )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_None"));
		return GCR_None;
	}

	CONT_DEF_GUILD_LEVEL::const_iterator def_iter = pkDefGuildLevel->find(m_kBasicInfo.sLevel);//현재 레벨
	if( pkDefGuildLevel->end() == def_iter )
	{
		// 현재 레벨이 없다
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_None"));
		return GCR_None;
	}

	EGuildCommandRet eRet = GCR_None;

	float const fMaxAccumRate = 1.2f;
	__int64 iAddedExp = std::max(m_kBasicInfo.iExperience + iAddExp, 0i64);
	__int64 iNextExp = __int64((*def_iter).second.iExp * fMaxAccumRate);	// 지금이 설정상 최고 레벨
	CONT_DEF_GUILD_LEVEL::const_iterator nextdef_iter = pkDefGuildLevel->find(m_kBasicInfo.sLevel+1);//다음 레벨
	if( pkDefGuildLevel->end() != nextdef_iter )
	{
		iNextExp = __int64((*nextdef_iter).second.iExp * fMaxAccumRate);	// 설정상 다음레벨이 존재
	}
	if( 0 > iNextExp
	||	_I64_MAX < iNextExp ) // check over flow
	{
		iNextExp = _I64_MAX;
	}

	if( 0 > iAddedExp
	||	iNextExp < iAddedExp ) // check over flow
	{
		iAddedExp = iNextExp;
	}

	m_kBasicInfo.iExperience = iAddedExp;

	//int const iPercent = 100;
	//__int64 const iCurPercent = __min(m_kBasicInfo.iExperience / iNextExp * iPercent, iPercent);
	//__int64 const iChangedPercent = __min(iAddedExp / iNextExp * iPercent, iPercent);
	//if( iCurPercent != iChangedPercent ) // 100 단계로 변경치 알림
	//{
	//	return GCR_Notify; // 경험치 바뀐걸 알려줘라
	//}
	return GCR_Success; // 경험치 바뀐걸 알려주지 말아라
}

EGuildCommandRet PgGuild::Levelup(bool const bTest)
{
	const CONT_DEF_GUILD_LEVEL *pkDefGuildLevel = NULL;
	g_kTblDataMgr.GetContDef(pkDefGuildLevel);
	if( !pkDefGuildLevel )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	CONT_DEF_GUILD_LEVEL::const_iterator def_iter = pkDefGuildLevel->find(m_kBasicInfo.sLevel);//현재 레벨
	if( pkDefGuildLevel->end() == def_iter )
	{
		//현재 레벨이 없다
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	CONT_DEF_GUILD_LEVEL::const_iterator nextdef_iter = pkDefGuildLevel->find(m_kBasicInfo.sLevel+1);//다음 레벨
	if( pkDefGuildLevel->end() == nextdef_iter )
	{
		//다음 레벨이 없다(== 지금이 설정상 최고 레벨)
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Max"));
		return GCR_Max;
	}

	const CONT_DEF_GUILD_LEVEL::mapped_type &rkDefGuildLevel = (*nextdef_iter).second;
	if( rkDefGuildLevel.iExp > m_kBasicInfo.iExperience )
	{
		//경험치 모질라
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotEnoughExp"));
		return GCR_NotEnoughExp;
	}

	if( !bTest )
	{
		//레벨업 했다
		++m_kBasicInfo.sLevel;
		m_kBasicInfo.iExperience -= rkDefGuildLevel.iExp;
		m_kBasicInfo.sSkillPoint += rkDefGuildLevel.iSkillPoint;
	}
	return GCR_Success;
}

EGuildCommandRet PgGuild::SetLevel(int const iLv)
{
	if(m_kBasicInfo.sLevel == iLv)
	{
		return GCR_Same;
	}

	const CONT_DEF_GUILD_LEVEL *pkDefGuildLevel = NULL;
	g_kTblDataMgr.GetContDef(pkDefGuildLevel);
	if( !pkDefGuildLevel )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	CONT_DEF_GUILD_LEVEL::const_iterator def_iter = pkDefGuildLevel->find(iLv);//현재 레벨
	if( pkDefGuildLevel->end() == def_iter )
	{
		//현재 레벨이 없다
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	const CONT_DEF_GUILD_LEVEL::mapped_type &rkDefGuildLevel = (*def_iter).second;
	
	//레벨업 했다
	if(m_kBasicInfo.sLevel < iLv)
	{
		m_kBasicInfo.sSkillPoint +=  rkDefGuildLevel.iSkillPoint;
	}
	m_kBasicInfo.sLevel = static_cast<unsigned short>(iLv);
	return GCR_Success;
}

bool PgGuild::GetMemberGuid(VEC_GUID& rkVec, BM::GUID const &rkExcludeGuid) const
{
	ContGuildMember::const_iterator kMember_iter = m_kMember.begin();
	while(m_kMember.end() != kMember_iter)
	{
		const ContGuildMember::mapped_type &rkElement = kMember_iter->second;
		if(rkElement.kCharGuid != rkExcludeGuid)
		{
			rkVec.push_back(rkElement.kCharGuid);
		}
		++kMember_iter;
	}

	return !m_kMember.empty();
}

EGuildCommandRet PgGuild::ChangeOwner(BM::GUID const &rkNewOwner, bool bTest)
{
	SGuildMemberInfo *pkMemberInfo = NULL, *pkMasterInfo = NULL;

	bool const bGetMasterRet = Get( Master(), pkMasterInfo);
	bool const bGetMemberRet = Get(rkNewOwner, pkMemberInfo);
	if( !pkMemberInfo
	||	!pkMasterInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	if( bTest )
	{
		pkMemberInfo->cGrade = GMG_Owner;
		pkMasterInfo->cGrade = GMG_Membmer;
		
		m_kBasicInfo.kMasterGuid = rkNewOwner;
	}
	return GCR_Success;
}

EGuildCommandRet PgGuild::ChangeEmblem(BYTE const cNewEmblem)
{
	BYTE cTemp = cNewEmblem;
	if( cMaxGuildEmblem <= cNewEmblem )
	{
		cTemp = BM::Rand_Range(cMaxGuildEmblem);
	}

	if( m_kBasicInfo.cEmblem != cTemp )
	{
		m_kBasicInfo.cEmblem = cTemp;
	}
	return GCR_Success;
}

bool PgGuild::IsMaster(BM::GUID const &rkCharGuid) const
{
	if( IsOwner(rkCharGuid) )
	{
		return true;
	}

	ContGuildMember::const_iterator kMember_iter = m_kMember.find(rkCharGuid);
	if(m_kMember.end() != kMember_iter)
	{
		const ContGuildMember::mapped_type& rkElement = kMember_iter->second;
		return (rkElement.cGrade<=GMG_Master);//마스터 이상이면
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//bool PgGuild::SetEmblem()
//{
//	m_kBasicInfo.cEmblem = 1;
//	return true;
//}

EGuildCommandRet PgGuild::AddSkill(int const iSkillNo, bool const bTest)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if( 0 < iSkillNo
	&&	pkSkillDef )
	{
		int const iType = pkSkillDef->GetAbil(AT_TYPE);
		int const iDivideType = pkSkillDef->GetAbil(AT_SKILL_DIVIDE_TYPE);

		if( SDT_Guild != iDivideType )
		{
			assert(false);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
			return GCR_Failed;
		}

		CONT_DEF_GUILD_SKILL const *pkDefGuildSkill = NULL;
		g_kTblDataMgr.GetContDef(pkDefGuildSkill);
		if( !pkDefGuildSkill )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
			return GCR_Failed;
		}

		CONT_DEF_GUILD_SKILL::const_iterator skilldef_iter = pkDefGuildSkill->find(iSkillNo);
		if( pkDefGuildSkill->end() == skilldef_iter )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
			return GCR_Failed;
		}

		CONT_DEF_GUILD_SKILL::mapped_type const &rkGuildSkillDef = (*skilldef_iter).second;
		//if( rkGuildSkillDef.iSkillPoint
		//&&	rkGuildSkillDef.iSkillPoint > SkillPoint() )
		//{
		//	return GCR_Point;
		//}

		if( rkGuildSkillDef.iGuildExp > m_kBasicInfo.iExperience )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotEnoughExp"));
			return GCR_NotEnoughExp;
		}

		if( rkGuildSkillDef.sGuildLv > GuildLevel() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Level"));
			return GCR_Level;
		}

		if( m_kMySkill.IsExist(iSkillNo) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Pre"));
			return GCR_Pre;
		}

		if( !bTest )
		{
			m_kMySkill.LearnNewSkill(iSkillNo);
			//if( rkGuildSkillDef.iSkillPoint )
			//{
			//	m_kBasicInfo.sSkillPoint -= rkGuildSkillDef.iSkillPoint;
			//}
			if( rkGuildSkillDef.iGuildExp )
			{
				m_kBasicInfo.iExperience -= rkGuildSkillDef.iGuildExp;
			}
		}
		return GCR_Success;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
	return GCR_Failed;
}

void PgGuild::WriteToSavePacket(BM::Stream &rkPacket)
{
	static ContSkillDivideType kDefaultSaveSet;
	if( kDefaultSaveSet.empty() )
	{
		std::inserter(kDefaultSaveSet, kDefaultSaveSet.end()) = SDT_Guild;
	}

	rkPacket.Push(m_kBasicInfo.sLevel);
	rkPacket.Push(m_kBasicInfo.iExperience);
	rkPacket.Push(m_kBasicInfo.cEmblem);
	rkPacket.Push(m_kBasicInfo.sSkillPoint);
	m_kMySkill.Save(MAX_DB_GUILD_SKILL_SIZE, m_kBasicInfo.cGuildSkill, kDefaultSaveSet);
	rkPacket.Push(m_kBasicInfo.cGuildSkill);
}

EGuildCommandRet PgGuild::TestSetGrade(BM::GUID const& rkMasterGuid, BM::GUID const& rkCharGuid, BYTE const cTargetGrade)
{
	if( !IsOwner(rkMasterGuid ) )
	{
		return GCR_NotAuth;
	}

	int const iMaxMasterCount = 2; // 부길마는 최대 2명
	switch( cTargetGrade )
	{
	case GMG_Owner:
		{
			return GCR_Max;
		}break;
	case GMG_Master:
		{
			int iCount = 0;
			ContGuildMember::const_iterator iter = m_kMember.begin();
			while( m_kMember.end() != iter )
			{
				if( (*iter).second.cGrade == cTargetGrade )
				{
					++iCount;
				}
				++iter;
			}

			if( iMaxMasterCount <= iCount )
			{
				return GCR_Max;
			}
		}break;
	default:
		{
		}break;
	};

	SGuildMemberInfo const* pkMemberInfo = NULL;
	if( !Get(rkCharGuid, pkMemberInfo) )
	{
		return GCR_Member;
	}

	return GCR_Success;
}

void PgGuild::SendMail( BYTE const byGrade, std::wstring const &wstrSender, std::wstring const &wstrTitle, std::wstring const &wstrContents )const
{
	ContGuildMember::const_iterator itr = m_kMember.begin();
	for ( ; itr != m_kMember.end() ; ++itr )
	{
		ContGuildMember::mapped_type const &kElement = itr->second;
		if ( byGrade >= kElement.cGrade )
		{
			g_kPostMgr.PostSystemMail( kElement.kCharGuid, wstrSender, kElement.kCharName, wstrTitle, wstrContents, 0, 0, 0i64 );
		}
	}
}

void PgGuild::SetGuildInv( BYTE const * pkInvExtern, BYTE const * pkExternIdx )
{
	::memcpy(m_kBasicInfo.abyInvExtern, pkInvExtern, MAX_DB_INVEXTEND_SIZE );
	::memcpy(m_kBasicInfo.abyInvExternIdx, pkExternIdx, MAX_DB_INVEXTEND_SIZE );
}

void PgGuild::AddGuildInventoryObserver(const BM::GUID& rkCharGuid)
{
	m_kContGuildInventoryObserver.insert(rkCharGuid);
}

void PgGuild::RemoveGuildInventoryObserver(const BM::GUID& rkCharGuid)
{
	CONT_GUILD_INVENTORY_OBSERVER::iterator find_iter = m_kContGuildInventoryObserver.find(rkCharGuid);
	if( m_kContGuildInventoryObserver.end() != find_iter )
	{
		m_kContGuildInventoryObserver.erase(find_iter);
	}
}

void PgGuild::SendGuildInventory(const BM::GUID& rkCharGuid, const BYTE byInvType)
{
	BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast<BYTE>(GC_InventoryOpen));
	kPacket.Push(static_cast<BYTE>(GCR_Success));
	kPacket.Push(byInvType);
	m_kInv.WriteToPacket(static_cast<EInvType>(byInvType), kPacket );
	kPacket.Push( m_kInv.Money() );
	
	g_kRealmUserMgr.Locked_SendToUser( rkCharGuid, kPacket, false );
}

void PgGuild::BroadCastGuildInventoryObserverForUpdateItem(const DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, const std::wstring &rkCharName)
{
	BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast<BYTE>(GC_InventoryUpdate));//길드금고 아이템에 변화가 생김
	kPacket.Push(static_cast<BYTE>(GCR_Success));
	kPacket.Push(rkCharName);
	PU::TWriteArray_M(kPacket, rkChangeArray); // 변경이 일어난 아이템들
	
	{//길드금고를 오픈한 길드원들에게 브로드캐스트
		CONT_GUILD_INVENTORY_OBSERVER::const_iterator loop_iter = m_kContGuildInventoryObserver.begin();
		while( m_kContGuildInventoryObserver.end() != loop_iter )
		{
			CONT_GUILD_INVENTORY_OBSERVER::value_type kCharGuid = (*loop_iter);
			g_kRealmUserMgr.Locked_SendToUser( kCharGuid, kPacket, false );
			++loop_iter;
		}
	}
	
	{// 길드 금고 수정, 전송 완료 했다.(수정중인 목록에서 없애 준다)
		DB_ITEM_STATE_CHANGE_ARRAY::const_iterator loop_iter = rkChangeArray.begin();
		while( rkChangeArray.end() != loop_iter )
		{
			DB_ITEM_STATE_CHANGE_ARRAY::value_type kElement = (*loop_iter);
			SItemPos kPos = kElement.ItemWrapper().Pos();
			RemoveFixingItem(kPos);

			++loop_iter;
		}
	}
}

bool PgGuild::IsHaveGuildInvAuthority_In(const BM::GUID& rkCharGuid)
{
	ContGuildMember::const_iterator kMember_itr = m_kMember.find(rkCharGuid);
	if( m_kMember.end() != kMember_itr )
	{
		ContGuildMember::mapped_type const &kElement = kMember_itr->second;

		const BYTE byAuthority = GetGuildInvAuthority_In();		
		switch( byAuthority )
		{
		case GMG_Owner:		{ return IsOwner(rkCharGuid); }break;
		case GMG_Master:	{ return IsMaster(rkCharGuid); }break;
		default:			{ return true; } break;
		}
	}

	return false;
}

bool PgGuild::IsHaveGuildInvAuthority_Out(const BM::GUID& rkCharGuid)
{
	ContGuildMember::const_iterator kMember_itr = m_kMember.find(rkCharGuid);
	if( m_kMember.end() != kMember_itr )
	{
		ContGuildMember::mapped_type const &kElement = kMember_itr->second;
		
		const BYTE byAuthority = GetGuildInvAuthority_Out();
		switch( byAuthority )
		{
		case GMG_Owner:		{ return IsOwner(rkCharGuid); }break;
		case GMG_Master:	{ return IsMaster(rkCharGuid); }break;
		default:			{ return true; } break;
		}
	}

	return false;
}

void PgGuild::BroadCastGuildInventoryExtend( HRESULT hRet, const __int64 iCause, const EInvType kInvType, const BYTE byExtendSize )
{
	BM::Stream kPacket(PT_N_C_NFY_GUILD_INV_EXTEND);
	kPacket.Push(hRet); // 성공
	kPacket.Push( iCause );
	kPacket.Push( kInvType );
	kPacket.Push( byExtendSize );

	ContGuildMember::const_iterator loop_iter = m_kMember.begin();
	while( m_kMember.end() != loop_iter )
	{
		ContGuildMember::mapped_type const &kElement = loop_iter->second;
		g_kRealmUserMgr.Locked_SendToUser(kElement.kCharGuid, kPacket, false);
		
		++loop_iter;
	}
}

bool PgGuild::AddFixingItem(const SItemPos& rkPos)
{
	auto bSuccess = m_kContFixingItemPos.insert(rkPos);
	if( bSuccess.second )
	{
		return true;
	}

	return false;
}

bool PgGuild::AddFixingItem(const SItemPos& rkCasterPos, const SItemPos& rkTargetPos)
{
	auto bSuccess = m_kContFixingItemPos.insert(rkCasterPos);
	if( !bSuccess.second )
	{
		return false;
	}

	bSuccess = m_kContFixingItemPos.insert(rkTargetPos);
	if( !bSuccess.second )
	{
		m_kContFixingItemPos.erase(rkCasterPos);
		return false;
	}

	return true;
}

void PgGuild::RemoveFixingItem(const SItemPos& rkPos)
{	
	m_kContFixingItemPos.erase(rkPos);
}