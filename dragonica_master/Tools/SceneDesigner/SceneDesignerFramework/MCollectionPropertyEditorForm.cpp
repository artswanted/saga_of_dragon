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

// Precompiled Header
#include "SceneDesignerFrameworkPCH.h"
#include "MCollectionPropertyEditorForm.h"
#include <windows.h>

using namespace Emergent::Gamebryo::SceneDesigner::Framework;


MCollectionPropertyEditorForm::MCollectionPropertyEditorForm()
{
    InitializeComponent();
}
//---------------------------------------------------------------------------
Object* MCollectionPropertyEditorForm::get_EditValue()[]
{
    return m_pmEditValue;
}
//---------------------------------------------------------------------------
void MCollectionPropertyEditorForm::set_EditValue(Object* pmValue[])
{
    m_pmEditValue = pmValue;
    m_pmctlNumberOfItems->Value = pmValue->Count;
}
//---------------------------------------------------------------------------
void MCollectionPropertyEditorForm::OnOk_Click(System::Object*  sender,
    System::EventArgs *  e)
{
    int iNewCount = (int)m_pmctlNumberOfItems->Value;
    int iOldCount = m_pmEditValue->Count;

    Object* pmNewList[] = new Object*[iNewCount];
    for (int iIndex = 0; iIndex < iNewCount; iIndex++)
    {
        if (iIndex < iOldCount)
        {
            pmNewList[iIndex] = m_pmEditValue[iIndex];
        }
    }
    m_pmEditValue = pmNewList;
}
//---------------------------------------------------------------------------
