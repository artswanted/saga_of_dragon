#ifndef CONTENTS_CONTENTSSERVER_DATABASE_PGDBPROCESS_H
#define CONTENTS_CONTENTSSERVER_DATABASE_PGDBPROCESS_H

typedef enum : BYTE
{
	EPQT_NONE = 0,
	EPQT_GM_ORDER = 1,
} E_PREMIUM_QUERY_TYPE;

extern bool LoadDB(bool bReload=false);
extern bool LoadMemberDB();

extern void LoadMissionRank(CONT_DEF_MISSION_ROOT const *pkMissionRoot);
extern void LoadMissionRank(SMissionKey const& rkKey);
extern void SaveMissionRank(SMissionKey const& rkKey, const TBL_MISSION_RANK& rkRankData);
extern void CleanDeleteCharacter();

extern void ReloadData_Contents();

extern bool Q_DQT_UPDATE_MEMBER_PW(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_FRIENDLIST_ADD_BYGUID(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_FRIENDLIST_SELECT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_FRIENDLIST_DEL(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_FRIENDLIST_UPDATE_GROUP(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_TAKE_COUPON_REWARD(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_FRIENDLIST_UPDATE_CHATSTATUS(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_FRIENDLIST_MODIFY(CEL::DB_RESULT &rkResult);
extern HRESULT Q_DQT_USER_EVENT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SAVE_CLIENTOPTION(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEL_SKILLSET(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SAVE_SKILLSET(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SAVECHARACTER_MAPINFO(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SAVECHARACTER_EXTERN(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_MISSION_REPORT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SAVE_MISSION_REPORT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_MISSION_RANK(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SAVE_MISSION_RANK(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SAVECHARACTER(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_RECORD_LEVELUP(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_LOAD_COUPONEVENT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_CLEAN_DELETE_CHARACTER(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_CHANGE_DELETED_CHARACTER_NAME(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_OXQUIZ_EVENT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_OXQUIZ_EVENT_STATE(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_OXQUIZ_UPDATE_STATE(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LUCKYSTAR_LOAD_EVENT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LUCKYSTAR_LOAD_EVENT_SUB(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LUCKYSTAR_LOAD_JOINEDUSER(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LUCKYSTAR_UPDATE_EVENT_SUB(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LUCKYSTAR_UPDATE_JOINEDUSER(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LUCKYSTAR_UPDATE_JOINEDUSER_READED(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_MACROCHECKTABLE(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_PVP_TIME(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_PVP_RANK( CEL::DB_RESULT &rkResult );
extern bool Q_DQT_PVP_RANK_UPDATE( CEL::DB_RESULT &rkResult );
extern bool Q_DQT_PVP_RANK(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_PET_BONUSSTATUS(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_PCROOM(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_PCCAFE(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_PCCAFE_ABIL(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_BASE_CHARACTER( CEL::DB_RESULT &rkResult );
extern bool Q_DQT_DEFDYNAMICABILRATE( CEL::DB_RESULT &rkResult );
extern bool Q_DQT_DEFDYNAMICABILRATE_BAG( CEL::DB_RESULT &rkResult );
extern bool Q_DQT_DEFGMCMD(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_DEF_QUEST_RANDOM(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_DEF_SHOP_IN_EMPORIA(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_DEF_SHOP_IN_GAME(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_DEF_SHOP_IN_STOCK(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_DEF_ITEM_RARITY_CONTROL(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_DEF_ITEM_PLUSUP_CONTROL(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_DEF_ITEM_DISASSEMBLE(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_DEF_PVP_REWARD( CEL::DB_RESULT& rkResult );
extern bool Q_DQT_LOAD_DEF_BS_ITEMPOINT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_MAPENTITY(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_MAPEFFECT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_MAP_STONE_CONTROL(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_TACTICS_QUEST_PSEUDO(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_CASH_ITEM_SHOP(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_GEMSTORE2(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_GEMSTORE(const char* pkTBPath);
extern bool Q_DQT_DEF_CASHITEMABILFILTER(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_GAMBLE(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_DEF_MYHOME_DEFAULT_ITEM(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SYNC_LOCAL_LIMITED_ITEM(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SAVE_PENALTY(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_CHECK_PENALTY(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_CREATE_CHARACTER_EVENT_REWARD(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_DEFREALTYDEALER(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_DEF_RARE_OPT_MAGIC(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_DEF_RARE_OPT_SKILL(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_DEF_MIXUPITEM(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_DEF_GAMBLEMACHINE(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_GMCMD_CASHITEMGIFT_INSERT(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_GMCMD_CASHITEMGIFT_DELETE(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_EVENT_ITEM_REWARD(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_TREASURE_CHEST(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_PROCESS_SETPLAYERPLAYTIME(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_UPDATE_SETPLAYERPLAYTIME(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_DEF_MONSTER_GRADE_PROBABILITY(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_LOAD_DEF_SUPER_GROUND(const char* rkTbPath);
extern bool Q_DQT_DEFGEMSTORE(const char* rkTbPath);
extern bool Q_DQT_ADMIN_GM_COPYTHAT(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_UPDATE_USER_JOBSKILL_HISTORYITEM(CEL::DB_RESULT &rkResult);
extern bool Q_DQT_SELECT_MEMBERID(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_SAVE_CHARACTOR_SLOT(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_UPDATE_PREMIUM_CUSTOM_DATA(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_EVENT_STORE(CEL::DB_RESULT& rkResult);
extern bool Q_DQT_LOAD_JUMPINGCHAREVENT(CEL::DB_RESULT& rkResult);
#endif // CONTENTS_CONTENTSSERVER_DATABASE_PGDBPROCESS_H