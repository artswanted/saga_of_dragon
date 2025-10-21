#ifndef MAP_MAPSERVER_ACTION_AI_PGSKILLCHOOSER_H
#define MAP_MAPSERVER_ACTION_AI_PGSKILLCHOOSER_H

#define MAX_AI_SKILL_NUM 10
// 각 Monster 마다의 Skill+Weight 값을 caching 한다.

class PgGround;

class PgAISkillChooser
{
	typedef struct tagSSkillWeight
	{
		tagSSkillWeight()
		{
			iSkill = 0;
			iWeight = 0;
		}

		int iSkill;
		int iWeight;

		bool operator < (const tagSSkillWeight& rhs)
		{
			// Descending sorting 이므로 return 값을 반대로 하자
			return (iWeight <= rhs.iWeight) ? false : true;
		}
	} SSkillWeight;
	typedef std::vector<SSkillWeight*> List_SkillWeight;	// List로 했었는데, operator - 를 overloading 해야 하나 보다 ㅡㅡ;

	typedef struct
	{
		int iWeightTotal;
		List_SkillWeight kList;
	} SkillWeightInfo;

public:
	PgAISkillChooser();
	~PgAISkillChooser();

	bool Init();
	void Clear();

	int GetAvailableSkill(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SActArg* pkActArg);
	int GetReservableSkill(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SActArg* pkActArg);
	static bool List_SkillWeight_Sort(SSkillWeight* pkFirst, SSkillWeight* pkSecond);

protected:
	typedef std::map<TBL_PAIR_KEY_INT, SkillWeightInfo*> ContAISkillWeightInfo;	// < <ClassID,Level>, SkillWeightInfo>
	void Clear(PgAISkillChooser::SkillWeightInfo* pkSkillWeightInfo);
	const PgAISkillChooser::SkillWeightInfo* Get(CUnit* pkUnit);
	const PgAISkillChooser::SkillWeightInfo* ReadUnitSkillWeight(CUnit* pkUnit, ContAISkillWeightInfo::key_type const& kInfoKey);
	//const PgAISkillChooser::SkillWeightInfo* ReadNewMonsterInfo(CUnit* pkUnit);
	//const PgAISkillChooser::SkillWeightInfo* ReadNewClassInfo(CUnit* pkUnit);

	const PgAISkillChooser::SkillWeightInfo* GetSkillGroup(int const iSkillGroupNo);
	//const PgAISkillChooser::SkillWeightInfo* ReadNewInfo(const CSkillDef* pkSkillDef);
	const PgAISkillChooser::SkillWeightInfo* ReadNewInfo(const int iKey, ContAISkillWeightInfo& rkContainer, const CAbilObject* pkAbilObj);
	const PgAISkillChooser::SkillWeightInfo* ReadNewInfo(const ContAISkillWeightInfo::key_type& iKey, ContAISkillWeightInfo& rkContainer, const CAbilObject* pkAbilObj);

	// SkillChose function
	int GetAvailableSkill_Weight(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo, SActArg* pkActArg);
	int GetReservableSkill_Weight(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo, SActArg* pkActArg);
	int GetAvailableSkill_Random(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo, SActArg* pkActArg);
	int GetReservableSkill_Random(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo, SActArg* pkActArg);
	int GetAvailableSkill_Sequence(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo, SActArg* pkActArg);
	int GetReservableSkill_Sequence(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo, SActArg* pkActArg);

private:
	typedef BM::TObjectPool<SSkillWeight> SkillWeightPool;
	SkillWeightPool m_kSkillWeightPool;

	typedef BM::TObjectPool<SkillWeightInfo> SkillWeightInfoPool;
	SkillWeightInfoPool m_kSkillWeightInfoPool;
	ContAISkillWeightInfo m_kUnitSkillInfo;
	ContAISkillWeightInfo m_kSkillGroupInfo;

	bool CheckLockSkill(CUnit const* pkUnit, int const iSkillNo)const;
	bool CheckUseProjectile(CUnit const* pkUnit, int const iSkillNo)const;
	bool CheckIsAllySkill(CUnit const* pkUnit, PgGround * pkGround)const;
	bool CheckIsInClass(CUnit const* pkUnit, int const iSkillNo, PgGround * pkGround)const;
	bool Check(CUnit const* pkUnit, int const iSkillNo, PgGround * pkGround)const;
	bool CheckSkillHPLimit(CUnit const* pkUnit, CSkillDef const* pkDef);

	mutable Loki::Mutex m_kMutex;
};

#define g_kAISkillChooser SINGLETON_STATIC(PgAISkillChooser)

#endif // MAP_MAPSERVER_ACTION_AI_PGSKILLCHOOSER_H