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
inline int NiScreenElementsData::GetMaxPQuantity() const
{
    return (int)m_usMaxPQuantity;
}
//---------------------------------------------------------------------------
inline void NiScreenElementsData::SetPGrowBy(int iPGrowBy)
{
    m_usPGrowBy = (unsigned short)(iPGrowBy > 0 ? iPGrowBy : 1);
}
//---------------------------------------------------------------------------
inline int NiScreenElementsData::GetPGrowBy() const
{
    return (int)m_usPGrowBy;
}
//---------------------------------------------------------------------------
inline int NiScreenElementsData::GetMaxVQuantity() const
{
    return (int)m_usMaxVQuantity;
}
//---------------------------------------------------------------------------
inline void NiScreenElementsData::SetVGrowBy(int iVGrowBy)
{
    m_usVGrowBy = (unsigned short)(iVGrowBy > 0 ? iVGrowBy : 1);
}
//---------------------------------------------------------------------------
inline int NiScreenElementsData::GetVGrowBy() const
{
    return (int)m_usVGrowBy;
}
//---------------------------------------------------------------------------
inline int NiScreenElementsData::GetMaxTQuantity() const
{
    return (int)m_usMaxIQuantity/3;
}
//---------------------------------------------------------------------------
inline void NiScreenElementsData::SetTGrowBy(int iTGrowBy)
{
    m_usIGrowBy = (unsigned short)(iTGrowBy > 0 ? 3 * iTGrowBy : 3);
}
//---------------------------------------------------------------------------
inline int NiScreenElementsData::GetTGrowBy() const
{
    return (int)m_usIGrowBy/3;
}
//---------------------------------------------------------------------------
inline int NiScreenElementsData::GetNumPolygons() const
{
    return (int)m_usPQuantity;
}
//---------------------------------------------------------------------------
inline bool NiScreenElementsData::BoundNeedsUpdate() const
{
    return m_bBoundNeedsUpdate;
}
//---------------------------------------------------------------------------
inline NiScreenElementsData::Polygon& NiScreenElementsData::GetPolygon(
    int iPolygon)
{
    return m_akPolygon[m_ausPIndexer[iPolygon]];
}
//---------------------------------------------------------------------------
inline const NiScreenElementsData::Polygon& NiScreenElementsData::GetPolygon(
    int iPolygon) const
{
    return m_akPolygon[m_ausPIndexer[iPolygon]];
}
//---------------------------------------------------------------------------
