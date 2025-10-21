#include "stdafx.h"
#include "Lohengrin/GameTime.h"
#include "Lohengrin/variablecontainer.h"
#include "Variant/PgControlDefMgr.h"
#include "Variant/PgEventView.h"
#include "item/PgDBProcess.h"
#include "item/PgPostManager.h"
#include "item/PgCashShopManager.h"
#include "JobDispatcher.h"
#include "Transaction.h"
#include "PgGMProcessMgr.h"
#include "Global.h"
#include "PgEmporiaMgr.h"

extern HRESULT Q_DQT_UPDATE_USER_ITEM(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UPDATE_USER_FIELD(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UPDATE_ING_QUEST(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UPDATE_END_QUEST(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UPDATE_DATE_CONTENTS(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UPDATE_QUEST_CLEAR_COUNT(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_INIT_QUEST_CLEAR_COUNT(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_UPDATE_USER_ITEM(CEL::DB_RESULT_TRAN &rkResult);

extern HRESULT Q_DQT_POST_ADD_MAIL(CEL::DB_RESULT & rkResult);
extern HRESULT Q_DQT_POST_GET_MAIL(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_POST_MODIFY_MAIL(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_POST_GET_MAIL_ITEM_RESERVE(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_POST_GET_MAIL_MIN(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_POST_NEW_MAIL_NOTY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_POST_CHECK_EANBLE_SEND(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_POST_GET_MAIL_ITEM(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_POST_GET_MAIL_ITEM(CEL::DB_RESULT_TRAN &rkResult);

extern HRESULT Q_DQT_UM_ARTICLE_QUERY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UM_GO_TIME_PROC(CEL::DB_RESULT & rkResult);
extern HRESULT Q_DQT_UM_MY_ARTICLE_QUERY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UM_DEV_SET_ARTICLE_STATE(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UM_MODIFY_MARKET_STATE(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_UM_MARKET_OPEN(CEL::DB_RESULT_TRAN &rkResult);
extern HRESULT Q_DQT_UM_ARTICLE_BUY(CEL::DB_RESULT_TRAN & rkResult);
extern HRESULT Q_DQT_UM_DEALING_READ(CEL::DB_RESULT_TRAN & rkResult);
extern HRESULT Q_DQT_UM_ARTICLE_REG(CEL::DB_RESULT_TRAN &rkResult);
extern HRESULT Q_DQT_UM_ARTICLE_DEREG(CEL::DB_RESULT_TRAN &rkResult);
extern HRESULT Q_DQT_UM_MODIFY_MARKET(CEL::DB_RESULT_TRAN &rkResult);

extern HRESULT Q_DQT_CS_CASH_QUERY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_CS_CASH_MODIFY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_CS_ADD_GIFT(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_CS_GIFT_QUEYR(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_CS_MODIFY_VISABLE_RANK(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_CS_CASH_LIMIT_ITEM_QUERY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_CS_CASH_LIMIT_ITEM_UPDATE_QUERY(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_CS_CASHSHOP(CEL::DB_RESULT_TRAN &rkResult);
extern HRESULT Q_DQT_CS_RECV_GIFT(CEL::DB_RESULT_TRAN &rkResult);

extern HRESULT Q_DQT_SELECT_CHARA_SKILL(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_CHAR_RESET_SKILL(CEL::DB_RESULT_TRAN &rkResult);
extern HRESULT Q_DQT_GET_EVENT_COUPON(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_GET_LAST_RECVED_GIFT(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_HATCH_PET(CEL::DB_RESULT_TRAN & rkContResult);
extern HRESULT Q_DQT_REMOVE_PET(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_RENAME_PET(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_SETABIL_PET(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_LOAD_PET_ITEM(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_VISITLOG_ADD(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_VISITLOG_MODIFY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_VISITLOG_DELETE(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_VISITLOG_LIST(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_CHECK_ENABLE_VISIT_OPTION(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_MYHOME_SIDEJOB_ENTER(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_VISITFLAG_MODIFY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_MYHOME_MODIFY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_ADD_MYHOME(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_INVITATION_CREATE(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_INVITATION_SELECT(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_VISITORS_SELECT(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_EVENT_CASHITEM_GIFT1(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_UPDATE_ACHIEVEMENT_FIST(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_SELECT_ACHIEVEMENT_RANK(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_POST_GROUP_MAIL(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_SELECT_USER_QUEST_COMPLETE(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UPDATE_USER_QUEST_COMPLETE(CEL::DB_RESULT &rkResult);

extern HRESULT Q_DQT_UPDATE_SPECIFIC_REWARD(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_UPDATE_USER_JOBSKILL_INFO_NO_OP(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_NO_OP(CEL::DB_RESULT &rkResult);

HRESULT CALLBACK OnDB_EXECUTE_Item(CEL::DB_RESULT &rkResult)
{
	switch(rkResult.QueryType())
	{
	case DQT_INVITATION_SELECT:{return Q_DQT_INVITATION_SELECT(rkResult);}break;
	case DQT_VISITORS_SELECT:{return Q_DQT_VISITORS_SELECT(rkResult);}break;
	case DQT_INVITATION_CREATE:{return Q_DQT_INVITATION_CREATE(rkResult);}break;
	case DQT_VISITFLAG_MODIFY:{return Q_DQT_VISITFLAG_MODIFY(rkResult);}break;
	case DQT_VISITLOG_ADD:{return Q_DQT_VISITLOG_ADD(rkResult);}break;
	case DQT_VISITLOG_DELETE:{return Q_DQT_VISITLOG_DELETE(rkResult);}break;
	case DQT_VISITLOG_LIST:{return Q_DQT_VISITLOG_LIST(rkResult);}break;
	case DQT_CHECK_ENABLE_VISIT_OPTION:{return Q_DQT_CHECK_ENABLE_VISIT_OPTION(rkResult);}break;
	case DQT_MYHOME_SIDEJOB_ENTER:{return Q_DQT_MYHOME_SIDEJOB_ENTER(rkResult);}break;

	case DQT_UM_DEV_SET_ARTICLE_STATE:{return Q_DQT_UM_DEV_SET_ARTICLE_STATE(rkResult);}break;
	case DQT_UM_MY_ARTICLE_QUERY:{return Q_DQT_UM_MY_ARTICLE_QUERY(rkResult);}break;
	case DQT_UM_GO_TIME_PROC:{return Q_DQT_UM_GO_TIME_PROC(rkResult);}break;
	case DQT_UM_ARTICLE_QUERY:{return Q_DQT_UM_ARTICLE_QUERY(rkResult);}break;
	case DQT_UM_MODIFY_MARKET_STATE:{return Q_DQT_UM_MODIFY_MARKET_STATE(rkResult);}break;

	case DQT_POST_NEW_MAIL_NOTY:{return Q_DQT_POST_NEW_MAIL_NOTY(rkResult);}break;
	case DQT_POST_GET_MAIL_MIN:{return Q_DQT_POST_GET_MAIL_MIN(rkResult);}break;
	case DQT_POST_GET_MAIL_ITEM_RESERVE:{return Q_DQT_POST_GET_MAIL_ITEM_RESERVE(rkResult);}break;
	case DQT_POST_ADD_MAIL:{return Q_DQT_POST_ADD_MAIL(rkResult);}break;
	case DQT_POST_GET_MAIL:{return Q_DQT_POST_GET_MAIL(rkResult);}break;
	case DQT_POST_MODIFY_MAIL:{return Q_DQT_POST_MODIFY_MAIL(rkResult);}break;
	case DQT_POST_GET_MAIL_ITEM:{return Q_DQT_POST_GET_MAIL_ITEM(rkResult);}break;

	case DQT_UPDATE_USER_ITEM: {return Q_DQT_UPDATE_USER_ITEM(rkResult);}break;
	case DQT_UPDATE_USER_FIELD:{return Q_DQT_UPDATE_USER_FIELD(rkResult);}break;
	case DQT_CLEAR_CONNECTION_CHANNEL:{Q_DQT_CLEAR_CONNECTION_CHANNEL(rkResult);}break;
	case DQT_POST_CHECK_EANBLE_SEND:{return Q_DQT_POST_CHECK_EANBLE_SEND(rkResult);}break;
	case DQT_UPDATE_ING_QUEST:{ return Q_DQT_UPDATE_ING_QUEST(rkResult);}break;
	case DQT_UPDATE_END_QUEST:{ return Q_DQT_UPDATE_END_QUEST(rkResult);}break;
	case DQT_UPDATE_DATE_CONTENTS:{ return Q_DQT_UPDATE_DATE_CONTENTS(rkResult);}break;
	case DQT_UPDATE_QUEST_CLEAR_COUNT: {return Q_DQT_UPDATE_QUEST_CLEAR_COUNT(rkResult);}break;
	case DQT_INIT_QUEST_CLEAR_COUNT: {return Q_DQT_INIT_QUEST_CLEAR_COUNT(rkResult);}break;

	case DQT_CS_CASH_QUERY:{return Q_DQT_CS_CASH_QUERY(rkResult);}break;
	case DQT_CS_CASH_MODIFY:{return Q_DQT_CS_CASH_MODIFY(rkResult);}break;
	case DQT_CS_ADD_GIFT:{return Q_DQT_CS_ADD_GIFT(rkResult);}break;
	case DQT_CS_GIFT_QUEYR:{return Q_DQT_CS_GIFT_QUEYR(rkResult);}break;
	case DQT_CS_MODIFY_VISABLE_RANK:{return Q_DQT_CS_MODIFY_VISABLE_RANK(rkResult);}break;
	case DQT_CS_CASH_LIMIT_ITEM_QUERY:{return Q_DQT_CS_CASH_LIMIT_ITEM_QUERY(rkResult);}break;
	case DQT_CS_CASH_LIMIT_ITEM_UPDATE_QUERY:{return Q_DQT_CS_CASH_LIMIT_ITEM_UPDATE_QUERY(rkResult);}break;

	case DQT_SELECT_CHARA_SKILL:{return Q_DQT_SELECT_CHARA_SKILL(rkResult);}break;

	case DQT_GET_EVENT_COUPON:{return Q_DQT_GET_EVENT_COUPON(rkResult);}break;

	case DQT_GET_LAST_RECVED_GIFT:{return Q_DQT_GET_LAST_RECVED_GIFT(rkResult);}break;
	case DQT_SAVE_PLAYER_EFFECT: { return Q_DQT_SAVE_PLAYER_EFFECT(rkResult); }break;
	case DQT_REMOVE_PET:{return Q_DQT_REMOVE_PET(rkResult);}break;
	case DQT_RENAME_PET:{return Q_DQT_RENAME_PET(rkResult);}break;
	case DQT_SETABIL_PET:{return Q_DQT_SETABIL_PET(rkResult);}break;
	case DQT_LOAD_PET_ITEM:{return Q_DQT_LOAD_PET_ITEM(rkResult);}break;
	case DQT_CASH_TRAN_COMMIT: { return Q_DQT_CASH_TRAN_COMMIT(rkResult); } break;
	case DQT_CASH_TRANSACTION: { return Q_DQT_CASH_TRANSACTION(rkResult); } break;
	case DQT_MYHOME_MODIFY: { return Q_DQT_MYHOME_MODIFY(rkResult); } break;
	case DQT_EVENT_CASHITEM_GIFT1:{return Q_DQT_EVENT_CASHITEM_GIFT1(rkResult);}break;

	case DQT_UPDATE_ACHIEVEMENT_FIST:{Q_DQT_UPDATE_ACHIEVEMENT_FIST(rkResult);}break;
	case DQT_SELECT_ACHIEVEMENT_RANK:{Q_DQT_SELECT_ACHIEVEMENT_RANK(rkResult);}break;
	case DQT_POST_GROUP_MAIL:{Q_DQT_POST_GROUP_MAIL(rkResult);}break;
	case DQT_SELECT_USER_QUEST_COMPLETE:{Q_DQT_SELECT_USER_QUEST_COMPLETE(rkResult);}break;
	case DQT_UPDATE_USER_QUEST_COMPLETE:{Q_DQT_UPDATE_USER_QUEST_COMPLETE(rkResult);}break;

	case DQT_UPDATE_SPECIFIC_REWARD:{Q_DQT_UPDATE_SPECIFIC_REWARD(rkResult);}break;

	case DQT_ADD_MYHOME:{Q_DQT_ADD_MYHOME(rkResult);}break;
	case DQT_MYHOME_REMOVE:{g_kRealmUserMgr.Q_DQT_MYHOME_REMOVE(rkResult);}break;
	case DQT_UPDATE_USER_JOBSKILL_INFO_NO_OP:{Q_DQT_UPDATE_USER_JOBSKILL_INFO_NO_OP(rkResult);}break;
	case DQT_NO_OP:{Q_DQT_NO_OP(rkResult);}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled DB Result Type[") << rkResult.QueryType() << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}break;
	}
	return S_OK;
}

HRESULT CALLBACK OnDB_EXECUTE_TRAN_Item(CEL::DB_RESULT_TRAN & rkResult)
{
	switch(rkResult.QueryType())
	{
	case DQT_UPDATE_USER_ITEM:{return Q_DQT_UPDATE_USER_ITEM(rkResult);}break;
	case DQT_POST_GET_MAIL_ITEM:{return Q_DQT_POST_GET_MAIL_ITEM(rkResult);}break;

	case DQT_UM_MARKET_OPEN:{return Q_DQT_UM_MARKET_OPEN(rkResult);}break;
	case DQT_UM_DEALING_READ:{return Q_DQT_UM_DEALING_READ(rkResult);}break;
	case DQT_UM_ARTICLE_BUY:{return Q_DQT_UM_ARTICLE_BUY(rkResult);}break;
	case DQT_UM_ARTICLE_REG:{return Q_DQT_UM_ARTICLE_REG(rkResult);}break;
	case DQT_UM_ARTICLE_DEREG:{return Q_DQT_UM_ARTICLE_DEREG(rkResult);}break;
	case DQT_UM_MODIFY_MARKET:{return Q_DQT_UM_MODIFY_MARKET(rkResult);}break;

	case DQT_CS_CASHSHOP:{return Q_DQT_CS_CASHSHOP(rkResult);}break;
	case DQT_CS_RECV_GIFT:{return Q_DQT_CS_RECV_GIFT(rkResult);}break;

	case DQT_CHAR_RESET_SKILL:{return Q_DQT_CHAR_RESET_SKILL(rkResult);}break;
	case DQT_HATCH_PET:{return Q_DQT_HATCH_PET(rkResult);}break;
	case DQT_SWAP_EMPORIA:{return g_kEmporiaMgr.Locked_Q_DQT_SWAP_EMPORIA(rkResult);}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled DB Result Type[") << rkResult.QueryType() << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return S_OK;
}

bool InitDB()
{
	bool bReturn = true;
	SReloadDef kReloadDef;

	g_kTblDataMgr.GetReloadDef(kReloadDef, PgControlDefMgr::EDef_ItemDefMgr);
	bReturn = bReturn && g_kControlDefMgr.Update(kReloadDef, PgControlDefMgr::EDef_ItemDefMgr);
	return bReturn;
}

HRESULT Q_DQT_UPDATE_QUEST_CLEAR_COUNT( CEL::DB_RESULT &rkResult )
{
	if( CEL::DR_NO_RESULT == rkResult.eRet
	||  CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		while( rkResult.vecArray.end() != itor )
		{
			int iError = 0;
			(*itor).Pop(iError);	++itor;
			if( iError )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			return S_OK;
		}		
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_INIT_QUEST_CLEAR_COUNT( CEL::DB_RESULT &rkResult )
{
	if( CEL::DR_NO_RESULT == rkResult.eRet
	||  CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		while( rkResult.vecArray.end() != itor )
		{
			int iError = 0;
			(*itor).Pop(iError);	++itor;
			if( iError )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			return S_OK;
		}		
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UPDATE_USER_ITEM(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		while( rkResult.vecArray.end() != itor )
		{
			int iIsSuccess = 0;

			(*itor).Pop(iIsSuccess);					++itor;

			if(iIsSuccess)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			return S_OK;
		}
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UPDATE_USER_ITEM(CEL::DB_RESULT_TRAN &rkContResult)
{
	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
		{
			switch(rkResult.QueryType())
			{
			case DQT_POST_ADD_MAIL:
				{
					if(S_OK != Q_DQT_POST_ADD_MAIL(rkResult))
					{
						return E_FAIL;
					}
				}break;
			default:
				{
					CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

					while( rkResult.vecArray.end() != itor )
					{
						int iIsSuccess = 0;

						(*itor).Pop(iIsSuccess);					++itor;

						if(iIsSuccess)
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
							return E_FAIL;
						}

						break;
					}
				}break;
			}
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT Q_DQT_UPDATE_ING_QUEST(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UPDATE_END_QUEST(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UPDATE_DATE_CONTENTS(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UPDATE_USER_FIELD(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}


bool Q_DQT_CLEAR_CONNECTION_CHANNEL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
		INFO_LOG( BM::LOG_LV7, __FL__ << _T("Success") );
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		int iErrorCode = 0;

		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iErrorCode);

			if(iErrorCode == 1)	//뭔가 문제 있다
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}			
			INFO_LOG( BM::LOG_LV7, _T("Sucess Clear User Connection Channel") );
		}
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void READ_SEND_MAIL_INFO(SRecvMailInfo & kMailInfo, CEL::DB_DATA_ARRAY::const_iterator & itor)
{
	(*itor).Pop(kMailInfo.kMailGuid);		++itor;
	(*itor).Pop(kMailInfo.i64MailIndex);	++itor;
	(*itor).Pop(kMailInfo.kFromName);		++itor;
	(*itor).Pop(kMailInfo.kMailTitle);		++itor;
	(*itor).Pop(kMailInfo.kMailText);		++itor;
	(*itor).Pop(kMailInfo.kMailState);		++itor;
	(*itor).Pop(kMailInfo.bPamentType);		++itor;
	(*itor).Pop(kMailInfo.i64Money);		++itor;
	(*itor).Pop(kMailInfo.kLimitTime);		++itor;

	SDBItemData kDBItemData;
	(*itor).Pop(kDBItemData.ItemGuid);		++itor;
	(*itor).Pop(kDBItemData.OwnerGuid);		++itor;
	(*itor).Pop(kDBItemData.ItemNo);		++itor;
	(*itor).Pop(kDBItemData.Count);			++itor;
	(*itor).Pop(kDBItemData.Enchant_01);	++itor;
	(*itor).Pop(kDBItemData.Enchant_02);	++itor;
	(*itor).Pop(kDBItemData.Enchant_03);	++itor;
	(*itor).Pop(kDBItemData.Enchant_04);	++itor;
	(*itor).Pop(kDBItemData.State);			++itor;
	(*itor).Pop(kDBItemData.CreateDate);	++itor;
	std::wstring kMailName;
	(*itor).Pop(kMailName);					++itor;// 메일 텍스트를 임의로 뿌려 주고자 한다면 이 이름으로 바꾸어 준다.

	if(false == kMailName.empty())
	{
		kMailInfo.kFromName = kMailName;
	}

	PgBase_Item kItem;
	SItemPos kItemPos;
	CastSDBItemDataToSItem(kDBItemData,kMailInfo.kItem,kItemPos);

	SClassKey kPetClassKey;
	std::wstring wstrPetName;
	__int64 i64PetExp = 0i64;
	int iPetMP = 0;
	int iPetHealth = 0;
	int iPetMental = 0;
	char cPetColor = 0;
	PgItem_PetInfo::SStateValue kStateValue[PgItem_PetInfo::MAX_SKILL_SIZE];
	itor->Pop( kPetClassKey.iClass );		++itor;
	itor->Pop( kPetClassKey.nLv );			++itor;
	itor->Pop( wstrPetName );				++itor;
	itor->Pop( i64PetExp );					++itor;
	itor->Pop( iPetMP );					++itor;
	itor->Pop( iPetHealth );				++itor;
	itor->Pop( iPetMental );				++itor;
	itor->Pop( cPetColor );					++itor;
	for ( size_t i = 0; i<PgItem_PetInfo::MAX_SKILL_SIZE; ++i )
	{
		itor->Pop( kStateValue[i] );		++itor;
	}

	SExpCard kExpCard;
	__int64 i64CurExp = 0,
			i64MaxExp = 0;
	short	kExpPer = 0;

	(*itor).Pop(i64CurExp);					++itor;
	(*itor).Pop(i64MaxExp);					++itor;
	(*itor).Pop(kExpPer);					++itor;

	kExpCard.CurExp(i64CurExp);
	kExpCard.MaxExp(i64MaxExp);
	kExpCard.ExpPer(kExpPer);

	PgItem_PetInfo *pkPetInfo = NULL;
	if (true == kMailInfo.kItem.GetExtInfo( pkPetInfo ) )
	{
		pkPetInfo->ClassKey( kPetClassKey );
		pkPetInfo->Name( wstrPetName );
		pkPetInfo->Exp( i64PetExp );
		pkPetInfo->SetAbil( AT_MP, iPetMP );
		pkPetInfo->SetState_Health( iPetHealth );
		pkPetInfo->SetState_Mental( iPetMental );
		pkPetInfo->SetAbil( AT_COLOR_INDEX, static_cast<int>(cPetColor) );

		__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::MINUTE );
		for ( size_t i = 0; i<PgItem_PetInfo::MAX_SKILL_SIZE; ++i )
		{
			__int64 const i64StateTime = static_cast<__int64>(kStateValue[i].Time()) + PgItem_PetInfo::SKILL_BASE_TIME;
			if ( i64NowTime < i64StateTime )
			{
				pkPetInfo->SetSkill( i, kStateValue[i] );
			}
		}
	}			

	if(false == kExpCard.IsEmpty())
	{
		kMailInfo.kItem.Set(kExpCard);
	}
}

HRESULT Q_DQT_POST_NEW_MAIL_NOTY(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		if( rkResult.vecArray.end() != itor )
		{
			SRecvMailInfo kMailInfo;
			READ_SEND_MAIL_INFO(kMailInfo,itor);
			BM::Stream kNotiPacket(PT_I_M_POST_NOTI_NEW_MAIL);
			kNotiPacket.Push(rkResult.QueryOwner());
			kMailInfo.WriteToPacket(kNotiPacket);
			SendToPacketHandler(kNotiPacket);
		}
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_POST_ADD_MAIL(CEL::DB_RESULT &rkResult)
{
	BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_SEND);
	kPacket.Push(rkResult.QueryOwner());

	EORDERType kExtType = ORDER_NONE;
	BM::GUID kGMCmdGuid;
	rkResult.contUserData.Pop(kGMCmdGuid);
	rkResult.contUserData.Pop(kExtType);
	
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		if( rkResult.vecArray.end() != itor )
		{
			int iError;

			(*itor).Pop(iError);	++itor;						// 에러 코드가 두곳에서 리턴된다. 아이템 처리/ 메일 처리
			if(iError)
			{
				if(ORDER_CREATE_MAIL == kExtType)
				{
					g_kGMProcessMgr.SendOrderState(kGMCmdGuid, OS_RESERVE);
				}

				kPacket.Push(static_cast<EPostMailSendResult>(iError));
				g_kRealmUserMgr.Locked_SendToUserGround(rkResult.QueryOwner(),kPacket,false,true);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			kPacket.Push(PMSR_SUCCESS);
			g_kRealmUserMgr.Locked_SendToUserGround(rkResult.QueryOwner(),kPacket,false,true);

			BM::GUID kToGuid,kMailGuid;

			(*itor).Pop(kToGuid);						++itor;
			(*itor).Pop(kMailGuid);						++itor;
			
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_POST_NEW_MAIL_NOTY, L"EXEC [dbo].[UP_Post_Get_UserMailInfo]");
			kQuery.InsertQueryTarget(rkResult.QueryOwner());
			kQuery.PushStrParam(kMailGuid);
			kQuery.QueryOwner(kToGuid);
			g_kCoreCenter.PushQuery(kQuery);


			if(ORDER_CREATE_MAIL == kExtType)
			{
				g_kGMProcessMgr.SendOrderState(kGMCmdGuid, OS_DONE);
			}
		}
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}

	kPacket.Push(PMSR_DB_ERROR);
	g_kRealmUserMgr.Locked_SendToUserGround(rkResult.QueryOwner(),kPacket,false,true);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));

	if(ORDER_CREATE_MAIL == kExtType)
	{
		g_kGMProcessMgr.SendOrderState(kGMCmdGuid, OS_RESERVE);
	}

	return E_FAIL;
}

int const POST_GET_MAIL_RESULT_COUNT = 3;

HRESULT Q_DQT_POST_GET_MAIL(CEL::DB_RESULT &rkResult)
{
	BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_RECV);
	kPacket.Push(rkResult.QueryOwner());

	SGroundKey kGndKey;
	SERVER_IDENTITY kSI;
	rkResult.contUserData.Pop(kGndKey);
	kSI.ReadFromPacket(rkResult.contUserData);

	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		if(POST_GET_MAIL_RESULT_COUNT > rkResult.vecResultCount.size())
		{
			INFO_LOG(BM::LOG_LV6,__FL__ << _T("MAIL QUERY ERROR !! INVALID RESULT COUNT : ") << rkResult.vecResultCount.size());
			return E_FAIL;
		}

		kPacket.Push(PMRR_SUCCESS);
		
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		int iMailCount;
		(*itor).Pop(iMailCount);		++itor;
		kPacket.Push(static_cast<BYTE>(iMailCount));
		(*itor).Pop(iMailCount);		++itor;
		kPacket.Push(static_cast<BYTE>(iMailCount));

		CONT_RECV_MAIL_LIST kCont;

		while( rkResult.vecArray.end() != itor )
		{
			SRecvMailInfo kMailInfo;
			READ_SEND_MAIL_INFO(kMailInfo,itor);
			kCont.push_back(kMailInfo);
		}

		kPacket.Push(kCont.size());
		CONT_RECV_MAIL_LIST::iterator mitor = kCont.begin();
		while(mitor != kCont.end())
		{
			(*mitor).WriteToPacket(kPacket); ++mitor;
		}

		::SendToGround(kSI,kGndKey,kPacket);
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}

	kPacket.Push(PMRR_DB_ERROR);
	::SendToGround(kSI,kGndKey,kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_POST_MODIFY_MAIL(CEL::DB_RESULT &rkResult)
{
	BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MODIFY);
	kPacket.Push(rkResult.QueryOwner());

	SGroundKey kGndKey;
	SERVER_IDENTITY kSI;
	rkResult.contUserData.Pop(kGndKey);
	kSI.ReadFromPacket(rkResult.contUserData);
	BM::GUID kReservedMailGuId;
	rkResult.contUserData.Pop(kReservedMailGuId);
	g_kPostMgr.Locked_ReleaseMail(kReservedMailGuId);

	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		if(itor != rkResult.vecArray.end())
		{
			int iError;
			(*itor).Pop(iError);		++itor;	
			kPacket.Push(static_cast<EPostMailModifyResult>(iError));

			if(iError)
			{
				::SendToGround(kSI,kGndKey,kPacket);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			EPostMailModifyType kState,kRetState;
			__int64 i64MailIndex;
			BM::DBTIMESTAMP_EX kDateTime;

			(*itor).Pop(i64MailIndex);	++itor;	
			(*itor).Pop(kRetState);		++itor;
			(*itor).Pop(kDateTime);		++itor;
			kPacket.Push(i64MailIndex);
			kPacket.Push(kRetState);
			kPacket.Push(kDateTime);

			::SendToGround(kSI,kGndKey,kPacket);

			BM::GUID kToGuid,kMailGuid;
			(*itor).Pop(kState);	++itor;
			(*itor).Pop(kToGuid);	++itor;
			(*itor).Pop(kMailGuid);	++itor;

			if(PMMT_RETURN == kState)
			{
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_POST_NEW_MAIL_NOTY, L"EXEC [dbo].[UP_Post_Get_UserMailInfo]");
				kQuery.InsertQueryTarget(kToGuid);
				kQuery.PushStrParam(kMailGuid);
				kQuery.QueryOwner(kToGuid);
				g_kCoreCenter.PushQuery(kQuery);
			}

			PgDoc_Player rkCopyPlayer;
			if(g_kRealmUserMgr.Locked_GetDoc_Player(rkResult.QueryOwner(),false,rkCopyPlayer))
			{
				PgLogCont kLogCont(ELogMain_Contents_Mail,ELogSub_None);
				kLogCont.LogKey(BM::GUID::Create());
				kLogCont.MemberKey(rkCopyPlayer.GetMemberGUID());
				kLogCont.UID(rkCopyPlayer.UID());
				kLogCont.CharacterKey(rkCopyPlayer.GetID());
				kLogCont.ID(rkCopyPlayer.MemberID());
				kLogCont.Name(rkCopyPlayer.Name());
				kLogCont.RealmNo(g_kProcessCfg.RealmNo());
				kLogCont.ChannelNo(rkCopyPlayer.GetChannel());
				kLogCont.Class(rkCopyPlayer.GetAbil(AT_CLASS));
				kLogCont.Level(rkCopyPlayer.GetAbil(AT_LEVEL));
				kLogCont.GroundNo(rkCopyPlayer.GroundKey().GroundNo());

				switch(kState)
				{
				case PMMT_READ:
					{
						kLogCont.LogSubType(ELogSub_Mail_Read);
						PgLog kLog(ELOrderMain_Mail,ELOrderSub_Read);
						kLog.Set(2,kMailGuid.str());
						kLogCont.Add(kLog);
					}break;
				case PMMT_DELETE:
					{
						kLogCont.LogSubType(ELogSub_Mail_Delete);
						PgLog kLog(ELOrderMain_Mail,ELOrderSub_Delete);
						kLog.Set(2,kMailGuid.str());
						kLogCont.Add(kLog);
					}break;
				case PMMT_RETURN:
					{
						kLogCont.LogSubType(ELogSub_Mail_Return);
						PgLog kLog(ELOrderMain_Mail,ELOrderSub_Return);
						kLog.Set(2,kMailGuid.str());
						kLogCont.Add(kLog);
					}break;
				}

				kLogCont.Commit();
			}
			return S_OK;
		}
	}

	kPacket.Push(PMSR_DB_ERROR);
	::SendToGround(kSI,kGndKey,kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_POST_GET_MAIL_ITEM_RESERVE(CEL::DB_RESULT &rkResult)
{
	SGroundKey kGndKey;
	EItemModifyParentEventType kCause;
	BM::GUID kMailGuid;
	SERVER_IDENTITY kSI;

	rkResult.contUserData.Pop(kGndKey);
	rkResult.contUserData.Pop(kCause);
	rkResult.contUserData.Pop(kMailGuid);
	kSI.ReadFromPacket(rkResult.contUserData);

	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MODIFY);
		kPacket.Push(rkResult.QueryOwner());
		kPacket.Push(PMSR_DB_ERROR);
		SendToGround(kSI,kGndKey,kPacket);
		g_kPostMgr.Locked_ReleaseMail(kMailGuid);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	if(itor == rkResult.vecArray.end())
	{
		BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MODIFY);
		kPacket.Push(rkResult.QueryOwner());
		kPacket.Push(PMSR_DB_ERROR);
		SendToGround(kSI,kGndKey,kPacket);
		g_kPostMgr.Locked_ReleaseMail(kMailGuid);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	PgBase_Item kItem;
	BYTE	bPaymentBit,
			bPayMentType;
	__int64 i64Money;
	SDBItemData kDBItemData;
	BM::GUID kToGuId;
	BYTE	bAnnexBit;

	(*itor).Pop(bPaymentBit);				++itor;
	(*itor).Pop(bPayMentType);				++itor;
	(*itor).Pop(i64Money);					++itor;
	(*itor).Pop(kDBItemData.ItemGuid);		++itor;
	(*itor).Pop(kDBItemData.OwnerGuid);		++itor;
	(*itor).Pop(kDBItemData.InvType);		++itor;
	(*itor).Pop(kDBItemData.InvPos);		++itor;
	(*itor).Pop(kDBItemData.ItemNo);		++itor;
	(*itor).Pop(kDBItemData.Count);			++itor;
	(*itor).Pop(kDBItemData.Enchant_01);	++itor;
	(*itor).Pop(kDBItemData.Enchant_02);	++itor;
	(*itor).Pop(kDBItemData.Enchant_03);	++itor;
	(*itor).Pop(kDBItemData.Enchant_04);	++itor;
	(*itor).Pop(kDBItemData.State);			++itor;
	(*itor).Pop(kDBItemData.CreateDate);	++itor;
	(*itor).Pop(bAnnexBit);					++itor;
	(*itor).Pop(kToGuId);					++itor;

	SClassKey kPetClassKey;
	std::wstring wstrPetName;
	__int64 i64PetExp = 0i64;
	int iPetMP = 0;
	int iPetHealth = 0;
	int iPetMental = 0;
	char cPetColor = 0;
	PgItem_PetInfo::SStateValue kStateValue[PgItem_PetInfo::MAX_SKILL_SIZE];
	itor->Pop( kPetClassKey.iClass );	++itor;
	itor->Pop( kPetClassKey.nLv );		++itor;
	itor->Pop( wstrPetName );			++itor;
	itor->Pop( i64PetExp );				++itor;
	itor->Pop( iPetMP );				++itor;
	itor->Pop( iPetHealth );			++itor;
	itor->Pop( iPetMental );			++itor;
	itor->Pop( cPetColor );				++itor;
	for ( size_t i = 0; i<PgItem_PetInfo::MAX_SKILL_SIZE; ++i )
	{
		itor->Pop( kStateValue[i] );	++itor;
	}

	SExpCard kExpCard;
	__int64 i64CurExp = 0,
			i64MaxExp = 0;
	short	kExpPer = 0;

	(*itor).Pop(i64CurExp);					++itor;
	(*itor).Pop(i64MaxExp);					++itor;
	(*itor).Pop(kExpPer);					++itor;

	kExpCard.CurExp(i64CurExp);
	kExpCard.MaxExp(i64MaxExp);
	kExpCard.ExpPer(kExpPer);

	BM::DBTIMESTAMP_EX kLimitTime;
	SMonsterCardTimeLimit kMonsterCard;
	(*itor).Pop(kLimitTime);				++itor;
	kMonsterCard.LimitTime(static_cast<BM::PgPackedTime>(kLimitTime));

	if(bAnnexBit || kToGuId != rkResult.QueryOwner())
	{
		g_kPostMgr.Locked_ReleaseMail(kMailGuid);
		BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MODIFY);
		kPacket.Push(rkResult.QueryOwner());
		kPacket.Push(PMSR_NOT_FOUND_ANNEX);
		SendToGround(kSI,kGndKey,kPacket);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	SItemPos kItemPos;
	CastSDBItemDataToSItem(kDBItemData,kItem,kItemPos);

	PgItem_PetInfo *pkPetInfo = NULL;
	if (true == kItem.GetExtInfo( pkPetInfo ) )
	{
		pkPetInfo->ClassKey( kPetClassKey );
		pkPetInfo->Name( wstrPetName );
		pkPetInfo->Exp( i64PetExp );
		pkPetInfo->SetAbil( AT_MP, iPetMP );
		pkPetInfo->SetState_Health( iPetHealth );
		pkPetInfo->SetState_Mental( iPetMental );
		pkPetInfo->SetAbil( AT_COLOR_INDEX, static_cast<int>(cPetColor) );

		__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::MINUTE );
		for ( size_t i = 0; i<PgItem_PetInfo::MAX_SKILL_SIZE; ++i )
		{
			__int64 const i64StateTime = static_cast<__int64>(kStateValue[i].Time()) + PgItem_PetInfo::SKILL_BASE_TIME;
			if ( i64NowTime < i64StateTime )
			{
				pkPetInfo->SetSkill( i, kStateValue[i] );
			}
		}
	}			

	if(false == kExpCard.IsEmpty())
	{
		kItem.Set(kExpCard);
	}

	if(false == kMonsterCard.IsEmpty())
	{
		kItem.Set(kMonsterCard);
	}

	CONT_PLAYER_MODIFY_ORDER kContOrder;

	if(bPaymentBit)
	{
		if(i64Money > 0)
		{
			SPMOD_Add_Money kAddMoney(-i64Money);
			SPMO kIMO(IMET_ADD_MONEY, rkResult.QueryOwner(),kAddMoney);
			kContOrder.push_back(kIMO);
		}
	}
	else if(!bPayMentType)
	{
		if(i64Money > 0)
		{
			SPMOD_Add_Money kAddMoney(i64Money);
			SPMO kIMO(IMET_ADD_MONEY, rkResult.QueryOwner(),kAddMoney);
			kContOrder.push_back(kIMO);
		}
	}

	if(BM::GUID::IsNotNull(kDBItemData.ItemGuid))
	{
		SPMOD_DB2Inv kAddItem(kItem,SItemPos(IT_POST,0));
		SPMO kIMO(IMET_MODIFY_DB2INV,rkResult.QueryOwner(),kAddItem);
		kContOrder.push_back(kIMO);
	}

	BM::Stream kPacket;
	kPacket.Push(kCause);
	kPacket.Push(rkResult.QueryOwner());
	kPacket.Push(kMailGuid);
	kPacket.Push(bPaymentBit);
	kPacket.Push(kGndKey);
	kSI.WriteToPacket(kPacket);

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(rkResult.QueryOwner());
	pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
	pkActionOrder->kCause = kCause;
	pkActionOrder->kContOrder = kContOrder;
	pkActionOrder->kPacket2nd = kPacket;
	g_kJobDispatcher.VPush(pkActionOrder);

	return S_OK;
}

HRESULT Q_DQT_POST_GET_MAIL_ITEM(CEL::DB_RESULT &rkResult)
{
	SGroundKey kGndKey;
	SERVER_IDENTITY kSI;
	BM::GUID kReservedMailGuId;
	rkResult.contUserData.Pop(kReservedMailGuId);
	rkResult.contUserData.Pop(kGndKey);
	kSI.ReadFromPacket(rkResult.contUserData);

	g_kPostMgr.Locked_ReleaseMail(kReservedMailGuId);

	BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MODIFY);
	kPacket.Push(rkResult.QueryOwner());

	if(	CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if( rkResult.vecArray.end() != itor )
		{
			int iError;
			__int64 i64MailIndex;
			EPostMailModifyType kState,kRetState;
			BM::DBTIMESTAMP_EX kDateTime;

			(*itor).Pop(iError);	++itor;
			if(PMMR_SUCCESS != iError)
			{
				kPacket.Push(static_cast<ePostMailModifyResult>(iError));
				::SendToGround(kSI,kGndKey,kPacket);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			(*itor).Pop(i64MailIndex);		++itor;
			(*itor).Pop(kRetState);			++itor;
			(*itor).Pop(kDateTime);			++itor;

			BM::GUID kToGuid,kMailGuid;
			(*itor).Pop(kState);			++itor;
			(*itor).Pop(kToGuid);			++itor;
			(*itor).Pop(kMailGuid);			++itor;

			if((PMMT_PAYMENT & kState) == PMMT_PAYMENT)
			{
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_POST_NEW_MAIL_NOTY, L"EXEC [dbo].[UP_Post_Get_UserMailInfo]");
				kQuery.InsertQueryTarget(kToGuid);
				kQuery.PushStrParam(kMailGuid);
				kQuery.QueryOwner(kToGuid);
				g_kCoreCenter.PushQuery(kQuery);
			}

			kPacket.Push(PMMR_SUCCESS);
			kPacket.Push(i64MailIndex);
			kPacket.Push(kRetState);
			kPacket.Push(kDateTime);

			::SendToGround(kSI,kGndKey,kPacket);
			return S_OK;
		}
	}

	kPacket.Push(PMMR_DB_ERROR);
	::SendToGround(kSI,kGndKey,kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_POST_GET_MAIL_ITEM(CEL::DB_RESULT_TRAN &rkContResult)
{
	SGroundKey kGndKey;
	SERVER_IDENTITY kSI;
	BM::GUID kReservedMailGuId;
	rkContResult.contUserData.Pop(kReservedMailGuId);
	rkContResult.contUserData.Pop(kGndKey);
	kSI.ReadFromPacket(rkContResult.contUserData);

	g_kPostMgr.Locked_ReleaseMail(kReservedMailGuId);

	BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MODIFY);
	kPacket.Push(rkContResult.QueryOwner());

	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(	CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
		{
			switch(rkResult.QueryType())
			{
			case DQT_POST_GET_MAIL_ITEM:
				{
					CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
					if( rkResult.vecArray.end() != itor )
					{
						int iError;
						__int64 i64MailIndex;
						EPostMailModifyType kState,kRetState;
						BM::DBTIMESTAMP_EX kDateTime;

						(*itor).Pop(iError);	++itor;
						if(PMMR_SUCCESS != iError)
						{
							kPacket.Push(static_cast<ePostMailModifyResult>(iError));
							::SendToGround(kSI,kGndKey,kPacket);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
							return E_FAIL;
						}

						(*itor).Pop(i64MailIndex);		++itor;
						(*itor).Pop(kRetState);			++itor;
						(*itor).Pop(kDateTime);			++itor;

						BM::GUID kToGuid,kMailGuid;
						(*itor).Pop(kState);			++itor;
						(*itor).Pop(kToGuid);			++itor;
						(*itor).Pop(kMailGuid);			++itor;

						if((PMMT_PAYMENT & kState) == PMMT_PAYMENT)
						{
							CEL::DB_QUERY kQuery(DT_PLAYER, DQT_POST_NEW_MAIL_NOTY, L"EXEC [dbo].[UP_Post_Get_UserMailInfo]");
							kQuery.InsertQueryTarget(kToGuid);
							kQuery.PushStrParam(kMailGuid);
							kQuery.QueryOwner(kToGuid);
							g_kCoreCenter.PushQuery(kQuery);
						}

						kPacket.Push(PMMR_SUCCESS);
						kPacket.Push(i64MailIndex);
						kPacket.Push(kRetState);
						kPacket.Push(kDateTime);
					}
				}break;
			case DQT_UPDATE_USER_ITEM:
				{
					CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
					if( rkResult.vecArray.end() != itor )
					{
						int iError;

						(*itor).Pop(iError);	++itor;
						if(iError)
						{
							kPacket.Push(static_cast<ePostMailModifyResult>(iError));
							::SendToGround(kSI,kGndKey,kPacket);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
							return E_FAIL;
						}
					}
				}break;
			}
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
		}
	}

	::SendToGround(kSI,kGndKey,kPacket);

	return S_OK;
}

HRESULT Q_DQT_POST_GET_MAIL_MIN(CEL::DB_RESULT &rkResult)
{
	SGroundKey kGndKey;
	SERVER_IDENTITY kSI;
	rkResult.contUserData.Pop(kGndKey);
	kSI.ReadFromPacket(rkResult.contUserData);

	BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MIN);
	kPacket.Push(rkResult.QueryOwner());

	__int64 i64MinIndex = 0;

	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if( rkResult.vecArray.end() != itor )
		{
			(*itor).Pop(i64MinIndex);
			kPacket.Push(i64MinIndex);
			::SendToGround(kSI,kGndKey,kPacket);
		}
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}

	kPacket.Push(i64MinIndex);
	::SendToGround(kSI,kGndKey,kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_POST_CHECK_EANBLE_SEND(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		SERVER_IDENTITY kSI;
		SGroundKey kGndKey;
		
		BM::GUID kOwnerGuid;
		CONT_PLAYER_MODIFY_ORDER kContOrder;
		EItemModifyParentEventType kCause;

		rkResult.contUserData.Pop(kCause);
		rkResult.contUserData.Pop(kOwnerGuid);
		kContOrder.ReadFromPacket(rkResult.contUserData);

		kGndKey.ReadFromPacket(rkResult.contUserData);
		kSI.ReadFromPacket(rkResult.contUserData);

		BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_SEND);
		kPacket.Push(kOwnerGuid);
		
		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		if(iter == rkResult.vecArray.end())
		{
			kPacket.Push(PMSR_DB_ERROR);
			::SendToGround(kSI,kGndKey,kPacket);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		int iError = 0;
		(*iter).Pop(iError); ++iter;

		if(iError)
		{
			kPacket.Push(static_cast<ePostMailSendResult>(iError));
			::SendToGround(kSI,kGndKey,kPacket);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		BM::GUID kToGuId;
		(*iter).Pop(kToGuId); ++iter;

		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(kOwnerGuid);
		pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
		pkActionOrder->kCause = kCause;
		pkActionOrder->kContOrder = kContOrder;
		g_kJobDispatcher.VPush(pkActionOrder);

		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UM_ARTICLE_REG(CEL::DB_RESULT_TRAN &rkContResult)
{
	BM::Stream kAnsPacket(PT_M_C_UM_ANS_ARTICLE_REG);

	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
		{
			CEL::DB_DATA_ARRAY::const_iterator retitor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != retitor )
			{
				int iIsSuccess = 0;

				(*retitor).Pop(iIsSuccess);

				if(iIsSuccess)
				{
					kAnsPacket.Push(UMR_DB_ERROR);
					g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);
					CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}

			continue;
		}

		kAnsPacket.Push(UMR_DB_ERROR);
		g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	kAnsPacket.Push(UMR_SUCCESS);
	kAnsPacket.Push(rkContResult.contUserData);
	g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);
	return S_OK;
}

HRESULT Q_DQT_UM_ARTICLE_ITEM_RESERVE(CEL::DB_RESULT &rkResult)
{
	return S_OK;
}

HRESULT Q_DQT_UM_ARTICLE_DEREG(CEL::DB_RESULT_TRAN &rkContResult)
{
	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
		{
			CEL::DB_DATA_ARRAY::const_iterator retitor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != retitor )
			{
				int iIsSuccess = 0;

				(*retitor).Pop(iIsSuccess);

				if(iIsSuccess)
				{
					CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}

			continue;
		}

		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	BM::Stream kPacket(PT_M_C_UM_ANS_ARTICLE_DEREG);
	kPacket.Push(UMR_SUCCESS);
	g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);
	return S_OK;
}

HRESULT Q_DQT_UM_ARTICLE_BUY(CEL::DB_RESULT_TRAN &rkContResult)
{
	BM::Stream kAnsPacket(PT_M_C_UM_ANS_ARTICLE_BUY);

	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
		{
			CEL::DB_DATA_ARRAY::const_iterator retitor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != retitor )
			{
				int iIsSuccess = 0;

				(*retitor).Pop(iIsSuccess);

				if(iIsSuccess)
				{
					kAnsPacket.Push(UMR_DB_ERROR);
					g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);
					CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}

			continue;
		}

		kAnsPacket.Push(UMR_DB_ERROR);
		g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	BM::GUID kArticleGuid;
	WORD wBuyItemNum;

	rkContResult.contUserData.Pop(kArticleGuid);
	rkContResult.contUserData.Pop(wBuyItemNum);

	kAnsPacket.Push(UMR_SUCCESS);
	kAnsPacket.Push(kArticleGuid);
	kAnsPacket.Push(wBuyItemNum);
	g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);
	return S_OK;
}

HRESULT Q_DQT_UM_ARTICLE_QUERY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
	{
		return S_OK;
	}

	CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UM_MY_ARTICLE_QUERY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
	{
		return S_OK;
	}

	CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UM_DEALING_QUERY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
	{
		return S_OK;
	}

	CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UM_DEALING_ITEM_RESERVE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
	{
		return S_OK;
	}

	CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UM_DEALING_READ(CEL::DB_RESULT_TRAN &rkContResult)
{
	BM::Stream kPacket(PT_M_C_UM_ANS_DEALINGS_READ);

	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
		{
			CEL::DB_DATA_ARRAY::const_iterator retitor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != retitor )
			{
				int iIsSuccess = 0;

				(*retitor).Pop(iIsSuccess);

				if(iIsSuccess)
				{
					kPacket.Push(UMR_DB_ERROR);
					g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);

					CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}

			continue;
		}

		kPacket.Push(UMR_DB_ERROR);
		g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	kPacket.Push(UMR_SUCCESS);
	g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);
	return S_OK;
}

HRESULT Q_DQT_UM_GO_TIME_PROC(CEL::DB_RESULT & rkResult)
{
	return S_OK;
}

HRESULT Q_DQT_UM_MODIFY_MARKET_STATE(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UM_DEV_SET_ARTICLE_STATE(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UM_MARKET_OPEN(CEL::DB_RESULT_TRAN &rkContResult)
{
	BM::Stream kAnsPacket(PT_M_C_UM_ANS_MARKET_OPEN);

	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
		{
			CEL::DB_DATA_ARRAY::const_iterator retitor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != retitor )
			{
				int iIsSuccess = 0;

				(*retitor).Pop(iIsSuccess);

				if(iIsSuccess)
				{
					kAnsPacket.Push(UMR_DB_ERROR);
					g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);

					CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Failed Market Error Code [") << iIsSuccess << _T("]") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}

			continue;
		}

		kAnsPacket.Push(UMR_DB_ERROR);
		g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);

		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Failed Query Error Code [") << rkResult.eRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	int iMarketOnlineTime = 0,
		iMarketOfflineTime = 0;

	rkContResult.contUserData.Pop(iMarketOnlineTime);
	rkContResult.contUserData.Pop(iMarketOfflineTime);

	PgOpenMarket kMarket;
	kMarket.ReadFromPacket_MarketInfo(rkContResult.contUserData);

	g_kRealmUserMgr.Locked_InsertMarket(kMarket);

	kAnsPacket.Push(UMR_SUCCESS);
	kAnsPacket.Push(iMarketOnlineTime);
	kAnsPacket.Push(iMarketOfflineTime);
	g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);

	return S_OK;
}

HRESULT Q_DQT_UM_MODIFY_MARKET(CEL::DB_RESULT_TRAN & rkContResult)
{
	BM::Stream kAnsPacket(PT_M_C_UM_ANS_USE_MARKET_MODIFY_ITEM);

	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
		{
			CEL::DB_DATA_ARRAY::const_iterator retitor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != retitor )
			{
				int iIsSuccess = 0;

				(*retitor).Pop(iIsSuccess);

				if(iIsSuccess)
				{
					kAnsPacket.Push(UMR_DB_ERROR);
					g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);

					CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}

			continue;
		}

		kAnsPacket.Push(UMR_DB_ERROR);
		g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	__int64 i64OnlineTime,
			i64OfflineTime;

	int		iMarketHP;

	rkContResult.contUserData.Pop(i64OnlineTime);
	rkContResult.contUserData.Pop(i64OfflineTime);
	rkContResult.contUserData.Pop(iMarketHP);

	kAnsPacket.Push(UMR_SUCCESS);
	kAnsPacket.Push(i64OnlineTime);
	kAnsPacket.Push(i64OfflineTime);
	kAnsPacket.Push(iMarketHP);
	g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kAnsPacket,false);

	return S_OK;
}

HRESULT Q_DQT_CS_CASH_QUERY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		if(iter != rkResult.vecArray.end())
		{
			BM::GUID		kMemberGuId;
			__int64			i64Cash = 0;
			__int64			i64Bonus = 0;
			(*iter).Pop(kMemberGuId);		++iter;
			(*iter).Pop(i64Cash);			++iter;
			(*iter).Pop(i64Bonus);			++iter;

			PgTranPointer kTran(rkResult.contUserData);
			PgTransaction* pkTran = kTran.GetTran();
			if (pkTran != NULL)
			{
				kTran.RecvCurrentCash(CSR_SUCCESS, i64Cash, i64Bonus);
				return S_OK;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Transaction information is NULL"));
		}
	}

	g_kRealmUserMgr.Locked_LeaveEvent(rkResult.QueryOwner());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_CS_CASH_LIMIT_ITEM_QUERY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator iter = rkResult.vecArray.begin();
		CEL::DB_RESULT_COUNT::const_iterator return_iter = rkResult.vecResultCount.begin();

		if((iter != rkResult.vecArray.end()) && (return_iter != rkResult.vecResultCount.end()))
		{
			int iEntityCount = (*return_iter); ++return_iter;

			int iItemTotalCount = 0;
			int iItemLimitCount = 0;
			int iItemBuyLimitCount = 0;
			for(int i = 0;i < iEntityCount;i++)
			{
				(*iter).Pop(iItemTotalCount);							++iter;
				(*iter).Pop(iItemLimitCount);							++iter;
				(*iter).Pop(iItemBuyLimitCount);						++iter;
			}

			iEntityCount = (*return_iter); ++return_iter;
			int iItemBuyCount = 0;
			for(int i = 0;i < iEntityCount;i++)
			{
				(*iter).Pop(iItemBuyCount);								++iter;
			}

			iEntityCount = (*return_iter); ++return_iter;

			CONT_CASH_SHOP_ITEM_LIMITSELL kContItemLimitSell;

			for(int i = 0;i < iEntityCount;i++)
			{
				int iItemIdx = 0;
				CONT_CASH_SHOP_ITEM_LIMITSELL::mapped_type kItemLimitSell;

				(*iter).Pop(iItemIdx);									++iter;
				(*iter).Pop(kItemLimitSell.iItemTotalCount);			++iter;
				(*iter).Pop(kItemLimitSell.iItemLimitCount);			++iter;
				(*iter).Pop(kItemLimitSell.ItemBuyLimitCount);			++iter;

				auto kPair = kContItemLimitSell.insert(std::make_pair(iItemIdx, kItemLimitSell));
				if( !kPair.second )
				{
					VERIFY_INFO_LOG( false , BM::LOG_LV0, __FL__ << _T("Data Error!!!") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed CASH_SHOP_ITEM_LIMITSELL"));
				}

			}				

			BM::Stream kPacket(PT_N_C_ANS_CASHSHOP_LIMIT_ITEM_INFO);
			PU::TWriteTable_MM(kPacket, kContItemLimitSell);
			g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);

			g_kTblDataMgr.SetContDef(kContItemLimitSell);

			PgTranPointer kTran(rkResult.contUserData);
			PgTransaction* pkTran = kTran.GetTran();
			if (pkTran != NULL)
			{			
				kTran.RecvCurerntLimitSell(CSR_SUCCESS, iItemTotalCount, iItemLimitCount, iItemBuyLimitCount, iItemBuyCount);
				return S_OK;
			}
		}
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Transaction information is NULL"));
	}

	g_kRealmUserMgr.Locked_LeaveEvent(rkResult.QueryOwner());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_CS_CASH_LIMIT_ITEM_UPDATE_QUERY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		CEL::DB_RESULT_COUNT::const_iterator return_iter = rkResult.vecResultCount.begin();

		if((iter != rkResult.vecArray.end()) && (return_iter != rkResult.vecResultCount.end()))
		{
			int iEntityCount = (*return_iter); ++return_iter;

			CONT_CASH_SHOP_ITEM_LIMITSELL kContItemLimitSell;
			for(int i = 0;i < iEntityCount;i++)
			{
				int iItemIdx = 0;
				CONT_CASH_SHOP_ITEM_LIMITSELL::mapped_type kItemLimitSell;

				(*iter).Pop(iItemIdx);									++iter;
				(*iter).Pop(kItemLimitSell.iItemTotalCount);			++iter;
				(*iter).Pop(kItemLimitSell.iItemLimitCount);			++iter;
				(*iter).Pop(kItemLimitSell.ItemBuyLimitCount);			++iter;

				auto kPair = kContItemLimitSell.insert(std::make_pair(iItemIdx, kItemLimitSell));
				if( !kPair.second )
				{
					VERIFY_INFO_LOG( false , BM::LOG_LV0, __FL__ << _T("Data Error!!!") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed CASH_SHOP_ITEM_LIMITSELL"));
				}
			}

			BM::Stream kPacket(PT_N_C_ANS_CASHSHOP_LIMIT_ITEM_INFO);
			PU::TWriteTable_MM(kPacket, kContItemLimitSell);
			g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);

			g_kTblDataMgr.SetContDef(kContItemLimitSell);
		}

		return S_OK;
	}

	g_kRealmUserMgr.Locked_LeaveEvent(rkResult.QueryOwner());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_CS_CASH_MODIFY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		if(iter != rkResult.vecArray.end())
		{
			BM::GUID		kMemberGuId;
			__int64			i64UsedCash = 0,
							i64OldCash = 0,
							i64CurCash = 0,
							i64OldBonus = 0,
							i64CurBonus = 0,
							i64UsedBonus = 0,
							i64AddedBonus = 0;

			int				iError = 0;
			ECASH_LOG_STATE eLogState;

			(*iter).Pop(iError);				++iter;
			(*iter).Pop(kMemberGuId);			++iter;
			(*iter).Pop(i64OldCash);			++iter;
			(*iter).Pop(i64CurCash);			++iter;
			(*iter).Pop(i64UsedCash);			++iter;
			(*iter).Pop(i64OldBonus);			++iter;
			(*iter).Pop(i64CurBonus);			++iter;
			(*iter).Pop(i64UsedBonus);			++iter;
			(*iter).Pop(i64AddedBonus);			++iter;
			(*iter).Pop(eLogState);				++iter;

			PgTranPointer kTran(rkResult.contUserData);
			PgTransaction* pkTran = kTran.GetTran();
			if (pkTran == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL, TransactionType =") << kTran.GetTransactionType());
				g_kRealmUserMgr.Locked_LeaveEvent(rkResult.QueryOwner());
				return E_SYSTEM_ERROR;
			}
			
			if (pkTran->InitCash() == 0)
			{
				// 처음에 balance 검사하지 않고, CashModify 되는 경우가 있다.
				pkTran->InitCash(i64OldCash);
			}
			if (pkTran->InitMileage() == 0)
			{
				// 처음에 balance 검사하지 않고, CashModify 되는 경우가 있다.
				pkTran->InitMileage(i64OldBonus);
			}
			pkTran->CurCash(i64CurCash);
			pkTran->CurMileage(i64CurBonus);
			pkTran->AddedMileage(i64AddedBonus);

			// eLogState == ECASH_LOG_STATE_ROLLBACK : CashRollback 이었다.
			EOrderSubType eSubLogType = ( eLogState == ECASH_LOG_STATE_ROLLBACK ) ? ELOrderSub_Rollback : ELOrderSub_Modify;
			if (i64UsedCash != 0)
			{
				PgLog kLog(ELOrderMain_Cash,eSubLogType);
				kLog.Set(0, iError);
				kLog.Set(0,i64UsedCash);
				kLog.Set(1,i64OldCash);
				kLog.Set(2,i64CurCash);
				kLog.Set(3,pkTran->OpenMarketDealingCash()); // 오픈마켓에서 판매 금액을 회수한 경우는 여기에 원래 금액이 저장되어 있다.
				pkTran->LogCont().Add(kLog);
			}
			if (i64UsedBonus != 0)
			{
				PgLog kLog(ELOrderMain_Mileage,eSubLogType);
				kLog.Set(0, iError);
				kLog.Set(0,i64UsedBonus);
				kLog.Set(1,i64OldBonus);
				kLog.Set(2,i64CurBonus);
				pkTran->LogCont().Add(kLog);
			}
			if (i64AddedBonus != 0)
			{
				PgLog kLog(ELOrderMain_Mileage,eSubLogType);
				kLog.Set(0, iError);
				kLog.Set(0,i64AddedBonus);
				kLog.Set(1,i64OldBonus);
				kLog.Set(2,i64CurBonus);
				pkTran->LogCont().Add(kLog);
			}

			if (eLogState == ECASH_LOG_STATE_ROLLBACK)
			{
				// 더이상 진행 되지 않기 때문에 LOG COMMIT
				if (true == kTran.SendLog())
				{
					return S_OK;
				}
				return E_FAIL;
			}

			return TRAN_HELPER::RecvCashModify(static_cast<ECashShopResult>(iError), kTran, i64CurCash, i64CurBonus, i64AddedBonus);
		}
		return E_FAIL;
	}
	
	g_kRealmUserMgr.Locked_LeaveEvent(rkResult.QueryOwner());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_CS_ADD_GIFT(CEL::DB_RESULT &rkResult)
{
	PgTranPointer kTran(rkResult.contUserData);
	PgSendGiftTran* pkTran = dynamic_cast<PgSendGiftTran*>(kTran.GetTran());
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("dynamic_cast<PgSendGiftTran*> failed"));
		return CSR_SYSTEM_ERROR;
	}

	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		if(iter != rkResult.vecArray.end())
		{
			int	iError = 0;
			
			(*iter).Pop(iError);		++iter;

			if(iError)
			{
				g_kRealmUserMgr.Locked_LeaveEvent(rkResult.QueryOwner());

				BM::Stream kPacket(PT_M_C_CS_ANS_SEND_GIFT);
				kPacket.Push(iError);
				g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
				
				kTran.Finish(E_FAIL);// 실패했으므로 Rollback 처리 되어야 한다.
			}
			else
			{
				BM::GUID kToGuid;
				(*iter).Pop(kToGuid);
				CEL::DB_QUERY kQuery(DT_PLAYER,DQT_GET_LAST_RECVED_GIFT,L"EXEC [dbo].[UP_CS_GET_LAST_RECVED_GIFT]");
				kQuery.InsertQueryTarget(kToGuid);
				kQuery.PushStrParam(kToGuid);
				kQuery.QueryOwner(kToGuid);
				g_kCoreCenter.PushQuery(kQuery);


				CEL::DB_QUERY kQueryRank(DT_PLAYER,0,L"EXEC [dbo].[UP_CS_SELECT_RANK]");
				kQueryRank.InsertQueryTarget(rkResult.QueryOwner());

				CEL::DB_QUERY kQueryGift(DT_PLAYER,1,L"EXEC [dbo].[UP_CS_SELECTGIFTLIST]");
				kQueryGift.InsertQueryTarget(rkResult.QueryOwner());
				kQueryGift.PushStrParam(rkResult.QueryOwner());
				
				CEL::DB_QUERY_TRAN kContQuery;
				kContQuery.DBIndex(DT_PLAYER);
				kContQuery.QueryType(DQT_CS_CASHSHOP);
				kContQuery.QueryOwner(rkResult.QueryOwner());

				kContQuery.push_back(kQueryRank);
				kContQuery.push_back(kQueryGift);
				g_kCoreCenter.PushQuery(kContQuery);

				kTran.Finish(S_OK);
			}
			return S_OK;
		}
	}
	
	g_kRealmUserMgr.Locked_LeaveEvent(rkResult.QueryOwner());
	pkTran->Rollback();

	return E_FAIL;
}

HRESULT Q_DQT_CS_GIFT_QUEYR(CEL::DB_RESULT &rkResult)
{
	ECashShopResult kError = CSR_DB_ERROR;

	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
	{
		kError = CSR_NOT_FOUND_GIFT;

		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		if(iter != rkResult.vecArray.end())
		{
			SCASHGIFTINFO kGift;
			(*iter).Pop(kGift.kGiftGuId);		++iter;
			(*iter).Pop(kGift.iCashItemIdx);		++iter;
			(*iter).Pop(kGift.bTimeType);		++iter;
			(*iter).Pop(kGift.iUseTime);		++iter;

			SERVER_IDENTITY kSI;
			SGroundKey kGndKey;
			kSI.ReadFromPacket(rkResult.contUserData);
			rkResult.contUserData.Pop(kGndKey);

			kError = g_kCashShopMgr.OnProcessReqRecvGift(kSI,kGndKey,rkResult.QueryOwner(),kGift);

			if(CSR_SUCCESS == kError)
			{
				return S_OK;
			}
		}
	}

	BM::Stream kPacket(PT_M_C_CS_ANS_RECV_GIFT);
	kPacket.Push(kError);
	g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
	g_kRealmUserMgr.Locked_LeaveEvent(rkResult.QueryOwner());
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_CS_RECV_GIFT(CEL::DB_RESULT_TRAN &rkContResult)
{
	g_kRealmUserMgr.Locked_LeaveEvent(rkContResult.QueryOwner());

	BM::Stream kPacket(PT_M_C_CS_ANS_RECV_GIFT);

	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)
		{
			CEL::DB_DATA_ARRAY::const_iterator retitor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != retitor )
			{
				int iIsSuccess = 0;

				(*retitor).Pop(iIsSuccess);

				if(iIsSuccess)
				{
					CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
					kPacket.Push(CSR_DB_ERROR);
					g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}

			continue;
		}

		kPacket.Push(CSR_DB_ERROR);
		g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);

		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	BM::GUID kGiftGuId;
	rkContResult.contUserData.Pop(kGiftGuId);

	kPacket.Push(CSR_SUCCESS);
	kPacket.Push(kGiftGuId);
	g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);
	return S_OK;
}

HRESULT Q_DQT_CS_MODIFY_VISABLE_RANK(CEL::DB_RESULT &rkResult)
{
	BM::Stream kPacket(PT_M_C_CS_ANS_MODIFY_VISABLE_RANK);

	int iMode = 0;
	rkResult.contUserData.Pop(iMode);

	if(iMode)
	{
		return S_OK;
	}

	if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		if(iter != rkResult.vecArray.end())
		{
			int iError = 0;
			(*iter).Pop(iError);		++iter;

			if(iError)
			{
				kPacket.Push(iError);
				g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			kPacket.Push(CSR_SUCCESS);
			g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
			return S_OK;
		}
	}

	kPacket.Push(CSR_DB_ERROR);
	g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_CS_CASHSHOP(CEL::DB_RESULT_TRAN &rkContResult)
{
	tagPT_M_C_CS_ANS_ENTER_CASHSHOP kData;

	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
		{
			switch(rkResult.QueryType())
			{
			case 0:
				{
					CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
					CONT_CASHRANKINFO::value_type kRank;

					while(iter != rkResult.vecArray.end())
					{
						(*iter).Pop(kRank.kName);			++iter;
						(*iter).Pop(kRank.i64Cash);			++iter;
						kData.kContRank.push_back(kRank);
					}
				}break;
			case 1:
				{
					CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
					CONT_CASHGIFTINFO::value_type kGift;
					while(iter != rkResult.vecArray.end())
					{
						(*iter).Pop(kGift.kGiftGuId);		++iter;
						(*iter).Pop(kGift.kCharName);		++iter;
						(*iter).Pop(kGift.kSendTime);		++iter;
						(*iter).Pop(kGift.iCashItemIdx);	++iter;
						(*iter).Pop(kGift.kComment);		++iter;
						(*iter).Pop(kGift.bType);			++iter;
						(*iter).Pop(kGift.bTimeType);		++iter;
						(*iter).Pop(kGift.iUseTime);		++iter;
						kData.kContGift.push_back(kGift);
					}
				}break;
			}

			continue;
		}
		
		kData.iError = CSR_DB_ERROR;
		BM::Stream kPacket;
		kData.WriteToPacket(kPacket);
		g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	kData.iError = CSR_SUCCESS;
	BM::Stream kPacket;
	kData.WriteToPacket(kPacket);
	g_kRealmUserMgr.Locked_SendToUser(rkContResult.QueryOwner(),kPacket,false);
	return S_OK;
}

typedef struct tagRESET_SKILL
{
	tagRESET_SKILL()
	{
		iESkillPt = 0;
	}

	BM::GUID kGuid;
	PgMySkill kMySkill;
	short iESkillPt;
}SRESET_SKILL;

LONG g_kResetSkillCount = 0;

HRESULT Q_DQT_SELECT_CHARA_SKILL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)
	{
		return E_FAIL;
	}

	BYTE abySkills[MAX_DB_SKILL_SIZE];
	BYTE abyNullSkills[MAX_DB_SKILL_SIZE];
	memset(abyNullSkills,0,sizeof(abyNullSkills));

	size_t kTotalCount = rkResult.vecArray.size()/3;
	g_kResetSkillCount = 0;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		SRESET_SKILL kEntity;
		memset(abySkills,0,sizeof(abySkills));
		(*iter).Pop(kEntity.kGuid);				++iter;
		(*iter).Pop(kEntity.iESkillPt);			++iter;
		(*iter).Pop(abySkills);					++iter;
		kEntity.kMySkill.Add(MAX_DB_SKILL_SIZE,abySkills);

		int iReturnSP = 0;
		kEntity.kMySkill.EraseSkill(SDT_Normal, &iReturnSP);
		__int64 const i64TotalSp = iReturnSP + kEntity.iESkillPt;

		CEL::DB_QUERY_TRAN kTran;
		kTran.DBIndex(DT_PLAYER);
		kTran.QueryOwner(rkResult.QueryOwner());
		kTran.QueryType(DQT_CHAR_RESET_SKILL);
		kTran.contUserData.Push(kTotalCount);

		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Skill2]");
			kQuery.InsertQueryTarget(kEntity.kGuid);
			kQuery.PushStrParam(kEntity.kGuid);
			kQuery.PushStrParam(abyNullSkills, MAX_DB_SKILL_SIZE);
			kTran.push_back(kQuery);
		}
		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Field2]");
			kQuery.InsertQueryTarget(kEntity.kGuid);
			kQuery.PushStrParam(kEntity.kGuid);
			kQuery.PushStrParam(std::wstring(_T("ExtraSkillPoint")));
			kQuery.PushStrParam(i64TotalSp);
			kTran.push_back(kQuery);
		}

		g_kCoreCenter.PushQuery(kTran);
	}

	g_kGMProcessMgr.SendOrderState(rkResult.QueryOwner(), OS_DONE);
	return S_OK;
}

HRESULT Q_DQT_CHAR_RESET_SKILL(CEL::DB_RESULT_TRAN &rkContResult)
{
	for(CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	{
		CEL::DB_RESULT & rkResult = (*iter);

		if(CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet)	
		{
			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

			while( rkResult.vecArray.end() != itor )
			{
				int iIsSuccess = 0;

				(*itor).Pop(iIsSuccess);					++itor;

				if(iIsSuccess)
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}

				break;
			}
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
	}
	
	LONG const lCount = ::InterlockedIncrement(&g_kResetSkillCount);

	size_t kTotalCount = 0;
	rkContResult.contUserData.Pop(kTotalCount);
	INFO_LOG(BM::LOG_LV6,__FL__ << _T("RESET SKILL : ") << lCount << _T(":") << kTotalCount);

	return S_OK;
}

HRESULT Q_DQT_GET_EVENT_COUPON(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)	
	{
		return E_FAIL;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if(iter != rkResult.vecArray.end())
	{
		int iError = 0;
		(*iter).Pop(iError);		++iter;
		if(iError)
		{
			return S_OK;
		}

		std::wstring kCouponID;
		(*iter).Pop(kCouponID);		++iter;

		BM::GUID kCharGuid;
		std::wstring kTitle,kText;
		rkResult.contUserData.Pop(kCharGuid);
		rkResult.contUserData.Pop(kTitle);
		rkResult.contUserData.Pop(kText);

		std::wstring kFrom;
		GetDefString(COUPON_RESULT_FROM, kFrom);

		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->kCause = CNE_POST_SYSTEM_MAIL;
		pkActionOrder->kPacket2nd.Push(kCharGuid);
		pkActionOrder->kPacket2nd.Push(std::wstring());
		pkActionOrder->kPacket2nd.Push(kTitle);
		pkActionOrder->kPacket2nd.Push(kText);
		pkActionOrder->kPacket2nd.Push(static_cast<int>(0));
		pkActionOrder->kPacket2nd.Push(static_cast<short>(0));
		pkActionOrder->kPacket2nd.Push(static_cast<int>(0));
		pkActionOrder->kPacket2nd.Push(kFrom);

		g_kJobDispatcher.VPush(pkActionOrder);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Q_DQT_GET_LAST_RECVED_GIFT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		return E_FAIL;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if(iter != rkResult.vecArray.end())
	{
		std::wstring kSenterName;
		(*iter).Pop(kSenterName);
		BM::Stream kPacket(PT_I_M_CS_ANS_LAST_RECVED_GIFT);
		kPacket.Push(rkResult.QueryOwner());
		kPacket.Push(kSenterName);
		SendToPacketHandler(kPacket);
	}
	return S_OK;
}

HRESULT Q_DQT_SAVE_PLAYER_EFFECT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("DB Query Failed Query=") << rkResult.Command());
		return E_FAIL;
	}
	return S_OK;
}

 HRESULT Q_DQT_HATCH_PET(CEL::DB_RESULT_TRAN & rkContResult)
 {
	 for( CEL::DB_RESULT_TRAN::iterator iter = rkContResult.begin();iter != rkContResult.end();++iter)
	 {
		 CEL::DB_RESULT & rkResult = (*iter);

		 if( CEL::DR_NO_RESULT == rkResult.eRet || CEL::DR_SUCCESS == rkResult.eRet )	
		 {
			 CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
			 while( rkResult.vecArray.end() != itr )
			 {
				 int iIsSuccess = 0;
				 itr->Pop(iIsSuccess);					++itr;
				 if(iIsSuccess)
				 {
					 LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					 return E_FAIL;
				 }
			 }
		 }
		 else
		 {
			 CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
			 LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			 return E_FAIL;
		 }
	 }

	 return S_OK;
 }

 HRESULT Q_DQT_REMOVE_PET(CEL::DB_RESULT &rkResult)
 {
	 if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)	
	 {
		 CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("DB Query Failed Query=") << rkResult.Command());
		 return E_FAIL;
	 }
	 return S_OK;
 }


HRESULT Q_DQT_RENAME_PET(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)	
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("DB Query Failed Query=") << rkResult.Command());
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Q_DQT_SETABIL_PET(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)	
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("DB Query Failed Query=") << rkResult.Command());
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Q_DQT_LOAD_PET_ITEM(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		if ( pkActionOrder )
		{
			BM::GUID kPlayerCharacterGuid;
			BM::GUID kPetID;
			rkResult.contUserData.Pop( kPlayerCharacterGuid );
			rkResult.contUserData.Pop( kPetID );

			pkActionOrder->InsertTarget(kPlayerCharacterGuid);
			pkActionOrder->kCause = CIE_LOAD_PET_ITEM;
			pkActionOrder->kAddonPacket.Push( kPlayerCharacterGuid );
			pkActionOrder->kAddonPacket.Push( kPetID );

			static BYTE abyInvExtern[MAX_DB_INVEXTEND_SIZE] = {	IT_EQUIP,			MAX_EQUIP_IDX,			//2
																IT_CONSUME,			MAX_CONSUME_IDX-1,		//4
																IT_ETC,				MAX_ETC_IDX,			//6
																IT_CASH,			MAX_CASH_IDX,			//8
																IT_SAFE,			MAX_SAFE_IDX,			//10
																IT_CASH_SAFE,		MAX_CASH_SAFE_IDX,		//12
																IT_HOME,			MAX_HOME_IDX,			//14
																IT_POST,			MAX_POST_IDX,			//16
																IT_USER_MARKET,		MAX_USER_MARKET_IDX,	//18
																IT_FIT_CASH,		MAX_FIT_IDX,			//20
																IT_RENTAL_SAFE1,	MAX_TIME_SAFE_IDX,		//22
																IT_RENTAL_SAFE2,	MAX_TIME_SAFE_IDX,		//24
																IT_RENTAL_SAFE3,	MAX_TIME_SAFE_IDX,		//26
																IT_PET,				MAX_PET_IDX,			//28
																0,					0,						//30
																0,					0,						//32
																0,					0,						//34
																0,					0,						//36
																0,					0,						//38
																0,					0						//40
																};
			BYTE abyExternIdx[MAX_DB_INVEXTEND_SIZE] = {0,};

			PgInventory kPetInven;
			kPetInven.OwnerGuid( kPetID );
			kPetInven.Init( abyInvExtern, abyExternIdx );

			CEL::DB_DATA_ARRAY::const_iterator db_itr = rkResult.vecArray.begin();//레퍼런스로 넣어주어야 하니까 반드시 할당해서 해야한다.
			CEL::DB_RESULT_COUNT::const_iterator count_itr = rkResult.vecResultCount.begin();// 상동

			PgRealmUserManager::ProcessItemQuery( db_itr, rkResult.vecArray.end(), count_itr, kPetInven );

			kPetInven.WriteToPacket( pkActionOrder->kAddonPacket, WT_DEFAULT );
			g_kJobDispatcher.VPush(pkActionOrder);
		}
		
		return S_OK;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("DB TranCommit failed Query=") << rkResult.Command());
	return E_FAIL;
}

 HRESULT Q_DQT_CASH_TRAN_COMMIT(CEL::DB_RESULT &rkResult)
 {
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		return S_OK;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("DB TranCommit failed Query=") << rkResult.Command());
	return E_FAIL;
 }


 HRESULT Q_DQT_CASH_TRANSACTION(CEL::DB_RESULT &rkResult)
 {
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		// GALA 등에서 Cash 변경 되었을 때 호출 됨.
		PgTranPointer kTran(rkResult.contUserData);
		PgTransaction* pkTran = kTran.GetTran();
		if (pkTran == NULL)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL"));
			return E_FAIL;
		}

		ECASH_LOG_STATE eLogState = ECASH_LOG_STATE_CASHUSE;
		int iError = 0;
		__int64 i64AddCash = 0;
		__int64 i64AddMileage = 0;
		__int64 i64AddedMileage = 0;
		 CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		 if ( rkResult.vecArray.end() != itor )
		 {
			 (*itor).Pop(iError);			++itor;
			 (*itor).Pop(eLogState);		++itor;
			 (*itor).Pop(i64AddCash);		++itor;	// 추가된 Cash(=Rollback된 캐시)
			 (*itor).Pop(i64AddMileage);	++itor;	// 추가된 Mileage(=Rollback 된 mileage)
			 (*itor).Pop(i64AddedMileage);	++itor;	// 적립된 Mileage(=Rollback 일 때는 항상 0)
		 }

		EOrderSubType eSubLogType = ( eLogState == ECASH_LOG_STATE_ROLLBACK ) ? ELOrderSub_Rollback : ELOrderSub_Modify;
		if (i64AddCash != 0)
		{
			PgLog kLog(ELOrderMain_Cash,eSubLogType);
			kLog.Set(0, iError);
			kLog.Set(0,i64AddCash);
			kLog.Set(1,pkTran->InitCash());
			kLog.Set(2,pkTran->CurCash());
			pkTran->LogCont().Add(kLog);
		}
		if (i64AddMileage != 0)
		{
			PgLog kLog(ELOrderMain_Mileage,eSubLogType);
			kLog.Set(0, iError);
			kLog.Set(0,i64AddMileage);
			kLog.Set(1,pkTran->InitMileage());
			kLog.Set(2,pkTran->CurMileage());
			pkTran->LogCont().Add(kLog);
		}
		if (i64AddedMileage != 0)
		{
			PgLog kLog(ELOrderMain_Mileage,eSubLogType);
			kLog.Set(0, iError);
			kLog.Set(0,i64AddedMileage);
			kLog.Set(1,pkTran->InitMileage());
			kLog.Set(2,pkTran->CurMileage());
			pkTran->LogCont().Add(kLog);
		}

		if ( eLogState == ECASH_LOG_STATE_ROLLBACK )
		{
			if (true == kTran.SendLog())
			{
				return S_OK;
			}
		}
		else
		{
			return TRAN_HELPER::RecvCashModify(static_cast<ECashShopResult>(iError), kTran, pkTran->CurCash(), pkTran->CurMileage(), pkTran->AddedMileage());
		}
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("DB failed Query=") << rkResult.Command());
	return E_FAIL;
 }

HRESULT Q_DQT_VISITLOG_ADD(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		while(itor != rkResult.vecArray.end())
		{
			short sStreetNo = 0;
			int iHouseNo = 0;
			int iAddValue = 0;
			BM::GUID kHomeGuid;

			(*itor).Pop(sStreetNo);			++itor;
			(*itor).Pop(iHouseNo);			++itor;
			(*itor).Pop(kHomeGuid);			++itor;
			(*itor).Pop(iAddValue);			++itor;

			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->InsertTarget(kHomeGuid);
			pkActionOrder->kCause = CIE_Home_Modify;
			pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL, kHomeGuid, SPMOD_AddAbil(AT_MYHOME_VISITLOGCOUNT, iAddValue)));
			pkActionOrder->kContOrder.push_back(SPMO(IMET_MYHOME_ADD_VISITLOGCOUNT, kHomeGuid, SMOD_MyHome_Sync_VisitLogCount(sStreetNo, iHouseNo, iAddValue)));
			g_kJobDispatcher.VPush(pkActionOrder);

			BM::Stream kPacket(PT_I_M_ANS_HOME_VISITLOG_ADD);
			kPacket.Push(S_OK);
			kPacket.Push(rkResult.QueryOwner());
			kPacket.Push(kHomeGuid);
			kPacket.Push(rkResult.contUserData);
			g_kRealmUserMgr.Locked_SendToUserGround(rkResult.QueryOwner(),kPacket,false,true);
		}

		return S_OK;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("DB TranCommit failed Query=") << rkResult.Command());
	return E_FAIL;
}

HRESULT Q_DQT_VISITLOG_DELETE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		int iResult = -1;

		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if(itor != rkResult.vecArray.end())
		{
			(*itor).Pop(iResult);			++itor;
		}
		
		BM::Stream kPacket(PT_M_C_ANS_HOME_VISITLOG_DELETE);

		if(0 == iResult)
		{
			kPacket.Push(S_OK);
		}
		else
		{
			kPacket.Push(E_MYHOME_NOT_HAVE_AUTHORITY);
		}
			
		kPacket.Push(rkResult.contUserData);
		g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
		return S_OK;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("DB TranCommit failed Query=") << rkResult.Command());
	return E_FAIL;
}

HRESULT Q_DQT_VISITLOG_LIST(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		BM::GUID kHomeOwnerGuid;
		rkResult.contUserData.Pop(kHomeOwnerGuid);

		CONT_HOMEVISITLOG kCont;
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		while(itor != rkResult.vecArray.end())
		{
			CONT_HOMEVISITLOG::value_type kValue;
			(*itor).Pop(kValue.kLogGuid);		++itor;
			(*itor).Pop(kValue.kVisitorGuid);	++itor;
			(*itor).Pop(kValue.kVisitorName);	++itor;
			(*itor).Pop(kValue.kVisitLog);		++itor;
			BM::DBTIMESTAMP_EX kDateTime;
			(*itor).Pop(kDateTime);				++itor;
			kValue.kLogTime = kDateTime;
			(*itor).Pop(kValue.bPrivate);		++itor;

			if((true == kValue.bPrivate) && (rkResult.QueryOwner() != kHomeOwnerGuid) && (kValue.kVisitorGuid != rkResult.QueryOwner()))
			{
				std::wstring().swap(kValue.kVisitLog); // 비밀글은 내용을 보내지 않는다.
			}

			kCont.push_back(kValue);
		}

		BM::Stream kPacket(PT_M_C_ANS_HOME_VISITLOG_LIST);
		kPacket.Push(S_OK);
		PU::TWriteArray_M(kPacket,kCont);
		g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
		return S_OK;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("DB TranCommit failed Query=") << rkResult.Command());
	return E_FAIL;
}

int const DEFAULT_MYHOME_SIDEJOB_ENTER_COST = 50;

HRESULT ProcessMyhomeEnter(BM::GUID const & kOwnerGuid, BM::GUID const & kHomeGuid,short const sStreetNo, int const iHouseNo,int const iGroundNo,int const iTodayHit, int const iTotalHit, WORD const wStreamType)
{
	SReqMapMove_MT kRMMIN( MMET_None );
	kRMMIN.kTargetSI.nChannel = 0;
	kRMMIN.nTargetPortal = 1;
	kRMMIN.kTargetKey.GroundNo(iGroundNo);
	kRMMIN.kTargetKey.Guid(kHomeGuid);

	BM::Stream kPacket(wStreamType);
	kPacket.Push(kOwnerGuid);
	kPacket.Push(kRMMIN);

	BM::Stream kAddonPacket;
	kAddonPacket.Push(true);
	kAddonPacket.Push(kOwnerGuid);
	kAddonPacket.Push(kPacket.Data());

	PgDoc_Player kCopyPlayer;
	if(true == g_kRealmUserMgr.Locked_GetDoc_Player(kHomeGuid, false, kCopyPlayer))
	{
		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(kHomeGuid);
		pkActionOrder->kCause = CIE_Home_Modify;
		pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ABIL, kHomeGuid, SPMOD_AddAbil(AT_MYHOME_TODAYHIT, iTodayHit)));
		pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ABIL, kHomeGuid, SPMOD_AddAbil(AT_MYHOME_TOTALHIT, iTotalHit)));
		g_kJobDispatcher.VPush(pkActionOrder);

		SGroundMakeOrder kOrder;
		kOrder.kKey.GroundNo( iGroundNo );
		kOrder.kKey.Guid( kHomeGuid );

		BM::Stream kPacket( PT_N_T_REQ_CREATE_PUBLICMAP, PMET_MYHOME );
		kOrder.WriteToPacket(kPacket);
		kPacket.Push(kAddonPacket);
		::SendToCenter( CProcessConfig::GetPublicChannel(), kPacket );
	}
	else
	{
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LOAD_MYHOME_INFO,L"EXEC [dbo].[up_LoadMyHomeInfo]");
		kQuery.InsertQueryTarget(kHomeGuid);
		kQuery.QueryOwner(kHomeGuid);
		kQuery.PushStrParam(sStreetNo);
		kQuery.PushStrParam(iHouseNo);
		kQuery.contUserData.Push(sStreetNo);
		kQuery.contUserData.Push(iHouseNo);
		kQuery.contUserData.Push(kAddonPacket);
		g_kCoreCenter.PushQuery(kQuery);
	}

	return S_OK;
}

HRESULT Q_DQT_MYHOME_SIDEJOB_ENTER(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CONT_HOMEVISITLOG kCont;
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if(itor != rkResult.vecArray.end())
		{
			int iResult = 0;
			(*itor).Pop(iResult); ++itor;
			if(0 == iResult)
			{
				int iGroundNo = 0;
				BM::GUID kGuid;
				int iTodayHit = 0;
				int iTotalHit = 0;

				(*itor).Pop(iGroundNo); ++itor;
				(*itor).Pop(kGuid);		++itor;
				(*itor).Pop(iTodayHit);	++itor;
				(*itor).Pop(iTotalHit);	++itor;

				short	siStreetNo = 0;
				int		iHouseNo = 0;

				rkResult.contUserData.Pop(siStreetNo);
				rkResult.contUserData.Pop(iHouseNo);

				ProcessMyhomeEnter(rkResult.QueryOwner(), kGuid, siStreetNo, iHouseNo, iGroundNo, iTodayHit, iTotalHit, PT_M_C_ANS_ENTER_SIDE_JOB);
				return S_OK;
			}
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_ENTER_SIDE_JOB);
	kPacket.Push(E_MYHOME_NOT_HAVE_AUTHORITY);
	g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
	return E_FAIL;
}

HRESULT Q_DQT_CHECK_ENABLE_VISIT_OPTION(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CONT_HOMEVISITLOG kCont;
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if(itor != rkResult.vecArray.end())
		{
			int iResult = 0;
			(*itor).Pop(iResult); ++itor;
			if(0 == iResult)
			{
				int iGroundNo = 0;
				BM::GUID kGuid;
				int iTodayHit = 0;
				int iTotalHit = 0;

				(*itor).Pop(iGroundNo); ++itor;
				(*itor).Pop(kGuid);		++itor;
				(*itor).Pop(iTodayHit);	++itor;
				(*itor).Pop(iTotalHit);	++itor;

				short	siStreetNo = 0;
				int		iHouseNo = 0;

				rkResult.contUserData.Pop(siStreetNo);
				rkResult.contUserData.Pop(iHouseNo);

				ProcessMyhomeEnter(rkResult.QueryOwner(), kGuid, siStreetNo, iHouseNo, iGroundNo, iTodayHit, iTotalHit, PT_I_M_ANS_MYHOME_ENTER);
				return S_OK;
			}
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_MYHOME_ENTER);
	kPacket.Push(E_MYHOME_NOT_HAVE_INVATIONCARD);
	g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
	return E_FAIL;
}

HRESULT Q_DQT_VISITFLAG_MODIFY(CEL::DB_RESULT &rkResult)
{
	BM::Stream kPacket(PT_M_C_ANS_HOME_VISITFLAG_MODIFY);
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if(itor != rkResult.vecArray.end())
		{
			BYTE bVisitFlag = 0;
			(*itor).Pop(bVisitFlag); ++itor;

			kPacket.Push(S_OK);
			kPacket.Push(bVisitFlag);
			g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
			return S_OK;
		}
	}

	kPacket.Push(E_MYHOME_NOT_HAVE_AUTHORITY);
	g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
	return E_FAIL;
}

HRESULT Q_DQT_ADD_MYHOME(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator itor = rkResult.vecArray.begin();
		if(itor != rkResult.vecArray.end())
		{
			int iErr = 0;
			(*itor).Pop(iErr); ++itor;
			if(0 == iErr)
			{
				BM::GUID kOwnerGuid;
				short siBuildingNo = 0;
				int iHouseNo = 0;
				(*itor).Pop(kOwnerGuid); ++itor;
				(*itor).Pop(siBuildingNo); ++itor;
				(*itor).Pop(iHouseNo); ++itor;

				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->kCause = CIE_Buy_MyHome;
				pkActionOrder->kContOrder.push_back(SPMO(IMET_MYHOME_SET_HOMEADDR, kOwnerGuid,SMOD_MyHome_MyHome_Set_Addr(siBuildingNo,iHouseNo)));	// 유저 주소 정보 설정
				g_kJobDispatcher.VPush(pkActionOrder);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LOAD_MYHOME_INFO,L"EXEC [dbo].[up_LoadMyHomeInfo]");
				kQuery.InsertQueryTarget(kOwnerGuid);
				kQuery.QueryOwner(kOwnerGuid);
				kQuery.PushStrParam(siBuildingNo);
				kQuery.PushStrParam(iHouseNo);
				kQuery.contUserData.Push(siBuildingNo);
				kQuery.contUserData.Push(iHouseNo);
				kQuery.contUserData.Push(false);
				g_kCoreCenter.PushQuery(kQuery);

				//친구 갱신 요청
				{
					BM::Stream kRefresh(PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_FRIEND);
					kRefresh.Push(kOwnerGuid);
					kRefresh.Push(siBuildingNo);
					kRefresh.Push(iHouseNo);
					::SendToFriendMgr(kRefresh);
				}
				{//길드 갱신 요청
					BM::Stream kRefresh(PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_GUILD);
					kRefresh.Push(kOwnerGuid);
					kRefresh.Push(siBuildingNo);
					kRefresh.Push(iHouseNo);
					::SendToGuildMgr(kRefresh);
				}
				{//커플 갱신 요청
					BM::Stream kRefresh(PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_COUPLE);
					kRefresh.Push(kOwnerGuid);
					kRefresh.Push(siBuildingNo);
					kRefresh.Push(iHouseNo);
					::SendToCoupleMgr(kRefresh);
				}
				{//파티 갱신 요청
					SContentsUser rkUser;
					if(S_OK ==  GetPlayerByGuid(kOwnerGuid, false, rkUser) )
					{
						BM::Stream kRefresh(PT_N_T_NFY_COMMUNITY_STATE_HOMEADDR_PARTY);
						kRefresh.Push(rkUser.kCharGuid);
						kRefresh.Push(siBuildingNo);
						kRefresh.Push(iHouseNo);
						::SendToGlobalPartyMgr(rkUser.sChannel, kRefresh);
					}
				}
			}
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT Q_DQT_MYHOME_MODIFY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator itor = rkResult.vecArray.begin();
		if(itor != rkResult.vecArray.end())
		{
			SMYHOME kMyHome;
			kMyHome.MyHomeReadFromDB(itor);

			BM::Stream kMyhomePacket;
			kMyHome.WriteToPacket(kMyhomePacket);

			SMYHOME kOldMyHome;
			kOldMyHome.ReadFromPacket(rkResult.contUserData);

			if(kOldMyHome.kOwnerGuid != kMyHome.kOwnerGuid)	// 주인이 변경되었다. 그럼 이전 주인한테도 한번 날려주자.
			{
				{
					BM::Stream kPacket(PT_I_M_MYHOME_MOVE_TO_HOMETOWN);
					kPacket.Push(kMyHome.kHomeInSideGuid);
					g_kRealmUserMgr.Locked_SendToUserGround(kMyHome.kHomeInSideGuid,kPacket,false,true);
				}
			}
			else
			{
				if(kMyHome.iHomeStyle != kOldMyHome.iHomeStyle) // 집 외형이 변경되면 내부도 변경되어야 한다. 안에 있는 녀석들 청소해야함.
				{
					BM::Stream kPacket(PT_I_M_MYHOME_MOVE_TO_HOMETOWN);
					kPacket.Push(kMyHome.kHomeInSideGuid);
					g_kRealmUserMgr.Locked_SendToUserGround(kMyHome.kHomeInSideGuid,kPacket,false,true);
				}
			}

			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT Q_DQT_INVITATION_CREATE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator itor = rkResult.vecArray.begin();
		if(itor != rkResult.vecArray.end())
		{
			int iRet = 0;
			(*itor).Pop(iRet); ++itor;

			BM::Stream kPacket(PT_M_C_ANS_MYHOME_POST_INVITATION_CARD);

			switch(iRet)
			{
			case 0:
				{
					std::wstring kVisitorName;
					rkResult.contUserData.Pop(kVisitorName);
					kPacket.Push(S_OK);
					kPacket.Push(kVisitorName);

					BM::GUID kOwnerGuid;
					std::wstring kName;

					(*itor).Pop(kOwnerGuid);	++itor;
					(*itor).Pop(kName);			++itor;

					BM::Stream kNoti(PT_M_C_HOME_NOTI_INVITATION);
					kNoti.Push(kName);
					g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid,kNoti,false);

					SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
					pkActionOrder->InsertTarget(rkResult.QueryOwner());
					pkActionOrder->kCause = CAE_Achievement;
					pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ABIL,rkResult.QueryOwner(),SPMOD_AddAbil(AT_ACHIEVEMENT_INVITATION,1)));
					g_kJobDispatcher.VPush(pkActionOrder);
				}break;
			case 1:
				{
					kPacket.Push(E_MYHOME_NOT_FOUND_VISITOR);
				}break;
			case 2:
				{
					kPacket.Push(E_MYHOME_TODAY_INVATION_LIMIT);
				}break;
			case 3:
				{
					kPacket.Push(E_MYHOME_DB_ERROR);
				}break;
			case 4:
				{
					kPacket.Push(E_MYHOME_CANT_POST_SELF);
				}break;
			}

			g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT Q_DQT_INVITATION_SELECT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator itor = rkResult.vecArray.begin();
		BM::Stream kPacket(PT_M_C_ANS_MYHOME_INVITATION_CARD);

		CONT_INVITATIONCARD kCont;
		while(itor != rkResult.vecArray.end())
		{
			CONT_INVITATIONCARD::value_type kValue;
			(*itor).Pop(kValue.kHomeOwnerName);	++itor;
			(*itor).Pop(kValue.siStreetNo);		++itor;
			(*itor).Pop(kValue.iHouseNo);		++itor;
			BM::DBTIMESTAMP_EX kDateTime;
			(*itor).Pop(kDateTime);				++itor;
			kValue.kLimitTime = static_cast<BM::PgPackedTime>(kDateTime);
			kCont.push_back(kValue);
		}

		PU::TWriteArray_M(kPacket,kCont);

		g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Q_DQT_VISITORS_SELECT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::iterator itor = rkResult.vecArray.begin();
		BM::Stream kPacket(PT_M_C_ANS_MYHOME_VISITORS);

		CONT_VISITORINFO kCont;
		while(itor != rkResult.vecArray.end())
		{
			short	siStreetNo = 0;
			int		iHouseNo = 0;
			CONT_VISITORINFO::value_type kValue;

			(*itor).Pop(kValue.kOwnerGuid);		++itor;
			(*itor).Pop(kValue.kName);			++itor;
			BM::DBTIMESTAMP_EX kTime;
			(*itor).Pop(kTime);					++itor;
			kValue.kVistTime = static_cast<BM::PgPackedTime>(kTime);
			(*itor).Pop(siStreetNo);			++itor;
			(*itor).Pop(iHouseNo);				++itor;

			kValue.kAddr = SHOMEADDR(siStreetNo, iHouseNo);
			kCont.push_back(kValue);
		}

		PU::TWriteArray_M(kPacket,kCont);

		g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT Q_DQT_EVENT_CASHITEM_GIFT1(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)	
	{
		return E_FAIL;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if(iter != rkResult.vecArray.end())
	{
		int iError = 0;
		(*iter).Pop(iError);		++iter;
		if(iError)
		{
			return S_OK;
		}

		int iEventNo = 0;
		int	iArticleIdx = 0;
		BYTE bTimeType = 0;
		int iUseTime = 0;
		int iOption = 0;
		BM::GUID kCharGuid;
		std::wstring kFrom, kTitle, kText;

		rkResult.contUserData.Pop(iEventNo); //이벤트 번호
		rkResult.contUserData.Pop(iArticleIdx); //선물번호
		rkResult.contUserData.Pop(bTimeType);
		rkResult.contUserData.Pop(iUseTime);
		rkResult.contUserData.Pop(iOption);
		rkResult.contUserData.Pop(kCharGuid);
		rkResult.contUserData.Pop(kFrom);
		rkResult.contUserData.Pop(kTitle);
		rkResult.contUserData.Pop(kText);

		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->kCause = CNE_POST_SYSTEM_MAIL;
		pkActionOrder->kPacket2nd.Push(kCharGuid);
		pkActionOrder->kPacket2nd.Push(std::wstring());
		pkActionOrder->kPacket2nd.Push(kTitle);
		pkActionOrder->kPacket2nd.Push(kText);
		pkActionOrder->kPacket2nd.Push(static_cast<int>(0));
		pkActionOrder->kPacket2nd.Push(static_cast<short>(0));
		pkActionOrder->kPacket2nd.Push(static_cast<int>(0));
		pkActionOrder->kPacket2nd.Push(kFrom);

		g_kJobDispatcher.VPush(pkActionOrder);


		SContentsUser rkOut;
		if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
		{
			PgLogCont kContLog(ELogMain_System_Event,ELogSub_Cash_Present,rkOut.kMemGuid,kCharGuid,rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
			PgLog kLog(ELOrderMain_Market_Article);
			kLog.Set(0,iEventNo);
			kLog.Set(1,iArticleIdx);
			kLog.Set(2,static_cast<int>(bTimeType));
			kLog.Set(3,iUseTime);
			kLog.Set(4,iOption);
			kLog.Set(0,kFrom);
			kContLog.Add(kLog);
			kContLog.Commit();
		}

		return S_OK;
	}
	return E_FAIL;
}

HRESULT Q_DQT_UPDATE_ACHIEVEMENT_FIST(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		return E_FAIL;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if(iter == rkResult.vecArray.end())
	{
		return E_FAIL;
	}

	int iError = 0;
	(*iter).Pop(iError);	++iter;
	if(iError)
	{
		return S_OK;
	}

	int iSaveIdx = 0;
	std::wstring kName;

	(*iter).Pop(iSaveIdx);	++iter;
	(*iter).Pop(kName);		++iter;

	CONT_DEF_ACHIEVEMENTS_SAVEIDX const * pkCont = NULL;
	g_kTblDataMgr.GetContDef(pkCont);
	if(pkCont)
	{
		CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator iter = pkCont->find(iSaveIdx);
		if(iter != pkCont->end())
		{
			{// 업적 랠름 최초 달성
				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->InsertTarget(rkResult.QueryOwner());
				pkActionOrder->kCause = CAE_Achievement;
				pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ABIL,rkResult.QueryOwner(),SPMOD_AddAbil(AT_ACHIEVEMENT_COMPLETE_FIRST,(*iter).second.iIdx)));
				g_kJobDispatcher.VPush(pkActionOrder);
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const* pItemDef = kItemDefMgr.GetDef((*iter).second.iItemNo);
			if(pItemDef)
			{
				if(IG_ARTIFACT <= pItemDef->GetAbil(AT_GRADE))
				{
					BM::Stream kPacket(PT_M_C_NOTI_ACHIEVEMENT_COMPLETE_FIRST);
					kPacket.Push((*iter).second.iIdx);
					kPacket.Push(kName);
					SendToServerType(CEL::ST_CENTER,kPacket);
				}
			}
		}
	}
	return S_OK;
}

HRESULT Q_DQT_SELECT_ACHIEVEMENT_RANK(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		return E_FAIL;
	}

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();
	if(count_iter == rkResult.vecResultCount.end())
	{
		return E_FAIL;
	}

	CONT_ACHIEVEMENT_RANK kCont;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();

	int const iPlayerCount = (*count_iter); ++count_iter;
	for(int i = 0;i < iPlayerCount;++i)
	{
		CONT_ACHIEVEMENT_RANK::key_type kKey;
		CONT_ACHIEVEMENT_RANK::mapped_type kValue;
		(*iter).Pop(kKey);					++iter;
		(*iter).Pop(kValue.kName);			++iter;
		(*iter).Pop(kValue.iRank);			++iter;
		(*iter).Pop(kValue.iTotalPoint);	++iter;
		kCont.insert(std::make_pair(kKey,kValue));
	}

	int const iRankDataCount = (*count_iter); ++count_iter;
	for(int i = 0;i < iRankDataCount;++i)
	{
		BM::GUID kGuid;
		int iCatetory = 0;
		int iPoint = 0;

		(*iter).Pop(kGuid);			++iter;
		(*iter).Pop(iCatetory);		++iter;
		(*iter).Pop(iPoint);		++iter;

		CONT_ACHIEVEMENT_RANK::iterator rank_iter = kCont.find(kGuid);
		if(rank_iter != kCont.end())
		{
			(*rank_iter).second.kContPoint.insert(std::make_pair(iCatetory,iPoint));
		}
	}

	BM::Stream kPacket(PT_M_C_ANS_ACHIEVEMENT_RANK);
	PU::TWriteTable_AM(kPacket,kCont);
	g_kRealmUserMgr.Locked_SendToUser(rkResult.QueryOwner(),kPacket,false);	

	return S_OK;
}

HRESULT Q_DQT_POST_GROUP_MAIL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		return E_FAIL;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if(iter == rkResult.vecArray.end())
	{
		return S_OK;
	}

	while(iter != rkResult.vecArray.end())
	{
		std::wstring kTitle, kText, kFrom;
		int iItemNo = 0;
		short siCount = 0;
		__int64 i64Money = 0;

		(*iter).Pop(kTitle);	++iter;
		(*iter).Pop(kText);		++iter;
		(*iter).Pop(iItemNo);	++iter;
		(*iter).Pop(siCount);	++iter;
		(*iter).Pop(i64Money);	++iter;
		(*iter).Pop(kFrom);		++iter;

		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->kCause = CNE_POST_SYSTEM_MAIL;
		pkActionOrder->kPacket2nd.Push(rkResult.QueryOwner());
		pkActionOrder->kPacket2nd.Push(std::wstring());
		pkActionOrder->kPacket2nd.Push(kTitle);
		pkActionOrder->kPacket2nd.Push(kText);
		pkActionOrder->kPacket2nd.Push(iItemNo);
		pkActionOrder->kPacket2nd.Push(siCount);
		pkActionOrder->kPacket2nd.Push(static_cast<int>(i64Money));
		pkActionOrder->kPacket2nd.Push(kFrom);

		g_kJobDispatcher.VPush(pkActionOrder);
	}

	return S_OK;
}

HRESULT Q_DQT_SELECT_USER_QUEST_COMPLETE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		return E_FAIL;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if(iter == rkResult.vecArray.end())
	{
		return S_OK;
	}

	BM::GUID kCharGuid;
	int iQuestID = 0;
	BYTE bComplete = 0;
	
	while(iter != rkResult.vecArray.end())
	{
		(*iter).Pop(kCharGuid);		++iter;
		(*iter).Pop(iQuestID);		++iter;
		(*iter).Pop(bComplete);		++iter;

		if(iQuestID)
		{
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->InsertTarget(kCharGuid);
			pkActionOrder->kCause = CIE_UserQuestComplete;
			pkActionOrder->kContOrder.push_back( SPMO(IMET_ADD_ENDQUEST, kCharGuid, SPMOD_SetEndQuest(iQuestID, bComplete)) );
			pkActionOrder->kPacket2nd.Push(CIE_UserQuestComplete);
			pkActionOrder->kPacket2nd.Push(kCharGuid);
			pkActionOrder->kPacket2nd.Push(iQuestID);
			g_kJobDispatcher.VPush(pkActionOrder);
		}
	}

	return S_OK;
}

HRESULT Q_DQT_UPDATE_USER_QUEST_COMPLETE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Q_DQT_UPDATE_SPECIFIC_REWARD(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_UPDATE_USER_JOBSKILL_INFO_NO_OP(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT Q_DQT_NO_OP(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		return S_OK;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Failed Query = ") << rkResult.Command() );
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}