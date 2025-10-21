#ifndef WEAPON_VARIANT_BASIC_GROUND_PGPORTALACCESS_H
#define WEAPON_VARIANT_BASIC_GROUND_PGPORTALACCESS_H

#include "Item.h"

int const MAX_DUNGEON_MODE = 3;

typedef enum
{
	E_MOVE_LOCK				= 0x00,
	E_MOVE_PERSONAL			= 0x01,
	E_MOVE_PARTYMASTER		= 0x02,
	E_MOVE_ANY_PARTYMEMMBER	= 0x04,
	E_MOVE_EXPEDITIONMASTER = 0x08,
}EPartyMoveType;

typedef enum eGateWayQuestType
{
	GWQT_Ing	=	0x01,	// 진행중일 때(실패 포함 안함)
	GWQT_IngAny	=	0x02,	// 진행중일 때(실패 포함)
	GWQT_Ended	=	0x04,	// 퀘스트를 완료 했을 때
	GWQT_Test_Flag	= GWQT_Ing| GWQT_Ended,	// 설정 가능 플래그 조합값
}EGateWayQuestType;

class PgPlayer;
struct SPortalAccessInfo;

class PgMapMoveChecker
{
public:
	explicit PgMapMoveChecker( CONT_DEFMAP const &kContDetMap );
	~PgMapMoveChecker(){}

	EPartyMoveType GetMoveType( int const iGroundNo )const;

private:
	CONT_DEFMAP	const &m_kContDefMap;

private:
	PgMapMoveChecker();

};

#pragma pack(1)
class PgPortalAccess
{
public:
	typedef struct tagKey_Base
	{
		tagKey_Base()
			:	iGroup(0)
		{}

		int iGroup;
	}SKey_Base;

	typedef struct tagKey
		:	public tagKey_Base
	{
		tagKey()
			:	iItemNo(0)
			,	iItemCount(1)
			,	kInvType(IT_ETC)
			,	bConsume(true)
			,	bParty(false)
		{
		}

		bool IsEmpty()const
		{
			return !iItemNo || !iItemCount;
		}

		int			iItemNo;
		size_t		iItemCount;
		EInvType	kInvType;
		bool		bConsume;//소모여부
		bool		bParty;
	}SKey;

	typedef struct tagQuestKey
		:	public tagKey_Base
	{
		tagQuestKey()
			:	sQuestNo(0)
			,	byState(0)
			,	bParty(false)
			,	i64ClassFlag(0i64)
		{
		}

		bool IsEmpty()const
		{
			return !sQuestNo;
		}

		bool CheckClass(int const iClassNo) const
		{
			__int64 const iRet = (i64ClassFlag&GET_CLASS_LIMIT(iClassNo));
			return (0 != iRet);
		}

		short int	sQuestNo;
		BYTE		byState;
		bool		bParty;
		__int64		i64ClassFlag;
	}SQKey;

	typedef struct tagTimeKey
	{
		typedef enum
		{
			TIME_IN,
			TIME_YET,
			TIME_OVER,
		}E_TIME_RET;

		tagTimeKey( WORD const _wMin_Open=0, WORD const _wMin_Close=0 )
			:	wMin_Open(_wMin_Open)
			,	wMin_Close(_wMin_Close)
		{}

		bool operator < ( tagTimeKey const &rhs )const
		{
			return wMin_Open < rhs.wMin_Open;
		}

		int Check( WORD const wMin )const
		{
			if ( wMin_Open > wMin )
			{
				return TIME_YET;
			}
			if ( wMin < wMin_Close )
			{
				return TIME_IN;
			}
			return TIME_OVER;
		}

		bool operator > ( tagTimeKey const &rhs ){return rhs < *this;}
		bool operator ==( tagTimeKey const &rhs ){return !(rhs < *this) && !(*this < rhs); }

		WORD wMin_Open;
		WORD wMin_Close;
	}STimeKey;

	// 퀘스트 존재할 때 이동할 포탈 정보
	typedef struct tagQuestPortal
	{
		tagQuestPortal()
		{
			iQuestID = 0;
			kTargetGroundKey.Clear();
			sTargetPortal = 0;
		}

		bool operator < (tagQuestPortal const &rhs) const
		{	
			if(iQuestID < rhs.iQuestID){ return true; }
			if(rhs.iQuestID < iQuestID){ return false; }

			if(kTargetGroundKey < rhs.kTargetGroundKey){ return true; }
			if(rhs.kTargetGroundKey < kTargetGroundKey){ return false; }
			
			return false;
		}

		int iQuestID;
		SGroundKey kTargetGroundKey;
		short sTargetPortal;
	}SQuestPortal;

	typedef enum
	{
		EMT_DEFAULT		= 0,
		EMT_HAVE_EFFECT,
		EMT_NOT_HAVE_EFFECT,
		EMT_MAX
	} E_MSG_TYPE;

	typedef enum
	{
		ERR_NONE		= 0,
		ERR_FAILED		= 810,
		ERR_MOVE_LOCK	= 811,
		ERR_LV_MIN		= 812,
		ERR_LV_MAX		= 813,
		ERR_TIME		= 814,
		ERR_QUEST		= 815,
		ERR_ITEM		= 816,
		ERR_EFFECT		= 817,
	} E_ERR;

	typedef std::set<int>			ConKey_Base;
	typedef std::vector<SKey>		ConKey;
	typedef std::vector<SQKey>		ConQKey;
	typedef std::vector<STimeKey>	ContTimeKey;
	typedef std::set<SQuestPortal> CONT_QUEST_PORTAL;

	typedef struct tagEffect
	{
		tagEffect() : bParty(false) {}
		SET_INT kCont;
		bool bParty;
	}SEffect;
	typedef std::pair<SEffect,bool>		SEffectInfo;	//first: SEffect, second: bHave
	typedef std::vector<SEffectInfo>	CONT_EFFECT_OR;
	typedef std::vector<CONT_EFFECT_OR>	CONT_EFFECT;

public:
	PgPortalAccess(void);
	~PgPortalAccess(void);// 상속금지?

	bool Build( TiXmlElement const *pkElement, CONT_DEFMAP const &kContDerMap, CONT_DEF_QUEST_REWARD const& kContDefQuest );

	int  GetChannelNo() const { return m_iChannelNo; }
	bool IsBossPortal() const { return m_bIsBossPortal; }
	BYTE GetMoveType()const{return m_byMoveType;}
	bool IsAccess( PgPlayer *pkPlayer, bool bIsPartyMaster, CONT_PLAYER_MODIFY_ORDER *pContOrder )const;
	void IsAccessInfo( PgPlayer * pkPlayer, bool const bIsPartyMaster, SPortalAccessInfo & rkOut )const;
	SGroundKey GetGroundKey() const { return m_kTargetGroundKey; }

	void Get( SReqMapMove_MT &rkRMM )const;

	int GetName()const{return m_iName;}
	int GetErrorMessage(E_MSG_TYPE const eType = EMT_DEFAULT)const{return m_iErrorMessage[eType];}	
	void SetQuestPortal( PgPlayer* pkPlayer );
	
	void SetModeNo(int const ModeNo);
	void SetTargetGroundKey(int const ModeNo);

	void SetMoveType(BYTE const byType) { m_byMoveType = byType; }
	bool IsChildOfChild() const { return m_bChildOfChild; }
	int GetLastAccessError()const { return static_cast<int>(m_eLastAccessErr); }

private:
	SGroundKey		m_kTargetGroundKey;
	short			m_nTargetPortal;
	BYTE			m_byMoveType;

	int				m_iName;

	int				m_iChannelNo;	// 채널이동을 하는 포탈이면 이값을 이용함.

	bool			m_bIsBossPortal;

	// Access
	bool	m_bChildOfChild;
	mutable E_ERR	m_eLastAccessErr;
	int		m_iErrorMessage[EMT_MAX];//이값이 0이면 기본메세지전송
	int		m_iLevelLimit_Min;
	int		m_iLevelLimit_Max;
	bool	m_bLevelParty;
	bool	m_bUseModeLevel;
	int		m_ModeNo;
	int		m_iModeLevelLimit_Min[MAX_DUNGEON_MODE];
	int		m_iModeLevelLimit_Max[MAX_DUNGEON_MODE];
	int		m_iModeGroundNo[MAX_DUNGEON_MODE];
	CONT_EFFECT	m_kContEffect;

	ConKey		m_kConKey;
	ConQKey		m_kConQuestKey;
	ContTimeKey	m_kContTimekey;

	ConKey_Base	m_kConKeyGroup;
	ConKey_Base	m_kConQuestKeyGroup;

	CONT_QUEST_PORTAL m_kContQuestPortal;
};
typedef std::vector<PgPortalAccess>		CONT_PORTAL_ACCESS;

#pragma pack()

#endif // WEAPON_VARIANT_BASIC_GROUND_PGPORTALACCESS_H