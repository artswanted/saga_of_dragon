#pragma once

#include "Loki/Threads.h"

#include "BM/stlsupport.h"
#include "BM/ThreadObject.h"
#include "BM/vstring.h"

#ifndef _MDo_
namespace BM
{
	class PgDebugLog;
};

class CPoolObserver;

class CObjectObserver
{
	typedef std::set< CPoolObserver* > CONT_OBSERVERS;
public:
	CObjectObserver(){}
	virtual ~CObjectObserver();
	
public:
	typedef struct tagObjectInfo {
		size_t iUnReleasedCount;
		size_t iMaxUsedCount;
		size_t iMaxReservedCount;

		tagObjectInfo(size_t iUsed, size_t iMax, size_t iReserved)
			: iUnReleasedCount(iUsed), iMaxUsedCount(iMax), iMaxReservedCount(iReserved)
		{
		}

		tagObjectInfo const & operator=(tagObjectInfo const& rhs)
		{
			iUnReleasedCount = rhs.iUnReleasedCount;
			iMaxUsedCount = rhs.iMaxUsedCount;
			iMaxReservedCount = rhs.iMaxReservedCount;

			return *this;
		}

		void Update(tagObjectInfo const& rhs)
		{
			iUnReleasedCount += rhs.iUnReleasedCount;
			iMaxUsedCount = (iMaxUsedCount > rhs.iMaxUsedCount) ? iMaxUsedCount : rhs.iMaxUsedCount;
			iMaxReservedCount =  (iMaxReservedCount > rhs.iMaxReservedCount) ? iMaxReservedCount : rhs.iMaxReservedCount;
		}
	} ObjectInfo;

	typedef std::map< std::string, ObjectInfo > CONT_OBJECTINFO;
	friend CPoolObserver;
	void DisplayState( BM::PgDebugLog &rkLog )const;

protected:
	void RegistObserver(CPoolObserver * const pObj);
	void UnregistObserver(CPoolObserver * const pObj);
	Loki::Mutex & GetLock() { return m_kMutex; }

protected:
	CONT_OBSERVERS m_kContObserver;
	CONT_OBJECTINFO m_kContObjectInfo;	// 삭제된 CPoolBase 에 대한 정보 담기
	mutable Loki::Mutex m_kMutex;
};

extern CObjectObserver g_kObjObserver;//싱글톤 선언하면 디버깅이 불편해서. 나중에는 싱글톤으로 가자.

// g_kObjObserver 의 Mutex 개체를 같이 쓴다.
class CPoolObserver
{
public:
	explicit CPoolObserver(std::string const &kM);
	virtual ~CPoolObserver();

	friend CObjectObserver;

	void Locked_New()
	{
		BM::CAutoMutex kLock(g_kObjObserver.GetLock());
		++m_nUsedCount;
		m_nMaxUsedCount = __max(static_cast<size_t>(m_nUsedCount), m_nMaxUsedCount);
		g_kObjObserver.RegistObserver(this);//등록.
	}

	void Locked_Delete()
	{
		BM::CAutoMutex kLock(g_kObjObserver.GetLock());
		if (--m_nUsedCount == 0)
		{
			g_kObjObserver.UnregistObserver(this);
		}
	}

	void VDisplayState( BM::PgDebugLog &rkLog )const
	{
//		kLog.LogNoArg(BM::LOG_LV1, (BM::vstring)_T("Name=") << UNI(m_kName.c_str()) << (BM::vstring)_T("Now=") << m_nUsedCount <<  (BM::vstring) _T("Max=") << m_nMaxUsedCount << (BM::vstring)_T("Res=") << 0 );
	}

protected:

	std::string const & GetName() { return m_kName; }
	void Registerd(bool const bReg)
	{
		m_bRegistered = bReg;
	}

	size_t	GetUsedCount() const {return m_nUsedCount; }//
	size_t	GetMaxUsedCount() const {return m_nMaxUsedCount;}
protected:

	long m_nUsedCount;
	size_t m_nMaxUsedCount;//최고로 올랐을때 갯수.
	std::string m_kName;
	bool m_bRegistered;
	//mutable Loki::Mutex m_kObMutex;
};
#endif	/* _MDo_ */

namespace BM
{
	template <class T>
	struct CreateArrayUsingNew
	{
	  static T* Create(size_t const count)
	  { 
		  return new T[count]; 
	  }
	  
	  static void Destroy(T* &p)
	  { 
		  delete []p; 
		  p = NULL;
	  }
	};

//#pragma pack(1)
	template < class T, template <class> class CreationPolicy = CreateArrayUsingNew >
	class TObjectPool
#ifndef _MDo_
		: CPoolObserver
#endif
	{
	protected:
		Loki::Mutex m_obj_pool_mutex;
		
		typedef typename T OBJECT;
	
		typedef typename std::list< T* > POOL_OBJ_LIST;

		typedef enum eValue
		{
			V_DEFAULT_INC_SIZE = 100,
		}E_VALUE;

	public:	
		TObjectPool();// inc_count -> 0 으로 하면 더이상 커지지 않는 풀
		TObjectPool(size_t const nInitSize, size_t const nGrowSize = V_DEFAULT_INC_SIZE);// inc_count -> 0 으로 하면 더이상 커지지 않는 풀
		virtual ~TObjectPool(void);

	public:	
		size_t Init(size_t const nInitSize, size_t const nGrowSize);
		void Terminate();
		T*		New();//취득
		bool	Delete(T *&pObj);
		
		size_t	GetFreeSize() const { return m_lstFree.size(); }//

		void swap(TObjectPool<T,CreationPolicy>& rkRight);
	protected:
		bool Grow(size_t const count);
		
	protected:
		POOL_OBJ_LIST m_listForDel;//지워질 애들
		POOL_OBJ_LIST m_lstFree;

		size_t m_inc_count;
		size_t m_nReserveCount;//예약된 개체 갯수.

		Loki::Mutex m_kFreeMutex;
		Loki::Mutex m_kDelMutex;

#ifdef _DEBUG
		typedef typename std::map< T* , int> POOL_OBJ_HASH;
		//만든놈 총괄.
		//나간놈 총괄.
		//대기하는놈 총괄.
		POOL_OBJ_HASH m_kContTotal;//나가기 대기중 //-> 정확하게 내가 만든건지.
		POOL_OBJ_HASH m_kContOut;//나감 //
		POOL_OBJ_HASH m_kContWait;//나감 //
#endif
	};

//#pragma pack()

#include "ObjectPool.inl"
};