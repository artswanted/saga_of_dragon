#include "stdafx.h"
#include "ItemBagMgr.h"
#include "Lohengrin/PacketType.h"
#include "PgMission.h"
#include "Lohengrin/LogGroup.h"
#include "tabledatamanager.h"

WORD const PgMission::ms_kGradeValue[EMGRADE_MONSTERMAX] = {30,60,200,200}; // ���� ��޺� ����
int const PgMission::ms_aiDownRank[MISSION_DOWN] = {0, 0, 0, 2, 3, 4, 5};


PgMission::PgMission()
{
	Clear();
}

PgMission::~PgMission()
{
}

PgMission_Base::PgMission_Base()
{
	Clear();
}

PgMission_Base::~PgMission_Base()
{
}

PgMission_Base::PgMission_Base(const PgMission_Base& rhs)
{
	this->Clone(rhs);
}

PgMission_Base& PgMission_Base::operator=(const PgMission_Base& rhs)
{
	PgMission_Base::Clone(rhs);
	return *this;
}

bool PgMission_Base::operator==(const PgMission_Base& rhs)const
{
	if( m_iNo != rhs.m_iNo ){return false;}
	if( m_kKey != rhs.m_kKey ){return false;}
	if( m_iAbilRateBagNo != rhs.m_iAbilRateBagNo ){return false;}
	if( m_iStageCount != rhs.m_iStageCount ){return false;}
	if( m_iModeType != rhs.m_iModeType ) {return false;}
	if( m_iLevelLimit_Min != rhs.m_iLevelLimit_Min ){return false;}
	if( m_iLevelLimit_Max != rhs.m_iLevelLimit_Max ){return false;}
	if( 0 != ::memcmp(&m_kMissionResult, &rhs.m_kMissionResult, sizeof(m_kMissionResult)) ){return false;}
	return m_kStage == rhs.m_kStage;
}

bool PgMission_Base::operator!=(const PgMission_Base& rhs)const
{
	return !((*this)==rhs);
}

bool PgMission_Base::Clone(const PgMission_Base& rhs)
{
	m_iNo = rhs.m_iNo;
	m_kKey = rhs.m_kKey;
	m_iAbilRateBagNo = rhs.m_iAbilRateBagNo;
	m_iStageCount = rhs.m_iStageCount;
	m_iStageCountExceptBonus = rhs.m_iStageCountExceptBonus;
	m_iModeType = rhs.m_iModeType;
	m_kMissionResult = rhs.m_kMissionResult;
	m_iCandidateNo = rhs.m_iCandidateNo;
	m_iLevelLimit_Min = rhs.m_iLevelLimit_Min;
	m_iLevelLimit_Max = rhs.m_iLevelLimit_Max;
	m_kStage = rhs.m_kStage;
	return true;
}

void PgMission_Base::Clear()
{
	m_iNo = 0;
	m_kKey.Clear();
	m_iAbilRateBagNo = 0;
	m_iCandidateNo = 0;
	m_iLevelLimit_Min = 0;
	m_iLevelLimit_Max = 0;
	m_iStageCount = 0;
	m_iStageCountExceptBonus = 0;
	m_kMissionResult.Clear();
	m_kStage.clear();
	m_iModeType = 0;
}

std::wstring PgMission_Base::GetMissionRankName( EMissionRank const kRank )
{
	switch ( kRank )
	{
	case MRANK_NONE:{return std::wstring(L"N");}break;
	case MRANK_SSS:{return std::wstring(L"SSS");}break;
	case MRANK_SS:{return std::wstring(L"SS");}break;
	case MRANK_S:{return std::wstring(L"S");}break;
	case MRANK_A:{return std::wstring(L"A");}break;
	case MRANK_B:{return std::wstring(L"B");}break;
	case MRANK_C:{return std::wstring(L"C");}break;
	case MRANK_D:{return std::wstring(L"D");}break;
	case MRANK_E:{return std::wstring(L"E");}break;
	case MRANK_F:{return std::wstring(L"F");}break;
	}

	return std::wstring();
}

bool PgMission_Base::Create(const TBL_DEF_MISSION_ROOT& rkMissionRoot,
							int const iLevel,
							const CONT_DEF_MISSION_CANDIDATE* pkMissionCandi,
							const CONT_DEF_MISSION_RESULT* pkContMissionResult)
{
	if ( iLevel >= DEFENCE8_MISSION_LEVEL ) // MAX_MISSION_LEVEL
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iRootLevelValue = 0;

	if( (iLevel+1) >= DEFENCE_MISSION_LEVEL )
	{
		switch( iLevel+1 )
		{
		case DEFENCE7_MISSION_LEVEL:
			{
				iRootLevelValue = rkMissionRoot.iDefence7;
			}break;
		case DEFENCE8_MISSION_LEVEL:
			{
				iRootLevelValue = rkMissionRoot.iDefence8;
			}break;
		case DEFENCE_MISSION_LEVEL:		
			{
				iRootLevelValue = rkMissionRoot.iDefence;
			}break;
		default:
			{
				iRootLevelValue = 0;				
			}break;
		}

		if( 0 == iRootLevelValue )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	else
	{
		iRootLevelValue = rkMissionRoot.aiLevel[iLevel];
	}

	

	if ( 0 == iRootLevelValue )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( NULL == pkMissionCandi )
	{
		g_kTblDataMgr.GetContDef(pkMissionCandi);
		if ( NULL == pkMissionCandi )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	CONT_DEF_MISSION_CANDIDATE::const_iterator candi_itr = pkMissionCandi->find(iRootLevelValue);
	if( pkMissionCandi->end() == candi_itr )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Can't Find Candidate[") << iRootLevelValue << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
	}

	CONT_DEF_MISSION_CANDIDATE::mapped_type const& kElement = candi_itr->second;
	if( 0 == kElement.iCandidate_End)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Candidate_End is 0") );
	}

	// ��ũ�� ����ϱ����� Candidate ��ȣ
	m_iCandidateNo = iRootLevelValue;

	for( int i=0; i!=MAX_MISSION_CANDIDATE; ++i)
	{
		if ( 0 != kElement.aiCandidate[i] )
		{
			SMissionStageKey kKey;
			kKey.iGroundNo = kElement.aiCandidate[i];
			kKey.kBitFalg = 0x01 << i;
			m_kStage.push_back(kKey);
		}
	}

	SMissionStageKey kKey;
	kKey.iGroundNo = kElement.iCandidate_End;
	kKey.kBitFalg = 0x00;
	m_kStage.push_back(kKey);
	if( m_kStage.size() < (size_t)(kElement.iUseCount) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("MissionNo=") << rkMissionRoot.iMissionNo << _T(", Key=") << rkMissionRoot.iKey << _T(", Level=") << iLevel <<  _T("UseCount[") << kElement.iUseCount << _T("] > CandidateCount[") << m_kStage.size() << _T("]") );
	}

	m_iNo = rkMissionRoot.iMissionNo;
	m_kKey.iKey = rkMissionRoot.iKey;
	m_kKey.iLevel = iLevel;	
	m_iStageCount = kElement.iUseCount;
	m_iModeType = kElement.iType;

	// ��� ����
	if ( NULL == pkContMissionResult )
	{
		g_kTblDataMgr.GetContDef(pkContMissionResult);
	}

	if( (iLevel+1) >= DEFENCE_MISSION_LEVEL )
	{
		m_iAbilRateBagNo = 0;

		if(pkContMissionResult)
		{
			CONT_DEF_MISSION_RESULT::const_iterator rst_itr = pkContMissionResult->find(rkMissionRoot.aiMissionResultNo[MRANK_SSS]);
			if ( rst_itr != pkContMissionResult->end() )
			{
				m_kMissionResult = (*rst_itr).second;
			}
		}

		return true;
	}
	else
	{
		m_iAbilRateBagNo = rkMissionRoot.aiLevel_AbilRateBagID[iLevel];

		if(pkContMissionResult)
		{
			CONT_DEF_MISSION_RESULT::const_iterator rst_itr = pkContMissionResult->find(rkMissionRoot.aiMissionResultNo[iLevel]);
			if ( rst_itr != pkContMissionResult->end() )
			{
				m_kMissionResult = (*rst_itr).second;
			}
		}
	}

	// ��������
	m_iLevelLimit_Min = rkMissionRoot.aiLevel_Min[iLevel];
	m_iLevelLimit_Max = rkMissionRoot.aiLevel_Max[iLevel];

	return true;
}

void PgMission_Base::WriteToPacket(BM::Stream& rkPacket)const
{
	rkPacket.Push(m_iNo);
	rkPacket.Push(m_kKey);
	rkPacket.Push(m_iAbilRateBagNo);
	m_kMissionResult.WriteToPacket(rkPacket);
	rkPacket.Push(m_iCandidateNo);
	rkPacket.Push(m_iLevelLimit_Min);
	rkPacket.Push(m_iLevelLimit_Max);
	rkPacket.Push(m_iStageCount);
	rkPacket.Push(m_iStageCountExceptBonus);
	rkPacket.Push(m_kStage);
	rkPacket.Push(m_iModeType);
}

void PgMission_Base::ReadFromPacket(BM::Stream& rkPacket)
{
	Clear();
	rkPacket.Pop(m_iNo);
	rkPacket.Pop(m_kKey);
	rkPacket.Pop(m_iAbilRateBagNo);
	m_kMissionResult.ReadFromPacket(rkPacket);
	rkPacket.Pop(m_iCandidateNo);
	rkPacket.Pop(m_iLevelLimit_Min);
	rkPacket.Pop(m_iLevelLimit_Max);
	rkPacket.Pop(m_iStageCount);
	rkPacket.Pop(m_iStageCountExceptBonus);
	rkPacket.Pop(m_kStage);
	rkPacket.Pop(m_iModeType);
}

void PgMission::Clear()
{
	PgMission_Base::Clear();
	m_kOwnerGuid.Clear();
	m_kConUser.clear();
	m_kGuidID.Clear();
	m_dwPlayTime = 0;
	m_iPlayTimePoint = 0;
	m_iRegistGndNo = 0;
	m_eTriggerType = 0;
	m_iOwnerLv = 0;

	m_iTimeMin = 0;
	m_iTimeSec = 0;

	m_kReqRestartUser.clear();
	m_kReqDefenceNextStageUser.clear();
	m_kReqGadaCoinUse.clear();
}

bool PgMission::CloneMission(PgMission* pkMission)
{
	PgMission_Base *pkBase = dynamic_cast<PgMission_Base*>(pkMission);
	if ( !pkBase )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !PgMission_Base::Clone(*pkBase) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kGuidID = pkMission->m_kGuidID;
	m_kOwnerGuid = pkMission->m_kOwnerGuid;
	m_iOwnerLv = pkMission->m_iOwnerLv;
	m_kConUser = pkMission->m_kConUser;
	m_iRegistGndNo = pkMission->m_iRegistGndNo;
	m_iTimeMin = pkMission->m_iTimeMin;
	m_iTimeSec = pkMission->m_iTimeSec;
	m_eTriggerType = pkMission->m_eTriggerType;
	m_kReqRestartUser = pkMission->m_kReqRestartUser;
	m_kReqDefenceNextStageUser = pkMission->m_kReqDefenceNextStageUser;
	m_kReqGadaCoinUse = pkMission->m_kReqGadaCoinUse;
	return true;
}

HRESULT PgMission::RestartUser(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMissionMutex);
	ConUser::iterator itr = m_kConUser.find(rkCharGuid);
	if ( itr==m_kConUser.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_kReqRestartUser.insert( rkCharGuid );

	if ( rkCharGuid == GetOwner() )
	{
		if ( m_kReqRestartUser.size() >= m_kConUser.size() )
		{
			return S_OK;
		}
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
	return S_FALSE;
}

HRESULT PgMission::ReqNextStageUser(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMissionMutex);
	ConUser::iterator itr = m_kConUser.find(rkCharGuid);
	if ( itr==m_kConUser.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_kReqDefenceNextStageUser.insert( rkCharGuid );

	if ( m_kReqDefenceNextStageUser.size() >= m_kConUser.size() )
	{
		m_kReqDefenceNextStageUser.clear();

		return S_OK;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
	return S_FALSE;
}

HRESULT PgMission::IsNextStageUser()
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	if ( m_kReqDefenceNextStageUser.size() >= m_kConUser.size() )
	{
		return S_OK;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
	return S_FALSE;
}

HRESULT PgMission::IsAbleOutUser(BM::GUID const &rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMissionMutex);
	SET_GUID::iterator itr = m_kReqRestartUser.find(rkCharGuid);
	if( itr != m_kReqRestartUser.end() )
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgMission::IsGadaCoinUse(BM::GUID const &rkCharGuid)
{	
	BM::CAutoMutex kLock(m_kMissionMutex);

	SET_GUID::iterator itr = m_kReqGadaCoinUse.find(rkCharGuid);
	if( itr == m_kReqGadaCoinUse.end() )
	{
		m_kReqGadaCoinUse.insert( rkCharGuid );
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgMission::GadaCoinUseClear()
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	m_kReqGadaCoinUse.clear();
}

void PgMission::GetGroundKey(SGroundKey& rkKey)
{
	rkKey.GroundNo(m_iRegistGndNo);
	rkKey.Guid(m_kGuidID);
}

bool PgMission::ChangeOwner(BM::GUID const &rkReqOwner)
{
	if ( GetOwner() != rkReqOwner )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}	

	ConUser::const_iterator owner_itr = m_kConUser.begin();
	while ( owner_itr != m_kConUser.end() )
	{
		if ( owner_itr->first != GetOwner() )
		{
			SetOwner(owner_itr->first);
			return true;
		}
		++owner_itr;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgMission::AddPlayTime(DWORD const dwPlayTime)
{
	// ���߿� ������ ȯ���Ҷ� 10���� ������ �־�� �Ѵ�.
	DWORD dwCheckMin = dwPlayTime / 60000;	// 1�д����� Ȯ��
	if ( dwCheckMin > 11 )
	{
		dwCheckMin = 11;//11���̻��� ������ ������
	}
	//m_iPlayTimePoint += ms_iTimePointValue[dwCheckMin];
	m_dwPlayTime += dwPlayTime;
}

void PgMission::WriteToPacket(BM::Stream& rkPacket)const
{
	PgMission_Base::WriteToPacket(rkPacket);
	rkPacket.Push(m_kGuidID);
	rkPacket.Push(m_iRegistGndNo);
	rkPacket.Push(m_eTriggerType);
	m_kConstellationMission.WriteToPacket(rkPacket);
	WriteToPacket_UserList(rkPacket,false);
	WriteToPacket_Mutator(rkPacket);
}

void PgMission::WriteToPacket_UserList(BM::Stream& rkPacket, bool bInfo)const
{
	rkPacket.Push(m_kOwnerGuid);
	rkPacket.Push(m_iOwnerLv);

	// �̼� Play�ð�
	rkPacket.Push(m_dwPlayTime);
	rkPacket.Push(m_iPlayTimePoint);

	rkPacket.Push(bInfo);
	rkPacket.Push(m_kConUser.size());
 	ConUser::const_iterator user_itr;
 	for ( user_itr=m_kConUser.begin(); user_itr!=m_kConUser.end(); ++user_itr )
 	{
 		rkPacket.Push(user_itr->first);
		if ( bInfo )
		{
			user_itr->second.WriteToPacket(rkPacket);
		}
 	}
}
void PgMission::ReadFromPacket_UserList(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	rkPacket.Pop(m_kOwnerGuid);
	rkPacket.Pop(m_iOwnerLv);
	rkPacket.Pop(m_dwPlayTime);
	rkPacket.Pop(m_iPlayTimePoint);

	bool bInfo = false;
	rkPacket.Pop(bInfo);
	size_t iSize = 0;
	rkPacket.Pop(iSize);
	while(iSize--)
	{
		BM::GUID kCharGuid;
		rkPacket.Pop(kCharGuid);

		if ( !bInfo )
		{
			AddMissionUser(kCharGuid);
		}
		else
		{
			ConUser::mapped_type kInfo;
			kInfo.ReadFromPacket(rkPacket);
			AddMissionUser(kCharGuid,kInfo);
		}
	}
}

void PgMission::ReadFromPacket(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMissionMutex);
	PgMission_Base::ReadFromPacket(rkPacket);
	rkPacket.Pop(m_kGuidID);
	rkPacket.Pop(m_iRegistGndNo);
	rkPacket.Pop(m_eTriggerType);
	m_kConstellationMission.ReadFromPacket(rkPacket);
	ReadFromPacket_UserList(rkPacket);
	ReadFromPacket_Mutator(rkPacket);
}

void PgMission::SetConstellationMission(Constellation::SConstellationMission const& constellationMission)
{
	m_kConstellationMission = constellationMission;
}

//
bool PgMission_Pack::WriteToPacket_MissionInfo( BM::Stream& rkPacket, PgPlayer_MissionData const &kPlayerMissionData, int const iPlayerLevel )
{
	SMissionInfo kInfo;
	size_t iTotalCount = 0;
	size_t iTotalClear = 0;
	BYTE kDoor = SMissionInfo::MDOOR_OPEN;
	bool bClearLevel[MAX_MISSION_LEVEL];

	for( size_t i=0; i!=MAX_MISSION_LEVEL; ++i )
	{
		bClearLevel[i] = false;

		if ( m_kConBase[i] )
		{
			kInfo.m_iMissionKey = m_kConBase[i]->GetKey();
			kInfo.m_iMissionNo = m_kConBase[i]->GetMissionNo();
			size_t iCount = m_kConBase[i]->GetTotalStageCount();
			size_t iClear = kPlayerMissionData.GetClearStageCount(i);
			iTotalClear += iClear;
			iTotalCount += iCount;

			if ( iPlayerLevel < m_kConBase[i]->LevelLimit_Min() )
			{
				kInfo.m_kDoor[i] = SMissionInfo::MDOOR_CLOSE;
			}
			else if ( kPlayerMissionData.IsClearLevel(i) )
			{
				kInfo.m_kDoor[i] = SMissionInfo::MDOOR_CLEARED;
				if ( i < 3 )
				{
					kDoor = SMissionInfo::MDOOR_OPEN;
				}
				else
				{
					kDoor = SMissionInfo::MDOOR_CLOSE;
				}
			}
			else
			{
				kInfo.m_kDoor[i] = kDoor;
				//kDoor = SMissionInfo::MDOOR_CLOSE;
				kDoor = SMissionInfo::MDOOR_OPEN;
			}

			if ( i >= 3 )
			{
				kDoor = SMissionInfo::MDOOR_OPEN;
			}

			bClearLevel[i] = kPlayerMissionData.IsClearLevel(i);
		}
	}
	if( iTotalCount > 0 )
	{
		kInfo.m_kTotalPercent = (BYTE)((iTotalClear*100) / iTotalCount);
	}
	else
	{
		kInfo.m_kTotalPercent = 0;
	}
	kInfo.WriteToPacket(rkPacket);
	rkPacket.Push(bClearLevel);
	return true;
}


//
PgMissionContMgr::PgMissionContMgr()
	:m_kBasePool(10, MAX_MISSION_LEVEL)
{
}

PgMissionContMgr::~PgMissionContMgr()
{
	Clear();
}

void PgMissionContMgr::swap(PgMissionContMgr& rkRight)
{
	m_kConBase.swap(rkRight.m_kConBase);
	m_kConPack.swap(rkRight.m_kConPack);
	m_kBasePool.swap(rkRight.m_kBasePool);
}

void PgMissionContMgr::Clear()
{
	ConBase::iterator base_itr = m_kConBase.begin();
	for ( ; base_itr != m_kConBase.end() ; ++base_itr )
	{
		Delete( base_itr->second );
	}
	m_kConBase.clear();
}

bool PgMissionContMgr::Build(	CONT_DEF_MISSION_RESULT const &rkResult,
								CONT_DEF_MISSION_CANDIDATE const &rkCandi,
								CONT_DEF_MISSION_ROOT const &rkRoot)
{
	Clear();

	PgMission_Base *pkBase = NULL;
	CONT_DEF_MISSION_ROOT::const_iterator root_itr;
	for ( root_itr=rkRoot.begin(); root_itr!=rkRoot.end(); ++root_itr )
	{
		PgMission_Pack kPack;
		CONT_DEF_MISSION_ROOT::mapped_type const &rkElement = root_itr->second;

//		CONT_DEF_MISSION_LEVELLIMIT::const_iterator level_itr = rkLevelLimmit.find(rkElement.iLevelLimitNo);
//		CONT_DEF_MISSION_LEVELLIMIT::mapped_type kLvLimmitData;
//		if ( level_itr != rkLevelLimmit.end() )
//		{
//			kLvLimmitData = level_itr->second;
//		}

		for(int i=0; i!=DEFENCE8_MISSION_LEVEL; ++i )	// MAX_MISSION_LEVEL
		{
			SMissionKey kKey;
			kKey.iKey = rkElement.iKey;
			kKey.iLevel = i;

			if ( New(pkBase) )
			{
				if ( pkBase->Create(rkElement,i,&rkCandi,&rkResult) )
				{	
					m_kConBase.insert(std::make_pair(kKey,pkBase));
					kPack.Set(i,pkBase);
					pkBase = NULL;
				}
				else
				{
					kPack.Set(i);
				}
			}
			//g_kRankMgr.AddRank(kKey,kLvLimmitData.aiLevel_Max[i]);
		}
		m_kConPack.insert(std::make_pair(rkElement.iKey, kPack));
	}
	Delete(pkBase);
	return true;
}

EDataCompareRet PgMissionContMgr::Compare(PgMissionContMgr& rkRight)
{
	EDataCompareRet eRet = DC_RET_EQUAL;

	// Base Compare
	ConBase::const_iterator base_itr_org = m_kConBase.begin();
	ConBase::const_iterator base_itr_new = rkRight.m_kConBase.begin();
	while( (base_itr_org!=m_kConBase.end()) && (base_itr_new!=rkRight.m_kConBase.end()) )
	{
		SMissionKey const& rkOrgKey = base_itr_org->first;
		SMissionKey const& rkNewKey = base_itr_new->first;
		if( rkOrgKey == rkNewKey )
		{
			if ( (*(base_itr_org->second)) != (*(base_itr_new->second)) )
			{
				//INFO_LOG(BM::LOG_LV0,_T("[%s]MissionBase[K%d/Lv%d] Modify"),__FUNCTIONW__,rkOrgKey.iKey,rkOrgKey.iLevel);
				eRet |= DC_RET_MODIFY;
			}
			++base_itr_new;
			++base_itr_org;
		}
		else
		{
			if ( rkOrgKey > rkNewKey )
			{
				//INFO_LOG(BM::LOG_LV0,_T("[%s]MissionBase[K%d/Lv%d] AddData"),__FUNCTIONW__,rkNewKey.iKey,rkNewKey.iLevel);
				eRet |= DC_RET_ADDTION;
				++base_itr_new;
				
			}
			else
			{
				//INFO_LOG(BM::LOG_LV0,_T("[%s]MissionBase[K%d/Lv%d] LostData"),__FUNCTIONW__,rkOrgKey.iKey,rkOrgKey.iLevel);
				eRet |= DC_RET_LOST;
				++base_itr_org;
			}
		}
	}

	while( base_itr_org!=m_kConBase.end() )
	{
		SMissionKey const& rkOrgKey = base_itr_org->first;
		//INFO_LOG(BM::LOG_LV0,_T("[%s]MissionBase[K%d/Lv%d] LostData"),__FUNCTIONW__,rkOrgKey.iKey,rkOrgKey.iLevel);
		eRet |= DC_RET_LOST;
		++base_itr_org;
	}

	while( base_itr_new!=rkRight.m_kConBase.end() )
	{
		SMissionKey const& rkNewKey = base_itr_new->first;
		//INFO_LOG(BM::LOG_LV0,_T("[%s]MissionBase[K%d/Lv%d] AddData"),__FUNCTIONW__,rkNewKey.iKey,rkNewKey.iLevel);
		eRet |= DC_RET_ADDTION;
		++base_itr_new;
	}

	return eRet;
}
