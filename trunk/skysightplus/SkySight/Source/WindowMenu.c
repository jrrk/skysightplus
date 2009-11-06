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

/***  DoWindowMenuItem  ******************************************************/

void DoWindowMenuItem ( long item )
{
	GWindowPtr dialog;
	
	switch ( item )
	{
		case WINDOW_TILE_ITEM:
			GTileWindows();
			break;

		case WINDOW_CASCADE_ITEM:
			GCascadeWindows();
			break;

		case WINDOW_ARRANGE_ITEM:
			GArrangeWindowIcons();
			break;

		case WINDOW_CONTROL_ITEM:
			dialog = GetCameraControlDialog();
			if ( GIsWindowVisible ( dialog ) )
				GCloseWindow ( dialog );
			else
				GSetActiveWindow ( dialog );
			break;
		
		/*** We are now letting GUILib's own window-menu functionality handle this.
		
        default:
        	if ( item > WINDOW_CONTROL_ITEM )
            {
            	GWindowPtr window = GetNthImageWindow ( item - WINDOW_CONTROL_ITEM );

                if ( window != NULL )
                	GSetActiveWindow ( window );
            }
            break; ***/
	}
}
