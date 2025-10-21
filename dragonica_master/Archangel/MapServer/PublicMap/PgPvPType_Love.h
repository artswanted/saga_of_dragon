#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_LOVE_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_LOVE_H

#include "PgPvPType_DeathMatch.h"
#include "PgPvPModeStruct.h"

const WORD DROPBEAR_ITEM_DELTIME = 5000;

class PgPvPType_Love
	: public PgPvPType_DeathMatch
{

public:

	PgPvPType_Love();
	virtual ~PgPvPType_Love();

public:
	
	virtual void ReadyGame(PgWarGround * pGnd = NULL);
	virtual bool Start(PgPlayer * pPlayer, PgWarGround * pGnd);
	virtual bool End(bool const bAutoChange, PgWarGround * pGnd);
	virtual bool Exit(BM::GUID const & Guid);
	virtual EResultEndType ResultEnd();
	
	virtual bool IsUseObjectUnit() const { return true; }

	virtual EPVPTYPE Type() const { return PVP_TYPE_LOVE; }
	virtual HRESULT Init(BM::Stream & Packet, PgWarGround * pGnd);

	virtual HRESULT Update(DWORD const dwCurTime);
	virtual DWORD GetTime(EPvPTime const Type) const;

	virtual HRESULT SetEvent_Kill(CUnit * pUnit, CUnit * pKiller, PgWarGround * pGnd);

	virtual void StartGame( BM::Stream & Packet, bool const bGame );

	virtual void SetRoundWinTeam(int WinTeam, PgWarGround * pGnd);		// GM커맨드.

public:

	void BearTouchDown(CUnit * pUnit, PgWarGround * pGnd);		// 곰 이펙트가 있는 상태로 스코어 트리거에 올라 섰을 때 호출.
	void BearFenceReset(PgObjectUnit * pObjUnit);				// 라운드가 끝나고 곰 울타리 입구 오브젝트 리젠됐음을, 트리거에 알리고 클라로 전송.

	void SetRedScoreTrigger(PgGTrigger_Score * pTrigger);
	void SetBlueScoreTrigger(PgGTrigger_Score * pTrigger);

	PgGTrigger_Score * const GetRedScoreTrigger() const;
	PgGTrigger_Score * const GetBlueScoreTrigger() const;

	void SendLoveGamePoint(BM::GUID const & CharGuid, EPvPSetting SettingType);

	void CheckPlayerHaveBear(CUnit * pUnit);

protected:

	PgWarGround * m_pGnd;

	CLASS_DECLARATION_S(int, RedBearNo);			// 빨간곰 아이템 번호(업고 가다가 죽을 때 생성시킬 아이템).
	CLASS_DECLARATION_S(int, BlueBearNo);			// 파란곰 아이템 번호(업고 가다가 죽을 때 생성시킬 아이템).

	CLASS_DECLARATION_S(bool, OnRedBear);			// 빨간곰이 제자리에 있는지.
	CLASS_DECLARATION_S(bool, OnBlueBear);			// 파란곰이 제자리에 있는지.

	CLASS_DECLARATION_S(bool, OnBlueBearRegen);		// 파란곰을 생성시켜야 하는가?
	CLASS_DECLARATION_S(bool, OnRedBearRegen);		// 빨간곰을 생성시켜야 하는가?

	CLASS_DECLARATION_S(WORD, RedBearDropAccumTime);		// 빨간곰 운송 도중 드랍되었을 때 바닥에 있는 시간.
	CLASS_DECLARATION_S(WORD, BlueBearDropAccumTime);		// 파란곰 운송 도중 드랍되었을 때 바닥에 있는 시간.

	PgGTrigger_Score * m_pRedScoreTrigger;			// 레드팀 스코어 트리거.
	PgGTrigger_Score * m_pBlueScoreTrigger;			// 블루팀 스코어 트리거.

	CLASS_DECLARATION_NO_SET(int, m_kPrevRoundWinTeam, PrevRoundWinTeam);		// 이전 라운드 승리 팀.

};	

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_LOVE_H