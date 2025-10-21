#ifndef WEAPON_VARIANT_PGCHECKMACADDRESS_H
#define WEAPON_VARIANT_PGCHECKMACADDRESS_H

#include <IPHlpApi.h>
#pragma comment( lib, "IPHlpApi.lib" )

class PgCheckMacAddress
{
	typedef std::set< std::wstring > CONT_MACADDR;

public:
	PgCheckMacAddress(){ m_kContMacAddr.clear(); }
	~PgCheckMacAddress(){ m_kContMacAddr.clear(); }

	void SetMacAddress( LOCAL_MGR::NATION_CODE const eNationCode );
	bool CheckMacAddress();

private:
	void setInternal();

private:
	CONT_MACADDR m_kContMacAddr;
	void PushMacAddress(std::wstring const& szAdd);
};

#define g_kCheckMacAddress SINGLETON_STATIC(PgCheckMacAddress)

#endif // WEAPON_VARIANT_PGCHECKMACADDRESS_H