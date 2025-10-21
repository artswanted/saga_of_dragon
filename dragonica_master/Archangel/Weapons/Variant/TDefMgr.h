#ifndef WEAPON_VARIANT_MISSION_TDEFMGR_H
#define WEAPON_VARIANT_MISSION_TDEFMGR_H

#include <map>

#include "BM/ObjectPool.h"

template < typename T_TB_BASE, typename T_TB_ABIL, typename T_DEF , typename T_KEY, typename T_ABIL_KEY >
class TDefMgr
{
public:
	typedef typename T_DEF DEF;
	typedef typename T_TB_BASE TB_BASE;
	typedef typename T_TB_ABIL TB_ABIL;

	typedef TDefMgr< T_TB_BASE, T_TB_ABIL, T_DEF , T_KEY, T_ABIL_KEY > T_MY_BASE_TYPE;
	typedef typename std::map<T_KEY, T_DEF* > CONT_DEF;
	typedef typename std::map<T_KEY, T_TB_BASE > CONT_BASE;
	typedef typename std::map<T_ABIL_KEY, T_TB_ABIL > CONT_ABIL;

private:
//	friend struct ::Loki::CreateStatic< CSkillDefMgr >;

public:
	TDefMgr():m_DefPool(100){}//m_DefPool
	virtual ~TDefMgr(){}

	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil) = 0;
	virtual bool DataCheck()const{return true;}//=0

	virtual void Clear()
	{
		CONT_DEF::iterator erase_iter = m_contDef.begin();
		while( m_contDef.end() != erase_iter )
		{
			m_DefPool.Delete( (*erase_iter).second );
			++erase_iter;
		}
		m_contDef.clear();
		m_DefPool.Terminate();
	}

	virtual void swap(TDefMgr< T_TB_BASE, T_TB_ABIL, T_DEF, T_KEY, T_ABIL_KEY > &rkRight)
	{
		m_contDef.swap(rkRight.m_contDef);
		m_DefPool.swap(rkRight.m_DefPool);
	}

public:
	T_DEF const* GetDef(const T_KEY &rkKey)const
	{
		CONT_DEF::const_iterator itor = m_contDef.find(rkKey);
		if(itor != m_contDef.end())
		{
			return (*itor).second;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	void GetCont(CONT_DEF &contDef)const//컨테이너 복제해서 받아옴.
	{
		contDef = m_contDef;
	}

	int RefreshScripts();
protected:
	virtual T_DEF* NewDef(T_DEF** pkDef=NULL)
	{
		if (!pkDef)
		{
			return m_DefPool.New();
		}
		else if ( !(*pkDef) )
		{
			*pkDef = m_DefPool.New();
		}
		return *pkDef;
	}
	virtual bool DeleteDef(T_DEF*& pkDef )
	{
		if ( pkDef )
		{
			return m_DefPool.Delete(pkDef);
		}
		return true;
	}

	CONT_DEF m_contDef;//실제 담김.

	BM::TObjectPool< T_DEF > m_DefPool;
};

#endif // WEAPON_VARIANT_MISSION_TDEFMGR_H