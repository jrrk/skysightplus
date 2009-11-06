/*** COPYRIGHT NOTICE AND PUBLIC SOURCE LICENSE ***************************************

	Portions Copyright (c) 1992-2001 Southern Stars Systems.  All Rights Reserved.

	This file contains Original Code and/or Modifications of Original Code as
	defined in and that are subject to the Southern Stars Systems Public Source
	License Version 1.0 (the 'License').  You may not use this file except in
	compliance with the License.  Please obtain a copy of the License at

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

****************************************************************************************/

#include "SkySight.h"

/*** local constants and macros ***/

#define ABOUT_BITMAP		256
#define ABOUT_BITMAP_RECT	2

/*** local data ***/

static int sExitSplashScreen = FALSE;

/***  DoAppleMenuItem  **************************************************************/

int DoAppleMenuItem ( short item )
{
	int respond = TRUE;
	
	switch ( item )
	{
		case APPLE_ABOUT_ITEM:
			GEnterModalDialog ( ABOUT_DIALOG, 0, DoAboutDialogEvent );
			break;
	}

	return ( respond );
}

/***  DoAboutDialogEvent  ***********************************************************/

int DoAboutDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	GRect			bitmapRect, rect;
	GBitmapPtr		bitmap;

	switch ( event )
	{
		case G_UPDATE_EVENT:
			if ( ( bitmap = GGetBitmap ( ABOUT_BITMAP ) ) != NULL )
			{
				GGetDialogItemRect ( dialog, ABOUT_BITMAP_RECT, &rect );
				GGetBitmapRect ( bitmap, &bitmapRect );
				GCenterRect ( &bitmapRect, &rect );
                GStartDrawing ( dialog );
				GDrawBitmap ( bitmap, NULL, &bitmapRect );
                GEndDrawing ( dialog );
			}
			break;
		
		case G_MOUSE_DOWN_EVENT:
			sExitSplashScreen = TRUE;
			break;
		 
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					GExitModalDialogLoop ( param1 );
					break;
			}
	}
	
	return ( TRUE );
}

/**** CreateSplashScreen *****************************************************************/

GWindowPtr CreateSplashScreen ( void )
{
	GRect		rect;
	GWindowPtr	window;
		
	if ( ( window = GCreateDialog ( ABOUT_DIALOG, 0, DoAboutDialogEvent ) ) != NULL )
	{
		GGetDialogItemRect ( window, ABOUT_BITMAP_RECT, &rect );
		GSetWindowContentRect ( window, &rect );
		GGetWindowContentRect ( window, &rect );
		GSetDialogItemRect ( window, ABOUT_BITMAP_RECT, &rect );
		GShowWindow ( window );
		GUpdateWindow ( window );
	}
	
	return ( window );
}

/*** DeleteSplashScreen *****************************************************************/

void DeleteSplashScreen ( GWindowPtr window )
{
	GDeleteWindow ( window );
}

/*** ShowSplashScreen *********************************************************************/

void ShowSplashScreen ( long timeout )
{
	long		startTick;
	GWindowPtr	dialog;
	GEvent		event;
	
	if ( ( dialog = CreateSplashScreen() ) != NULL )
	{
		startTick = GGetTickCount();
		sExitSplashScreen = FALSE;
	
		while ( sExitSplashScreen == FALSE )
		{
			GGetNextEvent ( &event );
			GProcessEvent ( &event );
		
			if ( ( GGetTickCount() - startTick ) > timeout )
				sExitSplashScreen = TRUE;
		}
	
		DeleteSplashScreen ( dialog );
	}
}
