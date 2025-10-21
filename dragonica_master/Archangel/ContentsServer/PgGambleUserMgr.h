#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGGAMBLEUSERMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGGAMBLEUSERMGR_H

#include "Lohengrin/packetstruct.h"
#include "BM/twrapper.h"
#include "variant/PgGambleMachine.h"

typedef struct tagGambleUserInfo
{
	BM::GUID					kCharGuid;
	int							iMixPoint;
	int							iRouletteCount;
	CONT_GAMBLEMACHINERESULT	kContResult;
	bool						bBroadcast;
	tagGambleUserInfo():iMixPoint(0),iRouletteCount(0),bBroadcast(false)
	{
	}
	explicit tagGambleUserInfo(BM::GUID const & __charguid, int const __roulettecount,int const __mixpoint = 0):
	kCharGuid(__charguid), iMixPoint(__mixpoint), iRouletteCount(__roulettecount), bBroadcast(false)
	{
	}
	~tagGambleUserInfo()
	{
	}
}SGambleUserInfo;

typedef std::map<BM::GUID, SGambleUserInfo>	CONT_GAMBLE_USER;

class PgGambleUserMgrImpl
{
	CONT_GAMBLE_USER	m_kCont;
public:
	PgGambleUserMgrImpl(){}
	~PgGambleUserMgrImpl(){}
public:

	bool IsGambling(BM::GUID const & kGuid);
	bool Get(BM::GUID const & kCharGuid, SGambleUserInfo & kGambleInfo);
	bool Set(BM::GUID const & kCharGuid, SGambleUserInfo & kGambleInfo);
	bool Add(BM::GUID const & kCharGuid, int const iRouletteCount, int const iMixPoint = 0);
	bool Remove(BM::GUID const & kCharGuid);
	void LogOut(SContentsUser const& rkCharInfo, int const iFrom, int const iTitle, int const iText);
};

class PgGambleUserMgr : public TWrapper<PgGambleUserMgrImpl>
{
public:

	PgGambleUserMgr(){}
	~PgGambleUserMgr(){}

public:

	bool IsGambling(BM::GUID const & kGuid)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_);
		return Instance()->IsGambling(kGuid);
	}

	bool Get(BM::GUID const & kCharGuid, SGambleUserInfo & kGambleInfo)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_);
		return Instance()->Get(kCharGuid, kGambleInfo);
	}

	bool Set(BM::GUID const & kCharGuid, SGambleUserInfo & kGambleInfo)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		return Instance()->Set( kCharGuid, kGambleInfo);
	}

	bool Add(BM::GUID const & kCharGuid, int const iRouletteCount, int const iMixPoint)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		return Instance()->Add(kCharGuid, iRouletteCount, iMixPoint);
	}

	bool Remove(BM::GUID const & kCharGuid)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		return Instance()->Remove(kCharGuid);
	}

	void LogOut(SContentsUser const& rkCharInfo, int const iFrom, int const iTitle, int const iText)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		return Instance()->LogOut(rkCharInfo, iFrom, iTitle, iText);
	}
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGGAMBLEUSERMGR_H