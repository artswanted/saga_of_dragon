#ifndef MAP_MAPSERVER_ACTION_ACTION_PGREQUEST_H
#define MAP_MAPSERVER_ACTION_ACTION_PGREQUEST_H

//#include "cel/common.h"
#include "Variant/PgMission.h"

class PgGround;

class PgRequest
{
public:
	explicit PgRequest(unsigned short const _kPacketType)
		:	m_kPacket(_kPacketType)
	{}

	virtual ~PgRequest(){}
	virtual bool DoAction(PgPlayer *pkPlayer) = 0;

protected:
	virtual bool Send()const;

	BM::Stream	m_kPacket;


};

class PgRequest_MissionInfo
	:	public PgRequest
{
public:
	explicit PgRequest_MissionInfo(int const iMissionKey, SGroundKey const& _rkGndkey, int rkType = 0);
	virtual ~PgRequest_MissionInfo(){}

	virtual bool DoAction(PgPlayer *pkPlayer);

protected:
	int const m_iMissionKey;
	int const m_iType;
};

class PgRequest_MissionJoin
	:	public PgRequest
{
public:
	explicit PgRequest_MissionJoin(SMissionKey const& rkMissionKey, PgGround const* pkGround, int const rkType, BM::Stream * const pkPacket);
	virtual ~PgRequest_MissionJoin(){}

	virtual bool DoAction(PgPlayer *pkPlayer);
private:
	SMissionKey m_MissionKey;
protected:
	int const m_iType;
	Constellation::SConstellationMission m_kConstellationMission;
	BM::Stream const m_pkAddonPacket;
};

class PgRequest_MissionReStart
	:	public PgRequest
{
public:
	explicit PgRequest_MissionReStart(BM::GUID const & _kMissionID, SMissionKey const& _kReMissionKey, BM::Stream * const pkPacket);
	virtual ~PgRequest_MissionReStart(){}

	virtual bool DoAction(PgPlayer *pkPlayer);
private:
	SMissionKey m_MissionKey;
protected:
	BM::Stream const m_pkAddonPacket;
};

class PgRequest_Notice
	:	public PgRequest
{
public:
	explicit PgRequest_Notice( E_NOTICE_TYPE kType=NOTICE_ALL );
	virtual ~PgRequest_Notice(){}

	virtual bool DoAction(PgPlayer *pkPlayer){return false;}
	virtual bool DoAction(BM::Stream &kPacket);
	void Add( BM::GUID const &kGuid );

protected:
	virtual bool Send(BM::Stream &kPacket);

protected:
	E_NOTICE_TYPE	m_kType;
	VEC_GUID		m_kGuidList;
};

class PgRequest_CheckPenalty
	:	public PgRequest
{
public:
	explicit PgRequest_CheckPenalty( SGroundKey const &kGndkey, WORD const wType, BM::Stream * const pkPacket );
	virtual ~PgRequest_CheckPenalty(void){}

	virtual bool DoAction(PgPlayer *pkPlayer);

protected:
	virtual bool Send()const;

	WORD const		m_kType;
	BM::Stream*	m_pkPacket;
};

namespace PgRequestUtil
{
	bool GetRecommendMissionMap(PgPlayer * pPlayer, SMissionKey const & MissionKey, ConStage & RecommendMissionMap);
}

#endif // MAP_MAPSERVER_ACTION_ACTION_PGREQUEST_H