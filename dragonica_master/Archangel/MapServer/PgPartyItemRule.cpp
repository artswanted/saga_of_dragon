#include "stdafx.h"
#include "PgPartyItemRule.h"

#pragma warning (push, 4)

BM::TObjectPool< SMemberItemScore > g_kPartyScorePool(1000, 100);

PgPartyItemRule::PgPartyItemRule()
{
}

PgPartyItemRule::~PgPartyItemRule()
{
	Clear();
}

void PgPartyItemRule::Clear()
{
	ContMemberItemScore::iterator erase_iter = m_kItemScore.begin();
	while( m_kItemScore.end() != erase_iter )
	{
		ContItemScore &rkList = (*erase_iter).second;
		ContItemScore::iterator sub_iter = rkList.begin();
		while( rkList.end() != sub_iter )
		{
			g_kPartyScorePool.Delete((*sub_iter));
			++sub_iter;
		}
		++erase_iter;
	}

	//m_kScore.clear();
	m_kItemScore.clear();
}

bool PgPartyItemRule::GetGroup(SGroundKey const& rkGndKey, ContItemScore*& pkOut)
{
	ContMemberItemScore::iterator group_iter = m_kItemScore.find(rkGndKey);
	if(m_kItemScore.end() != group_iter)
	{
		ContMemberItemScore::mapped_type& rkElement = (*group_iter).second;
		pkOut = &rkElement;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
bool PgPartyItemRule::AddGroup(SGroundKey const& rkGndKey, ContItemScore*& pkOut)
{
	auto kRet = m_kItemScore.insert(std::make_pair(rkGndKey, ContMemberItemScore::mapped_type()));
	if( kRet.second )
	{
		ContMemberItemScore::mapped_type& rkElement = (*kRet.first).second;
		rkElement.clear();
		pkOut = &rkElement;
		return true;//성공
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;//실패
}

bool PgPartyItemRule::DelGroup(SGroundKey const& rkGndKey)
{
	ContMemberItemScore::iterator group_iter = m_kItemScore.find(rkGndKey);
	if(m_kItemScore.end() != group_iter)
	{
		ContMemberItemScore::mapped_type &kElement = group_iter->second;
		ContItemScore::iterator itr = kElement.begin();
		for ( ; itr != kElement.end() ; ++itr )
		{
			SAFE_DELETE( *itr );
		}
		m_kItemScore.erase(group_iter);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPartyItemRule::MapMove(SGroundKey const& rkOldGndKey, SGroundKey const& rkNewGndKey, BM::GUID const & rkCharGuid)
{
	bool const bDelRet = Del(rkOldGndKey, rkCharGuid);
	if( !bDelRet )
	{
		//삭제는 실패 해도 ok
	}
	bool const bAddRet = Add(rkNewGndKey, rkCharGuid);
	if( !bAddRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//실패
	}
	return true;
}


bool PgPartyItemRule::Add(SGroundKey const& rkGndKey, BM::GUID const & rkCharGuid)
{
	ContMemberItemScore::mapped_type* pkGndGroupScore = NULL;
	bool const bFindGroup = GetGroup(rkGndKey, pkGndGroupScore);
	if( !bFindGroup )
	{
		bool const bAddGroup = AddGroup(rkGndKey, pkGndGroupScore);//없으면 만들어
		if( !bAddGroup )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	bool bDuplicate = false;
	ContItemScore::iterator member_iter = pkGndGroupScore->begin();
	while(pkGndGroupScore->end() != member_iter)
	{
		const ContItemScore::value_type pkElement = (*member_iter);
		if( pkElement->kCharGuid == rkCharGuid )
		{
			bDuplicate = true;//중복
			break;
		}
		++member_iter;
	}

	if( !bDuplicate )//중복 아닐 때 만 추가
	{
		ContItemScore::value_type pkElement = g_kPartyScorePool.New();
		if( !pkElement )
		{
			assert(pkElement);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		pkElement->Set(rkCharGuid, GetMax(rkGndKey));
		pkGndGroupScore->push_back(pkElement);
	}
	return true;
}

bool PgPartyItemRule::Del(SGroundKey const& rkGndKey, BM::GUID const & rkCharGuid)
{
	ContMemberItemScore::mapped_type* pkGndGroupScore = NULL;
	bool const bFindGroup = GetGroup(rkGndKey, pkGndGroupScore);
	if( !bFindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContItemScore::iterator member_iter = pkGndGroupScore->begin();//포인터 삭제
	while(pkGndGroupScore->end() != member_iter)
	{
		ContItemScore::value_type pkElement = (*member_iter);
		if( pkElement->kCharGuid == rkCharGuid )
		{
			g_kPartyScorePool.Delete(pkElement);
			pkGndGroupScore->erase(member_iter);
			break;
		}
		++member_iter;
	}

	//ContItemScoreVec::iterator vec_iter = m_kScore.begin();//실제 메모리 삭제
	//while(m_kScore.end() != vec_iter)
	//{
	//	ContItemScoreVec::value_type& rkElement = (*vec_iter);
	//	if( rkElement.kCharGuid == rkCharGuid )
	//	{
	//		m_kScore.erase(vec_iter);
	//		break;
	//	}
	//	++vec_iter;
	//}
	return true;
}

bool PgPartyItemRule::GetNext(SGroundKey const& rkGndKey, BM::GUID& rkOut)
{
	ContMemberItemScore::mapped_type* pkGndGroupScore = NULL;
	bool const bFindGroup = GetGroup(rkGndKey, pkGndGroupScore);
	if( !bFindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( pkGndGroupScore->size() )
	{
		//정렬을 쓰지 말고 vector<T>와 list<T*> 를 써서 list에 pop_front, push_back을 하자
		//pkGndGroupScore->sort();//정렬
		ContItemScore::value_type pkElement = (*pkGndGroupScore->begin());//가장 작은(항상 앞)
		pkElement->sItemScore += 1;//스코어 1증가
		if( USHRT_MAX <= pkElement->sItemScore )//현재 상태가 최대치
		{
			bool const bLimit = CheckLimitScore(rkGndKey);//다른것도 최대치 도달 체크
			if( bLimit )
			{
				ResetScore(rkGndKey);//도달했으면 전부 리셋
				pkElement->sItemScore = 1;//마지막 사람은 스코어 1
			}
		}

		rkOut = pkElement->kCharGuid;

		pkGndGroupScore->pop_front();
		pkGndGroupScore->push_back(pkElement);//가장 뒤로

		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPartyItemRule::CheckLimitScore(SGroundKey const& rkGndKey)
{
	ContMemberItemScore::mapped_type* pkGndGroupScore = NULL;
	bool const bFindGroup = GetGroup(rkGndKey, pkGndGroupScore);
	if( !bFindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool bLimit = true;
	ContItemScore::iterator member_iter = pkGndGroupScore->begin();
	while(pkGndGroupScore->end() != member_iter)
	{
		ContItemScore::value_type pkElement = (*member_iter);
		bLimit &= (USHRT_MAX <= pkElement->sItemScore);
		++member_iter;
	}
	return bLimit;//최대치에 도달?
}

void PgPartyItemRule::ResetScore(SGroundKey const& rkGndKey)
{
	ContMemberItemScore::mapped_type* pkGndGroupScore = NULL;
	bool const bFindGroup = GetGroup(rkGndKey, pkGndGroupScore);
	if( !bFindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetGroup is Failed!"));
		return ;
	}

	//스코어를 전부
	ContItemScore::iterator reset_iter = pkGndGroupScore->begin();
	while(pkGndGroupScore->end() != reset_iter)
	{
		ContItemScore::value_type pkElement = (*reset_iter);
		pkElement->sItemScore = 0;//0으로 리셋
		++reset_iter;
	}
}

unsigned short PgPartyItemRule::GetMax(SGroundKey const& rkGndKey)
{
	ContMemberItemScore::mapped_type* pkGndGroupScore = NULL;
	bool const bFindGroup = GetGroup(rkGndKey, pkGndGroupScore);
	if( !bFindGroup )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	if( pkGndGroupScore->size() )
	{
		pkGndGroupScore->sort();
		const ContItemScore::value_type pkElement = (*pkGndGroupScore->rbegin());//가장큰
		return pkElement->sItemScore;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

bool PgPartyItemRule::Sync(const ContPartyMemberGndKey& rkSet)
{
	//내 그라운드 그룹별, 전체목록을 돌면서 있으면 유지 없으면 삭제
	SET_GUID kGuidSet;
	ContMemberItemScore::iterator group_iter = m_kItemScore.begin();
	while(m_kItemScore.end() != group_iter)
	{
		bool bEraseGroup = false;
		const ContMemberItemScore::key_type& rkGndKey = (*group_iter).first;

		ContMemberItemScore::mapped_type* pkGndGroupScore = NULL;
		bool const bFindGroup = GetGroup(rkGndKey, pkGndGroupScore);
		if( bFindGroup )
		{
			ContItemScore::iterator member_iter = pkGndGroupScore->begin();
			while(pkGndGroupScore->end() != member_iter)
			{
				ContItemScore::value_type pkElement = (*member_iter);
				SPartyMemberGndKey kMemberGndKey(rkGndKey, pkElement->kCharGuid);
				bool const bFindMember = (rkSet.end() != rkSet.find(kMemberGndKey));
				if( !bFindMember )//찾지 못하였으면 삭제
				{
					kGuidSet.insert(pkElement->kCharGuid);//지울 목록에 추가
					g_kPartyScorePool.Delete(pkElement);//메모리 반환
					member_iter = pkGndGroupScore->erase(member_iter);
				}
				else//찾았으면 진행
				{
					++member_iter;
				}
			}

			bEraseGroup = pkGndGroupScore->empty();//비었으면 삭제
		}

		if( bEraseGroup )//삭제면
		{
			group_iter = m_kItemScore.erase(group_iter);//삭제 뒤 부터
		}
		else
		{
			++group_iter;//아니면 진행
		}
	}

	////실제 메모리도 삭제
	//ContItemScoreVec::iterator vec_iter = m_kScore.begin();
	//while(m_kScore.end() != vec_iter)
	//{
	//	const ContItemScoreVec::value_type& rkElement = (*vec_iter);
	//	bool bFindMember = (kGuidSet.end() != kGuidSet.find(rkElement.kCharGuid));
	//	if( bFindMember )//삭제목록에 있으면 삭제
	//	{
	//		vec_iter = m_kScore.erase(vec_iter);
	//	}
	//	else
	//	{
	//		++vec_iter;
	//	}
	//}

	return true;
}
