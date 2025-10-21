#pragma once
/*
#include <map>
#include "BM/GUID.h"
#include "BM/ClassSupport.h"
#include "BM/STLSupport.h"
#include "BM/ObjectPool.h"
#include "Loki/singleton.h"
#include "lohengrin/PacketStruct.h"
#include "lohengrin/PgChannel.h"
#include "Variant/pgtotalobjectmgr.h"
#include "Variant/PgPlayer.h"
#include "Variant/PgPet.h"
#include "Variant/PgMission.h"

// RULL
// 1. Primary Field(ё¶А»)Ає 0x00000001,0x00000002,0x00000003ЅДАё·О Бх°Ў
// 2. Secondary Field(ЗКµе)ґВ 0x00000100,0x00000200,0x00000300,0x00000001,0x00000101,0x00000201 ... ЅДАё·О Бх°Ў

class PgLChannelMgr
{
public:
	PgLChannelMgr()
		:	m_iDefaultChannelCount(1)//Default ChannelјцґВ ГЦјТ ЗС°і
		,	m_iMaxChannelCount(1)
		,	m_iMaxUserCountForChannel(100)
		,	m_iMaxUserCount(100)
		,	m_iNowUserCount(0)
		,	m_iIndunUserCount(0)//АОґшАЇАъ јэАЪ
	{};
	virtual ~PgLChannelMgr(){};

public:
	static bool IsField(SGroundKey const &kInKey);

	HRESULT RegistChannel(int const iLChannelNo);
	HRESULT UnRegistChannel(int const iLChannelNo);
	HRESULT GetMovableChannel(int const iLChannelNo,int const iCount);
	HRESULT AddUserCount(int const iLChannelNo,int const iCount);
	HRESULT GetAutoLChannel(SGroundKey& rkGndKey,int const iCount=1);
	
	CLASS_DECLARATION(int,m_iDefaultChannelCount,DefaultChannelCount)
	CLASS_DECLARATION(int,m_iMaxChannelCount,MaxChannelCount)
	CLASS_DECLARATION(int,m_iMaxUserCountForChannel,MaxUserCountForChannel)
	CLASS_DECLARATION(int,m_iMaxUserCount,MaxUserCount)
	CLASS_DECLARATION_NO_SET(int,m_iNowUserCount,NowUserCount)
	CLASS_DECLARATION(int,m_iIndunUserCount,IndunUserCount)
protected:
	Loki::Mutex			m_kMutex;
	// ёК ё®јТЅє ±вБШАё·О °ЎБш ј­№ц іС№цё¦ °ьё®.
	// №МјЗёКАє °ьё®±в°Ў ЗПБц ѕКАЅ.-> NullGUID ёё °ьё®.
	ContChannelState	m_kTotalState;
	void AddTotalUserCount(int const iAddCount);
};

inline bool PgLChannelMgr::IsField(SGroundKey const &kInKey)
{
	if(kInKey.Guid() == BM::GUID::NullData())
	{
		return true;
	}

	return false;
}

inline void PgLChannelMgr::AddTotalUserCount(int const iAddCount)
{
	m_iNowUserCount += iAddCount;
	if ( m_iNowUserCount < 0 )
	{
		assert(false);
		m_iNowUserCount = 0;
	}
}

#define g_kLChannelMgr SINGLETON_STATIC(PgLChannelMgr)
*/