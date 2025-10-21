#ifndef WEAPON_COLLINS_LOG_LOG_H
#define WEAPON_COLLINS_LOG_LOG_H

#include "Variant/DefAbilType.h"

// Main
typedef enum : short
{
	ELogMain_None						= 0,
	ELogMain_System						= 1000,	// 서버 시스템관련 로그
	ELogMain_System_LogServer			= 1001,	// 로그 서버 접속
	ELogMain_System_Service				= 1002,	// 게임 서버 서비스 관련(시작/종료)
	ELogMain_System_User_Count			= 1003,	// 서버 접속자 정보
	ELogMain_System_Create_NewAccount	= 1004, // 신규 계정 생성
	ELogMain_System_Change_Password		= 1005, // 비밀번호 변경
	ELogMain_System_Change_Birthday		= 1006,	// 생년월일 변경
	ELogMain_Statistics					= 1007,	// 통계정보
	ELogMain_System_Login				= 1100,	// 로그인 관련
	ELogMain_System_Login_TryAuth		= 1101,	// TryAuth
	ELogMain_System_Login_SelecteChar	= 1102,	// Selecte Character
	ELogMain_System_Login_LogOut		= 1103,	// LogOut
	ELogMain_System_Login_User_Info		= 1104,	// LogIn User
	ELogMain_System_First_Logined		= 1105,	// 처음 로그인 했음.
	ELogMain_System_Block				= 1106,	// Block accout
	ELogMain_System_Event_Log			= 1200,	// 이벤트 관련
	ELogMain_System_Event				= 1201,	// 이벤트
	ELogMain_User						= 2000,	// 유저 데이터
	ELogMain_User_Character				= 2100,	// 캐릭터
	ELogMain_Contents					= 3000,	// 컨텐츠
	ELogMain_Contents_Transaction		= 3001,	// 거래
	ELogMain_Contents_Item				= 3002,	// 아이템
	ELogMain_Contents_Mail				= 3003,	// 메일
	ELogMain_Contents_Quest				= 3004,	// 퀘스트
	ELogMain_Contents_Party				= 3005,	// 파티
	ELogMain_Contents_Guild				= 3006,	// 길드
	ELogMain_Contents_Friend			= 3007,	// 친구
	ELogMain_Contents_Cupple			= 3008,	// 커플
	ELogMain_Contents_Chatting			= 3009,	// 채팅
	ELogMain_Contents_Monster			= 3010,	// 몬스터
	ELogMain_Contents_MIssion			= 3011,	// 미션
	ELogMain_Contents_PVP				= 3012,	// PVP
	ELogMain_Contents_OpenMarket		= 3013, // 오픈마켓
	ELogMain_Contents_Pet				= 3014,	// Pet
	ELogMain_Contents_Achievements		= 3015,	// 업적
	ELogMain_Coupon_Cash				= 4000,	// 쿠폰 & Cash
	ELogMain_Coupon						= 4001,	// 쿠폰
	ELogMain_Cash						= 4002,	// 캐쉬
	ELogMain_OXQuiz						= 4003, // ox 퀴즈 이벤트
	ELogMain_Socket						= 4004, // 소켓
	ELogMain_GemStore					= 4005, // 보석 교환기
	ELogMain_ConvertItem				= 4006, // 아이템 변환기
	ELogMain_CCEReward					= 4007, // 캐릭 생성 이벤트
	ELogMain_MyHome						= 4008,	// 마이홈
	ELogMain_MixupItem					= 4009,	// 커스튬 조합
	ELogMain_EventQuest					= 4010, // 이벤트 퀘스트 시스템
	ELogMain_LuckyStar					= 4011, // 행운의 별자리 이벤트
	ELogMain_Contents_Expedition		= 4012, // 원정대
	ELogMain_Premium					= 4013, // 프리미엄
	ELogMain_JobSkill					= 4014, // 직업스킬
	ELogMain_Community_Event			= 4015, // 커뮤니티 이벤트

	ELogMain_GMCommand					= 10000,// GMCommand
} ELogMainType;

typedef enum : short
{
	ELogSub_None = 0,
	ELogSub_Service						= 50,	// 서비스
	ELogSub_Map							= 51,	// Map
	ELogSub_Character_Create			= 100,	// 캐릭터 생성
	ELogSub_Character_Change_Info,				// 캐릭터 정보 수정
	ELogSub_Character_Move,						// 캐릭터 이동
	ELogSub_Character_Merge,
	ELogSub_Character_DeathPenalty,
	ELogSub_Character_DBSave			= 105,	// 캐릭터 정보가 DB Save 됨.
	ELogSub_BattleSquare_Death			= 106,
	ELogSub_BattleSquare_DropItem		= 107,
	ELogSub_Mission_Defence7			= 108,
	ELogSub_Transaction					= 200,	// 거래
	ELogSub_Item_Equip					= 300,	// 아이템 장착
	ELogSub_Item_Unequip,						// 아이템 장착 해제
	ELogSub_Item_Enchent,						// 인첸트
	ELogSub_Item_Use,							// 아이템 사용
	ELogSub_Item_Creative,						// 제작
	ELogSub_Item_Give_Bond,						// 창고 보관
	ELogSub_Item_Take_Bond,						// 창고 찾기
	ELogSub_Item_Take_To_Monster,				// 몬스터로 부터 습득
	ELogSub_Item_Buy,							// 아이템 구입
	ELogSub_Item_Sell,							// 아이템 판매
	ELogSub_Item_Destroy,						// 아이템 파괴
	ELogSub_Item_SysInv,						// 시스템 인벤토리
	ELogSub_Item_Repair,						// 아이템 수리
	ELogSub_Item_Divide,						// 아이템 나누기
	ELogSub_Item_Dimension,						// 차원교환기
	ELogSub_Item_DimensionExchange,				// 차원교환기-저장된 내용교환
	ELogSub_Mail_Forwarding				= 400,	// 메일 전송
	ELogSub_Mail_Read,							// 메일 읽기
	ELogSub_Mail_Return,						// 메일 반송
	ELogSub_Mail_Delete,						// 메일 삭제
	ELogSub_Mail_RecvItem,						// 메일 아이템 수령
	ELogSub_Quest_Start					= 500,	// 퀘스트 시작
	ELogSub_Quest_ChangeInfo,					// 퀘스트 정보 수정
	ELogSub_Quest_End,							// 퀘스트 완료
	ELogSub_Quest_Drop,							// 퀘스트 포기
	ELogSub_Party						= 600,	// 파티
	ELogSub_Expedition					= 601,	// 원정대
	ELogSub_Guild_Create				= 700,	// 길드 생성
	ELogSub_Guild_Delete				= 701,	// 길드 해체
	ELogSub_Guild_Join					= 702,	// 길드 가입
	ELogSub_Guild_Leave					= 703,	// 길드 탈퇴
	ELogSub_Guild_Kick					= 704,	// 길드 추방
	ELogSub_Guild_Master				= 705,	// 길드장 위임
	ELogSub_Guild_Change_Officer		= 706,	// 부길마 설정
	ELogSub_Guild_Change_Info			= 707,	// 길드정보 변경
	ELogSub_Guild_Notice				= 708,	// 길드 공지
	ELogSub_Change_Level				= 709,	// 길드 레벨 변경
	ELogSub_Get_Exp						= 710,	// 길드 경험치 습득
	ELogSub_EmBattle_Reserve			= 711,	// 엠포리아 쟁탈전 신청
	ELogSub_EmBattle_RefundExp			= 712,	// 엠포리아 쟁탈전 경험치 환불
	ELogSub_Guild_InventoryCreate		= 713,	// 길드금고 생성
    ELogSub_EmBattle					= 714,	// 엠포리아 쟁탈전
	ELogSub_Guild_Invite				= 715,	// 길드 초대
	ELogSub_Friend_Apply				= 800,	// 친구 신청
	ELogSub_Friend_Add,							// 친구 추가
	ELogSub_Friend_Delete,						// 친구 삭제
	ELogSub_Friend_Block,						// 친구 차단
	ELogSub_Friend_Clear,						// 친구 해제
	ELogSub_Friend_Rejection,					// 친구 신청 거절
	ELogSub_Cupple_Apply				= 900,	// 커플 신청	
	ELogSub_Cupple_Break,						// 커플 탈퇴
	ELogSub_Chat_Nomal					= 1000,	// 채팅 일반
	ELogSub_Chat_Whisper,						// 채팅 귓속말
	ELogSub_Chat_Party,							// 채팅 파티
	ELogSub_Chat_Guild,							// 채팅 길드
	ELogSub_Chat_Friend,						// 채팅 친구
	ELogSub_Chat_Shout,							// 채팅 외치기
	ELogSub_Chat_PvpRoom,						// 채팅 PVP
	ELogSub_Chat_ManToMan,						// 1:1 대화
	ELogSub_Chat_ShoutChannel,					// 채널 외치기
	ELogSub_Chat_ShoutRealm,					// 렐름 외치기
	ELogSub_Chat_TeamChat,						// 공성전 팀챗
	ELogSub_Chat_Trade,							// 채팅 거래
	ELogSub_Monster_Create				= 1100,	// 몬스터 생성
	ELogSub_Monster_Death,						// 몬스터 죽음
	ELogSub_Mission_Start				= 1200,	// 미션 시작
	ELogSub_Mission_End,						// 미션 종료
	ELogSub_Mission_Game,					// 미션 게임중
	ELogSub_PVP_Game					= 1300,	// PVP 게임
	ELogSub_PVP_Result,							// PVP 결과
	ELogSub_PVP_UserCount,						// PVP 이용자 수
	ELogSub_OpenMarket_Open				= 1310,	// 마켓 개설
	ELogSub_OpenMarket_ChangeInfo,				// 마켓 정보 수정
	ELogSub_OpenMarket_Regist_Product,			// 물품 등록
	ELogSub_OpenMarket_Recall_Product,			// 물품 회수
	ELogSub_OpenMarket_Recall_Money,			// 판매대금 회수
	ELogSub_OpenMarket_Buy_Product,				// 물품 구매
	ELogSub_OpenMarket_Off,						// 마켓 종료
	ELogSub_Coupon						= 1400,	// 쿠폰
	ELogSub_Cash						= 1500,	// 캐쉬
	ELogSub_Cash_Present,						// 선물
	ELogSub_Cash_Buy,							// 구매
	ELogSub_Cash_Receipt,						// 수령
	ELogSub_Cash_AddTime,						// 기간 연장

	ELogSub_OxQuiz_Open					= 1600, // ox 퀴즈 시작
	ELogSub_OxQuiz_Close,						// 종료
	ELogSub_OxQuiz_User_Enter,					// ox 퀴즈 유저 참가
	ELogSub_OxQuiz_User_Exit,					// ox 퀴즈 유저 탈퇴
	ELogSub_OxQuiz_User_Log,					// 유저 퀴즈 진행 단계 저장
	ELogSub_OxQuiz_User_Num,					// 퀴즈 진행 단계별 유저 현황

	ELogSub_Pet							= 1700,	// Pet

	ELogSub_GMCommand_Recv				= 2000,	// GMCommand 수신
	ELogSub_GMCommand_Ret,						// GMCoommand 처리 결과

	ELogSub_Socket_Create				= 2300, // 소켓 생성
	ELogSub_Socket_Delete				= 2301, // 소켓 삭제
	ELogSub_Socket_Modify				= 2302, // 소켓 수정
	ELogSub_Socket_Remove				= 2303, // 카드만 삭제
	ELogSub_Socket_Extraction			= 2304, // 소켓 추출

	ELogSub_GemStore_Buy				= 2400,	// 보석 교환
	ELogSub_CollectAntique				= 2410,	// 골동품 수집
	ELogSub_ExchangeGem					= 2411, // 보석상인 교환
	ELogSub_ItemShop					= 2412,	// 이벤트아이템 교환

	ELogSub_ConvertItem					= 2500, // 아이템 변환기

	ELogSub_CCE_Reward					= 2600, // 캐릭 생성 이벤트 보상

	ELogSub_Item_Modify					= 2702,	// 아이템 수정
	ELogSub_Home_Modify					= 2703,	// 홈 수정
	ELogSub_Home_Bidding				= 2704,	// 홈 입찰 
	ELogSub_Home_Auction_Reg			= 2705,	// 홈 경매 등록
	ELogSub_Home_Auction_Unreg			= 2706,	// 홈 경매 취소
	ELogSub_Home_Auction_End			= 2707, // 경매 종료
	ELogSub_Home_Attachment				= 2708,	// 차압
	ELogSub_Home_Noti_Tex				= 2709,	// 세금 연체 통보
	ELogSub_Home_Pay_Tex				= 2710,	// 세금 납부

	ELogSub_Marry_Apply					= 2800,	// 결혼
	ELogSub_EventQuest					= 2900, // 이벤트 퀘스트 시스템
	ELogSub_UserQuestComplete			= 2901, // 퀘스트 완료

	ELogSub_LuckyStar_Open					= 3000, // 행운의 별자리 이벤트 시작
	ELogSub_LuckyStar_Close,						// 종료
	ELogSub_LuckyStar_User_Enter,					// 유저 참가
	ELogSub_LuckyStar_User_ReEnter,					// 유저 재참가
	ELogSub_LuckyStar_User_Logout,					// 유저 로그아웃
	ELogSub_LuckyStar_User_Exit,					// 유저 탈퇴
	ELogSub_LuckyStar_Event_Log,					// 진행 단계별 이벤트 현황
	ELogSub_LuckyStar_User_Log,						// 진행 단계별 유저 현황

	ELogSub_Gamblemachine				= 3100,		// 인게임 가챠
	ELogSub_Gamblemachine_Cash,						// 캐시샵 가챠
	ELogSub_Gamblemachine_Mixup,					// 캐시샵 커스튬 조합

	ELogSub_Achievements				= 3200,		// 업적 달성

	ELogSub_TreasureChest				= 3300,		// 보물상자
	ELogSub_EventItemReward				= 3310,		// 이벤트 지급상자
	ELogSub_Manufacture					= 3400,		// 즉석가공
	ELogSub_Collect						= 3401,		// 채집
	ELogSub_PetProduceItem				= 3403,		// 펫 스킬 (아이템 생산)
	ELogSub_Constellation				= 3404,		// 별자리 던전 보상
	
	ELogSub_Event_Keep_Access			= 3500,		// 접속 유지 이벤트

	ELogSub_Community_Event_Tick		= 3600,		// 커뮤니티 이벤트 일정 주기로 남기는 로그.
	ELogSub_Community_Event_State		= 3601,		// 커뮤니티 이벤트 상태 변경될 때 남기는 로그.
	ELogSub_Community_Event_NowEvent	= 3602,		// 현재 진행 중인 이벤트.
	ELogSub_Community_Event_NotifyStart = 3603,		// 이벤트가 시작되는 것을 다른 맵서버로 알림.
	ELogSub_Community_Event_RecvStart	= 3604,		// 이벤트가 시작되는 것을 커뮤니티 이벤트 맵으로부터 받음.
	ELogSub_Community_Event_NotifyEnd	= 3605,		// 이벤트가 끝나는 것을 다른 맵서버로 알림.
	ELogSub_Community_Event_RecvEnd		= 3606,		// 이벤트가 끝나는 것을 커뮤니티 이벤트 맵으로부터 받음.
	ELogSub_Community_Event_Failed_Enter = 3607,	// 유저가 요청한 커뮤니티 던전 입장에 실패했을 경우, 시간과 요청한 던전 번호를 남긴다.

	ELogSub_ExtractElement				= 3700,		// Element Extract

} ELogSubType;

// Order
typedef enum : short
{
	ELOrderMain_None = 0,
	ELOrderMain_Service				= 100,	// 서비스
	ELOrderMain_Time				= 101,	// 시간
	ELOrderMain_Event				= 200,	// 이벤트
	ELOrderMain_Character			= 300,	// 캐릭터
	ELOrderMain_Level				= 400,	// 레벨
	ELOrderMain_TacticLevel			= 401,	// 용병 레벨
	ELOrderMain_ChangeJob			= 500,	// 전직
	ELOrderMain_Skill				= 600,	// 스킬
	ELOrderMain_SP					= 601,	// SP
	ELOrderMain_Fran				= 700,	// 프랜
	ELOrderMain_Die					= 800,	// 사망
	ELOrderMain_Alive				= 900,	// 부활
	ELOrderMain_Exp					= 1000,	// 경험치
	ELOrderMain_TacticsExp			= 1001,	// 용병 경험치
	ELOrderMain_Map					= 1100, // 맵
	ELOrderMain_Transaction			= 1200,	// 거래
	ELOrderMain_Item				= 1300,	// 아이템
	ELOrderMain_Item_EnchantLvUp	= 1301, // 아이템 인챈트
	ELOrderMain_Item_SoulCraft		= 1302, // 아이템 영력상승
	ELOrderMain_ItemInsert			= 1303, // 아이템 넣기
	ELOrderMain_ItemDelete			= 1304, // 아이템 빼기
	ELOrderMain_Money				= 1400,	// 돈
	ELOrderMain_Mail				= 1500,	// 메일
	ELOrderMain_Quest				= 1600,	// 퀘스트
	ELOrderMain_Party				= 1700,	// 파티
	ELOrderMain_Expedition			= 1701, // 원정대
	ELOrderMain_Guild				= 1800,	// 길드
	ELOrderMain_Guild_Master		= 1900,	// 길드 마스터
	ELOrderMain_Guild_Officer		= 2000,	// 부길마
	ELOrderMain_Guild_Notice		= 2100,	// 길드 공지
	ELOrderMain_Guild_Invite		= 2101, // 길드 초대
	ELOrderMain_Friend				= 2200, // 친구
	ELOrderMain_Cupple				= 2300,	// 커플
	ELOrderMain_Monster				= 2400,	// 몬스터
	ELOrderMain_Entity				= 2401, // 엔티티
	ELOrderMain_Mission				= 2500,	// 미션
	ELOrderMain_Game				= 2600, // 게임
	ELOrderMain_Round				= 2700,	// 라운드
	ELOrderMain_Result_One			= 2800,	// 개인 결과
	ELOrderMain_User_Count			= 2801,	// 유저수
	ELOrderMain_Market				= 2810,	// 상점
	ELOrderMain_Market_Article		= 2811,	// 상점물품
	ELOrderMain_Exchange			= 2900,	// 교환
	ELOrderMain_Charge				= 3000, // 충전
	ELOrderMain_Use					= 3100, // 사용
	ELOrderMain_Product				= 3200, // 상품
	ELOrderMain_Cash				= 3300, // 캐쉬
	ELOrderMain_GMCommand			= 3400, // GMCommand
	ELOrderMain_Mileage				= 3500, // 마일리지
	ELOrderMain_Inventory			= 3600,	// 인벤토리
	ELOrderMain_Home_Addr			= 3700,	// 오너 홈 주소 변경
	ELOrderMain_Home_TexTime		= 3701,	// 홈 세금 납부 시간 변경
	ELOrderMain_Home_AuctionTime	= 3702,	// 홈 경매 종료 시간 변경
	ELOrderMain_Home_StyleItem		= 3703,	// 홈 외형 변경
	ELOrderMain_Home_PayTex			= 3704,	// 홈 세금 납부
	ELOrderMain_Home_Attachment		= 3705,	// 홈 차압
	ELOrderMain_Home_Auction		= 3706,	// 홈 경매 상태
	ELOrderMain_Home_Bidding		= 3707,	// 홈 경매 입찰
	ELOrderMain_Home_VisitFlag		= 3708,	// 홈 방문자 플레그
	ELOrderMain_AS_BonusStatus		= 3800,	// ActiveStatus Bonus Status
	ELOrderMain_AS_Str				= 3801,	// ActiveStatus STR
	ELOrderMain_AS_Int				= 3802,	// ActiveStatus Int
	ELOrderMain_AS_Con				= 3803,	// ActiveStatus Con
	ELOrderMain_AS_Dex				= 3804,	// ActiveStatus Dex
	ELOrderMain_Achievements		= 3900, // 업적 획득
	ELOrderMain_Structure			= 4000, // 구조물
	ELOrderMain_CommunityEvent		= 4100, // 커뮤니티 이벤트.
} EOrderMainType;

typedef enum : short
{
	ELOrderSub_None = 0,
	ELOrderSub_Connection			= 1,	// 접속
	ELOrderSub_Start				= 2,	// 시작
	ELOrderSub_End					= 3,	// 종료
	ELOrderSub_Create				= 4,	// 생성
	ELOrderSub_Delete				= 5,	// 삭제
	ELOrderSub_Modify				= 6,	// 수정
	ELOrderSub_Death				= 7,	// 사망
	ELOrderSub_Alive				= 8,	// 부활
	ELOrderSub_Move					= 9,	// 이동
	ELOrderSub_Complete				= 10,	// 완료
	ELOrderSub_Forwarding			= 11,	// 전송
	ELOrderSub_Read					= 12,	// 읽기
	ELOrderSub_Return				= 13,	// 반송
	ELOrderSub_Join					= 14,	// 가입
	ELOrderSub_Break				= 15,	// 탈퇴
	ELOrderSub_Exile				= 16,	// 추방
	ELOrderSub_Apply				= 17,	// 신청
	ELOrderSub_Block				= 18,	// 차단
	ELOrderSub_Clear				= 19,	// 해제
	ELOrderSub_Rejection			= 20,	// 거절
	ELOrderSub_Drop					= 21,	// 드롭
	ELOrderSub_DeEnd				= 22,	// DeEnd
	ELOrderSub_ChangeState			= 23,	// 변화
	ELOrderSub_Info					= 24,	// 정보보기
	ELOrderSub_Warning				= 25,	// 경고
	ELOrderSub_DBSave				= 26,	// DB Save
	ELOrderSub_Rollback				= 27,	// function Rollback
	ELOrderSub_Buy					= 28,	// 구매
	ELOrderSub_Sell					= 29,	// 판매
	ELOrderSub_Auction_Start		= 30,	// 경매 시작
	ELOrderSub_Auction_End			= 31,	// 경매 종료
	ELOrderSub_Auction_Cancel		= 32,	// 경매 취소
	ELOrderSub_Calculate			= 33,	// 계산
	ELOrderSub_Reward				= 34,	// 보상
	ELOrderSub_Install				= 35,	// 설치
	ELOrderSub_Learn				= 36,	// 배우기
	ELOrderSub_AccumPointToStrategyPoint	= 37,	// 포인트복제기사용 후 포인트전환시 발생
	ELOrderSub_KillMonsterByAccumPoint		= 38,	// 포인트복제기사용 후 몬스터 Kill시 발생
	ELOrderSub_RelayPlay			= 39,	// 이어하기
	ELOrderSub_Success				= 40,	// 성공
	ELOrderSub_Fail					= 41,	// 실패
} EOrderSubType;

class PgLog
{
public:
	explicit PgLog(EOrderMainType eMainType=ELOrderMain_None, EOrderSubType eSubType=ELOrderSub_None);
	~PgLog();

	typedef enum {
		E_INT_SIZE = 5,
		E_I64_SIZE = 5,
		//E_GUID_SIZE = 2,
		E_STR_SIZE = 4,
	};
public:
	PgLog& operator = (PgLog const& rhs);
	void Set( size_t const iIndex, int const iValue );
	void Set( size_t const iIndex, __int64 const i64Value );
	//void Set( size_t const iIndex, BM::GUID const& rkGuid );
	void Set( size_t const iIndex, std::wstring const& rkString );

	bool Get( size_t const iIndex, int& iValue ) const;
	bool Get( size_t const iIndex, __int64 &i64Value ) const;
	//bool Get( size_t const iIndex, BM::GUID &rkGuid ) const;
	bool Get( size_t const iIndex, std::wstring &rkString ) const;

	void WriteToPacket(BM::Stream &kPacket)const;
	void ReadFromPacket(BM::Stream &kPacket);
	bool BuildQuery(CEL::DB_QUERY& rkQuery) const;

protected:

	template< typename T_KEY, typename T_VALUE, typename T_PR, typename T_ALLOC >
	void BuildQuery_Sub( std::map< T_KEY, T_VALUE, T_PR, T_ALLOC > const &rkContainer, CEL::DB_QUERY& rkQuery, T_KEY const iMaxSize )const
	{
		T_KEY iIndex = 0;
		for ( ; iIndex<iMaxSize; ++iIndex )
		{
			std::map< T_KEY, T_VALUE, T_PR, T_ALLOC >::const_iterator itr = rkContainer.find( iIndex );
			if ( itr != rkContainer.end() )
			{
				rkQuery.PushStrParam( itr->second );
			}
			else
			{
				static const std::wstring wstrEmpty(L"NULL");
				rkQuery.PushStrParam( wstrEmpty, true );
			}
		}
	}

	template<typename T_VALUE>
	void BuildQuery_Sub(std::vector<T_VALUE> const &rkContainer, CEL::DB_QUERY& rkQuery)const
	{
		std::vector<T_VALUE>::const_iterator itor = rkContainer.begin();
		while (itor != rkContainer.end())
		{
			rkQuery.PushStrParam(*itor);
			++itor;
		}
	}


private:
	CLASS_DECLARATION_S(EOrderMainType, OrderMainType);
	CLASS_DECLARATION_S(EOrderSubType, OrderSubType);

	// Log Data
	typedef std::vector< int >					CONT_LOG_INT;
	typedef std::vector< __int64 >				CONT_LOG_INT64;
	//typedef std::vector< BM::GUID >				CONT_LOG_GUID;
	typedef std::map< size_t, std::wstring >	CONT_LOG_STRING;

	CONT_LOG_INT	m_kContInt;
	CONT_LOG_INT64	m_kContInt64;
	//CONT_LOG_GUID	m_kContGuid;
	CONT_LOG_STRING m_kContString;
};

class PgLogCont
{
public:
	explicit PgLogCont(ELogMainType const eMainType = ELogMain_None, ELogSubType const eSubType = ELogSub_None,
		BM::GUID const& rkMemberGuid = BM::GUID::NullData(), BM::GUID const& rkCharGuid = BM::GUID::NullData(), 
		std::wstring const& rkID = std::wstring(), std::wstring const& rkName = std::wstring(),
		short const sClass = 0, short const sLevel = 0, int const iGroundNo  = 0, EUnitGender eGender = UG_MALE );
	~PgLogCont();

public:
	typedef std::vector<PgLog> VECTOR_LOG;

	void WriteToPacket(BM::Stream &kPacket)const;
	void ReadFromPacket(BM::Stream &kPacket);
	void Add(PgLog const& kLog);
	bool Commit() const;
	bool BuildQuery(CEL::DB_QUERY& rkQuery) const;
	bool BuildQuery(size_t iIndex, CEL::DB_QUERY& rkQuery) const;
	size_t LogCount() const;
	PgLog const* const GetLog(int const iIndex) const;
	PgLog const* const GetLog(EOrderMainType eMainType, EOrderSubType eSubType); 
	void UpdateLog(PgLog const& rkNewLog);
	PgLogCont const& operator =(PgLogCont const& rhs);

private:
	CLASS_DECLARATION_S(ELogMainType, LogMainType);
	CLASS_DECLARATION_S(ELogSubType, LogSubType);
	CLASS_DECLARATION_S(BM::GUID, LogKey);
	CLASS_DECLARATION_S(BM::GUID, MemberKey);
	CLASS_DECLARATION_S(BM::GUID, CharacterKey);
	CLASS_DECLARATION_S(std::wstring,	ID);		// Account ID
	CLASS_DECLARATION_S(int, UID);					// NC 등에서 사용하는 Member 고유key
	CLASS_DECLARATION_S(std::wstring,	Name);		// Character name
	CLASS_DECLARATION_S(short, RealmNo);
	CLASS_DECLARATION_S(short, ChannelNo);
	CLASS_DECLARATION_S(short, Class);
	CLASS_DECLARATION_S(short, Level);
	CLASS_DECLARATION_S(int, GroundNo);
	CLASS_DECLARATION_S(EUnitGender, UnitGender);	// 캐릭터 성별

	VECTOR_LOG m_kLog;
};

typedef std::map<BM::GUID,PgLogCont> CONT_LOG_CONT;

class PgContLogMgr
{
public:

	PgContLogMgr();
	~PgContLogMgr(){}

	bool const AddContLog(PgLogCont const & kLog);
	bool const AddLog(BM::GUID const & kCharGuid,PgLog const & kLog);
	bool const AddUpdateLog(BM::GUID const & kCharGuid,PgLog const & kLog);
	void SendLog()const;
	PgLogCont * const GetLogCont(BM::GUID const& rkGuid);
	void GetLogCont(VEC_GUID& rkGuid) const;
	void MergeLogCont(PgContLogMgr const& rkCont);

private:
	CONT_LOG_CONT	m_kContLog;
	CLASS_DECLARATION_S(BM::GUID, Guid);
};

#endif // WEAPON_COLLINS_LOG_LOG_H