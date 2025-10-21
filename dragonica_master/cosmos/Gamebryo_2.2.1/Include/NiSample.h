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

#ifndef NISAMPLE_H
#define NISAMPLE_H

// NiApplication framework
#include <NiApplication.h>
#include <NiCursor.h>
#include <NiUISlot.h>
#include <NiUILabel.h>
#include <NiUIRenderGroup.h>
#include <NiNavFlyController.h>
#include <NiNavOrbitController.h>
#include <NiShaderFactory.h>
#include <NiString.h>

class NiUIGroup;

class NiSample : public NiApplication
{
public:
    NiSample
        (
        const char* pcWindowCaption,       // caption of window
        unsigned int uiWidth = 1024,       // client window width
        unsigned int uiHeight = 768,       // client window height
        unsigned int uiMenuID = 0,         // menu resource ID
        unsigned int uiNumStatusPanes = 0, // number of panes in status window
        unsigned int uiBitDepth = 0        // kColor depth of screen
        );

    //virtual ~NiSample();

protected:
    
    virtual bool Initialize();
    virtual bool CreateInputSystem();
    virtual bool CreateCursor();
    virtual bool CreateUISystem();
    virtual bool CreateNavigationControllers();
    virtual bool AdjustNavigationControllers(
        NiAVObject* pkControlledObject,
        NiAVObject* pkReferenceObject,
        const NiPoint3& kUpDir,
        float fScale);
    virtual bool CreateUIElements();
    virtual bool CompleteUISystem();
    virtual bool CreateShaderSystem();

    // NiShader Functions
    virtual bool RunShaderParsers(const char* pcShaderDir);
    virtual bool RunShaderLibraries(const char* pcShaderDir);
    virtual bool RegisterShaderParsers();
    virtual bool RegisterShaderLibraries();
#if !defined(_USRDLL)
    static bool LibraryClassCreate(const char* pcLibFile, 
        NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
        bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
    static unsigned int RunParser(const char* pcLibFile, 
        NiRenderer* pkRenderer, const char* pcDirectory, 
        bool bRecurseSubFolders);
#endif
    static unsigned int ShaderErrorCallback(const char* pcError, 
        NiShaderError eError, bool bRecoverable);

    virtual void RenderFrame();
    virtual void RenderUIElements();

    // Destroy the font and user strings.
    virtual void Terminate();

    virtual void UpdateFrame();
    virtual void EndUpdate();

    NiNavOrbitControllerPtr m_spOrbitController;
    NiNavFlyControllerPtr m_spFlyController;

    NiUIMemberSlot1<NiSample, unsigned char> m_kHideAll;
    NiUIMemberSlot0<NiSample> m_kToggleNavHelp;
    NiUIMemberSlot0<NiSample> m_kChangeController;
    NiUIMemberSlot0<NiSample> m_kQuit;

    void ToggleNavHelp();
    virtual void ChangeController();
    void HideAllPressed(unsigned char ucState);
    virtual void UpdateControllerLog(NiNavBaseController* pkNewController);

    // Layout controls
    unsigned int m_uiMaxOutputLogLines;
    float m_fUIElementHeight;
    float m_fUIElementWidth;
    float m_fUIGroupHeaderHeight;
    NiPoint2 m_kUINavHelpStart;
    NiPoint2 m_kUIElementGroupOffset;

    void AddDefaultUIElements(NiUIGroup* pkGroup, 
        float& fLeftOffset, float& fTopOffset, float fElementWidth, 
        float fElementHeight);

    void AddNavigationHelpUIElements(float fLeftOffset, float fTopOffset, 
        float fElementWidth, float fElementHeight,
        float fImageWidth, float fMinLabelWidth);

    unsigned int AddLogEntry(const char* pcNewLine);
    bool ChangeLogEntry(unsigned int uiLineNum, const char* pcNewLine);

    // Log elements
    void ReinitializeLogText();
    NiUILabelPtr m_spOutputLog;
    NiTPrimitiveArray<NiString*> m_kLogLines;
    unsigned int m_uiNumLogChars;
    unsigned int m_uiNavControllerLine;
    NiUILabelPtr m_spFrameRateLabel;

    // Navigation Help elements
    bool m_bShowNavHelp;
    NiTObjectArray<NiUIRenderGroupPtr> m_kNavHelpRenderGroups;

#if defined(WIN32)
    NiCursorPtr m_spCursor;
    virtual bool OnDefault(NiEventRef pEventRecord);
    static unsigned int NiSampleMessageBoxFunc(const char* pcText, 
        const char* pcCaption, void* pvExtraData = NULL);
#endif
    bool m_bUseNavSystem;
    NiPoint3 m_kNavUpAxis;
    float m_fNavDefaultScale;

#if defined(_USRDLL)
    NiTObjectArray<NiString> m_kShaderLibraries;
    NiTObjectArray<NiString> m_kParserLibraries;
#else
    NiTPrimitiveArray<NiShaderFactory::NISHADERLIBRARY_CLASSCREATIONCALLBACK> 
        m_kShaderLibraries;
    NiTPrimitiveArray<NiShaderFactory::NISHADERLIBRARY_RUNPARSERCALLBACK>
        m_kParserLibraries;
#endif
    unsigned int m_uiActiveCallbackIdx;
};

#endif
