#ifndef WEAPON_XUI_XUI_RESOURCE_MANAGER_H
#define WEAPON_XUI_XUI_RESOURCE_MANAGER_H

#include <map>
#include <assert.h>
#include "BM/Point.h"

class CXUI_Resource_Manager_Base
{
public:
	typedef std::wstring T_KEY;
	typedef void 		T_MAPPED;
	typedef std::map< T_KEY, T_MAPPED* > SPRITE_HASH;

public:
	CXUI_Resource_Manager_Base(){}
	virtual ~CXUI_Resource_Manager_Base(){}

	virtual bool VPrepareResource(const T_KEY& rKey) = 0;
	virtual T_MAPPED* VCreateResource(const T_KEY &rKey) = 0;
	virtual T_MAPPED* VCreateIconResource(int const IconNo, RECT rectIcon,bool bGrayScale = false) = 0;
	virtual bool VReleaseRsc(SPRITE_HASH::mapped_type &pRsc) = 0;
	virtual T_MAPPED* VCreateOffscreen(POINT2 const& ptSize, int staticNum, DWORD bgColor, float fAlpha) = 0;
	virtual bool VReleaseOffscreen(SPRITE_HASH::mapped_type &pRsc) = 0;

	virtual SPRITE_HASH::mapped_type AddRsc(const T_KEY &rKey)
	{
		SPRITE_HASH::mapped_type pRsc = VCreateResource(rKey);
		if( pRsc )
		{
			m_mapRsc.insert( std::make_pair(rKey, pRsc) );
			return pRsc;
		}
		assert( NULL && "Cant' Create Resource");
		return NULL;
	}

	virtual SPRITE_HASH::mapped_type GetRsc(const T_KEY &rKey)
	{
		return VCreateResource(rKey);
		//게임브리오 되면서 바뀜.
/*		SPRITE_HASH::iterator itor = m_mapRsc.find(rKey);
		if( m_mapRsc.end() == itor )
		{
			return AddRsc(rKey);
		}
		return (*itor).second;
*/
	}

	virtual bool ReleaseRsc(SPRITE_HASH::mapped_type &pRsc)
	{
		VReleaseRsc(pRsc);
		pRsc = NULL;
		return true;
	}
	

protected:
	SPRITE_HASH m_mapRsc;

};

#endif // WEAPON_XUI_XUI_RESOURCE_MANAGER_H