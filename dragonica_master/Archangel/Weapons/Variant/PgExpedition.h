#ifndef WEAPON_VARIANT_PARTY_PGEXPEDITION_H
#define WEAPON_VARIANT_PARTY_PGEXPEDITION_H

#include "Lohengrin/packetstruct.h"
#include "PgParty.h"
#include "pgpartymgr.h"

typedef enum eExpeditionValue
{
	EV_MAX_PARTY_CNT	= 2,		// 원정대 최대 파티수
	EV_MAX_MEMBER_CNT	= 8,		// 원정대 최대 멤버수
	EV_MAX_EXPEDITION_VIEW_LIST_CNT = 8,	// 원정대 찾기 보여지는 갯수
	EV_MAX_USER_VIEW_LIST_CNT = 50,	// 유저 검색 보여지는 갯수
}EExpeditionValue;

typedef enum eExpeditionOptionExp	// 원정대 경험치 옵션
{
	EOE_None		= 0,
	EOE_SafeMask	= 0xFFFF0000,	//항상 보존할 비트 들
	EOE_LikeSolo	= 0x00000001,	//솔플 처럼
	EOE_CalcShare	= 0x00000003,	//공평하게 수식으로 나눠서
} EExpeditionOptionExp;

typedef enum eExpeditionOptionItem	// 원정대 아이템 획득 방식 옵션
{
	EOI_None		= 0,
	EOI_SafeMask	= 0xFF00FFFF,	//항상 보존할 비트 들
	EOI_LikeSolo	= 0x00010000,	//아무나 먹는사람이
	EOI_Order		= 0x00030000,	//순차적으로 습득
} EExpeditionOptionItem;

typedef enum eExpeditionOptionPublicTitle // 원정대 공개/비공개 옵션
{
	EOT_None		= 0,
	EOT_SafeMask	= 0x00FFFFFF,	//항상 보존할 비트 들
	EOT_Public		= 0x01000000,	//공개
	EOT_Private		= 0x03000000,	//비공개
} EExpeditionOptionPublicTitle;

#define SetExpeditionBit(A, Bit, SafeBit) ((A&Bit)|Bit) | (A&SafeBit)

typedef enum eExpedtionState	// 원정대 상태
{
	ES_RECRUITING	= 0,	// 원정대 모집 중.
	ES_PROGRESSING	= 1,	// 원정대 컨텐츠 진행 중.
	ES_DISPERSE		= 2,	// 원정대 해체 됨.
} EExpeditionState;

typedef enum eExpeditionTeam
{
	EET_TEAM_0		= 0,		// 에러
	EET_TEAM_1		= 1,		// 1번팀
	EET_TEAM_2		= 2,		// 2번팀
	EET_TEAM_3		= 3,		// 3번팀
	EET_TEAM_4		= 4,		// 4번팀
	EET_TEAM_SHIFT	= 5,		// 교체 대기
	EET_END,
} EExpeditionTeam;

typedef enum eExpeditionJoinType	// 원정대 가입 요청하는 방법.
{
	EJT_DEFAULT		= 0,			// 일반적인 방법(원정대 모집 패널 눌러서).
	EJT_CHANMOVE	= 1,			// 다른 채널에서 원정대 리스트 보고 가입 요청하는 경우.
	EJT_END			= 2,
} EExpeditionJoinType;

typedef struct tagExpeditionOption	// 원정대 옵션
{
	tagExpeditionOption();
	tagExpeditionOption(const EExpeditionOptionExp eOptionExp, const EExpeditionOptionItem eOptionItem, const EExpeditionOptionPublicTitle eOptionPublicTitle);
	tagExpeditionOption(tagExpeditionOption const & rkInfo);
	void operator = (tagExpeditionOption const & rkInfo);
	void Clear();
	void Default();

	void SetOptionExp(const EExpeditionOptionExp eOptionExp);
	void SetOptionItem(const EExpeditionOptionItem eOptionItem);	
	void SetOptionPublicTitle(const EExpeditionOptionPublicTitle eOptionPublicTitle);
	void SetOptionLevel(int const iLevel);
	void SetOptionOption(int const iOption);
	void SetOptionArea_NameNo(int const iAreaName_No);
	void SetOptionContinent(int const iContinent);
	void SetOptionState(bool const bState);

	EExpeditionOptionExp GetOptionExp() const;
	EExpeditionOptionItem GetOptionItem() const;
	EExpeditionOptionPublicTitle GetOptionPublicTitle() const;
	int GetOptionLevel() const;
	int GetArea_NameNo() const;
	int GetContinent() const;
	bool GetState() const;

	void WriteToPacket(BM::Stream& rkPacket) const;

	bool ReadFromPacket(BM::Stream& rkPacket);

	int iExpeditionOption;	// 아이템 획득 방식, 공개/비공개
	int iExpeditionLevel;	// 원정대 참여 제한 레벨
	int iExpeditionArea_NameNo; // 원정대 지역 이름 번호.
	int iExpeditionContinent;	// 원정대 지역 대륙 번호
	bool bExpeditionState;	// 가입 가능 상태
} SExpeditionOption;

////////////////////////////////////////////////////////////////////
// Expedition 원정대

typedef std::vector<SExpeditionUserInfo> VEC_EPUSERINFO;
typedef std::vector<SPartyUserInfo> VEC_CHANGEPOS_USER;
typedef std::map<BM::GUID, SExpeditionUserInfo> ContExpeditionUserInfo;
typedef std::map<BM::GUID, DWORD> ContWaitJoinList; // 원정대 가입 대기자 목록.

template< class _T_Party >
class PgExpedition
{
	typedef std::vector<BM::GUID> ContParty;

public:

	PgExpedition();
	virtual ~PgExpedition();

	virtual void Init(BM::GUID const & rkNewGuid, std::wstring const & rkNewName, SExpeditionOption const& rkOption);
	virtual void Clear();

	virtual HRESULT Add(SContentsUser const & rkNewUserInfo, _T_Party * & OutParty, int & OutTeamNo);
	virtual HRESULT Add(SPartyUserInfo const & UserInfo, _T_Party * pParty);
	virtual HRESULT Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster = BM::GUID::NullData());

	virtual bool AddParty(_T_Party * pkParty);
	virtual bool DelParty(_T_Party * pkParty);

	virtual size_t GetExpeditionPartyCount() const;
	virtual size_t GetExpeditionMemberCount() const;
	size_t GetMaxMemberCount() { return MaxMemberCount(); }

	virtual bool IsMaster(BM::GUID const & rkCharGuid) const;
	virtual bool IsMember(BM::GUID const & rkCharGuid) const;

	virtual bool ChangeMaster(BM::GUID const & NewMaster);

	virtual void GetPartyList(VEC_GUID & rkOutParty) const;

	virtual bool IsPositionWait(BM::GUID const & CharGuid);
	virtual bool AddPositionWait(SPartyUserInfo const & UserInfo);
	virtual void GetPositionWaitList(VEC_CHANGEPOS_USER& rkOutList) const;
	virtual bool DelPositionWaitList(BM::GUID const & CharGuid);

	virtual void WriteToPacket(BM::Stream & rkPacket) const;					// 원정대속해있는 모든 파티(원)정보.
	virtual void ReadFromPacket(BM::Stream & rkPacket);

	virtual void WriteToPacketMemberInfo(BM::Stream & rkPacket) const;			// 원정대원 멤버 정보만.
	virtual void ReadFromPacketMemberInfo(BM::Stream & rkPacket);

	virtual bool AddExpeditionMemberInfo(SContentsUser const & rkUserInfo, SExpeditionUserInfo & OutInfo, unsigned short const sTeam, bool bAlive);
	virtual bool AddExpeditionMemberInfo(SPartyUserInfo* pkPartyUserInfo, SExpeditionUserInfo & OutInfo, unsigned short const sTeam, bool bAlive);
	virtual bool DelExpeditionMemberInfo(BM::GUID const & rkCharGuid);

	virtual void GetMemberCharGuidList(VEC_GUID & OutCharGuid, BM::GUID const & IgnoreGuid = BM::GUID::NullData() ) const;
	virtual bool GetMemberInfo(BM::GUID const & CharGuid, SPartyUserInfo& OutMember) const;
	virtual bool GetMemberInfo(BM::GUID const & CharGuid, SExpeditionUserInfo & OutMember) const;

	bool GetRand(SPartyUserInfo & UserInfo);

	virtual bool DisperseExpedition();

	_T_Party* Find(BM::GUID const & PartyGuid) const;

	void SetPartyHash(typename PgPartyMgr<_T_Party>::PartyHash * pPartyHash) { m_pPartyHash = pPartyHash; }
	_T_Party * GetParty(BM::GUID const & PartyGuid) const;

	CLASS_DECLARATION_S(BM::GUID, ExpeditionGuid);
	CLASS_DECLARATION_S(std::wstring, ExpeditionName);
	CLASS_DECLARATION_S(BM::GUID, MasterCharGuid);
	CLASS_DECLARATION_S(SExpeditionOption, Option);
	CLASS_DECLARATION_S(EExpeditionState, State);		// 원정대 상태.
	CLASS_DECLARATION_S(int, MaxMemberCount);
	CLASS_DECLARATION_S(int, MaxTeamCount);
	CLASS_DECLARATION_S(BM::GUID, ExpeditionNpc);	// 원정대 가입 신청을 받는 NPC GUID.

protected:

	mutable Loki::Mutex m_kMutex;

	ContParty			m_kPartyList;			// 원정대 소속 파티
	VEC_CHANGEPOS_USER	m_kPositionWaitList;	// 원정대 파티 교체 대기자(파티 정보 벡터)

	ContExpeditionUserInfo m_kUserInfo;			// 원정대원들의 간단한 정보.
	
	typename PgPartyMgr<_T_Party>::PartyHash * m_pPartyHash;			// 파티 매니저가 가지고 있는 파티 Hash 포인터..

};

////////////////////////////////////////////////////////////////////////////////////
// Global Expedition.

class PgGlobalExpedition :
	public PgExpedition<PgGlobalParty>
{

public:

	PgGlobalExpedition();
	virtual ~PgGlobalExpedition();

	virtual HRESULT AddWaitMember(BM::GUID const & rkCharGuid);
	virtual HRESULT DelWaitMember(BM::GUID const & rkCharGuid);

	virtual HRESULT Add(SContentsUser const & rkNewUserInfo, PgGlobalParty * & OutParty, int & OutTeamNo);
	virtual HRESULT Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster = BM::GUID::NullData());

	HRESULT AddToTeam(SPartyUserInfo const & UserInfo, int iTeam, PgGlobalParty * & pOutParty);

	bool IsWaitMember(BM::GUID const & rkCharGuid);
	bool CheckMovable(int iTeam);		// 팀이동이 가능한지 확인(인원수).

	void ModifyMemberState(SExpeditionUserInfo & MemberInfo);
	void GetJoinWaitList(ContWaitJoinList& rkOutList) const;

	void WrtieToPacketListInfo(BM::Stream & Packet);

	void ClearExpeditionJoinWaitMember(DWORD Time);	// 가입 신청 수락 이후 일정시간 동안 가입하지 않는 유저를 가입대기리스트에서 삭제.

protected:

	ContWaitJoinList	m_kJoinWaitList;		// 원정대 가입 대기자(CharGuid)

};

////////////////////////////////////////////////////////////////////////////////////
// Map Server에서 사용. Local Expedition.

typedef std::map<BM::GUID, bool> ContHaveKeyItemList; // 원정대 입장 아이템 보유 상태 컨테이너(원정대 생성할 때만 사용).

class PgLocalExpedition :
	public PgExpedition<PgLocalParty>
{

public:

	PgLocalExpedition();
	virtual ~PgLocalExpedition();

	virtual HRESULT Add(SContentsUser const & rkNewUserInfo, PgLocalParty * & OutParty);
	virtual HRESULT Del(BM::GUID const & rkCharGuid, BM::GUID const & rkNewMaster = BM::GUID::NullData() );

	virtual void Init(BM::GUID const & rkNewGuid, std::wstring const & rkNewName, SExpeditionOption const & rkOption);
	virtual void Clear();

	HRESULT IsStartable();
	BM::GUID const & GetExpeditionNpcGuid();

	bool GetLocalMemberList( SChnGroundKey const & GndKey, VEC_GUID & OutVec, BM::GUID const & IgnoreGuid = BM::GUID::NullData() ) const; //임의의 그라운드에 속한 유저리스트만 뽑아냄

	bool CleanUpPartyList(PgLocalParty * pParty);
	bool DeadExpeditionMember(BM::GUID const & CharGuid);

	int GetMemberTeamNo(BM::GUID const & CharGuid);

	void WrtieToPacketListInfo(BM::Stream & Packet);
};

#endif // WEAPON_VARIANT_PARTY_PGEXPEDITION_H