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

/*** local functions ***/

static void DrawImageWindowMonochromeBitmap ( GWindowPtr );
static void DrawImageWindowRGBColorBitmap ( GWindowPtr );

/***  ImageToWindow  *************************************************************

	Converts a position in an image window's image from the image's coordinate
	system to the window's coordinate system.
	
	void ImageToWindow ( GWindowPtr window, short *col, short *row )
	
	(window): pointer to image window
	(col):    contains horizontal coordinate to convert
	(row):    contains vertical coordinate to convert
	
	The function returns nothing.  On input, (col) and (row) contain the
	horizontal and vertical coordinates in the image's coordinate system;
	on output, they will be converted to the horizontal and vertical coordinates
	in the window's coordinate system.  The converted coordinates may lie
	outside the window's visible boundaries; no clipping is performed by
	this function.
	
**********************************************************************************/

void ImageToWindow ( GWindowPtr window, short *col, short *row )
{
	long	imgCtrRow, imgCtrCol;
	long	imgOrgRow, imgOrgCol;
	long	winCtrRow, winCtrCol;
	long	zoom, scale;
	GRect	rect;
	
	GetImageWindowDisplayRect ( window, &rect );
	
	zoom = GetImageWindowDisplayZoom ( window );

	imgCtrRow = GetImageWindowDisplayCenterRow ( window );
	imgCtrCol = GetImageWindowDisplayCenterCol ( window );
	
	winCtrRow = ( rect.bottom + rect.top ) / 2;
	winCtrCol = ( rect.left + rect.right ) / 2;
	
	if ( zoom >= 0 )
	{
		scale = 1 << zoom;

		imgOrgRow = winCtrRow - imgCtrRow * scale;
		imgOrgCol = winCtrCol - imgCtrCol * scale;

		*col = ( *col * scale ) + imgOrgCol;
		*row = ( *row * scale ) + imgOrgRow;
	}
	else
	{
		scale = 1 << -zoom;
		
		imgOrgRow = winCtrRow - imgCtrRow / scale;
		imgOrgCol = winCtrCol - imgCtrCol / scale;

		*col = ( *col / scale ) + imgOrgCol;
		*row = ( *row / scale ) + imgOrgRow;
	}
}

/***  WindowToImage  *************************************************************

	Converts a position in an image window's image from the window's coordinate
	system to the image's coordinate system.
	
	void WindowToImage ( GWindowPtr window, short *col, short *row )
	
	(window): pointer to image window.
	(col):    contains horizontal coordinate to convert.
	(row):    contains vertical coordinate to convert.
	
	The function returns nothing.  On input, (col) and (row) contain the
	horizontal and vertical coordinates in the window's coordinate system;
	on output, they will be converted to the horizontal and vertical coordinates
	in the image's coordinate system.  The converted coordinates may lie
	outside the image's boundaries; no clipping is performed by this function.
	
**********************************************************************************/

void WindowToImage ( GWindowPtr window, short *col, short *row )
{
	long	imgCtrRow, imgCtrCol;
	long	imgOrgRow, imgOrgCol;
	long	winCtrRow, winCtrCol;
	long	zoom, scale;
	GRect	rect;
	
	GetImageWindowDisplayRect ( window, &rect );
	zoom = GetImageWindowDisplayZoom ( window );
	
	imgCtrRow = GetImageWindowDisplayCenterRow ( window );
	imgCtrCol = GetImageWindowDisplayCenterCol ( window );
	
	winCtrRow = ( rect.bottom + rect.top ) / 2;
	winCtrCol = ( rect.left + rect.right ) / 2;

	if ( zoom >= 0 )
	{
		scale = 1 << zoom;
			
		imgOrgRow = winCtrRow - imgCtrRow * scale;
		imgOrgCol = winCtrCol - imgCtrCol * scale;
		
		*col = ( *col - imgOrgCol ) / scale;
		*row = ( *row - imgOrgRow ) / scale;
	}
	else
	{
		scale = 1 << -zoom;
		
		imgOrgRow = winCtrRow - imgCtrRow / scale;
		imgOrgCol = winCtrCol - imgCtrCol / scale;
		
		*col = ( *col - imgOrgCol ) * scale;
		*row = ( *row - imgOrgRow ) * scale;
	}
}

/*** WindowToImageRect ****************************************************

	Converts a rectangle from window to image coordinates.
	
	void WindowToImageRect ( GWindowPtr window, GRectPtr rect )
	
	(window): pointer to image window.
	(rect):   pointer to rectangle containing coordinates to convert.
	
	This function is a wrapper for WindowToImage().  Use it when you
	need to convert a rectangle from window to image coordinates, rather
	than passing the addresses of the rectangle's structure members
	directly.  This function is necessitated by the fact that the Rect
	structure has different sizes and layouts on the Mac and in Windows
	(and in fact has different layouts in different flavors of Windows,
	too!)
	
***************************************************************************/

void WindowToImageRect ( GWindowPtr window, GRectPtr rect )
{
	short	left = rect->left;
	short	top = rect->top;
	short	right = rect->right;
	short	bottom = rect->bottom;
	
	WindowToImage ( window, &left, &top );
	WindowToImage ( window, &right, &bottom );
	
	rect->left = left;
	rect->top = top;
	rect->right = right;
	rect->bottom = bottom;
}

/*** ImageToWindowRect ****************************************************

	Converts a rectangle from image to window coordinates.
	
	void ImageToWindowRect ( GWindowPtr window, GRectPtr rect )
	
	(window): pointer to image window.
	(rect):   pointer to rectangle containing coordinates to convert.
	
	This function is a wrapper for ImageToWindow().  Use it when you
	need to convert a rectangle from image to window coordinates, rather
	than passing the addresses of the rectangle's structure members
	directly.  This function is necessitated by the fact that the Rect
	structure has different sizes and layouts on the Mac and in Windows
	(and in fact has different layouts in different flavors of Windows,
	too!)
	
***************************************************************************/

void ImageToWindowRect ( GWindowPtr window, GRectPtr rect )
{
	short	left = rect->left;
	short	top = rect->top;
	short	right = rect->right;
	short	bottom = rect->bottom;
	
	ImageToWindow ( window, &left, &top );
	ImageToWindow ( window, &right, &bottom );
	
	rect->left = left;
	rect->top = top;
	rect->right = right;
	rect->bottom = bottom;
}

/***  ImageXYToWindow  *************************************************************

	Converts a position in an image window's image from the window's coordinate
	system to the image's coordinate system.
	
	void ImageXYToWindow ( GWindowPtr window, float x, float y, short *col, short *row )
	
	(window): pointer to image window.
	(x):      horizontal coordinate in image's coordinate system.
	(y):      vertical coordinate in image's coordinate system.
	(col):    recieves horizontal coordinate in window's coordinate system.
	(row):    recieves vertical coordinate in window's coordinate system.
	
	The function returns nothing.  On input, (col) and (row) contain the
	horizontal and vertical coordinates in the window's coordinate system;
	on output, they will be converted to the horizontal and vertical coordinates
	in the image's coordinate system.  The converted coordinates may lie
	outside the image's boundaries; no clipping is performed by this function.
	
**********************************************************************************/

void ImageXYToWindow ( GWindowPtr window, float x, float y, short *col, short *row )
{
	long	imgCtrRow, imgCtrCol;
	long	imgOrgRow, imgOrgCol;
	long	winCtrRow, winCtrCol;
	long	zoom, scale;
	GRect	rect;
	
	GetImageWindowDisplayRect ( window, &rect );
	
	zoom = GetImageWindowDisplayZoom ( window );

	imgCtrRow = GetImageWindowDisplayCenterRow ( window );
	imgCtrCol = GetImageWindowDisplayCenterCol ( window );
	
	winCtrRow = ( rect.bottom + rect.top ) / 2;
	winCtrCol = ( rect.left + rect.right ) / 2;
	
	if ( zoom >= 0 )
	{
		scale = 1 << zoom;

		imgOrgRow = winCtrRow - imgCtrRow * scale + scale / 2;
		imgOrgCol = winCtrCol - imgCtrCol * scale + scale / 2;

		*col = ( x * scale ) + imgOrgCol;
		*row = ( y * scale ) + imgOrgRow;
	}
	else
	{
		scale = 1 << -zoom;
		
		imgOrgRow = winCtrRow - imgCtrRow / scale;
		imgOrgCol = winCtrCol - imgCtrCol / scale;

		*col = ( x / scale ) + imgOrgCol;
		*row = ( y / scale ) + imgOrgRow;
	}
}

/***  WindowToImageXY  ***********************************************************

	Converts a position in an image window's image from the window's coordinate
	system to the image's coordinate system.
	
	void WindowToImageXY ( GWindowPtr window, float x, float y, short *col, short *row )
	
	(window): pointer to image window.
	(col):    horizontal coordinate in window's coordinate system.
	(row):    vertical coordinate in window's coordinate system.
	(x):      recieves horizontal coordinate in image's coordinate system.
	(y):      recieves vertical coordinate in image's coordinate system.
	
	The function returns nothing.  On input, (col) and (row) contain the
	horizontal and vertical coordinates in the window's coordinate system;
	on output, they will be converted to the horizontal and vertical coordinates
	in the image's coordinate system.  The converted coordinates may lie
	outside the image's boundaries; no clipping is performed by this function.
	
**********************************************************************************/

void WindowToImageXY ( GWindowPtr window, short col, short row, float *x, float *y )
{
	long	imgCtrRow, imgCtrCol;
	long	imgOrgRow, imgOrgCol;
	long	winCtrRow, winCtrCol;
	long	zoom, scale;
	GRect	rect;
	
	GetImageWindowDisplayRect ( window, &rect );
	zoom = GetImageWindowDisplayZoom ( window );
	
	imgCtrRow = GetImageWindowDisplayCenterRow ( window );
	imgCtrCol = GetImageWindowDisplayCenterCol ( window );
	
	winCtrRow = ( rect.bottom + rect.top ) / 2;
	winCtrCol = ( rect.left + rect.right ) / 2;

	if ( zoom >= 0 )
	{
		scale = 1 << zoom;
			
		imgOrgRow = winCtrRow - imgCtrRow * scale + scale / 2;
		imgOrgCol = winCtrCol - imgCtrCol * scale + scale / 2;
		
		*x = ( (float) col - imgOrgCol ) / scale;
		*y = ( (float) row - imgOrgRow ) / scale;
	}
	else
	{
		scale = 1 << -zoom;
		
		imgOrgRow = winCtrRow - imgCtrRow / scale;
		imgOrgCol = winCtrCol - imgCtrCol / scale;
		
		*x = ( (float) col - imgOrgCol ) * scale;
		*y = ( (float) row - imgOrgRow ) * scale;
	}
}

#if BRIGHTNESS_CONTRAST_CONTROL

void DrawImageWindowBitmap ( GWindowPtr window )
{
	PIXEL			*imageRow, min, max;
	float			brightness, contrast;
	float			slope, offset, value;
	float			pivotX, pivotY;
	unsigned char	*bitmapRow;
	short			row, col;
	GImagePtr		bitmap = GetImageWindowBitmap ( window );
	FITSImagePtr	image = GetImageWindowFITSImage ( window );
	
	brightness = GetImageWindowDisplayMin ( window );
	contrast = GetImageWindowDisplayMax ( window );
	
	min = GetImageWindowImageMin ( window );
	max = GetImageWindowImageMax ( window );
	
	if ( max == min )
		slope = 0.0;
	else
		slope  = 255.0 * tan ( contrast * PI / 2 ) / ( max - min );

	pivotX = max - brightness * ( max - min );
	pivotY = 255.0 * brightness;
	offset = pivotY - slope * pivotX;
	
	if ( contrast == 1.0 )
	{
		for ( row = 0; row < image->naxis2; row++ )
		{
			imageRow = image->data[0][row];
			bitmapRow = GGetImageDataRow ( bitmap, row );
			
			for ( col = 0; col < image->naxis1; col++ )
			{
				if ( imageRow[col] < pivotX )
					bitmapRow[col] = 0;
				else
					bitmapRow[col] = 255;
			}
		}
	}
	else
	{
		for ( row = 0; row < image->naxis2; row++ )
		{
			imageRow = image->data[0][row];
			bitmapRow = GGetImageDataRow ( bitmap, row );
			
			for ( col = 0; col < image->naxis1; col++ )
			{
				value = slope * imageRow[col] + offset;
				
				if ( value < 0.0 )
					value = 0.0;
					
				if ( value > 255.0 )
					value = 255.0;
				
				bitmapRow[col] = value;
			}
		}
	}
}

#else

void DrawImageWindowBitmap ( GWindowPtr window )
{
	ImagePtr		image = GetImageWindowImage ( window );
	GImagePtr		bitmap = GetImageWindowBitmap ( window );
	
	if ( GetImageType ( image ) == IMAGE_TYPE_RGB_COLOR )
		DrawImageWindowRGBColorBitmap ( window );
	else
		DrawImageWindowMonochromeBitmap ( window );
}

void DrawImageWindowMonochromeBitmap ( GWindowPtr window )
{
	PIXEL			*imageRow, min, max;
	float			value, scale;
	unsigned char	*bitmapRow;
	short			row, col, rows, columns;
	ImagePtr		image = GetImageWindowImage ( window );
	GImagePtr		bitmap = GetImageWindowBitmap ( window );
	
	/*** Determine the image's dimensions. ***/ 
	
	rows = GetImageRows ( image );
	columns = GetImageColumns ( image );
	
	/*** Determine the image display range, and the scaling factor from
	     image data values to bitmap values. ***/
	     
	min = GetImageWindowDisplayMin ( window );
	max = GetImageWindowDisplayMax ( window );

	if ( max == min )
		scale = 0.0;
	else
		scale = 255.0 / ( max - min );

	for ( row = 0; row < rows; row++ )
	{
		imageRow = GetImageDataRow ( image, 0, row );
		bitmapRow = GGetImageDataRow ( bitmap, row );

		if ( scale != 0.0 )
	    {
			for ( col = 0; col < columns; col++ )
			{
				value = scale * ( imageRow[col] - min );

				if ( value < 0.0 )
					value = 0.0;
		   		if ( value > 255.0 )
					value = 255.0;

				bitmapRow[col] = value;
			}
	    }
	    else
	    {
			for ( col = 0; col < columns; col++ )
			{
            	if ( imageRow[col] < min )
                	bitmapRow[col] = 0;
                else
                	bitmapRow[col] = 255;
		    }
        }
    }
}

void DrawImageWindowRGBColorBitmap ( GWindowPtr window )
{
	PIXEL			*redRow, *greenRow, *blueRow, min, max;
	float			red, green, blue, scale;
	double			balanceRed, balanceGreen, balanceBlue;
	unsigned long	*bitmapRow;
	short			row, col, rows, columns, frame;
	GImagePtr		bitmap = GetImageWindowBitmap ( window );
	ImagePtr		image = GetImageWindowImage ( window );
	
	/*** Determine the image's dimensions ***/ 
	
	rows = GetImageRows ( image );
	columns = GetImageColumns ( image );
	
	min = GetImageWindowDisplayMin ( window );
	max = GetImageWindowDisplayMax ( window );

	if ( max == min )
		scale = 0.0;
	else
		scale = 255.0 / ( max - min );

	GetRGBBalance ( &balanceRed, &balanceGreen, &balanceBlue );
	
	frame = GetImageWindowColorFrame ( window );
	
	for ( row = 0; row < rows; row++ )
	{
		redRow    = GetImageDataRow ( image, 0, row );
		greenRow  = GetImageDataRow ( image, 1, row );
		blueRow   = GetImageDataRow ( image, 2, row );
		
		bitmapRow = GGetImageDataRow ( bitmap, row );

		for ( col = 0; col < columns; col++ )
		{
			if ( frame == COLOR_FRAME_RED || frame == COLOR_FRAME_ALL )
				red = balanceRed * redRow[col];
			else
				red = 0;
				
			if ( frame == COLOR_FRAME_GREEN || frame == COLOR_FRAME_ALL )
				green = balanceGreen * greenRow[col];
			else
				green = 0;
				
			if ( frame == COLOR_FRAME_BLUE || frame == COLOR_FRAME_ALL )
				blue = balanceBlue * blueRow[col];
			else
				blue = 0;
				
			if ( scale != 0.0 )
			{
				red   = scale * ( red - min );
				green = scale * ( green - min );
				blue  = scale * ( blue - min );

				if ( red < 0.0 )
					red = 0.0;
		   		if ( red > 255.0 )
					red = 255.0;

				if ( green < 0.0 )
					green = 0.0;
		   		if ( green > 255.0 )
					green = 255.0;

				if ( blue < 0.0 )
					blue = 0.0;
		   		if ( blue > 255.0 )
					blue = 255.0;

				bitmapRow[col] = ( ( (long) red ) << 16 ) | ( ( (long) green ) << 8 ) | ( (long) blue );
			}
			else
			{
            	if ( red < min )
                	red = 0;
                else
                	red = 255;

            	if ( green < min )
                	green = 0;
                else
                	green = 255;

            	if ( blue < min )
                	blue = 0;
                else
                	blue = 255;

				bitmapRow[col] = ( ( (long) red ) << 16 ) | ( ( (long) green ) << 8 ) | ( (long) blue );
		    }
		}
    }
}

#endif

/*** GetRGBBalance **********************************************************

	Obtains the correction factors which will be applied to RGB image
	color data before drawing a color bitmap.  These factors are normally
	set to 1.0, so that the image's R, G, and B components are weighted
	equally when creating the color bitmap.  Currently, the program only
	adjusts them when previewing an RGB color balance operation (see
	RGBBalanceImageWindow().)
	
*****************************************************************************/

static double sRedBalance = 1.0;
static double sGreenBalance = 1.0;
static double sBlueBalance = 1.0;

void GetRGBBalance ( double *red, double *green, double *blue )
{
	*red = sRedBalance;
	*green = sGreenBalance;
	*blue = sBlueBalance;
}

void SetRGBBalance ( double red, double green, double blue )
{
	sRedBalance = red;
	sGreenBalance = green;
	sBlueBalance = blue;
}

void SetBitmapColorTableGrayscale ( GImagePtr bitmap )
{
	short	index, red, green, blue;

	for ( index = 0; index < 256; index++ )
	{
		red = index;
		green = index;
		blue = index;

		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableRed ( GImagePtr bitmap )
{
	short	index, red, green, blue;
	
	for ( index = 0; index < 256; index++ )
	{
		red = index;
		green = 0;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableGreen ( GImagePtr bitmap )
{
	short	index, red, green, blue;
	
	for ( index = 0; index < 256; index++ )
	{
		red = 0;
		green = index;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableBlue ( GImagePtr bitmap )
{
	short	index, red, green, blue;
	
	for ( index = 0; index < 256; index++ )
	{
		red = 0;
		green = 0;
		blue = index;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableWinter ( GImagePtr bitmap )
{
	short	index, red, green, blue;
	
	for ( index = 0; index <= 85; index++ )
	{
		red = 0;
		green = 0;
		blue = index * 3;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 85; index <= 170; index++ )
	{
		red = 0;
		green = ( index - 85 ) * 3;
		blue = 255;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 170; index <= 255; index++ )
	{
		red = ( index - 170 ) * 3;
		green = 255;
		blue = 255;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableSpring ( GImagePtr bitmap )
{
	short	index, red, green, blue;
	
	for ( index = 0; index < 85; index++ )
	{
		red = index * 3;
		green = 0;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 85; index <= 170; index++ )
	{
		red = 255;
		green = 0;
		blue = ( index - 85 ) * 3;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 170; index <= 255; index++ )
	{
		red = 255;
		green = ( index - 170 ) * 3;
		blue = 255;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableSummer ( GImagePtr bitmap )
{
	short	index, red, green, blue;
	
	for ( index = 0; index <= 85; index++ )
	{
		red = 0;
		green = index * 3;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 85; index <= 170; index++ )
	{
		red = ( index - 85 ) * 3;
		green = 255;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 170; index <= 255; index++ )
	{
		red = 255;
		green = 255;
		blue = ( index - 170 ) * 3;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableFall ( GImagePtr bitmap )
{
	short	index, red, green, blue;
	
	for ( index = 0; index <= 85; index++ )
	{
		red = index * 3;
		green = 0;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 85; index <= 170; index++ )
	{
		red = 255;
		green = ( index - 85 ) * 3;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 170; index <= 255; index++ )
	{
		red = 255;
		green = 255;
		blue = ( index - 170 ) * 3;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableRainbow ( GImagePtr bitmap )
{
	short	index, red, green, blue;

	for ( index = 0; index <= 51; index++ )
	{
		red = ( 51 - index ) * 5;
		green = 0;
		blue = 255;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 51; index <= 102; index++ )
	{
		red = 0;
		green = ( index - 51 ) * 5;
		blue = 255;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 102; index <= 153; index++ )
	{
		red = 0;
		green = 255;
		blue = ( 153 - index ) * 5;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 153; index <= 204; index++ )
	{
		red = ( index - 153 ) * 5;
		green = 255;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 204; index <= 255; index++ )
	{
		red = 255;
		green = ( 255 - index ) * 5;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

void SetBitmapColorTableFlame ( GImagePtr bitmap )
{
	short	index, red, green, blue;
	
	for ( index = 0; index < 64; index++ )
	{
		red = 0;
		green = 0;
		blue = index * 4;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 64; index < 128; index++ )
	{
		red = ( index - 64 ) * 4;
		green = 0;
		blue = ( 127 - index ) * 4;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 128; index < 192; index++ )
	{
		red = 255;
		green = ( index - 128 ) * 4;
		blue = 0;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}

	for ( index = 192; index < 256; index++ )
	{
		red = 255;
		green = 255;
		blue = ( index - 192 ) * 4;
		
		GSetImageColorTableEntry ( bitmap, index, red, green, blue );
	}
}

/*** ComputeImageDisplayRange *********************************************

	Given a pointer to an image window, computes the image's basic
	statistics (minimum, maximum, mean, and standard deviation) and
	computes a suggested image display range.

	void ComputeImageDisplayRange ( GWindowPtr window )
	
	(window): pointer to image window.
	
	The function returns nothing.
	
***************************************************************************/

void ComputeImageDisplayRange ( GWindowPtr window )
{
	ImagePtr	image = GetImageWindowImage ( window );
	PIXEL		min, max;
	
	/*** Compute the image's statistics and histogram. ***/
	     
	ComputeImageStatistics ( image );

	/*** If the image's display mode is set to "Automatic", set the image's
	     display range based upon the statistical distribution of the
	     image data. ***/
	     
	if ( GetImageWindowDisplayMode ( window ) == DISPLAY_MODE_AUTOMATIC )
	{
		min = GetImageMinimumValue ( image );
		max = GetImageMaximumValue ( image );
	
#if BRIGHTNESS_CONTRAST_CONTROL
		SetImageWindowDisplayMin ( window, 0.5 );
		SetImageWindowDisplayMax ( window, 0.5 );
#else

		/*** If all the image data are in the range 0-255,
		     assume we have an 8-bit image, so display the
		     data at the full 8-bit output of the monitor. ***/
		     
		if ( FALSE /* let's not actually do this ... min >= 0 && min <= 255 && max >= 0 && max <= 255 */ )
		{
			SetImageWindowDisplayMin ( window, 0 );
			SetImageWindowDisplayMax ( window, 255 );
		}
		else
		{
			/*** Otherwise, find the values corresponding to the 1st and 99th
			     percentiles of the image data.  If we scale the data between
			     them to the 8-bit bitmap values shown on the screen, only the
			     brightest 2% of the image will be displayed as white, and the
			     faintest 2% displayed as black; the other 96% of the image
			     will appear as some intermediate level.  This handles the vast
			     majority of deep-sky, lunar, and planetary images beautifully. ***/
			     
			min = GetImagePercentile ( image, 2.0 );
			max = GetImagePercentile ( image, 98.0 );
		
			/*** If the 2nd and 98th percentiles are identical, set the image
			     display range to the entire range of the image data, so that
			     the user at least sees something.  (This can happen with planetary
			     images, which contain mostly black sky, and only a small fraction
			     of interesting image data.) ***/
			     
			if ( min == max )
			{
				min = GetImageMinimumValue ( image );
				max = GetImageMaximumValue ( image );
			}

			SetImageWindowDisplayMin ( window, min );
			SetImageWindowDisplayMax ( window, max );
		}
#endif
	}
}

