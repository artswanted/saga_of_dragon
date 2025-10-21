#ifndef WEAPON_VARIANT_USERINFO_PGPLAYER_MISSIONREPROT_H
#define WEAPON_VARIANT_USERINFO_PGPLAYER_MISSIONREPROT_H

#include "Lohengrin/dbtables.h"

#pragma pack(1)
typedef struct tagMissionInfo
{
	enum eMissionDoor
	{
		MDOOR_NONE	= 0x0000,		// 워래 존재하지 않음
		MDOOR_OPEN  = 0x0001,		// 들어 갈 수 있음
		MDOOR_CLOSE	= 0x0002,		// 들어 갈 수 없음
		MDOOR_NEWOPEN = 0x0003,		// 새로 열렸음(히든)
		MDOOR_CLEARED = 0x0004,		// 클리어 했음
	};

	tagMissionInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_iMissionNo = 0;
		m_iMissionKey = 0;
		::memset( m_kDoor, 0, sizeof(m_kDoor) );
		m_kTotalPercent = 0;
	}

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		rkPacket.Push(m_iMissionNo);
		rkPacket.Push(m_iMissionKey);
		rkPacket.Push(m_kDoor, sizeof(m_kDoor));
		rkPacket.Push(m_kTotalPercent);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(m_iMissionNo);
		rkPacket.Pop(m_iMissionKey);
		rkPacket.PopMemory(m_kDoor, sizeof(m_kDoor));
		rkPacket.Pop(m_kTotalPercent);
	}

	int		m_iMissionNo;
	int		m_iMissionKey;
	BYTE	m_kDoor[MAX_MISSION_LEVEL];
	BYTE	m_kTotalPercent;
}SMissionInfo;

typedef enum
{
	SECTION_MISSIONAREA_CB1		= 0,
	SECTION_MISSIONAREA_CB2		= 1,
	SECTION_MISSIONAREA_MAX		= 2,
	SECTION_MISSIONAREA_ERROR	= 100,
}ESectionalMissionArea;

typedef struct tagMissionReportData
{
	tagMissionReportData()
		:	kStageFlag(0)
		,	kClearCount(0)
	{}
	BYTE kStageFlag;
	BYTE kClearCount;
}SMissionReportData;

class PgPlayer_MissionData
{
public:
	PgPlayer_MissionData();
	~PgPlayer_MissionData();// virtual금지 virtual사용하면 사이즈가 맞지 않는다!!!

	void Clear();

	bool IsPlayingLevel( size_t const iLevel )const;
	bool IsClearLevel( size_t const iLevel )const;
	bool IsClearStage( size_t const iLevel, size_t const iStage )const;

	bool IsPlayingLevel( std::vector<int> const &kMissionOpenPreLevel )const;
	bool IsClearLevel( std::vector<int> const &kMissionOpenPreLevel )const;

	size_t GetClearStageCount( size_t const iLevel )const;

	int const GetClearLevelCount(size_t const iLevel )const;

	HRESULT StageOn( size_t const iLevel, size_t const iStage );
	HRESULT StageOnForFlag( size_t const iLevel, BYTE const kStageFlag );
	
	size_t min_size()const;
	void WriteToPacket(BM::Stream& rkPacket)const;
	void ReadFromPacket(BM::Stream& rkPacket);

private:
	SMissionReportData	m_kData[MAX_MISSION_LEVEL];
	DWORD				m_dwExtData;
	// Total 16Byte
};
#pragma pack()

typedef std::vector<PgPlayer_MissionData>		CONT_PLAYER_MISSIONDATA;

class PgPlayer_MissionReport
{
public:
	typedef std::map< int, CONT_PLAYER_MISSIONDATA >		CONT_PLAYER_MISSIONREPORT;

public:
	PgPlayer_MissionReport();
	virtual ~PgPlayer_MissionReport();

	PgPlayer_MissionReport& operator=( PgPlayer_MissionReport const &rhs );

	static unsigned int GetMissionAreaSize( int const iArea );
	static unsigned int GetMissionAreaStartIndex( int const iArea );
	static bool GetMissionAreaAndIndex( unsigned int const iMissionKey, int &iOutArea, unsigned int& iOutIndex );

	void Clear();

	void WriteToPacket_MissionReport( BM::Stream& rkPacket )const;
	void ReadFromPacket_MissionReport( BM::Stream& rkPacket );

	bool Load( int const iArea, CEL::DB_DATA_ARRAY::const_iterator &itr );
	void WriteToQuery( CEL::DB_QUERY& kQuery )const;
	
	bool Update( SMissionKey const& kMissionKey, BYTE const kStageFlag );

	PgPlayer_MissionData *Get( unsigned int const iMissionKey );

protected:
	CONT_PLAYER_MISSIONREPORT		m_kContData;
};

#endif // WEAPON_VARIANT_USERINFO_PGPLAYER_MISSIONREPROT_H