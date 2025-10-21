#ifndef WEAPON_VARIANT_GUILD_EMPORIATOURNAMENT_H
#define WEAPON_VARIANT_GUILD_EMPORIATOURNAMENT_H

struct tagEmporiaGuildInfo;

class PgEmporiaTournamentElement
{
	friend class PgEmporiaTournament;

public:
	enum eState
	{
		STATE_NONE			= 0x00,
		STATE_REQ_CREATE	= 0x01,
		STATE_RECV_CREATE	= 0x02,
		STATE_CREATED		= STATE_REQ_CREATE|STATE_RECV_CREATE,
		STATE_RECV_START	= 0x04,
		
		FLAG_WIN_ATTACK		= 0x10,
		FLAG_WIN_DEFENCE	= 0x20,
		FLAG_CHECK_END		= 0x30,

		FLAG_FULL_ATTACK	= 0x40,
		FLAG_FULL_DEFENCE	= 0x80,
	};

	typedef std::pair< BYTE, tagEmporiaGuildInfoEx* >	T_WINNER_INFO;

public:
	PgEmporiaTournamentElement();
	~PgEmporiaTournamentElement();

	static HRESULT GetParentIndex( size_t const iIndex, size_t &iOutParentIndex );
	static void GetChildIndex( size_t const iIndex, size_t &iOutChildLeft, size_t &iOutChildRight );

	size_t GetIndex()const{return m_iIndex;}
	void SetState( BYTE const byState ){m_byState = byState;}
	BYTE GetState()const{return m_byState;}

	HRESULT SetWinner( BM::GUID const &kGuildGuid );
	HRESULT GetWinnerInfo( tagEmporiaGuildInfo &kOutGuildInfo )const;

	PgEmporiaTournamentElement const *GetParent()const{return m_pkParent;}
	__int64 GetBattleTime(void)const{return m_i64BattleTime;}
	HRESULT GetGuildInfo( bool const bAttacker, tagEmporiaGuildInfo &kOutGuildInfo )const;
	HRESULT GetGuildInfoEx( bool const bAttacker, tagEmporiaGuildInfoEx &kOutGuildInfo )const;
	BM::GUID GetGuildGuid( bool const bAttacker )const;
	PgLimitClass const GetMercenaryLimit();
	HRESULT SaveMercenary( bool const bAttacker, VEC_GUID const & rkMember );
	void GetMecenaryList(VEC_GUID & rkContList);

	bool IsGuild( bool const bAttacker, BM::GUID const &kGuildGuid )const;
	bool IsAccess( bool const bAttacker, PgLimitClass const &kLimitClass )const;
	HRESULT IsBattle()const;

	bool IsParentLinkAttack()const{return (0 == m_iIndex) || (100 <= m_iIndex) || (1 == (m_iIndex % 2));}
	bool IsBeforJoinMercenary(bool const bIsAttack, BM::GUID const & kCharGuid)const;
protected:
	T_WINNER_INFO GetWinner()const;

protected:
	size_t						m_iIndex;
	BYTE						m_byState;//eState 사용
	__int64						m_i64BattleTime;
	tagEmporiaGuildInfoEx*		m_pkAttackGuilDInfo;
	tagEmporiaGuildInfoEx*		m_pkDefenceGuilDInfo;
	PgEmporiaTournamentElement*	m_pkParent;
};

class PgEmporiaTournament
{
public:
	static size_t const EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX = 100;
	static __int64 const EMPORIA_BATTLE_CHALLENGE_STARTTIME_BEFORE_BATTLETIME = CGameTime::OneDay * 3i64;// 도전 시작이 가능한 시간
	static __int64 const EMPORIA_BATTLE_CHALLENGE_ENDTIME_BEFORE_BATTLETIME = CGameTime::OneDay;// 도전이 종료되는 시간 

public:
	typedef std::vector< __int64 >						CONT_INIT_TIME;
	typedef std::map< size_t, SEmporiaGuildInfo >		CONT_INIT_GUILD;// first index

	typedef std::vector< SEmporiaGuildInfoEx >			CONT_GUILDINFO;
	typedef std::vector< PgEmporiaTournamentElement >	CONT_TOURNAMENT;

public:
	static SEmporiaGuildInfo const		ms_kEmptyGuildInfo;

public:
	PgEmporiaTournament();
	~PgEmporiaTournament();

public:
	HRESULT Init( CONT_INIT_TIME const &kTournamentTime, CONT_INIT_TIME const &kTournamentTime2 );
	HRESULT InitGuild( CONT_INIT_GUILD const &kInitGuild );
	HRESULT InitMercenary( CONT_TBL_EM_MERCENARY_INDEX const &rkContData );
	void Release();

	HRESULT	SetWinner( size_t const iIndex, BM::GUID const &kWinGuildID );

	void WriteToPacket( BM::Stream &kPacket )const;
	bool ReadFromPacket( BM::Stream &kPacket );

	bool IsEmpty()const{return m_kContTournament.empty();}
	bool IsHaveGuild( BM::GUID const &kGuildID )const;
	bool IsNowJoinMercenary(PgLimitClass const & rkLimitClass)const;
	bool ProcessJoinMercenary(BM::GUID const & kMercenaryCharGuid, BM::GUID const & kGuildID, PgLimitClass const & rkLimitClass, float & fRemainTime)const;

	HRESULT SetGuildMercenary( BM::GUID const &kGuildID, PgLimitClass const &kLimitClass );

	HRESULT GetBattleTime( size_t const iIndex, __int64 &i64OutTime )const;
	HRESULT IsBattle( size_t const iIndex )const;
	HRESULT GetGuildInfo( size_t const iIndex, bool const bAttacker, tagEmporiaGuildInfo &kOutGuildInfo )const;
	HRESULT GetGuildInfoEx( size_t const iIndex, bool const bAttacker, tagEmporiaGuildInfoEx &kOutGuildInfo )const;
	void GetGuildInfo( CONT_GUILDINFO &rkOutContGuildInfo )const{rkOutContGuildInfo = m_kContGuildInfo;}
	void GetGuildListGuid(VEC_GUID & kContGuild);

	PgEmporiaTournamentElement* GetElement( size_t const iIndex );
	PgEmporiaTournamentElement const* GetElement( size_t const iIndex )const;

	void SetBattleTimeAsSoon( __int64 const i64Time );

private:
	void WriteToPacket( BM::Stream &kPacket, PgEmporiaTournamentElement const &kElement )const;
	void ReadFromPacket( BM::Stream &kPacket, PgEmporiaTournamentElement &kElement, bool const bIsSecond );

protected:
	SEmporiaGuildInfoEx			m_kEmptyGuildInfo;
	CONT_GUILDINFO				m_kContGuildInfo;
	CONT_TOURNAMENT				m_kContTournament;
	CONT_TOURNAMENT				m_kContTournament2;// 엠포리아 토너먼트

private:// Not Use
	PgEmporiaTournament( PgEmporiaTournament const & );
	PgEmporiaTournament& operator = ( PgEmporiaTournament const & );
};

#endif // WEAPON_VARIANT_GUILD_EMPORIATOURNAMENT_H