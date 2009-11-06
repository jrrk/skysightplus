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

/*** local function prototypes ***/

/*** NewImage *********************************************************************

	Allocates memory for a new image record.

	ImagePtr NewImage ( short type, short frames, short rows, short columns )
	
	(type):   image type.
	(frames): number of frames in the image.
	(rows):   number of rows of image data per frame.
	(cols):   number of columns of image data per row.
	
	If succeessful, the function returns a pointer to a new image record.
	The image data is initialized to zero.
	
	See GetImageType(), GetImageFrames(), GetImageRows(), GetImageCols(), etc.
	for a more complete description of the other parameters.
	
	On failure, the function returns NULL.
	
***********************************************************************************/

ImagePtr NewImage ( char *title, short type, short frames, short rows, short columns )
{
	short			frame, row, col;
	FITSImagePtr	fits;
	ImagePtr		image;
	
	/*** Allocate memory for the image record.  On failure, return NULL. ***/
	
	image = (ImagePtr) calloc ( sizeof ( Image ), 1 );
	if ( image == NULL )
		return ( NULL );

	/*** Allocate memory for the image's FITS image descriptor record, header,
	     and data matrix.  On failure, free the image record and return NULL. ***/
	
	fits = NewFITSImage ( -32, 3, columns, rows, frames, 0.0, 1.0 );
	if ( fits == NULL )
	{
		free ( image );
		return ( NULL );
	}
	
	/*** Initialize all frames of the image data matrix to zero. ***/
	
	for ( frame = 0; frame < frames; frame++ )
		for ( row = 0; row < rows; row++ )
			for ( col = 0; col < columns; col++ )
				fits->data[frame][row][col] = 0;
	
	/*** Initialize the other fields of the image record. ***/
				
	strncpy ( image->imageTitle, title, sizeof ( image->imageTitle ) );
	
	image->imageType          = type;
	image->imagePath          = NULL;
	image->imageWindow        = NULL;
	image->imageHeaderWindow  = NULL;
	image->imageFITSImage     = fits;
	image->imageFileFormat    = FILE_TYPE_FITS;
	image->imageObjectList    = NULL;
	image->imageObjectCount   = 0;
	image->imagePreviousImage = NULL;
	
	/*** Return a pointer to the initialized image record. ***/
	
	return ( image );
}

/*** NewImageFromBitmap ***/

ImagePtr NewImageFromBitmap ( char *title, GImagePtr bitmap )
{
	short			type, frames, rows, cols, row, col;
	ImagePtr		image = NULL;
	unsigned char	*data, red, green, blue;
	unsigned long	value;
	FITSImagePtr	fits;
	
	/*** Determine the pixel dimensions of the bitmap. ***/
	
	rows = GGetImageHeight ( bitmap );
	cols = GGetImageWidth ( bitmap );

	/*** Regardless of the bit-depth of the original bitmap,
	     we will create a color image window with 3 separate
	     frames for the red, green, and blue components.
	     We do this because SkySight's has no knowledge of the
	     original image's color table, so converting indexed-color
	     images to bitmap images is the only way to avoid losing
	     data or displaying it incorrectly. ***/
	     
	type = IMAGE_TYPE_RGB_COLOR;
	frames = 3;
	
	/*** Allocate memory for the image record.  On failure, return NULL. ***/
	
	image = NewImage ( title, type, frames, rows, cols );
	if ( image == NULL )
		return ( NULL );
	
	/*** Obtain a pointer to the image's FITS image information record,
	     then copy the image data from the bitmap into the FITS image data
	     matrix. ***/
	     
	fits = GetImageFITSImage ( image );
	
	for ( row = 0; row < rows; row++ )
	{
		data = GGetImageDataRow ( bitmap, row );
		for ( col = 0; col < cols; col++ )
		{
			value = GGetImageDataValue ( bitmap, data, col );
			GGetImageDataValueColor ( bitmap, value, &red, &green, &blue );
			
			fits->data[0][row][col] = red;
			fits->data[1][row][col] = green;
			fits->data[2][row][col] = blue;
		}
	}
	
	/*** Return a pointer to the new image. ***/
	
	return ( image );
}

/*** ReadFITSImageFile ***************************************************************

	Allocates a new image record and reads image data into it from a file stored
	on disk.

	ImagePtr ReadFITSImageFile ( GPathPtr path )
	
	(path): pointer to path specification record describing the file location.
	
	If successful, the function returns a pointer to the new image record; on
	failure, the function returns NULL.
	
**********************************************************************************/

ImagePtr ReadFITSImageFile ( GPathPtr path )
{
	FILE		*file = NULL;
	FITSImage	*fits = NULL;
	ImagePtr	image = NULL;
	
	/*** Open the file at the specified path location.  On failure, return
	     NULL. ***/
	     
	file = GOpenFile ( path, "rb", NULL, NULL );
	if ( file == NULL )
		return ( NULL );
	
	/*** Create a new FITS image information record and read the FITS file
	     image data into it, then close the file.  On failure, return NULL.  ***/
	      
	fits = ReadFITSImage ( file );
	fclose ( file );
	if ( fits == NULL )
		return ( NULL );
	
	/*** Allocate a new image record.  On failure, free the FITS image
	     information record, and return NULL. ***/
	     
	image = (ImagePtr) calloc ( sizeof ( Image ), 1 );
	if ( image == NULL )
	{
		FreeFITSImage ( fits );
		return ( NULL );
	}
	
	/*** Initialize the other fields of the image record. ***/
	
	if ( fits->naxis3 == 1 )
		image->imageType = IMAGE_TYPE_MONOCHROME;
	else if ( fits->naxis3 == 2 )
		image->imageType = IMAGE_TYPE_COMPLEX;
	else if ( fits->naxis3 == 3 )
		image->imageType = IMAGE_TYPE_RGB_COLOR;
			
	image->imagePath      = NULL;
	image->imageWindow    = NULL;
	image->imageFITSImage = fits;

	/*** Set the image's path specification and title to those of the
	     file we just read. ***/
	     
	SetImagePath ( image, path );
	
	return ( image );	
}

#if 0

/*** GetFITSImageInfo ***/

void GetFITSImageInfo ( char **header, ImagePtr image )
{
	double exposure = nan();
	double temperature = nan();
	
	/*** Loop through the lines in the header matrix, and look for the keywords
	     which describe the image header information we are looking for.
	     Save parameters in the image structure as we find them. ***/
	     
	for ( n = 0; ( line = GetFITSHeaderLine ( header, n ) ) != NULL; n++ )
	{
		if ( TestFITSHeaderKeyword ( line, "EXPOSURE" ) )
			GetFITSHeaderReal ( line, &exposure );

		if ( TestFITSHeaderKeyword ( line, "TEMPERAT" ) )
			GetFITSHeaderReal ( line, &temperature );
	}
	
	SetImageTemperature ( image, temperature );
	SetImageExposureLength ( image, exposure );
}

/*** SetFITSImageInfo ***/

void SetFITSImageInfo ( char ***header, ImagePtr image )
{
	char	line[81];
	
	memset ( line, ' ', 80 );
	line[80] = '\0';
	
	SetFITSHeaderKeyword ( line, "EXPOSURE" );
	SetFITSHeaderReal ( line, GetImageExposure ( image ) );
	ReplaceFITSHeaderLine ( header, line );
}

void ReplaceFITSHeaderKeywordReal ( char ***header, char *keyword, double value )
{

}

void SetFITSHeaderKeywordValue ( char ***header, char *keyword, char *value )
{
	char	keyword[9];
	long	linenum = 0;
	
	/*** If we find the keyword among the existing FITS header,
	     replace the entire line on which the keyword was found
	     with the new header line. ***/
	     
	if ( FindFITSHeaderLine ( *header, keyword, &linenum ) )
	{
		SetFITSHeaderLine ( header, linenum, value );
		SetFITSHeaderKeyword ( header, keyword );
	}
	else
	{
		/*** Otherwise, find the line number of the last header line,
		     which contains the "END" keyword.  If we can't find it,
		     we'll enter the new header line on the first line of the header. ***/
		     
		if ( ! FindFITSHeaderLine ( *header, "END     ", &linenum ) )
			linenum = 0;
		
		/*** Replace the "END" line with the new header line, then put a new
		     "END" keyword on the following line. ***/
		     	
		SetFITSHeaderLine ( header, linenum, line );
		SetFITSHeaderLine ( header, linenum + 1, "END     " );
	}
}

#endif

/*** ReadJPEGImageFile **********************************************************/

ImagePtr ReadJPEGImageFile ( GPathPtr path )
{
	FILE		*file = NULL;
	char		title[256];
	ImagePtr	image = NULL;
	GImagePtr	bitmap = NULL;

	/*** Open the file at the specified path location.  On failure, return
	     NULL. ***/
	     
	file = GOpenFile ( path, "rb", NULL, NULL );
	if ( file == NULL )
		return ( NULL );
	
	/*** Try to read a JPEG image from the file.  On failure, close the file,
	     and return NULL. ***/
	     
	bitmap = GReadJPEGImageFile ( file );
	if ( bitmap == NULL )
	{
		fclose ( file );
		return ( NULL );
	}
	
	/*** Extract the file name from the path specification to use as the
	     image title; then create a new image from the bitmap. ***/
	
	GGetPathName ( path, title );
	image = NewImageFromBitmap ( title, bitmap );
	if ( image == NULL )
	{
		fclose ( file );
		return ( NULL );
	}
	
	/*** Set the image's path specification and title to those of the
	     file we just read. ***/
	     
	SetImagePath ( image, path );
	SetImageFileFormat ( image, FILE_TYPE_JPEG );
	fclose ( file );
	
	return ( image );	
}

/*** ReadTIFFImageFile **********************************************************/

ImagePtr ReadTIFFImageFile ( GPathPtr path )
{
	FILE		*file = NULL;
	char		title[256];
	ImagePtr	image = NULL;
	GImagePtr	bitmap = NULL;

	/*** Open the file at the specified path location.  On failure, return
	     NULL. ***/
	     
	file = GOpenFile ( path, "rb", NULL, NULL );
	if ( file == NULL )
		return ( NULL );
	
	/*** Try to read a deep (i.e. bits-per-sample > 8) image from the TIFF file.
	     If we fail, try to read a normal bitmap image from the TIFF file, then
	     generate an image structure from that. ***/

	image = ReadDeepTIFFImageFile ( path );
	if ( image == NULL )
	{
		bitmap = GReadTIFFImageFile ( file );
		if ( bitmap == NULL )
		{
			fclose ( file );
			return ( NULL );
		}
		
		/*** Extract the file name from the path specification to use as the
		     image title; then create a new image from the bitmap.  If we fail,
		     close the file and return a NULL image pointer. ***/
		
		GGetPathName ( path, title );
		image = NewImageFromBitmap ( title, bitmap );
		if ( image == NULL )
		{
			fclose ( file );
			return ( NULL );
		}
	}
		
	/*** Set the image's path specification and title to those of the
	     file we just read.  Close the file and return a pointer to the
	     image. ***/
	     
	SetImagePath ( image, path );
	SetImageFileFormat ( image, FILE_TYPE_TIFF );
	
	fclose ( file );
	return ( image );	
}

/*** ReadDeepTIFFImageFile ******************************************************

	Reads a TIFF image file with a bit-depth of more than 8 bits per pixel.

	ImagePtr ReadDeepTIFFImageFile ( GPathPtr path )
	
	(path): pointer to path specification record describing the location of
	        the TIFF file to read.
	        
	If successful, the function returns a pointer to an image record containing
	data read from the TIFF file.  On failure, the function returns NULL.
	
	Pixel values from the TIFF file are converted to floating point data stored
	directly within the image's FITS data matrix.  This avoids loss of precision
	that would occur if the data were read instead through an 8-bit image structure.
	
	This function assumes that the TIFF data are organized in scanlines, and that
	there is only one sample (i.e. color channel) per pixel.  It will fail to read
	TIFF files that are organized in tiles, or that contain multiple samples per
	pixel.  It will also fail to read any color table information from the TIFF
	file, i.e. it assumes that values are grayscale.
	
*********************************************************************************/

ImagePtr ReadDeepTIFFImageFile ( GPathPtr path )
{
	unsigned long	width = 0, height = 0, row, col;
	unsigned long	tilewidth = 0, tilelength = 0;
	unsigned short	type, bitspersample = 0, samplesperpixel = 0, planarconfig = 0, photometric = 0;
	unsigned short	sampleformat = 0;
	unsigned short	*scanline = NULL;
	FILE			*file = NULL;
	TIFF			*tiff = NULL;
	FITSImage		*fits = NULL;
	ImagePtr		image = NULL;
	char			filename[256] = { '\0' };
	
	/*** Open the file at the specified path location.  On failure, return
	     NULL. ***/
	     
	file = GOpenFile ( path, "rb", NULL, NULL );
	if ( file == NULL )
		return ( NULL );

	/*** Create a TIFF record in memory.  (Note that we don't actually open the file
	     here!)  Note that this automatically reads the first TIFF Image File Directory
	     (i.e. all data associated with the first image in the TIFF file) in the file.
	     Return an error code on failure. ***/
	     
	GGetPathName ( path, filename );
	
	tiff = TIFFClientOpen ( filename, "r", file,
	       TIFFClientRead, TIFFClientWrite, TIFFClientSeek, TIFFClientClose, TIFFClientSize,
	       TIFFClientMap, TIFFClientUnmap );

	if ( tiff == NULL )
	{
		fclose ( file );
		return ( NULL );
	}
	
	/*** Obtain the TIFF image's dimensions, bit-depth, color organization, sample format,
	     etc. ***/
	
	TIFFGetField ( tiff, TIFFTAG_IMAGEWIDTH, &width );
	TIFFGetField ( tiff, TIFFTAG_IMAGELENGTH, &height );
	TIFFGetField ( tiff, TIFFTAG_TILEWIDTH, &width );
	TIFFGetField ( tiff, TIFFTAG_TILELENGTH, &height );
	TIFFGetField ( tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel );
	TIFFGetField ( tiff, TIFFTAG_BITSPERSAMPLE, &bitspersample );
	TIFFGetField ( tiff, TIFFTAG_PLANARCONFIG, &planarconfig );
	TIFFGetField ( tiff, TIFFTAG_PHOTOMETRIC, &photometric );
	TIFFGetField ( tiff, TIFFTAG_SAMPLEFORMAT, &sampleformat );
	
	/*** Now figure out what kind of image to create.  If we have an RGB TIFF file with
	     three color samples per pixel, we'll create a color image; if we have a grayscale
	     image with one color sample per pixel, we'll create a monochrome image.  We'll
	     fail if the TIFF tags indicate any other kind of image. ***/

	if ( photometric == PHOTOMETRIC_RGB && samplesperpixel == 3 )
	{
		type = IMAGE_TYPE_RGB_COLOR;
	}
	else if ( photometric == PHOTOMETRIC_MINISBLACK || photometric == PHOTOMETRIC_MINISWHITE )
	{
		type = IMAGE_TYPE_MONOCHROME;
		samplesperpixel = 1;
	}
	else
	{
		TIFFClose ( tiff );
		fclose ( file );
		return ( NULL );
	}
	
	/*** Make sure the sample organization and format are ones which we understand. ***/
	
	if ( planarconfig != PLANARCONFIG_CONTIG || tilewidth > 0 || tilelength > 0
	|| ( sampleformat != SAMPLEFORMAT_INT && sampleformat != SAMPLEFORMAT_UINT && sampleformat != SAMPLEFORMAT_IEEEFP ) )
	{
		TIFFClose ( tiff );
		fclose ( file );
		return ( NULL );
	}
	
	/*** Allocate a new image with the same pixel dimensions and type (RGB color,
	     or grayscale/monochrome) as the TIFF image.  On failure, free the TIFF
	     object, close the file, and return a NULL pointer. ***/
	
	image = NewImage ( filename, type, samplesperpixel, height, width );
	if ( image == NULL )
	{
		TIFFClose ( tiff );
		fclose ( file );
		return ( NULL );
	}
	
	/*** Allocate a temporary buffer big enough to hold one scanline of TIFF
	     image data.  On failure, free memory for the TIFF object, close the file,
	     and return a NULL pointer. ***/
	     
	scanline = malloc ( TIFFScanlineSize ( tiff ) );
	if ( scanline == NULL )
	{
		TIFFClose ( tiff );
		fclose ( file );
		return ( NULL );
	}
	
	/*** Now, for each row of imdge data in the TIFF file, read the data from the
	     file, then copy the data into the appropriate row in the FITS image data
	     matrix. ***/
	     
	fits = GetImageFITSImage ( image );
	for ( row = 0; row < height; row++ )
	{
		if ( TIFFReadScanline ( tiff, scanline, row, 0 ) == -1 )
		{
			free ( scanline );
			TIFFClose ( tiff );
			fclose ( file );
			return ( NULL );
		}
		
		for ( col = 0; col < width; col++ )
		{
			if ( samplesperpixel == 1 )
			{
				if ( sampleformat == SAMPLEFORMAT_UINT )
					fits->data[0][row][col] = ( (unsigned short *) scanline )[ col ];
				else if ( sampleformat == SAMPLEFORMAT_INT )
					fits->data[0][row][col] = ( (signed short *) scanline )[ col ];
				else if ( sampleformat == SAMPLEFORMAT_IEEEFP )
					fits->data[0][row][col] = ( (float *) scanline )[ col ];
			}
			else
			{
				if ( sampleformat == SAMPLEFORMAT_UINT )
				{
					fits->data[0][row][col] = ( (unsigned short *) scanline )[ 3 * col     ];
					fits->data[1][row][col] = ( (unsigned short *) scanline )[ 3 * col + 1 ];
					fits->data[2][row][col] = ( (unsigned short *) scanline )[ 3 * col + 2 ];
				}
				else if ( sampleformat == SAMPLEFORMAT_INT )
				{
					fits->data[0][row][col] = ( (signed short *) scanline )[ 3 * col     ];
					fits->data[1][row][col] = ( (signed short *) scanline )[ 3 * col + 1 ];
					fits->data[2][row][col] = ( (signed short *) scanline )[ 3 * col + 2 ];
				}
				else if ( sampleformat == SAMPLEFORMAT_IEEEFP )
				{
					fits->data[0][row][col] = ( (float *) scanline )[ 3 * col     ];
					fits->data[1][row][col] = ( (float *) scanline )[ 3 * col + 1 ];
					fits->data[2][row][col] = ( (float *) scanline )[ 3 * col + 2 ];
				}
			}
		}
	}
	
	/*** We're done.  Free the scanline buffer and the TIFF object. ***/
	
	free ( scanline );
	TIFFClose ( tiff );
	
	/*** Set the image's path specification and title to those of the
	     file we just read.  Close the file and return a pointer to the
	     image. ***/
	     
	SetImagePath ( image, path );
	SetImageFileFormat ( image, FILE_TYPE_TIFF );

	fclose ( file );
	return ( image );	
}

/*** WriteDeepTIFFImageFile *****************************************************

	Writes a TIFF image file with a bit-depth of more than 8 bits per pixel.

	ImagePtr ReadDeepTIFFImageFile ( FILE *file, char *name, ImagePtr image )
	
	(file): pointer to TIFF file, which must be opened for writing in binary mode.
	(name): pointer to NUL-terminated string containing file name.
	(image): pointer to image structure containing image data to write.
	(format): value indicating sample format for TIFF image file data.
	
	If successful, the function returns TRUE.  On failure, it returns FALSE.
	
	Pixel values within the image's FITS data matrix are converted to the specified
	sample format and written directly to the TIFF file.  This avoids loss of precision
	that would occur if the data were written instead through an 8-bit image structure.
	
	The (format) parameter indicates the sample format in which the data should
	be written.  The value passed in this parameter must be on of the sample format
	tags #defined in the TIFF library header file, "tiff.h"; specifically:
	
	SAMPLEFORMAT_INT  for signed 16-bit integer samples;
	SAMPLEFORMAT_UINT for unsigned 16-bit integer samples;
	SAMPLEFORMAT_IEEEFP for 32-bit IEEE floating-point samples.
	
	This function writes the TIFF data in scanlines; it does not create tiled
	images.
	
*********************************************************************************/

int WriteDeepTIFFImageFile ( ImagePtr image, FILE *file, char *filename, unsigned short sampleformat )
{
	unsigned long	width = 0, height = 0, type, row, col;
	unsigned short	bitspersample = 0, samplesperpixel = 0, planarconfig = 0, photometric = 0;
	unsigned short	*scanline = NULL;
	TIFF			*tiff = NULL;
	FITSImage		*fits = NULL;
	
	/*** Obtain the image's dimensions and type. ***/
	
	width = GetImageColumns ( image );
	height = GetImageRows ( image );
	type = GetImageType ( image );
	
	/*** If we have a color image, we'll write three samples per pixel which represent
	     the red, green, and blue color components of the pixel.  For monochrome images,
	     we'll only write one color sample per pixel which represents a grayscale value.
	     For other image types, we'll return an error code because we don't know how
	     to handle them. ***/
	      
	if ( type == IMAGE_TYPE_RGB_COLOR )
	{
		samplesperpixel = 3;
		photometric = PHOTOMETRIC_RGB;
	}
	else if ( type == IMAGE_TYPE_MONOCHROME )
	{
		samplesperpixel = 1;
		photometric = PHOTOMETRIC_MINISBLACK;
	}
	else
		return ( FALSE );
	
	/*** Determine the size of each pixel sample.  If we have an unknown pixel sample
	     format, return FALSE to indicate failure. ***/
	
	if ( sampleformat == SAMPLEFORMAT_IEEEFP )
		bitspersample = sizeof ( float ) * 8;
	else if ( sampleformat == SAMPLEFORMAT_INT || sampleformat == SAMPLEFORMAT_UINT )
		bitspersample = sizeof ( short ) * 8;
	else
		return ( FALSE );
		
	/*** Create a TIFF record in memory.  (Note that we don't actually open the file
	     here!)  Note that this automatically reads the first TIFF Image File Directory
	     (i.e. all data associated with the first image in the TIFF file) in the file.
	     Return an error code on failure. ***/
	     
	tiff = TIFFClientOpen ( filename, "w", file,
	       TIFFClientRead, TIFFClientWrite, TIFFClientSeek, TIFFClientClose, TIFFClientSize,
	       TIFFClientMap, TIFFClientUnmap );

	if ( tiff == NULL )
		return ( FALSE );
	
	/*** Now set the apprpriate TIFF tags to indicate what kind of image we're
	     writing. ***/
	
	TIFFSetField ( tiff, TIFFTAG_IMAGEWIDTH, width );
	TIFFSetField ( tiff, TIFFTAG_IMAGELENGTH, height );
	TIFFSetField ( tiff, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel );
	TIFFSetField ( tiff, TIFFTAG_BITSPERSAMPLE, bitspersample );
	TIFFSetField ( tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG );
	TIFFSetField ( tiff, TIFFTAG_PHOTOMETRIC, photometric );
	TIFFSetField ( tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE );
	TIFFSetField ( tiff, TIFFTAG_SAMPLEFORMAT, sampleformat );
	
	/*** Allocate a temporary buffer big enough to hold one scanline of TIFF
	     image data. ***/
	     
	scanline = malloc ( TIFFScanlineSize ( tiff ) );
	if ( scanline == NULL )
	{
		TIFFClose ( tiff );
		return ( FALSE );
	}
	
	/*** For each row in the image, copy data from the FITS image array
	     into the temporary scanline buffer, then write the scanline to
	     the TIFF file. ***/
	     
	fits = GetImageFITSImage ( image );
	for ( row = 0; row < height; row++ )
	{
		/*** If we have a monochome image, simply copy the pixel values from
		     each row of the image data matrix into the scanline buffer; if
		     we have a color image, copy the pixel values from the read, green,
		     and blue frames of the image into the scanline buffer so that
		     color samples are arranged RGBRGBRGBRGB etc. ***/
		
		if ( samplesperpixel == 1 )
		{
			for ( col = 0; col < width; col++ )
			{
				if ( sampleformat == SAMPLEFORMAT_UINT )
					( (unsigned short *) scanline )[ col ] = fits->data[0][row][col];
				else if ( sampleformat == SAMPLEFORMAT_INT )
					( (signed short *) scanline )[ col ] = fits->data[0][row][col];
				else if ( sampleformat == SAMPLEFORMAT_IEEEFP )
					( (float *) scanline )[ col ] = fits->data[0][row][col];
			}
		}
		else
		{
			for ( col = 0; col < width; col++ )
			{
				if ( sampleformat == SAMPLEFORMAT_UINT )
				{
					( (unsigned short *) scanline )[ 3 * col     ] = fits->data[0][row][col];
					( (unsigned short *) scanline )[ 3 * col + 1 ] = fits->data[1][row][col];
					( (unsigned short *) scanline )[ 3 * col + 2 ] = fits->data[2][row][col];
				}	
				else if ( sampleformat == SAMPLEFORMAT_INT )
				{
					( (signed short *) scanline )[ 3 * col     ] = fits->data[0][row][col];
					( (signed short *) scanline )[ 3 * col + 1 ] = fits->data[1][row][col];
					( (signed short *) scanline )[ 3 * col + 2 ] = fits->data[2][row][col];
				}
				else if ( sampleformat == SAMPLEFORMAT_IEEEFP )
				{
					( (float *) scanline )[ 3 * col     ] = fits->data[0][row][col];
					( (float *) scanline )[ 3 * col + 1 ] = fits->data[1][row][col];
					( (float *) scanline )[ 3 * col + 2 ] = fits->data[2][row][col];
				}
			}
		}
		
		/*** Now write the scanline to the TIFF file.  On failure, free the scanline
		     buffer and TIFF file memory, then return FALSE to indicate failure. ***/
		     
		if ( TIFFWriteScanline ( tiff, scanline, row, 0 ) == -1 )
		{
			free ( scanline );
			TIFFClose ( tiff );
			return ( FALSE );
		}
	}
	
	/*** Free memory for the scanline, close the TIFF file, and return a
	     successful result code. ***/
	     
	free ( scanline );
	TIFFClose ( tiff );
	return ( TRUE );	
}


/*** ReadGIFImageFile **********************************************************/

ImagePtr ReadGIFImageFile ( GPathPtr path )
{
	FILE		*file = NULL;
	char		title[256];
	ImagePtr	image = NULL;
	GImagePtr	bitmap = NULL;

	/*** Open the file at the specified path location.  On failure, return
	     NULL. ***/
	     
	file = GOpenFile ( path, "rb", NULL, NULL );
	if ( file == NULL )
		return ( NULL );
	
	bitmap = GReadGIFImageFile ( file );
	if ( bitmap == NULL )
	{
		fclose ( file );
		return ( NULL );
	}
	
	/*** Extract the file name from the path specification to use as the
	     image title; then create a new image from the bitmap.  If we fail,
	     close the file and return a NULL image pointer. ***/
	
	GGetPathName ( path, title );
	image = NewImageFromBitmap ( title, bitmap );
	if ( image == NULL )
	{
		fclose ( file );
		return ( NULL );
	}
		
	/*** Set the image's path specification and title to those of the
	     file we just read.  Close the file and return a pointer to the
	     image. ***/
	     
	SetImagePath ( image, path );
	SetImageFileFormat ( image, FILE_TYPE_GIF );
	
	fclose ( file );
	return ( image );	
}

/*** ReadPICTImageFile **********************************************************/

ImagePtr ReadPICTImageFile ( GPathPtr path )
{
	FILE		*file = NULL;
	char		title[256];
	ImagePtr	image = NULL;
	GImagePtr	bitmap = NULL;

	/*** Open the file at the specified path location.  On failure,
	     return NULL. ***/
	     
	file = GOpenFile ( path, "rb", NULL, NULL );
	if ( file == NULL )
		return ( NULL );
	
	/*** Try to read a 32-bit image from the PICT file.  On failure,
	     close the file and return NULL. ***/
	     
	bitmap = GReadImageFile ( file, 32 );
	if ( bitmap == NULL )
	{
		fclose ( file );
		return ( NULL );
	}
	
	/*** Extract the file name from the path specification to use as the
	     image title; then create a new image from the bitmap. ***/
	
	GGetPathName ( path, title );
	image = NewImageFromBitmap ( title, bitmap );
	
	/*** Set the image's path specification and title to those of the
	     file we just read. ***/
	     
	SetImagePath ( image, path );
	SetImageFileFormat ( image, FILE_TYPE_PICT );
	
	return ( image );	
}

/*** DeleteImage ******************************************************************

	Destroys and releases memory occupied by an image record.

	void DeleteImage ( ImagePtr image )

	(image): pointer to image record you wish to destroy.
	
	On return, the image pointer will be invalid.
		
***********************************************************************************/

void DeleteImage ( ImagePtr image )
{
	if ( image->imageFITSImage != NULL )
		FreeFITSImage ( image->imageFITSImage );
		
	if ( image->imagePath != NULL )
		GDeletePath ( image->imagePath );
		
	DeleteImageObjectList ( image );
	free ( image );
}

/*** GetImageType *****************************************************************

	Returns a short integer which specifies how the data in an image should be
	interpreted.
	
	short GetImageType ( ImagePtr image )

	(image): pointer to image record.

	Currently, there are three supported types of images, as #defined in
	"SkySight.h":
	
	- IMAGE_TYPE_MONOCHROME.  Each frame of image data represents a single color
	channel.  There is an unlimited number of frames per image.
	
	- IMAGE_TYPE_RGB_COLOR.  There are at least three frames per image; the total
	number of frames must be a multiple of 3.  Within each triplet of frames, the
	first frame represents the red color channel; the second frame represents the
	green color channel; and the third frame represents the blue color channel.
	
	- IMAGE_TYPE_COMPLEX.  There are at least two frames per image, and the total
	number of frames must be a multiple of two.  The first frame in each pair
	represents the real part of the image, and the second frame in each pair
	represents the complex part.
	
************************************************************************************/

short GetImageType ( ImagePtr image )
{
	return ( image->imageType );
}

/*** GetImageFileFormat ************************************************************

	Returns the image's current file format.
	
	short GetImageFileFormat ( ImagePtr image )
	
	(image): pointer to image record.
	
	The image file format will be one of the file type specifiers #defined in
	"SkySight.h", e.g. FILE_TYPE_FITS, etc.
	
	This is the format of the file on disk which corresponds to the image.
	If the image has no file on disk (e.g. it hasn't been saved yet), the
	file format is the format in which it should be saved by default.
	
************************************************************************************/

short GetImageFileFormat ( ImagePtr image )
{
	return ( image->imageFileFormat );
}

/*** SetImageFileFormat ***********************************************************
	
	Sets an image's file format.
	
	void SetImageFileFileFormat ( ImagePtr image, short format )
	
	(image): pointer to image record.
	(format): specifies image's new file format type code.
	
	See GetImageFileFormat() for a more complete description.
		
***********************************************************************************/

void SetImageFileFormat ( ImagePtr image, short format )
{
	image->imageFileFormat = format;
}

/*** GetImageFrames ****************************************************************

	Returns the number of frames in an image.
	
	short GetImageColumns ( ImagePtr image )

	(image): pointer to image record.

	An image can contain one or more frames of image data.  Each frame is a
	rectangular matrix of rows and columns of pixels; all frames have the same
	dimensions, which can be determined by calling GetImageRows() and GetImageColumns().
	The interpretation of the data in each frame depends on the image type.
	
************************************************************************************/

short GetImageFrames ( ImagePtr image )
{
	return ( image->imageFITSImage->naxis3 );
}

/*** GetImageColumns ****************************************************************

	Returns the width of an image frame, in pixels.
	
	short GetImageColumns ( ImagePtr image )

	(image): pointer to image record.

	Please note that every frame of a multi-frame image has the same number
	of columns.
	
************************************************************************************/

short GetImageColumns ( ImagePtr image )
{
	return ( image->imageFITSImage->naxis1 );
}

/*** GetImageRows **************************************************************

	Returns the height of an image frame, in pixels.
	
	short GetImageColumns ( ImagePtr image )

	(image): pointer to image record.

	Please note that every frame of a multi-frame image has the same number
	of rows.
	
********************************************************************************/

short GetImageRows ( ImagePtr image )
{
	return ( image->imageFITSImage->naxis2 );
}

/*** GetImageDataFrame ************************************************************

	Returns a pointer to a single frame of image data.
	
	PIXEL **GetImageDataFrame ( ImagePtr image, short frame )

	(image): pointer to image record.
	(frame): desired frame number.
	
	The function returns a pointer to the start of the specified image data row.
	The frame number is counted from zero; i.e., frame zero is the first frame
	in the image, and row zero is the first (top) row in each frame.
	
************************************************************************************/

PIXEL **GetImageDataFrame ( ImagePtr image, short frame )
{
	return ( image->imageFITSImage->data[frame] );
}

/*** GetImageDataRow **************************************************************

	Returns a pointer to a single row of image data.
	
	PIXEL *GetImageDataRow ( ImagePtr image, short frame, short row )

	(image): pointer to image record.
	(frame): desired frame number.
	(row):   desired row number.
	
	The function returns a pointer to the start of the specified image data row.
	Both the frame and row number are counted from zero; i.e., frame zero is the
	first frame in the image, and row zero is the first (top) row in each frame.
	
************************************************************************************/

PIXEL *GetImageDataRow ( ImagePtr image, short frame, short row )
{
	return ( image->imageFITSImage->data[frame][row] );
}

/*** GetImageDataValue **************************************************************

	Obtains the value of a single pixel of image data.
	
	PIXEL GetImageDataValue ( ImagePtr image, short frame, short row, short col )

	(image): pointer to image record.
	(frame): desired frame number.
	(row):   desired row number.
	(col):   desired column number.
	
	The function returns the value of the specified image pixel.
	
	The frame, row, and column number are all counted from zero; i.e., frame zero
	is the first frame in the image, row zero is the first (top) row in each frame,
	and column zero is the first (leftmost) column in each row.
	
************************************************************************************/

PIXEL GetImageDataValue ( ImagePtr image, short frame, short row, short col )
{
	return ( image->imageFITSImage->data[frame][row][col] );
}

/*** SetImageDataValue **************************************************************

	Modifies the value of a single pixel of image data.
	
	void SetImageDataValue ( ImagePtr image, short frame, short row, short col,
	     PIXEL value )

	(image): pointer to image record.
	(frame): desired frame number.
	(row):   desired row number.
	(col):   desired column number.
	(value): desired value for pixel.
	
	The function returns nothing.
	
	The frame, row, and column number are all counted from zero; i.e., frame zero
	is the first frame in the image, row zero is the first (top) row in each frame,
	and column zero is the first (leftmost) column in each row.
	
************************************************************************************/

void SetImageDataValue ( ImagePtr image, short frame, short row, short col,
PIXEL value )
{
	image->imageFITSImage->data[frame][row][col] = value;
}

/*** GetImageFITSImage ************************************************************

	Returns a pointer to an image's FITS image information record.

	FITSImagePtr GetImageFITSImage ( ImagePtr image )
	
	(image): pointer to image record.
	
************************************************************************************/

FITSImagePtr GetImageFITSImage ( ImagePtr image )
{
	return ( image->imageFITSImage );
}

/*** SetImageFITSImage ************************************************************

	Stores a pointer to a FITS image information record into an image record.

	void SetImageFITSImage ( ImagePtr image, FITSImagePtr fits )
	
	(image): pointer to image record.
	(fits):  pointer to FITS image information record to store into the image.
	
***********************************************************************************/

void SetImageFITSImage ( ImagePtr image, FITSImagePtr fits )
{
	image->imageFITSImage = fits;
}

/*** GetImageTitle *****************************************************************

	Returns a pointer to an image's title string.
	
	char *GetImageTitle ( ImagePtr image )

	(image): pointer to image record.
	
	The function returns a pointer to the image's title string.
	
***********************************************************************************/

char *GetImageTitle ( ImagePtr image )
{
	return ( image->imageTitle );
}


/*** SetImageTitle *****************************************************************

	Modifies an image's title string.
	
	void GetImageTitle ( ImagePtr image, char *title )

	(image): pointer to image record.
	(title): ASCII NUL-terminated C string containing desired image title.
	
	The function returns nothing.
	
	In addition to changing the image's title, this function modifies the
	titles of the window(s) in which the image, the image's FITS header,
	and the image's histogram, are displayed.
	
***********************************************************************************/

void SetImageTitle ( ImagePtr image, char *title )
{
	GWindowPtr window;
	
	strncpy ( image->imageTitle, title, sizeof ( image->imageTitle ) );
	
	window = GetImageWindow ( image );
	if ( window != NULL )
		GSetWindowTitle ( window, title );
		
	window = GetImageHeaderWindow ( image );
	if ( window != NULL )
		SetHeaderWindowTitle ( window );

	window = GetImageHistogramWindow ( image );
	if ( window != NULL )
		SetHistogramWindowTitle ( window );
}

/*** GetImagePath *****************************************************************

	Returns a pointer to an image's path specification record.
	
	GPathPtr GetImagePath ( ImagePtr image )

	(image): pointer to image record.
	
	The path specification record returned describes the location of the image
	file where the image should be stored.  If the image has no file (i.e. it
	exists only in memory) then the function returns NULL.
	
***********************************************************************************/

GPathPtr GetImagePath ( ImagePtr image )
{
	return ( image->imagePath );
}

/*** SetImagePath *****************************************************************

	Sets an image's path specification record.
	
	void SetImagePath ( ImagePtr image, GPathPtr path )

	(image): pointer to image record.
	(path):  pointer to path specification for image file.
	
	The image's path specification record describes the location of the file
	where the image will be stored on disk.  If the image already has a path
	specification record, it will be destroyed when this function is called.
	
	You can pass NULL for (path) to dissociate the image from any file on disk.
	
	If (path) points to a valid path specification record, the image's title
	string will be set to the file title contained in the path.  Additionally,
	if the image has an associated image window, the window's title will be set
	to the file title contained in the path specification record.
	
***********************************************************************************/

void SetImagePath ( ImagePtr image, GPathPtr path )
{
	char	title[256];
	
	/*** If the specified path is already the same as the image's existing
	     path, we don't need to change anything! ***/
	     
	if ( path != image->imagePath )
	{
		/*** If the image already has a path specification record, delete it. ***/
		
		if ( image->imagePath != NULL )
			GDeletePath ( image->imagePath );
		
		/*** Make a copy of the path specification record and store it in the
		     image record.  Use the file title contained in the path specification
		     as the image title. ***/
		     	
		if ( path != NULL )
		{
			image->imagePath = GCreatePath ( path );
			GGetPathName ( path, title );
			SetImageTitle ( image, title );
		}
		else
		{
			image->imagePath = NULL;
		}
	}
}

/*** GetImageWindow ****************************************************************

	Returns a pointer to the window (if any) in which an image is displayed.

	GWindowPtr GetImageWindow ( ImagePtr image )
	
	(image): pointer to image record.
	
	The function returns a pointer to the image's associated window, if it has
	one.  If the image is not displayed in any window, the function returns NULL.
	
************************************************************************************/

GWindowPtr GetImageWindow ( ImagePtr image )
{
	return ( image->imageWindow );
}

/*** SetImageWindow ****************************************************************

	Associates an image record with a window.

	void SetImageWindow ( ImagePtr image, GWindowPtr window )
	
	(image): pointer to image record you wish to associate with a window.
	
	The window passed to this function will become the window in which the
	image is displayed.  You can pass NULL as the window pointer to dissociate
	the image from any window.

	If you pass a valid window pointer to this function, the window's title is set
	to the file name contained in the image's path specification record.
	
************************************************************************************/

void SetImageWindow ( ImagePtr image, GWindowPtr window )
{
	image->imageWindow = window;

	if ( window != NULL )
	{
		if ( GetImageWindowImage ( window ) != image )
		{
			SetImageWindowImage ( window, image );
			GSetWindowTitle ( window, image->imageTitle );
		}
	}
}

/*** GetImageHeaderWindow **********************************************************

	Returns a pointer to the window (if any) in which an image's FITS header
	is displayed.

	GWindowPtr GetImageHeaderWindow ( ImagePtr image )
	
	(image): pointer to image record.
	
	The function returns a pointer to the image's associated header window, if
	it has one.  If the image's FITS header is not being displayed, the function
	returns NULL.
	
************************************************************************************/

GWindowPtr GetImageHeaderWindow ( ImagePtr image )
{
	return ( image->imageHeaderWindow );
}

/*** SetImageHeaderWindow **********************************************************

	Associates an image record with a FITS header window.

	void SetImageHeaderWindow ( ImagePtr image, GWindowPtr window )
	
	(image): pointer to image record you wish to associate with a window.
	
	The window passed to this function will become the window in which the
	image's FITS header is displayed.  You can pass NULL as the window pointer
	to dissociate the image from any header window.
	
************************************************************************************/

void SetImageHeaderWindow ( ImagePtr image, GWindowPtr window )
{
	image->imageHeaderWindow = window;
	
	/*** If the header window is not already linked back to the image,
	     create that link now, which will have the side-effect of making
	     sure that the image's header text is displayed in the window. ***/
	
	if ( window != NULL )
		if ( GetHeaderWindowImage ( window ) != image )
			SetImageHeaderWindow ( image, window );
}

/*** GetImageHistogramWindow **********************************************************

	Returns a pointer to the window (if any) in which an image's histogram
	is displayed.

	GWindowPtr GetImageHistogramWindow ( ImagePtr image )
	
	(image): pointer to image record.
	
	The function returns a pointer to the image's associated histogram window, if
	it has one.  If the image's FITS header is not being displayed, the function
	returns NULL.
	
************************************************************************************/

GWindowPtr GetImageHistogramWindow ( ImagePtr image )
{
	return ( image->imageHistogramWindow );
}

/*** SetImageHistogramWindow *******************************************************

	Associates an image record with a histogram window.

	void SetImageHistogramWindow ( ImagePtr image, GWindowPtr window )
	
	(image): pointer to image record you wish to associate with a window.
	(window): pointer to window you wish to associate with the image.
	
	The window passed to this function will become the window in which the
	image's histogram is displayed.  You can pass NULL as the window pointer
	to dissociate the image from any histogram window.
	
************************************************************************************/

void SetImageHistogramWindow ( ImagePtr image, GWindowPtr window )
{
	image->imageHistogramWindow = window;
	
	/*** If the histogram window is not already linked back to the image,
	     create that link now, which will have the side-effect of making
	     sure that the image's header text is displayed in the window. ***/
	
	if ( window != NULL )
		if ( GetHistogramWindowImage ( window ) != image )
			SetImageHistogramWindow ( image, window );
}


float GetImageTemperature ( ImagePtr image )
{
	return ( image->imageTemperature );
}

void SetImageTemperature ( ImagePtr image, float temp )
{
	image->imageTemperature = temp;
}

float GetImageExposureLength ( ImagePtr image )
{
	return ( image->imageExposureLength );
}

void SetImageExposureLength ( ImagePtr image, float length )
{
	image->imageExposureLength = length;
}

/*** ComputeImageStatistics *********************************************

	Given a pointer to an image, computes the image's basic statistics
	(minimum, maximum, mean, and standard deviation) and histogram.

	void ComputeImageStatistics ( ImagePtr image )
	
	(image): pointer to image record.
	
	The function returns nothing.
	
***************************************************************************/

void ComputeImageStatistics ( ImagePtr image )
{
	short		numCols = GetImageColumns ( image );
	short		numRows = GetImageRows ( image );
	short		numFrames = GetImageFrames ( image );
	short		col, row, frame;
	double		sum, sum2, mean, stdev;
	long		n;
	PIXEL		min, max, value, *data;
	long		*histogram, numBins, bin;
	float		scale;
	
	/*** Pass through image data to find minimum, maximum,
	     mean, and standard deviation of image data values. ***/
	     
	min = GetImageDataValue ( image, 0, 0, 0 );
	max = min;
	
	sum = sum2 = n = 0;
	
	for ( frame = 0; frame < numFrames; frame++ )
	{
		for ( row = 0; row < numRows; row++ )
		{
			data = GetImageDataRow ( image, frame, row );
		
			for ( col = 0; col < numCols; col++ )
			{
				value = data[col];
			
				if ( value < min )
					min = value;
			
				if ( value > max )
					max = value;
				
				sum += value;
				sum2 += value * value;
				n++;
			}
		}
	}
	
	mean = sum / n;
	stdev = sqrt ( ( sum2 - 2.0 * sum * mean + n * mean * mean ) / ( n - 1 ) );

	image->imageMinimum = min;
	image->imageMaximum = max;
	image->imageMean = mean;
	image->imageStdDev = stdev;

	/*** Pass through image data to generate histogram of image data values ***/
	
	histogram = GetImageHistogram ( image, &numBins );
	
	for ( bin = 0; bin < numBins; bin++ )
		histogram[bin] = 0;
		
	if ( max == min )
		scale = 0.0;
	else
		scale = numBins / ( max - min );
	
	for ( frame = 0; frame < numFrames; frame++ )
	{
		for ( row = 0; row < numRows; row++ )
		{
			data = GetImageDataRow ( image, frame, row );
	
			for ( col = 0; col < numCols; col++ )
			{
				bin = scale * ( data[col] - min );
				
				if ( bin < 0 )
					bin = 0;
					
				if ( bin >= numBins )
					bin = numBins - 1;
					
				histogram[ bin ]++;
			}
		}
	}
}

/*** GetImageMininumValue **********************************************************

	Returns an image's minimum data value.
	
	PIXEL GetImageMininumValue ( ImagePtr image )
	
	(image): pointer to image record.
	
	The value returned by this function will be invalid unless you have
	previously called ComputeImageStatistics().
	
************************************************************************************/

PIXEL GetImageMinimumValue ( ImagePtr image )
{
	return ( image->imageMinimum );
}

/*** GetImageMaximumValue **********************************************************

	Returns an image's maximum data value.
	
	PIXEL GetImageMaxinumValue ( ImagePtr image )
	
	(image): pointer to image record.

	The value returned by this function will be invalid unless you have
	previously called ComputeImageStatistics().
	
************************************************************************************/

PIXEL GetImageMaximumValue ( ImagePtr image )
{
	return ( image->imageMaximum );
}

/*** GetImageMeanValue *************************************************************

	Returns an image's mean data value.
	
	PIXEL GetImageMeanValue ( ImagePtr image )
	
	(image): pointer to image record.

	The value returned by this function will be invalid unless you have
	previously called ComputeImageStatistics().
	
************************************************************************************/

double GetImageMeanValue ( ImagePtr image )
{
	return ( image->imageMean );
}

/*** GetImageStandardDeviation ****************************************************

	Returns the standard deviation of the image's data values.
	
	double GetImageStandardDeviation ( ImagePtr image )
	
	(image): pointer to image record.

	The value returned by this function will be invalid unless you have
	previously called ComputeImageStatistics().
	
************************************************************************************/

double GetImageStandardDeviation ( ImagePtr image )
{
	return ( image->imageStdDev );
}

/*** GetImageHistogram *************************************************************

	Returns an array of long integers which respresents a histogram of the image
	data.
	
	long *GetImageHistogram ( ImagePtr image, long *bins )
	
	(image): pointer to image record.
	(bins):  recieves number of bins in the histogram.
	
	The function returns a pointer to the image histogram.  If (bins) is non-NULL,
	then it recieves the number of bins in the histogram.
	
************************************************************************************/

long *GetImageHistogram ( ImagePtr image, long *bins )
{
	if ( bins != NULL )
		*bins = IMAGE_HISTOGRAM_BINS;
		
	return ( image->imageHistogram );
}

/*** GetImagePercentile ************************************************************

	Returns the value of a particular percentile of the image data histogram.
	
	PIXEL GetImagePercentile ( ImagePtr image, double percentile )
	
	(image): pointer to image record.
	(percentile): desired percentile of image data.
	
	The value returned by this function will be invalid unless you have
	previously called ComputeImageStatistics().
	
	The percentile desired should be expressed as a number from 0.0 to 100.0.
	 
************************************************************************************/

PIXEL GetImagePercentile ( ImagePtr image, double percentile )
{
	long		*histogram, numBins, bin, sum, rank;
	PIXEL		min, max, value;
	
	/*** Find the rank which corresponds to the requested percentile (i.e.
	     find the Nth brightest pixel in the image, which corresponds to the
	     requested percentile. ***/
	
	rank = percentile
	     * GetImageRows ( image ) * GetImageColumns ( image ) * GetImageFrames ( image )
	     / 100.0;

	/*** Now sum the numbers of pixels in the bins of the histogram, stopping
	     when the sum exceeds the rank we computed above. ***/
	     
	histogram = GetImageHistogram ( image, &numBins );

	for ( sum = bin = 0; bin < numBins; bin++ )
	{
		sum += histogram[bin];
		if ( sum > rank )
			break;
	}		
	
	/*** Compute and return the image data value which corresponds to the bin
	     number. ***/
	
	min   = GetImageMinimumValue ( image );
	max   = GetImageMaximumValue ( image );
	value = bin * ( max - min ) / numBins + min;
	
	return ( value );
}

/*** UpdateImage *******************************************************************

	Updates all displays of image data associated with an image.
	
	void UpdateImage ( ImagePtr image )

	(image): pointer to an image.
	
	This function returns nothing.
	
	You should call this function after changing an image's underlying data.
	This function will:
	
	1) recompute the image's statistics and display range;
	2) redraw the image's offscreen bitmap;
	3) invalidate the image window which will cause a redrawing;
	4) set the image's "needs save" flag, indicating that the image in memory
	   differs from its file on disk;
	5) recompute and redisplay the statics of the selected image region;
	6) recompute and redisplay the image's histogram (if present).
	
************************************************************************************/

void UpdateImage ( ImagePtr image )
{
	ImageRegionPtr	region;
	GWindowPtr		window;
	
	/*** Compute the image's display range; draw the image's
	     offscreen bitmap, and mark the image as needing to be saved
	     since the image data has changed. ***/

	window = GetImageWindow ( image );
	
	ComputeImageDisplayRange ( window );
	DrawImageWindowBitmap ( window );
	SetImageWindowNeedsSave ( window, TRUE );
	GInvalidateWindow ( window, NULL );

	/*** If the "Image Display" dialog panel is present, update the
	     positions of the sliders since the image min and max might
	     have changed. ***/
	
	SetImageDisplayDialogItems ( window );
	
    /*** If the image window has a selected region, compute its
         statistics.  If the image window is the active image window,
         display those statistics. ***/

	region = GetImageWindowSelectedRegion ( window );
    if ( region != NULL )
    {
    	ComputeImageRegionStatistics ( window, region );

        if ( window == GetActiveImageWindow() )
        	ShowImageRegionStatistics ( region );
    }

	/*** If the image has an associated histogram window, compute
	     the histogram of the selected region, then invalidate the
	     histogram window so the histogram gets redrawn. ***/
	
	window = GetImageHistogramWindow ( image );
	if ( window != NULL )
	{
		ComputeHistogramWindowHistogram ( window );
		GInvalidateWindow ( window, NULL );
	}	
}

/*** GetImageObjectList ************************************************************

	Returns a pointer to an image's linked list of object records.
	
	ImageObjectPtr GetImageObjectList ( ImagePtr image )
	
	The function returns a pointer to the first object in the list, or NULL if
	the image contains no objects.
	
	Once you have obtained a pointer to the first object, you can obtain a pointer
	to subsequent objects with GetNextImageObject().
	
************************************************************************************/

ImageObjectPtr GetImageObjectList ( ImagePtr image )
{
	return ( image->imageObjectList );
}

/*** GetImageObjectCount ************************************************************

	Returns the number of object records in an image's object list.
	
	long GetImageObjectCount ( ImagePtr image )

	(image): pointer to image record.
		
*************************************************************************************/

long GetImageObjectCount ( ImagePtr image )
{
	return ( image->imageObjectCount );
}

/*** AddImageObject *****************************************************************

	Adds an image object record to an image's linked list of object records.

	void AddImageObject ( ImagePtr image, ImageObjectPtr object )

	(image): pointer to image record
	(object): pointer to object record which should be added to the image.
	
*************************************************************************************/

void AddImageObject ( ImagePtr image, ImageObjectPtr object )
{
	SetNextImageObject ( object, image->imageObjectList );
	image->imageObjectList = object;
	image->imageObjectCount++;
}

/*** RemoveImageObject **************************************************************

	Removes an object record from an image's linked list of object records.

	void RemoveImageObject ( ImagePtr image, ImageObjectPtr object )

	(image): pointer to image record
	(object): pointer to object record which should be removed from the image.
	
	The function searches the image's linked-list of object records, and removes
	the specified object record from it, if found.  If the specified object is
	not found, then the image's object list is unchanged.
	
*************************************************************************************/

void RemoveImageObject ( ImagePtr image, ImageObjectPtr object )
{
	ImageObjectPtr imageObject, nextObject;
	
	if ( object == GetImageObjectList ( image ) )
	{
		image->imageObjectList = GetNextImageObject ( object );
		image->imageObjectCount--;
	}
	else
	{
		for ( imageObject = GetImageObjectList ( image ); imageObject != NULL; imageObject = nextObject )
		{
			nextObject = GetNextImageObject ( imageObject );
			if ( nextObject == object )
			{
				SetNextImageObject ( imageObject, GetNextImageObject ( object ) );
				image->imageObjectCount--;
				break;
			}
		}
	}
}

/*** DeleteImageObjectList **********************************************************

	Deletes all records in an image's object list.

	void DeleteImageObjectList ( ImagePtr image )
	
*************************************************************************************/

void DeleteImageObjectList ( ImagePtr image )
{
	ImageObjectPtr	object, nextObject;
	
	for ( object = image->imageObjectList; object != NULL; object = nextObject )
	{
		nextObject = GetNextImageObject ( object );
		DeleteImageObject ( object );
	}
	
	image->imageObjectList = NULL;
	image->imageObjectCount = 0;
}

/*** GetImagePreviousImage ***********************************************************

	Returns a pointer to an image's previous image record.  The previous image is
	used to restore an image to the state it was in before a particular change was
	applied, i.e. to implement the "Undo" command for images.
	
	ImagePtr GetImagePreviousImage ( ImagePtr image )

	(image): pointer to an image record.
	
	The function returns a pointer to the image's previous image record.  If the
	pointer returned is NULL, the image has no previous image record; hence no
	"Undo" is possible for that image.
		
	Please note: DO NOT use the function DeleteImage() to dispose of the image
	pointer returned by this function, since it may share data with the main image
	to which it belongs.  Use DeleteImagePreviousImage() instead.
	
***************************************************************************************/

ImagePtr GetImagePreviousImage ( ImagePtr image )
{
	return ( image->imagePreviousImage );
}

/*** PrepareImagePreviousImage *******************************************************

	Clones an image record and stores the clone as the image's "previous" image
	record.

	ImagePtr PrepareImagePreviousImage ( ImagePtr image )

	(image): pointer to an image record.
	
	The function returns a pointer to an image record which is a clone of the
	image record passed (image).  A copy of this clone pointer is also stored
	in the image record itself, and can be obtained with GetImagePreviousImage().
	
**************************************************************************************/

ImagePtr PrepareImagePreviousImage ( ImagePtr image )
{
	ImagePtr	previousImage = NULL;
	
	/*** Allocate a new image record.  On failure, return a NULL pointer. ***/
	
	previousImage = (ImagePtr) malloc ( sizeof ( Image ) );
	if ( previousImage == NULL )
		return ( NULL );

	/*** If the image already has a previous image record, dispose of it
	     to avoid leaking memory. ***/
	     
	if ( image->imagePreviousImage != NULL )
		DeleteImagePreviousImage ( image );
		
	/*** Copy the main image's data into the previous image record. ***/
	
	*previousImage = *image;
	
	/*** Store a pointer to the previous image record in the image itself,
	     then return a pointer to the previous image record. ***/
	
	image->imagePreviousImage = previousImage;
		
	return ( previousImage );
}

/*** RestoreImagePreviousImage **************************************************

	Restores an image record to the state saved in its previous image record.
	
	void RestoreImagePreviousImage ( ImagePtr image )

	(image): pointer to image which should be restored.
	
	The function returns TRUE if it is able to restore the image to its
	previous state, and FALSE otherwise.
	
	After this function returns, the image's previous state record will be
	deleted (i.e. NULL).
	
**********************************************************************************/

int RestoreImagePreviousImage ( ImagePtr image )
{
	Image	previousImage;
	
	/*** If the image has no saved, previous image state, do nothing.
	     Otherwise, make a local copy of the image's saved previous state. ***/
	
	if ( image->imagePreviousImage == NULL )
		return ( FALSE );
	else
		previousImage = *(image->imagePreviousImage);
		
	/*** For each component of the previous image which differs from the
	     image's current state, swap the previous and current values. ***/
	
	if ( previousImage.imageFITSImage != image->imageFITSImage )
	{
		image->imagePreviousImage->imageFITSImage = image->imageFITSImage;
		image->imageFITSImage = previousImage.imageFITSImage;
	}
	
	/*** Now release memory for the image's saved previous state record,
	     and return a successful result code. ***/
	
	DeleteImagePreviousImage ( image );
	return ( TRUE );
}

/*** DeleteImagePreviousImage *********************************************************

	Disposes an image's previous image record.  The previous image is used to restore
	an image to the state it was in before a particular change was applied, i.e. to
	implement the "Undo" command for images.
	
	void DeleteImagePreviousImage ( ImagePtr image )

	(image): pointer to an image record.
	
	The function returns a pointer to the image's previous image record.  If the
	pointer returned is NULL, the image has no previous image record; hence no
	"Undo" is possible for that image.
		
	Please note: Make sure that the image pointer you are passing here is a pointer
	to the main image record, not the previous image record pointer returned by
	GetImagePreviousImage().
	
***************************************************************************************/

void DeleteImagePreviousImage ( ImagePtr image )
{
	ImagePtr	previousImage = image->imagePreviousImage;
	
	/*** If the image has no saved, previous image state, do nothing. ***/
	
	if ( previousImage == NULL )
		return;
		
	/*** Only release memory for the previous image's various subcomponents
	     if they differ from the main image's. ***/
	     
	if ( previousImage->imagePath != image->imagePath )
		GDeletePath ( previousImage->imagePath );
		
	if ( previousImage->imageFITSImage != image->imageFITSImage )
		FreeFITSImage ( previousImage->imageFITSImage );
	
	if ( previousImage->imageObjectList != image->imageObjectList )
		DeleteImageObjectList ( previousImage );

	/*** Now free the previous image record itself, and set the main image's
	     previous image pointer to NULL. ***/
	     
	free ( previousImage );
	image->imagePreviousImage = NULL;
}
