#include "stdafx.h"
#include "GMCommand.h"

//============================================================================
// PgGMLevelChecker
//----------------------------------------------------------------------------
PgGMLevelChecker::PgGMLevelChecker( CONT_DEFGMCMD const &kContDefGMCMD )
:	m_kContDefGMCMD(kContDefGMCMD)
{
}

bool PgGMLevelChecker::IsAccess( EGMCmdType const iCmdType, BYTE const byGMLevel )const
{
	CONT_DEFGMCMD::const_iterator itr = m_kContDefGMCMD.find(iCmdType);
	if ( itr != m_kContDefGMCMD.end() )
	{
		if ( (byGMLevel & itr->second.byLevel) == itr->second.byLevel )
		{
			return true;
		}
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("GMLevel mismatch....Command=") << static_cast<int>(iCmdType) << _T(", NeedGMLevel=") << itr->second.byLevel << _T(", AccountGMLevel=") << byGMLevel);
	}
	return false;
}
