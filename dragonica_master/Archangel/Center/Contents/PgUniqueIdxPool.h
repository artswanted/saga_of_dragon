#ifndef CENTER_CENTERSERVER_CONTENTS_PGUNIQUEIDXPOOL_H
#define CENTER_CENTERSERVER_CONTENTS_PGUNIQUEIDXPOOL_H

#include <map>

#include "BM/GUID.h"
//#include "BM/MAP.h"
#include "BM/ClassSupport.h"
#include "BM/ObjectPool.h"
#include "Loki/singleton.h"

//	<simple pool using by unique-index>
//	       * made by radio_crew(another call simpson)
//
//
//   [example]
//
//   0  1  2  3  4  5  6  7  8  9   Ўз Array Index
//   Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй   
//	[2][3][6][1][7][5][Ўа][Ўа][Ўа][Ўа]  Ўз All using element have a unique-index
//
// 
//  [n] - Used memory block(have a value)
//  [Ўа] - Used memory block(no value)
//
//  m_kEndPoint  = 6 (using array index)
//  iReserveSiz  = 10
//  MAX_POOL_SIZ = iReserveSiz


//   Push_Front [9]
//
//   0  1  2  3  4  5  6  7  8  9   Ўз Array Index
//   Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй   
//	[9][2][3][6][1][7][5][Ўа][Ўа][Ўа]  Ўз All using element have a unique-index
//
//  m_kEndPoint  = 7 (using array index)
//  iReserveSiz  = 10
//  MAX_POOL_SIZ = iReserveSiz



//   Push_Back [4]
//
//   0  1  2  3  4  5  6  7  8  9   Ўз Array Index
//   Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй   
//	[2][3][6][1][7][5][4][Ўа][Ўа][Ўа]  Ўз All using element have a unique-index
//
//  m_kEndPoint  = 7 (using array index)
//  iReserveSiz  = 10
//  MAX_POOL_SIZ = iReserveSiz

//   Erase [6]
//
//   0  1  2  3  4  5  6  7  8  9   Ўз Array Index
//   Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй  Ўй   
//	[2][3][1][7][5][0][0][Ўа][Ўа][Ўа]  Ўз All using element have a unique-index
//
//  m_kEndPoint  = 5 (using array index)
//  iReserveSiz  = 10
//  MAX_POOL_SIZ = iReserveSiz

//
//	Notice!!
//  - Special things.
//
//  1. Continuity memoyblock(have a value) status, always
//	   - User can control that new value located first or last always
//
//  2. all element have a unique-index (unique-index is not array-index)
//     unique-idx coming from UniqIdxPool
// 

template< typename T_IDX, typename T_ELEMENT >
class PgUniqueIdxPool
{
	typedef typename std::map< T_IDX, int > UniqIdxPool;// first key : unique index, second : not available
	typedef typename std::vector< T_ELEMENT > ElementList;

public:
	PgUniqueIdxPool(int const iReserveSiz, const T_IDX& rDefaultErrIdx)
		:	m_kDefaultErrIdx(rDefaultErrIdx)
		,	MAX_POOL_SIZ(iReserveSiz)					// element count
		,	m_pkPool(new_tr T_ELEMENT[iReserveSiz])
		,	m_kEndPoint(0)
	{
		Init();
	}

	~PgUniqueIdxPool()
	{
		m_kIdxPool.clear();;						
		m_kEndPoint = 0;			

		if(m_pkPool)
		{ 
			delete[] m_pkPool;
			m_pkPool = NULL;
		}
	}

	T_IDX Push_Front(const T_ELEMENT& rkElement)
	{
		BM::CAutoMutex kLock(m_kMutex);
	
		const T_IDX retIdx = PopIdx();
		if(m_kDefaultErrIdx == retIdx)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << m_kDefaultErrIdx);
			return m_kDefaultErrIdx;
		}
	
		rkElement->Idx(retIdx);// set the unique index on element

		::memcpy(m_pkPool + 1, m_pkPool, sizeof(T_ELEMENT) * (MAX_POOL_SIZ - 1));
		::memset(m_pkPool, 0, sizeof(T_ELEMENT));
		::memcpy(m_pkPool, &rkElement, sizeof(T_ELEMENT));

		++m_kEndPoint;

		return retIdx;
	}

	T_IDX Push_Back(const T_ELEMENT& rkElement)
	{
		BM::CAutoMutex kLock(m_kMutex);
	
		if(MAX_POOL_SIZ <= m_kEndPoint)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << m_kDefaultErrIdx);
			return m_kDefaultErrIdx;
		}

		const T_IDX retIdx = PopIdx();
		if(m_kDefaultErrIdx == retIdx)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << m_kDefaultErrIdx);
			return m_kDefaultErrIdx;
		}
		
		rkElement->Idx(retIdx);// set the unique index on element
		::memcpy(&m_pkPool[m_kEndPoint], &rkElement, sizeof(T_ELEMENT));

		++m_kEndPoint;

		return retIdx;
	}

	void Erase(const T_ELEMENT& rkElement)
	{
		size_t idx = 0;
		for(idx = 0; idx < m_kEndPoint; idx++)
		{
			if(rkElement == m_pkPool[idx])
			{
				PushIdx(m_pkPool[idx]->Idx());// »зїлЗЯґш unique-idx ё¦ №ЭИЇЗСґЩ
				::memset(m_pkPool[idx], 0, sizeof(T_ELEMENT));
				break;
			}
		}
			
		// ordering
		memcpy(&m_pkPool[idx], &m_pkPool[idx + 1], sizeof(T_ELEMENT) * (MAX_POOL_SIZ - (idx + 1)));
		--m_kEndPoint;
	}

	void Erase(const T_IDX& rkIdx)
	{		
		BM::CAutoMutex kLock(m_kMutex);

		if((0 > rkIdx) || (MAX_POOL_SIZ <= rkIdx))
		{
			return;
		}

		for(size_t idx = 0; idx < m_kEndPoint; idx++)
		{
			if(rkIdx == m_pkPool[idx]->Idx())
			{
				PushIdx(m_pkPool[idx]->Idx());// »зїлЗЯґш unique-idx ё¦ №ЭИЇЗСґЩ
				::memset(m_pkPool[idx], 0, sizeof(T_ELEMENT));
				break;
			}
		}

		// ordering
		memcpy(&m_pkPool[rkIdx], &m_pkPool[rkIdx + 1], sizeof(T_ELEMENT) * (MAX_POOL_SIZ - (rkIdx + 1)));
		--m_kEndPoint;
	}

	void Pop_List(ElementList& rkOutList)
	{
		BM::CAutoMutex kLock(m_kMutex);
		
		rkOutList.clear();
		for(size_t idx = 0; idx < m_kEndPoint; idx++)
		{
			rkOutList.push_back(m_pkPool[idx]);
		}
	}

	size_t Size()
	{
		BM::CAutoMutex kLock(m_kMutex);		
		return m_kEndPoint;
	}

	const T_ELEMENT Find(BM::GUID const &rkGuid)
	{
		BM::CAutoMutex kLock(m_kMutex);		

		for(size_t idx = 0; idx < m_kEndPoint; idx++)
		{
			if(rkGuid == m_pkPool[idx]->Guid())
			{
				return m_pkPool[idx];
			}
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	const T_ELEMENT Find(const T_IDX& rkInIdx)
	{
		BM::CAutoMutex kLock(m_kMutex);		
		
		for(size_t idx = 0; idx < m_kEndPoint; idx++)
		{
			if(rkInIdx == m_pkPool[idx]->Idx())
			{
				return m_pkPool[idx];
			}
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	const T_ELEMENT FindByArrIdx(size_t const nIdx)
	{
		BM::CAutoMutex kLock(m_kMutex);
		
		if((0 > nIdx) || (nIdx >= MAX_POOL_SIZ))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
			return NULL;
		}
		return m_pkPool[nIdx];
	}

protected:
	void Init()
	{
		BM::CAutoMutex kLock(m_kMutex);

		for(T_IDX t = 1; t <= MAX_POOL_SIZ; t++)
		{
			m_kIdxPool.insert(std::make_pair(t, 0));	
		}

		::memset(m_pkPool, 0, sizeof(T_ELEMENT) * MAX_POOL_SIZ);
	}

	bool const PushIdx(const T_IDX& rkIdx)	
	{
		BM::CAutoMutex kLock(m_kMutex);

		if(MAX_POOL_SIZ <= m_kIdxPool.size())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		m_kIdxPool.insert(std::make_pair(rkIdx, 0));
		return true;
	}

	const T_IDX PopIdx()
	{
		BM::CAutoMutex kLock(m_kMutex);
		
		if(m_kIdxPool.empty())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << m_kDefaultErrIdx);
			return m_kDefaultErrIdx;
		}
		UniqIdxPool::iterator itor = m_kIdxPool.begin();
		UniqIdxPool::key_type kIdx = (*itor).first;
		m_kIdxPool.erase(itor);
		return kIdx;
	}

	Loki::Mutex m_kMutex;		// Auto mutex

	T_IDX m_kDefaultErrIdx;		// АЯёшµИ АОµ¦Ѕє№шИЈ
	size_t const MAX_POOL_SIZ;	// ёЮёрё®З® »зАМБо
	UniqIdxPool m_kIdxPool;		// Unique АОµ¦ЅєЗ®
	T_ELEMENT* m_pkPool;		// ёЮёрё®З® ЖчАОЕН
	size_t m_kEndPoint;			// ёЮёрё®З® ё¶Бцё· А§ДЎ
};

#endif // CENTER_CENTERSERVER_CONTENTS_PGUNIQUEIDXPOOL_H