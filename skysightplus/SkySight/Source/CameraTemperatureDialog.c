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

#define CAMERA_TEMPERATURE_SET_BUTTON		3
#define CAMERA_TEMPERATURE_OFF_BUTTON		5
#define CAMERA_TEMPERATURE_ON_BUTTON		6
#define CAMERA_TEMPERATURE_POWER_USE_TEXT	8
#define CAMERA_TEMPERATURE_AMBIENT_TEXT		10
#define CAMERA_TEMPERATURE_IMAGER_TEXT		12
#define CAMERA_TEMPERATURE_SETPOINT_SLIDER	14
#define CAMERA_TEMPERATURE_SETPOINT_TEXT	15

/*** DoCameraTemperatureDialogEvent *********************************************************

	Handles events related to the "Camera Temperature" panel of the "Camera Control"
	dialog when it is active.
	
*********************************************************************************************/

int DoCameraTemperatureDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int			respond = TRUE;
    short		state, item, min, max, value;
    float		temp;
	char		text[256];
    GControlPtr	control;
	CameraPtr	camera;
	
    switch ( event )
    {
    	case G_INITIAL_EVENT:
        	GSetWindowTitle ( GetCameraControlDialog(), "Camera Temperature Control" );
    		camera = GetActiveCamera();
    		if ( camera != NULL )
    		{
	        	for ( item = CAMERA_TEMPERATURE_SET_BUTTON; item <= CAMERA_TEMPERATURE_SETPOINT_TEXT; item++ )
	        		GEnableDialogItem ( dialog, item, TRUE );
	        		
	        	GetCameraTemperatureRegulation ( camera, &state, &temp );
	
	            if ( state )
	            	GSetSelectedDialogButton ( dialog, CAMERA_TEMPERATURE_OFF_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON );
	            else
	            	GSetSelectedDialogButton ( dialog, CAMERA_TEMPERATURE_OFF_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON, CAMERA_TEMPERATURE_OFF_BUTTON );
	
	            sprintf ( text, "%+.1f", temp );
	            GSetDialogItemText ( dialog, CAMERA_TEMPERATURE_SETPOINT_TEXT, text );
	
	            control = GGetControl ( dialog, CAMERA_TEMPERATURE_SETPOINT_SLIDER );
	            GSetControlMax ( control, 30 );
	            GSetControlMin ( control, -30 );
	            GSetControlValue ( control, temp );
	
				GSetDialogDefaultButton ( dialog, CAMERA_TEMPERATURE_SET_BUTTON );
				UpdateCameraTemperatureDialog ( 0 );
			}
			else
			{
	        	for ( item = CAMERA_TEMPERATURE_SET_BUTTON; item <= CAMERA_TEMPERATURE_SETPOINT_TEXT; item++ )
	        		GEnableDialogItem ( dialog, item, FALSE );
			}
        	break;

        case G_CONTROL_EVENT:
	   		camera = GetActiveCamera();
        	switch ( param1 )
            {
            	case CAMERA_TEMPERATURE_SET_BUTTON:
                    GGetDialogItemText ( dialog, CAMERA_TEMPERATURE_SETPOINT_TEXT, text );
                    sscanf ( text, "%f", &temp );

                    control = GGetControl ( dialog, CAMERA_TEMPERATURE_SETPOINT_SLIDER );
                    min = GGetControlMin ( control );
                    max = GGetControlMax ( control );
                    value = ( max - min ) * ( temp + 30.0 ) / 60.0 + min; 
                    GSetControlValue ( control, value );

                    if ( SetCameraTemperatureRegulation ( camera, TRUE, temp ) )
						GSetSelectedDialogButton ( dialog, CAMERA_TEMPERATURE_OFF_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON );
                	break;

                case CAMERA_TEMPERATURE_SETPOINT_SLIDER:
                    control = GGetControl ( dialog, CAMERA_TEMPERATURE_SETPOINT_SLIDER );
                    value = GGetControlValue ( control );
                    min = GGetControlMin ( control );
                    max = GGetControlMax ( control );
                    temp = 60.0 * ( value - min ) / ( max - min ) - 30.0;
                    sprintf ( text, "%+.1f", temp );
                    GSetDialogItemText ( dialog, CAMERA_TEMPERATURE_SETPOINT_TEXT, text );
                    
                    if ( param2 == G_SCROLL_THUMB )
                		if ( SetCameraTemperatureRegulation ( camera, TRUE, temp ) )
                			GSetSelectedDialogButton ( dialog, CAMERA_TEMPERATURE_OFF_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON );
                	break;

                case CAMERA_TEMPERATURE_SETPOINT_TEXT:
                    break;

                case CAMERA_TEMPERATURE_OFF_BUTTON:
                    GGetDialogItemText ( dialog, CAMERA_TEMPERATURE_SETPOINT_TEXT, text );
                    sscanf ( text, "%f", &temp );
                	if ( SetCameraTemperatureRegulation ( camera, FALSE, temp ) )
                		GSetSelectedDialogButton ( dialog, CAMERA_TEMPERATURE_OFF_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON, param1 );
                	break;
                	
                case CAMERA_TEMPERATURE_ON_BUTTON:
                    GGetDialogItemText ( dialog, CAMERA_TEMPERATURE_SETPOINT_TEXT, text );
                    sscanf ( text, "%f", &temp );
                	if ( SetCameraTemperatureRegulation ( camera, TRUE, temp ) )
                		GSetSelectedDialogButton ( dialog, CAMERA_TEMPERATURE_OFF_BUTTON, CAMERA_TEMPERATURE_ON_BUTTON, param1 );
                	break;
            }
    }

    return ( respond );
}

/*** UpdateCameraTemperatureDialog **********************************************

	Updates the camera thermal regulation status parameters displayed in the
	"Temperature" panel of the "Camera Control" dialog.
	
	int UpdateCameraTemperatureDialog ( long interval )

	(interval): minimum interval between updates, in system clock ticks.
	
	The function returns TRUE if successful, and FALSE on failure.
	
	If the time elapsed since the previous call of this function is less
	than (interval), the function will do nothing.  You can use this feature
	to control the rate at which the temperature status display is actually
	updated.
	
*********************************************************************************/

int UpdateCameraTemperatureDialog ( long interval )
{
	char		text[256];
    float		tempImg, tempAmb, power;
    long		currentTick;
    static long	lastUpdateTick = 0;
    GWindowPtr	dialog = NULL;
	CameraPtr	camera;
	
	if ( GetCameraSubDialog ( &dialog ) != CAMERA_TEMPERATURE_PAGE_DIALOG )
    	return ( TRUE );

	camera = GetActiveCamera();
	
    currentTick = GGetTickCount();
    if ( currentTick - lastUpdateTick < interval )
    	return ( TRUE );
        
    if ( GetCameraTemperatureStatus ( camera, &tempImg, &tempAmb, &power ) == FALSE )
    	return ( FALSE );

	sprintf ( text, "%+.1f", tempImg );
	GSetDialogItemText ( dialog, CAMERA_TEMPERATURE_IMAGER_TEXT, text );

	sprintf ( text, "%+.1f", tempAmb );
	GSetDialogItemText ( dialog, CAMERA_TEMPERATURE_AMBIENT_TEXT, text );

	sprintf ( text, "%.1f", power * 100.0 );
	GSetDialogItemText ( dialog, CAMERA_TEMPERATURE_POWER_USE_TEXT, text );

    lastUpdateTick = currentTick;
    return ( TRUE );
}

