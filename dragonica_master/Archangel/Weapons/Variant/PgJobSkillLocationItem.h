#ifndef WEAPON_VARIANT_JOBSKILL_PGJOBSKILLLOCATIONITEM_H
#define WEAPON_VARIANT_JOBSKILL_PGJOBSKILLLOCATIONITEM_H
//
typedef enum eGatherType : int
{	
	GT_None			= 0,	//
	GT_WoodMachine	= 1,	// 목공선반
	GT_Smelting		= 2,	// 제련기
	GT_Garden		= 3,	// 텃밭
	GT_Fishbowl		= 4,	// 어항
	GT_Jewelry		= 5,	// 보석세공기

	GT_ToolBox		= 10,	// 만능도구

	GT_Manager		=100,	// 관리 기계
}EGatherType;

typedef struct tagJobSkill_LocationItem
{
	tagJobSkill_LocationItem() 
		: iJobGrade(0), iGatherType(0), bEnable(true), bSpot(false), dwSpot_StartTime(0), iSpot_Cycle(0), iSpot_Dration(0)
	{}

	size_t min_size()const
	{
		return 
			sizeof(iJobGrade) + sizeof(iGatherType) + sizeof(bEnable) + sizeof(bSpot) + sizeof(dwSpot_StartTime) + sizeof(iSpot_Cycle) + sizeof(iSpot_Dration);
	}

	int			iJobGrade;
	int			iGatherType;
	bool		bEnable;
	bool		bSpot;
	DWORD		dwSpot_StartTime;
	int			iSpot_Cycle;
	int			iSpot_Dration;

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iJobGrade);
		kPacket.Push(iGatherType);
		kPacket.Push(bEnable);
		kPacket.Push(bSpot);
		kPacket.Push(dwSpot_StartTime);
		kPacket.Push(iSpot_Cycle);
		kPacket.Push(iSpot_Dration);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iJobGrade);
		kPacket.Pop(iGatherType);
		kPacket.Pop(bEnable);
		kPacket.Pop(bSpot);
		kPacket.Pop(dwSpot_StartTime);
		kPacket.Pop(iSpot_Cycle);
		kPacket.Pop(iSpot_Dration);
	}	
}SJobSkill_LocationItem;

typedef std::map<std::string, SJobSkill_LocationItem> CONT_JOBSKILL_LOCATIONINFO;
typedef std::map<BM::GUID, std::string> CONT_JOBSKILL_LOCATIONITEM_RESULT;
//

class PgJobSkillLocationItem
{
public:
	typedef CONT_JOBSKILL_LOCATIONINFO::key_type	KEY_TYPE;
	typedef CONT_JOBSKILL_LOCATIONINFO::mapped_type VALUE_TYPE;

	PgJobSkillLocationItem();
	~PgJobSkillLocationItem();	

	PgJobSkillLocationItem& operator=( PgJobSkillLocationItem const &rhs );

	void Clear();
	bool IsEmpty();
	void WriteToPacket(BM::Stream & kPacket) const;
	void ReadFromPacket(BM::Stream & kPacket);

	void RemoveAll();
	bool RemoveElement(KEY_TYPE const& kTriggerID);

	bool Set(std::string const& kTriggerID, VALUE_TYPE& kValue);
	bool Get(std::string const& kTriggerID, VALUE_TYPE& kOutValue) const;	
	bool GetJobGrade(std::string const& kTriggerID, int& iOutValue) const;
	bool IsEnable(std::string const& kTriggerID) const;
	bool SetEnable(std::string const& kTriggerID, bool bEnable);
	bool GetGatherType(std::string const& kTriggerID, EGatherType& iOutValue) const;
	bool IsSpot(std::string const& kTriggerID) const;
	bool SetSpotTime(std::string const& kTriggerID, DWORD dwTime, int const iCycle, int const iDration);
	void GetAll(CONT_JOBSKILL_LOCATIONINFO& kLocationItem);
	void LocationDrationTime(CONT_JOBSKILL_LOCATIONINFO& kOut);
	bool SetSpotProbability(std::string const& kTriggerID);
	bool GetDefLocationItem(int const iJobGrade, CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type& rkOut);

private:
	CONT_JOBSKILL_LOCATIONINFO m_kContLocationItem;
};


typedef struct tagJobSkillUserInfo
{
	tagJobSkillUserInfo() 
		: dwStartTime(0), dwDrationTime(0), iUseSkillNo(0), iUseToolItemNo(0)
	{}

	size_t min_size()const
	{
		return 
			sizeof(dwStartTime)+
			sizeof(dwDrationTime)+
			sizeof(kGuid)+
			sizeof(iUseSkillNo)+
			sizeof(iUseToolItemNo)+
			sizeof(iExpertness)+
			sizeof(iUseExhaustion);
	}

	DWORD dwStartTime;
	DWORD dwDrationTime;
	BM::GUID kGuid;
	int iUseSkillNo;
	int iUseToolItemNo;
	int iExpertness;
	int iUseExhaustion;

	bool operator == (BM::GUID const& kGuid) const
	{
		return this->kGuid == kGuid;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(dwStartTime);
		kPacket.Push(dwDrationTime);
		kPacket.Push(kGuid);
		kPacket.Push(iUseSkillNo);
		kPacket.Push(iUseToolItemNo);
		kPacket.Push(iExpertness);
		kPacket.Push(iUseExhaustion);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(dwStartTime);
		kPacket.Pop(dwDrationTime);
		kPacket.Pop(kGuid);
		kPacket.Pop(iUseSkillNo);
		kPacket.Pop(iUseToolItemNo);
		kPacket.Pop(iExpertness);
		kPacket.Pop(iUseExhaustion);
	}	
}SJobSkillUserInfo;


class PgJobSkillLocationUser
{
public:
	typedef std::string KEY_TYPE;
	typedef std::map< BM::GUID, SJobSkillUserInfo > VALUE_TYPE;

	// Name, CharGuid...
	typedef std::map< KEY_TYPE, VALUE_TYPE > CONT_JOBSKILL_LOCATIONUSER;
	
public:
	PgJobSkillLocationUser();
	~PgJobSkillLocationUser();

	void Clear();
	bool IsEmpty();
	void RemoveAll();

	void Set(KEY_TYPE const& kTriggerID, BM::GUID const kCharGuid, int const iSkillNo, int const iExpertness, DWORD const dwDrationTime, int const iItemNo, int const iUseExhaustion);
	//bool Get(KEY_TYPE const& kJobSkillNo, SJobSkillUserInfo& rkOut) const;
	bool Get(KEY_TYPE const& kTriggerID, BM::GUID kCharGuid, PgJobSkillLocationUser::VALUE_TYPE::mapped_type& rkOut) const;
	void Del(KEY_TYPE const& kTriggerID, BM::GUID const& kCharGuid);

	void WriteToPacket(BM::Stream & kPacket) const;
	void ReadFromPacket(BM::Stream & kPacket);

	void UserDrationSkillTime(CONT_JOBSKILL_LOCATIONITEM_RESULT& kOut);

private:
	CONT_JOBSKILL_LOCATIONUSER m_kContLocationUser;
};

#endif // WEAPON_VARIANT_JOBSKILL_PGJOBSKILLLOCATIONITEM_H