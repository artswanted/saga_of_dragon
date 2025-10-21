//// Skill Class
// Dukguru
//

#include "stdafx.h"
#include "constant.h"
#include "Effect.h"
#include "TableDataManager.h"
#include "DefAbilType.h"
#include "PgControlDefMgr.h"

CEffect::CEffect():
 m_iAddDurationTime(0)
{
	SetEndTime(0);
}

CEffect::~CEffect()
{

}

CEffect::CEffect( CEffect const& rhs )
{
	*this = rhs;
}

CEffect& CEffect::operator=( CEffect const& rhs )
{
	m_byType = rhs.m_byType;
	m_iEffectNo = rhs.m_iEffectNo;
	m_iValue = rhs.m_iValue;
	m_ulTime = rhs.m_ulTime;
	m_ulInterval = rhs.m_ulInterval;
	m_kCasterGuid = rhs.m_kCasterGuid;
	m_iInstanceID = rhs.m_iInstanceID;
	m_EffectVisualState = rhs.m_EffectVisualState;
	m_ulStartTime = rhs.m_ulStartTime;
	m_pEffectDef = NULL;
	m_kActArg.m_contValue = rhs.m_kActArg.m_contValue;
	m_kTargetList = rhs.m_kTargetList;
	m_kCasterExList = rhs.m_kCasterExList;
	m_ulEndTime = rhs.m_ulEndTime;
	m_kSubEffectNo = rhs.m_kSubEffectNo;
	ExpireTime(rhs.ExpireTime());
	m_iDeleteCause = rhs.m_iDeleteCause;
	m_bDelete = rhs.m_bDelete;
	m_iAddDurationTime = rhs.m_iAddDurationTime;
	return *this;
}

int const CEffect::GetEffectNo() const
{
	return m_iEffectNo;
}


int const CEffect::GetValue() const
{
	return m_iValue;
}

void CEffect::SetValue(int const iValue)
{
	m_iValue = iValue;
}

int CEffect::AddValue(int const iValue /* negativable */)
{
	m_iValue += iValue;
	return m_iValue;
}

unsigned long CEffect::GetTime() const
{
	return m_ulTime;
}

void CEffect::SetInterval(unsigned long ulInterval)
{
	m_ulInterval = ulInterval;
}

unsigned long CEffect::GetInterval() const
{
	return m_ulInterval;
}


CEffectDef const* CEffect::GetEffectDef() const
{
	if ( NULL == m_pEffectDef || m_pEffectDef->EffectID() != GetEffectNo() )
	{
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		m_pEffectDef = (CEffectDef*) kEffectDefMgr.GetDef(GetEffectNo());
	}

	if(NULL == m_pEffectDef)
	{
		return 0;
	}

	return m_pEffectDef;
}

int CEffect::GetAbil(WORD const Type) const
{
	switch(Type)
	{
	case AT_FRAN:
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
		{
			//VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s] __int64 처리해야 함."), __FUNCTIONW__);
			INFO_LOG(BM::LOG_LV0, __FL__<<L"call GetAbil64(..) . Type["<<Type<<L"]");
		}break;
	}
	CEffectDef const* pkDef = GetEffectDef();
	if (NULL == pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	return pkDef->GetAbil(Type);
}

void CEffect::Init()
{
	m_byType = EFFECT_TYPE_NONE;
	m_iEffectNo = 0 ;
	m_iValue = 0 ;
	m_ulTime = 0 ;
	m_ulInterval = 0 ;
	m_kCasterGuid = BM::GUID::NullData();
	m_kActArg.m_contValue.clear();
	m_iInstanceID = 0;
	m_EffectVisualState = EVS_VISIBLE;
	m_ulStartTime = 0;
	m_iDeleteCause = 0;
	m_bDelete = false;

	m_pEffectDef = NULL ;
	m_kExpireTime.Clear();

	// Target 리스트 및 Caster 리스트는 초기화를 하지 않는다.
	// 생성자에서만 되도록 한다.
	// 이펙트가 리셋되어도 해당 정보는 유지되어야 하기 때문이다.
	// 밑에 주석 제거 금지!!!
	// m_kTargetList.clear();
	// m_kCasterExList.clear();
}

int CEffect::CallAction(WORD wEffectActCode, SActArg* pEffectActArg)
{
	return 0;
}


ECT_CheckTick CEffect::CheckTick( unsigned long const ulElapsed, bool const bDamaged )
{
	m_ulTime += ulElapsed;

	if ( true == bDamaged )
	{
		if ( 0 < this->GetAbil(AT_DAMAGE_RESETTICK_EFFECT) )
		{
			m_ulInterval = 0;
			return ECT_RESET;
		}
	}

	ECT_CheckTick kCheck = ECT_NONE;
	m_ulInterval += ulElapsed;
	
	CEffectDef const* pkEffectDef = GetEffectDef();
	int const iInterval = (NULL == pkEffectDef) ? 0 : pkEffectDef->GetInterval();
	if (0 != iInterval && (int)m_ulInterval > iInterval)
	{
		m_ulInterval -= iInterval;
		kCheck = ECT_CheckTick(kCheck | ECT_DOTICK);
	}

	if (true == ExpireTime().IsNull())
	{
		int const iDurationTime = GetDurationTime();
		if ((0 != iDurationTime) && (m_ulTime > iDurationTime))
		{
			kCheck = ECT_CheckTick(kCheck | ECT_MUSTDELETE);
		}
	}
	else
	{
		BM::DBTIMESTAMP_EX kLocalTime;
		g_kEventView.GetLocalTime( kLocalTime );

		if (kLocalTime > ExpireTime())
		{
			kCheck = ECT_CheckTick(kCheck | ECT_MUSTDELETE);
		}
	}
	return kCheck;
}

void CEffect::Begin( SEffectCreateInfo const& rkCreate, bool const bRestart )
//void CEffect::Begin(BYTE const byType, int const iEffectNo, int const iValue, SActArg const* pkAct, unsigned long const ulElapsedTime, unsigned long const ulInterval)
{
	int const iEffect = ( bRestart ? m_iEffectNo : rkCreate.iEffectNum );
	BYTE const byType = ( bRestart ? m_byType : rkCreate.eType );

	BM::DBTIMESTAMP_EX const kTempExpireTime = ExpireTime();
	CEffect::Init();

	if( byType == (BYTE)EFFECT_TYPE_NONE
	||	byType == (BYTE)EFFECT_TYPE_NORMAL )
	{
		if ( true == bRestart )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Effect Type is invalid EffectID["<<iEffect<<L"],Type["<<m_byType<<L"]");
		}
		else
		{
			//VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] Effect Type is invalid EffectID[%d],Type[%d]"), __FUNCTIONW__, iEffectNo, (int)m_byType);
			INFO_LOG(BM::LOG_LV4, __FL__<<L"Bad Parameter SActArg is NULL Type["<< static_cast<int>(rkCreate.eType)<<L"], EffectNo["<< rkCreate.iEffectNum <<L"]");
			return;
		}
	}

	m_iEffectNo = iEffect;
	m_byType = byType;
	m_iValue = rkCreate.iValue;
	m_kActArg = rkCreate.kActArg;
	m_ulTime = rkCreate.dwElapsedTime;
	SetStartTime(0 == rkCreate.dwStartTime ? g_kEventView.GetServerElapsedTime() : rkCreate.dwStartTime);
	if(0 == rkCreate.dwEndTime)
	{
		SetEndTime(GetStartTime()+GetDurationTime() - m_ulTime);
	}
	else
	{
		SetEndTime(rkCreate.dwEndTime);
	}

	if ( !bRestart )
	{
		ExpireTime(kTempExpireTime);	//임시코드
		m_ulInterval = rkCreate.dwInterval;
		m_kActArg.Remove(ACTARG_ITEMNO);
	}
	
	if (true == ExpireTime().IsNull())	// CItemEffect 에서 이미 ExpireTime() 값이 설정 되었을 수 있다.
	{
		E_TIME_ELAPSED_TYPE const eTimeType = static_cast<E_TIME_ELAPSED_TYPE>(GetAbil(AT_DURATION_TIME_TYPE));
		if (eTimeType == E_TIME_ELAPSED_TYPE_WORLDTIME)
		{
			if (true == rkCreate.kWorldExpireTime.IsNull())
			{
				int const iDuration = GetDurationTime();
				SYSTEMTIME kLocalTime;
				g_kEventView.GetLocalTime(&kLocalTime);
				int iRemainTime = iDuration - static_cast<int>(rkCreate.dwElapsedTime);
				if (iRemainTime <= 0)
				{
					INFO_LOG(BM::LOG_LV4, __FL__ << _T("Effect ExpireTime error DurationTime=") << iDuration << _T(", ElapsedTime=") << rkCreate.dwElapsedTime);
				}
				CGameTime::AddTime(kLocalTime, iRemainTime*CGameTime::MILLISECOND);
				ExpireTime(BM::DBTIMESTAMP_EX(kLocalTime));
			}
		}
	}
	if (false == rkCreate.kWorldExpireTime.IsNull())
	{
		ExpireTime(rkCreate.kWorldExpireTime);
	}

	// Caster Count는 Begin될 때 한번만 초기화 시킨다.
}

void CEffect::SetActArg(int const iType, void const* pValue)
{
	m_kActArg.Set(iType, pValue);
}

void CEffect::SetActArg(int const iType, int const iValue)
{
	m_kActArg.Set(iType, iValue);
}

// 주의사항 :::: 반드시 kOutValue 값을 초기화 하고 호출 할 것
// 초기화 하지 않으면, Garbage 값 나올수 있음.
HRESULT CEffect::GetActArg(int const iType, int &kOutValue) const
{
	return m_kActArg.Get(iType, kOutValue);
}

int CEffect::Level() const
{
	CEffectDef const* pkDef = GetEffectDef();

	if (pkDef != NULL)
	{
		int const iLevel = pkDef->GetAbil(AT_LEVEL);
		if (iLevel > 0)
		{
			return iLevel;
		}
	}
	return m_iEffectNo - GET_BASICSKILL_NUM(m_iEffectNo) + 1;
}

// static function
int CEffect::GetLevel(int const iEffectNo)
{
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	int iLevel = kEffectDefMgr.GetAbil(iEffectNo, AT_LEVEL);
	if (iLevel == 0)
	{
		iLevel = iEffectNo - GET_BASICSKILL_NUM(iEffectNo) + 1;
	}
	return iLevel;
}

int CEffect::GetKey() const
{
	return GetEffectNo();
}

int CEffect::GetDurationTime() const
{
	CEffectDef const *pkDef = GetEffectDef();
	if ( pkDef )
	{
		return pkDef->GetDurationTime() + GetAddDurationTime();
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error Not Found EffectDef, EffectNo = " << m_iEffectNo );
	return 0;
}

int CEffect::GetAddDurationTime() const
{
	return m_iAddDurationTime;
}
void CEffect::SetAddDurationTime(int const iAddTime)
{
	m_iAddDurationTime = iAddTime;
}

bool const CEffect::IsSame(int const iEffectKey, bool const bInGroup) const
{
	int const iKey = GetKey();
	if (iEffectKey == iKey)
	{
		return true;
	}
	if (bInGroup && GroupNo() == GroupNo(iEffectKey))
	{
		return true;
	}
	return false;
}

bool CEffect::AddTarget(BM::GUID const& rkGuid)
{
	VEC_GUID::iterator itor = find(m_kTargetList.begin(), m_kTargetList.end(), rkGuid);
	if(itor == m_kTargetList.end())
	{
		m_kTargetList.push_back(rkGuid);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
bool CEffect::DeleteTarget(BM::GUID const& rkGuid)
{
	VEC_GUID::iterator itor = find(m_kTargetList.begin(), m_kTargetList.end(), rkGuid);
	if(itor != m_kTargetList.end())
	{
		m_kTargetList.erase(itor);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
bool CEffect::IsTarget(BM::GUID const& rkGuid)
{
	VEC_GUID::iterator itor = find(m_kTargetList.begin(), m_kTargetList.end(), rkGuid);
	if(itor != m_kTargetList.end())
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
BM::GUID CEffect::GetTarget(int const nIndex) const
{
	if((int)m_kTargetList.size() > nIndex)
	{
		return m_kTargetList[nIndex];
	}

	BM::GUID kGuid;
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return Guid"));
	return kGuid;
}

void CEffect::AddCasterEx(BM::GUID const& rkGuid)
{
	VEC_GUID::iterator itor = find(m_kCasterExList.begin(), m_kCasterExList.end(), rkGuid);
	if(m_kCasterExList.end() == itor)
	{
		m_kCasterExList.push_back(rkGuid);
	}
}
bool CEffect::IsCasterEx(BM::GUID const& rkGuid)
{
	VEC_GUID::iterator itor = find(m_kCasterExList.begin(), m_kCasterExList.end(), rkGuid);
	if(m_kCasterExList.end() == itor)
	{
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void CEffect::DeleteCasterEx(BM::GUID const& rkGuid)
{
	VEC_GUID::iterator itor = find(m_kCasterExList.begin(), m_kCasterExList.end(), rkGuid);
	if(m_kCasterExList.end() != itor)
	{
		m_kCasterExList.erase(itor);
	}
}

void CEffect::DeleteCasterExFirst()
{
	if(!m_kCasterExList.empty())
	{
		m_kCasterExList.erase(m_kCasterExList.begin());
	}	
}

bool CEffect::IsSendingTypeDuringMapMove() const
{
	switch ( m_byType )
	{
	case EFFECT_TYPE_PASSIVE:
	case EFFECT_TYPE_GROUND:
	case EFFECT_TYPE_ABIL:// Abil에 의해 생성되는 이펙트
//	case EFFECT_TYPE_ACTIVATE_SKILL:// pet용 false
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	default:
		{
		}break;
	}
	return true;
}

int CEffect::GetSubEffectNoByIndex(int const iIndex) const
{
	if(iIndex >= static_cast<int>(m_kSubEffectNo.size()))
	{
		return 0;
	}

	return m_kSubEffectNo[iIndex];
}

int CEffect::GetSubEffectNo(int const iSubEffectNo, bool const bInGroup) const
{
	for(CONT_INT::const_iterator itor = m_kSubEffectNo.begin(); itor != m_kSubEffectNo.end(); ++itor)
	{
		if(iSubEffectNo == (*itor))
		{
			return (*itor);
		}

		if(bInGroup && GET_BASICSKILL_NUM(iSubEffectNo) == GET_BASICSKILL_NUM((*itor)))
		{
			return (*itor);
		}		
	}

	return 0;
}

void CEffect::AddSubEffectNo(int const iSubEffectNo)
{
	if(0 < iSubEffectNo)
	{
		m_kSubEffectNo.push_back(iSubEffectNo);
	}
}

void CEffect::DeleteSubEffectNo(int const iSubEffectNo)
{
	CONT_INT::iterator itor = std::find(m_kSubEffectNo.begin(), m_kSubEffectNo.end(), iSubEffectNo);
	if(itor != m_kSubEffectNo.end())
	{
		m_kSubEffectNo.erase(itor);
	}	
}

void CEffect::swap_SubEffectCont(std::vector<int>& kSubEffectCont)
{
	m_kSubEffectNo.swap(kSubEffectCont);
}

CEffect::ESAVE_DB_TYPE CEffect::GetDBSaveType() const
{
	if (GetAbil(AT_SAVE_DB) <= 0)
	{
		return ESAVE_DB_TYPE_NOSAVE;
	}
	return (ExpireTime().IsNull()) ? ESAVE_DB_TYPE_PLAYTIME_ELAPSED_5S : ESAVE_DB_TYPE_WORLDTIME_EXPIRE;
}

int CEffect::GroupNo() const
{
	int iGroup = GetAbil(AT_EFFECT_GROUP_NO);
	if (iGroup > 0)
	{
		return iGroup;
	}
	return GET_BASICSKILL_NUM(m_iEffectNo);
}

int CEffect::GroupNo(int const iEffectNo)
{
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	int iGroup = kEffectDefMgr.GetAbil(iEffectNo, AT_EFFECT_GROUP_NO);
	if (iGroup > 0)
	{
		return iGroup;
	}
	return GET_BASICSKILL_NUM(iEffectNo);
}

// ---------------------------------
// CItemEffect
//-----------------------------------
CItemEffect::CItemEffect()
{
}

CItemEffect::~CItemEffect()
{
}

CItemEffect::CItemEffect( CItemEffect const& rhs )
{
	*this = rhs;
}

CItemEffect& CItemEffect::operator=( CItemEffect const& rhs )
{
	CEffect::operator=( rhs );
	m_pItemDef = NULL;
	ItemNo(rhs.ItemNo());
	ParentItemNo(rhs.ParentItemNo());
	return *this;
}

void CItemEffect::Init()
{
	m_pItemDef = NULL;
	ItemNo(0);
	ParentItemNo(0);

	CEffect::Init();
}

int CItemEffect::Level() const
{
	return GetAbil(m_kItemNo, AT_LEVEL);
}

CItemDef const* CItemEffect::GetItemDef() const
{
	if (NULL != m_pItemDef)
	{
		return m_pItemDef;
	}
	GET_DEF(CItemDefMgr, kItemDefMgr);
	m_pItemDef = (CItemDef*) kItemDefMgr.GetDef(ItemNo());

	if(NULL == m_pItemDef)
	{
		return 0;
	}

	return m_pItemDef;
}

// static function
int CItemEffect::GetLevel(int const iItemNo)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const* pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if (NULL == pkItemDef)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot get ItemDef ItemNo["<<iItemNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	return pkItemDef->GetAbil(AT_LEVEL);
}

int CItemEffect::GetKey() const
{
	return ItemNo();
}

void CItemEffect::Begin(SEffectCreateInfo const& rkCreate, bool const bRestart )
{
	CItemEffect::Init();
	if (S_OK != rkCreate.kActArg.Get(ACTARG_ITEMNO, m_kItemNo))
	{
		m_kItemNo = 0;
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot get ACTARG_ITEMNO"));
	}
	ParentItemNo(GetAbil(m_kItemNo, AT_PARENT_ITEM_NO));

	int const iEndTime = GetAbil(m_kItemNo, AT_ENABLE_USE_END_TIME);
	if (iEndTime > 0 )
	{
		if ( true == rkCreate.kWorldExpireTime.IsNull() )
		{
			// ExpireTime 설정 하기
			int const iEndTime24 = iEndTime % 24;	// 24hour
			BM::PgPackedTime kLocalTime;
			kLocalTime.SetLocalTime();
			if (iEndTime24 < static_cast<int>(kLocalTime.Hour()))
			{
				kLocalTime.Hour(iEndTime24);
				kLocalTime.Min(0);
				kLocalTime.Sec(0);
				// 다음날 iEndTime+1 hour 에 Expire
				CGameTime::AddTime(kLocalTime, CGameTime::OneDay+CGameTime::HOUR);
			}
			else
			{
				kLocalTime.Hour(iEndTime24);
				kLocalTime.Min(0);
				kLocalTime.Sec(0);
				// 오늘 iEndTime+1 hour에 Expire
				CGameTime::AddTime(kLocalTime, CGameTime::HOUR);
			}
			ExpireTime(BM::DBTIMESTAMP_EX(kLocalTime));
		}
	}
	CEffect::Begin( rkCreate, false );
}

// static function
int const CItemEffect::GetAbil(int const iItemNo, WORD const wType)
{
	switch(wType)
	{
	case AT_TYPE:
		{
			return EFFECT_TYPE_ITEM;
		}
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const* pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if (NULL == pkItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	return pkItemDef->GetAbil(wType);
}

int CItemEffect::GetAbil(WORD const Type) const
{
	switch(Type)
	{
	case AT_TYPE:
		{
			return GetType();
		}break;
	}

	return CEffect::GetAbil(Type);
}

bool const CItemEffect::IsSame(int const iEffectKey, bool const bInGroup) const
{
	if (iEffectKey == GetKey())
	{
		return true;
	}
	if (bInGroup)
	{
		int const iParentItem = CItemEffect::GetAbil(iEffectKey, AT_PARENT_ITEM_NO);
		if (0 == iParentItem)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if (iParentItem == GetAbil(m_kItemNo, AT_PARENT_ITEM_NO))
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int CItemEffect::GroupNo() const
{
	return GetAbil(AT_PARENT_ITEM_NO);
}

// ---------------------------------
// CEffectDef
//-----------------------------------
int CEffectDef::GetAbil(WORD const wAbilType) const
{
	int iValue = 0;
	switch(wAbilType)
	{
	case AT_C_PHY_ATTACK:	{ iValue = CAbilObject::GetAbil(AT_PHY_ATTACK_MIN); }break;
	case AT_C_MAGIC_ATTACK:	{ iValue = CAbilObject::GetAbil(AT_MAGIC_ATTACK); }break;
	case AT_TYPE:			{ iValue = m_sType; }break;
	case AT_NAMENO:			{ iValue = m_iName; }break;
	case AT_FRAN:
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"int64는 이 곳에서 사용하면 안됨");
		}break;
	default:
		{
			iValue = CAbilObject::GetAbil(wAbilType);
		}break;
	}
	return iValue;
}

bool CEffectDef::IsUseMapNo(int const iMapNo)const
{
	if(iMapNo && false == m_kContUseMapNo.empty())
	{
		return m_kContUseMapNo.end() != m_kContUseMapNo.find(iMapNo);
	}
	return true;
}

void CEffectDef::SetContUseMapNo(int const iValue)
{
	m_kContUseMapNo.insert(iValue);
}

///////////////////////////////////////////////////////////////////
//	CEffectDefMgr
///////////////////////////////////////////////////////////////////

CEffectDefMgr::CEffectDefMgr()
{
}


CEffectDefMgr::~CEffectDefMgr()
{
	T_MY_BASE_TYPE::Clear();
}


void CEffectDefMgr::swap(CEffectDefMgr& rkRight)
{
	m_contDef.swap(rkRight.m_contDef);
	m_DefPool.swap(rkRight.m_DefPool);
}

bool CEffectDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	bool bReturn = true;

	Clear();
	
	CONT_BASE::const_iterator base_itor = tblBase.begin();

	while( tblBase.end() != base_itor )
	{
		DEF *pDef = NewDef();

		pDef->EffectID((*base_itor).second.iEffectID);
		pDef->m_iName = (*base_itor).second.iName;
		pDef->m_strActionName= (*base_itor).second.strActionName;
		pDef->m_sType = (*base_itor).second.sType;
		pDef->m_sInterval = (*base_itor).second.sInterval;
		pDef->m_iDurationTime = (*base_itor).second.iDurationTime;
		pDef->m_byToggle = (*base_itor).second.byToggle;

		for( int iIndex = 0; MAX_EFFECT_ABIL_LIST > iIndex ; ++iIndex )
		{//기본 리스트 번호 찾고 
			int const iAbilNo = (*base_itor).second.aiAbil[iIndex];
			if( iAbilNo )
			{
				CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );

				if( abil_itor != tblAbil.end() )
				{
					for( int j = 0; MAX_EFFECT_ABIL_ARRAY > j ; ++j)
					{
						if(0 != (*abil_itor).second.iType[j])
						{
							switch((*abil_itor).second.iType[j])
							{
							case AT_FRAN:
							case AT_EXPERIENCE:
							case AT_MONEY:
							case AT_DIE_EXP:
							case AT_CLASSLIMIT:
								{
									VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Error!!");
									bReturn = false;
									pDef->SetAbil64((*abil_itor).second.iType[j], (*abil_itor).second.iValue[j]);
								}break;
							default:
								{
									if(AT_EFFECT_USE_MAPNO_01 <= (*abil_itor).second.iType[j] && (*abil_itor).second.iType[j] <= AT_EFFECT_USE_MAPNO_10)
									{
										if( (*abil_itor).second.iValue[j] )
										{
											pDef->SetContUseMapNo((*abil_itor).second.iValue[j]);
										}
									}
									else
									{
										pDef->SetAbil((*abil_itor).second.iType[j], (*abil_itor).second.iValue[j]);
									}
								}break;
							}
						}
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find Abil List Effect["<<pDef->EffectID()<<L"], Abil["<<iAbilNo<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
					bReturn = false;
				}
			}
		}

		auto kRet = m_contDef.insert( std::make_pair(pDef->EffectID(), pDef) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicater EffectID["<<pDef->EffectID()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			bReturn = false;
		}
		++base_itor;
	}
	return bReturn;
}

int CEffectDefMgr::GetAbil(int const iDefNo, WORD const wType) const
{
	CEffectDef const* pkDef = GetDef(iDefNo);
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	switch(wType)
	{
		case AT_FRAN:
		case AT_EXPERIENCE:
		case AT_MONEY:
		case AT_DIE_EXP:
		case AT_CLASSLIMIT:
		{
			//VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s] int64타입이므로 이곳에서 사용하면 안됨"), __FUNCTIONW__);
			INFO_LOG(BM::LOG_LV5, __FL__<<L"int64는 이 곳에서 사용하면 안됨 AbilType["<<wType<<L"]");
		}break;
	}
	return pkDef->GetAbil(wType);
}

int CEffectDefMgr::GetCallEffectNum(int const iEffectNo) const
{
	int iBasic = GetAbil(iEffectNo, AT_TEMPLATE_SKILLNO);
	if (0 == iBasic)
	{
		iBasic = GetAbil(iEffectNo, AT_PARENT_SKILL_NUM);
		if (0 == iBasic)
		{
			iBasic = GET_BASICSKILL_NUM(iEffectNo);
		}
	}
	return iBasic;
}

