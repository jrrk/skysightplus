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

#include "AstroLib.h"

/***********************  NewFITSHeader  *************************/

int NewFITSHeader ( char ***header )
{
	long i, j, k, n, n1;
	char **header1;
	
	/*** Count the number of rows in the current header matrix ***/
	
	if ( (*header) == NULL )
		n = 0;
	else
		for ( n = 0; (*header)[n] != NULL; n++ )
			;

	/*** Allocate an array which contains enough pointers to point
	     to the current header matrix rows, plus 36 new ones, plus
	     a terminating NULL pointer. ***/ 
	     
	n1 = n + 36;
	header1 = (char **) malloc ( sizeof ( char * ) * ( n1 + 1 ) );
	if ( header1 == NULL )
		return ( FALSE );
		
	/*** Copy the current header matrix row pointers into the new
	     header matrix row pointer array. ***/
	     
	for ( i = 0; i < n; i++ )
		header1[i] = (*header)[i];
	
	/*** Now allocate the new matrix rows.  On failure, free
	     everything allocated thus far; if successful, fill the
	     new matrix rows with null-terminated blank strings.
	     Add a NULL pointer after the last row. ***/
	
	for ( i = n; i < n1; i++ )
	{
		header1[i] = malloc ( 81 );
		
		if ( header1[i] == NULL )
		{
			for ( k = n; k < i; k++ )
				free ( header1[k] );
			free ( header1 );
			return ( FALSE );
		}
		
		for ( j = 0; j < 80; j++ )
			header1[i][j] = ' ';
		header1[i][80] = '\0';
	}
	
	header1[n1] = NULL;
	
	/*** Free the old header matrix; replace it with the new onw,
	     and return a successful result code. ***/
	
	free ( *header );
	*header = header1;
	return ( TRUE );
}

/***************************  FreeFITSHeader  ****************************/

void FreeFITSHeader ( char **header )
{
	long l;
	
	for ( l = 0; header[l] != NULL; l++ )
		free ( header[l] );
		
	free ( header );
}

/*** CopyFITSHeader ******************************************************/

FITSHeader CopyFITSHeader ( FITSHeader source, FITSHeader dest )
{
	long line;
	
	/*** If we have not yet been given a destination FITS header,
	     create a new one. ***/
	     
	if ( dest == NULL )
	{
		long k, numLines = CountFITSHeaderLines ( source );
		
		/*** Create an array of pointers to strings large enough to hold
		     all the lines in the source header, plus one terminating NULL
		     pointer. ***/
		     
		dest = (FITSHeader) malloc ( sizeof ( char * ) * ( numLines + 1 ) );
		if ( dest == NULL )
			return ( NULL );
		
		/*** For each string pointer, allocate a buffer to hold an 80-
		     character string plus terminating NULL character.  On failure,
		     delete all memory allocated thus far, and return NULL. ***/
		
		for ( line = 0; line < numLines; line++ )
		{
			dest[line] = malloc ( 81 );
			if ( dest[line] == NULL )
			{
				for ( k = 0; k < line; k++ )
					free ( dest[k] );
				
				free ( dest );	
				return ( NULL );
			}
		}
		
		/*** Set the last header line string pointer to NULL. ***/
		
		dest[line] = NULL;
	}

	/*** Copy each line from the source into the destination, quitting
	     when we reach the end of either header.  Return a pointer to the
	     destination FITS header. ***/
	
	for ( line = 0; source[line] != NULL && dest[line] != NULL; line++ )
		memcpy ( dest[line], source[line], 81 );
		
	return ( dest );	
}

/***************************  ReadFITSHeader  ****************************/

int ReadFITSHeader ( FILE *file, char ***header )
{
	long end = FALSE, i = 0, j;
	
	while ( end == FALSE )
	{
		/*** Allocate memory for 36 new rows of FITS header data;
		     on failure, return NULL. ***/
		     
		if ( NewFITSHeader ( header ) == FALSE )
			return ( FALSE );
		
		/*** Read 36 lines of 80 characters each from the file;
		     on failure, free the memory allocated for the FITS header
		     matrix thus far, and return NULL. ***/ 
		
		for ( j = i + 36; i < j; i++ )
		{
			if ( fread ( (*header)[i], 80, 1, file ) != 1 )
			{
				FreeFITSHeader ( *header );
				return ( FALSE );
			}
		
			/*** If we detect the "END" FITS header keyword on any
			     of the lines we read from the file, then we've read
			     the last header block.  Otherwise, continue to read
			     header blocks (in 36 lines of 80 characters each)
			     from the FITS file. ***/
			
			if ( TestFITSHeaderKeyword ( (*header)[i], "END     " ) == TRUE )
				end = TRUE;
		}
	}
	
	return ( TRUE );
}

/**************************  WriteFITSHeader  **************************/

int WriteFITSHeader ( FILE *file, char **header )
{
	long end = FALSE, i = 0, j;

	/*** Write 2880-byte blocks of FITS header data from the 
	     character matrix to the file, in 36 lines of 80 characters
	     each.  On failure, return FALSE.  Continue until we reach
	     a block containing the "END" keyword or until we reach a NULL
	     matrix row. ***/ 
		
	while ( header[i] != NULL && end == FALSE )
	{
		for ( j = i + 36; i < j; i++ )
		{
			if ( fwrite ( header[i], 80, 1, file ) != 1 )
				return ( FALSE );
		
			if ( TestFITSHeaderKeyword ( header[i], "END     " ) == TRUE )
				end = TRUE;
		}
	}
				
	return ( TRUE );
}

/***********************  TestFITSHeaderKeyword  ***************************/

int TestFITSHeaderKeyword ( char *line, char *keyword )
{
	if ( strncmp ( line, keyword, strlen ( keyword ) ) == 0 )
		return ( TRUE );
	else
		return ( FALSE );
}

/************************  FindFITSHeaderKeyword  ***************************/

int FindFITSHeaderKeyword ( char **header, char *keyword, long *line )
{
	long n;
	
	for ( n = *line; header[n] != NULL; n++ )
	{
		if ( TestFITSHeaderKeyword ( header[n], keyword ) == TRUE )
		{
			*line = n;
			return ( TRUE );
		}
	}
		
	return ( FALSE );
}

/************************  CountFITSHeaderLines  ***************************/

long CountFITSHeaderLines ( char **header )
{
	long n;
	
	for ( n = 0; header[n] != NULL; n++ )
		;
	return ( n );
}

/**************************  GetFITSHeaderLine  *****************************/

char *GetFITSHeaderLine ( char **header, long line )
{
	long n;
	
	for ( n = 0; n < line; n++ )
		if ( header[n] == NULL )
			return ( NULL );
			
	return ( header[n] );
}

/**********************  GetFITSHeaderKeyword  ************************/

void GetFITSHeaderKeyword ( char *line, char *keyword )
{
	strncpy ( keyword, line, 8 );
}

/**********************  GetFITSHeaderInteger  ************************/

void GetFITSHeaderInteger ( char *line, long *value )
{
	*value = atol ( &line[10] );
}

/************************  GetFITSHeaderReal  *************************/

void GetFITSHeaderReal ( char *line, double *value )
{
	*value = atof ( &line[10] );
}

/**********************  GetFITSHeaderString  *************************/

void GetFITSHeaderString ( char *line, char *string )
{
	int i, k;
	
	for ( k = 0, i = 11; i < 31 && line[i] != '\''; i++, k++ )
		string[k] = line[i];
		
	string[k] = '\0';
}

/*************************  SetFITSHeaderLine  ****************************/

int SetFITSHeaderLine ( char ***header, long n, char *line )
{
	int i, k;
	
	for ( i = 0; i <= n; i++ )
		if ( (*header) == NULL || (*header)[i] == NULL )
			if ( NewFITSHeader ( header ) == FALSE )
				return ( FALSE );
				
	for ( i--, k = 0; line[k] != '\0' && k < 80; k++ )
		(*header)[i][k] = line[k];

	return ( TRUE );
}

/*************************  SetFITSHeaderKeyword  ************************/

void SetFITSHeaderKeyword ( char *line, char *keyword )
{
	/*** We save the value of the character in the input line
	     which will be overwritten by the '\0' appended to the
	     output of sprintf(); then we replace it afterwards. ***/

	char temp = line[8];
	
	/*** If the keyword string is eight or more characters,
	     only copy the first 8; otherwise pad it with whitespace. ***/
	     
	if ( strlen ( keyword ) >= 8 )
		strncpy ( line, keyword, 8 );
	else
		sprintf ( line, "%-8s", keyword );
		
	line[8] = temp;
}

/************************  SetFITSHeaderLogical  *************************/

void SetFITSHeaderLogical ( char *line, int value )
{
	/*** We save the value of the character in the input line
	     which will be overwritten by the '\0' appended to the
	     output of sprintf(); then we replace it afterwards. ***/
	     
	char	temp = line[32];

	if ( value == TRUE )
		sprintf ( &line[8], "= %20c /", 'T' );
	else
		sprintf ( &line[8], "= %20c /", 'F' );
			
	line[32] = temp;
}

/*************************  SetFITSHeaderInteger  *************************/

void SetFITSHeaderInteger ( char *line, long value )
{
	/*** We save the value of the character in the input line
	     which will be overwritten by the '\0' appended to the
	     output of sprintf(); then we replace it afterwards. ***/

	char temp = line[32];
	
	sprintf ( &line[8], "= %20ld /", value );
	
	line[32] = temp;
}

/************************  SetFITSHeaderReal  **************************/

void SetFITSHeaderReal ( char *line, double value )
{
	/*** We save the value of the character in the input line
	     which will be overwritten by the '\0' appended to the
	     output of sprintf(); then we replace it afterwards. ***/

	char temp = line[32];
	
	sprintf ( &line[8], "= %+20.13E /", value );

	line[32] = temp;
}

/**********************  SetFITSHeaderString  **************************/

void SetFITSHeaderString ( char *line, char *value )
{
	long k;
	
	sprintf ( &line[8], "= \'%s\'", value );
	
	for ( k = strlen ( line ); k < 31; k++ )
		line[k] = ' ';
		
	line[31] = '/';
}

/**********************  SetFITSHeaderLineKeyword  ************************/

int SetFITSHeaderLineKeyword ( char ***header, long n, char *keyword )
{
	char line[33] = { '\0' };
	
	SetFITSHeaderKeyword ( line, keyword );
	
	return ( SetFITSHeaderLine ( header, n, line ) );
}

/*******************  SetFITSHeaderLineKeywordLogical  ********************/

int SetFITSHeaderLineKeywordLogical ( char ***header, long n, char *keyword, int value )
{
	char line[33] = { '\0' };
	
	SetFITSHeaderKeyword ( line, keyword );
	SetFITSHeaderLogical ( line, value );

	return ( SetFITSHeaderLine ( header, n, line ) );
}

/*******************  SetFITSHeaderLineKeywordInteger  ********************/

int SetFITSHeaderLineKeywordInteger ( char ***header, long n, char *keyword, long value )
{
	char line[33] = { '\0' };
	
	SetFITSHeaderKeyword ( line, keyword );
	SetFITSHeaderInteger ( line, value );

	return ( SetFITSHeaderLine ( header, n, line ) );
}

/*********************  SetFITSHeaderLineKeywordReal  *********************/

int SetFITSHeaderLineKeywordReal ( char ***header, long n, char *keyword, double value )
{
	char line[33] = { '\0' };
	
	SetFITSHeaderKeyword ( line, keyword );
	SetFITSHeaderReal ( line, value );

	return ( SetFITSHeaderLine ( header, n, line ) );
}

/*******************  SetFITSHeaderLineKeywordString  ********************/

int SetFITSHeaderLineKeywordString ( char ***header, long n, char *keyword, char *value )
{
	char line[33] = { '\0' };
	
	SetFITSHeaderKeyword ( line, keyword );
	SetFITSHeaderString ( line, value );
	
	return ( SetFITSHeaderLine ( header, n, line ) );
}

/*************************  SetFITSImageHeaderInfo  *********************/

void SetFITSImageHeaderInfo ( char ***header, long bitpix, long naxis,
long naxis1, long naxis2, long naxis3, double bzero, double bscale )
{
	long	  n = 0;
	
	/*** Add the minimum required FITS image header keywords,
	     as well as the optional BZERO and BSCALE keywords,
	     which describe the scaling of the image data. ***/
	      
	SetFITSHeaderLineKeywordLogical ( header, n++, "SIMPLE", TRUE );
	SetFITSHeaderLineKeywordInteger ( header, n++, "BITPIX", bitpix );
	SetFITSHeaderLineKeywordInteger ( header, n++, "NAXIS", naxis );

	if ( naxis >= 1 ) 
		SetFITSHeaderLineKeywordInteger ( header, n++, "NAXIS1", naxis1 );

	if ( naxis >= 2 )
		SetFITSHeaderLineKeywordInteger ( header, n++, "NAXIS2", naxis2 );

	if ( naxis >= 3 )
		SetFITSHeaderLineKeywordInteger ( header, n++, "NAXIS3", naxis3 );
		
	SetFITSHeaderLineKeywordReal ( header, n++, "BZERO", bzero );
	SetFITSHeaderLineKeywordReal ( header, n++, "BSCALE", bscale );
	
	SetFITSHeaderLineKeyword ( header, n, "END" );
}

/***********************  GetFITSImageHeaderInfo  ***********************/

void GetFITSImageHeaderInfo ( char **header, long *bitpix, long *naxis,
long *naxis1, long *naxis2, long *naxis3, double *bzero, double *bscale )
{
	long	n;
	char	*line;

	/*** Loop through the lines in the header matrix, and look
	     for the keywords which describe the formatting of the
	     image data.  Save parameters as we find them. ***/
	     
	for ( n = 0; ( line = GetFITSHeaderLine ( header, n ) ) != NULL; n++ )
	{
		if ( TestFITSHeaderKeyword ( line, "BITPIX  " ) )
			GetFITSHeaderInteger ( line, bitpix );

		if ( TestFITSHeaderKeyword ( line, "NAXIS   " ) )
			GetFITSHeaderInteger ( line, naxis );

		if ( TestFITSHeaderKeyword ( line, "NAXIS1  " ) )
			GetFITSHeaderInteger ( line, naxis1 );

		if ( TestFITSHeaderKeyword ( line, "NAXIS2  " ) )
			GetFITSHeaderInteger ( line, naxis2 );

		if ( TestFITSHeaderKeyword ( line, "NAXIS3  " ) )
			GetFITSHeaderInteger ( line, naxis3 );
			
		if ( TestFITSHeaderKeyword ( line, "BZERO   " ) )
			GetFITSHeaderReal ( line, bzero );

		if ( TestFITSHeaderKeyword ( line, "BSCALE  " ) )
			GetFITSHeaderReal ( line, bscale );
	}
}

/*****************************  ByteSwap  *******************************/

void ByteSwap ( void *buffer, long num, short size )
{
	short	i;
	char	temp, *ptr1, *ptr2;
	
	for ( i = 0; i < num; i++ )
	{
		ptr1 = (char *) buffer + size * i;
		for ( ptr2 = ptr1 + size - 1; ptr1 < ptr2; ptr1++, ptr2-- )
		{
			temp = *ptr1;
			*ptr1 = *ptr2;
			*ptr2 = temp;
		}
	}
}

/************************  ReadFITSImageDataRow  ************************/

int ReadFITSImageDataRow ( FILE *file, long bitpix, long naxis1, double bzero,
double bscale, PIXEL *data )
{
	int				result = FALSE;
	long			col, size;
	char			*buffer;
	unsigned char	*buffer8;
	short			*buffer16;
	long			*buffer32;
	float			*buffer32f;
	DOUBLE			*buffer64f;
	
	/*** Compute the number of bytes occupied by a raw image data row.
	     Allocate memory to hold the raw image data; on failure,
	     return an error code. ***/
	     
	size = naxis1 * abs ( bitpix / 8 );
	buffer = malloc ( size );
	if ( buffer == NULL )
		return ( FALSE );
	
	/*** Read the image data row into the buffer allocated above;
	     on failure, return an error code. ***/
		
	if ( fread ( buffer, size, 1, file ) != 1 )
	{
		free ( buffer );
		return ( FALSE );
	}
	
	/*** If necessary, byte-swap the raw image data. ***/
			
#if BYTESWAP
	ByteSwap ( buffer, naxis1, abs ( bitpix / 8 ) );
#endif

	/*** Now copy the raw image data into the output image data row
	     buffer, applying offset, scaling and casting to the correct
	     data type. ***/
	     
	switch ( bitpix )
	{
		case 8:
			buffer8 = (unsigned char *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer8[col] * bscale + bzero;
			else		
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer8[col];
			break;
		
		case 16:
			buffer16 = (signed short *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer16[col] * bscale + bzero;
			else
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer16[col];
			break;

		case 32:
			buffer32 = (signed long *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer32[col] * bscale + bzero;
			else
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer32[col];
			break;

		case -32:
			buffer32f = (float *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer32f[col] * bscale + bzero;
			else
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer32f[col];
			break;

		case -64:
			buffer64f = (DOUBLE *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer64f[col] * bscale + bzero;
			else
				for ( col = 0; col < naxis1; col++ )
					data[col] = buffer64f[col];
			break;
	}

	/*** Free the raw image data buffer; return a successul
	     result code. ***/
	     					
	free ( buffer );
	return ( TRUE );
}

/***********************  WriteFITSImageDataRow  ***********************/

int WriteFITSImageDataRow ( FILE *file, long bitpix, long naxis1, double bzero,
double bscale, PIXEL *data )
{
	int				result = FALSE;
	long			col, size;
	char			*buffer;
	unsigned char	*buffer8;
	short			*buffer16;
	long			*buffer32;
	float			*buffer32f;
	DOUBLE			*buffer64f;
	
	/*** Compute the number of bytes occupied by a raw image data row.
	     Allocate memory to hold the raw image data; on failure,
	     return an error code. ***/
	     
	size = naxis1 * abs ( bitpix / 8 );
	buffer = malloc ( size );
	if ( buffer == NULL )
		return ( FALSE );
		
	/*** Now copy the image data row into the raw image datya buffer,
	     buffer, applying offset, scaling and casting to the correct
	     data type. ***/

	switch ( bitpix )
	{
		case 8:
			buffer8 = (unsigned char *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					buffer8[col] = ( data[col] - bzero ) / bscale;
			else
				for ( col = 0; col < naxis1; col++ )
					buffer8[col] = data[col];
			break;
		
		case 16:
			buffer16 = (signed short *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					buffer16[col] = ( data[col] - bzero ) / bscale;
			else
				for ( col = 0; col < naxis1; col++ )
					buffer16[col] = data[col];
			break;

		case 32:
			buffer32 = (signed long *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					buffer32[col] = ( data[col] - bzero ) / bscale;
			else
				for ( col = 0; col < naxis1; col++ )
					buffer32[col] = data[col];
			break;

		case -32:
			buffer32f = (float *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					buffer32f[col] = ( data[col] - bzero ) / bscale;
			else
				for ( col = 0; col < naxis1; col++ )
					buffer32f[col] = data[col];
			break;

		case -64:
			buffer64f = (DOUBLE *) buffer;
			if ( bzero != 0.0 || bscale != 1.0 )
				for ( col = 0; col < naxis1; col++ )
					buffer64f[col] = ( data[col] - bzero ) / bscale;
			else
				for ( col = 0; col < naxis1; col++ )
					buffer64f[col] = data[col];
			break;
	}

	/*** If necessary, byte-swap the raw image data. ***/

#if BYTESWAP
	ByteSwap ( buffer, naxis1, abs ( bitpix ) / 8 );
#endif
	
	/*** Write the image data into the output file; if successful,
	     set a successful result code. ***/

	if ( fwrite ( buffer, size, 1, file ) == 1 )
		result = TRUE;

	/*** Free the raw image data buffer; return
	     the result code. ***/

	free ( buffer );
	return ( result );
}

/********************  ReadFITSImageDataPadding  **********************/

int ReadFITSImageDataPadding ( FILE *file, long bitpix, long naxis,
long naxis1, long naxis2, long naxis3 )
{
	long	size;
	
	/*** Compute the number of bytes of image data contained in the file.
	     Then compute the number of bytes to the start of the next 2880-
	     byte block. ***/		

	size = abs ( bitpix ) / 8;

	if ( naxis >= 1 )
		size *= naxis1;
		
	if ( naxis >= 2 )
		size *= naxis2;
		
	if ( naxis >= 3 )
		size *= naxis3;
	
	size = 2880 * ( ( size + 2879 ) / 2880 ) - size;
	
	/*** Seek to the start of the next 2880-byte block; return TRUE if
	     successful or FALSE otherwise. ***/
	     
	if ( fseek ( file, size, SEEK_CUR ) == 0 )
		return ( TRUE );
	else
		return ( FALSE );
}

/********************  WriteFITSImageDataPadding  *********************/

int WriteFITSImageDataPadding ( FILE *file, long bitpix, long naxis,
long naxis1, long naxis2, long naxis3 )
{
	char	*buffer;
	int		result = FALSE;
	long	size;
	
	/*** Compute the number of bytes of image data contained in the file.
	     Then compute the number of bytes to the start of the next 2880-
	     byte block; this is the number of padding bytes we need to add. ***/		

	size = abs ( bitpix ) / 8;

	if ( naxis >= 1 )
		size *= naxis1;
		
	if ( naxis >= 2 )
		size *= naxis2;
		
	if ( naxis >= 3 )
		size *= naxis3;
	
	size = 2880 * ( ( size + 2879 ) / 2880 ) - size;
	
	/*** If we don't need to add any padding bytes, we're done. ***/
	
	if ( size == 0 )
		return ( TRUE );
	
	/*** If we do, allocate a buffer big enough to hold the padding;
	     zero-initialize it and write it to the file if successful. ***/
	     
	buffer = malloc ( size );
	if ( buffer != NULL )
	{
		memset ( buffer, 0, size );
		if ( fwrite ( buffer, size, 1, file ) == 1 )
			result = TRUE;
		else
			result = FALSE;
		free ( buffer );
	}
	
	return ( result );
}

/*************************  NewFITSImageDataMatrix  ****************************/

PIXEL ***NewFITSImageDataMatrix ( long naxis1, long naxis2, long naxis3 )
{
	PIXEL ***matrix;
	long j, k;
	
	matrix = (PIXEL ***) malloc ( sizeof ( PIXEL ** ) * ( naxis3 + 1 ) );
	if ( matrix == NULL )
		return ( NULL );
		
	for ( j = 0; j < naxis3; j++ )
	{
		matrix[j] = (PIXEL **) NCreateMatrix ( sizeof ( PIXEL ), naxis2, naxis1 );
		if ( matrix[j] == NULL )
		{
			for ( k = 0; k < j; k++ )
				NDestroyMatrix ( matrix[k] );
			free ( matrix );
			return ( NULL );
		}
	}
	
	matrix[naxis3] = NULL;
	return ( matrix );
}

/*************************  FreeFITSImageDataMatrix  ****************************/

void FreeFITSImageDataMatrix ( PIXEL ***matrix )
{
	long k;
	
	for ( k = 0; matrix[k] != NULL; k++ )
		NDestroyMatrix ( matrix[k] );
		
	free ( matrix );
}

/*************************  ReadFITSImageDataMatrix  *****************************/

int ReadFITSImageDataMatrix ( FILE *file, PIXEL ***matrix, long bitpix, long naxis,
long naxis1, long naxis2, long naxis3, double bzero, double bscale )
{
	long	i, j;
		
	for ( i = 0; i < naxis3; i++ )
		for ( j = 0; j < naxis2; j++ )
			if ( ReadFITSImageDataRow ( file, bitpix, naxis1, bzero, bscale, matrix[i][j] ) == FALSE )
				return ( FALSE );

	return ( TRUE );
}

/*************************  WriteFITSImageDataMatrix  ****************************/

int WriteFITSImageDataMatrix ( FILE *file, PIXEL ***matrix, long bitpix, long naxis,
long naxis1, long naxis2, long naxis3, double bzero, double bscale )
{
	int i, j;
	
	for ( i = 0; i < naxis3; i++ )
		for ( j = 0; j < naxis2; j++ )
			if ( WriteFITSImageDataRow ( file, bitpix, naxis1, bzero, bscale, matrix[i][j] ) == FALSE )
				return ( FALSE );
			
	return ( TRUE );
}

/****************************  SetFITSTableHeaderInfo  ******************************/

void SetFITSTableHeaderInfo ( char ***header, long naxis1, long naxis2, long tfields )
{
	int	n = 0;
	
	SetFITSHeaderLineKeywordString  ( header, n++, "XTENSION", "TABLE" );
	SetFITSHeaderLineKeywordInteger ( header, n++, "BITPIX", 8 );
	SetFITSHeaderLineKeywordInteger ( header, n++, "NAXIS", 2 );
	SetFITSHeaderLineKeywordInteger ( header, n++, "NAXIS1", naxis1 );
	SetFITSHeaderLineKeywordInteger ( header, n++, "NAXIS2", naxis2 );
	SetFITSHeaderLineKeywordInteger ( header, n++, "PCOUNT", 0 );
	SetFITSHeaderLineKeywordInteger ( header, n++, "GCOUNT", 1 );
	SetFITSHeaderLineKeywordInteger ( header, n++, "TFIELDS", tfields );

	SetFITSHeaderLineKeyword ( header, n, "END" );
}

/************************  GetFITSTableHeaderInfo  ***********************************/

void GetFITSTableHeaderInfo ( char **header, long *naxis1, long *naxis2, long *tfields )
{
	int		n;
	char	*line;
	
	for ( n = 0; ( line = GetFITSHeaderLine ( header, n ) ) != NULL; n++ )
	{
		if ( TestFITSHeaderKeyword ( line, "NAXIS1  " ) == TRUE )
			GetFITSHeaderInteger ( line, naxis1 );
			
		if ( TestFITSHeaderKeyword ( line, "NAXIS2  " ) == TRUE )
			GetFITSHeaderInteger ( line, naxis2 );

		if ( TestFITSHeaderKeyword ( line, "TFIELDS " ) == TRUE )
			GetFITSHeaderInteger ( line, tfields );
	}
}

/*************************  SetFITSTableHeaderFieldInfo  *****************************/

void SetFITSTableHeaderFieldInfo ( char ***header, long field, long tbcol, char *tform,
char *ttype, char *tunit, double tzero, double tscal, char *tnull )
{
	long	line = 0;
	char	keyword[9];
	
	if ( FindFITSHeaderKeyword ( *header, "END", &line ) == TRUE )
	{
		sprintf ( keyword, "TBCOL%-3hd", field );
		SetFITSHeaderLineKeywordInteger ( header, line++, keyword, tbcol );
		
		sprintf ( keyword, "TFORM%-3hd", field );
		SetFITSHeaderLineKeywordString  ( header, line++, keyword, tform );
		
		sprintf ( keyword, "TTYPE%-3hd", field );
		SetFITSHeaderLineKeywordString ( header, line++, keyword, ttype );
		
		sprintf ( keyword, "TUNIT%-3hd", field );
		SetFITSHeaderLineKeywordString ( header, line++, keyword, tunit );

		sprintf ( keyword, "TZERO%-3hd", field );
		SetFITSHeaderLineKeywordReal ( header, line++, keyword, tzero );
		
		sprintf ( keyword, "TSCAL%-3hd", field );
		SetFITSHeaderLineKeywordReal ( header, line++, keyword, tscal );

		sprintf ( keyword, "TNULL%-3hd", field );
		SetFITSHeaderLineKeywordString ( header, line++, keyword, tnull );

		SetFITSHeaderLineKeyword ( header, line, "END" );
	}
}

/****************************  GetFITSTableHeaderFieldInfo  ***************************/

void GetFITSTableHeaderFieldInfo ( char **header, long field, long *tbcol, char *tform,
char *ttype, char *tunit, double *tzero, double *tscal, char *tnull )
{
	char	tbcoln[9];
	char	tformn[9];
	char	ttypen[9];
	char	tunitn[9];
	char	tzeron[9];
	char	tscaln[9];
	char	tnulln[9];
	char	*line;
	long	n;
		
	sprintf ( tbcoln, "TBCOL%-3hd", field );
	sprintf ( tformn, "TFORM%-3hd", field );
	sprintf ( ttypen, "TTYPE%-3hd", field );
	sprintf ( tunitn, "TUNIT%-3hd", field );
	sprintf ( tzeron, "TZERO%-3hd", field );
	sprintf ( tscaln, "TSCAL%-3hd", field );
	sprintf ( tnulln, "TNULL%-3hd", field );
	
	for ( n = 0; ( line = GetFITSHeaderLine ( header, n ) ) != NULL; n++ )
	{
		if ( TestFITSHeaderKeyword ( line, tbcoln ) )
			GetFITSHeaderInteger ( line, tbcol );
		
		if ( TestFITSHeaderKeyword ( line, tformn ) )
			GetFITSHeaderString ( line, tform );

		if ( TestFITSHeaderKeyword ( line, ttypen ) )
			GetFITSHeaderString ( line, ttype );

		if ( TestFITSHeaderKeyword ( line, tunitn ) )
			GetFITSHeaderString ( line, tunit );

		if ( TestFITSHeaderKeyword ( line, tzeron ) )
			GetFITSHeaderReal ( line, tzero );

		if ( TestFITSHeaderKeyword ( line, tscaln ) )
			GetFITSHeaderReal ( line, tscal );

		if ( TestFITSHeaderKeyword ( line, tnulln ) )
			GetFITSHeaderString ( line, tnull );
	}
}

/*************************  ReadFITSTableDataRow  *********************/

int ReadFITSTableDataRow ( FILE *file, long naxis1, char *data )
{
	if ( fread ( data, naxis1, 1, file ) == 1 )
		return ( TRUE );
	else
		return ( FALSE );
}

/*************************  WriteFITSTableDataRow  *********************/

int WriteFITSTableDataRow ( FILE *file, long naxis1, char *data )
{
	if ( fread ( data, naxis1, 1, file ) == 1 )
		return ( TRUE );
	else
		return ( FALSE );
}

/*************************  ReadFITSTableDataPadding  *********************/

int ReadFITSTableDataPadding ( FILE *file, long naxis1, long naxis2 )
{
	long	size;
	
	/*** Compute the number of bytes of image data contained in the file.
	     Then compute the number of bytes to the start of the next 2880-
	     byte block. ***/		

	size = naxis1 * naxis2;
	size = 2880 * ( ( size + 2879 ) / 2880 ) - size;
	
	/*** Seek to the start of the next 2880-byte block; return TRUE if
	     successful or FALSE otherwise. ***/
	     
	if ( fseek ( file, size, SEEK_CUR ) == 0 )
		return ( TRUE );
	else
		return ( FALSE );
}

/*************************  WriteFITSTableDataPaddding  *********************/

int WriteFITSTableDataPadding ( FILE *file, long naxis1, long naxis2 )
{
	char	*buffer;
	long	size;
	int		result = TRUE;
	
	/*** Compute the number of bytes of image data contained in the file.
	     Then compute the number of bytes to the start of the next 2880-
	     byte block; this is the number of padding bytes we need to add. ***/		

	size = naxis1 * naxis2;
	size = 2880 * ( ( size + 2879 ) / 2880 ) - size;
	
	/*** If we don't need to add any padding bytes, we're done. ***/
	
	if ( size == 0 )
		return ( TRUE );
	
	/*** If we do, allocate a buffer big enough to hold the padding;
	     zero-initialize it and write it to the file if successful. ***/
	     
	buffer = malloc ( size );
	if ( buffer != NULL )
	{
		memset ( buffer, ' ', size );
		if ( fwrite ( buffer, size, 1, file ) == 1 )
			result = TRUE;
		else
			result = FALSE;
		free ( buffer );
	}
	
	return ( result );
}

/***********************  GetFITSTableDataFieldString  **********************/

void GetFITSTableDataFieldString ( char *data, long tbcol, char *tform, char *field )
{
	long n = atol ( &tform[1] );
	
	strncpy ( field, &data[ tbcol - 1 ], n );
}

/***********************  GetFITSTableDataFieldInteger  **********************/

void GetFITSTableDataFieldInteger ( char *data, long tbcol, char *tform,
double tzero, double tscal, long *value )
{
	char field[100] = { '\0' };
	
	GetFITSTableDataFieldString ( data, tbcol, tform, field );
	*value = atol ( field );
	
	if ( tzero != 0.0 || tscal != 1.0 )
		*value = *value * tscal + tzero;
}

/*************************  GetFITSTableDataFieldReal  ***********************/

void GetFITSTableDataFieldReal ( char *data, long tbcol, char *tform,
double tzero, double tscal, double *value )
{
	char field[100] = { '\0' };
	
	GetFITSTableDataFieldString ( data, tbcol, tform, field );
	*value = atof ( field );
	
	if ( tzero != 0.0 || tscal != 1.0 )
		*value = *value * tscal + tzero;
}

/**********************  SetFITSTableDataFieldString  *************************/

void SetFITSTableDataFieldString ( char *data, long tbcol, char *tform, char *field )
{
	long w = atol ( &tform[1] );
	
	memcpy ( &data[ tbcol - 1 ], field, w );
}

/***********************  SetFITSTableDataFieldInteger  **********************/

void SetFITSTableDataFieldInteger ( char *data, long tbcol, char *tform,
double tzero, double tscal, long value )
{
	long w = atol ( &tform[1] );
	char fmt[5] = { '\0' };
	char field[100] = { '\0' };
	
	if ( tzero != 0.0 || tscal != 1.0 )
		value = ( value - tzero ) / tscal;
		
	sprintf ( fmt, "%%%ldld", w );
	sprintf ( field, fmt, value );
	memcpy ( &data[ tbcol - 1 ], field, w );
}

/***********************  SetFITSTableDataFieldReal  **********************/

void SetFITSTableDataFieldReal ( char *data, long tbcol, char *tform,
double tzero, double tscal, double value )
{
	long w = atol ( &tform[1] );
	long d = atol ( &tform[4] );
	char fmt[8] = { '\0' };
	char field[100] = { '\0' };
	
	if ( tzero != 0.0 || tscal != 1.0 )
		value = ( value - tzero ) / tscal;

	if ( tform[0] == 'F' )
		sprintf ( fmt, "%%%ld.%ldf", w, d );
	else if ( tform[0] == 'E' )
		sprintf ( fmt, "%%%ld.%ldE", w, d );
	else if ( tform[0] == 'D' )
		sprintf ( fmt, "%%%ld.%ldE", w, d );

	sprintf ( field, fmt, value );
	memcpy ( &data[ tbcol - 1 ], field, w );
}

/*************************  NewFITSTableDataMatrix  ************************/

char **NewFITSTableDataMatrix ( long naxis1, long naxis2 )
{
	long	i, j;
	char	**matrix;
	
	if ( naxis1 == 0 || naxis2 == 0 )
		return ( NULL );
	
	matrix = (char **) malloc ( sizeof ( char * ) * ( naxis2 + 1 ) );
	if ( matrix == NULL )
		return ( NULL );
			
	for ( i = 0; i < naxis2; i++ )
	{
		matrix[i] = malloc ( sizeof ( char ) * naxis1 );
		if ( matrix[i] == NULL )
		{
			for ( j = 0; j < i; j++ )
				free ( matrix[j] );
			return ( NULL );
		}
		else
		{
			for ( j = 0; j < naxis1; j++ )
				matrix[i][j] = ' ';
		}
	}
	
	matrix[naxis2] = NULL;
	
	return ( matrix );
}

/*************************  FreeFITSTableDataMatrix  ************************/

void FreeFITSTableDataMatrix ( char **matrix )
{
	long i;
	
	for ( i = 0; matrix[i] != NULL; i++ )
		free ( matrix[i] );
		
	free ( matrix );
}

/*************************  ReadFITSTableDataMatrix  ************************/

int ReadFITSTableDataMatrix ( FILE *file, long naxis1, long naxis2, char **matrix )
{
	long i;
	
	for ( i = 0; i < naxis2; i++ )
		if ( ReadFITSTableDataRow ( file, naxis1, matrix[i] ) == FALSE )
			return ( FALSE );
			
	return ( TRUE );
}

/*************************  WriteFITSTableDataMatrix  ************************/

int WriteFITSTableDataMatrix ( FILE *file, long naxis1, long naxis2, char **matrix )
{
	long i;
	
	for ( i = 0; i < naxis2; i++ )
		if ( WriteFITSTableDataRow ( file, naxis1, matrix[i] ) == FALSE )
			return ( FALSE );
			
	return ( TRUE );
}

/**************************  NewFITSImageHeader  *****************************/

FITSImage *NewFITSImageHeader ( long bitpix, long naxis, long naxis1, long naxis2, long naxis3,
double bzero, double bscale )
{
	char		**header = NULL;
	FITSImage	*image = NULL;
	
	/*** Allocate memory for the FITS image record.  On failure, return NULL. ***/
	
	image = (FITSImage *) malloc ( sizeof ( FITSImage ) );
	if ( image == NULL )
		return ( NULL );
	
	/*** Allocate memory for the FITS's image file's raw header.
	     On failure, free all memory allocated this far, and return NULL. ***/
	     	
	if ( NewFITSHeader ( &header ) == FALSE )
	{
		free ( image );
		return ( NULL );
	}
	
	/*** Create raw FITS image header information in the new header matrix.
	     Then initialize the fields of the FITS image record.  Note that
	     we set the image's data matrix pointer to NULL, to indicate that
	     we have not allocated memory for it yet. ***/

	SetFITSImageHeaderInfo ( &header, bitpix, naxis, naxis1, naxis2, naxis3, bzero, bscale );
	
	image->bitpix = bitpix;
	image->naxis  = naxis;
	image->naxis1 = naxis1;
	image->naxis2 = naxis2;
	image->naxis3 = naxis3;
	image->bzero  = bzero;
	image->bscale = bscale;
	image->header = header;
	image->data   = NULL;
	
	/*** Return a pointer to the new FITS image record. ***/
	
	return ( image );
}

/******************************  NewFITSImage  ***********************************/

FITSImage *NewFITSImage ( long bitpix, long naxis, long naxis1, long naxis2, long naxis3,
double bzero, double bscale )
{
	char		**header = NULL;
	PIXEL		***data = NULL;
	FITSImage	*image = NULL;
	
	/*** Allocate memory for the FITS image record and its raw header.
	     On failure, return NULL. ***/
	
	image = NewFITSImageHeader ( bitpix, naxis, naxis1, naxis2, naxis3, bzero, bscale );
	if ( image == NULL )
		return ( NULL );

	/*** Allocate memory for the FITS image data matrix.  On failure,
	     free all memory allocated thus far, and return NULL. ***/

	data = NewFITSImageDataMatrix ( naxis1, naxis2, naxis3 );	
	if ( data == NULL )
	{
		FreeFITSImage ( image );
		return ( NULL );
	}

	/*** Store a pointer to the image data matrix in the image record,
	     and return a pointer to the image record. ***/
	     
	image->data = data;
	return ( image );
}

/*****************************  FreeFITSImage  ***********************************/

void FreeFITSImage ( FITSImage *image )
{
	if ( image->header != NULL )
		FreeFITSHeader ( image->header );
		
	if ( image->data != NULL )
		FreeFITSImageDataMatrix ( image->data );
		
	free ( image );
}

/************************  ReadFITSImageHeader  *******************************/

FITSImage *ReadFITSImageHeader ( FILE *file )
{
	long	bitpix = 8;
	long	naxis  = 0;
	long	naxis1 = 1;
	long	naxis2 = 1;
	long	naxis3 = 1;
	double	bzero  = 0.0;
	double	bscale = 1.0;
	char	**header = NULL;
	FITSImage *image = NULL;
	
	/*** Allocate memory for a new FITS image record.  On failure,
	     return an error code. ***/
	     
	image = (FITSImage *) malloc ( sizeof ( FITSImage ) );
	if ( image == NULL )
		return ( NULL );
		
	/*** Read the raw header from the FITS file.  On failure,
	     free memory allocated thus far and return an error code. ***/
	
	if ( ReadFITSHeader ( file, &header ) == FALSE )
	{
		free ( image );
		return ( NULL );
	}
	
	/*** Get FITS image parameters from the raw FITS image header.
	     Store them in the image record, then return a pointer to it.
	     Set the FITS image data matrix pointer to NULL to indicate
	     that we have not yet allocated one. ***/
	
	GetFITSImageHeaderInfo ( header, &bitpix, &naxis, &naxis1, &naxis2, &naxis3, &bzero, &bscale );
	
	image->bitpix = bitpix;
	image->naxis  = naxis;
	image->naxis1 = naxis1;
	image->naxis2 = naxis2;
	image->naxis3 = naxis3;
	image->bzero  = bzero;
	image->bscale = bscale;
	image->header = header;
	image->data = NULL;
	
	return ( image );
}

/**************************  ReadFITSImageData  **********************************/

PIXEL ***ReadFITSImageData ( FILE *file, FITSImage *image )
{
	long	bitpix = image->bitpix;
	long	naxis  = image->naxis;
	long	naxis1 = image->naxis1;
	long	naxis2 = image->naxis2;
	long	naxis3 = image->naxis3;
	double	bzero  = image->bzero;
	double	bscale = image->bscale;
	PIXEL	***matrix;
	
	matrix = NewFITSImageDataMatrix ( naxis1, naxis2, naxis3 );
	if ( matrix == NULL )
		return ( FALSE );

	if ( ReadFITSImageDataMatrix ( file, matrix, bitpix, naxis, naxis1, naxis2, naxis3, bzero, bscale ) == FALSE )
	{
		FreeFITSImageDataMatrix ( matrix );
		return ( FALSE );
	}
	
	image->data = matrix;
	
	return ( matrix );
}

/*****************************  ReadFITSImage  ***********************************/

FITSImage *ReadFITSImage ( FILE *file )
{
	PIXEL		***data;
	FITSImage	*image;

	/*** Allocate memory for a new FITS image record, and read the
	     file's FITS image header.  On failure, return an error code. ***/
		
	image = ReadFITSImageHeader ( file );
	if ( image == NULL )
		return ( NULL );
	
	/*** Allocate a new FITS image data matrix, and read the FITS image
	     data into it.  On failure, free the FITS image record and
	     return a NULL pointer.  If successful, store a pointer to the
	     new FITS image data matrix in the image record. ***/
	
	data = ReadFITSImageData ( file, image );
	if ( data == NULL )
	{
		FreeFITSImage ( image );
		return ( NULL );
	}
	
	image->data = data;

	/*** Read the padding after the FITS image data.  Note that we don't care
	     if this fails, so we don't check the return code.  Finally, we return
	     a pointer to the new image record. ***/

	ReadFITSImageDataPadding ( file, image->bitpix, image->naxis,
	image->naxis1, image->naxis2, image->naxis3 );
	     
	return ( image );
}

/****************************  WriteFITSImage  ********************************/

int WriteFITSImage ( FILE *file, FITSImage *image )
{
	long	bitpix   = image->bitpix;
	long	naxis    = image->naxis;
	long	naxis1   = image->naxis1;
	long	naxis2   = image->naxis2;
	long	naxis3   = image->naxis3;
	double	bzero    = image->bzero;
	double	bscale   = image->bscale;
	char	**header = image->header;
	PIXEL	***data  = image->data;
		
	if ( WriteFITSHeader ( file, header ) == FALSE )
		return ( FALSE );
		
	if ( WriteFITSImageDataMatrix ( file, data, bitpix, naxis, naxis1, naxis2, naxis3, bzero, bscale ) == FALSE )
		return ( FALSE );
	
	if ( WriteFITSImageDataPadding ( file, bitpix, naxis, naxis1, naxis2, naxis3 ) == FALSE )
		return ( FALSE );
					
	return ( TRUE );
}

/*** ResizeFITSImage ***********************************************************/
	
FITSImage *ResizeFITSImage ( FITSImage *oldFITS, short naxis1, short naxis2, short naxis3,
int copy )
{
	long			frame, col, row, line = 0;
	FITSImage		*newFITS = NULL;
	
	/*** Create a new FITS image record.  On failure, return a NULL pointer.
	     If successful, copy the data from the old FITS image record into the
	     new one verbatim.  We'll make copies of the individual subcomponents
	     later. ***/
	
	newFITS = malloc ( sizeof ( FITSImage ) );
	if ( newFITS == NULL )
		return ( NULL );
	
	*newFITS = *oldFITS;
	
	/*** Allocate a new matrix for the FITS image data.  On failure,
	     free the FITS image record, and return a NULL pointer. ***/
	
	newFITS->data = NewFITSImageDataMatrix ( naxis1, naxis2, naxis3 );
	if ( newFITS->data == NULL )
	{
		free ( newFITS );
		return ( NULL );
	}
	
	/*** Set the corresponding values in the new FITS image record. ***/
	
	newFITS->naxis1 = naxis1;
	newFITS->naxis2 = naxis2;
	newFITS->naxis3 = naxis3;

	/*** Copy the image data from the old FITS image data matrix to the new one,
	     initializing the new FITS data matrix to NULL where values have no
	     corresponding pixel in the previous matrix. ***/
	
	if ( copy )
	{
		for ( frame = 0; frame < newFITS->naxis3 && frame < oldFITS->naxis3; frame++ )
		{
			if ( frame < oldFITS->naxis3 )
			{
				for ( row = 0; row < newFITS->naxis2; row++ )
				{
					if ( row < newFITS->naxis2 )
					{
						for ( col = 0; col < newFITS->naxis1; col++ )
						{
							if ( col < oldFITS->naxis1 )
								newFITS->data[frame][row][col] = oldFITS->data[frame][row][col];
							else
								newFITS->data[frame][row][col] = 0;
						}
					}
					else
					{
						for ( col = 0; col < newFITS->naxis1; col++ )
							newFITS->data[frame][row][col] = 0;
					}
				}
			}
			else
			{
				for ( row = 0; row < newFITS->naxis2; row++ )
					for ( col = 0; col < newFITS->naxis1; col++ )
						newFITS->data[frame][row][col] = 0;
			}
		}
	}
	
	/*** Clone the old FITS image's header. ***/
	
	newFITS->header = CopyFITSHeader ( oldFITS->header, NULL );
	
	/*** Find the FITS header keywords which describe the dimensions
	     of the image data matrix, and change them to the new values. ***/
	
	if ( FindFITSHeaderKeyword ( newFITS->header, "NAXIS1", &line ) )
		SetFITSHeaderLineKeywordInteger ( &newFITS->header, line, "NAXIS1", naxis1 );
		
	if ( FindFITSHeaderKeyword ( newFITS->header, "NAXIS2", &line ) )
		SetFITSHeaderLineKeywordInteger ( &newFITS->header, line, "NAXIS2", naxis2 );
		
	if ( FindFITSHeaderKeyword ( newFITS->header, "NAXIS3", &line ) )
		SetFITSHeaderLineKeywordInteger ( &newFITS->header, line, "NAXIS3", naxis3 );

	/*** Return a pointer to the new FITS image record. ***/
	
	return ( newFITS );
}

/*************************  NewFITSTableHeader  ********************************/

FITSTable *NewFITSTableHeader ( long naxis1, long naxis2, long tfields )
{
	long			i;
	char			**header = NULL;
	char			**data = NULL;
	FITSTable		*table = NULL;
	FITSTableField	*fields = NULL;
	
	/**** Allocate memory for a new FITS table record.  On failure,
	      return an error code. ***/
	      
	table = (FITSTable *) malloc ( sizeof ( FITSTable ) );
	if ( table == NULL )
		return ( NULL );
	
	/*** Allocate memory for the FITS table's array of field descriptors.
	     On failure, free memory allocated thus far and return NULL. ***/
	     
	fields = (FITSTableField *) malloc ( sizeof ( FITSTableField ) * tfields );
	if ( fields == NULL )
	{
		free ( table );
		return ( NULL );
	}

	/*** Allocate a FITS header character matrix for the table's raw
	     FITS header.  On failure, free all memory allocated thus fat
	     and return an error code. ***/
	
	if ( NewFITSHeader ( &header ) == FALSE )
	{
		free ( fields );
		free ( table );
		return ( NULL );
	}
	
	/*** Create raw FITS table header information in the new header matrix.
	     Then initialize the fields of the FITS table record.  Note that
	     we set the table's data matrix pointer to NULL, to indicate that
	     we have not allocated memory for it yet. ***/
	     
	SetFITSTableHeaderInfo ( &header, naxis1, naxis2, tfields );
	
	table->naxis1 = naxis1;
	table->naxis2 = naxis2;
	table->tfields = tfields;
	table->fields = fields;
	table->header = header;
	table->data = NULL;

	/*** Initialize the array of table field descriptors with defaults. ***/
	
	for ( i = 0; i < tfields; i++ )
	{
		fields[i].tbcol    = 0;
		fields[i].tform[0] = '\0';
		fields[i].ttype[0] = '\0';
		fields[i].tunit[0] = '\0';
		fields[i].tnull[0] = '\0';
		fields[i].tzero    = 0.0;
		fields[i].tscal    = 1.0;
	}
	
	/*** Return a pointer to the FITS table record. ***/
	
	return ( table );
}

/*************************  NewFITSTable  ********************************/

FITSTable *NewFITSTable ( long naxis1, long naxis2, long tfields )
{
	char		**data = NULL;
	FITSTable	*table = NULL;
		
	/*** Allocate memory for a new FITS table record and the raw
	     FITS table header. ***/
		 
	table = NewFITSTableHeader ( naxis1, naxis2, tfields );
	if ( table == NULL )
		return ( NULL );
	
	/*** Allocate memory for a new FITS table data matrix.  On
	     failure, return an error code. ***/
	     
	data = NewFITSTableDataMatrix ( naxis1, naxis2 );
	if ( data == NULL )
	{
		FreeFITSTable ( table );
		return ( NULL );
	}
	
	/*** Store a pointer to the new FITS table data matrix in the table
	     record, then return a pointer to the new FITS table record. ***/
	     
	table->data = data;
	return ( table );
}

/***************************  FreeFITSTable  ********************************/

void FreeFITSTable ( FITSTable *table )
{
	if ( table->header != NULL )
		FreeFITSHeader ( table->header );
		
	if ( table->data != NULL )
		FreeFITSTableDataMatrix ( table->data );
		
	free ( table );
}

/**************************  SetFITSTableFieldInfo  *******************************/

void SetFITSTableFieldInfo ( FITSTable *table, long n, long tbcol, char *tform, char *ttype,
char *tunit, double tzero, double tscal, char *tnull )
{
	if ( n >= 1 && n <= table->tfields )
	{
		n--;
		
		table->fields[n].tbcol = tbcol;
		table->fields[n].tzero = tzero;
		table->fields[n].tscal = tscal;
		
		strncpy ( table->fields[n].tform, tform, 20 );
		strncpy ( table->fields[n].ttype, ttype, 20 );
		strncpy ( table->fields[n].tunit, tunit, 20 );
		strncpy ( table->fields[n].tnull, tnull, 20 );

		SetFITSTableHeaderFieldInfo ( &table->header, n + 1, tbcol, tform, ttype, tunit,
		tzero, tscal, tnull );
	}
}

/*************************  ReadFITSTableHeader  *********************************/

FITSTable *ReadFITSTableHeader ( FILE *file )
{
	long			n;
	long			naxis1 = 0;
	long			naxis2 = 0;
	long			tfields = 0;
	char			**header = NULL;
	FITSTable		*table = NULL;
	FITSTableField	*fields = NULL;
	
	/*** Read the raw FITS header; on failure, return FALSE. ***/
	
	if ( ReadFITSHeader ( file, &header ) == FALSE )
		return ( NULL );
	
	/*** Determine the number of rows, columns, and fields in the
	     table. ***/
	     
	GetFITSTableHeaderInfo ( header, &naxis1, &naxis2, &tfields );
	
	/*** Allocate memory for a new FITS table record, and for its
	     array of table field descriptors.  On failure, free any
	     memory allocated, and return NULL. ***/
	
	table = (FITSTable *) malloc ( sizeof ( FITSTable ) );
	if ( table == NULL )
		return ( NULL );
		
	fields = (FITSTableField *) malloc ( sizeof ( FITSTableField ) * tfields );
	if ( fields == NULL )
	{
		free ( table );
		return ( NULL );
	}
	
	/*** For each table field, set default values, then read field
	     information from the raw FITS table header. ***/
	     
	for ( n = 0; n < tfields; n++ )
	{
		fields[n].tbcol    = 0;
		fields[n].tform[0] = '\0';
		fields[n].ttype[0] = '\0';
		fields[n].tunit[0] = '\0';
		fields[n].tnull[0] = '\0';
		fields[n].tzero    = 0.0;
		fields[n].tscal    = 1.0;
		
		GetFITSTableHeaderFieldInfo ( header, n + 1, &fields[n].tbcol, fields[n].tform,
		fields[n].ttype, fields[n].tunit, &fields[n].tzero, &fields[n].tscal, fields[n].tnull );
	}
	
	/*** Return the FITS table header information. ***/
	
	table->naxis1  = naxis1;
	table->naxis2  = naxis2;
	table->tfields = tfields;
	table->fields  = fields;
	table->header  = header;
	table->data    = NULL;
	
	return ( table );
}

/**************************  ReadFITSTableData  **********************************/

char **ReadFITSTableData ( FILE *file, FITSTable *table )
{
	long	i;
	long	naxis1 = 0;
	long	naxis2 = 0;
	long	tfields = 0;
	char	**header = NULL;
	char	**data = NULL;
	
	/*** Allocate memory for a new FITS table data matrix.  On failure,
	     return an error code. ***/
	     
	data = NewFITSTableDataMatrix ( table->naxis1, table->naxis2 );
	if ( data == NULL )
		return ( NULL );

	/*** Read rows from the FITS table file into the data matrix.  On
	     failure, free the data matrix and return an error code. ***/
	
	for ( i = 0; i < table->naxis2; i++ )
	{
		if ( ReadFITSTableDataRow ( file, table->naxis1, data[i] ) == FALSE )
		{
			FreeFITSTableDataMatrix ( data );
			return ( NULL );
		}
	}
	
	/*** Return a pointer to the new table data matrix. ***/
	     	
	return ( data );
}

/**************************  ReadFITSTable  **********************************/

FITSTable *ReadFITSTable ( FILE *file )
{
	FITSTable	*table;
	char		**data;
	
	/*** Allocate a new FITS table record and read the file's FITS
	     table header information. ***/
	     
	table = ReadFITSTableHeader ( file );
	if ( table == NULL )
		return ( NULL );
	
	/*** Allocate memory for the FITS table data and read it from
	     the FITS file.  On failure, free memory allocated thus far
	     and return NULL.  If successful, store a pointer to the
	     new FITS table data matrix in the table record. ***/
	  
	data = ReadFITSTableData ( file, table );  	
	if ( data == NULL )
	{
		FreeFITSTable ( table );
		return ( NULL );
	}
	
	table->data = data;
	
	/*** Read padding after the FITS table data; we don't care if this
	     fails, so no need to check return code.  Return a pointer to
	     the new FITS table record. ***/
	     
	ReadFITSTableDataPadding ( file, table->naxis1, table->naxis2 );
	
	return ( table );
}

/**************************  WriteFITSTable  **********************************/

int WriteFITSTable ( FILE *file, FITSTable *table )
{
	long	naxis1   = table->naxis1;
	long	naxis2   = table->naxis2;
	char	**header = table->header;
	char	**matrix = table->data;
		
	if ( WriteFITSHeader ( file, header ) == FALSE )
		return ( FALSE );
		
	if ( WriteFITSTableDataMatrix ( file, naxis1, naxis2, matrix ) == FALSE )
		return ( FALSE );
	
	if ( WriteFITSTableDataPadding ( file, naxis1, naxis2 ) == FALSE )
		return ( FALSE );
		
	return ( TRUE );
}

/************************  GetFITSTableFieldString  *****************************/

void GetFITSTableFieldString ( FITSTable *table, char *row, long n, char *value )
{
	long	tbcol;
	char	*tform;

	if ( n >= 1 && n <= table->tfields )
	{
		n = n - 1;
		
		tform = table->fields[ n ].tform;
		tbcol = table->fields[ n ].tbcol;

		GetFITSTableDataFieldString ( row, tbcol, tform, value );
	}
}

/************************  GetFITSTableFieldInteger  *****************************/

void GetFITSTableFieldInteger ( FITSTable *table, char *row, long n, long *value )
{
	char	*tform;
	long	tbcol;
	double	tzero, tscal;

	if ( n >= 1 && n <= table->tfields )
	{
		n = n - 1;
		
		tform = table->fields[ n ].tform;
		tbcol = table->fields[ n ].tbcol;
		tzero = table->fields[ n ].tzero;
		tscal = table->fields[ n ].tscal;
		
		GetFITSTableDataFieldInteger ( row, tbcol, tform, tzero, tscal, value );
	}		
}

/**************************  GetFITSTableFieldReal  *******************************/

void GetFITSTableFieldReal ( FITSTable *table, char *row, long n, double *value )
{
	char	*tform;
	long	tbcol;
	double	tzero, tscal;
	
	if ( n >= 1 && n <= table->tfields )
	{
		n = n - 1;
	
		tform = table->fields[ n ].tform;
		tbcol = table->fields[ n ].tbcol;
		tzero = table->fields[ n ].tzero;
		tscal = table->fields[ n ].tscal;
		
		GetFITSTableDataFieldReal ( row, tbcol, tform, tzero, tscal, value );
	}
}

/************************  SetFITSTableFieldString  *****************************/

void SetFITSTableFieldString ( FITSTable *table, char *row, long n, char *value )
{
	long	tbcol;
	char	*tform;

	if ( n >= 1 && n <= table->tfields )
	{
		n = n - 1;
		
		tform = table->fields[ n ].tform;
		tbcol = table->fields[ n ].tbcol;

		GetFITSTableDataFieldString ( row, tbcol, tform, value );
	}
}

/************************  SetFITSTableFieldInteger  *****************************/

void SetFITSTableFieldInteger ( FITSTable *table, char *row, long n, long value )
{
	char	*tform;
	long	tbcol;
	double	tzero, tscal;

	if ( n >= 1 && n <= table->tfields )
	{
		n = n - 1;
		
		tform = table->fields[ n ].tform;
		tbcol = table->fields[ n ].tbcol;
		tzero = table->fields[ n ].tzero;
		tscal = table->fields[ n ].tscal;
		
		SetFITSTableDataFieldInteger ( row, tbcol, tform, tzero, tscal, value );
	}		
}

/**************************  SetFITSTableFieldReal  *******************************/

void SetFITSTableFieldReal ( FITSTable *table, char *row, long n, double value )
{
	char	*tform;
	long	tbcol;
	double	tzero, tscal;
	
	if ( n >= 1 && n <= table->tfields )
	{
		n = n - 1;
	
		tform = table->fields[ n ].tform;
		tbcol = table->fields[ n ].tbcol;
		tzero = table->fields[ n ].tzero;
		tscal = table->fields[ n ].tscal;
		
		SetFITSTableDataFieldReal ( row, tbcol, tform, tzero, tscal, value );
	}
}
