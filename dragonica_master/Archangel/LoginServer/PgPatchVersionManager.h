#ifndef LOGIN_LOGINSERVER_PGPATCHVERSIONMANAGER_H
#define LOGIN_LOGINSERVER_PGPATCHVERSIONMANAGER_H

#include "datapack/pgdatapackmanager.h"

class PgPatchVersionManager
{
public:
	typedef bool (PgPatchVersionManager::*CheckVersionFunc)(__int64 const);

	PgPatchVersionManager()
		: m_CheckVersionFunc(&PgPatchVersionManager::_CheckVersion_Equal)
	{	
		m_kVersion.Version.i32Tiny = -1;
	}
	~PgPatchVersionManager(){}
public:
	__int64 const GetPatchVersion()
	{
		return (m_kVersion.Version.i32Tiny > 0 ? m_kVersion.iVersion : -1);
	}
	void SetPatchVersion(int const iMajor,int const iMinor,int const iPatch)
	{
		m_kVersion.Version.i16Major = iMajor;
		m_kVersion.Version.i16Minor = iMinor;
		m_kVersion.Version.i32Tiny = iPatch;
	}
	bool CheckVersion(__int64 const i64PatchVersion)
	{
		assert(NULL != this->m_CheckVersionFunc);

		return (this->*m_CheckVersionFunc)(i64PatchVersion);
	}

	void SetCheckVersion(int const iType)
	{
		if(1==iType)
		{
			m_CheckVersionFunc = &PgPatchVersionManager::_CheckVersion_SmallAndEqual;
		}
		else
		{
			m_CheckVersionFunc = &PgPatchVersionManager::_CheckVersion_Equal;
		}
	}
protected:

	BM::VersionInfo m_kVersion;

private:
	CheckVersionFunc m_CheckVersionFunc;
	bool _CheckVersion_SmallAndEqual(__int64 const i64PatchVersion)
	{
#ifdef _MDo_	// 외부 배포본은 무조건 버전 확인을 한다.
		return (GetPatchVersion() <= i64PatchVersion);
#else					// 내부 테스트는 버전이 없으면 체크 하지 않고 통과 시킨다.
		return ((GetPatchVersion() > 0) ? GetPatchVersion() <= i64PatchVersion : true);
#endif
	}
	bool _CheckVersion_Equal(__int64 const i64PatchVersion)
	{
#ifdef _MDo_	// 외부 배포본은 무조건 버전 확인을 한다.
		return (GetPatchVersion() == i64PatchVersion);
#else					// 내부 테스트는 버전이 없으면 체크 하지 않고 통과 시킨다.
		return ((GetPatchVersion() > 0) ? GetPatchVersion() == i64PatchVersion : true);
#endif
	}
};

#define g_kPatchVersionMgr SINGLETON_STATIC(PgPatchVersionManager)

#endif // LOGIN_LOGINSERVER_PGPATCHVERSIONMANAGER_H