#ifndef WEAPON_VARIANT_ACTION_BATTLE_PGAGGROMETER_H
#define WEAPON_VARIANT_ACTION_BATTLE_PGAGGROMETER_H

//
typedef struct tagAggroOwner
{
	BM::GUID kGuid;//누가
	BM::GUID kOwner;//누구의 주인 링크
	
	tagAggroOwner()
	{
		Clear();
	}
	tagAggroOwner(BM::GUID const &rkGuid, BM::GUID const &rkOwner)
	{
		Set(rkGuid, rkOwner);
	}

	void Clear()
	{
		kGuid.Clear();
		kOwner.Clear();
	}

	void Set(BM::GUID const &rkGuid, BM::GUID const &rkOwner)
	{
		kGuid = rkGuid;
		kOwner = rkOwner;
	}
/*
	operator size_t ()const
	{
		return (size_t)kGuid ^ (size_t)kOwner;
	}
*/
	bool operator < (tagAggroOwner const & rhs) const 
	{
		bool const bGuidRet = kGuid < rhs.kGuid;
		bool const bOwnerRet = kOwner < rhs.kOwner;
		if( bGuidRet && bOwnerRet )
		{
			return true;
		}
		if( bGuidRet && !bOwnerRet )
		{
			return true;
		}
		//( !bGuidRet && bOwnerRet )
		//( !bGuidRet && !bOwnerRet )
		return false;
	}

	bool operator != (tagAggroOwner const & rhs) const
	{
		return ((*this < rhs) || (rhs < *this));
	}

	bool operator == (tagAggroOwner const & rhs) const
	{
		return !(*this != rhs);
	}

	bool IsNull() const
	{
		return ((BM::GUID::NullData() == kGuid) && (BM::GUID::NullData() == kOwner));
	}
} SAggroOwner;


//
typedef struct tagAggro
{
	SAggroOwner kOwner;		//누가?
	size_t iScore;			//소유 점수(누가 소유 할건지)
	size_t iDamageAggro;	//누가 때린 총 어그로 양
	size_t iHitAggro;		//누가 맞은 총 어그로 양
	size_t iDamageCount;	//누가 때린 총 횟수
	size_t iHitCount;		//누가 맞은 총 횟수
	
	DWORD dwLastUpdatedTime;	//마지막 갱신 시간

	BM::GUID const& Guid() const	{ return kOwner.kGuid; };
	BM::GUID const& Owner() const	{ return kOwner.kOwner; };

	tagAggro();
	tagAggro(BM::GUID const &rkGuid, BM::GUID const &rkOwner);
	void Clear();
	void SetOwner(SAggroOwner const &rkOwner);
	void SetOwner(BM::GUID const &rkGuid, BM::GUID const &rkOwner);
	void Add(size_t const iAddScore, size_t const iAddDamage, size_t const iAddHit);
	static bool Score_Greater(tagAggro const* pLeft, tagAggro const* pRight);
	static bool Damage_Greater(tagAggro const* pLeft, tagAggro const* pRight);

	typedef std::vector< SAggroOwner > ContEraseVec;
	class PgSort_IntervalOver
	{
	public:
		PgSort_IntervalOver(DWORD const dwNowTime, DWORD const dwDiffInterval, ContEraseVec &rkOut);
		~PgSort_IntervalOver();

		void operator() (tagAggro const *pRight);

	private:
		DWORD const m_dwNowTime;
		DWORD const m_dwDiffInterval;
		ContEraseVec& m_kOut;
	};
} SAggro;


//
typedef enum eAggroMeterType
{
	AMT_None = 0,
	AMT_Damage,
	AMT_Hit,
} EAggroMeterType;


//
class PgAggroMeter
{
	typedef std::list< SAggro* > ContAggro;
	typedef std::map< SAggroOwner, SAggro* > ContAggroMap;

public:
	PgAggroMeter();
	PgAggroMeter(BM::GUID const &rkGuid, int const iHP);
	~PgAggroMeter();

	void Clear(bool bOnlyData = false);

	//때린것이 맞은것 보다 우선
	void Set(BM::GUID const &rkGuid, int const iHP);

	//때린 놈이 혼자 인 경우 / 파티가 있는 경우
	//때린 놈이 소환물인 경우
	//소환물의 주인이 파티가 있는 경우
	bool AddVal(const EAggroMeterType eType, SAggroOwner const &rkCaster, int const iDamage, VEC_GUID const* pkMember = NULL);

	//void Print();
	size_t GetScoreTop(VEC_GUID& rkOut, size_t const iTop = 3); //스코어 순위대로 뽑자
	size_t GetDamageTop(VEC_GUID& rkOut, size_t const iTop = 3); //순수 데미지 순위대로 뽑자

	size_t GetScoreBottom(VEC_GUID& rkOut, size_t const iTop = 3); //스코어 순위대로 뽑자
	size_t GetDamageBottom(VEC_GUID& rkOut, size_t const iTop = 3); //순수 데미지 순위대로 뽑자

	bool IsDiffCaller(SAggroOwner const& rkOwner) const;	//콜러와 오너 GUID가 다르냐?

protected:
	size_t GetTop(VEC_GUID& rkOut, size_t const iTop = 3) const;
	size_t GetBottom(VEC_GUID& rkOut, size_t const iTop = 3) const;

	void SortScore();
	void SortDamage();
	//void SortHit();

	bool AddDamage(SAggroOwner const &rkCaster, size_t const iDamage, VEC_GUID const* pkMember);
	bool AddHit(SAggroOwner const &rkTarget, size_t const iHit, VEC_GUID const* pkMember);
	inline void AddDamageScore(SAggroOwner const &rkOwner, size_t const iScore, size_t const iDamage, float fPercent = 1.f, size_t const iBonus = 0);
	inline void AddHitScore(SAggroOwner const &rkOwner, size_t const iScore, size_t const iHit, float fPercent = 1.f, size_t const iBonus = 0);

	bool GetAggro(SAggroOwner const &rkOwner, SAggro*& pkAggro);

	bool Add(SAggroOwner const &rkOwner, SAggro*& pkAggro);
	bool Get(SAggroOwner const &rkOwner, SAggro*& pkAggro);
	bool Del(SAggroOwner const &rkOwner);

	bool RemoveOldedAggro(DWORD const dwInterval);
private:
	mutable Loki::Mutex m_kMutex;

	BM::GUID m_kGuid;			//주인(몬스터)

	ContAggroMap m_kMap;		//찾기 용 < 대상Guid, 대상 개인의 어그로미터* >
	ContAggro m_kMeter;			//정렬 용 < 대상 개인의 어그로미터* >

	int m_iHP;
	size_t m_iTotalDamage;		//총 입은 피해량
	size_t m_iTotalHit;			//총 입힌 피해량
	size_t m_iTotalDamageCount;	//총 입은 피해횟수
	size_t m_iTotalHitCount;	//총 입힌 피해횟수
};



inline void PgAggroMeter::AddDamageScore(SAggroOwner const &rkOwner, size_t const iScore, size_t const iDamage, float fPercent, size_t const iBonus)
{
	if( !iScore )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("iScore is 0"));
		return;
	}

	if( BM::GUID::NullData() == rkOwner.kGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Guid is NullData"));
		return;
	}

	SAggro* pkAggro = NULL;
	bool const bFind = GetAggro(rkOwner, pkAggro);
	if( bFind )
	{
		size_t iResult = (size_t)((iScore+iBonus)*fPercent);
		iResult = __max(iResult, 1);
		pkAggro->Add(iResult, iDamage, 0);
	}
}


inline void PgAggroMeter::AddHitScore(SAggroOwner const &rkOwner, size_t const iScore, size_t const iHit, float fPercent, size_t const iBonus)
{
	if( !iScore )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("iScore is 0"));
		return;
	}

	if( BM::GUID::NullData() == rkOwner.kGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Guid is NullData"));
		return;
	}

	SAggro* pkAggro = NULL;
	bool const bFind = GetAggro(rkOwner, pkAggro);
	if( bFind )
	{
		size_t iResult = (size_t)((iScore+iBonus)*fPercent);
		iResult = __max(iResult, 1);
		pkAggro->Add(iResult, 0, iHit);
	}
}

#endif // WEAPON_VARIANT_ACTION_BATTLE_PGAGGROMETER_H