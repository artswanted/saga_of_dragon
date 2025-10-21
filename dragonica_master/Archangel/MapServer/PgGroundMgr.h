#ifndef MAP_MAPSERVER_MAP_PGGROUNDMGR_H
#define MAP_MAPSERVER_MAP_PGGROUNDMGR_H

#include <map>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "BM/Observer.h"
#include "variant/PgObjectMgr2.h"
#include "Variant/PgMission.h"
#include "PgSmallArea.h"
//#include "PgWayPoint.h"
#include "BM/PgTask.h"
#include "PgTask_Mapserver.h"
#include "PgGroundTrigger.h"
#include "PublicMap/PgWarGround.h"
#include "PublicMap/PgEmporiaGround.h"
#include "PgMissionGround.h"
#include "PgIndunHome.h"
#include "PublicMap/PgBSGround.h"
#include "PublicMap/PgHardCoreDungeon.h"
#include "PgSuperGround.h"
#include "PgExpeditionGround.h"
#include "PgExpeditionLobby.h"
#include "PgStaticEventGround.h"
#include "PgStaticRaceGround.h"
#include "PgConstellationGround.h"
#include "PgCartMissionGround.h"

int const TickInfo_LoopCountMax  = 999;

class PgAlramMissionMgr_Warpper;

typedef struct tagWorldNpc
{
	tagWorldNpc()
	{	
		kName.clear();
		iNpcKID = 0;
		kNpcGuid.Clear();
		kActorName.clear();
		kScriptName.clear();
		kLocation.clear();
		kNpcType = E_NPC_TYPE_FIXED;		
		iParam = 0;
		GiveEffectNo = 0;
	}

	std::string		kName;
	int				iNpcKID;
	BM::GUID		kNpcGuid;
	std::string		kActorName;
	std::string		kScriptName;
	std::string		kLocation;
	ENpcType		kNpcType;
	int				iParam;
	int				GiveEffectNo;
} SWorldNpc;

class PgConstellationMgr
{
public:
	PgConstellationMgr();
	~PgConstellationMgr();

	void Clear();
	TiXmlNode const* ParseXml(TiXmlNode const* pNode
							, CONT_DEFITEM const* pDefItem
							, CONT_DEFMONSTER const* pDefMonster
							, CONT_DEF_QUEST_REWARD const* pDefQuestReward
							, int & rOutGroundNo);

	Constellation::CONT_CONSTELLATION const& GetConstellation() const
	{
		return m_ContEnter;
	}
	Constellation::CONT_CONSTELLATION_ENTRANCE_CASH_ITEM const& GetConstellationCashItem() const
	{
		return m_ContEntranceCashItem;
	}
	Constellation::CONT_CONSTELLATION_BONUS_ITEM const& GetConstellationBonus() const
	{
		return m_ContBonusItem;
	}
private:
	Constellation::CONT_CONSTELLATION_ENTRANCE_CASH_ITEM m_ContEntranceCashItem;
	Constellation::CONT_CONSTELLATION m_ContEnter;
	Constellation::CONT_CONSTELLATION_BONUS_ITEM m_ContBonusItem;	// üũ�� ���ʽ������� �����̳�
};

class PgGroundRscMgr
{
public:
	typedef BM::TObjectPool< PgGroundResource >	GND_RESOURCE_POOL;
	typedef std::map< int, PgGroundResource* > GND_RESOURCE_CONT;	// first : GroundNo
	
	typedef std::map< int, CONT_GTRIGGER* >				CONT_GTRIGGER_MGR;	// first : GroundNo		
	typedef std::map<int, PgAlramMissionMgr_Warpper* >	CONT_ALRAM_MISSION_MGR;// first : alram_type

	typedef std::map< BM::GUID, SWorldNpc > CONT_NPC;
	typedef std::map< int, CONT_NPC > CONT_WORLD_NPC;

public:
	PgGroundRscMgr();
	virtual ~PgGroundRscMgr();

public:
	static bool GetMapBuildData(	SERVER_IDENTITY const &kSI,
									CONT_MAP_CONFIG const &rkStaticData,
									CONT_MAP_CONFIG const &rkMissionData,
									CONT_DEFMAP const *pkDefMap,
									CONT_DEF_MISSION_ROOT const *pkMissionRoot,
									CONT_DEF_MISSION_CANDIDATE const *pkMissionCandi,
									CONT_MISSION_BONUSMAP const *pkMissionBonusMap,
									CONT_DEF_SUPER_GROUND_GROUP const* pkSuperGroundGrp,
									CONT_MAP_BUILD_DATA &rOutAddData,
									bool const bOnlyMyServer=true);

	const CONT_WORLD_NPC& GetWorldNpc() { return m_kContWorldNpc; }

	HRESULT Reserve( CONT_MAP_BUILD_DATA &rHash );
	void Clear();

	HRESULT Locked_AddGroundResource(int const iGroundNo, T_GNDATTR const kAttr, PgAlramMissionMgr_Warpper const * pkAlramMissionMgr );
	HRESULT Locked_GetGroundResource(int const iGroundNo, PgGroundResource const *&rkOut )const;	// �׶��� ���ҽ��� ��ȯ�Ѵ�.

	Constellation::CONT_CONSTELLATION const& GetConstellation() const
	{
		return m_ConstellationMgr.GetConstellation();
	}
	Constellation::CONT_CONSTELLATION_ENTRANCE_CASH_ITEM const& GetConstellationCashItem() const
	{
		return m_ConstellationMgr.GetConstellationCashItem();
	}
	Constellation::CONT_CONSTELLATION_BONUS_ITEM const& GetConstellationBonus() const
	{
		return m_ConstellationMgr.GetConstellationBonus();
	}
protected:
	HRESULT ParseXml( PgGroundResource *pkGndResource, char const *pcXmlPath );
	bool ParseWorldXml( PgGroundResource *pkGndResource, TiXmlElement const *pkElement );

	HRESULT LoadAllNpc();
	TiXmlNode const* ParseAllNpcXml( TiXmlNode const *pkNode, int& rikOutGroundNo );
	bool AddNpc( const SWorldNpc& rkWorldNpc );
	bool AddWorldNpc( const int iGroundNo, const CONT_NPC& kContNpc );

	HRESULT LoadConstellation();

	bool	GetDefaultGsaPath(TiXmlNode const *pkNode,std::string &kPath)const;
	bool	GetGsaPathByID(TiXmlNode const *pkNode, std::string const& kEventID, std::string &kPath)const;
	bool	GetGsaPath(TiXmlNode const *pkNode, std::string const& kEventID, std::string &kPath)const;
	CONT_ALRAM_MISSION_MGR::mapped_type GetAlramMissionMgr( int const iType )const;

	GND_RESOURCE_POOL	m_kGndResourcePool;
	GND_RESOURCE_CONT	m_kGndResourceCon;//�ѹ�����ϸ� ������ �ʾƾ�. �ּҵ� ������ �ʾƾ�.
	CONT_ALRAM_MISSION_MGR	m_kContAlramMissionMgr;

	mutable BM::ACE_RW_Thread_Mutex_Ext m_kRscLock;//m_kGndLock�� �ռ� ������.

private:
	CONT_WORLD_NPC m_kContWorldNpc;
	CONT_NPC m_kContNpc;
	PgConstellationMgr m_ConstellationMgr;
};

class PgGroundManager
{
	typedef BM::TObjectPool< PgGround >				GND_POOL;
	typedef BM::TObjectPool< PgIndun >				GND_POOL_INDUN;
	typedef BM::TObjectPool< PgWarGround >			GND_POOL_WAR;
	typedef BM::TObjectPool< PgMissionGround >		GND_POOL_MISSION;
	typedef BM::TObjectPool< PgEmporiaGround >		GND_POOL_EMPORIA;
	typedef BM::TObjectPool< PgIndunHome >			GND_POOL_HOME;
	typedef BM::TObjectPool< PgBSGround >			GND_POOL_BS;
	typedef BM::TObjectPool< PgHardCoreDungeon >	GND_POOL_HARDCORE;
	typedef BM::TObjectPool< PgSuperGround >		GND_POOL_SUPERGND;
	typedef BM::TObjectPool< PgExpeditionGround >	GND_POOL_EXPEDITIONGND;
	typedef BM::TObjectPool< PgExpeditionLobby >	GND_POOL_EXPEDITIONLOBBY;
	typedef BM::TObjectPool< PgStaticEventGround >	GND_POOL_STATICEVENTGROUND;
	typedef BM::TObjectPool< PgStaticRaceGround >	GND_POOL_STATICRACEGROUND;
	typedef BM::TObjectPool< PgConstellationGround > GND_POOL_CONSTELLATIONGND;
	typedef BM::TObjectPool< PgCartMissionGround > 	 GND_POOL_CARTMISSION;

	typedef std::map<SGroundKey, PgGround*> GND_CONT;//��ü �����̳�
	
	typedef struct tagGroundAddDesc
	{
		tagGroundAddDesc()
			:	iBalance(0)
			,	iWeight(0)
			,	iControlID(0)
			,	iOwnerLv(0)
			,	kAttr(GATTR_DEFAULT)
			,	bIndunPartyDie(false)
			,	pkConstellationMission(NULL)
		{}

		tagGroundAddDesc( SGroundMakeOrder const &kMakeOrder )
			:	kKey( kMakeOrder.kKey )
			,	iBalance( kMakeOrder.byBalance )
			,	iWeight( kMakeOrder.byWeigth )
			,	iControlID(0)
			,	iOwnerLv(kMakeOrder.iOwnerLv)
			,	kAttr(GATTR_DEFAULT)
			,	bIndunPartyDie(kMakeOrder.bIndunPartyDie)
			,	pkConstellationMission(&kMakeOrder.constellationMission)
		{}
		
		T_GNDATTR	kAttr;
		int			iBalance;
		int			iWeight;	// Map�� ����ġ
		int			iControlID;	// Monster ControlID
		int			iOwnerLv;
		bool		bIndunPartyDie;
		SGroundKey	kKey;
		Constellation::SConstellationMission const* pkConstellationMission;
		MUTATOR_SET rkMutatorData; // Mutator data, used only for mission ground normal (F1 mission)
	}SGroundAddDesc;

	typedef std::map<int, SMapPlayTime> GND_PLAYTIME;	// <GroundNo, PlayTimeInfo>

public:
	PgGroundManager();
	virtual ~PgGroundManager();

	void Init(bool bIsPublicMap);

	bool GroundEventNfy(SGroundKey const& rkGroundKey, BM::GUID const &rkCharacterGuid, BM::Stream * const pkPacket);
	void BroadcastAll(BM::Stream const& pkPacket);
	void Clear();

//	void SendAllBalancingGnd();
	bool Reserve(CONT_MAP_BUILD_DATA &rHash);
	HRESULT OrderCreate( SGroundMakeOrder const &kOrder, BM::Stream * const pkPacket );
	bool OrderCreateMission( PgMission *pkMission );
	bool RestartMission(BM::Stream * const pkPacket);

	void RecvGndWrapped(BM::Stream* const pkPacket );
	void Release(const T_GNDATTR kReleaseGndAttr=GATTR_ALL);
	typedef struct tagSTickInfo 
	{
		short int sLoopCount;
		SGroundKey kNextGroundKey;
		tagSTickInfo()
		{
			Init();
		}
		void Init()
		{
			sLoopCount = 0;
			kNextGroundKey.Clear();
		}
	}STickInfo;

	typedef std::vector<STickInfo> TICK_INFO_VEC;

	void _TestReqCreateMissionGround(int const iCount) const;	// Mission ���� �׽�Ʈ �Լ� (�ܺο��� ��� ����) ���� �׽�Ʈ��

public:
	void OnTimer(ETickInterval eInterval);
	void DisplayState();
	int Locked_GetConnectionMapUserCount() const;

public:
	bool ProcessMsg(SEventMessage *pkMsg);

	//void SendToGroundUser(SGroundKey const& rkTrgGndKey, const BM::GUID rkGuid, BM::Stream const& rkPacket, DWORD const dwSendType = E_SENDTYPE_SELF) const;
	//void SendToGroundUser(SGroundKey const& rkTrgGndKey, VEC_GUID const& rkGuidVec, BM::Stream const& rkPacket, DWORD const dwSendType = E_SENDTYPE_SELF) const;

	void ProcessRemoveUser(SERVER_IDENTITY const &kSI);
	void ProcessRemoveUser(BM::GUID const &kCharGuid);

	// 
	void SendAllGround()const;

protected:
	// 
	PgGround *GetGround(SGroundKey const &rkKey);//! ���� ��ȯ�Ѵ�. --> Public���� �̵� ����!
	PgGround *AddGround(SGroundAddDesc const &rkAddDesc, GroundArr& rkOutGndArr, BM::Stream * const pkPacket=NULL );

	PgGround* Create( SGroundAddDesc const&rkAddDesc, BM::Stream * const pkPacket );
	PgGround* CreatePool( T_GNDATTR const kGndAttr );
	PgGround* PickupTickGround(ETickInterval eInterval);	// public �̵� ����(Lock�� �����������, ���� �ű��� ������)

	void OnTimer5s();
	void OnTimer30s();
	void AddPlayTime(SGroundKey const& rkGndKey, SMapPlayTime const& rkPlayTime);
	void WritePlayTimeLog(int const iElapsedTimeSec) const;
	bool IsHaveGround(SGroundKey const &rkKey) const;//�� �������� �����ϴ� �׶��� �ΰ�?

private:
	bool Delete(GND_KEYLIST& rkGndKeyList);
	void DeletePool( PgGround*& pkGnd );

	void SendNfyCreateGround( GroundArr const &kSendGndArr )const;

protected:
	//mutable Loki::Mutex m_kGndLock;// ������û�� �������� ���� ��ƾ���
	mutable BM::ACE_RW_Thread_Mutex_Ext m_kGndLock;
	
	GND_POOL					m_kGndPool;
	GND_POOL_INDUN				m_kGndPoolIndun;
	GND_POOL_WAR				m_kGndPoolWar;
	GND_POOL_MISSION			m_kGndPoolMission;
	GND_POOL_EMPORIA			m_kGndPoolEmporia;
	GND_POOL_HOME				m_kGndPoolHome;
	GND_POOL_BS					m_kGndPoolBS;
	GND_POOL_HARDCORE			m_kGndPoolHardCore;
	GND_POOL_SUPERGND			m_kGndPoolSuperGround;
	GND_POOL_EXPEDITIONGND		m_GndPoolExpeditionGround;
	GND_POOL_EXPEDITIONLOBBY	m_GndPoolExpeditionLobby;
	GND_POOL_STATICEVENTGROUND	m_GndPoolStaticEventGround;
	GND_POOL_STATICRACEGROUND	m_GndPoolStaticRaceGround;
	GND_POOL_CONSTELLATIONGND	m_GndPoolConstellationGround;
	GND_POOL_CARTMISSION		m_GndPoolCartMission;
	
	GND_CONT				m_kStaticMapCon;
	GND_PLAYTIME			m_kPlayTimeInfo;
	TICK_INFO_VEC			m_kTickInfo;
	Loki::Mutex				m_akTickInfoMutex[ETICK_INTERVAL_MAX];

public:
	PgGroundRscMgr			m_kGndRscMgr;
};

#include "PgGroundMgr.inl"

#define g_kGndMgr SINGLETON_STATIC(PgGroundManager)


//
class PgMissionDefMgr : public PgMissionContMgr
{
public:
	PgMissionDefMgr() {};
	virtual ~PgMissionDefMgr() {};

	size_t GetTotalMissionCount(int const iMissionKey) const
	{
		ConPack::const_iterator pack_iter = m_kConPack.find(iMissionKey);
		if(m_kConPack.end() != pack_iter)
		{
			const ConPack::mapped_type& rkMissionPack = (*pack_iter).second;
			return rkMissionPack.TotalMissionCount();
		}
		return 0;
	}
};
#define g_kMissionContMgr SINGLETON_STATIC(PgMissionDefMgr)//Mission Level Def Container

#endif // MAP_MAPSERVER_MAP_PGGROUNDMGR_H