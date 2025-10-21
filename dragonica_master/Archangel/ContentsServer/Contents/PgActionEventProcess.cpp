#include "stdafx.h"
#include "PgActionEventProcess.h"
#include "jobworker.h"
#include "Item/PgPostManager.h"
#include "jobdispatcher.h"

void PgActionEventProcess::DoAction(CONT_EVENT_STATE::mapped_type & kEvent,PgPlayer * pkPlayer)
{
	const TBL_EVENT &kTbl = kEvent.m_kTable;

	switch(kTbl.iEventType)
	{
	case ET_CHANGE_LEVEL:
	case ET_CHANGE_CLASS:
	case ET_NEW_CHARACTER:
		{
			// 이 세가지 경우는 이미 달성 여부를 검사하여 들어온 조건들이라 검사할 필요 없음
			// 바로 메일로 보상 쿠폰을 발송해 준다.
			CEL::DB_QUERY kQuery( DT_MEMBER, DQT_GET_EVENT_COUPON,L"EXEC [dbo].[up_GetEventCoupon]");
			kQuery.InsertQueryTarget(pkPlayer->GetMemberGUID());
			kQuery.QueryOwner(pkPlayer->GetMemberGUID());
			kQuery.PushStrParam(pkPlayer->GetMemberGUID());
			kQuery.PushStrParam(kTbl.aIntData[2]);
			kQuery.PushStrParam(g_kProcessCfg.SiteNo());
			kQuery.PushStrParam(g_kProcessCfg.RealmNo());

			kQuery.contUserData.Push(pkPlayer->GetID());
			kQuery.contUserData.Push(kTbl.aStringData[0]);
			kQuery.contUserData.Push(kTbl.aStringData[1]);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case ET_CONNECT_COUNT:
		{
			int const CONNECT_EVENT_CASHITEM = AT_EVENTFUNC_CONNECT_EVENT11;
			
			switch( kTbl.aIntData[0] )
			{
			case CONNECT_EVENT_CASHITEM:
				{// CashItem 선물함에 발송, 메일 발송
					{// 레벨 조건이 되는지 확인하고
						int const iMinLv = (kTbl.aBigintData[1] & 0xFF0000) / 0xFFFF;
						int const iMaxLv = (kTbl.aBigintData[1] & 0x00FF00) / 0xFF;
						int const iPlayerLv = pkPlayer->GetAbil(AT_LEVEL);
						if(iMinLv > iPlayerLv
							|| iMaxLv < iPlayerLv
							)
						{
							return;
						}
					}
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_EVENT_CASHITEM_GIFT1, L"EXEC [dbo].[UP_Event_CashItemGift1]");
					kQuery.PushStrParam(kTbl.iEventNo); //이벤트 번호
					kQuery.PushStrParam(pkPlayer->GetID());   //선물 받을 CharacterGuid
					kQuery.PushStrParam(kTbl.aStringData[0]); //보낸이
					kQuery.PushStrParam(kTbl.aStringData[1]); //선물내용
					kQuery.PushStrParam(kTbl.aIntData[1]); //선물번호
					kQuery.PushStrParam(static_cast<BYTE>(kTbl.aIntData[2])); //TimeType
					kQuery.PushStrParam(static_cast<int>(kTbl.aBigintData[0])); //UseTime
					kQuery.PushStrParam(static_cast<int>(kTbl.aBigintData[1])); //Option

					kQuery.contUserData.Push(kTbl.iEventNo); //이벤트 번호
					kQuery.contUserData.Push(kTbl.aIntData[1]); //선물번호
					kQuery.contUserData.Push(static_cast<BYTE>(kTbl.aIntData[2])); //TimeType
					kQuery.contUserData.Push(static_cast<int>(kTbl.aBigintData[0])); //UseTime
					kQuery.contUserData.Push(static_cast<int>(kTbl.aBigintData[1])); //Option
					kQuery.contUserData.Push(pkPlayer->GetID()); //받는이
					kQuery.contUserData.Push(kTbl.aStringData[0]); //보낸이
					kQuery.contUserData.Push(kTbl.kTitle); //메일제목
					kQuery.contUserData.Push(kTbl.kDescription); //메일내용

					g_kCoreCenter.PushQuery(kQuery);
				}break;
			case AT_EVENTFUNC_CONNECT_EVENT12:	// 일정 기간 내 한번만 주는 보상(인간 Classlimit 체크)
			case AT_EVENTFUNC_CONNECT_EVENT13:	// 일정 기간 내 한번만 주는 보상(용족 DraClasslimit 체크)
				{// 특정 기간내에 캐시 아이템을 주려 하는데
					int const iEventNo = pkPlayer->GetAbil(kTbl.aIntData[0]);
					if(iEventNo != kTbl.iEventNo)
					{// 아직 받지 않았고(iEventNo가 저장되어있지 않다면 받지 않았다고 가정, iEventNo는 유일한 번호여야함)
						
						{// 레벨 조건이 되는지 확인하고
							int const iMinLv = (kTbl.aBigintData[1] & 0xFF0000) / 0xFFFF;
							int const iMaxLv = (kTbl.aBigintData[1] & 0x00FF00) / 0xFF;
							int const iPlayerLv = pkPlayer->GetAbil(AT_LEVEL);
							if(iMinLv > iPlayerLv
								|| iMaxLv < iPlayerLv
								)
							{
								return;
							}
						}
						{// classLimit(직업)이 되는지 확인하고
							__int64 iTempClassLimit = kTbl.aBigintData[2];
							if(AT_EVENTFUNC_CONNECT_EVENT13 == kTbl.aIntData[0])
							{// 종족에 맞게끔 classLimit를 만들고
								iTempClassLimit = iTempClassLimit << DRAGONIAN_LSHIFT_VAL;
							}
							__int64 const iClassLimit = iTempClassLimit;
							bool const bOk = IS_CLASS_LIMIT( iClassLimit, pkPlayer->GetAbil(AT_CLASS) );
							if( !bOk )
							{
								return;
							}
						}
						{// 선물을 받을수 있다면 CashItem 선물함에 발송, 메일 발송하고
							CEL::DB_QUERY kQuery(DT_PLAYER, DQT_EVENT_CASHITEM_GIFT1, L"EXEC [dbo].[UP_Event_CashItemGift1]");
							kQuery.PushStrParam(kTbl.iEventNo); //이벤트 번호
							kQuery.PushStrParam(pkPlayer->GetID());   //선물 받을 CharacterGuid
							kQuery.PushStrParam(kTbl.aStringData[0]); //보낸이
							kQuery.PushStrParam(kTbl.aStringData[1]); //선물내용
							kQuery.PushStrParam(kTbl.aIntData[1]); //선물번호
							kQuery.PushStrParam(static_cast<BYTE>(kTbl.aIntData[2])); //TimeType
							kQuery.PushStrParam(static_cast<int>(kTbl.aBigintData[0])); //UseTime
							kQuery.PushStrParam(static_cast<int>(kTbl.aBigintData[1])); //Option

							kQuery.contUserData.Push(kTbl.iEventNo); //이벤트 번호
							kQuery.contUserData.Push(kTbl.aIntData[1]); //선물번호
							kQuery.contUserData.Push(static_cast<BYTE>(kTbl.aIntData[2])); //TimeType
							kQuery.contUserData.Push(static_cast<int>(kTbl.aBigintData[0])); //UseTime
							kQuery.contUserData.Push(static_cast<int>(kTbl.aBigintData[1])); //Option
							kQuery.contUserData.Push(pkPlayer->GetID()); //받는이
							kQuery.contUserData.Push(kTbl.aStringData[0]); //보낸이
							kQuery.contUserData.Push(kTbl.kTitle); //메일제목
							kQuery.contUserData.Push(kTbl.kDescription); //메일내용

							g_kCoreCenter.PushQuery(kQuery);
						}

						{// 다시 이벤트 지급을 받지 않게 하기 위해 사용한 EventNo를 저장한다
							CONT_PLAYER_MODIFY_ORDER		kOrder;
							kOrder.push_back( SPMO(IMET_SET_ABIL, pkPlayer->GetID(), SPMOD_SetAbil(kTbl.aIntData[0], kTbl.iEventNo)) );
							SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
							pkActionOrder->InsertTarget(pkPlayer->GetID());
							pkActionOrder->kGndKey.Set(pkPlayer->GetChannel(),pkPlayer->GroundKey());
							pkActionOrder->kCause = CPE_Event;
							pkActionOrder->kContOrder = kOrder;
							g_kJobDispatcher.VPush(pkActionOrder);
						}
					}
				}break;
			default:
				{
					int const iEventNo = pkPlayer->GetAbil(kTbl.aIntData[0]);
					if(iEventNo == kTbl.iEventNo)
					{
						int const iConnectCount = pkPlayer->GetAbil(kTbl.aIntData[1]);

						if((0 == kTbl.aIntData[1]) || (0 == kTbl.aIntData[2]))
						{
							CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("INVALID EVENT DATA : ET_CONNECT_COUNT [") << iEventNo << _T("]"));
							return;
						}

						CONT_PLAYER_MODIFY_ORDER		kOrder;
						kOrder.push_back(SPMO(IMET_ADD_ABIL, pkPlayer->GetID(), SPMOD_AddAbil(kTbl.aIntData[1],1)));

						SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
						pkActionOrder->InsertTarget(pkPlayer->GetID());
						pkActionOrder->kGndKey.Set(pkPlayer->GetChannel(),pkPlayer->GroundKey());
						pkActionOrder->kCause = CPE_Event;
						pkActionOrder->kContOrder = kOrder;
						g_kJobDispatcher.VPush(pkActionOrder);

						if(kTbl.aIntData[2] == iConnectCount)	// 발급 받는 기회는 단 한번뿐이다. 한번에 두번 접속 기록되는 일은 없겠지? ㅡㅡ;;;
						{// 여기서 쿠폰 메일 발송
							CEL::DB_QUERY kQuery( DT_MEMBER, DQT_GET_EVENT_COUPON,L"EXEC [dbo].[up_GetEventCoupon]");
							kQuery.InsertQueryTarget(pkPlayer->GetMemberGUID());
							kQuery.QueryOwner(pkPlayer->GetMemberGUID());
							kQuery.PushStrParam(pkPlayer->GetMemberGUID());
							kQuery.PushStrParam(static_cast<int>(kTbl.aBigintData[0]));
							kQuery.PushStrParam(g_kProcessCfg.SiteNo());
							kQuery.PushStrParam(g_kProcessCfg.RealmNo());

							kQuery.contUserData.Push(pkPlayer->GetID());
							kQuery.contUserData.Push(kTbl.aStringData[0]);
							kQuery.contUserData.Push(kTbl.aStringData[1]);
							g_kCoreCenter.PushQuery(kQuery);
						}
					}
					else
					{
						// 다른 이벤트 기록을 이미 가지고 있다면 그녀석은 여기서 초기화 시켜야 한다.
						//AT_EVENTFUNC_CONNECT_EVENT01,AT_EVENTFUNC_CONNECT_COUNT01 두개 어빌을 각각 현재 이벤트 정보와 카운트 1 으로 리셋 한다.
						CONT_PLAYER_MODIFY_ORDER		kOrder;
						kOrder.push_back(SPMO(IMET_SET_ABIL, pkPlayer->GetID(), SPMOD_AddAbil(kTbl.aIntData[0],kTbl.iEventNo)));
						kOrder.push_back(SPMO(IMET_SET_ABIL, pkPlayer->GetID(), SPMOD_AddAbil(kTbl.aIntData[1],1)));

						SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
						pkActionOrder->InsertTarget(pkPlayer->GetID());
						pkActionOrder->kGndKey.Set(pkPlayer->GetChannel(),pkPlayer->GroundKey());
						pkActionOrder->kCause = CPE_Event;
						pkActionOrder->kContOrder = kOrder;
						g_kJobDispatcher.VPush(pkActionOrder);
					}
				}
			}
		}break;
	default:
		{
			return;
		}break;
	}
}


PgActionCouponEvent::PgActionCouponEvent(E_USER_EVENT_TYPE const kEventType,PgPlayer * pkPlayer)
{
	g_kEventView.CallbackProcessEvent(&m_kEventProcess);
	g_kEventView.ProcessEvent(kEventType,pkPlayer);
}
