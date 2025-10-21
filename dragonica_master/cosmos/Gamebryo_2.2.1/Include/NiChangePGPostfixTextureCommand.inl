//---------------------------------------------------------------------------
inline NiChangePGPostfixTextureCommand::NiChangePGPostfixTextureCommand(
    NiEntityInterface* pkEntity, 
    const NiFixedString &kPGPostfixTexture) 
    :
    m_spEntity(pkEntity), 
    m_bOldDataValid(false)
{
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, 
        "%s \"%s\" entity", 
        (const char*)kPGPostfixTexture, 
        (const char*)m_spEntity->GetName());
    m_kCommandName = acCommandName;
	m_kPGPostfixTexture = kPGPostfixTexture;
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiChangePGPostfixTextureCommand::GetEntity() const
{
    return m_spEntity;
}
//---------------------------------------------------------------------------
inline NiFixedString NiChangePGPostfixTextureCommand::GetPGPostfixTexture() const
{
	return m_kPGPostfixTexture;
}
