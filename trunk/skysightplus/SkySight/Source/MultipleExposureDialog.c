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

#define MULTIPLE_NUMBER_TEXT				4
#define MULTIPLE_COMBINE_BUTTON				5
#define MULTIPLE_SEPARATE_BUTTON			6
#define MULTIPLE_SAVE_BUTTON				7
#define MULTIPLE_NAME_TEXT					8

/*** local functions ***/

static GPathPtr DoMultipleExposureSaveDialog ( GPathPtr, char *, short * );

/*** DoMultipleExposureDialogEvent ***/

int DoMultipleExposureDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int				respond = TRUE;
	short			count;
	char			text[256];
	CameraPtr		camera = (CameraPtr) GGetWindowData ( dialog );
	static short	format;
	static char		name[256];
	static GPathPtr	path = NULL;
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
			count = GetCameraExposureTotal ( camera );
			sprintf ( text, "%hd", count );
			GSetDialogItemText ( dialog, MULTIPLE_NUMBER_TEXT, text );
			
			/*** Set the "Seprate Image" or "Combined Image" radio button
			     depending on the camera's exposure mode. ***/
			
			if ( GetCameraExposureMode ( camera ) == EXPOSURE_MODE_COMBINED_IMAGE )
				GSetSelectedDialogButton ( dialog, MULTIPLE_COMBINE_BUTTON, MULTIPLE_SEPARATE_BUTTON, MULTIPLE_COMBINE_BUTTON );
			else
				GSetSelectedDialogButton ( dialog, MULTIPLE_COMBINE_BUTTON, MULTIPLE_SEPARATE_BUTTON, MULTIPLE_SEPARATE_BUTTON );
			
			strcpy ( name, GetCameraExposureName ( camera ) );
			GSetDialogItemText ( dialog, MULTIPLE_NAME_TEXT, name );
			
			/*** Make a copy of the camera's current exposure path, or
			     (if the camera has none) a copy of the current working
			     directory. ***/
			     
			path = GetCameraExposurePath ( camera );
			if ( path == NULL )
			{
				path = GCreatePath ( NULL );
				if ( path != NULL )
					GGetWorkingDirectory ( path );
			}
			else
			{
				path = GCreatePath ( path );
			}
			
			format = GetCameraExposureFormat ( camera );
			break;
			
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					
					/*** Set the camera's exposure mode, depending on whether the "Seprate Image" or
					    "Combined Image" radio button is selected. ***/

					if ( GGetSelectedDialogButton ( dialog, MULTIPLE_COMBINE_BUTTON, MULTIPLE_SEPARATE_BUTTON ) == MULTIPLE_COMBINE_BUTTON )
						SetCameraExposureMode ( camera, EXPOSURE_MODE_COMBINED_IMAGE );
					else
						SetCameraExposureMode ( camera, EXPOSURE_MODE_MULTIPLE_IMAGE );
					
					GGetDialogItemText ( dialog, MULTIPLE_NUMBER_TEXT, text );
					if ( sscanf ( text, "%hd", &count ) == 1 )
						SetCameraExposureTotal ( camera, count );

					SetCameraExposureFormat ( camera, format );
					SetCameraExposureName ( camera, name );
					SetCameraExposurePath ( camera, path );
					GDeletePath ( path );					
					
					GExitModalDialogLoop ( param1 );
					break;
					
				case G_CANCEL_BUTTON:
					GExitModalDialogLoop ( param1 );
					break;
					
				case MULTIPLE_COMBINE_BUTTON:
				case MULTIPLE_SEPARATE_BUTTON:
					GSetDialogControlValue ( dialog, MULTIPLE_COMBINE_BUTTON, FALSE );
					GSetDialogControlValue ( dialog, MULTIPLE_SEPARATE_BUTTON, FALSE );
					GSetDialogControlValue ( dialog, param1, TRUE );
					break;
					
				case MULTIPLE_SAVE_BUTTON:
					{
						GPathPtr newPath = DoMultipleExposureSaveDialog ( path, name, &format );
						if ( newPath != NULL )
						{
							GDeletePath ( path );
							path = newPath;
							
							GGetPathName ( path, name );
							GSetDialogItemText ( dialog, MULTIPLE_NAME_TEXT, name );
						}
					}
					break;
					
			}
			break;
	}
		
	return ( respond );
}

/*** DoMultipleExposureSaveDialog ***/

GPathPtr DoMultipleExposureSaveDialog ( GPathPtr path, char *name, short *format )
{
	char		prompt[256];
    char		filter[256];
    short		formats[] = { FILE_TYPE_FITS, FILE_TYPE_JPEG, FILE_TYPE_TIFF, FILE_TYPE_TIFF_16_BIT, FILE_TYPE_PICT };
    short		numformats = sizeof ( formats ) / sizeof ( formats[0] );
    short		item, i;
	
    /*** Get the prompt and filter strings for the dialog. ***/
       
	GGetString ( SAVE_IMAGE_PROMPT_STRING, prompt );
	GGetString ( SAVE_IMAGE_FILTER_STRING, filter );

	/*** Find the item in the list of file formats which corresponds
	     to the current file format. ***/
	     
	for ( item = 1, i = 0; i < numformats; i++ )
		if ( *format == formats[i] )
			item = i + 1;
	
	/*** Now display the "Save As" dialog.  If the window is associated
	     with a camera, and the camera has an exposure path, use the file
	     title contained in the exposure path as the default file name
	     displayed in the "Save As" dialog.  Otherwise, display the image's
	     current title in the dialog. ***/
	
	path = GDoSaveFileDialog ( prompt, filter, name, &item );
	
	/*** If "Save As..." dialog returned a valid path, save the
	     file format and file name selected by the user. ***/
	     	
	if ( path != NULL )
	{
		if ( item > 0 && item <= numformats )
			*format = formats[ item - 1 ];
	
		GGetPathName ( path, name );
	}
	
	return ( path );
}
