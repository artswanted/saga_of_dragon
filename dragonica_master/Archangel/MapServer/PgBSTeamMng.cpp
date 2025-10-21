#include "stdafx.h"
#include "Variant/PgBattleSquare.h"
#include "PgBSTeamMng.h"



//
PgBSTeamMng::PgBSTeamMng()
	: m_kTeamBlue(BST_BLUE), m_kTeamRed(BST_RED), m_kContCharTeam()
{
}
PgBSTeamMng::~PgBSTeamMng()
{
}

bool PgBSTeamMng::IsReseve(BM::GUID const& rkCharGuid) const
{
	return m_kContCharTeam.end() != m_kContCharTeam.find(rkCharGuid);
}
EBS_JOIN_RETURN PgBSTeamMng::Reserve(BM::GUID const& rkCharGuid, int const iMaxMemberCount)
{
	if( IsReseve(rkCharGuid) )
	{
		return BSJR_SUCCESS;
	}

	if( static_cast< size_t >(iMaxMemberCount) <= m_kContCharTeam.size() )
	{
		return BSJR_MAX;
	}

	m_kContCharTeam.insert( std::make_pair(rkCharGuid, BST_NONE) );
	return BSJR_SUCCESS;
}
EBS_JOIN_RETURN PgBSTeamMng::Join(CUnit* pkUnit, int const iMaxMemberCount, EBattleSquareTeam const eTeam)
{
	if( !pkUnit )
	{
		return BSJR_MAX;
	}

	if( pkUnit->IsInUnitType(UT_PLAYER)
	&&	0 == GetTeam(pkUnit->GetID()) )
	{
		EBS_JOIN_RETURN const eReserveRet = Reserve(pkUnit->GetID(), iMaxMemberCount);
		if( BSJR_SUCCESS != eReserveRet )
		{
			return eReserveRet;
		}

		PgBSTeam* pkSelectTeam = NULL;
		PgBSTeam* pkEnemyTeam = NULL;
		switch( eTeam )
		{
		case BST_BLUE:
			{
				pkSelectTeam = &m_kTeamBlue;
				pkEnemyTeam = &m_kTeamRed;
			}break;
		case BST_RED:
			{
				pkSelectTeam = &m_kTeamRed;
				pkEnemyTeam = &m_kTeamBlue;
			}break;
		default:
			{
				return BSJR_MAX;
			}break;
		}
		// 팀선택해서 참가 등록
		// 팀원의 차이가 많이 나면 대기 리스트 등록
		// 최대 대기 목록은 10명 까지

		// 사전 오픈 단계는 차이가 1명 까지 OK
		int const iMaxTeamMemberCount = (iMaxMemberCount / 2);
		if( pkSelectTeam->GetMemberCount() <= pkEnemyTeam->GetMemberCount() )
		{
			// 선택한 팀 인원수가 같거나 상대편 보다 적으면 바로 참가
			// 선택한 팀 인원수가 상대편 보다 적을 때 +2 명까지 바로 참가 가능
			pkSelectTeam->AddMember(pkUnit);

			CONT_BS_CHAR_TEAM::iterator find_iter = m_kContCharTeam.find( pkUnit->GetID() );
			if( m_kContCharTeam.end() != find_iter )
			{
				(*find_iter).second = eTeam;
			}
			pkEnemyTeam->DelWaiter(pkUnit->GetID()); // 상대팀 대기자에 있었을 지도 모른다
		}
		else
		{
			// 이외엔 전부 대기
			if( iMaxTeamMemberCount > pkSelectTeam->GetMemberCount() + pkSelectTeam->GetWaiterCount() )
			{
				// 팀에 가능한 인원수 내면 대기 가능
				pkSelectTeam->AddWaiter(pkUnit);
				return BSJR_BALANCE;
			}
			else
			{
				return BSJR_MAX; // 팀 선택 불가능
			}
		}
	}
	else
	{
		return BSJR_ALREAD_JOIN;
	}
	return BSJR_SUCCESS;
}
void PgBSTeamMng::Leave(CUnit* pkUnit)
{
	if( !pkUnit )
	{
		return;
	}

	bool bSend = false;
	if( pkUnit->IsInUnitType(UT_PLAYER) )
	{
		EBattleSquareTeam const eTeam = GetTeamType(pkUnit->GetID());
		switch( eTeam )
		{
		case BST_BLUE:
			{
				m_kTeamBlue.DelMember(pkUnit->GetID());
			}break;
		case BST_RED:
			{
				m_kTeamRed.DelMember(pkUnit->GetID());
			}break;
		default:
			{
			}break;
		}
		m_kTeamRed.DelWaiter(pkUnit->GetID());
		m_kTeamBlue.DelWaiter(pkUnit->GetID());
		m_kContCharTeam.erase(pkUnit->GetID());
		bSend = true;
	}

	pkUnit->SetAbil(AT_TEAM, 0, bSend, true);
}
void PgBSTeamMng::ProcessWaiter(int const iMaxMemberCount, ContGuidSet& rkOut)
{
	// 이미 가입하거나, 탈퇴한 뒤
	// 인원수 차이에 따라 대기자 처리 (항상 동등 하게 되도록)
	int const iMaxTeamMemberCount = (iMaxMemberCount / 2);
	int const iMaxBalance = 1; // 1명 이상 차이 나게 참가 가능
	if( m_kTeamRed.GetMemberCount() < m_kTeamBlue.GetMemberCount() )
	{
		int const iDiff = m_kTeamBlue.GetMemberCount() - m_kTeamRed.GetMemberCount();
		ProcessWaiter(m_kTeamRed, iMaxBalance, rkOut); // 항상 대기자 처리
		if( iMaxBalance > iDiff )
		{
			ProcessWaiter(m_kTeamBlue, iMaxBalance - iDiff, rkOut);
		}
	}
	else if( m_kTeamRed.GetMemberCount() > m_kTeamBlue.GetMemberCount() )
	{
		int const iDiff = m_kTeamRed.GetMemberCount() - m_kTeamBlue.GetMemberCount();
		if( iMaxBalance > iDiff )
		{
			ProcessWaiter(m_kTeamRed, iMaxBalance - iDiff, rkOut);
		}
		ProcessWaiter(m_kTeamBlue, iMaxBalance, rkOut); // 항상 대기자 처리
	}
	else // ==
	{
		ProcessWaiter(m_kTeamRed, iMaxBalance, rkOut);
		ProcessWaiter(m_kTeamBlue, iMaxBalance, rkOut);
	}
}
void PgBSTeamMng::ProcessWaiter(PgBSTeam& rkTeam, int iProcessManCount, ContGuidSet& rkOut)
{
	CONT_BS_TEAM_MEMBER::const_iterator iter = rkTeam.ContWaiter().begin();
	while( 0 < iProcessManCount && rkTeam.ContWaiter().end() != iter )
	{
		CONT_BS_TEAM_MEMBER::value_type const kBSMember((*iter));

		rkTeam.AddMember( kBSMember );
		rkOut.insert( kBSMember.kCharGuid );
		rkTeam.DelWaiter( kBSMember.kCharGuid );

		CONT_BS_CHAR_TEAM::iterator find_iter = m_kContCharTeam.find( kBSMember.kCharGuid );
		if( m_kContCharTeam.end() != find_iter )
		{
			(*find_iter).second = rkTeam.Team();
		}

		--iProcessManCount;
		iter = rkTeam.ContWaiter().begin();
	}
}
void PgBSTeamMng::Clear()
{
	m_kTeamBlue.Clear();
	m_kTeamRed.Clear();
	m_kContCharTeam.clear();
}

EBattleSquareTeam PgBSTeamMng::GetTeamType(BM::GUID const& rkCharGuid) const
{
	CONT_BS_CHAR_TEAM::const_iterator find_iter = m_kContCharTeam.find(rkCharGuid);
	if( m_kContCharTeam.end() != find_iter )
	{
		return (*find_iter).second;
	}
	return BST_NONE;
}
void PgBSTeamMng::SetTeam(CUnit* pkUnit, bool const bIsGame) const
{
	if( NULL == pkUnit )
	{
		return;
	}

	int iNewTeamNo = 0;
	bool bSend = false;
	switch( pkUnit->UnitType() )
	{
	case UT_ENTITY:
	case UT_SUMMONED:
		{
			BM::GUID const& rkCallerGuid = pkUnit->Caller();
			if( BM::GUID::IsNotNull(rkCallerGuid) )
			{
				iNewTeamNo = GetTeamType(rkCallerGuid);
			}
		}break;
	case UT_PLAYER:
		{
			iNewTeamNo = GetTeamType(pkUnit->GetID());
			bSend = true;
		}break;
	case UT_OBJECT:
	case UT_MONSTER:
	case UT_BOSSMONSTER:
	case UT_PET:
	case UT_NPC:
	case UT_GROUNDBOX:
	default:
		{
			// 팀설정 없다
			iNewTeamNo = 0;
		}break;
	}

	if( 0 != iNewTeamNo
	&&	bIsGame )
	{
		pkUnit->SetAbil(AT_TEAM, iNewTeamNo, bSend, true);
	}
}
void PgBSTeamMng::WriteToPacket(BM::Stream& rkPacket) const
{
	m_kTeamRed.WriteToPacket(rkPacket);
	m_kTeamBlue.WriteToPacket(rkPacket);
}
void PgBSTeamMng::WriteToScorePacket(ContGuidSet const& rkContGuid, BM::Stream& rkPacket, bool const bSyncAll) const
{
	m_kTeamRed.WriteToScorePacket(rkContGuid, rkPacket, bSyncAll);
	m_kTeamBlue.WriteToScorePacket(rkContGuid, rkPacket, bSyncAll);
}
bool PgBSTeamMng::GetMember(BM::GUID const& rkGuid, SBSTeamMember& rkOut) const
{
	CONT_BS_CHAR_TEAM::const_iterator team_iter = m_kContCharTeam.find(rkGuid);
	if( m_kContCharTeam.end() == team_iter )
	{
		return false;
	}
	switch( (*team_iter).second )
	{
	case BST_RED:	{ return m_kTeamRed.GetMember(rkGuid, rkOut); }break;
	case BST_BLUE:	{ return m_kTeamBlue.GetMember(rkGuid, rkOut); }break;
	default:
		{
		}break;
	}
	return false;
}
int PgBSTeamMng::GetTeam(BM::GUID const& rkGuid) const
{
	return static_cast< int >(GetTeamType(rkGuid));
}
int PgBSTeamMng::GetTeamPoint(BM::GUID const& rkGuid) const
{
	CONT_BS_CHAR_TEAM::const_iterator team_iter = m_kContCharTeam.find(rkGuid);
	if( m_kContCharTeam.end() == team_iter )
	{
		return 0;
	}
	switch( (*team_iter).second )
	{
	case BST_RED:	{ return m_kTeamRed.TeamPoint(); }break;
	case BST_BLUE:	{ return m_kTeamBlue.TeamPoint(); }break;
	default:
		{
		}break;
	}
	return 0;
}
void PgBSTeamMng::AddScore(BM::GUID const& rkGuid, int const iPoint, int const iKill, int const iDead)
{
	CONT_BS_CHAR_TEAM::const_iterator team_iter = m_kContCharTeam.find(rkGuid);
	if( m_kContCharTeam.end() == team_iter )
	{
		return;
	}
	switch( (*team_iter).second )
	{
	case BST_RED:	{ m_kTeamRed.AddScore(rkGuid, iPoint, iKill, iDead); }break;
	case BST_BLUE:	{ m_kTeamBlue.AddScore(rkGuid, iPoint, iKill, iDead); }break;
	default:
		{
		}break;
	}
}
void PgBSTeamMng::AddTeamPoint(BM::GUID const& rkGuid, int const iPoint)
{
	CONT_BS_CHAR_TEAM::const_iterator team_iter = m_kContCharTeam.find(rkGuid);
	if( m_kContCharTeam.end() == team_iter )
	{
		return;
	}
	switch( (*team_iter).second )
	{
	case BST_RED:		{ m_kTeamRed.TeamPoint(m_kTeamRed.TeamPoint() + iPoint); }break;
	case BST_BLUE:		{ m_kTeamBlue.TeamPoint(m_kTeamBlue.TeamPoint() + iPoint); }break;
	default:
		{
		}break;
	}
}
int PgBSTeamMng::GetWinTeam() const
{
	if( m_kTeamRed.TeamPoint() > m_kTeamBlue.TeamPoint() )
	{
		return BST_RED;
	}
	else if( m_kTeamRed.TeamPoint() < m_kTeamBlue.TeamPoint() )
	{
		return BST_BLUE;
	}
	return BST_NONE;
}
int PgBSTeamMng::GetTeamMemberCount(int const iTeamNo) const
{
	switch( iTeamNo )
	{
	case BST_RED:		{ return m_kTeamRed.GetMemberCount(); }break;
	case BST_BLUE:		{ return m_kTeamBlue.GetMemberCount(); }break;
	default:
		{
		}break;
	}
	return 0;
}
void PgBSTeamMng::GetTeamMemberGuid(int const iTeamNo, VEC_GUID &rkOutCont) const
{
	PgBSTeam const* pkTeam = NULL;
	switch( iTeamNo )
	{
	case BST_RED:
		{
			pkTeam = &m_kTeamRed;
		}break;
	case BST_BLUE:
		{
			pkTeam = &m_kTeamBlue;
		}break;
	default:
		{
		}break;
	}
	if( NULL != pkTeam )
	{
		rkOutCont.reserve(pkTeam->ContMember().size());
		CONT_BS_TEAM_GUID_MEMBER::const_iterator iter = pkTeam->ContMember().begin();
		while( pkTeam->ContMember().end() != iter )
		{
			rkOutCont.push_back( (*iter).first );
			++iter;
		}
	}
}

void PgBSTeamMng::SetTeamIconCount(int const iTeamNo, int const iIconCount)
{
	switch (iTeamNo)
	{
	case BST_RED:
		{
			m_kTeamRed.IconCount(iIconCount);
		}break;
	case BST_BLUE:
		{
			m_kTeamBlue.IconCount(iIconCount);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Unknown Team value =") << iTeamNo);
		}break;
	}
}

void PgBSTeamMng::GetTeamPoint(int const iTeamNo, int& rPoint, int& rIconCount) const
{
	switch (iTeamNo)
	{
	case BST_RED:
		{
			rPoint = m_kTeamRed.TeamPoint();
			rIconCount = m_kTeamRed.IconCount();
		}break;
	case BST_BLUE:
		{
			rPoint = m_kTeamBlue.TeamPoint();
			rIconCount = m_kTeamBlue.IconCount();
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Unknown Team value =") << iTeamNo);
		}break;
	}
}

void PgBSTeamMng::IncreaseIcon(BM::GUID const& rkGuid)
{
	CONT_BS_CHAR_TEAM::const_iterator team_iter = m_kContCharTeam.find(rkGuid);
	if( m_kContCharTeam.end() == team_iter )
	{
		return;
	}
	switch( (*team_iter).second )
	{
	case BST_RED:
		{ 
			m_kTeamRed.IncreaseIcon(rkGuid);
		}break;
	case BST_BLUE:
		{
			m_kTeamBlue.IncreaseIcon(rkGuid);
		}break;
	default:
		{
		}break;
	}
}

void PgBSTeamMng::DropAllIcon(BM::GUID const& rkGuid)
{
	CONT_BS_CHAR_TEAM::const_iterator team_iter = m_kContCharTeam.find(rkGuid);
	if( m_kContCharTeam.end() == team_iter )
	{
		return;
	}
	switch( (*team_iter).second )
	{
	case BST_RED:
		{
			m_kTeamRed.DropAllIcon(rkGuid);
		}break;
	case BST_BLUE:
		{
			m_kTeamBlue.DropAllIcon(rkGuid);
		}break;
	default:
		{
		}break;
	}
}

