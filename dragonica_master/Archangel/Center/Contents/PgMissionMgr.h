#ifndef CENTER_CENTERSERVER_CONTENTS_PGMISSIONMGR_H
#define CENTER_CENTERSERVER_CONTENTS_PGMISSIONMGR_H

#include "Lohengrin/DBTables.h"
#include "BM/ClassSupport.h"
#include "PgMission.h"

class PgMissionMgr : public PgMissionContMgr
{
public:
	typedef std::multiset<SPlayerRankInfo>				ConPlayRankInfo;
	typedef std::map< BM::GUID, PgMissionContents*>		ConMission;

	typedef std::map< BM::GUID, BM::GUID >				ConReqRegist;// №МјЗ µо·ПА» ЅЕГ»ЗС ір	// first-> CharacterGuid, second->MissionID

	PgMissionMgr();
	virtual ~PgMissionMgr();

	virtual bool Build(const CONT_DEF_MISSION_RESULT &rkResult,
		const CONT_DEF_MISSION_CANDIDATE &rkCandi,
		const CONT_DEF_MISSION_ROOT &rkRoot); //ЕЧАМєн №ЮѕЖј­ №МјЗ Б¤єё єфµщ

	void swap(PgMissionContMgr& rkRight);

	bool RecvPacket(BM::Stream * const pkPacket);

	bool RegistMission(BM::Stream* const pkPacket, bool const bAll=false);
	bool RegistMissionResult( BM::Stream* const pkPacket );
	bool RestartMission(BM::Stream* const pkPacket);
	HRESULT UnRegistMission( BM::GUID const &kMissionID );
	bool EndMission(BM::Stream* const pkPacket);
	void SendToGroundDeletePartyInfo(BM::Stream* const pkPacket);
	void SendToGroundPlayPartyInfo(BM::Stream* const pkPacket);
	//void SendEndMissionResult(ConPlayRankInfo const &kCUserList, BM::GUID const &rkMissionKey);

protected:

	void Recv_PT_N_T_RES_MISSION_RANKING(BM::Stream* const pkPacket);
	
	PgMissionContents* New(){return m_kMissionPool.New();}

	void Delete(PgMissionContents*& pkMission)
	{
		BM::CAutoMutex kLock(m_kMissionMutex);
		if(pkMission)
		{
			pkMission->Clear();
			m_kMissionPool.Delete(pkMission);
		}
	}

	PgMissionContents *GetMission(BM::GUID const &rkKey)
	{
		BM::CAutoMutex kLock(m_kMissionMutex);
		ConMission::iterator mission_itr = m_kConMission.find( rkKey );
		if ( mission_itr != m_kConMission.end() )
		{
			return mission_itr->second;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	bool AddMission(PgMissionContents* pkMission)
	{
		BM::CAutoMutex kLock(m_kMissionMutex);
		if ( !pkMission )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		std::pair<ConMission::iterator, bool> kPair = m_kConMission.insert(std::make_pair(pkMission->GetID(),pkMission));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Mission Key[") << pkMission->GetID().str().c_str() << _T("] Is Bad") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return kPair.second;
		}
		return kPair.second;
	}

	BM::TObjectPool<PgMissionContents>	m_kMissionPool;

	Loki::Mutex		m_kMissionMutex;

	ConMission		m_kConMission;
	ConReqRegist	m_kConReqRegist;
};

class PgMissionRegister
{
public:
	PgMissionRegister( PgMissionMgr::ConReqRegist &kContRegist )
		:	m_kContRegist(kContRegist)
		,	m_kID(BM::GUID::Create())
	{}

	~PgMissionRegister()
	{
		m_kContRegist.erase( m_kReqGuid );
	}

	bool Regist( BM::GUID const &kCharGuid )
	{
		std::pair<PgMissionMgr::ConReqRegist::iterator, bool> kPair = m_kContRegist.insert( std::make_pair( kCharGuid, m_kID ) );
		if ( kPair.second )
		{
			m_kReqGuid = kCharGuid;
			return true;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	void Release(void)
	{
		m_kReqGuid.Clear();
	}

	BM::GUID const &GetID()const{return m_kID;}

private:

	PgMissionMgr::ConReqRegist	&m_kContRegist;
	BM::GUID					m_kReqGuid;
	BM::GUID					m_kID;
};

#define g_kMissionMgr SINGLETON_STATIC(PgMissionMgr)

#endif // CENTER_CENTERSERVER_CONTENTS_PGMISSIONMGR_H