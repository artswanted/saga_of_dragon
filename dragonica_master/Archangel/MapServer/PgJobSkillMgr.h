#ifndef MAP_MAPSERVER_ACTION_JOBSKILL_PGJOBSKILLMGR_H
#define MAP_MAPSERVER_ACTION_JOBSKILL_PGJOBSKILLMGR_H

#include "Lohengrin/packetstruct.h"
#include "BM/twrapper.h"
#include "PgGroundTrigger.h"
#include "Variant/PgJobSkillLocationItem.h"
#include "Variant/PgProbability.h"
#include "Variant/Global.h"
#include "Lohengrin/PacketType.h"
#include "Variant/PgJobSkillSaveIdx.h"

class PgJobSkillLocationMgrImpl
{
public:

	PgJobSkillLocationMgrImpl(){}
	~PgJobSkillLocationMgrImpl(){}

	static int const SKILL_EXPERTNESS_MAX = 3;

	void Clear();
	bool IsEmpty();
	bool AddElement(GTRIGGER_ID kKey, SJobSkill_LocationItem kInfo);
	bool SetEnable(GTRIGGER_ID const& kKey, bool bEnable);
	bool SetUser(GTRIGGER_ID kKey, PgPlayer* pkPlayer, int const iSkillNo, DWORD& dwOutTurnTime, int const iUseExhaustion);
	void DelUser(GTRIGGER_ID kKey, BM::GUID kCharGuid);
	bool SetSpotProbability(std::string const& kTriggerID);

	bool GetLocationElement(GTRIGGER_ID kKey, SJobSkill_LocationItem& rkOut);
	bool GetDefLocationItem(int const iJobGrade, CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type& rkOut);		
	
	bool GetGatherType(std::string const& kTriggerID, EGatherType& iOutValue) const;
	bool GetJobGrade(std::string const& kTriggerID, int& iOutValue) const;
	bool GetCreateItem(std::string const& kTriggerID, ProbabilityUtil::SGetOneArguments& rkArg, int const iUseToolItemNo, PgBase_Item &kOutItem);	
	void GetAll(CONT_JOBSKILL_LOCATIONINFO& kLocationItem);
	//bool GetUser(GTRIGGER_ID kKey, PgJobSkillLocationUser::VALUE_TYPE& rkOut) const;
	bool GetUser(GTRIGGER_ID kKey, BM::GUID kCharGuid, PgJobSkillLocationUser::VALUE_TYPE::mapped_type& rkOut) const;
	int GetSkillNo(int const iJobGrade);

	bool IsEnable(std::string const& kTriggerID) const;
	bool IsSpot(std::string const& kTriggerID) const;	

	bool WriteToPacketLocation(BM::Stream& rkPacket);
	void SendLocationInfo(BM::Stream& rkPacket, CONT_JOBSKILL_LOCATIONINFO::key_type const& kTriggerID, CONT_JOBSKILL_LOCATIONINFO::mapped_type const& kElement);
	void SendLocationAction(BM::Stream& rkPacket, bool bResult);
	bool CheckSkillExpertness(PgPlayer* pkPlayer, int const iJobGrade, int &iErrMsgNo);
	void LocationDrationTime(CONT_JOBSKILL_LOCATIONINFO& kOut);
	void UserDrationSkillTime(CONT_JOBSKILL_LOCATIONITEM_RESULT& kOut);

	bool GetEventLocationItem(int const iJobGrade, CONT_HAVE_ITEM_DATA_ALL &kContOutItem, CONT_DEF_JOBSKILL_EVENT_LOCATION const *pkContLocation, CONT_DEF_EVENT_REWARD_ITEM_GROUP const *pkEventRewardItemGroup, BM::PgPackedTime const kCurTime);
	void AddEventLocationItem( int const iItemNo, int const iCount, CONT_HAVE_ITEM_DATA_ALL &kContOutItem );

private:	
	PgJobSkillLocationItem		m_kJobSkill_LocationItem;
	PgJobSkillLocationUser		m_kJobSkill_LocationUser;
};

class PgJobSkillLocationMgr : public TWrapper<PgJobSkillLocationMgrImpl>
{
public:
	PgJobSkillLocationMgr(){Clear();}
	~PgJobSkillLocationMgr(){}

	void Clear();
	bool IsEmpty();
	bool AddElement(GTRIGGER_ID kKey, SJobSkill_LocationItem kInfo);
	bool SetEnable(GTRIGGER_ID const& kKey, bool bEnable);
	bool SetUser(GTRIGGER_ID kKey, PgPlayer* pkPlayer, int const iSkillNo, DWORD& dwOutTurnTime, int const iUseExhaustion);
	void DelUser(CUnit* pkUnit, GTRIGGER_ID kKey, BM::GUID kCharGuid);
	bool SetSpotProbability(std::string const& kTriggerID);

	bool GetLocationElement(GTRIGGER_ID kKey, SJobSkill_LocationItem& rkOut);
	bool GetDefLocationItem(int const iJobGrade, CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type& rkOut);		
	
	bool GetGatherType(std::string const& kTriggerID, EGatherType& iOutValue) const;
	bool GetJobGrade(std::string const& kTriggerID, int& iOutValue) const;
	bool GetCreateItem(std::string const& kTriggerID, ProbabilityUtil::SGetOneArguments& rkArg, int const iUseToolItemNo, PgBase_Item &kOutItem);	
	
	bool GetEventLocationItem(int const iJobGrade, CONT_HAVE_ITEM_DATA_ALL &kContOutItem, CONT_DEF_JOBSKILL_EVENT_LOCATION const *pkContLocation, CONT_DEF_EVENT_REWARD_ITEM_GROUP const *pkEventRewardItemGroup, BM::PgPackedTime const kCurTime);

	void GetAll(CONT_JOBSKILL_LOCATIONINFO& kLocationItem);
	//bool GetUser(GTRIGGER_ID kKey, PgJobSkillLocationUser::VALUE_TYPE& rkOut) const;
	bool GetUser(GTRIGGER_ID kKey, BM::GUID kCharGuid, PgJobSkillLocationUser::VALUE_TYPE::mapped_type& rkOut) const;
	int GetSkillNo(int const iJobGrade);

	bool IsEnable(std::string const& kTriggerID) const;
	bool IsSpot(std::string const& kTriggerID) const;	

	bool WriteToPacketLocation(BM::Stream& rkPacket);
	void SendLocationInfo(BM::Stream& rkPacket, CONT_JOBSKILL_LOCATIONINFO::key_type const& kTriggerID, CONT_JOBSKILL_LOCATIONINFO::mapped_type const& kElement);
	void SendLocationAction(BM::Stream& rkPacket, bool bResult);
	bool CheckSkillExpertness(PgPlayer* pkPlayer, int const iJobGrade, int &iErrMsgNo);
	void LocationDrationTime(CONT_JOBSKILL_LOCATIONINFO& kOut);
	void UserDrationSkillTime(CONT_JOBSKILL_LOCATIONITEM_RESULT& kOut);
};

#endif //MAP_MAPSERVER_ACTION_JOBSKILL_PGJOBSKILLMGR_H