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

/*** local header files ***/

#include "SkySight.h"

/*** local data types ***/

typedef struct CameraType
{
	short 					cameraType;		/* camera model type code */
	char					*cameraName;	/* camera model name */
	CameraInterfaceProcPtr 	cameraProc;		/* camera interface procedure */
}
CameraType;

typedef struct FilterType
{
	short 					filterType;		/* filter model type code */
	char					*filterName;	/* filter model name */
}
FilterType;

/*** local data ***/

static CameraType sCameraTypes[] =
{
	{ CAMERA_TYPE_SXUSB, "Starlight Express USB", SXUSBCameraInterface },
	{ CAMERA_TYPE_DEMO, "Demo Interface Only", DemoCameraInterface },
//	{ CAMERA_TYPE_IBIS4b_DEMO, "IBIS4b Demo Camera", IBIS4bDemoCameraInterface },
//	{ CAMERA_TYPE_SBIG_ST4, "SBIG ST-4", SBIGST4CameraInterface },
//	{ CAMERA_TYPE_SBIG_ST4X, "SBIG ST-4X", SBIGSerialCameraInterface },
//	{ CAMERA_TYPE_SBIG_ST5, "SBIG ST-5", SBIGSerialCameraInterface },
//	{ CAMERA_TYPE_SBIG_ST6, "SBIG ST-6", SBIGSerialCameraInterface },
	{ CAMERA_TYPE_SBIG_ST7_IMAGING, "SBIG ST-7 Imaging Camera", SBIGParallelCameraInterface },
	{ CAMERA_TYPE_SBIG_ST7_TRACKING, "SBIG ST-7 Tracking Camera", SBIGParallelCameraInterface },
	{ CAMERA_TYPE_SBIG_ST8_IMAGING, "SBIG ST-8 Imaging Camera", SBIGParallelCameraInterface },
	{ CAMERA_TYPE_SBIG_ST8_TRACKING, "SBIG ST-8 Tracking Camera", SBIGParallelCameraInterface },
	{ CAMERA_TYPE_SBIG_ST5C, "SBIG ST-5C", SBIGParallelCameraInterface },
	{ CAMERA_TYPE_SBIG_ST237, "SBIG ST-237", SBIGParallelCameraInterface },
	{ CAMERA_TYPE_PIXCEL_255, "Celestron PixCel 255", SBIGParallelCameraInterface },
	{ CAMERA_TYPE_PIXCEL_237, "Celestron PixCel 237", SBIGParallelCameraInterface },
//	{ CAMERA_TYPE_COOKBOOK_211, "Cookbook 211", CookbookCameraInterface },
	{ CAMERA_TYPE_COOKBOOK_245, "Cookbook 245", CookbookCameraInterface }
};

static short sNumCameraTypes = sizeof ( sCameraTypes ) / sizeof ( sCameraTypes[0] );

static FilterType sFilterTypes[] =
{
	{ FILTER_TYPE_NONE, "None" },
	{ FILTER_TYPE_PIXCEL_VANE, "PixCel 2-position vane" },
	{ FILTER_TYPE_PIXCEL_COLOR, "PixCel 5-position color" },
	{ FILTER_TYPE_SBIG_CFW_6A, "SBIG CFW-6A" },
	{ FILTER_TYPE_SBIG_CFW_8, "SBIG CFW-8" },
	{ FILTER_TYPE_COLOUR_RGB, "Starlight Beyer Matrix" }
};

static short sNumFilterTypes = sizeof ( sFilterTypes ) / sizeof ( sFilterTypes[0] );

/*** GetNumCameraTypes *********************************************************

	Returns the total number of camera types that are currently supported.
	
********************************************************************************/

short GetNumCameraTypes ( void )
{
	return ( sNumCameraTypes );
}

/*** GetNthCameraType **********************************************************

	Returns information about an individual camera type.
	
	short GetNthCameraType ( short n, char *name, CameraInterfaceProcPtr *proc )
	
	(n):    integer from 1 to the total number of supported camera types.
	(name): recieves camera model name.
	(proc): recieves camera interface procedure function.
	
	If the integer camera type specifier is in the range from one to the value
	returned by GetNumCameraTypes(), the function returns the camera type code
	corresponding to the specified camera type.  Otherwise, the function returns
	zero.
	
	The camera model name corresponding to the camera type is returned in the
	string pointed to by the (name) parameter.  This string must be big enough
	to hold at least 256 characters.  If this parameter is NULL, the camera model
	name is not returned.
	
	A pointer to the camera's interface procedure function is returned in the
	(proc) parameter.  If this parameter is NULL, the camera's interface
	procedure function is not returned.
	
********************************************************************************/

short GetNthCameraType ( short n, char *name, CameraInterfaceProcPtr *proc )
{
	if ( n < 1 || n > sNumCameraTypes )
		return ( 0 );
		
	if ( name != NULL )
		strcpy ( name, sCameraTypes[ n - 1 ].cameraName );
		
	if ( proc != NULL )
		*proc = sCameraTypes[ n - 1].cameraProc;
		
	return ( sCameraTypes[ n - 1 ].cameraType );
}

/*** GetNumFilterTypes *********************************************************

	Returns the total number of filter types that are currently supported.
	
********************************************************************************/

short GetNumFilterTypes ( void )
{
	return ( sNumFilterTypes );
}

/*** GetNthFilterType **********************************************************

	Returns information about an individual filter type.
	
	short GetNthFilterType ( short n, char *name )
	
	(n):    integer from 1 to the total number of supported filter types.
	(name): recieves filter model name.
	
	If the integer filter type specifier is in the range from one to the value
	returned by GetNumFilterTypes(), the function returns the filter type code
	corresponding to the specified filter type.  Otherwise, the function returns
	zero.
	
	The filter model name corresponding to the filter type is returned in the
	string pointed to by the (name) parameter.  This string must be big enough
	to hold at least 256 characters.  If this parameter is NULL, the filter model
	name is not returned.
	
********************************************************************************/

short GetNthFilterType ( short n, char *name )
{
	if ( n < 1 || n > sNumFilterTypes )
		return ( 0 );
		
	if ( name != NULL )
		strcpy ( name, sFilterTypes[ n - 1 ].filterName );
		
	return ( sFilterTypes[ n - 1 ].filterType );
}
