#ifndef WEAPON_VARIANT_UTIL_PGLOGUTIL_H
#define WEAPON_VARIANT_UTIL_PGLOGUTIL_H

#include "Variant/PgExpedition.h"

class PgPlayer;
class PgLogCont;

namespace PgLogUtil
{
	typedef enum
	{
		ELogout_Type_Normal = 0,
		ELogout_Type_GMKick = 1,
	} ELogout_Type;

	//
	class PgLogWrapperPlayer : public PgLogCont
	{
	public:
		explicit PgLogWrapperPlayer(ELogMainType const eMainType, ELogSubType const eSubType, PgPlayer const& rkEventUser, int const iGroundNo);
		~PgLogWrapperPlayer();

	private:
		PgPlayer const& m_kEventUser;
	};


	//
	class PgLogWrapperContents : public PgLogCont
	{
	public:
		explicit PgLogWrapperContents(ELogMainType const eMainType, ELogSubType const eSubType, SContentsUser const& rkEventUser);
		~PgLogWrapperContents();

	private:
		SContentsUser const& m_kEventUser;
	};


	//
	inline size_t AtIndex(size_t const iIndex)
	{
		return (iIndex - 1);
	}

	std::wstring const GetPartyOptionItemString(EPartyOptionItem const eValue);
	std::wstring const GetPartyOptionPublic(EPartyOptionPublicTitle const eValue);
	std::wstring const GetExpeditionOptionItemString(EExpeditionOptionItem const eValue);
	std::wstring const GetExpeditionOptionPublic(EExpeditionOptionPublicTitle const eValue);
}

namespace PgChatLogUtil
{
	void Log(ELogSubType const eLogSubType, SContentsUser const &rkFromUser, SContentsUser const &rkToUser, std::wstring const &rkContents);
	void Log(ELogSubType const eLogSubType, PgPlayer const* pkPlayer, std::wstring const &rkContents, int const iGroundNo);
};

namespace PgQuestLogUtil
{
	void ParamLog(PgContLogMgr &rkContLogMgr, EOrderSubType const eType, BM::GUID const& rkCharGuid, int const iQuestID, int const iParamNo, int const iPreVal, int const iNextVal);
	void IngLog(PgContLogMgr &rkContLogMgr, EOrderSubType const eType, BM::GUID const& rkCharGuid, int const iQuestID, int const iVal3, int const iVal4, int const iVal5 = 0);
	void EndLog(PgContLogMgr &rkContLogMgr, EOrderSubType const eType, BM::GUID const& rkCharGuid, int const iQuestID);
	void EndLog(PgContLogMgr &rkContLogMgr, EOrderSubType const eType, BM::GUID const& rkCharGuid, std::vector< int > const& rkVec, int const iIgnoreID = 0);
};

namespace PgPlayerLogUtil
{
	void DeathLog(PgPlayer* pkPlayer, ELogUserDieType const eKillerType, SGroundKey const& rkGndKey, std::wstring const& rkCasterName, BM::GUID const& rkCasterGuid);
	void AliveLog(PgPlayer* pkPlayer, ELogUserResurrectType const eResurrectType, SGroundKey const& rkGndKey, std::wstring const& rkCasterName, BM::GUID const& rkCasterGuid, __int64 const iPreExp, __int64 const iResultExp);
};

namespace PgItemLogUtil
{
	void CreateLog(PgLog& kLog, PgItemWrapper const & kCurItem, EOrderMainType const eMainType = ELOrderMain_Item, EOrderSubType const eSubType = ELOrderSub_Create);
	void CreateLog(PgLogCont& rkLogCont, PgItemWrapper const & kCurItem);
	void CreateLog(BM::GUID const & kOwnerGuid,PgContLogMgr &rkContLogMgr,PgItemWrapper const & kItem);
	void ModifyLog(BM::GUID const & kOwnerGuid,PgContLogMgr &rkContLogMgr,PgItemWrapper const & kCurItem,PgItemWrapper const & kOldItem);
	void DeleteLog(BM::GUID const & kOwnerGuid,PgContLogMgr &rkContLogMgr,PgItemWrapper const & kItem);
};

#endif // WEAPON_VARIANT_UTIL_PGLOGUTIL_H