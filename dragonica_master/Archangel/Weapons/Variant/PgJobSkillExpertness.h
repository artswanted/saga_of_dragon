#ifndef WEAPON_VARIANT_JOBSKILL_PGJOBSKILLEXPERTNESS_H
#define WEAPON_VARIANT_JOBSKILL_PGJOBSKILLEXPERTNESS_H

class PgPremiumMgr;

class PgJobSkillExpertness
{// 직업 스킬 숙련도
public:
	typedef int KEY_TYPE;
	typedef int VALUE_TYPE;
	typedef std::map< KEY_TYPE, VALUE_TYPE > CONT_EXPERTNESS;	// 숙련도 컨테이너
	
public:
	PgJobSkillExpertness();
	~PgJobSkillExpertness();

	void Init(CEL::DB_DATA_ARRAY::const_iterator& itor, CEL::DB_RESULT_COUNT::const_iterator& count_itor, PgPremiumMgr const& rkPremium);
	
	void Set(KEY_TYPE const& kJobSkillNo, VALUE_TYPE const& kValue);
	bool Get(KEY_TYPE const& kJobSkillNo, PgJobSkillExpertness::VALUE_TYPE& rkOut) const;
	int Get(KEY_TYPE const& kJobSkillNo) const;
	bool IsHave(KEY_TYPE const& kJobSkillNo) const;
	CONT_EXPERTNESS GetAllSkillExpertness() const;
		
	bool Remove(KEY_TYPE const& kJobSkillNo);
	void RemoveAll();
	void Clear();

	void WriteToPacket(BM::Stream & kPacket) const;
	void ReadFromPacket(BM::Stream & kPacket);
	void CopyTo(PgJobSkillExpertness& rhs) const;

	CLASS_DECLARATION(int, m_iCurExhaustion, CurExhaustion);
	CLASS_DECLARATION(int, m_iCurBlessPoint, CurBlessPoint);
	CLASS_DECLARATION(BM::DBTIMESTAMP_EX, m_kLastResetExhaustionTime, LastResetExhaustionTime);
	CLASS_DECLARATION(BM::DBTIMESTAMP_EX, m_kLastResetBlessPointTime, LastResetBlessPointTime);
private:
	CONT_EXPERTNESS m_kContExpertness;
};


namespace JobSkillUtil
{
	//
	bool IsJobSkill(int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill);
	bool IsJobSkill(int const iSkillNo);

	//
	bool GetType(int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, int& rkOutGatherType, EJobSkillType& reOutJobSkillType); // 채집 종류, 채집 스킬 종류 둘다 얻기
	int GetGatherType(int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill); // 채집 종류 만
	EJobSkillType GetJobSkillType(int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill); // 채집 스킬 종류 만
	int GetJobSkillNo(int const eGatherType, EJobSkillType const eSkillType, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill);// 채집타입/스킬타입으로 스킬번호 얻기

	//
	bool CheckJobSkillExpertness(PgPlayer& rkCaster, int const iNeedSkill, int const iNeedExpertness);
};

namespace JobSkillExpertnessUtil
{
	//
	bool Set(PgJobSkillExpertness& kExpertness, PgJobSkillExpertness::KEY_TYPE const& kJobSkillNo, PgJobSkillExpertness::VALUE_TYPE const& kSetVal);
	bool Inc(int const iCharLevel, PgJobSkillExpertness& kExpertness, PgJobSkillExpertness::KEY_TYPE const& kJobSkillNo, PgJobSkillExpertness::VALUE_TYPE const& kIncVal);
	bool Del(PgJobSkillExpertness& kExpertness, PgJobSkillExpertness::KEY_TYPE const& kJobSkillNo);

	//
	bool IsCanResetExhaustion(PgJobSkillExpertness& kExpertness);
	bool AddExhaustion(PgPremiumMgr const& rkPremium, PgJobSkillExpertness& kExpertness, int const iAddExhaustion);
	bool AddBlessPoint(PgPremiumMgr const& rkPremium, PgJobSkillExpertness& kExpertness, int const iAddBlessPoint);
	bool ResetExhaustion(PgPremiumMgr const& rkPremium, PgJobSkillExpertness& kExpertness, __int64 const iForceResetMinute = 0);
	bool InitExhaustion(PgJobSkillExpertness& kExpertness);
	bool InitBlessPoint(PgJobSkillExpertness& kExpertness);

	//
	bool Update(DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkItemChange, BM::Stream& rkAddonData, PgPlayer& rkPlayer); // Recv From Contents Server(Map / Client)

	// 숙련도 구간 찾는 함수
	bool FindExpertnessArea(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness, CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type& rkOut);
	bool FindExpertnessArea(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type& rkOut);
	CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type FindExpertnessArea(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);
	// 다음 숙련도 구간 찾는 함수
	int FindNextExpertness_Min(int const iSkillNo, int const iExpertness);
	bool FindExpertnessArea(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type& rkOut);
	//
	int GetBasicTurnTime(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);
	int GetBasicTurnTime(int const iSkillNo, int const iExpertness);

	//
	int GetExpertnessUp(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);

	//
	int GetMaxExpertness(int const iCharLevel, int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);
	int GetMaxExpertness(int const iCharLevel, int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill);
	int GetMaxExpertness(int const iCharLevel, int const iSkillNo, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);
	int GetMaxExpertness(int const iCharLevel, int const iSkillNo);
	int GetBiggestMaxExpertness(int const iSkillNo);

	//
	int GetUseExhaustion(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);
	int GetUseExhaustion(int const iSkillNo, int const iExpertness);
	int GetMaxExhaustion(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);
	int GetBiggestMaxExhaustion(PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness, EJobSkillType const eFilterType = JST_NONE);
	int GetBiggestMaxExhaustion(PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness, EJobSkillType const eSkillType = JST_1ST_MAIN);

	int GetUseDuration(int const iSkillNo, int const iExpertness, int const iAddRate);

	//
	int	GetMaxExhaustion_1ST(PgPremiumMgr const& rkPremiumMgr, PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness);
	int GetMaxExhaustion_1ST(PgPremiumMgr const& rkPremiumMgr, PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);
	int GetMaxExhaustion_2ND(PgPremiumMgr const& rkPremiumMgr, PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness);
	int GetMaxExhaustion_2ND(PgPremiumMgr const& rkPremiumMgr, PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness);

};

#endif // WEAPON_VARIANT_JOBSKILL_PGJOBSKILLEXPERTNESS_H