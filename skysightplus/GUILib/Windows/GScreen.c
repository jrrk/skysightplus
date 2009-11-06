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

#include "GUILib.h"
#include "GUIPriv.h"

/***********************  GGetScreenRect  *********************************/

LPRECT GGetScreenRect ( LPRECT lpRect )
{
	GetWindowRect ( GetDesktopWindow(), lpRect );
	return ( lpRect );
}

/***********************  GGetDesktopRect  *********************************/

LPRECT GGetDesktopRect ( LPRECT lpRect )
{
#if ( WINVER >= 0x0400 )
		SystemParametersInfo ( SPI_GETWORKAREA, 0, lpRect, 0 );
#else
		GetWindowRect ( GetDesktopWindow(), lpRect );
#endif

	return ( lpRect );
}

/***********************  GGetWorkspaceRect  *********************************/

LPRECT GGetWorkspaceRect ( LPRECT lpRect )
{
	HWND	hWnd = GGetMDIClientWindow();

	if ( hWnd != NULL )
	{
		GetClientRect ( hWnd, lpRect );
	}
	else
	{
#if ( WINVER >= 0x0400 )
		SystemParametersInfo ( SPI_GETWORKAREA, 0, lpRect, 0 );
#else
		GetWindowRect ( GetDesktopWindow(), lpRect );
#endif
	}

	return ( lpRect );
}

/****************************  GetScreenWidth  *****************************/

short GGetScreenWidth ( void )
{
	return ( GetSystemMetrics ( SM_CXSCREEN ) );
}

/****************************  GetScreenHeight  *****************************/

short GGetScreenHeight ( void )
{
	return ( GetSystemMetrics ( SM_CYSCREEN ) );
}

/*****************************  GGetScreenDepth  ***************************/

short GGetScreenDepth ( void )
{
	int	nBitsPixel, nPlanes;
	HDC	hDC;
	
	hDC = GetDC ( NULL );
	nBitsPixel = GetDeviceCaps ( hDC, BITSPIXEL );
	nPlanes = GetDeviceCaps ( hDC, PLANES );
	ReleaseDC ( NULL, hDC );
	
	return ( nBitsPixel * nPlanes );
}

/****************************  GSetScreenDepth  *****************************/

int GSetScreenDepth ( short depth )
{
	return ( FALSE );
}

/***********************  GWindowToScreen  **********************************/

void GWindowToScreen ( GWindowPtr hwnd, short *lpwX, short *lpwY )
{
	POINT point;

	point.x = *lpwX;
	point.y = *lpwY;

	ClientToScreen ( hwnd, &point );

	*lpwX = point.x;
    *lpwY = point.y;
}

/***********************  GScreenToWindow  ******************************/

void GScreenToWindow ( GWindowPtr hwnd, short *lpwX, short *lpwY )
{
	POINT point;

	point.x = *lpwX;
	point.y = *lpwY;

	ScreenToClient ( hwnd, &point );

	*lpwX = point.x;
    *lpwY = point.y;
}

/**********************  GWindowToScreenRect  **************************/

GRectPtr GWindowToScreenRect ( GWindowPtr hwnd, GRectPtr lpRect )
{
	POINT	point;

	point.x = lpRect->left;
	point.y = lpRect->top;

	ClientToScreen ( hwnd, &point );

	lpRect->left = point.x;
	lpRect->top = point.y;

	point.x = lpRect->right;
	point.y = lpRect->bottom;

	ClientToScreen ( hwnd, &point );

	lpRect->right = point.x;
	lpRect->bottom = point.y;
	
	return ( lpRect );
}

/**********************  GScreenToWindowRect  **************************/

GRectPtr GScreenToWindowRect ( GWindowPtr hwnd, GRectPtr lpRect )
{
	POINT	point;

	point.x = lpRect->left;
	point.y = lpRect->top;

	ScreenToClient ( hwnd, &point );

	lpRect->left = point.x;
	lpRect->top = point.y;

	point.x = lpRect->right;
	point.y = lpRect->bottom;

	ScreenToClient ( hwnd, &point );

	lpRect->right = point.x;
	lpRect->bottom = point.y;
	
	return ( lpRect );
}

/****************************  GGetSystemColors  ***********************************/

long GGetSystemColors ( GColor rgbColors[256], long lFirstIndex, long lLastIndex )
{
	long i;

	for ( i = lFirstIndex; i < 21; i++ )
		rgbColors[i] = GetSysColor ( i );

	return ( 21 );
}

/***************************  GSetSystemColors  *************************************/

long GSetSystemColors ( GColor rgbColors[256], long lFirstIndex, long lLastIndex, int fUpdate )
{
	int i, nDspElements[21];

	for ( i = 0; i < 21; i++ )
		nDspElements[i] = i;

	SetSysColors ( 21, nDspElements, rgbColors );

    return ( 21 );
}

