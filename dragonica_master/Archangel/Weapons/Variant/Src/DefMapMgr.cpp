#include "stdafx.h"
#include "DefMapMgr.h"


bool PgDefMapMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	bool bReturn = true;
	Clear();

	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	while( tblBase.end() != base_itor )
	{
		DEF *pkDef = NewDef();
		pkDef->MapNo((*base_itor).second.iMapNo);
		for (int i=0; i<MAX_MAP_ABIL_COUNT;i++)
		{
			int const iAbilNo = (*base_itor).second.iAbil[i];
			if (iAbilNo > 0)
			{
				CONT_ABIL::const_iterator abil_itor = tblAbil.find(iAbilNo);
				if (tblAbil.end() == abil_itor)
				{
					bReturn = false;
					VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannto find MapAbil MapNo=") << (*base_itor).second.iMapNo << _T(", AbilNo=") << iAbilNo);
				}
				else
				{
					for (int j=0; j<MAX_MAP_ABIL; j++)
					{
						if ((*abil_itor).second.iType[j] != 0)
						{
							pkDef->SetAbil((*abil_itor).second.iType[j], (*abil_itor).second.iValue[j]);
						}
					}
				}
			}
		}

		auto kRet = m_contDef.insert(std::make_pair(pkDef->MapNo(), pkDef));
		if (false == kRet.second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Duplicate MapNo=") << pkDef->MapNo());
			DeleteDef(pkDef);
			pkDef = NULL;
		}

		++base_itor;
	}
	return bReturn;
}

int PgDefMapMgr::GetAbil(int const iMapNo, WORD const wType) const
{
	CONT_DEF::const_iterator itor_map = m_contDef.find(iMapNo);
	if (m_contDef.end() == itor_map)
	{
		return 0;
	}
	return (*itor_map).second->GetAbil(wType);
}