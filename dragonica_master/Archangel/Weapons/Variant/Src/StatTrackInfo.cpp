#include "StdAfx.h"
#include "StatTrackInfo.h"

tagStatTrackInfo::tagStatTrackInfo()
{
	Clear();
}

void tagStatTrackInfo::Clear()
{
	m_kHasStatTrack = false;
	m_kMonsterKillCount = 0;
	m_kPlayerKillCount = 0;
}

void tagStatTrackInfo::IncKillCount(EStatTrackKillType kKillType, int iCount)
{
	switch (kKillType) {
		case STKT_PLAYER: { m_kPlayerKillCount += iCount;	}break;
		case STKT_MONSTER: {m_kMonsterKillCount += iCount;  } break;
		default: __asm int 3; // TODO: check for error
	}
}
