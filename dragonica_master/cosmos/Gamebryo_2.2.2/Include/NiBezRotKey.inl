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
inline NiBezRotKey::NiBezRotKey()
{
    // for streaming, members will be filled in by LoadBinary
}
//---------------------------------------------------------------------------
inline NiBezRotKey::NiBezRotKey(float fTime, float fAngle,
    const NiPoint3& axis)
    :
    NiRotKey(fTime,fAngle,axis)
{
    // additional members filled in by FillDerivedVals
}
//---------------------------------------------------------------------------
inline NiBezRotKey::NiBezRotKey(float fTime, const NiQuaternion& quat)
    :
    NiRotKey(fTime,quat)
{
    // additional members filled in by FillDerivedVals
}
//---------------------------------------------------------------------------
