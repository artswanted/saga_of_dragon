#ifndef MAP_MAPSERVER_OBJECT_PARTY_PGPARTYITEMRULE_H
#define MAP_MAPSERVER_OBJECT_PARTY_PGPARTYITEMRULE_H

//이 정보는 파티에 종속적이다
//
//파티에 파티원 별 아이템 습득 점수를 unsigned short로 기록한다.
//	매번 자신의 차례에 아이템을 습득하면 점수를 +1씩 한다.
//	USHRT_MAX에 모든 파티원이 도달하면 모든점수를 0으로 초기화 한다.
//	새로운 파티원이 들어오면 파티원의 점수는 이전의 가장 높은 파티원의 점수와 같게 만든다.(가장 마지막에 아이템을 습득)
//
//파티원별 아이템 습득 점수는 각 그라운드 별로 가진다.

typedef struct tagPartyMemberGndKey
{
	tagPartyMemberGndKey(SGroundKey const& rkGndKey, BM::GUID const & rkCharGuid)
	{
		kGndKey = rkGndKey;
		kCharGuid = rkCharGuid;
	}

	bool const operator < (const tagPartyMemberGndKey& rhs) const
	{
		bool const bGndKey = (kGndKey<rhs.kGndKey);
		bool const bGuidKey = (kCharGuid<rhs.kCharGuid);
		if( bGndKey && bGuidKey )
		{
			return true;
		}
		else if( bGndKey != bGuidKey )//어느 하나가 다르다
		{
			if( !bGndKey && bGuidKey )
			{
				return true;
			}
			else// if( bGndKey && !bGuidKey )
			{
				return false;
			}
		}
		//else if( !bGndKey && !bGuidKey )
		return false;//정렬 없이
	}

	bool const operator == (const tagPartyMemberGndKey& rhs) const
	{
		return (kGndKey==rhs.kGndKey) && (kCharGuid==rhs.kCharGuid);
	}

	bool const operator != (const tagPartyMemberGndKey& rhs) const
	{
		return !(*this==rhs);
	}
/*
	operator size_t ()const
	{
		return ((size_t)kGndKey)^((size_t)kCharGuid);
	}
*/
	SGroundKey kGndKey;
	BM::GUID kCharGuid;
} SPartyMemberGndKey;
typedef std::set< SPartyMemberGndKey > ContPartyMemberGndKey;

typedef struct tagMemberItemScore
{
	tagMemberItemScore()
	{
		Clear();
	}

	tagMemberItemScore(BM::GUID const & rkCharGuid, unsigned short sInitScore)
	{
		Clear();
		Set(rkCharGuid, sInitScore);
	}

	void Set(BM::GUID const & rkCharGuid, unsigned short sNewScore)
	{
		kCharGuid = rkCharGuid;
		sItemScore = sNewScore;
	}

	void Clear()
	{
		kCharGuid.Clear();
		sItemScore = 0;
	}

	bool const operator < (const tagMemberItemScore& rhs) const
	{
		return (sItemScore < rhs.sItemScore);
	}

	BM::GUID kCharGuid;
	unsigned short sItemScore;
} SMemberItemScore;
//typedef std::vector< SMemberItemScore > ContItemScoreVec;//메모리 홀더
typedef std::list< SMemberItemScore* > ContItemScore;//워킹 포인터
typedef std::map< SGroundKey, ContItemScore > ContMemberItemScore;

class PgPartyItemRule
{
public:
	PgPartyItemRule();
	~PgPartyItemRule();

	void Clear();

	bool Add(SGroundKey const& rkGndKey, BM::GUID const & rkCharGuid);
	bool Del(SGroundKey const& rkGndKey, BM::GUID const & rkCharGuid);
	bool MapMove(SGroundKey const& rkOldGndKey, SGroundKey const& rkNewGndKey, BM::GUID const & rkCharGuid);
	bool GetNext(SGroundKey const& rkGndKey, BM::GUID& rkOut);

	bool Sync(const ContPartyMemberGndKey& rkSet);

protected:
	bool GetGroup(SGroundKey const& rkGndKey, ContItemScore*& pkOut);
	bool AddGroup(SGroundKey const& rkGndKey, ContItemScore*& pkOut);
	bool DelGroup(SGroundKey const& rkGndKey);

	bool CheckLimitScore(SGroundKey const& rkGndKey);
	void ResetScore(SGroundKey const& rkGndKey);
	unsigned short GetMax(SGroundKey const& rkGndKey);

private:
	//ContItemScoreVec m_kScore;
	ContMemberItemScore m_kItemScore;
};

#endif // MAP_MAPSERVER_OBJECT_PARTY_PGPARTYITEMRULE_H