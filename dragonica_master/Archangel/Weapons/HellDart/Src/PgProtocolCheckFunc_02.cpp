#include "stdafx.h"
#include "BM/Stream.h"
#include "variant/PgPlayer.h"
#include "lohengrin/Packetstruct.h"

HRESULT CALLBACK Check_PT_C_M_REQ_ACTION2(BM::Stream &kPacket)
{//Not Yet
	SActionInfo kActioninfo;
	DWORD dwSyncTime = 0;
	DWORD dwSeedCallCounter = 0;
	BYTE byTargetNum = 0;	
	BM::GUID kGuid;
	BYTE byCollision = 0;
	bool bExtraData = 0;

	if(kPacket.Pop(kActioninfo))
	if(kPacket.Pop(dwSyncTime))
	if(kPacket.Pop(dwSeedCallCounter))
	if(kPacket.Pop(byTargetNum))
	{
		for (BYTE byI=0; byI<byTargetNum; ++byI)
		{		
			if(kPacket.Pop(kGuid))
			if(kPacket.Pop(byCollision))
			{
				continue;
			}
			else
			{
				return E_FAIL;
			}
		}
		
		if(kPacket.Pop(bExtraData))
		{
//			kActioninfo.dwTimeStamp = BM::GetTime32();
//			kPacket.ModifyData(action_info_rd_pos, &kActioninfo, sizeof(kActioninfo));
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_SELECTED_BOX(BM::Stream &kPacket)
{//OK. Lim 090408
	int iTempInt = 0;
	if( kPacket.Pop( iTempInt ) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RESULT_END(BM::Stream &kPacket)
{
	if ( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
