#include "StdAfx.h"
#include "PgFriendMgr.h"
#include "Global.h"
#include "PgTask_Contents.h"

PoolFriendItem PgMessenger::m_kPoolFriendItem(FM_InitPool_FriendItem,100);//친구 Item 풀(by CharGuid)

PgFriendMgr::PgFriendMgr(void)
	:	m_kPoolFriendList(FM_InitPool_FriendList)
{
}

PgFriendMgr::~PgFriendMgr(void)
{
	Clear();
}

void PgFriendMgr::Clear()
{
	BM::CAutoMutex kLock_1(m_kMutex);
	ContMessenger::iterator iter = m_kContMsgr.begin();
	while( m_kContMsgr.end() != iter )
	{
		m_kPoolFriendList.Delete((*iter).second);
		++iter;
	}
	m_kContMsgr.clear();
}

PgMessenger* PgFriendMgr::GetMessenger(BM::GUID const &rkGuid) const
{
	BM::CAutoMutex kLock_1(m_kMutex);

	ContMessenger::const_iterator kIter = m_kContMsgr.find(rkGuid);
	if(m_kContMsgr.end() != kIter)
	{
		return (kIter->second);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}
PgMessenger* PgFriendMgr::NewMessenger(BM::GUID const &rkGuid)
{
	BM::CAutoMutex kLock_1(m_kMutex);

	PgMessenger *pkList = m_kPoolFriendList.New();
	if(pkList)
	{
		auto ibRet = m_kContMsgr.insert(std::make_pair(rkGuid, pkList));
		if(ibRet.second)
		{
			return pkList;
		}
		//Failed
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Can't insert map") );
		m_kPoolFriendList.Delete(pkList);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
};

bool PgFriendMgr::DelMessenger(BM::GUID const &rkGuid)
{
	BM::CAutoMutex kLock_1(m_kMutex);

	PgMessenger *pkList = GetMessenger(rkGuid);
	if(pkList)
	{
		m_kContMsgr.erase(rkGuid);
		pkList->Clear();
		return m_kPoolFriendList.Delete(pkList);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
};

bool PgFriendMgr::Recv_Logout(SContentsUser const &rkUser)
{
	BM::CAutoMutex kLock_1(m_kMutex);

	if( rkUser.Empty() )
	{
		//INFO_LOG(BM::LOG_LV1, _T("[%s]-[%d] can't find user (maybe logout)"), __FUNCTIONW__, __LINE__);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMessenger *pkList = GetMessenger(rkUser.kCharGuid);
	if(!pkList)
	{
		//INFO_LOG(BM::LOG_LV1, __FL__ << _T("Critical error: Logined User not found Name=") << rkUser.kName);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//로그아웃 알림
	NfyMySelfToFriend(rkUser, (BYTE)FLS_OFFLINE);
	//
	DelMessenger(rkUser.kCharGuid);

	//INFO_LOG(BM::LOG_LV8, _T("[%s]-[%d] friend list User logout[%s]"), __FUNCTIONW__, __LINE__, rkUser.kName.c_str());

	return true;
}

bool PgFriendMgr::Recv_MapMove(SContentsUser const &rkUser)
{
	BM::CAutoMutex kLock_1(m_kMutex);

	if( rkUser.Empty() )
	{
		//INFO_LOG(BM::LOG_LV1, _T("[%s]-[%d] can't find user (maybe logout)"), __FUNCTIONW__, __LINE__);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMessenger *pkList = GetMessenger(rkUser.kCharGuid);
	if(pkList)
	{
		NfyMySelfToFriend(rkUser, pkList->LinkStatus());
	}

	return true;
}

bool PgFriendMgr::RecvMsn_Friend_AddByGuid(SContentsUser const &rkOwnerUser, SContentsUser const &rkFriendUser)
{//Owner = 신청자, Friend = 피신청자
	HRESULT hRet = S_OK;

	if(  rkFriendUser.Empty() )
	{//친구를 찾는다
		hRet = FEC_NotFound;//찾을 수 없는놈이었다.
		goto __FAILED;
	}

	if(rkOwnerUser.kCharGuid == rkFriendUser.kCharGuid)
	{
		hRet = FEC_Failed_Me;
		goto __FAILED;
	}

	hRet = RecvMsn_Friend_Add(rkOwnerUser, rkFriendUser);
	switch(hRet)
	{
	case FEC_Success:
		{
			goto __SUCCESS;
		}break;
	case FEC_Already_Added://
	case FEC_Failed:
	case FEC_Failed_Limit:
	case FEC_NotFound:
	default:
		{
			//INFO_LOG(BM::LOG_LV8, _T("[%s]-[%d] Invalid FriendAdd ret code"), __FUNCTIONW__, __LINE__);
			goto __FAILED;
		}break;
	}
	
	goto __FAILED;
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
////////////////////////
__FAILED:
	{
		BM::Stream kPacket(PT_N_C_ANS_MSN_FRIENDCOMMAND, (BYTE)FCT_ADD_BYGUID);//
		kPacket.Push((BYTE)hRet);	//Error Code
		kPacket.Push(rkFriendUser.kCharGuid);
		g_kRealmUserMgr.Locked_SendToUser(rkOwnerUser.kCharGuid, kPacket, false);//실패 패킷
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
__SUCCESS:
	{
		return true;
	}
////////////////////////
}

int const PgFriendMgr::AddFriend_Check(PgMessenger *pkCasterMsgr, SContentsUser const &rkFriendUser)
{
	if (BM::GUID::NullData() == rkFriendUser.kCharGuid)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FEC_Failed"));
		return FEC_Failed;
	}

	if(pkCasterMsgr->IsExist(rkFriendUser.kCharGuid)) //중복 확인
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FEC_Already_added"));
		return FEC_Already_Added;
	}

	if (FM_MAXFRIEND <= pkCasterMsgr->Size())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EFC_Failed_Limit"));
		return FEC_Failed_Limit;
	}

	return FEC_Success;
}


int PgFriendMgr::RecvMsn_Friend_Add(SContentsUser const &rkOwnerUser, SContentsUser const &rkFriendUser)
{//Owner = 신청자, Friend = 피신청자
	//둘다 로그인 아니면 여기로 안들어 온다!!
	BM::CAutoMutex kLock_1(m_kMutex);
	PgMessenger *pkCasterMsgr = GetMessenger(rkOwnerUser.kCharGuid);
	if(!pkCasterMsgr)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FEC_Failed"));
		return FEC_Failed;
	}

	PgMessenger *pkTargetMsgr = GetMessenger(rkFriendUser.kCharGuid);
	if(!pkTargetMsgr)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FEC_Failed"));
		return FEC_Failed;
	}

	// Log
	PgLogCont kLogCont(ELogMain_Contents_Friend, ELogSub_Friend_Apply);				
	kLogCont.MemberKey(rkOwnerUser.kMemGuid);
	kLogCont.UID(rkOwnerUser.iUID);
	kLogCont.CharacterKey(rkOwnerUser.kCharGuid);
	kLogCont.ID(rkOwnerUser.kAccountID);
	kLogCont.Name(rkOwnerUser.Name());
	kLogCont.ChannelNo(static_cast<short>(rkOwnerUser.sChannel));
	kLogCont.Class(static_cast<short>(rkOwnerUser.iClass));
	kLogCont.Level(static_cast<short>(rkOwnerUser.sLevel));
	kLogCont.GroundNo(static_cast<int>(rkOwnerUser.kGndKey.GroundNo()));
	PgLog kLog(ELOrderMain_Friend, ELOrderSub_Apply);
	kLog.Set(0, static_cast<std::wstring>(rkFriendUser.Name()));
	kLog.Set(2, rkFriendUser.kCharGuid.str());
	kLogCont.Add(kLog);
	kLogCont.Commit();

	const EFriendErrorCode eMy = (EFriendErrorCode)AddFriend_Check(pkCasterMsgr, rkFriendUser);
	if (FEC_Success!=eMy)	//When Failed
	{
		//Send To Client Error Message 나한테만 보내자
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << eMy);
		return eMy;
	}

	//나만 체크 하면 된다
	const EFriendErrorCode eOther = (EFriendErrorCode)AddFriend_Check(pkTargetMsgr, rkOwnerUser);	//push to list as Wait State
	if (FEC_Success!=eOther && FEC_Already_Added!=eOther)	//When Failed
	{
		//Send To Client Error Message
		if (FEC_Success==eMy)
		{
			//and if Caster Has Success, Delete the User in Caster's msn
			//디비에 쿼리하면 안된다. 그냥 리스트에서만 지워야 된다.  
			VEC_GUID kVec;
			kVec.push_back(rkFriendUser.kCharGuid);
			pkCasterMsgr->DelFromListOnly(kVec);
		}
		return eOther;	//둘다 한테 보내자
	}

	//Send To Each Client what Success Message
	//성공했으니까 여기서 디비 쿼리를 하자.

	{//나
		if(S_OK != pkCasterMsgr->AddFriend(rkFriendUser, FCS_ADD_WAIT_REQ))//나한테는 친구를 등록한다
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FEC_Failed_Limit"));
			return FEC_Failed_Limit;
		}
	}

	if (FEC_Success==eOther)//있으면 추가 할 필요 없고
	{//상대
		if(S_OK != pkTargetMsgr->AddFriend(rkOwnerUser, FCS_ADD_WAIT_ANS))//피등록자는 대기목록에 나를 넣자
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FEC_Failed_Limit"));
			return FEC_Failed_Limit;
		}
	}
	else if (FEC_Already_Added==eOther)	//이미 추가 되 있으면
	{
		RecvMsn_Friend_AddAccept(rkFriendUser.kCharGuid, rkOwnerUser.kCharGuid);
	}
	
	return FEC_Success;
}

bool PgFriendMgr::RecvMsn_Friend_Del(SContentsUser const &rkOwnerUser, VEC_GUID const &rkContFriendChar)
{
	BM::CAutoMutex kLock_1(m_kMutex);

	PgMessenger *pkList = GetMessenger(rkOwnerUser.kCharGuid);
	if(!pkList)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID::const_iterator guid_iter = rkContFriendChar.begin();
	BM::GUID rkFriendGuid = BM::GUID::NullData();
	rkFriendGuid = (*guid_iter);
	SFriendItem pkFromItem;
	std::wstring kFromName = std::wstring();
	if( pkList->Get(rkFriendGuid, pkFromItem) == S_OK )
	{
		kFromName = pkFromItem.ExtendName();
	}

	HRESULT const hRet = pkList->Del(rkContFriendChar);
	
	if(S_OK == hRet)
	{
		DelMyInfoFormOther(rkOwnerUser, rkContFriendChar);

		// Log
		PgLogCont kLogCont(ELogMain_Contents_Friend, ELogSub_Friend_Delete);				
		kLogCont.MemberKey(rkOwnerUser.kMemGuid);
		kLogCont.UID(rkOwnerUser.iUID);
		kLogCont.CharacterKey(rkOwnerUser.kCharGuid);
		kLogCont.ID(rkOwnerUser.kAccountID);
		kLogCont.Name(rkOwnerUser.Name());
		kLogCont.ChannelNo(static_cast<short>(rkOwnerUser.sChannel));
		kLogCont.Class(static_cast<short>(rkOwnerUser.iClass));
		kLogCont.Level(static_cast<short>(rkOwnerUser.sLevel));
		kLogCont.GroundNo(static_cast<int>(rkOwnerUser.kGndKey.GroundNo()));

		PgLog kLog(ELOrderMain_Friend, ELOrderSub_Delete);		
		kLog.Set(0, static_cast<std::wstring>(std::wstring(kFromName)));
		kLog.Set(0, static_cast<int>(FCS_ADD_DELETED));
		kLog.Set(2, rkFriendGuid.str());

		kLogCont.Add(kLog);
		kLogCont.Commit();
	}
	else
	{
		//INFO_LOG(BM::LOG_LV1, _T("[%s]-[%d] Can't delete friend"), __FUNCTIONW__, __LINE__);
		//assert(NULL);//개발할때만 써라.

		BM::Stream kPacket(PT_N_C_NFY_MSN_ELEMENT_MODIFY);
		kPacket.Push((BYTE)FCT_NOT_DELETE);
		g_kRealmUserMgr.Locked_SendToUser(rkOwnerUser.kCharGuid, kPacket, false);

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgFriendMgr::RecvMsn_Friend_AddAccept(BM::GUID const &rkOwnerCharGuid, BM::GUID const &rkFriendGuid)
{
	if(BM::GUID::NullData() == rkOwnerCharGuid)
	{
		//INFO_LOG(BM::LOG_LV1, _T("[%s]-[%d] Login user is null"), __FUNCTIONW__, __LINE__);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BYTE cChatStatus = FCS_ADD_ALLOW|FCS_BLOCK_NONE;

	//내목록 업데이트-->전부한테 보내 줄 필요 없다. accept한 애만 modify 해서 보내주자
	PgMessenger *pkList = GetMessenger(rkOwnerCharGuid);
	PgMessenger *pkFriendList = GetMessenger(rkFriendGuid);
	if (pkList)
	{
		if (pkFriendList)	//상대가 로그인 해 있으면
		{
			pkList->LinkChange(rkFriendGuid, pkFriendList->LinkStatus());		//상대의 현재 채팅 상태를 갖고와서 업데해 주자
			pkFriendList->LinkChange(rkOwnerCharGuid, pkList->LinkStatus());	//내 현재 채팅 상태를 보내주자
		}
	}
	if(!RecvMsn_Friend_ChatStatus(rkOwnerCharGuid, rkFriendGuid, cChatStatus))
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Failed update my chat status") );
	}
	
	//상대꺼 업데이트-->내 현재 채팅 상태를 보내줘서 업데이트 하게 하도록 하자	
	if(!RecvMsn_Friend_ChatStatus(rkFriendGuid, rkOwnerCharGuid, cChatStatus))
	{//상대편이 온라인이 아니면 강제 저장
		if (pkList)
		{
			SFriendItem kItem;
			if( pkList->Get(rkFriendGuid, kItem) == S_OK )
			{
				SContentsUser kUser;
				if( S_OK == ::GetPlayerByGuid(rkOwnerCharGuid, false, kUser) )
				{
					BM::Stream kPacket(PT_N_T_REQ_MSN_FRIENDCOMMAND, rkFriendGuid);
					kPacket.Push((BYTE)FCT_MODIFY);
					kItem.CharGuid(kUser.kCharGuid);
					kItem.ExtendName(kUser.Name());
					kItem.ChatStatus(cChatStatus);
					kItem.Class( kUser.iClass );
					kItem.Level( kUser.sLevel );
					kItem.Channel( kUser.sChannel );
					kItem.WriteToDBPacket(kPacket);
					SendToPacketHandler(kPacket);
				}
			}	
		}
	}

	// 친구 추가 업적: 오프라인일 수도 있으므로 캐릭터 로그인시에 한번 더 체크 해 준다.
	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(rkOwnerCharGuid);
	pkActionOrder->InsertTarget(rkFriendGuid);
	pkActionOrder->kCause = CAE_Achievement;
	
	if( pkFriendList ) // 요청한 사람이 온라인이면
	{
		// 수락한 사람에게 전송
		BM::Stream kPacket(PT_N_C_ANS_MSN_FRIENDCOMMAND, (BYTE)FCT_ADD_ACCEPT);
		kPacket.Push((BYTE)FEC_Success);
		kPacket.Push(pkFriendList->Name());
		g_kRealmUserMgr.Locked_SendToUser(rkOwnerCharGuid, kPacket, false);
		pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ABIL,rkFriendGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_FRIEND,pkFriendList->Size())));// 요청한 사람 업적
	}

	if( pkList ) // 수락한 사람이 온라인이면
	{
		// 요청한 사람에게 전송
		BM::Stream kPacket(PT_N_C_ANS_MSN_FRIENDCOMMAND, (BYTE)FCT_ADD_ACCEPT);
		kPacket.Push((BYTE)FEC_Accept);
		kPacket.Push(pkList->Name());
		g_kRealmUserMgr.Locked_SendToUser(rkFriendGuid, kPacket, false);
		pkActionOrder->kContOrder.push_back(SPMO(IMET_SET_ABIL,rkOwnerCharGuid,SPMOD_AddAbil(AT_ACHIEVEMENT_FRIEND,pkList->Size())));// 수락한 사람 업적
	}

	g_kJobDispatcher.VPush(pkActionOrder);

	// Log
	SContentsUser kMyUser;
	if( S_OK == ::GetPlayerByGuid(rkFriendGuid, false, kMyUser) )
	{
		PgLogCont kLogCont(ELogMain_Contents_Friend, ELogSub_Friend_Add);				
		kLogCont.MemberKey(kMyUser.kMemGuid);
		kLogCont.UID(kMyUser.iUID);
		kLogCont.CharacterKey(kMyUser.kCharGuid);
		kLogCont.ID(kMyUser.kAccountID);
		kLogCont.Name(kMyUser.Name());
		kLogCont.ChannelNo(static_cast<short>(kMyUser.sChannel));
		kLogCont.Class(static_cast<short>(kMyUser.iClass));
		kLogCont.Level(static_cast<short>(kMyUser.sLevel));
		kLogCont.GroundNo(static_cast<int>(kMyUser.kGndKey.GroundNo()));

		PgLog kLog(ELOrderMain_Friend, ELOrderSub_Create);
		if( pkList )
		{
			kLog.Set(0, static_cast<std::wstring>(pkList->Name()));			
		}
		else
		{
			kLog.Set(0, static_cast<std::wstring>(std::wstring()));
		}
		kLog.Set(0, static_cast<int>(cChatStatus));
		kLog.Set(2, rkOwnerCharGuid.str());

		kLogCont.Add(kLog);
		kLogCont.Commit();
	}
	
	return true;
}

bool PgFriendMgr::RecvMsn_Friend_ChatStatus(BM::GUID const &rkOwnerCharGuid, BM::GUID const &rkFriendGuid, BYTE const cChatStatus, bool const bFromUser, PgLog *pkLog)
{
	//Owner = 신청자, Friend = 피신청자
	BM::CAutoMutex kLock_1(m_kMutex);

	PgMessenger *pkList = GetMessenger(rkOwnerCharGuid);
	if(!pkList)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SFriendItem kItem;
	if(S_OK == pkList->Get(rkFriendGuid, kItem))
	{
		BYTE cChange = cChatStatus;
		if (bFromUser)
		{
			BYTE const cBase = 0x01<<cChatStatus;
			cChange = kItem.ChatStatus()^cBase;
			if ( !(cChange & FCS_ADD_ALLOW) )
			{
				cChange|=FCS_ADD_ALLOW;	//허가표시 계속 유지하기
			}
			if( pkLog )
			{				
				if( (cChange == FCS_ADD_ALLOW) || (cChange == (FCS_ADD_ALLOW|FCS_BLOCK_NO_RECV|FCS_BLOCK_NO_SEND)) )
				{
					if( cChange == FCS_ADD_ALLOW )
					{
						pkLog->OrderSubType(static_cast<EOrderSubType>(ELOrderSub_Clear));
					}
					pkLog->Set(0, static_cast<int>(cChange));
				}				
			}
		}

		kItem.ChatStatus(cChange);
		pkList->Modify(kItem);

		pkList->SendToDB(&kItem, (BYTE)FCT_MODIFY);

		//성공
		BM::Stream kPacket(PT_N_C_ANS_MSN_FRIENDCOMMAND, (BYTE)FCT_MODIFY);
		kPacket.Push((BYTE)FEC_Success);
		kPacket.Push((int)1);
		kItem.WriteToPacket(kPacket);
		g_kRealmUserMgr.Locked_SendToUser(rkOwnerCharGuid, kPacket, false);

		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgFriendMgr::RecvPT_T_N_NFY_MSN_FULL_DATA(SContentsUser const &rkUser, BM::Stream* pkPacket)
{
	BM::CAutoMutex kLock_1(m_kMutex);

	if( rkUser.Empty() )
	{
		//INFO_LOG(BM::LOG_LV1, _T("[%s]-[%d] can't find user (maybe logout?)"), __FUNCTIONW__, __LINE__);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMessenger *pkMsgr = GetMessenger(rkUser.kCharGuid);
	if(!pkMsgr)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkMsgr->DelAll();//클리어링.

	size_t element_size = 0;
	pkPacket->Pop(element_size);
	for(size_t i = 0; element_size > i; ++i)
	{
		SFriendItem kFriendItem;
		kFriendItem.ReadFromDBPacket(*pkPacket);
		SContentsUser kFriend;
		SEventMessage kEventMsg;
		if(S_OK != WritePlayerInfoToPacket_ByGuid(kFriendItem.CharGuid(), false, kEventMsg))
		{
			//INFO_LOG(BM::LOG_LV0, _T("[%s]-[%d] can't find login user"), __FUNCTIONW__, __LINE__);
			kFriend.kCharGuid = kFriendItem.CharGuid();
		}
		else
		{
			kFriend.ReadFromPacket(kEventMsg);
		}
		
		if (FEC_Success == AddFriend_Check(pkMsgr, kFriend))
		{ 
			kFriendItem.GroundKey( kFriend.kGndKey );
			kFriendItem.Class( kFriend.iClass );
			kFriendItem.Level( kFriend.sLevel );
			kFriendItem.Channel( kFriend.sChannel );
			pkMsgr->AddFriend(kFriendItem, false);
		}
	}
	//아래 두함수 처리를 하고.
	NfyMySelfToFriend(rkUser);
	GetFriendMsnState(pkMsgr);
	// 유저한테 덮어쓰라고 패킷을 때려!!.
	ContFriendVec kFrndVec;
	pkMsgr->GetFriendList(kFrndVec);
	pkMsgr->SendModify(kFrndVec, FCT_FULL_DATA);

	//INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] General Log: Friend list Update from DB"), __FUNCTIONW__, __LINE__);
	return true;
}

bool PgFriendMgr::RecvMsn_Friend_ChangeGroupName(SContentsUser const &rkUser, std::wstring const &rkOldName, std::wstring const &rkNewName)
{
	BM::CAutoMutex kLock_1(m_kMutex);

	PgMessenger *pkList = GetMessenger(rkUser.kCharGuid);
	if (!pkList)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContFriendVec kFriendVec;
	pkList->GetFriendList(kFriendVec);

	ContFriendVec::iterator frnd_iter = kFriendVec.begin();
	while (kFriendVec.end() != frnd_iter)
	{
		SFriendItem &kFriend = (*frnd_iter);
		if (kFriend.GroupName() == rkOldName)
		{
			std::wstring kName = rkNewName;
			if (rkNewName.size()>20)
			{
				kName.resize(20);
				kName[19] = _T('\0');
			}
			kFriend.GroupName(kName);
			pkList->Modify(kFriend);
			pkList->SendToDB(&kFriend, FCT_MODIFY);
		}
		++frnd_iter;
	}

	pkList->SendModify(kFriendVec, FCT_MODIFY);

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgFriendMgr::RecvMsn_Friend_ChangeLinkState(SContentsUser const &rkUser, BYTE const cLinkState)
{
	NfyMySelfToFriend(rkUser, cLinkState);	//그냥 내 상태를 나를 등록한 유져들에게 알려 주면 된다

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
//
bool PgFriendMgr::SendPacket_ToFriend_ByCharGuid(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock_1(m_kMutex);

	//친구 목록
	PgMessenger *pkList = GetMessenger(rkCharGuid);
	if(!pkList)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContFriendVec kVec;
	pkList->GetFriendList(kVec);
	if (!kVec.empty())
	{
		ContFriendVec::iterator kItem_iter = kVec.begin();
		while(kVec.end() != kItem_iter)
		{
			SFriendItem &kItem = (*kItem_iter);
			PgMessenger *pFriend = GetMessenger(kItem.CharGuid());
			if (pFriend)
			{
				SFriendItem kMyItem;
				if(S_OK == pFriend->Get(rkCharGuid, kMyItem))	//내 상태를 얻어서
				{//내가 있으면
					if (!(kMyItem.ChatStatus() & FCS_BLOCK_NO_RECV) &&
						(kMyItem.ChatStatus() & FCS_ADD_ALLOW))	//내가 차단 안되있고 듣기도 차단 안되있고
					{
						if (!(kItem.ChatStatus() & FCS_BLOCK_NO_SEND) && 
							(kItem.ChatStatus() & FCS_ADD_ALLOW))	//나도 그놈이 차단 안되있고 보내기 차단 안되있으면
						{
							g_kRealmUserMgr.Locked_SendToUser(kItem.CharGuid(), rkPacket, false); //그때서야 보낸다
						}
					}
				}
			}

			++kItem_iter;
		}
	}
	g_kRealmUserMgr.Locked_SendToUser(rkCharGuid, rkPacket, false);//친구가 있건 없건 나한테 보내자


	return true;
}

bool PgFriendMgr::ProcessMsg(SEventMessage *pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool bRet = false;
	PACKET_ID_TYPE wType = 0;
	pkMsg->Pop(wType);
	switch(wType)
	{
	case PT_T_N_NFY_USER_ENTER_GROUND:
		{
			SAnsMapMove_MT kAMM;
			SContentsUser kUser;

			pkMsg->Pop(kAMM);
			kUser.ReadFromPacket( *(BM::Stream*)pkMsg);

			switch(kAMM.eRet)
			{
			case MMR_SUCCESS:
			case MMR_PVP_LOBBY:
				{
					PgMessenger const *pkFind = GetMessenger(kUser.kCharGuid);
					if( !pkFind )//처음 로그인
					{
						if( MakeMsn(kUser) )	// 메신져를 생성하자
						{	//메신져가 생성되면 서버에 풀 데이터를 요구하자
							BM::Stream kPacket(PT_N_T_REQ_MSN_FULL_DATA, kUser.kCharGuid);
							SendToPacketHandler(kPacket);
						}
					}
					else//맵이동
					{
						Recv_MapMove(kUser);
					}
				}break;
			default:
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				}break;
			}
		}break;
	case PT_A_U_SEND_CHANGE_PLACE:
		{
			SContentsUser kUser;
			kUser.ReadFromPacket( *(BM::Stream*)pkMsg );
			Recv_MapMove(kUser);
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			SContentsUser kUser;
			kUser.ReadFromPacket(*(BM::Stream*)pkMsg);
			bRet = Recv_Logout(kUser);
		}break;
	case PT_T_N_NFY_MSN_FULL_DATA_2ND:
		{
			SContentsUser kUser;
			kUser.ReadFromPacket(*(BM::Stream*)pkMsg);
			bRet = RecvPT_T_N_NFY_MSN_FULL_DATA(kUser, pkMsg);
		}break;
	case PT_C_N_REQ_MSN_FRIENDCOMMAND://메신져, 친구목록 명령어
		{
			BYTE cCommandType = 0;
			BM::GUID kCharGuid;
			BM::GUID kFriendMemberGuid;
			std::wstring kFriendName;
			std::wstring kGroupName;
			BYTE cChatStatus = 0;
			BYTE cLinkStatus = 0;
			int iSize = 0;
			ContFriendVec kFriendVec;

			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(cCommandType);

			BM::Stream kEPacket(PT_C_N_REQ_MSN_FRIENDCOMMAND_2ND);
			//SEventMessage kEventMsg;
			//kEventMsg.PriType(PMET_FRIEND);
			//kEventMsg.SecType(usType);
			kEPacket.Push(cCommandType);
			if(S_OK != ::WritePlayerInfoToPacket_ByGuid(kCharGuid, false, kEPacket))
			{
				INFO_LOG( BM::LOG_LV0, __FL__ << _T("can't find login user") );
			}
			
			switch(cCommandType)
			{
			case FCT_ADD_BYNAME://이름으로 친구 추가
				{
					PU::TLoadArray_M(*pkMsg, kFriendVec);
					kEPacket.Push((int)kFriendVec.size());
					ContFriendVec::const_iterator frnd_iter = kFriendVec.begin();
					while (kFriendVec.end() != frnd_iter)
					{
						WritePlayerInfoToPacket_ByName((*frnd_iter).ExtendName(), kEPacket);
						++frnd_iter;
					}
				}break;
			case FCT_ADD_BYGUID://CharGuid로 친구 추가
				{
					PU::TLoadArray_M(*pkMsg, kFriendVec);
					kEPacket.Push((int)kFriendVec.size());
					ContFriendVec::const_iterator frnd_iter = kFriendVec.begin();
					while (kFriendVec.end() != frnd_iter)
					{

						::WritePlayerInfoToPacket_ByGuid((*frnd_iter).CharGuid(), false, kEPacket);

						++frnd_iter;
					}
				}break;
			case FCT_ADD_ACCEPT:
			case FCT_ADD_REJECT:
				{
					PU::TLoadArray_M(*pkMsg, kFriendVec);
					kEPacket.Push((int)kFriendVec.size());
					ContFriendVec::const_iterator frnd_iter = kFriendVec.begin();
					while (kFriendVec.end() != frnd_iter)
					{
						kEPacket.Push((*frnd_iter).CharGuid());
						++frnd_iter;
					}
				}break;
			case FCT_MODIFY_CHATSTATUS:
				{
					PU::TLoadArray_M(*pkMsg, kFriendVec);
					kEPacket.Push((int)kFriendVec.size());
					ContFriendVec::const_iterator frnd_iter = kFriendVec.begin();
					while (kFriendVec.end() != frnd_iter)
					{
						::WritePlayerInfoToPacket_ByGuid((*frnd_iter).CharGuid(), false, kEPacket);
						kEPacket.Push((*frnd_iter).ChatStatus());
						++frnd_iter;
					}
				}break;
			case FCT_FULL_DATA://친구 목록 요청
				{

				}break;
			case FCT_DELETE:
				{
					PU::TLoadArray_M(*pkMsg, kFriendVec);
					kEPacket.Push((int)kFriendVec.size());
					ContFriendVec::const_iterator frnd_iter = kFriendVec.begin();
					while (kFriendVec.end() != frnd_iter)
					{
						kEPacket.Push((*frnd_iter).CharGuid());
						++frnd_iter;
					}
				}break;
			case FCT_GROUP:
				{
					std::wstring kNewGroup;
					PU::TLoadArray_M(*pkMsg, kFriendVec);
					kEPacket.Push((int)kFriendVec.size());
					ContFriendVec::const_iterator frnd_iter = kFriendVec.begin();
					while (kFriendVec.end() != frnd_iter)
					{
						kEPacket.Push((*frnd_iter).GroupName());	//옛날 이름
						kEPacket.Push((*frnd_iter).ExtendName());	//새 이름
						++frnd_iter;
					}
				}break;
			case FCT_MODIFY:
				{
					PU::TLoadArray_M(*pkMsg, kFriendVec);
					kEPacket.Push((int)kFriendVec.size());
					ContFriendVec::const_iterator frnd_iter = kFriendVec.begin();
					while (kFriendVec.end() != frnd_iter)
					{
						(*frnd_iter).WriteToPacket(kEPacket);
						++frnd_iter;
					}
				}break;
			case FCT_LINKSTATUS:
				{
					pkMsg->Pop(cLinkStatus);
					kEPacket.Push(cLinkStatus);

					BM::Stream kPacket(PT_T_N_ANS_COUPLE_COMMAND, (BYTE)CC_Status);
					kPacket.Push( (BYTE)CoupleCR_None );
					kPacket.Push( kCharGuid );
					kPacket.Push( NULL );
					kPacket.Push( cLinkStatus );
					kPacket.Push( 0 );
					SendToCoupleMgr(kPacket);
				}break;
			default:
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("is not preparation friend command[") << static_cast<int>(cCommandType) << _T("]") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				}break;
			}//end switch
			
			//
			SendToFriendMgr(kEPacket);

		}break;
	case PT_C_N_REQ_MSN_FRIENDCOMMAND_2ND:
		{
			BYTE cCmdType = 0;
			SContentsUser kFromUser;
			SContentsUser kToUser;
			std::wstring kGroupName;

			pkMsg->Pop(cCmdType);
			kFromUser.ReadFromPacket(*(BM::Stream*)pkMsg);
			int iSize = 0;

			bool bRet = false;
			switch(cCmdType)
			{
			case FCT_ADD_BYNAME:
			case FCT_ADD_BYGUID:
				{
					pkMsg->Pop(iSize);
					for (int i = 0; i < iSize; ++i)
					{
						kToUser.ReadFromPacket(*(BM::Stream*)pkMsg);
						bRet = RecvMsn_Friend_AddByGuid(kFromUser, kToUser);
					}
				}break;
			case FCT_ADD_ACCEPT:
				{
					pkMsg->Pop(iSize);

					BM::GUID kToCharGuid;
					for (int i = 0; i < iSize; ++i)
					{
						pkMsg->Pop(kToCharGuid);
						bRet = RecvMsn_Friend_AddAccept(kFromUser.kCharGuid, kToCharGuid);
					}
				}break;
			case FCT_ADD_REJECT://거부하면 삭제
				{
					pkMsg->Pop(iSize);

					BM::GUID kToCharGuid;
					VEC_GUID kVecGuid;
					for (int i = 0; i < iSize; ++i)
					{
						pkMsg->Pop(kToCharGuid);
						kVecGuid.push_back(kToCharGuid);
					}
					bRet = RecvMsn_Friend_Del(kFromUser, kVecGuid);
	
					VEC_GUID::const_iterator guid_it = kVecGuid.begin();
					
					BM::Stream kPacket(PT_N_C_ANS_MSN_FRIENDCOMMAND);
					kPacket.Push(cCmdType);
					kPacket.Push((BYTE)FEC_None);
					kPacket.Push(kFromUser.kCharGuid);
					kPacket.Push(kFromUser.Name());

					while(guid_it!=kVecGuid.end())
					{
						BM::GUID const &rkGuid = (*guid_it);
						g_kRealmUserMgr.Locked_SendToUser(rkGuid, kPacket, false);//실패 패킷
						++guid_it;
					}

					// Log
					PgLogCont kLogCont(ELogMain_Contents_Friend, ELogSub_Friend_Rejection);				
					kLogCont.MemberKey(kFromUser.kMemGuid);
					kLogCont.UID(kFromUser.iUID);
					kLogCont.CharacterKey(kFromUser.kCharGuid);
					kLogCont.ID(kFromUser.kAccountID);
					kLogCont.Name(kFromUser.Name());
					kLogCont.ChannelNo(static_cast<short>(kFromUser.sChannel));
					kLogCont.Class(static_cast<short>(kFromUser.iClass));
					kLogCont.Level(static_cast<short>(kFromUser.sLevel));
					kLogCont.GroundNo(static_cast<int>(kFromUser.kGndKey.GroundNo()));

					PgLog kLog(ELOrderMain_Friend, ELOrderSub_Rejection);
					SContentsUser kUser;

					VEC_GUID::const_iterator guidFrom_itr = kVecGuid.begin();		
					if( guidFrom_itr != kVecGuid.end() )
					{
						BM::GUID const &rkFromGuid = (*guidFrom_itr);
						if( S_OK == ::GetPlayerByGuid(rkFromGuid, false, kUser) )
						{
							kLog.Set(0, static_cast<std::wstring>(std::wstring(kUser.Name())));
							kLog.Set(2, rkFromGuid.str());
						}
						else
						{
							kLog.Set(0, static_cast<std::wstring>(std::wstring()));
							kLog.Set(2, BM::GUID::NullData().str());
						}
					}
					else
					{
						kLog.Set(0, static_cast<std::wstring>(std::wstring()));
						kLog.Set(2, BM::GUID::NullData().str());
					}
					kLogCont.Add(kLog);
					kLogCont.Commit();
				}break;
			//case EMCT_Messenger_Friend_Add_Accept:
			case FCT_MODIFY_CHATSTATUS:
				{
					pkMsg->Pop(iSize);

					BM::GUID kToCharGuid;
					for (int i = 0; i < iSize; ++i)
					{
						BYTE cChatStatus = 0;
						kToUser.ReadFromPacket(*(BM::Stream*)pkMsg);
						pkMsg->Pop(cChatStatus);

						// Log
						PgLog kLog(ELOrderMain_Friend, ELOrderSub_Block);
						kLog.Set(0, static_cast<std::wstring>(kToUser.Name()));

						bRet = RecvMsn_Friend_ChatStatus(kFromUser.kCharGuid, kToUser.kCharGuid, cChatStatus, true, &kLog);

						if( bRet )
						{
							if( kLog.OrderSubType() == ELOrderSub_Clear )
							{
								PgLogCont kLogCont(ELogMain_Contents_Friend, ELogSub_Friend_Clear);
								kLogCont.MemberKey(kFromUser.kMemGuid);
								kLogCont.UID(kFromUser.iUID);
								kLogCont.CharacterKey(kFromUser.kCharGuid);
								kLogCont.ID(kFromUser.kAccountID);
								kLogCont.Name(kFromUser.Name());
								kLogCont.ChannelNo(static_cast<short>(kFromUser.sChannel));
								kLogCont.Class(static_cast<short>(kFromUser.iClass));
								kLogCont.Level(static_cast<short>(kFromUser.sLevel));
								kLogCont.GroundNo(static_cast<int>(kFromUser.kGndKey.GroundNo()));

								kLog.Set(2, kToUser.kCharGuid.str());
								kLogCont.Add(kLog);
								kLogCont.Commit();
							}
							else
							{
								PgLogCont kLogCont(ELogMain_Contents_Friend, ELogSub_Friend_Block);
								kLogCont.MemberKey(kFromUser.kMemGuid);
								kLogCont.UID(kFromUser.iUID);
								kLogCont.CharacterKey(kFromUser.kCharGuid);
								kLogCont.ID(kFromUser.kAccountID);
								kLogCont.Name(kFromUser.Name());
								kLogCont.ChannelNo(static_cast<short>(kFromUser.sChannel));
								kLogCont.Class(static_cast<short>(kFromUser.iClass));
								kLogCont.Level(static_cast<short>(kFromUser.sLevel));
								kLogCont.GroundNo(static_cast<int>(kFromUser.kGndKey.GroundNo()));

								kLog.Set(2, kToUser.kCharGuid.str());
								kLogCont.Add(kLog);
								kLogCont.Commit();
							}
						}
					}
				}break;
			case FCT_DELETE:
				{
					pkMsg->Pop(iSize);

					BM::GUID kToCharGuid;
					VEC_GUID kConGuid;
					for (int i = 0; i < iSize; ++i)
					{
						pkMsg->Pop(kToCharGuid);
						kConGuid.push_back(kToCharGuid);
					}
					bool bRet = RecvMsn_Friend_Del(kFromUser, kConGuid);
					if( true == bRet )
					{
						SContentsUser kUser;
						if( S_OK == ::GetPlayerByGuid(kToCharGuid, false, kUser) )
						{
							kConGuid.clear();
							kConGuid.push_back(kFromUser.kCharGuid);
							bRet = RecvMsn_Friend_Del(kUser, kConGuid);
						}
						else
						{// 온라인 아니면
							if( BM::GUID::NullData() != kToCharGuid )
							{
								SFriendItem kItem;
								kItem.Clear();
								kItem.CharGuid(kFromUser.kCharGuid);

								BM::Stream kDbPacket(PT_N_T_REQ_MSN_FRIENDCOMMAND, kToCharGuid);
								kDbPacket.Push((BYTE)FCT_DELETE);
								kItem.WriteToDBPacket(kDbPacket);
								SendToPacketHandler(kDbPacket);
							}
						}
					}
				}break;
			case FCT_GROUP:
					{
						pkMsg->Pop(iSize);

						std::wstring kOldName, kNewName;
						for (int i = 0; i < iSize; ++i)
						{
							pkMsg->Pop(kOldName);
							pkMsg->Pop(kNewName);
							RecvMsn_Friend_ChangeGroupName(kFromUser, kOldName, kNewName);
						}
					}break;
			case FCT_MODIFY:
				{
					PgMessenger* pkMyMsn = GetMessenger(kFromUser.kCharGuid);
					if (pkMyMsn)
					{
						pkMsg->Pop(iSize);
						ContFriendVec kCon;
						for (int i = 0; i < iSize; ++i)
						{
							SFriendItem kItem;
							kItem.ReadFromPacket(*pkMsg);
							kCon.push_back(kItem);

							pkMyMsn->Modify(kItem);
							pkMyMsn->SendToDB(&kItem, FCT_MODIFY);
						}

						pkMyMsn->SendModify(kCon, FCT_MODIFY);
					}
				}break;
			case FCT_LINKSTATUS:
				{
					BYTE cLinkStatus = 0;
					pkMsg->Pop(cLinkStatus);

					RecvMsn_Friend_ChangeLinkState(kFromUser, cLinkStatus);
				}break;
			default:
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("invalid friend command") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				};
			}
			switch(cCmdType)
			{
			case FCT_ADD_BYNAME:
			case FCT_ADD_BYGUID:
			case FCT_ADD_ACCEPT:
			case FCT_ADD_REJECT:
			case FCT_DELETE:
				{
					// TODO : Friend 와 Party가 Channel이 다른경우 어떻게 처리 할 것인가 정책 정하고 나서 수정
					BM::Stream kPacket( PT_A_U_SEND_TOFRIEND_PARTYINFO, kFromUser.kCharGuid );
					kPacket.Push(true);
					SendToGlobalPartyMgr(kFromUser.sChannel, kPacket);
				}break;
			default:
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				}break;
			}
		}break;

		
		//
	case PT_A_U_SEND_TOFRIEND_BYCHARGUID:
		{
			BM::GUID kCharGuid;
			BM::Stream::STREAM_DATA kData;
			BM::Stream kPacket;

			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(kData);

			kPacket.Push(&kData.at(0), kData.size()*sizeof(BM::Stream::STREAM_DATA::value_type));

			bRet = SendPacket_ToFriend_ByCharGuid(kCharGuid, kPacket);
		}break;
	case PT_A_U_SEND_TOFRIEND_PARTY:
		{			
			bRet = RecvMsn_Friend_Party(pkMsg);			
		}break;
	case PT_T_N_NFY_MSN_FULL_DATA:
		{
			BM::GUID kOwnerCharGuid;
			BYTE cCommandType = 0;
			BYTE cCountFriend = 0;

			pkMsg->Pop(kOwnerCharGuid);

			BM::Stream kBPacket(PT_T_N_NFY_MSN_FULL_DATA_2ND);
			::WritePlayerInfoToPacket_ByGuid(kOwnerCharGuid, false, kBPacket);
			kBPacket.Push(*(BM::Stream*)pkMsg);//Push remain packet data
			SendToFriendMgr(kBPacket);
		}break;
	case PT_T_N_REQ_FRIEND_LINK_INFO:
		{
			SContentsUser kUser;
			BM::GUID kCharGuid;
			BYTE cLinkStatus = 0;

			pkMsg->Pop(kCharGuid);

			if( S_OK == ::GetPlayerByGuid(kCharGuid, false, kUser) )
			{
				cLinkStatus = LinkStatus(kCharGuid);
				RecvMsn_Friend_ChangeLinkState(kUser, cLinkStatus);
			}
		}break;
	case PT_N_N_NFY_COMMUNITY_STATE_HOMEADDR_FRIEND:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			SHOMEADDR kHomeAddr;
			kHomeAddr.ReadFromPacket(*pkMsg);
			
			if( true == ChangeHomeAddr(kCharGuid, kHomeAddr) )
			{
				SContentsUser kUser;
				if( S_OK == ::GetPlayerByGuid(kCharGuid, false, kUser) )
				{//유저가 접속중이면
					BYTE cLinkStatus = LinkStatus(kCharGuid);
					kUser.kHomeAddr = kHomeAddr;
					RecvMsn_Friend_ChangeLinkState(kUser, cLinkStatus);
				}
				else
				{//접속중이 아닐때
					NfyToMyFriend(kCharGuid);
				}
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("invalid packet type [") << wType << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}
	}
	return true;
}

bool PgFriendMgr::ChangeHomeAddr(BM::GUID const& rkCharGuid, SHOMEADDR const kHomeAddr)
{
	bool bRet = false;
	ContMessenger::iterator msgr_itor = m_kContMsgr.begin();
	while(msgr_itor != m_kContMsgr.end())
	{
		SFriendItem kItem;
		if( S_OK == (*msgr_itor).second->Get(rkCharGuid, kItem) )
		{
			kItem.HomeAddr(kHomeAddr);
			(*msgr_itor).second->Modify(kItem);
			bRet = true;
		}
		++msgr_itor;
	}
	return bRet;
}

bool PgFriendMgr::RecvMsn_Friend_Party(BM::Stream* pkPacket)
{
	VEC_UserInfo kVec_UserInfo;
	VEC_UserInfo kVec_UserInfoSend;
	BM::GUID rkCharGuid;
	BM::GUID rkPartyGuid;
	bool bSet = false;
	int iMemberFriendCount = 0;

	short sChannel;

	pkPacket->Pop(sChannel);
	pkPacket->Pop(rkPartyGuid);
	pkPacket->Pop(rkCharGuid);	
	pkPacket->Pop(bSet);
	PU::TLoadArray_M(*pkPacket, kVec_UserInfo);

	
	BM::Stream kPacket( PT_A_U_SEND_TOFRIEND_PARTY, rkCharGuid);
	kPacket.Push(bSet);
	kPacket.Push(rkPartyGuid);
	

	PgMessenger *pkList = GetMessenger(rkCharGuid);
	if(!pkList)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}	


	VEC_UserInfo::iterator iter = kVec_UserInfo.begin();
	while( kVec_UserInfo.end() != iter )
	{
		SPartyUserInfo &kMember = (*iter);

		BM::GUID const &rkCharGuidList = kMember.kCharGuid;

		SFriendItem kItem;
		if(S_OK == pkList->Get(rkCharGuidList, kItem))
		{
			PgMessenger *pFriend = GetMessenger(rkCharGuidList);
			if (pFriend)
			{
				SFriendItem kMyItem;
				if(S_OK == pFriend->Get(rkCharGuid, kMyItem))	//내 상태를 얻어서
				{//내가 있으면
					if ((kMyItem.ChatStatus() & FCS_ADD_ALLOW))	//내가 등록되 있고
					{
						if ((kItem.ChatStatus() & FCS_ADD_ALLOW))	//나도 등록되 있고
						{
							(bSet) ? kMember.iFriend++ : kMember.iFriend--;

							if( kMember.iFriend > PV_MAX_MEMBER_CNT ) 
							{
								kMember.iFriend = PV_MAX_MEMBER_CNT;
							}
							else if( kMember.iFriend < 0 ) 
							{
								kMember.iFriend = 0;
							}
							kVec_UserInfoSend.push_back(kMember);
							if( bSet ) //나랑 친구인넘이 몇명있냐
							{
								iMemberFriendCount++;
							}
						}
					}
				}
			}
		}
		else
		{
			kVec_UserInfoSend.push_back(kMember);
		}
		++iter;
	}

	VEC_UserInfo::iterator itor = kVec_UserInfoSend.begin();
	while( kVec_UserInfoSend.end() != itor )
	{
		if( rkCharGuid == (*itor).kCharGuid )
		{
			(*itor).iFriend = iMemberFriendCount;
			break;
		}
		++itor;
	}	

	PU::TWriteArray_M(kPacket, kVec_UserInfoSend);
	SendToGlobalPartyMgr(sChannel, kPacket);

	return true;
}

bool PgFriendMgr::Online(BM::GUID const &rkGuid) const
{
	return (LinkStatus(rkGuid) != FLS_OFFLINE);
}

EFriendLinkStatus PgFriendMgr::LinkStatus(BM::GUID const &rkGuid) const
{
	const PgMessenger *pkList = GetMessenger(rkGuid);
	if(!pkList)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FLS_OFFLINE"));
		return FLS_OFFLINE;
	}
	return (EFriendLinkStatus)pkList->LinkStatus();
}

void PgFriendMgr::NfyMySelfToFriend(SContentsUser const &rkOwnerUser, BYTE const eState)
{
	ContMessenger::iterator msgr_itor = m_kContMsgr.begin();
	ContFriendVec kVec;

	PgMessenger *pkCurMsgr = GetMessenger(rkOwnerUser.kCharGuid);
	if (pkCurMsgr)
	{
		ContFriendVec kFriendVec;
		pkCurMsgr->GetFriendList(kFriendVec);	//내 메신져에 있는 친구들 중에
		ContFriendVec::iterator frnd_iter = kFriendVec.begin();
		while (kFriendVec.end() != frnd_iter)
		{
			SFriendItem &rkItem = (*frnd_iter);
			if ( !(rkItem.ChatStatus() & 0x30) )	//내가 허락한 친구 중에
			{
				PgMessenger *pkFriendMsgr = GetMessenger(rkItem.CharGuid());
				if (pkFriendMsgr)	// 온라인이면
				{
					SFriendItem kFriendItem;
					if ( S_OK == pkFriendMsgr->Get(rkOwnerUser.kCharGuid, kFriendItem) )
					{
						kVec.clear();
						kFriendItem.LinkStatus(eState);
						kFriendItem.GroundKey(rkOwnerUser.kGndKey);
						kFriendItem.Class( rkOwnerUser.iClass );
						kFriendItem.Level( rkOwnerUser.sLevel );
						kFriendItem.Channel( rkOwnerUser.sChannel );
						kFriendItem.HomeAddr( rkOwnerUser.kHomeAddr) ;
						kVec.push_back(kFriendItem);
						pkFriendMsgr->Modify(kFriendItem);
						pkFriendMsgr->SendModify(kVec, FCT_MODIFY);
					}
				}
			}
			++frnd_iter;
		}
		pkCurMsgr->LinkStatus(eState);


		PgMessenger *pkCurMsgrTemp = GetMessenger(rkOwnerUser.kCoupleGuid);
		if (pkCurMsgrTemp)
		{
			// 커플 정보 : 상대방 상태 및 위치정보
			BM::Stream kPacket(PT_T_N_ANS_COUPLE_COMMAND, (BYTE)CC_Status);
			kPacket.Push( (BYTE)CoupleCR_None );
			kPacket.Push( rkOwnerUser.kCharGuid );
			kPacket.Push( rkOwnerUser.kCoupleGuid );
			kPacket.Push( pkCurMsgr->LinkStatus() );
			kPacket.Push( pkCurMsgrTemp->LinkStatus() );
			SendToCoupleMgr(kPacket);
		}		
	}
}

void PgFriendMgr::NfyToMyFriend(BM::GUID const &rkCharGuid)
{
	ContFriendVec kVec;
	ContMessenger::iterator msgr_itor = m_kContMsgr.begin();
	while(msgr_itor != m_kContMsgr.end())
	{
		SFriendItem kItem;
		if( S_OK == (*msgr_itor).second->Get(rkCharGuid, kItem) )
		{
			SFriendItem kFriendItem;
			if ( S_OK == (*msgr_itor).second->Get(rkCharGuid, kFriendItem) )
			{
				kVec.clear();
				kVec.push_back(kFriendItem);
				(*msgr_itor).second->Modify(kFriendItem);
				(*msgr_itor).second->SendModify(kVec, FCT_MODIFY);
			}
		}
		++msgr_itor;
	}
}

HRESULT PgFriendMgr::GetFriendMsnState(PgMessenger *pkMsgr)
{
	if(!pkMsgr)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
	ContFriendVec kFrndVec;
	pkMsgr->GetFriendList(kFrndVec);

	ContFriendVec::iterator frnd_itor = kFrndVec.begin();
	while(frnd_itor != kFrndVec.end())
	{
		PgMessenger *pkTargetMsgr = GetMessenger((*frnd_itor).CharGuid());
		SFriendItem kMyItem;
		BYTE cLink = FLS_OFFLINE;
		if (!pkTargetMsgr || E_FAIL == pkTargetMsgr->Get(pkMsgr->CharGuid(), kMyItem) 
			|| (kMyItem.ChatStatus() & FCS_ADD_WAIT_REQ) == FCS_ADD_WAIT_REQ
			|| (kMyItem.ChatStatus() & FCS_ADD_WAIT_ANS) == FCS_ADD_WAIT_ANS )
		{
			cLink = FLS_OFFLINE;	
		}
		else
		{
			cLink = pkTargetMsgr->LinkStatus();
		}
		(*frnd_itor).LinkStatus(cLink);
		pkMsgr->Modify((*frnd_itor));
		++frnd_itor;
	}

	return S_OK;
}

void PgFriendMgr::DelMyInfoFormOther(SContentsUser const &rkOwnerUser, VEC_GUID const &rkContFriendChar)
{
	VEC_GUID::const_iterator frnd_iter = rkContFriendChar.begin();
	PgMessenger *pkTargetMsgr = NULL;
	while (rkContFriendChar.end()!=frnd_iter)
	{
		pkTargetMsgr = GetMessenger((*frnd_iter));	//지울 넘의 메신져를 찾아서
		if (pkTargetMsgr)	//로그인 했으먼
		{
			SFriendItem kItem;
			ContFriendVec kVec;
			pkTargetMsgr->Get(rkOwnerUser.kCharGuid,kItem);
			kItem.LinkStatus(FLS_OFFLINE);
			kItem.ChatStatus(FCS_ADD_DELETED);
			pkTargetMsgr->Modify(kItem);
			kVec.push_back(kItem);
			pkTargetMsgr->SendModify(kVec, FCT_MODIFY);

			pkTargetMsgr->SendToDB(&kItem, FCT_MODIFY); //지울놈의 DB를 업데 시켜주자
		}
		++frnd_iter;
	}	
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// PgMyFriendList
void PgMessenger::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);

	DelAll();

	m_kCharGuid.Clear();
	LinkStatus(0);
	m_kName.clear();
}

bool PgMessenger::IsExist(BM::GUID const &rkGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);

	ContFriendItem::const_iterator kItem_iter = m_kFriendList.find(rkGuid);
	return (m_kFriendList.end() != kItem_iter);
}

HRESULT PgMessenger::Get(BM::GUID const &rkGuid, SFriendItem &rkItem) const
{
	BM::CAutoMutex kLock(m_kMutex);

	ContFriendItem::const_iterator kItem_iter = m_kFriendList.find(rkGuid);
	if(m_kFriendList.end() != kItem_iter)
	{
		rkItem = *kItem_iter->second;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgMessenger::Modify(const SFriendItem &rkItem)
{
	BM::CAutoMutex kLock(m_kMutex);

	ContFriendItem::iterator kItem_iter = m_kFriendList.find(rkItem.CharGuid());
	if(m_kFriendList.end() != kItem_iter)
	{
		*(kItem_iter->second) = rkItem;

		//SendModify에서 전송하도록 하자
	//	ContFriendVec kFrndVec;
	//	kFrndVec.push_back(rkItem);
	//	SendModify(kFrndVec, FCT_MODIFY);
		
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgMessenger::SendModify(ContFriendVec const &kFrndVec, EFriendCommandType const eType)
{
	//if(kFrndVec.empty()) //내용물이 없어도 패킷은 보내야지
	//{
	//	return S_OK;
	//}

	BM::Stream kPacket(PT_N_C_NFY_MSN_ELEMENT_MODIFY, (BYTE)eType);
	kPacket.Push((BYTE)FEC_Success);
	kPacket.Push((int)kFrndVec.size());

	ContFriendVec::const_iterator frnd_itor = kFrndVec.begin();
	while(frnd_itor != kFrndVec.end())
	{
		const SFriendItem &kItem = (*frnd_itor);
		kItem.WriteToPacket(kPacket);//Delete, Modify, Add
	//	SendToDB(&kItem, eType); //일단 막아놓자
		++frnd_itor;
	}
	
	g_kRealmUserMgr.Locked_SendToUser(CharGuid(), kPacket, false);
	return S_OK;
}


HRESULT PgMessenger::AddFriend(const SFriendItem &kItem, bool bIsNew)
{
	BM::CAutoMutex kLock(m_kMutex);
	SFriendItem *pkFriendItem = NULL;

	pkFriendItem = m_kPoolFriendItem.New();
	if(!pkFriendItem)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
//////////이 아래 실패 리턴은 __FAILED_OBJ_REMOVE 로 가시오.
	*pkFriendItem = kItem;

	if (bIsNew)
	{
		pkFriendItem->LinkStatus(FLS_OFFLINE); //처음 추가했을 땐 오프라인으로 보이자
	}
	auto ibRet = m_kFriendList.insert(std::make_pair(pkFriendItem->CharGuid(), pkFriendItem));
	if(!ibRet.second)
	{
		goto __FAILED_OBJ_REMOVE;
	}
	goto __SUCCESS;
	return S_OK;

__FAILED_OBJ_REMOVE:
	{
		m_kPoolFriendItem.Delete(pkFriendItem);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
__SUCCESS:
	{
		//DB저장 쿼리 (나)
		if (bIsNew)
		{
			SendToDB(pkFriendItem, (BYTE)FCT_ADD_BYGUID);

			ContFriendVec kFrndVec;
			kFrndVec.push_back(*pkFriendItem);
			SendModify(kFrndVec, FCT_ADD_BYGUID);
		}
		return S_OK;
	}
}

HRESULT PgMessenger::AddFriend(SContentsUser const &kItem, BYTE const eState, bool bIsNew)
{
	SFriendItem kFriendItem;
	kFriendItem.Set(kItem, eState);
	return AddFriend(kFriendItem, bIsNew);
}

HRESULT PgMessenger::Del(VEC_GUID const &rkFriendCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	VEC_GUID::const_iterator guid_iter = rkFriendCharGuid.begin();

	HRESULT hRet = S_OK;

	ContFriendVec kContDeleted;
	while(guid_iter != rkFriendCharGuid.end() )
	{
		ContFriendItem::iterator kItem_iter = m_kFriendList.find((*guid_iter));
		if(m_kFriendList.end() != kItem_iter)
		{
			ContFriendItem::mapped_type pkElement = kItem_iter->second;
//			if( pkElement->ChatStatus() & FCS_ADD_WAIT_REQ )
//			{
//				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
//				return S_FALSE;
//			}
			{
				SendToDB(pkElement, (BYTE)FCT_DELETE);	//내 DB에서 지우고
			}

			kContDeleted.push_back(*pkElement);

			m_kPoolFriendItem.Delete(pkElement);
			m_kFriendList.erase(kItem_iter);
		}
		++guid_iter;
	}


	SendModify(kContDeleted, FCT_DELETE);
	return S_OK;
}

HRESULT PgMessenger::DelAll()
{
	BM::CAutoMutex kLock(m_kMutex);

	ContFriendItem::iterator kItem_iter = m_kFriendList.begin();
	while(m_kFriendList.end() != kItem_iter)
	{
		ContFriendItem::mapped_type kElement = kItem_iter->second;
		m_kPoolFriendItem.Delete(kElement);
		++kItem_iter;
	}

	m_kFriendList.clear();
	return S_OK;
}

void PgMessenger::DelFromListOnly(VEC_GUID const &rkFriendCharGuid)	//디비에 쿼리하지 않고 단순히 리스트에서만 지우자.
{
	BM::CAutoMutex kLock(m_kMutex);

	VEC_GUID::const_iterator guid_iter = rkFriendCharGuid.begin();

	while (rkFriendCharGuid.end() != guid_iter)
	{
		ContFriendItem::iterator frn_iter = m_kFriendList.find((*guid_iter));
		if (m_kFriendList.end() != frn_iter)
		{
			m_kFriendList.erase(frn_iter);
		}
		++guid_iter;
	}
}

void PgMessenger::GetFriendList(ContFriendVec &rkFrndVec) const
{
	BM::CAutoMutex kLock(m_kMutex);

	ContFriendItem::const_iterator kItem_iter = m_kFriendList.begin();
	while(m_kFriendList.end() != kItem_iter)
	{
		ContFriendItem::mapped_type pkElement = kItem_iter->second;
		rkFrndVec.push_back(*pkElement);
		++kItem_iter;
	}
}

// bool PgMessenger::MapMove(BM::GUID const &rkGuid, int iMapNo)
// {
// 	BM::CAutoMutex kLock(m_kMutex);
// 	SFriendItem kItem;
// 	if(S_OK == Get(rkGuid, kItem))
// 	{
// 		kItem.MapNo(iMapNo);
// 		return true;
// 	}
// 	return false;
// }

bool PgMessenger::LinkChange(BM::GUID const &rkGuid, BYTE cLinkStatus)
{
	BM::CAutoMutex kLock(m_kMutex);
	SFriendItem kItem;
	if(S_OK == Get(rkGuid, kItem))
	{
		kItem.LinkStatus(cLinkStatus);
		Modify(kItem);
		return true;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool PgMessenger::SendToDB(const SFriendItem *kItem, BYTE const eType)
{
	if (kItem==NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	BM::Stream kDbPacket(PT_N_T_REQ_MSN_FRIENDCOMMAND, CharGuid());
	kDbPacket.Push((BYTE)eType);
	kItem->WriteToDBPacket(kDbPacket);
	return SendToPacketHandler(kDbPacket);
}

//메신져(from 서버)

bool PgFriendMgr::MakeMsn(SContentsUser const &rkUser)
{//풀로 받고 이 작업을 해야함.
	BM::CAutoMutex kLock(m_kMutex);
// 나를 알려줘야하고!. 
// 내가 데리고 있는 애들을 가져와야됨.->->-> 내 정보를 DB에서 받았을때. 가져오면됨.
	PgMessenger *pkMsgr = GetMessenger(rkUser.kCharGuid);
	if(!pkMsgr)
	{
		pkMsgr = NewMessenger(rkUser.kCharGuid);
		if(!pkMsgr)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	pkMsgr->Name(rkUser.kName);
	pkMsgr->LinkStatus(FLS_ONLINE);
	pkMsgr->CharGuid(rkUser.kCharGuid);
	return true;
}
