#include "stdafx.h"
#include "BM/Stream.h"
#include "variant/PgPlayer.h"
#include "Lohengrin/Packetstruct.h"
#include "Variant/PgMission.h"

HRESULT CALLBACK Check_PT_C_M_REQ_MISSION_RESTART(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MISSION_ROULETTE_STOP(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_PARTY_LIST(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kOrderGuid;
	int iPartyAttribute = 0;
	int iPartyContinent = 0;
	int iPartyArea_NameNo = 0;

	if(kPacket.Pop(kOrderGuid))
	if(kPacket.Pop(iPartyAttribute))
	if(kPacket.Pop(iPartyContinent))
	if(kPacket.Pop(iPartyArea_NameNo))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_PARTY_NAME(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kPartyGuid;

	if(kPacket.Pop(kPartyGuid))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_FIND_PARTY_USER_LIST(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kCharGuid;
	int iClass;
	unsigned short sHLevel;
	unsigned short sLLevel;

	if( kPacket.Pop(kCharGuid) &&
		kPacket.Pop(iClass) &&
		kPacket.Pop(sHLevel) &&
		kPacket.Pop(sLLevel) )
	{
		if( !kPacket.RemainSize() )
			return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REGIST_PRIVATE_PARTY_FIND(BM::Stream &kPacket)
{
	BM::GUID kCharGuid;
	std::wstring kContents;

	if( kPacket.Pop(kCharGuid) &&
		kPacket.Pop(kContents) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_N_C_REQ_UNREGIST_PRIVATE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SEARCH_PEOPLE_LIST(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_JOIN_PARTY_REFUSE(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bState;
	if( kPacket.Pop(bState) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MSGBOX_CALL(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bRet;
	DWORD m_iItemNo;

	if(kPacket.Pop(bRet))
	if(kPacket.Pop(m_iItemNo))
	{
		if(BM::IsCorrectType(bRet))
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MISSION_CHAOS_ACTION(BM::Stream &kPacket)
{
	int iMissionNo = 0;
	//std::string strTriggerID;
	//if ( true == kPacket.Pop( strTriggerID, MAX_PATH ) )
	if( kPacket.Pop(iMissionNo) )
	{
		int iLevel;
		int iMapType;

		if(kPacket.Pop(iLevel))
			if(kPacket.Pop(iMapType))
			{
				int iCustomData1 = 0;
				int iCustomData2 = 0;
				int iCustomData3 = 0;

				switch(iMapType)
				{
				case MMST_NONE:
					{
					}break;
				case MMST_AUTO:
					{
						if( !kPacket.Pop(iCustomData1) )
						{
							return E_FAIL;
						}
					}break;
				case MMST_SELECT:
					{
						if( !kPacket.Pop(iCustomData1) )
						{
							return E_FAIL;
						}
						if( !kPacket.Pop(iCustomData2) )
						{
							return E_FAIL;
						}
						if( !kPacket.Pop(iCustomData3) )
						{
							return E_FAIL;
						}
					}break;
				default:
					{
						return E_FAIL;
					}
				}

				if(!kPacket.RemainSize())
				{
					return S_OK;
				}
			}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MISSION_TRIGGER_ACTION(BM::Stream &kPacket)
{
	std::string strTriggerID;
	if ( true == kPacket.Pop( strTriggerID, MAX_PATH ) )
	{
		int iLevel;
		int iMapType;

		if(kPacket.Pop(iLevel))
		if(kPacket.Pop(iMapType))
		{
			int iCustomData1 = 0;
			int iCustomData2 = 0;
			int iCustomData3 = 0;

			switch(iMapType)
			{
			case MMST_NONE:
				{
				}break;
			case MMST_AUTO:
				{
					if( !kPacket.Pop(iCustomData1) )
					{
						return E_FAIL;
					}
				}break;
			case MMST_SELECT:
				{
					if( !kPacket.Pop(iCustomData1) )
					{
						return E_FAIL;
					}
					if( !kPacket.Pop(iCustomData2) )
					{
						return E_FAIL;
					}
					if( !kPacket.Pop(iCustomData3) )
					{
						return E_FAIL;
					}
				}break;
			default:
				{
					return E_FAIL;
				}
			}
			int iMutatorCount = 0;
			if(!kPacket.Pop(iMutatorCount))
			{
				return E_FAIL;
			}

			if(iMutatorCount > 0)
			{
				for(int i = 0; i < iMutatorCount; i++)
				{
					int iMutatorNo;

					if(!kPacket.Pop(iMutatorNo))
					{
						return E_FAIL;
					}
				}
			}

			if(!kPacket.RemainSize())
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ENTER_MISSION(BM::Stream &kPacket)
{//OK. Lim 090408
	int iLevel;
	int iMapType;

	if(kPacket.Pop(iLevel))
	if(kPacket.Pop(iMapType))
	{
		int iCustomData1 = 0;
		int iCustomData2 = 0;
		int iCustomData3 = 0;

		switch(iMapType)
		{
		case MMST_NONE:
			{
			}break;
		case MMST_AUTO:
			{
				if( !kPacket.Pop(iCustomData1) )
				{
					return E_FAIL;
				}
			}break;
		case MMST_SELECT:
			{
				if( !kPacket.Pop(iCustomData1) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(iCustomData2) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(iCustomData3) )
				{
					return E_FAIL;
				}
			}break;
		default:
			{
				return E_FAIL;
			}
		}

		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_ANS_MISSION_RANK_INPUTMEMO(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kMemoID;
	std::wstring wstrMemo;

	if(kPacket.Pop(kMemoID))
	if(kPacket.Pop(wstrMemo, MAX_MISSION_RANK_INPUTMEMO_LEN))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_C_NFY_MISSION_CLOSE(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_MSN_FRIENDCOMMAND(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cCommandType;	

	if(kPacket.Pop(cCommandType))
	{
		switch(cCommandType)
		{
		case FCT_ADD_BYNAME:
		case FCT_ADD_BYGUID:
		case FCT_ADD_ACCEPT:
		case FCT_ADD_REJECT:
		case FCT_MODIFY_CHATSTATUS:
		case FCT_DELETE:
		case FCT_GROUP:
		case FCT_MODIFY:
			{
				typedef std::vector< SFriendItem > ContFriendVec;
				ContFriendVec kFriendVec;
				if( !PU::TLoadArray_M(kPacket, kFriendVec, FM_MAXFRIEND) )
				{
					return E_FAIL;
				}
			}break;
		case FCT_LINKSTATUS:
			{
				BYTE cLinkStatus = 0;
				
				if( !kPacket.Pop(cLinkStatus) )
				{
					return E_FAIL;
				}
			}break;
		case FCT_FULL_DATA:
			{
			}break;
		default:
			{
				return E_FAIL;
			}break;
		}

		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_COUPLE_COMMAND(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cCmdType;
	
	if(kPacket.Pop(cCmdType))
	{
		switch(cCmdType)
		{
		case CC_Req_Couple:
			{
				bool bByName = false;
				bool bInstance = false;
				BM::GUID kAnswererGuid;
		
				if( !kPacket.Pop(bByName) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(bInstance) )
				{
					return E_FAIL;
				}

				if( !bByName )
				{
					if( !kPacket.Pop(kAnswererGuid) )
					{
						return E_FAIL;
					}
				}
				else
				{
					std::wstring kAnswererName;
					
					if( !kPacket.Pop(kAnswererName, MAX_CHARACTER_NAME_LEN) )
					{
						return E_FAIL;
					}
				}
			}break;
		case CC_Ans_Couple:
			{
				bool bSayYes = false;
				bool bInstance = false;
				BM::GUID kRequesterGuid;

				if( !kPacket.Pop(kRequesterGuid) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(bSayYes) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(bInstance) )
				{
					return E_FAIL;
				}
			}break;
		case CC_Req_Find:	
		case CC_Req_Break:	
		case CC_Req_Info:
			{
			}break;
		case CC_Req_Warp:
			{
				int iRecvItemNo = 0;
				
				if( !kPacket.Pop(iRecvItemNo) )
				{
					return E_FAIL;
				}
			}break;
		case CC_Req_Warp1:
		case CC_Req_Warp2:
		case CC_Ans_Reject:
		case CC_Ans_Notify:
			{
				POINT3 pt3TargetPos;
				SGroundKey kTargetKey;

				if( !kPacket.Pop(pt3TargetPos) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(kTargetKey) )
				{
					return E_FAIL;
				}
			}break;
		case CC_Req_SweetHeart_Quest:
			{
				int iQuestID = 0;
				BM::GUID kAnswererGuid;
		
				if( !kPacket.Pop(kAnswererGuid) )
				{
					return E_FAIL;
				}				
				if( !kPacket.Pop(iQuestID) )
				{
					return E_FAIL;
				}
			}break;
		case CC_Ans_SweetHeart_Quest:
			{
				int iQuestID = 0;
				bool bSayYes = false;
				BM::GUID kRequesterGuid;

				if( !kPacket.Pop(kRequesterGuid) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(iQuestID) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(bSayYes) )
				{
					return E_FAIL;
				}
			}break;
		case CC_Req_SweetHeart_Complete:
			{
				BM::GUID kCoupleGuid;
				int iQuestID = 0;

				if( !kPacket.Pop(kCoupleGuid) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(iQuestID) )
				{
					return E_FAIL;
				}
			}break;
		case CC_Ans_SweetHeart_Complete:
			{
				bool bSayYes = false;
				BM::GUID kRequesterGuid;
				int iQuestID = 0;

				if( !kPacket.Pop(kRequesterGuid) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(bSayYes) )
				{
					return E_FAIL;
				}
				if( !kPacket.Pop(iQuestID) )
				{
					return E_FAIL;
				}
			}break;
		default:
			{
				return E_FAIL;
			}break;
		}

		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REWORD_FRAN_EXP(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid;

	if(kPacket.Pop(kNpcGuid))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_JOIN_PARTY(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cJoinType = 0;

	if(kPacket.Pop(cJoinType))
	{
		switch(cJoinType)
		{
		case PCT_REQJOIN_CHARGUID:
			{
				BM::GUID kUserGuid;

				if( !kPacket.Pop(kUserGuid) )
				{
					return E_FAIL;
				}
			}break;
		case PCT_REQJOIN_CHARNAME:
			{
				std::wstring kCharName;

				if( !kPacket.Pop(kCharName, MAX_CHARACTER_NAME_LEN) )
				{
					return E_FAIL;
				}
			}break;
		case PCT_REQJOIN_MEMBERGUID:
			{
				BM::GUID kUserGuid;

				if( !kPacket.Pop(kUserGuid) )
				{
					return E_FAIL;
				}
			}break;
		default:
			{
				return E_FAIL;
			}break;
		}
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_ANS_JOIN_PARTY(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bAnsJoin = false;
	BM::GUID kPartyGuid;

	if(kPacket.Pop(bAnsJoin))
	if(kPacket.Pop(kPartyGuid))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_LEAVE_PARTY(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bLeave;

	if(kPacket.Pop(bLeave))
	if(BM::IsCorrectType(bLeave))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_PARTY_CHANGE_MASTER(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNewMasterGuid;

	if(kPacket.Pop(kNewMasterGuid))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_PARTY_CHANGE_OPTION(BM::Stream &kPacket)
{//OK. Lim 090408
	std::wstring kNewPartyName;
	bool bClear = false;
	
	if(kPacket.Pop(kNewPartyName, MAX_PARTYNAME_LEN))
	{
		SPartyOption kNewOption;

		if(kPacket.Pop(bClear))
		if(BM::IsCorrectType(bClear))
		{
			if(kNewOption.ReadFromPacket(kPacket))
			{
				if(!kPacket.RemainSize())
				{
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_KICKOUT_PARTY_USER(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cCommandType = 0;

	if(kPacket.Pop(cCommandType))
	{
		switch(cCommandType)
		{
		case PCT_KICK_CHARGUID:
			{
				BM::GUID kCharGuid;

				if( !kPacket.Pop(kCharGuid) )
				{
					return E_FAIL;
				}
			}break;
		case PCT_KICK_CHARNAME:
			{
				std::wstring kCharName;

				if( !kPacket.Pop(kCharName, MAX_CHARACTER_NAME_LEN) )
				{
					return E_FAIL;
				}
			}break;
		default:
			{
				return E_FAIL;
			}break;
		}
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_CREATE_PARTY(BM::Stream &kPacket)
{//OK. Lim 090408
	std::wstring	kNewPartyName;
	SPartyOption	kNewOption;

	if(kPacket.Pop(kNewPartyName, MAX_PARTYNAME_LEN))
	{
		if(kNewOption.ReadFromPacket(kPacket))
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_PARTY_RENAME(BM::Stream &kPacket)
{//OK. Lim 090408
	std::wstring kNewPartyName;
	bool bClear = false;
	
	if(kPacket.Pop(kNewPartyName, MAX_PARTYNAME_LEN))
	{
		if(kPacket.Pop(bClear))
		if(BM::IsCorrectType(bClear))
		{
			if(!kPacket.RemainSize())
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_JOIN_PARTYFIND(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cJoinType = 0;

	if(kPacket.Pop(cJoinType))
	{
		switch(cJoinType)
		{
		case PCT_REQJOIN_CHARGUID:
			{
				BM::GUID kUserGuid;

				if( !kPacket.Pop(kUserGuid) )
				{
					return E_FAIL;
				}
			}break;
		case PCT_REQJOIN_CHARNAME:
			{
				std::wstring kCharName;

				if( !kPacket.Pop(kCharName, MAX_CHARACTER_NAME_LEN) )
				{
					return E_FAIL;
				}
			}break;
		case PCT_REQJOIN_MEMBERGUID:
			{
				BM::GUID kUserGuid;

				if( !kPacket.Pop(kUserGuid) )
				{
					return E_FAIL;
				}
			}break;
		default:
			{
				return E_FAIL;
			}break;
		}
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_ANS_JOIN_PARTYFIND(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kUserGuid;
	bool bAnsJoin = false;
	BM::GUID kPartyGuid;
	SGroundKey GndKey;

	if(kPacket.Pop(kUserGuid))
	if(kPacket.Pop(bAnsJoin))
	if(BM::IsCorrectType(bAnsJoin))
	if(kPacket.Pop(kPartyGuid))
	if(kPacket.Pop(GndKey))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MISSION_GADACOIN_ITEM(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MON_BLOWSTATUS(BM::Stream &kPacket)
{
	BM::GUID guidMonster;
	POINT3 pt3MonsterPos;
	bool bFirstFloor = false;
	bool bNoUseStandUpTime = false;

	if( kPacket.Pop(guidMonster) )
	if( kPacket.Pop(pt3MonsterPos) )
	if( kPacket.Pop(bFirstFloor) )
	if(BM::IsCorrectType(bFirstFloor))
	{
		if( kPacket.Pop(bNoUseStandUpTime) )
		if( BM::IsCorrectType(bNoUseStandUpTime) )
		{
			if( !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MARRY_COMMAND(BM::Stream &kPacket)
{
	BYTE cCmdType;
	
	if(kPacket.Pop(cCmdType))
	{
		switch(cCmdType)
		{
		case MC_ReqMarry:
		case MC_ReqSendMoney:
			{
				int iGoldMoney = 0;
		
				if( !kPacket.Pop(iGoldMoney) )
				{
					return E_FAIL;
				}
			}break;
		case MC_SetActionState:
			{
				int iType = 0;

				if( !kPacket.Pop(iType) )
				{
					return E_FAIL;
				}
			}break;
		case MC_TotalMoney:
			{
			}break;
		default:
			{
				return E_FAIL;
			}break;
		}

		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HIDDEN_ITEM(BM::Stream &kPacket)
{
	int iItemNo = 0;
	SItemPos rkItemInvPos;

	if(kPacket.Pop(iItemNo))
	if(kPacket.Pop(rkItemInvPos))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HIDDEN_REWORDITEM(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HIDDEN_GIVE_REWORDITEM(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HIDDEN_ITEM_PACK(BM::Stream &kPacket)
{
	SItemPos rkItemInvPos;

	if(kPacket.Pop(rkItemInvPos))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_DEFENCE_STAGE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_DEFENCE_WAVE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_DEFENCE_ENDSTAGE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_DEFENCE_DIRECTION(BM::Stream &kPacket)
{
	BYTE kDirection = 0;

	if(kPacket.Pop(kDirection))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFENCE_INFALLIBLE_SELECTION(BM::Stream &kPacket)
{
	BYTE kDirection = 0;
	SItemPos rkItemInvPos;

	if(kPacket.Pop(kDirection))
	if(kPacket.Pop(rkItemInvPos))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFENCE_TIMEPLUS(BM::Stream &kPacket)
{
	SItemPos rkItemInvPos;

	if(kPacket.Pop(rkItemInvPos))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFENCE_SELECT_CLOSE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFENCE_POTION(BM::Stream &kPacket)
{
	SItemPos rkItemInvPos;

	if(kPacket.Pop(rkItemInvPos))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_GET_ENTRANCE_OPEN_GUILD(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_GUILD_ENTRANCE(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	std::wstring kCharName;
	short sLevel = 0;
	BYTE byClass = 0;
	std::wstring wStr;
	__int64 iGuildEntranceFee;
	
	if( kPacket.Pop(kGuid) )
	if( kPacket.Pop(kCharName) )
	if( kPacket.Pop(sLevel) )
	if( kPacket.Pop(byClass) )
	if( kPacket.Pop(wStr) )
	if( kPacket.Pop(iGuildEntranceFee))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_N_C_REQ_PAYMENT_GUILD_ENTRANCE_FEE(BM::Stream &kPacket)
{
	BM::GUID kGuildGuid;
	__int64 iMoney;

	if(kPacket.Pop(kGuildGuid))
	if(kPacket.Pop(iMoney))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_GUILD_ENTRANCE_CANCEL(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GUARDIAN_SET(BM::Stream &kPacket)
{
	int iGuardianNo = 0;
	std::wstring kTriggerName;

	if(kPacket.Pop(iGuardianNo))
	if(kPacket.Pop(kTriggerName))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GUARDIAN_UPGRADE(BM::Stream &kPacket)
{
	int iGuardianNo = 0;
	std::wstring kTriggerName;

	if(kPacket.Pop(iGuardianNo))
	if(kPacket.Pop(kTriggerName))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFENCE_ITEM_USE(BM::Stream &kPacket)
{
	int iItemNo = 0;
	SItemPos kItemPos;
	int iOption = 0;

	if(kPacket.Pop(iItemNo))
	if(kPacket.Pop(kItemPos))
	if(kPacket.Pop(iOption))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFENCE_SKILL_USE(BM::Stream &kPacket)
{
    int iGuardianNo = 0;
	int iSkillNo = 0;
	BM::GUID rkGuardianGuid;

    if(kPacket.Pop(iGuardianNo))
	if(kPacket.Pop(iSkillNo))
	if(kPacket.Pop(rkGuardianGuid))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFENCE_PARTYLIST(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GUARDIAN_REMOVE(BM::Stream &kPacket)
{
	std::wstring kTriggerName;

	if(kPacket.Pop(kTriggerName))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MISSION_START(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_MISSION_NEXT_STAGE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GENMONSTER_GROUP_NUM(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_CREATE_EXPEDITION(BM::Stream &kPacket)
{
	BM::GUID kCharGuid, NpcGuid;
	SExpeditionOption kOption;
	std::wstring kName;

	if( kPacket.Pop(kCharGuid) )
	{
		if( kPacket.Pop(NpcGuid) )
		{
			if( kOption.ReadFromPacket(kPacket) )
			{
				if( kPacket.Pop(kName) )
				{
					if( !kPacket.RemainSize() )
					{
						return S_OK;
					}
				}
			}
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_DISPERSE_EXPEDITION(BM::Stream &kPacket)
{
	BM::GUID kCharGuid;
	if( kPacket.Pop(kCharGuid) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_INFO_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid;
	if( Packet.Pop(ExpeditionGuid) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_JOIN_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID CharGuid, ExpeditionGuid;
	if( Packet.Pop(CharGuid) )
	{
		if( Packet.Pop(ExpeditionGuid) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_LEAVE_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid, CharGuid;
	if( Packet.Pop(ExpeditionGuid) )
	{
		if( Packet.Pop(CharGuid) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_ANS_JOIN_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid, CharGuid;
	bool bAns;
	if( Packet.Pop(ExpeditionGuid) )
	{
		if( Packet.Pop(CharGuid) )
		{
			if( Packet.Pop(bAns) )
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

HRESULT CALLBACK Check_PT_C_M_REQ_LIST_USER_EXPEDITION(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_INVITE_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid;
	VEC_GUID GuidList;

	if( Packet.Pop(ExpeditionGuid) )
	{
		if( Packet.Pop(GuidList) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_ANS_INVITE_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid, CharGuid;
	bool bAnsJoin;
	
	if( Packet.Pop(ExpeditionGuid) )
	{
		if( Packet.Pop(ExpeditionGuid) )
		{
			if( Packet.Pop(bAnsJoin) )
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

HRESULT CALLBACK Check_PT_C_N_REQ_TEAM_MOVE_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid, CharGuid;
	int iTeam;

	if( Packet.Pop(ExpeditionGuid) )
	{
		if( Packet.Pop(CharGuid) )
		{
			if( Packet.Pop(iTeam ) )
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

HRESULT CALLBACK Check_PT_C_N_REQ_CHANGEMASTER_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid, NewMasterGuid;

	if( Packet.Pop(ExpeditionGuid) )
	{
		if( Packet.Pop(NewMasterGuid) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_RENAME_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid;
	std::wstring NewName;

	if( Packet.Pop(ExpeditionGuid) )
	{
		if( Packet.Pop(NewName) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_CHANGEOPTION_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid;
	SExpeditionOption Option;

	if( Packet.Pop(ExpeditionGuid) )
	{
		Option.ReadFromPacket(Packet);
		if( !Packet.RemainSize() ) 
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_KICKOUT_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid, KickCharGuid;

	if( Packet.Pop(ExpeditionGuid) )
	{
		if( Packet.Pop(KickCharGuid) )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_NPC_ENTER_EXPEDITION(BM::Stream & Packet)
{
	BM::GUID NpcGuid;

	if( Packet.Pop(NpcGuid) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_EXIT_EXPEDITION_COMPLETE(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_TENDER_ITEM(BM::Stream & Packet)
{
	BM::GUID CharGuid;
	
	if( Packet.Pop(CharGuid) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_GIVEUP_TENDER(BM::Stream & Packet)
{
	BM::GUID CharGuid;
	
	if( Packet.Pop(CharGuid) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_LIST_EXPEDITION(BM::Stream & Packet )
{
	int Continent;
	if( Packet.Pop(Continent) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ALARM_MINIMAP(BM::Stream & Packet)
{
	POINT2 Pos;
	if( Packet.Pop(Pos) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_TRIGGER_ONENTER(BM::Stream & Packet)
{
	std::string TriggerName;
	if( Packet.Pop(TriggerName) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_TRIGGER_ONLEAVE(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REFRESH_NEED_ITEM_INFO(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_OPEN_DEFENCE_WINDOW(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ENTER_DEFENCE(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXCHANGE_ACCUMPOINT_TO_STRATEGICPOINT(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CANCLE_DEFENCE(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_POINT_COPY(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_KILL_MONSTER_BY_ACCUMPOINT(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SCORE_TRIGGER_ONENTER(BM::Stream & Packet)
{
	std::string TriggerName;
	if( Packet.Pop(TriggerName) )
	{
		if( !Packet.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}