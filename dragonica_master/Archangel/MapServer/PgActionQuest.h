#ifndef MAP_MAPSERVER_ACTION_ACTION_PGACTIONQUEST_H
#define MAP_MAPSERVER_ACTION_ACTION_PGACTIONQUEST_H

//
namespace PgActionQuestUtil
{
	void AfterBeginCheckQuest(CUnit* pkCaster, int const iQuestID, SGroundKey const& rkGndKey, int const iMissionStageCount);
};

//
class PgCheckQuest 
	:	public PgUtilAction
{
public :
	explicit PgCheckQuest(ContSimpleQuest &rkOutVec);
	virtual ~PgCheckQuest() {};

	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);
private:
	ContSimpleQuest &m_kQuestVector;
};


//
class PgCheckQuestBegin 
	: public PgUtilAction
{
public :
	explicit PgCheckQuestBegin(int const iQuestID);
	virtual ~PgCheckQuestBegin() {};

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

	CLASS_DECLARATION_S_NO_SET(EQuestBeginLimit, Limit);
	CLASS_DECLARATION_S_NO_SET(int, MaxLevelLimit);
private:
	int const m_iQuestID;
};


//
class PgCheckQuestComplete
	: public PgUtilAction
{
public :
	explicit PgCheckQuestComplete(int const iQuestID, CONT_PLAYER_MODIFY_ORDER &rkOrder, SUserQuestState const *pkNextState);
	virtual ~PgCheckQuestComplete(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);
	static bool DoAction(BM::GUID const &rkCharGuid, SUserQuestState const *pkState, PgQuestInfo const *pkQuestInfo, CONT_PLAYER_MODIFY_ORDER &rkOrder);
	
private:
	SUserQuestState const *m_pkNextState;
	CONT_PLAYER_MODIFY_ORDER &m_kOrder;
	int m_iQuestID;
};


//
class PgAction_SetQuestParam
	: public PgUtilAction
{
public:
	explicit PgAction_SetQuestParam(SGroundKey const &rkGndKey, int const iQusetID, int const iParamNo, size_t const iTargetCount);
	virtual ~PgAction_SetQuestParam();

	virtual bool DoAction(CUnit* pkUser, CUnit* Nothing);
	static bool DoAction(BM::GUID const &rkCharGuid, SUserQuestState const *pkState, int const iParamNo, size_t const iParamCount, CONT_PLAYER_MODIFY_ORDER &rkOrder);

private:
	SGroundKey const &m_kGndKey;
	int const m_iQuestID;
	int const m_iParamNo;
	size_t const m_iTargetCount;
};


//
class PgAction_IncQuestParam
	: public PgUtilAction
{
public:
	explicit PgAction_IncQuestParam(SGroundKey const &rkGndKey, int const iQuestID, int const iParamNo, size_t const iIncVal = 1);
	virtual ~PgAction_IncQuestParam();

	virtual bool DoAction(CUnit* pkUser, CUnit* Nothing);
	static inline bool DoAction(PgQuestInfo const *pkQuestInfo, SUserQuestState const *pkState, int const iParamNo, int const iIncVal, CUnit* pkUser, CONT_PLAYER_MODIFY_ORDER &rkOrder);

private:
	SGroundKey const &m_kGndKey;
	int const m_iQuestID;
	int const m_iParamNo;
	size_t const m_iIncVal;
};


class PgAction_PetQuestCheck
	: public PgUtilAction
{
public:
	explicit PgAction_PetQuestCheck(SGroundKey const &rkGndKey, int const iQusetID);
	virtual ~PgAction_PetQuestCheck() {}

	virtual bool DoAction(CUnit* pkUser, CUnit* Nothing);
	//static bool DoAction(BM::GUID const &rkCharGuid, SUserQuestState const *pkState, int const iParamNo, size_t const iParamCount, CONT_PLAYER_MODIFY_ORDER &rkOrder);

private:
	SGroundKey const &m_kGndKey;
	int const m_iQuestID;
//	int const m_iParamNo;
//	size_t const m_iTargetCount;
};


//
class PgAction_MonsterQuestCheck
	: public PgUtilAction
{
public:
	explicit PgAction_MonsterQuestCheck(int const iCurGroundNo, SGroundKey const &rkGndKey, unsigned int iCombo,
		int const iMissionKey, int const iMissionLevel, int const iMssionStageCount, PgGround const *pkGround, CUnit const* pkKiller);
	virtual ~PgAction_MonsterQuestCheck() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	bool CanPartyPass(PgPlayer *pkPC, int iQuestId);

private:
	int const m_iGroundNo;
	int const m_iMissionKey;
	int const m_iMissionLevel;
	int const m_iMissionStageCount;
	SGroundKey const &m_kGndKey;
	unsigned int m_iCombo;
	PgGround const *m_pkGround;
	CUnit const* m_pkKiller;
};


//
class PgAction_GroundQuestCheck
	: public PgUtilAction
{
public:
	explicit PgAction_GroundQuestCheck(int const iCurGroundNo, SGroundKey const &rkGndKey, unsigned int iCombo, unsigned int eState = 0);
	virtual ~PgAction_GroundQuestCheck() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	int const m_iGroundNo;
	SGroundKey const& m_kGndKey;
	unsigned int m_iCombo;
	const unsigned int m_kIndunState;
};

class PgAction_GlobalQuestCheck
	: public PgUtilAction
{
public:
	explicit PgAction_GlobalQuestCheck( int const iCurGoundNo, SGroundKey const &rkGndKey, unsigned int iCombo );
	virtual ~PgAction_GlobalQuestCheck() {};

	virtual bool DoAction( CUnit* pkCaster, CUnit* pkTarget );

private:
	int const m_iGroundNo;
	SGroundKey const &m_kGndKey;
	unsigned int m_iCombo;
};

//
class PgAction_GiveQuestReward
	: public PgUtilAction
{
public:
	explicit PgAction_GiveQuestReward(int const iQuestID, unsigned int const iSelect1, unsigned int const iSelect2, CONT_PLAYER_MODIFY_ORDER &rkOrder);
	virtual ~PgAction_GiveQuestReward() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);

	static void MakeRewardOrder(ContQuestItem const& rkRewardItem, bool const bSelective, unsigned int const iSelect, CONT_ITEM_CREATE_ORDER& rkOrderList);
protected:
	void CreateReward(PgQuestInfo const *pkQuestInfo, PgPlayer *pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder);

private:
	CONT_PLAYER_MODIFY_ORDER &m_kOrder;
	int const m_iQuestID;
	unsigned int const m_iSelect1;
	unsigned int const m_iSelect2;
};


namespace PgAction_QuestMissionUtil
{
	bool ProcessMissionPercent(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer);
	bool ProcessMissionClear(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer, int const iMissionLevel, EMissionRank const eClearRank);
	bool ProcessMissionClearN(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer, int const iMissionLevel, EMissionRank const eClearRank, int const iMssionStgaeCount);
	bool ProcessMissionDefenceClear(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer, size_t const iNowStage);
	bool ProcessQuestMission(int const MissionMode, SGroundKey const& rkGndKey, SMissionKey const& rkMissionKey, PgPlayer* pkPlayer, EMissionRank const eRank, size_t const iMissionStageCount, size_t const iNowStage, bool const bProcessOnlyDefence = false);
};

//
class PgAction_QuestMissionPercent
	: public PgUtilAction
{
public:
	explicit PgAction_QuestMissionPercent(SGroundKey const &rkGndKey, int const iMissionKey);
	virtual ~PgAction_QuestMissionPercent() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);

protected:
	SGroundKey const &m_kGndKey;
	int const m_iMissionKey;
};


//
class PgAction_QuestPvP
	: public PgUtilAction
{
public:
	explicit PgAction_QuestPvP(SGroundKey const &rkGndKey, BYTE const byWinLose);
	virtual ~PgAction_QuestPvP() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);

private:
	SGroundKey const &m_kGndKey;
	BYTE const m_byWinLose;
};


//
class PgAction_QuestRepair
	: public PgUtilAction
{
public:
	explicit PgAction_QuestRepair(SGroundKey const &rkGndKey, PgBase_Item const &rkItem);
	virtual ~PgAction_QuestRepair() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);

private:
	SGroundKey const &m_kGndKey;
	PgBase_Item const &m_kItem;
};


//
class PgAction_QuestUpgrade
	: public PgUtilAction
{
public:
	explicit PgAction_QuestUpgrade(SGroundKey const &rkGndKey, PgBase_Item const &rkItem);
	virtual ~PgAction_QuestUpgrade() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);
	
private:
	SGroundKey const &m_kGndKey;
	PgBase_Item const &m_kItem;
};


//
class PgAction_ChangeClassCheck//������ �ٸ� ����Ʈ�� ������ �Ǵ���
	: public PgUtilAction
{
public:
	explicit PgAction_ChangeClassCheck(SGroundKey const &rkGndKey);
	virtual ~PgAction_ChangeClassCheck() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);
private:
	SGroundKey const &m_kGndKey;
};

typedef std::list< int > CONT_QUEST_EFFECT;
//
class PgAction_QuestDialogEvent
	: public PgUtilAction
{
public:
	explicit PgAction_QuestDialogEvent(SGroundKey const& rkGndKey, BM::GUID const & rkObjectGuid, int const iQuestID, int const iEventNo, int const iDialogID, CONT_PLAYER_MODIFY_ORDER &rkOrder, CONT_QUEST_EFFECT& rkOut,PgGround * const pkGround=NULL);
	virtual ~PgAction_QuestDialogEvent() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNPCUnit);

	static void NfyAddEffectToMap(CUnit* pkUnit, CONT_QUEST_EFFECT const& rkContEffect);

	CLASS_DECLARATION_S_NO_SET(int, ItemResultFailDialogID);
private:
	CONT_PLAYER_MODIFY_ORDER &m_kOrder;
	SGroundKey const &m_kGndKey;
	BM::GUID const &m_kObjectGuid;
	int const m_iQuestID;
	int const m_iEventNo;
	int const m_iDialogID;
	CONT_QUEST_EFFECT& m_kContQuestEffect;
	PgGround * const m_pkGround;
};


//
class PgAction_QuestDeleteItem
	: public PgUtilAction
{
public:
	explicit PgAction_QuestDeleteItem(CONT_DEL_QUEST_ITEM const& rkCont, CONT_PLAYER_MODIFY_ORDER &rkOrder);
	virtual ~PgAction_QuestDeleteItem() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);

	CLASS_DECLARATION_NO_SET(int, m_iQuestID, QuestID);
private:
	CONT_DEL_QUEST_ITEM const& m_kCont;
	CONT_PLAYER_MODIFY_ORDER &m_kOrder;
};


//
class PgAction_AnsQuestDialog
	: public PgUtilAction
{
public:
	explicit PgAction_AnsQuestDialog(SGroundKey const& rkGndKey, BM::GUID const & rkObjectGuid, int const iQuestID, int const iPreDialogID, int const iNextDialogID, int const iSelect1, int const iSelect2,PgGround * const pkGround=NULL);
	virtual ~PgAction_AnsQuestDialog() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkObjectUnit);

protected:
	bool DoAccept(PgQuestInfo const *pkQuestInfo, EQuestState &eState, PgPlayer* pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder, int &iNextDialogID, int &iFailedDialogID);
	bool DoReject(PgQuestInfo const *pkQuestInfo, EQuestState &eState, PgPlayer* pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder, int &iNextDialogID, int &iFailedDialogID);
	bool DoComplete(PgQuestInfo const *pkQuestInfo, EQuestState &eState, PgPlayer* pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder, int &iNextDialogID, int &iFailedDialogID);
	bool DoRejectComplete(PgQuestInfo const *pkQuestInfo, EQuestState &eState, PgPlayer* pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder, int &iNextDialogID, int &iFailedDialogID);
	bool CheckCanDeletePet(PgQuestInfo const *pkQuestInfo, PgPlayer* pkPC);

private:
	SGroundKey const &m_kGndKey;
	BM::GUID const &m_kObjectGuid;
	int const m_iQuestID;
	int const m_iPreDialogID;
	int const m_iNextDialogID;
	int const m_iSelect1;
	int const m_iSelect2;
	PgGround * const m_pkGround;
};


//
class PgAction_QuestKillCount
	: public PgUtilAction
{
public:
	explicit PgAction_QuestKillCount(SGroundKey const& rkGndKey, int const iCurKillCount);
	virtual ~PgAction_QuestKillCount();

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);

private:
	SGroundKey const& m_kGndKey;
	int const m_iCurKillCount;
};

//
class PgAction_QuestMyHome
	: public PgUtilAction
{
public:
	explicit PgAction_QuestMyHome(SGroundKey const& rkGndKey, int const iTotalVisitorCount);
	virtual ~PgAction_QuestMyHome();

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);

private:
	SGroundKey const& m_kGndKey;
	int const m_iTotalVisitorCount;
};

//
class PgAction_QuestAbil
	: public PgUtilAction
{
public:
	explicit PgAction_QuestAbil(SGroundKey const& rkGndKey, int const iAbilType = 0);
	virtual ~PgAction_QuestAbil();

	virtual bool DoAction(CUnit* pkCaster, CUnit* Nothing);
private:
	SGroundKey const& m_kGndKey;
	int const m_iAbilType;
};

//
namespace PgQuestActionUtil
{
	void CheckItemQuestOrder(SGroundKey const &rkGndKey, ContItemNoSet const &rkItemSet, CUnit* pkCaster);
	void CheckItemQuest(ContItemNoSet const &rkItemSet, CUnit* pkCaster, CONT_PLAYER_MODIFY_ORDER &rkOrder);
	void CheckItemQuestPet(ContItemNoSet const &rkItemSet, CUnit* pkCaster, CONT_PLAYER_MODIFY_ORDER &rkOrder, SGroundKey const &rkGndKey);

	bool GetQuestPetFromInv(PgBase_Item& rkOutFirstItem, CUnit* pkCaster, int iLv, int iItemNo, bool bIncludeEquipPos = false);
	bool CheckCanClearPetItem(PgBase_Item& kSrcItem, int iClearLv, int iItemNo);
};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGACTIONQUEST_H