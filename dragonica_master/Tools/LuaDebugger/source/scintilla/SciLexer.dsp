# Microsoft Developer Studio Project File - Name="SciLexer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SciLexer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SciLexer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SciLexer.mak" CFG="SciLexer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SciLexer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SciLexer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SciLexer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\executable\Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SciLexer_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /O2 /I "." /I "..\..\include\scintilla" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCI_LEXER" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib comctl32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "SciLexer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\executable\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SciLexer_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /ZI /Od /I "." /I "..\..\include\scintilla" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SCI_LEXER" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib comctl32.lib /nologo /verbose /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "SciLexer - Win32 Release"
# Name "SciLexer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=AutoComplete.cxx
# End Source File
# Begin Source File

SOURCE=CallTip.cxx
# End Source File
# Begin Source File

SOURCE=CellBuffer.cxx
# End Source File
# Begin Source File

SOURCE=ContractionState.cxx
# End Source File
# Begin Source File

SOURCE=Document.cxx
# End Source File
# Begin Source File

SOURCE=DocumentAccessor.cxx
# End Source File
# Begin Source File

SOURCE=Editor.cxx
# End Source File
# Begin Source File

SOURCE=win32\ExternalLexer.cxx
# End Source File
# Begin Source File

SOURCE=Indicator.cxx
# End Source File
# Begin Source File

SOURCE=KeyMap.cxx
# End Source File
# Begin Source File

SOURCE=KeyWords.cxx
# End Source File
# Begin Source File

SOURCE=LexAda.cxx
# End Source File
# Begin Source File

SOURCE=LexAVE.cxx
# End Source File
# Begin Source File

SOURCE=LexBaan.cxx
# End Source File
# Begin Source File

SOURCE=LexBullant.cxx
# End Source File
# Begin Source File

SOURCE=LexConf.cxx
# End Source File
# Begin Source File

SOURCE=LexCPP.cxx
# End Source File
# Begin Source File

SOURCE=LexCrontab.cxx
# End Source File
# Begin Source File

SOURCE=LexEiffel.cxx
# End Source File
# Begin Source File

SOURCE=LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=LexLisp.cxx
# End Source File
# Begin Source File

SOURCE=LexLua.cxx
# End Source File
# Begin Source File

SOURCE=LexMatlab.cxx
# End Source File
# Begin Source File

SOURCE=LexOthers.cxx
# End Source File
# Begin Source File

SOURCE=LexPascal.cxx
# End Source File
# Begin Source File

SOURCE=LexPerl.cxx
# End Source File
# Begin Source File

SOURCE=LexPython.cxx
# End Source File
# Begin Source File

SOURCE=LexRuby.cxx
# End Source File
# Begin Source File

SOURCE=LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=LexVB.cxx
# End Source File
# Begin Source File

SOURCE=LineMarker.cxx
# End Source File
# Begin Source File

SOURCE=win32\PlatWin.cxx
# End Source File
# Begin Source File

SOURCE=PropSet.cxx
# End Source File
# Begin Source File

SOURCE=RESearch.cxx
# End Source File
# Begin Source File

SOURCE=ScintillaBase.cxx
# End Source File
# Begin Source File

SOURCE=win32\ScintillaWin.cxx
# End Source File
# Begin Source File

SOURCE=win32\ScintRes.rc
# End Source File
# Begin Source File

SOURCE=Style.cxx
# End Source File
# Begin Source File

SOURCE=StyleContext.cxx
# End Source File
# Begin Source File

SOURCE=UniConversion.cxx
# End Source File
# Begin Source File

SOURCE=ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=WindowAccessor.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=Platform.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
