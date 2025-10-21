#pragma once

class PgIndunPartyMgr
{
public:
	struct SPartyInfo
	{
		SPartyInfo() : dwTime(0)
		{
		}

		DWORD dwTime;
		CONT_INDUN_PARTY_CHANNEL kContChannel;
	};
	typedef std::map<int, SPartyInfo> CONT_INDUN_PARTY_LIST;

	struct SSendRefresh
	{
		SSendRefresh() : iMapNo(0), dwTime(0), bSending(false) {}
		BM::GUID kCharGuid;
		int iMapNo;
		DWORD dwTime;
		bool bSending;
		int Type;
	};
	typedef std::map<BM::GUID, SSendRefresh> CONT_SEND_REFRESH;

public:
	PgIndunPartyMgr();
	~PgIndunPartyMgr();

	void GetList(BM::GUID const& kCharGuid, BM::GUID const& kKeyGuid, int const Type, VEC_INT const& kContMapNo);
	void Update();
	void SyncIndunParty(BM::Stream * pkPacket);

private:
	void Send(BM::GUID const& kCharGuid, BM::GUID const& kKeyGuid, int const Type, CONT_INDUN_PARTY_CHANNEL const& kCont);
	void Send(BM::GUID const& kCharGuid, BM::GUID const& kKeyGuid, short const sChannel, int const Type, CONT_INDUN_PARTY const& kCont);
	void SendRefresh(BM::GUID const& kCharGuid, BM::GUID const& kKeyGuid, int const Type, int const iMapNo);

private:
	mutable Loki::Mutex m_kMutex;

	DWORD m_kCasingTime;
	DWORD m_kDelayTime;

	CONT_INDUN_PARTY_LIST m_kContIndunPartyList;
	CONT_SEND_REFRESH m_kContSendRefresh;
};

#define g_kIndunPartyMgr SINGLETON_STATIC(PgIndunPartyMgr)