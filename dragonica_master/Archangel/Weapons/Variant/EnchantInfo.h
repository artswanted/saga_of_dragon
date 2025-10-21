//// Enchant Info.
// Hwoarang

#ifndef WEAPON_VARIANT_ITEM_ENCHANTINFO_H
#define WEAPON_VARIANT_ITEM_ENCHANTINFO_H

#include <oledb.h>
#include "BM/ClassSupport.h"

#pragma pack(1)
enum E_ITEM_GRADE
{
	IG_SEAL = -2,//봉인. 강제 셋팅임. 실제 사용은 안됨
	IG_CURSE = -1,//저주. 강제 셋팅임. 실제 사용은 안됨
	
	IG_MIN = 0,
	IG_NORMAL = 0,		//일반
	IG_RARE = 1,		//고급
	IG_UNIQUE = 2,		//스페셜
	IG_ARTIFACT = 3,	//유물
	IG_LEGEND = 4,		//전설
	IG_MAX,
};

enum E_ITEM_DISPLAY_GRADE
{
	IDG_SEAL		= -2,
	IDG_CURSE		= -1,
	IDG_NORMAL		= 0,
	IDG_SET_NORMAL	= 1,
	IDG_SET_GLORY	= 2,
	IDG_SET_CHAOS	= 3,
	IDG_RARE		= 4,
	IDG_UNIQUE		= 5,
	IDG_EPIC		= 6,
	IDG_MAX,
};

typedef enum eItemDetail
{
	ID_NONE		= 0,	//없음
	ID_JUNK		= 1,	//흔한(쓰레기 라는 얘기)
	ID_LOW		= 2,	//서민
	ID_NORMAL	= 3,	//보통
	ID_QUALITY	= 4,	//고급
	ID_BEST		= 5,	//명품
	ID_HERO		= 6,	//영웅의
	ID_LEGEND	= 7,	//전설의
}EItemDetail;
 
typedef enum eItemSeries
{
	IS_NONE		= 0,	//	무등급
	IS_E		= 1,	//	E 등급
	IS_D		= 2,	//	D 등급
	IS_C		= 3,	//	C 등급
	IS_B		= 4,	//	B 등급
	IS_A		= 5,	//	A 등급
	IS_S		= 6,	//	S 등급
	IS_X		= 7,	//	X 등급
}EItemSeries;

typedef struct tagItemTimeInfo
{	//시간 24 비트
	tagItemTimeInfo()
	{
		year= 0;
		mon	= 0;
		day	= 0;
		hour= 0;
		min	= 0;
		pad	= 0;
	}

	bool ConvTime(DBTIMESTAMP &rkTime)const;
private:
	DWORD year	: 7;//2000		12
	DWORD mon	: 4;//16		16		12월
	DWORD day	: 5;//32		21		30일 
	DWORD hour	: 5;//32		26		24시간
	DWORD min	: 6;//64		32		60분
	DWORD pad	: 5;
}SItemTimeInfo;

typedef struct tagEnchantInfoExt
{
	tagEnchantInfoExt()
	{
		Pad(0);
		BrandID(0);
	}
private:
	CLASS_DECLARATION_S_BIT(DWORD, Pad, 16);
	CLASS_DECLARATION_S_BIT(DWORD, BrandID, 16);		//
}SEnchantInfoExt;

const __int64 MONSTER_CARD_MAX_IDX = 4095;	// 2 ^ 12 카드 최대 

typedef enum
{
	GSS_GEN_NONE = 0,
	GSS_GEN_SUCCESS = 1,
	GSS_GEN_FAIL = 2,
}E_GEN_SOCKET_STATE;

//사이즈가 16바이트에서 변하면 아이템 세이브할때 문제생기니 주의.
typedef struct tagEnchantInfo// 셋트 아이템 능력은 아이템 자체로 가지게 하자.
{//	+1 낡은 스마트 단검 오브 임페리얼
public:
	tagEnchantInfo();

	BYTE PlusLv()const;
	void PlusLv(BYTE const kInLv);
	void Clear();
	bool IsEmpty()const;
	bool IsPlus()const;
	bool IsMagic() const;
	int OptionCount() const;

	void BasicOptionTypes(std::list< int > &kOutCont) const;
	void RareOptionTypes(std::list< int > &kOutCont) const;

	bool	EanbleGenSocket(E_ITEM_GRADE const kItemGrade) const;
	bool	DestroySocket();

	bool AdjustValue();
//	int TotalLv()const{return (int)(BasicLv1()+ BasicLv2()+ BasicLv3()+ BasicLv4()+ EnchantLv1() +PlusLv());}
	// 주의사항 :
	//	비트를 나누어 줄때, __int64로 정의 했으므로, 64bit 단위로 나누어 지도록 비트를 쪼개야 한다.

// 0
	CLASS_DECLARATION_S_BIT(unsigned __int64, Rarity, 10);		//10 : 1024
	CLASS_DECLARATION_S_BIT(unsigned __int64, PlusType, 7);		//17 : 128 종	17 //plus는  인첸트에서 씀.
	unsigned __int64 m_kPlusLv:4;
//	CLASS_DECLARATION_S_BIT(unsigned __int64, PlusLv, 4);		//21 : 15종		21
	
	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicType1, 8);	//29 : 얘네는 영력에서 씀
	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicLv1, 4);		//33
	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicType2, 8);	//41
	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicLv2, 4);		//45
	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicType3, 8);	//53
	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicLv3, 4);		//57
	CLASS_DECLARATION_S_BIT(unsigned __int64, IsCurse, 1);		//58 : 저주.
	CLASS_DECLARATION_S_BIT(unsigned __int64, IsSeal, 1);		//59 : 봉인됨.
	CLASS_DECLARATION_S_BIT(unsigned __int64, IsNeedRepair, 1);	//60 : 수리 필요.
	CLASS_DECLARATION_S_BIT(unsigned __int64, IsBinding, 1);	//61 : 바인딩 되었음
	CLASS_DECLARATION_S_BIT(unsigned __int64, IsUnbindReq, 1);	//62 : 언바인딩 요청되어 있음
	CLASS_DECLARATION_S_BIT(unsigned __int64, GenSocketState, 2);//64 : 소켓 생성 상태 0 : 생성 하지 않음 1: 생성 2: 생성 실패	1번
//64

// 0
	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicType4, 8);	//8
	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicLv4, 4);		//12
	CLASS_DECLARATION_S_BIT(unsigned __int64, Attr, 4);			//16	속성. 화수목금토,  EPropertyType
	CLASS_DECLARATION_S_BIT(unsigned __int64, AttrLv, 4);		//20
	CLASS_DECLARATION_S_BIT(unsigned __int64, TimeType, 3);		//23	사용 시간 최대 16834 시간
	CLASS_DECLARATION_S_BIT(unsigned __int64, UseTime, 9);		//32	사용 시간 최대 16834 시간
	CLASS_DECLARATION_S_BIT(unsigned __int64, IsTimeLimit, 1);	//33
	CLASS_DECLARATION_S_BIT(unsigned __int64, IsAttached, 1);	//34	// 이미 장착 했었음 //장착시 귀속.
	CLASS_DECLARATION_S_BIT(unsigned __int64, MonsterCard, 12); //46 : 소켓 카드 장착

	CLASS_DECLARATION_S_BIT(unsigned __int64, SkillOpt, 9 );	//55
	CLASS_DECLARATION_S_BIT(unsigned __int64, RareOptType1, 8);	//63
	unsigned __int64 m_kPlusLvExt:1;
//	CLASS_DECLARATION_S_BIT(unsigned __int64, buf_flag,	1);		//64 : buffer 18 남음

// 64
	CLASS_DECLARATION_S_BIT(unsigned __int64, MagicOpt,	   12);
	CLASS_DECLARATION_S_BIT(unsigned __int64, RareOptLv1, 4);	//16
	
	CLASS_DECLARATION_S_BIT(unsigned __int64, RareOptType2, 8);	//24
	CLASS_DECLARATION_S_BIT(unsigned __int64, RareOptLv2, 4);	//28

	CLASS_DECLARATION_S_BIT(unsigned __int64, RareOptType3, 8);	//36
	CLASS_DECLARATION_S_BIT(unsigned __int64, RareOptLv3, 4);	//40

	CLASS_DECLARATION_S_BIT(unsigned __int64, RareOptType4, 8);	//48
	CLASS_DECLARATION_S_BIT(unsigned __int64, RareOptLv4, 4);	//52

	CLASS_DECLARATION_S_BIT(unsigned __int64, SkillOptLv,	3);	//55

	CLASS_DECLARATION_S_BIT(unsigned __int64, HasEnchantFail, 1);//56

	CLASS_DECLARATION_S_BIT(unsigned __int64, HasPetDead, 1);	//57

	CLASS_DECLARATION_S_BIT(unsigned __int64, IsTimeOuted, 1);	//58 : 사용 기간 만료 플레그 ( 한번 기간 만료처리가 된것은 다시 처리 하지 않기 위해서.. )

	CLASS_DECLARATION_S_BIT(unsigned __int64, IsMCTimeOuted, 1);//59 : 몬스터 카드 사용 기간 만료 플레그 ( 한번 기간 만료처리가 된것은 다시 처리 하지 않기 위해서.. )

	CLASS_DECLARATION_S_BIT(unsigned __int64, BasicAmpLv, 4);	//63 

	CLASS_DECLARATION_S_BIT(unsigned __int64, IsNotice,	1);			//64
// 64
	CLASS_DECLARATION_S_BIT(unsigned __int64, DecDuration, 13);	//13 : 내구도 삭감 수치 (최대 8000)
	CLASS_DECLARATION_S_BIT(unsigned __int64, GenSocketState2, 2);//15 : 소켓 생성 상태 0 : 생성 하지 않음 1: 생성 2: 생성 실패	2번
	CLASS_DECLARATION_S_BIT(unsigned __int64, GenSocketState3, 2);//17 : 소켓 생성 상태 0 : 생성 하지 않음 1: 생성 2: 생성 실패	3번

	CLASS_DECLARATION_S_BIT(unsigned __int64, MonsterCard2, 12); //29 : 소켓 카드 장착2
	CLASS_DECLARATION_S_BIT(unsigned __int64, MonsterCard3, 12); //41 : 소켓 카드 장착3

	CLASS_DECLARATION_S_BIT(unsigned __int64, __buffer3, 23);	//buffer 23 남음

	__int64 Field_1()const{return *(((__int64 const*)(this))+0);}
	__int64 Field_2()const{return *(((__int64 const*)(this))+1);}
	__int64 Field_3()const{return *(((__int64 const*)(this))+2);}
	__int64 Field_4()const{return *(((__int64 const*)(this))+3);}

	void Field_1(__int64 const &kValue){*(((__int64*)(this))+0) = kValue;}
	void Field_2(__int64 const &kValue){*(((__int64*)(this))+1) = kValue;}
	void Field_3(__int64 const &kValue){*(((__int64*)(this))+2) = kValue;}
	void Field_4(__int64 const &kValue){*(((__int64*)(this))+3) = kValue;}

	bool operator == (const tagEnchantInfo &rhs)const
	{
		return (0 == memcmp(this, &rhs, sizeof(tagEnchantInfo)));
	}
	bool operator != (const tagEnchantInfo &rhs)const
	{
		return !(*this == rhs);
	}
}SEnchantInfo;

#pragma pack()

#endif // WEAPON_VARIANT_ITEM_ENCHANTINFO_H