#ifndef CENTER_CENTERSERVER_CHANNELCHATMGR_H
#define CENTER_CENTERSERVER_CHANNELCHATMGR_H

#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"

class PgChannelChatMgr
{
public:
	PgChannelChatMgr(void);
	virtual ~PgChannelChatMgr(void);

protected:
	static bool RecvChat_Party(SContentsUser const &rkFromUser, BM::Stream &rkPacket);//친구대화
public:
	static bool ProcessMsg(SEventMessage *pkMsg);
};

#define g_kChannelChatMgr SINGLETON_STATIC(PgChannelChatMgr)

#endif // CENTER_CENTERSERVER_CHANNELCHATMGR_H