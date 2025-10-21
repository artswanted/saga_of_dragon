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


//---------------------------------------------------------------------------
#ifndef NI2DBUFFER_H
#define NI2DBUFFER_H

#include "NiPixelFormat.h"
#include "NiObject.h"
#include "NiRect.h"

class NiRenderer;

class NIMAIN_ENTRY Ni2DBuffer : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    virtual ~Ni2DBuffer();

    enum CopyFilterPreference
    {
        COPY_FILTER_NONE   = 0,   // No filtering
        COPY_FILTER_POINT  = 1,   // Point filtering
        COPY_FILTER_LINEAR = 2,   // Bilinear interpolation filtering 
        COPY_FILTER_MAX
    };

    // MultiSample Anti-Aliasing Preference 
    // Note must be enumerated in order of least to greatest
    enum MultiSamplePreference
    {
        MULTISAMPLE_NONE        = 0,
        MULTISAMPLE_2           = 1,
        MULTISAMPLE_4           = 2,
        MULTISAMPLE_4_ROTATE    = 3
    };


    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

    const NiPixelFormat* GetPixelFormat() const;
    MultiSamplePreference GetMSAAPref() const;

    // Copy an arbitrary region from one 2D buffer to the specified 
    // coordinates of a different 2D buffer. Note that the NiPixelFormats
    // of both Ni2DBuffers must match for this method. This is the fastest 
    // path possible for copying pixel data. Also note that a NULL value for 
    // the NiRect will use the dimensions of the Ni2DBuffer.
    virtual bool FastCopy(Ni2DBuffer* pkDest, 
        const NiRect<unsigned int>* pkSrcRect = NULL,
        unsigned int uiDestX = 0, unsigned int uiDestY = 0) const;
    // Copy an arbitrary region from one 2D buffer to another arbitrary region
    // in a different 2D buffer. Note that the NiPixelFormats
    // of both Ni2DBuffers may differ for this method. Also note that a NULL 
    // value for either NiRect will use the dimensions of the Ni2DBuffer.
    // This path is slower than the previous FastCopy method
    virtual bool Copy(Ni2DBuffer* pkDest, 
        const NiRect<unsigned int>* pkSrcRect,
        const NiRect<unsigned int>* pkDestRect,
        Ni2DBuffer::CopyFilterPreference ePref) const;
    /*** begin Emergent internal use only ***/
    
    // Renderer specific data class
    class NIMAIN_ENTRY RendererData : public NiRefObject
    {
    public:
        virtual ~RendererData();

        unsigned int GetWidth() const;
        unsigned int GetHeight() const;
        MultiSamplePreference GetMSAAPref() const;
        const NiPixelFormat* GetPixelFormat() const;

    protected:
        RendererData();
        RendererData(Ni2DBuffer* pkBuffer);
        Ni2DBuffer* m_pkBuffer;
        const NiPixelFormat* m_pkPixelFormat;
        MultiSamplePreference m_eMSAAPref;
    };
    typedef NiPointer<RendererData> RendererDataPtr;

    // Create methods for Ni2DBuffer. These methods are Emergent internal use 
    // only. 

    // Create a 2D buffer from specified renderer data. 
    static Ni2DBuffer* Create(unsigned int uiWidth, unsigned int uiHeight,
        Ni2DBuffer::RendererData* pkData);
    // Create a 2D buffer, but do not initialize its renderer data. Before
    // this buffer can be used, its data must be set by the SetRendererData
    // method.
    static Ni2DBuffer* Create(unsigned int uiWidth, unsigned int uiHeight);

    void SetRendererData(RendererData* pkRendererData);
    RendererData* GetRendererData() const;
    
    void ResetDimensions(unsigned int uiWidth, unsigned int uiHeight);
    /*** end Emergent internal use only ***/

protected:
    Ni2DBuffer();
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;

    RendererDataPtr m_spRendererData;

    virtual bool CreateRendererData(const NiPixelFormat& pkFormat,
        Ni2DBuffer::MultiSamplePreference eMSAAPref = 
        Ni2DBuffer::MULTISAMPLE_NONE);
};

NiSmartPointer(Ni2DBuffer);


#include "Ni2DBuffer.inl"
#endif
