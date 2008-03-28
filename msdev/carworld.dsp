# Microsoft Developer Studio Project File - Name="carworld" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=carworld - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "carworld.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "carworld.mak" CFG="carworld - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "carworld - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "carworld - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "carworld - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\obj"
# PROP Intermediate_Dir "..\..\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_WIN_SOCKETS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib ws2_32.lib SDL.lib SDLmain.lib /nologo /subsystem:windows /machine:I386 /out:"../../carworld.exe"

!ELSEIF  "$(CFG)" == "carworld - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\obj"
# PROP Intermediate_Dir "..\..\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_WIN_SOCKETS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib ws2_32.lib SDL.lib SDLmain.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../carworld.exe" /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "carworld - Win32 Release"
# Name "carworld - Win32 Debug"
# Begin Group "SDL"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\DXJoystick.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DXJoystick.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Main.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_PosixSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_WinSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\SDLJoystick.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\SDLJoystick.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SDLMain.cpp
# End Source File
# End Group
# Begin Group "Generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\automat.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Graphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Graphics.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Image.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Input.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Input.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Keys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Keys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Object.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Object.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Prompt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Prompt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Socket.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Standard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Standard.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Texture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Texture.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Variable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_Variable.h
# End Source File
# Begin Source File

SOURCE=..\..\src\H_XML.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\H_XML.h
# End Source File
# End Group
# Begin Group "CarWorld specific"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\CarWorld.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CarWorld.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CarWorldClasses.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CarWorldClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CarWorldClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CarWorldClient.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CarWorldMain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CarWorldNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CarWorldNet.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CWCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CWCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CWVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\CWVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\CWVersion.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Tacho.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Tacho.h
# End Source File
# Begin Source File

SOURCE=..\..\src\WorldBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\WorldBlock.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\res\carworld.rc
# End Source File
# Begin Source File

SOURCE=..\..\src\res\carworld.rc2
# End Source File
# Begin Source File

SOURCE=..\..\src\res\resource.h
# End Source File
# End Group
# End Target
# End Project
