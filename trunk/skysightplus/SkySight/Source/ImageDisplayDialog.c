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

#define DISPLAY_REDRAW_BUTTON				3
#define DISPLAY_SCALING_AUTOMATIC_BUTTON	5
#define DISPLAY_SCALING_MANUAL_BUTTON		6
#define DISPLAY_SCALING_MINIMUM_SLIDER		8
#define DISPLAY_SCALING_MINIMUM_TEXT		9
#define DISPLAY_SCALING_MAXIMUM_SLIDER		11
#define DISPLAY_SCALING_MAXIMUM_TEXT		12
#define DISPLAY_COLOR_TABLE_TEXT			13
#define DISPLAY_COLOR_FRAME_TEXT			13
#define DISPLAY_COLOR_TABLE_MENU			14
#define DISPLAY_LIVE_REDRAW_BOX				15

/*** local data ***/

static int sLiveRedraw = TRUE;

/*** local functions ***/

static void DoImageDisplayScalingControlClick ( GWindowPtr, short, short );

/*** DoImageDisplayDialogEvent ************************************************

	Handles events related to items in the "Image Display" panel of the
	"Camera Control" dialog.
	
*******************************************************************************/

int DoImageDisplayDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int				respond = TRUE;
	short			value;
	char			text[256];
	float			displayMin, displayMax;
	GControlPtr		control;
	GWindowPtr		window;
	ImagePtr		image;
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
        	GSetWindowTitle ( GetCameraControlDialog(), "Image Display" );
            
        	control = GGetControl ( dialog, DISPLAY_SCALING_MINIMUM_SLIDER );
            GSetControlMin ( control, 0 );
            GSetControlMax ( control, 100 );

        	control = GGetControl ( dialog,  DISPLAY_SCALING_MAXIMUM_SLIDER );
            GSetControlMin ( control, 0 );
            GSetControlMax ( control, 100 );

			control = GGetControl ( dialog, DISPLAY_COLOR_TABLE_MENU );
			
			GSetDialogControlValue ( dialog, DISPLAY_LIVE_REDRAW_BOX, sLiveRedraw );

			GSetDialogDefaultButton ( dialog, DISPLAY_REDRAW_BUTTON );
			
			window = GetActiveImageWindow();
			SetImageDisplayDialogItems ( window );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				/*** If the user clicked the "Redraw" button, get the values entered
				     in the display scaling minimum and maximum text fields, and use
				     them as the image's display minimum and maximum.  Then update
				     the other image display dialog items to reflect these values,
				     redraw the image, and invalidate its window so it will get re-
				     displayed. ***/
				     
				case DISPLAY_REDRAW_BUTTON:
					GGetDialogItemText ( dialog, DISPLAY_SCALING_MINIMUM_TEXT, text );
					sscanf ( text, "%f", &displayMin );

					GGetDialogItemText ( dialog, DISPLAY_SCALING_MAXIMUM_TEXT, text );
					sscanf ( text, "%f", &displayMax );
					
					window = GetActiveImageWindow();
					SetImageWindowDisplayMin ( window, displayMin );
					SetImageWindowDisplayMax ( window, displayMax );
					SetImageDisplayDialogItems ( window );
					DrawImageWindowBitmap ( window );
					GInvalidateWindow ( window, NULL );
					break;
				
				/*** If the user changed the display scaling text fields, switch the
				     image's display mode from automatic to manual, but don't redraw
				     anything or update the other dialog items until the user clicks
				     the "Redraw" button. ***/
					
				case DISPLAY_SCALING_MINIMUM_TEXT:
				case DISPLAY_SCALING_MAXIMUM_TEXT:
					window = GetActiveImageWindow();
                    if ( window )
                    {
						SetImageWindowDisplayMode ( window, DISPLAY_MODE_MANUAL );
						GSetSelectedDialogButton ( dialog, DISPLAY_SCALING_AUTOMATIC_BUTTON,
						DISPLAY_SCALING_MANUAL_BUTTON, DISPLAY_SCALING_MANUAL_BUTTON );
					}
                    break;
				
				/*** If the user clicked either of the scaling slider controls, handle
				     the event elsewhere. ***/
				      	
				case DISPLAY_SCALING_MINIMUM_SLIDER:
				case DISPLAY_SCALING_MAXIMUM_SLIDER:
					DoImageDisplayScalingControlClick ( dialog, param1, param2 );
					break;
				
				/*** If the user clicked in the color table menu, set the active image
				     window's color table according to the item the user selects from
				     the menu (if the window contains a monochrome image); or set the
				     color frame displayed to the corresponding menu item (if the window
				     contains a color image). ***/
				     
				case DISPLAY_COLOR_TABLE_MENU:
					window = GetActiveImageWindow();
					value = GGetDialogControlValue ( dialog, param1 );
					image = GetImageWindowImage ( window );
					
					if ( GetImageType ( image ) == IMAGE_TYPE_MONOCHROME )
					{
						SetImageWindowColorTable ( window, value );
						UpdateColorTableMenu ( window );
					}
					
					if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
					{
						SetImageWindowColorFrame ( window, value, TRUE );
						UpdateColorFrameMenu ( window );
					}
					
					break;
				
				/*** If the user clicked in the "Live Redraw" check box, toggle the value
				     of the "Live Redraw" flag and reset the check box accordingly. ***/
					
				case DISPLAY_LIVE_REDRAW_BOX:
					sLiveRedraw = ! sLiveRedraw;
					GSetDialogControlValue ( dialog, DISPLAY_LIVE_REDRAW_BOX, sLiveRedraw );
					break;
				
				/*** If the user clicked on either of the display scaling mode radio button
				     controls, select the radio button which was clicked.  If the "Manual"
				     button was clicked, set the active image window's display scaling mode
				     to "Manual" and select the corresponding radio button, but don't redraw
				     anything.  If the user selected the "Automatic" radio button, do the
				     same, but also reset the image's display scaling range to its automatic
				     value, redraw the image, and invalidate its window so it gets redisplayed. ***/

				case DISPLAY_SCALING_AUTOMATIC_BUTTON:
				case DISPLAY_SCALING_MANUAL_BUTTON:
					window = GetActiveImageWindow();
					if ( param1 == DISPLAY_SCALING_MANUAL_BUTTON )
					{
						SetImageWindowDisplayMode ( window, DISPLAY_MODE_MANUAL );
						GSetSelectedDialogButton ( dialog, DISPLAY_SCALING_AUTOMATIC_BUTTON,
						DISPLAY_SCALING_MANUAL_BUTTON, param1 );
					}
					else
					{
						SetImageWindowDisplayMode ( window, DISPLAY_MODE_AUTOMATIC );
						ComputeImageDisplayRange ( window );
						SetImageDisplayDialogItems ( window );
						DrawImageWindowBitmap ( window );
						GInvalidateWindow ( window, NULL );
					}
					break;
				
			}
	}
	
    return ( respond );
}

#if ( ! BRIGHTNESS_CONTRAST_CONTROL )

/*** DoImageDisplayScalingControlClick **************************************************

	Handles events related to the image display scaling controls in the "Image Display"
	panel of the "Camera Control" dialog.

	void DoImageDisplayScalingControlClick ( GWindowPtr dialog, short controlID, short part )
	
	(dialog):    pointer to the dialog containing the control that generated the event.
	(controlID): ID number of the control that generated the event.
	(part):      part code indicating ehat part of the control was clicked.
	
	The function returns nothing.
	
	If the control part code indicates that the control's moveable "thumb" or "indicator"
	is being dragged, this function updates the display value in the text field adjacent
	to the control.  If the part code indicates that the thumb has been released and
	repositioned, the function also redraws the active image window's offscreen bitmap
	and invalidates its window, so that the image is actually re-displayed with the new
	scaling value selected by the user.
	
*****************************************************************************************/

void DoImageDisplayScalingControlClick ( GWindowPtr dialog, short controlID, short part )
{
	GWindowPtr		window = GetActiveImageWindow();
	ImagePtr		image = GetImageWindowImage ( window );
	GControlPtr		control = GGetControl ( dialog, controlID );
	short			min = GGetControlMin ( control );
	short			max = GGetControlMax ( control );
	PIXEL			value = GGetControlValue ( control );
	PIXEL			imageMin, imageMax;
	char			text[256];
	
	/*** Increment the value of the scroll bar by 1 or 10 units,
		 depending on what part of the scroll bar the user clicked. ***/
		 
	switch ( part )
	{
		case G_SCROLL_LINE_UP:
			GSetControlValue ( control, value - 1 );
			break;
			
		case G_SCROLL_LINE_DOWN:
			GSetControlValue ( control, value + 1 );
			break;
			
		case G_SCROLL_PAGE_UP:
			GSetControlValue ( control, value - 10 );
			break;
			
		case G_SCROLL_PAGE_DOWN:
			GSetControlValue ( control, value + 10 );
			break;
	}

	/*** Obtain the maximum and minimum image data values, which the maximum and minimum
	     of the slider controls represent.  Exception: if all the image data lies between
	     0 and 255, the sliders represent the full range of 0-255. ***/

	imageMin = GetImageMinimumValue ( image );
	imageMax = GetImageMaximumValue ( image );

	if ( imageMin >= 0 && imageMin <= 255 && imageMax >= 0 && imageMax <= 255 )
	{
		imageMin = 0;
		imageMax = 255;
	}

	/*** Now determine the image data value that corresponds to the scroll bar's new position,
	     and set the value of the adjacent text field accordingly.  ***/
         
	value = GGetControlValue ( control );
	value = imageMin + value * ( imageMax - imageMin ) / ( max - min );
	
	switch ( controlID )
	{
		case DISPLAY_SCALING_MINIMUM_SLIDER:
			SetImageWindowDisplayMin ( window, value );
			sprintf ( text, "%.2f", value );
			GSetDialogItemText ( dialog, DISPLAY_SCALING_MINIMUM_TEXT, text );
			break;
			
		case DISPLAY_SCALING_MAXIMUM_SLIDER:
			SetImageWindowDisplayMax ( window, value );
			sprintf ( text, "%.2f", value );
			GSetDialogItemText ( dialog, DISPLAY_SCALING_MAXIMUM_TEXT, text );
			break;
	}

	/*** If the image should be redrawn while the user is still tracking the
	     slider's indicator or "thumb", redraw and redisplay the image now.
	     Otherwise only redisplay the image if the control part code indicates
	     that something other than thumb-tracking is going on. ***/
	     
	if ( sLiveRedraw || part != G_TRACK_THUMB )
    {
		DrawImageWindowBitmap ( window );
		GInvalidateWindow ( window, NULL );
		GUpdateWindow ( window );
	}
	
	/*** Since the user has changed the image display, reset the display
	     mode from automatic to manual, and select the appropriate dialog
	     radio button accordingly. ***/

	if ( GetImageWindowDisplayMode ( window ) != DISPLAY_MODE_MANUAL )
	{
		SetImageWindowDisplayMode ( window, DISPLAY_MODE_MANUAL );
		GSetSelectedDialogButton ( dialog, DISPLAY_SCALING_AUTOMATIC_BUTTON,
		DISPLAY_SCALING_MANUAL_BUTTON, DISPLAY_SCALING_MANUAL_BUTTON );
	}
}

/*** SetImageDisplayDialogItems **************************************************

	Updates the image display parameters shown in the "Display" panel of the
	"Camera Control" dialog when it is active.
	
	void SetImageDisplayDialogItems ( GWindowPtr window )

	(window): pointer to image window whose display parameters should be shown.
	
	The function returns nothing.
	
	If the image window pointer (window) is valid, this function will set the
	values displayed in the image's minimum and maximum slider controls and
	text fields to reflect the image's actual display range; and color table
	popup menu to reflect the current color table.
	
	If the image window pointer is NULL, the function disables all of the
	dialog's controls and fills them with blank values.
	
	You should call this function every time the active image window changes.
	
**********************************************************************************/

void SetImageDisplayDialogItems ( GWindowPtr window )
{
	char		text[256];
	short		item, controlMin, controlMax, controlRange, controlValue;
	PIXEL		imageMin, imageMax, imageRange, displayMin, displayMax;
	GWindowPtr	dialog;
	GControlPtr	control, minScroll, maxScroll;
	ImagePtr	image = NULL;
	
	if ( GetCameraSubDialog ( &dialog ) != CAMERA_DISPLAY_PAGE_DIALOG )
		return;
			
	minScroll = GGetControl ( dialog, DISPLAY_SCALING_MINIMUM_SLIDER );
	maxScroll = GGetControl ( dialog, DISPLAY_SCALING_MAXIMUM_SLIDER );
	
	if ( window == NULL )
	{
		for ( item = DISPLAY_REDRAW_BUTTON; item <= DISPLAY_COLOR_TABLE_MENU; item++ )
			GEnableDialogItem ( dialog, item, FALSE );

		GEnableControl ( minScroll, FALSE );
		GEnableControl ( maxScroll, FALSE );
		GSetDialogItemText ( dialog, DISPLAY_SCALING_MINIMUM_TEXT, "" );
		GSetDialogItemText ( dialog, DISPLAY_SCALING_MAXIMUM_TEXT, "" );
		
		control = GGetControl ( dialog, DISPLAY_COLOR_TABLE_MENU );
		GEnableControl ( control, FALSE );
	}
	else
	{
		for ( item = DISPLAY_REDRAW_BUTTON; item <= DISPLAY_COLOR_TABLE_MENU; item++ )
			GEnableDialogItem ( dialog, item, TRUE );

		/*** select automatic or manual display mode button ***/
		
		if ( GetImageWindowDisplayMode ( window ) == DISPLAY_MODE_MANUAL )
		{
			GSetSelectedDialogButton ( dialog, DISPLAY_SCALING_AUTOMATIC_BUTTON,
			DISPLAY_SCALING_MANUAL_BUTTON, DISPLAY_SCALING_MANUAL_BUTTON );
		}
		else
		{
			GSetSelectedDialogButton ( dialog, DISPLAY_SCALING_AUTOMATIC_BUTTON,
			DISPLAY_SCALING_MANUAL_BUTTON, DISPLAY_SCALING_AUTOMATIC_BUTTON );
		}
		
		/*** enable minimum/maximum display level controls. ***/
		
		GEnableControl ( minScroll, TRUE );
		GEnableControl ( maxScroll, TRUE );
		
		image = GetImageWindowImage ( window );
		imageMin = GetImageMinimumValue ( image );
		imageMax = GetImageMaximumValue ( image );
		
		/*** If the maximum and minimum image data values are both between 0 and 255,
		     the maximum/minimum slider controls should let the user select any value
		     in the full range of 0-255.  We allow this by pretending that the image's
		     minimum and maximum data values (which the sliders represent) are actually
		     0 and 255. ***/
		
		if ( imageMin >= 0 && imageMin <= 255 && imageMax >= 0 && imageMax <= 255 )
		{
			imageMin = 0;
			imageMax = 255;
		}
		
		imageRange = imageMax - imageMin;
		
		displayMin = GetImageWindowDisplayMin ( window );
		displayMax = GetImageWindowDisplayMax ( window );

		/*** set value for display minimum control. ***/

		controlMin = GGetControlMin ( minScroll );
		controlMax = GGetControlMax ( minScroll );
		controlRange = controlMax - controlMin;

        if ( imageMax == imageMin )
        	controlValue = 0;
        else
			controlValue = controlMin + controlRange * ( displayMin - imageMin ) / imageRange;
		GSetControlValue ( minScroll, controlValue );

        /*** set value for display maximum control. ***/

		controlMin = GGetControlMin ( maxScroll );
		controlMax = GGetControlMax ( maxScroll );
		controlRange = controlMax - controlMin;

        if ( imageMax == imageMin )
        	controlValue = 0;
        else
			controlValue = controlMin + controlRange * ( displayMax - imageMin ) / imageRange;
			
		GSetControlValue ( maxScroll, controlValue );
		
		/*** display actual display minimum and maximum values in the
		     respective text fields. ***/
		     
		sprintf ( text, "%.2f", displayMin );
		GSetDialogItemText ( dialog, DISPLAY_SCALING_MINIMUM_TEXT, text );

		sprintf ( text, "%.2f", displayMax );
		GSetDialogItemText ( dialog, DISPLAY_SCALING_MAXIMUM_TEXT, text );

        /*** set value for color table popup menu button ***/
        
		control = GGetControl ( dialog, DISPLAY_COLOR_TABLE_MENU );
		GEnableControl ( control, TRUE );

        image = GetImageWindowImage ( window );
        if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
        {
        	GSetDialogItemText ( dialog, DISPLAY_COLOR_FRAME_TEXT, "Color Frame:" );
        	
			GHideControl ( control );
			for ( item = GGetControlMax ( control ); item > 0 ; item-- )
				GDeletePopupMenuItem ( control, item );

			GInsertPopupMenuItem ( control, 1, "Red + Green + Blue" );
			GInsertPopupMenuItem ( control, 2, "Red Only" );
			GInsertPopupMenuItem ( control, 3, "Green Only" );
			GInsertPopupMenuItem ( control, 4, "Blue Only" );

			GShowControl ( control );
			GSetControlValue ( control, GetImageWindowColorFrame ( window ) );
		}
		
		if ( GetImageType ( image ) == IMAGE_TYPE_MONOCHROME )
		{	
        	GSetDialogItemText ( dialog, DISPLAY_COLOR_TABLE_TEXT, "Color Table:" );

			GHideControl ( control );
			for ( item = GGetControlMax ( control ); item > 0 ; item-- )
				GDeletePopupMenuItem ( control, item );
				
			GInsertPopupMenuItem ( control, 1, "Grayscale" );
			GInsertPopupMenuItem ( control, 2, "Rainbow" );
			GInsertPopupMenuItem ( control, 3, "Flame" );
			GInsertPopupMenuItem ( control, 4, "Winter" );
			GInsertPopupMenuItem ( control, 5, "Spring" );
			GInsertPopupMenuItem ( control, 6, "Summer" );
			GInsertPopupMenuItem ( control, 7, "Fall" );
			GInsertPopupMenuItem ( control, 8, "Red" );
			GInsertPopupMenuItem ( control, 9, "Green" );
			GInsertPopupMenuItem ( control, 10, "Blue" );
			
			GShowControl ( control );
			GSetControlValue ( control, GetImageWindowColorTable ( window ) );
		}
	}		
}

#else

/*** DoImageDisplayScalingControlClick **************************************************

	Handles events related to the image display scaling controls in the "Image Display"
	panel of the "Camera Control" dialog.

	void DoImageDisplayScalingControlClick ( GWindowPtr dialog, short controlID, short part )
	
	(dialog):    pointer to the dialog containing the control that generated the event.
	(controlID): ID number of the control that generated the event.
	(part):      part code indicating ehat part of the control was clicked.
	
	The function returns nothing.
	
	If the control part code indicates that the control's moveable "thumb" or "indicator"
	is being dragged, this function updates the display value in the text field adjacent
	to the control.  If the part code indicates that the thumb has been released and
	repositioned, the function also redraws the active image window's offscreen bitmap
	and invalidates its window, so that the image is actually re-displayed with the new
	scaling value selected by the user.
	
*****************************************************************************************/

void DoImageDisplayScalingControlClick ( GWindowPtr dialog, short controlID, short part )
{
	GWindowPtr		window = GetActiveImageWindow();
	GControlPtr		control = GGetControl ( dialog, controlID );
	short			min = GGetControlMin ( control );
	short			max = GGetControlMax ( control );
	PIXEL			value = GGetControlValue ( control );
	PIXEL			imageMin, imageMax;
	char			text[256];
	
	/*** Increment the value of the scroll bar by 1 or 10 units,
		 depending on what part of the scroll bar the user clicked. ***/
		 
	switch ( part )
	{
		case G_SCROLL_LINE_UP:
			GSetControlValue ( control, value - 1 );
			break;
			
		case G_SCROLL_LINE_DOWN:
			GSetControlValue ( control, value + 1 );
			break;
			
		case G_SCROLL_PAGE_UP:
			GSetControlValue ( control, value - 10 );
			break;
			
		case G_SCROLL_PAGE_DOWN:
			GSetControlValue ( control, value + 10 );
			break;
	}

	value = (float) GGetControlValue ( control ) / max;
	
	switch ( controlID )
	{
		case DISPLAY_SCALING_MINIMUM_SLIDER:
			SetImageWindowDisplayMin ( window, value );
			sprintf ( text, "%.2f", value );
			GSetDialogItemText ( dialog, DISPLAY_SCALING_MINIMUM_TEXT, text );
			break;
			
		case DISPLAY_SCALING_MAXIMUM_SLIDER:
			SetImageWindowDisplayMax ( window, value );
			sprintf ( text, "%.2f", value );
			GSetDialogItemText ( dialog, DISPLAY_SCALING_MAXIMUM_TEXT, text );
			break;
	}

	/*** If the user is still actively dragging the scroll bar,
	     return without actually redrawing the window's image
	     bitmap yet. ***/
	     
	if ( sLiveRedraw || part != G_TRACK_THUMB )
    {
		DrawImageWindowBitmap ( window );
		GInvalidateWindow ( window, NULL );
		GUpdateWindow ( window );
	}
}

/*** SetImageDisplayDialogItems **************************************************

	Updates the image display parameters shown in the "Display" panel of the
	"Camera Control" dialog when it is active.
	
	void SetImageDisplayDialogItems ( GWindowPtr window )

	(window): pointer to image window whose display parameters should be shown.
	
	The function returns nothing.
	
	If the image window pointer (window) is valid, this function will set the
	values displayed in the image's minimum and maximum slider controls and
	text fields to reflect the image's actual display range; and color table
	popup menu to reflect the current color table.
	
	If the image window pointer is NULL, the function disables all of the
	dialog's controls and fills them with blank values.
	
	You should call this function every time the active image window changes.
	
**********************************************************************************/

void SetImageDisplayDialogItems ( GWindowPtr window )
{
	char		text[256];
	short		item, controlMin, controlMax, controlRange, controlValue;
	PIXEL		brightness, contrast;
	GWindowPtr	dialog;
	GControlPtr	control, minScroll, maxScroll;
	
	if ( GetCameraSubDialog ( &dialog ) != CAMERA_DISPLAY_PAGE_DIALOG )
		return;
			
	minScroll = GGetControl ( dialog, DISPLAY_SCALING_MINIMUM_SLIDER );
	maxScroll = GGetControl ( dialog, DISPLAY_SCALING_MAXIMUM_SLIDER );
	
	if ( window == NULL )
	{
		for ( item = DISPLAY_REDRAW_BUTTON; item <= DISPLAY_COLOR_TABLE_MENU; item++ )
			GEnableDialogItem ( dialog, item, FALSE );

		GEnableControl ( minScroll, FALSE );
		GEnableControl ( maxScroll, FALSE );
		GSetDialogItemText ( dialog, DISPLAY_SCALING_MINIMUM_TEXT, "" );
		GSetDialogItemText ( dialog, DISPLAY_SCALING_MAXIMUM_TEXT, "" );
		
		control = GGetControl ( dialog, DISPLAY_COLOR_TABLE_MENU );
		GEnableControl ( control, FALSE );
	}
	else
	{
		for ( item = DISPLAY_REDRAW_BUTTON; item <= DISPLAY_COLOR_TABLE_MENU; item++ )
			GEnableDialogItem ( dialog, item, TRUE );

		GEnableControl ( minScroll, TRUE );
		GEnableControl ( maxScroll, TRUE );
		
		brightness = GetImageWindowDisplayMin ( window );
		contrast = GetImageWindowDisplayMax ( window );

		/*** set value for brightness control ***/

		controlMin = GGetControlMin ( minScroll );
		controlMax = GGetControlMax ( minScroll );
		controlRange = controlMax - controlMin;
		controlValue = brightness * controlRange;
		GSetControlValue ( minScroll, controlValue );

        /*** set value for contrast control ***/

		controlMin = GGetControlMin ( maxScroll );
		controlMax = GGetControlMax ( maxScroll );
		controlRange = controlMax - controlMin;
		controlValue = contrast * controlRange;
		GSetControlValue ( maxScroll, controlValue );
		
		sprintf ( text, "%.2f", brightness );
		GSetDialogItemText ( dialog, DISPLAY_SCALING_MINIMUM_TEXT, text );

		sprintf ( text, "%.2f", contrast );
		GSetDialogItemText ( dialog, DISPLAY_SCALING_MAXIMUM_TEXT, text );

        /*** set value for color table popup menu button ***/
        
		control = GGetControl ( dialog, DISPLAY_COLOR_TABLE_MENU );
		GEnableControl ( control, TRUE );

        image = GetImageWindowImage ( window );
        if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
        {
        	GSetDialogItemText ( dialog, DISPLAY_COLOR_FRAME_TEXT, "Color Frame:" );
        	
			GHideControl ( control );
			for ( item = GGetControlMax ( control ); item > 0 ; item-- )
				GDeletePopupMenuItem ( control, item );

			GInsertPopupMenuItem ( control, 1, "Red + Green + Blue" );
			GInsertPopupMenuItem ( control, 2, "Red Only" );
			GInsertPopupMenuItem ( control, 3, "Green Only" );
			GInsertPopupMenuItem ( control, 4, "Blue Only" );

			GShowControl ( control );
			GSetControlValue ( control, GetImageWindowColorFrame ( window ) );
		}
		
		if ( GetImageType ( image ) == IMAGE_TYPE_MONOCHROME )
		{	
        	GSetDialogItemText ( dialog, DISPLAY_COLOR_TABLE_TEXT, "Color Table:" );

			GHideControl ( control );
			for ( item = GGetControlMax ( control ); item > 0 ; item-- )
				GDeletePopupMenuItem ( control, item );
				
			GInsertPopupMenuItem ( control, 1, "Grayscale" );
			GInsertPopupMenuItem ( control, 2, "Rainbow" );
			GInsertPopupMenuItem ( control, 3, "Flame" );
			GInsertPopupMenuItem ( control, 4, "Winter" );
			GInsertPopupMenuItem ( control, 5, "Spring" );
			GInsertPopupMenuItem ( control, 6, "Summer" );
			GInsertPopupMenuItem ( control, 7, "Fall" );
			GInsertPopupMenuItem ( control, 8, "Red" );
			GInsertPopupMenuItem ( control, 9, "Green" );
			GInsertPopupMenuItem ( control, 10, "Blue" );
			
			GShowControl ( control );
			GSetControlValue ( control, GetImageWindowColorTable ( window ) );
		}
	}		
}

#endif

GBitmapPtr DrawHistogramBitmap ( GWindowPtr imageWindow, GWindowPtr window, GRectPtr rect )
{
	ImagePtr	image = GetImageWindowImage ( imageWindow );
	GBitmapPtr 	bitmap;
	long		*histogram;
	long		numBins, bin, maxBin;
	short		x, y;
	
	bitmap = GCreateBitmap ( rect );
	if ( bitmap == NULL )
		return ( NULL );
		
	GStartDrawingBitmap ( window, bitmap );
	
	histogram = GetImageHistogram ( image, &numBins );
	
	for ( maxBin = bin = 0; bin < numBins; bin++ )
		if ( histogram[ bin ] > maxBin )
			maxBin = histogram[ bin ];
	
	for ( bin = 0; bin < numBins; bin++ )
	{
		x = bin * ( rect->right - rect->left ) / numBins;
		y = rect->bottom - histogram[ bin ] * ( rect->bottom - rect->top ) / maxBin;
		GDrawLine ( x, rect->bottom, x, y );
	}
	
	GEndDrawingBitmap ( window, bitmap );
	return ( bitmap );
}
