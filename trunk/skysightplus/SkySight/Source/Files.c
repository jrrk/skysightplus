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
#include "jpeglib.h"
#include <setjmp.h>

/*** OpenImageFile ***/

void OpenImageFile ( GPathPtr path )
{
	char		filename[256];
	ImagePtr	image;
	GWindowPtr	window;
	
	GGetPathName ( path, filename );

	switch ( FindFileType ( path ) )
	{
		case FILE_TYPE_FITS:
			image = ReadFITSImageFile ( path );
			break;
			
		case FILE_TYPE_JPEG:
			image = ReadJPEGImageFile ( path );
			break;
			
		case FILE_TYPE_TIFF:
			image = ReadTIFFImageFile ( path );
			break;

		case FILE_TYPE_GIF:
			image = ReadGIFImageFile ( path );
			break;
			
		case FILE_TYPE_PICT:
			image = ReadPICTImageFile ( path );
			break;
			
/*
		case FILE_TYPE_BMP:
			image = ReadBMPImageFile ( path );
			break;
*/
			
		default:
			image = NULL;
			break;
	}
	
	if ( image == NULL )
	{
		WarningMessage ( G_OK_ALERT, CANT_READ_FILE_STRING, filename );
		return;
	}
	
	window = NewImageWindow ( image );
	if ( window == NULL )
	{
		WarningMessage ( G_OK_ALERT, CANT_ALLOCATE_MEMORY_STRING );
		DeleteImage ( image );
	}
	else
    {
        ComputeImageDisplayRange ( window );
		DrawImageWindowBitmap ( window );
		SetImageWindowNeedsSave ( window, FALSE );
        CropImageWindow ( window );
        GSetActiveWindow ( window );
	}
}

/*** SaveImageWindow *************************************************************/

void SaveImageWindow ( GWindowPtr window, GPathPtr path )
{
	int				result;
	char			filename[256];
	char			*filetype;
	char			*filetypes[] = { "FITS", "GIFf", "JPEG", "TIFF", "TIFF", "PICT" };
	short			numfiletypes = sizeof ( filetypes ) / sizeof ( filetypes[0] );
	short			format;
	FILE			*file = NULL;
	ImagePtr		image = GetImageWindowImage ( window );
	FITSImagePtr	fits = GetImageFITSImage ( image );
	GWindowPtr		header = GetImageHeaderWindow ( image );
	GImagePtr		bitmap = GetImageWindowBitmap ( window );
	
	/*** Determine the file name, and select the file type which corresponds
	     to the specified file format. ***/
	
	GGetPathName ( path, filename );
	
	format = GetImageFileFormat ( image );
	if ( format > 0 && format <= numfiletypes )
		filetype = filetypes[ format - 1 ];
	else
		return;

	/*** Now open the file for writing in binary mode. ***/
	
	file = GOpenFile ( path, "wb", filetype, SKYSIGHT_FILE_CREATOR );
	if ( file == NULL )
	{
		WarningMessage ( G_OK_ALERT, CANT_OPEN_FILE_STRING, filename );
		return;
	}

	/*** If the image has an associated FITS header window open, update the
	     image's FITS header so that any changes the user made to the text
	     displayed in the header window get correctly propogated back to the
	     underlying FITS image before the file is saved. ***/
	
	if ( header != NULL )
		UpdateHeaderWindowImageHeader ( header );
		
	/*** Save the file in the specified format.  On failure, return an error. ***/
	
	if ( format == FILE_TYPE_FITS )
		result = WriteFITSImage ( file, fits );
	else if ( format == FILE_TYPE_GIF )
		result = GWriteGIFImageFile ( bitmap, file );
	else if ( format == FILE_TYPE_JPEG )
		result = GWriteJPEGImageFile ( bitmap, 75, FALSE, file );
	else if ( format == FILE_TYPE_TIFF )
		result = GWriteTIFFImageFile ( bitmap, filename, file );
	else if ( format == FILE_TYPE_TIFF_16_BIT )
		result = WriteDeepTIFFImageFile ( image, file, filename, SAMPLEFORMAT_UINT );
	else if ( format == FILE_TYPE_PICT )
		result = GWriteImageFile ( bitmap, file );
	else
		result = FALSE;
			
	fclose ( file );
	
	if ( result == FALSE )
	{
		WarningMessage ( G_OK_ALERT, CANT_WRITE_FILE_STRING, filename );
		return;
	}
	
	/*** Store the path specification of the file where we saved the image,
	     then rebuild the image menus so they contain the correct image title
	     (the title needs to change if we changed the file name). ***/
	     
    SetImagePath ( image, path );
    SetImageWindowNeedsSave ( window, FALSE );
	BuildImageMenus();
}

/*** FindFileType ********************************************************************

	Determines a file's format by examining the file header.	
	
	short FindFileType ( GPathPtr path )
	
	(path): pointer to file's path specification record.
	
	The file type returned by this function will be one of the file type codes
	specified in "SkySight.h".  If the file type cannot be recognized, or if some
	other error occurs, the function returns FILE_TYPE_UNKNOWN.

**************************************************************************************/

short FindFileType ( GPathPtr path )
{
	short 		type = FILE_TYPE_UNKNOWN;

	if ( IsJPEGImageFile ( path ) )
		type = FILE_TYPE_JPEG;
	else if ( IsTIFFImageFile ( path ) )
		type = FILE_TYPE_TIFF;
	else if ( IsGIFImageFile ( path ) )
		type = FILE_TYPE_GIF;
	else if ( IsFITSImageFile ( path ) )
		type = FILE_TYPE_FITS;
	else if ( IsPICTImageFile ( path ) )
		type = FILE_TYPE_PICT;
	else
		type = FILE_TYPE_UNKNOWN;	
	
	return ( type );
}

/*** IsFITSImageFile **************************************************************

	Determines whether or not a file is in FITS format by trying to read a FITS
	image header from it.
	
	int IsFITSImageFile ( GPathPtr path )
	
	(path): pointer to a path specification record.
	
	The function returns TRUE if it can successfully read a FITS image header
	from the file, and FALSE otherwise.  Afterwards, the file is closed.
	
***********************************************************************************/

int IsFITSImageFile ( GPathPtr path )
{
	int			result = FALSE;
	FILE 		*file;
	FITSImage	*fits;
	
	/*** Open the file for reading in binary mode; if we can't,
	     return a negative result. ***/
	     
	file = GOpenFile ( path, "rb", NULL, NULL );
	if ( file == NULL )
		return ( result );
		
	/*** Attempt to read a FITS image header from the file.  If
	     we can, then we'll assume the file is in FITS format;
	     discard the header we just read. ***/
	     
	fits = ReadFITSImageHeader ( file );
	if ( fits != NULL )
	{
		FreeFITSImage ( fits );
		result = TRUE;
	}
	
	/*** Close the file and return the result. ***/
	
	fclose ( file );
	return ( result );
}

/*** IsJPEGImageFile *******************************************************************

	Determines whether or not a file is in JPEG format by trying to read a JPEG image
	header from it.
	
	int IsJPEGImageFile ( GPathPtr path )
	
	(path): pointer to a path specification record.
	
	The function returns TRUE if it can successfully read a JPEG header from the
	file, and FALSE otherwise.  Afterwards, the file is closed.
	
****************************************************************************************/

int IsJPEGImageFile ( GPathPtr path )
{
	int								result = FALSE;
	struct	jpeg_decompress_struct	cinfo;
	struct	jpeg_error_mgr			jerr;
	jmp_buf							jerr_jmp_buf;
	FILE 							*file;
	
	/*** Open the file for reading in binary mode; if we can't,
	     return a negative result. ***/
	     
	file = GOpenFile ( path, "rb", NULL, NULL );
	if ( file == NULL )
		return ( result );

	/*** Set up default JPEG library error handling.  The override the
	     standard error_exit routine (which would quit the program!) with
	     our own replacement, which jumps back to context established in
	     the call to setjmp() below.  If this happens, the JPEG library
	     has signalled a fatal error, so close the file and return FALSE. ***/
	
	cinfo.err = jpeg_std_error_jump ( &jerr, &jerr_jmp_buf );
	if ( setjmp ( jerr_jmp_buf ) )
	{
    	fclose ( file );
    	return ( FALSE );
	}
	
	/*** Read a JPEG header from the file.  If we succeed, return TRUE. ***/
	
	jpeg_create_decompress ( &cinfo );
	jpeg_stdio_src ( &cinfo, file );
	if ( jpeg_read_header ( &cinfo, TRUE ) == JPEG_HEADER_OK )
		result = TRUE;
		
	jpeg_destroy_decompress ( &cinfo );

	/*** Close the file and return the result. ***/

	fclose ( file );
	return ( result );
}

/*** IsTIFFImageFile *******************************************************************

	Determines whether or not a file is in TIFF format by trying to read a TIFF file
	header from it.
	
	int IsTIFFImageFile ( GPathPtr path )
	
	(path): pointer to a path specification record.
	
	The function returns TRUE if it can successfully read a TIFF header from the
	file, and FALSE otherwise.  Afterwards, the file is closed.
	
****************************************************************************************/

int IsTIFFImageFile ( GPathPtr path )
{
	int		result = FALSE;
	char	filename[256];
	FILE	*file = NULL;
	TIFF	*tiff = NULL;
	
	file = GOpenFile ( path, "rb", "", "" );
	if ( file != NULL )
	{
		GGetPathName ( path, filename );
		
		tiff = TIFFClientOpen ( filename, "r", file,
		       TIFFClientRead, TIFFClientWrite, TIFFClientSeek, TIFFClientClose, TIFFClientSize,
		       TIFFClientMap, TIFFClientUnmap );
		
		if ( tiff != NULL )
		{
			result = TRUE;
			TIFFClose ( tiff );
		}
		
		fclose ( file );
	}
		
	return ( result );
}

/*** IsGIFImageFile *******************************************************************

	Determines whether or not a file is in GIF format by trying to read a GIF file
	header from it.
	
	int IsGIFImageFile ( GPathPtr path )
	
	(path): pointer to a path specification record.
	
	The function returns TRUE if it can successfully read a GIF header from the
	file, and FALSE otherwise.  Afterwards, the file is closed.
	
****************************************************************************************/

int IsGIFImageFile ( GPathPtr path )
{
	int			result = FALSE;
	char		filename[256];
	FILE		*file = NULL;
	GifFileType	*gif = NULL;
	
	file = GOpenFile ( path, "rb", "", "" );
	if ( file != NULL )
	{
		GGetPathName ( path, filename );
		
		gif = DGifOpen ( file, GGIFInputFunction );
		
		if ( gif != NULL )
		{
			result = TRUE;
			DGifCloseFile ( gif );
		}
		
		fclose ( file );
	}
		
	return ( result );
}

/*** IsPICTImageFile ***/

#ifdef GMAC

int IsPICTImageFile ( GPathPtr path )
{
	int		result = FALSE;
	char	type[8];
	
	GGetFileType ( path, type );
	
	if ( strcmp ( type, "PICT" ) == 0 )
		result = TRUE;
	else
		result = FALSE;
		
	return ( result );
}


#endif

#ifdef GWIN

int IsPICTImageFile ( GPathPtr path )
{
	/*** The Windows version can't read PICT files. ***/
	
	return ( FALSE );
}

#endif

/*** IsBMPImageFile ***/

int IsBMPImageFile ( GPathPtr path )
{
	return ( IsPICTImageFile ( path ) );
}
