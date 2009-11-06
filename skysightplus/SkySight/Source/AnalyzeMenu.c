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
#include "define.h"
#include "types.h"

/*** local data ***/

static double	sImageObjectRadius = 0.0;

/*** local functions ***/

int IsLocalMaximum ( PIXEL **, short, short, short, short );
void ComputeImageRegionBackground ( PIXEL **, ImageRegionPtr, double, double *, double * );
void ComputePixelBackground ( PIXEL **, short, short, short, short, short, short, double, double *, double * );

/*** DoAnalyzeMenuItem ***/

void DoAnalyzeMenuItem ( long item )
{
	GWindowPtr window = GetActiveImageWindow();
	
	switch ( item )
	{
		case ANALYZE_DEFINE_OBJECT_PSF:
			DefineImageWindowImageObjectPSF ( window );
			break;
			
		case ANALYZE_FIND_OBJECTS:
			FindImageWindowImageObjects ( window, PIXEL_MIN, PIXEL_MAX, 1.3 );
			break;
			
		case ANALYZE_DELETE_OBJECTS:
			DeleteImageWindowImageObjects ( window );
			break;
			
	}
}

void DefineImageWindowImageObjectPSF ( GWindowPtr window )
{
	ImagePtr		image;
	ImageRegionPtr	region;
	ImageObjectPtr	object;
	PIXEL			**matrix;
	double			min, max, params[5], errors[5];
	
	image = GetImageWindowImage ( window );
	matrix = GetImageDataFrame ( image, 0 );
	region = GetImageWindowSelectedRegion ( window );
	if ( region == NULL )
	{
		GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "Can't define object profile because you haven't selected any part of the image.  Select an area around a well-defined object, then choose this command again." );
		return;
	}
	
	/*** Find the maximum and minimum pixel value (and location thereof)
	     within the selected image region.  Use the minimum value as a
	     first guess at the background level, and the difference between
	     the maximum and as a first guess at the amplitude.  Then compute
	     the first estimates of the object's centroid position and moment
	     (i.e. one-sigma radius) using the "center of light" method. ***/
	     
	ComputeImageRegionStatistics ( window, region );
	
	min = GetImageRegionMin ( region );
	max = GetImageRegionMax ( region );
	
	SetImageObjectBackground ( region, min );
	SetImageObjectAmplitude ( region, max - min );
	
	ComputeImageObjectCentroid ( region, matrix );

	/*** Using the statistical estimates arrived at above as starting values
	     for the object's background, ampltiude, centroid, and radius, attempt
	     to fit a Gaussian to the image data within the selected image region.
	     On failure, display a warning message. ***/
	
	params[0] = GetImageObjectBackground ( region );
	params[1] = GetImageObjectAmplitude ( region );
	params[2] = GetImageRegionMaxCol ( region );
	params[3] = GetImageRegionMaxRow ( region );
	params[4] = GetImageObjectRadius ( region );
	
	if ( FitImageModel ( matrix, region, PIXEL_MIN, PIXEL_MAX, GaussianStarModel, 5, params, errors, 0.0001, 100 ) == FALSE )
	{
		GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "Can't fit object model to image data in the selected region.  Try selecting a different image region." );
		return;
	}
	
	/*** Create an image object centered on the best-fit centroid position,
	     with a size equal to the best-fit radius. ***/
	     
	object = CreateImageObject ( image, params[2], params[3], params[4] );
	if ( object != NULL )
	{
		SetImageObjectBackground ( object, params[0] );
		SetImageObjectAmplitude  ( object, params[1] );
		SetImageObjectCentroidX  ( object, params[2] );
		SetImageObjectCentroidY  ( object, params[3] );
		SetImageObjectRadius     ( object, params[4] );
	
		if ( FitImageModel ( matrix, object, PIXEL_MIN, PIXEL_MAX, GaussianStarModel, 5, params, errors, 0.0001, 100 ) == FALSE )
		{
			GDoAlert ( G_INFO_ALERT, G_OK_ALERT, "Can't fit object model to image data in the selected region.  Try selecting a different image region." );
			return;
		}

		AddImageObject ( image, object );
		GInvalidateWindow ( window, NULL );
		
		if ( GDoAlert ( G_INFO_ALERT, G_OK_CANCEL_ALERT, "Did SkySight locate the object correctly?  If so, press \"OK\" to accept the result as a typical object profile." ) == G_OK_BUTTON )
		{
			sImageObjectRadius = GetImageObjectRadius ( object );
		}
		else
		{
			RemoveImageObject ( image, object );
			DeleteImageObject ( object );
			GInvalidateWindow ( window, NULL );
		}
	}
}

#if 1

#define		OBJ_CROWDED	0x0001
#define		OBJ_MERGED	0x0002
#define		OBJ_SATUR	0x0004
#define		OBJ_TRUNC	0x0008
#define		OBJ_APERT_PB	0x0010
#define		OBJ_ISO_PB	0x0020
#define		OBJ_DOVERFLOW	0x0040
#define		OBJ_OVERFLOW	0x0080

void compute_myotherparams(objstruct *obj, float isomag)
  {
	char linbuf[256];
	ImagePtr		image;
	ImageObjectPtr	object;
	double			min, max, background = obj->bkg, amplitude = obj->apermag, maxcol = obj->mx, maxrow = obj->my;
	double radius = sqrt(pow(obj->xmax-obj->xmin,2.0)+pow(obj->ymax-obj->ymin,2.0))/2.0;

	GWindowPtr saved_window = GetActiveImageWindow();

	image = GetImageWindowImage ( saved_window );
			
	/*** Create an image object centered on the best-fit centroid position,
	     with a size equal to the best-fit radius. ***/
	     
	object = CreateImageObject ( image, maxcol, maxrow, radius );
	if ( object != NULL )
	{
		SetImageObjectBackground ( object, background );
		SetImageObjectAmplitude  ( object, amplitude );
		SetImageObjectCentroidX  ( object, maxcol );
		SetImageObjectCentroidY  ( object, maxrow );
		SetImageObjectRadius     ( object, radius );
	
		AddImageObject ( image, object );
		GInvalidateWindow ( saved_window, NULL );
		
			sImageObjectRadius = GetImageObjectRadius ( object );
	}
  sprintf(linbuf, "%8d %6.1f %6.1f %5.1f %5.1f %7.3f "
			"%c%c%c%c%c%c%c%c\n",
	obj->number, obj->mx, obj->my,
	obj->a, obj->b,
	isomag,
	obj->flag&OBJ_CROWDED?'C':'_',
	obj->flag&OBJ_MERGED?'M':'_',
	obj->flag&OBJ_SATUR?'S':'_',
	obj->flag&OBJ_TRUNC?'T':'_',
	obj->flag&OBJ_APERT_PB?'A':'_',
	obj->flag&OBJ_ISO_PB?'I':'_',
	obj->flag&OBJ_DOVERFLOW?'D':'_',
	obj->flag&OBJ_OVERFLOW?'O':'_');
  }

void FindImageWindowImageObjects ( GWindowPtr window, PIXEL minlimit, PIXEL maxlimit, double sigma )
{
	short			rows, cols, frames, row, col, frame;
	PIXEL			**data, min, max;
	ImagePtr		imageptr;
	ImageRegionPtr	region;
	double			params[5], errors[5];
	char *path;
	char l_name[ MAX_PATH + 1 ];
	l_name[ sizeof( l_name ) -1] = 0;
	GetModuleFileName( NULL, l_name, sizeof( l_name ) -1 );

	imageptr  = GetImageWindowImage ( window );
	frames = GetImageFrames ( imageptr );
	rows   = GetImageRows ( imageptr );
	cols   = GetImageColumns ( imageptr );

	path = imageptr->imagePath;
#if 0
	for ( frame = 0; frame < frames; frame++ )
	{
		min = HUGE_VAL;
		max = -HUGE_VAL;
		data = GetImageDataFrame ( imageptr, frame );

		for ( row = 0; row < rows; row++ )
		{
			for ( col = 0; col < cols; col++ )
			{
				if ( data[row][col] > max )
					max = data[row][col];
				if ( data[row][col] < min )
					min = data[row][col];

			}
		}
	}
	fprintf(stderr, "Min %f\n", min);
	fprintf(stderr, "Max %f\n", max);
#endif
	{
 	char *argv[] = {l_name, path};
	sextract_main(sizeof(argv)/sizeof(*argv), argv);
	}
}

#else

void FindImageWindowImageObjects ( GWindowPtr window, PIXEL min, PIXEL max, double sigma )
{
	short			rows, cols, frames, row, col, frame;
	short			radius, left, top, right, bottom;
	PIXEL			***oldMatrix, *data;
	ImagePtr		image;
	ImageRegionPtr	region;
	double			params[5], errors[5];
	
	image  = GetImageWindowImage ( window );
	frames = GetImageFrames ( image );
	rows   = GetImageRows ( image );
	cols   = GetImageColumns ( image );
	
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/

	oldMatrix = ResizeImageWindowImage ( window, cols, rows, TRUE );
	if ( oldMatrix == NULL )
		return;
	
	/*** Compute the radius of the region around each pixel to which we will
	     try to fit the stellar model function.  The value of this radius
	     should be 3 times the expected one-sigma stellar profile radius;
	     if stellar profiles are reasonably Gaussian, then 95% of the light
	     should fall within this radius. ***/
	     
	radius = 4.0 * sigma + 0.5;
	
	left = radius;
	top = radius;
	right = cols - radius;
	bottom = rows - radius;
	
	for ( frame = 0; frame < frames; frame++ )
	{
		for ( row = radius; row < rows - radius; row++ )
		{
			top = row - radius;
			bottom = row + radius;
			
			data = GetImageDataRow ( image, frame, row );
			
			for ( col = radius; col < cols - radius; col++ )
			{
				data[col] = 0;

				/*** Create a temporary image region surrounding the pixel we are trying to fit
				     the stellar model function to.  If we succeed, fit the surface function
				     to that region of the image. ***/
				 
				left = col - radius;
				right = col + radius;
				region = NewImageRegion ( IMAGE_REGION_TYPE_RECTANGULAR, left, top, right, bottom );

				if ( region != NULL )
				{
					/*** We are fitting only the first two terms (background and amplitude) in the
					     stellar model function, but we need to pass in values for the other terms
					     in the equation so that the first two can be evaluated. ***/
					
					params[2] = col;
					params[3] = row;
					params[4] = sigma;
					
					/*** If we can fit the background and amplitude, set the image data value at the
					     corresponding position to the amplitude.  Then delete the temporary image region. ***/
						
					if ( FitImageSurface ( oldMatrix[ frame ], region, min, max, GaussianSurface, 2, params, errors ) )
					{
						data[col] = params[0];
						DeleteImageRegion ( region );
					}
				}
			}
		}
	}

	/*** Draw the image window's offscreen bitmap, resize the image's window
	     to fit the image, then invalidate the window so it gets redrawn. ***/

	ComputeImageDisplayRange ( window );
	DrawImageWindowBitmap ( window );
	GInvalidateWindow ( window, NULL );
}

void FindImageWindowImageObjects ( GWindowPtr window, PIXEL min, PIXEL max, double sigma )
{
	short			rows, cols, frames, row, col, frame;
	short			radius, left, top, right, bottom;
	PIXEL			**data, ***oldMatrix;
	ImagePtr		image;
	ImageRegionPtr	region;
	ImageObjectPtr	object;
	double			threshold, background, noise;
	
	image  = GetImageWindowImage ( window );
	frames = GetImageFrames ( image );
	rows   = GetImageRows ( image );
	cols   = GetImageColumns ( image );
	
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/

	oldMatrix = ResizeImageWindowImage ( window, cols, rows, TRUE );
	if ( oldMatrix == NULL )
		return;

	/*** Compute the radius of the region around each pixel to which we will
	     try to fit the stellar model function.  The value of this radius
	     should be 3 times the expected one-sigma stellar profile radius;
	     if stellar profiles are reasonably Gaussian, then 95% of the light
	     should fall within this radius. ***/
	     
	radius = 4.0 * sigma + 0.5;
	
	left = radius;
	top = radius;
	right = cols - radius;
	bottom = rows - radius;
	
	for ( frame = 0; frame < frames; frame++ )
	{
		data = GetImageDataFrame ( image, frame );
			
		for ( row = radius; row < rows - radius; row++ )
		{
			bottom = row + 5 * sigma;
			if ( bottom >= rows )
				bottom = rows - 1;
				
			top = row - 5 * sigma;
			if ( top < 0 )
				top = 0;
				
			for ( col = radius; col < cols - radius; col++ )
			{
				left = col - 5 * sigma;
				if ( left < 0 )
					left = 0;
					
				right = col + 5 * sigma;
				if ( right >= cols )
					right = cols - 1;
				
				region = NewImageRegion ( IMAGE_REGION_TYPE_RECTANGULAR, left, top, right, bottom );
				if ( region != NULL )
				{
					ComputeImageRegionBackground ( oldMatrix[0], region, 3, &background, &noise );
					DeleteImageRegion ( region );
					
					threshold = background + 3 * noise;
					
					if ( data[row][col] > threshold && IsLocalMaximum ( data, rows, cols, row, col ) )
					{
						object = CreateImageObject ( image, col, row, sigma );
						if ( object != NULL )
							AddImageObject ( image, object );
					}
					
					data[row][col] = background;
				}
			}
		}
	}

	FreeFITSImageDataMatrix ( oldMatrix );
	
	/*** Draw the image window's offscreen bitmap, resize the image's window
	     to fit the image, then invalidate the window so it gets redrawn. ***/

	ComputeImageDisplayRange ( window );
	DrawImageWindowBitmap ( window );
	GInvalidateWindow ( window, NULL );
}

void FindImageWindowImageObjects ( GWindowPtr window, PIXEL min, PIXEL max, double sigma )
{
	short			rows, cols, frames, row, col, frame;
	short			radius, left, top, right, bottom;
	PIXEL			**data, ***oldMatrix;
	ImagePtr		image;
	ImageObjectPtr	object;
	double			threshold, background, noise;
	
	image  = GetImageWindowImage ( window );
	frames = GetImageFrames ( image );
	rows   = GetImageRows ( image );
	cols   = GetImageColumns ( image );
	
	/*** Re-allocate the image window's image data matrix, and save a pointer
	     to the previous image data matrix.  On failure, return. ***/

	oldMatrix = ResizeImageWindowImage ( window, cols, rows, TRUE );
	if ( oldMatrix == NULL )
		return;

	for ( frame = 0; frame < frames; frame++ )
	{
		data = oldMatrix[frame];
			
		for ( row = 0; row < rows; row++ )
		{
			for ( col = 0; col < cols; col++ )
			{
				if ( IsLocalMaximum ( data, rows, cols, row, col ) )
				{
					ComputePixelBackground ( data, cols, rows, col, row, 3.0 * sigma + 0.5, 5.0 * sigma + 0.5, 3.0, &background, &noise );
					threshold = background + 3 * noise;
					if ( data[row][col] > threshold )
					{
						object = CreateImageObject ( image, col, row, sigma );
						if ( object != NULL )
							AddImageObject ( image, object );
					}
				}
			}
		}
	}

	FreeFITSImageDataMatrix ( oldMatrix );
	
	/*** Draw the image window's offscreen bitmap, resize the image's window
	     to fit the image, then invalidate the window so it gets redrawn. ***/

	ComputeImageDisplayRange ( window );
	DrawImageWindowBitmap ( window );
	GInvalidateWindow ( window, NULL );
}

#endif

/*** DeleteImageWindowImageObjects *************************************************************

	Deletes all objects in an image window's selected region; or, if the window has no selected
	region, deletes all objects in an image window.

	void DeleteImageWindowImageObjects ( GWindowPtr window )
	
	(window): pointer to image window.
	
************************************************************************************************/

void DeleteImageWindowImageObjects ( GWindowPtr window )
{
	short			col, row;
	ImagePtr		image;
	ImageRegionPtr	region;
	ImageObjectPtr	object;
	
	image = GetImageWindowImage ( window );
	region = GetImageWindowSelectedRegion ( window );
	
	if ( region == NULL )
	{
		DeleteImageObjectList ( image );
	}
	else
	{
		for ( object = GetImageObjectList ( image ); object != NULL; object = GetNextImageObject ( object ) )
		{
			col = GetImageObjectCentroidX ( object ) + 0.5;
			row = GetImageObjectCentroidY ( object ) + 0.5;
			
			if ( IsPixelInImageRegion ( region, col, row ) )
			{
				RemoveImageObject ( image, object );
				DeleteImageObject ( object );
			}
		}
	}
	
	GInvalidateWindow ( window, NULL );
}

int IsLocalMaximum ( PIXEL **data, short rows, short cols, short row0, short col0 )
{
	short left, top, right, bottom, row, col;
	
	left = col0 - 1;
	top = row0 - 1;
	right = col0 + 1;
	bottom = row0 + 1;
	
	if ( left < 0 )
		left = 0;
		
	if ( top < 0 )
		top = 0;
		
	if ( right >= cols )
		right = cols - 1;
		
	if ( bottom >= rows )
		bottom = rows - 1;

	for ( row = top; row <= bottom; row++ )
		for ( col = left; col <= right; col++ )
			if ( col != col0 || row != row0 )
				if ( data[row][col] >= data[row0][col0] )
					return ( FALSE );
	
	return ( TRUE );
}

void ComputeImageRegionBackground ( PIXEL **data, ImageRegionPtr region, double max_sigma, double *background, double *noise )
{
	short	left, right, row, col;
	PIXEL	value;
	double	mean = 0.0, stdev = 0.0, old_mean, sum, sum2;
	long	n;
	
	do
	{
		old_mean = mean;
		sum = sum2 = n = 0;
		row = -1;

		while ( GetImageRegionSegment ( region, &row, &left, &right ) )
		{
			for ( col = left; col <= right; col++ )
			{
				value = data[row][col];
				
				if ( stdev == 0.0 || value - mean < max_sigma * stdev )
				{
					sum  += value;
					sum2 += value * value;
					n++;
				}
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
	}
	while ( mean != old_mean );
	
	*background = mean;
	*noise = stdev;
}

void ComputePixelBackground ( PIXEL **data, short cols, short rows,
short col0, short row0, short inn_radius, short out_radius, double max_sigma, double *background, double *noise )
{
	short	left, top, right, bottom, row, col;
	long	dcol, drow, dist2, inn_radius2, out_radius2;
	PIXEL	value;
	double	mean = 0.0, stdev = 0.0, old_mean, sum, sum2;
	long	n;
	
	inn_radius2 = inn_radius * inn_radius;
	out_radius2 = out_radius * out_radius;
	
	left = col0 - out_radius;
	top = row0 - out_radius;
	right = col0 + out_radius;
	bottom = row0 + out_radius;
	
	if ( left < 0 )
		left = 0;
		
	if ( top < 0 )
		top = 0;
		
	if ( right >= cols )
		right = cols - 1;
		
	if ( bottom >= rows )
		bottom = rows - 1;

	do
	{
		old_mean = mean;
		sum = sum2 = n = 0;

		for ( row = top; row <= bottom; row++ )
		{
			drow = row - row0;
			
			for ( col = left; col <= right; col++ )
			{
				dcol = col - col0;
				dist2 = dcol * dcol + drow * drow;
				
				if ( dist2 >= inn_radius2 && dist2 <= out_radius2 )
				{
					value = data[row][col];
					
					if ( stdev == 0.0 || value - mean < max_sigma * stdev )
					{
						sum  += value;
						sum2 += value * value;
						n++;
					}
				}
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
	}
	while ( mean != old_mean );
	
	*background = mean;
	*noise = stdev;
}

/*** FindLocalMaximumPixel ***/

void FindLocalMaximumPixel ( PIXEL **data, short cols, short rows, short *col0, short *row0, short radius )
{
	long	dcol, drow, radius2 = radius * radius;
	short	left, top, right, bottom, row, col;
	short	col1 = *col0;
	short	row1 = *row0;
	PIXEL	max = data[row1][col1];
	
	left = col1 - radius;
	top = row1 - radius;
	right = col1 + radius;
	bottom = row1 + radius;
	
	if ( left < 0 )
		left = 0;
		
	if ( top < 0 )
		top = 0;
		
	if ( right >= cols )
		right = cols - 1;
		
	if ( bottom >= rows )
		bottom = rows - 1;

	for ( row = top; row <= bottom; row++ )
	{
		drow = row - *row0;
		
		for ( col = left; col <= right; col++ )
		{
			dcol = col - *col0;

			if ( ( dcol * dcol + drow * drow ) <= radius2 )
			{
				if ( data[row][col] > max )
				{
					max = data[row][col];
					col1 = col;
					row1 = row;
				}
			}
		}
	}	

	*col0 = col1;
	*row0 = row1;
}
