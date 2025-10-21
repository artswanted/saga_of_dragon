#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "AilePack/Constant.h"
#include "AilePack/GALA_Constant.h"
#include "Constant.h"
#include "GALAServer.h"

#include "Nexon.h"
#include "Nexon/AuthCheck.h"

DWORD PgJapanServerImpl::GetNexonCashRequestKey()
{//딴데서 동시에 호출하면 말짱황.
	return ::InterlockedIncrement(&m_dwLastNexonCashRequestKey);//늘어난 값 리턴 됨.
}

//DAEMON->GAME : [XXX_Item] [Purchase_Confirm] {Request_Id} 혹은
//DAEMON->GAME : [XXX_Item] [Purchase_Cancel] {Request_Id}

void PgJapanServerImpl::RecvPT_A_CN_NFY_TRAN_COMMIT(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket, bool const bIsCommit)
{
//	0원 예외에서 타임오버 발생.
//커밋 날리면 아래 메세지가 최종적으로 온다고함
//	DAEMON->GAME : [XXX_Item] [Purchase_Confirm] {Request_Id} 혹은
//	DAEMON->GAME : [XXX_Item] [Purchase_Cancel] {Request_Id}
	BM::GUID kTranKey;
	BM::Stream::DEF_STREAM_TYPE ret_type;
	pkPacket->Pop(kTranKey);
	pkPacket->Pop(ret_type);

	//이걸. -> 트랜잭션 ID를 보넀다가 받아서 Commit 처리를 해주자.
	CONT_FINAL_COMMIT::const_iterator tran_itor = m_kContFinalTran.find(kTranKey);

	if(tran_itor != m_kContFinalTran.end())
	{
		CONT_FINAL_COMMIT::mapped_type const &kElem = tran_itor->second;
		BM::Stream kCommitPacket;
		
		eJAPAN_CASH_HEADER subtype = Purchase_Cancel;
		if(bIsCommit)
		{
			subtype = Purchase_Confirm;
		}

		if(bIsCommit)
		{
			U_STATE_LOG( BM::LOG_LV0, _T("Commit!/ ID:")<< kElem.m_strMemberID<< L" Key:" << kElem.m_dwRequestKey <<L" DBID:" << kElem.m_kDBID <<  L" TranKey:" << kElem.m_kJapanTranID);
		}
		else
		{
			U_STATE_LOG( BM::LOG_LV0, _T("Rollback!/ ID:")<< kElem.m_strMemberID<< L" Key:" << kElem.m_dwRequestKey <<L" DBID:" << kElem.m_kDBID <<  L" TranKey:" << kElem.m_kJapanTranID);
		}

		if(PT_A_CN_REQ_BUYCASHITEM_ROLLBACK == ret_type)
		{//응답으로 전환
			ret_type = PT_CN_A_ANS_BUYCASHITEM_ROLLBACK;
		}

		SNexonCashRequest kReqInfo(XXX_Item, subtype, kElem.m_dwRequestKey, kElem.m_kDBID, kElem.m_kJapanTranID, ret_type, rkFrom, pkPacket);//tran 용
		kReqInfo.m_bIsSimulate = kElem.m_bIsSimulate;//시뮬레이트 값은 기존것을 사용해야함
		kReqInfo.m_kTranKey = kElem.m_kTranKey;//트랜잭션값
		
		CAutoTran kAutoTran(*this);
		kAutoTran.SetFinalCommit();//파이널 트랜잭션용으로 셋팅

		kAutoTran.PrepareData(kReqInfo);//대기시에 새로운 ReqKey 를 셋팅 했으므로 문제 없다.
	}
}

void PgJapanServerImpl::RecvPT_A_CN_REQ_QUERY_CASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
//	Req 키를 받아와서 
//	캐쉬 쿼리 -> Map 같은데 담고.
//		-> 정보는 멤버.-> 캐릭.
//	보내기. 끝.

	DWORD const dwReqKey = GetNexonCashRequestKey();
	std::wstring kAccountID;
	DWORD dwUID;
	CEL::ADDR_INFO kAddress;
	pkPacket->Pop(kAccountID);
	pkPacket->Pop(dwUID);
	kAddress.ReadFromPacket(*pkPacket);
	

	U_STATE_LOG( BM::LOG_LV0, _T("Recv Req Cash Balance/ ID:") << kAccountID << L" Key:" << dwReqKey);

	SNexonCashRequest kReqInfo(XXX_Item, Remain_Cash, dwReqKey, kAccountID, PT_CN_A_ANS_QUERY_CASH, rkFrom, pkPacket);
	CAutoTran kAutoTran(*this);
	kAutoTran.PrepareData(kReqInfo);
}

void PgJapanServerImpl::RecvUseCash(BM::Stream::DEF_STREAM_TYPE const wSubType, LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	DWORD const dwReqKey = GetNexonCashRequestKey();

	std::wstring kAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring kCharacterName;
	CONT_BUYITEM kBuyItem;
	DWORD dwUID;
	CASH::SCashCost kCost;
	BM::GUID kTranKey;

	pkPacket->Pop(kAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(kCharacterName);
	pkPacket->Pop(dwUID);
	kCost.ReadFromPacket(*pkPacket);
	
	PU::TLoadArray_M(*pkPacket, kBuyItem);
	
	pkPacket->Pop(kTranKey);

	U_STATE_LOG( BM::LOG_LV1, _T("Recv Req Item Buy/ ID:") << kAccountID << L" Key:" << dwReqKey);

	if(!kBuyItem.size())
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("There is No Item") << dwReqKey);
		U_STATE_LOG( BM::LOG_LV0, _T("There is No Item") << dwReqKey);
		return;
	}

	SBuyItemInfo const& rkItem = kBuyItem.at(0);

	int const item_id = rkItem.iPriceIdx;
	int const price= (-kCost.i64UsedCash);//캐시양은 양수
	short const product_type = XXX_Item;
	BYTE const domaintype = rkFrom.nSiteNo;
	short const server_type = rkFrom.nRealm;

	WORD wRetType = 0;
	switch(wSubType)
	{
	case PT_A_CN_REQ_BUYCASHITEM:	{wRetType = PT_CN_A_ANS_BUYCASHITEM;}break;
	case PT_A_CN_REQ_EXPANDTIEM:	{wRetType = PT_CN_A_ANS_EXPANDTIEM;}break;
	default:
		{//이럴일 없음
		}break;
	}		

	//여러개 구매면. order 번호를 1로
	SNexonCashRequest kReqInfo(XXX_Item, Purchase, dwReqKey, ((1 == kBuyItem.size())?0:1),item_id, price, kAccountID, kCharacterName, domaintype, server_type, L"", 
		wRetType, rkFrom, kTranKey, pkPacket);
	
	if(0 > price )//돈이 차는건 용납 안됨.
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("Price Is Minus/ ID:") << kAccountID << L"Key:" << dwReqKey);
		U_STATE_LOG( BM::LOG_LV0, _T("Price Is Minus / Key:") << dwReqKey);
		SendCashFailResult(kReqInfo);
	}
	else
	{
		CAutoTran kAutoTran(*this);
		kAutoTran.PrepareData(kReqInfo);
	}
}

void PgJapanServerImpl::RecvPT_A_CN_REQ_SENDGIFT(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	DWORD const dwReqKey = GetNexonCashRequestKey();

	DWORD dwUID;
	std::wstring strAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring strCharacterName;
	std::wstring strReceiverName;
	DWORD dwItemNo;
	std::wstring strItemName;
	WORD wItemTime;
	CASH::SCashCost kCost;
	BM::GUID kTranKey;

	pkPacket->Pop(dwUID);
	pkPacket->Pop(strAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(strCharacterName);
	pkPacket->Pop(strReceiverName);
	pkPacket->Pop(dwItemNo);
	pkPacket->Pop(strItemName);
	pkPacket->Pop(wItemTime);
	kCost.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kTranKey);
////////////////////////////////////////////////////////////////////////////////////

//여기 맞춰야댐.
//	if (0 != kCost.i64UsedMileage || 0 != kCost.i64AddedMileage)
//	{
//		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Not support MileageUsage or AddMileage Cost=") << kCost.ToString());
//		SendNexonCashResult(SNexonCashRequest(strAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_SENDGIFT, rkFrom, pkPacket), kBill);
//		return;
//	}

	U_STATE_LOG( BM::LOG_LV2, _T("Recv Req Send Gift/ ID:")<< strAccountID  << L" Key:" << dwReqKey);

	int const item_id = dwItemNo;
	int const price= (-kCost.i64UsedCash);//캐시양은 양수
	short const product_type = XXX_Item;
	BYTE const domaintype = rkFrom.nSiteNo;
	short const server_type = rkFrom.nRealm;

	SNexonCashRequest kReqInfo(XXX_Item, Gift, dwReqKey, 0i64, item_id, price, strAccountID, strCharacterName, domaintype, server_type, strReceiverName, 
		PT_CN_A_ANS_SENDGIFT, rkFrom, kTranKey, pkPacket);

	if(0 == price)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("Gift Price Is 0 / Key:") << dwReqKey);
		U_STATE_LOG( BM::LOG_LV0, _T("Gift Price Is 0 / Key:") << dwReqKey);

		SendCashFailResult(kReqInfo, GALA::EGTDX_ERROR_CANNOT_BUY);//살 수 없다 0 원짜리는 선물 하지 말것
	}
	else if(0 > price )//돈이 차는건 용납 안됨.
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("Gift Price Is Minus / Key:") << dwReqKey);
		U_STATE_LOG( BM::LOG_LV0, _T("Gift Price Is Minus / Key:") << dwReqKey);
		SendCashFailResult(kReqInfo);
	}
	else
	{
		CAutoTran kAutoTran(*this);
		kAutoTran.PrepareData(kReqInfo);
	}
}

void PgJapanServerImpl::OnConnectFromNexonCash( CEL::CSession_Base *pkSession )
{
	PgSessionMgr::OnConnected(pkSession);
}

void PgJapanServerImpl::OnDisconnectFromNexonCash( CEL::CSession_Base *pkSession )
{
	PgSessionMgr::OnDisconnected(pkSession);
}

std::wstring ErrorNoToStr(BYTE const eError)
{
	switch(eError)
	{
	case Remain_Cash: {return L"Remain_Cash";}break;
	case Purchase: {return L"Purchase";}break;
	case Purchase_Ok : {return L"Purchase_Ok";}break;
	case Purchase_Confirm: {return L"Purchase_Confirm";}break;
	case Purchase_Cancel: {return L"Purchase_Cancel";}break;
	case No_Remain: {return L"No_Remain";}break;
	case DB_Error: {return L"DB_Error";}break;
	case Gift: {return L"Gift";}break;
	case Gift_Limit_Over: {return L"Gift_Limit_Over";}break;
	case Gift_Not_Ready : {return L"Gift_Not_Ready";}break;
	case Gift_Under_Age : {return L"Gift_Under_Age";}break;
	default:{return L"UnKnown";}break;
	}
}

void PgJapanServerImpl::OnRecvFromNexonCash(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{	// 세션키로 해당 채널을 찾아 메시지를  multiplexing 함.
	BYTE byTop= 0;
	BYTE byCase= 0;
	int iRequestID= 0;
	NP_Pop(*pkPacket, byTop);
	NP_Pop(*pkPacket, byCase);
	NP_Pop(*pkPacket, iRequestID);

//	트랜잭션인경우에 롤백 콜.
	CONT_JAPAN_CASH_REQUEST::const_iterator req_itor = m_kNexonCashRequest.find(iRequestID);
	if(req_itor != m_kNexonCashRequest.end())
	{
		CONT_JAPAN_CASH_REQUEST::mapped_type const &kReqElem = req_itor->second;
		if(byTop == XXX_Item)
		{
			int iRemainCash;
			WORD wRetCode = 0;//EGTDX_ERROR_SUCCESS;
			switch(byCase)
			{
			case Purchase_Confirm://트랜잭션 결과 패킷
				{
					U_STATE_LOG( BM::LOG_LV3, _T("Cash Ans: Commit OK Key:") << iRequestID);
					BM::Stream kDPacket(kReqElem.wReqPacketType);
					kDPacket.Push(kReqElem.m_kOrgPacket);
					SendCashResult(kReqElem.kFromSI, kDPacket);
				}break;
			case Purchase_Cancel://트랜잭션 결과 패킷
				{
					U_STATE_LOG( BM::LOG_LV3, _T("Cash Ans: RollBack OK Key Key:") << iRequestID);
					////////////////////////////////////////////////////////////////////////////
					BM::Stream kDPacket(kReqElem.wReqPacketType);
					
					kDPacket.Push((WORD)0);//성공
					kDPacket.Push((DWORD)0);//현재 돈
					kDPacket.Push((DWORD)0);//마일리지
					kDPacket.Push((DWORD)0);//추가 마일리지
					kDPacket.Push(kReqElem.m_kOrgPacket);//ECASH_TRAN_ADD_CASH// 요청때 부터 붙어 와야됨
					SendCashResult(kReqElem.kFromSI, kDPacket);
					////////////////////////////////////////////////////////////////////////////
				}break;
			case Remain_Cash:
				{//캐쉬값
					NP_Pop(*pkPacket, iRemainCash);

					U_STATE_LOG( BM::LOG_LV3, _T("Cash Ans: Balance/ PT:" << kReqElem.wReqPacketType <<L" Key:") << iRequestID << L" Cash:" <<iRemainCash);
					////////////////////////////////////////////////////////////////////////////
					BM::Stream kDPacket(kReqElem.wReqPacketType);
					
					kDPacket.Push((WORD)0);
					kDPacket.Push(kReqElem.m_strMemberID);
					kDPacket.Push(iRemainCash);
					kDPacket.Push((int)0);	// Mileage
					kDPacket.Push(kReqElem.m_kOrgPacket);//ECASH_TRAN_ADD_CASH// 요청때 부터 붙어 와야됨
					SendCashResult(kReqElem.kFromSI, kDPacket);
					////////////////////////////////////////////////////////////////////////////
				}break;
			case No_Remain://잔고 부족
			case DB_Error://디비에러
			case Gift_Under_Age://나이제한 걸림
			case Gift_Limit_Over:
			case Purchase_Ok://
				{
					U_STATE_LOG( BM::LOG_LV3, _T("Cash Ans: Result/ Key:") << iRequestID << L" Case: "<< ErrorNoToStr(byCase));

					if(No_Remain == byCase){wRetCode = GALA::EGTDX_ERROR_CASH_SHORTAGE;}//잔고 부족//;
					if(DB_Error == byCase){wRetCode = GALA::EGTDX_ERROR_BILLINGSERVER_ERROR;}//
					if(Gift_Under_Age == byCase){wRetCode = GALA::EGTDX_ERROR_BILLINGSERVER_ERROR;}//나이제한 걸림//
					if(Gift_Limit_Over == byCase)
					{//선물 한도 오버.
						int amount = 0;//의미 없는값.
						NP_Pop(*pkPacket, amount);
						wRetCode = GALA::EGTDX_ERROR_CANNOT_BUY;//
					}

					BYTE byDBID = 0;
					int iTransactionID = 0;

					if(Purchase_Ok == byCase)
					{	//트랜잭션 대기에 넣어주자.
						NP_Pop(*pkPacket, byDBID);
						NP_Pop(*pkPacket, iTransactionID);
						
						CONT_JAPAN_CASH_REQUEST::mapped_type kCopyTran = kReqElem;
						kCopyTran.m_dwRequestKey = GetNexonCashRequestKey();//ReqKey 새거 할당
						kCopyTran.m_kDBID = byDBID;
						kCopyTran.m_kJapanTranID = iTransactionID;
						kCopyTran.ui64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;

						U_STATE_LOG( BM::LOG_LV1, _T("Ready for Commit!/ Key:") << kCopyTran.m_dwRequestKey <<L" DBID:" << kCopyTran.m_kDBID <<  L" TranKey:" << kCopyTran.m_kJapanTranID);
						
						m_kContFinalTran.insert(std::make_pair(kReqElem.m_kTranKey, kCopyTran));
					}
					//////////////////////////////////////////////////////
					BM::Stream kDPacket(kReqElem.wReqPacketType);

					BM::GUID kCharacterGuid;
					std::wstring strAccountID;
					DWORD dwCash = 0;
					DWORD dwMileage = 0;
					DWORD dwAddedMileage = 0;
					std::wstring strChargeNo;
					kDPacket.Push(wRetCode);
					kDPacket.Push(kReqElem.m_kCharacterGUID);
					kDPacket.Push(kReqElem.m_strMemberID);
					kDPacket.Push(dwCash);
					kDPacket.Push(dwMileage);
					kDPacket.Push(dwAddedMileage);
					kDPacket.Push(strChargeNo);
					kDPacket.Push(kReqElem.m_kOrgPacket);//ECASH_TRAN_ADD_CASH// 요청때 부터 붙어 와야됨
					SendCashResult(kReqElem.kFromSI, kDPacket);
					//////////////////////////////////////////////////////
				}break;
			}
		}
		m_kNexonCashRequest.erase(iRequestID);//삭제.
	}
}

bool PgJapanServerImpl::OnRegistConnector(CEL::SRegistResult const &rkArg)
{
	BM::CAutoMutex kLock(m_kMutex);
	std::wstring wstrFilename(g_kProcessCfg.ConfigDir() + _T("Consent_JAPAN_Config.ini"));
	switch (rkArg.iIdentityValue)
	{
	case CEL::ST_IMMIGRATION:
		{
			LoadImmigration( rkArg.guidObj, wstrFilename );
		}break;
	case CEL::ST_EXTERNAL1:
		{//여기에 넥슨 캐쉬 데몬.
			TCHAR chValue[100] = {0,};
			int iValue;

//			GetPrivateProfileString(_T("NEXON"), _T("IP"), _T("127.0.0.1"), chValue, sizeof(chValue), wstrFilename.c_str());
//			iValue = GetPrivateProfileInt(_T("NEXON"), _T("PORT"), 12345, wstrFilename.c_str());

			GetPrivateProfileString(_T("NEXON"), _T("IP"), _T("59.128.93.158"), chValue, sizeof(chValue), wstrFilename.c_str());
			iValue = GetPrivateProfileInt(_T("NEXON"), _T("PORT"), 9006, wstrFilename.c_str());
			AddAddress(NexonCash_SERVER_SITE_NUM, rkArg.guidObj, chValue, static_cast<WORD>(iValue));
		}break;
	default:
		{
			U_STATE_LOG(BM::LOG_LV5, __FL__ << _T("Unknown ServerType=") << rkArg.iIdentityValue);
			return false;
		}break;
	}
	return true;
}

void PgJapanServerImpl::Timer5s()
{
	{
		ACE_UINT64 const ui64Now = BM::GetTime64();
		CONT_JAPAN_CASH_REQUEST::iterator itor_NexonCash = m_kNexonCashRequest.begin();
		while( m_kNexonCashRequest.end() != itor_NexonCash)
		{
			CONT_JAPAN_CASH_REQUEST::mapped_type const &kElem = itor_NexonCash->second;
			if( kElem.ui64ExpireTime < ui64Now )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV3, BM::vstring(std::wstring(_T("Nexon Req TimeOver/ Key:"))) << kElem.m_dwRequestKey << L" Main:" << (int)kElem.m_eMainType << L" Sub:" << (int)kElem.m_eSubType << L" ID:" << kElem.m_strMemberID );
				U_STATE_LOG( BM::LOG_LV3, BM::vstring(std::wstring(_T("Nexon Req TimeOver/ Key:"))) << kElem.m_dwRequestKey << L" Main:" << (int)kElem.m_eMainType << L" Sub:" << (int)kElem.m_eSubType << L" ID:" << kElem.m_strMemberID );
				itor_NexonCash = m_kNexonCashRequest.erase(itor_NexonCash);
				continue;
			}
			++itor_NexonCash;
		}
	}
	{
		ACE_UINT64 const ui64Now = BM::GetTime64();
		CONT_FINAL_COMMIT::iterator commit_itor = m_kContFinalTran.begin();
		while(m_kContFinalTran.end() != commit_itor)
		{
			CONT_FINAL_COMMIT::mapped_type const &kElem = commit_itor->second;
			if( kElem.ui64ExpireTime < ui64Now)
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV3, BM::vstring(std::wstring(_T("Nexon Req Commit TimeOver/ Key:"))) << kElem.m_dwRequestKey << 
					L" Main:" << (int)kElem.m_eMainType << L" Sub:" << (int)kElem.m_eSubType << L" ID:" << kElem.m_strMemberID <<
					L" DBID:" << kElem.m_kDBID <<  L" TranKey:" << kElem.m_kJapanTranID);

				U_STATE_LOG( BM::LOG_LV3, BM::vstring(std::wstring(_T("Nexon Req Commit TimeOver/ Key:"))) << kElem.m_dwRequestKey << 
					L" Main:" << (int)kElem.m_eMainType << L" Sub:" << (int)kElem.m_eSubType << L" ID:" << kElem.m_strMemberID <<
					L" DBID:" << kElem.m_kDBID <<  L" TranKey:" << kElem.m_kJapanTranID);

				commit_itor = m_kContFinalTran.erase(commit_itor);
				continue;
			}
			++commit_itor;
		}
	}

	PgSiteMemberMgr::Timer5s();
}

bool PgJapanServerImpl::SendToNexonCash( BM::Stream const &kPacket )const
{
	return PgSessionMgr::SendToServer(NexonCash_SERVER_SITE_NUM, kPacket);
}

bool PgJapanServerImpl::AddNexonCashRequest(SNexonCashRequest const &kReqInfo)
{
	auto ibRet = m_kNexonCashRequest.insert(std::make_pair(kReqInfo.m_dwRequestKey, kReqInfo));
	return ibRet.second;
}

bool PgJapanServerImpl::RemoveNexonCashRequest(DWORD const dwReqKey)
{
	m_kNexonCashRequest.erase(dwReqKey);
	return true;
}

bool PgJapanServerImpl::RemoveFinalCommit(BM::GUID const &kTranKey)
{
	m_kContFinalTran.erase(kTranKey);
	return true;
}

bool PgJapanServerImpl::SendCashResult(SERVER_IDENTITY_SITE const& rkFromSI, BM::Stream const& rkPacket)
{
	BM::Stream kFPacket(PT_X_N_ANS_CASH);
	dynamic_cast<tagServerIdentity const&>(rkFromSI).WriteToPacket(kFPacket);
	kFPacket.Push(rkPacket);
	return SendToServer(rkFromSI.nSiteNo, kFPacket);
}

bool PgJapanServerImpl::SendCashFailResult(SNexonCashRequest const &kNexonReq, WORD const wRetCode)
{
	BM::Stream kFPacket(PT_X_N_ANS_CASH);
	dynamic_cast<tagServerIdentity const&>(kNexonReq.kFromSI).WriteToPacket(kFPacket);
	kFPacket.Push(kNexonReq.wReqPacketType);

	switch(kNexonReq.wReqPacketType)
	{
	case PT_CN_A_ANS_QUERY_CASH:
		{
			std::wstring strAccountID = kNexonReq.m_strMemberID;
			DWORD dwCash=0, dwMileage=0;
			kFPacket.Push(wRetCode);
			kFPacket.Push(strAccountID);
			kFPacket.Push(dwCash);
			kFPacket.Push(dwMileage);
		}break;
	case PT_CN_A_ANS_BUYCASHITEM:
	case PT_CN_A_ANS_EXPANDTIEM:
	case PT_CN_A_ANS_SENDGIFT:
		{
			BM::GUID kCharacterGuid = kNexonReq.m_kCharacterGUID;
			std::wstring strAccountID = kNexonReq.m_strMemberID;
			DWORD dwCash = 0;
			DWORD dwMileage = 0;
			DWORD dwAddedMileage = 0;
			std::wstring strChargeNo;
			kFPacket.Push(wRetCode);
			kFPacket.Push(kCharacterGuid);
			kFPacket.Push(strAccountID);
			kFPacket.Push(dwCash);
			kFPacket.Push(dwMileage);
			kFPacket.Push(dwAddedMileage);
			kFPacket.Push(strChargeNo);
		}break;
	case PT_CN_A_ANS_BUYCASHITEM_ROLLBACK:
		{
			DWORD dwCashBalance = 0;
			DWORD dwMileage = 0;
			DWORD dwAddedMileage = 0;
			kFPacket.Push(wRetCode);
			kFPacket.Push(dwCashBalance);
			kFPacket.Push(dwMileage);
			kFPacket.Push(dwAddedMileage);
		}break;
	default:
		{
		}break;
	}

	kFPacket.Push(kNexonReq.m_kOrgPacket);//Tran 값
	return SendToServer(kNexonReq.kFromSI.nSiteNo, kFPacket);
}

bool PgJapanServerImpl::CashSimulate(SNexonCashRequest &kReqInfo)
{
	BM::Stream * pkPacket = &kReqInfo.m_kGDPacket;
	
	BYTE byPri;
	BYTE bySub;
	int dwReqKey;
	
	std::string strMemberID;
	std::string strCharName;

	NP_Pop(*pkPacket, byPri);
	NP_Pop(*pkPacket, bySub);
	NP_Pop(*pkPacket, dwReqKey);

	BM::Stream kAnsPacket;

	NP_Push(kAnsPacket, byPri);
	NP_Push(kAnsPacket, bySub);
	NP_Push(kAnsPacket, dwReqKey);

	if( XXX_Item == byPri
	&&	(Purchase == bySub ||	Gift == bySub))
	{//구입
		if(Purchase == bySub )
		{
			U_STATE_LOG( BM::LOG_LV1, _T("Simulate Buy ReqKey:") << dwReqKey);
		}
		
		if(Gift == bySub)
		{
			U_STATE_LOG( BM::LOG_LV2, _T("Simulate Gift ReqKey:") << dwReqKey);
		}

		int item_id= 0, price= 0;
		short product_type = 0;
		__int64 orderid = 0;
		BYTE domaintype = 0;
		short server_type = 0;

		NP_Pop(*pkPacket, product_type);
		NP_Pop(*pkPacket, orderid);
		NP_Pop(*pkPacket, item_id);
		NP_Pop(*pkPacket, price);
		NP_Pop(*pkPacket, strMemberID);
		NP_Pop(*pkPacket, strCharName);//캐릭명
		NP_Pop(*pkPacket, (BYTE)domaintype);
		NP_Pop(*pkPacket, server_type);//서버타입

		if(Gift == bySub)
		{
			std::string strTargetID;
			std::string strSSN;
			NP_Pop(*pkPacket, strTargetID);
			NP_Pop(*pkPacket, strSSN);
		}
		
		{//답문
			BM::Stream kDGPacket;
			BYTE byA = 0, byRet = 0, byDBID = 0;
			int  iRemainCash = 0, iTransactionID = 0;
			
			switch(2)
			{
/*			case 0:
				{//잔고부족
					NP_Push(kDGPacket, (BYTE)XXX_Item);
					NP_Push(kDGPacket, (BYTE)No_Remain);
					NP_Push(kDGPacket, (int)dwReqKey);
				}break;
			case 1:
				{//디비오류
					NP_Push(kDGPacket, (BYTE)XXX_Item);
					NP_Push(kDGPacket, (BYTE)DB_Error);
					NP_Push(kDGPacket, (int)dwReqKey);
				}break;
*/			case 2:
				{//
					NP_Push(kDGPacket, (BYTE)XXX_Item);
					NP_Push(kDGPacket, (BYTE)Purchase_Ok);
					NP_Push(kDGPacket, (int)dwReqKey);
					
					//아래 두개를 서버로 쏴주면 지급 완료.
					NP_Push(kDGPacket, (BYTE)(rand()%255));//DBID
					NP_Push(kDGPacket, (int)(rand()));//트랜ID

//					[XXX_Item] [Purchase_Confirm] {Request_Id} [DB_Id] {Transaction_Id} 이 포맷으로 게임서버가 응답을 주면됨
				}break;
			}
//			pkSession->VSend(kDGPacket);
			OnRecvFromNexonCash(NULL, &kDGPacket);
		}
	}

	if( XXX_Item == byPri
	&&	(Purchase_Confirm == bySub || Purchase_Cancel == bySub) )
	{
		if(Purchase_Confirm == bySub)
		{
			U_STATE_LOG( BM::LOG_LV3, _T("Simulate Commit ReqKey:")<<dwReqKey);
		}
		
		if(Purchase_Cancel == bySub)
		{
			U_STATE_LOG( BM::LOG_LV3, _T("Simulate RollBack ReqKey:")<<dwReqKey);
		}
		BM::Stream kDGPacket;
		NP_Push(kDGPacket, byPri);
		NP_Push(kDGPacket, bySub);
		NP_Push(kDGPacket, dwReqKey);
		
//		pkSession->VSend(kDGPacket);
		OnRecvFromNexonCash(NULL, &kDGPacket);
	}
	return true;
}