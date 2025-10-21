#include "stdafx.h"
#include "BM/PgFilterString.h"
#include "Lohengrin/GameTime.h"
#include "Variant/Global.h"
#include "Variant/PgPet.h"
#include "PgGround.h"
#include "SkillEffectAbilSystem/PgSkillAbilHandleManager.h"
#include "PgAction.h"
#include "PgAction_Pet.h"

PgActPet_TickState::PgActPet_TickState( UNIT_PTR_ARRAY &kDelUnitArray, PgGround * const pkGnd )
:	m_kDelUnitArray(kDelUnitArray)
,	m_i64CurSecTime( g_kEventView.GetLocalSecTime(CGameTime::SECOND) )
,	m_pkGround(pkGnd)
{
}

bool PgActPet_TickState::DoAction( PgPet * pkPet )
{
	if ( pkPet )
	{
		// 1. Pet의 기간 만료 체크해서 펫 없애기
//		static SAbil const kAbil[4] = { SAbil(AT_HUNGER,-100), SAbil(AT_HUNGER,-200), SAbil(AT_HEALTH,-100), SAbil(AT_HEALTH,-200), SAbil(AT_MENTAL,-100), SAbil(AT_MENTAL,-200) };
		
		__int64 const i64PetDieTime = pkPet->GetPeriodEndSecTime();
		if ( m_i64CurSecTime >= i64PetDieTime )
		{
			m_kDelUnitArray.Add( pkPet );
		}
		else if ( EPET_TYPE_2 == pkPet->GetPetType() || EPET_TYPE_3 == pkPet->GetPetType() )
		{
			// 성장 펫은 상태를 변경한다.
			BYTE const byChkType = pkPet->UpdateNextConditionCheckSecTime( m_i64CurSecTime );
			if ( byChkType )
			{
				if ( PET_STATE_CHK_EXP & byChkType )	
				{// 경험치를 줘야 한다...
					SPMOD_AddAbilPet kAddAbilPet( pkPet->GetID(), PgItem_PetInfo::ms_kPetItemEquipPos );

					GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);

					__int64 i64AddPlayerExp = 0i64;

					PgActPet_AddExp kAction( kClassPetDefMgr );
					if ( true == kAction.PopAction( pkPet, kAddAbilPet.kAbilList, i64AddPlayerExp ) )
					{
						if ( 0i64 < i64AddPlayerExp )
						{
							PgPlayer * pkOwnerPlayer = m_pkGround->GetUser( pkPet->Caller() );
							if ( pkOwnerPlayer )
							{
								PgAction_AddExp kAddPlayerExpAction( m_pkGround->GroundKey(), i64AddPlayerExp, AEC_PetExp, m_pkGround, 0 );
								kAddPlayerExpAction.DoAction( pkOwnerPlayer, NULL );
							}
						}
					}

					if ( kAddAbilPet.kAbilList.size() )
					{
						CONT_PLAYER_MODIFY_ORDER kOrder;
						SPMO kIMO( IMET_PET, pkPet->Caller(), kAddAbilPet );
						kOrder.push_back( kIMO );

						PgAction_ReqModifyItem kItemModifyAction( CIE_SetAbilPet, m_pkGround->GroundKey(), kOrder, BM::Stream(), true );//죽음무시
						kItemModifyAction.DoAction( pkPet->Caller() );
					}	
				}
			}
		}
	
		return true;
	}
	return false;
}

PgActPet_AddExp::PgActPet_AddExp( PgClassPetDefMgr const & kPetDefMgr )
:	m_kPetDefMgr(kPetDefMgr)
{}

__int64 PgActPet_AddExp::GetExp( PgPet * pkPet )const
{
	if ( pkPet )
	{
		SClassKey const kNowClassKey = pkPet->GetClassKey();

		short const nLastLv = m_kPetDefMgr.GetLastLv( kNowClassKey.iClass );
		if ( nLastLv > kNowClassKey.nLv )
		{
			PgClassPetDef kPetDef;
			if ( true == m_kPetDefMgr.GetDef( kNowClassKey, &kPetDef ) )
			{
				__int64 const i64NowExp = pkPet->GetAbil64( AT_EXPERIENCE );
				__int64 i64AddExp = static_cast<__int64>(kPetDef.GetAbil( AT_EXPERIENCE_TIME ));
				if ( 0i64 < i64AddExp )
				{
					__int64 const i64AddItemExp = i64AddExp * static_cast<__int64>(pkPet->GetAbil(AT_INVEN_PET_ADD_TIME_EXP_RATE)) / ABILITY_RATE_VALUE64;
					return i64AddExp + i64AddItemExp;
				}
			}
		}
	}
	return 0i64;
}

bool PgActPet_AddExp::PopAction( PgPet * pkPet, SPMOD_AddAbilPet::CONT_ABILLIST &rkOutAbilList, __int64 &ri64OutPlayerExp )const
{	
	__int64 i64AddExp = GetExp( pkPet );
	if ( 0i64 < i64AddExp )
	{
		int const iAddPlayerExpRate = pkPet->GetAbil(AT_GIVE_TO_CALLER_ADD_EXP_RATE);
		ri64OutPlayerExp = ( i64AddExp * static_cast<__int64>(iAddPlayerExpRate) / ABILITY_RATE_VALUE64 );

		rkOutAbilList.push_back( SPMOD_SAbil( AT_EXPERIENCE, i64AddExp ) );
		return true;
	}
	return false;
}

bool PgActPet_AddExp::PopAbilList( PgPet * pkPet, __int64 i64AddExp, SPMOD_AddAbilPet::CONT_ABILLIST &rkOutAbilList )const
{
	if ( true == GetCheckAddExp( pkPet, i64AddExp ) )
	{
		rkOutAbilList.push_back( SPMOD_SAbil( AT_EXPERIENCE, i64AddExp ) );
		return true;
	}
	return false;
}

bool PgActPet_AddExp::GetCheckAddExp( PgPet * pkPet, __int64& i64AddExp )const
{
	SClassKey const kNowClassKey = pkPet->GetClassKey();
	__int64 const i64MaxExp = m_kPetDefMgr.GetMaxExp( kNowClassKey.iClass );
	__int64 const i64CurrentExp = pkPet->GetAbil64( AT_EXPERIENCE );
	__int64 i64ExpGap = ( i64MaxExp - i64CurrentExp );
	if ( 0i64 < i64ExpGap )
	{
		i64AddExp = std::min( i64AddExp, i64ExpGap );
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//		PgActPet_Rename
//////////////////////////////////////////////////////////////////////////////
PgActPet_Rename::PgActPet_Rename( PgGround * const pkGround, SItemPos const &kPetItemPos )
:	m_kPetItemPos(kPetItemPos)
,	m_pkGround(pkGround)
{
}

int PgActPet_Rename::MakeUseOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, CUnit *pkUser )const
{
	__int64 const i64SpendMoney = PgPet::ms_i64RenamePetMoney;

	if ( i64SpendMoney > pkUser->GetAbil64( AT_MONEY ) )
	{
		// 돈이 모자랍니다.
		return 403078;
	}

	SPMO kIMO(IMET_ADD_MONEY, pkUser->GetID(), SPMOD_Add_Money(-i64SpendMoney));
	rkOrder.push_back(kIMO);
	return 0;
}

bool PgActPet_Rename::DoAction( CUnit* pkUser, std::wstring &wstrPetName )
{
	if ( pkUser && m_pkGround )
	{
		PgInventory *pkInv = pkUser->GetInven();

		PgBase_Item kPetItem;
		if ( S_OK == pkInv->GetItem( m_kPetItemPos, kPetItem ) )
		{
			if ( UIT_STATE_PET == kPetItem.State() )
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;

				int const iError = this->MakeUseOrder( kOrder, pkUser );

				if ( iError )
				{
					pkUser->SendWarnMessage( iError );
				}
				else
				{
					// 이름의 제한길이 체크
					if ( true == CheckBalanceWStringLen( wstrPetName, PgItem_PetInfo::MAX_PET_NAMELEN / 2 ) )
					{
						if (	!g_kUnicodeFilter.IsCorrect(UFFC_CHARACTER_NAME, wstrPetName)
							||	true == g_kFilterString.Filter( wstrPetName, false, FST_ALL ) )
						{
							// 사용 할 수 없는 단어가 들어가 있습니다.
							pkUser->SendWarnMessage( 700144 );
						}
						else
						{
							SPMO kIMO( IMET_PET, pkUser->GetID(), SPMOD_RenamePet( m_kPetItemPos, wstrPetName ) );
							kOrder.push_back( kIMO );

							PgAction_ReqModifyItem kItemModifyAction( CIE_RenamePet, m_pkGround->GroundKey(), kOrder );
							return kItemModifyAction.DoAction( pkUser, NULL );
						}
					}
					else
					{// 이름의 길이가 너무 깁니다.
						pkUser->SendWarnMessage( 7501 );
					}
				}
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgActPet_Rename_ForItem
//////////////////////////////////////////////////////////////////////////////
PgActPet_Rename_ForItem::PgActPet_Rename_ForItem( PgGround * const pkGround, SItemPos const &kPetItemPos, SItemPos const &kRenameItemPos )
:	PgActPet_Rename( pkGround, kPetItemPos )
,	m_kRenameItemPos(kRenameItemPos)
{
}

int PgActPet_Rename_ForItem::MakeUseOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, CUnit *pkUser )const
{
	PgBase_Item kRenameItem;
	if ( S_OK == pkUser->GetInven()->GetItem( m_kRenameItemPos, kRenameItem ) )
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const* pkItemDef = kItemDefMgr.GetDef(kRenameItem.ItemNo());
		if( pkItemDef )
		{
			if(		pkItemDef->IsType(ITEM_TYPE_CONSUME)
				&&	pkItemDef->PrimaryInvType() == m_kRenameItemPos.x 
			)//본인 메인 인벤 자리가 아니면 사용 불가. -> 즉. 창고 아이템은 안된다는 얘기.
			{
				int const iCustomType = pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
				if ( UICT_PET_RENAME == iCustomType )
				{
					SPMOD_Modify_Count kDelData( kRenameItem, m_kRenameItemPos, -1);//1씩 감소.
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkUser->GetID(), kDelData);
					rkOrder.push_back(kIMO);
					return 0;
				}
			}
		}
	}

	return 328;
}

//////////////////////////////////////////////////////////////////////////////
//		PgActPet_ColorChange
//////////////////////////////////////////////////////////////////////////////
PgActPet_ColorChange::PgActPet_ColorChange( PgGround * const pkGround, SItemPos const &kPetItemPos )
:	m_kPetItemPos(kPetItemPos)
,	m_pkGround(pkGround)
{
}

int PgActPet_ColorChange::MakeUseOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, CUnit *pkUser )const
{
	__int64 const i64SpendMoney = PgPet::ms_i64ChangeColorPetMoney;

	if ( i64SpendMoney > pkUser->GetAbil64( AT_MONEY ) )
	{
		// 돈이 모자랍니다.
		return 403078;
	}

	SPMO kIMO(IMET_ADD_MONEY, pkUser->GetID(), SPMOD_Add_Money(-i64SpendMoney));
	rkOrder.push_back(kIMO);
	return 0;
}


bool PgActPet_ColorChange::DoAction( CUnit* pkUser, char cColorIndex )
{
	if ( pkUser && m_pkGround )
	{
		PgInventory *pkInv = pkUser->GetInven();

		PgBase_Item kPetItem;
		if ( S_OK == pkInv->GetItem( m_kPetItemPos, kPetItem ) )
		{
			if ( UIT_STATE_PET == kPetItem.State() )
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;

				int const iError = this->MakeUseOrder( kOrder, pkUser );

				if ( iError )
				{
					pkUser->SendWarnMessage( iError );
				}
				else
				{
					// Color의 유효범위 체크
					if ( 0 <= cColorIndex )
					{
						SPMOD_AddAbilPet kAddAbilPet( kPetItem.Guid(), m_kPetItemPos );
						kAddAbilPet.kAbilList.push_back( SPMOD_SAbil(AT_COLOR_INDEX, static_cast<__int64>(cColorIndex)) );

						SPMO kIMO( IMET_PET, pkUser->GetID(), kAddAbilPet );
						kOrder.push_back( kIMO );

						PgAction_ReqModifyItem kItemModifyAction( CIE_SetAbilPet, m_pkGround->GroundKey(), kOrder );
						return kItemModifyAction.DoAction( pkUser, NULL );
					}
				}
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgActPet_ColorChange
//////////////////////////////////////////////////////////////////////////////
PgActPet_ColorChange_ForItem::PgActPet_ColorChange_ForItem( PgGround * const pkGround, SItemPos const &kPetItemPos, SItemPos const &kColorChangeItemPos )
:	PgActPet_ColorChange( pkGround, kPetItemPos )
,	m_kColorChangeItemPos(kColorChangeItemPos)
{
}

int PgActPet_ColorChange_ForItem::MakeUseOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, CUnit *pkUser )const
{
	PgBase_Item kRenameItem;
	if ( S_OK == pkUser->GetInven()->GetItem( m_kColorChangeItemPos, kRenameItem ) )
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const* pkItemDef = kItemDefMgr.GetDef(kRenameItem.ItemNo());
		if( pkItemDef )
		{
			if(		pkItemDef->IsType(ITEM_TYPE_CONSUME)
				&&	pkItemDef->PrimaryInvType() == m_kColorChangeItemPos.x 
				)//본인 메인 인벤 자리가 아니면 사용 불가. -> 즉. 창고 아이템은 안된다는 얘기.
			{
				int const iCustomType = pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
				if ( UICT_PET_COLORCHANGE == iCustomType )
				{
					SPMOD_Modify_Count kDelData( kRenameItem, m_kColorChangeItemPos, -1);//1씩 감소.
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkUser->GetID(), kDelData);
					rkOrder.push_back(kIMO);
					return 0;
				}
			}
		}
	}

	return 328;
}

//////////////////////////////////////////////////////////////////////////////
//		PgActPet_MakeClassChangeOrder
//////////////////////////////////////////////////////////////////////////////
PgActPet_MakeClassChangeOrder::PgActPet_MakeClassChangeOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, bool const bGMCommand, PgBase_Item kPetItem, SItemPos kItemPos )
:	m_rkOrder(rkOrder)
,	m_bGMCommand(bGMCommand)
,	m_kPetItem(kPetItem)
,	m_kItemPos(kItemPos)
{
}

bool PgActPet_MakeClassChangeOrder::DoAction( CUnit *pkPlayer, int const iGrade, short nLv )
{
	if ( 4 < iGrade )
	{
		return false;
	}

	if(m_kItemPos==SItemPos::NullData())
	{
		m_kItemPos = PgItem_PetInfo::ms_kPetItemEquipPos;
	}
	if(m_kPetItem.IsEmpty())
	{ //장착되지 않은 펫 전직이 아닌가? (==장착된 펫 전직인가?)
		if(S_OK != pkPlayer->GetInven()->GetItem( m_kItemPos, m_kPetItem ))
		{ //장착된 펫을 얻어온다.
			return false;
		}
	}

	PgItem_PetInfo *pkPetExtInfo = NULL;
	if ( true == m_kPetItem.GetExtInfo( pkPetExtInfo ) )
	{
		SClassKey const kOrgClassKey = pkPetExtInfo->ClassKey();

		GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
		PgClassPetDef kPetDef;
		if ( !kClassPetDefMgr.GetDef( kOrgClassKey, &kPetDef ) )
		{
			return false;
		}

		if ( EPET_TYPE_2 != kPetDef.GetPetType() && EPET_TYPE_3 != kPetDef.GetPetType() )
		{
			return false;
		}

		if ( !m_bGMCommand )
		{
			GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
			if ( !kClassPetDefMgr.IsChangeClass( kOrgClassKey ) )
			{
				pkPlayer->SendWarnMessage( 347 );
				return false;
			}

			int const iNextGrade = PgClassPetDef::GetClassGrade( kOrgClassKey.iClass + 1 );
			if ( iNextGrade != iGrade )
			{
				pkPlayer->SendWarnMessage( 348 );
				return false;
			}
		}

		SClassKey const kNewClassKey( PgClassPetDef::GetBaseClass( kOrgClassKey.iClass ) + iGrade, ( nLv ? nLv : kOrgClassKey.nLv ) );

		if ( kOrgClassKey != kNewClassKey )
		{
			GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
			PgClassPetDef kPetDef;

			if ( true == kClassPetDefMgr.GetDef( kNewClassKey, &kPetDef ) )
			{
				SPMOD_AddAbilPet kAddAbilPet( m_kPetItem.Guid(), m_kItemPos );

				if ( kNewClassKey.nLv != kOrgClassKey.nLv )
				{// 레벨을 먼저 넣어주어야 클라이언트에서 처리가 쉽게 된다.
					kAddAbilPet.kAbilList.push_back( SPMOD_SAbil(AT_LEVEL, static_cast<__int64>(kNewClassKey.nLv)) );
				}

				if ( kNewClassKey.iClass != kOrgClassKey.iClass )
				{
					kAddAbilPet.kAbilList.push_back( SPMOD_SAbil(AT_CLASS, static_cast<__int64>(kNewClassKey.iClass) ) );
				}

				__int64 const i64CurExp = pkPetExtInfo->Exp();
				__int64 const i64DefExp = kPetDef.GetAbil64( AT_EXPERIENCE );
				if ( i64CurExp > i64DefExp )
				{
					SClassKey kLevelUpClassKey;
					if ( true == kClassPetDefMgr.IsLvUp( kNewClassKey, i64CurExp, kLevelUpClassKey ) )
					{
						// 강제로 경험치 조정
						kAddAbilPet.kAbilList.push_back( SPMOD_SAbil(AT_EXPERIENCE, i64DefExp-i64CurExp) );
					}
				}
				else if ( i64CurExp < i64DefExp )
				{
					// 강제로 경험치 조정
					kAddAbilPet.kAbilList.push_back( SPMOD_SAbil(AT_EXPERIENCE, i64DefExp-i64CurExp) );
				}

				SPMO kIMO( IMET_PET, pkPlayer->GetID(), kAddAbilPet );
				m_rkOrder.push_back( kIMO );

				return true;
			}

			pkPlayer->SendWarnMessage( 348 );
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
//		PgActPet_RefreshPassiveSkill
//////////////////////////////////////////////////////////////////////////////
PgActPet_RefreshPassiveSkill::PgActPet_RefreshPassiveSkill( PgGround const *pkGround )
:	m_pkGround(pkGround)
{
}

bool PgActPet_RefreshPassiveSkill::DoAction_ReleaseTarget( PgPet * pkPet, PgPlayer *pkCallerPlayer, PgGround *pkGround )
{
	if ( pkPet && pkCallerPlayer )
	{
		int iSkillNo = 0;
		size_t iIndex = 0;
		PgMySkill* pkMySkill = pkPet->GetMySkill();

		SActArg kArg;
		PgGroundUtil::SetActArgGround(kArg, pkGround);

		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		while( (iSkillNo = pkMySkill->GetSkillNo(EST_PASSIVE, iIndex++)) > 0 )
		{
			CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
			if(pkSkillDef)
			{
				// 펫은 없어지니까 굳이 낭비 하지 말자.
				if ( ESTARGET_SELF & pkSkillDef->GetTargetType() )
				{
					CEffect const* pkEffect = pkPet->GetEffect( iSkillNo, true );
					if ( pkEffect )
					{
						// 실제 레벨에 해당되는 패시브 스킬을 삭제
						g_kSkillAbilHandleMgr.SkillEnd( pkPet, pkSkillDef, &kArg ); 
					}
				}

				if ( ESTARGET_CASTER & pkSkillDef->GetTargetType() )
				{
					CEffect const* pkEffect = pkCallerPlayer->GetEffect( iSkillNo, true );
					if ( pkEffect )
					{
						// 실제 레벨에 해당되는 패시브 스킬을 삭제
						g_kSkillAbilHandleMgr.SkillEnd( pkCallerPlayer, pkSkillDef, &kArg ); 
					}
				}
			}
			else
			{
				// 해당되는 스킬Def를 찾을 수 없다.~
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Passive Skill Cannot End. SkillDef[") << iSkillNo << _T("] is not Find")); // 디버그용
			}
		}
		return true;
	}
	return false;
}

bool PgActPet_RefreshPassiveSkill::DoAction( PgPet * pkPet )
{
	if ( pkPet )
	{
		int iSkillNo = 0;
		size_t iIndex = 0;
		PgMySkill* pkMySkill = pkPet->GetMySkill();

		SActArg kArg;
		PgGroundUtil::SetActArgGround(kArg, m_pkGround);

		PgPlayer * pkCallerPlayer = m_pkGround->GetUser( pkPet->Caller() );

		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		while( (iSkillNo = pkMySkill->GetSkillNo(EST_PASSIVE, iIndex++)) > 0 )
		{
			CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
			if(pkSkillDef)
			{
				if ( ESTARGET_SELF & pkSkillDef->GetTargetType() )
				{
					CEffect const* pkEffect = pkPet->GetEffect( iSkillNo, true );
					if ( !pkEffect )
					{
						g_kSkillAbilHandleMgr.SkillPassive( pkPet, iSkillNo, &kArg );
					}
				}

				if ( ESTARGET_CASTER & pkSkillDef->GetTargetType() )
				{// Caller한테도 걸어주어야 한다.
					if ( pkCallerPlayer )
					{
						CEffect const* pkEffect = pkCallerPlayer->GetEffect( iSkillNo, true );
						if ( !pkEffect )
						{
							g_kSkillAbilHandleMgr.SkillPassive( pkCallerPlayer, iSkillNo, &kArg );
						}
					}
				}
			}
			else
			{
				// 해당되는 스킬Def를 찾을 수 없다.~
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Passive Skill Cannot End. SkillDef[") << iSkillNo << _T("] is not Find")); // 디버그용
			}
		}
		return true;
	}
	return false;
}
