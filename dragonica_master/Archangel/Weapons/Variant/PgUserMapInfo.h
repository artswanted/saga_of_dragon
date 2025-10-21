#ifndef WEAPON_VARIANT_USERINFO_PGUSERMAPINFO_H
#define WEAPON_VARIANT_USERINFO_PGUSERMAPINFO_H

#include "Lohengrin/packettype.h"
#include "pgdbcache.h"

class TiXmlElement;

//#include "TableDataManager.h"
#include "PgControlDefMgr.h"

template<typename T_TYPE=DWORD>
class PgUserMapInfo
{//절대 virtual 선언 하지말것.
public:

	static size_t const MAX_BIT_SIZE = 256;
	static size_t const MAX_TYPE_SIZE = sizeof(T_TYPE)*8;

	PgUserMapInfo()
	{
		Clear();
	}

	~PgUserMapInfo()
	{}

	PgUserMapInfo<T_TYPE>& operator=( PgUserMapInfo<T_TYPE> const &rhs )
	{
		::memcpy( m_kMapInfo, rhs.m_kMapInfo, sizeof(m_kMapInfo) );
		return *this;
	}

	void Clear()
	{
		memset(m_kMapInfo,0,sizeof(m_kMapInfo));
	}

	bool On(size_t iKey)
	{
		if ( 0 == iKey || iKey > MAX_BIT_SIZE )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		--iKey;
		size_t iIndex = iKey / MAX_TYPE_SIZE;
		size_t iBit = 0x01 << (iKey % MAX_TYPE_SIZE);
		if ( !(m_kMapInfo[iIndex] & iBit) )
		{
			m_kMapInfo[iIndex] |= iBit;
			return true;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool IsOn(size_t iKey)const
	{
		if ( 0 == iKey || iKey > MAX_BIT_SIZE )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		--iKey;
		size_t iIndex = iKey / MAX_TYPE_SIZE;
		size_t iBit = iKey % MAX_TYPE_SIZE;
		return 0!=(m_kMapInfo[iIndex] & (0x01 << iBit));
	}

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		rkPacket.Push(m_kMapInfo, sizeof(m_kMapInfo));
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.PopMemory(m_kMapInfo, sizeof(m_kMapInfo));
	}

	bool LoadDB( CEL::DB_DATA_ARRAY::const_iterator itor )
	{
		return itor->PopMemory(m_kMapInfo,sizeof(m_kMapInfo));
	}

protected:
	T_TYPE		m_kMapInfo[MAX_BIT_SIZE/MAX_TYPE_SIZE];

};

#endif // WEAPON_VARIANT_USERINFO_PGUSERMAPINFO_H