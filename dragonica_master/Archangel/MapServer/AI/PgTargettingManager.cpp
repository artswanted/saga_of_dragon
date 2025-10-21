#include "stdafx.h"
#include "PgTargettingManager.h"
#include "PgGround.h"
#include "Global.h"

namespace TargettingUtil
{
	inline int GetAbil(CSkillDef const* pkDef, CUnit* pkUnit, int const iAbil)
	{
		int iValue = 0;
		if(pkDef)	//먼저 스킬데프에서 찾아보자
		{
			iValue = pkDef->GetAbil(iAbil);
		}

		if(0==iValue && pkUnit)	//값이 없으면 unit에서 찾아보자
		{
			iValue = pkUnit->GetAbil(iAbil);
		}

		return iValue;
	}

	inline int const GetSecType(CUnit * pkUnit, CSkillDef const* pkDef, int const iSecondType)	
	{
		if(iSecondType > 0)
		{
			return iSecondType;
		}

		return GetAbil(pkDef, pkUnit, AT_AI_TARGETTING_SECOND_TYPE);
	}
}

CUnit* PgIBaseTargetting::SetArray(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(CUnit * pkFindUnit = CalcWeight(pkUnit, pkDef, pkArr, pkGround, iSecondType))
	{
		return pkFindUnit;
	}

	return (*pkArr->begin()).pkUnit;
}

CUnit* PgTargettingNone::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	return (*pkArr->begin()).pkUnit;
}

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingHP::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{	
	if(0 == TargettingUtil::GetSecType(pkUnit, pkDef, iSecondType))
	{
		pkArr->sort(PgTargettingHP::SortGreater);
	}
	else
	{
		pkArr->sort(PgTargettingHP::SortLesser);
	}

	return (*pkArr->begin()).pkUnit;
}

bool PgTargettingHP::SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_HP) > rhs.pkUnit->GetAbil(AT_HP);
}

bool PgTargettingHP::SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_HP) <= rhs.pkUnit->GetAbil(AT_HP);
}

//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingLevel::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(0 == TargettingUtil::GetSecType(pkUnit, pkDef, iSecondType))
	{
		pkArr->sort(PgTargettingLevel::SortGreater);
	}
	else
	{
		pkArr->sort(PgTargettingLevel::SortLesser);
	}

	return (*pkArr->begin()).pkUnit;
}

bool PgTargettingLevel::SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_LEVEL) > rhs.pkUnit->GetAbil(AT_LEVEL);
}

bool PgTargettingLevel::SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_LEVEL) <= rhs.pkUnit->GetAbil(AT_LEVEL);
}

//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingBaseClass::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	CONT_DEFUPGRADECLASS const* pkUpClass = NULL;
	g_kTblDataMgr.GetContDef(pkUpClass);
	if(pkUpClass)
	{
		int iClass = TargettingUtil::GetAbil(pkDef, pkUnit, AT_AI_TARGETTING_THIRD_TYPE);//클래스 타입을 미리 받아놓자
		if(0 == TargettingUtil::GetSecType(pkUnit, pkDef, iSecondType))
		{
			return FindEqual(iClass, pkUpClass, pkArr);
		}
		else
		{
			return FindDiff(iClass, pkUpClass, pkArr);
		}
	}

	return (*pkArr->begin()).pkUnit;
}

CUnit* PgTargettingBaseClass::FindEqual(int const iClass, CONT_DEFUPGRADECLASS const* pkUpClass, UNIT_PTR_ARRAY* pkArr) const
{
	UNIT_PTR_ARRAY::iterator unit_it = pkArr->begin();
	while(unit_it!=pkArr->end())
	{
		if((*unit_it).pkUnit)
		{
			CONT_DEFUPGRADECLASS::const_iterator char_it = pkUpClass->find((*unit_it).pkUnit->GetAbil(AT_CLASS));
			if(char_it != pkUpClass->end() && iClass == (*char_it).second.byKind)
			{
				UNIT_PTR_HELPER kForCopy = (*unit_it);
				pkArr->push_front(kForCopy);//찾은걸 맨 첨으로
				pkArr->erase(unit_it);
				return kForCopy.pkUnit;
			}
		}
		++unit_it;
	}

	return (*pkArr->begin()).pkUnit;
}

CUnit* PgTargettingBaseClass::FindDiff(int const iClass, CONT_DEFUPGRADECLASS const* pkUpClass, UNIT_PTR_ARRAY* pkArr) const
{
	UNIT_PTR_ARRAY::iterator unit_it = pkArr->begin();
	while(unit_it!=pkArr->end())
	{
		if((*unit_it).pkUnit)
		{
			CONT_DEFUPGRADECLASS::const_iterator char_it = pkUpClass->find((*unit_it).pkUnit->GetAbil(AT_CLASS));
			if(char_it != pkUpClass->end() && iClass != (*char_it).second.byKind)
			{
				UNIT_PTR_HELPER kForCopy = (*unit_it);
				pkArr->push_front(kForCopy);//찾은걸 맨 첨으로
				pkArr->erase(unit_it);
				return kForCopy.pkUnit;
			}
		}
		++unit_it;
	}

	return (*pkArr->begin()).pkUnit;
}

//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingAggro::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(!pkGround)
	{
		return (*pkArr->begin()).pkUnit;
	}
	if(0 == TargettingUtil::GetSecType(pkUnit, pkDef, iSecondType))
	{
		return FindGreatest(pkUnit, pkArr, pkGround);
	}
	else
	{
		return FindLeast(pkUnit, pkArr, pkGround);
	}

	return (*pkArr->begin()).pkUnit;
}

CUnit* PgTargettingAggro::FindGreatest(CUnit* pkUnit, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround) const
{
	BM::GUID kGuid;
	if(pkUnit->GetTargetFromAggro(kGuid))
	{
		if(CUnit* pkGetUnit = pkGround->GetUnit(kGuid))
		{
			UNIT_PTR_ARRAY::iterator unit_it = pkArr->begin();
			while(unit_it!=pkArr->end())
			{
				if((*unit_it).pkUnit && (*unit_it).pkUnit->GetID() == kGuid)	//찾았으면
				{
					UNIT_PTR_HELPER kForCopy = (*unit_it);
					pkArr->push_front(kForCopy);//찾은걸 맨 첨으로
					pkArr->erase(unit_it);
					return kForCopy.pkUnit;
				}
				++unit_it;
			}
			return pkUnit;//타겟리스트에 없으면 그냥 이놈을 반환하자
		}
	}

	return (*pkArr->begin()).pkUnit;
}

CUnit* PgTargettingAggro::FindLeast(CUnit* pkUnit, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround) const
{
	BM::GUID kGuid;
	VEC_GUID kGuidVec;
	if(pkUnit->AggroMeter()->GetScoreBottom(kGuidVec,1))	//한개만
	{
		if(CUnit* pkGetUnit = pkGround->GetUnit(kGuid))
		{
			UNIT_PTR_ARRAY::iterator unit_it = pkArr->begin();
			while(unit_it!=pkArr->end())
			{
				if((*unit_it).pkUnit && (*unit_it).pkUnit->GetID() == kGuid)	//찾았으면
				{
					UNIT_PTR_HELPER kForCopy = (*unit_it);
					pkArr->push_front(kForCopy);//찾은걸 맨 첨으로
					pkArr->erase(unit_it);
					return kForCopy.pkUnit;
				}
				++unit_it;
			}
			return pkUnit;//타겟리스트에 없으면 그냥 이놈을 반환하자
		}
	}

	return (*pkArr->begin()).pkUnit;
}

//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
CUnit* PgTargettingDistance::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(0 == TargettingUtil::GetSecType(pkUnit, pkDef, iSecondType))
	{
		return FindGreatest(pkUnit, pkArr, pkGround);	//거리가 먼 순으로
	}
	else
	{
		//아무것도 할 필요가 없다.
	}

	return (*pkArr->begin()).pkUnit;
}

CUnit* PgTargettingDistance::FindGreatest(CUnit* pkUnit, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround) const
{
	if(!pkArr)	{return NULL;}
	pkArr->reverse();
	
	return (*pkArr->begin()).pkUnit;
}
//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingMoveSpeed::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(0 == TargettingUtil::GetSecType(pkUnit, pkDef, iSecondType))
	{
		pkArr->sort(PgTargettingMoveSpeed::SortGreater);
	}
	else
	{
		pkArr->sort(PgTargettingMoveSpeed::SortLesser);
	}

	return (*pkArr->begin()).pkUnit;
}

bool PgTargettingMoveSpeed::SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_MOVESPEED) > rhs.pkUnit->GetAbil(AT_MOVESPEED);
}

bool PgTargettingMoveSpeed::SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_MOVESPEED) <= rhs.pkUnit->GetAbil(AT_MOVESPEED);
}

//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingStrategicPoint::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(0 == TargettingUtil::GetSecType(pkUnit, pkDef, iSecondType))
	{
		pkArr->sort(PgTargettingStrategicPoint::SortGreater);
	}
	else
	{
		pkArr->sort(PgTargettingStrategicPoint::SortLesser);
	}

	return (*pkArr->begin()).pkUnit;
}

bool PgTargettingStrategicPoint::SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_STRATEGIC_POINT) > rhs.pkUnit->GetAbil(AT_STRATEGIC_POINT);
}

bool PgTargettingStrategicPoint::SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_STRATEGIC_POINT) <= rhs.pkUnit->GetAbil(AT_STRATEGIC_POINT);
}

//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingCallerTarget::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(!pkUnit)
	{
		return NULL;
	}

	BM::GUID const& kGuid = pkUnit->Caller();
	UNIT_PTR_ARRAY::iterator unit_it = pkArr->begin();
	while(unit_it!=pkArr->end())
	{
		if((*unit_it).pkUnit && (*unit_it).pkUnit->GetTarget()==kGuid)	//찾았으면
		{
			UNIT_PTR_HELPER kForCopy = (*unit_it);
			pkArr->push_front(kForCopy);//찾은걸 맨 첨으로
			unit_it = pkArr->erase(unit_it);
			continue;
		}
		++unit_it;
	}	

	return (*pkArr->begin()).pkUnit;
}

//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingAggroPoint::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(!pkUnit || !pkArr)
	{
		return NULL;
	}

	//우선 거리순으로 정렬
	CUnit * pkNearByUnit = NULL;
	UNIT_PTR_ARRAY::iterator unit_it = pkArr->begin();
	while(unit_it!=pkArr->end())
	{
		if((*unit_it).pkUnit)
		{
			if( !pkNearByUnit )
			{
				pkNearByUnit = (*unit_it).pkUnit;
			}

			float const fNearByDistQ = GetDistanceQ(pkUnit->GetPos(), pkNearByUnit->GetPos());
			float const fUnitDistQ = GetDistanceQ(pkUnit->GetPos(), (*unit_it).pkUnit->GetPos());
			if(fNearByDistQ > fUnitDistQ)
			{
				pkNearByUnit = (*unit_it).pkUnit;

				UNIT_PTR_HELPER kForCopy = (*unit_it);
				pkArr->push_front(kForCopy);//찾은걸 맨 첨으로
				unit_it = pkArr->erase(unit_it);
				continue;
			}
		}
		++unit_it;
	}

	if(0 == TargettingUtil::GetSecType(pkUnit, pkDef, iSecondType))
	{
		pkArr->sort(PgTargettingAggroPoint::SortGreater);
	}
	else
	{
		pkArr->sort(PgTargettingAggroPoint::SortLesser);
	}

	return (*pkArr->begin()).pkUnit;
}

bool PgTargettingAggroPoint::SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_AGGRO_POINT) > rhs.pkUnit->GetAbil(AT_AGGRO_POINT);
}

bool PgTargettingAggroPoint::SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs)
{
	if(!lhs.pkUnit) {return true;}
	if(!rhs.pkUnit) {return false;}

	return lhs.pkUnit->GetAbil(AT_AGGRO_POINT) <= rhs.pkUnit->GetAbil(AT_AGGRO_POINT);
}

//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingClassNo::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	int const iClass = TargettingUtil::GetAbil(pkDef, pkUnit, AT_AI_TARGETTING_THIRD_TYPE);//클래스 타입을 미리 받아놓자
	if(iClass > 0)
	{
		UNIT_PTR_ARRAY::iterator unit_it = pkArr->begin();
		while(unit_it!=pkArr->end())
		{
			if((*unit_it).pkUnit && (*unit_it).pkUnit->GetAbil(AT_CLASS) == iClass)
			{
				UNIT_PTR_HELPER kForCopy = (*unit_it);
				pkArr->push_front(kForCopy);//찾은걸 맨 첨으로
				pkArr->erase(unit_it);
				return kForCopy.pkUnit;
			}
			++unit_it;
		}
	}

	return NULL;
}

//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingPartyMaster::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	if(pkGround)
	{
		CONT_PARTY_MASTER kCont;
		if( pkGround->GetPartyMaster(kCont) )
		{
			for(CONT_PARTY_MASTER::const_iterator c_it = kCont.begin(); c_it != kCont.end(); ++c_it)
			{
				UNIT_PTR_ARRAY::iterator unit_it = std::find(pkArr->begin(), pkArr->end(), (*c_it).second);
				if(unit_it != pkArr->end())
				{
					UNIT_PTR_HELPER kForCopy = (*unit_it);
					pkArr->push_front(kForCopy);//찾은걸 맨 첨으로
					pkArr->erase(unit_it);
					return kForCopy.pkUnit;
				}
			}
		}
	}

	return NULL;
}

//
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//

CUnit* PgTargettingConstellation::CalcWeight(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, int const iSecondType) const
{
	int iRate = TargettingUtil::GetAbil(pkDef, pkUnit, AT_AI_TARGETTING_RATE);
	if(0 == iRate)
	{
		iRate = ABILITY_RATE_VALUE;
	}

	if (iRate > BM::Rand_Index(ABILITY_RATE_VALUE))
	{
		if(PgIBaseTargetting const* pkTargetting = g_kTargettingManager.GetPtr(PgTargettingManager::ETT_CLASS_NO))
		{
			if(CUnit * pkFindUnit = pkTargetting->CalcWeight(pkUnit, pkDef, pkArr, pkGround))
			{
				return pkFindUnit;
			}
		}
	}

	if(PgIBaseTargetting const* pkTargetting = g_kTargettingManager.GetPtr(PgTargettingManager::ETT_PARTY_MASTER))
	{
		return pkTargetting->CalcWeight(pkUnit, pkDef, pkArr, pkGround);
	}

	return (*pkArr->begin()).pkUnit;
}

//
//////////////////////////////////////////////////////////////////////

void PgTargettingManager::Init()
{
	int i = 0;
	while(i < ETT_MAX)
	{
		PgIBaseTargetting* pkBase = NULL;
		switch(i)
		{
		case ETT_NONE:
			{
				pkBase = new_tr PgTargettingNone;
			}break;
		case ETT_DAMAGE:
			{
				pkBase = new_tr PgTargettingAggro;
			}break;
		case ETT_BASE_CALSS:
			{
				pkBase = new_tr PgTargettingBaseClass;
			}break;
		case ETT_LEVEL:
			{
				pkBase = new_tr PgTargettingLevel;
			}break;
		case ETT_HP:
			{
				pkBase = new_tr PgTargettingHP;
			}break;
		case ETT_DISTANCE:
			{
				pkBase = new_tr PgTargettingDistance;
			}break;
        case ETT_MOVESPEED:
            {
                pkBase = new_tr PgTargettingMoveSpeed;
            }break;
        case ETT_STRATEGICPOINT:
            {
                pkBase = new_tr PgTargettingStrategicPoint;
            }break;
		case ETT_CALLERTARGET:
			{
				pkBase = new_tr PgTargettingCallerTarget;
			}break;
		case ETT_AGGROPOINT:
			{
				pkBase = new_tr PgTargettingAggroPoint;
			}break;
		case ETT_CLASS_NO:
			{
				pkBase = new_tr PgTargettingClassNo;
			}break;
		case ETT_PARTY_MASTER:
			{
				pkBase = new_tr PgTargettingPartyMaster;
			}break;
		case ETT_CONSTELLATION:
			{
				pkBase = new_tr PgTargettingConstellation;
			}break;
		case ETT_MAX:
			{
				return;
			}break;
		}

		if(pkBase)
		{
			m_kTargetting_Map.insert(std::make_pair(i, pkBase));
		}
		++i;
	}
}

PgTargettingManager::~PgTargettingManager()
{
	Targetting_Map::iterator t_it = m_kTargetting_Map.begin();
	while(m_kTargetting_Map.end() != t_it)
	{
		SAFE_DELETE((*t_it).second);
		++t_it;
	}

	m_kTargetting_Map.clear();
}

const char* g_kTargettingType[PgTargettingManager::ETT_MAX] = {"NONE", "DAMAGE", "CALSS", "LEVEL", "HP", "DISTANCE", "MOVESPEED", "STRATEGICPOINT"};
CUnit* PgTargettingManager::FindTarget(CUnit* pkUnit, CSkillDef const* pkDef, UNIT_PTR_ARRAY* pkArr, PgGround* pkGround, E_TARGETTING_TYPE eType, int const iSecType) const
{
	if(!pkArr || pkArr->empty())	//이건 뭐 답이 없음
	{
		return NULL;
	}
	if(1==pkArr->size()) //한놈뿐이면
	{
		return (*pkArr->begin()).pkUnit;
	}

    if(ETT_NONE == eType)
    {
		eType = static_cast<E_TARGETTING_TYPE>(TargettingUtil::GetAbil(pkDef, pkUnit, AT_AI_TARGETTING_TYPE));
    }
	
	Targetting_Map::const_iterator tar_it = m_kTargetting_Map.find(eType);
	
	if(eType != ETT_NONE && tar_it!=m_kTargetting_Map.end())
	{
		Targetting_Map::mapped_type pkTargetting = (*tar_it).second;
		return pkTargetting->SetArray(pkUnit, pkDef, pkArr, pkGround, iSecType);

		/*
        INFO_LOG(BM::LOG_LV5, L"PgTargettingManager::FindTarget ["<< g_kTargettingType[eType] << L"]");
        INFO_LOG(BM::LOG_LV5, L" <<    Befor    >>");
        UNIT_PTR_ARRAY::iterator unit_it = pkArr->begin();
        while(unit_it != pkArr->end())
        {
            CUnit * pkTargetUnit = unit_it->pkUnit;
            if(pkUnit && pkUnit->IsUnitType(UT_ENTITY) && pkTargetUnit)
            {
                INFO_LOG(BM::LOG_LV5, L" Guid<" << pkTargetUnit->GetID() << L"> "
                    << L"HP<" << pkTargetUnit->GetAbil(AT_HP) << L"> "
                    << L"MOVESPEED<" << pkTargetUnit->GetAbil(AT_MOVESPEED) << L"> "
                    << L"STRATEGICPOINT<" << pkTargetUnit->GetAbil(AT_STRATEGIC_POINT) << L"> "
                    );
            }
            ++unit_it;
        }

        CUnit* pkResultUnit = pkTargetting->SetArray(pkUnit, pkDef, pkArr, pkGround, iSecType);

        INFO_LOG(BM::LOG_LV5, L" <<    After    >>");
        unit_it = pkArr->begin();
        while(unit_it != pkArr->end())
        {
            CUnit * pkTargetUnit = unit_it->pkUnit;
            if(pkUnit && pkUnit->IsUnitType(UT_ENTITY) && pkTargetUnit)
            {
                INFO_LOG(BM::LOG_LV5, L" Guid<" << pkTargetUnit->GetID() << L"> "
                    << L"HP<" << pkTargetUnit->GetAbil(AT_HP) << L"> "
                    << L"MOVESPEED<" << pkTargetUnit->GetAbil(AT_MOVESPEED) << L"> "
                    << L"STRATEGICPOINT<" << pkTargetUnit->GetAbil(AT_STRATEGIC_POINT) << L"> "
                    );
            }
            ++unit_it;
        }

		return pkResultUnit;
		*/
	}

	return (*pkArr->begin()).pkUnit;
}

CUnit* PgTargettingManager::FindTarget(CUnit* pkUnit, CSkillDef const* pkDef, CUnit::DequeTarget& rkList, PgGround* pkGround, E_TARGETTING_TYPE const eType, int const iSecType) const
{
	if(!pkGround) {return NULL;}
	UNIT_PTR_ARRAY kArr;
	pkGround->DequeTargetToUNIT_PTR_ARRAY(rkList, kArr);

	CUnit* pkResultUnit = FindTarget(pkUnit, pkDef, &kArr, pkGround, eType, iSecType);
	if(pkResultUnit)
	{
		pkUnit->SetReserveTarget(pkResultUnit->GetID());
	}

	return pkResultUnit;
}

PgIBaseTargetting const* PgTargettingManager::GetPtr(PgTargettingManager::E_TARGETTING_TYPE const eType)const
{
	Targetting_Map::const_iterator tar_it = m_kTargetting_Map.find(eType);
	if(tar_it != m_kTargetting_Map.end())
	{
		return tar_it->second;
	}
	return NULL;
}