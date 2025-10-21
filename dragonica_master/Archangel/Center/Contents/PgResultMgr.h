#ifndef CENTER_CENTERSERVER_CONTENTS_PGRESULTMGR_H
#define CENTER_CENTERSERVER_CONTENTS_PGRESULTMGR_H

#include "PgInstanceDungeonPlayInfo.h"

class PgResultMgr
{
public:
	typedef BM::TObjectPool< PgInstanceDungeonPlayInfo >			ConResultPool;
	typedef std::map< SGroundKey, PgInstanceDungeonPlayInfo* >		ConResult;
	typedef std::queue<PgInstanceDungeonPlayInfo*>					ConIndunResultTickQueue;
	typedef BM::TObjectPool< PgExpeditionIndunPlayInfo >			ContexpeditionResultPool;
	typedef std::map< SGroundKey, PgExpeditionIndunPlayInfo* >		ContExpeditionResult;

	PgResultMgr();
	virtual ~PgResultMgr();

	void Clear();

	void ProcessMsg(SEventMessage *pkMsg);

	bool EraseResult( SGroundKey const &rkKey );

	bool AddPlayer( SGroundKey const &rkKey, VEC_JOININDUN const& rkPlayerList );
	bool DeletePlayer( SGroundKey const &rkKey, BM::GUID const &rkCharGuid );

	bool Start( SGroundKey const &rkKey, VEC_RESULTINDUN const& rkResultList );
	bool SetItem( SGroundKey const &rkKey, const SNfyResultItemList& rkReusltItem );
//	bool SetBonus( SGroundKey const &rkKey, SConResultBonus& rkBonus );
	bool SelectBox( SGroundKey const &rkKey, BM::GUID const &rkCharGuid, int const iSelect );

	void Tick( DWORD const dwCurTime );
	void ExpeditionTick(DWORD const CurTime);

	void BroadCast( SGroundKey const &rkKey, BM::Stream const& rkPacket );

	bool ExpeditionStart( SGroundKey const & Key, VEC_RESULTINDUN const & ResultList);
	bool SetExpeditionResultItem( SGroundKey const & Key, BM::Stream & Pakcet );
	bool AddExpeditionPlayer( SGroundKey const & Key, VEC_JOININDUN const & PlayerList);
	bool DeleteExpeditionPlayer(SGroundKey const & Key, BM::GUID const & CharGuid);

	bool TenderItem(SGroundKey const & Key, BM::GUID const & CharGuid);
	bool GiveupItem(SGroundKey const & Key, BM::GUID const & CharGuid);

	bool ExpeditionClose(SGroundKey GndKey);

protected:

	PgInstanceDungeonPlayInfo* GetInfo( SGroundKey const &rkKey );
	PgExpeditionIndunPlayInfo* GetExpeditionInfo( SGroundKey const & key);

private:
	Loki::Mutex					m_kMutex;

	ConResultPool				m_kPool;
	ConResult					m_kResult;
	ConIndunResultTickQueue		m_kTick;

	ContexpeditionResultPool	m_ExpeditionPool;
	ContExpeditionResult		m_ExpeditionResult; 
};

inline PgInstanceDungeonPlayInfo* PgResultMgr::GetInfo( SGroundKey const &rkKey )
{
	ConResult::iterator itr = m_kResult.find( rkKey );
	if ( itr == m_kResult.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	return itr->second;
}

inline PgExpeditionIndunPlayInfo* PgResultMgr::GetExpeditionInfo( SGroundKey const & Key)
{
	ContExpeditionResult::iterator iter = m_ExpeditionResult.find(Key);
	if( iter == m_ExpeditionResult.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	return iter->second;
}

#define g_kResultMgr SINGLETON_STATIC(PgResultMgr)

#endif // CENTER_CENTERSERVER_CONTENTS_PGRESULTMGR_H