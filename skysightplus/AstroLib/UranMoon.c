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

#define SIN(x) sin((x)*RAD_PER_DEG)
#define COS(x) cos((x)*RAD_PER_DEG)
#define MOD(x) fmod((x)*RAD_PER_DEG,TWO_PI)

/*****************************  SetUranusMoonMatrix  ******************************

  DESCRIPTION: Sets matrix for transforming XYZ vectors from Uranus's
               equatorial frame to the Earth's B1950 equatorial frame.
              
          (m): Recieves 9-element (3x3) rotation matrix. 
           
      RETURNS: Nothing.  Use this matrix to convert XYZ positions computed from
               the orbits for Uranus's moons to Earth's B1950 equatorial frame.
               
               The north pole of Uranus's moons's orbital plane is that from which
               their motion appears counterclockwise.  This corresponds to Uranus's
               south pole, which by definition lies south of the invariable plane
               of the solar system.
               
               The position of the south pole, which determines the inclination
               (je) and longitude of the ascending node (ne) of the orbit plane
               with the Earth's B1950 equatorial plane, is assumed to be fixed.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 354-356.
   
************************************************************************************/
              
void SetUranusMoonMatrix ( double m[3][3] )
{
	double je, ne;
	
	je = ( 90.0 - 15.04 ) * RAD_PER_DEG;
	ne = ( 90.0 + 76.72 ) * RAD_PER_DEG;
	
	SetRotationMatrix ( m, 2, 0, je, 2, ne );
}

/**************************  MirandaOrbit  ***************************************

  DESCRIPTION: Computes Miranda's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to Uranus's equatorial plane.
              The origin of longitudes is the ascending node (intersection)
              of this plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector from this frame to the Earth's
              B1950 equatorial frame, use the rotation matrix computed by
              SetUranusMoonMatrix().

              This function adds 180 degrees to the orbital longitudes given by
              the Explanatory Supplement's formula to produce agreement with the
              Astronomical Almanac's ephemerides.

  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 368-372.
  
*************************************************************************************/

void MirandaOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double t, l, ecosp, esinp;
	double l1, l2, l3, p1, p2, p3;
	
	t    = jd - 2444239.5;
	
	l1     = Mod2Pi ( -2.18167e-4 * t + 1.32 );
	l2     = Mod2Pi ( -4.36336e-4 * t + 2.64 );
	l3     = Mod2Pi ( -6.54502e-4 * t + 3.97 );
	
	l    = - 0.23805158 + 4.44519055 * t + 0.02547217 * sin ( l1 )
	       - 0.00308831 * sin ( l2 ) - 3.181e-4 * sin ( l3 );
	
	p1     = Mod2Pi ( 1.5273e-4 * t + 0.61 );
	p2     = Mod2Pi ( 0.08606   * t + 0.15 );
	p3     = Mod2Pi ( 0.709     * t + 6.04 );
	
	ecosp  = 1.31238e-3 * cos ( p1 ) - 1.2331e-4 * cos ( p2 ) - 1.9410e-4 * cos ( p3 );
	esinp  = 1.31238e-3 * sin ( p1 ) - 1.2331e-4 * sin ( p2 ) - 1.9410e-4 * sin ( p3 );
	      
	*a     = 0.00086492;
	*e     = sqrt ( ecosp * ecosp + esinp * esinp );
	*gamma = 2.0 * asin ( 0.03787171 );
	*theta = Mod2Pi ( -1.54449e-4 * t + 5.70 );
	*p     = atan2pi ( esinp, ecosp );
	*m     = Mod2Pi ( l - *p + PI );
}

/**************************  ArielOrbit  ***************************************

  DESCRIPTION: Computes Ariel's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to Uranus's equatorial plane.
              The origin of longitudes is the ascending node (intersection)
              of this plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector from this frame to the Earth's
              B1950 equatorial frame, use the rotation matrix computed by
              SetUranusMoonMatrix().

              This function adds 180 degrees to the orbital longitudes given by
              the Explanatory Supplement's formula to produce agreement with the
              Astronomical Almanac's ephemerides.

  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 368-372.
  
*************************************************************************************/

void ArielOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double t, l, ecosp, esinp, gcost, gsint;
	double l1, l2, p1, p2, theta1, theta2;
	
	t      = jd - 2444239.5;
	
	l1     = Mod2Pi ( -2.18167e-4 * t + 1.32 );
	l2     = Mod2Pi ( -4.36336e-4 * t + 2.64 );
	
	l      = 3.09804641 + 2.49295252 * t
	       - 1.86050e-3 * sin ( l1 ) + 2.1999e-4 * sin ( l2 );
	
	p1     = Mod2Pi ( 4.727824e-5 * t + 2.41 );
	p2     = Mod2Pi ( 2.179316e-5 * t + 2.07 );
	
	theta1 = Mod2Pi ( -4.782474e-5 * t + 0.40 );
	theta2 = Mod2Pi ( -2.156628e-5 * t + 0.59 );
	
	ecosp  = 1.18763e-3 * cos ( p1 ) + 8.6159e-4 * cos ( p2 );
	esinp  = 1.18763e-3 * sin ( p1 ) + 8.6159e-4 * sin ( p2 );

	gcost  = 3.5825e-4 * cos ( theta1 ) + 2.9008e-4 * cos ( theta2 );
	gsint  = 3.5825e-4 * sin ( theta1 ) + 2.9008e-4 * sin ( theta2 );
	
	*a     = 0.00127689;
	*e     = sqrt ( ecosp * ecosp + esinp * esinp );
	*gamma = 2.0 * asin ( sqrt ( gcost * gcost + gsint * gsint ) );
	*theta = atan2pi ( gsint, gcost );
	*p     = atan2pi ( esinp, ecosp );
	*m     = Mod2Pi ( l - *p + PI );
}

/**************************  UmbrielOrbit  ***************************************

  DESCRIPTION: Computes Umbriel's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to Uranus's equatorial plane.
              The origin of longitudes is the ascending node (intersection)
              of this plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector from this frame to the Earth's
              B1950 equatorial frame, use the rotation matrix computed by
              SetUranusMoonMatrix().

              This function adds 180 degrees to the orbital longitudes given by
              the Explanatory Supplement's formula to produce agreement with the
              Astronomical Almanac's ephemerides.

  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 368-372.
  
*************************************************************************************/

void UmbrielOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double t, l, ecosp, esinp, gcost, gsint;
	double p1, p2, p3, p4, p5, theta1, theta2;
	
	t      = jd - 2444239.5;
	
	l      = 2.28540169 + 1.51614811 * t
	       + 6.6057e-4  * sin ( -2.18167e-4 * t + 1.32 );
	
	p1     = Mod2Pi ( 4.727824e-5  * t + 2.41 );
	p2     = Mod2Pi ( 2.179316e-5  * t + 2.07 );
	p3     = Mod2Pi ( 1.580524e-5  * t + 0.74 );
	p4     = Mod2Pi ( 2.9363068e-6 * t + 0.43 );
	p5     = Mod2Pi ( -0.01157     * t + 5.71 );
	
	theta1 = Mod2Pi ( -2.156628e-5 * t + 0.59 );
	theta2 = Mod2Pi ( -1.401373e-5 * t + 1.75 );
	
	ecosp  = - 2.2795e-4 * cos ( p1 ) + 3.90469e-3 * cos ( p2 )
	         + 3.0917e-4 * cos ( p3 ) + 2.2192e-4  * cos ( p4 )
	         + 5.4923e-4 * cos ( p5 );
	         
	esinp  = - 2.2795e-4 * sin ( p1 ) + 3.90469e-3 * sin ( p2 )
	         + 3.0917e-4 * sin ( p3 ) + 2.2192e-4  * sin ( p4 )
	         + 5.4923e-4 * sin ( p5 );
	       
	gcost  = 1.1136e-3 * cos ( theta1 ) + 3.5014e-4 * cos ( theta2 );
	gsint  = 1.1136e-3 * sin ( theta1 ) + 3.5014e-4 * sin ( theta2 );

	*a     = 0.00170811;
	*e     = sqrt ( ecosp * ecosp + esinp * esinp );
	*gamma = 2.0 * asin ( sqrt ( gcost * gcost + gsint * gsint ) );
	*theta = atan2pi ( gsint, gcost );
	*p     = atan2pi ( esinp, ecosp );
	*m     = Mod2Pi ( l - *p + PI );
}

/**************************  TitaniaOrbit  ***************************************

  DESCRIPTION: Computes Titania's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.
              
        NOTE: The angular elements are referred to Uranus's equatorial plane.
              The origin of longitudes is the ascending node (intersection)
              of this plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector from this frame to the Earth's
              B1950 equatorial frame, use the rotation matrix computed by
              SetUranusMoonMatrix().

              This function adds 180 degrees to the orbital longitudes given by
              the Explanatory Supplement's formula to produce agreement with the
              Astronomical Almanac's ephemerides.

  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 368-372.
  
*************************************************************************************/

void TitaniaOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double t, l, ecosp, esinp, gcost, gsint;
	double p1, p2, p3, theta1, theta2;
	
	t      = jd - 2444239.5;
	
	l      = 0.85635879 + 0.72171851 * t;
	
	p1     = Mod2Pi (  1.580524e-5  * t + 0.74 );
	p2     = Mod2Pi (  2.9363068e-6 * t + 0.43 );
	p3     = Mod2Pi ( -6.9008e-3    * t + 1.82 );
	
	theta1 = Mod2Pi ( -1.401373e-5  * t + 1.75 );
	theta2 = Mod2Pi ( -1.9713918e-6 * t + 4.21 );
	
	ecosp  = 9.3281e-4 * cos ( p1 ) + 1.12089e-3 * cos ( p2 ) + 7.9343e-4 * cos ( p3 );
	esinp  = 9.3281e-4 * sin ( p1 ) + 1.12089e-3 * sin ( p2 ) + 7.9343e-4 * sin ( p3 );
	
	gcost  = 6.8572e-4 * cos ( theta1 ) + 3.7832e-4 * cos ( theta2 );
	gsint  = 6.8572e-4 * sin ( theta1 ) + 3.7832e-4 * sin ( theta2 );
	
	*a     = 0.00291388;
	*e     = sqrt ( ecosp * ecosp + esinp * esinp );
	*gamma = 2.0 * asin ( sqrt ( gcost * gcost + gsint * gsint ) );
	*theta = atan2pi ( gsint, gcost );
	*p     = atan2pi ( esinp, ecosp );
	*m     = Mod2Pi ( l - *p + PI );
}

/*****************************  OberonOrbit  ***************************************

  DESCRIPTION: Computes Oberon's orbital elements.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
      (gamma): Recieves inclination, in radians.
          (p): Recieves longitude of pericenter, in radians.
      (theta): Recieves longitude of ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
      RETURNS: Nothing.
              
         NOTE: The angular elements are referred to Uranus's equatorial plane.
               The origin of longitudes is the ascending node (intersection)
               of this plane with the Earth's B1950 equatorial plane.
              
               To convert an XYZ coordinate vector from this frame to the Earth's
               B1950 equatorial frame, use the rotation matrix computed by
               SetUranusMoonMatrix().

               This function adds 180 degrees to the orbital longitudes given by
               the Explanatory Supplement's formula to produce agreement with the
               Astronomical Almanac's ephemerides.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 368-372.
  
*************************************************************************************/

void OberonOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double t, l, ecosp, esinp, gcost, gsint;
	double p1, p2, p3, theta1, theta2;
		
	t      = jd - 2444239.5;

	l      = -0.91559180 + 0.46669212 * t;
	
	p1     = Mod2Pi (  1.580524e-5  * t + 0.74 );
	p2     = Mod2Pi (  2.9363068e-6 * t + 0.43 );
	p3     = Mod2Pi ( -6.9008e-3    * t + 1.82 );

	theta1 = Mod2Pi ( -1.401373e-5  * t + 1.75 );
	theta2 = Mod2Pi ( -1.9713918e-6 * t + 4.21 );

	ecosp  = -7.5868e-4 * cos ( p1 ) + 1.39734e-3 * cos ( p2 ) - 9.8726e-4 * cos ( p3 );
	esinp  = -7.5868e-4 * sin ( p1 ) + 1.39734e-3 * sin ( p2 ) - 9.8726e-4 * sin ( p3 );
	
	gcost  = -5.9633e-4 * cos ( theta1 ) + 4.5196e-4 * cos ( theta2 );
	gsint  = -5.9633e-4 * sin ( theta1 ) + 4.5196e-4 * sin ( theta2 );
	
	*a     = 0.00390059;
	*e     = sqrt ( ecosp * ecosp + esinp * esinp );
	*gamma = 2.0 * asin ( sqrt ( gcost * gcost + gsint * gsint ) );
	*theta = atan2pi ( gsint, gcost );
	*p     = atan2pi ( esinp, ecosp );
	*m     = Mod2Pi ( l - *p + PI );
}

/*******************************  MirandaRotation  *****************************

  DESCRIPTION: Computes Miranda's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 412.
     
*******************************************************************************/

void MirandaRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, u11, u12;
	
	d = jd - J2000;
	t = d / 36525.0;
	
	u11 = Mod2Pi ( ( 102.23 - 2024.22 * t ) * RAD_PER_DEG );
	u12 = Mod2Pi ( ( 316.41 + 2863.96 * t ) * RAD_PER_DEG );
	
	*a0 = ( 257.43 + 4.41 * sin ( u11 ) - 0.04 * sin ( 2.0 * u11 ) ) * RAD_PER_DEG;
	*d0 = ( -15.08 + 4.25 * cos ( u11 ) - 0.02 * cos ( 2.0 * u11 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 30.70 - 254.6906892 * d - 1.27 * sin ( u12 ) + 1.15 * sin ( u11 )
	    + 0.15 * sin ( 2.0 * u12 ) - 0.09 * sin ( 2.0 * u11 ) ) * RAD_PER_DEG );
}


/*******************************  ArielRotation  *****************************

  DESCRIPTION: Computes Ariel's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 412.
     
*******************************************************************************/

void ArielRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, u12, u13;
	
	d = jd - J2000;
	t = d / 36525.0;
	
	u12 = Mod2Pi ( ( 316.41 + 2863.96 * t ) * RAD_PER_DEG );
	u13 = Mod2Pi ( ( 304.01 -   51.94 * t ) * RAD_PER_DEG );
	
	*a0 = ( 257.43 + 0.29 * sin ( u13 ) ) * RAD_PER_DEG;
	*d0 = ( -15.10 + 0.28 * cos ( u13 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 156.22 - 142.8356681 * d + 0.05 * sin ( u12 )
	    + 0.08 * sin ( u13 ) ) * RAD_PER_DEG );
}


/*******************************  UmbrielRotation  *****************************

  DESCRIPTION: Computes Umbriel's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 412.
     
*******************************************************************************/

void UmbrielRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, u12, u14;
	
	d = jd - J2000;
	t = d / 36525.0;
	
	u12 = Mod2Pi ( ( 316.41 + 2863.96 * t ) * RAD_PER_DEG );
	u14 = Mod2Pi ( ( 308.71 -   93.17 * t ) * RAD_PER_DEG );
	
	*a0 = ( 257.43 + 0.21 * sin ( u14 ) ) * RAD_PER_DEG;
	*d0 = ( -15.10 + 0.20 * cos ( u14 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 108.05 - 86.8688923 * d - 0.09 * sin ( u12 )
	    + 0.06 * sin ( u14 ) ) * RAD_PER_DEG );
}


/*******************************  TitaniaRotation  *****************************

  DESCRIPTION: Computes Titania's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 412.
     
*******************************************************************************/

void TitaniaRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, u15;
	
	d = jd - J2000;
	t = d / 36525.0;
	
	u15 = Mod2Pi ( ( 340.82 - 75.32 * t ) * RAD_PER_DEG );
	
	*a0 = ( 257.43 + 0.29 * sin ( u15 ) ) * RAD_PER_DEG;
	*d0 = ( -15.10 + 0.28 * cos ( u15 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 77.74 - 41.3514316 * d + 0.08 * sin ( u15 ) ) * RAD_PER_DEG );
}


/*******************************  OberonRotation  *****************************

  DESCRIPTION: Computes Oberon's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 412.
     
*******************************************************************************/

void OberonRotation ( double jd, double *a0, double *d0, double *w )
{
	double d, t, u16;
	
	d = jd - J2000;
	t = d / 36525.0;
	
	u16 = Mod2Pi ( ( 259.14 - 504.81 * t ) * RAD_PER_DEG );
	
	*a0 = ( 257.43 + 0.16 * sin ( u16 ) ) * RAD_PER_DEG;
	*d0 = ( -15.10 + 0.16 * cos ( u16 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 8.77 - 26.7394932 * d + 0.04 * sin ( u16 ) ) * RAD_PER_DEG );
}
