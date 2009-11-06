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

/*****************************  SetSaturnMoonMatrix  ******************************

  DESCRIPTION: Sets matrix for transforming XYZ vectors from Saturn's
               equatorial frame to the Earth's B1950 equatorial frame.
              
          (m): Recieves 3 x 3 rotation matrix. 
           
      RETURNS: Nothing.  Use this matrix to convert XYZ positions computed from
               the orbits for Saturn's moons Mimas, Enceladus, Tethys, and Dione
               to the Earth's B1950 equatorial frame.  For Saturn's other moons
               Rhea, Titan, Hyperion, Iapetus, and Phoebe, use an ecliptic-to-
               equatorial rotation matrix, since their orbits are referred to
               the ecliptic rather than Saturn's equator.  See SetEclipticMatrix(). 

               The position of the north pole, which determines the inclination
               (je) and longitude of the ascending node (ne) of the orbit plane
               with the Earth's B1950 equatorial plane, is assumed to be fixed.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 354-356.
   
************************************************************************************/
              
void SetSaturnMoonMatrix ( double m[3][3] )
{
	double je, ne;
	
	je = ( 90.0 - 83.33 ) * RAD_PER_DEG;
	ne = ( 90.0 + 38.40 ) * RAD_PER_DEG;
	
	SetRotationMatrix ( m, 2, 0, je, 2, ne );
}

/**************************  MimasOrbit  ******************************************

  DESCRPTION: Computes Mimas's orbital elements, referred to Saturn's equator.
              
        (jd): Julian date on which to compute elements.
         (a): Receives semimajor axis, in AU.
         (e): Recieves eccentricity.
     (gamma): Recieves inclination, in radians.
         (p): Recieves longitude of pericenter, in radians.
     (theta): Recieves longitude of ascending node, in radians.
         (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.  Periodic variations smaller than 0.1 degree are neglected.
              
              The angular elements are measured in Saturn's equatorial frame.
              The origin of longitudes is the ascending node (intersection) of
              Saturn's equatorial plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector in this frame to the Earth's
              J2000 equatorial frame, use the rotation matrix computed by
              SetSaturnMoonMatrix().

              The inclination (ie) and longitude (ne) of the ascending node of
              Saturn's equatorial plane with the B1950 ecliptic, and the angle
              (ab) between the nodes of Saturn's equatorial plane on the Earth's
              B1950 equator and ecliptic, are assumed to be fixed.
                   
  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 356-360.
  
*************************************************************************************/
              
void MimasOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double ie, ne, ab, d, y, t, theta1, l1, p1;
	
	ie =  28.06 * RAD_PER_DEG;
	ne = 168.83 * RAD_PER_DEG;
	ab =  41.53 * RAD_PER_DEG;

	d = jd - 2411093.0;
	y = d / 365.25;
	t = MOD ( 5.0616 * ( ( jd - 2433282.423 ) / 365.25 + 84.0 ) );
			
	theta1 = MOD ( 49.4 - 365.025 * y );
	l1 = MOD ( 128.839 + 381.994516 * d - 43.415 * sin ( t ) - 0.714 * sin ( 3.0 * t ) );
	p1 = MOD ( 107.0 + 365.560 * y );
		      
	*a = 0.00124171;
	*e = 0.01986;
	*gamma = 1.570 * RAD_PER_DEG;
	*theta = Mod2Pi ( theta1 - ne + ab );
	*p = Mod2Pi ( p1 - ne + ab );
	*m = Mod2Pi ( l1 - p1 );
}

/**************************  EnceladusOrbit  ***************************************

  DESCRPTION: Computes Enceladus's orbital elements, referred to Saturn's equator.
              
        (jd): Julian date on which to compute elements.
         (a): Receives semimajor axis, in AU.
         (e): Recieves eccentricity.
     (gamma): Recieves inclination, in radians.
         (p): Recieves longitude of pericenter, in radians.
     (theta): Recieves longitude of ascending node, in radians.
         (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.  Periodic variations smaller than 0.1 degree are neglected.
              
              The angular elements are measured in Saturn's equatorial frame.
              The origin of longitudes is the ascending node (intersection) of
              Saturn's equatorial plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector in this frame to the Earth's
              J2000 equatorial frame, use the rotation matrix computed by
              SetSaturnMoonMatrix().

              The inclination (ie) and longitude (ne) of the ascending node of
              Saturn's equatorial plane with the B1950 ecliptic, and the angle
              (ab) between the nodes of Saturn's equatorial plane on the Earth's
              B1950 equator and ecliptic, are assumed to be fixed.
                   
  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 356-360.
  
*************************************************************************************/

void EnceladusOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double ie, ne, ab, d, y, theta1, l1, p1;
		
	ie =  28.06 * RAD_PER_DEG;
	ne = 168.83 * RAD_PER_DEG;
	ab =  41.53 * RAD_PER_DEG;

	d = jd - 2411093.0;
	y = d / 365.25;

	theta1 = MOD ( 145. - 152.7 * y );
	l1 = MOD ( 200.155 + 262.7319052 * d );
	p1 = MOD ( 312.7 + 123.42 * y );

	*a = 0.00158935;
	*e = 0.00532;
	*gamma = 0.036 * RAD_PER_DEG;
	*theta = Mod2Pi ( theta1 - ne + ab );
	*p = Mod2Pi ( p1 - ne + ab );
	*m = Mod2Pi ( l1 - p1 );	
}

/**************************  TethysOrbit  ***************************************

  DESCRPTION: Computes Tethys's orbital elements, referred to Saturn's equator.
              
        (jd): Julian date on which to compute elements.
         (a): Receives semimajor axis, in AU.
         (e): Recieves eccentricity.
     (gamma): Recieves inclination, in radians.
         (p): Recieves longitude of pericenter, in radians.
     (theta): Recieves longitude of ascending node, in radians.
         (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.  Periodic variations smaller than 0.1 degree are neglected.
              
              The angular elements are measured in Saturn's equatorial frame.
              The origin of longitudes is the ascending node (intersection) of
              Saturn's equatorial plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector in this frame to the Earth's
              J2000 equatorial frame, use the rotation matrix computed by
              SetSaturnMoonMatrix().

              The inclination (ie) and longitude (ne) of the ascending node of
              Saturn's equatorial plane with the B1950 ecliptic, and the angle
              (ab) between the nodes of Saturn's equatorial plane on the Earth's
              B1950 equator and ecliptic, are assumed to be fixed.
                   
  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 356-360.
  
*************************************************************************************/

void TethysOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double ie, ne, ab, d, y, t, theta1, l1, p1;
		
	ie =  28.06 * RAD_PER_DEG;
	ne = 168.83 * RAD_PER_DEG;
	ab =  41.53 * RAD_PER_DEG;

	d = jd - 2411093.0;
	y = d / 365.25;
	t = MOD ( 5.0616 * ( ( jd - 2433282.423 ) / 365.25 + 84.0 ) );

	theta1 = MOD ( 111.41 - 72.24754 * y );
	l1 = MOD ( 284.9982 + 190.697920278 * d + 2.0751 * sin ( t ) );
	p1 = MOD ( 107.0 + 72.29 * y );

	*a = 0.00197069;
	*e = 0.000212;
	*gamma = 1.1121 * RAD_PER_DEG;
	*theta = Mod2Pi ( theta1 - ne + ab );
	*p = Mod2Pi ( p1 - ne + ab );
	*m = Mod2Pi ( l1 - p1 );	
}

/**************************  DioneOrbit  ***************************************

  DESCRPTION: Computes Dione's orbital elements, referred to Saturn's equator.
              
        (jd): Julian date on which to compute elements.
         (a): Receives semimajor axis, in AU.
         (e): Recieves eccentricity.
     (gamma): Recieves inclination, in radians.
         (p): Recieves longitude of pericenter, in radians.
     (theta): Recieves longitude of ascending node, in radians.
         (m): Recieves mean anomaly, in radians.
         
     RETURNS: Nothing.  Periodic variations smaller than 0.1 degree are neglected.
              
              The angular elements are measured in Saturn's equatorial frame.
              The origin of longitudes is the ascending node (intersection) of
              Saturn's equatorial plane with the Earth's B1950 equatorial plane.
              
              To convert an XYZ coordinate vector in this frame to the Earth's
              J2000 equatorial frame, use the rotation matrix computed by
              SetSaturnMoonMatrix().

              The inclination (ie) and longitude (ne) of the ascending node of
              Saturn's equatorial plane with the B1950 ecliptic, and the angle
              (ab) between the nodes of Saturn's equatorial plane on the Earth's
              B1950 equator and ecliptic, are assumed to be fixed.
                   
  REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 356-360.
  
*************************************************************************************/

void DioneOrbit (double jd, double *a, double *e, double *gamma, double *p, double *theta, double *m)
{
	double ie, ne, ab, d, y, theta1, l1, p1;
		
	ie =  28.06 * RAD_PER_DEG;
	ne = 168.83 * RAD_PER_DEG;
	ab =  41.53 * RAD_PER_DEG;

	d = jd - 2411093.0;
	y = d / 365.25;
	
	theta1 = MOD ( 228. - 30.6197 * y );
	l1 = MOD ( 255.1183 + 131.534920026 * d );
	p1 = MOD ( 173.6 + 30.8381 * y );
	
	*a = 0.00252413;
	*e = 0.001715;
	*gamma = 0.0289 * RAD_PER_DEG;
	*theta = Mod2Pi ( theta1 - ne + ab );
	*p = Mod2Pi ( p1 - ne + ab );
	*m = Mod2Pi ( l1 - p1 );	
}

/******************************  RheaOrbit  ***************************************

  DESCRIPTION: Computes Rhea's orbital elements, referred to the B1950 ecliptic.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
          (i): Recieves inclination, in radians.
          (w): Recieves longitude of periapse, in radians.
          (n): Recieves longitude of the ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
      RETURNS: Nothing.  Periodic variations smaller than 0.1 degree are neglected.
              
               The orbit plane precesses around Saturn's equatorial plane rather
               than the ecliptic, introducing the periodic terms in the expressions
               for the angular elements.  The eccentricity is forced by Titan.
              
               The expression for (i) in the Explanatory Supplement lacks the term
               sin(gamma0) term and appears to be a misprint.
              
               The inclination (ie) and longitude (ne) of the ascending node of
               Saturn's equatorial plane with the B1950 ecliptic are assumed fixed.
                   
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 360-368.
  
*************************************************************************************/

void RheaOrbit (double jd, double *a, double *e, double *i, double *w, double *n, double *m)
{
	double ie, ne, d, y, p, wt, gamma0, theta0, esinw, ecosw, lambda;
	
	ie =  28.06 * RAD_PER_DEG;
	ne = 168.83 * RAD_PER_DEG;

	d = jd - 2411093.0;
	y = d / 365.25;
	
	p = MOD (  305.0  + 10.2077 * y );
	wt = MOD ( 276.49 +  0.5219 * ( jd - 2411368.0 ) / 365.25 );
	
	gamma0 = 0.3305 * RAD_PER_DEG;
	theta0 = MOD ( 356.87 - 10.2077 * y );
	esinw = 0.000210 * sin ( p ) + 0.00100 * sin ( wt );
	ecosw = 0.000210 * cos ( p ) + 0.00100 * cos ( wt );
	lambda = MOD ( 359.4727 + 79.6900400700 * d )
		   + sin ( gamma0 ) * sin ( theta0 ) * tan ( ie / 2.0 );

	*a = 0.00352400;
	*e = sqrt ( esinw * esinw + ecosw * ecosw );
	*w = atan2pi ( esinw, ecosw );
	*i = ie - 0.0455 * RAD_PER_DEG
	       + sin ( gamma0 ) * cos ( theta0 );
	*n = ne - 0.0078 * RAD_PER_DEG + 
	       ( sin ( gamma0 ) * sin ( theta0 ) ) / sin ( ie );
	*m = Mod2Pi ( lambda - *w );
}

/******************************  TitanOrbit  ***************************************

  DESCRIPTION: Computes Titan's orbital elements, referred to the B1950 ecliptic.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
          (i): Recieves inclination, in radians.
          (w): Recieves longitude of periapse, in radians.
          (n): Recieves longitude of the ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
      RETURNS: Nothing.
              
        NOTES: The orbit plane precesses around Saturn's equatorial plane rather
               than the ecliptic, introducing the periodic terms in the expressions
               for the angular elements.  Additionally, there are significant solar
               perturbations on Titan's orbit, which are functions of the Sun's
               apparent orbit around Saturn (mean elements are is, ns, ms, and ls).
              
               The inclination (ie) and longitude of the ascending node (ne) of
               Saturn's equatorial plane with the B1950 ecliptic are assumed fixed.
                   
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 360-368.
  
*************************************************************************************/

void TitanOrbit (double jd, double *a, double *e, double *i, double *w, double *n, double *m)
{
	double ie, ne, t, d, y, is, ns, ms, ls, ia, na, wa, gamma0, theta0, lambda;
	double psi, theta, g;
	
	ie =  28.06 * RAD_PER_DEG;
	ne = 168.83 * RAD_PER_DEG;

	t = ( jd - 2415020.0 ) / 36525.0;
	d = jd - 2411368.0;
	y = d / 365.25;

	is = ( 2.4891 + 0.002435 * t ) * RAD_PER_DEG;
	ns = Mod2Pi ( ( 113.350  -    0.2597 * t ) * RAD_PER_DEG );
	ms = Mod2Pi ( ( 175.4762 + 1221.5515 * t ) * RAD_PER_DEG );
	ls = Mod2Pi ( ( 267.2635 + 1222.1136 * t ) * RAD_PER_DEG );
	
	gamma0 = 0.2990 * RAD_PER_DEG;
	theta0 = Mod2Pi ( ( 41.28 - 0.5219 * y ) * RAD_PER_DEG );

	ia = ie - 0.6204 * RAD_PER_DEG + sin ( gamma0 ) * cos ( theta0 );
	na = ne - 0.1418 * RAD_PER_DEG + sin ( gamma0 ) * sin ( theta0 ) / sin ( ie );
	wa = Mod2Pi ( ( 275.837 + 0.5219 * y ) * RAD_PER_DEG );
	
	psi = atan2pi ( sin ( is ) * sin ( na - ns ),
	      cos ( is ) * sin ( ia ) - sin ( is ) * cos ( ia ) * cos ( na - ns ) );
	      
	theta = ns + atan2pi ( sin ( ia ) * sin ( ns - ns ),
	        cos ( is ) * sin ( ia ) * cos ( na - ns ) - sin ( is ) * cos ( ia ) );
	        
	ls = ls - theta;
	g = wa - na - psi;
	
	lambda = Mod2Pi ( RAD_PER_DEG * ( 261.3121 + 22.57697385 * d )
	       + sin ( gamma0 ) * tan ( ie / 2.0 ) * sin ( theta0 )
	       - 0.000176 * sin ( ms ) - 0.000215 * sin ( 2.0 * ls )
	       + 0.000057 * sin ( 2.0 * ls + psi ) );

	*a = 0.00816765;
	*e = 0.028815 - 0.000184 * cos ( 2.0 * g ) + 0.000073 * cos ( 2.0 * ( ls - g ) );
	*w = wa + 0.00630  * sin ( 2.0 * g ) + 0.00250 * sin ( 2.0 * ( ls - g ) );
	*i = ia + 0.000232 * cos ( 2.0 * ls + psi );
	*n = na + 0.000503 * sin ( 2.0 * ls + psi );		
	*m = Mod2Pi ( lambda - *w );
}

/******************************  HyperionOrbit  ***************************************

  DESCRIPTION: Computes Hyperion's orbital elements, referred to the B1950 ecliptic.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
          (i): Recieves inclination, in radians.
          (w): Recieves longitude of periapse, in radians.
          (n): Recieves longitude of the ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
      RETURNS: Nothing.  Periodic variations smaller than 0.1 degree are neglected.
              
               The orbit plane precesses around Saturn's equatorial plane rather
               than the ecliptic, introducing the periodic terms in the expressions
               for the angular elements.
              
               The inclination (ie) and longitude (ne) of the ascending node of
               Saturn's equatorial plane with the B1950 ecliptic are assumed fixed.
                   
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 360-368.
  
*************************************************************************************/

void HyperionOrbit (double jd, double *a, double *e, double *i, double *w, double *n, double *m)
{
	double ie, ne, d, t, tau, zeta, lambda;
	double theta0, theta1, theta2, theta3, theta4, theta5;
	
	ie =  28.06 * RAD_PER_DEG;
	ne = 168.83 * RAD_PER_DEG;

	d = jd - 2415020.0;
	t = ( jd - 2433282.42345905 ) / 365.2422 + 50.0;
	
	tau    = fmod (  93.13 +  0.562039 * d, 360.0 ) * RAD_PER_DEG;
	zeta   = fmod ( 148.72 - 19.184    * t, 360.0 ) * RAD_PER_DEG;
	theta0 = fmod ( 105.31 -  2.392    * t, 360.0 ) * RAD_PER_DEG;
	theta1 = fmod (  38.73 -  0.5353   * t, 360.0 ) * RAD_PER_DEG;
	theta2 = fmod (  13.0  + 24.44     * t, 360.0 ) * RAD_PER_DEG;
	theta3 = fmod (  31.9  + 61.7524   * t, 360.0 ) * RAD_PER_DEG;
	theta4 = fmod (   8.0  + 24.44     * t, 360.0 ) * RAD_PER_DEG;
	theta5 = fmod ( 176.0  + 12.22     * t, 360.0 ) * RAD_PER_DEG;
	
	*a = 0.0099040 - 0.00003422 * cos ( tau );
	
	*e = 0.10441
	   + 0.02321 * cos ( zeta )
	   - 0.00401 * cos ( tau )
	   - 0.00110 * cos ( 2.0 * zeta )
	   + 0.00013 * cos ( theta3 )
	   + 0.00009 * cos ( zeta - tau );
	   
	*i = ie + ( -0.747 + 0.6200 * cos ( theta0 )
	                   + 0.315  * cos ( theta1 ) 
	                   - 0.018  * cos ( theta2 ) ) * RAD_PER_DEG;
	          
	*n = ne + ( -0.061 + 0.6200 * sin ( theta0 )
	                   + 0.315  * sin ( theta1 )
	                   - 0.018  * sin ( theta2 )
	          ) * RAD_PER_DEG / sin ( ie - 0.747 * RAD_PER_DEG );
	   
	*w = ( 69.993 - 18.6702 * t
	              - 13.36   * sin ( zeta )
	              +  2.16   * sin ( 2.0 * zeta )
	              -  0.47   * sin ( tau )
	              +  0.1507 * sin ( theta0 ) 
	              +  0.07   * sin ( theta3 ) ) * RAD_PER_DEG;
	              
	lambda = ( 176.7481 + 16.9199514 * d
	                    +  9.089  * sin ( tau )
	                    +  0.211  * sin ( zeta + tau )
	                    +  0.192  * sin ( zeta - tau )
	                    +  0.1507 * sin ( theta0 )
	                    -  0.091  * sin ( zeta )
	                    +  0.017  * sin ( theta4 )
	                    -  0.014  * sin ( 3.0 * tau )
	                    -  0.013  * sin ( theta5 )
	                    +  0.007  * sin ( 2.0 * tau ) ) * RAD_PER_DEG;
	                    
	*w = Mod2Pi ( *w );
	*m = Mod2Pi ( lambda - *w );  
}

/******************************  IapetusOrbit  ***************************************

  DESCRIPTION: Computes Iapetus's orbital elements, referred to the B1950 ecliptic.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
          (i): Recieves inclination, in radians.
          (w): Recieves longitude of periapse, in radians.
          (n): Recieves longitude of the ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
      RETURNS: Nothing.  Periodic variations smaller than 0.1 degree are neglected.
              
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 360-368.
  
*************************************************************************************/

void IapetusOrbit (double jd, double *a, double *e, double *i, double *w, double *n, double *m)
{
	double d, c, c2, c3, t, lambda, theta, ls, lt, ws, wt, phi;
	double l, g, gs, gt, gl;
	
	d = jd - 2409786.0;
	c = d / 36525.0;
	c2 = c * c;
	c3 = c2 * c;
	t = ( jd - 2415020.0 ) / 36525.0;
	
	*a = 0.02380984;
	*e = 0.0288184 + 0.000575 * c;
	*i = ( 18.45959 - 0.9555 * c - 0.0720 * c2 + 0.0054 * c3 ) * RAD_PER_DEG;
	*n = fmod ( 143.1294 - 3.797 * c + 0.116 * c2 + 0.008 * c3, 360.0 ) * RAD_PER_DEG;
	*w = fmod ( 352.905 + 11.65 * c, 360.0 ) * RAD_PER_DEG;
	lambda = fmod ( 76.19854 + 4.53795711 * d, 360.0 ) * RAD_PER_DEG;

	theta = fmod ( 4.367 - 0.195 * t, 360.0 ) * RAD_PER_DEG;
	ls  = fmod ( 267.263 + 1222.114 * t, 360.0 ) * RAD_PER_DEG;
	ws  = fmod ( 91.796 + 0.562 * t, 360.0 ) * RAD_PER_DEG;
	lt  = fmod ( 261.319 + 22.576974 * ( jd - 2411368.0 ), 360.0 ) * RAD_PER_DEG;
	wt  = fmod ( 277.102 +  0.001389 * ( jd - 2411368.0 ), 360.0 ) * RAD_PER_DEG;
	phi = fmod ( 60.470 + 1.521 * t, 360.0 ) * RAD_PER_DEG;
	
	l  = lambda - *w;
	ls = ls - ws;
	lt = lt - wt;
	g  = *w - *n - theta;
	gs = ws - fmod ( 146.819 - 3.918 * t, 360.0 ) * RAD_PER_DEG;
	gt = wt - fmod ( 205.055 - 2.091 * t, 360.0 ) * RAD_PER_DEG;
	gl = *w - *n - phi;
	
	*w += ( 0.08077 * sin ( gl - gt )
	      + 0.03547 * sin ( lt + gt - gl )
	      + 0.02139 * sin ( 2.0 * ( ls + gs - g ) )
	      + 0.01632 * sin ( 2.0 * l + gl - lt - gt )
	      + 0.01380 * sin ( l )
	      - 0.00676 * sin ( l + 2.0 * ( gl - ls - gs ) )
	      + 0.00028 * sin ( 3.0 * ls + 2.0 * gs - g )
	      ) * RAD_PER_DEG / *e;

	*n += ( 0.04204 * sin ( 2.0 * ( ls + gs ) + theta )
	      - 0.0142  * sin ( ls )
	      + 0.00358 * sin ( l + gl - lt - gt + phi )
	      + 0.0028  * sin ( 3.0 * ls + 2.0 * gs )
	      - 0.0012  * sin ( ls + 2.0 * gs )
	      - 0.0006  * sin ( 2.0 * ls )
	      + 0.0003  * sin ( 4.0 * ls + 2.0 * gs )
	      ) * RAD_PER_DEG / sin ( *i );

	*a += *a * ( 98.79e-5 * cos ( l + gl - lt - gt )
	           +  7.87e-5 * cos ( 2.0 * ( l + g - ls - gs ) ) );
	          
	*e += -140.97e-5 * cos ( gl - gt )
	      + 61.90e-5 * cos ( lt + gt - gl )
	      + 37.33e-5 * cos ( 2.0 * ( ls + gs - g ) )
	      + 28.49e-5 * cos ( 2.0 * l + gl - lt - gt )
	      + 24.08e-5 * cos ( l )
	      + 11.80e-5 * cos ( l + 2.0 * ( g - ls - gs ) );
	      
	*i += ( 0.04204 * cos ( 2.0 * ( ls + gs ) + theta )
	      + 0.00360 * cos ( l + gl - lt - gt + phi )
	      + 0.00235 * cos ( l + gl + lt + gt + phi )
	      + 0.0058  * cos ( 3.0 * ls + 2.0 * gs )
	      - 0.0024  * cos ( ls + 2.0 * gs ) ) * RAD_PER_DEG;

	lambda += ( - 0.06312 * sin ( ls )
	            - 0.04299 * sin ( l + gl - lt - gt )
	            - 0.02231 * sin ( 2.0 * ( ls + gs ) )
	            - 0.00789 * sin ( 2.0 * ( l + g - ls - gs ) )
	            + 0.00650 * sin ( 2.0 * ( ls + gs ) + theta )
	          ) * RAD_PER_DEG;

	*w = Mod2Pi ( *w );	          
	*n = Mod2Pi ( *n );
	*m = Mod2Pi ( lambda - *w );
}

/******************************  PhoebeOrbit  ***************************************

  DESCRIPTION: Computes Phoebe's orbital elements, referred to the B1950 ecliptic.
              
         (jd): Julian date on which to compute elements.
          (a): Receives semimajor axis, in AU.
          (e): Recieves eccentricity.
          (i): Recieves inclination, in radians.
          (w): Recieves longitude of periapse, in radians.
          (n): Recieves longitude of the ascending node, in radians.
          (m): Recieves mean anomaly, in radians.
         
      RETURNS: Nothing.
              
   REFERENCES: Explanatory Supplement to the Astronomical Almanac, pp. 360-368.
  
*************************************************************************************/

void PhoebeOrbit (double jd, double *a, double *e, double *i, double *w, double *n, double *m)
{
	double d, y, lambda;
	
	d = jd - 2433282.5;
	y = d / 365.25;
	lambda = MOD ( 277.872 - 0.6541068 * d );
	
	*a = 0.0865752;
	*e = 0.16326;
	*i = MOD ( 173.949 - 0.020 * y );
	*w = MOD ( 280.165 - 0.19586 * y );
	*n = MOD ( 245.998 - 0.41353 * y );
	*m = Mod2Pi ( lambda - *w );
}

/*******************************  MimasRotation  *****************************

  DESCRIPTION: Computes Mimas's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 410.
     
*******************************************************************************/

void MimasRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, s1, s3, s9;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	s1 = Mod2Pi ( ( 353.32 + 75076.7 * t ) * RAD_PER_DEG );
	s3 = Mod2Pi ( ( 177.40 - 36505.5 * t ) * RAD_PER_DEG );
	s9 = Mod2Pi ( ( 316.45 +   506.2 * t ) * RAD_PER_DEG );
	
	*a0 = ( 40.66 - 0.036 * t + 13.56 * sin ( s3 ) ) * RAD_PER_DEG;
	*d0 = ( 83.52 - 0.004 * t -  1.53 * cos ( s3 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 337.46 + 381.9945550 * d - 13.48 * sin ( s3 )
	    - 44.85 * sin ( s9 ) ) * RAD_PER_DEG );
}

/*******************************  EnceladusRotation  *****************************

  DESCRIPTION: Computes Enceladus's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 410.
     
*******************************************************************************/

void EnceladusRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	*a0 = ( 40.66 - 0.036 * t ) * RAD_PER_DEG;
	*d0 = ( 83.52 - 0.004 * t ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 2.82 + 262.7318996 * d ) * RAD_PER_DEG );
}

/*******************************  TethysRotation  *****************************

  DESCRIPTION: Computes Tethys's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 410.
     
*******************************************************************************/

void TethysRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, s4, s9;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	s4 = Mod2Pi ( ( 300.00 - 7225.9 * t ) * RAD_PER_DEG );
	s9 = Mod2Pi ( ( 316.45 +  506.2 * t ) * RAD_PER_DEG );
	
	*a0 = ( 40.66 - 0.036 * t + 9.66 * sin ( s4 ) ) * RAD_PER_DEG;
	*d0 = ( 83.52 - 0.004 * t - 1.09 * cos ( s4 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 10.45 + 190.6979085 * d - 9.60 * sin ( s4 )
	    + 2.23 * sin ( s9 ) ) * RAD_PER_DEG );
}

/*******************************  DioneRotation  *****************************

  DESCRIPTION: Computes Dione's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 410.
     
*******************************************************************************/

void DioneRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	*a0 = (  40.66 - 0.036 * t ) * RAD_PER_DEG;
	*d0 = (  83.52 - 0.004 * t ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 357.00 + 131.5349316 * d ) * RAD_PER_DEG );
}

/*******************************  RheaRotation  *****************************

  DESCRIPTION: Computes Rhea's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 410.
     
*******************************************************************************/

void RheaRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, s7;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	s7 = Mod2Pi ( ( 345.20 - 1016.3 * t ) * RAD_PER_DEG );
	
	*a0 = (  40.66 - 0.036 * t + 3.10 * sin ( s7 ) ) * RAD_PER_DEG;
	*d0 = (  83.52 - 0.004 * t - 0.35 * cos ( s7 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 235.16 + 79.6900478 * d - 3.08 * sin ( s7 ) ) * RAD_PER_DEG );
}

/*******************************  TitanRotation  *****************************

  DESCRIPTION: Computes Titan's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 410.
     
*******************************************************************************/

void TitanRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t, s8;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	s8 = Mod2Pi ( (  29.80 - 52.1 * t ) * RAD_PER_DEG );
	
	*a0 = (  36.41 - 0.036 * t + 2.66 * sin ( s8 ) ) * RAD_PER_DEG;
	*d0 = (  83.94 - 0.004 * t - 0.30 * cos ( s8 ) ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 189.64 + 22.5769768 * d - 2.64 * sin ( s8 ) ) * RAD_PER_DEG );
}

/*******************************  IapetusRotation  *****************************

  DESCRIPTION: Computes Iapetus's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 410.
     
*******************************************************************************/

void IapetusRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	*a0 = ( 318.16 - 3.949 * t ) * RAD_PER_DEG;
	*d0 = (  75.03 - 1.143 * t ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 350.20 + 4.5379572 * d ) * RAD_PER_DEG );
}

/*******************************  PhoebeRotation  *****************************

  DESCRIPTION: Computes Phoebe's rotational elements.
  
         (jd): Julian date on which to compute elements.
         (a1): Receives J2000 right ascension of north pole, in radians.
         (d1): Receives J2000 declination of north pole, in radians.
         (w1): Receives argument of prime meridian, in radians from 0 to TWO_PI.
          
      RETURNS: Nothing.
      
         NOTE: Uses the formulae of the 1991 IAU working group on cartographic
               and rotational elements.

   REFERENCES: Explanatory Supplement to the Astronomical Almanac, p. 410.
     
*******************************************************************************/

void PhoebeRotation (double jd, double *a0, double *d0, double *w)
{
	double d, t;
	
	d = jd - 2451545.0;
	t = d / 36525.0;

	*a0 =   355.16 * RAD_PER_DEG;
	*d0 = (  68.70 - 1.143 * t ) * RAD_PER_DEG;
	*w  = Mod2Pi ( ( 304.70 + 930.8338720 * d ) * RAD_PER_DEG );
}
