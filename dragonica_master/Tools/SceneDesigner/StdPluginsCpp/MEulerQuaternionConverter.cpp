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
#include "StdPluginsCppPCH.h"

#include "MEulerQuaternionConverter.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Collections;

//---------------------------------------------------------------------------
PropertyDescriptorCollection* MEulerQuaternionConverter::GetProperties(
    ITypeDescriptorContext* pmContext, Object* pmValue, Attribute* amFilter[])
{
    if (pmValue->GetType() == __typeof(MQuaternion) &&
        pmContext->PropertyDescriptor != NULL)
    {
        MQuaternion* pmQuaternion = static_cast<MQuaternion*>(pmValue);

        ArrayList* pmPropertyDescs = new ArrayList();
        pmPropertyDescs->Add(new ElementDescriptor(pmQuaternion, 0,
            pmContext->PropertyDescriptor, "X Angle", amFilter));
        pmPropertyDescs->Add(new ElementDescriptor(pmQuaternion, 1,
            pmContext->PropertyDescriptor, "Y Angle", amFilter));
        pmPropertyDescs->Add(new ElementDescriptor(pmQuaternion, 2,
            pmContext->PropertyDescriptor, "Z Angle", amFilter));

        PropertyDescriptor* amPropertyDescArray[] =
            dynamic_cast<PropertyDescriptor*[]>(pmPropertyDescs->ToArray(
            __typeof(PropertyDescriptor)));
        return new PropertyDescriptorCollection(amPropertyDescArray);
    }

    return TypeDescriptor::GetProperties(pmValue, amFilter);
}
//---------------------------------------------------------------------------
bool MEulerQuaternionConverter::GetPropertiesSupported(
    ITypeDescriptorContext* pmContext)
{
    return true;
}
//---------------------------------------------------------------------------
bool MEulerQuaternionConverter::CanConvertTo(
    ITypeDescriptorContext* pmContext, Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(MQuaternion))
    {
        return true;
    }

    return TypeConverter::CanConvertTo(pmContext,
        pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MEulerQuaternionConverter::ConvertTo(
    ITypeDescriptorContext* pmContext, CultureInfo* pmCulture,
    Object* pmValue, Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(String) &&
        pmValue->GetType() == __typeof(MQuaternion))
    {
        MQuaternion* pmQuaternion = static_cast<MQuaternion*>(pmValue);

        // Build the string as (x, y, z).
        return String::Format("({0}, {1}, {2})",
            __box(MUtility::RadiansToDegrees(pmQuaternion->XAngle)),
            __box(MUtility::RadiansToDegrees(pmQuaternion->YAngle)),
            __box(MUtility::RadiansToDegrees(pmQuaternion->ZAngle)));
    }

    return TypeConverter::ConvertTo(pmContext, pmCulture, pmValue,
        pmDestinationType);
}
//---------------------------------------------------------------------------
bool MEulerQuaternionConverter::CanConvertFrom(
    ITypeDescriptorContext* pmContext, Type* pmType)
{
    if (pmType == __typeof(String))
    {
        return true;
    }

    return TypeConverter::CanConvertFrom(pmContext, pmType);
}
//---------------------------------------------------------------------------
Object* MEulerQuaternionConverter::ConvertFrom(
    ITypeDescriptorContext* pmContext, CultureInfo* pmInfo, Object* pmValue)
{
    if (pmValue->GetType() == __typeof(String))
    {
        try
        {
            String* strValue = static_cast<String*>(pmValue);

            // Parse the format (x, y, z).
            int iOpenParen = strValue->IndexOf('(');
            int iComma1 = strValue->IndexOf(',', iOpenParen + 1);
            int iComma2 = strValue->IndexOf(',', iComma1 + 1);
            int iCloseParen = strValue->IndexOf(')', iComma2 + 1);
            if (iOpenParen != -1 && iComma1 != -1 && iComma2 != -1 &&
                iCloseParen != -1)
            {
                // Get x.
                String* strXAngle = strValue->Substring(iOpenParen + 1,
                    iComma1 - (iOpenParen + 1));
                float fXAngle = MUtility::DegreesToRadians(Single::Parse(
                    strXAngle));

                // Get y.
                String* strYAngle = strValue->Substring(iComma1 + 1,
                    iComma2 - (iComma1 + 1));
                float fYAngle = MUtility::DegreesToRadians(Single::Parse(
                    strYAngle));

                // Get z.
                String* strZAngle = strValue->Substring(iComma2 + 1,
                    iCloseParen - (iComma2 + 1));
                float fZAngle = MUtility::DegreesToRadians(Single::Parse(
                    strZAngle));

                MQuaternion* pmQuaternion = new MQuaternion();
                pmQuaternion->FromEulerAnglesXYZ(fXAngle, fYAngle, fZAngle);
                return pmQuaternion;
            }
        }
        catch(...)
        {
            throw new ArgumentException(String::Format("Cannot convert '{0}' "
                "to type MPoint3", pmValue));
        }
    }

    return TypeConverter::ConvertFrom(pmContext, pmInfo, pmValue);
}
//---------------------------------------------------------------------------
// ElementDescriptor implementation.
//---------------------------------------------------------------------------
MEulerQuaternionConverter::ElementDescriptor::ElementDescriptor(
    MQuaternion* pmQuaternion, unsigned int uiElement,
    PropertyDescriptor* pmParentDescriptor, String* strName,
    Attribute* amAttributes[]) : PropertyDescriptor(strName, amAttributes),
    m_pmQuaternion(pmQuaternion), m_uiElement(uiElement),
    m_pmParentDescriptor(pmParentDescriptor)
{
    MAssert(m_pmQuaternion != NULL,
        "MEulterRotationTypeConverter::ElementDescriptor Error: Null "
        "MQuaternion provided to constructor.");
    MAssert(m_uiElement < 3,
        "MEulterRotationTypeConverter::ElementDescriptor Error: Element ID "
        "out of range.");
    MAssert(m_pmParentDescriptor != NULL,
        "MEulterRotationTypeConverter::ElementDescriptor Error: Null "
        "PropertyDescriptor provided to constructor.");
}
//---------------------------------------------------------------------------
Type* MEulerQuaternionConverter::ElementDescriptor::get_ComponentType()
{
    return m_pmQuaternion->GetType();
}
//---------------------------------------------------------------------------
bool MEulerQuaternionConverter::ElementDescriptor::get_IsReadOnly()
{
    return m_pmParentDescriptor->IsReadOnly;
}
//---------------------------------------------------------------------------
Type* MEulerQuaternionConverter::ElementDescriptor::get_PropertyType()
{
    return this->Element->GetType();
}
//---------------------------------------------------------------------------
bool MEulerQuaternionConverter::ElementDescriptor::CanResetValue(
    Object* pmComponent)
{
    return m_pmParentDescriptor->CanResetValue(pmComponent);
}
//---------------------------------------------------------------------------
Object* MEulerQuaternionConverter::ElementDescriptor::GetValue(
    Object* pmComponent)
{
    return this->Element;
}
//---------------------------------------------------------------------------
void MEulerQuaternionConverter::ElementDescriptor::ResetValue(
    Object* pmComponent)
{
    m_pmParentDescriptor->ResetValue(pmComponent);
}
//---------------------------------------------------------------------------
void MEulerQuaternionConverter::ElementDescriptor::SetValue(
    Object* pmComponent, Object* pmValue)
{
    this->Element = pmValue;
    m_pmParentDescriptor->SetValue(pmComponent, m_pmQuaternion);
}
//---------------------------------------------------------------------------
bool MEulerQuaternionConverter::ElementDescriptor::ShouldSerializeValue(
    Object* pmComponent)
{
    return m_pmParentDescriptor->ShouldSerializeValue(pmComponent);
}
//---------------------------------------------------------------------------
Object* MEulerQuaternionConverter::ElementDescriptor::get_Element()
{
    switch (m_uiElement)
    {
        case 0:
            return __box(MUtility::RadiansToDegrees(m_pmQuaternion->XAngle));
        case 1:
            return __box(MUtility::RadiansToDegrees(m_pmQuaternion->YAngle));
        case 2:
            return __box(MUtility::RadiansToDegrees(m_pmQuaternion->ZAngle));
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MEulerQuaternionConverter::ElementDescriptor::set_Element(
    Object* pmValue)
{
    __box float* pfValue = dynamic_cast<__box float*>(pmValue);
    MAssert(pfValue != NULL, "Element type is not correct; cannot set "
        "value.");

    switch (m_uiElement)
    {
        case 0:
            m_pmQuaternion->FromEulerAnglesXYZ(
                MUtility::DegreesToRadians(*pfValue), m_pmQuaternion->YAngle,
                m_pmQuaternion->ZAngle);
            break;
        case 1:
            m_pmQuaternion->FromEulerAnglesXYZ(m_pmQuaternion->XAngle,
                MUtility::DegreesToRadians(*pfValue), m_pmQuaternion->ZAngle);
            break;
        case 2:
            m_pmQuaternion->FromEulerAnglesXYZ(m_pmQuaternion->XAngle,
                m_pmQuaternion->YAngle, MUtility::DegreesToRadians(*pfValue));
            break;
    }
}
//---------------------------------------------------------------------------
