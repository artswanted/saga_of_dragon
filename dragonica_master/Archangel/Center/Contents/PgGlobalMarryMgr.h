#ifndef CENTER_CENTERSERVER_CONTENTS_PGGLOBALMARRYMGR_H
#define CENTER_CENTERSERVER_CONTENTS_PGGLOBALMARRYMGR_H

class PgGlobalMarryMgr
{
public:

	PgGlobalMarryMgr();
	virtual ~PgGlobalMarryMgr();

	virtual void Clear();
	void Tick();

	bool ProcessMsg(unsigned short const usType, BM::Stream* const pkMsg);

protected:
	void SetMoney(__int64 i64SendAdd);
	void AddMoney(__int64 i64SendAdd);
	void Init(SGroundKey const & kGndKey, BM::GUID &kCharGuid, BM::GUID &kCoupleGuid, std::wstring &kName, std::wstring &kCoupleName, int iGender1, int iGender2);
	bool IsMarryUse();
	bool IsMarryCouple(BM::GUID const &kCharGuid);
	void SetGndKey(SGroundKey const &kGroundKey);
	bool IsAddTotalMoneyMax(__int64 i64SendAdd);	

	CLASS_DECLARATION_S(DWORD, MarryStartCheckTime);
	CLASS_DECLARATION_S(DWORD, NpcTalkCheckTime);
	CLASS_DECLARATION_S(__int64, i64TotalMoney);
	CLASS_DECLARATION_S(SGroundKey, GndKey);
	CLASS_DECLARATION_S(BM::GUID, CharGuid);
	CLASS_DECLARATION_S(BM::GUID, CoupleGuid);
	CLASS_DECLARATION_S(std::wstring, CharName);
	CLASS_DECLARATION_S(std::wstring, CoupleName);
	CLASS_DECLARATION_S(int, Gender1);
	CLASS_DECLARATION_S(int, Gender2);
	CLASS_DECLARATION_S(SGroundKey, VillageGndKey);	

	mutable Loki::Mutex m_kMutex;
};

#define g_kMarryMgr SINGLETON_STATIC(PgGlobalMarryMgr)

#endif // CENTER_CENTERSERVER_CONTENTS_PGGLOBALMARRYMGR_H