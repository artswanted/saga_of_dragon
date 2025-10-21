#ifndef GM_GMSERVER_GM_IGMLOCALEPROCESS_H
#define GM_GMSERVER_GM_IGMLOCALEPROCESS_H

#include "CEL/CEL.h"
#include "CEL/CoreCenter.h"
#include "PgGMUserMgr.h"

class IGMLocaleProcess
{
public:
	IGMLocaleProcess(){}
	virtual ~IGMLocaleProcess(){}
public:
	virtual HRESULT VHookVPush(void const* pkWorkData) = 0;
	virtual HRESULT VCompleteMsg(BM::GUID const &kMsgGuid) = 0;
	virtual HRESULT VPostErrorMsg(BM::GUID const &kMsgGuid, std::wstring const& kAccountID, short const nType, int const iErrorCode, int const iCash = 0) = 0;

public:
	//계정관련
	virtual bool ReqCreateAccount(BM::GUID const &rkReqKey, wchar_t const *szAccountId, wchar_t const *szPassword, void const *Input) = 0;
	virtual void RecvCreateAccount(BM::Stream *pkPacket) = 0;
	virtual bool ReqChangePassWord(BM::GUID const &rkReqKey, wchar_t const *szAccountId, wchar_t const *szNewPassword, void const *Input) = 0;
	virtual void RecvChangePassWord(BM::Stream *pkPacket) = 0;
	//정보요청
	virtual bool ReqServerInfo() = 0;
	virtual void RecvServerInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqCharacterList(BM::GUID const &rkReqKey, void* input1, void* input2) = 0;
	virtual void RecvCharacterList(BM::Stream *pkPacket) = 0;
	virtual bool ReqCharacterBaseInfo(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvCharacterBaseInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqEquipItemInfo(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvEquipItemInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqInvenInfo(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvInvenInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqBankInfo(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvBankInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqSkillInfo(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvSkillInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqStatusInfo(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvStatusInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqResistanceInfo(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvResistanceInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqChangeJobList(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvChangeJobList(BM::Stream *pkPacket) = 0;
	virtual bool ReqPetInfo(BM::GUID const &rkReqKey, wchar_t const *szString) = 0;
	virtual void RecvPetInfo(BM::Stream *pkPacket) = 0;
	virtual bool ReqIsUseName(BM::GUID const &rkReqKey, wchar_t const *szName) = 0;
	virtual void RecvIsUseName(BM::Stream *pkPacket) = 0;

	//정보 수정
	virtual bool ReqGiveSkillPoint(BM::GUID const &rkReqKey, wchar_t const *szString, int iPoint) = 0;
	virtual void RecvGiveSkillPoint(BM::Stream *pkPacket) = 0;
	virtual bool ReqGiveMoney(BM::GUID const &rkReqKey, wchar_t const *szString, __int64 biMoney) = 0;
	virtual void RecvGiveMoney(BM::Stream *pkPacket) = 0;
	virtual bool ReqMapMove(BM::GUID const &rkReqKey, wchar_t const *szString, int iMapNo, float fx, float fy, float fz) = 0;
	virtual bool ReqGiveItem(BM::GUID const &rkReqKey, wchar_t const *szString, int iItemNo, int iSIze = 0) = 0;
	virtual void RecvGiveItem(BM::Stream *pkPacket) = 0;
	virtual bool ReqDeleteItem(BM::GUID const &rkReqKey, wchar_t const *szString, BM::GUID &rkItemGuid, int iSIze = 0) = 0;
	virtual void RecvDeleteItem(BM::Stream *pkPacket) = 0;
	virtual bool ReqChangeSkill(BM::GUID const &rkReqKey, wchar_t const *szString, int iSkillId, int flag) = 0;	//(add = 1, delete = 2, Change = 3)
	virtual void RecvChangeSkill(BM::Stream *pkPacket) = 0;
	virtual bool ReqChangeCharacterName(BM::GUID const &rkReqKey, wchar_t const *szOldName, wchar_t const *szNewName) = 0;
	virtual void RecvChangeCharacterName(BM::Stream *pkPacket) = 0;
	virtual bool ReqChangeCharacterExp(BM::GUID const &rkReqKey, wchar_t const *szString, int iExp) = 0;
	virtual void RecvChangeCharacterExp(BM::Stream *pkPacket) = 0;
	virtual bool ReqChangeCharacterLevel(BM::GUID const &rkReqKey, wchar_t const *szString, int iLv) = 0;
	virtual void RecvChangeCharacterLevel(BM::Stream *pkPacket) = 0;
	virtual bool ReqChangeGmLevel(BM::GUID const &rkReqKey, wchar_t const *szString, int iGmLv) = 0;
	virtual void RecvChangeGmLevel(BM::Stream *pkPacket) = 0;
	virtual bool ReqAddPoint(BM::GUID const &rkReqKey, wchar_t const *szAccountId, int iAddPoint) = 0;
	virtual bool ReqGetPoint(BM::GUID const &rkReqKey, wchar_t const *szAccountId) = 0;
	virtual bool ReqChangeBirthday(BM::GUID const &rkReqKey, wchar_t const *szAccount, wchar_t const *szBirthDay) = 0;
	
	//제재 관련
	virtual bool ReqFreezeAccount(BM::GUID const &rkReqKey,wchar_t const *szAccountId, int iValue, void* input) = 0;
	virtual void RecvFreezeAccount(BM::Stream *pkPacket) = 0;
	virtual bool ReqKickUser(BM::GUID const &rkReqKey,wchar_t const *szString) = 0;
	virtual void RecvKickUser(BM::Stream *pkPacket) = 0;
	virtual bool ReqDeleteCharacter(BM::GUID const &rkReqKey,wchar_t const *szString) = 0;
	virtual void RecvDeleteCharacter(BM::Stream *pkPacket) = 0;
	virtual bool ReqAttachmentItem(BM::GUID const &rkReqKey,wchar_t const *szString, BM::GUID const &rkItemGuid) = 0;
	virtual void RecvAttachmentItem(BM::Stream *pkPacket) = 0;
	virtual bool ReqDisAttachmentItem(BM::GUID const &rkReqKey,wchar_t const *szString, BM::GUID const &rkItemGuid) = 0;
	virtual void RecvDisAttachmentItem(BM::Stream *pkPacket) = 0;
	virtual void LocaleGMCommandProcess(BM::Stream * const pkPacket) = 0;
};

#endif // GM_GMSERVER_GM_IGMLOCALEPROCESS_H