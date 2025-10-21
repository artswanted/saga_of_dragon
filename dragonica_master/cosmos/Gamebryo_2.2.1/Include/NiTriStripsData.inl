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
inline unsigned short NiTriStripsData::GetStripCount() const
{
    return m_usStrips;
}

//---------------------------------------------------------------------------
inline unsigned short* NiTriStripsData::GetStripLengths() const
{
    return m_pusStripLengths;
}

//---------------------------------------------------------------------------
inline unsigned short* NiTriStripsData::GetStripLists() const
{
    return m_pusStripLists;
}

//---------------------------------------------------------------------------
inline unsigned short NiTriStripsData::GetStripLengthSum() const
{
    return m_usTriangles + 2 * m_usStrips;
}

