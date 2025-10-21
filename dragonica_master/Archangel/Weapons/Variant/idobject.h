//// Data(Savable / Loadable) Object Interface
// Dukguru
//

#ifndef WEAPON_VARIANT_BASIC_IDOBJECT_H
#define WEAPON_VARIANT_BASIC_IDOBJECT_H

#include <map>
#include <tchar.h>

#include "IGObject.h"
#include "BM/Point.h"

typedef struct tagAbil
{
	tagAbil(WORD const wType, int const iValue)
		: m_wType(wType), m_iValue(iValue)
	{}

	WORD const m_wType;
	int	const  m_iValue;
}SAbil;

typedef struct tagSAbilIterator
{
	tagSAbilIterator()
		:	wType(0)
		,	iValue(0)
	{}

	typedef std::map< WORD, int> DYN_ABIL;
	DYN_ABIL::const_iterator itor;
	WORD wType;
	int iValue;
}SAbilIterator;

class CAbilObject
{
public:
	typedef std::map< WORD, int> DYN_ABIL;
	typedef std::map< WORD, __int64> DYN_ABIL64;

public:
	CAbilObject(){}
	virtual ~CAbilObject(){}
public:
	bool IsEmpty()const
	{
		size_t const total_count = m_mapAbils.size() + m_mapAbils64.size();
		if( !total_count )
		{
			return true;
		}
		return false;
	}

	size_t AbilCount()const
	{
		return m_mapAbils.size();
	}
	
	size_t Abil64Count()const
	{
		return m_mapAbils.size();
	}

	size_t TotalAbilCount()const
	{
		return AbilCount()+Abil64Count();
	}

	void operator +=( CAbilObject const &rhs)//합침.
	{
		DYN_ABIL::const_iterator abil_itor = rhs.m_mapAbils.begin();
		while(abil_itor != rhs.m_mapAbils.end() )
		{
			AddAbil((*abil_itor).first, (*abil_itor).second);
			++abil_itor;
		}

		DYN_ABIL64::const_iterator abil64_itor = rhs.m_mapAbils64.begin();
		while(abil64_itor != rhs.m_mapAbils64.end() )
		{
			AddAbil64((*abil64_itor).first, (*abil64_itor).second);
			++abil64_itor;
		}

		return;
	}

	void operator -=( CAbilObject const &rhs)//뺌
	{
		DYN_ABIL::const_iterator abil_itor = rhs.m_mapAbils.begin();
		while(abil_itor != rhs.m_mapAbils.end() )
		{
			AddAbil((*abil_itor).first, -(*abil_itor).second);
			++abil_itor;
		}

		DYN_ABIL64::const_iterator abil64_itor = rhs.m_mapAbils64.begin();
		while(abil64_itor != rhs.m_mapAbils64.end() )
		{
			AddAbil64((*abil64_itor).first, -(*abil64_itor).second);
			++abil64_itor;
		}

		return;
	}

	void operator *= ( int const iValue)
	{
		DYN_ABIL::iterator abil_itor = m_mapAbils.begin();
		while(abil_itor != m_mapAbils.end() )
		{
			(*abil_itor).second *= iValue;
			++abil_itor;
		}

		DYN_ABIL64::iterator abil64_itor = m_mapAbils64.begin();
		while(abil64_itor != m_mapAbils64.end() )
		{
			(*abil64_itor).second *= iValue;
			++abil64_itor;
		}
	}

	void operator /= ( int const iValue)
	{
		DYN_ABIL::iterator abil_itor = m_mapAbils.begin();
		while(abil_itor != m_mapAbils.end() )
		{
			(*abil_itor).second /= iValue;
			++abil_itor;
		}

		DYN_ABIL64::iterator abil64_itor = m_mapAbils64.begin();
		while(abil64_itor != m_mapAbils64.end() )
		{
			(*abil64_itor).second /= iValue;
			++abil64_itor;
		}
	}

	virtual int GetAbil(WORD const wAbilType) const
	{
		return GetAbil<int>(m_mapAbils, wAbilType);
	}

	virtual __int64 GetAbil64(WORD const wAbilType) const
	{
		return GetAbil<__int64>(m_mapAbils64, wAbilType);
	}

	virtual bool AddAbil(WORD const Type, int const iValue, bool const bZeroAutoDel = false )
	{
		return AddAbil(m_mapAbils, Type, iValue, bZeroAutoDel);
	}

	virtual bool AddAbil64(WORD const Type, __int64 const iValue, bool const bZeroAutoDel = false )
	{
		return AddAbil(m_mapAbils64, Type, iValue, bZeroAutoDel);
	}
	
	virtual bool SetAbil(WORD const Type, int const iValue)
	{
		return SetAbil(m_mapAbils, Type, iValue);
	}

	virtual bool SetAbil64(WORD const Type, __int64 const iValue)
	{
		return SetAbil(m_mapAbils64, Type, iValue);
	}

	void EraseAbil(WORD const Type)
	{
		m_mapAbils.erase( Type );
	}

	void EraseAbil64(WORD const Type)
	{
		m_mapAbils64.erase( Type );
	}

	virtual void FirstAbil(SAbilIterator* pkIter) const
	{
		if( !pkIter )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"pkIter is NULL");
			return;
		}
		pkIter->itor = m_mapAbils.begin();
	}

	virtual bool NextAbil(SAbilIterator* pkIter) const
	{
		if( !pkIter )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"pkIter is NULL");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		
		if (pkIter->itor == m_mapAbils.end())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		pkIter->wType = pkIter->itor->first;
		pkIter->iValue = pkIter->itor->second;

		pkIter->itor++;
		return true;
	}

	void Clear()
	{
		m_mapAbils.clear();
		m_mapAbils64.clear();
	}

	void Swap( CAbilObject & kAbilObject )
	{
		m_mapAbils.swap( kAbilObject.m_mapAbils );
		m_mapAbils64.swap( kAbilObject.m_mapAbils64 );
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(m_mapAbils);
		rkPacket.Push(m_mapAbils64);		
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		m_mapAbils.clear();
		m_mapAbils64.clear();

		rkPacket.Pop(m_mapAbils);
		rkPacket.Pop(m_mapAbils64);
	}

	bool HasAbil(WORD const wType) const
	{
		return HasAbil(m_mapAbils, wType);
	}

	bool HasAbil64(WORD const wType) const
	{
		return HasAbil(m_mapAbils64, wType);
	}

protected:
	template<typename T_CONT>
	bool HasAbil(T_CONT &kCont, WORD const wType) const
	{
		typename T_CONT::const_iterator iter = kCont.find(wType);
		if (iter != kCont.end())
		{
			return true;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	template<typename T_CONT, typename T_VALUE>
	bool SetAbil(T_CONT &kCont, WORD const Type, const T_VALUE tValue)
	{
		typename T_CONT::iterator iter = kCont.find(Type);
		if (iter != kCont.end())
		{
			if((*iter).second != tValue)
			{
				// 값이 0 이라고 erase 하지 않도록 한다.
				// Abil 계산 함수 쪽에서, GetAbil()했을 때, 이값이 정말로 0인지, 아니면 없어서 0으로 
				// return 했는지 구별해야 할 필요가 있다.
				//if(0 != tValue)
				{
					(*iter).second = tValue;
				}
				//else
				//{
				//	kCont.erase(Type);
				//}
				return true;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		kCont.insert(std::make_pair( Type, tValue));
		return true;
	}

	template<typename T_VALUE, typename T_CONT>
	T_VALUE GetAbil(T_CONT &kCont, WORD const wAbilType)const
	{
		typename T_CONT::const_iterator iter = kCont.find(wAbilType);
		if( iter != kCont.end() )
		{
			return (*iter).second;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	
	template<typename T_CONT, typename T_VALUE>
	bool AddAbil(T_CONT &kCont, WORD const Type, T_VALUE const tValue, bool const bZeroAutoDel )
	{
		if(!tValue)
		{
			return true;
		}

		typename auto ret = kCont.insert( std::make_pair(Type, tValue) );
		
		if( !ret.second )
		{//넣기 실패라면 해당 itor에 합산해넣으라.
			(ret.first)->second += tValue;
			if (	( 0 == ret.first->second )
				&&	( true == bZeroAutoDel ) 
			)
			{
				kCont.erase( ret.first );
			}
		}
		return true;
	}

protected:
	DYN_ABIL m_mapAbils;//동적 능력치. Def 계열이 아님
	DYN_ABIL64 m_mapAbils64;//동적 능력치. Def 계열이 아님
};

class IDObject
	:	public IGObject
	,	public CAbilObject
{
	typedef enum eValue
	{
		MAX_NAME = 21,
		MAX_NICK = 21,
	}E_VALUE;

public:
	IDObject(){}
	virtual ~IDObject(){}

public:
	BM::GUID const &GetID()const{return m_kGuid;}
	void SetID(BM::GUID const &guid){m_kGuid = guid;}

	virtual POINT3 const & GetPos() const{ return m_Pos; }
	virtual void SetPos(POINT3 const &ptNew)
	{ 
		m_Pos = ptNew;
	}

	virtual void Clear()
	{
		m_kGuid.Clear();
		m_Pos.Clear();
	}

protected:
	BM::GUID m_kGuid;
	POINT3 m_Pos;
};

#endif // WEAPON_VARIANT_BASIC_IDOBJECT_H