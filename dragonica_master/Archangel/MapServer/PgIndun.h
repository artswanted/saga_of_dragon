#ifndef MAP_MAPSERVER_MAP_GROUND_PGINDUN_H
#define MAP_MAPSERVER_MAP_GROUND_PGINDUN_H

#include "Lohengrin/PacketStruct4Map.h"
#include "PgGround.h"
#include "Variant/PgConstellation.h"
#include "PgConstellationResult.h"

typedef struct tagMapWaitUser
{
	tagMapWaitUser( const BM::GUID kCharacterGuid, const EOpeningState _kStep=E_OPENING_NONE,unsigned char per=0 )
		:	kCharGuid( kCharacterGuid )
		,	ucPer( per )
		,	kStep( _kStep )
	{}

	bool operator<(const tagMapWaitUser& rhs)const
	{
		return kCharGuid < rhs.kCharGuid;
	}

	bool operator<(BM::GUID const & rkCharGuid)const
	{
		return kCharGuid < rkCharGuid;
	}

	bool operator==( const tagMapWaitUser& kPair ) const
	{
		return kCharGuid == kPair.kCharGuid;
	}

	bool operator==( BM::GUID const & rkCharGuid )const
	{
		return kCharGuid == rkCharGuid;
	}

	const BM::GUID kCharGuid;
	unsigned char ucPer;
	EOpeningState kStep;
} SWaitUser;

typedef struct tagOwnerGroundInfo
{
	tagOwnerGroundInfo()
	{}

	SGroundKey	kOwnerGndKey;
	__int64		i64EndTime;
}SOwnerGroundInfo;

class PgIndun 
	: public PgGround
{
public:

	static DWORD const md_dwMaxOpenWaitingTime		= 1000*60*10;	//10분
	static DWORD const ms_dwMaxWaitUserWaitngTime	= 1000*60*3;	//맵로딩 기다려 주는 시간 : 최대3분까지만 기다려 준다)
	typedef std::set<SWaitUser>						ConWaitUser;

public:
	PgIndun();
	virtual ~PgIndun();

	virtual EOpeningState Init( int const iMonsterControlID=0, bool const bMonsterGen=true );
	virtual void Clear();
	bool Clone( PgIndun* pkIndun );
	virtual bool Clone( PgGround* pkGround );

	// Ground Manager의 OnTimer에서 호출(리턴값이 true이면 그라운드가 삭제된다.)
	virtual bool IsDeleteTime()const;

	virtual void OnTick1s();
	virtual void OnTick5s();

	virtual EGroundKind GetKind()const{	return GKIND_INDUN;	}
	virtual bool ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall=false, bool const bSendArea=true );

	virtual EIndunState GetState()const{return m_eState;}
	virtual int GetGroundNo()const { return GroundKey().GroundNo();}; //현재 그라운드 번호
	virtual SRecentInfo const& GetRecentInfo()const { return m_kRecentInfo; }

public:
	virtual void SetState( EIndunState const eState, bool bAutoChange=false, bool bChangeOnlyState=false);

protected:
	virtual void RecvUnitDie(CUnit *pkUnit);
	virtual HRESULT SetUnitDropItem(CUnit *pkOwner, CUnit *pkDroper, PgLogCont &kLogCont );
	virtual bool RecvRecentMapMove( PgPlayer *pkUser );	// 리센트맵 위치로 돌려 보내라!
	bool UpdateIndunRecentMap(PgPlayer * pkUser);	// 인던에서 나갈때 조건에 따라 돌아갈 리센트 맵의 위치를 변경
	void AllPlayerRegen(DWORD const dwSendFlag=E_SENDTYPE_BROADCAST_GROUND);
	CUnit * GetOwnerUser()const;
	CUnit * GetPartyMasterUser()const;
	bool ChangeOwner(CUnit * pkOwner);

	void SendToParty_ConstellationMission();

protected:
	virtual bool VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy );
	virtual bool RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket );
	virtual bool RecvGndWrapped_ItemPacket(unsigned short usType, BM::Stream* const pkPacket );

	// User Loading에 관한
	virtual void InitWaitUser( VEC_GUID& rkCharGuidList );
	virtual bool AddWaitUser( BM::GUID const & rkCharGuid );
	virtual HRESULT	ReleaseWaitUser( PgPlayer *pkUser );
	virtual bool IsAllUserSameStep(EOpeningState const eState=E_OPENING_NONE)const;

	virtual bool RecvMapMove( UNIT_PTR_ARRAY &rkUnitArray, SReqMapMove_MT& rkRMM, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER const &kModifyOrder );
	virtual void SendMapLoadComplete( PgPlayer *pkUser );
	void MapLoadComplete();

	void SetAutoNextState( DWORD const dwAutoNextStateTime ){m_dwAutoStateRemainTime = dwAutoNextStateTime;}
	void UpdateAutoNextState(DWORD const dwNow, DWORD const dwElapsedTime);
	
	// Result Item 전송
	void SendResultItem( SNfyResultItemList& rkResultItem );

	// Death Delay Time
	virtual DWORD GetDeathDelayTime()const{	return 5000;	}
	virtual DWORD GetResultWaitTime();

	virtual bool IsMonsterTickOK() const {return m_eState==INDUN_STATE_PLAY;}
	static void SetUnitAbil(UNIT_PTR_ARRAY &rkUnitArray, WORD const wType, int const iValue);

	virtual bool IsAlramMission(void)const{return ((NULL != m_pkAlramMissionMgr) && (INDUN_STATE_PLAY == m_eState));}

	// Opening 연출
private:
	// 이 함수는 특정 예외 상황에서 호출되는 함수이다.(잘모르면 함부로 건들지 말자)
	void ClearManualOpeningMonster(UNIT_PTR_ARRAY& rkUnitArray);

	CLASS_DECLARATION( DWORD, m_dwStartTime, StartTime );	// 실제 인던의 상태가 Play가 되기 시작한 시간
	void CommitIndunEndLog(PgPlayer const *player) const;

public:
	EOpeningState SummonNPC_Generate(PgGenPoint_Object& rkGenPoint, int const iLevel);
	EOpeningState SummonNPC_Generate(PgGenPoint_Object& rkGenPoint, PgGenPoint_Object::OwnGroupCont const& kCont, int const iLevel, CUnit * pkCaller);

protected:
	ConWaitUser			m_kWaitUserList;
	EIndunState			m_eState;

	DWORD				m_dwAutoStateRemainTime;
	EOpeningState		m_eOpening;

	SOwnerGroundInfo	m_kOwnerGndInfo;
	BM::GUID			m_kOwnerGuid;
	SRecentInfo			m_kRecentInfo;
// 특정 옵션
public:

	virtual bool IsGroundEffect()const{return m_bUseGroundEffect;}
	virtual bool IsUseItem()const{return m_bUseItem;}

	virtual void SetResultMoveMapNum(int iNum) { m_iResultMoveMapNum = iNum; }
	virtual int	 GetResultMoveMapNum() { return m_iResultMoveMapNum; }

protected:
	bool			m_bUseGroundEffect;
	bool			m_bUseItem;

	int				m_iResultMoveMapNum;

	PgConstellationResult m_kConstellationResult;
	DWORD m_FailTime; // 인던 실패한 순간의 시간
};

#include "PgIndun.inl"

#endif // MAP_MAPSERVER_MAP_GROUND_PGINDUN_H