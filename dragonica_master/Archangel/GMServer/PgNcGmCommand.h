#ifndef GM_GMSERVER_GM_KOREA_PGNCGMCOMMAND_H
#define GM_GMSERVER_GM_KOREA_PGNCGMCOMMAND_H

#include "IGmLocaleProcess.h"

class PgNcGmCommand
	: public IGMLocaleProcess
{
protected:
	virtual HRESULT VHookVPush(void const* pkWorkData){return S_OK;}//NC는 훅 할것이 없음.
	virtual HRESULT VCompleteMsg(BM::GUID const &kMsgGuid){return S_OK;}
	virtual HRESULT VPostErrorMsg(BM::GUID const &kMsgGuid, std::wstring const& kAccountID, short const nType, int const iErrorCode, int const iCash = 0){return S_OK;}
	
protected:
	//계정관련
	virtual bool ReqCreateAccount(BM::GUID const &rkReqKey, wchar_t const *szAccountId, wchar_t const *szPassword, void const *Input){return false;};
	virtual void RecvCreateAccount(BM::Stream *pkPacket){};
	virtual bool ReqChangePassWord(BM::GUID const &rkReqKey, wchar_t const *szAccountId, wchar_t const *szNewPassword, void const *Input){return false;};
	virtual void RecvChangePassWord(BM::Stream *pkPacket){};
	//정보요청
	virtual bool ReqServerInfo();
	virtual void RecvServerInfo(BM::Stream *pkPacket){};
	virtual bool ReqCharacterList(BM::GUID const &rkReqKey, void*  input, void*  input2);
	virtual void RecvCharacterList(BM::Stream *pkPacket);
	virtual bool ReqCharacterBaseInfo(BM::GUID const &rkReqKey, wchar_t const *szString);
	virtual void RecvCharacterBaseInfo(BM::Stream *pkPacket);
	virtual bool ReqEquipItemInfo(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvEquipItemInfo(BM::Stream *pkPacket){};
	virtual bool ReqInvenInfo(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvInvenInfo(BM::Stream *pkPacket){};
	virtual bool ReqBankInfo(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvBankInfo(BM::Stream *pkPacket){};
	virtual bool ReqSkillInfo(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvSkillInfo(BM::Stream *pkPacket){};
	virtual bool ReqStatusInfo(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvStatusInfo(BM::Stream *pkPacket){};
	virtual bool ReqResistanceInfo(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvResistanceInfo(BM::Stream *pkPacket){};
	virtual bool ReqChangeJobList(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvChangeJobList(BM::Stream *pkPacket){};
	virtual bool ReqPetInfo(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvPetInfo(BM::Stream *pkPacket){};
	virtual bool ReqIsUseName(BM::GUID const &rkReqKey, wchar_t const *szName);
	virtual void RecvIsUseName(BM::Stream *pkPacket);

	//정보 수정
	virtual bool ReqGiveSkillPoint(BM::GUID const &rkReqKey, wchar_t const *szString, int iPoint){return false;};
	virtual void RecvGiveSkillPoint(BM::Stream *pkPacket){};
	virtual bool ReqGiveMoney(BM::GUID const &rkReqKey, wchar_t const *szString, __int64 biMoney){return false;};
	virtual void RecvGiveMoney(BM::Stream *pkPacket){};
	virtual bool ReqMapMove(BM::GUID const &rkReqKey, wchar_t const *szString, int iMapNo, float fx, float fy, float fz){return false;};
	virtual bool ReqGiveItem(BM::GUID const &rkReqKey, wchar_t const *szString, int iItemNo, int iSIze = 0){return false;};
	virtual void RecvGiveItem(BM::Stream *pkPacket){};
	virtual bool ReqDeleteItem(BM::GUID const &rkReqKey, wchar_t const *szString, BM::GUID &rkItemGuid, int iSIze = 0){return false;};
	virtual void RecvDeleteItem(BM::Stream *pkPacket){};
	virtual bool ReqChangeSkill(BM::GUID const &rkReqKey, wchar_t const *szString, int iSkillId, int flag){return false;};
	virtual void RecvChangeSkill(BM::Stream *pkPacket){};
	virtual bool ReqChangeCharacterName(BM::GUID const &rkReqKey, wchar_t const *szOldName, wchar_t const *szNewName){return false;};
	virtual void RecvChangeCharacterName(BM::Stream *pkPacket){};
	virtual bool ReqChangeCharacterExp(BM::GUID const &rkReqKey, wchar_t const *szString, int iExp){return false;};
	virtual void RecvChangeCharacterExp(BM::Stream *pkPacket){};
	virtual bool ReqChangeCharacterLevel(BM::GUID const &rkReqKey, wchar_t const *szString, int iLv){return false;};
	virtual void RecvChangeCharacterLevel(BM::Stream *pkPacket){};
	virtual bool ReqChangeGmLevel(BM::GUID const &rkReqKey, wchar_t const *szString, int iGmLv){return false;};
	virtual void RecvChangeGmLevel(BM::Stream *pkPacket){};
	virtual bool ReqAddPoint(BM::GUID const &rkReqKey, wchar_t const* szAccountId, int iAddPoint){return false;};
	virtual bool ReqGetPoint(BM::GUID const &rkReqKey, wchar_t const* szAccountId){return false;};
	virtual bool ReqChangeBirthday(BM::GUID const &rkReqKey, wchar_t const *szAccount, wchar_t const *szBirthDay){return false;};
	
	//제재 관련
	virtual bool ReqFreezeAccount(BM::GUID const &rkReqKey, wchar_t const *szAccountId, int iValue, void*  input){return false;};
	virtual void RecvFreezeAccount(BM::Stream *pkPacket){};
	virtual bool ReqKickUser(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvKickUser(BM::Stream *pkPacket){};
	virtual bool ReqDeleteCharacter(BM::GUID const &rkReqKey, wchar_t const *szString){return false;};
	virtual void RecvDeleteCharacter(BM::Stream *pkPacket){};
	virtual bool ReqAttachmentItem(BM::GUID const &rkReqKey, wchar_t const *szString, BM::GUID const &rkItemGuid){return false;};
	virtual void RecvAttachmentItem(BM::Stream *pkPacket){};
	virtual bool ReqDisAttachmentItem(BM::GUID const &rkReqKey, wchar_t const *szString, BM::GUID const &rkItemGuid){return false;};
	virtual void RecvDisAttachmentItem(BM::Stream *pkPacket){};

public:
	virtual void LocaleGMCommandProcess(BM::Stream * const pkPacket);

	PgNcGmCommand(void);
	~PgNcGmCommand(void);
};

#endif // GM_GMSERVER_GM_KOREA_PGNCGMCOMMAND_H