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

/***************************************************************************

	This is an example program which makes use of the functions in the
	AstroLib library to compute an ephemeris of the sun, moon, planets,
	and an asteroid or comet whose orbital elements are specified by the
	user.  It is written entirely in ANSI C, and uses only the standard
	C library functions for console input and output.  It has been tested
	on MacOS (MetroWerks CodeWarrior), Windows 95 (Borland C++), and UNIX
	(UNIX cc), and should be portable to a wide variety of other platforms.
	
	In order to build this program, this source file must be compiled
	and linked with the following source files from the AstroLib library:
	
	MathEx.c
	Time.c
	Angle.c
	CoordSys.c
	Reduce.c
	Orbit.c
	VFPlanet.c
	Physical.c

	In addition, you must create a file called "target.h" and place it in
	your compiler's #include path.  This file contains macros which will
	determine some target-platform-specific compilation options; a complete
	description of the contents required for this file are included in the
	header file "AstroLib.h".

	This example program demonstrates most of the basic capabilities of the
	AstroLib library, such as:
	
	- converting calendar dates to Julian dates and vice-versa;
	- converting angular values from degrees, minutes, and seconds to
	  radians and vice-versa;
	- converting spherical coordinates to rectangular (X-Y-Z) coordinates
	  and vice-versa;
	- converting rectangular coordinates from one coordinate system to
	  another through the use of rotation matrices;
	- computing the positions, magnitudes, angular sizes, and phase angles
	  of the planets at any given time;
	- computing the position and magnitude of an asteroid or comet at any
	  given time from its orbital elements;
	- obtaining an object's apparent coordinates by correcting for
	  precession, nutation, and aberration of light;
	- correcting an object's position for geocentric parallax to compute 
	  its position as seen from any point on the earth's surface;
	- computing an object's altitude and azimuth as seen from any point
	  on the earth's surface, and taking atmospheric refraction into account;
	- computing the time an object rises or sets on any given day, from
	  from any point on the earth's surface.
	
	The source code has been extensively commented, and should be relatively
	straightforward to modify for use in other applications.  This program
	may be freely copied and modified for non-commercial purposes.
	
	While it has been tested succesfully on a number of platforms, this
	program is provided on an "as-is" basis.  Southern Stars Software makes
	no claim for the accuracy of its predictions on any platforms other than
	those tested, and will not be responsible for any damages or losses
	resulting, directly or indirectly, from its use.
	
****************************************************************************/
	 
/*** header files ***/

#include <stdio.h>
#include <math.h>

#include "AstroLib.h"

/*** constants and macros ***/

#define JULIAN		0
#define GREGORIAN	1

#define RISE		-1
#define TRANSIT		0
#define SET			+1

#define SUN			0
#define MERCURY		1
#define VENUS		2
#define MOON		3
#define MARS		4
#define JUPITER		5
#define SATURN		6
#define URANUS		7
#define NEPTUNE		8
#define PLUTO		9
#define COMET		10
#define ASTEROID	11

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#define MAX_ITERATIONS	10
#define PRECISION		(1.0/86400.0)

/*** function prototypes ***/

void	GetLocation ( void );
void	GetObject ( void );
void	GetObjectOrbit ( void );
void	GetEphemerisOptions ( void );
void	ComputeEphemeris ( void );
void	PrintEphemeris ( short, double );
double	ComputeObjectRiseSetTime ( double, double, double, int );
void	RiseSetFunction ( double, double *, double * );

void	ComputeObjectPosition ( short, double, int, int, double, double, double,
		double, double *, double * );
void	ComputeObjectPhysicalPosition ( short, double, double, double * );
void	ComputeObjectPhysicalEphemeris ( short, double *, double *, double *, double *, double * );
void	ComputeOrbitPosition ( double, double, double, double, double,
		double, double, double, double, double, double * );
void	ComputeApparentPosition ( double *, double, double, double,
		int, int, double *, double *, double * );
void	ComputeGeocentricPosition ( double, double, double, double, double, double * );
void	ComputeHorizonCoordinates ( double, double, double, double, double,
		double, int, double *, double * );

/*** global data ***/

double	gLongitude = 0.0;				/* viewer's east longitude, in radians */
double	gLatitude = 0.0;				/* viewer's north latitude, in radians */
double	gTimeZone = 0.0;				/* viewer's time zone, in days east of Greenwich */
double	gAltitude = 0.0;				/* viewer's altitude, in AU */

short	gObject = SUN;					/* object to compute ephemeris for, as #defined above */
double	gOrbitPrecessionEpoch = J2000;	/* precession epoch of orbital elements, as Julian date */
double	gOrbitEpoch = J2000;			/* osculation epoch of orbital elements, as Julian date */	
double	gOrbitPeriDist = 1.0;			/* orbital perihelion distance, in AU */
double	gOrbitEcc = 0.0;				/* orbital eccentricity (dimensionless) */
double	gOrbitIncl = 0.0;				/* orbital inclination to ecliptic, radians */
double	gOrbitArgPeri = 0.0;			/* argument of perihelion, in radians */
double	gOrbitLonAscNode = 0.0;			/* longitude of ascending node, in radians */
double	gOrbitMeanAnom = 0.0;			/* mean anomaly at epoch, in radians */
double	gH = 0.0, gG = 0.0;				/* object's magnitude parameters */

double	gPrecessionEpoch = J2000;		/* output precession epoch, as Julian date */
int		gPrecession = TRUE;				/* if TRUE, use current date as output precession epoch */
int		gNutation = FALSE;				/* if TRUE, correct for nutation */
int		gAberration = FALSE;			/* if TRUE, correct for aberration of light */
int		gTopocentric = TRUE;			/* if TRUE, correct for geocentric parallax */
int		gLightTime = TRUE;				/* if TRUE, correct for light travel time */
int		gRefraction = TRUE;				/* if TRUE, correct for atmospheric refraction */
int		gDynamicTime = FALSE;			/* if TRUE, convert UT to TDT */
int		gCorrectRiseSetTimes = TRUE;	/* if TRUE, correct rise/set times for object motion */
double	gHorizonAltitude = 0.0;			/* horizon altitude for rise/set times, in radians */

/*********************************  main  *********************************************/

int main ( int argc, char *argv[] )
{
	short	option;
	
	printf ( "AstroLib Ephemeris Example Program\n" );
	printf ( "Copyright 1997 by Tim DeBenedictis.  All Rights Reserved.\n" );

	/*** Display the main menu of options, then obtain and perform
	     the option desired by the user. ***/
	     
	while ( TRUE )
	{	
		printf ( "\n1. Enter observer location\n" );
		printf ( "2. Select object for ephemeris computation\n" );
		printf ( "3. Change ephemeris computation options\n" );
		printf ( "4. Compute ephemeris\n" );
		printf ( "5. Quit\n\n" );
		
		do
		{
			printf ( "Enter option (1-5): " );
			scanf ( "%hd", &option );
		}
		while ( option < 1 || option > 5 );
		
		if ( option == 1 )
			GetLocation();
		
		if ( option == 2 )
			GetObject();
			
		if ( option == 3 )
			GetEphemerisOptions();
			
		if ( option == 4 )
			ComputeEphemeris();
		
		if ( option == 5 )
			break;
	}
	
	return ( 0 );
}

/****************************  GetLocation  *************************************/

void GetLocation ( void )
{
	char	degs[5] = { '\0' };
	short	min = 0;
	double	sec = 0.0;
	
	printf ( "\nEast longitude (deg min sec): " );
	scanf ( "%s%hd%lf", degs, &min, &sec );
	gLongitude = DegMinSecToDecimal ( atol ( degs ), min, sec, degs[0] ) / DEG_PER_RAD;
	
	printf ( "North latitude (deg min sec): " );
	scanf ( "%s%hd%lf", degs, &min, &sec );
	gLatitude = DegMinSecToDecimal ( atol ( degs ), min, sec, degs[0] ) / DEG_PER_RAD;
	
	printf ( "Altitude (meters): " );
	scanf ( "%lf", &gAltitude );
	gAltitude = gAltitude / 1000.0 / KM_PER_AU;

	printf ( "Time zone (hrs east of Greenwich): " );
	scanf ( "%lf", &gTimeZone );
	gTimeZone = gTimeZone / 24.0;
}

/******************************  GetObject  ***********************************/

void GetObject ( void )
{
	printf ( "\n0. Sun\n" );
	printf ( "1. Mercury\n" );
	printf ( "2. Venus\n" );
	printf ( "3. Moon\n" );
	printf ( "4. Mars\n" );
	printf ( "5. Jupiter\n" );
	printf ( "6. Saturn\n" );
	printf ( "7. Uranus\n" );
	printf ( "8. Neptune\n" );
	printf ( "9. Pluto\n" );
	printf ( "10. Comet\n" );
	printf ( "11. Asteroid\n\n" );
	
	do
	{
		printf ( "Compute ephemeris for which object? (0-11): " );
		scanf ( "%hd", &gObject );
	}
	while ( gObject < SUN || gObject > ASTEROID );
	
	if ( gObject > PLUTO )
		GetObjectOrbit(); 
}

/*************************  GetObjectOrbit  ***********************************/

void GetObjectOrbit ( void )
{
	long	year = 2000;
	short	month = 1;
	double	day = 1.5;
	
	/*** Get the precession epoch of the orbital elements, and the
	     osculation epoch.  Convert both to Julian dates. ***/
	     
	printf ( "\nPrecession epoch of orbital elements (year): " );
	scanf ( "%lf", &gOrbitPrecessionEpoch );
	gOrbitPrecessionEpoch = JulianYearToJD ( gOrbitPrecessionEpoch );
	
	printf ( "Epoch of orbital elements (year/month/day): " );
	scanf ( "%ld/%hd/%lf", &year, &month, &day );
	gOrbitEpoch = DateTimeToJD ( year, month, day, 0, 0, 0.0, 0.0, GREGORIAN );

	/*** Get the orbital eccentricity. ***/
	
	printf ( "Eccentricity: " );
	scanf ( "%lf", &gOrbitEcc ); 

	/*** For asteroids, we are usually given the semimajor axis.
	     Convert this to the perihelion distance.  For comets,
	     the perihelion distance is usually listed directly. ***/
	     
	if ( gObject == ASTEROID )
	{
		printf ( "Semimajor axis (AU): " );
		scanf ( "%lf", &gOrbitPeriDist );
		gOrbitPeriDist = gOrbitPeriDist * ( 1.0 - gOrbitEcc );
	}
	
	if ( gObject == COMET )
	{
		printf ( "Perihelion distance (AU): " );
		scanf ( "%lf", &gOrbitPeriDist );
	}
	
	/*** Get the inclination, argument of perihelion, and longitude
	     of the ascending node; convert them to radians. ***/
	     
	printf ( "Inclination (degrees): " );
	scanf ( "%lf", &gOrbitIncl );
	gOrbitIncl = gOrbitIncl / DEG_PER_RAD;
	
	printf ( "Argument of perihelion (degrees): " );
	scanf ( "%lf", &gOrbitArgPeri );
	gOrbitArgPeri = gOrbitArgPeri / DEG_PER_RAD;
	
	printf ( "Longitude of ascending node (degrees): " );
	scanf ( "%lf", &gOrbitLonAscNode );
	gOrbitLonAscNode = gOrbitLonAscNode / DEG_PER_RAD;
	
	/*** For asteroids, the mean anomaly at the epoch of the
	     orbital elements is usually given directly.  For comets,
	     it's a little trickier because we are usually given the
	     date of perihelion passage instead of the mean anomaly
	     at epoch.  However, we can compute the mean anomaly by
	     computing the mean rate of motion and multiplying it by
	     the time between the epoch and the perihelion date. ***/
	
	if ( gObject == ASTEROID )
	{
		printf ( "Mean anomaly at epoch (degrees): " );
		scanf ( "%lf", &gOrbitMeanAnom );
		gOrbitMeanAnom = gOrbitMeanAnom / DEG_PER_RAD;
	}
	
	if ( gObject == COMET )
	{
		printf ( "Perihelion date (year/month/day): " );
		scanf ( "%ld/%hd/%lf", &year, &month, &day );

		gOrbitMeanAnom = MeanMotion ( HELIO_GAUSS_CONST * HELIO_GAUSS_CONST, gOrbitPeriDist, gOrbitEcc )
		               * ( gOrbitEpoch - DateTimeToJD ( year, month, day, 0, 0, 0.0, 0.0, GREGORIAN ) );
	}
	
	/*** Obtain the magnitude parameters. ***/
	
	printf ( "Magnitude parameters (H G): " );
	scanf ( "%lf%lf", &gH, &gG );
}

/*************************  GetEphemerisOptions  *****************************/

void GetEphemerisOptions ( void )
{
	char answer[4];
	
	/*** Determine whether objects' equatorial coordinates should
	     be given for the same epoch at which they were computed. ***/
	     
	printf ( "\nCorrect positions for precession? (y/n): " );
	scanf ( "%s", answer );
	if ( answer[0] == 'y' )
		gPrecession = TRUE;
	else
		gPrecession = FALSE;
	
	/*** If not, determine the precession epoch for which objects'
	     equatorial coordinates will be computed. ***/
	     
	if ( gPrecession == FALSE )
	{
		printf ( "Precession epoch (year): " );
		scanf ( "%lf", &gPrecessionEpoch );
		gPrecessionEpoch = JulianYearToJD ( gPrecessionEpoch );
	}
	
	/*** Determine whether objects' equatorial coordinates should
	     be corrected for nutation. ***/
	
	printf ( "Correct positions for nutation? (y/n): " );
	scanf ( "%s", answer );
	if ( answer[0] == 'y' )
		gNutation = TRUE;
	else
		gNutation = FALSE;
	
	/*** Determine whether objects' equatorial coordinates should be
	     corrected for aberration of light. ***/
	     	
	printf ( "Correct positions for aberration of light? (y/n): " );
	scanf ( "%s", answer );
	if ( answer[0] == 'y' )
		gAberration = TRUE;
	else
		gAberration = FALSE;
	
	/*** Determine whether objects' positions should be corrected for
	     light time. ***/
	     
	printf ( "Correct positions for light time? (y/n): " );
	scanf ( "%s", answer );
	if ( answer[0] == 'y' )
		gLightTime = TRUE;
	else
		gLightTime = FALSE;

	/*** Determine whether objects' positions should be corrected for
	     geocentric parallax. ***/
	     
	printf ( "Correct positions for geocentric parallax? (y/n): " );
	scanf ( "%s", answer );
	if ( answer[0] == 'y' )
		gTopocentric = TRUE;
	else
		gTopocentric = FALSE;

	/*** Determine whether objects' altitudes should be corrected for
	     atmospheric refraction. ***/
	     
	printf ( "Correct altitudes for atmospheric refraction? (y/n): " );
	scanf ( "%s", answer );
	if ( answer[0] == 'y' )
		gRefraction = TRUE;
	else
		gRefraction = FALSE;

	/*** Determine whether time should be converted from UT to TDT
	     before computing objects' positions. ***/
	
	printf ( "Convert UT to TDT when computing positions? (y/n): " );
	scanf ( "%s", answer );
	if ( answer[0] == 'y' )
		gDynamicTime = TRUE;
	else
		gDynamicTime = FALSE;

	/*** Determine whether objects' rise/set times should be corrected
	     for their daily motion. ***/
	
	printf ( "Correct rise/set times for planetary motion? (y/n): " );
	scanf ( "%s", answer );
	if ( answer[0] == 'y' )
		gCorrectRiseSetTimes = TRUE;
	else
		gCorrectRiseSetTimes = FALSE;
	
	/*** Determine the horizon altitude to use for rise/set times. ***/
	
	printf ( "Horizon altitude to use for rise/set times (degrees): " );
	scanf ( "%lf", &gHorizonAltitude );
	gHorizonAltitude = gHorizonAltitude / DEG_PER_RAD;
}

/**************************  ComputeEphemeris  *******************************/

void ComputeEphemeris ( void )
{
	long	year = 2000;
	short	month = 1;
	double	day = 1.5;
	short	hour = 0;
	short	min = 0;
	double	sec = 0.0;
	double	start, end, step, jd;
	
	printf ( "\nStart date and time (year/month/day hour:min:sec): " );
	scanf ( "%ld/%hd/%lf %hd:%hd:%lf", &year, &month, &day, &hour, &min, &sec );
	start = DateTimeToJD ( year, month, day, hour, min, sec, gTimeZone, GREGORIAN );
	
	printf ( "End date and time (year/month/day hour:min:sec): " );
	scanf ( "%ld/%hd/%lf %hd:%hd:%lf", &year, &month, &day, &hour, &min, &sec );
	end = DateTimeToJD ( year, month, day, hour, min, sec, gTimeZone, GREGORIAN );
	
	printf ( "Time step (days): " );
	scanf ( "%lf", &step );
	
	PrintEphemeris ( -1, start );
	
	for ( jd = start; jd <= end; jd += step )
		PrintEphemeris ( gObject, jd );
}

/**************************  PrintEphemeris  *******************************/

void PrintEphemeris ( short object, double jd )
{
	char	sign;
	long	year;
	short	month, hour, min, rah, ram, decd, decm;
	double	day, sec, ras, decs;
	double	jdt, epoch, distance;
	double	ra, dec, size, phase, mag, azm, alt;
	double	position[3];
	double	direction[3];
	
	/*** If we are given an invalid object number, print ephemeris
	     column headings. ***/
	     
	if ( object < SUN )
	{
		printf ( "\n%-10s ", "Date" );
		printf ( "%-8s\t", "Time" );
		printf ( "%-10s\t", "RA" );
		printf ( "%-9s\t", "Dec" );
		printf ( "%-8s\t", "Dist" );
		printf ( "%-6s\t", "Size" );
		printf ( "%-5s\t", "Illum" );
		printf ( "%-5s\t", "Mag" );
		printf ( "%-7s\t", "Azm" );
		printf ( "%-7s\t", "Alt" );
		printf ( "%-8s\t", "Rises" );
		printf ( "%-8s\n", "Sets" );
		
		return;
	}
	
	/*** Compute and display the local date and time ***/
	
	JDToDateTime ( jd, gTimeZone, &year, &month, &day, &hour, &min, &sec, GREGORIAN );
	printf ( "%04ld/%02hd/%02.0f ", year, month, floor ( day ) );
	printf ( "%02hd:%02hd:%02.0f\t", hour, min, sec );
	
	/*** If we are correcting for the difference between universal
	     and dynamic time, convert the time from UT to TDT. ***/
	
	if ( gDynamicTime )
		jdt = jd + DeltaT ( jd ) / 86400.0;  /* CHECK SIGN HERE! */
	else
		jdt = jd;
		
	/*** If we are precessing coordinates to the current epoch, use
	     the current time as the output coordinate epoch. ***/

	if ( gPrecession )
		epoch = jdt;
	else
		epoch = gPrecessionEpoch;
	
	/*** Compute the object's position vectors relative to the sun
	     and viewer in the J2000 equatorial frame.  Use these to compute
	     the object's size, phase, and magnitude.  Then obtain the object's
	     equatorial coordinates and distance at the desired output precession
	     epoch, optionally correcting for nutation and aberration of light as well. ***/
	
	ComputeObjectPosition ( object, jd, gLightTime, gTopocentric,
	gLongitude, gLatitude, gAltitude, J2000, position, direction );
	
	ComputeObjectPhysicalEphemeris ( object, position, direction,
	&size, &phase, &mag );
	
	ComputeApparentPosition ( direction, jd, J2000, epoch, gNutation,
	gAberration, &ra, &dec, &distance );

	/*** Display the object's position, distance, size, illumination,
	     and magnitude. ***/
	     
	DecimalToDegMinSec ( ra * DEG_PER_RAD / 15.0, &rah, &ram, &ras, &sign );
	DecimalToDegMinSec ( dec * DEG_PER_RAD, &decd, &decm, &decs, &sign );
	
	printf ( "%02hd %02hd %04.1f\t", rah, ram, ras );
	printf ( "%c%02hd %02hd %02.0f\t", sign, decd, decm, decs );
	printf ( "%8.5f\t", distance );
	printf ( "%6.1f\t", size * ARCSEC_PER_RAD );
	printf ( "%5.3f\t", IlluminatedFraction ( phase ) );
	printf ( "%+5.1f\t", mag );
	
	/*** Compute and display the object's horizon coordinates ***/
	
	ComputeHorizonCoordinates ( gLongitude, gLatitude, jd, epoch,
	ra, dec, gRefraction, &azm, &alt );

	printf ( "%07.3f\t", azm * DEG_PER_RAD );
	printf ( "%+07.3f\t", alt * DEG_PER_RAD );
	
	/*** Compute and display the object's time of rising. ***/
	
	jdt = ComputeObjectRiseSetTime ( jd, ra, dec, RISE );
	if ( jdt == INFINITY )
		printf ( "%-8s\t", "No Set" );
	else if ( jdt == -INFINITY )
		printf ( "%-8s\t", "No Rise" );
	else
	{	
		JDToDateTime ( jdt, gTimeZone, &year, &month, &day, &hour, &min, &sec, GREGORIAN );
		printf ( "%02hd:%02hd:%02.0f\t", hour, min, sec );
	}
	
	/*** Compute and display the object's time of setting. ***/
	
	jdt = ComputeObjectRiseSetTime ( jd, ra, dec, SET );
	if ( jdt == INFINITY )
		printf ( "%-8s\n", "No Set" );
	else if ( jdt == -INFINITY )
		printf ( "%-8s\n", "No Rise" );
	else
	{	
		JDToDateTime ( jdt, gTimeZone, &year, &month, &day, &hour, &min, &sec, GREGORIAN );
		printf ( "%02hd:%02hd:%02.0f\n", hour, min, sec );
	}
}

/***********************  RiseSetFunction  ******************************

	This is a callback function used by RiseSetTimeSearch.  It
	computes the right ascension and declination of the currently-
	selected object at the given Julian date, taking into acccount
	the currently-selected options for precession, nutation,
	aberration of light, light time, and geocentric parallax.
	
*************************************************************************/

void RiseSetFunction ( double jd, double *ra, double *dec )
{
	double position[3], direction[3];
	
	ComputeObjectPosition ( gObject, jd, gLightTime, gTopocentric,
	gLongitude, gLatitude, gAltitude, J2000, position, direction );
	
	ComputeApparentPosition ( direction, jd, J2000, gPrecessionEpoch,
	gNutation, gAberration, ra, dec, NULL );
}

/************************  ComputeObjectRiseSetTime  *************************/

double ComputeObjectRiseSetTime ( double jd, double ra, double dec, int sign )
{
	/*** If we want to compute correct rise/set times, taking the object's
	     motion during the day in question into considersation, we need to
	     search for the rise/set time iteratively.  Otherwise, we'll compute
	     the rise/set time assuming the object's position stays fixed on
	     the day in question. ***/ 
	     
	if ( gCorrectRiseSetTimes )
		jd = DailyRiseSetTimeSearch ( RiseSetFunction, jd, sign,
		     gTimeZone,  gLongitude, gLatitude, gHorizonAltitude,
		     PRECISION, MAX_ITERATIONS );
	else	
		jd = RiseSetTime ( ra, dec, jd, sign, gLongitude, gLatitude,
		gHorizonAltitude );
		     
	return ( jd );
}

/**********************  ComputeObjectPosition  **************************

	Computes a solar system object's position relative to both the sun
	and the viewer in the equatorial coordinate system at any given time.
	
	void ComputeObjectPosition ( short object, double jd, int lighttime,
	     int topocent, double lon, double lat, double alt, double epoch,
	     double position[3], double direction[3] )
	
	(object):    object identifier, as #defined above.
	(jd):        Julian date at which to compute object's position.
	(lighttime): if TRUE, correct for light time.
	(topocent):  if TRUE, compute topocentric; if FALSE, geocentric.
	(lon):       viewer's east longitude, in radians.
	(lat):       viewer's north latitude, in radians.
	(alt):       viewer's altitude, in AU.
	(epoch):     precession epoch for which positions should be computed.
	(position):  receives object's heliocentric position vector.
	(direction): recieves object's position vector relative to viewer.
	
	The position vectors computed by this function will both be computed
	in the equatorial coordinate system at the precession epoch specified
	in the (epoch) parameter as a Julian date.
	
	The (position) vector receieves the object's heliocentric position
	in units of AU.  The (direction) vector receives the object's position
	vector relative to the viewer in units of AU.
	
	If the flag (lighttime) is TRUE, the object's position and direction
	vectors will be antedated for light time.  In other words, they
	will be computed for the time the light visible at Julian date (jd)
	left the object, rather than at (jd) itself.  If this flag is set to
	FALSE, the position vectors will be computed for Julian date (jd)
	directly.

	If the flag (topocent) is TRUE, the object's position will be
	computed for a viewer located on the Earth's surafce at the longitude, 
	latitude, and altitude specified in the parameters (lon), (lat), and
	(alt).  If this flag is FALSE, the object's relative position vector
	will be geocentric rather than topocentric, i.e. relative to the Earth's
	center rather than to the viewer's position on its surface.
	
***************************************************************************/

void ComputeObjectPosition ( short object, double jd, int lighttime,
int topocent, double lon, double lat, double alt,
double epoch, double position[3], double direction[3] )
{
	double	sun[3], viewer[3] = { 0.0, 0.0, 0.0 };
	double	oldtau = 0.0, tau = 0.0;
	
	/*** If we're computing the object's topocentric direction
	     (i.e. direction as seen from the viewer's position on the
	     earth's surface), compute the viewer's geocentric position
	     (i.e. position relative to the earth's center.)  Otherwise,
	     leave the viewer's geocentric position as zero (i.e. the
	     viewer is at the center of the earth.) ***/
	     
	if ( topocent )
		ComputeGeocentricPosition ( lon, lat, alt, jd, epoch, viewer );
	
	/*** For all objects but the sun and moon, the sun is the origin
	     of the coordinate system.  Compute the viewer's heliocentric
	     position (i.e. position relative to the sun) by computing the
	     sun's geocentric position, then subtracting it from the
	     viewer's geocentric position. ***/
	     
	if ( object != SUN && object != MOON )
	{
		ComputeObjectPhysicalPosition ( SUN, jd, epoch, sun );
		VectorDifference ( viewer, sun, viewer );
	}
	
	/*** Compute the planet's position antedated for light time: i.e.,
	     its position at the time the light we see now left it.  The
	     planet appears to be there, rather then where it actually is
	     right now.   This is an iterative process, because at first
	     the planet's distance, and hence light time, is unknown!
	     Initially, assume that the light time is zero.  Then, on each
	     iteration, compute the planet's position antedated by the current
	     estimate of the light time, then subtract the viewer's position
	     relative to the origin to obtain the planet's position relative
	     to the viewer.  From this, determine the distance to the planet,
	     and generate an improved estimate of the light time to use on the
	     next interation.  We stop when the estimate of the light time
	     converges to a precision of one second (in practice this should
	     happen on the 3rd or 4th iteration at most). ***/
	     
	do
	{
		ComputeObjectPhysicalPosition ( object, jd - tau, epoch, position );
		VectorDifference ( position, viewer, direction );

		oldtau = tau;
		tau = VectorMagnitude ( direction ) * LIGHT_DAYS_PER_AU;
	}
	while ( lighttime && fabs ( tau - oldtau ) > 1.0 / SEC_PER_DAY );
	
	/*** For the sun and moon, compute the object's position vector
	     relative to the sun by computing the sun's geocentric position
	     antedated for light-time and subtracting it from the object's
	     (which we have computed above).  Note that for all other objects,
	     we have already computed the position relative to the sun. ***/
	
	if ( object == SUN || object == MOON )
	{
		ComputeObjectPhysicalPosition ( SUN, jd - oldtau, epoch, sun );
		VectorDifference ( position, sun, position );
	}
}

/***********************  ComputeApparentPosition  *****************************

	Given an object's relative position vector, computes the object's equatorial
	coordinates and distance, optionally correcting for precession, nutation,
	and aberration of light.
	
	void ComputeApparentPosition ( double direction[3], double jd,
	     double epoch0, double epoch1, int nutation, int aberration,
	     double *ra, double *dec, double *distance )
	     
	(position):  object's equatorial position vector relative to viewer.
	(jd):        Julian date at which object's position is computed.
	(epoch0):    precession epoch of input position vector.
	(epoch1):    desired precession epoch of output coordinates.
	(nutat):     if TRUE, correct for nutation.
	(aberr):     if TRUE, correct for aberration of light.
	(ra):        receives object's right ascension, in radians.
	(dec):       receives object's declination, in radians.
	(distance):  receives object's distance.
	
	The input position vector (position) gives an object's position relative
	to the viewer at the Julian date specified in the parameter (jd).  The
	position is given as a vector in the equatorial coordinate frame of the
	mean (i.e. uncorrected for nutation) precessional epoch specified as a
	Julian date in the parameter (epoch0).
	
	The parameter (epoch1) specifies the output precessional epoch for which 
	the object's equatorial coordinates are desired, again as a Julian date.
	The flag (nutat) determines whether or not the object's position should
	be corrected for nutation at the output precession epoch; the (aberr) flag
	determines whether or not the object's position should be corrected for
	aberration of light.
	
	Finally, the parameters (ra) and (dec) recieve the object's equatorial
	coordinates, expressed in the precession epoch specified by paratmer
	(epoch1), and optionally corrected for nutation and aberration of light.
	The parameter (distance) receives the object's distance, in the same
	units that the original vector was expressed in.  If you wish to ignore
	the distance, you may pass NULL for this parameter.
	
***********************************************************************************/

void ComputeApparentPosition ( double position[3], double jd, double epoch0,
double epoch1, int nutation, int aberration, double *ra, double *dec, double *distance )
{
	double velocity[3], matrix[3][3];
	
	/*** If desired, correct the input direction vector for the effects
	     of aberration of light. ***/
	     
	if ( aberration )
	{
		/*** Compute the earth's heliocentric velocity vector and convert it
		     from units of AU/day to a fraction of the speed of light.  The
		     resulting vector is in the coordinate frame defined by the earth's
		     equator at epoch J2000 in units of AU/day. ***/
		     
		EarthVelocity ( jd, &velocity[0], &velocity[1], &velocity[2] );
		ScaleVector ( velocity, LIGHT_DAYS_PER_AU );
		
		/*** If the object's direction vector is in the equatorial frame of
		     some other epoch, compute a rotation matrix to transform the
		     velocity vector to the equatorial frame at that other epoch. ***/
		
		if ( epoch0 != J2000 )
		{
			SetPrecessionRotationMatrix ( matrix, J2000, epoch0, FALSE );
			TransformVector ( matrix, velocity );
		}
		
		/*** Correct the object's apparent direction vector for aberration
		     of light by applying the earth's velocity vector. ***/
		
		RelativisticAberration ( position, velocity, position );
	}

	/*** If we want to precess the object's coordinates to some other epoch
	     or correct for nutation, compute the rotation matrix which will
	     transform the position vector from the equatorial frame at the
	     input epoch to the equatorial frame at the output epoch.  Then
	     use it to transform the vector to the desired precession epoch. ***/
	     
	if ( epoch1 != epoch0 || nutation )
	{
		SetPrecessionRotationMatrix ( matrix, epoch0, epoch1, nutation );
		TransformVector ( matrix, position );
	}
	
	/*** Finally, convert the direction vector from rectangular to spherical
	     coordinates. ***/
	     
	XYZToSpherical ( position[0], position[1], position[2], ra, dec, distance );
}

/********************  ComputeObjectPhysicalEphemeris  ************************

	Computes a solar system object's angular size, phase angle, and
	apparent magnitude.
	
	void ComputeObjectPhysicalEphemeris ( short object, double sun[3],
	     double viewer[3], double *size, double *phase, double *mag )
	
	(object):   object identifier, as #defined above.
	(sun):      object's position vector relative to sun.
	(viewer):   object's position vector relative to viewer.
	(size):     receives object's angular diameter.
	(phase):    receives object's phase angle.
	(mag):      receives object's apparent magnitude.
	
	The object's position vectors relative to the sun and viewer 
	
********************************************************************************/

void ComputeObjectPhysicalEphemeris ( short object, double sun[3], double viewer[3],
double *size, double *phase, double *mag )
{
	double	sundist, viewerdist;
	
	/*** Compute the object's distance from the sun and viewer. ***/
	
	sundist = VectorMagnitude ( sun );
	viewerdist = VectorMagnitude ( viewer );

	/*** If both distances are greater than zero, compute the object's
	     phase angle from the dot product of the sun-to-object and
	     viewer-to-object vectors.  The phase angle is the angle between
	     the sun and the viewer as seen from the object; the dot product
	     of any two vectors is the product of their magnitudes and the
	     cosine of the angle between them.  Technically we should take
	     the dot poduct of the object-to-sun vector and object-to-viewer
	     vectors rather than the sun-to-object and viewer-to-object vectors,
	     but mathematically the computations are equivalent because one
	     vector is the negative of the other, and the minus signs cancel
	     out when we multiply them. ***/
	     
	if ( sundist > 0.0 && viewerdist > 0.0 )
	{
		*phase = DotProduct ( sun, viewer ) / ( sundist * viewerdist );
		if ( *phase <= 1.0 )
			*phase = acos ( *phase );
	}
	
	/*** Now that we have computed the object's distances and phase
	     angle, we can compute is apparent magnitude. ***/
	     
	if ( object == SUN )
		*mag = SunMagnitude ( viewerdist );
	else if ( object == MERCURY )
		*mag = MercuryMagnitude ( sundist, viewerdist, *phase );
	else if ( object == VENUS )
		*mag = VenusMagnitude ( sundist, viewerdist, *phase );
	else if ( object == MOON )
		*mag = MoonMagnitude ( sundist, viewerdist, *phase );
	else if ( object == MARS )
		*mag = MarsMagnitude ( sundist, viewerdist, *phase );
	else if ( object == JUPITER )
		*mag = JupiterMagnitude ( sundist, viewerdist, *phase );
	else if ( object == SATURN )
		*mag = SaturnMagnitude ( sundist, viewerdist, *phase,
		       SaturnRingPlaneInclination ( viewer, viewerdist ) );
	else if ( object == URANUS )
		*mag = UranusMagnitude ( sundist, viewerdist, *phase );
	else if ( object == NEPTUNE )
		*mag = NeptuneMagnitude ( sundist, viewerdist, *phase );
	else if ( object == PLUTO )
		*mag = PlutoMagnitude ( sundist, viewerdist, *phase );
	else if ( object == ASTEROID )
		*mag = AsteroidMagnitude ( sundist, viewerdist, *phase, gH, gG );	
	else if ( object == COMET )
		*mag = CometMagnitude ( sundist, viewerdist, gH, gG );
	else
		*mag = 0.0;
		
	/*** Compute the object's apparent angular size. ***/
	
	if ( object == SUN )
		*size = AngularSize ( 1922.4 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == MERCURY )
		*size = AngularSize ( 6.743 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == VENUS )
		*size = AngularSize ( 16.6754 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == MOON )
		*size = AngularSize ( 4.792536 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == MARS )
		*size = AngularSize ( 9.3796 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == JUPITER )
		*size = AngularSize ( 196.7004 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == SATURN )
		*size = AngularSize ( 165.6566 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == URANUS )
		*size = AngularSize ( 70.9098 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == NEPTUNE )
		*size = AngularSize ( 66.838 / ARCSEC_PER_RAD, viewerdist );
	else if ( object == PLUTO )
		*size = AngularSize ( 3.844 / ARCSEC_PER_RAD, viewerdist );
	else
		*size = 0.0;
}

/***********************  ComputeHorizonCoordinates  ************************

	Given an object's equatorial coordinates, computes the object's altitude
	and azimuth at any time for any location on the earth's surface, optionally
	correcting for atmospheric refraction.

	void ComputeHorizonCoordinates ( double lon, double lat, double jd,
	     double ra, double dec, double *azm, double *alt )
	
	(lon):     observer's east longitude, in radians.
	(lat):     observer's north latitude, in radians.
	(jd):      Julian date at which to compute object's position.
	(epoch):   precessional epoch of object's right ascension and declination.
	(ra):      object's right ascension, in radians.
	(dec):     object's declination, in radians.
	(refract): if TRUE, correct altitude for atmospheric refraction.
	(azm):     receives object's azimuth.
	(alt):     receives object's altitude.
	
	The function computes the object's azimuth and altitude at the specified
	location and time.  The object's equatorial coordinates (ra and dec)
	should be given for the precessional epoch specified in the (epoch)
	parameter as a Julian date.  If this epoch differs from the Julian
	date at which the altitude and azimuth should be computed (jd), this
	function will precess the coordinates to the epoch of computation (jd).
	
*****************************************************************************/

void ComputeHorizonCoordinates ( double lon, double lat, double jd, double epoch,
double ra, double dec, int refract, double *azm, double *alt )
{
	double lst, vector[3], matrix[3][3];
	
	/*** Transform the equatorial coordinates supplied from spherical
	     coordinates to a rectangular coordinate vector. ***/
	     
	SphericalToXYZ ( ra, dec, 1.0, &vector[0], &vector[1], &vector[2] );

	/*** If the equatorial coordinates' precession epoch differs from
	     the time at which we are computing the horizon coordinates,
	     compute the rotation matrix needed to transform the coordinates
	     from the equatorial frame at the original epoch to the equatorial
	     frame at the time of computation.  Then use it to precess the
	     coordinates to the equatorial frame at the epoch of computation. ***/
	     
	if ( epoch != jd )
	{
		SetPrecessionRotationMatrix ( matrix, epoch, jd, TRUE );
		TransformVector ( matrix, vector );
	}
		
	/*** Determine the local sidereal time.  Use this and the observer's
	     latitude to compute a rotation matrix which will convert coordinates
	     from the equatorial frame to the local horizon frame at the
	     observer's location at the specified Julian date.  Then transform
	     the coordinate vector from the equatorial frame to the local
	     horizon frame using the rotation matrix. ***/
	     
	lst = LocalSiderealTime ( jd, lon );
	SetHorizonRotationMatrix ( matrix, lst, lat, +1 );
	TransformVector ( matrix, vector );
	
	/*** Finally, transform the resulting vector from rectangular coordinates
	     back to spherical coordinates.  The result is the object's altitude
	     and azimuth as seen from the observer's location. ***/
	       
	XYZToSpherical ( vector[0], vector[1], vector[2], azm, alt, NULL );

	/*** If we want to correct the object's altitude for atmospheric
	     refraction, subtract the refraction angle from the object's
	     true altitude.  Here we assume an atmospheric pressure of 1000
	     millibars, and a temperature of zero degrees Centigrade. ***/
	     
	if ( refract )
		*alt = *alt - RefractionAngle ( *alt, 1000.0, 0.0 );	
}

/*****************  ComputeObjectPhysicalPosition  *********************

	Computes a solar system object's physical position at any given time
	in the equatorial coordinate system.

	void ComputeObjectPhysicalPosition ( short object, double jd, double epoch,
	     double position[3] )
	
	(object):   object identifier, as #defined above.
	(jd):       Julian date at which to compute object's position.
	(epoch):    precession epoch of equatorial coordinate system.
	(position): vector to recieve object's position.
	
	On return, the object's position at the specified Julian date (jd)
	will be returned in the vector (position).  The position vector will
	be given in units of AU, in the equatorial coordinate system of the
	precessional epoch specified in the parameter (epoch), as a Julian
	date.
	
	For the sun and moon, the position vector returned will be geocentric
	(relative to the center of the earth).  For all other objects, the
	position vector returned will be heliocentric (relative to the center
	of the sun).
	
**************************************************************************/

void ComputeObjectPhysicalPosition ( short object, double jd, double epoch,
double position[3] )
{
	double lon, lat, dist, matrix[3][3];

	/*** For asteroids and comets, compute the object's position
	     using its heliocentric orbital elements. ***/
	     
	if ( object == ASTEROID || object == COMET )
	{
		ComputeOrbitPosition ( gOrbitEpoch, gOrbitPeriDist, gOrbitEcc,
		                       gOrbitIncl, gOrbitArgPeri, gOrbitLonAscNode,
		                       gOrbitMeanAnom, gOrbitPrecessionEpoch,
		                       jd, epoch, position );
		return;
	}
	
	/*** Compute the object's ecliptic longitude, latitude, and distance
	     from the sun (or earth, in the case of the sun and moon). ***/
	     
	if ( object == SUN )
		Sun ( jd, &lon, &lat, &dist );
	else if ( object == MERCURY )
		Mercury ( jd, &lon, &lat, &dist );
	else if ( object == VENUS )
		Venus ( jd, &lon, &lat, &dist );
	else if ( object == MOON )
		Moon ( jd, &lon, &lat, &dist );
	else if ( object == MARS )
		Mars ( jd, &lon, &lat, &dist );
	else if ( object == JUPITER )
		Jupiter ( jd, &lon, &lat, &dist );
	else if ( object == SATURN )
		Saturn ( jd, &lon, &lat, &dist );
	else if ( object == URANUS )
		Uranus ( jd, &lon, &lat, &dist );
	else if ( object == NEPTUNE )
		Neptune ( jd, &lon, &lat, &dist );
	else if ( object == PLUTO )
		Pluto ( jd, &lon, &lat, &dist );
	
	/*** For the moon, the formulae give the distance from the earth
	     in earth-radii; convert this to AU for consistency. ***/
	     
	if ( object == MOON )
		dist = dist * KM_PER_EARTH_RADII / KM_PER_AU;
	
	/*** Convert the ecliptic longitude, latitude, and distance to
	     a rectangular (i.e. x-y-z) coordinate vector.  Then compute
	     a rotation matrix to transform this vector from the ecliptic
	     coordinate system to the equatorial system, and transform
	     the vector to equatorial coordinates. ***/
		
	SphericalToXYZ ( lon, lat, dist, &position[0], &position[1], &position[2] );
	SetEclipticRotationMatrix ( matrix, Obliquity ( jd ), -1 );
	TransformVector ( matrix, position );
	
	/*** The planet formulae give the planet's position relative
	     to the ecliptic at the epoch (i.e. time) of computation.
	     If we want its position at another epoch (e.g. 1950.0 or
	     2000.0), compute a rotation matrix which will convert
	     its coordinates from the equatorial system at the epoch
	     of computation to the equatorial system at the desired
	     output epoch.  Then transform the planet's position vector
	     to the equatorial system at the output epoch. ***/
	
	if ( epoch != jd )
	{
		SetPrecessionRotationMatrix ( matrix, jd, epoch, FALSE );
		TransformVector ( matrix, position );
	}
}

/**************************  ComputeOrbitPosition  ***************************

	Computes an object's heliocentric position in the equatorial coordinate
	system at any given time from its heliocentric ecliptic orbital elements.
	
	void ComputeOrbitPosition ( double t, double q, double e, double i,
	     double w, double n, double m, double ecliptic, double jd,
	     double equator, double position[3] )
	
	(t):        epoch of orbital elements, as a Julian date.
	(q):        perihelion distance, in AU.
	(e):        orbital eccentricity.
	(i):        inclination of orbital plane to ecliptic plane, in radians.
	(w):        argument of perihelion, in radians.
	(n):        longitude of ascending node, in radians.
	(m):        mean anomaly at epoch (t), in radians.
	(ecliptic): precession epoch of ecliptic orbital elements, as a JD.
	(jd):       date at which to compute object's position, as a JD.
	(equator):  precession epoch of equatorial coordinate system, as a JD.
	(position): vector to recieve object's equatorial coordinates.
	
	On return, the object's position at the specified Julian date (jd)
	will be returned in the vector (position).  The position vector will
	be given in units of AU, in the equatorial coordinate system of the
	precessional epoch specified in the parameter (equator) as a Julian
	date.

	The orbital elements which define the size and shape of the orbit are
	the perihelion distance (q) and eccentricity (e).  The inclination (i)
	and longitude of the ascending node (n) define the orientation of the
	orbital plane relative to the ecliptic plane (i.e. the plane of the
	earth's orbit, commonly used as the reference plane for orbits of other
	solar system objects).  The argument of perihelion (w) gives the location
	of the perihelion along the orbit, relative to the point where the orbit
	crosses the ecliptic plane.  The mean anomaly (m) describes the position
	of the object in its orbit relative to its perihelion passage at the
	epoch of the orbital elements (t).
	
	The parameter (ecliptic) specifies the precessional epoch, as a Julian
	date, of the ecliptic to which the angular orbital elements (i) and (n)
	are referred.
	
******************************************************************************/

void ComputeOrbitPosition ( double t, double q, double e, double i, double w,
double n, double m, double ecliptic, double jd, double equator, double position[3] )
{
	double	mdot, matrix[3][3];
	
	/*** Compute the object's mean motion (i.e. the average angular rate
	     at which it travels around the sun).  Use this to compute the
	     object's mean anomaly at the current time.  This is the angular
	     distance that the object would be from the perihelion point
	     along its orbit if it always travelled at the average rate. ***/

	mdot = MeanMotion ( HELIO_GAUSS_CONST * HELIO_GAUSS_CONST, q, e );
	m = m + mdot * ( jd - t );

	/*** Use the current mean anomaly, and the object's other orbital
	     elements, to compute its heliocentric position vector.  Since
	     orbital elements are traditionally given relative to the ecliptic,
	     the resulting position vector is in the ecliptic coordinate frame.
	     We want the object's position in the equatorial frame, so compute
	     a rotation matrix to transform the vector from the ecliptic frame
	     to the equatorial frame, then transform it. ***/ 
	
	OrbitToXYZ ( q, e, i, w, n, m, &position[0], &position[1], &position[2] );
	SetEclipticRotationMatrix ( matrix, Obliquity ( ecliptic ), -1 );
	TransformVector ( matrix, position );
	
	/*** We have now obtained the object's heliocentric position vector in
	     the equatorial frame at the epoch for which its orbital elements are
	     given.  If we want the object's position in the equatorial frame
	     at a different epoch, compute a rotation matrix to transform its
	     position to the coordinate frame at the desired output epoch, and
	     transform the vector. ***/
	     
	if ( ecliptic != equator )
	{
		SetPrecessionRotationMatrix ( matrix, ecliptic, equator, FALSE );
		TransformVector ( matrix, position );
	}
}

/***********************  ComputeGeocentricPosition  *****************************/

void ComputeGeocentricPosition ( double lon, double lat, double alt,
double jd, double equinox, double position[3] )
{
	double	a = KM_PER_EARTH_RADII / KM_PER_AU;
	double	f = EARTH_FLATTENING;
	double	lmst, matrix[3][3];
	
	/*** Compute the local sidereal time (LMST) by computing Greenwich mean
	     sidereal time (GMST) and adding the viewer's longitude.  The LMST
	     is the right ascension of the viewer's position projected onto the
	     celestial sphere from the center of the earth; the viewer's latitude
	     is its declination.  With these, we can compute the viewer's position
	     relative to the center of the earth in the equatorial coordinate system. ***/
	
	lmst = LocalSiderealTime ( jd, lon );
	GeodeticToGeocentricXYZ ( lmst, lat, alt, a, f, &position[0], &position[1], &position[2] );
	
	/*** The resulting position vector is in the coordinate frame defined
	     by the plane of the earth's equator at the epoch (i.e. time) of
	     computation.  If we want the position in the equatorial frame at
	     some other epoch, compute the rotation matrix needed to transform
	     the vector to the equatorial frame at that epoch, then transform
	     the vector. ***/
	     
	if ( equinox != jd )
	{
		SetPrecessionRotationMatrix ( matrix, jd, equinox, FALSE );
		TransformVector ( matrix, position );
	}
}
