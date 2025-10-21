#ifndef MAP_MAPSERVER_OBJECT_PARTY_PGEXPEDITIONITEMRULE_H
#define MAP_MAPSERVER_OBJECT_PARTY_PGEXPEDITIONITEMRULE_H

// PartyItemRule과 동일 하지만 원정대 전용.
// 나중에 파티와 아이템 룰이 달라질 수 있기 때문에 별도로 만듬.

typedef struct tagExpeditionMemberGndKey
{
	tagExpeditionMemberGndKey(SGroundKey const & Key, BM::GUID const & Guid)
	{
		GndKey = Key;
		CharGuid = Guid;
	}

	bool const operator < (tagExpeditionMemberGndKey const & rhs) const
	{
		bool const bGndKey = ( GndKey < rhs.GndKey );
		bool const bGuidKey = ( CharGuid < rhs.CharGuid );
		if( bGndKey && bGuidKey )
		{
			return true;
		}
		else if( bGndKey != bGuidKey )
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
		return false;
	}

	bool const operator == (tagExpeditionMemberGndKey const & rhs) const
	{
		return (GndKey == rhs.GndKey) && (CharGuid == rhs.CharGuid);
	}

	bool const operator != (tagExpeditionMemberGndKey const & rhs) const
	{
		return !(*this == rhs);
	}

	SGroundKey GndKey;
	BM::GUID CharGuid;
}SExpeditionMemberGndKey;
typedef std::set<SExpeditionMemberGndKey> ContExpeditionMemberGndKey;

typedef struct tagExpeditionMemberItemScore
{
	tagExpeditionMemberItemScore()
	{
		Clear();
	}

	tagExpeditionMemberItemScore(BM::GUID const & CharGuid, unsigned short InitScore)
	{
		Clear();
		Set(CharGuid, InitScore);
	}

	void Set(BM::GUID const & Guid, unsigned short NewScore)
	{
		CharGuid = Guid;
		ItemScore = NewScore;
	}

	void Clear()
	{
		CharGuid.Clear();
		ItemScore = 0;
	}

	bool const operator < (tagExpeditionMemberItemScore const & rhs) const
	{
		return (ItemScore < rhs.ItemScore);
	}

	BM::GUID CharGuid;
	unsigned short ItemScore;
} SExpeditionMemberItemScore;
typedef std::list< SExpeditionMemberItemScore* > ContExpeditionItemScore;
typedef std::map< SGroundKey, ContExpeditionItemScore > ContExpeditionMemberItemScore;

class PgExpeditionItemRule
{

public:

	PgExpeditionItemRule();
	~PgExpeditionItemRule();

	void Clear();

	bool Add(SGroundKey const & GndKey, BM::GUID const & CharGuid);
	bool Del(SGroundKey const & GndKey, BM::GUID const & CharGuid);
	bool MapMove(SGroundKey const & OldGndKey, SGroundKey const & NewGndKey, BM::GUID const & CharGuid);
	bool GetNext(SGroundKey const & GndKey, BM::GUID & OutGuid);

	bool Sync(ContExpeditionMemberGndKey const & Set);

protected:

	bool GetGroup(SGroundKey const & GndKey, ContExpeditionItemScore *& pOut);
	bool AddGroup(SGroundKey const & GndKey, ContExpeditionItemScore *& pOut);
	bool DelGroup(SGroundKey const & GndKey);

	bool CheckLimitScore(SGroundKey const & GndKey);
	void ResetScore(SGroundKey const & GndKey);
	unsigned short GetMax(SGroundKey const & GndKey);

private:

	ContExpeditionMemberItemScore m_ItemScore;

};

#endif // MAP_MAPSERVER_OBJECT_PARTY_PGEXPEDITIONITEMRULE_H