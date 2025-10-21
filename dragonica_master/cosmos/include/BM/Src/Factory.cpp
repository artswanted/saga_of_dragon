#include "stdafx.h"
/*
#include "factory.h"
CFactory::CFactory(void)
{
	m_mapElement.clear();
}

CFactory::~CFactory(void)
{
	Clear();
}

void CFactory::Init()
{
	m_mapElement.clear();
}

void CFactory::Clear()
{
	ElementIter Itor = m_mapElement.begin();
	while(Itor != m_mapElement.end())
	{
		CFactoryObject* pBO = (*Itor).second;
		m_mapElement.erase(Itor++);
		this->Destroy(&pBO);
	}
	m_mapElement.clear();
}

void CFactory::Destroy(CFactoryObject **ppBaseObject)
{
	(*ppBaseObject)->VRelease();
	(*ppBaseObject) = NULL;
}
*/