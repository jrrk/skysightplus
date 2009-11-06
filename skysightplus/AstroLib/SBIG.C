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

int ReadSBIGImageHeader ( FILE *file, SBIGInfo *info )
{
	char	buffer[2048] = { '\0' };
	char	*keyword, *value;
	int		i, k, start;
	
	if ( fread ( buffer, 2048, 1, file ) != 1 )
		return ( FALSE );
		
	for ( keyword = buffer, start = 0, i = 1; i < 2048; i++ )
	{
		if ( buffer[i] == '\r' && buffer[i - 1] == '\n' )
		{
			buffer[i - 1] = '\0';
			
			for ( value = buffer + start; *value != '\0'; value++ )
			{
				if ( *value == '=' )
				{
					*( value - 1 ) = '\0';
					value += 2;
					break;
				}	
			}
			
			if ( strcmp ( keyword, "File_version" ) == 0 )
				info->file_version = atoi ( value );
				
			if ( strcmp ( keyword, "Data_version" ) == 0 )
				info->data_version = atoi ( value );
				
			if ( strcmp ( keyword, "Exposure" ) == 0 )
				info->exposure = atof ( value ) / 100.0;
			
			if ( strcmp ( keyword, "Focal_length" ) == 0 )
				info->focal_length = atof ( value );
				
			if ( strcmp ( keyword, "Aperture" ) == 0 )
				info->aperture = atof ( value );
			
			if ( strcmp ( keyword, "Response_factor" ) == 0 )
				info->response_factor = atof ( value );

			if ( strcmp ( keyword, "Background" ) == 0 )
				info->background = atol ( value );

			if ( strcmp ( keyword, "Range" ) == 0 )
				info->range = atol ( value );

			if ( strcmp ( keyword, "Height" ) == 0 )
				info->height = atoi ( value );
				
			if ( strcmp ( keyword, "Width" ) == 0 )
				info->width = atoi ( value );
			
			if ( strcmp ( keyword, "Date" ) == 0 )
			{
				sscanf ( value, "%hd/%hd/%hd", &info->date_month, &info->date_day, &info->date_year );
				if ( info->date_year > 50 )
					info->date_year += 1900;
				else
					info->date_year += 2000;
			}
			
			if ( strcmp ( keyword, "Time" ) == 0 )
				sscanf ( value, "%hd:%hd:%hd", &info->time_hour, &info->time_min, &info->time_sec );
			
			if ( strcmp ( keyword, "Exposure_state" ) == 0 )
				info->exposure_state = atol ( value );
			
			if ( strcmp ( keyword, "Temperature" ) == 0 )
				info->temperature = atof ( value );
				
			if ( strcmp ( keyword, "Number_exposures" ) == 0 )
				info->number_exposures = atoi ( value );
						
			if ( strcmp ( keyword, "Each_exposure" ) == 0 )
				info->number_exposures = atof ( value ) / 100.0;

			if ( strcmp ( keyword, "X_pixel_size" ) == 0 )
				info->x_pixel_size = atof ( value ) / 100.0;
				
			if ( strcmp ( keyword, "Y_pixel_size" ) == 0 )
				info->y_pixel_size = atof ( value ) / 100.0;
								
			if ( strcmp ( keyword, "Pedestal" ) == 0 )
				info->pedestal = atoi ( value );
			
			if ( strcmp ( keyword, "E_gain" ) == 0 )
				info->e_gain = atof ( value );

			if ( strcmp ( keyword, "End" ) == 0 )
				break;
								
			start = i + 1;
			keyword = buffer + start;
		}
	}
	
	return ( TRUE );
}

#if BYTESWAP
#define GetSBIGPixelValue(buffer,k) \
	(unsigned short) ( ( (unsigned char) buffer[k] ) << 8 | (unsigned char) buffer[k+1] )
#else
#define GetSBIGPixelValue(buffer,k) \
	(unsigned short) ( (unsigned char) buffer[k] | ( (unsigned char) buffer[k+1] ) << 8 )
#endif	

int ReadSBIGImageDataRow ( FILE *file, short ncols, PIXEL *data )
{
	signed char		*databuf, lenbuf[2];
	unsigned short	j, k, length, pixel;

	/*** Read the number of bytes of data occupied by the
	     image data row. ***/
		
	if ( fread ( &lenbuf, 2, 1, file ) != 1 )
		return ( FALSE );
	
	length = GetSBIGPixelValue ( lenbuf, 0 );
	
	/*** Allocate enough memory to store the raw (i.e. compressed)
	     image data row.  On failure, return. ***/
	
	databuf = (signed char *) malloc ( sizeof ( signed char ) * length );
	if ( databuf == NULL )
		return ( FALSE );
	
	/*** Read the image data row into the data buffer.  On failure,
	     free the buffer and return. ***/
	     
	if ( fread ( databuf, length, 1, file ) != 1 )
	{
		free ( databuf );
		return ( FALSE );
	}
	
	/*** If the length of the image data row indicates that the image
	     data is uncompressed, copy it into the output image data
	     row buffer directly.  Otherwise, copy it using the SBIG
	     compression algorithm. ***/
	
	if ( length == ncols * sizeof ( short ) )
	{
		for ( k = 0, j = 0; j < ncols; j++, k+= 2 )
			data[j] = GetSBIGPixelValue ( databuf, k );
	}
	else
	{
		/*** Read the value of the first pixel from the first two
		     bytes in the image data row. ***/
		
		pixel = GetSBIGPixelValue ( databuf, 0 );
			
		for ( k = 2, j = 0; j < ncols; j++ )
		{
			data[j] = pixel;
			
			if ( databuf[k] == -128 )
			{
				pixel = GetSBIGPixelValue ( databuf, k + 1 );
				k += 3;
			}
			else
			{
				pixel += databuf[k];
				k++;
			}
		}
	}
	
	/*** Free the temporary SBIG image data buffer and return a
	     successful result code. ***/
	     
	free ( databuf );
	return ( TRUE );
}
