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

#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "sxusbcam.h"
#include "GUILib.h"
#include "AstroLib.h"
#include "CameraInterface.h"

/***************************************************************************\
*                                                                           *
*                              CCD characteristics                          *
*                                                                           *
\***************************************************************************/

#define H9_INDEX            0
#define H9_MODEL            0x09
#define H9_WIDTH            1392
#define H9_HEIGHT           1040
#define H9_PIX_WIDTH        (int)(6.45*256)
#define H9_PIX_HEIGHT       (int)(6.45*256)
#define H9_HFRONT_PORCH     23
#define H9_HBACK_PORCH      40
#define H9_VFRONT_PORCH     11
#define H9_VBACK_PORCH      2
#define H9_VCLK_DELAY      -5  // VCLK delays are expressed in negative values for neg_udelay() function
#define HX_IMAGE_FIELDS     1
#define MX5_INDEX           1
#define MX5_MODEL           0x45
#define MX5_WIDTH           500
#define MX5_HEIGHT          290
#define MX5_PIX_WIDTH       (int)(9.8*256)
#define MX5_PIX_HEIGHT      (int)(12.6*256)
#define MX5_HFRONT_PORCH    23
#define MX5_HBACK_PORCH     30
#define MX5_VFRONT_PORCH    8
#define MX5_VBACK_PORCH     1
#define MX5_VCLK_DELAY      -2
#define MX7_INDEX           2
#define MX7_MODEL           0x47
#define MX7_WIDTH           752
#define MX7_HEIGHT          290
#define MX7_PIX_WIDTH       (int)(8.6*256)
#define MX7_PIX_HEIGHT      (int)(16.6*256)
#define MX7_HFRONT_PORCH    25
#define MX7_HBACK_PORCH     40
#define MX7_VFRONT_PORCH    7
#define MX7_VBACK_PORCH     1
#define MX7_VCLK_DELAY      -2
#define MX9_INDEX           3
#define MX9_MODEL           0x49
#define MX9_WIDTH           752
#define MX9_HEIGHT          290
#define MX9_PIX_WIDTH       (int)(11.6*256)
#define MX9_PIX_HEIGHT      (int)(22.4*256)
#define MX9_HFRONT_PORCH    25
#define MX9_HBACK_PORCH     40
#define MX9_VFRONT_PORCH    7
#define MX9_VBACK_PORCH     1
#define MX9_VCLK_DELAY      -2
#define MX25_INDEX          4
#define MX25_MODEL          0x59
#define MX25_WIDTH          3032
#define MX25_HEIGHT         2016
#define MX25_PIX_WIDTH      (int)(7.8*256)
#define MX25_PIX_HEIGHT     (int)(7.8*256)
#define MX25_HFRONT_PORCH   51
#define MX25_HBACK_PORCH    50
#define MX25_VFRONT_PORCH   4
#define MX25_VBACK_PORCH    2
#define MX25_VCLK_DELAY     -15
#define MX_IMAGE_FIELDS     2

int sx_ccd_hfront_porch[]   = {H9_HFRONT_PORCH,   MX5_HFRONT_PORCH,   MX7_HFRONT_PORCH,   MX9_HFRONT_PORCH, MX25_HFRONT_PORCH};
int sx_ccd_hback_porch[]    = {H9_HBACK_PORCH,    MX5_HBACK_PORCH,    MX7_HBACK_PORCH,    MX9_HBACK_PORCH,  MX25_HBACK_PORCH };
int sx_ccd_vfront_porch[]   = {H9_VFRONT_PORCH,   MX5_VFRONT_PORCH,   MX7_VFRONT_PORCH,   MX9_VFRONT_PORCH, MX25_VFRONT_PORCH};
int sx_ccd_vback_porch[]    = {H9_VBACK_PORCH,    MX5_VBACK_PORCH,    MX7_VBACK_PORCH,    MX9_VBACK_PORCH,  MX25_VBACK_PORCH };
int sx_ccd_width[]          = {H9_WIDTH,          MX5_WIDTH,          MX7_WIDTH,          MX9_WIDTH,        MX25_WIDTH       };
int sx_ccd_height[]         = {H9_HEIGHT,         MX5_HEIGHT,         MX7_HEIGHT,         MX9_HEIGHT,       MX25_HEIGHT      };
int sx_ccd_pix_width[]      = {H9_PIX_WIDTH,      MX5_PIX_WIDTH,      MX7_PIX_WIDTH,      MX9_PIX_WIDTH,    MX25_PIX_WIDTH   };
int sx_ccd_pix_height[]     = {H9_PIX_HEIGHT,     MX5_PIX_HEIGHT,     MX7_PIX_HEIGHT,     MX9_PIX_HEIGHT,   MX25_PIX_HEIGHT  };
int sx_ccd_vclk_delay[]     = {H9_VCLK_DELAY,     MX5_VCLK_DELAY,     MX7_VCLK_DELAY,     MX9_VCLK_DELAY,   MX25_VCLK_DELAY  };

extern void Arrayswap (short *Imptr1, short *Imptr2, short height, short width, short *end1, short *end2);

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
    
static HANDLE hCams[SXCCD_MAX_CAMS];
static short	sRow = 0;
static int colours, expose_flag, download_flag, cindex;
static short *cameraImage;

/*** SXUSBCameraInterface *************************************************

	This procedure is the entry point for the camera interface.
	
	short SXUSBCameraInterface ( short function, CameraInterfaceDataPtr data )

	(function): specifies which function the procedure should perform.
	(data):     on input, contains data needed by the procedure to perform
	            the specified function; on output, receives data generated
	            by the procedure.
	        
	The camera interface procedure should return an integer error code
	indicating whether or not it could complete the specified function
	successfully.  At present the only two return codes used are TRUE
	(success) and FALSE (failure).
	
**************************************************************************/

short SXUSBCameraInterface ( short function, CameraInterfaceDataPtr data )
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
		data->cameraFilterColor = FILTER_RED;
		strcpy ( data->cameraDescription, "Red" );
	}
	else if ( data->cameraFilterPosition == 3 )
	{
		data->cameraFilterColor = FILTER_GREEN;
		strcpy ( data->cameraDescription, "Green" );
	}
	else if ( data->cameraFilterPosition == 4 )
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
	short 		result = FALSE;
    struct t_sxccd_params *ccd_params = (struct t_sxccd_params *)(data->cameraPrivateData);
	GetCameraFilterInfo(data);

	switch (data->cameraFilterColor)
	{
	case FILTER_RED:
	case FILTER_GREEN:
	case FILTER_BLUE:
		if ( data->cameraReadoutMode != 2) return FALSE;
	case FILTER_CLEAR:
		if ( data->cameraReadoutMode >= 1 && data->cameraReadoutMode <= 8 )
		{
    		data->cameraImageWidth = sx_ccd_width[cindex] / data->cameraReadoutMode;
    		data->cameraImageHeight = sx_ccd_height[cindex] / data->cameraReadoutMode;
			sprintf ( data->cameraDescription, "%hd x %hd",
			data->cameraImageWidth, data->cameraImageHeight );
			result = TRUE;
			break;
		}
	}
	return ( result );
}

/***  GGetNumPorts  **********************************************************/

short GGetNumPorts ( void )
{
	return ( sxOpen(hCams) );
}

/***  GGetPortName  **********************************************************/

void GGetPortName ( short wPortNum, char *lpszPortName )
{
    int model, fw;
	struct t_sxccd_params *ccd_params = (struct t_sxccd_params *)malloc(sizeof(struct t_sxccd_params));
	if ( wPortNum > 0 )
			{
            model = sxGetCameraModel(hCams[wPortNum-1]);
            fw    = sxGetFirmwareVersion(hCams[wPortNum-1]);
            sxGetCameraParams(hCams[wPortNum-1], 0, ccd_params);
            wsprintf(lpszPortName, "SX-%c%d%c", model & 0x40 ? 'M' : 'H', model & 0x1F, model & 0x80 ? 'C' : ' ');
			}
	free(ccd_params);
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
    int model, fw;    
	data->cameraTECState = TEC_OFF;
	data->cameraTECPowerUse = 0.0;
	data->cameraTemperatureSetpoint = 0.0;
	data->cameraTemperatureImager = 20.0;
	data->cameraTemperatureAmbient = 20.0;
	
	data->cameraFilterPosition = 1;
	data->cameraFilterType = FILTER_TYPE_COLOUR_RGB;
	data->cameraAntibloomingState = 1;
	data->cameraReadoutMode = 1;
	data->cameraShutter = SHUTTER_OPEN;

	colours = FILTER_CLEAR;

	if ( data->cameraPort >= 1 && data->cameraPort <= GGetNumPorts())
			{
			char linbuf[256];
			struct t_sxccd_params *ccd_params = (struct t_sxccd_params *)malloc(sizeof(struct t_sxccd_params));
            model = sxGetCameraModel(hCams[data->cameraPort-1]);
            fw    = sxGetFirmwareVersion(hCams[data->cameraPort-1]);
            sxGetCameraParams(hCams[data->cameraPort-1], 0, ccd_params);
            sprintf(linbuf, "Camera model SX-%c%d%c found ", model & 0x40 ? 'M' : 'H', model & 0x1F, model & 0x80 ? 'C' : ' ');
			switch(model & ~0x80)
				{
				case 9: cindex = H9_INDEX; break;
				case 69: cindex = MX5_INDEX; break;
				case 71: cindex = MX7_INDEX; break;
				case 73: cindex = MX9_INDEX; break;
				case 89: cindex = MX25_INDEX; break;
				abort();
				}
            sprintf(linbuf, "with F/W version %d-%d\n", fw >> 16, fw & 0xFFFF);
            data->cameraImageWidth = sx_ccd_width[cindex];
			data->cameraImageHeight = sx_ccd_height[cindex];
            sprintf(linbuf, "\tImage array horizontal front porch: %d pixels, back porch: %d pixels\n", ccd_params->hfront_porch, ccd_params->hback_porch);
            sprintf(linbuf, "\tImage array vertical   front porch: %d pixels, back porch: %d pixels\n", ccd_params->vfront_porch, ccd_params->vback_porch);
            sprintf(linbuf, "\tPixel dimensions: %.1f x %.1f microns\n", ccd_params->pix_width, ccd_params->pix_height);
            sprintf(linbuf, "\tSerial ports: %d\n", ccd_params->num_serial_ports);
            if (ccd_params->extra_caps & SXCCD_CAPS_STAR2K) sprintf(linbuf, "\tIntegrated STAR2000 port\n");
            if (ccd_params->extra_caps & SXCCD_CAPS_EEPROM) sprintf(linbuf, "\tOn-camera EEPROM\n");
            if (ccd_params->extra_caps & SXCCD_CAPS_GUIDER)
            {
                sprintf(linbuf, "\tAttached guider camera\n");
                sxGetCameraParams(hCams[data->cameraPort-1], 1, ccd_params);
                sprintf(linbuf, "\t\tGuider image array dimensions: (%d, %d)\n", ccd_params->width, ccd_params->height);
                sprintf(linbuf, "\t\tGuider pixel dimensions: (%.1f, %.1f) microns\n", ccd_params->pix_width, ccd_params->pix_height);
            }
			data->cameraPrivateData = ccd_params;
			cameraImage = (short *)malloc(sx_ccd_width[cindex] * sx_ccd_height[cindex] * sizeof(short));
			sxReset(hCams[data->cameraPort-1]);
			return TRUE;
    }
	return ( FALSE );
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

	free(data->cameraPrivateData);
	sxClose(hCams[data->cameraPort-1]);

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

	if ( data->cameraFilterType == FILTER_TYPE_NONE )
		result = FALSE;
	else colours = data->cameraFilterPosition;
	GetCameraFilterInfo(data);

	switch( data->cameraFilterColor )
	{
	case FILTER_GREEN:
		{
		expose_flag = 1;
		break;
		}
	case FILTER_RED:
	case FILTER_BLUE:
		{
		expose_flag = 0;
		break;
		}
	}

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

	data->cameraFilterPosition = colours;
	
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
	int result = TRUE;
	enum {camIndex = 0};
	USHORT xoffset = 0;
	USHORT yoffset = 0;
    struct t_sxccd_params *ccd_params = (struct t_sxccd_params *)(data->cameraPrivateData);
	ULONG exposure = (ULONG)(data->cameraExposureLength * 1000.0 + 0.5);
	GetCameraFilterInfo(data);
	if (!GetCameraReadoutInfo(data))
		return FALSE;
	switch( data->cameraFilterColor )
	{
	case FILTER_CLEAR:
		{
		ULONG xVideoBin = data->cameraReadoutMode;
		ULONG yVideoBin = data->cameraReadoutMode;
		sxClearPixels(hCams[data->cameraPort-1], SXCCD_EXP_FLAGS_NOWIPE_FRAME, camIndex);
		sxClearPixels(hCams[data->cameraPort-1], 0, camIndex);
		sxClearPixels(hCams[data->cameraPort-1], 0, camIndex);
		/* begin next exposure */
		break;
		}
	case FILTER_RED:
	case FILTER_GREEN:
	case FILTER_BLUE:
		if (expose_flag)
		{
		ULONG xVideoBin = 1; 
		ULONG yVideoBin = 1;
		sxClearPixels(hCams[data->cameraPort-1], SXCCD_EXP_FLAGS_NOWIPE_FRAME, camIndex);
		sxClearPixels(hCams[data->cameraPort-1], 0, camIndex);
		sxClearPixels(hCams[data->cameraPort-1], 0, camIndex);
		/* begin next exposure */
		expose_flag = 0;
		download_flag = 1;
		break;
		}
	}
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
	short *tmpImage;
	int result = TRUE;
	enum {camIndex = 0};
	USHORT xoffset = 0;
	USHORT yoffset = 0;
    struct t_sxccd_params *ccd_params = (struct t_sxccd_params *)(data->cameraPrivateData);

    sRow = 0;

	GetCameraFilterInfo(data);
	if (!GetCameraReadoutInfo(data))
		return FALSE;

	tmpImage = (short *)malloc(sizeof(short) * sx_ccd_height[cindex] * sx_ccd_width[cindex]);
	switch( data->cameraFilterColor )
	{
	case FILTER_CLEAR:
		{
		ULONG xVideoBin = data->cameraReadoutMode;
		ULONG yVideoBin = data->cameraReadoutMode;
		sxLatchPixels(hCams[data->cameraPort-1], SXCCD_EXP_FLAGS_FIELD_BOTH, camIndex, xoffset, yoffset, sx_ccd_width[cindex]<<1, sx_ccd_height[cindex]>>1, xVideoBin, yVideoBin);
		sxReadPixels(hCams[data->cameraPort-1], tmpImage, sx_ccd_height[cindex] * sx_ccd_width[cindex] / xVideoBin / yVideoBin);
		Arrayswap(cameraImage, tmpImage, sx_ccd_height[cindex] / data->cameraReadoutMode, sx_ccd_width[cindex] / data->cameraReadoutMode,
			cameraImage + sx_ccd_width[cindex] * sx_ccd_height[cindex], tmpImage + sx_ccd_height[cindex] * sx_ccd_width[cindex]);
		break;
		}
	case FILTER_RED:
	case FILTER_GREEN:
	case FILTER_BLUE:
		if (download_flag)
		{
		ULONG xVideoBin = 1; 
		ULONG yVideoBin = 1;
		sxLatchPixels(hCams[data->cameraPort-1], SXCCD_EXP_FLAGS_FIELD_BOTH, camIndex, xoffset, yoffset, sx_ccd_width[cindex]<<1, sx_ccd_height[cindex]>>1, xVideoBin, yVideoBin);
		sxReadPixels(hCams[data->cameraPort-1], tmpImage, sx_ccd_height[cindex] * sx_ccd_width[cindex]);
		Arrayswap(cameraImage, tmpImage, sx_ccd_height[cindex]/ yVideoBin, sx_ccd_width[cindex] / xVideoBin,
			cameraImage + sx_ccd_width[cindex] * sx_ccd_height[cindex], tmpImage + sx_ccd_height[cindex] * sx_ccd_width[cindex]);
		download_flag = 0;
		break;
		}
	}
 	free(tmpImage);
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
    struct t_sxccd_params *ccd_params = (struct t_sxccd_params *)(data->cameraPrivateData);
	GetCameraFilterInfo(data);
	if (!GetCameraReadoutInfo(data))
		return FALSE;
	switch( data->cameraFilterColor )
	{
	case FILTER_CLEAR:
		{
    memcpy(data->cameraDownloadBuffer, &(cameraImage[sRow*data->cameraDownloadWidth]),
			data->cameraDownloadWidth * sizeof(short));
		break;
		}
	case FILTER_RED:
		{
			int i;
			for (i = 0; i < data->cameraDownloadWidth; i++)
			    data->cameraDownloadBuffer[i] = cameraImage[(sRow*2+0)*sx_ccd_width[cindex]+i*2];
		break;
		}
	case FILTER_GREEN:
		{
			int i;
			for (i = 0; i < data->cameraDownloadWidth; i++)
			    data->cameraDownloadBuffer[i] = cameraImage[(sRow*2+1)*sx_ccd_width[cindex]+i*2];
		break;
		}
	case FILTER_BLUE:
		{
			int i;
			for (i = 0; i < data->cameraDownloadWidth; i++)
			    data->cameraDownloadBuffer[i] = cameraImage[(sRow*2+0)*sx_ccd_width[cindex]+i*2+1];
		break;
		}
	}
    sRow++;
    return ( result );
}
