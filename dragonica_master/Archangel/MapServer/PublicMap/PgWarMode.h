#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARMODE_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARMODE_H

#include "Variant/PgTotalObjectMgr.h"
#include "Lohengrin/PacketStruct2.h"
#include "PgPvPSetting.h"

typedef enum 
{
	RESULT_END_DEFAULT,	// 그냥 현재 상태 유지
	RESULT_END_CLOSE,	// 인던을 닫아야 함(쫒아내야함)
	RESULT_END_RESTART,	// 재시작 해야함
	RESULT_END_WAIT_ENTRY,// 엔트리 선택 대기
}EResultEndType;

class PgWarGround;

class PgWarMode
	: public BM::CSubject<BM::Stream*>
{
public:
	static bool ParseXml( char const* szPath );
	static PgPvPSetting ms_kSetting;

protected:
	static bool ParseXml( EPVPTYPE const kType, TiXmlElement const *pkElement );
	static int GetEnemyTeam( int const iTeam );
	
public:
	PgWarMode();
	virtual ~PgWarMode(){}

	virtual EPVPTYPE Type()const = 0;
	virtual int GetPvPLobbyID(void)const{return 0;}

	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround* pkGnd );

	virtual bool IsJoin( PgPlayer *  ) = 0;
	virtual HRESULT Join( PgPlayer *pkPlayer, bool const bInturde=false ) = 0;
	virtual bool Exit( BM::GUID const &kCharGuid ) = 0;
	virtual bool GetReadyUnitList( VEC_GUID& /*kUnitList*/ )const{return false;}
	virtual HRESULT IsObModePlayer( PgPlayer *pkPlayer, BM::GUID& rkOutTargetGuid ) { return E_FAIL; }

	virtual void ReadyGame( PgWarGround *pkGnd=NULL ) = 0;
	virtual bool IsStart( DWORD const dwNowTime ) = 0;
	virtual bool Start( PgPlayer *pkPlayer, PgWarGround * pkGnd ) = 0;
	virtual void StartGame( BM::Stream &kPacket, bool const bGame ) = 0;
	virtual bool End( bool const bAutoChange, PgWarGround *pkGnd ) = 0;
	virtual EResultEndType ResultEnd() = 0;

//	Update와 관련.
	virtual HRESULT Update( DWORD const dwCurTime ) = 0;
	virtual bool IsRevive()const{return true;}

//	ObjectUnit 관련
	virtual bool InitUnitInfo( UNIT_PTR_ARRAY const & /*kUnitArray*/, PgWarGround * /*pkGnd*/ ){return false;}
	virtual bool IsUseObjectUnit()const{return false;}
	virtual bool IsUseMonster()const{return false;}

	virtual void Modify( BM::Stream & /*kPacket*/ ){}

	virtual DWORD GetTime( EPvPTime const kType )const{return ms_kSetting.GetTime(kType);}

	// Event
	virtual HRESULT SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd ) = 0;
	virtual HRESULT SetEvent_BonusPoint( CUnit *pkUnit, short const nAddPoint, PgWarGround *pkGnd ) = 0;
	virtual HRESULT SetEvent_GetInstanceItem( CUnit * pUnit, PgWarGround * pGnd ) = 0;
	virtual HRESULT SetEvent_CapturePoint( CUnit * pUnit, float const AddPoint, PgWarGround * pGnd ) = 0;
	virtual void SetEvent_AdjustArea( CUnit *){}

	virtual bool Send( BM::Stream& kPacket )const = 0;

	virtual bool IsUseItem()const{return true;}
	virtual bool IsExercise(void)const{return false;}

	// 어뷰징 검사
	virtual bool IsAbusing(void)const{return false;}
	virtual int GetRewardID(void)const{return 0;}

	int GetBattleLevel(void)const{return m_iBattleLevel;}

	//대기시간 검사
	virtual bool CheckEntryWait(DWORD const dwElapsedTime) { return true; }
	virtual bool SelectEntry(int const iTeam, BM::GUID const &rkCharGuid) { return false; }

	virtual void SetRoundWinTeam(int WinTeam, PgWarGround * pGnd); 

	CLASS_DECLARATION_NO_SET( bool, m_bEndGame, IsEndGame );
	CLASS_DECLARATION_NO_SET( bool, m_bNoEnemy, NoEnemy );
	CLASS_DECLARATION_NO_SET( DWORD, m_dwGamePlayTime, GameTime );
	CLASS_DECLARATION_S( int, GamePoint);
	CLASS_DECLARATION_S( SGroundKey, GndKey);

protected:
	int m_iWinTeam;// 승리한 팀은
	int m_iBattleLevel;

	__int64		m_i64NextEventTime;// 현재는 공성전에서 시작전에 남은 타임용도로만 사용되고 있음.
};

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARMODE_H