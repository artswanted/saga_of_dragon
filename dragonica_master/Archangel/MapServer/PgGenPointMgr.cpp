
#include "stdafx.h"
#include "Variant/Global.h"
#include "Variant/MonsterDefMgr.h"
#include "PgGenPointMgr.h"
#include "Global.h"

PgGenPoint::PgGenPoint()
{
	LastGenTime(0);
	TotalRaiseRate(0);
}

PgGenPoint::~PgGenPoint()
{
}

bool PgGenPoint::Build(CONT_DEF_MONSTER_BAG_ELEMENTS::mapped_type const &rkMonsterBag, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &rkSuccessRateControl)
{
	int i = 0;
	while(i != MAX_SUCCESS_RATE_ARRAY)
	{//	while - 한개의 아이템 백에 있는 모든 아이템정보를 검색
		BagElementCont::value_type Element( rkMonsterBag.aElement[i], rkSuccessRateControl.aRate[i]);
		
		if(!Element.IsEmpty())
		{
			m_kContBag.push_back(Element);
		}
		++i;
	}
	AccTotalRaiseValue();
	if(TotalRaiseRate() != rkSuccessRateControl.iTotal)
	{
		INFO_LOG(BM::LOG_LV8, __FL__<<L"Diff TotalRaiseValue MapNo:"<<Info().iMapNo<<L", BagControlNo:"<<Info().iBagControlNo<<L", PosGuid:"<<Info().kPosGuid<<L", AccTotal:"<<TotalRaiseRate()<<L", ControlTotal:"<<rkSuccessRateControl.iTotal);
	}
	TotalRaiseRate(std::max(TotalRaiseRate(), rkSuccessRateControl.iTotal));

	//LastGenTime(BM::GetTime32());
	LastGenTime(0);
	return true;
}

bool PgGenPoint::Build( TBL_DEF_MAP_REGEN_POINT const &rkInfo, CONT_DEF_MONSTER_BAG_ELEMENTS::mapped_type const &rkMonsterBag, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &rkSuccessRateControl )
{
	if( Build(rkMonsterBag, rkSuccessRateControl) )
	{
		Info(rkInfo);
		return true;
	}
	return false;
}

bool PgGenPoint::Build(CONT_DEF_ITEM_BAG_ELEMENTS::mapped_type const & ItemBag, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const & SuccessRateControl)
{
	int i = 0;
	while(i != MAX_SUCCESS_RATE_ARRAY)
	{//	while - 한개의 아이템 백에 있는 모든 아이템정보를 검색
		BagElementCont::value_type Element( ItemBag.aElement[i], SuccessRateControl.aRate[i]);
		
		if( !Element.IsEmpty() )
		{
			m_kContBag.push_back(Element);
		}
		++i;
	}
	AccTotalRaiseValue();

	//LastGenTime(BM::GetTime32());
	LastGenTime(0);
	return true;
}

bool PgGenPoint::Build(TBL_DEF_MAP_REGEN_POINT const & rkInfo, CONT_DEF_ITEM_BAG_ELEMENTS::mapped_type const & ItemBag, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const & SuccessRateControl)
{
	if( Build(ItemBag, SuccessRateControl) )
	{
		Info(rkInfo);
		return true;
	}
	return false;
}

bool PgGenPoint::CanGen()const
{//젠할게 없고. 라스트 젠시간과 피리어드를 합친게 같으면.
#ifdef AI_DEBUG
	if (!m_kOwnChild.size())
#else
	bool const bNoChild = m_kOwnChild.empty();
	bool const bFirstTime = (LastGenTime() == 0 && Info().dwPeriod >= 0); // 처음일 때에는 무조건 Gen 시킨다.(dwPeriod < 0 이면 Gen 시키지 않는다.)
	
	bool const bGenTime = 
	(	LastGenTime() > 0 
	&&	BM::GetTime32() - LastGenTime() >= Info().dwPeriod
	&&	Info().dwPeriod > 0);

	if( bNoChild
	&&	(bFirstTime || bGenTime) )
#endif
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGenPoint::AddChild( BM::GUID const &kUnitGuid)
{
	auto ret = m_kOwnChild.insert(kUnitGuid);
	if(!ret.second)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"map insert failed");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data!"));
	}	
	LastGenTime(0);

	return ret.second;
}

bool PgGenPoint::RemoveChild(BM::GUID const &kUnitGuid)
{
	m_kOwnChild.erase(kUnitGuid);
	if (m_kOwnChild.empty())
	{
		LastGenTime(BM::GetTime32());	// <--- GenTime 계산은 몬스터 죽었을 때 부터의 경과시간으로 해야 한다.
	}
	return true;
}

void PgGenPoint::RemoveAll()
{
	m_kOwnChild.clear();
	LastGenTime(BM::GetTime32());
}

HRESULT PgGenPoint::PopMonster(int &rOutResult)
{
	HRESULT hRet = E_FAIL;
	if( !TotalRaiseRate() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return hRet;
	}

	int const iResult = (BM::Rand_Index(TotalRaiseRate())); 
	
	int iAcc = 0;
	BagElementCont::const_iterator itor = m_kContBag.begin();
//Random 돌려서 나온 값을. 
	while( m_kContBag.end() != itor ) // first key : ItemNo
	{
		BagElementCont::value_type const& rElement = (*itor);
		
		if( rElement.iMonNo > 0)//몬스터 번호 있을때.
		{
			iAcc += rElement.nRate;
		}
	
		if( iResult <= iAcc )
		{
			rOutResult = rElement.iMonNo;
			hRet = S_OK;
			break;
		}
		++itor;
	}

	//LastGenTime(BM::GetTime32());
	LastGenTime(0);// <--- GenTime 계산은 몬스터 죽었을 때 부터의 경과시간으로 해야 한다.
	return hRet;
}

HRESULT PgGenPoint::IsMonster(int const MonNo)const
{
	if ( MonNo <= 0 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL ;
	}

	BagElementCont::const_iterator itor = m_kContBag.begin();

	while( m_kContBag.end() != itor )
	{
		BagElementCont::value_type const& rElement = (*itor);
		
		if( rElement.iMonNo > 0 && rElement.iMonNo == MonNo )
		{
			return S_OK;
		}

		++itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgGenPoint::AccTotalRaiseValue()
{
	BagElementCont::const_iterator itor = m_kContBag.begin();
	
	int iAcc = 0;
	while(m_kContBag.end() != itor )
	{//전체 확률 갱신을 위한 while문
		const BagElementCont::value_type &rElement = (*itor);

		if(rElement.iMonNo)//나올놈이 있으면...
		{
			iAcc += rElement.nRate;
		}
		++itor;
	}

	TotalRaiseRate(iAcc);	// 하나의 ItemBag 에서 RaiseRate 의 총합

	if(TotalRaiseRate() > SHRT_MAX)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Overflow RaiseRate MapNo:"<<Info().iMapNo<<L", BagControlNo:"<<Info().iBagControlNo<<L", PosGuid:"<<Info().kPosGuid);
	}
}

// -> GetPoint

// GetPoint Object ->
PgGenPoint_Object::PgGenPoint_Object()
	:	m_dwRemoveAllTime(0)
{

}

PgGenPoint_Object::PgGenPoint_Object(BM::GUID const &rkGuid)
	:	m_dwRemoveAllTime(0)
{
	m_kInfo.kPosGuid = rkGuid;
}

PgGenPoint_Object::~PgGenPoint_Object()
{

}

bool PgGenPoint_Object::Build(	TBL_DEF_MAP_REGEN_POINT const &kInfo, 
								CONT_DEF_OBJECT_BAG const &kObjBags,
								CONT_DEF_OBJECT_BAG_ELEMENTS const &kObjElements )
{
	CONT_DEF_OBJECT_BAG::const_iterator bag_itr = kObjBags.find(kInfo.iBagControlNo);
	if ( bag_itr != kObjBags.end() )
	{
		for ( int i=0; i!=MAX_OBJECT_ELEMENT_NO; ++i )
		{
			if ( bag_itr->second.iElementNo[i] )
			{
				CONT_DEF_OBJECT_BAG_ELEMENTS::const_iterator element_itr = kObjElements.find(bag_itr->second.iElementNo[i]);
				if ( element_itr != kObjElements.end() )
				{
					SObjUnitBaseInfo kBaseInfo( SObjUnitGroupIndex(kInfo.kPosGuid,i), element_itr->second.iObjectNo );
					kBaseInfo.pt3Pos = kInfo.pt3Pos + element_itr->second.pt3RelativePos;
					m_kGroup.push_back(kBaseInfo);
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"Not Found TB_OBJECT_BAG_ELEMENTS["<<bag_itr->second.iElementNo[i]<<L"] From TB_OBJECT_BAG["<<kInfo.iBagControlNo<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data!"));
				}
			}
		}

		m_kInfo = kInfo;
		Reset();
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"Not Found TB_OBJECT_BAG["<<kInfo.iBagControlNo<<L"] From TBL_DEF_MAP_REGEN_POINT's GUID["<<kInfo.kPosGuid<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgGenPoint_Object::GetBaseInfo( OwnGroupCont &rkOutCont )const
{
	rkOutCont = m_kGroup;
}

bool PgGenPoint_Object::CanGen(DWORD const dwCurTime)const
{//젠할게 없고. 라스트 젠시간과 피리어드를 합친게 같으면.
	if ( m_kOwnChild.empty() )
	{
		if ( m_dwRemoveAllTime && ( Info().dwPeriod < ::DifftimeGetTime(m_dwRemoveAllTime,dwCurTime)) )
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGenPoint_Object::AddChild(BM::GUID const &kUnitGuid)
{
	auto ret = m_kOwnChild.insert(kUnitGuid);
	if(!ret.second)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"map insert failed");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Inser Failed Data!"));
	}

	return ret.second;
}

bool PgGenPoint_Object::RemoveChild(BM::GUID const &kUnitGuid)
{
	m_kOwnChild.erase(kUnitGuid);
	if (m_kOwnChild.empty())
	{
		if ( Info().dwPeriod != 0 )
		{
			m_dwRemoveAllTime = __max( BM::GetTime32(), 1 );
		}
		else
		{
			m_dwRemoveAllTime = 0;
		}
		
	}
	return true;
}

void PgGenPoint_Object::RemoveAll()
{
	m_kOwnChild.clear();
	m_dwRemoveAllTime = 0;
}

void PgGenPoint_Object::Reset()
{
	m_dwRemoveAllTime = __max( BM::GetTime32() - Info().dwPeriod+1, 1);
}


// <- GetPoint Object 

PgGenPointMgr::PgGenPointMgr(void)
{
}

PgGenPointMgr::~PgGenPointMgr(void)
{
}

void PgGenPointMgr::Clear()
{
	m_kGen_Monster.clear();
	m_kGen_Object.clear();
	m_kGen_SummonNPC.clear();
}

void PgGenPointMgr::swap(PgGenPointMgr &rRight)
{
	m_kGen_Monster.swap(rRight.m_kGen_Monster);
	m_kGen_Object.swap(rRight.m_kGen_Object);
	m_kGen_SummonNPC.swap(rRight.m_kGen_SummonNPC);
}

bool PgGenPointMgr::Build(	CONT_DEF_MAP_REGEN_POINT const &rkMonsterRegen,
							CONT_DEF_MONSTER_BAG_CONTROL const &rkMonsterBagControl,
							CONT_DEF_MONSTER_BAG const &rkMonsterBag,
							CONT_DEF_MONSTER_BAG_ELEMENTS const &rkMonsterBagElements,
							CONT_DEF_SUCCESS_RATE_CONTROL const &rkSuccessRateControl,
							CONT_DEF_OBJECT_BAG const &rkObjectBag,
							CONT_DEF_OBJECT_BAG_ELEMENTS const &rkObjectBagElements,
							CONT_DEF_ITEM_BAG const & ItemBag,
							CONT_DEF_ITEM_BAG_ELEMENTS const & ItemBagElements,
							CONT_MAP_BUILD_DATA const &rkMapBuildData)
{
	bool bReturn = true;
	// Data verifying.....
	// MonsterBagElement 에 있는 MonsterNo가 실제로 존재하는 것인지 검사.
	INFO_LOG(BM::LOG_LV7, __FL__<<L"Monster Bag Element verifying Count["<<rkMonsterBagElements.size()<<L"]....");
	CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator kBagElementItor = rkMonsterBagElements.begin();
	GET_DEF(CMonsterDefMgr, kMonsterDefMgr);
	while (rkMonsterBagElements.end() != kBagElementItor)
	{
		for (int i=0; i<MAX_SUCCESS_RATE_ARRAY; i++)
		{
			if (kBagElementItor->second.aElement[i] != 0 && kMonsterDefMgr.GetDef(kBagElementItor->second.aElement[i]) == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot Monster Def BagElement ID["<<kBagElementItor->first<<L"], MonsterID["<<kBagElementItor->second.aElement[i]<<L"]");
				bReturn = false;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
			}
		}
		++kBagElementItor;
	}

	CONT_DEF_BS_GEN_GROUND const* pkDefBSGenGround = NULL;
	g_kTblDataMgr.GetContDef(pkDefBSGenGround);
	if(!pkDefBSGenGround)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Def BattleSquare Gen Greound is NULL");
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
		return bReturn;
	}

	CONT_MAP_BUILD_DATA::const_iterator map_it = rkMapBuildData.begin();
	std::set<int> kMapSet;
	while(rkMapBuildData.end() != map_it)
	{
		kMapSet.insert((*map_it).iGroundNo);

		//
		CONT_DEF_BS_GEN_GROUND::const_iterator def_it=pkDefBSGenGround->find((*map_it).iGroundNo);
		if(def_it!=pkDefBSGenGround->end())
		{
			CONT_DEF_BS_GEN_GROUND::mapped_type const& kCont = (*def_it).second;
			CONT_DEF_BS_GEN_GROUND::mapped_type::const_iterator c_it = kCont.begin();
			while(c_it!=kCont.end())
			{
				kMapSet.insert((*c_it));
				++c_it;
			}
		}

		++map_it;
	}

	int iIndex = 0 ;
	CONT_DEF_MAP_REGEN_POINT::const_iterator regen_itor = rkMonsterRegen.begin();
	while(regen_itor != rkMonsterRegen.end())
	{
#ifdef AI_DEBUG
		//if (regen_itor->second.kPosGuid != BM::GUID(_T("D5FCC928-F7F3-4564-8811-FB4F3ABE32E9")))	// 9010100 첫번째 몬스터
		//if (regen_itor->second.kPosGuid != BM::GUID(_T("C45FD6A4-C6E1-4F8F-9434-CF99E120EA3A")))	// 9010391 윗쪽 가장 오른쪽 늑대
		//if (regen_itor->second.kPosGuid != BM::GUID(_T("79249A1D-D73D-4C1C-A2FC-9B0A60767149")))	// 9010391
		//if (regen_itor->second.kPosGuid != BM::GUID(_T("FC048881-036A-4566-A890-DD52DA3BE1DF")))	// 미션맵(9010108)압둘라
		//if (regen_itor->second.kPosGuid != BM::GUID(_T("022500D4-1E90-470E-84EE-41273062FAB9")))	// 9010391 왼쪽 아래 첫번째 초목이
		//if (regen_itor->second.kPosGuid != BM::GUID(_T("2CEED56D-17E0-4508-B538-FE613E162932")))	// 배가본드
		//if (regen_itor->second.kPosGuid != BM::GUID(_T("79249A1D-D73D-4C1C-A2FC-9B0A60767149")))	// 9010391 아래쪽 독거미
		if (regen_itor->second.kPosGuid != BM::GUID(_T("F44FD1AE-A3D6-4C20-BDB1-0066ABC62E06")))
		{
			++regen_itor;
			continue;
		}
#endif

		const TBL_DEF_MAP_REGEN_POINT &rRegenElement = (*regen_itor).second;
		int const iMapNo = rRegenElement.iMapNo;
		int const iMonBagControlNo = rRegenElement.iBagControlNo;
		BM::GUID const &kPosGuid = rRegenElement.kPosGuid;

		if(kMapSet.find(iMapNo) == kMapSet.end())
		{
			++regen_itor;
			continue;
		}
		
		switch( rRegenElement.cBagControlType )	// Type : ERegenBagControlType
		{
		case ERegenBag_Type_Monster:
			{
				CONT_DEF_MONSTER_BAG_CONTROL::const_iterator bag_control_itor = rkMonsterBagControl.find(iMonBagControlNo);

				if(bag_control_itor == rkMonsterBagControl.end())
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"[TB_DefMonsterRegen] in 'Monster Bag Control No["<<iMonBagControlNo<<L"]' is not found from [TB_DefMonsterBag]");
					bReturn = false;
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
					break;
				}

				TBL_DEF_MAP_REGEN_POINT kRegenPoint = rRegenElement;
				for(int iIDX = 0; MAX_MONSTERBAG_ELEMENT > iIDX ; iIDX++ )
				{
					int const iMonBagNo = (*bag_control_itor).second.aBagElement[iIDX];

					if(iMonBagNo)
					{
						CONT_DEF_MONSTER_BAG::const_iterator bag_itor = rkMonsterBag.find(iMonBagNo);
						if(bag_itor == rkMonsterBag.end())
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Monster BagNo["<<iMonBagNo<<L"] cannot find");
							bReturn = false;
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
						}
						else
						{
							CONT_DEF_MONSTER_BAG::mapped_type const &rBag = (*bag_itor).second;

							CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator element_itor = rkMonsterBagElements.find(rBag.iElementNo);
							CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator rate_itor = rkSuccessRateControl.find(rBag.iRateNo);

							if( element_itor == rkMonsterBagElements.end() || rate_itor == rkSuccessRateControl.end() )
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"MonBag["<<iMonBagNo<<L"] BagElement["<<rBag.iElementNo<<L"] Rate["<<rBag.iRateNo<<L"] Failed !!");
								bReturn = false;
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
							}
							else
							{
								auto ret = m_kGen_Monster.insert( std::make_pair( SGenGroupKey(iMapNo,iIDX), PgGenGroup<PgGenPoint>() ) );

								kRegenPoint.iTunningNo = GetMonsterBagTunningNo(rBag.iTunningNo_Min, rBag.iTunningNo_Max);
								PgGenPoint kGenPoint;
								if ( kGenPoint.Build(kRegenPoint, (*element_itor).second, (*rate_itor).second) )
								{
									(*ret.first).second.insert( kGenPoint.Info().iPointGroup, kGenPoint.Info().kPosGuid, kGenPoint );
								}
							}
						}
					}
				}
			}break;
		case ERegenBag_Type_InstanceItem:
			{
				TBL_DEF_MAP_REGEN_POINT RegenPoint = rRegenElement; 
				CONT_DEF_ITEM_BAG::const_iterator bag_itor = ItemBag.find(CONT_DEF_ITEM_BAG::key_type(iMonBagControlNo, 1)); // 인스턴스 아이템은 iMonBagContorlNo를 통해서 바로 아이템 백번호를 찾음.
				if( bag_itor == ItemBag.end() )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Item BagNo[" << iMonBagControlNo << L"] cannot find");
					bReturn = false;
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
				}
				else
				{
					CONT_DEF_ITEM_BAG::mapped_type const & Bag = (*bag_itor).second;

					CONT_DEF_ITEM_BAG_ELEMENTS::const_iterator element_itor = ItemBagElements.find(Bag.iElementsNo);
					CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator rate_itor = rkSuccessRateControl.find(Bag.iRaseRateNo);
					
					if( element_itor == ItemBagElements.end() || rate_itor == rkSuccessRateControl.end() )
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"ItemBag[" << iMonBagControlNo << L"] BagElement[" << Bag.iElementsNo << L"] Rate [" << Bag.iRaseRateNo << L"] Failed !!");
						bReturn = false;
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
					}
					else
					{
						auto ret = m_kGen_Monster.insert(std::make_pair( SGenGroupKey(iMapNo, 0), PgGenGroup<PgGenPoint>() ));

						PgGenPoint GenPoint;
						if( GenPoint.Build( RegenPoint, (*element_itor).second, (*rate_itor).second) )
						{
							(*ret.first).second.insert( GenPoint.Info().iPointGroup, GenPoint.Info().kPosGuid, GenPoint );
						}
					}
				}
			}break;
		case ERegenBag_Type_OjbUnit:
			{
				auto ret = m_kGen_Object.insert( std::make_pair(iMapNo, PgGenGroup<PgGenPoint_Object>()) );

				PgGenPoint_Object kObejctPoint;
				if ( kObejctPoint.Build( rRegenElement, rkObjectBag, rkObjectBagElements ) )
				{
					(*ret.first).second.insert( kObejctPoint.Info().iPointGroup, kObejctPoint.Info().kPosGuid, kObejctPoint );
				}
			}break;
		case ERegenBag_Type_SummonNPC:
			{
				auto ret = m_kGen_SummonNPC.insert( std::make_pair(iMapNo, PgGenGroup<PgGenPoint_Object>()) );

				PgGenPoint_Object kObejctPoint;
				if ( kObejctPoint.Build( rRegenElement, rkObjectBag, rkObjectBagElements ) )
				{
					(*ret.first).second.insert( kObejctPoint.Info().iPointGroup, kObejctPoint.Info().kPosGuid, kObejctPoint );
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Unknown BagControlType ["<<rRegenElement.cBagControlType<<L"]");
				bReturn = false;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
			}break;
		}	
		++regen_itor;
	}
	return bReturn;
}

HRESULT PgGenPointMgr::GetGenPoint_Monster( ContGenPoint_Monster &rOut, CONT_REGEN_GROUP_MONSTER::key_type const &kKey, bool const bReset, int const iGenGroup ) const
{
	CONT_REGEN_GROUP_MONSTER::const_iterator group_itr = m_kGen_Monster.find(kKey);
	if( group_itr != m_kGen_Monster.end())
	{
		return group_itr->second.Get( rOut, bReset, iGenGroup );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgGenPointMgr::GetGenPoint_Object( ContGenPoint_Object &rOut, CONT_REGEN_GROUP_OBJECT::key_type const &kKey, bool const bReset, int const iGenGroup ) const
{
	CONT_REGEN_GROUP_OBJECT::const_iterator group_itr = m_kGen_Object.find(kKey);
	if( group_itr != m_kGen_Object.end())
	{
		return group_itr->second.Get( rOut, bReset, iGenGroup );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgGenPointMgr::GetGenPoint_SummonNPC( ContGenPoint_Object &rOut, CONT_REGEN_GROUP_OBJECT::key_type const &kKey, bool const bReset, int const iGenGroup ) const
{
	CONT_REGEN_GROUP_OBJECT::const_iterator group_itr = m_kGen_SummonNPC.find(kKey);
	if( group_itr != m_kGen_SummonNPC.end())
	{
		return group_itr->second.Get( rOut, bReset, iGenGroup );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}
