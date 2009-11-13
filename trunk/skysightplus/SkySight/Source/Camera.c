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
// #include "ParLib.h"

/*** local functions ***/

struct Camera
{
	CameraInterfaceData		cameraInterfaceData;
	CameraInterfaceProcPtr	cameraInterfaceProc;
	short					cameraExposureMode;
	short					cameraExposureFilter;
	GWindowPtr				cameraExposureWindow;
	short					cameraExposureNumber;
	short					cameraExposureTotal;
	char					cameraExposureName[256];
	GPathPtr				cameraExposurePath;
	short					cameraExposureFormat;
	ExposureList			cameraExposureList;
	ExposurePtr				cameraCurrentExposure;
	FITSImagePtr			cameraDarkFrame;
	short					cameraDarkFrameMode;
	short					cameraStatus;
	long					cameraExposureStartTime;
	short					cameraDownloadRow;
};

/*** local data ***/

static CameraPtr	sActiveCamera = NULL;

static ExposurePtr	sCameraExposure = NULL;
static ExposurePtr	sCameraExposureList = NULL;

/*** NewCameraConnection **********************************************************

	Opens a communications link with the camera and creates a new camera record.
	
	CameraPtr NewCameraConnection ( short camera, short filter, short port )

	(camera): integer specifying camera communications protocol.
	(filter): integer specifying filter type.
	(port):   integer specifying communications port that should be used.
	
	If successful, the function returns a pointer to a new camera record
	indicating the camera's type and status.  Otherwise, the function returns
	NULL.
	
	The integers specifying the camera and filter types may range from 1 to
	the total mber of supported camera/filter types.  These can be determined
	by using the functions GetNumCameraTypes() and GetNumFilterTypes().  You
	can get information about a particular camera/filter type (e.g. model name,
	type code, etc.) by using the functions GetNthCameraType()/GetNthFilterType().
	
	If there is not yet any active camera, and the camera connection can be
	opened successfully, the camera record created by this function becomes
	the active camera.
		
************************************************************************************/

CameraPtr NewCameraConnection ( short cameraType, short filterType, short port )
{
	short						result = FALSE;
	CameraPtr					camera = NULL;
	CameraInterfaceProcPtr		proc = NULL;
	
	/*** If the specified camera or filter types are invalid, return
	     an error code to indicate failure. ***/
	
	if ( cameraType < 1 || cameraType > GetNumCameraTypes() )
		return ( NULL );
	
	if ( filterType < 1 || filterType > GetNumFilterTypes() )
		return ( NULL );

	/*** Get the camera type code, camera interface procedure, and
	     filter type codes which correspond to the specified camera
	     type. ***/
	     
	cameraType = GetNthCameraType ( cameraType, NULL, &proc );
	filterType = GetNthFilterType ( filterType, NULL );

	/*** Create a new camera record.  On failure, return NULL. ***/
	
	camera = (CameraPtr) calloc ( 1, sizeof ( Camera ) );
	if ( camera == NULL )
		return ( NULL );
		
	/*** Store the camera type code, port address, and interface procedure
	     in the camera's interface data record, and initialize the camera's
	     interface data parameters. ***/
	
	camera->cameraDarkFrame = NULL;
	camera->cameraDarkFrameMode = DARK_FRAME_NEVER;
	camera->cameraExposureMode = EXPOSURE_MODE_IMAGE;
	camera->cameraExposureFilter = FILTER_CLEAR;
	camera->cameraExposureWindow = NULL;
	camera->cameraExposureList = NULL;
	camera->cameraExposurePath = NULL;
	camera->cameraExposureTotal = 1;
	camera->cameraExposureNumber = 0;
	camera->cameraExposureFormat = FILE_TYPE_FITS;
	camera->cameraCurrentExposure = NULL;
	camera->cameraStatus = CAMERA_STATUS_WAITING;
	camera->cameraInterfaceData.cameraType = cameraType;
	camera->cameraInterfaceData.cameraFilterType = filterType;
	camera->cameraInterfaceData.cameraPort = port;
	camera->cameraInterfaceData.cameraTemperatureSetpoint = 0.0;
	camera->cameraInterfaceData.cameraTECState = TEC_OFF;     
	camera->cameraInterfaceData.cameraTECPowerUse = 0;     
	camera->cameraInterfaceData.cameraTemperatureImager = 0;     
	camera->cameraInterfaceData.cameraTemperatureAmbient = 0;     
	camera->cameraInterfaceData.cameraExposureLength = 1.0;
	camera->cameraInterfaceData.cameraSynchronous = TRUE;
	camera->cameraInterfaceData.cameraFilterPosition = 1;
    camera->cameraInterfaceData.cameraShutter = SHUTTER_OPEN;
    camera->cameraInterfaceData.cameraReadoutMode = 1;
    camera->cameraInterfaceData.cameraAntibloomingState = 1;
	camera->cameraInterfaceProc = proc;
	
	strcpy ( camera->cameraExposureName, "New Exposure" );
	
	/*** Call the camera's interface procedure to open the camera
	     connection and establish communications with the camera.
	     If we fail, delete the camera record and return NULL. ***/
	     
	result = (*proc) ( CAMERA_OPEN_CONNECTION, &camera->cameraInterfaceData );

    if ( result != TRUE )
    {
    	free ( camera );
    	return ( NULL );
    }
    
	/*** If there is as yet no active camera, make the camera record we just
	     allocated the active one.  Finally, return a pointer to the new camera
	     record. ***/
	
	if ( GetActiveCamera() == NULL )
		SetActiveCamera ( camera );
		
	return ( camera );
}

/*** DeleteCameraConnection ******************************************************

	Closes the communications link with the camera and deletes the corresponding
	camera record.
	
	void CloseCameraConnection ( CameraPtr camera )

	(camera): pointer to camera record you wish to delete.
	
	The function returns nothing.  If you delete the active camera record, the
	active camera connection will be set to NULL.
		
**********************************************************************************/

void DeleteCameraConnection ( CameraPtr camera )
{
	int result = FALSE;
	
	/*** Close the camera's communications link. ***/
	     
	result = (*camera->cameraInterfaceProc) ( CAMERA_CLOSE_CONNECTION, &camera->cameraInterfaceData );

	/*** Free memory for the camera's dark frame image buffer (if it has one),
	     unlink the camera from its exposure results window (if it has one),
	     free the camera's linked-list of exposure records (if present),
	     and then free memory for the camera record itself. ***/
	     
	if ( camera->cameraDarkFrame != NULL )
		FreeFITSImage ( camera->cameraDarkFrame );
	
	if ( camera->cameraExposureWindow != NULL )
		SetImageWindowCamera ( camera->cameraExposureWindow, NULL );
	
	if ( camera->cameraExposureList != NULL )
		DeleteExposureList ( camera->cameraExposureList );
	
	if ( camera->cameraExposurePath != NULL )
		GDeletePath ( camera->cameraExposurePath );
			
	free ( camera );

	/*** If we just deleted the camera record which corresponds to the active
	     camera connection, set the active camera to NULL. ***/
	     
	if ( camera == GetActiveCamera() )
		SetActiveCamera ( NULL );
}

/*** GetActiveCamera **********************************************************

	Returns a pointer to the active camera.
	
	CameraPtr GetActiveCamera ( void )
	
	The function returns a pointer to the camera whose status and properties
	are currently being controlled in the "Camera Control" dialog.

	If the application is not currently controlling any camera, the function
	returns NULL.
		
*******************************************************************************/

CameraPtr GetActiveCamera ( void )
{
	return ( sActiveCamera );
}

/*** SetActiveCamera **********************************************************

	Designates a particular camera record as the active camera.
	
	void SetActiveCamera ( CameraPtr camera )
	
	(camera): pointer to a camera record you wish to make active.
	
	This function does not update the contents of the "Camera Control" dialog
	to reflect any change in the active camera.
			
*******************************************************************************/

void SetActiveCamera ( CameraPtr camera )
{
	sActiveCamera = camera;
}

/*** GetCameraType ************************************************************

	Returns an integer value specifying the protocol that should be used when
	communicating with the camera.
	
	short GetCameraType ( CameraPtr camera )

	(camera): pointer to camera record whose camera type you wish to determine.
	
	The value returned by this function will be one of the camera type
	contants #defined in SkySight.h, e.g. CAMERA_TYPE_SBIG_ST4, etc.

********************************************************************************/

short GetCameraType ( CameraPtr camera )
{
	return ( camera->cameraInterfaceData.cameraType );
}

/*** GetCameraPort *************************************************************

	Returns the port number of the communication port used to communicate
	with a camera.
	 
	short GetCameraPort ( void )

	(camera): pointer to camera record whose connection port number you wish
	          to determine.
	
*********************************************************************************/

short GetCameraPort ( CameraPtr camera )
{
	return ( camera->cameraInterfaceData.cameraPort );
}

/*** CameraHasCooling ***************************************************************

	Determines whether or not the camera has a temperature regulation system.

	short CameraHasCooling ( CameraPtr camera )
	
	(camera): pointer to camera you wish to query.
	
*************************************************************************************/

short CameraHasCooling ( CameraPtr camera )
{
	short	result;
	
	result = (*camera->cameraInterfaceProc) ( CAMERA_GET_TEC_INFO, &camera->cameraInterfaceData );

	if ( camera->cameraInterfaceData.cameraTECType == TEC_PRESENT )
		return ( TRUE );
	else
		return ( FALSE );
}

/*** CameraHasShutter ***************************************************************

	Determines whether or not the camera has a shutter.

	short CameraHasShutter ( CameraPtr camera )
	
	(camera): pointer to camera you wish to query.
	
*************************************************************************************/

short CameraHasShutter ( CameraPtr camera )
{
	short	result;
	
	result = (*camera->cameraInterfaceProc) ( CAMERA_GET_SHUTTER_INFO, &camera->cameraInterfaceData );

	if ( camera->cameraInterfaceData.cameraShutterType == SHUTTER_TYPE_NONE )
		return ( FALSE );
	else
		return ( TRUE );
}

/*** SetCameraTemperatureRegulation ***************************************************

	Turns the camera's thermal regulation off or on, and sets the temperature
	setpoint (e.g. desired temperature) for the imaging device.

	short SetCameraTemperatureRegulation ( CameraPtr camera, short state, float setpoint )
	
	(camera): pointer to camera whose thermal regulation parameters you wish to set.
	(state): desired regulation state, e.g. FALSE (off) or TRUE (on)
	(setpoint): desired temperature, in degrees centigrade.
	
	The function returns TRUE if it is able to complete the command
	successfully, or FALSE if it fails.
	
****************************************************************************************/

short SetCameraTemperatureRegulation ( CameraPtr camera, short state, float setpoint )
{
	short	result = FALSE;
	short	oldState = camera->cameraInterfaceData.cameraTECState;
	short	oldSetpoint = camera->cameraInterfaceData.cameraTemperatureSetpoint;
	
	camera->cameraInterfaceData.cameraTECState = state;
	camera->cameraInterfaceData.cameraTemperatureSetpoint = setpoint;
	
	result = (*camera->cameraInterfaceProc) ( CAMERA_SET_TEMPERATURE, &camera->cameraInterfaceData );
	
	if ( result != TRUE )
	{
		camera->cameraInterfaceData.cameraTECState = oldState;
		camera->cameraInterfaceData.cameraTemperatureSetpoint = oldSetpoint;
	}
	
    return ( result );
}

/*** GetCameraTemperatureRegulation ******************************************************

	Determines the camera's temperature regulation parameters.
	
	int GetCameraTemperatureRegulation ( CameraPtr camera, short *state, float *setpoint )
	
	(camera):   pointer to camera whose thermal regulation parameters you wish to set.
	(state):    recieves camera's thermal regulation state, e.g. TRUE (on) or FALSE (off).
	(setpoint): recieves the camera's temperature setpoint, in degrees C.
	
	This function always returns TRUE.  The values returned in (state) and/or
	(setpoint) are simply the values most recently passed to the function
	SetCameraTemperatureRegulation().
	
	You can ignore either the state or setpoint value by passing NULL in
	for the respective parameter.
	
******************************************************************************************/

short GetCameraTemperatureRegulation ( CameraPtr camera, short *state, float *setpoint )
{
	if ( state != NULL )
		*state = camera->cameraInterfaceData.cameraTECState;
		
	if ( setpoint != NULL )
    	*setpoint = camera->cameraInterfaceData.cameraTemperatureSetpoint;

    return ( TRUE );
}

/*** GetCameraTemperatureStatus ***********************************************************

	Obtains the current temperatures for the camera's imaging device, ambient
	temperature, and thermal regulator power usage.
	
	int GetCameraTemperatureStatus ( CameraPtr camera, float *tempImg, float *tempAmb,
	    float *powerUse )
	
	(camera):   pointer to camera whose thermal parameters you wish to determine.
	(tempImg):  receives the camera's imaging device temperature, in degrees C.
	(tempAmb):  recieves the camera's ambient temperature, in degrees C.
	(powerUse): recieves the camera's thermal regulation system's power usage,
	            as a value from 0.0 (no power usage) to 1.0 (maximum power)
	            
	The function returns TRUE if successful, or FALSE on failure.
	
********************************************************************************************/

short GetCameraTemperatureStatus ( CameraPtr camera, float *tempImg, float *tempAmb, float *power )
{
	short		result = FALSE;

	result = (*camera->cameraInterfaceProc) ( CAMERA_GET_TEMPERATURE, &camera->cameraInterfaceData );
	
	if ( result == TRUE )
	{
		if ( tempImg != NULL )
			*tempImg = camera->cameraInterfaceData.cameraTemperatureImager;
			
		if ( tempAmb != NULL )
			*tempAmb = camera->cameraInterfaceData.cameraTemperatureAmbient;
			
		if ( power != NULL )
			*power = camera->cameraInterfaceData.cameraTECPowerUse;
	}
	
    return ( result );
}

/*** NewCameraDarkFrame ********************************************************

	Creates a new camera dark frame image buffer, replacing any that the camera
	already has.

	FITSImagePtr NewCameraDarkFrame ( CameraPtr camera, short width, short height )
	
	(camera): pointer to camera record.
	(width):  camera's image width, in pixels.
	(height): camera's image height, in pixels.
	
	The function returns a pointer to the newly-allocated dark frame image
	buffer, if successful, or NULL on failure.
	
	If the camera already has a dark-frame image, it is destroyed and replaced
	with the new image created by this function.
	
	You should create a new dark frame buffer before starting a new exposure
	if the camera's dark frame mode is anything other than DARK_FRAME_NEVER
	(see SetCameraDarkFrameMode().)  Additionally, you must enure that the
	dark frame's width and height match the image's width and height, as
	returned by GetCameraImageSize(); because of this, you will need to
	reallocate the camera's dark frame image buffer if you change its
	binning mode.
	 
********************************************************************************/

FITSImagePtr NewCameraDarkFrame ( CameraPtr camera, short width, short height )
{
	FITSImagePtr	image;
	
	image = NewFITSImage ( 16, 3, width, height, 1, 0.0, 1.0 );
	if ( image == NULL )
		return ( NULL );

	if ( camera->cameraDarkFrame != NULL )
		FreeFITSImage ( camera->cameraDarkFrame );
		
	camera->cameraDarkFrame = image;
	return ( image );
}

/*** GetCameraDarkFrame ********************************************************

	Returns a pointer to a camera's current dark frame image buffer.

	FITSImagePtr GetCameraDarkFrame ( CameraPtr camera )

	(camera): pointer to camera record.
	
	If the camera does not have a dark-frame image buffer, this function
	returns NULL.
	
********************************************************************************/

FITSImagePtr GetCameraDarkFrame ( CameraPtr camera )
{
	return ( camera->cameraDarkFrame );
}

/*** SetCameraDarkFrameMode **********************************************************

	Changes a camera's behavior with regard to taking dark frames and/or
	applying them to light image exposures.
	
	void SetCameraDarkFrameMode ( CameraPtr camera, short mode )

	(camera): pointer to camera record.
	(mode):   desired dark frame mode specifier.
	
	The following dark frame modes are currently supported:
	
	DARK_FRAME_NEVER.  The camera never takes dark frames, and does not perform
	any dark-fram subtraction from light image exposures.
	
	DARK_FRAME_ONLY.  The camera takes only dark exposures, not light exposures.
	The results of each exposure are copied to the camera's dark-frame image
	buffer, and displayed in the camera's exposure window.
	
	DARK_FRAME_ALWAYS.  The camera takes a dark frame before every light exposure.
	The results of the dark frame are displayed in the exposure window, then copied
	to the camera's dark frame image buffer.  The camera then takes a light exposure,
	and subtracts the dark frame from it afterwards.  Finally, the results of the
	dark-subtracted light exposure are displayed in the camera's exposure results
	window.
	
	DARK_FRAME_AUTOMATIC.  Not yet implemented.
	
**************************************************************************************/

void SetCameraDarkFrameMode ( CameraPtr camera, short mode )
{
	camera->cameraDarkFrameMode = mode;
}

/*** GetCameraDarkFrameMode *****************************************************

	Returns a camera's dark frame behavior mode constant.

	short GetCameraDarkFrameMode ( CameraPtr camera )

	(camera): pointer to camera record.
	
	The dark frame behavior mode constant returned will be one of the values
	#defined in "SkySight.h".  See SetCameraDarkFrameMode() for more information.
	
*********************************************************************************/

short GetCameraDarkFrameMode ( CameraPtr camera )
{
	return ( camera->cameraDarkFrameMode );
}

/*** SetCameraExposureMode ******************************************************

	Changes the camera's exposure mode.

	void SetCameraExposureMode ( short mode )
	
	(mode): camera exposure mode constant, as #defined in "SkySight.h"
	
	This function returns nothing.

	The following camera exposure modes are currently supported:
	
	EXPOSURE_MODE_IMAGE.  The camera takes a single exposure and displays it
	in the camera's exposure window.  The entire image is read out, regarless of
	what portion has been selected by the user.  Any binning should be performed
	in software, as opposed to on-chip (to prevent ADC overflows).  The goal here
	is to maximize image quality.
	
	EXPOSURE_MODE_FOCUS.  The camera takes repeated exposures and displays each
	one in the camera's exposure window.  Additionally, only the selected image
	region should downloaded from the camera after each exposure; and any binning
	should be performed on-chip.  The goal here is to minimize download time.
	
	If this function is called while an exposure is already in progress,
	it should change the exposure mode at the start of the next exposure,
	not the current one.

*********************************************************************************/

void SetCameraExposureMode ( CameraPtr camera, short mode )
{
	camera->cameraExposureMode = mode;
}

/*** GetCameraExposureMode ******************************************************

	Returns a camera's current exposure mode.

	short GetCameraExposureMode ( CameraPtr camera )
	
	This function returns the current camera's exposure mode constant,
	as #defined in "SkySight.h".  See SetCameraExposureMode() for more
	information.

*********************************************************************************/

short GetCameraExposureMode ( CameraPtr camera )
{
	return ( camera->cameraExposureMode );
}

/*** SetCameraExposureFilter ****************************************************

	Sets the default position for the camera's filter wheel.
	
	void SetCameraExposureFilter ( CameraPtr camera, short filter )
	
	(camera): pointer to camera record.
	(filter): desired default filter position.

	The filter positions #defined in SkySight.h are FILTER_NONE (clear),
	FILTER_DARK (opaque), FILTER_RED (red), FILTER_GREEN (green), and
	FILTER_BLUE (blue).

	This function does not actually change the filter wheel's position;
	to physically move to filter, call SetCameraFilter().  Instead, this
	function sets the position that the camera's filter wheel should move
	to by default, after it has finished taking an exposure or a sequence
	of exposures.  Note that the filter's position actual position while
	taking an exposure, may differ from this: e.g. if the camera's default
	filter position is set to "red", but the camera is currently taking a
	dark frame, the filter's actual position may be temporarily set to
	"dark".
		
	To obtain the camera's default filter wheel position, use the function
	GetCameraExposureFilter().  To obtain the camera's actual, instantaneous
	filter wheel position, use the function GetCameraFilter().
	
********************************************************************************/

void SetCameraExposureFilter ( CameraPtr camera, short filter )
{
	camera->cameraExposureFilter = filter;
}

/*** GetCameraExposureFilter ******************************************************

	Returns a camera's default filter position.

	short GetCameraExposureFilter ( CameraPtr camera )

	(camera): pointer to camera record whose filter default position
	          you wish to determine.
	
	See SetCameraExposureFilter() for more information. 
	
***********************************************************************************/

short GetCameraExposureFilter ( CameraPtr camera )
{
	return ( camera->cameraExposureFilter );
}

/*** SetCameraExposureShutter ***************************************************

	Controls whether the shutter should be open or closed while exposing.

	int SetCameraExposureShutter ( short state )
	
	(camera): pointer to camera record.
	(state):  camera shutter state constant, as #defined in "SkySight.h"
	
	This function returns nothing.

	Two camera shutter modes are currently supported: SHUTTER_CLOSED and
	SHUTTER_OPEN.  In the "closed" mode, the camera's shutter is kept
	closed during the exposure, so that no light is allowed to fall on
	the imaging device.  In the "open" mode, the shutter is kept open
	for the duration of the exposure but closed while the image is being
	read out.
	
	If this function is called while an exposure is already in progress,
	it should change the shutter mode at the start of the next exposure,
	not for the current one.

*********************************************************************************/

void SetCameraExposureShutter ( CameraPtr camera, short state )
{
	camera->cameraInterfaceData.cameraShutter = state;
}

/*** GetCameraExposureShutter ***************************************************

	Returns current behavior of the camera's shutter during an exposure.

	short GetCameraExposureShutter ( CameraPtr camera )
	
	(camera): pointer to camera record.
	
	This function returns the current camera shutter mode constant,
	as #defined in "SkySight.h".  See SetCameraExposureShutter() for more
	information.

*********************************************************************************/

short GetCameraExposureShutter ( CameraPtr camera )
{
	return ( camera->cameraInterfaceData.cameraShutter );
}

/*** SetCameraExposureLength **************************************************

	Sets a camera exposure length.
	
	void SetCameraExposureLength ( CameraPtr camera, float length )

	(camera): pointer to camera record.
	(length): desired camera exposure length, in seconds.

	This function returns nothing.  If you change the exposure length while
    an exposure is currently in progress, the change will not take effect
    until the next exposure is started.

*******************************************************************************/

void SetCameraExposureLength ( CameraPtr camera, float length )
{
	camera->cameraInterfaceData.cameraExposureLength = length;
}

/*** GetCameraExposureLength **************************************************

	Returns the current camera exposure length, in seconds.
	
	float GetCameraExposureLength ( CameraPtr camera )

	(camera): pointer to camera record.

*******************************************************************************/

float GetCameraExposureLength ( CameraPtr camera )
{
	return ( camera->cameraInterfaceData.cameraExposureLength );
}

/*** GetCameraExposureWindow ******************************************************

	Returns a pointer to the window in which the results of the camera's
	exposures should be displayed.

	GWindowPtr GetCameraExposureWindow ( CameraPtr camera )
	
	(camera): pointer to camera record.
	
	If the camera does not yet have an exposure window, this function should
	return NULL.
	
***********************************************************************************/

GWindowPtr GetCameraExposureWindow ( CameraPtr camera )
{
	return ( camera->cameraExposureWindow );
}

/*** SetCameraExposureWindow ***************************************************

	Designates an image window as the window in which output from a camera
	will be displayed.

	void SetCameraExposureWindow ( CameraPtr camera, GWindowPtr window )
	
	(camera): pointer to camera record.
	(window): pointer to the desired image window for the camera's output.
	
	You can pass NULL for the window pointer if you do not want to designate
	any window as the camera's exposure window; if you do this, StartExposure()
	will create a new image window for the camera when a new exposure begins.
	
	If the image window pointer (window) is non-NULL, then calling this
	function will also link the image window to the camera record (camera).
	
********************************************************************************/

void SetCameraExposureWindow ( CameraPtr camera, GWindowPtr window )
{
	camera->cameraExposureWindow = window;
	
	/*** If we have a valid image window pointer, we need to check to see
	     if the window's camera has already been set to point to our
	     camera; otherwise, we'll have a race condition if we try to
	     set it now. ***/
	     
	if ( window != NULL )
		if ( GetImageWindowCamera ( window ) != camera )
			SetImageWindowCamera ( window, camera );
}

/*** GetCameraExposureList ***/

ExposureList GetCameraExposureList ( CameraPtr camera )
{
	return ( camera->cameraExposureList );
}

/*** SetCameraExposureList ***/

void SetCameraExposureList ( CameraPtr camera, ExposureList list )
{
	camera->cameraExposureList = list;
}

/*** GetCameraExposure ***/

ExposurePtr GetCameraExposure ( CameraPtr camera )
{
	return ( camera->cameraCurrentExposure );
}

/*** SetCameraExposure ***/

void SetCameraExposure ( CameraPtr camera, ExposurePtr exposure )
{
	camera->cameraCurrentExposure = exposure;
}

/*** GetCameraExposurePath ***************************************************

	Returns a pointer to a camera's exposure path record.
	
	GPathPtr GetCameraExposurePath ( CameraPtr camera )

	(camera): pointer to a camera record.
	
	The function returns a pointer to the camera's exposure path record.
	This is the default file path specification where new exposures will
	be saved.
	
	This function will return NULL if the camera's exposure path has not yet
	been specified.

******************************************************************************/

GPathPtr GetCameraExposurePath ( CameraPtr camera )
{
	return ( camera->cameraExposurePath );
}

/*** SetCameraExposurePath ***************************************************

	Modifies a camera's exposure path record.
	
	void SetCameraExposurePath ( CameraPtr camera, GPathPtr path )

	(camera): pointer to a camera record.
	(path):   pointer to path specification record.
	
	The function modifies a camera's exposure path specification record.
	This is the default file path specification where new exposures will
	be saved.
	
	This function stores a COPY of the given path specification record
	in the camera record; it does not store a pointer to the  path record
	itself.

******************************************************************************/

void SetCameraExposurePath ( CameraPtr camera, GPathPtr path )
{
	if ( path != camera->cameraExposurePath )
	{
		if ( camera->cameraExposurePath != NULL )
			GDeletePath ( camera->cameraExposurePath );
			
		camera->cameraExposurePath = GCreatePath ( path );
	}
}

/*** GetCameraExposureName ***************************************************

	Returns a pointer to a camera's exposure name string.
	
	char *GetCameraExposureName ( CameraPtr camera )

	(camera): pointer to a camera record.
	
	The function returns a pointer to the camera's exposure name string.
	This is an ASCII NUL-terminated string which contains the default file
	name under which new exposures will be saved.  
		
******************************************************************************/

char *GetCameraExposureName ( CameraPtr camera )
{
	return ( camera->cameraExposureName );
}

/*** SetCameraExposureName ***************************************************

	Modifies a camera's exposure name string.
	
	void SetCameraExposureName ( CameraPtr camera, char *name )

	(camera): pointer to a camera record.
	(name):   ASCII NUL-terminated string containing new exposure name.
	
	The function returns nothing.  The exposure name is an ASCII NUL-terminated
	string which contains the default file name under which new exposures will
	be saved.  
		
******************************************************************************/

void SetCameraExposureName ( CameraPtr camera, char *name )
{
	strncpy ( camera->cameraExposureName, name, sizeof ( camera->cameraExposureName ) );
}

/*** SetCameraExposureFormat ***************************************************

	Modifies a camera's exposure file format.
	
	void SetCameraExposureFormat ( CameraPtr camera, short format )

	(camera): pointer to a camera record.
	(format): file format code.
	
	The function returns nothing.
	
	The exposure format is the file format in which new exposures will be
	saved, and is one of the file format codes specified in "SkySight.h".
			
********************************************************************************/

void SetCameraExposureFormat ( CameraPtr camera, short format )
{
	camera->cameraExposureFormat = format;
}

/*** GetCameraExposureFormat ***************************************************

	Returns a camera's exposure file format.
	
	short GetCameraExposureFormat ( CameraPtr camera )

	(camera): pointer to a camera record.
	
	The function returns the exposure format code.  The exposure format is
	the file format in which new exposures will be saved, and is one of the
	file format codes specified in "SkySight.h".
		
********************************************************************************/

short GetCameraExposureFormat ( CameraPtr camera )
{
	return ( camera->cameraExposureFormat );
}

/*** SetCameraExposureNumber **************************************************

	Modifies the number of the current exposure in a multiple-exposure
	sequence.
	
	void SetCameraExposureNumber ( CameraPtr camera, short number )
	
	(camera): pointer to a camera record.
	(number):  number of current exposure in a camera exposure sequence.
	
	This function returns nothing.
	
	The number exposure number should range from 1 to the total number
	of exposures in the sequence (see GetCameraExposureTotal()).
	It should be zero if there is no exposure in progress.

	Please note that the camera's exposure count is only used if the
	exposure mode is set to "Multiple Image".
	
******************************************************************************/

void SetCameraExposureNumber ( CameraPtr camera, short number )
{
	camera->cameraExposureNumber = number;
}

/*** GetCameraExposureNumber **************************************************

	Returns the number of the current exposure in a multiple-exposure
	sequence.
	
	short GetCameraExposureNumber ( CameraPtr camera )
	
	(camera): pointer to a camera record.
	
	This function returns the number of the current exposure in a multiple-
	exposure sequence.  The number should range from 1 to the total number
	of exposures in the sequence (see GetCameraExposureTotal()).
	It should be zero if there is no exposure in progress.

	Please note that the camera's exposure number is only used if the
	exposure mode is set to "Multiple Image".
	
******************************************************************************/

short GetCameraExposureNumber ( CameraPtr camera )
{
	return ( camera->cameraExposureNumber );
}

/*** SetCameraExposureTotal **************************************************

	Modifies the total number of exposures in a multiple-exposure sequence.
	
	void SetCameraExposureTotal ( CameraPtr camera, short total )
	
	(camera): pointer to a camera record.
	(total):  total number of exposures to take in camera exposure sequence.
	
	This function returns nothing.
	
	Please note that the camera's exposure total is only used if the
	exposure mode is set to "Multiple Image".
	
******************************************************************************/

void SetCameraExposureTotal ( CameraPtr camera, short total )
{
	camera->cameraExposureTotal = total;
}

/*** GetCameraExposureTotal **************************************************

	Returns the total number of exposure's in a camera's multiple-exposure
	sequence.
	
	short GetCameraExposureTotal ( CameraPtr camera )
	
	(camera): pointer to a camera record.
	
	This function returns the number of exposures in a camera's multiple-
	exposure sequence.

	Please note that the camera's exposure total is only used if the
	exposure mode is set to "Multiple Image".
	
******************************************************************************/

short GetCameraExposureTotal ( CameraPtr camera )
{
	return ( camera->cameraExposureTotal );
}

/*** GetCameraFilterInfo *****************************************************

	Obtains information about a particular filter in a camera's filter wheel.

	short GetCameraFilterInfo ( CameraPtr camera, short position, short *color,
	char *name )
	
	(camera):   pointer to camera record.
	(position): position of filter wheel you wish to obtain information about.
	(color):    recieves color code of filter position.
	(name):     recieves name of filter.
	
	The function returns TRUE if successful, or FALSE on failure.  For cameras
	which have no filter, this function always returns FALSE.
	
******************************************************************************/

short GetCameraFilterInfo ( CameraPtr camera, short position, short *color, char *name )
{
	short result = FALSE, savedPosition;
	
	if ( camera->cameraInterfaceData.cameraFilterType == FILTER_TYPE_NONE )
		return ( result );
		
	savedPosition = camera->cameraInterfaceData.cameraFilterPosition;
	camera->cameraInterfaceData.cameraFilterPosition = position;
	
	result = (*camera->cameraInterfaceProc) ( CAMERA_GET_FILTER_INFO, &camera->cameraInterfaceData );
	
	if ( result == TRUE )
	{
		if ( color != NULL )
			*color = camera->cameraInterfaceData.cameraFilterColor;
			
		if ( name != NULL )
			strcpy ( name, camera->cameraInterfaceData.cameraDescription );
	}
	
	camera->cameraInterfaceData.cameraFilterPosition = savedPosition;
	return ( result );
}

/*** GetCameraFilter *********************************************************

	Determines the camera's current filter wheel position.
	
	short GetCameraFilter ( CameraPtr camera, short *position )

	(camera): pointer to camera record.
	(position): recieves camera's filter wheel position.
	
	The function returns TRUE if successful, or FALSE if it fails to
	determine the camera's filter position.  For cameras which have no
	filter, this function always returns FALSE.
	
	On failure, the value in (filter) is not modified.
	
	This function returns the filter's actual, instantaneous position at the
	time this function is called.  It does not return the filter's default
	position, which is obtained by GetCameraExposureFilter().
	
******************************************************************************/

short GetCameraFilter ( CameraPtr camera, short *position )
{
	short		result = FALSE;
	
	if ( camera->cameraInterfaceData.cameraFilterType == FILTER_TYPE_NONE )
		return ( result );

	result = (*camera->cameraInterfaceProc) ( CAMERA_GET_FILTER, &camera->cameraInterfaceData );
	
	if ( result == TRUE )
		*position = camera->cameraInterfaceData.cameraFilterPosition;
	
	return ( result );
}

/*** SetCameraFilter ***********************************************************

	Moves the camera's color filter wheel.
	
	int SetCameraFilter ( CameraPtr camera, short position, short wait )
	
	(camera):      pointer to camera record.
	(position):    desired color filter wheel position.
	(synchronous): indicates whether or not to wait for filter wheel
	               to finish moving into position before returning.

	The function returns TRUE if the camera's filter wheel can be set
	to the desired position, and FALSE otherwise.  Valid values for the 
	filter position range from 1 to the total number of positions in the
	filter wheel.  You can obtain information about a particular filter
	by calling the function GetCameraFilterInfo().

	For cameras which have no filter, this function always returns FALSE.
	
	The (synchronous) parameter determines whether or not the function
	will return immediately after issuing the "set filter" command to the
	camera, or whether it will wait until the filter wheel has finished
	moving into the desired position.  If TRUE, the function waits for
	the filter wheel to stop moving before returning; if FALSE, the
	function returns immedately after issuing the "set filter" command.
	
	Please note that changing the filter position by calling this function
	does not change the camera's default filter position.  That can only
	be changed by calling function SetCameraExposureFilter().
	
	To obtain the camera's current filter wheel position, use the function
	GetCameraFilter().  To obtain the camera's default filter wheel position,
	use the function GetCameraExposureFilter().
	
********************************************************************************/

short SetCameraFilter ( CameraPtr camera, short position, short wait )
{
	short		result = FALSE;
	
	if ( camera->cameraInterfaceData.cameraFilterType == FILTER_TYPE_NONE )
		return ( result );
		
	camera->cameraInterfaceData.cameraFilterPosition = position;
	camera->cameraInterfaceData.cameraSynchronous = wait;
	
	result = (*camera->cameraInterfaceProc) ( CAMERA_SET_FILTER, &camera->cameraInterfaceData );
	
	return ( result );
}

/*** GetCameraAntibloomingInfo ************************************************

	Obtains information about a particular antiblooming mode supported by the
	camera.

	short GetCameraAntibloomingInfo ( CameraPtr camera, short state, char *name )
	
	(camera): pointer to camera record.
	(state):  antiblooming state we are curious about.
	(name):   recieves name of antiblooming state.
	
	The function returns TRUE if the antiblooming state is valid, and FALSE
	otherwise.  Valid antiblooming states range from 1 (the default) to the
	total number of antiblooming states supported by the camera.
	
*******************************************************************************/

short GetCameraAntibloomingInfo ( CameraPtr camera, short state, char *name )
{
	short result, savedState;
	
	savedState = camera->cameraInterfaceData.cameraAntibloomingState;
	camera->cameraInterfaceData.cameraAntibloomingState = state;
	
	result = (*camera->cameraInterfaceProc) ( CAMERA_GET_ANTIBLOOMING_INFO, &camera->cameraInterfaceData );
	
	if ( result == TRUE )
	{
		if ( name != NULL )
			strcpy ( name, camera->cameraInterfaceData.cameraDescription );
	}
	
	camera->cameraInterfaceData.cameraAntibloomingState = savedState;
	return ( result );
}

/*** SetCameraAntiblooming ******************************************************

	Changes the camera's antiblooming gate state.

	void SetCameraAntiblooming ( CameraPtr camera, int state )
	
	(camera): pointer to camera record whose antiblooming you wish to set.
	(state):  camera antiblooming state.
	
	This function returns nothing.

	Valid values for the antiblooming state parameter (state) range from
	1 (the default) to the total number of antiblooming gate states supported
	by the camera.  To obtain a description of a particular antiblooming state,
	use the function GetCameraAntibloomingInfo(); you can also use this function
	to count the total number of antiblooming states supported by the camera.
	
	If this function is called while an exposure is already in progress,
	it should change the antiblooming gate state at the start of the next
	exposure, not the current one.

*********************************************************************************/

short SetCameraAntiblooming ( CameraPtr camera, short state )
{
	camera->cameraInterfaceData.cameraAntibloomingState = state;
	
	return ( TRUE );
}

/*** GetCameraAntiblooming ******************************************************

	Returns the camera's current antiblooming gate state.

	short GetCameraAntiblooming ( CameraPtr camera )
	
	(camera): pointer to camera record whose antiblooming you wish to determine
	
	This function returns the camera's current antibloming gate state.

*********************************************************************************/

short GetCameraAntiblooming ( CameraPtr camera, short *state )
{
	(*state) = camera->cameraInterfaceData.cameraAntibloomingState;
	
	return ( TRUE );
}

/*** GetCameraReadoutModeInfo *******************************************************

	Obtains information about a particular readout mode supported by the camera.

	int GetCameraReadoutModeInfo ( CameraPtr camera, short mode,
	    short *width, short *height, char *name )

	(camera): pointer to camera whose image size we wish to determine.
	(mode):   readout mode specifier.
    (width):  receives the width of an image frame, in pixels.
    (height): receives the height of an image frame, in pixels.
	(name):   recieves text description of readout mode.
	
    The function returns TRUE if it can successfully query the camera,
    and FALSE if the specified readout mode is not supported by the camera.
    Valid values for the readout mode specification parameter (mode) range
    from 1 to the total number of readout modes supported by the camera.  
    
    The function should return the image size in the camera's current binning
    mode, not the camera's physical pixel dimensions.  For example, if a
    camera's physical pixel dimensions are 320 x 240, and the binning is set
    to 2 x 2, the image width and height returned by this function should be
    160 x 120.
    
******************************************************************************/

short GetCameraReadoutModeInfo ( CameraPtr camera, short mode,
short *width, short *height, char *name )
{
	short result, savedMode = camera->cameraInterfaceData.cameraReadoutMode;
	
	camera->cameraInterfaceData.cameraReadoutMode = mode;
	result = (*camera->cameraInterfaceProc) ( CAMERA_GET_READOUT_INFO, &camera->cameraInterfaceData );
	if ( result )
	{
		if ( width != NULL )
			*width = camera->cameraInterfaceData.cameraImageWidth;
			
		if ( height != NULL )
			*height = camera->cameraInterfaceData.cameraImageHeight;

		if ( name != NULL )
			strcpy ( name, camera->cameraInterfaceData.cameraDescription );
	}
	
	camera->cameraInterfaceData.cameraReadoutMode = savedMode;
    return ( result );
}

/*** GetCameraReadoutMode *************************************************

	Returns a camera's current readout mode.

	short GetCameraReadoutMode ( CameraPtr camera, short *mode )
	
	(camera): pointer to camera record.
	(mode):   recieves camera's readout mode.
	
	This function returns TRUE if able to successfully query the camera,
	and FALSE otherwise.

*******************************************************************************/

short GetCameraReadoutMode ( CameraPtr camera, short *mode )
{
	*mode =	camera->cameraInterfaceData.cameraReadoutMode;
	
	return ( TRUE );
}

/*** SetCameraReadoutMode ***************************************************

	Changes a camera's readout mode.

	short SetCameraReadoutMode ( CameraPtr camera, short mode )
	
	(camera): pointer to camera record.
	(mode):   camera readout mode.
	
	This function returns TRUE if successful, or FALSE on failure.
	
	The readout mode value (mode) should range from 1 (for the default
	readout mode) to the total number of readout modes supported by the camera.
	Values outside this range will cause this function to fail.

	The camera's current readout mode can be determined by calling the function
	GetCameraReadoutMode().  The dimensions of an image read out in the
	current binning mode can be determined with the function
	GetCameraReadoutModeInfo().
	
	If this function is called while an exposure is already in progress,
	it should change the readout mode at the start of the next exposure,
	not the current one.

*********************************************************************************/

short SetCameraReadoutMode ( CameraPtr camera, short mode )
{
	short result;
	
	camera->cameraInterfaceData.cameraReadoutMode = mode;

	result = (*camera->cameraInterfaceProc) ( CAMERA_GET_READOUT_INFO, &camera->cameraInterfaceData );
		
	return ( result );
}

/*** GetCameraStatus ************************************************************

	Determines the camera's current exposure status code, and optionally
	the amount of time elapsed since the start of the current exposure
	integration, and/or the number of rows that have been downloaded.

	int GetCameraStatus ( CameraPtr camera, float *time, short *rows )

	(camera): pointer to camera record whose status you wish to determine.
    (time):   receives number of seconds since start of integration.
    (rows):   recieves number of rows downloaded from the camera.
    
    The function returns the camera's current exposure status code, as set
    by the most recent call to SetCameraStatus().

    If there is an exposure integration in progress, the number of seconds
    elapsed since the start of the exposure is returned in the (time) parameter.
    If not, the contents of (time) are unchanged.  You can ignore the time
    elapsed by passing NULL for this parameter.
    
    Similarly, if there is a download in progress, the number of rows downloaded
    is returned in the (rows) parameter.  You can ignore the number of rows
    downloaded by passing NULL for this parameter.
    
********************************************************************************/

short GetCameraStatus ( CameraPtr camera, float *time, short *rows )
{
	if ( camera->cameraStatus == CAMERA_STATUS_EXPOSING )
		if ( time != NULL )
			*time = (float) ( GGetTickCount() - camera->cameraExposureStartTime ) / G_TICKS_PER_SECOND;

	if ( camera->cameraStatus == CAMERA_STATUS_DOWNLOADING )
		if ( rows != NULL )
			*rows = camera->cameraDownloadRow;
			
	return ( camera->cameraStatus );
}

/*** StartCameraExposure *******************************************************

	Starts a new exposure.
	
	short StartCameraExposure ( CameraPtr camera, short wait )

	(camera): pointer to camera record.
	(wait):   if TRUE, wait for exposure to complete before returning.
	
	The function returns TRUE if it can start the exposure successfully,
    or FALSE if it fails.

	A complete exposure contains two phases: integration and download.
	Integration is the first phase, where the camera's imaging sensor
	device accumulates a signal.  Download is the second phase, where the
	accumulated signal is read out from the camera.  The exposure can be
	ended during or after either phase by calling EndCameraExposure().
	The camera's current exposure phase can be determined by calling
	GetCameraExposureStatus().  It will be one of the status codes
	#defined in "SkySight.h".
	
    The camera's current status code, as returned by GetCameraExposureStatus(),
    must be CAMERA_STATUS_WAITING when this function is called; otherwise,
    the function will fail.
    
    The exposure length and shutter state are taken from the values most
    recently passed to SetCameraExposureLength() and SetCameraExposureType().

	To end the exposure, call either the function StartCameraExposureDownload()
	or EndCameraExposure().
	
	- If you intend to download the results of the exposure, you should
	  call StartCameraDownload().
	  
	- If you intend to discard the results of the exposure without downloading
	  them, you should pass EndCameraExposure().
	
    Please note that EndCameraExposure() can be called to end the exposure
    before the total exposure time has elapsed; this is a normal, acceptable
    usage.  However, if there is an exposure in progress, you should not start
    a new exposure without ending the one in progress first!

*******************************************************************************/

short StartCameraExposure ( CameraPtr camera, short wait )
{
	short		result = FALSE;

	/*** If an exposure or download is already in progress,
	     return a failure code. ***/
	
	if ( camera->cameraStatus != CAMERA_STATUS_WAITING )
		return ( FALSE );
	
	/*** Now tell the camera to start the exposure, and return right away,
	     without waiting for the exposure to complete (i.e. asynchronously). ***/
	
	camera->cameraInterfaceData.cameraSynchronous = wait;
			
	result = (*camera->cameraInterfaceProc) ( CAMERA_START_EXPOSURE, &camera->cameraInterfaceData );

    /*** If we successfully started an exposure, record the exposure
         start time, and change the camera's exposure status code.
         Then return the result code which indicates whether we succeeded
         or not. ***/

	if ( result == TRUE )
	{
		camera->cameraStatus = CAMERA_STATUS_EXPOSING;
		camera->cameraExposureStartTime = GGetTickCount();
    }
    	
    return ( result );
}

/*** StartCameraDownload *******************************************************

	Ends an exposure integration and prepares the camera for image download.
	
	short StartCameraDownload ( CameraPtr camera )

	(camera): pointer to camera record.
	
	The function returns TRUE if it can end the exposure successfully,
    or FALSE if it fails.

    Before calling this function, you must have called the function
    StartCameraExposure().  If the camera exposure status code,
    as returned by GetCameraStatus(), is not CAMERA_STATUS_EXPOSING,
    this function will fail.
    
    To download individual rows of image data from the camera, call
    DownloadCameraImageRow() after calling this function.  When you are
    finished downloading image data from the camera, call EndCameraDownload()
    to return the camera to its idle state.
    
	If you intend to discard the results of the exposure without downloading
	them, you should call EndCameraExposure() without calling this function.
	
    Please note that function can be called to end the exposure before the
    total exposure time has elapsed; this is a normal, acceptable usage.
    However, if there is an exposure in progress, you should not start a new
    exposure without ending the one in progress first!

*******************************************************************************/

short StartCameraDownload ( CameraPtr camera )
{
	short		result = FALSE;
	
	/*** If an exposure is not in progress, return a failure code. ***/
	
	if ( camera->cameraStatus != CAMERA_STATUS_EXPOSING )
		return ( FALSE );

	/*** Tell the camera to end the exposure and prepare it for
	     download. ***/
	     
	result = (*camera->cameraInterfaceProc) ( CAMERA_START_DOWNLOAD, &camera->cameraInterfaceData );
	
    /*** If we were able to end the exposure successfully, reset the
         exposure start time counter and row download counter, and set
         the exposure status code to "downloading". ***/
         
    if ( result == TRUE )
	{
		camera->cameraStatus = CAMERA_STATUS_DOWNLOADING;
        camera->cameraExposureStartTime = 0;
        camera->cameraDownloadRow = 0;
	}
	
	return ( result );
}

/*** EndCameraExposure ******************************************************

	Ends a camera exposure and returns the camera to its idle state.
	
	short EndCameraExposure ( CameraPtr camera )

	(camera): pointer to camera whose exposure you wish to end.
	
	The function returns TRUE if it can end the exposure successfully,
	or FALSE if it fails.

	This function will cancel the exposure in progress and return the camera
	to its idle, waiting state.  You will not be able to download any image
	data from the camera.  To end an exposure and start downloading image
	data, call the function StartCameraDownload().
	
	This function must be called while an exposure is in progress (i.e. after
	a call to StartCameraExposure()); otherwise it will fail.  You can call
	GetCameraStatus() to determine if a download is in progress.
	  
	Please note that you MUST call either this function or StartCameraDownload()
	at some point after calling StartCameraExposure(); otherwise you will not
	be able to begin a new exposure!

*******************************************************************************/

short EndCameraExposure ( CameraPtr camera )
{
	short result = FALSE;
	
	/*** If an exposure is not already in progress, fail. ***/
	
	if ( camera->cameraStatus != CAMERA_STATUS_EXPOSING )
		return ( FALSE );

	/*** Cancel the exposure, and return the camera to its idle state. ***/
	     
	result = (*camera->cameraInterfaceProc) ( CAMERA_END_EXPOSURE, &camera->cameraInterfaceData );
	
    /*** If we were able to end the exposure successfully, reset the exposure
         start time and row download counters, and change the exposure status
         code to "waiting". ***/

	if ( result == TRUE )
	{
		camera->cameraStatus = CAMERA_STATUS_WAITING;
        camera->cameraExposureStartTime = 0;
        camera->cameraDownloadRow = 0;
	}
	
	return ( result );
}

/*** EndCameraDownload ******************************************************

	Ends a camera download and returns the camera to its idle state.
	
	short EndCameraDownload ( CameraPtr camera )

	(camera): pointer to camera whose download you wish to end.
	
	The function returns TRUE if it can end the download, or FALSE
	if it fails.

	This function will end the camera download and return the camera to its
	idle, waiting state.  You will not be able to read out any additional data
	from the camera until you begin a new exposure.
	
	This function must be called while a download is in progress (i.e. after
	StartCameraDownload() has been called); otherwise it will fail.  You
	can call GetCameraStatus() to determine if a download is in progress.
	
	Please note that you MUST call this function at some point after
	calling StartCameraExposure() or StartCameraDownload(); otherwise
	you will not be able to begin a new exposure!

*******************************************************************************/

short EndCameraDownload ( CameraPtr camera )
{
	short result = FALSE;
	
	/*** If a download is not already in progress, fail. ***/
	
	if ( camera->cameraStatus != CAMERA_STATUS_DOWNLOADING )
		return ( FALSE );

	/*** End the download, and return the camera to its idle state. ***/
	     
	result = (*camera->cameraInterfaceProc) ( CAMERA_END_DOWNLOAD, &camera->cameraInterfaceData );
	
    /*** If we were able to end the download successfully, reset the exposure
         start time and row download counters, and change the
         exposure status code to "waiting". ***/

	if ( result == TRUE )
	{
		camera->cameraStatus = CAMERA_STATUS_WAITING;
        camera->cameraExposureStartTime = 0;
        camera->cameraDownloadRow = 0;
	}
	
	return ( result );
}

/*** DiscardCameraImageRows **************************************************

	Skips a number of rows of image data from the camera.

	short DiscardCameraImageRows ( CameraPtr camera, short rows )

	(camera): pointer to camera record.
    (rows):   number of rows of image data to discard.

	The function returns TRUE if can complete the command successfully,
    or FALSE if it fails.

	You must call StartCameraDownload() before calling this function;
	otherwise this function will fail.
	
    The function will instruct the camera to discard a number of image rows
    equal to the (rows) parameter.  Please note that these are binned image
    rows, not physical camera rows, e.g. if the camera's vertical binning
    factor is 2, then each call of this function will discard two rows of
    physical pixel data from the camera.

*********************************************************************************/

short DiscardCameraImageRows ( CameraPtr camera, short rows )
{
	short					result = FALSE;
	unsigned short			*buffer = NULL;

	/*** If we haven't yet begun downloading image data, fail. ***/
	
	if ( camera->cameraStatus != CAMERA_STATUS_DOWNLOADING )
		return ( FALSE );
		
	/*** Tell the camera to discard the specified number of image rows. ***/
	
	camera->cameraInterfaceData.cameraDownloadDiscard = rows;
		
	result = (*camera->cameraInterfaceProc) ( CAMERA_DISCARD_ROWS, &camera->cameraInterfaceData );

	/*** If we succeeded, add the number of discarded rows to the current
	     download row number. ***/
	     
	if ( result == TRUE )
		camera->cameraDownloadRow += rows;
		     
    return ( result );
}

/*** DownloadCameraImageRow ****************************************************

	Reads out all or a portion of one row of image data from the camera.

	int DownloadCameraImageRow ( CameraPtr camera, short left, short right, PIXEL *data )

	(camera): pointer to camera record.
    (left):   column from which to start reading data.
    (right):  column at which to finish reading data.
    (data):   buffer to receive data, or NULL to discard.

	The function returns TRUE if can complete the command successfully, or FALSE
	if it fails.

    All of the contents of the image data buffer from data[left] up to and
    including data[right] will be overwritten with new values obtained from
    the camera.  Please note that both (left) and (right) are counted from zero;
    hence data[0] is the leftmost pixel in the row, and data[ image width - 1 ]
    is the rightmost pixel in the row.

	Also note that these are binned image pixels, not physical camera pixels,
	e.g. if the camera's horizontal binning factor is 2, then each image pixel
	corresponds to two physical camera pixels.

***********************************************************************************/

short DownloadCameraImageRow ( CameraPtr camera, short left, short right, PIXEL *data )
{
	short					width, col, result = FALSE;
	unsigned short			*buffer = NULL;

	/*** If we haven't yet begun downloading image data, fail. ***/
	
	if ( camera->cameraStatus != CAMERA_STATUS_DOWNLOADING )
		return ( FALSE );

	/*** Allocate a temporary buffer of short integers into which
	     data will be copied from the camera driver.  If we can't,
	     return a memory allocation error code. ***/
	     
    width = right - left + 1;
	buffer = (unsigned short *) malloc ( sizeof ( unsigned short ) * width );
	if ( buffer == NULL )
		return ( FALSE );

	/*** Now download one row of image data from the camera. ***/

	camera->cameraInterfaceData.cameraDownloadStart = left;
	camera->cameraInterfaceData.cameraDownloadWidth = width;
	camera->cameraInterfaceData.cameraDownloadBuffer = buffer;
	
	result = (*camera->cameraInterfaceProc) ( CAMERA_DOWNLOAD_ROW, &camera->cameraInterfaceData );

	/*** If we successfully downloaded a row of image data from the camera,
	     copy data from the buffer into the corresponding FITS image matrix
	     row, then update the row download counter. ***/
	     
	if ( result == TRUE )
	{
    	for ( col = left; col <= right; col++ )
    		data[col] = buffer[ col - left ];
		
		camera->cameraDownloadRow++;
    }

	/*** Finally, free the buffer and return the result code. ***/
	
	free ( buffer );
    return ( result );
}
