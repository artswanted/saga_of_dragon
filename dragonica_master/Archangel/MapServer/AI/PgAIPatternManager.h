#ifndef MAP_MAPSERVER_ACTION_AI_PGAIPATTERNMANAGER_H
#define MAP_MAPSERVER_ACTION_AI_PGAIPATTERNMANAGER_H

#include "PgAIPattern.h"

class PgAIPatternManager
{
public:
	PgAIPatternManager(void);
	~PgAIPatternManager(void);

	bool Init();
	bool Add(short int sID, const wchar_t* pwszName);
	bool AddActTransit(short int sID, int iFromAct, int iToAct, BYTE byWeight);
	bool GetNextAction(short int sPattern, int iCurrentAction, PgAIPattern::VECTOR_PATTERN_ACTION_INFO const* &pkOutVector)const;

	void Release();
	
	bool IsPatternExist(short int const iAI_Type, int const iAction) const;

protected:
	bool LoadAIPattern(char const* szPath);
	PgAIPattern* GetPattern(short int sID);
	PgAIPattern const* GetPattern(short int sID) const;
	
private:
	typedef BM::TObjectPool<PgAIPattern> AIPATTERN_POOL;
	typedef std::map<short int, PgAIPattern*> CONT_AI_PATTERN;
	
	static AIPATTERN_POOL m_kPatternPool;
	CONT_AI_PATTERN m_kPattern;
};

#define g_kPatternMng SINGLETON_STATIC(PgAIPatternManager)

#endif // MAP_MAPSERVER_ACTION_AI_PGAIPATTERNMANAGER_H