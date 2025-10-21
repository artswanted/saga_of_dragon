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
inline NiGeneralComponentProperty::NiGeneralComponentProperty(
    const NiFixedString& kPropertyName, const NiFixedString& kDisplayName,
    const NiFixedString& kPrimitiveType, const NiFixedString& kSemanticType,
    const NiFixedString& kDescription, bool bIsCollection) : 
    m_kPropertyName(kPropertyName), m_kDisplayName(kDisplayName), 
    m_kPrimitiveType(kPrimitiveType), m_kSemanticType(kSemanticType), 
    m_kDescription(kDescription), m_bIsCollection(bIsCollection),
    m_stDataSizeInBytes(0)
{
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetPropertyName()
    const
{
    return m_kPropertyName;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetPropertyName(
    const NiFixedString& kPropertyName)
{
    m_kPropertyName = kPropertyName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetDisplayName() const
{
    return m_kDisplayName;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetDisplayName(
    const NiFixedString& kDisplayName)
{
    m_kDisplayName = kDisplayName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetPrimitiveType()
    const
{
    return m_kPrimitiveType;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetPrimitiveType(
    const NiFixedString& kPrimitiveType)
{
    m_kPrimitiveType = kPrimitiveType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetSemanticType()
    const
{
    return m_kSemanticType;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetSemanticType(
    const NiFixedString& kSemanticType)
{
    m_kSemanticType = kSemanticType;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiGeneralComponentProperty::GetDescription()
    const
{
    return m_kDescription;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetDescription(
    const NiFixedString& kDescription)
{
    m_kDescription = kDescription;
}
//---------------------------------------------------------------------------
inline NiBool NiGeneralComponentProperty::GetIsCollection() const
{
    return m_bIsCollection;
}
//---------------------------------------------------------------------------
inline void NiGeneralComponentProperty::SetIsCollection(bool bIsCollection)
{
    m_bIsCollection = bIsCollection;
}
//---------------------------------------------------------------------------
inline unsigned int NiGeneralComponentProperty::GetSize() const
{
    if (m_bIsCollection)
    {
        return m_kDataPointers.GetSize();
    }
    else
    {
        return 1;
    }

}
//---------------------------------------------------------------------------
template<class T>
inline void NiGeneralComponentProperty::SetData(T* pvDataArray,
    size_t stArrayCount, unsigned int uiIndex)
{

    //Note, the following call contains an index/"isCollection" checking assert
    //so one is ommitted in this function.

    void* pvData = NULL;
    if (uiIndex < m_kDataPointers.GetSize())
    {
        pvData = GetData(uiIndex);
    }

    if (pvData)
    {
        NiFree(pvData);
        m_kDataPointers.SetAt(uiIndex, NULL);
        //m_pvData = NULL;
        if (!m_bIsCollection)
        {
            m_stDataSizeInBytes = 0;
        }
    }

    if (pvDataArray)
    {
        pvData = NiAlloc(T, stArrayCount);
        //m_pvData = NiAlloc(T, stArrayCount);
        m_stDataSizeInBytes = stArrayCount * sizeof(pvDataArray[0]);
        NiMemcpy(pvData, m_stDataSizeInBytes, pvDataArray,
            m_stDataSizeInBytes);
        m_kDataPointers.SetAtGrow(uiIndex, pvData);
    }
}
//---------------------------------------------------------------------------
inline void* NiGeneralComponentProperty::GetData(unsigned int uiIndex) const
{
    assert(m_bIsCollection || uiIndex == 0);
    if (uiIndex < m_kDataPointers.GetSize())
    {
        return m_kDataPointers.GetAt(uiIndex);
    }
    else
    {
        return NULL;
    }
    //return m_pvData;
}
//---------------------------------------------------------------------------
inline size_t NiGeneralComponentProperty::GetDataSizeInBytes() const
{
    return m_stDataSizeInBytes;
}
//---------------------------------------------------------------------------
inline NiGeneralComponentProperty* NiGeneralComponentProperty::Clone()
{
    NiGeneralComponentProperty* pkClone = NiNew NiGeneralComponentProperty(
        m_kPropertyName, m_kDisplayName, m_kPrimitiveType, m_kSemanticType,
        m_kDescription, m_bIsCollection);

    unsigned int uiSize = m_kDataPointers.GetSize();

    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        pkClone->SetData((char*)m_kDataPointers.GetAt(ui), m_stDataSizeInBytes,
            ui);
    }

    //pkClone->SetData((char*) m_pvData, m_stDataSizeInBytes);

    return pkClone;
}
//---------------------------------------------------------------------------
