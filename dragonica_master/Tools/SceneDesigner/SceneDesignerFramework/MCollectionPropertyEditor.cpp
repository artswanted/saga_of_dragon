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

#include "MCollectionPropertyEditor.h"
#include "MCollectionPropertyEditorForm.h"
#include "MEntityPropertyDescriptor.h"
#include "ServiceProvider.h"
#include "MComponent.h"

using namespace System::Collections;
using namespace System::ComponentModel;
using namespace System::ComponentModel::Design;
using namespace System::Diagnostics;
using namespace System::Reflection;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;


//---------------------------------------------------------------------------
MCollectionPropertyEditor::MCollectionPropertyEditor(Type* pmCollectionType,
    Type* pmItemType) : CollectionEditor(pmCollectionType)
{
    m_pmItemType = pmItemType;
}
//---------------------------------------------------------------------------
Type* MCollectionPropertyEditor::CreateCollectionItemType()
{
    return m_pmItemType;
}
//---------------------------------------------------------------------------
Object* MCollectionPropertyEditor::CreateInstance(System::Type* type)
{
    return __super::CreateInstance(type);
}
//---------------------------------------------------------------------------
Object* MCollectionPropertyEditor::EditValue(ITypeDescriptorContext* pmContext,
    System::IServiceProvider* sp, Object* pmValue)
{
    if (pmContext != NULL && pmContext->PropertyDescriptor != NULL &&
        __typeof(MEntityPropertyDescriptor)->IsAssignableFrom(
        pmContext->PropertyDescriptor->GetType()))
    {
        //SetContext(pmContext);
        MEntityPropertyDescriptor* pmDescriptor =
            static_cast<MEntityPropertyDescriptor*> 
            (pmContext->PropertyDescriptor);

        Debug::Assert(pmDescriptor->PropertyContainer->HasProperty(
            pmDescriptor->PropertyName), "Entity does not contain " 
            "property!");

        unsigned int uiCount = 
            pmDescriptor->PropertyContainer->GetElementCount(
            pmDescriptor->PropertyName);
        ArrayList* pmValues = new ArrayList();
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            pmValues->Add(               
                pmDescriptor->PropertyContainer->GetPropertyData(
                pmDescriptor->PropertyName, ui) );
        }
        MCollectionPropertyEditorForm* pmDialog = 
            new MCollectionPropertyEditorForm();
        pmDialog->EditValue = 
            static_cast<Object*[]>(pmValues->ToArray(__typeof(Object)));

        if (pmDialog->ShowDialog() == DialogResult::OK)
        {
            ICommandService* pmCommandService = 
                MGetService(ICommandService);
            pmCommandService->BeginUndoFrame(String::Format(
                "Change affected entities for \"{0}{1}",
                pmDescriptor->PropertyContainer->Name , S"\" light"));

            Object* pmNewData[] = GetItems(pmDialog->EditValue);
            unsigned int uiNewCount = pmNewData->Count;
            //Clear out existing values
            MEntity* pmEntity = 
                dynamic_cast<MEntity*>(pmDescriptor->PropertyContainer);
            
            MComponent* pmComponent = NULL;

            if (pmEntity != NULL)
            {
                pmComponent = GetComponentContainingProperty(
                    pmEntity, pmDescriptor->Name);
            }

            if (pmComponent != NULL && 
                pmComponent->IsAddOrRemovePropertySupported())
            {
                String* strPropertyName = pmDescriptor->PropertyName;
                String* strDisplayName = pmComponent->GetPropertyDisplayName(
                    strPropertyName);
                PropertyType* pmPropertyType = 
                    pmComponent->GetPropertyType(strPropertyName);
                String* strPrimativeType = pmPropertyType->PrimitiveType;
                String* strSemanticType = pmPropertyType->Name;
                String* strDescription = pmDescriptor->Description;


                if (pmComponent->IsPropertyUnique(strPropertyName))
                {

                    if (pmComponent->CanResetProperty(strPropertyName))
                    {
                        pmComponent->ResetProperty(strPropertyName);
                    }
                    else
                    {
                        pmComponent->RemoveProperty(strPropertyName, true);
                    }
                }

                pmComponent->AddProperty(strPropertyName, strDisplayName, 
                    strPrimativeType, strSemanticType, true, strDescription,
                    true);

                for ( unsigned int ui = 0; ui < uiNewCount; ui++)
                {
                    pmComponent->SetPropertyData(
                        pmDescriptor->PropertyName, 
                        pmNewData->get_Item(ui), ui, true);
                }
            }
            else
            {
                for ( unsigned int ui = 0; ui < uiCount; ui++)
                {
                    pmDescriptor->PropertyContainer->SetPropertyData(
                        pmDescriptor->PropertyName, NULL, ui, true);
                }
                for ( unsigned int ui = 0; ui < uiNewCount; ui++)
                {
                    pmDescriptor->PropertyContainer->SetPropertyData(
                        pmDescriptor->PropertyName, 
                        pmNewData->get_Item(ui), ui, true);
                }
            }

            pmCommandService->EndUndoFrame(true);
        }
    }
    return pmValue;
}
//---------------------------------------------------------------------------
MComponent* MCollectionPropertyEditor::GetComponentContainingProperty(
    MEntity* pmEntity, String* strPropertyName)
{
    MComponent* amComponents[] = pmEntity->GetComponents();
    unsigned int uiSize = amComponents->Count;
    for ( unsigned int uiIndex = 0; uiIndex < uiSize; uiIndex++)
    {
        MComponent* pmComponent = amComponents[uiIndex];
        if (pmComponent->HasProperty(strPropertyName))
        {
            return pmComponent;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
