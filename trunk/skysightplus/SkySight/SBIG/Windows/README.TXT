                        Santa Barbara Instrument Group
                          Parallel Port Driver Disk
                              May 18, 1999

The files on this disk are for users that want to write their own software
for the SBIG ST-7, ST-8, PixCel255 and PixCel237 cameras.  The following
environments are supported:

MS-DOS
Windows 3.1 in 16 bits
Windows 3.1 in 32 bits using WIN32S
Windows 95 in 16 bits
Windows 95/98 in 32 bits using Microsoft Visual C++, Visual Basic or Borland C++

The files are used as follows:
------------------------------
PARDRV.H - Include this in any C or C++ files that need to use the driver
           It contains the function prototypes and struct definitions used
           with the software.
PARDRV.LIB - You can statically link this module with DOS, Windows 3.1 and
           16 bit Windows 95 programs.  You need to use the large or huge
           model as this is a far function that takes far pointers.
SBIG16.DLL - This is a DLL that you can use with 16 bit Windows 3.1 and
           Windows 95 programs.
SBIG16.LIB - This is a 16 bit import library for the SBIG16.DLL that you
           include with your 16 bit Windows projects.  It resolves
           references to the ParDrvCommand() function in the SBIG16.DLL
           file.
SBIG32M.DLL, SBIG32B.DLL - These are 32 bit DLLs for Windows 95.  They
           contain the ParDrvCommand() function.  If you are developing
           32 bit Windows 95 code put one of these DLLs in the same
           directory as your application.  If you're developing under
           the Microsoft Visual C++ environment use the SBIG32M.DLL.
           If you're developing under the Borland C++ environment use
           the SBIG32B.DLL.
SBIG32M.LIB, SBIG32B.LIB - These are 32 bit Import Libraries that you
           include in your project and link with your 32 bit Windows 95
           program.  They resolve reverences to the ParDrvCommand() function.
           If you're developing under the Microsoft Visual C++ environment
           use the SBIG32M.LIB.  If you're developing under the Borland C++
           environment use the SBIG32B.LIB.
SBIG32.VXD - This is the 32 bit VxD driver for Windows 95.  If you are
           developing 32 bit software for Windows 95 and you are using
           the SBIG32M/B.DLL then copy this file into the WINDOWS\SYSTEM
           directory so it can be loaded and used by the SBIG32M/B.DLL.
SBIG32S.DLL - This is a 32 bit DLL that can be used under Windows 3.1
           when running WIN32S or under Windows 95 without having to
           use the SBIG32.VXD driver.  If you're really writing a
           32 bit Windows 95 program you ought to use the SBIG32M/B.DLL
           and the SBIG32.VXD for compatibility.
SBIG32S.LIB - This is a Microsoft format Import Library that is used
           with SBIG32S.DLL.  Include this in your Visual C++ project
           to resolve external references to the ParDrvCommand()
           function.
SBIG32VB.DLL - This is a DLL that you use with Visual Basic programs.
           It works in conjunction with the SBIG32.VXD.  Also see
           the vbexampl directory.
<ST7DEMO> - This directory contains a sample Visual Basic 5 program
           for talking to an ST-7. 
<S5CDEMO> - This directory contains a sample Visual Basic 5 program
           for talking to an ST-5C.
<ST7DLL> - This directory contains the latest version of the ST7.DLL
           (and a import library) that was used with our original
           CCDOPS for Windows, the 16 bit Windows 3.1 application.
PARDRV.DOC - This is a Microsoft Word for Macintosh file containing the
           documentation for the driver.
PARDRV.PDF - This is a Adobe Acrobat file containing the documentation
           for the driver.

If you have any questions about this software please check our homepage
<www.sbig.com> for the latest information or send e-mail to SBIG
Technical Support at <sbig@sbig.com>.

