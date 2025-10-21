#include <tchar.h>

template < class T, template <class> class CreationPolicy >
TObjectPool< T, CreationPolicy >::TObjectPool()
#ifndef _MDo_
	: CPoolObserver(typeid(T).name())
#endif
{
	m_nReserveCount	= 0;
//	Init(0, V_DEFAULT_INC_SIZE);
}

template < class T, template <class> class CreationPolicy >
TObjectPool< T, CreationPolicy >::TObjectPool(size_t const init_size, size_t const inc_count)
#ifndef _MDo_
	: CPoolObserver(typeid(T).name())
#endif
{
	m_nReserveCount	= 0;
//	Init(init_size, inc_count);
}

template < class T, template <class> class CreationPolicy >
TObjectPool< T, CreationPolicy >::~TObjectPool()
{
//	Terminate();
}

template < class T, template <class> class CreationPolicy >
void TObjectPool< T, CreationPolicy >::swap(TObjectPool<T,CreationPolicy>& rkRight)
{
	return;
/*	BM::CAutoMutex kLockFree(m_kFreeMutex);

	m_listForDel.swap(rkRight.m_listForDel);
	m_lstFree.swap(rkRight.m_lstFree);
	std::swap(m_inc_count,rkRight.m_inc_count);
	std::swap(m_nUsedCount,rkRight.m_nUsedCount);
	std::swap(m_nMaxUsedCount,rkRight.m_nMaxUsedCount);
	std::swap(m_nReserveCount,rkRight.m_nReserveCount);

#ifdef _DEBUG
	m_kContTotal.swap(rkRight.m_kContTotal);
	m_kContOut.swap(rkRight.m_kContOut);
	m_kContWait.swap(rkRight.m_kContWait);
#endif
*/
}

template < class T, template <class> class CreationPolicy >
size_t TObjectPool< T, CreationPolicy >::Init(size_t const init_size, size_t const inc_count)
{
	return 0;
/*	BM::CAutoMutex kLockFree(m_kFreeMutex);

	m_inc_count = __max( inc_count, 0 );
	
	if(init_size)
	{
		Grow(init_size);
	}

	return GetCapacity();
*/
}

template < class T, template <class> class CreationPolicy >
void TObjectPool< T, CreationPolicy >::Terminate()
{
/*
	BM::CAutoMutex kLockFree(m_kFreeMutex);
#ifndef EXTERNAL_RELEASE
	char buf[256] = { 0, };
	sprintf_s(buf, 256, "[ObjectPool] %s object pool has maximum %d (%d,%d) element\n", typeid(T).name(), m_nMaxUsedCount, m_nUsedCount, m_nReserveCount);
	::OutputDebugStringA(buf);
#endif
	m_lstFree.clear();

	for( POOL_OBJ_LIST::iterator itr = m_listForDel.begin() ; itr != m_listForDel.end() ; )
	{
		POOL_OBJ_LIST::value_type pElement = (*itr);
		//		SAFE_DELETE_ARRAY(pElement);
		CreationPolicy<T>::Destroy(pElement);
		//m_listForDel.erase(itr++);
		itr = m_listForDel.erase(itr);
	}

	m_listForDel.clear();
#ifdef _DEBUG
	m_kContTotal.clear();
	m_kContOut.clear();
	m_kContWait.clear();
#endif
	*/
}

template < class T, template <class> class CreationPolicy >
T* TObjectPool< T, CreationPolicy >::New()
{
#ifndef _MDo_
	CPoolObserver::Locked_New();
#endif
	return new T;
/*
	BM::CAutoMutex kLockFree(m_kFreeMutex);

	if( m_lstFree.empty() )
	{
		if(!Grow( m_inc_count) )
		{
			assert(NULL && "TObjectPool< T, CreationPolicy >::New()");
			::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::New()\n"));
			::OutputDebugStringA(typeid(T).name());
			return NULL;
		}
	}

	T* pObj = m_lstFree.front();
	assert(pObj && "TObjectPool< T, CreationPolicy >::New Front is NULL");
	if(!pObj)
	{
		::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::New Front is NULL"));
		::OutputDebugStringA(typeid(T).name());
	}
	m_lstFree.pop_front();
	
	++m_nUsedCount;
	m_nMaxUsedCount = __max(m_nUsedCount, m_nMaxUsedCount);


#ifdef _DEBUG
	POOL_OBJ_HASH::iterator wait_itor = m_kContWait.find(pObj);
	if(wait_itor != m_kContWait.end())
	{
		m_kContWait.erase(wait_itor);
		
		auto out_ret = m_kContOut.insert( std::make_pair(pObj,0));
		if(!out_ret.second)
		{
			assert(NULL && "TObjectPool< T, CreationPolicy >::New Already Out");
			::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::New Already Out\n"));
			::OutputDebugStringA(typeid(T).name());
		}
	}
	else
	{
		assert(NULL && "TObjectPool< T, CreationPolicy >::New that's not waiting");
		::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::New that's not waiting\n"));
		::OutputDebugStringA(typeid(T).name());
	}

#endif

	return pObj;
*/
}

template < class T, template <class> class CreationPolicy >
bool TObjectPool< T, CreationPolicy >::Delete(T *&pObj)
{
#ifndef _MDo_
	CPoolObserver::Locked_Delete();
#endif
	SAFE_DELETE(pObj);

	return true;
/*
	BM::CAutoMutex kLockFree(m_kFreeMutex);


#ifdef _DEBUG
	POOL_OBJ_HASH::iterator out_itor = m_kContOut.find(pObj);
	if(out_itor != m_kContOut.end())//나간거 검색.
	{
		m_kContOut.erase(out_itor);//나간거 지워.
		
		auto wait_ret = m_kContWait.insert( std::make_pair(pObj,0));
		if(!wait_ret.second)
		{
			assert(NULL && "TObjectPool< T, CreationPolicy >::New Already Deleted");
			::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::New Already Deleted\n"));
			::OutputDebugStringA(typeid(T).name());
		}

		POOL_OBJ_HASH::iterator total_itor = m_kContTotal.find(pObj);
		if(total_itor == m_kContTotal.end())
		{
			assert(NULL && "TObjectPool< T, CreationPolicy >::New Not Mine in Total!!");
			::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::New Not Mine in Total!!\n"));
			::OutputDebugStringA(typeid(T).name());
		}
	}
	else
	{
		assert(NULL && "TObjectPool< T, CreationPolicy >::New that's Not Mine!!!");
		::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::New that's Not Mine!!!\n"));
		::OutputDebugStringA(typeid(T).name());
	}
#endif
	assert(pObj && "TObjectPool< T, CreationPolicy >::Delete pOjb == NULL");
	if(!pObj)
	{
		::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::Delete pOjb == NULL\n"));
		::OutputDebugStringA(typeid(T).name());
	}

	m_lstFree.push_front( pObj );
	pObj = NULL;

	--m_nUsedCount;
	assert(m_nUsedCount >= 0 && "TObjectPool< T, CreationPolicy >::Delete use count minus");
	return true;
*/
}

template < class T, template <class> class CreationPolicy >
bool TObjectPool< T, CreationPolicy >::Grow(size_t const count)
{//외부에서 락을 걸고 사용할 것 //추가될 리스트들이 락에 잡혀 오므로 데드락이나 스레드 빠질일 없이 괜찮다.
/*	BM::CAutoMutex kLockFree(m_kFreeMutex);
	
	if( !count ){return false;}

	T* pObj = CreationPolicy<T>::Create(count);
//	T* pObj T_ALLOCATOR::Create(count);

//	T* pObj = new T[count];
	m_nReserveCount += count;

	assert(pObj && _T("TObjectPool< T, CreationPolicy >::Grow Can't new element"));

	if(!pObj)
	{
		::OutputDebugString(_T("TObjectPool< T, CreationPolicy >::Grow Can't new element\n"));
		::OutputDebugStringA(typeid(T).name());
	}

	m_listForDel.push_back( pObj );//지워질 리스트 대상에는 배열의 포인터를 넣는다.

	size_t i = 0;
	while( count != i)
	{
		T* pTemp = (pObj+i);
		m_lstFree.push_back( pTemp );
#ifdef _DEBUG
		m_kContTotal.insert(std::make_pair(pTemp,0));//토탈 등록
		m_kContWait.insert(std::make_pair(pTemp,0));//웨이트 등록
#endif
		++i;
	}
*/
	return true;
}