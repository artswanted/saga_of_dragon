#ifndef MAP_MAPSERVER_ACTION_ACTION_PGMARRYMGR_H
#define MAP_MAPSERVER_ACTION_ACTION_PGMARRYMGR_H

#include "Lohengrin/packetstruct.h"
#include "BM/twrapper.h"

class PgMarryMgrImpl
{
public:

	PgMarryMgrImpl(){}
	~PgMarryMgrImpl(){}

public:
	void Clear();
	void Init(SGroundKey const & kGndKey, BM::GUID &kCharGuid, BM::GUID &kCoupleGuid, std::wstring &kName, std::wstring &kCoupleName, int iGender1, int iGender2);
	bool SetMoney(__int64 const iMoney);
	bool GetTotalMoney(__int64 & iTotalMoney) const;
	void Tick();
	bool SetActionState(BYTE eType);
	bool GetActionState(BYTE & eType) const;
	bool IsMarryCouple(BM::GUID const &kCharGuid);
	int GetEffectMALE();
	int GetEffectFEMALE();
	bool IsMarryUse();
	void GetMarryCoupleGuid(BM::GUID &kCharGuid, BM::GUID &kCoupleGuid);

private:
	int GetTextNo(int iTextGroup);

	CLASS_DECLARATION_S(SGroundKey, GndKey);
	CLASS_DECLARATION_S(BM::GUID, CharGuid);
	CLASS_DECLARATION_S(BM::GUID, CoupleGuid);
	CLASS_DECLARATION_S(std::wstring, CharName);
	CLASS_DECLARATION_S(std::wstring, CoupleName);

	CLASS_DECLARATION_S(__int64, i64TotalMoney);
	CLASS_DECLARATION_S(BYTE, bActionState);
	CLASS_DECLARATION_S(int, iTextGroup);
	CLASS_DECLARATION_S(DWORD, LastCheckTime);	
	CLASS_DECLARATION_S(int, Gender1);
	CLASS_DECLARATION_S(int, Gender2);
	CLASS_DECLARATION_S(DWORD, TotalLastCheckTime);
	CLASS_DECLARATION_S(DWORD, MarryCheckTime);
};

class PgMarryMgr : public TWrapper<PgMarryMgrImpl>
{
public:
	PgMarryMgr(){Clear();}
	~PgMarryMgr(){}

	void Clear();
	void Init(SGroundKey const & kGndKey, BM::GUID &kCharGuid, BM::GUID &kCoupleGuid, std::wstring &kName, std::wstring &kCoupleName, int iGender1, int iGender2);
	bool SetMoney(__int64 const iMoney);
	bool GetTotalMoney(__int64 & iTotalMoney);
	void Tick();
	bool SetActionState(BYTE eType);
	bool GetActionState(BYTE & eType);
	bool IsMarryCouple(BM::GUID const &kCharGuid);
	int GetEffectMALE();
	int GetEffectFEMALE();
	bool IsMarryUse();
	void GetMarryCoupleGuid(BM::GUID &kCharGuid, BM::GUID &kCoupleGuid);
};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGMARRYMGR_H