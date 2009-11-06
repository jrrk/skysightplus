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

/***********************  GetGSCRegionIndexFilePath  ***************************/

void GetGSCRegionIndexFilePath ( char *separator, char *volume, char *directory,
char *filename )
{
	/*** Write strings containing the volume name, directory containing
	     the region index file, and the region index file name. ***/
	     
	sprintf ( volume, "USA_AURA_STSI_GSC1_1001" );
	sprintf ( directory, "%sTABLES%s", separator, separator );
	sprintf ( filename, "REGIONS.TBL" );
}

/**********************  ReadGSCRegionIndexFileHeader  *************************/

FITSTable *ReadGSCRegionIndexFileHeader ( FILE *file )
{
	char		**header = NULL;
	FITSTable	*table = NULL;
	
	/*** Read the GSC region index file's FITS image header.
	     On failure, return FALSE.  Discard the header, if succcesful. ***/
	     
	if ( ReadFITSHeader ( file, &header ) == FALSE )
		return ( FALSE );
		
	FreeFITSHeader ( header );
	
	/*** Now read the file's FITS table header.  Return a pointer
	     to a FITS table struct, if successful. ***/
	
	table = ReadFITSTableHeader ( file );
	
	return ( table );
}

/***********************  ReadGSCRegionIndexFileRecord  *************************/

int ReadGSCRegionIndexFileRecord ( FILE *file, FITSTable *table, GSCRegion *region )
{
	char	*row = NULL;
	float	temp;
	long	naxis1 = table->naxis1;
	long	rgn_no = 0;
	long	ra_h_lo = 0;
	long	ra_h_hi = 0;
	long	ra_m_lo = 0;
	long	ra_m_hi = 0;
	double	ra_s_lo = 0.0;
	double	ra_s_hi = 0.0;
	char	dec_si_lo[2] = { '+' };
	char	dec_si_hi[2] = { '+' };
	long	dec_d_lo = 0;
	long	dec_d_hi = 0;
	long	dec_m_lo = 0;
	long	dec_m_hi = 0;
	double	dec_s_lo = 0.0;
	double	dec_s_hi = 0.0;

	/*** Allocate memory for a buffer to hold a row of FITS table data.
	     On failure, return FALSE. ***/
	
	row = malloc ( naxis1 );
	if ( row == NULL )
		return ( FALSE );
		
	/*** Read one row from the GSC region file into the buffer. ***/

	if ( ReadFITSTableDataRow ( file, naxis1, row ) == FALSE )
	{
		free ( row );
		return ( FALSE );
	}

	/*** Parse fields from the table and copy them into the GSC record struct. ***/
	
	GetFITSTableFieldInteger ( table, row, 1, &rgn_no );

	GetFITSTableFieldInteger ( table, row, 2, &ra_h_lo );
	GetFITSTableFieldInteger ( table, row, 3, &ra_m_lo );
	GetFITSTableFieldReal    ( table, row, 4, &ra_s_lo );
	
	GetFITSTableFieldInteger ( table, row, 5, &ra_h_hi );
	GetFITSTableFieldInteger ( table, row, 6, &ra_m_hi );
	GetFITSTableFieldReal    ( table, row, 7, &ra_s_hi );

	GetFITSTableFieldString  ( table, row, 8, dec_si_lo );
	GetFITSTableFieldInteger ( table, row, 9, &dec_d_lo );
	GetFITSTableFieldInteger ( table, row, 10, &dec_m_lo );

	GetFITSTableFieldString  ( table, row, 11, dec_si_hi );
	GetFITSTableFieldInteger ( table, row, 12, &dec_d_hi );
	GetFITSTableFieldInteger ( table, row, 13, &dec_m_hi );
	
	region->rgn_no = rgn_no;
	region->ra_lo  = DegMinSecToDecimal ( ra_h_lo, ra_m_lo, ra_s_lo, '+' ) * 15.0;
	region->ra_hi  = DegMinSecToDecimal ( ra_h_hi, ra_m_hi, ra_s_hi, '+' ) * 15.0;
	region->dec_lo = DegMinSecToDecimal ( dec_d_lo, dec_m_lo, 0.0, dec_si_lo[0] );
	region->dec_hi = DegMinSecToDecimal ( dec_d_hi, dec_m_hi, 0.0, dec_si_hi[0] );
	
	/*** If necessary, swap the values of the low and high declination
	     coordinate boundaries. ***/
	     
	if ( region->dec_lo > region->dec_hi )
	{
		temp = region->dec_lo;
		region->dec_lo = region->dec_hi;
		region->dec_hi = temp;
	}
	
	/*** Free the raw table data row, and return a successful result code. ***/
	
	free ( row );	
	return ( TRUE );
}

/***********************  ReadGSCRegionIndexFile  ********************************/

GSCRegion **ReadGSCRegionIndexFile ( FILE *file )
{
	long		i, k, n;
	FITSTable	*table;
	GSCRegion	**regions;
	
	/*** Read GSC region index file header. ***/
	
	table = ReadGSCRegionIndexFileHeader ( file );
	if ( table == NULL )
	{
		return ( NULL );
	}
	
	/*** Determine the number of GSC region files from the number of
	     lines in the FITS table.  Then allocate storage for array of
	     pointers to GSC region records. ***/
	
	n = table->naxis2;

	regions = (GSCRegion **) malloc ( sizeof ( GSCRegion * ) * ( n + 1 ) );
	if ( regions == NULL )
	{
		FreeFITSTable ( table );
		return ( NULL );
	}
	
	/*** Now allocate memory for individual GSC region records
	     themselves.  On error, free all memory allocated thus
	     far, and return NULL to signal failure. ***/
	
	for ( i = 0; i < n; i++ )
	{
		regions[i] = (GSCRegion *) malloc ( sizeof ( GSCRegion ) );
		if ( regions[i] == NULL )
		{
			FreeFITSTable ( table );
			for ( k = 0; k < i; k++ )
				free ( regions[k] );
			free ( regions );
			return ( NULL );
		}
	}
	
	/*** Set the last region pointer to NULL to indicate the
	     end of the array. ***/
	     
	regions[n] = NULL;

	/*** Read records from GSC region index file. ***/
	
	for ( i = 0; i < n; i++ )
	{
		if ( ReadGSCRegionIndexFileRecord ( file, table, regions[i] ) == FALSE )
		{
			FreeGSCRegionIndex ( regions );
			FreeFITSTable ( table );
			return ( NULL );
		}
	}

	/*** Free memory for the FITS table descriptor, and return
	     a pointer to the GSC region index array. ***/
	     
	FreeFITSTable ( table );
	return ( regions );
}

/**************************  FreeGSCRegionIndex  ****************************/

void FreeGSCRegionIndex ( GSCRegion **regions )
{
	long i;
	
	/*** Free memory for the individual GSC region records; then free
	     memory for the index array of region record pointers. ***/
	     
	for ( i = 0; regions[i] != NULL; i++ )
		free ( regions[i] );
		
	free ( regions );
}

/**************************  GetGSCRegionFilePath  ****************************/

void GetGSCRegionFilePath ( GSCRegion *region, char *separator,
char *volume, char *directory, char *filename )
{
	int			band, disk;
	static char	*gscsubdirs[] =
	{
	"S8230", "S7500", "S6730", "S6000", "S5230", "S4500", 
	"S3730", "S3000", "S2230", "S1500", "S0730", "S0000",
	"N0000", "N0730", "N1500", "N2230", "N3000", "N3730",
	"N4500", "N5230", "N6000", "N6730", "N7500", "N8230"
	};
	
	/*** Compute the declination band in which the region file
	     resides, and the disk on which regions on that band are
	     stored. ***/
	
	band = ( region->dec_lo + region->dec_hi + 180.0 ) / 15.0;
	
	if ( band >= 11 )
		disk = 1;
	else
		disk = 2;
		
	/*** Write strings containing the volume name, directory containing
	     the region file name, and the region file name itself. ***/
	     
	sprintf ( volume, "USA_AURA_STSI_GSC1_100%d", disk );
	sprintf ( directory, "%sGSC%s%s%s", separator, separator, gscsubdirs[band], separator );
	sprintf ( filename, "%04d.GSC", (int) region->rgn_no );
}

/************************  ReadGSCRegionFileHeader  ****************************/

FITSTable *ReadGSCRegionFileHeader ( FILE *file )
{
	char		**header = NULL;
	FITSTable	*table = NULL;
	
	/*** Read the GSC region file's FITS image header.  On failure,
	     return FALSE.  Discard the header, if succcesful. ***/
	     
	if ( ReadFITSHeader ( file, &header ) == FALSE )
		return ( FALSE );
		
	FreeFITSHeader ( header );
	
	/*** Now read the file's FITS table header.  Return a pointer
	     to a FITS table struct, if successful. ***/
	
	table = ReadFITSTableHeader ( file );
	
	return ( table );
}

/************************  ReadGSCRegionFileRecord  **************************/

int ReadGSCRegionFileRecord ( FILE *file, FITSTable *table, GSCRecord *record )
{
	long		naxis1 = table->naxis1;
	long		gsc_id = 0;
	double		ra = 0.0;
	double		dec = 0.0;
	double		pos_err = 0.0;
	double		mag = 0.0;
	double		mag_err = 0.0;
	long		mag_band = 0;
	long		classification = 0;
	char		plate_id[5] = { '\0' };
	char		multiple[2] = { '\0' };
	char		*row = NULL;
	
	/*** Allocate memory for a buffer to hold a row of FITS table data.
	     On failure, return FALSE. ***/
	
	row = malloc ( naxis1 );
	if ( row == NULL )
		return ( FALSE );
		
	/*** Read one row from the GSC region file into the buffer. ***/

	if ( ReadFITSTableDataRow ( file, naxis1, row ) == FALSE )
	{
		free ( row );
		return ( FALSE );
	}

	/*** Parse fields from the table and copy them into the GSC record struct. ***/
	
	GetFITSTableFieldInteger ( table, row, 1, &gsc_id );
	GetFITSTableFieldReal ( table, row, 2, &ra );
	GetFITSTableFieldReal ( table, row, 3, &dec );
	GetFITSTableFieldReal ( table, row, 4, &pos_err );
	GetFITSTableFieldReal ( table, row, 5, &mag );
	GetFITSTableFieldReal ( table, row, 6, &mag_err );
	GetFITSTableFieldInteger ( table, row, 7, &mag_band );
	GetFITSTableFieldInteger ( table, row, 8, &classification );
	GetFITSTableFieldString ( table, row, 9, record->plate_id );
	GetFITSTableFieldString ( table, row, 10, multiple );
	
	record->gsc_id = gsc_id;
	record->ra = ra;
	record->dec = dec;
	record->pos_err = pos_err;
	record->mag = mag;
	record->mag_err = mag_err;
	record->mag_band = mag_band;
	record->classification = classification;
	
	if ( multiple[0] == 'T' )
		record->multiple = TRUE;
	else
		record->multiple = FALSE;
	
	/*** Free the raw table data row, and return a successful result code. ***/
	
	free ( row );	
	return ( TRUE );
}

/************************ ReadGSCRegionFileObject  ***************************/

int ReadGSCRegionFileObject ( FILE *file, FITSTable *header, GSCRecord *object )
{
	int			result;
	long		offset, n = 1;
    GSCRecord	record;

    /*** Read one GSC record from the region file.  Then record the
         file position. ***/
    
	result = ReadGSCRegionFileRecord ( file, header, object );
    if ( result == FALSE )
		return ( FALSE );

    /*** If the GSC entry is not multiple, or is blank, don't do anything
         more with it. ***/
          
    if ( object->multiple == FALSE || object->gsc_id == 0 )
    	return ( TRUE );
        
    /*** Read multiple entries until we come to one with a different
         GSC ID number, or to the end of the file. ***/

    while ( TRUE )
    {
		/*** Save the current file mark, then read a new GSC record. ***/
        
		offset = ftell ( file );
		if ( ReadGSCRegionFileRecord ( file, header, &record ) == FALSE )
        	break;

        /*** If the new record has a different ID number from the one
             we started with, rewind to the start of the record and return. ***/

        if ( record.gsc_id != object->gsc_id )
        {
            fseek ( file, offset, SEEK_SET );
			break;
		}

        /*** Otherwise, add the record's position, magnitude, and errors
             into the object's averages. ***/

        object->ra += record.ra;
        object->dec += record.dec;
        object->mag += record.mag;
        object->pos_err += record.pos_err;
        object->mag_err += record.mag_err;
        n++;
     }

    /*** Average the object's position, magnitude, and errors. ***/
    
    object->ra /= n;
    object->dec /= n;
    object->mag /= n;
    object->pos_err /= n;
    object->mag_err /= n;

    return ( TRUE );
}

/************************  FindGSCRegion  **************************/

int FindGSCRegion ( GSCRegion **regions, double ra_lo, double dec_lo,
double ra_hi, double dec_hi, GSCRegion **region )
{
	long i = 0;
	
	/*** If we are given a valid region to begin searching from,
	     find that region in the region index, and if successful,
	     start searching with the following one. ***/
	     
	if ( *region != NULL )
	{
		for ( i = 0; regions[i] != NULL; i++ )
		{
			if ( regions[i] == *region )
			{
				i++;
				break;
			}
		}
	}
	
	/*** Search through the region index.  If we find one whose boundaries
	     intersect the specified range, save a pointer to it and stop
	     searching. ***/
	
	for ( i = i; regions[i] != NULL; i++ )
	{
		if ( TestGSCRegion ( regions[i], ra_lo, dec_lo, ra_hi, dec_hi ) == TRUE )
		{
			*region = regions[i];
			break;
		}
	}
	
	/*** If we did not find any more regions in the specified range, set the
	     returned region pointer to NULL, and return FALSE.  Otherwise return
	     TRUE, along with a pointer to the region record that was found. ***/
	
	if ( regions[i] == NULL )
		*region = NULL;

	if ( *region == NULL )
		return ( FALSE );
	else
		return ( TRUE );
}

/************************  TestGSCRegion  **************************/

int TestGSCRegion ( GSCRegion *region, double ra_lo, double dec_lo,
double ra_hi, double dec_hi )
{
	int		result = FALSE;
	
	/*** Test whether or not the coordinates of the specified region overlap
	     those of the area we are testing.  If either set of coordinates
	     wraps around the zero R.A. meridian, add a full circle to it so
	     we can do a valid comparison. ***/
	
	if ( region->dec_hi >= dec_lo && region->dec_lo < dec_hi )
	{
		double region_ra_hi = region->ra_hi;
		double region_ra_lo = region->ra_lo;
		
		if ( region_ra_hi < region_ra_lo )
			region_ra_hi += 360.0;
			
		if ( ra_hi < ra_lo )
		{
			ra_hi += 360.0;
			
			if ( region_ra_hi < 180.0 )
				region_ra_hi += 360.0;
			
			if ( region_ra_lo < 180.0 )
				region_ra_lo += 360.0;
		}

		result = region_ra_hi >= ra_lo && region_ra_lo < ra_hi;
	}
	
	return ( result );
}
