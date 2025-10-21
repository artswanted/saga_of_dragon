#pragma once

class PgItem;

using namespace NiManagedToolInterface;

class PgItemMan
{
public:
	typedef std::multimap<std::string, PgItem *> ItemContainer;
	typedef std::map<int, PgItem *> AttachedItemContainer;

	PgItemMan();
	~PgItemMan();

	void AddItem(PgItem *pkItem, bool bForce = false);
	void RemoveItem(const char *pcPath);
	PgItem *GetItem(const char *pcPath, int iGender, int iClass);
	void ClearAllItem();

	bool RegisterAttachedItem(PgItem *pkItem);
	bool UnregisterAttachedItem(PgItem *pkItem);
	PgItem *GetAttachedItem(int iItemPos);

private:
	int m_iItemCount;

	ItemContainer m_kItemContainer;
	AttachedItemContainer m_kAttachedItemContainer;
};