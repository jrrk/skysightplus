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

struct ImageRegion
{
	short			type;
	short			left;
	short			top;
	short			right;
	short			bottom;
	char			name[48];
	long			flags;
	double			params[IMAGE_REGION_MAX_PARAMS];
	double			errors[IMAGE_REGION_MAX_PARAMS];
	ImageRegionPtr	next;
};

/*** local functions ***/

void CompleteImageWindowSelection ( GWindowPtr, ImageRegionPtr );

/***  NewImageRegion  **********************************************************/

ImageRegionPtr NewImageRegion ( short type, short left, short top, short right,
short bottom )
{
	ImageRegionPtr	region = (ImageRegionPtr) malloc ( sizeof ( ImageRegion ) );
	
	if ( region != NULL )
	{
		memset ( region, 0, sizeof ( ImageRegion ) );
		
		region->type = type;
		
		if ( type == IMAGE_REGION_TYPE_LINEAR )
		{
			region->left = left;
			region->top = top;
			region->right = right;
			region->bottom = bottom;
		}
		else
		{
			region->left = left < right ? left : right;
			region->top = top < bottom ? top : bottom;
			region->right = left < right ? right : left;
			region->bottom = top < bottom ? bottom : top;
		}
	}
	
	return ( region );
}

/***  DeleteImageRegion  ********************************************************/

void DeleteImageRegion ( ImageRegionPtr region )
{
	free ( region );
}

/***  GetImageRegionType  *******************************************************/

short GetImageRegionType ( ImageRegionPtr region )
{
	return ( region->type );
}

/***  GetImageRegionLeft  *******************************************************/

short GetImageRegionLeft ( ImageRegionPtr region )
{
	return ( region->left );
}

/***  GetImageRegionTop  *******************************************************/

short GetImageRegionTop ( ImageRegionPtr region )
{
	return ( region->top );
}

/***  GetImageRegionRight  *******************************************************/

short GetImageRegionRight ( ImageRegionPtr region )
{
	return ( region->right );
}

/***  GetImageRegionBottom  *****************************************************/

short GetImageRegionBottom ( ImageRegionPtr region )
{
	return ( region->bottom );
}

/***  GetImageRegionWidth  ***************************************************/

short GetImageRegionWidth ( ImageRegionPtr region )
{
	return ( region->right - region->left + 1 );
}

/***  GetImageRegionHeight  **************************************************/

short GetImageRegionHeight ( ImageRegionPtr region )
{
	return ( region->bottom - region->top + 1 );
}

/*** GetNextImageRegion *********************************************************/

ImageRegionPtr GetNextImageRegion ( ImageRegionPtr region )
{
	return ( region->next );
}

/***  GetImageRegionName  *******************************************************/

char *GetImageRegionName ( ImageRegionPtr region )
{
	return ( region->name );
}

/***  GetImageRegionFlag  *******************************************************/

int GetImageRegionFlag ( ImageRegionPtr region, long flag )
{
	if ( region->flags & flag )
		return ( TRUE );
	else
		return ( FALSE );
}

/***  GetImageRegionParameter  *************************************************/

double GetImageRegionParameter ( ImageRegionPtr region, short i )
{
	if ( i >= 0 && i < IMAGE_REGION_MAX_PARAMS )
		return ( region->params[i] );
	else
		return ( 0.0 );
}

/***  GetImageRegionError  *************************************************/

double GetImageRegionError ( ImageRegionPtr region, short i )
{
	if ( i >= 0 && i < IMAGE_REGION_MAX_PARAMS )
		return ( region->errors[i] );
	else
		return ( 0.0 );
}

/*** SetNextImageRegion *******************************************************/

void SetNextImageRegion ( ImageRegionPtr region, ImageRegionPtr next )
{
	region->next = next;
}

/***  SetImageRegionName  *****************************************************/

void SetImageRegionName ( ImageRegionPtr region, char *name )
{
	strncpy ( region->name, name, sizeof ( region->name ) - 1 );
}

/***  SetImageRegionFlag  *****************************************************/

void SetImageRegionFlag ( ImageRegionPtr region, long flag, int value )
{
	if ( value )
		region->flags |=  flag;
	else
		region->flags &= ~flag;
}

/***  SetImageRegionParameter  **********************************************/

void SetImageRegionParameter ( ImageRegionPtr region, short i, double param )
{
	if ( i >= 0 && i < IMAGE_REGION_MAX_PARAMS )
		region->params[i] = param;
}

/***  SetImageRegionError  ****************************************************/

void SetImageRegionError ( ImageRegionPtr region, short i, double error )
{
	if ( i >= 0 && i < IMAGE_REGION_MAX_PARAMS )
		region->errors[i] = error;
}

/***  GetImageRegionSegment  ****************************************************/

int GetImageRegionSegment ( ImageRegionPtr region, short *row, short *left, short *right )
{
	int result;
	
	switch ( region->type )
	{
		case IMAGE_REGION_TYPE_RECTANGULAR:
			if ( *row < 0 )
				*row = region->top;
			else
				*row += 1;

			*left = region->left;
			*right = region->right;
			
			if ( *row >= region->top && *row <= region->bottom )
				result = TRUE;
			else
				result = FALSE;
			break;		

		default:
			result = FALSE;
			break;
	}
	
	return ( result );
}

/***  IsPixelInImageRegion  ******************************************************/

int IsPixelInImageRegion ( ImageRegionPtr region, short col, short row )
{
	if ( row < region->top || row > region->bottom || col < region->left || col > region->right )
		return ( FALSE );

	switch ( region->type )
	{
		case IMAGE_REGION_TYPE_RECTANGULAR:
			return ( TRUE );
			
		default:
			return ( FALSE );
	}
}

/***  DrawImageRegion  *****************************************************/

void DrawImageRegion ( GWindowPtr window, ImageRegionPtr region )
{
	short			type, left, top, right, bottom;
	GRect			rect;

	type = GetImageRegionType ( region );
	
	/*** Find the image region's bounding coordinates in the window's
	     coordinate system.  For rectangular and elliptical regions,
	     we need to add one pixel to the right and bottom coordinates
	     because the native OS will only draw rectangles/ellipses up
	     to those coordinates, not including them. ***/
	
	left = GetImageRegionLeft ( region );
	top = GetImageRegionTop ( region );

	if ( type == IMAGE_REGION_TYPE_LINEAR )
	{
		right = GetImageRegionRight ( region );
		bottom = GetImageRegionBottom ( region );
	}
	else
	{
		right = GetImageRegionRight ( region ) + 1;
		bottom = GetImageRegionBottom ( region ) + 1;
	}
	
	ImageToWindow ( window, &left, &top );
	ImageToWindow ( window, &right, &bottom );
	GSetRect ( &rect, left, top, right, bottom );
	GSetPenFill ( FALSE );
	
	if ( type == IMAGE_REGION_TYPE_LINEAR )
		GDrawLine ( left, top, right, bottom );
	else if ( type == IMAGE_REGION_TYPE_RECTANGULAR )
		GDrawRectangle ( left, top, right, bottom );
	else if ( type == IMAGE_REGION_TYPE_ELLIPTICAL )
		GDrawEllipse ( left, top, right, bottom );
}

/*** SelectImageRegion *********************************************************/

void SelectImageRegion ( GWindowPtr window, short col1, short row1 )
{
	short			col, row, col2, row2, colMax, rowMax, colMouse, rowMouse;
	short			mode;
	ImageRegionPtr	region;
	ImagePtr		image = GetImageWindowImage ( window );
	
	/*** Convert the position where the mouse went down from window
	     to image coordinates.  Then determine the maximum row and column
	     that can be selected into that region. ***/
	     
	WindowToImage ( window, &col1, &row1 );
	
	colMax = GetImageColumns ( image ) - 1;
	rowMax = GetImageRows ( image ) - 1;
	
	/*** If the mouse was clicked outside the selectable part of the image,
	     return without creating a new region.  Otherwise, save the current
	     pen mode, then set the pen mode to inverting mode so that we will
	     erase previous image regions when we redraw them. ***/
	     
	if ( col1 > colMax || row1 > rowMax || col1 < 0 || row1 < 0 )
		return;
		
	GStartDrawing ( window );
	mode = GGetPenMode();
	GSetPenMode ( G_INVERTING_PEN );
	
	/*** If the image window already has a selected image region,
	     draw it in inverting mode in order to erase it.  Then delete
	     the current selected region. ***/
	     
	region = GetImageWindowSelectedRegion ( window );
	if ( region != NULL )
	{
		DrawImageRegion ( window, region );
		SetImageWindowSelectedRegion ( window, NULL );
	}
	
	col2 = col1;
	row2 = row1;
	region = NewImageRegion ( IMAGE_REGION_TYPE_RECTANGULAR, col1, row1, col2, row2 );
	if ( region != NULL )
		DrawImageRegion ( window, region );
	
	/*** While the mouse button is down, get the mouse's coordinates,
	     convert them from window to image coordinates, and clip them
	     if they lie outside the image's boundaries.  If the coordinates
	     have changed, redraw the current selected region in XOR mode
	     to erase it, then draw the new selection. ***/
	
	while ( GMouseButtonIsDown ( 1 | 2 ) )
	{
		GGetMousePosition ( window, &colMouse, &rowMouse );
		
		col = colMouse;
		row = rowMouse;
		WindowToImage ( window, &col, &row );
		
		if ( col < 0 )
			col = 0;
		if ( row < 0 )
			row = 0;
		if ( col > colMax )
			col = colMax;
		if ( row > rowMax )
			row = rowMax;
		
		if ( col != col2 || row != row2 )
		{
			DrawImageRegion ( window, region );
			DeleteImageRegion ( region );

			col2 = col;
			row2 = row;
			region = NewImageRegion ( IMAGE_REGION_TYPE_RECTANGULAR, col1, row1, col2, row2 );
			if ( region != NULL )
				DrawImageRegion ( window, region );
				
			ShowPixelValues ( window, colMouse, rowMouse );
			DrawImageWindowMouseCoordinates ( window, colMouse, rowMouse );
		}
	}

	/*** When the mouse button is released, restore the pen mode, and end drawing. ***/
	
	GSetPenMode ( mode );
	GEndDrawing ( window );

	/*** Set the image window's selected region  to the region that was most recently
	     drawn while the mouse button was down.  Then compute and display the image
	     region's statistics. ***/
	     
	CompleteImageWindowSelection ( window, region );
}

/*** SelectAllOfImage ***/

void SelectAllOfImage ( GWindowPtr window )
{
	short 			right, bottom, mode;
	ImageRegionPtr	region;
	ImagePtr		image = GetImageWindowImage ( window );
	
	/*** Save the current pen mode, then start drawing in inverting mode. ***/
	
	GStartDrawing ( window );
	mode = GGetPenMode();
	GSetPenMode ( G_INVERTING_PEN );

	/*** If the image window already has a selected image region,
	     draw it in inverting mode in order to erase it. ***/
	     
	region = GetImageWindowSelectedRegion ( window );
	if ( region != NULL )
		DrawImageRegion ( window, region );

	/*** Create a new rectangular region containing the entire image,
	     and draw it. ***/
	
	right  = GetImageColumns ( image ) - 1;
	bottom = GetImageRows ( image ) - 1;
	region = NewImageRegion ( IMAGE_REGION_TYPE_RECTANGULAR, 0, 0, right, bottom );
	if ( region != NULL )
		DrawImageRegion ( window, region );
	
	/*** Restore the previous pen mode before ending drawing. ***/
	
	GSetPenMode ( mode );
	GEndDrawing ( window );

	CompleteImageWindowSelection ( window, region );
}

/*** CompleteImageWindowSelection ***/

void CompleteImageWindowSelection ( GWindowPtr window, ImageRegionPtr region )
{
	ImagePtr		image = GetImageWindowImage ( window );

	/*** Designate the new region as the image's selected region, 
	     then compute its statistics and display them. ***/
	     
	SetImageWindowSelectedRegion ( window, region );
	ComputeImageRegionStatistics ( window, region );
	ShowImageRegionStatistics ( region );

	/*** If the image window has an associated histogram window, compute the histogram
	     of the selected region, then invalidate the window so the histogram gets redrawn. ***/
	
	window = GetImageHistogramWindow ( image );
	if ( window != NULL )
	{
		ComputeHistogramWindowHistogram ( window );
		GInvalidateWindow ( window, NULL );
	}	
}

/*** SelectNoneOfImage ***/

void SelectNoneOfImage ( GWindowPtr window )
{
	short			mode;
	ImageRegionPtr	region;
	
	/*** If the image window already has a selected region, draw it in
	     inverting mode in order to erase it. ***/
	     
	region = GetImageWindowSelectedRegion ( window );
	if ( region != NULL )
	{
		GStartDrawing ( window );
		mode = GGetPenMode();
		GSetPenMode ( G_INVERTING_PEN );
		DrawImageRegion ( window, region );
		GSetPenMode ( mode );
		GEndDrawing ( window );
	}
	
	/*** Set the image window's selected region to NULL, which will delete
	     the previously-selected region.  Update the image region statistic
	     display to reflect this. ***/
	     
	SetImageWindowSelectedRegion ( window, NULL );
	ShowImageRegionStatistics ( NULL );
}

/***  ComputeImageObjectCentroid  *********************************************/

void ComputeImageObjectCentroid ( ImageObjectPtr object, PIXEL **matrix )
{
	short		row = -1, col, left, right;
	double		sum = 0.0;
	double		sumRow = 0.0;
	double		sumCol = 0.0;
	double		sumRow2 = 0.0;
	double		sumCol2 = 0.0;
	double		x, y, moment;
	PIXEL		value, background;
	
	background = GetImageObjectBackground ( object );
	
	while ( GetImageRegionSegment ( object, &row, &left, &right ) )
	{
		for ( col = left; col < right; col++ )
		{
			value    = matrix[row][col] - background;
			
			sum     += value;
			sumRow  += value * (long) row;
			sumCol  += value * (long) col;
			sumRow2 += value * (long) row * row;
			sumCol2 += value * (long) col * col;
		}	
	}
	
	if ( sum != 0.0 )
	{
		x = sumCol / sum;
		y = sumRow / sum;
		
		sumRow2 = sumRow2 - 2 * y * sumRow + y * y * sum;
		sumCol2 = sumCol2 - 2 * x * sumCol + x * x * sum;
		
		moment = sqrt ( ( sumRow2 + sumCol2 ) / sum );
		
		SetImageObjectCentroidX ( object, x );
		SetImageObjectCentroidY ( object, y );
		SetImageObjectRadius ( object, moment );
	}
}

/*** ComputeImageRegionStatistics ***********************************************/

void ComputeImageRegionStatistics ( GWindowPtr window, ImageRegionPtr region )
{
	ImagePtr	image = GetImageWindowImage ( window );
	PIXEL		min, max, value, *data;
	short		col, row = -1, left, right;
	short		minRow, minCol, maxRow, maxCol;
	double		sum, sum2, mean, stdev;
	long		n;
	
	min = PIXEL_MAX;
	max = PIXEL_MIN;
	sum = sum2 = n = 0;
	
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
	{
		data = GetImageDataRow ( image, 0, row );
		
		for ( col = left; col <= right; col++ )
		{
			value = data[col];
			
			if ( value < min )
			{
				min = value;
				minCol = col;
				minRow = row;
			}
			
			if ( value > max )
			{
				max = value;
				maxCol = col;
				maxRow = row;
			}
			
			sum += value;
			sum2 += value * value;
			n++;
		}
	}
	
	if ( n > 0 )
		mean = sum / n;
	else
		mean = 0.0;
		
	if ( n > 1 )
		stdev = sqrt ( ( sum2 - 2.0 * sum * mean + n * mean * mean ) / ( n - 1 ) );
	else
		stdev = 0.0;
		
	SetImageRegionNumPixels ( region, n );
	SetImageRegionSum ( region, sum );
	SetImageRegionMin ( region, min );
	SetImageRegionMax ( region, max );
	SetImageRegionMinCol ( region, minCol );
	SetImageRegionMinRow ( region, minRow );
	SetImageRegionMaxCol ( region, maxCol );
	SetImageRegionMaxRow ( region, maxRow );
	SetImageRegionMean ( region, mean );
	SetImageRegionStdDev ( region, stdev );
}

