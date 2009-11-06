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

/*** Header files ***/

#include "GUILib.h"
#include "ParLib.h"
#include "Cookbook.h"
#include "CameraInterface.h"

/*** local functions ***/

static short	OpenCameraConnection ( CameraInterfaceDataPtr );
static short	CloseCameraConnection ( CameraInterfaceDataPtr );
static short	GetCameraTECInfo ( CameraInterfaceDataPtr );
static short	GetCameraFilterInfo ( CameraInterfaceDataPtr );
static short	GetCameraReadoutInfo ( CameraInterfaceDataPtr );
static short	GetCameraAntibloomingInfo ( CameraInterfaceDataPtr );
static short	SetCameraTemperature ( CameraInterfaceDataPtr );
static short	GetCameraTemperature ( CameraInterfaceDataPtr );
static short	SetCameraFilter ( CameraInterfaceDataPtr );
static short	GetCameraFilter ( CameraInterfaceDataPtr );
static short	SetCameraBinning ( CameraInterfaceDataPtr );
static short	StartCameraExposure ( CameraInterfaceDataPtr );
static short	StartCameraDownload ( CameraInterfaceDataPtr );
static short	EndCameraExposure ( CameraInterfaceDataPtr );
static short	EndCameraDownload ( CameraInterfaceDataPtr );
static short	DownloadCameraRow ( CameraInterfaceDataPtr );
static short	DiscardCameraRows ( CameraInterfaceDataPtr );

/*** CookbookCameraInterface ***/

short CookbookCameraInterface ( short function, CameraInterfaceDataPtr data )
{
	short	result = FALSE;
	
	switch ( function )
	{
		case CAMERA_OPEN_CONNECTION:
			result = OpenCameraConnection ( data );
			break;
			
		case CAMERA_CLOSE_CONNECTION:
			result = CloseCameraConnection ( data );
			break;
			
		case CAMERA_GET_READOUT_INFO:
			result = GetCameraReadoutInfo ( data );
			break;
		
		case CAMERA_GET_TEC_INFO:
			result = GetCameraTECInfo ( data );
			break;

		case CAMERA_GET_ANTIBLOOMING_INFO:
			result = GetCameraAntibloomingInfo ( data );
			break;
			
		case CAMERA_GET_FILTER_INFO:
			result = GetCameraFilterInfo ( data );
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

/*** OpenCameraConnection ********************************************/

short OpenCameraConnection ( CameraInterfaceDataPtr data )
{
	short			result;

	result = OpenCookbookDriver ( GetParallelPortAddress ( data->cameraPort - LPT0 ) );

    if ( result == TRUE )
    {
    	data->cameraReadoutMode = 1;
    	
    	result = GetCameraReadoutInfo ( data );
	}
	
	return ( result );
}

/*** CloseCameraConnection ******************************************/

short CloseCameraConnection ( CameraInterfaceDataPtr data )
{
	CloseCookbookDriver();
    
    return ( TRUE );
}

/*** GetCameraReadoutInfo *********************************************/

short GetCameraReadoutInfo ( CameraInterfaceDataPtr data )
{
    short width, height, hbin, vbin, result;

	if( ( result = SetCookbookReadoutMode ( data->cameraReadoutMode ) ) != FALSE )
	{
			
		result = GetCookbookReadoutMode ( &width,
										  &height,
										  &hbin,
										  &vbin );
			
		data->cameraImageWidth = width;
		data->cameraImageHeight = height;
		
		sprintf ( data->cameraDescription, "%hd x %hd", width, height );
	}
	
    return ( result );
}

/*** GetCameraAntibloomingInfo ****************************************/

short GetCameraAntibloomingInfo ( CameraInterfaceDataPtr data )
{
	short result = TRUE;
	
	if ( data->cameraAntibloomingState == 1 )
		strcpy ( data->cameraDescription, "None" );
	else
		result = FALSE;
	
	return ( result );
}

/*** GetCameraTECInfo *************************************************/

short GetCameraTECInfo ( CameraInterfaceDataPtr data )
{
	short result = TRUE;
	
	data->cameraTECType = TEC_ABSENT;
	
	return ( result );
}

/*** GetCameraTemperature *******************************************/

short GetCameraTemperature ( CameraInterfaceDataPtr data )
{
	short result = TRUE;
	
	return ( result );
}

/*** SetCameraTemperature ********************************************/

short SetCameraTemperature ( CameraInterfaceDataPtr data )
{
	short result = TRUE;
	
	return ( result );
}

/*** GetCameraFilterInfo *************************************************/

short GetCameraFilterInfo ( CameraInterfaceDataPtr data )
{
	short result = TRUE;
	
	if ( data->cameraFilterPosition == 1 )
	{
		data->cameraFilterColor = FILTER_CLEAR;
		strcpy ( data->cameraDescription, "None" );
	}
	else
    {
    	result = FALSE;
	}
	
	return ( result );
}

/*** GetCameraFilter *********************************************************/

short GetCameraFilter ( CameraInterfaceDataPtr data )
{
	data->cameraFilterPosition = 1;
	
	return ( TRUE );
}

/*** SetCameraFilter *********************************************************/

short SetCameraFilter ( CameraInterfaceDataPtr data )
{
	if ( data->cameraFilterPosition == 1 )
		return ( TRUE );
	else
    	return ( FALSE );
}

/*** StartCameraExposure ********************************************/

short StartCameraExposure ( CameraInterfaceDataPtr data )
{
	int		result = TRUE;

    result = StartCookbookExposure ( data->cameraShutter == SHUTTER_OPEN );
    	
    return ( result );
}

/*** EndCameraExposure *********************************************/

short EndCameraExposure ( CameraInterfaceDataPtr data )
{
	int result;
	
	result = EndCookbookExposure();

	return ( result );
}

/*** StartCameraDownload *********************************************/

short StartCameraDownload ( CameraInterfaceDataPtr data )
{
	int		result;
	
	result = EndCookbookExposure();
	
	return ( result );
}

/*** EndCameraDownload ************************************************/

short EndCameraDownload ( CameraInterfaceDataPtr data )
{
	return ( TRUE );
}

/*** DiscardCameraRows ***********************************************/

short DiscardCameraRows ( CameraInterfaceDataPtr data )
{
	int		result = TRUE;

 	// THIS WILL FAIL.
	     
    return ( result );
}

/***  DownloadCameraRow  ***********************************************/

short DownloadCameraRow ( CameraInterfaceDataPtr data )
{
	int		result;
 	
	result = GetCookbookImageDataRow ( data->cameraDownloadBuffer );
	     
    return ( result );
}
