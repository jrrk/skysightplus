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

/******************************  Precession  **********************************/
                
void Precession ( double jd0, double jd1, double *zeta, double *z, double *theta,
double *eta, double *pi, double *p )
{
  double T, T2, t, t2, t3;

  T = ( jd0 - J2000 ) / 36525.0;
  T2 = T * T;

  t = ( jd1 - jd0 ) / 36525.0;
  t2 = t * t;
  t3 = t2 * t;

  if ( zeta )
    *zeta  = ( ( 2306.2181  + 1.39656  * T - 0.000139 * T2 ) * t
             + (    0.30188 - 0.000344 * T ) * t2
             + (    0.017998 ) * t3
             ) / ARCSEC_PER_RAD;

  if ( z )    
    *z     = ( ( 2306.2181  + 1.39656  * T - 0.000139 * T2 ) * t
             + (    1.09468 + 0.000066 * T ) * t2
             + (    0.018203 ) * t3
             ) / ARCSEC_PER_RAD;

  if ( theta )    
    *theta = ( ( 2004.3109  - 0.85330  * T - 0.000217 * T2 ) * t
             - (    0.42665 + 0.000217 * T ) * t2
             - (    0.041833 ) * t3
             ) / ARCSEC_PER_RAD;

  if ( eta )
    *eta = ( ( 47.0029  - 0.06603  * T + 0.000598 * T2 ) * t
           + ( -0.03302 + 0.000598 * T ) * t2
           + (  0.000060 ) * t3
           ) / ARCSEC_PER_RAD;

  if ( pi )         
    *pi  = (    174.876384 * RAD_PER_DEG ) +
           (   3289.4789 * T + 0.60622 * T2
           - (  869.8089 + 0.50491 * T ) * t
           + (    0.03536 ) * t2
           ) / ARCSEC_PER_RAD;

  if ( p )       
    *p   = ( ( 5029.0966  + 2.222226 * T - 0.0000042 * T2 ) * t
           + (    1.11113 - 0.000042 * T ) * t2
           - (    0.000006 ) * t3
           ) / ARCSEC_PER_RAD;
}

/******************************  Obliquity  ************************************/

double Obliquity ( double jd )
{
  double t, e;

  t = ( jd - 2451545.0 ) / 36525.0;
  e = 23.439291 + t * ( -0.0130042 + t * ( -0.00000016 + t * 0.000000504 ) );
  
  return ( e * RAD_PER_DEG );
}

/*******************************  Nutation  ***********************************/

void Nutation ( double jd, double *dl, double *de )
{
  double t, n, l, l1, sn, cn, s2n, c2n, s2l, c2l, s2l1, c2l1;
	
  t = ( jd - J2000 ) / 36525.0;
  n  = fmod ( 125.0445 -   1934.1363 * t, 360.0 ) * RAD_PER_DEG;
  l  = fmod ( 280.4665 +  36000.7698 * t, 360.0 ) * RAD_PER_DEG * 2.0;
  l1 = fmod ( 218.3165 + 481267.8813 * t, 360.0 ) * RAD_PER_DEG * 2.0;
  
  sn   = sin ( n );
  cn   = cos ( n );
  s2n  = 2.0 * sn * cn;
  c2n  = cn * cn - sn * sn;
  s2l  = sin ( l );
  c2l  = cos ( l );
  s2l1 = sin ( l1 );
  c2l1 = cos ( l1 );
  
  *dl = ( -17.20 * sn - 1.32 * s2l - 0.23 * s2l1 + 0.21 * s2n ) / ARCSEC_PER_RAD;
  *de = (   9.20 * cn + 0.57 * c2l + 0.10 * c2l1 - 0.09 * c2n ) / ARCSEC_PER_RAD;
}

/**************************  EarthVelocity  ****************************

void EarthVelocity ( double jd, double *vx, double *vy, double *vz )
{
  double t, g, l, sl, cl;
  
  t = ( jd - J2000 ) / 36525.0;
  
  g  = fmod ( 357.528 + 35999.050 * t, 360.0 ) * RAD_PER_DEG;
  l  = fmod ( 280.460 + 36000.770 * t, 360.0 ) * RAD_PER_DEG;
  
  l += ( 1.915 * sin ( g ) + 0.020 * sin ( 2.0 * g ) ) * RAD_PER_DEG;
  
  sl = sin ( l );
  cl = cos ( l );
  
  *vx =  0.0172 * sl;
  *vy = -0.0158 * cl;
  *vz = -0.0068 * cl;
}

*/

void EarthVelocity ( double jd, double *vx, double *vy, double *vz )
{
	int 	i;
	double	t, l2, l3, l4, l5, l6, l7, l8, lm, d, mm, f;
	double	a[9], cosa[9], sina[9];
	
	t = ( jd - J2000 ) / 36525.0;

	l2 = 3.1761467 + 1021.3285546 * t;
	l3 = 1.7534703 +  628.3075849 * t;
	l4 = 6.2034809 +  334.0612431 * t;
	l5 = 0.5995465 +   52.9690965 * t;
	l6 = 0.8740168 +   21.3299095 * t;
	l7 = 5.4812939 +    7.4781599 * t;
	l8 = 5.3118863 +    3.8133036 * t;
	lm = 3.8103444 + 8399.6847337 * t;
	 d = 5.1984667 + 7771.3771486 * t;
	mm = 2.3555559 + 8328.6914289 * t;
	 f = 1.6279052 + 8433.4661601 * t;
	 
	a[0] = l3;
	a[1] = 2.0 * l3;
	a[2] = l5;
	a[3] = lm;
	a[4] = 3.0 * l3;
	a[5] = l6;
	a[6] = f;
	a[7] = lm + mm;
	a[8] = 2.0 * l5;

	for ( i = 0; i < 9; i++ )
	{
		cosa[i] = cos ( a[i] );
		sina[i] = sin ( a[i] );
	}
	
	*vx = ( -1719914.0 -   2.0 * t ) * sina[0] +       -25.0               * cosa[0]
	    + (     6434.0 + 141.0 * t ) * sina[1] + (   28007.0 - 107.0 * t ) * cosa[1]
	    +        715.0               * sina[2]
	    +        715.0               * sina[3]
	    + (      486.0 -   5.0 * t ) * sina[4] + (    -236.0 -   4.0 * t ) * cosa[4]
	    +        159.0               * sina[5]

	    +         39.0               * sina[7]
	    +         33.0               * sina[8] +       -10.0               * cosa[8];

	*vy = (       25.0 -  13.0 * t ) * sina[0] + ( 1578089.0 + 156.0 * t ) * cosa[0]
	    + (    25697.0 -  95.0 * t ) * sina[1] + (   -5904.0 - 130.0 * t ) * cosa[1]
	    +          6.0               * sina[2] +      -657.0               * cosa[2]
	    +                                             -656.0               * cosa[3]
	    + (     -216.0 -   4.0 * t ) * sina[4] + (    -446.0 +   5.0 * t ) * cosa[4]
	    +          2.0               * sina[5] +      -147.0               * cosa[5]
	    +                                               26.0               * cosa[6]
	    +                                              -36.0               * cosa[7]
	    +         -9.0               * sina[8] +       -30.0               * cosa[8];
	    
	*vz = (       10.0 +  32.0 * t ) * sina[0] + (  684185.0 - 358.0 * t ) * cosa[0]
	    + (    11141.0 -  48.0 * t ) * sina[1] + (   -2559.0 -  55.0 * t ) * cosa[1]
	    +        -15.0               * sina[2] +      -282.0               * cosa[2]
	    +                                             -285.0               * cosa[3]
	    +        -94.0               * sina[4] +      -193.0               * cosa[4]
	    +         -6.0               * sina[5] +       -61.0               * cosa[5]
	    +                                              -59.0               * cosa[6]
	    +                                              -16.0               * cosa[7]
	    +         -5.0               * sina[7] +       -13.0               * cosa[8];
	    
	*vx *= 1.0e-8;
	*vy *= 1.0e-8;
	*vz *= 1.0e-8;
}

/***********************  RelativisticAberration  ********************************/

double RelativisticAberration ( double p[3], double v[3], double p1[3] )
{
	double beta, dot, r, s, n;
	
	beta = sqrt ( 1.0 - ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] ) );
	r = sqrt ( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] );
  
	if ( r > 0.0 )
		dot = ( p[0] * v[0] + p[1] * v[1] + p[2] * v[2] ) / r;
	else
		dot = 0.0;
  	
	s = r * ( 1.0 + dot / ( 1.0 + beta ) );
	n = 1.0 + dot;
	
	p1[0] = ( p[0] * beta + v[0] * s ) / n;
	p1[1] = ( p[1] * beta + v[1] * s ) / n;
	p1[2] = ( p[2] * beta + v[2] * s ) / n;
  
  return ( ( 1.0 - dot ) / beta );
}

#define TOLERANCE 1.0e-8

/**************************  GeodeticToGeocentricXYZ  ******************************/
      
void GeodeticToGeocentricXYZ ( double l, double p, double h, double a, double f,
double *x, double *y, double *z )
{
  double c, s;
	
  c = cos ( p );
  s = sin ( p );
  f = ( 1.0 - f ) * ( 1.0 - f );
	
  c = 1.0 / sqrt ( c * c + f * s * s );
  s = f * c;
	
  *x = ( a * c + h ) * cos ( p ) * cos ( l );
  *y = ( a * c + h ) * cos ( p ) * sin ( l );
  *z = ( a * s + h ) * sin ( p );
}

/**************************  GeocentricXYZToGeodetic  *****************************

  DESCRIPTION: Converts geocentric rectangular coordinates to geodectic longitude,
               latitude, and height.
               
          (x): Geocentric X-coordinate.
          (y): Geocentric Y-coordinate.
          (z): Geocentric Z-coordinate.
          (a): Equatorial radius of reference ellipsoid.
          (f): Flattening of reference ellipsoid: f = ( a - b ) / a,
               where a and b are ellipsoid's equatorial and polar radii. 
          (l): Recieves geodetic longitude, in radians from -PI to PI.
          (p): Recieves geodetic latitude, in radians from -PI/2 to PI/2.
          (h): Receives geodetic height, in same units as (a).
          
      RETURNS: Nothing.
               The values for a and f adopted in the 1976 IAU reference ellipsoid
               are a = 6378140 m and f = 1 / 298.257.
     
   REFERENCES: The Astronomical Almanac for the Year 1990, pp. K11-K13.
     
***********************************************************************************/

void GeocentricXYZToGeodetic ( double x, double y, double z, double a, double f,
double *l, double *p, double *h )
{
  double r, e2, p1, s, c;
	
  *l = atan2pi ( y, x );

  r = sqrt ( x * x + y * y );
  e2 = 2.0 * f - f * f;
	
  *p = atan ( z / r );
  do
  {
    p1 = *p;
    s = sin ( p1 );
    c = 1.0 / sqrt ( 1.0 - e2 * s * s );
    *p = atan ( ( z + a * c * e2 * s ) / r );
  }
  while ( fabs ( p1 - *p ) > TOLERANCE );
  *h = r / cos ( *p ) - a * c;
}

/*************************  RefractionAngle  ***********************************/

double RefractionAngle ( double a, double p, double t )
{
  double a2, r;
	
  if ( a > 0.2618 )
  {
    r = 0.00452 * p / ( ( 273.0 + t ) * tan ( a ) );
  }
  else
  {
    a *= DEG_PER_RAD;
    a2 = a * a;
    r = p * ( 0.1594 + 0.0196 * a + 0.00002 * a2 )
      / ( ( 273.0 + t ) * ( 1.0 + 0.505 * a + 0.0845 * a2 ) );
  }
  
  return ( r * RAD_PER_DEG );
}

/**********************  SetPrecessionRotationMatrix  ***************************/

void SetPrecessionRotationMatrix ( double m[3][3], double jd0, double jd1, int n )
{
	double zeta, z, theta, e, dl, de;

	Precession ( jd0, jd1, &zeta, &z, &theta, NULL, NULL, NULL );
	
	if ( n )
	{
		e = Obliquity ( jd1 );
		Nutation ( jd1, &dl, &de );
		SetRotationMatrix ( m, 6, 2, zeta, 1, theta, 2, z, 0, -e, 2, dl, 0, e + de );
	}
	else  
	{
		SetRotationMatrix ( m, 3, 2, zeta, 1, theta, 2, z );
	}
}

/*************************  SetNutationRotationMatrix  **************************/

void SetNutationRotationMatrix ( double m[3][3], double e, double dl, double de, int s )
{
	de += e;
  
	if ( s > 0 )
		SetRotationMatrix ( m, 3, 0, -e, 2, dl, 0, de );
	else
		SetRotationMatrix ( m, 3, 0, -de, 2, -dl, 0, e );
}

/******************  SetEclipticPrecessionRotationMatrix  ***********************/

void SetEclipticPrecessionRotationMatrix ( double m[3][3], double jd0, double jd1 )
{
	double eta, pi, p;

	Precession ( jd0, jd1, NULL, NULL, NULL, &eta, &pi, &p );
	SetRotationMatrix ( m, 3, 2, -pi, 0, -eta, 2, pi + p );
}

/*******************  SetEclipticRotationMatrix **************************/

void SetEclipticRotationMatrix ( double m[3][3], double e, int s )
{
	if ( s > 0 )
		SetRotationMatrix ( m, 1, 0, -e );
	else
		SetRotationMatrix ( m, 1, 0, e );
}

/**********************  SetGalacticRotationMatrix  ****************************/

void SetGalacticRotationMatrix ( double m[3][3], double jd, int s )
{
	double i, n, w, zeta, z, theta;
  
	w = GALACTIC_LON_ASC_NODE * RAD_PER_DEG;
	i = ( 90.0 - GALACTIC_N_POLE_DEC ) * RAD_PER_DEG;
	n = ( 90.0 + GALACTIC_N_POLE_RA  ) * RAD_PER_DEG;
  
	Precession ( jd, B1950, &zeta, &z, &theta, NULL, NULL, NULL );
  
	if ( s > 0 )
		SetRotationMatrix ( m, 6, 2, zeta, 1, theta, 2, z, 2, -n, 0, -i, 2, w );
	else
		SetRotationMatrix ( m, 6, 2, -w, 0, i, 2, n, 2, -z, 1, -theta, 2, -zeta );
}                              

/*********************  SetHorizonRotationMatrix  *************************/

void SetHorizonRotationMatrix ( double m[3][3], double l, double b, int s )
{
	int i;
  
	if ( s > 0 )
	{
		SetRotationMatrix ( m, 2, 2, PI - l, 1, b - HALF_PI );
		for ( i = 0; i < 3; i++ )
			m[1][i] = -m[1][i];
	}
	else
	{
		SetRotationMatrix ( m, 2, 1, HALF_PI - b, 2, l - PI );
		for ( i = 0; i < 3; i++ )
			m[i][1] = -m[i][1];
	}  
}

void LongTermPrecession ( double jd, double *e, double *p )
{
/*
	double	t, t2, t3;
	
	t = ( jd - 2451545.0 ) / 3652500.0;
	t2 = t * t;
	t3 = t2 * t;
	
	*p = 502909.66 * t
	   +  11111.3 * t2
	   -      6.0 * t3;
	   
	*e = 84381.448
	   -  4681.50 * t
	   -     5.9 * t2
	   +  1813.0 * t3;

	*p = *p / ARCSEC_PER_RAD;
	*e = *e / ARCSEC_PER_RAD;	   
*/
	int		k;
	double	sinecosp, sinesinp, t;
	
	static double nu[34] =
	{
		50.4712 / ARCSEC_PER_RAD,
		50.3017 / ARCSEC_PER_RAD,
		50.6408 / ARCSEC_PER_RAD,
		18.8507 / ARCSEC_PER_RAD,
		17.7533 / ARCSEC_PER_RAD,
		26.3302 / ARCSEC_PER_RAD,
		18.3011 / ARCSEC_PER_RAD,
		49.9245 / ARCSEC_PER_RAD,
		51.0199 / ARCSEC_PER_RAD,
		19.4005 / ARCSEC_PER_RAD,
		50.8342 / ARCSEC_PER_RAD,
		82.0908 / ARCSEC_PER_RAD,
		 5.6120 / ARCSEC_PER_RAD,
		50.1600 / ARCSEC_PER_RAD,
		19.1255 / ARCSEC_PER_RAD,
		 7.0773 / ARCSEC_PER_RAD,
		53.6768 / ARCSEC_PER_RAD,
		47.2655 / ARCSEC_PER_RAD,
		49.1217 / ARCSEC_PER_RAD,
		51.8203 / ARCSEC_PER_RAD,
		 6.9492 / ARCSEC_PER_RAD,
		 7.1961 / ARCSEC_PER_RAD,
		83.1895 / ARCSEC_PER_RAD,
		 6.8259 / ARCSEC_PER_RAD,
		74.6121 / ARCSEC_PER_RAD,
		18.9599 / ARCSEC_PER_RAD,
		82.6417 / ARCSEC_PER_RAD,
		 5.4865 / ARCSEC_PER_RAD,
		49.7240 / ARCSEC_PER_RAD,
		18.5455 / ARCSEC_PER_RAD,
		48.6168 / ARCSEC_PER_RAD,
		52.3235 / ARCSEC_PER_RAD,
		82.2652 / ARCSEC_PER_RAD,
		81.9390 / ARCSEC_PER_RAD
	};
	
	static double b[34] =
	{
		0.392066,
		0.033291,
		0.032118,
		0.011062,
		0.004573,
		0.003191,
		0.003258,
		0.002234,
		0.002173,
		0.002030,
		0.001956,
		0.001388,
		0.001284,
		0.001285,
		0.001120,
		0.001136,
		0.000902,
		0.000899,
		0.000679,
		0.000674,
		0.000635,
		0.000536,
		0.000519,
		0.000501,
		0.000445,
		0.000387,
		0.000369,
		0.000353,
		0.000319,
		0.000315,
		0.000312,
		0.000266,
		0.000256
	};
	
	static double psi[34] =
	{
		  7.718 / DEG_PER_RAD,
		329.456 / DEG_PER_RAD,
		226.240 / DEG_PER_RAD,
		298.608 / DEG_PER_RAD,
		248.612 / DEG_PER_RAD,
		 52.704 / DEG_PER_RAD,
		272.424 / DEG_PER_RAD,
		359.588 / DEG_PER_RAD,
		200.934 / DEG_PER_RAD,
		146.363 / DEG_PER_RAD,
		139.149 / DEG_PER_RAD,
		254.778 / DEG_PER_RAD,
		176.832 / DEG_PER_RAD,
		  2.051 / DEG_PER_RAD,
		 43.820 / DEG_PER_RAD,
		 82.426 / DEG_PER_RAD,
		173.456 / DEG_PER_RAD,
		 21.803 / DEG_PER_RAD,
		 98.474 / DEG_PER_RAD,
		 96.179 / DEG_PER_RAD,
		231.817 / DEG_PER_RAD,
		258.246 / DEG_PER_RAD,
		307.846 / DEG_PER_RAD,
		208.423 / DEG_PER_RAD,
		142.458 / DEG_PER_RAD,
		261.476 / DEG_PER_RAD,
		284.322 / DEG_PER_RAD,
		153.860 / DEG_PER_RAD,
		238.712 / DEG_PER_RAD,
		131.224 / DEG_PER_RAD,
		116.498 / DEG_PER_RAD,
		 74.288 / DEG_PER_RAD,
		122.071 / DEG_PER_RAD,
		252.485 / DEG_PER_RAD
	};

	t = ( jd - 2451545.0 ) / 365.25;
	sinecosp = 0.0;
	sinesinp = 0.0;

	for ( k = 0; k < 34; k++ )
	{
		sinecosp += b[k] * cos ( psi[k] + nu[k] * t );
		sinesinp += b[k] * sin ( psi[k] + nu[k] * t );
	}
	
	*p = atan2pi ( sinesinp, sinecosp );
	*e = asin ( sqrt ( sinesinp * sinesinp + sinecosp * sinecosp ) );
}

void LongTermPrecessionMatrix ( double m[3][3], double jd0, double jd1 )
{
	double p0, p1, e0, e1;
	
	LongTermPrecession ( jd0, &e0, &p0 );
	LongTermPrecession ( jd1, &e1, &p1 );

	SetRotationMatrix ( m, 3, 0, -e0, 2, p1 - p0, 0, e1 );
}