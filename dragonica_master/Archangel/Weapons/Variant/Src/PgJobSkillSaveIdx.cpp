#include "stdafx.h"
#include "Item.h"
#include "PgPlayer.h"
#include "PgJobSkillSaveIdx.h"
#include "tabledatamanager.h"

namespace JobSkillSaveIdxUtil
{
	bool Check(CONT_DEF_JOBSKILL_SAVEIDX const& kDefJobSkillSaveIdx, int const iSaveIdx)
	{
		if( 0 < iSaveIdx )
		{
			return ( kDefJobSkillSaveIdx.end() != kDefJobSkillSaveIdx.find( iSaveIdx ) );
		}
		return true; // Def 설정이 없으면 체크도 통과
	}
	
	bool IsUseableSaveIdx( PgPlayer const * pkPlayer, int const iSaveIdx )
	{
		if( !iSaveIdx )
		{	return false;	}
		if( !pkPlayer )
		{	return false;	}
		CONT_DEF_JOBSKILL_SAVEIDX const* pkDefJobSkillSaveIdx = NULL;
		g_kTblDataMgr.GetContDef(pkDefJobSkillSaveIdx);
		if( !pkDefJobSkillSaveIdx )
		{	return false;	}
		CONT_DEF_JOBSKILL_SAVEIDX::const_iterator save_it = pkDefJobSkillSaveIdx->find( iSaveIdx );
		if( pkDefJobSkillSaveIdx->end() == save_it )
		{	return false;	}
		CONT_DEF_JOBSKILL_SAVEIDX::mapped_type rkSaveIdxInfo = (*save_it).second;
		int const iExpertness01 = pkPlayer->JobSkillExpertness().Get(rkSaveIdxInfo.iNeedSkillNo01);
		int const iExpertness02 = pkPlayer->JobSkillExpertness().Get(rkSaveIdxInfo.iNeedSkillNo02);
		int const iExpertness03 = pkPlayer->JobSkillExpertness().Get(rkSaveIdxInfo.iNeedSkillNo03);
		if(    (iExpertness01 >= rkSaveIdxInfo.iNeedSkillExpertness01) 
			&& (iExpertness02 >= rkSaveIdxInfo.iNeedSkillExpertness02)
			&& (iExpertness03 >= rkSaveIdxInfo.iNeedSkillExpertness03) 
			&& (0!=rkSaveIdxInfo.iShowUser) )
		{
			return true;
		}
		return false;
	}
}