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

/*****************************  SetPhobosMatrix  ******************************

  DESCRIPTION: Sets matrix for transforming Phobos's XYZ vector to the Earth's
               equatorial frame.
          
         (jd): Julian date for which to compute matrix.    
          (m): Recieves 3 x 3 rotation matrix. 
           
      RETURNS: Nothing.  Use this matrix to convert XYZ positions computed from
               the orbit given by PhobosOrbit() to the Earth's equatorial frame.
               
               The Explanatory Supplement doesn't specify the precession epoch;
               it is probably either J2000 or B1950.  We assume J2000.
               
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 342-345.
   
********************************************************************************/

void SetPhobosMatrix ( double m[3][3], double jd )
{
	double y, na, ja;
	
	y = ( jd - 2441266.5 ) / 365.25;
	na = ( 47.39 - 0.0014 * y ) * RAD_PER_DEG;
	ja = ( 37.27 + 0.0008 * y ) * RAD_PER_DEG;

	SetRotationMatrix ( m, 2, 0, ja, 2, na );
}

/*****************************  SetDeimosMatrix  ******************************

  DESCRIPTION: Sets matrix for transforming Deimos's XYZ vector to the Earth's
               equatorial frame.
          
         (jd): Julian date for which to compute matrix.    
          (m): Recieves 3 x 3 rotation matrix. 
           
      RETURNS: Nothing.  Use this matrix to convert XYZ positions computed from
               the orbit given by DeimosOrbit() to the Earth's equatorial frame.
               
               The Explanatory Supplement doesn't specify the precession epoch;
               it is probably either J2000 or B1950.  We assume J2000.
               
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 342-345.
   
*******************************************************************************/

void SetDeimosMatrix ( double m[3][3], double jd )
{
	double y, na, ja;
	
	y = ( jd - 2441266.5 ) / 365.25;
	na = ( 46.37 - 0.0014 * y ) * RAD_PER_DEG;
	ja = ( 36.62 + 0.0008 * y ) * RAD_PER_DEG;

	SetRotationMatrix ( m, 2, 0, ja, 2, na );
}

/**************************  PhobosOrbit  ***************************************

  DESCRIPTION: Computes Phobos's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to a fixed plane approximately equal
              to Mars's equator.  The origin of longitudes is the ascending node
              (intersection) of this plane with the Earth's equatorial plane.
              
              To convert an XYZ coordinate vector in this frame to the Earth's
              equatorial frame, use the rotation matrix computed by SetPhobosMatrix().

  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 342-345.
  
*************************************************************************************/

void PhobosOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double d, y, l, na;
	
	d = jd - 2441266.5;
	y = d / 365.25;
	l = ( 232.41 + 1128.844556 * d + 0.00124 * y * y ) * RAD_PER_DEG;
	na = ( 47.39 - 0.0014 * y ) * RAD_PER_DEG;
	
	*a = 6.26974e-5;
	*e = 0.0150;
	*gamma = 1.10 * RAD_PER_DEG;
	*theta = Mod2Pi ( ( 327.90 - 0.43533 * d ) * RAD_PER_DEG );
	*p = Mod2Pi ( ( 278.96 + 0.43526 * d ) * RAD_PER_DEG - na );
	*m = Mod2Pi ( l - *p - na );
}

/**************************  DeimosOrbit  ***************************************

  DESCRIPTION: Computes Deimos's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to a fixed plane approximately equal
              to Mars's equator.  The origin of longitudes is the ascending node
              (intersection) of this plane with the Earth's equatorial plane.
              
              To convert an XYZ coordinate vector in this frame to the Earth's
              equatorial frame, use the rotation matrix computed by SetDeimosMatrix().

  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 342-345.
  
*************************************************************************************/

void DeimosOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double d, y, h, l, na;
	
	d = jd - 2441266.5;
	y = d / 365.25;
	h = Mod2Pi ( ( 196.55 - 0.01801 * d ) * RAD_PER_DEG );
	l = ( 28.96 + 285.161888 * d - 0.27 * sin ( h ) ) * RAD_PER_DEG;
	na = ( 46.37 - 0.0014 * y ) * RAD_PER_DEG;
	
	*a = 1.56828e-4;
	*e = 0.0004;
	*gamma = 1.79 * RAD_PER_DEG;
	*theta = Mod2Pi ( ( 240.38 - 0.01801 * d ) * RAD_PER_DEG );
	*p = Mod2Pi ( ( 111.7 + 0.01798 * d ) * RAD_PER_DEG - na );
	*m = Mod2Pi ( l - *p - na );
}

/*******************************  PhobosRotation  *****************************

  DESCRIPTION: Computes Phobos's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 407.
     
*******************************************************************************/

void PhobosRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, m1, m2;
	
	d = jd - 2451545.0;
	t = d / 36525.0;
	
	m1 = Mod2Pi ( ( 169.51 - 0.4357640 ) * RAD_PER_DEG );
	m2 = Mod2Pi ( ( 192.93 + 1128.4096700 * t + 0.6644e-9 * d * d ) * RAD_PER_DEG );
	
	*a0 = ( 317.68 - 0.108 * t + 1.79 * sin ( m1 ) ) * RAD_PER_DEG;
	*d0 = (  52.90 - 0.061 * t - 1.08 * cos ( m1 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 35.06 + 1128.8445850 * d + 0.6644e-9 * d * d
	    - 1.42 * sin ( m1 ) - 0.78 * sin ( m2 ) ) * RAD_PER_DEG );
}

/*******************************  DeimosRotation  *****************************

  DESCRIPTION: Computes Deimos's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 407.
     
*******************************************************************************/

void DeimosRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, m3;

	d = jd - 2451545.0;
	t = d / 36525.0;

	m3 = Mod2Pi ( ( 53.47 - 0.0181510 * d ) * RAD_PER_DEG );
	
	*a0 = ( 316.65 - 0.108 * t + 2.98 * sin ( m3 ) ) * RAD_PER_DEG;
	*d0 = (  53.52 - 0.061 * t - 1.78 * cos ( m3 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 79.41 + 285.1618970 * d + 0.390e-10 * d * d
	    - 2.58 * sin ( m3 ) + 0.19 * cos ( m3 ) ) * RAD_PER_DEG );
}
