#include "stdafx.h"
#include "Cashshop.h"
#include "BM/LocalMgr.h"

namespace CashShopUtil
{
	bool IsCanUseMileageServiceRegion()
	{
		// 주의!! g_kLocal.NationCode()가 아닌, g_kLocal.ServiceRegion()를 넣어야 함
		switch(g_kLocal.ServiceRegion())
		{
		case LOCAL_MGR::NC_TAIWAN:
		case LOCAL_MGR::NC_CHINA:
		case LOCAL_MGR::NC_DEVELOP:// 개발 버전도 마일리지 활성화 시켜 놓자
		case LOCAL_MGR::NC_SINGAPORE:
		case LOCAL_MGR::NC_THAILAND:
		case LOCAL_MGR::NC_INDONESIA:
		case LOCAL_MGR::NC_VIETNAM:
		case LOCAL_MGR::NC_PHILIPPINES:
		//case LOCAL_MGR::NC_EU:
		//case LOCAL_MGR::NC_JAPAN:
			{
				return true; // 사용함
			}break;
		default:
			{
			}break;
		}
		return false; // 사용하지 않음
	}
}