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
inline bool NiPhysXMeshDesc::GetIsConvex() const
{
    return m_bIsConvex;
}
//---------------------------------------------------------------------------
inline NxU32 NiPhysXMeshDesc::GetFlags() const
{
    return m_uiMeshFlags;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPhysXMeshDesc::GetName() const
{
    return m_kMeshName;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::GetData(size_t& kSize, unsigned char** ppucData)
{
    kSize = m_uiMeshSize;
    (*ppucData) = m_pucMeshData;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetIsConvex(const bool bIsConvex)
{
    m_bIsConvex = bIsConvex;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetFlags(const NxU32 uiFlags)
{
    m_uiMeshFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetName(const NiFixedString& kName)
{
    m_kMeshName = kName;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshDesc::SetData(const size_t uiSize,
    unsigned char* pucData)
{
    m_uiMeshSize = uiSize;
    m_pucMeshData = pucData;
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
