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

/***************************  SetJupiterMoonMatrix  ********************************

  DESCRIPTION: Sets matrix for transforming XYZ vectors from Jupiter's equatorial
               frame to the Earth's B1950 equatorial frame.  
  
          (m): Recieves 3 x 3 rotation matrix.

      RETURNS: Nothing.  Use the matrix computed by this function to transform
               the XYZ coordinates of Jupiter's Galilean satellites Io, Europa,
               Ganymede, and Callisto to the Earth's B1950 equatorial frame. 
  
               The position of the north pole, which determines the inclination
               (je) and longitude of the ascending node (ne) of Jupiter's equator
               with the Earth's B1950 equatorial plane, is assumed to be fixed.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 345-351.
               Jean Meeus, "Astronomical Algorithms", pp. 288-299.
               
               Note: the Supplement appears to contain typographical errors;
               the correct form of the matrix is given in Meeus.  Also, here
               the longitude of Jupiter's pole is assumed to be fixed.
               
***********************************************************************************/

void SetJupiterMoonMatrix ( double m[3][3] )
{
  double epsilon, omega, j, phi, i;

  epsilon = 23.44578889 * RAD_PER_DEG;
  omega = 99.99754 * RAD_PER_DEG;
  j = 1.30691 * RAD_PER_DEG;
  phi = 316.500101000 * RAD_PER_DEG - omega;
  i = 3.10401 * RAD_PER_DEG;
	
  SetRotationMatrix ( m, 5, 0, i, 2, phi, 0, j, 2, omega, 0, epsilon );
}

/*******************************  IoXYZ  **********************************************

  DESCRIPTION: Computes Io's XYZ coordinates in Jupiter's equatorial frame.
  
         (jd): Julian date on which to compute coordinates.
          (x): Recieves Io's X-coordinate, in AU.
          (y): Recieves Io's Y-coordinate, in AU.
          (z): Recieves Io's Z-coordinate, in AU.

      RETURNS: Nothing.  Terms smaller than 1 arcminute have been omitted. 
  
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 345-351.
   
***********************************************************************************/
  
void IoXYZ (double jd, double *x, double *y, double *z)
{
  double t, l1, l2, p3, p4, theta1, psi, xi, nu, zeta, a = 0.002819347;

  t = jd - 2443000.5;
  
  l1     = fmod ( 106.078590000 + 203.4889553630643 * t, 360.0 ) * RAD_PER_DEG;
  l2     = fmod ( 175.733787000 + 101.3747245566245 * t, 360.0 ) * RAD_PER_DEG;
  p3     = fmod ( 187.550171000 +   0.00712408 * t, 360.0 ) * RAD_PER_DEG;
  p4     = fmod ( 335.309254000 +   0.00183939 * t, 360.0 ) * RAD_PER_DEG;
  theta1 = fmod ( 308.365749000 -   0.13280610 * t, 360.0 ) * RAD_PER_DEG;
  psi    = fmod ( 316.500101000 -   0.00000248 * t, 360.0 ) * RAD_PER_DEG;

  xi     = -41279.0e-7 * cos ( 2.0 * ( l1 - l2 ) );
  
  nu     =  82363.0e-7 * sin ( 2.0 * ( l1 - l2 ) )
           - 5596.0e-7 * sin ( p3 - p4 );
           
  zeta   =   7038.0e-7 * sin ( l1 - theta1 + nu );

  *x     = a * ( 1.0 + xi ) * cos ( l1 - psi + nu );
  *y     = a * ( 1.0 + xi ) * sin ( l1 - psi + nu );
  *z     = a * zeta;
}

/******************************  EuropaXYZ  *****************************************

  DESCRIPTION: Computes Europa's XYZ coordinates in Jupiter's equatorial frame.
  
         (jd): Julian date on which to compute coordinates.
          (x): Recieves Europa's X-coordinate, in AU.
          (y): Recieves Europa's Y-coordinate, in AU.
          (z): Recieves Europa's Z-coordinate, in AU.

      RETURNS: Nothing.  Terms smaller than 1 arcminute have been omitted. 
  
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 345-351.
   
***********************************************************************************/

void EuropaXYZ (double jd, double *x, double *y, double *z)
{
  double t, l1, l2, p3, p4, theta2, theta3, psi, phi, xi, nu, zeta, a = 0.004485872;

  t = jd - 2443000.5;
  
  l1     = fmod ( 106.078590000 + 203.4889553630643 * t, 360.0 ) * RAD_PER_DEG;
  l2     = fmod ( 175.733787000 + 101.3747245566245 * t, 360.0 ) * RAD_PER_DEG;
  p3     = fmod ( 187.550171000 +   0.00712408 * t, 360.0 ) * RAD_PER_DEG;
  p4     = fmod ( 335.309254000 +   0.00183939 * t, 360.0 ) * RAD_PER_DEG;
  theta2 = fmod ( 100.438938000 -   0.03261535 * t, 360.0 ) * RAD_PER_DEG;
  theta3 = fmod ( 118.908928000 -   0.00717678 * t, 360.0 ) * RAD_PER_DEG;
  psi    = fmod ( 316.500101000 -   0.00000248 * t, 360.0 ) * RAD_PER_DEG;
  phi    = fmod ( 184.415351000 +   0.17356902 * t, 360.0 ) * RAD_PER_DEG;
  
  xi     =   93748.0e-7 * cos ( l1 - l2 )
            - 3187.0e-7 * cos ( l2 - p3 );
            
  nu     = -185640.0e-7 * sin ( l1 - l2 )
            + 7571.0e-7 * sin ( l1 - 2.0 * l2 + p3 )
            + 6394.0e-7 * sin ( l2 - p3 )
            + 4159.0e-7 * sin ( l1 - 2.0 * l2 + p4 )
            + 3451.0e-7 * sin ( l2 - p4 )
            + 3172.0e-7 * sin ( phi );
            
  zeta   =   81575.0e-7 * sin ( l2 - theta2 + nu )
            + 4512.0e-7 * sin ( l2 - theta3 + nu )
            - 3286.0e-7 * sin ( l2 - psi + nu );

  *x     = a * ( 1.0 + xi ) * cos ( l2 - psi + nu );
  *y     = a * ( 1.0 + xi ) * sin ( l2 - psi + nu );
  *z     = a * zeta;
}

/**************************  GanymedeXYZ  **********************************************

  DESCRIPTION: Computes Ganymede's XYZ coordinates in Jupiter's equatorial frame.
  
         (jd): Julian date on which to compute coordinates.
          (x): Recieves Ganymede's X-coordinate, in AU.
          (y): Recieves Ganymede's Y-coordinate, in AU.
          (z): Recieves Ganymede's Z-coordinate, in AU.

      RETURNS: Nothing.  Terms smaller than 1 arcminute have been omitted. 
  
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 345-351.
   
***********************************************************************************/

void GanymedeXYZ (double jd, double *x, double *y, double *z)
{
  double t, psi, l2, l3, l4, p3, p4, theta3, theta4, xi, nu, zeta, a = 0.007155352;

  t = jd - 2443000.5;
  
  l2     = fmod ( 175.733787000 + 101.3747245566245 * t, 360.0 ) * RAD_PER_DEG;
  l3     = fmod ( 120.561385500 + 50.31760915340462 * t, 360.0 ) * RAD_PER_DEG;
  l4     = fmod (  84.455823000 + 21.57107087517961 * t, 360.0 ) * RAD_PER_DEG;
  p3     = fmod ( 187.550171000 +   0.00712408 * t, 360.0 ) * RAD_PER_DEG;
  p4     = fmod ( 335.309254000 +   0.00183939 * t, 360.0 ) * RAD_PER_DEG;
  theta3 = fmod ( 118.908928000 -   0.00717678 * t, 360.0 ) * RAD_PER_DEG;
  theta4 = fmod ( 322.746564000 -   0.00176018 * t, 360.0 ) * RAD_PER_DEG;
  psi    = fmod ( 316.500101000 -   0.00000248 * t, 360.0 ) * RAD_PER_DEG;

  xi     = -14691.0e-7 * cos ( l3 - p3 )
          +  6333.0e-7 * cos ( l2 - l3 );
  
  nu     =  29387.0e-7 * sin ( l3 - p3 )
          + 15800.0e-7 * sin ( l3 - p4 )
          - 12038.0e-7 * sin ( l2 - l3 )
          +  6558.0e-7 * sin ( p3 - p4 )
          +  3218.0e-7 * sin ( 2.0 * ( l3 - l4 ) );
          
  zeta   =  32387.0e-7 * sin ( l3 - theta3 + nu )
          - 16876.0e-7 * sin ( l3 - psi + nu )
          +  6871.0e-7 * sin ( l3 - theta4 + nu );

  *x    = a * ( 1.0 + xi ) * cos ( l3 - psi + nu );
  *y    = a * ( 1.0 + xi ) * sin ( l3 - psi + nu );
  *z    = a * zeta;
}

/*******************************  CallistoXYZ  *****************************************

  DESCRIPTION: Computes Callisto's XYZ coordinates in Jupiter's equatorial frame.
  
         (jd): Julian date on which to compute coordinates.
          (x): Recieves Callisto's X-coordinate, in AU.
          (y): Recieves Callisto's Y-coordinate, in AU.
          (z): Recieves Callisto's Z-coordinate, in AU.

      RETURNS: Nothing.  Terms smaller than 1 arcminute have been omitted. 
  
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 345-351.
   
***********************************************************************************/

void CallistoXYZ (double jd, double *x, double *y, double *z)
{
  double t, l4, p3, p4, theta3, theta4, psi, g, pi, xi, nu, zeta, a = 0.012585436;

  t = jd - 2443000.5;
  
  l4     = fmod (  84.455823000 + 21.57107087517961 * t, 360.0 ) * RAD_PER_DEG;
  p3     = fmod ( 187.550171000 +   0.00712408 * t, 360.0 ) * RAD_PER_DEG;
  p4     = fmod ( 335.309254000 +   0.00183939 * t, 360.0 ) * RAD_PER_DEG;
  theta3 = fmod ( 118.908928000 -   0.00717678 * t, 360.0 ) * RAD_PER_DEG;
  theta4 = fmod ( 322.746564000 -   0.00176018 * t, 360.0 ) * RAD_PER_DEG;
  psi    = fmod ( 316.500101000 -   0.00000248 * t, 360.0 ) * RAD_PER_DEG;
  g      = fmod ( 30.2380210168 + 0.08309256178969453 * t, 360.0 ) * RAD_PER_DEG;
  pi     = 13.470395000 * RAD_PER_DEG;
  
  xi     = -73328.0e-7 * cos ( l4 - p4 );
  
  nu     = 145573.0e-7 * sin ( l4 - p4 )
           - 6112.0e-7 * sin ( p3 - p4 )
           - 5605.0e-7 * sin ( g )       
           - 4840.0e-7 * sin ( 2.0 * ( psi - pi ) )
           - 3318.0e-7 * sin ( l4 - p3 );
           
  zeta   = -76439.0e-7 * sin ( l4 - psi + nu )
          + 44300.0e-7 * sin ( l4 - theta4 + nu )
          -  5075.0e-7 * sin ( l4 - theta3 + nu );

  *x     = a * ( 1.0 + xi ) * cos ( l4 - psi + nu );
  *y     = a * ( 1.0 + xi ) * sin ( l4 - psi + nu );
  *z     = a * zeta;
}

/*******************************  IoRotation  ********************************

  DESCRIPTION: Computes Io's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Io's north pole, in radians.
         (d1): Receives J2000 declination of Io's north pole, in radians.
         (w1): Receives argument of Io's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements, identical to the 1985 formulae.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 405.
               The Astronomical Almanac for the Year 1984, p. S31.
     
*******************************************************************************/

void IoRotation ( double jd, double *a1, double *d1, double *w1 )
{
	double d, t, j3, j4;
	
	d = jd - 2451545.0;
	t = d / 36525.0;
	
	j3 = Mod2Pi ( ( 283.90 + 4850.7 * t ) * RAD_PER_DEG );
	j4 = Mod2Pi ( ( 355.80 + 1191.3 * t ) * RAD_PER_DEG );
	
	*a1 = ( 268.05 - 0.009 * t
		+ 0.094 * sin ( j3 )
		+ 0.024 * sin ( j4 ) ) * RAD_PER_DEG;
	*d1 = ( 64.50 + 0.003 * t
		+ 0.040 * cos ( j3 )
		+ 0.011 * cos ( j4 ) ) * RAD_PER_DEG;
	*w1 = Mod2Pi ( ( 200.39 + 203.4889538 * d
		- 0.085 * sin ( j3 )
		- 0.022 * sin ( j4 ) ) * RAD_PER_DEG );
}

/*******************************  EuropaRotation  ******************************

  DESCRIPTION: Computes Europa's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Europa's north pole, in radians.
         (d1): Receives J2000 declination of Europa's north pole, in radians.
         (w1): Receives argument of Europa's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.

         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements, identical to the 1985 formulae.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 405.
               The Astronomical Almanac for the Year 1984, p. S31.
      
*******************************************************************************/

void EuropaRotation ( double jd, double *a1, double *d1, double *w1 )
{
	double d, t, j4, j5, j6, j7;
	
	d = jd - 2451545.0;
	t = d / 36525.0;
	
	j4 = Mod2Pi ( ( 355.80 + 1191.3 * t ) * RAD_PER_DEG );
	j5 = Mod2Pi ( ( 119.90 +  262.1 * t ) * RAD_PER_DEG );
	j6 = Mod2Pi ( ( 229.80 +   64.3 * t ) * RAD_PER_DEG );
	j7 = Mod2Pi ( ( 352.25 + 2382.6 * t ) * RAD_PER_DEG );
	
	*a1 = ( 268.08 - 0.009 * t
		+ 1.086 * sin ( j4 )
		+ 0.060 * sin ( j5 )
		+ 0.015 * sin ( j6 )
		+ 0.009 * sin ( j7 ) ) * RAD_PER_DEG;
	*d1 = ( 64.51 + 0.003 * t
		+ 0.468 * cos ( j4 )
		+ 0.026 * cos ( j5 )
		+ 0.007 * cos ( j6 )
		+ 0.002 * cos ( j7 ) ) * RAD_PER_DEG;	
	*w1 = Mod2Pi ( ( 35.72 + 101.3747235 * d
		- 0.980 * sin ( j4 )
		- 0.054 * sin ( j5 )
		- 0.014 * sin ( j6 )
		- 0.008 * sin ( j7 ) ) * RAD_PER_DEG );
}

/*******************************  GanymedeRotation  ****************************

  DESCRIPTION: Computes Ganymede's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Ganymede's north pole, in radians.
         (d1): Receives J2000 declination of Ganymede's north pole, in radians.
         (w1): Receives argument of Ganymede's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.

         NOTE: Uses the method of the 1991 IAU working group on cartographic
               and rotational elements, identical to the 1985 formulae.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 405.
               The Astronomical Almanac for the Year 1984, p. S31.
      
*******************************************************************************/

void GanymedeRotation ( double jd, double *a1, double *d1, double *w1 )
{
	double d, t, j4, j5, j6;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	j4 = Mod2Pi ( ( 355.80 + 1191.3 * t ) * RAD_PER_DEG );
	j5 = Mod2Pi ( ( 119.90 +  262.1 * t ) * RAD_PER_DEG );
	j6 = Mod2Pi ( ( 229.80 +   64.3 * t ) * RAD_PER_DEG );
	
	*a1 = ( 268.20 - 0.009 * t
		- 0.037 * sin ( j4 )
		+ 0.431 * sin ( j5 )
		+ 0.091 * sin ( j6 ) ) * RAD_PER_DEG;
	*d1 = ( 64.57 + 0.003 * t
		- 0.016 * cos ( j4 )
		+ 0.186 * cos ( j5 )
		+ 0.039 * cos ( j6 ) ) * RAD_PER_DEG;
	*w1 = Mod2Pi ( ( 43.14 + 50.3176081 * d
		+ 0.033 * sin ( j4 )
		- 0.389 * sin ( j5 )
		- 0.082 * sin ( j6 ) ) * RAD_PER_DEG );
}

/*******************************  CallistoRotation  ****************************

  DESCRIPTION: Computes Callisto's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Callisto's north pole, in radians.
         (d1): Receives J2000 declination of Callisto's north pole, in radians.
         (w1): Receives argument of Callisto's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.

         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements, identical to the 1985 formulae.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 405.
               The Astronomical Almanac for the Year 1984, p. S32.
      
*******************************************************************************/

void CallistoRotation ( double jd, double *a1, double *d1, double *w1 )
{
	double d, t, j5, j6, j8;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	j5 = Mod2Pi ( ( 119.90 +  262.1 * t ) * RAD_PER_DEG );
	j6 = Mod2Pi ( ( 229.80 +   64.3 * t ) * RAD_PER_DEG );
	j8 = Mod2Pi ( ( 113.35 + 6070.0 * t ) * RAD_PER_DEG );
	
	*a1 = ( 268.72 - 0.009 * t
		- 0.068 * sin ( j5 )
		+ 0.590 * sin ( j6 )
		+ 0.010 * sin ( j8 ) ) * RAD_PER_DEG;
	*d1 = ( 64.83 + 0.003 * t
		- 0.029 * cos ( j5 )
		+ 0.254 * cos ( j6 )
		- 0.004 * cos ( j8 ) ) * RAD_PER_DEG;
	*w1 = Mod2Pi ( ( 259.67 + 21.5710715 * d
		+ 0.061 * sin ( j5 )
		- 0.533 * sin ( j6 )
		- 0.009 * sin ( j8 ) ) * RAD_PER_DEG );
}
