#ifndef WEAPON_LOHENGRIN_MAINFRAME_ERRORCODE_H
#define WEAPON_LOHENGRIN_MAINFRAME_ERRORCODE_H

// COMMON
#define EC_OK		0

// Character ID 에 대한 유효성 검사 결과
// PT_S_C_ANS_CHECK_NAME 에 대한 Error code
#define EC_CHNAME_SUCCESS	BYTE(0)		// OK
#define EC_CHNAME_TOOLONG	BYTE(0xF1)	// Too long name
#define EC_CHNAME_HASWHITE	BYTE(0xF2)	// White space error
#define EC_CHNAME_BADWORD	BYTE(0xF3)	// Bad word
#define EC_CHNAME_DUPLICATE	BYTE(0xF4)	// already exist ID
#define EC_CHNAME_CANNOTFIND	BYTE(0xF5)	// cannot find in Cache

// General Error code
#define EC_CONNECTED_USER	0xF010001	// Alread connected

// Inventory 작업 관련 에러 코드
#define E_NOT_ENOUGH_MONEY		(E_FAIL + 1)
#define E_NOT_ENOUGH_INV_SPACE	(E_FAIL + 2)
#define E_INCORRECT_ITEM		(E_FAIL + 3)	// 잘못된 아이템 정보
#define E_INVALID_INVENTYPE		(E_FAIL + 4)	// 잘못된 Inven Type
#define E_INVALID_ITEMPOS		(E_FAIL + 5)	// 잘못된 아이템 위치값
#define E_CANNOT_MODIFYITEM		(E_FAIL + 6)	// 아이템 수정 실패
#define E_INVALID_INVENTORY		(E_FAIL + 7)
#define E_NOT_ENOUGH_CP			(E_FAIL + 8)
#define E_NOT_FOUND_ITEM		(E_FAIL + 9)	// 없는 아이템이다.
#define E_BUSY_INVENTORY		(E_FAIL + 10)
#define E_NOT_ENOUGH_ITEM		(E_FAIL + 11)	// 아이템이 부족합니다.(삭제시)
#define E_DONT_SEND_RESULT		(E_FAIL + 12)
#define E_ENABLE_TIMEOUT		(E_FAIL + 13)	// 사용 기간 종료
#define E_NOT_OPENED_MAP		(E_FAIL + 14)	// 이동한적 없는 맵이다.
#define E_NOT_JOINED_PARTY		(E_FAIL + 15)	// 파티에 가입하지 않았다.
#define E_NOT_FOUND_MEMBER		(E_FAIL + 16)	// 파티원을 찾지 못했다.
#define E_ALREADY_EXTEND		(E_FAIL + 17)	// 이미 확장된 상태이다.
#define E_CANNOT_MOVE_MAP		(E_FAIL + 18)	// 이동할 수 없는 맵이다.
#define E_RETURN_REJECT			(E_FAIL + 19)	// 거절했다.
#define E_ALREADY_CREATED		(E_FAIL + 20)	// 이미 생성 되어 있다.
#define E_CANNOT_EXTEND			(E_FAIL + 21)	// 확장 할 수 없다.


// 캐릭터 카드 관련 에러 코드
#define E_NOT_FOUND_CARD		(E_FAIL + 22)	// 카드를 찾을수 없다.
#define E_NOT_ENOUGH_RP			(E_FAIL + 23)	// 추천 포인트가 부족하다.
#define E_DISABLE_CARD			(E_FAIL + 24)	// 카드가 활성화 되지 않았다.
#define E_NOT_ENOUGH_LEVEL		(E_FAIL + 25)	// 레벨이 부족하다?
#define E_BADSTRING				(E_FAIL + 26)	// 금칙어 사용
#define E_CANNOT_STATE_MAPMOVE	(E_FAIL + 27)	// 맵이동을 할수 없는 상태
#define E_SYSTEM_ERROR			(E_FAIL + 28)	// System error
#define E_ADDED_SYSTEM_INVEN	(E_FAIL + 29)	// System inven 에 아이템 작업이 되었다.

// 몬스터가 스킬을 쓰고 죽어야 할 때 리턴될 값
#define E_MON_DIE_SKILL			(E_FAIL + 30)
#define E_CANNOT_EXTEND_CHARACTER_NUM (E_FAIL + 31)	// 더 이상 캐릭터 생성 슬롯을 확장할 수 없다.
#define E_NOT_ENOUGH			(E_FAIL + 32)
#define E_LIMITED_STR			(E_FAIL + 33)
#define E_LIMITED_INT			(E_FAIL + 34)
#define E_LIMITED_CON			(E_FAIL + 35)
#define E_LIMITED_DEX			(E_FAIL + 36)
#define E_WRONG_REQUEST			(E_FAIL + 37)

// SERVER Setting 관련
#define E_INVALID_CHANNEL		(E_FAIL + 51)

// EventView 개체 관련
#define E_SYNCHED_FIRST_GAMETIME	(E_FAIL + 70)
#define E_SYNCHED_AGAIN_GAMETIME	(E_FAIL + 71)
#define E_SYNCHED_GAMETIME			(E_FAIL + 72)

// AntiHacking 관련
#define E_ANTIHACK_WRONG_GUID	(E_FAIL + 101)
#define E_ANTIHACK_DISTANCE		(E_FAIL + 102)
#define E_ANTIHACK_CANNOTFIND	(E_FAIL + 103)
#define E_ANTIHACK_MISMATCH_ID	(E_FAIL + 104)
#define E_ANTIHACK_MAX_OVER		(E_FAIL + 105)
#define E_ANTIHACK_USED			(E_FAIL + 106)

//Hacking 관련
#define E_ACTION_COUNT_SLOW_PACKET (E_FAIL + 201)
#define E_ACTION_COUNT_CASTING_CANCEL (E_FAIL + 202)



// 공통 에러 (E_FAIL + 1000) - (E_FAIL + 2999)
#define E_COMMON_CANNOT_FIND	(E_FAIL + 1000)
#define E_COMMON_TIMEOUT		(E_FAIL + 1001)

#define E_CANT_BEGINQUEST		(E_FAIL+1002)
#define E_CANT_DELQUEST			(E_FAIL+1003)
#define E_CANT_UPDATEQUEST		(E_FAIL+1004)
#define E_CANT_UPDATEQUESTPARAM (E_FAIL+1005)
#define E_CANT_DEFEND_REWORD_FAIL (E_FAIL+1006)	// 피로도 관련 보상 못받을시
#define E_CANNOT_READ			(E_FAIL+1007)
#define E_NOT_ENOUGH_MEMORY		(E_FAIL+1008)	// 메모리 부족
#define E_ALREADY_HAVE			(E_FAIL+1009)	// 이미 가지고 있음

#define E_PORTAL_FULL			(E_FAIL+1010)// 포탈을 더 이상 저장 할 수 없다.
#define E_PORTAL_ALREADY_EXISTS (E_FAIL+1011)// 이미 저장된 포탈이다.? 불가능 할텐데... 
#define E_NOT_FOUND_PORTAL		(E_FAIL+1012)// 포탈을 정보를 찾을수 없다.
#define E_CANNOT_SAVE			(E_FAIL+1013)// 저장 할수 없는 맵이다.


#define E_GS_NOT_FOUND_ITEM		(E_FAIL+1100)	// 아이템을 찾을수 없다.
#define E_GS_INVALID_IDX		(E_FAIL+1101)	// 잘못된 소캣 인덱스 이다.
#define E_GS_CANNOT_GEN			(E_FAIL+1102)	// 소캣을 생성 할수 없는 아이템
#define E_GS_ALREADY_GEN		(E_FAIL+1103)	// 이미 소캣을 생성했다.
#define E_GS_NOT_ENOUGH_MONEY	(E_FAIL+1104)	// 돈 부족
#define E_GS_NOT_ENOUGH_SOUL	(E_FAIL+1105)	// 소울 부족
#define E_GS_NOT_GEN			(E_FAIL+1106)
#define E_GS_SOCKET_FULL		(E_FAIL+1107)
#define E_GS_OVER_MAX_IDX		(E_FAIL+1108)
#define E_GS_GEN_SOCKET_FAIL	(E_FAIL+1109)
#define E_GS_CARD_FULL			(E_FAIL+1110)
#define E_GS_NOT_SET_CARD		(E_FAIL+1111)
#define E_GS_SYSTEM_ERROR		(E_FAIL+1112)
#define E_GS_CANNOT_SET			(E_FAIL+1113)	// 해당 카드를 장착 할수 없는 아이템 이다.
#define E_GS_NOT_ENOUGH_ITEM	(E_FAIL+1114)	// 해당 카드를 장착 할수 없는 아이템 이다.
#define E_GS_FAIL_SOCKET		(E_FAIL+1115)	// 소켓이 파괴된 경우

#define E_NOT_FOUND_GEMSTORE	(E_FAIL+1020)// 보석 교환기를 찾을수 없음
#define E_NOT_FOUND_ARTICLE		(E_FAIL+1021)// 교환 물품을 찾을수 없음
#define E_NOT_ENOUGH_GEMS		(E_FAIL+1022)// 보석이 부족함

#define E_NOT_FOUND_ANTIQUE		(E_FAIL+1025)// 골동품을 찾을수 없음

#define E_EM_NOT_FOUND_EMOTION	(E_FAIL+1030)// 가지고 있지 않은 이모션 그룹이다.
#define E_EM_NOT_HAVE_EMOTION	(E_FAIL+1031)// 가지고 있지 않은 이모션 그룹이다.
#define E_EM_ALREADY_HAVE		(E_FAIL+1032)// 이미 가지고 있는 이모션 그룹이다.
#define E_EM_INVALID_TYPE		(E_FAIL+1033)// 잘못된 이모션 타입이다.
#define E_EM_INVALID_EMOTION	(E_FAIL+1034)// 잘못된 이모션 이다.

#define E_RBE_NOT_FOUND_TARGET_ITEM		(E_FAIL+1040)	// 인첸트 정보를 롤백할 대상 아이템을 찾을수 없다.
#define E_RBE_NOT_FOUND_ROLLBACK_ITEM	(E_FAIL+1041)	// 롤백 기능 아이템을 찾을수 없다.
#define E_RBE_NOT_FOUND_ENCHANTINFO		(E_FAIL+1042)	// 롤백할 인첸트 정보가 없다.
#define E_RBE_CANNOT_REPLACE_OPTION		(E_FAIL+1043)	// 옵션을 재설정 할 수 없다.

#define E_OC_NOT_FOUND_TARGET_ITEM		(E_FAIL+1050)	// 오픈할 상자를 찾을수 없다.
#define E_OC_NOT_FOUND_KEY_ITEM			(E_FAIL+1051)	// 열쇠를 찾을수 없다.
#define E_OC_EMPTY_ITEMBAG				(E_FAIL+1052)	// 이건 시스템 에러 지급할 아이템백 테이블이 비어 있는경우 
#define E_OC_NOT_MATCH_KEY				(E_FAIL+1053)	// 맞지 않은 열쇠 이다.

#define E_GAMBLE_NOT_FOUND_ITEM			(E_FAIL+1060)	// 사용할 아이템을 찾을수 없다.
#define E_GAMBLE_OUTOFRANGESRCTR		(E_FAIL+1061)	// 아이템 랜덤 테이블 범위 오류
#define E_GAMBLE_CREATESITEMFAIL		(E_FAIL+1062)	// 아이템 생성 오류
#define E_GAMBLE_INVALIDITEMTIME		(E_FAIL+1063)	// 아이템 사용시간 설정 오류
#define E_GAMBLE_NO_MESSAGE				(E_FAIL+1064)

#define E_ME_INVALID_ACTION				(E_FAIL+1070)	// 해당 유닛이 할 수 없는 액션을 시도 했다.
#define E_ME_NOT_FOUND_EVENT			(E_FAIL+1071)	// 해당 이벤트를 찾을수 없다.
#define E_ME_NOT_FOUND_PCROOM			(E_FAIL+1072)	// 이벤트를 진행중인 PC방을 찾을수 없다.
#define E_ME_NOT_MANUAL_EVENT			(E_FAIL+1073)	// 유저 참여 이벤트가 아니다.(자동으로 진행되는 이벤트인데 유저가 억지로 호출하려 하였을경우)
														// 이부분은 클라이언트에서 미리 막아줄것(ET_MANUAL_EVENT_MIN ~ ET_MANUAL_EVENT_MAX 사이의 이벤트만 사용 가능)

#define E_CI_NOT_FOUND_SOURCEITEM		(E_FAIL+1080)	// 변환할 대상 아이템을 찾을수 없다.
#define E_CI_IS_NOT_CONVERTITEM			(E_FAIL+1081)	// 변환 가능한 아이템이 아니다.
#define E_CI_INVALID_CONVERTNUM			(E_FAIL+1082)	// 변환할 수 있는 수량이 아니다.(0 보다 작거나 같다.)
#define E_CI_NOT_ENOUGH_SOURCEITEM		(E_FAIL+1083)	// 변환할 아이템 수량이 부족하다.

#define E_EXPCARD_NOT_FOUND_ITEM		(E_FAIL+1090)	// 아이템을 찾을수 없다.
#define E_EXPCARD_NOT_EXPCARD			(E_FAIL+1091)	// 경험치 카드가 아니다.
#define E_EXPCARD_NOT_COMPLETE			(E_FAIL+1092)	// 경험치 카드가 가득 차지 않았다.
#define E_EXPCARD_NOT_ENOUGH_LEVEL		(E_FAIL+1093)	// 사용 가능한 레벨이 아니다.
#define E_EXPCARD_OVER_ENABLE_USE		(E_FAIL+1094)	// 하루 동안 사용 가능한 카드 수량이 초과 했다.
#define E_EXPCARD_NOT_MATCH				(E_FAIL+1095)	// 상고지서와 경험치카드가 맞지 않는다.


#define E_MYHOME_ALREADY_HAVE			(E_FAIL+1100)	// 이미 마이홈을 소지 하고 있다.
#define E_MYHOME_NOT_FOUND				(E_FAIL+1101)	// 마이홈을 찾을 수 없다.
#define E_MYHOME_NOT_FOUND_REALTYDEALER (E_FAIL+1102)	// 부동산 판매 npc를 찾을 수 없다.
#define E_MYHOME_NOT_FOUND_AUCTION_ARTICLE (E_FAIL+1103)	// 경매 물품을 찾을수 없다.
#define E_MYHOME_ALREADY_BIDDINIG		(E_FAIL+1104)	// 이미 경매에 참가중이다.
#define E_MYHOME_NOT_HAVE_AUTHORITY		(E_FAIL+1105)	// 권한이 없다.
#define E_MYHOME_NOT_FOUND_ITEM			(E_FAIL+1106)	// 아이템을 찾을수 없다.
#define E_MYHOME_HOME_INVEN_FULL		(E_FAIL+1107)	// 홈에 배치할 수 있는 공간이 없다.
#define E_MYHOME_IS_NOT_HOME_ITEM		(E_FAIL+1108)	// 홈 아이템이 아니다.
#define E_MYHOME_CHECK_FRIENDLIST		(E_FAIL+1109)	// 클라이언트로 가는 에러 코드 아님 --> 서버에서 사용 유저 친구 리스트 메니저로 검사하러 감
#define E_MYHOME_IS_TIMEOUT				(E_FAIL+1110)	// 마이홈 아이템 기간이 만료되었다.
#define E_MYHOME_IS_NOT_AUCTION_ARITCLE	(E_FAIL+1111)	// 경매중인 집이 아니다.
#define E_MYHOME_INVALID_AUCTION_TIME	(E_FAIL+1112)	// 잘 못된 경매 참가 시간이다.
#define E_MYHOME_ALREADY_REG_AUCTION	(E_FAIL+1113)	// 잘 못된 경매 상태 수정 요청이다.
#define E_MYHOME_CANT_MODIFY			(E_FAIL+1114)	// 집 상태를 수정할 수 없는 상태이다.(예를들어 경매중이던가..)
#define E_MYHOME_ALREADY_HAVE_HOME		(E_FAIL+1115)	// 이미 집을 가지고 있다. 우리는 일인 일주택!! 
#define E_MYHOME_HAVE_BIDDING_DEALYTIME	(E_FAIL+1116)	// 재입찰 대기 시간이 남아 있다. 5분 기다려야함
#define E_MYHOME_NEED_MORE_BIDDING_COST (E_FAIL+1117)	// 입찰 금액이 너무 적다.
#define E_MYHOME_NOT_REG_AUCTION		(E_FAIL+1118)	// 경매에 등록되지 않은 집이다.
#define E_MYHOME_IS_BLOCKED				(E_FAIL+1119)	// 기능이 블럭 당해 있다.
#define E_MYHOME_NOT_FOUND_REALTY		(E_FAIL+1120)	// 구매하려는 부동산을 찾을 수 없다.
#define E_MYHOME_NOT_ENOUGH_MONEY		(E_FAIL+1121)	// 구매 비용이 부족하다.
#define E_MYHOME_COST_ONLY_GOLD			(E_FAIL+1122)	// 골드 단위로만 등록할 수 있다.
#define E_MYHOME_IS_NOT_PAY_TEX_TIME	(E_FAIL+1123)	// 세금 지급할 기간이 아니다.
#define E_MYHOME_INVALID_EQUIP_POS		(E_FAIL+1124)	// 잘못된 장착 위치이다.
#define E_MYHOME_TODAY_INVATION_LIMIT	(E_FAIL+1125)	// 초청장을 모두 사용했다.
#define E_MYHOME_NOT_FOUND_VISITOR		(E_FAIL+1126)	// 방문자를 찾을수 없다.
#define E_MYHOME_DB_ERROR				(E_FAIL+1127)	// 디비 오류 
#define E_MYHOME_CANT_POST_SELF			(E_FAIL+1128)	// 자기 자신에겐 보낼 수 없다.
#define E_MYHOME_INVALID_LEVEL_LIMIT	(E_FAIL+1129)	// 홈 경매 레벨이 맞지 않는다.(높거나 낮다.)

#define E_MYHOME_NOT_HOMETOWN			(E_FAIL+1130)	// 홈타운이 아니다.
#define E_MYHOME_NOT_FOUND_MAP			(E_FAIL+1131)	// 맵을 찾을수 없다.
#define E_MYHOME_CANT_MOVE_MAP			(E_FAIL+1132)	// 이동할 수 없는 맵이다.
#define E_MYHOME_NOT_OPENED_MAP			(E_FAIL+1133)	// 열리지 않은 맵이다.
#define E_MYHOME_INVALID_COST_LIMIT		(E_FAIL+1134)	// 등록 가격이 이 맞지 않는다.(높거나 낮다.)
#define E_MYHOME_ALREADY_STARTED_JOB	(E_FAIL+1135)	// 이미 시작된 아르바이트
#define E_MYHOME_NOT_FOUND_JOB			(E_FAIL+1136)	// 실행중인 아르바이트가 없다.
#define E_MYHOME_HAVE_SIDE_JOB			(E_FAIL+1137)	// 아르바이트중에는 사용 불가능한 동작이다.
#define E_MYHOME_INVALID_TIME_LIMIT		(E_FAIL+1138)	// 정의되지 않은 아르바이트 기간이다.
#define E_MYHOME_CANT_BUY_BIDDING		(E_FAIL+1139)	// 경매중에는 구입 불가.
#define E_MYHOME_NOT_HAVE_INVATIONCARD	(E_FAIL+1140)	// 초대장을 가지고 있지 않다.
#define E_MYHOME_NEED_PAY_TEX			(E_FAIL+1141)	// 세금을 내야 한다.

#define E_MYHOME_ALREADY_CHAT_ROOM		(E_FAIL+1142)	// 이미 채팅룸안에 있다.
#define E_MYHOME_NOT_FOUND_ROOM			(E_FAIL+1143)	// 채팅룸을 찾을 수 없다.
#define E_MYHOME_THIS_ROOM_FULL			(E_FAIL+1144)	// 참가 인원이 가득 찼다.
#define E_MYHOME_NOT_IN_CHAT_ROOM		(E_FAIL+1145)	// 채팅룸 안이 아니다.
#define E_MYHOME_ALL_CHAT_ROOM_OPENED	(E_FAIL+1146)	// 모든 채팅룸이 열려있다. ( 더 이상 룸을 만들 수 없다.)
#define E_MYHOME_INVALID_GUEST_COUNT	(E_FAIL+1147)	// 잘못된 참가자 제한이다. ( 10을 넘을수 없고 2보다 작을수 없다.)
#define E_MYHOME_NOT_IN_HOME			(E_FAIL+1148)	// 마이홈 안에서만 가능하다.
#define E_MYHOME_NOT_MATCH_PASSWORD		(E_FAIL+1149)	// 페스워드가 틀리다.
#define E_MYHOME_NOT_FOUND_TARGET		(E_FAIL+1150)	// 대상을 찾을수 없다.
#define E_MYHOME_DISABLE_PRIVATEMSG		(E_FAIL+1151)	// 대상이 귓말 거부 상태이다.
#define E_MYHOME_IS_KICKED_ROOM			(E_FAIL+1152)	// 강퇴 당한 방에는 다시 들어갈 수 없다.

#define E_MIXUP_NOT_FOUND_ITEM			(E_FAIL+1201)	// 아이템을 찾을수 없다.
#define E_MIXUP_CANNOT_MIXUP_ITEM		(E_FAIL+1202)	// 조합할 수 없는 아이템이다.
#define E_MIXUP_NOT_MATCH_EQUIP			(E_FAIL+1203)	// 같은 장착 부위가 아니다.
#define E_MIXUP_CANT_USE_RATE_ITEM		(E_FAIL+1204)	// 확률 증가 아이템을 사용할 수 없다.
#define E_MIXUP_NOT_MATCH_GENDER		(E_FAIL+1205)	// 같은 성별 아이템이 아니다.
#define E_MIXUP_NOT_MATCH_GRADE			(E_FAIL+1206)	// 같은 등급의 아이템이 아니다.

#define E_GAMBLEMACHINE_INVALID_COIN		(E_FAIL+1300)	// 잘못된 코인을 사용하였다.
#define E_GAMBLEMACHINE_INVALID_ITEM_DATA	(E_FAIL+1301)	// 잘못된 아이템 테이블이다. (유저에게 보여줄 필요가 있을까...)
#define E_GAMBLEMACHINE_OVER_RANGE_RATE		(E_FAIL+1302)	// 랜덤 확률 범위를 벗어났다.(절대 발생하지 않아야 한다. 버그가 아니라면) 
#define E_GAMBLEMACHINE_NOT_FOUND_COIN		(E_FAIL+1303)	// 코인을 찾을수 없다.
#define E_GAMBLEMACHINE_NOT_ENOUGH_CASH		(E_FAIL+1304)	// 캐시가 부족하다.
#define E_GAMBLEMACHINE_ALREADY_GAMBLING	(E_FAIL+1305)	// 이미 겜블 진행 중이다.
#define E_GAMBLEMACHINE_NOT_GAMBLING		(E_FAIL+1306)	// 겜블 중이 아니다.
#define E_GAMBLEMACHINE_NOT_HAVE_ROULETTE	(E_FAIL+1307)	// 룰렛 카운트가 없다.
#define E_GAMBLEMACHINE_INVALID_MIXPOINT	(E_FAIL+1308)	// 잘못된 조합 포인트 이다.
#define E_GAMBLEMACHINE_INVALID_MIXLISTSIZE	(E_FAIL+1309)	// 잘못된 조합 테이블 이다.

#define E_REPAIR_NOT_FOUND_ITEM			(E_FAIL+1310)	// 수리할 아이템을 찾을수 없다.
#define E_REPAIR_NOT_MATCH_LEVEL		(E_FAIL+1311)	// 아이템의 수리 레벨이 맞지 않는다.
#define E_REPAIR_NEED_NOT_REPAIR		(E_FAIL+1312)	// 수리할 필요 없다.

#define E_ENCHANT_NOT_FOUND_ITEM		(E_FAIL+1313)	// 인첸트할 아이템을 찾을수 없다.
#define E_ENCHANT_CANT_UPGRADE			(E_FAIL+1314)	// 인첸트할 수 없는 아이템이다.

#define E_REPAIR_MAX_DURATION_NOT_FOUND_ITEM (E_FAIL+1315) // 최대 내구도 회복할 아이템을 찾을 수 없다.
#define E_REPAIR_MAX_DURATION_CANT_REPAIR (E_FAIL+1316) // 최대 내구도를 수리 할 필요가 없다.
#define E_REPAIR_CANT_REPAIR			(E_FAIL+1317)	// 수리 가능한 아이템이 아니다.
#define E_ENCHANT_REQUIRE_PLUS_LV_LIMIT (E_FAIL+1318)	// 인첸트할 수 없는 아이템이다.
#define E_ENCHANT_REQUIRE_EQUIP_LIMIT	(E_FAIL+1319)	// 인첸트할 수 없는 아이템이다.

// 길드금고 확장
#define E_CANNOT_GUILD_INVENTORY_EXTEND_LINE (E_FAIL+1400)	// 금고를 확장하려면 추가로 금고를 구입해야 합니다.
#define E_CANNOT_GUILD_INVENTORY_EXTEND_TAB (E_FAIL+1401)	// 더 이상 금고를 추가 할 수 없습니다.

#define E_BUILD_MAPITEMBAG_FAIL			(E_FAIL+2000)	// 아이템 드랍 위치 빌드 에러 (맵 백 아이템 빌드 실패)
#define E_BUILD_MONSTERITEMBAG_FAIL		(E_FAIL+2001)	// 아이템 드랍 위치 빌드 에러 (몬스터 백 아이템 빌드 실패)
#define E_BUILD_ITEMDROPMAPINFO_FAIL	(E_FAIL+2001)	// 아이템 드랍 위치 빌드 에러 (아이템 정보에 드랍 맵 정보 설정 실패)

#define E_REDICE_NOT_FOUND_ITEM			(E_FAIL+3001)	// 아이템 옵션 변경 아이템 사용 에러 ( 대상 아이템이 없음)
#define E_REDICE_NOT_ENOUGH_ITEM		(E_FAIL+3002)	// 아이템 옵션 변경 아이템 사용 에러 ( 아이템 부족 )
#define E_REDICE_NOT_EQUIP_ITEM			(E_FAIL+3003)	// 아이템 옵션 변경 아이템 사용 에러 ( 장비 아이템이 아니다. )

#define E_REDICE_NOT_FOUND_PET			(E_FAIL+3004)	// 펫 옵션 변경 아이템 사용 에러 ( 대상 펫이 없음)
#define E_REDICE_NOT_PET_ITEM			(E_FAIL+3005)	// 펫 옵션 변경 아이템 사용 에러 ( 펫 아이템이 아니다. )
#define E_REDICE_CANT_GENOPT_PET		(E_FAIL+3006)	// 펫 옵션 변경 아이템 사용 에러 ( 옵션을 생성할 수 없는 펫 아이템이다.)


#define E_UPGRADE_OPTION_NOT_FOUND_ITEM		(E_FAIL+3010)	// 아이템 옵션 강화 ( 대상 아이템 없음 )
#define E_UPGRADE_OPTION_NOT_EQUIP_ITEM		(E_FAIL+3011)	// 아이템 옵션 강화 ( 장착 아이템이 아님 )
#define E_UPGRADE_OPTION_CANT_UPGRADE_ITEM	(E_FAIL+3012)	// 아이템 옵션 강화 ( 업그레이드할 수 없는 녀석임 )
#define E_UPGRADE_OPTION_NOT_HAVE_OPTION	(E_FAIL+3013)	// 아이템 옵션 강화 ( 옵션 없음 )


#define E_BASICOPTIONAMP_SUCCESS					(S_OK)			// 아이템 옵션 증폭 성공
#define E_BASICOPTIONAMP_NOT_FOUND_ITEM				(E_FAIL+3020)	// 아이템 옵션 증폭 ( 대상 아이템이 없음 )
#define E_BASICOPTIONAMP_CANT_AMP_ITEM				(E_FAIL+3021)	// 아이템 옵션 증폭 ( 증폭 불가능한 아이템 )
#define E_BASICOPTIONAMP_NOT_FOUND_INSURANCEITEM	(E_FAIL+3022)	// 아이템 옵션 증폭 ( 보험 아이템을 찾을수 없음 )
#define E_BASICOPTIONAMP_NOT_ENOUGH_MONEY			(E_FAIL+3023)	// 아이템 옵션 증폭 ( 돈이 부족함 )
#define E_BASICOPTIONAMP_NOT_ENOUGH_SOUL			(E_FAIL+3024)	// 아이템 옵션 증폭 ( 소울이 부족함 )
#define E_BASICOPTIONAMP_FAIL_BROKEN				(E_FAIL+3025)	// 아이템 옵션 증폭 ( 실패 아이템 파괴 되었음 )
#define E_BASICOPTIONAMP_FAIL						(E_FAIL+3026)	// 아이템 옵션 증폭 ( 실패 )
#define E_BASICOPTIONAMP_NOT_ENOUGH_AMPITEM			(E_FAIL+3027)	// 아이템 옵션 증폭 ( 증폭 아이템 부족함 )
#define E_BASICOPTIONAMP_DBERROR					(E_FAIL+3028)	// 아이템 옵션 증폭 ( 디비 처리 에러 )

#define E_SKILLSET_MAX_LIMIT						(E_FAIL+3029)	// 아이템 최대 사용량 초과 ( 실패 )


#define E_GAMBLEMACHINE_ALREADY_MIXUP				(E_FAIL+3030)	// 커스튬 조합 ( 이미 조합중이다. )
#define E_GAMBLEMACHINE_NOT_MIXUP					(E_FAIL+3031)	// 커스튬 조합 ( 조합중이 아니다. )
#define E_GAMBLEMACHINE_NOT_HAVE_ROULETTE_MIXUP		(E_FAIL+3032)	// 커스튬 조합 ( 더 이상 룰렛을 돌릴수 없다. )
#define E_GAMBLEMACHINE_NOT_FOUND_RESOURCE_ITEM		(E_FAIL+3033)	// 커스튬 조합 ( 조합 재료 아이템을 찾을 수 없다. )
#define E_GAMBLEMACHINE_NEED_ITEM_GRADE				(E_FAIL+3034)	// 커스튬 조합 ( 조합 재로 등급이 있어야 한다. )
#define E_GAMBLEMACHINE_NOT_USE_OVERDATE_ITEM		(E_FAIL+3035)	// 커스튬 조합 ( 기간 만료 아이템은 사용 할 수 없다. )

#define E_INFALLIBLE_SELECTION_SUCCESS	(S_OK) 	// 백발백중 아이템 성공
#define E_INFALLIBLE_SELECTION_NOT_MISSION	(E_FAIL+3050)	// 백발백중 아이템 ( 미션맵에서만 사용할수 있습니다 )
#define E_INFALLIBLE_SELECTION_NOT_FOUND_ITEM (E_FAIL+3051)	// 백발백중 아이템 ( 대상 아이템이 없음 )
#define E_INFALLIBLE_SELECTION_NOT_ENOUGH_ITEM	(E_FAIL+3052)	// 백발백중 아이템 ( 아이템이 부족함 )
#define E_INFALLIBLE_SELECTION_CANT_USE	(E_FAIL+3053)	// 백발백중 아이템 ( 사용할 수 없음 - 다른 파티원이 이미 사용 )
#define E_INFALLIBLE_SELECTION_DBERROR	(E_FAIL+3054)	// 백발백중 아이템 ( 디비 처리 에러 )
#define E_CANNOT_ITEM					(E_FAIL+3055)	// 수호석 HP를 증가 시키지 못한다.(조건 불일치)

#define E_SKILLEXTEND_INVALID_EXTEND				(E_FAIL+3101)	// 스킬 슬롯 확장 ( 정의되지 않은 확장 정보이다. )
#define E_SKILLEXTEND_CANT_EXTEND_LEVEL				(E_FAIL+3102)	// 스킬 슬롯 확장 ( 이미 확장된 렙 보다 낮거나 같다. )
#define E_SKILLEXTEND_INVALID_SKILL					(E_FAIL+3103)	// 스킬 슬롯 확장 ( 정의되지 않은 스킬 번호이다. )
#define E_SKILLEXTEND_INVALID_CLASSLIMIT			(E_FAIL+3104)	// 스킬 슬롯 확장 ( 해당 스킬을 배울수 없는 클레스이다. )

#define E_STRATEGYSKILL_ALREADY_OPEN				(E_FAIL+3108)	// 전략스킬 이미 오픈됨


#define E_RESET_ATTACHED_NOT_FOUND_ITEM				(E_FAIL+3111)	// 코스튬 장착 귀속 해지 아이템 ( 아이템을 찾을 수 없다. )
#define E_RESET_ATTACHED_NOT_ATTACHED				(E_FAIL+3112)	// 코스튬 장착 귀속 해지 아이템 ( 귀속된 아이템이 아니다. )
#define E_RESET_ATTACHED_CANT_RESET					(E_FAIL+3113)	// 코스튬 장착 귀속 해지 아이템 ( 귀속 해지를 할 수 없다. )


#define E_ENCHANT_SHIFT_NOT_FOUND_ITEM				(E_FAIL+3200)	// 인첸트 전이 아이템 ( 필요 아이템을 찾을 수 없다. )
#define E_ENCHANT_SHIFT_CANT_SHIFT_ITEM				(E_FAIL+3201)	// 인첸트 전이 아이템 ( 인첸트 전이할 수 없는 아이템 이다. )
#define E_ENCHANT_SHIFT_NOT_MATCH_LEVELLIMIT		(E_FAIL+3202)	// 인첸트 전이 아이템 ( 보험 아이템 레벨이 맞지 않는다. )
#define E_ENCHANT_SHIFT_NOT_MATCH_EQUIP_LIMIT		(E_FAIL+3203)	// 인첸트 전이 아이템 ( 장착 부위가 맞지 않는다. )
#define E_ENCHANT_SHIFT_NOT_MATCH_WEAPON_TYPE		(E_FAIL+3204)	// 인첸트 전이 아이템 ( 무기 타입이 맞지 않는다. )
#define E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_COST		(E_FAIL+3205)	// 인첸트 전이 아이템 ( 인첸트 전이 비용이 부족하다. )
#define E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_ITEM		(E_FAIL+3206)	// 인첸트 전이 아이템 ( 인첸트 전이 아이템이(무기/방어구 강화 가루) 보족하다. )
#define E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_GEM		(E_FAIL+3207)	// 인첸트 전이 아이템 ( 인첸트 전이 구슬이 부족하다. )
#define E_ENCHANT_SHIFT_NOT_ENOUGH_SHIFT_INSURANCE	(E_FAIL+3208)	// 인첸트 전이 아이템 ( 인첸트 보험 아이템이 부족하다. )
#define E_ENCHANT_SHIFT_UNDER_SHIFT_LEVELLIMIT		(E_FAIL+3209)	// 인첸트 전이 아이템 ( 전이 대상이 재료보다 렙이 낮다. )
#define E_ENCHANT_SHIFT_OVER_SHIFT_LEVELLIMIT		(E_FAIL+3210)	// 인첸트 전이 아이템 ( 전이 대상 가능 레벨 범위가 아니다. )
#define E_ENCHANT_SHIFT_NOT_FIND_SRCITEM			(E_FAIL+3211)	// 인첸트 전이 아이템 ( 재료아이템을 찾을 수 없음. )
#define E_ENCHANT_SHIFT_SAME_SRC_AND_TARGET			(E_FAIL+3212)	// 인첸트 전이 아이템 ( 재료와 목표 아이템이 같음. )
#define E_ENCHANT_SHIFT_SRCITEM_MIN_PLUS_LEVEL 		(E_FAIL+3213)	// 인첸트 전이 아이템 ( 재료아이템이 등록 가능 레벨 보다 작음. )
#define E_ENCHANT_SHIFT_CANT_SHIFT_WEAPON 			(E_FAIL+3214)	// 인첸트 전이 아이템 ( 방어구만 전이 가능 )
#define E_ENCHANT_SHIFT_NOT_MATCH_INSURANCE			(E_FAIL+3215)	// 인첸트 전이 아이템 ( 보험 아이템이 맞지 않는다. )
#define E_ENCHANT_SHIFT_NOT_ENOUGH_GRADE			(E_FAIL+3216)	// 인첸트 전이 아이템 ( 목표아이템의 소울등급이 너무 낮습니다. )

// 여기 부터는 PT_M_C_ANS_ENCHANT_SHIFT 결과가 S_OK 일때 추가적인 처리 결과를 알기 위해서 사용한다.
#define E_ENCHANT_SHIFT_SUCCESS						(E_FAIL+3220)	// 인첸트 전이 아이템 ( 성공적으로 인첸트 전이를 했다. )
#define E_ENCHANT_SHIFT_FAIL_DESTROY				(E_FAIL+3221)	// 인첸트 전이 아이템 ( 인첸트 전이에 실패해서 아이템이 파괴되었다. )
#define E_ENCHANT_SHIFT_FAIL_WIDTH_INSURANCE		(E_FAIL+3222)	// 인첸트 전이 아이템 ( 인첸트 전이가 보험 아이템에 의해서 보호되었다. )

#define E_JS3_CREATEITEM_SUCCESS					(E_FAIL+3240)	// 채집3차 아이템 ( 성공적으로 아이템 생성 )
#define E_JS3_CREATEITEM_NOT_FOUND_ITEM				(E_FAIL+3241)	// 채집3차 아이템 ( 생성할 아이템을 찾을 수 없음 )
#define E_JS3_CREATEITEM_NOT_ENOUGH_NEED_ITEM		(E_FAIL+3242)	// 채집3차 아이템 ( 재료아이템이 모자람 )
#define E_JS3_CREATEITEM_FAIL_DESTORY				(E_FAIL+3243)	// 채집3차 아이템 ( 꽝 )
#define E_JS3_CREATEITEM_FAIL_TARGETITEM			(E_FAIL+3244)	// 채집3차 아이템 ( 목표 아이템 생성 실패 )
#define E_JS3_CREATEITEM_NOT_LEARN_NEEDSKILL		(E_FAIL+3245)	// 채집3차 아이템 ( 필요스킬을 배우지 못햇음 )
#define E_JS3_CREATEITEM_NOT_ENOUGH_NEED_PRODUCTPOINT		(E_FAIL+3246)	// 채집3차 아이템 ( 생산력이 모자람 )
#define E_JS3_CREATEITEM_NOT_ENOUGH_INVENTORY		(E_FAIL+3247)	// 채집3차 아이템 ( 인벤토리 공간이 모자람 )

//보물 상자
#define E_TREASURE_CHEST_UNMATCH					(E_FAIL + 3260) //등록되지 않은 열쇠를 사용한 경우 (상자!=열쇠)
#define E_TREASURE_CHEST_NOT_FOUND_CHEST			(E_FAIL + 3261) //등록되지 않은 상자를 사용한 경우
#define E_TREASURE_CHEST_NOT_FOUND_CHEST_INV		(E_FAIL + 3262) //인벤 내에 상자 없음(인벤에서 아이템 이동한 경우)
#define E_TREASURE_CHEST_NOT_FOUND_KEY_INV			(E_FAIL + 3263) //인벤 내에 열쇠 없음(인벤에서 아이템 이동한 경우)
#define E_TREASURE_CHEST_FAILED_TO_GET_TABLE		(E_FAIL + 3264) //맵서버에서 테이블 얻기를 실패한 경우

//PVP LEAGUE REGIST TEAM
#define E_PVPLEAGUE_REGIST_TEAM_SUCCESS						(S_OK)	 // PvP리그 ( 팀 등록 성공 )
#define E_PVPLEAGUE_REGIST_TEAM_NAME_DUPLICATE				(E_FAIL+3300) // PvP리그 ( 팀명이 중복되었다 )	
#define E_PVPLEAGUE_REGIST_TEAM_LIMIT_COUNT					(E_FAIL+3301) // PvP리그 ( 팀 갯수 한도 초과 )
#define E_PVPLEAGUE_REGIST_TEAM_WRONG_STATE					(E_FAIL+3302) // PvP리그 ( 등록할수 없는 상태 입니다 )
#define E_PVPLEAGUE_REGIST_TEAM_ERROR_SYSTEM				(E_FAIL+3303) // PvP리그 ( 시스템 에러 입니다 )
#define E_PVPLEAGUE_REGIST_TEAM_MEMBER_DUPLICATE			(E_FAIL+3304) // PvP리그 ( 다른 팀에 등록한 멤버가 있습니다 )

//별자리 던전
#define E_MIS_MATCH_DATA						(E_FAIL + 3401) // 맞지 않는 데이터
#define E_INDUN_MISSION_SUCCESS					(E_FAIL + 3500) // 던전 임무 성공
#define E_INDUN_MISSION_FAIL					(E_FAIL + 3501) // 던전 임무 실패
#define E_NOT_FIND_PARTY_MASTER					(E_FAIL + 3502) // 파티장을 찾을 수 없음
#define E_NOT_FIND_MISSION						(E_FAIL + 3503) // 미션데이터를 찾을 수 없음

#endif // WEAPON_LOHENGRIN_MAINFRAME_ERRORCODE_H