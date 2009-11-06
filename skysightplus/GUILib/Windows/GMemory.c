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

/*******************************  GNewPtr  *******************************/

GPtr GNewPtr ( long size )
{
	return ( malloc ( size ) );
}

/*******************************  GNewHandle  *****************************/

GHandle GNewHandle ( long size )
{
	return ( GlobalAlloc ( GMEM_MOVEABLE, size ) );
}

/*******************************  GLockHandle  *****************************/

GPtr GLockHandle ( GHandle handle )
{
	return ( GlobalLock ( handle ) );
}

/*******************************  GUnlockHandle  **************************/

void GUnlockHandle ( GHandle handle )
{
	GlobalUnlock ( handle );
}

/*******************************  GFreeHandle  *****************************/

void GFreeHandle ( GHandle handle )
{
	GlobalFree ( handle );
}

/*******************************  GFreePtr  ********************************/

void GFreePtr ( void *ptr )
{
	free ( ptr );
}

/*******************************  GNewMatrix  ********************************/

void **GNewMatrix ( long size, long rows, long cols )
{
	void **matrix;
	long i, k;

	/*** Allocate memory for array of pointers to matrix rows, plus
	     and one more to hold a terminating NULL row pointer.  If
	     successful, allocate memory for individual matrix rows. ***/

	matrix = (void **) malloc ( sizeof ( void * ) * ( rows + 1 ) );
	if ( matrix != NULL )
	{
		for ( i = 0; i < rows; i++ )
		{
			/*** If memory allocation for an individual matrix row fails,
				 delete all previous rows, then the row pointer array, and
				 return NULL to indicate failure. ***/ 

			matrix[i] = (void *) malloc ( size * cols );
			if ( matrix[i] == NULL )
			{
				for ( k = 0; k < i; k++ )
					free ( matrix[k] );
					
				free ( matrix );
				return ( NULL );
			}
		}
		
		/*** Set the last matrix row pointer to NULL. ***/
		
		matrix[i] = NULL;
	}

	return ( matrix );
}

/*************************  GFreeMatrix  **********************************/

void GFreeMatrix ( void **matrix )
{
	long i;

	/*** Free individual matrix rows, then the matrix row pointer array. ***/

	for ( i = 0; matrix[i] != NULL; i++ )
		free ( matrix[i] );
		
	free ( matrix );
}

/***  GGetFreeMemory  *******************************************************/

long GGetFreeMemory ( void )
{
#ifdef GWIN16
	return ( GetFreeSpace ( 0 ) );
#endif

#ifdef GWIN32
	MEMORYSTATUS	memstat;

	memstat.dwLength = sizeof ( MEMORYSTATUS );
	
	GlobalMemoryStatus ( &memstat );

	return ( memstat.dwAvailPhys );
#endif
}
