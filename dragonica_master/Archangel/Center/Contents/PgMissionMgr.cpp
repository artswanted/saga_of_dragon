#include "stdafx.h"
#include "Global.h"
#include "PgMissionMgr.h"
#include "Variant/PgMissionInfo.h"

//////////////////////////////////////////////////////////////////////////
//	PgMissionMgr
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool PgMissionMgr::Build(const CONT_DEF_MISSION_RESULT &rkResult,
						 const CONT_DEF_MISSION_CANDIDATE &rkCandi,
						 const CONT_DEF_MISSION_ROOT &rkRoot)
{
	if ( !PgMissionContMgr::Build(rkResult, rkCandi, rkRoot) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

void PgMissionMgr::swap(PgMissionContMgr& rkRight)
{
	BM::CAutoMutex kLock(m_kMissionMutex);
	EDataCompareRet eRet = PgMissionContMgr::Compare(rkRight);
	if ( eRet & DC_RET_LOST )
	{// 없어지는 건 허용하면 안된다.
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Failed : LoadData") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CompareRet is DC_RET_LOST"));
		return;
	}

	PgMissionContMgr::swap(rkRight);
}

PgMissionMgr::PgMissionMgr()
:	m_kMissionPool(30,20)
{
}

PgMissionMgr::~PgMissionMgr()
{
}

bool PgMissionMgr::RegistMission(BM::Stream* const pkPacket, bool const bAll)
{
	SMissionKey kKey;
	SGroundKey kOrgGndKey;
	BM::GUID kOwnerGuid;
	int iPlayerLevel;
	int iType;
	Constellation::SConstellationMission constellationMission;

	pkPacket->Pop(kKey);
	pkPacket->Pop(kOrgGndKey);
	pkPacket->Pop(iType);
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(iPlayerLevel);
	constellationMission.ReadFromPacket(*pkPacket);

	BM::CAutoMutex kLock(m_kMissionMutex);
	PgMissionRegister kRegister( m_kConReqRegist );
	if ( !kRegister.Regist( kOwnerGuid ) )
	{
		g_kServerSetMgr.Locked_SendWarnMessage( kOwnerGuid, 1711, EL_Warning );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ConBase::iterator base_itr = m_kConBase.find(kKey);
	if ( base_itr == m_kConBase.end() )
	{
		g_kServerSetMgr.Locked_SendWarnMessage( kOwnerGuid, 400225, EL_Warning);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iLevelLimit = base_itr->second->LevelLimit_Min();
	if ( iLevelLimit > iPlayerLevel )
	{
		// 레벨이 맞지 않아서 입장할 수 없다.
		g_kServerSetMgr.Locked_SendWarnMessage2( kOwnerGuid, 800, iLevelLimit, EL_Warning );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMissionContents *pkMission = New();
	if ( !pkMission )
	{
		g_kServerSetMgr.Locked_SendWarnMessage( kOwnerGuid, 6, EL_Warning );
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Mission Memory Error") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	if ( !pkMission->Start( *(base_itr->second), kRegister.GetID(), bAll, pkPacket ) )
	{
		// 조금있다가 시도해야 한다.
		g_kServerSetMgr.Locked_SendWarnMessage( kOwnerGuid, 6, EL_Warning );
		Delete(pkMission);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if (pkMission->m_iModeType > (int)MO_SCENARIO)
	{
		pkMission->ClearMutator();
	}

	//Check mutator to exist
	if (pkMission->GetMutatorSet().size() != 0)
	{
		CONT_DEF_MISSION_MUTATOR const* pkMutatorInTB = NULL;
		g_kTblDataMgr.GetContDef(pkMutatorInTB);
		if( !pkMission->MutatorVerify(pkMutatorInTB) )
		{
			g_kServerSetMgr.Locked_SendWarnMessage( kOwnerGuid, 401265, EL_Warning );
			Delete(pkMission);
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Mission use undefined mutator") );
			return false;
		}
	}

	pkMission->SetOwner(kOwnerGuid);
	pkMission->SetOwnerLevel(iPlayerLevel);
	int const eTriggerType = static_cast<int>(iType);
	pkMission->SetTriggerType(eTriggerType);
	pkMission->SetConstellationMission(constellationMission);

	SERVER_IDENTITY kFoundSI;
	HRESULT hRet = g_kServerSetMgr.Locked_GroundLoadBalance( SGroundKey(pkMission->GetMissionNo()), kFoundSI );
	if( S_OK != hRet )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Mission No[") << pkMission->GetMissionNo() << _T("]'s LoadBalance Failed!!") );
		g_kServerSetMgr.Locked_SendWarnMessage( kOwnerGuid, 6, EL_Warning );
		Delete(pkMission);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !AddMission(pkMission) )
	{
		g_kServerSetMgr.Locked_SendWarnMessage( kOwnerGuid, 400225, EL_Warning );
		Delete(pkMission);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SGroundKey kTargetKey;
	pkMission->GetGroundKey(kTargetKey);

	BM::Stream kOrgMapPacket( PT_N_M_ANS_ENTER_MISSION, kOwnerGuid );
	kOrgMapPacket.Push(pkMission->GetMissionNo());
	kOrgMapPacket.Push(pkMission->GetTotalStageCount());
	kOrgMapPacket.Push(kTargetKey);
	kOrgMapPacket.Push(pkMission->GetLevel());
	kOrgMapPacket.Push(eTriggerType);
	g_kServerSetMgr.Locked_SendToGround( kOrgGndKey, kOrgMapPacket );

	BM::Stream kPacket(PT_T_M_NFY_PREPARE_MISSION);
	pkMission->WriteToPacket(kPacket);
	SendToServer( kFoundSI, kPacket );

	kRegister.Release();// 성공시에만 이걸해주어야 한다
	return true;
}

bool PgMissionMgr::RegistMissionResult( BM::Stream* const pkPacket )
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	BM::GUID kMissionID;
	bool bSuc = false;
	pkPacket->Pop( kMissionID );
	pkPacket->Pop( bSuc );

	if ( !bSuc )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Failed MissionID=") << kMissionID );
		return SUCCEEDED( UnRegistMission( kMissionID ) );
	}

	ConMission::iterator mission_itr = m_kConMission.find(kMissionID);
	if ( mission_itr == m_kConMission.end() )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Not Found Mission ID=") << kMissionID );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMissionContents *pkMission = mission_itr->second;
	if ( pkMission )
	{
		ConReqRegist::iterator req_itr = m_kConReqRegist.find( pkMission->GetOwner() );
		if ( req_itr != m_kConReqRegist.end() )
		{
			if ( req_itr->second == kMissionID )
			{
				m_kConReqRegist.erase( req_itr );
			}
		}
	}

	return true;
}

bool PgMissionMgr::RestartMission(BM::Stream* const pkPacket)
{
	BM::GUID kMissionID;
	pkPacket->Pop(kMissionID);

	SMissionKey kMissionKey;
	pkPacket->Pop(kMissionKey);

	int iPlayerLevel=0;
	pkPacket->Pop(iPlayerLevel);

	PgMissionContents *pkMission = GetMission(kMissionID);
	if ( !pkMission )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::CAutoMutex kLock(m_kMissionMutex);
	ConBase::iterator base_itr = m_kConBase.find(kMissionKey);
	if ( base_itr == m_kConBase.end() )
	{
		g_kServerSetMgr.Locked_SendWarnMessage( pkMission->GetOwner(), 400225, EL_Warning );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iLevelLimit = base_itr->second->LevelLimit_Min();
	if ( iLevelLimit > iPlayerLevel )
	{
		// 레벨이 맞지 않아서 입장할 수 없다.
		g_kServerSetMgr.Locked_SendWarnMessage2( pkMission->GetOwner(), 800, iLevelLimit, EL_Warning );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !pkMission->Restart(*(base_itr->second), pkPacket) )
	{
		g_kServerSetMgr.Locked_SendWarnMessage( pkMission->GetOwner(), 6, EL_Warning );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SGroundKey kGndKey;
	pkMission->GetGroundKey(kGndKey);

	BM::Stream kPacket(PT_N_M_ANS_MISSION_RESTART);
	pkMission->WriteToPacket(kPacket);
	g_kServerSetMgr.Locked_SendToGround(kGndKey, kPacket, false);

	return true;
}

HRESULT PgMissionMgr::UnRegistMission( BM::GUID const &kMissionID )
{
	BM::CAutoMutex kLock(m_kMissionMutex);

	ConMission::iterator mission_itr = m_kConMission.find(kMissionID);
	if ( mission_itr == m_kConMission.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	PgMissionContents *pkMission = mission_itr->second;
	if ( pkMission )
	{
		ConReqRegist::iterator req_itr = m_kConReqRegist.find( pkMission->GetOwner() );
		if ( req_itr != m_kConReqRegist.end() )
		{
			if ( req_itr->second == kMissionID )
			{
				// 검사하는 이유는 미션을 등록한 오너가 미션을 나가서 다른 미션을 만들 수 도 있기 때문이다.
				m_kConReqRegist.erase( req_itr );
			}
		}
	}

	assert(pkMission);

	m_kConMission.erase(mission_itr);
	Delete(pkMission);
	return S_OK;
}


bool PgMissionMgr::EndMission(BM::Stream* const pkPacket)
{
	BM::GUID kMissionKey;
	SGroundKey kGndKey;

	pkPacket->Pop(kMissionKey);
	pkPacket->Pop(kGndKey);

	PgMissionContents* pkMission = GetMission(kMissionKey);
	if ( !pkMission )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkMission->ReadFromPacket_UserList(*pkPacket);
	pkMission->DoAction_Result_Req(kGndKey);
	//pkMission->DoAction_Result();

	return true;
}

void PgMissionMgr::SendToGroundDeletePartyInfo(BM::Stream* const pkPacket)
{
	SGroundKey kGndKey;
	BM::GUID kPartyGuid;

	pkPacket->Pop(kGndKey);
	pkPacket->Pop(kPartyGuid);	

	BM::Stream kPacket(PT_T_M_NFY_DEFENCE_DELETE_PARTYINFO);
	kPacket.Push( kPartyGuid );
	g_kServerSetMgr.Locked_SendToGround( kGndKey, kPacket );
}

void PgMissionMgr::SendToGroundPlayPartyInfo(BM::Stream* const pkPacket)
{
	SGroundKey kGndKey;
	BM::GUID kPartyGuid;

	pkPacket->Pop(kGndKey);
	pkPacket->Pop(kPartyGuid);	

	BM::Stream kPacket(PT_T_M_NFY_DEFENCE_PLAY_PARTYINFO);
	kPacket.Push( kPartyGuid );
	g_kServerSetMgr.Locked_SendToGround( kGndKey, kPacket );
}

bool PgMissionMgr::RecvPacket(BM::Stream * const pkPacket)
{
	PACKET_ID_TYPE kType;
	pkPacket->Pop(kType);
	switch( kType )
	{
	case PT_M_N_REQ_MISSION_INFO:
		{
			int iMissionKey = 0;
			PgPlayer_MissionData kPlayerMissionData;
			BM::GUID kMemGuid;
			int iPlayerLevel;
			int iType;

			SGroundKey rkGndkey;
			pkPacket->Pop(rkGndkey);

			pkPacket->Pop(iMissionKey);
			pkPacket->Pop(iType);
			kPlayerMissionData.ReadFromPacket(*pkPacket);
			pkPacket->Pop(kMemGuid);
			pkPacket->Pop(iPlayerLevel);

			SMissionInfo kInfo;
			ConPack::iterator pack_itr = m_kConPack.find(iMissionKey);
			if ( pack_itr != m_kConPack.end() )
			{
				BM::Stream kPacket(PT_N_M_ANS_MISSION_INFO);
				kPlayerMissionData.WriteToPacket(kPacket);
				pack_itr->second.WriteToPacket_MissionInfo( kPacket, kPlayerMissionData, iPlayerLevel );
				kPacket.Push(kMemGuid);				
				kPacket.Push(iType);
				g_kServerSetMgr.Locked_SendToGround(rkGndkey, kPacket);
			}
			else
			{
				BM::Stream kPacket(PT_M_C_NFY_WARN_MESSAGE, 400225);
				kPacket.Push(std::wstring());
				g_kServerSetMgr.Locked_SendToUser(kMemGuid,kPacket);
			}
		}break;
	case PT_M_N_REQ_ENTER_MISSION:
		{
			RegistMission(pkPacket);
		}break;
	case PT_M_N_ANS_ENTER_MISSION_FAILED:
		{
			BM::GUID kMissionID;
			pkPacket->Pop(kMissionID);
			UnRegistMission( kMissionID );
		}break;
	case PT_M_T_ANS_PREPARE_MISSION:
		{
			RegistMissionResult( pkPacket );
		}break;
	case PT_C_M_REQ_MISSION_RESTART:
		{
			RestartMission(pkPacket);
		}break;
	case PT_M_N_NFY_MISSION_RESULT:
		{
			EndMission(pkPacket);
		}break;
	case PT_T_T_NFY_MISSION_DELETE:
		{
			SGroundKey kGndKey;
			pkPacket->Pop( kGndKey );
			UnRegistMission( kGndKey.Guid() );
		}break;
	case PT_C_M_MISSION_TEST:
		{
			RegistMission( pkPacket, true );
		}break;
	case PT_N_T_RES_MISSION_RANKING:
		{
			Recv_PT_N_T_RES_MISSION_RANKING(pkPacket);
		}break;
	case PT_M_T_NFY_DEFENCE_DELETE_PARTYINFO:
		{
			SendToGroundDeletePartyInfo(pkPacket);
		}break;
	case PT_M_T_NFY_DEFENCE_PLAY_PARTYINFO:
		{
			SendToGroundPlayPartyInfo(pkPacket);
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled packet [") << kType << _T("]") );
		}
	}
	return true;
}

void PgMissionMgr::Recv_PT_N_T_RES_MISSION_RANKING(BM::Stream* const pkPacket)
{
	BM::GUID kMissionGuid;
	pkPacket->Pop(kMissionGuid);

	PgMissionContents* pkMission = GetMission(kMissionGuid);
	if ( !pkMission )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Cannot find Mission, Guid[") << kMissionGuid.str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find Mission"));
		return;
	}
	pkMission->DoAction_Result_Res(pkPacket);
}
