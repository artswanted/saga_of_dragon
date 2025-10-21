#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGFRIENDMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGFRIENDMGR_H

#include <string>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "BM/PgTask.h"
#include "Lohengrin/PacketStruct.h"
//////////////////////////////////////////////////////
// 친구는 Char Guid 기반입니다.

class PgMessenger;

typedef std::vector< SFriendItem > ContFriendVec;
typedef std::map< BM::GUID, SFriendItem* > ContFriendItem;
typedef BM::TObjectPool< SFriendItem > PoolFriendItem;

typedef std::map< BM::GUID, PgMessenger*> ContMessenger;
typedef std::set<BM::GUID> ContReqWaitFriend;
typedef BM::TObjectPool< PgMessenger > PoolFriendList;
typedef std::vector<SPartyUserInfo> VEC_UserInfo;

class PgMessenger
{
	static PoolFriendItem m_kPoolFriendItem;//친구 Item 풀(by CharGuid)

public:
	PgMessenger(){};
	virtual ~PgMessenger(){};

public:
	void Clear();
	bool IsExist(BM::GUID const &rkGuid) const;

	HRESULT AddFriend(const SFriendItem &kItem, bool bIsNew = true);
	HRESULT AddFriend(SContentsUser const &kItem, BYTE const eState, bool bIsNew = true);

	HRESULT Get(BM::GUID const &rkGuid, SFriendItem &pkItem) const;
//친구 및 그룹.
//추가
//삭제
//이터레이션.
//채팅
//상태 변경(On/Off, 및 기타 상태)
//말 금지.
	HRESULT Modify(const SFriendItem &rkItem);

	HRESULT SendModify(ContFriendVec const &kFrndVec, EFriendCommandType const eType);

	HRESULT Del(VEC_GUID const &rkFriendMembGuid);
	HRESULT DelAll();
	void DelFromListOnly(VEC_GUID const &rkFriendMembGuid);	//디비에 쿼리하지 않고 단순히 리스트에서만 지우자.
	
	size_t Size()const{return m_kFriendList.size();}

	void GetFriendList(ContFriendVec &rkFrndVec) const;
	
//	bool MapMove(BM::GUID const &rkGuid, int iMapNo);
	bool LinkChange(BM::GUID const &rkGuid, BYTE cLinkStatus);
	bool SendToDB(const SFriendItem *kItem, BYTE const eType);

protected:
	ContFriendItem m_kFriendList; //내친구 들 목록
	mutable Loki::Mutex m_kMutex;

	CLASS_DECLARATION(BM::GUID, m_kCharGuid, CharGuid); //친구 목록 Owner
	CLASS_DECLARATION(BYTE, m_cLinkStatus, LinkStatus); //온/오프라인 상태
	CLASS_DECLARATION(std::wstring, m_kName, Name); //현재 로그인한 캐릭명
};

class PgFriendMgr
{
public:
	PgFriendMgr(void);
	virtual ~PgFriendMgr(void);

public:
	bool ProcessMsg(SEventMessage *pkMsg);

protected://메신져(GUID는 모두 CharacterGUID이다)
	void Clear();

	bool RecvMsn_Friend_AddByGuid(SContentsUser const &rkOwnerUser, SContentsUser const &rkFriendUser);
	bool RecvMsn_Friend_Del(SContentsUser const &rkOwnerUser, VEC_GUID const &rkContFriendMem);
	bool RecvMsn_Friend_AddAccept(BM::GUID const &rkOwnerMembGuid, BM::GUID const &rkFriendGuid);
	bool RecvMsn_Friend_ChatStatus(BM::GUID const &rkOwnerCharGuid, BM::GUID const &rkFriendGuid, BYTE const cChatStatus, bool const bFromUser = false, PgLog *pkLog = NULL);//무조건 셋팅
	bool RecvPT_T_N_NFY_MSN_FULL_DATA(SContentsUser const &rkUser, BM::Stream* pkPacket);
	bool RecvMsn_Friend_ChangeGroupName(SContentsUser const &rkUser, std::wstring const &rkOldName, std::wstring const &rkNewName);
	bool RecvMsn_Friend_ChangeLinkState(SContentsUser const &rkUser, BYTE const cLinkState);

protected:	//서버 <-> 서버
	bool MakeMsn(SContentsUser const &rkUser);//사용자 로그인 자료 준비	(사용자 로그인시에 Center->Contents)
	bool Recv_Logout(SContentsUser const &rkUser);
	bool Recv_MapMove(SContentsUser const &rkUser);

	bool Online(BM::GUID const &rkGuid) const;
	EFriendLinkStatus LinkStatus(BM::GUID const &rkGuid) const;

	bool SendFriendList(BM::GUID const &rkGuid);
	bool SendPacket_ToFriend_ByCharGuid(BM::GUID const &rkCharGuid, BM::Stream &rkPacket);
	bool RecvMsn_Friend_Party(BM::Stream* pkPacket);
	bool ChangeHomeAddr(BM::GUID const& rkCharGuid, SHOMEADDR const kHomeAddr);

protected:
	PgMessenger* GetMessenger(BM::GUID const &rkGuid) const;
	PgMessenger* NewMessenger(BM::GUID const &rkGuid);
	bool DelMessenger(BM::GUID const &rkGuid);

	void NfyMySelfToFriend(SContentsUser const &rkOwnerUser, BYTE const eState = FLS_ONLINE);
	void PgFriendMgr::NfyToMyFriend(BM::GUID const &rkCharGuid);
	HRESULT GetFriendMsnState(PgMessenger *pkMsn);

private:
	int RecvMsn_Friend_Add(SContentsUser const &rkOwnerUser, SContentsUser const &rkFriendUser);
	int const AddFriend_Check(PgMessenger *pkCasterMsgr, SContentsUser const &rkFriendUser); //에러 체크
	void DelMyInfoFormOther(SContentsUser const &rkOwnerUser, VEC_GUID const &rkContFriendMem);

protected:
	mutable Loki::Mutex m_kMutex;
	ContMessenger m_kContMsgr;//메신저 로그인 목록(by CharGuid)
	PoolFriendList m_kPoolFriendList;//친구 List Pool(by CharGuid)
	//친구목록 받을 대기열
};

//#define g_kFriendMgr SINGLETON_STATIC(PgFriendMgr)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGFRIENDMGR_H