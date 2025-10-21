#include "stdafx.h"
#include "PgMyHome.h"
#include "tabledatamanager.h"

void PgMyHome::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket(rkPacket,kWriteType);

	m_kNetModule.GetNet().WriteToPacket( rkPacket );
	rkPacket.Push( m_kVolatileID );
	PgPlayerPlayTime::WriteToPacket_PlayTime(rkPacket);

	switch( kWriteType&WTCHK_TYPE )
	{
	case WT_DEFAULT:
	case WT_DEFAULT_WITH_EXCLUDE:
		{
			//SPlayerDBData
			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				rkPacket.Push( GetMemberGUID() );
				rkPacket.Push( m_kInfo );
			}

			rkPacket.Push( GetPos().x );
			rkPacket.Push( GetPos().y );
			rkPacket.Push( GetPos().z );

			rkPacket.Push( m_kBasic );//SPlayerBasicInfo

			m_kInv.WriteToPacket(rkPacket, kWriteType);

			rkPacket.Push( ConnArea() );
			rkPacket.Push( m_kMemberID );

			rkPacket.Push( GetAbil(AT_MYHOME_TODAYHIT) );
			rkPacket.Push( GetAbil(AT_MYHOME_TOTALHIT) );
			rkPacket.Push( GetAbil(AT_MYHOME_VISITLOGCOUNT) );

		}break;
	case WT_SIMPLE:
		{
//			SPlayerDBSimpleData kSimpleData;
//			GetInfo(kSimpleData);
//			rkPacket.Push( kSimpleData );
//			이전에 SPlayerDBSimpleData로 되어있던것
			rkPacket.Push( GetMemberGUID() );
			rkPacket.Push( GetID() );
			rkPacket.Push( PartyGuid() );
			rkPacket.Push( GuildGuid() );
			rkPacket.Push( CoupleGuid() );//5
			rkPacket.Push( Name() );
			rkPacket.Push( m_kInfo.byGender );
			rkPacket.Push( m_kInfo.iRace );
			rkPacket.Push( m_kInfo.iClass );
			rkPacket.Push( m_kInfo.wLv );//10
			rkPacket.Push( m_kInfo.sTacticsLevel );
			rkPacket.Push( GetPos() );
			rkPacket.Push( m_kInfo.iHairColor );
			rkPacket.Push( m_kInfo.iHairStyle );
			rkPacket.Push( m_kInfo.iFace );//15
			rkPacket.Push( m_kInfo.iJacket );
			rkPacket.Push( m_kInfo.iPants );
			rkPacket.Push( m_kInfo.iShoes );
			rkPacket.Push( m_kInfo.iGloves );
			rkPacket.Push( m_kInfo.byFiveElementBody );//20
			rkPacket.Push( m_kInfo.kBirthDate );
			rkPacket.Push( GetAbil(AT_RECENT) );
			rkPacket.Push( m_byGMLevel );	
			rkPacket.Push( m_kIsCreateCard);
			rkPacket.Push( GetAbil(AT_HP) );// 24
			
			//SIMPLE_EXTRA_USERINFO
			rkPacket.Push( GetActionID() );

			rkPacket.Push( (BYTE)GetAbil(AT_TEAM) );//팀

			m_kInv.WriteToPacket(IT_HOME, rkPacket);

			rkPacket.Push( GetAbil(AT_CLIENT_OPTION_SAVE) );

			m_kCardAbilObj.WriteToPacket(rkPacket);

			rkPacket.Push( GetAbil(AT_MYHOME_TODAYHIT) );
			rkPacket.Push( GetAbil(AT_MYHOME_TOTALHIT) );
			rkPacket.Push( GetAbil(AT_MYHOME_VISITLOGCOUNT) );
		}break;
	default:
		{
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled WriteType[%d]"), __FUNCTIONW__, kWriteType);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning! Invalid CaseType"));
		}break;
	}

	if (WT_DEFAULT_EXCLUDE & kWriteType)
	{
		m_kGroundKey.WriteToPacket(rkPacket);
	}

	if(0 == (kWriteType & WT_OP_No_Contents)
		&& ( WT_OP_MISSION & kWriteType ))
	{
		m_kMissionReport.WriteToPacket_MissionReport( rkPacket );
	}

	if( kWriteType & WT_OP_DBPlayerState )
	{
		rkPacket.Push( m_kInfo.byState );
	}

	rkPacket.Push( m_iUID );

	rkPacket.Push(m_kOwnerGuid);
	rkPacket.Push(m_kHomeAddr);
	rkPacket.Push(m_kIsInSide);
	rkPacket.Push(m_kOwnerName);
	rkPacket.Push(GetAbil(AT_MYHOME_VISITFLAG));
	rkPacket.Push(GetAbil(AT_MYHOME_STATE));
	rkPacket.Push(GetAbil(AT_HOME_SIDEJOB));
}

EWRITETYPE PgMyHome::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE const kWriteType = CUnit::ReadFromPacket(rkPacket);

	BM::GUID kMemberGuid = GetMemberGUID();
	SERVER_IDENTITY kSwitchServer;

	kSwitchServer.ReadFromPacket(rkPacket);
	rkPacket.Pop( m_kVolatileID );
	PgPlayerPlayTime::ReadFromPacket_PlayTime(rkPacket);
	
	switch( kWriteType & WTCHK_TYPE )
	{
	case WT_DEFAULT:
	case WT_DEFAULT_WITH_EXCLUDE:
		{
			//SPlayerInfo;
			BYTE byTeam = 0;
			POINT3 kPos;
			__int64 iVal64 = 0;	
			int iVal = 0;

			//SPlayerDBData
			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				rkPacket.Pop( kMemberGuid );
				rkPacket.Pop( m_kInfo );				//1
			}

			rkPacket.Pop( kPos.x );
			rkPacket.Pop( kPos.y );
			rkPacket.Pop( kPos.z ); SetPos(kPos);

			rkPacket.Pop( m_kBasic );//SPlayerBasicInfo	

			m_kInv.ReadFromPacket(rkPacket, kWriteType);

			rkPacket.Pop( m_kConnArea );				
			rkPacket.Pop( m_kMemberID );

			int iValue = 0;
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_TODAYHIT,iValue);
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_TOTALHIT,iValue);
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_VISITLOGCOUNT,iValue);

			Name(m_kInfo.szName);
			if (0 == (kWriteType & WT_OP_No_Contents))
			{
				m_kInv.OwnerGuid(m_kInfo.guidCharacter);
				m_kInv.OwnerName(m_kInfo.szName);
				InvenRefreshAbil();
			}

			MacroResetInputTime();					// 매크로 입력 시간은 맵 이동시마다 다시 초기화

			SetAbil(AT_ENABLE_AUTOHEAL, AUTOHEAL_PLAYER);

			CopyAbilFromClassDef();
			CalculateInitAbil();

		}break;
	case WT_SIMPLE:
		{
			SetState(US_IDLE);

//			SPlayerDBSimpleData kSimpleData;
//			rkPacket.Pop( kSimpleData );

//			이전에 SPlayerDBSimpleData로 되어 있던것
			rkPacket.Pop( kMemberGuid );
			rkPacket.Pop( m_kInfo.guidCharacter );	SetID( m_kInfo.guidCharacter );
			rkPacket.Pop( m_kPartyGuid );
			rkPacket.Pop( m_kInfo.kGuildGuid );
			rkPacket.Pop( m_kInfo.kCoupleGuid );//5
			rkPacket.Pop( m_kName );
			rkPacket.Pop( m_kInfo.byGender );
			rkPacket.Pop( m_kInfo.iRace );
			rkPacket.Pop( m_kInfo.iClass );
			rkPacket.Pop( m_kInfo.wLv );//10
			rkPacket.Pop( m_kInfo.sTacticsLevel );

			POINT3 kPos;
			rkPacket.Pop( kPos );
			SetPos( kPos );

			rkPacket.Pop( m_kInfo.iHairColor );
			rkPacket.Pop( m_kInfo.iHairStyle );
			rkPacket.Pop( m_kInfo.iFace );//15
			rkPacket.Pop( m_kInfo.iJacket );
			rkPacket.Pop( m_kInfo.iPants );
			rkPacket.Pop( m_kInfo.iShoes );
			rkPacket.Pop( m_kInfo.iGloves );
			rkPacket.Pop( m_kInfo.byFiveElementBody );//20
			rkPacket.Pop( m_kInfo.kBirthDate );
			rkPacket.Pop( m_kInfo.kRecentInfo.iMapNo );
			rkPacket.Pop( m_byGMLevel );
			rkPacket.Pop( m_kIsCreateCard);

			int iVal = 0;
			rkPacket.Pop( iVal );//24
			SetAbil( AT_HP, iVal );

			//SIMPLE_EXTRA_USERINFO
			rkPacket.Pop( iVal );	SetActionID(iVal);

			_tcscpy_s( m_kInfo.szName, MAX_CHARACTERNAME_LEN, Name().c_str() );

			BYTE byVal = 0;
			rkPacket.Pop( byVal ); SetAbil(AT_TEAM, static_cast<int>(byVal) );//팀
	
			//INV
			m_kInv.ReadFromPacket(IT_HOME, rkPacket);

			int iClientSaveOption = 0;
			rkPacket.Pop( iClientSaveOption );
			SetAbil(AT_CLIENT_OPTION_SAVE, iClientSaveOption);

			SPlayerBasicInfo kBasic;
			GET_DEF(PgClassDefMgr, kClassDefMgr);
			kClassDefMgr.GetAbil(SClassKey(m_kInfo.iClass, m_kInfo.wLv), kBasic);
			m_kBasic = kBasic;

			m_kCardAbilObj.ReadFromPacket(rkPacket);

			int iValue = 0;
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_TODAYHIT,iValue);
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_TOTALHIT,iValue);
			rkPacket.Pop( iValue ); SetAbil(AT_MYHOME_VISITLOGCOUNT,iValue);

			m_kQuest.Clear();
			CopyAbilFromClassDef();
			CalculateInitAbil();
		}break;
	}

	if (WT_DEFAULT_EXCLUDE & kWriteType)
	{
		m_kGroundKey.ReadFromPacket(rkPacket);
	}

	if(0 == (kWriteType & WT_OP_No_Contents) 
		&& ( WT_OP_MISSION & kWriteType ))
	{
		m_kMissionReport.ReadFromPacket_MissionReport(rkPacket);
	}

	if( kWriteType & WT_OP_DBPlayerState )
	{
		rkPacket.Pop( m_kInfo.byState );
	}

	rkPacket.Pop( m_iUID );

	rkPacket.Pop(m_kOwnerGuid);
	rkPacket.Pop(m_kHomeAddr);
	rkPacket.Pop(m_kIsInSide);
	rkPacket.Pop(m_kOwnerName);
	int iValue = 0;
	rkPacket.Pop(iValue);	SetAbil(AT_MYHOME_VISITFLAG,iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_MYHOME_STATE,iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_HOME_SIDEJOB,iValue);

	Latency(0);
	m_kHackIndex.clear();

	//
	m_kNetModule.Set( kMemberGuid, kSwitchServer );

	return kWriteType;
}


namespace MYHOMEUTIL
{
	__int64 const CalcTex(SMYHOME const & kMyHome)
	{
		__int64 i64PayTime = 0;
		__int64 const i64CurTime = g_kEventView.GetLocalSecTime();
		CGameTime::DBTimeEx2SecTime(BM::DBTIMESTAMP_EX(kMyHome.kPayTexTime),i64PayTime);

		if(i64CurTime < i64PayTime)	// 세금 내야할 기간이 아직 남았다.
		{
			if(MYHOME_PAY_TEX_TIME < (i64PayTime - i64CurTime))	// 일주일 이상 기간이 남았으면 아직은 세금을 낼 수 없다.
			{
				return 0;
			}
		}

		i64PayTime = std::max<__int64>((i64CurTime - i64PayTime),0);

		i64PayTime = std::min<__int64>(MAX_MYHOME_PAY_TEX_TIME,i64PayTime); // 4주 이상 누적되지 않도록 한다.
		
		__int64 i64Tex = ((i64PayTime/MYHOME_PAY_TEX_TIME) + 1) * ((kMyHome.i64HomePrice * MATR_WEEK)/100);

		if(MAX_MYHOME_PAY_TEX_TIME <= i64PayTime)
		{
			CONT_DEFMYHOMEBUILDINGS const * pkCont = NULL;
			g_kTblDataMgr.GetContDef(pkCont);
			if(!pkCont)
			{
				return 0;
			}

			CONT_DEFMYHOMEBUILDINGS::const_iterator iter = pkCont->find(kMyHome.siStreetNo);
			if(iter != pkCont->end())
			{
				if(0 == (*iter).second.iGrade)	// 등급이 0 이면 아파트 이다.
				{
					i64Tex += (kMyHome.i64HomePrice / 2); // 아파트는 차압이 없는 대신 과태료가 븥는다. 50% 과태료
				}
			}
		}

		return i64Tex;
	}


	void MailLog(BM::GUID const & kOwnerGuid,BM::GUID const & kMailGuid,SSendMailInfo const & kMailData,PgContLogMgr & kContLogMgr)
	{
		std::wstring strMail = kMailData.MailTitle() + _T("\r") + kMailData.MailText();
		PgLog kLog(ELOrderMain_Mail,ELOrderSub_Forwarding);
		kLog.Set(0,kMailData.TargetName());
		kLog.Set(1,strMail);
		kLog.Set(0,static_cast<int>(kMailData.PaymentType()));
		kLog.Set(0,kMailData.Money());
		kLog.Set(2,kMailGuid.str());
		kContLogMgr.AddLog(kOwnerGuid,kLog);
	}

	void MakeQuery(SMYHOME const & kMyHome, SMYHOME const & kOldMyHome, CEL::DB_QUERY & kQuery)
	{
		kQuery.PushStrParam(kMyHome.siStreetNo);
		kQuery.PushStrParam(kMyHome.iHouseNo);
		kQuery.PushStrParam(kMyHome.iHomeColor);
		kQuery.PushStrParam(kMyHome.iHomeStyle);
		kQuery.PushStrParam(kMyHome.iHomeFence);
		kQuery.PushStrParam(kMyHome.iHomeGarden);
		kQuery.PushStrParam(kMyHome.iRoomWall);
		kQuery.PushStrParam(kMyHome.iRoomFloor);
		kQuery.PushStrParam(kMyHome.i64HomePrice);
		kQuery.PushStrParam(BM::DBTIMESTAMP_EX(kMyHome.kAuctionTime));
		kQuery.PushStrParam(kMyHome.bAuctionState);
		kQuery.PushStrParam(kMyHome.iGroundNo);
		kQuery.PushStrParam(kMyHome.kOwnerGuid);
		kQuery.PushStrParam(kMyHome.bEnableVisitBit);
		kQuery.PushStrParam(kMyHome.kBidderGuid);
		kQuery.PushStrParam(kMyHome.i64FirstBiddingCost);
		kQuery.PushStrParam(kMyHome.i64LastBiddingCost);
		kQuery.PushStrParam(BM::DBTIMESTAMP_EX(kMyHome.kPayTexTime));
		kQuery.PushStrParam(BM::DBTIMESTAMP_EX(kMyHome.kPayTexNotiTime));
		kQuery.PushStrParam(kMyHome.iTotalVisitLogCount);

		kOldMyHome.WriteToPacket(kQuery.contUserData);
	}
};
