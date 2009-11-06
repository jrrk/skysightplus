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

/**************************  GGetCursor  **********************************/

HCURSOR GGetCursor ( short idCursor )
{
	return ( LoadCursor ( GGetInstance(), MAKEINTRESOURCE ( idCursor ) ) );
}

/************************  GGetArrowCursor  ********************************/

HCURSOR GGetArrowCursor ( void )
{
	return ( LoadCursor ( NULL, IDC_ARROW ) );
}

/************************  GGetWaitCursor  ********************************/

HCURSOR GGetWaitCursor ( void )
{
	return ( LoadCursor ( NULL, IDC_WAIT ) );
}

/************************  GGetCrossCursor  ********************************/

HCURSOR GGetCrossCursor ( void )
{
	return ( LoadCursor ( NULL, IDC_CROSS ) );
}

/************************  GGetTextCursor  ********************************/

HCURSOR GGetTextCursor ( void )
{
	return ( LoadCursor ( NULL, IDC_IBEAM ) );
}

/**************************  GSetCursor  **********************************/

void GSetCursor ( HCURSOR hCursor )
{
	if ( hCursor == NULL )
		SetCursor ( LoadCursor ( NULL, IDC_ARROW ) );
	else
		SetCursor ( hCursor );
}

/************************  GSetArrowCursor  ********************************/

void GSetArrowCursor ( void )
{
	SetCursor ( LoadCursor ( NULL, IDC_ARROW ) );
}

/************************  GSetWaitCursor  ********************************/

void GSetWaitCursor ( void )
{
	SetCursor ( LoadCursor ( NULL, IDC_WAIT ) );
}

/************************  GSetTextCursor  ********************************/

void GSetTextCursor ( void )
{
	SetCursor ( LoadCursor ( NULL, IDC_IBEAM ) );
}

/************************  GSetCrossCursor  ********************************/

void GSetCrossCursor ( void )
{
	SetCursor ( LoadCursor ( NULL, IDC_CROSS ) );
}

/***************************  GShowCursor  *********************************/

void GShowCursor ( void )
{
	ShowCursor ( TRUE );
}

/**************************  GHideCursor  **********************************/

void GHideCursor ( void )
{
	ShowCursor ( FALSE );
}

/**************************  GGetMousePosition  ***************************/

void GGetMousePosition ( GWindowPtr window, short *x, short *y )
{
	POINT point;

	GetCursorPos ( &point );
	if ( window )
		ScreenToClient ( window, &point );

	*x = point.x;
	*y = point.y;
}

/**********************  GGetDoubleClickTime  ************************/

long GGetDoubleClickTime ( void )
{
	return ( GetDoubleClickTime() );
}

/**************************  GMouseButtonIsDown  **************************/

int GMouseButtonIsDown ( unsigned char buttons )
{
	if ( ( buttons & 1 ) && ( GetAsyncKeyState ( VK_LBUTTON ) < 0  ) )
		return ( TRUE );

	if ( ( buttons & 2 ) && ( GetAsyncKeyState ( VK_RBUTTON ) < 0 ) )
		return ( TRUE );

	return ( FALSE );
}

/****************************  GKeyIsDown  ********************************/

int GKeyIsDown ( unsigned char vkCode )
{
	if ( GetAsyncKeyState ( vkCode ) < 0 )
		return ( TRUE );
	else
		return ( FALSE );
}


