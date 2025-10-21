#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGGUILD_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGGUILD_H

#include "Variant/PgEmporia.h"

//typedef enum eGuildDBFlag
//{
//	GDBF_None			= 0x0,
//	GDBF_Basic_Info		= 0x01,//셋팅되면 완료
//	GDBF_Extern_Info	= 0x02,
//	GDBF_Member_Grade	= 0x04,
//	GDBF_Member			= 0x08,
//	GDBF_ReqQuery		= 0x10,	// DB Query 요청하였다.
//	GDBF_AllReady		= GDBF_Basic_Info| GDBF_Extern_Info| GDBF_Member_Grade| GDBF_Member,
//} EGuildDBFlag;

typedef std::set<BM::GUID> CONT_GUILD_INVENTORY_OBSERVER;	// 길드금고 정보를 받을 브로드캐스트 대상(캐릭터 guid)
typedef std::set<SItemPos> CONT_GUILD_INVENTORY_FIXING;		// 길드금고에서 수정 중인 아이템( Modify가 완료 되면 지워준다 )

class PgGuild
{
public:
	PgGuild();
	~PgGuild();
	static const SGuildMemberGradeInfo DefaultGradeGroup();


public:
	BM::GUID const Guid()const					{ return m_kBasicInfo.kGuildGuid; }
	BM::GUID const Master()const				{ return m_kBasicInfo.kMasterGuid; }
	std::wstring const Name()const			{ return m_kBasicInfo.kGuildName; }
	unsigned short const GuildLevel()const		{ return m_kBasicInfo.sLevel; }
	//unsigned short const TaxRate()const			{ return m_kBasicInfo.sTaxRate; }
	HRESULT ChangePos(SContentsUser const &rkUser);
	//bool SetTaxRate(int const iPercent = 0); // n/100
	EGuildCommandRet ChangeOwner(BM::GUID const &rkNewOwner, bool bTest);
	EGuildCommandRet ChangeEmblem(BYTE const cNewEmblem);
	//bool SetEmblem();
	unsigned short const SkillPoint()			{ return m_kBasicInfo.sSkillPoint; }
	SGuildOtherInfo GetOtherInfo() const		{ return SGuildOtherInfo(m_kBasicInfo, m_kEmporiaInfo.byGrade); }
	__int64 GetExp() const						{ return m_kBasicInfo.iExperience; }
	BYTE Emblem()const							{ return m_kBasicInfo.cEmblem; }

	void BroadCast(BM::Stream const& rkPacket, BM::GUID const &rkIgnoreGuid=BM::GUID::NullData(), BYTE byGrade = GMG_Membmer)const;
	void Rename(std::wstring const& rkNewName) {m_kBasicInfo.kGuildName = rkNewName;};
	bool UpdateAbil(BM::GUID const &rkCharGuid, WORD const wAbilType, int const iUpdatedVal);
	EGuildCommandRet Levelup(bool const bTest);
	EGuildCommandRet AddExp(__int64 const iAddExp);
	EGuildCommandRet SetLevel(int const iLv);
	void SetExp( __int64 const iExp );
	bool UseExp(__int64 const iUseExp );
	EGuildCommandRet AddSkill(int const iSkillNo, bool const bTest);

	void SetGuildInv( BYTE const * pkInvExtern, BYTE const * pkExternIdx );

	// 입출금 권한
	const BYTE GetGuildInvAuthority_In()const	{ return m_kBasicInfo.abyInvAuthority[1]; }
	const BYTE GetGuildInvAuthority_Out()const	{ return m_kBasicInfo.abyInvAuthority[3]; }
	void SetGuildInvAuthority(const BYTE byIn, const BYTE byOut )	{ m_kBasicInfo.abyInvAuthority[1] = byIn; m_kBasicInfo.abyInvAuthority[3] = byOut; }

	bool IsHaveGuildInvAuthority_In(const BM::GUID& rkCharGuid);
	bool IsHaveGuildInvAuthority_Out(const BM::GUID& rkCharGuid);

	EGuildCommandRet TestSetGrade(BM::GUID const& rkMasterGuid, BM::GUID const& rkCharGuid, BYTE const cTargetGrade);

	bool Add(SContentsUser const& rkUser, int const iGrade = GMG_Membmer);
	bool Add(SGuildMemberInfo const& rkGuildMember);
	bool Del(BM::GUID const &rkCharGuid);
	void Set(const SGuildBasicInfo& rkBasicInfo);
	bool Set(BM::GUID const &rkGuildGuid, BM::GUID const &rkMasterGuid, std::wstring const& rkGuildName, BM::PgPackedTime const& rkCreateTime, BYTE const cEmblem);
	bool SetMember(BM::GUID const &rkCharGuid, int const iGrade);
	bool SetMemberHomeAddr(BM::GUID const &rkCharGuid, SHOMEADDR const kHomeAddr);
	bool GetMemberGuid(VEC_GUID& rkVec, BM::GUID const &rkExcludeGuid = BM::GUID::NullData()) const;
	bool AddGradeGroup(SGuildMemberGradeInfo const &rkGradeGroup);
	HRESULT SetGradeGroup(const SGuildMemberGradeInfo& rkGradeGroup);
	void GetGuildSkill(ContSkillNo &rkOut) const;

	void Clear();

	bool WriteMemberListToPacket(BM::Stream& rkPacket, BM::GUID const &rkCharGuid=BM::GUID::NullData())const;//Guild Member List
	void WriteInfoToPacket(BM::Stream& rkPacket) const;
	void WriteListToPacket(BM::Stream& rkPacket) const;
	bool WriteMemberPacket(BM::GUID const &rkCharGuid, BM::Stream& rkPacket) const;
	void WriteSkillToPacket(BM::Stream &rkPacket)const;
	void WriteToSavePacket(BM::Stream &rkPacket);

	bool IsMaster(BM::GUID const &rkCharGuid) const;
	bool IsMember(BM::GUID const &rkCharGuid) const;
	size_t MemberCount() const {return m_kMember.size();}
	bool IsOwner(BM::GUID const &rkCharGuid) const;

	void AddDBFlag(DWORD dwFlag);
	bool IsDBFlag(DWORD dwFlag);

	bool Get(BM::GUID const &rkCharGuid, SGuildMemberInfo &rkOut) const;
	void SendMail( BYTE const byGrade, std::wstring const &wstrSender, std::wstring const &wstrTitle, std::wstring const &wstrContents )const;

protected:
	bool Get(BM::GUID const &rkCharGuid, SGuildMemberInfo const* &pkOut) const;
	bool Get(BM::GUID const &rkCharGuid, SGuildMemberInfo* &pkOut);

	CLASS_DECLARATION(std::wstring, m_kNotice, Notice);
	CLASS_DECLARATION(BM::PgPackedTime, m_kLastSaveTime, LastSaveTime);
	CLASS_DECLARATION(bool, m_bLastSaveSuccess, LastSaveSuccess);

	// Emporia
	CLASS_DECLARATION(SGuildEmporiaInfo,	m_kEmporiaInfo,		EmporiaInfo);
	CLASS_DECLARATION(PgLimitClass,			m_kSetMercenary,	SetMercenary);

	// 길드가입 설정
	CLASS_DECLARATION(SSetGuildEntrancedOpen,	m_kSetGuildEntranceOpen, SetGuildEntranceOpen);


protected:
	DWORD m_dwDBFlag;
	SGuildBasicInfo m_kBasicInfo;
	ContGuildGrade m_kGrade;
	ContGuildMember m_kMember;
	PgMySkill m_kMySkill;
	
protected:
	mutable PgInventory				m_kInv; // 길드금고
	CONT_GUILD_INVENTORY_OBSERVER	m_kContGuildInventoryObserver;
	CONT_GUILD_INVENTORY_FIXING		m_kContFixingItemPos;

public:
	PgInventory* GetInven()							{ return &m_kInv; }
	PgInventory const* GetInven() const				{ return &m_kInv; }
	void SwapInven( PgInventory & rkInven )			{ m_kInv.Swap( rkInven );}

	void SendGuildInventory(const BM::GUID& rkCharGuid, const BYTE byInvType);
	void BroadCastGuildInventoryObserverForUpdateItem(const DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, const std::wstring& rkCharName); // 길드금고 아이템 변화
	void BroadCastGuildInventoryExtend( HRESULT hRet, const __int64 iCause, const EInvType kInvType, const BYTE byExtendSize );

	void AddGuildInventoryObserver(const BM::GUID& rkCharGuid);
	void RemoveGuildInventoryObserver(const BM::GUID& rkCharGuid);

	bool AddFixingItem(const SItemPos& rkPos);
	bool AddFixingItem(const SItemPos& rkCasterPos, const SItemPos& rkTargetPos);

	void RemoveFixingItem(const SItemPos& rkPos);
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGGUILD_H