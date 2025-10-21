#include "stdafx.h"
#include <tchar.h>
#include "PgCharacterCard.h"
#include "DefAbilType.h"
#include "tabledatamanager.h"

bool PgCharacterCard::LoadDB(CEL::DB_DATA_ARRAY::const_iterator & itor)
{
	(*itor).Pop(m_kOwnerGuid);		++itor;
	(*itor).Pop(m_kYear);			++itor;
	(*itor).Pop(m_kSex);			++itor;
	(*itor).Pop(m_kLocal);			++itor;
	(*itor).Pop(m_kPopularPoint);	++itor;
	(*itor).Pop(m_kTodayPopularPoint);	++itor;
	(*itor).Pop(m_kComment);		++itor;
	(*itor).Pop(m_kEnable);			++itor;
	(*itor).Pop(m_kConstellation);	++itor;
	(*itor).Pop(m_kBlood);			++itor;
	(*itor).Pop(m_kHobby);			++itor;
	(*itor).Pop(m_kStyle);			++itor;
	(*itor).Pop(m_kBGndNo);			++itor;
	return true;
}

void PgCharacterCard::MakeUpdateQuery(CEL::DB_QUERY & kQuery) const
{
	kQuery.InsertQueryTarget(OwnerGuid());
	kQuery.PushStrParam(OwnerGuid());
	kQuery.PushStrParam(Year());
	kQuery.PushStrParam(Sex());
	kQuery.PushStrParam(Local());
	kQuery.PushStrParam(Comment());
	kQuery.PushStrParam(Enable());
	kQuery.PushStrParam(PopularPoint());
	kQuery.PushStrParam(TodayPopularPoint());
	kQuery.PushStrParam(Constellation());
	kQuery.PushStrParam(Blood());
	kQuery.PushStrParam(Hobby());
	kQuery.PushStrParam(Style());
	kQuery.PushStrParam(BGndNo());
}

void PgCharacterCard::WriteToPacket(BM::Stream & kPacket) const
{
	kPacket.Push(OwnerGuid());
	kPacket.Push(Name());
	kPacket.Push(Year());
	kPacket.Push(Sex());
	kPacket.Push(Local());
	kPacket.Push(Comment());
	kPacket.Push(Enable());
	kPacket.Push(PopularPoint());
	kPacket.Push(TodayPopularPoint());
	kPacket.Push(Constellation());
	kPacket.Push(Blood());
	kPacket.Push(Hobby());
	kPacket.Push(Style());
	kPacket.Push(BGndNo());
}

void PgCharacterCard::ReadFromPacket(BM::Stream & kPacket)
{
	kPacket.Pop(m_kOwnerGuid);
	kPacket.Pop(m_kName);
	kPacket.Pop(m_kYear);
	kPacket.Pop(m_kSex);
	kPacket.Pop(m_kLocal);
	kPacket.Pop(m_kComment);
	kPacket.Pop(m_kEnable);
	kPacket.Pop(m_kPopularPoint);
	kPacket.Pop(m_kTodayPopularPoint);
	kPacket.Pop(m_kConstellation);
	kPacket.Pop(m_kBlood);
	kPacket.Pop(m_kHobby);
	kPacket.Pop(m_kStyle);
	kPacket.Pop(m_kBGndNo);
}

bool GetCardKey(TBL_DEF_CARD_KEY_STRING_KEY const & kKey,BYTE & bCardKey)
{
	CONT_DEF_CARD_KEY_STRING const * pkContKey = NULL;
	g_kTblDataMgr.GetContDef(pkContKey);
	if(!pkContKey)
	{
		return false;
	}

	CONT_DEF_CARD_KEY_STRING::const_iterator iter = pkContKey->find(kKey);
	if(iter == pkContKey->end())
	{
		return false;
	}

	bCardKey = (*iter).second.bCardKey;
	return true;
}

bool PgCharacterCard::GetCardAbil(CONT_CARD_ABIL::mapped_type & kAbil) const
{
	CONT_CARD_ABIL const * pkCont = NULL;
	g_kTblDataMgr.GetContDef(pkCont);
	if(!pkCont)
	{
		return false;
	}

	BYTE bBlood = 0;
	if(false == GetCardKey(TBL_DEF_CARD_KEY_STRING_KEY(CAKT_BLOOD,Blood()),bBlood))
	{
		return false;
	}

	BYTE bConstellation = 0;
	if(false == GetCardKey(TBL_DEF_CARD_KEY_STRING_KEY(CAKT_CONSTELLATION,Constellation()),bConstellation))
	{
		return false;
	}

	BYTE bHobby = 0;
	if(false == GetCardKey(TBL_DEF_CARD_KEY_STRING_KEY(CAKT_HOBBY,Hobby()),bHobby))
	{
		return false;
	}

	BYTE bStyle = 0;
	if(false == GetCardKey(TBL_DEF_CARD_KEY_STRING_KEY(CAKT_STYLE,Style()),bStyle))
	{
		return false;
	}

	CONT_CARD_ABIL::const_iterator iter = pkCont->find(TBL_CARD_ABIL_KEY(bConstellation,bHobby,bBlood,bStyle));
	if(iter == pkCont->end())
	{
		return false;
	}

	kAbil = (*iter).second;

	return true;
}
