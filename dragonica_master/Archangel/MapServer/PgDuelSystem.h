
#ifndef MAP_MAPSERVER_MAP_GROUND_PGDUELSYSTEM_H
#define MAP_MAPSERVER_MAP_GROUND_PGDUELSYSTEM_H

#include <map>

class PgGround;
class PgPlayer;
class BM::GUID;

class PgDuel
{
public:
	class Exception {};
	class InvalidGround : public Exception {}; //없는 그라운드가 참조되었다.
	class InvalidPlayer : public Exception {}; //없는 PC거나 유효하지 않은 PC 포인터이다.
	class PlayerDead : public Exception {}; //시작전인데 PC가 죽었다
	class DropConnect : public Exception {}; //유저와의 접속이 끊어졌다

	typedef enum
	{
		ESTATE_READY,
		ESTATE_BATTLE,
		ESTATE_FINISH
	}ESTATE;

	typedef enum
	{
		ERES_NOT_FINISH = 0,
		ERES_P1_WIN = 1,
		ERES_P2_WIN,
		ERES_DRAW,
	}EGAME_RESULT;

	typedef enum
	{
		ERES_CAUSE_KO = 0, //KO
		ERES_CAUSE_RING_OUT, //장외 (5초 제한)
		ERES_CAUSE_ZONE_OUT, //맵이동 또는 접속 종료
		ERES_CAUSE_TIME_OUT, //타임 오버
		ERES_CAUSE_TIME_LIMIT_RULE, //30초 제한 룰 (어뷰징 방지)
		ERES_CAUSE_DIE,
	}EGAME_RESULT_CAUSE;

	PgDuel(unsigned int,  PgGround*, const BM::GUID&, const BM::GUID&);
	~PgDuel();

	void Start(void);
	void End(void);
	void Update(void);
	ESTATE GetState(void) const { return m_eState; }
	bool IsExistPlayer(const BM::GUID&, const BM::GUID&);

protected:

private:
	WORD m_wGameTime, m_wPrepTime;
	WORD m_wAreaTime1, m_wAreaTime2;
	bool m_bAreaOut1, m_bAreaOut2;
	ESTATE m_eState;
	EGAME_RESULT m_eResult;
	EGAME_RESULT_CAUSE m_eResCauseBy;
	const int m_iID;
	const BM::GUID m_kIDPlayer1, m_kIDPlayer2;
	std::wstring m_stPlayerName1, m_stPlayerName2;
	BM::GUID m_kIDFlag;
	PgGround* m_pkGround;
	static WORD const m_wInitGameTime = 180; //게임시간 초기화할 상수
	static WORD const m_wInitPrepTime = 5;
	static WORD const m_wImpendTime = 5; //종료가 얼마 남지 않았음을 알릴 시간
	static WORD const m_wInitAreaTime = 5; //장외로 벗어났을때 제한시간을 초기화할 상수
	static WORD const m_wOutDist = 300; //결투시 깃발로부터의 유효한 반경 (벗어나면 장외 시간체크)

	PgDuel();
	PgPlayer* GetPlayer(const BM::GUID&);
	CUnit* GetFlag(void);
	void SetDuel(PgPlayer* pkPlayer); //결투 모드로 어빌 및 능력치 설정
	void RestorePlayer(PgPlayer* pkPlayer); //시작전의 PC 상태로 복구해주자(어빌, HP, MP, ...등등)
	void CreateFlag(void);
	bool CheckOutOfArea(void);
	bool CheckTimeOut(void);
	EGAME_RESULT DecideWinner(void); //승자를 얻어옴: 1이면 player1, 2이면 player2, 0이면 무승부
};

class PgDuelMgr
{
public:
	PgDuelMgr() : m_kMutex(), m_kContDuel() {}
	~PgDuelMgr() {}

	bool Create(PgGround*, BM::GUID const &, BM::GUID const &);
	void Update(void);
	bool FindExistPlayer(BM::GUID const&, BM::GUID const&);

protected:
	unsigned int GenerateID(void);

private:
	typedef std::map<int, PgDuel> CONT_DUEL;
	CONT_DUEL m_kContDuel;
	mutable Loki::Mutex m_kMutex;
};


#endif