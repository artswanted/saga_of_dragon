#ifndef WEAPON_VARIANT_GUILD_PGEMPORIA_H
#define WEAPON_VARIANT_GUILD_PGEMPORIA_H

#include "Lohengrin/dbtables.h"
#include "Variant/LimitClass.h"
#include "PgEmporiaFunction.h"

typedef enum
{
	EMPORIA_KEY_NONE			= 0	,	// 소유한 엠포리아가 없다.
	EMPORIA_KEY_MINE			= 1	,	// 소유한 엠포리아가 있다.
	EMPORIA_KEY_BATTLERESERVE	= 2	,	// 엠포리아 쟁탈전을 예약했다.
	EMPORIA_KEY_BATTLECONFIRM	= 3	,	// 엠포리아 쟁탈전을 예약해서 전쟁확정
	EMPORIA_KEY_BATTLE_ATTACK	= 4	,	// 엠포리아 쟁탈전 진행중(공격)
	EMPORIA_KEY_BATTLE_DEFENCE	= 5	,	// 엠포리아 쟁탈전 진행중(수비)
}EEmporiaKeyType;

#pragma pack(1)

typedef enum
{
	EMPORIA_CLOSE								= 0x0000,
	EMPORIA_PEACE								= 0x0001,// 평화로운 상태이다.
	EMPORIA_TERM_OF_CHALLENGE					= 0x0002,// 도전기간
	EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT		= 0x0003,// 도전자 토너먼트 하기전
	EMPORIA_CHALLENGE_OF_TOURNAMENT				= 0x0004,// 도전자 토너먼트 진행중
	EMPORIA_OWNERSHIP_OF_TOURNAMENT				= 0x0005,// 소유권 토너먼트 진행중

	EMPORIA_STATE_ADD							= 0x0001,
	EMPORIA_ROUND_MASK							= 0x00FF,
}E_EMPORIA_STATE;

typedef enum
{
	EMPORIA_MERCENARY_NONE		= 0,
	EMPORIA_MERCENARY_ATK		= 1,
	EMPORIA_MERCENARY_DEF		= 2,	
}E_EMPORIA_MERCENARY_STATE;

typedef enum
{
	EMBATTLE_NONE							= 0,
	EMBATTLE_REGIST_TOURNAMENT_1			= 1,// 토너먼트에 등록
	EMBATTLE_REGIST_TOURNAMENT_2			= 2,// 토너먼트에 등록
	EMBATTLE_REGIST_TOURNAMENT_3			= 3,// 토너먼트에 등록
	EMBATTLE_REGIST_TOURNAMENT_4			= 4,// 토너먼트에 등록
	EMBATTLE_REGIST_TOURNAMENT_5			= 5,// 토너먼트에 등록
	EMBATTLE_REGIST_TOURNAMENT_6			= 6,// 토너먼트에 등록
	EMBATTLE_REGIST_TOURNAMENT_7			= 7,// 토너먼트에 등록
	EMBATTLE_REGIST_TOURNAMENT_8			= 8,// 토너먼트에 등록

	EMBATTLE_CHECK_TORUNAMENT_INDEX			= 0x0F,

	EMBATTLE_LOSTEMPORIA					= 0x70,
}E_EMPORIA_BATTLE_STATE;

typedef enum
{
	EMPORIA_GATE_CLOSE,
	EMPORIA_GATE_GUILD,
	EMPORIA_GATE_PUBLIC,
	EMPORIA_GATE_MAX,
}E_EMPORIA_GATE_STATE;

typedef struct tagEmporiaKey
{
	tagEmporiaKey()
		:	byGrade(0)
	{}

	tagEmporiaKey( BM::GUID const &in_kID, BYTE const in_byGrade )
		:	kID(in_kID)
		,	byGrade(in_byGrade)
	{}

	bool operator < ( tagEmporiaKey const &rhs )const
	{
		if ( kID == rhs.kID ){return byGrade < rhs.byGrade;}
		return kID < rhs.kID;
	}

	bool operator > ( tagEmporiaKey const &rhs )const{return rhs < *this;}
	bool operator ==( tagEmporiaKey const &rhs )const{return !( ( *this < rhs ) && ( rhs < *this ) ); }

	BM::GUID	kID;
	BYTE		byGrade;

	BM::GUID const &GetKey()const{return kID;}
}SEmporiaKey;

typedef struct tagGuildEmporiaInfo
:	public SEmporiaKey
{
	tagGuildEmporiaInfo( BYTE const _byType=EMPORIA_KEY_NONE )
		:	byType(_byType)
		,	byMercenaryFlag(EMPORIA_MERCENARY_NONE)
		,	i64BattleTime(0i64)
	{}

	void Clear()
	{
		SEmporiaKey::operator = ( SEmporiaKey() );
		byType = EMPORIA_KEY_NONE;
		byMercenaryFlag = EMPORIA_MERCENARY_NONE;
	}

	void Set( SEmporiaKey const &kEmporiaKey )
	{
		SEmporiaKey::operator = ( kEmporiaKey );
	}

	bool IsNowBattle(void)const
	{
		return ( EMPORIA_KEY_BATTLE_ATTACK == byType || EMPORIA_KEY_BATTLE_DEFENCE == byType );
	}

	BYTE		byType;// EEmporiaKeyType 사용
	BYTE		byMercenaryFlag;
	__int64		i64BattleTime;
}SGuildEmporiaInfo;

typedef struct tagEmporiaGuildInfo
{
	tagEmporiaGuildInfo()
		:	nEmblem(0)
	{}

	explicit tagEmporiaGuildInfo( BM::GUID const &_kGuildID )
		:	kGuildID(_kGuildID)
	{
	}

	bool operator < ( tagEmporiaGuildInfo const &rhs )const
	{
		return kGuildID < rhs.kGuildID;
	}

	bool operator > ( tagEmporiaGuildInfo const &rhs )const
	{
		return rhs < *this;
	}

	bool operator == ( tagEmporiaGuildInfo const &rhs )const
	{
		return kGuildID == rhs.kGuildID;
	}

	bool IsEmpty(void)const{return BM::GUID::IsNull(kGuildID);}

	BM::GUID		kGuildID;
	std::wstring	wstrName;
	BYTE			nEmblem;

	void WriteToPacket( BM::Stream &rkPacket )const
	{
		rkPacket.Push( kGuildID );
		if ( BM::GUID::IsNotNull(kGuildID) )
		{
			rkPacket.Push( wstrName );
			rkPacket.Push( nEmblem );
		}
	}

	bool ReadFromPacket( BM::Stream &rkPacket )
	{
		rkPacket.Pop( kGuildID );
		if ( BM::GUID::IsNotNull( kGuildID ) )
		{
			if ( true == rkPacket.Pop( wstrName, MAX_GUILDNAME_LEN ) )
			{
				return rkPacket.Pop( nEmblem );
			}
		}
		else
		{
			wstrName = std::wstring();
			nEmblem = 0;
			return true;
		}
		
		return false;
	}

	size_t min_size()const
	{
		return	sizeof(kGuildID);
	}

	size_t max_size()const
	{
		return	min_size()
			+	sizeof(nEmblem)
			+	(size_t)
			+	(sizeof(wchar_t) * MAX_GUILDNAME_LEN);
	}
}SEmporiaGuildInfo;

typedef std::vector<BM::GUID>	CONT_EMPORIA_MERCENARY;
typedef struct tagEmporiaGuildInfoEx
:	public tagEmporiaGuildInfo
{
	tagEmporiaGuildInfoEx()
	{}

	explicit tagEmporiaGuildInfoEx( tagEmporiaGuildInfo const &rhs )
		:	tagEmporiaGuildInfo(rhs)
	{
	}

	tagEmporiaGuildInfoEx& operator = ( tagEmporiaGuildInfo const &rhs )
	{
		tagEmporiaGuildInfo::operator = ( rhs );
		return *this;
	}

	PgLimitClass	m_kLimitMercenary;

	void WriteToPacket( BM::Stream &rkPacket )const
	{
		tagEmporiaGuildInfo::WriteToPacket( rkPacket );
		m_kLimitMercenary.WriteToPacket( rkPacket );
		rkPacket.Push(kContMercenary);
	}

	bool ReadFromPacket( BM::Stream &rkPacket )
	{
		if( !tagEmporiaGuildInfo::ReadFromPacket( rkPacket ) ){ return false; }
		if( !m_kLimitMercenary.ReadFromPacket( rkPacket ) ){ return false; }
		if( !rkPacket.Pop(kContMercenary) ){ return false; }
		return true;
	}

	size_t min_size()const
	{
		return	tagEmporiaGuildInfo::min_size()
			+	m_kLimitMercenary.min_size()
			+	kContMercenary.size();
	}

	size_t max_size()const
	{
		return	tagEmporiaGuildInfo::max_size()
			+	m_kLimitMercenary.min_size();
	}

	VEC_GUID kContMercenary;
}SEmporiaGuildInfoEx;

typedef struct tagEmporiaChallenge
{
	tagEmporiaChallenge()
		:	i64Cost(0i64)
		,	i64ChallengeTime(0i64)
	{}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kGuildInfo.WriteToPacket( kPacket );
		kPacket.Push( i64Cost );
		kPacket.Push( i64ChallengeTime );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		if ( kPacket.RemainSize() < min_size() )
		{
			return false;
		}

		if ( true == kGuildInfo.ReadFromPacket( kPacket ) )
		{
			kPacket.Pop( i64Cost );
			return kPacket.Pop( i64ChallengeTime );
		}
		return false;
	}

	size_t min_size()const
	{
		return	sizeof(i64Cost)
			+	sizeof(i64ChallengeTime)
			+	kGuildInfo.min_size();
	}

	size_t max_size()const
	{
		return	sizeof(i64Cost)
			+	sizeof(i64ChallengeTime)
			+	kGuildInfo.max_size();
	}

	SEmporiaGuildInfo	kGuildInfo;
	__int64				i64Cost;
	__int64				i64ChallengeTime;
}SEmporiaChallenge;

typedef struct tagEmporiaBattleKey
{
	tagEmporiaBattleKey()
	{}

	bool operator < ( tagEmporiaBattleKey const &rhs )const
	{
		if ( kEmporiaID == rhs.kEmporiaID )
		{
			return kBattleID < rhs.kBattleID;
		}
		return kEmporiaID < rhs.kEmporiaID;
	}

	BM::GUID			kEmporiaID;
	BM::GUID			kBattleID;
}SEmporiaBattleKey;

typedef struct tagTBL_EMPORIA_CHALLENGE_BATTLE
:	public SEmporiaChallenge 
{
	tagTBL_EMPORIA_CHALLENGE_BATTLE()
		:	iState(0)
	{}

	BYTE	iState;

	void WriteToPacket( BM::Stream &kPacket )const
	{
		SEmporiaChallenge::WriteToPacket( kPacket );
		kPacket.Push( iState );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		if ( kPacket.RemainSize() >= min_size() )
		{
			if ( true == SEmporiaChallenge::ReadFromPacket( kPacket ) )
			{
				return kPacket.Pop( iState );
			}
		}
		
		return false;
	}

	size_t min_size()const
	{
		return	SEmporiaChallenge::min_size()
			+	sizeof(iState);
	}

	size_t max_size()const
	{
		return	SEmporiaChallenge::max_size()
			+	sizeof(iState);
	}

}TBL_EMPORIA_CHALLENGE_BATTLE;
typedef std::vector<TBL_EMPORIA_CHALLENGE_BATTLE>							CONT_TBL_EMPORIA_CHALLENGE_BATTLE;
typedef std::map< SEmporiaBattleKey, CONT_TBL_EMPORIA_CHALLENGE_BATTLE >	CONT_TBL_EMPORIA_CHALLENGE_BATTLE2;

typedef struct tagTBL_EMPORIA_TOURNAMENT
{
	tagTBL_EMPORIA_TOURNAMENT()
		:	nIndex(0)
	{}

	bool operator < ( tagTBL_EMPORIA_TOURNAMENT const &rhs )const{return nIndex < rhs.nIndex;}
	
	short				nIndex;
	BM::DBTIMESTAMP_EX	kBattleDate;
	BM::GUID			kWinGuildID;
	BM::GUID			kLoseGuildID;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_EMPORIA_TOURNAMENT;
typedef std::set<TBL_EMPORIA_TOURNAMENT>							CONT_TBL_EMPORIA_TOURNAMENT;
typedef std::map< SEmporiaBattleKey, CONT_TBL_EMPORIA_TOURNAMENT >	CONT_TBL_EMPORIA_TOURNAMENT2;


typedef struct tagTBL_EM_MERCENARY_JOINGUILD
{
	typedef std::vector<BM::GUID> CONT_CHARACTER;
	tagTBL_EM_MERCENARY_JOINGUILD()
	{}
	tagTBL_EM_MERCENARY_JOINGUILD(BM::GUID const & rkJoinGuildGuid)
		: kJoinGuildID(rkJoinGuildGuid)
	{}

	bool operator < ( tagTBL_EM_MERCENARY_JOINGUILD const &rhs )const{return kJoinGuildID < rhs.kJoinGuildID;}
	
	BM::GUID kJoinGuildID;
	CONT_CHARACTER kContCharID;

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(kJoinGuildID);
		kPacket.Push(kContCharID);
	}
	bool ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kJoinGuildID);
		kPacket.Pop(kContCharID);
	}
	size_t min_size()const
	{
		return 
			sizeof(kJoinGuildID)+
			sizeof(size_t);
	}
}TBL_EM_MERCENARY_JOINGUILD;
typedef std::set< TBL_EM_MERCENARY_JOINGUILD >							CONT_TBL_EM_MERCENARY_JOINGUILD;

typedef struct tagTBL_EM_MERCENARY_INDEX
{
	tagTBL_EM_MERCENARY_INDEX(short _in=0)
		:	nIndex(_in)
	{}

	bool operator < ( tagTBL_EM_MERCENARY_INDEX const &rhs )const{return nIndex < rhs.nIndex;}
	
	short nIndex;
	CONT_TBL_EM_MERCENARY_JOINGUILD kContJoinGuild;

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(nIndex);
		kPacket.Push(kContJoinGuild);
	}
	bool ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(nIndex);
		kPacket.Pop(kContJoinGuild);
	}
	size_t min_size()const
	{
		return 
			sizeof(nIndex)+
			sizeof(size_t);
	}
}TBL_EM_MERCENARY_INDEX;
typedef std::set< TBL_EM_MERCENARY_INDEX >							CONT_TBL_EM_MERCENARY_INDEX;
typedef std::map< SEmporiaBattleKey, CONT_TBL_EM_MERCENARY_INDEX >	CONT_TBL_EM_MERCENARY;


struct SEmporiaChallenge_Sorter
{
	bool operator()( SEmporiaChallenge const &_Left, SEmporiaChallenge const &_Right) const
	{	// apply operator> to operands
		if ( _Left.i64Cost == _Right.i64Cost )
		{
			return (_Left.i64ChallengeTime < _Right.i64ChallengeTime);
		}
		return (_Left.i64Cost > _Right.i64Cost);
	}
};

typedef struct tagEmporiaHaveInfo
{
	tagEmporiaHaveInfo()
		:	bOwner(false)
	{}

	explicit tagEmporiaHaveInfo( SEmporiaKey const &_kEmporiaKey, SEmporiaGuildInfo const &_kGuildInfo )
		:	kEmporiaKey(_kEmporiaKey)
		,	kGuildInfo(_kGuildInfo)
		,	bOwner( _kEmporiaKey.byGrade > 0 )
	{}

	explicit tagEmporiaHaveInfo( SEmporiaGuildInfo const &_kGuildInfo, BM::GUID const &kEmporiaID )
		:	kGuildInfo(_kGuildInfo)
		,	kEmporiaKey(kEmporiaID,0)
		,	bOwner(false)
	{
	}

	void WriteToPacket( BM::Stream &rkPacket )const
	{
		rkPacket.Push( kEmporiaKey );
		kGuildInfo.WriteToPacket( rkPacket );
		rkPacket.Push( bOwner );
	}

	bool ReadFromPacket( BM::Stream &rkPacket )
	{
		if ( rkPacket.RemainSize() < min_size() )
		{
			return false;
		}

		rkPacket.Pop( kEmporiaKey );
		if ( true == kGuildInfo.ReadFromPacket( rkPacket ) )
		{
			return rkPacket.Pop( bOwner );
		}
		return false;
	}

	size_t min_size()const
	{
		return	sizeof(kEmporiaKey)
			+	sizeof(bOwner)
			+	kGuildInfo.min_size();
	}

	size_t max_size()const
	{
		return	sizeof(kEmporiaKey)
			+	sizeof(bOwner)
			+	kGuildInfo.max_size();
	}

	SEmporiaKey			kEmporiaKey;
	SEmporiaGuildInfo	kGuildInfo;
	bool				bOwner;
}SEmporiaHaveInfo;
typedef std::map< BM::GUID, SEmporiaHaveInfo >	CONT_EMPORIA_FROMGUILD;// first: GuildID

typedef struct tagEmporiaBattleGroundInfo
{
	tagEmporiaBattleGroundInfo( int const _iGroundNo=0, EPVPTYPE const _kGameType=PVP_TYPE_NONE )
		:	iGroundNo(_iGroundNo)
		,	kGameType(_kGameType)
	{}

	int			iGroundNo;
	EPVPTYPE	kGameType;
}SEmporiaBattleGroundInfo;


#pragma pack()

class PgEmporia
{

public:
	PgEmporia(void);
	explicit PgEmporia( BM::GUID const &kID, BYTE const byGrade );
	explicit PgEmporia( SEmporiaKey const &kKey, SEmporiaGuildInfo const &kOwner, BYTE const byGate, BM::DBTIMESTAMP_EX const &kHaveDate );
	PgEmporia( PgEmporia const & );
	~PgEmporia(void);

	PgEmporia& operator = ( PgEmporia const &rhs );

	void Init( SEmporiaKey const &kKey, SEmporiaGuildInfo const &kOwner, BYTE const byGate, BM::DBTIMESTAMP_EX const &kHaveDate );

	void WriteToPacket( BM::Stream &kPacket, bool const bSimple = true  )const;
	bool ReadFromPacket( BM::Stream &kPacket );

	void WriteToPacket_Function( BM::Stream &kPacket )const{m_kFunction.WriteToPacket(kPacket);}
	void ReadFromPacket_Function( BM::Stream &kPacket ){m_kFunction.ReadFromPacket(kPacket);}

	bool Open( bool const bOpen );
	bool SetGate( BYTE const byGate );
	BYTE GetGate()const{return m_byGate;}

	bool IsAccess( BM::GUID const &kGuildID )const;

	bool SetOwner( SEmporiaGuildInfo const &kOwner );
	bool GetOwner( SEmporiaGuildInfo &rkOwner )const;
	bool IsOwner( BM::GUID const &kGuildID )const;
	bool IsHaveOwner(void)const{return IsOpen() && !m_kOwner.IsEmpty();}

	SEmporiaKey const &GetKey(void)const{return m_kKey;}
	BM::GUID const &GetOwnerGuildID(void)const{return m_kOwner.kGuildID;}
	bool IsOpen(void)const{return m_byGate!=EMPORIA_GATE_CLOSE;}
	BM::DBTIMESTAMP_EX const &GetHaveDate(void){return m_kHaveDate;}

	SGroundKey const& GetEmporiaGroundKey(void)const{return m_kEmporiaGroundKey;}
	void SetEmporiaGroundKey(SGroundKey const &kGndKey){m_kEmporiaGroundKey=kGndKey;}

	PgEmporiaFunction const &GetFunc(void)const{return m_kFunction;}
	PgEmporiaFunction& GetFunc(void){return m_kFunction;}

	static int GetRewardAchievementNo(int const iIdx, bool bIsTournament);

protected:
	SEmporiaKey			m_kKey;
	SEmporiaGuildInfo	m_kOwner;
	BYTE				m_byGate;
	BM::DBTIMESTAMP_EX	m_kHaveDate;

	SGroundKey			m_kEmporiaGroundKey;// no writetopacket
	SGroundKey			m_kEmporiaBattleGroundKey;;// no writetopacket

	PgEmporiaFunction	m_kFunction;
};

inline bool PgEmporia::IsOwner( BM::GUID const &kGuildID )const
{
	return !m_kOwner.IsEmpty() && (m_kOwner.kGuildID == kGuildID);
}

#endif // WEAPON_VARIANT_GUILD_PGEMPORIA_H