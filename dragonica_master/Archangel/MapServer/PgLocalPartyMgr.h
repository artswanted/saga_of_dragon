#ifndef MAP_MAPSERVER_OBJECT_PARTY_PGLOCALPARTYMGR_H
#define MAP_MAPSERVER_OBJECT_PARTY_PGLOCALPARTYMGR_H

#include "Variant/PgPartyMgr.h"
#include "Variant/PgExpeditionMgr.h"
class PgPartyItemRule;
class PgExpeditionItemRule;

//
class PgLocalPartyMgrImpl
	:	public PgPartyMgr< PgLocalParty >
	,	public PgExpeditionMgr< PgLocalExpedition >
{
	typedef std::set< SGroundKey > ContGndKeySet;
public:
	typedef std::map< SGroundKey, int > ContCheckOverlap;
	typedef BM::TObjectPool< PgPartyItemRule > PoolPartyItemRule;
	typedef std::map< BM::GUID, PgPartyItemRule* > ContPartyItemRule;

	typedef BM::TObjectPool< PgExpeditionItemRule > PoolExpeditionItemRule;
	typedef std::map< BM::GUID, PgExpeditionItemRule* > ContExpeditionItemRule;

	PgLocalPartyMgrImpl();
	virtual ~PgLocalPartyMgrImpl();

	void Clear();

	bool GetPartyMemberGround(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, VEC_GUID& rkOutVec, BM::GUID const& rkIgnore = BM::GUID::NullData())const;
	size_t GetMemberCount(BM::GUID const& rkPartyGuid)const;
	size_t GetMaxMemberCount(BM::GUID const& rkPartyGuid)const;
	std::wstring const& GetPartyName(BM::GUID const& rkPartyGuid)const;
	std::wstring const& GetPartySubName(BM::GUID const& rkPartyGuid)const;
	bool GetTotalLevel(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, unsigned int& iOutTotalLevel);
	bool GetPartyMember(BM::GUID const& rkPartyGuid, VEC_GUID& rkOut)const;
	void GetContPartyGuid(VEC_GUID & rkCont)const;
	
	bool IsMaster(BM::GUID const& rkPartyGuid, BM::GUID const& rkCharGuid)const;
	bool GetPartyMasterGuid(BM::GUID const& rkPartyGuid, BM::GUID& rkOutGuid)const;

	bool GetPartyOption(BM::GUID const& rkPartyGuid, SPartyOption& rkOut);
	bool GetPartyShareItem_NextOwner(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, BM::GUID& rkNextOwner);

	bool GetPartyMaxLevel(BM::GUID const& rkPartyGuid, int& iLevel) const;
	
	bool WriteToPacketPartyName(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket);
	void WriteToPacketPartyList(BM::Stream& rkPacket);
	
	bool ProcessMsg(unsigned short const usType, SGroundKey const& rkGndKey, BM::Stream* pkMsg);
	bool SyncFromContents(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket);

	int GetPartyMemberFriend(BM::GUID const& rkPartyGuid, BM::GUID const& rkCharGuid);
	PgLocalParty* GetLocalParty(BM::GUID const & PartyGuid);

	// 원정대
	bool SyncFromContentsExpedition(BM::GUID const& rkExpeditionGuid, BM::Stream& rkPacket);
	bool GetExpeditionMemberGround(BM::GUID const& ExpeditionGuid, SGroundKey const & GndKey, VEC_GUID & OutVec, BM::GUID const & Ignore) const;
	bool WriteToPacketExpeditionName(BM::GUID const & ExpeditionGuid, BM::Stream & Packet);
	bool GetExpeditionMasterGuid(BM::GUID const & ExpeditionGuid, BM::GUID & OutGuid) const;
	size_t GetExpeditionMemberCount(BM::GUID const & ExpeditionGuid) const;
	PgLocalExpedition* GetLocalExpedition(BM::GUID const & ExpeditionGuid) const;	
	bool GetExpeditionMember(BM::GUID const & ExpeditionGuid, VEC_GUID & Out);
	HRESULT IsStartable(BM::GUID const & ExpeditionGuid);
	BM::GUID const & GetExpeditionNpcGuid(BM::GUID const & ExpeditionGuid);
	int GetMemberTeamNo(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid);
	int GetMaxTeamCount(BM::GUID const & ExpeditionGuid);
	bool IsExpeditionMaster(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid);
	
	bool GetExpeditionOption(BM::GUID const & ExpeditionGuid, SExpeditionOption & Out);
	bool GetExpeditionShareItem_NextOwner(BM::GUID const ExpeditionGuid, SGroundKey const & GndKey, BM::GUID & NextOwner);

protected:
	virtual bool Delete(BM::GUID const& rkPartyGuid);

	bool Leave(SGroundKey const& rkGndKey, BM::Stream& rkPacket);
	bool ChangeMaster(BM::GUID const& rkPartyGuid, BM::GUID const& rkNewMasterGuid);
	bool MapMoved(BM::GUID const& rkPartyGuid, SGroundKey const& rkCurGndKey, SGroundKey const& rkTrgGndKey, VEC_GUID const& kVec);
	bool Rename(BM::GUID const& rkPartyGuid, std::wstring const& rkNewPartyName, SPartyOption const& rkNewPartyOption );

	bool AddItemRule(BM::GUID const& rkPartyGuid, PgPartyItemRule*& pkOut);//아이템 습득 룰에 관해서
	bool GetItemRule(BM::GUID const& rkPartyGuid, PgPartyItemRule*& pkOut);
	bool DelItemRule(BM::GUID const& rkPartyGuid);
	bool SyncPartyItemRule(PgLocalParty* pkParty);

	//원정대
	bool WriteToPacketExpeditionName(PgLocalExpedition* pExpedition, BM::Stream & Packet);
	bool WriteToPacketPartyName(PgLocalParty* pkParty, BM::Stream& rkPacket);
	bool ExpeditionMapMoved(BM::GUID const & ExpeditionGuid, SGroundKey const & CurGndKey, SGroundKey const & TrgGndKey, VEC_GUID const & Vec);
	bool IsRemainExpeditionMemberInServer( PgLocalExpedition const * pExpedition, SGroundKey const & GndKey ) const;
	bool CleanUpPartyList(PgLocalParty * pParty);
	void CleanUpExpedition(PgLocalExpedition * pExpedition);

	bool ChangeExpeditionMaster(BM::GUID const & ExpeditionGuid, BM::GUID const & NewMasterGuid);
	bool RenameExpedition(BM::GUID const & ExpeditionGuid, std::wstring const & NewName, SExpeditionOption const & NewOption);
	bool LeaveExpedition(SGroundKey const & GndKey, BM::Stream & Packet);

	bool AddExpeditionItemRule(BM::GUID const & ExpeditionGuid, PgExpeditionItemRule *& pOut);//아이템 습득 룰에 관해서
	bool GetExpeditionItemRule(BM::GUID const & ExpeditionGuid, PgExpeditionItemRule *& pOut);
	bool DelExpeditionItemRule(BM::GUID const & ExpeditionGuid);
	bool SyncExpeditionItemRule(PgLocalExpedition * pExpedition);

protected:
	bool UpdateProperty(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket);
	bool IsRemainMemberInServer( PgLocalParty const *pkParty, SGroundKey const& rkGndKey )const;

	PoolPartyItemRule m_kItemRulePool;
	ContPartyItemRule m_kPartyItemRule;

	PoolExpeditionItemRule m_ExpeditionItemRulePool;
	ContExpeditionItemRule m_ExpeditionItemRule;

};

//
class PgLocalPartyMgr
	: TWrapper< PgLocalPartyMgrImpl >
{
public:
	PgLocalPartyMgr();
	virtual ~PgLocalPartyMgr();

	void Clear();
	bool GetPartyMemberGround(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, VEC_GUID& rkOutVec, BM::GUID const& rkIgnore = BM::GUID::NullData())const;
	size_t GetMemberCount(BM::GUID const& rkPartyGuid)const;
	size_t GetMaxMemberCount(BM::GUID const& rkPartyGuid)const;
	std::wstring const& GetPartyName(BM::GUID const& rkPartyGuid)const;
	std::wstring const& GetPartySubName(BM::GUID const& rkPartyGuid)const;
	bool GetTotalLevel(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, unsigned int& iOutTotalLevel);
	bool GetPartyMember(BM::GUID const& rkPartyGuid, VEC_GUID& rkOut)const;
	void GetContPartyGuid(VEC_GUID & rkCont)const;
	bool IsMaster(BM::GUID const& rkPartyGuid, BM::GUID const& rkCharGuid)const;
	bool GetPartyMasterGuid(BM::GUID const& rkPartyGuid, BM::GUID& rkOutGuid)const;
	bool GetPartyMaxLevel(BM::GUID const& rkPartyGuid, int &rkOutGuid)const;
	bool GetPartyOption(BM::GUID const& rkPartyGuid, SPartyOption& rkOut);
	bool GetPartyShareItem_NextOwner(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, BM::GUID& rkNextOwner);
	bool WriteToPacketPartyName(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket);
	void WriteToPacketPartyList(BM::Stream& rkPacket);
	bool ProcessMsg(unsigned short const usType, SGroundKey const& rkGndKey, BM::Stream* pkMsg);
	bool SyncFromContents(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket);
	int GetPartyMemberFriend(BM::GUID const& rkPartyGuid, BM::GUID const& rkCharGuid);
	PgLocalParty* GetLocalParty(BM::GUID const & PartyGuid) const;

	// 원정대
	bool SyncFromContentsExpedition(BM::GUID const& rkExpeditionGuid, BM::Stream& rkPacket);
	bool GetExpeditionMemberGround(BM::GUID const& ExpeditionGuid, SGroundKey const & GndKey, VEC_GUID & OutVec, BM::GUID const & Ignore = BM::GUID::NullData()) const;
	size_t GetExpeditionMemberCount(BM::GUID const & ExpeditionGuid) const;
	bool GetExpeditionMasterGuid(BM::GUID const & ExpeditionGuid, BM::GUID & OutGuid) const;
	bool WriteToPacketExpeditionName(BM::GUID const & ExpeditionGuid, BM::Stream & Packet) const;
	PgLocalExpedition* GetLocalExpedition(BM::GUID const & ExpeditionGuid) const;
	bool IsExpeditionMaster(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid);
	bool GetExpeditionMember(BM::GUID const & ExpeditionGuid, VEC_GUID & out);
	HRESULT IsStartable(BM::GUID const & ExpeditionGuid);
	BM::GUID const & GetExpeditionNpcGuid(BM::GUID const & ExpeditionGuid);
	int GetMemberTeamNo(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid);
	int GetMaxTeamCount(BM::GUID const & ExpeditionGuid);
	bool GetExpeditionShareItem_NextOwner(BM::GUID const ExpeditionGuid, SGroundKey const & GndKey, BM::GUID & NextOwner);
	bool GetExpeditionOption(BM::GUID const & ExpeditionGuid, SExpeditionOption & Out);
};

#endif // MAP_MAPSERVER_OBJECT_PARTY_PGLOCALPARTYMGR_H