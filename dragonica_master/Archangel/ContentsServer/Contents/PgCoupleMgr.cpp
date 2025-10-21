#include "stdafx.h"
#include "Lohengrin/GameTime.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/DefAbilType.h"
#include "PgCoupleMgr.h"
#include "Variant/PgQuestInfo.h"

namespace PgCoupleMgrUtil
{
	void SetSweetHeartInfo(SContentsUser const &rkUser, BM::GUID const &rkTargetGuid, ECoupleCommand eType, int const iValue, BM::PgPackedTime const & kDate, BM::Stream const &rkAddonPacket = BM::Stream())
	{
		switch( eType )
		{
		case CC_SweetHeartQuest_Cancel:
		case CC_Ans_SweetHeart_Complete:
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;

				SPMOD_SetSweetHeart kSetData(eType, rkTargetGuid, iValue, kDate);
				kOrder.push_back( SPMO(IMET_SET_SWEETHEART, rkUser.kCharGuid, kSetData) );

				BM::Stream kPacket(PT_M_I_REQ_MODIFY_ITEM);
				kPacket.Push( (int)IMEPT_COUPLE );
				kPacket.Push( rkUser.kCharGuid );
				kOrder.WriteToPacket(kPacket);
				
				if( rkAddonPacket.IsEmpty() )
				{
					kPacket.Push((bool)false);
				}
				else
				{
					kPacket.Push((bool)true);
					kPacket.Push(rkAddonPacket.Data());
				}

				SERVER_IDENTITY kNullSI;
				kNullSI.nChannel = rkUser.sChannel;
				::SendToItem(kNullSI, rkUser.kGndKey, kPacket);
			}break;
		default:
			{
			}break;
		}
	}

	void SaveSweetHeartInfo(BM::GUID const &rkCharGuid, BM::GUID const &rkTargetGuid, ECoupleCommand eType, int const iValue, BM::PgPackedTime const & kDate, BM::Stream const &rkAddonPacket = BM::Stream())
	{
		SContentsUser kUser;
		if( S_OK == GetPlayerByGuid(rkCharGuid, false, kUser) )
		{
			SetSweetHeartInfo(kUser, rkTargetGuid, eType, iValue, kDate, rkAddonPacket);
		}
	}

	void SetCoupleGuid(SContentsUser const &rkUser, BM::GUID const &rkGuid, BM::PgPackedTime const & kDate, BM::Stream const &rkAddonPacket = BM::Stream())
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;

		SPMOD_SetGuid kSetGuidData(SGT_Couple, rkGuid);
		kOrder.push_back( SPMO(IMET_SET_GUID, rkUser.kCharGuid, kSetGuidData) );

		BM::Stream kPacket(PT_M_I_REQ_MODIFY_ITEM);
		kPacket.Push( (int)IMEPT_COUPLE);
		kPacket.Push( rkUser.kCharGuid );
		kOrder.WriteToPacket(kPacket);
		
		if( rkAddonPacket.IsEmpty() )
		{
			kPacket.Push((bool)false);
		}
		else
		{
			kPacket.Push((bool)true);
			kPacket.Push(rkAddonPacket.Data());
		}

		SERVER_IDENTITY kNullSI;
		kNullSI.nChannel = rkUser.sChannel;
		::SendToItem(kNullSI, rkUser.kGndKey, kPacket);

		if( BM::GUID::IsNull(rkGuid) )
		{
			SetSweetHeartInfo(rkUser, rkUser.kCharGuid, CC_SweetHeartQuest_Cancel, 0, kDate, rkAddonPacket);	// 연인 퀘스트 초기화
		}
	}

	void SetCoupleGuidByGuid(BM::GUID const &rkCharGuid, BM::GUID const &rkGuid, BM::PgPackedTime const & kDate, BM::Stream const &rkAddonPacket = BM::Stream())
	{
		SContentsUser kUser;
		if( S_OK == GetPlayerByGuid(rkCharGuid, false, kUser) )
		{
			SetCoupleGuid(kUser, rkGuid, kDate, rkAddonPacket);
		}
	}

	void SetCoupleColorGuid(SContentsUser const &rkUser, BM::GUID const &rkGuid, BM::Stream const &rkAddonPacket = BM::Stream())
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;

		SPMOD_SetGuid kSetGuidData(SGT_CoupleColor, rkGuid);
		kOrder.push_back( SPMO(IMET_SET_GUID, rkUser.kCharGuid, kSetGuidData) );

		BM::Stream kPacket(PT_M_I_REQ_MODIFY_ITEM);
		kPacket.Push( (int)IMEPT_COUPLE);
		kPacket.Push( rkUser.kCharGuid );
		kOrder.WriteToPacket(kPacket);
		
		if( rkAddonPacket.IsEmpty() )
		{
			kPacket.Push((bool)false);
		}
		else
		{
			kPacket.Push((bool)true);
			kPacket.Push(rkAddonPacket.Data());
		}

		SERVER_IDENTITY kNullSI;
		kNullSI.nChannel = rkUser.sChannel;
		::SendToItem(kNullSI, rkUser.kGndKey, kPacket);
	}
}


//#define DEACTIVATE_COUPLE_SYSTEM (1)

__int64 const iCouple_Infinity = 0;
__int64 const iCouple_OneDay = CGameTime::OneDay;

int PgCoupleMgr::iCoupleUse = 1;
__int64 PgCoupleMgr::iPanaltyDay = (CGameTime::OneDay * 3);
int const PgCoupleMgr::iCoupleSkillNo[] = {90000001, 100006301, 100006401, 100006501, 90000701, 0};//끝에 반드시 0을 넣어줘야 읽을때 거기까지만 읽게 된다!

tagCoupleInfo::tagCoupleInfo(BM::GUID const &rkWhoGuid)
{
	Clear();
	m_kAccepted = true;
	m_kCoupleGuid = rkWhoGuid;
}

void tagCoupleInfo::Accept(SContentsUser const &rkHim, BM::PgPackedTime const &rkNow, __int64 const iLimitTime)
{
	m_kAccepted = true;
	m_kBreaked = false;
	m_kCoupleGuid = rkHim.kCharGuid;
	m_kCharName = rkHim.kName;
	m_kStartDate = rkNow;
	m_kHomeAddr = rkHim.kHomeAddr;

	if( iLimitTime )
	{
		m_kLimitDate = m_kStartDate;
		CGameTime::AddTime(m_kLimitDate, iLimitTime);
	}
}

void tagCoupleInfo::Request(BM::GUID const &rkRequesterGuid)
{
	m_kCoupleGuid = rkRequesterGuid;
}

void tagCoupleInfo::CoupleLimitTime(BM::PgPackedTime const &rkLimitDate)
{
	m_kLimitDate = rkLimitDate;
}

void tagCoupleInfo::SweetHeartPanalty(BM::PgPackedTime const &rkNow)
{
	m_kCouplePanaltyLimitDate = rkNow;
}

PgCoupleMatcher::PgCoupleMatcher(size_t const iMaxCount, int const iLevelDiff, SContentsUser const &rkUser, ContContentsUser &rkOut, bool const bMinLevel)
:m_iMaxCount(iMaxCount), m_iLevelDiff(iLevelDiff), m_iCurLevel((int)rkUser.sLevel), m_kOut(rkOut), m_pkContDefMap(NULL)
{
	switch( rkUser.cGender )
	{
	case UG_MALE:
		{
			m_iTargetGender = UG_FEMALE;
		}break;
	case UG_FEMALE:
		{
			m_iTargetGender = UG_MALE;
		}break;
	default:
		{
			//넌 누구냐!?
			m_iTargetGender = 0;
		}
	}

	m_sCurChannel = rkUser.sChannel;

	short const sMinPlayerLevel = 5;
	if( bMinLevel
	&&	sMinPlayerLevel > rkUser.sLevel )
	{
		m_iTargetGender = 0;// 나이좀 먹고 와라잉~?
	}

	g_kTblDataMgr.GetContDef(m_pkContDefMap);
}

bool PgCoupleMatcher::operator () ( std::pair< CONT_CENTER_PLAYER_BY_KEY::key_type, CONT_CENTER_PLAYER_BY_KEY::mapped_type > const &rkPair) const
{
	if( !m_pkContDefMap )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;// break
	}

	if( !m_iTargetGender )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;// break
	}

	if( !rkPair.second )
	{
		return true;
	}

	{
		BM::CAutoMutex kLock(rkPair.second->kMutex);

		if( m_sCurChannel != rkPair.second->kSwitchServer.nChannel )
		{
			return true;// Channel
		}

		PgPlayer* pkPlayer = rkPair.second->pkPlayer;
		if( !pkPlayer )
		{
			return true;
		}

		CONT_DEFMAP::const_iterator map_iter = m_pkContDefMap->find( pkPlayer->GroundKey().GroundNo() );
		if( m_pkContDefMap->end() == map_iter )
		{
			return true;// Pass Ground
		}

		if( BM::GUID::IsNotNull(pkPlayer->CoupleGuid()) )
		{
			return true;// Have Couple
		}

		if( m_iTargetGender != pkPlayer->GetAbil(AT_GENDER) )
		{
			return true;// Gender
		}

		int const iLevelDiff = abs( m_iCurLevel - pkPlayer->GetAbil(AT_LEVEL) );
		if( m_iLevelDiff < iLevelDiff )
		{
			return true;// Level Limit
		}

		ContContentsUser::value_type kNew;
		rkPair.second->Copy(kNew);

		std::inserter(m_kOut, m_kOut.end()) = kNew;

		if( m_iMaxCount == m_kOut.size() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;// Full Count
		}
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
PgCoupleMgr::PgCoupleMgr()
{
}

PgCoupleMgr::~PgCoupleMgr()
{
}

ECoupleCommandResult PgCoupleMgr::ReqCouple(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	bool bByName = false, bInstance = false;
	BM::GUID const &kRequesterGuid = rkCharGuid;
	SContentsUser kRequester;
	SContentsUser kAnswerer;

	rkPacket.Pop( bByName );
	rkPacket.Pop( bInstance );

	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kRequester) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}
	if( !bByName )
	{
		BM::GUID kAnswererGuid;
		rkPacket.Pop( kAnswererGuid );
		if( S_OK != ::GetPlayerByGuid(kAnswererGuid, false, kAnswerer) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
			return CoupleCR_NotFind;
		}
	}
	else// By Guid
	{
		std::wstring kAnswererName;
		rkPacket.Pop(kAnswererName);
		if( S_OK != ::GetPlayerByName(kAnswererName, kAnswerer) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
			return CoupleCR_NotFind;
		}
	}

	if( kRequester.kCharGuid == kAnswerer.kCharGuid )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotGender"));
		return CoupleCR_NotGender; 
	} // 자신은 안되
	if( kRequester.cGender == kAnswerer.cGender )		
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotGender"));
		return CoupleCR_NotGender; 
	} // 동성은 안되
	mapped_type *pkRequesterCoupleInfo = Get(kRequester.kCharGuid);
	mapped_type *pkAnswererCoupleInfo = Get(kAnswerer.kCharGuid);
	if( !pkRequesterCoupleInfo )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( !pkAnswererCoupleInfo )		
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( BM::GUID::IsNotNull(pkRequesterCoupleInfo->CoupleGuid()) )	
	{
		if( !pkRequesterCoupleInfo->Accepted() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_ReqWait"));
			return CoupleCR_ReqWait;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Pre"));
		return CoupleCR_Pre; 
	}// 양다리?(딴놈이 날 찍었어)
	if( BM::GUID::IsNotNull(pkAnswererCoupleInfo->CoupleGuid()) )	
	{
		if( !pkAnswererCoupleInfo->Accepted() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_ReqWait"));
			return CoupleCR_ReqWait;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotSingle"));
		return CoupleCR_NotSingle; 
	}// 골키퍼 있다(혹은 신청자 있다)
	if( SweetHeartPanaltyCheck(pkRequesterCoupleInfo) )
	{
		RejectCouplePanalty(*pkRequesterCoupleInfo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleLimitDate is True"));
		return CoupleCR_SweetHeart_Panalty; // 안지났다
	}
	if( SweetHeartPanaltyCheck(pkAnswererCoupleInfo) )
	{
		RejectCouplePanalty(*pkAnswererCoupleInfo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleLimitDate is True"));
		return CoupleCR_SweetHeart_Panalty; // 안지났다
	}

	pkRequesterCoupleInfo->Request( kAnswerer.kCharGuid );	// 난 니꺼
	pkAnswererCoupleInfo->Request( kRequesterGuid );		// 넌 내꺼

	BM::Stream kRequesterPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Couple);
	kRequesterPacket.Push( (BYTE)CoupleCR_Success );
	kRequesterPacket.Push( kAnswerer.kName );				// 누구에게
	kRequesterPacket.Push( bInstance );						// 
	g_kRealmUserMgr.Locked_SendToUser(kRequester.kMemGuid, kRequesterPacket);		// 커플 신청 성공

	BM::Stream kAnsererPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Couple);
	kAnsererPacket.Push( (BYTE)CoupleCR_Notify );
	kAnsererPacket.Push( kRequester.kCharGuid );
	kAnsererPacket.Push( kRequester.kName );				// 누구로 부터
	kAnsererPacket.Push( bInstance );						// 
	g_kRealmUserMgr.Locked_SendToUser(kAnswerer.kMemGuid, kAnsererPacket);			// 커플 신청 했다 어떻게 할래?

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::AnsCouple(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	bool bSayYes = false, bInstance = false;
	BM::GUID const &kAnswererGuid = rkCharGuid;
	BM::GUID kRequesterGuid;
	SContentsUser kAnswerer;
	SContentsUser kRequester;

	rkPacket.Pop( kRequesterGuid );
	rkPacket.Pop( bSayYes );
	rkPacket.Pop( bInstance );
	
	if( S_OK != ::GetPlayerByGuid(kAnswererGuid, false, kAnswerer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}
	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kRequester) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	if( kRequester.cGender == kAnswerer.cGender )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotGender"));
		return CoupleCR_NotGender; 
	} // 동성은 안되
	mapped_type *pkAnswererCoupleInfo = Get(kAnswerer.kCharGuid);
	mapped_type *pkRequesterCoupleInfo = Get(kRequester.kCharGuid);
	if( !pkAnswererCoupleInfo )		
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( !pkRequesterCoupleInfo )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( pkAnswererCoupleInfo->CoupleGuid() != kRequester.kCharGuid )
	{
		// 뭥미? 딴놈한테 관심 있는겨?
		RejectCouple(kAnswerer, *pkAnswererCoupleInfo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}
	if( SweetHeartPanaltyCheck(pkRequesterCoupleInfo) )
	{
		RejectCouplePanalty(*pkRequesterCoupleInfo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleLimitDate is True"));
		return CoupleCR_SweetHeart_Panalty; // 안지났다
	}
	if( SweetHeartPanaltyCheck(pkAnswererCoupleInfo) )
	{
		RejectCouplePanalty(*pkAnswererCoupleInfo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleLimitDate is True"));
		return CoupleCR_SweetHeart_Panalty; // 안지났다
	}

	if( bSayYes )
	{
		BM::PgPackedTime const kNow = BM::PgPackedTime::LocalTime();

		__int64 const iLimitTime = (!bInstance)? iCouple_Infinity: iCouple_OneDay;

		pkRequesterCoupleInfo->Accept( kAnswerer, kNow, iLimitTime );		// 난 니꺼
		pkAnswererCoupleInfo->Accept( kRequester, kNow, iLimitTime );		// 넌 내꺼

		BM::Stream kRequesterPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Ans_Couple);
		kRequesterPacket.Push( (BYTE)CoupleCR_Success );
		kRequesterPacket.Push( kRequester.kCharGuid );
		kRequesterPacket.Push( kAnswerer.kCharGuid );

		COUPLE_SKILL_LIST kStack;
		int i = 0;
		while(iCoupleSkillNo[i])
		{
			kStack.push_back(iCoupleSkillNo[i]);
			++i;
		}
		PU::TWriteArray_A(kRequesterPacket, kStack);
											// 이 스킬 배워라
		
		kRequesterPacket.Push( kRequester.kName );
		kRequesterPacket.Push( kAnswerer.kName );											// 누구와
		kRequesterPacket.Push( bInstance );													// 1일 ?
		g_kRealmUserMgr.Locked_SendToUserGround(kRequester.kMemGuid, kRequesterPacket, true, true);				// 커플 되었으

		pkRequesterCoupleInfo->SweetHeartPanalty(kNow);
		PgCoupleMgrUtil::SetCoupleGuid(kRequester, kAnswerer.kCharGuid, kNow);

		BM::Stream kAnswererPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Ans_Couple);
		kAnswererPacket.Push( (BYTE)CoupleCR_Success );
		kAnswererPacket.Push( kAnswerer.kCharGuid );
		kAnswererPacket.Push( kRequester.kCharGuid );
		
		PU::TWriteArray_A(kAnswererPacket, kStack);

		kAnswererPacket.Push( kAnswerer.kName );
		kAnswererPacket.Push( kRequester.kName );											// 누구와
		kAnswererPacket.Push( bInstance );													// 1일 ?
		g_kRealmUserMgr.Locked_SendToUserGround(kAnswerer.kMemGuid, kAnswererPacket, true, true);					// 커플 되었으

		pkAnswererCoupleInfo->SweetHeartPanalty(kNow);
		PgCoupleMgrUtil::SetCoupleGuid(kAnswerer, kRequester.kCharGuid, kNow);

		// Color
		BM::GUID kCoupleColorGuid = BM::GUID::NullData();
		GetCoupleColorGuid(kRequester.kCharGuid, kAnswerer.kCharGuid, kCoupleColorGuid);
		// Setting
		pkRequesterCoupleInfo->ColorGuid( kCoupleColorGuid );
		pkAnswererCoupleInfo->ColorGuid( kCoupleColorGuid );
		PgCoupleMgrUtil::SetCoupleColorGuid(kRequester, kCoupleColorGuid);
		PgCoupleMgrUtil::SetCoupleColorGuid(kAnswerer, kCoupleColorGuid);
		SaveCoupleInfo(kRequester.kCharGuid, pkRequesterCoupleInfo, CoupleS_Normal);
		SaveCoupleInfo(kAnswerer.kCharGuid, pkAnswererCoupleInfo, CoupleS_Normal);

		CoupleStatusSave(kRequester, CoupleS_Normal);
		CoupleStatusSave(kAnswerer, CoupleS_Normal);

		SendGndCoupleStatus(kRequester);
		SendGndCoupleStatus(kAnswerer);

		// 커플 업적
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(kRequesterGuid);
		pkActionOrder->InsertTarget(kAnswererGuid);
		pkActionOrder->kCause = CAE_Achievement;
		if(false == bInstance)
		{
			pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kRequesterGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_COUPLE,1)));
			pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kAnswererGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_COUPLE,1)));
		}
		else
		{
			pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kRequesterGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_1DAYCOUPLE,1)));
			pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kAnswererGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_1DAYCOUPLE,1)));
		}
		g_kJobDispatcher.VPush(pkActionOrder);
	}
	else
	{
		RejectCouple(kAnswerer, *pkAnswererCoupleInfo);										// 거절 당했으
	}

	// Log
	PgLogCont kLogCont(ELogMain_Contents_Cupple, ELogSub_Cupple_Apply);				
	kLogCont.MemberKey(kRequester.kMemGuid);
	kLogCont.UID(kRequester.iUID);
	kLogCont.CharacterKey(kRequester.kCharGuid);
	kLogCont.ID(kRequester.kAccountID);
	kLogCont.Name(kRequester.Name());
	kLogCont.ChannelNo(static_cast<short>(kRequester.sChannel));
	kLogCont.Class(static_cast<short>(kRequester.iClass));
	kLogCont.Level(static_cast<short>(kRequester.sLevel));
	kLogCont.GroundNo(static_cast<int>(kRequester.kGndKey.GroundNo()));

	PgLog kLog(ELOrderMain_Cupple, ELOrderSub_Apply);
	kLog.Set(0, static_cast<std::wstring>(kAnswerer.Name()));

	int i = 0;
	while(iCoupleSkillNo[i])
	{
		kLog.Set(0, iCoupleSkillNo[i]);
		++i;
	}
	
	kLog.Set(2, kAnswerer.kCharGuid.str());

	kLogCont.Add(kLog);
	kLogCont.Commit();

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::BreakCouple(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &kBreakerGuid = rkCharGuid;
	SContentsUser kBreaker;
	SContentsUser kHim;

	if( S_OK != ::GetPlayerByGuid(kBreakerGuid, false, kBreaker) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}

	mapped_type *pkBreakerCoupleInfo = Get(kBreakerGuid);
	if( !pkBreakerCoupleInfo )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}
	if( BM::GUID::IsNull(pkBreakerCoupleInfo->CoupleGuid()) )
	{
		// 커플이 원래 없어
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_None"));
		return CoupleCR_None;
	}

	BYTE cStatus = pkBreakerCoupleInfo->cStatus();

	BM::GUID const kHimGuid = pkBreakerCoupleInfo->CoupleGuid();
	pkBreakerCoupleInfo->Clear();

	BM::Stream kBreakerPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Break);
	kBreakerPacket.Push( (BYTE)CoupleCR_Success );
	kBreakerPacket.Push( kBreaker.kCharGuid );							// 내 캐릭터
	g_kRealmUserMgr.Locked_SendToUserGround(kBreaker.kMemGuid, kBreakerPacket, true, true);	// 잘 해어젔어

	BM::PgPackedTime kNow;
	if( cStatus == (CoupleS_Normal | CoupleS_SweetHeart) )
	{
		// 연인이면 패널티 적용
		__int64 const iLimitTime = (iPanaltyDay);
		kNow = BM::PgPackedTime::LocalTime();
		CGameTime::AddTime(kNow, iLimitTime);

		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(kBreaker.kCharGuid);
		pkActionOrder->InsertTarget(kHim.kCharGuid);
		pkActionOrder->kCause = CAE_Achievement;
		pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kBreaker.kCharGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_DIVORCE,1)));
		pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kHim.kCharGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_DIVORCE,1)));
		g_kJobDispatcher.VPush(pkActionOrder);
	}
	pkBreakerCoupleInfo->SweetHeartPanalty(kNow);

	SetCoupleQuest(kBreaker, true, 0, 0);
	SaveDBSweetHeartInfo(kBreaker.kCharGuid, NULL);
	PgCoupleMgrUtil::SetCoupleGuid(kBreaker, BM::GUID::NullData(), kNow);	
	SaveCoupleInfo(kBreaker.kCharGuid, pkBreakerCoupleInfo, CoupleS_Normal);
	CoupleStatusSave(kBreaker, CoupleS_Normal);

	//
	mapped_type *pkHimCoupleInfo = Get(kHimGuid);

	if( pkHimCoupleInfo
	&&	S_OK == GetPlayerByGuid(kHimGuid, false, kHim) )
	{
		// 게임에 있고, 같은 채널이야
		pkHimCoupleInfo->Clear();

		BM::Stream kHimPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Break);
		kHimPacket.Push( (BYTE)CoupleCR_Notify );
		kHimPacket.Push( kHim.kCharGuid );							// 내 캐릭터
		kHimPacket.Push( kBreaker.kName );
		g_kRealmUserMgr.Locked_SendToUserGround(kHim.kMemGuid, kHimPacket, true, true);	// 해어짐 당했어

		pkHimCoupleInfo->SweetHeartPanalty(kNow);

		SetCoupleQuest(kHim, true, 0, 0);
		SaveDBSweetHeartInfo(kHim.kCharGuid, NULL);
		PgCoupleMgrUtil::SetCoupleGuid(kHim, BM::GUID::NullData(), kNow);
		SaveCoupleInfo(kHim.kCharGuid, pkHimCoupleInfo, CoupleS_Normal);
		CoupleStatusSave(kHim, CoupleS_Normal);
	}
	//else if( S_OK == GetPlayerByGuid(kHimGuid, false, kHim, true) )
	//{
	//	// 다른 채널
	//}
	else
	{
		// 로그 오프 중이야
		mapped_type kNewData(kBreakerGuid);						// 넌 이미 차였어
		SaveCoupleInfo(kHimGuid, &kNewData, CoupleS_Breaked);	// 삭제 DB에 저장
		SaveDBSweetHeartInfo(kHimGuid, NULL, kNow);
	}

	// Log
	PgLogCont kLogCont(ELogMain_Contents_Cupple, ELogSub_Cupple_Break);				
	kLogCont.MemberKey(kBreaker.kMemGuid);
	kLogCont.UID(kBreaker.iUID);
	kLogCont.CharacterKey(kBreaker.kCharGuid);
	kLogCont.ID(kBreaker.kAccountID);
	kLogCont.Name(kBreaker.Name());
	kLogCont.ChannelNo(static_cast<short>(kBreaker.sChannel));
	kLogCont.Class(static_cast<short>(kBreaker.iClass));
	kLogCont.Level(static_cast<short>(kBreaker.sLevel));
	kLogCont.GroundNo(static_cast<int>(kBreaker.kGndKey.GroundNo()));

	PgLog kLog(ELOrderMain_Cupple, ELOrderSub_Break);
	kLog.Set(0, static_cast<std::wstring>(kHim.Name()));
	
	int i = 0;
	while(iCoupleSkillNo[i])
	{
		kLog.Set(0, iCoupleSkillNo[i]);
		++i;
	}
	kLog.Set(2, kHim.kCharGuid.str());

	kLogCont.Add(kLog);
	kLogCont.Commit();

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::ReqSweetHeartComplete(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &kRequesterGuid = rkCharGuid;
	SContentsUser kRequester;
	SContentsUser kAnswerer;

	int iQuestID = 0;
	BM::GUID kAnswererGuid;
	rkPacket.Pop( kAnswererGuid );
	rkPacket.Pop( iQuestID );

	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kRequester) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}

	if( S_OK != ::GetPlayerByGuid(kAnswererGuid, false, kAnswerer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	mapped_type *pkRequesterCoupleInfo = Get(kRequester.kCharGuid);
	mapped_type *pkAnswererCoupleInfo = Get(kAnswerer.kCharGuid);

	if( !pkRequesterCoupleInfo )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( !pkAnswererCoupleInfo )		
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}

	BM::Stream kRequesterPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_SweetHeart_Complete);
	kRequesterPacket.Push( (BYTE)CoupleCR_Success );
	kRequesterPacket.Push( kAnswerer.kName );				// 누구에게
	g_kRealmUserMgr.Locked_SendToUser(kRequester.kMemGuid, kRequesterPacket);		// 연인 조건 신청 성공

	BM::Stream kAnsererPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_SweetHeart_Complete);
	kAnsererPacket.Push( (BYTE)CoupleCR_Notify );
	kAnsererPacket.Push( kRequester.kCharGuid );
	kAnsererPacket.Push( kRequester.kName );				// 누구로 부터
	kAnsererPacket.Push( iQuestID );
	g_kRealmUserMgr.Locked_SendToUser(kAnswerer.kMemGuid, kAnsererPacket);			// 연인 조건 신청했다 어떻게 할래?

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::AnsSweetHeartComplete(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	int iSkillNo = 0;
	int iQuestID = 0;
	bool bSayYes = false;
	BM::GUID const &kAnswererGuid = rkCharGuid;
	BM::GUID kRequesterGuid;
	SContentsUser kAnswerer;
	SContentsUser kRequester;

	rkPacket.Pop( kRequesterGuid );
	rkPacket.Pop( bSayYes );	
	rkPacket.Pop( iQuestID );
	rkPacket.Pop( iSkillNo );

	if( S_OK != ::GetPlayerByGuid(kAnswererGuid, false, kAnswerer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}
	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kRequester) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	mapped_type *pkAnswererCoupleInfo = Get(kAnswerer.kCharGuid);
	mapped_type *pkRequesterCoupleInfo = Get(kRequester.kCharGuid);
	if( !pkAnswererCoupleInfo )		
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( !pkRequesterCoupleInfo )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}

	if( bSayYes )
	{
		if( 0 < iSkillNo )
		{
			BM::Stream kRequesterPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_CoupleSkill);
			kRequesterPacket.Push( (BYTE)CoupleCR_Success );
			kRequesterPacket.Push( kRequester.kCharGuid );
			COUPLE_SKILL_LIST kStack;
			kStack.push_back(iSkillNo);
			PU::TWriteArray_A(kRequesterPacket, kStack);
			g_kRealmUserMgr.Locked_SendToUserGround(kRequester.kMemGuid, kRequesterPacket, true, true);
			pkRequesterCoupleInfo->LearnSkill(iSkillNo);
		}

		BM::PgPackedTime kNow;
		PgCoupleMgrUtil::SaveSweetHeartInfo(kRequester.kCharGuid, kRequester.kCharGuid, CC_Ans_SweetHeart_Complete, 0, kNow);
		SetCoupleQuest(kRequester, true, 0, 0);
		pkRequesterCoupleInfo->CoupleLimitTime(kNow);
		SaveCoupleInfo(kRequester.kCharGuid, pkRequesterCoupleInfo, (CoupleS_Normal | CoupleS_SweetHeart));
		CoupleStatusSave(kRequester, (CoupleS_Normal | CoupleS_SweetHeart));
		//SendGndCoupleStatus(kRequester);

		if( 0 < iSkillNo )
		{
			BM::Stream kAnswererPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_CoupleSkill);
			kAnswererPacket.Push( (BYTE)CoupleCR_Success );
			kAnswererPacket.Push( kAnswerer.kCharGuid );
			COUPLE_SKILL_LIST kStack;
			kStack.push_back(iSkillNo);
			PU::TWriteArray_A(kAnswererPacket, kStack);
			g_kRealmUserMgr.Locked_SendToUserGround(kAnswerer.kMemGuid, kAnswererPacket, true, true);
			pkAnswererCoupleInfo->LearnSkill(iSkillNo);
		}

		PgCoupleMgrUtil::SaveSweetHeartInfo(kAnswerer.kCharGuid, kAnswerer.kCharGuid, CC_Ans_SweetHeart_Complete, 0, kNow);
		SetCoupleQuest(kAnswerer, true, 0, 0);
		pkAnswererCoupleInfo->CoupleLimitTime(kNow);
		SaveCoupleInfo(kAnswerer.kCharGuid, pkAnswererCoupleInfo, (CoupleS_Normal | CoupleS_SweetHeart));
		CoupleStatusSave(kAnswerer, (CoupleS_Normal | CoupleS_SweetHeart));
		//SendGndCoupleStatus(kAnswerer);

		// Log
		PgLogCont kLogCont(ELogMain_Contents_Cupple, ELogSub_Marry_Apply);				
		kLogCont.MemberKey(kAnswerer.kMemGuid);
		kLogCont.UID(kAnswerer.iUID);
		kLogCont.CharacterKey(kAnswerer.kCharGuid);
		kLogCont.ID(kAnswerer.kAccountID);
		kLogCont.Name(kAnswerer.Name());
		kLogCont.ChannelNo(static_cast<short>(kAnswerer.sChannel));
		kLogCont.Class(static_cast<short>(kAnswerer.iClass));
		kLogCont.Level(static_cast<short>(kAnswerer.sLevel));
		kLogCont.GroundNo(static_cast<int>(kAnswerer.kGndKey.GroundNo()));

		PgLog kLog(ELOrderMain_Cupple, ELOrderSub_Complete);
		kLog.Set(0, static_cast<std::wstring>(kRequester.Name()));
		kLog.Set(0, static_cast<int>(iSkillNo));
		kLog.Set(2, kRequester.kCharGuid.str());

		kLogCont.Add(kLog);
		kLogCont.Commit();

		// 결혼 업적
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(kRequesterGuid);
		pkActionOrder->InsertTarget(kAnswererGuid);
		pkActionOrder->kCause = CAE_Achievement;
		pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kRequesterGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_MARRY,1)));
		pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,kAnswererGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_MARRY,1)));
		g_kJobDispatcher.VPush(pkActionOrder);
	}
	else
	{
		RejectSweetHeartComplete(kAnswerer, *pkAnswererCoupleInfo);			// 거절 당했으
	}

	/*// Log
	PgLogCont kLogCont(ELogMain_Contents_Cupple, ELogSub_Cupple_Apply);				
	kLogCont.MemberKey(kRequester.kMemGuid);
	kLogCont.UID(kRequester.iUID);
	kLogCont.CharacterKey(kRequester.kCharGuid);
	kLogCont.ID(kRequester.kAccountID);
	kLogCont.Name(kRequester.Name());
	kLogCont.ChannelNo(static_cast<short>(kRequester.sChannel));
	kLogCont.Class(static_cast<short>(kRequester.iClass));
	kLogCont.Level(static_cast<short>(kRequester.sLevel));
	kLogCont.GroundNo(static_cast<int>(kRequester.kGndKey.GroundNo()));

	PgLog kLog(ELOrderMain_Cupple, ELOrderSub_Apply);
	kLog.Set(0, static_cast<std::wstring>(kAnswerer.Name()));
	kLog.Set(0, static_cast<int>(iCoupleSkillNo));
	kLog.Set(2, kAnswerer.kCharGuid.str());

	kLogCont.Add(kLog);
	kLogCont.Commit();*/

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::SendGndCoupleStatusInfo(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &kSendGuid = rkCharGuid;

	SContentsUser kSendUser;
	if( S_OK != ::GetPlayerByGuid(kSendGuid, false, kSendUser) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}

	SendGndCoupleStatus(kSendUser);

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::CoupleSkillInfo(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &kSendGuid = rkCharGuid;

	SContentsUser kSendUser;
	if( S_OK != ::GetPlayerByGuid(kSendGuid, false, kSendUser) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}

	int iSkillNo = 0;

	rkPacket.Pop( iSkillNo );

	mapped_type *pkRequesterCoupleInfo = Get(kSendUser.kCharGuid);
	if( !pkRequesterCoupleInfo )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}

	pkRequesterCoupleInfo->LearnSkill(iSkillNo);

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::ReqSweetHeartQuest(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &kRequesterGuid = rkCharGuid;
	SContentsUser kRequester;
	SContentsUser kAnswerer;

	int iQuestID = 0;
	BM::GUID kAnswererGuid;
	rkPacket.Pop( kAnswererGuid );
	rkPacket.Pop( iQuestID );

	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kRequester) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}

	if( S_OK != ::GetPlayerByGuid(kAnswererGuid, false, kAnswerer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	if( kRequester.kCharGuid == kAnswerer.kCharGuid )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotGender"));
		return CoupleCR_NotGender; 
	} // 자신은 안되
	if( kRequester.cGender == kAnswerer.cGender )		
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotGender"));
		return CoupleCR_NotGender; 
	} // 동성은 안되

	mapped_type *pkRequesterCoupleInfo = Get(kRequester.kCharGuid);
	mapped_type *pkAnswererCoupleInfo = Get(kAnswerer.kCharGuid);

	if( !pkRequesterCoupleInfo )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( !pkAnswererCoupleInfo )		
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}

	BM::Stream kRequesterPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_SweetHeart_Quest);
	kRequesterPacket.Push( (BYTE)CoupleCR_Success );
	kRequesterPacket.Push( kAnswerer.kName );				// 누구에게
	g_kRealmUserMgr.Locked_SendToUser(kRequester.kMemGuid, kRequesterPacket);		// 연인 조건 신청 성공

	BM::Stream kAnsererPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_SweetHeart_Quest);
	kAnsererPacket.Push( (BYTE)CoupleCR_Notify );
	kAnsererPacket.Push( kRequester.kCharGuid );
	kAnsererPacket.Push( kRequester.kName );				// 누구로 부터
	kAnsererPacket.Push( iQuestID );
	g_kRealmUserMgr.Locked_SendToUser(kAnswerer.kMemGuid, kAnsererPacket);			// 연인 조건 신청했다 어떻게 할래?

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::AnsSweetHeartQuest(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	int iQuestID = 0;
	bool bSayYes = false;
	BM::GUID const &kAnswererGuid = rkCharGuid;
	BM::GUID kRequesterGuid;
	SContentsUser kAnswerer;
	SContentsUser kRequester;

	rkPacket.Pop( kRequesterGuid );
	rkPacket.Pop( iQuestID );
	rkPacket.Pop( bSayYes );	

	if( S_OK != ::GetPlayerByGuid(kAnswererGuid, false, kAnswerer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}
	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kRequester) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	if( kRequester.cGender == kAnswerer.cGender )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotGender"));
		return CoupleCR_NotGender; 
	} // 동성은 안되
	mapped_type *pkAnswererCoupleInfo = Get(kAnswerer.kCharGuid);
	mapped_type *pkRequesterCoupleInfo = Get(kRequester.kCharGuid);
	if( !pkAnswererCoupleInfo )		
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( !pkRequesterCoupleInfo )	
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}

	if( bSayYes )
	{
		BM::Stream kRequesterPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Ans_SweetHeart_Quest);
		kRequesterPacket.Push( (BYTE)CoupleCR_Success );
		kRequesterPacket.Push( kRequester.kCharGuid );
		kRequesterPacket.Push( kAnswerer.kCharGuid );
		kRequesterPacket.Push( iQuestID );
		kRequesterPacket.Push( kRequester.kName );
		kRequesterPacket.Push( kAnswerer.kName );		
		g_kRealmUserMgr.Locked_SendToUserGround(kRequester.kMemGuid, kRequesterPacket, true, true);
		//SetCoupleQuest(kRequester, false);

		BM::Stream kAnswererPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Ans_SweetHeart_Quest);
		kAnswererPacket.Push( (BYTE)CoupleCR_Success );
		kAnswererPacket.Push( kAnswerer.kCharGuid );
		kAnswererPacket.Push( kRequester.kCharGuid );
		kAnswererPacket.Push( iQuestID );
		kAnswererPacket.Push( kAnswerer.kName );
		kAnswererPacket.Push( kRequester.kName );		
		g_kRealmUserMgr.Locked_SendToUserGround(kAnswerer.kMemGuid, kAnswererPacket, true, true);
		//SetCoupleQuest(kAnswerer, false);
	}
	else
	{
		RejectSweetHeartQuest(kAnswerer, *pkAnswererCoupleInfo);			// 거절 당했으
	}

	/*// Log
	PgLogCont kLogCont(ELogMain_Contents_Cupple, ELogSub_Cupple_Apply);				
	kLogCont.MemberKey(kRequester.kMemGuid);
	kLogCont.UID(kRequester.iUID);
	kLogCont.CharacterKey(kRequester.kCharGuid);
	kLogCont.ID(kRequester.kAccountID);
	kLogCont.Name(kRequester.Name());
	kLogCont.ChannelNo(static_cast<short>(kRequester.sChannel));
	kLogCont.Class(static_cast<short>(kRequester.iClass));
	kLogCont.Level(static_cast<short>(kRequester.sLevel));
	kLogCont.GroundNo(static_cast<int>(kRequester.kGndKey.GroundNo()));

	PgLog kLog(ELOrderMain_Cupple, ELOrderSub_Apply);
	kLog.Set(0, static_cast<std::wstring>(kAnswerer.Name()));
	kLog.Set(0, static_cast<int>(iCoupleSkillNo));
	kLog.Set(2, kAnswerer.kCharGuid.str());

	kLogCont.Add(kLog);
	kLogCont.Commit();*/

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::SweetHeartQuest_Cancel(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &kRequesterGuid = rkCharGuid;
	BM::GUID kCoupleGuid;
	int iQuestID = 0;

	SContentsUser kRequester;
	SContentsUser kAnswerer;

	rkPacket.Pop( kCoupleGuid );
	rkPacket.Pop( iQuestID );

	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kRequester) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	SetCoupleQuest(kRequester, true, 0, 0);
	SaveDBSweetHeartInfo(kRequester.kCharGuid, NULL);

	if( S_OK != ::GetPlayerByGuid(kCoupleGuid, false, kAnswerer) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));

		// 커플이 접속을 하지 않았네? DB에 연인 퀘스트를 저장해 두자. 로그인하면 지워버리게...
		BM::PgPackedTime kNow;
		PgCoupleMgrUtil::SaveSweetHeartInfo( kRequesterGuid, kCoupleGuid, CC_SweetHeartQuest_Cancel, iQuestID, kNow );
		return CoupleCR_NotFind;
	}
	
	SetCoupleQuest(kAnswerer, true, 0, 0);
	SaveDBSweetHeartInfo(kAnswerer.kCharGuid, NULL);

	mapped_type *pkAnswererCoupleInfo = Get(kAnswerer.kCharGuid);
	if(NULL==pkAnswererCoupleInfo)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"pkAnswererCoupleInfo is NULL, Requester[" << kRequester.kCharGuid << L"] Answerer[" << kAnswerer.kCharGuid << L"]" );
	}

	if( BM::GUID::IsNotNull(kAnswerer.kCoupleGuid) )
	{
		BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND);
		kPacket.Push( (BYTE)CC_SweetHeartQuest_Cancel );
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( kAnswerer.kCharGuid );
		kPacket.Push( iQuestID );
		g_kRealmUserMgr.Locked_SendToUserGround(kAnswerer.kMemGuid, kPacket, true, true);
	}

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::SweetHeartQuestTimeInfo(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &kRequesterGuid = rkCharGuid;
	bool bInit = false;
	int iQuestID = 0;
	__int64 iPlayTime = 0;

	rkPacket.Pop( bInit );

	if( false == bInit )
	{
		rkPacket.Pop( iQuestID );
		rkPacket.Pop( iPlayTime );
	}

	SContentsUser kUser;

	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kUser) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	SetCoupleQuest(kUser, bInit, iQuestID, iPlayTime);

	if( false == bInit )
	{
		// 커플들이 같은 곳에 있는지 체크해서 시간을 갱신 해준다.
		bool const bRet = CheckCoupleGround(kRequesterGuid);
		SendSweetHeartQuestTime(kUser, (bRet) ? CoupleCR_Success : CoupleCR_Failed);
	}
	else
	{
		SendSweetHeartQuestTime(kUser, CoupleCR_None);
		SendSweetHeartQuestTime(kUser.kCoupleGuid, CoupleCR_None);
	}
	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::SweetHeartQuestComplete(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &kRequesterGuid = rkCharGuid;

	int iQuestTime = 0;
	EQuestType eType = QT_None;

	rkPacket.Pop( iQuestTime );
	rkPacket.Pop( eType );

	SContentsUser kUser;

	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, kUser) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	mapped_type *pkCoupleInfo = Get(kUser.kCharGuid);
	if( !pkCoupleInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! CoupleInfo is NULL"));
		return CoupleCR_NotFind;
	}	

	pkCoupleInfo->SweetHeartQuestTime( iQuestTime );
	pkCoupleInfo->SweetHeartQuestTimeGap( 0 );
	pkCoupleInfo->SweetHeartQuestUpdate( false );
	SaveDBSweetHeartInfo(kUser.kCharGuid, pkCoupleInfo);
	SendSweetHeartQuestTime(kUser, (QT_Couple == eType) ? CoupleCR_QuestComplete : CoupleCR_None);

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::ReqInfo(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::GUID const &rkInfoRequesterGuid = rkCharGuid;
	SContentsUser kInfoRequeter;
	if( S_OK != ::GetPlayerByGuid(rkInfoRequesterGuid, false, kInfoRequeter) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}

	mapped_type *pkCoupleInfo = Get(kInfoRequeter.kCharGuid);
	if( !pkCoupleInfo )		
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_None"));
		return CoupleCR_None;
	}

	SendCoupleInfo(kInfoRequeter);
	
	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::ReqCoupleInit(BM::GUID const &rkCharGuid)
{
	BM::GUID const &kRequesterGuid = rkCharGuid;

	SContentsUser rkUser;

	if( S_OK != ::GetPlayerByGuid(kRequesterGuid, false, rkUser) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	BM::PgPackedTime const kNow;

	mapped_type *pkCoupleInfo = Get(kRequesterGuid);
	if( pkCoupleInfo )		
	{		
		pkCoupleInfo->SweetHeartPanalty(kNow);
		SendCoupleInfo(rkUser);
	}
	else
	{
		BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_CoupleInit);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( kNow );
		g_kRealmUserMgr.Locked_SendToUser(rkUser.kMemGuid, kPacket);
	}

	BM::Stream kPacketUser(PT_N_T_REQ_COUPLE_COMMAND, (BYTE)CC_CoupleInit);
	kPacketUser.Push( (BYTE)CoupleCR_Success );
	kPacketUser.Push( rkUser.kCharGuid );
	SendToPacketHandler(kPacketUser);

	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::ReqFind(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	SContentsUser kUser;
	if( S_OK != ::GetPlayerByGuid(rkCharGuid, false, kUser) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}

	size_t const iMaxCount = 30;
	int const iLevelDiff = 5;
	ContContentsUser kVec;
	g_kRealmUserMgr.Locked_For_Each_Player( PgCoupleMatcher(iMaxCount, iLevelDiff, kUser, kVec, false) );

	//
	if( kVec.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;	// 
	}

	BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Find);
	kPacket.Push((BYTE)CoupleCR_Success);
	PU::TWriteArray_M(kPacket, kVec);
	g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
	return CoupleCR_Success;
}

ECoupleCommandResult PgCoupleMgr::ReqWarp(BM::GUID const &rkCharGuid, BM::Stream &rkPacket, BYTE const cCmdType)
{
	SContentsUser kRequster, kHim;
	BM::GUID const &rkRequesterGuid = rkCharGuid;

	POINT3			pt3TargetPos;//내가 갈 자리
	SGroundKey		kTargetKey;

	rkPacket.Pop( pt3TargetPos );
	rkPacket.Pop( kTargetKey );

	if( S_OK != ::GetPlayerByGuid(rkRequesterGuid, false, kRequster) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed;
	}

	// 내 정보
	SCoupleInfo *pkRequesterCoupleInfo = Get(kRequster.kCharGuid);
	if( !pkRequesterCoupleInfo )									
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	}
	if( BM::GUID::IsNull(pkRequesterCoupleInfo->CoupleGuid()) )		
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	} // 커플 맺고서 해

	// 상대편 정보
	if( S_OK != ::GetPlayerByGuid(pkRequesterCoupleInfo->CoupleGuid(), false, kHim) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind;
	}

	if( kRequster.sChannel != kHim.sChannel )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotChannel"));
		return CoupleCR_NotChannel;
	}

	SCoupleInfo *pkHimCoupleInfo = Get(pkRequesterCoupleInfo->CoupleGuid());
	if( !pkHimCoupleInfo )											
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_NotFind"));
		return CoupleCR_NotFind; 
	}
	if( pkHimCoupleInfo->CoupleGuid() != rkCharGuid )				
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Failed"));
		return CoupleCR_Failed; 
	} // 뭥미
	if( !kHim.kGndKey.IsField() )									
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Reject"));
		return CoupleCR_Reject; 
	}
	if( kRequster.kGndKey.GroundNo() == kHim.kGndKey.GroundNo() )	
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CoupleCR_Same"));
		return CoupleCR_Same; 
	}


	switch( cCmdType )
	{
	case CC_Req_Warp:
		{
			BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, cCmdType);
			kPacket.Push( (BYTE)CoupleCR_Success );
			kPacket.Push( kHim.kCharGuid );
			kPacket.Push( kRequster.kCharGuid );
			g_kRealmUserMgr.Locked_SendToUserGround(kHim.kMemGuid, kPacket, true, true);
		}break;
	case CC_Req_Warp1:
		{
			BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, cCmdType);
			kPacket.Push( (BYTE)CoupleCR_Success );
			kPacket.Push( kHim.kCharGuid );
			kPacket.Push( pt3TargetPos );
			kPacket.Push( kTargetKey );
			g_kRealmUserMgr.Locked_SendToUserGround(kHim.kMemGuid, kPacket, true, true);
		}break;
	case CC_Req_Warp2:
		{
			BM::Stream kPacket(PT_N_M_ANS_COUPLE_COMMAND, cCmdType);
			kPacket.Push( (BYTE)CoupleCR_Success );
			kPacket.Push( kHim.kCharGuid );					// 소환 해줘
			kPacket.Push( kRequster.kMemGuid );				// 나를
			kPacket.Push( kHim.kMemGuid );
			kPacket.Push( kRequster.kCharGuid );
			kPacket.Push( pt3TargetPos );
			kPacket.Push( kTargetKey );
			g_kRealmUserMgr.Locked_SendToUserGround(kHim.kMemGuid, kPacket, true, true);

			BM::Stream kAnsPacket(PT_N_C_ANS_COUPLE_COMMAND);
			kAnsPacket.Push( (BYTE)CC_Req_Warp );
			kAnsPacket.Push( (BYTE)CoupleCR_Success );
			g_kRealmUserMgr.Locked_SendToUser(kRequster.kMemGuid, kAnsPacket);
		}break;
	case CC_Ans_Reject:
		{
			BM::Stream kAnsPacket(PT_N_C_ANS_COUPLE_COMMAND);
			kAnsPacket.Push( (BYTE)CC_Req_Warp );
			kAnsPacket.Push( (BYTE)CoupleCR_Reject );
			g_kRealmUserMgr.Locked_SendToUser(kRequster.kMemGuid, kAnsPacket);
		}break;
	case CC_Ans_Notify:
		{
			BM::Stream kAnsPacket(PT_N_C_ANS_COUPLE_COMMAND);
			kAnsPacket.Push( (BYTE)CC_Req_Warp );
			kAnsPacket.Push( (BYTE)CoupleCR_Notify );
			g_kRealmUserMgr.Locked_SendToUser(kHim.kMemGuid, kAnsPacket);
		}break;
	}
	return CoupleCR_Success;
}

void PgCoupleMgr::SweetHeartPanalty(BM::GUID const &rkCharGuid, BM::GUID const &rkGuid)
{
	BM::PgPackedTime kNow;
	if( BM::GUID::IsNull(rkGuid) )
	{
		mapped_type *pkCoupleInfo = Get(rkCharGuid);
		if( pkCoupleInfo )
		{
			if( pkCoupleInfo->cStatus() == (CoupleS_Normal | CoupleS_SweetHeart) )
			{
				// 연인이면 패널티 적용
				__int64 const iLimitTime = (iPanaltyDay);
				kNow = BM::PgPackedTime::LocalTime();
				CGameTime::AddTime(kNow, iLimitTime);
			}
			pkCoupleInfo->SweetHeartPanalty(kNow);
		}
	}

	PgCoupleMgrUtil::SetCoupleGuidByGuid(rkCharGuid, rkGuid, kNow);
}

void PgCoupleMgr::Tick()
{
	BM::CAutoMutex kLock(m_kMutex);
	
	const_iterator couple_iter = m_kMap.begin();
	while(m_kMap.end() != couple_iter)
	{
		key_type const &rkCharGuid = (*couple_iter).first;
		if( CheckLimitTime(rkCharGuid) )
		{
			// 1일 커플만 시간 제한이 있다. 연인은 없다.
			SweetHeartPanalty(rkCharGuid, BM::GUID::NullData());
		}
		if( CheckLimitTimeQuest(rkCharGuid) )
		{
			// 커플 퀘스트 완료 했어
		}
		++couple_iter;
	}
}

bool PgCoupleMgr::ProcessCommand(BM::GUID const &rkCharGuid, BYTE const cCommand, BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

#ifdef DEACTIVATE_COUPLE_SYSTEM
	return true;
#else

	ECoupleCommandResult eRet = CoupleCR_Failed;
	switch( cCommand )
	{
	case CC_Req_Find:		{ eRet = ReqFind(rkCharGuid, rkPacket); }break;
	case CC_Req_Couple:		{ eRet = ReqCouple(rkCharGuid, rkPacket); }break;
	case CC_Ans_Couple:		{ eRet = AnsCouple(rkCharGuid, rkPacket); }break;
	case CC_Req_Break:		{ eRet = BreakCouple(rkCharGuid, rkPacket); }break;
	case CC_Req_Info:		{ eRet = ReqInfo(rkCharGuid, rkPacket); }break;
	case CC_Req_Warp:
	case CC_Req_Warp1:
	case CC_Req_Warp2:
	case CC_Ans_Reject:
	case CC_Ans_Notify:
		{
			eRet = ReqWarp(rkCharGuid, rkPacket, cCommand);
		}break;
	case CC_Req_SweetHeart_Quest:		{ eRet = ReqSweetHeartQuest(rkCharGuid, rkPacket); }break;
	case CC_Ans_SweetHeart_Quest:		{ eRet = AnsSweetHeartQuest(rkCharGuid, rkPacket); }break;
	case CC_SweetHeartQuest_Cancel:		{ eRet = SweetHeartQuest_Cancel(rkCharGuid, rkPacket); }break;
	case CC_SweetHeartQuestTimeInfo:	{ eRet = SweetHeartQuestTimeInfo(rkCharGuid, rkPacket); }break;
	case CC_SweetHeartQuest_Complete:	{ eRet = SweetHeartQuestComplete(rkCharGuid, rkPacket); }break;
	case CC_Req_SweetHeart_Complete:	{ eRet = ReqSweetHeartComplete(rkCharGuid, rkPacket); }break;
	case CC_Ans_SweetHeart_Complete:	{ eRet = AnsSweetHeartComplete(rkCharGuid, rkPacket); }break;
	case CC_SendGndCoupleStatus:		{ eRet = SendGndCoupleStatusInfo(rkCharGuid, rkPacket); }break;
	case CC_CoupleSkill:				{ eRet = CoupleSkillInfo(rkCharGuid, rkPacket); }break;
	case CC_CoupleInit:					{ eRet = ReqCoupleInit(rkCharGuid); }break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}

	// 실패 처리
	if( CoupleCR_Success != eRet )
	{
		BM::Stream kFailPacket(PT_N_C_ANS_COUPLE_COMMAND, cCommand);
		kFailPacket.Push( (BYTE)eRet );
		g_kRealmUserMgr.Locked_SendToUser(rkCharGuid, kFailPacket, false);
	}
	else
	{
		BM::Stream kPacket(PT_T_N_REQ_FRIEND_LINK_INFO);
		kPacket.Push(rkCharGuid);
		SendToFriendMgr(kPacket);
	}
#endif
	return true;
}

bool PgCoupleMgr::ProcessTCommand(BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BYTE cCmdType = 0;
	BYTE cCmdResult = 0;
	BM::GUID kGuid;

	rkPacket.Pop( cCmdType );
	rkPacket.Pop( cCmdResult );
	rkPacket.Pop( kGuid );

	switch( cCmdType )
	{
	case CC_Sys_Save:
		{
			//SContentsUser kUser;
			//if( S_OK == GetPlayerByGuid(kGuid, false, kUser) )
			//{
			//	mapped_type *pkCoupleInfo = Get(kUser.kCharGuid);
			//	if( !pkCoupleInfo )
			//	{
			//		break;
			//	}
			//	pkCoupleInfo->ReadFromPacket( rkPacket );
			//}
		}break;
	case CC_Req_Info:
		{
			SContentsUser kUser;
			if( S_OK == ::GetPlayerByGuid(kGuid, true, kUser) )
			{
				mapped_type *pkCoupleInfo = Get(kUser.kCharGuid);
				if( !pkCoupleInfo )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find CoupleInfo"));
					break;
				}

				pkCoupleInfo->ReadFromPacket( rkPacket );		// DB로 부터 온 정보 읽어

				if( CheckLimitTime(kUser.kCharGuid) )			// 커플 기간 지나는지 확인
				{					
					SetCoupleQuest(kUser, true, 0, 0);
					SaveDBSweetHeartInfo(kUser.kCharGuid, NULL);
					PgCoupleMgrUtil::SetCoupleGuid(kUser, BM::GUID::NullData(), pkCoupleInfo->CouplePanaltyLimitDate());
				}
				NotifyBreaked(kUser);							// 해어졌는지 알아봐
				AddCoupleSkill(kUser);							// 커플 스킬 추가	// 이모션 배운다.
				NotifyInfo(kUser);

				if( BM::GUID::IsNotNull(pkCoupleInfo->CoupleGuid()) )
				{
					//if( pkCoupleInfo->cStatus() == (CoupleS_Normal | CoupleS_SweetHeart) )
					{
						// 연인 스킬
						AddSweetHeartSkill(kUser);
					}
				}

				int iQuestValue = 0;					
				if( true == SweetHeartQuestDelete(kUser, pkCoupleInfo, iQuestValue) )
				{
					PgCoupleMgrUtil::SetSweetHeartInfo(kUser, kUser.kCharGuid, CC_SweetHeartQuest_Cancel, iQuestValue, pkCoupleInfo->CouplePanaltyLimitDate());	// 연인 퀘스트 초기화
				}

				SendCoupleInfo(kUser);							// 정보 다시 보내줘

				if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
				{
					// 엮어볼 목록 쏴줘
					size_t const iMaxCount = 10;
					int const iLevelDiff = 5;
					ContContentsUser kVec;
					g_kRealmUserMgr.Locked_For_Each_Player( PgCoupleMatcher(iMaxCount, iLevelDiff, kUser, kVec) );

					if( !kVec.empty() )
					{
						BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Auto_Match);
						kPacket.Push((BYTE)CoupleCR_Success);
						PU::TWriteArray_M(kPacket, kVec);
						g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
					}
				}
			}
		}break;
	case CC_Req_Break:
		{
			// 상대편 캐릭터가 삭제 되었을 때 여기로
			BM::GUID kBreakerGuid;
			std::wstring kBreakerName;

			rkPacket.Pop( kBreakerGuid );
			rkPacket.Pop( kBreakerName );

			mapped_type *pkCoupleInfo = Get(kGuid);

			SContentsUser kUser;
			if( pkCoupleInfo
			&&	S_OK == GetPlayerByGuid(kGuid, false, kUser) )
			{
				// 같은 채널
				pkCoupleInfo->Clear();

				BM::Stream kNfyPacket(PT_N_M_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Break);
				kNfyPacket.Push( (BYTE)CoupleCR_Notify );
				kNfyPacket.Push( kUser.kCharGuid );							// 내 캐릭터
				kNfyPacket.Push( kBreakerName );
				g_kRealmUserMgr.Locked_SendToUserGround(kUser.kMemGuid, kNfyPacket, true, true);	// 해어짐 당했어

				SaveCoupleInfo(kUser.kCharGuid, pkCoupleInfo, CoupleS_Normal);
			}
			//else if( S_OK == GetPlayerByGuid(kGuid, false, kUser, true) )
			//{
			//	// 다른 채널
			//}
			else
			{
				// 로그 오프 중
				mapped_type kCleanInfo(kBreakerGuid);
				SaveCoupleInfo(kGuid, &kCleanInfo, CoupleS_Breaked);
			}
		}break;
	case CC_Status:
		{
			BM::GUID kCoupleGuid;
			BYTE cMyLinkStatus = 0;			
			BYTE cYouLinkStatus = 0;

			rkPacket.Pop( kCoupleGuid );
			rkPacket.Pop( cMyLinkStatus );
			rkPacket.Pop( cYouLinkStatus );

			mapped_type *pkCoupleInfo;
			SContentsUser kMyUser;
			SContentsUser kYouUser;
			if( S_OK == ::GetPlayerByGuid(kGuid, false, kMyUser) )
			{
				pkCoupleInfo = Get(kMyUser.kCharGuid);
				if( !pkCoupleInfo )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find CoupleInfo"));
					break;
				}
				if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CoupleGuid is NULL"));
					break;
				}
			}

			if( BM::GUID::IsNull(kCoupleGuid) )
			{
				if( BM::GUID::IsNull(kMyUser.kCoupleGuid) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CoupleGuid is NULL"));
					break;					
				}
				kCoupleGuid = kMyUser.kCoupleGuid;
			}

			if( S_OK == ::GetPlayerByGuid(kCoupleGuid, false, kYouUser) )
			{
				pkCoupleInfo = Get(kYouUser.kCharGuid);
				if( !pkCoupleInfo )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find CoupleInfo"));
					break;
				}
				if( BM::GUID::IsNull(pkCoupleInfo->CoupleGuid()) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CoupleGuid is NULL"));
					break;
				}

				if( cYouLinkStatus )
				{
					BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Status);
					kPacket.Push((BYTE)CoupleCR_Success);
					kPacket.Push((BYTE)cYouLinkStatus);
					kPacket.Push(kYouUser.kGndKey.GroundNo());
					g_kRealmUserMgr.Locked_SendToUser(kMyUser.kMemGuid, kPacket);
				}

				BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Status);
				kPacket.Push((BYTE)CoupleCR_Success);
				kPacket.Push((BYTE)cMyLinkStatus);
				kPacket.Push(kMyUser.kGndKey.GroundNo());
				g_kRealmUserMgr.Locked_SendToUser(kYouUser.kMemGuid, kPacket);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Wrong couple command type[") << cCmdType << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}

	return true;
}

bool PgCoupleMgr::ProcessMsg(SEventMessage *pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex);

#ifdef DEACTIVATE_COUPLE_SYSTEM
	return true;
#else
	if( !pkMsg )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("EventMessage is NULL"));
		return false;
	}

	if( 0 == iCoupleUse )
	{
		return false;
	}

	PACKET_ID_TYPE wType =0;
	pkMsg->Pop(wType);
	switch( wType )
	{
	case PT_T_N_NFY_USER_ENTER_GROUND:
		{
			// 맵이동
			SAnsMapMove_MT kAMM;
			SContentsUser kUser;

			pkMsg->Pop(kAMM);
			kUser.ReadFromPacket(*pkMsg);

			if( !Check(kUser.kCharGuid) )
			{
				if( Add(kUser.kCharGuid, kUser.kCoupleGuid) )	// 커플 정보 추가
				{
					ReqDBCoupleInfo(kUser);						// DB에 정보 요청
				}
			}
			else
			{
				mapped_type *pkAnswererCoupleInfo = Get(kUser.kCharGuid);
				if( pkAnswererCoupleInfo
				&&	BM::GUID::IsNotNull(pkAnswererCoupleInfo->CoupleGuid())
				&&	!pkAnswererCoupleInfo->Accepted() )
				{
					RejectCouple(kUser, *pkAnswererCoupleInfo);
				}
			}
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			SContentsUser kUser;
			kUser.ReadFromPacket(*(BM::Stream*)pkMsg);

			if( !kUser.Empty() )
			{
				mapped_type *pkAnswererCoupleInfo = Get(kUser.kCharGuid);
				if( pkAnswererCoupleInfo
				&&	BM::GUID::IsNotNull(pkAnswererCoupleInfo->CoupleGuid())
				&&	!pkAnswererCoupleInfo->Accepted() )
				{
					RejectCouple(kUser, *pkAnswererCoupleInfo);
				}

				if( pkAnswererCoupleInfo 
					&& BM::GUID::IsNotNull(pkAnswererCoupleInfo->CoupleGuid())
					&& (0 < pkAnswererCoupleInfo->SweetHeartQuestTime()) )
				{
					// 협동 플레이 시간 갱신
					mapped_type *pkCoupleInfo = Get(pkAnswererCoupleInfo->CoupleGuid());
					if( pkCoupleInfo && BM::GUID::IsNotNull(pkCoupleInfo->CoupleGuid()) )
					{
						pkCoupleInfo->SweetHeartQuestTimeGap( 0 );
						pkCoupleInfo->SweetHeartQuestUpdate( false );
						SendSweetHeartQuestTime(pkAnswererCoupleInfo->CoupleGuid(),	CoupleCR_Failed);
					}
				}

				SaveCoupleInfo(kUser.kCharGuid, pkAnswererCoupleInfo);

				SaveDBSweetHeartInfo(kUser.kCharGuid, pkAnswererCoupleInfo);

				Del(kUser.kCharGuid);
			}
			// 유저 아웃
		}break;
	case PT_C_N_REQ_COUPLE_COMMAND:
		{
			BYTE cCmdType = 0;
			BM::GUID kCharGuid;

			pkMsg->Pop( kCharGuid );
			pkMsg->Pop( cCmdType );

			ProcessCommand(kCharGuid, cCmdType, *pkMsg);
		}break;
	case PT_T_N_ANS_COUPLE_COMMAND:
		{
			ProcessTCommand(*pkMsg);
		}break;
	case PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_COUPLE:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			SHOMEADDR kHomeAddr;
			kHomeAddr.ReadFromPacket(*pkMsg);

			BM::GUID kOwnerGuid;
			mapped_type *pkMyInfo = GetMyInfo(kCharGuid, kOwnerGuid);
			if(pkMyInfo )
			{
				pkMyInfo->HomeAddr(kHomeAddr);
				SendCoupleInfo(kCharGuid);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Wrong Task Msg[") << pkMsg->SecType() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}
	}
#endif
	return true;
}

bool PgCoupleMgr::InitConstantValue(void)
{
	int iValue = 1;	

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Contents, EVar_CoupleUse, iValue) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find 'EVar_CoupleUse'") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	iCoupleUse = iValue;

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Contents, EVar_CoupleSweetHeartPanaltyDay, iValue) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find 'EVar_CoupleSweetHeartPanaltyDay'") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	iPanaltyDay = static_cast<__int64>(CGameTime::OneDay * iValue);

	return true;
}