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

/*** local data types ***/

struct ImageHistogram
{
	long	*histogramBins;
	long	histogramNumBins;
	long	histogramPeakBinCount;
	PIXEL	histogramMinValue;
	PIXEL	histogramMaxValue;
	PIXEL	histogramResolution;
	GRect	histogramDisplayRect;
	PIXEL	histogramDisplayMinValue;
	PIXEL	histogramDisplayMaxValue;
	long	histogramDisplayMinCount;
	long	histogramDisplayMaxCount;
};

/*** CreateImageHistogram *********************************************************

	Allocates memory for a new image histogram structure.
	
	ImageHistogramPtr CreateImageHistogram ( PIXEL min, PIXEL max, PIXEL resolution )
	
	(min):  minimum image data value to be counted in the histogram.
	(max):  maximum image data value to be counted in the histogram.
	(res):  resolution (i.e. bin width) of values in the histogram.
	(bins): number of bins in the histogram.
	
	If successful, the function returns a pointer to a new image histogram
	structure; on failure, it returns NULL.
	
	If the (bins) parameter is non-zero, the histogram is created with the number
	of bins specified in the (bins) parameter, and the resolution is computed by
	dividing the difference between the maximum and mimum by the number of bins.
	
	If the (res) parameter is non-zero, the histogram is created with the resolution
	specified in the (res) parameter, and the number of bins is computed by
	dividing the difference between the maximum and minimum by the resolution.
	
	If both parameters are zero, the function will fail to create a histogram.
	
***********************************************************************************/

ImageHistogramPtr CreateImageHistogram ( PIXEL min, PIXEL max, PIXEL resolution, long bins )
{
	long				bin;
	ImageHistogramPtr	histogram = NULL;
	
	/*** If we are given a specific bin count, compute the corresponding resolution;
	     if we are given a resolution, compute the bin count.  If we have neither,
	     return NULL. ***/
	
	if ( bins > 0 )     
	{
		resolution = ( max - min ) / bins;
	}
	else if ( resolution > 0 )
	{
		/*** Make sure the histogram always has enough bins to hold the requested
		     range of data, and modify the maximum value so the range is an exact
		     multiple of the resolution. ***/
		
		bins = ( max - min ) / resolution + 1;
		max = min + bins * resolution;
	}
	else
	{
		return ( NULL );
	}
	
	/*** Allocate memory for the histogram record.  On failure, return NULL. ***/
	
	histogram = (ImageHistogramPtr) malloc ( sizeof ( ImageHistogram ) );
	if ( histogram == NULL )
		return ( NULL );
		
	/*** Now allocate an array big enough to hold the bin counts.  On failure, free
	     the histogram record and return NULL. ***/

	histogram->histogramBins = (long *) malloc ( sizeof ( long ) * bins );
	if ( histogram->histogramBins == NULL )
	{
		free ( histogram );
		return ( NULL );
	}
	
	/*** Zero-initalize the bin array, and store other relevant values. ***/
	
	for ( bin = 0; bin < bins; bin++ )
		histogram->histogramBins[bin] = 0;
	
	histogram->histogramMinValue = min;
	histogram->histogramMaxValue = max;
	histogram->histogramResolution = resolution;
	histogram->histogramNumBins = bins;
	
	histogram->histogramDisplayRect.left = 0;
	histogram->histogramDisplayRect.top = 0;
	histogram->histogramDisplayRect.right = 0;
	histogram->histogramDisplayRect.bottom = 0;

	histogram->histogramDisplayMinValue = min;
	histogram->histogramDisplayMaxValue = max;
	histogram->histogramDisplayMinCount = 0;
	histogram->histogramDisplayMaxCount = 0;
	
	return ( histogram );
}

/*** DeleteImageHistogram ******************************************************************

	Frees memory for an image histogram structure.

	void DeleteImageHistogram ( ImageHistogramPtr histogram )
	
	(histogram): pointer to image histogram structure.
	
	The function returns nothing.
	
******************************************************************************************/

void DeleteImageHistogram ( ImageHistogramPtr histogram )
{
	free ( histogram->histogramBins );
	free ( histogram );
}

/*** GetImageHistogramMinimumValue ********************************************************

	Returns the minimum value counted in an image histogram.

	PIXEL GetImageHistogramMinimumValue ( ImageHistogramPtr histogram )
	
	(histogram): pointer to image histogram record.
	
*******************************************************************************************/

PIXEL GetImageHistogramMinimumValue ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramMinValue );
}

/*** GetImageHistogramMaximumValue *********************************************************

	Returns the maximum value counted in an image histogram.

	PIXEL GetImageHistogramMaximumValue ( ImageHistogramPtr histogram )
	
	(histogram): pointer to image histogram record.
	
*******************************************************************************************/

PIXEL GetImageHistogramMaximumValue ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramMaxValue );
}

/*** GetImageHistogramResolution **********************************************************

	Returns the resolution of an image histogram.

	PIXEL GetImageHistogramResolution ( ImageHistogramPtr histogram )
	
	(histogram): pointer to image histogram record.
	
*******************************************************************************************/

PIXEL GetImageHistogramResolution ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramResolution );
}

/*** GetImageHistogramNumBins ************************************************************

	Returns the number of bins in an image histogram.

	long GetImageHistogramNumBins ( ImageHistogramPtr histogram )
	
	(histogram): pointer to image histogram record.
	
*******************************************************************************************/

long GetImageHistogramNumBins ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramNumBins );
}

/*** GetImageHistogramBinCount ************************************************************

	Returns the number of pixels in a particular bin of the histogram.

	long GetImageHistogramBinCount ( ImageHistogramPtr histogram, long bin )
	
	(histogram): pointer to image histogram record.
	(bin): index of bin whose pixel count you wish to determine.
	
	If the bin index is valid, the function returns the number of pixels in
	the specified bin; otherwise, the function returns -1.  Valid bin indices
	run from zero to the total number of bins minus one.  (See the function
	GetImageHistogramNumBins().)
	
*******************************************************************************************/

long GetImageHistogramBinCount ( ImageHistogramPtr histogram, long bin )
{
	if ( bin >= 0 && bin < histogram->histogramNumBins )
		return ( histogram->histogramBins[ bin ] );
	else
		return ( -1 );
}

/*** GetImageHistogramPeakBinCount ************************************************************

	Returns the number of pixels in the bin of the histogram which contains
	the largest number of pixels.

	long GetImageHistogramPeakBinCount ( ImageHistogramPtr histogram )
	
	(histogram): pointer to image histogram record.
	
	The function returns the number of pixels in the histogram bin containing
	the largest number of pixels.
	
*******************************************************************************************/

long GetImageHistogramPeakBinCount ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramPeakBinCount );
}


/*** GetImageHistogramBinIndex ************************************************************

	Converts a pixel value to a bin index.

	long GetImageHistogramBinIndex ( ImageHistogramPtr histogram, PIXEL value )
	
	(histogram): pointer to image histogram record.
	(value): pixel value you wish to determine bin index of.
	
	If the specified pixel value falls within the range of values counted in the
	histogram, the function returns the corresponding bin index.  Otherwise, the
	function returns -1.  The range of values counted in the histogram can be found
	by the functions GetImageHistogramMinimumValue() and GetImageHistogramMaximumValue().
	
*******************************************************************************************/

long GetImageHistogramBinIndex ( ImageHistogramPtr histogram, PIXEL value )
{
	long bin = ( value - histogram->histogramMinValue ) / histogram->histogramResolution;
	
	if ( bin >= 0 && bin < histogram->histogramNumBins )
		return ( bin );
	else
		return ( -1 );
}

/*** GetImageHistogramBinValue ************************************************************

	Converts bin index to a pixel value.

	long GetImageHistogramBinValue ( ImageHistogramPtr histogram, long bin )
	
	(histogram): pointer to image histogram record.
	(bin): index of bin whose value you wish to determine.
	
	The function actually returns the starting value of the specifed bin.  The
	actual range of values contained in the bin equals (the value returned by this
	function) up to but not including (that value plus the histogram resolution).
		
*******************************************************************************************/

PIXEL GetImageHistogramBinValue ( ImageHistogramPtr histogram, long bin )
{
	return ( histogram->histogramMinValue + histogram->histogramResolution * bin );
}

/*** ComputeImageRegionHistogram ***********************************************************

	Computes a histogram of a particular region of an image.

	void ComputeImageRegionHistogram ( ImagePtr image, ImageRegionPtr region, ImageHistogramPtr histogram )
	
	(image):     pointer to image
	(region):    pointer to region describing area of image you wish to take histogram of.
	(histogram): pointer to histogram which recieves results.
	
	The function returns nothing.
	
	After calling this function, the histogram's will contain the pixel counts in the
	specified region of the image.
	
	The function does not check to see whether or not the image region falls within the
	boundaries of the image.
	
********************************************************************************************/

void ComputeImageRegionHistogram ( ImagePtr image, ImageRegionPtr region, ImageHistogramPtr histogram )
{
	long	bin, bins = GetImageHistogramNumBins ( histogram );
	short	row = -1, col, left, right;
	PIXEL	*data;
	
	/*** First, zero the pixel counts in the histogram bins. ***/
	
	for ( bin = 0; bin < bins; bin++ )
		histogram->histogramBins[ bin ] = 0;
	
	/*** Now iterate through every pixel in the selected image region.
	     For each pixel, compute the bin index corresponding to the pixel's
	     intensity value.  If the pixel value corresponds to a valid bin
	     index, then increment the count of pixels in that bin by one. ***/
	     
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
	{
		data = GetImageDataRow ( image, 0, row );
		
		for ( col = left; col <= right; col++ )
		{
			bin = ( data[col] - histogram->histogramMinValue ) / histogram->histogramResolution;
			
			if ( bin >= 0 && bin < bins )
				histogram->histogramBins[ bin ]++;
		}
	}

	/*** Finally, find the peak value in image histogram (i.e. the pixel
	     count of the bin with the largest number of pixels). ***/
	
	for ( histogram->histogramPeakBinCount = bin = 0; bin < bins; bin++ )
	{
		if ( histogram->histogramBins[ bin ] > histogram->histogramPeakBinCount )
			histogram->histogramPeakBinCount = histogram->histogramBins[ bin ];
	}
}

/*** GetHistogramDisplayRect **********************************************************

	Returns a pointer to a rectangle describing the boundaries of the area of a window
	in which a histogram should be displayed.

	PIXEL GetHistogramDisplayMinimumValue ( ImageHistogramPtr histogram )
	
	(window): pointer to histogram window.
	
***************************************************************************************/

void SetHistogramDisplayRect ( ImageHistogramPtr histogram, GRectPtr rect )
{
	histogram->histogramDisplayRect = *rect;
}

GRectPtr GetHistogramDisplayRect ( ImageHistogramPtr histogram )
{
	return ( &histogram->histogramDisplayRect );
}

/*** GetHistogramDisplayMinimumValue ***************************************************

	Returns the minimum value displayed along the image data value (horizontal) axis
	of the histogram displayed.

	PIXEL GetHistogramDisplayMinimumValue ( ImageHistogramPtr histogram )
	
	(window): pointer to histogram window.
	
***************************************************************************************/

void SetHistogramDisplayMinimumValue ( ImageHistogramPtr histogram, PIXEL value )
{
	histogram->histogramDisplayMinValue = value;
}

PIXEL GetHistogramDisplayMinimumValue ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramDisplayMinValue );
}

/*** GetHistogramDisplayMaximumValue ***************************************************

	Returns the maximum value displayed along the image data value (horizontal) axis
	of the histogram.

	PIXEL GetHistogramDisplayMaximumValue ( ImageHistogramPtr histogram )
	
	(window): pointer to histogram window.
	
***************************************************************************************/

void SetHistogramDisplayMaximumValue ( ImageHistogramPtr histogram, PIXEL value )
{
	histogram->histogramDisplayMaxValue = value;
}

PIXEL GetHistogramDisplayMaximumValue ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramDisplayMaxValue );
}

/*** GetHistogramDisplayMaximumCount ***************************************************

	Returns the minimum value displayed along the pixel count (vertical) axis
	of the histogram.

	PIXEL GetHistogramDisplayMaximumCount ( ImageHistogramPtr histogram )
	
	(window): pointer to histogram window.
	
***************************************************************************************/

void SetHistogramDisplayMaximumCount ( ImageHistogramPtr histogram, long count )
{
	histogram->histogramDisplayMaxCount = count;
}

long GetHistogramDisplayMaximumCount ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramDisplayMaxCount );
}

/*** GetHistogramDisplayMinimumCount ***************************************************

	Returns the minimum value displayed along the pixel count (vertical) axis
	of the histogram.

	PIXEL GetHistogramDisplayMinimumCount ( ImageHistogramPtr histogram )
	
	(window): pointer to histogram window.
	
***************************************************************************************/

void SetHistogramDisplayMinimumCount ( ImageHistogramPtr histogram, long count )
{
	histogram->histogramDisplayMinCount = count;
}

long GetHistogramDisplayMinimumCount ( ImageHistogramPtr histogram )
{
	return ( histogram->histogramDisplayMinCount );
}

/*** HistogramColRowToValueCount *******************************************************

	Converts a column and row in a histogram window to a pixel value and count.
	
	void HistogramColRowToValueCount ( ImageHistogramPtr hist, short col, short row,
	     PIXEL *value, long *count )

	(col):   column in window for which pixel value is desired.
	(row):   row in window for which pixel count is desired.
	(hist):  pointer to histogram record.
	(value): recieves pixel value corresponding to specified column.
	(count): recieves pixel count corresponding to specified row.
	
	The function returns nothing.
	
****************************************************************************************/

void HistogramColRowToValueCount ( ImageHistogramPtr histogram, short col, short row,
PIXEL *value, long *count )
{
	GRectPtr	rect = &histogram->histogramDisplayRect;
	PIXEL		minValue = histogram->histogramDisplayMinValue;
	PIXEL		maxValue = histogram->histogramDisplayMaxValue;
	long		minCount = histogram->histogramDisplayMinCount;
	long		maxCount = histogram->histogramDisplayMaxCount;
	double		scale;
	
	scale = (double) ( maxValue - minValue ) / GWidth ( rect );
	*value = minValue + scale * ( col - GLeft ( rect ) );

	scale = (double) ( maxCount - minCount ) / GHeight ( rect );
	*count = minCount + scale * ( GBottom ( rect ) - row );
}

/*** HistogramValueCountToColRow ******************************************************

	Converts a column and row in a histogram window to a pixel value and count.
	
	void HistogramColRowToValueCount ( ImageHistogramPtr hist, short col, short row,
	     PIXEL *value, long *count )

	(value): pixel value for which window column is desired.
	(count): pixel count for which window row is desired.
	(hist):  pointer to histogram
	(col):   receives column in window corresponding to pixel value.
	(row):   receives row in window corresponding to pixel count.
	
	The function returns nothing.
	
****************************************************************************************/

void HistogramValueCountToColRow ( ImageHistogramPtr histogram, PIXEL value, long count,
short *col, short *row )
{
	GRectPtr	rect = &histogram->histogramDisplayRect;
	PIXEL		minValue = histogram->histogramDisplayMinValue;
	PIXEL		maxValue = histogram->histogramDisplayMaxValue;
	long		minCount = histogram->histogramDisplayMinCount;
	long		maxCount = histogram->histogramDisplayMaxCount;
	double		scale;
	
	scale = (double) GWidth ( rect ) / ( maxValue - minValue );
	*col = GLeft ( rect ) + scale * ( value - minValue );

	scale = (double) GHeight ( rect ) / ( maxCount - minCount );
	*row = GBottom ( rect ) - scale * ( count - minCount );
}

/*** DrawImageHistogram *******************************************************

	Draws a histogram into a rectangular region.
	
	void DrawImageHistogram ( ImageHistogramPtr hist )
	
	(hist): pointer to histogram you wish to draw.
	
	The function returns nothing.
	
	NOTE: YOU MUST BRACKET THIS FUNCTION WITH ONE OF THE GStartDrawing()/
	GEndDrawing() SETS OF FUNCTIONS!
	
*******************************************************************************/

#define MAX_TICKS	10

void DrawImageHistogram ( ImageHistogramPtr histogram )
{
	GRectPtr rect = GetHistogramDisplayRect ( histogram );
	PIXEL	minValue = GetHistogramDisplayMinimumValue ( histogram );
	PIXEL	maxValue = GetHistogramDisplayMaximumValue ( histogram );
	long	minCount = GetHistogramDisplayMinimumCount ( histogram );
	long	maxCount = GetHistogramDisplayMaximumCount ( histogram );
	double	resolution = GetImageHistogramResolution ( histogram );
	short	left = GLeft ( rect );
	short	right = GRight ( rect );
	short	top = GTop ( rect );
	short	bottom = GBottom ( rect );
	short	width = GWidth ( rect );
	short	height = GHeight ( rect );
	short	col0, col1, col, row;
	short	size, fill;
	GColor	color;
	PIXEL	value;
	long	bins = GetImageHistogramNumBins ( histogram );
	long	bin, count, tick, numTicks;
	double	delta;
	double	valueScale;
	double	countScale;
	char	label[256];
	short	labelWidth, labelHeight;
	
	/*** Determine the constants which will be used to scale from pixel value
	     to horizontal position, and from pixel count to vertical position. ***/
	
	valueScale = (double) ( width - 1 ) / ( maxValue - minValue );
	countScale = (double) height / ( maxCount - minCount );
	
	/*** Save the pen state, then set the pen fill so that we are drawing
	     filled shapes. ***/
	
	GGetPen ( &color, &size, &fill );
	GSetPenFill ( TRUE );
	
	/*** Determine the width of a histogram bin as it should be drawn.
	     Round the result upward, to make sure we are drawing the bins
	     at least one pixel wide, and so that there are no misleading gaps
	     in the data. ***/
	    
	width = ceil ( resolution * valueScale );
		
	/*** Now draw the bins. ***/ 
	
	for ( bin = 0; bin < bins; bin++ )
	{
		/*** Get the pixel count of the bin, then compute the pixel value
		     corresponding to the left (e.g. minimum value) side of the bin,
		     and then the corresponding columns where the left and right
		     side of the bin should be drawn. ***/
		     
		count = GetImageHistogramBinCount ( histogram, bin );
		value = GetImageHistogramBinValue ( histogram, bin );
		
		col0  = left + ( value - minValue ) * valueScale + 0.5;
		col1  = col0 + width;
		
		/*** If there are actually pixels in the bin, and the left and right
		     sides of the bin fall into the bounding rectangle, draw the thing. ***/
		
		if ( count > 0 && col0 <= right && col1 >= left )
		{
			/*** Clip the left and right sides of the bin to the bounding
			     rectangle, if they exceed it. ***/
			     				
			if ( col0 < left )
				col0 = left;
			if ( col1 > right )
				col1 = right;
			
			/*** Compute the row corresponding to pixel count, and clip
			     it to the top of the rectangle if it exceeds it. ***/
			     
			row = bottom - ( count - minCount ) * countScale;
			if ( row < top )
				row = top;

			/*** If the row corresponding to the pixel count is above
			     the bottom of the histogram's display rectangle (i.e.
			     the pixel count is greater than the minimum pixel count
			     displayed), then draw a filled rectangle represnting
			     the bin. ***/
			     
			if ( row < bottom )
				GDrawRectangle ( col0, row, col1, bottom );
		}
	}
	
	/*** Now restore the pen's previous fill state. ***/
	
	GSetPenFill ( fill );

	/*** Draw vertical (pixel count) axis and tick marks. ***/
	
	delta = floor ( ( maxCount - minCount ) / (double) MAX_TICKS );
	
	if ( delta < 1.0 )
	{
		delta = 1.0;
		numTicks = maxCount - minCount + 1;
	}
	else
	{
		numTicks = MAX_TICKS;
	}
	
	GDrawLine ( left, bottom, left, top );
	
	for ( tick = 0; tick <= numTicks; tick++ )
	{
		count = minCount + tick * delta;
		row = bottom - ( count - minCount ) * countScale;
		
		GDrawLine ( left, row, left - 10, row );
		
		sprintf ( label, "%ld", count );
		GGetStringSize ( label, 0, &labelWidth, &labelHeight );
		
		row = row - labelHeight / 2;
		col = left - 15 - labelWidth;
		
		GDrawString ( label, 0, col, row );
	}

	/*** Draw horizontal (pixel value) axis and tick marks. ***/

	delta = ( maxValue - minValue ) / (double) MAX_TICKS;

	if ( delta < resolution )
	{
		delta = resolution;
		numTicks = ( maxValue - minValue ) / delta;
	}
	else
	{
		numTicks = MAX_TICKS;
	}
	
	GDrawLine ( left, bottom, right, bottom );

	for ( tick = 0; tick <= numTicks; tick++ )
	{
		value = minValue + tick * delta;
		col   = left + ( value - minValue ) * valueScale + 0.5;
		
		GDrawLine ( col, bottom, col, bottom + 10 );

		sprintf ( label, "%.2f", value );
		GGetStringSize ( label, 0, &labelWidth, &labelHeight );
		
		col = col - labelWidth / 2;
		row = bottom + 15;
		
		GDrawString ( label, 0, col, row );
	}
}
