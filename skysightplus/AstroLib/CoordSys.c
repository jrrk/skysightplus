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

/************************  SphericalToXYZ  *****************************/

void SphericalToXYZ ( double a, double d, double r, double *x, double *y, double *z )
{
  *x = r * cos ( a ) * cos ( d );
  *y = r * sin ( a ) * cos ( d );
  *z = r * sin ( d );
}

/************************  XYZToSpherical  *****************************/

void XYZToSpherical ( double x, double y, double z, double *a, double *d, double *rp )
{
  double r;
  
  r = sqrt ( x * x + y * y + z * z );
  if ( rp )
    *rp = r;

  if ( r == 0.0 )
    *d = 0.0;
  else
    *d = asin ( z / r );
    
  if ( x == 0.0 && y == 0.0 )
    *a = 0.0;
  else
	 *a = atan2pi ( y, x );
}

/**************************  SphericalToXYZMotion  ************************/
	
void SphericalToXYZMotion ( double a, double d, double r, double da, double dd,
double dr, double *x, double *y, double *z, double *dx, double *dy, double *dz )
{
  double ca, sa, cd, sd;
	
  ca = cos ( a );
  sa = sin ( a );
  cd = cos ( d );
  sd = sin ( d );
	
  *x = r * ca * cd;
  *y = r * sa * cd;
  *z = r * sd;
	
  *dx = r * ( - cd * sa * da - ca * sd * dd ) + ca * cd * dr;
  *dy = r * ( ca * cd * da - sa * sd * dd ) + cd * sa * dr;
  *dz = r * cd * dd + sd * dr; 
}

/**************************  XYZToSphericalMotion  ****************************/

void XYZToSphericalMotion ( double x, double y, double z, double dx, double dy,
double dz, double *a, double *d, double *r, double *da, double *dd, double *dr )
{
  double cd;
  
  *r = sqrt ( x * x + y * y + z * z );
  if ( *r == 0.0 || ( x == 0.0 && y == 0.0 ) )
  {
    *a = *d = *dr = *dd = *da = 0.0;
  }
  else
  {
    *d = asin ( z / *r );
	*a = atan2pi ( y, x );

    cd = cos ( *d );
  
    *dr = ( x * dx + y * dy + z * dz ) / *r;
    *dd = ( *r * dz - z * *dr ) / ( cd * *r * *r );
    *da = ( x * dy - y * dx ) / ( x * x + y * y );
  }
}

/************************  VectorMagnitude  ******************************/

double VectorMagnitude ( double v[3] )
{
	return ( sqrt ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] ) );
}

/****************************  VectorSum  *********************************/

double *VectorSum ( double a[3], double b[3], double c[3] )
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
	
	return ( c );
}

/**************************  VectorDifference  ***************************/

double *VectorDifference ( double a[3], double b[3], double c[3] )
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
	
	return ( c );
}

/**************************  ScaleVector  ********************************/

double *ScaleVector ( double v[3], double m )
{
	v[0] *= m;
	v[1] *= m;
	v[2] *= m;
	
	return ( v );
}

/***********************  DotProduct ***********************************/

double DotProduct ( double a[3], double b[3] )
{
	return ( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] );
}

/***********************  CrossProduct *********************************/

double *CrossProduct ( double a[3], double b[3], double c[3] )
{
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
	
	return ( c );
}

/***********************  TransformVector ********************************/

double *TransformVector ( double m[3][3], double v[3] )
{
	double w[3];
	short i;
  
	w[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
	w[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
	w[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];

	for ( i = 0; i < 3; i++ )
		v[i] = w[i];
		
	return ( v );
}

/***********************  UnTransformVector *******************************/

double *UnTransformVector ( double m[3][3], double w[3] )
{
	double		v[3];
	short		i;
  
	v[0] = m[0][0] * w[0] + m[1][0] * w[1] + m[2][0] * w[2];
	v[1] = m[0][1] * w[0] + m[1][1] * w[1] + m[2][1] * w[2];
	v[2] = m[0][2] * w[0] + m[1][2] * w[1] + m[2][2] * w[2];

	for ( i = 0; i < 3; i++ )
		w[i] = v[i];
		
	return ( w );
}

/*********************  TransformRotationMatrix  ***************************/

void TransformRotationMatrix ( double b[3][3], double a[3][3] )
{
	double	c[3][3];
	int		i, j, k;

	for ( i = 0; i < 3; i++ )
		for ( k = 0; k < 3; k++ )
			for ( c[i][k] = 0.0, j = 0; j < 3; j++ )
        		c[i][k] += b[i][j] * a[j][k];

	for ( i = 0; i < 3; i++ )
		for ( k = 0; k < 3; k++ )
    		a[i][k] = c[i][k];
}

/********************  UnTransformRotationMatrix  *************************/

void UnTransformRotationMatrix ( double b[3][3], double a[3][3] )
{
	double	c[3][3];
	int		i, j, k;
  
	for ( i = 0; i < 3; i++ )
		for ( k = 0; k < 3; k++ )
      		for ( c[i][k] = 0.0, j = 0; j < 3; j++ )
        		c[i][k] += b[i][j] * a[k][j];

	for ( i = 0; i < 3; i++ )
		for ( k = 0; k < 3; k++ )
			a[i][k] = c[i][k];
}

/****************************  CopyVector  *********************************/

void CopyVector ( double u[3], double v[3] )
{
	int i;
	
	for ( i = 0; i < 3; i++ )
		u[i] = v[i];
}

/**********************  CopyRotationMatrix  *******************************/
   
void CopyRotationMatrix ( double b[3][3], double a[3][3] )
{
	int i, j;
  
	for ( i = 0; i < 3; i++ )
		for ( j = 0; j < 3; j++ )
			b[i][j] = a[i][j];
}

/*******************  TransposeRotationMatrix  ******************************/
   
void TransposeRotationMatrix ( double m[3][3] )
{
	double	temp;
	int		i, j;
	
	for ( i = 0; i < 3; i++ )
	{
		for ( j = 0; j < i; j++ )
		{
			temp = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = temp;
		}
	}
}

/***********************  SetRotationMatrix  *******************************/

void SetRotationMatrix ( double m[3][3], short n, ... )
{
	int i, k;
	double a, c, s, r[3][3];
	va_list ap;
  
	va_start ( ap, n );

	SetIdentityRotationMatrix ( m );
  
	for ( k = 0; k < n; k++ )
	{
		i = va_arg ( ap, int );
		a = va_arg ( ap, double );
		c = cos ( a );
		s = sin ( a );
    
		switch ( i )
		{
			case 0:
				r[0][0] = 1.0;  r[0][1] = 0.0;  r[0][2] = 0.0;
				r[1][0] = 0.0;  r[1][1] =   c;  r[1][2] =  -s;
				r[2][0] = 0.0;  r[2][1] =   s;  r[2][2] =   c;
				break;
				
			case 1:
				r[0][0] =   c;  r[0][1] = 0.0;  r[0][2] =  -s;
				r[1][0] = 0.0;  r[1][1] = 1.0;  r[1][2] = 0.0;
				r[2][0] =   s;  r[2][1] = 0.0;  r[2][2] =   c;
				break;
				
			case 2:
				r[0][0] =   c;  r[0][1] =  -s;  r[0][2] = 0.0;
				r[1][0] =   s;  r[1][1] =   c;  r[1][2] = 0.0;
				r[2][0] = 0.0;  r[2][1] = 0.0;  r[2][2] = 1.0;
				break;
		}
     
		TransformRotationMatrix ( r, m ); 
	}
   
	va_end ( ap ); 
}

/**********************  SetIdentityRotationMatrix  *********************/

void SetIdentityRotationMatrix ( double m[3][3] )
{
	int i, j;
	
	for ( i = 0; i < 3; i++ )
		for ( j = 0; j < 3; j++ )
			if ( i == j )
				m[i][j] = 1.0;
			else
				m[i][j] = 0.0;
}
