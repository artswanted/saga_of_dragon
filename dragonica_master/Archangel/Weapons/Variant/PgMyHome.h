#ifndef WEAPON_VARIANT_UNIT_PGMYHOME_H
#define WEAPON_VARIANT_UNIT_PGMYHOME_H

#include "Lohengrin/PacketStruct.h"
#include "Lohengrin/PacketStruct2.h"
#include "FCS/AntiHack.h"
#include "PgControlUnit.h"
#include "PgParty.h"
#include "PgMyQuest.h"
#include "PgQuickInventory.h"
#include "PgUserMapInfo.h"
#include "PgPlayer_MissionReport.h"
#include "PgPlayTime.h"
#include "PgAchievements.h"
#include "PgCharacterCard.h"
#include "PgPlayer.h"

class PgMyHome : public PgPlayer
{
	CLASS_DECLARATION_S(BM::GUID,OwnerGuid);
	CLASS_DECLARATION_S(bool,IsInSide);
	CLASS_DECLARATION_S(std::wstring,OwnerName);

public:
	PgMyHome(){}
	virtual ~PgMyHome(){}
public:
	virtual EUnitType UnitType()const{return UT_MYHOME;}
	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);
	virtual bool IsAlive()const { return true; }
};

namespace MYHOMEUTIL
{
	__int64 const CalcTex(SMYHOME const & kMyHome);
	void MailLog(BM::GUID const & kOwnerGuid,BM::GUID const & kMailGuid,SSendMailInfo const & kMailData,PgContLogMgr & kContLogMgr);
	void MakeQuery(SMYHOME const & kMyHome, SMYHOME const & kOldMyHome, CEL::DB_QUERY & kQuery);
};

#endif // WEAPON_VARIANT_UNIT_PGMYHOME_H