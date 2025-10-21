#pragma once
/*
#include "BM/STLSupport.h"

class CFactoryObject
{
public:
	CFactoryObject(){};
	virtual ~CFactoryObject(){};
public:
	virtual void VRelease() = 0;//릴리즈를 담당한다. 죽을때 소멸해야 하는 어떤것이 있으면 상속받아 재작성 요망.
	//{delete this;}를 하던가.. 여튼 스스로 죽어 줘야 한다.
};

template<typename T1>
class CCreator
{
	friend class CFactory;
protected:
	CCreator(){}
	~CCreator(){}

protected:
	virtual T1* VCreate()//스탠다드 이므로 상속받아 특별 처리 할게있으면 하면 됨.
	{
		T1* pT1 = new T1;
		if(!pT1)
		{
			assert( pT1 && "Can't Create Object");
			return NULL;
		}
		return pT1;
	}
};

class CFactory
//	: public CSingleton
{
public:
	CFactory(void);
	~CFactory(void);

	void Init();
	void Clear();
	
	template<typename T1>
	bool Create(T1 **ppT1);
	
	void Destroy(CFactoryObject **ppBaseObject);

private:
	typedef std::map<CFactoryObject*, CFactoryObject*> ElementHash;
	typedef ElementHash::iterator ElementIter;
	ElementHash	m_mapElement;
};

template<typename T1>
bool CFactory::Create(T1 **ppT1)
{//
	CCreator<T1> Creator;//크리에이터를 바꾸면 다른 방식으로 생성하기도 가능.

	(*ppT1) = Creator.VCreate();

	if(!(*ppT1))
	{
		assert( (*ppT1) && "Can't Create Object");
		return false;
	}

	ElementIter iter = m_mapElement.find((CFactoryObject*)(*ppT1)); 
	if (m_mapElement.end() != iter)
	{
		assert(NULL && "Already Created Element");
		return false;
	}

	m_mapElement.insert( make_pair((*ppT1), (*ppT1)) );
	return true;
}
*/