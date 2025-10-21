//// Item Class
// Dukguru
//

#ifndef WEAPON_VARIANT_ITEM_ITEM_H
#define WEAPON_VARIANT_ITEM_ITEM_H

#include "BM/GUID.h"
#include "IGObject.h"
#include "IDObject.h"
#include "EnchantInfo.h"
#include "Lohengrin/packetstruct.h"
#include "Lohengrin/gametime.h"
#include "PgEventView.h"
#include "DefAbilType.h"
#include "StatTrackInfo.h"
#include "PgDailyReward.h"

class CUnit;
class PgPremiumMgr;

typedef enum 
{	
	IT_NONE = 0,
	//1 ~ 7 까지는 DB 테이블 인덱스와 연관이 있기도 하니. 함부로 바꾸지 말 것.
	IT_EQUIP =			1,	// 장비 아이템
	IT_CONSUME =		2,	// 소비 아이템
	IT_ETC =			3,	// 기타 아이템
	IT_CASH =			4,	// 돈(상용 아이템)으로 구매한 아이템

	// 길드금고는 캐릭터 인벤토리를 사용한다.(단, 장비, 소비 같은 구분은 없다)
	IT_GUILD_INVENTORY1 = 1,
	IT_GUILD_INVENTORY2 = 2,
	IT_GUILD_INVENTORY3 = 3,
	IT_GUILD_INVENTORY4 = 4,

	IT_SAFE =			5,	// 창고
	IT_CASH_SAFE =		6,	// 금고(상용 아이템을 저장하는곳)
	IT_FIT =			7,	// 장착된 아이템만 나오는곳
	IT_HOME =			8,
	IT_POST =			9,	// 우편함
	IT_USER_MARKET =	10,	// 경매 보관함
	IT_FIT_CASH =		11,	// 캐쉬 아이템 장착

	IT_RENTAL_SAFE1	=	12, // 기간제 창고1
	IT_RENTAL_SAFE2	=	13, // 기간제 창고2
	IT_RENTAL_SAFE3	=	14, // 기간제 창고3

	IT_PET =			15,	// 펫 아이템 인벤

	IT_SHARE_RENTAL_SAFE1 = 16, // 계정 공유 기간제 창고1
	IT_SHARE_RENTAL_SAFE2 = 17, // 계정 공유 기간제 창고2
	IT_SHARE_RENTAL_SAFE3 = 18, // 계정 공유 기간제 창고3
	IT_SHARE_RENTAL_SAFE4 = 19, // 계정 공유 기간제 창고4

	IT_SOLD_ITEM		= 20,	// 상점에 판매한 아이템 임시 저장 인벤토리

	IT_SAFE_ADDITION	= 25,	// 추가 인벤토리
	IT_FIT_COSTUME		= 26,   // Costume by reOiL
}EInvType;

typedef enum eKindUIIconGroup
{
	KUIG_NONE				= 0,// default error
//IT_ 시리즈와 연동 되는것은 수정 하시오.
	KUIG_EQUIP				= IT_EQUIP,// = 1,
	KUIG_CONSUME			= IT_CONSUME,// = 2,
	KUIG_ETC				= IT_ETC,// = 3,
	KUIG_CASH				= IT_CASH,// = 4,

	KUIG_SAFE				= IT_SAFE,// = 5,
	KUIG_CASH_SAFE			= IT_CASH_SAFE,// = 6,
	KUIG_FIT				= IT_FIT,// = 7, 
	KUIG_HOME				= IT_HOME,
	KUIG_FIT_CASH			= IT_FIT_CASH,

	KUIG_RENTAL_SAFE1		= IT_RENTAL_SAFE1, // 기간제 창고1
	KUIG_RENTAL_SAFE2		= IT_RENTAL_SAFE2, // 기간제 창고2
	KUIG_RENTAL_SAFE3		= IT_RENTAL_SAFE3, // 기간제 창고3

	KUIG_PET				= IT_PET,
	KUIG_SHARE_RENTAL_SAFE1 = IT_SHARE_RENTAL_SAFE1,
	KUIG_SHARE_RENTAL_SAFE2 = IT_SHARE_RENTAL_SAFE2,
	KUIG_SHARE_RENTAL_SAFE3 = IT_SHARE_RENTAL_SAFE3,
	KUIG_SHARE_RENTAL_SAFE4 = IT_SHARE_RENTAL_SAFE4,
	
	KUIG_GUILD_INVENTORY1	= 21,
	KUIG_GUILD_INVENTORY2	= 22,
	KUIG_GUILD_INVENTORY3	= 23,
	KUIG_GUILD_INVENTORY4	= 24,

	KUIG_SAFE_ADDITION		= IT_SAFE_ADDITION,
	KUIG_FIT_COSTUME		= IT_FIT_COSTUME, // Costume update by reOiL

	KUIG_TREASURE_CHEST_KEY = 90, //보물상자 열쇠
	KUIG_MANUFACTURE_MATERIAL = 91, //즉석 가공 (대량 생산 재료 아이템)
	
	KUIG_GUILD_INV_VIEW		= 100,
	KUIG_QUICK_INVEN		= 101,
	KUIG_SKILLTREE			= 102,
	KUIG_GBOX				= 103, 
	KUIG_EFFECT				= 104,
	KUIG_ITEM_UPGRADE_DEST	= 105, 
	KUIG_ITEM_REPAIR		= 106, //수리
	KUIG_ITEM_SHINESTONEUP	= 107, //샤인스톤 업그레이드.
	KUIG_ITEM_TRADE_MY		= 108, //교환-내꺼
	KUIG_ITEM_TRADE_OTHER	= 109, //교환-남에꺼
	KUIG_ITEM_PLUS_UPGRADE_SRC				= 110,
	KUIG_ITEM_PLUS_UPGRADE_INSURENCE		= KUIG_ITEM_PLUS_UPGRADE_SRC + 1,
	KUIG_ITEM_PLUS_UPGRADE_PROBABILITY		= KUIG_ITEM_PLUS_UPGRADE_INSURENCE + 1,
	KUIG_ITEM_RARITY_UPGRADE_SRC			= 115,
	KUIG_ITEM_RARITY_UPGRADE_INSURENCE		= KUIG_ITEM_RARITY_UPGRADE_SRC + 1,
	KUIG_ITEM_RARITY_UPGRADE_PROBABILITY	= KUIG_ITEM_RARITY_UPGRADE_INSURENCE + 1,

	KUIG_ITEM_MAKING		= 120, //제조 인풋.
	KUIG_ITEM_MAKING_OUT	= 121, //제조 아웃풋.
	KUIG_ITEM_MAKING_COOK	= 122, //제조, 요리.
	KUIG_ITEM_SHINE_LOTTERY	= 123, //샤인스톤 뽑기
	KUIG_ITEM_MAKING_MAZE	= 124, //제조 대미궁 아이템
	KUIG_ITEM_UNLOCK_SLOT	= 125, //스페셜 귀속 해제 아이템
	KUIG_ITEM_PET_UPGRADE	= 126, //펫 전직

	KUIG_ITEM_MAIL_RECEIVED	= 130, //
	KUIG_ITEM_MAIL			= 131, //
	
	KUIG_ITEM_BIND			= 140, // 아이템 바인드
	KUIG_ITEM_UNBIND		= 141, // 아이템 언바인드

	KUIG_KEY_SET			= 151,

	KUIG_SOCKET_SYSTEM		= 160,// 소켓 시스템
	KUIG_ITEMCONVERT_SYSTEM	= 161,// 아이템교환 시스템

	KUIG_ITEM_RARITY_BUILDUP_SRC = 170,// 옵션 강화 
	KUIG_ITEM_RARITY_AMPLIFY_SRC = 171,// 옵션 증폭 
	KUIG_ITEM_INFALLIBLE_SELECTION_SRC = 172,// 백발백중
	KUIG_ITEM_RARITY_METERIAL_SRC = 173,

	KUIG_COMBO_SKILL  = 179,	//직업 스킬
	KUIG_JOB_SKILL  = 180,	//직업 스킬
	KUIG_ITEM_SKILL	= 181,	//아이템 스킬
	KUIG_JOB_SKILL3_ITEM	= 182,
	KUIG_JS3_RES_ITEM		= 183, // 채집3차 재료아이템
	KUIG_ITEM_COUNT			= 184, // SItem, 수량없으면 회색처리

	KUIG_EXP_POTION			= 190,	//PC 경험치 포션
	KUIG_EXP_POTION_PET		= 191,	//펫 경험치 포션
	KUIG_CONSUME_PET		= 192,	//펫 소비템
	KUIG_FIT_PET			= 197,	//펫 장착
	KUIG_SKILLTREE_PET		= 198,	//펫 스킬
	KUIG_ITEM_PET_FEED		= 199,	//펫 사료 아이콘
	KUIG_INV_VIEW			= 200,//인벤토리 보기에 있는 아이콘들

	KUIG_VIEW_OTHER_EQUIP	= 201,//남의 인벤장착.
	KUIG_VIEW_OTHER_EQUIP_CASH	= KUIG_VIEW_OTHER_EQUIP+1,//남의 Cash인벤장착.
	KUIG_VIEW_OTHER_EQUIP_COSTUME = KUIG_VIEW_OTHER_EQUIP+2, // Costume by reOiL

	KUIG_STORE				= 220,//상점용 아이콘.
	KUIG_STOCK_STORE		= 221,//주식상점용 아이콘.
	KUIG_TRANSTOWER_FREE	= 222,//전송타워 자유이용권 
	KUIG_MARKET				= 224,//마켓 등록용 아이콘.
	KUIG_MONSTER			= 225,
	KUIG_SYSTEM_INVEN		= 226,
	KUIG_VIEW_ONLY_ITEM		= 227,//그리기, 툴팁에 SItem
	KUIG_MONSTER_BIG		= 228,//정예 몬스터 에너지 게이지용
	KUIG_VIEW_ITEMNO_COUNT	= 229,//아이템 번호와 수량으로 그리기
	KUIG_DEFENCE_ITEM		= 230,//디펜스 모드 아이템
	KUIG_DEFENCE_GUARDIAN	= 231,//디펜스 모드 가디언
	KUIG_DEFENCE_SKILL		= 232,//디펜스 모드 스킬
	KUIG_ANTIQUE_SRC		= 240,//골동품수집 재료 슬롯
	KUIG_ANTIQUE_TGT		= 241,//골동품수집 목표 슬롯
	KUIG_ENCHANT_SHIFT_SRC	= 242,//인챈트전이 재료 슬롯
	KUIG_ENCHANT_SHIFT_TGT	= 243,//인챈트전이 목표 슬롯
	KUIG_SKILLSET_BASIC		= 245,
	KUIG_SKILLSET_JUMPICON	= 246,
	KUIG_SKILLSET_SKILLICON	= 247,
	KUIG_SKILLSET_SETICON	= 248,
	KUIG_REDICE_PET_OPT		= 249,//펫 옵션 변경
	KUIG_REDICE_CRAFT_OPT	= 250,//크래프트 옵션
	KUIG_BOOK_RECOMMEND		= 251,//북,추천장비
	KUIG_COMMON_GUID_TO_GRAY= 252,
	KUIG_COMMON_INV_POS		= 253,
	KUIG_COMMON_ITEMNO		= 254,//아이템 넘버로 그리기
	KUIG_ANY				= 255,// 그룹아님

	//이 값은 0~255 사이의 값이어야 한다.

}EKindUIIconGroup;

typedef enum tagGuildInvTradeType
{
	EGIT_NONE = 0,

	EGIT_MONEY_IN		= 1, // 골드 입금
	EGIT_MONEY_OUT		= 2, // 골드 출금
	EGIT_ITEM_IN		= 3, // 아이템 보관
	EGIT_ITEM_OUT		= 4, // 아이템 꺼내기
	EGIT_ITEM_MOVE		= 5, // 길드금고내 아이템 이동
	EGIT_ITEM_DESTROY	= 6, // 길드금고 아이템 파괴
    EGIT_EMPORIA_RESERVE    = 7, //엠포리아 도전
    EGIT_EMPORIA_THROW      = 8, //엠포리아 포기
    EGIT_EMPORIA_REWARD     = 9, //엠포리아 우승
	EGIT_GM_ORDER			= 10, //지엠 명령
} EGuildInvTradeType;

typedef enum E_ITEM_EXTEND_DATA_TYPE : WORD
{
	IEDT_NONE		= 0,
	IEDT_EXPCARD	= 1,	// 경험치 카드 추가 정보
	IEDT_UNBIND		= 2,	// 아이템 바인드 정보 (이거 언제 수정 하지 ㅠㅠ)
	IEDT_HOME		= 3,	// 마이홈 아이템 추가 정보
	IEDT_MONSTERCARD= 4,	// 기간제 몬스터 카드 정보
}EItemExtendDataType;

#pragma pack(1)

typedef struct tagExpCard	// 경험치 카드 확장 정보
{
	tagExpCard():m_kType(IEDT_EXPCARD),m_kExpPer(ABILITY_RATE_VALUE),m_kCurExp(0),m_kMaxExp(0){}
	CLASS_DECLARATION_S(EItemExtendDataType,Type);
	CLASS_DECLARATION_S(WORD,ExpPer);
	CLASS_DECLARATION_S(__int64,CurExp);
	CLASS_DECLARATION_S(__int64,MaxExp);
	bool operator == (tagExpCard const & rhs)const
	{
		return (Type() == rhs.Type() && ExpPer() == rhs.ExpPer() && CurExp() == rhs.CurExp() && MaxExp() == rhs.MaxExp());
	}
	bool operator != (tagExpCard const & rhs)
	{
		return !((*this) == rhs);
	}
	bool IsEmpty()const{return ((0 == m_kMaxExp) || (0 == m_kExpPer));}
	DEFAULT_TBL_PACKET_FUNC();
}SExpCard;

typedef struct tagHomeItem	// 경험치 카드 확장 정보
{
	tagHomeItem():m_kType(IEDT_HOME),m_kPos(POINT3(1,1,1)),m_kDir(360){}
	CLASS_DECLARATION_S(EItemExtendDataType,Type);
	CLASS_DECLARATION_S(POINT3,Pos);
	CLASS_DECLARATION_S(short,Dir);
	CLASS_DECLARATION_S(BM::GUID,ParentGuid);
	CLASS_DECLARATION_S(BYTE,LinkIdx);
	bool operator == (tagHomeItem const & rhs)const
	{
		return (Type() == rhs.Type() && Pos() == rhs.Pos() && Dir() == rhs.Dir() && ParentGuid() == rhs.ParentGuid() && LinkIdx() == rhs.LinkIdx());
	}
	bool operator != (tagHomeItem const & rhs)
	{
		return !((*this) == rhs);
	}
	bool IsEmpty()const{return (Pos() == POINT3(0,0,0)) && (Dir() == 0) && ParentGuid().IsNull() && (LinkIdx() == 0);}
	DEFAULT_TBL_PACKET_FUNC();
}SHomeItem;

typedef struct tagMonsterCardTimeLimit	// 경험치 카드 확장 정보
{
	tagMonsterCardTimeLimit():m_kType(IEDT_MONSTERCARD){}
	explicit tagMonsterCardTimeLimit(int const iUseTime):m_kType(IEDT_MONSTERCARD)
	{
		SetUseTime(iUseTime);
	}

	CLASS_DECLARATION_S(EItemExtendDataType,Type);
	CLASS_DECLARATION_S(BM::PgPackedTime,LimitTime);
	
	bool operator == (tagMonsterCardTimeLimit const & rhs)const
	{
		return (Type() == rhs.Type() && LimitTime() == rhs.LimitTime());
	}
	
	bool operator != (tagMonsterCardTimeLimit const & rhs)
	{
		return !((*this) == rhs);
	}

	bool IsEmpty()const{return LimitTime().IsNull();}

	bool IsUseTimeOut()
	{
		return 0 >= GetUseAbleTime();
	}

	__int64 GetUseAbleTime()
	{
		__int64 i64LimitSec = 0;
		CGameTime::DBTimeEx2SecTime(BM::DBTIMESTAMP_EX(LimitTime()),i64LimitSec);
		return std::max<__int64>(0,i64LimitSec - g_kEventView.GetLocalSecTime());
	}

	void SetUseTime(int const iUseTime)
	{
		__int64 i64LimitTime = g_kEventView.GetLocalSecTime() + iUseTime;
		BM::DBTIMESTAMP_EX kLimitTime;
		CGameTime::SecTime2DBTimeEx(i64LimitTime,kLimitTime);
		LimitTime(static_cast<BM::PgPackedTime>(kLimitTime));
	}

	DEFAULT_TBL_PACKET_FUNC();
}SMonsterCardTimeLimit;

typedef enum E_USE_ITEM_TYPE
{
	UIT_MIN		= 0,
	UIT_HOUR	= 1,
	UIT_DAY		= 2,
	UIT_NOLIMIT = 3,// its팀에서 사용중이다. 프로그램에선 무기한 구분은 사용 시간이 0 인것은 모두 무기한으로 처리된다.
	UIT_COUNT	= 4,
	UIT_NUM
}EUseItemType;

typedef enum EItemState
{
	UIT_STATE_NORMAL	= 0,
	UIT_STATE_PET		= 1,
}E_ITEM_STATE;

typedef struct tagSItemExtendDataWrapper
{
	tagSItemExtendDataWrapper(){}

	template<typename T>
	tagSItemExtendDataWrapper(T const & kData)
	{
		kData.WriteToPacket(kExtendData);
	}

	template<typename T>
	void Set(T const & kData)
	{
		kExtendData.Reset();
		kData.WriteToPacket(kExtendData);
	}

	template<typename T>
	void Get(T & kData) const
	{
		kExtendData.PosAdjust();
		kData.ReadFromPacket(kExtendData);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kExtendData.Data());
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kExtendData.Data());
	}

	size_t min_size() const
	{
		return sizeof(size_t);
	}

	bool operator==(tagSItemExtendDataWrapper const & rhs) const
	{
		return kExtendData.Data() == rhs.kExtendData.Data();
	}

	bool operator!=(tagSItemExtendDataWrapper const & rhs) const
	{
		return !((*this) == rhs);
	}

private:

	mutable BM::Stream kExtendData;
}SItemExtendDataWrapper;

typedef std::map< EItemExtendDataType, SItemExtendDataWrapper > CONT_ITEMEXTENDDATA;

class PgItemExtendDataMgr
{
public:
	PgItemExtendDataMgr(){}
	PgItemExtendDataMgr(PgItemExtendDataMgr const & rhs){ *this = rhs;}
	~PgItemExtendDataMgr(){}
	
	template<typename T>
	void Set(T const & kValue)
	{
		CONT_ITEMEXTENDDATA::iterator iter = m_kCont.find(kValue.Type());
		if(iter != m_kCont.end())
		{
			(*iter).second.Set(kValue);
		}
		else
		{
			m_kCont.insert(std::make_pair(kValue.Type(),SItemExtendDataWrapper(kValue)));
		}
	}
	
	template<typename T>
	bool Get(T & kValue) const
	{
		CONT_ITEMEXTENDDATA::const_iterator iter = m_kCont.find(kValue.Type());
		if(iter != m_kCont.end())
		{
			(*iter).second.Get(kValue);
			return true;
		}
		return false;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteTable_AM(kPacket,m_kCont);
	}

	bool ReadFromPacket(BM::Stream & kPacket)
	{
		return PU::TLoadTable_AM(kPacket,m_kCont);
	}

	size_t min_size() const
	{
		return sizeof(size_t);
	}

	bool Remove(EItemExtendDataType const kType)
	{
		CONT_ITEMEXTENDDATA::iterator iter = m_kCont.find(kType);
		if(iter == m_kCont.end())
		{
			return false;
		}
		m_kCont.erase(iter);
		return true;
	}

	void Clear()
	{
		m_kCont.clear();
	}

	void Swap( PgItemExtendDataMgr & rkExtendDataMgr )
	{
		m_kCont.swap( rkExtendDataMgr.m_kCont );
	}

	bool operator == (PgItemExtendDataMgr const & rhs) const;

	PgItemExtendDataMgr & operator = (PgItemExtendDataMgr const & rhs);

private:

	CLASS_DECLARATION_S(CONT_ITEMEXTENDDATA,Cont);
};

//-----------------------------------------------------------
// Pet
//-----------------------------------------------------------
class PgItem_ExtInfo
{
public:
	static const BYTE ms_NoneType = UIT_STATE_NORMAL;

public:
	PgItem_ExtInfo(void)
	{}

	virtual ~PgItem_ExtInfo(void)
	{}

	virtual BYTE GetType()const = 0;//E_ITEM_EXTINFO_TYPE사용

	virtual void WriteToPacket( BM::Stream & )const = 0;
	virtual bool ReadFromPacket( BM::Stream & ) = 0;
	virtual size_t max_size(void)const = 0;
	virtual size_t min_size(void)const = 0;
	virtual bool CopyTo( PgItem_ExtInfo * )const = 0;
};

class PgItem_PetInfo
	:	public PgItem_ExtInfo
{
public:
	struct SStateValue
	{
		SStateValue();
		explicit SStateValue( int const iValue );

		int Get()const;
		void Set( int const iValue );

		bool IsUseTimeOut( __int64 const i64TimeType )const;

		CLASS_DECLARATION_S_BIT( unsigned int, Time, 25 );	// 사용시간(시간)
		CLASS_DECLARATION_S_BIT( unsigned int, Value, 7 );			// 1~100
	};

public:
	typedef std::map< WORD, int >	CONT_ABILS;

	static size_t const MAX_PET_NAMELEN	= 16;
	static int const MAX_PET_STATE_VALUE = 100;

	static BYTE const SVFL_HATCHING = 0x80;
	static SItemPos const ms_kPetItemEquipPos;

	// Skill
	static size_t const MAX_SKILL_SIZE = 3;
	static __int64 const SKILL_BASE_TIME = 215242560i64;// 20100401 기본시작시간

public:
	PgItem_PetInfo(void);
	virtual ~PgItem_PetInfo(void){}

	explicit PgItem_PetInfo( PgItem_PetInfo const &rhs );
	PgItem_PetInfo& operator = ( PgItem_PetInfo const &rhs );

	virtual BYTE GetType()const{return UIT_STATE_PET;}//E_ITEM_STATE사용

	virtual void WriteToPacket( BM::Stream &kPacket )const;
	virtual bool ReadFromPacket( BM::Stream &KPacket );
	virtual size_t max_size(void)const;
	virtual size_t min_size(void)const;
	virtual bool CopyTo( PgItem_ExtInfo * )const;

	static bool CheckStateAbil( int &iValue, bool const bCorrect = true );

	bool IsEmpty(void)const{return 0 == m_kClassKey.iClass;}

	bool SetAbil( WORD const wType, int iValue );
	bool AddAbil( WORD const wType, int iValue );
	int GetAbil( WORD const wType )const;

	void SetState_Health( int const iValue ){m_kHealth.Set(iValue);}
	void SetState_Mental( int const iValue ){m_kMental.Set(iValue);}
	int GetState_Health(void)const{return m_kHealth.Get();}
	int GetState_Mental(void)const{return m_kMental.Get();}

	bool GetSkill( size_t const iIndex, SStateValue &rkOutValue )const;
	bool SetSkill( size_t const iIndex, SStateValue const &kStateValue );
	bool FindSkillIndex( unsigned int iSkillIndex, size_t &iOutIndex, bool const IsFindEmpty = true )const;

	CLASS_DECLARATION( SClassKey, m_kClassKey, ClassKey );
	CLASS_DECLARATION( std::wstring, m_wstrName, Name );
	CLASS_DECLARATION( __int64, m_i64Exp, Exp );
	CLASS_DECLARATION( BYTE, m_bySaveFlag, SaveFlag );

private:
	SStateValue		m_kHealth;
	SStateValue		m_kMental;
	int				m_iMP;
	char			m_cColorIndex;
	SStateValue		m_kSkill[MAX_SKILL_SIZE];//이것의 Time은 분단위이다. Time에 SKILL_BASE_TIME를 더해서 사용해야 한다. 2073년까지 버텨준다.
};

class PgBase_Item;

typedef struct tagItemWriteFlag
{
	tagItemWriteFlag()
	{
		HasEnchant(0);
		HasExtend(0);
		HasName(0);
		HasMemo(0);
		HasPetUnit(0);
		Pad(0);
	}

	tagItemWriteFlag(PgBase_Item const &kItem);
	
	CLASS_DECLARATION_S_BIT(BYTE, HasEnchant, 1);	//인챈트 정보가 있나?
	CLASS_DECLARATION_S_BIT(BYTE, HasExtend, 1);	//확장 정보를 가지고 있는가?
	CLASS_DECLARATION_S_BIT(BYTE, HasName, 1);		//이름 정보(경험치 카드) 있나?
	CLASS_DECLARATION_S_BIT(BYTE, HasMemo, 1);		//메모 정보 있나?
	CLASS_DECLARATION_S_BIT(BYTE, HasPetUnit, 1);	//펫 정보 있나?
	CLASS_DECLARATION_S_BIT(BYTE, Pad, 3);	//바인드 정보 있나?

	void WriteToPacket(PgBase_Item const &kItem, BM::Stream &kPacket)const;
	bool ReadFromPacket(PgBase_Item &kItem, BM::Stream &kPacket);
// 64
	bool operator == (tagItemWriteFlag const &rhs)const
	{
		return (0 == memcmp(this, &rhs, sizeof(tagItemWriteFlag)));
	}
	bool operator != (tagItemWriteFlag const &rhs)const
	{
		return !(*this == rhs);
	}
}SItemWriteFlag;

//typedef struct tagItem//크기는 39 바이트
class PgBase_Item
	:	public PgItemExtendDataMgr//크기는 39 바이트
{
	friend struct tagItemWriteFlag;
public:
	PgBase_Item()
		:	m_pkExtInfo(NULL)
	{
		Clear();
		Guid(BM::GUID::Create());//Null아이템이 되는것 방지.
		BM::DBTIMESTAMP_EX kLocalTime;
		g_kEventView.GetLocalTime(kLocalTime);
		CreateDate(static_cast<BM::PgPackedTime>(kLocalTime));//생성시간 엿되는거 방지.
	}

	PgBase_Item( PgBase_Item const &rhs );
	PgBase_Item& operator = ( PgBase_Item const &rhs );
	virtual ~PgBase_Item();
	static PgBase_Item const & NullData(){static PgBase_Item const kNullItem; return kNullItem;}
	void Clear()
	{
		m_kGuid.Clear();
		ItemNo(0);		//4	20
		Count(0);		//2	22	갯수
		m_kEnchantInfo.Clear();	//8 30	인첸트 정보
		State(UIT_STATE_NORMAL);	//1	31상태 E_ITEM_STATE 사용
		m_kCreateDate.Clear();//4 35	생성날짜
		m_kStatTrackInfo.Clear();
		SAFE_DELETE(m_pkExtInfo);
		PgItemExtendDataMgr::Clear();
	}
	
	size_t min_size()const
	{
		size_t iSize = 
			sizeof(Guid())+//16
			sizeof(ItemNo())+		//4	20
			sizeof(Count())+		//2	22	갯수
//			sizeof(EnchantInfo())+	//8 30	인첸트 정보
			sizeof(State())+		//1	31상태
			sizeof(CreateDate());   //4 35	생성날짜
			//sizeof(StatTrackInfo()) ;
//			PgItemExtendDataMgr::min_size();

		if ( m_pkExtInfo )
		{
			iSize += m_pkExtInfo->min_size();
		}
		return iSize;
	}

	size_t max_size()const
	{
		size_t iSize = 
			sizeof(Guid())+//16
			sizeof(ItemNo())+		//4	20
			sizeof(Count())+		//2	22	갯수
			sizeof(EnchantInfo())+	//8 30	인첸트 정보
			sizeof(State())+		//1	31상태
			sizeof(CreateDate())+	//4 35	생성날짜
			sizeof(StatTrackInfo()) + 
			PgItemExtendDataMgr::min_size();

			if ( m_pkExtInfo )
			{
				iSize += m_pkExtInfo->max_size();
			}
		return iSize;
	}

	void WriteToPacket(BM::Stream &kPacket)const;
	bool ReadFromPacket(BM::Stream &kPacket);

	static __int64 const CalcTimeToSec(EUseItemType const eUseTimeType,__int64 const i64UseTime)
	{
		__int64 i64TableTime = 0i64;
		switch(eUseTimeType)
		{
		case UIT_MIN:
			{
				i64TableTime = 60i64;
			}break;
		case UIT_HOUR:
			{
				i64TableTime = 3600i64;
			}break;
		case UIT_DAY:
			{
				i64TableTime = 86400i64;
			}break;
		}
		return i64UseTime * i64TableTime;
	}

	bool SetUseTime(__int64 const i64UseTimeType,__int64 const i64UseTime)
	{
		if((UIT_MIN > i64UseTimeType) || (UIT_DAY < i64UseTimeType))
		{
			return false;
		}

		m_kEnchantInfo.IsTimeLimit(1);

		m_kEnchantInfo.IsTimeOuted(0);

		m_kEnchantInfo.TimeType(i64UseTimeType);

		m_kEnchantInfo.UseTime(i64UseTime);

		BM::DBTIMESTAMP_EX kLocalTime;
		g_kEventView.GetLocalTime(kLocalTime);
		CreateDate(static_cast<BM::PgPackedTime>(kLocalTime));//생성시간 엿되는거 방지.
		return true;
	}

	bool AddUseTime(__int64 const i64UseTimeType,__int64 const i64UseTime)
	{
		if(i64UseTimeType != m_kEnchantInfo.TimeType())
		{
			return false;
		}

		if(true == IsUseTimeOut())
		{
			BM::DBTIMESTAMP_EX kLocalTime;
			g_kEventView.GetLocalTime(kLocalTime);
			CreateDate(static_cast<BM::PgPackedTime>(kLocalTime));//생성시간 엿되는거 방지.
		}

		m_kEnchantInfo.UseTime(i64UseTime + m_kEnchantInfo.UseTime());
		m_kEnchantInfo.IsTimeOuted(0);
		return true;
	}

	__int64 const GetDelTime()const
	{
		if( m_kEnchantInfo.IsTimeLimit() )
		{
			__int64 i64StartTime = 0i64;
			if( CGameTime::SystemTime2SecTime( m_kCreateDate, i64StartTime ) )
			{
				return CalcTimeToSec( static_cast<EUseItemType>(m_kEnchantInfo.TimeType()), m_kEnchantInfo.UseTime()) + i64StartTime;
			}
		}

		return 0i64;
	}

	__int64 const GetUseAbleTime() const
	{
		__int64 const i64DelTime = GetDelTime();
		if ( i64DelTime > 0i64 )
		{
			return i64DelTime - g_kEventView.GetLocalSecTime();
		}
		return 0i64;
	}

	bool IsTimeOuted() const	// 이미 타임 아웃이 된것인지 검사
	{
		return 	m_kEnchantInfo.IsTimeOuted();
	}

	bool IsUseTimeOut() const	// 사용 기간을 검사하여 타임 아웃 되었는지 검사.
	{
		if(!m_kEnchantInfo.IsTimeLimit())
		{
			return false;
		}

		return (GetUseAbleTime() <= 0);
	}

	static bool IsEmpty(PgBase_Item const *pkItem)
	{
		if(!pkItem)
		{
			return true;
		}

		return pkItem->IsEmpty();
	}

	bool IsEmpty()const
	{
		if(!ItemNo())
		{
			return true;
		}
		
		if(!Count())
		{
			return true;
		}

		return false;
	}

	bool operator == (PgBase_Item const &rhs)const
	{
		return 
		(	PgItemExtendDataMgr::operator==(rhs) 
		&&	Guid() == rhs.Guid() 
		&&	ItemNo() == rhs.ItemNo() 
		&&	Count() == rhs.Count() 
		&&	EnchantInfo() == rhs.EnchantInfo() 
		&&	State() == rhs.State() 
		&&	CreateDate() == rhs.CreateDate());
	}

	bool operator != (PgBase_Item const &rhs)const
	{
		if(*this == rhs)
		{
			return false;
		}
		return true;
	}

	int MaxDuration() const;

	CLASS_DECLARATION_S(BM::GUID, Guid);				//16
	CLASS_DECLARATION_S(DWORD, ItemNo);					//4	20
	CLASS_DECLARATION_S(WORD, Count);					//2	22	갯수
	CLASS_DECLARATION_S(SEnchantInfo, EnchantInfo);		//8 30	인첸트 정보	
	CLASS_DECLARATION_S(BM::PgPackedTime, CreateDate);	//4 34	생성날짜
	CLASS_DECLARATION_S(SStatTrackInfo, StatTrackInfo); //5 39 Stat track by reOil

	template< typename T_EXTINFO >
	bool GetExtInfo( T_EXTINFO *&pOutInfo )const
	{
		pOutInfo = dynamic_cast<T_EXTINFO*>( m_pkExtInfo );
		return ( NULL != pOutInfo );
	}

	bool State( BYTE const byState );
	BYTE State(void)const{return m_kState;}

	void Swap( PgBase_Item & rkItem );

private:
	BYTE	m_kState;//1	35상태
	PgItem_ExtInfo *m_pkExtInfo;
};

typedef struct tagUserMarketArticleInfo
{
	tagUserMarketArticleInfo():
		kCostType(0),
		kArticleCost(0),
		kGrade(0),
		kDisplayGrade(0),
		kLevelLimit(0),
		kInvType(0),
		kEquipPos(0),
		kClassLimit(0){}

	BM::GUID			kItemGuId;
	BYTE				kCostType;
	__int64				kArticleCost;
	std::wstring		kItemName;
	int					kGrade;
	int					kDisplayGrade;
	int					kLevelLimit;
	int					kInvType;
	int					kEquipPos;
	__int64				kClassLimit;
	PgBase_Item			kItem;

	size_t min_size() const
	{
		return	sizeof(BM::GUID) +
				sizeof(BYTE) +
				sizeof(__int64) +
				sizeof(size_t) +
				sizeof(int) +
				sizeof(int) +
				sizeof(int) + 
				sizeof(int) +
				sizeof(int) + 
				sizeof(__int64) +
				kItem.min_size();
	}


	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kItemGuId);
		kPacket.Push(kCostType);
		kPacket.Push(kArticleCost);
		kPacket.Push(kItemName);
		kPacket.Push(kGrade);
		kPacket.Push(kDisplayGrade);
		kPacket.Push(kLevelLimit);
		kPacket.Push(kInvType);
		kPacket.Push(kEquipPos);
		kPacket.Push(kClassLimit);
		kItem.WriteToPacket(kPacket);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kItemGuId);
		kPacket.Pop(kCostType);
		kPacket.Pop(kArticleCost);
		kPacket.Pop(kItemName);
		kPacket.Pop(kGrade);
		kPacket.Pop(kDisplayGrade);
		kPacket.Pop(kLevelLimit);
		kPacket.Pop(kInvType);
		kPacket.Pop(kEquipPos);
		kPacket.Pop(kClassLimit);
		kItem.ReadFromPacket(kPacket);
	}
}SUserMarketArticleInfo;

typedef std::list<SUserMarketArticleInfo> CONT_USER_MARKET_ARTICLEINFO_LIST;

typedef struct tagUserMarketArticleList
{	
	tagUserMarketArticleList()
	{
	}

	CONT_USER_MARKET_ARTICLEINFO_LIST kCont;

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kCont.size());
		CONT_USER_MARKET_ARTICLEINFO_LIST::const_iterator itor = kCont.begin();
		while(itor != kCont.end())
		{
			(*itor).WriteToPacket(kPacket); ++itor;
		}
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		CONT_USER_MARKET_ARTICLEINFO_LIST::size_type kSize;
		kPacket.Pop(kSize);
		SUserMarketArticleInfo kArticle;
		for(CONT_USER_MARKET_ARTICLEINFO_LIST::size_type i = 0;i < kSize;i++)
		{
			kArticle.ReadFromPacket(kPacket);
			kCont.push_back(kArticle);
		}
	}
}SUserMarketArticleList;

typedef enum tagStoreSecondType : BYTE
{
	ESST_NONE			= 0,
	ESST_COIN_CHANGE	= 1,
} EStoreSecondType;

typedef struct tagRecvMailInfo
{
	tagRecvMailInfo()
	{
		i64MailIndex = 0;
		kMailState = PMMT_NOT_READ;
		bPamentType = false;
		i64Money = 0;
	}
	
	BM::GUID			kMailGuid;
	__int64				i64MailIndex;
	std::wstring		kFromName;
	std::wstring		kMailTitle;
	std::wstring		kMailText;
	EPostMailModifyType	kMailState;
	bool				bPamentType;
	__int64				i64Money;
	BM::DBTIMESTAMP_EX	kLimitTime;
	PgBase_Item			kItem;

	void operator = (tagRecvMailInfo const & kMailInfo)
	{
		kMailGuid = kMailInfo.kMailGuid;
		i64MailIndex = kMailInfo.i64MailIndex;
		kFromName = kMailInfo.kFromName;
		kMailTitle = kMailInfo.kMailTitle;
		kMailText = kMailInfo.kMailText;
		kMailState = kMailInfo.kMailState;
		bPamentType = kMailInfo.bPamentType;
		i64Money = kMailInfo.i64Money;
		kLimitTime = kMailInfo.kLimitTime;
		kItem = kMailInfo.kItem;
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(kMailGuid);
		kPacket.Push(i64MailIndex);
		kPacket.Push(kFromName);
		kPacket.Push(kMailTitle);
		kPacket.Push(kMailText);
		kPacket.Push(kMailState);
		kPacket.Push(bPamentType);
		kPacket.Push(i64Money);
		kPacket.Push(kLimitTime);
		kItem.WriteToPacket(kPacket);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kMailGuid);
		kPacket.Pop(i64MailIndex);
		kPacket.Pop(kFromName);
		kPacket.Pop(kMailTitle);
		kPacket.Pop(kMailText);
		kPacket.Pop(kMailState);
		kPacket.Pop(bPamentType);
		kPacket.Pop(i64Money);
		kPacket.Pop(kLimitTime);
		kItem.ReadFromPacket(kPacket);
	}
}SRecvMailInfo;

typedef struct tagPT_M_C_POST_NOTI_NEW_MAIL
{
	tagPT_M_C_POST_NOTI_NEW_MAIL()
	{
	}

	CLASS_DECLARATION_S(SRecvMailInfo,NewMail);

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_M_C_POST_NOTI_NEW_MAIL);
		m_kNewMail.WriteToPacket(kPacket);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		m_kNewMail.ReadFromPacket(kPacket);
	}
}SPT_M_C_POST_NOTI_NEW_MAIL;

typedef std::list<SRecvMailInfo> CONT_RECV_MAIL_LIST;

typedef struct tagPT_M_C_POST_ANS_MAIL_RECV
{
	tagPT_M_C_POST_ANS_MAIL_RECV()
	{
		Result(PMRR_SUCCESS);
		ucMailCount(0);
		ucNewMailCount(0);
	}

	CLASS_DECLARATION_S(EPostMailRecvResult,Result);
	CLASS_DECLARATION_S(unsigned char,ucMailCount);
	CLASS_DECLARATION_S(unsigned char,ucNewMailCount);
	CLASS_DECLARATION_S(CONT_RECV_MAIL_LIST,Cont);

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_M_C_POST_ANS_MAIL_RECV);
		kPacket.Push(m_kResult);
		kPacket.Push(m_kucMailCount);
		kPacket.Push(m_kucNewMailCount);
		kPacket.Push(m_kCont.size());
		CONT_RECV_MAIL_LIST::const_iterator itor = m_kCont.begin();
		while(itor != m_kCont.end())
		{
			(*itor).WriteToPacket(kPacket);
		}
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		if(m_kResult)
			return;

		kPacket.Pop(m_kucMailCount);
		kPacket.Pop(m_kucNewMailCount);

		int iCount;
		kPacket.Pop(iCount);
		for(int i = 0;i < iCount;i++)
		{
			SRecvMailInfo kMailInfo;
			kMailInfo.ReadFromPacket(kPacket);
			m_kCont.push_back(kMailInfo);
		}
	}
}SPT_M_C_POST_ANS_MAIL_RECV;

typedef struct tagCONT_ARTICLEINFO_EX : public std::map<BM::GUID,SUserMarketArticleInfo>
{
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(this->size());
		for(tagCONT_ARTICLEINFO_EX::const_iterator iter = this->begin();iter != this->end();++iter)
		{
			(*iter).second.WriteToPacket(kPacket);
		}
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		tagCONT_ARTICLEINFO_EX::size_type kCount = 0;
		kPacket.Pop(kCount);
		for(tagCONT_ARTICLEINFO_EX::size_type i = 0;i < kCount;i++)
		{
			SUserMarketArticleInfo kArticleInfo;
			kArticleInfo.ReadFromPacket(kPacket);
			this->insert(std::make_pair(kArticleInfo.kItemGuId,kArticleInfo));
		}
	}
}CONT_ARTICLEINFO_EX;

typedef struct tagPT_M_C_UM_ANS_ARTICLE_REG
{
	CLASS_DECLARATION_S(EUserMarketResult,Result);
	CLASS_DECLARATION_S(SUserMarketArticleInfo,Article);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_M_C_UM_ANS_ARTICLE_REG);
		kPacket.Push(m_kResult);
		m_kArticle.WriteToPacket(kPacket);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		if(m_kResult)
		{
			return;
		}
		m_kArticle.ReadFromPacket(kPacket);
	}
}SPT_M_C_UM_ANS_ARTICLE_REG;

typedef std::map<SMARKET_KEY,SUserMarketArticleInfo> CONT_MARKET_SEARCH_RESULT;

int const MARET_ARTICLE_ONE_PAGE = 10;

typedef struct tagPT_M_C_UM_ANS_MARKET_QUERY
{
	CLASS_DECLARATION_S(EUserMarketResult,Result);
	CLASS_DECLARATION_S(CONT_MARKET_SEARCH_RESULT,ContMarket);
	CLASS_DECLARATION_S(int,MaxResultCount);

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_M_C_UM_ANS_MARKET_QUERY);
		kPacket.Push(m_kResult);
		PU::TWriteTable_MM(kPacket,m_kContMarket);
		kPacket.Push(m_kMaxResultCount);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		if(m_kResult)
		{
			return;
		}
		PU::TLoadTable_MM(kPacket,m_kContMarket);
		kPacket.Pop(m_kMaxResultCount);
	}
}SPT_M_C_UM_ANS_MARKET_QUERY;

typedef struct tagPT_M_C_UM_ANS_MY_MARKETE_QUERY
{
	CLASS_DECLARATION_S(EUserMarketResult,Result);
	CLASS_DECLARATION_S(std::wstring,MarketName);
	CLASS_DECLARATION_S(std::wstring,CharName);
	CLASS_DECLARATION_S(BM::GUID,OwnerGuId);
	CLASS_DECLARATION_S(int,ArticleCount);
	CLASS_DECLARATION_S(int,DealingCount);
	CLASS_DECLARATION_S(int,MarketGrade);
	CLASS_DECLARATION_S(__int64,OpenTime);
	CLASS_DECLARATION_S(__int64,CloseTime);
	CLASS_DECLARATION_S(__int64,OnlineTime);
	CLASS_DECLARATION_S(__int64,OfflineTime);
	CLASS_DECLARATION_S(int,MarketHP);
	CLASS_DECLARATION_S(int,State);
	CLASS_DECLARATION_S(SUserMarketArticleList,ContArticle);
	//CLASS_DECLARATION_S(SUserMarketDealingList,ContDealing);

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		if(m_kResult)
			return;
		kPacket.Pop(m_kMarketName);
		kPacket.Pop(m_kCharName);
		kPacket.Pop(m_kOwnerGuId);
		kPacket.Pop(m_kMarketGrade);
		kPacket.Pop(m_kOpenTime);
		kPacket.Pop(m_kCloseTime);
		kPacket.Pop(m_kOnlineTime);
		kPacket.Pop(m_kOfflineTime);
		kPacket.Pop(m_kMarketHP);
		kPacket.Pop(m_kState);
		m_kContArticle.ReadFromPacket(kPacket);
		//m_kContDealing.ReadFromPacket(kPacket);
	}
}SPT_M_C_UM_ANS_MY_MARKET_QUERY;

typedef struct tagPT_M_C_UM_ANS_MARKET_ARTICLE_QUERY
{
	CLASS_DECLARATION_S(EUserMarketResult,Result);
	CLASS_DECLARATION_S(SUserMarketArticleList,ContArticle);
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		if(UMR_SUCCESS == Result())
		{
			m_kContArticle.ReadFromPacket(kPacket);
		}
	}
}SPT_M_C_UM_ANS_MARKET_ARTICLE_QUERY;

/*
typedef enum : WORD
{
	// Item 능력치
	EItem_A_Base = 0x000F,
	EItem_A_Normal = 0x0000,	// noraml
	EItem_A_ADegree = 0x0001,	// A Degree
	EItem_A_SDegree = 0x0002,	// S Degree
	// Item Grade
	EItem_G_Base = 0x00F0,
	EItem_G_Normal = 0x0000,	// normal
	EItem_G_Set = 0x00F0,		// Set_Item (=Combo Item)
	EItem_G_Rare = 0x0020,		// Rare item
	EItem_G_Unique = 0x0050,	// Unique item
} EItemGrade;
*/
typedef enum : BYTE
{
	EMoney_Gold		 = 1,
	EMoney_Silver	 = 2,
	EMoney_Copper	 = 3,
	EMoney_Cash		 = 4,
	EMoney_BonusCash = 5,
	EMoeny_GuildMoney= 6,
} EMonetaryUnit;
/*
typedef struct tagSItemGrade
{
	tagSItemGrade() { wItemGrade = 0; }
	tagSItemGrade(WORD const wGrade) { wItemGrade = wGrade; }

	EItemGrade GetDegree() { return (EItemGrade) (EItem_A_Base & wItemGrade); }
	EItemGrade GetGrade() { return (EItemGrade) (EItem_G_Base & wItemGrade); }

	WORD wItemGrade;
} SItemGrade;
*/
typedef struct tagPlayerModifyOrderData_Add_Any// IMET_DELETE_ANY		=100
{//아무꺼나 삭제.
	tagPlayerModifyOrderData_Add_Any()
	{
		ItemNo(0);
		AddCount(0);
	}

	tagPlayerModifyOrderData_Add_Any(int const iInItemNo, int const iInAddCount)
	{
		ItemNo(iInItemNo);//대상 아이템
		AddCount(iInAddCount);
	}
	CLASS_DECLARATION_S(int, ItemNo);//대상 아이템
	CLASS_DECLARATION_S(int, AddCount);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Add_Any;

typedef struct tagPlayerModifyOrderData_Add_Fixed// IMET_DELETE_ANY		=100
{//아무꺼나 삭제.
	tagPlayerModifyOrderData_Add_Fixed()
	{
		ItemNo(0);
		AddCount(0);
		Pos(SItemPos(IT_FIT,0));
	}

	tagPlayerModifyOrderData_Add_Fixed(int const iInItemNo, int const iInAddCount, SItemPos const & kItemPos)
	{
		ItemNo(iInItemNo);//대상 아이템
		AddCount(iInAddCount);//변경될 인첸트
		Pos(kItemPos);
	}

	CLASS_DECLARATION_S(int, ItemNo);//대상 아이템
	CLASS_DECLARATION_S(int, AddCount);
	CLASS_DECLARATION_S(SItemPos,Pos);//저장위치

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Add_Fixed;

typedef struct tagPlayerModifyOrderData_Enchant//	IMET_MODIFY_ENCHANT	=200
{
	tagPlayerModifyOrderData_Enchant()
	{
	}

	explicit tagPlayerModifyOrderData_Enchant(PgBase_Item const &kItem, SItemPos const &kInItemPos, SEnchantInfo const &kInNewEnchantInfo)
	{
		Pos(kInItemPos);
		Item(kItem);//대상 아이템 -> 이건 검증용

		NewEnchantInfo(kInNewEnchantInfo);//변경될 인첸트
		NewGenDate(kItem.CreateDate());// 변경될 생성 날짜
	}

	explicit tagPlayerModifyOrderData_Enchant(PgBase_Item const &kItem, SItemPos const &kInItemPos, SEnchantInfo const &kInNewEnchantInfo,BM::PgPackedTime const & kInNewGenDate)
	{
		Pos(kInItemPos);
		Item(kItem);//대상 아이템 -> 이건 검증용

		NewEnchantInfo(kInNewEnchantInfo);//변경될 인첸트
		NewGenDate(kInNewGenDate);// 변경될 생성 날짜
	}

	CLASS_DECLARATION_S(PgBase_Item, Item);
	CLASS_DECLARATION_S(SItemPos, Pos);
	CLASS_DECLARATION_S(SEnchantInfo, NewEnchantInfo);//변경될 인첸트
	CLASS_DECLARATION_S(BM::PgPackedTime, NewGenDate);//변경될 생성날짜
	
	void WriteToPacket(BM::Stream &kPacket)const 
	{
		m_kItem.WriteToPacket(kPacket);
		kPacket.Push(m_kPos);
		kPacket.Push(m_kNewEnchantInfo);
		kPacket.Push(m_kNewGenDate);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		m_kItem.ReadFromPacket(kPacket);
		kPacket.Pop(m_kPos);
		kPacket.Pop(m_kNewEnchantInfo);
		kPacket.Pop(m_kNewGenDate);
	}

}SPMOD_Enchant;

typedef struct tagPlayerModifyOrderData_ExtendData
{
	tagPlayerModifyOrderData_ExtendData():m_kRemove(false)
	{
	}

	template<typename T>
	explicit tagPlayerModifyOrderData_ExtendData(PgBase_Item const &kItem, SItemPos const &kInItemPos,T const & kExtendData,bool const kRemove = false/*이 옵션은 강제로 확장 정보를 지울 때 사용한다.*/)
	{
		Pos(kInItemPos);
		Item(kItem);//대상 아이템 -> 이건 검증용
		Type(kExtendData.Type());
		Remove(kRemove);
		m_kExtendData.Set(kExtendData);
	}

	CLASS_DECLARATION_S(PgBase_Item, Item);
	CLASS_DECLARATION_S(SItemPos, Pos);
	CLASS_DECLARATION_S(EItemExtendDataType,Type);
	CLASS_DECLARATION_S(bool,Remove);
	
	SItemExtendDataWrapper m_kExtendData;
	
	void WriteToPacket(BM::Stream &kPacket)const 
	{
		m_kItem.WriteToPacket(kPacket);
		kPacket.Push(m_kPos);
		kPacket.Push(m_kType);
		kPacket.Push(m_kRemove);
		m_kExtendData.WriteToPacket(kPacket);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		m_kItem.ReadFromPacket(kPacket);
		kPacket.Pop(m_kPos);
		kPacket.Pop(m_kType);
		kPacket.Pop(m_kRemove);
		m_kExtendData.ReadFromPacket(kPacket);
	}

}SPMOD_ExtendData;

typedef struct tagPlayerModifyOrderData_Modify_Count//	IMET_MODIFY_COUNT	=201
{//지정된 
	tagPlayerModifyOrderData_Modify_Count()
	{
		AddCount(0);//변경될 갯수 -> 지우려면 음수를 넣으면 됨.
		DoRemove(false);
	}

	tagPlayerModifyOrderData_Modify_Count(PgBase_Item const &kItem, SItemPos const &kInItemPos, int const iInAddCount, bool const bDoRemove = false)
	{//
		Item(kItem);//대상 아이템
		Pos(kInItemPos);

		AddCount(iInAddCount);//까일 카운트
		DoRemove(bDoRemove);//지우냐? -> 카운트 무시됨.
	}

	CLASS_DECLARATION_S(PgBase_Item, Item);//대상 아이템
	CLASS_DECLARATION_S(SItemPos, Pos);//위치

	CLASS_DECLARATION_S(int, AddCount);//수정될 양
	CLASS_DECLARATION_S(bool, DoRemove);//수량 무관. 바로 지우기.

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		m_kItem.WriteToPacket(kPacket);
		kPacket.Push(m_kPos);
		kPacket.Push(m_kAddCount);
		kPacket.Push(m_kDoRemove);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		m_kItem.ReadFromPacket(kPacket);
		kPacket.Pop(m_kPos);
		kPacket.Pop(m_kAddCount);
		kPacket.Pop(m_kDoRemove);
	}
}SPMOD_Modify_Count;

typedef struct tagPlayerModifyOrderData_Modify_Count_Pet
:	public tagPlayerModifyOrderData_Modify_Count
{
	tagPlayerModifyOrderData_Modify_Count_Pet()
	{}

	tagPlayerModifyOrderData_Modify_Count_Pet( BM::GUID const &kPetID, PgBase_Item const &kItem, SItemPos const &kInItemPos, int const iInAddCount, bool const bDoRemove = false)
		:	tagPlayerModifyOrderData_Modify_Count(kItem, kInItemPos, iInAddCount, bDoRemove )
		,	m_kPetID(kPetID)
	{//
	}

	CLASS_DECLARATION_S(BM::GUID, PetID);//대상 아이템

	void WriteToPacket(BM::Stream &kPacket)const 
	{// 순서 바꾸지 마시오, 함부로 바꾸시 마시오.
		kPacket.Push(PetID());
		tagPlayerModifyOrderData_Modify_Count::WriteToPacket(kPacket);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_kPetID);
		tagPlayerModifyOrderData_Modify_Count::ReadFromPacket(kPacket);
	}
}SPMOD_Modify_Count_Pet;

typedef struct tagPlayerModifyOrderData_Modify_Pos//IMET_MODIFY_POS
{//돈을까고 뭔가 해야할 일이 있을경우.
	tagPlayerModifyOrderData_Modify_Pos()
	{
	}
/*	해킹 방어가 안되니 사용하지 못하게 하자..
	explicit tagPlayerModifyOrderData_Modify_Pos( SItemPos const &kOrgPos, SItemPos const &kTgtPos )
		:	m_kOrgPos( kOrgPos )
		,	m_kTgtPos( kTgtPos )
	{
//		Item(kItem);
	}
*/
	explicit tagPlayerModifyOrderData_Modify_Pos(SItemPos const &kOrgPos, SItemPos const &kTgtPos, PgBase_Item const & _OrgItem, PgBase_Item const & _TgtItem )
		:	m_kOrgPos( kOrgPos )
		,	m_kTgtPos( kTgtPos )
		,	m_kOrgItemGuid( (_OrgItem.IsEmpty()?BM::GUID::NullData():_OrgItem.Guid()) )
		,	m_kTgtItemGuid( (_TgtItem.IsEmpty()?BM::GUID::NullData():_TgtItem.Guid()) )
	{}

	CLASS_DECLARATION_S(SItemPos, OrgPos);
	CLASS_DECLARATION_S(SItemPos, TgtPos);
	CLASS_DECLARATION_S(BM::GUID, OrgItemGuid);
	CLASS_DECLARATION_S(BM::GUID, TgtItemGuid);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Modify_Pos;

typedef struct tagPlayerModifyOrderData_Modify_Pos_Pet//IMET_MODIFY_POS
:	public tagPlayerModifyOrderData_Modify_Pos
{
	tagPlayerModifyOrderData_Modify_Pos_Pet()
		:	m_bCasterIsPlayer(true)
	{
	}
/*
	explicit tagPlayerModifyOrderData_Modify_Pos_Pet( SItemPos const &kOrgPos, SItemPos const &kTgtPos )
		:	tagPlayerModifyOrderData_Modify_Pos( m_kOrgPos, m_kTgtPos )
	{
	}
*/
	explicit tagPlayerModifyOrderData_Modify_Pos_Pet(SItemPos const &kOrgPos, SItemPos const &kTgtPos, PgBase_Item const & _OrgItem, PgBase_Item const & _TgtItem )
		:	tagPlayerModifyOrderData_Modify_Pos( m_kOrgPos, m_kTgtPos, _OrgItem, _TgtItem )
		,	m_bCasterIsPlayer(true)
	{}

	explicit tagPlayerModifyOrderData_Modify_Pos_Pet(  SItemPos const &kOrgPos, SItemPos const &kTgtPos, PgBase_Item const & _OrgItem, PgBase_Item const & _TgtItem, BM::GUID const &kPetID )
		:	tagPlayerModifyOrderData_Modify_Pos( kOrgPos, kTgtPos, _OrgItem, _TgtItem )
		,	m_kPetID(kPetID)
		,	m_bCasterIsPlayer(true)
	{}

	CLASS_DECLARATION(bool, m_bCasterIsPlayer, CasterIsPlayer);
	CLASS_DECLARATION_S(BM::GUID, PetID);
	DEFAULT_TBL_PACKET_FUNC();

}SPMOD_Modify_Pos_Pet;

typedef struct tagPlayerModifyOrderData_Insert_Fixed//	IMET_INSERT_FIXED	=202	//특정위치에 특정아이템 넣음
{//지정된 대상 삭제.
	tagPlayerModifyOrderData_Insert_Fixed()
	{
		IsAnyPos(false);
	}

	tagPlayerModifyOrderData_Insert_Fixed(PgBase_Item const &kItem, SItemPos const &kInItemPos, bool const bIsAnyPos = false)
	{
		Item(kItem);
		Pos(kInItemPos);
		IsAnyPos(bIsAnyPos);
	}

	CLASS_DECLARATION_S(PgBase_Item, Item);//대상 아이템
	CLASS_DECLARATION_S(SItemPos, Pos);//위치
	CLASS_DECLARATION_S(bool, IsAnyPos);//위치

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		m_kItem.WriteToPacket(kPacket);
		kPacket.Push(m_kPos);
		kPacket.Push(m_kIsAnyPos);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		m_kItem.ReadFromPacket(kPacket);
		kPacket.Pop(m_kPos);
		kPacket.Pop(m_kIsAnyPos);
	}
}SPMOD_Insert_Fixed;

typedef struct tagPlayerModifyOrderData_DB2Inv//	IMET_INSERT_FIXED	=202	//특정위치에 특정아이템 넣음
{//지정된 대상 삭제.
	tagPlayerModifyOrderData_DB2Inv()
	{
	}

	tagPlayerModifyOrderData_DB2Inv(PgBase_Item const &kItem, SItemPos const &kOrgPos)
	{
		Item(kItem);
		OrgPos(kOrgPos);
	}

	CLASS_DECLARATION_S(PgBase_Item, Item);//대상 아이템
	CLASS_DECLARATION_S(SItemPos, OrgPos);//위치

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		m_kItem.WriteToPacket(kPacket);
		kPacket.Push(m_kOrgPos);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		m_kItem.ReadFromPacket(kPacket);
		kPacket.Pop(m_kOrgPos);
	}
}SPMOD_DB2Inv;

typedef struct tagPlayerModifyOrderData_Add_Money//돈 쓰는것도 이벤트화 됨.
{//돈을까고 뭔가 해야할 일이 있을경우.
	tagPlayerModifyOrderData_Add_Money()
	{
		AddMoney(0);
	}

	tagPlayerModifyOrderData_Add_Money(__int64 const iAddMoney)
	{
		AddMoney(iAddMoney);
	}

	CLASS_DECLARATION_S(__int64, AddMoney);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Add_Money;

typedef struct tagPlayerModifyOrderData_Add_CP
{
	tagPlayerModifyOrderData_Add_CP()
	{
		AddCP(0);
	}

	tagPlayerModifyOrderData_Add_CP(int const iAddCP)
	{
		AddCP(iAddCP);
	}

	CLASS_DECLARATION_S(int, AddCP);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Add_CP;

class PgQuickInventory;
typedef struct tagPlayerModifyOrderData_StrategySkillTabChange
{
	tagPlayerModifyOrderData_StrategySkillTabChange(PgQuickInventory & rkQInv):iValue(0),kQInv(rkQInv)
	{}

	int iValue;
	PgQuickInventory & kQInv;

	void WriteToPacket(BM::Stream & rkPacket)const;
	void ReadFromPacket(BM::Stream & rkPacket);
}SPMOD_ADD_StrategySkillTabChange;

typedef struct tagPlayerModifyOrderData_Sys2Inv//	시스템 인벤에서 인벤토리로 옮기기
{//지정된 대상 삭제.
	tagPlayerModifyOrderData_Sys2Inv()
	{
	}

	tagPlayerModifyOrderData_Sys2Inv(BM::GUID const & kItemGuid, SItemPos const & kInvPos, bool const bAnyPos)
	{
		ItemGuid(kItemGuid);
		Pos(kInvPos);
		AnyPos(bAnyPos);
	}

	CLASS_DECLARATION_S(BM::GUID,ItemGuid);//대상 아이템
	CLASS_DECLARATION_S(SItemPos, Pos);//위치
	CLASS_DECLARATION_S(bool, AnyPos);//위치

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Sys2Inv;

typedef struct tagPlayerModifyOrderData_SysItemRemove
{
	tagPlayerModifyOrderData_SysItemRemove(){}
	tagPlayerModifyOrderData_SysItemRemove(BM::GUID const & kItemGuid)
	{
		ItemGuid(kItemGuid);
	}
	CLASS_DECLARATION_S(BM::GUID,ItemGuid);//대상 아이템

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SysItemRemove;

typedef struct tagPlayerModifyOrderData_AddAbil
{
	tagPlayerModifyOrderData_AddAbil():m_kAbilType(0), m_kValue(0){}
	tagPlayerModifyOrderData_AddAbil(int const & iAbilType, __int64 const &iValue):
	m_kAbilType(iAbilType),	m_kValue(iValue){}
	CLASS_DECLARATION_S(int, AbilType);
	CLASS_DECLARATION_S(__int64, Value);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_AddAbil;

typedef struct tagPlayerModifyOrderData_AddAbilEX
{
	tagPlayerModifyOrderData_AddAbilEX():m_kAbilType(0), m_kValue(0), m_kOrgValue(0), m_kExtraValue(0){}
	tagPlayerModifyOrderData_AddAbilEX(int const iAbilType, __int64 const &iValue, __int64 const &iOrgValue, __int64 const &iExtraValue):
	m_kAbilType(iAbilType), m_kValue(iValue), m_kOrgValue(iOrgValue), m_kExtraValue(iExtraValue) {}
	CLASS_DECLARATION_S(int, AbilType);
	CLASS_DECLARATION_S(__int64, Value);
	CLASS_DECLARATION_S(__int64, OrgValue);
	CLASS_DECLARATION_S(__int64, ExtraValue);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_AddAbilEX;

typedef struct tagPlayerModifyOrderData_Point3
{
	tagPlayerModifyOrderData_Point3(){}
	tagPlayerModifyOrderData_Point3(int const & iPointType, POINT3 const &ptPos)
	{
		PointType(iPointType);
		Pos(ptPos);
	}
	CLASS_DECLARATION_S(int, PointType);
	CLASS_DECLARATION_S(POINT3, Pos);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SetPoint3;

typedef struct tagPlayerModifyOrderData_AddWorldMap
{
	tagPlayerModifyOrderData_AddWorldMap(){}
	tagPlayerModifyOrderData_AddWorldMap(int const & iMapNo)
	{
		MapNo(iMapNo);
	}
	CLASS_DECLARATION_S(int, MapNo);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_AddWorldMap;

typedef struct tagPlayerModifyOrderData_SetRecent
{
	tagPlayerModifyOrderData_SetRecent(){}
	tagPlayerModifyOrderData_SetRecent(int const & iLastVillage, int const & iRecentMap)
	{
		LastVillage(iLastVillage);
		RecentMap(iRecentMap);
	}
	CLASS_DECLARATION_S(int, LastVillage);
	CLASS_DECLARATION_S(int, RecentMap);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SetRecent;

typedef struct tagPlayerModifyOrderData_AddSkill
{
	tagPlayerModifyOrderData_AddSkill(){}
	tagPlayerModifyOrderData_AddSkill(int const & iSkillNo, bool const bIsReset = false)
	{
		SkillNo(iSkillNo);
		IsReset(bIsReset);
	}
	CLASS_DECLARATION_S(int, SkillNo);
	CLASS_DECLARATION_S(bool, IsReset);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_AddSkill;

typedef struct tagPlayerModifyOrderData_DelSkill
{
	tagPlayerModifyOrderData_DelSkill(){}
	tagPlayerModifyOrderData_DelSkill(int const iDevideType, int const & iSkillNo)
	{
		DevideType(iDevideType);
		SkillNo(iSkillNo);
	}
	CLASS_DECLARATION_S(int, SkillNo);
	CLASS_DECLARATION_S(int, DevideType);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_DelSkill;

typedef struct tagPlayerModifyOrderData_SetSkillExtend
{
	tagPlayerModifyOrderData_SetSkillExtend(){}
	tagPlayerModifyOrderData_SetSkillExtend(int const & iSkillNo, BYTE const bExtendLevel):m_kSkillNo(iSkillNo),m_kExtendLevel(bExtendLevel){}
	CLASS_DECLARATION_S(int, SkillNo);
	CLASS_DECLARATION_S(BYTE, ExtendLevel);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SetSkillExtend;

typedef struct tagPlayerModifyOrderData_SetAbil
{
	tagPlayerModifyOrderData_SetAbil(){}
	tagPlayerModifyOrderData_SetAbil(int const & iAbilType, __int64 const &iValue)
	{
		AbilType(iAbilType);
		Value(iValue);
	}
	CLASS_DECLARATION_S(int, AbilType);
	CLASS_DECLARATION_S(__int64, Value);

	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SetAbil;

//Begin Quest
//Inc Param
//Set Param
//Drop Quest
//Complete Quest
typedef struct tagPlayerModifyOrderData_AddIngQuest	// IMET_ADD_INGQUEST (Begin/ Drop/ Finished)
{
	tagPlayerModifyOrderData_AddIngQuest(){}
	tagPlayerModifyOrderData_AddIngQuest(int const iQuestID, int const iTargetState)
	{
		QuestID(iQuestID);
		TargetState(iTargetState);
	}
	CLASS_DECLARATION_S(int, QuestID);
	CLASS_DECLARATION_S(int, TargetState);// Begin/ None(Drop)/ Ended(Complete)
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_AddIngQuest;

typedef struct tagPlayerModifyOrderData_IngQuestParam //  IMET_ADD_INGQUEST, IMET_SET_INGQUEST (Inc Param/ Set Param)
{
	tagPlayerModifyOrderData_IngQuestParam(){}
	tagPlayerModifyOrderData_IngQuestParam(int const iQuestID, BYTE const cParamNo, BYTE const cValue, BYTE const cMaxValue, bool const bSet)
	{
		QuestID(iQuestID);
		ParamNo(cParamNo);
		Value(cValue);
		MaxValue(cMaxValue);
		Set(bSet);
	}
	CLASS_DECLARATION_S(int, QuestID);
	CLASS_DECLARATION_S(BYTE, ParamNo);
	CLASS_DECLARATION_S(BYTE, Value);
	CLASS_DECLARATION_S(BYTE, MaxValue);
	CLASS_DECLARATION_S(bool, Set);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_IngQuestParam;

typedef struct tagPlayerModifyOrderData_SetEndQuest // IMET_ADD_ENDQUEST, IMET_SET_ENDQUEST
{
	tagPlayerModifyOrderData_SetEndQuest(){}
	tagPlayerModifyOrderData_SetEndQuest(int const &iQuestID, bool const bTargetClear, bool const bNeedPrevStatus = false, bool const bPrevClear = false)
	{
		QuestID(iQuestID);
		TargetClear(bTargetClear);
		NeedPrevStatus(bNeedPrevStatus);
		PrevClear(bPrevClear);
	}
	CLASS_DECLARATION_S(int, QuestID);
	CLASS_DECLARATION_S(bool, TargetClear);
	CLASS_DECLARATION_S(bool, NeedPrevStatus);
	CLASS_DECLARATION_S(bool, PrevClear);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SetEndQuest;

typedef struct tagPlayerModifyOrderData_AddRankPoint //IMET_ADD_RANK_POINT
{
	tagPlayerModifyOrderData_AddRankPoint(){}
	tagPlayerModifyOrderData_AddRankPoint(int const eType, int const iValue)
	{
		Type(eType);
		Value(iValue);
	}
	CLASS_DECLARATION_S(int, Value);
	CLASS_DECLARATION_S(int, Type);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_AddRankPoint;

typedef enum eSetGuidType
{
	SGT_Guild = 0,
	SGT_Couple,
	SGT_CoupleColor,
} ESetGuidType;

typedef struct tagPlayerModifyOrderData_SetData //IMET_SET_SWEETHEART
{
	tagPlayerModifyOrderData_SetData(){}
	tagPlayerModifyOrderData_SetData(ECoupleCommand const eType, BM::GUID const &rkGuid, int const iValue, BM::PgPackedTime const & kDate)
	{
		Type(eType);
		Guid(rkGuid);
		Value(iValue);
		Date(kDate);
	}	
	CLASS_DECLARATION_S(ECoupleCommand, Type);
	CLASS_DECLARATION_S(BM::GUID, Guid);
	CLASS_DECLARATION_S(int, Value);
	CLASS_DECLARATION_S(BM::PgPackedTime, Date);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SetSweetHeart;

typedef struct tagPlayerModifyOrderData_SetGuid //IMET_SET_GUID
{
	tagPlayerModifyOrderData_SetGuid(){}
	tagPlayerModifyOrderData_SetGuid(ESetGuidType const eType, BM::GUID const &rkGuid)
	{
		Type(eType);
		Guid(rkGuid);
	}
	CLASS_DECLARATION_S(BM::GUID, Guid);
	CLASS_DECLARATION_S(ESetGuidType, Type);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SetGuid;

typedef struct tagPlayerModifyOrderData_SAbil
{
	tagPlayerModifyOrderData_SAbil( CAbilObject::DYN_ABIL::key_type const kType=0, __int64 kValue=0i64 )
		:	m_kType( kType )
		,	m_kValue( kValue )
	{}

	CLASS_DECLARATION_S( CAbilObject::DYN_ABIL::key_type, Type);
	CLASS_DECLARATION_S( __int64, Value);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_SAbil;

typedef struct tagPlayerModifyOrderData_ReadDealing
{
	tagPlayerModifyOrderData_ReadDealing(){}
	tagPlayerModifyOrderData_ReadDealing(BM::GUID const kDealingGuId)
	{
		DealingGuId(kDealingGuId);
	}
	CLASS_DECLARATION_S(BM::GUID, DealingGuId);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_ReadDealing;

typedef struct tagPlayerModifyOrderData_RemoveMarket
{
	tagPlayerModifyOrderData_RemoveMarket(){}
	tagPlayerModifyOrderData_RemoveMarket(BM::GUID const kMarketGuId)
	{
		MarketGuId(kMarketGuId);
	}
	CLASS_DECLARATION_S(BM::GUID, MarketGuId);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_RemoveMarket;

typedef struct tagPlayerModifyOrderData_Complete_Achievement
{
	tagPlayerModifyOrderData_Complete_Achievement(){}
	tagPlayerModifyOrderData_Complete_Achievement(int const iIdx,int const iCategory,int const iRankPoint,int const iUseTime, int const iGroupNo):
	m_kSaveIdx(iIdx), m_kCategory(iCategory), m_kRankPoint(iRankPoint), m_kUseTime(iUseTime), m_kGroupNo(iGroupNo){}
	CLASS_DECLARATION_S(int,SaveIdx);
	CLASS_DECLARATION_S(int,Category);
	CLASS_DECLARATION_S(int,RankPoint);
	CLASS_DECLARATION_S(int,UseTime);
	CLASS_DECLARATION_S(int,GroupNo);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Complete_Achievement;

typedef struct tagPlayerModifyOrderData_Achievement2Inv
{
	tagPlayerModifyOrderData_Achievement2Inv(){}
	tagPlayerModifyOrderData_Achievement2Inv(int const iIdx)
	{
		SaveIdx(iIdx);
	}
	CLASS_DECLARATION_S(int,SaveIdx);
	DEFAULT_TBL_PACKET_FUNC();
}SPlayerModifyOrderData_Achievement2Inv;

typedef struct tagPlayerModifyOrderData_ModifyAchievement
{
	tagPlayerModifyOrderData_ModifyAchievement(){}
	tagPlayerModifyOrderData_ModifyAchievement(int const iIdx,BYTE const bVal)
	{
		SaveIdx(iIdx);
		SaveValue(bVal);
	}
	CLASS_DECLARATION_S(int,SaveIdx);
	CLASS_DECLARATION_S(BYTE,SaveValue);
	DEFAULT_TBL_PACKET_FUNC();
}SPlayerModifyOrderData_ModifyAchievement;

typedef struct tagPlayerModifyOrderData_ModifyAchievementTimeLimit
{
	tagPlayerModifyOrderData_ModifyAchievementTimeLimit(){}
	tagPlayerModifyOrderData_ModifyAchievementTimeLimit(int const iIdx,int const iUseTime):m_kSaveIdx(iIdx),m_kUseTime(iUseTime){}
	CLASS_DECLARATION_S(int,SaveIdx);
	CLASS_DECLARATION_S(int,UseTime);
	DEFAULT_TBL_PACKET_FUNC();
}SOD_ModifyAchievementTimeLimit;

typedef struct tagPlayerModifyOrderData_ModifyHiddenOpen
{
	tagPlayerModifyOrderData_ModifyHiddenOpen(){}
	tagPlayerModifyOrderData_ModifyHiddenOpen(int const iIdx,BYTE const bVal)
	{
		SaveIdx(iIdx);
		SaveValue(bVal);
	}
	CLASS_DECLARATION_S(int,SaveIdx);
	CLASS_DECLARATION_S(BYTE,SaveValue);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Complete_HiddenOpen;

typedef struct tagPlayerModifyOrderData_ModifyRentalSafeTime
{
	tagPlayerModifyOrderData_ModifyRentalSafeTime(){}
	tagPlayerModifyOrderData_ModifyRentalSafeTime(EInvType const kType,int const iUseTime)
	{
		InvType(kType);
		UseTime(iUseTime);
	}
	CLASS_DECLARATION_S(EInvType,InvType);
	CLASS_DECLARATION_S(int,UseTime);
	DEFAULT_TBL_PACKET_FUNC();
}SPlayerModifyOrderData_ModifyRentalSafeTime;

typedef struct tagPlayerModifyOrderData_ModifyOpenMarket
{
	tagPlayerModifyOrderData_ModifyOpenMarket(){}
	tagPlayerModifyOrderData_ModifyOpenMarket(std::wstring const & kMarketName,std::wstring const & kCharName,
		int const iMarketGrade,int const iOnlineTime,int const iOfflineTime,CONT_ARTICLEINFO_EX const & kContArticle)
	{
		MarketName(kMarketName);
		CharName(kCharName);
		MarketGrade(iMarketGrade);
		OnlineTime(iOnlineTime);
		OfflineTime(iOfflineTime);
		ContArticle(kContArticle);
	}

	CLASS_DECLARATION_S(std::wstring,MarketName);
	CLASS_DECLARATION_S(std::wstring,CharName);
	CLASS_DECLARATION_S(int,MarketGrade);
	CLASS_DECLARATION_S(int,OnlineTime);
	CLASS_DECLARATION_S(int,OfflineTime);
	CLASS_DECLARATION_S(CONT_ARTICLEINFO_EX,ContArticle);

	void WriteToPacket(BM::Stream & rkPacket)const
	{
		rkPacket.Push(m_kMarketName);
		rkPacket.Push(m_kCharName);
		rkPacket.Push(m_kMarketGrade);
		rkPacket.Push(m_kOnlineTime);
		rkPacket.Push(m_kOfflineTime);
		m_kContArticle.WriteToPacket(rkPacket);
	}

	void ReadFromPacket(BM::Stream & rkPacket)
	{
		rkPacket.Pop(m_kMarketName);
		rkPacket.Pop(m_kCharName);
		rkPacket.Pop(m_kMarketGrade);
		rkPacket.Pop(m_kOnlineTime);
		rkPacket.Pop(m_kOfflineTime);
		m_kContArticle.ReadFromPacket(rkPacket);
	}
}SPlayerModifyOrderData_ModifyOpenMarket;

typedef struct tagPlayerModifyOrderData_ModifyRemoveArticle
{
	tagPlayerModifyOrderData_ModifyRemoveArticle(){}
	tagPlayerModifyOrderData_ModifyRemoveArticle(BM::GUID const & kArticleGuid,BM::GUID const & kMemberGuid,std::wstring const & kName)
	{
		ArticleGuid(kArticleGuid);
		MemberGuid(kMemberGuid);
		Name(kName);
	}
	CLASS_DECLARATION_S(BM::GUID,ArticleGuid);
	CLASS_DECLARATION_S(BM::GUID,MemberGuid);
	CLASS_DECLARATION_S(std::wstring,Name);

	void WriteToPacket(BM::Stream & rkPacket)const
	{
		rkPacket.Push(m_kArticleGuid);
		rkPacket.Push(m_kMemberGuid);
		rkPacket.Push(m_kName);
	}

	void ReadFromPacket(BM::Stream & rkPacket)
	{
		rkPacket.Pop(m_kArticleGuid);
		rkPacket.Pop(m_kMemberGuid);
		rkPacket.Pop(m_kName);
	}
}SPlayerModifyOrderData_ModifyRemoveArticle;

typedef struct tagPlayerModifyOrderData_ModifyBuyArticle
{
	tagPlayerModifyOrderData_ModifyBuyArticle(){}
	tagPlayerModifyOrderData_ModifyBuyArticle(BM::GUID const & kMemberGuid,BM::GUID const & kMarketGuid,BM::GUID const & kArticleGuid,WORD const wBuyNum,std::wstring const & kBuyerName,__int64 const i64Cash,__int64 const i64Bonus)
	{
		MemberGuid(kMemberGuid);
		MarketGuid(kMarketGuid);
		ArticleGuid(kArticleGuid);
		BuyNum(wBuyNum);
		BuyerName(kBuyerName);
		Cash(i64Cash);
		Bonus(i64Bonus);
	}

	CLASS_DECLARATION_S(BM::GUID,MemberGuid);
	CLASS_DECLARATION_S(BM::GUID,MarketGuid);
	CLASS_DECLARATION_S(BM::GUID,ArticleGuid);
	CLASS_DECLARATION_S(WORD,BuyNum);
	CLASS_DECLARATION_S(std::wstring,BuyerName);
	CLASS_DECLARATION_S(__int64,Cash);
	CLASS_DECLARATION_S(__int64,Bonus);

	void WriteToPacket(BM::Stream & rkPacket)const
	{
		rkPacket.Push(m_kMemberGuid);
		rkPacket.Push(m_kMarketGuid);
		rkPacket.Push(m_kArticleGuid);
		rkPacket.Push(m_kBuyNum);
		rkPacket.Push(m_kBuyerName);
		rkPacket.Push(m_kCash);
		rkPacket.Push(m_kBonus);
	}

	void ReadFromPacket(BM::Stream & rkPacket)
	{
		rkPacket.Pop(m_kMemberGuid);
		rkPacket.Pop(m_kMarketGuid);
		rkPacket.Pop(m_kArticleGuid);
		rkPacket.Pop(m_kBuyNum);
		rkPacket.Pop(m_kBuyerName);
		rkPacket.Pop(m_kCash);
		rkPacket.Pop(m_kBonus);
	}

}SPlayerModifyOrderData_ModifyBuyArticle;

typedef struct tagPlayerModifyOrderData_ModifyMarket
{
	tagPlayerModifyOrderData_ModifyMarket():m_kOnlineTime(0),m_kOfflineTime(0),m_kHistoryPoint(0),m_kGrade(0){}
	tagPlayerModifyOrderData_ModifyMarket(std::wstring const & wstrName,int const iOnlineTime,int const iOfflineTime,int const iHistoryPoint,int const iGrade)
	{
		Name(wstrName);
		OnlineTime(iOnlineTime);
		OfflineTime(iOfflineTime);
		HistoryPoint(iHistoryPoint);
		Grade(iGrade);
	}

	CLASS_DECLARATION_S(std::wstring,Name);
	CLASS_DECLARATION_S(int,OnlineTime);
	CLASS_DECLARATION_S(int,OfflineTime);
	CLASS_DECLARATION_S(int,HistoryPoint);
	CLASS_DECLARATION_S(int,Grade);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(Name());
		kPacket.Push(OnlineTime());
		kPacket.Push(OfflineTime());
		kPacket.Push(HistoryPoint());
		kPacket.Push(Grade());
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kName);
		kPacket.Pop(m_kOnlineTime);
		kPacket.Pop(m_kOfflineTime);
		kPacket.Pop(m_kHistoryPoint);
		kPacket.Pop(m_kGrade);
	}
}SPlayerModifyOrderData_ModifyMarket;

typedef struct tagPlayerModifyOrderData_ModifyMarketState
{
	tagPlayerModifyOrderData_ModifyMarketState(){}
	tagPlayerModifyOrderData_ModifyMarketState(BYTE const bState)
	{
		State(bState);
	}
	CLASS_DECLARATION_S(BYTE,State);
	DEFAULT_TBL_PACKET_FUNC();
}SPlayerModifyOrderData_ModifyMarketState;

typedef struct tagPlayerModifyOrderData_ModifyCashShopGift
{
	tagPlayerModifyOrderData_ModifyCashShopGift() {}
	tagPlayerModifyOrderData_ModifyCashShopGift(std::wstring const& _RecvName)
		: m_kRecvName(_RecvName)
	{}

	CLASS_DECLARATION_S(std::wstring, RecvName);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(m_kRecvName);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kRecvName);
	}
} SPlayerModifyOrderData_ModifyCashShopGift;

typedef struct tagPlayerModifyOrderData_RegMoney
{
	tagPlayerModifyOrderData_RegMoney(){}
	tagPlayerModifyOrderData_RegMoney(short const _money,__int64 const _selcost,bool const _iscash):m_kMoney(_money),m_kSelCost(_selcost),m_kIsCash(_iscash){}
	CLASS_DECLARATION_S(short,Money);
	CLASS_DECLARATION_S(__int64,SelCost);
	CLASS_DECLARATION_S(bool,IsCash);
	DEFAULT_TBL_PACKET_FUNC();
}SOD_RegMoney;

typedef struct tagPlayerModifyOrderData_AddUnbindDate
{
	tagPlayerModifyOrderData_AddUnbindDate(){}
	tagPlayerModifyOrderData_AddUnbindDate(SItemPos const & kItemPos,BYTE const bDelayDays):m_kItemPos(kItemPos),m_kDelayDays(bDelayDays){}
	CLASS_DECLARATION_S(SItemPos,ItemPos);
	CLASS_DECLARATION_S(BYTE,DelayDays);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_AddUnbindDate;

typedef struct tagPlayerModifyOrderData_DelUnbindDate
{
	tagPlayerModifyOrderData_DelUnbindDate(){}
	tagPlayerModifyOrderData_DelUnbindDate(BM::GUID const kItemGuid):m_kItemGuid(kItemGuid){}
	CLASS_DECLARATION_S(BM::GUID,ItemGuid);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_DelUnbindDate;

typedef struct tagPlayerModifyOrderData_SetDefaultItem
{
	tagPlayerModifyOrderData_SetDefaultItem(){}
	tagPlayerModifyOrderData_SetDefaultItem(int const iEquipPos,int const iItemNo):m_kEquipPos(iEquipPos),m_kItemNo(iItemNo){}
	CLASS_DECLARATION_S(int,EquipPos);
	CLASS_DECLARATION_S(int,ItemNo);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_SetDefaultItem;

typedef struct tagPlayerModifyOrderData_InvExtend
{
	tagPlayerModifyOrderData_InvExtend(){}
	tagPlayerModifyOrderData_InvExtend(EInvType const kInvType,BYTE const bExtendNum):m_kInvType(kInvType),m_kExtendNum(bExtendNum){}
	CLASS_DECLARATION_S(EInvType,InvType);
	CLASS_DECLARATION_S(BYTE,ExtendNum);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_InvExtend;

typedef struct tagPlayerModifyOrderData_RefreshDate
{
	tagPlayerModifyOrderData_RefreshDate(){}
	tagPlayerModifyOrderData_RefreshDate(BM::PgPackedTime const & kDate):m_kDate(kDate){}
	CLASS_DECLARATION_S(BM::PgPackedTime,Date);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_RefreshDate;

typedef struct tagPlayerModifyOrderData_SetRecommendPoint
{
	tagPlayerModifyOrderData_SetRecommendPoint(){}
	tagPlayerModifyOrderData_SetRecommendPoint(int const rp):m_kRP(rp){}
	CLASS_DECLARATION_S(int,RP);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_SetRecommendPoint;

typedef struct tagPlayerModifyOrderData_AddRecommendPoint
{
	tagPlayerModifyOrderData_AddRecommendPoint(){}
	tagPlayerModifyOrderData_AddRecommendPoint(int const rp):m_kRP(rp){}
	CLASS_DECLARATION_S(int,RP);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_AddRecommendPoint;

typedef struct tagPlayerModifyOrderData_CreateCard
{
	tagPlayerModifyOrderData_CreateCard(){}
	tagPlayerModifyOrderData_CreateCard(BYTE const bYear,BYTE const bSex,int const iLocal,std::wstring const & kComment,BYTE const bConstellation,BYTE const bHobby,BYTE const bBlood,BYTE const bStyle,int const iBGndNo):
	m_kYear(bYear),m_kSex(bSex),m_kLocal(iLocal),m_kComment(kComment),m_kConstellation(bConstellation),m_kHobby(bHobby),m_kBlood(bBlood),m_kStyle(bStyle),m_kBGndNo(iBGndNo){}
	CLASS_DECLARATION_S(BYTE,Year);
	CLASS_DECLARATION_S(BYTE,Sex);
	CLASS_DECLARATION_S(int,Local);
	CLASS_DECLARATION_S(std::wstring,Comment);

	CLASS_DECLARATION_S(BYTE,Constellation);
	CLASS_DECLARATION_S(BYTE,Hobby);
	CLASS_DECLARATION_S(BYTE,Blood);
	CLASS_DECLARATION_S(BYTE,Style);
	CLASS_DECLARATION_S(int,BGndNo);

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(Year());
		kPacket.Push(Sex());
		kPacket.Push(Local());
		kPacket.Push(Comment());
		kPacket.Push(Constellation());
		kPacket.Push(Hobby());
		kPacket.Push(Blood());
		kPacket.Push(Style());
		kPacket.Push(BGndNo());
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kYear);
		kPacket.Pop(m_kSex);
		kPacket.Pop(m_kLocal);
		kPacket.Pop(m_kComment);
		kPacket.Pop(m_kConstellation);
		kPacket.Pop(m_kHobby);
		kPacket.Pop(m_kBlood);
		kPacket.Pop(m_kStyle);
		kPacket.Pop(m_kBGndNo);
	}
}SMOD_CreateCard;

typedef tagPlayerModifyOrderData_CreateCard SMOD_ModifyCard;

typedef struct tagPlayerModifyOrderData_AddPopularPoint
{
	tagPlayerModifyOrderData_AddPopularPoint(){}
	tagPlayerModifyOrderData_AddPopularPoint(__int64 const & i64Point, std::wstring Owner=L""):m_kPoint(i64Point),m_kOwner(Owner){}
	CLASS_DECLARATION_S(__int64,Point);
	CLASS_DECLARATION_S(std::wstring,Owner);
	
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kPoint);
		kPacket.Push(m_kOwner);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kPoint);
		kPacket.Pop(m_kOwner);
	}
}SMOD_AddPopularPoint;

typedef struct tagPlayerModifyOrderData_Modify_Comment
{
	tagPlayerModifyOrderData_Modify_Comment(){}
	tagPlayerModifyOrderData_Modify_Comment(std::wstring const & kComment):m_kComment(kComment){}
	CLASS_DECLARATION_S(std::wstring,Comment);

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(Comment());
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kComment);
	}
}SMOD_Modify_Comment;

typedef struct tagPlayerModifyOrderData_Modify_CardState
{
	tagPlayerModifyOrderData_Modify_CardState(){}
	tagPlayerModifyOrderData_Modify_CardState(bool const bEnable):m_kEnable(bEnable){}
	CLASS_DECLARATION_S(bool,Enable);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_Modify_CardState;

typedef struct tagPlayerModifyOrderData_Portal_Create
{
	tagPlayerModifyOrderData_Portal_Create(){}
	tagPlayerModifyOrderData_Portal_Create(BM::GUID const & kGuid,std::wstring const & kComment,int const iGndNo,POINT3 const & kPos):
	m_kGuid(kGuid),m_kComment(kComment),m_kGroundNo(iGndNo),m_kPos(kPos){}
	CLASS_DECLARATION_S(BM::GUID,Guid);
	CLASS_DECLARATION_S(std::wstring,Comment);
	CLASS_DECLARATION_S(int,GroundNo);
	CLASS_DECLARATION_S(POINT3,Pos);

	size_t min_size() const
	{
		return sizeof(BM::GUID) + sizeof(size_t) + sizeof(int) + sizeof(POINT3);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kGuid);
		kPacket.Push(m_kComment);
		kPacket.Push(m_kGroundNo);
		kPacket.Push(m_kPos);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kGuid);
		kPacket.Pop(m_kComment);
		kPacket.Pop(m_kGroundNo);
		kPacket.Pop(m_kPos);
	}
}SMOD_Portal_Create;

typedef struct tagPlayerModifyOrderData_Portal_Delete
{
	tagPlayerModifyOrderData_Portal_Delete(){}
	tagPlayerModifyOrderData_Portal_Delete(BM::GUID const & kGuid):m_kGuid(kGuid){}
	CLASS_DECLARATION_S(BM::GUID,Guid);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_Portal_Delete;

typedef struct tagPlayerModifyOrderData_Add_MyHome
{
	tagPlayerModifyOrderData_Add_MyHome(){}
	tagPlayerModifyOrderData_Add_MyHome(short const __buildingno, int const __cost):m_kBuildingNo(__buildingno),m_kCost(__cost){}
	CLASS_DECLARATION_S(short,BuildingNo);
	CLASS_DECLARATION_S(int,Cost);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_Add_MyHome;

typedef struct tagPlayerModifyOrderData_MyHome_Bidding
{
	tagPlayerModifyOrderData_MyHome_Bidding(){}
	tagPlayerModifyOrderData_MyHome_Bidding(short const __street,int const __house,__int64 const __cost):m_kStreetNo(__street),m_kHouseNo(__house),m_kCost(__cost){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(__int64,Cost);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_Bidding;

typedef struct tagPlayerModifyOrderData_MyHome_Auction_Reg
{
	tagPlayerModifyOrderData_MyHome_Auction_Reg(){}
	tagPlayerModifyOrderData_MyHome_Auction_Reg(short const __street,int const __house,__int64 const __cost,int const __hour)
		:m_kStreetNo(__street),m_kHouseNo(__house),m_kCost(__cost),m_kHour(__hour){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(__int64,Cost);
	CLASS_DECLARATION_S(int,Hour);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_Auction_Reg;

typedef struct tagPlayerModifyOrderData_MyHome_Auction_Unreg
{
	tagPlayerModifyOrderData_MyHome_Auction_Unreg(){}
	tagPlayerModifyOrderData_MyHome_Auction_Unreg(short const __street,int const __house):m_kStreetNo(__street),m_kHouseNo(__house){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_Auction_Unreg;

typedef struct tagPlayerModifyOrderData_MyHome_VisitFlag
{
	tagPlayerModifyOrderData_MyHome_VisitFlag(){}
	tagPlayerModifyOrderData_MyHome_VisitFlag(short const __street,int const __house,BYTE const __flag)
		:m_kStreetNo(__street),m_kHouseNo(__house),m_kVisitFlag(__flag){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(BYTE,VisitFlag);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_VisitFlag;

typedef struct tagPlayerModifyOrderData_MyHome_PayTex
{
	tagPlayerModifyOrderData_MyHome_PayTex(){}
	tagPlayerModifyOrderData_MyHome_PayTex(short const __street,int const __house)
		:m_kStreetNo(__street),m_kHouseNo(__house){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_PayTex;

typedef enum E_MYHOME_RETURN_ITEM_TYPE
{
	RIT_ATTACHMENT,
	RIT_SELL,
}eMyHomeReturnItemType;

typedef struct tagPlayerModifyOrderData_MyHome_ReturnItem
{
	tagPlayerModifyOrderData_MyHome_ReturnItem(){}
	tagPlayerModifyOrderData_MyHome_ReturnItem(BM::GUID const & __ownerguid,short const __street,int const __house, eMyHomeReturnItemType const __returntype)
		:m_kOwnerGuid(__ownerguid),m_kStreetNo(__street),m_kHouseNo(__house),m_kReturnType(__returntype){}
	CLASS_DECLARATION_S(BM::GUID,OwnerGuid);
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(eMyHomeReturnItemType,ReturnType);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_ReturnItem;

typedef struct tagPlayerModifyOrderData_SetHomeStyle
{
	tagPlayerModifyOrderData_SetHomeStyle(){}
	tagPlayerModifyOrderData_SetHomeStyle(BM::GUID const & kGuid,short const siStreetNo,int const iHouseNo,int const iEquipPos,int const iItemNo):
	m_kOwnerGuid(kGuid),m_kStreetNo(siStreetNo),m_kHouseNo(iHouseNo),m_kEquipPos(iEquipPos),m_kItemNo(iItemNo){}
	CLASS_DECLARATION_S(BM::GUID,OwnerGuid);
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(int,EquipPos);
	CLASS_DECLARATION_S(int,ItemNo);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_SetHomeStyle;

typedef struct tagPlayerModifyOrderData_MyHome_Modify_Time
{
	tagPlayerModifyOrderData_MyHome_Modify_Time(){}
	tagPlayerModifyOrderData_MyHome_Modify_Time(short const __street,int const __house,BM::PgPackedTime const & __time)
		:m_kStreetNo(__street),m_kHouseNo(__house),m_kTime(__time){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(BM::PgPackedTime,Time);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_Modify_Time;

typedef struct tagPlayerModifyOrderData_MyHome_Set_OwnerInfo
{
	tagPlayerModifyOrderData_MyHome_Set_OwnerInfo(){}
	tagPlayerModifyOrderData_MyHome_Set_OwnerInfo(short const __streetno,int const __houseno,BM::GUID const & __ownerguid,std::wstring const & __ownername)
		:m_kStreetNo(__streetno),m_kHouseNo(__houseno),m_kOwnerGuid(__ownerguid),m_kOwnerName(__ownername){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(BM::GUID,OwnerGuid);
	CLASS_DECLARATION_S(std::wstring,OwnerName);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(m_kStreetNo);
		kPacket.Push(m_kHouseNo);
		kPacket.Push(m_kOwnerGuid);
		kPacket.Push(m_kOwnerName);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kStreetNo);
		kPacket.Pop(m_kHouseNo);
		kPacket.Pop(m_kOwnerGuid);
		kPacket.Pop(m_kOwnerName);
	}
	size_t min_size()const
	{
		return sizeof(short) + sizeof(int) + sizeof(BM::GUID) + sizeof(size_t);
	}
}SMOD_MyHome_MyHome_Set_OwnerInfo;

typedef struct tagPlayerModifyOrderData_MyHome_Set_State
{
	tagPlayerModifyOrderData_MyHome_Set_State(){}
	tagPlayerModifyOrderData_MyHome_Set_State(short const __streetno,int const __houseno,BYTE const __state)
		:m_kStreetNo(__streetno),m_kHouseNo(__houseno),m_kState(__state){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(BYTE,State);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_MyHome_Set_State;

typedef struct tagPlayerModifyOrderData_MyHome_Set_Addr
{
	tagPlayerModifyOrderData_MyHome_Set_Addr(){}
	tagPlayerModifyOrderData_MyHome_Set_Addr(short const __streetno,int const __houseno)
		:m_kStreetNo(__streetno),m_kHouseNo(__houseno){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_MyHome_Set_Addr;

typedef struct tagPlayerModifyOrderData_MyHome_SideJob_Insert
{
	tagPlayerModifyOrderData_MyHome_SideJob_Insert():m_kStreetNo(0), m_kHouseNo(0), m_kSideJob(MSJ_NONE), m_kJobType(MSJRT_NONE), m_kJobRate(0){}
	tagPlayerModifyOrderData_MyHome_SideJob_Insert(short const __streetno, int const __househo, eMyHomeSideJob const __sidejob, eMyHomeSideJobRateType const __jobratetype, int const __jobratevalue, BM::PgPackedTime const & __endtime)
		:m_kStreetNo(__streetno), m_kHouseNo(__househo), m_kSideJob(__sidejob), m_kJobType(__jobratetype), m_kJobRate(__jobratevalue), m_kEndTime(__endtime){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(eMyHomeSideJob,SideJob);
	CLASS_DECLARATION_S(eMyHomeSideJobRateType,JobType);
	CLASS_DECLARATION_S(int,JobRate);
	CLASS_DECLARATION_S(BM::PgPackedTime,EndTime);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_SideJob_Insert;

typedef struct tagPlayerModifyOrderData_MyHome_SideJob_Remove
{
	tagPlayerModifyOrderData_MyHome_SideJob_Remove():m_kSideJob(MSJ_NONE), m_kStreetNo(0), m_kHouseNo(0){}
	tagPlayerModifyOrderData_MyHome_SideJob_Remove(short const __streetno, int const __househo, eMyHomeSideJob const __sidejob)
		:m_kStreetNo(__streetno), m_kHouseNo(__househo), m_kSideJob(__sidejob){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(eMyHomeSideJob,SideJob);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_SideJob_Remove;

typedef struct tagPlayerModifyOrderData_MyHome_SideJob_Modify
{
	tagPlayerModifyOrderData_MyHome_SideJob_Modify():m_kSideJob(MSJ_NONE), m_kSellCost(0), m_kStreetNo(0), m_kHouseNo(0){}
	tagPlayerModifyOrderData_MyHome_SideJob_Modify(short const __streetno, int const __househo, eMyHomeSideJob const __sidejob,__int64 const __sellcost)
		:m_kStreetNo(__streetno), m_kHouseNo(__househo), m_kSideJob(__sidejob), m_kSellCost(__sellcost){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(eMyHomeSideJob,SideJob);
	CLASS_DECLARATION_S(__int64,SellCost);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_SideJob_Modify;

typedef struct tagPlayerModifyOrderData_MyHome_SideJob_Complete
{
	tagPlayerModifyOrderData_MyHome_SideJob_Complete():m_kSideJob(MSJ_NONE), m_kStreetNo(0), m_kHouseNo(0){}
	tagPlayerModifyOrderData_MyHome_SideJob_Complete(BM::GUID const __ownerguid,eMyHomeSideJob const __sidejob, short const __streetno, int const __househo)
		:m_kOwnerGuid(__ownerguid), m_kSideJob(__sidejob), m_kStreetNo(__streetno), m_kHouseNo(__househo){}
	CLASS_DECLARATION_S(BM::GUID,OwnerGuid);
	CLASS_DECLARATION_S(eMyHomeSideJob,SideJob);
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_SideJob_Complete;

typedef struct tagPlayerModifyOrderData_MyHome_Sync_EquipItemCount
{
	tagPlayerModifyOrderData_MyHome_Sync_EquipItemCount():m_kStreetNo(0), m_kHouseNo(0), m_kEquipCount(0){}
	tagPlayerModifyOrderData_MyHome_Sync_EquipItemCount(short const __streetno, int const __househo, int const __equipcount)
		:m_kStreetNo(__streetno), m_kHouseNo(__househo), m_kEquipCount(__equipcount){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(int,EquipCount);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_Sync_EquipItemCount;

typedef struct tagPlayerModifyOrderData_MyHome_Sync_VisitLogCount
{
	tagPlayerModifyOrderData_MyHome_Sync_VisitLogCount():m_kStreetNo(0), m_kHouseNo(0), m_kVisitLogCount(0), m_kSaveDB(false){}
	tagPlayerModifyOrderData_MyHome_Sync_VisitLogCount(short const __streetno, int const __househo, int const __logcount, bool const __savedb = false)
		:m_kStreetNo(__streetno), m_kHouseNo(__househo), m_kVisitLogCount(__logcount), m_kSaveDB(__savedb){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);
	CLASS_DECLARATION_S(int,VisitLogCount);
	CLASS_DECLARATION_S(bool,SaveDB);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_MyHome_Sync_VisitLogCount;

typedef struct tagPlayerModifyOrderData_Extend_CharacterNum
{
	tagPlayerModifyOrderData_Extend_CharacterNum(){}
	explicit tagPlayerModifyOrderData_Extend_CharacterNum(BYTE const bExtendNum):m_kExtendNum(bExtendNum){}
	CLASS_DECLARATION_S(BYTE,ExtendNum);
	DEFAULT_TBL_PACKET_FUNC();
}SMOD_Extend_CharacterNum;

typedef struct tagPlayerModifyOrderData_ActiveStatus
{
	tagPlayerModifyOrderData_ActiveStatus()
	{
		m_kStr = m_kInt = m_kDex = m_kCon = 0;
	}
	DEFAULT_TBL_PACKET_FUNC();

	CLASS_DECLARATION_S(short, Str);
	CLASS_DECLARATION_S(short, Int);
	CLASS_DECLARATION_S(short, Dex);
	CLASS_DECLARATION_S(short, Con);
} SPMO_ActiveStatus;

typedef enum eDateContentsType
{
	DCT_NONE				= 0,
	DCT_GUILD_LEAVE_DATE	= 1,
} EDateContentsType;

typedef struct tagPlayerModifyOrderData_DateContents
{
	tagPlayerModifyOrderData_DateContents()
		: m_kType(DCT_NONE), m_kDateTime()
	{
	}
	tagPlayerModifyOrderData_DateContents(EDateContentsType const eType, BM::DBTIMESTAMP_EX const& rkDateTime)
		: m_kType(eType), m_kDateTime(rkDateTime)
	{
	}
	tagPlayerModifyOrderData_DateContents(tagPlayerModifyOrderData_DateContents const& rhs)
		: m_kType(rhs.m_kType), m_kDateTime(rhs.m_kDateTime)
	{
	}

	CLASS_DECLARATION_S(EDateContentsType, Type);
	CLASS_DECLARATION_S(BM::DBTIMESTAMP_EX, DateTime);

	DEFAULT_TBL_PACKET_FUNC();
} SPMOD_DateContents;

typedef struct tagPlayerModifyOrderData_Gamble_Insert
{
	tagPlayerModifyOrderData_Gamble_Insert():m_kRouletteCount(0),m_kMixPoint(0){}
	tagPlayerModifyOrderData_Gamble_Insert(int const __roulettecount, int const __mixpoint = 0):m_kRouletteCount(__roulettecount),m_kMixPoint(__mixpoint){}
	CLASS_DECLARATION_S(int, RouletteCount);//대상 아이템
	CLASS_DECLARATION_S(int, MixPoint);//대상 아이템
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_Gamble_Insert;

typedef struct tagPlayerModifyOrderData_JobSkillExpertness
{
	tagPlayerModifyOrderData_JobSkillExpertness()
		: m_kSkillNo(0), m_kExpertness(0)
	{
	}
	tagPlayerModifyOrderData_JobSkillExpertness(int const iSkillNo, int const iExpertness)
		: m_kSkillNo(iSkillNo), m_kExpertness(iExpertness)
	{
	}
	CLASS_DECLARATION_S(int, SkillNo);
	CLASS_DECLARATION_S(int, Expertness);
	DEFAULT_TBL_PACKET_FUNC();
} SPMOD_JobSkillExpertness;
typedef struct tagPlayerModifyOrderData_JobSkillSaveIdx
{
	tagPlayerModifyOrderData_JobSkillSaveIdx()
		: m_kSaveIdx(0), m_kSet(false)
	{
	}
	tagPlayerModifyOrderData_JobSkillSaveIdx(int const iSaveIdx, bool const bSet)
		: m_kSaveIdx(iSaveIdx), m_kSet(bSet)
	{
	}
	CLASS_DECLARATION_S(int, SaveIdx);
	CLASS_DECLARATION_S(bool, Set);
	DEFAULT_TBL_PACKET_FUNC();
} SPMOD_JobSkillSaveIdx;
typedef struct tagPlayerModifyOrderData_JobSkillExhaustion
{
	tagPlayerModifyOrderData_JobSkillExhaustion()
		: m_kExhaustion(0)
	{
	}
	tagPlayerModifyOrderData_JobSkillExhaustion(int const iExhaustion)
		: m_kExhaustion(iExhaustion)
	{
	}
	CLASS_DECLARATION_S(int, Exhaustion);
	DEFAULT_TBL_PACKET_FUNC();
} SPMOD_JobSkillExhaustion;

////////////////////////////////////////////////////////////////////////////
//
typedef enum eOrderOwnerType : BYTE
{
	OOT_Player = 0,
	OOT_Guild = 1,
} EOrderOwnerType;

//
typedef struct tagModifyOrderOwner
{
	tagModifyOrderOwner()
		: kOwnerGuid(), eOwnerType(OOT_Player)
	{
	}
	explicit tagModifyOrderOwner(BM::GUID const& rkOwnerGuid, EOrderOwnerType const eType)
		: kOwnerGuid(rkOwnerGuid), eOwnerType(eType)
	{
	}
	explicit tagModifyOrderOwner(tagModifyOrderOwner const& rhs)
		: kOwnerGuid(rhs.kOwnerGuid), eOwnerType(rhs.eOwnerType)
	{
	}

	bool operator <(tagModifyOrderOwner const& rhs) const
	{
		if( kOwnerGuid < rhs.kOwnerGuid )
		{
			return true;
		}
		else
		{
			if( kOwnerGuid == rhs.kOwnerGuid
			&&	eOwnerType < rhs.eOwnerType )
			{
				return true;
			}
		}
		return false;
	}
	bool operator ==(tagModifyOrderOwner const& rhs) const
	{
		return kOwnerGuid == rhs.kOwnerGuid
			&& eOwnerType == rhs.eOwnerType;
	}

	size_t min_size()const
	{
		return 
			sizeof(kOwnerGuid)+
			sizeof(eOwnerType);//m_kPacket.Data()
	}
	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kOwnerGuid);
		kPacket.Push(eOwnerType);
	}
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kOwnerGuid);
		kPacket.Pop(eOwnerType);
	}

	BM::GUID kOwnerGuid;
	EOrderOwnerType eOwnerType;
} SModifyOrderOwner;

//
typedef struct tagPlayerModifyOrder
{//count down. modify
	tagPlayerModifyOrder()
		: m_kCause(0), m_kModifyOwner(), m_kPacket()
	{
	}
	template< typename T > tagPlayerModifyOrder(__int64 const eInCause/*EItemModifyEventType*/, BM::GUID const &kOwnerGuid, T const &t)
		: m_kCause(eInCause)
	{
		OwnerGuid(kOwnerGuid);
		Write(t);
	}
	tagPlayerModifyOrder(__int64 const eInCause/*EItemModifyEventType*/, BM::GUID const &kOwnerGuid)
		: m_kCause(eInCause), m_kPacket()
	{
		OwnerGuid(kOwnerGuid);
	}
	template< typename T > tagPlayerModifyOrder(__int64 const eInCause/*EItemModifyEventType*/, SModifyOrderOwner const &kOwner, T const &t)
		: m_kCause(eInCause), m_kModifyOwner(kOwner)
	{
		Write(t);
	}
	tagPlayerModifyOrder(__int64 const eInCause/*EItemModifyEventType*/, SModifyOrderOwner const &kOwner)
		: m_kCause(eInCause), m_kModifyOwner(kOwner), m_kPacket()
	{
	}

	tagPlayerModifyOrder(tagPlayerModifyOrder const& rhs)
		: m_kCause(rhs.m_kCause), m_kModifyOwner(rhs.m_kModifyOwner), m_kPacket(rhs.m_kPacket)
	{
		m_kPacket.PosAdjust();
	}

	tagPlayerModifyOrder const operator = (tagPlayerModifyOrder const& rhs)
	{
		m_kCause = rhs.m_kCause;
		m_kModifyOwner = rhs.m_kModifyOwner;
		m_kPacket.Reset();
		m_kPacket = rhs.m_kPacket;
		m_kPacket.PosAdjust();
		return (*this);
	}

	template< typename T >
	bool Write(T const &t)
	{//한번만 쓰고.
		if(!m_kPacket.Size())
		{
			t.WriteToPacket(m_kPacket);
//			m_kPacket.Push(t);
			return true;
		}

		__asm int 3;//들어오면 안됨
		return false;
	}

	template< typename T >
	bool Read(T &t)const
	{//읽기는 여러번.
		t.ReadFromPacket(m_kPacket);
//		if(m_kPacket.Pop(t))
		{
			m_kPacket.PosAdjust();//또 꺼낼 수 있게.
			return true;
		}

		__asm int 3;//들어오면 안됨
		return false;
	}

	size_t min_size()const
	{
		return 
			sizeof(m_kCause)+
			m_kModifyOwner.min_size()+
			sizeof(size_t);//m_kPacket.Data()
	}
	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(m_kCause);
		m_kModifyOwner.WriteToPacket(kPacket);
		kPacket.Push(m_kPacket.Data());
	}
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_kCause);
		m_kModifyOwner.ReadFromPacket(kPacket);
		kPacket.Pop(m_kPacket.Data());
		m_kPacket.PosAdjust();
	}

	BM::GUID const& OwnerGuid() const
	{
		return m_kModifyOwner.kOwnerGuid;
	}
	void OwnerGuid(BM::GUID const& rkOwnerGuid, EOrderOwnerType const eType = OOT_Player)
	{
		m_kModifyOwner.kOwnerGuid = rkOwnerGuid;
		m_kModifyOwner.eOwnerType = eType;
	}

	CLASS_DECLARATION_S(__int64, Cause);					//EItemModifyEventType ???(S로 시작하지만 enum임 -_-;)
	CLASS_DECLARATION_S(SModifyOrderOwner, ModifyOwner);	// 소유자
	mutable BM::Stream m_kPacket;							//데이터. Read 시에 원본 유지 되므로. mutable 예외를 적용
}SPMO;

//
typedef struct tagCONT_PLAYER_MODIFY_ORDER : public std::list< SPMO >
{
	void WriteToPacket(BM::Stream &kPacket)const
	{
		PU::TWriteArray_M(kPacket, *this);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		PU::TLoadArray_M(kPacket, *this);
	}
}CONT_PLAYER_MODIFY_ORDER;
// Item Type definition
typedef enum
{
	ITEM_TYPE_UNKNOWN	= 0x0001,
	ITEM_TYPE_EQUIP		= 0x0002,	// 장비 아이템
	ITEM_TYPE_CONSUME	= 0x0004,	// 소비 아이템
	ITEM_TYPE_ETC		= 0x0008,	// 기타 아이템
	ITEM_TYPE_QUEST		= 0x0010,	// 퀘스트 아이템
	ITEM_TYPE_AIDS		= 0x0020,	// 캐쉬 아이템
	ITEM_TYPE_ENCHANT	= 0x0040,	// 인첸트 아이템
	ITEM_TYPE_MATIRIAL	= 0x0080,	// 조합 아이템
	ITEM_TYPE_COOK		= 0x0100,	// 요리 아이템
	ITEM_TYPE_PET		= 0x0200,	// 펫 아이템
	ITEM_TYPE_MYHOME_IN	= 0x0400,	// 마이홈 아이템(내부)
	ITEM_TYPE_EVENT		= 0x0800,	// 이벤트 아이템
	ITEM_TYPE_NATURE	= 0x1000,	// 고유 아이템
	ITEM_TYPE_MYHOME_OUT= 0x2000,	// 마이홈 아이템(외부)
	ITEM_TYPE_USE_UI	= 0x4000,	// 특정 UI를 통해서만 사용가능한 아이템(퀵슬롯 등록 불가, 인벤토리에서 사용 불가).
}EItemType;

typedef enum eItemCantModifyEventType//AT_ITEM_CANT_EVENT 가 가질 수 있는 값
{
	ICMET_None					= 0,
	ICMET_Cant_ShopSell			= 0x00000001,		//상점에 팔 수 없음
	ICMET_Cant_PlayerTrade		= 0x00000002,		//플레이어 끼리 거래 안됨
	ICMET_Cant_MoveItemBox		= 0x00000004,		//창고에 넣을 수 없음
	ICMET_Cant_SendMail			= 0x00000008,		//메일로 쏠 수 없음
	ICMET_Cant_DropItem			= 0x00000010,		//버리기(=파괴) 할 수 없음
	ICMET_Cant_Auction			= 0x00000020,		//경매장에 올릴 수 없음
	ICMET_Cant_UsePVP			= 0x00000040,		//PVP 에서 사용 할 수 없음.
	ICMET_Cant_Enchant			= 0x00000080,		//인첸트 금지
	ICMET_Cant_SoulCraft		= 0x00000100,		//소울 크레프트 금지
	ICMET_Cant_GenSocket		= 0x00000200,		//소캣 생성 금지
	ICMET_Cant_SelfUse			= 0x00000400,		//유저가 마우스 오른쪽 클릭으로 사용 못한다.(시스템에서 자동 사용 용도 서버에서는 딱히 따로 구분 할 필요가 있을지 고민중...일단은 클라이언트 전용)
	ICMET_Cant_Seal				= 0x00000800,		//역 봉인 불가
	ICMET_Cant_Rollback			= 0x00001000,		//소울 크래프트 행운권 사용 금지
	ICMET_Cant_Repair			= 0x00002000,		//수리 불가
	ICMET_Cant_UseShareRental	= 0x00004000,		// 계정 금고 사용 불가
} EItemCantModifyEventType;

// Equip position (bit field checking)
typedef enum 
{
	EQUIP_POS_NONE			= 0,
	EQUIP_POS_HAIR_COLOR	= 0,
	EQUIP_POS_HAIR			= 1,	// 머리카락	2
	EQUIP_POS_FACE			= 2,	// 얼굴		4
	EQUIP_POS_SHOULDER		= 3,	// 어깨		8
	EQUIP_POS_CLOAK			= 4,	// 망토		16
	EQUIP_POS_GLASS			= 5,	// 안경		32
	EQUIP_POS_WEAPON		= 6,	// 무기		64
	EQUIP_POS_SHEILD		= 7,	// 방패		128
	EQUIP_POS_NECKLACE		= 8,	// 목걸이	256
	EQUIP_POS_EARRING		= 9,	// 귀걸이	512
	//EQUIP_POS_EARRING_L		= 9,
	//EQUIP_POS_EARRING_R		= 29,
	EQUIP_POS_RING			= 10,	// 반지		1024
	EQUIP_POS_RING_L		= 10,
	EQUIP_POS_RING_R		= 30,
	EQUIP_POS_BELT			= 11,	// 벨트		2048
	EQUIP_POS_ATTSTONE		= 12,	// 속성석	4096
	EQUIP_POS_MEDAL			= 13,	// 훈장		8192
	EQUIP_POS_HELMET		= 20,	// 투구		1048576	
	EQUIP_POS_SHIRTS		= 21,	// 상의		2097152
	EQUIP_POS_PANTS			= 22,	// 하의		4194304
	EQUIP_POS_BOOTS			= 23,	// 부츠		8388608
	EQUIP_POS_GLOVE			= 24,	// 장갑		16777216
	EQUIP_POS_ARM			= 25,	// 팔(지금은 SHEILD와 같은 위치)
	EQUIP_POS_PET			= 26,
	EQUIP_POS_KICKBALL		= 27,	//킥볼( == 풋브레이커)
	EQUIP_POS_MAX			= 31,

//	EQUIP_POS_TWO_HAND		= 32, //예외적 처리.(양손검


//==============================================================================================
//	마이홈 외형 정보용 기본 아이템 장착 위치 정보 한번 변경되면 되돌릴수 없음
//==============================================================================================

	EQUIP_POS_HOME_COLOR	= EQUIP_POS_HAIR_COLOR, // 홈 색상(텍스쳐?)
	EQUIP_POS_HOME_STYLE	= EQUIP_POS_HAIR,		// 홈 외형 
	EQUIP_POS_HOME_FENCE	= EQUIP_POS_FACE,		// 홈 울타리
	EQUIP_POS_HOME_GARDEN	= EQUIP_POS_SHIRTS,		// 홈 마당
	EQUIP_POS_ROOM_WALL		= EQUIP_POS_PANTS,		// 홈 내부 벽
	EQUIP_POS_ROOM_FLOOR	= EQUIP_POS_BOOTS,		// 홈 내부 바닥

//==============================================================================================
//	펫 장착용 아이템 위치는 기존 번호를 참조.
//==============================================================================================
	EQUIP_POS_PET_ACC		= EQUIP_POS_HELMET,		//1048576	//악세서리. 노스키닝으로 붙어야 하기때문에 투구와 같은 번호를 사용했음
	EQUIP_POS_PET_HEAD		= EQUIP_POS_SHIRTS,		//2097152	//머리
	EQUIP_POS_PET_BODY		= EQUIP_POS_PANTS,		//4194304	//몸통
	EQUIP_POS_PET_HELEM		= EQUIP_POS_BOOTS,		//8388608	//모자
	EQUIP_POS_PET_SHIRTS	= EQUIP_POS_GLOVE,		//16777216//상의
	EQUIP_POS_PET_TRAIN		= EQUIP_POS_ARM,		// 훈련아이템

	EQUIP_POS_PET_WEAPON	= EQUIP_POS_WEAPON,		//무기
	EQUIP_POS_PET_RING		= EQUIP_POS_RING,		//반지
	EQUIP_POS_PET_NECKLACE	= EQUIP_POS_NECKLACE,	//목걸이
	EQUIP_POS_PET_EARRING	= EQUIP_POS_EARRING,	//귀걸이

// 예약....

	EQUIP_POS_HOME_RESERVE5	= EQUIP_POS_GLOVE,		// 홈 예약5
}EEquipPos;

typedef enum 
{
	EQUIP_LIMIT_NONE = 0,
	EQUIP_LIMIT_HAIR_COLOR		=(0x00000001 << EQUIP_POS_HAIR_COLOR),
	EQUIP_LIMIT_HAIR			=(0x00000001 << EQUIP_POS_HAIR),
	EQUIP_LIMIT_FACE			=(0x00000001 << EQUIP_POS_FACE),
	EQUIP_LIMIT_SHOULDER		=(0x00000001 << EQUIP_POS_SHOULDER),
	EQUIP_LIMIT_CLOAK			=(0x00000001 << EQUIP_POS_CLOAK),
	EQUIP_LIMIT_GLASS			=(0x00000001 << EQUIP_POS_GLASS),
	EQUIP_LIMIT_WEAPON			=(0x00000001 << EQUIP_POS_WEAPON),
	EQUIP_LIMIT_SHEILD			=(0x00000001 << EQUIP_POS_SHEILD),
	EQUIP_LIMIT_NECKLACE		=(0x00000001 << EQUIP_POS_NECKLACE),
	EQUIP_LIMIT_EARRING			=(0x00000001 << EQUIP_POS_EARRING),
	EQUIP_LIMIT_RING			=(0x00000001 << EQUIP_POS_RING),
	EQUIP_LIMIT_BELT			=(0x00000001 << EQUIP_POS_BELT),
	EQUIP_LIMIT_ATTSTONE		=(0x00000001 << EQUIP_POS_ATTSTONE),
	EQUIP_LIMIT_MEDAL			=(0x00000001 << EQUIP_POS_MEDAL),
	EQUIP_LIMIT_HELMET			=(0x00000001 << EQUIP_POS_HELMET),
	EQUIP_LIMIT_SHIRTS			=(0x00000001 << EQUIP_POS_SHIRTS),
	EQUIP_LIMIT_PANTS			=(0x00000001 << EQUIP_POS_PANTS),
	EQUIP_LIMIT_BOOTS			=(0x00000001 << EQUIP_POS_BOOTS),
	EQUIP_LIMIT_GLOVE			=(0x00000001 << EQUIP_POS_GLOVE),
	EQUIP_LIMIT_ARM				=(0x00000001 << EQUIP_POS_ARM),
	EQUIP_LIMIT_KICKBALL		=(0x00000001 << EQUIP_POS_KICKBALL),
	EQUIP_LIMIT_PET				=(0x00000001 << EQUIP_POS_PET),

	EQUIP_LIMIT_PET_ACC			= EQUIP_LIMIT_HELMET,	// 투구		1048576	//악세서리. 노스키닝으로 붙어야 하기때문에 투구와 같은 번호를 사용했음
	EQUIP_LIMIT_PET_HEAD		= EQUIP_LIMIT_SHIRTS,	//2097152	//머리
	EQUIP_LIMIT_PET_BODY		= EQUIP_LIMIT_PANTS,	//4194304	//몸통
	EQUIP_LIMIT_PET_HELEM		= EQUIP_LIMIT_BOOTS,	//8388608	//모자
	EQUIP_LIMIT_PET_SHIRTS		= EQUIP_LIMIT_GLOVE,	//16777216//상의
	EQUIP_LIMIT_PET_TRAIN		= EQUIP_LIMIT_ARM,		// 훈련아이템

	EQUIP_LIMIT_PET_WEAPON		= EQUIP_LIMIT_WEAPON,	//무기
	EQUIP_LIMIT_PET_RING		= EQUIP_LIMIT_RING,		//반지
	EQUIP_LIMIT_PET_NECKLACE	= EQUIP_LIMIT_NECKLACE,	//목걸이
	EQUIP_LIMIT_PET_EARRING		= EQUIP_LIMIT_EARRING,	//귀걸이

	EQUIP_INCHANT_POSSIBLE_BUNDLE = EQUIP_LIMIT_SHOULDER | EQUIP_LIMIT_CLOAK | EQUIP_LIMIT_WEAPON | EQUIP_LIMIT_SHEILD 
									| EQUIP_LIMIT_BELT | EQUIP_LIMIT_HELMET | EQUIP_LIMIT_SHIRTS | EQUIP_LIMIT_PANTS | EQUIP_LIMIT_BOOTS
									| EQUIP_LIMIT_GLOVE | EQUIP_LIMIT_GLASS | EQUIP_LIMIT_MEDAL | EQUIP_LIMIT_KICKBALL
									/*| EQUIP_LIMIT_NECKLACE| EQUIP_LIMIT_EARRING | EQUIP_LIMIT_RING*/ , // 악세사리는 인챈트 안됨

	EQUIP_WEAPON_TYPE			= EQUIP_LIMIT_WEAPON,
	EQUIP_ARMOR_TYPE			= EQUIP_LIMIT_SHOULDER | EQUIP_LIMIT_CLOAK | EQUIP_LIMIT_SHEILD | EQUIP_LIMIT_BELT | EQUIP_LIMIT_HELMET | EQUIP_LIMIT_SHIRTS | EQUIP_LIMIT_PANTS | EQUIP_LIMIT_BOOTS | EQUIP_LIMIT_GLOVE | EQUIP_LIMIT_ARM | EQUIP_LIMIT_KICKBALL,
	EQUIP_ACC_TYPE				= EQUIP_LIMIT_NECKLACE | EQUIP_LIMIT_EARRING | EQUIP_LIMIT_RING | EQUIP_LIMIT_GLASS,

}eEquipLimit;

typedef enum
{
	EWEAPON_NONE		= 0x0000,
	EWEAPON_SWORD		= 0x0001,	// 한손검
	EWEAPON_BIHANDSWORD	= 0x0002,	// 양손검
	EWEAPON_STAFF		= 0x0004,
	EWEAPON_SPEAR		= 0x0008,
	EWEAPON_BOW			= 0x0010,
	EWEAPON_CROSSBOW	= 0x0020,
	EWEAPON_CLAW		= 0x0040,
	EWEAPON_KATTAR		= 0x0080,
	EWEAPON_SPECIAL		= 0x0100,

	EWEAPON_JOB_TOOL	= 0x0200,//도구의 타입은 AT_JOBSKILL_TOOL_TYPE 어빌에 셋팅됨, 타입은 아래 EToolType 에 정의

	EWEAPON_GUN_STAFF	= 0x0400,	//소환사 무기
	EWEAPON_GLOVE		= 0x0800,	//쌍둥이 무기
} EWeaponType;

typedef enum
{
	EEQUIP_NONE			= 0,
	EEQUIP_WEAPON		= 1,	//무기
	EEQUIP_ARMOR		= 2,	//방어구
	EEQUIP_ACC			= 3,	//악세서리(Accessory)
} EEquipType;

typedef enum
{
	ETOOL_NONE					= 0,
	ETOOL_AXE					= 1,	// 도끼
	ETOOL_PICKAX				= 2,	// 곡괭이
	ETOOL_HOE					= 3,	// 호미
	ETOOL_FISHINGROD			= 4,	// 낚시대

	ETOOL_ALL					= 10,	//만능 도구

	ETOOL_CHAINSAW				= 11,	// 전기톱
	ETOOL_DRILL					= 12,	// 드릴
	ETOOL_GRASSELIMINATOR		= 13,	// 예초기
	ETOOL_NET					= 14,	// 그물

	ETOOL_ALL_SUB				= 20,	//만능 보조 도구
} EToolType;

// Making Type definition	(제조 타입)
typedef enum
{
	EMAKING_TYPE_NONE				= 0,
	EMAKING_TYPE_COOKING			= 0x0001, // 요리 아이템
	EMAKING_TYPE_LOTTERY			= 0x0002, // 뽑기 아이템
	EMAKING_TYPE_COIN				= 0x0004, // 100마리 잡으면 나오는 코인 자판기용 아이템
	EMAKING_TYPE_ETC				= 0x0008, // 기타 아이템
	EMAKING_TYPE_UNSEALING			= 0x0010, // 봉인해제 주문서 교환
	EMAKING_TYPE_SOULSTONE			= 0x0020, // 영혼석으로 각종 아이템 교환(히든/카오스)
	EMAKING_TYPE_MAZE_ITEM			= 0x0040, // 대미궁 조합 아이템
	EMAKING_TYPE_MONSTER_CARD		= 0x0080,
	EMAKING_TYPE_COMPOSITE			= 0x0100,
	EMAKING_TYPE_MONSTER_CARD_TYPE2 = 0x0200,
	EMAKING_TYPE_NEW_COOKING		= 0x0400 | EMAKING_TYPE_COOKING, // 요리 아이템(TB_DefCooking 활용)
}EMakingType;

typedef enum eMissionQuestCardGrade//AT_ITEM_QUEST_CARD_GRADE
{
	MQCG_NOMAL = 0,
	MQCG_MAGIC = 1,
	MQCG_RARE,
	MQCG_UNIQUE,
}EMissionQuestCardGrade;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 /*
	PgItemWrapper 와 PgItemWrapper이 같은 의미로 쓰여야 한다.
	박스 아이템은 어떻게 되나??->맵에서 박스를 뿌리고. 박스에서 아이템 얻을때 가져가도록 해야겠다.
	맵서버에서 잘못하면 박스아이템으로 인한 복사(?) 같이 보이는 현상이 있을 수도 있겠다.
	-> 박스에 들어갈 아이템을 Create 명령으로 만들어놓고 박스를 보여주는 것도 방법.
*/
class PgItemWrapper
	:	public PgBase_Item
{
public:
	PgItemWrapper();
	PgItemWrapper( PgItemWrapper const &rhs );
	explicit PgItemWrapper(PgBase_Item const &kInItem, SItemPos const &kinPos);
	virtual ~PgItemWrapper();

	PgItemWrapper& operator = ( PgItemWrapper const & rhs );

	void Clear();
	void SetItem(PgBase_Item const &rkItem);
	void SetItemPos(SItemPos const &rkItemPos);

//	CLASS_DECLARATION_S(PgBase_Item, Item);
	CLASS_DECLARATION_S(SItemPos, Pos);

	CLASS_DECLARATION_S(bool, IsCreated);//생성되었느냐(DB저장이 안되었다) -> 최우선.
	CLASS_DECLARATION_S(bool, IsModify);//정보가 변경 되었는가
	CLASS_DECLARATION_S(bool, IsRemove);//지워졌는가 -> 지워진건 다른 액션을 절대 하지 않도록.

	size_t min_size()const
	{
		return 
			PgBase_Item::min_size()+
			sizeof(m_kPos)+
			sizeof(m_kIsCreated)+//생성되었느냐(DB저장이 안되었다) -> 최우선.
			sizeof(m_kIsModify)+//정보가 변경 되었는가
			sizeof(m_kIsRemove);//지워졌는가 -> 지워진건 다른 액션을 절대 하지 않도록.
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PgBase_Item::WriteToPacket(kPacket);
		kPacket.Push(m_kPos);
		kPacket.Push(m_kIsCreated);//생성되었느냐(DB저장이 안되었다) -> 최우선.
		kPacket.Push(m_kIsModify);//정보가 변경 되었는가
		kPacket.Push(m_kIsRemove);//지워졌는가 -> 지워진건 다른 액션을 절대 하지 않도록.
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		PgBase_Item::ReadFromPacket(kPacket);
		kPacket.Pop(m_kPos);
		kPacket.Pop(m_kIsCreated);//생성되었느냐(DB저장이 안되었다) -> 최우선.
		kPacket.Pop(m_kIsModify);//정보가 변경 되었는가
		kPacket.Pop(m_kIsRemove);//지워졌는가 -> 지워진건 다른 액션을 절대 하지 않도록.
	}
};

typedef enum 
{
	eOwnerTypeNone = 0,
	eOwnerTypeUnit = 1,
	eOwnerTypeMap = 2,
}EOwnerType;

typedef struct tagItemDropDesc
{
	tagItemDropDesc()
	{
		iMapNo = 0;
		iItemNo = 0;
	}

	int iMapNo;
	int iItemNo;

	POINT3 kpt3Pos;
//	BM::GUID kOwnerGuid;
	BM::GUID kHighPriorityUnitGuid;
}PgItemDropDesc;

typedef enum ePlusItemUpgradeResult // 로그DB에서도 사용함으로 숫자 순서 바꾸지 말것
{
	PIUR_NONE	= 0,
	PIUR_SUCCESS	= 1,
	PIUR_NOT_ENOUGH_RES	= 2,//재료 부족
	PIUR_CANT_DELETE_RES	= 3,//재료 부족
	PIUR_NORMAL_FAILED	= 4,//걍 실패(초급)
	PIUR_PANALTY_FAILED	= 5,//등급이 깎이는 실패(중급)
	PIUR_NOT_ENOUGH_MONEY	= 6,//재료 부족
	PIUR_NOT_FOUND_ITEM = 7,// 가공 아이템 찾을수 없음
	PIUR_OVER_LEVELLIMIT = 8,// 인첸트 제한 레벨에 도달했다?
	PIUR_CAN_NOT_ENCHANT = 9,// 인첸트 할수 없는 아이템이다.
	PIUR_DESTROY_FAILED = 10,// 아이템 파괴 실패(9급부터)
}EPlusItemUpgradeResult;

typedef struct tagPlusItemUpgradeResult
{
	tagPlusItemUpgradeResult()
	{
		eResult = PIUR_NONE;
	}
	EPlusItemUpgradeResult eResult;//S_OK가 성공
}PLUS_ITEM_UPGRADE_RESULT;

typedef enum eDBItemStateChangeType//이건 플래그 하면안됨.
{
	DISCT_NONE = 0,
//	DISCT_CMP_BASE				= 0x000F,
	DISCT_CREATE				= 1,
	DISCT_MODIFY				= 2,
	DISCT_REMOVE				= 3, //내구도가 0이면 Remove로 판단 합시다. -> 왜 이랬을까?
	DISCT_MODIFY_MONEY			= 4,
	
	DISCT_REMOVE_IMAGE			= 5, //지우기인데. DB업데이트는 안함.
	DISCT_MODIFY_CP				= 6,
	DISCT_REMOVE_SYSTEM			= 7, //지우기 DB 저장 없음

	DISCT_MODIFY_VALUE			= 8,
	DISCT_MODIFY_SKILL			= 9,

	DISCT_MODIFY_QUEST_ING		= 10,
	DISCT_MODIFY_QUEST_PARAM	= 11,
	DISCT_MODIFY_QUEST_END		= 12,
	DISCT_MODIFY_SET_GUID		= 13,
	DISCT_MODIFY_ADD_RANK_POINT	= 14,
	DISCT_MODIFY_SET_PVP_RECORD	= 15,
	DISCT_MODIFY_WORLD_MAP		= 16,
	DISCT_MODIFY_CLEAR_ING_QUEST		= 17,

	DISCT_CREATE_MAIL			= 18,
	DISCT_ADD_CASH				= 19,
	DISCT_REMOVE_DEALING		= 20,
	DISCT_REMOVE_MARKET			= 21,

	DISCT_COMPLETE_ACHIEVEMENT	= 22,	// 업적 달성
	DISCT_ACHIEVEMENT2INV		= 23,	// 훈장을 아이템으로 변환
	DISCT_SET_ACHIEVEMENT		= 24,
	DISCT_SET_RENTALSAFETIME	= 25,

	DISCT_OPEN_MARKET			= 26,
	DISCT_ADD_ARTICLE			= 27,
	DISCT_REMOVE_ARTICLE		= 28,
	DISCT_BUY_ARTICLE			= 29,
	DISCT_ADD_DEALING			= 30,
	DISCT_MODIFY_MARKET_INFO	= 32,
	DISCT_MODIFY_MARKET_STATE	= 33,

	DISCT_INV2ACHIEVEMENT		= 34,	// 아이템 -> 훈장 변환

	DISCT_ADD_UNBIND_DATE		= 35,
	DISCT_DEL_UNBIND_DATE		= 36,
	DISCT_SET_DEFAULT_ITEM		= 37,

	DISCT_INVENTORY_EXTEND		= 38,

	DISCT_MODIFY_REFRESHDATE	= 39,
	DISCT_MODIFY_RECOMMENDPOINT = 40,

	DISCT_CREATE_CHARACTERCARD	= 41,
	DISCT_MODIFY_CHARACTERCARD	= 42,
	DISCT_MODIFY_POPULARPOINT	= 43,
	DISCT_MODIFY_CARDCOMMENT	= 44,
	DISCT_MODIFY_CARDSTATE		= 45,

	DISCT_CREATE_PORTAL			= 46,
	DISCT_DELETE_PORTAL			= 47,

	DISCT_ADD_EMOTION			= 48,

	DISCT_HATCH_PET				= 49,
	DISCT_REMOVE_PET			= 50,
	DISCT_RENAME_PET			= 51,
	DISCT_SETABIL_PET			= 52,

	DISCT_MODIFY_MISSION_EVENT	= 53,
	
	DISCT_MYHOME_MODIFY			= 54,

	DISCT_EXTEND_MAX_IDX		= 55,

	DISCT_CREATE_MAIL_BYGUID	= 56,

    DISCT_MODIFY_SWEETHEART		= 57,

	DISCT_MYHOME_INVITATION_CLEAR = 58,

	DISCT_SET_HOME_DEFAULT_ITEM	= 59,
	DISCT_SET_HOME_OWNER_INFO	= 60,

	DISCT_SET_HOME_VISITFLAG	= 61,
	DISCT_SET_HOME_STATE		= 62,
	DISCT_SET_HOME_ADDR			= 63,

	DISCT_EMPORIAFUNC_UPDATE	= 64,

	DISCT_CREATE_PET			= 65,

	DISCT_EXTEND_CHARACTER_NUM	= 66,

	DISCT_REMOVE_ITEM_EXTEND_DATA	= 67,

	DISCT_TRANSTOWER_SAVE_RECENT	= 68,

	DISCT_MODIFY_ACHIEVEMENT_RANK = 69,	// 업적 랭킹 정보 갱신

	DISCT_MODIFY_QUEST_EXT			= 70,
	DISCT_MODIFY_SET_DATE_CONTENTS	= 71,

	DISCT_COMPLETE_HIDDEN_OPEN		= 72,
	DISCT_SET_HIDDEN_OPEN			= 73,
	
	DISCT_MODIFY_VALUEEX			= 74,// 휴식 경험치 적용된 어빌 변경 저장

	DISCT_SIDEJOB_INSERT			= 75,
	DISCT_SIDEJOB_REMOVE			= 76,
	DISCT_SIDEJOB_MODIFY			= 77,
	DISCT_SIDEJOB_EQUIPCOUNT		= 78,

	DISCT_GAMBLE_INSERT				= 79,
	DISCT_GAMBLE_MODIFY				= 80,
	DISCT_GAMBLE_REMOVE				= 81,
	DISCT_ADD_ABIL					= 82,

	DISCT_ADD_HOME					= 83,
	DISCT_SET_MYHOME_VISITLOGCOUNT	= 84,
	DISCT_ADD_MYHOME_VISITLOGCOUNT	= 85,

	DISCT_UPDATE_QUEST_CLEAR_COUNT	= 86,
	DISCT_INIT_QUEST_CLEAR_COUNT	= 87,

	DISCT_GAMBLE_MIXUP_INSERT		= 90,
	DISCT_GAMBLE_MIXUP_MODIFY		= 91,
	DISCT_GAMBLE_MIXUP_REMOVE		= 92,

	DISCT_ACHIEVEMENT_TIMELIMIT_MODIFY = 93,
	DISCT_ACHIEVEMENT_TIMELIMIT_DELETE = 94,

	DISCT_MODIFY_SKILLEXTEND		= 95,

	DISCT_MYHOME_REMOVE				= 96,

	DISCT_MODIFY_STRATEGYSKILL		= 97,
	DISCT_MODIFY_GM_INIT_SKILL		= 98,
	DISCT_MODIFY_GUILD_MONEY		= 99,
	DISCT_GUILD_INV_EXTEND			= 100,

	DISCT_JOBSKILL_SET_SKILL_EXPERTNESS = 101,
	DISCT_JOBSKILL_DEL_SKILL_EXPERTNESS = 102,
	DISCT_JOBSKILL_SAVE_EXHAUSTION		= 104,

	DISCT_DELETE_SKILL          		= 105,

	DISCT_PREMIUM_SERVICE_INSERT		= 106,
	DISCT_PREMIUM_SERVICE_MODIFY		= 107,
	DISCT_PREMIUM_ARTICLE_MODIFY		= 108,
	DISCT_PREMIUM_SERVICE_REMOVE		= 109,

	DISCT_DAILY_REWARD_UPDATE			= 110,
}EDBItemStateChangeType;

// 한개의 Item Action (소모,이동,거래 등)에 대한 정의 값
// WebGmTool 로그확인 툴과 연계되어 있기 때문에 중간 삽입금지..
// 새로운 값을 추가한 경우 doc/programming/log포맷/ItemCause 정리표.xlsx에 수정하고
// ITS담당자에게 꼭 알려줘야함.
typedef enum eItemModifyParentEventType
{
	IMEPT_NONE = 0,
	//IMEPT_QUEST_REWARD,		//퀘스트 보상으로
	IMEPT_QUEST_DIALOG,
	IMEPT_QUEST_DIALOG_Accept,		//퀘스트 대화중에
	IMEPT_QUEST_DIALOG_End,
	IMEPT_QUEST_DROP,		//퀘스트 포기로
	IMEPT_QUEST,			//퀘스트 상태값 변경
	IMEPT_EVENT_TW_EFFECTQUEST,	//
	IMEPT_QUEST_SHARE,		//퀘스트 공유
	IMEPT_QUEST_REMOTE_COMPLETE, // 원격 완료
	IMEPT_QUEST_BUILD_RANDOM,
	IMEPT_QUEST_BUILD_RANDOM_BY_ITEM,
	IMEPT_QUEST_BUILD_TACTICS_RANDOM_BY_ITEM,

	IMEPT_COUPLE,			//커플 생성/해제 시
	IMEPT_GUILD,			//길드 생성/가입/탈퇴/추방 시
	IMEPT_PVP,
	IMEPT_EVENTQUEST,		//이벤트 퀘스트 아이템 회수 해갈 때

	IMEPT_GODCMD,			//
	IMEPT_DEATHPENALTY,
	IMEPT_MISSIONBONUS,
	IMEPT_LEARNSKILL,		// PgAction_LearnSkill

	IMEPT_BATTLESQUARE_REWARD,	// 배틀 스퀘어 보상

	MIE_AddItem,			//습득 (Create)
	MIE_UseItem,			//사용
	MIE_Move,				//이동 (장착/해제/자리바꿈)
	MIE_Modify,				//수정 (수량 수정/파기)
	//클라이언트
	MIE_Destroy,			//파기
	MIE_Equip,				//장착
	MIE_UnEquip,			//해제

	//돈
	MCE_Loot,				//몬스터로 부터 습득
	MCE_BuyItem,			//아이템 구매
	MCE_SellItem,			//아이템 판매로 증가
	//MCE_Reward,				//퀘스트 보상
	MCE_ShareParty,			//몬스터+파티 나누기
	MCE_Skill,				// 스킬로 보너스 골드를 얻는 경우
	MCE_CreateGuild,		//길드 생성 시
	MCE_GodCmd,
	MCE_Script,				//(사용안함)
	MCE_FailCreateGuild,	//길드 생성 실패시
	MCE_Fran,				// 푸랜 경험치 교환
	MCE_EmporiaContribute,	//엠포리아 기부함
	MCE_MarryMoney,			//결혼 기부
	MCE_EffectControl,		// PgAction_EffectControl
	MCE_SpendMoney,			// PgAction_SpendMoney
	MCE_LuckyStar_CostMoney,	// LuckyStar 이벤트 참여 비용

	MCE_EXP,

	//아이템
	CIE_Loot,				//땅에서 줍기
	CIE_BuyItem,			//아이템 구매
	CIE_Make,				//제조
	CIE_Party,				//파티 옵션에 따른 지급(남이 먹었는데 그놈이 습득)	
	CIE_PVP,				//PvP보상
	//CIE_Reward,			//퀘스트 보상
	CIE_QuestItem,			//퀘스트 진행 아이템
	CIE_ShineStone,			//샤인스톤
	CIE_GodCmd,				//GM명령어 등에 의해서
	CIE_Event,				//Event 행사 등
	CIE_EnchantLvUp,		//인챈트
	CIE_SoulCraft,			//소울크래프트(영력)
	CIE_BasicOptionAmp,		//옵션능력치증가
	CIE_GateWayUnLock,		//맵이동 하면서 소진
	CIE_TimeOut,			//사용 시간 만료
	CIE_KillCount,			//몬스터 킬 카운트 보상
	CIE_OpenChest,			//상자 열어서 보상
	CIE_AlramMission,		//돌발미션 보상

	CIE_Mission,			//인던(미션맵) 보상
	CIE_Mission1,			//인던(미션맵) 보상-레벨
	CIE_Mission2,
	CIE_Mission3,
	CIE_Mission4,
	CIE_Mission_GadaCoin,	//가다코인을 이용한 아이템
	CIE_Mission_Event,		//이벤트 맵 클리어 체크
	CIE_Mission_Rank,
	CIE_Mission_InfallibleSelection,//디펜스 모드 방향 고르기 아이템
	CIE_Mission_DefenceTimePlus,//디펜스 모드 방어시간 증가
	CIE_Mission_DefencePotion,//디펜스 모드 수호석 회복
	CIE_Mission_DefenceWin,	// 디팬스 승리보상
	CIE_Mission_UseItem,	// 미션 전용 아이템
	CIE_Defence7_Relay_Stage,	//디펜스7 이어하기 아이템
	CIE_Defence7_Point_Copy,	//디펜스7 포인트 복제기.

	CIE_HiddenReword,
	CIE_HiddenRewordItem,

	CIE_Mouse_Event,		//드래그앤 드롭

	CIE_Dec_Dur_by_Defence,	//방어하다 감소
	CIE_Dec_Dur_by_Attack,	//공격하다 감소
	
	CIE_Repair,				//수리
	
	CIE_Dump,				//버림
	CIE_Disassemble_Result,	//버리고 나서 얻는거(분해)
	CIE_Exchange,			//교환
	CIE_Script_Delete,		//스크립트에서 지움
	CIE_Divide,				//Divide
	CIE_CallGodCmd,			// called by God Command
	//CIE_MissionQuestCard,	//(사용안함)
	CIE_GuildLevelUp,		//길드 레벨업
	CIE_GuildInventoryCreate, //길드금고 생성
	CIE_GuildLearnSkill,	//길드 스킬
	CIE_CoupleWarp,			//커플 워프
	CIE_CoupleLearnSkill,	//커플 스킬

	CIE_Post_Mail_Send,		//메일 보내기
	CIE_Post_Mail_Modify,	//메일 상태 변경
	CIE_Post_Mail_Item_Recv,//메일 아이템 수령

	CIE_UM_Market_Open,		//마켓 오픈 요청
	CIE_UM_Article_Reg,		//경매 물품 등록
	CIE_UM_Article_Dereg,	//등록된 경매 취소
	CIE_UM_Article_Query,	//경매 물품 조회
	CIE_UM_Article_Buy,		//경매 물품 구입
	CIE_UM_Dealing_Query,	//경매 내역 조회
	CIE_UM_Dealing_Read,	//경매 내역 읽기(실제 여기서 판매한 물품의 금액을 받는처리 및 구입한 아이템의 수령을 처리 한다.)
	CIE_UM_Modify_Market,	//마켓 상태 변경
	CIE_UM_Reg_Money,		//게임 머니 등록
	CIE_UM_Reg_Cash,		//캐시 등록

	CIE_Sys2Inv,			//시스템 인벤에서 인벤토리로 이동
	CIE_Delete_SysItem,		//시스템 인벤에서 아이템 삭제

	CIE_CS_Select,			//캐쉬 잔액 쿼리
	CIE_CS_Buy,				//캐쉬 아이템 구매
	CIE_CS_Gift,			//캐쉬 아이템 선물
	CIE_CS_Gift_Recv,		//선물 수령

	// CP
	CPE_REWARD_PVP,			// PvP에서 얻었다.
	CPE_BuyItem,			// Item 구매로 감소
	CPE_SellItem,			// Item 판매로 증가
	CPE_GodCmd,

	CIE_ChangeClass_ByNormal,	// called by God Command
	CIE_ChangeClass_ByGMCmd,	// called by God Command
	CNE_CONTENTS_EVENT,			// ContentsServer Event

	CIE_Rank_Point,
	CIE_SafeMode,				// 안전거품
	CIE_MissionUpdate,
	IMEPT_INSURANCE_REVIVE,		//사망보험
	IMEPT_INSURANCE_SOUL_CRAFT,	//소울 크래프트 보험
	IMEPT_INSURANCE_ENCHANT,	//인챈트 보험 보험
	IMEPT_WORLD_MAP,			//월드맵 업데이트
	IMEPT_RECENT,				//Recent 업데이트

	CAE_Achievement,		// 업적 관련 어빌 변경
	CAE_Achievement2Inv,	// 훈장 -> 아이템 변경

	CAE_HiddenOpen,			// 히든맵 오픈여부
		
	CIE_Coupon,				// 쿠폰 사용

	CIE_UserMapMove,		// 순간이동 카드 사용
	CIE_MoveToPartyMember,	// 파티원 찾기 이동 카드 사용
	CIE_RentalSafeExtend,	// 창고 확장

	SYS_UpdateCacheDB,		//

	CIE_UM_Modify_Market_State,// 마켓 상태 변경 OPEN, CLOSE, EDIT
	CIE_SummonPartyMember,	// 파티원 소환
	CIE_UM_Market_Remove,	// 마켓 삭제

	CIE_Equip,				//장착
	CIE_UnEquip,			//장착 해지
	CIE_MoveToSafe,			//창고 보관
	CIE_MoveToInv,			//창고 꺼내기
	
	CIE_MoveFromGuildInvToGuildInv, // 길드금고내 이동
	CIE_MoveFromGuildInvToInv,		// 길드금고 꺼내기
	CIE_MoveFromInvToGuildInv,		// 길드금고 보관

	CAE_Inv2Achievement,	// 아이템 -> 훈장 변경

	IMPET_MapmoveDeleteItem, // 맵이동시 삭제 되는 아이템 이벤트

	CIE_OpenPack,			//팩 열어서 보상
	CIE_OpenPack2,			//팩 열어서 보상 (No Rarity)
	CIE_HiddenPack,			//히든 보상 상자 열어서 보상

	CIE_CS_Add_TimeLimit,	// 캐시 아이템 시간 연장
	CPE_Event,				// 이벤트 -> 유저 정보 수정

	CNE_POST_SYSTEM_MAIL,		// 시스템 메일 전송
	CNE_POST_SYSTEM_MAIL_ORDER,
	CNE_CONTENTS_SENDTOUSER,// SendToUser packet

	CIE_Item_Bind,			// 아이템 잠금
	CIE_Item_Unbind,		// 아이템 잠금 해제

	CIE_Set_DefaultItem,	// 기본 장착 아이템 변경

	CIE_Create_CharacterCard,// 캐릭터 카드 생성
	CIE_Modify_RecommendPoint,	// 추천 포인트 변경
	CIE_Modify_CharacterCard,	// 캐릭터 카드 정보 변경
	CIE_Delete_CharacterCard,	// 캐릭터 카드 삭제

	CIE_Modify_UserPortal,		// 위치 기억 포탈 수정

	CIE_Gen_Socket,
	CIE_Set_MonsterCard,
	CIE_Remove_MonsterCard,
	CIE_Del_MonsterCard,
	CIE_EXTRACTION_MonsterCard,

	CIE_Cash_Pack,				// 캐시팩을 사용하여 캐시를 받음
	CIE_CoinChange_Buy,			// 코인교환
	CIE_GemStore_Buy,			// 보석 교환기
	CIE_CollectAntique,			// 골동품 수집
	CIE_ExchangeGem,			// 보석상인 교환
	CIE_Rollback,				// 인첸트 롤백
	CIE_Locked_Chest,			// 잠긴 상자 열기
	CIE_Open_Gamble,			// 겜블 아이템 열기
	CIE_Convert_Item,			// 아이템 변환
	CIE_HatchPet,				// Pet 부화
	CIE_RenamePet,				// Pet 이름 변경
	CIE_SetAbilPet,				// Pet Abil Set
	CIE_ItemPet,				// Pet Item
	CIE_ItemPetOnlyPop,			// 
	CIE_LOAD_PET_ITEM,			// Pet Item Load
	CIE_Use_ExpCard,			// 경험치 카드 사용
	CIE_CCE_Reward,				// 캐릭 생성 이벤트 보상

	CIE_EmporiaFunction,		// 엠포리아 기능 이용
    CIE_EmporiaReserve,         // 엠포리아 도전
    CIE_EmporiaThrow,           // 엠포리아 도전
    CIE_EmporiaReserve_Fail,    // 엠포리아 도전 실패

	CIE_CASH_LIMIT_ITEM_REQ,	// 현재 한정판매 문의
	CIE_CASH_LIMIT_ITEM_RCV,
	CIE_CASH_BALANCE_REQ,		// 현재 캐시량 문의
	CIE_CASH_BALANCE_RCV,
	CIE_CASH_USE_REQ,			// Cash 사용 요청
	CIE_CASH_USE_RCV,
	CIE_CASH_PROCESS_END,		// Cash 작업 마무리.

	CIE_Buy_MyHome,				// 마이홈 구입

	CIE_MixupItem,				// 아이템 조합
	CIE_UM_Article_CashBuy,		// OpenMarket 에서 Cash 구매
	CIE_UM_Article_CashReg,		// OpenMarket 에 Cash article 등록
	CIE_UM_Article_CashBack,	// OpenMarket 판매금액 회수(Cash로 회수)

	CIE_Home_Equip,				// 홈 아이템 배치
	CIE_Home_UnEquip,			// 홈 아이템 배치
	CIE_Home_Modify,			// 집 정보 수정
	CIE_Home_Bidding,			// 집 입찰 참여

	CIE_Home_Auction_Reg,		// 경매 등록
	CIE_Home_Auction_Unreg,		// 경매 취소
	CIE_Home_Auction_End,		// 경매 종료
	CIE_Home_Attachment,		// 홈 차압
	CIE_Home_Noti_PayTex,		// 홈 소지자에게 세금 납부 알림 메일 전송
	CIE_Home_PayTex,			// 홈 세금 지급

	CIE_Home_Item_Modify,		// 홈 아이템 수정 
	CIE_Home_Unit_Sync,			// 홈 유닛끼리 공유해야 하는 정보 동기화
	CIE_GambleMachine,			// 겜블머신
	CIE_ACTIVE_STATUS_SET,		// Status 변경하기(STR,INT,CON,DEX)

	CIE_TransTower_Save_Recent,	// Save TransTower Recent Position
	CIE_TransTower_Open_Map,	// Save TransTower Open Map

	CIE_OpenEventItemReward,	// 퍼블리셔 수정 가능 아이템 지급 박스	
	CIE_SendEventItemReward,	// 퍼블리셔 수정 가능 아이템 지급 박스	

	CNE_POST_GROUP_MAIL_ORDER,	// 그룹 메일 전송

	CIE_Home_SideJob_Insert,	// 아르바이트 생성
	CIE_Home_SideJob_Remove,	// 아르바이트 삭제
	CIE_Home_SideJob_Modify,	// 아르바이트 수정

	CIE_GambleMachine_Shop,		// 캐시샵 겜블

	CIE_Home_SideJob_Enter,		// 아르바이트 들어가기

	CIE_Inventory_Sort,			// 아이템 소트 기능

	CIE_GambleMachine_Mixup,	// 캐시샵 아이템 조합

	CIE_Item_Enchant_Shift,		// 아이템 인첸트 전이
	
	CIE_Revive_Feather_Login,	// 로그인 부활깃털 보상
	CIE_Revive_Feather_LevelUp,	// 레벨업 부활깃털 보상
	CIE_Login_Event,			// 접속 시간 유지 이벤트.

	CIE_JOBSKILL,				// 직업스킬(채집)
	CIE_JOBSKILL_LEARN,			// 직업스킬(배우기)
	CIE_JOBSKILL_DELETE,			// 직업스킬(삭제)
	CIE_JOBSKILL3_CREATE_ITEM,	// 채집3차 아이템

	CIE_SoulTransfer_Extract,		// 영력 전이 - 추출
	CIE_SoulTransfer_Transition,	// 영력 전이 - 삽입
	CIE_OpenTreasureChestReward,	// 퍼블리셔 수정 가능 아이템 지급 박스	
	CIE_SendTreasureChestReward,	// 퍼블리셔 수정 가능 아이템 지급 박스	

	CIE_ExtractElement,			//원소 추출

	CIE_PREMIUM_SERVICE,			// 프리미엄서비스

	CIE_Manufacture,			//즉석 가공

	CIE_PetUpgrade,				//펫 전직
	CIE_PetProduceItem,			//펫 스킬 (아이템 생산)

	CIE_RaceEvent,				//달리기 이벤트
	CIE_Constellation,			//별자리 던전 보상
	CIE_Notice,			//특정 아이템 브로드케스트 공지.
	CIE_UserQuestComplete,		// 캐릭터 퀘스트 완료
	CIE_DailyReward,			// Daily reward system
	IMEPT_QUEST_BATTLE_PASS,	// Battle Pass Quest
}EItemModifyParentEventType;

typedef struct tagPlayerModifyOrderData_AddCash
{
	tagPlayerModifyOrderData_AddCash(){}
	tagPlayerModifyOrderData_AddCash(BM::GUID const kMemberGuid,__int64 const i64AddCash,std::wstring const & kCharName,EItemModifyParentEventType const kCause)
	{
		MemberGuId(kMemberGuid);
		AddCash(i64AddCash);
		CharName(kCharName);
		Cause(kCause);
	}
	CLASS_DECLARATION_S(BM::GUID, MemberGuId);
	CLASS_DECLARATION_S(__int64, AddCash);
	CLASS_DECLARATION_S(std::wstring, CharName);
	CLASS_DECLARATION_S(EItemModifyParentEventType,Cause);
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kMemberGuId);
		kPacket.Push(m_kAddCash);
		kPacket.Push(m_kCharName);
		kPacket.Push(m_kCause);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kMemberGuId);
		kPacket.Pop(m_kAddCash);
		kPacket.Pop(m_kCharName);
		kPacket.Pop(m_kCause);
	}
}SPMOD_AddCash;

typedef struct tagDailyRewardOrderData
{
	PgDailyReward m_kDailyReward;

	tagDailyRewardOrderData() {}
	explicit tagDailyRewardOrderData(PgDailyReward kDailyReward): m_kDailyReward(kDailyReward) { }

	void  WriteToPacket( BM::Stream & kPacket )const
	{
		m_kDailyReward.WriteToPacket(kPacket);
	}

	void ReadFromPacket( BM::Stream & kPacket )
	{
		m_kDailyReward.ReadFromPacket(kPacket);
	}
} SPMOD_DailyReward;

typedef struct tagPlayerModifyOrderData_HatchPet
{
	tagPlayerModifyOrderData_HatchPet()
	{}

	explicit tagPlayerModifyOrderData_HatchPet(PgBase_Item const &kItem, SItemPos const &kInItemPos, int const iInAddCount, bool const bDoRemove = false)
		:	kSPMOD_MC( kItem, kInItemPos, iInAddCount, bDoRemove )
	{
	}

	void  WriteToPacket( BM::Stream & kPacket )const
	{
		kSPMOD_MC.WriteToPacket( kPacket );
		kPetItem.WriteToPacket( kPacket );
	}

	void ReadFromPacket( BM::Stream & kPacket )
	{
		kSPMOD_MC.ReadFromPacket( kPacket );
		kPetItem.ReadFromPacket( kPacket );
	}

	BM::GUID const &GetPetID()const{return kPetItem.Guid();}

	SPMOD_Modify_Count	kSPMOD_MC;
	PgBase_Item kPetItem;
}SPMOD_HatchPet;

typedef struct tagPlayerModifyOrderData_RenamePet
{
	tagPlayerModifyOrderData_RenamePet()
	{}

	explicit tagPlayerModifyOrderData_RenamePet( SItemPos const &kItemPos, std::wstring const &wstrName )
		:	kPetItemPos( kItemPos )
		,	wstrPetName( wstrName )
	{
	}

	void  WriteToPacket( BM::Stream & kPacket )const
	{
		kPacket.Push( kPetItemPos );
		kPacket.Push( wstrPetName );
	}

	void ReadFromPacket( BM::Stream & kPacket )
	{
		kPacket.Pop( kPetItemPos );
		kPacket.Pop( wstrPetName );
	}

	SItemPos			kPetItemPos;
	std::wstring		wstrPetName;
}SPMOD_RenamePet;

typedef struct tagPlayerModifyOrderData_AddAbilPet
{
	typedef std::list< SPMOD_SAbil >	CONT_ABILLIST;

	tagPlayerModifyOrderData_AddAbilPet()
	{}

	tagPlayerModifyOrderData_AddAbilPet( BM::GUID const &kPetID, SItemPos const &kPetItemPos )
		:	m_kPetID(kPetID)
		,	m_kPetItemPos( kPetItemPos )
	{}

	CLASS_DECLARATION_S( BM::GUID, PetID );
	CLASS_DECLARATION_S( SItemPos, PetItemPos );
	CONT_ABILLIST	kAbilList;

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push( m_kPetID );
		kPacket.Push( m_kPetItemPos );
		kPacket.Push( kAbilList );
	}

	bool ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop( m_kPetID );
		kPacket.Pop( m_kPetItemPos );
		return kPacket.Pop( kAbilList );
	}

	size_t min_size()const
	{
		return	sizeof(m_kPetID)
			+	sizeof(m_kPetItemPos)
			+	sizeof(size_t);
	}

}SPMOD_AddAbilPet;

typedef struct tagPlayerModifyOrderData_AddEmotion
{
	tagPlayerModifyOrderData_AddEmotion(){}
	tagPlayerModifyOrderData_AddEmotion(BYTE const _type,int const _group)
	{
		Type(_type);
		GroupNo(_group);
	}
	CLASS_DECLARATION_S(BYTE,Type);
	CLASS_DECLARATION_S(int,GroupNo);
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kType);
		kPacket.Push(m_kGroupNo);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kType);
		kPacket.Pop(m_kGroupNo);
	}
}SPMOD_AddEmotion;

typedef struct tagPlayerModifyOrderData_PremiumService
{
	tagPlayerModifyOrderData_PremiumService(int const iServiceNo=0)
		: m_kServiceNo(iServiceNo)
	{}
	CLASS_DECLARATION_S(int, ServiceNo);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_PremiumService;

typedef struct tagPlayerModifyOrderData_PremiumServiceModify
{
	tagPlayerModifyOrderData_PremiumServiceModify(int const iServiceNo=0,int const iUseDate=0)
		: m_kServiceNo(iServiceNo), m_kUseDate(iUseDate)
	{}
	CLASS_DECLARATION_S(int, ServiceNo);
	CLASS_DECLARATION_S(int, UseDate);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_PremiumServiceModify;

typedef struct tagPlayerModifyOrderData_PremiumArticle
{
	tagPlayerModifyOrderData_PremiumArticle(int const iArticleType=0)
		: m_kArticleType(iArticleType)
	{}
	CLASS_DECLARATION_S(int, ArticleType);
	BM::Stream m_kPacket;

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kArticleType);
		kPacket.Push(m_kPacket.Data());
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kArticleType);
		kPacket.Pop(m_kPacket.Data());
		m_kPacket.PosAdjust();
	}
}SPMOD_PremiumArticle;

typedef struct tagPlayerModifyOrderData_AddPacket
{
	tagPlayerModifyOrderData_AddPacket() {}

	template<typename T>
	tagPlayerModifyOrderData_AddPacket(T const& kData)
	{
		m_kPacket.Push(kData);
	}

	BM::Stream m_kPacket;

	bool IsEmpty()const { return m_kPacket.IsEmpty(); }
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kPacket.Data());
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kPacket.Data());
		m_kPacket.PosAdjust();
	}
}SPMOD_AddPacket;

typedef enum 
{
	GIOT_NONE = 0,
	GIOT_MAKING		= 1,
	GIOT_FIELD		= 2,
	GIOT_MISSION	= 3,
	GIOT_MISSION1	= 4,
	GIOT_MISSION2	= 5,
	GIOT_MISSION3	= 6,
	GIOT_MISSION4	= 7,

	GIOT_CHEST		= 8,
	GIOT_QUEST		= GIOT_NONE,

	GIOT_GM_NORMAL		= 101,//- 노말, 
	GIOT_GM_RARE		= 102,//- 레어, 
	GIOT_GM_UNIQUE		= 103,//- 유니크, 
	GIOT_GM_ARTIFACT	= 104,//- 아티펙
	GIOT_GM_LEGEND		= 105,//-  레전드 

	GIOT_GEMSTORE		= 201,// GEM STORE

	GIOT_MISSION_GADACOIN = 300,
	GIOT_MISSION_RANK	= 301,
}E_GEN_ITEM_OPTION_TYPE;

typedef enum eItemModifyEventType : __int64
{//아이템은 수량, 속성을 제외하고는 바뀔것이 없습니다.
//개별 이벤트에 대한 플래그

	IMET_NONE = 0,
	//Modify 계열은 Target을 기준으로 함.
	IMET_CMP_BASE				= 0x0000000000000FFF,	//(Bit flag 영역)인벤토리가 하느냐 플래그는 FFF 영역에.
	IMET_MODIFY_ENCHANT			= 0x0000000000000001,//속성 바꾸기
	IMET_MODIFY_COUNT			= 0x0000000000000002,//수량 및 내구도 수정.	단 Max 내구도를 넘으면 안됨. 이걸로 지우기도 가능.
	IMET_MODIFY_POS				= 0x0000000000000004,//위치 이동
	//Any 계열은 기본 아이템을 아무자리에 넣음.
	IMET_INSERT_FIXED			= 0x0000000000000008,//생성된 값을 특정 or 아무 자리에 넣기
	IMET_ADD_ANY				= 0x0000000000000010,//기본 아이템을 아무 자리라도 넣기 or 빼기
	IMET_ADD_MONEY				= 0x0000000000000020,//돈 수정//SPMOD_Add_Money
	IMET_ADD_FIXED				= 0x0000000000000040,//아이템을 특정 위치에 생성 시킨다.IMET_INSERT_FIXED 와는 다른 동작
	IMET_MODIFY_DB2INV			= 0x0000000000000080,//디비에 있는 아이템을 바로 
	IMET_ADD_CP					= 0x0000000000000100,//CP 수정//
	IMET_MODIFY_SYS2INV			= 0x0000000000000200,//임시 보관 창에서 인벤토리로 이동
	IMET_DELETE_SYSITEM			= 0x0000000000000400,//임시 보관 창에서 아이템 삭제
	IMET_MODIFY_EXTEND_DATA		= 0x0000000000000800,

	//추가 플래그	//해당 이벤트가 추가적으로 어떤 의미가 있는지.
	IMC_CMP_BASE				= 0x0000000000FFF000,	// Bit flag 영역

	IMC_DB_INIT					= 0x0000000000001000,
	IMC_UPGRADE_SUCCESS			= 0x0000000000002000,
	IMC_UPGRADE_FAILED			= 0x0000000000004000,
	IMC_DEC_DUR_BY_USE			= 0x0000000000008000,//사용
	IMC_DEC_DUR_BY_BATTLE		= 0x0000000000010000,//싸움중
	IMC_DEC_DUR_BY_SHOP_SELL	= 0x0000000000020000,//판매
	IMC_INC_DUR_BY_REPAIR		= 0x0000000000040000,//수리
	IMC_POS_BY_EXCHANGE			= 0x0000000000080000,//위치 바뀌는것
	IMC_DEC_DUR_BY_PENALTY		= 0x0000000000100000,//사망패널티
	IMC_POS_BY_SORT				= 0x0000000000200000,//정렬
	IMC_GUILD_INV				= 0x0000000000400000,//길드인벤
	IMC_MODIFY_PET_INV			= 0x0000000000800000,//펫인벤....
	IMC_UNIT_ABIL_CHANGED		= IMET_MODIFY_POS,
	
	IMET_CAUSE_AREA				= 0xFFFF000000000000,	// 이영역은 Bit flag 값으로 할 필요 없음.
	IMET_SET_ABIL				= 0x0001000000000000,//레벨.
	IMET_ADD_ABIL				= 0x0002000000000000,
	IMET_SET_ABIL64				= 0x0003000000000000,//레벨.
	IMET_ADD_ABIL64				= 0x0004000000000000,
	IMET_ADD_SKILL				= 0x0005000000000000,
	IMET_ADD_INGQUEST			= 0x0006000000000000,//진행정보 추가/제거/상태 강제 변화 (시작/완료/포기)(완료가능/실패/진행중)
	IMET_ADD_ENDQUEST			= 0x0007000000000000,//완료정보 갱신
	IMET_ADD_INGQUEST_PARAM		= 0x0008000000000000,//진행정보 파라메터 변화
	IMET_BUILD_DAYLOOP			= 0x0009000000000000,//일별로 반복되는 퀘스트 빌드
	IMET_ADD_GUILDEXP			= 0x000A000000000000,
	IMET_SET_GUID				= 0x000B000000000000,// 커플, 길드
	IMET_ADD_RANK_POINT			= 0x000C000000000000,// 랭킹포인트
	IMET_CONTENTS_EVENT			= 0x000D000000000000,// Contents Server Event (MapMove or Disconnect)
	IMET_ADD_PVP_RANK_RECORD	= 0x000E000000000000,// PVP 기록
	IMET_ADD_WORLD_MAP			= 0x000F000000000000,// 월드맵 기록
	IMET_END_MISSION			= 0x0010000000000000,// 미션 결과창 보기(시나리오 미션이 아닐 경우만)
	IMET_SWAP_MISSION			= 0x0011000000000000,// 미션 스테이지 변경하기
	IMET_REQ_REMOVE_GROUND		= 0x0012000000000000,
	IMET_END_MISSION_EVENT		= 0x0013000000000000,// 이벤트 미션 등급
	IMET_SET_SWEETHEART			= 0x0014000000000000,// 연인
	IMET_BUILD_RANDOM_QUEST		= 0x0015000000000000,// 랜덤 퀘스트 만들어
	IMET_SET_DATE_CONTENTS		= 0x0016000000000000,// 날자,시간 컨텐츠 설정
	IMET_BUILD_TACTICS_QUEST	= 0x0017000000000000,// 길드 랜덤 퀘스트 만들어
	IMET_ADD_ABIL64_MAPUSE		= 0x0018000000000000,// 맵서버에서 값을 AddAbil 할 값 (ContentsServer 에서는 skip 하기)
	IMET_BUILD_WANTED_QUEST		= 0x0019000000000000,// 현상 수배 퀘스트 빌드
	IMET_BUILD_DAYLOOP_FORCE	= 0x001A000000000000,// 강제로 일일 퀘스트들 빌드
	IMET_CLEAR_ING_QUEST		= 0x001B000000000000,// 진행중 퀘스트 모두 제거
	IMET_INIT_WANTED_QUEST		= 0x0020000000000000,// 현상 수배 퀘스트(클리어 카운트) 초기화
	IMET_PRE_CHECK_WANTED_QUEST = 0x0021000000000000,// 현상 수배 퀘스트 빌드 할 수 있는지 체크(플레이어의 레벨에 빌드 할 수 있는 퀘스트가 있는가?)
	IMET_BUILD_WEEKLOOP			= 0x0022000000000000,// 주간퀘스트

//	IMET_SET_POINT3				= 0x0030000000000000,
	IMET_SET_RENTALSAFETIME		= 0x0031000000000000,// 캐시 창고 유지시간 설정

	IMET_OPEN_MARKET			= 0x0032000000000000,	// OM Market open
	IMET_ADD_ARTICLE			= 0x0033000000000000,	// OM article 등록
	IMET_REMOVE_ARTICLE			= 0x0034000000000000,	// OM article 회수
	IMET_BUY_ARTICLE			= 0x0035000000000000,	// OM article 구매
	IMET_MODIFY_MARKET_INFO		= 0x0036000000000000,
	IMET_MODIFY_MARKET_STATE	= 0x0037000000000000,
	IMET_REMOVE_ARTICLE_CASH	= 0x0038000000000000,	// OM에서 Cash article 삭제

	IMET_CREATE_MAIL			= 0x0040000000000000,
	IMET_REMOVE_DEALING			= 0x0041000000000000,	// OM 판매 금액 회수
	IMET_REMOVE_MARKET			= 0x0042000000000000,
	IMET_ADD_CASH				= 0x0043000000000000,

	IMET_COMPLETE_ACHIEVEMENT	= 0x0044000000000000,	// 완료된 업적 기록
	IMET_ACHIEVEMENT2INV		= 0x0045000000000000,	// 훈장을 아이템으로 변환
	IMET_SET_ACHIEVEMENT		= 0x0046000000000000,
	IMET_INV2ACHIEVEMENT		= 0x0047000000000000,	// 아이템을 다시 훈장으로 반환

	IMET_ADD_UNBIND_DATE		= 0x0048000000000000,// 아이템 잠금 해지 대기 기간 등록
	IMET_CASH_SHOP_GIFT			= 0x0049000000000000,
	IMET_DEL_UNBIND_DATE		= 0x0050000000000000,// 아이템 잠금 해지 대기 기간 삭제

	IMET_SET_DEFAULT_ITEM		= 0x0051000000000000,
	IMET_INVENTORY_EXTEND		= 0x0052000000000000,// 인벤토리 확장

	IMET_CREATE_CHARACTERCARD	= 0x0053000000000000,
	IMET_MODIFY_CHARACTERCARD	= 0x0054000000000000,
	IMET_MODIFY_REFRESHDATE		= 0x0055000000000000,

	IMET_SET_RECOMMENDPOINT		= 0x0056000000000000,
	IMET_ADD_RECOMMENDPOINT		= 0x0057000000000000,
	IMET_ADD_POPULARPOINT		= 0x0058000000000000,
	IMET_MODIFY_CARD_COMMENT	= 0x0059000000000000,
	IMET_MODIFY_CARD_STATE		= 0x0060000000000000,

	IMET_CREATE_PORTAL			= 0x0061000000000000,
	IMET_DELETE_PORTAL			= 0x0062000000000000,

	IMET_ADD_EMOTION			= 0x0063000000000000,
	IMET_PET					= 0x0064000000000000,
	IMET_PET_ITEM				= IMET_PET|IMET_MODIFY_POS,
	IMET_PET_DUMP				= 0x0065000000000000,

	IMET_ADD_MYHOME				= 0x0066000000000000,

	IMET_EXTEND_MAX_IDX			= 0x0067000000000000,

	IMET_MYHOME_BIDDING			= 0x0068000000000000,
	IMET_MYHOME_AUCTION_REG		= 0x0069000000000000,
	IMET_MYHOME_VISITFLAG		= 0x006A000000000000,
	IMET_MYHOME_AUCTION_END		= 0x006B000000000000,
	IMET_MYHOME_AUCTION_UNREG	= 0x006C000000000000,
	IMET_MYHOME_ATTACHMENT		= 0x006D000000000000,
	IMET_MYHOME_NOTI_PAY_TEX	= 0x006E000000000000,
	IMET_MYHOME_PAY_TEX			= 0x006F000000000000,
	IMET_MYHOME_RETURN_ITEM		= 0x0070000000000000,
	IMET_MYHOME_STYLE_MODIFY	= 0x0071000000000000,
	IMET_MYHOME_TEX_TIME		= 0x0072000000000000,// 세금 납부 시간 변경
	IMET_MYHOME_AUCTION_TIME	= 0x0073000000000000,// 경매 종료 시간 변경
	IMET_CREATE_MAIL_BYGUID		= 0x0074000000000000,
	IMET_MYHOME_SET_OWNER_INFO	= 0x0075000000000000,
	IMET_MYHOME_SET_HOME_STATE	= 0x0076000000000000,
	IMET_MYHOME_RESERVE			= 0x0077000000000000,// 유닛 예약
	IMET_MYHOME_SET_HOMEADDR	= 0x0078000000000000,

	IMET_EXTEND_CHARACTER_NUM	= 0x0079000000000000,// 캐릭 생성 개수 확장
	IMET_ACTIVE_STATUS			= 0x007A000000000000, // ActiveStatus

	IMET_TRANSTOWER_SAVE_RECENT = 0x007B000000000000,
	IMET_COMPLETE_HIDDEN_OPEN	= 0x007C000000000000,
	IMET_SET_HIDDEN_OPEN		= 0x007D000000000000,

	IMET_SET_TODAYPOPULARPOINT	= 0x007E000000000000,// 오늘의 인기도 설정
	IMET_ADD_ABIL64EX			= 0x007F000000000000,// 휴식 경험치 때문에 어빌에 추가 정보 추가 가능하게 작업
	IMET_SIDEJOB_INSERT			= 0x0080000000000000,// 아르바이트 추가
	IMET_SIDEJOB_REMOVE			= 0x0081000000000000,// 아르바이트 삭제
	IMET_SIDEJOB_MODIFY			= 0x0082000000000000,// 아르바이트 수정
	IMET_SIDEJOB_COMPLETE		= 0x0083000000000000,// 아르바이트 완료
	IMET_MYHOME_SET_EQUIP_COUNT	= 0x0084000000000000,// 내부 마이홈 장비된 아이템 개수 동기화

	IMET_GAMBLE_INSERT			= 0x0085000000000000,
	IMET_GAMBLE_MODIFY			= 0x0086000000000000,
	IMET_GAMBLE_REMOVE			= 0x0087000000000000,

	IMET_MYHOME_SIDEJOB_ENTER	= 0x0088000000000000,// 아르바이트 진행중인 마이홈 들어가기

	IMET_SAVE_ACHIEVEMENTS		= 0x0089000000000000,// 업적 저장

	IMET_MYHOME_SET_VISITLOGCOUNT = 0x008A000000000000,// 방문록 기록 개수 갱신
	IMET_MYHOME_ADD_VISITLOGCOUNT = 0x008B000000000000,// 방문록 기록 개수 갱신

	IMET_GAMBLE_MIXUP_INSERT		= 0x008C000000000000,// 캐시샵 코스튬 조합
	IMET_GAMBLE_MIXUP_MODIFY		= 0x008D000000000000,// 캐시샵 코스튬 조합
	IMET_GAMBLE_MIXUP_REMOVE		= 0x008E000000000000,// 캐시샵 코스튬 조합

	IMET_SET_ACHIEVEMENT_TIMELIMIT	= 0x008F000000000000,// 업적 사용 기간 설정

	IMET_SET_SKILLEXTEND			= 0x0090000000000000,// 스킬 배우기 확장

	IMET_MYHOME_REMOVE				= 0x0091000000000000,// 마이홈 삭제

	IMET_SET_STRATEGYSKILL_TABNO	= 0x0092000000000000,// 전략스킬 변경

	IMET_EMPORIA_FUNCTION			= 0x0093000000000000,

	IMET_GM_INIT_SKILL				= 0x0094000000000000,	

	IMET_GUILD_INV_EXTEND_LINE		= 0x0095000000000000,
	IMET_GUILD_INV_EXTEND_TAB		= 0x0096000000000000,
	IMET_JOBSKILL_ADD_EXPERTNESS	= 0x0097000000000000,
	IMET_JOBSKILL_DEL_EXPERTNESS	= 0x0098000000000000,
	IMET_JOBSKILL_SET_SAVEIDX		= 0x0099000000000000,
	IMET_JOBSKILL_ADD_EXHAUSTION	= 0x009A000000000000,
	IMET_JOBSKILL_RESET_EXHAUSTION	= 0x009B000000000000,
	IMET_JOBSKILL_INIT_EXHAUSTION	= 0x009C000000000000,
	IMET_JOBSKILL_SET_EXPERTNESS	= 0x009D000000000000,	// !!주의!! 등록도 겸한다
	IMET_DEL_SKILL				    = 0x009E000000000000,
	IMET_JOBSKILL_ADD_BLESSPOINT	= 0x009F000000000000,
	IMET_JOBSKILL_INIT_BLESSPOINT	= 0x00A0000000000000,

	IMET_PREMIUM_SERVICE_INSERT		= 0x00A3000000000000,
	IMET_PREMIUM_SERVICE_MODIFY		= 0x00A4000000000000,
	IMET_PREMIUM_SERVICE_REMOVE		= 0x00A5000000000000,
	IMET_PREMIUM_ARTICLE_MODIFY		= 0x00A6000000000000,

	IMET_DAILY_REWARD_UPDATE		= 0x00A7000000000000,

	IMD_MERGED_SUCCESS			= 0x0000000000100000,	// Item이 Merge 되었다.


}EItemModifyEventType;


typedef struct tagDBItemStateChange
{
	tagDBItemStateChange()
	{
		State(DISCT_NONE);
		Cause(0);
		IsQuery(true);
		OwnerType(OOT_Player);
	}
	tagDBItemStateChange(const EDBItemStateChangeType kState, __int64 const iCause/*EItemModifyEventType*/, BM::GUID const &kRetOwnerGuid)
	{
		State(kState);
		Cause(iCause);
		IsQuery(true);
		RetOwnerGuid(kRetOwnerGuid);
		CharacterGuid(kRetOwnerGuid);
		
		if( iCause & IMC_GUILD_INV )
		{
			OwnerType(OOT_Guild);
		}
		else
		{
			OwnerType(OOT_Player);
		}
	}
	tagDBItemStateChange(const EDBItemStateChangeType kState, __int64 const iCause/*EItemModifyEventType*/, BM::GUID const &kRetOwnerGuid, BM::Stream const & kPacket)
	{
		State(kState);
		Cause(iCause);
		IsQuery(true);
		RetOwnerGuid(kRetOwnerGuid);
		CharacterGuid(kRetOwnerGuid);
		kAddonData = kPacket;

		if( iCause & IMC_GUILD_INV )
		{
			OwnerType(OOT_Guild);
		}
		else
		{
			OwnerType(OOT_Player);
		}
	}
	tagDBItemStateChange(const EDBItemStateChangeType kState, __int64 const iCause/*EItemModifyEventType*/, BM::GUID const &kRetOwnerGuid, BM::GUID const &kCharacterGuid, PgItemWrapper const &kPrevItemWrapper, PgItemWrapper const &kItemWrapper)
	{
		State(kState);
		Cause(iCause);
		IsQuery(true);
		RetOwnerGuid(kRetOwnerGuid);
		CharacterGuid(kCharacterGuid);
		PrevItemWrapper(kPrevItemWrapper);
		ItemWrapper(kItemWrapper);

		if( iCause & IMC_GUILD_INV )
		{
			OwnerType(OOT_Guild);
		}
		else
		{
			OwnerType(OOT_Player);
		}
	}

	CLASS_DECLARATION_S(EDBItemStateChangeType, State); //1:Create 2:Modify 3:Remove
	
	CLASS_DECLARATION_S(__int64, Cause);

	CLASS_DECLARATION_S(BM::GUID, RetOwnerGuid);//소유자 Guid (캐릭터 guid 또는 맴버 guid 둘 다 사용 가능하다.)
	CLASS_DECLARATION_S(BM::GUID, CharacterGuid);//소유자 캐릭터 Guid

	CLASS_DECLARATION_S(PgItemWrapper, ItemWrapper);
	CLASS_DECLARATION_S(PgItemWrapper, PrevItemWrapper);//이전 정보 -> 안내용 책자로 써라.

	CLASS_DECLARATION_S(bool, IsQuery);		// 실제로 DB쿼리를 할것인지 (패킷전송 없음)
	CLASS_DECLARATION_S(EOrderOwnerType, OwnerType); // 결과를 받을 대상 (항상 기본은 OOT_Player) (패킷전송 없음)

	BM::Stream	kAddonData;				// 추가 데이터 전송용

	size_t min_size()const
	{
		return 
			sizeof(m_kState)+ //1:Create 2:Modify 3:Remove
			sizeof(m_kCause)+
			sizeof(m_kRetOwnerGuid)+//소유자 Guid
			sizeof(m_kCharacterGuid)+//소유자 Guid
			m_kItemWrapper.min_size()+
			m_kPrevItemWrapper.min_size()+
			sizeof(size_t);// kPacket.Pop(kAddonData.Data());		// 추가 데이터 전송용
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kState); //1:Create 2:Modify 3:Remove
		kPacket.Push(m_kCause);
		kPacket.Push(m_kRetOwnerGuid);//소유자 Guid
		kPacket.Push(m_kCharacterGuid);//소유자 Guid
		m_kItemWrapper.WriteToPacket(kPacket);
		m_kPrevItemWrapper.WriteToPacket(kPacket);//이전 정보 -> 안내용 책자로 써라.
		kPacket.Push(kAddonData.Data());		// 추가 데이터 전송용
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kState); //1:Create 2:Modify 3:Remove
		kPacket.Pop(m_kCause);
		kPacket.Pop(m_kRetOwnerGuid);//소유자 Guid
		kPacket.Pop(m_kCharacterGuid);//소유자 Guid
		m_kItemWrapper.ReadFromPacket(kPacket);
		m_kPrevItemWrapper.ReadFromPacket(kPacket);
		kPacket.Pop(kAddonData.Data());		// 추가 데이터 전송용
		kAddonData.PosAdjust();
	}

}DB_ITEM_STATE_CHANGE;//디비 기록용


typedef std::vector< DB_ITEM_STATE_CHANGE > DB_ITEM_STATE_CHANGE_ARRAY;

typedef std::list< PgItemWrapper* > EXCHANGE_CANDIDATE_LIST;

enum E_ITEM_PLUSE_UPGRADE_LEVEL_LIMIT
{
	IPULL_NORMAL_LIMIT		= 7,
	IPULL_RARE_LIMIT		= 10,
	IPULL_UNIQUE_LIMIT		= 15,
	IPULL_ARTIFACT_LIMIT	= 20,//대만 요청으로 임시로 20 까지로 처리
	IPULL_LEGEND_LIMIT		= 25,
	IPULL_LIMIT_MAX			= 31,
};

int const ITEM_PLUSE_UPGRADE_NOTI_LIMIT = 19;
int const ITEM_PLUSE_UPGRADE_NOTI_START = 10;

int const ITEM_RARITY_LIMIT = 100;
int const GEN_RARITY_ARTIFACT_LIMIT = 90;

enum eElementDmgType
{
	ET_D_NONE				=	0,
	ET_D_DESTROY			=	1,
	ET_D_FIRE				=	2,
	ET_D_ICE				=	3,
	ET_D_NATURE				=	4,
	ET_D_CURSE				=	5,
	ET_R_DESTROY			=	11,
	ET_R_FIRE				=	12,
	ET_R_ICE				=	13,
	ET_R_NATURE				=	14,
	ET_R_CURSE				=	15,
};

enum E_GEMSTORE_TYPE
{
	EGT_OTHER			= 0,
	EGT_JEWEL_1			= 1,
	EGT_JEWEL_2			= 2,
	EGT_JOBSKILL_1		= 3,
	EGT_JOBSKILL_2		= 4,
	EGT_ELEMENT			= 5,
	EGT_ANTIQUE			= 6, //골동품
	EGT_SOCKETCARD		= 7,
	EGT_CARDBOX			= 8,
	EGT_GATHER			= 9, //채집교환
	EGT_EVENT			= 10,//이벤트용
	EGT_ELUNIUM			= 11,
	EGT_VALKYRIE		= 12,//발키리장비 교환
	EGT_PURESILVER_KEY	= 13,//순은열쇠 교환
	EGT_ITEM			= 14,//행운의 알 또는 아이템 교환
	EGT_TREE_ITEM		= 15,//아이템 교환창에 아이템 트리 표시
	EGT_ITEM_SHOP		= 16,//아이템 교환
	EGT_ITEM_PLUS		= 17,//아이템 강화
};

/*
enum E_ITEM_GRADE_RARITY	// 아이템 등급별 영력 수치
{
	IGR_NORMAL_MIN = 0,
	IGR_NORMAL_MAX = 30,
	IGR_RARE_MIN = 31,
	IGR_RARE_MAX = 60,
	IGR_UNIQUE_MIN = 61,
	IGR_UNIQUE_MAX = 85,
	IGR_ARTIFACT_MIN = 86,
	IGR_ARTIFACT_MAX = 98,
	IGR_LEGEND_MIN = 99,
	IGR_LEGEND_MAX = 100,
};

enum E_ITEM_GRADE_OPTION
{
	IGO_NORMAL = 0,
	IGO_RARE = 1,
	IGO_UNIQUE = 2,
	IGO_ARTIFACT = 3,
	IGO_LEGEND = 4,
};

enum E_ITEM_OPTION_LEVEL
{
	IOL_NORMAL_MIN = 0,
	IOL_NORMAL_MAX = 0,
	IOL_RARE_MIN = 1,
	IOL_RARE_MAX = 2,
	IOL_UNIQUE_MIN = 2,
	IOL_UNIQUE_MAX = 5,
	IOL_ARTIFACT_MIN = 3,
	IOL_ARTIFACT_MAX = 8,
	IOL_LEGEND_MIN = 4,
	IOL_LEGEND_MAX = 11,
};
*/
class CItemDef;

extern HRESULT CalcRepairNeedMoney(	PgBase_Item const &rkItem, int const iAddRepairCostRate, int &iOutValue);
extern HRESULT CreatePetInfo( SClassKey const &kPetClassKey, PgItem_PetInfo *pkPetInfo, SEnchantInfo *pkEnchantInfo = NULL );
extern HRESULT CreateSItem(int const iItemNo, int const iCount/* 수량 아이템은 갯수, 내구도 아이템은 내구도. */, int const iRarityControl/*레어도 생성 테이블*/, PgBase_Item &rItem, bool const bIsForceSeal = false);
extern bool CheckIsCashItem(PgBase_Item const &kItem);
extern E_ITEM_GRADE GetItemGrade(PgBase_Item const &kItem);
extern HRESULT RarityValue(E_ITEM_GRADE const eItemGrade, short &nMin, short &nMax);
extern HRESULT GenRarityValue(E_ITEM_GRADE const eItemGrade, short &nRet, bool const bGenItem = false);
extern HRESULT CalcSellMoney(PgBase_Item const &rkItem, __int64 &iOutValue, int const iSellPrice, int const iSellCount/*수량아이템만 쓰는값*/, PgPremiumMgr const& rkPremiumMgr);//판매가
extern int FinddMaxAbilLevel(int const iEquipPos,int const iType,int const iLevelLimit);
extern bool IsCanRepair(int const iItemNo);
extern EEquipType GetEquipType(int const iItemNo);
extern EEquipType GetEquipType(CItemDef const * const pkItemDef);
extern int GetAbilValue(CUnit const* pkUnit, int const Type);

typedef std::list< PgBase_Item > CONT_ITEM_CREATE_ORDER;
bool OrderCreateItem(CONT_ITEM_CREATE_ORDER & rkOrder, int const iItemNo, int const iItemCount, int const iRarityControl = GIOT_NONE);
typedef struct tagItemMoveOrder
{
	tagItemMoveOrder()
	{
	}
	SItemPos kCasterPos;
	SItemPos kTargetPos;
}SItemMoveOrder;

typedef struct tagItemRemoveOrder
{
	tagItemRemoveOrder()
	{
	}
	SItemPos kCasterPos;
}SItemRemoveOrder;

typedef std::vector< SItemRemoveOrder > ContItemRemoveOrder;

typedef struct tagExchangeItem
{
	SItemPos kPos;
	PgBase_Item kItem;

	size_t min_size()const	//TLoadArray_M쓰기위해 필요함
	{
		return 
			sizeof(SItemPos)+
			kItem.min_size();
	}

	tagExchangeItem()
	{
		Init();
	}

	tagExchangeItem(PgBase_Item const &rkItem)
	{
		Init();
		kItem = rkItem;
	}

	void Init()
	{
		kPos.Clear();
		kItem.Clear();
	}

	bool ReadFromPacket(BM::Stream &kPacket)
	{
		if(false==kPacket.Pop(kPos)) {return false;}
		return kItem.ReadFromPacket(kPacket);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kPos);
		kItem.WriteToPacket(kPacket);
	}
}SExchangeItem;

typedef std::vector< SExchangeItem > CONT_TRADE_ITEM;

typedef struct tagTradeInfo
{
	BM::GUID			kTradeGuid;		//교환용 GUID
	BM::GUID			kCasterGuid;		//교환 요청자
	BM::GUID			kTargetGuid;		//교환 대상자
	BM::PgPackedTime	kTime;			//교환 시작 시간
	bool				bCasterConfirm;	//고정
	bool				bTargetConfirm;
	bool				bCasterOk;		//확인
	bool				bTargetOk;
	CONT_TRADE_ITEM		kCasterItemCont;	//요청자 아이템 목록
	CONT_TRADE_ITEM		kTargetItemCont;	//대상자 아이템 목록
	ETRADERESULT		kResult;		//성공 실패 여부
	__int64				i64CasterMoney;	
	__int64				i64TargetMoney;	

	tagTradeInfo()
	{
		Init();
	}

	bool Create(BM::GUID const kFrom, BM::GUID const kTo)
	{
		if (BM::GUID::NullData()==kFrom || BM::GUID::NullData() == kTo)
		{
			return false;
		}
		kTradeGuid.Generate();
		kTime = BM::PgPackedTime::LocalTime();
		kCasterGuid = kFrom;
		kTargetGuid = kTo;
		return true;
	}

	void Init()
	{
		kTradeGuid.Clear();
		kCasterGuid.Clear();
		kTargetGuid.Clear();
		kTime.Clear();
		bCasterConfirm = false;
		bTargetConfirm = false;
		bCasterOk = false;
		bTargetOk = false;
		kCasterItemCont.clear();
		kTargetItemCont.clear();
		kResult = TR_NONE;
		i64CasterMoney = 0i64;
		i64TargetMoney = 0i64;

	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kTradeGuid);
		kPacket.Pop(kCasterGuid);
		kPacket.Pop(kTargetGuid);
		kPacket.Pop(kTime);
		kPacket.Pop(bCasterConfirm);
		kPacket.Pop(bTargetConfirm);
		kPacket.Pop(bCasterOk);
		kPacket.Pop(bTargetOk);
		PU::TLoadArray_M(kPacket, kCasterItemCont);
		PU::TLoadArray_M(kPacket, kTargetItemCont);
		kPacket.Pop(kResult);
		kPacket.Pop(i64CasterMoney);
		kPacket.Pop(i64TargetMoney);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kTradeGuid);		
		kPacket.Push(kCasterGuid);		
		kPacket.Push(kTargetGuid);	
		kPacket.Push(kTime);			
		kPacket.Push(bCasterConfirm);
		kPacket.Push(bTargetConfirm);
		kPacket.Push(bCasterOk);		
		kPacket.Push(bTargetOk);
		PU::TWriteArray_M(kPacket, kCasterItemCont);
		PU::TWriteArray_M(kPacket, kTargetItemCont);
		kPacket.Push(kResult);		
		kPacket.Push(i64CasterMoney);	
		kPacket.Push(i64TargetMoney);
	}
}STradeInfo;


typedef struct tagTradeInfoPacket
{
	tagTradeInfoPacket()
	{
		kMoney = 0;
	}

	BM::GUID			kTradeGuid;		//교환용 GUID
	BM::GUID			kOwnerGuid;		//이 아이템들이 누구꺼냐?
	CONT_TRADE_ITEM		kItemCont;		//아이템 목록
	__int64				kMoney;

	bool ReadFromPacket(BM::Stream &kPacket)
	{
		if(false==kPacket.Pop(kTradeGuid)) {return false;}
		if(false==kPacket.Pop(kOwnerGuid)) {return false;}
		PU::TLoadArray_M(kPacket, kItemCont);
		return kPacket.Pop(kMoney);
	}

	void WriteToPacket(BM::Stream &kPacket)
	{
		kPacket.Push(kTradeGuid);
		kPacket.Push(kOwnerGuid);
		PU::TWriteArray_M(kPacket,kItemCont);
		kPacket.Push(kMoney);
	}
}STradeInfoPacket;

typedef struct tagItemData
{
	tagItemData()
		: iCount(0)
	{
		kContItemPos.clear();
	}
	~tagItemData()
	{
		kContItemPos.clear();
	}

	size_t iCount;
	std::vector<SItemPos> kContItemPos;
}SItemData;

#pragma pack()

typedef std::set< int > CONT_HAVE_ITEM_NO;//갖고 있는 아이템 번호로
typedef std::map< int, PgBase_Item > CONT_HAVE_ITEM_DATA;//갖고 있는 아이템//겹칠경우. 쌩까임. 셋트아이템 체크용 itemno, 
typedef std::vector<PgBase_Item > CONT_HAVE_ITEM_DATA_ALL;//갖고 있는 아이템//겹치는 것까지 모두 가져옴
typedef std::map< int, size_t > ContHaveItemNoCount;//아이템 번호 + 수량
typedef std::map< int, SItemData > CONT_HAVE_ITEM_POS;//아이템 번호 + (수량, 위치들)

typedef std::map< tagTBL_KEY<int>, PgBase_Item > CONT_RESULT_ITEM_DATA;//생성 결과용 해쉬

typedef std::map<BM::GUID,PgItemWrapper > CONT_SELECTED_CHARACTER_ITEM_LIST;
typedef std::map<BM::GUID,PgItemWrapper > CONT_GUILD_ITEM_LIST; // 길드금고 아이템 리스트

__int64 const MAX_CASH_ITEM_TIMELIMIT = 511i64; // 기간제 아이템 최대 기간

int const MAX_ITEM_DURATION = 8000;	// 아이템 최대 내구도는 8000을 넘지 못한다.
int const MAX_BASKET_SLOT = 36; //장바구니

int const ENCHANT_SHIFT_MIN_LEVEL = 0;
int const ENCHANT_SHIFT_MAX_LEVEL = 10;
int const ENCHANT_SHIFT_MIN_PLUS_LEVEL = 5;

typedef std::set< int > CONT_QUEST_SET;
typedef std::map< int, CONT_QUEST_SET> CONT_QUEST_ITEM;

static SItemPos const g_RingRItemEquipPos(IT_FIT_CASH, EQUIP_POS_RING_R);
static SItemPos const g_RingLItemEquipPos(IT_FIT_CASH, EQUIP_POS_RING_L);

#endif // WEAPON_VARIANT_ITEM_ITEM_H