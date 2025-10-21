#ifndef WEAPON_VARIANT_WAR_PGPVPRANKING_H
#define WEAPON_VARIANT_WAR_PGPVPRANKING_H

#include "BM/twrapper.h"
#include "Lohengrin/PacketStruct2.h"
#include "PgPlayer.h"

typedef struct tagPvPRanking
{
	tagPvPRanking(void)
		:	iPoint(0)
		,	iWin(0)
		,	iDraw(0)
		,	iLose(0)
		,	iKill(0i64)
		,	iDeath(0i64)
	{}

	explicit tagPvPRanking( PgPlayer * pkPlayer )
	{
		Set( pkPlayer );
	}

	BM::GUID		kCharacterGuid;
	std::wstring	wstrName;
	SClassKey		kClassKey;
	int				iPoint;
	int				iWin;
	int				iDraw;
	int				iLose;
	__int64			iKill;
	__int64			iDeath;

	double GetWinAvg(void)const
	{
		if ( 0 < iWin )
		{
			if ( 0 < iLose )
			{
				double const fWin = static_cast<double>(iWin);
				double const fTotal = fWin + static_cast<double>(iLose);
				return fWin / fTotal;
			}
			return 1.0;
		}
		return 0.0;
	}

	void Set( PgPlayer *pkPlayer )
	{
		kCharacterGuid = pkPlayer->GetID();
		wstrName = pkPlayer->Name();
		kClassKey = pkPlayer->GetClassKey();

		SPvPReport *pkReport = dynamic_cast<SPvPReport*>(pkPlayer);
		if ( pkReport )
		{
			Set( *pkReport );
		}
	}

	void Set( SPvPReport const &kReport )
	{
		iPoint = kReport.m_iPoint;
		iWin = kReport.m_iRecords[GAME_WIN];
		iDraw = kReport.m_iRecords[GAME_DRAW];
		iLose = kReport.m_iRecords[GAME_LOSE];
		iKill = kReport.m_iKill;
		iDeath = kReport.m_iDeath;
	}

	bool operator < ( tagPvPRanking const &rhs )const
	{
		if ( iPoint == rhs.iPoint )
		{
			double const kWinAvg = GetWinAvg();
			double const kWinAvg_r = rhs.GetWinAvg();
			if ( kWinAvg == kWinAvg_r )
			{
				if ( kClassKey.nLv == rhs.kClassKey.nLv )
				{
					if ( iKill == rhs.iKill )
					{
						if ( iDeath == rhs.iDeath )
						{
							return kCharacterGuid < rhs.kCharacterGuid;
						}
						return iDeath > rhs.iDeath;
					}
					return iKill < rhs.iKill;
				}
				return kClassKey.nLv > rhs.kClassKey.nLv;
			}
			return kWinAvg < kWinAvg_r;
		}
		return iPoint < rhs.iPoint;
	}

	bool operator > ( tagPvPRanking const &rhs )const
	{
		return rhs < *this;
	}

	bool operator == ( tagPvPRanking const &rhs )const
	{
		return 0 == ::memcmp( this, &rhs, sizeof(tagPvPRanking));
	}

	bool operator != ( tagPvPRanking const &rhs )const
	{
		return !(*this == rhs);
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( kCharacterGuid );
		kPacket.Push( wstrName );
		kPacket.Push( kClassKey );
		kPacket.Push( iPoint );
		kPacket.Push( iWin );
		kPacket.Push( iDraw );
		kPacket.Push( iLose );
		kPacket.Push( iKill );
		kPacket.Push( iDeath );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		kPacket.Pop( kCharacterGuid );
		kPacket.Pop( wstrName );
		kPacket.Pop( kClassKey );
		kPacket.Pop( iPoint );
		kPacket.Pop( iWin );
		kPacket.Pop( iDraw );
		kPacket.Pop( iLose );
		kPacket.Pop( iKill );
		return kPacket.Pop( iDeath );
	}

	size_t min_size(void)const
	{
		return	sizeof(kCharacterGuid)
			+	sizeof(size_t)
			+	sizeof(kClassKey)
			+	sizeof(int) * 4
			+	sizeof(__int64) * 2;
	}

}SPvPRanking;
typedef std::vector< SPvPRanking >			VEC_PVP_RANKING;

class PgPvPRanking
{
public:
	static size_t const	ms_iMaxRank = 100;

	typedef	std::map< BM::GUID, SPvPRanking* >		CONT_RANK;

public:
	PgPvPRanking(void);
	~PgPvPRanking(void);

	void Init( VEC_PVP_RANKING &kVecPvPRanking );

	bool Update( SPvPRanking const &kRankingInfo );

	void WriteToPacket( BM::Stream &rkPacket )const;
	bool ReadFromPacket( BM::Stream &rkPacket );

	bool GetRanking( size_t iRank, SPvPRanking &rkOut )const;
	size_t GetRank( BM::GUID const &kCharGuid, SPvPRanking *pOutRanking )const;

	CLASS_DECLARATION( __int64, m_i64LastUpdateTime, LastUpdateTime );
protected:
	size_t GetRank( SPvPRanking *pkRank )const;

protected:
	SPvPRanking		m_kRank[ms_iMaxRank];
	CONT_RANK		m_kContRank;
};

class PgPvPRankingMgr
	:	public TWrapper< PgPvPRanking >
{
public:
	PgPvPRankingMgr(void);
	~PgPvPRankingMgr(void);

	void Init( VEC_PVP_RANKING &kVecPvPRanking );

	bool Update( SPvPRanking const &kRankingInfo );

	bool WriteToPacket( BM::Stream &rkPacket, __int64 const i64LastUpdateTime )const;
	void WriteToPacket( BM::Stream &rkPacket )const;
	bool ReadFromPacket( BM::Stream &rkPacket );

	bool GetRanking( size_t iRank, SPvPRanking &rkOut )const;
	size_t GetRank( BM::GUID const &kCharGuid, SPvPRanking *pOutRanking = NULL )const;
	__int64 GetLastUpdateTime(void)const;

protected:
	bool	m_bInit;
};

#define g_kPvPRankingMgr	SINGLETON_STATIC(PgPvPRankingMgr)

#endif // WEAPON_VARIANT_WAR_PGPVPRANKING_H
