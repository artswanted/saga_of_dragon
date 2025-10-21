#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPRANKINGSCHDULER_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPRANKINGSCHDULER_H

typedef struct tagPvPRankingSeasonScedule
{
	tagPvPRankingSeasonScedule()
		:	iSeason_InitPoint(0)
		,	kCycle_Pass(0)
		,	iCycleDay(0)
		,	fCycle_PointRate(0.0)
		,	iCycle_GivePoint(0)
	{}

	BM::DBTIMESTAMP_EX	kBeginDate;
	int					iSeason_InitPoint;
	char				kCycle_Pass;
	int					iCycleDay;
	double				fCycle_PointRate;
	int					iCycle_GivePoint;

}SPvPRankingSeasonScedule;

class PgPvPRankingSceduler
{
public:
	typedef enum
	{
		E_UPDATE_ERROR			= 0,
		E_UPDATE_NONE			= 1,
		E_UPDATE_NEW_SEASON		= 2,
		E_UPDATE_NEW_CYCLE		= 3,
	};

public:
	PgPvPRankingSceduler(void);
	~PgPvPRankingSceduler(void);

	void SetScedule( SPvPRankingSeasonScedule const &kScedule );
	void SetResult( int const iRet );
	bool CheckUpdate();
	bool IsError(void)const{return E_UPDATE_ERROR == m_iResult;}

protected:
	SPvPRankingSeasonScedule	m_kScedule;
	int							m_iResult;
};

#define g_kPvPRankingSceduler SINGLETON_STATIC(PgPvPRankingSceduler)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPRANKINGSCHDULER_H