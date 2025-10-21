#include "stdafx.h"
#include "PgAchievement.h"
#include "Lohengrin/packetstruct.h"

PgAchievementMgr::PgAchievementMgr()
{
}

PgAchievementMgr::~PgAchievementMgr()
{
}

void PgAchievementMgr::Broadcast(EAchievementType const Type, BM::Stream & rPacket)
{
	BM::Stream BroadcastPacket(PT_M_C_NFY_GENERIC_ACHIEVEMENT_NOTICE);

	switch( Type )
	{
	case EAchievementType::E_ACHI_LEVELUP:
		{
			WriteToPacket_LevelUp(rPacket, BroadcastPacket);
		}break;
	case EAchievementType::E_ACHI_ENCHANT_SUCCESS:
		{
			WriteToPacket_EnchantSuccess(rPacket, BroadcastPacket);
		}break;
	case EAchievementType::E_ACHI_GET_ITEM_MESSAGE:
		{
			WriteToPacket_GetItemMessage(rPacket, BroadcastPacket);
		}break;
	default:
		{
			return;
		}break;
	}

	SendToServerType(CEL::ST_CENTER, BroadcastPacket);
}

void PgAchievementMgr::WriteToPacket_LevelUp(BM::Stream & rPacket, BM::Stream & rOut)
{
	CONT_GENERIC_NOTICE ContNotice;
	std::wstring UserName;
	short Level = 0;

	rPacket.Pop(UserName);
	rPacket.Pop(Level);

	BM::vstring vLevel(Level);
	ContNotice.insert( std::make_pair(L"#NAME#", UserName) );
	ContNotice.insert( std::make_pair(L"#LEVEL#", (std::wstring)vLevel) );

	rOut.Push(2809);
	PU::TWriteTable_AA(rOut, ContNotice);
}

void PgAchievementMgr::WriteToPacket_GetItemMessage(BM::Stream & rPacket, BM::Stream & rOut)const
{
	CONT_GENERIC_NOTICE ContNotice;
	std::wstring UserName;
	std::wstring ItemName;

	rPacket.Pop(UserName);
	rPacket.Pop(ItemName);

	ContNotice.insert( std::make_pair(L"#NAME#", UserName) );
	ContNotice.insert( std::make_pair(L"#ITEMNAME#", ItemName) );

	rOut.Push(2812);
	PU::TWriteTable_AA(rOut, ContNotice);
}


void PgAchievementMgr::WriteToPacket_EnchantSuccess(BM::Stream & rPacket, BM::Stream & rOut)
{
	CONT_GENERIC_NOTICE ContNotice;
	std::wstring UserName, ItemName;
	int EnchantLevel = 0;

	rPacket.Pop(UserName);
	rPacket.Pop(EnchantLevel);
	rPacket.Pop(ItemName);

	BM::vstring vLevel(EnchantLevel);
	ContNotice.insert( std::make_pair(L"#NAME#", UserName) );
	ContNotice.insert( std::make_pair(L"#ENCHANT#", (std::wstring)vLevel) );
	ContNotice.insert( std::make_pair(L"#ITEMNAME#", ItemName) );

	rOut.Push(2810);
	PU::TWriteTable_AA(rOut, ContNotice);
}