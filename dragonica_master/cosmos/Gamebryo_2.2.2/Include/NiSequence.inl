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
inline const char* NiSequence::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline unsigned int NiSequence::GetCount() const
{
    return m_kObjectName.GetSize();
}
//---------------------------------------------------------------------------
inline const char* NiSequence::GetObjectName(unsigned int uiIndex)
{
    return m_kObjectName.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiTransformController* NiSequence::GetController(unsigned int uiIndex)
{
    return m_kCtrl.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiTextKeyExtraData* NiSequence::GetTextKeys() const
{
    return m_spTextKeys;
}
//---------------------------------------------------------------------------
inline void NiSequence::SetName(const char* pcName)
{
    NiFree(m_pcName);
    m_pcName = NULL;
    if (pcName)
    {
        unsigned int uiLen = strlen(pcName) + 1;
        m_pcName = NiAlloc(char, uiLen);
        NiStrcpy(m_pcName, uiLen, pcName);
    }
}
//---------------------------------------------------------------------------
inline void NiSequence::SetTextKeyReference(unsigned int uiReference)
{
    m_uiTextKeyReference = uiReference;
}
//---------------------------------------------------------------------------
inline void NiSequence::SetTextKeys(NiTextKeyExtraData* pkData)
{
    m_spTextKeys = pkData;
}
//---------------------------------------------------------------------------
