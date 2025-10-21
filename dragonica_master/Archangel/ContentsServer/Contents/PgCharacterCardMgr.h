#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGCHARACTERCARDMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGCHARACTERCARDMGR_H

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "BM/PgTask.h"
#include "Lohengrin/PacketStruct.h"
#include "Variant/PgCharacterCard.h"

typedef std::set<BM::GUID> CONT_CARDID;
typedef struct tagLOCAL_NODE
{
	int iLocal;
	CONT_CARDID kCont;
	tagLOCAL_NODE():iLocal(0){}
	tagLOCAL_NODE(int const kLocal):iLocal(kLocal){}
}SLOCAL_NODE;

typedef std::map<int,SLOCAL_NODE> CONT_LOCAL;
typedef struct tagAGE_NODE
{
	BYTE		bAge;
	CONT_LOCAL	kCont;
	tagAGE_NODE():bAge(0){}
	explicit tagAGE_NODE(BYTE const kAge):bAge(kAge){}
}SAGE_NODE;

typedef std::map<int,SAGE_NODE>	CONT_AGE;
typedef struct tagSEX_NODE
{
	BYTE		bSex;
	CONT_AGE	kCont;
	tagSEX_NODE():bSex(0){}
	explicit tagSEX_NODE(BYTE const kSex):bSex(kSex){}
}SSEX_NODE;

typedef std::map<EUnitGender,SSEX_NODE>		CONT_CARD_SEARCH_MGR;
typedef std::map<BM::GUID,PgCharacterCard>	CONT_CHARACTER_CARD;

class PgCharacterCardMgr
{
public:
	
	PgCharacterCardMgr(){}
	~PgCharacterCardMgr(){}

public:

	bool const Locked_RegCard(PgCharacterCard const & kCard);
	bool const Locked_UnRegCard(BM::GUID const & kCharGuid);
	bool const Locked_GetCard(BM::GUID const & kCharGuid,PgCharacterCard & kCard);
	bool const Locked_SetCard(PgCharacterCard const & kCard);

	void Locked_RecvPT_M_I_REQ_CHARACTER_CARD_INFO(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void Locked_RecvPT_M_I_REQ_SEARCH_MATCH_CARD(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

	void SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid);

private:

	bool const GetCard(BM::GUID const & kCharGuid,PgCharacterCard & kCard);
	void InsertSearchKey(PgCharacterCard const & kCard);
	void RemoveSearchKey(PgCharacterCard const & kCard);
	void FindCard(BM::GUID const & kReqGuid,EUnitGender const kSex,int const iAge,int const iLocal,CONT_MATCH_CARD_INFO & kContCard);
	
private:

	mutable Loki::Mutex m_kMutex;
	CONT_CHARACTER_CARD m_kContCard;
	CONT_CARD_SEARCH_MGR m_kContSearchMgr;
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGCHARACTERCARDMGR_H