#pragma once

#include "MFilenameEditor.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MXmlFilenameEditor : public MFilenameEditor
    {
    protected:
        virtual String* GetDialogTitle();
        virtual String* GetDialogFilter();
    };
}}}}