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

/*** local data ***/

static short sImageTool = EDIT_SELECT_TOOL;

static GWindowPtr sUndoWindow = NULL;

/***  DoEditMenuItem  ********************************************************/

void DoEditMenuItem ( long item )
{
	switch ( item )
	{
		case EDIT_UNDO_ITEM:
			DoUndo();
			break;
		
		case EDIT_COPY_ITEM:
			DoCopy ( GGetMenuEventWindow() );
			break;
				
		case EDIT_SELECT_ALL_ITEM:
			SelectAllOfImage ( GetActiveImageWindow() );
			break;
			
		case EDIT_SELECT_NONE_ITEM:
			SelectNoneOfImage ( GetActiveImageWindow() );
			break;
			
		case EDIT_SELECT_TOOL:
		case EDIT_DRAG_TOOL:
		case EDIT_ZOOM_IN_TOOL:
		case EDIT_ZOOM_OUT_TOOL:
		case EDIT_SHIFT_TOOL:
		case EDIT_MARK_TOOL:
			SetImageTool ( item );
			UpdateMenus();
			break;
	}
}

/*** DoEditColorFrameMenuItem *************************************************/

void DoEditColorFrameMenuItem ( long item )
{
	GWindowPtr window = GetActiveImageWindow();
	
	if ( window == NULL )
		return;
		
	switch ( item )
	{
		case COLOR_FRAME_ALL:
			SetImageWindowSelectedFrame ( window, ALL_FRAMES );
			break;
			
		case COLOR_FRAME_RED:
			SetImageWindowSelectedFrame ( window, RED_FRAME );
			break;

		case COLOR_FRAME_GREEN:
			SetImageWindowSelectedFrame ( window, GREEN_FRAME );
			break;

		case COLOR_FRAME_BLUE:
			SetImageWindowSelectedFrame ( window, BLUE_FRAME );
			break;
	}
}

/*** SetImageTool ***/

void SetImageTool ( short tool )
{
	sImageTool = tool;
}

/*** GetImageTool ***/

short GetImageTool ( void )
{
	return ( sImageTool );
}

/*** DoUndo ***/

void DoUndo ( void )
{
	
}

/*** GetUndoWindow ****************************************************************

	Returns a pointer to the window to which the "Undo" command will be applied.
	
	GWindowPtr GetUndoWindow ( void )
	
	The function returns a window pointer, or NULL if the undo command cannot be
	applied to any window.
	
***********************************************************************************/

GWindowPtr GetUndoWindow ( void )
{
	return ( sUndoWindow );
}

/*** PrepareUndo ******************************************************************

	Prepares a window so that the "Undo" command can later be applied to it.
	
	int PrepareUndo ( GWindowPtr window )
	
	(window): pointer to window which we are preparing for the "Undo" command.

	The function returns TRUE if successful, or FALSE on failure.
	
	You may pass NULL as the (window) argument to this function if you wish to
	disable the "Undo" command.  You should do this if the current undo window
	is about to be deleted, for example.
	
***********************************************************************************/

int PrepareUndo ( GWindowPtr window )
{
	ImagePtr image;
		
	if ( sUndoWindow != NULL )
	{
		image = GetImageWindowImage ( sUndoWindow );
		
		DeleteImagePreviousImage ( image );
	}
	
	sUndoWindow = window;
	
	if ( window != NULL )
	{
		image = GetImageWindowImage ( window );
		
		PrepareImagePreviousImage ( image );
	}
	
	return ( FALSE );
}

/*** DoCopy ***********************************************************************

	Handles the "Copy" command from the "Edit" menu.

	void DoCopy ( GWindowPtr window )
	
	(window): pointer to window which recieved the "Copy" command.
	
	The function returns nothing.
	
***********************************************************************************/

void DoCopy ( GWindowPtr window )
{
	if ( GGetWindowClass ( window ) == HISTOGRAM_WINDOW )
		CopyHistogramWindow ( window );
}

