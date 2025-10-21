#ifndef CENTER_CENTERSERVER_CONTENTS_PGGLOBALPARTYMGR_H
#define CENTER_CENTERSERVER_CONTENTS_PGGLOBALPARTYMGR_H

#include "Lohengrin/ProcessConfig.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgExpeditionMgr.h"

class PgGlobalPartyMgr_Base
{
public:
	PgGlobalPartyMgr_Base(){}
	virtual ~PgGlobalPartyMgr_Base(){}

	virtual bool ProcessMsg(SEventMessage *pkMsg) = 0;
	virtual bool GetPartyMemberList( VEC_GUID &rkOutCharGuidList, BM::GUID const &kGuid, bool const bPartyGuid=false, bool const bIgnoreCharGuid=false )const = 0;
	virtual void OnTick(DWORD Time) { };
};

class PgGlobalPartyMgrImpl
	: public PgPartyMgr< PgGlobalParty >
	, public PgExpeditionMgr< PgGlobalExpedition >
{
	typedef std::set< std::wstring > PartyNameSet;
	typedef std::set< SChnGroundKey > CheckOverlap;
	typedef std::map< BM::GUID, BM::GUID > ContCharToParty;
	typedef std::map< BM::GUID, BM::GUID > ContCharToExpedition;
	typedef std::map< BM::GUID, BM::GUID > ContPartyToExpedition;
	typedef std::vector< SPartyUserInfo > VEC_UserInfo;
	typedef std::map< BM::GUID, SOtherChannelPartyJoinInfo > ContOtherChannelJoinInfo;

public:

	PgGlobalPartyMgrImpl();
	virtual ~PgGlobalPartyMgrImpl();

	bool ProcessMsg(SEventMessage *pkMsg);
	bool GetPartyMemberList( VEC_GUID &rkOutCharGuidList, BM::GUID const &kGuid, bool const bPartyGuid=false, bool const bIgnoreCharGuid=false )const;
	void WritePartyListInfoToPacket(BM::Stream &rkPacket, BM::Stream& rkSendPacket);
	void WriteFindPartyUserListInfoToPacket(BM::Stream &rkPacket, BM::Stream &rkSendPacket);
	void ClearExpeditionJoinWaitMember(DWORD Time);
	void OnTick(DWORD Time);

protected:
	bool PartyBuffRefresh(BM::GUID const &kGuid, BM::GUID const &rkPartyGuid);
	HRESULT CheckJoinParty(SContentsUser const& rkMaster, SContentsUser const& rkUser, bool bIsDungeonParty = false);

	void SyncToLocalPartyMgr(const PgGlobalParty* pkParty);
	void SendToLocalPartyMgr(const PgGlobalParty* pkParty, BM::Stream const& rkPacket);

	// send packet interface.
	void SendToPartyMember( PgGlobalParty const * pkParty, BM::Stream const& rkSndPacket, BM::GUID const &rkIgnore = BM::GUID::NullData());// 파티멤버들에게 보냄

	HRESULT ReqCreateParty(SContentsUser const& rkMaster, BM::Stream& rkPacket);
	HRESULT ReqChangeOptionParty( SContentsUser const &kMaster, BM::Stream &rkPacket );
	HRESULT ReqRenameParty( BM::GUID const &rkCharGuid, BM::Stream& rkPacket);
	HRESULT ReqRenameParty( PgGlobalParty * pkParty, BM::GUID const &rkCharGuid, BM::Stream& rkPacket );
	HRESULT ReqJoinParty(SContentsUser const& rkMaster, SContentsUser const& rkUser, SPartyOption& rkOption);
	HRESULT ReqJoinPartyFind(SContentsUser const& rkMaster, SContentsUser const& rkUser, SPartyOption const& rkOption, bool bIsDungeonParty = false);
	HRESULT AnsJoinParty(BM::GUID const &rkPartyGuid, SContentsUser const& rkNewPartyUserInfo, bool const bWantJoin, bool const bMaster);
	HRESULT LeaveParty( BM::GUID const &kCharGuid, bool const bIsExpedition = false );
	HRESULT KickUser(BM::GUID const &rkPartyGuid, SContentsUser const &rkUser, BM::GUID const &rkKickCharGuid, int const &KickUserGroundNo, BM::GUID const &rkKickMemberGuid, std::wstring const& rkKickName);
	HRESULT ChangeMaster( BM::GUID const &kMasterCharGuid, BM::GUID const &kTargetCharGuid);

	HRESULT ReqCreateFindParty(SContentsUser const& rkChar, BM::Stream& rkPacket);
	HRESULT CreateFindParty(SContentsUser const& rkChar, std::wstring& rkContents);

	bool ReqDeleteFindParty(BM::GUID const& rkCharGuid);

	bool GetFindPartyList(BM::GUID const& rkCharGuid, SFindPartyUserListInfo& rkOutFindPartyUserInfo);

	bool MovedProcess( BM::GUID const &rkPartyGuid, SContentsUser const &rkUser, short const nChannelNo );
	void DestroyParty(BM::GUID const &rkPartyGuid);
	void PartyWaitUserSend(PgGlobalParty* pkParty, SContentsUser const &rkUser);
	void PartyWaitUserSend_Refuse(PgGlobalParty* pkParty, SContentsUser const &rkUser);

	bool MasterCancelJoining(SContentsUser const &rkMaster);
	bool ChangeMemberAbility(BM::GUID const &rkPartyGuid, SContentsUser const &rkUser, BM::Stream &rkPacket);
	bool SummonMember(BM::GUID const &rkPartyGuid, SContentsUser const &rkCaster);

	bool SendParty_Packet(BM::GUID const &rkCharGuid, BM::Stream const &rkPacket);

	bool GetCharToParty(BM::GUID const &rkCharGuid, BM::GUID& rkOutGuid) const;
	bool AddCharToParty(BM::GUID const &rkCharGuid, BM::GUID const &rkPartyGuid);
	BM::GUID DelCharToParty(BM::GUID const &rkCharGuid);
	void DelAllCharToParty(BM::GUID const & PartyGuid);
	bool FindCharToParty(BM::GUID const &rkCharGuid) const;

	PgGlobalParty * GetCharToParty(BM::GUID const &rkCharGuid )const;

	bool PartyFriendCheck(BM::Stream* pkPacket);
	bool PartyFriendCheckInfo(BM::Stream* pkPacket);
	void PartyBuffMember(PgGlobalParty* pkParty, BM::GUID const &rkCharGuid, bool bBuff);
	bool GetMemberGroundKey(BM::GUID const & rkCharGuid,SChnGroundKey & kGndKey)const;
	void DeleteLog(SContentsUser const &rkCaster, BM::GUID const &rkPartyGuid);
	bool LogPartyAction( PgParty* const pkParty, SContentsUser const& kUserInfo, EOrderSubType const kType )const;
	void SetPartyOptionAreaNo(int const iGroundNo, SPartyOption& rkOption);
	void SetPartyRefuseState(BM::GUID const& kCharGuid, bool const bRefuse, BYTE const byState);

	//Expedition
	HRESULT ReqCreateExpedition(SContentsUser const & rkMaster, BM::Stream& rkPacket);		// 원정대 생성 요청. 원정대 생성 후에 원정대장의 파티를 원정대에 넣고 빈 파티를 생성 한다.
	HRESULT ReqJoinExpedition(SContentsUser const & rkMaster, SContentsUser const & rkUser, BM::GUID & rkExpeditionGuid, BM::Stream & rkPacket);	// 원정대 가입 요청.
	HRESULT AnsJoinExpedition(SContentsUser const & rkUser, BM::GUID & rkExpeditionGuid, bool bJoin);						// 원정대 초대/가입 요청 응답.
	HRESULT ReqLeaveExpedition(BM::GUID const & rkCharGuid, BM::GUID const & ExpeditionGuid);								// 원정대 탈퇴 요청.
	HRESULT ReqDisperseExpedition(BM::GUID const & rkCharGuid);																// 원정대 해체 요청.
	HRESULT ReqInviteExpedition(SContentsUser const & Master, BM::GUID const & ExpeditionGuid, BM::Stream & Packet);		// 원정대 초대 요청.
	HRESULT ReqTeamMove(BM::GUID const & MasterGuid, BM::GUID const & ExpeditionGuid, BM::Stream & Packet);				// 원정대 팀원 팀 교체 요청.
	HRESULT ReqChangeExpeditionMaster(BM::GUID const & ExpeditionGuid, BM::GUID const & MasterGuid, BM::GUID const & NewMasterGuid);	// 원정대장 교체.
	HRESULT ReqExpeditoinRename(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid, std::wstring & NewName);				// 원정대 이름 변경.
	HRESULT ReqChangeExpeditionOption(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid, SExpeditionOption const & NewOption);// 원정대 옵션 변경.
	HRESULT ReqKickExpeditionMember(BM::GUID const & ExpeditionGuid, BM::GUID const & MasterGuid, BM::GUID const & KickUserGuid);		// 원정대 멤버 추방 요청.

	HRESULT CheckJoinExpedition(SContentsUser const& rkMaster, SContentsUser const& rkUser);						// 원정대 가입 조건 검사.

	bool GetCharToExpedition(BM::GUID const & rkCharGuid, BM::GUID & rkOutGuid) const;								// 원정대에서 캐릭터를 찾는다.
	bool AddCharToExpedition(BM::GUID const & rkCharGuid, BM::GUID const & rkExpeditionGuid);						// 원정대에 캐릭터를 넣음.
	BM::GUID DelCharToExpedition(BM::GUID const & rkCharGuid);														// 원정대에서 캐릭터를 삭제.

	bool GetPartyToExpedition(BM::GUID const & rkPartyGuid, BM::GUID & rkOutGuid) const;							// 원정대에서 파티를 찾는다.
	bool AddPartyToExpedition(BM::GUID const & rkPartyGuid, BM::GUID const & rkExpeditionGuid);						// 원정대에 파티를 넣음.
	BM::GUID DelPartyToExpedition(BM::GUID const & rkPartyGUid);													// 원정대에서 파티를 삭제.

	PgGlobalExpedition* GetPartyToExpedition(BM::GUID const & rkParty);												// 특정 파티를 포함하면 원정대 포인터 반환.

	void SendToPartyMember( PgParty const *pkParty, BM::Stream const& rkSndPacket, BM::GUID const &rkIgnore = BM::GUID::NullData());

	void SendToExpedition(PgGlobalExpedition const * pExpedition, BM::Stream const & SendPacket, BM::GUID const & rkIgnore = BM::GUID::NullData());		// 소속된 원정대원 모두에게 패킷 전송.

	HRESULT Recv_ExpeditionMemberState(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid, bool const & IsAlive); // 원정대 멤버의 현재 생존 여부 알림.

	bool LogExpeditionAction(PgGlobalExpedition * const pExpedition, SContentsUser const & UserInfo, EOrderSubType const Type) const;

	void SyncToLocalPartyMgr(PgGlobalExpedition const * pExpedition); // Local Sync.
	void SendToLocalPartyMgr(PgGlobalExpedition const * pExpedition, const BM::Stream & Packet);

	void CreateEventGroundParty(BM::GUID const & PartyGuid);
	void AddEventGroundParty(BM::GUID const & PartyGuid, SContentsUser const & NewPartyUserInfo, bool bMaster);

	void DeleteOhterChannelJoinInfo_MasterLeaveParty(BM::GUID const& MasterGuid, PgGlobalParty * pParty);
protected:

	HRESULT CreateExpedition(std::wstring const & rkExpeditionName, PgGlobalExpedition *& pkExpedition, BM::GUID & rkOutGuid, SExpeditionOption const & rkOption);
	bool DeleteExpedition(BM::GUID const & ExpeditionGuid);
	bool IsExpeditionPartyMember(BM::GUID const & ExpeditionGuid, BM::GUID const & rkCharGuid) const;

protected:
	void DestroyParty( PgGlobalParty * pkParty );

	virtual bool Delete(BM::GUID const &rkPartyGuid);
	virtual HRESULT CreateParty(std::wstring const& rkPartyName, BM::GUID& rkOutGuid, SPartyOption const& rkOption);

	bool SendToGround( SChnGroundKey const &kChnGndkey, BM::Stream const &kPacket )const;
	bool SendToUser( PgParty *pkParty, BM::GUID const &rkGuid, BM::Stream const &kPacket )const;
	bool SendToUser( short const nChannel, BM::GUID const &kMemberGuid, BM::Stream const &kPacket )const;
	void SendOptionParty(PgGlobalParty * pkParty);

protected:

	ContCharToParty m_kCharToParty;//Character Guid -> Party Guid
	PartyNameSet m_kContPartyNameSet;//Party Name Set
	ContCharToExpedition m_kCharToExpedition; // Character Guid -> Expedition Guid ( 원정대에 가입한 캐릭터들의 GUID ).
	ContPartyToExpedition m_kPartyToExpedition;  // 원정대에 포함 되어있는 파티들의 GUID.

	ContOtherChannelJoinInfo m_ContOtherChannelJoinInfo; // 다른 채널에서 파티 가입 신청한 캐릭터들의 정보를 관리.
};

class PgGlobalPartyMgr
	: public PgGlobalPartyMgr_Base
	, public TWrapper< PgGlobalPartyMgrImpl >
{
public:
	PgGlobalPartyMgr();
	virtual ~PgGlobalPartyMgr();

	virtual bool ProcessMsg(SEventMessage *pkMsg);
	virtual bool GetPartyMemberList( VEC_GUID &rkOutCharGuidList, BM::GUID const &kGuid, bool const bPartyGuid=false, bool const bIgnoreCharGuid=false ) const;
	virtual void OnTick(DWORD Time);
};

//
class PgGlobalPartyMgr_Public
{
public:
	typedef std::map< BM::GUID, short >		CONT_PARTY_CHANNEL;

public:
	PgGlobalPartyMgr_Public();
	virtual ~PgGlobalPartyMgr_Public();
	
	HRESULT Add( BM::GUID const &kCharGuid, short const nChannelNo );
	short Del( BM::GUID const &kCharGuid );
	short GetChannel( BM::GUID const &kCharGuid )const;

private:
	CONT_PARTY_CHANNEL		m_kContPartyChannel;
};

class PgGlobalPartyMgr_Public_Wrapper
	:	public PgGlobalPartyMgr_Base
	,	public	TWrapper<PgGlobalPartyMgr_Public>
{
public:
	PgGlobalPartyMgr_Public_Wrapper();
	virtual ~PgGlobalPartyMgr_Public_Wrapper();

	virtual bool ProcessMsg(SEventMessage *pkMsg);
	virtual bool GetPartyMemberList( VEC_GUID &rkOutCharGuidList, BM::GUID const &kGuid, bool const bPartyGuid=false, bool const bIgnoreCharGuid=false )const;

	HRESULT Add( BM::GUID const &kCharGuid, short const nChannelNo );
	short Del( BM::GUID const &kCharGuid );
	short GetChannel( BM::GUID const &kCharGuid )const;
};

template <class T> struct CreateStatic_GlobalPartyMgr;

template < > struct CreateStatic_GlobalPartyMgr< PgGlobalPartyMgr_Base >
{
	static PgGlobalPartyMgr_Base* Create()
	{
		if ( true == g_kProcessCfg.IsPublicChannel() )
		{
			return new PgGlobalPartyMgr_Public_Wrapper;
		}
		return new PgGlobalPartyMgr;
	}

	static void Destroy(PgGlobalPartyMgr_Base* p)
	{
		delete p;
	}
};

// PgServerSetMgr에서 g_kPartyMgr 싱글턴을 호출하게 되면 교착상태에 빠질 수 있음으로 확실하지 않으면 호출하지 말아야 됨

#define g_kPartyMgr Loki::SingletonHolder< PgGlobalPartyMgr_Base, CreateStatic_GlobalPartyMgr >::Instance()

#endif // CENTER_CENTERSERVER_CONTENTS_PGGLOBALPARTYMGR_H