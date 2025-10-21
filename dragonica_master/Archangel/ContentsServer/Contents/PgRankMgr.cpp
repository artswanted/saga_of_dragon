#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "Variant/Global.h"
#include "Variant/PgEventView.h"
#include "Variant/PgMission_Result.h"
#include "PgDBProcess.h"
#include "PgRankMgr.h"
#include "PgServerSetMgr.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//	PgMission_Rank
//////////////////////////////////////////////////////////////////////////////////

PgMission_Rank::PgMission_Rank()
{
	Clear();
}

PgMission_Rank::~PgMission_Rank()
{
	Clear();
}

void PgMission_Rank::Clear()
{
	m_kReport.Clear();
	m_kRankList.clear();
	m_dwLastSaveTime = BM::GetTime32();
	m_iRankLevel_Max = 100;
	m_iRankLevel_Min = 1;
}

void PgMission_Rank::MissionRankClear()
{
	m_kRankList.clear();
}

bool PgMission_Rank::Init(SMissionKey const& rkKey,TBL_MISSION_REPORT& rkReport)
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_kMissionKey != rkKey )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMission_Report::Init(rkReport);
	m_dwLastSaveTime = BM::GetTime32();
	return true;
}

bool PgMission_Rank::Init(SMissionKey const& rkKey,CONT_MISSION_RANK const &rkCon)
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_kMissionKey != rkKey )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 센터에서 올때 정렬되어 왔다.
	//	std::sort(rkCon.begin(),rkCon.end());
	WORD iStanding = 1;
	CONT_MISSION_RANK::const_iterator itr;
	for(itr=rkCon.begin();itr!=rkCon.end();++itr)
	{
		m_kRankList.insert( std::make_pair(SMissionRankKey(*itr),PgMission_RankItem(*itr,iStanding)) );
		++iStanding;
	}
	m_dwLastSaveTime = BM::GetTime32();
	return true;
}

bool PgMission_Rank::DoAction(ConPlayRankInfo& rkPlayerRankList,int& iClearTime_Top,int& iClearTime_Avg)
{
	BM::CAutoMutex kLock(m_kMutex);

	SYSTEMTIME kSysTime;
	g_kEventView.GetLocalTime(&kSysTime);

	int iPlayTime = 0;
	ConPlayRankInfo::iterator itr;
	for(itr=rkPlayerRankList.begin();itr!=rkPlayerRankList.end();++itr)
	{
		// TODO: Replace multiset to vector + sort after insert new elements
		// Now continue use hack over multiset because we want save 
		// compatibility with the old version of the protocol
		// by reOiL
		SPlayerRankInfo* pRankPlayer = const_cast<SPlayerRankInfo*>(&(*itr));
		// 레벨이 높은놈은 랭크에 포함시키지 않는다. 추가: SSS랭크일 때만 포함 시킨다
		if( (m_iRankLevel_Min > itr->iLevel) || (m_iRankLevel_Max < itr->iLevel) || itr->kRank != MRANK_SSS )
		{
			// 랭크에 포함되지 않는다는 것을 스텐딩 값에 맥스로 셋팅해서 알려준다.
			pRankPlayer->iStanding = USHRT_MAX;
			continue;
		}

		iPlayTime = itr->kRankKey.iPlayTime;

		PgMission_RankItem kRankItem(*itr);
		ConRankList::iterator insert_itr = m_kRankList.insert( std::make_pair(SMissionRankKey(itr->kRankKey),kRankItem) );
		if(insert_itr!=m_kRankList.end())
		{
			if (insert_itr!=m_kRankList.begin())
			{
				ConRankList::iterator rank_itr = insert_itr;
				--rank_itr;
				if ( rank_itr->second.Standing() < ms_MAX_SAVE_COUNT )
				{
					insert_itr->second.Standing(rank_itr->second.Standing()+1);
				}
				else
				{
					break;
				}
			}
			else
			{
				insert_itr->second.Standing(1);
			}
			insert_itr->second.RegistTime(kSysTime);

			//
			if( (1 <= insert_itr->second.Standing()) && (3 >= insert_itr->second.Standing()) )
			{
				pRankPlayer->iNewRank = insert_itr->second.Standing();
			}
			else
			{
				pRankPlayer->iNewRank = 0;
			}
			//

			pRankPlayer->iStanding = insert_itr->second.Standing();
			pRankPlayer->kMemoGuid = insert_itr->second.GetID();
			while( ++insert_itr != m_kRankList.end() )
			{
				insert_itr->second.AddRank();
			}
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			break;
		}
	}

	if( iPlayTime && PgMission_Report::Update(iPlayTime) )
	{
		SaveReport(BM::GetTime32());
	}
	iClearTime_Top = PgMission_Report::GetClearTime_Top();
	iClearTime_Avg = PgMission_Report::GetClearTime_Avg();

	Order();
	return true;
}

PgMission_RankItem* PgMission_Rank::UpdateMemo( SMissionRankKey const &rkkey, BM::GUID const &rkMemoGuid, std::wstring const &wstrMemo, bool const bSave)
{
	BM::CAutoMutex kLock(m_kMutex);
	ConRankList::iterator rank_itr = m_kRankList.lower_bound(rkkey);
	if ( rank_itr==m_kRankList.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	do
	{
		if ( rank_itr->second.UpdateMemo( rkMemoGuid, wstrMemo ) && bSave )
		{
			SaveMissionRank( MissionKey(), rank_itr->second.GetRank() );
			return &(rank_itr->second);
		}
		++rank_itr;
	}while( (rank_itr!=m_kRankList.end()) && (rkkey == rank_itr->first) );

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

void PgMission_Rank::Order()
{
	int iEraseSize = (int)m_kRankList.size() - (int)ms_MAX_SAVE_COUNT;
	ConRankList::iterator itr = m_kRankList.end();
	while ( 0 < iEraseSize )
	{
		--itr;
		--iEraseSize;
	}
	m_kRankList.erase(itr,m_kRankList.end());
}

void PgMission_Rank::Get(CONT_MISSION_RANK &rkRankList,size_t iCount)
{
	BM::CAutoMutex kLock(m_kMutex);
	rkRankList.clear();
	ConRankList::iterator itr = m_kRankList.begin();
	while ( itr!=m_kRankList.end() && (rkRankList.size()<iCount) )
	{
		rkRankList.push_back(itr->second.GetRank());
		++itr;
	}
}

void PgMission_Rank::Tick(DWORD const dwCurTime)
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( DifftimeGetTime(m_dwLastSaveTime,dwCurTime) > ms_SAVE_TIME_DELAY )
	{
		SaveReport(dwCurTime);
	}
}

void PgMission_Rank::SaveReport(DWORD const dwCurTime)
{
	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SAVE_MISSION_REPORT, L"EXEC [dbo].[UP_SaveMissionReport]");
	kQuery.PushStrParam(m_kMissionKey.iKey);
	BYTE kLevel = (BYTE)m_kMissionKey.iLevel;
	kQuery.PushStrParam(kLevel);
	kQuery.PushStrParam(m_kReport.iClearTime_Top);
	kQuery.PushStrParam(m_kReport.iClearTime_Avg);
	kQuery.PushStrParam(m_kReport.i64ClearTime_Total);
	kQuery.PushStrParam(m_kReport.i64ClearCount);
	g_kCoreCenter.PushQuery(kQuery);
	m_dwLastSaveTime = dwCurTime;
	//INFO_LOG(BM::LOG_LV8,_T("[%s]MissionKey:%d,Level:%d Report Save DB"),__FUNCTIONW__,m_kMissionKey.iKey,m_kMissionKey.iLevel);
}

void PgMission_Rank::ReadFromPacket(BM::Stream &kPacket)
{
	PgMission_Report::ReadFromPacket(kPacket);

	ReadFromPacket_Rank(kPacket);
}

void PgMission_Rank::WriteToPacket(BM::Stream &kPacket) const
{
	PgMission_Report::WirteToPacket(kPacket);

	WriteToPacket_Rank(kPacket);
}

void PgMission_Rank::ReadFromPacket_Rank(BM::Stream &kPacket)
{
	PACKETPOP_MAP_PM(kPacket, ConRankList, m_kRankList);
}

void PgMission_Rank::WriteToPacket_Rank(BM::Stream &kPacket) const
{
	PACKETPUSH_MAP_PM(kPacket, ConRankList, m_kRankList);
}

void PgMission_Rank::AddRankItem(SMissionRankKey const &rkKey, BM::GUID const &rkRankItemID, PgMission_RankItem const &rkRankItem)
{
//	미션의 랭크는 Key가 중복됨(중복되어야 하는데......)
//	SyncObject에서 쓰는거라서-_-;;;;;;;;;
// 	ConRankList::iterator itor = m_kRankList.find(rkKey);
// 	if (itor != m_kRankList.end())
// 	{
// 		INFO_LOG(BM::LOG_LV9, _T("[%s] RankItem alread exist"), __FUNCTIONW__);
// 		return;
// 	}
	m_kRankList.insert(std::make_pair(rkKey, rkRankItem));
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//	PgRankRegister
//////////////////////////////////////////////////////////////////////////////////

void PgRankRegister::Update(std::wstring& wstrMemo)
{
	if( NULL == m_pkRank )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Rank is NULL"));
		return;
	}

	if ( wstrMemo.size() > 20 )
	{
		wstrMemo.resize(20);
	}

	m_pkRank->UpdateMemo(m_kKey,m_kMemoGuid,wstrMemo, true);
	m_pkRank = NULL;
}

bool PgRankRegister::IsEnd(DWORD const dwNow)
{
	if ( NULL == m_pkRank )
	{
		return true;
	}

	// dwNow < m_dwStartTime 일 수도 있다.. (Timer가 Lock걸려서 대기상태로 있을 때)
	DWORD const dwGab = abs(long(dwNow-m_dwStartTime));
	if ( dwGab > 40000 )
	{
		m_pkRank->UpdateMemo(m_kKey,m_kMemoGuid, std::wstring(), true);
		m_pkRank = NULL;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

SMissionKey const * PgRankRegister::GetMissionKey()
{
	if (m_pkRank != NULL)
	{
		return &m_pkRank->MissionKey();
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

SMissionRankKey const& PgRankRegister::GetMissionRankKey()
{
	return m_kKey;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//	PgRankMgr
//////////////////////////////////////////////////////////////////////////////////

PgRankMgr::PgRankMgr()
:	m_kRegisterPool(30,20)
{

}

PgRankMgr::~PgRankMgr()
{
	{
		ConRegister::iterator erase_iter = m_kConRegister.begin();
		while( m_kConRegister.end() != erase_iter )
		{
			m_kRegisterPool.Delete( (*erase_iter).second );
			++erase_iter;
		}
		m_kConRegister.clear();
	}
	{
		ConRank::iterator erase_iter = m_kConRank.begin();
		while( m_kConRank.end() != erase_iter )
		{
			m_kRankPool.Delete( (*erase_iter).second );
			++erase_iter;
		}
		m_kConRank.clear();
	}
}

void PgRankMgr::OnRegisterMemoTick(DWORD const dwCurTime)
{
	BM::CAutoMutex kLock(m_kMutex);
	while ( !m_kQueue.empty() )
	{
		PgRankRegister* pkRegister = m_kQueue.front();
		if ( !pkRegister->IsEnd(dwCurTime) )
		{
			break;
		}

		m_kConRegister.erase(pkRegister->GetID());
		pkRegister->Clear();
		m_kRegisterPool.Delete(pkRegister);
		m_kQueue.pop();
	}
}

void PgRankMgr::OnTick(DWORD const dwCurTime)
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_kConRank.empty() )
	{
		return;
	}

	ConRank::const_iterator itr = m_kConRank.find(m_kNextKey);
	if ( itr == m_kConRank.end() )
	{
		itr = m_kConRank.begin();
	}

	int const iMissionKey = itr->first.iKey;
	while ( iMissionKey == itr->first.iKey )
	{
		itr->second->Tick(dwCurTime);
		if ( ++itr == m_kConRank.end() )
		{
			itr = m_kConRank.begin();
			break;
		}
	}
	//INFO_LOG(BM::LOG_LV8, _T("[%s] MissionKey : %d"), __FUNCTIONW__, iMissionKey );
	m_kNextKey = itr->first;
}

void PgRankMgr::AddRank(SMissionKey const& rkMissionKey, int const iRankLevel_Min, int const iRankLevel_Max)
{//AddRank는 반드시 처음에 빌드할때만 호출!!!!!!!
	PgMission_Rank* pkRank = GetRank(rkMissionKey);
	if ( pkRank == NULL )
	{
		pkRank = CreateMission_Rank(rkMissionKey);
	}
	pkRank->RankLevel_Min(iRankLevel_Min);
	pkRank->RankLevel_Max(iRankLevel_Max);
}

PgMission_Rank* PgRankMgr::GetRank(SMissionKey const& rkMissionKey)
{
	ConRank::iterator itr = m_kConRank.find(rkMissionKey);
	if ( itr != m_kConRank.end() )
	{
		return itr->second;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

/*
PgMission_Rank_Base* PgRankMgr::GetNextRank(SMissionKey &rkMissionKey)
{
	ConRank::iterator itor = m_kConRank.find(rkMissionKey);
	if ( itor == m_kConRank.end() )
	{
		itor = m_kConRank.begin();
	}
	else
	{
		++itor;
	}
	if (itor == m_kConRank.end())
	{
		return NULL;
	}
	PgMission_Rank_Base* pkFind = itor->second;
	rkMissionKey = pkFind->GetData().MissionKey();
	return pkFind;
}
*/

bool PgRankMgr::SendRankingList(SMissionKey const& rkMissionKey, BM::GUID const &rkMemberGuid)
{
	PgMission_Rank* pkRank = GetRank(rkMissionKey);
	if ( pkRank )
	{
		PgMission_Result kData;
		pkRank->Get(kData.GetRankData(),10);

		BYTE kWriteType = PgMission_Result::VIEWTYPE_RANK;
		BM::Stream kCPacket(PT_N_C_NFY_MISSION_RESULT,kWriteType);
		kData.WriteToPacket(kCPacket);
		g_kRealmUserMgr.Locked_SendToUser(rkMemberGuid,kCPacket);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRankMgr::SendRankingTop(SMissionKey const& rkMissionKey, BM::GUID const &rkMemberGuid)
{
	PgMission_Rank* pkRank = GetRank(rkMissionKey);
	if ( pkRank )
	{
		PgMission_Result kData;
		pkRank->Get(kData.GetRankData(), 1);

		BM::Stream kCPacket(PT_N_C_ANS_RANK_TOP);
		if( 0 < kData.GetRankCount() )
		{
			kCPacket.Push(kData.GetFirstElementFromRankData()->iPoint);
			kCPacket.Push(kData.GetFirstElementFromRankData()->wstrName);			

			g_kRealmUserMgr.Locked_SendToUser(rkMemberGuid, kCPacket);
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRankMgr::AddRegister(PgMission_Rank* pkRank, SPlayerRankInfo const &rPlayerRankInfo)
{
	PgRankRegister* pkMemo = m_kRegisterPool.New();
	if ( pkMemo )
	{
		BM::GUID const &kID = pkMemo->Init(pkRank,rPlayerRankInfo);
		if ( BM::GUID::NullData() != kID )
		{
			m_kConRegister.insert(std::make_pair(kID,pkMemo));
			m_kQueue.push(pkMemo);
			return true;
		}
		else
		{
			m_kRegisterPool.Delete(pkMemo);	
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRankMgr::UpdateRegister(BM::GUID const &rkGuid, std::wstring& wstrMemo)
{
	ConRegister::iterator itr = m_kConRegister.find(rkGuid);
	if ( itr == m_kConRegister.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	itr->second->Update(wstrMemo);

	return true;
}

/*
PgMission_Rank_Base* PgRankMgr::CreateAndGetObj(ESyncMissRank_Type const eType, SMissionKey const &rkMissionKey)
{
	PgMission_Rank_Base* pkRank = GetRank(rkMissionKey);
	if (pkRank != NULL)
	{
		return pkRank;
	}
	
	// 생성요청~~
	BM::Stream kNotify(EMissRank_PriType_ReqCreateObj, eType);
	kNotify.Push(rkMissionKey);
	VNotify(&kNotify);
	// 생성요청 하고 났으니, 다시 찾아 본다.
	return GetRank(rkMissionKey);
}


bool PgRankMgr::AddObj(SMissionKey const &rkMissionKey, PgMission_Rank_Base* pkRank)
{
	if (pkRank == NULL)
	{
		return false;
	}
	pkRank->GetData().MissionKey(rkMissionKey);
	auto ret = m_kConRank.insert(std::make_pair(rkMissionKey, pkRank));
	return ret.second;
}

bool PgRankMgr::DeleteObj(PgMission_Rank_Base* pkRank)
{
	m_kConRank.erase(pkRank->GetData().MissionKey());
	return true;
}
*/

PgMission_Rank*	PgRankMgr::CreateMission_Rank(SMissionKey const &rkMissionKey)
{
	PgMission_Rank* pkNew = m_kRankPool.New();
	pkNew->MissionKey(rkMissionKey);

	auto ibRet = m_kConRank.insert(std::make_pair(rkMissionKey, pkNew));
	if (!ibRet.second)
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("MissionRank alread exist MissionKey[") << rkMissionKey.str().c_str() << _T("]") );
		m_kRankPool.Delete(pkNew);
	
		ConRank::iterator itor_rank = m_kConRank.find(rkMissionKey);
		pkNew = itor_rank->second;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return pkNew;
}

void PgRankMgr::InitRank(SMissionKey const &rkKey, CONT_MISSION_RANK const &rkRank)
{
	BM::CAutoMutex kLock(m_kMutex);
	// Rank 정보를 기록(DB에서 읽어 온 값 넣기)
	PgMission_Rank* pkRank = GetRank(rkKey);
	if (pkRank == NULL)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Cannot get MissionRank[") << rkKey.str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Rank is NULL"));
		return;
	}
	if (pkRank != NULL)
	{
		pkRank->Init(rkKey, rkRank);
	}
}

void PgRankMgr::ClearRank()
{
	BM::CAutoMutex kLock(m_kMutex);

	ConRank::iterator itr = m_kConRank.begin();
	while( itr != m_kConRank.end() )
	{
		//PgMission_Rank* pkRank = GetRank(rkMissionKey);
		ConRank::mapped_type &Element = itr->second;
		Element->MissionRankClear();
		++itr;
	}
}

bool PgRankMgr::Build(CONT_DEF_MISSION_RESULT const &rkResult, CONT_DEF_MISSION_CANDIDATE const &rkCandi,
					  CONT_DEF_MISSION_ROOT const &rkRoot)
{
	BM::CAutoMutex kLock(m_kMutex);
	if (m_kMissionMgr.Build(rkResult, rkCandi, rkRoot))
	{
		PgMissionContMgr::ConBase const *pkBase = NULL;
		m_kMissionMgr.GetMissionBase(pkBase);
		PgMissionContMgr::ConBase::const_iterator base_itr = pkBase->begin();
		while (base_itr != pkBase->end())
		{
			AddRank(base_itr->first, base_itr->second->LevelLimit_Min(), base_itr->second->LevelLimit_Max());
			++base_itr;
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRankMgr::ProcessMsg(SEventMessage *pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex);
	PACKET_ID_TYPE usType = 0;
	pkMsg->Pop(usType);
	switch (usType)
	{
	case PT_T_N_REQ_MISSION_RANKING:
		{
			RecvPT_T_N_REQ_MISSION_RANKING((BM::Stream*)pkMsg);
		}break;
	case PT_C_M_REQ_RANK_PAGE:
		{
			BM::GUID kMemberGuid;
			SMissionKey kMissionKey;
			pkMsg->Pop(kMemberGuid);
			pkMsg->Pop(kMissionKey);
			SendRankingList(kMissionKey,kMemberGuid);
		}break;
	case PT_C_N_ANS_MISSION_RANK_INPUTMEMO:
		{
			BM::GUID kMemoID;
			std::wstring wstrMemo;
			pkMsg->Pop(kMemoID);
			pkMsg->Pop(wstrMemo);
			UpdateRegister(kMemoID,wstrMemo);
		}break;
	case PT_T_N_NFY_MISSION_REPORT_DATA:
		{
			CONT_MISSION_REPORT kMissionRank;
			PU::TLoadTable_MM(*pkMsg,kMissionRank);

			CONT_MISSION_REPORT::iterator rank_itr;
			for ( rank_itr=kMissionRank.begin(); rank_itr!=kMissionRank.end(); ++rank_itr )
			{
				PgMission_Rank* pkRank = GetRank(rank_itr->first);
				if ( pkRank )
				{
					pkRank->Init(rank_itr->first,rank_itr->second);
				}
			}
		}break;
	case PT_T_N_NFY_MISSION_RANK_DATA:
		{
			SMissionKey kKey;
			CONT_MISSION_RANK kData;
			pkMsg->Pop(kKey);
			PU::TLoadArray_M(*pkMsg,kData);

			InitRank(kKey, kData);
		}break;
	case PT_C_M_REQ_RANK_TOP:
		{
			BM::GUID kMemberGuid;
			SMissionKey kMissionKey;
			pkMsg->Pop(kMemberGuid);
			pkMsg->Pop(kMissionKey);
			SendRankingTop(kMissionKey,kMemberGuid);
		}break;
	case PT_T_N_NFY_MISSION_RANK_CLEAR:
		{
			ClearRank();
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Unhandled packet [") << usType << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

void PgRankMgr::RecvPT_T_N_REQ_MISSION_RANKING(BM::Stream * const pkPacket)
{
	SERVER_IDENTITY kSenderSI;
	ConPlayRankInfo kCUserList;
	BM::GUID kMissionGuid;
	SMissionKey kMissionKey;
	int iPlayTime;
	int iPlayTimePoint;
	SGroundKey kGndKey;

	pkPacket->Pop(kSenderSI);
	PU::TLoadKey_M(*pkPacket, kCUserList);
	pkPacket->Pop(kMissionGuid);
	kMissionKey.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kGndKey);
	pkPacket->Pop(iPlayTime);
	pkPacket->Pop(iPlayTimePoint);

	PgMission_Rank* pkRank = GetRank(kMissionKey);
	
	int iClearTime_Top = 0;
	int iClearTime_Avg = 0;
	if ( pkRank )
	{
		PgMission_Result kTopRankData;
		pkRank->DoAction(kCUserList,iClearTime_Top,iClearTime_Avg);
		pkRank->Get(kTopRankData.GetRankData(),1);

		BM::Stream kResPacket(PT_N_T_RES_MISSION_RANKING);
		kResPacket.Push(kMissionGuid);
		kMissionKey.WriteToPacket(kResPacket);
		kResPacket.Push(kGndKey);
		PU::TWriteKey_M(kResPacket, kCUserList);
		kTopRankData.WriteToPacket(kResPacket);
		kResPacket.Push(iClearTime_Top);
		kResPacket.Push(iClearTime_Avg);
		SendToMissionMgr(kSenderSI.nChannel, kResPacket);

		ConPlayRankInfo::iterator cuser_itr;
		for(cuser_itr=kCUserList.begin();cuser_itr!=kCUserList.end();++cuser_itr)
		{
			if (cuser_itr->kMemoGuid != BM::GUID::NullData())
			{
				AddRegister(pkRank,*cuser_itr);
			}
		}
		return;
	}
	INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot Get Mission_Rank MissionKey[") << kMissionKey.str().c_str() << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkRank is NULL"));
}