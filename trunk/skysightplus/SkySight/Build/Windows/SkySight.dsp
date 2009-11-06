# Microsoft Developer Studio Project File - Name="SkySight" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SkySight - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SkySight.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SkySight.mak" CFG="SkySight - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SkySight - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SkySight - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "skysightplus"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SkySight - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /W3 /GX /O2 /I "..\..\..\GUILib\Windows" /I "..\..\..\GUILib\JPEGLib" /I "..\..\..\GUILib\GIFLib" /I "..\..\..\GUILib\LibTIFF" /I "..\..\..\AstroLib" /I "..\..\ParLib" /I "..\..\Cookbook" /I "..\..\SX\Windows" /I "..\..\SBIG\Windows" /I "..\..\Source" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x809 /i "..\..\..\..\GUILib\Win" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib setupapi.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"..\..\..\..\SBIG\Drivers\Windows\Parallel"

!ELSEIF  "$(CFG)" == "SkySight - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /W3 /Gm /GX /ZI /Od /I "..\..\..\GUILib\Windows" /I "..\..\..\GUILib\JPEGLib" /I "..\..\..\GUILib\GIFLib" /I "..\..\..\GUILib\LibTIFF" /I "..\..\..\AstroLib" /I "..\..\ParLib" /I "..\..\Cookbook" /I "..\..\SX\Windows" /I "..\..\SBIG\Windows" /I "..\..\Source" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\GUILib\Win" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\SBIG\Drivers\Windows\Parallel"

!ENDIF 

# Begin Target

# Name "SkySight - Win32 Release"
# Name "SkySight - Win32 Debug"
# Begin Group "AstroLib sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\AstroLib\Angle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\Astrom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\CoordSys.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\FITS.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\GSC.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\JupiMoon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\MarsMoon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\MathEx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\Matrix.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\NeptMoon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\Orbit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\Physical.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\Reduce.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\SatuMoon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\Sbig.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\Time.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\UranMoon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\AstroLib\VFPlanet.c
# End Source File
# End Group
# Begin Group "SkySight sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Source\AnalyzeMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\AppleMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Camera.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\CameraDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\CameraExposureDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\CameraSetupDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\CameraTemperatureDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\CameraTypes.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\CookbookCameraInterface.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\DemoCameraInterface.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\DisplayMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\EditMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Exposure.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\FileMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Files.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\HeaderWindow.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\HelpMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\HistogramWindow.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Image.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ImageAnalysisDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ImageDisplay.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ImageDisplayDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ImageHistogram.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ImageObject.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ImageProcessing.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ImageRegion.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ImageWindow.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Main.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Menus.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\MultipleExposureDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\ProcessMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\SBIGParallelCameraInterface.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\SXUSBCameraInterface.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\Utilities.c
# End Source File
# Begin Source File

SOURCE=..\..\Source\WindowMenu.c
# End Source File
# End Group
# Begin Group "GUILib sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GCommunications.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GControls.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GDialogs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GEvents.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GFiles.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GGeometry.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GGraphics.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GImage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GMain.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GMemory.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GMouse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GPrinting.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GScreen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GSound.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\Windows\GWindows.c
# End Source File
# End Group
# Begin Group "ParLib sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ParLib\ParLib.c
# End Source File
# End Group
# Begin Group "JPEGLib sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\cdjpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jccolor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jchuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcinit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcparam.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jcsample.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jctrans.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdatadst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jerror.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jmemansi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\jutils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\rdbmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\rdcolmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\rdgif.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\rdppm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\rdrle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\rdswitch.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\rdtarga.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\wrbmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\wrgif.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\wrppm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\wrrle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\JPEGLib\wrtarga.c
# End Source File
# End Group
# Begin Group "LibTIFF sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_aux.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_close.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_codec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_compress.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_dir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_dirinfo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_dirread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_dirwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_dumpmode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_error.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_fax3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_fax3sm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_flush.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_getimage.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_jpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_luv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_lzw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_next.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_open.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_packbits.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_pixarlog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_predict.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_print.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_read.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_strip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_swab.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_thunder.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_tile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_version.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_warning.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_win32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_write.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\LibTIFF\tif_zip.c
# End Source File
# End Group
# Begin Group "GIFLib sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\GUILib\GIFLib\dgif_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\GIFLib\egif_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\GIFLib\gif_err.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\GIFLib\gif_font.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\GIFLib\gif_hash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\GUILib\GIFLib\gifalloc.c
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Resource\Windows\SkySight.rc
# End Source File
# End Group
# Begin Group "Cookbook sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Cookbook\CB245.c
# End Source File
# Begin Source File

SOURCE=..\..\Cookbook\Cookbook.c
# End Source File
# End Group
# Begin Group "SBIG sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Sbig\Windows\Sbig32m.lib
# End Source File
# End Group
# Begin Group "SX sources"

# PROP Default_Filter "*.c *.cpp"
# Begin Source File

SOURCE=..\..\SX\Windows\SwapMX25data.c
# End Source File
# Begin Source File

SOURCE=..\..\SX\Windows\sxusbcam.c
# End Source File
# Begin Source File

SOURCE=..\..\SX\Windows\sxvideo.cpp
# End Source File
# End Group
# Begin Group "sextract sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\esource\ANALYSE.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\ASTROMETRY.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\BACK.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\CATOUT.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\CLEAN.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\CONVOLVE.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\dlmalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\EXTRACT.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\FILEIO.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\FITSUTIL.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\GRAPH.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\heapwalk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\MAKEIT.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\MANOBJLI.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\MISC.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\NEURRO.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\PHOTOM.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\PREFS.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\REFINE.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\SCAN.C
# End Source File
# Begin Source File

SOURCE=..\..\..\esource\SEXTRACT.C
# End Source File
# End Group
# End Target
# End Project
