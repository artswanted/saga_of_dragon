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
// NiD3DXEffectFactory inline functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline unsigned int NiD3DXEffectFactory::GetD3DXEffectCreationFlags()
{
    return ms_uiCreationFlags;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectFactory::SetD3DXEffectCreationFlags(
    unsigned int uiFlags)
{
    ms_uiCreationFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NiD3DXEffectFactory::SetFXLSupport(bool bEnable)
{
    ms_bFXLSupport = bEnable;
    if (GetInstance())
        GetInstance()->UpdateMacros();
}
//---------------------------------------------------------------------------
inline bool NiD3DXEffectFactory::GetFXLSupport()
{
    return ms_bFXLSupport;
}
//---------------------------------------------------------------------------
