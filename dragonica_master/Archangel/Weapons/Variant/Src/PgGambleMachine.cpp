#include "stdafx.h"
#include "item.h"
#include "PgGambleMachine.h"
#include "variant/PgControlDefMgr.h"

PgGambleMachineImpl::PgGambleMachineImpl()
{
}
PgGambleMachineImpl::~PgGambleMachineImpl()
{
}

void PgGambleMachineImpl::WriteToPacket(BM::Stream & kPacket)
{
	PU::TWriteTable_AM(kPacket,m_kCont);
	PU::TWriteTable_AM(kPacket,m_kCont1);
	PU::TWriteTable_AA(kPacket,m_kContCost);
	PU::TWriteTable_AM(kPacket,m_kContGroup);
	PU::TWriteTable_AM(kPacket,m_kContMixup);
	PU::TWriteTable_AA(kPacket,m_kContMixupPoint);
}

void PgGambleMachineImpl::ReadFromPacket(BM::Stream & kPacket)
{
	CONT_GAMBLEMACHINE kCont;
	PU::TLoadTable_AM(kPacket,kCont);
	m_kCont = kCont;

	CONT_GAMBLEMACHINE kCont1;
	PU::TLoadTable_AM(kPacket,kCont1);
	m_kCont1 = kCont1;

	CONT_GAMBLEMACHINECOST kContCost;
	PU::TLoadTable_AA(kPacket,kContCost);
	m_kContCost = kContCost;

	CONT_GAMBLEMACHINEGROUPRES kContGroup;
	PU::TLoadTable_AM(kPacket,kContGroup);
	m_kContGroup = kContGroup;

	CONT_GAMBLEMACHINEMIXUP kContMixup;
	PU::TLoadTable_AM(kPacket,kContMixup);
	m_kContMixup = kContMixup;

	CONT_GAMBLEMACHINEMIXUPPOINT kContMixupPoint;
	PU::TLoadTable_AA(kPacket,kContMixupPoint);
	m_kContMixupPoint = kContMixupPoint;
}

int PgGambleMachineImpl::GetMixupPoint(int const iGrade) const
{
	CONT_GAMBLEMACHINEMIXUPPOINT::const_iterator iter = m_kContMixupPoint.find(iGrade);
	if(iter == m_kContMixupPoint.end())
	{
		return 0;
	}

	return (*iter).second;
}

int PgGambleMachineImpl::GetMaxMixupPoint() const
{
	CONT_GAMBLEMACHINEMIXUP::const_reverse_iterator riter = m_kContMixup.rbegin();
	if(riter == m_kContMixup.rend())
	{
		return 0;
	}

	return (*riter).first;
}


bool PgGambleMachineImpl::CheckEnableMixupPoint(int const iMixPoint) const
{
	return (m_kContMixup.find(iMixPoint) != m_kContMixup.end());
}

bool PgGambleMachineImpl::MakeEnablePoint(int const iMixPoint,int & iEnablePoint) const
{
	int const iMaxPoint = GetMaxMixupPoint();
	int iPoint = std::min(iMixPoint, iMaxPoint);

	for(int i = iPoint;i >= MAX_GAMBLEMACHINE_MIXUP_NUM;--i)
	{
		if(true == CheckEnableMixupPoint(i))
		{
			iEnablePoint = i;
			return true;
		}
	}

	return false;
}

HRESULT PgGambleMachineImpl::MakeGambleResult(SGAMBLEMACHINEITEM const & kResultItem, CONT_GAMBLEMACHINERESULT & kContResult)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(kResultItem.iItemNo);
	if(!pkItemDef)
	{
		return E_GAMBLEMACHINE_INVALID_ITEM_DATA;
	}

	if(true == pkItemDef->IsAmountItem())
	{
		PgBase_Item kItem;
		if(S_OK != CreateSItem(kResultItem.iItemNo, kResultItem.siCount, 0,kItem))
		{
			return E_GAMBLEMACHINE_INVALID_ITEM_DATA;
		}
		
		if(0 < kResultItem.siUseTime)
		{
			kItem.SetUseTime(static_cast<__int64>(kResultItem.bTimeType),static_cast<__int64>(kResultItem.siUseTime));
		}

		kContResult.push_back(kItem);

		return S_OK;
	}

	for(int i = 0;i < kResultItem.siCount;++i)
	{
		PgBase_Item kItem;
		if(S_OK != CreateSItem(kResultItem.iItemNo,1,0,kItem))
		{
			return E_GAMBLEMACHINE_INVALID_ITEM_DATA;
		}
		
		if(0 < kResultItem.siUseTime)
		{
			kItem.SetUseTime(static_cast<__int64>(kResultItem.bTimeType),static_cast<__int64>(kResultItem.siUseTime));
		}

		kContResult.push_back(kItem);
	}

	return S_OK;
}

HRESULT PgGambleMachineImpl::GetGambleResult(eGambleCoinGrade const kGcg, CONT_GAMBLEMACHINERESULT & kContResult, ERACE_TYPE eRaceType, bool & bBroadcast, SGAMBLEMACHINEITEM & kGambleItem)
{
	const CONT_GAMBLEMACHINE* const pkCont = GetCont(eRaceType); //인간족과 용족의 아이템 리스트는 구분되어진다.
	if(!pkCont)
	{
		return E_FAIL;
	}
	CONT_GAMBLEMACHINE::const_iterator iter = pkCont->find(kGcg);
	if(iter == pkCont->end())
	{
		return E_GAMBLEMACHINE_INVALID_COIN;
	}

	int const iRet = BM::Rand_Range(((*iter).second.iTotalRate - 1),0);

	CONT_GAMBLEITEM const & kCont = (*iter).second.kCont;

	int iTotalRate = 0;

	for(CONT_GAMBLEITEM::const_iterator item_iter = kCont.begin();item_iter != kCont.end();++item_iter)
	{
		iTotalRate += (*item_iter).iRate;
		if(iRet < iTotalRate)
		{
			kGambleItem = (*item_iter);
			bBroadcast = kGambleItem.bBroadcast;
			return MakeGambleResult(kGambleItem, kContResult);
		}
	}

	return E_GAMBLEMACHINE_OVER_RANGE_RATE;
}

int const MAX_GAMBLE_ROULETTE_NUM = 10;

HRESULT PgGambleMachineImpl::GetGambleRoulette(eGambleCoinGrade const kGcg, CONT_GAMBLEMACHINERESULT & kContResult, CONT_GAMBLEITEM & kContDumy, ERACE_TYPE eRaceType, bool & bBroadcast)
{
	SGAMBLEMACHINEITEM kGambleItem;
	HRESULT const kErr = GetGambleResult(kGcg, kContResult, eRaceType, bBroadcast, kGambleItem);
	if(S_OK != kErr)
	{
		return kErr;
	}

	const CONT_GAMBLEMACHINE* const pkCont = GetCont(eRaceType); //인간족과 용족의 아이템 리스트는 구분되어진다.
	if(!pkCont)
	{
		return E_FAIL;
	}

	CONT_GAMBLEMACHINE::const_iterator iter = pkCont->find(kGcg);
	if(iter == pkCont->end())
	{
		return E_GAMBLEMACHINE_INVALID_COIN;
	}

	CONT_GAMBLEITEM kCont = (*iter).second.kCont;

	std::random_shuffle(kCont.begin(),kCont.end(),BM::Rand_Index);

	kContDumy.push_back(kGambleItem);

	for(CONT_GAMBLEITEM::const_iterator dumy_iter = kCont.begin();dumy_iter != kCont.end();++dumy_iter)
	{
		if(MAX_GAMBLE_ROULETTE_NUM <= kContDumy.size())
		{
			break;
		}

		if(kGambleItem.iItemNo == (*dumy_iter).iItemNo)
		{
			continue;
		}

		kContDumy.push_back((*dumy_iter));
	}

	return S_OK;
}

bool PgGambleMachineImpl::GetGambleMachineCost(eGambleCoinGrade const kGcg,int & iCost)
{
	CONT_GAMBLEMACHINECOST::const_iterator iter = m_kContCost.find(kGcg);
	if(iter == m_kContCost.end())
	{
		return false;
	}

	iCost = (*iter).second;
	return true;
}

typedef struct tagRandomElement
{
	tagRandomElement():iRate(0){}
	tagRandomElement(int __rate,SGAMBLEMACHINEITEM const & __gambleitem):iRate(__rate),kGambleItem(__gambleitem){}
	int iRate;
	SGAMBLEMACHINEITEM kGambleItem;
}SRandomElement;

typedef std::vector<SRandomElement> CONT_RANDOM_TABLE;

HRESULT PgGambleMachineImpl::GetMixupRoulette(int const iMixPoint, CONT_GAMBLEMACHINERESULT & kContResult, CONT_GAMBLEITEM & kContDumy, ERACE_TYPE eRaceType, bool & bBroadcast)
{
	CONT_GAMBLEMACHINEMIXUP::const_iterator mixup_iter = m_kContMixup.find(iMixPoint);
	if(mixup_iter == m_kContMixup.end())
	{
		return E_GAMBLEMACHINE_INVALID_MIXPOINT;
	}

	CONT_GAMEBLEMACHINEMIXUPLELEMENT const & kContElement = (*mixup_iter).second.kCont;

	CONT_RANDOM_TABLE kContRandTable;

	int iTotalRate = 0;

	const CONT_GAMBLEMACHINE* const pkCont = GetCont(eRaceType); //인간족과 용족의 아이템 리스트는 구분되어진다.
	if(!pkCont)
	{
		return E_FAIL;
	}

	for(CONT_GAMEBLEMACHINEMIXUPLELEMENT::const_iterator element_iter = kContElement.begin();element_iter != kContElement.end();++element_iter)
	{
		CONT_GAMEBLEMACHINEMIXUPLELEMENT::value_type const & kElement = (*element_iter);

		CONT_GAMBLEMACHINE::const_iterator gamble_iter = pkCont->find(static_cast<eGambleCoinGrade>(kElement.iGradeNo));
		if(gamble_iter == pkCont->end())
		{
			return E_GAMBLEMACHINE_INVALID_COIN;
		}

		CONT_GAMBLEITEM kContGamble = (*gamble_iter).second.kCont;

		std::random_shuffle(kContGamble.begin(),kContGamble.end(),BM::Rand_Index);

		if(true == kContGamble.empty())
		{
			return E_GAMBLEMACHINE_INVALID_MIXLISTSIZE;
		}

		kContRandTable.push_back(CONT_RANDOM_TABLE::value_type(kElement.iRate,(*kContGamble.begin())));

		iTotalRate += kElement.iRate;
	}

	int const iRand = BM::Rand_Index(iTotalRate);

	int iAddRate = 0;

	SGAMBLEMACHINEITEM kGambleItem;

	for(CONT_RANDOM_TABLE::const_iterator rand_iter = kContRandTable.begin();rand_iter != kContRandTable.end();++rand_iter)
	{
		iAddRate += (*rand_iter).iRate;

		if(iRand < iAddRate)
		{
			kGambleItem = (*rand_iter).kGambleItem;
			break;
		}
	}

	for(CONT_RANDOM_TABLE::const_iterator iter = kContRandTable.begin();iter != kContRandTable.end();++iter)
	{
		kContDumy.push_back((*iter).kGambleItem);
	}

	bBroadcast = kGambleItem.bBroadcast;
	return MakeGambleResult(kGambleItem, kContResult);
}

bool PgGambleMachineImpl::IsCanCostumeMixup(ERACE_TYPE eRaceType)
{
	CONT_GAMBLEMACHINE* pkCont = GetCont(eRaceType);
	if(!pkCont)
	{
		return false;
	}
	return (pkCont->end() != pkCont->find(GCG_COSTUMEMIX_NORMAL))
		&&	(pkCont->end() != pkCont->find(GCG_COSTUMEMIX_ADV))
		&&	(pkCont->end() != pkCont->find(GCG_COSTUMEMIX_SPCL));
		/*&&	(m_kCont.end() != m_kCont.find(GCG_COSTUMEMIX_ARTIFACT))
		&&	(m_kCont.end() != m_kCont.find(GCG_COSTUMEMIX_LEGND));*/
}

//========================================================================================================

void PgGambleMachine::WriteToPacket(BM::Stream & kPacket)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->WriteToPacket(kPacket);
}

void PgGambleMachine::ReadFromPacket(BM::Stream & kPacket)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_,true);
	Instance()->ReadFromPacket(kPacket);
	return ;
}

HRESULT PgGambleMachine::GetGambleResult(eGambleCoinGrade const kGcg, CONT_GAMBLEMACHINERESULT & kContResult, ERACE_TYPE eRaceType, bool & bBroadcast)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetGambleResult(kGcg, kContResult, eRaceType, bBroadcast);
}

bool PgGambleMachine::GetGambleMachineCost(eGambleCoinGrade const kGcg,int & iCost)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetGambleMachineCost(kGcg,iCost);
}

void PgGambleMachine::GetCont(CONT_GAMBLEMACHINE & kCont, ERACE_TYPE eRaceType)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	Instance()->GetCont(kCont, eRaceType);
}

void PgGambleMachine::GetContRes(CONT_GAMBLEMACHINEGROUPRES & kCont)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	Instance()->GetContRes(kCont);
}

void PgGambleMachine::GetContMixup(CONT_GAMBLEMACHINEMIXUP & kCont)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	Instance()->GetContMixup(kCont);
}

void PgGambleMachine::GetContMixupPoint(CONT_GAMBLEMACHINEMIXUPPOINT & kCont)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	Instance()->GetContMixupPoint(kCont);
}

HRESULT PgGambleMachine::GetGambleRoulette(eGambleCoinGrade const kGcg, CONT_GAMBLEMACHINERESULT & kContResult, CONT_GAMBLEITEM & kContDumy, ERACE_TYPE eRaceType, bool & bBroadcast)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetGambleRoulette(kGcg, kContResult, kContDumy, eRaceType, bBroadcast);
}

HRESULT PgGambleMachine::GetMixupRoulette(int const iMixPoint, CONT_GAMBLEMACHINERESULT & kContResult, CONT_GAMBLEITEM & kContDumy, ERACE_TYPE eRaceType, bool & bBroadcast)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetMixupRoulette(iMixPoint, kContResult, kContDumy, eRaceType, bBroadcast);
}

int PgGambleMachine::GetMixupPoint(int const iGrade)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetMixupPoint(iGrade);
}

bool PgGambleMachine::CheckEnableMixupPoint(int const iMixPoint)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->CheckEnableMixupPoint(iMixPoint);
}

int	PgGambleMachine::GetMaxMixupPoint() const
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetMaxMixupPoint();
}

bool PgGambleMachine::MakeEnablePoint(int const iMixPoint,int & iEnablePoint)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->MakeEnablePoint(iMixPoint, iEnablePoint);
}

bool PgGambleMachine::IsCanCostumeMixup(ERACE_TYPE eRaceType)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->IsCanCostumeMixup(eRaceType);
}