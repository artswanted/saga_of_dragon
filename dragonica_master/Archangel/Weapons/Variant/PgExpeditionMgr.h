#ifndef WEAPON_VARIANT_PARTY_PGEXPEDITIONMGR_H
#define WEAPON_VARIANT_PARTY_PGEXPEDITIONMGR_H

// 원정대. PgPartyMgr 참조해서 만듬.
template< class _T_Expedition >
class PgExpeditionMgr
{

public:

	typedef PgExpeditionMgr< typename _T_Expedition > T_MY_BASE_MGR_TYPE;
	
	typedef BM::TObjectPool< _T_Expedition > ExpeditionPool;
	typedef std::map< BM::GUID, _T_Expedition* > ExpeditionHash;

	typedef struct tagFunctionExpeditionForEach
	{
		tagFunctionExpeditionForEach() {}
		virtual void operator () (ExpeditionHash const & ContExpedition) = 0;
	}SFunctionExpeditionForEach;

public:

	PgExpeditionMgr();
	virtual ~PgExpeditionMgr();

	void Clear();
	
	virtual HRESULT CreateExpedition(std::wstring const &rkExpeditionName, _T_Expedition *& pkOutExpedition, BM::GUID& rkOutGuid, SExpeditionOption const& kNewOption);

protected:

	virtual HRESULT NewExpedition(BM::GUID const &rkExpeditionGuid, _T_Expedition * &pkOut, SExpeditionOption const& kNewOption, std::wstring const &rkNewName = _T(""));
	_T_Expedition* GetExpedition(BM::GUID const &rkExpeditionGuid) const;
	virtual bool DeleteExpedition(BM::GUID const &rkExpeditionGuid);

	void ExpeditionForEach(SFunctionExpeditionForEach & Function);

private:

	// 락은 상속 받은 클래스에서 잡는다
	ExpeditionPool m_kExpeditionPool;
	ExpeditionHash m_kContExpeditionHash;

};

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////
template< class _T_Expedition >
PgExpeditionMgr< _T_Expedition >::PgExpeditionMgr()
	: m_kExpeditionPool(500, 100), m_kContExpeditionHash()
{
}

template< class _T_Expedition >
PgExpeditionMgr< _T_Expedition >::~PgExpeditionMgr()
{
	Clear();
}

template< class _T_Expedition >
void PgExpeditionMgr< _T_Expedition >::Clear()
{
	ExpeditionHash::iterator iter = m_kContExpeditionHash.begin();
	while( m_kContExpeditionHash.end() != iter )
	{
		m_kExpeditionPool.Delete( (*iter).second );
		++iter;
	}
	m_kContExpeditionHash.clear();
}

template< class _T_Expedition >
HRESULT PgExpeditionMgr< _T_Expedition >::CreateExpedition(std::wstring const &rkExpeditionName, _T_Expedition *& pkOutExpedition, BM::GUID& rkOutGuid, SExpeditionOption const& kNewOption)
{
	BM::GUID const kNewGuid = BM::GUID::Create();

	_T_Expedition* pkExpedition = NULL;
	HRESULT const hResult = NewExpedition(kNewGuid, pkExpedition, kNewOption, rkExpeditionName);
	if( PRC_Success != hResult )
	{
		goto __FAILED;
	}

	// 마스터 추가는 바깥에서
	pkOutExpedition = pkExpedition;
	rkOutGuid = kNewGuid;
	return PRC_Success;

__FAILED:
	{
		DeleteExpedition(kNewGuid);
		return hResult; //실패
	}
}

template< class _T_Expedition >
_T_Expedition* PgExpeditionMgr< _T_Expedition >::GetExpedition(BM::GUID const & rkExpeditionGuid) const
{
	ExpeditionHash::const_iterator itor = m_kContExpeditionHash.find(rkExpeditionGuid);
	if( m_kContExpeditionHash.end() != itor )
	{
		return itor->second;
	}

	return NULL;
}

template< class _T_Expedition >
bool PgExpeditionMgr< _T_Expedition >::DeleteExpedition(BM::GUID const &rkExpeditionGuid)
{
	ExpeditionHash::iterator itor = m_kContExpeditionHash.find(rkExpeditionGuid);
	if( m_kContExpeditionHash.end() == itor )
	{
		return false;
	}

	ExpeditionHash::mapped_type pkExpedition = (*itor).second;
	if( pkExpedition )
	{
		m_kContExpeditionHash.erase(rkExpeditionGuid); // 원정대 포인터 삭제.
		pkExpedition->Clear();
		m_kExpeditionPool.Delete(pkExpedition); // 원정대 메모리 해제.

		return true;
	}

	return false;
}

template< class _T_Expedition >
HRESULT PgExpeditionMgr< _T_Expedition >::NewExpedition(BM::GUID const &rkExpeditionGuid, _T_Expedition * &pkOut, SExpeditionOption const& kNewOption, std::wstring const &rkNewName = _T(""))
{
	_T_Expedition* pkNewExpedition = m_kExpeditionPool.New();
	if( !pkNewExpedition )
	{
		return PRC_Fail;
	}

	pkNewExpedition->Clear();

	auto kRet = m_kContExpeditionHash.insert(std::make_pair(rkExpeditionGuid, pkNewExpedition));
	if( !kRet.second )
	{
		m_kExpeditionPool.Delete(pkNewExpedition);
		return PRC_Fail;
	}

	pkNewExpedition->Init(rkExpeditionGuid, rkNewName, kNewOption);

	pkOut = pkNewExpedition;

	return PRC_Success;
}

template< class _T_Expedition>
void PgExpeditionMgr< _T_Expedition >::ExpeditionForEach(SFunctionExpeditionForEach & Function)
{
	Function(m_kContExpeditionHash);
}

#endif  // WEAPON_VARIANT_PARTY_PGEXPEDITIONMGR_H