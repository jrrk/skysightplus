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

/*** local functions ***/

static void UpdateImageToolMenu ( void );
static void UpdateMagnificationMenu ( GMenuPtr, GWindowPtr );
static void UpdateMouseCoordinatesMenu ( GMenuPtr, GWindowPtr );

/***  DoMenuItem  ************************************************************/

void DoMenuItem ( long menuID, long item )
{
	switch ( menuID )
	{
		case APPLE_MENU_ID:
			DoAppleMenuItem ( item );
			break;
			
		case FILE_MENU_ID:
			DoFileMenuItem ( item );
			break;

		case EDIT_MENU_ID:
			DoEditMenuItem ( item );
			break;

		case EDIT_COLOR_FRAME_MENU_ID:
			DoEditColorFrameMenuItem ( item );
			break;
			
		case DISPLAY_MENU_ID:
			DoDisplayMenuItem ( item );
			break;

		case COLOR_TABLE_MENU_ID:
			DoColorTableMenuItem ( item );
			break;
			
		case COLOR_FRAME_MENU_ID:
			DoColorFrameMenuItem ( item );
			break;
			
		case MOUSE_COORDINATES_MENU_ID:
			DoMouseCoordinatesMenuItem ( item );
			break;
			
		case PROCESS_MENU_ID:
			DoProcessMenuItem ( item );
			break;
		
		case ADD_MENU_ID:
		case SUBTRACT_MENU_ID:
		case MULTIPLY_MENU_ID:
		case DIVIDE_MENU_ID:
			DoImageArithmeticMenuItem ( menuID, item );
			break;
		
		case ROTATE_MENU_ID:
			DoRotateMenuItem ( item );
			break;
		
		case CONVOLVE_MENU_ID:
			DoConvolveMenuItem ( item );
			break;
		
		case ALIGN_MENU_ID:
			DoAlignMenuItem ( item );
			break;
			
		case MOSAIC_MENU_ID:
			DoMosaicMenuItem ( item );
			break;
			
		case ANALYZE_MENU_ID:
			DoAnalyzeMenuItem ( item );
			break;
					
		case WINDOW_MENU_ID:
			DoWindowMenuItem ( item );
			break;

        case HELP_MENU_ID:
            DoHelpMenuItem ( item );
        	break;

		case MAGNIFICATION_MENU_ID:
			DoMagnificationMenuItem ( item );
			break;
	}
}

/***  UpdateMenus  ***********************************************************/

void UpdateMenus ( void )
{
	GMenuPtr	fileMenu = GGetMainMenu ( FILE_MENU );
	GMenuPtr	editMenu = GGetMainMenu ( EDIT_MENU );
	GMenuPtr	windowMenu = GGetMainMenu ( WINDOW_MENU );
	GMenuPtr	menu;
	short		frame, item, numItems;
	ImagePtr	image = NULL;
	GWindowPtr	window = GGetMenuEventWindow();
	
	if ( window == NULL )
	{
		GEnableMenuItem ( fileMenu, FILE_NEW_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_OPEN_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_CLOSE_ITEM, FALSE );
 		GEnableMenuItem ( fileMenu, FILE_SAVE_ITEM, FALSE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_AS_ITEM, FALSE );
		GEnableMenuItem ( fileMenu, FILE_PRINT_ITEM, FALSE );
		GEnableMenuItem ( fileMenu, FILE_PAGE_SETUP_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_QUIT_ITEM, TRUE );

        GEnableMainMenu ( EDIT_MENU, FALSE );
        UpdateImageToolMenu();
        
        GEnableMainMenu ( DISPLAY_MENU, FALSE );
        GEnableMainMenu ( PROCESS_MENU, FALSE );
		GEnableMainMenu ( ANALYZE_MENU, FALSE );
	}
	else if ( GGetWindowClass ( window ) == IMAGE_WINDOW )
	{
    	image = GetImageWindowImage ( window );
    	
    	/*** Enable/disable "File" menu items ***/

		GEnableMenuItem ( fileMenu, FILE_CLOSE_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_AS_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_PRINT_ITEM, TRUE );

        /*** Enable/disable "Edit" menu items ***/
        
        GEnableMainMenu ( EDIT_MENU, TRUE );

		GEnableMenuItem ( editMenu, EDIT_UNDO_ITEM, FALSE );
		GEnableMenuItem ( editMenu, EDIT_CUT_ITEM, FALSE );
		GEnableMenuItem ( editMenu, EDIT_COPY_ITEM, TRUE );
		GEnableMenuItem ( editMenu, EDIT_PASTE_ITEM, FALSE );
		GEnableMenuItem ( editMenu, EDIT_DELETE_ITEM, FALSE );

        GEnableMenuItem ( editMenu, EDIT_SELECT_ALL_ITEM, TRUE );
        GEnableMenuItem ( editMenu, EDIT_SELECT_NONE_ITEM, TRUE );
		
		if ( GetImageType ( image ) == IMAGE_TYPE_MONOCHROME )
		{
			GEnableMenuItem ( editMenu, EDIT_COLOR_FRAME_ITEM, FALSE );
		}
		
		if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
		{
			GEnableMenuItem ( editMenu, EDIT_COLOR_FRAME_ITEM, TRUE );
			
			frame = GetImageWindowSelectedFrame ( window, NULL, NULL );
			menu = GGetSubMenu ( editMenu, EDIT_COLOR_FRAME_ITEM );
			
			GCheckMenuItem ( menu, COLOR_FRAME_ALL, frame == ALL_FRAMES );
			GCheckMenuItem ( menu, COLOR_FRAME_RED, frame == RED_FRAME );
			GCheckMenuItem ( menu, COLOR_FRAME_GREEN, frame == GREEN_FRAME );
			GCheckMenuItem ( menu, COLOR_FRAME_BLUE, frame == BLUE_FRAME );
		}

		UpdateImageToolMenu();
				
		/*** Enable/disable "Display" menu items ***/
		
        GEnableMainMenu ( DISPLAY_MENU, TRUE );
        menu = GGetMainMenu ( DISPLAY_MENU );

		if ( GetImageType ( image ) == IMAGE_TYPE_MONOCHROME )
		{
			GEnableMenuItem ( menu, DISPLAY_COLOR_TABLE_ITEM, TRUE );
			GEnableMenuItem ( menu, DISPLAY_COLOR_FRAME_ITEM, FALSE );
			UpdateColorTableMenu ( window );
		}
		
		if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
		{
			GEnableMenuItem ( menu, DISPLAY_COLOR_TABLE_ITEM, FALSE );
			GEnableMenuItem ( menu, DISPLAY_COLOR_FRAME_ITEM, TRUE );
			UpdateColorFrameMenu ( window );
		}
		
		/*** Check/un-check "Magnification" sub-menu items ***/
		
    	GSetCheckedMenuItem ( GGetSubMenu ( menu, DISPLAY_MAGNIFICATION_ITEM ),
    	MAGNIFICATION_16X_ITEM, MAGNIFICATION_1_16X_ITEM,
    	GetImageWindowDisplayMagnification ( window ) );
    	
		GSetCheckedMenuItem ( GGetSubMenu ( menu, DISPLAY_MOUSE_COORDINATES_ITEM ),
        MOUSE_COORDINATES_NONE, MOUSE_COORDINATES_RA_DEC,
        GetImageWindowDisplayMouseCoordinates ( window ) );

		/*** Check/uncheck object display items ***/
		
		GCheckMenuItem ( menu, DISPLAY_OBJECTS_ITEM, GetImageWindowDisplayObjects ( window ) );
		GCheckMenuItem ( menu, DISPLAY_OBJECT_CENTROIDS_ITEM, GetImageWindowDisplayObjectCentroids ( window ) );
		GCheckMenuItem ( menu, DISPLAY_OBJECT_NAMES_ITEM, GetImageWindowDisplayObjectNames ( window ) );
		
		/*** Enable/disable "Process" menu items ***/
		
		GEnableMainMenu ( PROCESS_MENU, TRUE );
		menu = GGetMainMenu ( PROCESS_MENU );
		
		if ( GetImageType ( image ) == IMAGE_TYPE_MONOCHROME )
		{
			GEnableMenuItem ( menu, PROCESS_RGB_SEPARATE_ITEM, FALSE );
			GEnableMenuItem ( menu, PROCESS_RGB_BALANCE_ITEM, FALSE );
		}
		
		if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
		{
			GEnableMenuItem ( menu, PROCESS_RGB_SEPARATE_ITEM, TRUE );
			GEnableMenuItem ( menu, PROCESS_RGB_BALANCE_ITEM, TRUE );
		}

		/*** Enable/disable "Analyze" menu items ***/
		
		GEnableMainMenu ( ANALYZE_MENU, TRUE );
		
        /*** Check the "Window" menu item which corresponds to the active image window.
             We are now letting GUILib's window-menu-handling functionality do this.

        window = GetFirstImageWindow();
        numItems = GGetNumMenuItems ( windowMenu );
    	for ( item = WINDOW_CONTROL_ITEM + 1; item <= numItems && window != NULL; item++ )
    	{
        	if ( window == GetActiveImageWindow() )
    			GCheckMenuItem ( windowMenu, item, TRUE );
    		else
    			GCheckMenuItem ( windowMenu, item, FALSE );

			window = GetNextImageWindow ( window );
		}
		
		 ***/
    }
	else if ( GGetWindowClass ( window ) == HEADER_WINDOW )
	{
		GEnableMainMenu ( FILE_MENU, TRUE );
		
		GEnableMenuItem ( fileMenu, FILE_NEW_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_OPEN_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_CLOSE_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_AS_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_PAGE_SETUP_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_PRINT_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_QUIT_ITEM, TRUE );

		GEnableMainMenu ( EDIT_MENU, FALSE );
		GEnableMainMenu ( DISPLAY_MENU, FALSE );
		GEnableMainMenu ( PROCESS_MENU, FALSE );
		GEnableMainMenu ( ANALYZE_MENU, FALSE );
	}
	else if ( GGetWindowClass ( window ) == HISTOGRAM_WINDOW )
	{
		GEnableMainMenu ( FILE_MENU, TRUE );
		
		GEnableMenuItem ( fileMenu, FILE_NEW_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_OPEN_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_CLOSE_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_ITEM, FALSE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_AS_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_PAGE_SETUP_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_PRINT_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_QUIT_ITEM, TRUE );
		
		GEnableMainMenu ( EDIT_MENU, TRUE );

		GEnableMenuItem ( editMenu, EDIT_UNDO_ITEM, FALSE );
		GEnableMenuItem ( editMenu, EDIT_CUT_ITEM, FALSE );
		GEnableMenuItem ( editMenu, EDIT_COPY_ITEM, TRUE );
		GEnableMenuItem ( editMenu, EDIT_PASTE_ITEM, FALSE );
		GEnableMenuItem ( editMenu, EDIT_DELETE_ITEM, FALSE );

        GEnableMenuItem ( editMenu, EDIT_SELECT_ALL_ITEM, FALSE );
        GEnableMenuItem ( editMenu, EDIT_SELECT_NONE_ITEM, FALSE );
		
		GEnableMainMenu ( DISPLAY_MENU, FALSE );
		GEnableMainMenu ( PROCESS_MENU, FALSE );
		GEnableMainMenu ( ANALYZE_MENU, FALSE );
	}
	
	/*** The "window" menu is a little different.  It should always be enabled... ***/
	
	GEnableMainMenu ( WINDOW_MENU, TRUE );
	
	/*** ... but its "Tile"/"Cascade"/"Arrange Icons" items should be disabled if
	     there are no document windows present/active. ***/
	     	
	window = GGetMenuEventWindow();
	if ( window == NULL )
	{
        GEnableMenuItem ( windowMenu, WINDOW_TILE_ITEM, FALSE );
        GEnableMenuItem ( windowMenu, WINDOW_CASCADE_ITEM, FALSE );
        GEnableMenuItem ( windowMenu, WINDOW_ARRANGE_ITEM, FALSE );
	}
	else
	{
        GEnableMenuItem ( windowMenu, WINDOW_TILE_ITEM, TRUE );
        GEnableMenuItem ( windowMenu, WINDOW_CASCADE_ITEM, TRUE );
        GEnableMenuItem ( windowMenu, WINDOW_ARRANGE_ITEM, TRUE );
	}
	
	/*** The "Camera Control" Window menu item should always be enabled, and checked
	     or un-checked depending on whether or not the camera control dialog is visible. ***/
		     
    GEnableMenuItem ( windowMenu, WINDOW_CONTROL_ITEM, TRUE );
	GCheckMenuItem ( windowMenu, WINDOW_CONTROL_ITEM, GIsWindowVisible ( GetCameraControlDialog() ) );
}

/*** UpdateImageToolMenu ***/

void UpdateImageToolMenu ( void )
{
    GMenuPtr	menu = GGetMainMenu ( EDIT_MENU );
	short		item;
	
	for ( item = EDIT_SELECT_TOOL; item <= EDIT_MARK_TOOL; item++ )
	{
		if ( item == GetImageTool() )
			GCheckMenuItem ( menu, item, TRUE );
		else
			GCheckMenuItem ( menu, item, FALSE );
	}
}


/*** UpdateColorTableMenu ***/

void UpdateColorTableMenu ( GWindowPtr window )
{
	GMenuPtr	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_COLOR_TABLE_ITEM );
	short		item, table = GetImageWindowColorTable ( window );
	
	for ( item = COLOR_TABLE_GRAYSCALE; item <= COLOR_TABLE_BLUE; item++ )
	{
		if ( item == table )
			GCheckMenuItem ( menu, item, TRUE );
		else
			GCheckMenuItem ( menu, item, FALSE );
	}
}

/*** UpdateColorFrameMenu ***/

void UpdateColorFrameMenu ( GWindowPtr window )
{
	GMenuPtr	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_COLOR_FRAME_ITEM );
	short		item, frame = GetImageWindowColorFrame ( window );
	
	for ( item = COLOR_FRAME_ALL; item <= COLOR_FRAME_BLUE; item++ )
    	GCheckMenuItem ( menu, item, item == frame );
}

/***  BuildImageMenus  ***************************************************

	Rebuilds all menus which should contain titles of image windows.

	void BuildImageMenus ( void )
	
	This function should be called whenever an image window is created,
	destroyed, or renamed.
	
**************************************************************************/

void BuildImageMenus ( void )
{
	GMenuPtr	menu, parentMenu = GGetMainMenu ( PROCESS_MENU );
    
	menu = GGetSubMenu ( parentMenu, PROCESS_ADD_ITEM );
	AddImageMenuItems ( menu, ADD_MENU_ID, ARITHMETIC_IMAGE_SEPARATOR );
	
	menu = GGetSubMenu ( parentMenu, PROCESS_SUBTRACT_ITEM );
	AddImageMenuItems ( menu, SUBTRACT_MENU_ID, ARITHMETIC_IMAGE_SEPARATOR );

	menu = GGetSubMenu ( parentMenu, PROCESS_MULTIPLY_ITEM );
	AddImageMenuItems ( menu, MULTIPLY_MENU_ID, ARITHMETIC_IMAGE_SEPARATOR );

	menu = GGetSubMenu ( parentMenu, PROCESS_DIVIDE_ITEM );
	AddImageMenuItems ( menu, DIVIDE_MENU_ID, ARITHMETIC_IMAGE_SEPARATOR );

	menu = GGetSubMenu ( parentMenu, PROCESS_ALIGN_ITEM );
	AddImageMenuItems ( menu, DIVIDE_MENU_ID, 0 );

	menu = GGetSubMenu ( parentMenu, PROCESS_MOSAIC_ITEM );
	AddImageMenuItems ( menu, DIVIDE_MENU_ID, 0 );
}

/***  AddImageMenuItems  **************************************************

	Appends the titles of all image windows in the global image window
	list to a menu.

	void AddImageMenuItems ( GMenuPtr menu, short menuID, short lastItem )
	
	(menu):   pointer to menu to which image list should be appended.
	(menuID): menu ID number of the menu.
	(item):   number of item after which image list should be appended.
	
	On return, all items in the menu higher than (item) will be deleted
	and replaced with the titles of image windows in the global image
	window list.
	
***************************************************************************/

void AddImageMenuItems ( GMenuPtr menu, short menuID, short lastItem )
{
	char		*title;
	short		item;
	GWindowPtr	window;
	ImagePtr	image;
	
	/*** Remove existing menu items including or after the last non-image item. ***/
	
	for ( item = GGetNumMenuItems ( menu ); item > lastItem; item-- )
		GDeleteMenuItem ( menu, item );
		
	/*** Go thru the image window list, and for each window, add the window's
	     title to the menu. ***/
	     
	for ( item = lastItem + 1, window = GetFirstImageWindow(); window != NULL; window = GetNextImageWindow ( window ), item++ )
	{
		image = GetImageWindowImage ( window );
		title = GetImageTitle ( image );

		GInsertMenuItem ( menu, menuID, item, title );
	}
}

/*** BuildImagePopupMenu **********************************************************

	Inserts the titles of all open image windows into a popup menu control.
	
	void BuildImagePopupMenu ( GControlPtr control, GWindowPtr window )

	(control): pointer to popup menu control.
	(window):  pointer to image window which should be selected.
	
	If (window) is not a pointer to a valid image window, then the control value
	will be 1 (i.e. the selected item will be the first one).
	
	If (window) points to an image window, then the control value will be set
	to the item corresponding to the window's title.
	 
***********************************************************************************/

void BuildImagePopupMenu ( GControlPtr control, GWindowPtr selectedWindow )
{
	char		*title;
	short	 	item, selectedItem = 1;
	ImagePtr	image;
	GWindowPtr	window;
	
	for ( item = 1, window = GetFirstImageWindow(); window != NULL; window = GetNextImageWindow ( window ), item++ )
	{
		image = GetImageWindowImage ( window );
		title = GetImageTitle ( image );

		GInsertPopupMenuItem ( control, item, title );

		if ( window == selectedWindow )
			selectedItem = item;
	}

	GSetControlValue ( control, selectedItem );
}

