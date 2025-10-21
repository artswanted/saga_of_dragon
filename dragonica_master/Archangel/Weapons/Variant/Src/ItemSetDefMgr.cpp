#include "StdAfx.h"
#include "ItemSetDefMgr.h"
#include "DefAbilType.h"
#include "PgControlDefMgr.h"
#include "inventoryutil.h"

////////////////////////////////////////////////////////////////////////////////////
//	CItemDefMgr
////////////////////////////////////////////////////////////////////////////////////

CItemSetDef::CItemSetDef()
{
}

CItemSetDef::~CItemSetDef()
{
}

int CItemSetDef::CheckNeedItem(CONT_HAVE_ITEM_DATA const &kContItem,CUnit const * pkUnit, bool& rbCompleteSet)const
{
	size_t const total_need_count = m_kContElement.size();

	if( !total_need_count )//m_kContElement.size() 와 같음.
	{//요구 사항이 없음.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		rbCompleteSet = false;
		return false;
	}

	CONT_HAVE_ITEM_NO kContTemp = m_kContElement;
	
	CONT_HAVE_ITEM_DATA::const_iterator list_itor = kContItem.begin();
	while(list_itor != kContItem.end())
	{
		if(false == PgInventoryUtil::CheckDisableItem((*list_itor).second,pkUnit))
		{
			kContTemp.erase((*list_itor).first);
		}
		++list_itor;
	}
	rbCompleteSet = kContTemp.empty(); // 모두 비면 모든 아이템 착용 완료
	return static_cast< int >(m_kContElement.size() - kContTemp.size());
}

void CItemSetDefMgr::Clear()
{
	TDefMgr< TBL_DEF_ITEM_SET, TBL_DEF_ITEM_OPTION_ABIL, CItemSetDef, TBL_KEY_INT, TBL_KEY_INT  >::Clear();
	
	m_kContItemToSet.clear();

	{
		CONT_DEF_DIVIDE_ABILS::iterator erase_iter = m_kContDivSetAbil.begin();
		while( m_kContDivSetAbil.end() != erase_iter )
		{
			m_DefPool.Delete( (*erase_iter).second );
			++erase_iter;
		}
		m_kContDivSetAbil.clear();
	}
}

void CItemSetDefMgr::swap(CItemSetDefMgr& rkRight)
{
	m_contDef.swap(rkRight.m_contDef);
	m_DefPool.swap(rkRight.m_DefPool);
	m_kContItemToSet.swap(rkRight.m_kContItemToSet);
}

int CItemSetDefMgr::GetItemSetNo(int const iItemNo) const
{
	CONT_ITEM_TO_SET::const_iterator itor = m_kContItemToSet.find(iItemNo);
	if(itor != m_kContItemToSet.end())
	{
		return (*itor).second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

CItemSetDef const * CItemSetDefMgr::GetEquipAbilDef(int const iSetNo, int const iEquipPiece)const
{
	if( 0 >= iEquipPiece )
	{
		return NULL;
	}
	CItemSetDef const * pkOrgDef = GetDef(iSetNo);
	if(pkOrgDef)
	{
		CONT_DEF_DIVIDE_ABILS::const_iterator itor = m_kContDivSetAbil.find(POINT2(iSetNo, iEquipPiece));
		if(itor != m_kContDivSetAbil.end())
		{
			return (*itor).second;
		}
		else
		{
			return GetEquipAbilDef(iSetNo, iEquipPiece-1);
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

bool CItemSetDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil, CItemDefMgr const &rkItemDefMgr)
{
	bool bReturn = true;
	Clear();
	
	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	//전체 아이템을 해쉬화 시키고 거기서
	//세컨을 셋 넘버로 가지게 함.
	//유저 인벤에서 셋트 능력 뽑을때는 셋 std::set을 써서
	//찾아 올 수 있도록.
	while( tblBase.end() != base_itor )
	{
		DEF *pDef = NewDef();

		pDef->SetNo((*base_itor).second.iSetNo);
		pDef->NameNo((*base_itor).second.iNameNo);

		int iAbilCount = 0;
		int iIndex = 0;// 어빌은 1번만 사용한다
		//for(int iIndex = 0; MAX_ITEM_SET_ABIL > iIndex ; ++iIndex)
		{//기본 리스트 번호 찾고 
			int const iAbilNo = (*base_itor).second.aiAbilNo[iIndex];
			if( iAbilNo )
			{
				CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );
				if( tblAbil.end() != abil_itor )
				{
					(*abil_itor).second.iOptionAbilNo;
					CONT_TBL_DEF_ITEM_OPTION_ABIL_PIECE const& rkContPieceAbil = (*abil_itor).second.kContPieceAbil;
					CONT_TBL_DEF_ITEM_OPTION_ABIL_PIECE::const_iterator peice_iter = rkContPieceAbil.begin();
					while( rkContPieceAbil.end() != peice_iter )
					{
						CONT_TBL_DEF_ITEM_OPTION_ABIL_PIECE::value_type const& rkPieceAbil = (*peice_iter);
						for( int j = 0; MAX_OPTION_ABIL_ARRAY > j ; ++j)
						{
							int const iCurAbilType = rkPieceAbil.aiType[j];
							int const iCurAbilValue = rkPieceAbil.aiValue[j];
							if( 0 != iCurAbilType )
							{
								switch(iCurAbilType)
								{
								case AT_EXPERIENCE:
								case AT_MONEY:
								case AT_DIE_EXP:
								case AT_CLASSLIMIT:	
									{//__int64 어빌을 쓸일이 있나??
										// (*abil_itor).second.aValue[j] 값이 int 라서 int64로 건네줄때 값이 짤릴텐데
										// classlimit는 m_iClassLimit 에 직접 때려박는다지만 다른건 어떻게 해야하나
										// CONT_ABIL 를 int64용으로 또 만들어야 하나?
										VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Error!!");
										bReturn = false;
	//									pDef->SetAbil64(iCurAbilType, iCurAbilValue); 
									}break;
								default:
									{ 
										//빌드 타이밍에 에러 내주자
										if(pDef->HasAbil(iCurAbilType))
										{
											int const iPreAbilValue = pDef->GetAbil(iCurAbilType);
											VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("SetNo[") << pDef->SetNo() << _T("] is a Duplicate Have Avil [Type:") << iCurAbilType << _T(" Value:") << iPreAbilValue << _T("] in a [Table:TB_DefOptionAbil, OptionAbilNo:") << iAbilNo << _T(", Type") << j+1 << _T(":") <<  iCurAbilType << _T(", Value") << j+1 << _T(":") << iCurAbilValue << _T("]"));
											bReturn = false;
	//										return false;
										}

										pDef->SetAbil(iCurAbilType, iCurAbilValue);
										POINT2 const kDivKey(pDef->SetNo(), rkPieceAbil.iPieceCount);
	////////////////////////////////////////////////////////////////////////////// 셋트 아이템은 def 를 2중으로 만듬.
										CONT_DEF_DIVIDE_ABILS::iterator div_iter = m_kContDivSetAbil.find( kDivKey );
										if( m_kContDivSetAbil.end() != div_iter )
										{
											*(*div_iter).second = *pDef;
										}
										else
										{
											DEF *pCopyDef = NewDef();
											*pCopyDef = *pDef;
											m_kContDivSetAbil.insert( std::make_pair(kDivKey, pCopyDef) );
										}
	///////////////////////////////////////////////////////////////////////////////
									}break;
								}
							}
						}
						++peice_iter;
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find OptionAbil["<<iAbilNo<<L"], from [TB_DefItemOptionAbil]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
					bReturn = false;
				}
			}
		}

		for(int iIndex = 0; MAX_ITEM_SET_NEED_ITEM_ARRAY > iIndex ; ++iIndex)
		{//기본 리스트 번호 찾고 
			int const iItemNo = (*base_itor).second.aiNeedItemNo[iIndex];
			if( iItemNo )
			{
				CItemDef const* pItemDef = rkItemDefMgr.GetDef(iItemNo);
				if(pItemDef)
				{
					auto ret = m_kContItemToSet.insert(std::make_pair(iItemNo, (*base_itor).second.iSetNo));
					if(!ret.second)
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Already Insert ItemNo["<<iItemNo<<L"]");
						bReturn = false;
					}

					pDef->m_kContElement.insert(iItemNo);
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find ItemDef["<<iItemNo<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pItemDef is NULL"));
					bReturn = false;
				}
			}
		}

		auto kRet = m_contDef.insert( std::make_pair(pDef->SetNo(), pDef) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicater ItemSet No["<<pDef->SetNo()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			bReturn = false;
		}
		++base_itor;
	}

	return bReturn && DataCheck();
}

bool CItemSetDefMgr::DataCheck()const
{
/*	if(!m_contDef.size())
	{
		assert(NULL);
		return false;
	}

	bool bRet = true;
	CONT_DEF::const_iterator def_itor = m_contDef.begin();
	while(def_itor != m_contDef.end())
	{
		const DEF *pDef = (*def_itor).second;

		if(0 == pDef->GetAbil(AT_VIRTUAL_ITEM))
		{
			if(!pDef->GetAbil(AT_DEFAULT_AMOUNT))	
			{
				INFO_LOG(BM::LOG_LV5, _T("[%s] ItemDef NoAmount Item[%d]"), __FUNCTIONW__, (*def_itor).first);
//				return false;
				bRet = false;
			}//
			if(!pDef->GetAbil(AT_PRIMARY_INV))		
			{
				INFO_LOG(BM::LOG_LV5, _T("[%s] ItemDef NoPriInv Item[%d]"), __FUNCTIONW__, (*def_itor).first);
//				return false;
				bRet = false;
			}//
		}

		++def_itor;
	}
	return bRet;
}

int CItemDefMgr::GetAbil(int iItemNo, int iAbil)
{
	CItemDef const* pkDef = GetDef(iItemNo);
	if (!VERIFY_INFO_LOG(pkDef != NULL, BM::LOG_LV5, _T("[%s] Cannot Get ItemDef Item[%d]"), __FUNCTIONW__, iItemNo))
	{
		return 0;
	}
	return pkDef->GetAbil(iAbil);
}

bool CItemEnchantDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	Clear();
	
	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	while( tblBase.end() != base_itor )
	{
		DEF *pDef = m_DefPool.New();

		pDef->Type((*base_itor).second.Type);
		pDef->Lv((*base_itor).second.Lv);
		pDef->NameNo((*base_itor).second.NameNo);
		
		for(int iIndex = 0; MAX_ITEM_ENCHANT_ABIL_LIST > iIndex ; ++iIndex)
		{//기본 리스트 번호 찾고 
			int const iAbilNo = (*base_itor).second.aAbil[iIndex];
			if( iAbilNo )
			{
				CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );

				if( abil_itor != tblAbil.end() )
				{
					for( int j = 0; MAX_ITEM_ABIL_ARRAY > j ; ++j)
					{
						if(0 != (*abil_itor).second.aType[j])
						{
							switch((*abil_itor).second.aType[j])
							{
							case AT_EXPERIENCE:
							case AT_MONEY:
							case AT_DIE_EXP:
							case AT_CLASSLIMIT:	{ pDef->SetAbil((*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]); }break;
							default:			{ pDef->SetAbil( (*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]); }break;
							}
						}
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Can't Find Abil List"), __FUNCTIONW__, __LINE__);
				}
			}
		}

		m_contDef.insert( std::make_pair(pDef->Key(), pDef) );
		++base_itor;
	}
	return true;
}

bool CRareDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	Clear();
	
	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	while( tblBase.end() != base_itor )
	{
		DEF *pDef = m_DefPool.New();

		pDef->No((*base_itor).second.RareNo);
		pDef->NameNo((*base_itor).second.NameNo);
		

		m_contDef.insert( std::make_pair(pDef->No(), pDef) );
		++base_itor;
	}
*/
	return true;
}