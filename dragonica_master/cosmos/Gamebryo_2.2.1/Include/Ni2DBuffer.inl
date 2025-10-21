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

#include "Ni2DBuffer.h"

//---------------------------------------------------------------------------
inline void Ni2DBuffer::SetRendererData(RendererData* pkRendererData)
{
    m_spRendererData = pkRendererData;
}
//---------------------------------------------------------------------------
inline unsigned int Ni2DBuffer::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int Ni2DBuffer::GetHeight() const
{
    return m_uiHeight;
}
//---------------------------------------------------------------------------
inline unsigned int Ni2DBuffer::RendererData::GetWidth() const
{
    return m_pkBuffer->m_uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int Ni2DBuffer::RendererData::GetHeight() const
{
    return m_pkBuffer->m_uiHeight;
}
//---------------------------------------------------------------------------
inline const NiPixelFormat* Ni2DBuffer::RendererData::GetPixelFormat() const
{
    return m_pkPixelFormat;
}
//---------------------------------------------------------------------------
inline const NiPixelFormat* Ni2DBuffer::GetPixelFormat() const
{
    if (m_spRendererData)
        return m_spRendererData->GetPixelFormat();
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline Ni2DBuffer::MultiSamplePreference Ni2DBuffer::GetMSAAPref() const
{
    if (m_spRendererData)
        return m_spRendererData->GetMSAAPref();
    else
        return MULTISAMPLE_NONE;
}
//---------------------------------------------------------------------------
inline Ni2DBuffer::MultiSamplePreference Ni2DBuffer::
RendererData::GetMSAAPref() const
{
    return m_eMSAAPref;
}
//---------------------------------------------------------------------------
inline Ni2DBuffer::RendererData* Ni2DBuffer::GetRendererData() const
{
    return m_spRendererData;
}
//---------------------------------------------------------------------------
inline Ni2DBuffer::RendererData::RendererData() : m_pkBuffer(NULL), 
    m_pkPixelFormat(NULL)
{
}
//---------------------------------------------------------------------------
inline Ni2DBuffer::RendererData::RendererData(Ni2DBuffer* pkBuffer) : 
    m_pkBuffer(pkBuffer), m_pkPixelFormat(NULL)
{
}
//---------------------------------------------------------------------------
inline Ni2DBuffer::RendererData::~RendererData()
{
    /* */
}
//---------------------------------------------------------------------------

