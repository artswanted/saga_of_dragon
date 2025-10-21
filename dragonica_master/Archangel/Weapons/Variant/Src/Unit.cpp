#include "stdafx.h"
#include <math.h>
#include "constant.h"
#include "Global.h"
#include "Unit.h"
#include "TableDataManager.h"
#include "PgTotalObjectMgr.h"
#include "PgControlDefMgr.h"
#include "InventoryUtil.h"
#include "BM/LocalMgr.h"

//EffectPool CUnit::m_kEffectPool(20, 10);

extern int GetElementAbilValue(CUnit const* pkUnit, eElementDmgType const Type, 
							   EGetAbilType::Enum const getabiltype = EGetAbilType::EGAT_ALL);

CUnit::CUnit()
:	m_kFrontDirection(DIR_NONE)
,	m_kMoveDir(DIR_NONE)
{
	Init();
}

CUnit::~CUnit()
{
	AggroMeter(NULL);
}

void CUnit::CalculateInitAbil()
{
	NftChangedAbil( AT_MAX_HP, E_SENDTYPE_NONE );
	NftChangedAbil( AT_HP_RECOVERY_INTERVAL, E_SENDTYPE_NONE );
	NftChangedAbil( AT_HP_RECOVERY, E_SENDTYPE_NONE );
	NftChangedAbil( AT_MAX_MP, E_SENDTYPE_NONE );
	NftChangedAbil( AT_MP_RECOVERY_INTERVAL, E_SENDTYPE_NONE );
	NftChangedAbil( AT_MP_RECOVERY, E_SENDTYPE_NONE );
	NftChangedAbil( AT_STR, E_SENDTYPE_NONE );
	NftChangedAbil( AT_INT, E_SENDTYPE_NONE );
	NftChangedAbil( AT_CON, E_SENDTYPE_NONE );
	NftChangedAbil( AT_DEX, E_SENDTYPE_NONE );
	NftChangedAbil( AT_MOVESPEED, E_SENDTYPE_NONE );
	NftChangedAbil( AT_PHY_DEFENCE, E_SENDTYPE_NONE );
	NftChangedAbil( AT_MAGIC_DEFENCE, E_SENDTYPE_NONE );
	NftChangedAbil( AT_ATTACK_SPEED, E_SENDTYPE_NONE );
	NftChangedAbil( AT_BLOCK_SUCCESS_VALUE, E_SENDTYPE_NONE );
	NftChangedAbil( AT_DODGE_SUCCESS_VALUE, E_SENDTYPE_NONE );
	NftChangedAbil( AT_CRITICAL_SUCCESS_VALUE, E_SENDTYPE_NONE );
	NftChangedAbil( AT_CRITICAL_POWER, E_SENDTYPE_NONE );
	NftChangedAbil( AT_HIT_SUCCESS_VALUE, E_SENDTYPE_NONE );
	NftChangedAbil( AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE );
	NftChangedAbil( AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE );
	NftChangedAbil( AT_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE );
	NftChangedAbil( AT_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE );
}


void CUnit::SetPos(POINT3 const& ptNew)
{
	if(GetPos() != ptNew)
	{
		m_kLastPos = GetPos();
	}
	IDObject::SetPos(ptNew);
}

POINT3BY const& CUnit::GetOrientedVector()
{
	return m_kOrientedVector;
}

void CUnit::SetOrientedVector(POINT3BY const& rkOrientedVec)
{
	m_kOrientedVector = rkOrientedVec;
}

BM::GUID const& CUnit::GetTarget()
{
	AdjustTargetList();
	DequeTarget::const_iterator itor = m_kTargetDeque.begin();
	if (m_kTargetDeque.end() == itor)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("TargetDeque is NullData"));
		return BM::GUID::NullData();
	}
	return itor->kGuid;
}

void CUnit::SetTarget(BM::GUID const& guidTarget)
{
	// �����ڵ�� ȣȯ�� ������ ���� �ϴ� �Լ��� ���� ���´�.
	//if (m_kTargetGuid.size() > 0)
	//{
	//	m_kTargetGuid.at(0) == guidTarget;
	//}
	//else
	//{
	//	m_kTargetGuid.push_back(guidTarget);
	//}
	//m_kTargetGuidVec.insert(m_kTargetGuidVec.begin(), guidTarget);
	if (guidTarget == BM::GUID::NullData())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("guidTarget is NullData"));
		ClearTargetList();
		return;
	}
	// ������ Target�� ���� �տ� ���°��� �����ϱ� ���ؼ�... Delete �Ѵ�.
	// ��, ���߿� GetTarget(..)���� �� SetTarget(..) ���� ������ ���� �Ѿ�� �Ѵ�.
	AddTarget(guidTarget, true);
}

bool CUnit::FindTarget(BM::GUID const& rkGuid)
{
	DequeTarget::const_iterator itor = m_kTargetDeque.begin();
	while (m_kTargetDeque.end() != itor)
	{
		if (itor->kGuid == rkGuid)
		{
			return true;
		}
		++itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void CUnit::AddTarget(BM::GUID const& guidTarget, bool const bFront)
{
	if (guidTarget == BM::GUID::NullData())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("guidTarget is NullData"));
		return;
	}

	if ( bFront )
	{
		DeleteTarget( guidTarget );
		m_kTargetDeque.push_front(STargetInfo(guidTarget));
		return;
	}

	if ( !FindTarget(guidTarget) )
	{
		m_kTargetDeque.push_back(STargetInfo(guidTarget));
	}
}

CUnit::DequeTarget& CUnit::GetTargetList()
{
	AdjustTargetList();
	return m_kTargetDeque;
}

BM::GUID const&  CUnit::GetReserveTarget()	//Ÿ�� ����. ���� �� �޵�� �ʱ�ȭ ����
{
	return m_kReserveTarget;
}

void CUnit::SetReserveTarget(BM::GUID const& guidTarget)
{
	m_kReserveTarget = guidTarget;
}

void CUnit::SetReserveTargetToDeque(bool const bClearReserve)
{
	AddTarget(GetReserveTarget(), true);
	if(bClearReserve)
	{
		m_kReserveTarget.Clear();
	}
}

void CUnit::Init()
{
	CAbilObject::Clear();//Ŭ���� �ݵ��.
	IDObject::Clear();

	Name(_T(""));
	m_Skill.Init();
	m_State = US_NONE;
	m_kTargetDeque.clear();
	m_dwHPElapsedTime = m_dwMPElapsedTime = 0;
	m_dwGroggyElapsedTime = 0;

	memset(&m_kCommon, 0, sizeof(UNIT_COMMON_ABIL));
	memset(&m_kBasic, 0, sizeof(SPlayerBasicInfo));
	m_iActionInstanceID = 1;

	LastTickTime(0);
	TickNum(0);
	m_kEffect.Clear();

	// ClearObserver() �� Unit ���ο��� ȣ���ϸ� Deadlock �߻���.
	//ClearObserver();
	m_kInv.Clear();
	m_kAI.Clear();
	DeathTime(0);
	WeaponDecCount(0);
	ArmorDecCount(0);

	m_iDelay = 0;
	m_iAttackDelay = 0;
	m_kSummonUnit.clear();
	m_kDieSummonUnit.clear();
	m_bySyncType = SYNC_TYPE_DEFAULT;

	m_kOrientedVector.Set(-1,0,0);
	m_kPathNormal.Set(0,1,0);
	m_kCaller.Clear();
	m_kDeliverDamageUnit.Clear();

	m_pkAggroMeter = NULL;
	StandUpTime(0);
	RandomSeedCallCounter(0);
	LastAreaIndex(0);
	SetReserveTarget(BM::GUID::NullData());
	UseRandomSeedType(true);
	SetRandomSeed(g_kEventView.GetServerElapsedTime());
	StartPos(POINT3::NullData());

	m_bDamageAction = false;
	m_kActionID = 0;
}

// TickTime Checking
// [PARAMETER]
//	dwTickGab : tick time interval what you want
//	dwElapsedTime : [IN/OUT]
//		IN : Elapsed time from before calling
//		OUT : result elapsed time
bool CUnit::CheckTickTime(DWORD const dwTickGab, DWORD& dwElapsedTime)
{
	m_kLastTickTime += (dwElapsedTime > dwTickGab) ? dwTickGab : dwElapsedTime;
	dwElapsedTime = m_kLastTickTime;
	if (dwTickGab <= m_kLastTickTime)
	{
		m_kLastTickTime -= dwTickGab;
		return true;
	}
	return false;
}
int CUnit::GetMyClass()const
{
	return GetAbil(AT_CLASS);
}

void CUnit::SendAbil64(EAbilType const eAT, DWORD const dwSendFlag)
{
	WORD wType = eAT;
	SendAbiles64(&wType, 1, dwSendFlag);//�ᱹ ��Ŷ�� ������ -_-; ���Ϸ� �̷����Ÿ���.
}

void CUnit::SendAbil(EAbilType const eAT, DWORD const dwSendFlag)
{
	WORD wType = eAT;
	this->SendAbiles(&wType, 1, dwSendFlag);//�ᱹ ��Ŷ�� ������ -_-; ���Ϸ� �̷����Ÿ���.
}

void CUnit::SendAbiles(WORD const* pkAbils, BYTE const byNum, DWORD const dwFlag)
{
	BM::Stream kAbilPacket(PT_M_C_NFY_STATE_CHANGE2, GetID());

	E_SENDABIL_TARGET eTarget = (dwFlag & E_SENDTYPE_EFFECTABIL) ? E_SENDABIL_TARGET_EFFECT : E_SENDABIL_TARGET_UNIT;
	kAbilPacket.Push((BYTE)eTarget);
	kAbilPacket.Push(byNum);
	for (int i=0; i<(int)byNum; ++i)
	{
		switch(pkAbils[i])
		{
		case AT_FRAN:
		case AT_EXPERIENCE:
		case AT_MONEY:
		case AT_DIE_EXP:
		case AT_CLASSLIMIT:
		case AT_TACTICS_EXP:
		case AT_CASH:
		case AT_BONUS_CASH:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[") << __FL__ << _T("] __int64 ó���ؾ� ��.") );
			}break;

		}
		switch(eTarget)
		{
		case E_SENDABIL_TARGET_EFFECT:
			{
				SAbilInfo kAbilInfo(pkAbils[i], GetEffectMgr().GetAbil(pkAbils[i]));
				kAbilPacket.Push(kAbilInfo);
			}break;
		default:
			{
				SAbilInfo kAbilInfo(pkAbils[i], GetAbil(pkAbils[i]));
				kAbilPacket.Push(kAbilInfo);
			}break;
		}
	}

	Send(kAbilPacket, dwFlag|E_SENDTYPE_MUSTSEND);
}

void CUnit::SendAbiles64(WORD const* pkAbils, BYTE const byNum, DWORD const dwFlag)
{
	BM::Stream kAbilPacket(PT_M_C_NFY_STATE_CHANGE64, GetID());

	kAbilPacket.Push(byNum);
	for (int i=0; i<(int)byNum; ++i)
	{
		switch(pkAbils[i])
		{
		case AT_FRAN:
		case AT_EXPERIENCE:
		case AT_MONEY:
		case AT_DIE_EXP:
		case AT_CLASSLIMIT:
		case AT_TACTICS_EXP:
		case AT_CASH:
		case AT_BONUS_CASH:
		case AT_REST_EXP_ADD_MAX:
		case AT_SKILL_LINKAGE_FLAG:
			{
				SAbilInfo64 kAbilInfo(pkAbils[i], GetAbil64(pkAbils[i]));
				kAbilPacket.Push(kAbilInfo);
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[") << __FL__ << _T("] int ó���ؾ� ��.") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			}break;
		}
	}

	Send(kAbilPacket, dwFlag);
}

bool CUnit::SendCaller( BM::Stream const &kPacket, DWORD const dwSendFlag )
{
	if ( BM::GUID::IsNotNull(Caller()) )
	{
		if ( GetID() != Caller() )
		{// ���� ȣ�� ���� -_-a
			BM::Stream kGPacket( PT_U_G_NFY_SEND_CALLER, Caller() );
			kGPacket.Push(dwSendFlag);
			kGPacket.Push( kPacket );
			VNotify(&kGPacket);
			return true;
		}
	}
	return false;
}

bool CUnit::SendObserver( BM::Stream const &kPacket )
{
	if ( m_kContObserver.empty() )
	{
		return false;
	}

	BM::Stream kWrappedPacket( PT_M_S_WRAPPED_USER_PACKET );
	size_t const iWRPos = kWrappedPacket.WrPos();

	size_t iSize = 0;
	kWrappedPacket.Push( iSize );

	PgNetModule<>::CONT_NETMODULE::const_iterator net_itr = m_kContObserver.begin();
	SERVER_IDENTITY kPrevNet = net_itr->GetNet(); 

	for ( ; net_itr != m_kContObserver.end() ; ++net_itr )
	{
		if ( kPrevNet != net_itr->GetNet() )
		{
			kWrappedPacket.Push( kPacket.Data() );
			kWrappedPacket.ModifyData( iWRPos, &iSize, sizeof(size_t) );
			g_kProcessCfg.Locked_SendToServer( kPrevNet, kWrappedPacket );

			iSize = 0;
			kPrevNet = net_itr->GetNet();
			kWrappedPacket.Reset( PT_M_S_WRAPPED_USER_PACKET );
			kWrappedPacket.Push( iSize );
		}

		kWrappedPacket.Push( net_itr->GetID() );
		++iSize;
	}

	if ( iSize )
	{
		kWrappedPacket.Push( kPacket.Data() );
		kWrappedPacket.ModifyData( iWRPos, &iSize, sizeof(size_t) );
		g_kProcessCfg.Locked_SendToServer( kPrevNet, kWrappedPacket );
	}

	return true;
}

void CUnit::Send(BM::Stream const &Packet, DWORD const dwSendFlag)
{//�⺻������ ������ ��ε�ĳ��Ʈ��. Player�� ���� �϶�.
	if((dwSendFlag & E_SENDTYPE_BROADCAST_GROUND) == E_SENDTYPE_BROADCAST_GROUND)
	{
		DWORD const dwNewSendFlag = (E_SENDTYPE_OPTION & dwSendFlag);

		BM::Stream kBPacket( PT_U_G_BROADCAST_GROUND, dwNewSendFlag );
		kBPacket.Push(Packet);
		VNotify(&kBPacket);
		return;// Notify���� �ڱ� �ڽſ��Ե� ��Ŷ�� ������.
	}
	
	if((dwSendFlag & E_SENDTYPE_BROADCAST) == E_SENDTYPE_BROADCAST)
	{
		DWORD const dwNewSendFlag = (E_SENDTYPE_OPTION & dwSendFlag);

		BM::Stream kBPacket( PT_U_G_BROADCAST_AREA, dwNewSendFlag );
		kBPacket.Push(Caller());// ���ܴ���� Caller
		kBPacket.Push(Packet);
		VNotify(&kBPacket);
	}

	if( E_SENDTYPE_SELF & dwSendFlag )
	{
		SendCaller( Packet, E_SENDTYPE_OPTION & dwSendFlag );
		SendObserver(Packet);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CUnit::GetAbil(WORD const Type) const
{
	int iValue = 0;
	//iValue = CAbilObject::GetAbil(Type);
	//if(iValue)
	//{ 
	//	return iValue; 
	//}
	switch(Type)
	{
	case AT_LEVEL:					{ iValue = m_kCommon.sLevel; }break;
	case AT_CLASS:					{ iValue = m_kCommon.iClass; }break;
	case AT_GENDER:					{ iValue = m_kCommon.byGender; }break;
	case AT_RACE:					{ iValue = m_kCommon.iRace; }break;
	case AT_MAX_HP:					{ iValue = m_kBasic.iMaxHP; }break;
	case AT_HP_RECOVERY_INTERVAL:	{ iValue = m_kBasic.iHPRecoveryInterval; }break;
	case AT_HP_RECOVERY:			{ iValue = m_kBasic.sHPRecovery; /* + GetAbil(AT_HP_RECOVERY_ADD_RATE); */}break;
	case AT_MAX_MP:					{ iValue = m_kBasic.iMaxMP; }break;
	case AT_MP_RECOVERY_INTERVAL:	{ iValue = m_kBasic.iMPRecoveryInterval; }break;
	case AT_MP_RECOVERY:			{ iValue = m_kBasic.sMPRecovery; /*+ GetAbil(AT_MP_RECOVERY_ADD_RATE); */}break;
	case AT_STR_BASIC:				{ iValue = m_kBasic.sInitStr; }break;
	case AT_INT_BASIC:				{ iValue = m_kBasic.sInitInt; }break;
	case AT_CON_BASIC:				{ iValue = m_kBasic.sInitCon; }break;
	case AT_DEX_BASIC:				{ iValue = m_kBasic.sInitDex; }break;
	case AT_STR:					{ iValue = m_kBasic.sInitStr + GetAbil(AT_STR_ADD); }break;
	case AT_INT:					{ iValue = m_kBasic.sInitInt + GetAbil(AT_INT_ADD); }break;
	case AT_CON:					{ iValue = m_kBasic.sInitCon + GetAbil(AT_CON_ADD); }break;
	case AT_DEX:					{ iValue = m_kBasic.sInitDex + GetAbil(AT_DEX_ADD); }break;
	case AT_MOVESPEED:				{ iValue = m_kBasic.sMoveSpeed; }break;
	case AT_PHY_DEFENCE:			{ iValue = m_kBasic.sPhysicsDefence; }break;
	case AT_MAGIC_DEFENCE:			{ iValue = m_kBasic.sMagicDefence; }break;
	case AT_ATTACK_SPEED:			{ iValue = m_kBasic.sAttackSpeed; }break;
	case AT_BLOCK_SUCCESS_VALUE:				{ iValue = m_kBasic.sBlockRate; }break;
	case AT_DODGE_SUCCESS_VALUE:				{ iValue = m_kBasic.sDodgeRate; }break;
	case AT_CRITICAL_SUCCESS_VALUE:			{ iValue = m_kBasic.sCriticalRate; }break;
	case AT_CRITICAL_POWER:			{ iValue = m_kBasic.sCriticalPower; }break;
	case AT_HIT_SUCCESS_VALUE:				{ iValue = m_kBasic.sHitRate; }break;
	case AT_AI_TYPE:				{ iValue = m_kBasic.iAIType; }break;
	case AT_C_ATTACK_RANGE:			{ iValue = m_Skill.GetAbil(AT_ATTACK_RANGE); }break;
	case AT_CHAT_BALLOOON_TYPE:
		{
			iValue = m_kInv.GetAbil(Type);
			if( 0 == iValue )
			{
				iValue = CAbilObject::GetAbil(Type);
			}
		}break;
	case AT_C_PHY_ATTACK:
		{
			int const iMin = GetAbil(AT_C_PHY_ATTACK_MIN);
			int const iGap = GetAbil(AT_C_PHY_ATTACK_MAX) - iMin;
			iValue = iMin + ((iGap == 0) ? 0 : BM::Rand_Index(iGap));
		}break;
	case AT_C_MAGIC_ATTACK:
		{
			int const iMin = GetAbil(AT_C_MAGIC_ATTACK_MIN);
			int const iGap = GetAbil(AT_C_MAGIC_ATTACK_MAX) - iMin;
			iValue = iMin + ((iGap == 0) ? 0 : BM::Rand_Index(iGap));
		}break;
	case AT_OWNER_TYPE:
	case AT_CALLER_TYPE:
		{
			return (int)UnitType();
		}break;
	case AT_FRAN:
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
	case AT_TACTICS_EXP:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[") << __FL__ << _T("] __int64 ó���ؾ� ��.") );
		}break;
	case AT_CAST_TIME_RATE:
	case AT_OFFENSE_PROPERTY:
	case AT_DEFENCE_PROPERTY:
	case AT_OFFENSE_PROPERTY_LEVEL:
	case AT_DEFENCE_PROPERTY_LEVEL:
		{
			// Monster�� ��쿡�� Unit�� ��� ���� �о�� �ϰ�
			// Player�� ��쿡�� Inventory�� ��� ���� �о�� �Ѵ�.
			iValue = CAbilObject::GetAbil(Type) + m_kInv.GetAbil(Type);
		}break;
	case AT_ADDEXP_FINAL:
		{
			// GetAbil(AT_ADD_EXP_PER) + GetAbil(AT_ADD_EXP_CAN_DUPLICATE)
			iValue = CAbilObject::GetAbil(Type);
			if (iValue == 0)	// 0 �̸� caching update
			{
				iValue = GetAbil(AT_ADD_EXP_PER) + GetAbil(AT_ADD_EXP_CAN_DUPLICATE) + 100;	// ó������ 100�� ���Ѱ� ���
				((CAbilObject*)this)->SetAbil(Type, iValue);	// value caching
			}
		}break;
	case AT_SKILL_EFFICIENCY:
	case AT_ADD_R_NEED_HP:
	case AT_ADD_R_NEED_MP:
	case AT_ADD_ENCHANT_RATE:
	case AT_ADD_SOULCRAFT_RATE:
	case AT_ADD_SOUL_RATE_REFUND:
	case AT_ADD_REPAIR_COST_RATE:
		{
			iValue = CAbilObject::GetAbil(Type) + m_kInv.GetAbil(Type);
		}break;
	case AT_HIT_DAMAGE_HP_RATE_ACTIVATE:
	case AT_HIT_DAMAGE_HP_RATE:	
	case AT_HIT_DECREASE_MP_RATE_ACTIVATE:
	case AT_HIT_DECREASE_MP_RATE:
	case AT_CRITICAL_DAMAGE_RATE_ACTIVATE:
	case AT_CRITICAL_DAMAGE_RATE:
	case AT_CRITICAL_DAMAGE_ACTIVATE:
	case AT_CRITICAL_DAMAGE:
	case AT_ATTACKED_DECREASE_DAMAGE_ACTIVATE_HP_RATE:
	case AT_ATTACKED_DECREASE_DAMAGE_RATE:
	case AT_JS_1ST_ADD_SKILL_TURNTIME:
	case AT_JS_1ST_ADD_USE_EXHAUSTION:
	case AT_JS_1ST_ADD_TOOL_USE_DURATION_RATE:
	case AT_JS_1ST_ADD_RESULT_ITEM_COUNT:
	case AT_JS_1ST_ADD_EXPERTNESS_RATE:
	case AT_JS_1ST_SUB_ADD_SKILL_TURNTIME:
	case AT_JS_1ST_SUB_ADD_USE_EXHAUSTION:
	case AT_JS_1ST_SUB_ADD_TOOL_USE_DURATION_RATE:
	case AT_JS_1ST_SUB_ADD_RESULT_ITEM_COUNT:
	case AT_JS_1ST_SUB_ADD_EXPERTNESS_RATE:
		{
			iValue = m_kInv.GetAbil( Type ) + m_kEffect.GetAbil( Type );
		}break;
	case AT_C_ATTACK_ADD_FIRE://30016
	case AT_C_ATTACK_ADD_ICE:
	case AT_C_ATTACK_ADD_NATURE:
	case AT_C_ATTACK_ADD_CURSE:
		{
			iValue = GetElementAbilValue(this, (eElementDmgType)(Type-30014));
		}break;
	case AT_C_ATTACK_ADD_DESTROY:
		{
			iValue = GetElementAbilValue(this, ET_D_DESTROY);
		}break;
	case AT_C_RESIST_ADD_FIRE:
	case AT_C_RESIST_ADD_ICE:
	case AT_C_RESIST_ADD_NATURE:
	case AT_C_RESIST_ADD_CURSE:
		{
			iValue = GetElementAbilValue(this, (eElementDmgType)(Type-30044));
		}break;
	case AT_C_RESIST_ADD_DESTROY:
		{
			iValue = GetElementAbilValue(this, ET_R_DESTROY);
		}break;
	case AT_BASE_ATTACK_ADD_FIRE://30331
	case AT_BASE_ATTACK_ADD_ICE:
	case AT_BASE_ATTACK_ADD_NATURE:
	case AT_BASE_ATTACK_ADD_CURSE:
		{
			iValue = GetElementAbilValue(this, (eElementDmgType)(Type-30329), EGetAbilType::EGAT_UNIT);
		}break;
	case AT_BASE_ATTACK_ADD_DESTROY:
		{
			iValue = GetElementAbilValue(this, ET_D_DESTROY, EGetAbilType::EGAT_UNIT);
		}break;
	case AT_BASE_RESIST_ADD_FIRE://30371
	case AT_BASE_RESIST_ADD_ICE:
	case AT_BASE_RESIST_ADD_NATURE:
	case AT_BASE_RESIST_ADD_CURSE:
		{
			iValue = GetElementAbilValue(this, (eElementDmgType)(Type-30359), EGetAbilType::EGAT_UNIT);
		}break;
	case AT_BASE_RESIST_ADD_DESTROY:
		{
			iValue = GetElementAbilValue(this, ET_R_DESTROY, EGetAbilType::EGAT_UNIT);
		}break;
	case AT_INV_ATTACK_ADD_FIRE://30341
	case AT_INV_ATTACK_ADD_ICE:
	case AT_INV_ATTACK_ADD_NATURE:
	case AT_INV_ATTACK_ADD_CURSE:
		{
			iValue = GetElementAbilValue(this, (eElementDmgType)(Type-30339), EGetAbilType::EGAT_INV);
		}break;
	case AT_INV_ATTACK_ADD_DESTROY:
		{
			iValue = GetElementAbilValue(this, ET_D_DESTROY, EGetAbilType::EGAT_INV);
		}break;
	case AT_INV_RESIST_ADD_FIRE://30381
	case AT_INV_RESIST_ADD_ICE:
	case AT_INV_RESIST_ADD_NATURE:
	case AT_INV_RESIST_ADD_CURSE:
		{
			iValue = GetElementAbilValue(this, (eElementDmgType)(Type-30369), EGetAbilType::EGAT_INV);
		}break;
	case AT_INV_RESIST_ADD_DESTROY:
		{
			iValue = GetElementAbilValue(this, ET_R_DESTROY, EGetAbilType::EGAT_INV);
		}break;
	//case AT_C_FINAL_HIT_SUCCESS_RATE:
	//case AT_C_FINAL_DODGE_SUCCESS_RATE:
	//case AT_C_FINAL_BLOCK_SUCCESS_RATE:
	//case AT_C_FINAL_CRITICAL_SUCCESS_RATE:
	//	{
	//		int const iCAbilType = GetCalculateAbil(Type);
	//		iValue = CAbilObject::GetAbil(iCAbilType) + m_kInv.GetAbil(iCAbilType) + m_kEffect.GetAbil(iCAbilType);
	//	}break;
	default:
		{
			//INFO_LOG(BM::LOG_LV0, _T("[%s] Not defined Ability [%hd], UnitType[%d], Guid[%s]"), __FUNCTIONW__, Type, UnitType(), GetID().str().c_str());
		}break;
	}

	if (iValue == 0)
	{
		// Effect/Inven �� CalculatedAbil�̿��� ���� �����Ѵٸ�, �Ʒ������ �ؾ� �Ѵ�.
		// iValue = CAbilObject::GetAbil(Type) + m_kInv.GetAbil(Type) + m_kEffect.GetAbil(Type);
		// Effect/Inven �� CalculatedAbil�� ������ ���� �̷��� ȣ���ص� �ȴ�.
		iValue = CAbilObject::GetAbil(Type);
	}
	if ( 0 == iValue && ::IsCalculatedAbil(Type))
	{
		// CalculatedAbil �̸鼭 ����0 ��� ����ؼ� ��� �´�.
		// �Լ� ����ο� �ִ� const ������ ������..
		//iValue = ((CUnit*)this)->CalculateAbil(Type);
		return ((CUnit*)this)->CalculateAbil(Type);
	}

	if( ::IsMonsterCardAbil(Type) )
	{
		iValue = CAbilObject::GetAbil(Type) + m_kInv.GetAbil(Type);
	}
	return iValue;
}

__int64 CUnit::GetAbil64(WORD const eAT)const
{
	__int64 i64Value = 0;
	switch(eAT)
	{
	case AT_MONEY:		{ i64Value = m_kInv.Money(); }break;
	case AT_REST_EXP_ADD_MAX:
	default:
		{
			i64Value = CAbilObject::GetAbil64(eAT);
		}break;
	}
	return i64Value;
}

bool CUnit::SetAbil64(WORD const Type, __int64 const iInValue, bool const bIsSend , bool const bBroadcast)
{	// �� �Լ����� �����;� CheckSetAbil(..) OnSetAbil(..) ���� �Լ��� ȣ��ȴ�.
	// ���ܿ����� ȣ�� �� �� �ִ� �ϰ��� �ִ� ȣ��ü�谡 �ʿ�
	__int64 iValue = iInValue;
	if ( !CheckSetAbil(Type, (int&)iValue) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	switch(Type)
	{
	case AT_MONEY:		{ m_kInv.Money(iValue); }break;
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
	case AT_TACTICS_EXP:
	case AT_CASH:
	case AT_BONUS_CASH:
	case AT_MILEAGE_TOTAL_CASH:
	case AT_MILEAGE_100:
	case AT_MILEAGE_1000:
	case AT_EXPERIENCE:
	case AT_REST_EXP_ADD_MAX:
	case AT_SKILL_LINKAGE_FLAG:
	case AT_TIME:	// PgPlayer���� AT_TIME�� SetAbil64�� ����Ͽ� Set�ϱ� ������ �߰�.
		{
			if( !CAbilObject::SetAbil64(Type, iValue))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Invalid Abil...AbilType[") << Type << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}

	DWORD dwSendFlag = 0;

	if(bIsSend){dwSendFlag |= E_SENDTYPE_SELF;}
	if(bBroadcast){dwSendFlag |= E_SENDTYPE_BROADCAST;}

	if(dwSendFlag)
	{
		SendAbil64((EAbilType)Type, dwSendFlag);
	}

	//	OnSetAbil(Type, iValue);
	return true;
}

bool CUnit::SetAbil(WORD const Type, int const iInValue, bool const bIsSend, bool const bBroadcast)
{
	// �� �Լ����� �����;� CheckSetAbil(..) OnSetAbil(..) ���� �Լ��� ȣ��ȴ�.
	// ���ܿ����� ȣ�� �� �� �ִ� �ϰ��� �ִ� ȣ��ü�谡 �ʿ�
	int iValue = iInValue;
	if ( !CheckSetAbil(Type, iValue) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	DWORD dwSendFlag = 0;
	if( bIsSend )
	{
		dwSendFlag |= E_SENDTYPE_SELF;
	}

	switch(Type)
	{
	case AT_LEVEL:					{ m_kCommon.sLevel = iValue; }break;
	case AT_CLASS:					{ m_kCommon.iClass = iValue; }break;
	case AT_GENDER:					{ m_kCommon.byGender = iValue; }break;
	case AT_RACE:					{ m_kCommon.iRace = iValue; }break;
	case AT_MAX_HP:					{ m_kBasic.iMaxHP = iValue; }break;
	case AT_HP_RECOVERY_INTERVAL:	{ m_kBasic.iHPRecoveryInterval = iValue; }break;
	case AT_HP_RECOVERY:			{ m_kBasic.sHPRecovery = iValue; }break;
	case AT_MAX_MP:					{ m_kBasic.iMaxMP = iValue; }break;
	case AT_MP_RECOVERY_INTERVAL:	{ m_kBasic.iMPRecoveryInterval = iValue; }break;
	case AT_MP_RECOVERY:			{ m_kBasic.sMPRecovery = iValue; }break;
	case AT_STR_BASIC:				{ m_kBasic.sInitStr = iValue; }break;
	case AT_INT_BASIC:				{ m_kBasic.sInitInt = iValue; }break;
	case AT_CON_BASIC:				{ m_kBasic.sInitCon = iValue; }break;
	case AT_DEX_BASIC:				{ m_kBasic.sInitDex = iValue; }break;
	case AT_MOVESPEED:				{ m_kBasic.sMoveSpeed = iValue; }break;
	case AT_PHY_DEFENCE:			{ m_kBasic.sPhysicsDefence = iValue; }break;
	case AT_MAGIC_DEFENCE:			{ m_kBasic.sMagicDefence = iValue; }break;
	case AT_ATTACK_SPEED:			{ m_kBasic.sAttackSpeed = iValue; }break;
	case AT_BLOCK_SUCCESS_VALUE:				{ m_kBasic.sBlockRate = iValue; }break;
	case AT_DODGE_SUCCESS_VALUE:				{ m_kBasic.sDodgeRate = iValue; }break;
	case AT_CRITICAL_SUCCESS_VALUE:			{ m_kBasic.sCriticalRate = iValue; }break;
	case AT_CRITICAL_POWER:			{ m_kBasic.sCriticalPower = iValue; }break;
	case AT_AI_TYPE:				{ m_kBasic.iAIType = iValue; }break;
//	case AT_INVEN_SIZE:				{ m_kBasic.sInvenSize = iValue; }break;
//	case AT_EQUIPS_SIZE:			{ m_kBasic.sEquipsSize = iValue; }break;
	case AT_STR:
	case AT_INT:
	case AT_DEX:
	case AT_CON:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Donot call SetAbil Abil=") << Type);
			return false;
		}break;
	case AT_FRAN:
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
	case AT_TACTICS_EXP:
	case AT_CASH:
	case AT_BONUS_CASH:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[") << __FL__ << _T("]  __int64 ó���ؾ� ��.") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	case AT_HP:
		{
			if ( this->IsDead() && iInValue > 0 )
			{
				// �׾������� HP�� ���� ������ ��
				this->SetState(US_IDLE);
			}
			if( 0 >= iInValue && 0 < GetAbil(AT_COMBAT_REVIVE_ADD_EFFECT_NO) )
			{
				BM::Stream kPacket(PT_U_G_NFY_COMBAT_REVIVE);
				VNotify(&kPacket);
				return true;
			}

			dwSendFlag |= E_SENDTYPE_SEND_BYFORCE;
		} //break; ���� ��� ����
	case AT_MP:
		{
			if ( GetAbil(AT_HPMP_BROADCAST) > 0 )
			{
				dwSendFlag |= E_SENDTYPE_BROADCAST_GROUND;
			}
		} //break; ���� ��� ����.
	default:
		{
			if( !CAbilObject::SetAbil(Type, iValue) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	if( bBroadcast )
	{
		dwSendFlag |= E_SENDTYPE_BROADCAST;
	}

	if( dwSendFlag & E_SENDTYPE_AREATYPE_BITMASK)
	{
		dwSendFlag |= E_SENDTYPE_MUSTSEND;
		SendAbil((EAbilType)Type, dwSendFlag);
	}
	
	OnSetAbil(Type, iValue);
	return true;
}


bool CUnit::OnSetAbil(WORD const Type, int const iValue)
{
	switch(Type)
	{
	case	AT_HP://HP 0 ���� �۰ų� ������.
		{
 			//if((0 >= iValue) && (HP_VALUE_KILL_NOTADDED_UNIT != iValue))
			if(0 >= iValue)
			{
				VOnDie();
			}
		}break;
	case AT_C_MAX_HP:
		{
			if ( 0 < iValue )
			{
				if ( GetAbil(AT_HP) > iValue )
				{
					SetAbil(AT_HP, iValue);
				}
			}
		}break;
	case AT_C_MAX_MP:
		{
			if ( 0 < iValue )
			{
				if ( GetAbil(AT_MP) > iValue )
				{
					SetAbil(AT_MP, iValue);
				}
			}
		}break;
// 	case AT_C_MAX_DP:
// 		{
// 			if ( 0 < iValue )
// 			{
// 				if ( GetAbil(AT_DP) > iValue )
// 				{
// 					SetAbil(AT_DP, iValue);
// 				}
// 			}
// 		}break;
	default:
		{
			if((AT_EQUIP_LEVELLIMIT_MIN	<= Type) && (Type <= AT_EQUIP_LEVELLIMIT_MAX))
			{
				InvenRefreshAbil();
			}
		}break;
	}
	return true;
}

void CUnit::Invalidate()
{
	Init();
}

bool CUnit::IsAlive()const
{
	if(m_State == US_DEAD)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iHP = GetAbil(AT_HP);
	if(0 < iHP)//HP �� ������ �������.
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CUnit::IsDead()const
{
	return !IsAlive();
}

int CUnit::AutoHeal( unsigned long ulElapsedTime, float const fMultiplier)
{
	if ( true == this->IsDead() )
	{
		return 0;
	}

	if ( 0.0f >= fMultiplier )
	{
		return 0;
	}

	int const iAutoHeal = GetAbil(AT_ENABLE_AUTOHEAL);
	
	WORD wAbil[3] = {0,};
	BYTE byNum = 0;

	if ( iAutoHeal & AUTOHEAL_HP )
	{
		m_dwHPElapsedTime += ulElapsedTime;
		int iInterval = GetAbil(AT_C_HP_RECOVERY_INTERVAL);
		if ((iInterval > 0) && ((int)m_dwHPElapsedTime > iInterval)  )
		{
			int iAmount = static_cast<int>(GetAbil(AT_C_HP_RECOVERY) * fMultiplier);

			m_dwHPElapsedTime = 0;
			// HP, MP, SP�� �÷�����...
			int const iMaxHP = GetAbil(AT_C_MAX_HP);
			int const iCurHP = GetAbil(AT_HP);
			int iNewValue = iCurHP;
			if(iMaxHP > iCurHP)
			{
				//iNewValue = __min(iCurHP + iAmount, int(iMaxHP*0.9));	// 90%���� ����
				iNewValue = std::min( iCurHP + iAmount, iMaxHP );	// 100%���� ����

				if (iNewValue > iCurHP)
				{
					wAbil[byNum++] = AT_HP;
					SetAbil( AT_HP, iNewValue);

					if(ESHR_PARENT==GetAbil(AT_SHARE_HP_RELATIONSHIP))
					{
						BM::Stream kDPacket(PT_U_G_NFY_SHARE_HP_SET);
						kDPacket.Push(iNewValue);
						VNotify(&kDPacket);
					}
					//INFO_LOG(BM::LOG_LV8, _T("[%s] MaxHP:%d, NewHP:%d, OldHP:%d"), __FUNCTIONW__, iMaxHP, iNewValue, iCurHP);
				}
			}
		}
	}

	if ( iAutoHeal & AUTOHEAL_MP )
	{
		m_dwMPElapsedTime += ulElapsedTime;
		int iInterval = GetAbil(AT_C_MP_RECOVERY_INTERVAL);
		if (iInterval > 0 && ((int)m_dwMPElapsedTime > iInterval))
		{
			int const iAmount = static_cast<int>(GetAbil(AT_C_MP_RECOVERY) * fMultiplier);

			m_dwMPElapsedTime = 0;
			int const iMaxMP = GetAbil(AT_C_MAX_MP);
			int const iCurMP = GetAbil(AT_MP);
			int iNewValue = iCurMP;
			if(iMaxMP > iCurMP)
			{
				iNewValue = std::min( iCurMP + iAmount, iMaxMP );
				if (iNewValue > iCurMP)
				{
					wAbil[byNum++] = AT_MP;
					SetAbil(AT_MP, iNewValue);
				}
			}
		}
	}

	if ( byNum > 0 )
	{
		DWORD dwFlag = E_SENDTYPE_SELF;
		if ( GetAbil(AT_TEAM) > 0 )
		{// ���� ���� ��쿡�� ������ BroadCast������.
			dwFlag|= E_SENDTYPE_BROADCAST_GROUND;
		}
		this->SendAbiles( wAbil, byNum, dwFlag );
		return 1;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int CUnit::GetGroggyRecovery()const
{
	switch(GetAbil(AT_ELITEPATTEN_STATE))
	{
	case EPS_FRENZY:
		{
			return GetAbil(AT_FRENZY_RECOVERY);
		}break;
	case EPS_GROGGY:
		{
			return GetAbil(AT_GROGGY_RECOVERY);
		}break;
	}

	return 0;
}

float GetGroggyPointRate(CUnit const * pkUnit)
{
	int iRate = 0;
	if( pkUnit && (iRate = pkUnit->GetAbil(AT_C_DEC_GROGGYPOINT_RATE)))
	{
		switch(pkUnit->GetAbil(AT_ELITEPATTEN_STATE))
		{
		case EPS_FRENZY:
			{
				return iRate/ABILITY_RATE_VALUE_FLOAT;
			}break;
		}
	}

	return 1.f;
}

int CUnit::GetGroggyRecoveryValue(float const fMultiplier)const
{
	float const fRate = -GetGroggyPointRate(this);
	return static_cast<int>(GetGroggyRecovery() * fMultiplier * fRate);
}

int CUnit::AutoGroggy(unsigned long ulElapsedTime, float const fMultiplier)
{
	if ( true == this->IsDead() )
	{
		return 0;
	}

	if ( 0.0f >= fMultiplier )
	{
		return 0;
	}

	if ( 0 == GetAbil(AT_GROGGY_MAX) )
	{
		return 0;
	}

	int const iInterval = GetAbil(AT_C_GROGGY_RECOVERY_INTERVAL);
	int const iAmount = GetGroggyRecoveryValue(fMultiplier);	
	if(iInterval<=0 || 0==iAmount)
	{
		m_dwGroggyElapsedTime = 0;
		return 0;
	}

	m_dwGroggyElapsedTime += ulElapsedTime;
	int const iMaxGroggy = GetAbil(AT_GROGGY_MAX);
	while( m_dwGroggyElapsedTime > iInterval )
	{
		int const iCurGroggy = GetAbil(AT_GROGGY_NOW);
		
		int iNewValue = std::max<int>(iCurGroggy+iAmount, 0);
		iNewValue = std::min<int>(iNewValue, iMaxGroggy);
		if(iCurGroggy!=iNewValue)
		{
			SetAbil(AT_GROGGY_NOW, iNewValue);
			m_dwGroggyElapsedTime = std::max<int>(m_dwGroggyElapsedTime-iInterval,0);
		}
		else
		{
			m_dwGroggyElapsedTime = 0;
		}
	}

	return 0;
}

void CUnit::UpdateGroggyRelaxTime(DWORD const dwElapsedTime)
{	
	int const iRelaxTime = GetAbil(AT_GROGGY_RELAX_TIME);
	if( iRelaxTime )
	{
		int const iNowTime = std::max<int>(iRelaxTime-dwElapsedTime,0);
		if( 0>=iNowTime )
		{
			SetAbil(AT_DEC_GROGGYPOINT_RATE, 0);
			NftChangedAbil(AT_DEC_GROGGYPOINT_RATE, E_SENDTYPE_BROADALL);
		}
		SetAbil(AT_GROGGY_RELAX_TIME, iNowTime);
	}
}

int CUnit::GetDetectRange()const
{
	return GetAbil(AT_DETECT_RANGE) * ((ABILITY_RATE_VALUE+GetAbil(AT_DETECT_RANGE_RATE))/ABILITY_RATE_VALUE_FLOAT);
}

void CUnit::SetTargetGroundNo(int const iGroundNo)
{
	m_kEffect.TargetGroundNo(iGroundNo);
}

int CUnit::UpdateStandUpTime( unsigned long ulElapsedTime )
{
	if( this->IsAlive() )
	{
		if(0<StandUpTime())
		{
			StandUpTime(StandUpTime()-ulElapsedTime);
			if(StandUpTime()<=0)//ƽ�� ��������
			{
				StandUpTime(0);
				SetAbil(AT_POSTURE_STATE,0);
			}
		}
		else if(GetAbil(AT_POSTURE_STATE))
		{
			SetAbil(AT_POSTURE_STATE,0);
		}
	
		return 1;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

/*
TCHAR const* CUnit::GetName()const
{
	return m_chName;
}

void CUnit::SetName(LPCTSTR lpszName)
{
	_tcscpy_s(m_chName, MAX_NAME_LEN, lpszName);
}
*/
/*
bool CUnit::SetSkill(WORD const wSkillNo)
{
	CSkillDef* pkSkillDef = const_cast<CSkillDef*>(g_SkillDefMgr.GetDef(wSkillNo));
//	if ( pkSkillDef == NULL )
//		return false ;

	if ( m_Skill.SetSkillDef(pkSkillDef) )
		return true ;

	return false ;
}
*/

CEffect const *CUnit::FindEffect(int iEffectNo)const
{
	return m_kEffect.FindEffect(iEffectNo);
}

CEffect* CUnit::GetEffect(int iEffectNo, bool bInGroup)const
{
	return m_kEffect.FindInGroup(iEffectNo, bInGroup);
}

CEffect* CUnit::GetItemTypeEffect(int iEffectNo, bool bInGroup)
{
	return m_kEffect.FindInGroupItemEffect(iEffectNo, bInGroup);
}
bool CUnit::IsItemEffect(int iEffectNo, bool bInGroup)
{
	return m_kEffect.FindInGroupItem(iEffectNo, bInGroup);
}

CEffect* CUnit::AddEffect(int const iEffectNo, int const iValue, SActArg const* pkAct, CUnit const* pkCaster, BYTE btEffectType)
{
	if(0 >= iEffectNo
		|| UT_SUB_PLAYER == UnitType()	// ����ĳ���ʹ� ����Ʈ �ɸ� ���� ����
		)
	{
		return NULL;
	}

	if(EFFECT_TYPE_NONE == btEffectType)
	{
		btEffectType = EFFECT_TYPE_NORMAL;
	}

	SEffectCreateInfo kCreate;
	kCreate.eType = static_cast<EEffectType>(btEffectType);
	kCreate.iEffectNum = iEffectNo;
	kCreate.iValue = iValue;
	if (pkAct != NULL)
	{
		kCreate.kActArg = *pkAct;
		kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
	}
	else
	{
		kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyClient;
	}
	if(pkCaster)
	{// �����ڸ� ������(effectfuncion���� Begin�ÿ� caster�� �������� �ʿ�)
		kCreate.kActArg.Set(ACTARG_CUSTOM_UNIT1, pkCaster->GetID());
		kCreate.kActArg.Set(ACTARG_CUSTOM_UNIT1_TYPE, pkCaster->UnitType());
	}
	
	CEffect* pkEffect = AddEffect(kCreate);
	if (pkEffect)
	{
		if (pkCaster)
		{	
			if(!pkEffect->IsCasterEx(pkCaster->GetID()))
			{
				pkEffect->AddCasterEx(pkCaster->GetID());
			}

			pkEffect->SetCaster(pkCaster->GetID());

			::CheckSkillFilter_Delete_Effect(pkCaster, this, iEffectNo);
		}
		return pkEffect;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

// Client������ pkArg==NULL �̴�
//CEffect* CUnit::AddEffect(BYTE byType, int const iEffectNo, int iValue, SActArg* pkArg, unsigned long ulElapsedTime, unsigned long ulInterval)
CEffect* CUnit::AddEffect(SEffectCreateInfo& rkCreate)
{
	if(0 >= rkCreate.iEffectNum
		|| UT_SUB_PLAYER == UnitType()	// ����ĳ���ʹ� ����Ʈ �ɸ� ���� ����
		)
	{
		return NULL;
	}

	BM::GUID kGuid;
	EUnitType eUnitType = UT_NONETYPE;
	if (S_OK != rkCreate.kActArg.Get(ACTARG_CUSTOM_UNIT1, kGuid))
	{
		kGuid = BM::GUID::NullData();
	}
	rkCreate.kActArg.Get(ACTARG_CUSTOM_UNIT1_TYPE, eUnitType);

	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(rkCreate.iEffectNum);
	if (NULL == pkEffectDef)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" EffectDef Cannot find EffectNo[") << rkCreate.iEffectNum << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	//
	if(IsUnitType(UT_MONSTER) && UT_PLAYER==eUnitType)
	{
		if(0x01 & GetAbil(AT_UNABLE_EFFECT_PC))
		{
			if(EFFECT_TYPE_BLESSED==pkEffectDef->GetType() && pkEffectDef->GetDurationTime()>0 && pkEffectDef->GetInterval()==0)
			{
				return NULL;
			}
		}
		if(0x02 & GetAbil(AT_UNABLE_EFFECT_PC))
		{
			if(EFFECT_TYPE_CURSED==pkEffectDef->GetType() && pkEffectDef->GetDurationTime()>0 && pkEffectDef->GetInterval()==0)
			{
				return NULL;
			}
		}
		if(0x01 & GetAbil(AT_UNABLE_EFFECT_PC_INTERVAL))
		{
			if(EFFECT_TYPE_BLESSED==pkEffectDef->GetType() && pkEffectDef->GetDurationTime()>0 && pkEffectDef->GetInterval()>0)
			{
				return NULL;
			}
		}
		if(0x02 & GetAbil(AT_UNABLE_EFFECT_PC_INTERVAL))
		{
			if(EFFECT_TYPE_CURSED==pkEffectDef->GetType() && pkEffectDef->GetDurationTime()>0 && pkEffectDef->GetInterval()>0)
			{
				return NULL;
			}
		}
	}
	
	{// �鿪 üũ�� �ؾ� �ϴ� ����Ʈ �ΰ�?
		int iImmunityCheckAbilNo =  pkEffectDef->GetAbil(AT_CHECK_IMMUNITY_TYPE);
		if(0 < iImmunityCheckAbilNo)
		{// �鿪 üũ�ؾ��� ����� �ִٸ�
			iImmunityCheckAbilNo = ((iImmunityCheckAbilNo/10)*10)+3;		// _C_ ���� ��� ��� ��ȣ�� �����ְ�(�����ڸ��� 3�� ���)
			int const iImmunityRate = this->GetAbil(iImmunityCheckAbilNo);	// �鿪 Ȯ���� ����ؼ� 
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
			if(iRandValue <= iImmunityRate)
			{// �鿪 ó�� �ؾ��ϰ�
				int const iAddEffectNo = pkEffectDef->GetAbil(AT_ADD_EFFECT_WHEN_SUCCESS_IMMUNITY);
				if(iAddEffectNo)
				{// �����Ҷ� �ɾ��� ����Ʈ�� �ִٸ�(�ð����� ȿ���� ���� ����)
					// ����Ʈ�� �ɾ��ش�
					SActArg kArg = rkCreate.kActArg;
					this->AddEffect(iAddEffectNo, 0, &kArg, this);
				}
				return NULL;
			}
		}
	}

	int iUnitGrade = GetAbil(AT_GRADE);
	if (0 < iUnitGrade && ((pkEffectDef->GetAbil(AT_EXCEPT_GRADE) & (1<<iUnitGrade)) != 0))
	{
		// � Effect�� � Unit�� ���ؼ� ����Ǹ� �ȵǴ� ��찡 �ִ�
		// �� : STUN ���� Effect�� ����/���� ���Ϳ��� ������� �ʵ��� ����.
		//INFO_LOG( BM::LOG_LV7, __FL__ << _T(" AddEffect failed UnitType[") << static_cast<int>(UnitType()) << _T("],ClassNo[") << GetAbil(AT_CLASS)
		//	<< _T("],Grade[") << iUnitGrade << _T("],Name[") << Name().c_str() << _T("]") );
		//LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	else if( pkEffectDef->GetType() )
	{
		if(GetAbil(AT_EXCEPT_EFFECT_TYPE)&(1<<pkEffectDef->GetType()))	//�� ��� Ÿ���� �Ȱɸ���
		{
			return NULL;
		}
	}

	EAddEffectResult eResult;
	CEffect* pkNew = m_kEffect.AddEffect(rkCreate, eResult);
	if(NULL == pkNew)
	{
		return NULL;
	}
	// ������ ���� ������ Effect�� �ִ°� Ȯ���� ����
	if (rkCreate.eOption & SEffectCreateInfo::ECreateOption_CallbyServer)
	{
		int iDelEffect = 0;
		rkCreate.kActArg.Get(ACTARG_DEL_EFFECT, iDelEffect);
		rkCreate.kActArg.Remove(ACTARG_DEL_EFFECT);
		if (iDelEffect > 0)
		{
			if (rkCreate.iEffectNum == iDelEffect)
			{
				CAUTION_LOG(BM::LOG_LV3, __FL__ << _T("CANNOT DELETE because of NewEffectNo == DeleteEffectNo, EffectNo=") << iDelEffect);
			}
			else
			{
				BM::Stream kDPacket(PT_U_G_NFY_DELETE_EFFECT);
				kDPacket.Push(iDelEffect);
				kDPacket.Push((bool)true);
				kDPacket.Push(E_EDC_DELETE_BY_HIGH_LEVEL);
				VNotify(&kDPacket);
				GetSkill()->EffectFilter(false, iDelEffect);
				
				m_kEffect.DeleteEffect(iDelEffect);
			}
		}

		//�����ð� ����
		int iAddDuration = 0;

		if( EFFECT_TYPE_CURSED==pkEffectDef->GetType() )
		{
			iAddDuration += GetAbil(AT_ADD_CURSED_DURATIONTIME);
		}
		else
		{
			iAddDuration += GetAbil(AT_ADD_BLESSED_DURATIONTIME);
		}

		// �̸� Def�� ���ǵ� �� �ð��� �ջ��� �����Ǵ� �ð��� ����� �ְ�
		int iCheckTime = pkEffectDef->GetDurationTime() + iAddDuration;
		if(0 < pkEffectDef->GetDurationTime()
			&& 0 >= iCheckTime
			)
		{// �����ð��� 0���ϰ� �Ǹ� ����Ʈ�� ���� �ʴ´�
			return NULL;
		}

		if(0 != iAddDuration)
		{// �ɸ��ٸ� �߰����ش�
			pkNew->SetAddDurationTime(iAddDuration);
			pkNew->SetEndTime(pkNew->GetStartTime()+pkNew->GetDurationTime());
		}

		//if (pkNew->GetAbil(AT_TYPE) != EFFECT_TYPE_PASSIVE)
		{
			// EFFECT_TYPE_PASSIVE : Passive�� ���� ������ Effect�̸� Client�� ������ �ʿ䰡 ����.
			switch(pkNew->GetAbil(AT_TYPE))
			{
			case EFFECT_TYPE_PASSIVE:
				{
				}break;
			default:
				{
					eResult |= E_AERESULT_SENDPACKET;
				}break;
			}
			//			eResult = (EAddEffectResult)((pkNew->GetAbil(AT_TYPE) == EFFECT_TYPE_PASSIVE) ? eResult : eResult|E_AERESULT_SENDPACKET);
			int iActionInstanceID = 0;//�����Ⱚ ����

			//Restart �ÿ� Ŭ��� ������ �ʴ� ����϶��� �Ⱥ�����.
			if ( true == this->CanNfyAddEffect() )
			{
				if(0 == (eResult & E_AERESULT_RESTART_NOT_SEND_CLIENT))
				{
					rkCreate.kActArg.Get(ACTARG_ACTIONINSTANCEID, iActionInstanceID);
					BM::Stream kAPacket(PT_U_G_NFY_ADD_EFFECT);
					kAPacket.Push(pkNew->GetKey());
					kAPacket.Push(pkNew->GetEffectNo());
					kAPacket.Push(kGuid);
					kAPacket.Push(iActionInstanceID);
					kAPacket.Push(rkCreate.iValue);
					kAPacket.Push(eResult);
					rkCreate.kActArg.WriteToPacket(kAPacket);
					VNotify(&kAPacket);
				}
			}
		}
	}

	SetOnDamageDelEffect(pkNew);
	GetSkill()->EffectFilter(true, rkCreate.iEffectNum);
	return pkNew;
}

bool CUnit::DeleteEffect(int iEffectKey, bool bInGroup)
{
	if(0==iEffectKey)
	{
		return false;
	}

	// Packet�� ���� ������ Effect�� ������ �Ѵ�.
	// ���� : Effect_EndXXXX() Lua �Լ� �ȿ��� CEffect ��ü�� �ʿ��ϴ�.
	CEffect* pkEffect = m_kEffect.FindInGroup(iEffectKey, bInGroup);
	if (pkEffect != NULL)
	{
		pkEffect->DeleteCasterExFirst();
		if(pkEffect->GetCasterExSize())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		iEffectKey = pkEffect->GetKey();

		// EFFECT_TYPE_PASSIVE : Passive�� ���� ������ Effect�̸� Client�� ������ �ʿ䰡 ����.
		BM::Stream kDPacket(PT_U_G_NFY_DELETE_EFFECT);
		// Create Packet -> To Ground -> Create Packet2 -> To Client

		bool bSendNft = true;
		switch(pkEffect->GetAbil(AT_TYPE))
		{
		case EFFECT_TYPE_PASSIVE:
			{
				bSendNft = false;
				kDPacket.Push(iEffectKey);
			}break;
		case EFFECT_TYPE_ITEM:
			{
				kDPacket.Push(iEffectKey);
			}break;
		default:
			{
				kDPacket.Push(iEffectKey);
			}break;
		}

		//����Ʈ�� ���� ����Ʈ ��ȣ�� ���� �����Ѵ�.
		iEffectKey = pkEffect->GetEffectNo();
		
		kDPacket.Push(bSendNft);
		VNotify(&kDPacket);
	}

	GetSkill()->EffectFilter(false, iEffectKey);
	return m_kEffect.DeleteEffect(iEffectKey);
}

bool CUnit::DeleteBuffEffect(int iEffectKey, bool bInGroup)
{// ���� ��û���� ����, ������ ������ ����
	// �Ϲݹ��� ���� ã��,
	CEffect* pkEffect = m_kEffect.FindInGroup(iEffectKey, bInGroup);
	if(!pkEffect)
	{// ���������� ���� ���� �̸�
		int const& riItemEffectNo = iEffectKey;
		pkEffect = m_kEffect.FindInGroupItemEffectByEffectNo(riItemEffectNo,bInGroup);
		if(!pkEffect)
		{
			return false;
		}
		CItemEffect* pkItemEffect = dynamic_cast<CItemEffect*>(pkEffect);
		if(!pkItemEffect)
		{
			return false;
		}
		// ������ ��ȣ�� EffectMap�� Key �̹Ƿ� �����ϰ�, 
		iEffectKey = pkItemEffect->ItemNo();
	}
	if(0 < pkEffect->GetAbil(AT_IGNORE_REQ_USER_DEL_EFFECT))
	{// ���� �� �� ���� ����� ���� Effect
		return false;
	}
	switch(pkEffect->GetAbil(AT_TYPE))
	{
	case EFFECT_TYPE_ITEM:
	case EFFECT_TYPE_BLESSED:
		{// �Ϲ� ����, ������ ������ ���� ��û��
			return DeleteEffect(iEffectKey, bInGroup);
		}break;
	default:
		{// �����, �г�Ƽ�� ������ ���� �Ҽ� ����.
			return false;
		}break;
	}
	return false;
}

void CUnit::SetOnDamageDelEffect(CEffect const* pkEffect)
{
	if( pkEffect && pkEffect->GetAbil(AT_ONDAMAGE_DEL_EFFECT))
	{
		m_kContOnDamageDelEffect.push_back(pkEffect->GetEffectNo());
	}
}

void CUnit::OnDamageDelEffect()
{
	if(false==m_kContOnDamageDelEffect.empty())
	{
		for(VEC_INT::const_iterator c_it=m_kContOnDamageDelEffect.begin(); c_it!=m_kContOnDamageDelEffect.end(); ++c_it)
		{
			DeleteEffect(*c_it);
		}
		m_kContOnDamageDelEffect.clear();
	}
}

/*
void CUnit::WriteToPacketFromEffect(BM::Stream &rPacket, bool const bIsSimple)const
{
	EffectMap::const_iterator itor = m_kEffectMap.begin();

	size_t const effect_count = m_kEffectMap.size();
	rPacket.Push(effect_count);

	while (m_kEffectMap.end() != itor)
	{
		rPacket.Push((*itor).second->GetType());
		rPacket.Push((*itor).second->GetEffectNo());
		rPacket.Push((*itor).second->GetValue());
		if(!bIsSimple)
		{
			rPacket.Push((*itor).second->GetTime());
			rPacket.Push((*itor).second->GetInterval());
		}
		itor++;
	}
}

void CUnit::ReadFromPacketToEffect(BM::Stream &rPacket, bool const bIsSimple)
{
	size_t effect_count = 0;

	int iEffectNo = 0, iEffectValue = 0;
	unsigned long lTime = 0, lInterval = 0;
	BYTE byType;

	rPacket.Pop(effect_count);
	
	while(effect_count--)
	{
		rPacket.Pop(byType);
		rPacket.Pop(iEffectNo);
		rPacket.Pop(iEffectValue);
		if(!bIsSimple)
		{
			rPacket.Pop(lTime);
			rPacket.Pop(lInterval);
		}
		AddEffect(byType, iEffectNo, iEffectValue, NULL, lTime, lInterval);
	}
}
*/

void CUnit::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	rkPacket.Push(UnitType());	//-> Recv_PT_M_C_ADD_UNIT �������� �� �տ��ִٰ� �����ϰ� �����Ƿ� ���� �ٲ��� ����.
	rkPacket.Push(GetID());	//-�̰� �ι�° ��� ������;
	rkPacket.Push(kWriteType);
	rkPacket.Push(GetAbil(AT_CLASS));

	if ( WT_EFFECT_FLAG & kWriteType )
	{
		m_kEffect.WriteToPacket(rkPacket, 0 != (WT_SIMPLE&kWriteType));

		int const iCurrentMoveSpeed = GetAbil(AT_C_MOVESPEED);
		int const iCurrentJumpHeight = GetAbil(AT_C_JUMP_HEIGHT);
		rkPacket.Push(iCurrentMoveSpeed);
		rkPacket.Push(iCurrentJumpHeight);
	}

	rkPacket.Push(GetRandomSeed());
	rkPacket.Push(m_kFrontDirection);
}

void CUnit::EffectNftChangeAbil()
{
	SAbilIterator kItor;
	m_kEffect.FirstAbil(&kItor);
	while ( m_kEffect.NextAbil(&kItor) )
	{
		NftChangedAbil(kItor.wType);
	}
}

EWRITETYPE CUnit::ReadFromPacket(BM::Stream &rkPacket)
{
	EUnitType kUnitType;
	EWRITETYPE kWriteType;

	rkPacket.Pop(kUnitType);	//-> Recv_PT_M_C_ADD_UNIT �������� �� �տ��ִٰ� �����ϰ� �����Ƿ� ���� �ٲ��� ����.
	rkPacket.Pop(m_kGuid);		//-�̰� �ι�° ��� ������;.
	rkPacket.Pop(kWriteType);	//
	rkPacket.Pop(m_kCommon.iClass);
	if( 0 != (kWriteType&(WT_DEFAULT|WT_SIMPLE)) )
	{
		m_kEffect.ReadFromPacket(rkPacket, 0 != (kWriteType&WT_SIMPLE));
		// Effect �ɷ�ġ �����ϱ�
		//DoEffect();
		int iCurrentMoveSpeed = 0;
		int iCurrentJumpHeight = 0;
		rkPacket.Pop(iCurrentMoveSpeed);
		rkPacket.Pop(iCurrentJumpHeight);
		SetAbil(AT_C_MOVESPEED, iCurrentMoveSpeed);
		SetAbil(AT_C_JUMP_HEIGHT, iCurrentJumpHeight);
	}

	DWORD dwRandomSeed = 0;
	rkPacket.Pop(dwRandomSeed);
	rkPacket.Pop(m_kFrontDirection);
	SetRandomSeed(dwRandomSeed);

	return kWriteType;
}

int CUnit::GetActionInstanceID()
{
	if (m_iActionInstanceID > 1000000)
	{
		m_iActionInstanceID = 1;
	}
	return m_iActionInstanceID++;
}

void CUnit::SetActionInstanceID(int const iActionID)
{
	m_iActionInstanceID = iActionID;
}

DWORD CUnit::GetRandom()
{
	if(UseRandomSeedType())
	{
		++m_kRandomSeedCallCounter;
		//{ BM::vstring vStr(m_kRandomSeedCallCounter); vStr+="\n"; OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
		return static_cast<DWORD>(m_kRandom.Integer());
	}
	else
	{
		return static_cast<DWORD>(BM::Rand_Index(100000));
	}
}

void CUnit::SetRandomSeed(DWORD dwSeed)
{
	m_dwRandomSeed = dwSeed;
	m_kRandom.Reseed(m_dwRandomSeed);
}

DWORD CUnit::GetRandomSeed() const
{	
	return m_dwRandomSeed;
}


void CUnit::SetBlowAttacker(BM::GUID guidAttacker)
{
	if( guidAttacker != BM::GUID::NullData() )
	{
		m_kAI.SetEvent(guidAttacker, EAI_EVENT_BLOWUP);
	}
	m_guidBlowAttacker = guidAttacker ;
}

bool CUnit::IsState(int iCheckState)const
{
	int iState = GetState();
	return ((iState & iCheckState) == iCheckState) ? true : false;
}

void CUnit::SetDamageAction(bool const bAction)
{
	m_bDamageAction = bAction;
}

bool CUnit::IsDamageAction()const
{
	return m_bDamageAction;
}

bool CUnit::OnDamage( int& iPower, int& iHP )
{	
	iHP = GetAbil(AT_HP);
	if ( iHP <= 0 )
	{
		// �̹� �׾��ִ� �����̸� �ƹ��͵� ���� �ʴ´�.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( 0 < iPower )	// POWER ���� ������ ��� ���� �Ǹ� DP ���� ������ ���� �� �� �ִ�. (Power�� �����ΰͺ��� �����̱�� ������)
	{
		iPower = std::min( iHP, iPower );
		iHP -= iPower;
	}

	m_kEffect.Damaged( true );
	return true;
}

void CUnit::VOnDie()
{
	SetState(US_DEAD);// �״� �̺�Ʈ �߻� ���ڸ��� �׾����.

	DWORD dwCurTime = BM::GetTime32();
	DeathTime( ((0==dwCurTime) ? 1 : dwCurTime) );	

	BM::Stream kPacket(PT_UNIT_NFY_ON_DIE);
	VNotify(&kPacket);
}

void CUnit::VOnLvUp()
{
	switch( UnitType() )
	{
	case UT_PLAYER:
	case UT_PET:
		{
			BM::Stream kNfyPacket(PT_U_G_NFY_ABIL_CHANGE, static_cast<WORD>(AT_LEVEL) );
			VNotify(&kNfyPacket);
		}break;
	default:
		{

		}break;
	}
}
void CUnit::VOnChangeClass()
{
	switch( UnitType() )
	{
	case UT_PLAYER:
	case UT_PET:
		{
			BM::Stream kNfyPacket(PT_U_G_NFY_ABIL_CHANGE, static_cast<WORD>(AT_CLASS) );
			VNotify(&kNfyPacket);
		}break;
	default:
		{

		}break;
	}
}

void CUnit::VOnRefreshAbil()
{
	/*
	WORD const wType[] = { AT_MAX_HP, AT_MAX_MP, AT_HP, AT_MP, AT_STR
						, AT_INT, AT_CON, AT_DEX, AT_SP, AT_PHY_DMG_DEC
						, AT_MAGIC_DMG_DEC, AT_LEVEL, AT_CLASS, AT_BATTLE_LEVEL };
						//1. �ݵ�� Class���� Level�� ���� �־���� �Ѵ�.
						//2. �ݵ�� Level�� Class�� �ǵڿ� �־��ش�(�׷��� Ŭ�󿡼� �������� �ľ� �� �� �ִ�.
	SendAbiles(wType, 14, E_SENDTYPE_BROADALL);//������/������ ������ ��ο��� �����־��
	*/
	WORD const wType[] = {	AT_LEVEL, AT_CLASS, AT_BATTLE_LEVEL, AT_SP, AT_PHY_DMG_DEC
						,	AT_MAGIC_DMG_DEC };
	//1. �ݵ�� Class���� Level�� ���� �־���� �Ѵ�.
	SendAbiles(wType, 6, E_SENDTYPE_BROADALL);//������/������ ������ ��ο��� �����־��
}

void CUnit::VOnChangeTactics()
{
	bool const bIsPlayer = IsUnitType(UT_PLAYER);
	if( bIsPlayer )
	{
		BM::Stream kNfyPacket(PT_U_G_NFY_ABIL_CHANGE, (WORD)AT_TACTICS_LEVEL);
		VNotify(&kNfyPacket);
	}
}

bool CUnit::CheckSetAbil(WORD const Type, int& iValue)
{
	switch(Type)
	{
	case 0:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	case AT_C_MOVESPEED:
		{
			if (GetAbil(AT_FIXED_MOVESPEED) > 0)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	case AT_HP:
		{
			switch (iValue)
			{
			case HP_VALUE_MISSION_MAP_PENALTY:
				{
					// Minus���� ����Ѵ�.
				}break;;
			default:
				{
					int const iGmCmdLimitHP = GetAbil(AT_GMCMD_LIMIT_HP);
					if( 0 < iGmCmdLimitHP )
					{
						iValue = std::max(iGmCmdLimitHP, iValue);
						iValue = std::min(GetAbil(AT_MAX_HP), iValue);
					}
					else
					{
						iValue = __max(0, iValue);
					}
				}break;
			}
		}break;
	case AT_MP:
		{
			switch (iValue)
			{
			case HP_VALUE_MISSION_MAP_PENALTY:
				{
					// Minus���� ����Ѵ�.
				}break;;
			default:
				{
					iValue = __max(0, iValue);
				}break;
			}
		}break;
	case AT_ADD_EXP_CAN_DUPLICATE:
	case AT_ADD_EXP_PER:
		{
			// AT_ADDEXP_FINAL ���� GetAbil(AT_ADD_EXP_PER)+GetAbil(AT_ADD_EXP_CAN_DUPLICATE) �̹Ƿ�
			//	���� �ʱ�ȭ �Ͽ� �ٽ� ���ǵ��� �Ѵ�.
			CAbilObject::SetAbil(AT_ADDEXP_FINAL, 0);
		}break;
	case AT_STR_BASIC:
	case AT_STR_ADD: { CAbilObject::SetAbil(AT_C_STR, 0); }break;
	case AT_INT_BASIC:
	case AT_INT_ADD: { CAbilObject::SetAbil(AT_C_INT, 0); }break;
	case AT_CON_BASIC:
	case AT_CON_ADD: { CAbilObject::SetAbil(AT_C_CON, 0); }break;
	case AT_DEX_BASIC:
	case AT_DEX_ADD: { CAbilObject::SetAbil(AT_C_DEX, 0); }break;
	}
	return true;
}

void CUnit::SetDelay(int const iDelay)
{
	m_iDelay = iDelay;
}

/*
bool CUnit::IsEnemy(CUnit* pkTarget, bool bCheckTargetValid, BYTE byTargetType)
{
	if (pkTarget == NULL || pkTarget == this)
	{
		return false;
	}
	EUnitType eOther = pkTarget->UnitType();
	bool bEnemy = false;
	switch(eOther)
	{
	case UT_NPC:
	case UT_PET:
		break;
	case UT_MONSTER:
	case UT_BOSSMONSTER:
		if (IsUnitType(UT_PLAYER) || IsUnitType(UT_ENTITY))
		{
			bEnemy = true;
		}
		break;
	case UT_PLAYER:
	case UT_ENTITY:
		if (IsUnitType(UT_MONSTER))
		{
			bEnemy = true;
		}
		else if (IsUnitType(UT_PLAYER) || IsUnitType(UT_ENTITY))
		{
			bEnemy = (GetAbil(AT_TEAM) == pkTarget->GetAbil(AT_TEAM)) ? false : true;
		}
		break;
	}
	if (bCheckTargetValid && bEnemy)
	{
		bEnemy = pkTarget->CanbeTarget();
		if (bEnemy)
		{
			// �ֱٿ� �̹� Damage�� �¾Ҵٸ�, �ʹ� ���� ������ ���� �ʵ��� �Ѵ�.
			BM::GUID const& rkTargetGuid = pkTarget->GetID();
			DequeTarget::const_iterator itor = m_kTargetDeque.begin();
			while (itor != m_kTargetDeque.end())
			{
				if (itor->kGuid == rkTargetGuid)
				{
					DWORD dwNow = BM::GetTime32();
					bEnemy = (itor->dwDmgTime+500) < dwNow;	// 500ms ������ Dmg �޾Ҵ°�?
					//INFO_LOG(BM::LOG_LV9, _T("[%s] Target cannot be enemy DmgTime[%ud], Now[%ud]"), __FUNCTIONW__, itor->dwDmgTime, dwNow);
					break;
				}
				++itor;
			}
			}
	}

	return bEnemy;
}
*/

bool CUnit::IsTarget(CUnit* pkTarget, bool bCheckTargetValid, int iSkillTargetType, CSkillDef const* pkSkillDef, int iDmgCheckTime)
{
	if(0 > iDmgCheckTime)
	{
		iDmgCheckTime = DEFAULT_DMG_CHECK_TIME;
	}

	if (pkTarget == NULL || pkTarget->GetAbil(AT_HP) <= 0)	// �׾��ٸ� �ϴ� false
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if (pkSkillDef && pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE) && 0!=pkTarget->GetAbil(AT_CANNOT_DAMAGE))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( this == pkTarget && ((iSkillTargetType & ESTARGET_SELF) == 0) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( iSkillTargetType & ESTARGET_CASTER )
	{
		if( Caller() != pkTarget->GetID() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if ( iSkillTargetType & ESTARGET_SUMMONED )
	{
		if( false==pkTarget->IsUnitType(UT_SUMMONED) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if(pkTarget->Caller() != GetID())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if(IsUnitType(UT_ENTITY) && ENTITY_GUARDIAN==GetAbil(AT_ENTITY_TYPE))
	{
		if(pkTarget->GetAbil(AT_CANNOT_DAMAGE_BY_GUARDIAN))
		{
			return false;
		}
	}

	//Entity�� Entity�� ��ȯ�ϴ� ��� �÷��̾ ������ �޾�
	//AT_CALLER_TYPE���� AT_OWNER_TYPE���� ����
	int const iMyUnitType = GetAbil(AT_OWNER_TYPE);
	int const iTargetUnitType = pkTarget->GetAbil(AT_OWNER_TYPE);
	if(0==iMyUnitType || 0==iTargetUnitType)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" AT_OWNER_TYPE NULL MyUnitType[") << iMyUnitType << _T("] TargetUnitType[") << iTargetUnitType << _T("]"));
	}

	if (GetAbil(AT_DUEL) > 0 && this != pkTarget && GetAbil(AT_DUEL) == pkTarget->GetAbil(AT_DUEL))
	{
		bool bTargetIsNotMyCaller = true;
		bool bImNotTargetsCaller = true;
		bool bDiffCaller = true;
		if(Caller().IsNotNull() && Caller() == pkTarget->GetID())
		{
			bTargetIsNotMyCaller = false; //Ÿ���� �� �����̴�.
		}
		if(pkTarget->Caller().IsNotNull() && pkTarget->Caller() == GetID())
		{
			bImNotTargetsCaller = false;
		}
		if(Caller().IsNotNull() && pkTarget->Caller().IsNotNull() && Caller() == pkTarget->Caller())
		{
			bDiffCaller = false;
		}
		if(bTargetIsNotMyCaller && bImNotTargetsCaller && bDiffCaller)
		{
			return true;
		}

		//if(Caller() != pkTarget->GetID() && pkTarget->Caller() != GetID()) //��ȯü�� ������ ������ ���� PC�� �ڱ� ��ȯü�� ������ ����
		//{
		//	return true;
		//}
	}

	if ( IsTargetUnitType( iMyUnitType, iTargetUnitType, iSkillTargetType) )
	{
		int const iMyTeam = GetAbil(AT_TEAM);
		if ( iMyTeam )
		{// Unit's Team�� �˻�.
			switch( iTargetUnitType )
			{
			case UT_PLAYER:
			case UT_PET:
			case UT_MONSTER:
			case UT_BOSSMONSTER:
			case UT_OBJECT:
			case UT_SUMMONED:
			case UT_SUB_PLAYER:
				{
					int const iTargetTeam = pkTarget->GetAbil(AT_TEAM);
					if ( iSkillTargetType & ESTARGET_ENEMY )
					{
						if ( iMyTeam == iTargetTeam || pkTarget->GetAbil(AT_UNIT_HIDDEN) )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}
					else if ( iSkillTargetType & ESTARGET_ALLIES )
					{
						if ( iMyTeam != iTargetTeam )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}
				}break;
			default:
				{
				}break;
			}
		}
		else if (IsUnitType(UT_ENTITY) && ENTITY_GUARDIAN==GetAbil(AT_ENTITY_TYPE) && UT_PLAYER==iTargetUnitType && !pkSkillDef)
		{
		}
		else if ( iTargetUnitType == iMyUnitType && (iSkillTargetType&ESTARGET_CASTER) && IsUnitType(UT_SUMMONED))
		{
		}
		else if ( this == pkTarget && (iSkillTargetType & ESTARGET_SELF) )
		{
		}
		else if ( this != pkTarget && (iSkillTargetType & ESTARGET_SELF) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		else if ( iTargetUnitType == iMyUnitType && !(iSkillTargetType & ESTARGET_ALLIES))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		else if ( (UT_MONSTER == iMyUnitType) && (UT_OBJECT == iTargetUnitType) && !(iSkillTargetType & ESTARGET_ALLIES))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		else
		{
			if( pkSkillDef )
			{
				int const iTargetGrade = pkSkillDef->GetAbil(AT_GRADE);
				if( EMGRADE_NONE != iTargetGrade
				&&	pkTarget->GetAbil(AT_GRADE) != iTargetGrade ) // ��ų�� Ÿ�� Grade�� ���� �Ǿ���, Ÿ���� ��ް� ���� ������ Ÿ���� �ȵȴ�
				{
					return false;
				}
			}
		}

		if (bCheckTargetValid)
		{
			if (pkTarget->CanbeTarget(iSkillTargetType))
			{
				// �ֱٿ� �̹� Damage�� �¾Ҵٸ�, �ʹ� ���� ������ ���� �ʵ��� �Ѵ�.
				BM::GUID const& rkTargetGuid = pkTarget->GetID();
				DequeTarget::const_iterator itor = m_kTargetDeque.begin();
				int iPos = 0;
				if(pkSkillDef)
				{
					iPos = pkSkillDef->GetAbil(AT_ATTACK_UNIT_POS);//EAttackedUnitPos
				}
				else if(CSkill* kSkill = GetSkill())
				{
					iPos = kSkill->GetAbil(AT_ATTACK_UNIT_POS);//EAttackedUnitPos
					pkSkillDef = kSkill->GetSkillDef();
				}

				GET_DEF(CEffectDefMgr, kEffectDefMgr);//�ݺ��� ���� ���� �̸� �غ��� ����

				while (itor != m_kTargetDeque.end())
				{
					if (itor->kGuid == rkTargetGuid)
					{
						if(pkSkillDef && pkSkillDef->GetAbil(AT_MON_CAN_TARGET_NO_DELAY) == 1)
						{
							return true;
						}
						bool bCorrectTarget = false;
						DWORD const dwNow = BM::GetTime32();
						if(iDmgCheckTime >=0 && dwNow - itor->dwDmgTime >= static_cast<DWORD>(iDmgCheckTime))
						{
							bCorrectTarget = true;	// üũ Ÿ��(Default : DEFAULT_DMG_CHECK_TIME = 500) ������ Dmg �޾Ҵ°�?
						}

						if(bCorrectTarget)
						{
							if(pkSkillDef && UnitType()==UT_PLAYER && pkTarget->UnitType()==UT_MONSTER)
							{
								CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(pkSkillDef->GetAbil(AT_DAM_EFFECT_S + pkTarget->GetAbil(AT_UNIT_SIZE) -1));
								if(pkEffectDef && pkEffectDef->GetAbil(AT_BLOW_VALUE))
								{
									return true;	
								}
							}
							int const iPosture = pkTarget->GetAbil(AT_POSTURE_STATE);	//���� Ÿ���� ����

							if(EAttacked_Pos_None==iPos) {iPos = EAttacked_Pos_Normal;}

							if((0==iPosture || 1==iPosture) && iPos&EAttacked_Pos_Normal)
							{
								bCorrectTarget = true;
							}
							else if(iPos&EAttacked_Pos_Blowup) // ���� ���� �ϴ� �� ó��
							{
								bCorrectTarget = true;
							}
							else if(iPos&EAttacked_Pos_ground)//�Ѿ��� �ְ�
							{
								bCorrectTarget = true;
								if(pkTarget->IsUnitType(UT_PLAYER))
								{
									bCorrectTarget = (1==iPosture);
								}
							}
							else
							{
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								bCorrectTarget = false;
							}
						}
					
						return bCorrectTarget;
			
						//INFO_LOG(BM::LOG_LV9, _T("[%s] Target cannot be enemy DmgTime[%ud], Now[%ud]"), __FUNCTIONW__, itor->dwDmgTime, dwNow);
					}
					++itor;
				}
			}
			else
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;

}

void CUnit::SetAICurrentAction(EAIActionType eNewAction)
{
	m_kAI.eCurrentAction = eNewAction;
}

bool CUnit::GetSync()
{
	bool bOld = m_bSendSyncPacket;	
	m_bSendSyncPacket = false;
	return bOld;
}

bool CUnit::CanbeTarget(int const iTargetType)
{
	if(	((iTargetType & ESTARGET_DEAD) == 0) //��� ���°� �ƴ� Ÿ��.
	&&	(this->IsDead()) )//����̸� ����.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}	
	if (	(iTargetType & ESTARGET_ENEMY)//����
		&& ( (0 != GetAbil(AT_CANNOT_DAMAGE)) || (0 < GetAbil(AT_UNIT_HIDDEN)) ) 
		)	// ���� �ְų� ���� ĳ���̸� ����.
	{
		// Hidden �����̰ų� AT_CANNOT_DAMAGE �����̸� ���� �� ����.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

// Ability ���� ����Ǿ�����, �ٽ� ����� �־�� �Ѵٴ� ���� �뺸
void CUnit::NftChangedAbil(WORD const wAbil, DWORD dwSendType)
{
	switch(wAbil)
	{
	case AT_REFRESH_ABIL_INV:
		{
			// Inven�� ������� ���ʴ�� �о �ٽ� ����ϵ��� �Ѵ�.
			SAbilIterator kItor;
			
			// Abil��ȭ ���� �� �ʱ�ȭ
			m_kInv.FirstAbil(&kItor);
			while ( m_kInv.NextAbil(&kItor) )
			{
				WORD wCal = GetCalculateAbil(kItor.wType);
				SetAbil( wCal, 0 );
			}		

			InvenRefreshAbil();

			// Abil��ȭ �Ŀ� �� �ʱ�ȭ
			m_kInv.FirstAbil(&kItor);
			while ( m_kInv.NextAbil(&kItor) )
			{
				WORD wCal = GetCalculateAbil(kItor.wType);
				SetAbil( wCal, 0 );
			}

			// �������İ� �����Ȱ��� �ٽ� ���
			CalculateInitAbil();
			
			//Max Hp / Max Mp���� ���� �� �ٷ� ������Ʈ���ش�.
			GetAbil(AT_C_MAX_HP);
			GetAbil(AT_C_MAX_MP);

			BM::Stream kAPacket(PT_U_G_NFY_CHANGED_INVEN);
			VNotify(&kAPacket);
		}break;
	case AT_EVENT_LEVELUP:
		{
			SAbilIterator kItor;
			FirstAbil(&kItor);
			while (NextAbil(&kItor))
			{
				if (IsCalculatedAbil(kItor.wType))
				{
					SetAbil(kItor.wType, 0);
					//FirstAbil(&kItor);
				}
			}
			//CalculateInitAbil();
		}break;
	case AT_ADD_COOL_TIME:
		{
			int iValue = GetEffectMgr().GetAbil(wAbil);
			SetAbil(wAbil, iValue);
			
			if(GetSkill())
			{				
				GetSkill()->SetSkillCoolTime(iValue);
			}

			if ( dwSendType != E_SENDTYPE_NONE)
			{
				SendAbil((EAbilType)wAbil, dwSendType);
			}
			
		}break;
	case AT_ADD_COOL_TIME_RATE:
		{
			int iValue = GetEffectMgr().GetAbil(wAbil);
			SetAbil(wAbil, iValue);

			if(GetSkill())
			{				
				GetSkill()->SetSkillCoolTimeRate(iValue);
			}

			if ( dwSendType != E_SENDTYPE_NONE)
			{
				SendAbil((EAbilType)wAbil, dwSendType);
			}
		}break;
	case AT_ADD_CAST_TIME:
		{
			int iValue = GetEffectMgr().GetAbil(wAbil);
			SetAbil(wAbil, iValue);

			if(GetSkill())
			{
				GetSkill()->SetSkillCastingTime(iValue);
			}		

			if ( dwSendType != E_SENDTYPE_NONE)
			{
				SendAbil((EAbilType)wAbil, dwSendType );
			}
		}break;
	case AT_FINAL_MAX_HP:
	case AT_R_FINAL_MAX_HP:
	case AT_C_FINAL_MAX_HP:
		{
			SetAbil(AT_C_FINAL_MAX_HP, 0);
			SetAbil(AT_C_MAX_HP, 0);
			if ( dwSendType != E_SENDTYPE_NONE)
			{
				SendAbil((EAbilType)wAbil, dwSendType );
			}
		}break;
	case AT_FINAL_MAX_MP:
	case AT_R_FINAL_MAX_MP:
	case AT_C_FINAL_MAX_MP:
		{
			SetAbil(AT_C_FINAL_MAX_MP, 0);
			SetAbil(AT_C_MAX_MP, 0);
			if ( dwSendType != E_SENDTYPE_NONE)
			{
				SendAbil((EAbilType)wAbil, dwSendType );
			}
		}break;
	default:
		{
			WORD wCal = ::GetCalculateAbil(wAbil);
			if ( 0 < wCal )
			{
				switch(wCal)
				{
				// �Ʒ� �װ��� ����� �������İ� �����Ǿ� �ֱ� ������ �ٷ� ����� ���� �Ѵ�.
				case AT_C_STR:
				case AT_C_INT:
				case AT_C_DEX:
				case AT_C_CON:
					CalculateAbil(wCal);
					break;
				default:
					SetAbil(wCal, 0);
					break;
				}
			}
			else
			{
				CalculateAbil(wAbil);
			}
			if ( dwSendType != E_SENDTYPE_NONE)
			{
				SendAbil((EAbilType)wAbil, dwSendType );
			}
		}break;
	}
}

// ���� ����� ���� ���� �� ȣ���Ѵ�.
int CUnit::CalculateAbil(WORD const wAbil)
{
	int iValue = 0;
	if (IsCalculatedAbil(wAbil))
	{
		// CalculateAbil�� ��� ���� ����
		//	1. ��� = Unit.GetAbil(Basic) + Inven.GetAbil(Basic) + Effect.GetAbil(Basic)
		//	2. ��� += (Unit.GetAbil(Rate) + Inven.GetAbil(Rate) + Effect.GetAbil(Rate)) * Unit.GetAbil(Basic) / 1000
		WORD wBasic = GetBasicAbil(wAbil);	
		WORD wRate = GetRateAbil(wAbil);
		int iBasicValue = GetAbil(wBasic);
		if( wAbil == AT_C_CRITICAL_POWER)
		{//ũ��Ƽ�� �������� ��쿣 ��� ���� �����־�� �Ѵ�.
			wRate = 0;
			//iValue += int(GetAbil(wRate) + m_kEffect.GetAbil(wRate) + iRateFromInv);
		}

		// AT_IGNORE_PVP_MODE_ABIL �� ���õǾ� �ִٸ�, ���潺 ����̱� ������ PvP��尡 �ƴ�!!!!!!!
		bool const bIsPvP = ( (0 < GetAbil(AT_TEAM)) && ( 0 == GetAbil(AT_IGNORE_PVP_MODE_ABIL)) ) || (0 < GetAbil(AT_DUEL) );	// PVP�� �� ������ �Ǿ��ְų�, �������̶��

		// ������ ��� Rate ����� �̸� ���밪���� ���ȴ�
		int const iValueFromInv = bIsPvP ? m_kInv.GetRemovedAmplyAbil(wBasic) : m_kInv.GetAbil(wBasic);	// pvp �뷱���� ���� �������� ����
		iValue = iBasicValue + iValueFromInv + m_kEffect.GetAbil(wBasic);

		// ��� RateAbil�� õ������ ����Ѵ�.
		int const iRateFromInv = bIsPvP ? m_kInv.GetRemovedAmplyAbil(wRate) : m_kInv.GetAbil(wRate);		// pvp �뷱���� ���� �������� ����

		switch(g_kLocal.ServiceRegion())
		{
		case LOCAL_MGR::NC_TAIWAN:
		case LOCAL_MGR::NC_CHINA:
		case LOCAL_MGR::NC_SINGAPORE:
		case LOCAL_MGR::NC_BRAZIL:
			{
				iBasicValue = iValue;
			}break;
		default:
			{				
			}break;
		}


		if( (true == PgInventoryUtil::FilterPercentAbil( wRate )) && bIsPvP )
		{	// filter�� true�� ��ȯ �Ǵ� Abil���� �ɼ� ���� ����Ǳ� ���� ���� ����ִ�.
			iValue += (int(iBasicValue * ((GetAbil(wRate) + m_kEffect.GetAbil(wRate)) / ABILITY_RATE_VALUE_FLOAT))) - iRateFromInv;
		}
		else
		{
			iValue += int(iBasicValue * ((GetAbil(wRate) + m_kEffect.GetAbil(wRate) + iRateFromInv) / ABILITY_RATE_VALUE_FLOAT));
		}
		

		if(AT_C_MAX_HP==wAbil && iValue < 0)
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("ClassNo [") << GetAbil(AT_CLASS) << _T("]") << _T("AT_C_MAX_HP [") << iValue << _T("]") );
			iValue = INT_MAX;
		}
		
		switch(wAbil)
		{
		case AT_C_MAX_HP:		
			{
				int const iRate = GetAbil(AT_C_FINAL_MAX_HP);
				if(iRate)
				{
					__int64 i64Temp = iValue;
					i64Temp *= iRate;
					i64Temp /= ABILITY_RATE_VALUE64;
					i64Temp += iValue;
					iValue = static_cast<int>( i64Temp );
				}
			}break;
		case AT_C_MAX_MP:
			{
				int const iRate = GetAbil(AT_C_FINAL_MAX_MP);
				if(iRate)
				{
					__int64 i64Temp = iValue;
					i64Temp *= iRate;
					i64Temp /= ABILITY_RATE_VALUE64;
					i64Temp += iValue;
					iValue = static_cast<int>( i64Temp );
				}
			}break;
		}

		SetAbil(wAbil, iValue);

		// ���ݷ�/������ �⺻ Abil�̴�.
		/*
		switch(wAbil)
		{
		case AT_C_STR:
		case AT_C_INT:
		case AT_C_CON:
		case AT_C_DEX:
			CalculateBattleAbil();
			break;
		}
		*/
		// Min/Max���� ������ Abil�� �ִ�.
		int iNewValue = iValue;
#ifndef ABIL_NO_MINMAX
		switch(wAbil)
		{
		case AT_C_PHY_DMG_DEC:
		case AT_C_MAGIC_DMG_DEC:
			iNewValue = __min(iValue, MAKE_ABIL_RATE(100));	// MAX:100% 2009/01/09 ������� ��û
			break;
		//case AT_C_HIT_SUCCESS_VALUE:
		//	iNewValue = __max(__min(iValue, MAKE_ABIL_RATE(95)), MAKE_ABIL_RATE(25));	// MAX:95%, MIN:25%
		//	break;
		//case AT_DODGE_SUCCESS_VALUE:
		//	iNewValue = __min(iValue, MAKE_ABIL_RATE(95));	// MAX:95%
		//	break;
		case AT_C_MOVESPEED:
			iNewValue = __max(0, iValue);	// ��Ȥ �ߺ��� ������� MoveSpeed�� <0 �ɼ� �����Ƿ� ���⼭ �����ش�.
			break;
		case AT_C_MAX_HP:
			{
				int const iHP = GetAbil(AT_HP);
				if ( iHP > iValue )
				{
					SetAbil( AT_HP, iValue, true, true );
				}
			}break;
		case AT_C_MAX_MP:
			{
				int const iMP = GetAbil(AT_MP);
				if ( iMP > iValue )
				{
					SetAbil( AT_MP, iValue, true, true );
				}
			}break;
// 		case AT_C_MAX_DP:
// 			{
// 				int const iDP = GetAbil(AT_DP);
// 				if ( iDP > iValue )
// 				{
// 					SetAbil( AT_DP, iValue, true, true );
// 				}
// 			}break;
		}
#endif
		if (iNewValue != iValue)
		{
			SetAbil(wAbil, iNewValue);
			iValue = iNewValue;
		}
	}
	else
	{
		iValue = m_kEffect.GetAbil(wAbil);

		if ( true == IsCountAbil( wAbil ) )
		{
			int iUnitValue = GetAbil( wAbil );
			iUnitValue &= (~AT_CF_EFFECT_CHK);
			iValue += iUnitValue;
		}

		SetAbil(wAbil, iValue);
	}

	return iValue;
}

/*
void CUnit::CalculateBattleAbil()
{
}
*/

// MapMove �� ���� �� Effect�� �����Ű��
void CUnit::DoEffect()
{
	BM::Stream kAPacket(PT_U_G_NFY_ADD_EFFECT);
	kAPacket.Push(EFFECT_ALL_MYEFFECT_NUM);	// ��� Effect�� ���ؼ�
	VNotify(&kAPacket);

	DoEquipEffect();
}

bool CUnit::IsAIType(const EAIType eMonAIType)const
{
	int iAI = GetAbil(AT_AI_TYPE);

	if(iAI&eMonAIType )
	{
		return true ;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void CUnit::DeleteTarget(BM::GUID const& rkGuid)
{
	//ConTarget::iterator remove_itr = std::remove(m_kTargetGuidCon.begin(), m_kTargetGuidCon.end(), rkGuid );
	//m_kTargetGuidCon.erase( remove_itr, m_kTargetGuidCon.end() );
 	DequeTarget::iterator itor = m_kTargetDeque.begin();
 	while (itor != m_kTargetDeque.end())
 	{
 		if (itor->kGuid == rkGuid)
 		{
 			itor->kGuid.Clear();
 			break;
 		}
 		++itor;
 	}
}

 void CUnit::AdjustTargetList()
 {
 	DequeTarget::iterator itor = m_kTargetDeque.begin();
 	while (itor != m_kTargetDeque.end())
 	{
 		if (itor->kGuid == BM::GUID::NullData())
 		{
 			m_kTargetDeque.erase(itor);
 			itor = m_kTargetDeque.begin();
 			continue;
 		}
 		++itor;
 	}
}

void CUnit::SetTargetList(const UNIT_PTR_ARRAY& rkArray)
{
	m_kTargetDeque.clear();

	UNIT_PTR_ARRAY::const_iterator itor = rkArray.begin();
	while (itor != rkArray.end())
	{
		if (itor->pkUnit != NULL)
		{
			m_kTargetDeque.push_back(STargetInfo(itor->pkUnit->GetID()));
		}
		++itor;
	}
}

void CUnit::ClearTargetList()
{
	m_kTargetDeque.clear();
}

bool CUnit::IsSummonUnit(BM::GUID const& rkGuid, bool const bCheckDieUnit)
{
	VEC_SUMMONUNIT::const_iterator itor = find(m_kSummonUnit.begin(), m_kSummonUnit.end(), rkGuid);
	if(itor != m_kSummonUnit.end())
	{
		return true;
	}
	
	if(bCheckDieUnit)
	{
		for(VEC_DIE_SUMMONUNIT::iterator die_itor=m_kDieSummonUnit.begin(); die_itor!=m_kDieSummonUnit.end(); ++die_itor)
		{
			if((*die_itor).first == rkGuid)
			{
				//�ӽ÷� 10�ʱ����� �����Ű��, ���� �� ������ ����ð��� ������ �� �ִ� ����� �߰��Ǹ� ������(��ŷ������)
				if((BM::GetTime32()-(*die_itor).second) > 10000)
				{
					m_kDieSummonUnit.erase(die_itor);
					return false;
				}

				return true;
			}
		}
	}

	return false;
}

// ��ȯ������ Ŭ������ �Ѱ��� �����ϰ� �ִ���?
bool CUnit::IsSummonUnitClass(int const iClassNo)const
{
	VEC_SUMMONUNIT::const_iterator itor = find(m_kSummonUnit.begin(), m_kSummonUnit.end(), iClassNo);
	return itor != m_kSummonUnit.end();
}

int CUnit::GetSummonUnitCount()
{
	return (int)m_kSummonUnit.size();
}

// UniqueClass�� ��ȯ���� Ŭ������ �Ѱ��� �����ؾ� �Ѵ�.
bool CUnit::AddSummonUnit(BM::GUID const& rkSummonUnitGuid, int const iClassNo, SummonOptionType const eOption, int const iMaxSummonUnitCount)
{
	if (false==(ESO_IGNORE_MAXCOUNT&eOption) && GetSummonUnitCount() >= iMaxSummonUnitCount)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if (IsSummonUnit(rkSummonUnitGuid))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( (ESO_UNIQUE_CLASS&eOption) && IsSummonUnitClass(iClassNo) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kSummonUnit.push_back(SSummonUnitInfo(rkSummonUnitGuid,iClassNo));
	return true;
}

bool CUnit::DeleteSummonUnit(BM::GUID const& rkSummonUnitGuid)
{
	VEC_SUMMONUNIT::iterator itor = find(m_kSummonUnit.begin(), m_kSummonUnit.end(), rkSummonUnitGuid);
	if (itor != m_kSummonUnit.end())
	{
		m_kSummonUnit.erase(itor);
		m_kDieSummonUnit.push_back(std::make_pair(rkSummonUnitGuid, BM::GetTime32()));
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

BM::GUID const& CUnit::GetSummonUnit(int const iIndex) const
{
	if (iIndex < (int)m_kSummonUnit.size())
	{
		return m_kSummonUnit.at(iIndex).kGuid;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return Guid NullData"));
	return BM::GUID::NullData();
}

void CUnit::ClearSummonUnit()
{
	m_kSummonUnit.clear();
}

void CUnit::GoalPos(POINT3 const& ptPos)
{
	if(IsUnitType(UT_SUMMONED) && POINT3::NullData()!=m_kGoalPos && POINT3::NullData()==ptPos)
	{
		bool a= false;
		a = true;
	}

	m_kGoalPos = ptPos;
	SetAbil(AT_GOALPOS_SET_TIME, (int)BM::GetTime32());
}

bool CUnit::IsNextGoalPos()
{
	return !m_kContNextGoalPos.empty();
}

POINT3 CUnit::NextGoalPos()
{
	POINT3 kPos;
	if(!m_kContNextGoalPos.empty())
	{
		kPos = m_kContNextGoalPos[0];
		m_kContNextGoalPos.pop_front();
	}
	else
	{
		kPos = POINT3::NullData();
	}

	return kPos;
}

void CUnit::NextGoalPos_Add(POINT3 const &ptPos)
{
	m_kContNextGoalPos.push_back(ptPos);
}

void CUnit::NextGoalPos_Clear()
{
	m_kContNextGoalPos.clear();
}

bool CUnit::CheckSkillFilter(int const iSkillNo, SSFilter_Result* pkResult, ESkillFilterType eFilterType)
{
	if (pkResult != NULL)
	{
		pkResult->Init();
	}
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if( !pkSkillDef )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Cannot get skilldef [") << iSkillNo << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	if( 1 == pkSkillDef->GetAbil(AT_IS_CHAIN_COMBO) 
		|| 1 == pkSkillDef->GetAbil(AT_IS_OLD_COMBO) )
	{
		return true;
	}
	int iCount = 0;
	int iEffect = 0;
	while ((iCount < 10) && (iEffect = pkSkillDef->GetAbil(AT_NEED_EFFECT_01+iCount)) > 0)
	{
		if (GetEffect(iEffect, true) == NULL)
		{
			// Effect�� ������ ���� �����Ƿ� ����
			if (pkResult != NULL)
			{
				pkResult->eResult = SSFilter_Result::ESFResult_NeedEffect;
				pkResult->iCauseID = iEffect;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		++iCount;
	}

	return GetSkill()->CheckFilter(iSkillNo, pkResult, eFilterType);
}

void CUnit::OnTargetDamaged(BM::GUID const& guidTarget)
{
	DequeTarget::iterator& itor = m_kTargetDeque.begin();
	while (itor != m_kTargetDeque.end())
	{
		if (itor->kGuid == guidTarget)
		{
			itor->dwDmgTime = BM::GetTime32();
			itor->eState = STargetInfo::ETState_Damaged;
			return;
		}
		++itor;
	}

	{
		STargetInfo kInfo(guidTarget);
		kInfo.eState = STargetInfo::ETState_Damaged;
		kInfo.dwDmgTime = BM::GetTime32();

		m_kTargetDeque.push_back(kInfo);
	}
}

// Caster/Target�� UnitType ������ Skill�� Target�� �� �� �ִ°� �˻��Ѵ�.
bool CUnit::IsTargetUnitType(int eCaster, int eTarget, int iSkillTargetType)
{
	static const int eUnitTypeArray[] = {UT_PLAYER, UT_MONSTER, UT_GROUNDBOX, UT_PET, UT_NPC, UT_BOSSMONSTER, UT_ENTITY, UT_SUMMONED, UT_OBJECT, UT_NONETYPE};
	static bool const bEnemyTable[][10] = {	{true,  true,  false, false, false, true,  false, true,  true,  false}, // UT_PLAYER
											{true,  false, false, false, false, false, false, true,  true,  false},	// UT_MONSTER
											{false, false, false, false, false, false, false, false, false, false},	// UT_GROUNDBOX
											{false, true,  false, false, false, true,  false, true,  true,  false},	// UT_PET
											{false, false, false, false, false, false, false, false, false, false},	// UT_NPC
											{true,  false, false, false, false, false, false, false, true,  false},	// UT_BOSSMONSTER
											{true,  true,  false, false, false, true,  false, true,  true,  false},	// UT_ENTITY
											{true,  true,  false, false, false, true,  false, true,  true,  false},	// UT_SUMMONED
											{false, false, false, false, false, false, false, false, false, false}};	// UT_NONETYPE
	static bool const bFriendTable[][10] = {{true,  false,  true,  true,  true, false,  true,  true, false, false}, // UT_PLAYER
											{false,  true,  true,  true,  true,  true,  true, false, false, false},	// UT_MONSTER
											{true,   true,  true,  true,  true,  true,  true,  true,  true, false},	// UT_GROUNDBOX
											{true,  false,  true,  true,  true, false,  true, false, false, false},	// UT_PET
											{true,   true,  true,  true,  true,  true,  true,  true,  true, false},	// UT_NPC
											{false,  true,  true,  true,  true,  true,  true,  true, false, false},	// UT_BOSSMONSTER
											{false, false,  true,  true,  true, false,  true, false, false, false},	// UT_ENTITY
											{true,  false,  true,  true,  true, false,  true, true,  false, false},	// UT_SUMMONED
											{false, false, false, false, false, false, false, false, false, false}};	// UT_NONETYPE
	int iCasterIndex = 0, iTargetIndex = 0;
	int iIndex = 0;
	while (eUnitTypeArray[iIndex] != UT_NONETYPE && (iCasterIndex == 0 || iTargetIndex == 0))
	{
		if (eUnitTypeArray[iIndex] == eCaster)
		{
			iCasterIndex = iIndex;
		}
		if (eUnitTypeArray[iIndex] == eTarget)
		{
			iTargetIndex = iIndex;
		}

		++iIndex;
	}
	if (iCasterIndex < 0 && iTargetIndex < 0)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" Cannot find UnitType CasterUnitType[") << eCaster << _T("], TargetUnitType[") << eTarget << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool bFindEnemy = false, bFindFriend = false;
	if (iSkillTargetType & ESTARGET_SELF)		{ bFindEnemy = bFindEnemy || false;	bFindFriend = bFindFriend || true;	}
	if (iSkillTargetType & ESTARGET_ENEMY)		{ bFindEnemy = bFindEnemy || true;	bFindFriend = bFindFriend || false;	}
	//if (iSkillTargetType & ESTARGET_PARTY)	{ bFindEnemy = bFindEnemy || false;	bFindFriend = bFindFriend || true;	}
	//if (iSkillTargetType & ESTARGET_LOCATION) { bFindEnemy = bFindEnemy || true;	bFindFriend = bFindFriend || true;	}
	//if (iSkillTargetType & ESTARGET_HIDDEN)	{ bFindEnemy = bFindEnemy || true;	bFindFriend = bFindFriend || true;	}
	//if (iSkillTargetType & ESTARGET_DEAD)		{ bFindEnemy = bFindEnemy || true;	bFindFriend = bFindFriend || true;	}
	if (iSkillTargetType & ESTARGET_ALLIES)		{ bFindEnemy = bFindEnemy || false;	bFindFriend = bFindFriend || true;	}
	if (iSkillTargetType & ESTARGET_CASTER)		{ bFindEnemy = bFindEnemy || true;	bFindFriend = bFindFriend || false;	}
	if (iSkillTargetType & ESTARGET_SUMMONED)	{ bFindEnemy = bFindEnemy || false;	bFindFriend = bFindFriend || true;	}

	bool bResult = false;
	if (bFindEnemy)
	{
		bResult = bResult || bEnemyTable[iCasterIndex][iTargetIndex];
	}
	if (bFindFriend)
	{
		bResult = bResult || bFriendTable[iCasterIndex][iTargetIndex];
	}
	return bResult;
}

void CUnit::ClearAllEffect( bool const bDirectDelete, bool const bLogout, bool const bPenalty )
{
	if ( true == bDirectDelete )
	{
		ContEffectItor kItor;
		m_kEffect.GetFirstEffect(kItor);
		CEffect* pkEffect = NULL;
		std::vector<int> kDeleteEffect;
		while ( (pkEffect = m_kEffect.GetNextEffect(kItor)) != NULL )
		{
			switch ( pkEffect->GetType() )
			{
			case EFFECT_TYPE_PENALTY:
				{
					if ( !bPenalty )
					{
						break;
					}
				}// no break
			case EFFECT_TYPE_BLESSED:
			case EFFECT_TYPE_CURSED:
			case EFFECT_TYPE_ITEM:
				{
					//Clear�ÿ� �������� �ʾƾ��ϴ� ����Ʈ�� �ƴ� ���
					if ( 0 >= pkEffect->GetAbil( AT_SAVE_DB ) )
					{
						if( true == bLogout )
						{
							kDeleteEffect.push_back( pkEffect->GetKey() );
						}
						else if( 0 == pkEffect->GetAbil(AT_NOT_DELETE_EFFECT) )
						{
							kDeleteEffect.push_back( pkEffect->GetKey() );
						}
					}
				}break;
			case EFFECT_TYPE_ACTIVATE_SKILL:
			default:
				{
				}break;
			}
		}

		std::vector<int>::iterator delete_itr = kDeleteEffect.begin();
		for ( ; delete_itr != kDeleteEffect.end() ; ++delete_itr )
		{
			//INFO_LOG( BM::LOG_LV9, __FL__ << _T(" Effect deleted [") << *delete_itr << _T("]") );
			DeleteEffect( *delete_itr );
		}

		m_kEffect.MustAllClear(false);
		m_kEffect.MustCurseClear(false);
		m_kEffect.Damaged(false);
	}
	else
	{
		// �������� �Ѵٰ� ǥ�ø� ���ܶ�...
		// ��Ȥ EffectTick ���� SetAbil(AT_HP, 0) ���� �װ� �� �� �ִµ�, �̶� VOnDie() ȣ��ǰ�
		// ClearAllEffect(..)�� ȣ��ǰ� �Ǹ� Effect�� Iterator�� ������ ������ �װ� �ȴ�.
		// �̰��� �����ϱ� ���ؼ� clear ǥ�ø� ����� EffectTick���� ���쵵�� �Ѵ�.
		m_kEffect.MustAllClear(true);
	}
}

void CUnit::ClearCurseEffect(bool const bDirectDelete)
{
	if (bDirectDelete)
	{
		ContEffectItor kItor;
		m_kEffect.GetFirstEffect(kItor);
		CEffect* pkEffect = NULL;
		std::vector<int> kDeleteEffect;
		while ((pkEffect = m_kEffect.GetNextEffect(kItor)) != NULL)
		{
			EEffectType eType = (EEffectType) pkEffect->GetType();
			if(eType == EFFECT_TYPE_CURSED)
			{	
				if(0 == pkEffect->GetAbil(AT_NOT_DELETE_EFFECT))
				{
					kDeleteEffect.push_back(pkEffect->GetKey());
				}
			}
		}

		for (size_t i=0; i<kDeleteEffect.size(); ++i)
		{
			INFO_LOG( BM::LOG_LV9, __FL__ << _T(" Effect deleted [") << kDeleteEffect.at(i) << _T("]") );
			DeleteEffect(kDeleteEffect.at(i));
		}

		m_kEffect.MustCurseClear(false);
		m_kEffect.Damaged(false);
	}
	else
	{
		// �������� �Ѵٰ� ǥ�ø� ���ܶ�...
		// ��Ȥ EffectTick ���� SetAbil(AT_HP, 0) ���� �װ� �� �� �ִµ�, �̶� VOnDie() ȣ��ǰ�
		// ClearAllEffect(..)�� ȣ��ǰ� �Ǹ� Effect�� Iterator�� ������ ������ �װ� �ȴ�.
		// �̰��� �����ϱ� ���ؼ� clear ǥ�ø� ����� EffectTick���� ���쵵�� �Ѵ�.
		m_kEffect.MustCurseClear(true);
	}
}

BM::GUID const &CUnit::Caller()const
{
	if ( BM::GUID::IsNull(m_kCaller) )
	{
		return GetID();
	}
	return m_kCaller;
}

bool CUnit::IsHaveCaller()const
{
	if ( m_kCaller.IsNull() || m_kCaller == GetID())
	{
		return false;
	}
	return true;
}

void CUnit::SetDeliverDamageUnit(BM::GUID const& rkGuid)
{
	m_kDeliverDamageUnit = rkGuid;
}

BM::GUID const& CUnit::GetDeliverDamageUnit() const
{
	return m_kDeliverDamageUnit;
}

void CUnit::DoEquipEffect()
{
	// ���� ������ Effect �� ������� �ش�.
	CONT_HAVE_ITEM_DATA kItemList;
	GetInven()->GetItems(IT_FIT, kItemList);
	
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CONT_HAVE_ITEM_DATA::const_iterator itor = kItemList.begin();
	while (itor != kItemList.end())
	{
		PgBase_Item const & kItem = (*itor).second;
		
		for(int i=AT_EFFECTNUM1; i <= AT_EFFECTNUM10; ++i)
		{
			int iEffectNum = kItemDefMgr.GetAbil(kItem.ItemNo(), i);
			if (iEffectNum > 0 && !kItem.EnchantInfo().IsNeedRepair())
			{
				SEffectCreateInfo kCreate;
				kCreate.eType = EFFECT_TYPE_NORMAL;
				kCreate.iEffectNum = iEffectNum;
				AddEffect(kCreate);
			}
			else
			{
				break;
			}
		}

		/*int const iMonsterCardIndex = kItem.EnchantInfo().MonsterCard();
		if( iMonsterCardIndex > 0 )
		{
			DoEquipEffectMonsterCard(EQT_ADD_EFFECT, iMonsterCardIndex);
		}*/

		++itor;
	}
	DoEquipEffectSetItem();
}

void CUnit::DoEquipEffectSetItem()
{// �Ϲ� ��Ʈ �����ۿ� ���� ����Ʈ �߰�
	PgInventory* pkInv = GetInven();
	if(NULL == pkInv)
	{
		return;
	}
	
	CONT_UNIQUE_INT kContAddEffect;
	CONT_UNIQUE_INT kContDelEffect;

	CONT_HAVE_ITEM_DATA kItemList;
	if( S_OK == pkInv->GetItems(IT_FIT, kItemList) )
	{
		DoSetItemEffect(kItemList, kContAddEffect, kContDelEffect);
	}

	CONT_HAVE_ITEM_DATA kItemCashList;
	if( S_OK == pkInv->GetItems(IT_FIT_CASH, kItemCashList) )
	{
		DoSetItemEffect(kItemCashList, kContAddEffect, kContDelEffect);
	}

	{// �ݵ�� ����� !����! �����
		CONT_UNIQUE_INT::iterator kItor = kContDelEffect.begin();
		while(kContDelEffect.end() != kItor)
		{
			DeleteEffect( (*kItor) );
			++kItor;
		}
	}
	{// ����Ʈ �ɰ� �Ǵ�
		CONT_UNIQUE_INT::iterator kItor = kContAddEffect.begin();
		while(kContAddEffect.end() != kItor)
		{
			SEffectCreateInfo kCreate;
			kCreate.eOption =  SEffectCreateInfo::ECreateOption_CallbyServer;
			kCreate.eType = EFFECT_TYPE_NORMAL;
			kCreate.iEffectNum = (*kItor);
			AddEffect(kCreate);
			++kItor;
		}
	}
}

void CUnit::DoSetItemEffect(CONT_HAVE_ITEM_DATA const& rkItemList, CONT_UNIQUE_INT& rkContAddEffect, CONT_UNIQUE_INT& rkContDelEffect)
{
	if( rkItemList.empty() )
	{
		return;
	}
	
	GET_DEF(CItemSetDefMgr, kItemSetDefMgr);
	CAbilObject kSetItemAbil;
	PgInventory::CONT_SETITEM_NO kSetCont;

	// SetItem
	CONT_HAVE_ITEM_DATA::const_iterator item_itor = rkItemList.begin();
	while(item_itor != rkItemList.end())
	{
		PgBase_Item const &kItem = (*item_itor).second;
		int const iSetNo = kItemSetDefMgr.GetItemSetNo(kItem.ItemNo());
		if( iSetNo )
		{
			kSetCont.insert(iSetNo);
		}
		++item_itor;
	}

	PgInventory::CONT_SETITEM_NO::iterator set_itor = kSetCont.begin();
	while(set_itor != kSetCont.end())
	{
		int const iSetNo = (*set_itor);
		CItemSetDef const *pOrgSetDef = kItemSetDefMgr.GetDef(iSetNo);
		
		if(pOrgSetDef)
		{
			bool bCompleteSet = false;
			int const iEquipPiece = pOrgSetDef->CheckNeedItem(rkItemList, this, bCompleteSet);
			if( iEquipPiece )
			{
				CItemSetDef const *pEquipSetDef = kItemSetDefMgr.GetEquipAbilDef(iSetNo, iEquipPiece);
				if( pEquipSetDef )
				{
					for(int i=AT_EFFECTNUM1; i <= AT_EFFECTNUM10; ++i)
					{
						int iSetEffectNum = pOrgSetDef->GetAbil(i);
						if(0 == iSetEffectNum)
						{
							break;
						}

						if( pEquipSetDef->GetAbil(i) == iSetEffectNum )
						{// pOrgSetDef�� �ְ� pEquipSetDef�� �־�� ����Ʈ�� �ɾ��� ����(������ ����)
							rkContAddEffect.insert(iSetEffectNum);
						}
						else 
						{// pOrgSetDef���� �ְ� pEquipSetDef���� ���ٸ�, ����Ʈ�� ������ ���̴�(������ ��ü �� ����)
							rkContDelEffect.insert(iSetEffectNum);
						}
					}
				}
			}
		}
		++set_itor;
	}
}

void CUnit::DoEquipEffectMonsterCard(EffectQueueType eType, int const iOrderIndex, int const iCardIndex)
{
	CONT_MONSTERCARD const *kContMonsterCard = NULL;
	g_kTblDataMgr.GetContDef(kContMonsterCard);
	if( kContMonsterCard )
	{
		CONT_MONSTERCARD::key_type kKey(iOrderIndex, iCardIndex);

		CONT_MONSTERCARD::const_iterator iter = kContMonsterCard->find(kKey);
		if( kContMonsterCard->end() != iter )
		{
			CONT_MONSTERCARD::mapped_type const iMonsterCardItemNo = (*iter).second;

			GET_DEF(CItemDefMgr, kItemDefMgr);
			int const iMonsterCardEffectNo = kItemDefMgr.GetAbil(iMonsterCardItemNo, AT_EFFECTNUM1);
			if( iMonsterCardEffectNo > 0 )
			{
				if( EQT_ADD_EFFECT == eType )
				{
					SEffectCreateInfo kCreate;
					kCreate.eType = EFFECT_TYPE_NORMAL;
					kCreate.iEffectNum = iMonsterCardEffectNo;
					AddEffect(kCreate);
				}
				else
				{
					DeleteEffect(iMonsterCardEffectNo);
				}
			}
		}
	}
}

bool CUnit::CheckPassive(int iSkillNo, SActArg const* pkAct)
{
	if (!IsUnitType(UT_PLAYER) && !IsUnitType(UT_PET))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	PgControlUnit* pkUnit = dynamic_cast<PgControlUnit*>(this);

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (pkSkillDef == NULL)
	{
		INFO_LOG( BM::LOG_LV4, __FL__ << _T(" Cannot Get SkillDef SkillNo[") << iSkillNo << _T("]") );
		pkUnit->GetMySkill()->Delete(iSkillNo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(IsUnitType(UT_PLAYER))
	{
		__int64 i64Value = pkSkillDef->GetAbil64(AT_CLASSLIMIT);
		int iClass = this->GetAbil(AT_CLASS);
		const CONT_DEFUPGRADECLASS* pkUpClass = NULL;
		g_kTblDataMgr.GetContDef(pkUpClass);

		if(!pkUpClass)
		{
			INFO_LOG( BM::LOG_LV3, __FL__ << _T(" Cannot Get CONT_DEFUPGRADECLASS....") );
			pkUnit->GetMySkill()->Delete(iSkillNo);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if (!IS_CLASS_LIMIT(i64Value, iClass))
		{
			// Class Limit
			INFO_LOG( BM::LOG_LV2, __FL__ << _T(" Player has unauthorized Passive Skill Cause[ClassLimit], SkillNo[") << iSkillNo << _T("]") );
			pkUnit->GetMySkill()->Delete(iSkillNo);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		int iValue = pkSkillDef->GetAbil(AT_LEVELLIMIT);
		if (iValue > this->GetAbil(AT_LEVEL))
		{
			// Level Limit
			INFO_LOG( BM::LOG_LV2, __FL__ << _T(" Player has unauthorized Passive Skill Cause[LevelLimit], SkillNo[") << iSkillNo << _T("]") );
			pkUnit->GetMySkill()->Delete(iSkillNo);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		iValue = pkSkillDef->GetAbil(AT_WEAPON_LIMIT);
		if (iValue != 0)
		{
			PgBase_Item kItem;
			if(S_OK != pkUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_WEAPON), kItem))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef* pkDef = (CItemDef*) kItemDefMgr.GetDef(kItem.ItemNo());
			if( NULL == pkDef )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV2, BM::vstring()<<__FL__<<L"Cannot Get ItemDef ItemNo["<<kItem.ItemNo()<<L"]");
				return false;
			}
			if ((pkDef->GetAbil(AT_WEAPON_TYPE) & iValue) == 0)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	return true;
}

void CheckSkillFilter_Delete_Effect(CUnit const* pkCaster, CUnit* pkTarget, int iSkillNo)
{
	if(pkTarget)
	{
		// Ÿ���� �ɷ��ִ� ��ų�� ���Ϳ��� �ش� ��ų�� Effect_Delete Filter�� ���� ���
		PgSkillFilter* pkFilter = pkTarget->GetSkill()->GetSkillFilter(iSkillNo);
		if(!pkFilter)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFilter is NULL"));
			return;
		}

		if(ESFilter_Delete_Effect > pkFilter->Type())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Wrong FilterType"));
			return;
		}

		VEC_EXCEPT_SKILL *pkDeleteList = pkFilter->GetExceptList();
		if(!pkDeleteList)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkDeleteList is NULL"));
			return;
		}

		if(pkDeleteList->empty())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("DeleteList Empty"));
			return;
		}

		for(VEC_EXCEPT_SKILL::iterator itor = pkDeleteList->begin(); itor != pkDeleteList->end(); ++itor)
		{
			//���� ������ ��ų�� ���� ��� ����
			CEffect* pkEffect = pkTarget->GetEffect((*itor), true);
			if(!pkEffect)
				continue;

			bool bDelete = true;
			if(ESFilter_Delete_Effect_From_Caster == pkFilter->Type())
			{
				//���� ����Ʈ�� �ִ� ����Ʈ�� Caster�� �� ����Ʈ�� ���
				//�ƴϸ� ���� ���� ����
				if( !pkEffect->IsCasterEx(pkCaster->GetID()) ) // ĳ���Ͱ� ������ ���� �Ǿ� �ϴ°�?
				{
					bDelete = false;
				}
			}

			if(bDelete)
			{
				pkTarget->DeleteEffect(pkEffect->GetEffectNo());
			}
		}
	}
}

float CUnit::GetAbilFloat(WORD const Type) const
{
	float fOut = (float) GetAbil(Type);
	switch (Type)
	{
	case AT_MOVESPEED:
	case AT_C_MOVESPEED:
		{
			fOut /= ABILITY_FLOAT_MULTIFLY;
		}break;
	}
	return fOut;
}

bool CUnit::SetAbilFloat(WORD const Type, float const fInValue, bool const bIsSend, bool const bBroadcast)
{
	float fNewValue = fInValue;
	switch (Type)
	{
	case AT_MOVESPEED:
	case AT_C_MOVESPEED:
		{
			fNewValue *= ABILITY_FLOAT_MULTIFLY;
		}break;
	}
	return SetAbil(Type, static_cast<int>(fNewValue), bIsSend, bBroadcast);
}

int CUnit::GetCountAbil( WORD const Type, int const iFlag )const
{
	CAbilObject const *pkAbilObject = ( (AT_CF_EFFECT == iFlag) ? dynamic_cast<CAbilObject const*>(&m_kEffect) : dynamic_cast<CAbilObject const*>(this) );
	return ::GetCountAbil( pkAbilObject, Type, iFlag );
}

void CUnit::AddCountAbil( WORD const Type, int const iFlag, bool const bAdd, DWORD const dwSendFlag )
{
	CAbilObject *pkAbilObject = ( (AT_CF_EFFECT == iFlag) ? dynamic_cast<CAbilObject*>(&m_kEffect) : dynamic_cast<CAbilObject*>(this) );
	if ( !::AddCountAbil( pkAbilObject, Type, iFlag, bAdd ) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Error Type<") << Type << _T("> Flag<") << iFlag << _T("> Name<") << Name() << _T("> CharGuid<") << GetID() << _T(">") );
	}

	this->NftChangedAbil( Type, dwSendFlag );
}

void CUnit::RemoveCountAbil( WORD const Type, int const iFlag, DWORD const dwSendFlag )
{
	CAbilObject *pkAbilObject = ( (AT_CF_EFFECT == iFlag) ? dynamic_cast<CAbilObject*>(&m_kEffect) : dynamic_cast<CAbilObject*>(this) );
	if ( !::RemoveCountAbil( pkAbilObject, Type, iFlag ) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Error Type<") << Type << _T("> Flag<") << iFlag << _T("> Name<") << Name() << _T("> CharGuid<") << GetID() << _T(">") );
	}

	this->NftChangedAbil( Type, dwSendFlag );
}

bool CUnit::GetTargetFromAggro(BM::GUID &rkGuid)
{
	BM::GUID kGuid;
	kGuid.Clear();
	if(AggroMeter())
	{
		VEC_GUID kVec;
		AggroMeter()->GetScoreTop(kVec);
		if(!kVec.empty())
		{
			rkGuid = (*kVec.begin());
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void CUnit::AddEffectQueue(EffectQueueData &rkQueueData)
{
	if(rkQueueData.m_kEffectNo==0)
	{
		return;
	}
	//������ Pointer�� �״�� ���� �Ǿ�� �Ѵ�.
	m_kContEffectQueue.push_back(rkQueueData);
}

void CUnit::SwapEffectQueue(CONT_EFFECT_QUEUE& rkOut)
{
	m_kContEffectQueue.swap(rkOut);
}

void CUnit::EffectQueueUpdate()
{
	BM::Stream kPacket(PT_U_G_NFY_UPDATE_QUEUE_EFFECT);
	VNotify(&kPacket);
}

void CUnit::SetSyncType( BYTE const bySyncType, bool const bOnlyChangeState )
{
	if ( true == bOnlyChangeState )
	{
		m_bySyncType = bySyncType;
		return;
	}

	if ( m_bySyncType != bySyncType )
	{
		BYTE const byAddType = ( bySyncType & (~(m_bySyncType & bySyncType)) );// �߰��� �÷���
		BYTE const byRemoveType = ( m_bySyncType & (~bySyncType) ); // ������ �÷���
		m_bySyncType = bySyncType;

		BM::Stream kPacket( PT_U_G_NFY_SYNCTYPE_CHANGE, byAddType );
		kPacket.Push( byRemoveType );
		VNotify(&kPacket);
	}	
}


void CUnit::SetMutatorAbil(const SMissionMutatorAbil &pkMutationAbil)
{
	for(int i = 0; i < MAX_MUTATUR_ABIL_TYPE; i++)
	{
		int const iAbilType = pkMutationAbil.iType[i];
		int iAbilValue = pkMutationAbil.iValue[i];

		switch(iAbilType)
		{
		case AT_MUTATOR_HYDRA_SELF:
			{
				continue; // Used only for normal monster
			}break;
		case AT_R_MAX_HP:
		case AT_C_MAX_HP:
			{
				iAbilValue = (__int64)GetAbil(iAbilType) * iAbilValue;
				SetAbil(AT_MAX_HP, iAbilValue);
				SetAbil(AT_HP, iAbilValue);
			}break;
		case AT_R_PHY_ATTACK_MAX:
		case AT_C_PHY_ATTACK_MAX:
			{
				iAbilValue = (__int64)GetAbil(iAbilType) * iAbilValue;
				SetAbil(AT_PHY_ATTACK_MAX, iAbilValue);
			}break;
		case AT_R_PHY_ATTACK_MIN:
		case AT_C_PHY_ATTACK_MIN:
			{
				iAbilValue = (__int64)GetAbil(iAbilType) * iAbilValue;
				SetAbil(AT_PHY_ATTACK_MIN, iAbilValue);
			}break;
		case AT_HP:
		case AT_MAX_HP:
		default:
			{ // nothing
			}break;
		}

		SetAbil(iAbilType, iAbilValue);
	}
}

bool CUnit::AddClientProjectile(int const iInstanceID, SClientProjectile const & kProjectile)
{
	auto ibRet = m_kClientProjectile.insert(std::make_pair(iInstanceID, kProjectile));
	if (false == ibRet.second)
	{
		return false;
	}
	return true;
}

HRESULT CUnit::CheckClientProjectile(int const iActionID, int const iInstanceID, BYTE byIndex, POINT3 const&  ptTarget, int const iUnitSizeXY,
									 bool const bCheckUsed) const
{
	CONT_CLIENT_PROJECTILE::const_iterator itor = m_kClientProjectile.find(iInstanceID);
	if (m_kClientProjectile.end() == itor)
	{
		return E_ANTIHACK_CANNOTFIND;
	}

	SClientProjectile const& rkProjectile = (*itor).second;
	if (rkProjectile.iActionID != iActionID)
	{
		return E_ANTIHACK_MISMATCH_ID;
	}
	
	if ( byIndex >= rkProjectile.byMaxProjectile)
	{
		return E_ANTIHACK_MAX_OVER;
	}
	
	if (bCheckUsed && ( rkProjectile.byMaxTargetPerProjectile <= rkProjectile.kUsed[byIndex]))
	{
		if (rkProjectile.byMaxTargetPerProjectile <= rkProjectile.kUsed[byIndex])
		{
			return E_ANTIHACK_USED;
		}
		++rkProjectile.kUsed[byIndex];
	}

	// Range �˻�
	POINT3 ptDist = ptTarget - rkProjectile.ptFirePos;
	float fDistQ = ptDist.x * ptDist.x + ptDist.y * ptDist.y + ptDist.z * ptDist.z - pow( static_cast<float>(iUnitSizeXY), 2.0f ) * 3.0f;
	if (fDistQ  > rkProjectile.fDmgRangeQ)
	{
		return E_ANTIHACK_DISTANCE;
	}

	// Angle �˻�
	// Lag ������ ������ ���� �ٵ�... �ϴ� ������...

	return S_OK;
}

void CUnit::ClientProjectileTimeout(DWORD const dwNowTime)
{
	// �����ð� ���� Projectile ���� ������ �ֱ�
	bool bClear = false;
	CONT_CLIENT_PROJECTILE::iterator itor = m_kClientProjectile.begin();
	while (m_kClientProjectile.end() != itor )
	{
		if ((*itor).second.dwExpireTime > dwNowTime)
		{
			// �ð�������� ���ĵǾ� �ִٰ� �� �� �ִ�.
			if (itor != m_kClientProjectile.begin())
			{
				--itor;	// ���� �Ѵܰ� ������ �̵����� �־�� �Ѵ�.
				m_kClientProjectile.erase(m_kClientProjectile.begin(), itor);
			}
			return;
		}
		++itor;
		bClear = true;
	}

	// ��� ������ �Ѵ�.
	if (bClear)
	{
		m_kClientProjectile.clear();
	}
}

int CUnit::GetDiffEnchantLevel(CUnit* const pkDefenceUnit) const // �������� EnchantLevel�� ������ + �� / ������ - �� ����
{
	if(GetAbil(AT_MON_ENCHANT_LEVEL) || pkDefenceUnit->GetAbil(AT_MON_ENCHANT_LEVEL))
	{
		int const iOffenceEnchantLevel = GetAbil(AT_OFFENCE_ENCHANT_LEVEL);
		int const iDefenceEnchantLevel = pkDefenceUnit->GetAbil(AT_DEFENCE_ENCHANT_LEVEL);		

		return iOffenceEnchantLevel - iDefenceEnchantLevel;
	}
	else
	{
		return 0;
	}

	return 0;
}

void CUnit::GetObserverMemberList( CONT_GUID_LIST &rkContGuidList )const
{
	PgNetModule<>::CONT_NETMODULE::const_iterator net_itr = m_kContObserver.begin();
	for ( ; net_itr != m_kContObserver.end() ; ++net_itr )
	{
		rkContGuidList.push_back( net_itr->GetID() );
	}
}
void CUnit::GetObserverMemberList(CONT_OBSERVER_SWITH_MEMBER_LIST& rkContOut) const
{
	PgNetModule<>::CONT_NETMODULE::const_iterator net_itr = m_kContObserver.begin();
	for ( ; net_itr != m_kContObserver.end() ; ++net_itr )
	{
		rkContOut.push_back( std::make_pair( net_itr->GetNet(), net_itr->GetID() ) );
	}
}

void CUnit::InvenRefreshAbil()
{
	m_kInv.RefreshAbil(this);	
}

POINT3 CUnit::GetDirectionVector(BYTE byDirection)
{
	bool bLeft = ((byDirection & DIR_LEFT) == 0 ? false : true);
	bool bRight = ((byDirection & DIR_RIGHT) == 0 ? false : true);
	bool bUp = ((byDirection & DIR_UP) == 0 ? false : true);
	bool bDown = ((byDirection & DIR_DOWN) == 0 ? false : true);

	POINT3 kMovingDir(0.0f,0.0f,0.0f);
	static POINT3 const UNIT_Z(0.0f,0.0f,1.0f);
	POINT3 const kTempPathNormal(m_kPathNormal.x, m_kPathNormal.y, m_kPathNormal.z);

	if(bLeft || bRight)
	{
		kMovingDir += kTempPathNormal.Cross(bLeft ?  -UNIT_Z: UNIT_Z);
	}

	if(bUp || bDown)
	{
		kMovingDir += (bUp ? m_kPathNormal : -m_kPathNormal);
	}

	kMovingDir.Normalize();
	return kMovingDir;
}

WORD CUnit::GetBasicElementDmgRateAbil(WORD const wAbil)
{
	if( AT_ATTACK_ADD_FIRE <= wAbil && AT_ATTACK_ADD_DESTROY >= wAbil) { return (wAbil+10); }
	if( AT_RESIST_ADD_FIRE <= wAbil && AT_RESIST_ADD_DESTROY >= wAbil) { return (wAbil+10); }	
	return 0;
}

WORD CUnit::GetBasicElementAbilTotal(WORD const wAbil)
{
	if( AT_ATTACK_ADD_FIRE <= wAbil && AT_ATTACK_ADD_DESTROY >= wAbil) { return AT_ATTACK_ADD_5ELEMENT; }
	if( AT_RESIST_ADD_FIRE <= wAbil && AT_RESIST_ADD_DESTROY >= wAbil) { return AT_RESIST_ADD_5ELEMENT; }		
	return 0;
}

WORD CUnit::GetBasicElementRateAbilTotal(WORD const wAbil)
{
	if( AT_ATTACK_ADD_FIRE <= wAbil && AT_ATTACK_ADD_DESTROY >= wAbil) { return AT_R_ATTACK_ADD_5ELEMENT; }
	if( AT_RESIST_ADD_FIRE <= wAbil && AT_RESIST_ADD_DESTROY >= wAbil) { return AT_R_RESIST_ADD_5ELEMENT; }		
	return 0;
}

WORD CUnit::GetConvertElementRateAbilTotal(WORD const wAbil)
{
	if( AT_R_ATTACK_ADD_FIRE_PHY <= wAbil && AT_R_ATTACK_ADD_DESTROY_PHY >= wAbil ) { return AT_R_ATTACK_ADD_5ELEMENT_PHY; }
	if( AT_R_ATTACK_ADD_FIRE_MAGIC <= wAbil && AT_R_ATTACK_ADD_DESTROY_MAGIC >= wAbil ) { return AT_R_ATTACK_ADD_5ELEMENT_MAGIC; }
	if( AT_R_RESIST_ADD_FIRE_PHY <= wAbil && AT_R_RESIST_ADD_DESTROY_PHY >= wAbil ) { return AT_R_RESIST_ADD_5ELEMENT_PHY; }
	if( AT_R_RESIST_ADD_FIRE_MAGIC <= wAbil && AT_R_RESIST_ADD_DESTROY_MAGIC >= wAbil ) { return AT_R_RESIST_ADD_5ELEMENT_MAGIC; }

	return 0;
}

bool CUnit::Is4ElementAbil(WORD const wAbil)
{
	switch( wAbil )
	{
	case AT_ATTACK_ADD_DESTROY:
	case AT_RESIST_ADD_DESTROY:
	case AT_R_ATTACK_ADD_DESTROY_PHY:
	case AT_R_RESIST_ADD_DESTROY_PHY:
	case AT_R_ATTACK_ADD_DESTROY_MAGIC:
	case AT_R_RESIST_ADD_DESTROY_MAGIC:
		{
			return false;
		}break;
	default:
		{
		}break;
	}
	return true;
}

int CUnit::GetBasicElementDmg(WORD const wAbil, int const iAddValue, 
							  EGetAbilType::Enum const getabiltype) const
{
	int iValue = 0;
	if( IsBasicElementDmgAbilRange(wAbil) )
	{
		WORD wBasic = wAbil;
		WORD wBasic5 = CUnit::GetBasicElementAbilTotal(wAbil);
		WORD wBasic4 = (CUnit::Is4ElementAbil(wAbil)) ? std::max<WORD>(wBasic5 - 1, 0) : 0;
		WORD wRate = CUnit::GetBasicElementDmgRateAbil(wAbil);
		WORD wRate5 = CUnit::GetBasicElementRateAbilTotal(wAbil);
		WORD wRate4 = (CUnit::Is4ElementAbil(wAbil)) ? std::max<WORD>(wRate5 - 1, 0) : 0;
		int iRate = 0;

		if(EGetAbilType::EGAT_BASE & getabiltype)
		{
			iValue += (GetAbil(wBasic) + GetAbil(wBasic4) + GetAbil(wBasic5));
		}
		if(EGetAbilType::EGAT_INV & getabiltype)
		{
			iValue += (m_kInv.GetAbil(wBasic) + m_kInv.GetAbil(wBasic4) + m_kInv.GetAbil(wBasic5));
		}

		//������ ��ü�� ���ؼ� ����ؾ� �Ѵ�.
		iRate += (GetAbil(wRate) + GetAbil(wRate4) + GetAbil(wRate5));
		iRate += (m_kInv.GetAbil(wRate) + m_kInv.GetAbil(wRate4) + m_kInv.GetAbil(wRate5));

		iValue += iAddValue;

		iValue = iValue * (1 + (iRate / ABILITY_RATE_VALUE_FLOAT));
	}
	return iValue;
}

int CUnit::GetConvertElementalDmgRate(WORD const wAbil, int const iAddValue, 
									  EGetAbilType::Enum const getabiltype) const
{// ��ȯ���� ���з�
	int iValue = 0;	
	if( IsConvertElementalDmgRateAbilRange(wAbil) )
	{
		WORD wRate = wAbil;
		WORD wRate5 = CUnit::GetConvertElementRateAbilTotal(wAbil);
		WORD wRate4 = (CUnit::Is4ElementAbil(wAbil)) ? std::max<WORD>(wRate5 - 1, 0) : 0;

		if(EGetAbilType::EGAT_BASE & getabiltype)
		{
			iValue+=static_cast<int>(GetAbil(wRate) + GetAbil(wRate4) + GetAbil(wRate5));
		}
		if(EGetAbilType::EGAT_INV & getabiltype)
		{
			iValue+=static_cast<int>(m_kInv.GetAbil(wRate) + m_kInv.GetAbil(wRate4) + m_kInv.GetAbil(wRate5));
		}

		iValue = static_cast<int>(iAddValue * (iValue / ABILITY_RATE_VALUE_FLOAT));
	}
	return iValue;
}