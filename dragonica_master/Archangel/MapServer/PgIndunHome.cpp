#include "stdafx.h"
#include "Variant/PgMission.h"
#include "Variant/PgQuestInfo.h"
#include "PgIndunHome.h"
#include "PgActionAchievement.h"
#include "PgAction.h"
#include "PgActionQuest.h"

__int64 const HOME_DELETE_TIME = 180;

PgIndunHome::PgIndunHome()
{
	m_i64DeleteTime = g_kEventView.GetLocalSecTime() + HOME_DELETE_TIME;
}

PgIndunHome::~PgIndunHome()
{
}

bool PgIndunHome::IsDeleteTime()const
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if(m_i64DeleteTime > g_kEventView.GetLocalSecTime())
	{
		return false;
	}

	return ( 0 == PgObjectMgr::GetUnitCount(UT_PLAYER) );
}

void PgIndunHome::SendMapLoadComplete( PgPlayer *pkUser )
{
	PgGround::SendMapLoadComplete( pkUser );

	BM::CAutoMutex Lock( m_kRscMutex );

	CONT_OBJECT_MGR_UNIT::iterator unit_itr;
	PgObjectMgr::GetFirstUnit(UT_MYHOME, unit_itr);

	PgMyHome * pkHome = dynamic_cast<PgMyHome *>(PgObjectMgr::GetNextUnit(UT_MYHOME, unit_itr));
	if(NULL == pkHome)	// 마이홈 유닛은 홈 안에 단 하나뿐이다. 없으면 버그다 ㅡㅡ;; 물론 많아도 버그...
	{
		return;
	}

	if(pkHome->OwnerGuid() != pkUser->GetID())	// 집주인이 들어온게 아니다.
	{// 주인의 방문회수 업적 카운트 증가
		PgPlayer* pkHomeOwner = dynamic_cast<PgPlayer*>( GetUnit( pkHome->OwnerGuid() ) );
		if( pkHomeOwner )
		{
			PgAddAchievementValue kMA( AT_ACHIEVEMENT_MYHOME_HITCOUNT, 1, GroundKey() );
			kMA.DoAction( pkHomeOwner, NULL );
		}

		return ;
	}

	// 마이홈과 연결된 퀘스트 확인
	PgAction_QuestMyHome kQuestAction(GroundKey(), pkHome->GetAbil(AT_MYHOME_TOTALHIT));
	kQuestAction.DoAction(pkUser, NULL);
}
