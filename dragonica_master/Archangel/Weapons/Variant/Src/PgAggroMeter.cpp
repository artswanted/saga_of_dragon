#include "stdafx.h"
#include "Global.h"
#include "PgAggroMeter.h"

BM::TObjectPool< SAggro > g_kAggroPool(1000, 500);


tagAggro::tagAggro()
{
	Clear();
}

tagAggro::tagAggro(BM::GUID const &rkGuid, BM::GUID const &rkOwner)
{
	Clear();
	SetOwner(rkGuid, rkOwner);
}

void tagAggro::Clear()
{
	kOwner.Clear();
	iScore = 0;
	iDamageAggro = iHitAggro = 0;
	iDamageCount = iHitCount = 0;
	dwLastUpdatedTime = BM::GetTime32();
}

void tagAggro::SetOwner(SAggroOwner const &rkOwner)
{
	kOwner = rkOwner;
}

void tagAggro::SetOwner(BM::GUID const &rkGuid, BM::GUID const &rkOwner)
{
	SetOwner(SAggroOwner(rkGuid, rkOwner));
}

void tagAggro::Add(size_t const iAddScore, size_t const iAddDamage, size_t const iAddHit)
{
	if( iAddDamage )
	{
		iDamageAggro += iAddDamage;
		++iDamageCount;
	}

	if( iAddHit )
	{
		iHitAggro += iAddHit;
		++iHitCount;
	}

	if( iAddScore )
	{
		iScore += iAddScore;
	}

	dwLastUpdatedTime = BM::GetTime32();
}

bool tagAggro::Score_Greater(tagAggro const* pLeft, tagAggro const* pRight)
{
	if( pLeft->iScore > pRight->iScore )
	{
		return true;
	}
	else if( pLeft->iScore == pRight->iScore )
	{
		if( pLeft->iDamageAggro > pRight->iDamageAggro )
		{
			return true;
		}
		else if( pLeft->iDamageAggro == pRight->iDamageAggro )
		{
			if( pLeft->iHitAggro < pRight->iHitAggro )
			{
				return true;
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool tagAggro::Damage_Greater(tagAggro const* pLeft, tagAggro const* pRight)
{
	if( pLeft->iDamageAggro > pRight->iDamageAggro )
	{
		return true;
	}
	else if( pLeft->iDamageAggro == pRight->iDamageAggro )
	{
		if( pLeft->iHitAggro < pRight->iHitAggro )
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


//
tagAggro::PgSort_IntervalOver::PgSort_IntervalOver(DWORD const dwNowTime, DWORD const dwDiffInterval, ContEraseVec &rkOut)
	: m_dwNowTime(dwNowTime), m_dwDiffInterval(dwDiffInterval), m_kOut(rkOut)
{
}

tagAggro::PgSort_IntervalOver::~PgSort_IntervalOver() {}

void tagAggro::PgSort_IntervalOver::operator() (tagAggro const *pRight)
{
	if( !pRight )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pRight is NULL"));
		return;
	}

	if( m_dwDiffInterval > DifftimeGetTime(m_dwNowTime, pRight->dwLastUpdatedTime) )
	{
		std::back_inserter(m_kOut) = pRight->kOwner;
	}
}



//
PgAggroMeter::PgAggroMeter()
{
	Clear();
}

PgAggroMeter::PgAggroMeter(BM::GUID const &rkGuid, int const iHP)
{
	PgAggroMeter();
	Set(rkGuid, iHP);
};

PgAggroMeter::~PgAggroMeter()
{
	Clear();
};

void PgAggroMeter::Clear(bool bOnlyData)
{
	BM::CAutoMutex kLock(m_kMutex);
	ContAggro::iterator aggro_iter = m_kMeter.begin();
	while(m_kMeter.end() != aggro_iter)
	{
		ContAggro::value_type pkElement = (*aggro_iter);
		if( pkElement )
		{
			g_kAggroPool.Delete(pkElement);
		}
		++aggro_iter;
	}
	m_kMap.clear();
	m_kMeter.clear();

	m_iTotalDamage = m_iTotalHit = 0;
	m_iTotalDamageCount = m_iTotalHitCount = 0;

	if(!bOnlyData)
	{
		m_kGuid.Clear();
		m_iHP = 0;
	}
}

void PgAggroMeter::Set(BM::GUID const &rkGuid, int const iHP)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kGuid = rkGuid;
	m_iHP = iHP;
}

bool PgAggroMeter::AddVal(const EAggroMeterType eType, SAggroOwner const &rkCaster, int const iVal, VEC_GUID const* pkMember)
{
	BM::CAutoMutex kLock(m_kMutex);
	bool bRet = false;
	switch(eType)
	{
	case AMT_Damage:
		{
			bRet = AddDamage(rkCaster, iVal, pkMember);
		}break;
	case AMT_Hit:
		{
			bRet = AddHit(rkCaster, iVal, pkMember);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Invalid AddVal Type: "<<static_cast<int>(eType));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}
	return bRet;
}

size_t PgAggroMeter::GetScoreTop(VEC_GUID& rkOut, size_t const iTop)
{
	BM::CAutoMutex kLock(m_kMutex);
	SortScore();
	return GetTop(rkOut, iTop);
}

size_t PgAggroMeter::GetDamageTop(VEC_GUID& rkOut, size_t const iTop) //순수 데미지 순위대로 뽑자
{
	BM::CAutoMutex kLock(m_kMutex);
	SortDamage();
	return GetTop(rkOut, iTop);
}

size_t PgAggroMeter::GetScoreBottom(VEC_GUID& rkOut, size_t const iTop)
{
	BM::CAutoMutex kLock(m_kMutex);
	SortScore();
	return GetBottom(rkOut, iTop);
}

size_t PgAggroMeter::GetDamageBottom(VEC_GUID& rkOut, size_t const iTop) //순수 데미지 순위대로 뽑자
{
	BM::CAutoMutex kLock(m_kMutex);
	SortDamage();
	return GetBottom(rkOut, iTop);
}

bool PgAggroMeter::AddDamage(SAggroOwner const &rkCaster, size_t const iDamage, VEC_GUID const* pkMember)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( rkCaster.IsNull() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	size_t const iBonus = (!m_iTotalDamage)? (size_t)(m_iHP*0.15f): 0;//첫타면
	m_iTotalDamage += iDamage;
	++m_iTotalDamageCount;

	//타격 주인에게 +
	AddDamageScore(rkCaster, iDamage, iDamage, 1.f, iBonus);

	if( BM::GUID::NullData() != rkCaster.kOwner )//소환물체 이면
	{
		AddDamageScore(SAggroOwner(rkCaster.kOwner, BM::GUID::NullData()), iDamage, 0, 0.50f);//주인은 50%만큼
	}
	
	if( pkMember )//파티원에게
	{
		VEC_GUID::const_iterator guid_iter = pkMember->begin();
		while(pkMember->end() != guid_iter)
		{
			VEC_GUID::value_type const& rkGuid = (*guid_iter);
			AddDamageScore(SAggroOwner(rkGuid, BM::GUID::NullData()), iDamage, 0, 0.30f);//파티원은 30%만큼
			++guid_iter;
		}
	}
	return true;
}

bool PgAggroMeter::AddHit(SAggroOwner const &rkTarget, size_t const iHit, VEC_GUID const* pkMember)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( rkTarget.IsNull() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iBonus = (!m_iTotalHit)? (int)(m_iHP*0.15f): 0;//첫 히트면
	m_iTotalHit += iHit;
	++m_iTotalHitCount;

	//대상에게
	AddHitScore(rkTarget, iHit, iHit, 1.f, iBonus);

	if( BM::GUID::NullData() != rkTarget.kOwner )//소환물체 이면
	{
		AddHitScore(SAggroOwner(rkTarget.kOwner, BM::GUID::NullData()), iHit, 0, 0.50f);//대상 주인에게 50%만큼
	}

	//if( pkMember )//파티원에게
	//{
	//	//
	//}
	return true;
}

//void PgAggroMeter::Print()
//{
//	SortScore();
//
//	cout << "Total HP: " << m_iHP << endl;
//	ContAggro::iterator iter = m_kMeter.begin();
//	while(m_kMeter.end() != iter)
//	{
//		const ContAggro::value_type pkElement = (*iter);
//		printf_s("%s S:%4d D:%4d H:%4d\n", MB(pkElement->Guid().str()), pkElement->iScore, pkElement->iDamageAggro, pkElement->iHitAggro);
//		++iter;
//	}
//
//	cout << "Total Damage: " << m_iTotalDamage << " Total Hit: " << m_iTotalHit << endl;
//}

void PgAggroMeter::SortScore()
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kMeter.sort(SAggro::Score_Greater);
}

void PgAggroMeter::SortDamage()
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kMeter.sort(SAggro::Damage_Greater);
}


bool PgAggroMeter::GetAggro(SAggroOwner const &rkOwner, SAggro*& pkAggro)
{
	BM::CAutoMutex kLock(m_kMutex);
	bool const bFindAggro = Get(rkOwner, pkAggro);
	if( bFindAggro )
	{
		return true;
	}

	bool const bAddAggro = Add(rkOwner, pkAggro);
	if( bAddAggro )
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;//실패!
}

bool PgAggroMeter::Add(SAggroOwner const &rkOwner, SAggro*& pkAggro)
{
	BM::CAutoMutex kLock(m_kMutex);

	SAggro* pkNewAggro = g_kAggroPool.New();
	if( pkNewAggro )
	{
		pkNewAggro->Clear();
		pkNewAggro->SetOwner(rkOwner);

		auto kRet = m_kMap.insert(std::make_pair(rkOwner, pkNewAggro));
		if( kRet.second )
		{
			pkAggro = pkNewAggro;
			m_kMeter.push_back(pkAggro);
			return true;
		}

		g_kAggroPool.Delete(pkNewAggro);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAggroMeter::Get(SAggroOwner const &rkOwner, SAggro*& pkAggro)
{
	BM::CAutoMutex kLock(m_kMutex);
	ContAggroMap::iterator aggro_iter = m_kMap.find(rkOwner);
	if( m_kMap.end() != aggro_iter )
	{
		pkAggro = (*aggro_iter).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAggroMeter::Del(SAggroOwner const &rkOwner)
{
	BM::CAutoMutex kLock(m_kMutex);

	SAggro* pkAggro = NULL;
	bool const bFindAggro = Get(rkOwner, pkAggro);
	if( !bFindAggro )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContAggroMap::iterator aggro_iter = m_kMap.find(rkOwner);
	if( m_kMap.end() != aggro_iter )
	{
		g_kAggroPool.Delete( aggro_iter->second );
		m_kMap.erase(aggro_iter);
	}

	ContAggro::iterator meter_iter = m_kMeter.begin();
	while(m_kMeter.end() != meter_iter)
	{
		ContAggro::value_type pkElement = (*meter_iter);
		if( pkElement->kOwner == rkOwner )
		{
			m_kMeter.erase(meter_iter);
			g_kAggroPool.Delete(pkElement);
			return true;
		}
		++meter_iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAggroMeter::RemoveOldedAggro(DWORD const dwInterval)
{
	BM::CAutoMutex kLock(m_kMutex);

	SAggro::ContEraseVec kVec;
	std::for_each(m_kMeter.begin(), m_kMeter.end(), SAggro::PgSort_IntervalOver(BM::GetTime32(), dwInterval, kVec));
	
	SAggro::ContEraseVec::const_iterator loop_iter = kVec.begin();
	while( kVec.end() != loop_iter )
	{
		Del( *loop_iter );
		++loop_iter;
	}
	return true;
}

size_t PgAggroMeter::GetTop(VEC_GUID& rkOut, size_t const iTop) const
{
	BM::CAutoMutex kLock(m_kMutex);

	size_t iCount = 1;
	ContAggro::const_iterator iter = m_kMeter.begin();
	while(m_kMeter.end() != iter)
	{
		if(iTop < iCount)
		{
			break;
		}

		ContAggro::value_type pkElement = (*iter);
		//INFO_LOG(BM::LOG_LV5, "GetTop "<<pkElement->Guid()<<" Damage : "<<pkElement->iDamageAggro);
		rkOut.push_back(pkElement->Guid());

		++iter, ++iCount;
	}

	return rkOut.size();
}

size_t PgAggroMeter::GetBottom(VEC_GUID& rkOut, size_t const iTop) const
{
	BM::CAutoMutex kLock(m_kMutex);

	size_t iCount = 1;
	ContAggro::const_reverse_iterator iter = m_kMeter.rbegin();
	while(m_kMeter.rend() != iter)
	{
		if(iTop < iCount)
		{
			break;
		}

		ContAggro::value_type pkElement = (*iter);
		//INFO_LOG(BM::LOG_LV5, "GetTop "<<pkElement->Guid()<<" Damage : "<<pkElement->iDamageAggro);
		rkOut.push_back(pkElement->Guid());

		++iter, ++iCount;
	}

	return rkOut.size();
}

bool PgAggroMeter::IsDiffCaller(SAggroOwner const& rkOwner) const
{
	if(rkOwner.kGuid.IsNull())	{return true;}
	ContAggro::const_iterator it = m_kMeter.begin();
	while(it!=m_kMeter.end())
	{
		SAggro const* pkAggro = (*it);
		if(pkAggro)
		{
			if(pkAggro->kOwner.kGuid==rkOwner.kGuid)//콜러는 같고 오너가 다를 경우
			{
				return true;
			}
		}
		++it;
	}

	return false;
}