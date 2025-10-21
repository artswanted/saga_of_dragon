#ifndef MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGHARDCOREDUNGEON_H
#define MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGHARDCOREDUNGEON_H

#include "PgGround.h"

struct SHardBossGndInfo
{
	SHardBossGndInfo()
		:	i64BeginTime(0i64)
		,	i64EndTime(0i64)
	{}

	SGroundKey	kGndKey;
	BM::GUID	kPartyGuid;
	__int64		i64BeginTime;
	__int64		i64EndTime;
};

class PgHardCoreDungeon
	:	public PgGround
{
public:
	PgHardCoreDungeon(void);
	virtual ~PgHardCoreDungeon(void);

	virtual void Clear();
	bool Clone( PgHardCoreDungeon *pkGround );
	virtual bool Clone( PgGround *pkGround );

	virtual EGroundKind GetKind()const{return GKIND_HARDCORE;}

	EOpeningState Init( int const iID, __int64 const i64EndTime, int const iBossGroundNo, int const iMonsterControlID, bool const bMonsterGen );

	virtual bool IsDeleteTime()const;
	virtual void OnTick5s();

	virtual bool RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket );

	bool ReqJoinBossGround( PgPlayer *pkReqPlayer, SPMO const &kModifyOrder );

protected:
	virtual void WriteToPacket_AddMapLoadComplete( BM::Stream &rkPacket )const;

private:
	int					m_iModeID;
	__int64				m_i64EndTime;
	int					m_iBossGroundNo;
	bool				m_bIsDelete;

	SHardBossGndInfo	m_kHardBossGndInfo;
};

#endif // MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGHARDCOREDUNGEON_H