#ifndef WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONACTIONDEF_H
#define WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONACTIONDEF_H

namespace ALRAM_MISSION
{
	typedef enum eAlramType
	{
		AMT_NONE			= 0,//다른 미션에 이어지는 타입으로만 사용
		AMT_GUILDWAR		= 1,//공성전에서 사용
		AMT_BATTLESQUARE	= 2,
	}E_ALRAMTYPE;

	enum eEventResult
	{
		RET_NONE			= S_OK,
		RET_SUCCESS			= S_FALSE,
		RET_NEED_NEWACTION,
		RET_END_READY,
		RET_FAIL			= E_FAIL,
		RET_TIMEOVER		= E_ABORT,
		RET_ERROR			= E_ACCESSDENIED,
	};

	// 다음의 숫자는 DB에서 사용한다 함부로 바꾸지 말자
	enum eEventType
	{
		EVENT_KILL_ANYBODY			= 1,// 누군가를 죽인다.
		EVENT_DIE					= 2,// 죽어라
		EVENT_HIT_ANYBODY			= 3,// 누군가를 때려라
		EVENT_STRUCT				= 4,// 맞는다.
		EVENT_USEITEM				= 5,// 아이템 사용
		EVENT_PICKUP_ITEM			= 6,// 아이템 획득
	};

	enum eObejectType
	{
		OBJECT_ME					= 0,// 나혼자한테만 해당하는 것임
		OBJECT_CLASS				= 1,// 플레이어 클래스 체크
		OBJECT_UNITTYPE				= 2,// UnitType
		OBJECT_HADABIL				= 3,// 어빌을 가지고 있느냐? __int64를 비트플레그로 4개까지 체크한다
		OBJECT_HADABIL_CHKVALUE		= 4,// 어빌을 가지고 있느냐? __int64 앞 4바이트는 타입, 뒤 4바이트는 Value
		OBJECT_ITEMNO				= 5,
		OBJECT_SKILL				= 6,
		OBJECT_DRA_CLASS			= 7,// 플레이어 용족 클래스 체크
	};
};

#endif // WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONACTIONDEF_H