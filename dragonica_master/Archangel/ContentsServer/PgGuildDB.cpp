#include "stdafx.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgControlDefMgr.h"
#include "PgGuildDB.h"
#include "PgGuildMgr.h"
#include "PgEmporiaMgr.h"

// Select
bool DBR_Community::Q_DQT_GUILD_SELECT_BASIC_INFO(CEL::DB_RESULT &rkResult)//One Guild Info
{
	BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner()); //여기서 Owner는 Guild Guid이다
	kPacket.Push( static_cast< BYTE >(GC_DB_Basic) );

	bool bRet = false;
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		kPacket.Push( static_cast< BYTE >(GCR_Success) );

		CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

		SGuildBasicInfo kBasicInfo;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();

		//while(rkResult.vecArray.end() != result_iter)
		{ // Basic info
			int const iCount = (*count_iter);		++count_iter;

			unsigned short sTaxRate = 0;
			DBTIMESTAMP kDateTime;
			result_iter->Pop(kBasicInfo.kGuildGuid);				++result_iter;
			result_iter->Pop(kBasicInfo.kMasterGuid);				++result_iter;
			result_iter->Pop(kBasicInfo.kGuildName);				++result_iter;
			result_iter->Pop(kBasicInfo.sLevel);					++result_iter;
			result_iter->Pop(kBasicInfo.iExperience);				++result_iter;
			result_iter->Pop(kBasicInfo.cEmblem);					++result_iter;
			result_iter->Pop(kDateTime);							++result_iter;	kBasicInfo.kCreateDate = kDateTime;
			result_iter->Pop(sTaxRate);								++result_iter;
			result_iter->Pop(kBasicInfo.sSkillPoint);				++result_iter;
			result_iter->Pop(kBasicInfo.cGuildSkill);				++result_iter;
			result_iter->Pop(kBasicInfo.i64Money);					++result_iter;
			result_iter->Pop(kBasicInfo.abyInvExtern);				++result_iter;
			result_iter->Pop(kBasicInfo.abyInvExternIdx);			++result_iter;
			result_iter->Pop(kBasicInfo.abyInvAuthority);			++result_iter;
			kBasicInfo.WriteToPacket(kPacket);
		}
		{ // EmporiaInfo도 여기서 준다.
			SGuildEmporiaInfo kEmporiaInfo;
			g_kEmporiaMgr.Locked_GetEmporiaInfo( kBasicInfo.kGuildGuid, kEmporiaInfo );
			kPacket.Push( kEmporiaInfo );
		}
		{ // Extern info
			int const iCount = (*count_iter);		++count_iter;
			kPacket.Push( iCount );

			for( int iCur = 0; iCount > iCur; ++iCur )
			{
				std::wstring kGuildNotice;
				result_iter->Pop(kGuildNotice);	++result_iter;

				kPacket.Push(kGuildNotice);
			}
		}
		{ // Member Grade
			int const iCount = (*count_iter);		++count_iter;
			kPacket.Push( iCount );

			SGuildMemberGradeInfo kGradeInfo;
			for( int iCur = 0; iCount > iCur; ++iCur )
			{
				result_iter->Pop( kGradeInfo.kGuildGuid );			++result_iter;
				result_iter->Pop( kGradeInfo.cGradeGroupLevel );	++result_iter;
				result_iter->Pop( kGradeInfo.kGradeName[0] );		++result_iter;
				result_iter->Pop( kGradeInfo.kGradeName[1] );		++result_iter;
				result_iter->Pop( kGradeInfo.kGradeName[2] );		++result_iter;
				result_iter->Pop( kGradeInfo.kGradeName[3] );		++result_iter;
				result_iter->Pop( kGradeInfo.kGradeName[4] );		++result_iter;
				result_iter->Pop( kGradeInfo.kLastModifyDate );		++result_iter;

				kGradeInfo.WriteToPacket(kPacket);
			}
		}
		{// Mercenary
			int const iCount = (*count_iter);		++count_iter;

			__int64 i64ClassLimit = 0i64;
			short nLevelLimit = 0;
			for( int iCur = 0; iCount > iCur; ++iCur )
			{
				result_iter->Pop( i64ClassLimit );	++result_iter;
				result_iter->Pop( nLevelLimit );	++result_iter;
			}

			PgLimitClass kSetMercenary( i64ClassLimit, nLevelLimit );
			kSetMercenary.WriteToPacket( kPacket );
		}
		{ // Member
			int const iCount = (*count_iter);		++count_iter;
			kPacket.Push( iCount );

			SGuildMemberInfo kMember;
			short sStreetNo;
			int iHouseNo;

			for( int iCur = 0; iCount > iCur; ++iCur )
			{
				result_iter->Pop( kMember.kCharGuid );		++result_iter;
				result_iter->Pop( kMember.cGrade );			++result_iter;
				result_iter->Pop( kMember.sLevel );			++result_iter;
				result_iter->Pop( kMember.kCharName );		++result_iter;
				result_iter->Pop( kMember.cClass );			++result_iter;
				result_iter->Pop(sStreetNo);					++result_iter;
				result_iter->Pop(iHouseNo);						++result_iter;

				kMember.kHomeAddr = SHOMEADDR(sStreetNo, iHouseNo);
			
				kMember.WriteToPacket(kPacket);
			}
		}
		{// 길드가입 설정 정보
			int const iCount = (*count_iter);	++count_iter;
			
			SSetGuildEntrancedOpen kGuildEntranceOpen;
			for( int iCur = 0; iCount > iCur; ++iCur )
			{
				result_iter->Pop( kGuildEntranceOpen.bIsGuildEntrance );		++result_iter;
				result_iter->Pop( kGuildEntranceOpen.sGuildEntranceLevel );		++result_iter;
				result_iter->Pop( kGuildEntranceOpen.i64GuildEntranceClass );	++result_iter;
				result_iter->Pop( kGuildEntranceOpen.i64GuildEntranceFee );		++result_iter;
				result_iter->Pop( kGuildEntranceOpen.wstrGuildPR );				++result_iter;
			}

			kGuildEntranceOpen.WriteToPacket( kPacket );
		}
		{//contUserData
			kPacket.Push(rkResult.contUserData.Data());
		}
		::SendToGuildMgr(kPacket);
		bRet = true;
	}
	else
	{
		kPacket.Push( static_cast< BYTE >(GCR_Failed) ); // 실패
		::SendToGuildMgr(kPacket);
	}

	g_kCoreCenter.ClearQueryResult( rkResult );

	if( !bRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	return bRet;
}

bool DBR_Community::Q_DQT_GUILD_INV_AUTHORITY(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet
	||	CEL::DR_NO_RESULT == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		if( result_iter != rkResult.vecArray.end() )
		{
			BM::GUID kGuildGuid;
			BYTE byAuthority[MAX_DB_GUILD_INV_AUTHORITY] = {0, };

			result_iter->Pop(kGuildGuid);		++result_iter;
			result_iter->Pop(byAuthority);		++result_iter;

			BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner()); //GuildGuid
			kPacket.Push((BYTE)GC_M_InventorySupervision);
			kPacket.Push((BYTE)GCR_Success);			
			kPacket.Push(byAuthority);
			SendToGuildMgr(kPacket);
			return true;
		}
	}

	CAUTION_LOG( BM::LOG_LV0, __FL__ << L"Failed GuildID : " << rkResult.QueryOwner() );
	LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ << L"Return false" );
	return false;
}

//	Create / Destroy
bool DBR_Community::Q_DQT_GUILD_PROC(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		BM::GUID kGuildGuid;
		std::wstring kGuildName;

		bool bRet = false;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		while(rkResult.vecArray.end() != result_iter)
		{
			int iRet = S_OK;
			BYTE cCmdType;
			result_iter->Pop(iRet);			++result_iter;
			result_iter->Pop(cCmdType);		++result_iter;
			result_iter->Pop(kGuildGuid);		++result_iter;
			result_iter->Pop(kGuildName);		++result_iter;

			bRet = (S_OK==iRet) || bRet;

			BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner()); //플레이어
			kPacket.Push((BYTE)cCmdType);
			kPacket.Push((BYTE) ((bRet)?GCR_Success: GCR_Failed));
			kPacket.Push(kGuildGuid);
			kPacket.Push(kGuildName);
			SendToGuildMgr(kPacket);
		}
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//Create / Modify / Delete
bool DBR_Community::Q_DQT_GUILD_MEMBER_PROC(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		bool bRet = false;
		BM::GUID kGuildGuid;
		BM::GUID kCharGuid;
		BYTE cMemberGrade = 0;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		while(rkResult.vecArray.end() != result_iter)
		{
			int iRet = S_OK;
			BYTE cCmdType = GC_None;
			result_iter->Pop(iRet);				++result_iter;
			result_iter->Pop(cCmdType);			++result_iter;
			result_iter->Pop(kGuildGuid);		++result_iter;
			result_iter->Pop(kCharGuid);		++result_iter;
			result_iter->Pop(cMemberGrade);		++result_iter;

			bRet = (S_OK==iRet) || bRet;

			BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());
			kPacket.Push( (BYTE)cCmdType );
			kPacket.Push( (BYTE) ((bRet)? GCR_Success: GCR_Failed) );
			kPacket.Push( kGuildGuid );
			kPacket.Push( kCharGuid );
			kPacket.Push( cMemberGrade );
			SendToGuildMgr(kPacket);
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool DBR_Community::Q_DQT_GUILD_UPDATE_MEMBER_GRADE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);
		
		bool bRet = false;
		BM::GUID kCharGuid;
		SGuildMemberGradeInfo kGradeInfo;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		while(rkResult.vecArray.end() != result_iter)
		{
			int iRet = S_OK;
			result_iter->Pop(iRet);			++result_iter;
			result_iter->Pop(kGradeInfo.kGuildGuid);			++result_iter;
			result_iter->Pop(kGradeInfo.cGradeGroupLevel);	++result_iter;
			result_iter->Pop(kGradeInfo.kGradeName[0]);		++result_iter;
			result_iter->Pop(kGradeInfo.kGradeName[1]);		++result_iter;
			result_iter->Pop(kGradeInfo.kGradeName[2]);		++result_iter;
			result_iter->Pop(kGradeInfo.kGradeName[3]);		++result_iter;
			result_iter->Pop(kGradeInfo.kGradeName[4]);		++result_iter;

			bRet = (S_OK==iRet) || bRet;
		}

		//결과 전송
		BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());
		kPacket.Push((BYTE)GC_M_Grade);
		kPacket.Push((BYTE) ((bRet)? GCR_Success: GCR_Failed));
		kGradeInfo.WriteToPacket(kPacket);
		SendToGuildMgr(kPacket);

		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


//bool DBR_Community::Q_DQT_GUILD_UPDATE_TAX_RATE(CEL::DB_RESULT &rkResult)
//{
//	if(CEL::DR_SUCCESS == rkResult.eRet)
//	{
//		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);
//
//		bool bRet = false;
//		BM::GUID kGuildGuid;
//		BYTE cTaxRate = 0;
//		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
//		while(rkResult.vecArray.end() != result_iter)
//		{
//			int iRet = S_OK;
//			result_iter->Pop(iRet);			++result_iter;
//			result_iter->Pop(kGuildGuid);		++result_iter;
//			result_iter->Pop(cTaxRate);		++result_iter;
//
//			bRet = (S_OK==iRet) || bRet;
//		}
//
//		//결과 전송
//		BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());
//		kPacket.Push((BYTE)GC_M_TaxRate);
//		kPacket.Push((BYTE) ((bRet)? GCR_Success: GCR_Failed));
//		kPacket.Push(kGuildGuid);
//		kPacket.Push(cTaxRate);
//		SendToGuildMgr(kPacket);
//		
//		return true;
//	}
//	VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Failed Ret[%d]"), __FUNCTIONW__, __LINE__, rkResult.eRet);
//	return false;
//}

bool DBR_Community::Q_DQT_GUILD_UPDATE_NOTICE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		bool bRet = false;
		BM::GUID kGuildGuid;
		std::wstring kNotice;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		while(rkResult.vecArray.end() != result_iter)
		{
			int iRet = S_OK;
			result_iter->Pop(iRet);			++result_iter;
			result_iter->Pop(kGuildGuid);		++result_iter;
			result_iter->Pop(kNotice);		++result_iter;

			bRet = (S_OK==iRet) || bRet;
		}

		//결과 전송
		BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());
		kPacket.Push((BYTE)GC_M_Notice);
		kPacket.Push((BYTE) ((bRet)? GCR_Success: GCR_Failed));
		kPacket.Push(kGuildGuid);
		kPacket.Push(kNotice);
		SendToGuildMgr(kPacket);
		
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_UPDATE_EXP_LEVEL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		BYTE cCmdType = 0;
		bool bRet = false;
		BM::GUID kGuildGuid;
		unsigned short sLevel = 0;
		__int64 iExperience = 0;
		unsigned short sSkillPoint = 0;
		BYTE cGuildSkill[MAX_DB_GUILD_SKILL_SIZE] = {0, };
		int iExternInt = 0;
		BM::GUID kOperatorGuid;

		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		while(rkResult.vecArray.end() != result_iter)
		{
			int iRet = S_OK;
			result_iter->Pop(iRet);				++result_iter;
			result_iter->Pop(cCmdType);			++result_iter;
			result_iter->Pop(kGuildGuid);		++result_iter;
			result_iter->Pop(sLevel);			++result_iter;
			result_iter->Pop(iExperience);		++result_iter;
			result_iter->Pop(sSkillPoint);		++result_iter;
			result_iter->Pop(cGuildSkill);		++result_iter;
			result_iter->Pop(iExternInt);		++result_iter;
			result_iter->Pop(kOperatorGuid);	++result_iter;

			bRet = (S_OK==iRet) || bRet;
		}

		//결과 전송
		BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());
		kPacket.Push(cCmdType);
		kPacket.Push((BYTE) ((bRet)? GCR_Success: GCR_Failed));
		kPacket.Push(sLevel);
		kPacket.Push(iExperience);
		kPacket.Push(sSkillPoint);
		kPacket.Push(cGuildSkill);
		kPacket.Push(iExternInt);
		kPacket.Push(kOperatorGuid);
		SendToGuildMgr(kPacket);
		
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool DBR_Community::Q_DQT_GUILD_INVENTORY_CREATE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		if(rkResult.vecArray.end() != result_iter)
		{
			int iRet = S_OK;
			BYTE abyInvExtern[MAX_DB_INVEXTEND_SIZE] = {0,};
			BYTE abyInvExternIdx[MAX_DB_INVEXTEND_SIZE] = {0,};

			result_iter->Pop(iRet);				++result_iter;
			result_iter->Pop(abyInvExtern);		++result_iter;
			result_iter->Pop(abyInvExternIdx);	++result_iter;

			bool bRet = (S_OK == iRet);

			//결과 전송
			BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());
			kPacket.Push(static_cast<BYTE>(GC_M_InventoryCreate));
			kPacket.Push(static_cast<BYTE>(((bRet)? GCR_Success: GCR_Failed)));
			kPacket.Push(abyInvExtern);
			kPacket.Push(abyInvExternIdx);
			SendToGuildMgr(kPacket);
			return true;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_INVENTORY_LOAD(CEL::DB_RESULT &rkResult)
{
	INFO_LOG( BM::LOG_LV6, __FL__ << L" Guild Guid : "<< rkResult.QueryOwner().str()
		<< L" eRet : "<< (int)rkResult.eRet );

	if( CEL::DR_SUCCESS == rkResult.eRet 
	|| CEL::DR_NO_RESULT == rkResult.eRet ) // 길드금고에 아이템이 없어도 실패는 아니잖아.
	{
		g_kGuildMgr.ProcessItemQuery( rkResult.QueryOwner(), rkResult );
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_INVENTORY_EXTEND(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}	

	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}

bool DBR_Community::Q_DQT_GUILD_INVENTORY_LOG_DELETE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}	

	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}

bool DBR_Community::Q_DQT_GUILD_INVENTORY_LOG_SELECT(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet 
	||	CEL::DR_NO_RESULT == rkResult.eRet ) 
	{
		CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();

		BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, static_cast<BYTE>(GC_InventoryLog));
		kPacket.Push(static_cast<BYTE>(GCR_Success));

		int iTotalPage = 0;
		int iCurPage = 0;
		__int64 i64TempRowNum = 0;
		SGuild_Inventory_Log kInvLog;

		{
			int const iCount = (*count_iter);				++count_iter;
			
			result_iter->Pop(iTotalPage);					++result_iter;
			result_iter->Pop(iCurPage);						++result_iter;
			
			kPacket.Push(iTotalPage);
			kPacket.Push(iCurPage);
		}		

		{// 해당 페이지의 로그
			int const iCount = (*count_iter);				++count_iter;
			kPacket.Push(iCount);

			for( int iCur = 0; iCount > iCur; ++iCur )
			{
				result_iter->Pop( kInvLog.m_kGuildGuid );	++result_iter;
				result_iter->Pop( kInvLog.m_kItemGuid );	++result_iter;
				result_iter->Pop( kInvLog.m_kDateTime );	++result_iter;
				result_iter->Pop( kInvLog.m_kCharName );	++result_iter;
				result_iter->Pop( kInvLog.m_kType );		++result_iter;
				result_iter->Pop( kInvLog.m_kItemNo );		++result_iter;
				result_iter->Pop( kInvLog.m_kCount );		++result_iter;
				result_iter->Pop( i64TempRowNum );			++result_iter;

				kInvLog.WriteToPacket(kPacket);
			}
		}

		g_kRealmUserMgr.Locked_SendToUser( rkResult.QueryOwner(), kPacket, false );
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_INVENTORY_LOG_INSERT(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}

bool DBR_Community::Q_DQT_GUILD_CHECK_NAME(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);
		
		int iRet = S_OK;
		BYTE cCmdType = 0;
		BM::GUID kGuildGuid;
		std::wstring kGuildName;
		BM::GUID kOwnerCharGuid;
		int iExternInt1 = 0;

		bool bRet = false;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		while(rkResult.vecArray.end() != result_iter)
		{
			result_iter->Pop(iRet);					++result_iter;
			result_iter->Pop(cCmdType);				++result_iter;
			result_iter->Pop(kGuildGuid);			++result_iter;
			result_iter->Pop(kGuildName);			++result_iter;
			result_iter->Pop(kOwnerCharGuid);		++result_iter;
			result_iter->Pop(iExternInt1);			++result_iter;

			bRet = (S_OK==iRet) || bRet;
		}

		BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());//GC_PreCreate
		kPacket.Push(cCmdType);
		kPacket.Push( (BYTE)((bRet)? GCR_Success: GCR_Failed) );
		kPacket.Push(kGuildName);
		kPacket.Push(kGuildGuid);
		SendToGuildMgr(kPacket);
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_RENAME(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		int iRet = S_OK;
		BYTE cCmdType = 0;
		BM::GUID kGuildGuid;
		std::wstring kGuildName;
		BM::GUID kOwnerCharGuid;
		int iExternInt1 = 0;

		bool bRet = false;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
		while(rkResult.vecArray.end() != result_iter)
		{
			result_iter->Pop(iRet);					++result_iter;
			result_iter->Pop(cCmdType);				++result_iter;
			result_iter->Pop(kGuildGuid);			++result_iter;
			result_iter->Pop(kGuildName);			++result_iter;
			result_iter->Pop(kOwnerCharGuid);		++result_iter;
			result_iter->Pop(iExternInt1);			++result_iter;

			bRet = (S_OK==iRet) || bRet;
		}

		BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());//GC_M_Rename
		kPacket.Push( cCmdType );
		kPacket.Push( (BYTE)((bRet)? GCR_Success: GCR_Failed) );
		kPacket.Push( kGuildName );
		kPacket.Push( kGuildGuid );
		kPacket.Push( (bool)((0 != iExternInt1)? 1: 0) );
		SendToGuildMgr(kPacket);
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_CHANGE_OWNER(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		bool bQuerySuccess = true;
		int iCmdType = 0;
		BM::GUID kNewGuildOwner, kOldGuildOwner;
		CEL::DB_DATA_ARRAY &rkArray = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkArray.begin();
		while(rkArray.end() != result_iter)
		{
			int iRet = S_OK;

			result_iter->Pop( iRet );				++result_iter;
			result_iter->Pop( iCmdType );			++result_iter;
			result_iter->Pop( kNewGuildOwner );		++result_iter;
			result_iter->Pop( kOldGuildOwner );		++result_iter;

			bQuerySuccess = (S_OK == iRet);
		}

		BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW, rkResult.QueryOwner());//GC_M_ChangeOwner
		kPacket.Push( static_cast< BYTE >(GC_M_ChangeOwner) );
		kPacket.Push( static_cast< BYTE >((bQuerySuccess)? GCR_Success: GCR_Failed) );
		kPacket.Push( kNewGuildOwner );
		kPacket.Push( iCmdType );
		kPacket.Push( kOldGuildOwner );
		SendToGuildMgr(kPacket);
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_SENDMAIL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet
	||	CEL::DR_NO_RESULT == rkResult.eRet )
	{
		return true;
	}

	CAUTION_LOG( BM::LOG_LV0, __FL__ << L"Failed GuildID : " << rkResult.QueryOwner() );
	LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ << L"Return false" );
	return false;
}

bool DBR_Community::Q_DQT_COUPLE_INFO_SELECT(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet
	||	CEL::DR_NO_RESULT == rkResult.eRet )
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		SCouple kMyCoupleInfo;

		CEL::DB_DATA_ARRAY &rkArray = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkArray.begin();
		while(rkArray.end() != result_iter)
		{
			kMyCoupleInfo.ReadFromDBResult(result_iter);
		}

		PgDoc_Player kCopyPlayer;
		if( g_kRealmUserMgr.Locked_GetDoc_Player(rkResult.QueryOwner(), true, kCopyPlayer) )
		{
			kMyCoupleInfo.QuestID( PgQuestInfoUtil::GetHaveCoupleQuestID(kCopyPlayer.GetMyQuest()) );
		}
		else
		{
			kMyCoupleInfo.QuestID( 0 );
		}

		BM::Stream kPacket(PT_T_N_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Info);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( rkResult.QueryOwner() );
		kMyCoupleInfo.WriteToPacket( kPacket );
		SendToCoupleMgr(kPacket);
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_COUPLE_UPDATE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);

		bool bRet = true;
		CEL::DB_DATA_ARRAY &rkArray = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkArray.begin();
		while(rkArray.end() != result_iter)
		{
			int iRet = 0;

			result_iter->Pop( iRet );		++result_iter;

			bRet = (0 == iRet) & bRet;
		}

		BM::Stream kPacket(PT_T_N_ANS_COUPLE_COMMAND, (BYTE)CC_Sys_Save);
		kPacket.Push( (BYTE)((bRet)? CoupleCR_Success: CoupleCR_Failed) );
		kPacket.Push( rkResult.QueryOwner() );
		SendToCoupleMgr(kPacket);

		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
bool DBR_Community::Q_DQT_GUILD_SELECT_OWNER_LAST_LOGIN_DAY(CEL::DB_RESULT &rkResult, bool const bInit)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
		CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

		BM::Stream kPacket(PT_A_N_REQ_GUILD_OWNER_LAST_LOGIN_DAY);
		kPacket.Push( static_cast< int >(GACOR_NONE) );
		kPacket.Push( (*count_iter) );
		kPacket.Push( bInit );
		while( rkVec.end() != result_iter )
		{
			SGuildOwnerLoginDay kTemp;
			kTemp.ReadFromDBResult(result_iter);

			kTemp.WriteToPacket( kPacket );
		}

		::SendToGuildMgr(kPacket);
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return true;
}
bool DBR_Community::Q_DQT_GUILD_SELECT_NEXT_OWNER(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		BM::Stream kPacket(PT_A_N_REQ_GUILD_OWNER_LAST_LOGIN_DAY);

		CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

		{
			int iCmdType = 0, iGuildLevel = 0;
			BM::GUID kGuildGuid, kOldOwner, kNewOwner;
			std::wstring kOldOwnerName, kNewOwnerName;
			result_iter->Pop( iCmdType );		++result_iter;
			result_iter->Pop( iGuildLevel );	++result_iter;
			result_iter->Pop( kGuildGuid );		++result_iter;
			result_iter->Pop( kOldOwner );		++result_iter;
			result_iter->Pop( kNewOwner );		++result_iter;
			result_iter->Pop( kOldOwnerName );	++result_iter;
			result_iter->Pop( kNewOwnerName );	++result_iter;

			kPacket.Push( iCmdType );
			kPacket.Push( iGuildLevel );
			kPacket.Push( kGuildGuid );
			kPacket.Push( kOldOwner );
			kPacket.Push( kNewOwner );
			kPacket.Push( kOldOwnerName );
			kPacket.Push( kNewOwnerName );
		}

		::SendToGuildMgr(kPacket);
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return true;
}
bool DBR_Community::Q_DQT_GUILD_COMMON(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
	&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}

bool DBR_Community::Q_DQT_GUILD_MERCENARY_SAVE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}

bool DBR_Community::Q_DQT_GUILD_ENTRANCEOPEN_SAVE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}

bool DBR_Community::Q_DQT_GUILD_ENTRANCEOPEN_LIST(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY &rkArray = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkArray.begin();		

		bool bIsGMCommand = false;
		BM::GUID kCharGuid;

		rkResult.contUserData.Pop(bIsGMCommand);
		rkResult.contUserData.Pop(kCharGuid);

		g_kGuildMgr.ClearEntranceOpenGuild();

		int iRank = 0;
		while( rkArray.end() != result_iter )
		{
			SEntranceOpenGuild kEntranceOpenGuild;
			kEntranceOpenGuild.iRank = (++iRank);
			
			result_iter->Pop( kEntranceOpenGuild.kGuildGuid );				++result_iter;
			result_iter->Pop( kEntranceOpenGuild.sGuildEntranceLevel );		++result_iter;
			result_iter->Pop( kEntranceOpenGuild.i64GuildEntranceClass );	++result_iter;
			result_iter->Pop( kEntranceOpenGuild.i64GuildEntranceFee );		++result_iter;
			result_iter->Pop( kEntranceOpenGuild.wstrGuildPR );				++result_iter;
			result_iter->Pop( kEntranceOpenGuild.byEmblem );				++result_iter;
			result_iter->Pop( kEntranceOpenGuild.wstrGuildName );			++result_iter;
			result_iter->Pop( kEntranceOpenGuild.sGuildLevel );				++result_iter;
			result_iter->Pop( kEntranceOpenGuild.wstrMasterName );			++result_iter;
			result_iter->Pop( kEntranceOpenGuild.iGuildMemberCount );		++result_iter;

			g_kGuildMgr.AddEntranceOpenGuild(kEntranceOpenGuild);
		}

		g_kGuildMgr.SetRecommendGuild(); // 추천길드 결정(일단은 랜덤임, 인덱스는 랭킹)

		if( bIsGMCommand 
			&& ( BM::GUID::NullData() != kCharGuid ) )
		{//Net_GuildBoardRefresh() 사용한 GM에게 전송해줌
			g_kGuildMgr.NotifyEntranceOpenGuildList(kCharGuid);
		}

		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_REQ_ENTRANCE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		BM::Stream kPacket(PT_N_C_ANS_GUILD_ENTRANCE);

		CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

		int iRet = S_OK;
		result_iter->Pop( iRet );

		bool bRet = (S_OK==iRet);
		kPacket.Push( bRet );

		g_kRealmUserMgr.Locked_SendToUser( rkResult.QueryOwner(), kPacket, false );
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_REQ_ENTRANCE_CANCEL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}

bool DBR_Community::Q_DQT_GUILD_APPLICANT_LIST(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY &rkArray = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkArray.begin();

		while( rkArray.end() != result_iter )
		{
			SGuildEntranceApplicant kGuildEntranceApplicant;
			result_iter->Pop(kGuildEntranceApplicant.kGuildGuid);			++result_iter;
			result_iter->Pop(kGuildEntranceApplicant.kCharGuid);			++result_iter;
			result_iter->Pop(kGuildEntranceApplicant.wstrName);				++result_iter;
			result_iter->Pop(kGuildEntranceApplicant.sLevel);				++result_iter;
			result_iter->Pop(kGuildEntranceApplicant.byClass);				++result_iter;
			result_iter->Pop(kGuildEntranceApplicant.wstrMessage);			++result_iter;
			result_iter->Pop(kGuildEntranceApplicant.kDateTime);			++result_iter;
			result_iter->Pop(kGuildEntranceApplicant.byState);				++result_iter;
			result_iter->Pop(kGuildEntranceApplicant.i64GuildEntranceFee);	++result_iter;

			g_kGuildMgr.AddGuildEntranceApplicant(kGuildEntranceApplicant);
		}

		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool DBR_Community::Q_DQT_GUILD_ENTRANCE_PROCESS(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret[") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}