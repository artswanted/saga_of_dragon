#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPSETTING_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPSETTING_H

typedef enum
{
	PVP_TIME_READY,
	PVP_TIME_ROUNDTERM,
	PVP_TIME_REVIVE,
	PVP_TIME_RESULT,
	PVP_TIME_ENTRYSELECT,
	PVP_TIME_EVENT_SCENE_RESULT,	// 라운드가 끝났을 때 연출 씬이 있을 때 연출 시간 만큼 대기.
}EPvPTime;

typedef enum
{
	PVP_ST_NONE					= 0,

	PVP_ST_KILL_POINT			= 1,
	PVP_ST_KILL_POINT_WEIGHT	= 2,
	PVP_ST_DEATH_POINT			= 3,
	PVP_ST_DEATH_POINT_WEIGHT	= 4,

	PVP_ST_RANK_POINT_CHK		= 10,
	PVP_ST_RANK_01_POINT			,
	PVP_ST_RANK_02_POINT			,
	PVP_ST_RANK_03_POINT			,
	PVP_ST_RANK_04_POINT			,
	PVP_ST_RANK_05_POINT			,
	PVP_ST_RANK_06_POINT			,
	PVP_ST_RANK_07_POINT			,
	PVP_ST_RANK_08_POINT			,
	PVP_ST_RANK_09_POINT			,
	PVP_ST_RANK_10_POINT			,

	PVP_ST_INSITEM_POINT		= 30,
	PVP_ST_INSITEM_BOMBERMAN	= 31,

	PVP_ST_LOVE_MODE_DROP_RED_BEAR	= 40,
	PVP_ST_LOVE_MODE_DROP_BLUE_BEAR	= 41,

	PVP_ST_CAPTURE_DEC_VALUE = 50,

	PVP_ST_DESTROYFENCE		= 60,
	PVP_ST_BEARUSERKILL		= 61,
	PVP_ST_GETROUNDSCORE	= 62,
	PVP_ST_PICKUPBEAR		= 63,

	PVP_ST_WIN_ROUND_POINT		= 101,
	PVP_ST_WIN_GAME_POINT		= 102,
	PVP_ST_LOSE_ROUND_POINT		= 103,
	PVP_ST_LOSE_GAME_POINT		= 104,

	PVP_ST_TIME_READY				= 108,
	PVP_ST_OCCUPY_POINT				= 109,
	PVP_ST_VICTORY_INTERVAL_TIME	= 110,
	PVP_ST_NEXT_EXP_ENTITY_LV0		= 111,
	PVP_ST_NEXT_EXP_ENTITY_LV1			 ,
	PVP_ST_NEXT_EXP_ENTITY_LV2			 ,
	PVP_ST_NEXT_EXP_ENTITY_LV3			 ,
	PVP_ST_NEXT_EXP_ENTITY_LV4			 ,
	PVP_ST_DEFAULT_ENTITY_EFFECT	= 120,
	PVP_ST_OCCUPY_POINT_NOT_IDLE_RATE	= 121,	//Idle상태가 아닌 경우 적용될 확률
}EPvPSetting;

typedef enum
{
	PVP_SPT_NONE				= 0x00,
	PVP_SPT_DEATH				= 0x01,
	PVP_SPT_KILL				= 0x02,
	PVP_SPT_MAX					= 0x03,
}EPvPSpecialType;

class PgPvPSetting
{
public:
	struct SKey
	{
		SKey( EPVPTYPE const _kType, WORD const _kSet=PVP_ST_NONE )
			:	kType(_kType)
			,	kSet(_kSet)
		{

		}

		bool operator<( SKey const &rhs )const
		{
			if ( kType == rhs.kType ){return kSet < rhs.kSet;}
			return kType < rhs.kType;
		}
		EPVPTYPE	kType;
		WORD		kSet;
	};

	struct SSpecial
	{
		SSpecial( EPVPTYPE const _kType, BYTE const _kSType=PVP_SPT_NONE, WORD const _usCount=0, WORD const _usPoint=0 )
			:	kType(_kType)
			,	kSType(_kSType)
			,	usCount(_usCount)
			,	usPoint(_usPoint)
		{}

		bool operator<( SSpecial const &rhs )const
		{
			if ( kType == rhs.kType )
			{
				if ( kSType == rhs.kSType )
				{
					switch ( kSType )
					{
					case PVP_SPT_DEATH:{return usCount < rhs.usCount;}
					case PVP_SPT_KILL:{return usCount > rhs.usCount;/*꺼꾸로*/ }
					}
					return usCount < rhs.usCount;
				}
				return kSType < rhs.kSType;
			}
			return kType < rhs.kType;
		}
		bool operator>( SSpecial const &rhs )const
		{
			return rhs < *this;
		}
		bool operator==( SSpecial const &rhs )const
		{
			return (kType == rhs.kType) && (kSType == rhs.kSType) && (usCount == rhs.usCount);
		}

		EPVPTYPE	kType;
		BYTE		kSType;
		WORD		usCount;
		WORD		usPoint;
	};

	typedef std::map< SKey, int >		CONT_SETTING;
	typedef std::vector<SSpecial>		CONT_SPECIAL;

public:
	PgPvPSetting()
		:	m_dwReadyTime(5000)
		,	m_dwResultTime(25000)
		,	m_dwReviveTime(5000)
		,	m_dwRoundTermTime(10000)
		,	m_dwEntrySelectTime(30000)
		,	m_dwEventSceneResult(10000)
	{
		m_kContSpecial.reserve(4);
		m_kContSpecial.push_back( SSpecial(PVP_TYPE_DM) );
		m_kContSpecial.push_back( SSpecial(PVP_TYPE_KTH) );
		m_kContSpecial.push_back( SSpecial(PVP_TYPE_ANNIHILATION) );
		m_kContSpecial.push_back( SSpecial(PVP_TYPE_DESTROY) );
	}
	virtual ~PgPvPSetting(){}

	DWORD GetTime( EPvPTime const kTimeType )const
	{
		switch ( kTimeType )
		{
		case PVP_TIME_READY:{return m_dwReadyTime;}break;
		case PVP_TIME_ROUNDTERM:{return m_dwRoundTermTime;}break;			
		case PVP_TIME_REVIVE:{return m_dwReviveTime;}break;
		case PVP_TIME_RESULT:{return m_dwResultTime;}break;
		case PVP_TIME_ENTRYSELECT:{return m_dwEntrySelectTime;}break;
		case PVP_TIME_EVENT_SCENE_RESULT: { return m_dwEventSceneResult; }break;
		}
		assert(false);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	void SetTime( EPvPTime const kTimeType, DWORD const dwValue )
	{
		switch ( kTimeType )
		{
		case PVP_TIME_READY:{m_dwReadyTime=dwValue;}break;
		case PVP_TIME_ROUNDTERM:{m_dwRoundTermTime=dwValue;}break;			
		case PVP_TIME_REVIVE:{m_dwReviveTime=dwValue;}break;
		case PVP_TIME_RESULT:{m_dwResultTime=dwValue;}break;
		case PVP_TIME_ENTRYSELECT:{m_dwEntrySelectTime=dwValue;}break;
		case PVP_TIME_EVENT_SCENE_RESULT: { m_dwEventSceneResult = dwValue; }break;
		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
				assert(false);
			}
		}
	}

	int Get( EPVPTYPE const kType, WORD const kSet )const
	{
		CONT_SETTING::const_iterator itr = m_kContSetting.find( SKey(kType,kSet) );
		if ( itr != m_kContSetting.end() )
		{
			return itr->second;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	bool Set( EPVPTYPE const kType, WORD const kSet, int const iValue )
	{
		auto kPair = m_kContSetting.insert( std::make_pair( SKey(kType,kSet), iValue ) );
		return kPair.second;
	}

	bool SetSpecial( EPVPTYPE const kType, BYTE const kSType, WORD const usCount, WORD const usPoint )
	{
		if ( PVP_SPT_MAX > kSType )
		{
			m_kContSpecial.push_back( SSpecial(kType, kSType, usCount, usPoint ) );
			std::unique( m_kContSpecial.begin(), m_kContSpecial.end() );
			std::sort( m_kContSpecial.begin(), m_kContSpecial.end() );
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	HRESULT GetSpecial( EPVPTYPE const kType, CONT_PVP_GAME_REWARD::value_type &kRet )const
	{
		if ( kRet.kSpecialPoint )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__<<L"Error kRet" );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		BYTE kApply = PVP_SPT_NONE;
		CONT_SPECIAL::const_iterator itr = std::lower_bound( m_kContSpecial.begin(), m_kContSpecial.end(), SSpecial(kType) );
		while ( itr != m_kContSpecial.end() )
		{
			if ( itr->kType != kType )
			{
				break;
			}

			if ( !(itr->kSType & kApply) )
			{
				switch( itr->kSType )
				{
				case PVP_SPT_DEATH:
					{
						if ( itr->usCount >= kRet.kResult.usDieCount )
						{
							// 혹시나 Overflow 발생할까봐 검사하는 코드 추가
							WORD wTemp = kRet.kSpecialPoint;
							kRet.kSpecialPoint = kRet.kSpecialPoint + (itr->usPoint);
							if (wTemp > kRet.kSpecialPoint)
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"[PVP_SPT_DEATH] kSpecialPoint overflow ["<<wTemp<<L"]->["<<kRet.kSpecialPoint<<L"]");
							}
							kApply |= itr->kSType;
						}
					}break;
				case PVP_SPT_KILL:
					{
						if ( itr->usCount <= kRet.kResult.usKillCount )
						{
							// 혹시나 Overflow 발생할까봐 검사하는 코드 추가
							WORD wTemp = kRet.kSpecialPoint;
							kRet.kSpecialPoint = kRet.kSpecialPoint + (itr->usPoint);
							if (wTemp > kRet.kSpecialPoint)
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"[PVP_SPT_DEATH] kSpecialPoint overflow ["<<wTemp<<L"]->["<<kRet.kSpecialPoint<<L"]");
							}
							kApply |= itr->kSType;
						}
					}break;
				}
			}
			++itr;
		}
		return S_OK;
	}

protected:
	CONT_SETTING	m_kContSetting;
	CONT_SPECIAL	m_kContSpecial;

	DWORD			m_dwReadyTime;
	DWORD			m_dwResultTime;
	DWORD			m_dwReviveTime;
	DWORD			m_dwRoundTermTime;
	DWORD			m_dwEntrySelectTime;
	DWORD			m_dwEventSceneResult;
};

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPSETTING_H