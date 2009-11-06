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

/*** DoFileMenuItem *******************************************************/

void DoFileMenuItem ( long item )
{
	int	respond = TRUE;

	switch ( item )
	{
		case FILE_NEW_ITEM:
 			DoNew();
			break;

        case FILE_OPEN_ITEM:
			DoOpenDialog();
        	break;

		case FILE_CLOSE_ITEM:
			GCloseWindow ( GGetMenuEventWindow() );
			break;

		case FILE_SAVE_ITEM:
			DoSave ( GGetMenuEventWindow() );
			break;
			
		case FILE_SAVE_AS_ITEM:
            DoSaveAsDialog ( GGetMenuEventWindow() );
            break;
        
        case FILE_PAGE_SETUP_ITEM:
        	GDoPrintSetupDialog();
        	break;
        
        case FILE_PRINT_ITEM:
        	DoPrintDialog ( GGetMenuEventWindow() );
        	break;
        		   
		case FILE_QUIT_ITEM:
			GExitMainLoop ( 0 );
			break;
	}
}

/*** DoNew ****************************************************************/

void DoNew ( void )
{
	GWindowPtr	window;
	ImagePtr	image;

	image = NewImage ( "New Image", IMAGE_TYPE_MONOCHROME, 1, 240, 320 );
	if ( image == NULL )
	{
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		return;
	}
	
	window = NewImageWindow ( image );
	if ( window == NULL )
	{
		DeleteImage ( image );
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		return;
	}
	
    ComputeImageDisplayRange ( window );
	DrawImageWindowBitmap ( window );
	SetImageWindowNeedsSave ( window, TRUE );
    CropImageWindow ( window );
    GSetActiveWindow ( window );
}

/*** DoOpenFileDialog ****************************************************/

void DoOpenDialog ( void )
{
	char		prompt[256];
	char		filter[256];
	short		item = 1;
	GPathPtr	path;
	
	GGetString ( OPEN_FILE_PROMPT_STRING, prompt );
	GGetString ( OPEN_FILE_FILTER_STRING, filter );
	
	if ( ( path = GDoOpenFileDialog ( prompt, filter, &item ) ) != NULL )
	{
		OpenImageFile ( path );
		GDeletePath ( path );
	}
}

/*** DoSave ****************************************************************/

void DoSave ( GWindowPtr window )
{
	if ( GGetWindowClass ( window ) == IMAGE_WINDOW )
	{
		ImagePtr	image = GetImageWindowImage ( window );
		GPathPtr	path = GetImagePath ( image );
		
		if ( path == NULL )
			DoSaveImageDialog ( window );
		else
			SaveImageWindow ( window, path );
	}
	else if ( GGetWindowClass ( window ) == HEADER_WINDOW )
	{
		ImagePtr	image = GetHeaderWindowImage ( window );
		GPathPtr	path = GetImagePath ( image );
		
		window = GetImageWindow ( image ); 
		if ( path == NULL )
			DoSaveImageDialog ( window );
		else
			SaveImageWindow ( window, path );
	}
}

/*** DoSaveAsDialog ********************************************************/

void DoSaveAsDialog ( GWindowPtr window )
{
	if ( GGetWindowClass ( window ) == IMAGE_WINDOW )
		DoSaveImageDialog ( window );
	else if ( GGetWindowClass ( window ) == HISTOGRAM_WINDOW )
		DoSaveHistogramDialog ( window );
}

/*** DoSaveImageDialog  ******************************************************/

void DoSaveImageDialog ( GWindowPtr window )
{
	char		prompt[256];
    char		filter[256];
    short		format, formats[] = { FILE_TYPE_FITS, FILE_TYPE_GIF, FILE_TYPE_JPEG, FILE_TYPE_TIFF, FILE_TYPE_TIFF_16_BIT, FILE_TYPE_PICT };
    short		numformats = sizeof ( formats ) / sizeof ( formats[0] );
    short		item, i;
	GPathPtr	path;
	ImagePtr	image;
	CameraPtr	camera;
	
    /*** Get the prompt and filter strings for the dialog. ***/
       
	GGetString ( SAVE_IMAGE_PROMPT_STRING, prompt );
	GGetString ( SAVE_IMAGE_FILTER_STRING, filter );

	/*** Determine what camera (if any) is using the window to display
	     its exposures.  If the window is a camera exposure window,
	     set the default format displayed in the "Save" dialog to the
	     camera exposure format; otherwise, use the image's current
	     file format. ***/
	
	image = GetImageWindowImage ( window );
	camera = GetImageWindowCamera ( window );
	if ( camera == NULL )
		format = GetImageFileFormat ( image );
	else
		format = GetCameraExposureFormat ( camera );
		
	/*** Find the item in the list of file formats which corresponds
	     to the current file format. ***/
	     
	for ( item = 1, i = 0; i < numformats; i++ )
		if ( format == formats[i] )
			item = i + 1;
	
	/*** Now display the "Save As" dialog.  If the window is associated
	     with a camera, and the camera has an exposure path, use the file
	     title contained in the exposure path as the default file name
	     displayed in the "Save As" dialog.  Otherwise, display the image's
	     current title in the dialog. ***/
	
	if ( camera != NULL && ( path = GetCameraExposurePath ( camera ) ) != NULL )
	{
		char name[256];
		
		GGetPathName ( path, name );
		path = GDoSaveFileDialog ( prompt, filter, name, &item );
	}
	else
	{
		char *name;
		
    	name = GetImageTitle ( GetImageWindowImage ( window ) );
		path = GDoSaveFileDialog ( prompt, filter, name, &item );
	}
	
	/*** If the user selected a valid item from the "Format:" menu,
	     find the file format which it corresponds to. ***/
	     
	if ( item > 0 && item <= numformats )
		format = formats[ item - 1 ];
	
	/*** If "Save As..." dialog returned a valid path, save the
	     image window in the format selected by the user. ***/
	     	
	if ( path != NULL )
	{
    	/*** If the window is associated with a camera, set the camera's
    	     exposure format and path to those selected by the user.
    	     Then sever the link between the image window and the camera,
    	     so that new camera exposures willl not overwrite the image
    	     window's contents but appear in a new image window instead. ***/

    	if ( camera != NULL )
    	{
    		SetCameraExposureFormat ( camera, format );
    		SetCameraExposurePath ( camera, path );
	    	SetCameraExposureWindow ( camera, NULL );
	    	SetImageWindowCamera ( window, NULL );
    	}
    	
    	SetImageFileFormat ( image, format );
    	SaveImageWindow ( window, path );
		GDeletePath ( path );
	}
}

/*** DoSaveHistogramDialog *************************************************************/

void DoSaveHistogramDialog ( GWindowPtr window )
{
	char			prompt[256];
    char			filter[256];
    char			title[256];
    short			format, formats[] = { FILE_TYPE_TEXT, FILE_TYPE_JPEG, FILE_TYPE_TIFF, FILE_TYPE_PICT };
    short			numformats = sizeof ( formats ) / sizeof ( formats[0] );
    static short	item = 1;
	GPathPtr		path;
	
    /*** Get the prompt and filter strings for the dialog. ***/
       
	GGetString ( SAVE_HISTOGRAM_PROMPT_STRING, prompt );
	GGetString ( SAVE_HISTOGRAM_FILTER_STRING, filter );

	/*** Display the "Save As..." dialog, with the current file format shown
	     in the "Format:" menu and the window title shown in the "Name:" text field. ***/
			 
    GGetWindowTitle ( window, title );
	path = GDoSaveFileDialog ( prompt, filter, title, &item );

	/*** If the user selected a valid item from the "Format:" menu,
	     find the file format which it corresponds to. ***/
	     
	if ( item > 0 && item <= numformats )
		format = formats[ item - 1 ];
	
	/*** If "Save As..." dialog returned a valid path, save the
	     image window in the format selected by the user. ***/
	     	
	if ( path != NULL )
	{
    	SaveHistogramWindow ( window, path, format );
		GDeletePath ( path );
	}
}

/*** DoPrintDialog ************************************************************************/

void DoPrintDialog ( GWindowPtr window )
{
	char			title[256];
	static short	firstPage = 1, lastPage = 1, numCopies = 1, copy;
	GRect			pageRect;
	
	if ( GDoPrintJobDialog ( &firstPage, &lastPage, &numCopies ) == FALSE )
		return;
	
	GGetWindowTitle ( window, title );
	
	if ( GStartPrintJob ( title ) )
	{
		GGetPageRect ( &pageRect );
		
		for ( copy = 0; copy < numCopies; copy++ )
		{
			if ( GGetWindowClass ( window ) == HISTOGRAM_WINDOW )
				if ( PrintHistogramWindow ( window, &pageRect ) == FALSE )
					break;
		}
	}
	
	GEndPrintJob();
}
