#ifndef MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGWARGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGWARGROUND_H

#include "PgIndun.h"
#include "PublicMap/PgPvPSetting.h"

class PgWarMode;
class PgGTrigger_KingOfHill;

typedef struct tagInsItemDropInfo
{
	tagInsItemDropInfo()
	{
		DroperGuid.Clear();
		DropTime = 0;
		Team = TEAM_NONE;
		bDelete = false;
	}

	tagInsItemDropInfo( tagInsItemDropInfo const & rhs )
	{
		DroperGuid = rhs.DroperGuid;
		DropTime = rhs.DropTime;
		Team = rhs.Team;
		bDelete = rhs.bDelete;
	}

	BM::GUID DroperGuid;
	DWORD DropTime;
	ETeam Team;
	bool bDelete;
	
} SInsItemDropInfo;

typedef std::map<BM::GUID, SInsItemDropInfo> CONT_INSITEMDROPINFO;

class PgWarGround
	:	public PgIndun
{
public:
	PgWarGround();
	virtual ~PgWarGround();

	virtual EOpeningState Init( int const iMonsterControlID=0, bool const bMonsterGen=true );
	virtual void Clear();

	bool Clone( PgWarGround* pkIndun );
	virtual bool Clone( PgGround* pkGround );

	// Ground Manager의 OnTimer에서 호출(리턴값이 true이면 그라운드가 삭제된다.)
	virtual bool IsDeleteTime()const;

	virtual EGroundKind GetKind()const{	return GKIND_WAR;	}
	virtual bool ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall=false, bool const bSendArea=true );

	virtual void OnTick1s();

protected:
//NetWork
	virtual void VUpdate( BM::CSubject< BM::Stream* > * const pChangedSubject, BM::Stream* iNotifyCause );
	virtual bool VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy );
	virtual bool RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket );
	virtual void RecvUnitDie(CUnit *pkUnit);
	virtual bool RecvGndWrapped_ItemPacket(unsigned short usType, BM::Stream* const pkPacket );

	virtual void RecvMapMoveComeFailed( BM::GUID const &kCharGuid );
	virtual bool AdjustArea( CUnit *pkUnit, bool const bIsSendAreaData, bool const bIsCheckPos );

protected:
	virtual bool SaveUnit( CUnit *pkUnit, SReqSwitchReserveMember const *pRSRM = NULL, BM::Stream * const pPacket = NULL );

	virtual bool IsAccess( PgPlayer *pkPlayer );
	virtual HRESULT SetUnitDropItem( CUnit *pkOwner, CUnit *pkDroper, PgLogCont &kLogCont );

	virtual bool IsDeathPenalty()const{return false;}
	virtual void EntityTick(CUnit * pkUnit, DWORD const dwkElapsed, float const fAutoHealMultiplier);
	virtual bool IsMonsterTickOK()const;

public:
	virtual bool IsDecEquipDuration()const{return false;}

//	인던 상속 함수
	virtual void SetState( EIndunState const eState, bool bAutoChange=false, bool bChangeOnlyState=false);
	virtual void SendMapLoadComplete( PgPlayer *pkUser );

//	그라운드 상속 함수
	void CallAlramReward( PgPlayer * pkPlayer );

	bool IsModeType( EPVPTYPE const kChkType )const;// War Ground가 PvP모드냐 다른거냐(공성전등)

protected:
	virtual void GMCommand_RecvGamePoint( PgPlayer *pkPlayer, int const iPoint );

// 전용함수
public:
	void Cancel();
	HRESULT Ready( BM::Stream& kPacket );

	int Get_KOH_OccupyPoint_NotIdleRate()const;
	PgGTrigger_KingOfHill const* Get_KOH_Trigger(BM::GUID const& rkUnitGuid)const;
	void KOH_SetTriggerUnitPoint(std::string const& rkTriggerID, int const iPoint);
	void KOH_CpatureStrongHold(CUnit * pUnit, float const Point);
	void SendNfyMessage(CUnit * pUnit, int MessageNo, bool bPlaySound = false, bool bBroadCast = true);
	int GetKOH_EntityLevelExp(EPvPSetting Setting);
	int GetKOH_CaptureDecValue(EPvPSetting Setting);
	void RemoveEnemeyInstanceItem(CUnit* pUnit);	// 상대팀의 인스턴트 아이템을 획득 할 때 호출.
	void RoundWin(int WinTeam);

	// 러브러브 모드.
	void BearTouchDown(CUnit * pUnit);	// 곰 이펙트가 있는 상태로 스코어 트리거에 올라갔을 때 호출.
	void CheckGenBear();				// 곰 아이템을 생성 시킬지 확인.
	void CheckDropBearDeleteTime();		// 곰 운송 도중 플레이어가 사망해서 바닥에 곰이 생성되었을 때, 다시 원래 자리로 되돌아갈 시간이 됐는지 검사.
	void PickupBear(CUnit * pUnit);		// 곰 아이템을 획득 했을 때 호출.
	void RecvBreakLoveFence(BM::Stream & Packet);		// 곰 우리 입구 오브젝트가 파괴 되었을 때 호출(클라이언트로 알려주기 위해).
	void ResetBreakObject();			// 라운드가 종료되어 곰 우리 입구 오브젝트가 다시 생성되었음을 트리거로 알람.
	void AddInsItemDropInfo(BM::GUID const & Droper, ETeam Team);	// 인스턴트 아이템이 드랍되었을 때, 해당 정보를 컨테이너에 저장해 두기 위함( 플레이어가 드랍하는 인스턴트 아이템 ).
	void RemoveInsItemDropInfo(BM::GUID const & CharGuid); // 인스턴트 아이템 드랍 정보를 컨테이너에서 제거.
	void CleanInsItemDropInfo(); // 드랍된 인스턴트 아이템중 제거해야될 원소가 있으면 컨테이너에서 제거.

protected:

	void RecvPvPReward( BM::Stream& kPacket );

protected:
	PgWarMode			*m_pkMode;

	CONT_INSITEMDROPINFO m_ContInsItemDropInfo;	// 인스턴스 아이템 드랍 정보.

	DWORD m_dwLoveModeObjectDestroyWaitTime;
	bool m_bCheckLoveModeObjectDestroyWaitTime;

};

#endif // MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGWARGROUND_H