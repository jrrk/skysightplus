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

#define CAMERA_SETUP_CONNECT_BUTTON			3
#define CAMERA_SETUP_CAMERA_MENU			5
#define CAMERA_SETUP_FILTER_MENU			7
#define CAMERA_SETUP_PORT_MENU				9

/*** local data ***/

static short		sCameraPort = 1;
static short		sCameraType = 1;
static short		sFilterType = 1;

/*** DoCameraSetupDialogEvent **********************************************************

	Handles events related to items in the "Camera Setup" panel of the "Camera Control"
	dialog when it is active.
	
****************************************************************************************/

int DoCameraSetupDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int			respond = TRUE;
    short		item, port, numPorts;
    char		text[256];
    GControlPtr	control;
    CameraPtr	camera;
    
    switch ( event )
    {
    	case G_INITIAL_EVENT:
        	GSetWindowTitle ( GetCameraControlDialog(), "Camera Setup" );
    	
    		control = GGetControl ( dialog, CAMERA_SETUP_CAMERA_MENU );
    		for ( item = 1; item <= GetNumCameraTypes(); item++ )
    		{
    			GetNthCameraType ( item, text, NULL );
    			GSetPopupMenuItemText ( control, item, text );
    		}
    		GSetControlValue ( control, sCameraType );
   			
    		control = GGetControl ( dialog, CAMERA_SETUP_FILTER_MENU );
    		for ( item = 1; item <= GetNumFilterTypes(); item++ )
    		{
    			GetNthFilterType ( item, text );
    			GSetPopupMenuItemText ( control, item, text );
    		}
    		GSetControlValue ( control, sFilterType );

        	numPorts = GGetNumPorts();
            control = GGetControl ( dialog, CAMERA_SETUP_PORT_MENU );
            for ( port = 1; port <= numPorts; port++ )
            {
            	GGetPortName ( port, text );
                GSetPopupMenuItemText ( control, port, text );
            }
			GSetControlValue ( control, sCameraPort );

            camera = GetActiveCamera();
            if ( camera == NULL )
            {
            	GSetDialogItemText ( dialog, CAMERA_SETUP_CONNECT_BUTTON, "Connect" );
        		GEnableDialogItem ( dialog, CAMERA_SETUP_PORT_MENU, TRUE );
        		GEnableDialogItem ( dialog, CAMERA_SETUP_CAMERA_MENU, TRUE );
        		GEnableDialogItem ( dialog, CAMERA_SETUP_FILTER_MENU, TRUE );
            }
            else
            {
            	GSetDialogItemText ( dialog, CAMERA_SETUP_CONNECT_BUTTON, "Disconnect" );
        		GEnableDialogItem ( dialog, CAMERA_SETUP_PORT_MENU, FALSE );
        		GEnableDialogItem ( dialog, CAMERA_SETUP_CAMERA_MENU, FALSE );
        		GEnableDialogItem ( dialog, CAMERA_SETUP_FILTER_MENU, FALSE );
            }

			GSetDialogDefaultButton ( dialog, CAMERA_SETUP_CONNECT_BUTTON );
        	break;

        case G_CONTROL_EVENT:
        	switch ( param1 )
            {
            	case CAMERA_SETUP_CONNECT_BUTTON:
            		camera = GetActiveCamera();
                	if ( camera == NULL )
                    {
                		control = GGetControl ( dialog, CAMERA_SETUP_PORT_MENU );
                		sCameraPort = GGetControlValue ( control );
						
                		control = GGetControl ( dialog, CAMERA_SETUP_CAMERA_MENU );
                		sCameraType = GGetControlValue ( control );

                		control = GGetControl ( dialog, CAMERA_SETUP_FILTER_MENU );
                		sFilterType = GGetControlValue ( control );

						camera = NewCameraConnection ( sCameraType, sFilterType, sCameraPort );
						
                    	if ( camera != NULL )
                    	{
			            	GSetDialogItemText ( dialog, CAMERA_SETUP_CONNECT_BUTTON, "Disconnect" );
			        		GEnableDialogItem ( dialog, CAMERA_SETUP_PORT_MENU, FALSE );
			        		GEnableDialogItem ( dialog, CAMERA_SETUP_CAMERA_MENU, FALSE );
			        		GEnableDialogItem ( dialog, CAMERA_SETUP_FILTER_MENU, FALSE );
                    	}
                    	else
                    	{
                    		GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't open camera connection!" );
                    	}
                    }
                    else
                    {
                    	if ( DisconnectCamera() == FALSE )
                        {
                        	GDoAlert ( G_WARNING_ALERT, G_OK_ALERT, "Can't close camera connection!" );
                        }
                        else
                        {
			            	GSetDialogItemText ( dialog, CAMERA_SETUP_CONNECT_BUTTON, "Connect" );
			        		GEnableDialogItem ( dialog, CAMERA_SETUP_PORT_MENU, TRUE );
			        		GEnableDialogItem ( dialog, CAMERA_SETUP_CAMERA_MENU, TRUE );
			        		GEnableDialogItem ( dialog, CAMERA_SETUP_FILTER_MENU, TRUE );
                        }
                    }
                    break;
                    
            	case CAMERA_SETUP_CAMERA_MENU:
            		sCameraType = GGetDialogControlValue ( dialog, param1 );
            		break;

            	case CAMERA_SETUP_FILTER_MENU:
            		sFilterType = GGetDialogControlValue ( dialog, param1 );
            		break;

            	case CAMERA_SETUP_PORT_MENU:
            		sCameraPort = GGetDialogControlValue ( dialog, param1 );
            		break;
            }
            
        	break;
    }
    
    return ( respond );
}

/*** DisconnectCamera *********************************************************

	High-level function which performs all tasks related to disconnecting
    from the camera.

	void DisconnectCamera ( void )

    This function takes no parameters.  It returns TRUE if it can complete
    all of its tasks successfully, and FALSE otherwise.

******************************************************************************/

int DisconnectCamera ( void )
{
	int			result = TRUE;
    CameraPtr	camera = GetActiveCamera();
    
	if ( camera != NULL )
    {
        /*** If there is an exposure in progress, cancel it. ***/
        
        if ( GetCameraStatus ( camera, NULL, NULL ) != CAMERA_STATUS_WAITING )
        	CancelExposure ( camera );
            
        /*** Turn off the camera's thermal regulation, if present. ***/
        
        if ( CameraHasCooling ( camera ) )
    		result = SetCameraTemperatureRegulation ( camera, FALSE, 0.0 );

        /*** Finally, close the communication link to the camera
             and destroy the camera record. ***/
        
        DeleteCameraConnection ( camera );
    }

    return ( result );
}
