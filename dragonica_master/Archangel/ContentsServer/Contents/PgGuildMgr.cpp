#include "stdafx.h"
#include "Collins/Log.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/PgLogUtil.h"
#include "Variant/PgEmporiaFunction.h"
#include "Global.h"
#include "PgGuildMgr.h"
#include "PgServerSetMgr.h"
#include "Item/PgPostManager.h"
#include "jobdispatcher.h"

extern SGuild_Inventory_Log GetGuildInvLog(BM::GUID const & kGuildID, EGuildInvTradeType const eType, __int64 const i64Money);

//
tagGuildOwnerLoginDay::tagGuildOwnerLoginDay()
	: kGuildGuid(), sGuildLevel(0), iLastLoginDay(0)
{
}
tagGuildOwnerLoginDay::tagGuildOwnerLoginDay(tagGuildOwnerLoginDay const& rhs)
	: kGuildGuid(rhs.kGuildGuid), sGuildLevel(rhs.sGuildLevel), iLastLoginDay(rhs.iLastLoginDay)
{
}
void tagGuildOwnerLoginDay::ReadFromDBResult(CEL::DB_DATA_ARRAY::const_iterator& result_iter)
{
	result_iter->Pop( kGuildGuid );		++result_iter;
	result_iter->Pop( sGuildLevel );	++result_iter;
	result_iter->Pop( iLastLoginDay );	++result_iter;
}
void tagGuildOwnerLoginDay::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( kGuildGuid );
	rkPacket.Push( sGuildLevel );
	rkPacket.Push( iLastLoginDay );
}
void tagGuildOwnerLoginDay::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop( kGuildGuid );
	rkPacket.Pop( sGuildLevel );
	rkPacket.Pop( iLastLoginDay );
}

//
tagGuildOwnerLoginDayResult::tagGuildOwnerLoginDayResult()
	: SGuildOwnerLoginDay(), eResult(GACOR_NONE)
{
}
tagGuildOwnerLoginDayResult::tagGuildOwnerLoginDayResult(tagGuildOwnerLoginDayResult const& rhs)
	: SGuildOwnerLoginDay(rhs), eResult(rhs.eResult)
{
}
tagGuildOwnerLoginDayResult::tagGuildOwnerLoginDayResult(BM::Stream& rkPacket)
	: eResult(GACOR_NONE)
{
	SGuildOwnerLoginDay::ReadFromPacket(rkPacket);
}

//
namespace PgGuildMgrUtil
{
	PgAutoChangeOwnerMgr::PgAutoChangeOwnerMgr()
		: m_kLastTickDate(BM::PgPackedTime::LocalTime()), m_kContLastLoginDay()
	{
	}
	PgAutoChangeOwnerMgr::~PgAutoChangeOwnerMgr()
	{
	}
	bool PgAutoChangeOwnerMgr::Tick()
	{
		BM::PgPackedTime const kCurDate = BM::PgPackedTime::LocalTime();
		if( kCurDate.Day() != m_kLastTickDate.Day() )
		{
			m_kLastTickDate = kCurDate;
			return true;
		}
		return false;
	}
	void PgAutoChangeOwnerMgr::GetOwnerLastLoginDay(BM::GUID const& rkMgrGuid, bool const bInit)
	{
		CEL::DB_QUERY kQuery(DT_PLAYER, ((bInit)? DQT_GUILD_INIT_OWNER_LAST_LOGIN_DAY: DQT_GUILD_SELECT_OWNER_LAST_LOGIN_DAY), _T("EXEC [dbo].[UP_Guild_SELECT_OwnerLastLoginDay]"));
		kQuery.InsertQueryTarget(rkMgrGuid);
		kQuery.QueryOwner(rkMgrGuid);
		g_kCoreCenter.PushQuery(kQuery);
	}
	void PgAutoChangeOwnerMgr::CheckDay(CONT_GUILD_LAST_LOGIN_DAY_RESULT& rkContLastLoginDayResult)
	{
		CONT_DEF_GUILD_LEVEL const* pkDefGuild = NULL;
		g_kTblDataMgr.GetContDef(pkDefGuild);

		{
			CONT_GUILD_LAST_LOGIN_DAY_RESULT::iterator iter = m_kContLastLoginDay.begin();
			while( m_kContLastLoginDay.end() != iter )
			{
				if( rkContLastLoginDayResult.end() == rkContLastLoginDayResult.find((*iter).first) )
				{
					iter = m_kContLastLoginDay.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}

		CONT_GUILD_LAST_LOGIN_DAY_RESULT::iterator iter = rkContLastLoginDayResult.begin();
		while( rkContLastLoginDayResult.end() != iter )
		{
			CONT_GUILD_LAST_LOGIN_DAY_RESULT::mapped_type& rkLastLoginDay = (*iter).second;

			CONT_DEF_GUILD_LEVEL::const_iterator find_iter = pkDefGuild->find(rkLastLoginDay.sGuildLevel);
			if( pkDefGuild->end() != find_iter ) // 레벨정보에 없는 길드는 ......... (무시 하자)
			{
				CONT_DEF_GUILD_LEVEL::mapped_type const& rkDefGuildLevel = (*find_iter).second;
				if( rkDefGuildLevel.iAutoChangeOwnerRun <= rkLastLoginDay.iLastLoginDay )
				{
					rkLastLoginDay.eResult = GACOR_RUN;
				}
				else if( rkDefGuildLevel.iAutoChangeOwner2 <= rkLastLoginDay.iLastLoginDay )
				{
					rkLastLoginDay.eResult = GACOR_WARNNING_2;
				}
				else if( rkDefGuildLevel.iAutoChangeOwner1 <= rkLastLoginDay.iLastLoginDay )
				{
					rkLastLoginDay.eResult = GACOR_WARNNING_1;
				}
				else
				{
					// 경고 대상도, 삭제 대상도 아니다 제거
					iter = rkContLastLoginDayResult.erase(iter);
					continue;
				}
			}

			CONT_GUILD_LAST_LOGIN_DAY_RESULT::iterator prev_iter = m_kContLastLoginDay.find(rkLastLoginDay.kGuildGuid);
			if( m_kContLastLoginDay.end() != prev_iter )
			{
				if( (*prev_iter).second.eResult == rkLastLoginDay.eResult ) // 이전과 동일하면 출력물에서 제거
				{
					iter = rkContLastLoginDayResult.erase(iter);
					continue;
				}
				else
				{
					(*prev_iter).second = (*iter).second;
				}
			}
			else
			{
				// 없으면 추가
				m_kContLastLoginDay.insert( std::make_pair(rkLastLoginDay.kGuildGuid, rkLastLoginDay) );
			}
			++iter;
		}
	}

	//
	void SendToItemOrder(SContentsUser const &rkUser, CONT_PLAYER_MODIFY_ORDER const &kOrder, BM::Stream const &rkAddonPacket)
	{
		BM::Stream kPacket(PT_M_I_REQ_MODIFY_ITEM);
		kPacket.Push( (int)IMEPT_GUILD );
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

	void SendToItemOrder(BM::GUID const &rkCharGuid, CONT_PLAYER_MODIFY_ORDER const &kOrder, BM::Stream const &rkAddonPacket)
	{
		SContentsUser kUser;
		if( S_OK == GetPlayerByGuid(rkCharGuid, false, kUser) )
		{
			SendToItemOrder(kUser, kOrder, rkAddonPacket);
		}
	}

	//void SetGuildGuid(SContentsUser const &rkUser, BM::GUID const &rkGuid, BM::Stream const &rkAddonPacket = BM::Stream())
	//{
	//	CONT_PLAYER_MODIFY_ORDER kOrder;
	//
	//	SPMOD_SetGuid kSetGuidData(SGT_Guild, rkGuid);
	//	kOrder.push_back( SPMO(IMET_SET_GUID, rkUser.kCharGuid, kSetGuidData) );
	//
	//	::SendToItemOrder(rkUser, kOrder, rkAddonPacket);
	//}

	inline std::pair<bool,int> GetMapAttr(int const iGroundNo)
	{
		std::pair<bool,int> kResult(false,0);

		CONT_DEFMAP const *pkDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkDefMap);

		if( !pkDefMap )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't get map def") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return kResult;
		}

		CONT_DEFMAP::const_iterator find_iter = pkDefMap->find( iGroundNo );
		if( pkDefMap->end() == find_iter )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return kResult;
		}

		CONT_DEFMAP::mapped_type const &rkMapDef = (*find_iter).second;
		
		kResult.first = true;
		kResult.second = rkMapDef.iAttr;
		
		return kResult;
	}

	inline bool IsCanReqInOutState(int const iGroundNo)
	{	
		std::pair<bool,int> kResult = GetMapAttr(iGroundNo);
		if( !kResult.first
		||	GATTR_PVP == kResult.second
		||	GATTR_EMPORIABATTLE == kResult.second )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		return true;
	}

	inline bool IsCanReqInOutState(SContentsUser const &rkUser)
	{
		return IsCanReqInOutState(rkUser.kGndKey.GroundNo());
	}

	void JoinGuildOrder(PgGuild *pkGuild, SContentsUser const &rkUser)
	{
		if( !pkGuild )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Guild is NULL"));
			return;
		}
		CONT_PLAYER_MODIFY_ORDER kOrder;

		SPMOD_SetGuid kSetGuidData(SGT_Guild, pkGuild->Guid());
		kOrder.push_back( SPMO(IMET_SET_GUID, rkUser.kCharGuid, kSetGuidData) );

		ContSkillNo kGuildSkill;
		pkGuild->GetGuildSkill(kGuildSkill);

		ContSkillNo::const_iterator skill_iter = kGuildSkill.begin();
		while( kGuildSkill.end() != skill_iter )
		{
			SPMOD_AddSkill kAddSkillData( (*skill_iter) );
			kOrder.push_back( SPMO(IMET_ADD_SKILL, rkUser.kCharGuid, kAddSkillData) );
			++skill_iter;
		}

		SendToItemOrder(rkUser, kOrder);
	}

	void JoinGuildOrder(PgGuild *pkGuild, BM::GUID const &rkCharGuid)
	{
		SContentsUser kUser;
		if( S_OK == GetPlayerByGuid(rkCharGuid, false, kUser) )
		{
			JoinGuildOrder(pkGuild, kUser);
		}
	}

	void JoinGuildOrder(PgGuild *pkGuild, VEC_GUID const &rkVec)
	{
		VEC_GUID::const_iterator loop_iter = rkVec.begin();
		while( rkVec.end() != loop_iter )
		{
			JoinGuildOrder(pkGuild, (*loop_iter));
			++loop_iter;
		}
	}

	void LeaveGuildOrder(SContentsUser const &rkUser)
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;

		SPMOD_SetGuid kSetGuidData(SGT_Guild, BM::GUID::NullData());
		kOrder.push_back( SPMO(IMET_SET_GUID, rkUser.kCharGuid, kSetGuidData) );

		SPMOD_DateContents kSetDateContents(DCT_GUILD_LEAVE_DATE, BM::DBTIMESTAMP_EX(BM::PgPackedTime::LocalTime()));
		kOrder.push_back( SPMO(IMET_SET_DATE_CONTENTS, rkUser.kCharGuid, kSetDateContents) );

		//SPMOD_AddSkill kAddSkillData( (*skill_iter) );
		//kOrder.push_back( SPMO(IMET_ADD_SKILL, rkUser.kCharGuid, kAddSkillData) );

		SendToItemOrder(rkUser, kOrder);
	}
	void LeaveGuildOrder(BM::GUID const &rkCharGuid)
	{
		SContentsUser kUser;
		if( S_OK == GetPlayerByGuid(rkCharGuid, false, kUser) )
		{
			LeaveGuildOrder(kUser);
		}
	}

	//
	void ChangeOwnerQuery(EGuildCommand const eCmdType, BM::GUID const& rkGuildGuid, BM::GUID const& rkMasterCharGuid, BM::GUID const& rkNewMasterCharGuid)
	{
		BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, rkGuildGuid);
		kDBPacket.Push( static_cast< BYTE >(eCmdType) );
		kDBPacket.Push( rkMasterCharGuid );
		kDBPacket.Push( static_cast< BYTE >(GMG_Membmer) );
		kDBPacket.Push( rkNewMasterCharGuid );
		kDBPacket.Push( static_cast< BYTE >(GMG_Owner) );
		::SendToGuildMgr(kDBPacket);
	}

	__int64 iAfterCanRejoinHour = 0; // 기본 0
	bool InitGuildConstant()
	{
		int iValue = 1;
		if( S_OK != g_kVariableContainer.Get(EVar_Kind_Guild, EVar_CAN_REJOIN_GUILD_HOUR, iValue) )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find 'EVar_CoupleUse'") );
			return false;
		}
		iAfterCanRejoinHour = std::max(iValue, 0);
		return true;
	}
	bool IsCanRejoinUser(SContentsUser const& rkUser)
	{
		if( 0 == iAfterCanRejoinHour )
		{
			return true;
		}

		__int64 iUserTime = 0, iCurTime = 0;
		SYSTEMTIME kCurTime;
		::GetLocalTime(&kCurTime);
		CGameTime::SystemTime2SecTime(kCurTime, iCurTime, CGameTime::HOUR);
		CGameTime::DBTimeEx2SecTime(rkUser.kGuildLeaveDate, iUserTime, CGameTime::HOUR);

		bool const bRet = (iCurTime - iUserTime) >= iAfterCanRejoinHour;
		return bRet;
	}
	void UpdateLastLoingDate(BM::GUID const& rkGuildGuid, BM::GUID const& rkCharGuid)
	{
		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_COMMON, _T("EXEC [DBO].[UP_Guild_Member_Proc_UpdateLastLogin]"));
		kQuery.InsertQueryTarget(rkGuildGuid);
		kQuery.PushStrParam(rkCharGuid);
		kQuery.QueryOwner(rkGuildGuid);
		g_kCoreCenter.PushQuery(kQuery);
	}
}

namespace PgGuildMgrLogUtil
{
	void ChangeOwnerLog(PgGuild const* pkGuild, SContentsUser const& rkMaster, SContentsUser const& rkUser, EGuildCommandRet const eRet)
	{
		PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Guild_Master, rkMaster);
		{
			PgLog kSubLog(ELOrderMain_Guild_Master, ELOrderSub_Modify);
			kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
			kSubLog.Set( PgLogUtil::AtIndex(2), rkUser.Name() );
			kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(eRet) );
			kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Guid().str() );
			kSubLog.Set( PgLogUtil::AtIndex(4), rkUser.kCharGuid.str() );
			kLogAction.Add(kSubLog);
		}
		kLogAction.Commit();
	}
}


//////////////////////////////////////////////////////////////////////////////////
//	PgGuildMgr
//////////////////////////////////////////////////////////////////////////////////
PgGuildMgr::PgGuildMgr()
	: m_kAutoChangeOwnerMgr()
{
	Clear();
	m_kMgrGuid.Generate();
}

PgGuildMgr::~PgGuildMgr()
{
	Clear();
}

void PgGuildMgr::Clear()
{
	ContGuild::iterator erase_iter = m_kGuild.begin();
	while( m_kGuild.end() != erase_iter )
	{
		m_kGuildPool.Delete( (*erase_iter).second );
		++erase_iter;
	}
	m_kGuild.clear();
	m_kCharToGuild.clear();
	m_kNameToGuild.clear();
	m_kWaiter.Clear();
	m_kOtherWaiter.Clear();
	m_kGuildApplicantList.clear();
	m_iRecommendGuildRank = 0;
}

void PgGuildMgr::Locked_Tick()
{
	BM::CAutoMutex kLock( m_kMutex );
	if( m_kAutoChangeOwnerMgr.Tick() )
	{
		m_kAutoChangeOwnerMgr.GetOwnerLastLoginDay(MgrGuid());
	}
}

void PgGuildMgr::CheckNewDay()
{
	BM::CAutoMutex kLock( m_kMutex );

	static BM::PgPackedTime kNextTime;

	BM::PgPackedTime kNowTime = BM::PgPackedTime::LocalTime();
	if( kNextTime < kNowTime) // 처음엔 무조건 현재시간이 크다
	{
		// 길드목록 초기화
		ClearEntranceOpenGuild();
		
		{// 길드 목록 업데이트 요청
			bool bIsGMCommand = false;
			BM::Stream kPacket(PT_A_N_REQ_GET_ENTRANCE_OPEN_GUILD);
			kPacket.Push(bIsGMCommand);
			kPacket.Push(BM::GUID());
			::SendToGuildMgr(kPacket);
		}

		// 다음날 0시 0분 0초로 세팅
		kNextTime = kNowTime;
		CGameTime::AddTime(kNextTime, CGameTime::OneDay);
		kNextTime.Hour(0), kNextTime.Min(0), kNextTime.Sec(0);
	}
}

void PgGuildMgr::ProcessItemQuery(const BM::GUID &rkGuildGuid, const CEL::DB_RESULT &rkResult)
{
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();

	int const iCount = (*count_iter);

	if(iCount)
	{
		INFO_LOG( BM::LOG_LV6, __FL__<<" rkGuildGuid "<<rkGuildGuid<<" iCount : "<<iCount);
	}

	PgGuild* pkGuild = NULL;
	if( S_OK == Get(rkGuildGuid, pkGuild) )
	{
		PgInventory* pkInv = pkGuild->GetInven();
		if( pkInv )
		{
			CONT_SELECTED_CHARACTER_ITEM_LIST kContItemList;
			bool bRet = PgRealmUserManager::ProcessItemQuery( result_iter, rkResult.vecArray.end(), count_iter, *pkInv, &kContItemList );
			if(false==bRet)
			{
				INFO_LOG( BM::LOG_LV6, __FL__ << L"PgRealmUserManager::ProcessItemQuery return false. Guid : "<< rkGuildGuid);
			}

			if( 0 < pkInv->GetMaxIDX(IT_EQUIP) )
			{// 길드 금고를 개설 했다면
				BYTE abyExtern[MAX_DB_INVEXTEND_SIZE] = {0, };
				BYTE abyExternIdx[MAX_DB_INVEXTEND_SIZE] = {0, };

				if( pkInv->GetReductionInfo2Bin(abyExtern)
					&& pkInv->GetExtendIdx2Bin(abyExternIdx) )
				{
					if( false == g_kRealmUserMgr.Locked_RegistGuildInfo(pkGuild->Guid(), pkGuild->Name(), abyExtern, abyExternIdx, &kContItemList, pkInv->Money() ) )
					{//캐릭터와 동급으로 사용하도록 길드등록
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Fail Locked_RegistGuildInfo ")<<rkGuildGuid );
					}
					else
					{
						INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Success Locked_RegistGuildInfo ")<<rkGuildGuid);
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Fail Guild Inventory Get Size ")<<rkGuildGuid );
				}
			}
			else
			{
				INFO_LOG( BM::LOG_LV6, __FL__ << L"GetMaxIDX(IT_EQUIP) is 0. Guid : "<< rkGuildGuid);
			}
		}
		else
		{
			INFO_LOG( BM::LOG_LV6, __FL__ << L"Cannot GetInven. Guid : "<< rkGuildGuid);
		}
	}
	else
	{
		INFO_LOG( BM::LOG_LV6, __FL__ << L"Cannot Get Guild. Guid : "<< rkGuildGuid);
	}
}

void PgGuildMgr::Locked_SetEmporiaChallengeInfo( BM::GUID const &kReqCharGuid, BM::GUID const &kGuildID, SGuildEmporiaInfo const &kEmporiaInfo, __int64 const i64CostMoney, bool const bThrow, BYTE const byType )
{
	BM::CAutoMutex kLock( m_kMutex );

	PgGuild *pkGuild = NULL;
	if ( S_OK == Get( kGuildID, pkGuild ) )
	{
		pkGuild->EmporiaInfo( kEmporiaInfo );

        if( bThrow )
        {
            SGuild_Inventory_Log kLog( GetGuildInvLog(kGuildID, EGIT_EMPORIA_THROW, i64CostMoney) );

            BM::Stream kPacket;
            kLog.WriteToPacket(kPacket);
            kPacket.Push(kReqCharGuid);
            kPacket.Push(pkGuild->Master());
            kPacket.Push(byType);
            kPacket.Push(i64CostMoney);            

            SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
            pkActionOrder->InsertTarget(kReqCharGuid);
            pkActionOrder->kCause = CIE_EmporiaThrow;
            pkActionOrder->kContOrder.push_back( SPMO(IMET_ADD_MONEY|IMC_GUILD_INV,SModifyOrderOwner(kGuildID,OOT_Guild),SPMOD_Add_Money(i64CostMoney)) );
            pkActionOrder->kAddonPacket.Push(kPacket.Data());
            g_kJobDispatcher.VPush(pkActionOrder);
        }
        else
        {
            if ( GCR_None != byType )
		    {
			    BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, static_cast<BYTE>(GC_ReserveBattleForEmporia) );
			    kAnsPacket.Push(byType);
			    kAnsPacket.Push(kEmporiaInfo);
                kAnsPacket.Push(pkGuild->GetInven()->Money());
                kAnsPacket.Push(i64CostMoney);
			    pkGuild->BroadCast(kAnsPacket);
		    }
        }

        //여기서 Inv->Money가 적용 되었다는 것을 확신할 수 있을까?
		bool const bReserve = ( EMPORIA_KEY_BATTLERESERVE == kEmporiaInfo.byType );

		PgLogCont kLogCont( ELogMain_Contents_Guild, (bReserve ? ELogSub_EmBattle_Reserve : ELogSub_EmBattle_RefundExp) );

		PgLog kLog( ELOrderMain_Exp, ELOrderSub_Modify );
		kLog.Set( 0, pkGuild->Name() );
		kLog.Set( 2, kGuildID.str() );

        kLog.Set( 0, i64CostMoney );	                // 변화값
		kLog.Set( 2, pkGuild->GetInven()->Money() );	// 변화후 값

		// 여기는 성공
        kLog.Set( 0, static_cast<int>(GCR_Success==byType ? GCR_Success : GCR_Failed) );

		kLogCont.Add( kLog );
		kLogCont.Commit();

        if( !(bReserve && !bThrow) )
        {
            //길드전에 예약했다는 타입인데 도전취소값이 들어오는 난감한 경우
            VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Type mismatch [") << kEmporiaInfo.byType << _T("] != [") << (int)(bThrow) << _T("]") );
        }
	}
}

bool PgGuildMgr::ProcessMCommand(int const iCommand, BM::GUID const &rkCharGuid, BM::Stream *pkPacket)
{
	// 맵서버가 전송한 커맨드 (유저로 부터 오면 안된다)
	BM::Stream kAPacket;
	kAPacket.Push((BYTE)iCommand);

	switch(iCommand)
	{
	case GC_AddExp:
		{
			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(rkCharGuid, false, kAPacket) )
			{
				return false;
			}
		}break;
	case GC_UpKeepEmporiaExp:
		{
		}break;
	case GC_None:
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Invalid Guild Command [") << iCommand << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}

	kAPacket.Push(*pkPacket);

	return ProcessMsgFromUser(&kAPacket);//Recv From Client
}

bool PgGuildMgr::LoginGuildUser(SContentsUser const& rkUser)
{
	if( BM::GUID::NullData() == rkUser.kGuildGuid )
	{
		NotifyGuildApplicationState(rkUser.kCharGuid);
		DelCharToGuild(rkUser.kCharGuid);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgGuild* pkGuild = NULL;
	if( S_OK != Get(rkUser.kGuildGuid, pkGuild) )
	{
		// Load 된것이 없다면 DB에서 읽어 와야 한다.
		ReqLoadData(rkUser.kGuildGuid);
	}
	else
	{
		ChangePosGuildUser(rkUser);
		PgGuildMgrUtil::JoinGuildOrder(pkGuild, rkUser); // 길드 Guid, 스킬 다시 설정
	}
	// 실제로 Guild 정보를 user에게 보내는 것은 client가 GC_Info 요청할 때 보내면 된다. 여기서 보낼 필요 없다.
	if( pkGuild != NULL ) 
	{
		return true;
	}
	INFO_LOG( BM::LOG_LV6, __FL__ << L" First Time GuildUser Login. Guild Guid : "<< rkUser.kGuildGuid.str());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuildMgr::DelCharToGuild(BM::GUID const &rkCharGuid)
{
	m_kCharToGuild.erase(rkCharGuid);
	return true;
}

// 아래 함수 구현은 다른 채널의 Guild Member에 대해서 작동 하지 않는다.
HRESULT PgGuildMgr::ChangePosGuildUser(SContentsUser const &rkUserInfo)
{
	BM::GUID kGuildGuid;
	if( GetCharToGuild(rkUserInfo.kCharGuid, kGuildGuid) )
	{
		PgGuild* pkGuild = NULL;
		if(S_OK == Get(kGuildGuid, pkGuild))
		{
			SGuildMemberInfo kTemp;
			if( pkGuild->Get(rkUserInfo.kCharGuid, kTemp) )
			{
				if( 0 == kTemp.kGndKey.GroundNo()
				&&	0 != rkUserInfo.kGndKey.GroundNo() )
				{
					PgGuildMgrUtil::UpdateLastLoingDate(kGuildGuid, rkUserInfo.kCharGuid);
				}
			}

			int iOldPos = 0;
			if( S_OK == pkGuild->ChangePos(rkUserInfo) )
			{
				BM::Stream kListPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_List);
				kListPacket.Push((BYTE)GCR_Moved);
				if( pkGuild->WriteMemberListToPacket(kListPacket) )
				{
					pkGuild->BroadCast(kListPacket);
					return S_OK;
				}
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
			return S_FALSE;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgGuildMgr::Get(BM::GUID const &rkGuildGuid, PgGuild* &pkOut) const
{
	ContGuild::const_iterator kGuild_iter = m_kGuild.find(rkGuildGuid);
	if(m_kGuild.end() != kGuild_iter)
	{
		pkOut = kGuild_iter->second;
		return S_OK;
	}
	pkOut = NULL;
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

// Database 에서 Guild 정보를 읽어 온다.
void PgGuildMgr::ReqLoadData(BM::GUID const &rkGuildGuid, BM::Stream & rkCommandPacket)
{
	PgGuild* pkGuild = NULL;
	if( S_OK == Get(rkGuildGuid, pkGuild) )
	{
		return; // 이미 로드 되어져 있으면 무시
	}
	
	{
		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_SELECT_BASIC_INFO, _T("EXEC [DBO].[up_Guild_Proc_Info_SELECT]"));
		kQuery.InsertQueryTarget(rkGuildGuid);
		kQuery.PushStrParam(rkGuildGuid);
		kQuery.QueryOwner(rkGuildGuid);
		kQuery.contUserData.Push(rkCommandPacket.Data());
		g_kCoreCenter.PushQuery(kQuery);
	}

	/*{// 길드 금고 아이템
		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_INVENTORY_LOAD, _T("EXEC [dbo].[up_Item_Select]"));
		kQuery.InsertQueryTarget(rkGuildGuid);
		kQuery.PushStrParam(rkGuildGuid);
		kQuery.QueryOwner(rkGuildGuid);
		g_kCoreCenter.PushQuery(kQuery);
	}*/

	INFO_LOG( BM::LOG_LV6, __FL__ << L" Guild Guid : "<< rkGuildGuid.str() );
}

void PgGuildMgr::NotifyGuildApplicationState(BM::GUID const &rkCharGuid, BM::GUID const &rkGuildGuid, bool const bDestroy)
{
	if( bDestroy ) // 길드 해체시 전체 전송 용
	{// 모든 길드가입 신청자에게 전송
		CONT_GUILD_ENTRANCE_APPLICANT_LIST::iterator loop_iter = m_kGuildApplicantList.begin();
		while( loop_iter != m_kGuildApplicantList.end() )
		{
			CONT_GUILD_ENTRANCE_APPLICANT_LIST::mapped_type& rkEmlement = (*loop_iter).second;
			if( rkEmlement.kGuildGuid == rkGuildGuid )
			{// 해당 길드에 가입신청한 유저들의 상태를 모두 거절 상태로 바꾸고 알려준다.
				BYTE const byState = static_cast<BYTE>(AS_REJECT);
				rkEmlement.byState = byState;

				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GUILD_ENTRANCE_PROCESS, _T("EXEC [dbo].[up_Guild_Entrance_Process]"));
				kQuery.InsertQueryTarget(rkGuildGuid);
				kQuery.QueryOwner(rkGuildGuid);
				kQuery.PushStrParam(rkEmlement.kCharGuid);
				kQuery.PushStrParam(byState);
				g_kCoreCenter.PushQuery(kQuery);
				
				BM::Stream kPacket(PT_N_C_NFY_GUILD_ENTRANCE_STATE);
				kPacket.Push(byState);
				kPacket.Push(rkGuildGuid);
				kPacket.Push(rkEmlement.i64GuildEntranceFee);
				g_kRealmUserMgr.Locked_SendToUser(rkEmlement.kCharGuid, kPacket, false);
			}

			++loop_iter;
		}

		return ;
	}

	BM::Stream kPacket(PT_N_C_NFY_GUILD_ENTRANCE_STATE);
	CONT_GUILD_ENTRANCE_APPLICANT_LIST::iterator find_iter = m_kGuildApplicantList.find(rkCharGuid);
	if( find_iter != m_kGuildApplicantList.end() )
	{// 길드가입 신청한 유저에게 상태를 알려 줌.(상태는 ChangeApplicantState() 에서 이미 바꿧다.)
		CONT_GUILD_ENTRANCE_APPLICANT_LIST::mapped_type kGuildEntranceApplicant = (*find_iter).second;
		
		kPacket.Push(kGuildEntranceApplicant.byState);
		kPacket.Push(kGuildEntranceApplicant.kGuildGuid);
		kPacket.Push(kGuildEntranceApplicant.i64GuildEntranceFee);
	}
	else
	{
		kPacket.Push(static_cast<BYTE>(AS_NONE));
		kPacket.Push(BM::GUID::NullData());
		kPacket.Push(static_cast<__int64>(0));
	}
	g_kRealmUserMgr.Locked_SendToUser(rkCharGuid, kPacket, false);
}

void PgGuildMgr::WriteGuildEntranceApplicantListToPacket(BM::GUID const &rkGuildGuid, BM::Stream& rkPacket) const
{
	size_t iCount = 0;

	CONT_GUILD_ENTRANCE_APPLICANT_LIST::const_iterator loop_iter = m_kGuildApplicantList.begin();
	while( m_kGuildApplicantList.end() != loop_iter )
	{		
		CONT_GUILD_ENTRANCE_APPLICANT_LIST::mapped_type kApplicant = (*loop_iter).second;

		// 길드가입 거부한 유저는 빼고 보낸다.
		if( kApplicant.kGuildGuid == rkGuildGuid 
		&&	kApplicant.byState != AS_REJECT )
		{
			++iCount;
		}

		++loop_iter;
	}

	rkPacket.Push(iCount);

	CONT_GUILD_ENTRANCE_APPLICANT_LIST::const_iterator kApplicant_iter = m_kGuildApplicantList.begin();
	while( m_kGuildApplicantList.end() != kApplicant_iter )
	{
		CONT_GUILD_ENTRANCE_APPLICANT_LIST::mapped_type kApplicant = (*kApplicant_iter).second;

		if( kApplicant.kGuildGuid == rkGuildGuid
		&&	kApplicant.byState != AS_REJECT )
		{
			kApplicant.WriteToPacket(rkPacket);
		}

		++kApplicant_iter;
	}
}

void PgGuildMgr::AddGuildEntranceApplicant(const SGuildEntranceApplicant& kGuildEntranceApplicant, bool bSend)
{
	auto kResult = m_kGuildApplicantList.insert(std::make_pair(kGuildEntranceApplicant.kCharGuid, kGuildEntranceApplicant));
	if( !kResult.second )
	{
		return ;
	}

	if( bSend )
	{
		PgGuild* pkGuild = NULL;
		if( S_OK == Get(kGuildEntranceApplicant.kGuildGuid, pkGuild) )
		{// 신규가입 신청이 왔다. 마스터 이상 알려준다.
			BM::Stream kPacket(PT_N_C_NFY_NEW_GUILD_APPLICANT);
			kGuildEntranceApplicant.WriteToPacket(kPacket);
			pkGuild->BroadCast(kPacket, BM::GUID::NullData(), GMG_Master);
		}
	}
}

void PgGuildMgr::ChangeApplicantState(BM::GUID const &rkGuildGuid, BM::GUID const &rkCharGuid, BYTE byState)
{
	PgGuild* pkGuild = NULL;
	if( S_OK != Get(rkGuildGuid, pkGuild) )
	{
		return ;
	}

	BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast<BYTE>(GC_GuildEntranceApplicant_State));

	CONT_GUILD_ENTRANCE_APPLICANT_LIST::iterator find_iter = m_kGuildApplicantList.find(rkCharGuid);
	if( find_iter != m_kGuildApplicantList.end() )
	{
		CONT_GUILD_ENTRANCE_APPLICANT_LIST::mapped_type& rkGuildEntranceApplicant = (*find_iter).second;
		rkGuildEntranceApplicant.byState = byState; // 상태 변경
		
		kPacket.Push(static_cast<BYTE>(GCR_Success));
		kPacket.Push(rkCharGuid);
		kPacket.Push(byState);
	}
	else
	{// 대상을 찾을 수 없다.
		kPacket.Push(static_cast<BYTE>(GCR_NotFoundUser));
	}

	// 길드 오너, 마스터에게 전송
	pkGuild->BroadCast(kPacket, BM::GUID::NullData(), GMG_Master);
}

void PgGuildMgr::DelGuildEntranceApplicant(BM::GUID const &rkCharGuid)
{
	{
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_GUILD_REQ_ENTRANCE_CANCEL, _T("EXEC [dbo].[up_Guild_Entrance_Cancel]") );
		kQuery.InsertQueryTarget( rkCharGuid );
		kQuery.QueryOwner( rkCharGuid );		
		kQuery.PushStrParam( rkCharGuid );
		g_kCoreCenter.PushQuery( kQuery );
	}

	CONT_GUILD_ENTRANCE_APPLICANT_LIST::iterator find_iter = m_kGuildApplicantList.find(rkCharGuid);
	if( find_iter != m_kGuildApplicantList.end() )
	{// 길드가입 신청 취소 or 길드 가입 완료 or 길드 해체로 인한 삭제
		PgGuild* pkGuild = NULL;
		if( S_OK == Get((*find_iter).second.kGuildGuid, pkGuild) )
		{
			BM::Stream kPacket(PT_N_C_NFY_GUILD_ENTRANCE_CANCEL);
			kPacket.Push(rkCharGuid);
			pkGuild->BroadCast(kPacket, BM::GUID::NullData(), GMG_Master);
		}

		m_kGuildApplicantList.erase(find_iter);
	}
}

void PgGuildMgr::NotifyEntranceOpenGuildList(BM::GUID const& rkCharGuid)
{
	BM::Stream kPacket( PT_N_C_ANS_GET_ENTRANC_OPEN_GUILD, m_kOpenGuildList.size() );
	CONT_ENTRANCE_OPEN_GUILD_LIST::const_iterator loop_iter = m_kOpenGuildList.begin();
	while( loop_iter != m_kOpenGuildList.end() )
	{
		CONT_ENTRANCE_OPEN_GUILD_LIST::value_type kEntranceOpenGuild = (*loop_iter);
		kEntranceOpenGuild.WriteToPacket(kPacket);
		++loop_iter;
	}

	kPacket.Push(GetRecommendGuild());
	g_kRealmUserMgr.Locked_SendToUser( rkCharGuid, kPacket, false );
}

void PgGuildMgr::AddEntranceOpenGuild(SEntranceOpenGuild const& kEntranceOpenGuild)
{
	m_kOpenGuildList.push_back(kEntranceOpenGuild);
}

bool PgGuildMgr::AddGuildName(std::wstring const& rkGuildName, BM::GUID const &rkGuildGuid)
{
	ContWstrToGuild::iterator kName_iter = m_kNameToGuild.find(rkGuildName);
	if(m_kNameToGuild.end() != kName_iter)
	{
		m_kNameToGuild.erase(kName_iter);
	}
	auto ibRet = m_kNameToGuild.insert(std::make_pair(rkGuildName, rkGuildGuid));
	return ibRet.second;
}

bool PgGuildMgr::ProcessWaiter(BM::GUID const &rkGuildGuid)
{
	bool bRet = false;

	ContInfoWaiter::ContWaiterVec kInfoWaiter;
	bool const bInfoWaiter = m_kWaiter.GetWaiter(rkGuildGuid, kInfoWaiter);

	ContInfoWaiter::ContWaiterVec kOtherWaiter;
	bool const bOtherWatier = m_kOtherWaiter.GetWaiter(rkGuildGuid, kOtherWaiter);

	PgGuild* pkGuild = NULL;
	if( S_OK == Get(rkGuildGuid, pkGuild) )
	{
		if( bInfoWaiter
		&&	kInfoWaiter.size() )
		{
			// 길드 맴버
			BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_Info);
			kPacket.Push((BYTE)GCR_Success);
			pkGuild->WriteInfoToPacket(kPacket);

			BM::Stream kListPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_List);
			kListPacket.Push((BYTE)GCR_Success);
			pkGuild->WriteListToPacket(kListPacket);

			g_kRealmUserMgr.Locked_SendToUser(kInfoWaiter, kPacket);//Basic Info
			g_kRealmUserMgr.Locked_SendToUser(kInfoWaiter, kListPacket);//Member Info
			
			PgGuildMgrUtil::JoinGuildOrder(pkGuild, kInfoWaiter); // 길드 Guid, 스킬 다시 설정

			m_kWaiter.DelWaiter(rkGuildGuid);
		}

		if( bOtherWatier
		&&	kOtherWaiter.size() )
		{
			// 다른 사람
			BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_OtherInfo);
			kPacket.Push((BYTE)GCR_Success);

			SGuildOtherInfo const kInfo = pkGuild->GetOtherInfo();
			kInfo.WriteToPacket(kPacket);
			
			g_kRealmUserMgr.Locked_SendToUser(kOtherWaiter, kPacket);//Other Info

			m_kOtherWaiter.DelWaiter(rkGuildGuid);
		}
		return true;//성공
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;//실패
}

bool PgGuildMgr::Delete(PgGuild* &pkGuild)
{
	if( !pkGuild )
	{
		return false;
	}

	BM::GUID const kDeleteGuildGuid = pkGuild->Guid();
	pkGuild = NULL;
	return Delete(kDeleteGuildGuid);
}

bool PgGuildMgr::Delete(BM::GUID const &rkGuildGuid)
{
	ContGuild::iterator find_iter = m_kGuild.find(rkGuildGuid);
	if( m_kGuild.end() != find_iter )
	{
		m_kGuildPool.Delete(find_iter->second);
		m_kGuild.erase(find_iter);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuildMgr::AddCharToGuild(BM::GUID const &rkCharGuid, BM::GUID const &rkGuildGuid)
{
	if( BM::GUID::IsNull(rkCharGuid) )
	{
		return false;
	}

	ContCharToGuild::const_iterator kChar_iter = m_kCharToGuild.find(rkCharGuid);
	if(m_kCharToGuild.end() != kChar_iter)
	{
		if(kChar_iter->first == rkCharGuid && kChar_iter->second == rkGuildGuid)
		{
			return true;
		}
	}

	auto ibRet = m_kCharToGuild.insert(std::make_pair(rkCharGuid, rkGuildGuid));

	if( !ibRet.second )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return ibRet.second;
}

bool PgGuildMgr::ReqUpdateAbil(BM::GUID const &rkCharGuid, WORD const sAbilType, int const iUpdatedVal)
{
	BM::CAutoMutex kLock(m_kMutex);
	BM::GUID kGuildGuid;

	if( !GetCharToGuild(rkCharGuid, kGuildGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgGuild *pkGuild = NULL;
	if( S_OK != Get(kGuildGuid, pkGuild) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkGuild->UpdateAbil(rkCharGuid, sAbilType, iUpdatedVal);

	// Guild Member에게 전체 보내기
	BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_List);
	kPacket.Push((BYTE)GCR_Moved);
	if ( pkGuild->WriteMemberListToPacket( kPacket, rkCharGuid) )
	{
		pkGuild->BroadCast(kPacket);
	}
	return true;
}

bool PgGuildMgr::GetCharToGuild(BM::GUID const &rkCharGuid, BM::GUID& rkOut) const
{
	if( BM::GUID::IsNull(rkCharGuid) )
	{
		return false;
	}

	ContCharToGuild::const_iterator kChar_iter = m_kCharToGuild.find(rkCharGuid);
	if(m_kCharToGuild.end() != kChar_iter)
	{
		rkOut = kChar_iter->second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGuildMgr::LogoutGuildUser(SContentsUser const& rkUser)
{
	BM::GUID kGuildGuid;
	if( GetCharToGuild(rkUser.kCharGuid, kGuildGuid) )
	{
		PgGuild *pkGuild = NULL;
		if( S_OK == Get(kGuildGuid, pkGuild) )
		{
			if( !pkGuild->IsMember(rkUser.kCharGuid) )
			{
				Join(kGuildGuid, rkUser, false, GCR_None);
			}
		}
	}
	return SUCCEEDED(ChangePosGuildUser(rkUser));
}

EGuildCommandRet PgGuildMgr::Join(BM::GUID const &rkGuildGuid, SContentsUser const& rkUser, bool const bJoin, HRESULT hDBRet)
{
	eGuildCommandRet eRet = GCR_Failed;
	CONT_DEF_GUILD_LEVEL::const_iterator iter;

	//실제 가입이 일어 난다.
	if(BM::GUID::NullData() == rkGuildGuid)
	{
		eRet = GCR_Error;
		goto __FAILED_RET;
	}

	if( rkUser.Empty() )
	{
		eRet = GCR_Error;
		goto __FAILED_RET;
	}


	PgGuild *pkGuild = NULL;
	if( S_OK != Get(rkGuildGuid, pkGuild) )
	{
		eRet = GCR_Failed;
		goto __FAILED_RET;
	}

	if( !PgGuildMgrUtil::IsCanReqInOutState(rkUser) )
	{
		eRet = GCR_CantMe;
		goto __FAILED_RET;
	}

	CONT_DEF_GUILD_LEVEL const *pkDefGuildLevel = NULL;
	g_kTblDataMgr.GetContDef(pkDefGuildLevel);
	if( !pkDefGuildLevel )
	{
		assert(false);
		eRet = GCR_Max;
		goto __FAILED_RET;
	}

	iter = pkDefGuildLevel->find(pkGuild->GuildLevel());
	if( pkDefGuildLevel->end() == iter )
	{
		assert(false);
		eRet = GCR_Max;
		goto __FAILED_RET;
	}

	const int iMaxMemberCount = (*iter).second.iMaxMemberCount;
	if( iMaxMemberCount == pkGuild->MemberCount() )
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Limit max guild member count [%d] [%s]"), __FUNCTIONW__, __LINE__, iMaxMemberCount, pkGuild->Name().c_str());
		eRet = GCR_Max;
		goto __FAILED_RET;
	}
	//

	switch(hDBRet)
	{
	case GCR_None:
		{
			if( bJoin )//가입이면 DB로
			{
				BM::Stream kPacket(PT_N_T_REQ_GUILD_COMMAND, rkUser.kMemGuid);
				kPacket.Push((BYTE)GC_Join);
				kPacket.Push(rkGuildGuid);
				kPacket.Push(rkUser.kCharGuid);
				kPacket.Push((BYTE)GMG_Membmer);
				::SendToGuildMgr(kPacket);

				eRet = GCR_Success;
				goto __SUCCESSED_RET;
			}
			else//마스터에게 거절 메시지
			{
				BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_ReqJoin);
				kPacket.Push((BYTE)GCR_Reject);
				kPacket.Push(rkUser.Name());
				g_kRealmUserMgr.Locked_SendToUser(pkGuild->Master(), kPacket, false);

				eRet = GCR_Success;
				goto __FAILED_RET;
			}
		}break;
	case GCR_Success://DB 까지 갔다 와서 성공이면
		{
			if( AddCharToGuild(rkUser.kCharGuid, rkGuildGuid)
			&&	pkGuild->Add(rkUser, GMG_Membmer) )
			{
				eRet = GCR_Success;

				if( !rkUser.Empty() )
				{
					PgLogUtil::PgLogWrapperContents kLogAction(ELogMain_Contents_Guild, ELogSub_Guild_Join, rkUser);
					{
						PgLog kSubLog(ELOrderMain_Guild, ELOrderSub_Join);
						kSubLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
						kSubLog.Set( PgLogUtil::AtIndex(1), static_cast< int >(eRet) );
						kSubLog.Set( PgLogUtil::AtIndex(3), pkGuild->Guid().str() );
						kSubLog.Set( PgLogUtil::AtIndex(4), pkGuild->Master().str() );
						kLogAction.Add(kSubLog);
					}
					kLogAction.Commit();
				}
			}
			else
			{
				eRet = GCR_Failed;
				goto __FAILED_RET;
			}
			
		}break;
	case GCR_Failed://DB결과 실패
	default:
		{
			eRet = GCR_Failed;
			goto __FAILED_RET;
		}break;
	}

	if( GCR_Success == eRet
	&&	GCR_Success == hDBRet )//DB, 메모리도 모두 성공이어야
	{
		SGuildOtherInfo const kInfo = pkGuild->GetOtherInfo();
		//맵서버로 전송 pkUnit에 GuildGuid를 설정하고, BroadCasting하고, 저장한다.
		BM::Stream kMapPacket(PT_N_M_NFY_GUILD_COMMAND, (BYTE)GC_Join);//
		kMapPacket.Push((BYTE)GCR_Success);
		kMapPacket.Push(rkUser.kCharGuid);
		kInfo.WriteToPacket(kMapPacket);
		//pkGuild->WriteSkillToPacket(kMapPacket);
		g_kRealmUserMgr.Locked_SendToUserGround(rkUser.kMemGuid, kMapPacket, true, true);

		PgGuildMgrUtil::JoinGuildOrder(pkGuild, rkUser);

		ChangePosGuildUser(rkUser);

		BM::Stream kNfyPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_Join);//길드 맴버들에게 통보
		kNfyPacket.Push((BYTE)GCR_Notify);
		bool const bFindMemberRet = pkGuild->WriteMemberPacket(rkUser.kCharGuid, kNfyPacket);
		if( bFindMemberRet )
		{
			SendToGuild(pkGuild, kNfyPacket, rkUser.kCharGuid);
		}
	}
	else
	{
		BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_Join);//Return Code
		kPacket.Push((BYTE)GCR_Failed);
		g_kRealmUserMgr.Locked_SendToUser(rkUser.kMemGuid, kPacket);
	}

__SUCCESSED_RET:
	return eRet;

__FAILED_RET:
	DelCharToGuild(rkUser.kCharGuid);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << eRet);
	return eRet;
}

EGuildCommandRet PgGuildMgr::Leave(SContentsUser const& rkUser)
{
	BM::GUID kGuildGuid;
	if( !GetCharToGuild(rkUser.kCharGuid, kGuildGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
		return GCR_Error;
	}

	if( !PgGuildMgrUtil::IsCanReqInOutState(rkUser) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_CantMe"));
		return GCR_CantMe;
	}

	PgGuild *pkGuild = NULL;
	if( S_OK != Get(kGuildGuid, pkGuild) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
		return GCR_Error;
	}

	bool const bMember = pkGuild->IsMember(rkUser.kCharGuid);
	bool const bOwner = pkGuild->IsOwner(rkUser.kCharGuid);
	if( bOwner )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_None"));
		return GCR_None;//오너는 탈퇴 안되
	}

	if( !bMember )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
		return GCR_Error;
	}

	BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, rkUser.kMemGuid);//DB로 명령
	kDBPacket.Push((BYTE)GC_Leave);
	kDBPacket.Push(pkGuild->Guid());//일어날 장소?
	kDBPacket.Push(rkUser.kCharGuid);//삭제할 것
	kDBPacket.Push((BYTE)GMG_Membmer);
	::SendToGuildMgr(kDBPacket);

	return GCR_Success;
}

EGuildCommandRet PgGuildMgr::Kick(BM::GUID const rkGuildGuid, SContentsUser const& rkMaster, BM::GUID const& rkKickedGuid, bool const bGMCmd)
{
	BM::GUID kCharGuid;
	if( !bGMCmd )
	{
		if( rkMaster.kCharGuid == rkKickedGuid )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_DoNotMe"));
			return GCR_DoNotMe;
		}

		BM::GUID kGuildGuid;
		if( !GetCharToGuild(rkMaster.kCharGuid, kGuildGuid) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
			return GCR_Error;
		}

		if( !PgGuildMgrUtil::IsCanReqInOutState(rkMaster) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_CantMe"));
			return GCR_CantMe;
		}

		PgGuild *pkGuild = NULL;
		if( S_OK != Get(kGuildGuid, pkGuild) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
			return GCR_Error;
		}
		if( rkGuildGuid != kGuildGuid )
		{
			return GCR_Error;
		}

		bool const bMaster = pkGuild->IsMaster(rkMaster.kCharGuid);
		bool const bIsOwner = pkGuild->IsOwner(rkKickedGuid);
		SGuildMemberInfo kMasterInfo, kMemberInfo;
		bool const bGetMaster = pkGuild->Get(rkMaster.kCharGuid, kMasterInfo);
		bool const bIsMember = pkGuild->Get(rkKickedGuid, kMemberInfo);

		if( !bIsMember )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
			return GCR_Error;
		}
		
		SContentsUser kKickUser;
		if( S_OK == GetPlayerByGuid( kMemberInfo.kCharGuid, false, kKickUser ) )
		{// 추방대상이 온라인일 경우에만
			if( !PgGuildMgrUtil::IsCanReqInOutState(kMemberInfo.kGndKey.GroundNo()) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_CantHim"));
				return GCR_CantHim;
			}
		}

		if( !bMaster
		||	!bGetMaster )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotAuth"));
			return GCR_NotAuth;
		}

		if( bIsOwner )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_None"));
			return GCR_None;
		}

		if( kMasterInfo.cGrade == kMemberInfo.cGrade )
		{
			return GCR_Same;
		}
		kCharGuid = rkMaster.kCharGuid;
	}

	BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, kCharGuid); // DB로 명령
	kDBPacket.Push( static_cast< BYTE >(GC_M_Kick) ); // 신청자
	kDBPacket.Push( rkGuildGuid ); // 일어날 장소?
	kDBPacket.Push( rkKickedGuid ); // 삭제할 것
	kDBPacket.Push( static_cast< BYTE >(GMG_Membmer) );
	::SendToGuildMgr(kDBPacket);

	return GCR_Success;
}

bool PgGuildMgr::SendToGuild(BM::GUID const &rkGuildGuid, BM::Stream const& rkPacket, BM::GUID const &rkPassCharGuid) const
{
	if(BM::GUID::NullData() == rkGuildGuid) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if(0 == rkPacket.Size()) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgGuild* pkGuild = NULL;
	if( S_OK != Get(rkGuildGuid, pkGuild) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return SendToGuild(pkGuild, rkPacket, rkPassCharGuid);
}

bool PgGuildMgr::SendToGuild(PgGuild const* pkGuild, BM::Stream const& rkPacket, BM::GUID const &rkIgnoreGuid) const
{
	if( !pkGuild )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkGuild->BroadCast(rkPacket, rkIgnoreGuid);
	return true;
}

bool PgGuildMgr::SendToGuild_ByChar(BM::GUID const &rkCharGuid, BM::Stream const& rkPacket, BM::GUID const &rkPassCharGuid) const
{
	BM::GUID kGuildGuid;
	if( !GetCharToGuild(rkCharGuid, kGuildGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return SendToGuild(kGuildGuid, rkPacket, rkPassCharGuid);
}

PgGuild* PgGuildMgr::NewGuild()
{
	PgGuild* pkNew = m_kGuildPool.New();
	pkNew->Clear();
	return pkNew;
}

bool PgGuildMgr::FindGuildName(std::wstring const& rkGuildName) const
{
	if( rkGuildName.empty()
	||	MAX_GUILDNAME_LEN < rkGuildName.size() )
	{
		return true;
	}

	ContWstrToGuild::const_iterator kGuild_iter = m_kNameToGuild.find(rkGuildName);
	if( m_kNameToGuild.end() != kGuild_iter )
	{
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

const SGuildMemberGradeInfo PgGuild::DefaultGradeGroup()//Static
{
	static SGuildMemberGradeInfo kTempGrade;
	std::wstring kTemp;
	int const iDefStringBase = 5;
	std::wstring const &rkGradeName01 = kTempGrade.kGradeName[0];
	if(!rkGradeName01.size())
	{
		if(GetDefString(iDefStringBase, kTemp)) {kTempGrade.kGradeName[0] = kTemp;};
		if(GetDefString(iDefStringBase+1, kTemp)) {kTempGrade.kGradeName[1] = kTemp;};
		if(GetDefString(iDefStringBase+2, kTemp)) {kTempGrade.kGradeName[2] = kTemp;};
		if(GetDefString(iDefStringBase+3, kTemp)) {kTempGrade.kGradeName[3] = kTemp;};
		if(GetDefString(iDefStringBase+4, kTemp)) {kTempGrade.kGradeName[4] = kTemp;};
	}
	return kTempGrade;
}

EGuildCommandRet PgGuildMgr::Create(SContentsUser const& rkMaster, std::wstring const& rkGuildName, BYTE const cEmblem)
{
	BM::GUID kGuildGuid;
	BM::GUID kNewGuid = BM::GUID::Create();
	PgGuild *pkNewGuild = NULL;
	SGuildMemberGradeInfo const &rkGradeInfo = PgGuild::DefaultGradeGroup();

	EGuildCommandRet eRet = GCR_Success;
	if( FindGuildName(rkGuildName) )
	{
		eRet = GCR_Duplicate;
		goto GCR_CREATE_RETURN;
	}

	if( GetCharToGuild(rkMaster.kCharGuid, kGuildGuid) )
	{
		eRet = GCR_Duplicate;
		goto GCR_CREATE_RETURN;
	}

	if( Find(kNewGuid) )
	{
		kNewGuid.Generate();
		if( Find(kNewGuid) )
		{
			eRet = GCR_Duplicate;
			goto GCR_CREATE_RETURN;
		}
	}

	pkNewGuild = NewGuild();
	if( !pkNewGuild )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Guild Create failed MasterGuid[") << rkMaster.kCharGuid.str().c_str() << _T("]") );
		eRet = GCR_Failed;
		goto GCR_CREATE_RETURN;
	}

	if( !pkNewGuild->Set(kNewGuid, rkMaster.kCharGuid, rkGuildName, BM::PgPackedTime::LocalTime(), cEmblem) )
	{
		eRet = GCR_Failed;
		goto GCR_CREATE_RETURN;
	}
	
	//기본 계급명 설정
	if( !pkNewGuild->AddGradeGroup(rkGradeInfo) )
	{
		eRet = GCR_Failed;
		goto GCR_CREATE_RETURN;
	}

	if( !AddCharToGuild(rkMaster.kCharGuid, pkNewGuild->Guid()) )
	{
		eRet = GCR_Failed;
		goto GCR_CREATE_RETURN;
	}

	//Master add
	if( !pkNewGuild->Add(rkMaster, GMG_Owner) )
	{
		DelCharToGuild(rkMaster.kCharGuid);
		eRet = GCR_Failed;
		goto GCR_CREATE_RETURN;
	}

	if( !AddGuildName(pkNewGuild->Name(), pkNewGuild->Guid()) )
	{
		eRet = GCR_Failed;
		goto GCR_CREATE_RETURN;
	}

	auto kRet = m_kGuild.insert( std::make_pair(kNewGuid, pkNewGuild) );
	if( !kRet.second )
	{
		eRet = GCR_Failed;
		goto GCR_CREATE_RETURN;
	}

	if( GCR_Success == eRet
	&&	pkNewGuild )
	{
//		INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] [GUILD-Create: %s] Success - Goto Create Guild in DB [GuildName: %s]"), __FUNCTIONW__, __LINE__, rkMaster.Name().c_str(), rkGuildName.c_str());

		BM::Stream kPacket(PT_N_T_REQ_GUILD_COMMAND, rkMaster.kMemGuid);
		kPacket.Push((BYTE)GC_Create);
		kPacket.Push(kNewGuid);
		kPacket.Push(rkMaster.kCharGuid);
		kPacket.Push(rkGuildName);
		kPacket.Push(cEmblem);
		kPacket.Push(rkGradeInfo.kGradeName[0]);//1
		kPacket.Push(rkGradeInfo.kGradeName[1]);//2
		kPacket.Push(rkGradeInfo.kGradeName[2]);//3
		kPacket.Push(rkGradeInfo.kGradeName[3]);//4
		kPacket.Push(rkGradeInfo.kGradeName[4]);//5
		::SendToGuildMgr(kPacket);
	}

GCR_CREATE_RETURN :
	if( GCR_Success != eRet
	&&	pkNewGuild != NULL )
	{
		DelCharToGuild(rkMaster.kCharGuid);
		DelGuildName(rkGuildName);
		m_kGuildPool.Delete(pkNewGuild);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << eRet);
	}

	return eRet;
}

bool PgGuildMgr::Find(BM::GUID const &rkGuildGuid) const
{
	ContGuild::const_iterator kGuild_iter = m_kGuild.find(rkGuildGuid);
	if(m_kGuild.end() != kGuild_iter)
	{
		return true;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

EGuildCommandRet PgGuildMgr::ReqJoin(SContentsUser const& rkMaster, BM::GUID const &rkGuildGuid, SContentsUser const& rkUser) const
{//실제 가입은 없다
	if( rkMaster.Empty() ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
		return GCR_Error;
	}
	if( BM::GUID::NullData() == rkGuildGuid ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
		return GCR_Error;
	}
	if( rkUser.Empty() ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}//상태를 못 찾았다

	BM::GUID kUserGuildGuid;
	if( GetCharToGuild(rkUser.kCharGuid, kUserGuildGuid) )
	{
		if( rkGuildGuid != kUserGuildGuid )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Duplicate"));
			return GCR_Duplicate;//이미 다른 길드 가입
		}
		else if( BM::GUID::NullData() == rkUser.kGuildGuid )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_JoinWait"));
			return GCR_JoinWait;
		}
		else 
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Member"));
			return GCR_Member;//이미 같은 길드
		}
	}

	PgGuild *pkGuild = NULL;
	if(S_OK == Get(rkGuildGuid, pkGuild))
	{
		if( !pkGuild->IsMaster(rkMaster.kCharGuid) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotAuth"));
			return GCR_NotAuth;
		}

		if( !PgGuildMgrUtil::IsCanReqInOutState(rkMaster) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_CantMe"));
			return GCR_CantMe;
		}
		if( !PgGuildMgrUtil::IsCanReqInOutState(rkUser) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_CantHim"));
			return GCR_CantHim;
		}

		CONT_DEF_GUILD_LEVEL const *pkDefGuildLevel = NULL;
		g_kTblDataMgr.GetContDef(pkDefGuildLevel);
		if( !pkDefGuildLevel )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Cannot get DefGuildLevel from TblDataMgr") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Max"));
			return GCR_Max;
		}

		CONT_DEF_GUILD_LEVEL::const_iterator iter = pkDefGuildLevel->find(pkGuild->GuildLevel());
		if( pkDefGuildLevel->end() == iter )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Cannot get GuildLevel Level[") << pkGuild->GuildLevel() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Max"));
			return GCR_Max;
		}

		const int iMaxMemberCount = (*iter).second.iMaxMemberCount;
		if(iMaxMemberCount <= (int)pkGuild->MemberCount())
		{
			//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Limit max guild member count [%d] [%s]"), __FUNCTIONW__, __LINE__, iMaxMemberCount, rkGuild.Name().c_str());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Max"));
			return GCR_Max;//만땅
		}

		if( !PgGuildMgrUtil::IsCanRejoinUser(rkUser) )
		{
			BM::Stream kRetPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast< BYTE >(GC_ReqJoin));//Return Code
			kRetPacket.Push( static_cast< BYTE >(GCR_Date_Limit) );
			kRetPacket.Push( PgGuildMgrUtil::iAfterCanRejoinHour );
			g_kRealmUserMgr.Locked_SendToUser(rkUser.kMemGuid, kRetPacket); // 길드 마스터에게 패킷을 보내자

			return GCR_Date_Limit; // 길드 탈퇴 한지 얼마 지나지 않았음
		}

		//Request Join
		BM::Stream kReqPacket(PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_ReqJoin);
		kReqPacket.Push((BYTE)GCR_None);
		kReqPacket.Push(rkMaster.Name());//누구로 부터
		kReqPacket.Push(pkGuild->Guid());
		kReqPacket.Push(pkGuild->Name());//이름의 길드에
		g_kRealmUserMgr.Locked_SendToUser(rkUser.kMemGuid, kReqPacket);//가입 요청이 들어왔습니다. (Y/N)


		{
			PgLogUtil::PgLogWrapperContents kLogCont(ELogMain_Contents_Guild, ELogSub_Guild_Invite, rkMaster);
			PgLog kLog(ELOrderMain_Guild_Invite, ELOrderSub_None);
			kLog.Set( PgLogUtil::AtIndex(1), pkGuild->Name() );
			kLog.Set( PgLogUtil::AtIndex(2), pkGuild->Guid().str() );
			kLog.Set( PgLogUtil::AtIndex(3), rkUser.Name() );
			kLog.Set( PgLogUtil::AtIndex(4), rkUser.kCharGuid.str() );
			kLogCont.Add(kLog);
			kLogCont.Commit();
		}

		return GCR_Success;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Error"));
	return GCR_Error;
}

bool PgGuildMgr::DelGuildName(std::wstring const& rkGuildName)
{
	m_kNameToGuild.erase(rkGuildName);
	return true;
}

EGuildCommandRet PgGuildMgr::ReqRename(BM::GUID const& rkGuildGuid, BM::GUID const& rkMasterMemberGuid, std::wstring const& rkNewName, bool const bGMCmd)
{
	if( BM::GUID::IsNull(rkGuildGuid) )
	{
		return GCR_Failed;
	}

	if( rkNewName.empty() ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed; 
	}

	if( GE_Name_Max < rkNewName.size() )
	{
		return GCR_Max;
	}

	std::wstring kNewGuildName = rkNewName;
	if( false == g_kUnicodeFilter.IsCorrect(UFFC_GUILD_NAME, kNewGuildName)
	||	true == g_kFilterString.Filter(kNewGuildName, false, FST_ALL) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_BadName"));
		return GCR_BadName;
	}

	if( FindGuildName(rkNewName) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Duplicate"));
		return GCR_Duplicate;//중복된 이름
	}

	BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, rkMasterMemberGuid);
	kDBPacket.Push( (BYTE)GC_M_Rename );
	kDBPacket.Push( rkGuildGuid );
	kDBPacket.Push( rkNewName );
	kDBPacket.Push( bGMCmd );
	::SendToGuildMgr(kDBPacket);

	return GCR_Success;
}

EGuildCommandRet PgGuildMgr::ReqRename(PgGuild* pkGuild, SContentsUser const& rkMaster, std::wstring const& rkNewName)
{
	if( !pkGuild )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	bool const bIsMaster = pkGuild->IsMaster(rkMaster.kCharGuid);
	if( bIsMaster
	&&	GE_Name_Max < rkNewName.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Max"));
		return GCR_Max;
	}

	if(	!bIsMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotAuth"));
		return GCR_NotAuth;
	}

	return ReqRename(pkGuild->Guid(), rkMaster.kMemGuid, rkNewName);
}

EGuildCommandRet PgGuildMgr::ReqLevelup(PgGuild* pkGuild, SContentsUser const &rkMaster, bool const bTest)
{
	if( rkMaster.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	if( rkMaster.kCharGuid != pkGuild->Master() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotAuth"));
		return GCR_NotAuth;
	}

	EGuildCommandRet const eRet = pkGuild->Levelup(bTest);
	switch(eRet)
	{
	case GCR_Success:
		{
			//성공
			if( bTest )
			{
				BM::Stream kPacket(PT_N_M_NFY_GUILD_COMMAND, (BYTE)GC_M_LvUp);
				kPacket.Push((BYTE)eRet);
				kPacket.Push(rkMaster.kCharGuid);
				kPacket.Push(pkGuild->GuildLevel());
				g_kRealmUserMgr.Locked_SendToUserGround(rkMaster.kMemGuid, kPacket, true, true);
			}
			else
			{
				Save(pkGuild, rkMaster.kCharGuid, GC_M_LvUp);
			}
		}break;
	case GCR_Max:
		{
			//최고 레벨
		}break;
	case GCR_Failed:
	case GCR_NotEnoughExp:
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotEnoughExp"));
			return GCR_NotEnoughExp;//무조건 모질라
		}
	}
	return eRet;
}

EGuildCommandRet PgGuildMgr::ReqGuildInventoryCreate(PgGuild* pkGuild, SContentsUser const &rkMaster, bool const bTest)
{
	if( rkMaster.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	if( rkMaster.kCharGuid != pkGuild->Master() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotAuth"));
		return GCR_NotAuth;
	}

	if( bTest )
	{
		BM::Stream kPacket(PT_N_M_NFY_GUILD_COMMAND, (BYTE)GC_M_InventoryCreate);
		kPacket.Push(static_cast<BYTE>(GCR_Success));
		kPacket.Push(rkMaster.kCharGuid);
		kPacket.Push(pkGuild->GuildLevel());
		g_kRealmUserMgr.Locked_SendToUserGround(rkMaster.kMemGuid, kPacket, true, true);
	}
	else
	{// 길드금고를 생성 할 수 있는 모든 테스트 통과
		Save(pkGuild, rkMaster.kCharGuid, GC_M_InventoryCreate);
	}

	return GCR_Success;
}

EGuildCommandRet PgGuildMgr::ReqAddSkill(PgGuild* pkGuild, SContentsUser const &rkMaster, int const iSkillNo, bool const bTest)
{
	if( rkMaster.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	if( !pkGuild->IsMaster(rkMaster.kCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotAuth"));
		return GCR_NotAuth;
	}

	EGuildCommandRet eRet = pkGuild->AddSkill(iSkillNo, bTest);
	switch(eRet)
	{
	case GCR_Success:
		{
			if( bTest )
			{
				BM::Stream kPacket(PT_N_M_NFY_GUILD_COMMAND, (BYTE)GC_M_AddSkill);
				kPacket.Push((BYTE)GCR_None);//
				kPacket.Push(rkMaster.kCharGuid);
				kPacket.Push(pkGuild->GuildLevel());
				kPacket.Push(iSkillNo);
				g_kRealmUserMgr.Locked_SendToUserGround(rkMaster.kMemGuid, kPacket, true, true);
			}
			else
			{
				Save(pkGuild, rkMaster.kCharGuid, GC_M_AddSkill, iSkillNo);
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << eRet);
			//실패
		}break;
	}
	return eRet;
}

EGuildCommandRet PgGuildMgr::ReqChangeOwner(EGuildCommand const eCmdType, PgGuild* pkGuild, BM::GUID const& rkMasterCharGuid, BM::GUID const& rkNewMasterCharGuid)
{
	if( BM::GUID::IsNull(rkMasterCharGuid)
	||	BM::GUID::IsNull(rkNewMasterCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	if( rkMasterCharGuid == rkNewMasterCharGuid )
	{
		return GCR_Same;
	}

	if( !pkGuild->IsOwner(rkMasterCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotAuth"));
		return GCR_NotAuth;
	}

	EGuildCommandRet const eRet = pkGuild->ChangeOwner(rkNewMasterCharGuid, true);
	switch( eRet )
	{
	case GCR_Success:
		{
			PgGuildMgrUtil::ChangeOwnerQuery(eCmdType, pkGuild->Guid(), rkMasterCharGuid, rkNewMasterCharGuid);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << eRet);
		}break;
	}
	return eRet;
}

EGuildCommandRet PgGuildMgr::ReqSetGuildNotice(PgGuild* pkGuild, SContentsUser const &rkMaster, BM::Stream &rkPacket)
{
	if( rkMaster.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Failed"));
		return GCR_Failed;
	}

	std::wstring kNotice;
	rkPacket.Pop( kNotice );

	if( !pkGuild->IsMaster(rkMaster.kCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_NotAuth"));
		return GCR_NotAuth;
	}

	if( false == g_kUnicodeFilter.IsCorrect(UFFC_GUILD_NOTICE, kNotice)
	||	true == g_kFilterString.Filter(kNotice, false, FST_BADWORD) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_BadName"));
		return GCR_BadName;
	}

	if( iNoticeMaxCharacter < kNotice.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Max"));
		return GCR_Max;
	}

	if( pkGuild->Notice() == kNotice )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Same"));
		return GCR_Same;
	}

	BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, rkMaster.kMemGuid);
	kDBPacket.Push( (BYTE)GC_M_Notice );
	kDBPacket.Push( pkGuild->Guid() );
	kDBPacket.Push( kNotice );
	::SendToGuildMgr(kDBPacket);
	
	return GCR_Success;
}

EGuildCommandRet PgGuildMgr::ReqSetLevel(PgGuild * pkGuild, int const iLv)
{
	if( !pkGuild )
	{
		return GCR_Failed;
	}

	EGuildCommandRet const eRet = pkGuild->SetLevel(iLv);
	switch(eRet)
	{
	case GCR_Success:
		{
			//성공
			BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, pkGuild->Guid());
			kDBPacket.Push( static_cast<BYTE>(GC_GM_SetLv) );
			kDBPacket.Push( iLv );
			kDBPacket.Push( pkGuild->SkillPoint() );
			::SendToGuildMgr(kDBPacket);
		}break;
	case GCR_Same:
		{
			//최고 레벨
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return Not Defined"));
		}
	}
	return eRet;
}

EGuildCommandRet PgGuildMgr::ReqSetExp(PgGuild * pkGuild, __int64 const iExp)
{
	if( !pkGuild )
	{
		return GCR_Failed;
	}

	EGuildCommandRet eRet = GCR_Success;
	if( iExp == pkGuild->GetExp() )
	{
		eRet = GCR_Same;
	}

	switch(eRet)
	{
	case GCR_Success:
		{
			pkGuild->SetExp(iExp);

			//성공
			BM::Stream kDBPacket(PT_N_T_REQ_GUILD_COMMAND, pkGuild->Guid());
			kDBPacket.Push( static_cast<BYTE>(GC_GM_SetExp) );
			kDBPacket.Push( pkGuild->GetExp() );
			::SendToGuildMgr(kDBPacket);
		}break;
	case GCR_Same:
		{
			//최고 레벨
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return Not Defined"));
		}
	}
	return eRet;
}

EGuildCommandRet PgGuildMgr::ReqSetMoney(PgGuild * pkGuild, __int64 const iMoney)
{
	if( !pkGuild )
	{
		return GCR_Failed;
	}

	__int64 const iAddMoney = iMoney - pkGuild->GetInven()->Money();
	if( 0==iAddMoney )
	{
		return GCR_Same;
	}

	BM::GUID const& kGuildGuid = pkGuild->Guid();
	SGuild_Inventory_Log kLog( GetGuildInvLog(kGuildGuid, EGIT_GM_ORDER, iAddMoney) );

	BM::Stream kPacket;
	kLog.WriteToPacket(kPacket);

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kGuildGuid);
	//pkActionOrder->kCause = CIE_EmporiaThrow;
	pkActionOrder->kContOrder.push_back( SPMO(IMET_ADD_MONEY|IMC_GUILD_INV,SModifyOrderOwner(kGuildGuid,OOT_Guild),SPMOD_Add_Money(iAddMoney)) );
	pkActionOrder->kAddonPacket.Push(kPacket.Data());
	g_kJobDispatcher.VPush(pkActionOrder);
	return GCR_Success;
}

HRESULT PgGuildMgr::Save(PgGuild *pkGuild, BM::GUID const& rkOperatorGuid, EGuildCommand eCmd, int const iExternInt) const
{
	if( !pkGuild )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	BM::GUID const &rkGuildGuid = pkGuild->Guid();

	BM::Stream kPacket(PT_N_T_REQ_GUILD_COMMAND, rkGuildGuid);
	kPacket.Push((BYTE)eCmd);
	pkGuild->WriteToSavePacket(kPacket);
	kPacket.Push(iExternInt);
	kPacket.Push(rkOperatorGuid);
	::SendToGuildMgr(kPacket);
	return S_OK;
}

HRESULT PgGuildMgr::ModifyGuildInventory(const BM::GUID const& rkGuildGuid, const DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, const std::wstring &rkCharName)
{
	PgGuild* pkGuild = NULL;
	if( S_OK == Get(rkGuildGuid, pkGuild) )
	{
		PgInventory* pkInv = pkGuild->GetInven();
		if( pkInv )
		{
			DB_ITEM_STATE_CHANGE_ARRAY::const_iterator item_itor = rkChangeArray.begin();
			while( rkChangeArray.end() != item_itor )
			{
				DB_ITEM_STATE_CHANGE_ARRAY::value_type const &rkCurModify = (*item_itor);

				if( rkCurModify.State() == DISCT_MODIFY_GUILD_MONEY )
				{
					BM::Stream kAddonData = rkCurModify.kAddonData;
					__int64 i64NewMoney;
					kAddonData.Pop(i64NewMoney);
					pkInv->Money(i64NewMoney);
				}
				else
				{
					PgItemWrapper const& rkItemWrapper = rkCurModify.ItemWrapper();
					if( false == pkInv->Modify(rkItemWrapper.Pos(), rkItemWrapper) )
					{
						return E_FAIL;
					}
				}

				++item_itor;
			}
		}

		pkGuild->BroadCastGuildInventoryObserverForUpdateItem(rkChangeArray, rkCharName);
		return S_OK;
	}
	
	return E_FAIL;
}

PgInventory* PgGuildMgr::GetInven(const BM::GUID& rkGuildGuid )
{
	PgGuild* pkGuild = NULL;
	if( S_OK == Get(rkGuildGuid, pkGuild) )
	{
		return pkGuild->GetInven();
	}

	return NULL;
}

bool PgGuildMgr::IsHaveGuildInvAuthority_In(const BM::GUID& rkGuildGuid, const BM::GUID& rkCharGuid )
{
	PgGuild* pkGuild = NULL;
	if( S_OK == Get(rkGuildGuid, pkGuild) )
	{
		return pkGuild->IsHaveGuildInvAuthority_In(rkCharGuid);
	}

	return false;
}

bool PgGuildMgr::IsHaveGuildInvAuthority_Out(const BM::GUID& rkGuildGuid, const BM::GUID& rkCharGuid )
{
	PgGuild* pkGuild = NULL;
	if( S_OK == Get(rkGuildGuid, pkGuild) )
	{
		return pkGuild->IsHaveGuildInvAuthority_Out(rkCharGuid);
	}

	return false;
}
