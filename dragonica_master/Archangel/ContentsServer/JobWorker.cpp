#include "StdAfx.h"
#include "BM/TImeCheck.h"
#include "JobWorker.h"
#include "Item/PgPostManager.h"
#include "Item/CashManager.h"
#include "PgGMProcessMgr.h"

PgJobWorker::ActionJobPool PgJobWorker::ms_kJobPool(100, 50);

PgJobWorker::PgJobWorker()
{
}

PgJobWorker::~PgJobWorker()
{
}

SActionOrder* PgJobWorker::AllocJob()
{
	SActionOrder* pkNew = ms_kJobPool.New();
	if (pkNew != NULL)
	{
		pkNew->Clear();
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Cannot alloc new job") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	return pkNew;
}

void PgJobWorker::Free( SActionOrder *& pkJob )
{
	ms_kJobPool.Delete(pkJob);
}

void PgJobWorker::FreeJob( SActionOrder *& pkJob )
{
	ms_kJobPool.Delete(pkJob);
}

HRESULT PgJobWorker::VProcess(SActionOrder *pkJob)
{
	PgTimeCheck kTimeCheck( dynamic_cast<PgLogWorker_Base*>(&g_kLogWorker), LT_S_STATE, _T("JOB WORK"), pkJob->kCause, BM::vstring(500) );//1초 넘으면. 로그 찍음.

	switch (pkJob->kCause)
	{
	case CNE_POST_SYSTEM_MAIL_ORDER:
		{
			std::wstring	kFromName,
							kToName,
							kTitle,
							kText;

			int iItemNo = 0;
			short iItemCount = 0;
			int iMoney = 0;

			BM::GUID kCmdGuid;

			BM::Stream & kPacket = pkJob->kPacket2nd;
			kPacket.Pop(kFromName);
			kPacket.Pop(kToName);
			kPacket.Pop(kTitle);
			kPacket.Pop(kText);
			kPacket.Pop(iItemNo);
			kPacket.Pop(iItemCount);
			kPacket.Pop(iMoney);
			kPacket.Pop(kCmdGuid);

			bool const bRet = g_kPostMgr.PostSystemMail(BM::GUID::NullData(), kFromName, kToName, kTitle, kText, iItemNo, iItemCount, iMoney, kCmdGuid);
			if(false == bRet)
			{
				g_kGMProcessMgr.SendOrderFailed(kCmdGuid, GE_SYSTEM_ERR);
			}
		}break;
	case CNE_POST_SYSTEM_MAIL:
		{
			BM::GUID		kCharGuid;
			std::wstring	kToName,
							kTitle,
							kText,
							kFrom;

			int iItemNo = 0;
			short iItemCount = 0;
			int iMoney = 0;

			BM::Stream & kPacket = pkJob->kPacket2nd;
			kPacket.Pop(kCharGuid);
			kPacket.Pop(kToName);
			kPacket.Pop(kTitle);
			kPacket.Pop(kText);
			kPacket.Pop(iItemNo);
			kPacket.Pop(iItemCount);
			kPacket.Pop(iMoney);
			kPacket.Pop(kFrom);

			g_kPostMgr.PostSystemMailByGuid(kCharGuid, kFrom, kTitle, kText, iItemNo, iItemCount, iMoney);
		}break;
	case CNE_CONTENTS_EVENT:
		{
			g_kRealmUserMgr.Locked_ContentsEvent(pkJob);
		}break;
	case CIE_MissionUpdate:
		{
			g_kRealmUserMgr.Locked_UpdateMissionReport(pkJob);
		}break;
	case CIE_LOAD_PET_ITEM:
		{
			g_kRealmUserMgr.Locked_CIE_LOAD_PET_ITEM(pkJob->kAddonPacket);
		}break;
	case CNE_CONTENTS_SENDTOUSER:
		{
			if (pkJob->kContOrder.empty())
			{
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("CNE_CONTENTS_SENDTOUSER, ContOrder is empty"));

			}
			else
			{
				SPMO& kSPMO = *(pkJob->kContOrder.begin());
				ContentsActionEvent_SendPacket kEvent;
				kSPMO.Read(kEvent);
				switch (kEvent.SendType())
				{
				case ContentsActionEvent_SendPacket::E_SendUser_ToOneUser:
					{
						g_kRealmUserMgr.Locked_SendToUser(kEvent.Guid(), kEvent.Packet(), kEvent.MemberGuid());
					}break;
				default:
					{
						INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown SendUserType = ") << static_cast<BYTE>(kEvent.SendType()));
					}break;
				}
			}
		}break;
	case CIE_CASH_LIMIT_ITEM_REQ:
		{
			PgCashManager::RequestCurerntLimitSell(pkJob->kPacket2nd);
		}break;
	case CIE_CASH_LIMIT_ITEM_RCV:
		{
			PgCashManager::RecvCurerntLimitSell(pkJob->kPacket2nd);
		}break;
	case CIE_CASH_BALANCE_REQ:
		{
			PgCashManager::RequestCurerntCash(pkJob->kPacket2nd);
		}break;
	case CIE_CASH_BALANCE_RCV:
		{
			PgCashManager::RecvCurrentCash(pkJob->kPacket2nd);
		}break;
	case CIE_CASH_USE_REQ:
		{
			PgCashManager::RequestCashModify(pkJob->kPacket2nd);
		}break;
	case CIE_CASH_USE_RCV:
		{
			PgCashManager::RecvCashModify(pkJob->kPacket2nd);
		}break;
	case CIE_CASH_PROCESS_END:
		{
			PgCashManager::FinishCashProcess(pkJob->kPacket2nd);
		}break;
	default:
		{
			g_kRealmUserMgr.Locked_ProcessModifyPlayer(pkJob->kCause, pkJob->kContOrder, pkJob->kGndKey, pkJob->kAddonPacket, pkJob->kPacket2nd);
		}break;
	}
	return S_OK;
}