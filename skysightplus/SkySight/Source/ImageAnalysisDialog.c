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

#define ANALYSIS_X_LABEL					3
#define ANALYSIS_X_TEXT						4
#define ANALYSIS_Y_TEXT						6
#define ANALYSIS_VALUE_TEXT					8
#define ANALYSIS_SUM_TEXT					10
#define ANALYSIS_MIN_TEXT					12
#define ANALYSIS_MAX_TEXT					14
#define ANALYSIS_MEAN_TEXT					16

/*** DoImageAnalysisDialogEvent ***********************************************

	Handles events related to items in the "Image Analysis" panel of the
	"Camera Control" dialog.
	
*******************************************************************************/

int DoImageAnalysisDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int				respond = TRUE;
	short			col, row;
	GWindowPtr		window;
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
        	GSetWindowTitle ( GetCameraControlDialog(), "Image Analysis" );

			window = GetActiveImageWindow();
			if ( window == NULL )
			{
				ShowPixelValues ( NULL, 0, 0 );
				ShowImageRegionStatistics ( NULL );
			}
			else
			{
				GGetMousePosition ( window, &col, &row );
				ShowPixelValues ( window, col, row );
				ShowImageRegionStatistics ( GetImageWindowSelectedRegion ( window ) );
			}
			break;
			
		case G_CONTROL_EVENT:
			break;
	}
	
    return ( respond );
}


/*** ShowPixelValues ***/

void ShowPixelValues ( GWindowPtr window, short col, short row )
{
	short			type;
	char			text[256];
	float			x, y;
	double			value, red, grn, blu, re, im;
	ImagePtr		image;
	GWindowPtr		dialog;
	
	if ( GetCameraSubDialog ( &dialog ) != CAMERA_ANALYSIS_PAGE_DIALOG )
		return;
		
	if ( window != NULL )
	{
		image = GetImageWindowImage ( window );
		
		WindowToImageXY ( window, col, row, &x, &y );
		WindowToImage ( window, &col, &row );
		
		if ( col >= 0 && col < GetImageColumns ( image ) && row >= 0 && row < GetImageRows ( image ) )
		{
			sprintf ( text, "%hd", col );
			GSetDialogItemText ( dialog, ANALYSIS_X_TEXT, text );
			
			sprintf ( text, "%hd", row );
			GSetDialogItemText ( dialog, ANALYSIS_Y_TEXT, text );
			
			type = GetImageType ( image );

			if ( type == IMAGE_TYPE_MONOCHROME )
			{
				value = GetImageDataValue ( image, 0, row, col );
				sprintf ( text, "%.2f", value );
			}
			else if ( type == IMAGE_TYPE_RGB_COLOR )
			{
				red = GetImageDataValue ( image, 0, row, col );
				grn = GetImageDataValue ( image, 1, row, col );
				blu = GetImageDataValue ( image, 2, row, col );
				
				sprintf ( text, "%.2f,%.2f,%.2f", red, grn, blu );
			}
			else if ( type == IMAGE_TYPE_COMPLEX )
			{
				re = GetImageDataValue ( image, 0, row, col );
				im = GetImageDataValue ( image, 1, row, col );
				
				sprintf ( text, "%.2f,%.2f", re, im );
			}
			
			GSetDialogItemText ( dialog, ANALYSIS_VALUE_TEXT, text );
			return;
		}
	}

	GSetDialogItemText ( dialog, ANALYSIS_X_TEXT, "" );
	GSetDialogItemText ( dialog, ANALYSIS_Y_TEXT, "" );
	GSetDialogItemText ( dialog, ANALYSIS_VALUE_TEXT, "" );
}

/*** ShowImageRegionStatistics ***/

void ShowImageRegionStatistics ( ImageRegionPtr region )
{
	char			text[256];
	GWindowPtr		dialog;

	if ( GetCameraSubDialog ( &dialog ) != CAMERA_ANALYSIS_PAGE_DIALOG )
		return;
		
	if ( region == NULL )
	{
		GSetDialogItemText ( dialog, ANALYSIS_SUM_TEXT, "" );
		GSetDialogItemText ( dialog, ANALYSIS_MIN_TEXT, "" );
		GSetDialogItemText ( dialog, ANALYSIS_MAX_TEXT, "" );
		GSetDialogItemText ( dialog, ANALYSIS_MEAN_TEXT, "" );
	}
	else
	{
		sprintf ( text, "%.2f (%ld pixels)",
		GetImageRegionSum ( region ),
		GetImageRegionNumPixels ( region ) );
		GSetDialogItemText ( dialog, ANALYSIS_SUM_TEXT, text );
		
		sprintf ( text, "%.2f (at col %hd, row %hd)",
		GetImageRegionMin ( region ),
		GetImageRegionMinCol ( region ),
		GetImageRegionMinRow ( region ) );
		GSetDialogItemText ( dialog, ANALYSIS_MIN_TEXT, text );
		
		sprintf ( text, "%.2f (at col %hd, row %hd)",
		GetImageRegionMax ( region ),
		GetImageRegionMaxCol ( region ),
		GetImageRegionMaxRow ( region ) );		
		GSetDialogItemText ( dialog, ANALYSIS_MAX_TEXT, text );
	
		sprintf ( text, "%.2f (std. dev.: %.2f)",
		GetImageRegionMean ( region ),
		GetImageRegionStdDev ( region ) );
		GSetDialogItemText ( dialog, ANALYSIS_MEAN_TEXT, text );
	}
}
