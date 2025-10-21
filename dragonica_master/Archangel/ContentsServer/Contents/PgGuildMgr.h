#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGGUILDMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGGUILDMGR_H

#include "BM/PgTask.h"
#include "Variant/PgWaiter.h"

#include "Variant/PgEmporia.h"
#include "PgGuild.h"

//
typedef struct tagGuildOwnerLoginDay
{
	tagGuildOwnerLoginDay();
	tagGuildOwnerLoginDay(tagGuildOwnerLoginDay const& rhs);

	void ReadFromDBResult(CEL::DB_DATA_ARRAY::const_iterator& result_iter);
	void WriteToPacket(BM::Stream& rkPacket) const;
	void ReadFromPacket(BM::Stream& rkPacket);

	BM::GUID kGuildGuid;
	short sGuildLevel;
	int iLastLoginDay;
} SGuildOwnerLoginDay;
//typedef std::list< SGuildOwnerLoginDay > CONT_GUILD_LAST_LOGIN_DAY;

//
typedef enum eGuildAutoChangeOwnerResult
{
	GACOR_NONE			= 0,
	GACOR_WARNNING_1	= 1,
	GACOR_WARNNING_2	= 2,
	GACOR_RUN			= 3,
} EGuildAutoChangeOwnerResult;

//
typedef struct tagGuildOwnerLoginDayResult : public SGuildOwnerLoginDay
{
	tagGuildOwnerLoginDayResult();
	tagGuildOwnerLoginDayResult(tagGuildOwnerLoginDayResult const& rhs);
	tagGuildOwnerLoginDayResult(BM::Stream& rkPacket);

	EGuildAutoChangeOwnerResult eResult;
} SGuildOwnerLoginDayResult;
typedef std::map< BM::GUID, SGuildOwnerLoginDayResult > CONT_GUILD_LAST_LOGIN_DAY_RESULT;

//
namespace PgGuildMgrUtil
{
	class PgAutoChangeOwnerMgr
	{
	public:
		PgAutoChangeOwnerMgr();
		~PgAutoChangeOwnerMgr();

		bool Tick();
		void GetOwnerLastLoginDay(BM::GUID const& rkMgrGuid, bool const bInit = false);
		void CheckDay(CONT_GUILD_LAST_LOGIN_DAY_RESULT& rkContLastLoginDayResult);
	protected:

	private:
		BM::PgPackedTime m_kLastTickDate;
		CONT_GUILD_LAST_LOGIN_DAY_RESULT m_kContLastLoginDay;
	};

	bool IsCanRejoinUser(SContentsUser const& rkUser);

	void SendToItemOrder(SContentsUser const &rkUser, CONT_PLAYER_MODIFY_ORDER const &kOrder, BM::Stream const &rkAddonPacket = BM::Stream());
	void SendToItemOrder(BM::GUID const &rkCharGuid, CONT_PLAYER_MODIFY_ORDER const &kOrder, BM::Stream const &rkAddonPacket = BM::Stream());
	inline std::pair<bool,int> GetMapAttr(int const iGroundNo);
	inline bool IsCanReqInOutState(int const iGroundNo);
	inline bool IsCanReqInOutState(SContentsUser const &rkUser);
	void JoinGuildOrder(PgGuild *pkGuild, SContentsUser const &rkUser);
	void JoinGuildOrder(PgGuild *pkGuild, BM::GUID const &rkCharGuid);
	void JoinGuildOrder(PgGuild *pkGuild, VEC_GUID const &rkVec);
	void LeaveGuildOrder(SContentsUser const &rkUser);
	void LeaveGuildOrder(BM::GUID const &rkCharGuid);
	void ChangeOwnerQuery(EGuildCommand const eCmdType, BM::GUID const& rkGuildGuid, BM::GUID const& rkMasterCharGuid, BM::GUID const& rkNewMasterCharGuid);

	extern __int64 iAfterCanRejoinHour;
	bool InitGuildConstant();
	void UpdateLastLoingDate(BM::GUID const& rkGuildGuid, BM::GUID const& rkCharGuid);
}

namespace PgGuildMgrLogUtil
{
	void ChangeOwnerLog(PgGuild const* pkGuild, SContentsUser const& rkMaster, SContentsUser const& rkUser, EGuildCommandRet const eRet);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//
class PgGuildMgr 
{
public:
	PgGuildMgr();
	~PgGuildMgr();


public:
	bool ProcessMsg(SEventMessage *pkMsg);
	bool ProcessTCommand(int const iCommand, BM::GUID const &rkGuid, BM::Stream *pkPacket);
	bool ProcessMCommand(int const iCommand, BM::GUID const &rkCharGuid, BM::Stream *pkPacket);
	bool ProcessCommand(int const iCommand, BM::GUID const &rkCharGuid, BM::Stream *pkPacket);
	bool ReqUpdateAbil(BM::GUID const &rkCharGuid, WORD const sAbilType, int const iUpdatedVal);

	void ClearEntranceOpenGuild() { m_kOpenGuildList.clear(); }
	void AddEntranceOpenGuild(const SEntranceOpenGuild& kEntranceOpenGuild);
	void SetRecommendGuild() { m_iRecommendGuildRank = BM::Rand_Range(m_kOpenGuildList.size(), 1); }// 추천길드 결정(일단은 랜덤)
	int	 GetRecommendGuild() const { return m_iRecommendGuildRank; }
	void NotifyEntranceOpenGuildList(BM::GUID const& rkCharGuid);
	
	void AddGuildEntranceApplicant(const SGuildEntranceApplicant& kGuildEntranceApplicant, bool bSend = false);
	void DelGuildEntranceApplicant(BM::GUID const &rkCharGuid);
	void ChangeApplicantState(BM::GUID const &rkGuildGuid, BM::GUID const &rkCharGuid, BYTE byState);

	void WriteGuildEntranceApplicantListToPacket(BM::GUID const &rkGuildGuid, BM::Stream& rkPacket) const;
	void NotifyGuildApplicationState(BM::GUID const &rkCharGuid, BM::GUID const &rkGuildGuid = BM::GUID(), bool const bDestroy = false);

	void Locked_SetEmporiaChallengeInfo( BM::GUID const &kReqMemberGuid, BM::GUID const &kGuildID, SGuildEmporiaInfo const &kEmporiaInfo, __int64 const i64CostMoney, bool const bThrow, BYTE const byType );

	void Locked_Tick();
	void CheckNewDay();

	void ProcessItemQuery(const BM::GUID& rkGuildGuid, const CEL::DB_RESULT& rkResult);
	PgInventory* GetInven(const BM::GUID& rkGuildGuid );
	HRESULT ModifyGuildInventory(const BM::GUID& rkGuildGuid, const DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, const std::wstring &rkCharName);	
	
	bool IsHaveGuildInvAuthority_In(const BM::GUID& rkGuildGuid, const BM::GUID& rkCharGuid );
	bool IsHaveGuildInvAuthority_Out(const BM::GUID& rkGuildGuid, const BM::GUID& rkCharGuid );

protected:
	void Clear();
	bool ProcessMsgFromUser(BM::Stream *pkMsg);
	bool ProcessMsgFromServer(BM::Stream *pkMsg);
	bool ProcessGMCommand(int const iCommandType, BM::Stream *pkMsg);

	bool LoginGuildUser(SContentsUser const& rkUser);
	bool LogoutGuildUser(SContentsUser const& rkUser);
	bool DelCharToGuild(BM::GUID const &rkCharGuid);
	bool DelGuildName(std::wstring const& rkGuildName);
	HRESULT ChangePosGuildUser(SContentsUser const &rkUserInfo);
	HRESULT Get(BM::GUID const &rkGuildGuid, PgGuild* &pkOut) const;
	bool AddGuildName(std::wstring const& rkGuildName, BM::GUID const &rkGuildGuid);
	bool ProcessWaiter(BM::GUID const &rkGuildGuid);
	void WriteInfoToPacket(BM::Stream& rkPacket) const;
	void WriteListToPacket(BM::Stream& rkPacket) const;
	bool WriteMemberListToPacket( BM::Stream& rkPacket, BM::GUID const &rkCharGuid )const;
	bool AddCharToGuild(BM::GUID const &rkCharGuid, BM::GUID const &rkGuildGuid);
	bool GetCharToGuild(BM::GUID const &rkCharGuid, BM::GUID& rkOut) const;
	bool Find(BM::GUID const &rkGuildGuid) const;
	bool FindGuildName(std::wstring const& rkGuildName) const;

	bool SendToGuild_ByChar(BM::GUID const &rkCharGuid, BM::Stream const& rkPacket, BM::GUID const &rkPassCharGuid = BM::GUID::NullData()) const;
	bool SendToGuild(BM::GUID const &rkGuildGuid, BM::Stream const& rkPacket, BM::GUID const &rkPassCharGuid = BM::GUID::NullData()) const;
	bool SendToGuild(PgGuild const* pkGuild, BM::Stream const& rkPacket, BM::GUID const &rkIgnoreGuid = BM::GUID::NullData()) const;
	HRESULT Save(PgGuild *pkGuild, BM::GUID const& rkOperatorGuid, EGuildCommand eCmd = GC_S_SaveBasic, int const iExternInt = 0) const;

	PgGuild* NewGuild();
	bool Delete(PgGuild* &pkGuild);
	bool Delete(BM::GUID const &rkGuildGuid);

	void ReqLoadData(BM::GUID const &rkGuildGuid, BM::Stream & rkCommandPacket = BM::Stream());
	EGuildCommandRet Join(BM::GUID const &rkGuildGuid, SContentsUser const& rkUser, bool const bJoin, HRESULT hDBRet);
	EGuildCommandRet Leave(SContentsUser const& rkUser);
	EGuildCommandRet Kick(BM::GUID const rkGuildGuid, SContentsUser const& rkMaster, BM::GUID const& rkKickedGuid, bool const bGMCmd = false);
	EGuildCommandRet Create(SContentsUser const& rkMaster, std::wstring const& rkGuildName, BYTE const cEmblem);
	EGuildCommandRet ReqJoin(SContentsUser const& rkMaster, BM::GUID const &rkGuildGuid, SContentsUser const& rkUser) const;
	EGuildCommandRet ReqRename(PgGuild* pkGuild, SContentsUser const& rkMaster, std::wstring const& rkNewName);
	EGuildCommandRet ReqRename(BM::GUID const& rkGuildGuid, BM::GUID const& rkMasterMemberGuid, std::wstring const& rkNewName, bool const bGMCmd = false);
	EGuildCommandRet ReqLevelup(PgGuild* pkGuild, SContentsUser const &rkMaster, bool const bTest);
	EGuildCommandRet ReqGuildInventoryCreate(PgGuild* pkGuild, SContentsUser const &rkMaster, bool const bTest);
	EGuildCommandRet ReqAddSkill(PgGuild* pkGuild, SContentsUser const &rkMaster, int const iSkillNo, bool const bTest);
	EGuildCommandRet ReqChangeOwner(EGuildCommand const eCmdType, PgGuild* pkGuild, BM::GUID const& rkMasterCharGuid, BM::GUID const& rkNewMasterCharGuid);
	EGuildCommandRet ReqSetGuildNotice(PgGuild* pkGuild, SContentsUser const &rkMaster, BM::Stream &rkPacket);
	EGuildCommandRet ReqSetLevel(PgGuild * pkGuild, int const iLv);
	EGuildCommandRet ReqSetExp(PgGuild * pkGuild, __int64 const iExp);
	EGuildCommandRet ReqSetMoney(PgGuild * pkGuild, __int64 const iMoney);

	CLASS_DECLARATION_S_NO_SET(BM::GUID, MgrGuid);	// GuildManager 자체의 GUID

protected:
	typedef std::map< BM::GUID, PgGuild* > ContGuild;
	typedef std::map< BM::GUID, BM::GUID > ContCharToGuild;
	typedef std::map< std::wstring, BM::GUID > ContWstrToGuild;
	typedef PgWaiterSet< BM::GUID, BM::GUID > ContInfoWaiter;
	typedef std::vector< SEntranceOpenGuild > CONT_ENTRANCE_OPEN_GUILD_LIST;
	typedef std::map< BM::GUID, SGuildEntranceApplicant > CONT_GUILD_ENTRANCE_APPLICANT_LIST;

	ContGuild m_kGuild;
	ContCharToGuild m_kCharToGuild;
	ContWstrToGuild m_kNameToGuild;
	ContInfoWaiter m_kWaiter;
	ContInfoWaiter m_kOtherWaiter;
	CONT_ENTRANCE_OPEN_GUILD_LIST		m_kOpenGuildList;
	CONT_GUILD_ENTRANCE_APPLICANT_LIST	m_kGuildApplicantList;
	int m_iRecommendGuildRank;

	mutable Loki::Mutex m_kMutex;

	BM::TObjectPool<PgGuild> m_kGuildPool;

	PgGuildMgrUtil::PgAutoChangeOwnerMgr m_kAutoChangeOwnerMgr;
};

#define g_kGuildMgr SINGLETON_STATIC(PgGuildMgr)

// 서버 구조 변경 후에 할 일
//	- Guild Load/Save 를 GuildMgr이 직접 하도록 수정

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGGUILDMGR_H