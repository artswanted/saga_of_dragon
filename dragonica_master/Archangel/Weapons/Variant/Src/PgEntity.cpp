#include "stdafx.h"
#include "Global.h"
#include "PgEntity.h"
#include "lohengrin/dbtables.h"
#include "TableDataManager.h"
//#include "../PgClassDefMgr.h"
#include "PgControlDefMgr.h"

PgEntity::PgEntity()
:	m_bUseVisualField(false)
,	m_bEternalLife(false)
{
}

PgEntity::~PgEntity()
{
}

void PgEntity::Init()
{
	LifeTime(0);
	Caller(BM::GUID::NullData());
	Party(BM::GUID::NullData());
	m_bUseVisualField = false;
	m_bEternalLife = false;
	CUnit::Init();
}

HRESULT PgEntity::Create(void const *pkInfo )
{
	Init();

	SEntityInfo const *pkEntityInfo = (SEntityInfo const*)pkInfo;
	if ( SetInfo( pkEntityInfo ) )
	{
        /*
        //가디언의 경우 DefClass -> DefMonster -> DefGuardian -> Tunning 순으로 Abil이 세팅됨
        int const iTunningNo = pkEntityInfo->iTunningNo;
	    int const iGrade = GetAbil(AT_GRADE);
	    int const iTunningLv = pkEntityInfo->iTunningLevel;
	    TunningAbil(iTunningNo, iGrade, iTunningLv);*/
		SetAbil( AT_CANNOT_DAMAGE, 0 );	//공격받아서는 안된다.

		int iValue = GetAbil(AT_AI_TYPE);
		m_kAI.SetPattern((iValue>0) ? iValue : 1);
        if( ENTITY_GUARDIAN==GetAbil(AT_ENTITY_TYPE) )
        {
            m_kAI.SetCurrentAction(EAI_ACTION_OPENING);
        }
        else
        {
		    //m_kAI.SetCurrentAction(EAI_ACTION_OPENING);
		    // Dummy Entity의 경우 클라이언트에 어떠한 동기화 패킷도 보내면 안된다.
		    // 그래서 처음부터 IDLE 상태로 만들고 상태 전환이 없도록 했다.
			if(GetSkill() && GetAbil(AT_MON_SKILL_ON_FIRST))
			{
				GetSkill()->PushSkillStack(GetAbil(AT_MON_SKILL_ON_FIRST));
			}
		    m_kAI.SetCurrentAction(EAI_ACTION_IDLE);
        }
		for (int i=1; i<EAI_ACTION_MAX; ++i)
		{
			iValue = GetAbil(AT_AI_ACTIONTYPE_MIN + i);
			if (iValue != 0)
			{
				m_kAI.AddActionType((EAIActionType)i, iValue);
			}
		}

		int const iOpeningDelay = GetAbil(AT_MON_OPENING_TIME);
		if(0<iOpeningDelay)
		{
			SetDelay(iOpeningDelay);
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgEntity::IsSkipTunningAbil(int const iAbilType)const
{
    switch(iAbilType)
    {
    case AT_AI_TYPE:
        {
            return true;
        }break;
    }
    return false;
}

void PgEntity::TunningAbil(int const iNo, int const iGrade, int const iLv, bool const bSend, bool const bBroadcast)
{
	CONT_DEFMONSTERTUNNINGABIL::mapped_type const * pMonsterTunning = GetContTunningAbilDef(iNo,iGrade,iLv);
	if(!pMonsterTunning)
	{
		return;
	}

	CONT_DEFMONSTERABIL const * pkContAbil = NULL;
	g_kTblDataMgr.GetContDef(pkContAbil);
	if(!pkContAbil)
	{
		return;
	}

    SetAbil(AT_MONSTER_TUNNING_NO,iNo,bSend);
    SetAbil(AT_MONSTER_TUNNING_GRADE,iGrade,bSend);
    SetAbil(AT_MONSTER_TUNNING_USER_LV,iLv,bSend);

	for(int i=0; i<MAX_MONSTER_ABIL_ARRAY; ++i)
	{
		CONT_DEFMONSTERABIL::const_iterator c_abil = (*pkContAbil).find(pMonsterTunning->iAbil[i]);
		if((*pkContAbil).end()==c_abil)
		{
			continue;
		}

		for(int j=0; j<MAX_MONSTER_ABIL_ARRAY; ++j)
		{
			CONT_DEFMONSTERABIL::mapped_type const &kAbil = (*c_abil).second;
			int const iType = kAbil.aType[j];
			if(IsSkipTunningAbil(iType))
			{
				continue;
			}

			int const iValue = kAbil.aValue[j];
			switch(iType)
			{
			case AT_EXPERIENCE:
			case AT_MONEY:
			case AT_DIE_EXP:
			case AT_CLASSLIMIT:
				{
					SetAbil64(iType,iValue,bSend,bBroadcast);
				}break;
			default:
				{
					SetAbil(iType,iValue,bSend,bBroadcast);
				}break;
			}
		}
	}
}

int PgEntity::AutoHeal( unsigned long ulElapsedTime, float const fMultiplier )
{
	if ( !m_bEternalLife )
	{
		bool bAutoValue = false;
		if( ENTITY_GUARDIAN==GetAbil(AT_ENTITY_TYPE) )
		{
			int const iSecondType = GetAbil(AT_ENTITY_SECOND_TYPE);
			if(ENTITY_SEC_KTH_BASE_HILL==iSecondType || ENTITY_SEC_KTH_HILL==iSecondType)
			{
				bAutoValue = true;
			}
		}

		if(bAutoValue)
		{
			if(0 == GetAbil(AT_LEVEL))
			{
				//고지전 가디언의 포인트를 깍는다
				int const iPoint = GetAbil(AT_OCCUPY_POINT);
				int const iMinPoint = GetAbil(AT_MIN_OCCUPY_POINT);
				if(iPoint>iMinPoint)
				{
					AddAbil(AT_OCCUPY_POINT, -1);
				}
				else if(iPoint<iMinPoint)
				{
					AddAbil(AT_OCCUPY_POINT, 1);
				}
			}
		}
		else
		{
			m_kLifeTime = (m_kLifeTime <= ulElapsedTime) ? 0 : m_kLifeTime - ulElapsedTime;
			if ( !m_kLifeTime )
			{
				//SetState(US_DEAD);
				SetAbil(AT_HP,0);
				m_bEternalLife = true;
			}
		}
	}
	return 0;

//	return CUnit::AutoHeal( ulElapsedTime, fMultiplier );
}

void PgEntity::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket(rkPacket,kWriteType);
	rkPacket.Push(m_kCommon.sLevel);
	rkPacket.Push(m_kCaller);
	rkPacket.Push(m_kParty);
	rkPacket.Push(m_kLifeTime);
	rkPacket.Push( GetAbil(AT_DUEL) );
	rkPacket.Push(ActionID());
	rkPacket.Push(GetPos());
	CAbilObject::WriteToPacket(rkPacket);
}

EWRITETYPE PgEntity::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE kWriteType = CUnit::ReadFromPacket(rkPacket);
	rkPacket.Pop(m_kCommon.sLevel);
	rkPacket.Pop(m_kCaller);
	rkPacket.Pop(m_kParty);
	rkPacket.Pop(m_kLifeTime);
	int iValue = 0;
	rkPacket.Pop(iValue);	SetAbil(AT_DUEL, iValue);
	rkPacket.Pop(iValue);	ActionID(iValue);
	POINT3 ptPos;
	rkPacket.Pop(ptPos);
	SetPos(ptPos);	
	CAbilObject::ReadFromPacket(rkPacket);

	// 클라이언트는 Name을 서버로 받지 말고, 스스로 읽어 오도록 하자.
	if(ENTITY_GUARDIAN==GetAbil(AT_ENTITY_TYPE))
	{
		GET_DEF(CMonsterDefMgr, kMonsterDefMgr)
		const CMonsterDef *pkDef = kMonsterDefMgr.GetDef(GetAbil(AT_CLASS));
		if (pkDef != NULL)
		{
			std::wstring wName;
			if (g_kTblDataMgr.GetStringName(pkDef->NameNo(), wName))
			{
				Name(wName);
			}
		}
	}
	return kWriteType;
}

void PgEntity::Invalidate()
{
	m_kLifeTime = 0;

	CUnit::Invalidate();
}

bool PgEntity::SetInfo( SEntityInfo const *pkInfo )
{
	if( !pkInfo )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" pkInfo is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// ClassDef로 부터 모든 Abil 세팅하기
	GET_DEF(PgClassDefMgr, kClassDefMgr);
	CLASS_DEF_BUILT const* pkClassDef = kClassDefMgr.GetDef(pkInfo->kClassKey);
	if( !pkClassDef )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" Cannot Get ClassDef(Class:") << pkInfo->kClassKey.iClass << _T(", Level:") << pkInfo->kClassKey.nLv << _T(") caller " << pkInfo->kCaller) );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	CopyAbilFromClassDef(pkClassDef);

	SetID( pkInfo->kGuid );
	m_kCaller = pkInfo->kCaller;
	m_kParty = pkInfo->kParty;
	m_kLifeTime = kClassDefMgr.GetAbil(pkInfo->kClassKey, AT_LIFETIME);
	SetAbil( AT_C_MAX_HP, kClassDefMgr.GetAbil(pkInfo->kClassKey, AT_MAX_HP));
	SetAbil( AT_CLASS, pkInfo->kClassKey.iClass );
	SetAbil( AT_LEVEL, pkInfo->kClassKey.nLv );
	m_kCallerClassKey.iClass = pkInfo->kCallerClassKey.iClass;
	m_bySyncType = ( (true == pkInfo->bSyncUnit) ? SYNC_TYPE_DEFAULT : SYNC_TYPE_NONE );
	m_bEternalLife = pkInfo->bEternalLife;
	
	std::map<WORD, int>::const_iterator itor = pkInfo->kAbil.begin();
	while (itor != pkInfo->kAbil.end())
	{
		SetAbil(itor->first, itor->second);
		++itor;
	}
	return true;
}

int PgEntity::GetMyClass()const
{
	return m_kCallerClassKey.iClass ? m_kCallerClassKey.iClass : CUnit::GetMyClass();
}

bool PgEntity::IsCheckZoneTime( DWORD dwElapsed )
{
	if (m_dwElapsedTimeCheckZone < dwElapsed)
	{
		m_dwElapsedTimeCheckZone += 1500;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgEntity::AddExp(__int64 const& iAddValue)
{
	//BM::Stream kNfyPacket(PT_U_G_NFY_ADD_EXP);
	//kNfyPacket.Push(iAddValue);
	//VNotify(&kNfyPacket);
	return S_OK;
}

int PgEntity::GetAbil(WORD const Type) const
{
	switch( Type )
	{
	case AT_DETECT_RANGE:
		{
			if ( m_bUseVisualField )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
				return -1;
			}
		}break;
	case AT_OWNER_TYPE:
		{
			return CAbilObject::GetAbil(AT_OWNER_TYPE);
		}break;
	case AT_CALLER_TYPE:
		{
			return CAbilObject::GetAbil(AT_CALLER_TYPE);
		}break;
	}
	return CUnit::GetAbil(Type);
}

void PgEntity::CopyAbilFromClassDef(const CLASS_DEF_BUILT* pkDef)
{
	if (pkDef == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkDef is NULL"));
		return;
	}

	CLASSDEF_ABIL_CONT::const_iterator itor = pkDef->kAbil.begin();
	while (itor != pkDef->kAbil.end())
	{
		SetAbil(itor->first, itor->second);

		++itor;
	}
	if(0 == GetAbil(AT_MAX_HP))
	{
		SetAbil(AT_MAX_HP, static_cast<int>(pkDef->kDef.sMaxHP) );
	}
	if(0 == GetAbil(AT_HP))
	{
		SetAbil(AT_HP, GetAbil(AT_MAX_HP) );
	}
	
	SetAbil(AT_HP_RECOVERY_INTERVAL, static_cast<int>(pkDef->kDef.sHPRecoveryInterval) );
	SetAbil(AT_HP_RECOVERY, static_cast<int>(pkDef->kDef.sHPRecovery) );
	SetAbil(AT_MAX_MP, static_cast<int>(pkDef->kDef.sMaxMP) );
	SetAbil(AT_MP, static_cast<int>(pkDef->kDef.sMaxMP) );
	SetAbil(AT_MP_RECOVERY_INTERVAL, static_cast<int>(pkDef->kDef.sMPRecoveryInterval) );
	SetAbil(AT_MP_RECOVERY, static_cast<int>(pkDef->kDef.sMPRecovery) );
	SetAbil(AT_MOVESPEED, static_cast<int>(pkDef->kDef.sMoveSpeed) );
	SetAbil(AT_IDLE_TIME, m_kLifeTime * 2);	// AI로직에서 IDLE을 LifeTime동안 유지 하기 위해 세팅
}
