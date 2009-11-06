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

#define TOLERANCE 1.0e-8

/**************************  MeanMotion  ***********************************/
                     
double MeanMotion ( double mu, double q, double e )
{
  double n, a;
  
  if ( q <= 0.0 )
    return ( 0.0 );
    
  if ( e < 1.0 )
  {
    a = q / ( 1.0 - e );
    n = sqrt ( mu / ( a * a * a ) );
  }
  
  if ( e == 1.0 )
  {
    n = 3.0 * sqrt ( mu / ( 2.0 * q * q * q ) );
  }
  
  if ( e > 1.0 )
  {
    a = q / ( e - 1.0 );
    n = sqrt ( mu / ( a * a * a ) );
  }
    
  return ( n );  
}

/****************************  J2MeanMotion  ********************************/

void J2MeanMotion (double ae, double j2, double a, double e, double i, double dm, double *dw, double *dn, double *dmm)
{
  double ci, p;

  ci   = cos ( i );
  p    = a * ( 1.0 - e * e );
  *dw  = (  0.75 * j2 * dm * ae * ae * ( 5.0 * ci * ci - 1.0 ) ) / ( p * p );
  *dn  = ( -1.50 * j2 * dm * ae * ae * ( ci ) ) / ( p * p );
  *dmm = (  0.75 * j2 * dm * ae * ae * ( 3.0 * ci * ci - 1.0 ) ) /
         ( a * a * pow ( 1.0 - e * e, 1.5 ) );
}

/*************************  SolveKeplersEqn  *********************************/

#define MAX_ITERATIONS 1000

void SolveKeplersEqn ( double m, double e, double q, double *nu, double *r )
{	
  short i = 0, f = 1;
  double ea, ha, s, s2, s3, delta;

  if ( q == 0.0 )
    *nu = *r = 0.0;

  /*** Orbital eccentricities less than zero are of course non-physical.
	   However, the polynomial expressions for the orbital elements of
	   severl bodies given in this library can produce negative values of
	   e for times in the very distant past or future.  This solution
	   is a hack which comes as close as any to handling this situation
	   "correctly" without causing invalid domain errors in the sqrt(),
	   sinh(), and cosh() functions. ***/

  if ( e < 0.0 )
  {
	e = -e;
	q = -q;
  }

  if ( e < 1.0 )
  {
    m = Mod2Pi ( m );

    if ( e < 0.975 )
    {
      ea = m;
      do
      {
        delta = ea - e * sin ( ea ) - m;
        ea -= delta / ( 1.0 - e * cos ( ea ) );
        i++;
      }
      while ( fabs ( ea ) < 1.0e8 && fabs ( delta ) > TOLERANCE && i < MAX_ITERATIONS );
	}
    else
    {
      if ( m < PI )
        ea = HALF_PI;
      else
        ea = 3.0 * HALF_PI;
      delta = HALF_PI / 2.0;
      do
      {
        if ( ( m - ( ea - e * sin ( ea ) ) ) > 0.0 )
          ea += delta;
        else
          ea -= delta;
        delta /= 2.0;
      }
      while ( delta > TOLERANCE );
    }
    
    *nu = 2.0 * atan ( sqrt ( ( 1.0 + e ) / ( 1.0 - e ) ) * tan ( ea / 2.0 ) );
    *r = q * ( 1.0 + e ) / ( 1.0 + e * cos ( *nu ) );
  }
	
  if ( e == 1.0 )
  {
    s = m;
    do
    {
      s2 = s * s;
      s3 = s2 * s;
      s = ( 2.0 * s3 + m ) / ( 3.0 * ( s2 + 1.0 ) );
      delta = s3 + 3.0 * s - m;
      i++;
    }
    while ( fabs ( delta ) > TOLERANCE && i < MAX_ITERATIONS );
		
    *nu = 2.0 * atan ( s );
    *r = q * ( 1.0 + s2 );
  }
	
  if ( e > 1.0 )
  {
    ha = asinh ( m / e );
    do
    {
      delta = ha - e * sinh ( ha ) + m;
      ha -= delta / ( 1.0 - e * cosh ( ha ) );
      i++;
    }
    while ( fabs ( delta ) > TOLERANCE && i < MAX_ITERATIONS );
		
    *nu = 2.0 * atan ( sqrt ( ( e + 1.0 ) / ( e - 1.0 ) ) * tanh ( ha / 2.0 ) );
    *r = q * ( 1.0 + e ) / ( 1.0 + e * cos ( *nu ) );
  }
}

#undef MAX_ITERATIONS

/***********************  OrbitToSpherical  *********************************/

void OrbitToSpherical ( double q, double e, double i, double w, double n, double m,
double *l, double *b, double *r )
{
  double  nu, u;

  SolveKeplersEqn ( m, e, q, &nu, r );
  u = w + nu;
  *l = atan2pi ( cos ( i ) * sin ( u ), cos ( u ) ) + n;
  if ( *l > TWO_PI )
    *l -= TWO_PI;
  if ( *l < 0 )
    *l += TWO_PI;
  *b = asin ( sin ( u ) * sin ( i ) );
}

/*******************************  OrbitToXYZ  ********************************/

void OrbitToXYZ ( double q, double e, double i, double w, double n, double m,
double *x, double *y, double *z )
{
  double nu, r;
  double u, cu, su, ci, si, cn, sn;

  SolveKeplersEqn ( m, e, q, &nu, &r );
      
  u = w + nu;
  cu = cos ( u );
  su = sin ( u );
  ci = cos ( i );
  si = sin ( i );
  cn = cos ( n );
  sn = sin ( n );

  *x = r * ( cu * cn - su * ci * sn );
  *y = r * ( cu * sn + su * ci * cn );
  *z = r * ( su * si );
}

/***********************  OrbitToXYZMotion  *******************************/

void OrbitToXYZMotion ( double q, double e, double i, double w, double n, double m,
double dm, double *x, double *y, double *z, double *dx, double *dy, double *dz, double *mu )
{
  double a, nu, r, p, h, dnu, dr;
  double u, cu, su, ci, si, cn, sn;

  SolveKeplersEqn ( m, e, q, &nu, &r );
  
  if ( e < 1.0 )
  {
    a = q / ( 1.0 - e );
    *mu = a * a * a * dm * dm;
  }
  
  if ( e == 1.0 )
  {
    *mu = 2.0 * q * q * q * dm * dm / 9.0;
  }
    
  if ( e > 1.0 )
  {
    a = q / ( e - 1.0 );
    *mu = a * a * a * dm * dm;
  }

  p = q * ( 1.0 + e );
  h = sqrt ( *mu * p );
  dnu = h / ( r * r );
  dr = h * e * sin ( nu ) / p;
    
  u = w + nu;
  cu = cos ( u );
  su = sin ( u );
  ci = cos ( i );
  si = sin ( i );
  cn = cos ( n );
  sn = sin ( n );

  *x = r * ( cu * cn - su * ci * sn );
  *y = r * ( cu * sn + su * ci * cn );
  *z = r * ( su * si );
  
  *dx = *x * dr / r + r * dnu * ( - su * cn - cu * ci * sn );
  *dy = *y * dr / r + r * dnu * ( - su * sn + cu * ci * cn );
  *dz = *z * dr / r + r * dnu * ( cu * si );
}

/*****************************  XYZMotionToOrbit  *******************************/
       
void XYZMotionToOrbit (double x, double y, double z, double dx, double dy, double dz, double mu, double *q, double *e, double *i, double *w, double *n, double *m, double *dm)
{
  double r2, v2, rv, hx, hy, hz, h2, h, r, u, a, nu, p, ea, s, ha;
  
  hx = y * dz - z * dy;
  hy = z * dx - x * dz;
  hz = x * dy - y * dx;
  
  r2 = x * x + y * y + z * z;
  v2 = dx * dx + dy * dy + dz * dz;
  h2 = hx * hx + hy * hy + hz * hz;
  rv = x * dx + y * dy + z * dz;

  h = sqrt ( h2 );
  r = sqrt ( r2 );

  p = h2 / mu;
  a = 1.0 / ( 2.0 / r - v2 / mu );
  *e = sqrt ( 1.0 - p / a );
  if ( fabs ( *e - 1.0 ) < TOLERANCE )
    *e = 1.0;
  *q = p / ( 1.0 + *e );
  nu = acos ( ( p / r - 1.0 ) / *e );
  if ( rv < 0.0 )
    nu = - nu;
  
  if ( *e < 1.0 )
  {
    ea = 2.0 * atan ( sqrt ( ( 1.0 - *e ) / ( 1.0 + *e ) ) * tan ( nu / 2.0 ) );
    *m = ea - *e * sin ( ea );
    *dm = sqrt ( mu / ( a * a * a ) );
  }

  if ( *e == 1.0 )
  {
    s = tan ( nu / 2.0 );
    *m = s * s * s + 3.0 * s;
    *dm = 3.0 * sqrt ( mu / ( 2.0 * *q * *q * *q ) );
  }
  
  if ( *e > 1.0 )
  {
    ha = 2.0 * atanh ( sqrt ( ( *e - 1.0 ) / ( *e + 1.0 ) ) * tan ( nu / 2.0 ) );
    *m = *e * sinh ( ha ) - ha;
    *dm = sqrt ( - mu / ( a * a * a ) );
  }

  *i = acos ( hz / h );
  
  *n = atan2pi ( hx, - hy );
    
  u = atan2pi ( ( - x * hx - y * hy ) / hz, ( - x * hy + y * hx ) / h );
  *w = u - nu;
  if ( *w < 0.0 )
    *w += TWO_PI;
  if ( *w > TWO_PI )
    *w -= TWO_PI;
}

/****************************  TransformOrbit  *********************************/

void TransformOrbit ( double m[3][3], double *i, double *w, double *n)
{
  double cw, sw, ci, si, cn, sn, e[3], h[3];
  
  cw = cos ( *w );
  sw = sin ( *w );
  ci = cos ( *i );
  si = sin ( *i );
  cn = cos ( *n );
  sn = sin ( *n );
  
  /*** Compute a unit eccentricity vector.  This lies in the orbit plane
       and points toward the periapse. ***/
       
  e[0] = cw * cn - sw * sn * ci;
  e[1] = cw * sn + sw * cn * ci;
  e[2] = sw * si;
  
  /*** Compute a unit angular momentum vector.  This points perpendicular
       to the orbit place.  ***/
       
  h[0] = sn * si;
  h[1] = -cn * si;
  h[2] = ci;
  
  /*** Rotate both vectors into the new frame. ***/
  
  TransformVector ( m, e );
  TransformVector ( m, h );

  /*** Extract the sines and cosines of the angular elements in the new frame
       from the transformed eccentricity and angular momentum vectors. ***/
         
  ci = h[2];
  si = sqrt ( h[0] * h[0] + h[1] * h[1] );
  
  if ( si == 0.0 )
  {
    cn = 1.0;
    sn = 0.0;
  }
  else
  {
    cn = -h[1] / si;
    sn = h[0] / si;
  }
  
  cw = e[0] * cn + e[1] * sn;
  if ( ci == 0.0 )
    sw = e[2];
  else
    sw = ( -e[0] * sn + e[1] * cn ) / ci;
  
  /*** Compute the elements, and place them in the right quadrant! ***/
  
  *i = atan2pi ( si, ci );
  *n = atan2pi ( sn, cn );
  *w = atan2pi ( sw, cw );
}
