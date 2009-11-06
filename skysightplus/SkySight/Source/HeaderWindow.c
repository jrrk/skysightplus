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

#ifdef GMAC
#define EOL_SIZE	1
#define EOL_STR		"\r"
#define HEADER_FONT_NAME	"Courier"
#define HEADER_FONT_SIZE	10
#endif

#ifdef GWIN
#define EOL_SIZE	2
#define EOL_STR		"\r\n"
#define HEADER_FONT_NAME	"Courier New"
#define HEADER_FONT_SIZE	14
#endif

#define HEADER_EDIT_TEXT		1

/*** local data types ***/

typedef struct HeaderWindowData
{
	ImagePtr image;
}
HeaderWindowData, *HeaderWindowDataPtr;

#define GetHeaderWindowData(window) ( (HeaderWindowDataPtr) GGetWindowData ( window ) )

/*** local function prototypes ***/

static int DoHeaderWindowEvent ( short, GWindowPtr, long, long );
static void EditTextToFITSHeader ( GControlPtr, char *** );

/*** CreateHeaderWindow ***/

GWindowPtr CreateHeaderWindow ( GWindowPtr imageWindow )
{
	char				title[256];
	GWindowPtr			window;
	GControlPtr			control;
	GRect				rect;
	HeaderWindowDataPtr	data;
	
	strcpy ( title, "Header" );

	/*** Create the window; on failure, return a NULL pointer. ***/
		
	window = GCreateWindow ( HEADER_WINDOW, title, -1, -1, -1, -1, FALSE,
	         G_APPLICATION_WINDOW, 0, DoHeaderWindowEvent );

	if ( window == NULL )
		return ( NULL );
	
	/*** Create the window's editable-text control.  On failure, destroy
	     the window and return NULL. ***/
	
	GGetWindowContentRect ( window, &rect );
	GInsetRect ( &rect, -1, -1 );	
	control = GCreateEditText ( window, HEADER_EDIT_TEXT, "", &rect, TRUE, TRUE, FALSE );
	if ( control == NULL )
	{
		GDeleteWindow ( window );
		return ( NULL );
	}
	
	/*** Create the window's data record.  On failure, destroy the window
	     and return NULL. If successful, attach a pointer to the window data
	     record to the window itself. ***/
	     
	data = (HeaderWindowDataPtr) malloc ( sizeof ( HeaderWindowData ) );
	if ( data == NULL )
	{
		GDeleteWindow ( window );
		return ( NULL );
	}
	
	GSetWindowData ( window, (long) data );
	
	GSetEditTextWrap ( control, FALSE );
	GSetEditTextFont ( control, HEADER_FONT_NAME, HEADER_FONT_SIZE, G_PLAIN_STYLE );
	
	SetHeaderWindowImage ( window, GetImageWindowImage ( imageWindow ) );
	
	GSetActiveWindow ( window );
	return ( window );
}

/*** DeleteHeaderWindow ***/

void DeleteHeaderWindow ( GWindowPtr window )
{
	HeaderWindowDataPtr	data = GetHeaderWindowData ( window );
	ImagePtr			image = data->image;

	/*** Free the header window's data record, then destroy the
	     header window itself. ***/
	     	
	free ( data );
	GDeleteWindow ( window );
	
	/*** If we are storing a pointer to the window in the associated
	     image record, set that pointer to NULL since we just destroyed
	     the image's associated header window. ***/
	     
	if ( GetImageHeaderWindow ( image ) == window )
		SetImageHeaderWindow ( image, NULL );
}

/*** DoHeaderWindowEvent ***/

int DoHeaderWindowEvent ( short event, GWindowPtr window, long param1, long param2 )
{
	int			respond = TRUE;
	GRect		rect;
	GControlPtr	control;
	
	switch ( event )
	{
		case G_CLOSE_EVENT:
		{
			GControlPtr		control = GGetControl ( window, HEADER_EDIT_TEXT );
			ImagePtr		image = GetHeaderWindowImage ( window );
			FITSImagePtr	fits = GetImageFITSImage ( image );
			
			EditTextToFITSHeader ( control, &(fits->header) );
			DeleteHeaderWindow ( window );
			respond = FALSE;
		}
		break;
		
		case G_ACTIVATE_EVENT:
			control = GGetControl ( window, HEADER_EDIT_TEXT );
			GSetKeyEventControl ( control );
			UpdateMenus();
			break;
				
		case G_SIZE_EVENT:
			control = GGetControl ( window, HEADER_EDIT_TEXT );
			GGetWindowContentRect ( window, &rect );
			GInsetRect ( &rect, -1, -1 );	
			GSetControlRect ( control, &rect );
			break;
	}
	
	return ( respond );
}

/*** SetHeaderWindowTitle ************************************************

	Sets a header window's title so it correctly matches the title
	of the associated image.

	void SetHeaderWindowTitle ( GWindowPtr window )
	
	(window): pointer to a header window.
	
	Call this function after you have associated an image with the
	header window, or changed the title of the underlying image.
	
**************************************************************************/

void SetHeaderWindowTitle ( GWindowPtr window )
{
	char		title[256];
	ImagePtr	image = GetHeaderWindowData ( window )->image;
	
	strcpy ( title, GetImageTitle ( image ) );
	strcat ( title, " (header)" );

	GSetWindowTitle ( window, title );
}

/*** SetHeaderWindowImage ***/

void SetHeaderWindowImage ( GWindowPtr window, ImagePtr image )
{
	char				**header, *buffer, *bufferPtr, *line;
	long				numLines, lineNum;
	FITSImagePtr		fits;
	GControlPtr			control;
	HeaderWindowDataPtr	data = GetHeaderWindowData ( window );
	
	/*** Store a pointer to the image in the header window's
	     private data record; similarly, link the image's header
	     window pointer back to the window.  Then set the header
	     window's title properly. ***/
	     
	data->image = image;
	SetImageHeaderWindow ( image, window );
	SetHeaderWindowTitle ( window );
	
	/*** Onbtain a pointer to the image's FITS header matrix. ***/
	
	fits = GetImageFITSImage ( image );
	header = fits->header;
	
	/*** Determine the number of lines in the FITS header.
	     Only disply the first 360 lines in the header window,
	     if there are > 360. ***/
	
	numLines = CountFITSHeaderLines ( header );
	if ( numLines > 360 )
	{
		// display a warning here!
		numLines = 360;
	}
	
	buffer = malloc ( numLines * ( 80 + EOL_SIZE ) + 1 );
	if ( buffer == NULL )
	{
		// error message here!
		return;
	}

	for ( bufferPtr = buffer, lineNum = 0; lineNum < numLines; lineNum++ )
	{
		line = GetFITSHeaderLine ( header, lineNum );
		
		strncpy ( bufferPtr, line, 80 );
		strcpy ( bufferPtr + 80, EOL_STR );
		
		bufferPtr += 80 + EOL_SIZE;
	}
	
	*bufferPtr = '\0';
	
	control = GGetControl ( window, HEADER_EDIT_TEXT );
	GSetEditTextText ( control, buffer );
	
	free ( buffer );
}

/*** GetHeaderWindowImage ***/

ImagePtr GetHeaderWindowImage ( GWindowPtr window )
{
	HeaderWindowDataPtr	data = GetHeaderWindowData ( window );
	
	return ( data->image );
}

/*** EditTextToFITSHeader ***/

void EditTextToFITSHeader ( GControlPtr control, char ***header )
{
	long	lineNum, numLines, i;
	char	buffer[81];
	
	/*** Determine the number of lines of text in the control. ***/
	
	numLines = GGetEditTextNumLines ( control );
	
	for ( lineNum = 0; lineNum < numLines; lineNum++ )
	{
		/*** Grab a pointer to the start of the Nth line of text in the control.
		     Then copy the first 80 characters of the line (or up to the first
		     EOL character) from the line into a temporary buffer. ***/
		     
		GGetEditTextLine ( control, lineNum, buffer, 80 );
		
		/*** If the line ended before the 80th character, pad the buffer with
		     whitespace, since FITS header lines must all be exactly 80 characters.
		     Then terminate the buffer with an ASCII NUL. ***/
		
		for ( i = strlen ( buffer ); i < 80; i++ )
			buffer[i] = ' ';
			
		buffer[i] = '\0';
		
		/*** Finally, copy the buffer line into the actual FITS header. ***/
		
		SetFITSHeaderLine ( header, lineNum, buffer );
		
		/*** If we've copied a line containing the "END" keyword, proceed
		     no further, since there should be no data in the FITS header
		     after this keyword.  If we were to add data past this keyword,
		     it could make the FITS file unreadable.  ***/
		
		if ( TestFITSHeaderKeyword ( buffer, "END      " ) )
			break;
	}
}

/*** UpdateHeaderWindowImageHeader **********************************************

	Updates the FITS header of the image associated with a FITS header window,
	so that changes made by the user to the text displayed in the FITS header
	window are correctly propogated to the underlying image. 

	void UpdateHeaderWindowImageHeader ( GWindowPtr window )
	
	(window): pointer to a FITS header window.
	
	This function returns nothing.
	
	You should call this function when closing a FITS header window, and
	before saving the underlying FITS image.
	
*********************************************************************************/

void UpdateHeaderWindowImageHeader ( GWindowPtr window )
{
	ImagePtr		image = GetHeaderWindowImage ( window );
	FITSImagePtr	fits = GetImageFITSImage ( image );
	GControlPtr		control = GGetControl ( window, HEADER_EDIT_TEXT );
	
	EditTextToFITSHeader ( control, &(fits->header) );
	GetFITSImageHeaderInfo ( fits->header, &fits->bitpix, &fits->naxis,
	&fits->naxis1, &fits->naxis2, &fits->naxis3, &fits->bzero, &fits->bscale );
}
