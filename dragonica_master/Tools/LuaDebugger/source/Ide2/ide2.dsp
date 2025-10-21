# Microsoft Developer Studio Project File - Name="ide2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ide2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ide2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ide2.mak" CFG="ide2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ide2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ide2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ide2 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\executable\Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\core" /I "..\..\include\cj60" /I "..\..\include\lua" /I "..\..\include\scintilla" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CJ60Lib.lib core.lib standard.lib shlwapi.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\executable\Release/LuaIDE.exe" /libpath:"..\..\library\release"

!ELSEIF  "$(CFG)" == "ide2 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\executable\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\core" /I "..\..\include\cj60" /I "..\..\include\lua" /I "..\..\include\scintilla" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CJ60Libd.lib core.lib standard.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\executable\Debug/LuaIDE.exe" /pdbtype:sept /libpath:"..\..\library\debug"

!ENDIF 

# Begin Target

# Name "ide2 - Win32 Release"
# Name "ide2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CallStack.cpp
# End Source File
# Begin Source File

SOURCE=.\Debugger.cpp
# End Source File
# Begin Source File

SOURCE=.\Executor.cpp
# End Source File
# Begin Source File

SOURCE=.\ide2.cpp
# End Source File
# Begin Source File

SOURCE=.\ide2.rc
# End Source File
# Begin Source File

SOURCE=.\LuaDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\MDIClientWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Project.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectNew.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\ScintillaBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ScintillaView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TreeViewFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\VariablesBar.cpp
# End Source File
# Begin Source File

SOURCE=.\WatchBar.cpp
# End Source File
# Begin Source File

SOURCE=.\WatchList.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkspaceWnd.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CallStack.h
# End Source File
# Begin Source File

SOURCE=.\Debugger.h
# End Source File
# Begin Source File

SOURCE=.\Executor.h
# End Source File
# Begin Source File

SOURCE=.\ide2.h
# End Source File
# Begin Source File

SOURCE=.\LuaDoc.h
# End Source File
# Begin Source File

SOURCE=.\LuaEditor.h
# End Source File
# Begin Source File

SOURCE=.\LuaFrame.h
# End Source File
# Begin Source File

SOURCE=.\LuaHelper.h
# End Source File
# Begin Source File

SOURCE=.\LuaView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\MDIClientWnd.h
# End Source File
# Begin Source File

SOURCE=.\OutputWnd.h
# End Source File
# Begin Source File

SOURCE=.\Project.h
# End Source File
# Begin Source File

SOURCE=.\ProjectFile.h
# End Source File
# Begin Source File

SOURCE=.\ProjectNew.h
# End Source File
# Begin Source File

SOURCE=.\ProjectProperties.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScintillaBar.h
# End Source File
# Begin Source File

SOURCE=.\ScintillaView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TreeViewFiles.h
# End Source File
# Begin Source File

SOURCE=.\VariablesBar.h
# End Source File
# Begin Source File

SOURCE=.\WatchBar.h
# End Source File
# Begin Source File

SOURCE=.\WatchList.h
# End Source File
# Begin Source File

SOURCE=.\WorkspaceWnd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ide2.ico
# End Source File
# Begin Source File

SOURCE=.\res\ide2.rc2
# End Source File
# Begin Source File

SOURCE=.\res\il_file.bmp
# End Source File
# Begin Source File

SOURCE=.\res\il_tab.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LuaDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\HelloWorld.lua
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
