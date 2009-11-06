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

static short	ComputeNumTerms ( short );
static void		ComputeXYPolynomial ( double, double, double[10] );

/*****************************  RADecToXiEta  *******************************/

void RADecToXiEta ( double ra, double dec, double ra0, double dec0, double *xi, double *eta )
{
	double h;
	
	h = sin ( dec ) * sin ( dec0 ) + cos ( dec ) * cos ( dec0 ) * cos ( ra - ra0 );
	
	*xi = cos ( dec ) * sin ( ra - ra0 ) / h;
	*eta = ( sin ( dec ) * cos ( dec0 ) - cos ( dec ) * sin ( dec0 ) * cos ( ra - ra0 ) ) / h;
}

/******************************  XiEtaToRADec  *********************************/

void XiEtaToRADec ( double xi, double eta, double ra0, double dec0, double *ra, double *dec )
{
	double delta, gamma;
	
	delta = cos ( dec0 ) - eta * sin ( dec0 );
	gamma = sqrt ( xi * xi + delta * delta );
	*ra = atan2 ( xi, delta ) + ra0;
	*ra = *ra - TWO_PI * floor ( *ra / TWO_PI );
	*dec = atan ( ( sin ( dec0 ) + eta * cos ( dec0 ) ) / gamma );
}

/********************************  XiEtaToXY  *********************************/

void XiEtaToXY ( double xi, double eta, double **c, double *x, double *y )
{
	int		i;
	double	p[10];
	
	ComputeXYPolynomial ( xi, eta, p );
	
	for ( *x = *y = 0.0, i = 0; i < 10; i++ )
	{
		*x += c[i][0] * p[i];
		*y += c[i][1] * p[i];
	}	 
}

/******************************  XiEtaToXY  **************************************/

void XYToXiEta ( double x, double y, double **c, double *xi, double *eta )
{
	int		i;
	double	p[10];
	
	ComputeXYPolynomial ( x, y, p );
	
	for ( *xi = *eta = 0.0, i = 0; i < 10; i++ )
	{
		*xi  += c[i][0] * p[i];
		*eta += c[i][1] * p[i];
	}	 
}

/*****************************  XYToRADec  ***********************************/

void XYToRADec ( double x, double y, double **c, double ra0, double dec0,
double *ra, double *dec )
{
	double xi, eta;
	
	XYToXiEta ( x, y, c, &xi, &eta );
	XiEtaToRADec ( xi, eta, ra0, dec0, ra, dec );
}

/*******************************  RADecToXY  ***********************************/

void RADecToXY ( double ra, double dec, double ra0, double dec0, double **c,
double *x, double *y )
{
	double xi, eta;
	
	RADecToXiEta ( ra, dec, ra0, dec0, &xi, &eta );
	XiEtaToXY ( xi, eta, c, x, y );
}

/**************************  NewAstrometricSolution  ****************************/

int NewAstrometricSolution ( double ***a, double ***b, double ***c, double ***d )
{
	if ( a )
	{
		*a = NMatrix ( double, 10, 10 );
		if ( *a == NULL )
		{
			return ( FALSE );
		}
	}
	
	if ( b )
	{
		*b = NMatrix ( double, 10, 2 );
		if ( *b == NULL )
		{
			if ( a )
				NDestroyMatrix ( *a );
				
			return ( FALSE );
		}
	}
	
	if ( c )
	{
		*c = NMatrix ( double, 10, 10 );
		if ( *c == NULL )
		{
			if ( a )
				NDestroyMatrix ( *a );
				
			if ( b )
				NDestroyMatrix ( *b );
				
			return ( FALSE );
		}
	}
	
	if ( d )
	{
		*d = NMatrix ( double, 10, 2 );
		if ( *d == NULL )
		{
			if ( a )
				NDestroyMatrix ( *a );
			
			if ( b )	
				NDestroyMatrix ( *b );
			
			if ( c )
				NDestroyMatrix ( *c );
				
			return ( FALSE );
		}
	}
		
	return ( TRUE );
}

/*************************  DeleteAstrometricSolution  ******************************/

void DeleteAstrometricSolution ( double **a, double **b, double **c, double **d )
{
	if ( a )
		NDestroyMatrix ( a );
	
	if ( b )	
		NDestroyMatrix ( b );
	
	if ( c )	
		NDestroyMatrix ( c );
		
	if ( d )
		NDestroyMatrix ( d );
}

/**********************  InitializeAstrometricSolution  *****************************/

int InitializeAstrometricSolution ( double **a, double **b, double **c, double **d )
{
	int i, j;
	
	if ( a )
		for ( i = 0; i < 10; i++ )
			for ( j = 0; j < 10; j++ )
				a[i][j] = 0.0;
		
	if ( b )
		for ( i = 0; i < 10; i++ )
			for ( j = 0; j < 2; j++ )
				b[i][j] = 0.0;

	if ( c )
		for ( i = 0; i < 10; i++ )
			for ( j = 0; j < 10; j++ )
				c[i][j] = 0.0;

	if ( d )
		for ( i = 0; i < 10; i++ )
			for ( j = 0; j < 2; j++ )
				d[i][j] = 0.0;

	return ( TRUE );
}

/*************************  CopyAstrometricSolution  ****************************/

void CopyAstrometricSolution ( double **a, double **b, double **c, double **d )
{
	int i, j;
	
	if ( a && c )
		for ( i = 0; i < 10; i++ )
			for ( j = 0; j < 10; j++ )
				a[i][j] = c[i][j];
				
	if ( b && d )
		for ( i = 0; i < 10; i++ )
			for ( j = 0; j < 2; j++ )
				b[i][j] = d[i][j];
}

/**************************  SetAstrometricSolution  ******************************/

void SetAstrometricSolution ( double x, double y, double a, double s, double **b, double **d )
{
	int i;
	
	if ( b )
	{
		b[0][0] =  x;
		b[0][1] =  y;
		b[1][0] =  cos ( a ) / s;
		b[1][1] =  sin ( a ) / s;
		b[2][0] = -sin ( a ) / s;
		b[2][1] =  cos ( a ) / s;
		
		for ( i = 3; i < 10; i++ )
			b[i][0] = b[i][1] = 0.0;
	}
	
	if ( d )
	{	
		d[0][0] = -s * cos ( a ) * x - s * sin ( a ) * y;
		d[0][1] =  s * sin ( a ) * x - s * cos ( a ) * y;
		d[1][0] =  s * cos ( a );
		d[1][1] =  s * sin ( a );
		d[2][0] = -s * sin ( a );
		d[2][1] =  s * cos ( a );

		for ( i = 3; i < 10; i++ )
			d[i][0] = d[i][1] = 0.0;
	}
}

/**************************  GetAstrometricSolution  *****************************/

void GetAstrometricSolution ( double **b, double **d, double *x, double *y, double *a, double *s )
{
	if ( b )
	{
		*s = b[1][0] * b[2][1] - b[2][0] * b[1][1];
		if ( *s > 0.0 )
		{
			*s = 1.0 / sqrt ( *s );
			*a = atan2pi ( b[1][1] - b[2][0], b[1][0] + b[2][1] );
			*x = b[0][0];
			*y = b[0][1];
		}
	}

	if ( d )
	{
		*s = d[1][0] * d[2][1] - d[2][0] * d[1][1];
		if ( *s > 0.0 )
		{
			*s = sqrt ( *s );
			*a = atan2pi ( d[2][0] - d[1][1], d[1][0] + d[2][1] );
			*x = ( -cos ( *a ) * d[0][0] + sin ( *a ) * d[0][1] ) / *s;
			*y = ( -sin ( *a ) * d[0][0] - cos ( *a ) * d[0][1] ) / *s;
		}
	}
}

/*************************  AugmentAstrometricSolution  ****************************/

void AugmentAstrometricSolution ( double **a, double **b, double **c, double **d,
short order, double ra0, double dec0, double ra, double dec, double x, double y )
{
	int n = ComputeNumTerms ( order );
	double xi, eta, p[10], q[2];
	
	RADecToXiEta ( ra, dec, ra0, dec0, &xi, &eta );

	q[0] = x;
	q[1] = y;
	
	ComputeXYPolynomial ( xi, eta, p );
	NAugmentNormalEqns ( n, 2, p, q, a, b );

	q[0] = xi;
	q[1] = eta;
	
	ComputeXYPolynomial ( x, y, p );
	NAugmentNormalEqns ( n, 2, p, q, c, d );
}

/****************************  FitAstrometricSolution  ******************************/

int FitAstrometricSolution ( double **a, double **b, double **c, double **d, short order )
{
	int result1, result2, n = ComputeNumTerms ( order );
	
	result1 = NGaussJordanSolveMatrixEqn ( a, n, b, 2 );
	result2 = NGaussJordanSolveMatrixEqn ( c, n, d, 2 );
	
	return ( result1 && result2 );
}

/*****************************  ComputeXYPolynomial  **********************************

	Computes polynomial terms up to 3rd order for use in astrometric solutions.
	
	(x,y): coordinate variables.
	(p): recieves polynomial terms.
	
***************************************************************************************/

void ComputeXYPolynomial ( double x, double y, double p[10] )
{
	p[0] = 1.0;
	p[1] = x;
	p[2] = y;
	p[3] = x * x;
	p[4] = y * y;
	p[5] = x * y;
	p[6] = x * x * x;
	p[7] = y * y * y;
	p[8] = x * x * y;
	p[9] = y * y * x;
}

/*****************************  ComputeNumTerms  *************************************/

short ComputeNumTerms ( short order )
{
	short	n = 0;
	
	if ( order == 1 )
		n = 3;
		
	if ( order == 2 )
		n = 6;
		
	if ( order == 3 )
		n = 10;

	return ( n );		
}
