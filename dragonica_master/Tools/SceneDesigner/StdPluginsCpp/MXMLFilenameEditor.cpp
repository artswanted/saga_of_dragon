// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MXmlFilenameEditor.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
String* MXmlFilenameEditor::GetDialogTitle()
{
    return "Choose Xml File";
}
//---------------------------------------------------------------------------
String* MXmlFilenameEditor::GetDialogFilter()
{
    return "Xml Files (*.xml)|*.xml";
}
//---------------------------------------------------------------------------
