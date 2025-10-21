#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_REALMCHATMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_REALMCHATMGR_H

#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "BM/PgTask.h"

//
class PgRealmChatMgr
{
public:
	PgRealmChatMgr(void);
	virtual ~PgRealmChatMgr(void);

public:
protected:
	bool SendChat_Whisper(int const iChatMode, SContentsUser const &rkFromUser, SContentsUser const &rkToUser, std::wstring const &rkContents, BM::Stream const& rkAddonPacket);
	bool SendChat_ManToMan(SContentsUser const& rkFromUser, SContentsUser const& rkToUser, std::wstring const &rkContents, BM::Stream const& rkAddonPacket);
	//bool RecvChat_Whisper_ByName(SContentsUser const &rkFromUser, SContentsUser const &rkToUser, std::wstring const &rkChat);//귓말(닉네임으로)
	bool RecvChat_Whisper_ByGuid(int const iChatMode, SContentsUser const &rkFromUser, SContentsUser const &rkToUser, std::wstring const &rkContents, BM::Stream const& rkAddonPacket);//귓말(Guid로)
	bool RecvChat_ManToMan(SContentsUser const& rkFromUser, SContentsUser const& rkToUser, std::wstring const &rkContents, BM::Stream const& rkAddonPacket);//1:1
	//bool RecvChat_Party(SContentsUser const &rkFromUser, BM::Stream const & kPacket) const;//친구대화
	bool RecvChat_Friend(SContentsUser const &rkFromUser, BM::Stream& rkPacket) const;//친구대화
	bool RecvChat_Guild(SContentsUser const &rkFromUser, BM::Stream& rkPacket) const;//길드
	bool RecvChat_Trade(SContentsUser const &rkFromUser, BM::Stream& rkPacket) const;//거래
	//>>임시적으로 잠금(기획상 없다)
	//bool RecvChat_NoticeWorld(BM::GUID const &rSenderGUID, std::wstring const &rwstrChat);	//공지
	//bool RecvChat_NoticeMap(BM::GUID const &rSenderGUID, std::wstring const &rwstrChat);		//공지
	//bool RecvChat_NoticeParty(BM::GUID const &rSenderGUID, std::wstring const &rwstrChat);	//공지
	//bool RecvChat_NoticeTeam(BM::GUID const &rSenderGUID, std::wstring const &rwstrChat);		//공지
	//bool RecvChat_NoticeMan(BM::GUID const &rSenderGUID, std::wstring const &rwstrChat);		//공지
	//<<임시적으로 잠금

	//bool SendChat_Whisper(SContentsUser const &rkFromUser, SContentsUser const &rkToUser, std::wstring const &rkChat);//귓말내부처리(결과)

public:
	bool ProcessMsg(SEventMessage *pkMsg);
};

#define g_kRealmChatMgr SINGLETON_STATIC(PgRealmChatMgr)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_REALMCHATMGR_H