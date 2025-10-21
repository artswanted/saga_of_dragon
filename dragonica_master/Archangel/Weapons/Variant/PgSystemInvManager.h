#ifndef WEAPON_VARIANT_USERINFO_PGSYSTEMINVMANAGER_H
#define WEAPON_VARIANT_USERINFO_PGSYSTEMINVMANAGER_H

#include "item.h"

typedef struct tagSySSimpleItemInfo
{
	DWORD	dwDeleteTime;
	PgBase_Item kSysItem;
	
	tagSySSimpleItemInfo()
	{
		dwDeleteTime = 0;
	}
	tagSySSimpleItemInfo(PgBase_Item const & kItem)
	{
		dwDeleteTime = BM::GetTime32() + SYSTEM_INVENTORY_TIME_LIMIT_PLUSE;
		kSysItem = kItem;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(dwDeleteTime);
		kSysItem.WriteToPacket(kPacket);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(dwDeleteTime);
		kSysItem.ReadFromPacket(kPacket);
	}
}SSYSSimpleItemInfo;

typedef struct tagCONT_SYS_ITEM : public std::map<BM::GUID,SSYSSimpleItemInfo>
{
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(size());
		for(tagCONT_SYS_ITEM::const_iterator iter = begin();iter != end();++iter)
		{
			(*iter).second.WriteToPacket(kPacket);
		}
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		tagCONT_SYS_ITEM::size_type kCount;
		tagCONT_SYS_ITEM::mapped_type kData;
		kPacket.Pop(kCount);
		for(tagCONT_SYS_ITEM::size_type i = 0;i < kCount;i++)
		{
			kData.ReadFromPacket(kPacket);
			insert(std::make_pair(kData.kSysItem.Guid(),kData));
		}
	}
}CONT_SYS_ITEM;

typedef std::map<BM::GUID,CONT_SYS_ITEM> CONT_CONT_SYS_ITEM;

class PgSystemInvManager
{
public:
	
	PgSystemInvManager(){}
	~PgSystemInvManager(){}

	void ClearTimeOutSysItem(BM::GUID const & kCharGuid);
	bool RemoveSysItem(BM::GUID const & kCharGuid,BM::GUID const & kGuid);
	PgBase_Item GetSysItem(BM::GUID const & kCharGuid, BM::GUID const & kGuid);
	HRESULT AddSysItem(BM::GUID const & kCharGuid, PgBase_Item const & kItem);

protected:

	CONT_CONT_SYS_ITEM	m_kContContSysItem;
	Loki::Mutex			m_kMutex;
};

#define g_kSysInvMgr SINGLETON_STATIC(PgSystemInvManager)

#endif // WEAPON_VARIANT_USERINFO_PGSYSTEMINVMANAGER_H