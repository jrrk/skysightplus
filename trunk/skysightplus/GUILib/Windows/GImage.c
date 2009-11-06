/*** COPYRIGHT NOTICE AND PUBLIC SOURCE LICENSE *********************************

Portions Copyright (c) 1992-2001 Southern Stars Systems.  All Rights Reserved.

This file contains Original Code and/or Modifications of Original Code as defined
in and that are subject to the Southern Stars Systems Public Source License
Version 1.0 (the 'License').  You may not use this file except in compliance with
the License.  Please obtain a copy of the License at

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

*********************************************************************************/

#include "GUILib.h"
#include "GUIPriv.h"

typedef struct GImage
{
	BITMAPINFOHEADER	imageFormat;
	RGBQUAD				imageColorTable[256];
	unsigned char		*imageData;
	BITMAP				*imageBitmap;
}
GImage;

/***  GCreateImage  *********************************************************/

GImagePtr GCreateImage ( short wWidth, short wHeight, short wBitsPerPixel )
{
	short				wNumColors;
	long				lBytesPerRow;
	HDC					hDC;
	GImagePtr			pImage;

	if ( wBitsPerPixel == 0 )
	{
		hDC = GetDC ( NULL );
		wBitsPerPixel = GetDeviceCaps ( hDC, BITSPIXEL ) * GetDeviceCaps ( hDC, PLANES );
		ReleaseDC ( NULL, hDC );
	}

	if ( wBitsPerPixel > 8 )
		wNumColors = 0;
	else
		wNumColors = 1L << wBitsPerPixel;
	
	lBytesPerRow = G_BITMAP_ROW_BYTES ( wBitsPerPixel, wWidth );

	pImage = (GImagePtr) malloc ( sizeof ( GImage ) );
	if ( pImage == NULL )
		return ( NULL );

	pImage->imageFormat.biSize = sizeof ( BITMAPINFOHEADER );
	pImage->imageFormat.biWidth = wWidth;
	pImage->imageFormat.biHeight = wHeight;
	pImage->imageFormat.biPlanes = 1;
	pImage->imageFormat.biBitCount = wBitsPerPixel;
	pImage->imageFormat.biCompression = BI_RGB;
	pImage->imageFormat.biSizeImage = lBytesPerRow * wHeight;
	pImage->imageFormat.biXPelsPerMeter = 0;
	pImage->imageFormat.biYPelsPerMeter = 0;
	pImage->imageFormat.biClrUsed = 0;
	pImage->imageFormat.biClrImportant = 0;
	
	hDC = GetDC ( NULL );
	pImage->imageBitmap = CreateDIBSection ( hDC, &pImage->imageFormat, DIB_RGB_COLORS, &pImage->imageData, NULL, 0 );
	if ( pImage->imageBitmap == NULL )
	{
		free ( pImage );
		return ( NULL );
	}

	if ( wNumColors > 0 )
		GetDIBColorTable ( hDC, 0, wNumColors, pImage->imageColorTable );
	
	ReleaseDC ( NULL, hDC );

	return ( pImage );
}

/***  GDeleteImage  *********************************************************/

void GDeleteImage ( GImagePtr pImage )
{
	DeleteObject ( pImage->imageBitmap );
	free ( pImage );
}

/***  GSetImageColorTableEntry  *********************************************/

void GSetImageColorTableEntry ( GImagePtr pImage, unsigned char ucIndex,
unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
	short		wNumColors;
	LPRGBQUAD	pColors;

	if ( pImage->imageFormat.biBitCount > 8 )
		wNumColors = 0;
	else
		wNumColors = 1L << pImage->imageFormat.biBitCount;
	
	pColors = pImage->imageColorTable;

    if ( ucIndex >= 0 && ucIndex < wNumColors )
    {
		pColors[ ucIndex ].rgbRed = ucRed;
		pColors[ ucIndex ].rgbGreen = ucGreen;
		pColors[ ucIndex ].rgbBlue = ucBlue;
		pColors[ ucIndex ].rgbReserved = 0;
    }
}

/***  GGetImageColorTableEntry  *************************************************/

void GGetImageColorTableEntry ( GImagePtr pImage, unsigned char ucIndex,
unsigned char *pRed, unsigned char *pGreen, unsigned char *pBlue )
{
	short		wNumColors;
	LPRGBQUAD	pColors;

	if ( pImage->imageFormat.biBitCount > 8 )
		wNumColors = 0;
	else
		wNumColors = 1L << pImage->imageFormat.biBitCount;

	pColors = (LPRGBQUAD) ( (char *) pImage + sizeof ( BITMAPINFOHEADER ) );

    if ( ucIndex >= 0 && ucIndex < wNumColors )
    {
		*pRed = pColors[ ucIndex ].rgbRed;
		*pGreen = pColors[ ucIndex ].rgbGreen;
		*pBlue = pColors[ ucIndex ].rgbBlue;
    }
}

/***  GGetImageWidth  *********************************************************/

short GGetImageWidth ( GImagePtr pImage )
{
	return ( pImage->imageFormat.biWidth );
}

/***  GGetImageHeight  ********************************************************/

short GGetImageHeight ( GImagePtr pImage )
{
	return ( pImage->imageFormat.biHeight );
}

/***  GGetImageDepth  *********************************************************/

short GGetImageDepth ( GImagePtr pImage )
{
	return ( pImage->imageFormat.biBitCount );
}

/***  GGetImageDataRow  *******************************************************/

unsigned char *GGetImageDataRow ( GImagePtr pImage, short wRow )
{
	char	*pRow;

	pRow = pImage->imageData
	     + G_BITMAP_ROW_BYTES ( pImage->imageFormat.biBitCount, pImage->imageFormat.biWidth )
		 * ( pImage->imageFormat.biHeight - wRow - 1 );

	return ( pRow );
}

/*** GGetImageDataValue ****************************************************/

unsigned long GGetImageDataValue ( GImagePtr pImage, unsigned char *lpbData, short wCol )
{
	unsigned long lValue = 0;
    
    if ( pImage->imageFormat.biBitCount == 32 )
    {
    	lValue = ( (unsigned long *) lpbData )[ wCol ];
    }
    else if ( pImage->imageFormat.biBitCount == 8 )
    {
    	lValue = lpbData[ wCol ];
    }
    else
    {
		unsigned long lBits = wCol * pImage->imageFormat.biBitCount;
		unsigned long *lplAddress = (unsigned long *) ( lpbData + lBits / 8 );
		unsigned long lOffset = lBits % 8;
		unsigned long lMask = ( ( 1 << pImage->imageFormat.biBitCount ) - 1 ) << lOffset;

		lValue = ( *lplAddress & lMask ) >> lOffset;
    }

	return ( lValue );
}

/*** GGetImageDataValueColor **********************************************/

void GGetImageDataValueColor ( GImagePtr pImage, unsigned long lValue,
unsigned char *lpbRed, unsigned char *lpbGreen, unsigned char *lpbBlue )
{
	if ( pImage->imageFormat.biBitCount == 32 )
	{
		*lpbRed = ( lValue >> 16 ) & 0x000000FF;
		*lpbGreen = ( lValue >> 8 ) & 0x000000FF;
		*lpbBlue = ( lValue ) & 0x000000FF;
	}
	else if ( pImage->imageFormat.biBitCount <= 8 )
	{
		GGetImageColorTableEntry ( pImage, lValue, lpbRed, lpbGreen, lpbBlue );
	}
}

/***  GDrawImage  *********************************************************/

void GDrawImage ( GImagePtr pImage, GRectPtr pSrcRect, GRectPtr pDstRect )
{
	int		nResult;
	int		nSrcLeft, nSrcTop, nSrcWidth, nSrcHeight;
	int		nDstLeft, nDstTop, nDstWidth, nDstHeight;
	short	wNumColors;
    char	*pData;
	HDC		hDC = GGetGraphicPort();

	/*** If we have been given a source rectangle, use it; otherwise, use the
	     DIB's bounding rectangle as the source rectangle.  Note that we need
	     to pass the bitmap's height minus the bottom coordinate of the source
	     rectangle, because Windows considers the origin of the bitmap
		 to be the bottom left-hand corner, not the top left-hand corner. ***/

	if ( pSrcRect == NULL )
	{
		nSrcTop = 0;
		nSrcLeft = 0;
		nSrcWidth = pImage->imageFormat.biWidth;
		nSrcHeight = pImage->imageFormat.biHeight;
	}
	else
	{
		nSrcTop = pImage->imageFormat.biHeight - GBottom ( pSrcRect );
		nSrcLeft = GLeft ( pSrcRect );
		nSrcWidth = GWidth ( pSrcRect );
		nSrcHeight = GHeight ( pSrcRect );
	}
		
	/*** If we have been given a destination rectangle, use it; otherwise, use
	     a copy of the source rectangle as the destination. ***/

	if ( pDstRect == NULL )
	{
		nDstLeft = nSrcLeft;
		nDstTop = pSrcRect == NULL ? 0 : GTop ( pSrcRect );
		nDstWidth = nSrcWidth;
		nDstHeight = nSrcHeight;
	}
	else
	{
		nDstLeft = GLeft ( pDstRect );
		nDstTop = GTop ( pDstRect );
		nDstWidth = GWidth ( pDstRect );
		nDstHeight = GHeight ( pDstRect );
	}
	
	if ( pImage->imageFormat.biBitCount > 8 )
		wNumColors = 0;
	else
		wNumColors = 1L << pImage->imageFormat.biBitCount;
	
	pData = (char *) pImage
	      + sizeof ( BITMAPINFOHEADER )
	      + sizeof ( RGBQUAD ) * wNumColors;

    nResult = StretchDIBits ( hDC,
    	nDstLeft,
        nDstTop,
        nDstWidth,
        nDstHeight,
        nSrcLeft,
        nSrcTop,
        nSrcWidth,
        nSrcHeight,
        pImage->imageData,
        &pImage->imageFormat,
        DIB_RGB_COLORS,
        SRCCOPY );
}

/*** GReadImageFile **********************************************************/

GImagePtr GReadImageFile ( FILE *pFile, short wDepth )
{
	return ( NULL );
}

/*** GWriteImageFile **********************************************************/

int GWriteImageFile ( GImagePtr pImage, FILE *pFile )
{
	return ( FALSE );
}

/*** GStartDrawingImage ******************************************************/

void GStartDrawingImage ( HWND hWnd, GImagePtr pImage )
{
	HDC	hDCWnd, hDCBmp;

	if ( ( hDCWnd = GetDC ( hWnd ) ) != NULL )
	{
		if ( ( hDCBmp = CreateCompatibleDC ( hDCWnd ) ) != NULL )
		{
			SelectObject ( hDCBmp, pImage->imageBitmap );
			GSetGraphicPort ( hDCBmp );
		}

		ReleaseDC ( hWnd, hDCWnd );
	}
}

/***  GEndDrawingImage  ****************************************************/

void GEndDrawingImage ( HWND hWnd, GImagePtr pImage )
{
	HDC	hDC;

	if ( ( hDC = GGetGraphicPort() ) != NULL )
	{
		GSetGraphicPort ( NULL );
		DeleteDC ( hDC );
	}
}

#ifdef GJPEG

/*** jpeg_std_error_jump ****************************************************/

static jmp_buf	*error_jump_buffer;

void error_jump ( j_common_ptr cinfo )
{
	/*** Clean up the JPEG compression or decompression object,
	     then return control to the setjmp point. ***/
     
	jpeg_destroy ( cinfo );
	longjmp ( *error_jump_buffer, 1 );
}

struct jpeg_error_mgr *jpeg_std_error_jump ( struct jpeg_error_mgr *err, jmp_buf *err_jmp_buf )
{
	jpeg_std_error ( err );
	err->error_exit = error_jump;
	error_jump_buffer = err_jmp_buf; 
	return ( err );
}

/*** GReadJPEGImageFile *****************************************************/

GImagePtr GReadJPEGImageFile ( FILE *file )
{
	GImagePtr		image = NULL;
	struct			jpeg_decompress_struct cinfo;
	struct			jpeg_error_mgr jerr;
	JSAMPROW		row_pointer[1];
	int				col, row_stride;
	jmp_buf			jerr_jmp_buf;
	unsigned long	*data = NULL;
	unsigned long	red, green, blue, index;

	/*** Set up default JPEG library error handling.  Then override the
	     standard error_exit routine (which would quit the program!) with
	     our own replacement, which jumps back to context established in
	     the call to setjmp() below.  If this happens, the JPEG library
	     has signalled a fatal error, so close the file and return FALSE. ***/
	
	cinfo.err = jpeg_std_error_jump ( &jerr, &jerr_jmp_buf );
	if ( setjmp ( jerr_jmp_buf ) )
	{
    	if ( image != NULL )
    		GDeleteImage ( image );
    	return ( NULL );
	}

	/*** Now allocate and initialize the JPEG decompression object, and specify
	     the file as the data source. */
	
	jpeg_create_decompress ( &cinfo );
	jpeg_stdio_src ( &cinfo, file );

	/*** Read the file parameters from the header.  On failure, return NULL. ***/

	if ( jpeg_read_header ( &cinfo, TRUE ) != JPEG_HEADER_OK )
	{
		return ( NULL );
	}

	/*** Create a new 32-bit image of the required width and height.  ***/
	      
	image = GCreateImage ( cinfo.image_width, cinfo.image_height, cinfo.num_components == 1 ? 8 : 32 );
	if ( image == NULL )
	{
		return ( NULL );
	}
	
	/*** If we have a grayscale (i.e. one-color-component) JPEG file, create
	     a simple grayscale color table for the image. ***/

	if ( cinfo.num_components == 1 )
		for ( index = 0; index < 256; index++ )
			GSetImageColorTableEntry ( image, index, index, index, index );

	/*** Start the JPEG decompressor. ***/
		
	jpeg_start_decompress ( &cinfo );

	/*** If we have a one-color-component (i.e. grayscale) JPEG file, read the
	     JPEG data directly into the image, one row at a time.  For RGB color
	     JPEG files, it's a little more complicated. ***/

	if ( cinfo.num_components == 1 )
	{
		while ( cinfo.output_scanline < cinfo.output_height )
		{
			row_pointer[0] = GGetImageDataRow ( image, cinfo.output_scanline );
			jpeg_read_scanlines ( &cinfo, row_pointer, 1 );
		}
	}
	else
	{
		/*** Determine the size, in bytes, of a row in the output buffer, then make
			 a one-row-high sample array. ***/

		row_stride = cinfo.output_width * cinfo.output_components;
		row_pointer[0] = malloc ( row_stride );

		/*** Read scanlines from the input file until there are no more scanlines
			 to be read.  Here we use the library's state variable cinfo.output_scanline
			 as the loop counter, so that we don't have to keep track ourselves. ***/

		while ( cinfo.output_scanline < cinfo.output_height )
		{
			/*** Note that jpeg_read_scanlines expects an array of pointers to
				 scanlines.  Here the array is only one element long, but you could
				 ask for more than one scanline at a time if that's more convenient. ***/

			data = (unsigned long *) GGetImageDataRow ( image, cinfo.output_scanline );
			jpeg_read_scanlines ( &cinfo, row_pointer, 1 );

			for ( col = 0; col < cinfo.image_width; col++ )
			{
				red   = row_pointer[0][ 3 * col     ];
				green = row_pointer[0][ 3 * col + 1 ];
				blue  = row_pointer[0][ 3 * col + 2 ];

				data[col] = ( red << 16 ) | ( green << 8 ) | blue;
			}
		}

		free ( row_pointer[0] );
	}

	/*** Finish decompression, and release the JPEG decompression object.
	     This is an important step since it will release a good deal of memory. ***/

	jpeg_finish_decompress ( &cinfo );
	jpeg_destroy_decompress ( &cinfo );

	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	return ( image );	
}

/*** GWriteJPEGImageFile *****************************************************/

int GWriteJPEGImageFile ( GImagePtr image, short quality, int grayscale, FILE *file )
{
	struct jpeg_compress_struct	cinfo;
	struct jpeg_error_mgr		jerr;
	JSAMPROW					row_pointer[1];
	JDIMENSION					col;
	int							row_stride;
	short						image_width = GGetImageWidth ( image );
	short						image_height = GGetImageHeight ( image );
	jmp_buf						jerr_jmp_buf;
	unsigned long				value;
	unsigned char				red, green, blue;
	
	/*** Set up default JPEG library error handling.  Then override the
	     standard error_exit routine (which would quit the program!) with
	     our own replacement, which jumps back to context established in
	     the call to setjmp() below.  If this happens, the JPEG library
	     has signalled a fatal error, so close the file and return FALSE. ***/
	
	cinfo.err = jpeg_std_error_jump ( &jerr, &jerr_jmp_buf );
	if ( setjmp ( jerr_jmp_buf ) )
	{
    	return ( FALSE );
	}

	/*** Now allocate and initialize the JPEG compression object, and specify
	     the file as the data source. */

	jpeg_create_compress ( &cinfo );
	jpeg_stdio_dest ( &cinfo, file );

	/*** Set parameters for compression.  First we supply a description of the
	     input image.  We must set at least cinfo.in_color_space, since the defaults
	     depend on the source color space.  Then use the library's routine to set
	     default compression parameters.  Finally, we can set any non-default
	     parameters we wish to.  Here we just use the quality (quantization table)
	     scaling ***/
	     
	cinfo.image_width = image_width;
	cinfo.image_height = image_height;
	cinfo.input_components = grayscale ? 1: 3;
	cinfo.in_color_space = grayscale ? JCS_GRAYSCALE : JCS_RGB;
  
	jpeg_set_defaults ( &cinfo);
	jpeg_set_quality ( &cinfo, quality, TRUE );

	/*** Start the JPEG compressor.  Determine the size, in bytes, of a row
	     in the output buffer, then make a one-row-high sample array. ***/

	jpeg_start_compress ( &cinfo, TRUE );

	row_stride = image_width * cinfo.input_components;
	row_pointer[0] = malloc ( row_stride );

	/*** Write scanlines to the output file until there are no more scanlines
	     to be read.  Here we use the library's state variable cinfo.next_scanline
	     as the loop counter, so that we don't have to keep track ourselves. ***/

	while ( cinfo.next_scanline < cinfo.image_height)
	{
		unsigned char *image_row = GGetImageDataRow ( image, cinfo.next_scanline );
		
		for ( col = 0; col < image_width; col++ )
		{
			/*** Copy the R-G-B color components of the image pixel into the
			     corresponding locations in the output row, then write the
			     entire row to the file. ***/
			     
			value = GGetImageDataValue ( image, image_row, col );
			GGetImageDataValueColor ( image, value, &red, &green, &blue );
			
			if ( grayscale )
			{
				row_pointer[0][col] = value;
			}
			else
			{
				row_pointer[0][ 3 * col ]     = red;
				row_pointer[0][ 3 * col + 1 ] = green;
				row_pointer[0][ 3 * col + 2 ] = blue;
			}
		}
		
		jpeg_write_scanlines ( &cinfo, row_pointer, 1 );
	}
	
	free ( row_pointer[0] );
  
	/*** Finish compression, and release the JPEG compression object ***/

	jpeg_finish_compress ( &cinfo );
	jpeg_destroy_compress ( &cinfo );

	/*** And we're done! ***/
	
	return ( TRUE );
}

#endif /* GJPEG */

#ifdef GTIFF

/*** TIFFErrorSupressor *****************************************************************/

void TIFFErrorSupressor ( const char *module, const char *fmt, va_list ap )
{

}

/*** TIFFClientRead *********************************************************************/

tsize_t TIFFClientRead ( thandle_t handle, tdata_t data, tsize_t size )
{
	FILE *file = (FILE *) handle;
	tsize_t result;
	
	/*** The TIFF library sometimes tries to read a file that is opened for writing;
	     this may causes an error condition which prevents further writing from happening.
	     This seems to cause problems on the Mac version of GUILib but appears to be
	     OK in Windows. ***/
	     
	result = fread ( data, size, 1, file );
		
	return ( result * size );
}

/*** TIFFClientWrite *********************************************************************/

tsize_t TIFFClientWrite ( thandle_t handle, tdata_t data, tsize_t size )
{
	FILE *file = (FILE *) handle;
	tsize_t result;
	
	/*** The TIFF library may try to write a file that is opened for reading;
	     this causes an error condition which prevents further reading from happening.
	     This seems to cause problems on the Mac version of GUILib but appears to be
	     OK in Windows. ***/

	result = fwrite ( data, size, 1, file );
		
	return ( result * size );
}

/*** TIFFClientSeek *********************************************************************/

toff_t TIFFClientSeek ( thandle_t file, toff_t offset, int whence )
{
	if ( fseek ( (FILE *) file, offset, whence ) == 0 )
		return ( ftell ( (FILE *) file ) );
	else
		return ( EOF );
}

/*** TIFFClientClose *********************************************************************/

int TIFFClientClose ( thandle_t file )
{
	return ( 0 ); /* we don't actually want the TIFF library to actually close the file! */
	return ( fclose ( (FILE *) file ) );
}

/*** TIFFClientSize *********************************************************************/

toff_t TIFFClientSize ( thandle_t file )
{
	long size = -1, offset;
	
	offset = ftell ( (FILE *) file );
	if ( offset > -1 )
	{
		if ( fseek ( (FILE *) file, 0, SEEK_END ) == 0 )
			size = ftell ( (FILE *) file );
			
		fseek ( (FILE *) file, offset, SEEK_SET );	
	}
			
	return ( size );
}

/*** TIFFClientMap *********************************************************************/

int TIFFClientMap ( thandle_t file, tdata_t* pbase, toff_t* psize )
{
	return ( 0 );
}

/*** TIFFClientUnmap *********************************************************************/

void TIFFClientUnmap ( thandle_t file, tdata_t base, toff_t size )
{

}

/*** GReadTIFFImageFile *******************************************************************/

GImagePtr GReadTIFFImageFile ( FILE *file )
{
	unsigned long	width = 0, height = 0, *data0, *data1, value, row, col;
	unsigned long	tilewidth = 0, tileheight = 0;
	unsigned short	bitspersample = 0, samplesperpixel = 0, planarconfig = 0, photometric = 0;
	unsigned short	numcolors, color, *redmap = NULL, *greenmap = NULL, *bluemap = NULL;
	unsigned char	red, green, blue;	
	TIFF			*tiff = NULL;
	GImagePtr		image = NULL;
	
	/*** Create a TIFF record in memory.  (Note that we don't actually open the file
	     here!)  Note that this automatically reads the first TIFF Image File Directory
	     (i.e. all data associated with the first image in the TIFF file) in the file.
	     Return an error code on failure. ***/
	     
	tiff = TIFFClientOpen ( "", "r", file,
	       TIFFClientRead, TIFFClientWrite, TIFFClientSeek, TIFFClientClose, TIFFClientSize,
	       TIFFClientMap, TIFFClientUnmap );

	if ( tiff == NULL )
		return ( FALSE );
	
	/*** Obtain the image's dimensions and bit-depth.  The create a new image with the
	     corresponding dimensions and bit-depth.  On failure, release memory for the
	     TIFF file and return a NULL pointer. ***/
	
	TIFFGetField ( tiff, TIFFTAG_IMAGEWIDTH, &width );
	TIFFGetField ( tiff, TIFFTAG_IMAGELENGTH, &height );
	TIFFGetField ( tiff, TIFFTAG_TILEWIDTH, &tilewidth );
	TIFFGetField ( tiff, TIFFTAG_TILELENGTH, &tileheight );
	TIFFGetField ( tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel );
	TIFFGetField ( tiff, TIFFTAG_BITSPERSAMPLE, &bitspersample );
	TIFFGetField ( tiff, TIFFTAG_PLANARCONFIG, &planarconfig );
	TIFFGetField ( tiff, TIFFTAG_PHOTOMETRIC, &photometric );
	
	/*** If we have a TIFF file which represents a monochrome/grayscale/palette image,
	     we create a indexed-color bitmap of the corresponding bit depth.  NOTE: THIS CODE
	     WILL FAIL TO READ TILED TIFF FILES. ***/
	     
	if ( bitspersample <= 8 && samplesperpixel == 1 && tilewidth == 0 && tileheight == 0 )
	{
		image = GCreateImage ( width, height, bitspersample );
		if ( image == NULL )
		{
			TIFFClose ( tiff );
			return ( NULL );
		}
		
		/*** If the TIFF tags indicate that the image contains a color palette, see if
		     we can successfully extract the colormap from the TIFF file, then copy it
		     into the bitmap image.  If we can't get the colormap from the TIFF file,
		     free the image, close the TIFF record, and return a NULL pointer to
		     indicate failure. ***/
		     
		numcolors = 1 << bitspersample;
		
		if ( photometric == PHOTOMETRIC_PALETTE && TIFFGetField ( tiff, TIFFTAG_COLORMAP, &redmap, &greenmap, &bluemap ) )
		{
			for ( color = 0; color < numcolors; color++ )
			{
				red = redmap[color] / 257;
				green = greenmap[color] / 257;
				blue = bluemap[color] / 257;
				
				GSetImageColorTableEntry ( image, color, red, green, blue );
			}
		}

		/*** If the TIFF file doesn't contain a colormap, we assume it's a
		     bi-level (1 bit) or grayscale (4- or 8-bit) image.  Fill the
		     color table of the bitmap image with a grayscale color table
		     of the required bit-depth (or a reversed white-to-black color
		     table, if the photometric interpretation tag indicates that
		     white is zero.) ***/
			     
		else if ( photometric == PHOTOMETRIC_MINISBLACK )
		{
			for ( color = 0; color < numcolors; color++ )
			{
				red = green = blue = 255 * color / ( numcolors - 1 );
				
				GSetImageColorTableEntry ( image, color, red, green, blue );
			}
		}
		else if ( photometric == PHOTOMETRIC_MINISWHITE )
		{
			for ( color = 0; color < numcolors; color++ )
			{
				red = green = blue = 255 - 255 * color / ( numcolors - 1 );
				
				GSetImageColorTableEntry ( image, color, red, green, blue );
			}
		}
		
		/*** Other cases are invalid TIFF files.  Delete the image record,
		     free TIFF memory, and return a NULL pointer to indicate failure. ***/
		
		else
		{
			GDeleteImage ( image );
			TIFFClose ( tiff );
			
			return ( NULL );
		}
		
		/*** For each row in the TIFF file, read image data into the bitmap directly.
		     We can get away with this because the data returned by the TIFF library
		     is in exactly the same format the Mac expects for an indexed-color bitmap. ***/
		
		for ( row = 0; row < height; row++ )
			TIFFReadScanline ( tiff, GGetImageDataRow ( image, row ), row, 0 );
	}
	else
	{
		/*** For all other TIFF image types, we need to create a 32-bit direct-color
		     RGB bitmap.  On failure, close the TIFF image, and return a NULL pointer. ***/
		     
		image = GCreateImage ( width, height, 32 );
		if ( image == NULL )
		{
			TIFFClose ( tiff );
			return ( NULL );
		}
		
		/*** Now we read the TIFF image data as RGB data directly into the bitmap's
		     data buffer.  Note that because a Macintosh PixMap stores all of the
		     rows of image data contiguously, we can pass a pointer to the start of
		     the first row as the raster into which the TIFF library will place the
		     RGB TIFF image data.  On failure, delete the image, close the TIFF
		     file, and return a NULL pointer, as usual. ***/
		     
		data0 = GGetImageDataRow ( image, 0 );
		if ( TIFFReadRGBAImage ( tiff, width, height, data0, TRUE ) == 0 )
		{
			GDeleteImage ( image );
			TIFFClose ( tiff );
			return ( NULL );
		}
		
		/*** The last problem we need to solve is the fact that the RGB data returned
		     by the TIFF library is in the wrong order and the wrong format.  In the
		     data returned by the TIFF library, the first row of the image is at the
		     bottom, and the bytes within each pixel represent R-G-B-A; whereas the Mac
		     expects the first row of the image to be at the top, and the bytes within
		     each pixel to represent A-R-G-B.  So, we need to swap the data from bottom
		     to top, and reorder the bytes within each pixel. ***/
		      
		for ( row = 0; row < height / 2; row++ )
		{
			data0 = GGetImageDataRow ( image, row );
			data1 = GGetImageDataRow ( image, height - row - 1 );
			
			for ( col = 0; col < width; col++ )
			{
				value = data0[col];
				
				red = TIFFGetR ( value );
				green = TIFFGetG ( value );
				blue = TIFFGetB ( value );
				
				value = data1[col];
				data1[col] = ( red << 16L ) | ( green << 8L ) | blue;
				
				red = TIFFGetR ( value );
				green = TIFFGetG ( value );
				blue = TIFFGetB ( value );

				data0[col] = ( red << 16L ) | ( green << 8L ) | blue;
			}
		}
	}
	
	/*** And we're done!  Close the TIFF file and return a pointer
	     to the bitmap image. ***/
	     
	TIFFClose ( tiff );
	return ( image );
}

/*** GWriteTIFFImageFile *******************************************************************/

int GWriteTIFFImageFile ( GImagePtr image, char *filename, FILE *file )
{
	unsigned long	width, height, depth, value, row, col;
	unsigned char	*data, *scanline, red, green, blue;
	unsigned short	*colormap = NULL, numcolors, color;
	TIFF			*tiff = NULL;

	/*** Create a TIFF record in memory.  (Note that we don't actually open the file
	     here!)  Return an error code on failure. ***/
	     
	tiff = TIFFClientOpen ( filename, "w", file,
	       TIFFClientRead, TIFFClientWrite, TIFFClientSeek, TIFFClientClose, TIFFClientSize,
	       TIFFClientMap, TIFFClientUnmap );

	if ( tiff == NULL )
		return ( FALSE );
	
	/*** Obtain the image dimensions and depth, and set the corresponding TIFF tags. ***/
	
	width = GGetImageWidth ( image );
	height = GGetImageHeight ( image );
	depth = GGetImageDepth ( image );
	
	TIFFSetField ( tiff, TIFFTAG_IMAGEWIDTH, width );
	TIFFSetField ( tiff, TIFFTAG_IMAGELENGTH, height );
	TIFFSetField ( tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG );	/* component values for each pixel are stored contiguously */
	TIFFSetField ( tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE ); 		/* no compression */

	if ( depth <= 8 )
	{
		/*** For an indexed-color image, set the appropriate TIFF tags to indicate
		     that we have a 1 color sample of up to the bit depth of the image. ***/

		TIFFSetField ( tiff, TIFFTAG_BITSPERSAMPLE, depth );
		TIFFSetField ( tiff, TIFFTAG_SAMPLESPERPIXEL, 1 );
		
		/*** If we have a 1-bit bitmap, we don't write any color table; we just indicate
		     that value zero is white and value 1 is black.  If we have an image with at
		     least 4 or 8 bits per pixel, extract the color table from the bitmap and
		     write it to the TIFF file. ***/
		
		if ( depth == 1 )
		{
			TIFFSetField ( tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE );
		}
		else
		{
			TIFFSetField ( tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE );
			
			/*** Compute the number of colors in the image's color table, and allocate
			     an array of short integers big enough to hold R-G-B color components
			     for each entry in the color table.  On failure, free the TIFF record
			     and return a NULL pointer. ***/
			     
			numcolors = 1 << depth;
			colormap = (unsigned short *) malloc ( sizeof ( unsigned short ) * 3 * numcolors );
			if ( colormap == NULL )
			{
				TIFFClose ( tiff );
				return ( FALSE );
			}

			/*** For each entry in the image's color table, copy the red components into
			     the TIFF color map first, followed by the blue components, then the green
			     ones.  Scale them to the range 0-65535. ***/
			
			for ( color = 0; color < numcolors; color++ )
			{
				GGetImageColorTableEntry ( image, color, &red, &green, &blue );
				
				colormap[ color                 ] = red * 257;
				colormap[ color + numcolors     ] = green * 257;
				colormap[ color + numcolors * 2 ] = blue * 257; 
			}

			TIFFSetField ( tiff, TIFFTAG_COLORMAP, &colormap[0], &colormap[ numcolors ], &colormap[ 2 * numcolors ] );
		}
		
		/*** Now, for each row of data in the image, write the pixel values in the row
		     (which represent indices into the color table) to the TIFF file.  When done,
		     free memory for the TIFF colormap. ***/
		
		for ( row = 0; row < height; row++ )
		{
			data = GGetImageDataRow ( image, row );
			TIFFWriteScanline ( tiff, data, row, 0 );
		}
		
		if ( colormap != NULL )
			free ( colormap );
	}
	else
	{
		/*** For direct-color images, we set the appropriate TIFF tags to indicate
		     that we have 3 eight-bit color samples per pixel, and that they should
		     be interpreted as RGB color values. ***/

		TIFFSetField ( tiff, TIFFTAG_BITSPERSAMPLE, 8 );
		TIFFSetField ( tiff, TIFFTAG_SAMPLESPERPIXEL, 3 );
		TIFFSetField ( tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB );
		
		/*** Allocate a buffer to hold a single scanline of TIFF RGB image file data;
		     close the file and return an error code on failure. ***/
		     
		scanline = malloc ( sizeof ( unsigned char ) * width * 3 );
		if ( scanline == NULL )
		{
			TIFFClose ( tiff );
			return ( FALSE );
		}
		
		/*** For each row in the image, store the RGB values in each pixel into the TIFF
		     scanline, then write the scanline to the TIFF file. ***/
		
		for ( row = 0; row < height; row++ )
		{
			for ( col = 0; col < width; col++ )
			{
				value = GGetImageDataValue ( image, GGetImageDataRow ( image, row ), col );
				GGetImageDataValueColor ( image, value, &red, &green, &blue );
				
				scanline[ col * 3     ] = red;
				scanline[ col * 3 + 1 ] = green;
				scanline[ col * 3 + 2 ] = blue;
			}

			TIFFWriteScanline ( tiff, scanline, row, 0 );
		}
		
		/*** Free memory for the TIFF scanline buffer. ***/
		
		free ( scanline );
	}
	
	/*** Close the TIFF file and return a successful result code. ***/
	
	TIFFClose ( tiff );  
	return ( TRUE );
}

#endif /* GTIFF */

#ifdef GGIF

/*** GGIFInputFunction ***********************************************************/

int GGIFInputFunction ( GifFileType *gif, GifByteType *buffer, int numbytes )
{
	FILE *file = gif->UserData;
	
	if ( fread ( buffer, numbytes, 1, file ) != 1 )
		return ( 0 );
	else
		return ( numbytes );
}

/*** GGIFOutputFunction ***********************************************************/

int GGIFOutputFunction ( GifFileType *gif, GifByteType *buffer, int numbytes )
{
	FILE *file = gif->UserData;

	if ( fwrite ( buffer, numbytes, 1, file ) != 1 )
		return ( 0 );
	else
		return ( numbytes );
}

/*** GReadGIFImageFile ***********************************************************/

GImagePtr GReadGIFImageFile ( FILE *file )
{
	GImagePtr		image = NULL;
	GifFileType		*gif;
	ColorMapObject 	*colormap;
	long			i, k, pass;
	unsigned char	*data;
    GifRecordType	type;
	GifByteType		*extension;
	int				function;
	
	/*** Read the header from the GIF file; return an error if we can't. ***/
	
	gif = DGifOpen ( file, GGIFInputFunction );
	if ( gif == NULL )
		return ( NULL );
	
	/*** Now read records from the GIF file, until we come to a GIF
	     image record or the GIF file termination record. ***/
	     
	do
	{
		/*** Read the next GIF file record type. ***/

		if ( DGifGetRecordType ( gif, &type ) != GIF_OK )
		{
			DGifCloseFile ( gif );
			return ( NULL );
		}

		/*** If we have a GIF extension record, read (but discard) the
		     extension data. ***/
		     
		if ( type == EXTENSION_RECORD_TYPE )
		{
			if ( DGifGetExtension ( gif, &function, &extension ) != GIF_OK )
			{
				DGifCloseFile ( gif );
			    return ( NULL );
			}
			
			while ( extension != NULL)
			{
			    if ( DGifGetExtensionNext ( gif, &extension ) != GIF_OK )
				{
					DGifCloseFile ( gif );
				    return ( NULL );
	            }
			}
		}
		
		/*** If the next GIF record is an image descriptor, read an image
		     from the GIF file. ***/
		     
		if ( type == IMAGE_DESC_RECORD_TYPE )
		{
			/*** Read the GIF Image descriptor; return an error code on failure. ***/
			
			if ( DGifGetImageDesc ( gif ) != GIF_OK )
			{
				DGifCloseFile ( gif );
				return ( NULL );
			}
			
			/*** Now allocate a native bitmap whose width and height are the same as
			     that of the first image in the GIF file; make the bitmap's bit-depth
			     8 bits per pixel (regardless of the color resolution indicated in the
			     GIF file header) because the GIF library always decodes data into a
			     buffer of 8 bits-per-pixel depth. ***/
			     
			image = GCreateImage ( gif->Image.Width, gif->Image.Height, 8 );
			if ( image == NULL )
			{
				DGifCloseFile ( gif );
				return ( NULL );
			}
			
			/*** If the GIF image has a local color table, use it to create the native
			     image's color table; otherwise, use the GIF file's global color table. ***/
			     
			if ( gif->Image.ColorMap != NULL )
				colormap = gif->Image.ColorMap;
			else
				colormap = gif->SColorMap;
			
			/*** Now copy the color table from the GIF image into the native image. ***/
				
			for ( i = 0; i < colormap->ColorCount; i++ )
				GSetImageColorTableEntry ( image, i, colormap->Colors[i].Red, colormap->Colors[i].Green, colormap->Colors[i].Blue );
		
			/*** After we've copied the color table, read the raster image data from the GIF
			     image into the native image.  Start with the first row, and continue
			     one row at a time until we've read the entire GIF image. ***/
			     
			for ( k = i = 0, pass = 1; i < gif->Image.Height; i++ )
			{
				/*** Find a pointer to the start of the current row in the native image,
				     then read the data from the GIF file directly into that row; return
				     an error code on failure. ***/
				     
				data = GGetImageDataRow ( image, k );
				if ( DGifGetLine ( gif, data, gif->Image.Width ) != GIF_OK )
				{
					DGifCloseFile ( gif );
					GDeleteImage ( image );
				    return ( NULL );
				}
				
				/*** Now move to the next row in the native bitmap.  If the GIF image is
				     interlaced, the following code locates the position of the "next row"
				     according to the interlacing scheme described in the GIF specs. ***/
				     
				if ( gif->Image.Interlace )
				{
					if ( pass == 1 || pass == 2 )
						k += 8;
					
					if ( pass == 3 )
						k += 4;
					
					if ( pass == 4 )
						k += 2;
						
					if ( k >= gif->Image.Height )
					{
						pass++;
						
						if ( pass == 2 )
							k = 4;
							
						if ( pass == 3 )
							k = 2;
							
						if ( pass == 4 )
							k = 1;
					}
				}
				else
				{
					k++;
				}
			}
			
			/*** Since we've read an image, we can stop reading the GIF file. ***/
			
			break;
		}
	}
	while ( type != TERMINATE_RECORD_TYPE );
	
	/*** Finally, release memory for the GIF object, and return the native bitmap
	     handle. ***/
	     
	DGifCloseFile ( gif );
	return ( image );
}

/*** GWriteGIFImageFile ***********************************************************/

int GWriteGIFImageFile ( GImagePtr image, FILE *file )
{
	GifFileType		*gif = NULL;
	
	/*** Write the header for the GIF87a file.  Return an error code
	     on failure. ***/

	gif = GWriteGIFHeader ( image, file, GIF87_STAMP );
	if ( gif == NULL )
		return ( FALSE );

	/*** Write the image to the GIF file.  Close the file and return
	     an error code on failure. ***/

	if ( GWriteGIFImage ( image, gif, FALSE, 0, FALSE, 0, 0 ) == FALSE )
	{
		EGifCloseFile ( gif );
		return ( FALSE );
	}

	/*** Close the GIF file, and return a successful result code. ***/
	
	EGifCloseFile ( gif );
	return ( TRUE );
}

/*** GWriteGIFHeader ******************************************************/

GifFileType *GWriteGIFHeader ( GImagePtr image, FILE *file, char *version )
{
	GifFileType		*gif;
	ColorMapObject 	*colormap = NULL;
	int				width = GGetImageWidth ( image );
	int				height = GGetImageHeight ( image );
	int				depth = GGetImageDepth ( image );
	int				numcolors = 1 << depth, i, row, col;

	/*** We can only write a GIF file if the native image has 8 bits per pixel
	     or less. ***/
	     
	if ( depth > 8 )
		return ( NULL );
	
	/*** Now create a GIF Color Map object to hold the image's color table.
	     If we can't, bail out and return an error code.  If we succeed, copy
	     the image's color table into the color map object. ***/
	     	
	colormap = MakeMapObject ( numcolors, NULL );
	if ( colormap == NULL )
		return ( NULL );
		
	for ( i = 0; i < numcolors; i++ )
		GGetImageColorTableEntry ( image, i, &colormap->Colors[i].Red,
		                                     &colormap->Colors[i].Green,
											 &colormap->Colors[i].Blue );
	
	/*** Write the GIF file header, then set the GIF file version. ***/

	gif = EGifOpen ( file, GGIFOutputFunction );
	if ( gif == NULL )
		return ( NULL );
	
	EGifSetGifVersion ( gif, version );

	/*** Write the GIF screen descriptor.  When finished, delete the GIF color
	     table. ***/
	
	if ( EGifPutScreenDesc ( gif, width, height, depth, 0, colormap ) != GIF_OK )
	{
		EGifCloseFile ( gif );
		return ( NULL );
	}

	FreeMapObject ( colormap );
	return ( gif );
}

/*** GWriteGIFImage ******************************************************************/

int GWriteGIFImage ( GImagePtr image, GifFileType *gif,
					 int transparent, unsigned char transparentColor,
                     int userInput, unsigned short delay, int disposalMethod )
{
	int				width = GGetImageWidth ( image );
	int				height = GGetImageHeight ( image );
	int				depth = GGetImageDepth ( image );
	int				row, col;
	GifPixelType	*line;
	unsigned char	*data;

	/*** We can only write a GIF image if the native image has 8 bits per pixel
	     or less. ***/
	     
	if ( depth > 8 )
		return ( FALSE );

	/*** If we are writing a GIF89a file, write a graphic control extension. ***/

	if ( memcmp ( gif->Version, GIF89_STAMP, GIF_STAMP_LEN ) == 0 )
	{
		if ( EGifPutGraphicControlExtension ( gif, disposalMethod, userInput, delay,
			transparent, transparentColor ) != GIF_OK )
			return ( FALSE );
	}

	/*** Now write the GIF screen descriptor. ***/
	
	if ( EGifPutImageDesc ( gif, 0, 0, width, height, 0, NULL ) != GIF_OK )
		return ( FALSE );
	
	/*** Allocate a buffer large enough to hold a single line of GIF raster
	     image data.  If we fail, return an error code. ***/
	     
	line = (GifPixelType *) malloc ( width * sizeof ( GifPixelType ) );
	if ( line == NULL )
		return ( FALSE );
	
	/*** Now write the entire image to the GIF file, line by line: ***/
	
	for ( row = 0; row < height; row++ )
	{
		/*** First copy one line of image data values from the native
		     bitmap into the line of GIF raster image data... ***/
		
		data = GGetImageDataRow ( image, row );
		for ( col = 0; col < width; col++ )
			line[col] = GGetImageDataValue ( image, data, col );
		
		/*** ... then write that line to the GIF file, returning an
		     error code if something goes wrong. ***/
		     
		if ( EGifPutLine ( gif, line, width ) != GIF_OK )
		{
			free ( line );
			return ( FALSE );
		}
	}
	
	/*** Free the line buffer, and return a successful result code. ***/
	
	free ( line );
	return ( TRUE );	
}

#endif /* GGIF */

