//// Item Class
// Dukguru
//
#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <limits>
#include "Global.h"
#include "item.h"
#include "ItemDefMgr.h"
#include "TableDataManager.h"
#include "PgControlDefMgr.h"
#include "PgPremiumMgr.h"

bool PgItemExtendDataMgr::operator == (PgItemExtendDataMgr const & rhs) const
{
	for(CONT_ITEMEXTENDDATA::const_iterator iter = m_kCont.begin();iter != m_kCont.end();++iter)
	{
		CONT_ITEMEXTENDDATA::const_iterator finditer = rhs.Cont().find((*iter).first);
		if(finditer == rhs.Cont().end())
		{
			return false;
		}
		if((*iter).second != (*finditer).second)
		{
			return false;
		}
	}
	return true;
}

PgItemExtendDataMgr & PgItemExtendDataMgr::operator = (PgItemExtendDataMgr const & rhs)
{
	m_kCont = rhs.Cont();
	return *this;
}

SItemPos const PgItem_PetInfo::ms_kPetItemEquipPos( IT_FIT, EQUIP_POS_PET );

PgItem_PetInfo::SStateValue::SStateValue()
{
	Time(0);
	Value(0);
}

PgItem_PetInfo::SStateValue::SStateValue( int const iValue )
{
	Set( iValue );
}

int PgItem_PetInfo::SStateValue::Get()const
{
	return *(reinterpret_cast<int const*>(this));
}

void PgItem_PetInfo::SStateValue::Set( int const iValue )
{
	(*(reinterpret_cast<int*>(this))) = iValue;
}

bool PgItem_PetInfo::SStateValue::IsUseTimeOut( __int64 const i64TimeType )const
{
	__int64 const iNowTime = g_kEventView.GetLocalSecTime( i64TimeType );
	if ( CGameTime::MINUTE == i64TimeType )
	{
		return iNowTime >= (static_cast<__int64>(Time()) + PgItem_PetInfo::SKILL_BASE_TIME);
	}

	return iNowTime >= static_cast<__int64>(Time());
}

PgItem_PetInfo::PgItem_PetInfo(void)
:	m_i64Exp(0i64)
,	m_bySaveFlag(0)
,	m_iMP(0)
,	m_cColorIndex(0)
{
}

PgItem_PetInfo::PgItem_PetInfo( PgItem_PetInfo const &rhs )
:	m_kClassKey(rhs.m_kClassKey)
,	m_wstrName(rhs.m_wstrName)
,	m_i64Exp(rhs.m_i64Exp)
,	m_bySaveFlag(rhs.m_bySaveFlag)
,	m_kHealth(rhs.m_kHealth)
,	m_kMental(rhs.m_kMental)
,	m_iMP(rhs.m_iMP)
,	m_cColorIndex(rhs.m_cColorIndex)
{
	::memcpy( m_kSkill, rhs.m_kSkill, sizeof(m_kSkill) );
}

PgItem_PetInfo& PgItem_PetInfo::operator = ( PgItem_PetInfo const &rhs )
{
	m_kClassKey = rhs.m_kClassKey;
	m_wstrName = rhs.m_wstrName;
	m_i64Exp = rhs.m_i64Exp;
	m_bySaveFlag = rhs.m_bySaveFlag;
	m_kHealth = rhs.m_kHealth;
	m_kMental = rhs.m_kMental;
	m_iMP = rhs.m_iMP;
	m_cColorIndex = rhs.m_cColorIndex;
	::memcpy( m_kSkill, rhs.m_kSkill, sizeof(m_kSkill) );
	return *this;
}

bool PgItem_PetInfo::CopyTo( PgItem_ExtInfo *pkExtInfo )const
{
	PgItem_PetInfo * p = dynamic_cast<PgItem_PetInfo*>(pkExtInfo);
	if ( p )
	{
		*p = *this;
		return true;
	}
	return false;
}

bool PgItem_PetInfo::CheckStateAbil( int &iValue, bool const bCorrect )
{
	if ( MAX_PET_STATE_VALUE < iValue )
	{
		if ( true == bCorrect )
		{
			iValue = MAX_PET_STATE_VALUE;
		}	
	}
	else if ( iValue < 0 )
	{
		if ( true == bCorrect )
		{
			iValue = 0;
		}
	}
	else
	{
		return true;
	}
	return false;
}

bool PgItem_PetInfo::SetAbil( WORD const wType, int iValue )
{
	switch( wType )
	{
	case AT_CLASS:
		{
			m_kClassKey.iClass = iValue;
		}break;
	case AT_LEVEL:
		{
			if ( (SHRT_MAX < iValue) || (1 > iValue) )
			{
				return false;
			}

			m_kClassKey.nLv = static_cast<short>(iValue);
		}break;
	case AT_COLOR_INDEX:
		{
			if ( (SCHAR_MAX < iValue) || (0 > iValue) )
			{
				return false;
			}

			m_cColorIndex = static_cast<char>(iValue);
		}break;
	case AT_HEALTH:
		{
			CheckStateAbil( iValue, true );
			m_kHealth.Value(iValue);
			return true;
		}break;
	case AT_MENTAL:
		{
			CheckStateAbil( iValue, true );
			m_kMental.Value(iValue);
			return true;
		}break;
	case AT_MP:
		{
			if ( m_iMP == iValue )
			{
				return false;
			}
			m_iMP = iValue;
		}break;
	default:
		{
			return false;
		}break;
	}

// 	if ( 0 == iValue )
// 	{
// 		return 1 == m_kAbils.erase( wType );
// 	}
// 
// 	auto kPair = m_kAbils.insert( std::make_pair( wType, iValue ) );
// 	if ( !kPair.second )
// 	{
// 		if ( kPair.first->second == iValue )
// 		{
// 			return false;
// 		}
// 		kPair.first->second = iValue;
// 	}

	return true;
}

bool PgItem_PetInfo::AddAbil( WORD const wType, int iValue )
{
	switch( wType )
	{
	case AT_CLASS:
	case AT_LEVEL:
	case AT_COLOR_INDEX:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Not Use This Type<" << wType << L">" );
			return false;
		}break;
	}

	return SetAbil( wType, GetAbil(wType) + iValue );
}

int PgItem_PetInfo::GetAbil( WORD const wType )const
{
	switch( wType )
	{
	case AT_CLASS:{return m_kClassKey.iClass;}break;
	case AT_LEVEL:{return static_cast<int>(m_kClassKey.nLv);}break;
	case AT_HEALTH:{return static_cast<int>(m_kHealth.Value());}break;
	case AT_MENTAL:{return static_cast<int>(m_kMental.Value());}break;
	case AT_MP:{return m_iMP;}break;
	case AT_COLOR_INDEX:{return static_cast<int>(m_cColorIndex);}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Type<" << wType << L"> Error" );
		}break;
	}
	return 0;
}

bool PgItem_PetInfo::GetSkill( size_t const iIndex, SStateValue &rkOutValue )const
{
	if ( MAX_SKILL_SIZE > iIndex )
	{
		if ( m_kSkill[iIndex].Value() )
		{
			rkOutValue = m_kSkill[iIndex];
			return true;
		}
	}
	return false;
}

bool PgItem_PetInfo::SetSkill( size_t const iIndex, SStateValue const &kStateValue )
{
	if ( MAX_SKILL_SIZE > iIndex )
	{
		m_kSkill[iIndex] = kStateValue;
		return true;
	}
	return false;
}

bool PgItem_PetInfo::FindSkillIndex( unsigned int iSkillIndex, size_t &iOutIndex, bool const IsFindEmpty )const
{
	bool bFindEmpty = false;
	for ( size_t i=0; i<MAX_SKILL_SIZE; ++i )
	{
		if ( m_kSkill[i].Value() )
		{
			if ( iSkillIndex == m_kSkill[i].Value() )
			{
				iOutIndex = i;
				return true;
			}
		}
		else if ( !bFindEmpty )
		{
			bFindEmpty = IsFindEmpty;
			iOutIndex = i;
			break;
		}
	}
	return bFindEmpty;
}

void PgItem_PetInfo::WriteToPacket( BM::Stream &rkPacket ) const
{
	rkPacket.Push( m_kClassKey );
	rkPacket.Push( m_wstrName );
	rkPacket.Push( m_i64Exp );
	rkPacket.Push( m_bySaveFlag );
	rkPacket.Push( m_kHealth );
	rkPacket.Push( m_kMental );
	rkPacket.Push( m_iMP );
	rkPacket.Push( m_cColorIndex );
	rkPacket.Push( m_kSkill, sizeof(m_kSkill) );
}

bool PgItem_PetInfo::ReadFromPacket( BM::Stream &rkPacket )
{
	if ( rkPacket.RemainSize() >= min_size() )
	{
		m_bySaveFlag = 0;
		rkPacket.Pop( m_kClassKey );
		rkPacket.Pop( m_wstrName, MAX_PET_NAMELEN );
		rkPacket.Pop( m_i64Exp );
		rkPacket.Pop( m_bySaveFlag );
		rkPacket.Pop( m_kHealth );
		rkPacket.Pop( m_kMental );
		rkPacket.Pop( m_iMP );
		rkPacket.Pop( m_cColorIndex );
		return rkPacket.PopMemory( m_kSkill, sizeof(m_kSkill) );
	}
	return false;
}

size_t PgItem_PetInfo::max_size(void)const
{
	return	min_size()
		+	(sizeof(wchar_t) * MAX_PET_NAMELEN);
}

size_t PgItem_PetInfo::min_size(void)const
{
	return	sizeof(m_kClassKey)
		+	sizeof(size_t)
		+	sizeof(m_i64Exp)
		+	sizeof(m_bySaveFlag)
		+	sizeof(m_kHealth);
		+	sizeof(m_kMental);
		+	sizeof(m_iMP);
		+	sizeof(m_cColorIndex)
		+	sizeof(m_kSkill);
}

PgBase_Item::PgBase_Item( PgBase_Item const &rhs )
	: PgItemExtendDataMgr(rhs)
,	m_kGuid( rhs.m_kGuid )
,	m_kItemNo( rhs.m_kItemNo )
,	m_kCount( rhs.m_kCount )
,	m_kEnchantInfo( rhs.m_kEnchantInfo )
,	m_kCreateDate( rhs.m_kCreateDate )
,	m_kState( UIT_STATE_NORMAL )
,	m_pkExtInfo(NULL)
,	m_kStatTrackInfo(rhs.m_kStatTrackInfo)
{
	State( rhs.m_kState );

	if ( rhs.m_pkExtInfo )
	{
		rhs.m_pkExtInfo->CopyTo( m_pkExtInfo );
	}
}

PgBase_Item& PgBase_Item::operator = ( PgBase_Item const &rhs )
{
	this->::PgItemExtendDataMgr::operator=(rhs);

	Guid( rhs.Guid() );
	ItemNo( rhs.ItemNo() );
	Count( rhs.Count() );
	EnchantInfo( rhs.EnchantInfo() );
	CreateDate( rhs.CreateDate() );
	State( rhs.State() );
	StatTrackInfo(rhs.StatTrackInfo());

	if ( rhs.m_pkExtInfo )
	{
		rhs.m_pkExtInfo->CopyTo( m_pkExtInfo );
	}
	return *this;
}

PgBase_Item::~PgBase_Item()
{
	SAFE_DELETE(m_pkExtInfo);
}

bool PgBase_Item::State( BYTE const byState )
{
	m_kState = byState;

	if (	m_pkExtInfo
		&&	m_pkExtInfo->GetType() == m_kState )
	{
		return true;
	}

	SAFE_DELETE( m_pkExtInfo );
	
	switch ( m_kState )
	{
	case UIT_STATE_PET:
		{
			m_pkExtInfo = new_tr PgItem_PetInfo;
			if ( !m_pkExtInfo )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Memory Error" );
				return false;
			}
		}break;
	default:
		{
			
		}break;
	}

	return true;
}

void PgBase_Item::Swap( PgBase_Item & rkItem )
{
	m_kCont.swap( rkItem.m_kCont );
	std::swap( m_kGuid, rkItem.m_kGuid );
	std::swap( m_kItemNo, rkItem.m_kItemNo );
	std::swap( m_kCount, rkItem.m_kCount );
	std::swap( m_kEnchantInfo, rkItem.m_kEnchantInfo );
	std::swap( m_kCreateDate, rkItem.m_kCreateDate );
	std::swap( m_pkExtInfo, rkItem.m_pkExtInfo );
	std::swap( m_kState, rkItem.m_kState );
}

PgItemWrapper::PgItemWrapper()
{
	Clear();
}

PgItemWrapper::PgItemWrapper( PgItemWrapper const &rhs )
:	PgBase_Item( rhs )
,	m_kPos(rhs.m_kPos)
{
	IsCreated( rhs.IsCreated() );
	IsModify( rhs.IsModify() );
	IsRemove( rhs.IsRemove() );
}

PgItemWrapper::PgItemWrapper(PgBase_Item const &kInItem, SItemPos const &kinPos)
{
	Clear();
	PgBase_Item::operator=(kInItem);
	m_kPos = kinPos;
}

PgItemWrapper& PgItemWrapper::operator = ( PgItemWrapper const &rhs )
{
	PgBase_Item::operator=(rhs);
	m_kPos = rhs.m_kPos;
	IsCreated( rhs.IsCreated() );
	IsModify( rhs.IsModify() );
	IsRemove( rhs.IsRemove() );
	return *this;
}

PgItemWrapper::~PgItemWrapper()
{
}

void PgItemWrapper::Clear()
{
	PgBase_Item::Clear();
	IsCreated(false);//생성되었느냐(DB저장이 안되었다)
	IsModify(false);//정보가 변경 되었는가
	IsRemove(false);//지워졌는가 -> 지워진건 다른 액션을 절대 하지 않도록.

	m_kPos.Clear();//인벤토리같은곳에서 사용
}

void PgItemWrapper::SetItem(PgBase_Item const &rkItem)
{
	if(PgBase_Item::IsEmpty(&rkItem))
	{
		IsRemove(true);
	}
	else
	{
		IsModify(true);
	}
	PgBase_Item::operator=(rkItem);
}

void PgItemWrapper::SetItemPos(SItemPos const &rkItemPos)
{
	IsModify(true);
	Pos(rkItemPos);
}

HRESULT CalcRepairNeedMoney(PgBase_Item const &rkItem, int const iAddRepairCostRate, int &iOutValue)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pDef = kItemDefMgr.GetDef(rkItem.ItemNo());
	if(pDef && pDef->CanEquip())
	{
		if(!IsCanRepair(rkItem.ItemNo()))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1219"));
			return 1219;//TextTable No
		}
		int const iMaxDur = rkItem.MaxDuration();
		if( (0 < iMaxDur
		&&	rkItem.Count() < iMaxDur)
		||	rkItem.EnchantInfo().IsNeedRepair())//리페어 요구는 무조건 수리. 현재 내구 0 으로 계산
		{	//여기서 돈 셋팅.
			int const iSellPrice = pDef->SellPrice();
			int const iNowDur = (rkItem.EnchantInfo().IsNeedRepair()?(0):(rkItem.Count()));//리페어 요구면 0 으로 계산.

			//iOutValue = static_cast<int>((pDef->BuyPrice()/iMaxDur)) * static_cast<int>(iMaxDur-iNowDur);
			iOutValue = int(static_cast<float>(((float)pDef->BuyPrice()/(float)iMaxDur)) * static_cast<float>(iMaxDur-iNowDur) * 0.7f * (1.0f + ((float)rkItem.EnchantInfo().Rarity()/70.0f)) * 0.1f);

//			아이템가격(Price)*(1-현재내구도/최대내구도)*(1+영력/100)

			iOutValue = __max(iOutValue,1);//-는 안되게.

//			iOutValue = (int)(((float)iSellPrice/(float)iMaxDur) * (1.0f- (float)iNowDur/(float)iMaxDur)*120 * (1.0f+ (float)rkItem.EnchantInfo().PlusLv()/(float)3));
			if ( iAddRepairCostRate )
			{
				iOutValue += std::max( -iOutValue, SRateControl::GetValueRate( iOutValue, iAddRepairCostRate ) );
			}
			return S_OK;
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1220"));
			return 1220;//TextTable No
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1219"));
	return 1219;//TextTable No
}

bool CheckIsCashItem(PgBase_Item const &kItem)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if( pDef && pDef->IsType(ITEM_TYPE_AIDS) )
	{
		return true;
	}
	return false;
}

E_ITEM_GRADE GetItemGrade(PgBase_Item const &kItem)
{
	if(kItem.EnchantInfo().IsSeal())
	{
		return IG_SEAL;
	}

	if(kItem.EnchantInfo().IsCurse())
	{
		return IG_CURSE;
	}

	const CONT_DEF_ITEM_RARITY_UPGRADE* pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);

	int const iRarity = static_cast<int>(kItem.EnchantInfo().Rarity());

	CONT_DEF_ITEM_RARITY_UPGRADE::const_iterator itor = pCont->begin();
	while( itor != pCont->end() )
	{
		if( (*itor).second.iItemRarityMin <= iRarity && (*itor).second.iItemRarityMax >= iRarity )
		{
			return static_cast<E_ITEM_GRADE>((*itor).second.iItemGrade);
		}
		++itor;
	}

	return IG_NORMAL;
}

HRESULT RarityValue(E_ITEM_GRADE const eItemGrade, short &nMin, short &nMax)
{
	const CONT_DEF_ITEM_RARITY_UPGRADE* pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);

	CONT_DEF_ITEM_RARITY_UPGRADE::const_iterator itor = pCont->find(eItemGrade);
	if(itor == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	nMin = (*itor).second.iItemRarityMin;
	nMax = (*itor).second.iItemRarityMax;

	return S_OK;
}

HRESULT GenRarityValue(E_ITEM_GRADE const eItemGrade, short & nRet, bool const bGenItem)
{
	short nMin=0, nMax=0;
	if(S_OK == RarityValue(eItemGrade, nMin, nMax))
	{
		nRet = nMin;

		short const nGab = (nMax-nMin);

		if(0 < nGab)
		{
			nRet += BM::Rand_Index(nGab);

			if(bGenItem)
			{
				nRet = __min(GEN_RARITY_ARTIFACT_LIMIT,nRet);// 90 이상 레어도를 가진 아이템은 생성되지 않음...
			}
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CreatePetInfo( SClassKey const &kPetClassKey, PgItem_PetInfo *pkPetInfo, SEnchantInfo *pkEnchantInfo )
{
	if ( !pkPetInfo )
	{
		return E_FAIL;
	}

	GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
	PgClassPetDef kPetDef;
	if ( !kClassPetDefMgr.GetDef( kPetClassKey, &kPetDef ) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found PetClass<" << kPetClassKey.iClass << L"> Level<" << kPetClassKey.nLv << L">" );
		return E_FAIL;
	}

	pkPetInfo->ClassKey( kPetClassKey );

	CONT_DEFSTRINGS const * pContDefStrings = NULL;
	g_kTblDataMgr.GetContDef( pContDefStrings );
	if ( pContDefStrings )
	{
		int const iNameNo = kPetDef.GetAbil(AT_NAMENO);
		CONT_DEFSTRINGS::const_iterator str_itr = pContDefStrings->find( iNameNo );
		if( str_itr != pContDefStrings->end() )
		{
			std::wstring wstrText = str_itr->second.strText;
			if( PgItem_PetInfo::MAX_PET_NAMELEN < wstrText.length() )
			{ // DEFSTRINGS에서 찾은 펫 이름이 제한글자를 넘어섰을 경우 자른다.
				std::wstring kTemp = wstrText;
				wstrText = kTemp.substr(0, PgItem_PetInfo::MAX_PET_NAMELEN);
			}
			pkPetInfo->Name( wstrText );
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found PetClass<" << kPetClassKey.iClass << L"> Level<" << kPetClassKey.nLv << L">'s Name<" << iNameNo << L">" );
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error" );
	}

	pkPetInfo->Exp( kPetDef.GetAbil64( AT_EXPERIENCE ) );

	switch ( kPetDef.GetPetType() )
	{
	case EPET_TYPE_1:
		{
		}break;
	case EPET_TYPE_2:
	case EPET_TYPE_3: //현재 성장형펫과 라이딩펫은 동일하게 처리
		{
			pkPetInfo->SetAbil( AT_MP, kPetDef.GetAbil( AT_MAX_MP ) );
			
//			pkPetInfo->SetAbil( AT_HEALTH, PgItem_PetInfo::MAX_PET_STATE_VALUE );
//			pkPetInfo->SetAbil( AT_MENTAL, PgItem_PetInfo::MAX_PET_STATE_VALUE );
		}break;
	default:
		{
		}break;
	}

	pkPetInfo->SaveFlag( PgItem_PetInfo::SVFL_HATCHING );// 저장되어야 한다.

	if ( pkEnchantInfo )
	{
		kPetDef.GetPetItemOption( *pkEnchantInfo );;
	}
	return S_OK;
}


enum E_SEAL_CURSE_STATE
{
	SCS_NONE	= 0,
	SCS_ENABLE	= 1,
	SCS_DISABLE = 2,
};

void SetDefaultEnchantInfo(CItemDef const *pkItemDef,PgBase_Item & kItem)
{
	if(NULL == pkItemDef)
	{
		return;
	}

	SEnchantInfo kEnchantInfo = kItem.EnchantInfo();

	/*================================================================================================
								아이템 기본 레어도 설정(고정값)
	================================================================================================*/

	int const iRarity = pkItemDef->GetAbil(AT_DEFAULT_ITEM_RARITY);
	if(iRarity)
	{
		kEnchantInfo.Rarity(iRarity);
	}
	
	/*================================================================================================
								아이템 기본 인첸트 정보(타입,등급)
	================================================================================================*/

	int const iPlusType = pkItemDef->GetAbil(AT_DEFAULT_ITEM_ENCHANT_TYPE);
	int const iPlusLv = pkItemDef->GetAbil(AT_DEFAULT_ITEM_ENCHANT_LEVEL);
	if(iPlusType && iPlusLv)
	{
		kEnchantInfo.PlusType(iPlusType);
		kEnchantInfo.PlusLv(iPlusLv);
	}

	/*================================================================================================
								아이템 기본 소울크래프트 옵션 1~4 (타입,등급)
	================================================================================================*/
	{
		int const iOptType = pkItemDef->GetAbil(AT_DEFAULT_ITEM_OPTION_TYPE1);
		int const iOptLv = pkItemDef->GetAbil(AT_DEFAULT_ITEM_OPTION_LEVEL1);
		if(iOptType && iOptLv)
		{
			kEnchantInfo.BasicType1(iOptType);
			kEnchantInfo.BasicLv1(iOptLv);
		}
	}
	{
		int const iOptType = pkItemDef->GetAbil(AT_DEFAULT_ITEM_OPTION_TYPE2);
		int const iOptLv = pkItemDef->GetAbil(AT_DEFAULT_ITEM_OPTION_LEVEL2);
		if(iOptType && iOptLv)
		{
			kEnchantInfo.BasicType1(iOptType);
			kEnchantInfo.BasicLv1(iOptLv);
		}
	}
	{
		int const iOptType = pkItemDef->GetAbil(AT_DEFAULT_ITEM_OPTION_TYPE3);
		int const iOptLv = pkItemDef->GetAbil(AT_DEFAULT_ITEM_OPTION_LEVEL3);
		if(iOptType && iOptLv)
		{
			kEnchantInfo.BasicType1(iOptType);
			kEnchantInfo.BasicLv1(iOptLv);
		}
	}
	{
		int const iOptType = pkItemDef->GetAbil(AT_DEFAULT_ITEM_OPTION_TYPE4);
		int const iOptLv = pkItemDef->GetAbil(AT_DEFAULT_ITEM_OPTION_LEVEL4);
		if(iOptType && iOptLv)
		{
			kEnchantInfo.BasicType1(iOptType);
			kEnchantInfo.BasicLv1(iOptLv);
		}
	}

	/*================================================================================================
								아이템 기본 저주 여부 (0 ~ 1)
	================================================================================================*/
	int const iCurse = pkItemDef->GetAbil(AT_DEFAULT_ITEM_CURSE);
	switch(iCurse)
	{
	case SCS_NONE:
		{
		}break;
	case SCS_ENABLE:
		{
			kEnchantInfo.IsCurse(true);
		}break;
	case SCS_DISABLE:
		{
			kEnchantInfo.IsCurse(false);
		}break;
	}

	/*================================================================================================
								아이템 기본 봉인 여부 (0 ~ 1)
	================================================================================================*/
	int const iSeal = pkItemDef->GetAbil(AT_DEFAULT_ITEM_SEAL);
	switch(iSeal)
	{
	case SCS_NONE:
		{
		}break;
	case SCS_ENABLE:
		{
			kEnchantInfo.IsSeal(true);
		}break;
	case SCS_DISABLE:
		{
			kEnchantInfo.IsSeal(false);
		}break;
	}

	kItem.EnchantInfo(kEnchantInfo);
}

HRESULT CreateSItem(int const iItemNo, int const iCount/* 수량 아이템은 갯수, 내구도 아이템은 내구도. */, int const iRarityControl/*레어도 생성 테이블*/, PgBase_Item &rItem, bool const bIsForceSeal )
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	GET_DEF(PgItemOptionMgr, kItemOptionMgr);

	CItemDef const *pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if(pkItemDef)
	{
		WORD wRetCount = 0;
		int const iDBAmount = pkItemDef->GetAbil(AT_DEFAULT_AMOUNT);
		int const iMaxAmount = (int)pkItemDef->MaxAmount();
		int iRetAmount = iDBAmount;

		if(pkItemDef->IsAmountItem())
		{
			iRetAmount = (iDBAmount%USE_AMOUNT_BASE_VALUE);
			iRetAmount = __min(iRetAmount, iMaxAmount);//아이템 최대 개수를 넘을 수 없고.
			
			iRetAmount = __min(iCount, iMaxAmount);//요청한 아이템 개수 만큼.
		}
		else
		{
			int const iInitDur = pkItemDef->GetAbil(AT_DEFAULT_DURATION);
			if (iInitDur > 0)
			{
				iRetAmount = __min(iInitDur, iDBAmount);
			}
			else
			{
				iRetAmount = iDBAmount;
			}
		}
		if( !iRetAmount 
		|| iRetAmount > USHRT_MAX
		|| 0 >= iRetAmount)
		{
			INFO_LOG(BM::LOG_LV0, __FL__<<L"CraeteSItem Ret Amount Too Big   ===>ItemNo["<<iItemNo<<L"] Amount["<<iRetAmount<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		rItem.Clear();
		rItem.Guid(BM::GUID::Create());	//16
		rItem.ItemNo(iItemNo);		//4	20
		rItem.Count(iRetAmount);		//2	22	갯수

		SYSTEMTIME kSysTime;
		g_kEventView.GetLocalTime(&kSysTime);
		BM::PgPackedTime kGenTime;
		kGenTime = kSysTime;
		rItem.CreateDate(kGenTime);//4 35	생성날짜

		if ( !pkItemDef->IsPetItem() )
		{// 펫 아이템은 레어도 생성 금지
			kItemOptionMgr.GenerateOption(rItem, iRarityControl);
		}

		SEnchantInfo kEnchantInfo = rItem.EnchantInfo();

		//옵션 제네레이팅 된상태의 등급 
		E_ITEM_GRADE const eitemGrade = GetItemGrade(rItem);

		if(	bIsForceSeal//강제 봉인이고.
		&&	!kEnchantInfo.IsSeal() //봉인 안됐는데.
		&&	eitemGrade > IG_NORMAL)//노멀 보다 높고.
		{
			kEnchantInfo.IsSeal(true);
			rItem.EnchantInfo(kEnchantInfo);
		}

		SetDefaultEnchantInfo(pkItemDef,rItem);

		switch(pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE))
		{
		case UICT_EXPCARD:
			{
				SExpCard kExpCard;
				kExpCard.ExpPer(static_cast<WORD>(pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1)));
				kExpCard.MaxExp(pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2));
				rItem.Set(kExpCard);
			}break;
		case UICT_PET:
			{
				rItem.State( UIT_STATE_PET );// 정보는 만들어 주어야 한다.

				SClassKey kClassKey( pkItemDef->GetAbil(AT_CLASS), pkItemDef->GetAbil(AT_LEVEL) );
				if ( 0 == kClassKey.iClass )
				{
					return E_FAIL;
				}

				if ( 0 == kClassKey.nLv )
				{
					kClassKey.nLv = 1;
				}

				PgItem_PetInfo *pkPetInfo = NULL;
				if ( true == rItem.GetExtInfo( pkPetInfo ) )
				{
					SEnchantInfo kEnchantInfo = rItem.EnchantInfo();

					if ( FAILED(CreatePetInfo( kClassKey, pkPetInfo, &kEnchantInfo ) ) )
					{
						rItem.State( UIT_STATE_NORMAL );//해제
						return E_FAIL;
					}

					rItem.EnchantInfo( kEnchantInfo );
				}
			}break;
		default:
			{
			}break;
		}

		if(true == pkItemDef->IsType(ITEM_TYPE_MYHOME_IN) || pkItemDef->IsType(ITEM_TYPE_MYHOME_OUT)) // 홈 장착 아이템 정보
		{
			SHomeItem kEquipPos;
			rItem.Set(kEquipPos);
		}

		int const iTimeType = pkItemDef->GetAbil(AT_TIME_TYPE);
		int const iUseTime = pkItemDef->GetAbil(AT_USE_TIME);
		if((0 < iUseTime) && (iUseTime <= MAX_CASH_ITEM_TIMELIMIT))
		{
			rItem.SetUseTime(iTimeType,iUseTime);
		}

		return S_OK;
	}

	INFO_LOG(BM::LOG_LV5, __FL__<<L"Can't CraeteSItem Cannot find ItemDef["<<iItemNo<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CalcSellMoney(PgBase_Item const &rkItem, __int64 &iOutValue, int const iInSellPrice, int const iSellCount/*수량아이템만 쓰는값*/, PgPremiumMgr const& rkPremiumMgr)//판매가
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDefItem = kItemDefMgr.GetDef(rkItem.ItemNo());

	if(!pkDefItem)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

//	int const iSellPrice = pkDefItem->SellPrice();
	if(!iInSellPrice)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	int iSellPrice = iInSellPrice;
	if( S_PST_StoreSellPremium const* pkPremiumSell = rkPremiumMgr.GetType<S_PST_StoreSellPremium>() )
	{
		iSellPrice += SRateControl::GetValueRate<int>( iSellPrice, pkPremiumSell->iPremiumRate );
	}

	if(pkDefItem->IsAmountItem())
	{
		int iRetSellCount = iSellCount;
		if(rkItem.Count() < iSellCount)
		{
			iRetSellCount = rkItem.Count();
		}
		iOutValue = iSellPrice * iRetSellCount;
	}
	else
	{
		int const iMaxDur = (int)pkDefItem->MaxAmount();
		if(iMaxDur > 0)
		{
			if(rkItem.EnchantInfo().IsNeedRepair())
			{//수리 요구는 내구도 0으로 간주
				iOutValue = 0;
			}
			else
			{
				int const iNowDur = rkItem.Count();
				iOutValue = static_cast<int>((float)iSellPrice * (float)iNowDur / (float)iMaxDur);
			}
		}
	}
	return S_OK;
}

int FinddMaxAbilLevel(int const iEquipPos,int const iType,int const iLevelLimit)
{
	GET_DEF(PgItemOptionMgr, kItemOptionMgr);

	int const iBase = iLevelLimit/5;
	int const iMinLv = std::max(1, iBase - 2);
	int const iMaxLv = std::min(MAX_DICE_ITEM_OPTION_LEVEL, iBase + 2);

	std::map<int,int> kCont;

	for(int i = iMinLv;i <= iMaxLv;++i)
	{
		kCont.insert(std::make_pair(kItemOptionMgr.GetBasicAbil(SItemExtOptionKey(iType, iEquipPos, i, 0)), i));
	}

	if(true == kCont.empty())
	{
		return 0;
	}

	return (*kCont.rbegin()).second;
}

bool IsCanRepair(int const iItemNo)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pDef = kItemDefMgr.GetDef(iItemNo);
	if(!pDef)
	{
		return false;
	}
	int	const iCantAbil = pDef->GetAbil(AT_ATTRIBUTE);
	if(iCantAbil)
	{
		if((iCantAbil & ICMET_Cant_Repair) == ICMET_Cant_Repair)
		{
			return false;
		}
	}
	return true;
}

void PgBase_Item::WriteToPacket(BM::Stream &kPacket)const
{
	kPacket.Push(Guid());	//16
	kPacket.Push(ItemNo());		//4	20
	kPacket.Push(Count());		//2	22	갯수
	kPacket.Push(State());		//1	31상태
	kPacket.Push(CreateDate());//4 35	생성날짜
	kPacket.Push(StatTrackInfo());
	tagItemWriteFlag kFlag(*this);
	kFlag.WriteToPacket(*this, kPacket);

//	kPacket.Push(EnchantInfo());	//8 30	인첸트 정보
//	PgItemExtendDataMgr::WriteToPacket(kPacket);

//	if ( m_pkExtInfo )
//	{
//		m_pkExtInfo->WriteToPacket( kPacket );
//	}
}

bool PgBase_Item::ReadFromPacket(BM::Stream &kPacket)
{
	Clear(); // 아이템 정보 초기화

	BYTE byState = UIT_STATE_NORMAL;
	if(	true == kPacket.Pop(m_kGuid) 
	&&	true == kPacket.Pop(m_kItemNo) 
	&&	true == kPacket.Pop(m_kCount) 
	&&	true == kPacket.Pop(byState) 
	&&	true == kPacket.Pop(m_kCreateDate)
	&&	true == kPacket.Pop(m_kStatTrackInfo))
	{
		State( byState );
		tagItemWriteFlag kFlag;
		return kFlag.ReadFromPacket(*this, kPacket);
//			&&	true == kPacket.Pop(m_kEnchantInfo) 
//			&&	true == PgItemExtendDataMgr::ReadFromPacket(kPacket))
//			{
//				if ( true == State( byState ) )
//				{
//					if ( m_pkExtInfo )
//					{
//						return m_pkExtInfo->ReadFromPacket( kPacket );
//					}
//				}
//				return true;
//			}
	}
	return false;
}

int PgBase_Item::MaxDuration() const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkDefItem = kItemDefMgr.GetDef(ItemNo());
	if(!pkDefItem)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return 0;
	}
	return pkDefItem->MaxAmount() - EnchantInfo().DecDuration();
}

tagItemWriteFlag::tagItemWriteFlag(PgBase_Item const &kItem)
{
	HasEnchant(!kItem.EnchantInfo().IsEmpty());
	HasExtend(0 < kItem.Cont().size());
	HasName(0);
	HasMemo(0);
	HasPetUnit( ((NULL == kItem.m_pkExtInfo) ? 0 : 1) );
	Pad(0);
}

void tagItemWriteFlag::WriteToPacket(PgBase_Item const &kItem, BM::Stream &kPacket)const
{
	size_t const iWrPos = kPacket.WrPos();
	kPacket.Push(*this);

	if(HasEnchant())
	{
		kPacket.Push(kItem.m_kEnchantInfo);
	}

	if (HasPetUnit())
	{
		if (	kItem.m_pkExtInfo
			&&	kItem.m_pkExtInfo->GetType() == kItem.State())
		{
			kItem.m_pkExtInfo->WriteToPacket( kPacket );
		}
		else
		{
			if ( kItem.m_pkExtInfo )
			{
				VERIFY_INFO_LOG( false ,BM::LOG_LV1, __FL__ << L"Critical Error!!! pkExtInfo Type<" << kItem.m_pkExtInfo->GetType() << L"> different State<" << kItem.State() << L">" );
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"Critical Error!!! pkExtInfo is NULL" );
			}
			
			tagItemWriteFlag kTemp( *this );
			kTemp.HasPetUnit(0);
			kPacket.ModifyData( iWrPos, &kTemp, sizeof(tagItemWriteFlag) );
		}
	}

	if(HasExtend())
	{
		kItem.PgItemExtendDataMgr::WriteToPacket(kPacket);
	}
}

bool tagItemWriteFlag::ReadFromPacket(PgBase_Item &kItem, BM::Stream &kPacket)
{
	kPacket.Pop(*this);

	if( HasEnchant() )
	{
		kPacket.Pop(kItem.m_kEnchantInfo);
	}

	if ( HasPetUnit() )
	{
		if ( kItem.m_pkExtInfo )
		{
			kItem.m_pkExtInfo->ReadFromPacket( kPacket );
		}
		else
		{
			return false;
		}
	}

	if(HasExtend())
	{
		kItem.PgItemExtendDataMgr::ReadFromPacket(kPacket);
	}
	return true;
}

EEquipType GetEquipType(int const iItemNo)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkItemDef = kItemDefMgr.GetDef(iItemNo);
	return GetEquipType(pkItemDef);
}

EEquipType GetEquipType(CItemDef const * const pkItemDef)
{
	if( pkItemDef )
	{
		int const iEquipPos = pkItemDef->GetAbil(AT_EQUIP_LIMIT);
		if( EQUIP_WEAPON_TYPE & iEquipPos )
		{
			return EEQUIP_WEAPON;
		}
		else if( EQUIP_ARMOR_TYPE & iEquipPos )
		{
			return EEQUIP_ARMOR;
		}
		else if( EQUIP_ACC_TYPE & iEquipPos )
		{
			return EEQUIP_ACC;
		}
	}

	return EEQUIP_NONE;
}

int GetAbilValue(CUnit const* pkUnit, int const Type)
{
	int iResultValue = 0;

	if( !pkUnit )
	{
		return iResultValue;
	}	
	if( pkUnit->IsUnitType(UT_PET) )
	{
		return pkUnit->GetAbil(Type);
	}

	switch(Type)
	{// 유저 정보중
	/*case AT_C_HIT_SUCCESS_VALUE:
	case AT_C_DODGE_SUCCESS_VALUE:
		{
			iResultValue += static_cast<int>(pkUnit->GetAbil(Type) * 0.01f);
		}break;*///의도를 모르겠다.
	case AT_C_PHY_ATTACK_MAX:
		{// 물리 공격력 정보를 보여줄때, %만큼 감소, 증가가 적용되어 있다면
			int const iC_Phy_Attack_Max = pkUnit->GetAbil(Type);
			int iTotalPer = pkUnit->GetAbil(AT_PHY_DMG_PER);
			if(0 == iTotalPer)
			{
				iTotalPer = ABILITY_RATE_VALUE;
			}
			iTotalPer +=  pkUnit->GetAbil(AT_PHY_DMG_PER2);
			float fC_Phy_Dmg_Per = iTotalPer/ABILITY_RATE_VALUE_FLOAT;
			if(0 >= fC_Phy_Dmg_Per)
			{
				fC_Phy_Dmg_Per = 1.0f;
			}
			iResultValue += static_cast<int>(iC_Phy_Attack_Max*fC_Phy_Dmg_Per);
		}break;
	case AT_C_MAGIC_ATTACK_MAX:
		{// 마법 공격력 정보를 보여줄때, %만큼 감소, 증가가 적용되어 있다면
			int const iC_Mag_Attack_Max = pkUnit->GetAbil(Type);
			int iTotalPer = pkUnit->GetAbil(AT_MAGIC_DMG_PER);
			if(0 == iTotalPer)
			{
				iTotalPer = ABILITY_RATE_VALUE;
			}
			iTotalPer += pkUnit->GetAbil(AT_MAGIC_DMG_PER2);
			float fC_Mag_Dmg_Per =  iTotalPer/ABILITY_RATE_VALUE_FLOAT;
			if(0 >= fC_Mag_Dmg_Per)
			{
				fC_Mag_Dmg_Per = 1.0f;
			}
			iResultValue += static_cast<int>(iC_Mag_Attack_Max*fC_Mag_Dmg_Per);
		}break;
	default:
		{
			iResultValue += pkUnit->GetAbil(Type);
		}break;
	}
	return iResultValue;
}

int GetElementAbilValue(CUnit const* pkUnit, eElementDmgType const Type, 
						EGetAbilType::Enum const getabiltype)
{
	if( !pkUnit )
	{
		return 0;
	}

	int iValue = 0;

	switch( Type )
	{
	case ET_D_DESTROY:
		{			
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_DESTROY_PHY, GetAbilValue(pkUnit, AT_C_PHY_ATTACK_MAX), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_DESTROY_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_ATTACK_MAX), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_ATTACK_ADD_DESTROY, iValue, getabiltype);
		}break;
	case ET_D_FIRE:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_FIRE_PHY, GetAbilValue(pkUnit, AT_C_PHY_ATTACK_MAX), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_FIRE_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_ATTACK_MAX), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_ATTACK_ADD_FIRE, iValue, getabiltype);
		}break;
	case ET_D_ICE:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_ICE_PHY, GetAbilValue(pkUnit, AT_C_PHY_ATTACK_MAX), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_ICE_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_ATTACK_MAX), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_ATTACK_ADD_ICE, iValue, getabiltype);
		}break;
	case ET_D_NATURE:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_NATURE_PHY, GetAbilValue(pkUnit, AT_C_PHY_ATTACK_MAX), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_NATURE_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_ATTACK_MAX), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_ATTACK_ADD_NATURE, iValue, getabiltype);
		}break;
	case ET_D_CURSE:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_CURSE_PHY, GetAbilValue(pkUnit, AT_C_PHY_ATTACK_MAX), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_ATTACK_ADD_CURSE_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_ATTACK_MAX), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_ATTACK_ADD_CURSE, iValue, getabiltype);
		}break;
	case ET_R_DESTROY:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_DESTROY_PHY, GetAbilValue(pkUnit, AT_C_PHY_DEFENCE), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_DESTROY_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_DEFENCE), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_RESIST_ADD_DESTROY, iValue, getabiltype);
		}break;
	case ET_R_FIRE:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_FIRE_PHY, GetAbilValue(pkUnit, AT_C_PHY_DEFENCE), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_FIRE_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_DEFENCE), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_RESIST_ADD_FIRE, iValue, getabiltype);
		}break;
	case ET_R_ICE:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_ICE_PHY, GetAbilValue(pkUnit, AT_C_PHY_DEFENCE), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_ICE_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_DEFENCE), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_RESIST_ADD_ICE, iValue, getabiltype);
		}break;
	case ET_R_NATURE:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_NATURE_PHY, GetAbilValue(pkUnit, AT_C_PHY_DEFENCE), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_NATURE_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_DEFENCE), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_RESIST_ADD_NATURE, iValue, getabiltype);
		}break;
	case ET_R_CURSE:
		{
			iValue = pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_CURSE_PHY, GetAbilValue(pkUnit, AT_C_PHY_DEFENCE), getabiltype);
			iValue += pkUnit->GetConvertElementalDmgRate(AT_R_RESIST_ADD_CURSE_MAGIC, GetAbilValue(pkUnit, AT_C_MAGIC_DEFENCE), getabiltype);
			iValue = pkUnit->GetBasicElementDmg(AT_RESIST_ADD_CURSE, iValue, getabiltype);
		}break;
	default:
		{
		}break;
	}
	return iValue;
}

bool OrderCreateItem(CONT_ITEM_CREATE_ORDER & rkOrder, int const iItemNo, int const iItemCount, int const iRarityControl)
{
	if(0 >= iItemNo || 0 >= iItemCount)
	{
		return false;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	if(CItemDef const* pkItemDef = kItemDefMgr.GetDef(iItemNo))
	{
		if( false == pkItemDef->IsAmountItem() ) // 겹쳐지는 아이템
		{
			for(int i=0; i<iItemCount; ++i)
			{
				PgBase_Item kItem;
				if(S_OK != CreateSItem(iItemNo, 1, iRarityControl, kItem))
				{
					return false;
				}

				rkOrder.push_back(kItem);
			}
		}
		else
		{
			PgBase_Item kItem;
			if(S_OK != CreateSItem(iItemNo, iItemCount, iRarityControl, kItem))
			{
				return false;
			}

			rkOrder.push_back(kItem);

			int const iRemainCount = iItemCount - kItem.Count();
			if(iRemainCount > 0)
			{
				return OrderCreateItem(rkOrder, iItemNo, iRemainCount, iRarityControl);
			}
		}
	}
	return (false == rkOrder.empty());
}
