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

/************************  header files  *************************************/

/*** Prevent multiple #includes! ***/

#ifndef ASTROLIB_H
#define ASTROLIB_H

/*** ANSI headers ***/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <time.h>

/*** Numerical routines headers ***/

#include "Numerics.h"

/*** Define all versions of true and false ***/

#ifndef false
#define false 0
#endif
#ifndef true
#define true (!false)
#endif

#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif

/*******************************  target.h  ***********************************

	This header file defines certain characteristics of the target platform
	you will be compiling AstroLib for.  It is not supplied by AstroLib but
	must be written by you.  Your "target.h" file must contain the following
	items:
	
	#define BITPIX [ 8 | 16 | 32 | -32 | -64 ]
	
	This constant defines the format in which FITS image data will be stored
	internally.  The value you #define BITPIX as must be either 8, 16, 32,
	-32, or -64, which correspond to the standard FITS 8-bit unsigned integer,
	16-bit signed integer, 32-bit signed integer, 32-bit floating point, and
	64-bit floating point data types.  Any other value for BITPIX will cause
	a compile error.  (Note that you can -read- a FITS file in any of the
	standard FITS data formats regardless of how you #define BITPIX; this just
	determines how the data will be stored in memory once it has been read.)
	
	#define DOUBLE [ double | short double ]
	
	This macro specifies the type of a 64-bit, IEEE754-compliant, double-
	precision floating-point value.  For most compilers, this is just the plain
	"double" type.  However, for others it is the "short double" type.  If
	you do not #define DOUBLE correctly, you will generate compile-time and/
	or runtime errors.  Check your compiler's documentation to find out which
	choice is correct.
	
	#define BYTESWAP [ TRUE | FALSE ]
	
	This constant defines the native byte-ordering scheme of the target
	platform.  "Byte-swapped" (or "Little-endian") chips include the Intel
	80x86 family and DEC's VAX and Alpha families.  Non-byte-swapped (or
	"Little-endian") chips include the Motorola 680x0 and PowerPC familes,
	and the Sun SPARC family.  If you do not #define BYTESWAP correctly,
	your program will not be able to read or write valid FITS files because
	the FITS standard specifies that image data must be written with
	"Big-endian" byte-ordering.

	#define MATHEX <C9X floating-point extensions header file>

	Several AstroLib functions make use of the C9X floating-point extensions
	to the standard ANSI C math library.  The C9X extensions, which are not
	yet an ANSI standard, are present in some implementations of the math
	library and not in others.  If the math library and its associated header
	file on your target platform does not contain the C9X extensions, you
	can #define MATHEX to indicate the name of the header file which contains
	the C9X extensions.  You may leave out this line if the C9X extensions
	are already included in one of the standard ANSI C header files (for
	example, <math.h> which are #included in the preprocessor statements
	above.
	
	Please note that if your development environment does not contain any C9X
	extensions header file or libraries, you may use the header file "MathEx.h"
	provided with AstroLib:
	
	#define MATHEX "MathEx.h"
	
	Also be sure to include the source file "MathEx.c", which contains the
	actual definitions of the C9X math extension functions used by AstroLib.

*******************************************************************************/

#include "target.h"

#ifdef MATHEX
#include MATHEX
#endif

#ifndef BITPIX
#define BITPIX	-32
#endif

#ifndef DOUBLE
#define DOUBLE	double
#endif

#ifndef MALLOC
#define MALLOC	malloc
#endif

/*** define image data type and minimum and maximum image data values ***/

#if ( BITPIX == 8 )

typedef unsigned char	PIXEL;
#define PIXEL_MIN		0
#define PIXEL_MAX		UCHR_MAX

#elif ( BITPIX == 16 )

typedef short			PIXEL;
#define PIXEL_MIN		SHRT_MIN
#define PIXEL_MAX		SHRT_MAX

#elif ( BITPIX == 32 )

typedef long			PIXEL;
#define PIXEL_MIN		LONG_MIN
#define PIXEL_MAX		LONG_MAX

#elif ( BITPIX == -32 )

typedef float			PIXEL;
#define PIXEL_MIN		-HUGE_VAL
#define PIXEL_MAX	 	HUGE_VAL

#elif ( BITPIX == -64 )

typedef DOUBLE			PIXEL;
#define PIXEL_MIN		-HUGE_VAL
#define PIXEL_MAX	 	HUGE_VAL

#else

#error BITPIX is undefined!

#endif

/************************ constants and macros  ******************************/

#define PI							3.141592654
#define TWO_PI						6.283185307
#define HALF_PI						1.570796327
#define RAD_PER_DEG					0.01745329252
#define DEG_PER_RAD					57.29577951
#define HOUR_PER_RAD				(DEG_PER_RAD/15.0)
#define ARCSEC_PER_RAD				206264.8062

#define B1900                		2415020.31352
#define B1950                		2433282.423
#define J2000                		2451545.0
#define SEC_PER_DAY					86400.0
#define DAYS_PER_JULIAN_YEAR		365.25
#define DAYS_PER_BESSELIAN_YEAR		365.242198781
#define SIDEREAL_PER_SOLAR_DAY		1.00273790934

#define HELIO_GAUSS_CONST			0.01720209895
#define LIGHT_DAYS_PER_AU			0.005775518
#define KM_PER_AU					149597870.0

#define GEO_GAUSS_CONST				0.01743669161
#define EARTHS_PER_SOLAR_MASS		332946.0
#define KM_PER_EARTH_RADII			6378.140
#define EARTH_FLATTENING			(1.0/298.257)
#define EARTH_J2					0.00108263
#define EARTH_J3					-0.254E-5
#define EARTH_J4					-0.161E-5

#define GALACTIC_N_POLE_RA			192.25
#define GALACTIC_N_POLE_DEC			27.4
#define GALACTIC_LON_ASC_NODE		33.0

#define APERTURE_MAX_PARAMS			16

#define APERTURE_TYPE_LINEAR		0x00000001
#define APERTURE_TYPE_RECTANGULAR	0x00000002
#define APERTURE_TYPE_ELLIPTICAL	0x00000003

#define APERTURE_FLAG_POS_STD		0x00010000
#define APERTURE_FLAG_MAG_STD		0x00020000

/**********************  structs and typedefs  ***************************/

typedef struct VFPTerm				VFPTerm;
typedef struct GuideStar			GuideStar, *GuideStarPtr;
typedef struct GuideStarList		GuideStarList, *GuideStarListPtr;
typedef struct SBIGInfo				SBIGInfo, *SBIGInfoPtr;

typedef double (*ImageSurfaceFuncPtr)	( long, long, double[] );
typedef double (*ImageModelFuncPtr)		( long, long, double[], double[] );

/***************************  FITSImage  **********************************

	This structure is used to conveniently store FITS image file header
	and image data; it is used by a number of routines in the file FITS.c.
	
***************************************************************************/

typedef char **FITSHeader;

typedef struct FITSImage
{
	long			bitpix;		/* number of bits per pixel */
	long			naxis;		/* number of axes in image data matrix */
	long			naxis1;		/* number of columns in image data matrix */
	long			naxis2;		/* number of rows in image data matrix */
	long			naxis3;		/* number of frames in image data matrix */
	double			bzero;		/* image data offset parameter */
	double			bscale;		/* image data scaling parameter */
	FITSHeader		header;		/* matrix containing FITS header information */
	PIXEL			***data;	/* matrix containing image data */
}
FITSImage;

/****************************  FITSTable  *********************************

	These structures are used to conveniently store FITS table file header
	and table data; it is used by a number of routines in the file FITS.c.
	
***************************************************************************/

typedef struct FITSTableField
{
	long			tbcol;		/* field starting column (from one) */			
	char			tform[20];	/* field format specification string */
	char			ttype[20];	/* field type */
	char			tunit[20];	/* units for field data */
	char			tnull[20];	/* string indicating null value (i.e. no data) */
	double			tzero;		/* field data offset parameter */
	double			tscal;		/* field data scaling parameter */
}
FITSTableField;

typedef struct FITSTable
{
	long			naxis1;		/* number of columns in table */
	long			naxis2;		/* number of rows in table */
	long			tfields;	/* number of table data fields per row */
	FITSTableField	*fields;	/* array of table field information */
	char			**header;	/* matrix containing FITS table header */ 
	char			**data;		/* matrix containing FITS table data */
}
FITSTable;

/****************************  GSCRegion  *********************************

	This structures is used to conveniently store information about a
	region file from the Hubble Guide Star Catalog.  It is used by several
	routines in the source file GSC.c.
	
***************************************************************************/

typedef struct GSCRegion
{
	long	rgn_no;		/* GSC region number */
	float	ra_lo;		/* low right ascention boundary of region, in degrees */
	float	ra_hi;		/* high right ascention boundary of region, in degrees */
	float	dec_lo;		/* low declination boundary of region, in degrees */
	float	dec_hi;		/* high declination boundary of region, in degrees */
}
GSCRegion;

/****************************  GSCRecord  *********************************

	This structures is used to conveniently store information about an
	record in a Hubble Guide Star Catalog region file.  It is used by
	several routines in the source file GSC.c.
	
***************************************************************************/

typedef struct GSCRecord
{
	short	gsc_id;				/* entry's ID number within region */
	double	ra;					/* right ascension, in degrees */
	double	dec;				/* declination, in degrees */
	double	pos_err;			/* position error, in arcseconds */
	double	mag;				/* magnitude */
	double	mag_err;			/* magnitude error */
	long	mag_band;			/* magnitude band */
	long	classification;		/* object type (see below) */
	char	plate_id[5];		/* GSSS internal plate number */
	char	multiple;			/* TRUE or FALSE indicating additional entries */
}
GSCRecord;

/*** GSC record classification codes ***/

#define GSC_RECORD_CLASS_STAR		0
#define GSC_RECORD_CLASS_NONSTAR	3

/**********************  Functions in Angle.c *****************************/

/******************************  atan2pi  *********************************

  DESCRIPTION: Returns the arctangent of (y/x) in the range zero to TWO_PI.

     SYNOPSIS: double atan2pi ( double x, double y )

          (y): Y coordinate
		  (x): X coordinate

      RETURNS: arctangent of (y/x) in the range 0 to TWO_PI.  If the angle
	           is undefined, returns zero.

****************************************************************************/

double atan2pi ( double, double );

/*********************************  Mod2Pi  ********************************

	Reduces an angle to the range 0 to TWO_PI.

	double Mod2Pi ( double x )

	(x): Value of the angle, in radians.

	Returns value of the angle, reduced to the range 0 to TWO_PI.

*****************************************************************************/

double Mod2Pi ( double );

/******************************  DegMinSecToDecimal  **********************

	Converts an angle in degrees, minutes, and seconds to its equivalent
	value in decimal degrees.

	double DegMinSecToDecimal ( short deg, short min, double sec, char sign )

	(deg): Degree part of the angle.
	(min): Minutes part of the angle.
	(sec): Seconds part of the angle.
	(sign): Sign of the angle.

	Returns the decimal-degree value of the angle.  The angle is assumed
	to be positive regardless of the sign on (deg), (min), or (sec), unless
	the value passed in (sign) is '-'.

****************************************************************************/

double DegMinSecToDecimal ( short, short, double, char );

/******************************  DecimalToDegMinSec  ***********************

	Converts an angle in decimal degrees to its equivalent value
	in degrees, minutes, and seconds.

	void DecimalToDegMinSec ( double dec,
	     short *deg, short *min, double *sec, char *sign )

	(dec): Decimal-degree value of the angle.
	(deg): Receives degree part of the angle.
	(min): Receives minutes part of the angle.
	(sec): Receives seconds part of the angle.
	(sgn): Receives sign of the angle, as '+' is the angle is zero
	       or positive, or as '-' if the angle is negative.

	This function returns nothing.
	
****************************************************************************/

void DecimalToDegMinSec ( double, short *, short *, double *, char * );

/***************************  Separation  *******************************

	Computes the angular separation between two points on a sphere.

	double Separation ( double a1, double d1, double a2, double d2 )

	(a1): Longitude coordinate of the first point, in radians.
	(d1): Latitude coordinate of the first point, in radians.
	(a2): Longitude coordinate of the second point, in radians.
	(d2): Latitude coordinate of the second point, in radians.

	Returns angular separation between points (a1,d1) and (a2,d2) in radians.

***************************************************************************/

double Separation ( double, double, double, double );

/****************************  PositionAngle  ****************************

	Computes the direction from one point on a sphere to another.

	double PositionAngle ( double a1, double d1, double a2, double d2 )

	(a1): Longitude coordinate of the first point, in radians.
	(d1): Latitude coordinate of the first point, in radians.
	(a2): Longitude coordinate of the second point, in radians.
	(d2): Latitude coordinate of the second point, in radians.

	Returns the direction from point (a1,d1) to (a2,d2), in radians.

	Position angle is measured eastward (counterclockwise) from north:
	north = 0, east = 90, south = 180, west = 270. (Note that these values
	would be returned in radians!)

**************************************************************************/

double PositionAngle ( double, double, double, double );

/***********************  VectorSeparation  ***************************

	Computes the angular separation between two points on a sphere,
	where both are expressed as rectangular unit vectors.
	
	double VectorPositionAngle ( double u[3], double v[3] )

	(u): Elements of the first vector.
	(v): Elements of the second vector.
	     
	Returns angular separation between vectors (u) and (v), in radians.
	
***********************************************************************/

double VectorSeparation ( double [3], double [3] );

/***********************  VectorPositionAngle  ************************

	Computes the position angle (direction) from one point to another
	on a sphere, where both are expressed as rectangular unit vectors.
	
	double VectorPositionAngle ( double u[3], double v[3] )

	(u): Elements of the first vector.
	(v): Elements of the second vector.
	     
	Returns position angle from vector (u) to vector (v), in radians.

	Position angle is measured eastward (counterclockwise)
	from north: north = 0, east = 90, south = 180, west = 270.
	
	(Note that these values would be returned in radians!)
	
***********************************************************************/

double VectorPositionAngle ( double [3], double [3] );

/*********************  Functions in CoordSys.c ***********************/

/************************  SphericalToXYZ  ********************************

	Converts spherical coordinates to rectangular.
  
	void SphericalToXYZ ( double l, double b, double r,
	double *x, double *y, double *z )

	(l): Longitude coordinate, in radians.
	(B): Latitude coordinate, in radians.
	(r): Radial coordinate.
	(x): Receives x coordinate.
	(y): Receives y coordinate.
	(z): Receives z coordinate.
          
	Return nothing.

***************************************************************************/

void SphericalToXYZ ( double, double, double, double *, double *, double * );

/**********************  XYZToSpherical  *********************************

	Converts rectangular coordinates to spherical.
  
	void XYZToSpherical ( double x, double y, double z,
	double *l, double *b, double *r )

	(x): X coordinate.
	(y): Y coordinate.
	(z): Z coordinate.
	(l): Receives longitude coordinate, in radians from 0 to TWO_PI.
	(b): Receives latitude coordinate, in radians from - HALF_PI to HALF_PI.
	(r): Receives radial coordinate.  If you do not wish to compute
	     the radial coordinate, pass NULL instead of a pointer.
          
	Returns nothing.  If the (r) contains zero, (a) and (d) will contain
	zero.

**************************************************************************/

void XYZToSpherical ( double, double, double, double *, double *, double * );

/*********************  SphericalToXYZMotion  *************************

	Converts position and motion in spherical coordinates to position
	and motion in rectangular coordinates.

	void SphericalToXYZMotion ( double l, double b, double r,
	double dl, double db, double dr, double *x, double *y, double *z,
	double *dx, double *dy, double *dz )

	 (l): Longitude coordinate, in radians.
	 (b): Latitude coordinate, in radians.
	 (r): Radial coordinate.
	(dl): Change in longitude coordinate per unit time.
	(db): Change in longitude coordinate per unit time.
	(dr): Change in radial coordinate per unit time.
	 (x): Receives x coordinate.
	 (y): Receives y coordinate.
	 (z): Receives z coordinate.
	(dx): Receives x velocity.
	(dy): Receives y velocity.
	(dz): Receives z velocity.

	Returns nothing.

	The longitude velocity (dl) should be expressed as the change
	in the longitude coordinate (l) per unit time.  To convert this
	value to radians per unit time, you should multiply it by the
	cosine of the latitude coordinate (b).  Note that latitude velocity
	is already expressed in radians per unit time.
	
*************************************************************************/
	
void SphericalToXYZMotion ( double, double, double, double, double, double,
     double *, double *, double *, double *, double *, double *);
     
/***********************  XYZToSphericalMotion  ***************************

	Converts position and motion in rectangular coordinates to position
	and motion in spherical coordinates.
               
	void XYZToSphericalMotion ( double x, double y, double z,
	double dx, double dy, double dz, double *l, double *b, double *r,
	double *dl, double *db, double *dr )

	 (x): X coordinate.
	 (y): Y coordinate.
	 (z): Z coordinate.
	(dx): X velocity.
	(dy): Y velocity.
	(dz): Z velocity.
	 (l): Receives longitude coordinate, in radians.
	 (b): Receives latitude coordinate, in radians.
	 (r): Receives radial coordinate.
	(dl): Receives change in longitude coordinate per unit time.
	(db): Receives change in latitude coordinate per unit time.
	(dr): Receives change in radial coordinate per unit time.

	Returns nothing.  For singular cases, which occur if (r) or both
	(x) and (y) are zero, the function will return zero for all values.
	
	The longitude velocity (dl) is expressed as the change in the
	longitude coordinate (l) per unit time.  To convert this value
	to radians per unit time, you should multiply it by the cosine
	of the latitude coordinate (b).  Note that latitude velocity is 
	already expressed in radians per unit time.
	
***************************************************************************/

void XYZToSphericalMotion ( double, double, double, double, double, double,
double *, double *, double *, double *, double *, double * );

/***********************  VectorMagnitude  ********************************

	Computes the magnitude of a vector.

	double VectorMagnitude ( double v[3] );
               
	(v): Contains the 3 elements of the vector.
          
	Returns magnitude of the vector.                   
  
***************************************************************************/

double VectorMagnitude ( double [3] );

/**************************  ScaleVector  *********************************

	Multiplies a vector by a constant.

	double *ScaleVector ( double v[3], double k )
               
	(v): Contains the 3 elements of the vector.
    (k): Constant by which to multiply vector.
          
	The function returns a pointer to the given vector (v).                   
  
***************************************************************************/

double *ScaleVector ( double [3], double );

/*************************  VectorSum  ************************************

	Computes the sum of two vectors.
	
	double *VectorSum ( double u[3], double v[3], double w[3] )

	(u): Contains the 3 elements of the first vector.
	(v): Contains the 3 elements of the second vector.
	(w): Recieves the 3 elements of the vector sum u + v.
	
	The function returns a pointer to the vector that receives the
	sum (w).  Note that (w) can be the same as either (u) or (v).
          
***************************************************************************/

double *VectorSum ( double [3], double [3], double [3] );

/*************************  VectorDifference  *****************************

	Computes the difference between two vectors.

	double *VectorDifference ( double u[3], double v[3], double w[3] );
               
	(u): Contains the 3 elements of the first vector.
	(v): Contains the 3 elements of the second vector.
	(w): Recieves the 3 elements of the vector difference u - v.
          
	The function returns a pointer to the vector that receives the
	difference (w).  Note that (w) can be the same as either (u) or (v).
  
***************************************************************************/

double *VectorDifference ( double [3], double [3], double [3] );

/*************************  DotProduct ************************************

	Computes the dot product of two vectors.
               
	double DotProduct ( double u[3], double v[3]);

	(u): Contains the 3 elements of the first vector.
	(v): Contains the 3 elements of the second vector.
          
	Returns dot product of the two vectors.                   
  
***************************************************************************/

double DotProduct ( double [3], double [3] );

/***********************  CrossProduct ************************************

	Computes the cross product of two vectors.
               
	double *CrossProduct ( double u[3], double v[3], double w[3] );

	(u): Contains the 3 elements of the first vector.
	(v): Contains the 3 elements of the second vector.
	(w): Recieves the 3 elements of the cross product u x v.
          
	This function returns a pointer to the vector which receives the
	cross product (w).  Note that the vector cross product is not
	commutative, i.e. u x v does not equal v x u; rather, u x v = - v x u.                
  
***************************************************************************/

double *CrossProduct ( double[3], double[3], double[3] );

/****************************  TransformVector *********************************

	Multiplies an XYZ coordinate vector by a 3x3 rotation matrix
	to transform it into a new coordinate frame.

	double *TransformVector ( double m[3][3], double v[3] )
	
	(m): Contains the 3 rows of the rotation matrix sequentially;
	     must consist of 9 consecutive doubles.
	(v): Contains the 3 elements of the vector.
	
	The function returns a pointer to the vector (v).  Upon return,
	the elements of (v) will be multiplied by (m).                   
  
********************************************************************************/

double *TransformVector ( double [3][3], double[3] );

/*****************************  UnTransformVector  *******************************

	Multiplies an XYZ coordinate vector by the transpose of a 3x3 rotation
	matrix.  Performs the opposite transformation as TransformVector().

	void UnTransformVector ( double m[3][3], double v[3] )
               
	(m): Contains the 3 rows of the rotation matrix sequentially;
	     must consist of 9 consecutive doubles.
	(v): Contains the 3 elements of the vector.
          
	The function returns a pointer to the vector (v).  Upon return, the elements
	of (v) will be multiplied by the transpose of (m).                   
  
*********************************************************************************/

double *UnTransformVector ( double [3][3], double [3] );

/**************************  TransformRotationMatrix  ****************************

	Multiplies one 3x3 rotation matrix by another to make a single matrix
	which is the equivalent of the first rotation followed by the second.

	void TransformRotationMatrix ( double a[3][3], double b[3][3] )
               
	(a): Contains the first rotation matrix.
	(b): Contains the second rotation matrix.
          
	Returns nothing.  Upon return, the columns of (a) will be rotated by (b).
	Note that the product b * a is not the same as a * b!!!
	
**********************************************************************************/

void TransformRotationMatrix ( double [3][3], double[3][3] );

/*************************  UnTransformRotationMatrix  ***************************

	Multiplies one 3x3 rotation matrix by the transpose of another.
               
	void UnTransformRotationMatrix ( double a[3][3], double b[3][3] );

	(a): Contains the first rotation matrix.
	(b): Contains the second rotation matrix.
	
	Returns nothing.  Upon return, the columns of (a) will be multipled
	by the columns of (b), i.e. rotated by the matrix which is the
	inverse of b.
               
**********************************************************************************/

void UnTransformRotationMatrix ( double [3][3], double [3][3] );

/*********************************  CopyVector  **********************************

	Copies elements from one 3-vector into another.
  
	void CopyVector ( double w[3], double v[3] )

	(w): Pointer to destination matrix.
	(v): Pointer to source matrix.
          
	The function returns nothing.  Both vectors must contain 3 elements).

**********************************************************************************/

void CopyVector ( double [3], double [3] );

/**********************  CopyRotationMatrix  ****************************

	Copies elements from one rotation matrix into another.
  
	void CopyRotationMatrix ( double b[3][3], double a[3] )

	(b): Pointer to destination matrix.
	(a): Pointer to source matrix.
          
	Both matrices must contain 9 doubles (the matrix rows).
               
	Returns nothing.

**************************************************************************/

void CopyRotationMatrix ( double [3][3], double [3][3] );

/*******************  TransposeRotationMatrix  ***************************

	Transposes a 3x3 rotation matrix in place.
  
	void TransposeRotationMatrix ( double [3][3] )
	
	(m): Address of the matrix, which must contain 9 doubles
	     (the 3 rows of the matrix sequentially).
               
	Returns nothing.  Upon return, the matrix in (m) is replaced
	with its transpose.

**************************************************************************/

void TransposeRotationMatrix ( double [3][3] );

/***********************  SetRotationMatrix  ******************************

	Calculates the elements for a generalized rotation matrix representing
	an arbitrary number of rotations about the principal XYZ axes.
               
	void SetRotationMatrix ( double m[3][3], short n, ... )

	  (m): An array of 9 doubles; will receive the elements of the matrix.
	  (n): The total number of rotations the rotation matrix represents.
	(...): Describes the actual rotations.  There must be (n) pairs of
	       arguments following (n).  The first argument in each pair
	       is an integer, representing the axis about which that rotation
	       is supposed to take place: 0 = X axis, 1 = Y axis, and 2 = Z axis.
	       The second argument in each pair is a double, representing the
	       angular size of the rotation in radians.
      
	Returns nothing.
               
	For example,
	
	SetRotationMatrix ( m, 3, 0, 0.5, 1, -0.6, 2, 0.7 );

	would return in (m) the elements of the rotation matrix that
	performs the following 3 transformations:
  
	1) rotation around the X axis by 0.5 rad;
	2) rotation around the Y axis by -0.6 rad;
	3) rotation around the Z axis by 0.7 rad.

****************************************************************************/

void SetRotationMatrix ( double [3][3], short, ... );

/**********************  SetIdentityRotationMatrix  **********************

	Creates an identity rotation matrix.

	void SetIdentityRotationMatrix ( double m[3][3] )
  
	(m): Address of the matrix, which must contain 9 doubles (the 3 rows
	     of the matrix sequentially).
               
	Returns nothing.  On return, (m) contains a 3x3 identity matrix.

**************************************************************************/

void SetIdentityRotationMatrix ( double [3][3] );

/**********************  Functions in Time.c *****************************/

/*******************************  LocalJD  *********************************

	Returns the current value of the computer's system clock, expressed
	as a Julian Date.
	
	double LocalJD ( void )
	
	This function takes no arguments, and returns the current value of
	the computer's system clock expressed as a Julian Day number.  No
	attempt is made to convert the time from the computer's local time
	zone to UTC, so the value returned is in local time.  To convert
	this value to UTC, subtract the local time zone, expressed as a
	fraction of a day (i.e. the number of hours east of Greenwich
	divided by 24). 
	
	This function makes use of the standard C library timing functions
	provided in <time.h>; this allows the computer's system clock to be
	read to a precision of one second (although the clock may measure
	time more to a higher precision internally.)
	
****************************************************************************/

double LocalJD ( void );

/****************************  DateTimeToJD  *******************************

	Converts a local date and time in the Gregorian or Julian calendar
	to a Julian Day number.
               
	double DateTimeToJD ( long year, short month, double day, short hour,
	short min, double sec, double zone, short calendar )

	    (year): Year.
	   (month): Month.
	     (day): Day, optionally including fractional part.
	    (hour): Hour.
	     (min): Minute.
	     (sec): Second.
	    (zone): Time zone (local time - UTC), as fraction of a day.
	(calendar): Calendar system: 0 = Julian, 1 = Gregorian.
               
	Returns Julian Day number corresponding to the calendar date.
      
	Reference: Jean Meeus, "Astronomical Algorithms", p. 61.
	
*****************************************************************************/

double DateTimeToJD ( long, short, double, short, short, double, double, short );

/**********************************  JDToDateTime  ********************************

	Converts a Julian date to a local date and time in the Gregorian or Julian
	calendar.
               
	JDToDateTime ( double jd, double zone, long *year, short *month, double *day,
	short *hour, short *min, double *sec, short calendar )

	      (jd): Julian Day number.
	    (zone): Time zone (local time - UTC), as a fraction of a day.
	    (year): Receives year.
	   (month): Receives month.
	     (day): Receives day, including fractional part.
	    (hour): Receives hour.
	     (min): Receives minute.
	     (sec): Receives second.
	(calendar): Calendar system: 0 = Julian, 1 = Gregorian.
               
	Returns nothing.  The algorithm presented in the reference has been modified
	to provide correct results for negative Julian dates as well as positive ones,
	unlike the original algorithm.
      
	Reference: Jean Meeus, "Astronomical Algorithms", p. 63.
	
**********************************************************************************/

void JDToDateTime ( double, double, long *, short *, double *, short *, short *, double *, short );

/****************************  GregorianToJD  *******************************

	Converts a Gregorian calendar date to a Julian Day number.
               
	double GregorianToJD ( long y, long m, double d )

	(y): Year.
	(m): Month.
	(d): Day, optionally including fractional part.
               
	Returns the Julian Day number.  The algorithm presented in the source
	reference is valid for all Gregorian calendar dates corresponding to
	JD >= 0, i.e. dates after -4713 November 23.
      
	Reference: The Explanatory Supplement to the Astronomical Almanac,
	           pp. 603-606.
	
******************************************************************************/

double GregorianToJD ( long, long, double );

/*******************************  JulianToJD  *******************************

	Converts a Julian calendar date to a Julian Day number.
               
	double JulianToJD ( long y, long m, double d )

	(y): Year.
	(m): Month.
	(d): Day, optionally including fractional part.
               
	Returns the Julian Day number.  The algorithm presented in the source
	reference is valid for all Julian calendar dates corresponding to
	JD >= 0, i.e. years after -4712.
      
	Reference: The Explanatory Supplement to the Astronomical Almanac,
	           pp. 603-606.
	
******************************************************************************/

double JulianToJD ( long, long, double );

/*******************************  IslamicToJD  *******************************

	Converts an Islamic tabular calendar date to a Julian Day number.
               
	double IslamicToJD ( long y, long m, double d, char c )

	(y): Year.
	(m): Month.
	(d): Day, optionally including fractional part.
    (c): Initial epoch; 0 = astronomical, 1 = civil.
               
	Returns the Julian Day number.  The algorithm presented in the source
	reference is valid for all years >= 1, corresponding to JD >= 1948440.
      
	Reference: The Explanatory Supplement to the Astronomical Almanac,
	           pp. 603-606.
	
******************************************************************************/

double IslamicToJD ( long, long, double, char );

/*******************************  IndianToJD  *******************************

	Converts an Indian civil calendar date to a Julian Day number.
               
	double IndianToJD ( long y, long m, double d )

	(y): Year.
	(m): Month.
	(d): Day, optionally including fractional part.
               
	Returns the Julian Day number.  The algorithm presented in the source
	reference is valid for all years >= 1, corresponding to JD >= 17499995.
      
	Reference: The Explanatory Supplement to the Astronomical Almanac,
	           pp. 603-606.
	
******************************************************************************/

double IndianToJD ( long, long, double );

/*******************************  JDToGregorian  ********************************

	Converts a Julian Day number to a date in the Gregorian calendar.
               
	void JDToGregorian ( double jd, long *y, short *m, double *d )

	(jd): Julian Day number.
	 (y): Receives year.
	 (m): Receives month.
	 (d): Receives day, including fractional part.
               
	Returns nothing.  The algorithm presented in the source reference is valid
	for all Gregorian calendar dates corresponding to JD >= 0, i.e. dates after
	-4713 November 23.
      
	Reference: The Explanatory Supplement to the Astronomical Almanac, pp. 603-606.
	
**********************************************************************************/

void JDToGregorian ( double, long *, short *, double * );

/*******************************  JDToJulian  ********************************

	Converts a Julian date to a date in the Julian calendar.
               
	void JDToJulian ( double jd, long *y, short *m, double *d )

	(jd): Julian Day number.
	 (y): Receives year.
	 (m): Receives month.
	 (d): Receives day, including fractional part.
               
	Returns nothing.  The algorithm presented in the source reference is valid
	for all Julian calendar dates corresponding to JD >= 0, i.e. years after
	-4712.
      
	Reference: The Explanatory Supplement to the Astronomical Almanac, pp. 603-606.
	
**********************************************************************************/

void JDToJulian ( double, long *, short *, double * );

/*******************************  JDToIslamic  ********************************

	Converts a Julian date to a date in the Islamic tabular calendar.
               
	void JDToIslamic ( double jd, long *y, short *m, double *d, char c )

	(jd): Julian Day number.
	 (y): Receives year.
	 (m): Receives month.
	 (d): Receives day, including fractional part.
     (c): calendar system; 0 = astronomical, 1 = civil.
               
	Returns nothing.  The algorithm presented in the source reference is valid
	for all years >= 1, corresponding to JD >= 1948440.
	
	Reference: The Explanatory Supplement to the Astronomical Almanac, pp. 603-606.
	
**********************************************************************************/

void JDToIslamic ( double, long *, short *, double *, char );

/*******************************  JDToIndian  ********************************

	Converts a Julian date to a date in the Indian civil calendar.
               
	void JDToJulian ( double jd, long *y, short *m, double *d )

	(jd): Julian Day number.
	 (y): Receives year.
	 (m): Receives month.
	 (d): Receives day, including fractional part.
               
	Returns nothing.  The algorithm presented in the source reference is valid
	for all years >= 1, or jd >= 1749995.
      
	Reference: The Explanatory Supplement to the Astronomical Almanac, pp. 603-606.
	
**********************************************************************************/

void JDToIndian ( double, long *, short *, double * );

/***********************  JulianYearToJD  *****************************

	Converts a Julian or Besselian epoch to a Julian date.
  
	double JulianYearToJD ( double year )
	double BesselianYearToJD ( double year )
	
	(year): The Julian or Besselian year.

	Returns the Julian date corresponding to the specified Julian
	or Besselian year.
      
	Reference: the Astronomical Almanac for the Year 1990, p. B4.
   
*************************************************************************/

double JulianYearToJD ( double );
double BesselianYearToJD ( double );

/**************************  JDToJulianYear  ******************************

	Converts a Julian date to a Julian or Besselian year.
  
	double JDToJulianYear ( double jd )
	double JDToBesselianYear ( double jd )
	
	(jd): The Julian date.
	
	Returns the Julian or Besselian year, as desired.
      
	Reference: the Astronomical Almanac for the Year 1990, p. B4.

***************************************************************************/

double JDToJulianYear ( double );
double JDToBesselianYear ( double );

/*************************  LocalWeekDay  *********************************

	Given a Julian date, computes the day of the week in a particular
	local time zone.

	int LocalWeekDay ( double jd, double zone )
	
	(jd):   Julian Date, expressed in Universal time.
	(zone): Local time zone, expressed as a fraction of a day east of
	        Greenwich; east time zones are positive.
	
	The function returns an integer from 0-7, indicating the day of
	the week: 0 = sunday, 1 = monday, 2 = tuesday ... 6 = saturday.
	
	Reference: Jean Meeus, "Astronomical Algorithms", p. 65
		
***************************************************************************/

int LocalWeekDay ( double, double );

/***********************  DaylightSavingsTime  ******************************

	Determines whether a Julian date falls within Daylight Savings Time
	in a particular local time zone.

	int DaylightSavingsTime ( double jd, double zone,
	    double *start, double *end )
	
	(jd):    Julian Date, expressed in Universal time.
	(zone):  Local time zone, expressed as a fraction of a day east of
	         Greenwich; east time zones are positive.
	(start): receives JD of start of daylight savings time.
	(end):   receives JD of end of daylight savings time.
	
	The function returns TRUE if the given JD lies within daylight savings
	time, and FALSE otherwise.  The variables (start) and (end) receive the
	Julian dates (expressed in Universal time) corresponding to the
	beginning and end of Daylight Savings Time in the specified local time
	zone, for the same year as the given JD.
	
	Daylight Savings Time is assumed to begin at 02:00:00 local time on the
	first Sunday in April, and ends at 02:00:00 local time on the last Sunday
	in October.
	
	Reference: Bill Owen, personal communication, 1998.
		
***************************************************************************/

int DaylightSavingsTime ( double, double, double *, double * );

/*****************************  DeltaT  ***********************************

	Computes the value of Delta T, the difference between Universal Time
	(UT) and Terrestrial Dynamic Time (TDT) or Ephemeris Time (ET) for any
	given date.
               
	double DeltaT ( double jd )
	
	(jd): Julian date at which to compute Delta T.
         
	Returns the value of TDT - UT in seconds at the specified Julian date
	(jd).  This should be added to UT to obtain TDT.
	
	Prior to 1984, TDT is referred to as ET.  For the years 1622.5 - 1997.5,
	Delta T is computed from tabulated values via a second-order interpolation,
	and should be accurate to about 1 sec. Otherwise, it is estimated from a
	quadratic approximation.
               
	References: The Astronomical Almanac.
                Jean Meeus, "Astronomical Formulae for Calculators."
  
***************************************************************************/

double DeltaT ( double );

/***********************  LocalSiderealTime  ******************************
	
	Calculates an observer's local mean sidereal time.
               
	double LocalSiderealTime ( double jd, double lon )
	
	(jd):  Julian date, including fractional part.
    (lon): observer's longitude, in radians; positive to the east.
        
	Returns the observer's local mean sidereal time at the time specified
	by the julian date (jd).  The result will be returned in radians in
	the range 0 to TWO_PI.

	To obtain Greenwich mean sidereal time, pass a value of zero for
	the longitude parameter.
	
	To convert mean sidereal time to true sidereal time, you should add
	the value of the nutation in longitude computed for the same Julian
	date; see Nutation().
	
	Reference: the Astronomical Almanac for the Year 1984.
	
***************************************************************************/

double LocalSiderealTime ( double, double );

/***************************  SemiDiurnalArc  *****************************

	Computes the angular distance from rising or setting to transit for an
	object with any given declination as seen from any latitude.
        
	double SemiDiurnalArc ( double a, double d, double f);

	(a): The object's altitude when rising/setting, in radians.
	(d): The object's declination, in radians.
	(f): The observer's latitude, in radians.       
               
	Returns the object's hour angle when it is at altitude (a) as viewed from
	latitude (f), in radians.  If the object's altitude is always greater than
	(a), SemiDiurnalArc() returns PI.  If the altitude is always less than (a),
	it returns zero.
  
****************************************************************************/
   
double SemiDiurnalArc ( double, double, double );

/*************************  RiseSetTime  *******************************

	Given an object's celestial coordinates at a particular instant,
	computes the object's time of an rising, transit, or setting that
	is closest to that instant, as seen from a particular location.

	double RiseSetTime ( double ra, double dec, double jd, int sign,
	double lon, double lat, double alt )

	(ra):   object's right ascension, in radians.
	(dec):  object's declination, in radians.
	(jd):   Julian date for which object's coordinates are given.
	(sign): event to compute: -1 = rising, 0 = transit, +1 = setting.
	(lon):  observer's east longitude, in radians.
	(lat):  observer's north latitude, in radians.
	(alt):  observer's local horizon altitude, in radians.
	
	The function returns a Julian date which represents the object's rise/
	transit/set time that is closest to the time specified for the object's
	coordinates (jd).  This will always be within half a day of the value
	passed in the (jd) parameter, with two exceptions: if the object does
	not set below the horizon as seen from the specified location, the
	function returns INFINTY; and if the object does rise above the horizon
	at the specified location, it returns -INFINITY.
	
	The object's equatorial coordinates, (ra) and (dec), should be given
	for their current precessional epoch (jd).
	
	The (alt) parameter specifies the altitude of the local horizon, in
	radians; the rise and set times returned by this function correspond
	to the instant that the object's coordinates lie at this altitude.
	For point objects, a horizon altitude of -0.5 degrees is typically
	used to account for atmospheric refraction.  For the sun and moon,
	which are extended objects, a horizon altitude of -50 arcminutes
	is conventionally used to compute rise/set times.  The times of
	civil, nautical, and astronomical twilight can be computed by using
	a horizon altitude of -6, -12, and -18 degrees, respectively.
	
	Note that this function does not take into account the object's motion
	over the course of the day.  For objects, like stars, whose equatorial
	coordinates remain relatively fixed over the course of a day, this should
	pose no problem.  However, for fast-moving objects like the sun and moon,
	you should call this function iteratively, recomputing the object's
	coordinates at the rise/set time predicted by the function on each
	iteration.
	
*******************************************************************************/

double RiseSetTime ( double, double, double, int, double, double, double );

/**************************  RiseSetTimeSearch  *****************************

	Computes the time of an object's rise, transit, or set that is closest
	to an initial starting date.
	
	double RiseSetTimeSearch ( RiseSetProcPtr proc, double jd, int sign,
	       double lon, double lat, double alt, double precis, int imax )

	(proc):   pointer to a function which compute the object's position.
	(jd):     starting date, expressed as a julian date.
	(sign):   event to find: -1 = rise, 0 = transit, +1 = set.
	(lon):    observer's east longitude, in radians.
	(lat):    observer's north latitude, in radians.
	(alt):    observer's local horizon altitude, in radians.
	(precis): desired precision of result, as a fraction of a day.
	(imax):   maximum number of iterations to perform.
	
	The function returns the  Julian date of the object's rising, transit,
	or setting that is closest to the initial starting date (jd), to the
	specified precision.  It works by iterately computing the object's
	position and using it to predict increasingly accurate estimates
	of the object's rise/transit/set time.  The maximum number of
	iterations to perform is specified by the (imax) parameter.  If the
	object reaches a position from which it does not rise or does not
	set on the specified date, the function returns -INFINITY or
	INFINITY, respectively.
	
	The (sign) parameter specifies the phenomenon to search for: -1
	specifies a rise time, 0 specifies a transit time, and +1 specifies
	a set time.
	
	The (proc) parameter is a pointer to a function which computes the
	object's right ascension and declination on a given Julian date.
	This function has the following prototype:
	
	void RiseSetSearchProc ( double jd, double *ra, double *dec )
	
	The Julian date at which the object's position should be computed is
	given in the (jd) parameter; its right ascension and declination
	should be returned in the (dec) parameter.  For highest accuracy,
	the right ascension and declination should be computed for the same
	precessional epoch as (jd), and should be corrected for nutation,
	aberration, light time, and geocentric parallax at the observer's
	position.

******************************************************************************/

typedef void (*RiseSetSearchProcPtr) ( double, double *, double * );

double RiseSetTimeSearch ( RiseSetSearchProcPtr, double, int,
double, double, double, double, int );

/**********************  DailyRiseSetTimeSearch  **************************

	Computes the time an object's rises, transits, or sets on a particular
	local day.
	
	double RiseSetTimeSearch ( RiseSetProcPtr proc, double jd, int sign,
	double lon, double lat, double alt, double precis, int imax )

	(proc):   pointer to a function which compute the object's position.
	(jd):     starting date, expressed as a julian date.
	(sign):   event to find: -1 = rise, 0 = transit, +1 = set.
	(zone):   observer's local time zone, days east of Greenwich.
	(lon):    observer's east longitude, in radians.
	(lat):    observer's north latitude, in radians.
	(alt):    observer's local horizon altitude, in radians.
	(precis): desired precision of result, as a fraction of a day.
	(imax):   maximum number of iterations to perform.
	
	The function returns the  Julian date of the object's rising, transit,
	or setting that is closest to the starting time (jd) on a particular
	local day.  It works by iterately computing the object's position and
	using it to predict increasingly accurate estimates of the object's
	rise/transit/set time, until the estimate of the time converges to the
	specified precision.  The maximum number of iterations to perform is
	specified by the (imax) parameter.  If the object does not rise or set
	on the specified local day, the function returns -INFINITY or INFINITY,
	respectively.
	
	This function is guaranteed to work only for objects which rise
	and set at most once per day.  It will not work for objects which
	rise and set multiple times, e.g. artifical earth satellites.
	
	The (sign) parameter specifies the phenomenon to search for: -1
	specifies a rise time, 0 specifies a transit time, and +1 specifies
	a set time.
	
	The (proc) parameter is a pointer to a function which computes the
	object's right ascension and declination on a given Julian date.
	See RiseSetTimeSearch() for a complete description of this function.

******************************************************************************/

double DailyRiseSetTimeSearch ( RiseSetSearchProcPtr, double, int,
double, double, double, double, double, int );

/***********************  Functions in Reduce.c ***************************/

/***************************  Precession  *********************************

	Computes the precessional constants.
               
	void Precession ( double jd0, double jd1, double *zeta, double *z,
	double *theta, double *eta, double *pi, double *p )

	  (jd0): Julian date of the initial epoch.
	  (jd1): Julian date of the final epoch.
	 (zeta): Receives Newcomb's precessional constant zeta, in radians.
	    (z): Receives Newcomb's precessional constant z, in radians.
	(theta): Receives Newcomb's precessional constant theta, in radians.
	  (eta): Receives angle of inclination between the two ecliptics.
	   (pi): Receives angle from initial equinox to intersection of ecliptics.
	    (p): Receives general precession in ecliptic longitude.

	Returns nothing.  To supress computation of any of the above constants,
	pass NULL instead of a pointer.
      
	References: Explanatory Supplement to the Astronomical Almanac, p. 104.
	            Jean Meeus, "Astronomical Algorithms", p. 126-128.
               
***************************************************************************/
                
void Precession ( double, double, double *, double *, double *, double *, double *, double * );

void LongTermPrecession ( double, double *, double * );
void LongTermPrecessionMatrix ( double [3][3], double, double );

/***********************  Obliquity  ************************************

	Computes the mean obliquity of the ecliptic for a given epoch.

	double Obliquity ( double jd )
               
	(jd): Julian Day number of the epoch.
         
	Returns the mean obliquity of the ecliptic for epoch (jd), in radians.
               
	References: The Astronomical Almanac for the Year 1984.

*************************************************************************/

double Obliquity ( double );

/****************************  Nutation  *********************************

	Computes the nutation in longitude and in obliquity for a given epoch.

	void Nutation ( double jd, double *dl, double *de )
               
	(jd): Julian Day number of the epoch.
	(dl): Receives nutation in longitude, in radians.
	(de): Receives nutation in obliquity, in radians.
         
	Returns nothing.  Values are computed using the 1980 IAU theory of
	nutation.  Only linear terms in the fundamental arguments, and the
	four largest periodic terms, have been retained, providing an accuracy
	for (dl) and (de) of 0".5 and 0".1.
               
	References: Explanatory Supplement to the Astronomical Almanac, pp. 109-116.
                Jean Meeus, "Astronomical Algorithms", pp. 131-134.

***************************************************************************/

void Nutation ( double, double *, double * );

/***************************  EarthVelocity  *****************************

	Computes the earth's heliocentric velocity.

	EarthVelocity ( double jd, double *vx, double *vy, double *vz )
  
	      (jd): Julian date on which to compute velocity.
	(vx,vy,vz): Recieves components of the earth's velocity vector
                with respect to the solar system barycenter in the
                J2000 equatorial frame, in units of AU per day.
               
	Returns nothing.
               
	References: Explanatory Supplement to the Astronomical Almanac,
	            pp. 130, 484.
   
**************************************************************************/

void EarthVelocity ( double, double *, double *, double * );

/**********************  RelativisticAberration  *************************

	Applies aberration of light to an object's geometric position vector.
  
	double Aberration ( double u[3], double v[3], double w[3] )

	(u): Object's geometric position vector relative to viewer.
	(v): Observer's velocity vector, in units of the speed of light.
    (w): Recieves apparent direction vector.
    
    The function returns the object's doppler shift.  To obtain the
    observed frequency of electromagnetic radiation emitted from the
    object, divide the true frequency emitted at the object by the
    doppler shift; similarly, to obtain the observed wavelenth,
    multiply the true wavelength by this value.
    
    The input vector (u) represents the object's geometric position
    relative to the viewer.  The vector (v) represents the viewer's
    velocity relative to the origin of the coordinate system in which
    both the viewer's and object's positions are measured.  While
    vector (u) can take any units and have any magnitude, vector (v)
    must be given in units of the speed of light; its magnitude must
    therefore be less than 1.
    
    The vector returned in (w) will have the same magnitude as the
    original vector (u), but its direction will be corrected for
    aberration of light.  If (w) points to the same array as (u),
    the original vector will be overwritten.
    
    This function is adapted from the relativistic expression for
    aberration given in the Explanatory Supplement.
              
	References:
	
	The Explanatory Supplement to the Astronomical Almanac, p. 129.
	
***************************************************************************/

double RelativisticAberration ( double *, double *, double * );

/**********************  GeodeticToGeocentricXYZ  *****************************

	Converts geodetic latitude, longitude, and height to geocentric rectangular
	coordinates.
               
	void GeodeticToGeocentricXYZ ( double l, double b, double h, double a, double f,
	double *x, double *y, double *z )

	(l): Geodetic longitude, in radians.
	(b): Geodetic latitude, in radians.
	(h): Geodetic height.
	(a): Equatorial radius of reference ellipsoid, in same units as (h).
	(f): Flattening of reference ellipsoid: f = ( a - b ) / a,
	     where a and b are ellipsoid's equatorial and polar radii. 
	(x): Receives geocentric X-coordinate, in same units as (h).
	(y): Receives geocentric Y-coordinate, in same units as (h).
	(z): Receives geocentric Z-coordinate, in same units as (h).

	Returns nothing.  Geocentric longitude, latitude, and radius can be
	computed by converting (x), (y), and (z) to spherical coordinates.
	The values for a and f adopted in the 1976 IAU reference ellipsoid
	are a = 6378140 m and f = 1 / 298.257.
               
    Reference: The Astronomical Almanac for the Year 1990, pp. K11-K13.
     
*********************************************************************************/

void GeodeticToGeocentricXYZ (double, double, double, double, double,
double *, double *, double * );

/************************  GeocentricXYZToGeodetic  *****************************

	Converts geocentric rectangular coordinates to geodectic longitude, latitude,
	and height.
               
	void GeocentricXYZToGeodetic ( double x, double y, double z,
	double a, double f, double *l, double *b, double *h )

	(x): Geocentric X-coordinate.
	(y): Geocentric Y-coordinate.
	(z): Geocentric Z-coordinate.
	(a): Equatorial radius of reference ellipsoid.
	(f): Flattening of reference ellipsoid: f = ( a - b ) / a,
	     where a and b are ellipsoid's equatorial and polar radii. 
	(l): Recieves geodetic longitude, in radians from -PI to PI.
	(b): Recieves geodetic latitude, in radians from -PI/2 to PI/2.
	(h): Receives geodetic height, in same units as (a).
          
	Returns nothing.  The values for a and f adopted in the 1976 IAU reference
	ellipsoid are a = 6378140 m and f = 1 / 298.257.
     
	Reference: The Astronomical Almanac for the Year 1990, pp. K11-K13.
     
********************************************************************************/

void GeocentricXYZToGeodetic ( double, double, double, double, double,
double *, double *, double * );

/*******************************  RefractionAngle  ***********************************

	Computes altitude correction to atmospheric refraction.
  
	double RefractionAngle ( double a, double p, double t )

	(a): The object's altitude in radians.
	(p): Barometric pressure in millibars.
	(t): Termperature in degrees centigrade.
          
	Returns refraction correction, in radians.  This should be subtracted
	from the true altitude to obtain the apparent altitude.
               
	Reference: The Astronomical Almanac for the Year 1990, p. B62.
	
*********************************************************************************/

double RefractionAngle ( double, double, double );

/*********************  SetEclipticRotationMatrix **************************

	Computes rotation matrix for transforming XYZ vectors between the
	equatorial and ecliptic frames.

	void SetEclipticRotationMatrix ( double m[3][3], double e, int i )
  
	(e): Obliquity of the ecliptic, in radians.
	(m): Array of 9 doubles to recieve rows of the matrix.
	(i): Direction of the operation desired for the matrix:
	     if i > 0, the matrix will transform equatorial -> ecliptic;
	     if i < 0, the matrix will transform ecliptic -> equatorial.
               
	This function returns nothing.
  
***************************************************************************/

void SetEclipticRotationMatrix ( double [3][3], double, int );

/*********************  SetHorizonRotationMatrix  **************************

	Computes rotation matrix for transforming XYZ vectors between the
	equatorial and local horizon frames.

	void SetHorizonRotationMatrix ( double m[3][3], double l, double b, int i )
          
	(m): Array of 9 doubles to recieve rows of the matrix.     
	(l): The local sidereal time, in radians.
	(b): The local latitude, in radians.
	(i): Direction of the operation desired for the matrix:
	     if i > 0, the matrix will transform equatorial -> horizon;
         if i < 0, the matrix will transform horizon -> equatorial.

	Returns nothing.
      
	NOTE: Azimuth increases eastward from north, i.e. N = 0, E = 90,
	S = 180, W = 270, i.e. clockwise as viewed from above.  Therefore,
	a vector transformed by this matrix will be mirror imaged
	with respect to the sky, which we view from below. 
               
******************************************************************************/

void SetHorizonRotationMatrix ( double [3][3], double, double, int );

/**********************  SetGalacticRotationMatrix  *****************************

	Computes matrix for transforming between equatorial and galactic coordinates.
               
	void SetGalacticRotationMatrix ( double m[3][3], double jd, int i );

	 (m): Array of 9 doubles to recieve rows of matrix.
	(jd): Julian date of precessional epoch to which equatorial coordinates
	      are referred.
	 (i): Direction of the operation desired for the matrix:
	      if i > 0, the matrix will transform equatorial -> galactic;
	      if i < 0, the matrix will transform galactic -> equatorial.

	This function returns nothing.
      
	NOTE: The galactic north pole is fixed by convention at B1950 coordinates
	R.A. 12h 49m, Dec +27 24'.  It does not move with precession, but lies fixed
	with the stars.  The matrix returned by this function will correctly precess
	an equatorial position vector to the B1950 frame before transforming it to
	the galactic frame, and vice-versa.
               
	References: Jean Meeus, "Astronomical Algorithms", pp. 89-91.
   
********************************************************************************/

void SetGalacticRotationMatrix ( double [3][3], double, int );

/*************************  SetPrecessionRotationMatrix  **************************

	Computes a rotation matrix for precessing equatorial coordinates.
  
	void SetPrecessionRotationMatrix ( double m[3][3], double jd0, double jd1, int n )

	  (m): array of 9 doubles to recieve rows of the matrix.
	(jd0): Julian date of the initial epoch.
	(jd1): Julian date of the final epoch.
	  (n): if non-zero, the matrix will also contain the effects of nutation.
               
	Returns nothing.  If nutation is included, the matrix returned will
	transform a vector from the the mean equatorial frame of epoch JD0
	to the true equatorial frame of epoch JD1.  If not, the matrix will
	only transform to the mean frame of epoch JD1.      

	References: Explanatory Supplement to the Astronomical Almanac, pp. 101-105.
                Jean Meeus, "Astronomical Algorithms", pp. 126-128.
  
	[Note: the signs of the rotations in the matrix given by the Supplement
	appear to be inconsistent.  The correct matrix is given below.]
                 
***********************************************************************************/

void SetPrecessionRotationMatrix ( double [3][3], double, double, int );

/**********************  SetEclipticPrecessionRotationMatrix  ***********************

	Computes a rotation matrix for precessing ecliptic coordinates.

	void SetEclipticPrecessionRotationMatrix ( double m[3][3], double jd0, double jd1 )
  
	  (m): array of 9 doubles to recieve rows of the matrix.
	(jd0): Julian date of the initial epoch.
	(jd1): Julian date of the final epoch.
  
	This function returns nothing.         

	References: Explanatory Supplement to the Astronomical Almanac, pp. 99-101.
                Jean Meeus, "Astronomical Algorithms", pp. 128-129.
                 
*************************************************************************************/

void SetEclipticPrecessionRotationMatrix ( double [3][3], double, double );

/***************************  SetNutationRotationMatrix  ******************************

	Calculates the matrix which applies nutation to transform between the mean
	and true equatorial frame of any epoch.
               
	void SetNutationRotationMatrix ( double m[3][3], double e, double dl, double de, int i )

	 (m): Array of 9 doubles to recieve elements of the matrix.
	 (e): Mean obliquity of the ecliptic at epoch, in radians.
	(dl): Nutation in longitude, in radians.
	(de): Nutation in obliquity, in radians.
	 (i): Sign of the operation desired for the matrix:
	      if i > 0, the matrix will transform mean->true;
          if i < 0, the matrix will transform true->mean.
	
	This function returns nothing.
	  
	References: Explanatory Supplement to the Astronomical Almanac, p. 114.

**************************************************************************************/

void SetNutationRotationMatrix ( double m[3][3], double e, double dl, double de, int i );

/**********************  Functions in Orbit.c **********************************/

/**************************  MeanMotion  **************************************

	Computes the mean motion of an object in a 2-body orbit.
  
	double MeanMotion ( double mu, double q, double e )

	(mu): Reduced mass of orbiting bodies.
	 (q): Periapse distance of the orbit.
	 (e): Eccentriciy of the orbit.
          
	Returns mean motion of the orbiting body; units depend on (k).
	(mu) is related to the masses of the orbiting bodies by
               
	mu = G * ( M + m ) = k * k * ( M + m )
               
	where G is the Newtonian constant of gravitation, and th masses of the
	bodies are M and m.  G is related to the Gaussian gravitational constant,
	k, by G = k * k.
               
	In the 1976 IAU heliocentric system of units, the unit of length is the AU,
	the unit of mass is the solar mass, the unit of time is the SI day, and k
	takes the value 0.01720209895.
               
	In the geocentric system, the unit of length is the Earth's equatorial
	radius, the unit of mass is the earth's mass, and the unit of time is the
	SI minute, and k takes the value 0.07436680.  
               
*******************************************************************************/

double MeanMotion ( double, double, double );

/****************************  J2MeanMotion  ************************************

	Computes J2-perturbed mean motion and secular variations in angular orbital
	elements due to J2.
  
	void J2MeanMotion ( double ae, double j2, double a, double e, double i,
	double dm, double *dw, double *dn, double *dmm )

	 (ae): Radius of primary, in arbitrary units.
	 (j2): J2 harmonic coefficient, dimensionless.
	  (a): Semimajor axis of the orbit, in same units as (ae).
	  (e): Eccentricity of the orbit.
	  (i): Inclination of the orbit plane to the equatorial plane of primary.
	 (dm): Unperturbed mean motion.
	 (dw): Recieves secular variation in argument of periapse.
	 (dn): Recieves secular variation in longitude of ascending node.
	(dmm): Recieves perturbation to mean motion due to J2.
        
	Returns nothing.  J2 does not cause any secular variations in the elements
	a, e, and i.
               
	References:
	
	Kelso, T.S. "SPACETRACK REPORT No. 3: Models for Propogation of
	Norad Element Sets", 31 December 1988, pp. 3-4. National Technical
	Information Service, Springfield, VA. 
      
	Danby, J.M.A., "Fundamentals of Celestial Mechanics", p. 347.
	Willmann-Bell, Richmond VA, 1992.
               
**********************************************************************************/

void J2MeanMotion ( double, double, double, double, double, double,
double *, double *, double * );

/*************************  SolveKeplersEqn  *************************************

	Solves Kepler's equation for elliptical, parabolic, and hyperbolic orbits.
               
	void SolveKeplersEqn ( double m, double e, double q, double *nu, double *r )

	 (m): Mean anomaly, in radians.
	 (e): Eccentricity.
	 (q): Periapse distance.
	(nu): Receives true anomaly, in radians.
	 (r): Receives true distance, in same units as (q).
           
	Returns nothing.  If convergence is not achieved with the maximum number of
	iterations (defined below), erroneous results may be returned.
     
	Note: The method used to solve Kepler's equation depends on the eccentricity
	of the orbit.  For closed (elliptical) orbits, Newton's method usually provides
	the fastest solution.  However, if the orbit is highly eccentric (e > 0.975),
	Newton's method may not converge, so we substitute a slower but foolproof binary
	search algorithm.  For parabolic orbits (e = 1.0), we use the the standard
	Newton method.

************************************************************************************/

void SolveKeplersEqn ( double, double, double, double *, double * );

/***********************  OrbitToSpherical  ********************************

	Computes spherical coordinates for an object in a 2-body orbit
	from classical Keplerian elements.
               
	void OrbitToSpherical ( double q, double e, double w, double i, double n,
	double m, double *l, double *b, double *r )

	(q): Periapse distance.
	(e): Eccentricity of the orbit.
	(w): Argument of the perihelion, in radians.
	(i): Inclination of the orbit, in radians.
	(n): Longitude of the ascending node, in radians.
	(m): Mean anomaly, in radians.
	(l): Receives the object's longitude coordinate, in radians.
	(b): Receives object's latitude coordinate, in radians.
	(r): Receives object's radial coordinate, in same units as (q).
          
********************************************************************************/

void OrbitToSpherical ( double, double, double, double, double, double,
double *, double *, double * );

/*******************************  OrbitToXYZ  ********************************

	Computes rectangular coordinates for an object in a 2-body orbit from
	classical elements.
               
	void OrbitToXYZ ( double q, double e, double i, double w, double n, double m,
	double *x, double *y, double *z )

	(q): Periapse distance.
	(e): Eccentricity of the orbit.
	(i): Inclination of the orbit, in radians.
	(w): Argument of the perihelion, in radians.
	(n): Longitude of the ascending node, in radians.
	(m): Mean anomaly, in radians.
	(x): Receives the object's X-coordinate, in same units as (q).
	(y): Receives object's Y-coordinate, in same units as (q).
	(z): Receives object's Z-coordinate, in same units as (q).
          
	Returns nothing.
                 
********************************************************************************/

void OrbitToXYZ(double, double, double, double, double, double, double *, double *, double *);

/***********************  OrbitToXYZMotion  ********************************

	Computes rectangular coordinates and motion for an object in a 2-body
	orbit from classical elements.
               
	void OrbitToXYZ ( double q, double e, double i, double w, double n,
	double m, double dm, double *x, double *y, double *z, double *dx,
	double *dy, double *dz, double *mu )

	 (q): Periapse distance.
	 (e): Eccentricity of the orbit.
	 (i): Inclination of the orbit, in radians.
	 (w): Argument of the perihelion, in radians.
	 (n): Longitude of the ascending node, in radians.
	 (m): Mean anomaly, in radians.
	(dm): Mean motion, in radians per unit time.
	 (x): Receives X coordinate.
	 (y): Receives Y coordinate.
	 (z): Receives Z coordinate.
	(dx): Receives X component of velocity.
	(dy): Receives Y component of velocity.
	(dz): Receives Z component of velocity.
	(mu): Receives combined mass of the bodies multiplied by the
	      Newtonian constant of gravitation: mu = G * ( M + m )
          
********************************************************************************/

void OrbitToXYZMotion ( double, double, double, double, double, double, double,
double *, double *, double *, double *, double *, double *, double * );

/*****************************  XYZMotionToOrbit  *********************************

	Computes classical 2-body orbital elements from rectangular position
	and velocity vectors.

	void XYZMotionToOrbit ( double, double, double, double, double, double, double,
	double *x, double *y, double *z, double *dx, double *dy, double *dz, double *mu )

	 (x): X component of position vector.
	 (y): Y component of position vector.
	 (z): Z component of position vector.
	(dx): X component of velocity vector.
	(dy): Y component of velocity vector.
	(dz): Z component of velocity vector.
	(mu): Combined masses of the two bodies, m and M, multiplied by the
	      Newtonian constant of gravitation, G; i.e. mu = G * ( M + m ).              
	 (q): Receives periapse distance, in same units as (X,Y,Z).
	 (e): Receives orbital eccentricity.
	 (i): Receives orbital inclination, in radians.
	 (w): Receives argument of periapse, in radians.
	 (n): Receives longitude of ascending node, in radians.
	 (m): Receives mean anomaly, in radians.
	(dm): Receives mean motion, in radians per unit time.
      
	Returns nothing.
      
	For combinations of position and velocity that imply an eccentricity
	within 1.0E-8 of 1.0, the orbit is assumed to be parabolic.
               
	Note that (w) is undetermined when (e) = 0.0 and (n) is undetermined
	when (i) = 0.0.
            
*******************************************************************************/

void XYZMotionToOrbit ( double, double, double, double, double, double, double,
double *, double *, double *, double *, double *, double *, double * );

/***************************  TransformOrbit  **********************************

	Transforms angular orbital elements from one coordinate frame to another.
  
	void TransformOrbit ( double m[3][3], double *i, double *w, double *n )

	(m): rotation matrix describing transformation from initial
	     to final frame.
	(i): inclination angle, in radians.
	(w): argument of periapse, in radians.
	(n): longitude of ascending node, in radians.
          
	Returns nothing.  On return, the values contained in (i,w,n) will be replaced
	by their values in the new frame.
      
*********************************************************************************/

void TransformOrbit ( double [3][3], double *, double *, double * );

/****************************  VFPlanet  **************************************

	Calculates the ecliptic longitude, latitude, and radius vector for the
    Sun, Moon, and major planets on a given Julian date.
               
	void VFPSun ( double jd, double *l, double *b, double *r )
	void VFPMoon ( double jd, double *l, double *b, double *r )

	void VFPMercury ( double jd, double *l, double *b, double *r )
	void VFPVenus ( double jd, double *l, double *b, double *r )
	void VFPEarth ( double jd, double *l, double *b, double *r )
	void VFPMars ( double jd, double *l, double *b, double *r )
	void VFPJupiter ( double jd, double *l, double *b, double *r )
	void VFPSaturn ( double jd, double *l, double *b, double *r )
	void VFPUranus ( double jd, double *l, double *b, double *r )
	void VFPNeptune ( double jd, double *l, double *b, double *r )
	void VFPPluto ( double jd, double *l, double *b, double *r )

	(jd): Julian date.
	 (l): Receives ecliptic longitude, in radians.
	 (b): Receives ecliptic latitude, in radians.
	 (r): Receives radius vector, in AU.
         
	Returns nothing.  (l) and (b) are referred to the mean ecliptic and equinox
	of date, and should be correct to 1' for any date within 300 years of the
    present.

    For the sun and moon, the longitude and latitude returned are geocentric.
    For the major planets Mercury - Pluto, the longitude and latitude returned
    are heliocentric.

	Note that VFPEarth simply computes the sun's geocentric position and
	reverses its coordinates by negating the latitude and adding 180 degrees
    to the longitude.

	Reference:
	
	T. C. Van Flandern and F. K. Pulkkinnen, "Low Precision Formulae for Planetary
	Positions", Astrophysical Journal Supplement Series 41: 391-411, 1979 November.

************************************************************************************/
    
void VFPSun ( double, double *, double *, double * );
void VFPMoon ( double, double *, double *, double * );
void VFPMercury ( double, double *, double *, double * );
void VFPVenus ( double, double *, double *, double * );
void VFPEarth ( double, double *, double *, double * );
void VFPMars ( double, double *, double *, double * );
void VFPJupiter ( double, double *, double *, double * );
void VFPSaturn ( double, double *, double *, double * );
void VFPUranus ( double, double *, double *, double * );
void VFPNeptune ( double, double *, double *, double * );
void VFPPluto ( double, double *, double *, double * );

/*****************************  VSOP87  ***************************************

	Calculates the heliocentric ecliptic longitude, latitude, and radius vector
    for the major planets Mercury - Neptune on a given Julian date.
               
	void VSOP87Mercury ( double jd, double *l, double *b, double *r )
	void VSOP87Venus ( double jd, double *l, double *b, double *r )
	void VSOP87Earth ( double jd, double *l, double *b, double *r )
	void VSOP87Mars ( double jd, double *l, double *b, double *r )
	void VSOP87Jupiter ( double jd, double *l, double *b, double *r )
	void VSOP87Saturn ( double jd, double *l, double *b, double *r )
	void VSOP87Uranus ( double jd, double *l, double *b, double *r )
	void VSOP87Neptune ( double jd, double *l, double *b, double *r )

	(jd): Julian date.
	 (l): Receives heliocentric ecliptic longitude, in radians.
	 (b): Receives heliocentric ecliptic latitude, in radians.
	 (r): Receives radius vector, in AU.
         
	Returns nothing.  (l) and (b) are referred to the mean ecliptic and equinox
	of date.

    For Mercury, Venus, Earth-Moon barycenter, and Mars, positions should be
    computed with a precision of 1" for 4000 years before and after J2000.
	The same precision is ensured for Jupiter and Saturn over 2000 years and
    for Uranus and Neptune over 6000 years before and after J2000.

	References:
	
	Bretagnon P., Francou G., : 1988, Astron. Astrophys., 202, 309.
	Meeus, J., "Astronomical Algorithms", pp. 205-208.

************************************************************************************/

void VSOP87Mercury ( double, double *, double *, double * );
void VSOP87Venus ( double, double *, double *, double * );
void VSOP87Earth ( double, double *, double *, double * );
void VSOP87Mars ( double, double *, double *, double * );
void VSOP87Jupiter ( double, double *, double *, double * );
void VSOP87Saturn ( double, double *, double *, double * );
void VSOP87Uranus ( double, double *, double *, double * );
void VSOP87Neptune ( double, double *, double *, double * );

/**************************  ELP2000Moon  **************************************

	Calculates the geocentric ecliptic longitude, latitude, and radius vector
    of the Moon on a given Julian date.
               
	void ELP2000Moon ( double jd, double *l, double *b, double *r )

	(jd): Julian date.
	 (l): Receives geocentric ecliptic longitude, in radians.
	 (b): Receives geocentric ecliptic latitude, in radians.
	 (r): Receives radius vector, in AU.
         
	Returns nothing.  (l) and (b) are referred to the mean ecliptic and
	equinox of date.

	References:
	
	Meeus, J. "Astronomical Algorithms". pp. 307-314.

	Chapront-Touze M., Chapront J. "ELP 2000-85: a semi-analytical lunar
	ephemeris adequate for historical times."  Astronomy & Astrophysics,
	vol. 190, p. 342 (1988).

	Chapront-Touze M., Chapront J. "The Lunar Ephemeris ELP 2000".
    Astronomy & Astrophysics, vol. 124, pp. 50-62 (1983).

************************************************************************************/

void ELP2000Moon ( double, double *, double *, double * );

/*********************  ELP2000SphericalToJ2000XYZ  *******************************

	Converts ELP2000 spherical coordinates referred to the ecliptic of date
	to rectangular coordinates referred to the J2000 ecliptic.
               
	void ELP2000SphericalToJ2000XYZ ( double jd, double l, double b, double r,
	double *x, double *y, double *z )

	(jd): Julian date.
	 (l): Geocentric ecliptic longitude, in radians.
	 (b): Geocentric ecliptic latitude, in radians.
	 (r): Radius vector, in AU.
     (x): Recieves J2000 ecliptic X coordinate, in A.U.
     (y): Recieves J2000 ecliptic Y coordinate, in A.U.
     (z): Recieves J2000 ecliptic Z coordinate, in A.U.
         
	Returns nothing.  The function implements Laskar's precession matrix as
	presented in the ELP 2000 papers described below, specifically for converting
	ELP2000 lunar coordinates from the ecltipic and equinox of date to the 
	ecltipic of J2000 in order to compare results against published values.

	References:
	
	Meeus, J. "Astronomical Algorithms". pp. 307-314.

	Chapront-Touze M., Chapront J. "ELP 2000-85: a semi-analytical lunar
	ephemeris adequate for historical times."  Astronomy & Astrophysics,
	vol. 190, p. 342 (1988).

	Chapront-Touze M., Chapront J. "The Lunar Ephemeris ELP 2000".
    Astronomy & Astrophysics, vol. 124, pp. 50-62 (1983).

************************************************************************************/

void ELP2000SphericalToJ2000XYZ ( double, double, double, double t,
double *, double *, double * );

/**************************  PLUTO95Pluto  **************************************

	Calculates Pluto's heliocentric rectangular coordinates on a given date.
               
	void PLUTO95Pluto ( double jd, double *x, double *y, double *z )

	(jd):    Julian date.
	(x,y,z): Receives Pluto's J2000 rectangular equatorial coordinates
	         on the given date.

	Returns nothing.  The rectangular coordinates (x,y,z) are returned in
	Astronomical Units, referred to the Earth's equator and equinox of J2000.
	
	The expressions used to compute Pluto's position are an analytic fit
	to the JPL numerical DE200/LE200 ephemerides.  The interval of validity
	of the fit is JD 2341972.5 - 2488092.5 (1 Jan 1700 - 24 Jan 2100).
	
	References:
	
	J. Chapront, G. Francou, "Representation of planetary ephemerides by
	frequency analysis."  Astronomy & Astrophysics Supplemental Series,
	109, 191 (1995).

************************************************************************************/

void PLUTO95Pluto ( double, double *, double *, double * );

/* Physical.c */

void PlanetographicCoordinates(double, double, double, double, double, double *, double *, double *);
void VectorPlanetographicCoordinates ( double[3], double, double, double, double *, double *, double * );
void SunRotation(double, double *, double *, double *);
void MercuryRotation(double, double *, double *, double *);
void VenusRotation(double, double *, double *, double *);
void EarthRotation(double, double *, double *, double *);
void MarsRotation(double, double *, double *, double *);
void JupiterRotation(double, double *, double *, double *, char);
void SaturnRotation(double, double *, double *, double *, char);
void UranusRotation(double, double *, double *, double *);
void NeptuneRotation(double, double *, double *, double *);
void PlutoRotation(double, double *, double *, double *);
void MoonRotation(double, double *, double *, double *);

/**************************  AngularSize  ********************************

	Computes an object's angular size as seen from a given distance.
	
	double AngularSize ( double diameter, double distance )
	
	(diameter): object's diameter. 
	(distance): object's distance, in same units as diameter.
	
	The function returns the object's angular diameter at the distance
	given in the (distance) parameter.  Note that the distance refers
	to the distance from the center of the object, not its surface. 

**************************************************************************/

double AngularSize ( double, double );

/**************************  PhaseAngle  **********************************

	Computes the angle between the sun and the viewer as seen from the
	object the viewer is observing.
	
	double PhaseAngle ( double sun[3], double viewer[3] )
	
	(sun):    sun's position vector relative to the object
	(viewer): viewer's position vector relative to the object
	
	The function returns the phase angle, in radians.  This is the
	angle between the sun and the viewer as seen from the object,
	and it determines the amount of the object's apparent disk that
	appears illuminated.  An object is fully illminated at a phase
	angle of zero degrees; it is completely dark at a phase angle
	of 180 degrees.
	
	Note that the input vectors may be expressed in any coordinate system,
	as long as they are the same for both vectors.  The units of the
	vectors need not be consistent; this function is only concerned
	with the directions of these vectors, not their magnitudes.

**************************************************************************/

double PhaseAngle ( double *, double * );

/**********************  IlluminatedFraction  ****************************

	Computes the illuminated fraction of a planet's disk from its phase
	angle.
	
	double IlluminatedFraction ( double phase )
	
	(phase): phase angle, in radians from zero to PI.
	
	The function returns the illuminated fraction of the planet's
	apparent disk as a floating-point value from zero to one.

**************************************************************************/

double IlluminatedFraction ( double );

/**************************  PlanetMagnitude  **********************************

	Computes the apparent magnitude of the major planets, given their distances
	from the sun and viewer, and phase angle.
	
	double MercuryMagnitude ( double r, double d, double phase )
	double VenusMagnitude ( double r, double d, double phase )
	double MarsMagnitude ( double r, double d, double phase )
	double JupiterMagnitude ( double r, double d, double phase  )
	double SaturnMagnitude ( double r, double d, double phase, double b0 )
	double UranusMagnitude ( double r, double d, double phase )
	double NeptuneMagnitude ( double r, double d, double phase )
	double PlutoMagnitude ( double r, double d, double phase )
	
	(d):     object's distance from viewer, in AU.
	(r):     object's distance from sun, in AU.
	(phase): object's phase angle, in radians.
	(b0):    Saturn's central latitude as seen by viewer.
	
	The functions return the planet's apparent magnitude using the formulae
	given in the Explanatory Supplement to the Astronomical Almanac, with
	the following modification: the absolute magnitudes used for the planets
	are those given in the Astronomical Almanac for the Year 1984, p. E88,
	and in Meeus, p. 270.  These values provide results which agree more
	closely with those actually tabulated in Astronomical Almanacs published
	after 1984 than the equivalent values listed in the Explanatory Supplement.
	    
	Note that for Saturn, the apparent magnitude is strongly dependent on
	the amount of light reflected by the rings, which in turn depends on the
	apparent inclination of the ring plane to the viewer.  Since the rings
	orbit in Saturn's equatorial plane, their inclination is equivalent to the
	planetographic latitude at the center of Saturn's apparent disk.  You
	can compute this value accurately enough for magnitude estimates with
	the function SaturnRingPlaneInclination().
	    
	References:
   
	The Explanatory Supplement to the Astronomical Almanac, pp. 401-407.
	The Astronomical Almanac for the Year 1984, p. E88.
	Jean Meeus, "Astronomical Algorithms", pp. 269-270.
	    
*******************************************************************************/

double MercuryMagnitude ( double, double, double );
double VenusMagnitude ( double, double, double );
double MarsMagnitude ( double, double, double );
double JupiterMagnitude ( double, double, double );
double SaturnMagnitude ( double, double, double, double );
double UranusMagnitude ( double, double, double );
double NeptuneMagnitude ( double, double, double );
double PlutoMagnitude ( double, double, double );

/***********************  SaturnRingPlaneInclination  ************************

	Computes the apparent inclination of Saturn's ring plane.
	
	double SaturnRingPlaneInclination ( double p[3], double d )
	
	(p): Saturn's position vector relative to viewer.
	(d): magnitude of vector p.
	
	The function returns the inclination of the ring plane to the viewer,
	in radians.  This is equivalent to the Saturnographic latitude at the
	center of Saturn's disk as seen from the viewer's position.  A return
	value of zero indicates that the rings are being viewed edge-on; a value
	of PI / 2 radians (i.e. 90 degrees) would indicate that the rings are
	being viewed from above Saturn's north pole.
	
	Saturn's position vector relative to the viewer (p) should be expressed
	in the J2000 equatorial coordinate frame.  The parameter (d) contains
	the magnitude of the vector, i.e. Saturn's distance from the viewer in
	the same units as (p).
	
	The function computes the ring plane inclination assuming fixed J2000
	coordinates of 40.66 degrees and 83.52 degrees for the right ascension
	and declination of Saturn's north pole.
	
	References:
	
	The Astronomical Almanac for the Year 1984, p. S30.
	
******************************************************************************/

double SaturnRingPlaneInclination ( double *, double );

/****************************  SunMagnitude  **********************************

	Computes the apparent magnitude of the sun.
	
	double SunMagnitude ( double d )

	(d): sun's distance from viewer, in AU.

	The function returns the Sun's apparent magnitude.  It is computed assuming
	an apparent magnitude of -26.72 at a distance of 1 AU.

*******************************************************************************/

double SunMagnitude ( double );

/**************************  MoonMagnitude  **********************************

	Computes the apparent magnitude of the moon, given its distance
	from the sun and viewer, and phase angle.

	double MoonMagnitude ( double r, double d, double phase )
	
	(d):     moon's distance from viewer, in AU.
	(r):     moon's distance from sun, in AU.
	(phase): moon's phase angle, in radians.
	
	The function returns the Moon's apparent magnitude.  The Explanatory
	Supplement to the Astronomical Almanac does not give an expression
	for the Moon's apparent magnitude.  This function computes the Moon's
	apparent magnitude as if the Moon were an asteroid with an absolute
	magnitude (H) of +0.21 and a phase parameter (G) of +0.25.  The absolute
	magnitude is taken from the Astronomical Almanac for the Year 1984,
	p. E88, and G = +0.25 is a typical value assumed for most asteroids.
	
	References:
	
	The Astronomical Almanac for the Year 1984, p. E88.
	The Explanatory Supplement to the Astronomical Almanac, p. 311.
	Jean Meeus, "Astronomical Algorithms", p. 217.
	
*******************************************************************************/
	    	
double MoonMagnitude ( double, double, double );

/**************************  AsteroidMagnitude  *******************************

	Computes the apparent magnitude of an asteroid, given its distance from
	the sun and viewer, phase angle, absolute magnitude, and phase parameter.
	
	double AsteroidMagnitude ( double r, double d, double phase, double h, double g )
	
	(d):     asteroid's distance from viewer, in AU.
	(r):     asteroid's distance from sun, in AU.
	(phase): asteroid's phase angle, in radians.
 	(h):     asteroid's absolute magnitude parameter.
	(g):     asteroid's phase parameter.
	
	The functions returns the asteroid's apparent magnitude.  This formula
	is only valid for phase angles less than 120 degrees.  It may return
	INFINITY if the input parameters result in an invalid magnitude.
		    
	The asteroid's absolute magnitude (h) is defined as its apparent magnitude
	at full illumination, at a distance of 1 AU from both the viewer and sun.
	The phase parameter (g) has no physical meaning, but describes how the
	magnitude varies with the phase angle.
	
	References:
   
	The Explanatory Supplement to the Astronomical Almanac, p. 311.
	Jean Meeus, "Astronomical Algorithms", p. 217.
	    
*******************************************************************************/

double AsteroidMagnitude ( double, double, double, double, double );

/**************************  AsteroidMagnitude  *******************************

	Computes the apparent magnitude of a comet, given its distance from
	the sun and viewer, phase angle, absolute magnitude, and heliocentric
	distance parameter.
	
	double CometMagnitude ( double r, double d, double phase, double h, double k )
	
	(d):     comet's distance from viewer, in AU.
	(r):     comet's distance from sun, in AU.
	(phase): comet's phase angle, in radians.
 	(h):     comet's absolute magnitude parameter.
	(k):     comet's heliocentric distance parameter.
	
	The functions returns the comet's apparent magnitude.  Note that comet
	magnitudes are extremely difficult to predict accurately, since they are
	heavily dependent on the amount of material released from the comet's
	surface.  Values computed by this function should only be considered
	rough estimates.
		    
	The comet's absolute magnitude (h) is defined as its apparent magnitude
	at full illumination, at a distance of 1 AU from both the viewer and sun.
	The phase parameter (k) describes how the absolute magnitude varies with
	the comet's distance from the sun.
	
	References:
   
	Jean Meeus, "Astronomical Algorithms", p. 216.
	    
*******************************************************************************/

double CometMagnitude ( double, double, double, double );

/* MarsMoon.c */

void SetPhobosMatrix ( double[3][3], double);
void SetDeimosMatrix ( double[3][3], double);
void PhobosOrbit(double, double *, double *, double *, double *, double *, double *);
void DeimosOrbit(double, double *, double *, double *, double *, double *, double *);
void PhobosRotation(double, double *, double *, double *);
void DeimosRotation(double, double *, double *, double *);

/* JupiMoon.c */

void SetJupiterMoonMatrix ( double[3][3] );
void IoXYZ(double, double *, double *, double *);
void EuropaXYZ(double, double *, double *, double *);
void GanymedeXYZ(double, double *, double *, double *);
void CallistoXYZ(double, double *, double *, double *);
void IoRotation(double, double *, double *, double *);
void EuropaRotation(double, double *, double *, double *);
void GanymedeRotation(double, double *, double *, double *);
void CallistoRotation(double, double *, double *, double *);

/* SatuMoon.c */

void SetSaturnMoonMatrix ( double[3][3] );
void MimasOrbit(double, double *, double *, double *, double *, double *, double *);
void EnceladusOrbit(double, double *, double *, double *, double *, double *, double *);
void TethysOrbit(double, double *, double *, double *, double *, double *, double *);
void DioneOrbit(double, double *, double *, double *, double *, double *, double *);
void RheaOrbit(double, double *, double *, double *, double *, double *, double *);
void TitanOrbit(double, double *, double *, double *, double *, double *, double *);
void HyperionOrbit(double, double *, double *, double *, double *, double *, double *);
void IapetusOrbit(double, double *, double *, double *, double *, double *, double *);
void PhoebeOrbit(double, double *, double *, double *, double *, double *, double *);
void MimasRotation(double, double *, double *, double *);
void EnceladusRotation(double, double *, double *, double *);
void TethysRotation(double, double *, double *, double *);
void DioneRotation(double, double *, double *, double *);
void RheaRotation(double, double *, double *, double *);
void TitanRotation(double, double *, double *, double *);
void IapetusRotation(double, double *, double *, double *);
void PhoebeRotation(double, double *, double *, double *);

/* UranMoon.c */

void SetUranusMoonMatrix ( double[3][3] );
void MirandaOrbit(double, double *, double *, double *, double *, double *, double *);
void ArielOrbit(double, double *, double *, double *, double *, double *, double *);
void UmbrielOrbit(double, double *, double *, double *, double *, double *, double *);
void TitaniaOrbit(double, double *, double *, double *, double *, double *, double *);
void OberonOrbit(double, double *, double *, double *, double *, double *, double *);
void MirandaRotation(double, double *, double *, double *);
void ArielRotation(double, double *, double *, double *);
void UmbrielRotation(double, double *, double *, double *);
void TitaniaRotation(double, double *, double *, double *);
void OberonRotation(double, double *, double *, double *);

/* NeptMoon.c */

void SetTritonMatrix ( double [3][3], double);
void SetNereidMatrix ( double [3][3] );
void TritonOrbit(double, double *, double *, double *, double *, double *, double *);
void NereidOrbit(double, double *, double *, double *, double *, double *, double *);
void CharonOrbit(double, double *, double *, double *, double *, double *, double *);
void TritonRotation(double, double *, double *, double *);
void CharonRotation(double, double *, double *, double *);

/**************************  Function in FITS.c ****************************/

/*****************************  NewFITSHeader  *****************************

	Creates or extends a FITS header matrix.
	
	int NewFITSHeader ( char ***header )
	
	(header): pointer to existing FITS header matrix, or NULL.
	
	The function returns TRUE if successful, or FALSE on failure.
	If successful, the pointer to the FITS header matrix passed in (*header)
	on input is replaced by a new header matrix pointer on output.  On failure,
	the input header matrix referenced by (*header) is unchanged.
	
	If the input header matrix pointer is NULL, this function creates a
	new matrix which contains 36 rows of 81 characters each.  The rows
	are filled with 80-character whitespace (' ') strings, each terminated
	by an ASCII NUL character ('\0').  The function returns an array which
	contains pointers to each of the header lines.  The last element of
	this array (i.e. header[36]) contains NULL to indicate the end of the
	array.  A pointer to the matrix is returned in (header).
	
	If (header) points to an existing FITS header matrix which has been
	previously allocated by this function, the function extends the
	existing header matrix instead of creating a new one.  It allocates
	a matrix row pointer array big enough to store pointers to all of
	the existing matrix rows plus 36 new ones.  The existing matrix row
	pointers are copied into the new array, and pointers to 36 new rows
	of 81 characters are added to the end of the new array.  The last
	element of the new array, after the last header matrix row, contains
	NULL.  If successful, the new matrix rows are filled with 80-character,
	ASCII-NUL-terminated whitespace strings; the original header matrix row
	pointer array is freed, and a pointer to the new matrix row pointer array
	is returned.  If a memory allocation failure occurrs, the original header
	matrix is unaffected and the function return NULL.
	
*****************************************************************************/

int NewFITSHeader ( char *** );

/***  CopyFITSHeader  *******************************************************

	Copies one FITS header matrix into another, or creates a clone of
	an existing FITS header.
	
	FITSHeader CopyFITSHeader ( FITSHeader source, FITSHeader dest )
	
	(source): pointer to source FITS header matrix.
	(dest):   pointer to destination FITS header matrix, or NULL.
	
	The function returns a pointer to the destination FITS header,
	if successful, or NULL on failure.
	
	If (dest) is NULL, this function allocates a new FITS header and
	copies the contents of the source FITS header (source) into it.
	
	If (dest) is non-null, the function copies lines from the source
	header into the destination header until it reaches the last line
	in either header.
	
*****************************************************************************/

FITSHeader CopyFITSHeader ( FITSHeader, FITSHeader );

/****************************  FreeFITSHeader  *****************************

	Frees memory for a FITS header matrix.
	
	void FreeFITSHeader ( char **header )
	
	(header): pointer to an existing FITS header matrix.
	
	Use this function to free memory for a FITS header matrix that was
	previously allocated by NewFITSHeader() or ReadFITSHeader().  After
	you call this function, the contents of the header matrix row pointer
	array, as well as the individual header matrix rows, will be destroyed.
	
*****************************************************************************/

void FreeFITSHeader ( char ** );

/*************************  ReadFITSHeader  *********************************

	Reads FITS header data from a file into a character matrix.
	
	int ReadFITSHeader ( FILE *file, char ***header )
	
	(file):   pointer to FITS file, opened for reading in binary mode.
	(header): recieves pointer to character matrix containing FITS
	          header data.
	          
	If successful, the function returns TRUE.  Otherwise, it returns
	FALSE.  On successful return, a pointer to the new FITS header matrix
	is returned in (*header).
	
	This function reads header data from the FITS file in 2880-byte blocks;
	each block consists of 36 lines of 80 characters each.  Each line is
	read into a separate row of the matrix.  The function continues to
	read blocks from the file until it encounters a block which contains
	the "END" keyword.
	
	Use FreeFITSHeader() to dispose of the header matrix allocated by
	this function when you no longer need it.
	
	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
*****************************************************************************/

int ReadFITSHeader ( FILE *, char ***header );

/*************************  WriteFITSHeader  *********************************

	Writes FITS header data from a character matrix into a FITS file.
	
	char **WriteFITSHeader ( FILE *file, char **header )
	
	(file): pointer to FITS file, opened for writing in binary mode.
	(header): pointer to character matrix containing header data.
	
	If successful, this function returns TRUE; on failure, it returns
	FALSE.
	
	The character matrix containing the FITS header data (header) must
	have been previously allocated by the functions NewFITSHeader() or
	ReadFITSHeader(), so that it contains an integer multiple of 36 rows
	of 80 characters each.  The function writes header data to the file
	in 2800-byte blocks; each block contains 36 rows of 80 characters.
	It writes blocks until it encounters a NULL row pointer in the
	header matrix, or until it encouters the a row containing the "END"
	keyword.

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
*****************************************************************************/

int WriteFITSHeader ( FILE *, char ** );

/*************************  GetFITSHeaderXXX  ******************************

	Returns keywords and values from strings containing FITS header lines.
	
	void GetFITSHeaderKeyword ( char *line, char *keyword )
	void GetFITSHeaderLogical ( char *line, int *logical );
	void GetFITSHeaderInteger ( char *line, long *integer );
	void GetFITSHeaderReal ( char *line, double *real );
	void GetFITSHeaderString ( char *line, char *string );

	(line):    pointer to string containing FITS header line
	(keyword): pointer to string which receives FITS header keyword
	(logical): pointer to integer which recieves logical value
	(integer): pointer to integer which recieves integer value
	(real):    pointer to integer which receives real value
	(string):  pointer to string which receives string value
	
	Use these functions to parse keywords and values from individual
	lines of a FITS header.  In each of these functions, the parameter
	(line) represents a pointer to a string containing a single line
	of FITS header data, e.g. as returned by GetFITSHeaderLine().
	
	GetFITSHeaderKeyword() obtains the 8-character keyword from the
	FITS header line.  The keyword is returned as an ASCII-NUL-terminated
	string in the (keyword) buffer, which must be able to store at least
	8 characters plus the terminating NUL character.
	
	To read the value contained in the FITS header line, use the following
	functions.  Which function to use depends on the type of data
	contained in the FITS header line, i.e. logical, integer, real, or
	character string.  The data types corresponding to specific FITS
	header keywords are defined in the FITS standard; e.g. the BITPIX
	keyword always represents an integer value.
	
	GetFITSHeaderLogical() returns the value contained in the FITS header
	line as a boolean (i.e. TRUE or FALSE).
	
	GetFITSHeaderInteger() returns the value contained in the FITS header
	line as an integer.
	
	GetFITSHeaderReal() returns the value contained in the FITS header
	line as a floating-point number.
	
	GetFITSHeaderString() obtains the value contained in the FITS header
	line as a string.  The value is returned as an ASCII-NUL terminated
	string in the parameter (string), which must be able to store at
	least 20 characters plus the terminating NUL character.

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
*****************************************************************************/

void GetFITSHeaderKeyword ( char *, char * );
void GetFITSHeaderLogical ( char *, int * );
void GetFITSHeaderInteger ( char *, long * );
void GetFITSHeaderReal ( char *, double * );
void GetFITSHeaderString ( char *, char * );

/*************************  SetFITSHeaderXXX  *******************************

	These functions write a keyword or value in a FITS header line.
	
	void SetFITSHeaderKeyword ( char *line, char *keyword );
	void SetFITSHeaderLogical ( char *line, int logical );
	void SetFITSHeaderInteger ( char *line, long integer);
	void SetFITSHeaderReal ( char *line, double real );
	void SetFITSHeaderCharacter ( char *line, char *string );

	(line):    pointer to string containing FITS header line to modify.
	(keyword): pointer to string containing FITS header line keyword
	(logical): logical value (TRUE or FALSE)
	(integer): integer value
	(real):    real value
	(string):  string value

	Use these functions to write keywords and values to individual
	lines of a FITS header.  In each of these functions, the parameter
	(line) represents a pointer to a string containing a single line
	of FITS header data, e.g. as returned by GetFITSHeaderLine().
	
	SetFITSHeaderKeyword() writes the 8-character keyword to the
	FITS header line.  The input (keyword) string must be an ASCII-NUL-
	terminated string of not more than eight characters (excluding
	the terminating NUL character).
	
	To write the value to the FITS header line, use the following
	functions.  Which function to use depends on the type of data
	you wish to write to the FITS header line, i.e. logical, integer,
	real, or character string.  The data types corresponding to specific
	FITS header keywords are defined in the FITS standard; e.g. the
	BITPIX keyword always represents an integer value.
	
	SetFITSHeaderLogical() writes a boolean value to the FITS header
	line (i.e. TRUE or FALSE).
	
	SetFITSHeaderInteger() writes an integer value to the FITS header
	line.
	
	GetFITSHeaderReal() writes a floating-point number to the FITS header
	line.
	
	GetFITSHeaderString() writes a string to the FITS header line.
	The input (string) value must be an ASCII-NUL terminated string
	containing not more than 20 characters (excluding the terminating
	NUL character).

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
*****************************************************************************/

void SetFITSHeaderKeyword ( char *, char * );
void SetFITSHeaderLogical ( char *, int );
void SetFITSHeaderInteger ( char *, long );
void SetFITSHeaderReal ( char *, double );
void SetFITSHeaderString ( char *, char * );

/***********************  TestFITSHeaderKeyword  ****************************

	Tests whether a line from a FITS header begins with a particular
	keyword.
	
	int TestFITSHeaderKeyword ( char *line, char *keyword )
	
	(line):    pointer to string containing FITS header line to test.
	(keyword): pointer to string containing keyword to test against.
	
	The function returns TRUE if the line begins with the specified
	keyword, and FALSE otherwise.
	
	The (line) string contains a line of FITS header data, e.g. as
	returned by GetFITSHeaderLine().
	
	The (keyword) string must be an	ASCII NUL-terminated string of up
	to 8 characters in length; if less than eight characters, only
	characters in (line) up to the length of the (keyword) string will
	be tested.  The keyword test is case-sensitive.
	
*****************************************************************************/

int TestFITSHeaderKeyword ( char *, char * );

/***********************  FindFITSHeaderKeyword  ****************************

	Searches for a FITS keyword in a character matrix containing FITS
	header data.
	
	int FindFITSHeaderLine ( char **header, char *keyword, long *n )
	
	(header):  pointer to character matrix containing FITS header data.
	(keyword): keyword to search for in character matrix.
	(n):       on input contains starting line number to search from;
	           on output recieves line number of line containing keyword.
	
	The function returns TRUE if it finds the specified keyword on any
	of the lines in the FITS header matrix, and FALSE if it fails.
	
	The string (keyword) specifies the keyword to search for; it must be
	an ASCII NUL-terminated string of up to eight characters in length, not
	counting the terminating NUL character.
	
	On input the parameter (n) specifies the line number of the FITS header
	matrix to start searching from, counting from zero.  On successful return,
	this parameter receives the line number on which the keyword was found.
	You can search for subsequent instances of the keyword by incrementing
	the line number returned by one, then calling this function again.
	
*****************************************************************************/

int FindFITSHeaderKeyword ( char **, char *, long * );

/***********************  CountFITSHeaderLines  *****************************

	Returns a pointer to string containing one line from a FITS header.
	
	long CountFITSHeaderLines ( char **header )
	
	(header): pointer to character matrix containing FITS header data.
	
	This function returns the number of lines in a FITS header matrix.
	
*****************************************************************************/

long CountFITSHeaderLines ( char ** );

/*************************  GetFITSHeaderLine  ******************************

	Returns a pointer to string containing one line from a FITS header.
	
	char *GetFITSHeaderLine ( char **header, long n )
	
	(header): pointer to character matrix containing FITS header data.
	(n): line number of desired line, counting from zero.
	
	This function returns a pointer to a specific line from a character
	matrix containing FITS header data.  The line number is specified
	by the parameter (i), counting from zero; i.e. to obtain a pointer
	to the first line, call GetFITSHeaderLine ( header, 0 ).
	
	If successful, the function will return a pointer to an 80-character
	ASCII NUL-terminated string containing the specified line of FITS
	header data.  This string is an actual row of data from the character
	matrix; you should not delete it or otherwise modify it directly.
	(Use the function SetFITSHeaderLine() to add or modify a specific FITS
	header line.)
	
	If you specify a line number which is greater than the number of
	lines in the matrix, the function will return a NULL pointer.
	
*****************************************************************************/

char *GetFITSHeaderLine ( char **, long );

/**************************  SetFITSHeaderLine  *****************************

	Adds or modifies a line in a character matrix containing FITS header data.
	
	int SetFITSHeaderLine ( char ***header, long n, char *line )
	
	(header): contains pointer to FITS header matrix.
	(n):      number of line you wish to add or modify.
	(line):   pointer to string containing new line.
	
	The function returns TRUE if successful, or FALSE on failure.
	
	The parameter (header) contains, on input, a pointer to the character
	matrix containing the FITS header data you wish to add to or modify.
	If successful, the function returns a pointer to the modified matrix
	in the same parameter (header).  The modified matrix pointer may be
	different from the original matrix pointer if the function needs to
	allocate storage for a new line.
	
	The parameter (n) specifies the number of the line you wish to modify.
	It is counted from zero, i.e. n = 0 indicates the first line in the
	header.  If you pass a value for (n) which is greater than the number
	of lines already in the header, the function will re-allocate the
	matrix and add the new line to the end of the existing header data.
	
	The parameter (line) is a pointer to an ASCII-NUL-terminated string
	containing the data you wish to copy into the FITS header line specified
	in the first two parameters.  As FITS header lines can contain no more
	than 80 characters, only data from the first 80 characters, or up to
	the first NUL character in (line), will be copied into the header matrix.
	
*****************************************************************************/

int SetFITSHeaderLine ( char ***, long n, char * );

/*********************  SetFITSHeaderLineKeywordXXX  ************************

	Adds or modifies a keyword and value in a line of data from a FITS header
	matrix.

	int SetFITSHeaderLineKeyword ( char ***header, long n, char *keyword )
	
	int SetFITSHeaderLineKeywordLogical ( char ***header, long n, char *keyword,
	int value )
	    
	int SetFITSHeaderLineKeywordInteger ( char ***header, long n, char *keyword,
	long value )
	    
	int SetFITSHeaderLineKeywordReal ( char ***header, long n, char *keyword,
	double value )
	    
	int SetFITSHeaderLineKeywordString ( char ***header, long n, char *keyword,
	char *value )

	(header): pointer to FITS header matrix
	(line): number of line to modify or add
	(keyword): keyword to add or modify on line
	(value): value to add to or modify on line
	
	All of these function return TRUE if successful, and FALSE otherwise.

	You can use these functions as a means of conveniently storing a
	FITS header keyword and the corresponding logical, integer, real,
	or character-string value on a specific line of a FITS header
	matrix.
	
	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
*****************************************************************************/

int SetFITSHeaderLineKeyword ( char ***, long, char * );
int SetFITSHeaderLineKeywordLogical ( char ***, long, char *, int );
int SetFITSHeaderLineKeywordInteger ( char ***, long, char *, long );
int SetFITSHeaderLineKeywordReal ( char ***, long, char *, double );
int SetFITSHeaderLineKeywordString ( char ***, long, char *, char * );

/***********************  SetFITSImageHeaderInfo  ***************************

	Adds the minimal keywords required for a FITS image file to a FITS
	header character matrix.
	
	void SetFITSImageHeaderInfo ( char ***header, long bitpix, long naxis,
	long naxis1, long naxis2, long naxis3, double bzero, double bscale )
	
	(header): contains/receives pointer to FITS header matrix.
	(bitpix): desired bits/pixel in image data.
	(naxis):  number of axes in image data matrix.
	(naxis1): number of columns in image data matrix
	(naxis2): number of rows in image data matrix
	(naxis3): number of planes in image data matrix
	(bzero):  image data value offset parameter
	(bscale): image data scaling parameter
	
	The parameter (header) contains a pointer to a blank character
	matrix, e.g. as allocated by NewFITSHeader().  If (*header) is
	NULL, this function will allocate a new FITS header matrix for
	you.  On output, (header) will contain a pointer to the new
	FITS header matrix.
	
	Use the function FreeFITSHeader() to free memory allocated by this
	function when it is no longer needed.

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
****************************************************************************/

void SetFITSImageHeaderInfo ( char ***, long, long, long, long, long, double, double );

/*************************  GetFITSImageHeaderInfo  *************************

	Finds FITS image data format parameters from a character matrix
	containing FITS header data.
	 
	void GetFITSImageHeaderInfo ( char **header, long *bitpix, long *naxis,
	long *naxis1, long *naxis2, long *naxis3, double *bzero, double *bscale )

	(header): pointer to character matrix containing FITS header data.
	(bitpix): receives bits/pixel in image data matrix
	(naxis):  receives number of axes in image data matrix
	(naxis1): receives number of columns in image data matrix
	(naxis2): receives number of rows in image data matrix
	(naxis3): receives number of planes in image data matrix
	(bzero):  receives data offset parameter
	(bscale): receives data scale parameter
	
	This function makes no attempt to validate the parameters; it simply
	reads them verbatim from the FITS header matrix.  If any of the
	FITS header keywords are missing, the contents of the corresponding
	function argument will remain unchanged.  Therefore, before calling
	this function, you may wish to set default values for these parameters
	as follows:
	
	BITPIX  8
	NAXIS   0
	NAXIS1  1
	NAXIS2  1
	NAXIS3  1
	BZERO   0.0
	BSCALE  1.0

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
******************************************************************************/

void GetFITSImageHeaderInfo ( char **, long *, long *, long *, long *, long *,
double *, double * );

/****************************  ByteSwap  ************************************

	Reverses the byte-ordering of a number of values in an array.
	
	void ByteSwap ( void *buffer, long num, short size );

	(buffer): pointer to the beginning of the array.
	(num): number of elements in the array.
	(size): size (in bytes) of an individual element of the array.
	
*****************************************************************************/

void ByteSwap ( void *, long, short );

/************************  NewFITSImageDataMatrix  **************************

	Allocates memory for a matrix of FITS image data.
	
	PIXEL ***NewFITSImageDataMatrix ( long naxis1, long naxis2, long naxis3 )
	
	(naxis1): number of columns in image data matrix
	(naxis2): number of rows in image data matrix
	(naxis3): number of frames in image data matrix
	
	The function returns a pointer to the new image data matrix if successful,
	and NULL on failure.  If successful, the new matrix will be initialized
	with zeros.  Use the function FreeFITSImageDataMatrix() to free memory
	allocated by this function.
	
	FITS image data matrices are three-dimensional, organized into one or more
	frames of image data.  Each frame is a two-dimentional matrix, consisting
	of one or more rows of data.
	
	If successful, this function returns an array of pointers to the individual
	frames of the image data matrix.  Each frame, in turn, consists of an array
	of pointers to the rows of image data which make up that frame.  Memory for
	each row of image data is allocated separately, but data within a particular
	row is contiguous in memory.
	
	To reference the ith frame of the matrix, use the notation matrix[i];
	to reference the jth row within the ith frame, use matrix[i][j].  This
	will return a pointer to the start of the row; to access the kth pixel
	within that row, use the notation matrix[i][j][k].  Here the integers
	i, j, and k are all counted from zero as per the C language standard.
	
******************************************************************************/

PIXEL ***NewFITSImageDataMatrix ( long, long, long );

/************************  FreeFITSImageDataMatrix  **************************

	Frees memory for a matrix of FITS image data.
	
	void FreeFITSImageDataMatrix ( PIXEL ***matrix )
	
	(matrix): pointer to the matrix.
	
	Use this function to free memory that was allocated by the function
	NewFITSImageDataMatrix().
	
******************************************************************************/

void FreeFITSImageDataMatrix ( PIXEL ***matrix );

/**********************  ReadFITSImageDataMatrix  *****************************

	Reads image data from a FITS file into an image data matrix.
	
	int ReadFITSImageDataMatrix ( FILE *file, PIXEL ***matrix,
	long bitpix, long naxis, long naxis1, long naxis2, long naxis3,
	double bzero, double bscale )

	(file):   pointer to the FITS file, opened for reading in binary mode.
	(matrix): pointer to image data matrix to receive data.
	(bitpix): bits-per-pixel in FITS file image data matrix.
	(naxis):  number of axes in FITS file image data matrix.
	(naxis1): number of column in FITS file image data matrix.
	(naxis2): number of rows in FITS file image data matrix.
	(naxis3): number of frames in FITS file image data matrix.
	(bzero):  data offset parameter in FITS file image data matrix.
	(bscale): data scaling parameter in FITS file image data matrix.
	
	This function returns TRUE if successful, and FALSE on failure.
	
	Use the function to read an entire matrix of image data from a FITS
	file.  The function assumes that the file has been opened for reading
	in binary mode, that the file's header has been read, and that the
	file read mark is queued up to the start of the file's image data matrix.

	The matrix into which image data will be read must have already been
	allocated by the function NewFITSImageDataMatrix().  The dimensions of
	the image data matrix (naxis, naxis1, naxis2, naxis3) and pixel value
	offsets and scaling (bzero, bscale) are assumed to have been read from
	the file's FITS image header, e.g. by the function GetFITSHeaderInfo().  
	Any data already residing in the matrix will be overwritten by this
	function.
	
	Note that the function does not read the zero-padding at the end of
	the image data matrix in the file.  You should call the function
	ReadFITSImageDataPadding() after reading the image data matrix in order
	to skip over the padding, and move the file read mark to the next block
	of header information or data stored in the FITS file.

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
*******************************************************************************/

int ReadFITSImageDataMatrix ( FILE *, PIXEL ***, long, long, long, long, long,
double, double );

/**********************  WriteFITSImageDataMatrix  *****************************

	Writes image data from an image data matrix to a FITS file.
	
	int WriteFITSImageDataMatrix ( FILE *file, PIXEL ***matrix,
	    long bitpix, long naxis, long naxis1, long naxis2, long naxis3,
	    double bzero, double bscale )

	(file):   pointer to the FITS file, opened for writing in binary mode.
	(matrix): pointer to image data matrix containing data.
	(bitpix): bits-per-pixel in FITS file image data matrix.
	(naxis):  number of axes in FITS file image data matrix.
	(naxis1): number of column in FITS file image data matrix.
	(naxis2): number of rows in FITS file image data matrix.
	(naxis3): number of frames in FITS file image data matrix.
	(bzero):  data offset parameter in FITS file image data matrix.
	(bscale): data scaling parameter in FITS file image data matrix.
	
	This function returns TRUE if successful, and FALSE on failure.
	
	Use the function to write an entire matrix of image data to a FITS
	file.  The function assumes that the file has been opened for writing
	in binary mode, that the file's header has been written, and that the
	file write mark is queued up to the start of the image data matrix.

	The dimensions of the image data matrix (naxis, naxis1, naxis2, naxis3)
	and pixel value offsets and scaling (bzero, bscale) are assumed to be
	the same as those contained in the file's FITS header, e.g. as by the
	function SetFITSHeaderInfo().  
	
	Note that this function DOES NOT zero-pad the image data matrix; you
	should call the function WriteFITSImageDataPadding() to assure that
	the length of the image data matrix in the file is an integer multiple
	of 2880 bytes.

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
*******************************************************************************/

int WriteFITSImageDataMatrix ( FILE *, PIXEL ***, long, long, long, long, long,
double, double );

/************************  ReadFITSImageDataRow  ****************************

	Reads one row of image data from a FITS image file.

	int ReadFITSImageDataRow ( FILE *file, long bitpix, long naxis1,
	double bzero, double bscale, PIXEL *data )

	(file): pointer to the FITS image file, opened for reading in binary mode.
	(bitpix): bits/pixel for data in the input image file.
	(naxis1): number of pixels in an image row.
	(bzero): offset added to raw image data values.
	(bscale): scaling applied to raw image data values.
	(data): pointer to buffer to receive output image data values.
	
	This function returns TRUE if successful, or FALSE on failure.
	
	The output image data array (data) must be able to contain at least
	(naxis1) values, which respesent the pixels along one row of the image.
	These are read from the input file (file), which must have previously
	been opened for reading in binary mode.  The parameters (bzero) and
	(bscale) are the offset and scaling applied to the image data after it
	has been read from the file, as follows:
	
	output data value = ( value written to file + bzero ) * bscale
	
	The parameter (bitpix) describes the number of bits per pixel
	for values in the input image file.  Legal values for (bitpix)
	are 8, 16, 32, -32, and -64, and correspond to unsigned 8-bit
	integer, signed 16-bit integer, signed 32-bit integer, 32-bit
	floating point, and 64-bit floating point representation, as
	specified in the FITS standard.  Note that values in the output
	data array may be truncated if they overflow the data type indicated
	by the (bitpix) parameter.  This function may also perform byte-swapping
	if the native byte-ordering on the platform on which the function is
	currently running differs from that specified in the FITS standard.
	(i.e. most-significant-byte-last).

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
******************************************************************************/

int ReadFITSImageDataRow ( FILE *, long, long, double, double, PIXEL * );

/************************  WriteFITSImageDataRow  ****************************

	Writes one row of image data to a FITS image file.

	int WriteFITSImageDataRow ( FILE *file, long bitpix, long naxis1,
	double bzero, double bscale, PIXEL *data )
	
	(file): pointer to the FITS image file, opened for writing in binary mode.
	(bitpix): desired bits/pixel for data in output image file.
	(naxis1): number of pixels in an image row.
	(bzero): offset subtracted from pixel values before writing.
	(bscale): scaling applied to pixel values before writing.
	(data): pointer to buffer containing image data values.
	
	This function returns TRUE if successful, or FALSE on failure.
	
	The input image data array (data) must contain at least (naxis1)
	elements, which respesent the pixel value along one row of the image.
	These are written to the output file (file), which must have previously
	been opened for writing in binary mode.  The parameters (bzero) and
	(bscale) describe an optional offset and scaling applied to the image
	data before it is written to the file as follows:
	
	value written to file = ( input data value - bzero ) / bscale
	
	If (bzero) is 0.0 and (bscale) is 1.0, no offset or scaling is
	applied.
	
	The parameter (bitpix) describes the number of bits per pixel
	in values written to the output image data file.  Legal values
	for (bitpix) are 8, 16, 32, -32, and -64, and correspond to
	unsigned 8-bit integer, signed 16-bit integer, signed 32-bit
	integer, 32-bit floating point, and 64-bit floating point
	representation, as specified in the FITS standard.  Note that
	values in the input data array may be truncated if they overflow
	the data type indicated by the (bitpix) parameter.  This function
	may also perform byte-swapping as necessary to ensure that data
	values are written with the byte-orering required by th FITS standard
	(i.e. most-significant-byte-last).

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
******************************************************************************/

int WriteFITSImageDataRow ( FILE *, long, long, double, double, PIXEL * );

/***********************  ReadFITSImageDataPadding  **************************

	Reads the padding at the end of a FITS image file.
	
	int ReadFITSImageDataPadding ( FILE *file, long bitpix, long naxis,
	long naxis1, long naxis2, long naxis3 )
	
	(file):   pointer to the file, opened for reading in binary mode
	(bitpix): bits/pixel in image data
	(naxis):  number of axes in image data matrix
	(naxis1): number of columns in image data matrix
	(naxis2): number of rows in image data matrix
	(naxis3): number of frames in image data matrix
	
	The function returns TRUE if successful, and FALSE otherwise.
	
	Because the FITS image standard specifies that images must consist of
	an integral number of 2880-byte blocks, the last image data block must
	be padded with zeros if the size of the image data matrix plus does not
	equal an integer multiple of 2880.
	
	You may call this function after reading the image data matrix to skip
	over the padding and move the file i/o mark to the start of the next
	header-data unit.  In practice, howevever, many FITS image writing
	programs may fail to pad image files if the file only contains a single
	header-image pair, and you should not be concerned if failure occurs in
	this case.

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
******************************************************************************/

int ReadFITSImageDataPadding ( FILE *, long, long, long, long, long );

/***********************  WriteFITSImageDataPadding  *************************

	Writes the padding at the end of a FITS image file.
	
	int WriteFITSImageDataPadding ( FILE *file, long bitpix, long naxis,
	long naxis1, long naxis2, long naxis3 )
	
	(file):   pointer to the file, opened for writing in binary mode
	(bitpix): bits/pixel in image data
	(naxis):  number of axes in image data matrix
	(naxis1): number of columns in image data matrix
	(naxis2): number of rows in image data matrix
	(naxis3): number of frames in image data matrix
	
	The function returns TRUE if successful, and FALSE otherwise.
	
	Because the FITS image standard specifies that images must consist of
	an integral number of 2880-byte blocks, the last image data block must
	be padded with zeros if the size of the image data matrix does not
	equal an integer multiple of 2880.
	
	You should call this function after writing the image data matrix to
	properly pad the image data to its correct length.  After a successful
	return, you may then be able to write additional header-data units
	to the file.

	References:
	
	Wells, D.C., et. al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
******************************************************************************/

int WriteFITSImageDataPadding ( FILE *, long, long, long, long, long );

/****************************  NewFITSImage  *********************************

	Allocates and initializes a new FITSImage structure for storing FITS
	image header and/or image data.
	
	FITSImage *NewFITSImageHeader ( long bitpix, long naxis, long naxis1,
	long naxis2, long naxis3, double bzero, double bscale )

	FITSImage *NewFITSImage ( long bitpix, long naxis, long naxis1,
	long naxis2, long naxis3, double bzero, double bscale )
	
	(bitpix): bits per pixel in image data matrix.
	(naxis):  number of axes in image data matrix.
	(naxis1): number of columns in image data matrix.
	(naxis2): number of rows in image data matrix.
	(naxis3): number of frames in image data matrix.
	(bzero):  data offset parameter in image data matrix.
	(bscale): data scaling parameter in image data matrix.

	If successful, these functions return a pointer to the new FITSImage
	structure; on failure, they will return NULL.  Use the function
	FreeFITSImage() to release memory allocated by these functions.
	
	The parameters (naxis1), (naxis2), and (naxis3) provide the dimensions
	of the image data matrix.  The paramters (bitpix), (bzero), and (bscale)
	are not needed to manipulate the image data in memory, but are needed
	to define the format with which image data values are written to disk
	(see WriteFITSImage() and WriteFITSImageDataRow() for more information.)
	
	The function NewFITSImageHeader() allocates and initializes a FITSImage
	structure according to the parameters passed.  A character matrix
	containing an initialized FITS header will also be allocated, and a
	pointer to it will be stored in the "header" member of the structure.
	Memory for storing image data will NOT be allocated, and the "data"
	member of the structure will be set to NULL.
	
	The function NewFITSImage() is identical to NewFITSImageHeader(), except
	that in addition to the header, it also allocates memory for an image
	data matrix.  A pointer to this matrix will be returned in the structure's
	"data" member, and the elements of the matrix will be initialized with
	zeros.
	
******************************************************************************/

FITSImage *NewFITSImageHeader ( long, long, long, long, long, double, double );
FITSImage *NewFITSImage ( long, long, long, long, long, double, double );

/****************************  FreeFITSImage  ********************************

	Frees memory for a FITSImage structure.
	
	void FreeFITSImage ( FITSImage *image )
	
	(image): pointer to FITS image record.
	
	Use this function to free memory for a FITSImage structure that was
	allocated by the functions NewFITSImage(), NewFITSImageHeader(),
	ReadFITSImage(), ReadFITSImageHeader(), etc.
	
	If the FITSImage structure being destroyed contains pointers to
	a FITS header and/or image data matrix, memory for those matrices
	will also be deallocated; if you wish to prevent this, you should
	set the FITSImage's "header" and "data" structure members to point
	to NULL before calling this function.
	
******************************************************************************/

void FreeFITSImage ( FITSImage * );

/****************************  ReadFITSImage  *********************************

	Allocates and initializes a new FITSImage, and reads FITS header and/or
	image data from a FITS file into it.
	
	FITSImage *ReadFITSImageHeader ( FILE *file )
	FITSImage *ReadFITSImage ( FILE *file )
	
	(file): pointer to FITS file, opened for reading in binary mode.
	
	If successful, these functions return a pointer to the new FITSImage
	structure; on failure, they will return NULL.  Use the function
	FreeFITSImage() to release memory allocated by these functions.
	
	The (file) parameter is a standard C I/O library file pointer, e.g.
	as returned by fopen().  It must refer to a FITS file which has been 
	opened for reading in binary mode.
	
	The function ReadFITSImageHeader() allocates and initializes a FITSImage
	structure.  It allocates a character matrix to store the file's FITS
	image header, and reads the file header into it.  It then scans the
	header to obtain the parameters which describe the dimensions (naxis,
	naxis1, naxis2, naxis3) and formatting (bitpix, bzero, bscale) of the
	image data matrix stored in the file.  It returns these parameters
	in the relevant FITSImage structure fields, but does not read the
	image data matrix from the file or allocate memory to store it.
	The structure's "data" pointer is set to NULL to indicate that no
	image data matrix storage has been allocated.
	
	The function ReadFITSImage() is identical to ReadFITSImageHeader(), except
	that in addition to the header, it also allocates memory for and reads
	the image data matrix.  A pointer to this matrix will be returned in the
	structure's "data" member.
	
******************************************************************************/

FITSImage *ReadFITSImageHeader ( FILE * );
FITSImage *ReadFITSImage ( FILE * );

/***************************  ReadFITSImageData  ******************************

	Allocates a new FITS image data matrix, and reads image data from a FITS
	file into it.
	
	PIXEL ***ReadFITSImageData ( FILE *file, FITSImage *image )
	
	(file):  pointer to FITS file, opened for reading in binary mode.
	(image): pointer to FITSImage structure describing format of image data.
	
	If successful, this function returns a pointer to a new FITS image
	data matrix; on failure, it will return NULL.  Use the function
	FreeFITSImageDataMatrix() to release memory allocated by this function.
	
	The (file) parameter is a standard C I/O library file pointer, e.g.
	as returned by fopen().  It must refer to a FITS file which has been 
	opened for reading in binary mode.  Additionally, the file read mark
	must be cued up to the start of the image data matrix (i.e., the
	function assumes that the file's header has already been read.)
	
	The (image) parameter is a pointer to an initialized FITSImage
	structure which describes the dimensions and formatting of the
	image data matrix in the FITS file.  This can be obtained with the
	function ReadFITSImageHeader().
	
******************************************************************************/

PIXEL ***ReadFITSImageData ( FILE *, FITSImage * );

/****************************  WriteFITSImage  *******************************

	Writes image header and/or data information from a FITSImage record
	in memory to a FITS file on disk.
	
	int WriteFITSImageHeader ( FILE *file, FITSImage *image )
	int WriteFITSImage ( FILE *file, FITSImage *image )
	
	(file):  pointer to FITS file, opened for writing in binary mode.
	(image): pointer to FITSImage structure containing image header/data.
	
	If successful, these functions return TRUE; on failure they return
	FALSE.
	
	The (file) parameter is a standard C I/O library file pointer, e.g.
	as returned by fopen().  It must refer to a FITS file which has been 
	opened for writing in binary mode.
	
	The function WriteFITSImageHeader() writes the header data stored in
	the image's "header" character matrix to the file.  It does not write
	the image data matrix, however.  Call the function WriteFITSImage()
	to write both the image header and data matrix to the file.
	
******************************************************************************/

int WriteFITSImageHeader ( FILE *, FITSImage * );
int WriteFITSImage ( FILE *, FITSImage * );

/*** ResizeFITSImage ***********************************************************

	Allocates a new FITS image from an existing one, and optionally resizes its
	data matrix.
	
	FITSImage *ResizeFITSImage ( FITSImagePtr fits, short naxis1, short naxis2,
	           short naxis3, int copy )
	
	(naxis1): number of columns in new FITS image data matrix
	(naxis2): number of rows in new FITS image data matrix
	(naxis3): number of frames in new FITS image data matrix
	(copy):   if TRUE, copy data from the old FITS image data matrix into
	          the new one.  if FALSE, leave new FITS image data uninitialized.
	          
	If successful, the function returns a pointer to a new FITS image containing
	a data matrix of the specified size.  The FITS header in the new image will
	be a copy of the header in the previous image, except that the keywords
	defining the image size will be updated to reflect the image's new size.
	All other fields of the new FITS image record will be identical to those
	of the old one.
	
	If the (copy) parameter is TRUE, image data values are copied from locations
	in the existing FITS image data matrix into corresponding locations in the
	new FITS image data matrix.  Pixels in the new image data matrix which have
	no corresponding location in the previous matrix (e.g. if the new image is
	larger than the old one) will be initialized with zeros.
	
	If (copy) is FALSE, image data values are NOT copied from the old FITS image
	into the new one; the new FITS image's data matrix contains uninitialized
	data.
	
	On failure, the function returns NULL.
	
*********************************************************************************/
	
FITSImage *ResizeFITSImage ( FITSImage *, short, short, short, int );

/*********************  GetFITSTableHeaderInfo  ******************************

	Obtains FITS table formatting parameters from a character matrix
	containing FITS table header information.
	
	void GetFITSTableHeaderInfo ( char **header, long *naxis1, long *naxis2,
	long *tfields )
	
	(header):  character matrix containing FITS table header.
	(naxis1):  receives number of columns in FITS table data matrix.
	(naxis2):  receives number of rows in FITS table data matrix.
	(tfields): receives number of fields per row in FITS table data matrix.
	
	Use this function to read the dimensions of a FITS table.
	
	The parameter (header) is a pointer to a character matrix which contains
	the table's FITS header, which has been previously read from the table
	file (e.g. by the function ReadFITSHeader().)
	
	The number of rows in the table is returned in (naxis1).  The number of
	characters (i.e. columns) per row is returnd in (naxis2).  The number
	of fields per row is returned in (tfields).  Values for all three of
	these parameters are obtained by searching the header for the NAXIS1,
	NAXIS2, and TFIELDS keywords; if these keywords are not found, the
	corresponding function arguments are left unchanged.  This function
	makes no attempt to validate the FITS header, so you may wish to set
	the values of the function arguments to invalid defaults (e.g. zero)
	before calling the function, then checking to see whether they have
	changed afterwards.
	
	After you have called this function to determine the number of fields
	per row in the table, you should call the function GetFITSTableFieldInfo()
	to determine the formatting for each field.

	References:
	
	Wells, D. C., et al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
	Grosbol, P., et al.  "Generalized Extensions and Blocking Factors
	for FITS".  Astronomy & Astrophysics Supplement Series, vol. 73,
	June 1988, pp. 359-964.
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
	
******************************************************************************/

void GetFITSTableHeaderInfo ( char **, long *, long *, long * );

/************************  SetFITSTableHeaderInfo  **************************

	Writes FITS table formatting parameters to a FITS header character
	matrix.
	
	void SetFITSTableHeaderInfo ( char ***header, long naxis1, long naxis2,
	long tfields )
	
	(header):  pointer to character matrix to receive FITS table header.
	(naxis1):  number of columns in FITS table data matrix.
	(naxis2):  number of rows in FITS table data matrix.
	(tfields): number of fields per row in FITS table data matrix.
	
	The parameter (*header) contains a pointer to a blank character
	matrix, e.g. as allocated by NewFITSHeader().  If (*header) is
	NULL, this function will allocate a new FITS header matrix for
	you.  On output, (header) will contain a pointer to a new
	character matrix containing FITS header data.
	
	This function adds the minimal FITS header keywords required to
	write a valid FITS table file.  However, it does not add any
	of the table field format keywords.  You must add field formatting
	parameters for each of the (tfields) fields or the resulting
	FITS header will be invalid (see the function SetFITSTableFieldInfo()
	for more information).
	
	Use the function FreeFITSHeader() to free memory allocated by this
	function when it is no longer needed.

	References:
	
	Wells, D. C., et al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
	Grosbol, P., et al.  "Generalized Extensions and Blocking Factors
	for FITS".  Astronomy & Astrophysics Supplement Series, vol. 73,
	June 1988, pp. 359-964.
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
		
******************************************************************************/

void SetFITSTableHeaderInfo ( char ***, long, long, long );

/*********************  GetFITSTableHeaderFieldInfo  ************************

	Obtains a FITS table field's formatting parameters from a character
	matrix containing FITS table header information.
	
	void GetFITSTableHeaderInfo ( char **header, long field, long *tbcol,
	char *tform, char *ttype, char *tunit, double *tzero, double *tscal,
	char *tnull )
	
	(header): character matrix containing FITS table header
	(field):  number of field to obtain information for
	(tbcol):  receives column in which field starts
	(tform):  receives field width and format string
	(ttype):  receives field data type string
	(tunit):  receives field data units string
	(tzero):  receives field data offset parameter
	(tscal):  receives field data scale parameter
	(tnull):  receives string which indicates null value for field
	
	Use this function to determine the formatting of a particular field
	in a FITS table file from a character matrix containing the file's
	FITS table header data.  The (header) parameter represents this
	character matrix; it must have been read from the file by the function
	ReadFITSHeader() or an equivalent.
	
	The parameter (field) is the number of the field you wish to determine
	formatting parameters of, counting from 1 for the first field.
	
	The variable (tbcol) receives the column at which the field starts,
	again counting from 1 (e.g. if tbcol is 5, the field starts at the
	5th column).
	
	The argument (tform) is a pointer to a buffer which receives the
	field's length and format as an ASCII NUL-terminated string.  The
	string describes the field's format in FORTRAN notation, and will
	be of the form "qww.dd" where
	
	q is the field data type, which can be one of:
	  
	  A (ASCII or character data)
	  I (Integer data)
	  F (Floating point data)
	  E (floating point data in Exponential notation)
	  D (floating point data in Double-precision exponential notation)
	  
	ww is the width of the field;
	dd is the decimal precision of the field (only present for the
	   F, E, and D field data types)
	
	For example, a field format string of "A14" indicates that the field
	is 14 characters wide and contains character data.  A field format of
	"I7" indicates a field seven characters wide containing integer
	data.  A field format of "F12.4" indicates a 12-character-wide floating
	point data with 4 digits after the decimal point, and "D17.9" indicates
	a 17-character-wide field containing double-precision floating-point
	data given to 9 significant decimals.
	
	Note that a valid (tform) string is mandatory; if one is not present,
	data cannot be read from the FITS file.
	
	The parameter (ttype) is a pointer to a buffer which will receive the
	field's type or label as an ASCII NUL-termated string.  This string names
	the quantity that data in the field actually measures, e.g. "Declination".
	This function assumes that the buffer can hold a string of 20 characters
	plus the terminating NUL; if the (ttype) string given in the header
	exceeds that length it will be truncated.
	
	The parameter (tunit) is a pointer to a buffer which will receive the
	field's units as an ASCII NUL-termated string.  This string names
	the units on the data in the field, e.g. "Degrees".  This function
	assumes that the buffer can hold a string of 20 characters plus the
	terminating NUL; if the (ttype) string given in the header exceeds that
	length it will be truncated.

	The parameters (tzero) and (tscale) are an optional offset and scaling
	that should be applied to raw numerical values read from the field; these
	are only relevant for integer or floating-point fields.  If present, they
	should be applied as follows:
	
	actual data value = raw data value * tscal + tzero
	
	The parameter (tnull) is a pointer to a buffer which will receive an
	ASCII NUL terminated string describing the null value for the field.
	If the contents of a field are equal to this string, then the field's
	data value should be assumed to be blank, not present, or unknown.
	
	This function works by searching the FITS header character matrix for
	the TBCOL, TFORM, TTYPE, TUNIT, TZERO, TSCAL, and TNULL keywords.  If
	these are not found, then the values in the corresponding function
	arguments will be unchanged.  This function makes no attempt to validate
	the values found; you should set defaults before calling it, and check
	them afterwards.  Suggested defaults are:
	
	TBCOL	0
	TFORM	""
	TTYPE	""
	TUNIT	""
	TZERO	0.0
	TSCAL	1.0
	TNULL	""
	
	(Here "" indicates an empty or zero-length string.)
	
	Note that the TTYPE, TUNIT, TZERO, TSCAL, and TNULL keywords are optional.
	However, the TBCOL and TFORM keywords must be preent and have legal values
	or the table field data will not be readable.
	
	References:
	
	Wells, D. C., et al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
	Grosbol, P., et al.  "Generalized Extensions and Blocking Factors
	for FITS".  Astronomy & Astrophysics Supplement Series, vol. 73,
	June 1988, pp. 359-964.
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
	
******************************************************************************/

void GetFITSTableHeaderFieldInfo ( char **, long, long *, char *, char *, char *,
     double *, double *, char * );
	
/*********************  SetFITSTableHeaderFieldInfo  ************************

	Obtains a FITS table field's formatting parameters from a character
	matrix containing FITS table header information.
	
	void SetFITSTableHeaderInfo ( char ***header, long field, long tbcol,
	     char tform, char ttype, char tunit, double tzero, double tscal,
	     char tnull )
	
	(header): character matrix to receive modified FITS table header
	(field):  number of field to add formatting information for
	(tbcol):  column in which field starts
	(tform):  field width and format string
	(ttype):  field data type string
	(tunit):  field data units string
	(tzero):  field data offset parameter
	(tscal):  field data scale parameter
	(tnull):  string which indicates null value for field
	
	Use this function to add keywords which describe the formatting of a
	single field to a character matrix containing a FITS table header.
	You should call it after you have called SetFITSTableInfo() to
	describe the table's overall dimensions and number of fields per row,
	and you should call it once for each field in the FITS table.
	
	The (header) parameter contains, on input, a pointer to a character
	matrix that has been built up by previous calls to the functions
	SetFITSTableHeaderInfo()/SetFITSTableHeaderFieldInfo().  On output,
	the matrix will be modified to contain the additional field formatting
	parameters.  This may require additional memory to be allocated; in
	that case, the pointer returned in (*header) will be different from
	the one passed on input, and the memory referenced by the original
	pointer will be freed.
	
	The (field) parameter is the number of the field you wish to add
	formatting information for.  This parameter can take any value
	from 1 (for the first field) through the value which was passed
	for (tfields) to the function SetFITSTableHeaderInfo().
	
	A more thorough discussion of the meanings of the remaining arguments
	to this function is given for the function SetFITSTableHeaderFieldInfo().
	Note that if you pass an empty string for the parameters (tform), (ttype),
	(tunit), and (tnull), the corresponding keywords will NOT be added
	to the FITS header.  All of these keywords, except TFORM, are optional;
	a valid value for the TFORM string must be present or the table will
	be unreadable.

	References:
	
	Wells, D. C., et al.  "FITS: A Flexible Image Transport System".
	Astronomy & Astrophysics Supplement Series, vol. 44, June 1981,
	pp. 363-370.
	
	Grosbol, P., et al.  "Generalized Extensions and Blocking Factors
	for FITS".  Astronomy & Astrophysics Supplement Series, vol. 73,
	June 1988, pp. 359-964.
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
	
******************************************************************************/
	
void SetFITSTableHeaderFieldInfo ( char ***, long, long, char *, char *, char *,
     double, double, char * );

/************************  NewFITSTableDataMatrix  **************************

	Allocates memory for a matrix of FITS table data.
	
	char **NewFITSTableDataMatrix ( long naxis1, long naxis2 )
	
	(naxis1): number of columns in table data matrix
	(naxis2): number of rows in table data matrix
	
	The function returns a pointer to the new table data matrix if successful,
	or NULL on failure.  If successful, the new matrix will be initialized
	with whitespace (ASCII value 32).  Use the function FreeFITSTableDataMatrix()
	to free memory allocated by this function.
	
	FITS table data matrices are two-dimensional, consisting of one or more
	rows of data.
	
	If successful, this function returns an array of pointers to the
	individual rows of the table data matrix.  Memory for each row of
	table data is allocated separately, but data within a particular
	row is contiguous in memory.
	
	To reference the ith row of the matrix, use matrix[i].  This will
	return a pointer to the start of the row; to access the jth character
	within that row, use the notation matrix[i][j].  Here the integers
	i and j are counted from zero as per the C language standard.
	
******************************************************************************/

char **NewFITSTableDataMatrix ( long, long );

/************************  FreeFITSTableDataMatrix  **************************

	Frees memory for a matrix of FITS table data.
	
	void FreeFITSTableDataMatrix ( char **matrix )
	
	(matrix): pointer to the matrix.
	
	Use this function to free memory that was allocated by the function
	NewFITSTableDataMatrix().
	
******************************************************************************/

void FreeFITSTableDataMatrix ( char ** );

/**********************  ReadFITSTableDataMatrix  *****************************

	Reads table data from a FITS file into a character table data matrix.
	
	int ReadFITSTableDataMatrix ( FILE *file, char **matrix, long naxis1,
	long naxis2 )

	(file):   pointer to the FITS file, opened for reading in binary mode.
	(matrix): pointer to character matrix to receive data.
	(naxis1): number of columns in FITS file table data matrix.
	(naxis2): number of rows in FITS file table data matrix.
	
	This function returns TRUE if successful, and FALSE on failure.
	
	Use the function to read an entire matrix of table data from a FITS
	file.  The function assumes that the file has been opened for reading
	in binary mode, that the file's header has been read, and that the
	file read mark is cued up to the start of the file's table data matrix.

	The matrix into which image data will be read must have already been
	allocated by the function NewFITSTableDataMatrix().  The dimensions of
	the table data matrix (naxis1, naxis2) are assumed to have been read
	from the file's FITS table header, e.g. by the function
	GetFITSTableHeaderInfo().  Any data already residing in the matrix will
	be overwritten by this function.
	
	Note that the function does not read the padding at the end of
	the table data in the file.  You should call the function
	ReadFITSTableDataPadding() after reading the table data matrix in order
	to skip over the padding, and move the file read mark to the next block
	of header information or data stored in the FITS file.

*******************************************************************************/

int ReadFITSTableDataMatrix ( FILE *, long, long, char ** );

/**********************  WriteFITSTableDataMatrix  *****************************

	Writes table data from a character data matrix to a FITS table file.
	
	int WriteFITSTableDataMatrix ( FILE *file, char **matrix,
	    long naxis1, long naxis2 )

	(file):   pointer to the FITS file, opened for writing in binary mode.
	(matrix): pointer to character matrix containing table data.
	(naxis1): number of columns in table data matrix.
	(naxis2): number of rows in table data matrix.

	
	This function returns TRUE if successful, and FALSE on failure.
	
	Use the function to write an entire matrix of table data to a FITS
	file.  The function assumes that the file has been opened for writing
	in binary mode, that the file's header has been written, and that the
	file write mark is queued up to the start of the table data matrix.

	The dimensions of the table data matrix (naxis, naxis1, naxis2, naxis3)
	are assumed to be the same as those contained in the file's FITS header,
	e.g. as set by the function SetFITSHeaderInfo().  
	
	Note that this function DOES NOT zero-pad the table data matrix; you
	should call the function WriteFITSTableDataPadding() to assure that
	the length of the table data matrix in the file is an integer multiple
	of 2880 bytes.

*******************************************************************************/

int WriteFITSTableDataMatrix ( FILE *, long, long, char ** );

/************************  ReadFITSTableDataRow  ****************************

	Reads one row of data from a FITS table file.

	int ReadFITSTableDataRow ( FILE *file, long naxis1, char *data )

	(file):   pointer to the FITS table file, opened for reading in binary mode.
	(naxis1): number of characters in a table row.
	(data):   pointer to buffer to receive table data.
	
	This function returns TRUE if successful, or FALSE on failure.
	
	The output table data array (data) must be able to contain at least
	(naxis1) characters, which respesent the one row of the table.
	These are read from the input file (file), which must have previously
	been opened for reading in binary mode.

	References:
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
		
******************************************************************************/

int ReadFITSTableDataRow ( FILE *, long, char * );

/************************  WriteFITSTableDataRow  ****************************

	Writes one row of data to a FITS table file.

	int WriteFITSTableDataRow ( FILE *file, long naxis1, char *data )
	
	(file):   pointer to the FITS image file, opened for writing in binary mode.
	(naxis1): number of pixels in an image row.
	(data):   pointer to buffer containing image data values.
	
	This function returns TRUE if successful, or FALSE on failure.
	
	The input table data array (data) must contain at least (naxis1)
	characters, which respesent the one row of the table.  These are written
	to the output file (file), which must have previously been opened for writing
	in binary mode.

	References:
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
	
******************************************************************************/

int WriteFITSTableDataRow ( FILE *, long, char * );

/***********************  ReadFITSTableDataPadding  **************************

	Reads the padding at the end of a FITS table file.
	
	int ReadFITSTableDataPadding ( FILE *file, long naxis1, long naxis2 )
	
	(file):   pointer to the file, opened for reading in binary mode
	(naxis1): number of columns in table data matrix
	(naxis2): number of rows in table data matrix
	
	The function returns TRUE if successful, and FALSE otherwise.
	
	Because the FITS standard specifies that files must consist of an integral
	number of 2880-byte blocks, the last table data block must be padded with
	whitespace if the size of the table header plus data matrix does not
	equal an integer multiple of 2880.
	
	You may call this function after reading the table data matrix to skip
	over the padding and move the file i/o mark to the start of the next
	header-data unit.  In practice, howevever, many FITS-writing programs
	may fail to pad FITS files, and you should not be concerned if failure
	occurs in this case.

	References:
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
	
******************************************************************************/

int ReadFITSTableDataPadding ( FILE *, long, long );

/***********************  WriteFITSTableDataPadding  **************************

	Writes the padding at the end of a FITS table file.
	
	int WriteFITSTableDataPadding ( FILE *file, long naxis1, long naxis2 )
	
	(file):   pointer to the file, opened for writing in binary mode
	(naxis1): number of columns in image data matrix
	(naxis2): number of rows in image data matrix
	
	The function returns TRUE if successful, and FALSE otherwise.
	
	Because the FITS standard specifies that files must consist of an integral
	number of 2880-byte blocks, the last table data block must be padded with
	whitespace if the size of the table header plus data matrix does not
	equal an integer multiple of 2880.
	
	You should call this function after writing the table data matrix to
	properly pad the table data to its correct length.  After a successful
	return, you may then be able to write additional header-data units
	to the file.

	References:
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
	
******************************************************************************/

int WriteFITSTableDataPadding ( FILE *, long, long );

/***********************  GetFITSTableDataFieldXXX  **************************

	Obtains character string, integer, or floating-point values from a field
	within a row of FITS table data.
	
	void GetFITSTableDataFieldString ( char *data, long tbcol, char *tform,
	     char *string )
	     
	void GetFITSTableDataFieldInteger ( char *data, long tbcol, char *tform,
	     double tzero, double tscal, long *integer )
	     
	void GetFITSTableDataFieldReal ( char *data, long tbcol, char *tform,
	     double tzero, double tscal, double *real )
	
	(data):    pointer to buffer containing row of FITS table data
	(tbcol):   column at which field starts
	(tform):   field width and format information string
	(tzero):   table data value offset parameter
	(tscal):   table data value scale parameter
	(string):  buffer to recieve character string value
	(integer): pointer to variable to receieve integer value
	(real):    pointer to variable to recieve floating-point value
	
	Use these functions to extract values from fields in a FITS table
	data row.
	
	The input argument (data) is a pointer to the start of a buffer
	containing a row of FITS table data.  The parameter (tbcol) is
	the column at which the field starts, counting from 1; the string
	(tform) gives the field's type, width, and precision.  The values
	(tzero) and (tscal) are an optional offset and scaling applied to
	numberical data values.  All of these parameters should have been
	obtained from the FITS table file's header; see the documentation
	for GetFITSTableHeaderFieldInfo() for more information.
	
	These function differ primarily in the type of information that
	they return.  The first function returns the value in the FITS table
	field as an ASCII NUL-terminated string.  The length of this string,
	plus the terminating NUL character, cannot exceed 100 characters,
	as per the FITS table standard.  The second and third functions
	return the value in the FITS table field as integer and floating-
	point values, respectively.

	References:
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.
	
********************************************************************************/

void GetFITSTableDataFieldString ( char *, long, char *, char * );
void GetFITSTableDataFieldInteger ( char *, long, char *, double, double, long * );
void GetFITSTableDataFieldReal ( char *, long, char *, double, double, double * );

/***********************  SetFITSTableDataFieldXXX  **************************

	Stores character string, integer, or floating-point values in a field
	within a row of FITS table data.
	
	void SetFITSTableDataFieldString ( char *data, long tbcol, char *tform,
	     char *string )
	     
	void SetFITSTableDataFieldInteger ( char *data, long tbcol, char *tform,
	     double tzero, double tscal, long integer )
	     
	void SetFITSTableDataFieldReal ( char *data, long tbcol, char *tform,
	     double tzero, double tscal, double real )
	
	(data):    pointer to buffer containing row of FITS table data
	(tbcol):   column at which field starts
	(tform):   field width and format information string
	(tzero):   data value offset parameter
	(tscal):   data value scale parameter
	(string):  string value to store in field
	(integer): integer value to store in field
	(real):    floating-point value to store in field
	
	Use these functions to place values into fields in a FITS table
	data row.
	
	The input argument (data) is a pointer to the start of a buffer
	containing a row that will receive the FITS table data.  The
	parameter (tbcol) is the column at which the field starts, counting
	from 1; the string (tform) gives the field's type, width, and precision.
	The values (tzero) and (tscal) are an optional offset and scaling
	applied to numberical data values.  See the documentation
	for GetFITSTableHeaderFieldInfo() for more information.
	
	These function differ primarily in the type of information that
	they store.
	
	SetFITSTableDataFieldString() stores an ASCII NUL-terminated
	string in the FITS table field.  The length of this string cannot
	exceed 99 characters, as per the FITS table standard.  If the length
	of the string is less than the field width indicated in the (tform)
	parameter, only characters up to the string's terminating NUL character
	will be copied into the field.
	
	SetFITSTableDataFieldInteger() stores an integer value in the FITS
	table field.  The value will be right-justified within the field and
	padded with whitespace.  For example, an integer value of 1234 stored
	within a field with a (tform) parameter of "I6" (integer data,
	6 characters wide) would be stored as "  1234".  If the integer value
	exceeds the field width, it will be truncated.  A sign will be present
	only if the value is negative.
	
	SetFITSTableDataFieldReal() stores a floating-point value in the FITS
	table field.  The value will be right-justified within the field and
	padded with whitespace; decimal digits will be given to the precision
	listed in the (tform) string.  For example, an value of 12.3456 stored
	within a field with a (tform) parameter of "F6.2" (floating-point data,
	6 characters wide, with 2 decimal digits) would be stored as " 12.35".
	If the length of the value string exceeds the field width, it will be
	truncated.  A sign will be present only if the valye is negative; values
	will be rounded to the precision listed in the (tform) string.

	References:
	
	Harten, R. H., et al.  "The FITS Tables Extension".  Astronomy &
	Astrophysics Supplement Series, vol. 73, June 1988, pp. 365-372.

********************************************************************************/

void SetFITSTableDataFieldString ( char *, long, char *, char * );
void SetFITSTableDataFieldInteger ( char *, long, char *, double, double, long );
void SetFITSTableDataFieldReal ( char *, long, char *, double, double, double );

/****************************  NewFITSTable  *********************************

	Allocates and initializes a new FITSTable structure for storing a FITS
	table header and/or table data.
	
	FITSTable *NewFITSTableHeader ( long naxis1, long naxis2, long tfields )
	FITSTable *NewFITSTable ( long naxis1, long naxis2, long tfields )
	
	(naxis1):  number of characters (i.e. columns) per row in table data matrix.
	(naxis2):  number of rows in table data matrix.
	(tfields): number of fields per row in table data matrix.

	If successful, these functions return a pointer to a new FITSTable
	structure; on failure, they will return NULL.  Use the function
	FreeFITSTable() to release memory allocated by these functions.
	
	The parameters (naxis1) and (naxis2) provide the overall dimensions
	of the table data matrix.  The parameter (tfields) provides the
	number of fields within each row of data.  For each field, you MUST
	call the function SetFITSTableFieldInfo() to define the field's
	starting column, format and width, etc.  The individual field parameters
	are only given default values here; if you do not specifically define
	them (using SetFITSTableDataFieldInfo()), the FITS table header created
	here will be incomplete and any FITS table file written to disk will
	be invalid.
	
	The function NewFITSTableHeader() allocates and initializes a FITSTable
	structure according to the parameters passed.  A character matrix
	containing an initialized FITS table header will also be allocated,
	and a pointer to it will be stored in the "header" member of the
	structure.  Memory for storing table data will NOT be allocated,
	and the "data" member of the structure will be set to NULL.  You may
	allocate memory for a table data matrix if you wish later by calling
	the function NewFITSTableDataMatrix().
	
	The function NewFITSTable() is identical to NewFITSTableHeader(), except
	that in addition to the header, it also allocates memory for a table
	data matrix.  A pointer to this matrix will be returned in the structure's
	"data" member, and the elements of the matrix will be initialized with
	zeros.
	
******************************************************************************/

FITSTable *NewFITSTableHeader ( long, long, long );
FITSTable *NewFITSTable ( long, long, long );

/****************************  FreeFITSTable  ********************************

	Frees memory for a FITSTable structure.
	
	void FreeFITSTable ( FITSTable *table )
	
	(table): pointer to FITS table record.
	
	Use this function to free memory for a FITSImage structure that was
	allocated by the functions NewFITSTable(), NewFITSTableHeader(),
	ReadFITSTable(), ReadFITSTableHeader(), etc.
	
	If the FITSTable structure being destroyed contains pointers to
	a FITS header and/or table data matrix, memory for those matrices
	will also be deallocated.  If you wish to prevent this, you should
	set the FITSTable's "header" and "data" structure members to point
	to NULL before calling this function.
	
******************************************************************************/

void FreeFITSTable ( FITSTable * );

/*************************  SetFITSTableFieldInfo  ****************************

	Sets field information for a table field within a FITSTable structure.
	
	void SetFITSTableFieldInfo ( FITSTable *table, long field, long tbcol,
	     char *tform, char *ttype, char *tunit, double tzero, double tscal,
	     char *tnull )
	
	(table): pointer to previously-allocated FITS table record.
	(field): number of field to add formatting information for
	(tbcol): column in which field starts
	(tform): field width and format string
	(ttype): field data type string
	(tunit): field data units string
	(tzero): field data offset parameter
	(tscal): field data scale parameter
	(tnull): string which indicates null value for field
	
	Use this function to define information for a field in a FITS table
	record.  The parameter (table) points to a FITSTable record that
	was created by the functions NewFITSTable() or NewFITSTableHeader().
	
	The (field) parameter indicates the number of the field you wish
	to define.  It is counted from 1 to the total number of fields
	per row (which should equal the value you passed in the (tfields)
	parameter to NewFITSTable() or NewFITSTableHeader().)
	
	The other parameters to this function describe the field's starting
	column, width, data type, etc.  They are described in more detail
	in the documentation for GetFITSTableHeaderFieldInfo().
	
	In addition to storing the information that describes the field in
	the FITSTable structure, this function also adds it to the FITS table
	header character matrix in the FITSTable's "header" field.  This
	may require additional memory to be allocated for the header matrix;
	if so, the "header" pointer will be changed after the function is
	called.
	
******************************************************************************/

void SetFITSTableFieldInfo ( FITSTable *, long, long, char *, char *,
char *, double, double, char * );

/****************************  ReadFITSTable  *********************************

	Allocates and initializes a new FITSTable structure, and reads FITS table
	header and/or data from a FITS table file into it.
	
	FITSTable *ReadFITSTableHeader ( FILE *file )
	FITSTable *ReadFITSTable ( FILE *file )
	
	(file): pointer to FITS file, opened for reading in binary mode.
	
	If successful, these functions return a pointer to the new FITSTable
	structure; on failure, they will return NULL.  Use the function
	FreeFITSTable() to release memory allocated by these functions.
	
	The (file) parameter is a standard C I/O library file pointer, e.g.
	as returned by fopen().  It must refer to a FITS table file which has
	been opened for reading in binary mode.
	
	The function ReadFITSTableHeader() allocates and initializes a FITSTable
	structure.  It allocates a character matrix to store the file's FITS
	table header, and reads the file's header into it.  It then scans the
	header to obtain the parameters which describe the dimensions (naxis1,
	naxis2), number of fields (tfields) and formatting of the table fields
	stored in the file's table data matrix.  It returns these parameters
	in the relevant FITSTable structure fields, but does not read the
	table data matrix from the file or allocate memory to store it.
	The structure's "data" pointer is set to NULL to indicate that no
	table data matrix storage has been allocated.  You can call the
	function ReadFITSTableData() if you wish to read the table data
	matrix at a later time.
	
	The function ReadFITSTable() is identical to ReadFITSTableHeader(), except
	that in addition to the header, it also allocates memory for and reads
	the table data matrix.  A pointer to this matrix will be returned in the
	structure's "data" member.
	
******************************************************************************/

FITSTable *ReadFITSTableHeader ( FILE * );
FITSTable *ReadFITSTable ( FILE * );

/***************************  ReadFITSTableData  ******************************

	Allocates a new FITS table data matrix, and reads image data from a FITS
	file into it.
	
	char **ReadFITSImageData ( FILE *file, FITSTable *table )
	
	(file):  pointer to FITS file, opened for reading in binary mode.
	(table): pointer to FITSTable structure describing format of table data.
	
	If successful, this function returns a pointer to a new FITS table
	data matrix; on failure, it will return NULL.  Use the function
	FreeFITSTableDataMatrix() to release memory allocated by this function.
	
	The (file) parameter is a standard C I/O library file pointer, e.g.
	as returned by fopen().  It must refer to a FITS table file which has
	been opened for reading in binary mode.  Additionally, the file read
	mark must be cued up to the start of the table data matrix (i.e., the
	function assumes that the file's header has already been read.)
	
	The (table) parameter is a pointer to an initialized FITSTable
	structure which describes the dimensions and formatting of the
	table data matrix in the FITS file.  This can be obtained with the
	function ReadFITSTableHeader().
	
******************************************************************************/

char **ReadFITSTableData ( FILE *, FITSTable * );

/****************************  WriteFITSTable  *******************************

	Writes image header and/or data information from a FITSTable record
	in memory to a FITS table file on disk.
	
	int WriteFITSTableHeader ( FILE *file, FITSTable *image )
	int WriteFITSTable ( FILE *file, FITSTable *image )
	
	(file):  pointer to FITS file, opened for writing in binary mode.
	(table): pointer to FITSTable structure containing table header/data.
	
	If successful, these functions return TRUE; on failure they return
	FALSE.
	
	The (file) parameter is a standard C I/O library file pointer, e.g.
	as returned by fopen().  It must refer to a FITS table file which has
	been opened for writing in binary mode.
	
	The function WriteFITSTableHeader() writes the header data stored in
	the table's "header" character matrix to the file.  It does not write
	the table data matrix, however.  Call the function WriteFITSTable()
	to write both the table header and data matrix to the file.
	
******************************************************************************/

int WriteFITSTableHeader ( FILE *, FITSTable * );
int WriteFITSTable ( FILE *, FITSTable * );

/***********************  GetFITSTableDataFieldXXX  **************************

	Obtains character string, integer, or floating-point values from a field
	within a row of FITS table data.
	
	void GetFITSTableFieldString ( FITSTable *table, char *data, long field,
	     char *string )
	     
	void GetFITSTableFieldInteger ( FITSTable *table, char *data, long field,
	     long *integer )
	     
	void GetFITSTableFieldReal ( FITSTable *table, char *data, long field,
	     double *real )

	(table):   pointer to FITSTable structure describing field
	(data):    pointer to buffer containing row of FITS table data
	(field):   number of field to obtain value from
	(string):  buffer to recieve character string value
	(integer): pointer to variable to receieve integer value
	(real):    pointer to variable to recieve floating-point value
	
	Use these functions to extract values from fields in a FITS table
	data row.
	
	The parameter (table) is a pointer to an initialized FITSTable structure
	which describes the formatting of the table's fields.  The parameter
	(data) is a pointer to the start of a buffer which contains the desired
	row of FITS table data; if the table data matrix is stored in the FITSTable
	structure's "data" field, you may pass table->data[i] to obtain a pointer
	to the ith row of the table data (where i is counted from zero.)  The
	parameter (field) contains the number of the field for which you wish
	to obtain a value; this parameter may take any value from 1 through the
	total number of fields per row, which equals the value stored in the
	"tfields" member of the FITSTable structure passed in (table).
	
	These function differ primarily in the type of information that
	they return.  The first function returns the value in the FITS table
	field as an ASCII NUL-terminated string.  The length of this string,
	plus the terminating NUL character, cannot exceed 100 characters,
	as per the FITS table standard.  The second and third functions
	return the value in the FITS table field as integer and floating-
	point values, respectively.
	
********************************************************************************/

void GetFITSTableFieldString ( FITSTable *, char *, long, char * );
void GetFITSTableFieldInteger ( FITSTable *, char *, long, long * );
void GetFITSTableFieldReal ( FITSTable *, char *, long, double * );

/***********************  SetFITSTableDataFieldXXX  **************************

	Stores character string, integer, or floating-point values in a field
	within a row of FITS table data.
	
	void SetFITSTableFieldString ( FITSTable *table, char *data, long field,
	     char *string )
	     
	void SetFITSTableFieldInteger ( FITSTable *table, char *data, long field,
	     long integer )
	     
	void SetFITSTableFieldReal ( FITSTable *table, char *data, long field,
	     double real )

	(table):   pointer to initialized FITSTable structure describing table
	(data):    pointer to FITS table data row to receive value
	(field):   number of field to store data value in
	(string):  string value to store in field
	(integer): integer value to store in field
	(real):    floating-point value to store in field
	
	Use these functions to place values into fields in a FITS table
	data row.
	
	The input argument (data) is a pointer to the start of a buffer
	containing a row of FITS table data that will receive the value.
	The parameter (table) is a pointer to an initialized FITSTable record
	which describes the formatting of the individual table fields.  The
	parameter (field) is the number of the field in which you wish to store
	a value; it can take any number from 1 to the total number of fields
	per row (equal to the value of the "tfields" member of the FITSTable
	structure).
	
	These function differ primarily in the type of information that
	they store.  SetFITSTableFieldString() stores an ASCII NUL-terminated
	string in the FITS table field; SetFITSTableFieldInteger()
	stores an integer value in the field; and SetFITSTableFieldReal()
	stores a floating-point value in the FITS table field.  See
	SetFITSTableDataField() for more information.

********************************************************************************/

void SetFITSTableFieldString ( FITSTable *, char *, long, char * );
void SetFITSTableFieldInteger ( FITSTable *, char *, long, long );
void SetFITSTableFieldReal ( FITSTable *, char *, long, double );

/***************************  functions in GSC.C  *****************************/

/********************  GetGSCRegionIndexFilePath  *************************

	Finds the location of the Guide Star Catalog region index file
	on the GSC CD-ROM.
	
	void GetGSCRegionFilePath ( char *separator, char *volume,
	char *directory, char *filename )
	
	(separator): directory separator string.
	(volume): receives name of GSC CD-ROM volume containing file.
	(directory): receives directory on CD-ROM containing file.
	(filename): receives name of region index file.
	
	Use this function to determine the name of the GSC region index file,
	and find the CD-ROM volume and directory on which that file resides.
	The GSC region index file is a FITS table file which serves as a
	master index to the 9,357 individual region files on the two GSC
	CD-ROMs.
	
	The argument (separator) is an ASCII NUL-terminated string which
	contains the directory separator character used by the native operating
	system.  For example, on the Macintosh platform, the directory separator
	character is ":" (colon); in DOS or Windows, it is "\\" (backslash);
	in UNIX it is "/" (forward slash).
	
	The argument (volume) is a pointer to a buffer that will receive,
	on output, the name of the GSC CD-ROM volume where the GSC region
	index file resides.  This buffer should be able to contain at least
	255 characters plus the terminating NUL character.
	
	The argument (directory) is a pointer to a buffer that will receive,
	on output, the name of the directory on the CD-ROM in which the
	GSC region index file resides.  This buffer should be able to contain
	at least 255 characters plus the terminating NUL character.
	
	The argument (filename) is a pointer to the buffer that will receive,
	on output, the name of the GSC region index file.  Again, this buffer
	should be able to hold at least 255 characters plus the terminating NUL.
	
	The following sample code utilizes this function to open the GSC
	region index file.  Note that the line defining the directory
	separator character may have to be modified to on other platforms.
	
	#include <stdio.h>
	
	#define SEPARATOR ":"
	
	char volume[256];
	char directory[256];
	char filename[256];
	char oldcwd[256];
	FILE *file;
	
	// Save current working directory.
	
	getcwd ( oldcwd );
	
	// Determine the location of the GSC region index file.
	// Form a single string containing the complete path to the
	// file by appending the directory string to the volume name
	// string.  This works on Macintosh; for DOS/Windows or UNIX,
	// you need to append the directory string to a string which
	// contains the drive letter (e.g. "E:") or root path (e.g.
	// "/dev/cdr0") to the GSC CD-ROM instead.
	
	GetGSCRegionIndexFilePath ( SEPARATOR, volume, directory, filename );
	strcat ( volume, directory );
	
	// Move to the directory containing the file; attempt to open it
	// in binary mode if successful; set the file pointer to NULL
	// to signal failure otherwise.
	
	if ( chdir ( volume ) == 0 )
		file = fopen ( filename, "rb" );
	else
		file = NULL;
	
	// Return to the old working directory, and return the file
	// pointer.
	
	chdir ( oldcwd );
	return ( file );
		
****************************************************************************/

void GetGSCRegionIndexFilePath ( char *, char *, char *, char * );

/******************  ReadGSCRegionIndexFileHeader  ***********************

	Reads the FITS table header of the Guide Star Catalog region index
	file header.
	
	FITSHeader *ReadGSCRegionIndexFileHeader ( FILE *file )
	
	(file): pointer to index file, opened for reading in binary mode.
	
	Use this function to read the GSC region index file's FITS table
	header.  If successful, this function will return a pointer to
	an initialized FITSTable structure which describes the layout and
	contents of the file.  On failure, the function returns NULL.
	
	The input argument (file) is a standard C I/O library file pointer,
	as obtained via fopen().  Note that you can use the routine
	GetGSCRegionIndexFilePath() to determine the location of this
	file; see documentation for that routine for an example of code
	which will open that file.
	
	You can use the function FreeFITSTable() to release memory allocated
	for the FITSTable structure by this function when you are finished
	with it.
		
****************************************************************************/

FITSTable *ReadGSCRegionIndexFileHeader ( FILE * );

/******************  ReadGSCRegionIndexFileRecord  ***********************

	Reads a single record from the GSC region index file.
	
	int ReadGSCRegionIndexFileRecord ( FILE *file, FITSTable *table,
	    GSCRegion *region )
	
	(file): pointer to index file, opened for reading in binary mode.
	(table): pointer to FITS table record describing file.
	(region): pointer to GSCRegion record to recieve region information.
	
	Use this function to read a record from the GSC region index file.
	The function assumes that the file has been opened for reading in
	binary mode, and that the file's FITS table header has already been
	read.  The argument (file) contains a pointer to the file; (table)
	is an initialized FITSTable structure describing the file header.
	See ReadGSCRegionIndexFileHeader() for more information. 
	
	Note that for GSC regions in the southern half of the celestial
	hemisphere, this function reverses the values in the dec_hi and
	dec_lo fields of the GSCRegion record.  This is because in the
	region index file itself, the dec_hi member contains the more
	negative declination boundary, and the dec_lo member contains
	the less negative declination boundary, which is mathematically
	the reverse of the convention for regions in the northern
	celestial hemisphere.  Records read by this function will be
	consistent in both hemispheres.
	
	If successful, this function will return TRUE; on failure, it will
	return FALSE.
		
****************************************************************************/

int ReadGSCRegionIndexFileRecord ( FILE *, FITSTable *, GSCRegion * );

/***********************  ReadGSCRegionIndexFile  *************************

	Reads the GSC region index file into an array of GSCRegion records.
	
	GSCRegion **ReadGSCRegionIndexFileRecord ( FILE *file )
	
	(file): pointer to index file, opened for reading in binary mode.
	
	Use this function to read the GSC region index file.  The function

	assumes that the file has been opened for reading in binary mode;
	see GetGSCRegionFilePath() for an example of code which will open
	the file.
	
	If successful, this function will return an array of pointers to
	GSCRegion records which contain the numbers and coordinate boundaries
	of the Hubble Guide Star Catalog region files.  The last element
	of this array will be set to NULL to indicate the end of the array.
	You can use the function FreeGSCRegionIndex() to release memory
	allocated by this function when you wish to dispose of the GSC
	region index.  On failure, ReadGSCRegionIndexFile() returns NULL.
	
****************************************************************************/

GSCRegion **ReadGSCRegionIndexFile ( FILE * );

/***********************  FreeGSCRegionIndex  *************************

	Frees memory for the GSCRegion record array allocated by
	ReadGSCRegionIndexFile().
	
	void FreeGSCRegionIndex ( GSCRegion **index );
	
	This function returns nothing.
	
****************************************************************************/

void FreeGSCRegionIndex ( GSCRegion ** );

/**********************  GetGSCRegionFilePath  ****************************

	Finds the location of a Guide Star Catalog region file on the GSC CD-ROMs.
	
	void GetGSCRegionFilePath ( GSCRegion *region, char *separator,
	     char *volume, char *directory, char *filename )
	
	(region): pointer to GSCRegion record containing region information.
	(separator): directory separator string.
	(volume): receives name of GSC CD-ROM volume containing file.
	(directory): receives directory on CD-ROM containing file.
	(filename): receives name of region file.
	
	Use this function to determine the name of an individual GSC region
	file, and find the CD-ROM volume and directory on which that file resides.
	
	The input argument (region) is a pointer to an initialized GSCRegion
	record which describes the boundaries of the region in the sky.  The
	(separator) argument is an ASCII NUL-terminated string which contains
	the directory separator character used by the native operating system.
	For example, on the Macintosh platform, the directory separator
	character is ":" (colon); in DOS or Windows, it is "\\" (backslash);
	in UNIX it is "/" (forward slash).
	
	The argument (volume) is a pointer to a buffer that will receive,
	on output, the name of the GSC CD-ROM volume where the GSC region
	file resides.  This buffer should be able to contain at least 255
	characters plus the terminating NUL character.
	
	The argument (directory) is a pointer to a buffer that will receive,
	on output, the name of the directory on the CD-ROM in which the
	GSC region file resides.  This buffer should be able to contain
	at least 255 characters plus the terminating NUL character.
	
	The argument (filename) is a pointer to the buffer that will receive,
	on output, the name of the GSC region file.  Again, this buffer should
	be able to hold at least 255 characters plus the terminating NUL.
	
	The documentation for the routine GetGSCRegionIndexFilePath() contains
	sample code which can be used to open the GSC region index file
	given the volume, directory, and file name strings; an analogous
	routine can be written to open the individual GSC region files with
	the strings returned by this function.
	
****************************************************************************/

void GetGSCRegionFilePath ( GSCRegion *, char *, char *, char *, char * );

/**********************  ReadGSCRegionFileHeader  *************************

	Reads the FITS table header of a Guide Star Catalog region file.
	
	FITSHeader *ReadGSCRegionFileHeader ( FILE *file )
	
	(file): pointer to region file, opened for reading in binary mode.
	
	Use this function to read a GSC region file's FITS table header.
	If successful, this function will return a pointer to an initialized
	FITSTable structure which describes the layout and contents of the file.
	On failure, the function returns NULL.
	
	The input argument (file) is a standard C I/O library file pointer,
	as obtained via fopen().  Note that you can use the routine
	GetGSCRegionFilePath() to determine the location of a region file.
	
	You can use the function FreeFITSTable() to release memory allocated
	for the FITSTable structure by this function when you are finished
	with it.
		
****************************************************************************/

FITSTable *ReadGSCRegionFileHeader ( FILE * );

/**********************  ReadGSCRegionFileRecord  *************************

	Reads a single record from a GSC region file.
	
	int ReadGSCRegionFileRecord ( FILE *file, FITSTable *table,
	    GSCRecord *record )
	
	(file): pointer to index file, opened for reading in binary mode.
	(table): pointer to FITS table record describing file.
	(record): pointer to GSCRecord to recieve record information.
	
	Use this function to read a record from a GSC region file.

	The function assumes that the file has been opened for reading in
	binary mode, and that the file's FITS table header has already been
	read.  The argument (file) contains a pointer to the file; (table)
	is an initialized FITSTable structure describing the file header.
	See ReadGSCRegionFileHeader() for more information. 

	Note: Some GSC region file entries are completely blank.  This function
    will return zero in the gsc_id field of (record) if it reads a blank
    record.

    Note 2: Some objects have multiple records in the GSC.  These are flagged
    with a 'T' in the (record) field, and are stored consecutively within the
    same region file.  To average all of the consecutive records for a single
    object, use the function ReadGSCRegionFileObject().
    
	If successful, this function will return TRUE; on failure, it will
	return FALSE.
		
****************************************************************************/

int ReadGSCRegionFileRecord ( FILE *, FITSTable *, GSCRecord * );

/**********************  ReadGSCRegionFileRecord  *************************

	Reads a single record from a GSC region file.
	
	int ReadGSCRegionFileRecord ( FILE *file, FITSTable *table,
	    GSCRecord *record )
	
	(file): pointer to index file, opened for reading in binary mode.
	(table): pointer to FITS table record describing file.
	(record): pointer to GSCRecord to recieve record information.
	
	If successful, this function will return TRUE; on failure, it will
	return FALSE.

	The function assumes that the file has been opened for reading in
	binary mode, and that the file's FITS table header has already been
	read.  The argument (file) contains a pointer to the file; (table)
	is an initialized FITSTable structure describing the file header.
	See ReadGSCRegionFileHeader() for more information. 

	Note: Some GSC region file entries are completely blank.  This function
    will return 0 in (record->gsc_id) if it reads a blank record.

    Note 2: Some objects have multiple records in the GSC.  These are flagged
    with a 'T' in (record->multiple), and are stored consecutively within the
    same region file.  To average all of the consecutive records for a single
    object, use the function ReadGSCRegionFileObject().
    
****************************************************************************/

int ReadGSCRegionFileRecord ( FILE *, FITSTable *, GSCRecord * );
int ReadGSCRegionFileObject ( FILE *, FITSTable *, GSCRecord * );

/***************************  FindGSCRegion  *******************************

	Finds GSC regions which lie within a specified set of coordinate
	boundaries.
	
	int FindGSCRegion ( GSCRegion **index, double ra_lo, double dec_lo,
	    double ra_hi, double dec_hi, GSCRegion **region )
	
	(index):  pointer to GSC region index array
	(ra_lo):  low right ascention boundary of area to search.
	(ra_hi):  high right ascention boundary of area to search.
	(dec_lo): low declination boundary of area to search.
	(dec_hi): high declination boundary of area to search.
	(region): pointer to GSCRegion record found.
	
	The function return TRUE if able to find a GSC region record
	within the specified coordinate boundaries, and FALSE otherwise.
	
	The input parameter (index) is an array of pointers to GSCRegion
	records, as returned by ReadGSCRegionIndexFile().
	
	The parameters (ra_lo,dec_lo) and (ra_hi,dec_hi) describe the low
	and high right ascension and declination boundaries of the area you
	wish to search; both values must be expressed in degrees.  Values for
	ra_lo and ra_hi must be between 0.0 to 360.0; values for dec_lo and
	dec_hi must be between -90.0 and +90.0.  Values for the "lo" coordinate
	boundaries must be lower than values for the "hi" coordinate boundaries,
	with one exception: if you wish to search a coordinate area which
	overlaps the zero-hour meridian of right ascension, pass the higher
	(i.e. eastern) R.A. boundary in the ra_lo parameter, and the lower
	(i.e. western) R.A. boundary in the ra_hi parameter.
	
	The parameter (region) contains, on input, a pointer to the previous
	GSCRegion record which was found by this function.  When you are
	calling this function to begin a new search, you should set the value
	of (*region) to NULL.  On a successful return, the value of this
	pointer will be changed to point to next the GSCRegion record which
	lies within the coordinate area specified.  If no more GSCRegion
	records lie within this area, the (*region) pointer will be set to
	NULL, and the function will return TRUE.
	
	The function will return TRUE if it can find any part of a subsequent
	region record which lies within the specified coordinate boundaries;
	it is not necessary for the entire region to lie within them.  See
	TestGSCRegion() for more information.
	
	The following sample code would find all of the GSC regions which
	lie from 23 hours to 1 hour of right ascension, and -10 to +10
	degrees of declination.  It is assumed that (file) is a pointer
	to the opened GSC region index file; for more information, see
	GetGSCRegionIndexFilePath(): 
	
	double      ra0 = 23.0 * 15.0
	double      ra1 = 1.0 * 15.0;
	double      dec0 = -10.0
	double      dec1 = +10.0;
	GSCRegion   *region = NULL;
	GSCRegion   **index = ReadGSCRegionIndexFile ( file );
	
	while ( FindGSCRegion ( index, ra0, dec0, ra1, dec1, &region ) == TRUE )
	{
		// do something
	}
	
	FreeGSCRegionIndex();
	
****************************************************************************/

int FindGSCRegion ( GSCRegion **, double, double, double, double, GSCRegion ** );

/***************************  TestGSCRegion  *******************************

	Determines whether or not a GSC region lies within a specified set of
	coordinate boundaries.
	
	int TestGSCRegion ( GSCRegion *region, double ra_lo, double dec_lo,
	    double ra_hi, double dec_hi, GSCRegion **region )
	
	(region): pointer to GSC region record to test.
	(ra_lo):  low right ascention boundary of area to test against.
	(ra_hi):  high right ascention boundary of area to test against.
	(dec_lo): low declination boundary of area to test against.
	(dec_hi): high declination boundary of area to test against.
	
	The function returns TRUE if any part of the specified region
	lies within the specified area, and FALSE otherwise.
	
	The parameters (ra_lo,dec_lo) and (ra_hi,dec_hi) describe the low
	and high right ascension and declination boundaries of the area you
	wish to search; both values must be expressed in degrees.  Values for
	ra_lo and ra_hi must be between 0.0 to 360.0; values for dec_lo and
	dec_hi must be between -90.0 and +90.0.  Values for the "lo" coordinate
	boundaries must be lower than values for the "hi" coordinate boundaries,
	with one exception: if you wish to search a coordinate area which
	overlaps the zero-hour meridian of right ascension, pass the higher
	(i.e. eastern) R.A. boundary in the ra_lo parameter, and the lower
	(i.e. western) R.A. boundary in the ra_hi parameter.

	If the area you are testing against exactly matches the region's lower
	right ascension or declination boundary, the region will be considered
	to overlap the area, and the function will return TRUE.  This will
	not happen, however, if the area exactly matches the region's upper
	R.A. or Dec. boundaries.  This is done so that all points on the
	celestial sphere will fall into a specific region, unambiguously.
	
*****************************************************************************/
	
int TestGSCRegion ( GSCRegion *, double, double, double, double );
	
/*************************  functions in Astrom.c  ***************************/

/*****************************  RADecToXiEta  *********************************

	Converts Right Ascension and Declination to standard coordinates.
	
	void RADecToXiEta ( double a, double d, double a0, double d0,
	double *xi, double *eta )

	   (a,d): R.A. and Dec. coordinates, in radians.
	 (a0,d0): R.A. and Dec. of adopted reference point, in radians.
	(xi,eta): receives standard coordinates, in radians.
	
	Reference: Marsden, B.G., "How to Reduce Plate Measurements",
	           Sky & Telescope, Sept 1982, p. 284.
	
*******************************************************************************/

void RADecToXiEta ( double, double, double, double, double *, double * );

/****************************  XiEtaToRADec  ***********************************

	Converts dimensionless "standard coordinates" to R.A./Dec.
	
	void XiEtaToRADec ( double xi, double eta, double ra0, double dec0,
	double *ra, double *dec )

	(xi,eta): standard coordinates
	(ra0,dec0): adopted R.A./Dec. of image center
	(ra,dec): receives R.A./Dec. corresponding to (xi,eta)

	Reference: Marsden, B.G., "How to Reduce Plate Measurements",
	           Sky & Telescope, Sept 1982, p. 284.
	
**********************************************************************************/

void XiEtaToRADec ( double, double, double, double, double *, double * );

/*******************************  XiEtaToXY  *************************************

	Converts dimensionless sky coordinates (xi,eta) to physical image coordinates.
	
	void XiEtaToXY ( double xi, double eta, double **coeffs, double *x, double *y )

	(xi,eta): sky coordinates, dimensionless units.
	(coeffs): matrix containing (xi,eta) -> (x,y) transformation coefficients.
	(x,y): receives physical image coordinates corresponding to (xi,eta).
	
***********************************************************************************/

void XiEtaToXY ( double, double, double **, double *, double * );

/*******************************  XYToXiEta  *************************************

	Converts physical image coordinates to dimensionless sky coordinates (xi,eta). 
	
	void XYToXiEta ( double x, double y, double **coeffs, double *xi, double *eta )

	(x,y): physical image coordinates.
	(coeffs): matrix containing (x,y) -> (xi,eta) transformation coefficients.
	(xi,eta): receives sky coordinates in dimensionless units.

**********************************************************************************/

void XYToXiEta ( double, double, double **, double *, double * );

/*****************************  XYToRADec  ***************************************

	Converts physical image coordinates to R.A./Dec. coordinates.
	
	void XYToRADec ( double x, double y, double **coefs, double ra0, double dec0,
	double *ra, double *dec )

	(x,y): physical image coordinates
	(coeffs): matrix containing (x,y) -> (xi,eta) transformation coefficients.
	(ra0,dec0): adopted coordinates of image center.
	(ra,dec): receives R.A./Dec. coordinates corresponding to (x,y).

***********************************************************************************/

void XYToRADec ( double, double, double **, double, double, double *, double * );

/*****************************  RADecToXY  ****************************************

	Converts R.A./Dec. coordinates to physical image coordinates.
	
	void RADecToXY ( double ra, double dec, double ra0, double dec0,
	double **coeffs, double *x, double *y )

	(ra,dec): receives R.A./Dec. coordinates corresponding to (x,y).
	(ra0,dec0): adopted coordinates of image center.
	(coeffs): matrix containing (xi,eta) -> (x,y) transformation coefficients.
	(x,y): receives physical image coordinates.

***********************************************************************************/

void RADecToXY ( double, double, double, double, double **, double *, double * );

/*************************  NewAstrometricSolution  ******************************

	Allocates and initializes matrices for a new astrometric coefficient solution.

	int NewAstrometricSolution ( double ***a, double ***b, double ***c, double ***d )

	(a): recieves pointer to (xi,eta) -> (x,y) transformation covariance matrix.
	(b): receives pointer to (xi,eta) -> (x,y) transformation coefficient matrix.
	(c): recieves pointer to (x,y) -> (xi,eta) transformation covariance matrix.
	(d): receives pointer to (x,y) -> (xi,eta) transformation coefficient matrix.
	
	The function returns TRUE if successful and FALSE on failure.
	
	If don't want to allocate all of the matrices, pass NULL instead of a
	matrix pointer for any which you do not wish to allocate.
	 
**********************************************************************************/

int NewAstrometricSolution ( double ***, double ***, double ***, double *** );

/************************  DeleteAstrometricSolution  *****************************

	Frees memory for astrometric coefficient solution matrices.
	
	int DeleteAstrometricSolution ( double **a, double **b, double **c, double **d )
	
	(a): (xi,eta) -> (x,y) transformation covariance matrix.
	(b): (xi,eta) -> (x,y) transformation coefficient matrix.
	(c): (x,y) -> (xi,eta) transformation covariance matrix.
	(d): (x,y) -> (xi,eta) transformation coefficient matrix.

	If you don't want to delete all of the matrices, pass NULL instead of a
	matrix pointer for any matrix which you don't wish to de-allocate.
	
***********************************************************************************/

void DeleteAstrometricSolution ( double **, double **, double **, double ** );

/************************  CopyAstrometricSolution  *****************************

	Copies astrometric coefficients and/or covariances from one matrix to another.
	
	void CopyAstrometricSolution ( double **a, double **b, double **c, double **d )
	
	(a): destination covariance matrix.
	(b): destination coefficient matrix.
	(c): source covariance matrix.
	(d): source coefficient matrix.

	All the matrices (a, b, c, and d) must have been created and initialized by
	the function NewAstrometricSolution().
	
	If you don't want to copy all of the matrices, pass NULL for any matrix which
	you don't wish to copy.  Note that to copy both sets of coefficient and
	covariance matrices allocated by NewAstrometricSolution, you must call this
	function twice.
	
***********************************************************************************/

void CopyAstrometricSolution ( double **, double **, double **, double ** );

/*************************  AugmentAstrometricSolution  ***************************

	Augments the normal equation matrices for an astrometric coefficient solution
	by adding position information for a reference star.
	
	int AugmentAstrometricSolution ( double **a, double **b, double **c, double **d,
	short order, double ra, double dec, double ra0, double dec0, double x, double y )
	
	    (a): (xi,eta) -> (x,y) transformation covariance matrix.
	    (b): (xi,eta) -> (x,y) transformation coefficient matrix.
	    (c): (x,y) -> (xi,eta) transformation covariance matrix.
	    (d): (x,y) -> (xi,eta) transformation coefficient matrix.
	(order): desired astrometric solution order (1-3).
	  (ra0): right ascention of reference point, in radians.
	 (dec0): declination of reference point, in radians.
	   (ra): known right ascention of reference object, in radians.
	  (dec): known declination of reference object, in radians.
	    (x): measured X coordinate of reference object on image.
	    (y): measured Y coordinate of reference object on image.

	The matrices (a, b, c, and d) must have been created and initialized
	by the function NewAstrometricSolution() prior to calling this function.
	
	Once you have created and initialized the astrometric solution matrices,
	you should call this function once for each known reference object whose
	position information you wish to add to the solution.  When you are done
	adding reference objects, call FitAstrometricSolution() to solve for the
	(x,y) -> (xi,eta) and (xi,eta) -> (x,y) transformation coefficients and
	associated covariance matrices.
	
*********************************************************************************/

int InitializeAstrometricSolution ( double **, double **, double **, double ** );

void AugmentAstrometricSolution ( double **, double **, double **, double **,
short, double, double, double, double, double, double );

/**************************  FitAstrometricSolution  *****************************

	Computes astrometric solution coefficients by least-squares best fit.
	
	int FitAstrometricSolution ( double **a, double **b, double **c, double **d,
	short order )
	
	    (a): (xi,eta) -> (x,y) transformation covariance matrix.
	    (b): (xi,eta) -> (x,y) transformation coefficient matrix.
	    (c): (x,y) -> (xi,eta) transformation covariance matrix.
	    (d): (x,y) -> (xi,eta) transformation coefficient matrix.
	(order): desired astrometric solution order (1-3).

	The function returns TRUE if it is able to successfully determine a
	least-squares best-fit, and FALSE otherwise.
	
	The input normal equation matrices (a, b, c, d) must have been created and
	initialized by NewAstrometricSolution() prior to calling this function, and
	information for all desired all reference stars must have been added by calls
	to the function AugmentAstrometricSolution().
	
**********************************************************************************/

int FitAstrometricSolution ( double **, double **, double **, double **, short );

/************************  SetAstrometricSolution **********************************

	Computes "ideal" coefficients for the (x,y) -> (xi,eta) and (xi,eta) -> (x,y)
	transformations, given an image reference pixel, field rotation, and scale.
	
	void SetAstrometricSolution ( double x, double y, double a, double s,
	double **b, double **d )
	
	(x): X coordinate of image reference pixel.
	(y): Y coordinate of image reference pixel.
	(a): field rotation angle, in radians.
	(s): image scale, in radians/pixel.
	(b): matrix to receive (xi,eta) -> (x,y) transformation coefficients.
	(d): matrix to receive (x,y) -> (xi,eta) transformation coefficients.

	The matrices (b) and (d) must have been previously created and initialized
	by the function NewAstrometricSolution().
	
	If you do not wish to compute one or the other transformation coefficient
	matrices, pass NULL instead of a pointer to it.
	
**************************************************************************************/

void SetAstrometricSolution ( double, double, double, double, double **, double ** );

/**************************  GetAstrometricSolution  *******************************

	Computes field reference pixel, rotation, and scale from astrometric coefficients.
	
	void GetAstrometricSolution ( double **b, double **d, double *x, double *y,
	double *a, double *s )

	(b): matrix containing (xi,eta) -> (x,y) transformation coefficients.
	(d): matrix containing (x,y) -> (xi,eta) transformation coefficients.
	(x): receieves X coordinate of image reference pixel.
	(y): receieves Y coordinate of image reference pixel.
	(a): receives field rotation angle, in radians.
	(s): recieves image scale, in radians/pixel.

	If you do not wish to compute field parameters from one or the other
	transformation coefficient matrices, simply pass NULL instead of a pointer to it.
	
**************************************************************************************/

void GetAstrometricSolution ( double **, double **, double *, double *, double *, double * );

/******************************************************************************/

struct SBIGInfo
{
	short	file_version;
	short	data_version;
	float	exposure;
	float	focal_length;
	float	aperture;
	float	response_factor;
	long	background;
	long	range;
	short	height;
	short	width;
	short	date_year;
	short	date_month;
	short	date_day;
	short	time_hour;
	short	time_min;
	short	time_sec;
	long	exposure_state;
	float	temperature;
	short	number_exposures;
	float	each_exposure;
	float	x_pixel_size;
	float	y_pixel_size;
	long	pedestal;
	float	e_gain;
};

int			ReadSBIGImageHeader ( FILE *, SBIGInfo * );
int			ReadSBIGImageDataRow ( FILE *, short, PIXEL * );

#endif
