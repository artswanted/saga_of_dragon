#include "stdafx.h"
#include "Variant/PgEventQuestBase.h"
#include "PgUserWrapper.h"
#include "Item/PgPostManager.h"
#include "PgEventQuestSystem.h"
#include "PgServerSetMgr.h"

//
PgEventQuestUser::PgEventQuestUser()
	: m_kUser()
{
}
PgEventQuestUser::~PgEventQuestUser()
{
}
bool PgEventQuestUser::CheckUser(BM::GUID const& rkGuid) const
{
	return m_kUser.end() != m_kUser.find( rkGuid );
}
void PgEventQuestUser::AddUser(BM::GUID const& rkGuid)
{
	m_kUser.insert( rkGuid );
}
void PgEventQuestUser::DelUser(BM::GUID const& rkGuid)
{
	m_kUser.erase( rkGuid );
}
void PgEventQuestUser::Clear()
{
	m_kUser.clear();
}

//
PgEventQuestSystemReward::PgEventQuestSystemReward(BM::GUID const& rkBaseGuid)
	: m_kBaseGuid(rkBaseGuid), m_kRewardUserList()
{
}
PgEventQuestSystemReward::~PgEventQuestSystemReward()
{
}
void PgEventQuestSystemReward::AddUser(ContGuidSet const& rkSet)
{
	ContGuidSet::const_iterator iter = rkSet.begin();
	while( rkSet.end() != iter )
	{
		m_kRewardUserList.insert( (*iter) ); // 강제로 추가
		++iter;
	}
}
void PgEventQuestSystemReward::AddUser(BM::GUID const& rkGuid)
{
	SContentsUser kUser;
	if( S_OK == ::GetPlayerByGuid(rkGuid, false, kUser) )
	{
		m_kRewardUserList.insert( rkGuid );

		BM::PgPackedTime const kNowTime( BM::PgPackedTime::LocalTime() );
		CEL::DB_QUERY kQuery(DT_MEMBER, DQT_INSERT_EVENT_QUEST_COMPLETESTATUS, _T("EXEC [DBO].[UP_EventQuest_Insert_CompleteStatus]"));
		kQuery.InsertQueryTarget(m_kBaseGuid);
		kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.ServerIdentity().nRealm) );
		kQuery.PushStrParam( kUser.kCharGuid );
		kQuery.PushStrParam( kUser.kName );
		kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kNowTime) );
		kQuery.QueryOwner(m_kBaseGuid);
		g_kCoreCenter.PushQuery(kQuery);
	}
}
bool PgEventQuestSystemReward::CheckUser(BM::GUID const& rkGuid)
{
	return (m_kRewardUserList.end() != m_kRewardUserList.find(rkGuid));
}
void PgEventQuestSystemReward::GetUserList(bool const bForReward)
{
	CEL::DB_QUERY kQuery(DT_MEMBER, DQT_LOAD_EVENT_QUEST_COMPLETESTATUS, _T("EXEC [DBO].[UP_EventQuest_Load_CompleteStatus]"));
	kQuery.InsertQueryTarget(m_kBaseGuid);
	kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.ServerIdentity().nRealm) );
	kQuery.PushStrParam( ((bForReward)? TRUE: FALSE) );
	kQuery.QueryOwner(m_kBaseGuid);
	g_kCoreCenter.PushQuery(kQuery);
}
void PgEventQuestSystemReward::GiveReward(PgEventQuest const& rkEventQuest)
{
	// 보상메일을 발송 한다.
	ContEventQuestReward const& rkRewardItem = rkEventQuest.RewardItem();
	ContGuidSet::const_iterator iter = m_kRewardUserList.begin();
	while( m_kRewardUserList.end() != iter )
	{
		ContGuidSet::value_type const& rkGuid = (*iter);
		SContentsUser kUser;
		if( S_OK == ::GetPlayerByGuid(rkGuid, false, kUser) ) // 온라인유저만 발송한다
		{
			ContEventQuestReward::const_iterator item_iter = rkRewardItem.begin();
			while( rkRewardItem.end() != item_iter )
			{
				ContEventQuestReward::value_type const& rkItem = (*item_iter);
				__int64 const iGold = (rkRewardItem.begin() == item_iter)? rkEventQuest.RewardGold(): 0;
				g_kPostMgr.PostSystemMail(rkGuid, kUser.kName, rkEventQuest.RewardMailTitle(), rkEventQuest.RewardMailContents(), rkItem.iItemNo, rkItem.iCount, iGold);
				++item_iter;
			}
		}
		++iter;
	}
	m_kRewardUserList.clear(); // 메모리 리워드 목록 초기화
}
void PgEventQuestSystemReward::TruncateUserList(BM::PgPackedTime const& rkStartTime)
{
	m_kRewardUserList.clear();

	CEL::DB_QUERY kQuery(DT_MEMBER, DQT_DELETE_EVENT_QUEST_COMPLETESTATUS, _T("EXEC [DBO].[UP_EventQuest_Delete_CompleteStatus]"));
	kQuery.InsertQueryTarget(m_kBaseGuid);
	kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.ServerIdentity().nRealm) );
	kQuery.PushStrParam( BM::DBTIMESTAMP_EX(rkStartTime) ); // 시작 시간보다 과거의 것들은 전부 제거 한다
	kQuery.QueryOwner(m_kBaseGuid);
	g_kCoreCenter.PushQuery(kQuery);
}


//
PgEventQuestStauts::PgEventQuestStauts(BM::GUID const& rkBaseGuid)
	: m_kBaseGuid(rkBaseGuid), m_kStatus(EQS_None)
{
}
PgEventQuestStauts::~PgEventQuestStauts()
{
}
//bool PgEventQuestStauts::Init()
//{
//	//if( EQS_End == m_kStatus )
//	//{
//		Update(EQS_None);
//		return true;
//	//}
//	//return false;
//}
bool PgEventQuestStauts::Start()
{
	if( EQS_None == m_kStatus
	||	EQS_End == m_kStatus )
	{
		Update(EQS_Ing);
		return true;
	}
	return false;
}
bool PgEventQuestStauts::End()
{
	if( IsNowEvent() )
	{
		Update(EQS_End);
		return true;
	}
	return false;
}
void PgEventQuestStauts::Enable()
{
	Update(EQS_None);
}
void PgEventQuestStauts::Disable()
{
	Update(EQS_Disable);
}
bool PgEventQuestStauts::IsEnable() const
{
	return EQS_Disable != m_kStatus;
}
bool PgEventQuestStauts::IsNowEvent() const
{
	return EQS_Ing == m_kStatus;
}
void PgEventQuestStauts::Update(EEvnetQuestStatus const eUpdate)
{
	if( m_kStatus != eUpdate )
	{
		m_kStatus = eUpdate;

		CEL::DB_QUERY kQuery(DT_MEMBER, DQT_UPDATE_EVENT_QUEST_STATUS, _T("EXEC [DBO].[UP_EventQuest_Update_Status]"));
		kQuery.InsertQueryTarget(m_kBaseGuid);
		kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.ServerIdentity().nRealm) );
		kQuery.PushStrParam( static_cast< int >(m_kStatus) );
		kQuery.PushStrParam( static_cast< int >(((IsEnable())? TRUE: FALSE)) );
		kQuery.QueryOwner(m_kBaseGuid);
		g_kCoreCenter.PushQuery(kQuery);
	}
}


//
PgEventQuestNotice::PgEventQuestNotice()
{
}
PgEventQuestNotice::~PgEventQuestNotice()
{
}
void PgEventQuestNotice::Tick()
{
	SYSTEMTIME kSystemTime;
	::GetLocalTime(&kSystemTime);
	__int64 iNowTime = 0;
	CGameTime::SystemTime2SecTime(kSystemTime, iNowTime);
	__int64 iStartTime = 0;
	CGameTime::SystemTime2SecTime(m_kStartTime, iStartTime);
	__int64 const iDiffTime = InvertSign(iStartTime - iNowTime); // 시작 이전이면

	ContEventQuestNotice::iterator iter = m_kNotice.begin();
	while( m_kNotice.end() != iter )
	{
		ContEventQuestNotice::value_type const& rkElement = (*iter);
		if( iDiffTime > rkElement.iTime )
		{
			NfyNotice( rkElement.kMessage );
			iter = m_kNotice.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}
void PgEventQuestNotice::ReadFromPacket(BM::PgPackedTime const& rkStartTime, BM::Stream& rkPacket)
{
	m_kStartTime = rkStartTime;

	ContEventQuestNotice kTempNotice;
	PU::TLoadArray_M(rkPacket, kTempNotice);

	SYSTEMTIME kSystemTime;
	::GetLocalTime(&kSystemTime);
	__int64 iNowTime = 0;
	CGameTime::SystemTime2SecTime(kSystemTime, iNowTime);
	__int64 iStartTime = 0;
	CGameTime::SystemTime2SecTime(rkStartTime, iStartTime);
	__int64 const iDiffTime = InvertSign(iStartTime - iNowTime); // 시작 이전이면

	m_kNotice.clear();
	ContEventQuestNotice::const_iterator iter = kTempNotice.begin();
	while( kTempNotice.end() != iter )
	{
		ContEventQuestNotice::value_type const& rkElement = (*iter);
		if( iDiffTime < rkElement.iTime )
		{
			m_kNotice.push_back( rkElement );
		}
		++iter;
	}
	m_kNotice.sort();
}
void PgEventQuestNotice::NfyNotice(std::wstring const& rkNotice)
{
	BM::Stream kPacket(PT_M_C_NFY_EVENT_QUEST_NOTICE);
	kPacket.Push( rkNotice );
	g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
}
__int64 PgEventQuestNotice::InvertSign(__int64 const iValue)
{
	return iValue * -1i64;
}


//
PgEventQuestContents::PgEventQuestContents(BM::GUID const& rkGuid)
	: PgEventQuest(), PgEventQuestStauts(rkGuid)
{
}
PgEventQuestContents::~PgEventQuestContents()
{
}
void PgEventQuestContents::ReadFromPacket(BM::Stream& rkPacket, bool const bNoInitStartTime)
{
	BM::PgPackedTime const kBackupStartTime = m_kStartTime;
	PgEventQuest::ReadFromPacket(rkPacket);
	if( bNoInitStartTime )
	{
		m_kStartTime = kBackupStartTime;
	}
}


//
PgEventQuestSystem::PgEventQuestSystem()
	: m_kMutex(), m_kBaseGuid(), m_kEvent(m_kBaseGuid), m_kNotice(), m_kReward(m_kBaseGuid), m_kUser()
{
}
PgEventQuestSystem::~PgEventQuestSystem()
{
}
void PgEventQuestSystem::OnTick()
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !m_kEvent.IsEnable() )
	{
		return;
	}

	if( !m_kEvent.IsEmpty()
	&&	m_kEvent.IsEnable() )
	{
		BM::PgPackedTime const kNowTime( BM::PgPackedTime::LocalTime() );

		m_kNotice.Tick();

		bool const bNowStatus = m_kEvent.IsNowEvent();
		bool const bCanRun = m_kEvent.IsCanRun(kNowTime);
		if( !bNowStatus
		&&	bCanRun ) // 시작
		{
			if( m_kEvent.Start() )
			{
				m_kReward.TruncateUserList(m_kEvent.StartTime());
				m_kReward.GetUserList(false);
				//m_kReward.TruncateUserList(BM::PgPackedTime::LocalTime());
				NfyAllMapServer(false);
			}
			return;
		}
		if( bNowStatus
		&&	!bCanRun ) // 종료
		{
			if( m_kEvent.End() )
			{
				NfyAllMapServer(true);
				m_kReward.GetUserList(true);
				m_kUser.Clear();
			}
			return;
		}
	}
}
bool PgEventQuestSystem::ProcessMsg(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::Stream::DEF_STREAM_TYPE usType = 0;
	rkPacket.Pop( usType );

	switch( usType )
	{
	case PT_N_N_REQ_RELOAD_EVENT_QUEST:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER, DQT_LOAD_EVENT_QUEST, _T("EXEC [DBO].[UP_EventQuest_Load]"));
			kQuery.InsertQueryTarget(m_kBaseGuid);
			kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.ServerIdentity().nRealm) );
			kQuery.QueryOwner(m_kBaseGuid);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_N_N_NFY_LOAD_EVENT_QUEST:
		{
			int iLoadType = 0;
			rkPacket.Pop( iLoadType );

			switch( iLoadType )
			{
			case PgEventQuestSystemUtil::LDT_EVENT_QUEST:
				{
					bool const bIngEvent = m_kEvent.IsNowEvent();
					bool bEnable = false, bNeedInitStatus = false;

					rkPacket.Pop( bEnable );
					rkPacket.Pop( bNeedInitStatus );
					m_kEvent.ReadFromPacket(rkPacket, bIngEvent); // 이미 진행중이면, 시작 시간값을 초기화 시키지 않는다(진행중인데, 이벤트가 끝나는 것을 방지)
					m_kNotice.ReadFromPacket(m_kEvent.StartTime(), rkPacket);

					if( !bEnable
					&&	bNeedInitStatus )
					{
						CEL::DB_QUERY kQuery(DT_MEMBER, DQT_INIT_EVENT_QUEST_STATUS, _T("EXEC [DBO].[UP_EventQuest_Init_Status]"));
						kQuery.InsertQueryTarget(m_kBaseGuid);
						kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.ServerIdentity().nRealm) );
						kQuery.QueryOwner(m_kBaseGuid);
						g_kCoreCenter.PushQuery(kQuery);
					}

					if( 0 >= m_kEvent.EndTime() ) // 종료 시간이 0 보다 작으면 강제로 Disable 시킨다.
					{
						bEnable = false;
					}

					if( bEnable )
					{
						if( !m_kEvent.IsEnable() )
						{
							m_kEvent.Enable();
						}
						if( bIngEvent ) // 진행중이면 수정내용을 다시 알린다
						{
							NfyAllMapServer(false);
						}
					}
					else
					{
						if( m_kEvent.IsEnable() )
						{
							m_kEvent.Disable();
						}
						if( bIngEvent ) // 진행중이면 강제 종료 시킨다.
						{
							// 보상도 줘? 말어?
							NfyAllMapServer(true);
						}
					}
				}break;
			case PgEventQuestSystemUtil::LDT_COMPLETESTATUS_LIST:
				{
					bool bForReward = false;
					ContGuidSet kSet;

					rkPacket.Pop( bForReward );
					PU::TLoadArray_A(rkPacket, kSet);

					m_kReward.AddUser( kSet );
					if( bForReward )
					{
						m_kReward.GiveReward(m_kEvent);
					}
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Wrong EventQuest load type:"<<iLoadType);
				}break;
			}
		}break;
	case PT_M_N_REQ_EVENT_QUEST_ADDUSER:
		{
			BM::GUID kCharGuid;
			rkPacket.Pop( kCharGuid );
			m_kReward.AddUser( kCharGuid );

			BM::Stream kPacket(PT_N_C_NFY_EVENT_QUEST_COMPLETE);
			g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);
		}break;
	case PT_M_N_REQ_EVENT_QUEST_CHECKUSER:
		{
			BM::GUID kCharGuid;
			BM::GUID kNpcGuid;

			rkPacket.Pop( kCharGuid );
			rkPacket.Pop( kNpcGuid );

			if( m_kReward.CheckUser( kCharGuid ) )
			{
				BM::Stream kPacket(PT_M_C_ANS_EVENT_QUEST_TALK); // 이미 완료 했어(클라이언트로 통보)
				kPacket.Push( static_cast< int >(EQTR_Duplicate) );
				kPacket.Push( kNpcGuid );
				g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);
				break;
			}

			PgDoc_Player kPlayer;
			if( g_kRealmUserMgr.Locked_GetDoc_Player(kCharGuid, false, kPlayer) )
			{
				PgInventory const* pkInven = kPlayer.GetInven();
				if( pkInven )
				{
					ContEventQuestTarget const& rkTarget = m_kEvent.TargetItem();
					ContEventQuestTarget::const_iterator iter = rkTarget.begin();
					while( rkTarget.end() != iter )
					{
						ContEventQuestTarget::value_type const& rkTargetItem = (*iter);
						if( static_cast< size_t >(rkTargetItem.iCount) > pkInven->GetTotalCount(rkTargetItem.iItemNo) )
						{
							BM::Stream kPacket(PT_M_C_ANS_EVENT_QUEST_TALK); // 이미 완료 했어(클라이언트로 통보)
							kPacket.Push( static_cast< int >(EQTR_NotEnd) );
							kPacket.Push( kNpcGuid );
							g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);
							break;
						}
						++iter;
					}
				}
			}

			{
				BM::Stream kPacket(PT_C_M_REQ_EVENT_QUEST_TALK); // 없어 아이템 빼앗고, 등록 시켜(그라운드로 통보)
				kPacket.Push( kCharGuid );
				kPacket.Push( kNpcGuid );
				PU::TWriteArray_M(kPacket, m_kEvent.TargetItem()); // DB가 바뀌면 어찌 될지 모르니 정보 밖아 넣어
				g_kRealmUserMgr.Locked_SendToUserGround(kCharGuid, kPacket, false, true);
			}
		}break;
	case PT_T_N_NFY_USER_ENTER_GROUND:
		{
			SAnsMapMove_MT kAMM;
			SContentsUser kUser;

			rkPacket.Pop(kAMM);
			kUser.ReadFromPacket(rkPacket);

			if( m_kEvent.IsNowEvent() )
			{
				if( !kUser.Empty() )
				{
					if( !m_kUser.CheckUser(kUser.kCharGuid) )
					{
						m_kUser.AddUser(kUser.kCharGuid);
						NfyUser(kUser, m_kEvent);
					}
				}
			}
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			SContentsUser kUser;

			kUser.ReadFromPacket(rkPacket);

			if( m_kEvent.IsNowEvent() )
			{
				m_kUser.DelUser(kUser.kCharGuid);
			}
		}break;
	case PT_N_N_REQ_EVENT_QUEST_UNLOCK_CHARS:
		{
			BM::GUID kMemberGUID;
			rkPacket.Pop(kMemberGUID);
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_UPDATE_USER_UNLOCK_ALL_CHARACTERS, _T("EXEC [dbo].[up_UnlockAllCharacters]"));
			kQuery.InsertQueryTarget(kMemberGUID);
			kQuery.QueryOwner(kMemberGUID);
			kQuery.PushStrParam(kMemberGUID);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Wrong EventQuest packet type:"<<usType);
		}break;
	}

	return true;
}
void PgEventQuestSystem::NfyAllMapServer(bool const bClearInfo)
{
	BM::Stream kPacket(PT_M_C_NFY_EVENT_QUEST_INFO);
	kPacket.Push( bClearInfo );
	if( !bClearInfo )
	{
		m_kEvent.WriteToMapPacket( kPacket );
	}
	g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
}
void PgEventQuestSystem::NfyUser(SContentsUser const& rkUser, PgEventQuest const& rkEvent)
{
	bool const bClearInfo = false;
	bool const bIncludeCompleteInfo = true;

	BM::Stream kPacket(PT_M_C_NFY_EVENT_QUEST_INFO);
	kPacket.Push( false );
	kPacket.Push( bIncludeCompleteInfo );
	kPacket.Push( m_kReward.CheckUser(rkUser.kCharGuid) ); // 완료 했냐?
	rkEvent.WriteToClientPacket( kPacket );
	g_kRealmUserMgr.Locked_SendToUser(rkUser.kMemGuid, kPacket, true);
}


// DB query
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace PgEventQuestSystemUtil
{
	bool Q_DQT_LOAD_EVENT_QUEST(CEL::DB_RESULT &rkResult)
	{
		if( CEL::DR_SUCCESS != rkResult.eRet )
		{
			return false;
		}

		ContEventQuestNotice kTempNotice;
		CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();

		int const iNoticeCount = (*count_iter);	++count_iter;
		for( int iCur = 0; iNoticeCount > iCur; ++iCur )
		{
			ContEventQuestNotice::value_type kElement;
			(*result_iter).Pop( kElement.iTime );		++result_iter;
			(*result_iter).Pop( kElement.kMessage );	++result_iter;
			kTempNotice.push_back( kElement );
		}

		PgEventQuest kTempEvent;
		kTempEvent.ReadFromDBResult(count_iter, result_iter);

		int const iEnableCount = (*count_iter);
		int iEnable = 0;
		if( iEnableCount )
		{
			(*result_iter).Pop( iEnable );
		}

		BM::Stream kPacket( PT_N_N_NFY_LOAD_EVENT_QUEST, static_cast< int >(LDT_EVENT_QUEST) ); // Load
		kPacket.Push( (0 != iEnable) );
		kPacket.Push( (0 == iEnableCount) ); // 상태 테이블(TB_EventQuest_Status)의 행(Row)가 없다
		kTempEvent.WriteToPacket( kPacket );
		PU::TWriteArray_M(kPacket, kTempNotice);
		::SendToRealmContents(PMET_EVENTQUEST, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	bool Q_DQT_LOAD_EVENT_QUEST_COMPLETESTATUS(CEL::DB_RESULT& rkResult)
	{
		if( CEL::DR_SUCCESS != rkResult.eRet )
		{
			return false;
		}

		ContGuidSet kSet;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();

		int iForReward = 0;
		{
			(*result_iter).Pop( iForReward );	++result_iter;
		}

		while( rkResult.vecArray.end() != result_iter )
		{
			ContGuidSet::value_type kGuid;
			(*result_iter).Pop( kGuid );		++result_iter;
			kSet.insert( kGuid );
		}

		BM::Stream kPacket( PT_N_N_NFY_LOAD_EVENT_QUEST, static_cast< int >(LDT_COMPLETESTATUS_LIST) );
		kPacket.Push( (0 != iForReward) );
		PU::TWriteArray_A(kPacket, kSet);
		::SendToRealmContents(PMET_EVENTQUEST, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	bool Q_DQT_INIT_EVENT_QUEST_STATUS(CEL::DB_RESULT &rkResult)
	{
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			return false;
		}

		BM::Stream kPacket(PT_N_N_REQ_RELOAD_EVENT_QUEST);
		::SendToRealmContents(PMET_EVENTQUEST, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	bool Q_DQT_EVENT_QUEST_COMMON(CEL::DB_RESULT &rkResult)
	{
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
			return false;
		}
		//CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		//while(kIter != rkResult.vecArray.end())
		//{
		//	CONT_LIMITED_ITEM_RECORD::mapped_type	kValue;
		//	kIter->Pop(kValue.iSafeCount);			++kIter;
		//	kCont.insert(std::make_pair(kValue.iEventNo, kValue));
		//}
		g_kCoreCenter.ClearQueryResult(rkResult);
		return true;
	}
}