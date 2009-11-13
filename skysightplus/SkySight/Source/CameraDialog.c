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

#define CAMERA_TAB_GROUP	1
#define CAMERA_PAGE_RECT	2

/*** local data ***/

static GWindowPtr	sCameraControlDialog = NULL;
static int			sCameraSubDialog = 0;

/*** CreateCameraControlDialog *****************************************************

	Creates and initializes the one-and-only camera control dialog window.  The
	function returns a pointer to the window, which can also be retrieved by calling
	GetCameraControlDialog().
	
************************************************************************************/

GWindowPtr CreateCameraControlDialog ( void )
{
	GWindowPtr	dialog;

	dialog = GCreateDialog ( CAMERA_CONTROL_DIALOG, 0, DoCameraControlDialogEvent );
	if ( dialog == NULL )
		return ( NULL );
	else
		sCameraControlDialog = dialog;
	
//	GShowWindow ( dialog );
	return ( dialog );
}

/*** GetCameraControlDialog ********************************************************

	Returns a pointer to the camera control dialog window.  If the dialog has not
	yet been created, this function returns NULL.
	
************************************************************************************/

GWindowPtr GetCameraControlDialog ( void )
{
	return ( sCameraControlDialog );
}

/*** GetCameraSubDialog ********************************************************

	Returns an integer indicating the dialog resource number of the sub-dialog
	which is currently being displayed in the "Camera Control" dialog, and,
	optionally, a pointer to the sub-dialog window itself.

	short GetCameraSubDialog ( GWindowPtr *dialog )
	
	(dialog): if not NULL, receives a pointer to the sub-dialog window.
	
	The function returns the resource number of the currently-displayed sub-
	dialog.
	
	Please note that on the Macintosh, the sub-dialog window is the same as
	the main "Camera Control" dialog window.  This is different in Windows,
	since the items displayed in the sub-dialog actually belong to a separate
	child window of the root camera page control.
	
************************************************************************************/

short GetCameraSubDialog ( GWindowPtr *dialog )
{
	if ( dialog != NULL )
    	*dialog = GGetSubDialog ( sCameraControlDialog, CAMERA_PAGE_RECT );
    	
	return ( sCameraSubDialog );
}

/*** DoCameraControlDialogEvent *****************************************************

	Handles events related to the "Camera Control" dialog.
	
	This function handles events related to the main "Camera Control" dialog,
	and in particular its tab group control.  When the user clicks on one of
	the tabs in the control, the items displayed inside the tab group switch.
	Events related to those items are handled in separate functions called
	from here.
	
*************************************************************************************/

int DoCameraControlDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int				respond = TRUE;
    short			item;
    GControlPtr		control;
    GRect			rect;
    
	switch ( event )
	{
        case G_INITIAL_EVENT:
        	sCameraSubDialog = param1;
        	switch ( sCameraSubDialog )
        	{
        		case CAMERA_CONTROL_DIALOG:
        			sCameraControlDialog = dialog;
        			
        			control = GGetControl ( dialog, CAMERA_TAB_GROUP );
					GInsertTabGroupItem ( control, 1, "Setup" );
					GInsertTabGroupItem ( control, 2, "Temperature" );
					GInsertTabGroupItem ( control, 3, "Exposure" );
	      			GInsertTabGroupItem ( control, 4, "Display" );
        			GInsertTabGroupItem ( control, 5, "Analysis" );
					GSetControlValue ( control, 1 );
					
					GGetTabGroupContentRect ( control, &rect );
            		GSetDialogItemRect ( dialog, CAMERA_PAGE_RECT, &rect );
					GSetSubDialog ( dialog, CAMERA_PAGE_RECT, CAMERA_SETUP_PAGE_DIALOG );
					break;

				case CAMERA_SETUP_PAGE_DIALOG:
					respond = DoCameraSetupDialogEvent ( event, dialog, param1, param2 );
					break;

				case CAMERA_TEMPERATURE_PAGE_DIALOG:
					respond = DoCameraTemperatureDialogEvent ( event, dialog, param1, param2 );
					break;

				case CAMERA_EXPOSURE_PAGE_DIALOG:
					respond = DoCameraExposureDialogEvent ( event, dialog, param1, param2 );
					break;
					
				case CAMERA_DISPLAY_PAGE_DIALOG:
					respond = DoImageDisplayDialogEvent ( event, dialog, param1, param2 );
					break;

				case CAMERA_ANALYSIS_PAGE_DIALOG:
					respond = DoImageAnalysisDialogEvent ( event, dialog, param1, param2 );
					break;
			}
            break;
        
        /*** When the control panel window recieves keyboard focus, update the menus
             so that the proper "Window" menu item will get checked. ***/
             
        case G_ACTIVATE_EVENT:
        	UpdateMenus();
        	break;
        	   
		case G_CLOSE_EVENT:
			GHideWindow ( dialog );
			UpdateMenus();
			respond = FALSE;
			break;
			
		case G_CONTROL_EVENT:
			if ( param1 <= CAMERA_PAGE_RECT )
			{
	        	control = GGetControl ( dialog, param1 );
	        	switch ( param1 )
	            {
	            	case CAMERA_TAB_GROUP:
	                	item = GGetControlValue ( control );
	                    switch ( item )
	                    {
	                        case 1:
	                            GSetSubDialog ( dialog, CAMERA_PAGE_RECT, CAMERA_SETUP_PAGE_DIALOG );
	                        	break;

	                        case 2:
	                        	GSetSubDialog ( dialog, CAMERA_PAGE_RECT, CAMERA_TEMPERATURE_PAGE_DIALOG );
	                            break;

	                        case 3:
	                        	GSetSubDialog ( dialog, CAMERA_PAGE_RECT, CAMERA_EXPOSURE_PAGE_DIALOG );
	                            break;
	                            
	                    	case 4:
	                            GSetSubDialog ( dialog, CAMERA_PAGE_RECT, CAMERA_DISPLAY_PAGE_DIALOG );
	                            break;

	                    	case 5:
	                            GSetSubDialog ( dialog, CAMERA_PAGE_RECT, CAMERA_ANALYSIS_PAGE_DIALOG );
	                            break;
	                    }
	                    break;
	            }
	        }
	        else
	        {
	        	switch ( sCameraSubDialog )
	        	{
					case CAMERA_SETUP_PAGE_DIALOG:
						respond = DoCameraSetupDialogEvent ( event, dialog, param1, param2 );
						break;

					case CAMERA_TEMPERATURE_PAGE_DIALOG:
						respond = DoCameraTemperatureDialogEvent ( event, dialog, param1, param2 );
						break;

					case CAMERA_EXPOSURE_PAGE_DIALOG:
						respond = DoCameraExposureDialogEvent ( event, dialog, param1, param2 );
						break;

					case CAMERA_DISPLAY_PAGE_DIALOG:
						respond = DoImageDisplayDialogEvent ( event, dialog, param1, param2 );
						break;

					case CAMERA_ANALYSIS_PAGE_DIALOG:
						respond = DoImageAnalysisDialogEvent ( event, dialog, param1, param2 );
						break;
	        	}
	        }
			break;
	}
	return ( respond );
}

