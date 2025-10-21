#pragma once

#include <NiMain.H>

#include "NiAnimationEvent.H"

class	NiActorManager;
class	NiControllerSequence;

class	NIANIMATION_ENTRY	NiAnimationEventSet	:	public	NiObject
{
public:

	NiAnimationEventSet(DWORD kEventType)
		:m_kEventType(kEventType)
	{
	};
	virtual	~NiAnimationEventSet()
	{
		for(unsigned int ui=0;ui<m_kAnimationEventCont.GetSize();++ui)
		{
			delete m_kAnimationEventCont.GetAt(ui);
		}
		m_kAnimationEventCont.RemoveAll();
	};
	
	unsigned	int	GetSize()	const	{	return	m_kAnimationEventCont.GetSize();	}
	NiAnimationEvent*	GetAt(unsigned int uiPos)	const	{	return	m_kAnimationEventCont.GetAt(uiPos);	}

	void	Add(NiAnimationEvent *pkEvent)
	{
		m_kAnimationEventCont.Add(pkEvent);
	}

	bool	RemoveAnimationEvent(NiTextKey *pkTextKey);
	void	Merge(NiAnimationEventSet *pkEventSet)
	{
		if(!pkEventSet)
		{
			return;
		}
		if(pkEventSet->GetSize() == 0 || pkEventSet->GetEventType() != GetEventType())
		{
			return;
		}
		for(unsigned int ui=0;ui<pkEventSet->GetSize();++ui)
		{
			Add(pkEventSet->GetAt(ui));
		}
		pkEventSet->ClearAllEvents();
	}

	bool	SaveFile(NiFile &kFile);
	static	NiAnimationEventSet*	LoadFile(NiFile &kFile,unsigned int uiVersion);
	static	NiAnimationEventSet*	LoadFile(NiFile &kFile,DWORD kEventType,NiAnimationEventSet *pkAnimationEventSet,unsigned int uiVersion);

	void	RegisterCallback(NiActorManager *pkActorManager,NiControllerSequence *pkSequence,unsigned int eSequenceID);
	void	RefreshCallback(NiActorManager *pkActorManager,unsigned int eSequenceID);

	DWORD	GetEventType()	const	{	return	m_kEventType;	}

private:

	static	NiAnimationEvent*	CreateAnimationEvent(DWORD kEventType);

	void	ClearAllEvents()
	{
		m_kAnimationEventCont.RemoveAll();
	}

private:

	typedef	NiTPrimitiveArray<NiAnimationEvent*>	AnimationEventArray;

	AnimationEventArray	m_kAnimationEventCont;
	DWORD	m_kEventType;
};

NiSmartPointer(NiAnimationEventSet);