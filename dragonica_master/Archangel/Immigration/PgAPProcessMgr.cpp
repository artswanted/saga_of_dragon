#include "stdafx.h"
#include "Lohengrin/PacketStruct_GFAP.h"
#include "PgAPProcessMgr.h"
#include "PgWaitingLobby.h"

PgAPProcessMgr::PgAPProcessMgr(void)
{

}

PgAPProcessMgr::~PgAPProcessMgr(void)
{

}

HRESULT PgAPProcessMgr::ProcessOrder( BM::Stream &rkPacket )
{
	using namespace GFAP_FUNCTION;

	size_t const iRDPos = rkPacket.RdPos();

	int iOrderType = GFAP_F_NONE;
	rkPacket.Pop( iOrderType );

	switch ( iOrderType )
	{

	case GFAP_F_OVERLAP_ACCOUNT:
		{// 계정체크	
			S_GFAP_F_OVERLAP_ACCOUNT kOrder;
			if ( true == kOrder.ReadFromPacket( rkPacket ) )
			{
				CEL::DB_QUERY kQuery(DT_MEMBER, DQT_AP_CHECK_ACCOUNT, L"EXEC [dbo].[Up_SelectMemberID]" );
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam(kOrder.wstrID);
				
				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );
				
				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
			}
		}break;
	case GFAP_F_CREATE_ACCOUNT:
		{// 계정생성

			S_GFAP_CREATE_ACCOUNT kOrder;
			if ( true == kOrder.ReadFromPacket( rkPacket ) )
			{
				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_AP_CREATE_ACCOUNT, _T("EXEC [dbo].[UP_CreateMemberAccount_GF]"));
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam( kOrder.wstrID );
				kQuery.PushStrParam( kOrder.wstrPW );
				kQuery.PushStrParam( kOrder.cGender);// Gender
				kQuery.PushStrParam( kOrder.kBirthDay );
				kQuery.PushStrParam( kOrder.byUseMobileLock );// Use Mobile Lock

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);
			}
			
		}break;
	case GFAP_F_CHECK_ACCOUNTPW:
		{// 계정비밀번호+Password

			S_GFAP_F_CHECK_ACCOUNTPW kOrder;
			if ( true == kOrder.ReadFromPacket( rkPacket ) )
			{
				CEL::DB_QUERY kQuery(DT_MEMBER, DQT_AP_CHECK_ACCOUNT, L"EXEC [dbo].[up_SelectMemberID]" );
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam(kOrder.wstrID);

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
			}
		}break;
	case GFAP_F_GET_CASH:
		{// 캐쉬 검색

			S_GFAP_F_OVERLAP_ACCOUNT kOrder;
			if ( true == kOrder.ReadFromPacket( rkPacket ) )
			{
				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_AP_GET_CASH, L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH_BYID]");
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam(kOrder.wstrID);

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
			}
		}break;
	case GFAP_F_ADD_CASH:
		{// 캐쉬 추가

			S_GFAP_F_ADD_CASH kOrder;
			if ( true == kOrder.ReadFromPacket( rkPacket ) )
			{
				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_AP_ADD_CASH, L"EXEC [dbo].[UP_CS_AddMemberCash_ByID]");
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam(kOrder.wstrID);
				kQuery.PushStrParam(BM::GUID::NullData());
				kQuery.PushStrParam(kOrder.i64AddCashValue);
				kQuery.PushStrParam(0i64);//마일리지
				kQuery.PushStrParam(0i64);//보너스 마일리지
				kQuery.PushStrParam(static_cast<BYTE>(MCT_AP_SERVER));
				kQuery.PushStrParam(static_cast<BYTE>(1));

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
			}
		}break;
	case GFAP_F_MODIFY_PASSWORD:
		{// 패스워드 수정

			S_GFAP_F_MODIFY_PASSWORD kOrder;
			if ( true == kOrder.ReadFromPacket( rkPacket ) )
			{
				CEL::DB_QUERY kQuery(DT_MEMBER, DQT_AP_CHECK_ACCOUNT, L"EXEC [dbo].[up_SelectMemberID]" );
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam(kOrder.wstrID);

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
			}
		}break;
	case GFAP_F_USE_COUPON:
		{// 쿠폰 사용

			S_GFAP_F_USE_COUPON kOrder;
			if ( true == kOrder.ReadFromPacket( rkPacket ) )
			{
				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_AP_CREATE_COUPON, L"EXEC [dbo].[up_CreateCoupon_GF]" );
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam(kOrder.wstrID);
				kQuery.PushStrParam(kOrder.wstrCouponID);
				kQuery.PushStrParam(kOrder.iEventKey);
				kQuery.PushStrParam(kOrder.iRewardKey);
				kQuery.PushStrParam(kOrder.nRealmNo);

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
			}
		}break;
	case GFAP_F_MODIFY_MOBILELOCK:
		{// 모바일락 사용 여부 수정

			S_GFAP_F_MODIFY_MOBILELOCK kOrder;
			if ( true == kOrder.ReadFromPacket( rkPacket ) )
			{
				CEL::DB_QUERY kQuery(DT_MEMBER, DQT_AP_MODIFY_MOBILELOCK, L"EXEC [dbo].[up_ChangeMemberSub_ExtVar_ByID]" );
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam(kOrder.wstrID);
				kQuery.PushStrParam(static_cast<short>(kOrder.byUseMobileLock));

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
/*
				CEL::DB_QUERY kQuery(DT_MEMBER, DQT_AP_CHECK_ACCOUNT, L"EXEC [dbo].[up_SelectMemberID]" );
				kQuery.QueryOwner(kOrder.kReqServerSessionID);
				kQuery.InsertQueryTarget(kOrder.kOrderID);

				kQuery.PushStrParam(kOrder.wstrID);

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Swap( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
*/
			}
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Unknow OrderType<") << iOrderType << _T(">") );
		}break;

	}
	return S_OK;
}

void PgAPProcessMgr::ProcessPT_GF_AP_REQ_SET_TABLEDATA( BM::Stream * const pkPacket )
{
	using namespace GFAP_FUNCTION;

	BM::GUID kRequestID;
	BYTE kTableType = 0;
	BYTE kFuncType = 0;
	int iKey = 0;
	pkPacket->Pop( kRequestID );
	pkPacket->Pop( kTableType );
	pkPacket->Pop( kFuncType );
	pkPacket->Pop(iKey);

	int iRet = GFAP_RET_SUCCESS;

	switch( kFuncType )
	{
	case GFAP_T_CON_INSERT:
	case GFAP_T_CON_UPDATE:
		{
			CEL::DB_QUERY kQuery;

			switch( kTableType )
			{
			case GFAP_T_EVENT_TYPE:
				{
					kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_AP_TABLE_CONTROL, L"EXEC [dbo].[up_GFAP_Create_EventType]");
					
					DBTIMESTAMP kStartDate;
					DBTIMESTAMP kEndDate;
					std::wstring wstrMemo;
					std::wstring wstrTitle;
					std::wstring wstrDiscription;
					pkPacket->Pop( kStartDate );
					pkPacket->Pop( kEndDate );
					pkPacket->Pop( wstrMemo );
					pkPacket->Pop( wstrTitle );
					pkPacket->Pop( wstrDiscription );

					if ( pkPacket->RemainSize() )
					{
						CAUTION_LOG( BM::LOG_LV5, _T("[PT_GF_AP_REQ_SET_TABLEDATA] Table3 Error Packet Size RequestID<") << kRequestID << _T(">"));
						iRet = GFAP_RET_VALUEERROR;
					}
					else
					{
						kQuery.PushStrParam( iKey );
						kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kStartDate) );
						kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kEndDate) );
						kQuery.PushStrParam( wstrMemo );
						kQuery.PushStrParam( wstrTitle );

						kQuery.PushStrParam( wstrDiscription );
					}

					INFO_LOG(BM::LOG_LV5, __FL__ << _T("GFAP_T_EVENT_TYPE : StartDate<") << kStartDate << _T("> EndDate<") << kEndDate
						<< _T("> Memo<") << wstrMemo << _T("> Title<") << wstrTitle << _T("> Discription<") << wstrDiscription << _T(">"));
				}break;
			case GFAP_T_EVENT_REWARD:
				{
					kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_AP_TABLE_CONTROL, L"EXEC [dbo].[up_GFAP_Create_EventReward]");

					int iRewardItemNo[4] = {0,};
					short nRewardItemCount[4] = {0,};
					char cRewardTimeType[4] = {0,};
					short nRewardUseTime[4] = {0,};
					int iMoney = 0;
					int iCash = 0;
					std::wstring wstrTitle;
					pkPacket->PopMemory( iRewardItemNo, sizeof(iRewardItemNo) );
					pkPacket->PopMemory( nRewardItemCount, sizeof(nRewardItemCount) );
					pkPacket->PopMemory( cRewardTimeType, sizeof(cRewardTimeType) );
					pkPacket->PopMemory( nRewardUseTime, sizeof(nRewardUseTime) );
					pkPacket->Pop( iMoney );
					pkPacket->Pop( iCash );
					pkPacket->Pop( wstrTitle );

					if ( pkPacket->RemainSize() )
					{
						CAUTION_LOG( BM::LOG_LV5, _T("[PT_GF_AP_REQ_SET_TABLEDATA] Table2 Error Packet Size RequestID<") << kRequestID << _T(">"));
						iRet = GFAP_RET_VALUEERROR;
					}
					else
					{
						kQuery.PushStrParam( iKey );
						for ( int i = 0; i < 4 ; ++i )
						{
							kQuery.PushStrParam( iRewardItemNo[i] );
							kQuery.PushStrParam( nRewardItemCount[i] );
							kQuery.PushStrParam( cRewardTimeType[i] );
							kQuery.PushStrParam( nRewardUseTime[i] );
						}
						kQuery.PushStrParam( iMoney );
						kQuery.PushStrParam( iCash );
						kQuery.PushStrParam( wstrTitle );
					}
				}break;
			case GFAP_T_EVENT_TYPE_REWARD:
				{
					kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_AP_TABLE_CONTROL, L"EXEC [dbo].[up_GFAP_Create_EventTypeReward]");

					int iRewardItem[10] = {0,};
					std::wstring wstrRewardTitle;
					pkPacket->PopMemory( iRewardItem, sizeof(iRewardItem) );
					pkPacket->Pop(wstrRewardTitle);

					if ( pkPacket->RemainSize() )
					{
						CAUTION_LOG( BM::LOG_LV5, _T("[PT_GF_AP_REQ_SET_TABLEDATA] Table3 Error Packet Size RequestID<") << kRequestID << _T(">"));
						iRet = GFAP_RET_VALUEERROR;
					}
					else
					{
						kQuery.PushStrParam( iKey );
						kQuery.PushStrParam(wstrRewardTitle);
						for ( int i = 0; i < 10 ; ++i )
						{
							kQuery.PushStrParam( iRewardItem[i] );
						}
					}
				}break;
			default:
				{
					CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Unknown TableType<") << kTableType << _T("> RequestID<") << kRequestID << _T(">") ); 
					iRet = GFAP_RET_VALUEERROR;
				}break;
			}

			if ( GFAP_RET_SUCCESS == iRet )
			{
				kQuery.PushStrParam( kFuncType );
				kQuery.QueryOwner( kRequestID );
				kQuery.InsertQueryTarget( kRequestID );
				if ( S_OK != g_kCoreCenter.PushQuery(kQuery) )
				{
					iRet = GFAP_RET_DBERROR;
				}
			}
		}break;
	case GFAP_T_CON_DELETE:
		{
			std::wstring wstrQuery;
			switch( kTableType )
			{
			case GFAP_T_EVENT_TYPE:
				{
					wstrQuery = L"EXEC [dbo].[up_GFAP_Delete_EventType]";
				}break;
			case GFAP_T_EVENT_REWARD:
				{
					wstrQuery = L"EXEC [dbo].[up_GFAP_Delete_EventReward]";
				}break;
			case GFAP_T_EVENT_TYPE_REWARD:
				{
					wstrQuery = L"EXEC [dbo].[up_GFAP_Delete_EventTypeReward]";
				}break;
			default:
				{
					CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Unknown TableType<") << kTableType << _T("> RequestID<") << kRequestID << _T(">") ); 
					iRet = GFAP_RET_VALUEERROR;
				}break;
			}

			if ( GFAP_RET_SUCCESS == iRet )
			{
				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_AP_TABLE_CONTROL, wstrQuery );
				kQuery.QueryOwner( kRequestID );
				kQuery.InsertQueryTarget( kRequestID );

				kQuery.PushStrParam(iKey);
				if ( S_OK != g_kCoreCenter.PushQuery(kQuery) )
				{
					iRet = GFAP_RET_DBERROR;
				}
			}
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Unknown FuncType<") << kFuncType << _T("> RequestID<") << kRequestID << _T(">") );
			iRet = GFAP_RET_VALUEERROR;
		}break;
	}

	if ( GFAP_RET_SUCCESS != iRet )
	{
		BM::Stream kFailedPacket( PT_AP_GF_ANS_SET_TABLEDATA, kRequestID );
		kFailedPacket.Push( iRet );
		Send( kFailedPacket );
	}
}

void PgAPProcessMgr::Q_DQT_AP_CHECK_ACCOUNT( CEL::DB_RESULT &rkResult )const
{
	using namespace GFAP_FUNCTION;

	BM::Stream &rkPacket = rkResult.contUserData;
	rkPacket.RdPos( sizeof(BM::Stream::DEF_STREAM_TYPE) );
	size_t const iRDPos = rkPacket.RdPos();

	int iOrderType = GFAP_F_NONE;
	S_GFAP_F_BASE kOrderInfo;
	rkPacket.Pop( iOrderType );
	kOrderInfo.ReadFromPacket( rkPacket );

	S_GFAP_F_OVERLAP_ACCOUNT_REPLY kReplyData;
	rkPacket.Pop( kReplyData.wstrID );

	std::wstring kID;
	CEL::DB_BINARY vtPW;

	kReplyData.iRet = GFAP_RET_ID;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor )
			{	
				itor->Pop( kReplyData.iRet );	++itor;

				if ( rkResult.vecArray.end() != itor )
				{
					itor->Pop( kID );		++itor;
					itor->Pop( vtPW );		++itor;
					kReplyData.iRet =		GFAP_RET_SUCCESS;
				}
			}
		}break;
	default:
		{
			kReplyData.iRet = GFAP_RET_DBERROR;
		}break;
	}

	switch( iOrderType )
	{
	case GFAP_F_OVERLAP_ACCOUNT:
		{
			if ( kID.size() )
			{
				kReplyData.iRet = GFAP_RET_ID;
			}
			else
			{
				kReplyData.iRet = GFAP_RET_SUCCESS;
			}
		}break;
	case GFAP_F_CHECK_ACCOUNTPW:
	case GFAP_F_MODIFY_PASSWORD:
	case GFAP_F_MODIFY_MOBILELOCK:
		{
			std::wstring wstrReqPW;
			rkPacket.Pop( wstrReqPW );

			if ( GFAP_RET_SUCCESS == kReplyData.iRet )
			{
				BM::vstring vSQL;
				std::wstring strQueryID, strQueryPW;
				CEL::DB_QUERY::StrConvForQuery(kID, strQueryID);
				CEL::DB_QUERY::StrConvForQuery(wstrReqPW, strQueryPW);
				vSQL = vSQL << _T("SELECT CONVERT(binary(60), HashBytes('MD5', REVERSE(UPPER('") << strQueryID << _T("')) + N'") << strQueryPW << _T("'))");
				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_TRY_AUTH_CHECKPW_AP, vSQL.operator std::wstring const&() );
				kQuery.QueryOwner(rkResult.QueryOwner());

				kQuery.contUserData.Push( vtPW );
				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Push( rkPacket );
				
				if ( S_OK == g_kCoreCenter.PushQuery( kQuery ) )
				{
					return;//return
				}
				kReplyData.iRet = GFAP_RET_DBERROR;
			}

			switch( iOrderType )
			{
			case GFAP_F_MODIFY_PASSWORD:
				{
					S_GFAP_F_MODIFY_PASSWORD_REPLY kReplyPWData;
					kReplyPWData.wstrID = kReplyData.wstrID;
					kReplyPWData.wstrNowPW = wstrReqPW;
					kReplyPWData.iRet = kReplyData.iRet;
					this->SendToResult( iOrderType, kOrderInfo, kReplyPWData );;
				}break;
			case GFAP_F_MODIFY_MOBILELOCK:
				{
					S_GFAP_F_MODIFY_MOBILELOCK_REPLY kReplyMLData;
					kReplyMLData.wstrID = kReplyData.wstrID;
					kReplyMLData.iRet = kReplyData.iRet;
					rkPacket.Pop( kReplyMLData.byUseMobileLock );
					this->SendToResult( iOrderType, kOrderInfo, kReplyMLData );
				}break;
			default:
				{

				}break;
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T("Unknown OrderType<") << iOrderType << _T(">") );
			return;
		}break;
	}

	this->SendToResult( iOrderType, kOrderInfo, kReplyData );
}

void PgAPProcessMgr::Q_DQT_AP_CREATE_ACCOUNT( CEL::DB_RESULT &rkResult )const
{
	using namespace GFAP_FUNCTION;

	BM::Stream &rkPacket = rkResult.contUserData;
	rkPacket.RdPos( sizeof(BM::Stream::DEF_STREAM_TYPE) );

	int iOrderType = GFAP_F_NONE;
	S_GFAP_F_BASE kOrderInfo;
	rkPacket.Pop( iOrderType );
	kOrderInfo.ReadFromPacket( rkPacket );

	S_GFAP_F_OVERLAP_ACCOUNT_REPLY kReplyData;
	rkPacket.Pop( kReplyData.wstrID );
	kReplyData.iRet = GFAP_RET_DBERROR;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			int iResult = 0;
			BM::GUID kMemberGuid;
			std::wstring kID;
			BYTE byGender = 0;
			std::wstring kBrithday;
			int iUID = 0;

			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor )
			{
				itor->Pop(iResult);		++itor;
				itor->Pop(kMemberGuid);	++itor;
				itor->Pop(kID);			++itor;
				itor->Pop(byGender);	++itor;
				itor->Pop(kBrithday);	++itor;
				itor->Pop(iUID);		++itor;

				switch(iResult)
				{
				case 0:{kReplyData.iRet = GFAP_RET_SUCCESS;}break;
				case 99:{kReplyData.iRet = GFAP_RET_ID;}break;
				}
			}
		}break;
	default:
		{
		}break;
	}

	this->SendToResult( iOrderType, kOrderInfo, kReplyData );
}

void PgAPProcessMgr::Q_DQT_AP_GET_CASH( CEL::DB_RESULT &rkResult )const
{
	using namespace GFAP_FUNCTION;

	BM::Stream &rkPacket = rkResult.contUserData;
	rkPacket.RdPos( sizeof(BM::Stream::DEF_STREAM_TYPE) );

	int iOrderType = GFAP_F_NONE;
	S_GFAP_F_BASE kOrderInfo;
	rkPacket.Pop( iOrderType );
	kOrderInfo.ReadFromPacket( rkPacket );

	S_GFAP_F_GET_CASH_REPLY kReplyData;
	rkPacket.Pop( kReplyData.wstrID );
	kReplyData.iRet = GFAP_RET_ID;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor )
			{
				__int64 i64NowBonus = 0i64;
				itor->Pop(kReplyData.wstrID);		++itor;
				itor->Pop(kReplyData.i64NowCash);	++itor;
				itor->Pop(i64NowBonus);				++itor;
				kReplyData.iRet = GFAP_RET_SUCCESS;
			}
		}break;
	default:
		{

		}break;
	}

	this->SendToResult( iOrderType, kOrderInfo, kReplyData );
}

void PgAPProcessMgr::Q_DQT_AP_ADD_CASH( CEL::DB_RESULT &rkResult )const
{
	using namespace GFAP_FUNCTION;

	BM::Stream &rkPacket = rkResult.contUserData;
	rkPacket.RdPos( sizeof(BM::Stream::DEF_STREAM_TYPE) );

	int iOrderType = GFAP_F_NONE;
	S_GFAP_F_BASE kOrderInfo;
	rkPacket.Pop( iOrderType );
	kOrderInfo.ReadFromPacket( rkPacket );

	S_GFAP_F_ADD_CASH_REPLY kReplyData;
	rkPacket.Pop( kReplyData.wstrID );
	kReplyData.iRet = GFAP_RET_ID;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor )
			{
				__int64 i64AddValue = 0i64;
				__int64 i64Bonus = 0i64;
				itor->Pop(kReplyData.iRet);			++itor;
				itor->Pop(kReplyData.wstrID);		++itor;
				itor->Pop(kReplyData.i64OldCash);	++itor;
				itor->Pop(kReplyData.i64NowCash);	++itor;
				itor->Pop(i64AddValue);				++itor;
				itor->Pop(i64Bonus);				++itor;//Old Bonus
				itor->Pop(i64Bonus);				++itor;//New Bonus
				itor->Pop(i64Bonus);				++itor;//Add Bonus

				if ( kReplyData.iRet )
				{
					kReplyData.iRet = GFAP_RET_ID;
				}
			}
		}break;
	default:
		{
			kReplyData.iRet = GFAP_RET_DBERROR;
		}break;
	}

	this->SendToResult( iOrderType, kOrderInfo, kReplyData );
}

void PgAPProcessMgr::Q_DQT_AP_MODIFY_PASSWORD( CEL::DB_RESULT &rkResult )const
{
	using namespace GFAP_FUNCTION;

	BM::Stream &rkPacket = rkResult.contUserData;
//	rkPacket.RdPos( sizeof(BM::Stream::DEF_STREAM_TYPE) );

	int iOrderType = GFAP_F_NONE;
	S_GFAP_F_BASE kOrderInfo;
	rkPacket.Pop( iOrderType );
	kOrderInfo.ReadFromPacket( rkPacket );

	S_GFAP_F_MODIFY_PASSWORD_REPLY kReplyData;
	rkPacket.Pop( kReplyData.wstrID );
	rkPacket.Pop( kReplyData.wstrNowPW );
	kReplyData.iRet = GFAP_RET_DBERROR;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			int iResult = 0;
			std::wstring kID;

			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor )
			{
				itor->Pop(iResult);		++itor;
				itor->Pop(kID);			++itor;

				switch(iResult)
				{
				case 0:{kReplyData.iRet = GFAP_RET_SUCCESS;}break;
				case 99:{kReplyData.iRet = GFAP_RET_ID;}break;
				}
			}
		}break;
	default:
		{

		}break;
	}

	if ( GFAP_RET_SUCCESS == kReplyData.iRet )
	{
		rkPacket.Pop( kReplyData.wstrNowPW );
	}

	this->SendToResult( iOrderType, kOrderInfo, kReplyData );
}

void PgAPProcessMgr::Q_DQT_AP_CREATE_COUPON( CEL::DB_RESULT &rkResult )const
{
	using namespace GFAP_FUNCTION;

	BM::Stream &rkPacket = rkResult.contUserData;
	rkPacket.RdPos( sizeof(BM::Stream::DEF_STREAM_TYPE) );

	int iOrderType = GFAP_F_NONE;
	S_GFAP_F_BASE kOrderInfo;
	rkPacket.Pop( iOrderType );
	kOrderInfo.ReadFromPacket( rkPacket );

	S_GFAP_F_USE_COUPON_REPLY kReplyData;
	rkPacket.Pop( kReplyData.wstrID );
	rkPacket.Pop( kReplyData.wstrCouponID );
	kReplyData.iRet = GFAP_RET_DBERROR;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor )
			{
				itor->Pop(kReplyData.iRet);			++itor;
				itor->Pop(kReplyData.wstrCouponID);	++itor;
			}
		}break;
	default:
		{

		}break;
	}

	this->SendToResult( iOrderType, kOrderInfo, kReplyData );
}

void PgAPProcessMgr::Q_DQT_AP_MODIFY_MOBILELOCK( CEL::DB_RESULT &rkResult )const
{
	using namespace GFAP_FUNCTION;

	BM::Stream &rkPacket = rkResult.contUserData;
	rkPacket.RdPos( sizeof(BM::Stream::DEF_STREAM_TYPE) );

	int iOrderType = GFAP_F_NONE;
	S_GFAP_F_BASE kOrderInfo;
	rkPacket.Pop( iOrderType );
	kOrderInfo.ReadFromPacket( rkPacket );

	std::wstring wstrPW;
	S_GFAP_F_MODIFY_MOBILELOCK_REPLY kReplyData;
	rkPacket.Pop( kReplyData.wstrID );
	rkPacket.Pop( wstrPW );
	rkPacket.Pop( kReplyData.byUseMobileLock );
	kReplyData.iRet = GFAP_RET_DBERROR;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			int iResult = 0;

			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor )
			{
				itor->Pop(iResult);		++itor;

				switch(iResult)
				{
				case 0:{kReplyData.iRet = GFAP_RET_SUCCESS;}break;
				case 99:{kReplyData.iRet = GFAP_RET_ID;}break;
				}
			}
		}break;
	default:
		{

		}break;
	}

	this->SendToResult( iOrderType, kOrderInfo, kReplyData );
}

void PgAPProcessMgr::Q_DQT_AP_TABLE_CONTROL( CEL::DB_RESULT &rkResult )const
{
	using namespace GFAP_FUNCTION;
	int iRet = GFAP_RET_SUCCESS;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itr )
			{
				itr->Pop(iRet);			++itr;
			}
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV2, __FL__ << _T("Error = ") << rkResult.eRet);
			iRet = GFAP_RET_DBERROR;
		}break;
	}

	BM::Stream kAnsPacket( PT_AP_GF_ANS_SET_TABLEDATA, rkResult.QueryOwner() );
	kAnsPacket.Push( iRet );
	Send( kAnsPacket );
}

void PgAPProcessMgr::HandleMessage(SEventMessage *pkMsg)
{
	EAP_TASK_TYPE const eSecType = static_cast<EAP_TASK_TYPE const>(pkMsg->SecType());
	switch (eSecType)
	{
	case EAP_2ND_PACKET:
		{
			PACKET_ID_TYPE wType;
			pkMsg->Pop(wType);
			switch (wType)
			{
			case PT_AP_IM_REQ_ORDER:
				{
					ProcessOrder(*(dynamic_cast<BM::Stream*>(pkMsg)));
				}break;
			case PT_GF_AP_REQ_SET_TABLEDATA:
				{
					ProcessPT_GF_AP_REQ_SET_TABLEDATA( pkMsg );
				}break;
			default:
				{
					INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown packet type=") << wType);
					CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Unknown packet type=") << wType);
				}break;
			}
		}break;
	case EAP_2ND_DBRESULT:
		{
			CEL::DB_QUERY kTemp;
			CEL::DB_RESULT kResult(kTemp);
			kResult.ReadFromPacket(*pkMsg);
			switch(kResult.QueryType())
			{
			case DQT_AP_CHECK_ACCOUNT:
				{
					Q_DQT_AP_CHECK_ACCOUNT( kResult );
				}break;
			case DQT_AP_CREATE_ACCOUNT:
				{
					Q_DQT_AP_CREATE_ACCOUNT(kResult);
				}break;
			case DQT_AP_GET_CASH:
				{
					Q_DQT_AP_GET_CASH(kResult);
				}break;
			case DQT_AP_ADD_CASH:
				{
					Q_DQT_AP_ADD_CASH(kResult);
				}break;
			case DQT_AP_MODIFY_PASSWORD:
				{
					Q_DQT_AP_MODIFY_PASSWORD(kResult);
				}break;
			case DQT_AP_CREATE_COUPON:
				{
					Q_DQT_AP_CREATE_COUPON(kResult);
				}break;
			case DQT_AP_MODIFY_MOBILELOCK:
				{
					Q_DQT_AP_MODIFY_MOBILELOCK(kResult);
				}break;
			case DQT_AP_TABLE_CONTROL:
				{
					Q_DQT_AP_TABLE_CONTROL(kResult);
				}break;
			case DQT_TRY_AUTH_CHECKPW_AP:
				{
					Q_DQT_TRY_AUTH_CHECKPW_AP( kResult );
				}break;
			default:
				{
					INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown QueryType=") << kResult.QueryType());
					CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unknown QueryType=") << kResult.QueryType());
				}break;
			}
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown Event type=") << eSecType);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Unknown Event type=") << eSecType);
		}break;
	}
}

void PgAPProcessMgr::Q_DQT_TRY_AUTH_CHECKPW_AP( CEL::DB_RESULT &rkResult )const
{	
	using namespace GFAP_FUNCTION;

	BM::Stream &rkPacket = rkResult.contUserData;
	
	CEL::DB_BINARY vtMyPW;
	rkPacket.Pop( vtMyPW );

	size_t const iRDPos = rkPacket.RdPos();

	int iOrderType = GFAP_F_NONE;
	S_GFAP_F_BASE kOrderInfo;
	rkPacket.Pop( iOrderType );
	kOrderInfo.ReadFromPacket( rkPacket );

	S_GFAP_F_OVERLAP_ACCOUNT_REPLY kReplyData;
	rkPacket.Pop( kReplyData.wstrID );
	
	std::wstring wstrReqPW;
	rkPacket.Pop( wstrReqPW );

	kReplyData.iRet = GFAP_RET_PW;

	if ( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if( rkResult.vecArray.end() != itor)
		{
			CEL::DB_BINARY vtPw;
			(*itor).Pop(vtPw);

			if ( vtMyPW.size() == vtPw.size() && ( 0 == ::memcmp(&vtMyPW.at(0), &vtPw.at(0), vtPw.size())) )
			{
				kReplyData.iRet = GFAP_RET_SUCCESS;
			}
		}
	}

	switch( iOrderType )
	{
	case GFAP_F_CHECK_ACCOUNTPW:
		{
			this->SendToResult( iOrderType, kOrderInfo, kReplyData );
		}break;
	case GFAP_F_MODIFY_PASSWORD:
		{
			S_GFAP_F_MODIFY_PASSWORD_REPLY kReplyPWData;
			kReplyPWData.wstrID = kReplyData.wstrID;
			kReplyPWData.wstrNowPW = wstrReqPW;
			kReplyPWData.iRet = kReplyData.iRet;

			if ( GFAP_RET_SUCCESS == kReplyPWData.iRet )
			{
				std::wstring wstrNewPW;
				rkPacket.Pop( wstrNewPW );

				if ( wstrNewPW.size() )
				{
					// 성공이면 이제 진짜 쿼리를 날려 패스워드를 변경하자
					CEL::DB_QUERY kQuery(DT_MEMBER, DQT_AP_MODIFY_PASSWORD, L"EXEC [dbo].[up_UpdateMemberPWByID]" );
					kQuery.QueryOwner(rkResult.QueryOwner());
					kQuery.InsertQueryTarget(kOrderInfo.kOrderID);

					kQuery.PushStrParam(kReplyPWData.wstrID);
					kQuery.PushStrParam(wstrNewPW);

					rkPacket.RdPos( iRDPos );
					kQuery.contUserData.Push( rkPacket );

					g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
					return;// return
				}

				kReplyPWData.iRet = GFAP_RET_PW_STRING_ERROR;
			}

			this->SendToResult( iOrderType, kOrderInfo, kReplyPWData );
		}break;
	case GFAP_F_MODIFY_MOBILELOCK:
		{
			// 이거 지금 사용하지 않음...!!!!!!

			S_GFAP_F_MODIFY_MOBILELOCK_REPLY kReplyMLData;
			kReplyMLData.wstrID = kReplyData.wstrID;
			kReplyMLData.iRet = kReplyData.iRet;

			rkPacket.Pop( kReplyMLData.byUseMobileLock );

			if ( GFAP_RET_SUCCESS == kReplyMLData.iRet )
			{
				// 성공이면 이제 진짜 쿼리를 날려 모바일락을 변경하자
				CEL::DB_QUERY kQuery(DT_MEMBER, DQT_AP_MODIFY_MOBILELOCK, L"EXEC [dbo].[up_ChangeMemberSub_ExtVar_ByID]" );
				kQuery.QueryOwner(rkResult.QueryOwner());
				kQuery.InsertQueryTarget(kOrderInfo.kOrderID);

				kQuery.PushStrParam(kReplyData.wstrID);
				kQuery.PushStrParam(static_cast<short>(kReplyMLData.byUseMobileLock));

				rkPacket.RdPos( iRDPos );
				kQuery.contUserData.Push( static_cast<BM::Stream::DEF_STREAM_TYPE>(0) );//혹시몰라
				kQuery.contUserData.Push( rkPacket );

				g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
				return;// return
			}

			this->SendToResult( iOrderType, kOrderInfo, kReplyMLData );
		}break;
	}
}
