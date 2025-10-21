#include "stdafx.h"
#include "PgItemMan.h"
#include "PgItem.h"
#include "PgAMPool.h"
#include "PgNifMan.h"

PgItemMan::PgItemMan() :
	m_iItemCount(0)
{
}

PgItemMan::~PgItemMan()
{
	ClearAllItem();
	g_kAMPool.Destroy();
	g_kNifMan.Destroy();
}

//PgItem *PgItemMan::AddItem(const char *pcPath)
//{
//	PgItem *pkItem = PgItem::CreateItemFromXmlFile(pcPath);
//	if(!pkItem)
//	{
//		return 0;
//	}
//
//	AddItem(pkItem);
//	return pkItem;
//}

void PgItemMan::AddItem(PgItem *pkItem, bool bForce)
{
	const char *pcPath = pkItem->GetPath();
	if(!pcPath)
	{
		return;
	}

	bool bFindItem = false;
	ItemContainer::iterator itr;
	for(itr = m_kItemContainer.lower_bound(pcPath);
		itr != m_kItemContainer.upper_bound(pcPath);
		++itr)
	{
		PgItem *pkStoredItem = itr->second;
		if(pkStoredItem->GetGender() == pkItem->GetGender() &&
			pkStoredItem->GetClass() == pkItem->GetClass())
		{
			bFindItem = true;
			break;
		}
	}

	// Class, Gender가 똑같은 아이템이 없거나
	if(bForce && bFindItem)
	{
		NiExternalDelete itr->second;
		itr->second = pkItem;
	}
	else if(!bFindItem)
	{
		m_kItemContainer.insert(std::make_pair(pcPath, pkItem));
		pkItem->IncRefCount();
		++m_iItemCount;
	}
}

void PgItemMan::RemoveItem(const char *pcPath)
{
	// 지울 때는 Class, Gender에 해당하는 모든 아이템을 다 지운다.
	if(!pcPath)
	{
		return;
	}

	std::string kPath(pcPath);
	for(ItemContainer::iterator itr = m_kItemContainer.lower_bound(kPath);
		itr != m_kItemContainer.upper_bound(kPath);
		++itr)
	{
		PgItem *pkStoredItem = itr->second;
		pkStoredItem->DecRefCount();
		if(pkStoredItem->GetRefCount() == 0)
		{
			NiDelete itr->second;
		}

		itr = m_kItemContainer.erase(itr);
		--m_iItemCount;
	}
}

PgItem *PgItemMan::GetItem(const char *pcPath, int iGender, int iClass)
{
	if(!pcPath)
	{
		return 0;
	}

	std::string kPath(pcPath);
	for(ItemContainer::iterator itr = m_kItemContainer.lower_bound(kPath);
		itr != m_kItemContainer.upper_bound(kPath);
		++itr)
	{
		PgItem *pkStoredItem = itr->second;
		if(pkStoredItem->GetGender() == iGender &&
			pkStoredItem->GetClass() == iClass)
		{
			return itr->second;
		}
	}

	return 0;
}

void PgItemMan::ClearAllItem()
{
	ItemContainer::iterator itr = m_kItemContainer.begin();
	while(itr != m_kItemContainer.end())
	{
		NiDelete itr->second;
		++itr;
	}

	m_iItemCount = 0;
	m_kItemContainer.clear();
	m_kAttachedItemContainer.clear();
}

bool PgItemMan::RegisterAttachedItem(PgItem *pkItem)
{
	AttachedItemContainer::iterator itr = m_kAttachedItemContainer.find(pkItem->ItemPos());
	if(itr != m_kAttachedItemContainer.end())
	{
		return false;
	}
	m_kAttachedItemContainer.insert(std::make_pair(pkItem->ItemPos(), pkItem));
	pkItem->SetAttached(true);

	return true;
}

bool PgItemMan::UnregisterAttachedItem(PgItem *pkItem)
{
	AttachedItemContainer::iterator itr = m_kAttachedItemContainer.find(pkItem->ItemPos());
	if(itr == m_kAttachedItemContainer.end())
	{
		pkItem->SetAttached(false);	//붙어있지 않은거니까 떨어진거다
		return false;
	}
	m_kAttachedItemContainer.erase(itr);
	pkItem->SetAttached(false);	

	return true;
}

PgItem *PgItemMan::GetAttachedItem(int iItemPos)
{
	AttachedItemContainer::iterator itr = m_kAttachedItemContainer.find(iItemPos);
	if(itr == m_kAttachedItemContainer.end())
	{
		return 0;
	}

	return itr->second;
}