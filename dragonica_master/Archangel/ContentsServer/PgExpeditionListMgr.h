#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGEXPEDITIONLISTMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGEXPEDITIONLISTMGR_H

// 원정대 채널에서 생성된 원정대 리스트를 다른 채널에서 볼 수 있게 해주고,
// 해당 리스트를 통해 원정대로 가입할 수 있도록 관리하는 매니저.
// 다른 채널에서 가입을 하게 되면 채널 이동을 하여 가입할 수 있도록 가입 요청자 리스트도 관리.

#include <map>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "BM/PgTask.h"
#include "Doc/PgDoc_Player.h"

typedef struct tagExpeditionList : public SExpeditionListInfo
{
	SExpeditionOption Option;

	tagExpeditionList()
	{
	}

	tagExpeditionList(
		BM::GUID const & Expedition, BM::GUID const & Master, 
		std::wstring const & Name, int Map, char CurMember, char MaxMember, SExpeditionOption const & ExpeditionOption)
		: SExpeditionListInfo(Expedition, Master, Name, Map, CurMember, MaxMember),
		  Option(ExpeditionOption)
	{
	}

	tagExpeditionList(tagExpeditionList const & rhs)
		: SExpeditionListInfo(rhs),
		  Option(rhs.Option)
	{
	}

	void Clear()
	{
		SExpeditionListInfo::Clear();
		Option.Clear();
	}

	bool operator == (tagExpeditionList const & rhs)
	{
		return (ExpeditionGuid == rhs.ExpeditionGuid);
	}

	void operator = (tagExpeditionList const & rhs)
	{
		SExpeditionListInfo::operator=(rhs);
		Option = rhs.Option;
	}

	size_t min_size() const
	{
		return SExpeditionListInfo::min_size() + sizeof(Option);
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		SExpeditionListInfo::ReadFromPacket(Packet);
		Option.ReadFromPacket(Packet);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		SExpeditionListInfo::WriteToPacket(Packet);
		Option.WriteToPacket(Packet);
	}
}SExpeditionList;

class PgExpeditionListMgr
{

public:

	typedef BM::TObjectPool< SExpeditionList > ExpeditionListPool;
	typedef std::map< BM::GUID, SExpeditionList* > ExpeditionListHash;
	typedef struct tagFunctionForEach
	{
		tagFunctionForEach(BM::Stream & Packet, BM::Stream & SendPacket)
			: m_Packet(Packet), m_SendPacket(SendPacket) { }
		virtual void operator () (ExpeditionListHash const & ContExpeditionList);

		BM::Stream & m_Packet;
		BM::Stream & m_SendPacket;
	}SFunctionForEach;

public:
	
	PgExpeditionListMgr();
	virtual ~PgExpeditionListMgr();

	bool ProcessMsg(BM::Stream & Packet);
	
protected:

	bool CreateExpeditionList(SExpeditionList const & ListInfo);
	bool DeleteExpeditionList(BM::GUID const & ExpeditionGuid);

	bool JoinExpeditionList(BM::GUID const & ExpeditionGuid, BM::Stream & Packet); // 리스트를 통해 가입신청.
	bool AnsJoinExpeditionList(BM::Stream & Packet);
	bool ModifyExpeditionList(SExpeditionList const & ListInfo);

	void ExpeditionListForEach(SFunctionForEach & Function);

	void WriteExpeditionListInfoToPacket(BM::Stream & Packet, BM::Stream & SendPacket);

private:

	ExpeditionListPool m_ExpeditionListPool;
	ExpeditionListHash m_ExpeditionListHash;

	Loki::Mutex m_Mutex;

};

#define g_ExpeditionListMgr SINGLETON_STATIC(PgExpeditionListMgr)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGEXPEDITIONLISTMGR_H