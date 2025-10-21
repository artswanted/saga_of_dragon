#pragma once

#include <NiMain.H>
#include "NiAnimationEventSet.H"


class	NIANIMATION_ENTRY	NiAnimationEventSetMgr	:	public	NiObject
{
	typedef std::map<unsigned int, NiAnimationEventSetPtr> AnimationEventSetWithSeqIDMap;
	typedef std::map<DWORD, AnimationEventSetWithSeqIDMap*> AnimationEventSetWithEventTypeMap;

public:

	~NiAnimationEventSetMgr()	{	ReleaseAll();	}

	bool	AddAnimationEventSet(DWORD kEventType,unsigned int uiSeqID,NiAnimationEventSet	*pkEventSet)
	{
		AnimationEventSetWithSeqIDMap	*pkMap = NULL;
		AnimationEventSetWithEventTypeMap::iterator itor = m_kAnimationEventSetCont.find(kEventType);
		if(itor == m_kAnimationEventSetCont.end())
		{
			pkMap = new AnimationEventSetWithSeqIDMap();
			m_kAnimationEventSetCont.insert(std::make_pair(kEventType,pkMap));
		}
		else
		{
			pkMap = itor->second;
		}

		pkMap->insert(std::make_pair(uiSeqID,pkEventSet));

		return	true;
	}
	NiAnimationEventSet*	GetAnimationEventSet(DWORD kEventType,unsigned int uiSeqID) const
	{
		NiAnimationEventSet	*pkEventSet = NULL;
		
		AnimationEventSetWithSeqIDMap	*pkMap = NULL;
		AnimationEventSetWithEventTypeMap::const_iterator itor = m_kAnimationEventSetCont.find(kEventType);
		if(itor == m_kAnimationEventSetCont.end())
		{
			return	NULL;
		}

		pkMap = itor->second;

		AnimationEventSetWithSeqIDMap::iterator itor2 = pkMap->find(uiSeqID);
		if(itor2 == pkMap->end())
		{
			return	NULL;
		}

		pkEventSet = itor2->second;

		return	pkEventSet;
	}

	void	SaveFile(NiFile &kFile,unsigned int uiSeqID);
	void	LoadFile(NiFile &kFile,unsigned int uiSeqID,unsigned int uiVersion);


	void	ReleaseAll()
	{
		for(AnimationEventSetWithEventTypeMap::iterator itor = m_kAnimationEventSetCont.begin();
			itor != m_kAnimationEventSetCont.end();
			++itor)
		{
			AnimationEventSetWithSeqIDMap	*pkMap = itor->second;
			delete pkMap;
		}
		m_kAnimationEventSetCont.clear();
	}

private:

	unsigned	int	GetNumEventType(unsigned int uiSeqID);

private:

	AnimationEventSetWithEventTypeMap	m_kAnimationEventSetCont;


};

NiSmartPointer(NiAnimationEventSetMgr);
