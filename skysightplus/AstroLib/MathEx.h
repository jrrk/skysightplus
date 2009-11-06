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

/****************  C9X floating-point extensions header file ****************/

#ifndef MATHEX_H
#define MATHEX_H

/*** constants and macros ***/

#define INFINITY	(*gInfinity)

/*** functions ***/

/*******************************  inf  **************************************

	Returns the floating-point representation of positive infinity.

	float inf ( void )

	This function should work on any machine which complies with IEEE754
	floating-point representation.  Be careful that your "target.h" file
	has correctly #defined the BYTESWAP macro to indicate the target
	platform's byte-ordering convention.

	You should not call this function directly, as it is not part of the
	floating-point extensions proposal, but instead use the INFINITY macro
	(which is part of the proposal).
	
*****************************************************************************/

float inf ( void );

/*******************************  isinf  *************************************

	Determines whether a floating-point value is infinite.

	int isinf ( float x );

	(x): floating-point value.

	This function returns TRUE if the value represents positive or negative
	infinity.  It returns FALSE otherwise.

******************************************************************************/

int isinf ( float );

/***************************  asinh  ***************************************

	Computes the inverse hyperbolic sine, cosine, and tangent of its argument.

	double asinh ( double x )
	double acosh ( double x )
	double atanh ( double x )

	(x): argument, in radians.
          
	These function return the inverse hyperbolic sine, cosine, and tangent
	of their arguments.

****************************************************************************/

double asinh ( double );
double acosh ( double );
double atanh ( double );

/*** global data: these extern declarations should be #included except
     when compiling "MathEx.c" itself.  The following statement allows
     the extern declarations to be included as long as you DO NOT #define
     MATHEX_NO_EXTERN_DECLARATIONS anywhere above this line! ***/

#ifndef MATHEX_NO_EXTERN_DECLARATIONS

extern float *gInfinity;

#endif

#endif
