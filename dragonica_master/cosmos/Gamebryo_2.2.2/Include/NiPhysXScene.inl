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
inline NxScene* NiPhysXScene::GetPhysXScene() const
{
    if (m_pkMasterScene)
    {
        return m_pkMasterScene->GetPhysXScene();
    }
    else
    {
        return m_pkPhysXScene;
    }
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetSlaved() const
{
    return (m_pkMasterScene != 0);
}
//---------------------------------------------------------------------------
inline NxMat34 NiPhysXScene::GetSlavedXform() const
{
    return m_kSlavedXform;
}
//---------------------------------------------------------------------------
inline const NiTransform& NiPhysXScene::GetSceneXform() const
{
    if (m_pkMasterScene)
    {
        return m_pkMasterScene->GetSceneXform();
    }
    else
    {
        return m_kSceneXform;
    }
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetSceneXform(const NiTransform& kXform)
{
    if (m_pkMasterScene)
    {
        m_pkMasterScene->SetSceneXform(kXform);
    }
    else
    {
        m_kSceneXform = kXform;
    }
}
//---------------------------------------------------------------------------
inline float NiPhysXScene::GetScaleFactor() const
{
    return m_fScalePToW;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetScaleFactor(const float fScale)
{
    m_fScalePToW = fScale;
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetUpdateSrc(void) const
{
    return GetBit(UPDATE_SRC_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetUpdateSrc(const bool bActive)
{
    SetBit(bActive, UPDATE_SRC_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetUpdateDest(void) const
{
    return GetBit(UPDATE_DEST_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetUpdateDest(const bool bActive)
{
    SetBit(bActive, UPDATE_DEST_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetDoFixedStep(void) const
{
    return GetBit(FIXED_TIMESTEP_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetDoFixedStep(const bool bActive)
{
    SetBit(bActive, FIXED_TIMESTEP_MASK);
}
//---------------------------------------------------------------------------
inline float NiPhysXScene::GetTimestep(void) const
{
    return m_fTimestep;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetTimestep(const float fStep)
{
    m_fTimestep = fStep;
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetDebugRender(void) const
{
    return GetBit(DEBUG_RENDER_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetInSimFetch() const
{
    return m_bInSimFetch;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetFetchTime(const float fCurrentTime)
{
    m_fPrevFetchTime = fCurrentTime;
    m_fNextFetchTime = fCurrentTime;
}
//---------------------------------------------------------------------------
inline float NiPhysXScene::GetPrevFetchTime() const
{
    return m_fPrevFetchTime;
}
//---------------------------------------------------------------------------
inline float NiPhysXScene::GetNextFetchTime() const
{
    return m_fNextFetchTime;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetPrevFetchTime(const float fTime)
{
    m_fPrevFetchTime = fTime;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetNextFetchTime(const float fTime)
{
    m_fNextFetchTime = fTime;
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXScene::GetSourcesCount() const
{
    return m_kSources.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXScene::AddSource(NiPhysXSrc* pkSrc)
{
    return m_kSources.AddFirstEmpty(pkSrc);
}
//---------------------------------------------------------------------------
inline NiPhysXSrc* NiPhysXScene::GetSourceAt(const unsigned int uiIndex)
{
    return m_kSources.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::DeleteSource(NiPhysXSrc* pkTarget)
{
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
        if (m_kSources.GetAt(i) == pkTarget)
        {
            m_kSources.RemoveAtAndFill(i);
            return true;
        }
        
    return false;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::DeleteSourceAt(const unsigned int uiIndex)   
{
    NiPhysXSrc* pkSrc = m_kSources.RemoveAtAndFill(uiIndex);
    assert(pkSrc);
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXScene::GetDestinationsCount() const
{
    return m_kDestinations.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXScene::AddDestination(NiPhysXDest* pkDest)
{
    return m_kDestinations.AddFirstEmpty(pkDest);
}
//---------------------------------------------------------------------------
inline NiPhysXDest* NiPhysXScene::GetDestinationAt(const unsigned int uiIndex)
{
    return m_kDestinations.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::DeleteDestination(NiPhysXDest* pkTarget)
{
    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
        if (m_kDestinations.GetAt(i) == pkTarget)
        {
            m_kDestinations.RemoveAtAndFill(i);
            return true;
        }
        
    return false;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::DeleteDestinationAt(const unsigned int uiIndex) 
{
    NiPhysXDest* pkDest = m_kDestinations.RemoveAtAndFill(uiIndex);
    assert(pkDest);
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::IsSlave(const NiPhysXScene* pkScene) const
{
    for (unsigned int ui = 0; ui < m_kSlaves.GetSize(); ui++)
    {
        if (pkScene == m_kSlaves.GetAt(ui))
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::AddSlave(NiPhysXScene* pkScene)
{
    m_kSlaves.AddFirstEmpty(pkScene);
    pkScene->m_pkMasterScene = this;
}
//---------------------------------------------------------------------------
inline NiPhysXSceneDesc* NiPhysXScene::GetSnapshot()
{
    return m_spSnapshot;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetSnapshot(NiPhysXSceneDesc* pkSnapshot)
{
    m_spSnapshot = pkSnapshot;
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::GetKeepMeshes() const
{
    return m_bKeepMeshes;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::SetKeepMeshes(const bool bVal)
{
    m_bKeepMeshes = bVal;
}
//---------------------------------------------------------------------------
inline void NiPhysXScene::ReleaseSnapshot()
{
    m_spSnapshot = 0;
}
//---------------------------------------------------------------------------
inline bool NiPhysXScene::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
