#include "stdafx.h"
#include "PgControlDefMgr.h"
#include "tabledatamanager.h"
#include "PgClassPetDefMgr.h"
#include "PgPet.h"
#include "PetSkillDataConverter.h"
#include "Effect.h"

bool PgPetSkillDataSet::AddEffect( __int64 const i64NowMinTime, __int64 const i64SkillTime, size_t const iIndex, int const iSkillNo, PgPet *pkPet )
{
	if ( iSkillNo )
	{
		__int64 const i64TimeGap = (i64SkillTime - i64NowMinTime);
		if ( 0i64 < i64TimeGap )
		{
//			if ( ms_i64MakeEffectTime > i64TimeGap )
//			{
				SEffectCreateInfo kCreate;
				kCreate.eType = EFFECT_TYPE_ACTIVATE_SKILL;
				kCreate.iEffectNum = (EFFECTNO_PETSKILL_ACTIVATE_BASE + iIndex);
				kCreate.iValue = iSkillNo;
				kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
				CGameTime::SecTime2DBTimeEx( i64SkillTime, kCreate.kWorldExpireTime, CGameTime::MINUTE );// 분으로 저장되어있다.
				pkPet->AddEffect( kCreate );
//			}
			return true;
		}
	}
	return false;
}

bool PgPetSkillDataSet::InitSkill( CONT_DEFCLASS_PET_SKILL::mapped_type::value_type const &kDefPetSkill, PgPet *pkPet, PgItem_PetInfo * pkPetItemInfo )
{
	__int64 const i64NowMinTime = g_kEventView.GetLocalSecTime(CGameTime::MINUTE);

	PgItem_PetInfo::SStateValue kSkillStateValue;
	BM::DBTIMESTAMP_EX kTempTime;

	PgMySkill *pkPetSkill = pkPet->GetMySkill();

	size_t i = 0;
	if ( pkPetItemInfo )
	{
		for ( i = 0; i< PgItem_PetInfo::MAX_SKILL_SIZE ; ++i )
		{
			if ( true == pkPetItemInfo->GetSkill( i, kSkillStateValue ) )
			{
				size_t const iSkillIndex = (static_cast<size_t>(kSkillStateValue.Value()) - 1);
				if ( iSkillIndex < MAX_PET_SKILLCOUNT )
				{
					__int64 const i64StateTime = static_cast<__int64>(kSkillStateValue.Time()) + PgItem_PetInfo::SKILL_BASE_TIME;
					AddEffect( i64NowMinTime, i64StateTime, iSkillIndex, kDefPetSkill.iSkillNo[iSkillIndex], pkPet );
					pkPetSkill->Add( kDefPetSkill.iSkillNo[iSkillIndex], static_cast<BYTE>(iSkillIndex) );
				}
			}
		}
	}

	int iCheckFlag = 1;
	for ( i=0 ; i<MAX_PET_SKILLCOUNT; ++i )
	{
		if ( kDefPetSkill.iSkillNo[i] )
		{
			if ( 0 == (kDefPetSkill.iSkillUseTimeBit & iCheckFlag) )
			{
				pkPetSkill->Add( kDefPetSkill.iSkillNo[i], static_cast<BYTE>(i) );
			}
			else if ( !pkPetItemInfo )
			{
				int const iEffectNo = (EFFECTNO_PETSKILL_ACTIVATE_BASE + i);
				if ( pkPet->FindEffect( iEffectNo ) )
				{
					pkPetSkill->Add( kDefPetSkill.iSkillNo[i], static_cast<BYTE>(i) );
				}
			}
		}

		iCheckFlag <<= 1;
	}
	return true;
}

int PgPetSkillDataSet::SetSkill( PgPet *pkPet, PgItem_PetInfo::SStateValue const &kSkillStateValue )
{
	GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
	PgClassPetDef kPetDef;
	SClassKey const kPetClassKey(pkPet->GetClassKey());
	if ( !kClassPetDefMgr.GetDef( kPetClassKey, &kPetDef ) )
	{
		return 0;
	}

	size_t const iSkillIndex = (static_cast<size_t>(kSkillStateValue.Value()) - 1);
	if ( iSkillIndex >= MAX_PET_SKILLCOUNT )
	{
		return 0;
	}

	CONT_DEFCLASS_PET_SKILL const *pkDefPetSkill = NULL;
	g_kTblDataMgr.GetContDef(pkDefPetSkill);

	CONT_DEFCLASS_PET_SKILL::const_iterator skill_itr = pkDefPetSkill->find( kPetDef.GetSkillDefID() );
	if ( skill_itr != pkDefPetSkill->end() )
	{
		CONT_DEFCLASS_PET_SKILL::mapped_type const &kContElement = skill_itr->second;
		size_t const index = kContElement.find( PgDefClassPetSkillFinder(kPetClassKey.nLv) );
		if ( BM::PgApproximate::npos != index )
		{
			CONT_DEFCLASS_PET_SKILL::mapped_type::value_type const &kElement = kContElement.at(index);
			
			__int64 const i64NowMinTime = g_kEventView.GetLocalSecTime(CGameTime::MINUTE);
			__int64 const i64StateTime = static_cast<__int64>(kSkillStateValue.Time()) + PgItem_PetInfo::SKILL_BASE_TIME;
			if ( true == AddEffect( i64NowMinTime, i64StateTime, iSkillIndex, kElement.iSkillNo[iSkillIndex], pkPet ) )
			{
				pkPet->GetMySkill()->Add( kElement.iSkillNo[iSkillIndex], static_cast<BYTE>(iSkillIndex) );
				return kElement.iSkillNo[iSkillIndex];
			}
			else
			{
				pkPet->GetMySkill()->Delete( kElement.iSkillNo[iSkillIndex] );
			}
		}
	}
	return 0;
}
