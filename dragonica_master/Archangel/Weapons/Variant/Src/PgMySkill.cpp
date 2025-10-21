#include "StdAfx.h"
#include "constant.h"
#include "PgMySkill.h"
#include "Skill.h"
#include "TableDataManager.h"
#include "PgControlDefMgr.h"
#include "InventoryUtil.h"

//
PgSaveSkillSetPacket::PgSaveSkillSetPacket() 
{}
PgSaveSkillSetPacket::PgSaveSkillSetPacket(CONT_USER_SKILLSET const & kContSkillSet) 
	: m_kContSkillSet(kContSkillSet) 
{}

void PgSaveSkillSetPacket::WriteToPacket(BM::Stream &rkPacket)const
{
	int const iSize = m_kContSkillSet.size();
	rkPacket.Push(iSize);

	for(CONT_USER_SKILLSET::const_iterator c_iter=m_kContSkillSet.begin();c_iter!=m_kContSkillSet.end();++c_iter)
	{
		if( IsEmptySkillGroup(c_iter->second) )
		{
			rkPacket.Push(ESPT_EMPTY);
			rkPacket.Push(c_iter->first);
		}
		else
		{
			rkPacket.Push(ESPT_NOT_EMPTY);
			rkPacket.Push(c_iter->second);
		}
	}
}
bool PgSaveSkillSetPacket::ReadFromPacket(BM::Stream &rkPacket)
{
	int iSize = 0;
	bool bRet = false;
	bRet = bRet | rkPacket.Pop(iSize);
	for(int i=0; i<iSize; ++i)
	{
		CONT_USER_SKILLSET::mapped_type element;
		
		ESKILLSET_PACKETTYTE eType = ESPT_NONE;
		bRet = bRet | rkPacket.Pop(eType);
		if(ESPT_EMPTY==eType)
		{			
			CONT_USER_SKILLSET::mapped_type::KEY_TYPE kKey = 0;
			bRet = bRet | rkPacket.Pop(kKey);
			element.bySetNo = kKey;
		}
		else
		{
			bRet = bRet | rkPacket.Pop(element);
		}

		m_kContSkillSet.insert(std::make_pair(element.bySetNo, element));
	}

	return bRet;
}
bool PgSaveSkillSetPacket::IsEmptySkillGroup(CONT_USER_SKILLSET::mapped_type const & rkSkillSet)const
{
	for(int i=0; i<MAX_SKILLSET_GROUP; ++i)
	{
		if(rkSkillSet.byDelay[i]){ return false; }
		if(rkSkillSet.iSkillNo[i]){ return false; }
	}
	return true;
}
void PgSaveSkillSetPacket::Clear()
{
	m_kContSkillSet.clear();
}
CONT_USER_SKILLSET const & PgSaveSkillSetPacket::ContSkillSet()
{
	return m_kContSkillSet;
}


//
PgMySkill::PgMySkill() :
	m_kAllSkillOver(0)
{
}

PgMySkill::~PgMySkill()
{
}

PgMySkill& PgMySkill::operator=(PgMySkill const& rhs)
{
	m_kGeneral = rhs.m_kGeneral;
	m_kActive = rhs.m_kActive;
	m_kPassive = rhs.m_kPassive;
	m_kToggle = rhs.m_kToggle;
	m_kToggleOn = rhs.m_kToggleOn;

	m_kClassSkillOverCont = rhs.m_kClassSkillOverCont;
	m_kSkillOverCont = rhs.m_kSkillOverCont;
	m_kAllSkillOver = rhs.m_kAllSkillOver;
	m_kSkillExtend = rhs.m_kSkillExtend;
	m_kContSkillSet = rhs.m_kContSkillSet;
	return *this;
}

void PgMySkill::Swap( PgMySkill & rhs )
{
	m_kGeneral.swap( rhs.m_kGeneral );
	m_kActive.swap( rhs.m_kActive );
	m_kPassive.swap( rhs.m_kPassive );
	m_kToggle.swap( rhs.m_kToggle );
	m_kToggleOn.swap( rhs.m_kToggleOn );

	m_kClassSkillOverCont.swap( rhs.m_kClassSkillOverCont );
	m_kSkillOverCont.swap( rhs.m_kSkillOverCont );
	std::swap( m_kAllSkillOver, rhs.m_kAllSkillOver );
	m_kSkillExtend.swap(rhs.m_kSkillExtend);
	m_kContSkillSet.swap(rhs.m_kContSkillSet);
}

HRESULT PgMySkill::GetContainer(ESkillType const eMST, CONT_MYSKILL*& pkCont)
{
	switch(eMST)
	{
	case EST_GENERAL:	{	pkCont = &m_kGeneral;	}break;
	case EST_ACTIVE:	{	pkCont = &m_kActive;	}break;
	case EST_PASSIVE:	{	pkCont = &m_kPassive;	}break;
	case EST_TOGGLE:	{	pkCont = &m_kToggle;	}break;
	case EST_TOGGLE_ON:	{	pkCont = &m_kToggleOn;	}break;
	default:			
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return E_FAIL;			
		}break;
	}
	return S_OK;
}

HRESULT PgMySkill::GetContainer(ESkillType const eMST, CONT_MYSKILL const*& pkCont) const
{
	switch(eMST)
	{
	case EST_GENERAL:	{	pkCont = &m_kGeneral;	}break;
	case EST_ACTIVE:	{	pkCont = &m_kActive;	}break;
	case EST_PASSIVE:	{	pkCont = &m_kPassive;	}break;
	case EST_TOGGLE:	{	pkCont = &m_kToggle;	}break;
	case EST_TOGGLE_ON:	{	pkCont = &m_kToggleOn;	}break;
	default:			
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return E_FAIL;			
		}break;
	}
	return S_OK;
}

void PgMySkill::Add( int const iSkill, BYTE const byIndex )
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkill);
	if ( pkSkillDef )
	{
		ESkillType const kType = static_cast<ESkillType>(pkSkillDef->GetAbil(AT_TYPE));
		int const iDivideType = pkSkillDef->GetAbil(AT_SKILL_DIVIDE_TYPE);
		SMySkill const kAddSkill( iSkill, static_cast< ESkillDivideType >(iDivideType), byIndex );
		Add( kType, kAddSkill );
	}
}

void PgMySkill::Add(size_t const iBufferSize, BYTE const* pkBuffer)
{
	size_t iIndex = 0;
	while(iIndex + sizeof(int) <= iBufferSize)
	{
		int const iSkill = *(int*)(pkBuffer+iIndex);
		if ( 0 < iSkill )
		{
			Add( iSkill, 0 );
		}
		iIndex += sizeof(int);
	}
}

void PgMySkill::InitSkillExtend(size_t const iExtendBufferSize, BYTE const * pkExtendBuffer)
{
	size_t iCurBufferPos = 0;
	while(iCurBufferPos < iExtendBufferSize)
	{
		int const iSkillBase = *(int*)(pkExtendBuffer + iCurBufferPos);
		iCurBufferPos += sizeof(int);
		BYTE const bExtendLevel = *(BYTE*)(pkExtendBuffer + iCurBufferPos);
		iCurBufferPos += sizeof(BYTE);

		if(0 < iSkillBase)
		{
			m_kSkillExtend[iSkillBase] = bExtendLevel;
		}
	}
}

bool PgMySkill::Add(ESkillType const eSkillType, SMySkill const& rkSkill)
{
	CONT_MYSKILL* pkCont = NULL;
	if(S_OK == GetContainer(eSkillType, pkCont))
	{
		CONT_MYSKILL::const_iterator itor = find(pkCont->begin(), pkCont->end(), rkSkill);
		if(itor == pkCont->end())
		{
			std::back_inserter(*pkCont) = rkSkill;
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMySkill::LearnedSkill(ESkillType const eSkillType, int const iSkillNo)
{
	CONT_MYSKILL* pkCont = NULL;
	if(S_OK == GetContainer(eSkillType, pkCont))
	{
		int const iFound = Exist(*pkCont, iSkillNo);
		return ((iFound > 0) ? (true) : (false));
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


void PgMySkill::Init(void)
{
	int i = EST_MIN;
	while(EST_MAX != i)
	{
		CONT_MYSKILL* pkCont = NULL;
		if(S_OK == GetContainer((ESkillType)i, pkCont))
		{
			pkCont->clear();
		}
		++i;
	}

	CONT_SKILLEXTEND().swap(m_kSkillExtend);
}

bool PgMySkill::SetExtendLevel(int const iSkillNo,BYTE const bExtendLevel)
{
	int const iBaseSkillNo = GET_BASICSKILL_NUM(iSkillNo);
	
	CONT_SKILLEXTEND::iterator iter = m_kSkillExtend.find(iBaseSkillNo);
	if(iter == m_kSkillExtend.end())
	{
		return m_kSkillExtend.insert(std::make_pair(iBaseSkillNo, bExtendLevel)).second;
	}

	if(bExtendLevel <= (*iter).second)
	{
		return false;
	}

	(*iter).second = bExtendLevel;

	return true;
}

BYTE PgMySkill::GetExtendLevel(int const iSkillNo) const
{
	int const iBaseSkillNo = GET_BASICSKILL_NUM(iSkillNo);
	CONT_SKILLEXTEND::const_iterator iter = m_kSkillExtend.find(iBaseSkillNo);
	if(iter != m_kSkillExtend.end())
	{
		return (*iter).second;
	}
	return 0;
}

bool PgMySkill::IsExist(int const iSkillNo, bool const bOverSkill) const
{
	//iSkillNo은 실제 스킬 번호가 올 수도 있고, OverSkillNo이 올 수 있다.
	int i = EST_MIN;
	while(EST_MAX != i)
	{
		CONT_MYSKILL const* pkCont = NULL;
		if(S_OK == GetContainer((ESkillType)i, pkCont))
		{
			if(Exist(*pkCont, iSkillNo))
			{				
				return true;
			}
		}
		++i;
	}

	if(bOverSkill)
	{
		int const iCheckSkillNo = GetLearnedSkill(iSkillNo, bOverSkill);
		if(iCheckSkillNo < iSkillNo) // 넘어온 번호와 오버된 번호가 같은지 확인(오버되지 않았으면 iSkillNo == iLearnedSkill == iOverSkill)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		else
		{
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

size_t PgMySkill::SaveExtend(size_t const iBufferSize, BYTE* pkBuffer)const
{
	memset(pkBuffer,0,iBufferSize);

	int iUsedBufferPos = 0;
	for(CONT_SKILLEXTEND::const_iterator iter = m_kSkillExtend.begin();iter != m_kSkillExtend.end();++iter)
	{
		if(iUsedBufferPos >= iBufferSize)
		{
			return iUsedBufferPos;
		}

		*(int*)(pkBuffer+iUsedBufferPos) = (*iter).first; iUsedBufferPos += sizeof(int);
		*(BYTE*)(pkBuffer+iUsedBufferPos) = (*iter).second; iUsedBufferPos += sizeof(BYTE);
	}
	return iUsedBufferPos;
}

size_t PgMySkill::Save(size_t const iBufferSize, BYTE* pkBuffer, ContSkillDivideType const& rkExceptionDivideType)
{
	if( !pkBuffer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	::memset(pkBuffer, 0, iBufferSize);

	size_t iIndex = 0;
	int i = EST_MIN;
	while(EST_MAX_STATIC != i)
	{
		CONT_MYSKILL* pkCont = NULL;
		if(S_OK == GetContainer((ESkillType)i, pkCont))
		{
			if( !pkCont->empty() )
			{
				size_t const copy_size = sizeof(SKILLNO_TYPE);
				CONT_MYSKILL::const_iterator iter = pkCont->begin();
				while(pkCont->end() != iter)
				{
					 CONT_MYSKILL::value_type const& rkElement = (*iter);
					bool bCanSaveSkill = false;
					if( !rkExceptionDivideType.empty() )//예외 조항이 설정 되었다
					{
						size_t iCur = 0;
						ContSkillDivideType::const_iterator find_iter = rkExceptionDivideType.find(rkElement.cDivideType);
						if( rkExceptionDivideType.end() != find_iter)
						{
							bCanSaveSkill = true;
						}
					}
					else//기본 설정
					{
						switch( rkElement.cDivideType )
						{
						case SDT_Normal:
						case SDT_Special:
						case SDT_Job:
							{
								bCanSaveSkill = true; //플레이어 스킬만 세이브 가능
							}break;
						default:
							{
							}break;
						}
					}

					if( bCanSaveSkill )
					{
						if ( iBufferSize >= iIndex+copy_size )//카피될 장소가 있다.
						{
							::memcpy(pkBuffer+iIndex, &(rkElement.iSkillNo), copy_size);
							iIndex += copy_size;
						}
						else
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Overflow Save Buff");
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Overflow Save Buff"));
							return iIndex;
						}
					}

					++iter;
				}
			}
		}
		++i;
	}
	return iIndex;
}

ELearnSkillRet PgMySkill::LearnSkillCheck(int const iSkillNo, int const iLevel, int const iClass)
{
	int const iBefore = GetLearnedSkill(iSkillNo);						

	if( iSkillNo <= iBefore )
	{
		return LS_RET_ERROR_SYSTEM;
	}

	CONT_DEFUPGRADECLASS const* pkUpClass = NULL;
	g_kTblDataMgr.GetContDef(pkUpClass);
	if( !pkUpClass )
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Get CONT_DEFUPGRADECLASS....");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_ERROR_SYSTEM"));
		return LS_RET_ERROR_SYSTEM;
	}
	
	GET_DEF( CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkDef )
	{
		INFO_LOG( BM::LOG_LV0, __FL__<<L"Error SkillNo["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_ERROR_DEF"));
		return LS_RET_ERROR_DEF;
	}
	
	int const iLevelLimit = pkDef->GetAbil(AT_LEVELLIMIT);
	if ( iLevel < iLevelLimit )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_LEVEL_LIMIT"));
		return LS_RET_LEVEL_LIMIT;
	}

	int const iLearnLevel = pkDef->GetAbil(AT_LEVEL);
	int const iMaxLearnLevel = MAX_SKILL_LEVEL + GetExtendLevel(iSkillNo);

	if(iMaxLearnLevel < iLearnLevel)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_LEVEL_LIMIT_MAX"));
		return LS_RET_LEVEL_LIMIT_MAX;
	}

	__int64 const i64ClassLimit = pkDef->GetAbil64(AT_CLASSLIMIT);
	if( !IS_CLASS_LIMIT( i64ClassLimit, iClass) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_CLASS_LIMIT"));
		return LS_RET_CLASS_LIMIT;
	}

	int iValue = 0;
	int iIndex = 0;
	while ((iValue = pkDef->GetAbil(AT_NEED_SKILL_01 + iIndex)) != 0 && iIndex < 11)
	{
		bool bFind = false;
		int iLevel = kSkillDefMgr.GetAbil(iValue, AT_LEVEL);
		while (iValue != 0)
		{
			if ( IsExist(iValue) )
			{
				bFind = true;
				break;
			}
			iValue = kSkillDefMgr.GetLevelupSkill(iValue, ++iLevel);
		}

		if (!bFind)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_NEEDSKILL_LIMIT"));
			return LS_RET_NEEDSKILL_LIMIT;
		}
		++iIndex;
	}

	iValue = pkDef->GetAbil(AT_MAINSKILL);
	if ( iValue )
	{
		bool bFind = false;
		int iLevel = 1;
		while ( iValue )
		{
			if ( IsExist(iValue) )
			{
				bFind = true;
				break;
			}
			iValue = kSkillDefMgr.GetLevelupSkill(iValue, iLevel);
			++iLevel;
		}

		if (!bFind)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return LS_RET_MAIN_SKILL_LIMIT"));
			return LS_RET_MAIN_SKILL_LIMIT;
		}
	}

	return LS_RET_SUCCEEDED;
}

HRESULT PgMySkill::LearnNewSkill(int const iSkillNo)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (pkSkillDef == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	int const iDivideType = pkSkillDef->GetAbil(AT_SKILL_DIVIDE_TYPE);
	int const iBefore = GetLearnedSkill(iSkillNo);
	Delete(iBefore);

	SMySkill const kNewSkill(iSkillNo, (BYTE)iDivideType, 0);
	Add((ESkillType)pkSkillDef->GetAbil(AT_TYPE), kNewSkill);
	return S_OK;
}

bool PgMySkill::Delete(int const iSkillNo)
{
	bool bRet = false;

	int i = EST_MIN;
	while(i != EST_MAX_STATIC)
	{
		if ( true == Delete((ESkillType)i, iSkillNo) )
		{
			bRet = true;
		}

		++i;
	}
	return bRet;
}

int PgMySkill::GetSkillNo(ESkillType eType, size_t const iIndex)
{
	CONT_MYSKILL *pkCont = NULL;
	if(S_OK == GetContainer(eType, pkCont))
	{
		if(pkCont->size() > iIndex) 
		{
			return pkCont->at(iIndex);
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

void PgMySkill::WriteToPacket( EWRITETYPE const kWriteType, BM::Stream& rkPacket )
{
	rkPacket.Push(kWriteType);
	
	switch( WTCHK_TYPE & kWriteType )
	{
	case WT_DEFAULT:
	case WT_DEFAULT_WITH_EXCLUDE:
		{
			if ((kWriteType & WT_OP_No_Contents) == 0)
			{
				int i = EST_MIN;
				while(i != EST_MAX)
				{
					CONT_MYSKILL *pkCont = NULL;
					if(S_OK == GetContainer((ESkillType)i, pkCont))
					{
						PU::TWriteArray_M(rkPacket, *pkCont);
					}
					++i;
				}

				PU::TWriteTable_AA(rkPacket, m_kSkillExtend);
			}
			else
			{
				CONT_MYSKILL *pkCont = NULL;
				if( S_OK == GetContainer(EST_TOGGLE_ON, pkCont) )
				{
					PU::TWriteArray_M( rkPacket, *pkCont );
				}
			}

			PgSaveSkillSetPacket kData(ContSkillSet());
			kData.WriteToPacket(rkPacket);
		}break;
	case WT_SIMPLE:
		{
			CONT_MYSKILL *pkCont = NULL;
			if( S_OK == GetContainer(EST_PASSIVE, pkCont) )
			{
				PU::TWriteArray_M(rkPacket, *pkCont);
			}

			if( S_OK == GetContainer(EST_TOGGLE_ON, pkCont) )
			{
				PU::TWriteArray_M(rkPacket, *pkCont);
			}
		}break;
	default:
		{
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled WriteType[%d]"), __FUNCTIONW__, kWriteType);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning! Invalid CaseType"));
		}break;
	}
}

void PgMySkill::ReadFromPacket( BM::Stream& rkPacket )
{
	EWRITETYPE kWriteType = WT_NONE;
	rkPacket.Pop(kWriteType);

	switch( kWriteType&WTCHK_TYPE )
	{
	case WT_DEFAULT:
	case WT_DEFAULT_WITH_EXCLUDE:
		{
			if ( (kWriteType & WT_OP_No_Contents) == 0)
			{
				Init();
				int i = EST_MIN;
				while(i != EST_MAX)
				{
					CONT_MYSKILL *pkCont = NULL;
					if(S_OK == GetContainer((ESkillType)i, pkCont))
					{
						pkCont->clear();
						PU::TLoadArray_M(rkPacket, *pkCont);
					}
					++i;
				}

				PU::TLoadTable_AA(rkPacket, m_kSkillExtend);
			}
			else
			{
				CONT_MYSKILL *pkCont = NULL;
				if( S_OK == GetContainer(EST_TOGGLE_ON, pkCont) )
				{
					pkCont->clear();
					PU::TLoadArray_M( rkPacket, *pkCont );
				}
			}

			PgSaveSkillSetPacket kSavePacket;
			kSavePacket.ReadFromPacket(rkPacket);
			m_kContSkillSet = kSavePacket.ContSkillSet();
		}break;
	case WT_SIMPLE:
		{
			CONT_MYSKILL *pkCont = NULL;
			if(S_OK == GetContainer(EST_PASSIVE, pkCont))
			{
				pkCont->clear();
				PU::TLoadArray_M(rkPacket, *pkCont);
			}

			if(S_OK == GetContainer(EST_TOGGLE_ON, pkCont))
			{
				pkCont->clear();
				PU::TLoadArray_M(rkPacket, *pkCont);
			}
		}break;
	default:
		{
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled WriteType[%d]"), __FUNCTIONW__, kWriteType);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning! Invalid CaseType"));
		}break;
	}
}

// 같은 종류(레벨만 다른 스킬)의 배운 스킬ID를 return한다
// [PARAMETER]
//	iSkillNo : 찾고자 하는 대표 스킬ID
// [RETURN]
//	iSkillNo와 같은 종류의 배운 스킬이 있으면 해당 SkillNo return
//	없으면 return 0
int PgMySkill::GetLearnedSkill(int const iSkillNo, bool const bOverSkill) const
{
	int const iModSkill = GET_BASICSKILL_NUM(iSkillNo);

	size_t iIndex = 0;
	int i = EST_MIN;
	while(i != EST_MAX_STATIC)
	{
		CONT_MYSKILL const* pkCont = NULL;
		if(S_OK == GetContainer((ESkillType)i, pkCont))
		{
			int iLearn = ExistBasicSkill(*pkCont, iModSkill);
			if (iLearn > 0)
			{
				if(bOverSkill)
				{
					iLearn = GetOverSkill(iLearn); // 오버된 스킬을 구한다.
				}
				return iLearn;
			}
		}
		++i;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int PgMySkill::GetNeedSP(int const iNewSkill, bool const bLearnNewSkill)
{
	int const iMySkill = bLearnNewSkill ? GetLearnedSkill(iNewSkill) : 0;
	int iCurrent = ((iMySkill > 0) ? (iMySkill+1) : (GET_BASICSKILL_NUM(iNewSkill)));
	int iSP = 0;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	for (;iCurrent <= iNewSkill; ++iCurrent)
	{
		CSkillDef const* pkDef = kSkillDefMgr.GetDef(iCurrent);
		if( NULL == pkDef )
		{
			INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot Get SkillDef SkillID["<<iCurrent<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 99999999"));
			return 99999999;	// 무지무지 큰수를 return 하여야 한다.
			//return INT_MAX;
		}
		iSP += pkDef->GetAbil(AT_NEED_SP);
	}
	return iSP;
}

bool PgMySkill::SetToggle(bool const bOn, int const iSkillNo)
{
	CONT_MYSKILL *pkCont = NULL;
	
	if(!LearnedSkill(EST_TOGGLE, iSkillNo)
		&& !IsExist(iSkillNo, true) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 토글은 베이스 스킬을 저장 해둔다.
	int const iModSkill = GET_BASICSKILL_NUM(iSkillNo);

	if(bOn)
	{
		return Add(EST_TOGGLE_ON, SMySkill(iModSkill, SDT_TOGGLE, 0));
	}
	else
	{
		return Delete(EST_TOGGLE_ON, iModSkill);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int PgMySkill::Exist(CONT_MYSKILL const& rkVector, int const iSkillNo) const
{
	CONT_MYSKILL::const_iterator itor = find(rkVector.begin(), rkVector.end(), iSkillNo);
	if(itor != rkVector.end())
	{
		return *itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

bool PgMySkill::Get(CONT_MYSKILL const& rkVector, int const iSkillNo, SMySkill& rkOut )const
{
	CONT_MYSKILL::const_iterator itr = find(rkVector.begin(), rkVector.end(), iSkillNo);
	if(itr != rkVector.end())
	{
		rkOut = *itr;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << L"Return false" );
	return false;
}

int PgMySkill::ExistBasicSkill(CONT_MYSKILL const& rkVector, int const iSkillNo) const
{
	CONT_MYSKILL::const_iterator itor = rkVector.begin();
	while(itor != rkVector.end())
	{
		if(GET_BASICSKILL_NUM(*itor) == iSkillNo)
		{
			return *itor;
		}
		++itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

bool PgMySkill::Delete(ESkillType const eSkillType, int const iSkillNo)
{
	CONT_MYSKILL *pkCont = NULL;
	if(	S_OK == GetContainer(eSkillType, pkCont))
	{
		CONT_MYSKILL::iterator itor = std::remove(pkCont->begin(), pkCont->end(), iSkillNo);
		bool const bRet = itor != pkCont->end();
		pkCont->erase(itor, pkCont->end());
		return bRet;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgMySkill::GetHaveSkills(ESkillDivideType const kSkillDivideType, ContSkillNo& rkOut)const
{
	size_t iCur = 0;
	while(iCur != EST_MAX_STATIC)
	{
		CONT_MYSKILL const* pkCont = NULL;
		if(S_OK == GetContainer((ESkillType)iCur, pkCont))
		{
			CONT_MYSKILL::const_iterator iter = pkCont->begin();
			while(pkCont->end() != iter)
			{
				CONT_MYSKILL::value_type const& rkElement = (*iter);
				if( kSkillDivideType == rkElement.cDivideType )
				{
					std::back_inserter(rkOut) = rkElement.iSkillNo;
				}
				++iter;
			}
		}
		++iCur;
	}
}

bool PgMySkill::EraseSkill(ESkillDivideType const eSkillDivideType, int* piOutputSP)
{
	if (piOutputSP != NULL)
	{
		*piOutputSP = 0;
	}
	
	//GET_DEF(CSkillDefMgr, kSkillDefMgr);

	switch(eSkillDivideType)
	{
	case SDT_Special:
		{
			int iCur = EST_MIN;
			while( iCur != EST_MAX )
			{
				CONT_MYSKILL* pkCont = NULL;
				if( S_OK == GetContainer(static_cast< ESkillType >(iCur), pkCont) )
				{
					CONT_MYSKILL::iterator loop_iter = pkCont->begin();
					while( pkCont->end() != loop_iter )
					{
						CONT_MYSKILL::value_type& rkLearnSkill = (*loop_iter);
						if( eSkillDivideType == rkLearnSkill.cDivideType )
						{
							// SP 들여서 배운 스킬들 SP반환 먼저
							if( piOutputSP )
							{
								*piOutputSP += GetNeedSP(rkLearnSkill.iSkillNo, false);
							}
							// 1레벨 스킬로 교체
							rkLearnSkill.iSkillNo = GET_BASICSKILL_NUM(rkLearnSkill.iSkillNo);
						}

						++loop_iter;
					}
				}
				++iCur;
			}
		}break;
	case SDT_Item:
	case SDT_Guild:
	case SDT_Couple:
	case SDT_Wedding:
	case SDT_Normal:
		{
			// 동적 스킬 종류만 제거 가능
			int iCur = EST_MIN;
			while(EST_MAX != iCur)
			{
				CONT_MYSKILL* pkCont = NULL;
				if( S_OK == GetContainer(static_cast< ESkillType >(iCur), pkCont) )
				{
					CONT_MYSKILL::iterator itor_remove = std::partition(pkCont->begin(), pkCont->end(), PgRemoveFilterIfNot(eSkillDivideType));
					if (piOutputSP)
					{
						CONT_MYSKILL::iterator itor_temp = itor_remove;
						// Skill을 지우면 복구될 수 있는 SP를 계산하자.
						while (pkCont->end() != itor_temp)
						{
							*piOutputSP += GetNeedSP((*itor_temp).iSkillNo, false);
							++itor_temp;
						}
					}
					pkCont->erase(itor_remove, pkCont->end());
				}
				++iCur;
			}
		}break;
	default:
		{
			// 제거 할 수 없는 종류
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Undefined SkilDivideType =") << eSkillDivideType);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}
	return true;
}

void PgMySkill::ClearOverSkillLevel(eSkillOverLevelType const eType)
{
	switch(eType)
	{
	case SOLT_ONLY_ONE_SKILL:
		{
			m_kSkillOverCont.clear();
		}break;
	case SOLT_ALL_SKILL:
		{
			m_kAllSkillOver = 0;
		}break;
	case SOLT_CLASS_SKILL:
		{
			m_kClassSkillOverCont.clear();
		}break;
	case SOLT_MAX:
		{
			m_kSkillOverCont.clear();
			m_kAllSkillOver = 0;
			m_kClassSkillOverCont.clear();
		}break;
	}
}

void PgMySkill::AddOverSkillLevel(eSkillOverLevelType const eType, int const iTypeValue, int const iLevel)
{
	switch(eType)
	{
	case SOLT_ONLY_ONE_SKILL:
		{
			PgInventoryUtil::GetOverSkill(m_kSkillOverCont, iTypeValue, iLevel);
		}break;
	case SOLT_ALL_SKILL:
		{
			m_kAllSkillOver += iLevel;
		}break;
	case SOLT_CLASS_SKILL:
		{
			PgInventoryUtil::GetOverSkill(m_kClassSkillOverCont, iTypeValue, iLevel);
		}break;
	}

}

int PgMySkill::GetOverSkill(int const iSkillNo) const
{
	int const iModSkill = GET_BASICSKILL_NUM(iSkillNo); // 베이스 스킬	

	int iOverSkillNo = iSkillNo;

	// 전체 스킬 + @가 있을 경우
	if(0 < m_kAllSkillOver) 
	{
		iOverSkillNo += m_kAllSkillOver;
	}

	// 직업 스킬 +@가 있는 경우
	for(CONT_SKILL_OPT::const_iterator itor = m_kClassSkillOverCont.begin(); itor != m_kClassSkillOverCont.end(); ++itor)
	{
		int iOverClass = (*itor).first;

		// 해당 스킬 정보를 얻어 옴
		GET_DEF( CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkDef = kSkillDefMgr.GetDef(iSkillNo);

		// 스킬의 ClassLimit은 bit flag로 직업 값이 중복되어 있으나, 비트 위치는 올라갈 수록 상위 직업이 된다.
		// 1이 되는 가장 작은 값이 그 직업의 스킬에 해당 됨.
		// ex) (가장 작은 비트의 위치만 검사하면 되는 이유)
		// 전사 스킬 : 1000100000011000000110000001100010 ( 전사 스킬은 전사, 투사, 기사, 검투사, 성기사, 파괴자, 용기사, 무도가, 패왕 사용 가능)
		// 투사 스킬 : 0000000000010000000100000001000000 ( 투사 스킬은 투사, 검투사, 파괴자 사용 가능)
		__int64 const i64ClassLimit = pkDef->GetAbil64(AT_CLASSLIMIT);
		for(__int64 i = 0 ; i < 64; ++i)
		{
			if( (i64ClassLimit >> i) & 0x01 )
			{
				if(i == iOverClass)
				{
					iOverSkillNo += (*itor).second;
				}
			}
		}
	}

	// 해당 스킬 +@ 가 있는 경우
	CONT_SKILL_OPT::const_iterator itor = m_kSkillOverCont.find(iModSkill);
	if(itor != m_kSkillOverCont.end())
	{
		iOverSkillNo += (*itor).second;
	}

	if(iSkillNo != iOverSkillNo)
	{
		// 테이블 최대치를 넘는지 검사한다.	
		GET_DEF( CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkDef = NULL;
		do
		{
			pkDef = kSkillDefMgr.GetDef(iOverSkillNo);

			if(NULL == pkDef)
			{
				--iOverSkillNo;
			}
			else
			{
				// 실제로 존재하면 정지
				break;
			}

		}while(NULL == pkDef || 0 > (iSkillNo - iOverSkillNo));
	}

	return iOverSkillNo;
}

int PgMySkill::GetOverSkillLevel(int const iSkillNo) const
{
	int const iModSkill = GET_BASICSKILL_NUM(iSkillNo); // 베이스 스킬	

	int iOverLevel = 0;

	// 전체 스킬 + @가 있을 경우
	if(0 < m_kAllSkillOver) 
	{
		iOverLevel += m_kAllSkillOver;
	}

	// 직업 스킬 +@가 있는 경우
	for(CONT_SKILL_OPT::const_iterator itor = m_kClassSkillOverCont.begin(); itor != m_kClassSkillOverCont.end(); ++itor)
	{
		int iOverClass = (*itor).first;

		// 해당 스킬 정보를 얻어 옴
		GET_DEF( CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkDef = kSkillDefMgr.GetDef(iSkillNo);

		// 스킬의 ClassLimit은 bit flag로 직업 값이 중복되어 있으나, 비트 위치는 올라갈 수록 상위 직업이 된다.
		// 1이 되는 가장 작은 값이 그 직업의 스킬에 해당 됨.
		// ex) (가장 작은 비트의 위치만 검사하면 되는 이유)
		// 전사 스킬 : 1000100000011000000110000001100010 ( 전사 스킬은 전사, 투사, 기사, 검투사, 성기사, 파괴자, 용기사, 무도가, 패왕 사용 가능)
		// 투사 스킬 : 0000000000010000000100000001000000 ( 투사 스킬은 투사, 검투사, 파괴자 사용 가능)
		__int64 const i64ClassLimit = pkDef->GetAbil64(AT_CLASSLIMIT);
		for(__int64 i = 0 ; i < 64; ++i)
		{
			if( (i64ClassLimit >> i) & 0x01 )
			{
				if(i == iOverClass)
				{
					iOverLevel += (*itor).second;
				}
			}
		}
	}

	// 해당 스킬 +@ 가 있는 경우
	CONT_SKILL_OPT::const_iterator itor = m_kSkillOverCont.find(iModSkill);
	if(itor != m_kSkillOverCont.end())
	{
		iOverLevel += (*itor).second;
	}

	if(0 < iOverLevel)
	{
		// 테이블 최대치를 넘는지 검사한다.	
		int const iLearnedSkill = GetLearnedSkill(iSkillNo);

		GET_DEF( CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkDef = NULL;
		do
		{
			pkDef = kSkillDefMgr.GetDef(iLearnedSkill + iOverLevel);

			if(NULL == pkDef)
			{
				--iOverLevel;
			}
			else
			{
				// 실제로 존재하면 정지
				break;
			}

		}while(NULL != pkDef || 0 < iOverLevel);
	}

	return iOverLevel;
}

bool PgMySkill::GetSkillIndex( int const iSkillNo, BYTE &rkOutIndex )const
{
	SMySkill kMySkill;
	int i = EST_MIN;
	while(EST_MAX != i)
	{
		CONT_MYSKILL const* pkCont = NULL;
		if(S_OK == GetContainer((ESkillType)i, pkCont))
		{
			if( true == Get(*pkCont, iSkillNo, kMySkill) )
			{
				rkOutIndex = kMySkill.byIndex;
				return true;
			}
		}
		++i;
	}

	return false;
}

void PgMySkill::SwapOverSkillLevel(eSkillOverLevelType const eType, CONT_SKILL_OPT& kSkillOpt)
{
	switch(eType)
	{
	case SOLT_ONLY_ONE_SKILL:
		{
			m_kSkillOverCont.swap(kSkillOpt);
		}break;
	case SOLT_CLASS_SKILL:
		{
			m_kClassSkillOverCont.swap(kSkillOpt);
		}break;
	default :
		{
		}break;
	}
}

CONT_SKILL_OPT const PgMySkill::GetCalcOverSkillList(eSkillOverLevelType const eType) const
{
	CONT_SKILL_OPT kRetCont;
	switch(eType)
	{
	case SOLT_ONLY_ONE_SKILL:	{ kRetCont = m_kSkillOverCont; }break;
	case SOLT_CLASS_SKILL:		{ kRetCont = m_kClassSkillOverCont; }break;
	default :
		{
		}break;
	}

	GET_DEF( CSkillDefMgr, kSkillDefMgr);
	for(CONT_SKILL_OPT::iterator itor = kRetCont.begin(); itor != kRetCont.end(); ++itor)
	{
		// 테이블 최대치를 넘는지 검사한다.	
		int const iLearnedSkill = GetLearnedSkill((*itor).first);
		int iOverLevel = (*itor).second;

		CSkillDef const* pkDef = NULL;

		while(NULL != pkDef || 0 < iOverLevel)
		{
			pkDef = kSkillDefMgr.GetDef(iLearnedSkill + iOverLevel);

			if(NULL == pkDef)
			{
				--iOverLevel;
			}
			else
			{
				// 실제로 존재하면 정지
				break;
			}
		}

		(*itor).second = iOverLevel;
	}

	return kRetCont;
}
