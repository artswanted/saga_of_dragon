//---------------------------------------------------------------------------
inline NiChangePGAlphaGroupCommand::NiChangePGAlphaGroupCommand(
    NiEntityInterface* pkEntity, 
    const NiFixedString &kPGAlphaGroup) 
    :
    m_spEntity(pkEntity), 
    m_bOldDataValid(false)
{
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, 
        "%s \"%s\" entity", 
        (const char*)kPGAlphaGroup, 
        (const char*)m_spEntity->GetName());
    m_kCommandName = acCommandName;
	m_kPGAlphaGroup = kPGAlphaGroup;
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiChangePGAlphaGroupCommand::GetEntity() const
{
    return m_spEntity;
}
//---------------------------------------------------------------------------
inline NiFixedString NiChangePGAlphaGroupCommand::GetPGAlphaGroup() const
{
	return m_kPGAlphaGroup;
}
