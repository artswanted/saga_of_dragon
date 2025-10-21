#include "stdafx.h"
#include "BM/Stream.h"
#include "Lohengrin/Packetstruct.h"
#include "Variant/Item.h"

// Pet

extern HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_ACTION( BM::Stream & );

HRESULT CALLBACK Check_PT_C_M_REQ_PET_ACTION(BM::Stream &kPacket)
{
	return Check_PT_C_M_REQ_ITEM_ACTION( kPacket );
}

HRESULT CALLBACK Check_PT_C_M_REQ_PET_RENAME(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	if ( true == kPacket.Pop( kItemPos ) )
	{
		std::wstring wstrPetName;
		if ( true == kPacket.Pop( wstrPetName, PgItem_PetInfo::MAX_PET_NAMELEN ) )
		{
			size_t const iRemainSize = kPacket.RemainSize();
			if (	!iRemainSize
				||	sizeof(SItemPos) == iRemainSize )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_PET_COLOR_CHANGE(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	if ( true == kPacket.Pop( kItemPos ) )
	{
		char cColorIndex = 0;
		kPacket.Pop( cColorIndex );

		if ( 0 <= cColorIndex  )
		{
			size_t const iRemainSize = kPacket.RemainSize();
			if (	!iRemainSize
				||	sizeof(SItemPos) == iRemainSize )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_CHANGE_TO_PET(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	if ( true == kPacket.Pop( kItemPos ) )
	{
		if ( true == kPacket.Pop( kItemPos ) )
		{
			DWORD dwClientTime;
			if ( true == kPacket.Pop( dwClientTime ) )
			{
				if ( sizeof(bool) == kPacket.RemainSize() )
				{
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_PET_UPGRADE(BM::Stream& rkPacket)
{
	SItemPos kItemPos;
	if ( true == rkPacket.Pop( kItemPos ) )
	{
		DWORD dwClientTime;
		if ( true == rkPacket.Pop( dwClientTime ) )
		{
			if ( 0 == rkPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REGIST_CHAT_BLOCK(BM::Stream & Packet)
{
	BYTE BlockType;
	std::wstring BlockName;
	if( true == Packet.Pop(BlockName) )
	{
		if( true == Packet.Pop(BlockType) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_UNREGIST_CHAT_BLOCK(BM::Stream & Packet)
{
	std::wstring BlockName;
	if( true == Packet.Pop(BlockName) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CHANGE_CHAT_BLOCK_OPTION(BM::Stream & Packet)
{
	BYTE BlockType;
	std::wstring BlockName;
	if( true == Packet.Pop(BlockName) )
	{
		if( true == Packet.Pop(BlockType) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MOVE_EVENT_GROUND(BM::Stream & Packet)
{
	int EventNo;
	if( true == Packet.Pop(EventNo) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EVENT_PROGRESS_INFO(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_INDUN_PARTY_LIST(BM::Stream & Packet)
{
	BM::GUID kGuid;
	int Type = 0;
	VEC_INT kContMapNo;
	if(Packet.Pop(kGuid))
	{
		if(Packet.Pop(Type))
		{
			if(Packet.Pop(kContMapNo))
			{
				if( !Packet.RemainSize() )
				{
					return S_OK;
				}
			}
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_INDUN_PARTY_ENTER(BM::Stream & Packet)
{
	short sChannel = 0;
	BM::GUID kPartyGuid;
	BM::GUID kCharGuid;
	if(Packet.Pop(sChannel))
	if(Packet.Pop(kCharGuid))
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_NFY_MAPMOVE_TO_PARTYMGR(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CHECK_CAN_KICK(BM::Stream & Packet)
{
	BM::vstring vStr;
	BM::GUID CharGuid;
	if( true == Packet.Pop(vStr) )
	{
		if( true == Packet.Pop(CharGuid) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GET_DAILY(BM::Stream & Packet)
{
	return S_OK;
}

HRESULT CALLBACK Check_PT_C_M_REQ_BATTLE_PASS_QUEST(BM::Stream & kPacket)
{
	int iBattlePassDay;
	int iBattlePassGrade;

    if (kPacket.Pop(iBattlePassDay) &&
    	kPacket.Pop(iBattlePassGrade) &&
		kPacket.RemainSize() == 0)
	{
		return S_OK;
	}
	return E_FAIL;
}
