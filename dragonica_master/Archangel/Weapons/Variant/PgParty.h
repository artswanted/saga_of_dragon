#ifndef WEAPON_VARIANT_PARTY_PGPARTY_H
#define WEAPON_VARIANT_PARTY_PGPARTY_H

#include "Lohengrin/packetstruct.h"

typedef enum ePartyValue
{
	PV_MAX_MEMBER_CNT		= DEF_PV_MAX_MEMBER_CNT, //12,
	PV_MAX_LIST_CNT			= 30, // 파티원 찾기
	PV_MAX_PARTY_LIST_CNT	= 100,// 파티 찾기 목록
	PV_MAX_PARTY_VIEW_LIST_CNT	= 9,// 파티 찾기 보여지는거 목록
	PV_ENTER_INDUN_EXPIRE_TIME = 5000,// 파티가 인던에 들어가기 전 대기 시간(n / 1000 초)
	PV_MAX_MULTI_MEMBER_CNT	= 4,	// 4 파티
}EPartyValue;

typedef enum
{
	PARTY_SYS_DEFAULT			= 0,	// 기본 파티 시스템
	PARTY_SYS_DELETE			= 1,
	PARTY_SYS_PVP				= 2,	// PvP에서 맺어지는 특수한 시스템.
	PARTY_SYS_EXPEDITION		= 3,	// 원정대
	PARTY_SYS_EXPEDITION_DELTE	= 4,
}EPartySystemType;

typedef enum ePartyReturnCode// 파티를 생성하거나 유저를 가입시키려는 파티장의 파티 생성/가입 결과 - 파티장이 보낸 내용을 서버에서 처리한 결과
{
	PRC_None = 0,			//에러 - 디폴트에러
	PRC_Success = 1,		//성공 - 파티요청 하는 유저가 이미 파티장이다
	PRC_Success_Create,		//성공 - 파티를 생성하고 성공했다.

	PRC_Success_Master,		//마스터 교체
	PRC_Success_Destroy,	//파티원이 나만 남았다 해제
	PRC_Success_Waiter,		//대기자가 나갔다
	PRC_Success_ChangeName,	//파티명 변경
	PRC_Success_PartyInvitePop, // 파티 가입 신청 메시지
	PRC_Success_PartyInvite, //파티 가입신청 하였다.

	PRC_Fail,
	PRC_Fail_Waiter,		// 에러 - 다른 파티가입 대기줄에 있는 유저
	PRC_Fail_Party,			// 에러 - 다른 파티에 가입된 유저
	PRC_Fail_NotMaster,		// 에러 - 파티가입을 시켜주는 유저가 파티장이 아님
	PRC_Fail_MemberCount,	// 에러 - 파티멤버수 제한
	PRC_Fail_Refuse,		// 성공 - 유저로부터 가입거절 응답이 왔다
	PRC_Fail_PVP,			// 에러 - 파티에 가입할 수 없는 상태이다(EX : PVP를 하고 있는 상태)
	PRC_Fail_Me,					
	PRC_Fail_NullMaster,
	PRC_Fail_NullUser,
	PRC_Fail_Duplicate,
	PRC_Fail_NotFind,
	PRC_Fail_NoWaiter,
	PRC_Fail_Destroy,
	PRC_Fail_NoMaster,
	PRC_Fail_MineMember,	//이미 추가된 맴버
	PRC_Fail_NotMember,		//맴버가 아니다
	PRC_Fail_NotInviteMe,	//나를 추가 하면 안된다.
	PMS_Fail_DupReqJoin,	//이미 신청한 상태

	PRC_Fail_ChangeMaster,
	PRC_Fail_Dup_Name,		//같은 파티 이름
	PRC_Fail_Max,			//길이
	PRC_Fail_BadName,		//욕 필터
	PRC_Fail_Level,		//레벨이 맞지 않아 파티 안됨
	PRC_Fail_AnswerWait,	//파티 수락 대기중인 상태
	PRC_Fail_KickUserNoArea,//추방 할수 없는 지역에 있음
	PRC_Fail_Area,			//상대방이 파티를 할 수 없는 지역에 있습니다.
	PCR_Fail_HardCoreVote1,	// 하드코어투표중 초대시
	PCR_Fail_HardCoreVote2,	// 하드코어투표중 가입시
	PCR_Fail_HardCoreVote3,	// 하드코어투표중 기타기능 불가
	PRC_Fail_WaitPlayer,	//원정대 대기자 리스트 있어서 출발 못함.
	PRC_Fail_NoHaveKeyItem, //원정대 입장 아이템이 없어서 출발 못함.
	PRC_Fail_Req_Join,		//파티 가입 요청 취소 됨.
	PRC_NeedItem_PartyBreakIn, // 파티 난입 시 입장아이템이 필요하다.
	PRC_Max,
}EPartyReturnCode;

typedef enum eFindPartyRetrunCode
{
	FPRC_None		= 0,
	FPRC_Success	= 1,
	FPRC_Fail,
	FPRC_Fail_Max,
	FPRC_Fail_BadName,
	FPRC_Max,
}EFindPartyReturnCode;

typedef enum eFindPartyValue
{
	FPV_MAX_CONTENTS_LENGTH = 30,
}EFindPartyValue;

typedef enum ePartyOptionExp//DWORD 비트 플래그
{
	POE_None = 0,
	POE_SafeMask	= 0xFFFF0000,	//항상 보존할 비트 들
	POE_LikeSolo	= 0x00000001,		//솔플 처럼
	POE_CalcShare	= 0x00000003,		//공평하게 수식으로 나눠서
} EPartyOptionExp;
typedef enum ePartyOptionItem
{
	POI_None		= 0,
	POI_SafeMask	= 0xFF00FFFF,	//항상 보존할 비트 들
	POI_LikeSolo	= 0x00010000,		//아무나 먹는사람이
	POI_Order		= 0x00030000,		//순차적으로 습득
} EPartyOptionItem;
typedef enum ePartyOptionPublicTitle
{
	POT_None		= 0,
	POT_SafeMask	= 0x00FFFFFF,	//항상 보존할 비트 들
	POT_Public		= 0x01000000,		//공개
	POT_Private		= 0x03000000,		//비공개
} EPartyOptionPublicTitle;

#define SetPartyBit(A, Bit, SafeBit) ((A&Bit)|Bit) | (A&SafeBit)

typedef enum ePartyOptionAttribute
{
	POA_All		= 0,
	POA_Field	= 1,		// 필드
	POA_Mission	= 2,		// 미션
	POA_Indun	= 3,		// 던젼
	POA_Hidden	= 4,		// 히든
	POA_Chaos	= 5,		// 카오스
	POA_Max		= 6,
} EPartyOptionAttribute;

typedef enum ePartyOptionArea
{
	POC_Area1	= 1,		// 바람대륙
	POC_Area2	= 2,		// 혼돈대륙
	POC_Area3	= 3,		// 고요의 섬
	POC_Max		= 4,
} EPartyOptionContinent;

typedef enum ePartyState : BYTE
{
	EPS_ERROR	= 0,
	EPS_WAITING = 1,
	EPS_RUNNING	= 2,
	EPS_REFUSE	= 3,
	EPS_END,
} EPartyState;

typedef enum ePartyRefuse : BYTE
{
	EPR_NONE		= 0,
	EPR_CLIENT		= 0x01,	//클라설정
	EPR_MAP			= 0x02,	//맵설정
	EPR_BOSSROOM	= 0x04,	//보스방 설정
} EPartyRefuse;

typedef struct tagPartyOption
{
	tagPartyOption();
	tagPartyOption(const EPartyOptionExp eOptionExp, const EPartyOptionItem eOptionItem, const EPartyOptionPublicTitle eOptionPublicTitle);
	tagPartyOption(const tagPartyOption &rkInfo);
	void operator = (const tagPartyOption &rkInfo);
	void Clear();
	void Default();

	void SetOptionExp(const EPartyOptionExp eOptionExp);
	void SetOptionItem(const EPartyOptionItem eOptionItem);	
	void SetOptionPublicTitle(const EPartyOptionPublicTitle eOptionPublicTitle);
	void SetOptionLevel(int const iLevel);
	void SetOptionOption(int const iOption);
	void SetOptionAttribute(const int iAttribute);
	void SetOptionContinent(const int iContinent);
	void SetOptionMaxMember(const int MaxMember);
	void SetOptionArea_NameNo(const int iArea_NameNo);
	void SetOptionState(const BYTE byState);
	void AddOptionState(const BYTE byState);
	void RemoveOptionState(const BYTE byState);

	EPartyOptionExp GetOptionExp() const;
	EPartyOptionItem GetOptionItem() const;
	EPartyOptionPublicTitle GetOptionPublicTitle() const;
	int GetOptionLevel() const;
	std::wstring const& tagPartyOption::GetOptionPartySubName() const;
	EPartyOptionAttribute GetOptionAttribute() const;
	EPartyOptionContinent GetOptionContinent() const;
	int GetOptionMaxMember() const;
	int GetOptionArea_NameNo() const;
	BYTE GetOptionState() const;

	void WriteToPacket(BM::Stream& rkPacket) const;

	bool ReadFromPacket(BM::Stream& rkPacket);
	
	int iPartyOption;
	int iPartyLevel;
	CLASS_DECLARATION_S(std::wstring, PartySubName);
	int iPartyAttribute;
	int iPartyContinent;
	int PartyMaxMember;
	int iPartyArea_NameNo;
	BYTE byPartyState;
} SPartyOption;

class PgPlayer;
class PgParty_Release
{
public:
	static void DoAction( PgPlayer *pkPlayer, bool const bOnlyEffect );
};

class PgPartyContents;

class PgParty
{
	typedef std::map< BM::GUID, SPartyUserInfo > ContPartyMember;
	typedef std::vector<SPartyUserInfo> VEC_UserInfo;

public:
	PgParty();
	virtual ~PgParty();

	virtual void Init(BM::GUID const &rkNewGuid, std::wstring const &rkNewName, SPartyOption const& kNewOption);
	virtual void Clear();

	bool GetMemberInfo(BM::GUID const &rkCharGuid, SPartyUserInfo& rkOutMember) const;
	bool GetMemberChnGndInfo( BM::GUID const &rkCharGuid, SChnGroundKey& rkOutChnGnd ) const;
	bool GetMasterInfo(SPartyUserInfo& rkOutMaster);
	bool IsMaster(BM::GUID const &rkCharGuid) const;
	bool IsMember(BM::GUID const &rkCharGuid) const;
	size_t MemberCount() const;
	size_t MaxMemberCount() const;
	HRESULT IsChangeMaster()const;
	HRESULT IsChangeOption()const;

	void GetMemberCharGuidList(VEC_GUID& rkOutCharGuid, BM::GUID const &rkIgnore = BM::GUID::NullData()) const;// 인자 : 캐릭터GUID - 파티멤버들의 캐릭터GUID를 반환
	void GetMemberGuidList(VEC_GUID& rkOutCharGuid, bool const bIsMemberGuid, short const nChannelNo, BM::GUID const &rkIgnoreCharGuid = BM::GUID::NullData()) const;

	virtual bool MovedGnd(BM::GUID const &rkCharGuid, SChnGroundKey const &rkKey);

	virtual BYTE ChangeAbility(BM::GUID const &rkCharGuid, BYTE const cType, BM::Stream &rkPacket);
	bool WriteToChangedAbil(BM::GUID const &rkCharGuid, BYTE const cChangedAbil, BM::Stream &rkOut);
	bool ChangeMaster(BM::GUID const &rkNewMaster);

	virtual HRESULT Add(SPartyUserInfo const& rkPartyMember, bool const bMaster);
	virtual HRESULT Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster = BM::GUID::NullData());
	bool Get(BM::GUID const &rkCharGuid, SPartyUserInfo* &pkOut);

	void ReadFromPacket(BM::Stream& rkPacket);
	void WriteToPacket(BM::Stream& rkPacket) const;

	HRESULT SetFriendState(BM::GUID const &rkCharGuid, bool bState);
	bool SetFriendHomeAddr(BM::GUID const &rkCharGuid, SHOMEADDR const &rkHomeAddr);
	int GetFriendCount(BM::GUID const &rkCharGuid);
	bool PartyFriendCheck(SPartyUserInfo& kMember);
	void WriteToMemberInfoList(BM::Stream& rkPacket) const;

	bool AttachPartyContents( PgPartyContents * pkPartyContents );
	bool DetachPartyContents();
	PgPartyContents *GetPartyContents(void)const{return m_pkPartyContents;}
	bool GetRand(SPartyUserInfo* &pkOut);

protected:
	bool Find(BM::GUID const &rkCharGuid) const;

	CLASS_DECLARATION_S(BM::GUID, PartyGuid);
	CLASS_DECLARATION_S(std::wstring, PartyName);
	CLASS_DECLARATION_S_NO_SET(BM::GUID, MasterCharGuid);
	CLASS_DECLARATION_S(SPartyOption, Option);//파티 경험치/아이템 나누기 룰
	CLASS_DECLARATION_S(bool, Expedition); // 원정대에 속해있는 파티인지 아닌지.
	CLASS_DECLARATION_S(bool, AllowEmpty); // 빈채로 있는 것이 허용된 파티.

protected:
	mutable Loki::Mutex m_kMutex;

	ContPartyMember m_kMemberArr;//파티원목록
	PgPartyContents*	m_pkPartyContents;
};

////////////////////////////////////////////////////////////////////
// Global(Contents server) Party
class PgGlobalParty : public PgParty
{
	typedef std::set< BM::GUID > ContWaitJoinList;// 파티등록 대기자목록. first key : 캐릭터GUID, second : N/A
	typedef std::map< BM::GUID, short > ContOtherChannelJoinInfo; // 다른 채널에서 가입을 신청한 캐릭터 정보를 수락/거절 응답할때까지 저장해두는 컨테이너.
																  // first : 캐릭터 guid, second : 채널 번호
public:
	PgGlobalParty();
	virtual ~PgGlobalParty();

	virtual void Init(BM::GUID const &rkNewGuid, std::wstring const &rkNewName, SPartyOption const& rkOption);
	virtual void Clear();

	HRESULT AddWait(BM::GUID const &rkCharGuid);//대기자목록에 등록
	bool IsWait(BM::GUID const &rkCharGuid);//대기자목록에 있는 유저?
	bool DelWait(BM::GUID const &rkCharGuid);//대기자목록에 삭제
	void GetWaitCharGuidList(VEC_GUID& rkOutWaitGuid);// 가입대기유저들의 캐릭터GUID
	bool IsDestroy() const;

	virtual BYTE ChangeAbility(BM::GUID const &rkCharGuid, BYTE const cType, BM::Stream &rkPacket);

	virtual HRESULT Add(SPartyUserInfo const& rkPartyMember, bool const bMaster);
	virtual HRESULT Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster = BM::GUID::NullData());

	bool SetPartyMaster(BM::GUID const &rkCharGuid, BM::GUID &rkNewMaster);
	HRESULT SetFriendState(BM::GUID const &rkCharGuid, bool bState);
	int GetFriendCount(BM::GUID const &rkCharGuid);
	bool PartyFriendCheck(SPartyUserInfo& kMember);

protected:
	ContWaitJoinList m_kContWaitJoinList;// 가입대기자목록
};

////////////////////////////////////////////////////////////////////
// Local(Map server) Party
class PgLocalParty : public PgParty
{
	typedef std::map< SChnGroundKey, unsigned int > ContGndTotalLevel;//그라운드 별 토탈 레벨
public:
	PgLocalParty();
	virtual ~PgLocalParty();

	virtual void Init(BM::GUID const &rkNewGuid, std::wstring const &rkNewName, SPartyOption const& rkOption);
	virtual void Clear();

//	void WaitToPortal(bool const bWait, SReqMapMove_MT const& rkRMM = SReqMapMove_MT());
	virtual bool MovedGnd(BM::GUID const &rkCharGuid, SChnGroundKey const &rkKey);
	void RefreshTotalLevel();
	bool GetGndTotalLevel(SChnGroundKey const &rkGndKey, unsigned int& iOutTotalLevel)const;

	bool GetLocalMemberList(SChnGroundKey const &rkGndKey, VEC_GUID& rkOutVec, BM::GUID const &rkIgnoreGuid = BM::GUID::NullData()) const;//임의의 그라운드에 속한 유저리스트만 뽑아냄

	virtual BYTE ChangeAbility(BM::GUID const &rkCharGuid, BYTE const cType, BM::Stream &rkPacket);

	virtual HRESULT Add(SPartyUserInfo const& rkPartyMember, bool const bMaster);
	virtual HRESULT Del(BM::GUID const &rkCharGuid, BM::GUID const &rkNewMaster = BM::GUID::NullData());

	void ReadFromPacket(BM::Stream& rkPacket);

	int GetLocalMemberFriend(BM::GUID const &rkCharGuid);

	CLASS_DECLARATION_S(BM::GUID, ExpeditionGuid);

	//	인던 포털관련 -------------------------------------------------------------------------
//	CLASS_DECLARATION_S(BM::GUID, PortalCancelUser);
//	CLASS_DECLARATION_S_NO_SET(SReqMapMove_MT, ReqMapMoveInfo);
//	CLASS_DECLARATION_S_NO_SET(DWORD, ReadyPortalTime);// 인던포탈준비를 시작한 시간
//	CLASS_DECLARATION_S(bool, IsNfyWaitToPortal);// 인던으로 파티포탈을 기다리라고 파티원들에게 통보했나?
//	CLASS_DECLARATION_S_NO_SET(bool, IsWaitToPortal);// 인던으로 파티포탈을 기다리는중인가?
protected:
	//파티 옵션 관련 -----------
	ContGndTotalLevel m_kGndTotalLevel;//그라운드당 총 플레이어 레벨
};

typedef std::vector<PgLocalParty*> ContLocalParty;
typedef std::vector<PgGlobalParty*> ContGlobalParty;

#endif // WEAPON_VARIANT_PARTY_PGPARTY_H