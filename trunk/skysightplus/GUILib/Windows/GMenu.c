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

static void sGetAllMenuIDs ( HMENU );

/************************  GGetMenuBar  **************************************/

GMenuBarPtr GGetMenuBar ( short wMenuBarID )
{
	/*** We only load the menu bar and accelerator table resources if the
	     application has created a window, and has not already loaded
	     the menu bar/accelerators.  We then attach the menu bar to the
	     application window. ***/

	if ( GGetMainWindow() && gMenuBar == NULL )
	{
		gMenuBar = LoadMenu ( GGetInstance(), MAKEINTRESOURCE ( wMenuBarID ) );
		gAccelerators = LoadAccelerators ( GGetInstance(), MAKEINTRESOURCE ( wMenuBarID ) );
		SetMenu ( GGetMainWindow(), gMenuBar );
	}
	
	return ( gMenuBar );
}

/**************************  GCreateMenu  ***********************************/

GMenuPtr GCreateMenu ( short wMenuID )
{
	return ( CreatePopupMenu() );
}

/**************************  GDeleteMenu  ***********************************/

void GDeleteMenu ( GMenuPtr hMenu )
{
	DestroyMenu ( hMenu );
}

/**************************  GGetMainMenu  **********************************/

GMenuPtr GGetMainMenu ( short wItem )
{
	HWND			hWnd;
	MENUITEMINFO	info;

	/*** If we have an MDI application, and any of the MDI child windows are
         maximized, Windows actually inserts the child window's system menu
         as the first menu in the menu bar.  We have to detect this and if so,
         return the correct menu handle. ***/

    if ( gClientWindow != NULL )
	{
		hWnd = GetWindow ( gClientWindow, GW_CHILD );
    	if ( hWnd != NULL && IsWindowVisible ( hWnd ) && IsZoomed ( hWnd ) )
			return ( GetSubMenu ( gMenuBar, wItem ) );
	}
	

	return ( GetSubMenu ( gMenuBar, wItem - 1 ) );
}

/**************************  GGetSubMenu  ************************************/

GMenuPtr GGetSubMenu ( GMenuPtr hMenu, short wItem )
{
	return ( GetSubMenu ( hMenu, wItem - 1 ) );
}

/*************************  GSetSubMenu  **********************************/

void GSetSubMenu ( GMenuPtr hMenu, short wItem, GMenuPtr hSubMenu )
{
	ModifyMenu ( hMenu, wItem - 1, MF_BYPOSITION | MF_POPUP, (UINT) hSubMenu, NULL );
}

/************************  GInsertMenuItem  *********************************/

void GInsertMenuItem ( GMenuPtr hMenu, short wMenuID, short wItem, char *lpszText )
{
	UINT	idItem;

	if ( wItem > GetMenuItemCount ( hMenu ) )
	{
		idItem = 100L * wMenuID + GetMenuItemCount ( hMenu ) + 1;
		InsertMenu ( hMenu, -1, MF_BYPOSITION | MF_STRING, idItem, lpszText );
	}
	else
	{
		idItem = 100L * wMenuID + wItem;
		InsertMenu ( hMenu, wItem - 1, MF_BYPOSITION | MF_STRING, idItem, lpszText );
	}

	for ( wItem++; wItem <= GetMenuItemCount ( hMenu ); wItem++ )
	{
    	idItem = GetMenuItemID ( hMenu, wItem - 1 ) + 1;
		ModifyMenu ( hMenu, wItem - 1, MF_BYPOSITION, idItem, NULL );
	}
}

/*************************  GDeleteMenuItem  ********************************/

void GDeleteMenuItem ( GMenuPtr hMenu, short wItem )
{
	UINT idItem;

	DeleteMenu ( hMenu, wItem - 1, MF_BYPOSITION );
	for ( wItem; wItem <= GetMenuItemCount ( hMenu ); wItem++ )
	{
		idItem = GetMenuItemID ( hMenu, wItem - 1 ) - 1;
		ModifyMenu ( hMenu, wItem - 1, MF_BYPOSITION, idItem, NULL );
	}
}

/*************************  GGetMenuItemText  ********************************/

char *GGetMenuItemText ( GMenuPtr hMenu, short wItem, char *lpszItem )
{
	GetMenuString ( hMenu, wItem - 1, lpszItem, 255, MF_BYPOSITION );
	return ( lpszItem );
}

/**************************  GSetMenuItemText  *******************************/

void GSetMenuItemText ( GMenuPtr hMenu, short wItem, char *lpszText )
{
	UINT idItem = GetMenuItemID ( hMenu, wItem - 1 );
	ModifyMenu ( hMenu, wItem - 1, MF_STRING | MF_BYPOSITION, idItem, lpszText );
}

/************************  GSetMenuItemGraphic  ***************************/

void GSetMenuItemGraphic ( GMenuPtr hMenu, short wItem, short idBitmap, char nIconType )
{
	UINT	idItem = GetMenuItemID ( hMenu, wItem - 1 );
	HBITMAP	hBitmap = LoadBitmap ( GGetInstance(), MAKEINTRESOURCE ( idBitmap ) );
	
	ModifyMenu ( hMenu, wItem - 1, MF_BITMAP | MF_BYPOSITION, idItem, (LPCSTR) hBitmap );
}

/**************************  GEnableMainMenu  ******************************/

void GEnableMainMenu ( short wMenu, int fEnable )
{
	if ( fEnable )
		EnableMenuItem ( gMenuBar, wMenu - 1, MF_BYPOSITION | MF_ENABLED );
	else
    	EnableMenuItem ( gMenuBar, wMenu - 1, MF_BYPOSITION | MF_GRAYED );

	DrawMenuBar ( GGetMainWindow() );
}

/**************************  GEnableMenuItem  ******************************/

void GEnableMenuItem ( GMenuPtr hMenu, short wItem, int fEnable )
{
	if ( fEnable )
		EnableMenuItem ( hMenu, wItem - 1, MF_BYPOSITION | MF_ENABLED );
	else
		EnableMenuItem ( hMenu, wItem - 1, MF_BYPOSITION | MF_GRAYED );
}

/************************  GIsMenuItemEnabled  ******************************/

int GIsMenuItemEnabled ( GMenuPtr hMenu, short wItem )
{
	UINT nState = GetMenuState ( hMenu, wItem, MF_BYPOSITION );

	if ( nState & ( MF_GRAYED | MF_DISABLED ) )
		return ( FALSE );
	else
		return ( TRUE );
}

/**************************  GCheckMenuItem  *********************************/

void GCheckMenuItem ( GMenuPtr hMenu, short wItem, int fCheck )
{
	if ( fCheck )
		CheckMenuItem ( hMenu, wItem - 1, MF_BYPOSITION | MF_CHECKED );
	else
		CheckMenuItem ( hMenu, wItem - 1, MF_BYPOSITION | MF_UNCHECKED );
}

/*************************  GIsMenuItemChecked  *****************************/

int GIsMenuItemChecked ( GMenuPtr hMenu, short wItem )
{
	UINT nState = GetMenuState ( hMenu, wItem - 1, MF_BYPOSITION );

	if ( nState & MF_CHECKED )
		return ( TRUE );
	else
		return ( FALSE );
}

/**************************  GSetCheckedMenuItem  ****************************/

void GSetCheckedMenuItem ( GMenuPtr hMenu, short wFirstItem, short wLastItem, short wCheckedItem )
{
	short	wItem;

	for ( wItem = wFirstItem; wItem <= wLastItem; wItem++ )
	{
		if ( wItem == wCheckedItem )
			CheckMenuItem ( hMenu, wItem - 1, MF_BYPOSITION | MF_CHECKED );
		else
			CheckMenuItem ( hMenu, wItem - 1, MF_BYPOSITION | MF_UNCHECKED );
	}
}

/**************************  GSetCheckedMenuItem  ****************************/

short GGetCheckedMenuItem ( GMenuPtr hMenu, short wFirstItem, short wLastItem )
{
	short	wItem;
	UINT	nState;

	for ( wItem = wFirstItem; wItem <= wLastItem; wItem++ )
	{
		nState = GetMenuState ( hMenu, wItem, MF_BYPOSITION );
		if ( nState & MF_CHECKED )
        	return ( wItem );
	}

	return ( 0 );
}

/************************  GGetNumMenuItems  *****************************/

short GGetNumMenuItems ( GMenuPtr hMenu )
{
	return ( GetMenuItemCount ( hMenu ) );
}

/****************************  GGetMenuBarHeight  ****************************/

short GGetMenuBarHeight ( void )
{
	return ( GetSystemMetrics ( SM_CYMENU ) );
}

/***************************  GDoPopupMenu  *******************************/

int GDoPopupMenu ( GMenuPtr hMenu, short wX, short wY, short wItem )
{
	UINT	fuFlags;
    int		nResult = FALSE;
    
	if ( GetAsyncKeyState ( VK_LBUTTON ) < 0 )
		fuFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON;
	else
		fuFlags = TPM_LEFTALIGN | TPM_RIGHTBUTTON;

	if ( TrackPopupMenu ( hMenu, fuFlags, wX, wY, 0, GGetMainWindow(), NULL ) != 0 )
    	nResult = TRUE;

    return ( nResult );
}

/**************************  GAddFontMenu  **********************************/

short GAddFontMenu ( GMenuPtr hMenu )
{
	return ( 0 );
}

/*************************  GDoFontMenuItem  ********************************/

void GDoFontMenuItem ( short wMenuID, short wItem, char *lpszFontName,
short *lpwSize, short *lpwStyle )
{

}

/*************************  GCheckFontMenuItems  ***************************/

void GCheckFontMenuItems ( char *lpszFontName, short wSize, short wStyle )
{

}

