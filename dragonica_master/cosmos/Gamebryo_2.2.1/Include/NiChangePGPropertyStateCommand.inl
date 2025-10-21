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
inline NiChangePGPropertyStateCommand::NiChangePGPropertyStateCommand(
    NiEntityInterface* pkEntity, 
    NiFixedString &kPGPropertyName) 
    :
    m_spEntity(pkEntity), 
    m_bOldDataValid(false)
{
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, 
        "%s \"%s\" entity", 
        kPGPropertyName, 
        (const char*)m_spEntity->GetName());
    m_kCommandName = acCommandName;
	m_kPGPropertyName = kPGPropertyName;
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiChangePGPropertyStateCommand::GetEntity() const
{
    return m_spEntity;
}
//---------------------------------------------------------------------------
inline NiFixedString NiChangePGPropertyStateCommand::GetPGProperty() const
{
	return m_kPGPropertyName;
}
