#include "stdafx.h"
#include "variant/constant.h"
#include "Variant/PgStringUtil.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgMission.h"
#include "Variant/def.h"
#include "Variant/MonsterDef.h"
#include "Variant/MonsterDefMgr.h"
#include "constant.h"
#include "PgQuest.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "Global.h"
#include "PgTask_MapServer.h"
#include "PgMonKillCountReward.h"

typedef BM::TObjectPool< PgQuestInfo > PoolQuest;
PoolQuest g_kQuestPool(100, 100);

namespace PgGroundQuestUtil
{
	bool IsCanShowQusetDialog(EQuestShowDialogType const eShowType, int const iQuestID, int const iDialogID)
	{
		PgQuestInfo const* pkQuestInfo = NULL;
		if( !iQuestID )
		{
			return false;
		}
		if( !g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
		{
			return false;
		}

		if( !pkQuestInfo )
		{
			return false;
		}

		switch( pkQuestInfo->Type() )
		{
		case QT_RandomTactics:
		case QT_Random:
		case QT_Day:
		case QT_Wanted:
			{
				if( QSDT_ErrorDialog == eShowType )
				{
					return false; // ���ϸ� ����Ʈ�� �� Ư��(��� ���� �޽�����, �޽��� �ڽ��� ó�� �ȴ�.
				}
			}break;
		default:
			{
				// do nothing
			}break;
		}

		return pkQuestInfo->FindDialog(iDialogID);
	}

	bool IsHaveCoupleQuest(PgMyQuest const* pkMyQuest)
	{
		if( !pkMyQuest )
		{
			return false;
		}

		ContUserQuestState kVec;
		if( !pkMyQuest->GetQuestList(kVec) )
		{
			return false;
		}

		ContUserQuestState::const_iterator iter = kVec.begin();
		while( kVec.end() != iter )
		{
			PgQuestInfo const* pkQuestInfo = NULL;
			if( g_kQuestMan.GetQuest((*iter).iQuestID, pkQuestInfo) )
			{
				switch( pkQuestInfo->Type() )
				{
				case QT_Couple:
				case QT_SweetHeart:
					{
						return true;
					}break;
				}
			}
			++iter;
		}
		return false;
	}

	bool MakeQuestReward(CONT_PLAYER_MODIFY_ORDER& rkOrder, PgQuestInfo const* pkQuestInfo, PgPlayer* pkPlayer, int const iSelect1, int const iSelect2)
	{
		// PgAction_AnsQuestDialog::DoComplete �Լ��� ���� ������ ó�� �ؾ� �ȴ�
		// 1. ������ ����
		// 2. QS_Finished ���� �߰�
		// 3. ���� ����
		if( !pkQuestInfo
		||	!pkPlayer )
		{
			return false;
		}

		PgAction_QuestDeleteItem kQuestAction(pkQuestInfo->m_kCompleteDeleteItem, rkOrder); // ���� ������ ����
		kQuestAction.DoAction(pkPlayer, NULL);

		{
			SPMOD_AddIngQuest kAddQuestData(pkQuestInfo->ID(), QS_Finished); // �Ϸ� ó��
			rkOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), kAddQuestData) );
		}

		PgAction_GiveQuestReward kRewardAction(pkQuestInfo->ID(), iSelect1, iSelect2, rkOrder); // ����
		if( !kRewardAction.DoAction(pkPlayer, NULL) )
		{
			rkOrder.clear(); // ��� ������ �ʱ�ȭ
			return false;
		}
		return true;
	}
};

//
tagNpcQuestInfoKey::tagNpcQuestInfoKey()
	: eType(QT_None), iQuestID(0)
{
}
tagNpcQuestInfoKey::tagNpcQuestInfoKey(EQuestType const& reType, int const& riQuestID)
	: eType(reType), iQuestID(riQuestID)
{
}
tagNpcQuestInfoKey::tagNpcQuestInfoKey(tagNpcQuestInfoKey const& rhs)
	: eType(rhs.eType), iQuestID(rhs.iQuestID)
{
}
tagNpcQuestInfoKey::~tagNpcQuestInfoKey()
{
}
bool tagNpcQuestInfoKey::operator <(tagNpcQuestInfoKey const& rhs) const
{
	return iQuestID < rhs.iQuestID;
}

////////////////////////
PgNPCQuest::PgNPCQuest(BM::GUID const & rkNpcGuid)
	:m_kNpcGuid(rkNpcGuid)
{
}
PgNPCQuest::~PgNPCQuest()
{
}
int PgNPCQuest::GetEventNo(int const iQuestID, const EQuestState eState) const
{
	ContQuestNpcPtr::const_iterator iter, end;
	switch(eState)
	{
	case QS_Begin:
		{
			iter = m_kClient.find( SNpcQuestInfoKey(QT_None,iQuestID) );
			end = m_kClient.end();
		}break;
	case QS_Ing:
		{
			iter = m_kAgent.find( SNpcQuestInfoKey(QT_None,iQuestID) );
			end = m_kAgent.end();
		}break;
	case QS_Failed:
	case QS_End:
		{
			iter = m_kPayer.find( SNpcQuestInfoKey(QT_None,iQuestID) );
			end = m_kPayer.end();
		}break;
	default:
		{
			return 0;
		}break;
	}
	if( end != iter )
	{
		return (*iter).second->iEventNo;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}
void PgNPCQuest::Build(PgQuestInfo const* pkQuestInfo, const ContQuestNpc &rkClient, const ContQuestNpc &rkAgent, const ContQuestNpc &rkPayer)
{
	if( !pkQuestInfo )
	{
		return;
	}

	PgNPCQuest::Build(pkQuestInfo, rkClient, m_kClient, NpcGuid());
	PgNPCQuest::Build(pkQuestInfo, rkAgent, m_kAgent, NpcGuid());
	PgNPCQuest::Build(pkQuestInfo, rkPayer, m_kPayer, NpcGuid());
}
void PgNPCQuest::Build(PgQuestInfo const* pkQuestInfo, const ContQuestNpc &rkSrcCont, ContQuestNpcPtr& rkTrgCont, BM::GUID const & rkNpcGuid)
{
	if( !pkQuestInfo )
	{
		return;
	}

	ContQuestNpc::const_iterator iter = rkSrcCont.begin();
	while(rkSrcCont.end() != iter)
	{
		if( (*iter).kNpcGuid == rkNpcGuid )
		{
			auto kRet = rkTrgCont.insert(std::make_pair(SNpcQuestInfoKey(pkQuestInfo->Type(),pkQuestInfo->ID()), &(*iter)));
			if( !kRet.second )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data!"));
				return;
			}
		}
		++iter;
	}
}


//---------------------------------------
// PgQuestManager 
//---------------------------------------
PgQuestManager::PgQuestManager()
{
}

PgQuestManager::~PgQuestManager()
{
	Destroy();
}

bool PgQuestManager::Create(CONT_DEF_QUEST_REWARD const* pkDefQuestReward)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( !pkDefQuestReward )
	{
		return false;
	}

	//Parse Quest
	bool bRet = true;
	CONT_DEF_QUEST_REWARD::const_iterator def_iter = pkDefQuestReward->begin();
	while( pkDefQuestReward->end() != def_iter )
	{
		CONT_DEF_QUEST_REWARD::mapped_type const& rkDefInfo = (*def_iter).second;
		if( PgQuestInfoUtil::IsLoadXmlType(rkDefInfo.iDBQuestType) )
		{
			std::wstring const kXmlPath = std::wstring(_T("XML/")) + rkDefInfo.kXmlPath;
			bRet = Add(kXmlPath, rkDefInfo.iQuestID) && bRet;
		}
		++def_iter;
	}
	return bRet;
}

void PgQuestManager::Reload()
{
#ifndef _MDo_
	BM::CAutoMutex kLock(m_kMutex);

	bool bRet = true;
	PgQuestManager kTemp;

	CONT_DEF_QUEST_REWARD const *pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);

	INFO_LOG(BM::LOG_LV1, __FL__<<L"2. Reload Quest XML");
	if( !pkQuestReward )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't load [CONT_DEF_QUEST_REWARD]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkQuestReward is NULL"));
		bRet = false;
	}

	INFO_LOG(BM::LOG_LV1, __FL__<<L"3. Rebuild Quest");
	if( bRet )
	{
		bRet = kTemp.Build(pkQuestReward);
	}

	if( bRet )
	{
		// Success
		INFO_LOG(BM::LOG_LV1, __FL__<<L"4. Rebuild Quest Monster Information");

		Swap(kTemp); // Apply

		SEventMessage kEventMsg;
		kEventMsg.PriType(PMET_GROUND_MGR);
		kEventMsg.SecType(PT_A_G_NFY_RELOAD_NPCQUEST);
		g_kTask.PutMsg(kEventMsg);
	}
	else
	{
		 INFO_LOG(BM::LOG_LV1, __FL__<<L"4. Cancel reload QUEST XML");
	}
#endif
}

bool PgQuestManager::Verify() const
{
	PgQuestInfoVerifyUtil::VerifyXmlToDef(m_kQuestMap);

	QuestContainer::const_iterator iter = m_kQuestMap.begin();
	while( m_kQuestMap.end() != iter )
	{
		PgQuestInfoVerifyUtil::Verify( (*iter).second, m_kQuestMap, g_kMonKillCountReward.MonsterKillResetMax() );
		++iter;
	}

	if( !PgQuestInfoVerifyUtil::DisplayResult() )
	{
		return false;
	}

	iter = m_kQuestMap.begin();
	while( m_kQuestMap.end() != iter )
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Quest XML Loading: ") << (*iter).second->XmlFileName());
		++iter;
	}
	return true;
}

void PgQuestManager::Swap(PgQuestManager& rhs)
{
	m_kQuestMap.swap( rhs.m_kQuestMap );
	m_kNpcToQuest.swap( rhs.m_kNpcToQuest );
	m_kQuestGroupMap.swap( rhs.m_kQuestGroupMap );
	m_kItemToQuest.swap( rhs.m_kItemToQuest );
	m_kItemToQuest_Repair.swap( rhs.m_kItemToQuest_Repair );
	m_kItemToQuest_Enchant.swap( rhs.m_kItemToQuest_Enchant );
	m_kItemToQuest_Pet.swap( rhs.m_kItemToQuest_Pet );
	m_kLocationToQuest.swap( rhs.m_kLocationToQuest );
	m_kMonsterQuest.swap( rhs.m_kMonsterQuest );
	m_kGroundQuest.swap( rhs.m_kGroundQuest );
	m_kKillCountQuest.swap( rhs.m_kKillCountQuest );
	m_kContQuestItems.swap( rhs.m_kContQuestItems );
}

void PgQuestManager::Destroy()
{
	BM::CAutoMutex kLock(m_kMutex);

	QuestContainer::iterator quest_iter = m_kQuestMap.begin();
	while(m_kQuestMap.end() != quest_iter)
	{
		QuestContainer::mapped_type pkElement = (*quest_iter).second;
		if( pkElement )
		{
			pkElement->Clear();
			g_kQuestPool.Delete( pkElement );
		}	
		else
		{
			CAUTION_LOG(BM::LOG_LV0, __FL__<<L"const Quest-Info pointer is null");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkElement is NULL"));
		}

		++quest_iter;
	}
	m_kQuestMap.clear();
	m_kNpcToQuest.clear();
	m_kQuestGroupMap.clear();
	m_kItemToQuest.clear();
	m_kItemToQuest_Repair.clear();
	m_kItemToQuest_Enchant.clear();
	m_kItemToQuest_Pet.clear();
	m_kLocationToQuest.clear();
	m_kMonsterQuest.clear();
	m_kGroundQuest.clear();
	m_kKillCountQuest.clear();
	m_kContQuestItems.clear();
}

bool PgQuestManager::Add(std::wstring const& rkXmlPath, int iQuestId)
{
	BM::CAutoMutex kLock(m_kMutex);

	PgQuestInfo* pkQuest = g_kQuestPool.New();
	pkQuest->Clear();

	std::string const kXmlPath( MB(rkXmlPath) );

	//char szPath[_MAX_PATH] = {0, };
	//sprintf_s(szPath, _MAX_PATH, "XML/Quest/%s", MB(rkXmlPath));

	TiXmlDocument kDocu(kXmlPath.c_str());
	if( !kDocu.LoadFile() )
	{
		PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError(PgQuestInfoVerifyUtil::ET_ParseError, kXmlPath, 0, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong XML format file or File is not Exist")) ));
		g_kQuestPool.Delete(pkQuest);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkQuest->XmlFileName( kXmlPath );
	TiXmlElement *pkChild = kDocu.FirstChildElement("QUEST");
	bool bRet = pkQuest->ParseXml(pkChild, iQuestId);
	if(!bRet)
	{
		PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError(PgQuestInfoVerifyUtil::ET_ParseError, kXmlPath, 0, __FUNCTIONW__, __LINE__, BM::vstring(_T("failed parsing QUEST XML file")) ));

		g_kQuestPool.Delete(pkQuest);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	auto ret = m_kQuestMap.insert(std::make_pair(pkQuest->ID(), pkQuest));
	if( !ret.second )
	{
		PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError(pkQuest, PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Quest Add failure duplicated QuestID[")) << pkQuest->ID() << _T("]") ));

		g_kQuestPool.Delete(pkQuest);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BuildDepend(pkQuest);

	pkQuest->Build();

	// ��ġ �̵����� ���� �׻� pkQuest->Build(); �ڿ� �־�� ��
	BuildQuestItem(pkQuest);

	return true;//����
}

void PgQuestManager::BuildQuestItem(PgQuestInfo const * pkQuest)
{
	for(CONT_DEL_QUEST_ITEM::const_iterator iter = pkQuest->m_kCompleteDeleteItem.begin();iter != pkQuest->m_kCompleteDeleteItem.end(); ++iter)
	{
		m_kContQuestItems[(*iter).iItemNo].insert(pkQuest->ID());
	}
}

bool PgQuestManager::GetItemQuests(int const iItemNo, CONT_QUEST_SET & kCont) const
{
	CONT_QUEST_ITEM::const_iterator iter = m_kContQuestItems.find(iItemNo);
	if(iter == m_kContQuestItems.end())
	{
		return false;
	}

	kCont = (*iter).second;
	return true;
}

bool PgQuestManager::BuildGroupQuest()
{
	bool bReturn = true;
	m_kQuestGroupMap.clear();

	QuestContainer::const_iterator loop_iter = m_kQuestMap.begin();
	while( m_kQuestMap.end() != loop_iter )
	{
		QuestContainer::mapped_type const pkQuestInfo = (*loop_iter).second;
		if( pkQuestInfo
		&&	pkQuestInfo->GroupNo() )
		{
			QuestGroupContainer::mapped_type kVec;
			QuestGroupContainer::iterator find_iter = m_kQuestGroupMap.end();
			auto kRet = m_kQuestGroupMap.insert(std::make_pair(pkQuestInfo->GroupNo(), kVec));
			if( !kRet.second )
			{
				find_iter = kRet.first;
			}

			if( m_kQuestGroupMap.end() != find_iter )
			{
				QuestGroupContainer::mapped_type &rkVec = (*find_iter).second;
				QuestGroupContainer::mapped_type::const_iterator find_iter = std::find(rkVec.begin(), rkVec.end(), pkQuestInfo->ID());
				if( rkVec.end() != find_iter )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplication QuestID["<<pkQuestInfo->ID()<<L"] in Group["<<pkQuestInfo->GroupNo()<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					bReturn = false;
				}
				std::back_inserter(rkVec) = pkQuestInfo->ID();
			}
		}

		++loop_iter;
	}
	return bReturn;
}

void PgQuestManager::BuildDepend(PgQuestInfo const* pkQuest)
{
	{//Item Event's
		ContQuestDependItem const& rkDepend_Item = pkQuest->m_kDepend_Item;
		ContQuestDependItem::const_iterator item_iter = rkDepend_Item.begin();
		while(rkDepend_Item.end() != item_iter)
		{
			ContQuestDependItem::mapped_type const& rkElement = (*item_iter).second;
			if( (QET_ITEM_ChangeCount == rkElement.iType)
				|| (QET_ITEM_NotChangeCount == rkElement.iType) )
			{
				AddItemDependence(rkElement.iItemNo, pkQuest->ID(), m_kItemToQuest);
			}
			else if( QET_ITEM_Durability_Check == rkElement.iType )
			{
				AddItemDependence(rkElement.iItemNo, pkQuest->ID(), m_kItemToQuest_Repair);
			}
			else if( QET_ITEM_Plus_Check == rkElement.iType )
			{
				AddItemDependence(rkElement.iItemNo, pkQuest->ID(), m_kItemToQuest_Enchant);
			}
			else if( QET_ITEM_Check_Pet == rkElement.iType )
			{
				AddItemDependence(rkElement.iItemNo, pkQuest->ID(), m_kItemToQuest_Pet);
			}

			++item_iter;
		}
	}
	{//Location
		const ContQuestLocation& rkDepend_Location = pkQuest->m_kDepend_Location;
		ContQuestLocation::const_iterator location_iter = rkDepend_Location.begin();
		while(rkDepend_Location.end() != location_iter)
		{
			const ContQuestLocation::value_type& rkElement = (*location_iter);

			SQuestTriggerInfo kInfo(rkElement.iGroundNo, rkElement.iLocationNo);//GroundNo, TriggerNo
			ContLocationQuest::iterator kNew_iter = m_kLocationToQuest.find(kInfo);
			if( m_kLocationToQuest.end() == kNew_iter )
			{
				auto ibRet = m_kLocationToQuest.insert(std::make_pair(kInfo, pkQuest->ID()));
				if( !ibRet.second )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Can't insert item Quest["<<pkQuest->ID()<<L"] GroundNo["<<rkElement.iGroundNo<<L"] TriggerNo["<<rkElement.iLocationNo<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data!"));
				}
			}

			++location_iter;
		}
	}
	{//KillCount
		if( 0 != pkQuest->m_kDepend_KillCount.iKillCount )
		{
			std::back_inserter(m_kKillCountQuest) = pkQuest->ID();
		}
	}
}

bool PgQuestManager::GetQuest(int const iQuestID, PgQuestInfo const*& pkOut) const
{
	if( !iQuestID )
	{
		INFO_LOG(BM::LOG_LV1, __FL__<<L"Can't QuestID is 0");
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't QuestID is 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	QuestContainer::const_iterator iter = m_kQuestMap.find(iQuestID);
	if(m_kQuestMap.end() != iter)
	{
		pkOut = (*iter).second;
		return true;
	}
	CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest Info["<<iQuestID<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgQuestManager::GetGroupQuestID(int const iGroupNo, ContQuestID const*& pkOut) const
{
	if( !iGroupNo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContQuestID const *pkVec = NULL;
	QuestGroupContainer::const_iterator kIter = m_kQuestGroupMap.find(iGroupNo);
	if(m_kQuestGroupMap.end() != kIter)
	{
		pkVec = &kIter->second;
	}
	else
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest GroupNo: "<<iGroupNo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Failed!"));
	}

	pkOut = pkVec;
	return true;
}

bool PgQuestManager::GetItemToQuest(int const iItemNo, ContQuestID const*& pkOut) const
{
	return GetItemToQuestDependence(iItemNo, pkOut, m_kItemToQuest);
}

bool PgQuestManager::GetItemToQuestRepair(int const iItemNo, ContQuestID const*& pkOut) const
{
	return GetItemToQuestDependence(iItemNo, pkOut, m_kItemToQuest_Repair);
}

bool PgQuestManager::GetItemToQuestEnchant(int const iItemNo, ContQuestID const*& pkOut) const
{
	return GetItemToQuestDependence(iItemNo, pkOut, m_kItemToQuest_Enchant);
}

bool PgQuestManager::GetItemToQuestPet(int const iItemNo, ContQuestID const*& pkOut) const
{
	return GetItemToQuestDependence(iItemNo, pkOut, m_kItemToQuest_Pet);
}

bool PgQuestManager::GetTriggerToQuest(SQuestTriggerInfo const &rkTriggerInfo, int& rsQuestID) const
{
	ContLocationQuest::const_iterator kIter = m_kLocationToQuest.find(rkTriggerInfo);//Location Trigger -> Quest ID
	if(m_kLocationToQuest.end() != kIter)
	{
		rsQuestID = kIter->second;
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Invalid Quest TriggerInfo [GroundNo: "<<rkTriggerInfo.iGroundNo<<L", TriggerNo: "<<rkTriggerInfo.iTriggerNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Failed!"));
	}
	return (m_kLocationToQuest.end() != kIter);
}

bool PgQuestManager::Build(CONT_DEF_QUEST_REWARD const *pkQuestReward)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool bReturn = true;
	if( !pkQuestReward )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEF_QUEST_REWARD pointer is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkQuestReward is NULL"));
		return false;
	}

	PgQuestManager kTemp;
	if( bReturn )
	{
		bReturn = bReturn && kTemp.Create(pkQuestReward);
	}

	//if( bReturn )
	{
		bReturn = kTemp.Verify();
	}

	if( bReturn )
	{
		bReturn = kTemp.BuildGroupQuest(); // �׷� ����Ʈ
	}
	if( bReturn )
	{
		kTemp.BuildMapQuest();
	}

	if( bReturn )
	{
		Swap(kTemp);
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return bReturn;
}

void PgQuestManager::BuildNpc(BM::GUID const & rkNPCGuid)
{
	if( BM::GUID::IsNull(rkNPCGuid) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"pointer is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("rkNPCGuid is NullData!"));
		return ;
	}

	auto kRet = m_kNpcToQuest.insert(std::make_pair(rkNPCGuid, PgNPCQuest(rkNPCGuid)));

	PgNPCQuest& rkNpcQuest = (*kRet.first).second;
	QuestContainer::const_iterator quest_iter = m_kQuestMap.begin();
	while(m_kQuestMap.end() != quest_iter)
	{
		PgQuestInfo const* pkQuest = (*quest_iter).second;
		if( pkQuest )
		{
			rkNpcQuest.Build(pkQuest, pkQuest->m_kNpc_Client, pkQuest->m_kNpc_Agent, pkQuest->m_kNpc_Payer);
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"QuestInfo Const Pointer is NULL");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkQuest is NULL"));
		}
		++quest_iter;
	}
}

size_t PgQuestManager::CheckQuest(PgPlayer* pkPC, BM::GUID const & rkNpcGuid, ContSimpleQuest& rkOutCont) const
{
	rkOutCont.clear();
	if( !pkPC )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"pointer is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	ContNpcQuest::const_iterator npc_iter = m_kNpcToQuest.find(rkNpcGuid);
	if( m_kNpcToQuest.end() == npc_iter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	ContNpcQuest::mapped_type const& rkNpcQuest = (*npc_iter).second;
	{//�������� ����Ʈ
		const ContQuestNpcPtr& rkQuest = rkNpcQuest.m_kAgent;
		ContQuestNpcPtr::const_iterator quest_iter = rkQuest.begin();
		while(rkQuest.end() != quest_iter)
		{
			int const iQuestID = (*quest_iter).first.iQuestID;
			const ContQuestNpcPtr::mapped_type pkNpcQuest = (*quest_iter).second;
			SUserQuestState const *pkState = pkMyQuest->Get(iQuestID);
			if( pkState )
			{
				if( QS_End != pkState->byQuestState )
				{
					rkOutCont.push_back( SQuestSimple(iQuestID, static_cast< BYTE >(pkNpcQuest->eMark), QDE_QuestLimitMaxLevel) );
				}
			}
			++quest_iter;
		}
	}
	{//���ο� ����Ʈ ���� �������� Ȯ���ϱ�
		int const iPlayerLevel = pkPC->GetAbil(AT_LEVEL);
		bool bAddRandomQuest = false; // ���� ����Ʈ ����ǥ ���� ó��
		bool bAddTacticsRandomQuest = false; // �뺴 ���� ����Ʈ ����ǥ ���� ó��
		bool bAddWantedQuest = false;	// ���� ���� ����Ʈ ����ǥ ���� ó��
		const ContQuestNpcPtr& rkQuest = rkNpcQuest.m_kClient;
		ContQuestNpcPtr::const_iterator quest_iter = rkQuest.begin();
		while(rkQuest.end() != quest_iter)
		{
			EQuestType const eQuestType = (*quest_iter).first.eType;
			int const iQuestID = (*quest_iter).first.iQuestID;

			// ����Ʈ �⺻ üũ
			PgCheckQuestBegin kBegin(iQuestID);
			bool const bCanBegin = kBegin.DoAction(pkPC, NULL);
			if( bCanBegin )
			{
				bool bAddInfo = true;
				switch( eQuestType )
				{
				case QT_RandomTactics:
				case QT_Random:
				case QT_Wanted:
					{
						// ���� ����Ʈ ���� ��ũ�� ������ �ʴ´�
					}break;
				default:
					{
						rkOutCont.push_back( SQuestSimple(iQuestID, QS_Begin, kBegin.MaxLevelLimit()) );
					}break;
				}
			}
			else
			{
				if( QBL_Level_One == kBegin.Limit() )
				{
					rkOutCont.push_back( SQuestSimple(iQuestID, QS_Begin_NYet, kBegin.MaxLevelLimit()) );
				}
			}

			// ���� ����Ʈ ���� üũ
			if( false == bAddRandomQuest
			&&	false == pkMyQuest->BuildedRandomQuest()
			&&	false == bCanBegin
			&&	QT_Random == eQuestType
			&&	RandomQuest::iRandomQuestMinLimitLevel <= iPlayerLevel )
			{
				rkOutCont.push_back( SQuestSimple(iQuestID, QS_Begin, kBegin.MaxLevelLimit()) );
				bAddRandomQuest = true;
			}

			// �뺴 ����Ʈ ���� üũ
			if( false == bAddTacticsRandomQuest
			&&	false == pkMyQuest->BuildedTacticsQuest()
			&&	false == bCanBegin
			&&	QT_RandomTactics == eQuestType
			&&	BM::GUID::IsNotNull(pkPC->GuildGuid())
			&&	RandomQuest::iRandomTacticsQuestMinLimitLevel <= iPlayerLevel )
			{
				rkOutCont.push_back( SQuestSimple(iQuestID, QS_Begin, kBegin.MaxLevelLimit()) );
				bAddTacticsRandomQuest = true;
			}
			
			// ������� ����Ʈ ���� üũ
			if( QT_Wanted == eQuestType )
			{
				PgQuestInfo const * pkOut;
				if( g_kQuestMan.GetQuest( iQuestID, pkOut) )
				{
					EQuestState eQuestState = QS_None;
					EQuestBeginLimit const eRet = PgQuestInfoUtil::CheckBeginQuest( const_cast<PgPlayer const *>(pkPC), iQuestID, pkOut );

					if( !pkMyQuest->BuildedWantedQuest() )
					{// ���������� �÷��̾��� ������ ���尡 ������ ����Ʈ�� �����ϸ� ����ǥ �ٿ���
						if( pkMyQuest->ExistCanBuildWantedQuest() )
						{
							eQuestState = QS_Begin;
						}
					}
					else
					{
						if( QBL_None == eRet )
						{
							eQuestState = QS_Begin;
						}
					}

					if( false == bAddWantedQuest 
					&&	QS_Begin == eQuestState )
					{
						rkOutCont.push_back( SQuestSimple( iQuestID, QS_Begin, kBegin.MaxLevelLimit() ) );
						bAddWantedQuest = true;
					}
				}
			}

			++quest_iter;
		}
	}
	{//����Ʈ ���� �������� Ȯ���ϱ�
		const ContQuestNpcPtr& rkQuest = rkNpcQuest.m_kPayer;
		ContQuestNpcPtr::const_iterator quest_iter = rkQuest.begin();
		while(rkQuest.end() != quest_iter)
		{
			int const iQuestID = (*quest_iter).first.iQuestID;
			const ContQuestNpcPtr::mapped_type pkNpcQuest = (*quest_iter).second;
			SUserQuestState const *pkQuest = pkMyQuest->Get(iQuestID);
			if( pkQuest
			&&	pkQuest->byQuestState == QS_End )
			{
				rkOutCont.push_back( SQuestSimple(iQuestID, static_cast< BYTE >(pkNpcQuest->eMark), QDE_QuestLimitMaxLevel) );
			}
			++quest_iter;
		}
	}

	return rkOutCont.size();
}

int PgQuestManager::GetNPCEventNo(BM::GUID const &rkNPCGuid, int const iQuestID, const EQuestState eState) const
{
	ContNpcQuest::const_iterator iter = m_kNpcToQuest.find(rkNPCGuid);
	if( m_kNpcToQuest.end() == iter )
	{//���� ����/�Ϸ� ����� ���鼭 ���ʿ��� �αװ� ���Ƽ� �ּ�ó����. 2011. 07. 04 ������
		//CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Depend NPC GUID["<<rkNPCGuid<<L"] QuestID["<<iQuestID<<L"] QuestState["<<eState<<L"]");
		//LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	ContNpcQuest::mapped_type const& rkNPCQuest = (*iter).second;
	int const iEventNo = rkNPCQuest.GetEventNo(iQuestID, eState);
	return iEventNo;
}

bool PgQuestManager::GetMonsterQuest(int const iMonsterClassNo, ContQuestID const*& pkOut)const
{
	ContMonsterQuest::const_iterator find_iter = m_kMonsterQuest.find(iMonsterClassNo);
	if( m_kMonsterQuest.end() == find_iter )
	{
		pkOut = NULL;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	pkOut = &(*find_iter).second;
	return true;
}

bool PgQuestManager::GetGroundQuest(int const iGroundNo, ContQuestID const*& pkOut)const
{
	ContGroundQuest::const_iterator find_iter = m_kGroundQuest.find(iGroundNo);
	if( m_kGroundQuest.end() == find_iter )
	{
		pkOut = NULL;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	pkOut = &(*find_iter).second;
	return true;
}

ContQuestID const& PgQuestManager::GetKillCountQuest() const
{
	return m_kKillCountQuest;
}

void PgQuestManager::BuildMapQuest()
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kMonsterQuest.clear();
	m_kGroundQuest.clear();

	QuestContainer::const_iterator quest_iter = m_kQuestMap.begin();
	while( m_kQuestMap.end() != quest_iter )
	{
		PgQuestInfo const* const pkQuest = (*quest_iter).second;
		int const iQuestID = pkQuest->ID();

		{
			ContQuestMonster const& rkDepend_Monster = pkQuest->m_kDepend_Monster;
			ContQuestMonster::const_iterator monster_iter = rkDepend_Monster.begin();
			while(rkDepend_Monster.end() != monster_iter)
			{
				SQuestMonster const& rkQuestMonster = (*monster_iter).second;

				ContMonsterQuest::iterator find_iter = m_kMonsterQuest.find(rkQuestMonster.iClassNo);
				if( m_kMonsterQuest.end() == find_iter )
				{
					ContQuestID kTempVec;
					std::back_inserter(kTempVec) = iQuestID;
					m_kMonsterQuest.insert( std::make_pair(rkQuestMonster.iClassNo, kTempVec) );
				}
				else
				{
					ContQuestID &rkVec = (*find_iter).second;
					if( rkVec.end() == std::find(rkVec.begin(), rkVec.end(), iQuestID) )
					{
						std::back_inserter(rkVec) = iQuestID;
					}
				}
				
				++monster_iter;
			}
		}
		{
			ContQuestGround const& rkDepend_Ground = pkQuest->m_kDepend_Ground;
			ContQuestGround::const_iterator ground_iter = rkDepend_Ground.begin();
			while(rkDepend_Ground.end() != ground_iter)
			{
				SQuestGround const& rkQuestGround = (*ground_iter).second;

				ContGroundQuest::iterator find_iter = m_kGroundQuest.find(rkQuestGround.iGroundNo);
				if( m_kGroundQuest.end() == find_iter )
				{
					ContQuestID kTempVec;
					std::back_inserter(kTempVec) = iQuestID;
					m_kGroundQuest.insert( std::make_pair(rkQuestGround.iGroundNo, kTempVec) );
				}
				else
				{
					ContQuestID &rkVec = (*find_iter).second;
					if( rkVec.end() == std::find(rkVec.begin(), rkVec.end(), iQuestID) )
					{
						std::back_inserter(rkVec) = iQuestID;
					}
				}
				
				++ground_iter;
			}
		}
		++quest_iter;
	}
}