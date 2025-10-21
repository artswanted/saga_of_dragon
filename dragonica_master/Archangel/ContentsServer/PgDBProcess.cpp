#include "stdafx.h"
#include "Lohengrin/dbtables.h"
#include "Lohengrin/dbtables2.h"
#include "Lohengrin/PgRealmManager.h"
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/PgPlayLimiter.h"
#include "Variant/PgDBCache.h"
#include "Variant/PgClassDefMgr.h"
#include "variant/ItemBagMgr.h"
#include "variant/PgStoreMgr.h"
#include "variant/PgCouponEventView.h"
#include "Variant/PgControlDefMgr.h"
#include "Variant/PgQuestResetSchedule.h"
#include "variant/pggamblemachine.h"
#include "Variant/PgPvPRanking.h"
#include "Variant/PgStringUtil.h"
#include "PgDBProcess.h"
#include "PgGuildDB.h"
#include "PgGMProcessMgr.h"
#include "PgEventDoc.h"
#include "Item/PgDBProcess.h"
#include "Contents/PgQuestRandom.h"
#include "Contents/PgQuestMng.h"
#include "PgCouponEventDoc.h"
#include "PgServerSetMgr.h"
#include "PgTask_Contents.h"
#include "PgSendWrapper.h"
#include "Item/ItemDropControl.h"
#include "PgBSGame.h"
#include "PgJobSkillChecker.h"
#include "PgEmporiaMgr.h"
#include "PgPvPRankingSceduler.h"
#include "PgHardCoreDungeonSystemMgr.h"
#include "PgTimer.h"
#include "PgPvPLeagueMgr.h"
#include "Variant/PgJumpingCharEventMgr.h"
#include <csvdb/def/items.h>
#include <csvdb/load.h>
#include "CSVQuery.h"

//
namespace DefLoadErrorLog
{
	typedef std::list< BM::vstring > CONT_ERROR_MSG;
	CONT_ERROR_MSG kContErrorMsg;
	void AddErrorMsg(BM::vstring const& rkErrorMsg)
	{
		kContErrorMsg.push_back( rkErrorMsg );
	}
	bool Display()
	{
		CONT_ERROR_MSG::const_iterator iter = kContErrorMsg.begin();
		while( kContErrorMsg.end() != iter )
		{
			CAUTION_LOG(BM::LOG_LV1, (*iter));
			++iter;
		}
		return !kContErrorMsg.empty();
	}

	struct AddError
	{
		AddError()
		{
		}
		void operator()(BM::vstring const& rhs)
		{
			AddErrorMsg(rhs);
		}
	};
};

inline void csv_get_col_as(CsvParser& csv, int idx, int &out)
{
	out = csv.col_int(idx);
}

#define BIND_COL(csv, field, name) \
	{ static int iColIndex = -1; if (iColIndex >= 0) csv_get_col_as(csv, iColIndex, field); else iColIndex = csv.col_idx(name); }

static bool Q_DQT_DEF_DAILY_REWARD(const char* pkTBPath)
{
	BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefDailyReward.csv");
	TBL_DEF_DAILY_REWARD dailyReward;
	CONT_DEF_DAILY_REWARD contDailyReward;

	for (; !csv.is_empty(); csv.next_row())
	{
		BIND_COL(csv, dailyReward.iDayNo, "f_iDayNo");
		BIND_COL(csv, dailyReward.iItems[0], "f_iItemNo01");
		BIND_COL(csv, dailyReward.iItems[1], "f_iItemNo02");
		BIND_COL(csv, dailyReward.iItems[2], "f_iItemNo03");
		BIND_COL(csv, dailyReward.iItems[3], "f_iItemNo04");
		BIND_COL(csv, dailyReward.iItems[4], "f_iItemNo05");
		BIND_COL(csv, dailyReward.iItems[5], "f_iItemNo06");
		BIND_COL(csv, dailyReward.iItems[6], "f_iItemNo07");
		if (csv.row_idx() == CSV_HEADER_ROW)
			continue;

		if (!contDailyReward.insert(std::make_pair(dailyReward.iDayNo, dailyReward)).second)
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate daily reward day:"<<dailyReward.iDayNo);
	}
	g_kTblDataMgr.SetContDef(contDailyReward);
	return true;
}

static bool Q_DQT_DEF_CART_MISSION_MONSTER(const char* pkTBPath)
{
	BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefCartMissionMonster.csv");
	TBL_DEF_CART_MISSION_MONSTER kMonster;
	CONT_DEF_CART_MISSION_MONSTER::key_type kKey;
	CONT_DEF_CART_MISSION_MONSTER kContCartMissionMonster;

	for (; !csv.is_empty(); csv.next_row())
	{
		BIND_COL(csv, kKey.kPriKey, "f_MapNo");
		BIND_COL(csv, kKey.kSecKey, "f_StageNo");
		BIND_COL(csv, kKey.kTrdKey, "f_GenNo");
		BIND_COL(csv, kMonster.iMonBagNo, "f_MonBagNo");
		if (csv.row_idx() == CSV_HEADER_ROW)
		{
			continue;
		}

		if (kContCartMissionMonster.insert(std::make_pair(kKey, kMonster)).second == false)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate cart mission spawn: " << kKey.kPriKey << "," << kKey.kSecKey << "," << kKey.kTrdKey);
			return false;
		}
	}
	g_kTblDataMgr.SetContDef(kContCartMissionMonster);
	return true;
}

static void Q_DQT_UPDATE_USER_UNLOCK_ALL_CHARACTERS(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet << "; Cmd: " << rkResult.Command());
		return;
	}

}

//! ���͸��� DB�� ���� �Ѵ�.
using namespace BM;

HRESULT CALLBACK OnDB_EXECUTE( CEL::DB_RESULT &rkResult )
{
	DWORD const dwStart = BM::GetTime32();

	if( !PgDBCache::OnDBExcute( rkResult ) )// Dump ������ ���� ���̺����� PgDBCache��
	{
		switch(rkResult.QueryType())
		{
		case DQT_GET_PLAYER_LIST:					{ g_kRealmUserMgr.Locked_Q_DQT_GET_PLAYER_LIST(rkResult); }break;
		case DQT_SELECT_PLAYER_DATA:				{ g_kRealmUserMgr.Locked_Q_DQT_SELECT_PLAYER_DATA(rkResult); }break;

		case DQT_SAVECHARACTER:
		case DQT_SAVECHARACTER_POINT:				{ Q_DQT_SAVECHARACTER(rkResult); }break;

		case DQT_RECORD_LEVELUP:					{ Q_DQT_RECORD_LEVELUP(rkResult); }break;			
		case DQT_UPDATE_MEMBER_PW:					{ Q_DQT_UPDATE_MEMBER_PW(rkResult); }break;
		case DQT_CREATECHARACTER:					{ g_kRealmUserMgr.Locked_Q_DQT_CREATECHARACTER(rkResult); }break;
		case DQT_CHECK_CHARACTERNAME_OVERLAP:		{ g_kRealmUserMgr.Locked_Q_DQT_CHECK_CHARACTERNAME_OVERLAP(rkResult); }break;
		case DQT_DELETECHARACTER:					{ g_kRealmUserMgr.Locked_Q_DQT_DELETECHARACTER(rkResult); }break;
		case DQT_REALMMERGE:						{ g_kRealmUserMgr.Locked_Q_DQT_REALMMERGE(rkResult); }break;
		case DQT_FRIENDLIST_ADD_BYGUID:				{ Q_DQT_FRIENDLIST_ADD_BYGUID(rkResult); }break;
		case DQT_FRIENDLIST_SELECT:					{ Q_DQT_FRIENDLIST_SELECT(rkResult); }break;
		case DQT_FRIENDLIST_UPDATE_CHATSTATUS:		{ Q_DQT_FRIENDLIST_UPDATE_CHATSTATUS(rkResult); }break;
		case DQT_FRIENDLIST_DEL:					{ Q_DQT_FRIENDLIST_DEL(rkResult); }break;
		case DQT_FRIENDLIST_UPDATE_GROUP:			{ Q_DQT_FRIENDLIST_UPDATE_GROUP(rkResult); }break;
		case DQT_FRIENDLIST_MODIFY:					{ Q_DQT_FRIENDLIST_MODIFY(rkResult); }break;
		case DQT_GUILD_PROC:						{DBR_Community::Q_DQT_GUILD_PROC(rkResult);}break;
		case DQT_GUILD_INV_AUTHORITY:				{DBR_Community::Q_DQT_GUILD_INV_AUTHORITY(rkResult);}break;
		case DQT_GUILD_SELECT_BASIC_INFO:			{DBR_Community::Q_DQT_GUILD_SELECT_BASIC_INFO(rkResult);}break;
		case DQT_GUILD_MEMBER_PROC:					{DBR_Community::Q_DQT_GUILD_MEMBER_PROC(rkResult);}break;
		case DQT_GUILD_UPDATE_MEMBER_GRADE:			{DBR_Community::Q_DQT_GUILD_UPDATE_MEMBER_GRADE(rkResult);}break;			
		case DQT_GUILD_UPDATE_NOTICE:				{DBR_Community::Q_DQT_GUILD_UPDATE_NOTICE(rkResult);}break;
		case DQT_GUILD_UPDATE_EXP_LEVEL:			{DBR_Community::Q_DQT_GUILD_UPDATE_EXP_LEVEL(rkResult);}break;
		case DQT_GUILD_INVENTORY_CREATE:			{DBR_Community::Q_DQT_GUILD_INVENTORY_CREATE(rkResult);}break;
		case DQT_GUILD_INVENTORY_LOAD:				{DBR_Community::Q_DQT_GUILD_INVENTORY_LOAD(rkResult);}break;
		case DQT_GUILD_INVENTORY_LOG_INSERT:		{DBR_Community::Q_DQT_GUILD_INVENTORY_LOG_INSERT(rkResult);}break;
		case DQT_GUILD_INVENTORY_LOG_SELECT:		{DBR_Community::Q_DQT_GUILD_INVENTORY_LOG_SELECT(rkResult);}break;
		case DQT_GUILD_INVENTORY_LOG_DELETE:		{DBR_Community::Q_DQT_GUILD_INVENTORY_LOG_DELETE(rkResult);}break;
		case DQT_GUILD_INVENTORY_EXTEND:			{DBR_Community::Q_DQT_GUILD_INVENTORY_EXTEND(rkResult);}break;
		case DQT_GUILD_CHECK_NAME:					{DBR_Community::Q_DQT_GUILD_CHECK_NAME(rkResult); }break;
		case DQT_GUILD_RENAME:						{DBR_Community::Q_DQT_GUILD_RENAME(rkResult); }break;
		case DQT_GUILD_CHANGE_OWNER:				{DBR_Community::Q_DQT_GUILD_CHANGE_OWNER(rkResult); }break;
		case DQT_GUILD_SENDMAIL:					{DBR_Community::Q_DQT_GUILD_SENDMAIL(rkResult); }break;
		case DQT_GUILD_SELECT_OWNER_LAST_LOGIN_DAY:	{DBR_Community::Q_DQT_GUILD_SELECT_OWNER_LAST_LOGIN_DAY(rkResult, false); }break;
		case DQT_GUILD_INIT_OWNER_LAST_LOGIN_DAY:	{DBR_Community::Q_DQT_GUILD_SELECT_OWNER_LAST_LOGIN_DAY(rkResult, true); }break;
		case DQT_GUILD_SELECT_NEXT_OWNER:			{DBR_Community::Q_DQT_GUILD_SELECT_NEXT_OWNER(rkResult); }break;
		case DQT_GUILD_COMMON:						{DBR_Community::Q_DQT_GUILD_COMMON(rkResult); }break;
		case DQT_GUILD_MERCENARY_SAVE:				{DBR_Community::Q_DQT_GUILD_MERCENARY_SAVE(rkResult);}break;
		case DQT_GUILD_ENTRANCEOPEN_SAVE:			{DBR_Community::Q_DQT_GUILD_ENTRANCEOPEN_SAVE(rkResult);}break;
		case DQT_GUILD_ENTRANCEOPEN_LIST:			{DBR_Community::Q_DQT_GUILD_ENTRANCEOPEN_LIST(rkResult);}break;
		case DQT_GUILD_REQ_ENTRANCE:				{DBR_Community::Q_DQT_GUILD_REQ_ENTRANCE(rkResult);}break;
		case DQT_GUILD_REQ_ENTRANCE_CANCEL:			{DBR_Community::Q_DQT_GUILD_REQ_ENTRANCE_CANCEL(rkResult);}break;
		case DQT_GUILD_APPLICANT_LIST:				{DBR_Community::Q_DQT_GUILD_APPLICANT_LIST(rkResult);}break;
		case DQT_GUILD_ENTRANCE_PROCESS:			{DBR_Community::Q_DQT_GUILD_ENTRANCE_PROCESS(rkResult);}break;		
		case DQT_LOAD_MISSION_REPORT:				{Q_DQT_LOAD_MISSION_REPORT(rkResult);	}break;
		case DQT_SAVE_MISSION_REPORT:				{Q_DQT_SAVE_MISSION_REPORT(rkResult);	}break;
		case DQT_LOAD_MISSION_RANK:					{Q_DQT_LOAD_MISSION_RANK(rkResult);	}break;
		case DQT_SAVE_MISSION_RANK:					{Q_DQT_SAVE_MISSION_RANK(rkResult);	}break;
		case DQT_SAVECHARACTER_MAPINFO:				{Q_DQT_SAVECHARACTER_MAPINFO(rkResult);	}break;
		case DQT_SAVECHARACTER_EXTERN:				{Q_DQT_SAVECHARACTER_EXTERN(rkResult);	}break;			
		case DQT_DEF_PVP_TIME:						{Q_DQT_DEF_PVP_TIME(rkResult);	}break;
		case DQT_DEF_PVP_RANK:						{Q_DQT_DEF_PVP_RANK(rkResult);	}break;
		case DQT_PVP_RANK_UPDATE:					{Q_DQT_PVP_RANK_UPDATE(rkResult);}break;
		case DQT_PVP_RANK:							{Q_DQT_PVP_RANK(rkResult);		}break;			
		case DQT_UPDATE_BS_STATUS:					{BSGameUtil::Q_DQT_BATTLE_SQUARE_COMMON(rkResult);}break;
		case DQT_INIT_BS_STATUS:					{BSGameUtil::Q_DQT_BATTLE_SQUARE_COMMON(rkResult);}break;
		case DQT_SAVE_CLIENTOPTION:					{Q_DQT_SAVE_CLIENTOPTION(rkResult);}break;
		case DQT_DEL_SKILLSET:						{Q_DQT_DEL_SKILLSET(rkResult);}break;
		case DQT_SAVE_SKILLSET:						{Q_DQT_SAVE_SKILLSET(rkResult);}break;
		case DQT_LOAD_EMPORIA:						{g_kEmporiaMgr.Locked_Q_DQT_LOAD_EMPORIA(rkResult);}break;
		case DQT_CREATE_EMPORIA:					{g_kEmporiaMgr.Locked_Q_DQT_CREATE_EMPORIA(rkResult);}break;

		case DQT_SAVE_EMPORIA:
		case DQT_SAVE_EMPORIA_PACK:
		case DQT_SAVE_EMPORIA_RESERVE:		
		case DQT_SAVE_EMPORIA_THROW:		
		case DQT_SAVE_EMPORIA_DELETE:			
		case DQT_SAVE_EMPORIA_BATTLE_STATE:	
		case DQT_CREATE_EMPORIA_TOURNAMENT:
		case DQT_SAVE_EMPORIA_TOURNAMENT:
		case DQT_SAVE_EMPORIA_MERCENARY:
		case DQT_SAVE_EMPORIA_FUNCTION:			
		case DQT_UPDATE_EMPORIA_FUNCTION:			{g_kEmporiaMgr.Locked_Q_DQT_SAVE_EMPORIA(rkResult);}break;

		case DQT_USER_EVENT:						{Q_DQT_USER_EVENT(rkResult);}break;			
		case DQT_CHANGE_CHARACTER_NAME:				{g_kGMProcessMgr.Q_DQT_CHANGE_CHARACTER_NAME(rkResult);}break;
		case DQT_CHANGE_CHARACTER_STATE:			{g_kGMProcessMgr.Q_DQT_CHANGE_CHARACTER_STATE(rkResult);}break;
		case DQT_CHANGE_CHARACTER_POS:				{g_kGMProcessMgr.Q_DQT_CHANGE_CHARACTER_POS(rkResult);}break;
		case DQT_CHANGE_GM_LEVEL:					{g_kGMProcessMgr.Q_DQT_CHANGE_GM_LEVEL(rkResult);}break;
		case DQT_CHANGE_QUEST_INFO:					{g_kGMProcessMgr.Q_DQT_CHANGE_QUEST_INFO(rkResult);}break;
		case DQT_CHANGE_QUEST_ENDED:				{g_kGMProcessMgr.Q_DQT_CHANGE_QUEST_INFO(rkResult);}break;
		case DQT_CHANGE_SKILL_INFO:					{g_kGMProcessMgr.Q_DQT_CHANGE_SKILL_INFO(rkResult);}break;
		case DQT_CHANGE_CHARACTER_FACE:				{g_kGMProcessMgr.Q_DQT_CHANGE_CHARACTER_FACE(rkResult);}break;
		case DQT_CHANGE_CP:							{g_kGMProcessMgr.Q_DQT_CHANGE_CP(rkResult);}break;			
		case DQT_COUPLE_INFO_SELECT:				{DBR_Community::Q_DQT_COUPLE_INFO_SELECT(rkResult);};break;			
		case DQT_COUPLE_UPDATE:						{DBR_Community::Q_DQT_COUPLE_UPDATE(rkResult);};break;			
		case DQT_TAKE_COUPON_REWARD:				{Q_DQT_TAKE_COUPON_REWARD(rkResult);}break;			
		case DQT_DEF_LOAD_COUPONEVENT:				{Q_DQT_DEF_LOAD_COUPONEVENT(rkResult);}break;		
		case DQT_DEF_MYHOME_DEFAULT_ITEM:			{Q_DQT_DEF_MYHOME_DEFAULT_ITEM(rkResult);} break;
		case DQT_DEF_PET_BONUSSTATUS:				{Q_DQT_DEF_PET_BONUSSTATUS(rkResult);}break;
		case DQT_PCROOM:							{Q_DQT_PCROOM(rkResult);} break;
		case DQT_PCCAFE:							{Q_DQT_PCCAFE(rkResult);} break;
		case DQT_PCCAFE_ABIL:						{Q_DQT_PCCAFE_ABIL(rkResult);} break;
		case DQT_CLEAN_DELETE_CHARACTER:			{Q_DQT_CLEAN_DELETE_CHARACTER(rkResult);}break;
		case DQT_CHANGE_DELETED_CHARACTER_NAME:		{Q_DQT_CHANGE_DELETED_CHARACTER_NAME(rkResult);}break;
		case DQT_CHANGE_ACHIEVEMENT:				{g_kGMProcessMgr.Q_DQT_CHANGE_ACHIEVEMENT(rkResult);}break;
		case DQT_OXQUIZ_EVENT:						{Q_DQT_OXQUIZ_EVENT(rkResult);}break;
		case DQT_OXQUIZ_UPDATE_STATE:				{Q_DQT_OXQUIZ_UPDATE_STATE(rkResult);}break;
		case DQT_LOAD_MACROCHECKTABLE:				{Q_DQT_LOAD_MACROCHECKTABLE(rkResult);}break;			
		case DQT_GMORDER_SELECT_MEMBER_GMLEVEL:		{ g_kGMProcessMgr.Q_DQT_GMORDER_SELECT_MEMBER_GMLEVEL(rkResult);} break;

		case DQT_LOAD_LOCAL_LIMITED_ITEM:
		case DQT_LOAD_LOCAL_LIMITED_ITEM_CONTROL:
		case DQT_CHECK_LIMIT_LIMITED_ITEM_RECORD:
		case DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD:
		case DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD_POP:
		case DQT_SYNC_LOCAL_LIMITED_ITEM:
		case DQT_UPDATE_SAFE_LIMITED_ITEM_RECORD:	{ g_kItemDropControl.OnDBProcess(rkResult); }break;			

		case DQT_SAVE_PENALTY:						{Q_DQT_SAVE_PENALTY(rkResult);}break;
		case DQT_CHECK_PENALTY:						{Q_DQT_CHECK_PENALTY(rkResult);}break;			
		case DQT_OXQUIZ_EVENT_STATE:				{Q_DQT_OXQUIZ_EVENT_STATE(rkResult);}break;
		case DQT_LOAD_CREATE_CHARACTER_EVENT_REWARD:{Q_DQT_LOAD_CREATE_CHARACTER_EVENT_REWARD(rkResult);}break;
		case DQT_LOAD_DEFREALTYDEALER:				{Q_DQT_LOAD_DEFREALTYDEALER(rkResult);}break;
		case DQT_LOAD_DEF_RARE_OPT_MAGIC:			{Q_DQT_LOAD_DEF_RARE_OPT_MAGIC(rkResult);}break;
		case DQT_LOAD_DEF_RARE_OPT_SKILL:			{Q_DQT_LOAD_DEF_RARE_OPT_SKILL(rkResult);}break;		
		case DQT_LOAD_DEF_MIXUPITEM:				{Q_DQT_LOAD_DEF_MIXUPITEM(rkResult);}break;
		case DQT_LOAD_BS_GAME:						{BSGameUtil::Q_DQT_LOAD_BS_GAME(rkResult);}break;
		case DQT_LOAD_MYHOME_INFO:					{g_kRealmUserMgr.Q_DQT_LOAD_MYHOME_INFO(rkResult);} break;			
		case DQT_INSERT_EVENT_QUEST_COMPLETESTATUS:	{PgEventQuestSystemUtil::Q_DQT_EVENT_QUEST_COMMON(rkResult);} break;
		case DQT_LOAD_EVENT_QUEST_COMPLETESTATUS:	{PgEventQuestSystemUtil::Q_DQT_LOAD_EVENT_QUEST_COMPLETESTATUS(rkResult);} break;
		case DQT_DELETE_EVENT_QUEST_COMPLETESTATUS:	{PgEventQuestSystemUtil::Q_DQT_EVENT_QUEST_COMMON(rkResult);} break;
		case DQT_UPDATE_EVENT_QUEST_STATUS:			{PgEventQuestSystemUtil::Q_DQT_EVENT_QUEST_COMMON(rkResult);} break;
		case DQT_LOAD_EVENT_QUEST:					{PgEventQuestSystemUtil::Q_DQT_LOAD_EVENT_QUEST(rkResult);} break;
		case DQT_INIT_EVENT_QUEST_STATUS:			{PgEventQuestSystemUtil::Q_DQT_INIT_EVENT_QUEST_STATUS(rkResult);} break;
		case DQT_LOAD_REALM_QUEST:					{PgRealmQuestEvent::Q_DQT_LOAD_REALM_QUEST(rkResult);} break;
		case DQT_UPDATE_REALM_QUEST:				{PgRealmQuestEvent::Q_DQT_UPDATE_REALM_QUEST(rkResult);} break;
		case DQT_LOAD_DEF_GAMBLEMACHINE:			{Q_DQT_LOAD_DEF_GAMBLEMACHINE(rkResult);} break;
		case DQT_LUCKYSTAR_LOAD_EVENT:				{Q_DQT_LUCKYSTAR_LOAD_EVENT(rkResult);}break;
		case DQT_LUCKYSTAR_LOAD_EVENT_SUB:			{Q_DQT_LUCKYSTAR_LOAD_EVENT_SUB(rkResult);}break;
		case DQT_LUCKYSTAR_LOAD_JOINEDUSER:			{Q_DQT_LUCKYSTAR_LOAD_JOINEDUSER(rkResult);}break;
        case DQT_LUCKYSTAR_UPDATE_LASTEVENT:		{Q_DQT_LUCKYSTAR_UPDATE_EVENT_SUB(rkResult);}break;
		case DQT_LUCKYSTAR_UPDATE_EVENT_SUB:		{Q_DQT_LUCKYSTAR_UPDATE_EVENT_SUB(rkResult);}break;
		case DQT_LUCKYSTAR_UPDATE_JOINEDUSER:		{Q_DQT_LUCKYSTAR_UPDATE_JOINEDUSER(rkResult);}break;
		case DQT_LUCKYSTAR_UPDATE_JOINEDUSER_READED:{Q_DQT_LUCKYSTAR_UPDATE_JOINEDUSER_READED(rkResult);}break;
		case DQT_GMCMD_CASHITEMGIFT_INSERT:			{Q_DQT_GMCMD_CASHITEMGIFT_INSERT(rkResult);}break;
		case DQT_GMCMD_CASHITEMGIFT_DELETE:			{Q_DQT_GMCMD_CASHITEMGIFT_DELETE(rkResult);}break;
		case DQT_DEF_BASE_CHARACTER:				{Q_DQT_DEF_BASE_CHARACTER(rkResult); }break;
		case DQT_DEFDYNAMICABILRATE:				{Q_DQT_DEFDYNAMICABILRATE(rkResult); }break;
		case DQT_DEFDYNAMICABILRATE_BAG:			{Q_DQT_DEFDYNAMICABILRATE_BAG(rkResult);	}break;
		case DQT_DEFGMCMD:							{Q_DQT_DEFGMCMD(rkResult); }break;
		case DQT_DEF_QUEST_RANDOM:					{Q_DQT_DEF_QUEST_RANDOM(rkResult); }break;
        case DQT_DEF_SHOP_IN_EMPORIA:				{Q_DQT_DEF_SHOP_IN_EMPORIA(rkResult); }break;
		case DQT_DEF_SHOP_IN_GAME:					{Q_DQT_DEF_SHOP_IN_GAME(rkResult); }break;
		case DQT_DEF_SHOP_IN_STOCK:					{Q_DQT_DEF_SHOP_IN_STOCK(rkResult); }break;
		case DQT_DEF_ITEM_RARITY_CONTROL:			{Q_DQT_DEF_ITEM_RARITY_CONTROL(rkResult);}break;
		case DQT_DEF_ITEM_PLUSUP_CONTROL:			{Q_DQT_DEF_ITEM_PLUSUP_CONTROL(rkResult);}break;
		case DQT_DEF_ITEM_DISASSEMBLE:				{Q_DQT_DEF_ITEM_DISASSEMBLE(rkResult);}break;
		case DQT_DEF_PVP_REWARD:					{Q_DQT_DEF_PVP_REWARD(rkResult);	}break;
		case DQT_DEF_MAPENTITY:						{Q_DQT_DEF_MAPENTITY(rkResult);}break;
		case DQT_DEF_MAPEFFECT:						{Q_DQT_DEF_MAPEFFECT(rkResult);}break;
		case DQT_DEF_MAP_STONE_CONTROL:				{Q_DQT_DEF_MAP_STONE_CONTROL(rkResult);}break;
		case DQT_DEF_TACTICS_QUEST_PSEUDO:			{Q_DQT_DEF_TACTICS_QUEST_PSEUDO(rkResult);}break;
		case DQT_DEF_CASH_ITEM_SHOP:				{Q_DQT_DEF_CASH_ITEM_SHOP(rkResult);}break;
		case DQT_GEMSTORE2:							{Q_DQT_GEMSTORE2(rkResult);}break;
		case DQT_DEF_CASHITEMABILFILTER:			{Q_DQT_DEF_CASHITEMABILFILTER(rkResult);}break;
		case DQT_DEF_GAMBLE:						{Q_DQT_DEF_GAMBLE(rkResult);}break;
		case DQT_LOAD_EVENT_ITEM_REWARD:			{Q_DQT_LOAD_EVENT_ITEM_REWARD(rkResult);}break;
		case DQT_LOAD_TREASURE_CHEST:				{Q_DQT_LOAD_TREASURE_CHEST(rkResult);}break;
		case DQT_PROCESS_SETPLAYERPLAYTIME:			{Q_DQT_PROCESS_SETPLAYERPLAYTIME(rkResult);}break;
		case DQT_UPDATE_SETPLAYERPLAYTIME:			{Q_DQT_UPDATE_SETPLAYERPLAYTIME(rkResult);}break;
		case DQT_LOAD_DEF_MONSTER_GRADE_PROBABILITY:{Q_DQT_LOAD_DEF_MONSTER_GRADE_PROBABILITY(rkResult);}break;
		case DQT_ADMIN_LOAD_DEFLOGITEMCOUNT:		{ItemCountLogHelper::Q_DQT_ADMIN_LOAD_DEFLOGITEMCOUNT(rkResult);}break;
		case DQT_ADMIN_GM_COPYTHAT:					{Q_DQT_ADMIN_GM_COPYTHAT(rkResult);}break;
		case DQT_NONE_RESULT_PROCESS:				{}break;
		case DQT_LOAD_DEF_PVPLEAGUE_TIME:				{PgPvPLeagueUtil::Q_DQT_LOAD_DEF_PVPLEAGUE_TIME(rkResult);}break;
		case DQT_LOAD_DEF_PVPLEAGUE_SESSION:		{PgPvPLeagueUtil::Q_DQT_LOAD_DEF_PVPLEAGUE_SESSION(rkResult);}break;
		case DQT_LOAD_DEF_PVPLEAGUE_REWARD:		{PgPvPLeagueUtil::Q_DQT_LOAD_DEF_PVPLEAGUE_REWARD(rkResult);}break;
		case DQT_LOAD_PVPLEAGUE:							{PgPvPLeagueUtil::Q_DQT_LOAD_PVPLEAGUE(rkResult);} break;
		case DQT_LOAD_PVPLEAGUE_SUB:					{PgPvPLeagueUtil::Q_DQT_LOAD_PVPLEAGUE_SUB(rkResult);} break;
		case DQT_INSERT_PVPLEAGUE:						{PgPvPLeagueUtil::Q_DQT_INSERT_PVPLEAGUE(rkResult);} break;
		case DQT_INSERT_PVPLEAGUE_TEAM:				{PgPvPLeagueUtil::Q_DQT_INSERT_PVPLEAGUE_TEAM(rkResult);} break;
		case DQT_DELETE_PVPLEAGUE_TEAM:				{PgPvPLeagueUtil::Q_DQT_DELETE_PVPLEAGUE_TEAM(rkResult);} break;
		case DQT_INSERT_PVPLEAGUE_TOURNAMENT:		{PgPvPLeagueUtil::Q_DQT_INSERT_PVPLEAGUE_TOURNAMENT(rkResult);} break;
		case DQT_INSERT_PVPLEAGUE_BATTLE:				{PgPvPLeagueUtil::Q_DQT_INSERT_PVPLEAGUE_BATTLE(rkResult);} break;
		case DQT_SET_PVPLEAGUE_BATTLE:					{PgPvPLeagueUtil::Q_DQT_SET_PVPLEAGUE_BATTLE(rkResult);} break;
		case DQT_SET_PVPLEAGUE_STATE:					{PgPvPLeagueUtil::Q_DQT_SET_PVPLEAGUE_STATE(rkResult);} break;
		case DQT_SET_PVPLEAGUE_TEAM:					{PgPvPLeagueUtil::Q_DQT_SET_PVPLEAGUE_TEAM(rkResult);} break;
		case DQT_SET_PVPLEAGUE_TEAM_INDEX:			{PgPvPLeagueUtil::Q_DQT_SET_PVPLEAGUE_TEAM(rkResult);} break;

		case DQT_UPDATE_USER_JOBSKILL_HISTORYITEM:	{Q_DQT_UPDATE_USER_JOBSKILL_HISTORYITEM(rkResult);}break;
		case DQT_SELECT_MEMBERID:					{Q_DQT_SELECT_MEMBERID(rkResult);}break;
		case DQT_SAVE_CHARACTOR_SLOT:				{Q_DQT_SAVE_CHARACTOR_SLOT(rkResult);}break;
		case DQT_FIND_CHARACTOR_EXTEND_SLOT:		{g_kRealmUserMgr.Locked_Q_DQT_FIND_CHARACTOR_EXTEND_SLOT(rkResult);}break;
		case DQT_GET_MEMBER_PREMIUM_SERVICE:		{g_kRealmUserMgr.Locked_Q_DQT_GET_MEMBER_PREMIUM_SERVICE(rkResult);}break;
		case DQT_UPDATE_PREMIUM_CUSTOM_DATA:		{Q_DQT_UPDATE_PREMIUM_CUSTOM_DATA(rkResult);}break;
		case DQT_EVENT_STORE:						{Q_DQT_EVENT_STORE(rkResult);}break;
		case DQT_LOAD_JUMPINGCHAREVENT:				{Q_DQT_LOAD_JUMPINGCHAREVENT(rkResult);}break;
		case DQT_GET_MEMBER_DAILY_REWARD:			{g_kRealmUserMgr.Locked_Q_DQT_GET_MEMBER_DAILY_REWARD(rkResult);}break;
		case DQT_UPDATE_USER_UNLOCK_ALL_CHARACTERS: {Q_DQT_UPDATE_USER_UNLOCK_ALL_CHARACTERS(rkResult);} break;
		default:
			{
				return OnDB_EXECUTE_Item(rkResult);

				//			VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Can't find DB Quest return type[%d]"), __FUNCTIONW__, __LINE__, (int)rkResult.QueryType());
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}break;
		}
	}	

	return S_OK;
}

HRESULT CALLBACK OnDB_EXECUTE_TRAN( CEL::DB_RESULT_TRAN &rkResult )
{
	//	switch(rkResult.QueryType())
	//	{
	//	case DQT_SELECT_PLAYER_DATA:	{ g_kRealmUserMgr.Locked_Q_DQT_SELECT_PLAYER_DATA(rkResult); }break;
	//	default:
	//		{
	return OnDB_EXECUTE_TRAN_Item(rkResult);
	//		}break;
	//	}
	//	return S_OK;
}

#define DT_DEF_PATH "./Table/DR2_Def/"
#define DT_LOCAL_PATH "./Table/DR2_Local/"
bool TableDataQuery( bool bReload )
{// ���ڵ� �������ϰ� ������ ���ÿ�.

	std::wstring kStrQuery;
	bool bIsImmidiate = true;
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_CASH_ITEM_SHOP, _T("EXEC [dbo].[UP_LoadDefCashShop8]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_BASE_CHARACTER, _T("EXEC [dbo].[UP_LoadDefBaseCharacter2]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEFGMCMD, _T("EXEC [dbo].[up_LoadDefGmCommand]"));								if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}	
    {	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_SHOP_IN_EMPORIA, _T("EXEC [dbo].[UP_LoadDefShopInEmporia]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_SHOP_IN_GAME, _T("EXEC [dbo].[UP_LoadDefShopInGame3]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_SHOP_IN_STOCK, _T("EXEC [dbo].[UP_LoadDefShopInStock]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_PVP_REWARD, _T("EXEC [dbo].[Up_LoadDefPvPReward]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_PVP_TIME, L"EXEC [dbo].[up_LoadDefPvPTime]");								if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_QUEST_RANDOM, _T("EXEC [dbo].[UP_LoadDefQuest_Random]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFDYNAMICABILRATE, _T("EXEC [dbo].[UP_LoadDefDynamicAbilRate]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFDYNAMICABILRATE_BAG, _T("EXEC [dbo].[UP_LoadDefDynamicAbilRateBag]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_RARITY_CONTROL, _T("EXEC [dbo].[UP_LoadDefItemRarityControl]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_PLUSUP_CONTROL, _T("EXEC [dbo].[up_LoadDefItemPlusUpControl]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_DISASSEMBLE, _T("EXEC [dbo].[UP_LoadDefItemDisassemble]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MAPEFFECT, _T("EXEC [dbo].[UP_LoadDefMapEffect]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MAPENTITY, _T("EXEC [dbo].[UP_LoadDefMapEntity]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MAP_STONE_CONTROL, _T("EXEC [dbo].[UP_LoadDefMapStoneControl]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_TACTICS_QUEST_PSEUDO, _T("EXEC [dbo].[UP_LoadDefTacticsQuestPseudo]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}	
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_OXQUIZ_EVENT, _T("EXEC [dbo].[up_LoadOXQuizEvent]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}	
		if (!Q_DQT_GEMSTORE( DT_LOCAL_PATH )) goto LABEL_QUERY_ERROR;
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_GEMSTORE2, _T("EXEC [dbo].[up_LoadGemStore2]"));								if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_CASHITEMABILFILTER, _T("EXEC [dbo].[up_LoadDefCashItemAbilFilter]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_GAMBLE, _T("EXEC [dbo].[up_LoadDefGamble]"));									if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_LOCAL_LIMITED_ITEM, _T("EXEC [dbo].[UP_Load_LimitedItem]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_LOCAL_LIMITED_ITEM_CONTROL, _T("EXEC [dbo].[UP_Load_LimitedItemControl]"));if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_PCROOM, _T("EXEC [dbo].[up_LoadPCRoom]"));										if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_PCCAFE, _T("EXEC [dbo].[up_LoadPCCafe]"));										if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_PCCAFE_ABIL, _T("EXEC [dbo].[up_LoadPCCafeAbil]"));								if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_PET_BONUSSTATUS, _T("EXEC [dbo].[up_LoadDefPet_BonusStatus]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_CREATE_CHARACTER_EVENT_REWARD, _T("EXEC [dbo].[up_LoadCreateCharacterEventReward]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEFREALTYDEALER, _T("EXEC [dbo].[up_LoadDefRealtyDealer]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_RARE_OPT_MAGIC, _T("EXEC [dbo].[up_LoadDefRareOptMagic]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_RARE_OPT_SKILL, _T("EXEC [dbo].[up_LoadDefRareOptSkill]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_MIXUPITEM, _T("EXEC [dbo].[up_LoadDefMixupItem]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MYHOME_DEFAULT_ITEM, _T("EXEC [dbo].[up_LoadDefMyHomeDefaultItem]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}	
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_GAMBLEMACHINE, _T("EXEC [dbo].[up_LoadDefGambleMachine]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LUCKYSTAR_LOAD_EVENT, _T("EXEC [dbo].[up_LoadLuckyStarEvent]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_EVENT_ITEM_REWARD, _T("EXEC [dbo].[up_LoadEventItemReward]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_MONSTER_GRADE_PROBABILITY, _T("EXEC [dbo].[UP_LoadDefMonsterGradeProbabilty]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	if ( !Q_DQT_LOAD_DEF_SUPER_GROUND(DT_DEF_PATH "TB_DefSuperGround.csv") ) goto LABEL_QUERY_ERROR; }
	{	if ( !Q_DQT_DEFGEMSTORE(DT_DEF_PATH) ) goto LABEL_QUERY_ERROR; }
	{	if ( !Q_DQT_DEF_DAILY_REWARD(DT_LOCAL_PATH) ) goto LABEL_QUERY_ERROR; }
	{	if ( !Q_DQT_DEF_CART_MISSION_MONSTER(DT_DEF_PATH) ) goto LABEL_QUERY_ERROR; }
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_PVPLEAGUE_TIME, _T("EXEC [dbo].[up_LoadDefPvPLeagueTime]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_PVPLEAGUE_SESSION, _T("EXEC [dbo].[up_LoadDefPvPLeagueSession]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_PVPLEAGUE_REWARD, _T("EXEC [dbo].[up_LoadDefPvPLeagueReward]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_SHOP, _T("EXEC [dbo].[UP_LoadDefJobSkill_Shop]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_TREASURE_CHEST, _T("EXEC [dbo].[up_LoadTreasureChest]"));	if(S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_TREASURE_CHEST, _T("EXEC [dbo].[up_LoadDefTreasureChest]"));	if(S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_EVENT_STORE, _T("EXEC [dbo].[up_LoadEventStore]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_JUMPINGCHAREVENT, _T("EXEC [dbo].[up_LoadDefJumpingCharEvent]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_JOBSKILL_EVENT_LOCATION, _T("EXEC [dbo].[up_DefEvent_JobSkill_Location]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	return true;

LABEL_QUERY_ERROR:
	INFO_LOG( BM::LOG_LV1, _T("Query Fail... Cause = ") << kStrQuery );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
#undef DT_LOCAL_PATH
#undef DT_DEF_PATH

bool LoadDB(bool bReload)
{
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Start DB Load") );

	CTableDataManager kTempTDM;
	g_kTblDataMgr.swap(kTempTDM, bReload);	//���� ������ ���.
	g_kTblDataMgr.Clear(bReload);//���̺� ������
	
	if( csvdb::def::load_items("./") &&
		(!PgDBCache::TableDataQuery( bReload ) // Dump ������ �ʿ��� ���̺�
		|| !TableDataQuery( bReload )) )	// Contents ���� ����
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Complete DB Load = Failed(DB Query)!!") );		
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));

		g_kTblDataMgr.Clear();
		g_kTblDataMgr.swap(kTempTDM, bReload);	//����� ������ �ǵ�����.

		return false;
	}

	if( PgDBCache::DisplayErrorMsg()
	||	DefLoadErrorLog::Display() )
	{
		return false; // ���� ����
	}

	// Def Building
	int iDefLoad = 
		PgControlDefMgr::EDef_MonsterDefMgr
		|	PgControlDefMgr::EDef_SkillDefMgr
		|	PgControlDefMgr::EDef_ItemDefMgr
		|	PgControlDefMgr::EDef_RareDefMgr
		|	PgControlDefMgr::EDef_ItemEnchantDefMgr
		|	PgControlDefMgr::EDef_ClassDefMgr
		|	PgControlDefMgr::EDef_ItemBagMgr
		|	PgControlDefMgr::EDef_StoreMgr
		|	PgControlDefMgr::EDef_ItemOptionMgr
		|	PgControlDefMgr::EDef_ItemSetDefMgr
		|	PgControlDefMgr::EDef_EffectDefMgr
		|	PgControlDefMgr::EDef_PropertyMgr
		|	PgControlDefMgr::EDef_GroundEffect
		|	PgControlDefMgr::EDef_RecommendationItem
		|	PgControlDefMgr::EDef_Pet
		|	PgControlDefMgr::EDef_MapDefMgr
		|	PgControlDefMgr::EDef_PremiumMgr
		|	PgControlDefMgr::EDef_Default;

	SReloadDef sReloadDef;
	if ( !bReload )
	{
		iDefLoad |= PgControlDefMgr::EDef_OnlyFirst;
	}

	g_kTblDataMgr.GetReloadDef(sReloadDef, iDefLoad);
	g_kControlDefMgr.StoreValueKey(BM::GUID::Create());
	INFO_LOG(BM::LOG_LV7, _T("Set StoreKey LoadDB::")<< g_kControlDefMgr.StoreValueKey());

	if ( !g_kControlDefMgr.Update( sReloadDef, iDefLoad ) )
	{
		return false;
	}

	if(!bReload)
	{ // Client Side TB.bin Load test
		PgControlDefMgr kDefTest;
		SReloadDef kTestDef;
		int const iTestLoadDef = ControlDefUtil::iClientLoadDef;
		g_kTblDataMgr.GetReloadDef(kTestDef, iTestLoadDef);
		if( !kDefTest.Update(kTestDef, iTestLoadDef) )
		{
			return false;
		}
	}

	// ETC
	if (!bReload)
	{
		g_kContentsTask.Build(*sReloadDef.pkMissionResult, *sReloadDef.pkMissionCandi, *sReloadDef.pkMissionRoot);

		g_kEmporiaMgr.LoadDB();
		g_kJumpingCharEventMgr.Build();
	}
	g_kTblDataMgr.Dump( L"./Table/" );

	CONT_DEF_MISSION_ROOT const * pkMissionRoot = NULL;
	g_kTblDataMgr.GetContDef(pkMissionRoot);
	LoadMissionRank(pkMissionRoot);

	CONT_DEF_QUEST_REWARD const* pkQuestReward = NULL;
	CONT_DEF_QUEST_RESET_SCHEDULE const* pkQuestResetSchedule = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);
	g_kTblDataMgr.GetContDef(pkQuestResetSchedule);
	if( !g_kQuestMng.Build(pkQuestReward, pkQuestResetSchedule) )
	{
		return false;
	}

	if (!bReload)
	{
		bool bRet = true;
		CONT_DEF_FILTER_UNICODE const* pkFilterUnicode = NULL;
		g_kTblDataMgr.GetContDef(pkFilterUnicode);
		if( pkFilterUnicode->empty() )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"[DR2_Def].[dbo].[TB_DefFilterUnicode] row count is 0");
		}
		CONT_DEF_FILTER_UNICODE::const_iterator filter_iter = pkFilterUnicode->begin();
		while( pkFilterUnicode->end() != filter_iter )
		{
			CONT_DEF_FILTER_UNICODE::value_type const& rkFilter = (*filter_iter);

			if( rkFilter.cEnd < rkFilter.cStart )
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Unicode Filter Error, can't [End] is greater than the [Start], FuncCode["<<rkFilter.iFuncCode<<L"] Start["<<rkFilter.cStart<<L", "<<HEX(rkFilter.cStart)<<L"] ~ End["<<rkFilter.cEnd<<L", "<<HEX(rkFilter.cEnd)<<L"] FilterType["<<rkFilter.bFilterType<<L"]");
				bRet = false;
			}
			else
			{
				INFO_LOG(BM::LOG_LV6, __FL__<<L"Unicode Filter Regist, FuncCode["<<rkFilter.iFuncCode<<L"] Start["<<rkFilter.cStart<<L", "<<HEX(rkFilter.cStart)<<L"] ~ End["<<rkFilter.cEnd<<L", "<<HEX(rkFilter.cEnd)<<L"] FilterType["<<rkFilter.bFilterType<<L"]");
			}

			g_kUnicodeFilter.AddRange(rkFilter.iFuncCode, rkFilter.bFilterType, rkFilter.cStart, rkFilter.cEnd);
			++filter_iter;
		}

		if( !bRet )
		{
			return false;
		}
	}

	CONT_DEF_QUEST_RANDOM const* pkQuestRandom = NULL;
	CONT_DEF_QUEST_RANDOM_EXP const* pkQuestRandomExp = NULL;
	CONT_DEF_QUEST_RANDOM_TACTICS_EXP const* pkQuestRandomTacticsExp = NULL;
	g_kTblDataMgr.GetContDef(pkQuestRandom);
	g_kTblDataMgr.GetContDef(pkQuestRandomExp);
	g_kTblDataMgr.GetContDef(pkQuestRandomTacticsExp);
	if( !g_kQuestMng.BuildRandom(pkQuestRandom, pkQuestRandomExp, pkQuestRandomTacticsExp) )
	{
		return false;
	}

	if( !PgQuestInfoVerifyUtil::DisplayResult() )
	{
		return false;
	}

	{
		CEL::DB_QUERY kQuery(DT_DEF, DQT_LOAD_BS_GAME, _T("EXEC [dbo].[UP_LoadDefBattleSquare]"));
		PushCSVQuery(kQuery, true);

		BSGameUtil::PgVerify kBSCheck;
		if( kBSCheck.Check() )
		{
			BM::Stream kPacket((bReload)? PT_A_N_REQ_RELOAD_BS_GAME: PT_A_N_REQ_INIT_BS_GAME);
			::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
		}
		else
		{
			return false;
		}
	}

	JSMgr::PgJobSkillVerify kJSCheck;
	if(false == kJSCheck.Check())
	{
		return false; // ���� ����
	}

	{// ��� ��û�� ���
		BM::Stream kPacket(PT_A_N_REQ_GET_GUILD_APPLICANT_LIST);
		::SendToGuildMgr(kPacket);
	}

	{// ���ݰ� �α� ����(2�ְ� ���� ������)
		BM::Stream kPacket(PT_A_N_REQ_DELETE_GUILD_INVENTORY_LOG);
		::SendToGuildMgr(kPacket);
	}

	{
		BM::Stream kPacket(PT_A_N_REQ_INIT_GUILD_OWNER_LAST_LOGIN_DAY);
		::SendToGuildMgr(kPacket);
	}

	{
		BM::Stream kPacket(PT_N_N_REQ_RELOAD_EVENT_QUEST);
		::SendToRealmContents(PMET_EVENTQUEST, kPacket);
	}
	{
		BM::Stream kPacket(PT_N_N_NFY_BUILD_BASE_ITEM_INFO);
		::SendToRealmContents(PMET_WEB_HELPER, kPacket);
	}

	if( !g_kContentsTask.ParseXml() )
	{
		return false;
	}

	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Complete DB Load") );

	if ( !bReload )
	{
		// PvP Ranking Season ������ �ؾ��ϴ��� �˻�
		// �ֱ⸦ �����ؾ� �ϴ��� �˻�
		{
			CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_PVP_RANK, _T("EXEC [dbo].[up_LoadDefPvPRankingSeason]"));
			if( S_OK != g_kCoreCenter.PushQuery(kQuery, true) )
			{
				INFO_LOG( BM::LOG_LV1, _T("Query Fail Data Recovery... Cause = ") << kQuery.Command() );
				return false;
			}

			if ( !g_kPvPRankingSceduler.CheckUpdate() )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("PvPRankingSceduler Checking Error!!") );
				return false;
			}

			// HardCore Dungeon
			CONT_DEF_PLAYLIMIT_INFO	const * pkContDefPlayLimitInfo = NULL;
			g_kTblDataMgr.GetContDef( pkContDefPlayLimitInfo );

			CONT_DEF_PLAYLIMIT_INFO::const_iterator playlimit_itr = pkContDefPlayLimitInfo->find( HardCoreDungeon );
			if ( playlimit_itr != pkContDefPlayLimitInfo->end() )
			{
				g_kHardCoreDungeonMgr.Regist( playlimit_itr->first );
			}

			// PvP League
			playlimit_itr = pkContDefPlayLimitInfo->find( PvP_Lobby_GroundNo_League );
			if ( pkContDefPlayLimitInfo->end() != playlimit_itr )
			{
				BM::Stream kPacket(PT_A_N_REQ_LOAD_PVPLEAGUE);
				::SendToRealmContents(PMET_PVP_LEAGUE, kPacket);
			}
		}
	}

	return true;
}

bool Q_DQT_DEF_MYHOME_DEFAULT_ITEM(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_MYHOME_DEFAULTITEM kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

	while(kIter != rkResult.vecArray.end())
	{
		CONT_MYHOME_DEFAULTITEM::key_type			kKey;
		CONT_MYHOME_DEFAULTITEM::mapped_type		kValue;
		kIter->Pop(kKey);							++kIter;
		kIter->Pop(kValue.iItemNo);					++kIter;
		kCont.insert(std::make_pair(kKey,kValue));
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool Q_DQT_DEF_GAMBLE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	if(2 > rkResult.vecResultCount.size())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	int const iGambleNum = (*count_iter);	++count_iter;

	CONT_GAMBLE kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

	for(int i = 0;i < iGambleNum;++i)
	{
		CONT_GAMBLE::key_type		kKey;
		CONT_GAMBLE::mapped_type	kValue;

		kIter->Pop(kKey);							++kIter;
		kIter->Pop(kValue.iContainerNo);			++kIter;
		kIter->Pop(kValue.bTimeType);				++kIter;
		kIter->Pop(kValue.sUseTime);				++kIter;
		kIter->Pop(kValue.iRarityControl);			++kIter;
		kIter->Pop(kValue.iPlusUpControl);			++kIter;

		kCont.insert(std::make_pair(kKey,kValue));
	}

	int const iShoutItemNum = (*count_iter);	++count_iter;

	CONT_GAMBLE_SHOUT_ITEM kContItem;

	for(int i = 0;i < iShoutItemNum;++i)
	{
		CONT_GAMBLE_SHOUT_ITEM::key_type		kKey;
		CONT_GAMBLE_SHOUT_ITEM::mapped_type		kValue;

		kIter->Pop(kKey.iGambleNo);					++kIter;
		kIter->Pop(kKey.iItemNo);					++kIter;
		kIter->Pop(kValue);							++kIter;

		kContItem.insert(std::make_pair(kKey,kValue));
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	g_kTblDataMgr.SetContDef(kContItem);
	return true;
}

bool Q_DQT_DEF_CASHITEMABILFILTER(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_CASHITEMABILFILTER kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

	while(kIter != rkResult.vecArray.end())
	{
		CONT_CASHITEMABILFILTER::value_type kValue;
		kIter->Pop(kValue);					++kIter;
		kCont.insert(kValue);
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool Q_DQT_GEMSTORE2(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	
	CONT_DEFANTIQUE DefaultContAntique;
	CONT_DEFANTIQUE kContAntique;

	BM::GUID kNpcGuid;
	int iMenu = 0;
	int iOrderIndex = 0;
	int iItemNo = 0;
	int iItemCount = 0;
	int iCP = 0;

	std::wstring NationCodeStr;

	while( rkResult.vecArray.end() != itor )
	{
		(*itor).Pop( NationCodeStr );	++itor;
		(*itor).Pop( kNpcGuid );		++itor;
		(*itor).Pop( iMenu );			++itor;
		(*itor).Pop( iOrderIndex );		++itor;
		(*itor).Pop( iItemNo );			++itor;
		(*itor).Pop( iItemCount );		++itor;
		(*itor).Pop( iCP );				++itor;

		switch(iMenu)
		{
		case EGT_ANTIQUE:
		case EGT_ITEM_SHOP:
			{//��ǰ, Ŭ�� E_GEMSTORE_TYPE ����
				static CONT_DEFANTIQUE::mapped_type element;
				static int iIndex = 0;

				for(int i = 0;i < MAX_DEFGEMS_NUM;++i)
				{
					int iGemNo = 0;
					short siCount = 0;
					(*itor).Pop( iGemNo );	++itor;
					(*itor).Pop( siCount );	++itor;

					if((0 == i) && (iGemNo > 0) && (siCount > 0))
					{
						TBL_PAIR_KEY_INT const kKey(iMenu,iGemNo);
						if( DBCacheUtil::IsDefaultNation(NationCodeStr) )
						{
							auto kPair = DefaultContAntique.insert(std::make_pair(kKey, element));
							if(kPair.second)
							{
								(*kPair.first).second.iNeedItem = iGemNo;
								(*kPair.first).second.iNeedCount = siCount;
							}
							CONT_DEF_ANTIQUE_ARTICLE & kArticles = (*kPair.first).second.kContArticles;
							kArticles.insert(std::make_pair(iIndex, std::make_pair(iItemNo,iItemCount)));
						}
						else
						{
							if( DBCacheUtil::IsCanNation< CONT_DEFANTIQUE::key_type, BM::vstring, DefLoadErrorLog::AddError >(NationCodeStr, kKey, __FUNCTIONW__, __LINE__) )
							{
								auto kPair = kContAntique.insert(std::make_pair(kKey, element));
								if(kPair.second)
								{
									(*kPair.first).second.iNeedItem = iGemNo;
									(*kPair.first).second.iNeedCount = siCount;
								}
								CONT_DEF_ANTIQUE_ARTICLE & kArticles = (*kPair.first).second.kContArticles;
								kArticles.insert(std::make_pair(iIndex, std::make_pair(iItemNo,iItemCount)));
							}
						}

						++iIndex;
					}
				}
			}break;
		default:
			{
				//���� ���߿���
			}break;
		}
	}

	if( DefaultContAntique.size() || kContAntique.size() )
	{
		kContAntique.insert(DefaultContAntique.begin(), DefaultContAntique.end());
		
		{//������ �ִ� �׸� ����
			TABLE_LOCK(CONT_DEFANTIQUE) kObjLock;
			g_kTblDataMgr.GetContDef(kObjLock);
			CONT_DEFANTIQUE const * pDefAntique = kObjLock.Get();
			if( pDefAntique )
			{
				kContAntique.insert(pDefAntique->begin(), pDefAntique->end());
			}
		}

		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kContAntique);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_GEMSTORE2 LOAD FAIL COUNT IS 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_GEMSTORE(const char* pkTBPath)
{
	CONT_GEMSTORE kCont;

	/*Code generated by codegen version: 0.0.1*/
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_GemStore.csv");
    const size_t f_NpcGuid = 1, f_ItemNo = 2, f_Gem1 = 3, f_GemCount1 = 4, f_CP = 13;
    while( csv.next_row() /*skip header*/ )
	{
		CONT_GEMSTORE_ARTICLE::mapped_type element;

		const int iItemNo = csv.col_int(f_ItemNo);
		const BM::GUID kNpcGuid(UNI(csv.col(f_NpcGuid)));

		for(int i = 0;i < MAX_GEMS_NUM;++i)
		{
			const int iGemNo = csv.col_int(f_Gem1 + (i << 1)); // Gem0N
			const short siCount = csv.col_int(f_GemCount1 + (i << 1)); // GemCount0N

			if((iGemNo > 0) && (siCount > 0))
			{
				if(false == element.kContGems.insert(std::make_pair(iGemNo,siCount)).second)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_GEMSTORE LOAD FAIL GEM:") << iGemNo);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}
		element.iCP = csv.col_int(f_CP);

		if(false == kCont[kNpcGuid].kContArticles.insert(std::make_pair(iItemNo,element)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_GEMSTORE LOAD FAIL ITEMNO:") << iItemNo);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( kCont.size() )
	{
		g_kTblDataMgr.SetContDef(kCont);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_GEMSTORE LOAD FAIL COUNT IS 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEFGEMSTORE(const char* pkTBPath)
{

	CONT_DEFGEMSTORE DefaultCont;
	CONT_DEFGEMSTORE kCont;
	
	CONT_DEFANTIQUE DefaultContAntique;
	CONT_DEFANTIQUE kContAntique;

	BM::GUID kNpcGuid;
	int iMenu = 0;
	int iOrderIndex = 0;
	int iItemNo = 0;
	int iItemCount = 0;

	std::wstring NationCodeStr;

    /*Code generated by codegen version: 0.0.1*/
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefGemStore.csv");
    const size_t f_NationCodeStr = 0, f_NpcGuid = 2, f_Menu = 3, f_OrderIndex = 4,
        f_ItemNo = 5, f_ItemCount = 6, f_Gem1 = 7, f_GemCount1 = 8;

    while( csv.next_row() /*skip header*/ )
	{
		NationCodeStr = UNI(csv.col(f_NationCodeStr));
		kNpcGuid = BM::GUID(csv.col(f_NpcGuid));
		iMenu = csv.col_int(f_Menu);
		iOrderIndex = csv.col_int(f_OrderIndex);
		iItemNo = csv.col_int(f_ItemNo);
		iItemCount = csv.col_int(f_ItemCount);

		if(EGT_ANTIQUE == iMenu)
		{//��ǰ, Ŭ�� E_GEMSTORE_TYPE ����
			static CONT_DEFANTIQUE::mapped_type element;
			static int iIndex = 0;

			for(int i = 0;i < MAX_DEFGEMS_NUM;++i)
			{
				int iGemNo = csv.col_int(f_Gem1 + (i << 1));
				short siCount = csv.col_int(f_GemCount1 + (i << 1));

				if((0 == i) && (iGemNo > 0) && (siCount > 0))
				{
					TBL_PAIR_KEY_INT const kKey(iMenu,iGemNo);
					if( DBCacheUtil::IsDefaultNation(NationCodeStr) )
					{
						auto kPair = DefaultContAntique.insert(std::make_pair(kKey, element));
						if(kPair.second)
						{
							(*kPair.first).second.iNeedItem = iGemNo;
							(*kPair.first).second.iNeedCount = siCount;
						}
						CONT_DEF_ANTIQUE_ARTICLE & kArticles = (*kPair.first).second.kContArticles;
						kArticles.insert(std::make_pair(iIndex, std::make_pair(iItemNo,iItemCount)));
					}
					else
					{
						if( DBCacheUtil::IsCanNation< CONT_DEFANTIQUE::key_type, BM::vstring, DefLoadErrorLog::AddError >(NationCodeStr, kKey, __FUNCTIONW__, __LINE__) )
						{
							auto kPair = kContAntique.insert(std::make_pair(kKey, element));
							if(kPair.second)
							{
								(*kPair.first).second.iNeedItem = iGemNo;
								(*kPair.first).second.iNeedCount = siCount;
							}
							CONT_DEF_ANTIQUE_ARTICLE & kArticles = (*kPair.first).second.kContArticles;
							kArticles.insert(std::make_pair(iIndex, std::make_pair(iItemNo,iItemCount)));
						}
					}

					++iIndex;
				}
			}
		}
		else
		{
			CONT_DEFGEMSTORE_ARTICLE::key_type	kKey;
			CONT_DEFGEMSTORE_ARTICLE::mapped_type element;

			kKey.iMenu = iMenu;
			kKey.iOrderIndex = iOrderIndex;
			kKey.iItemNo = iItemNo;

			for(int i = 0;i < MAX_DEFGEMS_NUM;++i)
			{
				SDefGemsData kData;
				kData.iGemNo = csv.col_int(f_Gem1 + (i << 1));
				kData.sCount = csv.col_int(f_GemCount1 + (i << 1));

				if((kData.iGemNo > 0) && (kData.sCount > 0))
				{
					element.kContGems.push_back(kData);
					element.iMenu		= iMenu;
					element.iOrderIndex = iOrderIndex;
					element.iItemCount	= iItemCount;
				}
			}
			if( DBCacheUtil::IsDefaultNation(NationCodeStr) )
			{
				if(false == DefaultCont[kNpcGuid].kContArticles.insert(std::make_pair(kKey,element)).second)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_GEMSTORE LOAD FAIL ITEMNO:") << iItemNo);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
			else
			{
				if( DBCacheUtil::IsCanNation< CONT_DEFGEMSTORE::key_type, BM::vstring, DefLoadErrorLog::AddError >(NationCodeStr, kNpcGuid, __FUNCTIONW__, __LINE__) )
				{
					if(false == kCont[kNpcGuid].kContArticles.insert(std::make_pair(kKey,element)).second)
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_GEMSTORE LOAD FAIL ITEMNO:") << iItemNo);
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}
				}
			}
		}
	}

	if( DefaultCont.size() || kCont.size() )
	{
		CONT_DEFGEMSTORE::const_iterator iter = kCont.begin();
		while( iter != kCont.end() )
		{
			CONT_DEFGEMSTORE::iterator find_iter = DefaultCont.find(iter->first);
			if( find_iter != DefaultCont.end() )
			{
				CONT_DEFGEMSTORE_ARTICLE::const_iterator Articles_iter = iter->second.kContArticles.begin();
				while( Articles_iter != iter->second.kContArticles.end() )
				{
					find_iter->second.kContArticles.insert(*Articles_iter);
					++Articles_iter;
				}
			}
			else
			{
				DefaultCont.insert(*iter);
			}
			++iter;
		}

		g_kTblDataMgr.SetContDef(DefaultCont);
	}

	if( DefaultContAntique.size() || kContAntique.size() )
	{
		kContAntique.insert(DefaultContAntique.begin(), DefaultContAntique.end());

		{//������ �ִ� �׸� ����
			TABLE_LOCK(CONT_DEFANTIQUE) kObjLock;
			g_kTblDataMgr.GetContDef(kObjLock);
			CONT_DEFANTIQUE const * pDefAntique = kObjLock.Get();
			if( pDefAntique )
			{
				kContAntique.insert(pDefAntique->begin(), pDefAntique->end());
			}
		}

		g_kTblDataMgr.SetContDef(kContAntique);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_GEMSTORE LOAD FAIL COUNT IS 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEF_CASH_ITEM_SHOP(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Can't load TB_DefTacticsLevel table] Result: "<<rkResult.eRet);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator citer = rkResult.vecArray.begin();
	CEL::DB_RESULT_COUNT::const_iterator return_iter = rkResult.vecResultCount.begin();

	if((citer != rkResult.vecArray.end()) && (return_iter != rkResult.vecResultCount.end()))
	{
		CONT_DEF_CASH_SHOP kContShop;

		int iEntityCount = (*return_iter); ++return_iter;

		CONT_DEF_CASH_SHOP::mapped_type kCategory;
		int iCIdx = 0;

		for(int i = 0;i < iEntityCount;i++)
		{
			(*citer).Pop(iCIdx);				++citer;
			(*citer).Pop(kCategory.bStep);		++citer;
			(*citer).Pop(kCategory.bRank);		++citer;
			(*citer).Pop(kCategory.iRootIdx);	++citer;
			(*citer).Pop(kCategory.iParentIdx); ++citer;
			(*citer).Pop(kCategory.bIsUsed);	++citer;
			(*citer).Pop(kCategory.wstrName);	++citer;
			kContShop.insert(std::make_pair(iCIdx,kCategory));
		}

		CONT_DEF_CASH_SHOP_ARTICLE kContArticle;
		CONT_DEF_CASH_SHOP_ARTICLE::mapped_type kArticle;

		iEntityCount = (*return_iter); ++return_iter;

		BM::DBTIMESTAMP_EX kDate;

		for(int i = 0;i < iEntityCount;i++)
		{
			(*citer).Pop(kArticle.iIdx);			++citer;
			(*citer).Pop(kArticle.iCategoryIdx);	++citer;
			(*citer).Pop(kArticle.bU);				++citer;
			(*citer).Pop(kArticle.bV);				++citer;
			(*citer).Pop(kArticle.bUVIndex);		++citer;
			(*citer).Pop(kArticle.wstrIconPath);	++citer;
			(*citer).Pop(kArticle.wstrName);		++citer;
			(*citer).Pop(kArticle.wstrDiscription);	++citer;
			(*citer).Pop(kArticle.bState);			++citer;
			(*citer).Pop(kArticle.bSaleType);		++citer;
			(*citer).Pop(kDate);					++citer;
			kArticle.kSaleStartDate = kDate;
			(*citer).Pop(kDate);					++citer;
			kArticle.kSaleEndDate = kDate;
			(*citer).Pop(kArticle.iMileage);		++citer;
			(*citer).Pop(kArticle.bDisplayTypeIdx);	++citer;
			(*citer).Pop(kArticle.wDisplayRank);	++citer;
			kContArticle.insert(std::make_pair(kArticle.iIdx,kArticle));
		}

		CONT_CASH_SHOP_ITEM_PRICE::mapped_type kPrice;

		iEntityCount = (*return_iter); ++return_iter;

		int iItemIdx = 0;

		for(int i = 0;i < iEntityCount;i++)
		{
			(*citer).Pop(iItemIdx);								++citer;
			(*citer).Pop(kPrice.iItemUseTime);					++citer;
			(*citer).Pop(kPrice.iCash);							++citer;
			(*citer).Pop(kPrice.iDiscountCash);					++citer;
			(*citer).Pop(kPrice.IsDiscount);					++citer;
			(*citer).Pop(kPrice.bTimeType);						++citer;
			(*citer).Pop(kPrice.iExtendCash);					++citer;
			(*citer).Pop(kPrice.IsExtendDiscount);				++citer;
			(*citer).Pop(kPrice.iDiscountExtendCash);			++citer;
			(*citer).Pop(kPrice.IsOnlyMileage);					++citer;
			(*citer).Pop(kPrice.iPriceIdx);						++citer;

			CONT_DEF_CASH_SHOP_ARTICLE::iterator finditer = kContArticle.find(iItemIdx);
			if(finditer != kContArticle.end())
			{
				(*finditer).second.kContCashItemPrice.insert(std::make_pair(tagTBL_DEF_CASH_SHOP_ITEM_PRICE_KEY(kPrice.bTimeType,kPrice.iItemUseTime),kPrice));
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Not found article Idx:"<<iItemIdx);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}

		CONT_CASH_SHOP_ITEM::value_type kItem;

		iEntityCount = (*return_iter); ++return_iter;

		for(int i = 0;i < iEntityCount;i++)
		{
			(*citer).Pop(iItemIdx);					++citer;
			(*citer).Pop(kItem.iItemNo);			++citer;
			(*citer).Pop(kItem.iItemCount);			++citer;

			CONT_DEF_CASH_SHOP_ARTICLE::iterator finditer = kContArticle.find(iItemIdx);
			if(finditer != kContArticle.end())
			{
				(*finditer).second.kContCashItem.push_back(kItem);
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Not found article Idx:"<<iItemIdx);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}

		CONT_CASH_SHOP_ITEM_LIMITSELL kContItemLimitSell;
		CONT_CASH_SHOP_ITEM_LIMITSELL::mapped_type kItemLimitSell;

		iEntityCount = (*return_iter); ++return_iter;

		for(int i = 0;i < iEntityCount;i++)
		{
			(*citer).Pop(iItemIdx);									++citer;
			(*citer).Pop(kItemLimitSell.iItemTotalCount);			++citer;
			(*citer).Pop(kItemLimitSell.iItemLimitCount);			++citer;
			(*citer).Pop(kItemLimitSell.ItemBuyLimitCount);			++citer;			

			CONT_DEF_CASH_SHOP_ARTICLE::iterator finditer = kContArticle.find(iItemIdx);
			if(finditer != kContArticle.end())
			{
				auto kPair = kContItemLimitSell.insert(std::make_pair(iItemIdx, kItemLimitSell));
				if( !kPair.second )
				{
					VERIFY_INFO_LOG( false , BM::LOG_LV0, __FL__ << _T("Data Error!!!") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed CASH_SHOP_ITEM_LIMITSELL"));
				}
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Not found article Idx:"<<iItemIdx);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}

		/*
		�Ʒ��� ���̺� ���� üũ ó��
		*/

		CONT_DEF_CASH_SHOP kTempShop = kContShop;

		for(CONT_DEF_CASH_SHOP_ARTICLE::const_iterator aiter = kContArticle.begin();aiter != kContArticle.end();++aiter)
		{
			CONT_DEF_CASH_SHOP::iterator finditer = kTempShop.find((*aiter).second.iCategoryIdx);
			if(finditer == kTempShop.end())
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Not found category Idx:"<<(*aiter).second.iCategoryIdx);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}

		for(int i = CSCT_SMALL;i > CSCT_LARGE;i--)
		{
			for(CONT_DEF_CASH_SHOP::const_iterator siter = kContShop.begin();siter != kContShop.end();++siter)
			{
				if(i != (*siter).second.bStep)
				{
					continue;
				}

				switch((*siter).second.bStep)
				{
				case CSCT_LARGE:
					{
						continue;
					}break;
				case CSCT_MIDDLE:
					{
						CONT_DEF_CASH_SHOP::iterator finditem = kTempShop.find((*siter).first);
						if(finditem != kTempShop.end())
						{
							CONT_DEF_CASH_SHOP::iterator findcont = kTempShop.find((*siter).second.iRootIdx);
							if(findcont == kTempShop.end())
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Not found category Idx:"<<(*siter).second.iRootIdx);
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}
						}
						else
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}break;
				case CSCT_SMALL:
					{
						CONT_DEF_CASH_SHOP::iterator finditem = kTempShop.find((*siter).first);
						if(finditem != kTempShop.end())
						{
							CONT_DEF_CASH_SHOP::iterator findcont = kTempShop.find((*siter).second.iParentIdx);
							if(findcont == kTempShop.end())
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Not found category Idx:"<<(*siter).second.iParentIdx);
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}
						}
						else
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Not found category Idx:"<<(*siter).first);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}break;
				}
			}
		}

		if( kTempShop.empty() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		g_kTblDataMgr.SetContDef(kContShop);
		g_kTblDataMgr.SetContDef(kContArticle);
		g_kTblDataMgr.SetContDef(kContItemLimitSell);
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool Q_DQT_DEF_TACTICS_QUEST_PSEUDO(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Can't load TB_DefTacticsLevel table] Result: "<<rkResult.eRet);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_TACTICS_QUEST_PSEUDO kCont;
	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	size_t const iMaxColumnCount = 10;
	int iVal = 0;
	while( rkVec.end() != result_iter )
	{
		CONT_DEF_TACTICS_QUEST_PSEUDO::mapped_type kNewElement;
		result_iter->Pop( kNewElement.iTacticsLevel );		++result_iter;
		for( size_t iCur = 0; iMaxColumnCount > iCur; ++iCur )
		{
			result_iter->Pop( iVal );			++result_iter;
			if( iVal )
			{
				std::back_inserter(kNewElement.kPseudoVec) = iVal;
			}
		}

		auto kRet = kCont.insert( std::make_pair(kNewElement.iTacticsLevel, kNewElement) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't insert new element: "<<kNewElement.iTacticsLevel);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( kCont.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}

bool Q_DQT_DEF_MAP_STONE_CONTROL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<rkResult.eRet<<L"] DB Load");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_MAP_STONE_CONTROL kCont;
	const CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
	while(rkVec.end() != result_iter)
	{
		CONT_MAP_STONE_CONTROL::mapped_type kValue;

		result_iter->Pop(kValue.iMapNo);	++result_iter;
		result_iter->Pop(kValue.iPeriod);	++result_iter;

		for(int i=0; i<MAX_STONE_TICKET; i++)
		{
			result_iter->Pop(kValue.aStoneCtrl[i].iStoneCount);	++result_iter;
			result_iter->Pop(kValue.aStoneCtrl[i].iTicketCount);	++result_iter;
		}

		kCont.insert(std::make_pair(kValue.iMapNo, kValue));
	}

	if( kCont.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}


bool Q_DQT_DEF_MAPEFFECT(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<rkResult.eRet<<L"] DB Load");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	typedef std::map< TBL_PAIR_KEY_INT,TBL_DEF_MAP_EFFECT> CONT_DEF_MAP_EFFECT_MAP;
	CONT_DEF_MAP_EFFECT_MAP kCont;
	
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_MAP_EFFECT_MAP::key_type, CONT_DEF_MAP_EFFECT_MAP::mapped_type, CONT_DEF_MAP_EFFECT_MAP,BM::vstring, DefLoadErrorLog::AddError > NationCodeUtil( L"Duplicate MAPEFFECT[" DBCACHE_KEY_PRIFIX L"]" );
	const CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
	while(rkVec.end() != result_iter)
	{
		CONT_DEF_MAP_EFFECT_MAP::mapped_type kValue;
		std::wstring NationCodeStr;
		
		result_iter->Pop(NationCodeStr);	++result_iter;
		result_iter->Pop(kValue.iEffectID);	++result_iter;
		result_iter->Pop(kValue.iGroundNo);	++result_iter;
		for (int i=0; i<MAX_MAP_EFFECT_ABIL_ARRAY; i++)
		{
			result_iter->Pop(kValue.aiType[i]);++result_iter;
			result_iter->Pop(kValue.aiValue[i]);++result_iter;
		}
		for (int j=0; j<MAX_MAP_EFFECT_ABIL64_ARRAY; j++)
		{
			result_iter->Pop(kValue.aiType64[j]);++result_iter;
			result_iter->Pop(kValue.aiValue64[j]);++result_iter;
		}
		//kCont.insert( std::make_pair( iIdx, kValue ) );

		CONT_DEF_MAP_EFFECT_MAP::key_type const kKey(kValue.iEffectID, kValue.iGroundNo);
		NationCodeUtil.Add(NationCodeStr, kKey, kValue, __FUNCTIONW__, __LINE__);
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		CONT_MAP_EFFECT vec;
		CONT_DEF_MAP_EFFECT_MAP const& kContResult = NationCodeUtil.GetResult();
		CONT_DEF_MAP_EFFECT_MAP::const_iterator Itor =  kContResult.begin();
		CONT_DEF_MAP_EFFECT_MAP::const_iterator ItorEnd =  kContResult.end();	

		while(ItorEnd != Itor)
		{
			vec.push_back((*Itor).second);
			++Itor;
		}
		if(!vec.empty())
		{
			g_kCoreCenter.ClearQueryResult(rkResult);
			g_kTblDataMgr.SetContDef(vec);
			return true;
		}
	
		return true;
	}
	if( NationCodeUtil.IsEmpty() )
	{
		INFO_LOG(LOG_LV6, __FL__ << L"[TB_Def_MAPEFFECT] table is Empty");
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool Q_DQT_DEF_MAPENTITY(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<rkResult.eRet<<L"] DB Load");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	typedef std::map< TBL_KEY_INT,TBL_DEF_MAP_ENTITY> CONT_DEF_MAP_ENTITY_MAP;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_MAP_ENTITY_MAP::key_type, CONT_DEF_MAP_ENTITY_MAP::mapped_type, CONT_DEF_MAP_ENTITY_MAP,BM::vstring, DefLoadErrorLog::AddError > NationCodeUtil( L"Duplicate MAPENTITY[" DBCACHE_KEY_PRIFIX L"]" );

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_itr = rkVec.begin();
	while(rkVec.end() != result_itr)
	{
		CONT_DEF_MAP_ENTITY_MAP::mapped_type kValue;
		std::wstring NationCodeStr;
		int iIDX = 0;
		
		result_itr->Pop(iIDX);						++result_itr;
		result_itr->Pop(NationCodeStr);				++result_itr;
		result_itr->Pop(kValue.iGroundNo);			++result_itr;
		result_itr->Pop(kValue.iClass);				++result_itr;
		result_itr->Pop(kValue.nLevel);				++result_itr;
		result_itr->Pop(kValue.iEffect);			++result_itr;
		result_itr->Pop(kValue.pt3Min.x);			++result_itr;
		result_itr->Pop(kValue.pt3Min.y);			++result_itr;
		result_itr->Pop(kValue.pt3Min.z);			++result_itr;
		result_itr->Pop(kValue.pt3Max.x);			++result_itr;
		result_itr->Pop(kValue.pt3Max.y);			++result_itr;
		result_itr->Pop(kValue.pt3Max.z);			++result_itr;
		result_itr->Pop(kValue.kOwnerPointGuid);	++result_itr;

		if ( kValue.pt3Min == kValue.pt3Max )
		{
			kValue.pt3Max.x = kValue.pt3Max.y = kValue.pt3Max.z = 10000000.0f;
			kValue.pt3Min.x = kValue.pt3Min.y = kValue.pt3Min.z = -10000000.0f;
		}

		if ( (kValue.pt3Min.x >= kValue.pt3Max.x) || (kValue.pt3Min.y >= kValue.pt3Max.y) || (kValue.pt3Min.z >= kValue.pt3Max.z) )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"Error Min["<<kValue.pt3Min<<L"] Max["<<kValue.pt3Max<<L"]" );
		}
		else
		{
			kValue.pt3Pos = kValue.pt3Min;
			kValue.pt3Pos.x += ((kValue.pt3Max.x - kValue.pt3Min.x) / 2.0f);
			kValue.pt3Pos.y += ((kValue.pt3Max.y - kValue.pt3Min.y) / 2.0f);
			NationCodeUtil.Add(NationCodeStr, iIDX, kValue, __FUNCTIONW__, __LINE__);
		}
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		CONT_MAP_ENTITY vec;
		CONT_DEF_MAP_ENTITY_MAP const& kContResult = NationCodeUtil.GetResult();
		CONT_DEF_MAP_ENTITY_MAP::const_iterator Itor =  kContResult.begin();
		CONT_DEF_MAP_ENTITY_MAP::const_iterator ItorEnd =  kContResult.end();	

		while(ItorEnd != Itor)
		{
			vec.push_back((*Itor).second);

			++Itor;
		}
		if(!vec.empty())
		{
			std::sort(vec.begin(), vec.end());
			g_kCoreCenter.ClearQueryResult(rkResult);
			g_kTblDataMgr.SetContDef(vec);
			return true;
		}
	
		return true;
	}
	if( NationCodeUtil.IsEmpty() )
	{
		INFO_LOG(LOG_LV6, __FL__ << L"[TB_Def_MAPENTITY] table is Empty");
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEF_PVP_REWARD( CEL::DB_RESULT& rkResult )
{
	if ( 2 != rkResult.vecResultCount.size() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Error!! Table Count is " << rkResult.vecResultCount.size() );
		return  false;
	}

	CEL::DB_RESULT_COUNT::const_iterator return_itr = rkResult.vecResultCount.begin();
	int iCount = *return_itr;

	CONT_DEF_PVP_REWARD vec;
	CONT_DEF_PVP_REWARD::value_type kOldElement;

	CONT_DEF_PVP_REWARD_ITEM map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while( itr != rkResult.vecArray.end() )
	{
		if ( iCount )
		{
			CONT_DEF_PVP_REWARD::value_type kElement;
			itr->Pop(kElement.iPoint_Min);			++itr;
			itr->Pop(kElement.iPoint_Max);			++itr;
			itr->Pop(kElement.iCP);					++itr;

			if ( kElement.Check() )
			{
				if ( vec.empty() || kElement.Check(kOldElement) )
				{
					vec.push_back( kElement );
					kOldElement = kElement;
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"POINT Min["<<kElement.iPoint_Min<<L"]-Max["<<kElement.iPoint_Max<<L"] : Min["<<kOldElement.iPoint_Min<<L"]-Max["<<kOldElement.iPoint_Max<<L"] Discord Value!!!");
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"POINT Min["<<kElement.iPoint_Min<<L"] Max["<<kElement.iPoint_Max<<L"] Wrong Value!!!");
			}

			--iCount;
		}
		else
		{
			CONT_DEF_PVP_REWARD_ITEM::key_type kKey = 0;
			CONT_DEF_PVP_REWARD_ITEM::mapped_type kElement;
			itr->Pop( kKey );							++itr;
			itr->Pop(kElement.byWin_ItemCount);			++itr;
			itr->Pop(kElement.byDraw_ItemCount);		++itr;
			itr->Pop(kElement.byLose_ItemCount);		++itr;

			for ( int i = 0 ; i < 3; ++i )
			{
				itr->Pop(kElement.iItemBag[i]);			++itr;
			}
			
			map.insert( std::make_pair( kKey, kElement ) );
		}
	}

	if ( vec.empty() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [CONT_DEF_PVP_REWARD] is row count 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::sort( vec.begin(), vec.end() );
	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(vec);
	g_kTblDataMgr.SetContDef(map);
	return true;
}


bool Q_DQT_DEF_ITEM_DISASSEMBLE(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_ITEM_DISASSEMBLE map;

	int iIDX = 0;
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_ITEM_DISASSEMBLE::mapped_type element;

		(*itor).Pop( element.iItemGrade );	++itor;
		(*itor).Pop( element.iItemBagGrpNo );	++itor;

		map.insert( std::make_pair(element.iItemGrade, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [CONT_DEF_ITEM_DISASSEMBLE] is row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool Q_DQT_DEF_ITEM_PLUSUP_CONTROL(CEL::DB_RESULT& rkResult)
{
	CONT_DEF_ITEM_PLUSUP_CONTROL map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itr )
	{
		CONT_DEF_ITEM_PLUSUP_CONTROL::mapped_type element;

		itr->Pop( element.iTypeNo );					++itr;
		itr->Pop( element.iSuccessRateControlNo_Low );	++itr;
		itr->Pop( element.iSuccessRateControlNo_High );	++itr;

		map.insert( std::make_pair(element.iTypeNo, element) );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);

	if ( map.size() )
	{
		g_kTblDataMgr.SetContDef(map);
	}
	return true;
}


bool Q_DQT_DEF_ITEM_RARITY_CONTROL(CEL::DB_RESULT& rkResult)
{
	CONT_DEF_ITEM_RARITY_CONTROL map;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_ITEM_RARITY_CONTROL::mapped_type element;

		(*itor).Pop( element.iTypeNo );	++itor;
		(*itor).Pop( element.iSuccessRateControlNo );	++itor;

		map.insert( std::make_pair(element.iTypeNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEF_SHOP_IN_EMPORIA(CEL::DB_RESULT& rkResult)
{//select [IDX],[ShopGuid],[Category],[ItemNo],[Price],[CP],[Coin],[TimeType],[UseTime] from dbo.TB_DefShopInEmporia
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_TBL_SHOP_IN_EMPORIA map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_TBL_SHOP_IN_EMPORIA::mapped_type element;
        TBL_KEY_SHORT iIDX = 0;

		(*itor).Pop( iIDX );			    ++itor;
		(*itor).Pop( element.kShopGuid);	++itor;
		(*itor).Pop( element.iCategory );	++itor;
		(*itor).Pop( element.iItemNo );		++itor;
		(*itor).Pop( element.iPrice );		++itor;
		(*itor).Pop( element.iCP );			++itor;
		(*itor).Pop( element.iCoin );		++itor;
		(*itor).Pop( element.bTimeType);	++itor;
		(*itor).Pop( element.iUseTime );	++itor;

        element.iIDX = iIDX;
		map.insert(std::make_pair(element.iIDX, element));
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEF_SHOP_IN_STOCK(CEL::DB_RESULT& rkResult)
{//select [BagNo] from dbo.TB_DefItemBag
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_TBL_SHOP_IN_STOCK map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_TBL_SHOP_IN_STOCK::mapped_type element;

		(*itor).Pop( element.iIDX);			++itor;
		(*itor).Pop( element.kShopGuid);	++itor;
		(*itor).Pop( element.iCategory );	++itor;
		(*itor).Pop( element.iItemNo );		++itor;
		(*itor).Pop( element.iPrice );		++itor;
		(*itor).Pop( element.iSellPrice );	++itor;
		(*itor).Pop( element.iCP );			++itor;
		(*itor).Pop( element.iCoin );		++itor;
		(*itor).Pop( element.bTimeType);	++itor;
		(*itor).Pop( element.iUseTime );	++itor;
		(*itor).Pop( element.iState );	++itor;

		map.insert(std::make_pair(element.iIDX, element));
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEF_SHOP_IN_GAME(CEL::DB_RESULT& rkResult)
{//select [BagNo] from dbo.TB_DefItemBag
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();
	if(count_iter == rkResult.vecResultCount.end())
	{
		return false;
	}

	int const iArticleCount = (*count_iter); ++count_iter;

	DBCacheUtil::PgNationCodeHelper< CONT_TBL_SHOP_IN_GAME::key_type, CONT_TBL_SHOP_IN_GAME::mapped_type, CONT_TBL_SHOP_IN_GAME, BM::vstring, DefLoadErrorLog::AddError > kNationCodeUtil( L"duplicate Idx[" DBCACHE_KEY_PRIFIX L"], in [TB_DefShopInGame]" );
	for(int i = 0;i < iArticleCount;++i)
	{
		std::wstring kNationCodeStr;
		CONT_TBL_SHOP_IN_GAME::mapped_type element;

		(*itor).Pop( element.iIDX);			++itor;
		(*itor).Pop( kNationCodeStr);		++itor;
		(*itor).Pop( element.kShopGuid);	++itor;
		(*itor).Pop( element.iCategory );	++itor;
		(*itor).Pop( element.iItemNo );		++itor;
		(*itor).Pop( element.iPrice );		++itor;
		(*itor).Pop( element.iCP );			++itor;
		(*itor).Pop( element.iCoin );		++itor;
		(*itor).Pop( element.bTimeType);	++itor;
		(*itor).Pop( element.iUseTime );	++itor;

		kNationCodeUtil.Add(kNationCodeStr, element.iIDX, element, __FUNCTIONW__, __LINE__);
	}

	int const iNpcCount = (*count_iter); ++count_iter;

	CONT_SHOPNPC_GUID npcmap;

	for(int i = 0;i < iNpcCount;++i)
	{
		CONT_SHOPNPC_GUID::key_type kKey;
		CONT_SHOPNPC_GUID::mapped_type kValue;
		(*itor).Pop( kKey );		++itor;
		(*itor).Pop( kValue );		++itor;

		if(false == npcmap.insert(std::make_pair(kKey,kValue)).second)
		{
			DefLoadErrorLog::AddErrorMsg( BM::vstring()<<__FL__<<L"SHOPINGAME LOAD FAIL SHOPNO DUPLICATED SHOPNO[" << kKey << L"] NPCGUID [" << kValue << L"]" );
			return false;
		}
	}

	if( !kNationCodeUtil.IsEmpty()
	&&	!npcmap.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		g_kTblDataMgr.SetContDef(npcmap);
		return true;
	}

	if( kNationCodeUtil.IsEmpty() )
	{
		DefLoadErrorLog::AddErrorMsg( BM::vstring()<<__FL__<<L"[TB_DefShopInGame] table is Empty" );
	}
	if( npcmap.empty() )
	{
		DefLoadErrorLog::AddErrorMsg( BM::vstring()<<__FL__<<L"[TB_DefShopNpc] table is Empty" );
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEF_QUEST_RANDOM(CEL::DB_RESULT& rkResult)
{
	const CEL::DB_DATA_ARRAY& rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_QUEST_RANDOM kNewVec;

	while(rkVec.end() != result_iter)
	{
		CONT_DEF_QUEST_RANDOM::value_type kNew;
		(*result_iter).Pop(kNew.iType);				++result_iter;
		(*result_iter).Pop(kNew.iMinLevel);			++result_iter;
		(*result_iter).Pop(kNew.iMaxLevel);			++result_iter;
		(*result_iter).Pop(kNew.iGroupNo);			++result_iter;
		(*result_iter).Pop(kNew.iStartQuestID);		++result_iter;
		(*result_iter).Pop(kNew.iEndQuestID);		++result_iter;
		(*result_iter).Pop(kNew.iDisplayCount);		++result_iter;

		std::back_inserter(kNewVec) = kNew;
	}

	//if( g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA) )
	{
		if( kNewVec.size() )
		{
			g_kCoreCenter.ClearQueryResult(rkResult);
			g_kTblDataMgr.SetContDef(kNewVec);
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("TABLE [TB_DefQuest_Random] is row count 0"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	return true;
}

bool Q_DQT_DEFGMCMD(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFGMCMD map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFGMCMD::mapped_type element;

		(*itor).Pop( element.iCmdNo );	++itor;
		(*itor).Pop( element.byLevel );	++itor;

		map.insert( std::make_pair(element.iCmdNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEF_BASE_CHARACTER( CEL::DB_RESULT &rkResult )
{//select [AbilNo], [Name] from dbo.TB_DefAbilType
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_BASE_CHARACTER map;
	DBCacheUtil::PgNationCodeHelper<CONT_DEF_BASE_CHARACTER::key_type, CONT_DEF_BASE_CHARACTER::mapped_type, CONT_DEF_BASE_CHARACTER, BM::vstring, DefLoadErrorLog::AddError> NationCodeUtil(L"Duplicate f_Class[" DBCACHE_KEY_PRIFIX L"]");
	while( rkResult.vecArray.end() != itor )
	{
		std::wstring NationCodeStr;
		CONT_DEF_BASE_CHARACTER::mapped_type element;

		(*itor).Pop( NationCodeStr );				++itor; // nation code

		(*itor).Pop( element.f_Class);				++itor;
		(*itor).Pop( element.f_State);				++itor;
		(*itor).Pop( element.f_Race);				++itor;	
		(*itor).Pop( element.f_Lv);					++itor;
		(*itor).Pop( element.f_Exp);				++itor;//5	

		(*itor).Pop( element.f_Money);				++itor;
		(*itor).Pop( element.f_HP);					++itor;
		(*itor).Pop( element.f_MP);					++itor;
		(*itor).Pop( element.f_CP);					++itor;
		(*itor).Pop( element.f_ExtraSkillPoint);	++itor;//10

		(*itor).Pop( element.f_Skills);				++itor;
		(*itor).Pop( element.f_RecentMap);			++itor;	
		(*itor).Pop( element.f_RecentLocX);			++itor;	
		(*itor).Pop( element.f_RecentLocY);			++itor;
		(*itor).Pop( element.f_RecentLocZ);			++itor;//15

		(*itor).Pop( element.f_LastVillage);		++itor;
		(*itor).Pop( element.f_LastVillageLocX);	++itor;
		(*itor).Pop( element.f_LastVillageLocY);	++itor;
		(*itor).Pop( element.f_LastVillageLocZ);	++itor;
		(*itor).Pop( element.f_QuickSlot);			++itor;//20

		(*itor).Pop( element.f_IngQuest);			++itor;
		(*itor).Pop( element.f_EndQuest);			++itor;
		(*itor).Pop( element.f_EndQuest2);			++itor;
		(*itor).Pop( element.f_EndQuestExt);		++itor;
		(*itor).Pop( element.f_EndQuestExt2);		++itor;//25

		(*itor).Pop( element.f_InvExtend);			++itor;//26

		NationCodeUtil.Add(NationCodeStr, element.f_Class, element, __FUNCTIONW__, __LINE__);
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(NationCodeUtil.GetResult());
		return true;
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEFDYNAMICABILRATE( CEL::DB_RESULT &rkResult )
{//select [AbilRateNo], [Type01], [Value01], [Type02], [Value02], [Type03], [Value03], [Type04], [Value04], [Type05], [Value05], [Type06], [Value06], [Type07], [Value07], [Type08], [Value08], [Type09], [Value09], [Type10], [Value10] from dbo.TB_DefDynamicAbilRate
	CONT_DEFDYNAMICABILRATE map;
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFDYNAMICABILRATE::mapped_type element;
		itor->Pop( element.AbilRateNo );	++itor;
		for(int i=0; i!=MAX_DYNAMICABILRATE_ARRAY ; ++i)
		{
			itor->Pop( element.aType[i] );		++itor;
			itor->Pop( element.aValue[i] );		++itor;
		}
		map.insert( std::make_pair(element.AbilRateNo, element) );
	}

	if( !map.empty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_DEFDYNAMICABILRATE_BAG( CEL::DB_RESULT &rkResult )
{//select [ID], [Abil01], [Abil02], [Value03], [Abil04], [Abil05], [Abil06], [Abil07], [Abil08], [Abil09], [Abil10] from dbo.TB_DefDynamicAbilRate
	CONT_DEFDYNAMICABILRATE_BAG map;
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFDYNAMICABILRATE_BAG::mapped_type element;
		itor->Pop( element.BagID );	++itor;
		for(int i=0; i!=MAX_DYNAMICABILRATE_LIST ; ++i)
		{
			itor->Pop( element.aAbil[i] );		++itor;
		}
		map.insert( std::make_pair(element.BagID, element) );
	}

	if( !map.empty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_PCROOM(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_PCROOMIP kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

	while(kIter != rkResult.vecArray.end())
	{
		CONT_PCROOMIP::mapped_type	kValue;
		std::wstring kIp;

		kIter->Pop(kIp);			++kIter;
		kIter->Pop(kValue);			++kIter;

		kCont.insert(std::make_pair(CEL::ADDR_INFO(kIp,0).ip.S_un.S_addr,kValue));
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool Q_DQT_PCCAFE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_PCCAFE kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

	while(kIter != rkResult.vecArray.end())
	{	
		std::wstring kIp;
		BYTE byGrade = 0;

		kIter->Pop(kIp);			++kIter;
		kIter->Pop(byGrade);		++kIter;

		kCont.insert(std::make_pair( kIp, byGrade ) );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool Q_DQT_PCCAFE_ABIL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_PCCAFE_ABIL kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

	while(kIter != rkResult.vecArray.end())
	{
		SPCCafeAbil kPCCafeAbil;

		kIter->Pop(kPCCafeAbil.byGrade);	++kIter;

		for( int i = 0; i < MAX_PCCAFEABIL_COUNT; ++i )
		{
			kIter->Pop(kPCCafeAbil.arAbilNo[i]);	++kIter;
		}

		kCont.insert(std::make_pair( kPCCafeAbil.byGrade, kPCCafeAbil ) );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool Q_DQT_DEF_PET_BONUSSTATUS(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_DEF_PET_BONUSSTATUS kCont;

	int i = 0;
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(itr != rkResult.vecArray.end())
	{
		CONT_DEF_PET_BONUSSTATUS::mapped_type	kElement;
		itr->Pop( kElement.iID );							++itr;
		itr->Pop( kElement.bySelectRate );					++itr;

		for ( i = 0 ; i < MAX_PET_BONUSSTATUS_ITEMOPT2_NUM ; ++i )
		{
			itr->Pop( kElement.iItemOpt2_OptionType[i] );	++itr;
			itr->Pop( kElement.iItemOpt2_Rate[i] );			++itr;
		}

		for ( i = 0 ; i < MAX_ITEM_OPTION_ABIL_NUM ; ++i )
		{
			itr->Pop( kElement.iItemOpt2_ValueLvRate[i] );	++itr;
		}

		kCont.insert( std::make_pair( kElement.iID, kElement ) );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool Q_DQT_UPDATE_MEMBER_PW(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet )
	{
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("PW Update Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_FRIENDLIST_ADD_BYGUID(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)
	{
		//INFO_LOG(LOG_LV6, _T("[%s]-[%d] General Log: Add friend by guid success = %d"), __FUNCTIONW__, __LINE__, rkResult.eRet);
		return true;
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Critical Error: Add friend by guid failed = ") << rkResult.eRet);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_FRIENDLIST_SELECT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		typedef std::vector< SDBFriendItem > FRIEND_ITEM_ARRAY;
		FRIEND_ITEM_ARRAY kFriendArray;//ģ���� 20�����? 
		while(rkResult.vecArray.end() != kIter)
		{
			SDBFriendItem kItem;

			BM::GUID kFriendCharGuid;

			std::wstring kFriendExtendName;
			std::wstring kFriendGroupName;
			int iBlockStatus = 0;
			SHOMEADDR kHomeAddr;

			//kIter->Pop(kOwnerMemberGuid); ++kIter;
			kIter->Pop(kFriendGroupName); ++kIter;
			kIter->Pop(kFriendCharGuid); ++kIter;
			kIter->Pop(kFriendExtendName); ++kIter;
			kIter->Pop(iBlockStatus); ++kIter;

			short sStreetNo;
			int iHouseNo;

			kIter->Pop(sStreetNo); ++kIter;
			kIter->Pop(iHouseNo); ++kIter;

			kItem.CharGuid(kFriendCharGuid);
			kItem.GroupName(kFriendGroupName);
			kItem.ExtendName(kFriendExtendName);
			kItem.ChatStatus((BYTE)(iBlockStatus&0x000000FF));
			kItem.HomeAddr(SHOMEADDR(sStreetNo, iHouseNo));

			if (FM_MAXFRIEND >= kFriendArray.size())
			{
				kFriendArray.push_back(kItem);
			}
		}

		BM::Stream kPacket(PT_T_N_NFY_MSN_FULL_DATA);
		kPacket.Push(rkResult.QueryOwner());

		kPacket.Push(kFriendArray.size());
		FRIEND_ITEM_ARRAY::const_iterator fitem_itor = kFriendArray.begin();
		while(fitem_itor != kFriendArray.end())
		{
			(*fitem_itor).WriteToPacket(kPacket);
			++fitem_itor;
		}

		SendToFriendMgr(kPacket);

		//INFO_LOG(LOG_LV6, _T("[%s]-[%d] General Log: Select Friend list success row[%d]"), __FUNCTIONW__, __LINE__, (int)rkResult.row_count);
		return true;
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Critical Error: Select friend list Failed = ") << rkResult.eRet);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_FRIENDLIST_UPDATE_CHATSTATUS(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)
	{
		//INFO_LOG(LOG_LV6, _T("[%s]-[%d] General Log: Update Chat Status Ret[%d]"), __FUNCTIONW__, __LINE__, rkResult.eRet);
		return true;
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Critical Error: failed Update Chat status = ") << rkResult.eRet);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
bool Q_DQT_FRIENDLIST_DEL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)
	{
		//INFO_LOG(LOG_LV6, _T("[%s]-[%d] General Log: Del friend by guid success = %d"), __FUNCTIONW__, __LINE__, rkResult.eRet);
		return true;
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Critical Error: Del friend by guid failed = ") << rkResult.eRet);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_FRIENDLIST_UPDATE_GROUP(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)
	{
		//INFO_LOG(LOG_LV6, _T("[%s]-[%d] General Log: update friend group success = %d"), __FUNCTIONW__, __LINE__, rkResult.eRet);
		return true;
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("Critical Error: update friend group failed = ") << rkResult.eRet);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
/*
enum E_COUPON_RESULT_TYPE
{
CRT_SUCCESS,
CRT_INVALID_COUPON,	// �߸��� �����̴�.
CRT_INVALID_EVENT,	// �߸��� �̺�Ʈ Ÿ���̴�.
CRT_EXPIRE,			// ��� �Ⱓ�� ����Ǿ���.
CRT_DB_ERROR,		// ��� ���� �Ѥ�;;
};
*/

int const MAX_COUPON_REWARD_NUM = 8;

bool Q_DQT_TAKE_COUPON_REWARD(CEL::DB_RESULT &rkResult)
{
	BM::Stream kPacket(PT_N_M_ANS_TAKE_COUPON, rkResult.QueryOwner());

	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		int iError = 0;
		int f_Money = 0;
		int f_Cash = 0;
		CONT_COUPON_REWARD kCont;
		CONT_COUPON_REWARD::value_type kReward;

		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iError);						++kIter;
			kPacket.Push(iError);

			if(iError)
			{
				BM::Stream kCouponPacket;
				kCouponPacket.Push(E_CouponEvent_UpdateFailed);
				kCouponPacket.Push(rkResult.QueryOwner());
				SendToCouponEventDoc(kCouponPacket);

				g_kRealmUserMgr.Locked_SendToUserGround( rkResult.QueryOwner(), kPacket, false, true);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			for(int i = 0;i < MAX_COUPON_REWARD_NUM;i++)
			{
				kIter->Pop(kReward.iItemNo);		++kIter;
				kIter->Pop(kReward.siItemCount);	++kIter;
				kIter->Pop(kReward.bTimeType);		++kIter;
				kIter->Pop(kReward.siUseTime);		++kIter;
				kIter->Pop(kReward.kItemGuid);		++kIter;

				if(kReward.iItemNo)
				{
					kCont.push_back(kReward);
				}
			}

			kIter->Pop(f_Money);					++kIter;
			kIter->Pop(f_Cash);						++kIter;

			kPacket.Push(f_Money);
			kPacket.Push(f_Cash);
			kPacket.Push(kCont);
			g_kRealmUserMgr.Locked_SendToUserGround( rkResult.QueryOwner(), kPacket, false, true);
			return true;
		}
	}

	kPacket.Push(CRT_DB_ERROR);
	g_kRealmUserMgr.Locked_SendToUserGround( rkResult.QueryOwner(), kPacket, false, true);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_FRIENDLIST_MODIFY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet)
	{
		//INFO_LOG(LOG_LV6, _T("[%s]-[%d] General Log: Modify friend success = %d"), __FUNCTIONW__, __LINE__ , rkResult.eRet);
		return true;
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("Critical Error: Modify friend failed = ") << rkResult.eRet);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT Q_DQT_USER_EVENT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet
		|| CEL::DR_NO_RESULT == rkResult.eRet)//�̰� ��� ������ �ȵ�
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		CONT_TBL_EVENT kMap;

		while(rkResult.vecArray.end() != itor)
		{
			TBL_EVENT element;

			(*itor).Pop( element.iEventNo );++itor;
			(*itor).Pop( element.iEventType );++itor;

			(*itor).Pop( element.kTitle );++itor;
			(*itor).Pop( element.kDescription );++itor;

			(*itor).Pop( element.timeStartDate );++itor;
			(*itor).Pop( element.timeEndDate );++itor;
			(*itor).Pop( element.iPeriod );++itor;

			for (int i=0; i<MAX_EVENT_ARG; i++)
			{
				(*itor).Pop( element.aIntData[i] );++itor;
			}

			for (int i=0; i<MAX_EVENT_ARG; i++)
			{
				(*itor).Pop( element.aBigintData[i] );++itor;
			}

			for (int i=0; i<MAX_EVENT_ARG; i++)
			{
				(*itor).Pop( element.aGuidData[i] );++itor;
			}

			for (int i=0; i<MAX_EVENT_ARG; i++)
			{
				(*itor).Pop( element.aStringData[i] );++itor;
			}

			kMap.insert(std::make_pair(element.iEventNo ,element));
		}

		g_kEventDoc.RefreshTbl(kMap);

		BM::Stream kPacket;
		g_kEventDoc.WriteToPacket(kPacket);
		g_kEventView.RecvSync(kPacket);//������ view ����
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("No FIlter Unicode Range") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
	}

	return S_OK;
}

bool Q_DQT_SAVE_CLIENTOPTION(CEL::DB_RESULT &rkResult)
{
	return true;
}

bool Q_DQT_DEL_SKILLSET(CEL::DB_RESULT &rkResult)
{
	return true;
}

bool Q_DQT_SAVE_SKILLSET(CEL::DB_RESULT &rkResult)
{
	return true;
}

bool Q_DQT_SAVECHARACTER_MAPINFO(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("MapInfo Save failed ErrorCode<") << rkResult.eRet << _T("> CharcterGUID<") << rkResult.QueryOwner() << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool Q_DQT_SAVECHARACTER_EXTERN(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("MissionData Save failed ErrorCode<") << rkResult.eRet << _T("> CharcterGUID<") << rkResult.QueryOwner() << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool Q_DQT_LOAD_MISSION_REPORT(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_MISSION_REPORT map;
	CONT_MISSION_REPORT::key_type kKey;
	CONT_MISSION_REPORT::mapped_type kElement;
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itr)
	{
		itr->Pop(kKey.iKey);				++itr;
		BYTE kLevel;
		itr->Pop(kLevel);						++itr;
		kKey.iLevel = (int)kLevel;
		itr->Pop(kElement.iClearTime_Top);		++itr;
		itr->Pop(kElement.iClearTime_Avg);		++itr;
		itr->Pop(kElement.i64ClearTime_Total);	++itr;
		itr->Pop(kElement.i64ClearCount);		++itr;
		auto kPair = map.insert(std::make_pair(kKey,kElement));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG( false , BM::LOG_LV0, __FL__ << _T("Data Error!!!") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed MissionReport"));
		}
	}

	BM::Stream kNPacket(PT_T_N_NFY_MISSION_REPORT_DATA);
	PU::TWriteTable_MM(kNPacket,map);
	SendToRankMgr(kNPacket);
	//INFO_LOG(BM::LOG_LV6,_T("[Send Mission REPORT] Count : %u"), map.size() );
	return true;
}

bool Q_DQT_LOAD_MISSION_RANK(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SMissionKey kMissionKey;
	CONT_MISSION_RANK vector;
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while( itr!=rkResult.vecArray.end() )
	{
		itr->Pop(kMissionKey.iKey);			++itr;
		BYTE kLevel;
		itr->Pop(kLevel);					++itr;
		kMissionKey.iLevel = (int)kLevel;

		CONT_MISSION_RANK::value_type kElement;
		itr->Pop(kElement.iPoint);			++itr;
		itr->Pop(kElement.iPlayTime);		++itr;
		itr->Pop(kElement.kCharGuid);		++itr;
		itr->Pop(kElement.wstrName);		++itr;
		itr->Pop(kElement.wstrMemo);		++itr;
		itr->Pop(kElement.iUserLevel);		++itr;
		itr->Pop(kElement.kClass);			++itr;
		itr->Pop(kElement.dtRegistTime);	++itr;

		vector.push_back(kElement);
	}

	if ( !vector.empty() )
	{
		BM::Stream kNPacket(PT_T_N_NFY_MISSION_RANK_DATA, kMissionKey);
		PU::TWriteArray_M(kNPacket,vector);
		SendToRankMgr(kNPacket);
	}
	return true;
}

bool Q_DQT_SAVE_MISSION_REPORT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("MissionReport Save failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool Q_DQT_SAVE_MISSION_RANK(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("MissionRank Save failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool Q_DQT_SAVECHARACTER(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Character Save failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool Q_DQT_RECORD_LEVELUP(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Character[") << rkResult.QueryOwner() << _T("] Save failed ErrorCode[") << rkResult.eRet << _T("], Query=")
			<< rkResult.Command());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	if ( true == IsUseLevelRank() )
	{
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		if( itr != rkResult.vecArray.end() )
		{
			int iNewEffectLv = 0;
			itr->Pop( iNewEffectLv );

			int iOldEffectLv = 0;
			rkResult.contUserData.Pop( iOldEffectLv );

			if ( iNewEffectLv != iOldEffectLv )
			{
				SPMOD_AddAbil kAddAbilData( AT_LEVEL_RANK, iNewEffectLv );

				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->InsertTarget(rkResult.QueryOwner());
				pkActionOrder->kCause = CPE_Event;
				pkActionOrder->kContOrder.push_back( SPMO(IMET_SET_ABIL, rkResult.QueryOwner(), kAddAbilData) );
				g_kJobDispatcher.VPush(pkActionOrder);
			}
		}
		else
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << L"Character[" << rkResult.QueryOwner() << L"] Select Level Rank Error!!!");
			return false;
		}
	}

	return true;
}

void LoadMissionRank(CONT_DEF_MISSION_ROOT const *pkMissionRoot)
{
	if ( !pkMissionRoot )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! pkMissionRoot is NULL"));
		return;
	}

	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_LOAD_MISSION_REPORT, L"EXEC [dbo].[UP_LoadMissionReport]");
	g_kCoreCenter.PushQuery(kQuery);

	SMissionKey kKey;
	CONT_DEF_MISSION_ROOT::const_iterator itr;
	CONT_DEF_MISSION_ROOT::mapped_type kElement;
	for(itr=pkMissionRoot->begin(); itr!=pkMissionRoot->end(); ++itr)
	{
		kElement = itr->second;
		kKey.iKey = kElement.iKey;
		for( int i=0; i!=MAX_MISSION_LEVEL; ++i )
		{
			if ( kElement.aiLevel[i] )
			{
				kKey.iLevel = i;
				LoadMissionRank(kKey);
			}
		}
	}
}

void LoadMissionRank(SMissionKey const &rkMissionKey)
{
	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_LOAD_MISSION_RANK, L"EXEC [dbo].[UP_LoadMissionRank]" );
	kQuery.PushStrParam(rkMissionKey.iKey);
	kQuery.PushStrParam((BYTE)rkMissionKey.iLevel);
	g_kCoreCenter.PushQuery(kQuery);
}

void SaveMissionRank(SMissionKey const &rkKey, TBL_MISSION_RANK const &rkRankData)
{
	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SAVE_MISSION_RANK, L"EXEC [dbo].[UP_SaveMissionRank]");
	kQuery.PushStrParam(rkKey.iKey);
	BYTE kLevel = (BYTE)rkKey.iLevel;
	kQuery.PushStrParam(kLevel);
	kQuery.PushStrParam(rkRankData.iPoint);
	kQuery.PushStrParam(rkRankData.iPlayTime);
	kQuery.PushStrParam(rkRankData.kCharGuid);

	kQuery.PushStrParam(rkRankData.wstrMemo);
	kQuery.PushStrParam(rkRankData.iUserLevel);
	kQuery.PushStrParam(rkRankData.kClass);
	kQuery.PushStrParam(rkRankData.dtRegistTime);
	g_kCoreCenter.PushQuery(kQuery);
}


bool Q_DQT_DEF_LOAD_COUPONEVENT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Load Coupon Event failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_COUPON_EVENT_INFO::value_type kElement;
	CONT_COUPON_EVENT_INFO kCont;

	CONT_COUPON_EVENT_REWARD::value_type kReward;
	CONT_COUPON_EVENT_REWARD kContReward;

	BM::DBTIMESTAMP_EX kDBTime;

	CEL::DB_RESULT_COUNT::const_iterator return_iter = rkResult.vecResultCount.begin();
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	if((itr!=rkResult.vecArray.end()) && (return_iter != rkResult.vecResultCount.end()))
	{
		int const iCount = (*return_iter);		++return_iter;

		for(int i = 0;i < iCount;i++)
		{
			(*itr).Pop(kElement.iEventKey);		++itr;
			(*itr).Pop(kDBTime);				++itr;
			kElement.kStartTime = kDBTime;
			(*itr).Pop(kDBTime);				++itr;
			kElement.kEndTime = kDBTime;
			(*itr).Pop(kElement.kTitle);		++itr;
			(*itr).Pop(kElement.kDiscription);	++itr;
			kCont.push_back(kElement);
		}

		while( itr!=rkResult.vecArray.end() )
		{
			(*itr).Pop(kReward.iIDX);				++itr;
			(*itr).Pop(kReward.iEventKey);			++itr;
			(*itr).Pop(kReward.kRewardTitle);		++itr;

			for(int i = 0;i < MAX_COUPON_EVENT_REWARD;i++)
			{
				(*itr).Pop(kReward.iItemNo[i]);		++itr;
			}

			kContReward.push_back(kReward);
		}
	}

	if(!kCont.size())
	{
		return true;
	}

	g_kContentsTask.Build(kCont,kContReward);

	return true;
}



bool Q_DQT_DEF_PVP_TIME( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring NationCodeStr;
	CONT_DEF_PLAYLIMIT_INFO DefaultMap;
	CONT_DEF_PLAYLIMIT_INFO kContPvPPlayTimeInfo;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(itr != rkResult.vecArray.end())
	{
		itr->Pop(NationCodeStr);				++itr;

		int iID = 0;
		TBL_DEF_PLAYLIMIT_INFO kElement;

		itr->Pop( iID );						++itr;

		char cHour = 0;
		char cMin = 0;

		itr->Pop( cHour );						++itr;
		itr->Pop( cMin );						++itr;
		kElement.i64BeginTime = (static_cast<__int64>(cHour) * CGameTime::HOUR) + (static_cast<__int64>(cMin) * CGameTime::MINUTE);

		itr->Pop( cHour );						++itr;
		itr->Pop( cMin );						++itr;
		kElement.i64EndTime = (static_cast<__int64>(cHour) * CGameTime::HOUR) + (static_cast<__int64>(cMin) * CGameTime::MINUTE);

		itr->Pop( kElement.nLevelLimit );		++itr;
		itr->Pop( kElement.iValue01 );			++itr;
		itr->Pop( kElement.iValue02 );			++itr;

		itr->Pop( kElement.iBalanceLevelMin );			++itr;
		itr->Pop( kElement.iBalanceLevelMax );			++itr;

		if( DBCacheUtil::IsDefaultNation(NationCodeStr) )
		{
			CONT_DEF_PLAYLIMIT_INFO::iterator info_itr = DefaultMap.find( iID );
			if ( info_itr != DefaultMap.end() )
			{
				info_itr->second.push_back( kElement );
			}
			else
			{
				CONT_DEF_PLAYLIMIT_INFO_LIST kList;
				kList.push_back( kElement );
				DefaultMap.insert( std::make_pair( iID, kList ) );
			}
		}
		else
		{
			if( DBCacheUtil::IsCanNation< CONT_DEF_PLAYLIMIT_INFO::key_type, BM::vstring, DefLoadErrorLog::AddError >(NationCodeStr, iID, __FUNCTIONW__, __LINE__) )
			{
				CONT_DEF_PLAYLIMIT_INFO::iterator info_itr = kContPvPPlayTimeInfo.find( iID );
				if ( info_itr != kContPvPPlayTimeInfo.end() )
				{
					info_itr->second.push_back( kElement );
				}
				else
				{
					CONT_DEF_PLAYLIMIT_INFO_LIST kList;
					kList.push_back( kElement );
					kContPvPPlayTimeInfo.insert( std::make_pair( iID, kList ) );
				}
			}
		}
	}

	if( kContPvPPlayTimeInfo.size() || DefaultMap.size() )
	{
		CONT_DEF_PLAYLIMIT_INFO::iterator default_iter = DefaultMap.begin();
		for( ; default_iter != DefaultMap.end() ; ++default_iter )
		{
			CONT_DEF_PLAYLIMIT_INFO::iterator find_iter = kContPvPPlayTimeInfo.find( default_iter->first );
			if( find_iter == kContPvPPlayTimeInfo.end() )
			{
				kContPvPPlayTimeInfo.insert( *default_iter );
			}
		}
	}

	CONT_DEF_PLAYLIMIT_INFO::iterator info_itr = kContPvPPlayTimeInfo.begin();
	for ( ; info_itr != kContPvPPlayTimeInfo.end() ; ++info_itr )
	{
		std::sort( info_itr->second.begin(), info_itr->second.end(), PgPlayLimit_Sorter() );
	}

	g_kTblDataMgr.SetContDef(kContPvPPlayTimeInfo);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool Q_DQT_DEF_PVP_RANK( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	if( itr != rkResult.vecArray.end() )
	{
		SPvPRankingSeasonScedule kScedule;
		itr->Pop( kScedule.kBeginDate );				++itr;
		itr->Pop( kScedule.iSeason_InitPoint );			++itr;
		itr->Pop( kScedule.kCycle_Pass );				++itr;
		itr->Pop( kScedule.iCycleDay );					++itr;
		itr->Pop( kScedule.fCycle_PointRate );			++itr;
		itr->Pop( kScedule.iCycle_GivePoint );			++itr;
		
		g_kPvPRankingSceduler.SetScedule( kScedule );
	}
	return true;
}

bool Q_DQT_PVP_RANK_UPDATE( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iResult = PgPvPRankingSceduler::E_UPDATE_ERROR;
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	if( itr != rkResult.vecArray.end() )
	{
		itr->Pop( iResult );
	}

	g_kPvPRankingSceduler.SetResult( iResult );
	return true;
}

bool Q_DQT_PVP_RANK( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_PVP_RANKING kVecPvPRanking;
	kVecPvPRanking.reserve( 1000 );

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(itr != rkResult.vecArray.end())
	{
		VEC_PVP_RANKING::value_type kElement;

		itr->Pop( kElement.kCharacterGuid );		++itr;
		itr->Pop( kElement.wstrName );				++itr;
		itr->PopMemory( &kElement.kClassKey.iClass, sizeof(char) );		++itr;// int ����, DB���� tinyint�ϱ� �̷��� ������ �ȴ�.
		itr->Pop( kElement.kClassKey.nLv );			++itr;
		itr->Pop( kElement.iPoint );				++itr;
		itr->Pop( kElement.iWin );					++itr;
		itr->Pop( kElement.iDraw );					++itr;
		itr->Pop( kElement.iLose );					++itr;
		itr->Pop( kElement.iKill );					++itr;
		itr->Pop( kElement.iDeath );				++itr;

		kVecPvPRanking.push_back( kElement );
	}

	CONT_DEF_PLAYLIMIT_INFO const *pkContPvPPlayInfo = NULL;
	g_kTblDataMgr.GetContDef(pkContPvPPlayInfo);

	BM::Stream kTPacket( PT_N_T_ANS_GET_PVPLOBBY_INFO );
	PU::TWriteTable_AA(kTPacket, *pkContPvPPlayInfo );
	PU::TWriteArray_M( kTPacket, kVecPvPRanking );
	::SendToCenter( CProcessConfig::GetPublicChannel(), kTPacket );

	return true;
}

void CleanDeleteCharacter()
{
	int iDelayDay = 0;
	if (S_OK != g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_CleanDeleteCharacter_DelayDay, iDelayDay))
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot read variable EVar_DB_CleanDeleteCharacter_DelayDay"));
		return;
	}
	if (iDelayDay > 0)
	{
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_CLEAN_DELETE_CHARACTER, _T("EXEC [dbo].[up_CleanDeleteCharacter]") );
		kQuery.PushStrParam( iDelayDay );
		g_kCoreCenter.PushQuery(kQuery);
	}

	int iChangeDeletedCharacterName = 0;
	if (S_OK != g_kVariableContainer.Get(EVar_Kind_DBManager, EVar_DB_ChangeDeletedCharacterName_DelayHour, iChangeDeletedCharacterName))
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot read variable EVar_DB_ChangeDeletedCharacterName_DelayHour"));
		return;
	}
	if (iChangeDeletedCharacterName > 0)
	{
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_CHANGE_DELETED_CHARACTER_NAME, _T("EXEC [dbo].[up_UpdateDeletedCharacterName]") );
		kQuery.PushStrParam( iChangeDeletedCharacterName );
		g_kCoreCenter.PushQuery(kQuery);
	}
}

bool Q_DQT_CLEAN_DELETE_CHARACTER(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	return true;
}

bool Q_DQT_CHANGE_DELETED_CHARACTER_NAME(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	return true;
}

bool Q_DQT_OXQUIZ_UPDATE_STATE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	return true;
}

bool Q_DQT_OXQUIZ_EVENT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	if(rkResult.vecResultCount.empty())
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" Empty ResultCount") << _T(", Query=") << rkResult.Command());
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	//typedef std::map< TBL_KEY_INT, SOXQUIZINFO>					CONT_OXQUIZINFO;
	//typedef std::map< SOXQUIZREWARDKEY, SOXQUIZREWARD>			CONT_OXQUIZREWARD;
	//typedef std::map< TBL_KEY_INT, SOXQUIZEVENTINFO>			CONT_OXQUIZEVENTINFO;

	int const iEventCount = (*count_iter); ++count_iter;

	CONT_OXQUIZEVENTINFO kContEvent;

	for(int i = 0;i < iEventCount;++i)
	{
		CONT_OXQUIZEVENTINFO::mapped_type kEvent;

		(*itr).Pop(kEvent.iEventID);		++itr;
		(*itr).Pop(kEvent.iQuizNoMin);		++itr;
		(*itr).Pop(kEvent.iQuizNoMax);		++itr;
		(*itr).Pop(kEvent.iUserMin);		++itr;
		BM::DBTIMESTAMP_EX kDate;
		(*itr).Pop(kDate);					++itr;
		kEvent.kStartDate = kDate;
		(*itr).Pop(kDate);					++itr;
		kEvent.kEndDate = kDate;
		(*itr).Pop(kEvent.iMaxStep);		++itr;
		(*itr).Pop(kEvent.kEventNotiClose);	++itr;
		(*itr).Pop(kEvent.iAnsTime);		++itr;
		(*itr).Pop(kEvent.iSaveMin);		++itr;
		(*itr).Pop(kEvent.bState);			++itr;
		(*itr).Pop(kEvent.bRepeat);			++itr;
		(*itr).Pop(kEvent.kMailTitle);		++itr;
		(*itr).Pop(kEvent.kMailText);		++itr;

		if(0 < kEvent.bRepeat)
		{
			kEvent.bState = OXES_NONE;
		}

		for(int i = 0;i < MAX_OXEVENTNOTI_NUM;++i)
		{
			CONT_OXEVENTNOTI::value_type kNoti;
			(*itr).Pop(kNoti.kMsg);			++itr;
			(*itr).Pop(kNoti.iDelayTime);	++itr;
			kEvent.kContEventNoti.push_back(kNoti);
		}

		auto kPair = kContEvent.insert(std::make_pair(kEvent.iEventID,kEvent));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Ox Quiz Event Error !! EventNo [") << kEvent.iEventID << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed Data"));
			return false;
		}
	}

	int const iRewardCount = (*count_iter); ++count_iter;

	CONT_OXQUIZREWARD::key_type		kRewardKey;
	CONT_OXQUIZREWARD::mapped_type	kReward;

	CONT_OXQUIZREWARD kContReward;

	for(int i = 0;i < iRewardCount;++i)
	{
		(*itr).Pop(kRewardKey.iEventNo);	++itr;
		(*itr).Pop(kRewardKey.iStep);		++itr;
		(*itr).Pop(kReward.iMoney);			++itr;

		for(int k = 0;k < CONT_OXQUIZREWARD::mapped_type::MAX_OXQUIZREWARDITEM_NUM;++k)
		{
			(*itr).Pop(kReward.kItems[k].iItemNo);			++itr;
			(*itr).Pop(kReward.kItems[k].iItemCount);		++itr;
		}

		auto kPair = kContReward.insert(std::make_pair(kRewardKey,kReward));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("OX Quiz Reward Error !! EventNo [") << kRewardKey.iEventNo << _T("]") << _T("Step [") << kRewardKey.iStep << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed Data"));
			return false;
		}
	}

	int const iQuizCount = (*count_iter);	++count_iter;

	CONT_OXQUIZINFO::mapped_type kQuiz;
	CONT_OXQUIZINFO kContQuiz;

	for(int i = 0;i < iQuizCount;++i)
	{
		(*itr).Pop(kQuiz.iQuizNo);				++itr;
		(*itr).Pop(kQuiz.kQuizText);				++itr;
		(*itr).Pop(kQuiz.bResult);				++itr;
		(*itr).Pop(kQuiz.kQuizDiscription);		++itr;

		auto kPair = kContQuiz.insert(std::make_pair(kQuiz.iQuizNo,kQuiz));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("OX Quiz Table Error !! QuizNo [") << kQuiz.iQuizNo << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed Data"));
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kContEvent);
	g_kTblDataMgr.SetContDef(kContReward);
	g_kTblDataMgr.SetContDef(kContQuiz);

	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_OXQUIZ_EVENT_STATE, _T("EXEC [dbo].[up_LoadOXQuizEventState]"));
	kQuery.contUserData.Push(rkResult.contUserData);

	g_kCoreCenter.PushQuery(kQuery);

	return true;
}

bool Q_DQT_OXQUIZ_EVENT_STATE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	if(rkResult.vecResultCount.empty())
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" Empty ResultCount") << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_OXQUIZEVENTINFO kCopyCont;

	{
		TABLE_LOCK(CONT_OXQUIZEVENTINFO) kObjLock;
		g_kTblDataMgr.GetContDef(kObjLock);
		CONT_OXQUIZEVENTINFO const * pkCont = kObjLock.Get();
		if(!pkCont)
		{
			return false;
		}

		kCopyCont = (*pkCont);
	}

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	int const iEventCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iEventCount;++i)
	{
		int iEventID = 0;
		BYTE bState = 0;

		(*itr).Pop(iEventID);		++itr;
		(*itr).Pop(bState);			++itr;

		CONT_OXQUIZEVENTINFO::iterator iter = kCopyCont.find(iEventID);
		if(iter == kCopyCont.end())
		{
			CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("WARNING !! NOT FOUND EVENT EVENTNO [") << iEventID << _T("]"));
			continue;
		}

		(*iter).second.bState = bState;
	}

	g_kTblDataMgr.SetContDef(kCopyCont);

	if(false == rkResult.contUserData.IsEmpty())
	{
		BM::Stream kPacket(PT_ANS_GMCMD_OXQUIZ_RELOAD);
		kPacket.Push(rkResult.contUserData);
		SendToOXGuizEvent(kPacket);
	}

	return true;
}

bool Q_DQT_LUCKYSTAR_LOAD_EVENT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	if(rkResult.vecResultCount.empty())
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" Empty ResultCount") << _T(", Query=") << rkResult.Command());
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();


	int const iEventCount = (*count_iter); ++count_iter;

	CONT_LUCKYSTAREVENTINFO kContEvent;

	for(int i = 0;i < iEventCount;++i)
	{
		CONT_LUCKYSTAREVENTINFO::mapped_type kEvent;

		(*itr).Pop(kEvent.iEventID);		++itr;
		(*itr).Pop(kEvent.iUserLimit);		++itr;
		(*itr).Pop(kEvent.iCostMoney);		++itr;
		BM::DBTIMESTAMP_EX kDate;
		(*itr).Pop(kDate);					++itr;
		kEvent.kStartDate = kDate;
		(*itr).Pop(kDate);					++itr;
		kEvent.kEndDate = kDate;
		(*itr).Pop(kEvent.kEventNotiClose);	++itr;
		(*itr).Pop(kEvent.kMailTitle);		++itr;
		(*itr).Pop(kEvent.kMailText);		++itr;
		(*itr).Pop(kEvent.iDelayTime);		++itr;
		(*itr).Pop(kEvent.iLimitTime);		++itr;
		(*itr).Pop(kEvent.iLuckyPoint);		++itr;

		auto kPair = kContEvent.insert(std::make_pair(kEvent.iEventID,kEvent));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Lucky Star Event Error !! EventNo [") << kEvent.iEventID << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed Data"));
			return false;
		}
	}

	int const iRewardCount = (*count_iter); ++count_iter;

	CONT_LUCKYSTARREWARD::key_type		kRewardKey;
	SLUCKYSTARREWARD					kReward;	

	CONT_LUCKYSTARREWARD kContReward;
	CONT_LUCKYSTARREWARDSTEP kContRewardStep;

	for(int i = 0;i < iRewardCount;++i)
	{
		(*itr).Pop(kRewardKey.iRewardNo);	++itr;
		(*itr).Pop(kRewardKey.iStep);		++itr;

		(*itr).Pop(kReward.iMoney);			++itr;
		(*itr).Pop(kReward.iItemNo);		++itr;
		(*itr).Pop(kReward.iItemCount);		++itr;

		CONT_LUCKYSTARREWARD::iterator reward_iter = kContReward.find(kRewardKey);
		if(kContReward.end() == reward_iter)
		{
			kContReward[kRewardKey].push_back(kReward);
		}
		else	
		{
			(*reward_iter).second.push_back(kReward);
		}

		kContRewardStep[kRewardKey.iRewardNo].push_back(kRewardKey.iStep);
	}

	for(CONT_LUCKYSTARREWARDSTEP::iterator step_iter = kContRewardStep.begin();kContRewardStep.end()!=step_iter;++step_iter)
	{
		CONT_LUCKYSTARREWARDSTEP::mapped_type & kContStep = (*step_iter).second;
		std::sort(kContStep.begin(), kContStep.end());
		CONT_LUCKYSTARREWARDSTEP::mapped_type::iterator pos = std::unique(kContStep.begin(), kContStep.end());
		kContStep.erase(pos, kContStep.end());
	}

	int const iJoinCount = (*count_iter); ++count_iter;

	CONT_LUCKYSTAREVENTJOINREWARD kContEventJoinReward;

	for(int i = 0;i < iJoinCount;++i)
	{
		int iEventNo = 0;
		int iRewardNo = 0;
		(*itr).Pop(iEventNo);		++itr;
		(*itr).Pop(iRewardNo);		++itr;

		CONT_LUCKYSTARREWARDSTEP::iterator reward_iter = kContRewardStep.find(iRewardNo);
		if( (kContRewardStep.end() != reward_iter)
		&& (false==(*reward_iter).second.empty()) )
		{
			kContEventJoinReward[iEventNo].push_back(iRewardNo);
		}
	}

	for(CONT_LUCKYSTAREVENTJOINREWARD::iterator join_iter = kContEventJoinReward.begin();kContEventJoinReward.end()!=join_iter;++join_iter)
	{
		CONT_LUCKYSTAREVENTJOINREWARD::mapped_type & kContJoin = (*join_iter).second;
		std::sort(kContJoin.begin(), kContJoin.end());
		CONT_LUCKYSTAREVENTJOINREWARD::mapped_type::iterator pos = std::unique(kContJoin.begin(), kContJoin.end());
		kContJoin.erase(pos, kContJoin.end());
	}

	g_kTblDataMgr.SetContDef(kContEvent);
	g_kTblDataMgr.SetContDef(kContReward);
	g_kTblDataMgr.SetContDef(kContRewardStep);	
	g_kTblDataMgr.SetContDef(kContEventJoinReward);	

	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LUCKYSTAR_LOAD_EVENT_SUB, _T("EXEC [dbo].[UP_Load_LuckyStarEventSub]"));
	kQuery.contUserData.Push(rkResult.contUserData);
	g_kCoreCenter.PushQuery(kQuery);

	return true;
}


bool Q_DQT_LUCKYSTAR_LOAD_EVENT_SUB(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	if(rkResult.vecResultCount.empty())
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" Empty ResultCount") << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_LUCKYSTAREVENTINFO kCopyCont;

	{
		CONT_LUCKYSTAREVENTINFO const * pkCont = NULL;
		g_kTblDataMgr.GetContDef(pkCont);
		if(!pkCont)
		{
			return false;
		}

		kCopyCont = (*pkCont);
	}

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	int const iEventCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iEventCount;++i)
	{
		int  iEventID = 0;
		BYTE bState = 0;
		BYTE bSendResultType = 0;
		int  iLastRewardId = 0;
		int  iEventCount = 0;
		int  iLastResultStar = 0;
		int  iFlag = 0;

		(*itr).Pop(iEventID);			++itr;
		(*itr).Pop(bState);				++itr;
		(*itr).Pop(bSendResultType);	++itr;
		(*itr).Pop(iLastRewardId);		++itr;
		(*itr).Pop(iEventCount);		++itr;
		(*itr).Pop(iLastResultStar);	++itr;
		(*itr).Pop(iFlag);				++itr;

		CONT_LUCKYSTAREVENTINFO::iterator iter = kCopyCont.find(iEventID);
		if(iter == kCopyCont.end())
		{
			CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("WARNING !! NOT FOUND EVENT EVENTNO [") << iEventID << _T("]"));
			continue;
		}

		CONT_LUCKYSTAREVENTINFO::mapped_type & kSub = (*iter).second;
		kSub.bState = bState;
		kSub.bSendResultType = bSendResultType;
		kSub.iLastRewardId = iLastRewardId;
		kSub.iEventCount = iEventCount;
		kSub.iLastResultStar = iEventCount;
		kSub.iFlag = iFlag;
	}

	g_kTblDataMgr.SetContDef(kCopyCont);

	BM::Stream kPacket(PT_ANS_GMCMD_LUCKYSTAR_RELOAD);
	if(false == rkResult.contUserData.IsEmpty())
	{	
		kPacket.Push(true);
		kPacket.Push(rkResult.contUserData);
	}
	else
	{
		kPacket.Push(false);
	}
	SendToLuckyStarEvent(kPacket);

	return true;
}


bool Q_DQT_LUCKYSTAR_LOAD_JOINEDUSER(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	if(rkResult.vecResultCount.empty())
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" Empty ResultCount") << _T(", Query=") << rkResult.Command());
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();


	int const iEventCount = (*count_iter); ++count_iter;

	CONT_LUCKYSTARJOINEDUSERINFO kContUser;

	for(int i = 0;i < iEventCount;++i)
	{
		BM::GUID kMemberGuid;
		CONT_LUCKYSTARJOINEDUSERINFO::mapped_type kUser;

		(*itr).Pop(kUser.iEventID);		++itr;
		(*itr).Pop(kMemberGuid);		++itr;
		(*itr).Pop(kUser.kCharGuid);	++itr;
		(*itr).Pop(kUser.iStar);		++itr;
		(*itr).Pop(kUser.iStep);		++itr;

		auto kPair = kContUser.insert(std::make_pair(kMemberGuid,kUser));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Lucky Star Joined User Error !! EventNo [") << kUser.iEventID << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed Data"));
			return false;
		}
	}

	BM::Stream kPacket(PT_ANS_LUCKYSTAR_LOAD_JOINEDUSER);
	kPacket.Push(kContUser);
	SendToLuckyStarEvent(kPacket);

	return true;
}

bool Q_DQT_LUCKYSTAR_UPDATE_EVENT_SUB(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	return true;
}

bool Q_DQT_LUCKYSTAR_UPDATE_JOINEDUSER(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	return true;
}

bool Q_DQT_LUCKYSTAR_UPDATE_JOINEDUSER_READED(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	return true;
}



bool Q_DQT_LOAD_MACROCHECKTABLE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Can't load [TB_DefAchievements] table Result: ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	SMACRO_CHECK_TABLE kTable;
	if( rkVec.end() != result_iter )
	{
		result_iter->Pop( kTable.iMacroCheckTime );		++result_iter;
		result_iter->Pop( kTable.iMacroReleaseTime );	++result_iter;
		result_iter->Pop( kTable.iMacroCheckCount );	++result_iter;
		result_iter->Pop( kTable.iMacroPopupMax );		++result_iter;
		result_iter->Pop( kTable.iMacroInputTime );		++result_iter;

		BM::Stream kPacket(PT_I_M_MACRO_CHECK_TABLE_SYNC);
		kTable.WriteToPacket(kPacket);
		SendToServerType(CEL::ST_CENTER,kPacket);
	}
	return true;
}




void ReloadData_Contents()
{
	std::wstring kStrQuery;
	BM::Stream kPacket(PT_A_A_NFY_REFRESH_DB_DATA);
	BM::Stream kStorePacket;

	// DB Query�� Immidiately ����̱� ������, �ؿ� GetContDef �ϴ� �κа� �и��Ǿ�� �Ѵ�.
	{
		bool bIsImmidiate = true;
		{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_CASH_ITEM_SHOP, _T("EXEC [dbo].[UP_LoadDefCashShop8]"));	if( S_OK != g_kCoreCenter.PushQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
        {	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_SHOP_IN_EMPORIA, _T("EXEC [dbo].[UP_LoadDefShopInEmporia]"));	if( S_OK != g_kCoreCenter.PushQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
		{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_SHOP_IN_GAME, _T("EXEC [dbo].[UP_LoadDefShopInGame3]"));	if( S_OK != g_kCoreCenter.PushQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
		{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_SHOP_IN_STOCK, _T("EXEC [dbo].[UP_LoadDefShopInStock]"));	if( S_OK != g_kCoreCenter.PushQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto LABEL_QUERY_ERROR;}}
	}

	{

		g_kControlDefMgr.StoreValueKey(BM::GUID::Create());

		INFO_LOG(BM::LOG_LV7, _T("Set StoreKey ReloadData::")<< g_kControlDefMgr.StoreValueKey());

		SReloadDef kReloadDef;
		int const iLoadDef = PgControlDefMgr::EDef_StoreMgr;
		g_kTblDataMgr.GetReloadDef(kReloadDef, iLoadDef);
		if ( !g_kControlDefMgr.Update(kReloadDef, iLoadDef) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot update StoreMgr"));
		}
	}

	{
        TABLE_LOCK(CONT_TBL_SHOP_IN_EMPORIA) kObjLock0;
		g_kTblDataMgr.GetContDef(kObjLock0);
		CONT_TBL_SHOP_IN_EMPORIA const *pkShopInEmporia = kObjLock0.Get();
		TABLE_LOCK(CONT_TBL_SHOP_IN_GAME) kObjLock1;
		g_kTblDataMgr.GetContDef(kObjLock1);
		CONT_TBL_SHOP_IN_GAME const *pkShopInGame = kObjLock1.Get();
		TABLE_LOCK(CONT_TBL_SHOP_IN_STOCK) kObjLock2;
		g_kTblDataMgr.GetContDef(kObjLock2);
		CONT_TBL_SHOP_IN_STOCK const *pkShopInStock = kObjLock2.Get();
		TABLE_LOCK(CONT_DEF_CASH_SHOP) kObjLock3;
		g_kTblDataMgr.GetContDef(kObjLock3);
		CONT_DEF_CASH_SHOP const *pkContShop = kObjLock3.Get();
		TABLE_LOCK(CONT_DEF_CASH_SHOP_ARTICLE) kObjLock4;
		g_kTblDataMgr.GetContDef(kObjLock4);
		CONT_DEF_CASH_SHOP_ARTICLE const *pkContArticle = kObjLock4.Get();
		TABLE_LOCK(CONT_SHOPNPC_GUID) kObjLock5;
		g_kTblDataMgr.GetContDef(kObjLock5);
		CONT_SHOPNPC_GUID const *pkShopNpcGuid = kObjLock5.Get();
		TABLE_LOCK(CONT_CASH_SHOP_ITEM_LIMITSELL) kObjLock6;
		g_kTblDataMgr.GetContDef(kObjLock6);
		CONT_CASH_SHOP_ITEM_LIMITSELL const *pkContShopLimitSell = kObjLock6.Get();

		//����ǥ GUID
		kPacket.Push(g_kControlDefMgr.StoreValueKey());
		//ĳ�ü� ���
		PU::TWriteTable_MM(kPacket, *pkContShop);
		PU::TWriteTable_MM(kPacket, *pkContArticle);
		PU::TWriteTable_MM(kPacket, *pkContShopLimitSell);
        //�������� �� ���
        PU::TWriteTable_MM(kPacket, *pkShopInEmporia);
		//�ΰ��� �� ��� 
		PU::TWriteTable_MM(kPacket, *pkShopInGame);
		//�ֽ��� �� ���
		PU::TWriteTable_MM(kPacket, *pkShopInStock);
		PU::TWriteTable_AA(kPacket, *pkShopNpcGuid);

		g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);

		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" ReLoad Success"));		
	}
	return;
LABEL_QUERY_ERROR:
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" ReLoadFailed=") << kStrQuery );		
	}
}


bool Q_DQT_SYNC_LOCAL_LIMITED_ITEM(CEL::DB_RESULT &rkResult)
{//â�� ������ limit�� 1�� �ű�°�.
	if( CEL::DR_SUCCESS != rkResult.eRet 
		&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Limit Field Refresh Failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		//		INFO_LOG( BM::LOG_LV6, __FL__ << _T("��ũ����!"));
		return false;
	}

	//	INFO_LOG( BM::LOG_LV6, __FL__ << _T("��ũ ����!"));
	return true;
}

bool Q_DQT_SAVE_PENALTY(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_SUCCESS != rkResult.eRet 
		&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Failed " << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}
	return true;
}

bool Q_DQT_CHECK_PENALTY(CEL::DB_RESULT &rkResult)
{
	BM::DBTIMESTAMP_EX kEndTime;
	__int64 i64RemainTime = 0i64;

	if(	CEL::DR_SUCCESS == rkResult.eRet 
		||	CEL::DR_NO_RESULT == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		if( itr != rkResult.vecArray.end() )
		{
			itr->Pop( kEndTime );
		}

		__int64 i64EndTime = 0i64;
		CGameTime::DBTimeEx2SecTime( kEndTime, i64EndTime, CGameTime::SECOND );

		__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::SECOND );
		i64RemainTime = i64EndTime - i64NowTime;
		if ( i64RemainTime < 0i64 )
		{
			i64RemainTime = 0i64;
		}
	}

	short nChannel = 0;
	SGroundKey kGndkey;
	BM::GUID kCharGuid;
	WORD wType = 0;
	rkResult.contUserData.Pop( nChannel );
	rkResult.contUserData.Pop( kGndkey );
	rkResult.contUserData.Pop( kCharGuid );
	rkResult.contUserData.Pop( wType );

	if ( 0i64 < i64RemainTime )
	{
		if ( rkResult.contUserData.RemainSize() >= sizeof(BM::Stream::DEF_STREAM_TYPE) )
		{
			BM::Stream kAnsPacket( PT_N_C_NFY_HAVE_PENALTY, wType );
			kAnsPacket.Push( i64RemainTime );
			g_kRealmUserMgr.Locked_SendToUser( kCharGuid, kAnsPacket, false );
		}
	}
	else
	{
		BM::Stream kAnsPacket( PT_N_M_ANS_CHECK_PENALTY, kCharGuid );
		kAnsPacket.Push( wType );
		kAnsPacket.Push( rkResult.contUserData );
		::SendToGround( nChannel, kGndkey, kAnsPacket, true );
	}

	return true;
}

int const MAX_CCE_REWARD_NUM = 10;

bool Q_DQT_LOAD_CREATE_CHARACTER_EVENT_REWARD(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_CCE_REWARD_TABLE::key_type kKey;
	CONT_CCE_REWARD_TABLE::mapped_type kData;
	CONT_CCE_REWARD_TABLE kTable;

	while( rkVec.end() != result_iter )
	{
		result_iter->Pop( kKey );				++result_iter;

		CONT_CCE_REWARD::value_type	kValue;
		CONT_CCE_REWARD kRewards;
		for(int i = 0;i < MAX_CCE_REWARD_NUM;++i)
		{
			result_iter->Pop( kValue.iItemNo);				++result_iter;
			result_iter->Pop( kValue.siCount);				++result_iter;
			result_iter->Pop( kValue.bTimeType);			++result_iter;
			result_iter->Pop( kValue.iUseTime);				++result_iter;
			result_iter->Pop( kValue.iRarityControlNo);		++result_iter;

			if(0 < kValue.iItemNo && 0 < kValue.siCount)
			{
				kData.kRewards.push_back(kValue);
			}
		}

		if(false == kTable.insert(std::make_pair(kKey,kData)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Event Reward Key Error !!") << _T("Key:") << kKey);
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool Q_DQT_LOAD_DEFREALTYDEALER(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_REALTYDEALER::key_type kKey;
	CONT_REALTYDEALER kTable;

	while( rkVec.end() != result_iter )
	{
		int iGroundNo = 0,
			iCost = 0;

		result_iter->Pop( kKey );				++result_iter;
		result_iter->Pop( iGroundNo );			++result_iter;
		result_iter->Pop( iCost );				++result_iter;

		if(false == kTable[kKey].kTable.insert(std::make_pair(iGroundNo,iCost)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("House Shop Article Key Error !!") << _T("Key:") << iGroundNo);
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool Q_DQT_LOAD_DEF_RARE_OPT_MAGIC(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_RAREOPT_MAGIC kTable;

	while( rkVec.end() != result_iter )
	{
		CONT_RAREOPT_MAGIC::key_type kKey;
		CONT_RAREOPT_MAGIC::mapped_type kValue;
		result_iter->Pop( kKey );						++result_iter;
		result_iter->Pop( kValue.iSuccessControlNo );	++result_iter;

		for(int i = 0;i < MAX_RAREOPT_MAGIC_ARRAY_NUM;++i)
		{
			result_iter->Pop( kValue.iMagicNo[i] );		++result_iter;
		}

		if(false == kTable.insert(std::make_pair(kKey,kValue)).second)
		{
			DefLoadErrorLog::AddErrorMsg( BM::vstring()<<__FL__<<_T("RARE OPTION MAGIC ERROR !!") << _T("Key:") << kKey);
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool Q_DQT_LOAD_DEF_RARE_OPT_SKILL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_RAREOPT_SKILL kTable;

	while( rkVec.end() != result_iter )
	{
		CONT_RAREOPT_SKILL::key_type kKey;
		CONT_RAREOPT_SKILL::mapped_type kValue;
		result_iter->Pop( kKey );								++result_iter;
		result_iter->Pop( kValue.iSuccessControlNo );			++result_iter;
		result_iter->Pop( kValue.iSkillLvSuccessControlNo );	++result_iter;

		for(int i = 0;i < MAX_RAREOPT_SKILL_ARRAY_NUM;++i)
		{
			result_iter->Pop( kValue.iSkillIdx[i] );			++result_iter;
		}

		if(false == kTable.insert(std::make_pair(kKey,kValue)).second)
		{
			DefLoadErrorLog::AddErrorMsg( BM::vstring()<<__FL__<<_T("RARE OPTION SKILL ERROR !!") << _T("Key:") << kKey);
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}



bool Q_DQT_LOAD_DEF_MIXUPITEM(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MIXUPITEM kTable;

	while( rkVec.end() != result_iter )
	{
		CONT_MIXUPITEM::key_type kKey;
		CONT_MIXUPITEM::mapped_type kValue;
		result_iter->Pop( kKey.iItemGrade );					++result_iter;
		result_iter->Pop( kKey.iEquipPos );						++result_iter;
		result_iter->Pop( kKey.iGenderLimit );					++result_iter;

		result_iter->Pop( kValue.iSuccessControlNo );			++result_iter;
		result_iter->Pop( kValue.iRarityControlNo );			++result_iter;

		for(int i = 0;i < MAX_MIXUP_ITEM_ARRAY_NUM;++i)
		{
			result_iter->Pop( kValue.iItemBagNo[i] );			++result_iter;
		}

		if(false == kTable.insert(std::make_pair(kKey,kValue)).second)
		{
			DefLoadErrorLog::AddErrorMsg( BM::vstring()<<__FL__
				<< _T("MIXUP KEY ERROR !!") 
				<< _T("Grade:") << kKey.iItemGrade 
				<< _T(" EquipPos:") << kKey.iEquipPos
				<< _T(" GenderLimit:") << kKey.iGenderLimit);
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}




bool Q_DQT_LOAD_DEF_GAMBLEMACHINE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_GAMBLEMACHINE kTable; //���� �ΰ� ���� ����
	CONT_GAMBLEMACHINE kTable1; //������ ����

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iItemCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iItemCount;++i)
	{
		CONT_GAMBLEMACHINE::key_type		kKey;
		SGAMBLEMACHINEITEM					kValue;
		int iItemNo1 = 0; //�ΰ��� ������ ��ȣ
		int iItemNo2 = 0; //������ ������ ��ȣ

		result_iter->Pop( kKey );				++result_iter;
		result_iter->Pop( iItemNo1 );			++result_iter;
		result_iter->Pop( iItemNo2 );			++result_iter;
		result_iter->Pop( kValue.siCount );		++result_iter;
		result_iter->Pop( kValue.iRate );		++result_iter;
		result_iter->Pop( kValue.bBroadcast );	++result_iter;
		result_iter->Pop( kValue.bTimeType );	++result_iter;
		result_iter->Pop( kValue.siUseTime );	++result_iter;
		result_iter->Pop( kValue.iGroupNo );	++result_iter;

		if(0 < iItemNo1) //�ΰ��� ������ ��ȣ�� �ִٸ�
		{
			kValue.iItemNo = iItemNo1;
			kTable[kKey].kCont.push_back(kValue);
		}
		if(0 < iItemNo2) //���� ������ ��ȣ�� �ִٸ�
		{
			kValue.iItemNo = iItemNo2;
			kTable1[kKey].kCont.push_back(kValue);
		}
	}

	int const iCostCount = (*count_iter); ++count_iter;

	CONT_GAMBLEMACHINECOST kContCost;

	for(int i = 0;i < iCostCount;++i)
	{
		CONT_GAMBLEMACHINECOST::key_type kKey;
		CONT_GAMBLEMACHINECOST::mapped_type kValue;
		result_iter->Pop( kKey );		++result_iter;
		result_iter->Pop( kValue );		++result_iter;

		if(false == kContCost.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("CONT_GAMBLEMACHINECOST Fail!! [Key][") << kKey << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	int const iResCount = (*count_iter); ++count_iter;

	CONT_GAMBLEMACHINEGROUPRES kContGroup;

	for(int i = 0;i < iResCount;++i)
	{
		CONT_GAMBLEMACHINEGROUPRES::key_type kKey;
		CONT_GAMBLEMACHINEGROUPRES::mapped_type kValue;

		result_iter->Pop( kKey );					++result_iter;
		kValue.iGroupNo = kKey;
		result_iter->Pop( kValue.bU );				++result_iter;
		result_iter->Pop( kValue.bV );				++result_iter;
		result_iter->Pop( kValue.siUVIndex );		++result_iter;
		result_iter->Pop( kValue.kIconPath );		++result_iter;
		result_iter->Pop( kValue.kDescription );	++result_iter;
		result_iter->Pop( kValue.iDisplayRank );	++result_iter;
		result_iter->Pop( kValue.kTitle );			++result_iter;

		if(false == kContGroup.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("CONT_GAMBLEMACHINECOST Fail!! [Key][") << kKey << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	int const iMixupCount = (*count_iter); ++count_iter;

	CONT_GAMBLEMACHINEMIXUP kContMixup;

	for(int i = 0;i < iMixupCount;++i)
	{
		CONT_GAMBLEMACHINEMIXUP::key_type kKey;
		CONT_GAMEBLEMACHINEMIXUPLELEMENT::value_type kValue;

		result_iter->Pop( kKey );					++result_iter;
		result_iter->Pop( kValue.iGradeNo );			++result_iter;
		result_iter->Pop( kValue.iRate );			++result_iter;

		kContMixup[kKey].iMixPoint = kKey;
		kContMixup[kKey].kCont.push_back(kValue);
	}

	int const iMixPointCount = (*count_iter); ++count_iter;

	CONT_GAMBLEMACHINEMIXUPPOINT kContMixPoint;

	for(int i = 0;i < iMixPointCount;++i)
	{
		CONT_GAMBLEMACHINEMIXUPPOINT::key_type kKey;
		CONT_GAMBLEMACHINEMIXUPPOINT::mapped_type kValue;

		result_iter->Pop( kKey );	++result_iter;
		result_iter->Pop( kValue );	++result_iter;
		if(false == kContMixPoint.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("CONT_GAMBLEMACHINEMIXPOINT Fail!! [Key][") << kKey << _T("]"));
			return false;
		}
	}

	BM::Stream kData;
	PU::TWriteTable_AM(kData,kTable);
	PU::TWriteTable_AM(kData,kTable1);
	PU::TWriteTable_AA(kData,kContCost);
	PU::TWriteTable_AM(kData,kContGroup);
	PU::TWriteTable_AM(kData,kContMixup);
	PU::TWriteTable_AA(kData,kContMixPoint);

	g_kGambleMachine.ReadFromPacket(kData);
	kData.PosAdjust();

	if(true == rkResult.contUserData.IsEmpty())
	{
		return true;
	}

	BM::Stream kPacket(PT_SYNC_GAMBLEMACHINE);
	kPacket.Push(kData);
	SendToServerType(CEL::ST_CENTER,kPacket);

	g_kCoreCenter.ClearQueryResult(rkResult);

	return true;
}

bool Q_DQT_GMCMD_CASHITEMGIFT_INSERT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	BM::GUID kGmCharGuid;
	rkResult.contUserData.Pop( kGmCharGuid );

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if(iter != rkResult.vecArray.end())
	{
		int iError = 0;
		(*iter).Pop(iError);		++iter;
		if(iError)
		{
			g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 99500+iError, EL_Warning, false );
			return true;
		}

		int iEventNo = 0;
		(*iter).Pop(iEventNo);		++iter;

		g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 99500, EL_Normal, false );

		BM::Stream kPacket(PT_M_C_NFY_CHAT);
		kPacket.Push(static_cast<BYTE>(CT_CASHITEM_GIFT));
		kPacket.Push(99008);
		kPacket.Push(iEventNo);
		g_kRealmUserMgr.Locked_SendToUser(kGmCharGuid, kPacket, false);
	}
	else
	{
		g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 99006, EL_Warning, false );
	}

	return true;
}

bool Q_DQT_GMCMD_CASHITEMGIFT_DELETE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if(iter != rkResult.vecArray.end())
	{
		BM::GUID kGmCharGuid;
		rkResult.contUserData.Pop( kGmCharGuid );

		int iError = 0;
		(*iter).Pop(iError);		++iter;		
		if(iError)
		{
			g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 99006, EL_Warning, false );
			return true;
		}

		g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 99007, EL_Normal, false );
	}


	return true;
}

bool Q_DQT_LOAD_EVENT_ITEM_REWARD(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_EVENT_ITEM_REWARD kCont;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_EVENT_ITEM_REWARD::key_type	kKey;

		(*iter).Pop(kKey);					++iter;

		CONT_EVENT_ITEM_REWARD_ELEMENT::value_type	kValue;

		(*iter).Pop(kValue.iItemNo);		++iter;
		(*iter).Pop(kValue.siCount);		++iter;
		(*iter).Pop(kValue.bTimeType);		++iter;
		(*iter).Pop(kValue.siUseTime);		++iter;
		(*iter).Pop(kValue.bRarity);		++iter;
		(*iter).Pop(kValue.bEnchantType);	++iter;
		(*iter).Pop(kValue.bEnchantLv);		++iter;
		(*iter).Pop(kValue.iRate);			++iter;
		(*iter).Pop(kValue.bBroadcast);		++iter;

		if(0 < kValue.iRate)
		{
			kCont[kKey].iTotalRate += kValue.iRate;
			kCont[kKey].kContRandom.push_back(kValue);
		}
		else
		{
			kCont[kKey].kContAll.push_back(kValue);
		}
	}

	if(false == rkResult.contUserData.IsEmpty())
	{
		BM::Stream kPacket(PT_SYNC_EVENT_ITEM_REWARD);
		PU::TWriteTable_AM(kPacket,kCont);
		SendToServerType(CEL::ST_CENTER,kPacket);
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);

	return true;
}

bool Q_DQT_LOAD_TREASURE_CHEST(CEL::DB_RESULT& rkResult)
{	//����: ���Լ��� ���� ���̺������Ϳ� insert�ؼ� �����س����Ƿ� ���ε尡 �ʿ��ϴٸ� ���� �����͸� �Ұ�����߸� �Ѵ�.
	//(DR2_Def�� DR_Local ���̺��� �������Ƿ�...)
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	struct TreasureChestKey
	{
		TreasureChestKey(int const chestIdx, int const keyIdx, int const itemNo, short const count)
			: kChestIdx(chestIdx), kKeyIdx(keyIdx), iItemNo(itemNo), siCount(count)
		{
		}

		bool operator < (TreasureChestKey const &rhs)const
		{
			if( kChestIdx < rhs.kChestIdx )	{return true;}
			if( kChestIdx > rhs.kChestIdx )	{return false;}

			if( kKeyIdx < rhs.kKeyIdx )	{return true;}
			if( kKeyIdx > rhs.kKeyIdx )	{return false;}

			if( iItemNo < rhs.iItemNo )	{return true;}
			if( iItemNo > rhs.iItemNo )	{return false;}

			if( siCount < rhs.siCount )	{return true;}
			if( siCount > rhs.siCount )	{return false;}

			return false;
		}

		operator std::wstring const& ()const
		{
			kStr = L"";
			kStr += L"<";
			kStr += kChestIdx;
			kStr += L",";
			kStr += kKeyIdx;
			kStr += L",";
			kStr += iItemNo;
			kStr += L",";
			kStr += siCount;
			kStr += L">";
			return kStr;
		}

		int kChestIdx;
		int kKeyIdx;
		int iItemNo;
		short siCount;
		mutable BM::vstring kStr;
	};
	typedef std::map<TreasureChestKey,CONT_EVENT_ITEM_REWARD_ELEMENT::value_type> CONT_VALUE;

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper<CONT_VALUE::key_type, CONT_VALUE::mapped_type, CONT_VALUE, BM::vstring, DefLoadErrorLog::AddError> NationCodeUtil(L"Duplicate key[" DBCACHE_KEY_PRIFIX L"]");

	CONT_TREASURE_CHEST kCont;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_TREASURE_CHEST::key_type				kChestIdx;
		CONT_TREASURE_CHEST_KEY_GROUP::key_type		kKeyIdx;
		(*iter).Pop(NationCodeStr);						++iter;
		(*iter).Pop(kChestIdx);							++iter;
		(*iter).Pop(kKeyIdx);							++iter;

		CONT_EVENT_ITEM_REWARD_ELEMENT::value_type		kValue;
		(*iter).Pop(kValue.iItemNo);					++iter;
		(*iter).Pop(kValue.siCount);					++iter;
		(*iter).Pop(kValue.bTimeType);					++iter;
		(*iter).Pop(kValue.siUseTime);					++iter;
		(*iter).Pop(kValue.bRarity);					++iter;
		(*iter).Pop(kValue.bEnchantType);				++iter;
		(*iter).Pop(kValue.bEnchantLv);					++iter;
		(*iter).Pop(kValue.iRate);						++iter;
		(*iter).Pop(kValue.bBroadcast);					++iter;

		NationCodeUtil.Add(NationCodeStr, TreasureChestKey(kChestIdx,kKeyIdx,kValue.iItemNo,kValue.siCount), kValue, __FUNCTIONW__, __LINE__);
	}

	CONT_VALUE const& kContResult = NationCodeUtil.GetResult();
	for(CONT_VALUE::const_iterator c_it=kContResult.begin(); c_it!=kContResult.end(); ++c_it)
	{
		int const& kChestIdx = (*c_it).first.kChestIdx;
		int const& kKeyIdx = (*c_it).first.kKeyIdx;
		CONT_VALUE::mapped_type const& kValue = (*c_it).second;
		kCont[kChestIdx].kCont[kKeyIdx].iTotalRate += kValue.iRate; //Rand Result = Rate / TotalRate
		kCont[kChestIdx].kCont[kKeyIdx].kContElem.push_back(kValue);
	}


	{
		TABLE_LOCK(CONT_TREASURE_CHEST) kObjLock;
		g_kTblDataMgr.GetContDef(kObjLock);
		CONT_TREASURE_CHEST const* pkContSrc = kObjLock.Get();
		if(pkContSrc != NULL && !pkContSrc->empty())
		{
			kCont.insert(pkContSrc->begin(), pkContSrc->end());
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);

	return true;
}

bool Q_DQT_PROCESS_SETPLAYERPLAYTIME(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	
	if(rkResult.vecArray.end() == iter)
	{
		return false;
	}

	BM::GUID kMemberGuid;
	(*iter).Pop(kMemberGuid);	++iter;

	int iAccConSec = 0;
	int iAccDicSec = 0;
	rkResult.contUserData.Pop(iAccConSec);
	rkResult.contUserData.Pop(iAccDicSec);

	CEL::DB_QUERY kQuery( DT_MEMBER, DQT_UPDATE_SETPLAYERPLAYTIME, L"EXEC [dbo].[up_UpdateSetPlayerPlayTime]" );
	kQuery.PushStrParam( kMemberGuid );
	kQuery.PushStrParam( iAccConSec );
	kQuery.PushStrParam( iAccDicSec );
	g_kCoreCenter.PushQuery( kQuery );
	return true;
}

bool Q_DQT_UPDATE_SETPLAYERPLAYTIME(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_SUCCESS != rkResult.eRet 
		&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Failed " << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}
	return true;
}

bool Q_DQT_LOAD_DEF_MONSTER_GRADE_PROBABILITY(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP kCont;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP::key_type kKey;
		CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY::value_type kValue;
		(*iter).Pop(kKey);							++iter;
		(*iter).Pop(kValue.iProbability);			++iter;
		(*iter).Pop(kValue.iMonsterEnchantGrade);	++iter;

		CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP::iterator find_iter = kCont.find(kKey);
		if( kCont.end() == find_iter )
		{
			auto kRet = kCont.insert(std::make_pair(kKey,CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP::mapped_type()));
			if( kRet.second )
			{
				find_iter = kRet.first;
			}
		}
		if( kCont.end() != find_iter )
		{
			(*find_iter).second.Add( kValue );
		}
	}

	{
		CONT_DEF_MONSTER_ENCHANT_GRADE const* pkDefMonEnchantGrade = NULL;
		g_kTblDataMgr.GetContDef(pkDefMonEnchantGrade);
		CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP::const_iterator group_iter = kCont.begin();
		while( kCont.end() != group_iter )
		{
			CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP::key_type const& rkKey = (*group_iter).first;
			CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP::mapped_type const& rkVal = (*group_iter).second;
			CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY::const_iterator prob_iter = rkVal.kContProbability.begin();
			while( rkVal.kContProbability.end() != prob_iter )
			{
				CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY::value_type const& rkProb = (*prob_iter);
				if( 0 < rkProb.iMonsterEnchantGrade
				&&	pkDefMonEnchantGrade->end() == pkDefMonEnchantGrade->find(rkProb.iMonsterEnchantGrade) )
				{
					DefLoadErrorLog::AddErrorMsg(BM::vstring()<<__FL__<<L"Wrong MonterEnchantProbGroup[Group"<<rkKey<<L", Probability:"<<rkProb.iProbability<<L", MonsterEnchantGradeNo:"<<rkProb.iMonsterEnchantGrade<<L"], Can't find [MonsterEnchantGradeNo] in [TB_DefDefMonsterEnchantGrade] table" );
				}
				++prob_iter;
			}
			++group_iter;
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}
bool Q_DQT_LOAD_DEF_SUPER_GROUND(const char* rkTbPath)
{
	BM::Stream::STREAM_DATA buff;
	CsvParser csv;
	csvdb::load(buff, csv, BM::vstring(rkTbPath));

	CONT_DEF_SUPER_GROUND_GROUP kCont;
	size_t f_iSuperGroundNo = 0, f_iGroundNo = 1, f_iOrderNo = 2,
		   f_Memo = 3, f_Level1_MonsterEnchantGroupNo = 4,
		   f_Level1_ItemRairityNo = 7, f_Level1_ItemEnchantControlNo = 10,
		   f_Level1_BossItemContainerNo = 13;
	while(csv.next_row()/*skip header*/)
	{
		CONT_SUPER_GROUND::value_type kValue;
		CONT_DEF_SUPER_GROUND_GROUP::key_type kKey = csv.col_int(f_iSuperGroundNo);
		kValue.iGroundNo = csv.col_int(f_iGroundNo);
		kValue.iOrderNo = csv.col_int(f_iOrderNo);
		for( int iCur = 0; MAX_SUPER_GROUND_MODE_NO > iCur; ++iCur )
			kValue.aiMonsterEnchantGroupNo[iCur] = csv.col_int(f_Level1_MonsterEnchantGroupNo + iCur);

		for( int iCur = 0; MAX_SUPER_GROUND_MODE_NO > iCur; ++iCur )
			kValue.aiItemRarityNo[iCur] = csv.col_int(f_Level1_ItemRairityNo + iCur);

		for( int iCur = 0; MAX_SUPER_GROUND_MODE_NO > iCur; ++iCur )
			kValue.aiItemEnchantControlNo[iCur] = csv.col_int(f_Level1_ItemEnchantControlNo + iCur);

		for( int iCur = 0; MAX_SUPER_GROUND_MODE_NO > iCur; ++iCur )
			kValue.aiBossItemContainerNo[iCur] = csv.col_int(f_Level1_BossItemContainerNo + iCur);

		CONT_DEF_SUPER_GROUND_GROUP::iterator find_iter = kCont.find(kKey);
		if( kCont.end() == find_iter )
		{
			std::pair<CONT_DEF_SUPER_GROUND_GROUP::iterator, bool> kRet = kCont.insert(std::make_pair(kKey,CONT_DEF_SUPER_GROUND_GROUP::mapped_type()));
			if( kRet.second )
			{
				find_iter = kRet.first;
			}
		}
		if( kCont.end() != find_iter )
		{
			(*find_iter).second.kContSuperGround.push_back(kValue);
		}
	}

	{
		CONT_DEFMAP const* pkDefMap = NULL;
		CONT_DEF_ITEM_RARITY_CONTROL const* pkDefItemRarity = NULL;
		CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP const* pkDefMonGradeProbGroup = NULL;
		CONT_DEF_ITEM_PLUSUP_CONTROL const* pkDefEnchantUpControl = NULL;
		CONT_DEF_ITEM_CONTAINER const* pkDefItemContainer = NULL;
		g_kTblDataMgr.GetContDef(pkDefMap);
		g_kTblDataMgr.GetContDef(pkDefItemRarity);
		g_kTblDataMgr.GetContDef(pkDefMonGradeProbGroup);
		g_kTblDataMgr.GetContDef(pkDefEnchantUpControl);
		g_kTblDataMgr.GetContDef(pkDefItemContainer);
		CONT_DEF_SUPER_GROUND_GROUP::const_iterator super_iter = kCont.begin();
		while( kCont.end() != super_iter )
		{
			CONT_DEF_SUPER_GROUND_GROUP::key_type const& rkKey = (*super_iter).first;
			CONT_DEF_SUPER_GROUND_GROUP::mapped_type const& rkVal = (*super_iter).second;
			if( MAX_SUPER_GROUND_COUNT < (*super_iter).second.kContSuperGround.size() )
			{
				DefLoadErrorLog::AddErrorMsg(BM::vstring()<<__FL__<<L"over maximmum ground count["<<MAX_SUPER_GROUND_COUNT<<L"] SuperGroundNo["<<rkKey<<L", Count:"<<rkVal.kContSuperGround.size()<<L"]");
			}
			CONT_SUPER_GROUND::const_iterator ground_iter = rkVal.kContSuperGround.begin();
			while( rkVal.kContSuperGround.end() != ground_iter )
			{
				int const iGroundNo = (*ground_iter).iGroundNo;
				if( pkDefMap->end() == pkDefMap->find(iGroundNo) )
				{
					DefLoadErrorLog::AddErrorMsg(BM::vstring()<<__FL__<<L"Wrong Ground[SuperGroundNo:"<<rkKey<<L", GroundNo:"<<iGroundNo<<L"], Can't find [GroundNo] in [TB_DefMap] table" );
				}
				for( int iCur = 0; MAX_SUPER_GROUND_MODE_NO > iCur; ++iCur )
				{
					int const iMonEnchantGradeGroupNo = (*ground_iter).aiMonsterEnchantGroupNo[iCur];
					if( 0 < iMonEnchantGradeGroupNo
					&&	pkDefMonGradeProbGroup->end() == pkDefMonGradeProbGroup->find(iMonEnchantGradeGroupNo) )
					{
						DefLoadErrorLog::AddErrorMsg(BM::vstring()<<__FL__<<L"Wrong Ground[MonEnchantGruop"<<(iCur+1)<<L":"<<iMonEnchantGradeGroupNo<<L", SuperGroundNo:"<<rkKey<<L", GroundNo:"<<iGroundNo<<L"], Can't find [MonsterEnchantGradeGroupNo] in [TB_DefMonsterGradeProbabilty] table" );
					}
					int const iRarityNo = (*ground_iter).aiItemRarityNo[iCur];
					if( 0 < iRarityNo
					&&	pkDefItemRarity->end() == pkDefItemRarity->find(iRarityNo) )
					{
						DefLoadErrorLog::AddErrorMsg(BM::vstring()<<__FL__<<L"Wrong Ground[Rarity"<<(iCur+1)<<L":"<<iRarityNo<<L", SuperGroundNo:"<<rkKey<<L", GroundNo:"<<iGroundNo<<L"], Can't find [RarityNo] in [TB_DefItemRarityControl] table" );
					}
					int const iEnchantControlNo = (*ground_iter).aiItemEnchantControlNo[iCur];
					if( 0 < iEnchantControlNo
					&&	pkDefEnchantUpControl->end() == pkDefEnchantUpControl->find(iEnchantControlNo) )
					{
						DefLoadErrorLog::AddErrorMsg(BM::vstring()<<__FL__<<L"Wrong Ground[Enchant"<<(iCur+1)<<L":"<<iEnchantControlNo<<L", SuperGroundNo:"<<rkKey<<L", GroundNo:"<<iGroundNo<<L"], Can't find [EnchantControlNo] in [TB_DefItemPlusUpControl] table" );
					}
					int const iBossItemContainerNo = (*ground_iter).aiBossItemContainerNo[iCur];
					if( 0 < iBossItemContainerNo
					&&	pkDefItemContainer->end() == pkDefItemContainer->find(iBossItemContainerNo) )
					{
						DefLoadErrorLog::AddErrorMsg(BM::vstring()<<__FL__<<L"Wrong Ground[ItemContainerNo"<<(iCur+1)<<L":"<<iBossItemContainerNo<<L", SuperGroundNo:"<<rkKey<<L", GroundNo:"<<iGroundNo<<L"], Can't find [ItemContainerNo] in [TB_DefItemContainer] table" );
					}
				}
				++ground_iter;
			}
			++super_iter;
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool Q_DQT_ADMIN_GM_COPYTHAT(CEL::DB_RESULT& rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	BYTE cClass = 0;
	short sLevel = 0;
	SPlayerStrategySkillData kData;
	PgInventory kTempInven;

	CEL::DB_RESULT_COUNT::iterator count_iter = rkResult.vecResultCount.begin();
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		int const iFirstResultCount = (*count_iter); ++count_iter;
		
		(*iter).Pop(sLevel);					++iter;
		(*iter).Pop(cClass);					++iter;
		(*iter).Pop(kData.abySkills);			++iter;
		(*iter).Pop(kData.abySkillExtends);		++iter;

		PgRealmUserManager::ProcessItemQuery( iter, rkResult.vecArray.end(), count_iter, kTempInven );
	}

	BM::Stream kNPacket( PT_C_M_GODCMD, rkResult.QueryOwner() );
	kNPacket.Push( static_cast< int >(GMCMD_COPY_THAT_FROM_DB) );
	kNPacket.Push( static_cast< int >(cClass) );
	kNPacket.Push( static_cast< int >(sLevel) );
	kNPacket.Push( kData );
	kTempInven.WriteToPacket( kNPacket, WT_DEFAULT );
	g_kRealmUserMgr.Locked_SendToUserGround(rkResult.QueryOwner(), kNPacket, false, true);

	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool Q_DQT_UPDATE_USER_JOBSKILL_HISTORYITEM(CEL::DB_RESULT &rkResult)
{
	return true;
}

bool Q_DQT_SELECT_MEMBERID(CEL::DB_RESULT& rkResult)
{
	if(	CEL::DR_SUCCESS != rkResult.eRet 
		&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Failed " << rkResult.Command() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	if( iter != rkResult.vecArray.end() )
	{
		BM::GUID kMemberGuid;
		(*iter).Pop(kMemberGuid);	++iter;

		EContentsMessageType eRetContentsType = PMET_NONE;
		rkResult.contUserData.Pop( eRetContentsType );
		{
			BM::Stream kRetPacket;
			kRetPacket.Push( rkResult.contUserData );
			kRetPacket.Push( kMemberGuid );
			::SendToRealmContents(eRetContentsType, kRetPacket);
		}
	}
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool Q_DQT_SAVE_CHARACTOR_SLOT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Charactor Slot Save failed ErrorCode<") << rkResult.eRet << _T("> CharcterGUID<") << rkResult.QueryOwner() << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}


bool Q_DQT_UPDATE_PREMIUM_CUSTOM_DATA(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
	 && CEL::DR_NO_RESULT != rkResult.eRet )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Premium CustomData Update failed ErrorCode<") << rkResult.eRet << _T("> CharcterGUID<") << rkResult.QueryOwner() << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool Q_DQT_EVENT_STORE(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_GEMSTORE const * pDefGemStore = NULL;
	g_kTblDataMgr.GetContDef(pDefGemStore); // CONT_GEMSTORE �����̳�.
	if( NULL == pDefGemStore )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_GEMSTORE is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_GEMSTORE * pContDefStore = const_cast<CONT_GEMSTORE *>(pDefGemStore); // const �Ӽ� ����.
	if( NULL == pContDefStore )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_GEMSTORE is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}


	CONT_GEMSTORE kCont;

	BM::GUID kNpcGuid;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_GEMSTORE_ARTICLE::mapped_type element;

		int iItemNo = 0;

		(*itor).Pop( kNpcGuid);		++itor;
		(*itor).Pop( iItemNo);		++itor;

		for(int i = 0;i < MAX_GEMS_NUM;++i)
		{
			int iGemNo = 0;
			short siCount = 0;
			(*itor).Pop( iGemNo);	++itor;
			(*itor).Pop( siCount);	++itor;

			if((iGemNo > 0) && (siCount > 0))
			{
				if(false == element.kContGems.insert(std::make_pair(iGemNo,siCount)).second)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_EVENTSTORE LOAD FAIL GEM:") << iGemNo);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}

		(*itor).Pop(element.iCP);	++itor;

		if(false == kCont[kNpcGuid].kContArticles.insert(std::make_pair(iItemNo,element)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_EVENTSTORE LOAD FAIL ITEMNO:") << iItemNo);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( kCont.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		pContDefStore->insert(kCont.begin(), kCont.end());
		g_kTblDataMgr.SetContDef(*pContDefStore);		
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Q_DQT_EVENTSTORE LOAD FAIL COUNT IS 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool Q_DQT_LOAD_JUMPINGCHAREVENT(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iEventCount = (*count_iter);		++count_iter;
	CONT_DEF_JUMPINGCHAREVENT kContEvent;
	for(int i=0; i<iEventCount; ++i)
	{
		CONT_DEF_JUMPINGCHAREVENT::mapped_type	kValue;
		(*itor).Pop( kValue.iEventNo );				++itor;
		(*itor).Pop( kValue.StartTime );			++itor;
		(*itor).Pop( kValue.EndTime );				++itor;
		(*itor).Pop( kValue.iLevelLimit );			++itor;
		(*itor).Pop( kValue.iMaxRewardCount );		++itor;
		(*itor).Pop( kValue.iRewardGroupNo );		++itor;

		kContEvent.insert(std::make_pair(kValue.iEventNo, kValue));
	}

	int const iRewardCount = (*count_iter);		++count_iter;
	CONT_DEF_JUMPINGCHAREVENT_REWARD kContReward;
	for(int i=0; i<iRewardCount; ++i)
	{
		CONT_DEF_JUMPINGCHAREVENT_REWARD::mapped_type::value_type	kValue;
		(*itor).Pop( kValue.iRewardGroupNo );		++itor;
		(*itor).Pop( kValue.iClass );				++itor;
		(*itor).Pop( kValue.iLevel );				++itor;
		(*itor).Pop( kValue.iMapNo );				++itor;
		(*itor).Pop( kValue.f_IngQuest );			++itor;
		(*itor).Pop( kValue.f_EndQuest );			++itor;
		(*itor).Pop( kValue.f_EndQuest2 );			++itor;
		(*itor).Pop( kValue.f_EndQuestExt );		++itor;
		(*itor).Pop( kValue.f_EndQuestExt2 );		++itor;
		(*itor).Pop( kValue.iItemGroupNo );			++itor;

		kContReward[kValue.iRewardGroupNo].insert(kValue);
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kContEvent);
	g_kTblDataMgr.SetContDef(kContReward);
	return true;
}