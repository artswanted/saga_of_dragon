#include "stdafx.h"
#include "Lohengrin/GameTime.h"
#include "Item.h"
#include "PgPlayer.h"
#include "PgJobSkillExpertness.h"
#include "PgJobSkill.h"
#include "tabledatamanager.h"

PgJobSkillExpertness::PgJobSkillExpertness()
	: m_kContExpertness(), m_iCurExhaustion(0), m_iCurBlessPoint(0), m_kLastResetExhaustionTime(), m_kLastResetBlessPointTime()
{
}

PgJobSkillExpertness::~PgJobSkillExpertness()
{
}

void PgJobSkillExpertness::Init(CEL::DB_DATA_ARRAY::const_iterator& itor, CEL::DB_RESULT_COUNT::const_iterator& count_itor, PgPremiumMgr const& rkPremium)
{
	Clear();

	int const iJobSkillExhautionCount = (*count_itor); ++count_itor;//
	if( 0 < iJobSkillExhautionCount )
	{
		(*itor).Pop(m_iCurExhaustion);		++itor;
		(*itor).Pop(m_kLastResetExhaustionTime);		++itor;
		(*itor).Pop(m_iCurBlessPoint);		++itor;
		(*itor).Pop(m_kLastResetBlessPointTime);		++itor;
	}
	else
	{
		g_kEventView.GetLocalTime(m_kLastResetExhaustionTime);
		g_kEventView.GetLocalTime(m_kLastResetBlessPointTime);
	}

	int iJobSkillExpertnessCount = (*count_itor); ++count_itor;//
	while( iJobSkillExpertnessCount-- )
	{
		int iSkillNo = 0, iExpertness = 0;
		(*itor).Pop( iSkillNo );		++itor;
		(*itor).Pop( iExpertness );		++itor;
		Set(iSkillNo, iExpertness);
	}

	JobSkillExpertnessUtil::ResetExhaustion(rkPremium, *this);
}
	
void PgJobSkillExpertness::Set(KEY_TYPE const& kJobSkillNo, VALUE_TYPE const& kValue)
{// 숙련도값 설정

	CONT_DEF_JOBSKILL_SKILL const* pkDefJSSkill = NULL;
	g_kTblDataMgr.GetContDef(pkDefJSSkill);

	CONT_DEF_JOBSKILL_SKILL::const_iterator iter = pkDefJSSkill->find(kJobSkillNo);
	if( pkDefJSSkill->end() == iter )
	{
		return;
	}

	if( IsHave(kJobSkillNo) )
	{
		switch( (*iter).second.eJobSkill_Type )
		{
			case JST_1ST_SUB:
			case JST_2ND_SUB:
				{
					return Set((*iter).second.i01NeedParent_JobSkill_No, kValue); // 공유되는 스킬의 숙련도로 재귀호출
				}break;
			case JST_1ST_MAIN:
			case JST_2ND_MAIN:
			case JST_3RD_MAIN:
			default:
				{
					// pass through
				}break;
		}
	}

	auto kRet = m_kContExpertness.insert(std::make_pair(kJobSkillNo, kValue));
	if(!kRet.second)
	{// 실패(이미 존재하면)
		kRet.first->second = kValue;	//대입 해준다
	}
}

bool PgJobSkillExpertness::Get(KEY_TYPE const& kJobSkillNo, PgJobSkillExpertness::VALUE_TYPE& rkOut) const
{// 숙련도값 얻기

	CONT_DEF_JOBSKILL_SKILL const* pkDefJSSkill = NULL;
	g_kTblDataMgr.GetContDef(pkDefJSSkill);

	CONT_DEF_JOBSKILL_SKILL::const_iterator iter = pkDefJSSkill->find(kJobSkillNo);
	if( pkDefJSSkill->end() == iter )
	{
		return false;
	}

	if( IsHave(kJobSkillNo) )
	{
		switch( (*iter).second.eJobSkill_Type )
		{
			case JST_1ST_SUB:
			case JST_2ND_SUB:
				{
					return Get((*iter).second.i01NeedParent_JobSkill_No, rkOut); // 공유되는 스킬의 숙련도로 재귀호출
				}break;
			case JST_1ST_MAIN:
			case JST_2ND_MAIN:
			case JST_3RD_MAIN:
			default:
				{
					// pass through
				}break;
		}
	}

	CONT_EXPERTNESS::const_iterator kItor = m_kContExpertness.find(kJobSkillNo);
	if( m_kContExpertness.end() == kItor )
	{
		rkOut = 0;
		return false;
	}
	rkOut = kItor->second;
	return true;
}
int PgJobSkillExpertness::Get(KEY_TYPE const& kJobSkillNo) const
{
	int iExpertness = 0;
	if( Get(kJobSkillNo, iExpertness) )
	{
		return iExpertness;
	}
	return 0;
}
bool PgJobSkillExpertness::IsHave(KEY_TYPE const& kJobSkillNo) const
{
	return m_kContExpertness.end() != m_kContExpertness.find(kJobSkillNo);
}
PgJobSkillExpertness::CONT_EXPERTNESS PgJobSkillExpertness::GetAllSkillExpertness() const
{
	CONT_EXPERTNESS kNewExpertness;
	CONT_EXPERTNESS::const_iterator iter = m_kContExpertness.begin();
	while( m_kContExpertness.end() != iter )
	{
		int const iSkillNo = (*iter).first;
		kNewExpertness.insert( std::make_pair(iSkillNo, Get(iSkillNo)) );
		++iter;
	}
	return kNewExpertness;
}

bool PgJobSkillExpertness::Remove(KEY_TYPE const& kJobSkillNo)
{// 해당 직업 스킬의 숙련도 제거
	return m_kContExpertness.erase(kJobSkillNo) ? true : false;
}

void PgJobSkillExpertness::RemoveAll()
{// 컨테이너 비우기
	CONT_EXPERTNESS kTemp;
	kTemp.swap(m_kContExpertness);
}
void PgJobSkillExpertness::Clear()
{// 컨테이너 비우기
	m_kLastResetBlessPointTime.Clear();
	m_kLastResetExhaustionTime.Clear();
	RemoveAll();
	m_iCurExhaustion = 0;
	m_iCurBlessPoint = 0;
}

void PgJobSkillExpertness::WriteToPacket(BM::Stream & kPacket) const
{
	kPacket.Push( m_kLastResetExhaustionTime );
	kPacket.Push( m_kLastResetBlessPointTime );
	PU::TWriteTable_AA(kPacket, m_kContExpertness);
	kPacket.Push( m_iCurExhaustion );
	kPacket.Push( m_iCurBlessPoint );
}

void PgJobSkillExpertness::ReadFromPacket(BM::Stream & kPacket)
{
	RemoveAll();
	kPacket.Pop( m_kLastResetExhaustionTime );
	kPacket.Pop( m_kLastResetBlessPointTime );
	PU::TLoadTable_AA(kPacket, m_kContExpertness);
	kPacket.Pop( m_iCurExhaustion );
	kPacket.Pop( m_iCurBlessPoint );
}
void PgJobSkillExpertness::CopyTo(PgJobSkillExpertness& rhs) const
{
	BM::Stream kPacket;
	WriteToPacket(kPacket);
	rhs.ReadFromPacket(kPacket);
}

namespace JobSkillUtil
{
	//
	bool IsJobSkill(int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill)
	{
		return rkContDefJobSkill.end() != rkContDefJobSkill.find(iSkillNo);
	}
	bool IsJobSkill(int const iSkillNo)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill = NULL;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		if( !pkContDefJobSkill )
		{
			return false;
		}
		return IsJobSkill(iSkillNo, *pkContDefJobSkill);
	}
	bool GetType(int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, int& rkOutGatherType, EJobSkillType& reOutJobSkillType)
	{
		CONT_DEF_JOBSKILL_SKILL::const_iterator find_iter = rkContDefJobSkill.find(iSkillNo);
		if( rkContDefJobSkill.end() != find_iter )
		{
			CONT_DEF_JOBSKILL_SKILL::mapped_type const& rkDefJobSkill = (*find_iter).second;
			rkOutGatherType = rkDefJobSkill.iGatherType;
			reOutJobSkillType = rkDefJobSkill.eJobSkill_Type;
			return true;
		}
		return 0;
	}
	int GetGatherType(int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill)
	{
		int iGatherType = 0;
		EJobSkillType eJobSkillType = JST_NONE;
		GetType(iSkillNo, rkContDefJobSkill, iGatherType, eJobSkillType);
		return iGatherType;
	}
	EJobSkillType GetJobSkillType(int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill)
	{
		int iGatherType = 0;
		EJobSkillType eJobSkillType = JST_NONE;
		GetType(iSkillNo, rkContDefJobSkill, iGatherType, eJobSkillType);
		return eJobSkillType;
	}

	int GetJobSkillNo(int const eGatherType, EJobSkillType const eSkillType, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill)
	{
		CONT_DEF_JOBSKILL_SKILL::const_iterator find_iter = rkContDefJobSkill.begin();
		while( rkContDefJobSkill.end() != find_iter )
		{
			CONT_DEF_JOBSKILL_SKILL::mapped_type const &kElement = find_iter->second;
			if( eGatherType == kElement.iGatherType 
				&& eSkillType == kElement.eJobSkill_Type )
			{
				return find_iter->first;
			}
			++find_iter;
		}

		return 0;
	}

	bool CheckJobSkillExpertness(PgPlayer& rkCaster, int const iNeedSkill, int const iNeedExpertness)
	{
		if( 0 < iNeedSkill
		&&	0 < iNeedExpertness )
		{
			PgJobSkillExpertness const& rkJobSkillExpertness = rkCaster.JobSkillExpertness();
			PgJobSkillExpertness::VALUE_TYPE iCurExpertness = 0;
			if( rkJobSkillExpertness.Get(iNeedSkill, iCurExpertness)
			&&	iNeedExpertness <= iCurExpertness )
			{
				return true;
			}
			BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_NEED_SKILL_EXPERTNESS);
			kPacket.Push( iNeedSkill );
			kPacket.Push( iNeedExpertness );
			rkCaster.Send(kPacket);
			return false;
		}
		return true;
	}
};

namespace JobSkillExpertnessUtil
{
	//
	bool Set(PgJobSkillExpertness& kExpertness, PgJobSkillExpertness::KEY_TYPE const& kJobSkillNo, PgJobSkillExpertness::VALUE_TYPE const& kSetVal)
	{// 숙련도 값 증가 시키기기 위해
		if( !JobSkillUtil::IsJobSkill(kJobSkillNo) )
		{
			return false;
		}

		kExpertness.Set(kJobSkillNo, kSetVal); // 증가 시킨다
		return true;
	}
	bool Inc(int const iCharLevel, PgJobSkillExpertness& kExpertness, PgJobSkillExpertness::KEY_TYPE const& kJobSkillNo, PgJobSkillExpertness::VALUE_TYPE const& kIncVal)
	{// 숙련도 값 증가 시키기기 위해
		if( !JobSkillUtil::IsJobSkill(kJobSkillNo) )
		{
			return false;
		}

		PgJobSkillExpertness::VALUE_TYPE kCurValue = 0;
		if( true == kExpertness.Get(kJobSkillNo, kCurValue) )
		{// 해당 직업 스킬이 존재 하면
			int const iMaxExpertness = GetMaxExpertness(iCharLevel, kJobSkillNo);
			if( iMaxExpertness >= kCurValue+kIncVal )
			{
				kExpertness.Set(kJobSkillNo, kCurValue+kIncVal); // 증가 시킨다
				return true;
			}
		}
		return false;
	}
	bool Del(PgJobSkillExpertness& kExpertness, PgJobSkillExpertness::KEY_TYPE const& kJobSkillNo)
	{
		if( !JobSkillUtil::IsJobSkill(kJobSkillNo) )
		{
			return false;
		}
		PgJobSkillExpertness::VALUE_TYPE kCurValue = 0;
		if( true == kExpertness.Remove(kJobSkillNo) )
		{
			return true;
		}
		return false;
	}

	//
	__int64 DiffExhaustionTime(BM::DBTIMESTAMP_EX const& rkUserTime, BM::DBTIMESTAMP_EX const& rkLocalTime)
	{
		__int64 iUserTime = 0;
		__int64 iLocalTime = 0;
		CGameTime::DBTimeEx2SecTime(rkUserTime, iUserTime, CGameTime::DEFAULT);
		CGameTime::DBTimeEx2SecTime(rkLocalTime, iLocalTime, CGameTime::DEFAULT);
		return iLocalTime - iUserTime;
	}
	bool IsCanResetExhaustion(PgJobSkillExpertness& kExpertness) // 그라운드에서만 쓰인다(둘다 체크하자)
	{
		if( 0 == g_kEventView.VariableCont().iJobSkill_ExhaustionGap )
		{
			return false;
		}
		__int64 const iGap = CGameTime::MINUTE * g_kEventView.VariableCont().iJobSkill_ExhaustionGap;
		BM::DBTIMESTAMP_EX kLocalTime;
		if( (g_kEventView.GetLocalTime(kLocalTime) )
		&&  (iGap < DiffExhaustionTime(kExpertness.LastResetExhaustionTime(), kLocalTime))
		||	(iGap < DiffExhaustionTime(kExpertness.LastResetBlessPointTime(), kLocalTime)) )
		{
			return true;
		}
		return false;
	}
	bool AddExhaustion(PgPremiumMgr const& rkPremium, PgJobSkillExpertness& kExpertness, int const iAddExhaustion)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill = NULL;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness = NULL;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);
		int const iMaxExhaustion = GetMaxExhaustion_1ST(rkPremium, kExpertness.GetAllSkillExpertness(), *pkContDefJobSkill, *pkContDefJobSkillExpertness); // 1차 주 스킬만 피로도가 있다
		int iNewExhaustion = kExpertness.CurExhaustion() + iAddExhaustion;
		iNewExhaustion = std::min(iMaxExhaustion, iNewExhaustion);
		iNewExhaustion = std::max(0, iNewExhaustion);
		kExpertness.CurExhaustion( iNewExhaustion );
		return true;
	}
	bool AddBlessPoint(PgPremiumMgr const& rkPremium, PgJobSkillExpertness& kExpertness, int const iAddBlessPoint)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill = NULL;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness = NULL;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);
		int const iMaxBlessPoint = GetMaxExhaustion_2ND(rkPremium, kExpertness.GetAllSkillExpertness(), *pkContDefJobSkill, *pkContDefJobSkillExpertness); // 2차 주 스킬만 축복게이지가 있다
		int iNewBlessPoint = kExpertness.CurBlessPoint() + iAddBlessPoint;
		iNewBlessPoint = std::min(iMaxBlessPoint, iNewBlessPoint);
		iNewBlessPoint = std::max(0, iNewBlessPoint);
		kExpertness.CurBlessPoint( iNewBlessPoint );
		return true;
	}
	bool ResetExhaustion(PgPremiumMgr const& rkPremium, PgJobSkillExpertness& kExpertness, __int64 const iForceResetMinute)
	{
		if( 0 == g_kEventView.VariableCont().iJobSkill_ExhaustionGap )
		{
			return false;
		}
		CONT_DEF_JOBSKILL_SKILL const* pkDefJSSkill = NULL;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkDefJSExpertness = NULL;
		g_kTblDataMgr.GetContDef(pkDefJSSkill);
		g_kTblDataMgr.GetContDef(pkDefJSExpertness);
		__int64 const iRestoreGap = CGameTime::MINUTE * g_kEventView.VariableCont().iJobSkill_ExhaustionGap;
		BM::DBTIMESTAMP_EX kLocalTime;
		if( !g_kEventView.GetLocalTime(kLocalTime) )
		{
			return false;
		}
		bool bRet = false;
		{
			__int64 const iDiffTime = DiffExhaustionTime(kExpertness.LastResetExhaustionTime(), kLocalTime);
			if( iRestoreGap < iDiffTime
			||	iForceResetMinute )
			{
				int const iMaxExhaustion = GetMaxExhaustion_1ST(rkPremium, kExpertness.GetAllSkillExpertness(), *pkDefJSSkill, *pkDefJSExpertness);
				int const iRestoreExhaustion = iMaxExhaustion / g_kEventView.VariableCont().iJobSkill_ExhaustionRestore;
				int iResultExhaustion = 0;
				if( iForceResetMinute )
				{
					iResultExhaustion = static_cast< int >(CGameTime::MINUTE * iForceResetMinute * iRestoreExhaustion / iRestoreGap); // GM Command에 의해서
					kExpertness.LastResetExhaustionTime( kLocalTime );
				}
				else
				{
					__int64 const iRemainTime = iDiffTime % iRestoreGap;
					iResultExhaustion = static_cast< int >((iDiffTime-iRemainTime) * iRestoreExhaustion / iRestoreGap); // Map서버의 Tick코드에 의해서

					BM::PgPackedTime kTempTime(kLocalTime);
					CGameTime::AddTime(kTempTime, -iRemainTime);
					kExpertness.LastResetExhaustionTime( BM::DBTIMESTAMP_EX(kTempTime) );
				}
				kExpertness.CurExhaustion( std::max(kExpertness.CurExhaustion() - iResultExhaustion, 0) );
				bRet = true;
			}
		}
		{
			__int64 const iDiffTime = DiffExhaustionTime(kExpertness.LastResetBlessPointTime(), kLocalTime);
			if( iRestoreGap < iDiffTime
				||	iForceResetMinute )
			{
				int const iMaxBlessPoint = GetMaxExhaustion_2ND(rkPremium, kExpertness.GetAllSkillExpertness(), *pkDefJSSkill, *pkDefJSExpertness);
				int const iRestoreBlessPoint = iMaxBlessPoint / g_kEventView.VariableCont().iJobSkill_ExhaustionRestore;
				int iResultBlessPoint = 0;
				if( iForceResetMinute )
				{
					iResultBlessPoint = static_cast< int >(CGameTime::MINUTE * iForceResetMinute * iRestoreBlessPoint / iRestoreGap); //
					kExpertness.LastResetBlessPointTime( kLocalTime );
				}
				else
				{
					__int64 const iRemainTime = iDiffTime % iRestoreGap;
					iResultBlessPoint = static_cast< int >((iDiffTime-iRemainTime) * iRestoreBlessPoint / iRestoreGap); // 

					BM::PgPackedTime kTempTime(kLocalTime);
					CGameTime::AddTime(kTempTime, -iRemainTime);
					kExpertness.LastResetBlessPointTime( BM::DBTIMESTAMP_EX(kTempTime) );
				}
				kExpertness.CurBlessPoint( std::max(kExpertness.CurBlessPoint() - iResultBlessPoint, 0) );
				bRet = true;
			}
		}
		return bRet;
	}
	bool InitExhaustion(PgJobSkillExpertness& kExpertness)
	{
		BM::DBTIMESTAMP_EX kLocalTime;
		if( !g_kEventView.GetLocalTime(kLocalTime) )
		{
			return false;
		}
		kExpertness.CurExhaustion( 0 );
		kExpertness.LastResetExhaustionTime( kLocalTime );
		return true;
	}
	bool InitBlessPoint(PgJobSkillExpertness& kExpertness)
	{
		BM::DBTIMESTAMP_EX kLocalTime;
		if( !g_kEventView.GetLocalTime(kLocalTime) )
		{
			return false;
		}
		kExpertness.CurBlessPoint( 0 );
		kExpertness.LastResetBlessPointTime(kLocalTime);
		return true;
	}

	//
	bool Update(DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkItemChange, BM::Stream& rkAddonData, PgPlayer& rkPlayer) // Recv From Contents Server(Map / Client)
	{
		switch( rkItemChange.State() )
		{
		case DISCT_JOBSKILL_SET_SKILL_EXPERTNESS:
			{
				SPMOD_JobSkillExpertness kData;
				kData.ReadFromPacket( rkAddonData );
				Set( rkPlayer.JobSkillExpertness(), kData.SkillNo(), kData.Expertness() );
			}break;
		case DISCT_JOBSKILL_DEL_SKILL_EXPERTNESS:
			{
				int iSkillNo = 0;
				rkAddonData.Pop( iSkillNo );
				Del( rkPlayer.JobSkillExpertness(), iSkillNo );
			}break;
		case DISCT_JOBSKILL_SAVE_EXHAUSTION:
			{
				int iCurExhaustion = 0, iCurBlessPoint = 0;
				BM::DBTIMESTAMP_EX kLastResetExhaustionTime;
				BM::DBTIMESTAMP_EX kLastResetBlessPointTime;
				rkAddonData.Pop( iCurExhaustion );
				rkAddonData.Pop( iCurBlessPoint );
				rkAddonData.Pop( kLastResetExhaustionTime );
				rkAddonData.Pop( kLastResetBlessPointTime );

				rkPlayer.JobSkillExpertness().CurExhaustion(iCurExhaustion);
				rkPlayer.JobSkillExpertness().CurBlessPoint(iCurBlessPoint);
				rkPlayer.JobSkillExpertness().LastResetExhaustionTime(kLastResetExhaustionTime);
				rkPlayer.JobSkillExpertness().LastResetBlessPointTime(kLastResetBlessPointTime);
			}break;
		default:
			{
				return false;
			}break;
		}
		return true;
	}

	//
	bool FindExpertnessArea(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness, CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type& rkOut)
	{
		if( !JobSkillUtil::IsJobSkill(iSkillNo, rkContDefJobSkill) )
		{
			return false;
		}
		
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill = NULL;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		if( !pkContDefJobSkill )
		{
			return false;
		}
		int const iGatherType = JobSkillUtil::GetGatherType(iSkillNo, *pkContDefJobSkill);
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::const_iterator begin_iter = rkContDefJobSkillExpertness.end();
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::const_iterator iter = rkContDefJobSkillExpertness.begin();
		while( rkContDefJobSkillExpertness.end() != iter )
		{
			CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type const& rkDefExpertness = (*iter);
			if( rkDefExpertness.iSkillNo == iSkillNo )
			{
				if( rkContDefJobSkillExpertness.end() == begin_iter )
				{
					begin_iter = iter; // 시작점 기록
				}
				if( JSTT_ALL == iGatherType )
				{
					rkOut = (*iter);
					return true;
				}
				if( rkDefExpertness.iSkill_Expertness_Min <= iExpertness
				&&	rkDefExpertness.iSkill_Expertness_Max >= iExpertness )
				{
					rkOut = (*iter);
					return true;
				}
			}
			else
			{
				if( rkContDefJobSkillExpertness.end() != begin_iter )
				{
					break;
				}
			}
			++iter;
		}
		return false;
	}

	//다음 숙련도 구간의 최소값 리턴
	int FindNextExpertness_Min(int const iSkillNo, int const iExpertness)
	{
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness;
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);
		
		if( !JobSkillUtil::IsJobSkill(iSkillNo) )
		{
			return false;
		}

		std::list<int> kContExpertness;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::const_iterator find_iter = pkContDefJobSkillExpertness->begin();
		while( pkContDefJobSkillExpertness->end() != find_iter)
		{
			CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type const& rkExpertness = (*find_iter);
			if( rkExpertness.iSkillNo == iSkillNo)
			{	
				kContExpertness.push_back(rkExpertness.iSkill_Expertness_Min);
			}
			++find_iter;
		}
		kContExpertness.sort();
		std::list<int>::const_iterator exp_iter = kContExpertness.begin();
		
		int iNextExpertness = iExpertness;
		while( kContExpertness.end() != exp_iter)
		{
			if(iExpertness >= (*exp_iter) )
			{
				std::list<int>::const_iterator Next_iter = exp_iter;
				++Next_iter;
				if(kContExpertness.end() != Next_iter)
				{//다음이 있으면
					if(iExpertness < (*Next_iter) )
					{
						iNextExpertness = (*Next_iter);
					}
				}
				else
				{//없으면 지금
					iNextExpertness = (*exp_iter);
				}
			}
			++exp_iter;
		}
		return iNextExpertness;
	}

	bool FindExpertnessArea(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type& rkOut)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkDefJSSkill = NULL;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkDefJSSkillExpertness = NULL;
		g_kTblDataMgr.GetContDef(pkDefJSSkill);
		g_kTblDataMgr.GetContDef(pkDefJSSkillExpertness);
		return FindExpertnessArea(iSkillNo, iExpertness, *pkDefJSSkill, *pkDefJSSkillExpertness, rkOut);
	}

	CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type FindExpertnessArea(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kDefExpertness;
		FindExpertnessArea(iSkillNo, iExpertness, rkContDefJobSkill, rkContDefJobSkillExpertness, kDefExpertness);
		return kDefExpertness;
	}

	//
	int GetBasicTurnTime(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kDefExpertness;
		if( FindExpertnessArea(iSkillNo, iExpertness, rkContDefJobSkill, rkContDefJobSkillExpertness, kDefExpertness) )
		{
			return kDefExpertness.iBasic_Turn_Time;
		}
		return 0;
	}
	int GetBasicTurnTime(int const iSkillNo, int const iExpertness)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill = NULL;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness = NULL;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);
		return GetBasicTurnTime(iSkillNo, iExpertness, *pkContDefJobSkill, *pkContDefJobSkillExpertness);
	}

	//
	int GetExpertnessUp(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kDefExpertness;
		if( !FindExpertnessArea(iSkillNo, iExpertness, rkContDefJobSkill, rkContDefJobSkillExpertness, kDefExpertness) )
		{
			return 0;
		}

		int iAddExpertness = 0;
		int const iRandRet = BM::Rand_Range(kDefExpertness.iExpertness_Gain_TotalProbability);
		if( kDefExpertness.iExpertness_Gain_Probability >= iRandRet )
		{
			iAddExpertness = BM::Rand_Range(kDefExpertness.iExpertness_Gain_Max, kDefExpertness.iExpertness_Gain_Min);
		}
		return iAddExpertness;
	}

	//
	int GetMaxExpertness(int const iCharLevel, int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		if( !JobSkillUtil::IsJobSkill(iSkillNo, rkContDefJobSkill) )
		{
			return 0;
		}

		int iHeightestExpertness = 0, iTempHeightestExpertness = 0;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::const_iterator begin_iter = rkContDefJobSkillExpertness.end();
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::const_iterator iter = rkContDefJobSkillExpertness.begin();
		while( rkContDefJobSkillExpertness.end() != iter )
		{
			CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type const& rkDefExpertness = (*iter);
			if( rkDefExpertness.iSkillNo == iSkillNo )
			{
				if( rkContDefJobSkillExpertness.end() == begin_iter )
				{
					begin_iter = iter; // 시작점 기록
				}
				if( rkDefExpertness.iNeed_CharLevel <= iCharLevel )
				{
					iHeightestExpertness = std::max(iHeightestExpertness, rkDefExpertness.iSkill_Expertness_Max); // 최대값
				}
				iTempHeightestExpertness = std::max(iHeightestExpertness, rkDefExpertness.iSkill_Expertness_Max); // 최대값
			}
			else
			{
				if( rkContDefJobSkillExpertness.end() != begin_iter )
				{
					break; // 검사 중단
				}
			}
			++iter;
		}
		if( 0 == iHeightestExpertness )
		{
			return iTempHeightestExpertness;
		}
		return iHeightestExpertness;
	}
	int GetMaxExpertness(int const iCharLevel, int const iSkillNo, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill)
	{
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness = NULL; 
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);
		return GetMaxExpertness(iCharLevel, iSkillNo, rkContDefJobSkill, *pkContDefJobSkillExpertness);
	}
	int GetMaxExpertness(int const iCharLevel, int const iSkillNo, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill = NULL;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		return GetMaxExpertness(iCharLevel, iSkillNo, *pkContDefJobSkill, rkContDefJobSkillExpertness);
	}
	int GetMaxExpertness(int const iCharLevel, int const iSkillNo)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill = NULL;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness = NULL; 
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);
		return GetMaxExpertness(iCharLevel, iSkillNo, *pkContDefJobSkill, *pkContDefJobSkillExpertness);
	}
	int GetBiggestMaxExpertness(int const iSkillNo)
	{
		if( !JobSkillUtil::IsJobSkill(iSkillNo) )
		{
			return 0;
		}
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness = NULL; 
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::const_iterator find_iter = pkContDefJobSkillExpertness->begin();
		int iBiggestMaxExpertness = 0;
		while( find_iter != pkContDefJobSkillExpertness->end() )
		{
			CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type const &rkExpertness = (*find_iter);
			if( rkExpertness.iSkillNo == iSkillNo)
			{
				if(iBiggestMaxExpertness < rkExpertness.iSkill_Expertness_Max)
				{
					iBiggestMaxExpertness = rkExpertness.iSkill_Expertness_Max;
				}
			}
			++find_iter;
		}
		return iBiggestMaxExpertness;
	}

	//
	int GetUseExhaustion(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kDefExpertnessArea;
		if( FindExpertnessArea(iSkillNo, iExpertness, rkContDefJobSkill, rkContDefJobSkillExpertness, kDefExpertnessArea) )
		{
			return kDefExpertnessArea.iUse_Exhaustion;
		}
		return 1; // 기본값 1?
	}
	//
	int GetUseExhaustion(int const iSkillNo, int const iExpertness)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness;
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);

		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kDefExpertnessArea;
		if( FindExpertnessArea(iSkillNo, iExpertness, *pkContDefJobSkill, *pkContDefJobSkillExpertness, kDefExpertnessArea) )
		{
			return kDefExpertnessArea.iUse_Exhaustion;
		}
		return 1; // 기본값 1?
	}
	int GetMaxExhaustion(int const iSkillNo, int const iExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kDefExpertnessArea;
		if( FindExpertnessArea(iSkillNo, iExpertness, rkContDefJobSkill, rkContDefJobSkillExpertness, kDefExpertnessArea) )
		{
			return kDefExpertnessArea.iMax_Exhaustion;
		}
		return 0;
	}
	int GetBiggestMaxExhaustion(PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness, EJobSkillType const eFilterType)
	{
		int iBiggestMaxExhaustion = 0;
		typedef PgJobSkillExpertness::CONT_EXPERTNESS CONT_EXPERTNESS;
		CONT_EXPERTNESS::const_iterator iter = rkContExpertness.begin();
		while( rkContExpertness.end() != iter )
		{
			int const iSkillNo = (*iter).first;

			CONT_DEF_JOBSKILL_SKILL::const_iterator def_iter = rkContDefJobSkill.find(iSkillNo);
			if( rkContDefJobSkill.end() != def_iter )
			{
				//if( eFilterType == JST_NONE
				//||	eFilterType == (*def_iter).second.eJobSkill_Type )
				//{//직업기술 통합하면서 스킬타입 전체로 검사하도록 수정
					iBiggestMaxExhaustion = std::max( GetMaxExhaustion(iSkillNo, (*iter).second, rkContDefJobSkill, rkContDefJobSkillExpertness), iBiggestMaxExhaustion );
				//}
			}
			++iter;
		}
		return iBiggestMaxExhaustion;
	}
	
	int GetBiggestMaxExhaustion(PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness, EJobSkillType const eSkillType)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill = NULL;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const *pkContDefJobSkillExpertness = NULL;
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);

		return GetBiggestMaxExhaustion(rkContExpertness, *pkContDefJobSkill, *pkContDefJobSkillExpertness, eSkillType);
	}
	//
	int GetUseDuration(int const iSkillNo, int const iExpertness, int const iAddRate)
	{
		CONT_DEF_JOBSKILL_SKILL const* pkDefJobSkill = NULL;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkContDefJobSkillExpertness = NULL;
		g_kTblDataMgr.GetContDef(pkDefJobSkill);
		g_kTblDataMgr.GetContDef(pkContDefJobSkillExpertness);
		if( pkDefJobSkill && pkContDefJobSkillExpertness )
		{
			CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kDefExpertness;
			if( FindExpertnessArea(iSkillNo, iExpertness, *pkDefJobSkill, *pkContDefJobSkillExpertness, kDefExpertness) )
			{
				int const iDurationTotalRate = kDefExpertness.iUse_Duration_TotalProbability;
				int const iDurationRate = kDefExpertness.iUse_Duration_Probability * (ABILITY_RATE_VALUE + iAddRate) / ABILITY_RATE_VALUE;
				int const iRetDurationRate = BM::Rand_Range((iDurationTotalRate - 1), 0);
				if( iRetDurationRate < iDurationRate )
				{
					return kDefExpertness.iUse_Duration;
				}
			}
		}

		return 0;
	}

	//1차 최대 피로도
	int	GetMaxExhaustion_1ST(PgPremiumMgr const& rkPremiumMgr, PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness)
	{
		int iMax = 0;
		if(false==rkContExpertness.empty())
		{
			iMax = GetBiggestMaxExhaustion(rkContExpertness, JST_1ST_MAIN);
			if( S_PST_JS1_MaxExhaustion const* pkPremium = rkPremiumMgr.GetType<S_PST_JS1_MaxExhaustion>() )
			{
				iMax += pkPremium->iMax;
			}
		}
		return iMax;
	}

	int GetMaxExhaustion_1ST(PgPremiumMgr const& rkPremiumMgr, PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		int iMax = 0;
		if(false==rkContExpertness.empty())
		{
			iMax = GetBiggestMaxExhaustion(rkContExpertness, rkContDefJobSkill, rkContDefJobSkillExpertness, JST_1ST_MAIN);
			if( S_PST_JS1_MaxExhaustion const* pkPremium = rkPremiumMgr.GetType<S_PST_JS1_MaxExhaustion>() )
			{
				iMax += pkPremium->iMax;
			}
		}
		return iMax;
	}

	//2차 최대 축복게이지
	int GetMaxExhaustion_2ND(PgPremiumMgr const& rkPremiumMgr, PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness)
	{
		int iMax = 0;
		if(false==rkContExpertness.empty())
		{
			iMax = GetBiggestMaxExhaustion(rkContExpertness, JST_2ND_MAIN);
			if( S_PST_JS2_MaxBless const* pkPremium = rkPremiumMgr.GetType<S_PST_JS2_MaxBless>() )
			{
				iMax += pkPremium->iMax;
			}
		}
		return iMax;
	}

	int GetMaxExhaustion_2ND(PgPremiumMgr const& rkPremiumMgr, PgJobSkillExpertness::CONT_EXPERTNESS const& rkContExpertness, CONT_DEF_JOBSKILL_SKILL const& rkContDefJobSkill, CONT_DEF_JOBSKILL_SKILLEXPERTNESS const& rkContDefJobSkillExpertness)
	{
		int iMax = 0;
		if(false==rkContExpertness.empty())
		{
			iMax = GetBiggestMaxExhaustion(rkContExpertness, rkContDefJobSkill, rkContDefJobSkillExpertness, JST_2ND_MAIN);
			if( S_PST_JS2_MaxBless const* pkPremium = rkPremiumMgr.GetType<S_PST_JS2_MaxBless>() )
			{
				iMax += pkPremium->iMax;
			}
		}
		return iMax;
	}
};