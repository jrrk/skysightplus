/*** COPYRIGHT NOTICE AND PUBLIC SOURCE LICENSE *********************************

Portions Copyright (c) 1992-2001 Southern Stars Systems.  All Rights Reserved.

This file contains Original Code and/or Modifications of Original Code as defined
in and that are subject to the Southern Stars Systems Public Source License
Version 1.0 (the 'License').  You may not use this file except in compliance with
the License.  Please obtain a copy of the License at

http://www.southernstars.com/opensource/

and read it before using this file.

The Original Code and all software distributed under the License are distributed
on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, AND
SOUTHERN STARS SYSTEMS HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT
LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
QUIET ENJOYMENT, OR NON-INFRINGEMENT.  Please see the License for the specific
language governing rights and limitations under the License.

CONTRIBUTORS:

TCD - Tim DeBenedictis (timmyd@southernstars.com)

MODIFICATION HISTORY:

1.0.0 - 09 Apr 2001 - TCD - Original Code.

*********************************************************************************/

/*****************************  header files  ********************************/

#include "GUILib.h"
#define GLOBAL extern
#include "GUIPriv.h"
#undef GLOBAL

/***  GGetNumPorts  **********************************************************/

short GGetNumPorts ( void )
{
	return ( 13 );
}

/***  GGetPortName  **********************************************************/

void GGetPortName ( short wPortNum, char *lpszPortName )
{
	if ( wPortNum > 0 && wPortNum < 10 )
		wsprintf ( lpszPortName, "COM%hd", wPortNum );

	if ( wPortNum > 9 && wPortNum < 14 )
    	wsprintf ( lpszPortName, "LPT%hd", wPortNum - 9 );
}

#ifdef GWIN32
#ifdef COM32S_H

/***  GOpenPort (Win32s implementation) **************************************/

HANDLE GOpenPort ( short nPort, long lInBufferSize, long lOutBufferSize )
{
	char	szPort[8] = { 0 };
	HANDLE	hCom;

	GGetPortName ( nPort, szPort );

	hCom = OpenComm32s ( szPort, lInBufferSize, lOutBufferSize );
	if ( hCom < 0 )
		return ( G_OPEN_PORT_FAILED );

	return ( hCom );
}

/***  GClosePort (Win32s implementation) *************************************/

int GClosePort ( HANDLE hCom )
{
	return ( CloseComm32s ( (SHORT) hCom ) == 0 ? TRUE : FALSE );
}

/***  GReadPort (Win32s implementation) **************************************/

int GReadPort ( HANDLE hCom, long lNumBytes, void *lpvBuffer )
{
	ReadComm32s ( (SHORT) hCom, lpvBuffer, lNumBytes );
    return ( GetCommError32s ( (SHORT) hCom, NULL ) == 0 ? TRUE : FALSE );
}
    
/***  GWritePort (Win32s implementation) **************************************/

int GWritePort ( HANDLE hCom, long lNumBytes, void *lpvBuffer )
{
	WriteComm32s ( (SHORT) hCom, lpvBuffer, lNumBytes );
    return ( GetCommError32s ( (SHORT) hCom, NULL ) == 0 ? TRUE : FALSE );
}

/***  GSetPortState (Win32s implementation) ***********************************/

int GSetPortState ( HANDLE hCom, long lBaudRate, long lParity, long lDataBits, long lStopBits )
{
	DCB16	dcb;

    if ( GetCommState32s ( (SHORT) hCom, &dcb ) < 0 )
    	return ( FALSE );

    dcb.BaudRate = lBaudRate;
   	dcb.Parity = lParity;
   	dcb.ByteSize = lDataBits;
  	dcb.StopBits = lStopBits;

    if ( SetCommState32s ( &dcb ) < 0 )
		return ( FALSE );

	return ( TRUE );
}

/***  GGetPortState (Win32s implementation) ***********************************/

int GGetPortState ( HANDLE hCom, long *lBaudRate, long *lParity, long *lDataBits, long *lStopBits )
{
	DCB16	dcb;

    if ( GetCommState32s ( (SHORT) hCom, &dcb ) < 0 )
    	return ( FALSE );

    *lBaudRate = dcb.BaudRate;
   	*lParity   = dcb.Parity ;
   	*lDataBits = dcb.ByteSize;
  	*lStopBits = dcb.StopBits ;

	return ( TRUE );
}

/***  GGetPortDataLength (Win32s implementation) *****************************/

int GGetPortDataLength ( HANDLE hCom, long *lplInputBytes, long *lplOutputBytes )
{
	COMSTAT16	comstat;

    if ( GetCommError32s ( (SHORT) hCom, &comstat ) )
    	return ( FALSE );

    if ( lplInputBytes )
    	*lplInputBytes = comstat.cbInQue;

    if ( lplOutputBytes )
    	*lplOutputBytes = comstat.cbOutQue;

	return ( TRUE );
}

#else

/***  GOpenPort (Win32 implementation) ***************************************/

HANDLE GOpenPort ( short nPort, long lInBufferSize, long lOutBufferSize )
{
	char	szPort[8] = { 0 };
	HANDLE	hCom;

	GGetPortName ( nPort, szPort );

	hCom = CreateFile ( szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );

	if ( hCom == INVALID_HANDLE_VALUE )
		return ( G_OPEN_PORT_FAILED );

	if ( SetupComm ( hCom, lInBufferSize, lOutBufferSize ) == FALSE )
	{
		CloseHandle ( hCom );
		return ( G_OPEN_PORT_FAILED );
	}

	return ( hCom );
}

/***  GClosePort (Win32 implementation) **************************************/

int GClosePort ( HANDLE hCom )
{
	return ( CloseHandle ( hCom ) );
}

/***  GReadPort (Win32 implementation) ***************************************/

int GReadPort ( HANDLE hCom, long lNumBytes, void *lpvBuffer )
{
	long lNumBytesRead;

    return ( ReadFile ( hCom, lpvBuffer, lNumBytes, &lNumBytesRead, NULL ) );
}

/***  GWritePort (Win32 implementation) **************************************/

int GWritePort ( HANDLE hCom, long lNumBytes, void *lpvBuffer )
{
	long	lNumBytesWritten;

    return ( WriteFile ( hCom, lpvBuffer, lNumBytes, &lNumBytesWritten, NULL ) );
}

/***  GSetPortState (Win32 implementation) ***********************************/

int GSetPortState ( HANDLE hCom, long lBaudRate, long lParity, long lDataBits, long lStopBits )
{
	DCB		dcb;

	if ( GetCommState ( hCom, &dcb ) == FALSE )
		return ( FALSE );

	dcb.BaudRate = lBaudRate;
	dcb.Parity = lParity;
	dcb.ByteSize = lDataBits;
	dcb.StopBits = lStopBits;

	if ( SetCommState ( hCom, &dcb ) == FALSE )
		return ( FALSE );

	return ( TRUE );
}

/***  GGetPortState (Win32 implementation) ***********************************/

int GGetPortState ( HANDLE hCom, long *lBaudRate, long *lParity, long *lDataBits, long *lStopBits )
{
	DCB	dcb;

    if ( GetCommState ( hCom, &dcb ) < 0 )
    	return ( FALSE );

    *lBaudRate = dcb.BaudRate;
   	*lParity   = dcb.Parity ;
   	*lDataBits = dcb.ByteSize;
  	*lStopBits = dcb.StopBits ;

	return ( TRUE );
}

/***  GGetPortDataLength (Win32 implementation) *******************************/

int GGetPortDataLength ( HANDLE hCom, long *lplInputBytes, long *lplOutputBytes )
{
	COMSTAT		comstat;
	long		lError;

	if ( ClearCommError ( hCom, &lError, &comstat ) == FALSE )
	 	return ( FALSE );

	if ( lplInputBytes )
		*lplInputBytes = comstat.cbInQue;

	if ( lplOutputBytes )
		*lplOutputBytes = comstat.cbOutQue;

	return ( TRUE );
}

#endif

#else

/***  GOpenPort (Win16 implementation) ***************************************/

HANDLE GOpenPort ( short nPort, long lInBufferSize, long lOutBufferSize )
{
	char	szPort[8] = { 0 };
	HANDLE	hCom;

	GGetPortName ( nPort, szPort );

	hCom = OpenComm ( szPort, lInBufferSize, lOutBufferSize );
	if ( hCom < 0 )
		return ( G_OPEN_PORT_FAILED );

	return ( hCom );
}

/***  GClosePort (Win16 implementation) **************************************/

int GClosePort ( HANDLE hCom )
{
	return ( CloseComm ( hCom ) == 0 ? TRUE : FALSE );
}

/***  GReadPort (Win16 implementation) ***************************************/

int GReadPort ( HANDLE hCom, long lNumBytes, void *lpvBuffer )
{
	ReadComm ( hCom, lpvBuffer, lNumBytes );

	return ( GetCommError( hCom, NULL ) == 0 ? TRUE : FALSE );
}

/***  GWritePort (Win16 implementation) **************************************/

int GWritePort ( HANDLE hCom, long lNumBytes, void *lpvBuffer )
{
	WriteComm ( hCom, lpvBuffer, lNumBytes );

	return ( GetCommError( hCom, NULL ) == 0 ? TRUE : FALSE );
}

/***  GSetPortState (Win16 implementation) ************************************/

int GSetPortState ( HANDLE hCom, long lBaudRate, long lParity, long lDataBits, long lStopBits )
{
	DCB		dcb;

	if ( GetCommState ( hCom, &dcb ) < 0 )
		return ( FALSE );

	dcb.BaudRate = lBaudRate;
	dcb.Parity = lParity;
	dcb.ByteSize = lDataBits;
	dcb.StopBits = lStopBits;

	if ( SetCommState ( &dcb ) < 0 )
		return ( FALSE );

	return ( TRUE );
}

/***  GGetPortDataLength (Win16 implementation) ******************************/

int GGetPortDataLength ( HANDLE hCom, long *lplInputBytes, long *lplOutputBytes )
{
	COMSTAT		comstat;

	if ( GetCommError ( hCom, &comstat ) )
		return ( FALSE );

	if ( lplInputBytes )
		*lplInputBytes = comstat.cbInQue;

	if ( lplOutputBytes )
		*lplOutputBytes = comstat.cbOutQue;

	return ( TRUE );
}

#endif

