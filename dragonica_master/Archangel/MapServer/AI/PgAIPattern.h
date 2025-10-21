#ifndef MAP_MAPSERVER_ACTION_AI_PGAIPATTERN_H
#define MAP_MAPSERVER_ACTION_AI_PGAIPATTERN_H

typedef struct tagSPatternActionInfo
{
	tagSPatternActionInfo()
	{
		Set(EAI_ACTION_NONE, EAI_ACTION_NONE, 0);
	}

	EAIActionType eFrom;
	EAIActionType eTo;
	BYTE byWeight;

	void Set(EAIActionType const eFromAct, EAIActionType const eToAct, BYTE const byWeightValue)
	{
		eFrom = eFromAct;
		eTo = eToAct;
		byWeight = byWeightValue;
	}
	bool operator < (tagSPatternActionInfo &rhs)const
	{
		// Descending 되도록 sorting 하자.
		if( byWeight > rhs.byWeight )	{return true;}
		return false;
	}

} SPatternActionInfo;

class PgAIPattern
{
public:
	PgAIPattern(void);
	~PgAIPattern(void);

	typedef std::vector<SPatternActionInfo*> VECTOR_PATTERN_ACTION_INFO;
	void SetInfo(short int sID, const wchar_t* pwszName);
	void AddTransit(int iFrom, int iTo, BYTE byWeight);
	bool GetNextAction(int const iCurrentAction, PgAIPattern::VECTOR_PATTERN_ACTION_INFO const* &rkOutVector) const;
	bool IsPatternExist(int const iAction) const;
//	static const VECTOR_PATTERN_ACTION_INFO GetNullPatternActionInfo();
	void Release();

protected:
	static bool ChangeSort(SPatternActionInfo* pkFirst, SPatternActionInfo* pkSecond);
private:
	CLASS_DECLARATION_S(short int, ID);
	std::wstring m_kName;
	typedef BM::TObjectPool<SPatternActionInfo> PATTERN_ACTION_INFO_POOL;
	static PATTERN_ACTION_INFO_POOL m_kPatternActionInfoPool;
//	static VECTOR_PATTERN_ACTION_INFO m_kNullPatternActionInfo;
	typedef std::map<int, VECTOR_PATTERN_ACTION_INFO> CONT_AI_ACTION_CHANGE;
	CONT_AI_ACTION_CHANGE m_kChange;
};

#endif // MAP_MAPSERVER_ACTION_AI_PGAIPATTERN_H