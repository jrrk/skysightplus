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

/*******************************  LocalJD  *********************************/

double LocalJD ( void )
{
	time_t		timer;
	struct tm	t;
	double		jd;
	
	time ( &timer );
	t = *localtime ( &timer );
	jd = DateTimeToJD ( t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, 0.0, TRUE );

	return ( jd );
}

/*****************************  DateTimeToJD  *******************************/
   
double DateTimeToJD ( long year, short month, double day,
short hour, short min, double sec, double zone, short calendar )
{
  double jd;
  long a, b;

  day += hour / 24.0 + min / 1440.0 + sec / 86400.0 - zone;

  if ( month < 3 )
  {
    year -= 1;
    month += 12;
  }

  if ( calendar == 1 )
  {
    a = year / 100;
    b = 2 - a + a / 4;
  }
  else
    b = 0;

  jd = floor ( 365.25 * ( year + 4716 ) ) + floor ( 30.6001 * ( month + 1 ) ) +
       day + b - 1524.5;

  return ( jd );
}

/******************************  JDToDateTime  ***********************************/

void JDToDateTime ( double jd, double zone, long *year, short *month, double  *day,
short *hour, short *min, double *sec, short calendar )
{
  long z, a, b, c, d, e;
  double f;

  jd += zone + 0.5;
  z = floor ( jd );
  f = jd - z;

  if ( calendar == 1 )
  {
    a = ( z - 1867216.25 ) / 36524.25;
    a = z + 1 + a - a / 4;
  }
  else
    a = z;

  b = a + 1524;
  c = floor ( ( b - 122.1 ) / 365.25 );
  d = floor ( 365.25 * c );
  e = ( b - d ) / 30.6001;

  if ( e < 14 )
    *month = e - 1;
  else
    *month = e - 13;

  if ( *month > 2 )
    *year = c - 4716;
  else
    *year = c - 4715;

  *day = b - d - floor ( 30.6001 * e ) + f;

  f = *day - floor ( *day );

  *hour = 24.0 * f;
  *min = 1440.0 * f - 60.0 * *hour;
  *sec = 86400.0 * f - 3600.0 * *hour - 60.0 * *min;
}

/***************************  JulianYearToJD  *****************************/

double JulianYearToJD ( double year )
{
	return ( J2000 + DAYS_PER_JULIAN_YEAR * ( year - 2000.0 ) );
}

/**************************  BesselianYearToJD  ****************************/

double BesselianYearToJD ( double year )
{
	return ( B1900 + DAYS_PER_BESSELIAN_YEAR * ( year - 1900.0 ) );
}

/**************************  LocalWeekDay  ********************************/

int LocalWeekDay ( double jd, double zone )
{
	long	i = floor ( jd + zone + 1.5 );
	
	return ( i % 7 );
}

/*************************  DaylightSavingsTime  ***************************/

int DaylightSavingsTime ( double jd, double zone, double *start, double *end )
{
	long	year;
	short	month, hour, min, weekday;
	double	day, sec;
	
	JDToDateTime ( jd, zone, &year, &month, &day, &hour, &min, &sec, 1 );
	
	*start = DateTimeToJD ( year, 4, 1.0, 2, 0, 0.0, zone, 1 );
	weekday = LocalWeekDay ( *start, zone );
	if ( weekday > 0 )
		*start = *start + ( 7 - weekday );
	
	*end = DateTimeToJD ( year, 10, 31.0, 2, 0, 0.0, zone, 1 );
	weekday = LocalWeekDay ( *end, zone );
	*end = *end - weekday;
	
	if ( jd > *start && jd < *end )
		return ( TRUE );
	else
		return ( FALSE );
}

/****************************  JulianToJD  ********************************/

double JulianToJD ( long y, long m, double d )
{
	double jd;
	
	jd = 367 * y
	   - ( 7 * ( y + 5001 + ( m - 9 ) / 7 ) ) / 4
	   + ( 275 * m ) / 9 + d + 1729776.5;
	   
	return ( jd );
}

/****************************  GregorianToJD  ********************************/

double GregorianToJD ( long y, long m, double d )
{
	double	jd;
	
	jd = ( 1461 * ( y + 4800 + ( m - 14 ) / 12 ) ) / 4
	   + ( 367 * ( m - 2 - 12 * ( ( m - 14 ) / 12 ) ) ) / 12
	   - ( 3 * ( ( y + 4900 + ( m - 14 ) / 12 ) / 100 ) ) / 4
	   + d - 32075.5;
	   
	return ( jd );
}

/****************************  IslamicToJD  ********************************/

double IslamicToJD ( long y, long m, double d, char c )
{
	long	jd0;
	double	jd;
	
	jd0 = c ? 1948440 : 1948939;
	
	jd = ( 11 * y + 3 ) / 30
	   + 354 * y
	   + 30 * m
	   - ( m - 1 ) / 2
	   + d + jd0 - 385.5;
	   
	return ( jd );
}

/****************************  IndianToJD  ********************************/

double IndianToJD ( long y, long m, double d )
{
	double jd;
	
	jd = 365 * y + ( y + 78 - 1 / m ) / 4 + 31 * m - ( m + 9 ) / 11
	   - ( m / 7 ) * ( m - 7 )
	   - ( 3 * ( ( y + 78 - 1 / m ) / 100 + 1 ) ) / 4
	   + d + 1749578.5;
	   
	return ( jd );
}

/***************************  JDToJulianYear  ****************************/

double JDToJulianYear ( double jd )
{
	return ( ( jd - J2000 ) / DAYS_PER_JULIAN_YEAR + 2000.0 );
}

/**************************  JDToBesselianYear  ***************************/

double JDToBesselianYear ( double jd )
{
	return ( ( jd - B1900 ) / DAYS_PER_BESSELIAN_YEAR + 1900.0 );
}

/****************************  JDToJulian  ********************************/

void JDToJulian ( double jd, long *y, short *m, double *d )
{
	long	j, k, l, n, i;
	double	f;
	
	jd = jd + 0.5;
	j = floor ( jd );
	f = jd - j;

	j = j + 1402;
	k = ( j - 1 ) / 1461;
	l = j - 1461 * k;
	n = ( l - 1 ) / 365 - l / 1461;
	i = l - 365 * n + 30;
	j = ( 80 * i ) / 2447;
	
	*d = i - ( 2447 * j ) / 80 + f;
	i = j / 11;
	*m = j + 2 - 12 * i;
	*y = 4 * k + n + i - 4716;
}

/****************************  JDToGregorian  ********************************/

void JDToGregorian ( double jd, long *y, short *m, double *d )
{
	long	l, n, i, j;
	double	f;
	
	jd = jd + 0.5;
	j = floor ( jd );
	f = jd - j;
	
	l = j + 68569;
	n = ( 4 * l ) / 146097;
	l = l - ( 146097 * n + 3 ) / 4;
	i = ( 4000 * ( l + 1 ) ) / 1461001;
	l = l - ( 1461 * i ) / 4 + 31;
	j = ( 80 * l ) / 2447;
	
	*d = l - ( 2447 * j ) / 80 + f;
	l = j / 11;
	*m = j + 2 - 12 * l;
	*y = 100 * ( n - 49 ) + i + l;
}

/*****************************  JDToIslamic  ********************************/

void JDToIslamic ( double jd, long *y, short *m, double *d, char c )
{
	long	jd0, l, n, j;
	double	f;
	
	jd = jd + 0.5;
	j = floor ( jd );
	f = jd - j;

	jd0 = c ? 1948440 : 1948939;
	
	l = j - jd0 + 10632;
	n = ( l - 1 ) / 10631;
	l = l - 10631 * n + 354;
	j = ( ( 10985 - l ) / 5316 ) * ( ( 50 * l ) / 17719 )
	  + ( l / 5670 ) * ( ( 43 * l ) / 15238 );
	l = l - ( ( 30 - j ) / 15 ) * ( ( 17719 * j ) / 50 )
	  - ( j / 16 ) * ( ( 15238 * j ) / 43 ) + 29;
	  
	*m = ( 24 * l ) / 709;
	*d = l - ( *m * 709 ) / 24 + f;
	*y = 30 * n + j - 30;
}

/******************************  JDToIndian  ********************************/

void JDToIndian ( double jd, long *y, short *m, double *d )
{
	long	l, n, i, j;
	double	f;
	
	jd = jd + 0.5;
	j = floor ( jd );
	f = jd - j;

	l = j + 68518;
	n = ( 4 * l ) / 146097;
	l = l - ( 146097 * n + 3 ) / 4;
	i = ( 4000 * ( l + 1 ) ) / 1461001;
	l = l - ( 1461 * i ) / 4 + 1;
	j = ( ( l - 1 ) / 31 ) * ( 1 - l / 185 )
	  + ( l / 185 ) * ( ( l - 156 ) / 30 + 5 ) - l / 366;
	  
	*d = l - 31 * j + ( ( j + 2 ) / 8 ) * ( j - 5 ) + f;
	l = j / 11;
	*m = j + 2 - 12 * l;
	*y = 100 * ( n - 49 ) + l + i - 78;
}

/*****************************  DeltaT  ********************************/

double DeltaT ( double jd )
{
  short x;
  double year, t, a, b, c, n;
  static float dt[77][2] = {
  { 1620.0, 124.0 },
  { 1625.0, 102.0 },
  { 1630.0,  85.0 },
  { 1635.0,  72.0 },
  { 1640.0,  62.0 },
  { 1645.0,  54.0 },
  { 1650.0,  48.0 },
  { 1655.0,  43.0 },
  { 1660.0,  37.0 },
  { 1665.0,  32.0 },
  { 1670.0,  26.0 },
  { 1675.0,  21.0 },
  { 1680.0,  16.0 },
  { 1685.0,  12.0 },
  { 1690.0,  10.0 },
  { 1695.0,   9.0 },
  { 1700.0,   9.0 },
  { 1705.0,   9.0 },
  { 1710.0,  10.0 },
  { 1715.0,  10.0 },
  { 1720.0,  11.0 },
  { 1725.0,  11.0 },
  { 1730.0,  11.0 },
  { 1735.0,  12.0 },
  { 1740.0,  12.0 },
  { 1745.0,  13.0 },
  { 1750.0,  13.0 },
  { 1755.0,  14.0 },
  { 1760.0,  15.0 },
  { 1765.0,  16.0 },
  { 1770.0,  16.0 },
  { 1775.0,  17.0 },
  { 1780.0,  17.0 },
  { 1785.0,  17.0 },
  { 1790.0,  17.0 },
  { 1795.0,  16.0 },
  { 1800.0,  13.7 },
  { 1805.0,  12.6 },
  { 1810.0,  12.5 },
  { 1815.0,  12.5 },
  { 1820.0,  12.0 },
  { 1825.0,  10.2 },
  { 1830.0,   7.5 },
  { 1835.0,   5.8 },
  { 1840.0,   5.7 },
  { 1845.0,   6.3 },
  { 1850.0,   7.1 },
  { 1855.0,   7.6 },
  { 1860.0,   7.88 },
  { 1865.0,   6.02 },
  { 1870.0,   1.61 },
  { 1875.0,  -3.24 },
  { 1880.0,  -5.40 },
  { 1885.0,  -5.79 },
  { 1890.0,  -5.87 },
  { 1895.0,  -6.47 },
  { 1900.0,  -2.77 },
  { 1905.0,   3.86 },
  { 1910.0,  10.46 },
  { 1915.0,  17.20 },
  { 1920.0,  21.16 },
  { 1925.0,  23.62 },
  { 1930.0,  24.02 },
  { 1935.0,  23.93 },
  { 1940.0,  24.33 },
  { 1945.0,  26.77 },
  { 1950.0,  29.16 },
  { 1955.0,  29.15 },
  { 1960.0,  33.15 },
  { 1965.0,  35.73 },
  { 1970.0,  40.18 },
  { 1975.0,  45.48 },
  { 1980.0,  50.54 },
  { 1985.0,  54.34 },
  { 1990.0,  56.86 },
  { 1995.0,  60.78 },
  { 2000.0,  65.   } };

  year = 2000.0 + ( jd - 2451545.0 ) / 365.25;
  if ( year > 1622.5 && year < 1997.5 )
  {
    x = ( year - 1620.0 ) / 5.0 + 0.5;
    a = dt[x][1] - dt[x-1][1];
    b = dt[x+1][1] - dt[x][1];
    c = b - a;
    n = ( year - dt[x][0] ) / 5.0;
    return ( (double) dt[x][1] + ( a + b + n * c ) * n / 2.0 );
  }
  else
  {
    t = ( year - 1900.0 ) / 100.0;
    return ( 60.0 * ( 0.41 + 1.2053 * t + 0.4992 * t * t ) );
  }   
}

/*************************  LocalSiderealTime *********************************/

double LocalSiderealTime ( double jd, double lon )
{
	double jd0, gmst, t, t2, t3;
	
	jd0 = floor ( jd - 0.5 ) + 0.5;
	
	t = ( jd0 - 2451545.0 ) / 36525.0;
	t2 = t * t;
	t3 = t2 * t;
	
	gmst = 24110.54841 + 8640184.812866 * t + 0.093104 * t2 - 0.0000062 * t3;
	gmst /= 86400.0;
	gmst = gmst - floor ( gmst ) + ( jd - jd0 ) * SIDEREAL_PER_SOLAR_DAY;
	
	return ( Mod2Pi ( gmst * TWO_PI + lon ) );
}

/**********************  SemiDiurnalArc  **********************************/
   
double SemiDiurnalArc ( double a, double d, double f )
{
	double ch;
	
	ch = ( sin ( a ) - sin ( d ) * sin ( f ) ) / ( cos ( d ) * cos ( f ) );
	
	if ( ch >= 1.0 )
		return ( 0.0 );
		
	if ( ch <= -1.0 )
		return ( PI );

 	return ( acos ( ch ) );
}

/*************************  RiseSetTime  *******************************/

double RiseSetTime ( double ra, double dec, double jd, int sign,
double lon, double lat, double alt )
{
	double ha, lst, theta;
	
	/*** Compute the object's hour angle when it is on the horizon.
	     This is the difference between the object's right ascension
	     and the line of right ascension which is on the meridian
	     at the time the object appears on the horizon. ***/

	ha = SemiDiurnalArc ( alt, dec, lat );

	/*** If the object never sets, return infinity; if it never rises,
	     return negative infinity. ***/
	     
	if ( ha == PI )
		return ( INFINITY );
	
	if ( ha == 0.0 )
		return ( -INFINITY );
	
	/*** Compute the local sidereal time.  This is the same as the circle
	     of right ascension which is currently on the local meridian. ***/
	
	lst = LocalSiderealTime ( jd, lon );
	
	/*** Now compute the angular distance that the earth needs to turn
	     through to make the star appear on the horizon.  If the angle
	     is outside the range -PI to +PI, reduce it to that range. ***/
	
	theta = ( ra - lst + sign * ha );
	
	while ( theta > PI )
		theta = theta - TWO_PI;
		
	while ( theta < -PI )
		theta = theta + TWO_PI;
	
	/*** Obtain the time of rising or setting by adding the amount of time
	     the earth takes to rotate through the angle calculated above to the
	     current time. ***/ 
		
	jd = jd + theta / TWO_PI / SIDEREAL_PER_SOLAR_DAY;

	return ( jd );
}

/*************************  RiseSetTimeSearch  *******************************/

double RiseSetTimeSearch ( RiseSetSearchProcPtr proc, double jd, int sign,
double lon, double lat, double alt, double precision, int imax )
{
	double		ra, dec, oldjd;
	int			i = 0;
		     
	do
	{
		oldjd = jd;
		(*proc) ( jd, &ra, &dec );
		jd = RiseSetTime ( ra, dec, jd, sign, lon, lat, alt );
		i = i + 1;
	}
	while ( fabs ( jd - oldjd ) > precision && fabs ( jd ) != INFINITY && i < imax );
			
	return ( jd );
}

/*********************  DailyRiseSetTimeSearch  **************************/

double DailyRiseSetTimeSearch ( RiseSetSearchProcPtr proc, double jd,
int sign, double zone, double lon, double lat, double alt, double precis,
int imax )
{
	double	start, end;
	
	/*** Find the julian dates that correspond to the start and end
	     of the local day. ***/
	     
	start = floor ( jd - 0.5 + zone ) + 0.5 - zone;
	end = start + 1.0;

	/*** Search for the object's exact rise/set time, starting from
	     the middle of the local day. ***/
	     
	jd = RiseSetTimeSearch ( proc, start + 0.5, sign,
	     lon, lat, alt, precis, imax );
	
	/*** If the resulting rise/set time is after the end of the day,
	     start searching again from the middle of the previous day;
	     similarly, if the resulting rise/set time is before the start
	     of the current day, start searching again from the middle
	     of the following day. ***/

	if ( jd > end )
		jd = RiseSetTimeSearch ( proc, start - 0.5, sign,
		     lon, lat, alt, precis, imax );
	else if ( jd < start )
		jd = RiseSetTimeSearch ( proc, end + 0.5, sign,
		     lon, lat, alt, precis, imax );

	/*** If the resulting rise/set time is still before the beginning or
	     after the end of the local day, the object does not rise or set
	     at all on that day.  Return positive infinity to indicate the
	     object does not set or negative infinity to indicate that it
	     does not rise. ***/
	
	if ( jd > end || jd < start )
	{
		if ( sign == -1 )
			jd = -INFINITY;
		else
			jd = INFINITY;
	}
				
	return ( jd );
}

