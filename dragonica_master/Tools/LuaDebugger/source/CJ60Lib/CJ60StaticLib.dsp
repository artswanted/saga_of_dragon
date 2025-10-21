# Microsoft Developer Studio Project File - Name="CJ60StaticLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CJ60StaticLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CJ60StaticLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CJ60StaticLib.mak" CFG="CJ60StaticLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CJ60StaticLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CJ60StaticLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/CJ60STATICLIB", JRAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CJ60StaticLib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Library\release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"StaticRelease/CJ60StaticLib.pch" /YX /Fo"StaticRelease/" /Fd"StaticRelease/" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CJ60StaticLib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Library\debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"StaticDebug/CJ60StaticLib.pch" /YX /Fo"StaticDebug/" /Fd"StaticDebug/" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Library\debug\CJ60StaticLibd.lib"

!ENDIF 

# Begin Target

# Name "CJ60StaticLib - Win32 Release"
# Name "CJ60StaticLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\CJ60Lib\CJ60Lib.rc
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJCaption.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJControlBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJDockBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJDockContext.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJExplorerBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJFlatButton.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJFlatComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJFlatHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJListView.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJMDIFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJMiniDockFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJOutlookBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJPagerCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJSearchEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJSizeDockBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJTabctrlBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CJToolBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CoolBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\CoolMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\FixTB.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\FlatBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\Globals.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\MenuBar.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\ModulVer.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\ShellPidl.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\ShellTree.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\SHFileInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\Subclass.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\CJ60Lib.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJCaption.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJControlBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJDockBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJDockContext.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJExplorerBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatButton.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatComboBox.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJListCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJListView.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMDIFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMiniDockFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJOutlookBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJPagerCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJSearchEdit.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJSizeDockBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJTabCtrlBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJToolBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CoolBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CoolMenu.h
# End Source File
# Begin Source File

SOURCE=..\Include\FixTB.h
# End Source File
# Begin Source File

SOURCE=..\Include\FlatBar.h
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\Globals.h
# End Source File
# Begin Source File

SOURCE=..\Include\hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\Include\MenuBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\ModulVer.h
# End Source File
# Begin Source File

SOURCE=..\CJ60Lib\resource.h
# End Source File
# Begin Source File

SOURCE=..\Include\ShellPidl.h
# End Source File
# Begin Source File

SOURCE=..\Include\ShellTree.h
# End Source File
# Begin Source File

SOURCE=..\Include\SHFileInfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\Subclass.h
# End Source File
# End Group
# End Target
# End Project
