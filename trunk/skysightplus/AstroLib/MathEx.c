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

#include <math.h>
#include "Target.h"

/*** #include "MathEx.h" WITHOUT any of the extern declarations it contains! ***/
     
#define MATHEX_NO_EXTERN_DECLARATIONS
#include "MathEx.h"
#undef MATHEX_NO_EXTERN_DECLARATIONS

/*** global data ***/

#if BYTESWAP
static char sInfinity[4] = { 0x00, 0x00, 0x80, 0x7F };
#else
static char sInfinity[4] = { 0x7F, 0x80, 0x00, 0x00 };
#endif

float *gInfinity = (float *) sInfinity;

/******************************  inf  **************************************/

float inf ( void )
{
	return ( INFINITY );
}

/***************************  isinf  ****************************************/

int isinf ( float x )
{
	return ( x == INFINITY || x == -INFINITY );
}

/***************************  asinh  **************************************/

double asinh ( double x )
{
	if ( x > 0.0 )
		return (  log ( x + sqrt ( x * x + 1.0 ) ) );
	else
		return ( -log ( sqrt ( x * x + 1.0 ) - x ) );
}

/***************************  atanh  *************************************/

double atanh ( double x )
{
	return ( 0.5 * log ( ( 1.0 + x ) / ( 1.0 - x ) ) );
}

