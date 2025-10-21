#include "stdafx.h"
#include <NiCollision.h>
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/PacketStruct4Map.h"
#include "FCS/AntiHack.h"
#include "Variant/constant.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgEntity.h"
#include "Variant/PgControlDefMgr.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgMission.h"
#include "Variant/PgEventView.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/AlramMissionMgr.h"
#include "Global.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "AI/PgAISkillChooser.h"
#include "AI/EnemySelector.h"
#include "PgStoneMgr.h"
#include "PgQuest.h"
#include "SkillEffectAbilSystem\PgSkillAbilHandleManager.h"
#include "PgMissionGround.h"
#include "AI/PgTargettingManager.h"
#include "PgAction_Pet.h"
#include "publicmap/PgBSGround.h"

extern void OnAttacked(CUnit* pkUnit, UNIT_PTR_ARRAY* pkUnitArray, int const iSkillNo, PgActionResultVector* pkResult, SActArg* pArg, DWORD const dwTimeStamp);
bool IsDetailAI(CUnit *pkUnit);

namespace PgGroundUtil
{
	bool SortAbs(float const& lhs, float const& rhs)
	{
		return NiAbs(lhs) < NiAbs(rhs);
	}

	float Point2DCross(POINT3 const& lhs, POINT3 const& rhs)
	{
		return (lhs.x * rhs.y) - (lhs.y * rhs.x);
	}

	bool IsIn2DTriangle(POINT3 const& kP1, POINT3 const& kP2, POINT3 const& kP3, POINT3 const& kPos)
	{
		typedef std::vector< float > ContCross;
		ContCross kVec(3);
		kVec[0] = Point2DCross(kP2 - kP1, kP1 - kPos);
		kVec[1] = Point2DCross(kP3 - kP2, kP2 - kPos);
		kVec[2] = Point2DCross(kP1 - kP3, kP3 - kPos);
		std::sort(kVec.begin(), kVec.end(), SortAbs);
		if( 0 <= kVec[0] && 0 < kVec[1] && 0 < kVec[2] )
		{
			return true;
		}
		if( 0 >= kVec[0] && 0 > kVec[1] && 0 > kVec[2] )
		{
			return true;
		}
		return false;
	}
}

int PgGround::GetEntity(BM::GUID const &kCaller, int const iClass, UNIT_PTR_ARRAY& rkUnitArray)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_ENTITY, kItor);
	CUnit* pkUnit = NULL;
	int iCount = 0;
	while ((pkUnit = PgObjectMgr::GetNextUnit(UT_ENTITY, kItor)) != NULL)
	{
		PgEntity* pkEntity = dynamic_cast<PgEntity*> (pkUnit);
		if( (kCaller.IsNull() || pkEntity->Caller() == kCaller)
		 && (iClass==0 || pkEntity->GetAbil(AT_CLASS) == iClass))
		{
			rkUnitArray.Add(pkUnit);
			++iCount;
		}
	}
	return iCount;
}

void PgGround::DeletePet( BM::GUID const &kPetID )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	PgPet * pkPet = GetPet( kPetID );
	if ( pkPet )
	{
		PgPlayer * pkPlayer = GetUser( pkPet->Caller() );
		if (	pkPlayer 
			&&	(pkPlayer->CreatePetID() == pkPet->GetID()) 
		)
		{		
			PgActPet_RefreshPassiveSkill::DoAction_ReleaseTarget( pkPet, pkPlayer, this );
			ReleaseUnit(pkPet);
			pkPlayer->CreatePetID( BM::GUID::NullData() );
		}
		else
		{
			ReleaseUnit(pkPet);
		}
	}
}

PgPet* PgGround::CreatePet( PgPlayer * pkCaller, BM::GUID const &kPetID, CONT_PET_MAPMOVE_DATA::mapped_type &kPetData )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if ( ms_kOffPet_GroundAttr & this->GetAttr() )
	{
		return NULL;
	}

	PgBase_Item kPetItem;
	HRESULT const hRet = pkCaller->GetInven()->GetSelectedPetItem( kPetItem );

	{
		PgPet *pkOldPet = GetPet( pkCaller );
		if ( pkOldPet )
		{
			if ( pkOldPet->GetID() == kPetItem.Guid() )
			{
				// �ð��� ���ŵǾ����� Ȯ��
				__int64 const i64LimitTime = kPetItem.GetDelTime();
				if ( 0i64 < i64LimitTime )
				{
					if ( pkOldPet->GetPeriodEndSecTime() != i64LimitTime )
					{
						pkOldPet->SetPeriodEndSecTime( i64LimitTime );
					}
				}
				return pkOldPet;
			}

			PgActPet_RefreshPassiveSkill::DoAction_ReleaseTarget( pkOldPet, pkCaller, this );
			ReleaseUnit( pkOldPet );
			pkCaller->CreatePetID( BM::GUID::NullData() );//�� �ʱ�ȭ
		}
	}
	
	if ( S_OK == hRet )
	{// �κ����� ���� �����Ϸ� �ϴ� �� �������� ���� �ϰ�
		if ( kPetID != kPetItem.Guid() )
		{
			return NULL;
		}

		if ( !kPetItem.IsUseTimeOut() )
		{// ���� ������ �� ���� �ʾҰ�
			PgItem_PetInfo *pkPetInfo = NULL;
			if ( true == kPetItem.GetExtInfo( pkPetInfo ) )
			{// �߰� ������ ������ �����Ѵٸ�, UT_PETŸ�� ������ �� ������ GUID�� �����ϰ�
				PgPet *pkPet = dynamic_cast<PgPet*>(g_kTotalObjMgr.CreateUnit(UT_PET, kPetItem.Guid() ) );
				if ( pkPet )
				{
					if ( SUCCEEDED(pkPet->Create( kPetItem, pkCaller, kPetData )) )
					{// LV, CLASS_NO, �̸�, ��ų����, ��Ż(����?), �ｺ(�ǰ�?), �κ�, ����, State(US_IDLE), FrontDirection, OwnerType ���� ������
						pkPet->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );	// ?
						if ( true == AddUnit( pkPet, true ) )
						{
							pkCaller->CreatePetID( pkPet->GetID() );

							UNIT_PTR_ARRAY kAddUnitArray;
							kAddUnitArray.Add(pkPet);
							BM::Stream kAddPacket(PT_M_C_ADD_UNIT);
							kAddUnitArray.WriteToPacket( kAddPacket, WT_DEFAULT );
							pkCaller->Send( kAddPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );

							if ( !pkCaller->IsMapLoading() )
							{
								pkPet->SetAddPetToOwner();
							}
							return pkPet;
						}
					}

					g_kTotalObjMgr.ReleaseUnit( dynamic_cast<CUnit*>(pkPet) );
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Cannot CreatePet Guid["<<kPetItem.Guid()<<L"]");
				}
			}// GetExtInfo
		}// IsTimeOut

		BM::Stream kCPacket( PT_M_C_NFY_PET_INVENTORY, kPetItem.Guid() );
		kPetData.kInventory.WriteToPacket( kCPacket, WT_DEFAULT );
		pkCaller->Send( kCPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );//�κ������� �����־�� �Ѵ�.
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

CUnit* PgGround::CreateEntity(CUnit* pkCaller, SCreateEntity* pkCreateInfo, LPCTSTR lpszName, bool bSyncUnit)
{
	BM::CAutoMutex kLock( m_kRscMutex );

	PgEntity *pkEntity = dynamic_cast<PgEntity*>(g_kTotalObjMgr.CreateUnit(UT_ENTITY, pkCreateInfo->kGuid));
	if (pkEntity == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot CreateEntity Guid["<<pkCreateInfo->kGuid<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	//VERIFY_INFO_LOG_RETURN(NULL, VPARAM(pkEntity != NULL, BM::LOG_LV2, _T("[%s] Cannot Create Entity ...Guid[%s]"),
	//	__FUNCTIONW__, kEntityGuid.str().c_str()));

	SEntityInfo kInfo(pkCreateInfo->kGuid, pkCreateInfo->kClassKey);
	kInfo.bSyncUnit = bSyncUnit;
	kInfo.bEternalLife = pkCreateInfo->bEternalLife;

	Direction eFrontDir = DIR_DOWN;
	if ( pkCaller )
	{
		kInfo.kCaller = pkCaller->GetID();
		kInfo.kCallerClassKey.iClass = pkCaller->GetAbil(AT_CLASS);
		kInfo.SetAbil(AT_TEAM, pkCaller->GetAbil(AT_TEAM));
		kInfo.SetAbil(AT_PHY_ATTACK_MIN, pkCaller->GetAbil(AT_PHY_ATTACK_MIN));
		kInfo.SetAbil(AT_PHY_ATTACK_MAX, pkCaller->GetAbil(AT_PHY_ATTACK_MAX));
		kInfo.SetAbil(AT_MAGIC_ATTACK_MIN, pkCaller->GetAbil(AT_MAGIC_ATTACK_MIN));
		kInfo.SetAbil(AT_MAGIC_ATTACK_MAX, pkCaller->GetAbil(AT_MAGIC_ATTACK_MAX));
		kInfo.SetAbil(AT_C_PHY_ATTACK_MIN, pkCaller->GetAbil(AT_C_PHY_ATTACK_MIN));
		kInfo.SetAbil(AT_C_PHY_ATTACK_MAX, pkCaller->GetAbil(AT_C_PHY_ATTACK_MAX));
		kInfo.SetAbil(AT_C_MAGIC_ATTACK_MIN, pkCaller->GetAbil(AT_C_MAGIC_ATTACK_MIN));
		kInfo.SetAbil(AT_C_MAGIC_ATTACK_MAX, pkCaller->GetAbil(AT_C_MAGIC_ATTACK_MAX));
		kInfo.SetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MIN, pkCaller->GetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MIN) );
		kInfo.SetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MAX, pkCaller->GetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MAX) );
		kInfo.SetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN, pkCaller->GetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN) );
		kInfo.SetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX, pkCaller->GetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX) );
		kInfo.SetAbil(AT_C_ABS_ADDED_DMG_PHY, pkCaller->GetAbil(AT_C_ABS_ADDED_DMG_PHY) );
		kInfo.SetAbil(AT_C_ABS_ADDED_DMG_MAGIC, pkCaller->GetAbil(AT_C_ABS_ADDED_DMG_MAGIC) );		
		kInfo.SetAbil(AT_HIT_SUCCESS_VALUE, pkCaller->GetAbil(AT_HIT_SUCCESS_VALUE));
		kInfo.SetAbil(AT_OWNER_TYPE, pkCaller->GetAbil(AT_OWNER_TYPE));
		kInfo.SetAbil(AT_CALLER_TYPE, pkCaller->UnitType());
		kInfo.SetAbil(AT_CALLER_LEVEL, pkCaller->GetAbil(AT_LEVEL));
		kInfo.SetAbil(AT_PHY_DMG_PER, pkCaller->GetAbil(AT_PHY_DMG_PER));
		kInfo.SetAbil(AT_MAGIC_DMG_PER, pkCaller->GetAbil(AT_MAGIC_DMG_PER));
		kInfo.SetAbil(AT_DUEL, pkCaller->GetAbil(AT_DUEL));
		
		{// ũ��Ƽ�� ����� �����
			kInfo.SetAbil(AT_C_CRITICAL_SUCCESS_VALUE, pkCaller->GetAbil(AT_C_CRITICAL_SUCCESS_VALUE));
			kInfo.SetAbil(AT_CRITICAL_ONEHIT, pkCaller->GetAbil(AT_CRITICAL_ONEHIT));
			kInfo.SetAbil(AT_C_CRITICAL_POWER, pkCaller->GetAbil(AT_C_CRITICAL_POWER));
		}
		
		if(0 < ms_iEnableEntityCritialAttack)
		{
			kInfo.SetAbil(AT_C_CRITICAL_SUCCESS_VALUE, pkCaller->GetAbil(AT_C_CRITICAL_SUCCESS_VALUE));
			kInfo.SetAbil(AT_C_CRITICAL_POWER, pkCaller->GetAbil(AT_C_CRITICAL_POWER));
		}

		if(0 < ms_iEnableEntityHitRate)
		{
			kInfo.SetAbil(AT_C_HIT_SUCCESS_VALUE, pkCaller->GetAbil(AT_C_HIT_SUCCESS_VALUE));
			kInfo.SetAbil(AT_DEC_TARGETDODGE_ABS, pkCaller->GetAbil(AT_DEC_TARGETDODGE_ABS));
			kInfo.SetAbil(AT_DEC_TARGETDODGE_RATE, pkCaller->GetAbil(AT_DEC_TARGETDODGE_RATE));
		}

		if(0 < ms_iEnableEntityAttackExtInfo)
		{
			kInfo.SetAbil(AT_1ST_ATTACK_ADDED_RATE, pkCaller->GetAbil(AT_1ST_ATTACK_ADDED_RATE));
			kInfo.SetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM, pkCaller->GetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM));
			kInfo.SetAbil(AT_PHY_DEFENCE_IGNORE_RATE, pkCaller->GetAbil(AT_PHY_DEFENCE_IGNORE_RATE));
			kInfo.SetAbil(AT_MAGIC_DEFENCE_IGNORE_RATE, pkCaller->GetAbil(AT_MAGIC_DEFENCE_IGNORE_RATE));
			kInfo.SetAbil(AT_ABS_ADDED_DMG_PHY_2, pkCaller->GetAbil(AT_ABS_ADDED_DMG_PHY_2));
			kInfo.SetAbil(AT_ABS_ADDED_DMG_MAGIC_2, pkCaller->GetAbil(AT_ABS_ADDED_DMG_MAGIC_2));
			kInfo.SetAbil(AT_GRADE_NORMAL_DMG_ADD_RATE, pkCaller->GetAbil(AT_GRADE_NORMAL_DMG_ADD_RATE));
			kInfo.SetAbil(AT_GRADE_UPGRADED_DMG_ADD_RATE, pkCaller->GetAbil(AT_GRADE_UPGRADED_DMG_ADD_RATE));
			kInfo.SetAbil(AT_GRADE_ELITE_DMG_ADD_RATE, pkCaller->GetAbil(AT_GRADE_ELITE_DMG_ADD_RATE));
			kInfo.SetAbil(AT_GRADE_BOSS_DMG_ADD_RATE, pkCaller->GetAbil(AT_GRADE_BOSS_DMG_ADD_RATE));			
		}

		kInfo.SetAbil(AT_ENABLE_CHECK_ATTACK, pkCaller->GetAbil(AT_ENABLE_CHECK_ATTACK));
				
		//kInfo.SetAbil(AT_CRITICAL_ONEHIT, pkCaller->GetAbil(AT_CRITICAL_ONEHIT));

		if ( 0 < pkCaller->GetAbil(AT_USENOT_SMALLAREA) )
		{// Caller�� BigArea�����̸� ��ȯ���� BigArea�� ���� �Ѵ�.
			kInfo.SetAbil( AT_USENOT_SMALLAREA, 1 );
		}
		if( 0 < pkCaller->GetAbil(AT_MON_ENCHANT_LEVEL) )
		{
			kInfo.SetAbil(AT_MON_ENCHANT_LEVEL, pkCaller->GetAbil(AT_MON_ENCHANT_LEVEL));
		}
		kInfo.SetAbil(AT_OFFENCE_ENCHANT_LEVEL, pkCaller->GetAbil(AT_OFFENCE_ENCHANT_LEVEL));
		kInfo.SetAbil(AT_DEFENCE_ENCHANT_LEVEL, pkCaller->GetAbil(AT_DEFENCE_ENCHANT_LEVEL));

		eFrontDir = pkCaller->FrontDirection();

		//if(pkCaller)
		//{// �̰����� �ɸ� �ʿ��� ������ ����Ʈ�� �ɷ��ִ� ���·� ���޵��� ����
		//	PgUnitEffectMgr& rkEffectMgr = pkCaller->GetEffectMgr();
		//	CEffect* pkEffect = NULL;
		//	ContEffectItor kItor;
		//	rkEffectMgr.GetFirstEffect(kItor);
		//	while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
		//	{
		//		if(pkEffect
		//			&& GlobalHelper::IsAdjustSkillEffect(pkEffect)
		//			)
		//		{// Caller���� �ɷ��ִٸ� �Ȱ��� Entity���Ե� �������ش�.
		//			SEffectCreateInfo kCreate;
		//			kCreate.eType = EFFECT_TYPE_NORMAL;
		//			kCreate.iEffectNum = pkEffect->GetEffectNo();
		//			kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
		//			pkEntity->AddEffect(kCreate);
		//		}
		//	}
		//}
	}
	else
	{
		kInfo.SetAbil(AT_OWNER_TYPE, UT_MONSTER);
		kInfo.SetAbil(AT_CALLER_TYPE, UT_MONSTER);
		kInfo.SetAbil(AT_CALLER_LEVEL, 0);
	}

	// pkCaller�� ���̶�� ���� ������ caller�� �Ǿ�� �Ѵ�.
	// ���� �ؿ� �ڵ� �κ�
	if(pkCaller)
	{
		if(pkCaller->IsUnitType(UT_PET))
		{
			CUnit* pkCallerUnit = GetUnit(pkCaller->Caller());

			//���� user�� ���´�.
			if(pkCallerUnit)
			{
				//���� ������ Entity�� Caller�� �������ش�.
				pkCaller = pkCallerUnit;
				kInfo.kCaller = pkCaller->GetID();
			}
		}
	}
	
	if ( SUCCEEDED(pkEntity->Create( &kInfo )) )
	{
		if ( lpszName )
		{
			pkEntity->Name( lpszName );
		}
		if ( pkCreateInfo->iLifeTime )
		{
			pkEntity->LifeTime(pkCreateInfo->iLifeTime);
		}
		pkEntity->SetPos( pkCreateInfo->ptPos );
		pkEntity->SetState(US_IDLE);
		pkEntity->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );
		pkEntity->FrontDirection( eFrontDir );

		if ( !pkCaller || pkCaller->AddSummonUnit( pkEntity->GetID(), pkCreateInfo->kClassKey.iClass, PgGroundUtil::GetSummonOptionType(pkCreateInfo->bUniqueClass), GetMaxSummonUnitCount(pkCaller)) )
		{
			AddUnit(pkEntity, true);

			//��ų�� ����Ҷ� �ڽ� ��ų�� ������ �޴� ��ų�� ����ؾ��ϴ� ���
			//Entity�� Entitny�� ���� ������ �ش�. 
			//Variant�� CS_GetSkillResultDefault ���� �Լ� ȣ�� �� Entity�� Caller�� �����͸� ���� �� ���� ������
			//Caller �� ������ �޴� �ڽ� ��ų�� ������� �� ���� ����. 
			//Entity�� ���� ������ �־ �����ؾ� �Ѵ�.			
			
			if(pkEntity && pkCaller)
			{
				if(int const iSkillNo = pkEntity->GetAbil(AT_MON_SKILL_01))
				{
					if(0 < iSkillNo)
					{
						GET_DEF(CSkillDefMgr, kSkillDefMgr);
						CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
						if(pkSkill)
						{
							if(PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaller))
							{
								int iSkillindex = 0;
								while(CHILD_SKILL_MAX > iSkillindex)
								{
									//���� �޴� ���̽� ��ų
									int const iChildBaseSkillNo = pkSkill->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
									if(0 < iChildBaseSkillNo)
									{
										if(PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill())
										{
											//�ش� ��ų�� ���� ������ �ش�Ǵ� ��ų�� ��� �´�.
											int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
											if(0 < iLearnedChildSkillNo)
											{
												//���� ��ų ��ȣ�� Entity�� �����Ѵ�.
												//Variant�� CS_GetSkillResultDefault ���� �Լ��� ȣ��ɶ� Caller�� �����͸� ���� ��� �� �� ����.
												pkEntity->SetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex, iLearnedChildSkillNo);												
											}
										}
									}
									else
									{
										break;
									}
									++iSkillindex;
								}
							}
						}
					}
				}
			}

			return pkEntity;
		}
	}

	g_kTotalObjMgr.ReleaseUnit( dynamic_cast<CUnit*>(pkEntity) );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

/*
CUnit* PgGround::CreateEntity(CUnit* pkCaller, int iClass, short int iLevel, LPCTSTR lpszName, DWORD dwDistance)
{
	VERIFY_INFO_LOG_RETURN(NULL, VPARAM(pkCaller != NULL, BM::LOG_LV4, _T("[%s] Caller is NULL"), __FUNCTIONW__));
	BM::GUID kEntityGuid = BM::GUID::Create();
	PgEntity *pkEntity = dynamic_cast<PgEntity*>(g_kTotalObjMgr.CreateUnit(UT_ENTITY, kEntityGuid));
	VERIFY_INFO_LOG_RETURN(NULL, VPARAM(pkEntity != NULL, BM::LOG_LV2, _T("[%s] Cannot Create Entity ...Guid[%s]"),
		__FUNCTIONW__, kEntityGuid.str().c_str()));

	SEntityInfo kInfo;
	kInfo.kGuid = kEntityGuid;
	kInfo.kCaller = pkCaller->GetID();
	SClassKey kClassKey(iClass, iLevel);
	kInfo.ulLifeTime = g_ClassDef.GetAbil(kClassKey, AT_LIFETIME);
	kInfo.iClass = iClass;
	kInfo.sLevel = iLevel;
	kInfo.SetAbil(AT_TEAM, pkCaller->GetAbil(AT_TEAM));
	kInfo.SetAbil(AT_PHY_ATTACK_MIN, pkCaller->GetAbil(AT_PHY_ATTACK_MIN));
	kInfo.SetAbil(AT_PHY_ATTACK_MAX, pkCaller->GetAbil(AT_PHY_ATTACK_MAX));
	kInfo.SetAbil(AT_MAGIC_ATTACK_MIN, pkCaller->GetAbil(AT_MAGIC_ATTACK_MIN));
	kInfo.SetAbil(AT_MAGIC_ATTACK_MAX, pkCaller->GetAbil(AT_MAGIC_ATTACK_MAX));
	kInfo.SetAbil(AT_HIT_SUCCESS_VALUE, pkCaller->GetAbil(AT_HIT_SUCCESS_VALUE));
	
	pkEntity->Create( &kInfo );
	pkEntity->Name( lpszName );

	// YonMy 20070824 Party Skill.. Caller�� PartyGuid�� ����
	// Entity ��ġ ����ϱ�
	POINT3 ptEntityPos = pkCaller->GetPos();
	GetDistanceToPosition( GetPathRoot(), pkCaller->GetPos(), pkCaller->FrontDirection(), dwDistance, ptEntityPos );
	// �ٴڿ� ���� ��Ų��
	NxRay kRay(NxVec3(ptEntityPos.x, ptEntityPos.y, ptEntityPos.z+20), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
	if(pkHitShape)
	{
		ptEntityPos.z = kHit.worldImpact.z;
	}

	pkEntity->SetPos( ptEntityPos );
	pkEntity->SetState(US_IDLE);

	AddUnit(pkEntity, true);
	return pkEntity;
}
*/


PgSubPlayer* PgGround::CreateSubPlayer( PgPlayer * pkCaller, BM::GUID const& rkSubPlayerID )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	{// �ֵ��� ĳ����
		int const iClass = pkCaller->GetAbil(AT_CLASS);
		if( IsClass_OwnSubPlayer(iClass) )
		{//�̶��
			{
				PgSubPlayer* pkOldSubPlayer = GetSubPlayer(pkCaller);	// ���� ����ĳ���͸� ������
				pkCaller->SubPlayerID( BM::GUID::NullData() );	// ����ü�� �����Ǳ����� NULL GUID�� �������ְ�
				if(pkOldSubPlayer)
				{// �̹� �ִٸ� �����ϰ� �ٽ� ����
					pkOldSubPlayer->SetCallerUnit(NULL);
					ReleaseUnit( pkOldSubPlayer );
				}
			}
			
			PgSubPlayer* pkSubPlayer = dynamic_cast<PgSubPlayer*>(g_kTotalObjMgr.CreateUnit(UT_SUB_PLAYER, rkSubPlayerID) );
			if(pkSubPlayer)
			{
				pkCaller->SubPlayerID(rkSubPlayerID);
				pkSubPlayer->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );	// ?
				pkSubPlayer->Create(pkCaller);

				AddUnit(pkSubPlayer);

				UNIT_PTR_ARRAY kAddUnitArray;
				kAddUnitArray.Add(pkSubPlayer);
				BM::Stream kAddPacket(PT_M_C_ADD_UNIT);
				kAddUnitArray.WriteToPacket( kAddPacket, WT_DEFAULT );
				pkCaller->Send( kAddPacket, E_SENDTYPE_BROADALL|E_SENDTYPE_MUSTSEND );

				if ( !pkCaller->IsMapLoading() )
				{
					//pkSubPlayer->SetAddPetToOwner(); // �ƿĿĿ�
				}
				return pkSubPlayer;
			}
		}
	}
	return NULL;
}

void PgGround::DeleteSubPlayer( BM::GUID const& rkSubPlayerID )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	PgSubPlayer * pkSubPlayer = GetSubPlayer( rkSubPlayerID );
	if ( pkSubPlayer )
	{
		PgPlayer * pkPlayer = GetUser( pkSubPlayer->Caller() );
		pkSubPlayer->SetCallerUnit(NULL);
		if ( pkPlayer
			&&	(pkPlayer->SubPlayerID() == pkSubPlayer->GetID()) 
		)
		{
			pkPlayer->SubPlayerID( BM::GUID::NullData() );
		}
		ReleaseUnit(pkSubPlayer);
	}
}

bool PgGround::Locked_IsUnitInRange(ETeam const eTeam, POINT3 const& ptFind, int const iRange, EUnitType const eType)
{
	// Ground ��ü���� ã�´�.
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;
	while ((pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		if( pkUnit->GetAbil(AT_TEAM)==eTeam &&
			IsUnitInRangeImp(pkUnit, ptFind, iRange, eType, iRange, false) )
		{
			return true;
		}
	}
	return false;
}

void PgGround::GetUnitInDistance( POINT3 const& ptPos, int const iRange, EUnitType const eType, UNIT_PTR_ARRAY& rkUnitArray, int const iZLimit)const
{
	// Ground ��ü���� ã�´�.
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;
	while ((pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		float const fDistanceQ = GetDistanceQ(ptPos, pkUnit->GetPos());
		if (fDistanceQ <= (float)iRange * iRange)
		{
			if(iZLimit > 0)
			{
				POINT3 const ptDist = ptPos - pkUnit->GetPos();
				if (abs(ptDist.z) > iZLimit)
				{
					++kItor;
					continue;
				}
			}

			rkUnitArray.Add(pkUnit);
		}
		++kItor;
	}
}

void PgGround::GetUnitInRange( POINT3 const& ptFind, int iRange, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray, int iZLimit, bool const bFindFromGround)
{
	// Ground ��ü���� ã�´�.
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;
	int iZ = (iZLimit == 0) ? iRange : iZLimit;

	while ((pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		CUnit* pRetUnit = IsUnitInRangeImp(pkUnit, ptFind, iRange, eType, iZ, bFindFromGround);
		if(NULL!=pRetUnit)
		{
			rkUnitArray.AddToUniqe(pRetUnit);
		}
	}
}

void PgGround::GetUnitInRange(EUnitType & eCheckUnitType, POINT3 const& FromPos, int const iRange, EUnitType const eUnitType, UNIT_PTR_ARRAY & rkUnitArray, int const iZLimit, bool const bFindFromGround)
{
	if(false == (eCheckUnitType&eUnitType))
	{
		GetUnitInRange(FromPos, iRange, eUnitType, rkUnitArray, iZLimit, bFindFromGround);
		eCheckUnitType = static_cast<EUnitType>(eCheckUnitType | eUnitType);
	}
}

CUnit * PgGround::IsUnitInRangeImp(CUnit *pUnit, POINT3 const& ptFind, int const iRange, EUnitType const eType, int const iZLimit, bool const bFindFromGround)
{
	if (NULL==pUnit)
	{
		return NULL;
	}
	int iApplyRange = iRange;
	if(UT_MONSTER == eType && pUnit->GetAbil(AT_GROWTH_SKILL_RANGE) )
	{
		iApplyRange = iRange + pUnit->GetAbil(AT_UNIT_SIZE_XY);
	}

	EInRangeRet eRet = ::IsInRange(GetUnitPos(pUnit), ptFind, iApplyRange, iZLimit, false);
	if(true == bFindFromGround && ERange_Z==eRet)	//Z�� ������ ��ã�� ��� �ɼ��� ������ �ѹ� �� ã�ƺ���
	{
		eRet = ::IsInRange(GetUnitPos(pUnit, true), ptFind, iApplyRange, iZLimit, false);
	}
	if(ERange_OK==eRet)
	{
		return pUnit;
	}
	return NULL;
}

void PgGround::GetUnitInCube( POINT3 const &ptCubeMin, POINT3 const &ptCubeMax, POINT3 const &ptPos, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;
	while ((pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		POINT3 ptPos = GetUnitPos(pkUnit);
		if ( IsInCube( ptCubeMin, ptCubeMax, ptPos ) )
		{
			rkUnitArray.Add(pkUnit);
		}
	}
}

CUnit* PgGround::GetUnitByClassNo( int const iClassNo, EUnitType eType )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;
	while ((pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		if( pkUnit->GetAbil(AT_CLASS) == iClassNo )
		{
			return pkUnit;
		}
	}

	return NULL;
}

int PgGround::GetUnitByClassNo( int const iClassNo, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;
	while ((pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		if( pkUnit->GetAbil(AT_CLASS) == iClassNo )
		{
			rkUnitArray.Add(pkUnit);
		}
	}
	return (static_cast<int>(rkUnitArray.size()));
}

CUnit* PgGround::GetUnitByType( EUnitType const eType )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	return PgObjectMgr::GetNextUnit(eType, kItor);
}

int PgGround::GetUnitByType( EUnitType const eType, UNIT_PTR_ARRAY& rkUnitArray )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;
	while ((pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		rkUnitArray.Add(pkUnit);
	}
	return (static_cast<int>(rkUnitArray.size()));
}

// Mutator

void PgGround::MakeMutatorAbil()
{
	CONT_DEF_MISSION_MUTATOR const * pkMutator = NULL;
	g_kTblDataMgr.GetContDef(pkMutator);
	if(!pkMutator)
	{
		return;
	}

	int iResultExpRate = 0;
	int iResultMoneyRate = 0;
	for(std::set<int>::iterator it = m_kGroundMutator.begin(); it != m_kGroundMutator.end(); it++)
	{
		CONT_DEF_MISSION_MUTATOR::const_iterator kMutator = pkMutator->find(*it);
		if(kMutator == pkMutator->end())
		{
			continue;
		}
		iResultExpRate += kMutator->second.iAddedExpRate;
		iResultMoneyRate += kMutator->second.iAddedMoneyRate;
	}
	SetMutatorAbil(AT_ADD_EXP_PER,iResultMoneyRate);
	SetMutatorAbil(AT_ADD_MONEY_PER, iResultMoneyRate);
}

void PgGround::SetMutatorAbil(const int iType, const int iValue)
{
	m_kMutatorBoostAbil.insert(std::make_pair(iType, iValue));
}

int PgGround::GetMutatorAbil(const int iType) const
{
	const MutatorBoostAbil::const_iterator it = m_kMutatorBoostAbil.find(iType);
	if(it != m_kMutatorBoostAbil.end())
	{
		return it->second;
	}
	return 0;
}

void PgGround::GetUnitInWidthFromLine(POINT3 const& ptStart, POINT3 const& ptEnd, int iWidth, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray)
{
	BM::CAutoMutex kLock( m_kRscMutex );
	
	// Unit Pos���� ptEnd - ptStart Vector�� �������� ���� ���̰� iWidth���� ������ Range�ȿ� �� ���̴�.
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;

	NiPoint3 kStart(ptStart.x, ptStart.y, ptStart.z);
	NiPoint3 kEnd(ptEnd.x, ptEnd.y, ptEnd.z);
	NiPoint3 kDirectionVec(kEnd - kStart);
	NiPoint3 kUnDirectionVec(kStart - kEnd);

	while ((pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		bool bInRange = false;
		POINT3 ptUnitPos = GetUnitPos(pkUnit);
		NiPoint3 kUnitPos(ptUnitPos.x, ptUnitPos.y, ptUnitPos.z);

		// ptPos���� kCenterLineVec���� ������ ���� ��, �� ���̰� iWidth���� ������ InRange!!
		NiPoint3 kProjection = (((kUnitPos - kStart).Dot(kDirectionVec)) / (kDirectionVec.Dot(kDirectionVec))) * kDirectionVec;
		float iLength = ((kUnitPos - kStart) - kProjection).Length();
		
		if(iWidth > iLength)
		{
			//���� �������ε� �˻簡 �Ǵ� ���� ����
			NiPoint3 kDif1 = kUnitPos-kStart;
			float kDot1 = kDirectionVec.Dot(kDif1);
			if(0.0f>kDot1)
			{
				continue;
			}

			NiPoint3 kDif2 = kUnitPos-kEnd;
			float kDot2 = kUnDirectionVec.Dot(kDif2);
			if(0.0f>kDot2)
			{
				continue;
			}

			rkUnitArray.Add(pkUnit);
		}
	}
}

void PgGround::GetUnitIn2DRectangle(POINT3 const& kP1, POINT3 const& kP2, POINT3 const& kP3, POINT3 const& kP4, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray)
{
	BM::CAutoMutex kLock( m_kRscMutex );
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eType, kItor);
	CUnit* pkUnit = NULL;

	bool bInArea = false;
	while( (pkUnit = PgObjectMgr::GetNextUnit(eType, kItor)) != NULL)
	{
		POINT3 kUnitPos = GetUnitPos(pkUnit);
		kUnitPos.z = 0;
		bInArea = PgGroundUtil::IsIn2DTriangle(kP1, kP2, kP3, kUnitPos);
		if( !bInArea )
		{
			bInArea = PgGroundUtil::IsIn2DTriangle(kP2, kP3, kP4, kUnitPos);
		}
		
		if( bInArea )
		{
			rkUnitArray.Add(pkUnit);
			bInArea = false;
		}
	}
}

POINT3 PgGround::GetUnitPos( CUnit* pkUnit, bool bCheckImpact, float const fRayRange )
{
	if (pkUnit->UnitType() == UT_PLAYER)
	{
		static int iValue = -1;
		if(0>iValue)
		{
			if(S_OK != g_kVariableContainer.Get(EVar_Kind_AI, UseRayCache, iValue) )
			{
				iValue = 1;
			}
		}
			
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		PLAYER_ACTION_INFO* pkAction = pkPlayer->GetPlayerActionInfo();
		DWORD const dwNow = BM::GetTime32();
		
		bool const bPosAtGroundTime = bCheckImpact && ((dwNow >= pkPlayer->PosAtGroundTime() + (MONSTER_AI_TICK_INTER-50)) || !iValue);
		if (dwNow >= pkAction->dwCalculateTime + MONSTER_AI_TICK_INTER || bPosAtGroundTime )
		{
			POINT3 ptNew = SimulateUnitPos2(pkPlayer, pkPlayer->GetPos(), pkAction->iActionID, dwNow-pkAction->dwLastTime, pkPlayer->Latency());
			pkAction->SetPos(dwNow+pkPlayer->Latency(), ptNew, true);
			if (POINT3::NullData() == pkPlayer->PosAtGround())
			{
				pkPlayer->PosAtGround(ptNew);
			}
			
			if (true==bCheckImpact)
			{
				if(ACTIONID_A_JUMP!=pkAction->iActionID)
				{
					bCheckImpact = false;
				}
				else
				{
					NxRay kRay(NxVec3(ptNew.x, ptNew.y, ptNew.z+20), NxVec3(0, 0, -1.0f));
					NxRaycastHit kHit;
					NxShape *pkHitShape = RayCast(kRay, kHit, fRayRange);

					pkPlayer->PosAtGroundTime(dwNow+pkPlayer->Latency());
					if(pkHitShape)
					{
						ptNew.z = kHit.worldImpact.z;
						pkPlayer->PosAtGround(ptNew);
						return ptNew;
					}
				}
			}
		}
		return bCheckImpact ? pkPlayer->PosAtGround() : pkAction->ptPos;
	}

	return pkUnit->GetPos();
}

bool PgGround::IsAttackable(EAbilType eType, CUnit* pkCaster, CUnit *pkTarget,int& iSkillNo)
{
	int iRand = BM::Rand_Index(100) ;
	if(pkCaster == NULL || pkTarget == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	if(pkTarget->IsDead())
	{
		return false; 
	}
	
	if (pkTarget->GetAbil(AT_UNIT_HIDDEN) > 0 || ( 0!=pkTarget->GetAbil(AT_CANNOT_DAMAGE)) )
	{
		// Hidden �����̰ų� AT_CANNOT_DAMAGE �����̸� ���� �� ����.
		return false;
	}
	POINT3 const &ptM = GetUnitPos(pkCaster);
	POINT3 const &ptC = GetUnitPos(pkTarget);
	if (abs(ptM.z - ptC.z) > 30)
	{
		return false;
	}

	float fDistDouble = GetDistanceQ(ptM, ptC);

	int iBaseDist = pkCaster->GetAbil(eType) ;
	if ( pkCaster->IsUnitType(UT_MONSTER) && iSkillNo == 0 ) // ���Ͱ� ��ų�� ���ٸ� ���⼭ ��ų�� �����ؾ� �Ѵ�.
	{
		iSkillNo = pkCaster->GetAbil(AT_NORMAL_SKILL_ID);
		
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		iBaseDist = kSkillDefMgr.GetDef(iSkillNo)->GetAbil(AT_ATTACK_RANGE) ;

		int iMaxSkill = pkCaster->GetAbil(AT_MAX_SKILL_NUM);
		for ( int i = 0 ; i < iMaxSkill ; i ++ )
		{
			int skillno = pkCaster->GetAbil(AT_MON_SKILL_01 + i) ;
			const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(skillno);
			if ( pkSkillDef )
			{
				int skilldist = pkSkillDef->GetAbil(AT_ATTACK_RANGE) ;
				if ( iBaseDist <= skilldist )
				{
					if ( iRand <= pkCaster->GetAbil(AT_MON_SKILL_RATE_01 + i) )
					{
						iBaseDist = skilldist ;
						iSkillNo = skillno ;
						break;
					}
				}
			}
		}


	}

	if(fDistDouble > iBaseDist*iBaseDist )
	{
		return false;
	}
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Enabled Attack Distance["<<sqrt(fDistDouble)<<L"], BaseDist["<<iBaseDist<<L"]");
#endif

	return true;
}

//bool PgGround::SkillFire(int const iSkillNo, CUnit* pkCaster, CUnit* pkTarget, PgActionResultVector* pkResult, SActArg* pkAct, bool bSendPacket)
//{
//	if ( !pkCaster )
//	{
//		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"pkCaster is NULL");
//		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
//		return false;
//	}
//
//	UNIT_PTR_ARRAY kTargetArray;
//	void* pkVoid = NULL;
//	pkAct->Get(ACTARG_GROUND, pkVoid);
//	PgGround* pkGround = (PgGround*)pkVoid;
//	if (NULL == pkTarget && pkGround != NULL)
//	{
//		pkGround->AI_GetSkillTargetList(pkCaster, iSkillNo, kTargetArray, true, pkAct);
//	}
//	else
//	{
//		kTargetArray.Add(pkTarget);
//	}
//
//	int iReturn = g_kSkillAbilHandleMgr.SkillFire(pkCaster, iSkillNo, pkAct, &kTargetArray, pkResult);
//
//	OnAttacked(pkCaster, &kTargetArray, iSkillNo, pkResult, pkAct);
//
//	if (bSendPacket && pkGround != NULL)
//	{
//		pkGround->SendNfyAIAction(pkCaster, US_ATTACK, pkResult);
//	}
//	return (iReturn > 0);
//}

bool NpcQuestEventFilter(const ContSimpleQuest::value_type& rkItem)
{
	switch(rkItem.cState)
	{
	case QS_Begin:
	case QS_End:
	case QS_Ing://��ȭ ������ ���µ�
		{
			return false;//���� ����
		}break;
	}
	return true;//����
}

bool ScrollQuestFilter( const ContSimpleQuest::value_type& rkItem )
{
	PgQuestInfo const *pkQuestInfo = NULL;
	g_kQuestMan.GetQuest(rkItem.iQuestID, pkQuestInfo);
	
	if( !pkQuestInfo )
	{
		return false;
	}

	if( QT_Scroll == pkQuestInfo->Type() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void PgGround::ReqNpcTrigger(CUnit* pkUnit, BM::GUID const& kObjectGuid, int iActionType)
{
	CUnit *pkNpcUnit = GetNPC(kObjectGuid);
	if( !pkNpcUnit )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find NPC Unit, GUID: "<<kObjectGuid<<L" Ground: "<<GetGroundNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkNpcUnit is NULL"));
		return;
	}

	PgNpc* pkNpc = dynamic_cast< PgNpc* >(pkNpcUnit);
	if( !pkNpc )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't NPC Unit[UnitType: "<<pkNpcUnit->UnitType()<<L", GUID: "<<pkNpcUnit->GetID()<<L", Name: "<<pkNpcUnit->Name()<<L"] Ground: "<<GetGroundNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkNpc is NULL"));
		return;
	}
	
	if( !PgGroundUtil::IsCanTalkableRange(pkUnit, kObjectGuid, pkNpc, GetGroundNo(), NMT_Quest, __FUNCTIONW__, __LINE__) )
	{
		return;
	}
	
	ContSimpleQuest kStateVec;
	PgCheckQuest kCheck(kStateVec);
	if( !kCheck.DoAction(pkUnit, pkNpc) )
	{
		return;
	}

	ContSimpleQuest::iterator new_iter = std::remove_if(kStateVec.begin(), kStateVec.end(), NpcQuestEventFilter);
	kStateVec.erase(new_iter, kStateVec.end());

	// ��ũ�� ����Ʈ�� NPC���� ����
	new_iter = std::remove_if(kStateVec.begin(), kStateVec.end(), ScrollQuestFilter);
	kStateVec.erase(new_iter, kStateVec.end());

	if( 1 == kStateVec.size() )
	{
		ContSimpleQuest::value_type const& rkElement = kStateVec.front();
		ReqIntroQuest(pkUnit, rkElement.iQuestID, kObjectGuid);
	}
	else if( !kStateVec.empty() )
	{
		//list�� �̹� Ŭ���̾�Ʈ�� �˰� ������ ������ �ʴ´�.
		SendShowQuestDialog(pkUnit, kObjectGuid, QSDT_SelectDialog, 0, QRDID_Close);
	}
}

// Monster���� �浹 ������ ...
void PgGround::ReqMonsterTrigger(CUnit* pkUnit, BM::GUID const & kObjectGuid, int iActionType)
{
	CUnit* pkCaster = PgObjectMgr::GetUnit(kObjectGuid);
	if (pkCaster == NULL || (!pkCaster->IsUnitType(UT_MONSTER) && !pkCaster->IsUnitType(UT_ENTITY)))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkCaster is NULL"));
		return;
	}

	PgAction_ReqCollisionDamage kAction(GroundKey(), this);
	kAction.DoAction(pkCaster, pkUnit);
}

// void PgGround::SendNoticeToAllUser(std::wstring wstrContent)
// {
// 	// ��� �������� ������ ������.
// 	BM::CAutoMutex Lock(m_kMutex);
// 
// 	PgPlayer* pkPlayer = NULL;
// 	CONT_OBJECT_MGR_UNIT::iterator kItor;
// 	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
// 	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
// 	{
// 		if (pkPlayer->UnitType() == UT_PLAYER)
// 		{
// 			// �������� ����.
// 			BM::Stream kNoticePacket(PT_M_C_NFY_CHAT, (BYTE)CT_NOTICE);
// 			kNoticePacket.Push(wstrContent);
// 			pkPlayer->Send(kNoticePacket);
// 		}
// 	}
// }

EOpeningState PgGround::ActivateMonsterGenGroup( int const iGenGroup, bool const bReset, bool const bOnce, int const iCopy )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	SGenGroupKey kGenGroupKey;
	GetGenGroupKey(kGenGroupKey);

	GET_DEF_CUSTOM( PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, kGenPointMgr);
	if ( false==bReset && true==bOnce )
	{
		ContGenPoint_Monster kContGenPoint;
		if( SUCCEEDED(kGenPointMgr.GetGenPoint_Monster( kContGenPoint, kGenGroupKey, bReset, iGenGroup )) )
		{
//			INFO_LOG( BM::LOG_LV6, _T("[%s]One GenPoint MapNo[%d], MapLevel[%d], bReset[%d], GenGroup[%d], Count[%u]"), __FUNCTIONW__, kGenGroupKey.iMapNo, GetMapLevel(), (int)bReset, iGenGroup, kContGenPoint.size());
			EOpeningState const eRet = MonsterGenerate( kContGenPoint, iCopy, NULL );

			ContGenPoint_Monster::const_iterator c_it = kContGenPoint.begin();
			while(c_it != kContGenPoint.end())
			{
				auto kRet = m_kContGenPoint_Monster.insert(std::make_pair((*c_it).first, (*c_it).second));
				if(false==kRet.second)
				{
					OwnChildCont kContChild;
					(*c_it).second.GetChild(kContChild);
					OwnChildCont::const_iterator it = kContChild.begin();
					while(it != kContChild.end())
					{
						kRet.first->second.AddChild( (*it) );
						++it;
					}
				}
				++c_it;
			}
			return eRet;
		}
	}
	else if ( bOnce )
	{
		ContGenPoint_Monster kContGenPoint;
		if( SUCCEEDED(kGenPointMgr.GetGenPoint_Monster( kContGenPoint, kGenGroupKey, bReset, iGenGroup )) )
		{
//			INFO_LOG( BM::LOG_LV6, _T("[%s]One GenPoint MapNo[%d], MapLevel[%d], bReset[%d], GenGroup[%d], Count[%u]"), __FUNCTIONW__, kGenGroupKey.iMapNo, GetMapLevel(), (int)bReset, iGenGroup, kContGenPoint.size());
			return MonsterGenerate( kContGenPoint, iCopy, NULL );
		}
	}
	else
	{
		if( SUCCEEDED(kGenPointMgr.GetGenPoint_Monster( m_kContGenPoint_Monster, kGenGroupKey, bReset, iGenGroup )) )
		{
//			INFO_LOG( BM::LOG_LV6, _T("[%s]Loop GenPoint MapNo[%d], MapLevel[%d], bReset[%d], GenGroup[%d], Count[%u]"), __FUNCTIONW__, kGenGroupKey.iMapNo, GetMapLevel(), (int)bReset, iGenGroup, m_kContGenPoint_Monster.size());
			return MonsterGenerate( m_kContGenPoint_Monster, iCopy, NULL );
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_OPENING_NONE"));
	return E_OPENING_NONE;
}

EOpeningState PgGround::ActivateObjectUnitGenGroup( int const iGenGroup, bool const bReset, bool const bOnce)
{
	GET_DEF_CUSTOM( PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, kGenPointMgr);
	if ( bOnce )
	{
		ContGenPoint_Object kContGenPoint;
		if( SUCCEEDED(kGenPointMgr.GetGenPoint_Object( kContGenPoint, GetGroundNo(), bReset, iGenGroup )) )
		{
			return ObjectUnitGenerate( kContGenPoint, bReset);
		}
	}
	else
	{
		if( SUCCEEDED(kGenPointMgr.GetGenPoint_Object( m_kContGenPoint_Object, GetGroundNo(), bReset, iGenGroup )) )
		{
			return ObjectUnitGenerate(m_kContGenPoint_Object, bReset);
		}
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_OPENING_NONE"));
	return E_OPENING_NONE;
}

;
EOpeningState PgGround::ObjectUnitGenerate(PgGenPoint_Object& rkGenPoint)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	// CanGen() �Լ��� �̸� �˻� �ؾ� �Ѵ� (�ƴϸ� ���� �����̰ų�)

	PgGenPoint_Object::OwnGroupCont kCont;
	rkGenPoint.GetBaseInfo( kCont );
	return ObjectUnitGenerate(rkGenPoint, kCont);
}

EOpeningState PgGround::ObjectUnitGenerate(PgGenPoint_Object& rkGenPoint, PgGenPoint_Object::OwnGroupCont const& kCont)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	// CanGen() �Լ��� �̸� �˻� �ؾ� �Ѵ� (�ƴϸ� ���� �����̰ų�)

	BM::GUID kGuid;
	EOpeningState eState = E_OPENING_NONE;
	PgGenPoint_Object::OwnGroupCont::const_iterator info_itr = kCont.begin();
	for( ; info_itr!=kCont.end(); ++info_itr )
	{
		HRESULT hRet = InsertObjectUnit( rkGenPoint.Info(), (*info_itr), kGuid ); 
		eState = __max((EOpeningState)hRet,eState);
		if ( SUCCEEDED(hRet) )
		{
			rkGenPoint.AddChild( kGuid );
			PgWorldEventMgr::IncObjectGenPointCount(rkGenPoint.Info().kPosGuid);
			PgWorldEventMgr::IncObjectGenPointGroupCount(rkGenPoint.Info().iPointGroup);

			if( E_OPENING_NONE != eState )
			{
				PgWorldEventMgr::DoObjectRegenPoint(GetGroundNo(), rkGenPoint.Info().kPosGuid, rkGenPoint.Info().iPointGroup); // ���� �̺�Ʈ �˻�
			}
		}
	}
	return eState;
}

EOpeningState PgGround::ObjectUnitGenerate(ContGenPoint_Object & kContGenPoint, bool const bReset)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	EOpeningState eState = E_OPENING_NONE;
	if( PgGroundUtil::IsBSGround(GetAttr())
	&&	false == PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
	{
		return eState; // ��Ʋ ������ ���������� �ʴٸ� ���� ���� ����
	}

	DWORD const dwCurTime = BM::GetTime32();
	BM::GUID kGuid;

	ContGenPoint_Object::iterator gen_point_itr = kContGenPoint.begin();
	for ( ; gen_point_itr!=kContGenPoint.end(); ++gen_point_itr )
	{
		PgGenPoint_Object &kGenPoint = gen_point_itr->second;
		if ( bReset )
		{
			kGenPoint.Reset();
		}

		if ( kGenPoint.CanGen(dwCurTime) )
		{
			EOpeningState const eTempState = ObjectUnitGenerate(kGenPoint);
			eState = __max(eTempState,eState);
		}
	}

	return eState;
}

void PgGround::ObjectUnitGenerate( bool const bReset, int const iGenGroup )
{
	BM::CAutoMutex kLock(m_kRscMutex);

	DWORD const dwCurTime = BM::GetTime32();
	BM::GUID kGuid;
	ContGenPoint_Object::iterator gen_point_itr;
	for ( gen_point_itr=m_kContGenPoint_Object.begin(); gen_point_itr!=m_kContGenPoint_Object.end(); ++gen_point_itr )
	{
		PgGenPoint_Object& kGenPoint = gen_point_itr->second;
		if ( bReset )
		{
			kGenPoint.Reset();
		}

		if ( (kGenPoint.Info().iPointGroup == iGenGroup) && kGenPoint.CanGen(dwCurTime) )
		{
			PgGenPoint_Object::OwnGroupCont kCont;
			kGenPoint.GetBaseInfo( kCont );

			ObjectUnitGenerate(kGenPoint, kCont);
		}
	}
}

EOpeningState PgGround::MonsterGenerate(PgGenPoint& rkGenPoint, int& iMonNo, BM::GUID& kMonGuid, CUnit* pkCaller)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	// CanGen() üũ�� �̹� �ϰ� ���;� �Ѵ�. (�Ǵ� ���� ������ ���� �Ѵ�.)
	EOpeningState eState = E_OPENING_NONE;
	if( S_OK == rkGenPoint.PopMonster(iMonNo) )
	{
		HRESULT hRet = InsertMonster( rkGenPoint.Info(), iMonNo, kMonGuid, pkCaller ); 
		eState = (EOpeningState)hRet;
		if( SUCCEEDED(hRet) )
		{
			rkGenPoint.AddChild(kMonGuid);
			PgWorldEventMgr::IncMonsterGenPointCount(rkGenPoint.Info().kPosGuid);
			PgWorldEventMgr::IncMonsterGenPointGroupCount(rkGenPoint.Info().iPointGroup);
			PgWorldEventMgr::DoMonsterRegenPoint(GetGroundNo(), rkGenPoint.Info().kPosGuid, rkGenPoint.Info().iPointGroup); // ���� �̺�Ʈ �˻�
		}
	}
	else
	{
		rkGenPoint.AddChild(BM::GUID::NullData());
	}
	return eState;
}

EOpeningState PgGround::MonsterGenerate( ContGenPoint_Monster &kContGenPoint, int const iCopy, CUnit* pkCaller )
{
	BM::CAutoMutex kLock(m_kRscMutex);

	EOpeningState eState = E_OPENING_NONE;
	if( PgGroundUtil::IsBSGround(GetAttr())
	&&	false == PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
	{
		return eState; // ��Ʋ ������ ���������� �ʴٸ� ���� ���� ����
	}

	int iMonNo;
	BM::GUID kMonGuid;
	ContGenPoint_Monster::iterator gen_point_itor;
	for ( gen_point_itor=kContGenPoint.begin(); gen_point_itor!=kContGenPoint.end(); ++gen_point_itor )
	{
		PgGenPoint &kGenPoint = gen_point_itor->second;
		if( kGenPoint.CanGen() )
		{
			int _iCopy = iCopy+1;
			while ( _iCopy-- )
			{
				EOpeningState const eTempState = MonsterGenerate(kGenPoint, iMonNo, kMonGuid, pkCaller);
				eState = __max(eTempState,eState);
			}
		}
	}
	return eState;
}

bool PgGround::IsTargetInRange(CUnit *pkUnit, int iZLimit, bool const bCheckFromGround)
{
	if(!pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CUnit::DequeTarget& rkTargetList = pkUnit->GetTargetList();
	CUnit::DequeTarget::iterator itor = rkTargetList.begin();

	bool const bMonster = UT_MONSTER==pkUnit->UnitType();

	int iCount = 0;
	ESkillTargetType kType = pkUnit->GetSkill()->GetSkillDef() ? static_cast<ESkillTargetType>(pkUnit->GetSkill()->GetSkillDef()->GetTargetType()) : ESTARGET_ENEMY;
	//static const NxVec3 kDownRay(0.0f, 0.0f, -1.0f);

	while (itor != rkTargetList.end())
	{
		CUnit* pkTarget = GetUnit(itor->kGuid);
		if (pkTarget == NULL)
		{
			pkUnit->DeleteTarget(itor->kGuid);
			++itor;
			continue;
		}
		if (! pkUnit->IsTarget(pkTarget, true, kType))
		{
			pkUnit->DeleteTarget(itor->kGuid);
			++itor;
			continue;
		}

		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkTarget);
		if(pkPlayer)
		{
			PLAYER_ACTION_INFO const* pkAction = pkPlayer->GetPlayerActionInfo();
			if(pkAction && ACTIONID_A_JUMP!=pkAction->iActionID)
			{
				iZLimit = 30;//�� ���� ���� ��� ��� �ϳ�?
			}
		}

		int const iDetectRange = pkUnit->GetDetectRange();
		EInRangeRet eRet = ::IsInRange(pkUnit->GetPos(), GetUnitPos(pkTarget), iDetectRange, iZLimit, false);
		if (ERange_OK!=eRet)
		{
			if(true==bCheckFromGround && ERange_Z==eRet)	//z�˻縦 �������� �Ѵٴ� ����
			{
				eRet = ::IsInRange(pkUnit->GetPos(), GetUnitPos(pkTarget, true, 300.0f), iDetectRange, iZLimit, false);	//�ٴ� üũ�ؼ� �ѹ� �� �˻��� ����
			}
			
			if(ERange_OK!=eRet)
			{
				pkUnit->DeleteTarget(itor->kGuid);
				++itor;
				continue;
			}
		}

		++itor;
		++iCount;
	}
	return (iCount > 0);
}

POINT3BY PgGround::GetMoveDirection(BM::GUID const & rkGuid)
{
	CUnit* pkUnit = GetUnit(rkGuid);
	if(!pkUnit)
	{
		return POINT3BY(0, 0, 0);
	}

	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(!pkPlayer)
	{
		return POINT3BY(0, 0, 0);
	}

	return pkPlayer->MoveDirection();
}

POINT3BY PgGround::GetPathNormal(BM::GUID const & rkGuid)
{
	CUnit* pkUnit = GetUnit(rkGuid);
	if(!pkUnit)
	{
		return POINT3BY(0, 0, 0);
	}

	/*
	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(!pkPlayer)
	{
		return POINT3BY(0, 0, 0);
	}

	return pkPlayer->PathNormal();
	*/
	return pkUnit->PathNormal();
}

POINT3 PgGround::GetUnitPos(BM::GUID const & rkGuid, bool bCheckImpact, float const fRayRange)
{
	CUnit* pkUnit = GetUnit(rkGuid);
	if (pkUnit == NULL)
	{
		return POINT3(0,0,0);
	}
	return GetUnitPos(pkUnit, bCheckImpact, fRayRange);
}

bool PgGround::CanAttackTarget(CUnit* pkAttacker)
{
	DWORD const dwServerTime = g_kEventView.GetServerElapsedTime();
	if ( (0 != pkAttacker->GetAbil(AT_CANNOT_ATTACK)) || !pkAttacker->GetSkill()->CheckCoolTime(SKILL_NO_GLOBAL_COOLTIME, dwServerTime))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( pkAttacker->IsUnitType(UT_SUMMONED) && false==IsInBattleZone(pkAttacker) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	/*
	CUnit::ConTarget& rkList = pkAttacker->GetTargetList();
	CUnit::ConTarget::iterator itor = rkList.begin();
	UNIT_PTR_ARRAY kTargetArray;
	while (itor != rkList.end())
	{
		CUnit* pkTarget = GetUnit(*itor);
		if (pkTarget == NULL)
		{
			pkAttacker->DeleteTarget(*itor);
		}
		else
		{
			if (!pkAttacker->IsEnemy(pkTarget))
			{
				pkAttacker->DeleteTarget(*itor);
			}
			else
			{
				kTargetArray.Add(pkTarget);
			}
		}

		++itor;
	}
	*/

	UNIT_PTR_ARRAY kTargetArray;
	SActArg kActArg;
	PgGroundUtil::SetActArgGround(kActArg, this);
	//int iSkillNum = pkAttacker->GetSkill()->GetSkillNo();
	int iSkillNum = 0;
	//if (iSkillNum <= 0)
	{
		iSkillNum = g_kAISkillChooser.GetAvailableSkill(pkAttacker, kTargetArray, true, &kActArg);
	}
	if (iSkillNum > 0)
	{
		if(!kTargetArray.empty())
		{
			UNIT_PTR_ARRAY::iterator it = kTargetArray.begin();
			PgMonster* pkMonster = dynamic_cast<PgMonster*>(pkAttacker);
			if(pkMonster)
			{
				while(it!=kTargetArray.end())
				{
					CUnit const* pkTarget = (*it).pkUnit;
					if(pkTarget)
					{
						if(pkMonster->CheckIgnoreTarget(pkTarget, BM::GetTime32()))
						{
							break;	//�ѳ��̶� ����� �� ���� �ִٸ�
						}
						else
						{
							it = kTargetArray.erase(it);
							continue;
						}
					}
					++it;
				}
			}
		}
		pkAttacker->SetTargetList(kTargetArray);

#ifdef AI_DEBUG
		//INFO_LOG(BM::LOG_LV9, _T("[%s] AvailableSkill[%d]"), __FUNCTIONW__, iSkillNum);
#endif
		//ESkillCastType eCastType = (ESkillCastType) g_SkillDefMgr.GetAbil(iSkillNum, AT_CASTTYPE);
		return pkAttacker->GetSkill()->CheckCoolTime(iSkillNum, dwServerTime);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGround::AI_SkillCasting(CUnit* pkCaster, SActArg* pkActArg, BM::Stream& rkTailPacket)
{
	//CUnit* pkTarget = GetUnit(pkCaster->GetTarget());
	//if (pkTarget == NULL)
	//{
	//	INFO_LOG(BM::LOG_LV5, _T("[%s] Unit cannot get Target Skill[%d]"), __FUNCTIONW__, pkTarget->GetSkill());
	//	return false;
	//}
	int const iSkillNo = pkCaster->GetSkill()->GetSkillNo();
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if( !pkSkillDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Cannot get SkillDef ["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return (g_kSkillAbilHandleMgr.SkillBegin(pkCaster, iSkillNo, pkActArg, ESS_CASTTIME, &rkTailPacket) >= 0);
}

bool PgGround::AI_SkillFire(CUnit* pkCaster, SActArg* pkActArg, const ESkillStatus eSkillStatus)
{
	//CUnit* pkTarget = GetUnit(pkCaster->GetTarget());
	//if (pkTarget == NULL)
	//{
	//	return false;
	//}

	if ( (0 != pkCaster->GetAbil(AT_CANNOT_ATTACK)) )
	{
		//INFO_LOG(BM::LOG_LV5, _T("WARNING...[%s] Caster->GetAbil(AT_CANNOT_ATTACK)=[%d], Cannot Attack!!!!"), __FUNCTIONW__, pkCaster->GetAbil(AT_CANNOT_ATTACK));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	int iSkillNo = pkCaster->GetSkill()->GetSkillNo();
	if (iSkillNo <= 0)
	{
		//INFO_LOG(BM::LOG_LV0, __FL__<<L"Unit cannot FireSkill because SkillNo is "<<iSkillNo);
		pkCaster->SetDelay(0);	// ERROR �߻��Ͽ� �ٸ� Action���� �����ؾ� ��.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	PgActionResultVector kResult;
	ESkillFireType eFireType = (ESkillFireType) pkCaster->GetSkill()->GetAbil(AT_FIRE_TYPE);
	if ((eSkillStatus != ESS_FIRE || (eFireType & EFireType_DontDmgWhenFire) == 0) && eSkillStatus!=ESS_TIMEWAITING)	//ESS_TIMEWAITING �� �����϶�� ��ȣ�� ������
	{
		// Target�� �ٽ� ��� ����
		UNIT_PTR_ARRAY kUnitArray;
		if ( !AI_GetSkillTargetList(pkCaster, iSkillNo, kUnitArray, true, pkActArg))
		{
			//if (eSkillStatus == ESS_TARGETLISTMODIFY)
			//{
				// Target�� ���� ESS_TARGETLISTMODIFY �����̹Ƿ� �׳� �����Ѵ�.
			//	return false;
			//}
			// Target�� ��� Fire �׼��� ������� Ŭ���̾�Ʈ���� ������ �ȴ�.
			//return false;
		}
		//EActionResultCode eActionResult = pkCaster->GetSkill()->Fire(iSkillNo);

		CheckTargetList(kUnitArray);
		
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
		if( !pkSkillDef )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Cannot get SkillDef ["<<iSkillNo<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		//if (CanAttackTarget(pkCaster))
		// Client Control Projectile ��ų�϶����� ó������ ������ Hit�� �������� �ʴ´�.
		// ���߿� Client �ڽ��� �¾����� �˷� �´�.
		bool bClientCtrl = true;
		if (!pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE) || pkSkillDef->IsSkillAtt(static_cast<ESkillAttribute>(SAT_SERVER_FIRE)))
		{
			g_kTargettingManager.FindTarget(pkCaster, pkSkillDef, &kUnitArray, this);
			pkActArg->Set(ACTARG_EFFECT_CASTER_UNIT_TYPE, pkCaster->UnitType());	  // ĳ���� Ÿ���� �����Ѵ�
			g_kSkillAbilHandleMgr.SkillFire(pkCaster, iSkillNo, pkActArg, &kUnitArray, &kResult);
			pkCaster->SetAbil(AT_AI_SKILL_USE_COUNT, pkCaster->GetAbil(AT_AI_SKILL_USE_COUNT)+1);
			pkCaster->SetAbil(AT_AI_SKILL_FIRE_TIME, BM::GetTime32());

			OnAttacked(pkCaster, &kUnitArray, iSkillNo, &kResult, pkActArg, g_kEventView.GetServerElapsedTime());
#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"SkillFired Skill["<<iSkillNo<<L"] TIME["<<BM::GetTime32()<<L"] Target["<<kUnitArray.size()<<L"]");
#endif
			bClientCtrl = false;
		}

		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		UNIT_PTR_ARRAY::const_iterator itor = kUnitArray.begin();
		while(itor != kUnitArray.end())
		{
			if(CUnit* pkTarget = itor->pkUnit)
			{
				if(PgActionResult* pkResult = kResult.GetResult(pkTarget->GetID(), bClientCtrl))
				{
					if(IsDecEquipDuration())
					{//������ ���.
						if(pkResult->GetValue())
						{//��ħ�� �ƴҶ���.
							PgAction_DecEquipDuration kDecDurAction(GroundKey(), false, pkResult->GetValue());
							kDecDurAction.DoAction(pkTarget, pkCaster);
						}
					}

					int const iDmgEffect = pkResult->GetEffect(0);
					if(pkTarget->IsAlive() && 0<iDmgEffect && pkResult->IsDamageAction())
					{
						CEffectDef const* pkDmgEffect = kEffectDefMgr.GetDef(iDmgEffect);
						if(pkDmgEffect && 0 < pkDmgEffect->GetAbil(AT_BLOW_VALUE) )	// AT_BLOW_VALUE���� ����̸� DamageDelay�� �ִ� �����̴�.
						{
							pkTarget->SetBlowAttacker(pkCaster->GetID());
							pkTarget->SetAbil(AT_DAMAGE_EFFECT_DURATION, pkDmgEffect->GetDurationTime());//������ �׼� �ð� ����
							pkTarget->SetDelay(__max(pkDmgEffect->GetDurationTime(), 3000));
						}
					}
				}
			}
			++itor;
		}
	}

	EActionResultCode eActionResult = pkCaster->GetSkill()->CheckSkillAction(iSkillNo, eSkillStatus, g_kEventView.GetServerElapsedTime(), pkCaster->GetAbil(AT_R_COOLTIME_RATE_SKILL), pkCaster->GetAbil(AT_CAST_TIME_RATE));
	if (eActionResult > EActionR_Success_Max)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Unit cannot FireSkill EActionResultCode["<<static_cast<int>(eActionResult)<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// Sending Result
	// ��Ȥ �ڻ���ź������ ��쿡�� �̹� US_DEAD �ϼ� �ֱ� ������
	// SendNfyAIAction�� GetState() �Լ��� ������� �ʰ� ���� US_ATTACK�� �����Ͽ� ȣ���Ѵ�.
	if (eSkillStatus == ESS_FIRE)
	{
		SendNfyAIAction(pkCaster, US_ATTACK, &kResult);
	}
	else
	{
		EUnitState const eState = pkCaster->GetState()==US_DEAD ? US_SKILL_FIRE : pkCaster->GetState();
		SendNfyAIAction(pkCaster, eState, &kResult);
	}

	return true;
}

void PgGround::ReleaseAllUnit()
{
	PgObjectMgr2::InitObjectMgr();
}

bool PgGround::GetTargetPos(CUnit* pkUnit, POINT3& rkTargetPos)
{
	CUnit* pkTarget = GetUnit(pkUnit->GetTarget());
	if (pkTarget == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	rkTargetPos = pkTarget->GetPos();
	return true;
}

void PgGround::AI_CallHelp(CUnit* pkUnit, float fCallRange)
{
	EUnitType eUnitType = pkUnit->UnitType();
	POINT3 const kPos = pkUnit->GetPos();
	CUnit* pkFind = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eUnitType, kItor);
	while((pkFind = PgObjectMgr::GetNextUnit(eUnitType, kItor)) != NULL)
	{
		if(ERange_OK==::IsInRange(kPos, pkFind->GetPos(), (int)fCallRange, 30))
		{
//#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Call Help UnitType["<<eUnitType<<L"], CalledUnit["<<pkFind->GetID()<<L"]");
//#endif
			pkFind->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_CALLED_HELP);
			pkUnit->SetAbil(AT_WAYPOINT_GROUP, -1);	// -1 �̸� Patrol ���¿��� GoalPos�� �̵��ϵ��� �Ѵ�.
			pkFind->GoalPos(kPos);
#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__ << _T("GoalPos =") << pkFind->GoalPos());
#endif
		}
	}
	
}

int PgGround::GetReserveAISkill(CUnit* pkUnit)
{
	UNIT_PTR_ARRAY kTargetArray;
	GetTargetList(pkUnit, kTargetArray);
	if (kTargetArray.size() == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	SActArg kActArg;
	PgGroundUtil::SetActArgGround(kActArg, this);
	return g_kAISkillChooser.GetReservableSkill(pkUnit, kTargetArray, true, &kActArg);
}

// SetValidGoalsPos()�� ��ü��.
//bool PgGround::CheckUnitGoalPos(CUnit* pkUnit)
//{
//	// Ray�� �ٴ����� ��Ƽ� �������� ���� �ɰ��ΰ� �Ǵ�����.
//	DWORD const dwMoveTime = 200;
//	int iMoveSpeed = pkUnit->GetAbil(AT_C_MOVESPEED);
//	POINT3& ptCurrentPos = pkUnit->GetPos();
//	POINT3 const ptGoal = pkUnit->GoalPos();
//	NxVec3 kTargetVec = NxVec3(ptGoal.x, ptGoal.y, 0) - NxVec3(ptCurrentPos.x, ptCurrentPos.y, 0);
//	kTargetVec.normalize();
//	NxVec3 kResultVec = kTargetVec * NxReal(iMoveSpeed * dwMoveTime / 1000.0f) + NxVec3(ptCurrentPos.x, ptCurrentPos.y, ptCurrentPos.z);
//
//	// TODO : Z�� ������ Client�� �����Ͽ� ���ֵ��� �ؾ� �Ѵ�.
//	NxRay kRay(NxVec3(kResultVec.x, kResultVec.y, ptCurrentPos.z+20), NxVec3(0, 0, -1.0f));
//	NxRaycastHit kHit;
//	NxShape *pkHitShape = PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 90.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
//	if(pkHitShape)
//	{
//		return true;
//	}
//	INFO_LOG(BM::LOG_LV9, _T("[%s] Check GoalPos failed GroundNo[%d]"), __FUNCTIONW__, GroundKey().GroundNo());
//	return false;
//}

CUnit* PgGround::GetPlayer(char const* pcCharacterName)
{
	std::wstring wCharacterName = UNI(pcCharacterName);
	return GetPlayer( wCharacterName );
}

CUnit* PgGround::GetPlayer( std::wstring const &wstrName )
{
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr2::GetFirstUnit(UT_PLAYER, kItor);
	CUnit* pkPlayer = NULL;
	while ((pkPlayer = PgObjectMgr2::GetNextUnit(UT_PLAYER, kItor)) != NULL)
	{
		if ( wstrName == pkPlayer->Name() )
		{
			return pkPlayer;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

// �� ��ų�� ������ TargetList�� ��� ����.
//	bDamageFire : Damage�� �ֱ� ���� Target�� ��� ���ΰ�?
// [NOTICE]
//	��� ��ų�� SkillFire�� �ϱ� ���� Target�� ��� �����, ������ Damage�� �� �� �ִ� Target��� ����� �ٸ� �� �ִ�.
//	���� ���, ���ݰ� ���� ��ų�� ó���� Range�ȿ� ������ ��ų�� �ߵ��Ͽ� ���� ����������,
//	������ �浹�Ͽ��߸�, �������� �־�� �ϱ� ������, �������� �޶�� �Ѵ�.
bool PgGround::AI_GetSkillTargetList(CUnit* pkCaster, int const iSkillNo, UNIT_PTR_ARRAY& rkTargetList, bool bDamageFire, SActArg const* pkActArg, POINT3 const* pkDir, SAddRange const& rkAddRange)
{
	if (pkCaster == NULL || iSkillNo < 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if( !pkSkillDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Cannot Get SkillDef["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	/*int const iMaxRange = PgAdjustSkillFunc::GetAttackRange(pkCaster, pkSkillDef);
	if (iMaxRange <= 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}*/

	//int iMinRange = pkSkillDef->GetAbil(AT_SKILL_MIN_RANGE);
	POINT3 const ptCasterPos = GetUnitPos(pkCaster);
	int const iTargetType = pkSkillDef->GetTargetType();
	// ���� �ʿ���� UnitType�� �� ������
	//EUnitType const eUnitTypeArray[] = {UT_PLAYER, UT_MONSTER, UT_GROUNDBOX, UT_PET, UT_NPC, UT_BOSSMONSTER, UT_FIGURE, UT_ENTITY, UT_NONETYPE};
	static EUnitType const eUnitTypeArray[] = {UT_SUMMONED, UT_PLAYER, UT_MONSTER, UT_BOSSMONSTER, UT_OBJECT, UT_NONETYPE};	

	int iIndex = 0;
	EUnitType const eCasterUnitType = pkCaster->UnitType();
	rkTargetList.clear();
	while (eUnitTypeArray[iIndex] != UT_NONETYPE)
	{
		// Target�� �ɼ� �ִ°� �˻��ϰ�
		if (CUnit::IsTargetUnitType(eCasterUnitType, eUnitTypeArray[iIndex], iTargetType))
		{
			// �����ȿ� Ÿ�� Unit���� �����´�.
			GetTargetInArea(pkCaster, pkSkillDef, eUnitTypeArray[iIndex], rkTargetList, bDamageFire, pkDir, rkAddRange);
		}
		++iIndex;
	}

	// �ϴ��� Caster�� �߽����� Range�ȿ� ������ ��� Target�� �ȴ�.
	/*
	int iMinRange = pkSkillDef->GetAbil(AT_SKILL_MIN_RANGE);
	EUnitType eUnitType = pkCaster->UnitType();
	POINT3 const ptCasterPos = GetUnitPos(pkCaster);
	int iFound = 0;
	switch(eUnitType)
	{
	case UT_PLAYER:
	case UT_ENTITY:
		iFound = GetTargetInArea(pkCaster, pkSkillDef, UT_PLAYER, rkTargetList, bDamageFire);
		iFound += GetTargetInArea(pkCaster, pkSkillDef, UT_MONSTER, rkTargetList, bDamageFire);
		break;
	case UT_MONSTER:
	case UT_BOSSMONSTER:
		iFound = GetTargetInArea(pkCaster, pkSkillDef, UT_PLAYER, rkTargetList, bDamageFire);
		break;
	default:
		VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] NOT implemented UnitType[%d]"), __FUNCTIONW__, eUnitType);
		break;
	}
	*/

	if( rkTargetList.size()  > 0 )
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
	//return (iFound > 0) ? true : false;
}

inline bool IsTarget(CUnit const& rkCaster, CUnit const& rkTarget, CSkillDef const& rkSkillDef, VEC_GUID const& rkPartyList)
{
	if(rkCaster.IsUnitType(UT_SUMMONED) && (ESTARGET_PARTY&rkSkillDef.GetTargetType()))
	{
		if(rkPartyList.end() != std::find(rkPartyList.begin(), rkPartyList.end(), rkTarget.GetID()))
		{
			return true;
		}
	}
	return false;
}

int PgGround::GetTargetInArea(CUnit* pkCaster, const CSkillDef* pkSkillDef, EUnitType const eUnitType, UNIT_PTR_ARRAY& rkTargetList, bool const bDamageFire, POINT3 const* pkDir, SAddRange const& rkAddRange)
{
	if(!pkCaster || !pkSkillDef)
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	bool const b2ndRange = (bDamageFire && pkSkillDef->GetAbil(AT_2ND_AREA_PARAM1) > 0);
	ESkillArea const eAreaType = (ESkillArea) pkSkillDef->GetAbil((b2ndRange) ? AT_2ND_AREA_TYPE : AT_1ST_AREA_TYPE);
	int const iRange = PgAdjustSkillFunc::CalcAttackRange(pkCaster, pkSkillDef->GetAbil((b2ndRange) ? AT_2ND_AREA_PARAM1 : AT_ATTACK_RANGE)) + std::tr1::get<0>(rkAddRange);
	int const iParam2 = pkSkillDef->GetAbil((b2ndRange) ? AT_2ND_AREA_PARAM2 : AT_1ST_AREA_PARAM2) + std::tr1::get<1>(rkAddRange);
	int const iParam3 = pkSkillDef->GetAbil((b2ndRange) ? AT_2ND_AREA_PARAM3 : AT_1ST_AREA_PARAM3) + std::tr1::get<2>(rkAddRange);
	int const iParam4 = pkSkillDef->GetAbil((b2ndRange) ? AT_2ND_AREA_PARAM4 : AT_1ST_AREA_PARAM4) + std::tr1::get<3>(rkAddRange);

	if (iParam2 <= 0 && (eAreaType == ESArea_Cube || eAreaType == ESArea_Cone || eAreaType == ESArea_Front_Sphere))
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Skill["<<pkSkillDef->No()<<L"] has "<<eAreaType<<L" area type and "<<iParam2<<L" param2");
	}

	ESkillFireType const eFireType = (ESkillFireType) pkSkillDef->GetAbil(AT_FIRE_TYPE);
	POINT3 ptCaster = GetUnitPos(pkCaster);
	int iMinRange = pkSkillDef->GetAbil(AT_SKILL_MIN_RANGE);
	int iPCount = 0;
	std::vector<POINT3> vecCasterPos;
	if (bDamageFire)
	{
		if (eFireType & EFireType_Projectile) 
		{ 
			iPCount = pkSkillDef->GetAbil(AT_MON_PROJECTILE_NUM);
			if(0 == iPCount)
			{
				ptCaster = pkCaster->Projectile(0).ptCurrent;
			}
			else
			{
				for(int i=0; i<iPCount; ++i)
				{
					vecCasterPos.push_back(pkCaster->Projectile(i).ptCurrent);
				}
			}
			iMinRange = 0;	//������ƿ�� ������ �� MinRange�� ������ �ȵ�
		}	// �����ϴ� ��ü�� Projectile �̴�.
		else if (eFireType & EFireType_AttackToGoalPos) { ptCaster = pkCaster->GoalPos(); }	// GoalPos ��ġ�� �����ϴ� ���̴�.
	}
	int const iMaxTargetNum = std::max(1, pkSkillDef->GetAbil(AT_MAX_TARGETNUM));

#ifdef AI_DEBUG
	// Test�� ���� ���� ���� ����
	//eAreaType = ESArea_Cube;
	//iParam2 = 10;
#endif

	NxVec3 kVisionDir;
	if(pkDir)
	{
		kVisionDir.setx(pkDir->x);
		kVisionDir.sety(pkDir->y);
		kVisionDir.setz(pkDir->z);
	}
	else
	{
		kVisionDir.zero();

		bool bUseTargetDir = true;
		if((eFireType & EFireType_KeepVisionVector) || pkCaster->GetAbil(AT_MON_ANGLE_FIX))
		{
			bUseTargetDir = !bDamageFire;
		}
		
		GetVisionDirection(pkCaster, kVisionDir, bUseTargetDir);
		if(!kVisionDir.isZero())
		{
			POINT3BY kOrientedVec;
			POINT3_2_POINT3BY(POINT3(kVisionDir.x, kVisionDir.y, kVisionDir.z), kOrientedVec);
			pkCaster->SetOrientedVector(kOrientedVec);
		}
		else	//���ڸ��� ��� �� ������ ���� ���Ͱ� 0�� ���� �ִ� �׷� �� ������ ���������� ���� ������ ���ؼ� ������ �ξ��� ������Ƽ�� ���Ϳ��� �����ؼ� �������
		{
			POINT3BY kOrientedVec = pkCaster->GetOrientedVector();
			kVisionDir.set(kOrientedVec.x, kOrientedVec.y, kOrientedVec.z);
			kVisionDir.normalize();
		}
	}

	//
	VEC_GUID kPartyList;
	if(pkCaster->IsUnitType(UT_SUMMONED) && (ESTARGET_PARTY&pkSkillDef->GetTargetType()))
	{
		if(CUnit * pkOwnerUnit = GetUnit(pkCaster->Caller()))
		{
			BM::GUID const& kPartyGuid = pkOwnerUnit->GetPartyGuid();
			if(BM::GUID::IsNotNull(kPartyGuid))
			{
				GetPartyMember(kPartyGuid, kPartyList);
			}
		}
	}

	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(eUnitType, kItor);
	CUnit* pkEnemy = NULL;

    PgTargettingManager::E_TARGETTING_TYPE eSelectType = static_cast<PgTargettingManager::E_TARGETTING_TYPE>(pkSkillDef->GetAbil(AT_TARGET_SELECT_TYPE));
	if(PgTargettingManager::ETT_NONE==eSelectType)
	{
		eSelectType = static_cast<PgTargettingManager::E_TARGETTING_TYPE>(pkCaster->GetAbil(AT_TARGET_SELECT_TYPE));
	}

    if(PgTargettingManager::ETT_NONE==eSelectType)
    {
	    int iCount = rkTargetList.size();
	    while (iCount < iMaxTargetNum && (pkEnemy = PgObjectMgr::GetNextUnit(eUnitType, kItor)) != NULL)
	    {
		    if (!pkCaster->IsTarget(pkEnemy, true, pkSkillDef->GetTargetType(),pkSkillDef) && !IsTarget(*pkCaster, *pkEnemy, *pkSkillDef, kPartyList))
		    {
			    continue;
		    }
		    if(iPCount)
		    {
			    bool bInArea = false;
			    for(int i=0; i<iPCount; ++i)
			    {
				    if(PgSkillAreaChooser::InArea(pkEnemy, eAreaType, vecCasterPos.at(i), kVisionDir, GetUnitPos(pkEnemy, (!bDamageFire && IsDetailAI(pkCaster)), 300.0f), iRange, iParam2, iParam3, iParam4, iMinRange))
				    {
					    bInArea = true;
				    }
			    }
			    if(!bInArea)
			    {
				    continue;
			    }
		    }
		    else
		    {
			    if (!PgSkillAreaChooser::InArea(pkEnemy, eAreaType, ptCaster, kVisionDir, GetUnitPos(pkEnemy, (!bDamageFire && IsDetailAI(pkCaster)), 300.0f), iRange, iParam2, iParam3, iParam4, iMinRange))
			    {
				    continue;
			    }
		    }

		    // Debug Code...
		    //PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkEnemy);
		    //if(pkPlayer)
		    //{
		    //	INFO_LOG(BM::LOG_LV8, _T("[%s] GetUnitPos's Latency : %d"), __FUNCTIONW__, pkPlayer->Latency());
		    //}

		    rkTargetList.Add(pkEnemy);
		    ++iCount;
	    }
    }
    else
    {
	    while ((pkEnemy = PgObjectMgr::GetNextUnit(eUnitType, kItor)) != NULL)
	    {
		    if (!pkCaster->IsTarget(pkEnemy, true, pkSkillDef->GetTargetType(),pkSkillDef) && !IsTarget(*pkCaster, *pkEnemy, *pkSkillDef, kPartyList))
		    {
			    continue;
		    }
		    if(iPCount)
		    {
			    bool bInArea = false;
			    for(int i=0; i<iPCount; ++i)
			    {
				    if(PgSkillAreaChooser::InArea(pkEnemy, eAreaType, vecCasterPos.at(i), kVisionDir, GetUnitPos(pkEnemy, (!bDamageFire && IsDetailAI(pkCaster)), 300.0f), iRange, iParam2, iParam3, iParam4, iMinRange))
				    {
					    bInArea = true;
				    }
			    }
			    if(!bInArea)
			    {
				    continue;
			    }
		    }
		    else
		    {
			    if (!PgSkillAreaChooser::InArea(pkEnemy, eAreaType, ptCaster, kVisionDir, GetUnitPos(pkEnemy, (!bDamageFire && IsDetailAI(pkCaster)), 300.0f), iRange, iParam2, iParam3, iParam4, iMinRange))
			    {
				    continue;
			    }
		    }

		    // Debug Code...
		    //PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkEnemy);
		    //if(pkPlayer)
		    //{
		    //	INFO_LOG(BM::LOG_LV8, _T("[%s] GetUnitPos's Latency : %d"), __FUNCTIONW__, pkPlayer->Latency());
		    //}

		    rkTargetList.Add(pkEnemy);
	    }

        g_kTargettingManager.FindTarget(pkCaster, pkSkillDef, &rkTargetList, this, eSelectType, pkSkillDef->GetAbil(AT_TARGET_SELECT_VALUE));
        if(iMaxTargetNum < rkTargetList.size())
        {
            rkTargetList.resize(iMaxTargetNum);
        }
    }

	return rkTargetList.size();
}

bool PgGround::FireToggleSkill(CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, const SActionInfo &rkAction, PgActionResultVector* pkActionResultVec)
{
	if(pkCaster == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( 0 != pkCaster->GetAbil(AT_CANNOT_ATTACK) 
		&& rkAction.byActionParam != ESS_TOGGLE_OFF
		)
	{
		//INFO_LOG(BM::LOG_LV0, _T("WARNING...[%s] Caster->GetAbil(AT_CANNOT_ATTACK)=[%d], Cannot Attack!!!!"), __FUNCTIONW__, pkCaster->GetAbil(AT_CANNOT_ATTACK));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// �����ִٰ� �°ų�/������ Hidden �Ӽ��� �����
	// �� AT_LOCK_HIDDEN_ATTACK �� ������ Ǯ���� �ʴ´�.
	int iHiddenAbil = pkCaster->GetAbil(AT_UNIT_HIDDEN);
	if (iHiddenAbil > 0 && pkCaster->GetAbil(AT_LOCK_HIDDEN_ATTACK) <= 0)
	{
		pkCaster->SetAbil(AT_UNIT_HIDDEN, iHiddenAbil-1);
	}

	SActArg kArg;
	PgGroundUtil::SetActArgGround(kArg, this);
	kArg.Set(ACTARG_ACTIONINSTANCEID, rkAction.iActionInstanceID);	// Client���� AddEffect ��Ŷ ������ �ʿ��ϴ�.
	{
		int iLuaRet = g_kSkillAbilHandleMgr.SkillToggle(pkCaster, rkAction.iActionID, &kArg, (rkAction.byActionParam == ESS_TOGGLE_ON) ? true : false, 
			&rkTargetArray, pkActionResultVec);
		if (iLuaRet < 0)
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			int iBasicSkill = kSkillDefMgr.GetCallSkillNum(rkAction.iActionID);
	
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Skill_Toggle"<<iBasicSkill<<L" returnd ["<<iLuaRet<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	return true;
}

CUnit* PgGround::GetUnitControl(CUnit* pkUnit, CUnit* pkCalledUnit, unsigned short usPacketType, BM::Stream& rkPacket)
{
	CUnit* pkControlUnit = NULL;
	switch(usPacketType)
	{
// 	case PT_C_M_REQ_CREATE_ENTITY:
// 		{
// 			if (pkUnit->GetSummonUnitCount() >= pkUnit->GetMaxSummonUnitCount())
// 			{
// 				INFO_LOG(BM::LOG_LV0, _T("[%s] Cannot Create Entity anymore CurrentCount[%d]"), __FUNCTIONW__, pkUnit->GetSummonUnitCount());
// 				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
// 				return NULL;
// 			}
// 			SCreateEntity sCreateInfo;
// 			rkPacket.Pop(sCreateInfo);
// 			pkControlUnit = CreateEntity(pkUnit, &sCreateInfo, _T("ClientMade"), false);
// 		}break;
// 	case PT_C_M_REQ_DELETE_ENTITY:
// 		{
// 			BM::GUID kEntityGuid;
// 			rkPacket.Pop(kEntityGuid);
// 			CUnit* pkDelete = GetUnit(kEntityGuid);
// 			if (pkDelete != NULL && pkDelete->IsUnitType(UT_ENTITY))
// 			{
// 				PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkDelete);
// 				if (pkEntity != NULL && pkEntity->Caller() == pkUnit->GetID())
// 				{
// 					ReleaseUnit(pkEntity);
// 				}
// 			}
// 		}break;
	case PT_C_M_REQ_ACTION2:
		{
			if (pkCalledUnit != NULL || pkCalledUnit->Caller() == pkUnit->GetID())
			{
				pkControlUnit = pkCalledUnit;
			}
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Unknown Packet Type ["<<usPacketType<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
		}break;
	}

	return pkControlUnit;
}

void PgGround::ReqProjectileAction(CUnit* pkSendUnit, BM::Stream * const pkPacket)
{	
	SProjectileAction kAction;
	float fLifeTime = 0;
	pkPacket->Pop(kAction);
	pkPacket->Pop(fLifeTime);
	CUnit* pkCaster = PgObjectMgr::GetUnit(kAction.kCasterGuid);
	if ( pkCaster == NULL )
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Caster is NULL Guid["<<kAction.kCasterGuid<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkCaster is NULL"));
		return;
	}

	//INFO_LOG(BM::LOG_LV6, __FL__ << _T("Caster=") << kAction.kCasterGuid << _T(", iActionID=") << kAction.iActionID << _T(", InstanceID=")
	//	<< kAction.iActionInstanceID << _T(", ProjectileUID=") << kAction.uProjectileUID);

	// ���Ȱ˻縦 ������ �� ���ΰ� ? ����~~~
	// ���� 1 : Monster�� ������ ��� Target�� Player �ڱ��ڽŸ� �־�� �Ѵ�.
	// ���� 2 : Player�� ������ ��� Fire �� ��ų�� �´°� �˻�.
	//	(�̰��� Player�� �߻�ü Skill Fire ���Ŀ��� �ٸ� ��ų�� ����� �� ���ٴ� ������ �ʿ��ϴ�.)
	//	(��, �߻�ü�� ���ư��� �ִ� ���߿��� �ٸ� ���� Skill Fire �� �� ���ų�, �̹� �߻�� �߻�ü�� ��ҵǾ�� �Ѵ�.)
	// ���� 3 : Skill�� Client�� �����ϴ� Projectile Skill�� �´��� Ȯ���Ѵ�.
	if ( !::CheckUnitAction( pkCaster, kAction.iActionID, this, PT_C_M_REQ_PACTION ) )
	{
		return;
	}

	// Caster�� ���� �ƴѰ��� Target�� ������ �Ѵ�.
	BM::GUID const kCheckGuid = pkSendUnit->Caller();
	bool const bDamageAction = ( kCheckGuid != pkCaster->Caller() );
	
	BYTE byTargetNum;
	pkPacket->Pop(byTargetNum);
	// ���� : Client�� Packet ���̸� �����ϱ� ������ Max Limit�� ������ �Ѵ�.

	bool bHackingUser = false;

	PgActionResultVector kActionResultVec;
	UNIT_PTR_ARRAY kTargetArray;
	byTargetNum = ((byTargetNum > MAX_SKILL_TARGETNUM) ? MAX_SKILL_TARGETNUM : byTargetNum);
	
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = (CSkillDef*) kSkillDefMgr.GetDef(kAction.iActionID);
	if(pkSkillDef)
	{
		int iMaxTarget = pkSkillDef->GetAbil(AT_MAX_TARGETNUM);

		//���� ����� ����ϴ� ��ų �̸� ���� ���� �� ���ش�.
		if(0 < pkSkillDef->GetAbil(AT_PENETRATION_USE_TYPE))
		{
			//������ ������ �ȵǰ� ��ų���� ����ϴ°͸� �־�����Ѵ�.
			int iPenetrationCount = pkCaster->GetAbil(AT_PENETRATION_COUNT);
			if(0 != iPenetrationCount)
			{
				iMaxTarget += iPenetrationCount;
			}
		}
		else if (kAction.iActionID == 102100301)
		{
			iMaxTarget += 1;	//���� (Ŭ���̾�Ʈ �����ε�..)
		}

		if(static_cast<int>(byTargetNum) > iMaxTarget)
		{
			if (PgGround::ms_kAntiHackCheckVariable.bUse_Projectile)
			{
				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkSendUnit);
				if(pkPlayer)
				{
					// �̳��� ��ŷ�̴�.
					BM::vstring kLogMsg;
					kLogMsg << __FL__ << _T("[HACKING][Projectile MaxTarget] User[") << pkPlayer->Name() << _T("] GUID[") << pkPlayer->GetID() << _T("] MemberGUID[") << pkPlayer->GetMemberGUID() 
						<< _T("] GroundNo[") << GroundKey().GroundNo() << _T("] HackingType[ProjectileHacking] Action ID[") << kAction.iActionID << _T("] MaxTarget[") << iMaxTarget << _T("] ProjectileTarget[") << byTargetNum << _T("]");
					//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg);
					HACKING_LOG(BM::LOG_LV0, kLogMsg);

					if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
					{
						pkPlayer->SendWarnMessageStr(kLogMsg);
					}

					if (pkPlayer->SuspectHacking(EAHP_Projectile, PgGround::ms_kAntiHackCheckVariable.sProjectile_AddIndex))
					{
						// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
						BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_Projectile_Hack) );
						kDPacket.Push( pkPlayer->GetMemberGUID() );
						SendToServer( pkPlayer->GetSwitchServer(), kDPacket );						
					}
					return;
				}
			}
		}
	}

	BM::GUID kTargetGuid;
	BYTE bySphereIndex = 0;
	CUnit* pkTarget = NULL;
	for (BYTE byI=0; byI<byTargetNum; ++byI)
	{
		pkPacket->Pop(kTargetGuid);
		pkPacket->Pop(bySphereIndex);
		pkTarget = PgObjectMgr::GetUnit(kTargetGuid);
		if (pkTarget != NULL)
		{
			if ( (true == bDamageAction) && (kCheckGuid != pkTarget->Caller()) )
			{
				if (PgGround::ms_kAntiHackCheckVariable.bUse_Projectile)
				{
					PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkSendUnit);
					if(pkPlayer)
					{
						bHackingUser = true;

						// �̰� ��Ŀ��
						BM::vstring kLogMsg;
						kLogMsg << __FL__ << _T("[HACKING][Projectile] User[") << pkPlayer->Name() << _T("] GUID[") << pkPlayer->GetID() << _T("] MemberGUID[") << pkPlayer->GetMemberGUID() 
							<< _T("] GroundNo[") << GroundKey().GroundNo() << _T("] HackingType[ProjectileHacking] Action ID : ") << kAction.iActionID;
						//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg);
						HACKING_LOG(BM::LOG_LV0, kLogMsg);
						if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
						{
							pkPlayer->SendWarnMessageStr(kLogMsg);
						}

						if (pkPlayer->SuspectHacking(EAHP_Projectile, PgGround::ms_kAntiHackCheckVariable.sProjectile_AddIndex))
						{
							// SpeedHack�� �ǽɽ������ �������� ���� ��Ų��.
							BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_Projectile_Hack) );
							kDPacket.Push( pkPlayer->GetMemberGUID() );
							SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
						}
					}
				}
				// ��ŷ�ص� ��ŷ�� ������ ������ �޾ƾ� �ϴϱ� Loop�� �� ���ƺ���
			}
			else
			{
				if ((false == bDamageAction) && (pkCaster->IsUnitType(UT_SUMMONED)))
				{
					//��ȯü�� �߻�ü�� �߻��� ��� ��� �Ҳ���?
					//PVP������ ��ȯü�� ������ ������ �� �ִµ� ��ŷ ó���� ��� �Ұ�����;;
				}
				else if ((false == bDamageAction) && (true == PgGround::ms_kAntiHackCheckVariable.bUse_Projectile))
				{
					// Player(or Entity) �� �ٸ� Ÿ���� �����Ѵ�.. Hacking �˻�����.
					HRESULT hResult = pkCaster->CheckClientProjectile(kAction.iActionID, kAction.iActionInstanceID, static_cast<BYTE>(kAction.uProjectileUID), pkTarget->GetPos(), pkTarget->GetAbil(AT_UNIT_SIZE_XY), (byI==0) ? true : false);

					if (S_OK != hResult)
					{
						if (hResult == E_ANTIHACK_DISTANCE)
						{
							// �Ÿ� �˻�� Network Lag �� ���� �߻��� ���� �ִ� ���� �̹Ƿ�.
							// ��ŷ���� ���� ���� �ʰ�, �������� ���� �ʵ��� �Ѵ�.
							return;
						}
						if (hResult == E_ANTIHACK_CANNOTFIND && kAction.iActionID == 102100101)
						{
							// Client �����ε�, ������ �������鼭 �ü��⺻���� ��� �׼���Ŷ�� �ȿ´�.
							return;
						}
						PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkSendUnit);
						if (pkPlayer)
						{
							BM::vstring kLogMsg;
							kLogMsg << __FL__ << "[HACKING][Projectile] User[" << pkPlayer->Name() << "] GUID[" << pkPlayer->GetID() << "] MemberGUID[" << pkPlayer->GetMemberGUID() 
								<< "] GroundNo[" << GroundKey().GroundNo() << "] Cause=" << (hResult-E_FAIL) << ", ActionID=" << kAction.iActionID << ", InstanceID=" << kAction.iActionInstanceID
								<< ", ProjectileID=" << kAction.uProjectileUID;
							//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg);
							HACKING_LOG(BM::LOG_LV0, kLogMsg);
							if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
							{
								pkPlayer->SendWarnMessageStr(kLogMsg);
							}

							if (pkPlayer->SuspectHacking(EAHP_Projectile, PgGround::ms_kAntiHackCheckVariable.sProjectile_AddIndex))
							{
								BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_Projectile_Hack) );
								kDPacket.Push( pkPlayer->GetMemberGUID() );
								SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
							}
						}
						return;
					}
				}
				kTargetArray.Add(pkTarget);
				// kActionResultVec.GetResult �� NULL�� �ƴ��� ����.
				PgActionResult* pkAResult = kActionResultVec.GetResult(kTargetGuid, true);
				if(pkAResult)
				{
					pkAResult->SetCollision(bySphereIndex);
				}
			}
		}
	}

	if(bHackingUser)
	{
		// ���� ���� ��Ŷ�� ���� �ƴѳ���� ����ִٸ� ��ŷ
		// ���� Ÿ�� ����Ʈ���� ���� ���� ��쵵 ������ ���� ������ �����ؼ� �°� �Ѵ�.			
		kTargetArray.Add(pkSendUnit);
		PgActionResult* pkAResult = kActionResultVec.GetResult(kCheckGuid, true);
		if(pkAResult)
		{
			pkAResult->SetCollision(0);
		}
	}

	if ( kTargetArray.empty() )
	{
		return;
	}
	
	
	pkCaster->UseRandomSeedType(false);

	// Skill : Target �� �����Ű��
	SActionInfo kActionInfo;
	kActionInfo.byActionParam = ESS_TARGETLISTMODIFY;
	kActionInfo.iActionID = kAction.iActionID;
	kActionInfo.iActionInstanceID = kAction.iActionInstanceID;
	if (FireSkill(pkCaster, kTargetArray, kActionInfo, &kActionResultVec))	// Success
	{
		// Send Broadcasting
		BYTE byResultNum = kActionResultVec.GetValidCount();

		BM::Stream kPPacket(PT_M_C_NFY_PACTION, kAction);
		kActionResultVec.WriteToPacket(kPPacket);
//		kPPacket.Push(pkCaster->LastRandomSeed());
//		kPPacket.Push(pkCaster->RandomSeedCallCounter());
		kPPacket.Push(fLifeTime);

		SendToArea(&kPPacket, pkCaster->LastAreaIndex(), BM::GUID::NullData(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );
	}

	pkCaster->UseRandomSeedType(true);
}

void PgGround::SendNfyMapLoaded( PgPlayer* pkUser )
{
	BM::Stream kComplete(PT_M_C_NFY_MAPLOADED);//���ø�Ʈ
	g_kEventView.WriteToPacket(false, kComplete);

	kComplete.Push(static_cast<DWORD>(GetState()));
	m_kStoneCtrl.WriteToPacket(pkUser->GetID(), kComplete);//���ø�Ʈ�� Stone���� ����
	m_kGroundOwnerInfo.WriteToPacket( kComplete );

	// BigArea�� ������ ���� ���� �־�� �Ѵ�.
	UNIT_PTR_ARRAY kBigAreaUnitArray;
	if ( m_pkBigArea )
	{
		m_pkBigArea->GetUnitList( kBigAreaUnitArray, pkUser, UT_NONETYPE, SYNC_TYPE_SEND_ADD );
	}

	kBigAreaUnitArray.WriteToPacket( kComplete, WT_SIMPLE );

	PgWorldEventMgr::WorldEventWriteToPacket( kComplete );

	PgWEClientObjectServerMgr::WEClientObjectWriteToPacket( kComplete );
	kComplete.Push( static_cast< int >(DynamicGndAttr()) );
	{
		kComplete.Push( m_kContTrigger.size() );
		CONT_GTRIGGER::const_iterator triger_iter = m_kContTrigger.begin();
		while( m_kContTrigger.end() != triger_iter )
		{
			(*triger_iter).second->WriteToPacket(kComplete);
			++triger_iter;
		}
	}
	kComplete.Push( m_kWorldEnvironmentStatus.GetFlag() );
	PU::TWriteTable_AM(kComplete, m_kRealmQuestInfo);

	kComplete.Push( m_kContEmporiaPortal.size() );
	CONT_EMPORIA_PORTAL::const_iterator po_itr = m_kContEmporiaPortal.begin();
	for ( ; po_itr != m_kContEmporiaPortal.end() ; ++po_itr )
	{
		po_itr->second.WriteToPacket_ToClient( kComplete );
	}

	this->WriteToPacket_AddMapLoadComplete( kComplete );

	PgAlramMission &rkAlramMission = pkUser->GetAlramMission();
	if ( PgAlramMission::STATE_PACKET == rkAlramMission.GetState() )
	{
		if ( IsAlramMission() )
		{
			if ( S_OK == m_pkAlramMissionMgr->GetNewAction( pkUser->GetAbil(AT_CLASS), rkAlramMission ) )
			{
				rkAlramMission.WriteToPacket_Client( kComplete );
			}
		}
		else
		{
			rkAlramMission = PgAlramMission();//�ʱ�ȭ
		}
	}
	
	pkUser->Send( kComplete, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND ); // ����

	PgSmallArea* pkArea = GetArea( pkUser->LastAreaIndex() );
	if( !pkArea )
	{
		POINT3 const& rkPos = pkUser->GetPos();
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Can't Find ") << pkUser->Name() << _T("'s SmallArea Index=") << pkUser->LastAreaIndex() << _T("Pos=") << rkPos.x << _T(",") << rkPos.y  << _T(",") << rkPos.z );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkArea is NULL"));
	}

	SendAreaData(pkUser, pkArea, NULL);//���� ����	

	// ���� �ʱ�ȭ�� ���⼭ ���־���� Owner�� ��Ȯ�� ��Ŷ�� ���� �� �ִ�!!!
	PgPet * pkPet = GetPet( pkUser );
	if ( pkPet )
	{
		if ( !pkPet->CanNfyAddEffect() )
		{
			pkPet->DoEffect();
			pkPet->SetAddPetToOwner();
		}
		
		// Passive skill setting						
		PgActPet_RefreshPassiveSkill kRefreshPassiveSkillAction( this );
		kRefreshPassiveSkillAction.DoAction( pkPet );
	}

	//��ȯü
	PgSummoned * pkSummoned = NULL;
	VEC_SUMMONUNIT const kContSummonUnit = pkUser->GetSummonUnit();
	for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
	{
		pkSummoned = dynamic_cast<PgSummoned*>(GetUnit((*c_it).kGuid));
		if(pkSummoned && !pkSummoned->CanNfyAddEffect() )
		{
			pkSummoned->DoEffect();
			pkSummoned->SetAddSummonedToOwner();
		}

		//��ȯü ��ȯ�翡�� �нú� ����
		PgAction_MapLoaded::RefreshPassiveSkill(pkSummoned, this, dynamic_cast<PgPlayer*>(pkUser));
	}

	BM::Stream kJobSkillPacket;
	JobSkillLocationMgr().WriteToPacketLocation(kJobSkillPacket);
	pkUser->Send( kJobSkillPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );

	pkUser->SetAbil(AT_EVENT_SCRIPT, 0); // �ʱ�ȭ
	pkUser->SetAbil(AT_EVENT_SCRIPT_TIME, 0); // �ʱ�ȭ
	BM::Stream LockPacket(PT_M_C_REQ_UNLOCK_INPUT_EVENT_SCRIPT);
	pkUser->Send(LockPacket, E_SENDTYPE_SELF);
}

BM::GUID const & PgGround::GetFollowingHead(CUnit* pkTail, bool const bGotoEnd, short int sCallDepth)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if (pkTail == NULL)
	{
		return BM::GUID::NullData();
	}
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkTail);
	BM::GUID const & rkHead = pkPlayer->FollowingHeadGuid();
	if (rkHead == BM::GUID::NullData())
	{
		return (sCallDepth == 0) ? BM::GUID::NullData() : pkPlayer->GetID();
	}
	PgPlayer* pkHeadPlayer = dynamic_cast<PgPlayer*>(PgObjectMgr::GetUnit(UT_PLAYER, rkHead));
	if (pkHeadPlayer == NULL)
	{
		pkPlayer->FollowingHeadGuid(BM::GUID::NullData());
		return (sCallDepth == 0) ? BM::GUID::NullData() : pkPlayer->GetID();
	}
	if (bGotoEnd)
	{
		if (sCallDepth < MAX_FOLLOWING_MODE_LENGTH)
		{
			return GetFollowingHead(pkHeadPlayer, bGotoEnd, sCallDepth+1);
		}
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Folling Length to long CurrentLength["<<sCallDepth<<L"]");
	}
	return (sCallDepth == 0) ? BM::GUID::NullData() : pkPlayer->GetID();
}

int PgGround::GetUnitAbil(BM::GUID const & rkGuid, WORD const wType)
{
	BM::CAutoMutex kLock( m_kRscMutex );

	CUnit const* pkUnit = PgObjectMgr::GetUnit(UT_PLAYER, rkGuid);
	if (pkUnit != NULL)
	{
		return pkUnit->GetAbil(wType);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

void PgGround::OrderSyncGameTime(BM::Stream* const pkSendPacket)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_OBJECT_MGR_UNIT::iterator unit_itr;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
	CUnit* pkUnit = NULL;
	while ( (pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL )
	{
		// ��� ������ NotifyTime�� �ʱ�ȭ �ϰ� ��Ŷ�� �ٽ� ������.
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if (pkPlayer != NULL)
		{
			pkPlayer->NotifyTime(0);
			pkPlayer->Send(*pkSendPacket);
		}
	}
}

void PgGround::DequeTargetToUNIT_PTR_ARRAY(CUnit::DequeTarget const &rkDeque, UNIT_PTR_ARRAY& rkArr)
{
	BM::CAutoMutex kLock( m_kRscMutex );
	CUnit::DequeTarget::const_iterator deq_it = rkDeque.begin();
	while(deq_it!=rkDeque.end())
	{
		CUnit* pkFind = GetUnit((*deq_it).kGuid);
		if(pkFind)
		{
			rkArr.Add(pkFind);
		}
		++deq_it;
	}
}

NxShape* PgGround::RayCast(NxRay& rkWorldRay, NxRaycastHit& rkHit, NxReal maxDist, NxU32 hintFlags) const
{ 
	if(fabsf(rkWorldRay.dir.magnitudeSquared()-1)>=1e-4)	//������ ���̰� 1��Ÿ ũ��
	{
		rkWorldRay.dir.normalize();
	}
	else
	{
		if ( rkWorldRay.dir.isZero() )
		{
			INFO_LOG(BM::LOG_LV6, __FL__<<" rkWorldRay.dir is Zero!! MapNo : "<<GroundKey().GroundNo()<<" origin X : "<<rkWorldRay.orig.x<<" origin Y : "<<rkWorldRay.orig.y<<" origin Z : "<<rkWorldRay.orig.z);
			return NULL;
		}
	}
		
	return PhysXScene()->GetPhysXScene()->raycastClosestShape(rkWorldRay, NX_STATIC_SHAPES, rkHit, -1, maxDist, hintFlags);;
}

bool PgGround::CheckPathNormal(CUnit* pkUnit, CSkillDef const* pkSkillDef, SActionInfo const& rkAction, float const fErrorDelta)
{
	if(GetUnlockBidirection() & pkUnit->UnitType())
	{
		return true;
	}

	if(pkSkillDef && ESArea_Sphere==pkSkillDef->GetAbil(AT_1ST_AREA_TYPE))	//Ÿ���� Ÿ���� ������ ��� �н��븻 üũ�� �ҿ����
	{
		return true;
	}
	NiPoint3 kDir(rkAction.ptDirection.x, rkAction.ptDirection.y, rkAction.ptDirection.z);
	kDir.Unitize();
	NiPoint3 kPathNormal(rkAction.ptPathNormal.x, rkAction.ptPathNormal.y, 0);
	kPathNormal.Unitize();
	float const fAngle = fabs(kDir.Dot(kPathNormal))*90.0f;
	if(fErrorDelta < fAngle)//���ݹ���� �н��븻�� ������ �ƴ�
	{
		INFO_LOG(BM::LOG_LV5, __FL__<<" Dir and PathNormal is Not Cross. Name : "<<pkUnit->Name()<<" Angle : "<<fAngle);
		return false;
	}

	NiPick kPick;
	kPick.SetTarget(GetPathRoot());
	kPick.SetReturnNormal( true );
	NiPoint3 kOpPathNormal(kPathNormal.x * -1, kPathNormal.y * -1, 0);//Ŭ�󿡼� ������ �н������� ���������� ������

	kPick.PickObjects( NiPoint3(rkAction.ptPos.x, rkAction.ptPos.y, rkAction.ptPos.z+25.0f), kOpPathNormal, true );	//Ŭ�󿡼��� 25���⶧����
	if ( kPick.GetSize() )
	{
		NiPoint3 const kNiNormal = kPick.GetResults().GetAt(0)->GetNormal();
		if( fErrorDelta>=fabs(90.0f - kNiNormal.Dot(kPathNormal)*90.0f))
		{
			return true;//�� ������ ��ġ�ؾ� ��
		}
		else
		{
			INFO_LOG(BM::LOG_LV5, __FL__<<" Server PathNormal and Client PathNormal is Not SameDir. Name : "<<pkUnit->Name());
		}
	}

	INFO_LOG(BM::LOG_LV5, __FL__<<" Cannot Find PathNormal. Name : "<<pkUnit->Name());
	return false;
}

void PgGround::SetAntiHackVariable()
{
	// ms_kAntiHackCheckActionCount
	PgGround::ms_kAntiHackCheckActionCount.m_bUseAntiHackCheckActionCount = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_ActionCount);

	int iValue = 0;
	float fValue = 0;
	if (S_OK == g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_ActionCount, EHack_CVar_ActionCount_ForcedResultView, iValue))
	{
		PgGround::ms_kAntiHackCheckActionCount.m_bForcedResultView = (0 == iValue) ? false : true;
	}
	else
	{
		PgGround::ms_kAntiHackCheckActionCount.m_bForcedResultView = false;
	}
	if (S_OK == g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_ActionCount, EHack_CVar_ActionCount_ResultToPlayer, iValue))
	{
		PgGround::ms_kAntiHackCheckActionCount.m_bResultToPlayer = (0 == iValue) ? false : true;
	}
	else
	{
		PgGround::ms_kAntiHackCheckActionCount.m_bResultToPlayer = false;
	}
	if (S_OK == g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_ActionCount, EHack_CVar_ActionCount_Use_HackingPoint,iValue))
	{
		PgGround::ms_kAntiHackCheckActionCount.m_bHackingIndex = (0 == iValue) ? false : true;
	}
	else
	{
		PgGround::ms_kAntiHackCheckActionCount.m_bHackingIndex = false;
	}
	g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_ActionCount, EHack_CVar_ActionCount_CheckTime,			PgGround::ms_kAntiHackCheckActionCount.m_iActionCount_CheckTime);
	g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_ActionCount, EHack_CVar_ActionCount_ActionCount,			PgGround::ms_kAntiHackCheckActionCount.m_iCheckCountSum);
	PgGround::ms_kAntiHackCheckActionCount.m_iActionCount_CheckTime_Mutiflier = static_cast<int>(g_kAntiHackMgr.Locked_GetPolicy_IndexMutiflier(EAHP_ActionCount));
	PgGround::ms_kAntiHackCheckActionCount.m_iAddHackingPoint = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_ActionCount);

	// PgPlayer::m_sMaxHackIndex
	PgPlayer::m_sMaxHackIndex.resize(EAHP_POLICY_MAX+1);
	for (int i = 1; i <= EAHP_POLICY_MAX; ++i)
	{
		PgPlayer::m_sMaxHackIndex[i] = g_kAntiHackMgr.Locked_GetPolicy_MaxHackIndex(static_cast<EAntiHack_Policy>(i));
	}

	// ms_kAntiHackCheckVariable
	PgGround::ms_kAntiHackCheckVariable.bUse_GameTimeOver = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_CheckGameTimeOver);
	PgGround::ms_kAntiHackCheckVariable.sGameTimeOver_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_CheckGameTimeOver);

	PgGround::ms_kAntiHackCheckVariable.bUse_GameTimeUnder = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_CheckGameTimeUnder);
	PgGround::ms_kAntiHackCheckVariable.sGameTimeUnder_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_CheckGameTimeUnder);

	PgGround::ms_kAntiHackCheckVariable.bUse_MaxTarget = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_MaxTarget);
	PgGround::ms_kAntiHackCheckVariable.sMaxTarget_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_MaxTarget);

	PgGround::ms_kAntiHackCheckVariable.bUse_SkillRange = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_SkillRange);
	PgGround::ms_kAntiHackCheckVariable.sSkillRange_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_SkillRange);

	PgGround::ms_kAntiHackCheckVariable.bUse_MoveSpeed = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_MoveSpeed);
	PgGround::ms_kAntiHackCheckVariable.sMoveSpeed_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_MoveSpeed);
	g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_MoveSpeed, EHack_CVar_MoveSpeed_BoundRate, PgGround::ms_kAntiHackCheckVariable.fMoveSpeed_BoundRate);
	g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_MoveSpeed, EHack_CVar_MoveSpeed_HackingCounter, PgGround::ms_kAntiHackCheckVariable.sMoveSpeed_HackingCount);

	PgGround::ms_kAntiHackCheckVariable.bUse_Projectile = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_Projectile);
	PgGround::ms_kAntiHackCheckVariable.sProjectile_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_Projectile);

	PgGround::ms_kAntiHackCheckVariable.bUse_OpenMarket = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_OpenMarket);
	PgGround::ms_kAntiHackCheckVariable.sOpenMarket_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_OpenMarket);

	PgGround::ms_kAntiHackCheckVariable.bUse_CashShop = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_CashShop);
	PgGround::ms_kAntiHackCheckVariable.sCashShop_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_CashShop);

	PgGround::ms_kAntiHackCheckVariable.bUse_NPCRange = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_NPCRange);
	PgGround::ms_kAntiHackCheckVariable.sNPCRange_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_NPCRange);

	PgGround::ms_kAntiHackCheckVariable.bUse_ClientNotify = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_ClientNotify);
	PgGround::ms_kAntiHackCheckVariable.sClientNotify_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_ClientNotify);

	PgGround::ms_kAntiHackCheckVariable.bUse_AntiMacro = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_AntiMacro);
	PgGround::ms_kAntiHackCheckVariable.sAntiMacro_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_AntiMacro);

	PgGround::ms_kAntiHackCheckVariable.bUse_HyperMove = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_HyperMove);
	PgGround::ms_kAntiHackCheckVariable.sHyperMove_WrongGuid_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_HyperMove);
	g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_HyperMove, EHack_CVar_HyperMove_WrongDistance_HackIndex_Add, PgGround::ms_kAntiHackCheckVariable.sHyperMove_WrongDistance_AddIndex);

	PgGround::ms_kAntiHackCheckVariable.bUse_Blowup = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_Blowup);
	PgGround::ms_kAntiHackCheckVariable.sBlowup_AddIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_Blowup);

	PgGround::ms_kAntiHackCheckVariable.bUse_RandomSeedCallCounter = g_kAntiHackMgr.Locked_GetPolicy_Enabled(EAHP_RandomSeedCallCounter);
	PgGround::ms_kAntiHackCheckVariable.sRandomSeedCallCounter_AddHackIndex = g_kAntiHackMgr.Locked_GetPolicy_IndexAdd(EAHP_RandomSeedCallCounter);
	g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_RandomSeedCallCounter, EHack_CVar_RandomSeedCallCounter_CheckValue, PgGround::ms_kAntiHackCheckVariable.iRandomSeedCallCounter_CheckValue);

}

void PgGround::GetClinetProjectileInfo(CUnit* const pkCaster, SActionInfo const & rkAction, CSkillDef const* pkSkillDef, SClientProjectile & kInfo) const
{
	kInfo.ptFirePos = pkCaster->GetPos();
	kInfo.ptDirection = rkAction.ptDirection;
	kInfo.iActionID = rkAction.iActionID;
	switch (rkAction.iActionID)
	{
	case 104301101:	// a_Gimmlet (�����ڵ�), Ŭ���̾�Ʈ ������ �̺κ� ����
	case 104301102:
	case 104301103:
	case 104301104:
	case 104301105:
		{
			kInfo.byMaxTargetPerProjectile = 20;
			kInfo.fDmgRangeQ *= 1.7f;
		}break;
	default:
		{
			kInfo.byMaxTargetPerProjectile = 1;
		}break;
	}

	// ������ʹ� ��ų���� ������ �־�� �� ��
	kInfo.dwExpireTime = BM::GetTime32() + 15000;	// �ϵ��ڵ�...�ִ밪...
	kInfo.byMaxProjectile = std::max(1, pkSkillDef->GetAbil(AT_PROJECTILE_NUM));
	// Range���� �ܺο��� �����Ѵ�.
	//kInfo.fDmgRangeQ = pow(iSkillRange, 2) * 1.5;
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if (pkPlayer)
	{
		int iAddedTarget = 0;
		int iSkillindex = 0;
		PgMySkill* const pkPlayerSkill = pkPlayer->GetMySkill();
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		while(CHILD_SKILL_MAX > iSkillindex)
		{
			//���� �޴� ���̽� ��ų
			int const iChildBaseSkillNo = pkSkillDef->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
			if(0 < iChildBaseSkillNo)
			{
				if(pkPlayerSkill)
				{
					//�ش� ��ų�� ���� ������ �ش�Ǵ� ��ų�� ��� �´�.
					int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
					if(0 < iLearnedChildSkillNo)
					{
						CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo);
						if (pkLearnedSkillDef)
						{
							iAddedTarget += pkLearnedSkillDef->GetAbil(AT_PROJECTILE_NUM);						
						}
					}
				}
			}
			else
			{
				break;
			}
			++iSkillindex;
		}
		if (iAddedTarget > 0)
		{
			kInfo.byMaxProjectile = iAddedTarget;
		}
	}
	if (0 < pkSkillDef->GetAbil(AT_PENETRATION_USE_TYPE))	// ���� ����ϴ°�?
	{
		kInfo.byMaxTargetPerProjectile += pkCaster->GetAbil(AT_PENETRATION_COUNT);
		kInfo.fDmgRangeQ *= 3.0f;	// ������� �Ÿ� ���� �ִ�..(Ŭ���̾�Ʈ ���� �ʿ�)
	}

	switch (rkAction.iActionID)
	{
	case 110003001:	// a_HE Granade
	case 110003002:
	case 110003003:
	case 110003004:
	case 110003005:
		{
			kInfo.fDmgRangeQ *= 6.0f;
		}break;
	default:
		{
		}break;
	}
}

void PgGround::SetChangeGroundOwnerInfo( SGroundOwnerInfo const &kInfo, bool const bBroadCast )
{
	if ( kInfo != m_kGroundOwnerInfo )
	{
		m_kGroundOwnerInfo = kInfo;
		if ( true == bBroadCast )
		{
			BM::Stream kPacket( PT_M_C_NFY_GROUNDOWNER_INFO );
			m_kGroundOwnerInfo.WriteToPacket( kPacket );
			Broadcast( kPacket );
		}

		// ������ üũ�Ͽ� �i�Ƴ��� �Ѵ�.
		std::list< PgPlayer * >	kOutPlayerList;

		{
			PgPlayer* pkPlayer = NULL;
			CONT_OBJECT_MGR_UNIT::iterator kItor;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
			pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor));
			while ( pkPlayer )
			{
				if ( !this->IsAccess(pkPlayer) )
				{
					kOutPlayerList.push_back( pkPlayer );
				}

				pkPlayer = dynamic_cast<PgPlayer*>(PgObjectMgr::GetNextUnit(UT_PLAYER, kItor));
			}
		}

		std::list< PgPlayer * >::iterator player_itr = kOutPlayerList.begin();
		for ( ; player_itr != kOutPlayerList.end() ; ++player_itr )
		{
			this->RecvRecentMapMove( *player_itr );
		}
	}
}

void PgGround::CheckEffectUser(CUnit* pkUnit)
{
	if( this->GetAttr() & GATTR_MARRY )
	{
		// Center�� ���� ���� ��ȥ �ϴ��� üũ�� �ؼ� ���� ��ü�� �õ��� ����.
		BM::Stream kPacket(PT_C_M_REQ_MARRY_COMMAND, static_cast<BYTE>(MC_EffectCheck));
		kPacket.Push(pkUnit->GetID());
		kPacket.Push(GroundKey());
		SendToCenter(kPacket);
	}
	/*else
	{
		int iEffectNo = 0;
		if( GWL_MALE == pkUnit->GetAbil(AT_GENDER) )
		{
			iEffectNo = m_kMarryMgr.GetEffectMALE();
		}
		else
		{
			iEffectNo = m_kMarryMgr.GetEffectFEMALE();
		}
		CEffect const* pkEffect = pkUnit->GetEffect(iEffectNo, true);
		if(NULL != pkEffect)
		{
			if( 0 < iEffectNo )
			{
				pkUnit->DeleteEffect(iEffectNo);
			}
		}
	}*/
}

void PgGround::HiddenMapCheck(CUnit* pkUnit)
{
	if( this->GetAttr() & GATTR_HIDDEN_F )
	{
		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if( pkPlayer )
		{
			int const iGroundNo = GetGroundNo();

			if( 0 != iGroundNo )
			{
				const CONT_DEFMAP* pkContDefMap = NULL;
				g_kTblDataMgr.GetContDef(pkContDefMap);

				if(!pkContDefMap)
				{
					INFO_LOG(BM::LOG_LV0, __FL__ << L"Cannot find ContDefMap");
					return;
				}

				CONT_DEFMAP::const_iterator itor = pkContDefMap->find(iGroundNo);
				if (itor == pkContDefMap->end())
				{
					INFO_LOG(BM::LOG_LV0, __FL__ << L"Cannot find DefMap MapNo[" << iGroundNo << "]" );
					return;
				}

				const TBL_DEF_MAP& rkDefMap = itor->second;
				if( false == pkPlayer->GetHiddenOpen()->IsComplete(rkDefMap.sHiddenIndex) )
				{
					CONT_PLAYER_MODIFY_ORDER kCont;

					kCont.push_back(SPMO(IMET_COMPLETE_HIDDEN_OPEN, pkPlayer->GetID(), SPMOD_Complete_HiddenOpen(rkDefMap.sHiddenIndex, 1)));

					PgAction_ReqModifyItem kItemModifyAction(CAE_HiddenOpen, GroundKey(), kCont,BM::Stream(),true);

					if( !pkUnit )
					{
						return;
					}

					kItemModifyAction.DoAction(pkUnit, NULL);
				}
			}
		}
	}
}

bool PgGround::PickUpGroundBox(CUnit* pkUnit, BM::GUID const& rkGuid, CUnit* pkLooter)
{
	CUnit *pkTargetUnit = PgObjectMgr::GetUnit(UT_GROUNDBOX, rkGuid);
	if( pkTargetUnit )
	{
		PgAction_PickUpItem kPickUpAction( this, this->m_kLocalPartyMgr, pkLooter );
		bool const bPickItem = kPickUpAction.DoAction( pkUnit, pkTargetUnit);
		if( true == bPickItem )
		{
			this->ReleaseUnit( pkTargetUnit, false, false );//����
			return true;
		}
	}

	return false;
}

void PgGround::GetStatisticsPlayTime(__int64 i64Now, SMapPlayTime& rkOut)
{
	BM::CAutoMutex kLock( m_kRscMutex );

	// ���� �������� ������ Playtime �� �ջ�
	CUnit *pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
	{
		PgPlayer const* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if (pkPlayer != NULL)
		{
			__int64 i64Enter = i64Now - std::max<__int64>(pkPlayer->GroundEnterTimeSec(), LastPlayTimeCalculateTime());
			m_kPlayTime.iPlayTime += i64Enter;
		}
	}

	rkOut = PlayTime();

	LastPlayTimeCalculateTime(i64Now);
	m_kPlayTime.Clear();
}

void PgGround::AddEffectToAllPlayer(int const iEffectNo)
{
	BM::CAutoMutex kLock( m_kRscMutex );

	SEffectCreateInfo kCreate;
	kCreate.eType = EFFECT_TYPE_NORMAL;
	kCreate.iEffectNum = iEffectNo;
	PgGroundUtil::SetActArgGround(kCreate.kActArg, this);
	kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

	CUnit *pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
	{
		PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
		if( pkPlayer != NULL )
		{
			pkPlayer->AddEffect(kCreate);
		}
	}
}

int PgGround::GetMapAbil(WORD const wType)
{
	// lock ����
	// ���� �ܺ� ��ü�� ȣ���Ϸ� �Ѵٸ�(=public �Լ�) lock �� ���־�� �Ѵ�.
	CONT_MAPABIL::const_iterator itor_abil = m_kContMapAbil.find(wType);
	if (itor_abil != m_kContMapAbil.end())
	{
		return (*itor_abil).second;
	}

	GET_DEF(PgDefMapMgr, kDefMap);
	int iValue = kDefMap.GetAbil(GroundKey().GroundNo(), wType);
	m_kContMapAbil.insert(std::make_pair(wType, iValue));
	return iValue;
}

void PgGround::SetEventQuest(PgEventQuest const & rkNewEventQuest)
{
	BM::CAutoMutex kLock( m_kRscMutex );
	m_kEventQuest = rkNewEventQuest;
}

void PgGround::OnPT_T_C_NFY_MARRY(EMarryState const eCommandType, BM::Stream *const pkPacket)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	
	
	switch( eCommandType )
	{
	case EM_MONSTER:
		{
			int iMonsterNo = 0;
			int iMonsterCount = 0;
			BM::GUID kCasterGuid;
			SGroundKey kGndKey;

			pkPacket->Pop( iMonsterNo );
			pkPacket->Pop( iMonsterCount );
			pkPacket->Pop( kCasterGuid );
			pkPacket->Pop( kGndKey );

			if(iMonsterCount < 1 )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				break;
			}					

			POINT3 kCurPos1(-50, 161, 1540);
			POINT3 kCurPos2(435, 182, 1540);

			BM::GUID kMonsterGuid;
			for( int i=0; i<iMonsterCount; ++i)
			{
				int const iSpreadRange = 200;
				int m_iLength = __max(2, iSpreadRange);
				int x = BM::Rand_Range(0, m_iLength);
				int y = BM::Rand_Range(0, m_iLength);
				int const iRandScaleX = (BM::Rand_Index(2) == 0) ? x : x*-1;
				int const iRandScaleY = (BM::Rand_Index(2) == 0) ? y : y*-1;

				POINT3 kRegenPos;
				kRegenPos = (BM::Rand_Index(2) == 0) ? kCurPos1 : kCurPos2;
				kRegenPos.x = kRegenPos.x + iRandScaleX;
				kRegenPos.y = kRegenPos.y + iRandScaleY;
				TBL_DEF_MAP_REGEN_POINT kRegenInfo;
				kRegenInfo.iMapNo = GetGroundNo();
				kRegenInfo.pt3Pos = kRegenPos;

				if (E_FAIL == InsertMonster( kRegenInfo, iMonsterNo, kMonsterGuid ))
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					break;
				}
			}
		}break;
	case EM_NPCTALK:
	case EM_MARRY_ACTION:
	case EM_ACTION:
		{
			if ( pkPacket )
			{
				BM::Stream kCenterPacket;
				kCenterPacket.Push( *pkPacket );
				SendToCenter(kCenterPacket);
			}
		}break;
	case EM_NOTICE:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("[EM_NOTICE] Cannot handle this type command"));
		}break;
	case EM_MARRY:
		{
			BM::GUID kGuid;
			BM::GUID kCoupleGuid;
			SGroundKey kGndKey;

			pkPacket->Pop( kGuid );
			pkPacket->Pop( kCoupleGuid );
			pkPacket->Pop( kGndKey );

			CUnit * pkUnit = GetUnit(kGuid);;
			if(NULL == pkUnit)
			{
				break;
			}

			if( kCoupleGuid == pkUnit->GetCoupleGuid() )
			{
				CUnit * pkCoupleUnit = GetUnit(kCoupleGuid);;
				if(NULL == pkCoupleUnit)
				{
					break;
				}

				BM::GUID kMineGuid = kGuid;
				bool bSayYes = true;
				int const iQuestID = 588;
				int const iSkillNo = 90000002;

				BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND, pkUnit->GetID());
				kPacket.Push( (BYTE)CC_Ans_SweetHeart_Complete );
				kPacket.Push( kCoupleGuid );
				kPacket.Push( bSayYes );
				kPacket.Push( iQuestID );
				kPacket.Push( iSkillNo );
				SendToCoupleMgr(kPacket);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("unhandled Marry Commandtype =") << eCommandType);
		}break;
	}

}

CUnit* PgGround::CreateGuardianEntity(CUnit* pkCaller, SCreateEntity* pkCreateInfo, LPCTSTR lpszName, bool bSyncUnit, int const iStartActionID)
{
	BM::CAutoMutex kLock( m_kRscMutex );

    if(!pkCreateInfo)
    {
        VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"CreateEntity NULL Data");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
        return NULL;
    }

	PgEntity *pkEntity = dynamic_cast<PgEntity*>(g_kTotalObjMgr.CreateUnit(UT_ENTITY, pkCreateInfo->kGuid));
	if (pkEntity == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot CreateGuardianEntity Guid["<<pkCreateInfo->kGuid<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	SEntityInfo kInfo(pkCreateInfo->kGuid, pkCreateInfo->kClassKey);
	kInfo.bSyncUnit = bSyncUnit;
	kInfo.bEternalLife = pkCreateInfo->bEternalLife;
    kInfo.iTunningNo = pkCreateInfo->iTunningNo;
    kInfo.iTunningLevel = pkCreateInfo->iTunningLevel;

	Direction eFrontDir = DIR_DOWN;
	if ( pkCaller )
	{
		kInfo.kCaller = pkCaller->GetID();
		kInfo.kCallerClassKey.iClass = pkCaller->GetAbil(AT_CLASS);
		kInfo.SetAbil(AT_TEAM, pkCaller->GetAbil(AT_TEAM));
		kInfo.SetAbil(AT_OWNER_TYPE, pkCaller->GetAbil(AT_OWNER_TYPE));
		kInfo.SetAbil(AT_CALLER_TYPE, pkCaller->UnitType());
		kInfo.SetAbil(AT_CALLER_LEVEL, pkCaller->GetAbil(AT_LEVEL));
		kInfo.SetAbil(AT_ENABLE_CHECK_ATTACK, pkCaller->GetAbil(AT_ENABLE_CHECK_ATTACK));
		kInfo.SetAbil(AT_ENTITY_TYPE, ENTITY_GUARDIAN);
				
		if ( 0 < pkCaller->GetAbil(AT_USENOT_SMALLAREA) )
		{// Caller�� BigArea�����̸� ��ȯ���� BigArea�� ���� �Ѵ�.
			kInfo.SetAbil( AT_USENOT_SMALLAREA, 1 );
		}
		if( 0 < pkCaller->GetAbil(AT_MON_ENCHANT_LEVEL) )
		{
			kInfo.SetAbil(AT_MON_ENCHANT_LEVEL, pkCaller->GetAbil(AT_MON_ENCHANT_LEVEL));
		}
		kInfo.SetAbil(AT_OFFENCE_ENCHANT_LEVEL, pkCaller->GetAbil(AT_OFFENCE_ENCHANT_LEVEL));
		kInfo.SetAbil(AT_DEFENCE_ENCHANT_LEVEL, pkCaller->GetAbil(AT_DEFENCE_ENCHANT_LEVEL));

		eFrontDir = pkCaller->FrontDirection();
	}
	else
	{
		kInfo.SetAbil(AT_OWNER_TYPE, UT_MONSTER);
		kInfo.SetAbil(AT_CALLER_TYPE, UT_MONSTER);
		kInfo.SetAbil(AT_CALLER_LEVEL, 0);
		kInfo.SetAbil(AT_ENTITY_TYPE, ENTITY_GUARDIAN);
	}
	
	if ( SUCCEEDED(pkEntity->Create( &kInfo )) )
	{
		if ( lpszName )
		{
			pkEntity->Name( lpszName );
		}
		pkEntity->SetPos( pkCreateInfo->ptPos );
		pkEntity->SetState(US_IDLE);
		pkEntity->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );
		pkEntity->FrontDirection( eFrontDir );
        pkEntity->LifeTime( pkCreateInfo->iLifeTime );
		if(iStartActionID)
		{
			pkEntity->ActionID(iStartActionID);
		}

		if ( !pkCaller || pkCaller->AddSummonUnit( pkEntity->GetID(), pkCreateInfo->kClassKey.iClass, PgGroundUtil::GetSummonOptionType(pkCreateInfo->bUniqueClass), GetMaxSummonUnitCount(pkCaller)) )
		{
			if ( GetAttr() & GATTR_MISSION )
			{
				if(PgMissionGround * pkMissionGround = dynamic_cast<PgMissionGround*>(this) )
				{
					//������忡�� ��ü ����� ���ݷ� ��� ��ų�� ��� ���

					CONT_DEFENCE_SKILL kCont;
					pkMissionGround->GetContDefenceMissionSkill(kCont);

					CONT_DEFENCE_SKILL::const_iterator team_it = kCont.find(pkCaller->GetAbil(AT_TEAM));
					if(kCont.end() != team_it)
					{
						GET_DEF(CSkillDefMgr, kSkillDefMgr);
						GET_DEF(CEffectDefMgr, kEffectDefMgr);

						typedef std::map<int, int> CONT_INT;
						CONT_INT kContData;
						int iApplyEffect = 0;
						CONT_SET_DATA::const_iterator learn_it = team_it->second.begin();
						while (learn_it!=team_it->second.end())
						{
							if(CSkillDef const* pkSkill = kSkillDefMgr.GetDef((*learn_it))) /*No Block*/
							if(CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(pkSkill->GetEffectNo())) /*No Block*/
							if(DS_ADD_PC_ATTACK == pkEffectDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE)) /*No Block*/
							if(int const iGuardianEffect = pkSkill->GetAbil(AT_EFFECTNUM1))
							{
								int const iBasic = GET_BASICSKILL_NUM(iGuardianEffect);
								auto kRet = kContData.insert(std::make_pair(iBasic,iGuardianEffect));
								if(false==kRet.second)
								{
									if(kRet.first->second<iGuardianEffect)
									{
										kRet.first->second = iGuardianEffect;
									}
								}
							}
							++learn_it;
						}

						for(CONT_INT::const_iterator c_it=kContData.begin(); c_it!=kContData.end(); ++c_it)
						{
							PgGroundUtil::InsertEffectAbil(pkEntity, (*c_it).second);
						}
					}
				}
			}

			AddUnit(pkEntity, true);

			//��ų�� ����Ҷ� �ڽ� ��ų�� ������ �޴� ��ų�� ����ؾ��ϴ� ���
			//Entity�� Entitny�� ���� ������ �ش�. 
			//Variant�� CS_GetSkillResultDefault ���� �Լ� ȣ�� �� Entity�� Caller�� �����͸� ���� �� ���� ������
			//Caller �� ������ �޴� �ڽ� ��ų�� ������� �� ���� ����. 
			//Entity�� ���� ������ �־ �����ؾ� �Ѵ�.			
			
			if(pkEntity && pkCaller)
			{
				if(int const iSkillNo = pkEntity->GetAbil(AT_MON_SKILL_01))
				{
					if(0 < iSkillNo)
					{
						GET_DEF(CSkillDefMgr, kSkillDefMgr);
						CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
						if(pkSkill)
						{
							if(PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaller))
							{
								int iSkillindex = 0;
								while(CHILD_SKILL_MAX > iSkillindex)
								{
									//���� �޴� ���̽� ��ų
									int const iChildBaseSkillNo = pkSkill->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
									if(0 < iChildBaseSkillNo)
									{
										if(PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill())
										{
											//�ش� ��ų�� ���� ������ �ش�Ǵ� ��ų�� ��� �´�.
											int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
											if(0 < iLearnedChildSkillNo)
											{
												//���� ��ų ��ȣ�� Entity�� �����Ѵ�.
												//Variant�� CS_GetSkillResultDefault ���� �Լ��� ȣ��ɶ� Caller�� �����͸� ���� ��� �� �� ����.
												pkEntity->SetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex, iLearnedChildSkillNo);												
											}
										}
									}
									else
									{
										break;
									}
									++iSkillindex;
								}
							}
						}
					}
				}
			}

			return pkEntity;
		}
	}

	g_kTotalObjMgr.ReleaseUnit( dynamic_cast<CUnit*>(pkEntity) );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

// �ν��Ͻ� ������ ���׷� Ȱ��ȭ.
// GenGroup - 10000 : �߸� ( �ƹ����̳� ���� �� ���� ).
// GenGroup - 10001 : ������ ( �������� �Ծ��� ���� ����Ʈ Ȱ��ȭ ).
// GenGroup - 10002 : ������ ( �������� �Ծ��� ���� ����Ʈ Ȱ��ȭ ).
EOpeningState PgGround::ActivateInstanceItemGenGroup( int const GenGroup, bool const bReset, bool const bOnce, int const Copy)
{
	BM::CAutoMutex Lock( m_kRscMutex );

	SGenGroupKey GenGroupKey;
	GetGenGroupKey(GenGroupKey);

	GET_DEF_CUSTOM(PgGenPointMgr, PgControlDefMgr::EDef_GenPointMgr, GenPointMgr);
	if( false == bReset && true == bOnce )
	{
		ContGenPoint_InstanceItem ContGenPoint;
		if( SUCCEEDED(GenPointMgr.GetGenPoint_Monster( ContGenPoint, GenGroupKey, bReset, GenGroup )) )
		{
			EOpeningState const eRet = InstanceItemGenerate( ContGenPoint, Copy, NULL );

			int Team = GenGroup % 10;
			ContGenPoint_InstanceItem * pContItem = NULL;
			switch( Team )
			{
			case TEAM_NONE:
				{
					pContItem = &m_ContGenPoint_InstanceItem_AnyTeam;
				}break;
			case TEAM_RED:
				{
					pContItem = &m_ContGenPoint_InstanceItem_RedTeam;
				}break;
			case TEAM_BLUE:
				{
					pContItem = &m_ContGenPoint_InstanceItem_BlueTeam;
				}break;
			}

			if(NULL == pContItem)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"MapNo["<<GenGroupKey.iMapNo<<L"] Balance["<<GenGroupKey.iBalance<<L"] GenGroup["<<GenGroup<<L"]");
				return E_OPENING_NONE;
			}

			ContGenPoint_InstanceItem::const_iterator c_itor = ContGenPoint.begin();
			while( c_itor != ContGenPoint.end() )
			{
				auto Ret = (*pContItem).insert( std::make_pair( (*c_itor).first, (*c_itor).second ) );
				if( false == Ret.second )
				{
					OwnChildCont ContChild;
					(*c_itor).second.GetChild(ContChild);
					OwnChildCont::const_iterator it = ContChild.begin();
					while( it != ContChild.end() )
					{
						Ret.first->second.AddChild( (*it) );
						++it;
					}
				}
				++c_itor;
			}
			return eRet;
		}
	}
	else if( bOnce )
	{
		ContGenPoint_InstanceItem ContGenPoint;
		if( SUCCEEDED( GenPointMgr.GetGenPoint_Monster( ContGenPoint, GenGroupKey, bReset, GenGroup ) ) )
		{
			return InstanceItemGenerate(ContGenPoint, Copy, NULL);
		}
	}
	else
	{
		int Team = GenGroup % 10;
		switch( Team )
		{
		case TEAM_NONE:
			{
				if( SUCCEEDED(GenPointMgr.GetGenPoint_Monster( m_ContGenPoint_InstanceItem_AnyTeam, GenGroupKey, bReset, GenGroup )) )
				{
					return InstanceItemGenerate(m_ContGenPoint_InstanceItem_AnyTeam, Copy, NULL);
				}
			}break;
		case TEAM_RED:
			{
				if( SUCCEEDED(GenPointMgr.GetGenPoint_Monster( m_ContGenPoint_InstanceItem_RedTeam, GenGroupKey, bReset, GenGroup )) )
				{
					return InstanceItemGenerate(m_ContGenPoint_InstanceItem_RedTeam, Copy, NULL);
				}
			}break;
		case TEAM_BLUE:
			{
				if( SUCCEEDED(GenPointMgr.GetGenPoint_Monster( m_ContGenPoint_InstanceItem_BlueTeam, GenGroupKey, bReset, GenGroup )) )
				{
					return InstanceItemGenerate(m_ContGenPoint_InstanceItem_BlueTeam, Copy, NULL);
				}
			}break;
		}			
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_OPENING_NONE"));
	return E_OPENING_NONE;
}

EOpeningState PgGround::InstanceItemGenerate(ContGenPoint_InstanceItem & ContGenPoint, int const iCopy, CUnit* pCaller)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	EOpeningState eState = E_OPENING_NONE;
	if( PgGroundUtil::IsBSGround(GetAttr())
		&& false == PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
	{
		return eState;
	}

	int ItemNo;
	BM::GUID ItemGuid;
	ContGenPoint_InstanceItem::iterator gen_point_itor;
	for( gen_point_itor = ContGenPoint.begin() ; gen_point_itor != ContGenPoint.end() ; ++gen_point_itor )
	{
		PgGenPoint & GenPoint = gen_point_itor->second;
		if( GenPoint.CanGen() )
		{
			int _iCopy = iCopy + 1;
			while( _iCopy-- )
			{
				EOpeningState const eTempState = InstanceItemGenerate(GenPoint, ItemNo, ItemGuid, pCaller);
				eState = __max(eTempState, eState);
			}
		}
	}
	return eState;
}

EOpeningState PgGround::InstanceItemGenerate(PgGenPoint & GenPoint, int & ItemNo, BM::GUID & ItemGuid, CUnit * pCaller)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	EOpeningState eState = E_OPENING_NONE;
	if( S_OK == GenPoint.PopMonster( ItemNo ) )
	{
		PgBase_Item InstanceItem;
		if( SUCCEEDED(::CreateSItem(ItemNo, 1, GIOT_FIELD, InstanceItem)) )
		{
			POINT3 GenPos = GenPoint.Info().pt3Pos;

			VEC_GUID OwnerVec;
			PgLogCont LogCont;
			BM::GUID InsertBoxGuid;
			InsertItemBox( GenPoint.Info(), OwnerVec, NULL, InstanceItem, 0i64, LogCont, InsertBoxGuid );
			GenPoint.AddChild( InsertBoxGuid );
			return E_OPENING_READY;
		}
	}
	return E_OPENING_NONE;
}

void PgGround::RemoveInstanceItem(TBL_DEF_MAP_REGEN_POINT const & GenInfo, BM::GUID const & ItemGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	int const Team = GenInfo.iPointGroup % 10;
	ContGenPoint_InstanceItem * pContItem = NULL;
	switch( Team )
	{
	case TEAM_NONE:
		{
			pContItem = &m_ContGenPoint_InstanceItem_AnyTeam;
		}break;
	case TEAM_RED:
		{
			pContItem = &m_ContGenPoint_InstanceItem_RedTeam;
		}break;
	case TEAM_BLUE:
		{
			pContItem = &m_ContGenPoint_InstanceItem_BlueTeam;
		}break;
	}

	if(NULL == pContItem)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"MapNo["<<GenInfo.iMapNo<<L"BagControlNo["<<GenInfo.iBagControlNo<<L"] PointGroup["<<GenInfo.iPointGroup<<L"] Team["<<Team<<L"]");
		return;
	}

	ContGenPoint_InstanceItem::iterator gen_point_itor = (*pContItem).find(GenInfo.kPosGuid);
	if( gen_point_itor != (*pContItem).end() )
	{
		PgGenPoint & GenPoint = gen_point_itor->second;
		GenPoint.RemoveChild(ItemGuid);
	}
}

void PgGround::ReserveDeleteInsItem(BM::GUID const & CharGuid, TBL_DEF_MAP_REGEN_POINT const & GenInfo, BM::GUID const & ItemGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	SDeleteReserveInsItemInfo DelInfo;
	DelInfo.GenInfo = GenInfo;
	DelInfo.DelItemGuid = ItemGuid;

	m_ContDeleteReserveInsItem.insert( std::make_pair(CharGuid, DelInfo) );
}

void PgGround::RemoveReserveDeleteInsItem(BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_DELETERESERVERINSITEM::iterator iter = m_ContDeleteReserveInsItem.find(CharGuid);
	if( iter != m_ContDeleteReserveInsItem.end() )
	{
		CONT_DELETERESERVERINSITEM::mapped_type const & Element = (*iter).second;

		RemoveInstanceItem(Element.GenInfo, Element.DelItemGuid);

		m_ContDeleteReserveInsItem.erase(iter);
	}
}

void PgGround::UpdateDeleInsItemOwner(BM::GUID const & Looter, BM::GUID const & Droper)
{	// ������ �������� ȹ���ߴ� �÷��̾��� GUID�� ���� �������� ȹ���ϴ� �÷��̾��� GUID�� ��ü�Ѵ�.
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_DELETERESERVERINSITEM::iterator iter = m_ContDeleteReserveInsItem.find(Droper);
	if( iter != m_ContDeleteReserveInsItem.end() )
	{
		CONT_DELETERESERVERINSITEM::mapped_type const & Element = (*iter).second;

		m_ContDeleteReserveInsItem.erase(iter);

		m_ContDeleteReserveInsItem.insert( std::make_pair(Looter, Element) );
	}
}