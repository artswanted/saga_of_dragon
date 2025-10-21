#include "stdafx.h"
#include "PgExpeditionItemRule.h"

#pragma warning( push, 4 )

BM::TObjectPool< SExpeditionMemberItemScore > g_ExpeditionScorePool(1000, 100);

PgExpeditionItemRule::PgExpeditionItemRule()
{
}

PgExpeditionItemRule::~PgExpeditionItemRule()
{
	Clear();
}

void PgExpeditionItemRule::Clear()
{
	ContExpeditionMemberItemScore::iterator erase_iter = m_ItemScore.begin();
	while( erase_iter != m_ItemScore.end() )
	{
		ContExpeditionItemScore & List = (*erase_iter).second;
		ContExpeditionItemScore::iterator sub_iter = List.begin();
		while( sub_iter != List.end() )
		{
			g_ExpeditionScorePool.Delete((*sub_iter));
			++sub_iter;
		}
		++erase_iter;
	}

	m_ItemScore.clear();
}

bool PgExpeditionItemRule::GetGroup(SGroundKey const & GndKey, ContExpeditionItemScore *& pOut)
{
	ContExpeditionMemberItemScore::iterator group_iter = m_ItemScore.find(GndKey);
	if( group_iter != m_ItemScore.end() )
	{
		ContExpeditionMemberItemScore::mapped_type & Element = (*group_iter).second;
		pOut = &Element;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgExpeditionItemRule::AddGroup(SGroundKey const & GndKey, ContExpeditionItemScore *& pOut)
{
	auto Ret = m_ItemScore.insert( std::make_pair(GndKey, ContExpeditionMemberItemScore::mapped_type()) );
	if( Ret.second )
	{
		ContExpeditionMemberItemScore::mapped_type & Element = (*Ret.first).second;
		Element.clear();
		pOut = &Element;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgExpeditionItemRule::DelGroup(SGroundKey const & GndKey)
{
	ContExpeditionMemberItemScore::iterator group_iter = m_ItemScore.find(GndKey);
	if( group_iter != m_ItemScore.end() )
	{
		ContExpeditionMemberItemScore::mapped_type & Element = group_iter->second;
		ContExpeditionItemScore::iterator iter = Element.begin();
		for( ; iter != Element.end() ; ++iter )
		{
			SAFE_DELETE(*iter);
		}
		m_ItemScore.erase(group_iter);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgExpeditionItemRule::Add(SGroundKey const & GndKey, BM::GUID const & CharGuid)
{
	ContExpeditionMemberItemScore::mapped_type * pGndGroupScore = NULL;
	bool const FindGroup = GetGroup(GndKey, pGndGroupScore);
	if( !FindGroup )
	{
		bool const bAddGroup = AddGroup(GndKey, pGndGroupScore);
		if( !bAddGroup )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	bool Duplicate = false;
	ContExpeditionItemScore::iterator member_iter = pGndGroupScore->begin();
	while( member_iter != pGndGroupScore->end() )
	{
		ContExpeditionItemScore::value_type const pElement = (*member_iter);
		if( pElement->CharGuid == CharGuid )
		{
			Duplicate = true;//중복.
			break;
		}
		++member_iter;
	}

	if( !Duplicate )	// 중복이 아닐 때 만.
	{
		ContExpeditionItemScore::value_type pElement = g_ExpeditionScorePool.New();
		if( !pElement )
		{
			assert(pElement);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		pElement->Set(CharGuid, GetMax(GndKey));
		pGndGroupScore->push_back(pElement);
	}
	return true;
}

bool PgExpeditionItemRule::Del(SGroundKey const & GndKey, BM::GUID const & CharGuid)
{
	ContExpeditionMemberItemScore::mapped_type * pGndGroupScore = NULL;
	bool const FindGroup = GetGroup(GndKey, pGndGroupScore);
	if( !FindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContExpeditionItemScore::iterator member_iter = pGndGroupScore->begin();
	while( member_iter != pGndGroupScore->end() )
	{
		ContExpeditionItemScore::value_type pElement = (*member_iter);
		if( pElement->CharGuid == CharGuid )
		{
			g_ExpeditionScorePool.Delete(pElement);
			pGndGroupScore->erase(member_iter);
			break;
		}
		++member_iter;
	}
	return true;
}
	
bool PgExpeditionItemRule::MapMove(SGroundKey const & OldGndKey, SGroundKey const & NewGndKey, BM::GUID const & CharGuid)
{
	bool const DelRet = Del(OldGndKey, CharGuid);
	if( !DelRet)
	{
		//삭제는 실패 해도 ok
	}

	bool const AddRet = Add(NewGndKey, CharGuid);
	if( !AddRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgExpeditionItemRule::GetNext(SGroundKey const & GndKey, BM::GUID & OutGuid)
{
	ContExpeditionMemberItemScore::mapped_type * pGndGroupScore = NULL;
	bool const FindGroup = GetGroup(GndKey, pGndGroupScore);
	if( !FindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( pGndGroupScore->size() )
	{
		ContExpeditionItemScore::value_type pElement = (*pGndGroupScore->begin()); // 가장 작은 애.
		pElement->ItemScore += 1; //스코어 1증가.
		if( USHRT_MAX <= pElement->ItemScore ) // 최대값에 도달 하면.
		{
			bool const Limit = CheckLimitScore(GndKey); // 다른 것들도 최대치에 도달 했는지 체크.
			if( Limit )
			{
				ResetScore(GndKey); // 초기화.
				pElement->ItemScore = 1; // 마지막 사람은 스코어 1.
			}
		}

		OutGuid = pElement->CharGuid;

		pGndGroupScore->pop_front();
		pGndGroupScore->push_back(pElement); // 맨 뒤로 보냄.

		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgExpeditionItemRule::Sync(ContExpeditionMemberGndKey const & Set)
{
	SET_GUID GuidSet;
	ContExpeditionMemberItemScore::iterator group_iter = m_ItemScore.begin();
	while( group_iter != m_ItemScore.end() )
	{
		bool EraseGroup = false;
		ContExpeditionMemberItemScore::key_type const & GndKey = (*group_iter).first;

		ContExpeditionMemberItemScore::mapped_type * pGndGroupScore = NULL;
		bool const FindGroup = GetGroup(GndKey, pGndGroupScore);
		if( FindGroup )
		{
			ContExpeditionItemScore::iterator member_iter = pGndGroupScore->begin();
			while( member_iter != pGndGroupScore->end() )
			{
				ContExpeditionItemScore::value_type pElement = (*member_iter);
				SExpeditionMemberGndKey MemberGndKey(GndKey, pElement->CharGuid);
				bool const FindMember = (Set.end() != Set.find(MemberGndKey));
				if( !FindMember )
				{
					GuidSet.insert(pElement->CharGuid); // 지울 목록.
					g_ExpeditionScorePool.Delete(pElement); // 메모리 반환.
					member_iter = pGndGroupScore->erase(member_iter);
				}
				else	// 찾았다면 진행.
				{
					++member_iter;
				}
			}

			EraseGroup = pGndGroupScore->empty(); // 비었으면 삭제.
		}

		if( EraseGroup ) // 삭제되면
		{
			group_iter = m_ItemScore.erase(group_iter); // 삭제 뒤 부터.
		}
		else
		{
			++group_iter; // 아니면 진행.
		}
	}

	return true;
}

bool PgExpeditionItemRule::CheckLimitScore(SGroundKey const & GndKey)
{
	ContExpeditionMemberItemScore::mapped_type * pGndGroupScore = NULL;
	bool const FindGroup = GetGroup(GndKey, pGndGroupScore);
	if( !FindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool Limit = true;
	ContExpeditionItemScore::iterator member_iter = pGndGroupScore->begin();
	while( member_iter != pGndGroupScore->end() )
	{
		ContExpeditionItemScore::value_type pElement = (*member_iter);
		Limit &= (USHRT_MAX <= pElement->ItemScore );
		++member_iter;
	}

	return Limit; // 최대치 도달했나.
}

void PgExpeditionItemRule::ResetScore(SGroundKey const & GndKey)
{
	ContExpeditionMemberItemScore::mapped_type * pGndGroupScore = NULL;
	bool const FindGroup = GetGroup(GndKey, pGndGroupScore);
	if( !FindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetGroup is Failed!"));
		return ;
	}

	ContExpeditionItemScore::iterator reset_iter = pGndGroupScore->begin();
	while( reset_iter != pGndGroupScore->end() )
	{
		ContExpeditionItemScore::value_type pElement = (*reset_iter);
		pElement->ItemScore = 0; // 0으로 리셋.
		++reset_iter;
	}
}

unsigned short PgExpeditionItemRule::GetMax(SGroundKey const & GndKey)
{
	ContExpeditionMemberItemScore::mapped_type * pGndGroupScore = NULL;
	bool const FindGroup = GetGroup(GndKey, pGndGroupScore);
	if( !FindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( pGndGroupScore->size() )
	{
		pGndGroupScore->sort();
		ContExpeditionItemScore::value_type const pElement = (*pGndGroupScore->rbegin()); // 가장 큰 것.
		return pElement->ItemScore;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}