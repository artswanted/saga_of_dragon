#ifndef CENTER_CENTERSERVER_CONTENTS_PGINSTANCEDUNGEONPLAYINFO_H
#define CENTER_CENTERSERVER_CONTENTS_PGINSTANCEDUNGEONPLAYINFO_H

#include "Lohengrin/PacketStruct4Map.h"
#include "Variant/PgMonster.h"

typedef struct tagIndunPlayerInfo
{
	static size_t const ms_unSelectBoxNum = 0xFFFFFFFF;

	tagIndunPlayerInfo( BM::GUID const &kMemGuid=BM::GUID::NullData(), __int64 const iExp=0, TCHAR const* szName=NULL )
		:	kMemberGuid(kMemGuid)
		,	iNowExp(iExp)
		,	uiSelectBoxNum(ms_unSelectBoxNum)
		,	fParticipationRate(0)
	{
		if ( szName )
		{
			wstrName = szName;
		}
	}

	bool operator==( const tagIndunPlayerInfo& rkPair )
	{
		return kMemberGuid == rkPair.kMemberGuid;
	}

	tagIndunPlayerInfo( const tagIndunPlayerInfo& rkPair )
	{
		kMemberGuid = rkPair.kMemberGuid;
		iNowExp = rkPair.iNowExp;
		uiSelectBoxNum = rkPair.uiSelectBoxNum;
		wstrName = rkPair.wstrName;
		kResultItemList = rkPair.kResultItemList;
		fParticipationRate = rkPair.fParticipationRate;
//		kBonus = rkPair.kBonus;
	}

	tagIndunPlayerInfo &operator=( const tagIndunPlayerInfo& rkPair )
	{
		kMemberGuid = rkPair.kMemberGuid;
		iNowExp = rkPair.iNowExp;
		uiSelectBoxNum = rkPair.uiSelectBoxNum;
		wstrName = rkPair.wstrName;
		kResultItemList = rkPair.kResultItemList;
		fParticipationRate = rkPair.fParticipationRate;
//		kBonus = rkPair.kBonus;
		return *this;
	}

	BM::GUID			kMemberGuid;
	__int64				iNowExp;
	size_t				uiSelectBoxNum;
	std::wstring		wstrName;
	CONT_ITEM_CREATE_ORDER kResultItemList;
	float				fParticipationRate;
//	SConResultBonus		kBonus;

} SINDUNPLAYINFO;

class PgInstanceDungeonPlayInfo
{
public:
	static size_t const ms_iRESULTSELECT_MAXCOUNT = 4;
	static size_t const ms_iRESULTITEM_MAXCOUNT = 4;
	static DWORD const	ms_dwRESULTSELECT_WAIT_TIME	= 28000;//19000;

	PgInstanceDungeonPlayInfo();
	virtual ~PgInstanceDungeonPlayInfo();

	void Init( SGroundKey const rkKey );
	SGroundKey const& GetKey()const{	return m_kKey;	}
	void Clear();

	bool Join(const SNfyJoinIndun& kJoinInfo );
	void Leave( BM::GUID const &kCharGuid );
	DWORD GetTime()	{	return m_dwTime;	}

	bool SetSelectBox( size_t iSelect, BM::GUID const &kCharGuid );
	bool ResultStart( VEC_RESULTINDUN const& rkResultList );
	void ResultEnd();
	void BroadCast( BM::Stream const& rkPacket );
	bool SetResultItem( const SNfyResultItemList& rkResultItem );

	bool IsStart()const{return m_bResultStart;}
//	bool SetResultBonus( SConResultBonus& rkBonus );

public:
	typedef std::map< BM::GUID, SINDUNPLAYINFO>		CONT_PLAYER;	// first key : Character GUID, second key : Info
	typedef std::vector< BM::GUID >					CONT_SELECT;

protected:
	Loki::Mutex			m_kMutex;

	DWORD				m_dwTime;
	SGroundKey			m_kKey;
	CONT_PLAYER			m_kPlayerInfo;
	bool				m_bRecvResultItem;
	bool				m_bResultStart;		// 결과 시작이 되었느냐?

	CONT_SELECT			m_kSelectBox;
	int					m_iMaxItemCount;	// 최대 아이템 갯수
	
};

////////////////////////////////////////////////////////////////////////////////////
// 원정대용.

const int DefaultTenderCount = 1;

/////////////////////////////////////////////
// 각 상태별 대기 시간.

DWORD const EXPEDITION_BOSSDOWN		= 3000;		// 처음 한번 만.
DWORD const EXPEDITION_START		= 5000;		// 결산 UI 나오는 시간.
//DWORD const EXPEDITION_TENDER_WAIT= 3000;		// 특별 보상 아이템 나오는 시간.
DWORD const EXPEDITION_TENDER		= 15000;	// 입찰 가능 시간.
DWORD const EXPEDITION_TENDER_END	= 8000;		// 입찰 끝.

/////////////////////////////////////////////

typedef enum eResultState
{
	ERS_NONE		= 0,
	ERS_1ST			= 1,		// 보스가 죽고, 결산 시작하기 전.
	ERS_WAIT		= 2,		// 결산 UI 대기.
	ERS_START		= 3,		// 특별 보상 입찰 시작 대기. 3초 기다림.
	ERS_TENDER		= 4,		// 입찰 중. 10초 기다림. 10초 내에 모두 입찰,포기 하면 바로 다음 상태로.
	ERS_TENDEREND	= 5,		// 입찰 끝. 5초 기다림.
	ERS_RESULTEND	= 6,		// 결과 알림. 다음 진행 상황 체크.
	ERS_END			= 7,
} ERESULTSTATE;

typedef struct tagExpeditionIndunPlayInfo
{
	tagExpeditionIndunPlayInfo( 
		BM::GUID const & MemGuid = BM::GUID::NullData(), 
		BM::GUID const & Char = BM::GUID::NullData(), 
		__int64 Exp = 0, TCHAR const * szName = NULL, size_t const Tenderable = 0 )
		:	MemberGuid(MemGuid)
		,	CharGuid(Char)
		,	NowExp(Exp)
		,	TenderableCount(Tenderable)
	{
		if ( szName )
		{
			wstrName = szName;
		}
	}

	bool operator ==(const tagExpeditionIndunPlayInfo & rhs)
	{
		return MemberGuid == rhs.MemberGuid;
	}

	tagExpeditionIndunPlayInfo(const tagExpeditionIndunPlayInfo & rhs)
	{
		MemberGuid = rhs.MemberGuid;
		CharGuid = rhs.CharGuid;
		wstrName = rhs.wstrName;
		NowExp = rhs.NowExp;
		ResultItemList = rhs.ResultItemList;
		TenderableCount = rhs.TenderableCount;
	}

	tagExpeditionIndunPlayInfo & operator=(const tagExpeditionIndunPlayInfo & rhs)
	{
		MemberGuid = rhs.MemberGuid;
		CharGuid = rhs.CharGuid;
		wstrName = rhs.wstrName;
		NowExp = rhs.NowExp;
		ResultItemList = rhs.ResultItemList;
		TenderableCount = rhs.TenderableCount;

		return *this;
	}

	BM::GUID			MemberGuid;
	BM::GUID			CharGuid;
	std::wstring		wstrName;
	__int64				NowExp;
	CONT_ITEM_CREATE_ORDER ResultItemList;
	size_t TenderableCount; // 입찰 가능 회수.
}SEXPEDITIONINDUNPLAYINFO;

class PgExpeditionIndunPlayInfo
{
	
public:

	PgExpeditionIndunPlayInfo();
	virtual ~PgExpeditionIndunPlayInfo();

	void Init(SGroundKey const Key);
	SGroundKey const & GetKey() const { return m_Key; }
	void Clear();

	bool Join(SNfyJoinIndun const & JoinInfo);
	void Leave(BM::GUID const & CharGuid);
	DWORD GetTime() { return m_Time; }

	bool ResultStart(VEC_RESULTINDUN const & ResultList);
	void ResultEnd();

	void BroadCast(BM::Stream const & Packet);
	bool SetExpeditionResultItem(BM::Stream & Pakcet);

	bool Tender(BM::GUID const & CharGuid);			// 입찰.
	bool GiveupTender(BM::GUID const & CharGuid);	// 입찰 포기.

	bool IsStart() const { return m_ResultStart; }

	void OnTick(DWORD CurTime);
	void SetState(ERESULTSTATE State);

public:

	typedef std::map<BM::GUID, SEXPEDITIONINDUNPLAYINFO> CONT_EXPEDITION_PLAYER;

protected:

	Loki::Mutex						m_Mutex;

	DWORD							m_Time;
	SGroundKey						m_Key;
	CONT_EXPEDITION_PLAYER			m_PlayerInfo;
	bool							m_RecvResultItem;
	bool							m_ResultStart;

	CONT_SPECIALITEM				m_SpecialItem;		// 특별보상.

	VEC_GUID						m_TenderCharList;	// 입찰자 리스트.
	VEC_GUID						m_TenderGiveUpList;	// 입찰 포기 리스트.

	int								m_MaxItemCount;
	int								m_MaxSpecialItemCount;

	ERESULTSTATE					m_State;	// 결과 진행 상태.

	int								m_BestTeamNo;
	int								m_BestTeamDP;
	int								m_BestPlayerDP;
	int								m_BestSupporterHP;
	BM::GUID						m_BestPlayer;
	BM::GUID						m_BestSupporter;

	bool							m_Tender;

};

#endif // CENTER_CENTERSERVER_CONTENTS_PGINSTANCEDUNGEONPLAYINFO_H