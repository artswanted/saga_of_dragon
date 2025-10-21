#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGMYHOMEMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGMYHOMEMGR_H

#include "Lohengrin/packetstruct.h"
#include "BM/twrapper.h"
#include "Doc/PgDoc_Player.h"

typedef std::map<BM::GUID,__int64> CONT_BIDDING_TIME;
typedef std::set<BM::GUID>	CONT_BIDDER_LIST;
typedef std::map<BM::GUID,int> CONT_HOME_LOCK;

typedef std::map<short,CONT_MYHOME> CONT_MYHOMEBUILDINGS;

typedef std::map<BM::GUID,BM::GUID> CONT_MYHOME_CHAT_PLAYER;

class PgMyHomeMgrImpl
{
public:

	PgMyHomeMgrImpl();
	~PgMyHomeMgrImpl(){}

public:

	bool GetHome(short const kStreetNo,int const kHouseNo,SMYHOME & kMyHome)const;
	bool SetHome(SMYHOME const & kMyHome);
	bool HasBiddingDelayTime(BM::GUID const & kOwnerGuid);
	bool AddBiddingTime(BM::GUID const & kOwnerGuid);
	bool IsBidding(BM::GUID const & kOwnerGuid);
	bool AddBidder(BM::GUID const & kOwnerGuid);
	void RemoveBidder(BM::GUID const & kOwnerGuid);
	void Tick();

	HRESULT ProcessModifyPlayer(EItemModifyParentEventType const kCause, SPMO const &kOrder, PgDoc_Player* pkDocPlayer,BM::Stream const &kAddonPacket, BM::Stream & rkPacket, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr & kContLogMgr);

	void AddMyHome(short const siStreetNo, CONT_MYHOME const & kCont);

	bool RemoveMyHome(short const siStreetNo, int const iHouseNo);

	void RecvPacketHandler(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

	bool IsLocked(BM::GUID const kGuid) const;

	void Lock(BM::GUID const kGuid);

	void Unlock(BM::GUID const kGuid);

	void RecvHomeChatHandler(BM::Stream::DEF_STREAM_TYPE const kPacketType, BM::Stream * const pkPacket);

private:

	void ClearBiddingDelayTime();
	void ProcessBidding(SMYHOME const & kMyHome,BM::PgPackedTime const & kCurTime);
	void ProcessAttachment(SMYHOME const & kMyHome,BM::PgPackedTime const & kCurTime);
	void ProcessSideJob(SMYHOME const & kMyHome,BM::PgPackedTime const & kCurTime);
	void SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid = true/* false 는 캐릭터guid */);

	HRESULT Proc_PT_M_I_REQ_MYHOME_AUCTION_UNREG(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Proc_PT_M_I_REQ_MYHOME_PAY_TEX(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Proc_PT_M_I_REQ_MYHOME_AUCTION_REG(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Proc_PT_M_I_REQ_MYHOME_BIDDING(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);

	CLASS_DECLARATION_S(CONT_MYHOMEBUILDINGS,Cont);
	CLASS_DECLARATION_S(CONT_BIDDING_TIME,ContTime);
	CLASS_DECLARATION_S(CONT_BIDDER_LIST,ContBidder);
	CLASS_DECLARATION_S(CONT_HOME_LOCK,ContLock);
	CLASS_DECLARATION_S(CONT_MYHOMECHATROOM,ContChatRoom);
	CLASS_DECLARATION_S(CONT_MYHOME_CHAT_PLAYER,ContChatPlayer);
	CLASS_DECLARATION_S(BM::GUID,GlobalChatRoomGuid);

	HRESULT Proc_PT_M_I_REQ_START_SIDE_JOB(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Proc_PT_M_I_REQ_CANCEL_SIDE_JOB(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);

	bool FindGuestFromChatRoom(BM::GUID const & kPlayerGuid, BM::GUID & kChatRoomGuid);
	void SendToRoom(BM::GUID const & kRoomGuid, BM::Stream const & kPacket, BM::GUID const & kIgnoreGuid = BM::GUID::NullData());

	HRESULT Recv_PT_C_M_REQ_MYHOME_CHAT_ENTER(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Recv_PT_C_M_REQ_MYHOME_CHAT_EXIT(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Recv_PT_C_M_REQ_MYHOME_CHAT_ROOM_CREATE(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Recv_PT_C_M_REQ_MYHOME_CHAT_MODIFY_ROOM(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Recv_PT_C_M_REQ_MYHOME_CHAT_MODIFY_GUEST(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Recv_PT_C_M_REQ_CHAT(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Recv_PT_C_M_REQ_MYHOME_CHAT_SET_ROOMMASTER(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
	HRESULT Recv_PT_C_M_REQ_MYHOME_CHAT_KICK_GEUST(BM::GUID const & kOwnerGuid, BM::Stream * const pkPacket);
};

class PgMyHomeMgr : public TWrapper<PgMyHomeMgrImpl>
{
public:
	PgMyHomeMgr(){}
	~PgMyHomeMgr(){}

	bool GetHome(short const kStreetNo,int const kHouseNo,SMYHOME & kMyHome);
	bool SetHome(SMYHOME & kMyHome);
	bool HasBiddingDelayTime(BM::GUID const & kOwnerGuid);
	bool AddBiddingTime(BM::GUID const & kOwnerGuid);
	bool IsBidding(BM::GUID const & kOwnerGuid);
	bool AddBidder(BM::GUID const & kOwnerGuid);
	void RemoveBidder(BM::GUID const & kOwnerGuid);
	void Tick();

	void AddMyHome(short const siStreetNo, CONT_MYHOME const & kCont);

	bool RemoveMyHome(short const siStreetNo, int const iHouseNo);

	HRESULT ProcessModifyPlayer(EItemModifyParentEventType const kCause, SPMO const &kOrder, PgDoc_Player* pkDocPlayer,BM::Stream const &kAddonPacket, BM::Stream & rkPacket, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr & kContLogMgr);

	void	RecvPacketHandler(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

	bool IsLocked(BM::GUID const kGuid) const;

	void Lock(BM::GUID const kGuid);

	void Unlock(BM::GUID const kGuid);

	void RecvHomeChatHandler(BM::Stream::DEF_STREAM_TYPE const kPacketType, BM::Stream * const pkPacket);
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGMYHOMEMGR_H