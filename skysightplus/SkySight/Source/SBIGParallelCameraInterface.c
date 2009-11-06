/*** COPYRIGHT NOTICE AND PUBLIC SOURCE LICENSE *********************************

Portions Copyright (c) 1992-2001 Southern Stars Systems.  All Rights Reserved.

This file contains Original Code and/or Modifications of Original Code as defined
in and that are subject to the Southern Stars Systems Public Source License
Version 1.0 (the 'License').  You may not use this file except in compliance with
the License.  Please obtain a copy of the License at

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

1.0.0 - 09 Apr 2001 - TCD - Original version.

*********************************************************************************/

#include <math.h>
#include "GUILib.h"
#include "ParLib.h"
#include "ParDrv.h"
#include "CameraInterface.h"

/*** constants and macros ***/

#define T_0				25.0
#define R_0				3.0
#define MAX_AD			4096
#define R_RATIO_CCD		2.57
#define R_BRIDGE_CCD	10.0
#define DT_CCD			25.0
#define R_RATIO_AMB		7.791
#define R_BRIDGE_AMB	3.0
#define DT_AMB			45.0

/*** local functions ***/

static short	GetCameraTECInfo ( CameraInterfaceDataPtr );
static short	GetCameraFilterInfo ( CameraInterfaceDataPtr );
static short	GetCameraShutterInfo ( CameraInterfaceDataPtr );
static short	GetCameraAntibloomingInfo ( CameraInterfaceDataPtr );
static short	GetCameraReadoutInfo ( CameraInterfaceDataPtr );

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

static short			SetCFWFilter ( CameraInterfaceDataPtr );
static short			GetCFWFilter ( CameraInterfaceDataPtr );
static short			SetPixCelFilter ( CameraInterfaceDataPtr );
static short			GetPixCelFilter ( CameraInterfaceDataPtr );
static short			SetPixCellShutter ( CameraInterfaceDataPtr, short );
static double			TemperatureToSetpoint ( double );
static double			SetpointToTemperature ( double, double, double, double );
static CCD_REQUEST		RequestedCCD ( short );
static unsigned short	ReadoutMode ( short );

/*** local data ***/

#ifdef GMAC

char			parDrvCableDelay = 0;
Handle			relayClickH = NULL;

#endif

/*** SBIGParallelCameraInterface ***/

short SBIGParallelCameraInterface ( short function, CameraInterfaceDataPtr data )
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
		
		case CAMERA_GET_SHUTTER_INFO:
			result = GetCameraShutterInfo ( data );
			break;
			
		case CAMERA_GET_TEC_INFO:
			result = GetCameraTECInfo ( data );
			break;
				
		case CAMERA_GET_FILTER_INFO:
			result = GetCameraFilterInfo ( data );
			break;
			
		case CAMERA_GET_ANTIBLOOMING_INFO:
			result = GetCameraAntibloomingInfo ( data );
			break;

		case CAMERA_GET_READOUT_INFO:
			result = GetCameraReadoutInfo ( data );
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

/*** OpenCameraConnection ******************************************************/

short OpenCameraConnection ( CameraInterfaceDataPtr data )
{
	short					result;
	EstablishLinkParams		establishLinkParams;
    EstablishLinkResults	establishLinkResults;
	GetCCDInfoParams		getCCDInfoParams;
	GetCCDInfoResults0		*getCCDInfoResults;
	
	/*** Open the SBIG parallel driver.  On failure, return an error code. ***/
	
	result = ParDrvCommand ( CC_OPEN_DRIVER, NULL, NULL );
	if ( result != CE_NO_ERROR )
		return ( FALSE );
	
	/*** Establish a communications link with the camera.  If we can't,
	     close the driver and return an error code. ***/
		
	establishLinkParams.port = BASE_ADDR;
	establishLinkParams.baseAddress = GetParallelPortAddress ( data->cameraPort - LPT0 );
	
	result = ParDrvCommand ( CC_ESTABLISH_LINK, &establishLinkParams, &establishLinkResults );
	if ( result != CE_NO_ERROR )
	{
		ParDrvCommand ( CC_CLOSE_DRIVER, NULL, NULL );
		return ( FALSE );
	}
	
	/*** Allocate memory for a SBIG CCD Info Type 0 Results record.  If we fail,
	     close the driver and return an error code. ***/
			     
	getCCDInfoResults = (GetCCDInfoResults0 *) malloc ( sizeof ( GetCCDInfoResults0 ) );
	if ( getCCDInfoResults == NULL )
	{
		ParDrvCommand ( CC_CLOSE_DRIVER, NULL, NULL );
		return ( FALSE );
	}
	
	/*** Get the CCD information from the SBIG driver.  If we fail, free memory
	     for the CCD information record, close the driver, and return an error code. ***/
	     
	getCCDInfoParams.request = RequestedCCD ( data->cameraType );
	result = ParDrvCommand ( CC_GET_CCD_INFO, &getCCDInfoParams, getCCDInfoResults );
    if ( result != CE_NO_ERROR )
    {
    	free ( getCCDInfoResults );
    	ParDrvCommand ( CC_CLOSE_DRIVER, NULL, NULL );
    	return ( FALSE );
    }
    
    /*** Store a pointer to the CCD information record in the camera interface
         data's private data pointer, and return a successful result code. ***/
         
	data->cameraPrivateData = getCCDInfoResults;
	return ( TRUE );
}

/*** CloseCameraConnection ******************************************************/

short CloseCameraConnection ( CameraInterfaceDataPtr data )
{
	/*** Free memory for the SBIG CCD Info record we allocated when we opened
	     the camera connection, the close the driver and return a successful
	     result code. ***/
	
	free ( data->cameraPrivateData );
	ParDrvCommand ( CC_CLOSE_DRIVER, NULL, NULL );
    
    return ( TRUE );
}

/*** GetCameraTECInfo ***************************************************/

short GetCameraTECInfo ( CameraInterfaceDataPtr data )
{
	short	result = TRUE;

	data->cameraTECType = TEC_PRESENT;
	
    return ( result );
}

/*** GetCameraFilterInfo ************************************************/

short GetCameraFilterInfo ( CameraInterfaceDataPtr data )
{
	short					result = TRUE;
    GetCCDInfoResults0		*getCCDInfoResults = (GetCCDInfoResults0 *) data->cameraPrivateData;
	
	/*** If the camera has no filter, return an error code. ***/
	
	if ( data->cameraFilterType == FILTER_TYPE_NONE )
	{
		result = FALSE;
	}
	
	/*** If the camera has a PixCel 2-position vane, return "clear"
	     or "dark" for the two filter positions. ***/
	     
	else if ( data->cameraFilterType == FILTER_TYPE_PIXCEL_VANE )
	{
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
	}
	
	/*** For all other filters, return clear, dark, red, green,
	     and blue positions. ***/
	     
	else
	{	
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
		{
			result = FALSE;
		}
	}
		
    return ( result );
}

/*** GetCameraShutterInfo ***********************************************/

short GetCameraShutterInfo ( CameraInterfaceDataPtr data )
{
	short					result = TRUE;
    GetCCDInfoResults0		*getCCDInfoResults;
    
    getCCDInfoResults = (GetCCDInfoResults0 *) data->cameraPrivateData;

	if ( getCCDInfoResults->cameraType == ST5C_CAMERA )
		data->cameraShutterType = SHUTTER_TYPE_ELECTRONIC;
	else if ( getCCDInfoResults->cameraType == ST237_CAMERA	)
		data->cameraShutterType = SHUTTER_TYPE_ELECTRONIC;
	else
		data->cameraShutterType = SHUTTER_TYPE_PHYSICAL;

    return ( result );
}

/*** GetCameraAntibloomingInfo ********************************************/

short GetCameraAntibloomingInfo ( CameraInterfaceDataPtr data )
{
	short					result = TRUE;
    
	if ( data->cameraAntibloomingState == 1 )
		strcpy ( data->cameraDescription, "High" );
	else if ( data->cameraAntibloomingState == 2 )
		strcpy ( data->cameraDescription, "Medium" );
	else if ( data->cameraAntibloomingState == 3 )
		strcpy ( data->cameraDescription, "Low" );
	else if ( data->cameraAntibloomingState == 4 )
		strcpy ( data->cameraDescription, "Off" );
	else
		result = FALSE;
		
    return ( result );
}

/*** GetCameraReadoutInfo *************************************************/

short GetCameraReadoutInfo ( CameraInterfaceDataPtr data )
{
	short					result = FALSE, i;
    GetCCDInfoResults0		*getCCDInfoResults;
    
    getCCDInfoResults = (GetCCDInfoResults0 *) data->cameraPrivateData;
	i = data->cameraReadoutMode - 1;

	if ( i >= 0 && i < getCCDInfoResults->readoutModes )
	{
		/*** The ST-7/8 support variable-width binning modes, and off-chip
		     binning modes, which we don't want to present to the user.
		     The only binning modes we'll support are the on-chip 1x1, 2x2,
		     and 3x3 binning modes. ***/
	
		if ( getCCDInfoResults->readoutInfo[i].mode > 2 )
		{
			result = FALSE;
		}
		else
		{
			data->cameraImageWidth = getCCDInfoResults->readoutInfo[i].width;
			data->cameraImageHeight = getCCDInfoResults->readoutInfo[i].height;
		
			sprintf ( data->cameraDescription, "%hd x %hd",
			data->cameraImageWidth, data->cameraImageHeight );
		
			result = TRUE;
		}
	}
	
    return ( result );
}

/*** SetCameraTemperature ******************************************************/

short SetCameraTemperature ( CameraInterfaceDataPtr data )
{
	short							result;
	SetTemperatureRegulationParams	setTemperatureRegulationParams;

	if ( data->cameraTECState == TEC_ON )
		setTemperatureRegulationParams.regulation = REGULATION_ON;
	else
		setTemperatureRegulationParams.regulation = REGULATION_OFF;
		
	setTemperatureRegulationParams.ccdSetpoint = TemperatureToSetpoint ( data->cameraTemperatureSetpoint );

	result = ParDrvCommand ( CC_SET_TEMPERATURE_REGULATION, &setTemperatureRegulationParams, NULL );
	if ( result == CE_NO_ERROR )
		result = TRUE;
    else
		result = FALSE;
	
    return ( result );
}

/*** GetCameraTemperature ********************************************************/

short GetCameraTemperature ( CameraInterfaceDataPtr data )
{
	short							result;
	QueryTemperatureStatusResults	queryTemperatureStatusResults;

	result = ParDrvCommand ( CC_QUERY_TEMPERATURE_STATUS, NULL, &queryTemperatureStatusResults );

 	if ( result == CE_NO_ERROR )
	{
		data->cameraTemperatureImager = SetpointToTemperature ( queryTemperatureStatusResults.ccdThermistor, R_RATIO_CCD, R_BRIDGE_CCD, DT_CCD );
		data->cameraTemperatureAmbient = SetpointToTemperature ( queryTemperatureStatusResults.ambientThermistor, R_RATIO_AMB, R_BRIDGE_AMB, DT_AMB );
    	data->cameraTECPowerUse = queryTemperatureStatusResults.power / 255.0;

        result = TRUE;
    }
    else
	{
    	result = FALSE;
    }
	
    return ( result );
}

/*** SetCameraFilter **********************************************************/

short SetCameraFilter ( CameraInterfaceDataPtr data )
{
	short		result;
	
	if ( data->cameraFilterType == FILTER_TYPE_NONE )
		result = FALSE;
	else if ( data->cameraFilterType >= FILTER_TYPE_PIXCEL_VANE && data->cameraFilterType <= FILTER_TYPE_PIXCEL_COLOR )
		result = SetPixCelFilter ( data );
	else if ( data->cameraFilterType == FILTER_TYPE_SBIG_CFW_6A )
		result = SetCFWFilter ( data );
	else if ( data->cameraFilterType == FILTER_TYPE_SBIG_CFW_8 )
		result = SetCFWFilter ( data );
	else
		result = FALSE;
		
	return ( result );
}

/*** GetCameraFilter **********************************************************/

short GetCameraFilter ( CameraInterfaceDataPtr data )
{
	short	result;
	
	if ( data->cameraFilterType == FILTER_TYPE_PIXCEL_VANE || data->cameraFilterType == FILTER_TYPE_PIXCEL_COLOR )
	{
		result = GetPixCelFilter ( data );
	}
	else if ( data->cameraFilterType == FILTER_TYPE_SBIG_CFW_6A || data->cameraFilterType == FILTER_TYPE_SBIG_CFW_8 )
	{
		result = GetCFWFilter ( data );
	}
	else
	{
		result = FALSE;
	}

	return ( result );
}

/*** StartCameraExposure **********************************************************/

short StartCameraExposure ( CameraInterfaceDataPtr data )
{
	int						result = TRUE;
	StartExposureParams		startExposureParams;

//	SetPixCellShutter ( data, data->cameraShutter );
	
	startExposureParams.ccd = RequestedCCD ( data->cameraType );
    startExposureParams.exposureTime = data->cameraExposureLength * 100;
    
    if ( data->cameraAntibloomingState == 4 )
		startExposureParams.abgState = ABG_LOW7;
    else if ( data->cameraAntibloomingState == 3 )
		startExposureParams.abgState = ABG_CLK_LOW7;
    else if ( data->cameraAntibloomingState == 2 )
		startExposureParams.abgState = ABG_CLK_MED7;
    else if ( data->cameraAntibloomingState == 1 )
		startExposureParams.abgState = ABG_CLK_HI7;
	else
		startExposureParams.abgState = ABG_LOW7;
	
	if ( data->cameraShutter == SHUTTER_OPEN )
    	startExposureParams.openShutter = SC_OPEN_SHUTTER;
	else
    	startExposureParams.openShutter = SC_CLOSE_SHUTTER;
	
	result = ParDrvCommand ( CC_START_EXPOSURE, &startExposureParams, NULL );
	if ( result == CE_NO_ERROR )
        result = TRUE;
    else
    	result = FALSE;
    
	if ( result == TRUE && data->cameraSynchronous == TRUE )
		delay ( (unsigned long) ( 1000 * data->cameraExposureLength ) );
            		
    return ( result );
}

/*** StartCameraDownload *****************************************/

short StartCameraDownload ( CameraInterfaceDataPtr data )
{
	int						result;
	EndExposureParams		endExposureParams;

	endExposureParams.ccd = RequestedCCD ( data->cameraType );

	result = ParDrvCommand ( CC_END_EXPOSURE, &endExposureParams, NULL );

    if ( result == CE_NO_ERROR )
    	result = TRUE;
    else
    	result = FALSE;
	
	return ( result );
}

/*** EndCameraExposure ********************************************/

short EndCameraExposure ( CameraInterfaceDataPtr data )
{
	int						result;
	EndExposureParams		endExposureParams;

	endExposureParams.ccd = RequestedCCD ( data->cameraType );

	result = ParDrvCommand ( CC_END_EXPOSURE, &endExposureParams, NULL );
	
    if ( result == CE_NO_ERROR )
	{
		EndReadoutParams		endReadoutParams;

		endReadoutParams.ccd = 0;
        
		result = ParDrvCommand ( CC_END_READOUT, &endReadoutParams, NULL );
	}

//	SetPixCellShutter ( data, SHUTTER_OPEN );
	
	if ( result == CE_NO_ERROR )
        result = TRUE;
    else
    	result = FALSE;

	return ( result );
}

/*** EndCameraDownload *****************************************************/

short EndCameraDownload ( CameraInterfaceDataPtr data )
{	
	short					result;
	EndReadoutParams		endReadoutParams;

	endReadoutParams.ccd = RequestedCCD ( data->cameraType );
    
	result = ParDrvCommand ( CC_END_READOUT, &endReadoutParams, NULL );
	if ( result == CE_NO_ERROR )
    {
    	result = ParDrvCommand ( CC_UPDATE_CLOCK, NULL, NULL );
    }
    
//	SetPixCellShutter ( data, SHUTTER_OPEN );

	if ( result == CE_NO_ERROR )
        result = TRUE;
    else
    	result = FALSE;
	
	return ( result );
}

/*** DiscardCameraRows ***************************************************/

short DiscardCameraRows ( CameraInterfaceDataPtr data )
{
	short					result, i;
    DumpLinesParams			dumpLinesParams;
    GetCCDInfoResults0		*getCCDInfoResults;

    getCCDInfoResults = (GetCCDInfoResults0 *) data->cameraPrivateData;
	i = data->cameraReadoutMode - 1;

	dumpLinesParams.ccd = RequestedCCD ( data->cameraType );
    dumpLinesParams.readoutMode = getCCDInfoResults->readoutInfo[i].mode;
    dumpLinesParams.lineLength = data->cameraDownloadDiscard;

	result = ParDrvCommand ( CC_DUMP_LINES, &dumpLinesParams, NULL );
    if ( result == CE_NO_ERROR )
    	result = TRUE;
    else
    	result = FALSE;
	     
    return ( result );
}

/*** DownloadCameraRow ***************************************************/

short DownloadCameraRow ( CameraInterfaceDataPtr data )
{
	short					result, i;
    ReadoutLineParams		readoutLineParams;
    GetCCDInfoResults0		*getCCDInfoResults;

    getCCDInfoResults = (GetCCDInfoResults0 *) data->cameraPrivateData;
	i = data->cameraReadoutMode - 1;

	readoutLineParams.ccd = RequestedCCD ( data->cameraType );
	readoutLineParams.readoutMode = getCCDInfoResults->readoutInfo[i].mode;
	readoutLineParams.pixelStart = data->cameraDownloadStart;
	readoutLineParams.pixelLength = data->cameraDownloadWidth;

	result = ParDrvCommand ( CC_READOUT_LINE, &readoutLineParams, data->cameraDownloadBuffer );
    if ( result == CE_NO_ERROR )
        result = TRUE;
	else
    	result = FALSE;
	     
    return ( result );
}

/*** SetPixCelFilter *********************************************************/

short SetPixCelFilter ( CameraInterfaceDataPtr data )
{
	short			result;
	PulseOutParams	params;
	
	params.numberPulses = 1;
	params.pulseWidth = 0;
	
	if ( data->cameraFilterPosition == 1 ) /* clear */
		params.pulsePeriod = 1;
	else if ( data->cameraFilterPosition == 2 ) /* dark */
		params.pulsePeriod = 2;
	else if ( data->cameraFilterPosition == 3 ) /* red */
		params.pulsePeriod = 3;
	else if ( data->cameraFilterPosition == 4 ) /* green */
		params.pulsePeriod = 4;
	else if ( data->cameraFilterPosition == 5 ) /* blue */
		params.pulsePeriod = 5;
	else
		params.pulsePeriod = 0;
			
	result = ParDrvCommand ( CC_PULSE_OUT, &params, NULL );
	if ( result == CE_NO_ERROR && data->cameraSynchronous )
	{
		QueryCommandStatusParams	query;
		QueryCommandStatusResults	results;
		
		query.command = CC_PULSE_OUT;
		
		while ( TRUE )
		{
			result = ParDrvCommand ( CC_QUERY_COMMAND_STATUS, &query, &results );
			if ( result != CE_NO_ERROR )
				break;
				
			if ( ( results.status >> 1 ) != 0 )
				break;
		}		
	}
	
	if ( result == CE_NO_ERROR )
		result = TRUE;
	else
		result = FALSE;
	
	return ( result );
}

/*** GetPixCelFilter **********************************************************/

short GetPixCelFilter ( CameraInterfaceDataPtr data )
{
	int		result = TRUE;
	short	filter;
	
	QueryCommandStatusParams	query;
	QueryCommandStatusResults	results;
	
	query.command = CC_PULSE_OUT;
	
	result = ParDrvCommand ( CC_QUERY_COMMAND_STATUS, &query, &results );
	if ( result == CE_NO_ERROR )
	{
		filter = ( results.status & 0x000F ) >> 1;

		if ( filter > 0 && filter < 6 )
		{
			data->cameraFilterPosition = filter;
			result = TRUE;
		}
		else
		{
			result = FALSE;
		}
	}
	else
	{
		result = FALSE;
	}

	return ( result );
}

/*** SetCFWFilter *****************************************************************/

short SetCFWFilter ( CameraInterfaceDataPtr data )
{
	short			result;
	PulseOutParams	params;
	
	params.numberPulses = 60;
    params.pulsePeriod = 42000;

    if ( data->cameraFilterPosition == 1 ) /* clear */
    	params.pulseWidth = 3000;
    else if ( data->cameraFilterPosition == 2 ) /* dark */
    	params.pulseWidth = 3600;
    else if ( data->cameraFilterPosition == 3 ) /* red */
    	params.pulseWidth = 1200;
    else if ( data->cameraFilterPosition == 4 ) /* green */
    	params.pulseWidth = 1800;
    else if ( data->cameraFilterPosition == 5 ) /* blue */
    	params.pulseWidth = 2400;
	
	if ( data->cameraFilterType == FILTER_TYPE_SBIG_CFW_8 )
	{
		params.pulsePeriod *= 0.435;
		params.pulseWidth *= 0.435;
	}
	
	result = ParDrvCommand ( CC_PULSE_OUT, &params, NULL );
	if ( data->cameraSynchronous && result == CE_NO_ERROR )
	{
		QueryCommandStatusParams	query;
		QueryCommandStatusResults	results;
		
		query.command = CC_PULSE_OUT;
		
		while ( TRUE )
		{
			result = ParDrvCommand ( CC_QUERY_COMMAND_STATUS, &query, &results );
			if ( result == CE_NO_ERROR )
	            if ( ( results.status & 0x0001 ) == 0x0000 )
    	        	break;
		}		
	}
	
	if ( result == CE_NO_ERROR )
		result = TRUE;
	else
		result = FALSE;
	
	return ( result );
}

/*** GetCFWFilter *****************************************************************/

short GetCFWFilter ( CameraInterfaceDataPtr data )
{
	/*** there does not seem to be any way of returning this data. ***/
	
	return ( TRUE );
}

/*** SetPixCellShutter *********************************************************

	Positions the internal color filter wheel of the PixCell 255 & 237 (and
	their SBIG equivalents) to simulate the shutter state, since those cameras
	have no shutters.

	void SetPixCellShutter ( CameraInterfaceDataPtr data, short shutter )
	
	(shutter): desired shutter state to simulate, e.g. SHUTTER_OPEN or
	           SHUTTER_CLOSED.
	           
	If the shutter state is SHUTTER_CLOSED, the function moves the filter
	wheel to the "dark" position.
	
	If the shutter state is SHUTTER_OPEN, the function moves the filter wheel
	to the position indicated by the "cameraFilter" member of the camera interface
	data record (data).
	
*********************************************************************************/

short SetPixCellShutter ( CameraInterfaceDataPtr data, short shutter )
{
	short filter, synchronous;
	
	if ( data->cameraType >= CAMERA_TYPE_SBIG_ST5C && data->cameraType <= CAMERA_TYPE_PIXCEL_237 )
	{
		filter = data->cameraFilterPosition;
		synchronous = data->cameraSynchronous;
		
		data->cameraSynchronous = TRUE;
		if ( shutter == SHUTTER_CLOSED )
			data->cameraFilterPosition = 2; /* dark filter */

		SetCameraFilter ( data );
		
		data->cameraFilterColor = filter;
		data->cameraSynchronous = synchronous;
	}
	
	return ( TRUE );
}

/*** RequestedCCD **********************************************************

	Converts camera type code to SBIG CCD Request specifier.
	
*****************************************************************************/

CCD_REQUEST RequestedCCD ( short type )
{
	if ( type == CAMERA_TYPE_SBIG_ST7_TRACKING || type == CAMERA_TYPE_SBIG_ST8_TRACKING )
		return ( CCD_TRACKING );
	else
		return ( CCD_IMAGING );
}

/*** TemperatureToSetpoint **************************************************

	Converts CCD temperature to setpoint.

	double TemperatureToSetpoint ( double t )

    (t): temperature, in degrees C

    The function returns the corresponding setpoint value, in A/D units.
        
******************************************************************************/

double TemperatureToSetpoint ( double t )
{
	double	r, setpoint;

    r = R_0 * exp ( log ( R_RATIO_CCD ) * ( T_0 - t ) / DT_CCD );
    setpoint = MAX_AD / ( ( R_BRIDGE_CCD / r ) + 1.0 );

    return ( setpoint );
}

/***  SetpointToTemperature  *************************************************

	Converts CCD or ambient setpoint to temperature.

	double TemperatureToSetpoint ( double setpoint, double r_ratio,
	       double r_bridge, double dt )

    (setpoint): setpoint value in A/D units
	(r_ratio, r_bridge, dt): temperature constants; see SBIG documentation.
	
    The function returns the corresponding temperature in degrees C.
        
******************************************************************************/

double SetpointToTemperature ( double setpoint, double r_ratio, double r_bridge,
double dt )
{
	double	r, t;

	r = r_bridge / ( ( MAX_AD / setpoint ) - 1.0 );
	t = T_0 - dt * ( log ( r / R_0 ) / log ( r_ratio ) );

/*
    r = R_BRIDGE_CCD / ( ( MAX_AD / setpoint ) - 1.0 );
    t = T_0 - DT_CCD * ( log ( r / R_0 ) / log ( R_RATIO_CCD ) );
*/

    return ( t );
}
