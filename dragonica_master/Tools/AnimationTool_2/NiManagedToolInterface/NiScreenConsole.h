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

// NiScreenConsole.h

#ifndef NISCREENCONSOLE_H
#define NISCREENCONSOLE_H

// Disable warning for generating native code, since AnimationTool is
// a Win32-specific application.
#pragma warning( disable : 4793 )

#include <NiRefObject.h>
#include <NiPoint2.h>
#include <NiCamera.h>
#include <NiScreenTexture.h>
#include <NiTexture.h>

namespace NiManagedToolInterface
{
#pragma unmanaged
class NiScreenConsole : public NiRefObject
{
    public:
        NiScreenConsole();
        ~NiScreenConsole();

        static void SetDefaultFontPath(const char* pcPath);
        static const char* GetDefaultFontPath();

        void Enable(bool bOn);
        bool IsEnabled() const;

        // Given the current dimensions and the current text size, this
        // is the total number of lines that can be drawn to the screen
        unsigned int GetMaxLineCount() const;
        unsigned int GetMaxLineLength() const;

        class NiConsoleFont : public NiMemObject
        {
            public:
                ~NiConsoleFont() {NiFree(
                    m_pcTextImage);}
                char* m_pcTextImage;
                unsigned int m_uiCharWidth;
                unsigned int m_uiCharHeight;
                unsigned int m_uiCharSpacingX;
                unsigned int m_uiCharSpacingY;
                unsigned int m_uiCharBaseU;
                unsigned int m_uiCharBaseV;
        };

        NiConsoleFont* GetFont()  const;
        void SetFont(NiConsoleFont* pkFont);
        
        NiConsoleFont* CreateLargeFont();
        NiConsoleFont* CreateTinyFont();
        NiConsoleFont* CreateConsoleFont();

        // Works in screen polygon coords:  <0-1, 0-1> to set the position of 
        // the lower-left edge of the text.
        NiPoint2 GetOrigin()  const;
        void SetOrigin(NiPoint2 kOrigin);
        
        // Defines the clipping region of the text in pixels. If a line will 
        // not fit within the dimensions at the current text size, it will 
        // not be rendered.
        NiPoint2 GetDimensions() const;
        void SetDimensions(NiPoint2 kDimensions);
        
        // Some applications may wish to reserve a free line number.
        // This is not enforced in the SetLine method, but if handled 
        // responsibly, this will act as a valid mechanism for holding line  
        // numbers for specific information. If a valid index cannot be  
        // generated, the function returns -1, an invalid index.
        int GrabFreeLineNumber();
        void ReleaseLineNumber(unsigned int uiLineNumber);
        unsigned int GetCurrentReservedLineCount() const;
        
        // Manipulates the given line number. Strings will be truncated
        // to fit within the dimensions of the screen.
        void SetLine(const char* pcLine, unsigned int uiWhichLine);
        const char* GetLine(unsigned int uiWhichLine) const;

        void SetCamera(NiCamera* pkCamera);
        void RecreateText();
        NiScreenTexture* GetActiveScreenTexture() const;

    protected:
        static char* ms_pcDefaultFontPath;

        char* TruncateString(const char* pcString);

        NiCameraPtr m_spCamera;
        NiConsoleFont* m_pkFont;
        NiScreenTexturePtr m_spScreenTexture;
        NiScreenTexturePtr m_spActiveScreenTexture;
        NiTexturePtr m_spTextTexture;

        unsigned int m_uiNumCurrentRows;
        unsigned int m_uiCurrentColumn;
     
        int m_iTextOriginX, m_iTextOriginY;
        int m_iTextBoxDimensionsX, m_iTextBoxDimensionsY;

        NiTPrimitiveArray<char*> m_apcText;
        NiTPrimitiveArray<bool> m_abTextIsReserved;

        bool m_bIsOn;

        static const unsigned int ms_uiASCIIMin;
        static const unsigned int ms_uiASCIIMax;
        static const unsigned int ms_uiASCIICols;
};

NiSmartPointer(NiScreenConsole);
#pragma managed
}
#endif  // #ifndef NISCREENCONSOLE_H
