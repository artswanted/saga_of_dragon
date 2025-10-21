#include "stdafx.h"
#include "BM/Stream.h"
#include "Lohengrin/Packetstruct.h"
#include "variant/PgPlayer.h"
#include "Lohengrin/PacketStruct4Map.h"
#include "Variant/ClientOption.h"
#include "variant/PgBattleSquare.h"
#include "Variant/LimitClass.h"
#include "Variant/PgConstellation.h"

int const iMaxChatLen = 200; // 임시로 최대 채팅 글자수는 200자로 제한 한다.

// 1차
HRESULT CALLBACK Check_PT_C_M_ANS_SHOWDIALOG(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kSessionGuid;
	BM::GUID kObjectGuid;
	int iQuestID = 0;
	int iDialogID = 0;
	int iNextDialogID = 0;
	int iSelect1 = 0;
	int iSelect2 = 0;

	bool bRet = kPacket.Pop(kSessionGuid);
	bRet = (bRet && kPacket.Pop(kObjectGuid));
	bRet = (bRet && kPacket.Pop(iQuestID));
	bRet = (bRet && kPacket.Pop(iDialogID));
	bRet = (bRet && kPacket.Pop(iNextDialogID));
	bRet = (bRet && kPacket.Pop(iSelect1));
	bRet = (bRet && kPacket.Pop(iSelect2));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ACTOR_SIMULATE(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bSimulate = false;
	POINT3 kNewPos;

	bool bRet = kPacket.Pop( bSimulate );
	bRet = (bRet && kPacket.Pop( kNewPos ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CHAT(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cChatMode = 0;
	DWORD dwColor = 0;
	std::wstring kChatText;

	if ( !kPacket.Pop( cChatMode ) )
	{
		return E_FAIL;
	}

	int const iOneItemLinkPerAddedTextCount = 44; //아이템 링크 시 스크립트 문장 길이 기존 36에서 44로 변경되어 수정함
	if ( !kPacket.Pop( kChatText, (iMaxChatLen + (iMaxChatLen * iOneItemLinkPerAddedTextCount)) ))
	{
		return E_FAIL;
	}

	if ( !kPacket.Pop( dwColor ) )
	{
		return E_FAIL;
	}

	unsigned int iCount = 0;
	if (	!kPacket.Pop( iCount )
		||	iMaxChatLen < iCount
		)
	{
		return E_FAIL;
	}

	unsigned int iCur = 0;
	unsigned int iCount2 = 0;
	for( ; iCount > iCur; ++iCur )
	{
		if (	!kPacket.Pop( iCount2 )
			||	iMaxChatLen < iCount2
			)
		{
			return E_FAIL;
		}

		unsigned int iCur2 = 0;
		for( ; iCount2 > iCur2; ++iCur2 )
		{
			std::wstring	m_kType;
			std::wstring	m_kValue;
			std::vector<BYTE>	m_kBinaryValue;

			int const iOnlySizeUse = 2;
			int const iNotUseNow = 0;
			if (	!kPacket.Pop( m_kType, iOnlySizeUse )
				||	!kPacket.Pop( m_kValue,  iNotUseNow )
				||	!kPacket.Pop( m_kBinaryValue, iOnlySizeUse )
				)
			{
				return E_FAIL;
			}
		}
	}

	int iAddValue = 0;
	if( iCount > 10 )
	{
		iAddValue = iCount - 10;
	}

	if( iMaxChatLen < (kChatText.size() - ((iCount * iOneItemLinkPerAddedTextCount) + iAddValue)) )
	{
		return E_FAIL;
	}

	bool bRet = true;
	switch( cChatMode )
	{
	case CT_WHISPER_BYGUID:
	case CT_MYHOME_PRIVATE:
		{
			BM::GUID kTempGuid;
			bRet = (bRet && kPacket.Pop( kTempGuid ));
		}break;
	case CT_WHISPER_BYNAME:
		{
			std::wstring kTempStr;
			bRet = (bRet && kPacket.Pop( kTempStr, MAX_CHARACTER_NAME_LEN ));
		}break;
	case CT_MYHOME_PUBLIC:
	case CT_NORMAL:
	case CT_PARTY:
	case CT_FRIEND:
	case CT_GUILD:
	case CT_TEAM:
	case CT_EMOTION:
	case CT_TRADE:
		{
		}break;
	case CT_MANTOMAN:
		{
			BYTE cType = 0;
			bRet = (bRet && kPacket.Pop( cType ));
			if( 0 == cType )
			{
				BM::GUID kGuid;
				bRet = (bRet && kPacket.Pop( kGuid ));
			}
			else
			{
				std::wstring kTempStr;
				bRet = (bRet && kPacket.Pop( kTempStr, MAX_CHARACTER_NAME_LEN ));
			}
			BYTE cType2 = 0;
			bRet = (bRet && kPacket.Pop( cType2 ));
		}break;
	default:
		{
			return E_FAIL;
		}break;
	}

	if( (true == bRet)
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CHAT_INPUTNOW(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cChatMode = 0;
	bool bPopup = false;

	bool bRet = kPacket.Pop( cChatMode );
	switch( cChatMode )
	{
	case CT_NORMAL:
	case CT_PARTY:
	case CT_GUILD:
		{
		}break;
	default:
		{
			return E_FAIL;
		}break;
	}
	bRet = (bRet && kPacket.Pop( bPopup ));

	if( !BM::IsCorrectType( bPopup ) )
	{
		return E_FAIL;
	}

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DROPQUEST(BM::Stream &kPacket)
{//OK. Lim 090408
	int iQuestID = 0;

	bool bRet = kPacket.Pop( iQuestID );
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_END_DAILYQUEST(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid;
	int iQuestID = 0;

	bool bRet = kPacket.Pop( kNpcGuid );
	bRet = (bRet && kPacket.Pop( iQuestID ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_END_EVENT_SCRIPT(BM::Stream &kPacket)
{//OK. Lim 090408
	int iEventScriptID = 0;

	bool bRet = kPacket.Pop( iEventScriptID );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ENTIRE_SYNC_TIME(BM::Stream &kPacket)
{//OK. Lim 090408
	if( 0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_INTROQUEST(BM::Stream &kPacket)
{//OK. Lim 090408
	int iQuestID = 0;
	BM::GUID kObjectID;
	bool bRemoteAccept = false;

	bool bRet = kPacket.Pop( iQuestID );
	bRet = (bRet && kPacket.Pop( kObjectID ));
	bRet = (bRet && kPacket.Pop( bRemoteAccept ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_PACTION(BM::Stream &kPacket)
{//OK. Lim 090408
	SProjectileAction kAction;
	float fLifeTime = 0;
	BYTE cTargetNum = 0;
	BM::GUID kTempGuid;
	BYTE bySphereIndex = 0;

	bool bRet = kPacket.Pop( kAction );
	bRet = (bRet && kPacket.Pop( fLifeTime ));
	bRet = (bRet && kPacket.Pop( cTargetNum ));
	if( MAX_SKILL_TARGETNUM < cTargetNum )
	{
		return E_FAIL;
	}

	BYTE cCur = 0;
	for( ; bRet && cTargetNum > cCur; ++cCur )
	{
		bRet = (bRet && kPacket.Pop( kTempGuid ));
		bRet = (bRet && kPacket.Pop( bySphereIndex ));
	}

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RUN_EVENT_SCRIPT(BM::Stream &kPacket)
{//OK. Lim 090408
	int iEventScriptID = 0;

	bool bRet = kPacket.Pop( iEventScriptID );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_UPDATE_DIRECTION(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE byDirection = 0;
	DWORD dwActionTerm = 0;
	POINT3BY ptDirection;
	POINT2BY ptPathNormal;
	POINT3 kCurPos;

	bool bRet = kPacket.Pop(byDirection);
	bRet = (bRet && kPacket.Pop(ptDirection));
	bRet = (bRet && kPacket.Pop(ptPathNormal));
	bRet = (bRet && kPacket.Pop(dwActionTerm));
	bRet = (bRet && kPacket.Pop(kCurPos));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_SAVE_OPTION(BM::Stream &kPacket)
{//OK. Lim 090408
	if( sizeof(ClientOption) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CLIENT_CUSTOMDATA(BM::Stream &kPacket)
{
	int const iSize = sizeof(EClientCustomDataType) + sizeof(int);
	if( iSize == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_SAVE_SKILLSET(BM::Stream &kPacket)
{
	PgSaveSkillSetPacket kSavePacket;
	
	bool bRet = kSavePacket.ReadFromPacket(kPacket);
	
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_GUILD_COMMAND(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cCommand = 0;

	bool bRet = kPacket.Pop( cCommand );

	switch( cCommand )
	{
	case GC_Leave:
	case GC_PreCreate_Test:
		{
		}break;
	case GC_PreCreate:
		{
			std::wstring kTempStr;
			bRet = (bRet && kPacket.Pop( kTempStr, MAX_GUILDNAME_LEN ));
		}break;
	case GC_Create:
		{
			std::wstring kTempStr;
			BYTE cEmblem = 0;
			bRet = (bRet && kPacket.Pop( kTempStr, MAX_GUILDNAME_LEN ));
			bRet = (bRet && kPacket.Pop( cEmblem ));
		}break;
	case GC_M_ChangeOwner:
	case GC_M_Destroy:
	case GC_OtherInfo:
	case GC_Info:
	case GC_List:
	case GC_GuildEntranceApplicant_List:
	case GC_ReqJoin:
	case GC_M_Kick:
	case GC_GuildEntranceApplicant_Accept:
	case GC_GuildEntranceApplicant_Reject:
	case GC_InventoryClose:
	case GC_InventoryMoney:
		{
			BM::GUID kTempGuid;
			bRet = (bRet && kPacket.Pop( kTempGuid ));
		}break;
	case GC_InventoryOpen:
		{
			BM::GUID kTempGuid;
			BYTE byInvType = 0;

			bRet = (bRet && kPacket.Pop(kTempGuid));
			bRet = (bRet && kPacket.Pop(byInvType));
		}break;	
	case GC_ReqJoin_Name:
		{
			std::wstring kTempStr;
			bRet = (bRet && kPacket.Pop( kTempStr, MAX_CHARACTER_NAME_LEN ));
		}break;
	case GC_Join:
		{
			BM::GUID kTempGuid;
			bool bAns = false;
			bRet = (bRet && kPacket.Pop( kTempGuid ));
			bRet = (bRet && kPacket.Pop( bAns ));
			if( !BM::IsCorrectType(bAns) )
			{
				return E_FAIL;
			}
		}break;
	case GC_M_AddSkill:
		{
			int iSkillNo = 0;
			bool bTest = false;

			bRet = (bRet && kPacket.Pop( iSkillNo ));
			if( 0 > iSkillNo )
			{
				return E_FAIL;
			}
			bRet = (bRet && kPacket.Pop( bTest ));
			if( !BM::IsCorrectType(bTest) )
			{
				return E_FAIL;
			}
		}break;
	case GC_M_LvUp:
	case GC_M_InventoryCreate:
		{
			bool bTest = false;
			bRet = (bRet && kPacket.Pop( bTest ));
			if( !BM::IsCorrectType(bTest) )
			{
				return E_FAIL;
			}
		}break;
	case GC_M_InventorySupervision:
		{
			BYTE byAuthority_In = 0;
			BYTE byAuthority_Out = 0;
			bRet = (bRet && kPacket.Pop( byAuthority_In ));
			bRet = (bRet && kPacket.Pop( byAuthority_Out ));
		}break;
	case GC_InventoryLog:
		{
			BM::GUID kGuildGuid;
			int iPage = 0;

			bRet = (bRet && kPacket.Pop( kGuildGuid ));
			bRet = (bRet && kPacket.Pop( iPage ));
		}break;
	case GC_InventoryUpdate:
		{
			BM::GUID kGuildGuid;
			int iType;			
			__int64 i64Money = 0i64;
			SItemPos kCasterPos;
			SItemPos kTargetPos;
			DWORD dwTime;
			BM::GUID kGuid;

			bRet = (bRet && kPacket.Pop( kGuildGuid ));
			bRet = (bRet && kPacket.Pop( iType ));

			switch( iType )
			{
			case EGIT_MONEY_IN:
			case EGIT_MONEY_OUT:
				{
					bRet = (bRet && kPacket.Pop( i64Money ));
				}break;
			default:
				{
					bRet = (bRet && kPacket.Pop( kCasterPos ));
					bRet = (bRet && kPacket.Pop( kTargetPos ));
					bRet = (bRet && kPacket.Pop( dwTime ));
					bRet = (bRet && kPacket.Pop( kGuid ));
				}break;
			}
		}break;
	case GC_M_MemberGrade:
		{
			BM::GUID kTempGuid;
			BYTE cNewGrade = 0;
			bRet = (bRet && kPacket.Pop( kTempGuid ));
			bRet = (bRet && kPacket.Pop( cNewGrade ));
		}break;
	case GC_M_Notice:
		{
			std::wstring kTempStr;
			bRet = (bRet && kPacket.Pop( kTempStr, iNoticeMaxCharacter ));
		}break;
	case GC_ReserveBattleForEmporia:
		{
			if ( true == bRet )
			{
				bRet = false;
				bool bThrow;
				if ( true == kPacket.Pop( bThrow ) )
				{
					if ( true == BM::IsCorrectType(bThrow) )
					{
						if ( !bThrow )
						{
							// 도전일때는 패킷이 틀리다.
							BM::GUID kEmporiaID;
							__int64 i64Exp = 0i64;
							if ( true == kPacket.Pop( kEmporiaID ) )
							{
								if ( true == kPacket.Pop( i64Exp ) )
								{
									bRet = true;
								}
							}
						}
						else
						{
							bRet = true;
						}
					}
				}
			}
		}break;
	case GC_M_AuthOwner:
	case GC_M_AuthMaster:
		{
			BM::Stream::DEF_STREAM_TYPE usType = 0;

			bRet = (bRet && kPacket.Pop( usType ));

			switch( usType )
			{
			case PT_C_M_REQ_ITEM_ACTION:
				{
					SItemPos kInvenPos;
					DWORD dwElapsedTime = 0;
					BYTE cNewGuildMark = 0;

					bRet = (bRet && kPacket.Pop( kInvenPos ));
					bRet = (bRet && kPacket.Pop( dwElapsedTime ));
					bRet = (bRet && kPacket.Pop( cNewGuildMark ));
				}break;
			default:
				{
					return E_FAIL;
				}break;
			}
		}break;
	case GC_SetMercenary:
		{
			PgLimitClass kLimitClass;
			bRet = ( bRet && kLimitClass.ReadFromPacket( kPacket ) );
		}break;
	case GC_SetGuildEntranceOpen:
		{
			SSetGuildEntrancedOpen kGuildEntranceOpen;
			bRet = ( bRet && kGuildEntranceOpen.ReadFromPacket( kPacket ) );
		}break;
	default:
		{
			return E_FAIL;
		}break;
	}

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_CHAT_NOTICE(BM::Stream &kPacket)
{//OK. Lim 090408
	if( 0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}


// 2차 작업 분
HRESULT CALLBACK Check_PT_C_M_REQ_FOLLOWING(BM::Stream &kPacket)
{//OK. Lim 090408
	EPlayer_Follow_Mode eMode = EFollow_None;
	BM::GUID kHeadGuid;
	bool bFriend;
	
	bool bRet = kPacket.Pop( eMode );
	bRet = (bRet && kPacket.Pop( kHeadGuid ));
	bRet = (bRet && kPacket.Pop( bFriend ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_LEARN_SKILL(BM::Stream &kPacket)
{//OK. Lim 090408
	int iSkillNo = 0;
	bool bRet = kPacket.Pop( iSkillNo );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_BEGINCAST(BM::Stream &kPacket)
{//OK. Lim 090408
	int iSkillNo = 0;
	DWORD dwCurrentTime;
	bool bRet = kPacket.Pop( iSkillNo );
	bRet = (bRet && kPacket.Pop( dwCurrentTime ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DELETEEFFECT(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kCharGuid;
	int iEffectNo = 0;

	bool bRet = kPacket.Pop( kCharGuid );
	bRet = (bRet && kPacket.Pop( iEffectNo ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_O_REQ_GMCHAT(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kGMGUid;
	BM::GUID kPetitionGuid;
	std::wstring kSender;
	std::wstring kText;

	bool bRet = kPacket.Pop( kGMGUid );
	bRet = (bRet && kPacket.Pop( kPetitionGuid ));
	bRet = (bRet && kPacket.Pop( kSender, MAX_CHARACTER_NAME_LEN ));
	bRet = (bRet && kPacket.Pop( kText, iMaxChatLen ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_GM_REQ_RECEIPT_PETITION(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kMembGuid;
	std::wstring kName;
	std::wstring kTitle;
	std::wstring kNote;

	int const iMaxTitleLen = 30;
	int const iMaxNoteLen = 1000;
	bool bRet = kPacket.Pop( kMembGuid );
	bRet = (bRet && kPacket.Pop( kName, MAX_CHARACTER_NAME_LEN ));
	bRet = (bRet && kPacket.Pop( kTitle, iMaxTitleLen ));
	bRet = (bRet && kPacket.Pop( kNote, iMaxNoteLen ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_GM_REQ_REMAINDER_PETITION(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kTempGuid;

	bool bRet = kPacket.Pop( kTempGuid );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CHECK_WORLDEVENT_CONDITION(BM::Stream &kPacket)
{//OK. Lim 090408
	WORD kTemp = 0;
	POINT3 kTempPos;

	bool bRet = kPacket.Pop( kTemp );
	bRet = bRet && kPacket.Pop( kTempPos );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_HACKSHIELD_CALLBACK(BM::Stream &kPacket)
{//OK. Lim 090408
	long lCode = 0;
	std::wstring kMsg;

	size_t const iMaxDescriptionLength = 2048;

	bool bRet = kPacket.Pop( lCode );
	bRet = (bRet && kPacket.Pop( kMsg, iMaxDescriptionLength ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_REALM_MERGE(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kCharGuid;
	std::wstring kNewName;

	bool bRet = kPacket.Pop( kCharGuid );
	bRet = (bRet && kPacket.Pop( kNewName, MAX_CHARACTER_NAME_LEN ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DELETE_BUFFEFFECT(BM::Stream &kPacket)
{
	BM::GUID kCharGuid;
	int iEffectNo = 0;

	bool bRet = kPacket.Pop( kCharGuid );
	bRet = (bRet && kPacket.Pop( iEffectNo ));

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EVENT_TW_USE_EFFECTQUEST(BM::Stream &kPacket)
{
	BM::GUID kNpcGuid;

	bool bRet = kPacket.Pop( kNpcGuid );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RAGNAROK_EFFECT(BM::Stream &kPacket)
{
	BM::GUID NpcGuid;
	int EffectNo;

	if( true == kPacket.Pop(NpcGuid) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SHAREQUEST(BM::Stream &kPacket)
{
	int iTemp = 0;

	bool bRet = kPacket.Pop( iTemp );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ACCEPT_SHAREQUEST(BM::Stream &kPacket)
{
	bool bTemp = false;
	int iTemp = 0;
	BM::GUID kTemp;

	bool bRet = kPacket.Pop( bTemp );
	bRet = bRet && kPacket.Pop( iTemp );
	bRet = bRet && kPacket.Pop( kTemp );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_COMPLETE_QUEST(BM::Stream &kPacket)
{
	int iTemp = 0;
	bool bRet = kPacket.Pop( iTemp );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EVENT_QUEST_TALK(BM::Stream &kPacket)
{
	BM::GUID kTempGuid;
	bool bRet = kPacket.Pop( kTempGuid );

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_WANT_JOIN_BS(BM::Stream &kPacket)
{
	EBattleSquareTeam eTeam = BST_NONE;
	bool bRet = kPacket.Pop( eTeam );
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_WANT_JOIN_BS_CHANNEL(BM::Stream &kPacket)
{
	int iTemp = 0;

	bool bRet = kPacket.Pop( iTemp );
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_RANDOMQUEST_BUILD(BM::Stream &kPacket)
{
	if( 0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_RANDOMTACTICSQUEST_BUILD(BM::Stream &kPacket)
{
	if( 0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_WANTEDQUEST_BUILD(BM::Stream &kPacket)
{
	if( 0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_BEGIN_RANDOMQUEST(BM::Stream &kPacket)
{
	int iQuestID = 0;
	bool bRet = kPacket.Pop( iQuestID );
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_END_RANDOMQUEST(BM::Stream &kPacket)
{
	int iQuestID = 0;
	bool bRet = kPacket.Pop( iQuestID );
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_BEGIN_WANTEDQUEST(BM::Stream &kPacket)
{
	int iQuestID = 0;
	bool bRet = kPacket.Pop( iQuestID );
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_END_WANTEDQUEST(BM::Stream &kPacket)
{
	int iQuestID = 0;
	bool bRet = kPacket.Pop( iQuestID );
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_N_REQ_REALM_QUEST_INFO(BM::Stream &kPacket)
{
	std::wstring kTemp;
	bool bRet = kPacket.Pop( kTemp, MAX_PATH );
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_BS_EXIT(BM::Stream &kPacket)
{
	if( 0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_EXIT_SUPER_GROUND(BM::Stream &kPacket)
{
	if( 0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_NPC_TALK_MAP_MOVE(BM::Stream &kPacket)
{
	BM::GUID kNpcGuid;
	int iTemp = 0;
	bool bRet = kPacket.Pop(kNpcGuid);
	bRet = bRet && kPacket.Pop(iTemp);
	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}


HRESULT CALLBACK Check_PT_C_M_REQ_VENDOR_CREATE(BM::Stream &kPacket)
{
	BM::GUID kCharGuid;
	if ( true == kPacket.Pop( kCharGuid ) )
	{
		std::wstring kVendorName;
		if ( true == kPacket.Pop( kVendorName ) )
		{
			if( !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_VENDOR_RENAME(BM::Stream &kPacket)
{
	BM::GUID kCharGuid;
	if ( true == kPacket.Pop( kCharGuid ) )
	{
		std::wstring kVendorName;
		if ( true == kPacket.Pop( kVendorName ) )
		{
			if( !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_VENDOR_DELETE(BM::Stream &kPacket)
{
	BM::GUID kCharGuid;
	if ( true == kPacket.Pop( kCharGuid ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_VENDOR_STATE(BM::Stream &kPacket)
{
	if ( sizeof(BM::GUID) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MY_VENDOR_QUERY(BM::Stream &kPacket)
{
	BM::GUID kOwnerGuid;
	if( true == kPacket.Pop(kOwnerGuid) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_VENDOR_ENTER(BM::Stream &kPacket)
{
	BM::GUID kOwnerGuid;
	if( true == kPacket.Pop(kOwnerGuid) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_VENDOR_REFRESH_QUERY(BM::Stream &kPacket)
{
	BM::GUID kOwnerGuid;
	if( true == kPacket.Pop(kOwnerGuid) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_VENDOR_EXIT(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CANCEL_JOBSKILL(BM::Stream &kPacket)
{
	std::string kTiggerID;
	BM::GUID kPlayerGuid;

    if(kPacket.Pop(kTiggerID))
	if(kPacket.Pop(kPlayerGuid))
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_LEARN_JOBSKILL(BM::Stream &kPacket)
{
	int iSkill = 0;
	if( true == kPacket.Pop(iSkill) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DELETE_JOBSKILL(BM::Stream &kPacket)
{
	int iSkill = 0;
	if( true == kPacket.Pop(iSkill) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_JOBSKILL3_CREATEITEM(BM::Stream &kPacket)
{
	int iItemNo = 0;
	CONT_JS3_RESITEM_INFO kContResItemInfo;
	if( kPacket.Pop(iItemNo)
	&&  kPacket.Pop(kContResItemInfo, MAX_JOBSKILL3_CREATEITEM_RESOURCE_LEN)
	&&  0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_TOGGLSKILL_OFF(BM::Stream &rkPacket)
{
	BM::GUID kGuid;	
	int iSkillNo;
	if( rkPacket.Pop(kGuid)
		&&	rkPacket.Pop(iSkillNo)
	&&	0 == rkPacket.RemainSize()
	)
	{
		return S_OK;
	}	
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ENTER_CONSTELLATION(BM::Stream &rkPacket)
{
	int Position, Difficulty;
	bool UseCashItem;

	if( rkPacket.Pop(Position)
		&& rkPacket.Pop(Difficulty)
		&& rkPacket.Pop(UseCashItem)
		&& 0 == rkPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ENTER_CONSTELLATION_BOSS(BM::Stream &rkPacket)
{
	Constellation::SConstellationKey Key;

	Key.ReadFromPacket(rkPacket);
	if( 0 == rkPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CONSTELLATION_MISSION(BM::Stream & Packet)
{
	if( !Packet.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CONSTELLATION_CASH_REWARD(BM::Stream & Packet)
{
	if( 0 == Packet.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CONSTELLATION_CASH_REWARD_COMPLETE(BM::Stream & Packet)
{
	if( 0 == Packet.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CONSTELLATION_PARTY_LIST(BM::Stream & Packet)
{
	BM::GUID kGuid;
	int Type = 0;
	VEC_INT kContMapNo;
	if(Packet.Pop(kGuid))
	{
		if(Packet.Pop(Type))
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}