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

#define CAMERA_EXPOSURE_START_BUTTON		3
#define CAMERA_EXPOSURE_MODE_TEXT			4
#define CAMERA_EXPOSURE_MODE_MENU			5
#define CAMERA_EXPOSURE_DARK_FRAME_MENU		7
#define CAMERA_EXPOSURE_BINNING_MENU		9
#define CAMERA_EXPOSURE_ANTIBLOOMING_MENU	11
#define CAMERA_EXPOSURE_FILTER_MENU			13
#define CAMERA_EXPOSURE_FILTER_DISPLAY_BOX	14
#define CAMERA_EXPOSURE_LENGTH_SLIDER		16
#define CAMERA_EXPOSURE_LENGTH_TEXT			17
#define CAMERA_EXPOSURE_STATUS_TEXT			19
#define CAMERA_EXPOSURE_STATUS_BAR			20

/*** local functions ***/

static void			BuildReadoutMenu ( GWindowPtr, CameraPtr );
static void			BuildFilterMenu ( GWindowPtr, CameraPtr );
static void			BuildAntibloomingMenu ( GWindowPtr, CameraPtr );

static float		ExposureControlValueToTime ( short );
static short		ExposureTimeToControlValue ( float );
static int			DownloadImage ( CameraPtr, ImagePtr, ImageRegionPtr );
static int			DoDownloadEvent ( CameraPtr, long, short, short, short );
static void			DarkFrameImageDataRow ( CameraPtr, short, short, short, PIXEL * );
static ExposureList	NewCameraExposureList ( CameraPtr camera );
static void			DeleteCameraExposureList ( CameraPtr camera );

/*** local data ***/

static int			sDisplayFilterColorTable = TRUE;

/*** DoCameraExposureDialogEvent *******************************************************

	Handles events related to the "Camera Temperature" panel of the "Camera Exposure"
	dialog when it is active.
	
****************************************************************************************/

int DoCameraExposureDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int				respond = TRUE;
	short			item, value;
	char			text[256];
	float			time;
	GControlPtr		control;
	CameraPtr		camera;
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
        	GSetWindowTitle ( GetCameraControlDialog(), "Camera Exposure Control" );
			camera = GetActiveCamera();
			if ( camera != NULL )
			{
				for ( item = CAMERA_EXPOSURE_START_BUTTON; item <= CAMERA_EXPOSURE_STATUS_TEXT; item++ )
					GEnableDialogItem ( dialog, item, TRUE );

				/*** Add the appropriate items to the "Exposure Mode" popup menu.
				     Select the menu item which corresponds to the camera's exposure
				     mode.  The "multiple image" and "combined image" modes both
				     correspond to item number 4; the user selects between them by
				     using the "multiple image" dialog that appears when that item
				     is selected. ***/
				     
				control = GGetControl ( dialog, CAMERA_EXPOSURE_MODE_MENU );
				GSetPopupMenuItemText ( control, 1, "Focus" );
				GSetPopupMenuItemText ( control, 2, "Image" );
				GSetPopupMenuItemText ( control, 3, "Color Image" );
				GSetPopupMenuItemText ( control, 4, "Multiple Image..." );
				
				item = GetCameraExposureMode ( camera );
				if ( item == EXPOSURE_MODE_FOCUS )
					GSetControlValue ( control, 1 );
				else if ( item == EXPOSURE_MODE_IMAGE )
					GSetControlValue ( control, 2 );
				else if ( item == EXPOSURE_MODE_COLOR_IMAGE )
					GSetControlValue ( control, 3 );
				else if ( item == EXPOSURE_MODE_MULTIPLE_IMAGE )
					GSetControlValue ( control, 4 );
				else if ( item == EXPOSURE_MODE_COMBINED_IMAGE )
					GSetControlValue ( control, 4 );
				
				/*** Add the appropriate items to the "Dark frame mode" menu. ***/
				
				control = GGetControl ( dialog, CAMERA_EXPOSURE_DARK_FRAME_MENU );
				GSetPopupMenuItemText ( control, 1, "Never" );
				GSetPopupMenuItemText ( control, 2, "Only" );
				GSetPopupMenuItemText ( control, 3, "Always" );
				GSetPopupMenuItemText ( control, 4, "Automatic" );
				GSetControlValue ( control, GetCameraDarkFrameMode ( camera ) );

				BuildReadoutMenu ( dialog, camera );
				BuildAntibloomingMenu ( dialog, camera );
				BuildFilterMenu ( dialog, camera );
				
				GSetDialogControlValue ( dialog, CAMERA_EXPOSURE_FILTER_DISPLAY_BOX, sDisplayFilterColorTable );
				
				time = GetCameraExposureLength ( camera );
				sprintf ( text, "%.2f", time );
				GSetDialogItemText ( dialog, CAMERA_EXPOSURE_LENGTH_TEXT, text );

				control = GGetControl ( dialog, CAMERA_EXPOSURE_LENGTH_SLIDER );
				GSetControlMin ( control, 1 );
				GSetControlMax ( control, 50 );
				GSetControlValue ( control, ExposureTimeToControlValue ( time ) );
				
				control = GGetControl ( dialog, CAMERA_EXPOSURE_STATUS_BAR );
				GSetControlMin ( control, 0 );
				GSetControlMax ( control, 100 );

				control = GGetControl ( dialog, CAMERA_EXPOSURE_START_BUTTON );
				if ( GetCameraStatus ( camera, NULL, NULL ) == CAMERA_STATUS_WAITING )
					GSetControlTitle ( control, "Start" );
				else
					GSetControlTitle ( control, "Stop" );
		
				UpdateCameraExposureDialog ( 0 );
				
				GSetDialogDefaultButton ( dialog, CAMERA_EXPOSURE_START_BUTTON );
			}
			else
			{
				for ( item = CAMERA_EXPOSURE_START_BUTTON; item <= CAMERA_EXPOSURE_STATUS_TEXT; item++ )
					GEnableDialogItem ( dialog, item, FALSE );
					
				GSetDialogItemText ( dialog, CAMERA_EXPOSURE_START_BUTTON, "Start" );
			}
			break;
			
		case G_CONTROL_EVENT:
			camera = GetActiveCamera();
			switch ( param1 )
			{
				/*** If the user makes a selection from the "Exposure Mode" menu, find out what
				     item they chose.  If they chose the "Multiple Image..." mode, display the
				     "Multiple Exposure Setup" dialog, and reset the exposure mode menu item
				     displayed to the camera's previous exposure mode if the user cancels.
				     (If the user hits the "OK" button, the exposure mode will be set inside
				     the modal dialog loop.) ***/
				     
				case CAMERA_EXPOSURE_MODE_MENU:
					item = GGetDialogControlValue ( dialog, param1 );
					if ( item == EXPOSURE_MODE_MULTIPLE_IMAGE )
					{
						if ( GEnterModalDialog ( MULTIPLE_EXPOSURE_DIALOG, (long) camera, DoMultipleExposureDialogEvent ) == G_CANCEL_BUTTON )
							GSetDialogControlValue ( dialog, param1, GetCameraExposureMode ( camera ) );
					}
					else
					{	
						SetCameraExposureMode ( camera, item );
					}
					break;
					
				case CAMERA_EXPOSURE_DARK_FRAME_MENU:
					item = GGetDialogControlValue ( dialog, param1 );
					SetCameraDarkFrameMode ( camera, item );
					break;
					
				case CAMERA_EXPOSURE_BINNING_MENU:
					item = GGetDialogControlValue ( dialog, param1 );
					SetCameraReadoutMode ( camera, item );
					break;
					
				case CAMERA_EXPOSURE_ANTIBLOOMING_MENU:
					item = GGetDialogControlValue ( dialog, param1 );
					SetCameraAntiblooming ( camera, item );
					break;
					
				case CAMERA_EXPOSURE_FILTER_MENU:
					item = GGetDialogControlValue ( dialog, CAMERA_EXPOSURE_FILTER_MENU );
					if ( MoveFilter ( camera, item ) == FALSE )
						GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't set desired filter!" );
					else
						SetCameraExposureFilter ( camera, item );
					break;
				
				case CAMERA_EXPOSURE_FILTER_DISPLAY_BOX:
					sDisplayFilterColorTable = ! sDisplayFilterColorTable;
					GSetDialogControlValue ( dialog, param1, sDisplayFilterColorTable );
					break;
					
				case CAMERA_EXPOSURE_LENGTH_SLIDER:
                    control = GGetControl ( dialog, CAMERA_EXPOSURE_LENGTH_SLIDER );
                    value = GGetControlValue ( control );
                    time = ExposureControlValueToTime ( value );
                    sprintf ( text, "%.2f", time );
                    GSetDialogItemText ( dialog, CAMERA_EXPOSURE_LENGTH_TEXT, text );
                    SetCameraExposureLength ( camera, time );
					break;
					
                case CAMERA_EXPOSURE_LENGTH_TEXT:
                    break;

				case CAMERA_EXPOSURE_START_BUTTON:
				
					/*** If the camera is waiting, and has no exposures pending, start a new exposure sequence.
					     Otherwise, cancel the exposure or download which is currently in progress, and delete
					     any remaining exposures in the camera's exposure list. ***/
					      
					if ( GetCameraStatus ( camera, NULL, NULL ) == CAMERA_STATUS_WAITING && GetCameraExposure ( camera ) == NULL )
					{
						GGetDialogItemText ( dialog, CAMERA_EXPOSURE_LENGTH_TEXT, text );

	                    sscanf ( text, "%f", &time );
	                    value = ExposureTimeToControlValue ( time );
	                    control = GGetControl ( dialog, CAMERA_EXPOSURE_LENGTH_SLIDER );
	                    GSetControlValue ( control, value );

						SetCameraExposureLength ( camera, time );
						StartExposure ( camera );
					}
					else
					{
						CancelExposure ( camera );
					}
					break;
			}
			
			break;
			
	}
	
    return ( respond );
}

/*** ExposureControlValueToTime ********************************************

	Converts the exposure length slider control value to an exposure
	time, in seconds.

	float ExposureControlValueToTime ( short value )
	
	(value): exposure time slider control value
	
	The function returns the equivalent exposure time, in seconds.
	
***************************************************************************/

float ExposureControlValueToTime ( short value )
{
	float time;
	
	if ( value <= 10 )
		time = 0.01 * value;
	else if ( value > 10 && value <= 20 )
		time = 0.1 * ( value - 10 );
	else if ( value > 20 && value <= 30 )
		time = 1.0 * ( value - 20 );
	else if ( value > 30 && value <= 40 )
		time = 10.0 * ( value - 30 );
	else if ( value > 40 && value <= 50 )
		time = 100.0 * ( value - 40 );
		
	return ( time );
}

/*** ExposureTimeToControlValue ********************************************

	Converts an exposure time in seconds to an exposure time slider control
	value.

	short ExposureTimeToControlValue ( float time )
	
	(value): exposure time, in seconds
	
	The function returns the equivalent exposure slider control value.
	
***************************************************************************/

short ExposureTimeToControlValue ( float time )
{
	short value;
	
	if ( time < 0.01 )
		value = 0;
	else if ( time >= 0.01 && time < 0.1 )
		value = time * 100.0;
	else if ( time >= 0.1 && time < 1.0 )
		value = 10 + time * 10.0;
	else if ( time >= 1.0 && time < 10.0 )
		value = 20 + time;
	else if ( time >= 10.0 && time < 100.0 )
		value = 30 + time / 10.0;
	else if ( time >= 100.0 && time < 1000.0 )
		value = 40 + time / 100.0;
	else if ( time >= 1000.0 )
		value = 50.0;
		
	return ( value );
}

/*** BuildReadoutMenu **********************************************************

	Builds the "Readout" popup menu from the camera's possible readout modes,
	and sets its current value to the camera's current readout mode.
	
********************************************************************************/

void BuildReadoutMenu ( GWindowPtr dialog, CameraPtr camera )
{
	char			text[256];
	short			item, mode, width, height;
	GControlPtr		control;
	
	if ( GetCameraReadoutMode ( camera, &mode ) == FALSE )
		return;

	control = GGetControl ( dialog, CAMERA_EXPOSURE_BINNING_MENU );
	
	for ( item = 1; GetCameraReadoutModeInfo ( camera, item, &width, &height, text ) == TRUE; item++ )
		GSetPopupMenuItemText ( control, item, text );
	
	GSetControlValue ( control, mode );
}

/*** BuildFilterMenu **********************************************************

	Builds the "Filter" popup menu from the camera's possible filter positions,
	and sets its current value to the camera's current filter position.
	
********************************************************************************/

void BuildFilterMenu ( GWindowPtr dialog, CameraPtr camera )
{
	char			text[256];
	short			position, filter, color;
	GControlPtr		control;
	
	control = GGetControl ( dialog, CAMERA_EXPOSURE_FILTER_MENU );

	/*** If the camera has no filter, display "None" as the filter position,
	     and disable the filter popup menu. ***/
	     
	if ( GetCameraFilter ( camera, &position ) == FALSE )
	{
		GSetPopupMenuItemText ( control, 1, "None" );
		GSetControlValue ( control, 1 );
		GEnableControl ( control, FALSE );
		GEnableDialogItem ( dialog, CAMERA_EXPOSURE_FILTER_MENU, FALSE );
		return;
	}
	
	for ( filter = 1; GetCameraFilterInfo ( camera, filter, &color, text ) == TRUE; filter++ )
		GSetPopupMenuItemText ( control, filter, text );

	GSetControlValue ( control, position );
}

/*** BuildAntibloomingMenu ********************************************************

	Builds the "Antiblooming" popup menu from the camera's possible antiblooming
	states, and sets its current value to the camera's current antiblooming state.
	
***********************************************************************************/

void BuildAntibloomingMenu ( GWindowPtr dialog, CameraPtr camera )
{
	char			text[256];
	short			item, state;
	GControlPtr		control;
	
	if ( GetCameraAntiblooming ( camera, &state ) == FALSE )
		return;
		
	control = GGetControl ( dialog, CAMERA_EXPOSURE_ANTIBLOOMING_MENU );

	for ( item = 1; GetCameraAntibloomingInfo ( camera, item, text ) == TRUE; item++ )
		GSetPopupMenuItemText ( control, item, text );

	GSetControlValue ( control, state );
}

/*** UpdateCameraExposureDialog ************************************************

	Updates the camera exposure status parameters displayed in the
	"Exposure" panel of the "Camera Control" dialog.
	
	int UpdateCameraExposureDialog ( long interval )

	(interval): minimum interval between updates, in system clock ticks.
	
	The function returns TRUE if successful, and FALSE on failure.
	
	If the time elapsed since the previous call of this function is less
	than (interval), the function will do nothing.  You can use this feature
	to control the rate at which the exposure status display is actually
	updated.
	
*********************************************************************************/

int UpdateCameraExposureDialog ( long interval )
{
	short		item, minimum, maximum, value, row;
	short		status, filter, shutter;
	float		time;
	char		text[256], countString[32], *filterString;
    long		currentTick;
    static long	lastUpdateTick = 0;
	GWindowPtr	dialog = NULL;
	GControlPtr	control;
	CameraPtr	camera;
	ExposurePtr	exposure;
	
	/*** If the "exposure" panel of the "Camera control" dialog is not
	     currently being displayed, do nothing further. ***/
	     
	if ( GetCameraSubDialog ( &dialog ) != CAMERA_EXPOSURE_PAGE_DIALOG )
		return ( TRUE );

	/*** Obtain a pointer to the active camera. ***/
	
	camera = GetActiveCamera();
	if ( camera == NULL )
		return ( TRUE );
		
	/*** Obtain the current system clock tick count.  If the amount of time
	     elapsed since the last call of this function is less than the
	     specified interval, return without doing anything further. ***/
	     
	currentTick = GGetTickCount();
	if ( currentTick - lastUpdateTick < interval )
		return ( TRUE );

	/*** Obtain the camera's current exposure status, the time elapsed and/or
	     number of rows downloaded since the start of the current exposure. ***/
	     
	status = GetCameraStatus ( camera, &time, &row );
	exposure = GetCameraExposure ( camera );
	
	/*** If there is an exposure in progress, get the filter position and shutter
	     state for the exposure.  Otherwise, query the camera directly to get the
	     shutter state and filter position. ***/
	
	if ( status != CAMERA_STATUS_WAITING && exposure != NULL )
	{
		filter = GetExposureFilter ( exposure );
		shutter = GetExposureShutter ( exposure );
	}
	else
	{
		shutter = GetCameraExposureShutter ( camera );
		if ( GetCameraFilter ( camera, &filter ) == FALSE )
			filter = GetCameraExposureFilter ( camera );
	}
		
	/*** Now find the approprate text string which corresponds to the
	     filter position. ***/
	     
	if ( filter == FILTER_DARK || shutter == SHUTTER_CLOSED )
		filterString = "dark";
	else if ( filter == FILTER_RED )
		filterString = "red";
	else if ( filter == FILTER_GREEN )
		filterString = "green";
	else if ( filter == FILTER_BLUE )
		filterString = "blue";
	else
		filterString = "light";

	/*** If we are in multiple-exposure mode, make a string which indicates
	     the current exposure number out of the total number of exposures.
	     Otherwise make the count string empty. ***/
	     
	if ( GetCameraExposureMode ( camera ) == EXPOSURE_MODE_MULTIPLE_IMAGE )
	{
		short	number = GetCameraExposureNumber ( camera );
		short	total =  GetCameraExposureTotal ( camera );
		
		sprintf ( countString, "%hd of %hd ", number, total );
	}
	else
	{
		countString[0] = '\0';
	}
	
	/*** Obtain a handle to the exposure progress bar control, and obtain the
	     control's minimum and maximum values. ***/
	     
	control = GGetControl ( dialog, CAMERA_EXPOSURE_STATUS_BAR );
	minimum = GGetControlMin ( control );
	maximum = GGetControlMax ( control );
	
	/*** If the camera is waiting to take a new exposure, set the control's value
	     to its minimum, and display its status as "Waiting". ***/
	
	if ( status == CAMERA_STATUS_WAITING )
	{
		value = minimum;
		strcpy ( text, "Ready to expose." );
	}
	
	/*** If an exposure is in progress, set the control's value to indicate the
	     portion of the exposure which has been completed, and display the time
	     elapsed since the start of the exposure. ***/
	
	if ( status == CAMERA_STATUS_EXPOSING )
	{
		if ( time < 0.0 )
			time = 0.0;

		value = minimum + ( maximum - minimum ) * ( time / GetCameraExposureLength ( camera ) );
		sprintf ( text, "Exposing %s image %s(%.1f sec)...", filterString, countString, time );
	}
	
	/*** If the image is being downloaded, set the control's value to indicate the
	     percentage of rows which have been downloaded, and display the camera status
	     as "Downloading". ***/
	     
	if ( status == CAMERA_STATUS_DOWNLOADING )
	{
		long numRows = GetImageRows ( GetImageWindowImage ( GetCameraExposureWindow ( camera ) ) );
		value = minimum + ( maximum - minimum ) * row / numRows;
		sprintf ( text, "Downloading %s image %s(%.1f%%)...", filterString, countString, 100.0 * row / numRows );
	}
	
	/*** Now actually display the status text in the dialog, and update the progress
	     bar control value. ***/
	     
	GSetDialogItemText ( dialog, CAMERA_EXPOSURE_STATUS_TEXT, text );
	GSetControlValue ( control, value );

	/*** If desired, set the text of the "Start"/"Stop" button to indicate
	     whether the button will start a new exposure or stop the one currently
	     in progress. ***/
	     
	if ( interval == 0 )
	{
		if ( status == CAMERA_STATUS_WAITING )
		{
			GSetDialogItemText ( dialog, CAMERA_EXPOSURE_START_BUTTON, "Start" );
			for ( item = CAMERA_EXPOSURE_MODE_TEXT; item <= CAMERA_EXPOSURE_LENGTH_TEXT; item++ )
				GEnableDialogItem ( dialog, item, TRUE );
		}
		else
		{
			GSetDialogItemText ( dialog, CAMERA_EXPOSURE_START_BUTTON, "Stop" );
			for ( item = CAMERA_EXPOSURE_MODE_TEXT; item <= CAMERA_EXPOSURE_LENGTH_TEXT; item++ )
				GEnableDialogItem ( dialog, item, FALSE );
		}
	}	
	
	lastUpdateTick = currentTick;
	return ( TRUE );
}

/*** MoveCameraFilter *************************************************************

	Changes the position of the camera's filter wheel, and displays the operation
	in the exposure status text field of the "exposure" page of the "camera control"
	dialog if it is visible.
	
	short MoveCameraFilter ( CameraPtr camera, short filter )

	(camera): pointer to camera whose filter you want to move.
	(filter): desired filter position.
	
	The function returns TRUE if it is able to move the filter wheel into the
	desired position, and FALSE otherwise.  The filter position should be one
	of the filter position constants specified in "CameraInterface.h", e.g.
	FILTER_NONE, FILTER_DARK, FILTER_RED, FILTER_GREEN, FILTER_BLUE, etc.
		
***********************************************************************************/

short MoveFilter ( CameraPtr camera, short filter )
{
	short		result;
	char		text[256], oldText[256];
	GWindowPtr	dialog = NULL;
	
	/*** If the "Exposure" page of the "Camera Control" dialog is visible,
	     get a copy of the text currently displayed in the status field.
	     Then change the text to indicate which filter we are moving to. ***/
	      
	if ( GetCameraSubDialog ( &dialog ) == CAMERA_EXPOSURE_PAGE_DIALOG && camera == GetActiveCamera() )
	{
		GGetDialogItemText ( dialog, CAMERA_EXPOSURE_STATUS_TEXT, oldText );

		if ( filter == FILTER_DARK )
			sprintf ( text, "Moving to dark filter..." );
		else if ( filter == FILTER_RED )
			sprintf ( text, "Moving to red filter..." );
		else if ( filter == FILTER_GREEN )
			sprintf ( text, "Moving to green filter..." );
		else if ( filter == FILTER_BLUE )
			sprintf ( text, "Moving to blue filter..." );
		else
			sprintf ( text, "Moving to clear filter..." );

		GSetDialogItemText ( dialog, CAMERA_EXPOSURE_STATUS_TEXT, text );
	}
	
	/*** Now change the filter, and wait until the filter has finished
	     moving into position. ***/
	
	GSetWaitCursor();
	result = SetCameraFilter ( camera, filter, TRUE );
	GSetArrowCursor();
	
	/*** When the filter is done moving, restore the status text to what
	     it was before we moved the filter.  If we successfully moved the filter,
	     display the new filter position in the dialog's "Filter" menu. ***/
	     
	if ( GetCameraSubDialog ( &dialog ) == CAMERA_EXPOSURE_PAGE_DIALOG && camera == GetActiveCamera() )
	{
		GSetDialogItemText ( dialog, CAMERA_EXPOSURE_STATUS_TEXT, oldText );
		
		if ( result == TRUE )
			GSetDialogControlValue ( dialog, CAMERA_EXPOSURE_FILTER_MENU, filter );
	}
	
	return ( result );
}

/*** DeleteCameraExposureList ***/

void DeleteCameraExposureList ( CameraPtr camera )
{
	DeleteExposureList ( GetCameraExposureList ( camera ) );
	SetCameraExposureList ( camera, NULL );
	SetCameraExposure ( camera, NULL );
}

/*** NewCameraExposureList ***/

ExposureList NewCameraExposureList ( CameraPtr camera )
{
	short				iExposureMode = GetCameraExposureMode ( camera );
	short				iDarkMode = GetCameraDarkFrameMode ( camera );
	short				iBinning, i;
	short				iNumExposures = GetCameraExposureTotal ( camera );
	short				iShutter = GetCameraExposureShutter ( camera );
	short				iFilter = GetCameraExposureFilter ( camera );
	float				fLength = GetCameraExposureLength ( camera );
	ExposureList		pExposureList = NULL;
	
	/*** Create an exposure sequence for the camera, based on the
	     camera's current dark frame mode, binning, shutter, filter
	     mode, exposure length. ***/
	
	GetCameraReadoutMode ( camera, &iBinning );	     

	if ( iExposureMode == EXPOSURE_MODE_FOCUS || iExposureMode == EXPOSURE_MODE_IMAGE || iExposureMode == EXPOSURE_MODE_MULTIPLE_IMAGE || iExposureMode == EXPOSURE_MODE_COMBINED_IMAGE )
	{
		if ( iExposureMode == EXPOSURE_MODE_MULTIPLE_IMAGE || iExposureMode == EXPOSURE_MODE_COMBINED_IMAGE )
			iNumExposures = GetCameraExposureTotal ( camera );
		else
			iNumExposures = 1;
			
		if ( iDarkMode != DARK_FRAME_ONLY )
			for ( i = 0; i < iNumExposures; i++ )
				NewExposure ( &pExposureList, iBinning, SHUTTER_OPEN, iFilter, fLength );
		
		if ( iDarkMode == DARK_FRAME_ALWAYS || iDarkMode == DARK_FRAME_ONLY )
			for ( i = 0; i < iNumExposures; i++ )
				NewExposure ( &pExposureList, iBinning, SHUTTER_CLOSED, FILTER_DARK, fLength );
	}
	else if ( iExposureMode == EXPOSURE_MODE_COLOR_IMAGE )
	{
		if ( iDarkMode == DARK_FRAME_ALWAYS || iDarkMode == DARK_FRAME_ONLY )
			NewExposure ( &pExposureList, iBinning, SHUTTER_CLOSED, FILTER_DARK, fLength );

		if ( iDarkMode != DARK_FRAME_ONLY )
		{
			NewExposure ( &pExposureList, iBinning, SHUTTER_OPEN, FILTER_RED, fLength );
			NewExposure ( &pExposureList, iBinning, SHUTTER_OPEN, FILTER_GREEN, fLength );
			NewExposure ( &pExposureList, iBinning, SHUTTER_OPEN, FILTER_BLUE, fLength );
		}
	}
	
	/*** If the camera already has an exposure list, delete it. ***/
	
	if ( GetCameraExposureList ( camera ) != NULL )
		DeleteCameraExposureList ( camera );
	
	/*** Attach the new exposure list to the camera, and set the
	     camera's current exposure to the first record in the list.
	     Then return a pointer to the newly-created exposure list. ***/
	     
	SetCameraExposureList ( camera, pExposureList );
	SetCameraExposure ( camera, pExposureList );
	
	return ( pExposureList );		
}

/*** StartExposure ***********************************************************

	Starts a new exposure; or, if the camera's exposure list is empty,
	starts a new sequence of exposures.

    void StartExposure ( camera )

	(camera): pointer to camera you wish to start exposing.
	
******************************************************************************/

void StartExposure ( CameraPtr camera )
{
	int				result = TRUE;
	short			width = 320;
    short			height = 240;
    short			type, mode;
    FITSImagePtr	dark = NULL;
    ImagePtr		image = NULL;
	GWindowPtr		window = NULL;
	ExposurePtr		exposure = NULL;
	
    /*** Determine the camera's image size.  On failure, display an error
         message and return an error code. ***/
	
	result = GetCameraReadoutMode ( camera, &mode );
	if ( result == TRUE )
		result = GetCameraReadoutModeInfo ( camera, mode, &width, &height, NULL );
		
    if ( result == FALSE )
    {
    	GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't determine camera's image size!" );
        return;
    }

	/*** If camera already has an exposure window, make sure its image size
	     matches the camera's image size.  Also make sure that if the camera
	     is exposing in color, the image is a color image, and vice-versa.
	     If these things do not match, delete the window; this will force
	     a new camera exposure window to be created, below. ***/
	     
	window = GetCameraExposureWindow ( camera );
	if ( window != NULL )
	{
		image = GetImageWindowImage ( window );
		if ( GetImageColumns ( image ) != width || GetImageRows ( image ) != height )
		{
			DeleteImageWindow ( window );
		}
		
		mode = GetCameraExposureMode ( camera );
		type = GetImageType ( image );
		
		if ( mode == EXPOSURE_MODE_COLOR_IMAGE )
		{
			if ( type != IMAGE_TYPE_RGB_COLOR )
				DeleteImageWindow ( window );
		}
		else	
		{
			if ( type == IMAGE_TYPE_RGB_COLOR )
				DeleteImageWindow ( window );
		}
	}
	
	/*** Allocate a buffer for the dark frame if we don't have one already;
	     or reallocate it if the image size has changed. ***/
	
	dark = GetCameraDarkFrame ( camera );
	if ( dark == NULL )
		dark = NewCameraDarkFrame ( camera, width, height );
	else
		if ( dark->naxis1 != width || dark->naxis2 != height )
			dark = NewCameraDarkFrame ( camera, width, height );
	
	/*** If the camera does not have a window in which the image will be displayed,
	     create one now.  Otherwise, simply obtain a pointer to its current exposure
	     results image window. ***/
	
	window = GetCameraExposureWindow ( camera );
	if ( window == NULL )    
	{
	    /*** Allocate memory for the window's image.  On failure,
	         display an error message and return an error code. ***/
	    
	    if ( GetCameraExposureMode ( camera ) == EXPOSURE_MODE_COLOR_IMAGE )     
			image = NewImage ( "New Exposure", IMAGE_TYPE_RGB_COLOR, 3, height, width );
		else
			image = NewImage ( "New Exposure", IMAGE_TYPE_MONOCHROME, 1, height, width );
			
	    if ( image == NULL )
	    {
	        GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't allocate memory for new image!" );
			return;
	    }

	    /*** Allocate memory for the image window itself.  On failure,
	         display an error message and return an error code. ***/

	    window = NewImageWindow ( image );
	    if ( window == NULL )
	    {
	        GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't allocate memory for new image window!" );
	    	FreeFITSImage ( image );
	        return;
	    }
	    
	    /*** Designate the window we just created as the one which should
	         receive output from the camera. ***/
	         
	    SetCameraExposureWindow ( camera, window );
	}
	
	/*** If the camera doesn't have current exposure record, create a new
	     exposure list now. ***/
	
	exposure = GetCameraExposure ( camera );
	if ( exposure == NULL )
	{
		if ( NewCameraExposureList ( camera ) == NULL )
		{
			GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't allocate memory for new exposure list!" );
			DeleteImageWindow ( window );
			return;
		}

		exposure = GetCameraExposure ( camera );
	}
	
	/*** If the camera has no shutter, and the desired shutter state for the exposure
	     differs from the camera's current shutter state, then display a message to
	     the user to place or remove an obstruction in front of the camera. ***/
	
	if ( ! CameraHasShutter ( camera ) )
	{
		if ( GetExposureShutter ( exposure ) != GetCameraExposureShutter ( camera ) )
		{
			if ( GetExposureShutter ( exposure ) == SHUTTER_CLOSED )
			{
				GDoAlert ( G_INFO_ALERT, G_OK_ALERT, GString ( CLOSE_SHUTTER_STRING ) );
			}
			else
			{
				GDoAlert ( G_INFO_ALERT, G_OK_ALERT, GString ( OPEN_SHUTTER_STRING ) );
			}
		}
	}
	
	/*** Get a pointer to the camera's current exposure record, and set the
	     camera's binning/shutter/filter/exposure length accordingly. ***/
	     
	MoveFilter ( camera, GetExposureFilter ( exposure ) );
	SetCameraExposureLength ( camera, GetExposureLength ( exposure ) );
	SetCameraExposureShutter ( camera, GetExposureShutter ( exposure ) );
	SetCameraReadoutMode ( camera, GetExposureBinning ( exposure ) );
	
	/*** Now increment the camera's exposure number. ***/
	
	SetCameraExposureNumber ( camera, GetCameraExposureNumber ( camera ) + 1 );
	
	/*** Now start the exposure.  If the exposure length is less than 0.1 seconds,
	     wait for the exposure to finish here, then end the exposure and download
	     the image.  Otherwise, we'll check to see whether the exposure is finished
	     as part of our main event-loop null-event processing.  This ensures that short
	     exposures will be accurate, since no background processing will happen while
	     we are waiting for them to finish; and that the user will be able to interact
	     with the machine while long exposures are taking place. ***/
	     
	if ( GetCameraExposureLength ( camera ) < 0.1 )
	{
		result = StartCameraExposure ( camera, TRUE );
		CompleteExposure ( camera );
	}
	else
	{
		result = StartCameraExposure ( camera, FALSE );
		UpdateCameraExposureDialog ( 0 );
	}
	
	/*** If the exposure could not be started successfully, cancel the remaining
	     exposures in the exposure list, and display a warning message. ***/
	
	if ( result != TRUE )
	{
		CancelExposure ( camera );
		GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't start the exposure!" );
	}
}

/*** CompleteExposure *************************************************************

	Completes a camera's exposure, downloads the image, and displays the results
	in the camera exposure window.

 	void CompleteExposure ( CameraPtr camera )
	
	(camera): pointer to camera record.
	                                         
**********************************************************************************/

void CompleteExposure ( CameraPtr camera )
{
	short			filter;
	int				result;
	GWindowPtr		window = GetCameraExposureWindow ( camera );
   	ImagePtr		image;
    ImageRegionPtr	region;
	ExposurePtr		exposure;
	
	/*** First tell the camera to end the exposure and set the camera
	     status code to indicate that we are about to download the image. ***/

	StartCameraDownload ( camera );
	UpdateCameraExposureDialog ( 0 );
	
	window = GetCameraExposureWindow ( camera );
    image = GetImageWindowImage ( window );
    region = GetImageWindowSelectedRegion ( window );

	/*** If we are in "Image" mode, download the entire image from the camera;
	     otherwise download only the selected region of the image.  When
	     finished, set the exposure status to indicate that we are ready
	     to take a new exposure. ***/
	
	if ( GetCameraExposureMode ( camera ) == EXPOSURE_MODE_IMAGE )
		result = DownloadImage ( camera, image, NULL );
	else
		result = DownloadImage ( camera, image, region );
	
	/*** If the exposure results window is still present, redraw and display
	     the image.  (If the user cancelled partway through a download, we
	     should redraw the portion of the image that was downloaded if the
	     exposure window is visible.  If the user cancelled the download
	     before the window has yet been made visible, it would have been
	     deleted, so we should not redraw or redisplay anything.) ***/
	     
	window = GetCameraExposureWindow ( camera );
	if ( window != NULL )
	{
		/*** Update all displays of image data since the image data has changed. ***/
		
		UpdateImage ( image );

		/*** Get the camera's color filter wheel position; if successful,
		     set the color table in which the image should be displayed
		     accordingly. ***/
		     
		if ( GetCameraFilter ( camera, &filter ) == TRUE && sDisplayFilterColorTable )
		{
			if ( filter == FILTER_RED )
				SetImageWindowColorTable ( window, COLOR_TABLE_RED );
			else if ( filter == FILTER_GREEN )
				SetImageWindowColorTable ( window, COLOR_TABLE_GREEN );
			else if ( filter == FILTER_BLUE )
				SetImageWindowColorTable ( window, COLOR_TABLE_BLUE );
			else
				SetImageWindowColorTable ( window, COLOR_TABLE_GRAYSCALE );
		}
		
		/*** If the exposure image window is not yet visible, activate it. ***/
             
    	if ( ! GIsWindowVisible ( window ) )
		{
        	CropImageWindow ( window );
        	GSetActiveWindow ( window );
        }

		/*** If the user allowed the image to download completely, the camera's
		     status code will still be "downloading".  If the user cancelled the
		     download, the status code will be "waiting", and we don't need to
		     do anything mode. ***/

		if ( GetCameraStatus ( camera, NULL, NULL ) == CAMERA_STATUS_DOWNLOADING )
		{
			/*** First: end the download and return the camera to its waiting state. ***/
			
			EndCameraDownload ( camera );
			
			/*** if we failed to download the whole image, we'll delete the camera's
			     exposure list, and not attempt to take any more exposures. ***/ 
			
			if ( result == FALSE ) 
			{
				DeleteCameraExposureList ( camera );
				UpdateCameraExposureDialog ( 0 );
			}
			else
			{
				/*** If the image was downloaded successfully, set the current camera
				     exposure to the next exposure in the camera's exposure list. ***/
				
				exposure = GetCameraExposure ( camera );
				exposure = GetNextExposure ( exposure );
				SetCameraExposure ( camera, exposure );
				
				/*** If we're in focus mode, and there are no more exposures
				     in the camera's exposure list, create a new exposure list
				     for the camera. ***/
				     
				if ( GetCameraExposureMode ( camera ) == EXPOSURE_MODE_FOCUS && exposure == NULL )
					exposure = NewCameraExposureList ( camera );

				/*** If we're taking multiple exposures, save the exposure results
				     to a file with the current exposure name, path, and file format. ***/
				
				if ( GetCameraExposureMode ( camera ) == EXPOSURE_MODE_MULTIPLE_IMAGE )
				{
					char		name[256];
					GPathPtr	path = GetCameraExposurePath ( camera );
					
					sprintf ( name, GetCameraExposureName ( camera ), (int) GetCameraExposureNumber ( camera ) );
					
					GSetPathName ( path, name );
					SetImageFileFormat ( image, GetCameraExposureFormat ( camera ) );
					SaveImageWindow ( window, path );
				}
				
				/*** If there are no more exposures in the camera's exposure list, delete the
				     exposure list, move the filter back to its default position, and update the
				     camera status display in the "camera control" dialog.  (We don't need to move
				     the filter or update the display if there are exposures remaining because the
				     filter may be moved and/or the display updated when the next exposure begins.) ***/
				
				if ( exposure == NULL )
				{
					SetCameraExposureNumber ( camera, 0 );
					DeleteCameraExposureList ( camera );
					MoveFilter ( camera, GetCameraExposureFilter ( camera ) );
					UpdateCameraExposureDialog ( 0 );
				}
			}
		}
	}
}

/*** CancelExposure **************************************************************

	Cancels an exposure or download in progress.

 	void CancelExposure ( CameraPtr camera )
	
	(camera): camera whose exposure you wish to cancel.
	
	There are two possible ways this function can be called:
	
	- while the camera is exposing (to cancel and discard the exposure);
	
	- while the image is being downloaded from the camera (to abort the
	  download in progress).  In the latter case, this function actually
	  gets called recursively from within the function which downloads the
	  image from the camera, GetCameraImage().

	This function will also delete the camera's entire list, thereby cancelling
	the entire sequence of exposures described in the list.
		                                               
**********************************************************************************/

void CancelExposure ( CameraPtr camera )
{
	GWindowPtr	window;
	
	/*** Return the camera to its idle state, delete the camera's exposure list,
	     move the camera's filter back to its default position,
	     and update the exposure status display in the "camera control" dialog. ***/
	
	if ( GetCameraStatus ( camera, NULL, NULL ) == CAMERA_STATUS_EXPOSING )     
		EndCameraExposure ( camera );
	
	if ( GetCameraStatus ( camera, NULL, NULL ) == CAMERA_STATUS_DOWNLOADING )
		EndCameraDownload ( camera );
		
	MoveFilter ( camera, GetCameraExposureFilter ( camera ) );
	DeleteCameraExposureList ( camera );
	UpdateCameraExposureDialog ( 0 );
	SetCameraExposureNumber ( camera, 0 );
	
	/*** If the camer's exposure window is not yet visible, delete it too:
	     there's no point in keeping a hidden image window lying around!
	     (This situation can happen before the first exposure or download
	     in a sequence has been made visible.) ***/
	     
	window = GetCameraExposureWindow ( camera );
	if ( ! GIsWindowVisible ( window ) )
		DeleteImageWindow ( window );
}

/*** DownloadImage **********************************************************

	Reads out either a full frame or a partial region of image data from the
    camera.

	int DownloadImage ( CameraPtr camera, ImagePtr image, ImageRegionPtr region )

	(camera): pointer to camera record.
    (image):  pointer to image structure to receive image data.
    (region): pointer to image region structure specifying portion of
              image data to read; pass NULL to read the whole image.

    The function returns TRUE if able to read the image data successfully,
    or FALSE on failure.

	On a successful return, the image data will be stored into the image
    data matrix of the specified image record (image).  If the image
    region pointer (region) specifies a valid image region structure, then
    only data from that portion of the image will be read into the image
    data matrix; otherwise the entire image frame will be read.

    Please note that this function does no error-checking on the positions
    and sizes of either the image data matrix or the image region
    structure.  You must make sure that the image data matrix is large enough
    to hold all of the pixels specified in the image region (if the region
    pointer is valid) or in the entire image frame (if the region pointer is
    NULL).

******************************************************************************/

int DownloadImage ( CameraPtr camera, ImagePtr image, ImageRegionPtr region )
{
	int		result = TRUE, deleteRegion = FALSE;
	short	left, top, right, bottom, width, height, mode, filter, row, col;
	PIXEL	*data, *data1, *data2, *dark, *buffer;
	
	/*** Determine the dimensions of the image we will download. ***/
	
    width = GetImageColumns ( image );
    height = GetImageRows ( image );

	/*** Get the camera's exposure mode and filter position.  If the shutter
	     was closed during the exposure, treat the exposure as a dark-filter
	     exposure. ***/
	     
	mode = GetCameraExposureMode ( camera );
	
	if ( GetCameraFilter ( camera, &filter ) == FALSE )
		filter = FILTER_CLEAR;

	if ( GetCameraExposureShutter ( camera ) == SHUTTER_CLOSED )
		filter = FILTER_DARK;

    /*** If we have not already been given region describing what portion
         of the image to download from the camera, create a new region
         containing the entire image. ***/
         
    if ( region == NULL )
    {
    	region = NewImageRegion ( IMAGE_REGION_TYPE_RECTANGULAR, 0, 0,
                 width - 1, height - 1 );

        if ( region == NULL )
        	return ( FALSE );
        else
            deleteRegion = TRUE;
    }

    /*** If the top of the region is below the top of the image, skip image
         data rows until we come to the region's top row.  If we fail at
         any point, delete the image region (if necessary) and return an
         error code.  If the user cancels the download, return a successful
         result code. ***/

    top = GetImageRegionTop ( region );
	if ( top > 0 )
	{
    	if ( DiscardCameraImageRows ( camera, top ) == FALSE )
        {
        	if ( deleteRegion )
            	DeleteImageRegion ( region );
        	GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't download image from camera!" );
        	return ( FALSE );
        }
	}

	/*** Allocate a temporary buffer big enough to hold one row of pixels,
	     into which data will be copied from the camera driver.  If we can't,
	     display a memory allocation warning and return an error code. ***/

	buffer = (PIXEL *) malloc ( sizeof ( PIXEL ) * width );
	if ( buffer == NULL )
	{
		if ( deleteRegion )
			DeleteImageRegion ( region );
        GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, GString ( CANT_ALLOCATE_MEMORY_STRING ) );
		return ( FALSE );
	}
	
    /*** Now read the rows of the inage which lie within the specified
         region.  If we fail at any point, delete the region (if necessary)
         and return an error code.  If the user cancels the download,
         return a successful result code. ***/

    bottom = GetImageRegionBottom ( region );
    for ( row = -1; GetImageRegionSegment ( region, &row, &left, &right ); row = row )
    {
		/*** Obtain a pointer to the image row which should receive the
		     data downloaded from the camera. ***/
		
		if ( mode == EXPOSURE_MODE_COLOR_IMAGE )
		{
			/*** If we're exposing a color image, make sure we place the image data
			     into the proper frame of the image, depending on which filter the
			     image has been taken with. ***/
		     
			if ( filter == FILTER_RED )
				data = GetImageDataRow ( image, 0, row );
			else if ( filter == FILTER_GREEN )
				data = GetImageDataRow ( image, 1, row );
			else if ( filter == FILTER_BLUE )
				data = GetImageDataRow ( image, 2, row );
			else
				data = GetImageDataRow ( image, 0, row );
		}
		else
		{
			/*** For monochrome images, we always insert the data into the first
			     image frame. ***/
			     
			data = GetImageDataRow ( image, 0, row );
		}
		
		/*** Obtain a pointer to the corresponding row in the camera's dark-frame
		     buffer. ***/
		
		dark = GetCameraDarkFrame ( camera )->data[0][row];

		/*** Download the image data into the temporary buffer.  If we failed to
		     download the image data row, display a warning and return an error code. ***/
		
		result = DownloadCameraImageRow ( camera, left, right, buffer );
		if ( result == FALSE )
        {
            free ( buffer );
            if ( deleteRegion )
            	DeleteImageRegion ( region );
        	GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't download image from camera!" );
        	return ( FALSE );
    	}

		/*** What we do next with the data we just downloaded depends on whether we
		     are taking a dark or light image, and on the camera's current exposure
		     and dark-frame behavior. ***/
		     
		if ( GetCameraExposureShutter ( camera ) == SHUTTER_CLOSED )
		{
			/*** If we're taking dark frames only, and we're past the first exposure
			     of an image in combined-image mode, add the data in the buffer to the
			     data in the dark image.  Otherwise, just copy the data from the buffer
			     into the dark image. ***/
			     
			if ( GetCameraDarkFrameMode ( camera ) == DARK_FRAME_ONLY )
			{
				if ( GetCameraExposureMode ( camera ) == EXPOSURE_MODE_COMBINED_IMAGE && GetCameraExposureNumber ( camera ) > 1 )
				{
					for ( col = left; col <= right; col++ )
						data[col] = dark[col] += buffer[col];
				}
				else
				{
					for ( col = left; col <= right; col++ )
						data[col] = dark[col] = buffer[col];
				}
			}
			else
			{
				/*** Otherwise, we're subtracting dark frames from light image
				     frames, so copy the camera data into the dark image and then
				     subtract it from light image. ***/
				     
				for ( col = left; col <= right; col++ )
					data[col] -= dark[col] = buffer[col];
			}
		}
		else
		{
			/*** If we're taking a light exposure, and we're past the first exposure
			     of an image in combined-image mode, add the data in the buffer to the
			     data in the image.  Otherwise, just copy the data from the buffer
			     into the dark image. ***/
			
			if ( GetCameraExposureMode ( camera ) == EXPOSURE_MODE_COMBINED_IMAGE && GetCameraExposureNumber ( camera ) > 1 )
			{
				for ( col = left; col <= right; col++ )
					data[col] += buffer[col];
			}
			else
			{
				for ( col = left; col <= right; col++ )
					data[col] = buffer[col];
			}
		}
		
		/*** If we are exposing a color image, but the filter is neither
		     red, green, nor blue, then copy the image data into the green
		     and blue image frames, so that the image will appear monochrome.  WHY IS THIS HERE? ***/
		     
    	if ( mode == EXPOSURE_MODE_COLOR_IMAGE && ( filter < FILTER_RED || filter > FILTER_BLUE ) )
    	{
    		data1 = GetImageDataRow ( image, 1, row );
    		data2 = GetImageDataRow ( image, 2, row );
    		
    		for ( col = left; col < right; col++ )
    			data1[col] = data2[col] = data[col];
    	}
    	
    	/*** Allow background event handling ten times per second.  If the user cancels
    	     the download at this point, break out of the download loop. ***/
    	     
    	if ( DoDownloadEvent ( camera, G_TICKS_PER_SECOND / 10, row, bottom, filter ) == FALSE )
    		break;
	}

    /*** Free the temporary download buffer, delete the image region (if necessary),
         and return a successful result code. ***/
    
    free ( buffer );
       
    if ( deleteRegion )
    	DeleteImageRegion ( region );
    
	return ( TRUE );
}

/*** DoDownloadEvent ****************************************************************

	Handles user-interface events that take place while an image is being downloaded
	from the camera.
	
	int DoDownloadEvent ( CameraPtr camera, long interval, short row, short numRows,
	    short filter )

	(camera):   pointer to camera record.
	(interval): minimum interval between events in system clock ticks.
	(row):      number of row currently being downloaded from the camera.
	(numRows):  total number of rows which need to be downloaded.
	(filter):   identifies the filter with which the image was taken.
	
	The function returns TRUE if the image should continue to be downloaded from
	the camera, or FALSE if the user cancelled the download.
	
	The function updates the value of the progress bar control in the "Exposure"
	pane of the "Camera Control" dialog to indicate the proportion of the
	image rows which have been downloaded.
	
	If the time elapsed since the previous call of this function is less than
	(interval) system clock ticks, the function will return immediately, without
	handling any user-interface events or updating the progress bar control.
	
*************************************************************************************/

int DoDownloadEvent ( CameraPtr camera, long interval, short row, short numRows, short filter )
{
	static long		lastEventTick = 0;
	GEvent			event;
	GWindowPtr		dialog;
	
	/*** Determine the amount of time elapsed since the last time a user-interface
	     event was handled.  If it is more than the specified interval, update the
	     exposure progress bar control to indicate the portion of the image which
	     has been downloaded, and then process pending UI events. ***/
	      
	if ( ( GGetTickCount() - lastEventTick ) > interval )
	{
		if ( GetCameraSubDialog ( &dialog ) == CAMERA_EXPOSURE_PAGE_DIALOG )
			UpdateCameraExposureDialog ( interval );

        /*** Now retrieve and process UI events until there are no more
             events to be processed, or until we've been retrieving/processing
             them for more than the specified time interval. ***/
             
		lastEventTick = GGetTickCount();

		while ( TRUE )
		{
			GGetNextEvent ( &event );
			GProcessEvent ( &event );

            /*** If there are no more events waiting to be processed,
                 we can stop now. ***/

            if ( GEventPending() == FALSE )
            	break;

			/*** If we've been processing events for more than a tenth of
                 a second, we should take a break and let the application
                 go back to whatever else it was doing; we'll get to the
                 rest of the events next time around.  The Windows version
                 of the program occasionally locks up if this code is missing;
                 for some reason, Windows sometimes thinks the app always has
                 messages pending. ***/

            if ( GGetTickCount() - lastEventTick > interval )
            	break;
		}

		lastEventTick = GGetTickCount();
	}

	/*** If the user clicked the "Stop" button, the resulting call to StopCameraExposure()
	     will have changed the exposure status code from "downloading" to "waiting".
	     This is our signal that the download should be aborted prematurely.  If this
	     happens, return FALSE; otherwise, return TRUE. ***/

	if ( GetCameraStatus ( camera, NULL, NULL ) == CAMERA_STATUS_DOWNLOADING )
		return ( TRUE );
	else
		return ( FALSE );
}

/*** DarkFrameImageDataRow ***/

void DarkFrameImageDataRow ( CameraPtr camera, short row, short left, short right, PIXEL *imageRow )
{
	short 	mode, col;
	PIXEL	*darkRow = NULL;
	
	/*** Determine the camera's current dark frame mode, and get a pointer
	     to the row in the dark frame which corresponds to the row in the
	     image frame which we are currently processing. ***/
	     
	mode = GetCameraDarkFrameMode ( camera );
	darkRow = GetCameraDarkFrame ( camera )->data[0][row];
	
	/*** If we are taking a dark frame, what we do next depends on whether
	     we're taking dark frames only, or also subtracting them from the light
	     image.  If we're taking taking dark frames only, copy the data from
	     the dark frame buffer into the image buffer; otherwise, we should subtract
	     the data in the dark frame buffer from the data in the image buffer. ***/
	     
	if ( GetCameraExposureShutter ( camera ) == SHUTTER_CLOSED )
	{
		if ( mode == DARK_FRAME_ONLY )
		{
			for ( col = left; col <= right; col++ )
				imageRow[col] = darkRow[col];
		}
		else
		{
			for ( col = left; col <= right; col++ )
				imageRow[col] -= darkRow[col];
		}
	}
}