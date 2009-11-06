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

/****************************  PlanetographicCoordinates  *************************

  DESCRIPTION: Determines the planetographic coordinatates of the center
               of a planet's disk and position angle of its north pole.
           
          (a): The planet's apparent right ascension, in radians.
          (d): The planet's apparent declination, in radians.
         (a1): Right ascension of the planet's north pole, in radians.
         (d1): Declination of the planet's north pole, in radians.
         (w1): Argument of the planet's prime meridian, in radians;
               positive for planets where longitude increases clockwise,
               negative for planets where longitude decreases clockwise.
               See note below.
          (l): Receieves longitude of the central meridian of the
               planet's disk, in radians.
          (b): Receievs planetocentric declination of the central
               point on the planet's disk, in radians.
          (p): Receives position angle of the north pole of the planet's
               disk, in radians.
               
      RETURNS: Nothing.  One should give (w1) for the time of observation
               antedated by the light time from the planet.
               
         NOTE: Currently, the IAU has embraced several different, conflicting
               conventions for measuring planetary longitudes.  Under the
               current definitions,

               1) A planet's north pole is the pole which lies north of the
                  invariable plane of the solar system (roughly the ecliptic).

               2) For the Earth, Sun, and Moon, longitude increases in the direction
                  of planetary rotation.  For all other planets, longitude increases
                  in the direction -opposite- planetary rotation.

               These definitions lead to several problems.  First, because of
               definition (1), some planets (Venus, Uranus, Pluto) rotate
               clockwise when viewed from above their north poles, while others
               rotate anticlockwise.  Worse, depending on whether the planet itself
               rotates clockwise or anticlockwise, convention (2) can make longitudes
               increase either clockwise -or- anticlockwise when viewed from above.
               For Uranus, it's even more confusing because the north pole lies
               south of the Earth's equator, which reverses things once again.
               
               In summary, for the Earth, Sun, Moon, Venus, and Pluto, longitude
               increases anticlockwise when viewed from above the north pole, so
               for these bodies the value given for (w1) should be negative.
               For the other planets, it should be positive.
               
               Also, note that (b) differs slightly from the latitude of the
               central point on the planet's disk, due to the planet's oblateness.
               
   REFERENCES: The Astronomical Almanac for the Year 1990.
               The Explanatory Supplement to the Astronomical Almanac, p. 387.
               
************************************************************************************/

void PlanetographicCoordinates (double a, double d, double a1, double d1, double w1, double *l, double *b, double *p)
{
  double sa, ca, sd, cd, sd1, cd1, k;
	
  sa = sin ( a1 - a );
  ca = cos ( a1 - a );
  sd = sin ( d );
  cd = cos ( d );
  sd1 = sin ( d1 );
  cd1 = cos ( d1 );
	
  *b = asin ( - sd1 * sd - cd1 * cd * ca );
  *p = atan2pi ( cd1 * sa, sd1 * cd - cd1 * sd * ca );

  k = atan2pi ( - cd1 * sd + sd1 * cd * ca, cd * sa );
  if ( w1 >= 0.0 )
    *l = Mod2Pi ( w1 - k );
  else
    *l = Mod2Pi ( k + w1 );
}

/**************************  VectorPlanetographicCoordinates  *********************

  DESCRIPTION: Determines the planetographic coordinatates of the center
               of a planet's disk and position angle of its north pole.
           
          (v): The planet's apparent direction, expressed as a unit vector
               in the J2000 equatorial frame.
         (a1): Right ascension of the planet's north pole, in radians.
         (d1): Declination of the planet's north pole, in radians.
         (w1): Argument of the planet's prime meridian, in radians;
               positive for planets where longitude increases clockwise,
               negative for planets where longitude decreases clockwise.
               See note above.
          (l): Receieves longitude of the central meridian of the
               planet's disk, in radians.
          (b): Receievs planetocentric declination of the central
               point on the planet's disk, in radians.
          (p): Receives position angle of the north pole of the planet's
               disk, in radians.
               
      RETURNS: Nothing.  One should give (w1) for the time of observation
               antedated by the light time from the planet.
               
         NOTE: See PlanetographicCoordinates() for more information on
               sign conventions regarding planetary longitudes.  This
               function should give identical results, but significant
               computation can be saved by using a vector-matrix method
               rather than the spherical-trigonometric method given in the
               Astronomical Almanace.  The method is one of my own devising
               and is detailed in the comments below.

************************************************************************************/

void VectorPlanetographicCoordinates ( double v[3], double a1, double d1, double w1,
double *l0, double *b0, double *pN )
{
	double v0[3], m[3][3], k, j1, n1;
	
	/*** Compute viewer's direction vector as seen from the planet. ***/
	
	v0[0] = -v[0];
	v0[1] = -v[1];
	v0[2] = -v[2];

	/*** Compute inclination of planet's equator to J2000 equator, and
	     longitude of ascending node of planet's equator on J2000 equator. ***/
	
	j1 = HALF_PI - d1;
	n1 = a1 + HALF_PI;
	
	/*** Compute matrix which transforms coordinates from J2000 equatorial frame
	     to planetographic frame, assuming argument of planet's prime meridian
	     is fixed at zero (i.e. no planetary rotation).  Then transform viewer's
	     direction vector to planetographic frame. ***/
	
	SetRotationMatrix ( m, 2, 2, -n1, 0, -j1 );	
	TransformVector ( m, v0 );
	
	/*** Convert to spherical coordinates to obtain planetographic latitude and
	     longitude of sub-viewer point; add (or subtract, depending on convention)
	     argument of planet's prime meridian to correct for planetary rotation. ***/

	XYZToSpherical ( v0[0], v0[1], v0[2], &k, b0, NULL );
	if ( w1 > 0.0 )
		*l0 = Mod2Pi ( w1 - k );
	else
		*l0 = Mod2Pi ( k + w1 );
		
	/*** Compute position angle from planet's direction vector to planet's north
	     pole vector (which corresponds to bottom row, i.e. Z-axis vector, of
	     J2000-to-planetographic matrix computed above. ***/
	     
	*pN = VectorPositionAngle ( v, m[2] );
}

/*******************************  SunRotation  ********************************

  DESCRIPTION: Computes the Sun's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Sun's north pole, in radians.
         (d1): Receives J2000 declination of Sun's north pole, in radians.
         (w1): Receives MINUS the argument of Sun's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.

         NOTE: See PlanetographicCoordinates().
      
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 398.
      
*******************************************************************************/
         
void SunRotation ( double jd, double *a1, double *d1, double *w1 )
{
  double d, t;
	
  d = jd - 2451545.0;
  t = d / 36525.0;
	
  *a1 = 286.13 * RAD_PER_DEG;
  *d1 =  63.87 * RAD_PER_DEG;
  *w1 = -Mod2Pi ( ( 84.10 + 14.1844000 * d ) * RAD_PER_DEG );
}

/*******************************  MercuryRotation  ****************************

  DESCRIPTION: Computes Mercury's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Mercury's north pole, in radians.
         (d1): Receives J2000 declination of Mercury's north pole, in radians.
         (w1): Receives argument of Mercury's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.

        NOTES: Uses the formulae of the 1985 IAU working group on cartographic
               and rotational elements, identical to the 1991 formulae.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 402.
      
*******************************************************************************/

void MercuryRotation ( double jd, double *a1, double *d1, double *w1 )
{
  double d, t;

  d = jd - 2451545.0;
  t = d / 36525.0;

  *a1 = ( 281.01 - 0.033 * t ) * RAD_PER_DEG;
  *d1 = (  61.45 - 0.005 * t ) * RAD_PER_DEG;
  *w1 = Mod2Pi ( ( 329.71 + 6.1385025 * d ) * RAD_PER_DEG );
}

/*******************************  VenusRotation  ******************************

  DESCRIPTION: Computes Venus's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Venus's north pole, in radians.
         (d1): Receives J2000 declination of Venus's north pole, in radians.
         (w1): Receives MINUS the argument of Venus's prime meridian,
               in radians from -TWO_PI to 0.
          
         NOTE: See PlanetographicCoordinates().
               Uses the method of the 1985 IAU working group on cartographic
               and rotational elements.  The 1991 forumla for (w) differs.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 402.
      
*******************************************************************************/

void VenusRotation ( double jd, double *a1, double *d1, double *w1 )
{
  double d, t;

  d = jd - 2451545.0;
  t = d / 36525.0;

  *a1 = 272.78 * RAD_PER_DEG;
  *d1 =  67.21 * RAD_PER_DEG;	
  *w1 = -Mod2Pi ( ( 159.91 - 1.4814205 * d ) * RAD_PER_DEG );

  /*** *w1 = -Mod2Pi ( ( 160.26 - 1.4813596 * d ) * RAD_PER_DEG ); 1991 value ***/
}

/*******************************  EarthRotation  ********************************

  DESCRIPTION: Computes the Earth's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Earth's north pole, in radians.
         (d1): Receives J2000 declination of Earth's north pole, in radians.
         (w1): Receives argument of Earth's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: See PlanetographicCoordinates().  Additionally, the constant
               term in the expression for (w1) had to be changed from 100.21
               to 190.21 degrees to obtain results consistent with current
               terrestrial longitude convention.

   REFERENCES: The Astronomical Almanac for the Year 1984, p. S30.
      
*******************************************************************************/

void EarthRotation ( double jd, double *a1, double *d1, double *w1 )
{
  double d, t;

  d = jd - 2451545.0;
  t = d / 36525.0;

  *a1 = (  0.00 - 0.641 * t ) * RAD_PER_DEG;
  *d1 = ( 90.00 - 0.557 * t ) * RAD_PER_DEG;
  *w1 = -Mod2Pi ( ( 190.21 + 360.9856123 * d ) * RAD_PER_DEG );
}

/*******************************  MarsRotation  *****************************

  DESCRIPTION: Computes Mars's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Mars's north pole, in radians.
         (d1): Receives J2000 declination of Mars's north pole, in radians.
         (w1): Receives argument of Mars's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.

        NOTES: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements, identical to the 1985 formulae.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 403.
               The Astronomical Almanac for the Year 1984, p. S30.
               
*******************************************************************************/

void MarsRotation ( double jd, double *a1, double *d1, double *w1 )
{
	double d, t;
	
	d = jd - 2451545.0;
	t = d / 36525.0;
	
	*a1 = ( 317.681 - 0.108 * t ) * RAD_PER_DEG;
	*d1 = (  52.886 - 0.061 * t ) * RAD_PER_DEG;
	*w1 = Mod2Pi ( ( 176.868 + 350.8919830 * d ) * RAD_PER_DEG );
}

/*******************************  JupiterRotation  **************************

  DESCRIPTION: Computes Jupiter's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Jupiter's north pole, in radians.
         (d1): Receives J2000 declination of Jupiter's north pole, in radians.
         (w1): Receives argument of Jupiter's prime meridian,
               in radians from 0 to TWO_PI.
     (system): the longitude system that (w1) should be computed for:
               1 = System I, the mean equatorial atmospheric rotation
               2 = System II, the mean atmospheric rotation north of the
                   south component of the north equatorial belt and south
                   of the north component of the south equatorial belt.
               3 = System III, the magnetic field rotation, which corresponds
                   to the rotation of the deep interior.
               All other values will produce meaningless results.
              
      RETURNS: Nothing.
      
        NOTES: Uses the formulae of the 1985 IAU working group on cartographic
               and rotational elements, identical to the 1991 formulae.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 403.
               The Astronomical Almanac for the Year 1984, p. S30.
      
*******************************************************************************/

void JupiterRotation ( double jd, double *a1, double *d1, double *w1, char system )
{
  double d, t;

  d = jd - 2451545.0;
  t = d / 36525.0;

  *a1 = ( 268.05 - 0.009 * t ) * RAD_PER_DEG;
  *d1 = (  64.49 + 0.003 * t ) * RAD_PER_DEG;
  if ( system == 1 )
    *w1 = Mod2Pi ( (  67.10 + 877.9000000 * d ) * RAD_PER_DEG );
  if ( system == 2 )
    *w1 = Mod2Pi ( (  43.30 + 870.2700000 * d ) * RAD_PER_DEG );
  if ( system == 3 )
    *w1 = Mod2Pi ( ( 284.95 + 870.5360000 * d ) * RAD_PER_DEG );
}

/*******************************  SaturnRotation  *****************************

  DESCRIPTION: Computes Saturn's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Saturn's north pole, in radians.
         (d1): Receives J2000 declination of Saturn's north pole, in radians.
         (w1): Receives argument of Saturn's prime meridian,
               in radians from 0 to TWO_PI.
     (system): the longitude system that (w1) should be computed for:
               1 = System I, the mean atmospheric rotation.  This system has
                   been found to be of little use since the Voyager encounters,
                   and is no longer supported by the IAU.
               3 = System III, the magnetic field rotation, which corresponds
                   to the rotation of the deep interior.
               All other values will produce meaningless results.
          
      RETURNS: Nothing.

        NOTES: Uses the method of the 1985 IAU working group on cartographic
               and rotational elements, identical to the 1991 formulae.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 404.
               The Astronomical Almanac for the Year 1984, p. S30.
      
*******************************************************************************/

void SaturnRotation ( double jd, double *a1, double *d1, double *w1, char system )
{
  double d, t;

  d = jd - 2451545.0;
  t = d / 36525.0;

  *a1 = ( 40.66 - 0.036 * t ) * RAD_PER_DEG;
  *d1 = ( 83.52 - 0.004 * t ) * RAD_PER_DEG;
  if ( system == 1 )
    *w1 = Mod2Pi ( ( 227.2037 + 844.3000000 * d ) * RAD_PER_DEG );
  if ( system == 3 )
    *w1 = Mod2Pi ( (  38.9000 + 810.7939024 * d ) * RAD_PER_DEG );
}

/*******************************  UranusRotation  *****************************

  DESCRIPTION: Computes Uranus's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Uranus's north pole, in radians.
         (d1): Receives J2000 declination of Uranus's north pole, in radians.
         (w1): Receives MINUS the argument of Uranus's prime meridian,
               in radians from -TWO_PI to 0.
          
      RETURNS: Nothing.  As with Saturn, only the rotation rate for the magnetic
               field (System III) is defined for Uranus.

         NOTE: See PlanetographicCoordinates().
         
               Referred to the ecliptic, the invariable plane of the solar system,
               or Uranus's orbit plane, Uranus's rotation rate is negative.
                
               Referred to the Earth's equator, Uranus's rotation rate is positive,
               since its north pole lies south of the Earth's equator.
               
               Uses the method of the 1985 IAU working group on cartographic
               and rotational elements.  The 1991 formula for (w1) differs.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 405.
               The Astronomical Almanac for the Year 1984, p. S30.
   
*******************************************************************************/

void UranusRotation ( double jd, double *a1, double *d1, double *w1 )
{
  double d, t;

  d = jd - 2451545.0;
  t = d / 36525.0;

  *a1 = 257.43 * RAD_PER_DEG;
  *d1 = -15.10 * RAD_PER_DEG;
  *w1 = Mod2Pi ( ( 261.62 - 554.9130000 * d ) * RAD_PER_DEG );
  
  /*** *w1 = -Mod2Pi ( ( 203.81 - 501.1600928 * d ) * RAD_PER_DEG ); 1991 value ***/
}

/*******************************  NeptuneRotation  ****************************

  DESCRIPTION: Computes Neptune's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Neptune's north pole, in radians.
         (d1): Receives J2000 declination of Neptune's north pole, in radians.
         (w1): Receives argument of Neptune's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.  As with Saturn, only the rotation rate for the magnetic
               field (System III) is defined for Neptune.

        NOTES: Uses the formulae of the 1985 IAU working group on cartographic
               and rotational elements.  The formulae of the 1991 working group
               are included but commented out, since the physical ephemeris
               tabulated in the Astronomical Almanac still uses the 1985 formulae.
               
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 406.
               The Astronomical Almanac for the Year 1984, p. S30.
      
**************************************************************************************/

void NeptuneRotation ( double jd, double *a1, double *d1, double *w1 )
{
	double d, t;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

/******************************  1991 formulae  ****************************************

	n = Mod2Pi ( 357.85 + 52.316 * t ) * RAD_PER_DEG;
	*a1 = ( 299.36 + 0.07 * sin ( n ) ) * RAD_PER_DEG;
	*d1 = (  43.46 - 0.51 * cos ( n ) ) * RAD_PER_DEG;
	*w1 = Mod2Pi ( ( 253.18 + 536.3128492 * d - 0.48 * sin ( n ) ) * RAD_PER_DEG );

***************************************************************************************/

	*a1 = 295.33 * RAD_PER_DEG;
	*d1 =  40.65 * RAD_PER_DEG;
	*w1 = Mod2Pi ( ( 107.21 + 48.75000000 * d ) * RAD_PER_DEG );
}

/*******************************  PlutoRotation  *************************************

  DESCRIPTION: Computes Pluto's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Pluto's north pole, in radians.
         (d1): Receives J2000 declination of Pluto's north pole, in radians.
         (w1): Receives MINUS the argument of Pluto's prime meridian,
               in radians from -TWO_PI to 0.
          
      RETURNS: Nothing.

         NOTE: See PlanetographicCoordinates().
               Uses the formulae of the 1985 IAU working group on cartographic
               and rotational elements.  The formulae of the 1991 working group
               are included but commented out, since the physical ephemeris
               tabulated in the Astronomical Almanac still uses the 1985 formulae.
               
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 406.
               The Astronomical Almanac for the Year 1984, p. S30.
      
**************************************************************************************/

void PlutoRotation ( double jd, double *a1, double *d1, double *w1 )
{
	double d;
	
	d = jd - 2451545.0;

	*a1 = 313.02 * RAD_PER_DEG;
	*d1 =   9.09 * RAD_PER_DEG;
	*w1 = -Mod2Pi ( ( 236.77 - 56.3623195 * d ) * RAD_PER_DEG );

/******************************  1982 formulae  ****************************************

	*a1 = 311.63 * RAD_PER_DEG;
	*d1 =   4.18 * RAD_PER_DEG;
	*w1 = -Mod2Pi ( ( 252.66 - 56.3640000 * d ) * RAD_PER_DEG );
	
***************************************************************************************/
}

/*******************************  MoonRotation  ********************************

  DESCRIPTION: Computes the Moon's rotational elements at a given time.
  
         (jd): Desired time, expressed as a Julian Date.
         (a1): Receives J2000 right ascension of Moon's north pole, in radians.
         (d1): Receives J2000 declination of Moon's north pole, in radians.
         (w1): Receives MINUS the argument of Moon's prime meridian,
               in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.

         NOTE: See PlanetographicCoordinates().
               Uses the formulae of the 1985 IAU working group on cartographic
               and rotational elements.

   REFERENCES: The Astronomical Almanac for the Year 1984, p. S31.
      
*******************************************************************************/

void MoonRotation ( double jd, double *a1, double *d1, double *w1 )
{
	double d, e1, e2, e3, e4, e5;

	d = jd - 2451545.0;
		
	e1 = Mod2Pi ( ( 125.045 -  0.052992 * d ) * RAD_PER_DEG );
	e2 = Mod2Pi ( ( 249.390 -  0.105948 * d ) * RAD_PER_DEG );
	e3 = Mod2Pi ( ( 196.694 - 13.012000 * d ) * RAD_PER_DEG );
	e4 = Mod2Pi ( ( 176.630 + 13.340716 * d ) * RAD_PER_DEG );
	e5 = Mod2Pi ( ( 358.219 -  0.985600 * d ) * RAD_PER_DEG );
	
	*a1 = ( 270.000
		- 3.878 * sin ( e1 )
		- 0.120 * sin ( e2 )
		+ 0.070 * sin ( e3 )
		- 0.017 * sin ( e4 ) ) * RAD_PER_DEG;
	*d1 = ( 66.534
		+ 1.543 * cos ( e1 )
		+ 0.024 * cos ( e2 )
		- 0.028 * cos ( e3 )
		+ 0.007 * cos ( e4 ) ) * RAD_PER_DEG;
	*w1 = -Mod2Pi ( ( 38.314 + 13.1763581 * d
		+ 3.558 * sin ( e1 )
		+ 0.121 * sin ( e2 )
		- 0.064 * sin ( e3 )
		+ 0.016 * sin ( e4 )
		+ 0.025 * sin ( e5 ) ) * RAD_PER_DEG );
}

/************************  AngularSize  ******************************/

double AngularSize ( double diameter, double distance )
{
	double radius = diameter / 2.0;
	
	if ( distance < radius )
		return ( PI );
	else
		return ( 2.0 * asin ( radius / distance ) );
}

/************************  PhaseAngle  ********************************/

double PhaseAngle ( double sun[3], double viewer[3] )
{
	double scale, phase = 0.0;
	
	scale = VectorMagnitude ( sun ) * VectorMagnitude ( viewer );
	if ( scale > 0.0 )
	{
		phase = DotProduct ( sun, viewer ) / scale;
		
		if ( phase < -1.0 )
			phase = PI;
		else if ( phase > 1.0 )
			phase = 0.0;
		else
			phase = acos ( phase );
	}

	return ( phase );
}

/*********************  IlluminatedFraction  **************************/

double IlluminatedFraction ( double phase )
{
	return ( ( 1.0 + cos ( phase ) ) / 2.0 );
}

/*************************  SunMagnitude  ******************************/

double SunMagnitude ( double d )
{
	return ( -26.72 + 5.0 * log10 ( d ) );
}

/************************  MoonMagnitude  ******************************/

double MoonMagnitude ( double r, double d, double b )
{
	return ( AsteroidMagnitude ( r, d, b, 0.21, 0.25 ) );
}

/*************************  MercuryMagnitude  ***************************/
   
double MercuryMagnitude ( double r, double d, double b )
{
	double i, i2, i3;
	double m;
	
	i = b * DEG_PER_RAD;
	i2 = i * i;
	i3 = i2 * i;
	m = -0.42 + 5.0 * log10 ( r * d ) + 0.0380 * i - 0.000273 * i2 + 0.0000002 * i3;
	
	return ( m );
}

/************************  VenusMagnitude  *****************************/

double VenusMagnitude ( double r, double d, double b )
{
	double i, i2, i3, m;
	
	i = b * DEG_PER_RAD;
	i2 = i * i;
	i3 = i2 * i;
	m = -4.40 + 5.0 * log10 ( r * d ) + 0.0009 * i + 0.000239 * i2 - 0.00000065 * i3;
	
	return ( m );
}

/************************  MarsMagnitude  ******************************/

double MarsMagnitude ( double r, double d, double b )
{
	double m, i;
	
	i = b * DEG_PER_RAD;
	m = -1.52 + 5.0 * log10 ( r * d ) + 0.016 * i;
	
	return ( m );
}

/***********************  JupiterMagnitude  *****************************/

double JupiterMagnitude ( double r, double d, double b )
{
	double m, i;
	
	i = b * DEG_PER_RAD;
	m = -9.40 + 5.0 * log10 ( r * d ) + 0.005 * i;
	return ( m );
}

/*********************  SaturnMagnitude  ***************************/

double SaturnMagnitude ( double r, double d, double beta, double b0 )
{
	double	m, i, sb;
	
	sb = sin ( b0 );
	i = beta * DEG_PER_RAD;
	m = -8.88 + 5.0 * log10 ( r * d ) + 0.044 * i - 2.60 * sb + 1.25 * sb * sb;
	
	return ( m );
}

/*****************  SaturnRingPlaneInclination  *********************/

double SaturnRingPlaneInclination ( double p[3], double d )
{
	double	ps[3] = { -0.0856116575103, -0.0735337181506, -0.99361131052 };
	
	/*** The inclination of the ring plane is equivalent to the
	     Saturnocentric latitude at the center of the planet's disk.
	     This value, in turn, is equivalent to 90 degrees minus the
	     angle between the Saturn-to-viewer vector and Saturn's north
	     pole vector, or the angle between the viewer-to-Saturn vector
	     (which is what we have!) and Saturn's south pole vector.  We
	     can find this angle from the dot product of the two vectors.
	     
	     The elements of Saturn's south pole vector in the J2000 equatorial
	     frame were computed above using constant values of 40.66 degrees
	     and 83.52 degrees for the right ascensision and declination of
	     Saturn's north pole, respectively (see the Astronomical Almanac
	     for the Year 1984, p. S30).  They can also be obtained by
	     un-commenting the following line:

	     SphericalToXYZ ( 40.66 * RAD_PER_DEG, 83.52 * RAD_PER_DEG, -1.0,
	     &ps[0], &ps[1], &ps[2] ); ***/
	
	return ( HALF_PI - acos ( DotProduct ( p, ps ) / d ) );
}

/************************  UranusMagnitude  *************************/

double UranusMagnitude ( double r, double d, double b )
{
	double m, i;
	
	i = b * DEG_PER_RAD;
	m = -7.19 + 5.0 * log10 ( r * d ) + 0.0028 * i;
	
	return ( m );
}

/**********************  NeptuneMagnitude  **************************/

double NeptuneMagnitude ( double r, double d, double b )
{
	double m, i;
	
	i = b * DEG_PER_RAD;
	m = -6.87 + 5.0 * log10 ( r * d );
	
	return ( m );
}

/********************  PlutoMagnitude  *******************************/

double PlutoMagnitude ( double r, double d, double b )
{
	double m, i;
	
	i = b * DEG_PER_RAD;
	m = -1.01 + 5.0 * log10 ( r * d ) + 0.041 * i;
	
	return ( m );
}

/************************  AsteroidMagnitude  *******************************/

double AsteroidMagnitude ( double r, double d, double b, double h, double g )
{
	double phi1, phi2, m;
	
	phi1 = exp ( -3.33 * pow ( tan ( b / 2.0 ), 0.63 ) );
	phi2 = exp ( -1.87 * pow ( tan ( b / 2.0 ), 1.22 ) );
	
	m = ( 1.0 - g ) * phi1 + g * phi2;
	
	if ( m > 0.0 )
		m = h + 5.0 * log10 ( r * d ) - 2.5 * log10 ( m );
	else
		m = INFINITY;
		
	return ( m );
}

/***********************  CometMagnitude  *************************/

double CometMagnitude (double r, double d, double h, double k)
{
	double m;
	
	m = h + 5.0 * log10 ( d ) + 2.5 * k * log10 ( r );
	
	return ( m );
}



