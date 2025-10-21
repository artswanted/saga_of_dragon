#pragma once

#include "PgPvPType_DeathMatch.h"
#include "PgPvPModeStruct.h"

struct SHoldRet
{
	SHoldRet()
		:	dwHoldTime(0)
	{}

	BM::GUID	kCharGuid;
	DWORD		dwHoldTime;
};

class PgPvPHillStatus
{
public:
	PgPvPHillStatus( SPvPHillNode const &rkNode );
	~PgPvPHillStatus();

	HRESULT OnHill( CUnit *pkUnit, SHoldRet &kOldRet );
	HRESULT OutHill( CUnit *pkUnit, SHoldRet &kOldRet );

	void Hold( bool bHold );
	bool Hold()const{return m_bHold;}

	bool Refresh();
	bool GetHoldRet( DWORD const dwCurTime, SHoldRet &kRet );
	int	GetHoldTeam()const{return m_iHoldTeam;}

protected:
	SPvPHillNode		m_kContNode;
	CONT_PVPHILL_ITEM	m_kContHoldUnitInfo;
	DWORD				m_dwLastHoldTime;
	bool				m_bHold;
	int					m_iHoldTeam;
};


class PgPvPMode_KingOfHill
	:	public PgPvPType_DeathMatch
{
public:
	typedef std::map<size_t,PgPvPHillStatus>	CONT_PVPHILL_STATUS;
	typedef std::map< GTRIGGER_ID, PgGTrigger_KingOfHill* >	CONT_KOH_TRIGGER;

public:
	PgPvPMode_KingOfHill();
	virtual ~PgPvPMode_KingOfHill();

	virtual void ReadyGame( PgWarGround *pkGnd=NULL );
	virtual bool Start( PgPlayer *pkPlayer, PgWarGround *pkGnd );
	virtual bool End( bool const bAutoChange, PgWarGround *pkGnd );
	virtual bool Exit( BM::GUID const& kGuid );
	virtual EResultEndType ResultEnd();

	virtual EPVPTYPE Type()const{return PVP_TYPE_KTH;}
	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround* pkGnd );

	virtual HRESULT Update(DWORD const dwCurTime);

	virtual DWORD GetTime( EPvPTime const kType )const;

	// Event
	virtual HRESULT SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller );
	virtual HRESULT SetEvent_HillUp( CUnit *pkUnit, size_t const iHillNo );
	virtual HRESULT SetEvent_HillOut( CUnit *pkUnit );

	virtual HRESULT SetEvent_BonusPoint( CUnit *pkUnit, short const nAddPoint, PgWarGround *pkGnd );
	virtual HRESULT SetEvent_GetInstanceItem( CUnit * pUnit, PgWarGround * pGnd );
	virtual HRESULT SetEvent_CapturePoint( CUnit * pUnit, float const AddPoint, PgWarGround * pGnd );

	void SetTriggerUnitPoint(std::string const& kTriggerID, int const iPoint);
	void SetTriggerUnitPoint(CUnit * pkUnit, int const iPoint);

	PgGTrigger_KingOfHill const* GetTrigger(std::string const& kTriggerID)const;
	PgGTrigger_KingOfHill const* GetTrigger(BM::GUID const& kUnitGuid)const;

	void SendGamePoint();

protected:
	virtual bool CalcRoundWinTeam( int &iOutRoundWinTeam );// 리턴값은 게임 종료여부

protected:

	bool UpdateHoldResult( SHoldRet const &kRet );
	void ClearContTrigger();
	void InstallContTriggerEntity();
	void InsertEntity(std::string const& kTriggerID, int const iEntityNo, int const iLevel, ETeam const eTeam);

	KOH_ENTITY_KEY GetEntityNo(std::string const& kTriggerID, int const iLevel, ETeam const eTeam)const;
	int GetIngEffectNo(std::string const& kTriggerID, ETeam const eTeam)const;
	int GetLinkPoint(std::string const& kTriggerID, int const iTeam)const;
	void SetRedPoint(int const iAddPoint, bool const bSend);
	void SetBluePoint(int const iAddPoint, bool const bSend);

	PgGround *	m_pkGnd;

	CONT_PVPHILL_STATUS				m_kContHill;
	DWORD							m_dwNextChangeTime;
	CONT_KOH_TRIGGER				m_kContTrigger;
	
	int m_iRedPoint;
	int m_iBluePoint;
	DWORD m_dwVictoryPointStartTime;
	DWORD m_dwChangeEntityTime;
	bool m_bEndRedLimt;
	bool m_bEndBlueLimit;
};