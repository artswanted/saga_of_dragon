#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGEMPORIAPACK_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGEMPORIAPACK_H

#include "Variant/PgEmporia.h"
#include "Variant/EmporiaTournament.h"

size_t const EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT = 8;
size_t const EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMINDEX[EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT] = { 14, 10, 8, 12, 11, 7, 9, 13 };

extern __int64 EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TIMETERM[EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT-1];
extern __int64 EMPORIA_BATTLE_EMPORIA_TOURNAMENT_TIMETERM[MAX_EMPORIA_GRADE];

typedef std::map< BM::GUID, SEmporiaChallenge > CONT_CHALLENGE_BATTLE;

typedef struct tagBattleInfo
{
	explicit tagBattleInfo( SEmporiaBattleGroundInfo const &kGndInfo )
		:	pkElement(NULL)
		,	kGameType(kGndInfo.kGameType)
		,	kBattleGroundKey( kGndInfo.iGroundNo, BM::GUID::Create() )
		,	i64RealBattleTime(0)
	{}

	bool operator == ( tagBattleInfo const &rhs )const
	{
		return pkElement == rhs.pkElement;
	}

	PgEmporiaTournamentElement*	pkElement;
	EPVPTYPE					kGameType;
	SGroundKey					kBattleGroundKey;
	__int64						i64RealBattleTime;
}SBattleInfo;
typedef std::list<SBattleInfo>		CONT_ING_BATTLE;

typedef struct tagResultRewardInfo
{
	tagResultRewardInfo():iItemNo(0),iAchieveSaveIdx(0),i64GuildGold(0){}

	int iItemNo;
	int iAchieveSaveIdx;
	__int64 i64GuildGold;
}SResultRewardInfo;

template< size_t iNum >
class PgEmporiaPack
{
public:
	static int const ms_iChallengeResultMail_Sender = 9000100;
	static int const ms_iChallengeResultMail_Title = 9000101;
	static int const ms_iChallengeResultMail_Contents_Top = 9000102;
	static int const ms_iChallengeResultMail_Contents_1 = 9000104;// 도전 길드 수
	static int const ms_iChallengeResultMail_Contents_2	= 9000105;// 토너먼트 평균 길드골드
	static int const ms_iChallengeResultMail_Contents_3 = 9000103;// 도전 길드골드
	static int const ms_iChallengeResultMail_Contents_Date = 9000106;// 전투 시간
	static int const ms_iChallengeResultMail_Gold = 73000220;//골드

	static SEmporiaBattleGroundInfo const ms_kBattleGroundInfo_Challenge;	// default
	static SEmporiaBattleGroundInfo	const ms_kBattleGroundInfo_Emporia;		// default

public:
	explicit PgEmporiaPack( BM::GUID const &kID, int const iState, char const nBattlePeriodForWeeks, BM::GUID const &kBattleID, BM::DBTIMESTAMP_EX const &kBattleDate, BM::GUID const &kPrevBattleID, SGroundKey const &kBaseMapKey );
	~PgEmporiaPack(void);

	HRESULT InitTournament( BM::GUID const &kBattleID, CONT_TBL_EMPORIA_TOURNAMENT const &kContTournament, CONT_TBL_EMPORIA_CHALLENGE_BATTLE const &kBattleElement, CONT_EMPORIA_FROMGUILD &kContGuild );
	HRESULT InitMercenary( CONT_TBL_EM_MERCENARY const & kContMercenary );
	void CheckData();
	
	HRESULT RecvCreateEmporiaGround( size_t iIndex, SGroundKey const &kEmporiaGndKey, bool const bSuccess );
	HRESULT RecvDeleteEmporiaGround(  SGroundKey const &kGndKey );

	BM::GUID const &GetID(void)const{return m_kID;}
	BM::GUID const &GetBattleID(void)const{return m_kBattleID;}
	BM::GUID const &GetPrevBattleID(void)const{return m_kPrevBattleID;}
	int GetState(void)const{return m_iState;}

	SGroundKey const &GetBaseMapKey(void)const{return m_kBaseMapKey;}
	HRESULT GetEmporiaInfo( BM::GUID const &kGuildID, SGuildEmporiaInfo& rkOutEmporiaInfo )const;
	BYTE GetEmporiaKey( size_t iGrade, BM::GUID const &kGuildGuid, SGroundKey &kOutGndKey )const;
	BYTE GetEmporiaKeyFromMercenary( BM::GUID const &kGuildGuid, BM::GUID const &kCharGuid, PgLimitClass const &kClassInfo, size_t const iJoinIndex, bool const bIsAttack, SGroundKey &kOutGndKey )const;

	PgEmporia const* GetElement( size_t iIndex )const;
	bool SetElement( size_t iIndex, PgEmporia const &rkElement );

	size_t GetOpenLevel(void)const;
	HRESULT Open( BM::DBTIMESTAMP_EX const &kNextBattleDate, char nBattlePeriodsForWeeks, size_t iOpenLevel );
	HRESULT Close();

	HRESULT OpenElement( size_t const iIndex, bool const bOpen );

	HRESULT AddChallenge( SEmporiaChallenge const *pkChallenge, bool const bSystem = false );
	HRESULT RemoveChallenge( BM::GUID const &kGuildID, SEmporiaChallenge *pkOutChallenge, bool const bSystem = false );

	void Update( __int64 const i64NowTime );
	HRESULT BattleStart( __int64 const i64Time );//강제시작
	HRESULT RecvCreateBattle( SGroundKey const &kBattleGndKey, bool const bStart );
	HRESULT CheckCreateBattleCompleted();
	HRESULT RecvBattleResult( SGroundKey const &kBattleGndKey, bool const bAttackerWin, BM::GUID &rkOutEraseGuildID, CONT_EM_RESULT_USER const & rkWinMember, CONT_EM_RESULT_USER const & rkLoseMember, EEmporiaResultNoticeType const eResultType );
	HRESULT SetChangeEmporiaOwner( size_t const iIndex, SEmporiaGuildInfo const &kGuildInfo, bool const bSave, bool const bCallSystem );
	HRESULT SetEmporiaBattleLock( SGroundKey const &kBattleGndkey, bool const bAttack, bool const bLock );
	HRESULT SetGate( size_t const iIndex, BYTE const byGate, bool const bSave );
	void RecvSettingMercenary( BM::Stream * const pkPacket );
	HRESULT SetEmporiaBattleReady( SGroundKey const &kBattleGndkey );

	__int64 GetBattleTime()const;
	HRESULT GetFunction( size_t const iIndex, short nFuncNo, SEmporiaFunction &rkFuncInfo )const;
	HRESULT AddFunction( size_t const iIndex, short nFuncNo, SEmporiaFunction const &kFuncInfo );
	HRESULT RemoveFunction( size_t const iIndex, short nFuncNo );
	HRESULT UpdateFunctionExtValue( size_t const iIndex, short nFuncNo, __int64 i64ExtValue );

	void WriteToPacket( BM::Stream &kPacket )const;
	HRESULT WriteToPacket_Grade( size_t const iIndex, BM::Stream &rkPacket, bool const bSimple )const;

	HRESULT Send( size_t const iIndex, BM::Stream const &kPacket )const;

protected:
	void Save();
	void SaveElement( size_t const iIndex, SEmporiaGuildInfo const *pOldGuildInfo );
	void SaveBattleRet( BM::GUID const &kWinGuildID, BM::GUID const &kLoseGuildID, short const nIndex );
	void SaveSwapEmporia( size_t const iLeft, size_t const iRight );
	void SaveMercenary( int const iBattleIndex, BM::GUID const & rkGuildID, BM::GUID const & rkCharID ) const;
	HRESULT SaveWinMercenary(bool const bAttacker, PgEmporiaTournamentElement * pkElement, BM::GUID const & rkGuildID, CONT_EM_RESULT_USER const & rkMember, int const iBattleIndex );
	HRESULT SendReward( bool const bWinner, CONT_EM_RESULT_USER const & rkMember, int const iBattleIndex, const BM::GUID &rkWinGuildGuid, SResultRewardInfo & kRewardInfo ) const;

	HRESULT SetState( int const iState, bool bCall );

	bool CheckChallengeTournament( __int64 i64NowTime );
	bool CheckEmporiaTorunament( __int64 i64NowTime );
	HRESULT ReqCreateBattle( SBattleInfo const &kBattleInfo );

private:
	mutable Loki::Mutex	m_kMutex;

	BM::GUID const		m_kID;// 엠포리아 ID
	SGroundKey const	m_kBaseMapKey;

	// 상태 컨트롤 관련
	int					m_iState;
	__int64				m_i64NextStateChangeTime;
	
	// 전쟁정보
	char				m_nBattlePeriodsForWeeks;
	BM::GUID			m_kBattleID;
	BM::GUID			m_kPrevBattleID;
	__int64				m_i64BattleTime;

	// 엠포리아 정보
	PgEmporia			m_kEmporia[iNum];

	// Tournament
	PgEmporiaTournament		m_kTournament;

	// Challenge(도전정보)
	CONT_CHALLENGE_BATTLE	m_kContChallenge;
	__int64					m_i64TotalChallengeExp;

	// 현재 진행중인 전쟁 정보
	CONT_ING_BATTLE			m_kContBattle_Ing;
//	bool					m_bSendBegin;//드럽네...

private:// 사용 금지
	PgEmporiaPack(void);
	PgEmporiaPack( PgEmporiaPack const & );
	PgEmporiaPack& operator=( PgEmporiaPack const & );

};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGEMPORIAPACK_H