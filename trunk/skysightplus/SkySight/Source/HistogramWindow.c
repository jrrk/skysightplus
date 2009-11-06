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

/*** local data types, constants, and macros ***/

typedef struct HistogramWindowData
{
	ImagePtr			image;
	ImageHistogramPtr	histogram;
}
HistogramWindowData, *HistogramWindowDataPtr;

#define GetHistogramWindowData(window) ( (HistogramWindowDataPtr) GGetWindowData ( window ) )

/*** local functions ***/

static int		DoHistogramWindowEvent ( short, GWindowPtr, long, long );
static void		DoHistogramWindowClick ( GWindowPtr, short, short );

static void		DeleteHistogramWindow ( GWindowPtr );
static void		DrawImageHistogramTickMarks ( ImageHistogramPtr, GRectPtr );
static void		GetHistogramWindowHistogramRect ( GWindowPtr, GRectPtr );

static int SaveHistogramWindowAsText ( GWindowPtr, GPathPtr );
static int SaveHistogramWindowAsPicture ( GWindowPtr, GPathPtr );

/*** CreateHistogramWindow **************************************************************

	Creates a new window which contains a histogram of values in the selected region
	of a particular image window.
	
	GWindowPtr CreateHistogramWindow ( GWindowPtr imageWindow )
	
	(imageWindow): pointer to image window.
	
*****************************************************************************************/

GWindowPtr CreateHistogramWindow ( GWindowPtr imageWindow )
{
	char					title[256];
	GWindowPtr				window;
	ImagePtr				image;
	HistogramWindowDataPtr	data;
	
	/*** Get the image window's image title, append ": Histogram",
	     and use the result as the histogram window's title. ***/
	     
	image = GetImageWindowImage ( imageWindow );
	
	strcpy ( title, GetImageTitle ( image ) );
	strcat ( title, ": Histogram" );
	
	/*** Create the window; on failure, return a NULL pointer. ***/
		
	window = GCreateWindow ( HISTOGRAM_WINDOW, title, -1, -1, -1, -1, FALSE,
	         G_APPLICATION_WINDOW, 0, DoHistogramWindowEvent );

	if ( window == NULL )
		return ( NULL );
	
	/*** Create the window's data record.  On failure, destroy the window
	     and return NULL. If successful, attach a pointer to the window data
	     record to the window itself. ***/
	     
	data = (HistogramWindowDataPtr) malloc ( sizeof ( HistogramWindowData ) );
	if ( data == NULL )
	{
		GDeleteWindow ( window );
		return ( NULL );
	}
	
	data->histogram = NULL;
	
	GSetWindowData ( window, (long) data );
	
	SetHistogramWindowImage ( window, GetImageWindowImage ( imageWindow ) );
	ComputeHistogramWindowHistogram ( window );

	
	GSetActiveWindow ( window );
	return ( window );
}

/*** DeleteHistogramWindow ******************************************************

	Deletes a histogram window and all associated data.

	void DeleteHistogramWindow ( GWindowPtr window )
	
	(window): pointer to histogram window to delete.
	
	This function returns nothing.
	
*********************************************************************************/

void DeleteHistogramWindow ( GWindowPtr window )
{
	HistogramWindowDataPtr	data = GetHistogramWindowData ( window );
	ImagePtr				image = data->image;
	ImageHistogramPtr		histogram = NULL;
	
	/*** Free the header window's associated data, then destroy the
	     window itself. ***/
	
	if ( data->histogram != NULL )
		DeleteImageHistogram ( data->histogram );
			
	free ( data );
	GDeleteWindow ( window );
	
	/*** If we are storing a pointer to the window in the associated
	     image record, set that pointer to NULL since we just destroyed
	     the image's associated histogram window. ***/
	     
	if ( GetImageHistogramWindow ( image ) == window )
		SetImageHistogramWindow ( image, NULL );
}

/*** DoHistogramWindowEvent *******************************************************

	This is the main event-handling procedure for a histogram window.
	
***********************************************************************************/

int DoHistogramWindowEvent ( short event, GWindowPtr window, long param1, long param2 )
{
	int							respond = TRUE;
	GRect						rect;
	HistogramWindowDataPtr		data = GetHistogramWindowData ( window );
	
	switch ( event )
	{
		case G_CLOSE_EVENT:
			DeleteHistogramWindow ( window );
			respond = FALSE;
			break;
		
		case G_ACTIVATE_EVENT:
			UpdateMenus();
			break;
		
		case G_MOUSE_DOWN_EVENT:
			DoHistogramWindowClick ( window, param1, param2 );
			break;
			
		case G_SIZE_EVENT:
			GetHistogramWindowHistogramRect ( window, &rect );
			SetHistogramDisplayRect ( data->histogram, &rect );
			break;
			
		case G_UPDATE_EVENT:
			GStartDrawing ( window );
			DrawImageHistogram ( data->histogram );
			GEndDrawing ( window );
			break;
	}
	
	return ( respond );
}

/*** DoHistogramWindowClick ***************************************************************

	Handles mouse-down events which ocurr in a histogram window.

	void DoHistogramWindowClick ( GWindowPtr window, short col1, short row1 )

	(window): pointer to histogram wihdow.
	(col1):   column in window coordinates where mouse was clicked.
	(row1):   row in window coordinates where mouse was clicked.

	The function returns nothing.
			
*******************************************************************************************/

void DoHistogramWindowClick ( GWindowPtr window, short col1, short row1 )
{
	short				col, row, col2, row2;
	short				left, top, right, bottom;
	short				mode;
	long				minCount, maxCount;
	PIXEL				minValue, maxValue;
	GRect				windowRect;
	ImageHistogramPtr	histogram = GetHistogramWindowHistogram ( window );

	/*** Obtain the window's content rectangle.  We'll use this to determine
	     whether the mouse is inside the window, and provide the user a means
	     of cancelling the select action (e.g. by releasing the mouse outside
	     the window. ***/
	     
	GGetWindowContentRect ( window, &windowRect );
	
	/*** Start drawing in the window.  Set the pen mode so that it inverts
	     background pixels.  Then draw a rectangle which represents the initial
	     selection (e.g. where the mouse went down.) ***/
	     
	GStartDrawing ( window );
	mode = GGetPenMode();
	GSetPenMode ( G_INVERTING_PEN );
	GSetPenFill ( FALSE );
	
	left = right = col2 = col1;
	top = bottom = row2 = row1;
	
	GDrawRectangle ( left, top, right, bottom );
	
	/*** While the mouse button is down, get the mouse's coordinates.
	     If the coordinates have changed, redraw the current rectangle
	     to erase it, then draw the new rectangle. ***/
	
	while ( GMouseButtonIsDown ( 1 | 2 ) )
	{
		GGetMousePosition ( window, &col, &row );
		
		if ( col != col2 || row != row2 )
		{
			/*** Only erase the previous rectangle if it falls within the
			     window boundaries. ***/
			
			if ( GIsPointInRect ( col2, row2, &windowRect ) )
				GDrawRectangle ( left, top, right, bottom );

			col2 = col;
			row2 = row;
			
			if ( col1 <= col2 )
			{
				left = col1;
				right = col2;
			}
			else
			{
				left = col2;
				right = col1;
			}
				
			if ( row1 <= row2 )
			{
				top = row1;
				bottom = row2;
			}
			else
			{
				top = row2;
				bottom = row1;
			}

			/*** Only draw the new rectangle if it falls within the
			     window boundaries. ***/
			     
			if ( GIsPointInRect ( col2, row2, &windowRect ) )
				GDrawRectangle ( left, top, right, bottom );
		}
	}

	/*** When the mouse button is released, draw the current rectangle to erase it,
	     restore the pen mode, and end drawing. ***/
	
	if ( GIsPointInRect ( col2, row2, &windowRect ) )
		GDrawRectangle ( left, top, right, bottom );
	
	GSetPenMode ( mode );
	GEndDrawing ( window );
	
	/*** If the mouse was released outside the window boundaries, do nothing more. ***/
	
	if ( ! GIsPointInRect ( col2, row2, &windowRect ) )
		return;
		
	/*** Compute the pixel value and pixel count which correspond to the corners
	     of the rectangle that the user selected.  Clip the minimum count to zero. ***/
	     
	HistogramColRowToValueCount ( histogram, left, top, &minValue, &maxCount );
	HistogramColRowToValueCount ( histogram, right, bottom, &maxValue, &minCount );
	
	if ( minCount < 0 )
		minCount = 0;
	
	if ( maxCount == minCount )
		maxCount = minCount + 1;
		
	if ( minValue < GetImageHistogramMinimumValue ( histogram ) )
		minValue = GetImageHistogramMinimumValue ( histogram );
	
	if ( maxValue > GetImageHistogramMaximumValue ( histogram ) )
		maxValue = GetImageHistogramMaximumValue ( histogram );

	if ( maxValue == minValue )
		maxValue = minValue + GetImageHistogramResolution ( histogram );
			
	/*** Now set the minima and maxima of the pixel value and count axes displayed
	     in the histogram window to the values computed above.  When done, invalidate
	     the window so that the histogram gets redrawn. ***/
		
	SetHistogramDisplayMinimumValue ( histogram, minValue );
	SetHistogramDisplayMaximumValue ( histogram, maxValue );
	SetHistogramDisplayMinimumCount ( histogram, minCount );
	SetHistogramDisplayMaximumCount ( histogram, maxCount );
	
	GInvalidateWindow ( window, NULL );
}

/*** SetHistogramWindowTitle ************************************************

	Sets a histogram window's title so it correctly matches the title
	of the associated image.

	void SetHistogramWindowTitle ( GWindowPtr window )
	
	(window): pointer to a histogram window.
	
	Call this function after you have associated an image with the
	histogram window, or changed the title of the underlying image.
	
**************************************************************************/

void SetHistogramWindowTitle ( GWindowPtr window )
{
	char		title[256];
	ImagePtr	image = GetHistogramWindowData ( window )->image;
	
	strcpy ( title, GetImageTitle ( image ) );
	strcat ( title, " (histogram)" );

	GSetWindowTitle ( window, title );
}

/*** SetHistogramWindowImage *************************************************

	Associates an image with a histogram window.
	
	void SetHistogramWindowImage ( GWindowPtr window, ImagePtr image )
	
	(window): pointer to histogram window in which to display data.
	(image):  pointer to image whose histogram should be displayed.
	
	The function returns nothing.
	
	After calling this function, the window's title is set to the image's
	title, with a "histogram" suffix appended.
	
******************************************************************************/

void SetHistogramWindowImage ( GWindowPtr window, ImagePtr image )
{
	HistogramWindowDataPtr	data = GetHistogramWindowData ( window );
	
	/*** Store a pointer to the image in the header window's
	     private data record; similarly, link the image's header
	     window pointer back to the window.  Then set the histogram
	     window's title properly. ***/
	     
	data->image = image;
	SetImageHistogramWindow ( image, window );
	SetHistogramWindowTitle ( window );
}

/*** GetHistogramWindowImage ***********************************************

	Returns a pointer to the image associated with a histogram window.

	ImagePtr GetHistogramWindowImage ( GWindowPtr window )
	
	(window): pointer to histogram window.
	
	The function returns a pointer to the window's associated image.
	
****************************************************************************/

ImagePtr GetHistogramWindowImage ( GWindowPtr window )
{
	HistogramWindowDataPtr	data = GetHistogramWindowData ( window );
	
	return ( data->image );
}

/*** GetHistogramWindowHistogram *******************************************

	Returns a pointer to the histogram associated with a histogram window.

	ImageHistogramPtr GetHistogramWindowHistogram ( GWindowPtr window )
	
	(window): pointer to histogram window.
	
	The function returns a pointer to the window's associated histogram.
	
****************************************************************************/

ImageHistogramPtr GetHistogramWindowHistogram ( GWindowPtr window )
{
	HistogramWindowDataPtr	data = GetHistogramWindowData ( window );
	
	return ( data->histogram );
}

/*** ComputeHistogramWindowHistogram ****************************************

	Computes a histogram of values in the selected region of an image
	associated with a histogram window.
	
	void ComputeHistogramWindowHistogram ( GWindowPtr window )

	(window): pointer to histogram window.
	
	On a successful return, the histogram window's histogram structure will
	be replaced with a new one containing this histogram of values in the
	image's selected region.
	
****************************************************************************/

void ComputeHistogramWindowHistogram ( GWindowPtr window )
{
	HistogramWindowDataPtr	data = GetHistogramWindowData ( window );
	ImagePtr				image = data->image;
	ImageHistogramPtr		histogram = data->histogram;
	ImageRegionPtr			region = GetImageWindowSelectedRegion ( GetImageWindow ( image ) );
	PIXEL					min, max;
	GRect					rect;
	
	/*** If the image window has no associated selected region, return
	     we'll create an empty histogram for holding pixel values
	     between 0 and 255.  Otherwise, we'll create a new histogram to
	     contain all of the image data values in the selected region to
	     a bin resolution of 1 pixel unit.***/
	     
	if ( region == NULL )
	{
		min = 0;
		max = 255;
	}
	else
	{
		min = GetImageRegionMin ( region );
		max = GetImageRegionMax ( region );
	}
	
	/*** Create the histogram.  On failure, display an error message and get out. ***/
		
	histogram = CreateImageHistogram ( min, max, 1.0, 0 );
	if ( histogram == NULL )
	{
		// DISPLAY AN ERROR?
		return;
	}
	
	/*** If the histogram window already has an existing histogram
	     structure associated, delete it, and replace it with the new
	     one.  ***/
	     
	if ( data->histogram != NULL )
		DeleteImageHistogram ( data->histogram );
	
	data->histogram = histogram;
	
	/*** If there is in fact a selected region, compute the actual
	     histogram values in that region. ***/

	if ( region != NULL )
	{
		ComputeImageRegionHistogram ( image, region, histogram );

	}

	/*** Set the maximum pixel count displayed in the histogram to the actual peak bin
	     count contained in the histogram.  Then compute the optimal rectangle in which
	     the histogram should be displayed.  (We need to set the maximum value first
	     because that affects the width of the pixel count axis label, which in turn
	     affects the size and position of the rectangle.) ***/

	SetHistogramDisplayMaximumCount ( histogram, GetImageHistogramPeakBinCount ( histogram ) );

	GetHistogramWindowHistogramRect ( window, &rect );
	SetHistogramDisplayRect ( histogram, &rect );
}

/*** GetHistogramWindowHistogramRect **********************************************************

	Finds the boundaries of the rectangle in which a histogram window's histogram should be
	displayed.

	void GetHistogramWindowHistogramRect ( GWindowPtr window, GRectPtr rect )
	
	(window): pointer to histogram window.
	(rect):   receives histogram rectangle.
	
	The coordinates of the rectangle are returned in units of pixels in the window's local
	coordinate system.
	
***********************************************************************************************/

void GetHistogramWindowHistogramRect ( GWindowPtr window, GRectPtr rect )
{
	char				label[256];
	short				width, height;
	ImageHistogramPtr	histogram = GetHistogramWindowHistogram ( window );
	
	GGetWindowContentRect ( window, rect );

	sprintf ( label, "%ld", GetImageHistogramPeakBinCount ( histogram ) );
	
	GStartDrawing ( window );
	GGetStringSize ( label, 0, &width, &height );
	GEndDrawing ( window );
	
	rect->top += 10;
	rect->right -= 10;
	rect->bottom -= height + 25;
	rect->left += width + 25;
}

/*** CreateHistogramWindowPicture *****************************************************

	Creates a picture (vector-graphic object) which represents a drawing of the
	histogram contained in a histogram window.
	
	GPicturePtr CreateHistogramWindowPicture ( GWindowPtr window )

	(window): pointer to histogram window.
	
	The function returns a pointer to the histogram picture, or NULL on failure.
	
	The picture returned by this function can be deleted with GDeletePicture().
	
***************************************************************************************/

GPicturePtr CreateHistogramWindowPicture ( GWindowPtr window )
{
	GRect				rect;
	GPicturePtr			picture;
	ImageHistogramPtr	histogram = GetHistogramWindowHistogram ( window );
	
	GGetWindowContentRect ( window, &rect );
	
	picture = GStartDrawingPicture ( window, &rect );
	if ( picture == NULL )
	{
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		return ( NULL );
	}

	DrawImageHistogram ( histogram );
	GEndDrawingPicture ( window, picture );
	
	return ( picture );
}

/*** SaveHistogramWindow ******************************************************/

int SaveHistogramWindow ( GWindowPtr window, GPathPtr path, short format )
{
	int result = FALSE;
	
	if ( format == FILE_TYPE_TEXT )
		result = SaveHistogramWindowAsText ( window, path );
	else if ( format == FILE_TYPE_PICT )
		result = SaveHistogramWindowAsPicture ( window, path );

	return ( result );
}

/*** SaveHistogramWindowAsText ************************************************/

int SaveHistogramWindowAsText ( GWindowPtr window, GPathPtr path )
{
	ImageHistogramPtr	histogram = GetHistogramWindowHistogram ( window );
	FILE				*file;
	char				name[256];
	long				bin, bins, count;
	double				value;
	
	/*** Get the file title from the path specification record.  Open a file
	     for writing in text mode at that location; display a warning message
	     and return an error code if we fail. ***/
	
	GGetPathName ( path, name );
	file = GOpenFile ( path, "w", "TEXT", "ttxt" );
	if ( file == NULL )
	{
		WarningMessage ( G_OK_ALERT, CANT_OPEN_FILE_STRING, name );
		return ( FALSE );
	}
	
	/*** For each bin in the histogram, write a line to the text file which
	     gives the image data value corresponding to the bin's lower bound,
	     and then give the pixel count of the bin. ***/
	
	bins = GetImageHistogramNumBins ( histogram );
	for ( bin = 0; bin < bins; bin++ )
	{
		value = GetImageHistogramBinValue ( histogram, bin );
		count = GetImageHistogramBinCount ( histogram, bin );
		
		fprintf ( file, "%f\t%ld\n", value, count );
	}
	
	/*** Close the file, and return a result code indicating success. ***/
	
	fclose ( file );
	return ( TRUE );
}

/*** SaveHistogramWindowAsPicture **********************************************/

int SaveHistogramWindowAsPicture ( GWindowPtr window, GPathPtr path )
{
	int					result;
	char				name[256];
	GPicturePtr			picture;
	
	/*** First, create a picture object from the histogram window. ***/
	
	picture = CreateHistogramWindowPicture ( window );
	if ( picture == NULL )
		return ( FALSE );
	
	/*** Now, write the picture to a file at the specified path location.
	     If we fail, display an error message and return a result code. ***/
	     	
	GGetPathName ( path, name );
	result = GWritePictureFile ( path, picture );
	if ( result <= 0 )
	{
		WarningMessage ( G_OK_ALERT, CANT_WRITE_FILE_STRING, name );
		result = FALSE;
	}
	else
	{
		result = TRUE;
	}
	
	GDeletePicture ( picture );
	return ( result );
}

/*** PrintHistogramWindow *********************************************************/

int PrintHistogramWindow ( GWindowPtr window, GRectPtr page )
{
	int					result;
	GRect				histRect;
	GRect				pageRect;
	ImageHistogramPtr	histogram = GetHistogramWindowHistogram ( window );
	
	if ( GStartDrawingPage() )
	{
		DrawImageHistogram ( histogram );
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}
	
	GEndDrawingPage();
	return ( result );
}

/*** CopyHistogramWindow ***********************************************************/

int CopyHistogramWindow ( GWindowPtr window )
{
	GPicturePtr		picture;
	
	picture = CreateHistogramWindowPicture ( window );
	if ( picture == NULL )
		return ( FALSE );
		
	GCopyPictureToClipboard ( picture );
	GDeletePicture ( picture );
	return ( TRUE );
}
