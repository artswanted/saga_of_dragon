#pragma once

#define MAX_STAT_TRACK_ABIL 5
#pragma pack(1)

typedef enum tagStatTrackKillType: unsigned short
{
	STKT_PLAYER = 1,
	STKT_MONSTER = 2,
}	EStatTrackKillType;

typedef struct tagStatTrackInfo
{
public:
	tagStatTrackInfo();
public:
	CLASS_DECLARATION_S(bool, HasStatTrack);
	CLASS_DECLARATION_S(int, PlayerKillCount);
	CLASS_DECLARATION_S(int, MonsterKillCount);

	void Clear();
	void IncKillCount(EStatTrackKillType kKillType, int iCount = 1);
} SStatTrackInfo;
#pragma  pack()