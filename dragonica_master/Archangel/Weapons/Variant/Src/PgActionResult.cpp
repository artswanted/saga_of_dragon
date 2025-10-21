#include "stdafx.h"
#include "CEL/Common.h"
#include "constant.h"
#include "PgActionResult.h"
#include "Lohengrin/LogGroup.h"
#include "Item.h"
#include "Unit.h"

PgActionResult::PgActionResult(void)
{
	Init();
}

PgActionResult::~PgActionResult(void)
{
	Init();
}

void PgActionResult::SetValue(int const iValue)
{
	m_iValue = iValue;
}

int PgActionResult::GetValue()
{
	return m_iValue;
}

void PgActionResult::AddEffect(int const iEffect)
{
	return m_vecEffect.push_back(iEffect);
}
void PgActionResult::AddStatusEffect(int const iEffect, int iEffectValue)
{
	return	m_vecStatusEffect.push_back(stStatusEffect(iEffect,iEffectValue));
}
std::vector<int>& PgActionResult::GetResultEffect()
{
	return m_vecEffect;
}
size_t PgActionResult::GetStatusEffectNum()const
{
	return m_vecStatusEffect.size();
}
PgActionResult::stStatusEffect* PgActionResult::GetStatusEffect(int const iEffectIndex)
{
	size_t const eff_size = m_vecStatusEffect.size();
	if((int)eff_size <= iEffectIndex)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	return &(m_vecStatusEffect.at(iEffectIndex));
}

size_t PgActionResult::GetEffectNum()const
{
	return m_vecEffect.size();
}
void	PgActionResult::ClearStatusEffect()
{
	m_vecStatusEffect.clear();
}
int PgActionResult::GetEffect(int const iEffectIndex)
{
	size_t const eff_size = m_vecEffect.size();
	if((int)eff_size <= iEffectIndex)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	return m_vecEffect.at(iEffectIndex);
}

void PgActionResult::Init()
{
	m_bMissed = false;
	m_byBlockType = EBT_NONE;
	m_bComic = false;
	m_bDead = false;
	m_bInvalid = false;
	m_bDodged = false;
	m_bCritical = false;
	m_iValue = 0;
	m_bIsRealEffect = false;
	m_iRemainHP = 0;
	m_bValidHP = false;
	m_bEndure = false;
	m_bRestore = false;
	m_bDamageAction = false;
	m_iAbsorbValue = 0;
	m_vecEffect.clear();
	m_vecStatusEffect.clear();
	CAbilObject::Clear();

	ElemDmgFire(false);
	ElemDmgIce(false);
	ElemDmgNature(false);
	ElemDmgCurse(false);
	ElemDmgDestroy(false);
}

void PgActionResult::SetMissed(bool const bMissed)
{
	m_bMissed = bMissed;
}

bool PgActionResult::GetMissed()const
{
	return m_bMissed;
}

void PgActionResult::SetBlocked(BYTE byBlockType)
{
	m_byBlockType = byBlockType;
}

BYTE PgActionResult::GetBlocked()const
{
	return m_byBlockType;
}

void PgActionResult::SetComic(bool const bComic)
{
	m_bComic = bComic;
}

bool PgActionResult::GetComic()const
{
	return m_bComic;
}

void PgActionResult::SetDead(bool const bDead)
{
	m_bDead = bDead;
}

bool PgActionResult::GetDead()const
{
	return m_bDead;
}

void PgActionResult::SetInvalid(bool const bInvalid)
{
	m_bInvalid = bInvalid;
}

bool PgActionResult::GetInvalid()const
{
	return m_bInvalid;
}

void	PgActionResult::CopyEffectTo(PgActionResult &kTarget)
{
	kTarget.m_vecEffect.clear();

	kTarget.m_vecEffect = m_vecEffect;
}

void PgActionResult::ClearEffect()
{
	m_vecEffect.clear();
}

void PgActionResult::SetDodged(bool const bDodged)
{
	m_bDodged = bDodged;
}

bool PgActionResult::GetDodged() const
{
	return m_bDodged;
}

bool PgActionResult::GetEndure() const
{
	return m_bEndure;
}

void PgActionResult::SetEndure(bool const bEndure)
{
	m_bEndure = bEndure;
}

bool PgActionResult::GetRestore() const
{
	return m_bRestore;
}

void PgActionResult::SetRestore(bool const bRestore)
{
	m_bRestore = bRestore;
}

bool PgActionResult::GetDefenceIgnore() const
{
	return m_bDefenceIgnore;
}

void PgActionResult::SetDefenceIgnore(bool const bDefence)
{
	m_bDefenceIgnore = bDefence;
}

void PgActionResult::ReadFromPacket(BM::Stream &rkPacket)
{
	BYTE byPack;
	rkPacket.Pop(byPack);
	READ_BIT_TO_BOOL(byPack, 0, m_bMissed);
	READ_BIT_TO_BOOL(byPack, 1, m_bComic);
	READ_BIT_TO_BOOL(byPack, 2, m_bDead);
	READ_BIT_TO_BOOL(byPack, 3, m_bInvalid);
	READ_BIT_TO_BOOL(byPack, 4, m_bDodged);
	READ_BIT_TO_BOOL(byPack, 5, m_bMissed);
	READ_BIT_TO_BOOL(byPack, 6, m_bCritical);
	READ_BIT_TO_BOOL(byPack, 7, m_bValidHP);
	rkPacket.Pop(byPack);
	READ_BIT_TO_BOOL(byPack, 0, m_bEndure);
	READ_BIT_TO_BOOL(byPack, 1, m_bRestore);
	READ_BIT_TO_BOOL(byPack, 2, m_bDamageAction);
	READ_BIT_TO_BOOL(byPack, 3, m_bDefenceIgnore);	

	CAbilObject::ReadFromPacket(rkPacket);
	if (!m_bInvalid)
	{
		rkPacket.Pop(m_iValue);
		rkPacket.Pop(m_iRemainHP);
		rkPacket.Pop(m_vecEffect);
		rkPacket.Pop(m_byCollision);
		rkPacket.Pop(m_iAbsorbValue);
		rkPacket.Pop(m_byBlockType);
	}
}

void PgActionResult::WriteToPacket(BM::Stream &rkPacket)const
{
	BYTE byPack;
	MAKE_BYTE_FROM_BIT(byPack, m_bMissed, m_bComic, m_bDead, m_bInvalid, m_bDodged, m_bMissed, m_bCritical, m_bValidHP);
	rkPacket.Push(byPack);
	MAKE_BYTE_FROM_BIT(byPack, m_bEndure, m_bRestore, m_bDamageAction, m_bDefenceIgnore, 0, 0, 0, 0);
	rkPacket.Push(byPack);

	CAbilObject::WriteToPacket(rkPacket);
	if (!m_bInvalid)
	{
		rkPacket.Push(m_iValue);
		rkPacket.Push(m_iRemainHP);
		rkPacket.Push(m_vecEffect);
		rkPacket.Push(m_byCollision);
		rkPacket.Push(m_iAbsorbValue);
		rkPacket.Push(m_byBlockType);
	}
}

void PgActionResult::SetCollision(BYTE const byCollision)
{
	m_byCollision = byCollision;
}

BYTE PgActionResult::GetCollision()const
{
	return m_byCollision;
}

int PgActionResult::GetRemainHP()const
{
	return m_iRemainHP;
}

void PgActionResult::SetRemainHP(int const iHP)
{
	m_iRemainHP = iHP;
	m_bValidHP = true;
}

void PgActionResult::SetCritical(bool const bCritical)
{
	m_bCritical = bCritical;
}

bool PgActionResult::GetCritical()
{
	return m_bCritical;
}

bool PgActionResult::IsValidHP()const
{
	return m_bValidHP;
}

void PgActionResult::SetDamageAction(bool const bDamageAction)
{
	m_bDamageAction = bDamageAction;
}

bool PgActionResult::IsDamageAction()const
{
	return m_bDamageAction;
}

void PgActionResult::SetAbsorbValue(int const iAbsorbValue)
{
	m_iAbsorbValue = iAbsorbValue;
}

int  PgActionResult::GetAbsorbValue() const
{
	return m_iAbsorbValue;
}

void PgActionResult::SetElemDmg(CUnit* pkCaster, int const iDestroy, int const iFire, int const iIce, int const iNature, int const iCurse)
{// 속성 데미지를 받아서
	if(!pkCaster)
	{
		return;
	}
	if(0 == iDestroy
		&& 0 == iFire
		&& 0 == iIce
		&& 0 == iNature
		&& 0 == iCurse
		)
	{
		return;
	}

	std::vector<SElemDmgSortData> kTempCont;
	kTempCont.push_back( SElemDmgSortData(ET_D_DESTROY, iDestroy) );
	kTempCont.push_back( SElemDmgSortData(ET_D_FIRE, iFire) );
	kTempCont.push_back( SElemDmgSortData(ET_D_ICE, iIce) );
	kTempCont.push_back( SElemDmgSortData(ET_D_NATURE, iNature) );
	kTempCont.push_back( SElemDmgSortData(ET_D_CURSE, iCurse) );
	// 가장 큰 데미지를 구한후
	std::sort(kTempCont.begin(), kTempCont.end());
	std::vector<SElemDmgSortData>::const_iterator kItor = kTempCont.begin();
	if( kTempCont.end() != kItor )
	{
		ElemDmgDestroy(false);
		ElemDmgFire(false);
		ElemDmgIce(false); 
		ElemDmgNature(false);
		ElemDmgCurse(false);
		
		int const iRand = BM::Rand_Index(100000) % ABILITY_RATE_VALUE;
		int const iValue = ( (*kItor).iDmg / 100 ) * 500; // 100데미지당 5%
		if(iRand <= iValue)
		{// 확률적으로 속성 데미지 파티클이 터지게
			switch( (*kItor).eDmgType )
			{// 그 데미지만 이펙트에 터질수 있게 수정 해준다
			case ET_D_DESTROY:	{ ElemDmgDestroy(true); }break;
			case ET_D_FIRE:		{ ElemDmgFire(true); }break;
			case ET_D_ICE:		{ ElemDmgIce(true); }break;
			case ET_D_NATURE:	{ ElemDmgNature(true); }break;
			case ET_D_CURSE:	{ ElemDmgCurse(true); }break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//	PgActionResultVector
/////////////////////////////////////////////////////////////////////////////

PgActionResultVector::ACTIONRESULT_POOL PgActionResultVector::m_kPool(5, 5);

PgActionResultVector::PgActionResultVector()
{
	//INFO_LOG(BM::LOG_LV7, _T("[%s]"), __FUNCTIONW__);
}

PgActionResultVector::~PgActionResultVector()
{
	//INFO_LOG(BM::LOG_LV7, _T("[%s]"), __FUNCTIONW__);
	Clear();
}

PgActionResult* PgActionResultVector::GetResult(BM::GUID const &rkGuid, bool const bMake)
{
	ACTIONRESULT_MAP::iterator itor = m_kResult.find(rkGuid);
	if (itor != m_kResult.end())
	{
		return itor->second;
	}

	if (bMake)
	{
		PgActionResult* pkNew = m_kPool.New();
		pkNew->Init();

		auto kPair = m_kResult.insert(std::make_pair(rkGuid, pkNew));
		if(kPair.second)
		{
			return pkNew;
		}
		m_kPool.Delete( pkNew );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

int PgActionResultVector::GetValidCount()const
{
	ACTIONRESULT_MAP::const_iterator itor = m_kResult.begin();

	int iCount = 0;
	while (itor != m_kResult.end())
	{
		if (!itor->second->GetInvalid())
		{
			++iCount;
		}
		++itor;
	}
	return iCount;
}
int PgActionResultVector::GetTotalCount() const
{
	return static_cast<int>(m_kResult.size());

}

bool PgActionResultVector::Remove( BM::GUID const &rkGuid )
{
	ACTIONRESULT_MAP::iterator itr = m_kResult.find( rkGuid );
	if ( itr != m_kResult.end() )
	{
		m_kPool.Delete( itr->second );
		m_kResult.erase( itr );
		return true;
	}
	return false;
}

void PgActionResultVector::Clear()
{
	ACTIONRESULT_MAP::iterator itor = m_kResult.begin();
	while (itor != m_kResult.end())
	{
		PgActionResult* pkResult = itor->second;
		if (pkResult != NULL)
		{
			m_kPool.Delete(pkResult);
		}
		++itor;
	}

	m_kResult.clear();
}

void PgActionResultVector::ReadFromPacket(BM::Stream& rkPacket)
{
	BYTE byCount;
	rkPacket.Pop(byCount);
	BM::GUID kGuid;
	for (BYTE byI=0; byI<byCount; byI++)
	{
		rkPacket.Pop(kGuid);
		PgActionResult* pkAction = GetResult(kGuid, true);
		if (pkAction == NULL)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot be ActionResult is NULL Guid["<<kGuid<<L"]");
			PgActionResult kTempResult;
			kTempResult.ReadFromPacket(rkPacket);
		}
		else
		{
			pkAction->ReadFromPacket(rkPacket);
		}
	}
}

void PgActionResultVector::WriteToPacket(BM::Stream& rkPacket)const
{
	rkPacket.Push((BYTE)GetValidCount());
	ACTIONRESULT_MAP::const_iterator itor = m_kResult.begin();
	while (itor != m_kResult.end())
	{
		if (itor->second->GetInvalid())
		{
			++itor;
			continue;
		}
		rkPacket.Push(itor->first);	// GUID
		itor->second->WriteToPacket(rkPacket);

		++itor;
	}
}

PgActionResult* PgActionResultVector::GetResult(int const iIndex, BM::GUID& rkGuid)
{
	PgActionResult* pkResult = NULL;

	ACTIONRESULT_MAP::iterator itor = m_kResult.begin();
	int iCount = 0;
	while (itor != m_kResult.end())
	{
		if (iCount >= iIndex)
		{
			pkResult = itor->second;
			rkGuid = itor->first;
			break;
		}
		++iCount;
		++itor;
	}
	return pkResult;
}

