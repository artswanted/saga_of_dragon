#include "stdafx.h"
#include "PgJobSkillMgr.h"
#include "Variant/PgJobSkill.h"
#include "Variant/PgJobSkillTool.h"

void PgJobSkillLocationMgrImpl::Clear()
{
	m_kJobSkill_LocationItem.Clear();
	m_kJobSkill_LocationUser.Clear();
}

bool PgJobSkillLocationMgrImpl::IsEmpty()
{
	return m_kJobSkill_LocationItem.IsEmpty();
}

bool PgJobSkillLocationMgrImpl::AddElement(GTRIGGER_ID kKey, SJobSkill_LocationItem kInfo)
{
	return m_kJobSkill_LocationItem.Set(kKey, kInfo);
}

bool PgJobSkillLocationMgrImpl::GetLocationElement(GTRIGGER_ID kKey, SJobSkill_LocationItem& rkOut)
{
	return m_kJobSkill_LocationItem.Get(kKey, rkOut);
}

bool PgJobSkillLocationMgrImpl::GetDefLocationItem(int const iJobGrade, CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type& rkOut)
{
	return m_kJobSkill_LocationItem.GetDefLocationItem(iJobGrade, rkOut);
}

bool PgJobSkillLocationMgrImpl::IsEnable(std::string const& kTriggerID) const
{
	return m_kJobSkill_LocationItem.IsEnable(kTriggerID);
}

bool PgJobSkillLocationMgrImpl::SetEnable(GTRIGGER_ID const& kKey, bool bEnable)
{
	return m_kJobSkill_LocationItem.SetEnable(kKey, bEnable);
}

bool PgJobSkillLocationMgrImpl::WriteToPacketLocation(BM::Stream& rkPacket)
{
	rkPacket.Clear();

	rkPacket.Push((BM::Stream::DEF_STREAM_TYPE)PT_M_C_NFY_LOCATION_ITEM);
	m_kJobSkill_LocationItem.WriteToPacket(rkPacket);

	return true;
}

bool PgJobSkillLocationMgrImpl::GetGatherType(std::string const& kTriggerID, EGatherType& iOutValue) const
{
	return m_kJobSkill_LocationItem.GetGatherType(kTriggerID, iOutValue);
}

bool PgJobSkillLocationMgrImpl::GetJobGrade(std::string const& kTriggerID, int& iOutValue) const
{
	return m_kJobSkill_LocationItem.GetJobGrade(kTriggerID, iOutValue);
}

bool PgJobSkillLocationMgrImpl::GetCreateItem(std::string const& kTriggerID, ProbabilityUtil::SGetOneArguments& rkArg, int const iUseToolItemNo, PgBase_Item &kOutItem)
{
	const CONT_DEF_JOBSKILL_PROBABILITY_BAG *pkJobSkillProbability;
	g_kTblDataMgr.GetContDef(pkJobSkillProbability);
	if( !pkJobSkillProbability )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_JOBSKILL_PROBABILITY_BAG is NULL") );
		return false;
	}

	int iJobGrade = 0;
	CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type kElement;
	if( GetJobGrade(kTriggerID, iJobGrade) )
	{
		if( GetDefLocationItem(iJobGrade, kElement) )
		{
			PgJobSkillLocationUser::VALUE_TYPE::mapped_type kUserValue;
			if( !GetUser(kTriggerID, rkArg.kCharGuid, kUserValue) )
			{
				return false;
			}

			// (PC숙련도-기준숙련도) X [확률증가]
			int const iLimitExpertness = 1000000;
			int iAddResultProbability = std::min((kUserValue.iExpertness - kElement.iBase_Expertness), iLimitExpertness) * kElement.iProbability_UpRate / JSE_PROBABILITY_UPRATE_DEVIDE;
			if( 0 > iAddResultProbability )
			{
				iAddResultProbability = 0;
			}
			//Tool 테이블에서 Result_Min과 Result_Max값을 받아오자
			CONT_DEF_JOBSKILL_TOOL const* pkDefJobSkillTool;
			g_kTblDataMgr.GetContDef(pkDefJobSkillTool);
			CONT_DEF_JOBSKILL_TOOL::const_iterator tool_iter = pkDefJobSkillTool->find(iUseToolItemNo);
			if( tool_iter == pkDefJobSkillTool->end() )
			{
				return false;
			}
			CONT_DEF_JOBSKILL_TOOL::mapped_type rkToolInfo = (*tool_iter).second;
			//MIn~Max 사이의 값을 임의로 받아와서 LocationItem의 ResultProbability[01~10]중 하나를 선택하자.
			int const iResultProbabilityNo = BM::Rand_Range(rkToolInfo.iResult_No_Max, rkToolInfo.iResult_No_Min);
			if( 0 >= iResultProbabilityNo
				|| iResultProbabilityNo > COUNT_JOBSKILL_RESULT_PROBABILITY_NO)
			{// iResultProbabilityNo 의 값은 1~설정된 최대값(현재 10)이어야 한다.
				return false;
			}
			SProbability kProbability = ProbabilityUtil::GetOne(rkArg, *pkJobSkillProbability, kElement.iResultProbability_No[iResultProbabilityNo-1], iAddResultProbability);

			int const iItemNo = kProbability.iResultNo;
			int const iCount = kProbability.iCount;

			if( 0 < iItemNo
			&&	0 < iCount
			&&	S_OK == CreateSItem(iItemNo, iCount, GIOT_NONE, kOutItem))
			{
				return true;
			}
		}
	}
	return false;
}

bool PgJobSkillLocationMgrImpl::GetEventLocationItem(int const iJobGrade, CONT_HAVE_ITEM_DATA_ALL &kContOutItem, CONT_DEF_JOBSKILL_EVENT_LOCATION const *pkContLocation, CONT_DEF_EVENT_REWARD_ITEM_GROUP const *pkEventRewardItemGroup, BM::PgPackedTime const kCurTime)
{
	if( !pkContLocation || !pkEventRewardItemGroup)
	{
		return false;
	}

	CONT_DEF_JOBSKILL_EVENT_LOCATION::const_iterator itor_location = pkContLocation->begin();
	while( pkContLocation->end() != itor_location )
	{
		if( iJobGrade == (*itor_location).iJobGrade 
			&& (*itor_location).kStartDate < kCurTime
			&& kCurTime < (*itor_location).kEndDate )
		{
			int const iRate = (*itor_location).iRate;
			int const iRandRate = BM::Rand_Range(10000, 0);
			if( iRate < iRandRate )
			{
				++itor_location;
				continue;
			}
			CONT_DEF_EVENT_REWARD_ITEM_GROUP::const_iterator item_iter = pkEventRewardItemGroup->find( (*itor_location).iRewardItemGroup );
			if( item_iter == pkEventRewardItemGroup->end() )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot Find Reward Item Group No: ") << (*itor_location).iRewardItemGroup);
				return false;
			}

			int TotalRate = 0;
			bool bAlldrop = false;
			bool ExceptionCase = false;
			VEC_EVENTREWARDITEMGROUP::const_iterator drop_iter = (*item_iter).second.begin();
			for( ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
			{
				TotalRate += (*drop_iter).Rate;
			}
			int AllDropRate = (*item_iter).second.size() * 100;
			if( AllDropRate == TotalRate )
			{	// 컨테이너에 들어있는 아이템 드랍 확률이 모두 100이라면.
				for( drop_iter = (*item_iter).second.begin() ; drop_iter != (*item_iter).second.end() ; ++drop_iter )
				{
					AddEventLocationItem( (*drop_iter).ItemNo, (*drop_iter).Count, kContOutItem );
				}
			}
			else if( TotalRate < 100 )
			{	// 총합확률이 100이 안되면 예외 케이스로, 각 아이템은 백분률로 계산.
				int Rate = BM::Rand_Range(100, 0);
				for(drop_iter = (*item_iter).second.begin() ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
				{
					if( Rate <= (*drop_iter).Rate )
					{
						AddEventLocationItem( (*drop_iter).ItemNo, (*drop_iter).Count, kContOutItem );
						break;
					}
				}
			}
			else
			{
				int Rate = BM::Rand_Index(TotalRate);
				int CurRate = 0;
				for(drop_iter = (*item_iter).second.begin() ; drop_iter != (*item_iter).second.end() ; ++drop_iter)
				{								
					CurRate += (*drop_iter).Rate;
					if( Rate < CurRate )
					{
						AddEventLocationItem( (*drop_iter).ItemNo, (*drop_iter).Count, kContOutItem );
						break;
					}
				}
			}
		}
		++itor_location;
	}
	return true;
}

void PgJobSkillLocationMgrImpl::AddEventLocationItem( int const iItemNo, int const iCount, CONT_HAVE_ITEM_DATA_ALL &kContOutItem )
{
	for( int i = 0 ; i < iCount ; ++i )
	{
		PgBase_Item kOutItem;
		if(S_OK == CreateSItem(iItemNo, 1, GIOT_FIELD, kOutItem))
		{
			kContOutItem.push_back(kOutItem);
		}
	}
}

bool PgJobSkillLocationMgrImpl::IsSpot(std::string const& kTriggerID) const
{
	return m_kJobSkill_LocationItem.IsSpot(kTriggerID);
}

bool PgJobSkillLocationMgrImpl::SetSpotProbability(std::string const& kTriggerID)
{
	return m_kJobSkill_LocationItem.SetSpotProbability(kTriggerID);
}

void PgJobSkillLocationMgrImpl::SendLocationInfo(BM::Stream& rkPacket, CONT_JOBSKILL_LOCATIONINFO::key_type const& kTriggerID, CONT_JOBSKILL_LOCATIONINFO::mapped_type const& kElement)
{
	rkPacket.Clear();

	rkPacket.Push((BM::Stream::DEF_STREAM_TYPE)PT_M_C_NFY_LOCATION_INFO);
	rkPacket.Push(kTriggerID);
	kElement.WriteToPacket(rkPacket);	
}

void PgJobSkillLocationMgrImpl::GetAll(CONT_JOBSKILL_LOCATIONINFO& kLocationItem)
{
	m_kJobSkill_LocationItem.GetAll(kLocationItem);
}

void PgJobSkillLocationMgrImpl::SendLocationAction(BM::Stream& rkPacket, bool bResult)
{
	rkPacket.Clear();

	rkPacket.Push((BM::Stream::DEF_STREAM_TYPE)PT_M_C_NFY_LOCATION_ACTION);
	rkPacket.Push(bResult);

	if( !bResult )
	{
		return;
	}
}

bool PgJobSkillLocationMgrImpl::CheckSkillExpertness(PgPlayer* pkPlayer, int const iJobGrade, int &iErrMsgNo)
{
	if( !pkPlayer )
	{
		return false;
	}

	CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type kJobElement;
	GetDefLocationItem(iJobGrade, kJobElement); 

	int iSkillNo[SKILL_EXPERTNESS_MAX] = {0,};
	int iSkillExpertness[SKILL_EXPERTNESS_MAX] = {0,};
	int iSaveIdx[SKILL_EXPERTNESS_MAX] = {0,};

	iSkillNo[0] = kJobElement.i01Need_Skill_No;
	iSkillNo[1] = kJobElement.i02Need_Skill_No;
	iSkillNo[2] = kJobElement.i03Need_Skill_No;
	iSkillExpertness[0] = kJobElement.i01Need_Expertness;
	iSkillExpertness[1] = kJobElement.i02Need_Expertness;
	iSkillExpertness[2] = kJobElement.i03Need_Expertness;
	iSaveIdx[0] = kJobElement.i01Need_SaveIdx;
	iSaveIdx[1] = kJobElement.i02Need_SaveIdx;
	iSaveIdx[2] = kJobElement.i03Need_SaveIdx;

	for(int i=0; i<SKILL_EXPERTNESS_MAX; ++i)
	{
		if( 0 < iSkillNo[i] )
		{
			PgJobSkillExpertness::VALUE_TYPE kExpertness;

			if( !pkPlayer->JobSkillExpertness().Get(iSkillNo[i], kExpertness) )
			{
				// 안배움
				//iErrMsgNo = 25002;
				BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_NEED_SKILL);
				kPacket.Push( iSkillNo[i] );
				pkPlayer->Send(kPacket);
				return false;
			}

			if( iSkillExpertness[i] > kExpertness )
			{
				// 숙련도 미달
				BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_NEED_SKILL_EXPERTNESS);
				kPacket.Push( iSkillNo[i] );
				kPacket.Push( iSkillExpertness[i] );
				pkPlayer->Send(kPacket);
				return false;
			}
		}
	}	

	return true;
}

bool PgJobSkillLocationMgrImpl::SetUser(GTRIGGER_ID kKey, PgPlayer* pkPlayer, int const iSkillNo, DWORD& dwOutTurnTime, int const iUseExhaustion)
{	
	if( 0 < iSkillNo )
	{
		PgJobSkillExpertness::VALUE_TYPE kExpertness;

		if( pkPlayer->JobSkillExpertness().Get(iSkillNo, kExpertness) )
		{
			EGatherType iGatherType = GT_None;
			if( GetGatherType(kKey, iGatherType) )
			{// 채집 하는데
				int const iItemNo = JobSkillToolUtil::GetUseItem(pkPlayer, iGatherType, iSkillNo);
				DWORD const dwDrationTime = JobSkillExpertnessUtil::GetBasicTurnTime(iSkillNo, kExpertness);
				dwOutTurnTime = JobSkillToolUtil::CalcOptionTurnTime(iItemNo, dwDrationTime);
				static __int64 const i64MinimumTurnTime = 2000;
				// 채집 시간 감소하는 어빌이 있다면
				int iJobSkill_AddTurnTime = AT_JS_1ST_ADD_SKILL_TURNTIME;
				if( JST_1ST_SUB == JobSkill_Util::GetJobSkillType(iSkillNo) )
				{
					iJobSkill_AddTurnTime = AT_JS_1ST_SUB_ADD_SKILL_TURNTIME;
				}
				// 채집 시간을 감소시켜 주는데
				__int64 i64TotalTime = 	dwOutTurnTime;
				i64TotalTime += pkPlayer->GetAbil(iJobSkill_AddTurnTime);
				// i64MinimumTurnTime보다 적을수는 없다
				dwOutTurnTime = static_cast<DWORD>( std::max(i64MinimumTurnTime, i64TotalTime) );
				m_kJobSkill_LocationUser.Set(kKey, pkPlayer->GetID(), iSkillNo, kExpertness, dwOutTurnTime, iItemNo, iUseExhaustion);
				return true;
			}
		}
	}

	return false;
}

//bool PgJobSkillLocationMgrImpl::GetUser(GTRIGGER_ID kKey, PgJobSkillLocationUser::VALUE_TYPE& rkOut) const
//{
//	return m_kJobSkill_LocationUser.Get(kKey, rkOut);
//}

bool PgJobSkillLocationMgrImpl::GetUser(GTRIGGER_ID kKey, BM::GUID kCharGuid, PgJobSkillLocationUser::VALUE_TYPE::mapped_type& rkOut) const
{
	return m_kJobSkill_LocationUser.Get(kKey, kCharGuid, rkOut);
}

void PgJobSkillLocationMgrImpl::DelUser(GTRIGGER_ID kKey, BM::GUID kCharGuid)
{
	m_kJobSkill_LocationUser.Del(kKey, kCharGuid);
}

int PgJobSkillLocationMgrImpl::GetSkillNo(int const iJobGrade)
{
	CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type kJobElement;
	GetDefLocationItem(iJobGrade, kJobElement); 

	return kJobElement.i01Need_Skill_No;
}

void PgJobSkillLocationMgrImpl::LocationDrationTime(CONT_JOBSKILL_LOCATIONINFO& kOut)
{
	m_kJobSkill_LocationItem.LocationDrationTime(kOut);
}

void PgJobSkillLocationMgrImpl::UserDrationSkillTime(CONT_JOBSKILL_LOCATIONITEM_RESULT& kOut)
{
	m_kJobSkill_LocationUser.UserDrationSkillTime(kOut);
}

//===============================================================================================================================================================================================

void PgJobSkillLocationMgr::Clear()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	Instance()->Clear();
}

bool PgJobSkillLocationMgr::IsEmpty()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->IsEmpty();
}

bool PgJobSkillLocationMgr::AddElement(GTRIGGER_ID kKey, SJobSkill_LocationItem kInfo)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	return Instance()->AddElement(kKey, kInfo);
}

bool PgJobSkillLocationMgr::GetLocationElement(GTRIGGER_ID kKey, SJobSkill_LocationItem& rkOut)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->GetLocationElement(kKey, rkOut);
}

bool PgJobSkillLocationMgr::GetDefLocationItem(int const iJobGrade, CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type& rkOut)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->GetDefLocationItem(iJobGrade, rkOut);
}

bool PgJobSkillLocationMgr::IsEnable(std::string const& kTriggerID) const
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->IsEnable(kTriggerID);
}

bool PgJobSkillLocationMgr::SetEnable(GTRIGGER_ID const& kKey, bool bEnable)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	return Instance()->SetEnable(kKey, bEnable);
}

bool PgJobSkillLocationMgr::WriteToPacketLocation(BM::Stream& rkPacket)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->WriteToPacketLocation(rkPacket);
}

bool PgJobSkillLocationMgr::GetGatherType(std::string const& kTriggerID, EGatherType& iOutValue) const
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->GetGatherType(kTriggerID, iOutValue);
}

bool PgJobSkillLocationMgr::GetJobGrade(std::string const& kTriggerID, int& iOutValue) const
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->GetJobGrade(kTriggerID, iOutValue);
}

bool PgJobSkillLocationMgr::GetCreateItem(std::string const& kTriggerID, ProbabilityUtil::SGetOneArguments& rkArg, int const iUseToolItemNo, PgBase_Item &kOutItem)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->GetCreateItem(kTriggerID, rkArg, iUseToolItemNo, kOutItem);
}

bool PgJobSkillLocationMgr::GetEventLocationItem(int const iJobGrade, CONT_HAVE_ITEM_DATA_ALL &kContOutItem, CONT_DEF_JOBSKILL_EVENT_LOCATION const *pkContLocation, CONT_DEF_EVENT_REWARD_ITEM_GROUP const *pkEventRewardItemGroup, BM::PgPackedTime const kCurTime)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	if( !pkContLocation )
	{ return false; }
	if( !pkEventRewardItemGroup )
	{ return false; }
	return Instance()->GetEventLocationItem(iJobGrade, kContOutItem, pkContLocation, pkEventRewardItemGroup, kCurTime);
}
bool PgJobSkillLocationMgr::IsSpot(std::string const& kTriggerID) const
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->IsSpot(kTriggerID);
}

bool PgJobSkillLocationMgr::SetSpotProbability(std::string const& kTriggerID)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->SetSpotProbability(kTriggerID);
}

void PgJobSkillLocationMgr::SendLocationInfo(BM::Stream& rkPacket, CONT_JOBSKILL_LOCATIONINFO::key_type const& kTriggerID, CONT_JOBSKILL_LOCATIONINFO::mapped_type const& kElement)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	Instance()->SendLocationInfo(rkPacket, kTriggerID, kElement);
}

void PgJobSkillLocationMgr::GetAll(CONT_JOBSKILL_LOCATIONINFO& kLocationItem)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	Instance()->GetAll(kLocationItem);
}

void PgJobSkillLocationMgr::SendLocationAction(BM::Stream& rkPacket, bool bResult)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	Instance()->SendLocationAction(rkPacket, bResult);	
}

bool PgJobSkillLocationMgr::CheckSkillExpertness(PgPlayer* pkPlayer, int const iJobGrade, int &iErrMsgNo)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->CheckSkillExpertness(pkPlayer, iJobGrade, iErrMsgNo);
}

bool PgJobSkillLocationMgr::SetUser(GTRIGGER_ID kKey, PgPlayer* pkPlayer, int const iSkillNo, DWORD& dwOutTurnTime, int const iUseExhaustion)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	return Instance()->SetUser(kKey, pkPlayer, iSkillNo, dwOutTurnTime, iUseExhaustion);
}

//bool PgJobSkillLocationMgr::GetUser(GTRIGGER_ID kKey, PgJobSkillLocationUser::VALUE_TYPE& rkOut) const
//{
//	BM::CAutoMutex lock(m_kMutex_Wrapper_);
//
//	return Instance()->GetUser(kKey, rkOut);
//}

bool PgJobSkillLocationMgr::GetUser(GTRIGGER_ID kKey, BM::GUID kCharGuid, PgJobSkillLocationUser::VALUE_TYPE::mapped_type& rkOut) const
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->GetUser(kKey, kCharGuid, rkOut);
}

void PgJobSkillLocationMgr::DelUser(CUnit* pkUnit, GTRIGGER_ID kKey, BM::GUID kCharGuid)
{
	if(pkUnit)
	{//유닛이 있을 경우에만 장착 해제 어빌 제거
		pkUnit->SetAbil(AT_CANNOT_EQUIP, 0);
	}
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	Instance()->DelUser(kKey, kCharGuid);
}

int PgJobSkillLocationMgr::GetSkillNo(int const iJobGrade)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->GetSkillNo(iJobGrade);	
}

void PgJobSkillLocationMgr::LocationDrationTime(CONT_JOBSKILL_LOCATIONINFO& kOut)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->LocationDrationTime(kOut);
}

void PgJobSkillLocationMgr::UserDrationSkillTime(CONT_JOBSKILL_LOCATIONITEM_RESULT& kOut)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	Instance()->UserDrationSkillTime(kOut);
}