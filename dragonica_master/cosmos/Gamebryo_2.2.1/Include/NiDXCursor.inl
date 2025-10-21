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
// NiDXCursor inline functions
//---------------------------------------------------------------------------
inline NiDXCursor::NiDXCursor(NiRenderer* pkRenderer) :
    NiCursor(pkRenderer), 
    m_pkD3DSurface(0)
{
    assert(m_spRenderer);

    NiD3DRenderer* pkD3DRenderer = (NiD3DRenderer*)pkRenderer;
    m_pkD3DDevice = pkD3DRenderer->GetD3DDevice();
    D3D_POINTER_REFERENCE(m_pkD3DDevice);
}
//---------------------------------------------------------------------------
