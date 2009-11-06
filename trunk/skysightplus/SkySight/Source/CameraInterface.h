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

/*** Prevent multiple #includes ***/

#ifndef CAMERA_INTERFACE_H
#define CAMERA_INTERFACE_H

/*** TRUE/FALSE ***/

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

/*** GUILib port number to LPT number conversion constants.
     Yes, this is an ugly hack; we'll solve it more elegantly later. ***/

#if defined ( GMAC )
#define LPT0	0
#elif defined ( GWIN )
#define LPT0	9
#endif

/*** Camera type codes: add new camera types here. ***/

#define CAMERA_TYPE_DEMO				0
#define CAMERA_TYPE_SBIG_ST4			1
#define CAMERA_TYPE_SBIG_ST4X			2
#define CAMERA_TYPE_SBIG_ST5			3
#define CAMERA_TYPE_SBIG_ST6			4
#define CAMERA_TYPE_SBIG_ST7_IMAGING	5
#define CAMERA_TYPE_SBIG_ST7_TRACKING	6
#define CAMERA_TYPE_SBIG_ST8_IMAGING	7
#define CAMERA_TYPE_SBIG_ST8_TRACKING	8
#define CAMERA_TYPE_SBIG_ST5C			9
#define CAMERA_TYPE_SBIG_ST237			10
#define CAMERA_TYPE_PIXCEL_255			11
#define CAMERA_TYPE_PIXCEL_237			12
#define CAMERA_TYPE_COOKBOOK_211		13
#define CAMERA_TYPE_COOKBOOK_245		14
#define CAMERA_TYPE_SXUSB				15

/*** Camera interface procedure function codes ***/

#define CAMERA_OPEN_CONNECTION		1
#define CAMERA_CLOSE_CONNECTION		2
#define CAMERA_SET_TEMPERATURE		3
#define CAMERA_GET_TEMPERATURE		4
#define CAMERA_SET_FILTER			5
#define CAMERA_GET_FILTER			6
#define CAMERA_START_EXPOSURE		8
#define CAMERA_END_EXPOSURE			9
#define CAMERA_START_DOWNLOAD		10
#define CAMERA_END_DOWNLOAD			11
#define CAMERA_DISCARD_ROWS			12
#define CAMERA_DOWNLOAD_ROW			13

/*** Camera information request codes ***/

#define CAMERA_GET_SHUTTER_INFO			100
#define CAMERA_GET_FILTER_INFO			101
#define CAMERA_GET_TEC_INFO				102
#define CAMERA_GET_READOUT_INFO			103
#define CAMERA_GET_ANTIBLOOMING_INFO	104

/*** Camera shutter type codes ***/

#define SHUTTER_TYPE_NONE				0
#define SHUTTER_TYPE_PHYSICAL			1
#define SHUTTER_TYPE_ELECTRONIC			2

/*** Shutter state codes ***/

#define SHUTTER_CLOSED					0
#define SHUTTER_OPEN					1

/*** Thermoelectric cooling type codes ***/

#define TEC_ABSENT						0
#define TEC_PRESENT						1						

/*** Thermoelectric cooling state codes ***/

#define TEC_OFF							0
#define TEC_ON							1
#define TEC_UNKNOWN						-274	// represents an unknown temperature.  Why this value?  Absolute zero is -273.15 so -274 is an "impossible" temperature.

/*** Filter type codes ***/

#define FILTER_TYPE_NONE				0
#define FILTER_TYPE_PIXCEL_VANE			2
#define FILTER_TYPE_PIXCEL_COLOR		5
#define FILTER_TYPE_SBIG_CFW_6A			6
#define FILTER_TYPE_SBIG_CFW_8			8
#define FILTER_TYPE_COLOUR_RGB			9

/*** Filter color codes ***/

#define FILTER_UNKNOWN					0
#define FILTER_CLEAR					1
#define FILTER_DARK						2
#define FILTER_RED						3
#define FILTER_GREEN					4
#define FILTER_BLUE						5

/*** Common camera interface data structure ***/

typedef struct CameraInterfaceData
{
	short			cameraType;						/* type code */
	short			cameraPort;						/* GUILib port specification code */
	short			cameraTECType;					/* thermoelectric cooling type code */
	short			cameraTECState;					/* thermoelectric cooling state (off or on) */
	float			cameraTECPowerUse;				/* thermoelectric cooling power usage (0-1) */
	float			cameraTemperatureSetpoint;		/* desired temperature for imaging sensor, degrees C */
	float			cameraTemperatureImager;		/* actual temperature of imaging sensor, degrees C */
	float			cameraTemperatureAmbient;		/* ambient temperature sensor reading, degrees C */
	float			cameraExposureLength;			/* desired exposure length, in seconds */
	short			cameraShutterType;				/* shutter type code */
	short			cameraShutter;					/* shutter state during exposure */
	short			cameraFilterType;				/* filter type code */
	short			cameraFilterPosition;			/* filter position */
	short			cameraFilterColor;				/* filter color code */
	short			cameraSynchronous;				/* wait for filter & exposure commands to complete before returning */
	short			cameraAntibloomingState;		/* antiblooming gate state */
	short			cameraReadoutMode;				/* readout/binning mode */
	short			cameraImageWidth;				/* number of image columns in current binning mode */
	short			cameraImageHeight;				/* number of image rows in current binning mode */
	short			cameraDownloadDiscard;			/* number of rows to discard */
	short			cameraDownloadStart;			/* starting column to digitize */
	short			cameraDownloadWidth;			/* number of pixels to digitize */
	unsigned short	*cameraDownloadBuffer;			/* buffer to place downloaded data */
	void			*cameraPrivateData;				/* pointer to private camera data */
	char			cameraDescription[255];			/* buffer for textual description of camera items */
}
CameraInterfaceData, *CameraInterfaceDataPtr;

/*** Generic camera interface procedure function prototype ***/

typedef short (*CameraInterfaceProcPtr) ( short, CameraInterfaceDataPtr );

/*** Specific camera interface procedures: add new interfaces here. ***/

short DemoCameraInterface ( short, CameraInterfaceDataPtr );
short CookbookCameraInterface ( short, CameraInterfaceDataPtr );
short SBIGParallelCameraInterface ( short, CameraInterfaceDataPtr );
short SBIGSerialCameraInterface ( short, CameraInterfaceDataPtr );
short SBIGST4CameraInterface ( short, CameraInterfaceDataPtr );

#endif /* CAMERA_INTERFACE_H */

