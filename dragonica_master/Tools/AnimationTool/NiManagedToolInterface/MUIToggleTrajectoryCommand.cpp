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

#include "stdafx.h"
#include "MUIToggleTrajectoryCommand.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;
#define NUM_VERTICES 400
#define CURRENT_COUNT 30
#define MIN_TIME 1.0f/60.0f
//---------------------------------------------------------------------------
MUIToggleTrajectoryCommand::MUIToggleTrajectoryCommand()
{
    m_bOn = false;
    m_uiTrajectoryIdx = -1;
    m_fLastTime = -NI_INFINITY;
    m_uiNextVertex = NUM_VERTICES - 1;
}
//---------------------------------------------------------------------------
String* MUIToggleTrajectoryCommand::GetName()
{
    return "Toggle Actor Path";
}
//---------------------------------------------------------------------------
bool MUIToggleTrajectoryCommand::Execute(MUIState* pkState)
{
    NiLines* pkLines = GetTrajectory();

    if (!pkLines)
        return false;

    if (!m_bOn)
        ResetTrajectory();

    m_bOn = !m_bOn;
    pkLines->SetAppCulled(!m_bOn);
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
bool MUIToggleTrajectoryCommand::IsToggle()
{
    return true;
}
//---------------------------------------------------------------------------
bool MUIToggleTrajectoryCommand::GetActive()
{
    return m_bOn;
}
//---------------------------------------------------------------------------
bool MUIToggleTrajectoryCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
#pragma unmanaged
// A bug in the compiler forces the create methods to live in 
// completely unmanaged code or else an InvalidProgramException
// is thrown in release mode.

NiLines* CreateTrajectory()
{
    NiBool* pkConnections = NiAlloc(NiBool, NUM_VERTICES);
    for (unsigned int ui = 0; ui < NUM_VERTICES; ui++)
        pkConnections[ui] = false;
    NiPoint3* pkVertices = NiNew NiPoint3[NUM_VERTICES];
    for (unsigned int ui = 0; ui < NUM_VERTICES; ui++)
        pkVertices[ui] = NiPoint3::ZERO;
    NiColorA* pkColors = NiNew NiColorA[NUM_VERTICES];
    for (unsigned int ui = 0; ui < NUM_VERTICES; ui++)
        pkColors[ui] = NiColorA::WHITE;


    NiLines* pkLines = NiNew NiLines(NUM_VERTICES, pkVertices, pkColors, NULL,
        0, NiGeometryData::NBT_METHOD_NONE, pkConnections);
    pkLines->GetModelData()->SetConsistency(NiGeometryData::MUTABLE);
    pkLines->SetTranslate(NiPoint3::ZERO);
    NiMatrix3 kRot;
    kRot.MakeIdentity();
    pkLines->SetRotate(kRot);
    pkLines->SetScale(1.0f);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty();
    pkMatProp->SetEmittance(NiColor::WHITE);

    NiWireframeProperty* pkWireProp = NiNew NiWireframeProperty();
    pkWireProp->SetWireframe(true);

    NiVertexColorProperty* pkVertProp = NiNew NiVertexColorProperty();
    pkVertProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVertProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);

    NiAlphaProperty* pkAlphaProp = NiNew NiAlphaProperty();
    pkAlphaProp->SetAlphaBlending(true);
    pkAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    pkAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    pkAlphaProp->SetAlphaTesting(false);

    pkLines->AttachProperty(pkAlphaProp);
    pkLines->AttachProperty(pkVertProp);
    pkLines->AttachProperty(pkWireProp);
    pkLines->AttachProperty(pkMatProp);

    pkLines->Update(0.0f);
    pkLines->UpdateProperties();
    pkLines->UpdateEffects();
    pkLines->UpdateNodeBound();
    pkLines->Update(0.0f);

    return pkLines;
}
#pragma managed
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::RefreshData()
{
    DeleteContents();
    MSharedData* pkSharedData = MSharedData::Instance;
    if (pkSharedData)
    {
        // A bug in the compiler forces the create methods to live in 
        // completely unmanaged code or else an InvalidProgramException
        // is thrown in release mode.
        pkSharedData->Lock();
        NiLines* pkLines = CreateTrajectory();
        m_uiNextVertex = 0;
        pkLines->SetAppCulled(!m_bOn);
        if (m_uiTrajectoryIdx != -1)
            pkSharedData->SetScene(m_uiTrajectoryIdx, pkLines);
        else
            m_uiTrajectoryIdx = pkSharedData->AddScene(pkLines);
        pkSharedData->Unlock();
    }
            
    AttachTrajectory();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::AttachTrajectory()
{
}
//---------------------------------------------------------------------------
NiLines* MUIToggleTrajectoryCommand::GetTrajectory()
{
    NiLines* pkLines = NULL;
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    if (m_uiTrajectoryIdx != -1)
    {
        NiAVObject* pkObj = pkSharedData->GetScene(m_uiTrajectoryIdx);
        if ( pkObj != NULL)
        {
           pkLines = NiDynamicCast(NiLines, pkObj);
        }
    }
    pkSharedData->Unlock();
    return pkLines;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIToggleTrajectoryCommand::GetCommandType()
{
    return MUICommand::TOGGLE_TRAJECTORY;
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::Update(float fTime)
{
    if (m_bOn)
    {
        if (fTime - m_fLastTime < MIN_TIME)
            return;

        MSharedData* pkSharedData = MSharedData::Instance;
        pkSharedData->Lock();
        NiActorManager* pkManager = pkSharedData->GetActorManager();
        if (pkManager != NULL)
        {
            NiAVObject* pkAccumRoot = pkManager->GetAccumRoot();
            if (pkAccumRoot)
            {
                NiPoint3 kWorldPos = pkAccumRoot->GetWorldTranslate();
                NiLines* pkLines = GetTrajectory();
                NiLinesData* pkModelData = 
                    (NiLinesData*) pkLines->GetModelData();

                if (pkModelData)
                {
                    NiPoint3* pkVerts = pkModelData->GetVertices();
                    NiColorA* pkColors = pkModelData->GetColors();
                    NiBool* pkConnects = pkModelData->GetFlags();
                    if (pkVerts)
                    {
                        pkVerts[m_uiNextVertex].x = kWorldPos.x;
                        pkVerts[m_uiNextVertex].y = kWorldPos.y;
                        pkVerts[m_uiNextVertex].z = kWorldPos.z;

                        pkConnects[m_uiNextVertex] = false;
                        
                        if (m_uiNextVertex != 0)
                        {
                            pkConnects[m_uiNextVertex-1] = true;
                        }
                        else if (m_fLastTime != -NI_INFINITY)
                        {
                            pkConnects[NUM_VERTICES - 1] = true;
                        }

                        float fLifetimeMax = 
                            (float)CURRENT_COUNT / (float)NUM_VERTICES;
                        for (unsigned int ui = 0; ui < NUM_VERTICES; ui++)
                        {
                            float fLifetime = 0.0f;
                            if (ui < m_uiNextVertex)
                                fLifetime = 
                                    (((float)(m_uiNextVertex - ui)) / 
                                     (float)NUM_VERTICES);
                            else if (ui > m_uiNextVertex)
                                fLifetime = 
                                    (((float)(m_uiNextVertex + NUM_VERTICES - 
                                        ui)) / (float)NUM_VERTICES);
                            
                            float fAlpha = 1.0f - fLifetime;
                            if (fLifetime < fLifetimeMax)
                            {
                                pkColors[ui].r = 1.0f;
                                pkColors[ui].g = 0.0f;
                                pkColors[ui].b = 0.0f;
                            }
                            else
                            {
                                pkColors[ui].r = 1.0f;
                                pkColors[ui].g = 1.0f;
                                pkColors[ui].b = 1.0f;
                            }

                            pkColors[ui].a = fAlpha;
                            
                        }

                        pkModelData->MarkAsChanged(
                            NiGeometryData::VERTEX_MASK | 
                            NiGeometryData::COLOR_MASK);
                        pkModelData->GetBound().ComputeFromData(
                            NUM_VERTICES, pkVerts);

                        pkLines->Update(0.0f);
                        pkLines->UpdateNodeBound();
                        m_fLastTime = fTime;
                        
                        if (m_uiNextVertex == NUM_VERTICES - 1)
                            m_uiNextVertex = 0;
                        else
                            m_uiNextVertex++;

                    }
                }
            }
        }
        pkSharedData->Unlock();
    }
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::ResetTrajectory()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    NiActorManager* pkManager = pkSharedData->GetActorManager();
    if (pkManager != NULL)
    {
        NiAVObject* pkAccumRoot = pkManager->GetAccumRoot();
        if (pkAccumRoot)
        {
            NiPoint3 kWorldPos = pkAccumRoot->GetWorldTranslate();
            NiLines* pkLines = GetTrajectory();
            NiLinesData* pkModelData = (NiLinesData*) pkLines->GetModelData();

            if (pkModelData)
            {
                NiPoint3* pkVerts = pkModelData->GetVertices();
                NiColorA* pkColors = pkModelData->GetColors();
                NiBool* pkConnects = pkModelData->GetFlags();
                for (unsigned int ui = 0; ui < NUM_VERTICES; ui++)
                    pkConnects[ui] = false;
                for (unsigned int ui = 0; ui < NUM_VERTICES; ui++)
                    pkVerts[ui] = NiPoint3::ZERO;
                for (unsigned int ui = 0; ui < NUM_VERTICES; ui++)
                    pkColors[ui] = NiColorA::WHITE;

                pkModelData->MarkAsChanged(
                    NiGeometryData::VERTEX_MASK | NiGeometryData::COLOR_MASK);
                pkModelData->GetBound().ComputeFromData(NUM_VERTICES, pkVerts);

                pkLines->Update(0.0f);
                pkLines->UpdateNodeBound();
                m_fLastTime = -NI_INFINITY;
            }
            m_uiNextVertex = 0;        
        }
    }
    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
