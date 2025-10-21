#ifndef WEAPON_VARIANT_PGCHARACTERCARD_H
#define WEAPON_VARIANT_PGCHARACTERCARD_H

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "BM/PgTask.h"
#include "Lohengrin/PacketStruct.h"
#include "Lohengrin/dbtables.h"

class PgCharacterCard
{
public:
	
	PgCharacterCard():m_kYear(0),m_kSex(0),m_kLocal(0),m_kPopularPoint(0),m_kTodayPopularPoint(0),m_kEnable(false),m_kConstellation(0),m_kBlood(0),m_kHobby(0),m_kStyle(0),m_kBGndNo(0){}
	explicit PgCharacterCard(BM::GUID const & kGuid,BYTE const bYear,BYTE const bSex,int const iLocal,__int64 const i64Point,std::wstring const & kComment,bool const kEnable,
		BYTE const kConstellation,BYTE const kBlood,BYTE const kHobby,BYTE const kStyle,int const iBGndNo):
	m_kOwnerGuid(kGuid),m_kYear(bYear),m_kSex(bSex),m_kLocal(iLocal),m_kPopularPoint(i64Point),m_kTodayPopularPoint(0),m_kComment(kComment),
	m_kEnable(kEnable),m_kConstellation(kConstellation),m_kBlood(kBlood),m_kHobby(kHobby),m_kStyle(kStyle),m_kBGndNo(iBGndNo){}
	~PgCharacterCard(){}

public:

	CLASS_DECLARATION_S(BM::GUID,OwnerGuid);
	CLASS_DECLARATION_S(std::wstring,Name);
	CLASS_DECLARATION_S(BYTE,Year);
	CLASS_DECLARATION_S(BYTE,Sex);
	CLASS_DECLARATION_S(int,Local);
	CLASS_DECLARATION_S(__int64,PopularPoint);
	CLASS_DECLARATION_S(__int64,TodayPopularPoint);
	CLASS_DECLARATION_S(std::wstring,Comment);
	CLASS_DECLARATION_S(bool,Enable);
	CLASS_DECLARATION_S(BYTE,Constellation);
	CLASS_DECLARATION_S(BYTE,Blood);
	CLASS_DECLARATION_S(BYTE,Hobby);
	CLASS_DECLARATION_S(BYTE,Style);
	CLASS_DECLARATION_S(int,BGndNo);

public:

	bool LoadDB(CEL::DB_DATA_ARRAY::const_iterator & itor);
	void MakeUpdateQuery(CEL::DB_QUERY & kQuery) const;
	bool GetCardAbil(CONT_CARD_ABIL::mapped_type & kAbil) const;
	void WriteToPacket(BM::Stream & kPacket) const;
	void ReadFromPacket(BM::Stream & kPacket);
};

#endif // WEAPON_VARIANT_PGCHARACTERCARD_H