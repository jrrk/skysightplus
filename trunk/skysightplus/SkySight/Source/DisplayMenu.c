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

#define DISPLAY_MINIMUM_TEXT		4
#define DISPLAY_MAXIMUM_TEXT		6

/*** DoDisplayMenuItem ***/

void DoDisplayMenuItem ( long item )
{
	GWindowPtr window = GetActiveImageWindow();
	
	switch ( item )
	{
		case DISPLAY_ZOOM_IN_ITEM:
		case DISPLAY_ZOOM_OUT_ITEM:
			ZoomImageWindow ( item );
			break;
			
		case DISPLAY_CROP_ITEM:
			CropImageWindow ( window );
			break;
			
		case DISPLAY_OBJECTS_ITEM:
			SetImageWindowDisplayObjects ( window, ! GetImageWindowDisplayObjects ( window ) );
			GInvalidateWindow ( window, NULL );
			UpdateMenus();
			break;
			
		case DISPLAY_OBJECT_CENTROIDS_ITEM:
			SetImageWindowDisplayObjectCentroids ( window, ! GetImageWindowDisplayObjectCentroids ( window ) );
			GInvalidateWindow ( window, NULL );
			UpdateMenus();
			break;
			
		case DISPLAY_OBJECT_NAMES_ITEM:
			SetImageWindowDisplayObjectNames ( window, ! GetImageWindowDisplayObjectNames ( window ) );
			GInvalidateWindow ( window, NULL );
			UpdateMenus();
			break;
			
		case DISPLAY_FITS_HEADER_ITEM:
			if ( GetImageHeaderWindow ( GetImageWindowImage ( window ) ) == NULL )
				CreateHeaderWindow ( window );
			else
				GSetActiveWindow ( GetImageHeaderWindow ( GetImageWindowImage ( window ) ) );
			break;

		case DISPLAY_HISTOGRAM_ITEM:
			
			// if the active image window has no selected region, we should display a warning
			// message here telling the user to select one.
			
			if ( GetImageHistogramWindow ( GetImageWindowImage ( window ) ) == NULL )
				CreateHistogramWindow ( window );
			else
			{
				ImagePtr			image = GetImageWindowImage ( window );
				GWindowPtr			histogramWindow = GetImageHistogramWindow ( image );
				ImageHistogramPtr	histogram = GetHistogramWindowHistogram ( histogramWindow );
				PIXEL				minValue = GetImageHistogramMinimumValue ( histogram );
				PIXEL				maxValue = GetImageHistogramMaximumValue ( histogram );
				long				maxCount = GetImageHistogramPeakBinCount ( histogram );
				
				SetHistogramDisplayMinimumValue ( histogram, minValue );
				SetHistogramDisplayMaximumValue ( histogram, maxValue );
				SetHistogramDisplayMaximumCount ( histogram, maxCount );
				SetHistogramDisplayMinimumCount ( histogram, 0 );
				
				GInvalidateWindow ( histogramWindow, NULL );
				GSetActiveWindow ( histogramWindow );
			}
			break;
	}
}

/*** DoMagnificationMenuItem ***/

void DoMagnificationMenuItem ( long item )
{
	GWindowPtr	window;
	GRect		rect;
	
	window = GetActiveImageWindow();
	if ( window == NULL )
		return;

	switch ( item )
	{
		case MAGNIFICATION_1X_ITEM:
			SetImageWindowDisplayZoom ( window, 0 );
			break;
			
		case MAGNIFICATION_2X_ITEM:
			SetImageWindowDisplayZoom ( window, 1 );
			break;

		case MAGNIFICATION_4X_ITEM:
			SetImageWindowDisplayZoom ( window, 2 );
			break;

		case MAGNIFICATION_8X_ITEM:
			SetImageWindowDisplayZoom ( window, 3 );
			break;

		case MAGNIFICATION_16X_ITEM:
			SetImageWindowDisplayZoom ( window, 4 );
			break;

		case MAGNIFICATION_1_2X_ITEM:
			SetImageWindowDisplayZoom ( window, -1 );
			break;

		case MAGNIFICATION_1_4X_ITEM:
			SetImageWindowDisplayZoom ( window, -2 );
			break;

		case MAGNIFICATION_1_8X_ITEM:
			SetImageWindowDisplayZoom ( window, -3 );
			break;

		case MAGNIFICATION_1_16X_ITEM:
			SetImageWindowDisplayZoom ( window, -4 );
			break;
	}

	/*** Now the tricky part.  If the window's ideal or cropped size,
	     at the new zoom factor, fits within the application's workspace
	     area, then crop the window.  If not, only crop the window if
	     the window is not already maximized.  As a result, once the
	     window is maximized, its size, position, or centering will not
	     change as a result of the user changing the magnification. ***/
	     
	if ( GetImageWindowCroppedContentRect ( window, &rect ) )
	{
		CropImageWindow ( window );
	}
	else
	{
		if ( ! GIsWindowMaximized ( window ) )
			CropImageWindow ( window );
	}
	
	/*** Invalidate the entire window so it gets redrawn, then update
	     the menus. ***/
	     
	GInvalidateWindow ( window, NULL );
	UpdateMenus();
}

/*** DoColorTableMenuItem ******************************************************/

void DoColorTableMenuItem ( long item )
{
	GWindowPtr	window = NULL;
	ImagePtr	image = NULL;
	
	window = GetActiveImageWindow();
	if ( window == NULL )
		return;
	
	image = GetImageWindowImage ( window );
	if ( image == NULL )
		return;
		
	if ( GetImageType ( image ) != IMAGE_TYPE_MONOCHROME )
		return;
		
	if ( item < COLOR_TABLE_GRAYSCALE || item > COLOR_TABLE_BLUE )
		return;
		
	SetImageWindowColorTable ( window, item );
	SetImageDisplayDialogItems ( window );
	UpdateMenus();
}

/*** DoColorFrameMenuItem ******************************************************/

void DoColorFrameMenuItem ( long item )
{
	GWindowPtr	window = NULL;
	ImagePtr	image = NULL;
	
	window = GetActiveImageWindow();
	if ( window == NULL )
		return;
	
	image = GetImageWindowImage ( window );
	if ( image == NULL )
		return;
		
	if ( GetImageType ( image ) != IMAGE_TYPE_RGB_COLOR )
		return;
		
	if ( item < COLOR_FRAME_ALL || item > COLOR_FRAME_BLUE )
		return;
		
	SetImageWindowColorFrame ( window, item, TRUE );
	SetImageDisplayDialogItems ( window );
	UpdateMenus();
}

/*** DoMouseCoordinatesMenuItem ************************************************/

void DoMouseCoordinatesMenuItem ( long item )
{
	short		delta = 0;
	GWindowPtr	window = GetActiveImageWindow();

    if ( window == NULL )
    	return;

	/*** If we are changing the mouse coordinate display from "None" to
	     something other than none, we will need to enlarge the window frame;
	     similarly, we will need to shrink the window frame if we are changing
	     the mouse coordinate display from something other than "None" to "None". ***/
	     
	if ( GetImageWindowDisplayMouseCoordinates ( window ) == MOUSE_COORDINATES_NONE )
	{
		if ( item != MOUSE_COORDINATES_NONE )
			delta = GGetScrollBarWidth();
	}
	else
	{
		if ( item == MOUSE_COORDINATES_NONE )
			delta = -GGetScrollBarWidth();
	}
	
	/*** If we need to change the window's frame size, do so now; but don't
	     enlarge the window beyond the limits of the workspace. ***/
	
	if ( delta != 0 )
	{
		GRect	windowRect, workRect;
		
		GGetWindowFrameRect ( window, &windowRect );
		GGetWorkspaceRect ( &workRect );
		
		windowRect.bottom += delta;
		if ( windowRect.bottom <= workRect.bottom && ! GIsWindowMaximized ( window ) )
			GSetWindowFrameRect ( window, &windowRect );
	}
	
    SetImageWindowDisplayMouseCoordinates ( window, item );
    GInvalidateWindow ( window, NULL );
    UpdateMenus();
}

/*** ZoomImageWindow *********************************************************

void ZoomImageWindow ( long item )
{
	GWindowPtr	window;
	GRect		rect;
	short		zoom;
	
	window = GetActiveImageWindow();
	if ( window == NULL )
		return;
		
	zoom = GetImageWindowDisplayZoom ( window );
	
	switch ( item )
	{
		case DISPLAY_ZOOM_IN_ITEM:
			zoom++;
			if ( zoom > 4 )
				zoom = 4;
			break;
			
		case DISPLAY_ZOOM_OUT_ITEM:
			zoom--;
			if ( zoom < -4 )
				zoom = -4;
			break;
	}
	
	if ( zoom == GetImageWindowDisplayZoom ( window ) )
		return;
		
	SetImageWindowDisplayZoom ( window, zoom );
	CropImageWindow ( window );
	GetImageWindowDisplayRect ( window, &rect );
	GInvalidateWindow ( window, &rect );
	UpdateMenus();
}

*****/

void ZoomImageWindow ( long item )
{
	GWindowPtr	window;
	GRect		rect;
	short		zoom;
	
	window = GetActiveImageWindow();
	if ( window == NULL )
		return;
	
	/*** Get the image window's current zoom factor, then increment it
	     or decrement it by one, depending on the item that was selected.
	     Don't zoom in or out beyond a factor of 2^4 (i.e. 16). ***/
	     	
	zoom = GetImageWindowDisplayZoom ( window );
	
	switch ( item )
	{
		case DISPLAY_ZOOM_IN_ITEM:
			zoom++;
			if ( zoom > 4 )
				zoom = 4;
			break;
			
		case DISPLAY_ZOOM_OUT_ITEM:
			zoom--;
			if ( zoom < -4 )
				zoom = -4;
			break;
	}
	
	/*** If we didn' change the zoom factor (most likely, because it's
	     already at the maximum or minimum) return without doing anything
	     further.  Otherwise, set the imae window's display zoom factor
	     to the new value. ***/
		
	if ( zoom == GetImageWindowDisplayZoom ( window ) )
		return;
	
	SetImageWindowDisplayZoom ( window, zoom );
	
	/*** Now the tricky part.  If the window's ideal or cropped size,
	     at the new zoom factor, fits within the application's workspace
	     area, then crop the window.  If not, only crop the window if
	     the window is not already maximized.  As a result, once the
	     window is maximized, its size, position, or centering will not
	     change as a result of the user changing the magnification. ***/
	     
	if ( GetImageWindowCroppedContentRect ( window, &rect ) )
	{
		CropImageWindow ( window );
	}
	else
	{
		if ( ! GIsWindowMaximized ( window ) )
			CropImageWindow ( window );
	}
	
	/*** Invalidate the entire window so it gets redrawn, then update
	     the menus. ***/
	     
	GInvalidateWindow ( window, NULL );
	UpdateMenus();
}
