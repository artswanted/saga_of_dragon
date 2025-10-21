#include "stdafx.h"
#include "PgDBCache.h"
#include "PgPlayer_MissionReport.h"

// ------------------------------------------
// PgMissionReport

PgPlayer_MissionData::PgPlayer_MissionData()
{
	Clear();
}

PgPlayer_MissionData::~PgPlayer_MissionData()
{
}

void PgPlayer_MissionData::Clear()
{
	::memset( m_kData, 0, sizeof(m_kData) );
	m_dwExtData = 0;
}

// 플레이가 가능한 레벨이냐?
bool PgPlayer_MissionData::IsPlayingLevel( size_t const iLevel ) const
{
	switch( iLevel )
	{
	case 0:{return true;}break;
	case 1:case 2:case 3:
		{
			return IsClearLevel( iLevel-1 );
		}
	case 4:
	case 5:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Error Level[") << iLevel << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPlayer_MissionData::IsClearLevel( size_t const iLevel )const
{
	if ( (iLevel < MAX_MISSION_LEVEL) )
	{
		return ( m_kData[iLevel].kClearCount > 0 );
	}
	else
	{
		if( DEFENCE_MISSION_LEVEL <= (iLevel+1) )
		{
			return false;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Error Level[") << iLevel << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPlayer_MissionData::IsPlayingLevel( std::vector<int> const &kMissionOpenPreLevel )const
{
	return IsClearLevel( kMissionOpenPreLevel );
}

bool PgPlayer_MissionData::IsClearLevel( std::vector<int> const &kMissionOpenPreLevel )const
{
	std::vector<int>::const_iterator itr = kMissionOpenPreLevel.begin();
	while( kMissionOpenPreLevel.end() != itr )
	{
		if ( !IsClearLevel( *itr-1 ) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		++itr;
	}
	return true;
}

HRESULT PgPlayer_MissionData::StageOn( size_t const iLevel, size_t const iStage )
{
	if ( iStage > MAX_MISSION_CANDIDATE )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Error Level[") << iLevel << _T("] Stage[") << iStage << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	BYTE kStageFlag = 0x01 << (BYTE)iStage;
	return StageOnForFlag( iLevel, kStageFlag );
}

HRESULT PgPlayer_MissionData::StageOnForFlag( size_t const iLevel, BYTE const kStageFlag )
{
	if ( iLevel < MAX_MISSION_LEVEL )
	{// kStageFlag가 0이면 LastStage이다.
		if ( kStageFlag )
		{
			if ( 0 == (m_kData[iLevel].kStageFlag & kStageFlag) )
			{
				m_kData[iLevel].kStageFlag |= kStageFlag;
				return S_OK;
			}
		}
		else
		{	
			if ( UCHAR_MAX > m_kData[iLevel].kClearCount )
			{
				++m_kData[iLevel].kClearCount;
				return S_OK;
			}
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
		return S_FALSE;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Error Level[") << iLevel << _T("] StageFlag[") << kStageFlag << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgPlayer_MissionData::IsClearStage( size_t const iLevel, size_t const iStage )const
{
	if ( iLevel < MAX_MISSION_LEVEL )
	{
		if ( iStage < MAX_MISSION_CANDIDATE )
		{
			return m_kData[iLevel].kStageFlag & (0x01<<iStage);
		}
		
		if ( iStage == MAX_MISSION_CANDIDATE )
		{
			return m_kData[iLevel].kClearCount > 0;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Error Level[") << iLevel << _T("] Stage[") << iStage << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int const PgPlayer_MissionData::GetClearLevelCount(size_t const iLevel )const
{
	if ( iLevel < MAX_MISSION_LEVEL )
	{
		return m_kData[iLevel].kClearCount;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

size_t PgPlayer_MissionData::GetClearStageCount( size_t const iLevel )const
{
	if ( iLevel < MAX_MISSION_LEVEL )
	{
		size_t iRet = (m_kData[iLevel].kClearCount > 0) ? 1:0;
		BYTE kFlag = 0x01;
		while ( kFlag )
		{
			if ( m_kData[iLevel].kStageFlag & kFlag )
			{
				++iRet;
			}
			kFlag <<= 1;
		}
		return iRet;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Error Level[") << iLevel << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

size_t PgPlayer_MissionData::min_size()const
{
	return sizeof(m_kData);
}

void PgPlayer_MissionData::WriteToPacket(BM::Stream& rkPacket)const
{
	rkPacket.Push( m_kData, sizeof(m_kData) );
}

void PgPlayer_MissionData::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.PopMemory( m_kData, sizeof(m_kData) );
}


//-->
unsigned int PgPlayer_MissionReport::GetMissionAreaSize( int const iArea )
{
	switch ( iArea )
	{
	case SECTION_MISSIONAREA_CB1:{return 25;}break;
	case SECTION_MISSIONAREA_CB2:{return 25;}break;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Bad Area[") << iArea << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

unsigned int PgPlayer_MissionReport::GetMissionAreaStartIndex( int const iArea )
{
	switch ( iArea )
	{
	case SECTION_MISSIONAREA_CB1:{return 0;}break;
	case SECTION_MISSIONAREA_CB2:{return 100;}break;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Bad Area[") << iArea << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

bool PgPlayer_MissionReport::GetMissionAreaAndIndex( unsigned int const iMissionKey, int &iOutArea, unsigned int& iOutIndex )
{
	// iOutArea : 대륙 번호
	// iOutIndex : Mission Key. Mission Key는 0~24, 100~124만 지정할 수있음( 최대 50개, DB구조상 제한. 유저가 미션 클리어 여부나 횟수를 저장하기 위함. ).
	if ( iMissionKey < 100 )
	{
		iOutArea = 0;					// Mission Key가 100보다 작으면 대륙 번호 0.
	}
	else
	{
		iOutArea = iMissionKey / 100;	// Mission Key가 100 이상이면 100으로 나눈 값(1) 이 대륙 번호.
	}

	iOutIndex = iMissionKey % 100;		// 대륙별로 25개까지만 미션을 지정할 수있기때문에 Mission Key의 나머지 값이 25 이상이면 안됨.
	if ( iOutIndex < GetMissionAreaSize( iOutArea ) )
	{
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" MissionKey[") << iMissionKey << _T("] Error!!! Mission Key Range Only 0~24 or 100~124!!!") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return false;
}

//<--
PgPlayer_MissionReport::PgPlayer_MissionReport()
{

}

PgPlayer_MissionReport::~PgPlayer_MissionReport()
{

}

PgPlayer_MissionReport& PgPlayer_MissionReport::operator=( PgPlayer_MissionReport const &rhs )
{
	m_kContData = rhs.m_kContData;
	return *this;
}

void PgPlayer_MissionReport::Clear()
{
	m_kContData.clear();
}

void PgPlayer_MissionReport::WriteToPacket_MissionReport( BM::Stream& rkPacket )const
{
	rkPacket.Push( m_kContData.size() );
	CONT_PLAYER_MISSIONREPORT::const_iterator itr = m_kContData.begin();
	for ( ; itr!=m_kContData.end(); ++itr )
	{
		rkPacket.Push( itr->first );
		PU::TWriteArray_M( rkPacket, itr->second );
	}
}

void PgPlayer_MissionReport::ReadFromPacket_MissionReport( BM::Stream& rkPacket )
{
	size_t iDataSize = 0;
	CONT_PLAYER_MISSIONREPORT::key_type kKey = 0;

	rkPacket.Pop( iDataSize );
	while ( iDataSize-- )
	{
		if ( rkPacket.Pop( kKey ) )
		{
			auto kPair = m_kContData.insert( std::make_pair( kKey, CONT_PLAYER_MISSIONREPORT::mapped_type() ) );
			kPair.first->second.clear();
			PU::TLoadArray_M( rkPacket, kPair.first->second );
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Critical Error!!! DataSize[") << iDataSize << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Data Pop Error"));
			return;
		}
	}
}

bool PgPlayer_MissionReport::Load( int const iArea, CEL::DB_DATA_ARRAY::const_iterator &itr )
{
	CONT_PLAYER_MISSIONREPORT::mapped_type kVecMissionData;
	kVecMissionData.resize(25);

	if ( itr->PopMemory( &(kVecMissionData.at(0)), 400 ) )
	{
		auto kPair = m_kContData.insert( std::make_pair( iArea, kVecMissionData ) );
		if ( !kPair.second )
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Area[") << iArea << _T("] Load Overlap") );
			kPair.first->second.swap(kVecMissionData);
		}
		++itr;
		return true;
	}

	INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Load Error!! Area[") << iArea << _T("]") );
	++itr;
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgPlayer_MissionReport::WriteToQuery( CEL::DB_QUERY& kQuery )const
{
	CONT_PLAYER_MISSIONREPORT::const_iterator itr = m_kContData.begin();
	for( ; itr!=m_kContData.end() ; ++itr )
	{
		kQuery.PushStrParam( itr->second );
	}
}

bool PgPlayer_MissionReport::Update( SMissionKey const& kMissionKey, BYTE const kStageFlag )
{
	int iArea = 0;
	unsigned int iIndex = 0;
	if ( GetMissionAreaAndIndex( (unsigned int)kMissionKey.iKey, iArea, iIndex ) )
	{
		CONT_PLAYER_MISSIONREPORT::iterator itr = m_kContData.find(iArea);
		if ( itr != m_kContData.end() )
		{
			if ( S_OK == itr->second.at(iIndex).StageOnForFlag( (size_t)kMissionKey.iLevel, kStageFlag ) )
			{
				return true;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Error MissionKey[") << kMissionKey.iKey << _T("] MissionLevel[") << kMissionKey.iLevel << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

PgPlayer_MissionData *PgPlayer_MissionReport::Get( unsigned int const iMissionKey )
{
	int iArea = 0;
	unsigned int iIndex = 0;
	if ( GetMissionAreaAndIndex( iMissionKey, iArea, iIndex ) )
	{
		CONT_PLAYER_MISSIONREPORT::iterator itr = m_kContData.find(iArea);
		if ( itr != m_kContData.end() )
		{
			return &(itr->second.at(iIndex));
		}
	}
	INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Error MissionKey[") << iMissionKey << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}