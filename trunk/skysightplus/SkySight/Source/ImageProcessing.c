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

/***************************  DuplicateImage  ********************************/

void DuplicateImage ( PIXEL **dest, PIXEL **src, ImageRegionPtr region )
{
	short row = -1, col, left, right;
	
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
		for ( col = left; col <= right; col++ )
			dest[row][col] = src[row][col];
}

/**************************  FlipImageRows  *****************************/

void FlipImageRows ( PIXEL **dest, PIXEL **src, ImageRegionPtr region )
{
	short	row = -1, col, left, right, middle, end;
	
	left = GetImageRegionLeft ( region );
	right = GetImageRegionRight ( region );

	middle = ( left + right ) / 2;
	end = ( left + right ) - 1;
	 
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
		for ( col = left; col <= right; col++ )
			dest[row][end - col] = src[row][col];
}

/**************************  FlipImageCols  ******************************/

void FlipImageCols ( PIXEL **dest, PIXEL **src, ImageRegionPtr region )
{
	short	row = -1, col, left, right, middle, end, top, bottom;
	
	top = GetImageRegionTop ( region );
	bottom = GetImageRegionBottom ( region );

	middle = ( bottom + top ) / 2;
	end = ( bottom + top ) - 1;
	 
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
		for ( col = left; col <= right; col++ )
			dest[end - row][col] = src[row][col];
}

/**************************  InvertImage  ********************************

void InvertImage ( PIXEL **dest, PIXEL **src, ImageRegionPtr region )
{
	short	row = -1, col, left, right;
	PIXEL	min, max, range;
	
	ComputeImageRegionMinMax ( region, src, &min, &max, NULL, NULL, NULL, NULL );
	max = max + min;
	
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
		for ( col = left; col < right; col++ )
			dest[row][col] = max - src[row][col];
}

**/

/*****************************  AddImage  ************************************/

void AddImage ( PIXEL **dest, PIXEL **src, PIXEL constant, ImageRegionPtr region )
{
	short		row = -1, col, left, right;
	
	if ( src )
	{
		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
			for ( col = left; col < right; col++ )
				dest[row][col] += src[row][col];
	}
	else
	{
		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
			for ( col = left; col < right; col++ )
				dest[row][col] += constant;
	}
}

/*****************************  SubtractImage  ************************************/

void SubtractImage ( PIXEL **dest, PIXEL **src, PIXEL constant, ImageRegionPtr region )
{
	short		row = -1, col, left, right;
	
	if ( src )
	{
		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
			for ( col = left; col < right; col++ )
				dest[row][col] -= src[row][col];
	}
	else
	{
		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
			for ( col = left; col < right; col++ )
				dest[row][col] -= constant;
	}
}

/*****************************  MultiplyImage  ************************************/

void MultiplyImage ( PIXEL **dest, PIXEL **src, PIXEL constant, ImageRegionPtr region )
{
	short		row = -1, col, left, right;
	
	if ( src )
	{
		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
			for ( col = left; col < right; col++ )
				dest[row][col] *= src[row][col];
	}
	else
	{
		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
			for ( col = left; col < right; col++ )
				dest[row][col] *= constant;
	}
}

/*****************************  DivideImage  ************************************/

void DivideImage ( PIXEL **dest, PIXEL **src, PIXEL constant, ImageRegionPtr region )
{
	short		row = -1, col, left, right, step;
	
	if ( src )
	{
		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
			for ( col = left; col < right; col += step )
				if ( src[row][col] )
					dest[row][col] /= src[row][col];
				else
					dest[row][col] = 0;
	}
	else
	{
		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
			for ( col = left; col < right; col++ )
				if ( constant )
					dest[row][col] /= constant;
				else
					dest[row][col] = 0;
	}
}

/************************  LogarithmicScaleImage  *******************************/

void LogarithmicScaleImage ( PIXEL **matrix, double base, ImageRegionPtr region )
{
	short		row = -1, col, left, right;
	double		logbase = log ( base );
	
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
		for ( col = left; col < right; col++ )
			if ( matrix[row][col] > 0.0 )
				matrix[row][col] = log ( matrix[row][col] ) / logbase;
			else
				matrix[row][col] = 0.0;
}

/***********************  ExponentialScaleImage  ********************************/

void ExponentialScaleImage ( PIXEL **matrix, double base, ImageRegionPtr region )
{
	short		row = -1, col, left, right;
	
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
		for ( col = left; col < right; col++ )
			matrix[row][col] = pow ( base, matrix[row][col] );
}

/**************************  PowerScaleImage  *********************************/

void PowerScaleImage ( PIXEL **matrix, double exponent, ImageRegionPtr region )
{
	short		row = -1, col, left, right;
	
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
		for ( col = left; col < right; col++ )
			matrix[row][col] = pow ( matrix[row][col], exponent );
}

/***  DoImageWindowArithmetic  *********************************************************/

void DoImageWindowArithmetic ( short operation, GWindowPtr window1, GWindowPtr window2, PIXEL constant )
{
	short		nframes, nrows, ncols;
	short		frame, row = -1, col, left, right;
	ImagePtr	image1 = NULL, image2 = NULL;
	PIXEL		*data1 = NULL, *data2 = NULL;
	
	/*** Get a pointer to the target image window's FITS image data and its
	     image data matrix, and determine the dimensions of its matrix. ***/
	     
	image1  = GetImageWindowImage ( window1 );
	nframes = GetImageFrames ( image1 );
	nrows   = GetImageRows ( image1 );
	ncols   = GetImageColumns ( image1 );
	
	/*** If there is a second image window invoved in this arithmetic operation,
	     get pointers to its FITS image data and matrix.  Choose the dimensions
	     of the smaller of the two images. ***/
	
	if ( window2 )
	{
		image2 = GetImageWindowImage ( window2 );

		if ( GetImageColumns ( image2 ) < ncols )
			ncols = GetImageColumns ( image2 );
			
		if ( GetImageRows ( image2 ) )
			nrows = GetImageRows ( image2 );
			
		if ( GetImageFrames ( image2 ) )
			nframes = GetImageFrames ( image2 );
	}
	
	/*** For each pixel in the target image, perform the desired arithmetic
	     operation with the corresponding pixel in the other image (if there
	     is one) or with the specified constant (if there is none). ***/
	
	for ( frame = 0; frame < nframes; frame++ )
	{
		for ( row = 0; row < nrows; row++ )
		{
			data1 = GetImageDataRow ( image1, frame, row );
			
			if ( operation == IMAGE_ADDITION )
			{
				if ( window2 )
				{
					data2 = GetImageDataRow ( image2, frame, row );
					for ( col = 0; col < ncols; col++ )
						data1[col] += data2[col];
				}
				else
				{
					for ( col = 0; col < ncols; col++ )
						data1[col] += constant;
				}
			}
			else if ( operation == IMAGE_SUBTRACTION )
			{
				if ( window2 )
				{
					data2 = GetImageDataRow ( image2, frame, row );
					for ( col = 0; col < ncols; col++ )
						data1[col] -= data2[col];
				}
				else
				{
					for ( col = 0; col < ncols; col++ )
						data1[col] -= constant;
				}
			}
			else if ( operation == IMAGE_MULTIPLICATION )
			{
				if ( window2 )
				{
					data2 = GetImageDataRow ( image2, frame, row );
					for ( col = 0; col < ncols; col++ )
						data1[col] *= data2[col];
				}
				else
				{
					for ( col = 0; col < ncols; col++ )
						data1[col] *= constant;
				}
			}
			else if ( operation == IMAGE_DIVISION )
			{
				if ( window2 )
				{
					data2 = GetImageDataRow ( image2, frame, row );
					for ( col = 0; col < ncols; col++ )
						if ( data2[col] == 0.0 )
							data1[col] = 0.0;
						else
							data1[col] /= data2[col];
				}
				else
				{
					if ( constant == 0.0 )
						for ( col = 0; col < ncols; col++ )
							data1[col] = 0.0;
					else
						for ( col = 0; col < ncols; col++ )
							data1[col] /= constant;
				}
			}
		}
	}

	/*** Perform any window updating that needs to be done, now that the
	     underlying image data has changed. ***/

	UpdateImage ( image1 );
}

/*** InterpolatePixelValue ***/

PIXEL InterpolatePixelValue ( PIXEL **frame, short ncols, short nrows, double x, double y )
{
	PIXEL 	value;
	short	col, row;
	
	if ( x < 0 || y < 0 || x >= ncols || y >= nrows )
	{
		value = 0;
	}
	else
	{
		col = x;
		row = y;
		
		value = frame[ row ][ col ];
	}

	return ( value );
}
