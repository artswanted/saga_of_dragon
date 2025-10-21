#include "stdafx.h"
#include "Lohengrin/packetstruct2.h"
#include "VirtualWorld.h"

PgGround::PgGround(SGroundKey const &rkGroundKey)
: m_kGroundKey(rkGroundKey)
{
}

PgGround::~PgGround()
{
}

void PgGround::AddUnit(CUnit* pkUnit)
{
}


PgVirtualWorld::PgVirtualWorld()
{
}

PgVirtualWorld::~PgVirtualWorld()
{
	CONT_GROUND::iterator itor_ground = m_kContGround.begin();
	while (itor_ground != m_kContGround.end())
	{
		SAFE_DELETE(itor_ground->second);

		++itor_ground;
	}
	m_kContGround.clear();
}

HRESULT PgVirtualWorld::Locked_AddUnit(SGroundKey const &rkGround, UNIT_PTR_ARRAY& rkUnitArray)
{
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_GROUND::iterator itor_ground = m_kContGround.find(rkGround);
		if (itor_ground != m_kContGround.end())
		{
			return AddUnit(itor_ground->second, rkUnitArray);
		}
	}
	{

		BM::CAutoMutex kLock(m_kMutex, true);
		PgGround* pkNew = new PgGround(rkGround);
		auto ibRet = m_kContGround.insert(std::make_pair(rkGround, pkNew));
		if (!ibRet.second)
		{
			// 위의 Lock과 미묘한 시간차이로, insert 실패 할수도 있으니.. 오류 처리
			delete pkNew;
			CONT_GROUND::iterator itor_ground = m_kContGround.find(rkGround);
			pkNew = itor_ground->second;
		}

		return AddUnit(pkNew, rkUnitArray);
	}
	return E_FAIL;
}

HRESULT PgVirtualWorld::AddUnit(PgGround* pkGround, UNIT_PTR_ARRAY &rkUnitArray)
{
	UNIT_PTR_ARRAY::const_iterator itor_unit = rkUnitArray.begin();
	while (itor_unit != rkUnitArray.end())
	{
		pkGround->AddUnit((*itor_unit).pkUnit);
		++itor_unit;
	}
	return S_OK;
}

