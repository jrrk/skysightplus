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

/******************************  atan2pi  *********************************/

double atan2pi ( double y, double x )
{
  if ( x == 0.0 )
  {
	 if ( y > 0.0 )
	  return ( HALF_PI );
	if ( y < 0.0 )
	  return ( -HALF_PI );
  }
  else
  {
	 if ( y < 0.0 )
	  return ( atan2 ( y, x ) + TWO_PI );
	else
	  return ( atan2 ( y, x ) );
  }

  return ( 0.0 );
}

/*********************************  Mod2Pi  ************************************/

double Mod2Pi ( double x )
{
  return ( x - TWO_PI * floor ( x / TWO_PI ) );
}

/******************************  DegMinSecToDecimal  **********************/

double DegMinSecToDecimal ( short deg, short min, double sec, char sign )
{
	double decimal;

	decimal = abs ( deg ) + min / 60.0 + sec / 3600.0;
	if ( sign == '-' )
	  decimal = - decimal;

	return ( decimal );
}

/****************************  DecimalToDegMinSec  ***********************/

void DecimalToDegMinSec ( double decimal,
short *deg, short *min, double *sec, char *sign )
{
	if ( decimal < 0.0 )
		*sign = '-';
	else
		*sign = '+';

	decimal = fabs ( decimal );
	*deg = floor ( decimal );
	*min = floor ( 60.0 * ( decimal - *deg ) );
	*sec = 3600.0 * ( decimal - *deg - *min / 60.0 );
}

/****************************  Separation  *******************************/

double Separation ( double a1, double d1, double a2, double d2 )
{
	double s;

	s = acos ( sin ( d1 ) * sin ( d2 ) + cos ( d1 )  * cos ( d2 ) * cos ( a2 - a1 ) );
	return ( s );
}

/****************************  PositionAngle  *****************************/

double PositionAngle ( double a1, double d1, double a2, double d2 )
{
  double eta, xi, pa;

  eta = cos ( d2 ) * sin ( a2 - a1 );
  xi = cos ( d1 ) * sin ( d2 ) - sin ( d1 ) * cos ( d2 ) * cos ( a2 - a1 );
  pa = atan2pi ( eta, xi );

  return ( pa );
}	

/************************  VectorSeparation  ***************************/

double VectorSeparation ( double u[3], double v[3] )
{
	return ( acos ( u[0] * v[0] + u[1] * v[1] + u[2] * v[2] ) );
}

/**********************  VectorPositionAngle  **************************/

double VectorPositionAngle ( double u[3], double v[3] )
{
	double n[3], e[3], edotv, ndotv, pa;

	n[2] = sqrt ( 1.0 - u[2] * u[2] );
	if ( n[2] == 0.0 )
		return ( 0.0 );

	n[0] = -u[0] * u[2] / n[2];
	n[1] = -u[1] * u[2] / n[2];

	e[0] = -u[1] / n[2];
	e[1] =  u[0] / n[2];
	e[2] = 0.0;
	
	edotv = e[0] * v[0] + e[1] * v[1];
	ndotv = n[0] * v[0] + n[1] * v[1] + n[2] * v[2];
	
	if ( edotv == 0.0 && ndotv == 0.0 )
		pa = 0.0;
	else
		pa = atan2pi ( edotv, ndotv );

	return ( pa );
}
