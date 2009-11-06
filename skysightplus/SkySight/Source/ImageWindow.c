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

/*** Local constants and macros ***/

#define IMAGE_WINDOW_VERTICAL_SCROLL	1
#define IMAGE_WINDOW_HORIZONTAL_SCROLL	2

#define IMAGE_HISTOGRAM_BINS			100

#define IMAGE_WINDOW_IMAGE_PART					1
#define IMAGE_WINDOW_MAGNIFICATION_PART			2
#define IMAGE_WINDOW_COLOR_TABLE_PART			3
#define IMAGE_WINDOW_MOUSE_COORDINATES_PART		4

#ifdef GMAC
#define IMAGE_WINDOW_FONT_NAME			"Geneva"
#define IMAGE_WINDOW_FONT_SIZE			10
#else
#define IMAGE_WINDOW_FONT_NAME			"MS Sans Serif"
#define IMAGE_WINDOW_FONT_SIZE			14
#endif

/*** Local data types ***/

typedef struct ImageWindowData
{
	int				iwNeedsSave;
	GWindowPtr		iwWindow;
	GWindowPtr		iwNextImageWindow;
	GImagePtr		iwBitmap;
	ImagePtr		iwImage;
	ImageRegionPtr	iwSelectedRegion;
	short			iwSelectedFrame;
	CameraPtr		iwCamera;
	long			iwDisplayMode;
	PIXEL			iwDisplayMin;
	PIXEL			iwDisplayMax;
	short			iwColorTable;
	short			iwColorFrame;
	short			iwDisplayZoom;
	short			iwDisplayCenterRow;
	short			iwDisplayCenterCol;
    short			iwDisplayMouseCoordinates;
	short			iwObjectDisplayFlags;
}
ImageWindowData, *ImageWindowDataPtr;

/*** Local functions ***/

static ImageWindowDataPtr	CreateImageWindowData ( void );
static void					DeleteImageWindowData ( ImageWindowDataPtr );

static ImageWindowDataPtr	GetImageWindowData ( GWindowPtr );
static void					SetActiveImageWindow ( GWindowPtr );

static void					DoImageWindowClick ( GWindowPtr, short, short );
static void					DoShiftToolClick ( GWindowPtr, short, short );
static void					DoMarkToolClick ( GWindowPtr, short, short );
static void					CloseImageWindow ( GWindowPtr );
static void					UpdateImageWindow ( GWindowPtr, GRectPtr );
static void					ScrollImageWindow ( GWindowPtr, short, short );

static void					GetImageWindowImageRect ( GWindowPtr, GRectPtr );
static void					GetImageWindowMagnificationMenuRect ( GWindowPtr, GRectPtr );
static void					GetImageWindowColorTableMenuRect ( GWindowPtr, GRectPtr );
static void					GetImageWindowMouseCoordinatesMenuRect ( GWindowPtr, GRectPtr );

static void					DrawImageWindowMagnificationMenu ( GWindowPtr, int );
static void					DrawImageWindowColorTableMenu ( GWindowPtr, int );
static void					DrawImageWindowMouseCoordinatesMenu ( GWindowPtr, int );

static void					DoImageWindowMagnificationMenuClick ( GWindowPtr );
static void					DoImageWindowColorTableMenuClick ( GWindowPtr );
static void					DoImageWindowMouseCoordinatesMenuClick ( GWindowPtr );

static short				GetImageWindowPart ( GWindowPtr, short, short );

static void					SetImageWindowCursor ( GWindowPtr, short, short );

/*** Local data ***/

static GWindowPtr			sActiveImageWindow = NULL;
static GWindowPtr			sImageWindowList = NULL;

/*** CreateImageWindow ******************************************************

	Creates a new image window.

	GWindowPtr NewImageWindow ( ImagePtr image )

    (image): pointer to image record which should be displayed in the window.

    The function returns a pointer to the new image window if successful,
    or NULL on failure.

    This function allocates memory for the image's offscreen bitmap, but
    does not actually compute the image display range or draw the bitmap.
    To perform these functions, call ComputeImageMinMax() and/or
    DrawImageWindowBitmap().
    
    The new window will not yet be visible; call GSetActiveWindow() to
    display and activate it.
    
*****************************************************************************/

GWindowPtr NewImageWindow ( ImagePtr image )
{
	GWindowPtr			window = NULL;
	ImageWindowDataPtr	data = NULL;
	GControlPtr			control = NULL;
	GPathPtr			path = NULL;
	GRect				vertScrollRect, horiScrollRect;
	short				type, width, height;
	char				*title;

	/*** Allocate memory for ImageWindow's private data. ***/
	
	data = CreateImageWindowData();
	if ( data == NULL )
		return ( NULL );
	
	/*** Allocate memory for ImageWindow's offscreen bitmap. ***/
	
	width = GetImageColumns ( image );
	height = GetImageRows ( image );
	type = GetImageType ( image );

	if ( type == IMAGE_TYPE_RGB_COLOR )
		data->iwBitmap = GCreateImage ( width, height, 32 );
	else
		data->iwBitmap = GCreateImage ( width, height, 8 );
		
	if ( data->iwBitmap == NULL )
	{
		DeleteImageWindowData ( data );
		return ( NULL );
	}

	/*** Create the ImageWindow's window.  Initally, the window will be hidden,
	     and have the default size and position.  We will properly size the window
		 and make it visible later.  Supress automatic erasure of the window on
	     update events, and display the window's resize box. ***/
	
	title  = GetImageTitle ( image );
	window = GCreateWindow ( IMAGE_WINDOW, title, -1, -1, width, height, FALSE,
	         G_APPLICATION_WINDOW, (long) data, DoImageWindowEvent );

	if ( window == NULL )
	{
		DeleteImageWindowData ( data );
		return ( NULL );
	}

	GEraseOnUpdates ( window, FALSE );
	GShowSizeBox ( window );
    
	/*** Create the ImageWindow's scroll bars. ***/
	
	GSetWindowScrollRects ( window, &vertScrollRect, &horiScrollRect, NULL );

	control = GCreateScrollBar ( window, IMAGE_WINDOW_VERTICAL_SCROLL, "", &vertScrollRect, TRUE, height / 2, 0, height );
	if ( control == NULL )
	{
		DeleteImageWindow ( window );
		return ( NULL );
	}
	
	control = GCreateScrollBar ( window, IMAGE_WINDOW_HORIZONTAL_SCROLL, "", &horiScrollRect, TRUE, width / 2, 0, width );
	if ( control == NULL )
	{
		DeleteImageWindow ( window );
		return ( NULL );
	}
	 
	/*** Associate the image record with the window; this will also set the window's
	     title.  Initialize the other image display values associated with the window. ***/

	SetImageWindowImage ( window, image );

	SetImageWindowColorTable ( window, COLOR_TABLE_GRAYSCALE );
	SetImageWindowColorFrame ( window, COLOR_FRAME_ALL, FALSE );
	SetImageWindowDisplayMode ( window, DISPLAY_MODE_AUTOMATIC );
	SetImageWindowDisplayMin ( window, 0 );
	SetImageWindowDisplayMax ( window, 4095 );
	SetImageWindowDisplayZoom ( window, 0 );
	SetImageWindowDisplayCenterCol ( window, width / 2 );
	SetImageWindowDisplayCenterRow ( window, height / 2 );
    SetImageWindowDisplayMouseCoordinates ( window, MOUSE_COORDINATES_NONE );
    
	SetImageWindowSelectedRegion ( window, NULL );
	SetImageWindowSelectedFrame ( window, ALL_FRAMES );
	
	SetImageWindowDisplayObjects ( window, TRUE );
	SetImageWindowDisplayObjectCentroids ( window, TRUE );
	SetImageWindowDisplayObjectRadii ( window, TRUE );
	SetImageWindowDisplayObjectNames ( window, TRUE );
	SetImageWindowDisplayObjectMagnitudes ( window, FALSE );

	/*** Add the image window to the start of the linked list of image windows.
	     Then rebuild the image menus. ***/

	SetNextImageWindow ( window, sImageWindowList );
	sImageWindowList = window;
	BuildImageMenus();

	return ( window );
}

/*** DeleteImageWindow *********************************************************

	Destroys an image window and all associated data, and removes the window
	from the linked-list of image windows.

	void DeleteImageWindow ( GWindowPtr window )
	
	(window): pointer to image window to be destroyed.
	
********************************************************************************/

void DeleteImageWindow ( GWindowPtr window )
{
	GWindowPtr			listWindow;
	ImageWindowDataPtr	data = GetImageWindowData ( window );
	CameraPtr			camera;
		
    /*** If we are deleting the image window designated to recieve
         output from a particular camera, make sure that camera's
         output is designated not to go to any window. ***/

	camera = GetImageWindowCamera ( window );
	if ( camera != NULL )
		SetCameraExposureWindow ( camera, NULL );

	/*** Remove the image window from the linked list. **/
	
	if ( sImageWindowList == window )
		sImageWindowList = GetNextImageWindow ( window );
	else
		for ( listWindow = sImageWindowList; listWindow; listWindow = GetNextImageWindow ( listWindow ) )
			if ( GetNextImageWindow ( listWindow ) == window )
				SetNextImageWindow ( listWindow, GetNextImageWindow ( window ) );

	/*** Delete the image window, then delete the image window's private data.
	     We need to delete the window first because the Windows MDIDestroy message
		 seems to send an activate event to the window, which causes havoc if the
		 window's private data is missing. ***/

	GDeleteWindow ( window );
	if ( data != NULL )
		DeleteImageWindowData ( data );

	/*** Finally, rebuild the image menus. ***/

	BuildImageMenus();
}

/*** CreateImageWindowData ***/

ImageWindowDataPtr CreateImageWindowData ( void )
{
	ImageWindowDataPtr	data = NULL;
	
	data = (ImageWindowDataPtr) malloc ( sizeof ( ImageWindowData ) );
	if ( data == NULL )
		return ( NULL );
		
	memset ( data, 0, sizeof ( ImageWindowData ) );
	return ( data );
}

/*** DeleteImageWindowData ***/

void DeleteImageWindowData ( ImageWindowDataPtr data )
{
	if ( data->iwImage != NULL )
		DeleteImage ( data->iwImage );
	
	if ( data->iwBitmap != NULL )
		GDeleteImage ( data->iwBitmap );
	
	if ( data->iwSelectedRegion != NULL )
		DeleteImageRegion ( data->iwSelectedRegion );
				
	free ( data );
}

/***  IsImageWindow  **************************************************

	Returns TRUE or FALSE, depending on whether or not a given
	window pointer is, in fact, an image window.
	
***********************************************************************/

int IsImageWindow ( GWindowPtr window )
{
	/*** GGetWindowID is not yet implemented in MS Windows, doh!

	if ( window != NULL && GGetWindowID ( window ) == IMAGE_WINDOW )
		return ( TRUE );
	else		
		return ( FALSE );

    ***/

    return FALSE;
}

/***  GetFirstImageWindow  ******************************************

	Returns a pointer to the first image window in the global
	image window list.

	GWindowPtr GetFirstImageWindow ( void )
	
	If there are no image windows present, the function returns
	NULL.
	
*********************************************************************/

GWindowPtr GetFirstImageWindow ( void )
{
	return ( sImageWindowList );
}

/***  GetNextImageWindow  ********************************************

	Given an image window pointer, returns a pointer to the next
	image window in the global image window list.

	GWindowPtr GetNextImageWindow ( GWindowPtr window )

	(window): pointer to an image window.
	
	The function returns a pointer to the image window immediately
	following the given window in the global image window list.
	If the window provided is the last one in the list, the function
	returns NULL.
	 
**********************************************************************/

GWindowPtr GetNextImageWindow ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwNextImageWindow );
}

/***  SetNextImageWindow  ********************************************

	Sets the window which follows an image window in the global
	image window list.

	void SetNextImageWindow ( GWindowPtr window, GWindowPtr next )
	
	(window): pointer to image window
	(next):   pointer to image window which should follow (window).
	
**********************************************************************/

void SetNextImageWindow ( GWindowPtr window, GWindowPtr nextImageWindow )
{
	GetImageWindowData ( window )->iwNextImageWindow = nextImageWindow;
}

/***  GetNthImageWindow  *********************************************

	Returns a pointer to the Nth image window in the global image
	window list.

	GWindowPtr GetNthImageWindow ( long n )
	
	(n): image window number
	
	The function returns a pointer to the specified image window.
	Windows are counted from 1 to the number of image window which
	are currently present.  If (n) is less than 1 or greater than
	the number of image windows currently in memory, the function
	will return NULL.
	
**********************************************************************/

GWindowPtr GetNthImageWindow ( long n )
{
	long		i = 1;
	GWindowPtr	window = GetFirstImageWindow();
	
	while ( i < n && window != NULL )
	{
		i++;
		window = GetNextImageWindow ( window );
	}
	
	return ( window );
}

/*** GetActiveImageWindow *******************************************

	Returns a pointer to the image window which has most recently
	been selected by the user.
	
	GWindowPtr GetActiveImageWindow ( void )

	The window pointer returned by this function is the image
	window to which all operations should be applied.  If there
	are no image windows present, this function should return NULL.
		
*********************************************************************/

GWindowPtr GetActiveImageWindow ( void )
{
	return ( sActiveImageWindow );
}

/*** SetActiveImageWindow ***/

void SetActiveImageWindow ( GWindowPtr window )
{
	sActiveImageWindow = window;

	SetImageDisplayDialogItems ( window );
	
	if ( window == NULL )
		ShowImageRegionStatistics ( NULL );
	else
		ShowImageRegionStatistics ( GetImageWindowSelectedRegion ( window ) );
	
	UpdateMenus();
}

/*** GetImageWindowVerticalScrollBar ***/

GControlPtr GetImageWindowVerticalScrollBar ( GWindowPtr window )
{
	GControlPtr	control;
	
	control = GGetControl ( window, IMAGE_WINDOW_VERTICAL_SCROLL );
	
	return ( control );
}

/*** GetImageWindowHorizontalScrollBar ***/

GControlPtr GetImageWindowHorizontalScrollBar ( GWindowPtr window )
{
	GControlPtr	control;
	
	control = GGetControl ( window, IMAGE_WINDOW_HORIZONTAL_SCROLL );
	
	return ( control );
}

/*** GetImageWindowData ***/

ImageWindowDataPtr GetImageWindowData ( GWindowPtr window )
{
	return ( (ImageWindowDataPtr) GGetWindowData ( window ) );
}

/***  GetImageWindowNeedsSave  ***/

int GetImageWindowNeedsSave ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwNeedsSave );
}

/*** SetImageWindowNeedsSave ***/

void SetImageWindowNeedsSave ( GWindowPtr window, int state )
{
	GetImageWindowData ( window )->iwNeedsSave = state;
}

/*** GetImageWindowImage *******************************************************************

	Returns a pointer to an image window's associated image record.
	
	ImagePtr GetImageWindowImage ( GWindowPtr window )

	(window): pointer to image window.
		
********************************************************************************************/

ImagePtr GetImageWindowImage ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwImage );
}

/*** SetImageWindowImage *******************************************************************

	Associates an image window with an image record.
	
	void GetImageWindowImage ( GWindowPtr window, ImagePtr image )

	(window): pointer to image window.
	(image):  pointer to image record.
	
	The function returns nothing.  You can dissociate the image window from any
	image record by passing NULL for the image record.  If you pass a valid image
	record pointer, the image window's title will be set to the file name
	contained in the image's path specification record.
	
********************************************************************************************/

void SetImageWindowImage ( GWindowPtr window, ImagePtr image )
{
	GetImageWindowData ( window )->iwImage = image;
	
	if ( image != NULL )
	{
		if ( GetImageWindow ( image ) != window )
			SetImageWindow ( image, window );
	}
	else	
	{
		GSetWindowTitle ( window, "" );		
	}
}

/*** GetImageWindowBitmap *********************************************************

	Returns a pointer to an image window's offscreen bitmap.

	GImagePtr GetImageWindowBitmap ( GWindowPtr window )

	(window): pointer to image window.
		
***********************************************************************************/

GImagePtr GetImageWindowBitmap ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwBitmap );
}

/*** NewImageWindowBitmap *********************************************************

	Allocates (or reallocates) memory for an ImageWindow's offscreen bitmap.

	GImagePtr NewImageWindowBitmap ( GWindowPtr window )
	
	(window): pointer to image window.
	
	If successful, the function returns a pointer to the bitmap; on failure,
	it returns NULL.  If the function fails, the image window's existing
	bitmap is left unchanged.
	
	The bitmap is dimensioned so that its width and height match the number of
	columns and rows in the underlying image.
	
	If the underlying image is a 3-frame, RGB color image, the bitmap is created
	as a 32-bit direct-color bitmap.

	If the underlying image is a single-frame, monochrome image, the bitmap is
	created as an 8-bit indexed-color bitmap.  If the window already contains
	an 8-bit bitmap, that old bitmap's color table is copied into the new one.
	If there is no previous bitmap, or if the previous bitmap is not 8-bit,
	the bitmap's color table is set to a simple grayscale ramp.
	
	If the window already contains a bitmap, that bitmap is destroyed and replaced
	with the new one created by this function.

***********************************************************************************/

GImagePtr NewImageWindowBitmap ( GWindowPtr window )
{
	ImageWindowDataPtr	data = GetImageWindowData ( window );
	ImagePtr			image = GetImageWindowImage ( window );
	short				cols = GetImageColumns ( image );
	short				rows = GetImageRows ( image );
	GImagePtr			bitmap;
	
	/*** First reallocate the image window's image display bitmap.
	     On failure, display a warning message and return a NULL pointer. ***/
	
	if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
		bitmap = GCreateImage ( cols, rows, 32 );
	else
		bitmap = GCreateImage ( cols, rows, 8 );
		
	if ( bitmap == NULL )
		return ( NULL );

	/*** If we have an 8-bit indexed-color bitmap, copy the bitmap's
	     old color table entries into the new bitmap (if the old bitmap
	     exists and is also an 8-bit bitmap); otherwise, create a
	     grayscale color table for the bitmap. ***/
	
	if ( GGetImageDepth ( bitmap ) == 8 )
	{
		short				index;

		if ( data->iwBitmap != NULL && GGetImageDepth ( data->iwBitmap ) == 8 )
		{
			unsigned char		red, green, blue;
		
			for ( index = 0; index < 256; index++ )
			{
				GGetImageColorTableEntry ( data->iwBitmap, index, &red, &green, &blue );
				GSetImageColorTableEntry ( bitmap, index, red, green, blue );
			}
		}
		else
		{
			for ( index = 0; index < 256; index++ )
				GSetImageColorTableEntry ( bitmap, index, index, index, index );
		}
	}
			
	/*** Delete the image window's old bitmap (if present), and replace
	     it with the new one we just allocated. ***/
	
	if ( data->iwBitmap != NULL )     
		GDeleteImage ( data->iwBitmap );
		
	data->iwBitmap = bitmap;
	
	return ( bitmap );
}

/*** ResizeImageWindowImage *******************************************************

	Reallocates an image window's display bitmap, as well as the underlying
	FITS image data matrix.

	FITSImagePtr ResizeImageWindowImage ( GWindowPtr window, short cols, short rows )
	
	(window): pointer to image window
	(cols): desired number of columns in new image data matrix.
	(rows): desired number of rows in new image data matrix.
	(copy): if TRUE, copy image data from old matrix into new one.
	
	If successful, the function returns a pointer to the image window's
	previous FITS image record.  On failure, the function returns NULL.
	
	If (copy) is non-zero, the image data will be copied from the old image
	data matrix to the corresponding locations in the new one, wherever possible.
	
	On a successful return, the image's old bitmap is destroyed.  The new bitmap
	is not yet drawn, the window is not updated, and the window's size and position
	are unchanged.  You must call separate functions to modify these.
	
*************************************************************************************/

FITSImagePtr ResizeImageWindowImage ( GWindowPtr window, short cols, short rows, int copy )
{
	ImageWindowDataPtr	data = GetImageWindowData ( window );
	ImagePtr			image = GetImageWindowImage ( window );
	FITSImagePtr		oldFITS = GetImageFITSImage ( image ), newFITS;
	short				oldRows = GetImageRows ( image );
	short				oldCols = GetImageColumns ( image );
	short				oldFrames = GetImageFrames ( image );
	PIXEL				***oldData = NULL;
	GImagePtr			bitmap = NULL;

	/*** Now resize the image's underlying FITS image data matrix.
	     On failure, display a warning message, free memory for the
	     bitmap, and return.  If successful, store the new FITS image
	     record into the image. ***/
	     
	newFITS = ResizeFITSImage ( oldFITS, cols, rows, oldFrames, copy );
	if ( newFITS == NULL )
	{
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		return ( NULL );
	}

	SetImageFITSImage ( image, newFITS );
	
	/*** Now resize the image window's offscreen bitmap, if necessary.
	     On failure, restore the image's previous FITS image record,
	     release memory for the new FITS image record, display a warning
	     message, and return a NULL pointer. ***/
	
	bitmap = GetImageWindowBitmap ( window );
	if ( cols != GGetImageWidth ( bitmap ) || rows != GGetImageHeight ( bitmap ) )
	{
		bitmap = NewImageWindowBitmap ( window );
		if ( bitmap == NULL )
		{
			SetImageFITSImage ( image, oldFITS );
			FreeFITSImage ( newFITS );
			WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
			return ( NULL );
		}
	}
		
	/*** Return a pointer to the image window's old FITS image record. ***/
	
	return ( oldFITS );
}

/*** SetImageWindowColorTable ***/

void SetImageWindowColorTable ( GWindowPtr window, short item )
{
	GImagePtr	bitmap = GetImageWindowBitmap ( window );
	
	GetImageWindowData ( window )->iwColorTable = item;
	
	switch ( item )
	{
		case COLOR_TABLE_GRAYSCALE:
			SetBitmapColorTableGrayscale ( bitmap );
			break;
			
		case COLOR_TABLE_RAINBOW:
			SetBitmapColorTableRainbow ( bitmap );
			break;

		case COLOR_TABLE_FLAME:
			SetBitmapColorTableFlame ( bitmap );
			break;

		case COLOR_TABLE_RED:
			SetBitmapColorTableRed ( bitmap );
			break;
			
		case COLOR_TABLE_GREEN:
			SetBitmapColorTableGreen ( bitmap );
			break;

		case COLOR_TABLE_BLUE:
			SetBitmapColorTableBlue ( bitmap );
			break;
			
		case COLOR_TABLE_WINTER:
			SetBitmapColorTableWinter ( bitmap );
			break;
			
		case COLOR_TABLE_SPRING:
			SetBitmapColorTableSpring ( bitmap );
			break;

		case COLOR_TABLE_SUMMER:
			SetBitmapColorTableSummer ( bitmap );
			break;

		case COLOR_TABLE_FALL:
			SetBitmapColorTableFall ( bitmap );
			break;
	}
	
	GInvalidateWindow ( window, NULL );
}

/*** GetImageWindowColorTable ***/

short GetImageWindowColorTable ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwColorTable );
}

/*** SetImageWindowColorFrame ***/

void SetImageWindowColorFrame ( GWindowPtr window, short item, int redraw )
{
	GetImageWindowData ( window )->iwColorFrame = item;

	if ( redraw )
	{
		DrawImageWindowBitmap ( window );
		GInvalidateWindow ( window, NULL );
	}
}

/*** GetImageWindowColorFrame ***/

short GetImageWindowColorFrame ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwColorFrame );
}

/*** GetImageWindowDisplayMode ***/

short GetImageWindowDisplayMode ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwDisplayMode );
}

/*** SetImageWindowDisplayMode ***/

void SetImageWindowDisplayMode ( GWindowPtr window, short mode )
{
	GetImageWindowData ( window )->iwDisplayMode = mode;
}

/*** GetImageWindowDisplayMin ***/

PIXEL GetImageWindowDisplayMin ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwDisplayMin );
}

/*** SetImageWindowDisplayMin ***/

void SetImageWindowDisplayMin ( GWindowPtr window, PIXEL min )
{
	GetImageWindowData ( window )->iwDisplayMin = min;
}

/*** GetImageWindowDisplayMax ***/

PIXEL GetImageWindowDisplayMax ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwDisplayMax );
}

/*** SetImageWindowDisplayMax ***/

void SetImageWindowDisplayMax ( GWindowPtr window, PIXEL max )
{
	GetImageWindowData ( window )->iwDisplayMax = max;
}

/*** GetImageWindowDisplayZoom ***/

short GetImageWindowDisplayZoom ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwDisplayZoom );
}

/*** SetImageWindowDisplayZoom ***/

void SetImageWindowDisplayZoom ( GWindowPtr window, short zoom )
{
	GetImageWindowData ( window )->iwDisplayZoom = zoom;
}

/*** GetImageWindowDisplayMagnification ***/

short GetImageWindowDisplayMagnification ( GWindowPtr window )
{		 
	short	zoom = GetImageWindowDisplayZoom ( window );
	short	item = 0;
	
    switch ( zoom )
    {
    	case 4:
    		item = MAGNIFICATION_16X_ITEM;
    		break;
    		
    	case 3:
    		item = MAGNIFICATION_8X_ITEM;
    		break;

    	case 2:
    		item = MAGNIFICATION_4X_ITEM;
    		break;

    	case 1:
    		item = MAGNIFICATION_2X_ITEM;
    		break;

    	case 0:
    		item = MAGNIFICATION_1X_ITEM;
    		break;

    	case -1:
    		item = MAGNIFICATION_1_2X_ITEM;
    		break;

    	case -2:
    		item = MAGNIFICATION_1_4X_ITEM;
    		break;

    	case -3:
    		item = MAGNIFICATION_1_8X_ITEM;
    		break;

    	case -4:
    		item = MAGNIFICATION_1_16X_ITEM;
    		break;
    }

	return ( item );
}

/*** GetImageWindowDisplayMouseCoordinates ***/

short GetImageWindowDisplayMouseCoordinates ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwDisplayMouseCoordinates );
}

/*** SetImageWindowDisplayMouseCoordinates ***/

void SetImageWindowDisplayMouseCoordinates ( GWindowPtr window, short item )
{
	GetImageWindowData ( window )->iwDisplayMouseCoordinates = item;
}

/*** GetImageWindowDisplayCenterRow ***/

short GetImageWindowDisplayCenterRow ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwDisplayCenterRow );
}

/*** SetImageWindowDisplayCenterRow ***/

void SetImageWindowDisplayCenterRow ( GWindowPtr window, short row )
{
	GetImageWindowData ( window )->iwDisplayCenterRow = row;
	GSetControlValue ( GetImageWindowVerticalScrollBar ( window ), row );	
}

/*** GetImageWindowDisplayCenterCol ***/

short GetImageWindowDisplayCenterCol ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwDisplayCenterCol );
}

/*** SetImageWindowDisplayCenterCol ***/

void SetImageWindowDisplayCenterCol ( GWindowPtr window, short col )
{
	GetImageWindowData ( window )->iwDisplayCenterCol = col;
	GSetControlValue ( GetImageWindowHorizontalScrollBar ( window ), col );	
}

/*** GetImageWindowDisplayRect ***/

void GetImageWindowDisplayRect ( GWindowPtr window, GRectPtr rect )
{
	GRect	vertScrollRect, horiScrollRect, mouseMenuRect;
	
	GSetWindowScrollRects ( window, &vertScrollRect, &horiScrollRect, rect );
	if ( GetImageWindowDisplayMouseCoordinates ( window ) > MOUSE_COORDINATES_NONE )
	{
		GetImageWindowMouseCoordinatesMenuRect ( window, &mouseMenuRect );
		rect->top = mouseMenuRect.bottom + 1;
	}
}

/*** GetImageWindowSelectedRegion ***/

ImageRegionPtr GetImageWindowSelectedRegion ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwSelectedRegion );
}

/*** SetImageWindowSelectedRegion ***/

void SetImageWindowSelectedRegion ( GWindowPtr window, ImageRegionPtr region )
{
	ImageWindowDataPtr	data = GetImageWindowData ( window );
	
	if ( data->iwSelectedRegion != NULL )
		DeleteImageRegion ( data->iwSelectedRegion );
		
	data->iwSelectedRegion = region;
}

/*** GetImageWindowSelectedFrame ***/

short GetImageWindowSelectedFrame ( GWindowPtr window, short *frame0, short *frame1 )
{
	short frame = GetImageWindowData ( window )->iwSelectedFrame;
	
	if ( frame == ALL_FRAMES )
	{
		if ( frame0 != NULL )
			*frame0 = 0;
		
		if ( frame1 != NULL )
			*frame1 = GetImageFrames ( GetImageWindowImage ( window ) ) - 1;
	}
	else
	{
		if ( frame0 != NULL )
			*frame0 = frame;
			
		if ( frame1 != NULL )
			*frame1 = frame;
	}
	
	return ( frame );
}

/*** SetImageWindowSelectedFrame ***/

void SetImageWindowSelectedFrame ( GWindowPtr window, short frame )
{
	GetImageWindowData ( window )->iwSelectedFrame = frame;
}

/*** GetImageWindowObjectDisplayFlag ********************************************

	Determines whether or not a particular image window object display flag bit
	is set.

	short GetImageWindowObjectDisplayFlag ( GWindowPtr window, short flag )
	
	(window): pointer to image window.
	(flag):   flag bit whose status you wish to determine.
	
	The function returns TRUE if the specified flag bit is set, and
	FALSE otherwise.
	
	The possible display flags are #defined in "SkySight.h" with names that
	begin with IMAGE_WINDOW_DISPLAY_OBJECT.  It may be easier to use the
	corresponding macros for each flag, rather than calling this function
	directly; e.g. use
	
	GetImageWindowDisplayObjectMagnitudes ( window )
	
	instead of
	
	GetImageWindowObjectDisplayFlag ( window, IMAGE_WINDOW_DISPLAY_OBJECT_MAGNITUDES )
	
*********************************************************************************/

short GetImageWindowObjectDisplayFlag ( GWindowPtr window, short flag )
{
	if ( GetImageWindowData ( window )->iwObjectDisplayFlags & flag )
		return ( TRUE );
	else
		return ( FALSE );	
}

/*** SetImageWindowObjectDisplayFlag ********************************************

	Sets a particular image window object display flag bit.

	short SetImageWindowObjectDisplayFlag ( GWindowPtr window, short flag, short value )
	
	(window): pointer to image window.
	(flag):   flag bit whose status you wish to modify.
	(value):  zero to turn bit off; non-zero to turn bit on.
	
	The function returns nothing.
	
	The possible display flags are #defined in "SkySight.h" with names that
	begin with IMAGE_WINDOW_DISPLAY_OBJECT.  It may be easier to use the
	corresponding macros for each flag, rather than calling this function
	directly; e.g. use
	
	SetImageWindowDisplayObjectMagnitudes ( window, TRUE )
	
	instead of
	
	GetImageWindowObjectDisplayFlag ( window, IMAGE_WINDOW_DISPLAY_OBJECT_MAGNITUDES, TRUE )

	This function does not actually cause the window to be redrawn; to do so,
	call GInvalidateWindow() to force an update event for the window after
	calling this function.
	
*********************************************************************************/

void SetImageWindowObjectDisplayFlag ( GWindowPtr window, short flag, short value )
{
	ImageWindowDataPtr	data = GetImageWindowData ( window );
	
	if ( value )
		data->iwObjectDisplayFlags |=  flag;
	else
		data->iwObjectDisplayFlags &= ~flag;
}

/*** GetImageWindowCamera *******************************************************/

CameraPtr GetImageWindowCamera ( GWindowPtr window )
{
	return ( GetImageWindowData ( window )->iwCamera );
}

/*** SetImageWindowCamera ******************************************************/

void SetImageWindowCamera ( GWindowPtr window, CameraPtr camera )
{
	GetImageWindowData ( window )->iwCamera = camera;
	
	/*** If we have a valid camera pointer, we need to check to see if
	     the camera's exposure window has already been set to point to our
	     window; otherwise, we'll have a race condition if we try to set it
	     now. ***/

	if ( camera != NULL )
		if ( GetCameraExposureWindow ( camera ) != window )
			SetCameraExposureWindow ( camera, window );
}

/*** DoImageWindowEvent ***********************************************************

	Event-handling procedure for image windows.
	
	int DoImageWindowEvent ( short event, GWindowPtr window, long param1, long param2 )
	
	(event): event type code
	(window): pointer to window for which event was intended.
	(param1): additional event information; meaning is event-dependent.
	(param2): additional event information; meaning is event-dependent.
	
	The function returns TRUE or FALSE, depending on whether or not the default
	event-handling should be performed in addition.
	
	This function is provided as a callback to the routine which creates the
	window.  It is mainly a switch to other functions which handle specific
	types of events.
	
***********************************************************************************/

int DoImageWindowEvent ( short event, GWindowPtr window, long param1, long param2 )
{
	int respond = TRUE;
	
	switch ( event )
	{
		case G_ACTIVATE_EVENT:
			SetActiveImageWindow ( window );
			break;
			
		case G_UPDATE_EVENT:
			UpdateImageWindow ( window, NULL );
			break;
		
		case G_SIZE_EVENT:
			SizeImageWindow ( window );
			break;
				
		case G_CLOSE_EVENT:
			CloseImageWindow ( window );
			respond = FALSE;
			break;
		
		case G_MOUSE_DOWN_EVENT:
			DoImageWindowClick ( window, param1, param2 );
			break;
			
		case G_MOUSE_MOVED_EVENT:
			ShowPixelValues ( window, param1, param2 );
			GStartDrawing ( window );
			DrawImageWindowMouseCoordinates ( window, param1, param2 );
			GEndDrawing ( window );
			SetImageWindowCursor ( window, param1, param2 );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case IMAGE_WINDOW_VERTICAL_SCROLL:
				case IMAGE_WINDOW_HORIZONTAL_SCROLL:
					ScrollImageWindow ( window, param1, param2 );
					break;
			}
			break;
	}
	
	return ( respond );
}

/*** CloseImageWindow ****************************************************************

	Performs tasks associated with closing an image window.
	
	void CloseImageWindow ( GWindowPtr window )

	(window): pointer to window which should be closed.
	
	You should call this function whenever the user wishes to close an image window,
	e.g. by clicking in the window's close box, selecting "Close" from the "File"
	menu, or quitting the program.
		
**************************************************************************************/

void CloseImageWindow ( GWindowPtr window )
{
	int					result = G_NO_BUTTON;
	char				*title = NULL;
	ImageWindowDataPtr	data = GetImageWindowData ( window );
	ImagePtr			image = GetImageWindowImage ( window );
	GPathPtr			path = GetImagePath ( image );
	GWindowPtr			headerWindow = GetImageHeaderWindow ( image );
	GWindowPtr			histogramWindow = GetImageHistogramWindow ( image );
	CameraPtr			camera = GetImageWindowCamera ( window );
	
	/*** If the image needs to be saved, ask the user if they wish to
	     save it before closing.  If the user cancelled, return without
	     closing the window. ***/

	if ( GetImageWindowNeedsSave ( window ) )
	{
		title = GetImageTitle ( image );
		result = InfoMessage ( G_YES_NO_CANCEL_ALERT, SAVE_CHANGES_STRING, title );
		if ( result == G_CANCEL_BUTTON )
			return;
	}
	
	/*** If the user decides to save the image, do so now. ***/
	
	if ( result == G_YES_BUTTON )
	{
		if ( path == NULL )
			DoSaveAsDialog ( window );
		else
			SaveImageWindow ( window, path );
	}

    /*** If we are closing a window which is recieving camera exposure output,
         cancel any exposure which the camera has currently in progress. ***/
    
    if ( camera != NULL )
    	CancelExposure ( camera );

	/*** If the image associated with the window has its histogram
	     displayed in another window, close the histogram window. ***/
	     
	if ( histogramWindow != NULL )
		GCloseWindow ( histogramWindow );

	/*** If the image associated with the window has its FITS header
	     displayed in another window, close the FITS header window. ***/
	     
	if ( headerWindow != NULL )
		GCloseWindow ( headerWindow );

	/*** If we are deleting the window to which the "Undo" command would be
	     applied, disable the "Undo" command. ***/

	if ( window == GetUndoWindow() )
		PrepareUndo ( NULL );
		
	/*** Now delete the image window itself.  If there are no more image windows
	     open, set the active image window to NULL.  (Otherwise, the active image
	     window will be set automatically when it comes to the foreground.) ***/
	    
	DeleteImageWindow ( window );
	if ( GetFirstImageWindow() == NULL )
		SetActiveImageWindow ( NULL );
}

/*** UpdateImageWindow ***************************************************************

	Handles updating (i.e. repainting) of image windows.
	
	void UpdateImageWindow ( GWindowPtr window )
	
	(window): pointer to image window requiring update.
	(rect):   pointer to rectangle (in the window's coordinate system in units
	          of pixels) indicating the area of window to update.
	
	This function returns nothing.  Call this function whenever an image window
	recieves an update event, indicating that part (or all) of the window needs
	to be repainted.  If you pass NULL for (rect), the entire window will be updated;
	otherwise, only the portion of the window specified in (rect) will be redrawn.

**************************************************************************************/

void UpdateImageWindow ( GWindowPtr window, GRectPtr rect )
{
	GImagePtr		bitmap = GetImageWindowBitmap ( window );
	GRect			imageRect, windowRect, bitmapRect, updateRect, backRect;
	ImagePtr		image;
	ImageObjectPtr	object;
	ImageRegionPtr	region;
	short			col, row;
	
	/*** Define the area of the window we wish to update: if we have been given
	     a particular rectangle, update only that rectangle; otherwise update the
	     entire area of the window that can be occupied by the image.  Clip drawing
	     to that rectangle, so that the window's scroll bars and other controls don't
	     get overdrawn. ***/
	     
	if ( rect != NULL )
		updateRect = *rect;
	else
		GetImageWindowDisplayRect ( window, &updateRect );
	
	GStartDrawing ( window );
	GSetPen ( GBlackColor(), 1, G_COPY_PEN );
	GSetClipRect ( &updateRect );
	
	/*** Convert the update rectangle from window to image coordinates;
	     add one row/column to deal with roundoff error, then make sure
	     the rectangle lies within the bitmap's boundaries. ***/
	     
	imageRect = updateRect;
	
	WindowToImageRect ( window, &imageRect );
	GInsetRect ( &imageRect, -1, -1 );

	GSetRect ( &bitmapRect, 0, 0, GGetImageWidth ( bitmap ), GGetImageHeight ( bitmap ) );
	GClipRect ( &imageRect, &bitmapRect );

	/*** Convert the resulting image rectangle to window coordinates, then
	     draw the portion of the image bitmap inside the image rectangle
	     into the portion of the window inside the window rectangle. ***/

	windowRect = imageRect;
	
	ImageToWindowRect ( window, &windowRect );
	GDrawImage ( bitmap, &imageRect, &windowRect );
	
	/*** Now, paint the areas surrounding the image with black.  Note that
	     there are up to 4 possible such areas, all rectangular in shape.
	     We do it this way, rather than painting the background black and
	     then drawing the image on top of it, to avoid flickering when we
	     redraw the image rapidly in succession (as when manually adjusting
	     brightness/contrast with live feedback.) ***/
	
	if ( windowRect.top > updateRect.top )
	{
		GSetRect ( &backRect, updateRect.left, updateRect.top, updateRect.right, windowRect.top );
		GDrawRect ( &backRect );
	}

	if ( windowRect.left > updateRect.left )
	{
		GSetRect ( &backRect, updateRect.left, windowRect.top, windowRect.left, windowRect.bottom );
		GDrawRect ( &backRect );
	}

	if ( windowRect.right < updateRect.right )
	{
		GSetRect ( &backRect, windowRect.right, windowRect.top, updateRect.right, windowRect.bottom );
		GDrawRect ( &backRect );
	}

	if ( windowRect.bottom < updateRect.bottom )
	{
		GSetRect ( &backRect, updateRect.left, windowRect.bottom, updateRect.right, updateRect.bottom );
		GDrawRect ( &backRect );
	}

	/*** Set the font used for drawing object labels and controls. ***/
	
	GSetPenFont ( IMAGE_WINDOW_FONT_NAME, IMAGE_WINDOW_FONT_SIZE, 0 );
	
	/*** Draw the objects in the image's object list. ***/
	
	if ( GetImageWindowDisplayObjects ( window ) )
	{
		image = GetImageWindowImage ( window );
		for ( object = GetImageObjectList ( image ); object != NULL; object = GetNextImageObject ( object ) )
			DrawImageObject ( object, window );
	}
	
	/*** If the image has a selected region, draw it in inverting mode.
	     We need to draw the selected region on top of everything else
	     in the image, since it must invert everything which it lies above. ***/
	
	region = GetImageWindowSelectedRegion ( window );
	if ( region != NULL )
	{
		short mode = GGetPenMode();
		
		GSetPenColor ( GBlackColor() );
		GSetPenMode ( G_INVERTING_PEN );
		DrawImageRegion ( window, region );
		GSetPenMode ( mode );
	}
	
	/*** Remove the clipping region. ***/
	
	GSetNullClipRegion();

	/*** Draw the mouse coordinate display, if desired. ***/
	
	if ( GetImageWindowDisplayMouseCoordinates ( window ) > MOUSE_COORDINATES_NONE )
	{
		GGetMousePosition ( window, &col, &row );
		DrawImageWindowMouseCoordinates ( window, col, row );
	}
	
	GEndDrawing ( window );
	
	/*** Draw the fake buttons. ***/
	
	DrawImageWindowMagnificationMenu ( window, FALSE );
	DrawImageWindowMouseCoordinatesMenu ( window, FALSE );
    DrawImageWindowColorTableMenu ( window, FALSE );
}

/*** SizeImageWindow *********************************************************

	Handles resizing of image windows.

	void SizeImageWindow ( GWindowPtr window )
	
	(window): pointer to ImageWindow which has been resized.
	
	The function returns nothing.  Call this function after an ImageWindow
	has been resized, or when it is initially created, in order to ensure
	proper placement of the window's scroll bars and other controls.
			
******************************************************************************/

void SizeImageWindow ( GWindowPtr window )
{
	GControlPtr		control;
	GRect			vertScrollRect, horiScrollRect, magMenuRect, mouseMenuRect;
	GRect			colorMenuRect;
    
	GSetWindowScrollRects ( window, &vertScrollRect, &horiScrollRect, NULL );
	GetImageWindowMagnificationMenuRect ( window, &magMenuRect );
    GetImageWindowColorTableMenuRect ( window, &colorMenuRect );
	GetImageWindowMouseCoordinatesMenuRect ( window, &mouseMenuRect );

	horiScrollRect.left = colorMenuRect.right;
	vertScrollRect.top = mouseMenuRect.bottom;
    
	control = GetImageWindowVerticalScrollBar ( window );
	GSetControlRect ( control, &vertScrollRect );
	
	control = GetImageWindowHorizontalScrollBar ( window );
	GSetControlRect ( control, &horiScrollRect );
}

/*** DoImageWindowClick *****************************************************

	Handles mouse-down events which occur in an image window.

	void DoImageWindowClick ( GWIndowPtr window, short col, short row )
	
	(window): pointer to image window where mouse-down occurred.
	(col): horizontal coordinate of point where mouse-down ocurred.
	(row): vertical coordinate of point where mouse-down occurred.
	
	The coordinates where the mouse-down occurred should be given in units
	of pixels in the window's coordinate system, i.e. measured from the top
	left corner of the window.
		
*****************************************************************************/

#define IMAGE_WINDOW_MANIFICATION_MENU	2

void DoImageWindowClick ( GWindowPtr window, short col, short row )
{
	switch ( GetImageWindowPart ( window, col, row ) )
	{
		case IMAGE_WINDOW_IMAGE_PART:
			switch ( GetImageTool() )
			{
				case EDIT_SELECT_TOOL:
					SelectImageRegion ( window, col, row );
					break;
				
				case EDIT_SHIFT_TOOL:
					DoShiftToolClick ( window, col, row );
					break;
						
				case EDIT_MARK_TOOL:
					DoMarkToolClick ( window, col, row );
					break;
			}
			break;
				
		case IMAGE_WINDOW_MAGNIFICATION_PART:
			DoImageWindowMagnificationMenuClick ( window );
			break;

		case IMAGE_WINDOW_MOUSE_COORDINATES_PART:
        	DoImageWindowMouseCoordinatesMenuClick ( window );
            break;

        case IMAGE_WINDOW_COLOR_TABLE_PART:
        	DoImageWindowColorTableMenuClick ( window );
            break;
	}
}

/*** SetImageWindowCursor ****************************************************/

void SetImageWindowCursor ( GWindowPtr window, short col, short row )
{
	switch ( GetImageWindowPart ( window, col, row ) )
	{
		case IMAGE_WINDOW_IMAGE_PART:
			GSetWindowCursor ( window, GGetCrossCursor() );
			break;
			
		default:
			GSetWindowCursor ( window, NULL );
			break;
	}
}

/*** GetImageWindowPart ******************************************************/

short GetImageWindowPart ( GWindowPtr window, short col, short row )
{
	GRect	rect;
	
	GetImageWindowMagnificationMenuRect ( window, &rect );
	if ( GIsPointInRect ( col, row, &rect ) )
		return ( IMAGE_WINDOW_MAGNIFICATION_PART );
	
 	GetImageWindowColorTableMenuRect ( window, &rect );
	if ( GIsPointInRect ( col, row, &rect ) )
		return ( IMAGE_WINDOW_COLOR_TABLE_PART );

 	GetImageWindowMouseCoordinatesMenuRect ( window, &rect );
	if ( GIsPointInRect ( col, row, &rect ) )
		return ( IMAGE_WINDOW_MOUSE_COORDINATES_PART );

	GetImageWindowImageRect ( window, &rect );
	if ( GIsPointInRect ( col, row, &rect ) )
		return ( IMAGE_WINDOW_IMAGE_PART );
		
	return ( 0 );
}

/*** DoImageWindowMagnificationMenuClick **************************************/

void DoImageWindowMagnificationMenuClick ( GWindowPtr window )
{
	GRect		rect;
	GMenuPtr	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_MAGNIFICATION_ITEM );
	short		item = GetImageWindowDisplayMagnification ( window );
	short		left, top;
	
	DrawImageWindowMagnificationMenu ( window, TRUE );
	GetImageWindowMagnificationMenuRect ( window, &rect );

	left = rect.right;
	top = rect.top;
	
	GWindowToScreen ( window, &left, &top );
	GDoPopupMenu ( menu, left, top, item );
    DrawImageWindowMagnificationMenu ( window, FALSE );
}

/*** DoImageWindowMouseCoordinatesMenuClick **********************************/

void DoImageWindowMouseCoordinatesMenuClick ( GWindowPtr window )
{
	GRect		rect;
	GMenuPtr	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_MOUSE_COORDINATES_ITEM );
	short		item = GetImageWindowDisplayMouseCoordinates ( window );
	short		left, top;
	
	DrawImageWindowMouseCoordinatesMenu ( window, TRUE );
	GetImageWindowMouseCoordinatesMenuRect ( window, &rect );

	left = rect.right;
	top = rect.top;
	GWindowToScreen ( window, &left, &top );
	GDoPopupMenu ( menu, left, top, item );
    DrawImageWindowMouseCoordinatesMenu ( window, FALSE );
}

/*** DoImageWindowColorTableMenuClick **********************************/

void DoImageWindowColorTableMenuClick ( GWindowPtr window )
{
	GRect		rect;
	GMenuPtr	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_MOUSE_COORDINATES_ITEM );
	short		item = GetImageWindowDisplayMouseCoordinates ( window );
	short		left, top;
    ImagePtr	image = GetImageWindowImage ( window );

    if ( GetImageType ( image ) == IMAGE_TYPE_MONOCHROME )
    {
    	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_COLOR_TABLE_ITEM );
        item = GetImageWindowColorTable ( window );
    }

    if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
    {
    	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_COLOR_FRAME_ITEM );
        item = GetImageWindowColorFrame ( window );
    }

	DrawImageWindowColorTableMenu ( window, TRUE );
	GetImageWindowColorTableMenuRect ( window, &rect );

	left = rect.right;
	top = rect.top;
	GWindowToScreen ( window, &left, &top );
	GDoPopupMenu ( menu, left, top, item );
    DrawImageWindowColorTableMenu ( window, FALSE );
}

/*** GetImageWindowImageRect *************************************************/

void GetImageWindowImageRect ( GWindowPtr window, GRectPtr rect )
{
	ImagePtr	image = GetImageWindowImage ( window );
	
	GSetRect ( rect, 0, 0, GetImageColumns ( image ), GetImageRows ( image ) );
	ImageToWindowRect ( window, rect );
}

/*** GetImageWindowMagnificationMenuRect *************************************/

void GetImageWindowMagnificationMenuRect ( GWindowPtr window, GRectPtr rect )
{
	GGetWindowContentRect ( window, rect );

	rect->left = rect->left - 1;
	rect->right = rect->left + 50;
	rect->top = rect->bottom - GGetScrollBarHeight() + 1;

//	GSetWindowScrollRects ( window, NULL, rect, NULL );

//	rect->right = rect->left + 50;
}

/*** GetImageWindowColorTableMenuRect ***************************************/

void GetImageWindowColorTableMenuRect ( GWindowPtr window, GRectPtr rect )
{
	GetImageWindowMagnificationMenuRect ( window, rect );

	rect->left = rect->right;
	rect->right = rect->left + 100;
}

/*** GetImageWindowMouseCoordinatesMenuRect **********************************/

void GetImageWindowMouseCoordinatesMenuRect ( GWindowPtr window, GRectPtr rect )
{
	GGetWindowContentRect ( window, rect );
	
	rect->top = rect->top - 1;
	rect->bottom = rect->top + GGetScrollBarWidth() - 1;
	rect->left = rect->right - GGetScrollBarWidth() + 1;
}

/*** DrawImageWindowMagnificationMenu ****************************************/

void DrawImageWindowMagnificationMenu ( GWindowPtr window, int pressed )
{
	char		text[256];
	short		item;
	GRect		rect;
	GMenuPtr	menu;
	
	GetImageWindowMagnificationMenuRect ( window, &rect );
	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_MAGNIFICATION_ITEM );
	item = GetImageWindowDisplayMagnification ( window );

	GGetMenuItemText ( menu, item, text );
	GDrawButton ( window, &rect, text, NULL, pressed );
}

/*** DrawImageWindowMouseCoordinatesMenu *************************************/

void DrawImageWindowMouseCoordinatesMenu ( GWindowPtr window, int pressed )
{
	GRect		rect;

	GetImageWindowMouseCoordinatesMenuRect ( window, &rect );
	GDrawButton ( window, &rect, "M", NULL, pressed );
}

/*** DrawImageWindowColorTableMenu *******************************************/

void DrawImageWindowColorTableMenu ( GWindowPtr window, int pressed )
{
	char		text[256];
	GRect		rect;
    GMenuPtr	menu;
    short		item;
    ImagePtr	image = GetImageWindowImage ( window );

    if ( GetImageType ( image ) == IMAGE_TYPE_MONOCHROME )
    {
    	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_COLOR_TABLE_ITEM );
        item = GetImageWindowColorTable ( window );
    }

    if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
    {
    	menu = GGetSubMenu ( GGetMainMenu ( DISPLAY_MENU ), DISPLAY_COLOR_FRAME_ITEM );
        item = GetImageWindowColorFrame ( window );
    }

    GGetMenuItemText ( menu, item, text );
	GetImageWindowColorTableMenuRect ( window, &rect );
	GDrawButton ( window, &rect, text, NULL, pressed );
}

/*** DrawImageWindowMouseCoordinates ******************************************/

void DrawImageWindowMouseCoordinates ( GWindowPtr window, short col, short row )
{
	float		x, y, red, grn, blu, re, im;
	double		ra = 0.0, dec = 0.0;
	ImagePtr	image;
	short		frame, coordinates, size, mode, width, height, type;
	char		text[256] = { '\0' }, text2[256] = { '\0' };
	int			fill;
	GRect		rect;
	GColor		color;
	PIXEL		value;
	
	coordinates = GetImageWindowDisplayMouseCoordinates ( window );
	if ( coordinates == MOUSE_COORDINATES_NONE )
		return;
		
	image = GetImageWindowImage ( window );
	
	WindowToImageXY ( window, col, row, &x, &y );
	WindowToImage ( window, &col, &row );
	
	if ( col >= 0 && col < GetImageColumns ( image ) && row >= 0 && row < GetImageRows ( image ) )
	{
		if ( coordinates == MOUSE_COORDINATES_COL_ROW )
			sprintf ( text, "Col: %4hd  Row: %4hd", col, row );
			
		if ( coordinates == MOUSE_COORDINATES_X_Y )
			sprintf ( text, "X: %7.2f  Y: %7.2f", x, y );
			
		if ( coordinates == MOUSE_COORDINATES_RA_DEC )
		{
			short	rah, ram, decd, decm;
			double	ras, decs;
			char	sign;
			
			DecimalToDegMinSec ( ra,  &rah,  &ram,  &ras,  &sign );
			DecimalToDegMinSec ( dec, &decd, &decm, &decs, &sign );
			
			sprintf ( text, "R.A.: %02hd %02hd %05.2f  Dec.: %c%02hd %02hd %04.1f", rah, ram, ras, sign, decd, decm, decs );
		}

		type = GetImageType ( image );

		if ( type == IMAGE_TYPE_MONOCHROME )
		{
			value = GetImageDataValue ( image, 0, row, col );
			sprintf ( text2, "  Value:%8.2f", value );
		}
		else if ( type == IMAGE_TYPE_RGB_COLOR )
		{
			
			red = GetImageDataValue ( image, RED_FRAME, row, col );
			grn = GetImageDataValue ( image, GREEN_FRAME, row, col );
			blu = GetImageDataValue ( image, BLUE_FRAME, row, col );
			
			frame = GetImageWindowColorFrame ( window );
			if ( frame == COLOR_FRAME_ALL )
				sprintf ( text2, "  R:%8.2f  G:%8.2f  B:%8.2f", red, grn, blu );
			else if ( frame == COLOR_FRAME_RED )
				sprintf ( text2, "  R:%8.2f", red );
			else if ( frame == COLOR_FRAME_GREEN )
				sprintf ( text2, "  G:%8.2f", grn );
			else if ( frame == COLOR_FRAME_BLUE )
				sprintf ( text2, "  B:%8.2f", blu );
			
		}
		else if ( type == IMAGE_TYPE_COMPLEX )
		{
			re = GetImageDataValue ( image, 0, row, col );
			im = GetImageDataValue ( image, 1, row, col );
			
			sprintf ( text2, "  Re:%8.2f  Im:%8.2f", re, im );
		}

		strcat ( text, text2 );
	}

	GetImageWindowMouseCoordinatesMenuRect ( window, &rect );
	
	rect.right = rect.left;
	rect.left = 0;
	
	GGetPen ( &color, &size, &fill );
	mode = GGetPenMode();
	
	GSetPen ( GBlackColor(), 1, TRUE );
	GSetPenMode ( G_COPY_PEN );
	GDrawRect ( &rect );
	
	GSetPenColor ( GWhiteColor() );
    GSetPenFont ( IMAGE_WINDOW_FONT_NAME, IMAGE_WINDOW_FONT_SIZE, 0 );
	GGetStringSize ( text, 0, &width, &height );
	GDrawString ( text, 0, 3, rect.top + ( rect.bottom - rect.top - height ) / 2 + 1 );
	GDrawLine ( rect.left, rect.bottom, rect.right, rect.bottom );
	GSetPen ( &color, size, fill );
	GSetPenMode ( mode );
}

/*** DoShiftToolClick ********************************************************/

void DoShiftToolClick ( GWindowPtr window, short col, short row )
{
	short			col0, row0, col1, row1;
	short			left, top, right, bottom;
	short			mode;
	ImagePtr		image = GetImageWindowImage ( window );

	GStartDrawing ( window );
	mode = GGetPenMode();
	GSetPenMode ( G_INVERTING_PEN );
	
	/*** Compute the window coordinates of the left/top/right/bottom
	     boundaries of the image. ***/
	     
	left = 0;
	top = 0;
	right = GetImageColumns ( image ) - 1;
	bottom = GetImageRows ( image ) - 1;
	
	ImageToWindow ( window, &left, &top );
	ImageToWindow ( window, &right, &bottom );
	
	/*** If the mouse was clicked outside the image, return without doing
	     anything further. ***/
	     
	if ( row < top || row > bottom || col < left || col > right )
		return;
	
	/*** Un-select the selected part of the image, then save the
	     coordinates of the point where the mouse went down. ***/
		
	SelectNoneOfImage ( window );
	
	col0 = col;
	row0 = row;
	
	/*** Draw the initial crosshair at the mouse positition. ***/
	
	GDrawLine ( col, top, col, bottom );
	GDrawLine ( left, row, right, row );

	/*** While the mouse button is down, get the mouse's coordinates.
	     Each time the mouse moves, draw a crosshair at the mouse's
	     position as long as the mouse stays within the image. ***/
	
	while ( GMouseButtonIsDown ( 1 | 2 ) )
	{
		GGetMousePosition ( window, &col1, &row1 );
		
		if ( col != col1 || row != row1 )
		{
			/*** If the mouse's previous position was inside the image, draw
			     a crosshair at the previous position to erase it. ***/
			     
			if ( col >= left && col < right && row >= top && row < bottom )
			{
				GDrawLine ( col, top, col, bottom );
				GDrawLine ( left, row, right, row );
			}
	
			/*** If mouse's new position is inside the image, draw a crosshair
			     at the new position, and show pixel values/mouse coordinates
			     at that position. ***/
	
			if ( col1 >= left && col1 < right && row1 >= top && row1 < bottom )
			{
				GDrawLine ( col1, top, col1, bottom );
				GDrawLine ( left, row1, right, row1 );
	
				ShowPixelValues ( window, col1, row1 );
				DrawImageWindowMouseCoordinates ( window, col1, row1 );
			}
			
			/*** Save the mouse's position so it becomes the previous position
			     when the mouse moves again. ***/
			      		
			col = col1;
			row = row1;
		}
	}
	
	/*** If the mouse button is released inside the image, draw the crosshair
	     once more to erase it.  Then compute the image coordinates of the points
	     where the mouse was first clicked and last released. ***/
	
	if ( col >= left && col < right && row >= top && row < bottom )
	{
		float x, y, x0, y0;
		
		GDrawLine ( col, top, col, bottom );
		GDrawLine ( left, row, right, row );
		
		WindowToImageXY ( window, col0, row0, &x0, &y0 );
		WindowToImageXY ( window, col,  row,  &x,  &y );
		
		ShiftImageWindow ( window, x - x0, y - y0, FALSE );
	}

	GSetPenMode ( mode );
	GEndDrawing ( window );
}

/*** DoMarkToolClick **********************************************************/

void DoMarkToolClick ( GWindowPtr window, short col, short row )
{
	ImagePtr 		image = GetImageWindowImage ( window );
	short			width = GetImageColumns ( image );
	short			height = GetImageRows ( image );
	ImageObjectPtr	object = NULL;
	
	WindowToImage ( window, &col, &row );
	
	if ( col < 0 || col >= width || row < 0 || row >= height )
		return;
	
	FindLocalMaximumPixel ( GetImageDataFrame ( image, 0 ), width, height, &col, &row, 3 );
	
	object = CreateImageObject ( image, col, row, 1.3 );
	if ( object == NULL )
		return;
		
	AddImageObject ( image, object );
	GInvalidateWindow ( window, NULL );	
}

/***  CropImageWindow  **********************************************************

	Resizes an image window so that the window's content rectangle exactly
	matches the boundaries of the image it contains. 

	void CropImageWindow ( GWindowPtr window )

	(window): pointer to image window to crop.
		
	The function returns nothing.
	
*********************************************************************************/

void CropImageWindow ( GWindowPtr window )
{
	GRect		content;
	ImagePtr	image = NULL;
	
	/*** If the window is maximized, restore it first. ***/

    if ( GIsWindowMinimized ( window ) )
	    GRestoreWindow ( window );
    	
	/*** Compute the window's cropped content rectangle.  If the rectangle
	     fits inside the application's workspace, then set its actual content
	     rectangle to the result.  If the window would exceed the workspace
	     when cropped, simply maximize the window (if it's not maximized
	     already!) ***/
	
	if ( GetImageWindowCroppedContentRect ( window, &content ) )
	{
		if ( GIsWindowMaximized ( window ) )
			GRestoreWindow ( window );
			
		GSetWindowContentRect ( window, &content );
	}
	else
	{
		if ( ! GIsWindowMaximized ( window ) )
			GMaximizeWindow ( window );
	}

	/*** If the window is not visible, we will need to explicitly reposition
         its controls, since the window will not recieve resize events as a
         result of our maximize/restore calls. ***/

    if ( ! GIsWindowVisible ( window ) )
		SizeImageWindow ( window );
        
	/*** Center the image on its central row and column, move the
	     window's controls, then invalidate it so it gets redrawn. ***/

	image = GetImageWindowImage ( window );
	SetImageWindowDisplayCenterCol ( window, GetImageColumns ( image ) / 2 );
	SetImageWindowDisplayCenterRow ( window, GetImageRows ( image ) / 2 );
	GInvalidateWindow ( window, NULL );
}

/*** GetImageWindowCroppedContentRect *********************************************/

int GetImageWindowCroppedContentRect ( GWindowPtr window, GRectPtr rect )
{
	int			result = TRUE;
	short		numCols, numRows, width, height, zoom;
	GRect		frame, content, maximum, workspace;
	ImagePtr	image = NULL;
	
	/*** Determine the boundaries of the window's frame and content
	     rectangles, and the workspace rectangle, in screen coordinates. ***/
	     
	GGetWindowFrameRect ( window, &frame );
	GGetWindowContentRect ( window, &content );
	GWindowToScreenRect ( window, &content );
	GGetWorkspaceRect ( &workspace );
	
	/*** Determine the maximum content rectangle so that its frame
	     does not exceed the workspace. ***/
	     
	maximum.left = workspace.left + content.left - frame.left;
	maximum.top = workspace.top + content.top - frame.top;
	maximum.right = workspace.right + content.right - frame.right;
	maximum.bottom = workspace.bottom + content.bottom - frame.bottom;
	
	/*** Determine the image dimensions and display zoom factor. ***/
	    
	zoom = GetImageWindowDisplayZoom ( window );
	image = GetImageWindowImage ( window );
	numCols = GetImageColumns ( image );
	numRows = GetImageRows ( image );
	
	/*** Multiply or divide the image dimensions by the zoom factor,
	     then add room for the scroll bars. ***/

	if ( zoom >= 0 )
	{	
		width  = ( numCols <<  zoom ) + GGetScrollBarWidth()  - 1;
		height = ( numRows <<  zoom ) + GGetScrollBarHeight() - 1;
	}
	else
	{
		width  = ( numCols >> -zoom ) + GGetScrollBarWidth()  - 1;
		height = ( numRows >> -zoom ) + GGetScrollBarHeight() - 1;
	}
	
	/*** If the mouse coordinates are being displayed in the window,
	     add room for the mouse coordinate display area. ***/
	     
	if ( GetImageWindowDisplayMouseCoordinates ( window ) > MOUSE_COORDINATES_NONE )
		height += GGetScrollBarHeight();
	
	/*** Determine the window's content rectangle's left and
         right sides so that it doesn't overflow the workspace. ***/
	     
	content.right = content.left + width;
	if ( content.right > maximum.right )
	{
		result = FALSE;
		content.right = maximum.right;
		content.left = maximum.right - width;
		if ( content.left < maximum.left )
			content.left = maximum.left;
	}
	
	/*** Determine the window's content rectangle's top and
         bottom sides so that it doesn't overflow the workspace. ***/

	if ( content.top < maximum.top )
		content.top = maximum.top;
		
	content.bottom = content.top + height;
	if ( content.bottom > maximum.bottom )
	{
		result = FALSE;
		content.bottom = maximum.bottom;
		content.top = maximum.bottom - height;
		if ( content.top < maximum.top )
			content.top = maximum.top;
	}

	/*** If the caller wants the coordinates of the cropped content rectangle,
	     convert the content rectangle to window coordinates and return them,
	     then return TRUE/FALSE depending on whether or not we can actually
	     crop the window. ***/
	
	if ( rect != NULL )
	{
		GScreenToWindowRect ( window, &content );
		*rect = content;
	}
	
	return ( result );
}

/*** ScrollImageWindow *******************************************************

	Handles clicks in an image window's scroll bars.

	void ScrollImageWindow ( GWindowPtr window, short scroll, short part )
	
	(window): pointer to ImageWindow whose scroll bar has been clicked.
	(scroll): control ID of scroll bar which has been clicked.
	(part):   code indicating which part of scroll bar has been clicked.
	
	The function returns nothing.  Call this function after an ImageWindow
	recieves a control event corresponding to either its vertical or
	horizontal scroll bar.  The parameters associated with the event should
	indicate the scroll bar's control ID and part code; simply pass them
	along to this function.
	
******************************************************************************/

void ScrollImageWindow ( GWindowPtr window, short scroll, short part )
{
	long		zoom, scale, value, max, delta;
	short		right = 0, down = 0;
	GRect		imageRect;
	ImagePtr	image = GetImageWindowImage ( window );
	
	/*** Determine the size of the window area in which image data can
	     be displayed, and determine the scale at which the image is being
	     displated (i.e. window pixels per image pixel). ***/
	     
	GetImageWindowDisplayRect ( window, &imageRect );
	
	zoom = GetImageWindowDisplayZoom ( window );
	if ( zoom >= 0 )
		scale = 1 << zoom;
	else
		scale = 1 << -zoom;
	
	/*** Determine the value of the coordinate on which the image is
	     currently centered, as well as the maximum value of that
	     coordinate. ***/

	switch ( scroll )
	{
		case IMAGE_WINDOW_HORIZONTAL_SCROLL:
			value = GetImageWindowDisplayCenterCol ( window );
			max = GetImageColumns ( image );
			break;
			
		case IMAGE_WINDOW_VERTICAL_SCROLL:
			value = GetImageWindowDisplayCenterRow ( window );
			max = GetImageRows ( image );
			break;
	}
	
	/*** Now determine the amount by which the image center coordinate
	     should be changed. ***/
	
	switch ( part )
	{
		case G_SCROLL_LINE_UP:
		case G_SCROLL_LINE_DOWN:
		
			if ( zoom > 0 )
			{
				if ( part == G_SCROLL_LINE_UP )
					delta = -1;
	
				if ( part == G_SCROLL_LINE_DOWN )
					delta = 1;
				
				if ( scroll == IMAGE_WINDOW_VERTICAL_SCROLL )
					down = -delta * scale;
	
				if ( scroll == IMAGE_WINDOW_HORIZONTAL_SCROLL )
					right = -delta * scale;
			}
			else
			{
	
				if ( part == G_SCROLL_LINE_UP )
					delta = -scale;
	
				if ( part == G_SCROLL_LINE_DOWN )
					delta = scale;
				
				if ( scroll == IMAGE_WINDOW_VERTICAL_SCROLL )
					down = -delta / scale;
	
				if ( scroll == IMAGE_WINDOW_HORIZONTAL_SCROLL )
					right = -delta / scale;
			}
			break;
			
		case G_SCROLL_PAGE_UP:
		case G_SCROLL_PAGE_DOWN:
			if ( scroll == IMAGE_WINDOW_VERTICAL_SCROLL )
			{
				if ( part == G_SCROLL_PAGE_UP )
					delta = imageRect.top - imageRect.bottom;
					
				if ( part == G_SCROLL_PAGE_DOWN )
					delta = imageRect.bottom - imageRect.top;

				if ( zoom >= 0 )
				{
					delta = delta / scale;
					down = -delta * scale;
				}
				else
				{
					delta = delta * scale;
					down = -delta / scale;
				}
			}
			
			if ( scroll == IMAGE_WINDOW_HORIZONTAL_SCROLL )
			{
				if ( part == G_SCROLL_PAGE_UP )
					delta = imageRect.left - imageRect.right;

				if ( part == G_SCROLL_PAGE_DOWN )
					delta = imageRect.right - imageRect.left;
				
				if ( zoom > 0 )
				{
					delta = delta / scale;
					right = -delta * scale;
				}
				else
				{
					delta = delta * scale;
					right = -delta / scale;
				}
			}
			break;

		case G_SCROLL_THUMB:
		case G_TRACK_THUMB:
			value = GGetControlValue ( GGetControl ( window, scroll ) );
			switch ( scroll )
			{
				case IMAGE_WINDOW_HORIZONTAL_SCROLL:
					SetImageWindowDisplayCenterCol ( window, value );
					break;
					
				case IMAGE_WINDOW_VERTICAL_SCROLL:
					SetImageWindowDisplayCenterRow ( window, value );
					break;
			}

			GInvalidateWindow ( window, &imageRect );
			GUpdateWindow ( window );
			return;
	}

	/*** Determine the coordinate on which the image should be centered
	     after applying the scroll increment. ***/
	     
	value += delta;
	
	if ( value > max )
		value = max;
		
	if ( value < 0 )
		value = 0;

	/*** If the coordinate on which the image should be centered differs
	     from the coordinate on which it actually is currently centered,
	     update the image's central coordinate to match.  Otherwise, return
	     without doing anything further.  (The latter case happens when
	     the user has already scrolled to maximum coordinate and tries to
	     scroll further; returning here prevents flickering caused by
	     unnecessary redrawing.) ***/  

	switch ( scroll )
	{
		case IMAGE_WINDOW_HORIZONTAL_SCROLL:
			if ( value != GetImageWindowDisplayCenterCol ( window ) )
				SetImageWindowDisplayCenterCol ( window, value );
			else
				return;
			break;
			
		case IMAGE_WINDOW_VERTICAL_SCROLL:
			if ( value != GetImageWindowDisplayCenterRow ( window ) )
				SetImageWindowDisplayCenterRow ( window, value );
			else
				return;
			break;
	}

	/*** Now scroll the window's contents, and update the area invalidated
	     by moving the image. ***/
	     
	GScrollWindow ( window, right, down, &imageRect, &imageRect, TRUE );
	GUpdateWindow ( window );
}

/*** RestoreImageWindow *********************************************************/

void RestoreImageWindow ( GWindowPtr window )
{
	ImagePtr	image = GetImageWindowImage ( window );
	GImagePtr	bitmap = GetImageWindowBitmap ( window );
	
	if ( RestoreImagePreviousImage ( image ) )
	{
		if ( GGetImageWidth ( bitmap ) != GetImageColumns ( image ) || GGetImageHeight ( bitmap ) != GetImageRows ( image ) )
		{
			bitmap = NewImageWindowBitmap ( window );
			if ( bitmap == NULL )
				WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		}
	}
}

