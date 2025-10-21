; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ide2.h"
LastPage=0

ClassCount=17
Class1=CIdeApp
Class2=CAboutDlg
Class3=CLuaDoc
Class4=CLuaEditor
Class5=CLuaFrame
Class6=CLuaView
Class7=CMainFrame
Class8=CMDIClientWnd
Class9=CProjectProperties
Class10=CScintillaView
Class11=CTreeViewFiles

ResourceCount=12
Resource1=IDD_PROJECT_PROPERTIES
Resource2=IDR_ACCEL_DEBUG_BREAK (English (U.S.))
Resource3=IDR_IDE2TYPE (English (U.S.))
Resource4=IDR_MAINFRAME (English (U.S.))
Resource5=IDR_PROJECT_MENU
Resource6=IDR_ACCEL_DEBUG (English (U.S.))
Resource7=IDD_NEW_PROJECT
Resource8=IDD_ABOUTBOX (English (U.S.))
Resource9=IDR_SUBMENU_DEBUG
Class12=CDebuggerThread
Class13=CVariablesBar
Class14=CWatchList
Class15=CWatchBar
Resource10=IDR_ACCEL_BUILD (English (U.S.))
Class16=aaaaa
Resource11=IDR_ACCEL_NO_PROJECT (English (U.S.))
Class17=CProjectNew
Resource12=IDR_SUBMENU_BUILD

[CLS:CIdeApp]
Type=0
BaseClass=CWinApp
HeaderFile=ide2.h
ImplementationFile=ide2.cpp
LastObject=CIdeApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=ide2.cpp
ImplementationFile=ide2.cpp

[CLS:CLuaDoc]
Type=0
BaseClass=CDocument
HeaderFile=LuaDoc.h
ImplementationFile=LuaDoc.cpp
LastObject=CLuaDoc

[CLS:CLuaEditor]
Type=0
BaseClass=CWnd
HeaderFile=LuaEditor.h
ImplementationFile=LuaEditor.cpp
LastObject=CLuaEditor
Filter=W
VirtualFilter=WC

[CLS:CLuaFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=LuaFrame.h
ImplementationFile=LuaFrame.cpp
Filter=M
VirtualFilter=mfWC
LastObject=CLuaFrame

[CLS:CLuaView]
Type=0
BaseClass=CView
HeaderFile=LuaView.h
ImplementationFile=LuaView.cpp
LastObject=CLuaView
Filter=C
VirtualFilter=VWC

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrame.h
ImplementationFile=MainFrame.cpp
Filter=T
VirtualFilter=fWC
LastObject=ID_HELP_LUAHOMEPAGE

[CLS:CMDIClientWnd]
Type=0
BaseClass=CWnd
HeaderFile=MDIClientWnd.h
ImplementationFile=MDIClientWnd.cpp
LastObject=ID_FILE_OPENPROJECT

[CLS:CProjectProperties]
Type=0
BaseClass=CDialog
HeaderFile=ProjectProperties.h
ImplementationFile=ProjectProperties.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_GENERATE_LISTING

[CLS:CScintillaView]
Type=0
BaseClass=CView
HeaderFile=ScintillaView.h
ImplementationFile=ScintillaView.cpp

[CLS:CTreeViewFiles]
Type=0
BaseClass=CTreeView
HeaderFile=TreeViewFiles.h
ImplementationFile=TreeViewFiles.cpp
Filter=C
VirtualFilter=VWC
LastObject=CTreeViewFiles

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_PROJECT_PROPERTIES]
Type=1
Class=CProjectProperties
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_OUTPUT_DIR,edit,1350631552
Control5=IDC_OUTPUT_PREFIX,edit,1350631552
Control6=IDC_GENERATE_LISTING,button,1342242819
Control7=IDC_STATIC,static,1342308352
Control8=IDC_INTERMEDIATE_DIR,edit,1350631552
Control9=IDCANCEL,button,1342242816

[MNU:IDR_SUBMENU_BUILD]
Type=1
Class=?
Command1=ID_BUILD_COMPILE
Command2=ID_BUILD_BUILD
Command3=ID_BUILD_REBUILDALL
Command4=ID_BUILD_CLEAN
Command5=ID_BUILD_GO
Command6=ID_BUILD_STEPINTO
Command7=ID_BUILD_RUNTOCURSOR
Command8=ID_BUILD_EXECUTE
CommandCount=8

[MNU:IDR_SUBMENU_DEBUG]
Type=1
Class=?
Command1=ID_DEBUG_GO
Command2=ID_DEBUG_RESTART
Command3=ID_DEBUG_STOPDEBUGGING
Command4=ID_DEBUG_BREAK
Command5=ID_DEBUG_STEPINTO
Command6=ID_DEBUG_STEPOVER
Command7=ID_DEBUG_STEPOUT
Command8=ID_DEBUG_RUNTOCURSOR
CommandCount=8

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_NEWPROJECT
Command4=ID_FILE_OPENPROJECT
Command5=ID_FILE_SAVEPROJECT
Command6=ID_FILE_SAVEPROJECTAS
Command7=ID_FILE_CLOSEPROJECT
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_VIEW_WORKSPACE
Command14=ID_VIEW_OUTPUT
Command15=ID_VIEW_CALLSTACK
Command16=ID_VIEW_LOCALS
Command17=ID_VIEW_WATCHES
Command18=ID_HELP_LUAHELPPDF
Command19=ID_HELP_LUAHOMEPAGE
Command20=ID_HELP_VISITHOMEPAGE
Command21=ID_HELP_CONTACTAUTHOR
Command22=ID_APP_ABOUT
CommandCount=22

[MNU:IDR_IDE2TYPE (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_NEWPROJECT
Command7=ID_FILE_OPENPROJECT
Command8=ID_FILE_SAVEPROJECT
Command9=ID_FILE_SAVEPROJECTAS
Command10=ID_FILE_CLOSEPROJECT
Command11=ID_FILE_PRINT
Command12=ID_FILE_PRINT_PREVIEW
Command13=ID_FILE_PRINT_SETUP
Command14=ID_FILE_MRU_FILE1
Command15=ID_APP_EXIT
Command16=ID_EDIT_UNDO
Command17=ID_EDIT_REDO
Command18=ID_EDIT_CUT
Command19=ID_EDIT_COPY
Command20=ID_EDIT_PASTE
Command21=ID_EDIT_CLEAR
Command22=ID_EDIT_SELECT_ALL
Command23=ID_TOGGLE_BREAKPOINT
Command24=ID_VIEW_TOOLBAR
Command25=ID_VIEW_STATUS_BAR
Command26=ID_VIEW_WORKSPACE
Command27=ID_VIEW_OUTPUT
Command28=ID_VIEW_CALLSTACK
Command29=ID_VIEW_LOCALS
Command30=ID_VIEW_WATCHES
Command31=ID_WINDOW_CASCADE
Command32=ID_WINDOW_TILE_HORZ
Command33=ID_WINDOW_ARRANGE
Command34=ID_HELP_LUAHELPPDF
Command35=ID_HELP_LUAHOMEPAGE
Command36=ID_HELP_VISITHOMEPAGE
Command37=ID_HELP_CONTACTAUTHOR
Command38=ID_APP_ABOUT
CommandCount=38

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_SELECT_ALL
Command2=ID_EDIT_COPY
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_PRINT
Command6=ID_FILE_SAVE
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_BUILD_RUNTOCURSOR
Command11=ID_BUILD_STEPINTO
Command12=ID_BUILD_GO
Command13=ID_BUILD_EXECUTE
Command14=ID_NEXT_PANE
Command15=ID_PREV_PANE
Command16=ID_BUILD_BUILD
Command17=ID_BUILD_COMPILE
Command18=ID_PROJECT_PROPERTIES
Command19=ID_EDIT_COPY
Command20=ID_EDIT_PASTE
Command21=ID_EDIT_CUT
Command22=ID_EDIT_REDO
Command23=ID_EDIT_UNDO
CommandCount=23

[ACL:IDR_ACCEL_BUILD (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_SELECT_ALL
Command2=ID_EDIT_COPY
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_PRINT
Command6=ID_FILE_SAVE
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_BUILD_RUNTOCURSOR
Command11=ID_BUILD_STEPINTO
Command12=ID_BUILD_GO
Command13=ID_BUILD_EXECUTE
Command14=ID_NEXT_PANE
Command15=ID_PREV_PANE
Command16=ID_BUILD_BUILD
Command17=ID_BUILD_COMPILE
Command18=ID_PROJECT_PROPERTIES
Command19=ID_TOGGLE_BREAKPOINT
Command20=ID_EDIT_COPY
Command21=ID_EDIT_PASTE
Command22=ID_EDIT_CUT
Command23=ID_EDIT_REDO
Command24=ID_EDIT_UNDO
CommandCount=24

[ACL:IDR_ACCEL_DEBUG (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_SELECT_ALL
Command2=ID_EDIT_COPY
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_PRINT
Command6=ID_FILE_SAVE
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_DEBUG_STOPDEBUGGING
Command11=ID_DEBUG_RESTART
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
Command14=ID_PROJECT_PROPERTIES
Command15=ID_TOGGLE_BREAKPOINT
Command16=ID_EDIT_COPY
Command17=ID_EDIT_PASTE
Command18=ID_EDIT_CUT
Command19=ID_EDIT_REDO
Command20=ID_EDIT_UNDO
CommandCount=20

[ACL:IDR_ACCEL_DEBUG_BREAK (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_SELECT_ALL
Command2=ID_EDIT_COPY
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_PRINT
Command6=ID_FILE_SAVE
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_DEBUG_STEPOVER
Command11=ID_DEBUG_RUNTOCURSOR
Command12=ID_DEBUG_STEPINTO
Command13=ID_DEBUG_STEPOUT
Command14=ID_DEBUG_GO
Command15=ID_DEBUG_STOPDEBUGGING
Command16=ID_DEBUG_RESTART
Command17=ID_NEXT_PANE
Command18=ID_PREV_PANE
Command19=ID_PROJECT_PROPERTIES
Command20=ID_TOGGLE_BREAKPOINT
Command21=ID_EDIT_COPY
Command22=ID_EDIT_PASTE
Command23=ID_EDIT_CUT
Command24=ID_EDIT_REDO
Command25=ID_EDIT_UNDO
CommandCount=25

[CLS:CDebuggerThread]
Type=0
HeaderFile=DebuggerThread.h
ImplementationFile=DebuggerThread.cpp
BaseClass=CWinThread
Filter=N
VirtualFilter=TC
LastObject=ID_TTTTTEST

[CLS:CVariablesBar]
Type=0
HeaderFile=VariablesBar.h
ImplementationFile=VariablesBar.cpp
BaseClass=CCJControlBar

[CLS:CWatchList]
Type=0
HeaderFile=WatchList.h
ImplementationFile=WatchList.cpp
BaseClass=CCJListCtrl
Filter=W
LastObject=CWatchList
VirtualFilter=FWC

[CLS:CWatchBar]
Type=0
HeaderFile=WatchBar.h
ImplementationFile=WatchBar.cpp
BaseClass=CCJControlBar
LastObject=IDC_LIST1

[CLS:aaaaa]
Type=0
HeaderFile=aaaaa.h
ImplementationFile=aaaaa.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=aaaaa

[MNU:IDR_PROJECT_MENU]
Type=1
Class=?
Command1=ID_PROJECT_ADD_FILES
Command2=ID_PROJECT_PROPERTIES
CommandCount=2

[ACL:IDR_ACCEL_NO_PROJECT (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_SELECT_ALL
Command2=ID_EDIT_COPY
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_PRINT
Command6=ID_FILE_SAVE
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_NEXT_PANE
Command11=ID_PREV_PANE
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_EDIT_CUT
Command15=ID_EDIT_REDO
Command16=ID_EDIT_UNDO
CommandCount=16

[DLG:IDD_NEW_PROJECT]
Type=1
Class=CProjectNew
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_PROJECT_TYPES,SysListView32,1350680591
Control4=IDC_STATIC,static,1342308352
Control5=IDC_PROJECT_NAME,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_PROJECT_LOCATION,edit,1350631552
Control8=IDC_PROJECT_SELLOC,button,1342242816

[CLS:CProjectNew]
Type=0
HeaderFile=ProjectNew.h
ImplementationFile=ProjectNew.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CProjectNew

