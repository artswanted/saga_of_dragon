#ifndef WEAPON_LOHENGRIN_PACKET_PACKETSTRUCTLOG_H
#define WEAPON_LOHENGRIN_PACKET_PACKETSTRUCTLOG_H

//#define LOG_MODE_ON	// Log Mode를 사용지 Define(LogTable/Def_LogType)
typedef enum
{
	LOG_NONE							= 0,
	LOG_SYSTEM							= 10000,	// 서버 시스템 관련 로그
	LOG_SYSTEM_ACCEPT					= 10001,	// 로그서버접속
	LOG_SYSTEM_SERVICESTART				= 10002,	// 서비스 시작
	LOG_SYSTEM_SERVER_USERCOUNTER		= 10003,	// 서버 접속자 정보
	LOG_SYSTEM_LOGIN					= 10100,	// 로그인관련
	LOG_SYSTEM_LOGIN_TRYAUTH			= 10101,	// TryAuth
	LOG_SYSTEM_LOGIN_SELECT				= 10102,	// Select Character 
	LOG_SYSTEM_LOGIN_LOGOUT				= 10103,	// LogOut 
	LOG_SYSTEM_LOGIN_USER_INFO			= 10104,	// LogIn User Info
	LOG_SYSTEM_EVENT					= 10500,	// 이벤트 로그
	LOG_SYSTEM_EVENT_START				= 10501,	// 이벤트 시작
	LOG_SYSTEM_EVENT_END				= 10502,	// 이벤트 종료

	LOG_USER							= 20000,	// 유저데이터 LV1
	LOG_USER_CHAR_CREATE				= 20001,	// 케릭터 생성
	LOG_USER_CHAR_DELETE				= 20002,	// 케릭터 삭제
	LOG_USER_HAVEMONEY					= 20003,	// 돈획득
	LOG_USER_LVUP						= 20004,	// 레벨업
	LOG_USER_ADDEXP						= 20005,	// 경험치 획득
	LOG_USER_JOB_CHANGE					= 20006,	// 전직
	LOG_USER_GET_SKILL					= 20007,	// 스킬 습득
	LOG_USER_ADDFRAN					= 20008,	// Fran
	LOG_USER_DIE						= 20009,	// 유저 사망 
	LOG_USER_ALIVE						= 20010,	// 유저 부활

	LOG_USER_MAIL_SEND					= 20011,	// 메일 전송
	LOG_USER_MAIL_READ					= 20012,	// 메일 읽기
	LOG_USER_MAIL_RETURN				= 20013,	// 메일 반송
	LOG_USER_MAIL_DELETE				= 20014,	// 메일 삭제
	LOG_USER_MAIL_GET_ANNEX				= 20015,	// 첨부 물품 수령

	LOG_USER_MAPMOVE					= 20016,	// 맵이동

	LOG_CONTENTS						= 30000,	// 컨텐츠 로그
	LOG_CONTENTS_QUEST					= 30100,	// 퀘스트
	LOG_CONTENTS_QUEST_START			= 30101,	// 퀘스트 시작
	LOG_CONTENTS_QUEST_FLAG				= 30102,	// 퀘스트 플래그 변견
	LOG_CONTENTS_QUEST_END				= 30103,	// 퀘스트 완료
	LOG_CONTENTS_PARTY					= 30200,	// 파티
	LOG_CONTENTS_PARTY_CREATE			= 30201,	// 파티 생성
	LOG_CONTENTS_PARTY_ADD				= 30202,	// 파티 가입
	LOG_CONTENTS_PARTY_LEAVE			= 30203,	// 파티 탈퇴
	LOG_CONTENTS_PARTY_PURGE			= 30204,	// 파티 추방
	LOG_CONTENTS_PARTY_DELETE			= 30205,	// 파티 삭제
	LOG_CONTENTS_PARTY_CHANGE_INFO		= 30206,	// 파티 정보 수정
	LOG_CONTENTS_GUILD					= 30300,	// 길드
	LOG_CONTENTS_GUILD_CREATE			= 30301,	// 길드 생성
	LOG_CONTENTS_GUILD_DELETE			= 30302,	// 길드 삭제
	LOG_CONTENTS_GUILD_ADD				= 30303,	// 길드 가입
	LOG_CONTENTS_GUILD_CHANGE_MASTER	= 30304,	// 길드장 위임
	LOG_CONTENTS_GUILD_SET_SUBMASTER	= 30305,	// 부길마 임명
	LOG_CONTENTS_GUILD_CHANGE_INFO		= 30306,	// 길드 정보 수정
	LOG_CONTENTS_GUILD_NOTICE			= 30307,	// 길드 공지
	LOG_CONTENTS_GUILD_CHANGE_LEVEL		= 30308,	// 길드 레벨 변경
	LOG_CONTENTS_GUILD_ADD_EXP			= 30309,	// 길드 경험치 획득
	LOG_CONTENTS_FRIEND					= 30400,	// 친구
	LOG_CONTENTS_FRIEND_REQUEST			= 30401,	// 친구 요청
	LOG_CONTENTS_FRIEND_ADD				= 30402,	// 친구 추가
	LOG_CONTENTS_FRIEND_DELETE			= 30403,	// 친구 삭제
	LOG_CONTENTS_FRIEND_CUT				= 30404,	// 친구 차단
	LOG_CONTENTS_FRIEND_NONE_CUT		= 30405,	// 친구 차단 해제
	LOG_CONTENTS_FRIEND_REFUSE			= 30406,	// 친구 거절
	LOG_CONTENTS_FRIEND_MAKE_CUPPLE		= 30407,	// 커플 맺기
	LOG_CONTENTS_FRIEND_CUT_CUPPLE		= 30408,	// 커플 떠나기
	LOG_CONTENTS_CHATTING				= 30500,	// 채팅
	LOG_CONTENTS_CHATTING_NOMAL			= 30501,	// 일반
	LOG_CONTENTS_CHATTING_WISPER		= 30502,	// 귓속말
	LOG_CONTENTS_CHATTING_PARTY			= 30503,	// 파티
	LOG_CONTENTS_CHATTING_GUILD			= 30504,	// 길드
	LOG_CONTENTS_CHATTING_FRIEND		= 30505,	// 친구채팅
	LOG_CONTENTS_CHATTING_SHOUT			= 30506,	// 외치기(그라운드)
	LOG_CONTENTS_CHATTING_PVP_ROOM		= 30507,	// PVP방 채팅
	LOG_CONTENTS_CHATTING_TEAMCHAT		= 30508,	// 팀채팅
	LOG_CONTENTS_CHATTING_MANTOMAN		= 30509,	// 1대 1 채팅 (친구 메뉴를 통한)
	LOG_CONTENTS_CHATTING_SHOUT_CHANNEL	= 30510,	// 외치기(채널)
	LOG_CONTENTS_CHATTING_SHOUT_REALM	= 30511,	// 외치기(렐름)
	LOG_CONTENTS_MAP					= 30600,	// MAP
	LOG_CONTENTS_MAP_CREATE				= 30601,	// Ground 생성/삭제
	LOG_CONTENTS_MAP_IN_MONSTER_INFO	= 30602,	// Ground 내 몬스터 정보
	LOG_CONTENTS_MAP_IN_USER_INFO		= 30603,	// Ground 접속자 수 정보
	LOG_CONTENTS_MAP_MISSION_START		= 30604,	// 미션 시작(캐릭터 관점)
	LOG_CONTENTS_MAP_MOSSION_END		= 30605,	// 미션 종료(캐릭터 관점)
	LOG_CONTENTS_PVP					= 30700,	// PVP
	LOG_CONTENTS_PVP_ROUND_START		= 30701,	// PVP 라운드 시작
	LOG_CONTENTS_PVP_ROUND_END			= 30702,	// PVP 라운드 종료
	LOG_CONTENTS_PVP_GAME_START			= 30703,	// PVP 게임 시작
	LOG_CONTENTS_PVP_GAME_END			= 30704,	// PVP 게임 종료
	LOG_CONTENTS_PVP_RESULT				= 30705,	// PVP 개인 결과
	LOG_CONTENTS_PVP_USE_COUNT			= 30706,	// PVP 이용자수

	LOG_ITEM							= 40000,	// 아이템
	LOG_ITEM_BUY						= 40001,	// 아이템 구입
	LOG_ITEM_SEL						= 40002,	// 아이템 판매
	LOG_ITEM_DROP						= 40003,	// 아이템 드롭
	LOG_ITEM_GET						= 40004,	// 아이템 습득
	LOG_ITEM_DELETE						= 40005,	// 아이템 삭제
	LOG_ITEM_EQUIP						= 40006,	// 아이템 장착
	LOG_ITEM_UNEQUIP					= 40007,	// 아이템 장착 해제
	LOG_ITEM_MOVE_BANK					= 40008,	// 아이템 은행에 보관
	LOG_ITEM_MOVE_INVEN					= 40009,	// 아이템 아이템 찾기
	LOG_ITEM_EXCHANGE					= 40010,	// 아이템 교환
	LOG_ITEM_CREATIVE					= 40011,	// 아이템 제작
	LOG_ITEM_CREATEVE_USE_DELETE		= 40012,	// 아이템 제작 소모
	LOG_ITEM_EXPERT						= 40013,	// 아이템 감정

	LOG_CASH							= 90000,	// 쿠폰 및 캐쉬 정보
	LOG_CHSH_CHARGE						= 90001,	// 캐쉬 충전 내역
	LOG_CHSH_USE						= 90002,	// 캐쉬 사용 내역
	LOG_CHSH_EXCHANGE_ITEM				= 90003,	// 물품 구매 내역(캐쉬 사용)
	LOG_COUPON							= 90100,	// 쿠폰 내역
	LOG_COUPON_EXCHANGE					= 90101,	//  쿠폰 교환 내역
}ELogType;

//LogD연동 관련 Log_ID
typedef enum eNcLogD
{
	LOG_NC_NONE							= 0,
	LOG_NC_CHAT_SHOUT					= 1,
	LOG_NC_CHAT_GENERAL					= 2,
	LOG_NC_CHAT_WHISPER					= 3,
	LOG_NC_CHAT_PARTY					= 4,
	LOG_NC_CHAT_GUILD					= 5,
	LOG_NC_SERVER_START					= 101,
	LOG_NC_SERVER_END					= 102,
	LOG_NC_SERVER_STATE					= 103,
	LOG_NC_CCU_ALL						= 201,
	LOG_NC_CCU_CHANNEL					= 202,
	LOG_NC_USER_LOGIN					= 1001,
	LOG_NC_USER_LOGOUT					= 1002,
	LOG_NC_CHARACTER_LOGIN				= 1021,
	LOG_NC_CHARACTER_LOGOUT				= 1022,
	LOG_NC_CHARACTER_CREATE				= 2001,
	LOG_NC_CHARACTER_DELETE				= 2002,
	LOG_NC_CHARACTER_CHANGE_NAME		= 2003,
	LOG_NC_CHARACTER_LEVEL_UP			= 2004,
	LOG_NC_CHARACTER_CHANGE_JOB			= 2005,
	LOG_NC_CHARACTER_BASE_INFO			= 2008,
	LOG_NC_CHARACTER_QUEST_INFO			= 2009,
	LOG_NC_ITEM_EQUIP					= 3006,
	LOG_NC_ITEM_UNEQUIP					= 3007,
	LOG_NC_ITEM_GIFT					= 3008,
	LOG_NC_ITEM_DROP					= 3009,
	LOG_NC_ITEM_GET						= 3010,
	LOG_NC_ITEM_BUY_YP					= 3011,
	LOG_NC_ITEM_EXCHANGE_YP				= 3012,
	LOG_NC_ITEM_KEEP					= 3013,
	LOG_NC_ITEM_TAKEOUT					= 3014,
	LOG_NC_ITEM_DELETE					= 3015,
	LOG_NC_ITEM_EXCHANGE				= 3016,
	LOG_NC_ITEM_ENCHANT					= 3017,
	LOG_NC_ITEM_ENCHANT_CONSUME			= 3018,
	LOG_NC_MISSION_START				= 4001,
	LOG_NC_MISSION_END					= 4002,
	LOG_NC_MISSION_MOVE					= 4003,
	LOG_NC_MAP_MOVE						= 4004,
	LOG_NC_SKILL_GET					= 4005,
	LOG_NC_SKILL_USE					= 4006,
	LOG_NC_SKILL_USE_CANCEL				= 4007,
	LOG_NC_SKILL_DELETE					= 4008,
	LOG_NC_PARTY_MAKE					= 4009,
	LOG_NC_PARTY_JOIN					= 4010,
	LOG_NC_PARTY_DELETE					= 4011,
	LOG_NC_PARTY_LEAVE					= 4012,
	LOG_NC_PARTY_BANISH					= 4013,
	LOG_NC_PARTY_CHANGE_LEADER			= 4014,
	LOG_NC_PVP_START					= 7003,
	LOG_NC_PVP_END						= 7004,
	LOG_NC_QUEST_START					= 8001,
	LOG_NC_QUEST_END					= 8002,
	LOG_NC_QUEST_REPAY					= 8003,
	LOG_NC_QUEST_PROGRESS_INFO			= 8004,
	LOG_NC_QUEST_DELETE_ITEM			= 8005,
	LOG_NC_CHARACTER_KILL				= 20001,
	LOG_NC_CHARACTER_DIE				= 20002,
	LOG_NC_CHARACTER_ALIVE				= 20004,
	LOG_NC_CHARACTER_HIT_INFO			= 20030,
}ELogNcType;

typedef enum eLogUserDieType // DB 사용값이므로 숫자를 변경 하지 말것
{
	LUDT_Unknown		= 0, // 불명확하다(pkCaster가 NULL 일때)
	LUDT_Monster		= 1, // 몬스터가 죽였다
	LUDT_BossMonster	= 2, // 보스 몬스터가 죽였다
	LUDT_Player			= 3, // 플레이어가 죽였다.
	LUDT_Entity			= 4, // 엔티티(소환물)이 죽였다.
	LUDT_Trap			= 5, // 트랩이 죽였다.
	LUDT_Summoned		= 6, // 소환물이 죽였다.
} ELogUserDieType;

typedef enum eLogUserResurrectType // DB 사용값이므로 숫자를 변경 하지 말것
{
	LURT_Self			= 0, // 부활 시켜준 사람이 없을 때
	LURT_Player			= 1, // 플레이어가
	LURT_Entity			= 2, // 엔티티(소환물)이
	LURT_Monster		= 3, // 몬스터가(?)
	LURT_BossMonster	= 4, // 보스몬스터가(?)
	LURT_MapMove		= 5, // 맵이동으로 살아났다(가장 최근맵으로 이동)
	LURT_Item			= 6, // 아이템으로 살아났다(예: 피닉스의 깃털)
	LURT_Gold			= 7, // 골드로 살아났다(예: 미션 골드 부활기능)
	LURT_GMCommand		= 8, // GM Command로 살아났다
	LURT_WarGround		= 9, // 공성전 관련으로 살아났다
	LURT_Macro			= 10,// 매크로 사용 유저 채크에서 올바른 암호 입력시
	LURT_BattleSquare	= 11,
	LURT_MissionGround	= 12,
} ELogUserResurrectType;

/*
#pragma pack(1)

typedef struct tagLogFormat
{
	tagLogFormat(int const iType=LOG_NONE, BM::GUID const &rkMemGuid=BM::GUID::NullData(), BM::GUID const &rkCharGuid=BM::GUID::NullData())
	{
		Clear();
		iLogType = iType;
		kMemberKey = rkMemGuid;
		kCharacterKey = rkCharGuid;
	}

	void Clear()
	{
		iLogType = 0;
		kMemberKey.Clear();
		kCharacterKey.Clear();
		strName.clear();
		usChannelNo = 0;

		::memset( iValue, 0, sizeof(iValue) );
		::memset( i64Value, 0, sizeof(i64Value) );
		for( int i=0; i < 4; ++i )
			strMessage[i].clear();
		for(int i = 0; i < 5; ++i)
			guidValue[i].Clear();
	}

	int				iLogType;
	BM::GUID		kMemberKey;
	BM::GUID		kCharacterKey;
	std::wstring	strID;
	std::wstring	strName;
	unsigned short	usServerNo;
	unsigned short	usChannelNo;
	std::wstring	strMessage[4];
	int				iValue[5];
	__int64			i64Value[5];
	BM::GUID		guidValue[5];
}SLogFormat;
typedef std::vector<SLogFormat>	ConLogFormat;

#pragma pack()
*/

class PgLogWrapper
{
public:
	typedef enum 
	{
		MAX_ARRAY_INT = 6,
		MAX_ARRAY_INT64 = 5,
		MAX_ARRAY_GUID = 5,
		MAX_ARRAY_STRING = 4,
		
		MAX_STRING_NVARCHAR = 199,//디비에 선언된거 -1 해야함 Terminate String 때문
	}E_VALUE;

public:
	explicit PgLogWrapper(ELogType const eInType = LOG_NONE, BM::GUID const &kInMemberGuid = BM::GUID::NullData(), BM::GUID const &kCharGuid = BM::GUID::NullData())
	{
		LogType(eInType);
		MemberKey(kInMemberGuid);
		CharacterKey(kCharGuid);
		ServerNo(0);
		ChannelNo(0);
	}
	~PgLogWrapper(){}
public:
	template< typename T1 >
	void Push( T1 const &kData){}

	template<  >
	void Push( int const &kData){m_kContInt.push_back(kData);}

	template<  >
	void Push( __int64 const &kData){m_kContInt64.push_back(kData);}
	
	template<  >
	void Push( BM::GUID const &kData){m_kContGuid.push_back(kData);}

	template<  >
	void Push( std::wstring const &kData){m_kContString.push_back(kData);}

	template< typename T1 >
	void Pop( T1 &kData){}

	template<  >
	void Pop(int &kData)
	{
		kData = 0;
		if(m_kContInt.size())
		{
			kData = m_kContInt.front();
			m_kContInt.pop_front();
		}
	}

	template<  >
	void Pop( __int64 &kData)
	{
		kData = 0;
		if(m_kContInt64.size())
		{
			kData = m_kContInt64.front();
			m_kContInt64.pop_front();
		}
	}
	
	template<  >
	void Pop( BM::GUID &kData)
	{
		kData.Clear();
		if(m_kContGuid.size())
		{
			kData = m_kContGuid.front();
			m_kContGuid.pop_front();
		}
	}

	template<  >
	void Pop( std::wstring &kData)
	{
		kData.clear();
		if(m_kContString.size())
		{
			kData = m_kContString.front();
			kData.resize( __min(kData.size(), MAX_STRING_NVARCHAR) );//nvarchar 200 이기 때문.
			m_kContString.pop_front();
		}
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(m_kLogType);
		kPacket.Push(m_kMemberKey);
		kPacket.Push(m_kCharacterKey);
		kPacket.Push(m_kID);
		kPacket.Push(m_kName);
		kPacket.Push(m_kServerNo);
		kPacket.Push(m_kChannelNo);

		kPacket.Push(m_kContInt);
		kPacket.Push(m_kContInt64);
		kPacket.Push(m_kContGuid);
		kPacket.Push(m_kContString);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_kLogType);
		kPacket.Pop(m_kMemberKey);
		kPacket.Pop(m_kCharacterKey);
		kPacket.Pop(m_kID);
		kPacket.Pop(m_kName);
		kPacket.Pop(m_kServerNo);
		kPacket.Pop(m_kChannelNo);

		kPacket.Pop(m_kContInt);
		kPacket.Pop(m_kContInt64);
		kPacket.Pop(m_kContGuid);
		kPacket.Pop(m_kContString);
	}

	bool IsCorrect()const
	{
		if(m_kContInt.size() > MAX_ARRAY_INT){return false;}
		if(m_kContInt64.size() > MAX_ARRAY_INT64){return false;}
		if(m_kContGuid.size() > MAX_ARRAY_GUID){return false;}
		if(m_kContString.size() > MAX_ARRAY_STRING){return false;}
		
		return true;
	}

	bool Send();
protected:
	CLASS_DECLARATION_S(ELogType, LogType);
	CLASS_DECLARATION_S(BM::GUID, MemberKey);
	CLASS_DECLARATION_S(BM::GUID, CharacterKey);
	CLASS_DECLARATION_S(std::wstring,	ID);
	CLASS_DECLARATION_S(std::wstring,	Name);
	CLASS_DECLARATION_S(short, ServerNo);
	CLASS_DECLARATION_S(short, ChannelNo);

protected:
	std::list< int > m_kContInt;
	std::list< __int64 > m_kContInt64;
	std::list< BM::GUID > m_kContGuid;
	std::list< std::wstring > m_kContString;
};

#endif // WEAPON_LOHENGRIN_PACKET_PACKETSTRUCTLOG_H