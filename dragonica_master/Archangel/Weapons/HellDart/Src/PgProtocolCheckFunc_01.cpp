#include "stdafx.h"
#include "BM/Stream.h"
#include "Lohengrin/dbtables.h"
#include "lohengrin/Packetstruct.h"
#include "variant/PgPlayer.h"

HRESULT CALLBACK Check_PT_C_N_REQ_CHECK_CHARACTERNAME_OVERLAP( BM::Stream &kPacket )
{
	std::wstring kCharacterName;
	if( kPacket.Pop( kCharacterName ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}


HRESULT CALLBACK Check_PT_C_S_REQ_CREATE_CHARACTER( BM::Stream &kPacket )
{//OK. Lim 090408
	tagReqCreateCharacter kTemp;

	if(kTemp.ReadFromPacket(kPacket))
	{
		if(StrIsCorrect(kTemp.szName))
		{
			if(!kPacket.RemainSize())
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_REQ_DELETE_CHARACTER( BM::Stream &kPacket )
{//OK. Lim 090408
	BM::GUID kTemp;

	if(kPacket.Pop(kTemp))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_REQ_SELECT_CHARACTER( BM::Stream &kPacket )
{//OK. Lim 090408
	BM::GUID kSelCharGuid;
	bool bPassTutorial;

	if(kPacket.Pop(kSelCharGuid))
	if(kPacket.Pop(bPassTutorial))
	{
		if(BM::IsCorrectType(bPassTutorial))
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_REQ_CHARACTER_LIST( BM::Stream &kPacket )
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_ANS_ONLY_PING( BM::Stream &kPacket )
{//OK. Lim 090408
	BM::GUID kGuid;

	if(	kPacket.Pop(kGuid) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_NFY_UNIT_POS( BM::Stream &kPacket )
{//OK. Lim 090408
	BM::GUID kGuid;
	POINT3 ptPos;
	
	if(	kPacket.Pop(kGuid) )
	if(	kPacket.Pop(ptPos) )
	{
		bool bIncludeScPos;
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
		else
		{
			if( kPacket.Pop(bIncludeScPos)&& kPacket.Pop(ptPos) && !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EFFECT_ESCAPE_KEYDOWN( BM::Stream &kPacket )
{
	int iEffect;

	if(	kPacket.Pop(iEffect) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_EMOTION( BM::Stream &kPacket )
{
	BYTE byEmotionType = 0;	
	int iEmotiID;
	POINT3 ptPos;
	if ( kPacket.Pop(byEmotionType) )
	{
		if ( ET_MAX > byEmotionType )
		{			
			if(	kPacket.Pop(iEmotiID) )
			if(	kPacket.Pop(ptPos) )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}


HRESULT CALLBACK Check_PT_C_M_REQ_INTERACTIVE_EMOTION_REQUEST( BM::Stream &kPacket )
{
	int iActionNo = 0;
	BM::GUID kReqPlayerGUID;
	BM::GUID kTargetGUID;
	if(kPacket.Pop(iActionNo))
    if(kPacket.Pop(kReqPlayerGUID))
	if(kPacket.Pop(kTargetGUID))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_ANS_INTERACTIVE_EMOTION_REQUEST( BM::Stream &kPacket )
{
	bool bAccept = false;
	int iActionNo = 0;
	BM::GUID kReqPlayerGUID;
	BM::GUID kTargetGUID;
	if(kPacket.Pop(bAccept))
	if(kPacket.Pop(iActionNo))
    if(kPacket.Pop(kReqPlayerGUID))
	if(kPacket.Pop(kTargetGUID))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}