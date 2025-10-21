#ifndef WEAPON_LOHENGRIN_UTILITY_LOCKUTIL_H
#define WEAPON_LOHENGRIN_UTILITY_LOCKUTIL_H

class CObjLockWrapper
{
public:
	typedef enum
	{
		ELockType_None = 0,
		ELockType_Loki = 1,
		ELockType_AceRw = 2,
		ELockType_AceToken = 3,
		ELockType_AceRw_Ext = 4,
	} ELockObjectType;

	CObjLockWrapper()
		: m_eType(ELockType_None), m_pkLoki(NULL), m_pkAceRw(NULL), m_pkAceToken(NULL), m_pkAceRwExt(NULL), m_bWrite(false)
	{};

	~CObjLockWrapper()
	{
		Release();
	}

	void Set(Loki::Mutex * pkLock) 
	{
		Release();
		m_pkLoki = pkLock;
		if (m_pkLoki != NULL)
		{
			m_eType = ELockType_Loki;
			m_pkLoki->Lock();
		}
	}

	void Set(ACE_RW_Thread_Mutex * pkLock, bool bWriteLock = false)
	{
		Release();
		m_pkAceRw = pkLock;
		if (m_pkAceRw!= NULL)
		{
			m_eType = ELockType_AceRw;
			m_bWrite = bWriteLock;
			if (bWriteLock)
			{
				m_pkAceRw->acquire_write();
			}
			else
			{
				m_pkAceRw->acquire_read();
			}
		}
	}

	void Set(BM::ACE_RW_Thread_Mutex_Ext * pkLock, bool bWriteLock = false)
	{
		Release();
		m_pkAceRwExt = pkLock;
		if (m_pkAceRwExt != NULL)
		{
			m_eType = ELockType_AceRw_Ext;
			m_bWrite = bWriteLock;
			if (bWriteLock)
			{
				m_pkAceRwExt->acquire_write();
			}
			else
			{
				m_pkAceRwExt->acquire_read();
			}
		}
	}

	void Set(ACE_Token * pkLock, bool bWriteLock = false)
	{
		Release();
		m_pkAceToken = pkLock;
		if (m_pkAceToken != NULL)
		{
			m_eType = ELockType_AceToken;
			m_bWrite = bWriteLock;
			if (bWriteLock)
			{
				m_pkAceToken->acquire_write();
			}
			else
			{
				m_pkAceToken->acquire_read();
			}
		}
	}

	bool Locked() { (m_eType != ELockType_None) ? true : false; };

protected:
	void Release()
	{
		switch(m_eType)
		{
		case ELockType_Loki:
			{
				m_pkLoki->Unlock();
				m_pkLoki = NULL;
			}break;
		case ELockType_AceRw:
			{
				m_pkAceRw->release();
				m_pkAceRw = NULL;
			}break;
		case ELockType_AceRw_Ext:
			{
				if (m_bWrite)
				{
					m_pkAceRwExt->release_write();
				}
				else
				{
					m_pkAceRwExt->release_read();
				}
				m_pkAceRwExt = NULL;
			}break;
		case ELockType_AceToken:
			{
				m_pkAceToken->release();
				m_pkAceToken = NULL;
			}break;
		}
		m_eType = ELockType_None;
	}

private:
	ELockObjectType m_eType;
	Loki::Mutex * m_pkLoki;
	ACE_RW_Thread_Mutex * m_pkAceRw;
	ACE_Token* m_pkAceToken;
	BM::ACE_RW_Thread_Mutex_Ext* m_pkAceRwExt;
	bool m_bWrite;
};

template<typename T_OBJ, typename T_LOCK>
class CObjLock
{
	typename typedef T_OBJ OBJECT;
public:
	CObjLock() 
		: m_pkObj(NULL)
	{};

	~CObjLock()	{};

	void SetLock(T_LOCK* pkLock)
	{
		m_kLock.Set(pkLock);
	}

	void SetObj(OBJECT const* pkObj)
	{
		// *********************************
		// 주의 :  SetLock을 먼저 호출하고 SetObj() 호출해야 함.
		m_pkObj = pkObj;
	}

	operator OBJECT const* () const { return m_pkObj; }
	OBJECT const* Get() { return m_pkObj; }


private:
	OBJECT const* m_pkObj;
	CObjLockWrapper m_kLock;

	// 사용 금지
	CObjLock(CObjLock const& rhs);
	CObjLock const& operator=(CObjLock const& rhs);
};

#endif // WEAPON_LOHENGRIN_UTILITY_LOCKUTIL_H