#ifndef WEAPON_VARIANT_BASIC_CONSTANT_GM_CONST_H
#define WEAPON_VARIANT_BASIC_CONSTANT_GM_CONST_H

#include "Lohengrin/packetstruct.h"


// Error Code
typedef enum : int	//값이나 이름 바꿀경우 해외업체에 꼭 통보 해야함.
{//안쓰면 탭.
	GE_SUCCESS				=0x00000000,	// success
	GE_INCORRECT_ID			=0x10000001,	// 
	GE_WORNG_ID_OR_PW		=0x10000002,
	GE_NOT_ONLINE			=0x10000003,	// not online state
//	GE_NOT_ACCESS_GRADE		=0x10000004,	// Use Not PetitionTool
//	GE_TOO_LONG_STRING		=0x10000005,	// String is too long
	GE_NOT_IMPLEMENTED		=0x10000006,	// Not implemented
	GE_SYSTEM_ERR			=0x10000007,
	GE_TIMEOUT				=0x10000008,	// Time out
	GE_ALREADY_EXIST		=0x10000009,	// Already exist user
	GE_NOT_ALLOW_IP			=0x10000010,	// Unpermission IP	
	GE_CLOSE_ACCOUNT		=0x10000011,	//  set account expiration dates
	GE_LOGINED_ID			=0x10000012,

	GE_USING_ID				=0x10001001,	// 
//	GE_USING_NAME			=0x10001002,	// 

	GE_QUERY_FAILED			=0x10001003,
	GE_FORBIDDEN_NAME		=0x10001004,
	GE_DATA_NOT_FOUND		=0x10001005, //데이터를 찾을 수 없습니다.
	GE_UNKNOWN_ERROR		=0x19999999,
}E_GM_ERR;

typedef enum
{
	EACCOUNT_STATE_NONE		= 0,
	EACCOUNT_STATE_NORMAL	= 0,
	EACCOUNT_STATE_BLOCK		= 100,
} EAccountState;

typedef enum
{
	EPASSWORD_SET_NONE		= 0,
	EPASSWORD_SET_CHANGE	= 1,
	EPASSWORD_SET_INIT		= 2,
} EPasswordSetType;

typedef enum
{
	EBUILDERCOMMAND_NONE = 0,
	EBUILDERCOMMAND_ALLSERVER_NOTICE = 1,
	EBUILDERCOMMAND_KICKUSER = 2,
} EBuilderCommand;

typedef enum
{
	ECK_NONE,
	ECK_ORDER_KICK,
	ECK_GMC,
	ECK_CHN,
	ECK_ORDER,
}ECommandKind;

typedef enum : PACKET_ID_TYPE
{
	EGMC_NONE = 0,
//GM Command
	EGMC_KICKUSER,
	EGMC_FREEZEACCOUNT,
	EGMC_SETPINCODE,		//AuxiliaryPassword
	EGMC_CALL_USER,
	EGMC_NOTICE,						//게임 내 공지
	EGMC_MUTE,							//채금
//CM Tool Command
	//PetitionTool
	EGMC_GET_SERVER_LIST,				//Gm서버로 서버(채널) 리스트를 받아온다.
	EGMC_GET_USERLIST,					//현재 센터에 접속중인 유저리스트 전달
	EGMC_ADD_CONNECTION_USER,			//센터 새로 접속하는 유저 정보 전달
	EGMC_GET_GMLIST,					//GM리스트를 받아온다.
	EGMC_GET_PETITIONDATA,				//진정 정보를 얻어온다
	EGMC_MODIFY_PETITIONDATA,			//진정 정보가 수정되었다.
	EGMC_SENDMSG_TO_USER,				//GM->User에게 채팅 메시지전달
	EGMC_SENDMSG_TO_GM,					//USer->GM에게 채팅 메시지 전달
	EGMC_CHAT_END,						//채팅 종료
	EGMC_SEND_SERVER_TIME,				//GM서버 시간 전달
	EGMC_DELETE_GM,						//GM서버에 저장되어 있는 접속 GM유저를 지운다
	//Common
	EGMC_MATCH_GMID,					//DB에 등록 되어있는 올바른 GM ID인가? 아님 일반 유저인가?
	EGMC_CREATE_GM_ACCOUNT,				//계정 생성
	EGMC_CREATE_USER_ACCOUNT,			//계정 생성
	EGMC_CHANGEPASSWORD,				//비밀번호 변경
	//China Only
	EGMC_CHN_ADDCASH,
	EGMC_CHN_GIVEITEM,
	EGMC_CHN_GETCASH,
	EGMC_CHN_CHANGE_BIRTHDAY,
//GMTool Command
	EGMC_TOOL_REQ_ORDER				= 20001,
	EGMC_TOOL_ANS_ORDER				= 20002,
} EGMCommandType;

//Only Nc return value(value +1:Sucesse, value + 2:Faild)
typedef enum
{
	ENC_NONE					= 5000,
	ENC_CHARACTER_INFO			= 5001,		//캐릭터 정보	
	ENC_SERVER_INFO				= 5101,		//서버 정보	
	ENC_CHARACTER_LIST1			= 5103,		//서버내 캐릭터 리스트(삭제 캐릭터 미포함)
	ENC_CHARACTER_LIST2			= 5106,		//서버내 캐릭터 리스트(삭제 캐릭터 포함)
	ENC_EQUIP_ITEM_INFO			= 5109,		//착용 아이템 정보
	ENC_CHARACTER_ITEM_INFO		= 5112,		//캐릭터 아이템 정보
	ENC_ATTACHMENT_ITEM_INFO	= 5115,		//압류 아이템 정보
	ENC_ATTACHMENT_ITEM			= 5118,		//아이템 압류
	ENC_DISATTACHMENT_ITEM		= 5121,		//아이템 압류 해제
	ENC_INGAME_ITEM_INFO		= 5124,		//게임 내 존재 아이템 확인
	ENC_ADD_ITEM				= 5127,		//아이템 추가
	ENC_DEL_ITEM				= 5130,		//아이템 삭제
	ENC_CHARACTE_SKILL_INFO		= 5133,		//캐릭터 스킬 정보 확인
	ENC_INGAME_SKILL_INFO		= 5136,		//게임 내 존재 스킬 확인
	ENC_ADD_SKILL				= 5139,		//스킬 추가
	ENC_SKLL_LEVEL_CHANGE		= 5142,		//스킬 레벨 변경	
	ENC_DEL_SKILL				= 5145,		//스킬 삭제			
	ENC_CHARACTER_QUEST_INFO	= 5148,		//캐릭터 퀘스트 정보 확인
	ENC_INGAME_QUEST_INFO		= 5151,		//게임 내 존재 퀘스트 확인	
	ENC_ADD_QUEST				= 5154,		//퀘스트 추가			
	ENC_CHANGE_QUEST_INFO		= 5157,		//퀘스트 정보 수정	
	ENC_DEL_QUEST				= 5160,		//퀘스트 삭제			
	ENC_CHARACTER_STATE			= 5163,		//캐릭터 능력치 정보	
	ENC_CHARACTER_RESISTANCE	= 5166,		//캐릭터 저항력 정보
	ENC_CHANGE_JOB_LIST			= 5201,		//변경 가능 직업 리스트
	ENC_CAN_USING_NAME			= 5207,		//캐릭터명 중복 확인		
	ENC_CHANGE_CHARACTER_INFO1	= 5204,		//캐릭터 정보 변경(레벨, 경험치)
	ENC_CHANGE_CHARACTER_INFO2	= 5210,		//캐릭터 정보 변경(이름, GM레벨)
} ENcValue;


typedef enum
{ 
	EPSTATE_NONE = 0,
	EPSTATE_ASSIGN,
	EPSTATE_POSESSION,
	EPSTATE_COMPLETE,
} EPtetitionState;

typedef enum
{
	EANSWER_NONE = 0,
	EANSWER_CHATTING,
	EANSWER_MESSAGE,
} EAnswerKind;
/*
typedef enum
{
	EUS_LIST_ALL = 0,
	EUS_LIST_MINE,
	EUS_RETURN_USER_INFO,
} EUpdateState;
*/
typedef enum 
{
	ORDER_NONE			= 0,
	ORDER_CHANGE_NAME		= 1,	//이름바꾸기
	ORDER_CHANGE_STATE		= 2,	//캐릭터정보 변경
	ORDER_CHANGE_ITEM_INFO	= 3,	//아이템 정보 변경(생성, 수정, 삭제)
	ORDER_CHANGE_SKILL_INFO = 4,	//스킬정보바꾸기
	ORDER_CHANGE_POS		= 5,	//위치변경
	ORDER_CHANGE_QUEST_INFO	= 6,	//퀘스트 정보 변경
	ORDER_CHANGE_GM_LEVEL	= 7,	//GM레벨 변경
	ORDER_BLOCK_USER		= 8,	//Block User
	ORDER_KICK_USER			= 9,
	ORDER_CHANGE_GUILD_INFO	= 10,	//길드정보 변경
	ORDER_CHANGE_PARTY_NAME	= 11,	//파티명 변경
	ORDER_DEATH_OR_ALIVE_CHARACTER	= 12,	//캐릭터 삭제
	ORDER_PATCH_VERSION_EDIT		= 13,	//서버 패치 버즌 변경
	ORDER_CHANGE_CHARACTER_FACE		= 14,	//캐릭터 외형 변경
	ORDER_CHANGE_CP					= 15,	//CP변겅
	ORDER_NOTICES					= 16,
	ORDER_EMPORIA_OPEN				= 17,
	ORDER_EMPORIA_CLOSE				= 18,
	ORDER_EMPORIA_STARTTIME_CHANGE	= 19,
	ORDER_EMPORIA_OWNER_CHANGE		= 20,
	ORDER_CHANGE_ACHIEVEMENT		= 21,
	ORDER_CASHSHOP_TABLE_RELOAD		= 22,	// 캐시샵 테이블 갱신
	ORDER_DELETE_NOTICES			= 23,	// 공지 삭제
	ORDER_SHOPINGAME_TABLE_RELOAD	= 24,	// 게임 상점 정보 갱신
	ORDER_OXQUIZEVENT_RELOAD		= 25,	// OX 퀴즈 이벤트 테이블 갱신
	ORDER_CREATE_MAIL				= 26,	// 메일 발송
	ORDER_DELETE_MAIL				= 27,	// 메일 삭제
	ORDER_CHANGE_QUEST_ENDED		= 28,	// 완료 상태 변경
	ORDER_CLEAR_MISSION_RANK		= 29,	// 미션 랭킹 삭제 명령
	// = 30, // 빈공간
	// = 31, // 빈공간
	ORDER_DELETE_CASH_GIFT			= 32,	// 캐시 선물함 삭제
	ORDER_RETURN_MAIL				= 33,	// 메일 반송
	ORDER_MARKET_CLOSE				= 34,	// 마켓 정지
	//ORDER_GUILD_CHANGEOWNER			= 35,	// 길드 소유자 변경 //12.3.15 10번 타입으로 병합됨
	ORDER_BLOCKIP_ADD_ACCEPT		= 36,	// IP Filter, Accept 추가
	ORDER_EVENT_QUEST_RELOAD		= 37,	// 이벤트 퀘스트(최전선 지원) 리로드

	ORDER_GAMBLEMACHINE_RELOAD		= 38,	// 인게임 가챠 리로드 
	ORDER_BATTLE_SQUARE_RELOAD		= 39,	// 배틀 스퀘어 테이블 리로드
	ORDER_SUSPEND_BATTLESQUARE		= 40,	// 배틀 스퀘어 중단
	ORDER_LUCKYSTAREVENT_RELOAD		= 41,	// '행운의 별자리' 이벤트 테이블 갱신
	ORDER_LUCKYSTAR_UPDATE_EVENT	= 42,	// '행운의 별자리' 이벤트 정보 수정
	ORDER_EVENT_ITEM_REWARD_RELOAD	= 43,	// 퍼블리셔가 수정 가능한 아이템 박스
	ORDER_CHANGE_PLAYERPLAYTIME		= 44,	// 피로도 상태 변경

	ORDER_CREATE_GROUP_MAIL			= 45,	// 그룹 메일 발송 시작
	ORDER_DELETE_GROUP_MAIL			= 46,	// 그룹 메일 발송 취소
	ORDER_TREASURE_CHEST_RELOAD		= 47,	// 보물상자 테이블 리로드

	ORDER_PREMIUM_SERVICE_INSERT	= 48,	// 프리미엄 서비스 등록
	ORDER_PREMIUM_SERVICE_MODIFY	= 49,	// 프리미엄 서비스 연장
	ORDER_PREMIUM_SERVICE_REMOVE	= 50,	// 프리미엄 서비스 종료
	ORDER_RELOAD_META				= 51,   // Special command for reload abils from hotmeta
}EORDERType;

typedef enum : BYTE
{
	OS_NONE				= 0,	//처리전
	OS_DOING			= 1,	//처리중
	OS_DONE				= 2,	//처리완료
	OS_RESERVE			= 3,	//보류(먼가 문제있다) 보류되었을 시에는 ErrorCode에 에러 코드(E_GM_ERR) 가 기록된다.
}EOrderState;

typedef struct TagOrderDB
{
	TagOrderDB()
	{
		kCmdGuid = BM::GUID::NullData();
		kReqGuid = BM::GUID::NullData();
		Type = ORDER_NONE;
		dtTime[0].IsNull();
		dtTime[1].IsNull();
		for(int i = 0; i < 5; i++)
		{
			kGuid[i].NullData();
			fValue[i]	= 0;
			iValue[i]	= 0;
			biValue[i]	= 0;
		}
		usRealm = 0;
		usChannel = 0;
		wsString1.clear();
		wsString2.clear();
	};
		
	//Head
	BM::GUID				kCmdGuid;
	BM::GUID				kReqGuid;
	short					Type;
	unsigned short			usRealm;
	unsigned short			usChannel;

	//Body
	BM::GUID				kGuid[5];
	float					fValue[5];
	int						iValue[5];
	__int64					biValue[5];
	std::wstring			wsString1;
	std::wstring			wsString2;
	BM::DBTIMESTAMP_EX		dtTime[2];

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(kCmdGuid);
		rkPacket.Push(kReqGuid);
		rkPacket.Push(Type);
		rkPacket.Push(usRealm);
		rkPacket.Push(usChannel);
		for(int i = 0; i < 5; i++)
		{
			rkPacket.Push(kGuid[i]);
			rkPacket.Push(fValue[i]);
			rkPacket.Push(iValue[i]);
			rkPacket.Push(biValue[i]);
		}
		rkPacket.Push(wsString1);
		rkPacket.Push(wsString2);
		rkPacket.Push(dtTime[0]);
		rkPacket.Push(dtTime[1]);
	}

	void ReadFromPacket(BM::Stream *pkPacket) 
	{
		pkPacket->Pop(kCmdGuid);
		pkPacket->Pop(kReqGuid);
		pkPacket->Pop(Type);
		pkPacket->Pop(usRealm);
		pkPacket->Pop(usChannel);
		for(int i = 0; i < 5; i++)
		{
			pkPacket->Pop(kGuid[i]);
			pkPacket->Pop(fValue[i]);
			pkPacket->Pop(iValue[i]);
			pkPacket->Pop(biValue[i]);
		}
		pkPacket->Pop(wsString1);
		pkPacket->Pop(wsString2);
		pkPacket->Pop(dtTime[0]);
		pkPacket->Pop(dtTime[1]);
	}


//	protected: DWORD		dwLocale;
}SGmOrder;

typedef struct tagPetitionData
{
	tagPetitionData()
	{
		Clear();
	}

	BM::GUID			m_PetitionId;
	std::wstring		m_szCharacterName;
	std::wstring		m_szTitle;
	std::wstring		m_szPetition;
	BM::GUID			m_GmId;
	short				m_RealmNo;
	short				m_ChannelNo;
	short				m_State;
	short				m_Kind;
	BM::DBTIMESTAMP_EX m_dtReceiptTime;
	int					m_ReceiptIndex;
	short				m_Answerkind;		//0:None, 1:short, 2:Message
	BM::DBTIMESTAMP_EX m_dtStartTime;
	BM::DBTIMESTAMP_EX m_dtEndTime;


	void Clear()
	{
		m_PetitionId = BM::GUID::NullData();
		m_szCharacterName.clear();
		m_szTitle.clear();
		m_szPetition.clear();
		m_GmId = BM::GUID::NullData();
		m_RealmNo = 0;
		m_ChannelNo = 0;
		m_State = EPSTATE_NONE;
		m_Kind = 0;
		m_ReceiptIndex = 0;
		m_Answerkind = EANSWER_NONE;
		m_dtStartTime.SetLocalTime();
		m_dtEndTime.SetLocalTime();
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(m_PetitionId);
		rkPacket.Push(m_szCharacterName);
		rkPacket.Push(m_szTitle);
		rkPacket.Push(m_szPetition);
		rkPacket.Push(m_GmId);
		rkPacket.Push(m_RealmNo);
		rkPacket.Push(m_ChannelNo);
		rkPacket.Push(m_State);
		rkPacket.Push(m_Kind);
		rkPacket.Push(m_dtReceiptTime);
		rkPacket.Push(m_ReceiptIndex);
		rkPacket.Push(m_Answerkind);
		rkPacket.Push(m_dtStartTime);
		rkPacket.Push(m_dtEndTime);
	}
	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(m_PetitionId);
		rkPacket.Pop(m_szCharacterName);
		rkPacket.Pop(m_szTitle);
		rkPacket.Pop(m_szPetition);
		rkPacket.Pop(m_GmId);
		rkPacket.Pop(m_RealmNo);
		rkPacket.Pop(m_ChannelNo);
		rkPacket.Pop(m_State);
		rkPacket.Pop(m_Kind);
		rkPacket.Pop(m_dtReceiptTime);
		rkPacket.Pop(m_ReceiptIndex);
		rkPacket.Pop(m_Answerkind);
		rkPacket.Pop(m_dtStartTime);
		rkPacket.Pop(m_dtEndTime);
	}

} SPetitionData;


typedef struct Tag_NcGMCommandPack
{
	Tag_NcGMCommandPack()
	{
		memset(iValues, 0, sizeof(iValues));

		memset(biValues, 0, sizeof(biValues));
		memset(sValues, 0, sizeof(sValues));
		memset(byValues, 0, sizeof(byValues));

		kString1.clear();
		kString2.clear();
		kString3.clear();
	};

	int						iValues[8];

	__int64					biValues[5];
	short					sValues[5];
	BYTE					byValues[5];

	BM::GUID				kGuid[5];
	std::wstring			kString1;
	std::wstring			kString2;
	std::wstring			kString3;
	BM::DBTIMESTAMP_EX		dtTime[5];

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		for(int i = 0; i < 8; ++i)
		{
			rkPacket.Push(iValues[i]);
		}
		for(int i = 0; i < 5; ++i)
		{
			rkPacket.Push(biValues[i]);
			rkPacket.Push(sValues[i]);
			rkPacket.Push(byValues[i]);
			rkPacket.Push(kGuid[i]);
			rkPacket.Push(dtTime[i]);
		}
		rkPacket.Push(kString1);
		rkPacket.Push(kString2);
		rkPacket.Push(kString3);

	}

	void ReadFromPacket(BM::Stream *pkPacket)
	{
		for(int i = 0; i < 8; ++i)
		{
			pkPacket->Pop(iValues[i]);
		}
		for(int i = 0; i < 5; ++i)
		{
			pkPacket->Pop(biValues[i]);
			pkPacket->Pop(sValues[i]);
			pkPacket->Pop(byValues[i]);
			pkPacket->Pop(kGuid[i]);
			pkPacket->Pop(dtTime[i]);
		}
		pkPacket->Pop(kString1);
		pkPacket->Pop(kString2);
		pkPacket->Pop(kString3);
	}
}SNcGMCommandPack;


typedef std::map<std::wstring, SAuthInfo*> CONT_GM_AUTH_DATA_ID;
typedef std::map<BM::GUID, SAuthInfo*> CONT_GM_AUTH_DATA;

//진정 데이터
typedef std::vector<SPetitionData> CONT_PETITION_DATA;
//서버Number, 해당 서버의 모든 진덩 데이터
typedef std::map<short, CONT_PETITION_DATA> CONT_ALL_PETITION_DATA;

#endif // WEAPON_VARIANT_BASIC_CONSTANT_GM_CONST_H