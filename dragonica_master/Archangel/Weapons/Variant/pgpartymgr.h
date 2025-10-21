#ifndef WEAPON_VARIANT_PARTY_PGPARTYMGR_H
#define WEAPON_VARIANT_PARTY_PGPARTYMGR_H

template< class _T_Party >
class PgPartyMgr
{
public:
	typedef PgPartyMgr< typename _T_Party > T_MY_BASE_MGR_TYPE;
	typedef BM::TObjectPool< _T_Party > PartyPool;
	typedef std::map< BM::GUID, _T_Party* > PartyHash;
	typedef struct tagFunctionForEach
	{
		tagFunctionForEach()	{}
		virtual void operator() (PartyHash const& rkContParty) = 0;
	} SFunctionForEach;

	typedef BM::TObjectPool<SFindPartyUserListInfo> FindPartyUserInfoPool;
	typedef std::map<BM::GUID, SFindPartyUserListInfo* > FindPartyUserInfo;
	typedef struct tagFunctionForEachFindPartyUserList
	{
		tagFunctionForEachFindPartyUserList() { }
		virtual void operator () (FindPartyUserInfo const& rkContFindPartyUserList) = 0;
	} FunctionForEachFindPartyUserList;


public:
	PgPartyMgr();
	virtual ~PgPartyMgr();

	void Clear();

	virtual HRESULT CreateParty(std::wstring const &rkPartyName, BM::GUID& rkOutGuid, SPartyOption const& kNewOption);
 	
protected:
	virtual HRESULT NewParty(BM::GUID const &rkPartyGuid, _T_Party* &pkOut, SPartyOption const& kNewOption, std::wstring const &rkNewName = _T(""));
	virtual bool Delete(BM::GUID const &rkPartyGuid);
	_T_Party* GetParty(BM::GUID const &rkPartyGuid)const;
	void ForEach(SFunctionForEach& rkFunction)const;
	void ForEach(FunctionForEachFindPartyUserList& rkFunction)const;

protected:
	
	FindPartyUserInfoPool m_kFindPartyUserInfoPool;
	FindPartyUserInfo m_kContFindPartyUserInfo;

protected:
	// 락은 상속 받은 클래스에서 잡는다
	PartyHash m_kContPartyHash;

private:
	// 락은 상속 받은 클래스에서 잡는다
	PartyPool m_kPartyPool;

};

////////////////////////////
template< class _T_Party >
PgPartyMgr< _T_Party >::PgPartyMgr()
	: m_kPartyPool(500, 100), m_kContPartyHash()
{
}

template< class _T_Party >
PgPartyMgr< _T_Party >::~PgPartyMgr()
{
	Clear();
}

template< class _T_Party >
void PgPartyMgr< _T_Party >::Clear()
{
	PartyHash::iterator erase_iter = m_kContPartyHash.begin();
	while( m_kContPartyHash.end() != erase_iter )
	{
		m_kPartyPool.Delete( (*erase_iter).second );
		++erase_iter;
	}
	m_kContPartyHash.clear();
}

template< class _T_Party >
HRESULT PgPartyMgr< _T_Party >::CreateParty(std::wstring const &rkPartyName, BM::GUID& rkOutGuid, SPartyOption const& kNewOption)
{
	BM::GUID const kNewGuid = BM::GUID::Create();

	_T_Party* pkParty = NULL;
	HRESULT const hResult = NewParty(kNewGuid, pkParty, kNewOption, rkPartyName);
	if( PRC_Success != hResult )
	{
		goto __FAILED;
	}

	//마스터 추가는 바깥에서

	rkOutGuid = kNewGuid;
	return PRC_Success; //성공

__FAILED:
	{
		Delete(kNewGuid);
		return hResult;//실패
	}
}

template< class _T_Party >
HRESULT PgPartyMgr< _T_Party >::NewParty(BM::GUID const &rkPartyGuid, _T_Party* &pkOut, SPartyOption const& kNewOption, std::wstring const &rkNewName)
{
	_T_Party* pkNewParty = m_kPartyPool.New();
	if( !pkNewParty )
	{
		return PRC_Fail;
	}

	pkNewParty->Clear();

	auto kRet = m_kContPartyHash.insert( std::make_pair(rkPartyGuid, pkNewParty) );
	if( !kRet.second )
	{
		m_kPartyPool.Delete(pkNewParty);
		return PRC_Fail;
	}

	pkNewParty->Init(rkPartyGuid, rkNewName, kNewOption);//이 안에서 Guid Generate
	
	pkOut = pkNewParty;

	return PRC_Success;
}

template< class _T_Party >
_T_Party* PgPartyMgr< _T_Party >::GetParty(BM::GUID const &rkPartyGuid)const
{
	PartyHash::const_iterator itor = m_kContPartyHash.find(rkPartyGuid);
	if(m_kContPartyHash.end() != itor)
	{
		return itor->second;
	}
	return NULL;
}

template< class _T_Party >
bool PgPartyMgr< _T_Party >::Delete(BM::GUID const &rkPartyGuid)
{
	PartyHash::iterator itor = m_kContPartyHash.find(rkPartyGuid);
	if( m_kContPartyHash.end() == itor )
	{ 
		return false; 
	}

	PartyHash::mapped_type pkParty = (*itor).second;
	if( pkParty )
	{
		m_kContPartyHash.erase(rkPartyGuid);// 파티의 포인터해쉬에서 삭제
		//m_kContPartyNameSet.erase(pkParty->PartyName());
		pkParty->Clear();
		m_kPartyPool.Delete(pkParty);// 파티의 메모리삭제

		return true;
	}

	return false;
}

template< class _T_Party >
void PgPartyMgr< _T_Party >::ForEach(SFunctionForEach& rkFunction)const
{
	rkFunction(m_kContPartyHash);
}

template< class _T_Party >
void PgPartyMgr< _T_Party >::ForEach(FunctionForEachFindPartyUserList& rkFunction)const
{
	rkFunction(m_kContFindPartyUserInfo);
}

#endif // WEAPON_VARIANT_PARTY_PGPARTYMGR_H