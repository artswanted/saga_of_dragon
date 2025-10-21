#include "stdafx.h"
#include "PgReloadableDataMgr.h"

void PgReloadableDataMgr::DataCompress(BM::Stream const &kOrgPacket, BM::Stream &kTgtPacket)//압축 시도 해보고 실패 하면. 그냥 냅둠
{
	BM::Stream kCompPacket;
	
	size_t const packet_org_size = kOrgPacket.Data().size();
	kTgtPacket.Clear();
	if(BM::Compress(kOrgPacket.Data(), kCompPacket.Data()))
	{	
		kCompPacket.PosAdjust();
		bool bIsCompress = true;
		kTgtPacket.Push(bIsCompress);
		kTgtPacket.Push(packet_org_size);
		kTgtPacket.Push(kCompPacket);
//		BM::Stream kTemp2;
//		kTemp2.Resize(m_kCashPacketOrgSize);
//		BM::UnCompress(kTemp2.Data(), m_kPacket_CashShop.Data());
		kTgtPacket.PosAdjust();
	}
	else
	{	
		bool bIsCompress = false;
		kTgtPacket.Push(bIsCompress);
		kTgtPacket.Push(packet_org_size);
		kTgtPacket.Push(kOrgPacket);

		kTgtPacket.PosAdjust();
	}

	/*	size_t const cash_packet_org_size = kLinearPacket.Data().size();
	m_kPacket_CashShop.Clear();
	if(BM::Compress(kLinearPacket.Data(), kCompPacket.Data()))
	{	
		kCompPacket.PosAdjust();
		bool bIsCompress = true;
		m_kPacket_CashShop.Push(bIsCompress);
		m_kPacket_CashShop.Push(cash_packet_org_size);
		m_kPacket_CashShop.Push(kCompPacket);
//		BM::Stream kTemp2;
//		kTemp2.Resize(m_kCashPacketOrgSize);
//		BM::UnCompress(kTemp2.Data(), m_kPacket_CashShop.Data());
		m_kPacket_CashShop.PosAdjust();
	}
	else
	{	
		bool bIsCompress = false;
		m_kPacket_CashShop.Push(bIsCompress);
		m_kPacket_CashShop.Push(cash_packet_org_size);
		m_kPacket_CashShop.Push(kLinearPacket);

		m_kPacket_CashShop.PosAdjust();
	}
*/
}

void PgReloadableDataMgr::SyncProcess(BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	BM::GUID kStoreValueKey;
	CONT_DEF_CASH_SHOP kContShopMain;//.clear();
	CONT_DEF_CASH_SHOP_ARTICLE kContShopArticle;//.clear();
	CONT_CASH_SHOP_ITEM_LIMITSELL kContShopLimitSell;//.clear();
	BM::Stream kStorePacket;

///////////////////////////////////////////////////////
//Read Part
{
	pkPacket->Pop(kStoreValueKey);

	PU::TLoadTable_MM(*pkPacket, kContShopMain);
	PU::TLoadTable_MM(*pkPacket, kContShopArticle);
	PU::TLoadTable_MM(*pkPacket, kContShopLimitSell);

	//	pkPacket->Pop(kStorePacket.Data());
//	kStorePacket.PosAdjust();
//	게임내 샵 정보는 보낸걸 그대로 쓰자.-> 바로 압축해서.
}
///////////////////////////////////////////////////////
//Write Part
{	//캐시샵 Write
	BM::Stream kLinearPacket;
	PU::TWriteTable_MM(kLinearPacket, kContShopMain);
	PU::TWriteTable_MM(kLinearPacket, kContShopArticle);
	PU::TWriteTable_MM(kLinearPacket, kContShopLimitSell);

	m_kStoreValueKey = kStoreValueKey;

	INFO_LOG(BM::LOG_LV7, _T("Set StoreKey ReloadData::")<< m_kStoreValueKey);
	DataCompress(kLinearPacket, m_kPacket_CashShop);
}
}

void PgReloadableDataMgr::ReqSyncDataFromUser(BM::GUID const &kUserStoreValueKey, CEL::CSession_Base *pkSession)
{// E_RELOAD_FLAG 사용
	if(m_kStoreValueKey != kUserStoreValueKey)
	{
		BM::CAutoMutex kLock(m_kMutex);

		BM::Stream kPacket(PT_S_C_NFY_REFRESH_DATA);
	//	if(RF_CASH_SHOP & dwFlag)
		{
//			kPacket.Push(dwFlag);
			kPacket.Push(m_kStoreValueKey);
			kPacket.Push(m_kPacket_CashShop);
		}

		pkSession->VSend(kPacket);
	}
}
