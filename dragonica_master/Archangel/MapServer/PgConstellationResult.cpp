
#include "stdafx.h"
#include "Variant/Unit.h"
#include "Variant/PgPlayer.h"
#include "PgGroundMgr.h"
#include "PgIndun.h"
#include "PgConstellationResult.h"
#include "PgMissionGround.h"

const size_t PgConstellationResult::m_iInitEnteringResultTime = 14;

void PgConstellationResult::OnInit(PgIndun* pkGround)
{
	BM::CAutoMutex Lock(m_kMutex);
	if(!m_kContRewardInfo.empty())
	{
		CONT_REWARD_INFO kContNewRewardInfo;
		m_kContRewardInfo.swap(kContNewRewardInfo);
	}

	State(ER_NONE, pkGround);
	m_iEnteringResultTime = m_iInitEnteringResultTime;
}

void PgConstellationResult::OnSendMapLoadComplete(PgPlayer *pkUser)
{
	if(pkUser == NULL) { return; }

	BM::CAutoMutex Lock(m_kMutex);
	m_kContRewardInfo.insert(std::make_pair(pkUser, SRewardItemInfo()));
}

void PgConstellationResult::OnReleaseUnit(CUnit* pkUnit)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(pkPlayer == NULL) { return; }

	BM::CAutoMutex Lock(m_kMutex);
	CONT_REWARD_INFO::iterator iterPlayer = m_kContRewardInfo.find(pkPlayer);
	if(iterPlayer != m_kContRewardInfo.end())
	{
		m_kContRewardInfo.erase(iterPlayer);
	}
}

int PgConstellationResult::GetCurChoiceRewardIndex(PgPlayer* pkReqPlayer, SRewardItemInfo const* pkRewardItemInfo) const
{
	BM::CAutoMutex Lock(m_kMutex);
	if(pkRewardItemInfo == NULL)
	{
		CONT_REWARD_INFO::const_iterator iterRewardInfo = m_kContRewardInfo.find(pkReqPlayer);
		if(iterRewardInfo == m_kContRewardInfo.end()) { return -1; }
		pkRewardItemInfo = &iterRewardInfo->second;
	}


	size_t iRewardCount = 0;
	for(size_t i = 0; i < MAX_REWARD_CHOICE + 1; i++)
	{
		iRewardCount = i;
		if(i >= MAX_REWARD_CHOICE || pkRewardItemInfo->iAddChoiceRewardItemNo[i] == 0)
		{
			break;
		}
	}

	return iRewardCount;
}


void PgConstellationResult::SendFirstReward(PgIndun* pkGround)
{
	if(pkGround == NULL) { return; }

	BM::CAutoMutex Lock(m_kMutex);
	BM::Stream kPacket(PT_M_C_NFY_CONSTELLATION_RESULT); //기본 지급 정보
	kPacket.Push(m_kContRewardInfo.size());

	CONT_REWARD_INFO::const_iterator iterReward = m_kContRewardInfo.begin();
	for(; iterReward != m_kContRewardInfo.end(); ++iterReward)
	{
		PgPlayer const* pkPlayer = iterReward->first;
		SRewardItemInfo const* pkRewardInfo = &iterReward->second;

		//kPacket.Push(pkPlayer->Name());
		//kPacket.Push(pkRewardInfo->iRandRewardItemNo); //랜덤 지급 보상 아이템 번호
		//kPacket.Push(pkRewardInfo->iRewardItemNo); //기본 지급 보상 아이템 번호
		//kPacket.Push(pkRewardInfo->iAddChoiceRewardBagNo); //백그룹

		kPacket.Push(pkPlayer->Name());
		kPacket.Push(pkRewardInfo->iRewardItemNo); //기본 지급 보상 아이템 번호
		kPacket.Push(pkRewardInfo->iAddChoiceRewardBagNo); //백그룹
		kPacket.Push(pkRewardInfo->iAddChoiceRewardItemIndex[0]); //첫번째 원소는 랜덤 보상 아이템이다
		kPacket.Push(pkRewardInfo->iAddChoiceRewardItemNo[0]); //첫번째 원소는 랜덤 보상 아이템이다
	}
	pkGround->Broadcast(kPacket);
}

void PgConstellationResult::SendSecondaryReward(PgPlayer* pkReqPlayer, PgIndun* pkGround)
{
	if(pkGround == NULL || pkReqPlayer == NULL || pkReqPlayer->IsDead() == true) { return; }

	size_t iCurChoice = GetCurChoiceRewardIndex(pkReqPlayer);
	if(iCurChoice <= 1 || iCurChoice > MAX_REWARD_CHOICE) { return; } //이미 다 선택했다면

	BM::CAutoMutex Lock(m_kMutex);
	CONT_REWARD_INFO::const_iterator iterReward = m_kContRewardInfo.find(pkReqPlayer);
	if(iterReward == m_kContRewardInfo.end()) { return; }

	BM::Stream kPacket(PT_M_C_NFY_CONSTELLATION_CASH_REWARD); //추가 지급 정보
	kPacket.Push(pkReqPlayer->Name());
	kPacket.Push(iCurChoice); //추가 지급 보상: 몇번째 선택인가?
	kPacket.Push(iterReward->second.iAddChoiceRewardItemIndex[iCurChoice - 1]); //추가 지급 보상: 최종 지급될 아이템백 인덱스 번호
	kPacket.Push(iterReward->second.iAddChoiceRewardItemNo[iCurChoice - 1]); //추가 지급 보상: 최종 지급될 아이템 번호

	pkGround->Broadcast(kPacket);
}

void PgConstellationResult::Recv_PT_C_M_REQ_CONSTELLATION_CASH_REWARD(PgPlayer* pkReqPlayer, PgIndun* pkGround)
{
	if(pkGround == NULL || pkReqPlayer == NULL || pkReqPlayer->IsDead() == true) { return; }
	DWORD const eState = (pkGround->GetAttr() & GATTR_FLAG_MISSION) ? (INDUN_STATE_PLAY | INDUN_STATE_RESULT) : INDUN_STATE_RESULT;	
	if((eState & pkGround->GetState()) == 0) { return; } //현재 보상 단계가 아니라면 보상 요청을 무시
	
	BM::CAutoMutex Lock(m_kMutex);
	if(m_eRewardStep != ER_STEP3) { return; }

	if(true == ProcessSecondaryReward(pkReqPlayer, pkGround))
	{
		SendSecondaryReward(pkReqPlayer, pkGround);
	}
}

void PgConstellationResult::Recv_PT_C_M_REQ_CONSTELLATION_CASH_REWARD_COMPLETE(PgPlayer* pkReqPlayer, PgIndun* pkGround)
{
	if(pkReqPlayer == NULL || pkReqPlayer->IsDead() == true) { return; }
	using namespace Constellation;

	CONT_PLAYER_MODIFY_ORDER kOrder, kOrder2;

	bool bNeedConsumeItem = GetCurChoiceRewardIndex(pkReqPlayer) > 1; //첫번째 랜덤 선택은 공짜

	//Step1: 필요한 캐쉬 아이템 수량 감소
	if(bNeedConsumeItem)
	{
		PgInventory* pkInv = pkReqPlayer->GetInven();
		if(pkInv == NULL) { return; }

		SConstellationKey const& kKey = pkGround->GetConstellationKey();
		CONT_CONSTELLATION_BONUS_ITEM const& kContBonus = g_kGndMgr.m_kGndRscMgr.GetConstellationBonus();
		CONT_CONSTELLATION_BONUS_ITEM::const_iterator iterBonus = kContBonus.find(kKey.WorldGndNo);
		if( kContBonus.end() == iterBonus) { return; }

		//Step2. 캐시 아이템 필요갯수 체크
		int NeedItemNo = 0;
		int NeedItemCount = 0;
		int const NowChoiceCount = GetCurChoiceRewardIndex(pkReqPlayer) - 1;
		if( iterBonus->second.VecItemCount.size() >= NowChoiceCount )
		{
			try
			{
				NeedItemCount = iterBonus->second.VecItemCount.at(NowChoiceCount - 1);
			}
			catch(const std::out_of_range& exception)
			{
				return;
			}
		}
		else
		{// 횟수 초과
			return;
		}

		// Step3. 필요한 갯수만큼 소지하고 있는 캐시 아이템이 있는지 체크
		int HaveItemNo = 0;
		VEC_INT::const_reverse_iterator item_iter = iterBonus->second.VecItemNo.rbegin();
		while( iterBonus->second.VecItemNo.rend() != item_iter )
		{
			NeedItemNo = *item_iter;
			int const HaveItemCount = pkInv->GetInvTotalCount(NeedItemNo);

			if( HaveItemCount >= NeedItemCount )
			{ //캐쉬 아이템 검증
				HaveItemNo = NeedItemNo;
				break;
			}
			++item_iter;
		}

		if( 0 == HaveItemNo )
		{//Msg: 아이템 수량이 부족합니다.
			return;
		}

		kOrder.push_back(SPMO(IMET_ADD_ANY | IMC_DEC_DUR_BY_USE, pkReqPlayer->GetID(), SPMOD_Add_Any(NeedItemNo, -NeedItemCount)));
		
		PgAction_ReqModifyItem kItemModifyAction(CIE_Constellation, pkGround->GroundKey(), kOrder);
		kItemModifyAction.DoAction(pkReqPlayer, pkReqPlayer);
	}

	BM::CAutoMutex Lock(m_kMutex);
	//Step4: 실제 보상 아이템 지급
	CONT_REWARD_INFO::iterator iterRewardInfo = m_kContRewardInfo.find(pkReqPlayer);
	if(iterRewardInfo == m_kContRewardInfo.end()) { return; }
	CONT_REWARD_INFO::mapped_type& kRewardInfo = iterRewardInfo->second;

	int iCurChoice = GetCurChoiceRewardIndex(pkReqPlayer, &kRewardInfo);
	if(iCurChoice <= 0 || iCurChoice > MAX_REWARD_CHOICE) { return; }
	iCurChoice--;

	int iRewardItemNo = kRewardInfo.iAddChoiceRewardItemNo[iCurChoice];
	int iRewardItemCount = kRewardInfo.iAddChoiceRewardItemCount[iCurChoice];

	//보상 아이템 지급 오더
	kOrder2.push_back(SPMO(IMET_ADD_ANY, pkReqPlayer->GetID(), SPMOD_Add_Any(iRewardItemNo, iRewardItemCount)));

	PgAction_ReqModifyItem kItemModifyAction2(CIE_Constellation, pkGround->GroundKey(), kOrder2);
	kItemModifyAction2.DoAction(pkReqPlayer, pkReqPlayer);
}


void PgConstellationResult::SetDelayTime(size_t iSec)
{
	BM::CAutoMutex Lock(m_kMutex);
	m_iEnteringResultTime = iSec;
}

void PgConstellationResult::State(EREWARD_STEP eStep, PgIndun* pkGround)
{
	if(pkGround == NULL)
	{
		INFO_LOG(BM::LOG_LV3, __FL__ << _T("PgConstellationResult: Ground Pointer Is NULL !!!"));
		return;
	}

	BM::CAutoMutex Lock(m_kMutex);
	m_eRewardStep = eStep;

	switch(m_eRewardStep)
	{
	case ER_NONE:
		{
		}break;
	case ER_STAND_BY:
		{ //이 상태에서는 아무일도 하지 않고 대기
		}break;
	case ER_STEP1:
		{
			BM::Stream kPacket(PT_M_C_NFY_CONSTELLATION_RESULT_WAIT);
			pkGround->Broadcast(kPacket); //연출용 패킷
		}break;
	case ER_STEP2:
		{
		}break;
	case ER_STEP3:
		{
		}break;
	}
}

void PgConstellationResult::OnTick1s(PgIndun* pkGround, bool const IsLastStage)
{ //OnTick1s()에서 실행
	if(pkGround == NULL)
	{
		INFO_LOG(BM::LOG_LV3, __FL__ << _T("PgConstellationResult: Ground Pointer Is NULL !!!"));
		return;
	}
	switch(m_eRewardStep)
	{
	case ER_NONE:
		{
			BM::CAutoMutex Lock(m_kMutex);
			if( false == pkGround->IsEventScriptPlayer() )
			{
				if( IsLastStage )
				{
					State(ER_STEP1, pkGround);
				}
				else
				{
					if(m_iEnteringResultTime-- <= 0)
					{
						State(ER_STEP2, pkGround);
					}
				}
			}
		}break;
	case ER_STAND_BY:
		{ //이 상태에서는 아무일도 하지 않고 대기
		}break;
	case ER_STEP1:
		{
			BM::CAutoMutex Lock(m_kMutex);
			if(m_iEnteringResultTime-- <= 0)
			{
				State(ER_STEP2, pkGround);
			}
		}break;
	case ER_STEP2:
		{
			if(ProcessFirstReward(pkGround))
			{
				SendFirstReward(pkGround);
				State(ER_STEP3, pkGround);
			}
			else
			{
				//실패처리...
			}
		}break;
	case ER_STEP3:
		{ //대기상태이며, 시간 제한 없다 (나가기 버튼을 누르면 종료)
		}break;
	}
}


bool PgConstellationResult::ProcessFirstReward(PgIndun* pkGround)
{ //기본	보상 아이템 지급 (전체 인원)
	if(pkGround == NULL)
	{
		return false;
	}
	using namespace Constellation;

	SConstellationKey const& kKey = pkGround->GetConstellationKey();

	CONT_CONSTELLATION const& ContConstellation = g_kGndMgr.m_kGndRscMgr.GetConstellation();
	CONT_CONSTELLATION::const_iterator iter = ContConstellation.find(kKey.WorldGndNo);
	if( ContConstellation.end() == iter ) { return false; }

	CONT_CONSTELLATION_ENTER const& kElem = iter->second;
	CONT_CONSTELLATION_ENTER::const_iterator sub_iter = kElem.find(kKey.Key);
	if( kElem.end() == sub_iter ) { return false; }

	CONT_CONSTELLATION_ENTER::mapped_type const& kValue = sub_iter->second;
	GET_DEF(CItemBagMgr, kItemBagMgr);

	BM::CAutoMutex Lock(m_kMutex);
	CONT_REWARD_INFO::iterator iterRewardInfo = m_kContRewardInfo.begin();
	for(; iterRewardInfo != m_kContRewardInfo.end(); ++iterRewardInfo)
	{
		PgPlayer* pkPlayer = iterRewardInfo->first;
		if(pkPlayer == NULL) { continue; }
		if(true == pkPlayer->IsDead()) { continue; }
		CONT_REWARD_INFO::mapped_type& kRewardInfo = iterRewardInfo->second;
/*
		//step1: 랜덤아이템
		PgAction_PopItemBag kAction(CIE_PetProduceItem, pkGround->GroundKey(), 0, kValue.Reward.BagGroupNo, 0, NULL, 0);
		if(false == kAction.DoAction(pkPlayer, NULL)) { continue; }
		RESULT_ITEM_BAG_LIST_CONT kContItemList;
		kAction.SwapRetItemNo(kContItemList);
		kRewardInfo.iRewardItemNo = kContItemList.front();
*/
		//step1: 필수 보상
		PgBase_Item kItem;
		CONT_ITEM_CREATE_ORDER kOrderList;
		if( SUCCEEDED(CreateSItem(kValue.Reward.ItemNo, kValue.Reward.ItemCount, GIOT_NONE, kItem)) )
		{
			kOrderList.push_back(kItem);
		}
		PgAction_CreateItem kCreateItem(CIE_Constellation, pkGround->GroundKey(), kOrderList);//아이템 지급 요청
		if(false == kCreateItem.DoAction(pkPlayer,NULL)) { continue; }
		kRewardInfo.iRewardItemNo = kItem.ItemNo();

		//step2: 추가보상할 아이템백을 얻고 하나를 랜덤으로 뽑아서 지급
		int iItemBagNo = 0;
		if(E_FAIL != kItemBagMgr.GetItemBagNoByGrp(kValue.Reward.BagGroupNo, pkPlayer->GetAbil(AT_LEVEL), iItemBagNo) && iItemBagNo != 0)
		{
			kRewardInfo.iAddChoiceRewardBagNo = iItemBagNo;
			kRewardInfo.eStep = SRewardItemInfo::EPRS_BEGIN;

			ProcessSecondaryReward(pkPlayer, pkGround); //아이템백에서 랜덤으로 인덱스 하나를 뽑아놓고
		}

	}

	return true;
}

bool PgConstellationResult::ProcessSecondaryReward(PgPlayer* pkReqPlayer, PgIndun* pkGround)
{
	if(pkReqPlayer == NULL || pkReqPlayer->IsDead() == true) { return false; }

	BM::CAutoMutex Lock(m_kMutex);
	CONT_REWARD_INFO::iterator iterRewardInfo = m_kContRewardInfo.find(pkReqPlayer);
	if(iterRewardInfo == m_kContRewardInfo.end()) { return false; }
	CONT_REWARD_INFO::mapped_type& kRewardInfo = iterRewardInfo->second;

	int iChoiceCount = GetCurChoiceRewardIndex(pkReqPlayer, &kRewardInfo);
	if(iChoiceCount == -1 || iChoiceCount >= MAX_REWARD_CHOICE) { return false; } //이미 3개 이상을 모두 선택했다면

	//아이템백 얻기
	if(kRewardInfo.iAddChoiceRewardBagNo == 0) { return false; }
	PgItemBag kItemBag;
	GET_DEF(CItemBagMgr, kItemBagMgr);
	if(S_OK != kItemBagMgr.GetItemBag(kRewardInfo.iAddChoiceRewardBagNo, pkReqPlayer->GetAbil(AT_LEVEL), kItemBag)) { return false; }
	size_t iItemBagSize = kItemBag.GetElementsCount();
	if(iItemBagSize < MAX_REWARD_CHOICE)
	{ //아이템백 사이즈는 최소 9개 이상이어야 한다.
		return false;
	}
	if(iItemBagSize > MAX_REWARD_CHOICE)
	{
		iItemBagSize = MAX_REWARD_CHOICE;
	}


	//지급될 아이템을 결정
	int iItemNo = 0;
	size_t iItemCount = 1;
	int iItemIdx = 0;
	int iTry = 200;
	int iRandIdx = -1;
	bool bDupNo = false;
	do
	{ //이전에 이미 선택된(중복) 번호인지 검사
		bDupNo = false;
		iRandIdx = BM::Rand_Index(iItemBagSize); //0~8
		if(S_OK != kItemBag.PopItemToIndex(pkReqPlayer->GetAbil(AT_LEVEL), iItemNo, iItemCount, iRandIdx)) { break; }

		if( 0 == iRandIdx )
		{
			for(int i = 0; i < MAX_REWARD_CHOICE; i++)
			{
				if( 0 == kRewardInfo.iAddChoiceRewardItemIndex[i] )
				{
					if( 0 != kRewardInfo.iAddChoiceRewardItemNo[i] )
					{
						bDupNo = true;
						break;
					}
				}
			}
		}
		else
		{
			for(int i = 0; i < MAX_REWARD_CHOICE; i++)
			{
				if(kRewardInfo.iAddChoiceRewardItemIndex[i] == iRandIdx)
				{
					bDupNo = true;
					break;
				}
			}
		}
	}while(bDupNo == true && --iTry >= 0);

	if(bDupNo == true || iRandIdx == -1)
	{ //랜덤 인덱스 얻기에 실패했다면 종료
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("PgConstellationResult: Item Count Error!\n") << _T("ItemBagNo: ") << kRewardInfo.iAddChoiceRewardBagNo << _T(", ItemNo: ") << iItemNo << _T(", ItemCount: ") << iItemCount);
		return false;
	}
	iItemIdx = iRandIdx;

	kRewardInfo.iAddChoiceRewardItemIndex[iChoiceCount] = iItemIdx; //아이템백의 인덱스 번호를 저장
	kRewardInfo.iAddChoiceRewardItemNo[iChoiceCount] = iItemNo; //아이템백의 인덱스에 해당하는 아이템 번호를 저장
	kRewardInfo.iAddChoiceRewardItemCount[iChoiceCount] = iItemCount; //아이템백의 인덱스에 해당하는 아이템 갯수를 저장
	{ //아이템 지급 처리 중
		kRewardInfo.eStep = SRewardItemInfo::EPRS_PROCESS;
	}

	return true;
}


bool PgConstellationResult::IsComplete(void)
{
	BM::CAutoMutex Lock(m_kMutex);
	CONT_REWARD_INFO::const_iterator iterRewardInfo = m_kContRewardInfo.begin();
	for(; iterRewardInfo != m_kContRewardInfo.end(); ++iterRewardInfo)
	{ //보상 아이템을 지급 중이라면 즉시 끝낼 수 없다.
		SRewardItemInfo const& kRewardInfo = iterRewardInfo->second;
		if(kRewardInfo.eStep == SRewardItemInfo::EPRS_PROCESS)
		{
			return false;
		}
	}

	return true;
}

