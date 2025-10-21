#pragma once

#include <NiMain.H>
#include <NiAnimationLibType.H>

class	NIANIMATION_ENTRY	NiAnimationEvent	:	public	NiObject
{
	NiDeclareRTTI;

public:


	NiAnimationEvent()
		:m_pkTextKey(NULL)
	{
	};
	NiAnimationEvent(NiTextKey *pkTextKey)
		:m_pkTextKey(pkTextKey)
	{
	};

	virtual	~NiAnimationEvent()
	{
		m_pkTextKey = NULL;
	};

	NiTextKey*	GetTextKey()	{	return	m_pkTextKey;	}
	void	SetTextKey(NiTextKey *pkTextKey)	{	m_pkTextKey = pkTextKey;	}

	virtual	DWORD GetEventType() const	= 0;

	virtual	void	SaveFile(NiFile &kFile)
	{
	}
	virtual	void	LoadFile(NiFile &kFile,unsigned int uiVersion)
	{
	}

protected:

	void	SaveTextKey(NiFile &kFile);
	void	LoadTextKey(NiFile &kFile);

private:

	NiTextKey	*m_pkTextKey;

};

NiSmartPointer(NiAnimationEvent);