#ifndef WEAPON_LOHENGRIN_CONFIG_VARIABLECONTAINER_H
#define WEAPON_LOHENGRIN_CONFIG_VARIABLECONTAINER_H

#include "ActArg.h"

class PgVariableContainer
{
public:
	PgVariableContainer();
	~PgVariableContainer();

	bool LoadIni(std::wstring const& rkfilename);

	template< typename T >
	HRESULT Get(int const iKind, int const iID, T &rkOutValue)const
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_VARIABLE::const_iterator itor = m_kContVariable.find(iKind);
		if (itor == m_kContVariable.end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
		SActArg const &rkActArg = itor->second;
		return rkActArg.Get(iID, rkOutValue);
	}

protected:
	bool ReadIni(LPCTSTR lpszSectionName, LPCTSTR lpszFilename);
	bool ParsingValue(int const iKind, LPCTSTR lpszFilename, LPCTSTR lpszSectionName, LPCTSTR lpszKeyname);
	//bool GetActArg(int const iKind, SActArg*& pkOutActArg, bool const bCreate = false);	// public 금지~
	template< typename T >
	HRESULT Set(int const iKind, int const iID, T const &rkValue, bool const bCreate = false)
	{
		CONT_VARIABLE::iterator itor = m_kContVariable.find(iKind);
		if (itor == m_kContVariable.end())
		{
			if (!bCreate)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
			auto ibRet = m_kContVariable.insert(std::make_pair(iKind, SActArg()));
			itor = ibRet.first;
		}
		SActArg& rkActArg = itor->second;
		return rkActArg.Set(iID, rkValue);
	}

protected:
	typedef std::map<int, SActArg> CONT_VARIABLE;
	CONT_VARIABLE m_kContVariable;
	mutable ACE_RW_Thread_Mutex m_kMutex;
};


#define g_kVariableContainer SINGLETON_STATIC(PgVariableContainer)

#endif // WEAPON_LOHENGRIN_CONFIG_VARIABLECONTAINER_H