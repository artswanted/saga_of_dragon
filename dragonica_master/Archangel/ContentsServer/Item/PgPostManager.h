#ifndef CONTENTS_CONTENTSSERVER_ITEM_PGPOSTMANAGER_H
#define CONTENTS_CONTENTSSERVER_ITEM_PGPOSTMANAGER_H

#include <set>

typedef std::map<BM::GUID,BM::GUID> CONT_MAIL_RESERVE;

class PgPostManager
{
public:
	
	PgPostManager()
	{
		m_kPostManagerGuid.Generate();
	}
	
	~PgPostManager(){}

public:

	void OnRecvPT_M_I_POST_REQ_MAIL_SEND(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_POST_REQ_MAIL_RECV(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_POST_REQ_MAIL_MODIFY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_POST_REQ_MAIL_MIN(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

	CLASS_DECLARATION_S(BM::GUID,PostManagerGuid);
	CLASS_DECLARATION_S(CONT_MAIL_RESERVE,ContMailReserve);

	bool Locked_ReserveMail(BM::GUID const & kMailGuId)
	{
		BM::CAutoMutex kLock(m_kMutex);
		auto kRet = m_kContMailReserve.insert(std::make_pair(kMailGuId,kMailGuId));
		return kRet.second;
	}

	void Locked_ReleaseMail(BM::GUID const & kMailGuId)
	{
		BM::CAutoMutex kLock(m_kMutex);
		m_kContMailReserve.erase(kMailGuId);
	}

	bool PostSystemMail(BM::GUID const & kCharGuid,std::wstring const & kTo,std::wstring const & kTitle,std::wstring const & kText,int const iItemNo,short const iItemCount,__int64 const & i64Money);
	bool PostSystemMail(BM::GUID const & kCharGuid,std::wstring const & kFrom,std::wstring const & kTo,std::wstring const & kTitle,std::wstring const & kText,int const iItemNo,short const iItemCount,__int64 const & i64Money, BM::GUID const & kGMCmdGuid = BM::GUID());
	bool PostSystemMailByGuid(BM::GUID const & kCharGuid, std::wstring const & kFrom, std::wstring const & kTitle,std::wstring const & kText,int const iItemNo,short const iItemCount,__int64 const & i64Money);
	bool PostSystemMailByGuid(BM::GUID const & kCharGuid, std::wstring const & kFrom, std::wstring const & kTitle, std::wstring const & kText,PgBase_Item const & kItem, __int64 const & i64Money);

protected:

	Loki::Mutex		m_kMutex;
};

#define g_kPostMgr SINGLETON_STATIC(PgPostManager)

#endif // CONTENTS_CONTENTSSERVER_ITEM_PGPOSTMANAGER_H