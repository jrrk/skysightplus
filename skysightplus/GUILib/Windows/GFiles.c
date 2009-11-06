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

/*****************************  header files  ********************************/

#include "GUILib.h"
#define GLOBAL extern
#include "GUIPriv.h"
#undef GLOBAL

/*** This structure defines the header of a Windows metafile on disk.
     It was developed by ALdus corp., and is not documented anywhere
     in Microsoft's documentation.  However, it is required to make
     metafiles readable by other programs, as it has come into nearly
     universal usage.  I've carefully defined it so the total size and
     alignment of the record will be identical on both 16- and 32-bit
     Windows platforms.  This code is based on sample code from the
     October 1995 Misccrosoft Developer Network CDs. ***/

typedef struct tagBBOX
{
	WORD	left;
	WORD	top;
	WORD	right;
    WORD	bottom;
}
BBOX;

typedef struct tagMETAFILEHEADER
{
	DWORD   key;
	WORD  	hmf;
	BBOX    bbox;
	WORD    inch;
	DWORD   reserved;
	WORD    checksum;
}
METAFILEHEADER, FAR *LPMETAFILEHEADER;

#define METAFILEHEADERKEY		0x9AC6CDD7

static WORD MetaFileHeaderCheckSum ( METAFILEHEADER FAR *lpMFH );

/*******************************  GCreate  ********************************/

int GCreate ( LPSTR lpszPath, HFILE *hFile )
{
	if ( ( *hFile = _lcreat ( lpszPath, OF_READWRITE ) ) == HFILE_ERROR )
		return ( FALSE );
	else
		return ( TRUE );
}

/*******************************  GOpen  ***********************************/

int GOpen ( LPSTR lpszPath, HFILE *hFile )
{
	if ( ( *hFile = _lopen ( lpszPath, OF_READWRITE ) ) == HFILE_ERROR )
		return ( FALSE );
	else
		return ( TRUE );
}

/*******************************  GClose  **********************************/

int GClose ( HFILE hFile )
{
	if ( _lclose ( hFile ) == HFILE_ERROR )
		return ( FALSE );
	else
		return ( TRUE );
}

/*******************************  GRead  ***********************************/

long GRead ( HFILE hFile, void *lpvBuffer, long lNumBytes )
{
	if ( _hread ( hFile, lpvBuffer, lNumBytes ) == lNumBytes )
		return ( lNumBytes );
	else
		return ( -1 );
}

/******************************  GWrite  ************************************/

long GWrite ( HFILE hFile, void *lpvBuffer, long lNumBytes )
{
	if ( _hwrite ( hFile, lpvBuffer, lNumBytes ) == lNumBytes )
		return ( lNumBytes );
	else
		return ( -1 );
}

/*******************************  GSeek  ************************************/

long GSeek ( HFILE hFile, int nMode, long lNumBytes )
{
	return ( _llseek ( hFile, lNumBytes, nMode ) );
}

/*****************************  GExecute  ***********************************/

int GExecute ( LPSTR lpPath )
{
	if ( WinExec ( lpPath, SW_SHOWNORMAL ) > 31 )
		return ( TRUE );
	else
		return ( FALSE );
}

/****************************  GCreatePath  *********************************/

LPSTR GCreatePath ( LPSTR lpszOldPath )
{
	LPSTR	lpszNewPath = NULL;

	if ( ( lpszNewPath = malloc ( MAX_PATH ) ) != NULL )
	{
		if ( lpszOldPath )
			memcpy ( lpszNewPath, lpszOldPath, MAX_PATH );
		else
			memset ( lpszNewPath, 0, MAX_PATH );
	}

	return ( lpszNewPath );
}

/*****************************  GDeletePath  ********************************/

void GDeletePath ( LPSTR lpszPath )
{
	free ( lpszPath );
}

/*****************************  GCopyPath  ********************************/

void GCopyPath ( LPSTR lpszDstPath, LPSTR lpszSrcPath )
{
	memcpy ( lpszDstPath, lpszSrcPath, MAX_PATH );
}

/***************************  GComparePaths  *******************************/

int GComparePaths ( LPSTR lpszPath1, LPSTR lpszPath2 )
{
	if ( lstrcmpi ( lpszPath1, lpszPath2 ) == 0 )
		return ( TRUE );
	else
		return ( FALSE );
}

/****************************  GPathExists  ********************************/

int GPathExists ( LPSTR lpszPath )
{
#ifdef GWIN16
	unsigned	dwAtt;
	int			nResult = _dos_getfileattr ( lpszPath, &dwAtt );

	if ( nResult == 0 )
		return ( TRUE );
	else
    	return ( FALSE );
#endif

#ifdef GWIN32
	DWORD	dwAtt = GetFileAttributes ( lpszPath );

	if ( dwAtt != 0xFFFFFFFF )
		return ( TRUE );
	else
		return ( FALSE );
#endif
}

/****************************  GFileExists  ********************************/

int GFileExists ( LPSTR lpszPath )
{
#ifdef GWIN16
	unsigned	dwAtt;
	int			nResult = _dos_getfileattr ( lpszPath, &dwAtt );

	if ( ( nResult == 0 ) && ! ( dwAtt & _A_SUBDIR ) )
		return ( TRUE );
	else
		return ( FALSE );
#endif

#ifdef GWIN32
	DWORD	dwAtt = GetFileAttributes ( lpszPath );

	if ( ( dwAtt != 0xFFFFFFFF ) && ! ( dwAtt & FILE_ATTRIBUTE_DIRECTORY ) )
		return ( TRUE );
	else
		return ( FALSE );
#endif
}

/*************************  GDirectoryExists  ******************************/

int GDirectoryExists ( LPSTR lpszPath )
{
#ifdef GWIN16
	unsigned	dwAtt;
	int			nResult = _dos_getfileattr ( lpszPath, &dwAtt );

	if ( ( nResult == 0 ) && ( dwAtt & _A_SUBDIR ) )
		return ( TRUE );
	else
		return ( FALSE );
#endif

#ifdef GWIN32
	DWORD	dwAtt = GetFileAttributes ( lpszPath );

	if ( ( dwAtt != 0xFFFFFFFF ) && ( dwAtt & FILE_ATTRIBUTE_DIRECTORY ) )
		return ( TRUE );
	else
		return ( FALSE );
#endif
}

/*************************  GDirectoryExists  ******************************/

int GPathIsReadOnly ( LPSTR lpszPath )
{
#ifdef GWIN16
	unsigned	dwAtt;
	int			nResult = _dos_getfileattr ( lpszPath, &dwAtt );

	if ( ( nResult == 0 ) && ( dwAtt & _A_RDONLY ) )
		return ( TRUE );
	else
		return ( FALSE );
#endif

#ifdef GWIN32
	DWORD	dwAtt = GetFileAttributes ( lpszPath );

	if ( ( dwAtt != 0xFFFFFFFF ) && ( dwAtt & FILE_ATTRIBUTE_READONLY ) )
		return ( TRUE );
	else
		return ( FALSE );
#endif
}

/***********************  GGetApplicationFilePath  **************************/

void GGetApplicationFilePath ( LPSTR lpszPath )
{
	GetModuleFileName ( GGetInstance(), lpszPath, MAX_PATH - 1 );
}

/***********************  GGetWorkingDirectory  ******************************/

int GGetWorkingDirectory ( LPSTR lpszPath )
{
	if ( getcwd ( lpszPath, 255 ) == NULL )
		return ( FALSE );
	else
		return ( TRUE );
}

/***********************  GSetWorkingDirectory  ******************************/

int GSetWorkingDirectory ( LPSTR lpszPath )
{
	if ( chdir ( lpszPath, 255 ) == -1 )
		return ( FALSE );
	else
    	return ( TRUE );
}

/***********************  GGetParentDirectory  *******************************/

int GGetParentDirectory ( LPSTR lpszPath )
{
	char	c = '\0';
	int 	i = lstrlen ( lpszPath );

	for ( i--; i >= 0 && c != '\\'; i-- )
	{
		c = lpszPath[i];
		lpszPath[i] = '\0';
	}

	return ( TRUE );
}

/***************************  GDoOpenFileDialog  ****************************/

LPSTR GDoOpenFileDialog ( char *lpszPrompt, char *lpszFilter, short *lpwFormat )
{
	int				i, nLength, nResult;
	char			szFileName[256] = { 0 }, szFilePath[256] = { 0 };
	OPENFILENAME	ofn;
	LPSTR			lpszPath = NULL;
	
	/*** Convert separator characters in filter string
	     to null characters, then save filter string length. ***/

	for ( i = 0; lpszFilter[i]; i++ )
		if ( lpszFilter[i] == '|' )
			lpszFilter[i] = 0;

	nLength = i;

	/*** Initialize the OPENFILENAME structure ***/

	ofn.lStructSize = sizeof ( OPENFILENAME );
	ofn.hwndOwner = GetActiveWindow();
	ofn.hInstance = NULL;
	ofn.lpstrFilter = lpszFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = *lpwFormat;
	ofn.lpstrFile = szFilePath;
	ofn.nMaxFile = sizeof ( szFilePath ) - 1;
	ofn.lpstrFileTitle = szFileName;
	ofn.nMaxFileTitle = 63;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = lpszPrompt;
	ofn.Flags = OFN_CREATEPROMPT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

	/*** Display the standard "Open File..." dialog, which returns
	     TRUE/FALSE depending on whether the result was successful
		 (i.e., whether the user hit the "OK" button). ***/

	if ( ( nResult = GetOpenFileName ( &ofn ) ) != FALSE )
	{
		if ( ( lpszPath = malloc ( 256 ) ) != NULL )
		{
        	strncpy ( lpszPath, szFilePath, 256 );
            *lpwFormat = ofn.nFilterIndex;
        }
	}

	/*** Restore the old filter string and return the path. ***/

	for ( i = 0; i < nLength; i++ )
		if ( lpszFilter[i] == 0 )
			lpszFilter[i] = '|';

	return ( lpszPath );
}

/******************************  GDoSaveFileDialog  **************************/

GPathPtr GDoSaveFileDialog ( char *lpszPrompt, char *lpszFilter, char *lpszFileName, short *lpwFormat )
{
	int				i, nLength, nResult;
	char			szFilePath[256] = { 0 };
	OPENFILENAME	ofn;
	LPSTR			lpszPath = NULL;
	
	/*** Convert separator characters in filter string
	     to null characters, then save filter string length. ***/

	for ( i = 0; lpszFilter[i]; i++ )
		if ( lpszFilter[i] == '|' )
			lpszFilter[i] = 0;

	nLength = i;
	
	/*** Initialize the OPENFILENAME structure ***/

	strcpy ( szFilePath, lpszFileName );

	ofn.lStructSize = sizeof ( OPENFILENAME );
	ofn.hwndOwner = GetActiveWindow();
	ofn.hInstance = NULL;
	ofn.lpstrFilter = lpszFilter;
	ofn.lpstrCustomFilter = NULL;
 	ofn.nMaxCustFilter = 0;
 	ofn.nFilterIndex = *lpwFormat;
 	ofn.lpstrFile = szFilePath;
	ofn.nMaxFile = sizeof ( szFilePath ) - 1;
	ofn.lpstrFileTitle = lpszFileName;
	ofn.nMaxFileTitle = 255;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = lpszPrompt;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

	/*** Display the standard "Save File..." dialog, which returns
	     TRUE/FALSE depending on whether the result was successful
		 (i.e., whether the user hit the "OK" button). ***/

	if ( ( nResult = GetSaveFileName ( &ofn ) ) != FALSE )
	{
		if ( ( lpszPath = malloc ( 256 ) ) != NULL )
		{
        	strncpy ( lpszPath, szFilePath, 255 );
            *lpwFormat = ofn.nFilterIndex;
        }
	}

	/*** Restore the old filter string and return the path. ***/

	for ( i = 0; i < nLength; i++ )
		if ( lpszFilter[i] == 0 )
			lpszFilter[i] = '|';

	return ( lpszPath );
}

/*****************************  GGetFileType  *************************/

char *GGetFileType ( LPSTR lpszPath, char *ext )
{
	char    title[256];
	int     i, k;

	GetFileTitle ( lpszPath, title, 255 );

	for ( i = 0; title[i] && title[i] != '.'; i++ )
		;
	for ( k = 0; title[i] && k < 4; k++, i++ )
		ext[k] = title[i];
                
	ext[k] = 0;
        
	return ( ext );
}

/****************************  GGetPathName  **************************/

void GGetPathName ( LPSTR lpszPath, char *lpszFileName )
{
	GetFileTitle ( lpszPath, lpszFileName, 255 );
}

/************************  GSetPathName  *******************************/

int GSetPathName ( LPSTR lpszPath, char *lpszFileName )
{
	int		i;

	for ( i = 255; i > 0; i-- )
		if ( lpszPath[i] == '\\' || lpszPath[i] == ':' )
			break;

	lstrcpyn ( lpszPath + i + 1, lpszFileName, 255 - i );
	
	return ( TRUE );
}

/************************  GAppendPathName  **************************/

int GAppendPathName ( LPSTR lpszPath, char *lpszFileName )
{
	int nLength = lstrlen ( lpszPath );
	
	if ( lpszPath[ nLength - 1 ] != '\\' )
		lstrcat ( lpszPath, "\\" );
		
	lstrcat ( lpszPath, lpszFileName );
	
	return ( TRUE );
}

/************************  GCreateDirectory  **************************/

int GCreateDirectory ( LPSTR lpszPath )
{
	if ( mkdir ( lpszPath ) == 0 )
		return ( TRUE );
	else
		return ( FALSE );
}

/************************  GDeleteDirectory  **************************/

int GDeleteDirectory ( LPSTR lpszPath )
{
	if ( rmdir ( lpszPath ) == 0 )
		return ( TRUE );
	else
		return ( FALSE );
}

/************************  GDeleteFile  **************************/

int GDeleteFile ( LPSTR lpszPath )
{
	if ( unlink ( lpszPath ) == 0 )
		return ( TRUE );
	else
		return ( FALSE );
}

/**************************  GGetFileSize  ***************************/

long GGetFileSize ( LPSTR lpszPath )
{
	long		lSize = 0;
	HFILE		hFile = _lopen ( lpszPath, OF_READ );

	if ( hFile != HFILE_ERROR )
	{
		lSize = _llseek ( hFile, 0, SEEK_END );
		_llseek ( hFile, 0, SEEK_SET );
		_lclose ( hFile );
	}

	return ( lSize );
}

/*****************************  GOpenFile  ***************************/

FILE *GOpenFile ( LPSTR lpszPath, char *mode, char *type, char *creator )
{
	return ( fopen ( lpszPath, mode ) );
}

/*************************  GReadLine  ********************************/

long GReadLine ( FILE *lpFile, char *lpszLine, long lSize, char *lpszEOL )
{
	long	i, lOffset, lLength, lBytes;
	
	/*** Read one buffer's worth of data from the file.
	     Then determine the number of bytes actually read.
	     If we have an end-of-line buffer, null-terminate it. ***/

	lOffset = ftell ( lpFile );
	fread ( lpszLine, lSize - 1, 1, lpFile );
	lLength = lBytes = ftell ( lpFile ) - lOffset;

	if ( lpszEOL )
		lpszEOL[0] = '\0';

	/*** Search the buffer for end-of-line character(s).
		 If we find them, determine the line format and length.
		 For DOS and Mac line formats, convert the end-of-line
		 mark to a standard newline character. ***/

	for ( i = 0; i < lBytes; i++ )
	{
		if ( lpszLine[i] == '\r' )
		{
			if ( i == lSize - 2 )
			{
				lLength = i;
			}
			else
			{			
				if ( lpszLine[ i + 1 ] == '\n' )
				{
					lLength = i + 2;
					if ( lpszEOL )
						lstrcpy ( lpszEOL, "\r\n" );
				}
				else
				{
					lLength = i + 1;
					if ( lpszEOL )
						strcpy ( lpszEOL, "\r" );
				}
			}
			break;
		}

		if ( lpszLine[i] == '\n' )
		{
			lLength = i + 1;
			if ( lpszEOL )
				strcpy ( lpszEOL, "\n" );
			break;
		}
	}

	/*** Null-terminate the buffer.  If the line was shorter than
		 the number of bytes read into the buffer, rewind to the end
		 of the line.  Finally, return the number of bytes read. ***/

	lpszLine[i] = '\0';

	if ( lLength < lBytes )
		fseek ( lpFile, lOffset + lLength, SEEK_SET );

	return ( lLength );
}

/******************************  GWriteLine  *********************************/

long GWriteLine ( FILE *lpFile, char *lpszLine, long lSize, char *lpszEOL )
{
	long	i, lOffset;
	
	/*** Determine the number of bytes to write to the file. ***/

	for ( i = 0; i < lSize; i++ )
		if ( lpszLine[i] == '\0' )
		{
			lSize = i;
			break;
		}

	/*** If an end-of-line string has not been specified,
		 use the default DOS end-of-line mark (CR-LF). ***/

	if ( lpszEOL == NULL )
		lpszEOL = "\r\n";

	/*** Record the current file offset, then write the text to the file,
	     followed by the end-of-line mark.  Return the number of bytes
	     actually written to the file. ***/

	lOffset = ftell ( lpFile );

	fwrite ( lpszLine, lSize, 1, lpFile );
	fwrite ( lpszEOL, lstrlen ( lpszEOL ), 1, lpFile );

	return ( ftell ( lpFile ) - lOffset );
}

/***************************  GWriteBitmapFile  ******************************/

long GWriteBitmapFile ( LPSTR lpszPath, HBITMAP hBmp )
{
	long				lSizeDIB;
	long				lNumColors = 0;
	long				lBytes = 0;
	HFILE				hFile;
	HGLOBAL				hDIB;
	LPBITMAPINFOHEADER	lpDIB;
	BITMAPFILEHEADER	header;

	lBytes = sizeof ( size_t );

	if ( ( hDIB = GBitmapToDIB ( hBmp, &lSizeDIB ) ) != NULL )
	{
		lpDIB = (BITMAPINFOHEADER *) GlobalLock ( hDIB );

		/*** Determine number of colors in color table following DIB header ***/

		if ( lpDIB->biBitCount < 24 )
			lNumColors = 1L << lpDIB->biBitCount;
		else
			lNumColors = 0;

		/*** Initialize the bitmap file header structure.  Note that we need to set
		     the "bfType" structure field to 'MB' to ensure that the file signature
		     gets written out as 'BM', due to byte-swapping on the Intel platform. ***/

		header.bfType = 'MB';
		header.bfSize = sizeof ( header ) + lSizeDIB;
		header.bfReserved1 = 0;
		header.bfReserved2 = 0;
		header.bfOffBits = sizeof ( header ) + lpDIB->biSize + ( lNumColors * sizeof ( RGBQUAD ) );

		/*** Open the file.  If successful, write the file header, then the
			 device-independent bitmap data..  Record the number of bytes written.
			 Finally, close the file and free the DIB handle. ***/

		if ( ( hFile = _lcreat ( lpszPath, 0 ) ) != HFILE_ERROR )
		{
			lBytes += _hwrite ( hFile, &header, sizeof ( header ) );
			lBytes += _hwrite ( hFile, lpDIB, lSizeDIB );
			_lclose ( hFile );
		}

		GlobalUnlock ( hDIB );
		GlobalFree ( hDIB );
	}

	return ( lBytes );
}

/**********************  MetaFileHeaderCheckSum  ************************/

WORD MetaFileHeaderCheckSum ( METAFILEHEADER FAR *lpMFH )
{
	LPWORD	lpWord = (LPWORD) lpMFH;
	WORD	i, wCheckSum = lpWord[0];

	for ( i = 1; i <= 9; i++ )
		wCheckSum ^= lpWord[i];

	return ( wCheckSum );
}

/************************  GWritePictureFile  ***************************/

long GWritePictureFile ( LPSTR lpszPath, LPMETAFILEPICT lpPict )
{
	long			lSize, lBytes = 0;
	HDC				hDC;
    HMETAFILE		hMF;
    HFILE			hFile;
	HGLOBAL			hMFBits;
    LPSTR			lpMFBits;
	METAFILEHEADER	header;

    /*** Initialize the fields of the metafiel header record. ***/

	hDC = GetDC ( NULL );

	header.key = METAFILEHEADERKEY;
	header.hmf = (WORD) NULL;
	header.inch = GetDeviceCaps ( hDC, LOGPIXELSX );
    header.reserved = 0;
	header.bbox.left = 0;
	header.bbox.top = 0;
	header.bbox.right = lpPict->xExt;
	header.bbox.bottom = lpPict->yExt;
	header.checksum = MetaFileHeaderCheckSum ( &header );

	ReleaseDC ( NULL, hDC );

#ifdef GWIN32

	/*** Determine the size of the metafile data, and allocate a
         buffer for it. ***/
    
	hMF = lpPict->hMF;
	lSize = GetMetaFileBitsEx ( hMF, 0, NULL );
    hMFBits = GlobalAlloc ( GHND, lSize );
	if ( hMFBits != NULL )
	{
		/*** If successful, copy the data into the buffer. ***/
        
		lpMFBits = GlobalLock ( hMFBits );
		lSize = GetMetaFileBitsEx ( hMF, lSize, lpMFBits );

        /*** Create and open the file.  If successful, write the header
             into it, followed by the data.  When done, close the file
             and free the metafile data buffer. ***/
             
		hFile = _lcreat ( lpszPath, OF_READ );
		if ( hFile != HFILE_ERROR )
		{
			lBytes += _hwrite ( hFile, &header, sizeof ( header ) );
			lBytes += _hwrite ( hFile, lpMFBits, lSize );
			_lclose ( hFile );
		}

		GlobalUnlock ( hMFBits );
		GlobalFree ( hMFBits );
	}

#else

	/*** Make a copy of the metafile, since GetMetaFileBits() will
         invalidate the given metafile handle.  If sucessful, get
         the metafile data. ***/
         
	hMF = CopyMetaFile ( lpPict->hMF, NULL );
    if ( hMF != NULL )
    {
		hMFBits = GetMetaFileBits ( hMF );
		if (  hMFBits != NULL )
		{
			/*** If successful, create and open a file.  If successful,
                 write the header, followed by the data; the close the
                 file, and delete the copy of the metafile data. ***/

			lpMFBits = GlobalLock ( hMFBits );
            hFile = _lcreat ( lpszPath, OF_READ );
			if ( hFile != HFILE_ERROR )
			{
				lBytes += _hwrite( hFile, &header, sizeof ( header ) );
				lBytes += _hwrite( hFile, lpMFBits, GlobalSize ( hMFBits ) );
				_lclose ( hFile );
			}

			GlobalUnlock ( hMFBits );
			GlobalFree ( hMFBits );
		}
	}

#endif

	/*** Return the number of bytes written. ***/
    
    return ( lBytes );
}

/**************************  GReadPictureFile  ***************************/

#ifdef GWIN16
#define SetMetaFileBitsEx(lSize,hMF)	SetMetaFileBits(hMF)
#endif

LPMETAFILEPICT GReadPictureFile ( LPSTR lpszPath )
{
	int				xExt, yExt;
	long			lOffset;
	HDC				hDC;
	HFILE			hFile;
	LPMETAFILEPICT	lpPict = NULL;
	METAHEADER		metaheader;
	METAFILEHEADER  fileheader;

	if ( ( hFile = _lopen ( lpszPath, OF_READ ) ) != HFILE_ERROR )
	{
		lOffset = _lread ( hFile, &fileheader, sizeof ( METAFILEHEADER ) );

		if ( fileheader.key == METAFILEHEADERKEY )
		{
            hDC = GetDC ( NULL );
			xExt = (long) GetDeviceCaps ( hDC, LOGPIXELSX ) * ( fileheader.bbox.right - fileheader.bbox.left ) / fileheader.inch;
			yExt = (long) GetDeviceCaps ( hDC, LOGPIXELSY ) * ( fileheader.bbox.bottom - fileheader.bbox.top ) / fileheader.inch;
			ReleaseDC ( NULL, hDC );
		}
		else
		{
			xExt = 0;
            yExt = 0;
			lOffset = _llseek ( hFile, 0, 0 );
		}

		_lread ( hFile, &metaheader, sizeof ( METAHEADER ) );

		if ( metaheader.mtHeaderSize == 9 )
		{
			if ( ( lpPict = GlobalAllocPtr ( GPTR, sizeof ( METAFILEPICT ) ) ) != NULL )
			{
				if ( ( lpPict->hMF = GlobalAlloc ( GHND, metaheader.mtSize * 2 ) ) != NULL )
				{
					_llseek ( hFile, lOffset, 0 );
					_hread ( hFile, GlobalLock ( lpPict->hMF ), metaheader.mtSize * 2 );
					GlobalUnlock ( lpPict->hMF );

					lpPict->mm = MM_TEXT;
					lpPict->xExt = xExt;
					lpPict->yExt = yExt;
					lpPict->hMF = SetMetaFileBitsEx ( metaheader.mtSize * 2, lpPict->hMF );
				}
			}
		}

		_lclose ( hFile );
	}

	return ( lpPict );
}


/***********************  GCopyBitmapToClipboard  ****************************/

void GCopyBitmapToClipboard ( HBITMAP hBmp )
{
	long	lSize;
	HGLOBAL	hDIB;

	if ( ( hDIB = GBitmapToDIB ( hBmp, &lSize ) ) != NULL )
	{
		OpenClipboard ( GGetMainWindow() );
		EmptyClipboard();
		SetClipboardData ( CF_DIB, hDIB );
		CloseClipboard();
	}
}

/***********************  GCopyPictureToClipboard  ****************************/

void GCopyPictureToClipboard ( LPMETAFILEPICT lpPict )
{
	POINT			point;
	HDC				hDC;
	HGLOBAL			hNewPict;
	LPMETAFILEPICT	lpNewPict;

	point.x = lpPict->xExt;
	point.y = lpPict->yExt;

	if ( ( hDC = GetDC ( NULL ) ) != NULL )
	{
		SaveDC ( hDC );
		SetMapMode ( hDC, MM_HIMETRIC );
		DPtoLP ( hDC, &point, 1 );
        RestoreDC ( hDC, -1 );
		ReleaseDC ( NULL, hDC );
	}

	if ( ( hNewPict = GlobalAlloc ( GHND, sizeof ( METAFILEPICT ) ) ) != NULL )
	{
		lpNewPict = GlobalLock ( hNewPict );

		lpNewPict->mm = MM_ANISOTROPIC;
		lpNewPict->xExt = abs ( point.x );
		lpNewPict->yExt = abs ( point.y );
		lpNewPict->hMF = CopyMetaFile ( lpPict->hMF, NULL );

		GlobalUnlock ( hNewPict );
		OpenClipboard ( GGetMainWindow() );
		EmptyClipboard();
		SetClipboardData ( CF_METAFILEPICT, hNewPict );
		CloseClipboard();
	}
}

/*****************************  GBitmapToDIB  ******************************

	Converts a device-dependent bitmap to a device-independent bitmap (DIB).

	HGLOBAL GBitmapToDIB ( HBITMAP bitmap, long *dibSize )

	(bitmap): handle to device-dependent bitmap.
	(dibSize): receives size of resulting device-independent bitmap, in bytes.

	If successful, returns a handle to the device-independent bitmap, and
	returns the size of the handle in (dibSize.)  On failure, returns NULL,
	and (dibSize) receives zero.

***************************************************************************/

HGLOBAL GBitmapToDIB ( HBITMAP hBmp, long *lpSizeDIB )
{
	long				lWidth;
	long				lHeight;
	long				lBitsPerPixel;
	long				lNumColors;
	long				lSize;
	HDC					hDC;
	BITMAP				bm;
	BITMAPINFOHEADER	*lpBMI;
	HGLOBAL				hDIB;
	LPSTR				lpDIB;

	/*** Defaults ***/

	*lpSizeDIB = 0;
	lpDIB = NULL;

	/*** Obtain a handle to the screen's device context.  If successful,
		 and the bitmap handle is valid, convert it to DIB format ***/

	if ( ( hDC = GetDC ( NULL ) ) != NULL )
	{
		/*** Determine width, height, and bits/pixel of original bitmap ***/

		GetObject ( hBmp, sizeof ( BITMAP ), &bm );

		lWidth = bm.bmWidth;
		lHeight = bm.bmHeight;
		lBitsPerPixel = bm.bmPlanes * bm.bmBitsPixel;

		/*** Determine number of colors needed for color table (a 24-bit/pixel
			 bitmap needs no color table since the image data array will contain
			 the image's RGB color values written out explicitly) ***/

		if ( lBitsPerPixel < 24 )
			lNumColors = 1L << lBitsPerPixel;
		else
			lNumColors = 0;

		/*** Now allocate a buffer to hold the bitmap info header, plus
			 enough bytes to hold the color table ***/

		lSize = sizeof ( BITMAPINFOHEADER ) + lNumColors * sizeof ( RGBQUAD );

		if ( ( lpBMI = (BITMAPINFOHEADER *) GlobalAllocPtr ( GMEM_MOVEABLE, lSize ) ) != NULL )
		{
			lpBMI->biSize = sizeof ( BITMAPINFOHEADER );
			lpBMI->biWidth = bm.bmWidth;
			lpBMI->biHeight = bm.bmHeight;
			lpBMI->biPlanes = 1;
			lpBMI->biBitCount = bm.bmPlanes * bm.bmBitsPixel;
			lpBMI->biCompression = 0;
			lpBMI->biSizeImage = G_BITMAP_ROW_BYTES ( lBitsPerPixel, lWidth ) * lHeight;
			lpBMI->biXPelsPerMeter = 0;
			lpBMI->biYPelsPerMeter = 0;
			lpBMI->biClrUsed = 0;
			lpBMI->biClrImportant = 0;

			/*** Now allocate enough memory to hold the bitmap header plus
				 image data.  If successful, copy the DIB header and color
				 table information into the new buffer.  Then call GetDIBits()
				 again to copy the image into the new buffer in device-
			     independent format. ***/

			*lpSizeDIB = lSize + lpBMI->biSizeImage;
			if ( ( hDIB = GlobalAlloc ( GMEM_MOVEABLE, *lpSizeDIB ) ) != NULL )
			{
				lpDIB = GlobalLock ( hDIB );
				memcpy ( lpDIB, lpBMI, lSize );
				GetDIBits ( hDC, hBmp, 0, bm.bmHeight, lpDIB + lSize, (BITMAPINFO *) lpDIB, DIB_RGB_COLORS );
				GlobalUnlock ( hDIB );
			}
			GlobalFreePtr ( lpBMI );
		}
		ReleaseDC ( NULL, hDC );
	}

	/*** Return the handle to the device-independent bitmap ***/

	return ( hDIB );
}

