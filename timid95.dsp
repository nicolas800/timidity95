# Microsoft Developer Studio Project File - Name="testgui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=testgui - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "timid95.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "timid95.mak" CFG="testgui - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "testgui - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "testgui - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "testgui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "IA_WIN95" /D "AU_WIN95" /D "__WIN32__" /D "__WIN95__" /D TIMID_VERSION= /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib winmm.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "testgui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /Gm /Gi /vd0 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "IA_WIN95" /D "AU_WIN95" /D "__WIN32__" /D "__WIN95__" /D TIMID_VERSION= /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib winmm.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "testgui - Win32 Release"
# Name "testgui - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\timidity.ico
# End Source File
# Begin Source File

SOURCE=.\win95res.h
# End Source File
# Begin Source File

SOURCE=.\win95res.rc
# End Source File
# End Group
# Begin Group "Timidity source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\celeste_e.c
# End Source File
# Begin Source File

SOURCE=.\Chorus_e.c
# End Source File
# Begin Source File

SOURCE=.\Common.c
# End Source File
# Begin Source File

SOURCE=.\Controls.c
# End Source File
# Begin Source File

SOURCE=.\dumb_c.c
# End Source File
# Begin Source File

SOURCE=.\effects.c
# End Source File
# Begin Source File

SOURCE=.\filter.c

!IF  "$(CFG)" == "testgui - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "testgui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\getopt.c
# End Source File
# Begin Source File

SOURCE=.\instrum.c
# End Source File
# Begin Source File

SOURCE=.\Mix.c
# End Source File
# Begin Source File

SOURCE=.\phaser_e.c
# End Source File
# Begin Source File

SOURCE=.\Playmidi.c
# End Source File
# Begin Source File

SOURCE=.\raw_a.c
# End Source File
# Begin Source File

SOURCE=.\Readmidi.c
# End Source File
# Begin Source File

SOURCE=.\Resample.c
# End Source File
# Begin Source File

SOURCE=.\reverb_e.c
# End Source File
# Begin Source File

SOURCE=.\Tables.c
# End Source File
# Begin Source File

SOURCE=.\timidity.c
# End Source File
# End Group
# Begin Group "Timidity header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\controls.h
# End Source File
# Begin Source File

SOURCE=.\effects.h
# End Source File
# Begin Source File

SOURCE=.\filter.h
# End Source File
# Begin Source File

SOURCE=.\instrum.h
# End Source File
# Begin Source File

SOURCE=.\mix.h
# End Source File
# Begin Source File

SOURCE=.\output.h
# End Source File
# Begin Source File

SOURCE=.\playmidi.h
# End Source File
# Begin Source File

SOURCE=.\readmidi.h
# End Source File
# Begin Source File

SOURCE=.\resample.h
# End Source File
# Begin Source File

SOURCE=.\tables.h
# End Source File
# End Group
# Begin Group "Add on source  "

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Output.c
# End Source File
# Begin Source File

SOURCE=.\wave_a.c
# End Source File
# Begin Source File

SOURCE=.\win95_a.c
# End Source File
# Begin Source File

SOURCE=.\win95_i.c
# End Source File
# Begin Source File

SOURCE=.\win95cfg.c
# End Source File
# Begin Source File

SOURCE=.\win95ctl.c
# End Source File
# Begin Source File

SOURCE=.\win95dlg.c
# End Source File
# Begin Source File

SOURCE=.\win95hlp.c
# End Source File
# End Group
# Begin Group "Add on header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win95_i.h
# End Source File
# Begin Source File

SOURCE=.\win95cfg.h
# End Source File
# Begin Source File

SOURCE=.\win95dlg.h
# End Source File
# Begin Source File

SOURCE=.\win95hlp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\README.w95
# End Source File
# End Target
# End Project
