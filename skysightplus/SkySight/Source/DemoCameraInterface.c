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

#include "GUILib.h"
#include "AstroLib.h"
#include "CameraInterface.h"

/*** Local functions ***/

static short	GetCameraShutterInfo ( CameraInterfaceDataPtr );
static short	GetCameraFilterInfo ( CameraInterfaceDataPtr );
static short	GetCameraTECInfo ( CameraInterfaceDataPtr );
static short	GetCameraReadoutInfo ( CameraInterfaceDataPtr );
static short	GetCameraAntibloomingInfo ( CameraInterfaceDataPtr );

static short	OpenCameraConnection ( CameraInterfaceDataPtr );
static short	CloseCameraConnection ( CameraInterfaceDataPtr );
static short	SetCameraTemperature ( CameraInterfaceDataPtr );
static short	GetCameraTemperature ( CameraInterfaceDataPtr );
static short	SetCameraFilter ( CameraInterfaceDataPtr );
static short	GetCameraFilter ( CameraInterfaceDataPtr );
static short	StartCameraExposure ( CameraInterfaceDataPtr );
static short	StartCameraDownload ( CameraInterfaceDataPtr );
static short	EndCameraExposure ( CameraInterfaceDataPtr );
static short	EndCameraDownload ( CameraInterfaceDataPtr );
static short	DownloadCameraRow ( CameraInterfaceDataPtr );
static short	DiscardCameraRows ( CameraInterfaceDataPtr );

/*** local data ***/

static short	sRow = 0;

/*** DemoCameraInterface *************************************************

	This procedure is the entry point for the camera interface.
	
	short DemoCameraInterface ( short function, CameraInterfaceDataPtr data )

	(function): specifies which function the procedure should perform.
	(data):     on input, contains data needed by the procedure to perform
	            the specified function; on output, receives data generated
	            by the procedure.
	        
	The camera interface procedure should return an integer error code
	indicating whether or not it could complete the specified function
	successfully.  At present the only two return codes used are TRUE
	(success) and FALSE (failure).
	
**************************************************************************/

short DemoCameraInterface ( short function, CameraInterfaceDataPtr data )
{
	short	result = FALSE;
	
	switch ( function )
	{
		case CAMERA_GET_SHUTTER_INFO:
			result = GetCameraShutterInfo ( data );
			break;
		
		case CAMERA_GET_FILTER_INFO:
			result = GetCameraFilterInfo ( data );
			break;
			
		case CAMERA_GET_TEC_INFO:
			result = GetCameraTECInfo ( data );
			break;
			
		case CAMERA_GET_READOUT_INFO:
			result = GetCameraReadoutInfo ( data );
			break;

		case CAMERA_GET_ANTIBLOOMING_INFO:
			result = GetCameraAntibloomingInfo ( data );
			break;

		case CAMERA_OPEN_CONNECTION:
			result = OpenCameraConnection ( data );
			break;
			
		case CAMERA_CLOSE_CONNECTION:
			result = CloseCameraConnection ( data );
			break;
			
		case CAMERA_SET_TEMPERATURE:
			result = SetCameraTemperature ( data );
			break;

		case CAMERA_GET_TEMPERATURE:
			result = GetCameraTemperature ( data );
			break;

		case CAMERA_SET_FILTER:
			result = SetCameraFilter ( data );
			break;
			
		case CAMERA_GET_FILTER:
			result = GetCameraFilter ( data );
			break;
			
		case CAMERA_START_EXPOSURE:
			result = StartCameraExposure ( data );
			break;
			
		case CAMERA_END_EXPOSURE:
			result = EndCameraExposure ( data );
			break;
			
		case CAMERA_START_DOWNLOAD:
			result = StartCameraDownload ( data );
			break;
			
		case CAMERA_END_DOWNLOAD:
			result = EndCameraDownload ( data );
			break;
			
		case CAMERA_DOWNLOAD_ROW:
			result = DownloadCameraRow ( data );
			break;
			
		case CAMERA_DISCARD_ROWS:
			result = DiscardCameraRows ( data );
			break;
	}
	
	return ( result );
}

/*** GetCameraShutterInfo ***************************************************

	Determines the camera's shutter type and capabilities.

	short GetCameraShutterInfo ( CameraInterfaceDataPtr data )
	
	(data): pointer to camera interface data record.

	The camera's shutter type should be returned in the cameraShutterType
	member of the camera interface data structure (data).  It should be
	one of the following shutter type codes #defined in "CameraInterface.h":
	
	SHUTTER_NONE - camera has no shutter, or an unknown shutter type;
	SHUTTER_PHYSICAL - camera has a real, physical shutter;
	SHUTTER_ELECTRONIC - camera has an electronic shutter.
	
	The function should always return TRUE.
	
*****************************************************************************/

short GetCameraShutterInfo ( CameraInterfaceDataPtr data )
{
	short result = TRUE;

	data->cameraShutterType = SHUTTER_TYPE_NONE;

	return ( result );
}

/*** GetCameraFilterInfo ***************************************************

	Obtains information about a particular filter in the camera's filter
	wheel.

	short GetCameraFilterInfo ( CameraInterfaceDataPtr data )
	
	(data): pointer to camera interface data record.

	On input, the cameraFilterPosition member of the camera interface
	data structure (data) indicates which filter information is being
	requested for.  This value will always be greater than zero.
	
	The function should return TRUE if the requested filter position is
	valid; it should return FALSE if not (i.e., if the requested filter
	position is less than 1 or greater than the number of filters in the
	filter wheel.)  The function should always return FALSE if the
	camera has no filter wheel.
	
	If the requested filter position is valid, the function should return
	the corresponding filter color code in the cameraFilterColor member
	of the camera interface data structure (data).  If the filter color
	is unknown, or has no equivalent definition in "CameraInterface.h",
	then the function should return FILTER_COLOR_UNKNOWN in this member.

	Additionally, the function should return a textual description of
	the filter's color in the cameraDescription buffer of the camera
	interface data structure (data).  This description must be an ASCII
	NUL terminated string, no more than 255 characters in length.  This
	is the filter description which will appear in the filter menu which
	is presented to the user.

****************************************************************************/

short GetCameraFilterInfo ( CameraInterfaceDataPtr data )
{
	short result = TRUE;
	
	if ( data->cameraFilterPosition == 1 )
	{
		data->cameraFilterColor = FILTER_CLEAR;
		strcpy ( data->cameraDescription, "Clear" );
	}	
	else if ( data->cameraFilterPosition == 2 )
	{
		data->cameraFilterColor = FILTER_DARK;
		strcpy ( data->cameraDescription, "Dark" );
	}
	else if ( data->cameraFilterPosition == 3 )
	{
		data->cameraFilterColor = FILTER_RED;
		strcpy ( data->cameraDescription, "Red" );
	}
	else if ( data->cameraFilterPosition == 4 )
	{
		data->cameraFilterColor = FILTER_GREEN;
		strcpy ( data->cameraDescription, "Green" );
	}
	else if ( data->cameraFilterPosition == 5 )
	{
		data->cameraFilterColor = FILTER_BLUE;
		strcpy ( data->cameraDescription, "Blue" );
	}
	else
		result = FALSE;

	return ( result );
}

/*** GetCameraTECInfo ***************************************************

	Obtains information about the camera's thermoelectric cooling system.

	short GetCameraTECInfo ( CameraInterfaceDataPtr data )
	
	(data): pointer to camera interface data record.

	The function should always return TRUE.
		
	The function should return the camera's thermoelectric cooling system
	type in the cameraTECType member of the camera interface data record
	(data).  Currently, there are only two TEC type codes #defined in
	"CameraInterface.h": TEC_PRESENT and TEC_ABSENT.

****************************************************************************/

short GetCameraTECInfo ( CameraInterfaceDataPtr data )
{			
	data->cameraTECType = TEC_PRESENT;

	return ( TRUE );
}

/*** GetCameraAntibloomingInfo ********************************************

	Obtains information about a particular antiblooming state supported by
	the camera.

	short GetCameraAntibloomingInfo ( CameraInterfaceDataPtr data )
	
	(data): pointer to camera interface data record.

	On input, the cameraAntibloomingState member of the camera interface
	data structure (data) indicates which antiblooming state information
	is being requested for.  This value will always be greater than zero.
	
	The function should return TRUE if the requested antiblooming state
	is valid; it should return FALSE if not (i.e., if the requested state
	is less than 1 or greater than the number of states supported by the
	camera.)
		
	The function should return a textual description of the antiblooming
	state (e.g. "Off", "On") in the cameraDescription buffer of the camera
	interface data structure (data).  This description must be an ASCII NUL
	terminated string, no more than 255 characters in length.  This is the
	antiblooming state description which will appear in the antiblooming menu
	presented to the user.

****************************************************************************/

short GetCameraAntibloomingInfo ( CameraInterfaceDataPtr data )
{			
	short result = TRUE;
	
	if ( data->cameraAntibloomingState == 1 )
	{
		strcpy ( data->cameraDescription, "Off" );
	}
	else if ( data->cameraAntibloomingState == 2 )
	{
		strcpy ( data->cameraDescription, "Low" );
	}
	else if ( data->cameraAntibloomingState == 3 )
	{
		strcpy ( data->cameraDescription, "Medium" );
	}
	else if ( data->cameraAntibloomingState == 4 )
	{
		strcpy ( data->cameraDescription, "High" );
	}
	else
	{
		result = FALSE;
	}

	return ( result );
}

/*** GetCameraReadoutInfo ***************************************************

	Obtains information about a particular binning or readout mode supported
	by the camera.

	short GetCameraReadoutInfo ( CameraInterfaceDataPtr data )
	
	(data): pointer to camera interface data record.

	On input, the cameraReadoutMode member of the camera interface
	data structure (data) indicates which readout mode information is
	being requested for.  This value will always be greater than zero.
	
	The function should return TRUE if the requested readout mode is
	valid; it should return FALSE if not (i.e., if the requested readout
	mode is less than 1 or greater than the number of modes supported by
	the camera.)
	
	If the requested readout mode is valid, the function should return
	the corresponding image dimensions in the cameraImageWidth and
	cameraImageHeight members of the camera interface data record (data).
	
	Additionally, the function should return a textual description of
	the readout mode (e.g. "160 x 120 low-res" in the
	cameraDescription buffer of the camera interface data structure (data).
	This description must be an ASCII NUL terminated string, no more than
	255 characters in length.  This is the readout mode description which
	will appear in the readout mode menu presented to the user.

****************************************************************************/

short GetCameraReadoutInfo ( CameraInterfaceDataPtr data )
{
	short 		result = TRUE;
    FITSImage	*image = (FITSImage *) data->cameraPrivateData;
	
    /*** Our simulated "demo" camera will support 2 readout modes.  In the
         first (default) binning mode, the image size will be 320 x 240;
         in the second mode, it will be 160 x 120.  No other binning/readout
         modes will be supported. ***/

    if ( data->cameraReadoutMode >= 1 && data->cameraReadoutMode <= 2 )
    {
		if ( image != NULL )
    	{
    		data->cameraImageWidth = image->naxis1 / data->cameraReadoutMode;
    		data->cameraImageHeight = image->naxis2 / data->cameraReadoutMode;
    	}
    	else
    	{
    		data->cameraImageWidth = 320 / data->cameraReadoutMode;
    		data->cameraImageHeight = 240 / data->cameraReadoutMode;
    	}

		sprintf ( data->cameraDescription, "%hd x %hd",
		data->cameraImageWidth, data->cameraImageHeight );
	}
    else
	{
		result = FALSE;
	}

	return ( result );
}

/*** OpenCameraConnection ***************************************************

	Opens a communications link with the camera.
	
	short OpenCameraConnection ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	This function is always the first one that the camera interface
	procedure is called to peform.
	
	When this function is called, the following members of the camera
	interface data record (data) should be used as inputs:
	
	cameraType: the camera's type code.
	cameraPort: port number of the port to which the camera is connected.
	            
	This function should open the specified communication port, establish
	communications with the camera, and perform any other initializations
	needed by the camera driver.
	
	The same camera interface data record (data) will always be passed on
	subsequent calls to the camera interface procedure.  If the procedure
	needs to store any camera-specific data between calls, it should
	allocate memory for a struct containing such data at this time, and
	store a pointer to that struct in the "cameraPrivateData" member of
	the camera interface data record (data).
	
*****************************************************************************/

short OpenCameraConnection ( CameraInterfaceDataPtr data )
{
	short		result = TRUE;
    FILE		*file = NULL;
    FITSImage	*image = NULL;
    
	data->cameraTECState = TEC_OFF;
	data->cameraTECPowerUse = 0.0;
	data->cameraTemperatureSetpoint = 0.0;
	data->cameraTemperatureImager = 20.0;
	data->cameraTemperatureAmbient = 20.0;
	
	data->cameraFilterPosition = 1;
	data->cameraAntibloomingState = 1;
	data->cameraReadoutMode = 1;
	data->cameraShutter = SHUTTER_OPEN;
	data->cameraImageWidth = 320;
	data->cameraImageHeight = 240;

    /*** Attempt to open an image file called "DemoImage.fit".  If successful,
         read the file into memory and store a pointer to the image data in
         the camera's private data record.  This will be the image we display
         when taking simulated exposures. ***/

    file = fopen ( "DemoImage.fit", "rb" );
    if ( file != NULL )
    {
    	image = ReadFITSImage ( file );
    	fclose ( file );
    }
    
	if ( image != NULL )
    {
		data->cameraImageWidth = image->naxis1;
		data->cameraImageHeight = image->naxis2;
    }
    else
    {
		data->cameraImageWidth = 320;
		data->cameraImageHeight = 240;
    }

	data->cameraPrivateData = image;

	return ( result );
}

/*** CloseCameraConnection **************************************************

	Closes the communications link with the camera.
	
	short CloseCameraConnection ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	The CAMERA_CLOSE_CONNECTION function is always the last function
	that the camera interface procedure is called to peform.
	
	This function should end communications with the camera, close the
	camera communication port, and perform any other cleanup needed by
	the camera driver.
	
	If the OpenCameraConnection() function allocated any memory for the
	"cameraPrivateData" member of the camera interface data record (data),
	that memory should be freed now.

*****************************************************************************/

short CloseCameraConnection ( CameraInterfaceDataPtr data )
{
	int				result = TRUE;
    FITSImage		*image = (FITSImage *) data->cameraPrivateData;

    /*** If we read a FITS image file when opening the connection,
         free memory for it here. ***/
    
    if ( image != NULL )
    	FreeFITSImage ( image );

    return ( result );
}

/*** SetCameraTemperature ***************************************************

	Sets the camera's temperature regulation parameters.
	
	short SetCameraTemperature ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.

	The "cameraTECState" member of the camera interface data
	record (data) indicates whether the camera's thermoelectric cooling
	should be turned on (TEC_ON) or turned off (TEC_OFF).
	
	The "cameraTemperatureSetpoint" member indicates the desired temperature
	for the camera's imaging sensor, in degrees Celsius.
	
*******************************************************************************/

short SetCameraTemperature ( CameraInterfaceDataPtr data )
{
	short result = TRUE;
	
    if ( data->cameraTECState == TEC_ON )
    {
    	data->cameraTECPowerUse = 1.0;
    	data->cameraTemperatureImager = data->cameraTemperatureSetpoint;
    }
    else
    {
    	data->cameraTECPowerUse = 0.0;
    	data->cameraTemperatureImager = data->cameraTemperatureAmbient;
    }
    
	return ( result );
}

/*** GetCameraTemperature *****************************************************

	Obtains the camera's sensor temperature, ambient temperature, and
	thermoelectric cooling system power usage readings.
	
	short GetCameraTemperature ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.

	The camera's imaging sensor temperature measurement, in degrees Celsius,
	should be returned in the "cameraSensorTemperature" member of the
	camera interface data record (data).
	
	The camera's ambient sensor temperature measurement, in degrees Celsius,
	should be returned in the "cameraAmbientTemperature" member of the
	camera interface data record (data).
	
	The camera's thermoelectric cooling system's power usage, as a value from
	0.0 (off) to 1.0 (maximum power usage), should be returned in the
	"cameraTECPowerUsage" member of the camera interface data record (data).
	
	The "cameraTemperatureSetpoint" member indicates the desired temperature
	for the camera's imaging sensor, in degrees Celsius.
	
*******************************************************************************/

short GetCameraTemperature ( CameraInterfaceDataPtr data )
{
	short result = TRUE;
	
	return ( result );
}

/*** SetCameraFilter **********************************************************

	Sets the position of the camera's filter wheel.
	
	short SetCameraFilter ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	The "cameraFilter" member of the camera interface data record (data)
	indicates the desired position for the camera's filter wheel.  This
	will be one of the filter constants #defined in "CameraInterface.h",
	e.g. FILTER_CLEAR, FILTER_DARK, FILTER_RED, FILTER_GREEN, FILTER_BLUE.

	The "cameraSynchronous" member of the camera interface data record
	(data) indicates whether or not the function should wait for the filter
	wheel to finish moving into position before returning.  If this member is
	set to TRUE, the function should wait; if it is is FALSE, the function
	should return immediately after issuing the filter-positioning command
	to the camera.

*******************************************************************************/

short SetCameraFilter ( CameraInterfaceDataPtr data )
{
	short	result = TRUE;

    return ( result );
}

/*** GetCameraFilter ***********************************************************

	Obtains the current position of the camera's filter wheel.
	
	short GetCameraFilter ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	The position of the camera's filter wheel should be retuedn in the
	"cameraFilter" member of the camera interface data record (data), as
	one of the filter constants #defined in "CameraInterface.h", e.g.
	FILTER_CLEAR, FILTER_DARK, FILTER_RED, FILTER_GREEN, or FILTER_BLUE.

********************************************************************************/

short GetCameraFilter ( CameraInterfaceDataPtr data )
{
	short	result = TRUE;
	
	return ( result );
}

/*** StartCameraExposure *******************************************************

	Starts a new exposure integration.
	
	short StartCameraExposure ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	The "cameraExposureLength" member of the camera interface data record (data)
	indicates the desired exposure length, in seconds.
	
	The "cameraSynchronous" member of the camera interface data record (data)
	indicates whether or not the function should wait for the exposure to
	complete before returning.  If this member is set to TRUE, the function
	should wait; if it is FALSE, the function should return immediately after
	issuing the start-exposure command to the camera.

	The "cameraShutter" member of the camera interface data record (data)
	indicates whether the camera's shutter should be kept open or closed
	for the duration of the exposure.  It will be one of the shutter constants
	#defined in "CameraInterface.h", i.e. SHUTTER_OPEN or SHUTTER_CLOSED.
	
*********************************************************************************/

short StartCameraExposure ( CameraInterfaceDataPtr data )
{
	int		result = TRUE;

    return ( result );
}

/*** EndCameraExposure *********************************************************

	Ends an exposure in progress and returns the camera to its idle state.
	
	short EndCameraExposure ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	This function will be called to cancel an exposure and return the
	camera to its idle state without downloading any image data.  To
	end an exposure and prepare the camera for image download, the function
	StartCameraDownload() will be called instead.

********************************************************************************/

short EndCameraExposure ( CameraInterfaceDataPtr data )
{
	short result = TRUE;

	return ( result );
}

/*** StartCameraDownload ********************************************************

	Ends an exposure and prepares the camera for image download.
	
	short StartCameraDownload ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	To end an exposure and return the camera to its idle state without
	downloading any image data, the function EndCameraExposure() is called
	instead of this one.

**********************************************************************************/

short StartCameraDownload ( CameraInterfaceDataPtr data )
{
	int		result = TRUE;

    sRow = 0;

	return ( result );
}

/*** EndCameraDownload ***********************************************************

	Ends image download and returns the camera to its idle state.

	short EndCameraDownload ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.

	If successful, this function should return TRUE; on failure,
	it should return FALSE.

**********************************************************************************/

short EndCameraDownload ( CameraInterfaceDataPtr data )
{
	short result = TRUE;

	return ( result );
}

/*** DiscardCameraRows ************************************************************

	Discards a number of image data rows on the camera without reading them out.
	
	short DiscardCameraRows ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	The "cameraRowsDiscarded" member of the camera interface data record (data)
	indicates the number of rows to discard.

	Please note that the number of rows parameter refers to binned image rows,
	not physical camera rows.  For example, if the camera's vertical binning factor
	is 2, then a single image row corresponds to two physical camera rows, and
	the camera should discard two physical pixel rows for each image row that this
	function is called to discard.
	
***********************************************************************************/

short DiscardCameraRows ( CameraInterfaceDataPtr data )
{
	short result = TRUE;

	sRow += data->cameraDownloadDiscard;

	return ( result );
}

/*** DownloadCameraRow *************************************************************

	Reads out an image data row, or a portion of an image data row, from the camera.
	
	short DownloadCameraRow ( CameraInterfaceDataPtr data )

	(data): pointer to camera interface data record.
	
	If successful, this function should return TRUE; on failure,
	it should return FALSE.
	
	The "cameraRowStart" member of the camera interface data record (data) indicates
	the position of the first pixel in the row to digitize.  The leftmost pixel's
	position is zero; the rightmost pixel's position is the image width minus one.

	The "cameraRowWidth" member of the camera interface data record (data)
	indicates the number of pixels in the row to digitize.
	
	The "cameraRowBuffer" member of the camera interface data record (data)
	is a pointer to an array of unsigned short integers into which the digitized
	pixel values should be placed.  This array will be sized big enough to hold
	a number of pixel values equal to the "cameraRowWidth" member, above.
	
	Please note that the stating pixel position and number of pixels refer to binned
	image pixels, not physical camera pixels.  For example, if the camera's
	horizontal binning factor is 2, then a single image pixel corresponds to two
	physical camera pixels, and the image pixel width is half of the camera's
	pixel width.  Similarly, if the camera's vertical binning factor is 2, then
	this function should read out two rows of physical pixels on the camera each
	time it is called to read out a single image row.

************************************************************************************/

short DownloadCameraRow ( CameraInterfaceDataPtr data )
{
	int			result = TRUE;
	short		col;
    long		ticks;
    FITSImage	*image = (FITSImage *) data->cameraPrivateData;
    
    /*** Simulate a delay equivalent to reading out 50,000 pixels/second ***/
    
    ticks = G_TICKS_PER_SECOND * data->cameraDownloadWidth / 50000;
    if ( ticks < 1 )
    	ticks = 1;
    GWait ( ticks );

    if ( image != NULL )
    {
		for ( col = 0; col < data->cameraDownloadWidth; col++ )
			data->cameraDownloadBuffer[ col ] = image->data[0][ sRow ][ data->cameraDownloadStart + col ];
	}
    else
    {
		for ( col = 0; col < data->cameraDownloadWidth; col++ )
			data->cameraDownloadBuffer[ col ] = 0;
    }

    sRow++;
    
    return ( result );
}
