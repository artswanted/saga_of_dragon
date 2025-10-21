//---------------------------------------------------------------------------
inline NiChangePGOptimizationCommand::NiChangePGOptimizationCommand(
    NiEntityInterface* pkEntity, 
    const NiFixedString &kPGOptimization) 
    :
    m_spEntity(pkEntity), 
    m_bOldDataValid(false)
{
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, 
        "%s \"%s\" entity", 
        (const char*)kPGOptimization, 
        (const char*)m_spEntity->GetName());
    m_kCommandName = acCommandName;
	m_kPGOptimization = kPGOptimization;
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiChangePGOptimizationCommand::GetEntity() const
{
    return m_spEntity;
}
//---------------------------------------------------------------------------
inline NiFixedString NiChangePGOptimizationCommand::GetPGOptimization() const
{
	return m_kPGOptimization;
}
