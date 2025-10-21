#include "stdafx.h"
#include "Lohengrin/Dbtables.h"
#include "Variant/Global.h"
#include "PgJobSkillChecker.h"
#include "PgServerSetMgr.h"

namespace JSMgr
{
	PgJobSkillVerify::PgJobSkillVerify()
	{
		g_kTblDataMgr.GetContDef(m_pkDefJobSkill_Skill);
		g_kTblDataMgr.GetContDef(m_pkDefJobSkill_Expertness);
		g_kTblDataMgr.GetContDef(m_pkDefJobSkill_SaveIdx);
		g_kTblDataMgr.GetContDef(m_pkDefJobSkill_Tool);
		g_kTblDataMgr.GetContDef(m_pkDefJobSkill_LocationItem);
		g_kTblDataMgr.GetContDef(m_pkDefJobSkill_Probability_Bag);
		g_kTblDataMgr.GetContDef(m_pkDefJobSkill_Shop);
	}
	PgJobSkillVerify::~PgJobSkillVerify()
	{
	}
	bool PgJobSkillVerify::Check()
	{
		bool bRet = true;

		Check_Skill(bRet);
		Check_Expertness(bRet);
		Check_SaveIdx(bRet);
		Check_Tool(bRet);
		Check_Location(bRet);
		Check_Shop(bRet);
 
		if(!bRet)
		{
			ShowLogList();
		}
		return bRet;
	}

	void PgJobSkillVerify::Check_Skill(bool &rbRet)
	{
		//[TB_DefJobSkill_Skill].f_SkillNo 가 TB_DefSkill에 없다.
		CONT_DEF_JOBSKILL_SKILL::const_iterator find_iter = m_pkDefJobSkill_Skill->begin();
		while( find_iter != m_pkDefJobSkill_Skill->end() )
		{
			int const iSkillNo = (*find_iter).first;
			GET_DEF( CSkillDefMgr, kSkillDefMgr);
			CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef( iSkillNo );
			if(!pkSkillDef)
			{
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_Skill].[f_SkillNo] is not have SkillDef - #SKILLNO#");
				kLog.Replace(L"#SKILLNO#", iSkillNo);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			++find_iter;
		}
	}

	void PgJobSkillVerify::Check_SaveIdx(bool &rbRet)
	{
		//[TB_DefJobSkill_SaveIdx].f_Book_ItemNo 가 DefItem에 없는 경우 
		CONT_DEF_JOBSKILL_SAVEIDX::const_iterator find_iter = m_pkDefJobSkill_SaveIdx->begin();
		while( find_iter != m_pkDefJobSkill_SaveIdx->end() )
		{
			CONT_DEF_JOBSKILL_SAVEIDX::mapped_type const& rkDefSaveIdx = (*find_iter).second;
			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pItemDef = kItemDefMgr.GetDef(rkDefSaveIdx.iBookItemNo);
			if(!pItemDef)
			{
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_SaveIdx].[f_Book_ItemNo] is not have ItemDef - #ITEMNO#");
				kLog.Replace(L"#ITEMNO#", rkDefSaveIdx.iBookItemNo);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			int const iOneBytePerBit = 8;
			int const iLimitSaveIdx = MAX_DB_JOBKSILL_SAVEIDX_SIZE * iOneBytePerBit;
			if( rkDefSaveIdx.iSaveIdx >= iLimitSaveIdx )
			{
				BM::vstring kLog = BM::vstring() << L"[dbo.TB_DefJobSkill_SaveIdx].[f_SaveIdx][" << rkDefSaveIdx.iSaveIdx << L"] is can't over then " << iLimitSaveIdx;
				rbRet = false;
			}
			++find_iter;
		}
	}

	void PgJobSkillVerify::Check_Expertness(bool &rbRet)
	{
		//[TB_DefJobSkill_SkillExpertness].f_SkillNo 가 [TB_DefJobSkill_Skill].f_SkillNo에 없다. 
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::const_iterator find_iter = m_pkDefJobSkill_Expertness->begin();
		while( find_iter != m_pkDefJobSkill_Expertness->end() )
		{
			int const iSkillNo = (*find_iter).iSkillNo;
			CONT_DEF_JOBSKILL_SKILL::const_iterator Skill_iter = m_pkDefJobSkill_Skill->find(iSkillNo);
			if( Skill_iter == m_pkDefJobSkill_Skill->end() )
			{
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_SkillExpertness].[f_SkillNo] is not have [TB_DefJobSkill_Skill].[f_SkillNo] - #SKILLNO#");
				kLog.Replace(L"#SKILLNO#", iSkillNo);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			++find_iter;
		}
	}

	void PgJobSkillVerify::Check_Tool(bool &rbRet)
	{
		CONT_DEF_JOBSKILL_TOOL::const_iterator find_iter = m_pkDefJobSkill_Tool->begin();
		while( find_iter != m_pkDefJobSkill_Tool->end() )
		{
			int const iItemNo = (*find_iter).first;
			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pItemDef = kItemDefMgr.GetDef(iItemNo);
			if(!pItemDef)
			{//[TB_DefJobSkill_Tool].f_ItemNo 가 DefItem에 없는 경우
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_Tool].[f_ItemNo] is not have ItemDef - #ITEMNO#");
				kLog.Replace(L"#ITEMNO#", iItemNo);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			int const iResult_No_Min = (*find_iter).second.iResult_No_Min;
			int const iResult_No_Max = (*find_iter).second.iResult_No_Max;
			if( 0 >= iResult_No_Min )
			{//최소값이 0보다 작거나 같을 경우
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_Tool].[#ITEMNO#].[iResult_No_Min] is wrong : #MIN# =< 0");
				kLog.Replace(L"#ITEMNO#", iItemNo);
				kLog.Replace(L"#MIN#", iResult_No_Min);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			if( 0 >= iResult_No_Max )
			{//최대값이 0보다 작거나 같을 경우
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_Tool].[#ITEMNO#].[iResult_No_Max] is wrong : #MAX# =< 0");
				kLog.Replace(L"#ITEMNO#", iItemNo);
				kLog.Replace(L"#MAX#", iResult_No_Max);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			if( iResult_No_Max < iResult_No_Min )
			{// 최소값이 최대값보다 클 경우
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_Tool].[#ITEMNO#].[iResult_No_Min]&[iResult_No_Max] is worng : #MIN# > #MAX#");
				kLog.Replace(L"#ITEMNO#", iItemNo);
				kLog.Replace(L"#MIN#", iResult_No_Min);
				kLog.Replace(L"#MAX#", iResult_No_Max);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			if( iResult_No_Max > COUNT_JOBSKILL_RESULT_PROBABILITY_NO)
			{// 최대값이 정해진 최대 수치 번호보다 클 경우
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_Tool].[#ITEMNO#].[iResult_No_Max] is over Max size - Value : #VALUE#  COUNT_JOBSKILL_RESULT_PROBABILITY_NO : #MAX#");
				kLog.Replace(L"#ITEMNO#", iItemNo);
				kLog.Replace(L"#VALUE#", iResult_No_Max);
				kLog.Replace(L"#MAX#", COUNT_JOBSKILL_RESULT_PROBABILITY_NO);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			++find_iter;
		}
	}

	void PgJobSkillVerify::Check_Location(bool &rbRet)
	{
		//[TB_DefJobSkill_LocationItem].f_ResultProbability_No 가 [TB_DefJobSkill_Probability].f_No 에 없는 경우 
		CONT_DEF_JOBSKILL_LOCATIONITEM::const_iterator find_iter = m_pkDefJobSkill_LocationItem->begin();
		while( find_iter != m_pkDefJobSkill_LocationItem->end() )
		{
			CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type const &rkLocation = (*find_iter).second;
			for(int iCount = 0; iCount < 10; iCount++)
			{
				int const iResultProbability_No = rkLocation.iResultProbability_No[iCount];
				if( 0 != iResultProbability_No)
				{
					CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator bag_iter = m_pkDefJobSkill_Probability_Bag->find(iResultProbability_No);
					if( bag_iter == m_pkDefJobSkill_Probability_Bag->end())
					{
						BM::vstring kLog(L"[dbo.TB_DefJobSkill_LocationItem].[f_[#NO#]ResultProbability_No] is not have  [TB_DefJobSkill_Probability].[f_No] - #PROBABILITY#");
						kLog.Replace(L"#NO#", iCount);
						kLog.Replace(L"#PROBABILITY#", iResultProbability_No);
						std::wstring kstrLog = static_cast<std::wstring>(kLog);
						m_kContErrLog.push_back(kLog);
						rbRet = false;
					}
				}
			}
			++find_iter;
		}
	}

	void PgJobSkillVerify::Check_Shop(bool &rbRet)
	{
		//[TB_DefJobSkill_Shop].f_Job_Category 가 [TB_DefJobSkill_Skill].f_SkillNo에 없다. 
		//[TB_DefJobSkill_Shop].f_ItemNo 가 DefItem에 없는 경우
		CONT_DEF_JOBSKILL_SHOP::const_iterator find_iter = m_pkDefJobSkill_Shop->begin();
		while( find_iter != m_pkDefJobSkill_Shop->end() )
		{
			int const iJobCategory = (*find_iter).second.iCategory;
			CONT_DEF_JOBSKILL_SKILL::const_iterator Skill_iter = m_pkDefJobSkill_Skill->find(iJobCategory);
			if( Skill_iter == m_pkDefJobSkill_Skill->end() )
			{
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_Shop].[f_Job_Category] is not have  [TB_DefJobSkill_Skill].[f_SkillNo] - #CATEGORY#");
				kLog.Replace(L"#CATEGORY#", iJobCategory);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			int const iItemNo = (*find_iter).second.iItemNo;
			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pItemDef = kItemDefMgr.GetDef(iItemNo);
			if(!pItemDef)
			{
				BM::vstring kLog(L"[dbo.TB_DefJobSkill_Shop].[f_ItemNo] is not have ItemDef - #ITEMNO#");
				kLog.Replace(L"#ITEMNO#", iItemNo);
				std::wstring kstrLog = static_cast<std::wstring>(kLog);
				m_kContErrLog.push_back(kLog);
				rbRet = false;
			}
			++find_iter;
		}		
	}

	void PgJobSkillVerify::ShowLogList()
	{
		std::list<std::wstring>::const_iterator err_iter = m_kContErrLog.begin();
		while(err_iter != m_kContErrLog.end() )
		{
			std::wstring const kLog = (*err_iter);
			if(kLog.size() != 0)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << kLog.c_str() );
			}
			++err_iter;
		}
	}
}