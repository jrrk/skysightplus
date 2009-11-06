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

/*****************************  SetTritonMatrix  ******************************

  DESCRIPTION: Sets matrix for transforming Triton's XYZ vector to the Earth's
               J2000 equatorial frame.
          
         (jd): Julian date for which to compute matrix.    
          (m): Recieves 3 x 3 rotation matrix. 
           
      RETURNS: Nothing.  Use this matrix to convert XYZ positions computed from
               the orbit given by TritonOrbit() to the J2000 equatorial frame.
               
               Triton's orbit is given with respect to a fixed reference plane,
               defined so that the projections of Neptune's spin angular momentum
               and Triton's orbital angular momentum onto the plane are equal
               and opposite.
                              
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 354-356.
   
************************************************************************************/

void SetTritonMatrix ( double m[3][3], double jd )
{
	double t, n, ap, dp, je, ne;
	
	t = ( jd - J2000 ) / 36525.0;
	
	n = fmod ( 359.28 + 54.308 * t, 360.0 ) * RAD_PER_DEG;
	ap = 298.72 + 2.58 * sin ( n ) - 0.04 * sin ( 2.0 * n );
	dp =  42.63 - 1.90 * cos ( n ) + 0.01 * cos ( 2.0 * n );

	je = ( 90.0 - dp ) * RAD_PER_DEG;
	ne = ( 90.0 + ap ) * RAD_PER_DEG;
	
	SetRotationMatrix ( m, 2, 0, je, 2, ne );
}

/*****************************  SetNereidMatrix  ******************************

  DESCRIPTION: Sets matrix for transforming Nereid's XYZ vector to the Earth's
               B1950 equatorial frame.
              
          (m): Recieves 3 x 3 rotation matrix. 
           
      RETURNS: Nothing.  Use this matrix to convert XYZ positions computed from
               the orbit given by NereidOrbit() to the B1950 equatorial frame.
               
               Nereid's orbit is defined with respect to Neptune's orbital plane.
                              
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 354-356.
   
********************************************************************************/

void SetNereidMatrix ( double m[3][3] )
{
	SetRotationMatrix ( m, 2, 0, 22.313 * RAD_PER_DEG, 2, 3.522 * RAD_PER_DEG );
}

/**************************  TritonOrbit  ***************************************

  DESCRIPTION: Computes Triton's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (l): Recieves orbital longitude, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to a fixed plane approximately equal
              to Neptune's equator.  The origin of longitudes is the ascending node
              (intersection) of this plane with the Earth's equatorial plane.
              
              Since the eccentricity is zero, the pericenter and mean anomaly are
              undefined, so the ascending node is the origin of longitudes.

              To convert an XYZ coordinate vector in this frame to the Earth's J2000
              equatorial frame, use the rotation matrix computed by SetTritonMatrix().

  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 373-375.
  
*************************************************************************************/

void TritonOrbit ( double jd, double *a, double *e, double *gamma, double *p, double *theta, double *l )
{
	double d;
	
	d = jd - 2433282.5;
	
	*a = 0.002368266;
	*e = 0.0;
	*gamma = 158.996 * RAD_PER_DEG;
	*theta = Mod2Pi ( ( 151.401 + 0.57806 * d / 365.25 ) * RAD_PER_DEG );
	*p = *theta;
	*l = Mod2Pi ( ( 200.913 + 61.2588532 * d ) * RAD_PER_DEG );
}

/**************************  NereidOrbit  ***************************************

  DESCRIPTION: Computes Nereid's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to Neptune's B1950 orbit plane.
              The origin of longitudes is the ascending node (intersection) of
              this plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector in this frame to the Earth's
              equatorial frame, use the rotation matrix computed by SetNereidMatrix().

  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 375-377.
  
*************************************************************************************/

void NereidOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double d, t, psi;
	
	d = jd - 2433680.5;
	t = d / 36525.0;
	psi = fmod ( 289.2 + 2.68 * t, 360.0 ) * RAD_PER_DEG;
	
	*a = 0.036868;
	*e = 0.74515;
	*gamma = RAD_PER_DEG * 10.041;
	*theta = Mod2Pi ( RAD_PER_DEG * ( 329.3 - 2.4 * t
	       + 19.7 * sin ( 2.0 * psi ) - 3.3 * sin ( 4.0 * psi ) ) );
	*p = Mod2Pi ( psi - RAD_PER_DEG *
	     ( 19.25 * sin ( 2.0 * psi ) + 3.23 * sin ( 4.0 * psi ) ) );
	*m = Mod2Pi ( ( 358.91 + 0.999552 * d ) * RAD_PER_DEG );
}

/**************************  CharonOrbit  ***************************************

  DESCRIPTION: Computes Charon's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
          (j): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
          (n): Recieves longitude of ascending node, in radians.
          (u): Recieves orbital longitude measured from ascending node, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to the Earth's B1950 equator.
              The origin of longitudes is the ascending node.
              
              Since the eccentricity is zero, the pericenter and mean anomaly are
              undefined, so the ascending node is the origin of longitudes.
              
  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 377-378.
  
*************************************************************************************/

void CharonOrbit (double jd, double *a, double *e, double *j, double *p, double *n, double *u)
{
	double d;
	
	d = jd - 2445000.5;
	
	*a = 0.00012788;
	*e = 0.0;
	*j = 94.3 * RAD_PER_DEG;
	*n = 223.7 * RAD_PER_DEG;
	*p = *n;
	*u = Mod2Pi ( ( 78.6 + 56.3625 * d ) * RAD_PER_DEG );
}

/*******************************  TritonRotation  *****************************

  DESCRIPTION: Computes Triton's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 413.
     
*******************************************************************************/

void TritonRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, n7;
	
	d = jd - J2000;
	t = d / 365.25;
	n7 = Mod2Pi ( ( 177.85 + 52.316 * t ) * RAD_PER_DEG );
	
	*a0 = ( 299.36 - 32.35 * sin ( n7 ) - 6.28 * sin ( 2.0 * n7 )
	          - 2.08 * sin ( 3.0 * n7 ) - 0.74 * sin ( 4.0 * n7 )
	          - 0.28 * sin ( 5.0 * n7 ) - 0.11 * sin ( 6.0 * n7 )
	          - 0.07 * sin ( 7.0 * n7 ) - 0.02 * sin ( 8.0 * n7 )
	          - 0.01 * sin ( 9.0 * n7 ) ) * RAD_PER_DEG;
	          
	*d0 = (  41.17 + 22.55 * cos ( n7 ) + 2.10 * cos ( 2.0 * n7 )
	          + 0.55 * cos ( 3.0 * n7 ) + 0.16 * cos ( 4.0 * n7 )
	          + 0.05 * cos ( 5.0 * n7 ) + 0.02 * cos ( 6.0 * n7 )
	          + 0.01 * cos ( 7.0 * n7 ) ) * RAD_PER_DEG;
	          
	*w = Mod2Pi ( ( 296.53 -  61.2572637 * d  + 22.25 * sin ( n7 )
	                + 6.73 * sin ( 2.0 * n7 ) +  2.05 * sin ( 3.0 * n7 )
	                + 0.74 * sin ( 4.0 * n7 ) +  0.28 * sin ( 5.0 * n7 )
	                + 0.11 * sin ( 6.0 * n7 ) +  0.05 * sin ( 7.0 * n7 )
	                + 0.02 * sin ( 8.0 * n7 ) +  0.01 * sin ( 9.0 * n7 )
	              ) * RAD_PER_DEG );
}

/*******************************  CharonRotation  *****************************

  DESCRIPTION: Computes Charon's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 414.
     
*******************************************************************************/

void CharonRotation (double jd, double *a0, double *d0, double *w)
{
	double d;
	
	d = jd - J2000;
	
	*a0 = 313.02 * RAD_PER_DEG;
	*d0 = 9.09 * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 56.77 - 56.3623195 ) * RAD_PER_DEG );
}
