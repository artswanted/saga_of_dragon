#include "stdafx.h"
#include "Item.h"
#include "tabledatamanager.h"
#include "PgJobSkillLocationItem.h"
#include "Variant/PgJobSkillExpertness.h"
#include "global.h"

PgJobSkillLocationItem::PgJobSkillLocationItem()
{
	m_kContLocationItem.clear();
}

PgJobSkillLocationItem::~PgJobSkillLocationItem()
{
}

void PgJobSkillLocationItem::Clear()
{
	RemoveAll();
}

bool PgJobSkillLocationItem::IsEmpty()
{
	return m_kContLocationItem.empty();
}

void PgJobSkillLocationItem::WriteToPacket(BM::Stream & kPacket) const
{
	PU::TWriteTable_AM(kPacket, m_kContLocationItem);
}

void PgJobSkillLocationItem::ReadFromPacket(BM::Stream & kPacket)
{
	PU::TLoadTable_AM(kPacket, m_kContLocationItem);
}

void PgJobSkillLocationItem::RemoveAll()
{
	CONT_JOBSKILL_LOCATIONINFO kTemp;
	kTemp.swap(m_kContLocationItem);
}

bool PgJobSkillLocationItem::RemoveElement(KEY_TYPE const& kTriggerID)
{
	return m_kContLocationItem.erase(kTriggerID) ? true : false;
}

bool PgJobSkillLocationItem::Set(std::string const& kTriggerID, VALUE_TYPE& kValue)
{
	auto kRet = m_kContLocationItem.insert(std::make_pair(kTriggerID, kValue));
	if(!kRet.second)
	{
		kRet.first->second = kValue;
	}
	return true;
}

bool PgJobSkillLocationItem::Get(KEY_TYPE const& kTriggerID, VALUE_TYPE& kOutValue) const
{
	CONT_JOBSKILL_LOCATIONINFO ::const_iterator iter = m_kContLocationItem.find(kTriggerID);
	if(m_kContLocationItem.end() != iter)
	{
		kOutValue = iter->second;
		return true;
	}
	return false;
}

void PgJobSkillLocationItem::GetAll(CONT_JOBSKILL_LOCATIONINFO& kLocationItem)
{
	CONT_JOBSKILL_LOCATIONINFO::const_iterator itor = m_kContLocationItem.begin();

	while(itor != m_kContLocationItem.end())
	{
		auto kRet = kLocationItem.insert(std::make_pair(itor->first, itor->second));
		if( !kRet.second )
		{
		}
		++itor;
	}
}

bool PgJobSkillLocationItem::GetJobGrade(std::string const& kTriggerID, int& iOutValue) const
{
	CONT_JOBSKILL_LOCATIONINFO ::const_iterator iter = m_kContLocationItem.find(kTriggerID);
	if(m_kContLocationItem.end() != iter)
	{
		iOutValue = iter->second.iJobGrade;
		return true;
	}
	return false;
}

bool PgJobSkillLocationItem::IsEnable(std::string const& kTriggerID) const
{
	CONT_JOBSKILL_LOCATIONINFO ::const_iterator iter = m_kContLocationItem.find(kTriggerID);
	if(m_kContLocationItem.end() != iter)
	{
		return iter->second.bEnable;
	}
	return false;
}

bool PgJobSkillLocationItem::SetEnable(std::string const& kTriggerID, bool bEnable)
{
	CONT_JOBSKILL_LOCATIONINFO::iterator iter = m_kContLocationItem.find(kTriggerID);
	if( m_kContLocationItem.end() != iter )
	{
		CONT_JOBSKILL_LOCATIONINFO::mapped_type &kElement = iter->second;
		if( kElement.bSpot )	// Spot인 것만 변경 가능
		{
			kElement.bEnable = bEnable;
			return true;
		}
	}
	return false;
}

bool PgJobSkillLocationItem::GetGatherType(std::string const& kTriggerID, EGatherType& iOutValue) const
{
	CONT_JOBSKILL_LOCATIONINFO ::const_iterator iter = m_kContLocationItem.find(kTriggerID);
	if(m_kContLocationItem.end() != iter)
	{
		iOutValue = static_cast<EGatherType>(iter->second.iGatherType);
		return true;
	}
	return false;
}

bool PgJobSkillLocationItem::IsSpot(std::string const& kTriggerID) const
{
	CONT_JOBSKILL_LOCATIONINFO ::const_iterator iter = m_kContLocationItem.find(kTriggerID);
	if(m_kContLocationItem.end() != iter)
	{
		return iter->second.bSpot;
	}
	return false;
}

bool PgJobSkillLocationItem::SetSpotTime(std::string const& kTriggerID, DWORD dwTime, int const iCycle, int const iDration)
{
	CONT_JOBSKILL_LOCATIONINFO::iterator iter = m_kContLocationItem.find(kTriggerID);
	if( m_kContLocationItem.end() != iter )
	{
		CONT_JOBSKILL_LOCATIONINFO::mapped_type &kElement = iter->second;
		if( kElement.bSpot )	// Spot인 것만 변경 가능
		{
			kElement.dwSpot_StartTime = dwTime;
			kElement.iSpot_Cycle = iCycle;
			kElement.iSpot_Dration = iDration;
			kElement.bEnable = true;

			return true;
		}
	}
	return false;
}

void PgJobSkillLocationItem::LocationDrationTime(CONT_JOBSKILL_LOCATIONINFO& kOut)
{
	DWORD kNow = BM::GetTime32();

	CONT_JOBSKILL_LOCATIONINFO::iterator iter = m_kContLocationItem.begin();
	while( m_kContLocationItem.end() != iter )
	{
		bool bChange = false;
		CONT_JOBSKILL_LOCATIONINFO::key_type const &kTriggerID = iter->first;
		CONT_JOBSKILL_LOCATIONINFO::mapped_type &kElement = iter->second;
		if( kElement.bSpot ) // Spot 지역
		{
			if( kElement.bEnable )
			{
				// 활성화 된 지역								
				if( kElement.iSpot_Dration <= DifftimeGetTime(kElement.dwSpot_StartTime, kNow) )
				{
					// Cycle 주기 // 비활성 시키기
					kElement.bEnable = false;
					//kElement.dwSpot_StartTime = kNow;

					bChange = true;
				}
			}
			else
			{
				// 비활성화 된 지역
				if( kElement.iSpot_Cycle <= DifftimeGetTime(kElement.dwSpot_StartTime, kNow) )
				{
					// 등장 시간 초과
					if( !SetSpotProbability(kTriggerID) )
					{
						// 실패한 경우 시간을 넣어준다.
						kElement.dwSpot_StartTime = kNow;						
					}
					bChange = true;
				}
			}

			if( bChange )
			{
				kOut.insert(std::make_pair(kTriggerID, kElement));
			}
		}
		++iter;
	}
}

bool PgJobSkillLocationItem::SetSpotProbability(std::string const& kTriggerID)
{
	/*if( !IsSpot(kTriggerID) )
	{
		return false;
	}*/

	DWORD dwNowTime = BM::GetTime32();
	int iJobGrade = 0;
	CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type kElement;
	if( GetJobGrade(kTriggerID, iJobGrade) )
	{
		if( GetDefLocationItem(iJobGrade, kElement) )
		{
			int const iSpotTotalRate = kElement.iSpot_TotalProbability;
			int const iSpotRate = kElement.iSpot_Probability;
			int const iRetSpotRate = BM::Rand_Range((iSpotTotalRate - 1), 0);
			if( iRetSpotRate < iSpotRate )
			{
				int const iSpot_CycleMin = kElement.iSpot_Cycle_Min;
				int const iSpot_CycleMax = kElement.iSpot_Cycle_Max;
				int const iSpot_Cycle = BM::Rand_Range((iSpot_CycleMax - 1), iSpot_CycleMin);

				int const iSpot_DrationMin = kElement.iSpot_Dration_Min;
				int const iSpot_DrationMax = kElement.iSpot_Dration_Max;
				int const iSpot_Dration = BM::Rand_Range((iSpot_DrationMax - 1), iSpot_DrationMin);
				
				return SetSpotTime(kTriggerID, dwNowTime, iSpot_Cycle, iSpot_Dration);
			}
		}
	}
	return false;
}

bool PgJobSkillLocationItem::GetDefLocationItem(int const iJobGrade, CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type& rkOut)
{
	const CONT_DEF_JOBSKILL_LOCATIONITEM *pkJobSkillLocationItem;
	g_kTblDataMgr.GetContDef(pkJobSkillLocationItem);
	if( !pkJobSkillLocationItem )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_JOBSKILL_LOCATIONITEM is NULL") );
		return false;
	}	
	else
	{		
		CONT_DEF_JOBSKILL_LOCATIONITEM::key_type		kKey(iJobGrade);
		CONT_DEF_JOBSKILL_LOCATIONITEM::const_iterator iter = pkJobSkillLocationItem->find(kKey);
		if( iter != pkJobSkillLocationItem->end() )
		{
			rkOut = iter->second;
			return true;
		}
	}
	return false;
}

//===============================================================================================================================================================================================

PgJobSkillLocationUser::PgJobSkillLocationUser()
{
	m_kContLocationUser.clear();
}

PgJobSkillLocationUser::~PgJobSkillLocationUser()
{
}

void PgJobSkillLocationUser::Clear()
{
	RemoveAll();
}

bool PgJobSkillLocationUser::IsEmpty()
{
	return m_kContLocationUser.empty();
}

void PgJobSkillLocationUser::WriteToPacket(BM::Stream & kPacket) const
{
	PU::TWriteTable_AA(kPacket, m_kContLocationUser);
}

void PgJobSkillLocationUser::ReadFromPacket(BM::Stream & kPacket)
{
	PU::TLoadTable_AA(kPacket, m_kContLocationUser);
}

void PgJobSkillLocationUser::RemoveAll()
{
	CONT_JOBSKILL_LOCATIONUSER kTemp;
	kTemp.swap(m_kContLocationUser);
}

void PgJobSkillLocationUser::Set(KEY_TYPE const& kTriggerID, BM::GUID const kCharGuid, int const iSkillNo, int const iExpertness, DWORD const dwDrationTime, int const iItemNo, int const iUseExhaustion)
{
	CONT_JOBSKILL_LOCATIONUSER::iterator iter = m_kContLocationUser.find(kTriggerID);
	if( m_kContLocationUser.end() == iter )
	{
		auto kRet = m_kContLocationUser.insert(std::make_pair(kTriggerID, CONT_JOBSKILL_LOCATIONUSER::mapped_type()));
		iter = kRet.first;
	}

	VALUE_TYPE::mapped_type kValue;
	kValue.dwStartTime = BM::GetTime32();
	kValue.dwDrationTime = dwDrationTime;
	kValue.kGuid = kCharGuid;
	kValue.iUseSkillNo = iSkillNo;
	kValue.iUseToolItemNo = iItemNo;
	kValue.iExpertness = iExpertness;
	kValue.iUseExhaustion = iUseExhaustion;

	auto kRet = (*iter).second.insert( std::make_pair(kValue.kGuid, kValue) );
	if( !kRet.second )
	{
		(*kRet.first).second = kValue;
	}
}

//bool PgJobSkillLocationUser::Get(KEY_TYPE const& kTriggerID, SJobSkillUserInfo& rkOut) const
//{
//	CONT_JOBSKILL_LOCATIONUSER::const_iterator iter = m_kContLocationUser.find(kTriggerID);
//	if( m_kContLocationUser.end() != iter )
//	{
//		VALUE_TYPE::const_iterator iValueIter = iter->second.begin();
//
//		rkOut.resize( iter->second.size() );
//		std::copy( iter->second.begin(), iter->second.end(), rkOut.begin() );
//		return true;
//	}
//	return false;
//}

bool PgJobSkillLocationUser::Get(KEY_TYPE const& kTriggerID, BM::GUID kCharGuid, SJobSkillUserInfo& rkOut) const
{
	CONT_JOBSKILL_LOCATIONUSER::const_iterator iter = m_kContLocationUser.find(kTriggerID);
	if( m_kContLocationUser.end() != iter )
	{
		CONT_JOBSKILL_LOCATIONUSER::mapped_type const &kElement = iter->second;

		CONT_JOBSKILL_LOCATIONUSER::mapped_type::const_iterator find_iter = kElement.find(kCharGuid);
		if( kElement.end() != find_iter )
		{
			rkOut = (*find_iter).second;
			return true;
		}
	}
	return false;
}

void PgJobSkillLocationUser::Del(KEY_TYPE const& kTriggerID, BM::GUID const& kCharGuid)
{
	CONT_JOBSKILL_LOCATIONUSER::iterator iter = m_kContLocationUser.find(kTriggerID);
	if( m_kContLocationUser.end() != iter )
	{
		CONT_JOBSKILL_LOCATIONUSER::mapped_type& kElement = iter->second;
		CONT_JOBSKILL_LOCATIONUSER::mapped_type::iterator find_iter = kElement.find(kCharGuid);
		if( kElement.end() != find_iter )
		{
			kElement.erase(find_iter);
		}
	}
}

void PgJobSkillLocationUser::UserDrationSkillTime(CONT_JOBSKILL_LOCATIONITEM_RESULT& kOut)
{
	DWORD kNow = BM::GetTime32();

	VALUE_TYPE::value_type kValue;
	CONT_JOBSKILL_LOCATIONUSER::iterator iter = m_kContLocationUser.begin();
	for(; m_kContLocationUser.end() != iter; ++iter)	
	{
		CONT_JOBSKILL_LOCATIONUSER::mapped_type& kElement = iter->second;
		CONT_JOBSKILL_LOCATIONUSER::mapped_type::iterator find_iter = kElement.begin();
		for(; kElement.end() != find_iter; ++find_iter )
		{
			if( (*find_iter).second.dwDrationTime <= DifftimeGetTime((*find_iter).second.dwStartTime, kNow) )
			{
				kOut.insert(std::make_pair(find_iter->first, iter->first));
			}
		}
	}
}

