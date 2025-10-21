// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NIDXCURSOR_H
#define NIDXCURSOR_H

#include "NiCursor.h"
#include "NiCursor_D3DHeaders.h"

class NICURSOR_ENTRY NiDXCursor : public NiCursor
{
    NiDeclareRTTI;

public:
     // Static creation call - to allow for failing...
   static NiCursor* Create(NiRenderer* pkRenderer, 
        NiRect<int>& kRect, unsigned int uiFlags, 
        unsigned int uiHotSpotX, unsigned int uiHotSpotY, 
        const char* pacImageFile);
	
	//	Constructors
    NiDXCursor(NiRenderer* pkRenderer);

	//	Destructor
	virtual ~NiDXCursor();

    // Run-time functions
    virtual int Show(bool bShow);
    virtual bool SetPosition(float fTime, int iScreenSpaceX, 
        int iScreenSpaceY);
    virtual bool Move(float fTime, int iDeltaX, int iDeltaY);
    virtual void Draw();

    //*** begin - Gamebryo internal use only
    virtual bool SetImage(const char* pacImageFile);

    virtual bool WriteImageData(NiBinaryStream& kStream);
    virtual bool ReadImageData(NiBinaryStream& kStream);
    //*** end - Gamebryo internal use only

protected:
    D3DDevicePtr m_pkD3DDevice;
    D3DSurfacePtr m_pkD3DSurface;
};

typedef NiPointer<NiDXCursor> NiDXCursorPtr;

#include "NiDXCursor.inl"

#endif //#ifndef NIDXCURSOR_H
