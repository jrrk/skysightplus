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

/*** private data types ***/

struct Exposure
{
	short			iBinning;
	short			iShutter;
	short			iFilter;
	float			fLength;
	ExposurePtr		pNextExposure;
};

/*** NewExposure **********************************************************************

	Allocates a new exposure record and appends it to an existing linked list of
	exposure records.
	
***************************************************************************************/

ExposurePtr NewExposure ( ExposureList *pExposureList, short iBinning, short iShutter,
short iFilter, float fLength )
{
	ExposurePtr		pExposure, pPrevExposure;
	
	/*** Allocate memory for a new exposure record.  On failure, return NULL. ***/
	
	pExposure = (ExposurePtr) calloc ( 1, sizeof ( Exposure ) );
	if ( pExposure == NULL )
		return ( NULL );
	
	/*** Copy the exposure parameters into the newly-allocated exposure record. ***/
		
	pExposure->iBinning = iBinning;
	pExposure->iShutter = iShutter;
	pExposure->iFilter = iFilter;
	pExposure->fLength = fLength;
	pExposure->pNextExposure = NULL;
	
	/*** If there are no exposure records in the exposure list we've been given,
	     make the new exposure record the start of the list.  Otherwise, append
	     the new exposure record to the last record already in the list. ***/
	     
	if ( (*pExposureList) == NULL )
	{
		(*pExposureList) = pExposure;
	}
	else
	{
		pPrevExposure = (*pExposureList);
		while ( pPrevExposure->pNextExposure != NULL )
			pPrevExposure = pPrevExposure->pNextExposure;
		pPrevExposure->pNextExposure = pExposure;
	}
	
	/*** Return a pointer to the new exposure record we just created. ***/
	
	return ( pExposure );
}

/*** DeleteExposure *****************************************************************

	Deallocates memory associated with a single exposure record.
	
*************************************************************************************/

void DeleteExposure ( ExposurePtr pExposure )
{
	free ( pExposure );
}

/*** DeleteExposureList *************************************************************

	Deallocates memory for an entire linked list of exposure records.
	
*************************************************************************************/

void DeleteExposureList ( ExposureList pExposureList )
{
	ExposurePtr		pExposure, pNextExposure;
	
	for ( pExposure = pExposureList; pExposure != NULL; pExposure = pNextExposure )
	{
		pNextExposure = pExposure->pNextExposure;
		DeleteExposure ( pExposure );
	}
}

/*** GetExposureBinning ******************************************************************

	Obtains an exposure's binning mode.
	
*****************************************************************************************/

short GetExposureBinning ( ExposurePtr pExposure )
{
	return ( pExposure->iBinning );
}

/*** GetExposureShutter *************************************************************

	Obtains an exposure's shutter mode.
	
*****************************************************************************************/

short GetExposureShutter ( ExposurePtr pExposure )
{
	return ( pExposure->iShutter );
}

/*** GetExposureFilter ******************************************************************

	Obtains an exposure's filter position.
	
*****************************************************************************************/

short GetExposureFilter ( ExposurePtr pExposure )
{
	return ( pExposure->iFilter );
}

/*** GetExposureLength ******************************************************************

	Obtains an exposure's length, in seconds.
	
*****************************************************************************************/

float GetExposureLength ( ExposurePtr pExposure )
{
	return ( pExposure->fLength );
}

/*** GetNextExposure ********************************************************************

	Returns a pointer to the next record in a linked list of exposure records.
	
*****************************************************************************************/

ExposurePtr GetNextExposure ( ExposurePtr pExposure )
{
	return ( pExposure->pNextExposure );
}

/*

ExposureListPtr CreateImageExposureList ( float fLength, short iFilter )
{
	short				iDarkMode = GetCameraDarkFrameMode()
	ExposureListPtr		pExposureList = NULL;
	
	if ( iDarkMode == DARK_FRAME_ALWAYS || iDarkMode == DARK_FRAME_ONLY )
		NewExposure ( &pExposureList, SHUTTER_CLOSED, FILTER_DARK, fLength )
		
	if ( iDarkMode != DARK_FRAME_ONLY )
		NewExposure ( &pExposureList, SHUTTER_OPEN, iFilter );

	return ( pExposureList );		
}

ExposureListPtr CreateFocusExposureList ( float fLength, short iFilter )
{
	short				iDarkMode = GetCameraDarkFrameMode()
	ExposureListPtr		pExposureList = NULL;
	
	if ( iDarkMode == DARK_FRAME_ALWAYS || iDarkMode == DARK_FRAME_ONLY )
		NewExposure ( &pExposureList, SHUTTER_CLOSED, FILTER_DARK, fLength )
		
	if ( iDarkMode != DARK_FRAME_ONLY )
		NewExposure ( &pExposureList, SHUTTER_OPEN, iFilter );

	return ( pExposureList );		
}

*/
