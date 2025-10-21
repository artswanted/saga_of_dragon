#include "stdafx.h"
#include "PgDBProcess.h"
#include "Variant/PgDBCache.h"
#include "Variant/gm_const.h"
#include "PgGMTask.h"
#include "PgPetitionDataMgr.h"
#include "PgNoticeMgr.h"


HRESULT CALLBACK OnDB_EXECUTE(CEL::DB_RESULT &rkResult)
{
	switch(rkResult.QueryType())
	{
	case DQT_SELECT_PETITION_DATA: {Q_DQT_SELECT_PETITION_DATA(rkResult);}break;
	case DQT_RECEIPT_PETITION: {Q_DQT_RECEIPT_PETITION(rkResult);}break;
	case DQT_REMAINDER_PETITION: {Q_DQT_REMAINDER_PETITION(rkResult);}break;
	case DQT_SELECT_PETITION_STATE:{Q_DQT_SELECT_PETITION_STATE(rkResult);}break;
	case DQT_TRY_GMLOGIN:{Q_DQT_TRY_GMLOGIN(rkResult);}break;
	case DQT_MODIFY_PETITION_DATA: {Q_DQT_MODIFY_PETITION_DATA(rkResult);}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Can't find DB Quest return type[") << static_cast<int>(rkResult.QueryType()) << _T("]") );
			return E_FAIL;
		}break;
	}
	return S_OK;
}

HRESULT CALLBACK OnDB_EXECUTE_TRAN(CEL::DB_RESULT_TRAN &)
{
	return S_OK;
}

bool Q_DQT_SELECT_PETITION_DATA(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{
//		INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		CONT_PETITION_DATA kContPetition;
		kContPetition.clear();

		unsigned short usOldRealmNo = 0;
		
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		while(rkResult.vecArray.end() != kIter)
		{
			SPetitionData kNewPetitionData;
			kNewPetitionData.Clear();
			kIter->Pop(kNewPetitionData.m_PetitionId);kIter++;
			kIter->Pop(kNewPetitionData.m_szCharacterName);kIter++;
			kIter->Pop(kNewPetitionData.m_szTitle);kIter++;
			kIter->Pop(kNewPetitionData.m_szPetition);kIter++;
			kIter->Pop(kNewPetitionData.m_GmId);kIter++;
			kIter->Pop(kNewPetitionData.m_RealmNo);kIter++;
			kIter->Pop(kNewPetitionData.m_ChannelNo);kIter++;
			kIter->Pop(kNewPetitionData.m_State);kIter++;
			kIter->Pop(kNewPetitionData.m_Kind);kIter++;
			kIter->Pop(kNewPetitionData.m_dtReceiptTime);kIter++;
			kIter->Pop(kNewPetitionData.m_ReceiptIndex);kIter++;

			if(usOldRealmNo != 0 && usOldRealmNo != kNewPetitionData.m_RealmNo )
			{
				g_kPetitionDataMgr.AddPetitionData(usOldRealmNo, kContPetition);
				kContPetition.clear();
			}
			kContPetition.push_back(kNewPetitionData);
			usOldRealmNo = kNewPetitionData.m_RealmNo;
		}	
		g_kPetitionDataMgr.AddPetitionData(usOldRealmNo, kContPetition);

//		INFO_LOG(BM::LOG_LV7, _T("Petition Data Load Complite"));
		
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("TABLE [DQT_SELECT_PETITION_DATA] is row count 0") );
	return false;	
}

bool Q_DQT_TRY_GMLOGIN(CEL::DB_RESULT &rkResult)
{
	BM::Stream kPacket(PT_GM_L_ANS_GMTOOL, EGMC_MATCH_GMID);
	kPacket.Push(rkResult.QueryOwner());

	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{
//		INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		int iErrorCode = 0;
		SAuthInfo kGmInfo;
		std::wstring kId;
		std::wstring kPw;
		std::wstring kName;

		if(kIter != rkResult.vecArray.end())
		{
			kIter->Pop(iErrorCode);				++kIter;
			kIter->Pop(kGmInfo.kMemberGuid);	++kIter;
			kIter->Pop(kId);					++kIter;
			kIter->Pop(kPw);					++kIter;
			kIter->Pop(kName);					++kIter;
			kIter->Pop(kGmInfo.byGMLevel);		++kIter;

			kGmInfo.SetID(kId);
			kGmInfo.SetPW(kPw);
//			kGmInfo.SetName(kName);

			g_kGMUserMgr.TryAuth(iErrorCode, kGmInfo);
			return true;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Faild GM_AUTH") );
	return false;	
}

bool Q_DQT_RECEIPT_PETITION( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{
//		INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		int iReceiptIndex = 0;
		unsigned short usRealmNo = 0;
		BM::GUID kMemberGuid;
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		while(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(usRealmNo);		++kIter;
			kIter->Pop(kMemberGuid);	++kIter;
			kIter->Pop(iReceiptIndex);	++kIter;
		} 
		BM::Stream kPacket(PT_GM_C_ANS_RECEIPT_PETITION);
		kPacket.Push(usRealmNo);
		kPacket.Push(kMemberGuid);
		kPacket.Push(iReceiptIndex);
		SendToImmigration(kPacket);
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Failed") );
	return false;
}

bool Q_DQT_SELECT_PETITION_STATE( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Success") );
		int iCount = 0;
		unsigned short usRealmNo = 0;
		BM::GUID kMemberGuid;
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		while(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(usRealmNo);	++kIter;
			kIter->Pop(kMemberGuid); ++kIter;
			kIter->Pop(iCount); ++kIter;
		}

		//아직 처리되지 않은게 있으니 잠시 기다려라!!
		if (iCount > 0)
		{
			BM::Stream kPacket(PT_GM_C_ANS_WAIT_RECEIPT);
			kPacket.Push(usRealmNo);
			kPacket.Push(kMemberGuid);
			kPacket.Push(iCount);
			SendToImmigration(kPacket);
		}
		else
		{
			g_kPetitionDataMgr.RemainderPetition(usRealmNo, kMemberGuid);
		}			
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Failed") );
	return false;
}

bool Q_DQT_REMAINDER_PETITION( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
//		INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);
		int iCount = 0;
		unsigned short usRealmNo = 0;
		BM::GUID kMemberGuid;
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		while(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(kMemberGuid); ++kIter;
			kIter->Pop(usRealmNo); ++kIter;
			kIter->Pop(iCount); ++kIter;
		}
		BM::Stream kPacket(PT_GM_C_ANS_REMAINDER_PETITION);
		kPacket.Push(usRealmNo);
		kPacket.Push(kMemberGuid);
		kPacket.Push(iCount);
		SendToImmigration(kPacket);
//		SendToUser(MemberGuid, kPacket);

		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Failed") );
	return false;
}

bool Q_DQT_MODIFY_PETITION_DATA( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{
//		INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		CEL::DB_DATA_ARRAY::const_iterator Itr = rkResult.vecArray.begin();

		if(rkResult.vecArray.end() != Itr)
		{
			int iErrorCode;
			unsigned short usRealmNo = 0;
			BM::GUID kPetitionGuid;
			
			Itr->Pop(iErrorCode);		++Itr;
			Itr->Pop(usRealmNo);		++Itr;
			Itr->Pop(kPetitionGuid);	++Itr;

			g_kPetitionDataMgr.BroadcastPetitionData(kPetitionGuid, usRealmNo);
		}
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" TABLE [DQT_MODIFY_PETITION_DATA] is row count 0") );
	return false;
}