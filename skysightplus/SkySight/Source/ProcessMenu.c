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

#define TRIM_SELECTION_BUTTON		3
#define TRIM_PIXELS_BUTTON			4
#define TRIM_LEFT_TEXT				6
#define TRIM_RIGHT_TEXT				8
#define TRIM_TOP_TEXT				10
#define TRIM_BOTTOM_TEXT			12

#define SHIFT_RIGHT_TEXT				4
#define SHIFT_DOWN_TEXT					7
#define SHIFT_AROUND_EDGES_BOX			10

#define SCALE_BY_FACTOR_BUTTON			3
#define SCALE_TO_SIZE_BUTTON			4
#define SCALE_FACTOR_HORIZONTAL_TEXT	6
#define SCALE_FACTOR_VERTICAL_TEXT		8
#define SCALE_IMAGE_WIDTH_TEXT			10
#define SCALE_IMAGE_HEIGHT_TEXT			12
#define SCALE_PROPORTIONALLY_BOX		13

#define RGB_COMBINATION_RED_MENU	4
#define RGB_COMBINATION_GREEN_MENU	6
#define RGB_COMBINATION_BLUE_MENU	8

#define RGB_BALANCE_RED_SLIDER		4
#define RGB_BALANCE_RED_TEXT		5
#define RGB_BALANCE_GREEN_SLIDER	7
#define RGB_BALANCE_GREEN_TEXT		8
#define RGB_BALANCE_BLUE_SLIDER		10
#define RGB_BALANCE_BLUE_TEXT		11

#define CLIP_OLD_MAXIMUM_SLIDER		4
#define CLIP_OLD_MAXIMUM_TEXT		5
#define CLIP_NEW_MAXIMUM_TEXT		7
#define CLIP_OLD_MINIMUM_SLIDER		9
#define CLIP_OLD_MINIMUM_TEXT		10
#define CLIP_NEW_MINIMUM_TEXT		12
#define CLIP_LIVE_UPDATE_BOX		13

/*** local functions ***/

static int DoRGBCombinationDialogEvent ( short, GWindowPtr, long, long );
static int DoRGBBalanceDialogEvent ( short, GWindowPtr, long, long );
static int DoTrimDialogEvent ( short, GWindowPtr, long, long );
static int DoScaleDialogEvent ( short, GWindowPtr, long, long );
static int DoShiftDialogEvent ( short, GWindowPtr, long, long );

static int DoClipDialogEvent ( short, GWindowPtr, long, long );

static double **CreateImageAlignmentMatrix ( ImagePtr, ImagePtr );
static int CreateImageAlignmentMatrices ( ImagePtr, ImagePtr, double ***, double *** );
static void TransformImageCoordinates ( double, double, double **, double *, double * );

static void  SetPixelSliderValue ( GControlPtr, PIXEL, PIXEL, PIXEL );
static PIXEL GetPixelSliderValue ( GControlPtr, PIXEL, PIXEL );

/*** local data ***/

static GWindowPtr	sRedImageWindow = NULL;
static GWindowPtr	sGreenImageWindow = NULL;
static GWindowPtr	sBlueImageWindow = NULL;

static short		sTrimSelection = FALSE;
static short		sTrimLeft = 0;
static short		sTrimRight = 0;
static short		sTrimTop = 0;
static short		sTrimBottom = 0;

static int			sShiftAroundEdges = FALSE;
static double		sShiftRight = 0.0;
static double		sShiftDown = 0.0;

static int			sScaleProportionally = TRUE;
static int			sScaleToSize = TRUE;
static short		sScaleImageWidth = 0;
static short		sScaleImageHeight = 0;
static double		sScaleFactorHorizontal = 1.0;
static double		sScaleFactorVertical = 1.0;

static double		sRotationAngle = 0.0;

/*** DoProcessMenuItem ***/

void DoProcessMenuItem ( long item )
{
	switch ( item )
	{
		case DISPLAY_ZOOM_IN_ITEM:
		case DISPLAY_ZOOM_OUT_ITEM:
			ZoomImageWindow ( item );
			break;
			
		case DISPLAY_CROP_ITEM:
			CropImageWindow ( GetActiveImageWindow() );
			break;
		
		case PROCESS_FLIP_HORIZONTALLY_ITEM:
		case PROCESS_FLIP_VERTICALLY_ITEM:
			FlipImageWindow ( GetActiveImageWindow(), item );
			break;
			
		case PROCESS_TRIM_ITEM:
			TrimImageWindow ( GetActiveImageWindow() );
			break;
		
		case PROCESS_SHIFT_ITEM:
			
			/*** Display the "Shift" dialog, which lets the user select the amount
			     by which the image should be shifted right and down, and whether or
			     not data should be wrapped.  If the user selects the dialog's "OK" button,
			     actually shift the image data. ***/
	
			if ( GEnterModalDialog ( SHIFT_DIALOG, 0, DoShiftDialogEvent ) == G_OK_BUTTON )
			{
				ShiftImageWindow ( GetActiveImageWindow(), sShiftRight, sShiftDown, sShiftAroundEdges );
			}
			break;
		
		case PROCESS_CLIP_ITEM:
			DoClip ( GetActiveImageWindow() );
			break;
					
		case PROCESS_SCALE_ITEM:
			ScaleImageWindow ( GetActiveImageWindow() );
			break;
			
		case PROCESS_RGB_COMBINE_ITEM:
			DoRGBCombination();
			break;
			
		case PROCESS_RGB_SEPARATE_ITEM:
			DoRGBSeparation ( GetActiveImageWindow() );
			break;
			
		case PROCESS_RGB_BALANCE_ITEM:
			DoRGBBalance ( GetActiveImageWindow() );
			break;
	}
}

/*** DoImageArithmeticMenuItem ***********************************************/

void DoImageArithmeticMenuItem ( long menuID, long item )
{
	char			text[256] = { 0 };
	double			constant = 0.0;
	GWindowPtr		window1 = NULL, window2 = NULL;
	
	window1 = GetActiveImageWindow();

	if ( item > ARITHMETIC_IMAGE_SEPARATOR )
	{
		window2 = GetNthImageWindow ( item - ARITHMETIC_IMAGE_SEPARATOR );
	}
	else
	{
		if ( GDoDialog ( "Constant:", text ) )
			sscanf ( text, "%lf", &constant );
		else
			return;
	}
	
	switch ( menuID )
	{
		case ADD_MENU_ID:
			DoImageWindowArithmetic ( IMAGE_ADDITION, window1, window2, constant );
			break;
			
		case SUBTRACT_MENU_ID:
			DoImageWindowArithmetic ( IMAGE_SUBTRACTION, window1, window2, constant );
			break;
			
		case MULTIPLY_MENU_ID:
			DoImageWindowArithmetic ( IMAGE_MULTIPLICATION, window1, window2, constant );
			break;
			
		case DIVIDE_MENU_ID:
			DoImageWindowArithmetic ( IMAGE_DIVISION, window1, window2, constant );
			break;
	}
}

/*** DoRotateMenuItem ******************************************************/

void DoRotateMenuItem ( long item )
{
	char		text[256];
	
	GWindowPtr window = GetActiveImageWindow();
	
	switch ( item )
	{
		case ROTATE_90_DEGREES:
			RotateImageWindow ( window, 90.0 );
			break;
			
		case ROTATE_180_DEGREES:
			RotateImageWindow ( window, 180.0 );
			break;

		case ROTATE_270_DEGREES:
			RotateImageWindow ( window, 270.0 );
			break;
			
		case ROTATE_OTHER:
			sprintf ( text, "%.3f", sRotationAngle );
			if ( GDoDialog ( "Angle to rotate image by (degrees):", text ) )
			{
				sRotationAngle = atof ( text );
				RotateImageWindow ( window, sRotationAngle );
			}
			break;
	}
}


/*** DoConvolveMenuItem ***/

void DoConvolveMenuItem ( long item )
{
	float **filter;
	short	width, height;
	GWindowPtr window;
	
	window = GetActiveImageWindow();
	filter = GetConvolutionFilter ( item, &width, &height );
	
	if ( window != NULL && filter != NULL )
	{
		ConvolveImageWindow ( window, filter, width, height );
		DeleteConvolutionFilter ( filter );
	}
}

/*** DoAlignMenuItem ***/

void DoAlignMenuItem ( long item )
{
	GWindowPtr window1 = NULL, window2 = NULL;
	
	window1 = GetActiveImageWindow();
	window2 = GetNthImageWindow ( item );
	
	if ( window1 == NULL || window2 == NULL )
		return;
		
	AlignImageWindow ( window1, window2 );
}

/*** DoMosaicMenuItem ***/

void DoMosaicMenuItem ( long item )
{
	GWindowPtr window1 = NULL, window2 = NULL;
	
	window1 = GetActiveImageWindow();
	window2 = GetNthImageWindow ( item );
	
	if ( window1 == NULL || window2 == NULL )
		return;
		
	MosaicImageWindow ( window1, window2 );
}

/*** FlipImageWindow **************************************************************

	Inverts an image window either horizontally or vertically, and redraws its
	contents.
	
	void FlipImageWindow ( GWindowPtr window, short item )
	
	(window): pointer to image window you wish to have inverted.
	(item):   menu item identitying which direction image should be inverted.
	
	The (item) parameter should be either IMAGE_FLIP_HORIZONTALLY_ITEM or
	IMAGE_FLIP_VERTICALLY_ITEM, depending on which way you want the image to be
	inverted.
	
***********************************************************************************/

void FlipImageWindow ( GWindowPtr window, short item )
{
	ImagePtr		image = GetImageWindowImage ( window );
	FITSImagePtr	oldFITS = NULL;
	PIXEL			***oldMatrix, *data;
	short			cols, rows, frames, col, row, frame;
	
	/*** Determine the width, height, and number of frames in the image. ***/

	cols = GetImageColumns ( image );
	rows = GetImageRows ( image );
	frames = GetImageFrames ( image );
	
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/
	     
	oldFITS = ResizeImageWindowImage ( window, cols, rows, FALSE );
	if ( oldFITS == NULL )
		return;
	else
		oldMatrix = oldFITS->data;
	
	/*** For each frame in the new image data matrix, copy the data from the
	     corresponding location in the previous image data matrix. ***/
	
	     
	for ( frame = 0; frame < frames; frame++ )
	{
		for ( row = 0; row < rows; row++ )
		{
			data = GetImageDataRow ( image, frame, row );
			for ( col = 0; col < cols; col++ )
			{
				if ( item == PROCESS_FLIP_HORIZONTALLY_ITEM )
					data[col] = oldMatrix[frame][row][ cols - col - 1 ];
					
				if ( item == PROCESS_FLIP_VERTICALLY_ITEM )
					data[col] = oldMatrix[frame][ rows - row - 1 ][col];
			}
		}
	}

	/*** Now release memory for the previous image data matrix.  De-select
	     any selected region of the image, since that region may no longer
	     exist. ***/
	     
	FreeFITSImage ( oldFITS );
	SetImageWindowSelectedRegion ( window, NULL );
	
	/*** Draw the image window's offscreen bitmap, resize the image's window
	     to fit the image, then invalidate the window so it gets redrawn. ***/
	     
	DrawImageWindowBitmap ( window );
	CropImageWindow ( window );
	GInvalidateWindow ( window, NULL );
}

/*** TrimImageWindow *************************************************************

	Displays the "Trim" dialog, which lets the user add or subtract pixels from
	the edges of the image.
	
**********************************************************************************/

void TrimImageWindow ( GWindowPtr window )
{
	ImagePtr		image = GetImageWindowImage ( window );
	short			oldCols = GetImageColumns ( image );
	short			oldRows = GetImageRows ( image );
	FITSImagePtr	oldFITS = NULL;
	PIXEL			***oldMatrix = NULL, *data, *oldData;
	short			cols, rows, frames, col, row, frame, oldRow, oldCol;
	short			left, top, right, bottom;
	
	/*** Display the "Trim" dialog, which lets the user select exactly how much
	     should be subtracted from or added to each side of the image.  If the user
	     hits the dialog's "Cancel" button, return without doing anything further. ***/
	
	if ( GEnterModalDialog ( TRIM_DIALOG, 0, DoTrimDialogEvent ) == G_CANCEL_BUTTON )
		return;
	
	/*** If the user selected the "Trim image to current selection" option, determine
	     the number of pixels between the edges of the image and the boundaries of the
	     current selected region. ***/
	
	if ( sTrimSelection )
	{
		ImageRegionPtr	region;
		
		region = GetImageWindowSelectedRegion ( window );
		if ( region == NULL )
			return;
			
		left = GetImageRegionLeft ( region );
		top = GetImageRegionTop ( region );
		right = GetImageColumns ( image ) - GetImageRegionRight ( region );
		bottom = GetImageRows ( image ) - GetImageRegionBottom ( region );
	}
	
	/*** Otherwise, use the actual pixel values the user entered into the dialog. ***/
	
	else
	{
		left = sTrimLeft;
		top = sTrimTop;
		right = sTrimRight;
		bottom = sTrimBottom;
	}
	
	/*** Determine the image's previous (i.e. pre-trimming) size, then figure its
	     new size.  If the new width or height of the image is less than zero, the
	     result of trimming the window would be to delete it, so ask the user if
	     they simply wish to close the window instead. ***/
	     
	oldCols = GetImageColumns ( image );
	oldRows = GetImageRows ( image );

	rows = oldRows - bottom - top;
	cols = oldCols - left - right;

	if ( rows < 1 || cols < 1 )
	{
		if ( GDoAlert ( G_WARNING_ALERT, G_OK_CANCEL_ALERT, "This will delete the entire image.  Do you want to continue?" ) == G_OK_BUTTON )
			GCloseWindow ( window );
		return;
	}
	
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/
	
	oldFITS = ResizeImageWindowImage ( window, cols, rows, FALSE );
	if ( oldFITS == NULL )
		return;
	else
		oldMatrix = oldFITS->data;
		
	/*** For each frame in the new image data matrix, copy the data from the
	     corresponding location in the previous image data matrix. ***/

	frames = GetImageFrames ( image );
	for ( frame = 0; frame < frames; frame++ )
	{
		for ( row = 0; row < rows; row++ )
		{
			oldRow = row + top;
			if ( oldRow >= 0 && oldRow <= oldRows )
			{
				data = GetImageDataRow ( image, frame, row );
				oldData = oldMatrix[frame][oldRow];
			
				for ( col = 0; col < cols; col++ )
				{
					oldCol = col + left;
					if ( oldCol >= 0 && oldCol <= oldCols )
						data[col] = oldData[ oldCol ];
				}
			}
		}
	}

	/*** Now release memory for the previous FITS image.  De-select any selected
	     region of the image, since that region may no longer exist. ***/

	FreeFITSImage ( oldFITS );
	SetImageWindowSelectedRegion ( window, NULL );
		
	/*** Draw the image window's offscreen bitmap, resize the image's window
	     to fit the image, then invalidate the window so it gets redrawn. ***/

	DrawImageWindowBitmap ( window );
	CropImageWindow ( window );
	GInvalidateWindow ( window, NULL );
}

/*** DoTrimDialogEvent ***/

int DoTrimDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int		respond = TRUE;
	char	text[256];
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
			sprintf ( text, "%hd", sTrimLeft );
			GSetDialogItemText ( dialog, TRIM_LEFT_TEXT, text );

			sprintf ( text, "%hd", sTrimRight );
			GSetDialogItemText ( dialog, TRIM_RIGHT_TEXT, text );
			
			sprintf ( text, "%hd", sTrimBottom );
			GSetDialogItemText ( dialog, TRIM_BOTTOM_TEXT, text );
			
			sprintf ( text, "%hd", sTrimTop );
			GSetDialogItemText ( dialog, TRIM_TOP_TEXT, text );

			if ( sTrimSelection )
				GSetSelectedDialogButton ( dialog, TRIM_SELECTION_BUTTON, TRIM_PIXELS_BUTTON, TRIM_SELECTION_BUTTON );
			else
				GSetSelectedDialogButton ( dialog, TRIM_SELECTION_BUTTON, TRIM_PIXELS_BUTTON, TRIM_PIXELS_BUTTON );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					if ( GGetSelectedDialogButton ( dialog, TRIM_SELECTION_BUTTON, TRIM_PIXELS_BUTTON ) == TRIM_SELECTION_BUTTON )
						sTrimSelection = TRUE;
					else
						sTrimSelection = FALSE;
						
					GGetDialogItemText ( dialog, TRIM_LEFT_TEXT, text );
					sTrimLeft = atoi ( text );
					
					GGetDialogItemText ( dialog, TRIM_RIGHT_TEXT, text );
					sTrimRight = atoi ( text );

					GGetDialogItemText ( dialog, TRIM_TOP_TEXT, text );
					sTrimTop = atoi ( text );

					GGetDialogItemText ( dialog, TRIM_BOTTOM_TEXT, text );
					sTrimBottom = atoi ( text );
					
					GExitModalDialogLoop ( param1 );
					break;
					
				case G_CANCEL_BUTTON:
					GExitModalDialogLoop ( param1 );
					break;
					
				case TRIM_SELECTION_BUTTON:
				case TRIM_PIXELS_BUTTON:
					GSetSelectedDialogButton ( dialog, TRIM_SELECTION_BUTTON, TRIM_PIXELS_BUTTON, param1 );
					break;
					
				case TRIM_LEFT_TEXT:
				case TRIM_RIGHT_TEXT:
				case TRIM_BOTTOM_TEXT:
				case TRIM_TOP_TEXT:
					GSetSelectedDialogButton ( dialog, TRIM_SELECTION_BUTTON, TRIM_PIXELS_BUTTON, TRIM_PIXELS_BUTTON );
					break;
			}
			break;
	}
	
	return ( respond );
}

/*** ShiftImageWindow ***************************************************************

	Displays the "Shift" dialog, which lets the user shift image data within an
	image.
	
*************************************************************************************/

void ShiftImageWindow ( GWindowPtr window, double right, double down, int wrap )
{
	ImagePtr		image = GetImageWindowImage ( window );
	FITSImagePtr	oldFITS = NULL;
	PIXEL			***oldMatrix = NULL, *data = NULL;
	short			cols, rows, frame0, frame1;
	double			oldRow, oldCol;
	short			frame, row, col;
	
//	PrepareUndo ( window );
	
	/*** Determine the image's current dimensions. ***/
	
	cols = GetImageColumns ( image );
	rows = GetImageRows ( image );
	
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/

	oldFITS = ResizeImageWindowImage ( window, cols, rows, TRUE );
	if ( oldFITS == NULL )
		return;
	else
		oldMatrix = oldFITS->data;
			
	/*** For each frame in the new image data matrix, copy the data from the
	     corresponding location in the previous image data matrix. ***/

	GetImageWindowSelectedFrame ( window, &frame0, &frame1 );
	
	for ( frame = frame0; frame <= frame1; frame++ )
	{
		for ( row = 0; row < rows; row++ )
		{
			data = GetImageDataRow ( image, frame, row );
			
			oldRow = row - down;
			
			if ( wrap )
			{
				if ( oldRow < 0 )
					oldRow = oldRow + rows;
					
				if ( oldRow >= rows )
					oldRow = oldRow - rows;
			}
			
			for ( col = 0; col < cols; col++ )
			{
				oldCol = col - right;
				
				if ( wrap )
				{
					if ( oldCol < 0 )
						oldCol = oldCol + cols;
						
					if ( oldCol >= cols )
						oldCol = oldCol - cols;
				}

				data[col] = InterpolatePixelValue ( oldMatrix[frame], cols, rows, oldCol, oldRow );
			}
		}
	}

	/*** Now release memory for the previous image data matrix.  De-select
	     any selected region of the image, since that region may no longer
	     exist. ***/

	FreeFITSImage ( oldFITS );
	SetImageWindowSelectedRegion ( window, NULL );
		
	/*** Perform any window updating that needs to be done, now that the
	     underlying image data has changed. ***/

	UpdateImage ( image );
}

/*** DoShiftDialogEvent ***/

int DoShiftDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int 			respond = TRUE;
	char			text[256];
	short			value;
		
	switch ( event )
	{
		case G_INITIAL_EVENT:
			sprintf ( text, "%.2f", sShiftRight );
			GSetDialogItemText ( dialog, SHIFT_RIGHT_TEXT, text );

			sprintf ( text, "%.2f", sShiftDown );
			GSetDialogItemText ( dialog, SHIFT_DOWN_TEXT, text );

			GSetDialogControlValue ( dialog, SHIFT_AROUND_EDGES_BOX, sShiftAroundEdges );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					GGetDialogItemText ( dialog, SHIFT_RIGHT_TEXT, text );
					sShiftRight = atof ( text );

					GGetDialogItemText ( dialog, SHIFT_DOWN_TEXT, text );
					sShiftDown = atof ( text );

					sShiftAroundEdges = GGetDialogControlValue ( dialog, SHIFT_AROUND_EDGES_BOX );
					GExitModalDialogLoop ( param1 );
					break;
					
				case G_CANCEL_BUTTON:
					GExitModalDialogLoop ( param1 );
					break;
					
				case SHIFT_AROUND_EDGES_BOX:
					value = ! GGetDialogControlValue ( dialog, param1 );
					GSetDialogControlValue ( dialog, param1, value );
					break;
			}
	
	}
	
	return ( respond );
}

/*** ScaleImageWindow ***************************************************************

	Displays the "Scale" dialog, which lets the user enlarge or reduce an image,
	either proportionally or non-proportionally.
	
*************************************************************************************/

void ScaleImageWindow ( GWindowPtr window )
{
	ImagePtr		image = GetImageWindowImage ( window );
	FITSImagePtr	oldFITS = NULL;
	PIXEL			***oldMatrix = NULL, *data = NULL;
	short			oldRows, oldCols;
	double			oldRow, oldCol;
	short			frames, frame, row, col;
	
	/*** Determine the image's current dimensions. ***/
	
	sScaleImageWidth = oldCols = GetImageColumns ( image );
	sScaleImageHeight = oldRows = GetImageRows ( image );
	
	/*** Display the "Scale" dialog, which lets the user select the factors by
	     which the image should be scaled in both the horizontal and vertical
	     directions, as well as the image's new dimensions.  If the user selects
	     the dialog's "Cancel" button, return without doing anything further. ***/
	
	if ( GEnterModalDialog ( SCALE_DIALOG, 0, DoScaleDialogEvent ) == G_CANCEL_BUTTON )
		return;
		
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/

	oldFITS = ResizeImageWindowImage ( window, sScaleImageWidth, sScaleImageHeight, FALSE );
	if ( oldFITS == NULL )
		return;
	else
		oldMatrix = oldFITS->data;
			
	/*** For each frame in the new image data matrix, copy the data from the
	     corresponding location in the previous image data matrix. ***/

	frames = GetImageFrames ( image );
	for ( frame = 0; frame < frames; frame++ )
	{
		for ( row = 0; row < sScaleImageHeight; row++ )
		{
			oldRow = row / sScaleFactorVertical;
			data = GetImageDataRow ( image, frame, row );
			for ( col = 0; col < sScaleImageWidth; col++ )
			{
				oldCol = col / sScaleFactorHorizontal;
				data[col] = InterpolatePixelValue ( oldMatrix[frame], oldCols, oldRows, oldCol, oldRow );
			}
		}
	}

	/*** Now release memory for the previous image data matrix.  De-select
	     any selected region of the image, since that region may no longer
	     exist. ***/

	FreeFITSImageDataMatrix ( oldMatrix );
	SetImageWindowSelectedRegion ( window, NULL );
		
	/*** Perform any window updating that needs to be done, now that the
	     underlying image data has changed. ***/

	CropImageWindow ( window );
	UpdateImage ( image );
}

/*** DoScaleDialogEvent ***/

int DoScaleDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int 			respond = TRUE;
	char			text[256];
	static int		proportional;
	static short	width, height;
	static double	horizontal, vertical;
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
			proportional = sScaleProportionally;

			if ( sScaleToSize )
				GSetSelectedDialogButton ( dialog, SCALE_BY_FACTOR_BUTTON, SCALE_TO_SIZE_BUTTON, SCALE_TO_SIZE_BUTTON );
			else
				GSetSelectedDialogButton ( dialog, SCALE_BY_FACTOR_BUTTON, SCALE_TO_SIZE_BUTTON, SCALE_BY_FACTOR_BUTTON );

			sprintf ( text, "%hd", sScaleImageWidth );
			GSetDialogItemText ( dialog, SCALE_IMAGE_WIDTH_TEXT, text );

			sprintf ( text, "%hd", sScaleImageHeight );
			GSetDialogItemText ( dialog, SCALE_IMAGE_HEIGHT_TEXT, text );

			sprintf ( text, "%.3f", sScaleFactorHorizontal );
			GSetDialogItemText ( dialog, SCALE_FACTOR_HORIZONTAL_TEXT, text );

			sprintf ( text, "%.3f", sScaleFactorVertical );
			GSetDialogItemText ( dialog, SCALE_FACTOR_VERTICAL_TEXT, text );

			GSetDialogControlValue ( dialog, SCALE_PROPORTIONALLY_BOX, proportional );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					GGetDialogItemText ( dialog, SCALE_FACTOR_HORIZONTAL_TEXT, text );
					sScaleFactorHorizontal = atof ( text );

					GGetDialogItemText ( dialog, SCALE_FACTOR_VERTICAL_TEXT, text );
					sScaleFactorVertical = atof ( text );

					GGetDialogItemText ( dialog, SCALE_IMAGE_WIDTH_TEXT, text );
					sScaleImageWidth = atoi ( text );

					GGetDialogItemText ( dialog, SCALE_IMAGE_HEIGHT_TEXT, text );
					sScaleImageHeight = atoi ( text );

					if ( GGetSelectedDialogButton ( dialog, SCALE_BY_FACTOR_BUTTON, SCALE_TO_SIZE_BUTTON ) == SCALE_TO_SIZE_BUTTON )
						sScaleToSize = TRUE;
					else
						sScaleToSize = FALSE;
					
					sScaleProportionally = GGetDialogControlValue ( dialog, SCALE_PROPORTIONALLY_BOX );
					GExitModalDialogLoop ( param1 );
					break;
					
				case G_CANCEL_BUTTON:
					GExitModalDialogLoop ( param1 );
					break;
					
				case SCALE_BY_FACTOR_BUTTON:
				case SCALE_TO_SIZE_BUTTON:
					GSetSelectedDialogButton ( dialog, SCALE_BY_FACTOR_BUTTON, SCALE_TO_SIZE_BUTTON, param1 );
					break;
				
				case SCALE_FACTOR_HORIZONTAL_TEXT:
					GSetSelectedDialogButton ( dialog, SCALE_BY_FACTOR_BUTTON, SCALE_TO_SIZE_BUTTON, SCALE_BY_FACTOR_BUTTON );
					
					GGetDialogItemText ( dialog, SCALE_FACTOR_HORIZONTAL_TEXT, text );
					horizontal = atof ( text );
					width = horizontal * sScaleImageWidth;
					
					if ( proportional )
					{
						vertical = horizontal;
						height = vertical * sScaleImageHeight;

						GSetDialogItemText ( dialog, SCALE_FACTOR_VERTICAL_TEXT, text );

						sprintf ( text, "%hd", height );
						GSetDialogItemText ( dialog, SCALE_IMAGE_HEIGHT_TEXT, text );
					}
					
					sprintf ( text, "%hd", width );
					GSetDialogItemText ( dialog, SCALE_IMAGE_WIDTH_TEXT, text );
					break;

				case SCALE_FACTOR_VERTICAL_TEXT:
					GSetSelectedDialogButton ( dialog, SCALE_BY_FACTOR_BUTTON, SCALE_TO_SIZE_BUTTON, SCALE_BY_FACTOR_BUTTON );
					GGetDialogItemText ( dialog, SCALE_FACTOR_VERTICAL_TEXT, text );
					vertical = atof ( text );
					height = vertical * sScaleImageHeight;
					
					if ( proportional )
					{
						horizontal = vertical;
						width = horizontal * sScaleImageWidth;

						GSetDialogItemText ( dialog, SCALE_FACTOR_HORIZONTAL_TEXT, text );

						sprintf ( text, "%hd", width );
						GSetDialogItemText ( dialog, SCALE_IMAGE_WIDTH_TEXT, text );
					}

					sprintf ( text, "%hd", height );
					GSetDialogItemText ( dialog, SCALE_IMAGE_HEIGHT_TEXT, text );
					break;
				
				case SCALE_IMAGE_WIDTH_TEXT:
					GSetSelectedDialogButton ( dialog, SCALE_BY_FACTOR_BUTTON, SCALE_TO_SIZE_BUTTON, SCALE_TO_SIZE_BUTTON );
					
					GGetDialogItemText ( dialog, SCALE_IMAGE_WIDTH_TEXT, text );
					width = atoi ( text );
					horizontal = (double) width / sScaleImageWidth;
					
					if ( proportional )
					{
						vertical = horizontal;
						height = vertical * sScaleImageHeight;

						sprintf ( text, "%.3f", vertical );
						GSetDialogItemText ( dialog, SCALE_FACTOR_VERTICAL_TEXT, text );
						
						sprintf ( text, "%hd", height );
						GSetDialogItemText ( dialog, SCALE_IMAGE_HEIGHT_TEXT, text );
					}
					
					sprintf ( text, "%.3f", horizontal );
					GSetDialogItemText ( dialog, SCALE_FACTOR_HORIZONTAL_TEXT, text );
					break;
					
				case SCALE_IMAGE_HEIGHT_TEXT:
					GSetSelectedDialogButton ( dialog, SCALE_BY_FACTOR_BUTTON, SCALE_TO_SIZE_BUTTON, SCALE_TO_SIZE_BUTTON );
					
					GGetDialogItemText ( dialog, SCALE_IMAGE_HEIGHT_TEXT, text );
					height = atoi ( text );
					vertical = (double) height / sScaleImageHeight;
					
					if ( proportional )
					{
						horizontal = vertical;
						width = horizontal * sScaleImageWidth;

						sprintf ( text, "%.3f", horizontal );
						GSetDialogItemText ( dialog, SCALE_FACTOR_HORIZONTAL_TEXT, text );

						sprintf ( text, "%hd", width );
						GSetDialogItemText ( dialog, SCALE_IMAGE_WIDTH_TEXT, text );
					}
					
					sprintf ( text, "%.3f", vertical );
					GSetDialogItemText ( dialog, SCALE_FACTOR_VERTICAL_TEXT, text );
					break;

				case SCALE_PROPORTIONALLY_BOX:
					proportional = ! GGetDialogControlValue ( dialog, param1 );
					GSetDialogControlValue ( dialog, param1, proportional );
					
					if ( proportional )
					{
						GGetDialogItemText ( dialog, SCALE_FACTOR_HORIZONTAL_TEXT, text );
						horizontal = atof ( text );
						
						GGetDialogItemText ( dialog, SCALE_IMAGE_WIDTH_TEXT, text );
						width = atoi ( text );
						
						vertical = horizontal;
						height = vertical * sScaleImageHeight;
						
						sprintf ( text, "%.3f", vertical );
						GSetDialogItemText ( dialog, SCALE_FACTOR_VERTICAL_TEXT, text );
						
						sprintf ( text, "%hd", height );
						GSetDialogItemText ( dialog, SCALE_IMAGE_HEIGHT_TEXT, text );
					}
					break;
			}
	
	}
	
	return ( respond );
}

/*** RotateImageWindow *************************************************************

	Rotates an image window counterclockwise by a certain angle.

	void RotateImageWindow ( GWindowPtr window, double angle )
	
	(window): pointer to image window
	(angle):  angle by which to rotate the image, in degrees.
	
************************************************************************************/

void RotateImageWindow ( GWindowPtr window, double angle )
{
	double			rotation[3][3];
	double			corners[4][3], vector[3], left, right, top, bottom;
	short			oldCols, oldRows, frames, width, height, col, row, frame, i;
	ImagePtr		image = GetImageWindowImage ( window );
	FITSImagePtr	oldFITS;
	PIXEL			***oldMatrix, *data;
	
	/*** Convert the angle to degree, then determine the image's current
	     dimensions and number of frames. ***/
	
	angle = angle / DEG_PER_RAD;
	oldCols = GetImageColumns ( image );
	oldRows = GetImageRows ( image );
	frames = GetImageFrames ( image );
	
	/*** Determine the coordinates of the image's corners.  Since the image
	     vertical coordinate (i.e. row number) increases downward, but the
	     rotation matrix math assumes that the vertical coordinate increases
	     upward, we need to negate the sign of the image's bottom coordinate. ***/
	     
	left = 0;
	top = 0;
	right = oldCols;
	bottom = -oldRows;
	
	corners[0][0] = left;  corners[0][1] = top;    corners[0][2] = 0;
	corners[1][0] = right; corners[1][1] = top;    corners[1][2] = 0;
	corners[2][0] = right; corners[2][1] = bottom; corners[2][2] = 0;
	corners[3][0] = left;  corners[3][1] = bottom; corners[3][2] = 0;
	
	/*** Now determine the rotation matrix which will transform coordinates
	     from the image's old coordinate system into the new one. ***/
	
	SetRotationMatrix ( rotation, 1, 2, angle );
	
	/*** For each of the 4 corners, convert the coordianates of the corner
	     from the image's old coordinate system into its new coordinate
	     system.  Then determine the leftmost, topmost, rightmost, and
	     bottommost coordinate among the transformed corners. ***/
	
	left = top = right = bottom = 0;
	
	for ( i = 0; i < 4; i++ )
	{
		TransformVector ( rotation, corners[i] );
		
		if ( corners[i][0] < left )
			left = corners[i][0];
			
		if ( corners[i][0] > right )
			right = corners[i][0];
			
		if ( corners[i][1] > top )
			top = corners[i][1];
			
		if ( corners[i][1] < bottom )
			bottom = corners[i][1];
	}
	
	/*** Compute the image's new width and height by taking the difference
	     between the right and left, and top and bottom, coordinates of the
	     image's rotated corners. ***/
	
	width = right - left;
	height = top - bottom;
	
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/

	oldFITS = ResizeImageWindowImage ( window, width, height, FALSE );
	if ( oldFITS == NULL )
		return;
	else
		oldMatrix = oldFITS->data;
		
	/*** Determine the rotation matrix which will transform coordinates
	     from the image's new coordinate system into the old one (this is
	     the opposite of the matrix we computed above). ***/

	SetRotationMatrix ( rotation, 1, 2, -angle );
			
	/*** For each frame in the new image data matrix, copy the data from the
	     corresponding location in the previous image data matrix. ***/

	for ( frame	= 0; frame < frames; frame++ )
	{
		for ( row = 0; row < height; row++ )
		{
			data = GetImageDataRow ( image, frame, row );
			for ( col = 0; col < width; col++ )
			{
				vector[0] = col + left;
				vector[1] = top - row;
				vector[2] = 0;
				
				TransformVector ( rotation, vector );
				data[col] = InterpolatePixelValue ( oldMatrix[frame], oldCols, oldRows, vector[0], -vector[1] );
			}
		}
	}

	/*** Now release memory for the previous image data matrix.  De-select
	     any selected region of the image, since that region may no longer
	     exist. ***/

	FreeFITSImage ( oldFITS );
	SetImageWindowSelectedRegion ( window, NULL );
		
	/*** Perform any window updating that needs to be done, now that the
	     underlying image data has changed. ***/

	CropImageWindow ( window );
	UpdateImage ( image );
}

/*** ConvolveImageWindow **********************************************************/

void ConvolveImageWindow ( GWindowPtr window, float **filter, short filterCols, short filterRows )
{
	ImagePtr		image = GetImageWindowImage ( window );
	PIXEL			***oldMatrix, *data;
	FITSImagePtr	oldFITS;
	short			rows, cols, frames, row, col, frame;
	short			oldLeft, oldTop, oldRight, oldBottom, oldRow, oldCol;
	short			filterRow, filterCol, filterCtrRow, filterCtrCol;
	float			value;
	
	frames = GetImageFrames ( image );
	rows   = GetImageRows ( image );
	cols   = GetImageColumns ( image );
	
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/

	oldFITS = ResizeImageWindowImage ( window, cols, rows, FALSE );
	if ( oldFITS == NULL )
		return;
	else
		oldMatrix = oldFITS->data;
		
	/*** Locate the central pixel of the filter. ***/
	
	filterCtrRow = filterRows / 2;
	filterCtrCol = filterCols / 2;
	
	for ( frame = 0; frame < frames; frame++ )
	{
		for ( row = 0; row < rows; row++ )
		{
			oldTop = row - filterCtrRow;
			oldBottom = oldTop + filterRows;
			
			data = GetImageDataRow ( image, frame, row );
			
			for ( col = 0; col < cols; col++ )
			{
				oldLeft = col - filterCtrCol;
				oldRight = oldLeft + filterCols;
				
				if ( oldLeft >= 0 && oldRight < cols && oldTop >= 0 && oldBottom < rows )
				{
					value = 0;
					
					for ( filterRow = 0, oldRow = oldTop; oldRow < oldBottom; filterRow++, oldRow++ )
						for ( filterCol = 0, oldCol = oldLeft; oldCol < oldRight; filterCol++, oldCol++ )
							value += filter[ filterRow ][ filterCol ] * oldMatrix[ frame ][ oldRow ][ oldCol ];

					data[col] = value;
				}
				else
				{
					data[col] = oldMatrix[frame][row][col];
				}		
			}
		}
	}
	
	/*** Now release memory for the previous image data matrix.  De-select
	     any selected region of the image, since that region may no longer
	     exist. ***/

	FreeFITSImage ( oldFITS );
	
	/*** Perform any window updating that needs to be done, now that the
	     underlying image data has changed. ***/

	UpdateImage ( image );
}

/*** GetConvolutionFilter ***/

float **GetConvolutionFilter ( short item, short *width, short *height )
{
	float	**filter = NULL;
	short	row, col;
	
	switch ( item )
	{
		case CONVOLVE_SMOOTH_SOFT:
			filter = NCreateMatrix ( sizeof ( float ), 3, 3 );
			if ( filter != NULL )
			{
				*width = 3;
				*height = 3;
				
				for ( row = 0; row < *height; row++ )
					for ( col = 0; col < *width; col++ )
						filter[row][col] = 1.0 / 9.0;
			}
			break;
			
		case CONVOLVE_SMOOTH_HARD:
			filter = NCreateMatrix ( sizeof ( float ), 5, 5 );
			if ( filter != NULL )
			{
				*width = 5;
				*height = 5;
				
				for ( row = 0; row < *height; row++ )
					for ( col = 0; col < *width; col++ )
						filter[row][col] = 1.0 / 25.0;
			}
			break;
		
		case CONVOLVE_SHARPEN_SOFT:
			filter = NCreateMatrix ( sizeof ( float ), 3, 3 );
			if ( filter != NULL )
			{
				*width = 3;
				*height = 3;
				
				for ( row = 0; row < *height; row++ )
					for ( col = 0; col < *width; col++ )
						filter[row][col] = -1.0;
						
				filter[1][1] = 10.0;
			}
			break;
		
		case CONVOLVE_SHARPEN_HARD:
			filter = NCreateMatrix ( sizeof ( float ), 5, 5 );
			if ( filter != NULL )
			{
				*width = 5;
				*height = 5;
				
				for ( row = 0; row < *height; row++ )
					for ( col = 0; col < *width; col++ )
						filter[row][col] = -1.0;
						
				filter[2][2] = 25.0;
			}
			break;
		
		case CONVOLVE_GRADIENT:
			filter = NCreateMatrix ( sizeof ( float ), 5, 5 );
			if ( filter != NULL )
			{
				*width = 5;
				*height = 5;

				filter[0][0] =  0; filter[0][1] =  0; filter[0][2] = -1; filter[0][3] =  0; filter[0][4] = 0;
				filter[1][0] =  0; filter[1][1] = -1; filter[1][2] = -1; filter[1][3] = -1; filter[1][4] = 0;
				filter[2][0] = -1; filter[2][1] = -1; filter[2][2] =  1; filter[2][3] =  1; filter[2][4] = 1;
				filter[3][0] =  0; filter[3][1] =  1; filter[3][2] =  1; filter[3][3] =  1; filter[3][4] = 0;
				filter[4][0] =  0; filter[4][1] =  0; filter[4][2] =  1; filter[4][3] =  0; filter[4][4] = 0;
			}
			break;
	}

	return ( filter );
}

/*** DeleteConvolutionFilter ***/

void DeleteConvolutionFilter ( float **filter )
{
	NDestroyMatrix ( filter );
}

/*** DoClip ***/

static PIXEL		sClipOldMax = 65535;
static PIXEL		sClipNewMax = 65535;
static PIXEL		sClipOldMin = 0;
static PIXEL		sClipNewMin = 0;
static FITSImagePtr	sClipOldFITS = NULL;
static FITSImagePtr	sClipNewFITS = NULL;

static int			sClipLivePreview = TRUE;
static PIXEL		sClipOldImageMax, sClipOldImageMin;

void DoClip ( GWindowPtr window )
{
	short		rows, cols;
	ImagePtr	image = GetImageWindowImage ( window );

	/*** Determine the image's current maximum and minimum values.
	     These will be used to scale the values displayed by the
	     sliders in the "Clip" dialog. ***/
	     
	sClipOldImageMax = GetImageMaximumValue ( image );
	sClipOldImageMin = GetImageMinimumValue ( image );
	
	/*** Determine the image's pixel dimensions. ***/
	
	cols = GetImageColumns ( image );
	rows = GetImageRows ( image );
	
	/*** Reallocate the image's FITS data matrix, save a pointer
	     to the old FITS data matrix, and then obtain a pointer
	     to the new one.  Note that one or the other of these FITS
	     structures will be freed inside the "Clip" dialog handling
	     code, depending on whether or not the user selects the "OK"
	     or "Cancel" button. ***/
	
	sClipOldFITS = ResizeImageWindowImage ( window, cols, rows, FALSE );
	sClipNewFITS = GetImageFITSImage ( image );
	
	/*** If we can't allocate memory for the new FITS image data matrix,
	     display a warning message and return. ***/
	     
	if ( sClipOldFITS == NULL )
	{
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		return;
	}
	
	/*** Display the "Clip" dialog, and let the user pick values
	     for the new image's maximum and minimum.  If the user hits
	     the "OK" button, update the image so all related displays
	     of data are modified to reflect the new image data values. ***/
	
	if ( GEnterModalDialog ( CLIP_DIALOG, (long) window, DoClipDialogEvent ) == G_OK_BUTTON )
		UpdateImage ( image );
}

/*** DoClipDialogEvent ***/

int DoClipDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	char			text[256];
	int				respond = TRUE;
	static double	oldMax, newMax, oldMin, newMin;
	GWindowPtr		window = (GWindowPtr) GGetWindowData ( dialog );
	ImagePtr		image = GetImageWindowImage ( window );
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
		
			oldMax = sClipOldMax;
			newMax = sClipNewMax;
			oldMin = sClipOldMin;
			newMin = sClipNewMin;
			
			sprintf ( text, "%.02f", oldMax );
			GSetDialogItemText ( dialog, CLIP_OLD_MAXIMUM_TEXT, text );

			sprintf ( text, "%.02f", newMax );
			GSetDialogItemText ( dialog, CLIP_NEW_MAXIMUM_TEXT, text );

			sprintf ( text, "%.02f", oldMin );
			GSetDialogItemText ( dialog, CLIP_OLD_MINIMUM_TEXT, text );

			sprintf ( text, "%.02f", newMin );
			GSetDialogItemText ( dialog, CLIP_NEW_MINIMUM_TEXT, text );
			
			SetPixelSliderValue ( GGetControl ( dialog, CLIP_OLD_MAXIMUM_SLIDER ), sClipOldImageMax, sClipOldImageMin, oldMax );
			SetPixelSliderValue ( GGetControl ( dialog, CLIP_OLD_MINIMUM_SLIDER ), sClipOldImageMax, sClipOldImageMin, oldMin );
			
			GSetDialogControlValue ( dialog, CLIP_LIVE_UPDATE_BOX, sClipLivePreview );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					sClipLivePreview = GGetDialogControlValue ( dialog, CLIP_LIVE_UPDATE_BOX );
				
					GGetDialogItemText ( dialog, CLIP_OLD_MAXIMUM_TEXT, text );
					sscanf ( text, "%lf", &oldMax );

					GGetDialogItemText ( dialog, CLIP_NEW_MAXIMUM_TEXT, text );
					sscanf ( text, "%lf", &newMax );

					GGetDialogItemText ( dialog, CLIP_OLD_MINIMUM_TEXT, text );
					sscanf ( text, "%lf", &oldMin );

					GGetDialogItemText ( dialog, CLIP_NEW_MINIMUM_TEXT, text );
					sscanf ( text, "%lf", &newMin );

					sClipOldMax = oldMax;
					sClipNewMax = newMax;
					sClipOldMin = oldMin;
					sClipNewMin = newMin;
					
					/*** Clip the image to the final values the user selected before hitting
					     the "OK" button, then free the old image data.  The image's bitmap
					     and all other displays of image data will be updated after the modal
					     dialog exits. ***/
					     
					ClipImageData ( sClipOldFITS, sClipNewFITS, sClipOldMax, sClipNewMax, sClipOldMin, sClipNewMin );
					FreeFITSImage ( sClipOldFITS );

					GExitModalDialogLoop ( param1 );
					break;
					
				case G_CANCEL_BUTTON:
				
					/*** If the user cancels, restore the saved pointer to the image's previous
					     data matrix.  Then free the image's new data matrix.  Update all visible
					     displays of image data, then exit the modal dialog loop. ***/
					
					SetImageFITSImage ( image, sClipOldFITS );
					FreeFITSImage ( sClipNewFITS );

					UpdateImage ( image );
					GExitModalDialogLoop ( param1 );
					break;
				
				case CLIP_LIVE_UPDATE_BOX:

					/*** If the user turns the "live update" checkbox off, restore the image data
					     to its previous state, then update all displays of image data and repaint
					     the image window.  Also invert the value of the checkbox control! ***/
					     
					if ( GGetDialogControlValue ( dialog, param1 ) )
					{
						GSetDialogControlValue ( dialog, param1, FALSE );
						ClipImageData ( sClipOldFITS, sClipNewFITS, sClipOldImageMax, sClipOldImageMax, sClipOldImageMin, sClipOldImageMin );
						UpdateImage ( image );
						GUpdateWindow ( window );
					}
					else
					{
						/*** If the user turns the "live update" checkbox on, clip the image data
						     using the current values entered in the dialog box.  Then update all
						     displays of image data and repaint the image window.  Don't forget to
						     invert the value of the checkbox control. ***/

						GSetDialogControlValue ( dialog, param1, TRUE );
						ClipImageData ( sClipOldFITS, sClipNewFITS, oldMax, newMax, oldMin, newMin );
						UpdateImage ( image );
						GUpdateWindow ( window );
					}
					break;
						
				case CLIP_OLD_MAXIMUM_TEXT:
				case CLIP_NEW_MAXIMUM_TEXT:
				case CLIP_OLD_MINIMUM_TEXT:
				case CLIP_NEW_MINIMUM_TEXT:
					GGetDialogItemText ( dialog, CLIP_OLD_MAXIMUM_TEXT, text );
					sscanf ( text, "%lf", &oldMax );

					GGetDialogItemText ( dialog, CLIP_NEW_MAXIMUM_TEXT, text );
					sscanf ( text, "%lf", &newMax );

					GGetDialogItemText ( dialog, CLIP_OLD_MINIMUM_TEXT, text );
					sscanf ( text, "%lf", &oldMin );

					GGetDialogItemText ( dialog, CLIP_NEW_MINIMUM_TEXT, text );
					sscanf ( text, "%lf", &newMin );
					
					SetPixelSliderValue ( GGetControl ( dialog, CLIP_OLD_MAXIMUM_SLIDER ), sClipOldImageMax, sClipOldImageMin, oldMax );
					SetPixelSliderValue ( GGetControl ( dialog, CLIP_OLD_MINIMUM_SLIDER ), sClipOldImageMax, sClipOldImageMin, oldMin );

					/*** Clip the image data using the values temporarily entered by the user.
					     Update all visible displays of image data, then redraw the image window
					     immediately. ***/
					
					if ( GGetDialogControlValue ( dialog, CLIP_LIVE_UPDATE_BOX ) )
					{     
						ClipImageData ( sClipOldFITS, sClipNewFITS, oldMax, newMax, oldMin, newMin );
						UpdateImage ( image );
						GUpdateWindow ( window );
					}
					break;
					
				case CLIP_OLD_MAXIMUM_SLIDER:
					oldMax = newMax = GetPixelSliderValue ( GGetControl ( dialog, CLIP_OLD_MAXIMUM_SLIDER ), sClipOldImageMax, sClipOldImageMin );

					sprintf ( text, "%.02f", oldMax );
					GSetDialogItemText ( dialog, CLIP_OLD_MAXIMUM_TEXT, text );
					GSetDialogItemText ( dialog, CLIP_NEW_MAXIMUM_TEXT, text );

					/*** Clip the image data using the values temporarily entered by the user.
					     Update all visible displays of image data, then redraw the image window
					     immediately. ***/
					
					if ( GGetDialogControlValue ( dialog, CLIP_LIVE_UPDATE_BOX ) )
					{     
						ClipImageData ( sClipOldFITS, sClipNewFITS, oldMax, newMax, oldMin, newMin );
						UpdateImage ( image );
						GUpdateWindow ( window );
					}
					break;
					
				case CLIP_OLD_MINIMUM_SLIDER:
					oldMin = newMin = GetPixelSliderValue ( GGetControl ( dialog, CLIP_OLD_MINIMUM_SLIDER ), sClipOldImageMax, sClipOldImageMin );

					sprintf ( text, "%.02f", oldMin );
					GSetDialogItemText ( dialog, CLIP_OLD_MINIMUM_TEXT, text );
					GSetDialogItemText ( dialog, CLIP_NEW_MINIMUM_TEXT, text );

					/*** Clip the image data using the values temporarily entered by the user.
					     Update all visible displays of image data, then redraw the image window
					     immediately. ***/
					
					if ( GGetDialogControlValue ( dialog, CLIP_LIVE_UPDATE_BOX ) )
					{     
						ClipImageData ( sClipOldFITS, sClipNewFITS, oldMax, newMax, oldMin, newMin );
						UpdateImage ( image );
						GUpdateWindow ( window );
					}
					break;
			}
			break;
	}

	return ( respond );
}

void SetPixelSliderValue ( GControlPtr control, PIXEL pixelMax, PIXEL pixelMin, PIXEL pixelValue )
{
	short controlMin = GGetControlMin ( control );
	short controlMax = GGetControlMax ( control );
	short controlValue;
	
	if ( pixelMax == pixelMin )
	{
		if ( pixelValue >= pixelMax )
			controlValue = controlMax;
		else
			controlValue = controlMin;
	}
	else
	{
		controlValue = controlMin +
		             ( controlMax - controlMin ) * ( pixelValue - pixelMin ) / ( pixelMax - pixelMin );
	}
	
	GSetControlValue ( control, controlValue );
}

PIXEL GetPixelSliderValue ( GControlPtr control, PIXEL pixelMax, PIXEL pixelMin )
{
	short controlMin = GGetControlMin ( control );
	short controlMax = GGetControlMax ( control );
	
	PIXEL pixelValue = pixelMin +
	                 ( pixelMax - pixelMin ) *
	                 ( GGetControlValue ( control ) - controlMin ) /
	                 ( controlMax - controlMin );
	                 
	return ( pixelValue );
}

/*** ClipImageData ****************************************************************

	Sets values in a FITS image matrix which are above or below a certain maximum
	minimum to a new maximum and minimum.
	
	void ClipImageData ( FITSImagePtr oldFITS, FITSImagePtr newFITS, PIXEL oldMax,
	     PIXEL newMax, PIXEL oldMin, PIXEL newMin )
	
	(oldFITS): pointer to source FITS image structure.
	(newFITS): pointer to destination FITS image structure.
	(oldMax):  maximum image data value in source image.
	(newMax):  maximum image data value in destination image.
	(oldMin):  minimum image data value in source image.
	(newMin):  minimum image data value in destination image.
	
	After this function has been called, values in the new image data matrix
	will be overwritten with the corresponding values in the old image data
	matrix, except where values in the old matrix exceed the limits specified
	by (oldMin) and (oldMax).  Values below (oldMin) will be replaced with
	(newMin), and values above (oldMax) will be replaced with (newMax).
	
	The paramters (oldFITS) and (newFITS) may point to the same FITS image
	structure.  In that case, data in that structure will be overwritten.

***********************************************************************************/

void ClipImageData ( FITSImagePtr oldFITS, FITSImagePtr newFITS, PIXEL oldMax, PIXEL newMax, PIXEL oldMin, PIXEL newMin )
{
	PIXEL			*oldData, *newData;
	short			rows, cols, frames, row, col, frame;
	
	frames = oldFITS->naxis3;
	rows   = oldFITS->naxis2;
	cols   = oldFITS->naxis1;
	
	for ( frame = 0; frame < frames; frame++ )
	{
		for ( row = 0; row < rows; row++ )
		{
			oldData = oldFITS->data[frame][row];
			newData = newFITS->data[frame][row];

			for ( col = 0; col < cols; col++ )
			{
				if ( oldData[col] > oldMax )
					newData[col] = newMax;
				else if ( oldData[col] < oldMin )
					newData[col] = newMin;
				else
					newData[col] = oldData[col];
			}
		}
	}
}

/*** AlignImageWindow ***************************************************************

	Moves the front ImageWindow's data so it becomes aligned with the image correspon-
	ding to (item) in the "Align" menu.  The function looks for photometry records that
	have the same name in both images to align by.  When it finds a pair, calculate
	their centroids. If there are less than 3 pairs,
	the transformation is assumed to be a simple translation.  If there at least 3,
	a least-squares matrix is computed to transfrom (col,row) coordinates in the
	other image to (col,row) coordinates in the front image.  New memory is allocated
	to hold the new image data and PixMap.  Image data is copied into the new buffer
	and PixMap from the old one from locations indicated by the matrix.
	
*************************************************************************************/
	
void AlignImageWindow ( GWindowPtr window1, GWindowPtr window2 )
{
	short			cols, rows, frames, col2, row2, frame2;
	PIXEL			***oldMatrix, *newDataRow;
	double			**matrix = NULL, x1, y1;
	ImagePtr		image1 = GetImageWindowImage ( window1 );
	ImagePtr		image2 = GetImageWindowImage ( window2 );
	ImageObjectPtr	object1 = NULL, object2 = NULL;
	FITSImagePtr	oldFITS;
	
	/*** If we're aligning an image to itself, there's nothing to do. ***/
	
	if ( image1 == image2 )
		return;

	/*** Create the transformation matrix which will transform coordinates from
	     image 2 to image 1. ***/
	     
	matrix = CreateImageAlignmentMatrix ( image2, image1 );
	if ( matrix == NULL )
	{
		GDoAlert ( G_ERROR_ALERT, G_OK_ALERT, "Can't create image alignment matrix!" );
		return;
	}
	
	/*** Reallocate image 1's image data matrix. ***/
	
	cols = GetImageColumns ( image1 );
	rows = GetImageRows ( image1 );
	frames = GetImageFrames ( image1 );

	oldFITS = ResizeImageWindowImage ( window1, cols, rows, FALSE );
	if ( oldFITS == NULL )
	{
		NDestroyMatrix ( matrix );
		GDoAlert ( G_ERROR_ALERT, G_OK_ALERT, "Can't allocate memory to align image." );
		return;
	}
	else
	{
		oldMatrix = oldFITS->data;
	}
	
	/*** Copy the image data into the new matrix from the old one. ***/
	
	for ( frame2 = 0; frame2 < frames; frame2++ )
	{
		for ( row2 = 0; row2 < rows; row2++ )
		{
			newDataRow = GetImageDataRow ( image1, frame2, row2 );
	
			for ( col2 = 0; col2 < cols; col2++ )
			{
				x1 = col2 * matrix[0][0] + col2 * matrix[1][0] + matrix[2][0];
				y1 = row2 * matrix[0][1] + row2 * matrix[1][1] + matrix[2][1];
				
				newDataRow[col2] = InterpolatePixelValue ( oldMatrix[frame2], cols, rows, x1, y1 );
			}
		}
	}
	
	/*** Now release memory for the previous image data matrix.  De-select
	     any selected region of the image, since that region may no longer
	     exist. ***/

	NDestroyMatrix ( matrix );
	FreeFITSImage ( oldFITS );
	SetImageWindowSelectedRegion ( window1, NULL );
		
	/*** Perform any window updating that needs to be done, now that the
	     underlying image data has changed. ***/

	UpdateImage ( image1 );
}

/*** MosaicImageWindow ***/

void MosaicImageWindow ( GWindowPtr window1, GWindowPtr window2 )
{
	int				i, in1, in2;
	short			width1, height1, col1, row1;
	short			width2, height2, col2, row2;
	short			left, top, right, bottom, width, height, col, row, frame;
	PIXEL			***data = NULL, ***data1 = NULL, ***data2 = NULL;
	PIXEL			value1, value2;
	double			**transformation = NULL, x1, y1, x2, y2;
	double			corners[4][2];
	ImagePtr		image1 = GetImageWindowImage ( window1 );
	ImagePtr		image2 = GetImageWindowImage ( window2 );
	ImageObjectPtr	object1 = NULL, object2 = NULL;
	FITSImagePtr	oldFITS1 = NULL;
	
	/*** If we're mosaicing an image with itself, there's nothing to do. ***/
	
	if ( image1 == image2 )
		return;

	/*** Create the transformation matrix which will transform coordinates from
	     image 2 to image 1. ***/
	     
	transformation = CreateImageAlignmentMatrix ( image2, image1 );
	if ( transformation == NULL )
	{
		GDoAlert ( G_ERROR_ALERT, G_OK_ALERT, "Can't create image alignment matrix!" );
		return;
	}
	
	/*** Determine the coordinates of the second image's corners. ***/
	     
	left = 0;
	top = 0;
	right = width2 = GetImageColumns ( image2 );
	bottom = height2 = GetImageRows ( image2 );
	
	corners[0][0] = left;  corners[0][1] = top;    corners[0][2] = 0;
	corners[1][0] = right; corners[1][1] = top;    corners[1][2] = 0;
	corners[2][0] = right; corners[2][1] = bottom; corners[2][2] = 0;
	corners[3][0] = left;  corners[3][1] = bottom; corners[3][2] = 0;
	
	/*** Now determine the coordinates of the first image's corners. ***/
	
	left = 0;
	top = 0;
	right = width1 = GetImageColumns ( image1 );
	bottom = height1 = GetImageRows ( image1 );
	
	/*** For each of the 4 corners, convert the coordianates of the corner
	     from the 2nd image's coordinate system into the 1st image's coordinate
	     system.  If the 2nd image's corners lie outside the boundaries of the
	     first image, expans the boundaries to include them. ***/

	for ( i = 0; i < 4; i++ )
	{
		TransformImageCoordinates ( corners[i][0], corners[i][1], transformation, &corners[i][0], &corners[i][1] );
		
		if ( corners[i][0] < left )
			left = corners[i][0];
			
		if ( corners[i][0] > right )
			right = corners[i][0];
			
		if ( corners[i][1] < top )
			top = corners[i][1];
			
		if ( corners[i][1] > bottom )
			bottom = corners[i][1];
	}
	
	/*** Compute the new image width and height by taking the difference
	     between the right and left, and top and bottom, coordinates of the
	     combined image's corners. ***/
	
	width = right - left;
	height = bottom - top;

	/*** Now reallocate the first image window's image data matrix so it can
	     hold the data from the combined images.  If we can't, then destroy the
	     transformation matrix, display an error message, and return. ***/
	     
	oldFITS1 = ResizeImageWindowImage ( window1, width, height, FALSE );
	if ( oldFITS1 == NULL )
	{
		NDestroyMatrix ( transformation );
		GDoAlert ( G_ERROR_ALERT, G_OK_ALERT, "Can't allocate memory to align image." );
		return;
	}
	else
	{
		data1 = oldFITS1->data;
	}
	
	/*** Create the transformation matrix which will transform coordinates from
	     image 1 to image 2. ***/
	     
	NDestroyMatrix ( transformation );
	transformation = CreateImageAlignmentMatrix ( image1, image2 );
	if ( transformation == NULL )
	{
		GDoAlert ( G_ERROR_ALERT, G_OK_ALERT, "Can't create image alignment matrix!" );
		return;
	}

	/*** Obtain pointers to the second image's data matrix, and to the first
	     image's newly reallocated data matrix. ***/
	     
	data2 = GetImageFITSImage ( image2 )->data;
	data  = GetImageFITSImage ( image1 )->data;
	
	/*** For each pixel in the new, combined image, copy the data from the
	     corresponding location in the other, separate images.  Average the
	     values from the other images where they overlap. ***/
	
	for ( frame = 0; frame < 1 /* deal with multi-frames later */; frame++ )
	{
		for ( row = 0; row < height; row++ )
		{
			for ( col = 0; col < width; col++ )
			{
				/*** Compute corresponding location in the first image.
				     If it lies within the first image's boundaries, determine
				     the first image's pixel value at that location. ***/
				
				row1 = row + top;
				col1 = col + left;
				
				if ( col1 >= 0 && col1 < width1 && row1 >= 0 && row1 < height1 )
				{
					value1 = data1[frame][row1][col1];
					in1 = TRUE;
				}
				else
				{
					in1 = FALSE;
				}
				
				/*** Compute corresponding location in the second image.
				     If it lies within the second image's boundaries, determine
				     the second image's pixel value at that location. ***/
								
				TransformImageCoordinates ( col1, row1, transformation, &x2, &y2 );
				
				if ( x2 >= 0 && x2 < width2 && y2 < height2 && y2 >= 0 )
				{
					value2 = InterpolatePixelValue ( data2[frame], width2, height2, x2, y2 );
					in2 = TRUE;
				}
				else
				{
					in2 = FALSE;
				}	
				
				/*** If the pixel lies in both images, average the values from
				     those images; otherwise, if the pixel lies in only one of the
				     images, take the value from the image in which it lies.  If the
				     pixel doesn't lie in either image, set its value to zero. ***/
				    
				if ( in1 )
				{
					if ( in2 )
						data[frame][row][col] = ( value1 + value2 ) / 2;
					else
						data[frame][row][col] = value1;
				}
				else
				{
					if ( in2 )
						data[frame][row][col] = value2;
					else
						data[frame][row][col] = 0;
				}
			}
		}
	}

	/*** Now release memory for the previous image data matrix.  De-select
	     any selected region of the image, since that region may no longer
	     exist. ***/

	NDestroyMatrix ( transformation );
	FreeFITSImage ( oldFITS1 );
	SetImageWindowSelectedRegion ( window1, NULL );
		
	/*** Perform any window updating that needs to be done, now that the
	     underlying image data has changed. ***/

	CropImageWindow ( window1 );
	UpdateImage ( image1 );
}

/*** CreateImageAlignmentMatrices ***/

int CreateImageAlignmentMatrices ( ImagePtr image1, ImagePtr image2, double ***b12, double ***b21 )
{
	char			*name1 = NULL, *name2 = NULL;
	int				result = FALSE;
	long			n = 0;
	double			x1[3], x2[3], **a12 = NULL, **a21 = NULL;
	ImageObjectPtr	object1 = NULL, object2 = NULL;

	/*** Allocate/initialize normal equation matrices.  On failure, display
	     a warning message, and return. ***/

	if ( NCreateNormalEqns ( 3, 2, &a12, b12 ) == FALSE )
	{
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		return ( result );
	}
	
	if ( NCreateNormalEqns ( 3, 2, &a21, b21 ) == FALSE )
	{
		NDestroyMatrix ( a12 );
		NDestroyMatrix ( *b12 );
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		return ( result );
	}

	/*** Find objects with common names in both images.  For each common object,
	     augment the normal equation matrices. ***/
	     
	n = 0;
	object1 = GetImageObjectList ( image1 );
	while ( object1 != NULL )
	{
		object2 = GetImageObjectList ( image2 );
		while ( object2 != NULL )
		{
			name1 = GetImageRegionName ( object1 );
			name2 = GetImageRegionName ( object2 );
			
			if ( strcmp ( name1, name2 ) == 0 )
			{
				x1[0] = GetImageObjectCentroidX ( object1 );
				x1[1] = GetImageObjectCentroidY ( object1 );
				x1[2] = 1.0;
				
				x2[0] = GetImageObjectCentroidX ( object2 );
				x2[1] = GetImageObjectCentroidY ( object2 );
				x2[2] = 1.0;
				
				NAugmentNormalEqns ( 3, 2, x1, x2, a12, *b12 );
				NAugmentNormalEqns ( 3, 2, x2, x1, a21, *b21 );
				
				n++;
			}

			object2 = GetNextImageObject ( object2 );
		}
		
		object1 = GetNextImageObject ( object1 );
	}
	
	/*** If we don't have any common objects, display a warning message and
	     return an error code. ***/
	     
	if ( n == 0 )
	{
		GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "No common marked objects to align by!" );
	}
	else
	{
		/*** If we have less than 3 stars to align by, we'll just assume that we need to do
		     a fixed translational shift. ***/
		     
		if ( n < 3 )
		{
			(*b12)[0][0] = 1.0;		      (*b12)[0][1] = 0.0;
			(*b12)[1][0] = 0.0;		      (*b12)[1][1] = 1.0;
			(*b12)[2][0] = x2[0] - x1[0]; (*b12)[2][1] = x2[1] - x1[1];
	
			(*b21)[0][0] = 1.0;		      (*b21)[0][1] = 0.0;
			(*b21)[1][0] = 0.0;		      (*b21)[1][1] = 1.0;
			(*b21)[2][0] = x1[0] - x2[0]; (*b21)[2][1] = x1[1] - x2[1];
			
			result = TRUE;
		}
		else
		{
			/*** Otherwise, we'll do a least-squares best fit. ***/
			
			result = NGaussJordanSolveMatrixEqn ( a12, 3, b12, 2 )
			      && NGaussJordanSolveMatrixEqn ( a21, 3, b21, 2 );
			      
			if ( result == FALSE )
				GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Unable to calculate alignment matrices!" );
		}
	}

	/*** If for whatever reason we couldn't compute the best-fit matrix,
	     destroy memory for the matrices we allocated. ***/
			
	if ( result == FALSE )
	{
		NDestroyMatrix ( *b12 );
		NDestroyMatrix ( *b21 );
	}
	
	/*** Destroy the covariance matrices, which we don't care about anyhow.
	     Then return a successful result code. ***/
	
	NDestroyMatrix ( a12 );
	NDestroyMatrix ( a21 );
	
	return ( result );
}

/*** CreateImageAlignmentMatrix *****************************************************

	Creates a matrix that will transform (x,y) coordinates in one image to (x,y)
	coordinates in another by using the centroid positions of objects with the
	same names in both images.

	double **CreateImageAlignmentMatrix ( ImagePtr image1, ImagePtr image2 )
	
	(image1): pointer to the first (source) image
	(image2): pointer to the second (destination) image
	
	If successful, the function will return a pointer to a matrix that transforms
	coordinates from the first image's coordinate system to the second.  On failure,
	the function returns NULL.
	
	The function works by searching for objects that have the same name in both
	images.  The type of matrix which the function creates depends on the number
	of common objects.
	
	If there is only one object with the same name, the function will create a
	transformation matrix which simply represents a fixed translation from the
	position of object in the first image to its corresponding position in the
	second.

	If there are two objects with matching names, the function will create a matrix
	which represents a rotation and scaling between the two objects.  NOT YET
	IMPLEMENTED.
	
	If there are three or more objects with matching names, the function attempts
	a least-squares best-fit solution of the matrix which transforms the positions
	of the objects in the first image to their positions in the second.  The function
	makes no attempt to determine the "correctness" of the result, i.e. if an
	object has the same name in both images but is in fact a different object,
	the funcion will still find a best-fit solution matrix, but that matrix will 
	represent an incorrect transformation.  NEED TO FIX THIS.
	
	The matrix returned by this function should be used in calls to
	TransformImageCoordinates().  When you are finished using the matrix, you
	should call NDestroyMatrix() to release memory for it.
	
*************************************************************************************/

double **CreateImageAlignmentMatrix ( ImagePtr image1, ImagePtr image2 )
{
	char			*name1 = NULL, *name2 = NULL;
	int				result = FALSE;
	long			n = 0;
	double			x1[3], x2[3], **a = NULL, **b = NULL;
	ImageObjectPtr	object1 = NULL, object2 = NULL;

	/*** Allocate/initialize normal equation matrices.  On failure, display
	     a warning message, and return. ***/

	if ( NCreateNormalEqns ( 3, 2, &a, &b ) == FALSE )
	{
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		return ( NULL );
	}
	
	/*** Find objects with common names in both images.  For each common object,
	     augment the normal equation matrices. ***/
	     
	n = 0;
	object1 = GetImageObjectList ( image1 );
	while ( object1 != NULL )
	{
		object2 = GetImageObjectList ( image2 );
		while ( object2 != NULL )
		{
			name1 = GetImageRegionName ( object1 );
			name2 = GetImageRegionName ( object2 );
			
			if ( strcmp ( name1, name2 ) == 0 )
			{
				x1[0] = GetImageObjectCentroidX ( object1 );
				x1[1] = GetImageObjectCentroidY ( object1 );
				x1[2] = 1.0;
				
				x2[0] = GetImageObjectCentroidX ( object2 );
				x2[1] = GetImageObjectCentroidY ( object2 );
				x2[2] = 1.0;
				
				NAugmentNormalEqns ( 3, 2, x1, x2, a, b );
				
				n++;
			}

			object2 = GetNextImageObject ( object2 );
		}
		
		object1 = GetNextImageObject ( object1 );
	}
	
	/*** If we don't have any common objects, display a warning message and
	     return an error code. ***/
	     
	if ( n == 0 )
	{
		GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "No common marked objects to align by!" );
	}
	else
	{
		/*** If we have less than 3 stars to align by, we'll just assume that we need to do
		     a fixed translational shift. ***/
		     
		if ( n < 3 )
		{
			b[0][0] = 1.0;              b[0][1] = 0.0;
			b[1][0] = 0.0;              b[1][1] = 1.0;
			b[2][0] = x2[0] - x1[0];    b[2][1] = x2[1] - x1[1];
			
			result = TRUE;
		}
		else
		{
			/*** Otherwise, we'll do a least-squares best fit. ***/
			
			result = NGaussJordanSolveMatrixEqn ( a, 3, b, 2 );
			      
			if ( result == FALSE )
				GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Unable to calculate alignment matrices!" );
		}
	}

	/*** Destroy the covariance matrices, which we don't care about anyhow.
	     Then return a successful result code. ***/
	
	NDestroyMatrix ( a );
	
	/*** If for whatever reason we couldn't compute the best-fit matrix,
	     destroy memory for the matrices we allocated. ***/
			
	if ( result == FALSE )
	{
		NDestroyMatrix ( b );
		return ( NULL );
	}
	
	return ( b );
}

/*** TransformImageCoordinates ****************************************************

	Transforms positions in one image coordinate system to another using a
	transformation matrix.
	
	void TransformImageCoordinates ( double x1, double y1, double **matrix,
	     double *x2, double *y2 )
	
	(x1,y1):  position in first image coordinate system.
	(matrix): image coordinate transformation matrix.
	(x2,y2):  recieves position in second image coordinate system.
	
	The function returns nothing.  The image coordinate transformation matrix
	passed to this function must contain three rows and two columns.  Suitable
	matrices are created by the functions CreateImageAlignmentMatrix() and
	CreateImageTransformationMatrix().
	
***********************************************************************************/

void TransformImageCoordinates ( double x1, double y1, double **matrix, double *x2, double *y2 )
{
	*x2 = x1 * matrix[0][0] + x1 * matrix[1][0] + matrix[2][0];
	*y2 = y1 * matrix[0][1] + y1 * matrix[1][1] + matrix[2][1];
}

/*** DoRGBCombination ************************************************************/

void DoRGBCombination ( void )
{
	short		width, height, frame, col, row;
	ImagePtr	rgbImage, image, redImage, greenImage, blueImage;
	GWindowPtr	rgbWindow;
	PIXEL		*data, *rgbData;
	
	if ( GEnterModalDialog ( RGB_COMBINATION_DIALOG, 0, DoRGBCombinationDialogEvent ) == G_OK_BUTTON )
	{
		redImage   = GetImageWindowImage ( sRedImageWindow );
		greenImage = GetImageWindowImage ( sGreenImageWindow );
		blueImage  = GetImageWindowImage ( sBlueImageWindow );
		
		width  = GetImageColumns ( redImage );
		height = GetImageRows ( redImage );
		
		if ( GetImageType ( redImage ) != IMAGE_TYPE_MONOCHROME )
		{
			GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "Can't make RGB combination out of a non-monochrome image!" );
			return;
		}

		if ( GetImageType ( greenImage ) != IMAGE_TYPE_MONOCHROME )
		{
			GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "Can't make RGB combination out of a non-monochrome image!" );
			return;
		}

		if ( GetImageType ( blueImage ) != IMAGE_TYPE_MONOCHROME )
		{
			GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "Can't make RGB combination out of a non-monochrome image!" );
			return;
		}

		if ( GetImageColumns ( greenImage ) != width || GetImageRows ( greenImage ) != height )
		{
			GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "Can't make RGB combination out of different-sized images!" );
			return;
		}
		
		if ( GetImageColumns ( blueImage ) != width || GetImageRows ( blueImage ) != height )
		{
			GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "Can't make RGB combination out of different-sized images!" );
			return;
		}

		rgbImage = NewImage ( "New Image", IMAGE_TYPE_RGB_COLOR, 3, height, width );
		if ( rgbImage == NULL )
		{
			WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
			return;
		}
		
		rgbWindow = NewImageWindow ( rgbImage );
		if ( rgbWindow == NULL )
		{
			DeleteImage ( rgbImage );
			WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
			return;
		}

		for ( frame = 0; frame < 3; frame++ )
		{
			if ( frame == 0 )
				image = redImage;
			else if ( frame == 1 )
				image = greenImage;
			else if ( frame == 2 )
				image = blueImage;

			for ( row = 0; row < height; row++ )
			{
				data = GetImageDataRow ( image, 0, row );	
				rgbData = GetImageDataRow ( rgbImage, frame, row );
				
				for ( col = 0; col < width; col++ )
					rgbData[ col ] = data[ col ];
			}
		}
		
	    ComputeImageDisplayRange ( rgbWindow );
		DrawImageWindowBitmap ( rgbWindow );
		SetImageWindowNeedsSave ( rgbWindow, TRUE );
	    CropImageWindow ( rgbWindow );
	    GSetActiveWindow ( rgbWindow );
	}
}

/*** DoRGBSeparation ************************************************************/

void DoRGBSeparation ( GWindowPtr rgbWindow )
{
	char		*rgbTitle, title[256];
	short		row, col, frame, rows, columns;
	PIXEL		*data, *rgbData, min, max;
	ImagePtr 	image = NULL, rgbImage = NULL;
	GWindowPtr	window = NULL;
	
	if ( rgbWindow == NULL )
		return;
		
	rgbImage = GetImageWindowImage ( rgbWindow );
	rgbTitle = GetImageTitle ( rgbImage );
	
	if ( GetImageType ( rgbImage ) != IMAGE_TYPE_RGB_COLOR )
	{
		GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "SkySight can't do RGB color separation because this is not an RGB color image." );
	}
	
	rows    = GetImageRows ( rgbImage );
	columns = GetImageColumns ( rgbImage );

	min     = GetImageWindowDisplayMin ( rgbWindow );
	max     = GetImageWindowDisplayMax ( rgbWindow );
	
	for ( frame = 0; frame < 3; frame++ )
	{
		if ( frame == 0 )
			sprintf ( title, "%s (Red)", rgbTitle );
		else if ( frame == 1 )
			sprintf ( title, "%s (Green)", rgbTitle );
		else if ( frame == 2 )
			sprintf ( title, "%s (Blue)", rgbTitle );
		
		image = NewImage ( title, IMAGE_TYPE_MONOCHROME, 1, rows, columns );
		if ( image == NULL )
		{
			WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
			break;
		}
		
		for ( row = 0; row < rows; row++ )
		{
			rgbData = GetImageDataRow ( rgbImage, frame, row );
			data    = GetImageDataRow ( image, 0, row );
			
			for ( col = 0; col < columns; col++ )
				data[col] = rgbData[col];
		}
		
		window = NewImageWindow ( image );
		if ( window == NULL )
		{
			DeleteImage ( image );
			WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
			break;
		}
		
		if ( frame == 0 )
			SetImageWindowColorTable ( window, COLOR_TABLE_RED );
		else if ( frame == 1 )
			SetImageWindowColorTable ( window, COLOR_TABLE_GREEN );
		else if ( frame == 2 )
			SetImageWindowColorTable ( window, COLOR_TABLE_BLUE );

		SetImageWindowDisplayMode ( window, DISPLAY_MODE_MANUAL );
		SetImageWindowDisplayMin ( window, min );
		SetImageWindowDisplayMax ( window, max );
	    ComputeImageDisplayRange ( window );
		DrawImageWindowBitmap ( window );
		SetImageWindowNeedsSave ( window, TRUE );
	    CropImageWindow ( window );
	    GSetActiveWindow ( window );
	}
}

/*** DoRGBCombinationDialogEvent ************************************************************/

int DoRGBCombinationDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int			respond = TRUE;
	short		item;
	GControlPtr control;
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
			control = GGetControl ( dialog, RGB_COMBINATION_RED_MENU );
			BuildImagePopupMenu ( control, sRedImageWindow );
			
			control = GGetControl ( dialog, RGB_COMBINATION_GREEN_MENU );
			BuildImagePopupMenu ( control, sGreenImageWindow );

			control = GGetControl ( dialog, RGB_COMBINATION_BLUE_MENU );
			BuildImagePopupMenu ( control, sBlueImageWindow );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					item = GGetDialogControlValue ( dialog, RGB_COMBINATION_RED_MENU );
					sRedImageWindow = GetNthImageWindow ( item );
					
					item = GGetDialogControlValue ( dialog, RGB_COMBINATION_GREEN_MENU );
					sGreenImageWindow = GetNthImageWindow ( item );

					item = GGetDialogControlValue ( dialog, RGB_COMBINATION_BLUE_MENU );
					sBlueImageWindow = GetNthImageWindow ( item );

					GExitModalDialogLoop ( param1 );
					break;
					
				case G_CANCEL_BUTTON:
					GExitModalDialogLoop ( param1 );
					break;
			}
			break;
	}
	
	return ( respond );
}

/*** DoRGBBalance ****************************************************************

	Displays the "RGB Balance" dialog, which lets the user adjust the relative
	intensities of the red, green, and blue components of the image.
	
**********************************************************************************/

static GWindowPtr	sRGBBalanceWindow;

void DoRGBBalance ( GWindowPtr window )
{
	ImagePtr	image = GetImageWindowImage ( window );
	short		cols, rows, frames, col, row, frame;
	PIXEL		*data = NULL;
	double		redBalance, greenBalance, blueBalance;
	
	/*** Save a pointer to the image window whose RGB color balance we are about
	     to manipulate.  Initially, set the RGB adjustment factors to 1.0 so that
	     when the "RGB Balance" dialog is first displayed, the image is shown as
	     it is currently, un-adjusted. ***/
	
	sRGBBalanceWindow = window;
	SetRGBBalance ( 1.0, 1.0, 1.0 );
	
	/*** Now display the dialog.  If the user clicked the "Cancel" button, re-draw
	     the image window's  bitmap and invalidate the window, since the user may have
	     changed them while the dialog was displayed. ***/
	     
	if ( GEnterModalDialog ( RGB_BALANCE_DIALOG, 0, DoRGBBalanceDialogEvent ) == G_CANCEL_BUTTON )
	{
		SetRGBBalance ( 1.0, 1.0, 1.0 );
		DrawImageWindowBitmap ( window );
		GInvalidateWindow ( window, NULL );
		return;
	}
	
	/*** Obtain the RGB correction factors selected by the user before re-setting
	     them to 1.0. ***/
	     
	GetRGBBalance ( &redBalance, &greenBalance, &blueBalance );
	SetRGBBalance ( 1.0, 1.0, 1.0 );

	/*** Determine the width, height, and number of frames in the image. ***/
	
	cols = GetImageColumns ( image );
	rows = GetImageRows ( image );
	frames = GetImageFrames ( image );

	/*** For each frame, scale the image data values by the appropriate factor. ***/
	
	for ( frame = 0; frame < frames; frame++ )
	{
		for ( row = 0; row < rows; row++ )
		{
			data = GetImageDataRow ( image, frame, row );
			
			/*** The 0th, 3rd, 6th, etc. frames contain the red component of the
			     image.  Multiply them by the red balancing factor. ***/
			     
			if ( frame % 3 == 0 )
			{
				for ( col = 0; col < cols; col++ )
					data[col] *= redBalance;
			}

			/*** The 1st, 4th, 7th, etc. frames contain the green component of the
			     image.  Multiply them by the green balancing factor. ***/

			else if ( frame % 3 == 1 )
			{
				for ( col = 0; col < cols; col++ )
					data[col] *= greenBalance;
			}

			/*** The 2st, 5th, 8th, etc. frames contain the blue component of the
			     image.  Multiply them by the blue balancing factor. ***/

			else if ( frame % 3 == 2 )
			{
				for ( col = 0; col < cols; col++ )
					data[col] *= blueBalance;
			}			
		}
	}

	/*** Now redraw the image window's offscreen bitmap, and invalidate it so that
	     it gets updated. ***/
	     
	DrawImageWindowBitmap ( window );
	GInvalidateWindow ( window, NULL );
}

/*** DoRGBBalanceDialogEvent **************************************************************/

int DoRGBBalanceDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int				respond = TRUE;
	char			text[256];
	short			value;
	double			redBalance, greenBalance, blueBalance;
	GControlPtr		control;
	ImagePtr		image;
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
			image = GetImageWindowImage ( sRGBBalanceWindow );
			GetRGBBalance ( &redBalance, &greenBalance, &blueBalance );
			
			control = GGetControl ( dialog, RGB_BALANCE_RED_SLIDER );
			GSetControlMin ( control, 0 );
			GSetControlMax ( control, 200 );
			GSetControlValue ( control, 100 * redBalance );

			control = GGetControl ( dialog, RGB_BALANCE_GREEN_SLIDER );
			GSetControlMin ( control, 0 );
			GSetControlMax ( control, 200 );
			GSetControlValue ( control, 100 * greenBalance );
			
			control = GGetControl ( dialog, RGB_BALANCE_BLUE_SLIDER );
			GSetControlMin ( control, 0 );
			GSetControlMax ( control, 200 );
			GSetControlValue ( control, 100 * blueBalance );
			
			sprintf ( text, "%.2f", redBalance );
			GSetDialogItemText ( dialog, RGB_BALANCE_RED_TEXT, text );
			
			sprintf ( text, "%.2f", greenBalance );
			GSetDialogItemText ( dialog, RGB_BALANCE_GREEN_TEXT, text );

			sprintf ( text, "%.2f", blueBalance );
			GSetDialogItemText ( dialog, RGB_BALANCE_BLUE_TEXT, text );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					GGetDialogItemText ( dialog, RGB_BALANCE_RED_TEXT, text );
					redBalance = atof ( text );
					
					GGetDialogItemText ( dialog, RGB_BALANCE_GREEN_TEXT, text );
					greenBalance = atof ( text );

					GGetDialogItemText ( dialog, RGB_BALANCE_BLUE_TEXT, text );
					blueBalance = atof ( text );

					GExitModalDialogLoop ( param1 );
					break;
					
				case G_CANCEL_BUTTON:
					GExitModalDialogLoop ( param1 );
					break;
					
				case RGB_BALANCE_RED_TEXT:
				case RGB_BALANCE_GREEN_TEXT:
				case RGB_BALANCE_BLUE_TEXT:
					GGetDialogItemText ( dialog, param1, text );
					value = atof ( text );
					GSetDialogControlValue ( dialog, param1 - 1, value * 100 );
					
					break;

				case RGB_BALANCE_RED_SLIDER:
				case RGB_BALANCE_GREEN_SLIDER:
				case RGB_BALANCE_BLUE_SLIDER:
					value = GGetDialogControlValue ( dialog, param1 );
					sprintf ( text, "%.2f", value / 100.0 );
					GSetDialogItemText ( dialog, param1 + 1, text );
					
					image = GetImageWindowImage ( sRGBBalanceWindow );
					GetRGBBalance ( &redBalance, &greenBalance, &blueBalance );
					
					if ( param1 == RGB_BALANCE_RED_SLIDER )
						redBalance = value / 100.0;
									
					if ( param1 == RGB_BALANCE_GREEN_SLIDER )
						greenBalance = value / 100.0;

					if ( param1 == RGB_BALANCE_BLUE_SLIDER )
						blueBalance = value / 100.0;
						
					SetRGBBalance ( redBalance, greenBalance, blueBalance );
					DrawImageWindowBitmap ( sRGBBalanceWindow );
					GInvalidateWindow ( sRGBBalanceWindow, NULL );
					GUpdateWindow ( sRGBBalanceWindow );
					break;
			}
			break;
	}
	
	return ( respond );
}

/*** RGBBalanceImageWindow **************************************************************

	Adjusts the RGB color balance of a color image window by multiplying its red, green,
	and blue frames by a particular constant.

	void RGBBalanceImageWindow ( GWindowPtr window, double red, double grn, double blu )
	
	(window): pointer to an image window containing a color image.
	(red):    factor by which to multiply the red component.
	(grn):    factor by which to multiply the green frame.
	(blu):    factor by which to multiply the blue frame.
	
	After the image's R-G-B frames have been multipled by their respective factors,
	the image's offscreen bitmap is redrawn, and the window containing it is
	invalidated.
	
	If you pass 1.0, 1.0, and 1.0 as the RGB balancing factors, the image will be
	unchanged.
	 
*****************************************************************************************/

void RGBBalanceImageWindow ( GWindowPtr window, double red, double grn, double blu )
{
	ImagePtr	image = GetImageWindowImage ( window );
	short		cols, rows, frames, col, row, frame;
	PIXEL		*data = NULL;
	
	/*** Determine the width, height, and number of frames in the image. ***/
	
	cols = GetImageColumns ( image );
	rows = GetImageRows ( image );
	frames = GetImageFrames ( image );

	/*** For each frame, scale the image data values by the appropriate factor. ***/
	
	for ( frame = 0; frame < frames; frame++ )
	{
		for ( row = 0; row < rows; row++ )
		{
			data = GetImageDataRow ( image, frame, row );
			
			/*** The 0th, 3rd, 6th, etc. frames contain the red component of the
			     image.  Multiply them by the red balancing factor. ***/
			     
			if ( frame % 3 == 0 )
			{
				for ( col = 0; col < cols; col++ )
					data[col] *= red;
			}

			/*** The 1st, 4th, 7th, etc. frames contain the green component of the
			     image.  Multiply them by the green balancing factor. ***/

			else if ( frame % 3 == 1 )
			{
				for ( col = 0; col < cols; col++ )
					data[col] *= grn;
			}

			/*** The 2st, 5th, 8th, etc. frames contain the blue component of the
			     image.  Multiply them by the blue balancing factor. ***/

			else if ( frame % 3 == 2 )
			{
				for ( col = 0; col < cols; col++ )
					data[col] *= blu;
			}			
		}
	}

	/*** Perform any window updating that needs to be done, now that the
	     underlying image data has changed. ***/

	UpdateImage ( image );
}


