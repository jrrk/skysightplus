/*** COPYRIGHT NOTICE AND PUBLIC SOURCE LICENSE ***************************************

	Portions Copyright (c) 1992-2001 Southern Stars Systems.  All Rights Reserved.

	This file contains Original Code and/or Modifications of Original Code as
	defined in and that are subject to the Southern Stars Systems Public Source
	License Version 1.0 (the 'License').  You may not use this file except in
	compliance with the License.  Please obtain a copy of the License at

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

****************************************************************************************/

#include "SkySight.h"

/*** CreateImageObject ********************************************************/

ImageObjectPtr CreateImageObject ( ImagePtr image, double x, double y, double sigma )
{
	char			name[256];
	short			left, top, right, bottom;
	ImageRegionPtr	region;
	
	left = floor ( x - 3.0 * sigma + 0.5 );
	right = ceil ( x + 3.0 * sigma - 0.5 );
	top = floor ( y - 3.0 * sigma + 0.5 );
	bottom = ceil ( y + 3.0 * sigma - 0.5 );
	
	if ( left < 0 )
		left = 0;
		
	if ( top < 0 )
		top = 0;
		
	if ( bottom >= GetImageRows ( image ) )
		bottom = GetImageRows ( image ) - 1;
	
	if ( right >= GetImageColumns ( image ) )
		right = GetImageColumns ( image ) - 1;
		
	region = NewImageRegion ( IMAGE_REGION_TYPE_RECTANGULAR, left, top, right, bottom );
	
	if ( region != NULL )
	{
		sprintf ( name, "Object %ld", GetImageObjectCount ( image ) + 1 );
		SetImageRegionName ( region, name );

		SetImageObjectCentroidX ( region, x );
		SetImageObjectCentroidY ( region, y );
		SetImageObjectRadius ( region, sigma );
	}
	
	return ( region );
}

/**** DrawImageObject ********************************************************

	Draws an object in an image window.
	
	void DrawImageObject ( ImageObjectPtr object, GWindowPtr window )
	
	(object): pointer to image object to draw.
	(window): window in which object should be drawn.
	
	This function assumes that you have already called one of the
	GStartDrawing() functions to begin drawing in the window, and that
	the pen has already been set to the desired color.
	
******************************************************************************/

void DrawImageObject ( ImageObjectPtr object, GWindowPtr window )
{
	int				drawCentroids, drawRadii;
	short			size, col, row, left, top, right, bottom;
	double			x, y, r;
	GRect			rect;

	/*** Find the object's bounding coordinates in the window.  If the
	     object is a position or magnitude standard, draw it with a thick box;
	     otherwise, draw a thin box. ***/
	
	GetImageObjectWindowRect ( object, window, &rect );
	size = 1;
	
/*
	if ( GetApertureFlag ( object, APERTURE_FLAG_POS_STD ) )
		size = 2;
		
	if ( GetApertureFlag ( object, APERTURE_FLAG_MAG_STD ) )
		size = 2;
*/
	
	GSetPen ( GWhiteColor(), 1, FALSE );
//	GDrawRect ( &rect );
	GSetPenSize ( 1 );
	
	/*** If we are drawing centroids or radii, compute the position
	     of the centroid, and the coordinates of the rectangle enclosing
	     the radius, then draw them. ***/
	
	drawCentroids = TRUE; // GetImageDisplayApertureCentroidsFlag ( window );
	drawRadii = TRUE; // GetImageDisplayApertureRadiiFlag ( window );
	
	if ( drawCentroids || drawRadii )
	{
		x = GetImageObjectCentroidX ( object );
		y = GetImageObjectCentroidY ( object );
		r = GetImageObjectRadius ( object ) * 3.0;
		
		ImageXYToWindow ( window, x, y, &col, &row );
		ImageXYToWindow ( window, x - r, y - r, &left, &top );
		ImageXYToWindow ( window, x + r, y + r, &right, &bottom );
		
		/*** Draw the crosshairs to the object's radius. ***/
		     
		if ( drawCentroids )
		{
			GDrawLine ( left, row, right - 1, row );
			GDrawLine ( col, top, col, bottom - 1 );
		}
			
		if ( drawRadii )
		{
			GDrawEllipse ( left, top, right + 1, bottom + 1 );
		}
		
		if ( GetImageWindowDisplayObjectNames ( window ) )
		{
			r = r / 1.414;
			ImageXYToWindow ( window, x + r, y + r, &col, &row );
			GDrawString ( GetImageRegionName ( object ), 0, col, row );
		}
	}
}

/*** GetImageObjectWindowRect ***************************************************

	Computes the rectangle occupied by an object in an image window.

	void GetImageObjectWindowRect ( ImageObjectPtr object, GWindowPtr window,
	     GRectPtr rect )
	
	(object): pointer to image object record.
	(window): pointer to image window in which object is displayed.
	(rect):   recieves rectangle containing object.
	
	The function returns the coordinates of the smallest rectangle which
	contains the object, in the window's coordinate system.
	
*********************************************************************************/

void GetImageObjectWindowRect ( ImageObjectPtr object, GWindowPtr window, GRectPtr rect )
{
	short		left, top, right, bottom;
	ImagePtr	image = GetImageWindowImage ( window );
	
	left = GetImageRegionLeft ( object );
	top = GetImageRegionTop ( object );
	right = GetImageRegionRight ( object ) + 1;
	bottom = GetImageRegionBottom ( object ) + 1;
	
	ImageToWindow ( window, &left, &top );
	ImageToWindow ( window, &right, &bottom );
	GSetRect ( rect, left, top, right, bottom );
}

/***  FitImageSurface  ******************************************************************

	Finds the linear least-squares best fit of a mathematical surface to an image.
	
	int FitImageSurface ( PIXEL **data, ImageRegionPtr region, PIXEL min, PIXEL max,
	    ImageSurfaceFunctionPtr surface, short numParams, double *params, double *errors )
	
	(data):      pointer to matrix containing image data to which surface should be fit.
	(region):    pointer to structure defining region of image to which surface should be fit.
	(min):       minimum image data value to which surface should be fit.
	(max):       maximum image data value to which surface should be fit.
	(surface):   pointer to function which computes the terms in the surface equation.
	(numParams): number of parameters which should be fitted.
	(params):    recieves best-fitting parameter values.
	(errors):    recieves standard errors (uncertainties) in best-fit values.
	
	The function returns TRUE if it is able to compute the best-fitting parameters,
	and FALSE otherwise.
	
	The image fitted is the portion of (image) defined by (box); only every (dy)th row and
	(dx)th column is included in the fit, and pixels above (max) and below (min) are
	ignored.  The functions defining the surface are evaluated by (*functions)().  The
	best-fit parameters are returned in (parameters) and their standard deviations in
	(errors).  These must both be big enough to hold (numParameters) values.  (functions)
	must take three arguments: an integer x, y coordinate, and an array in which to
	return (numParameters) terms.  See paraboloid() for an example.
	
*************************************************************************************/
	
int FitImageSurface ( PIXEL **data, ImageRegionPtr region, PIXEL min, PIXEL max,
ImageSurfaceFunctionPtr surface, short numParams, double *params, double *errors )
{
	int				result;
	short			col, row, left, right, i;
	double			**covariances, **parameters, value;

	/*** Allocate an initialize the parameter vector and covariance matrix;
	     on failure, return an error code. ***/
	
	if ( NCreateNormalEqns ( numParams, 1, &covariances, &parameters ) == FALSE )
		return ( FALSE );   
	
	/*** At every pixel in the image which falls inside the specified image region,
	     and whose value is within the specified minimum and maximum, evaluate the
	     terms in the surface function, and augment the normal equations with the
	     evaluated terms and the actual pixel value. ***/
	
	row = -1;
	
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
	{
		for ( col = left; col <= right; col++ )
		{
			value = data[row][col];
			
			if ( value < max && value > min )
			{
				(*surface) ( col, row, params );
				NAugmentNormalEqns ( numParams, 1, params, &value, covariances, parameters );
			}
		}
	}
	
	/*** Solve the normal equations.  If we succeed, copy the best-fit parameters,
	     and compute their standard errors by taking the square root of the diagonal
	     values in the covariance matrix. ***/
	
	result = NGaussJordanSolveMatrixEqn ( covariances, numParams, parameters, 1 );

	if ( result == TRUE )
	{
		for ( i = 0; i < numParams; i++ )
		{
			params[i] = parameters[i][0];
			errors[i] = sqrt ( covariances[i][i] );
		}
	}

	/*** Free the normal equation matrices, and return TRUE or FALSE depending on
	     whether or not we were able to solve the normal equations. ***/

	NDestroyMatrix ( covariances );
	NDestroyMatrix ( parameters );

	return ( result );
}

/*** GaussianSurface ****************************************************************

	Evaluates the linear terms in the equation for a Gaussian surface.
	
	void GaussianSurface ( short x, short y, double terms[] )
	
	(x,y):   coordinates of pixel at which the surface function should be evaluated.
	(terms): recieves evaluated values of terms in the equation.
	
	The equation for a gaussian surface is as follows:
	
	z = B + A * exp ( - ( ( x - X ) ^ 2 + ( y - Y ) ^ 2 ) / ( R ^ 2 ) )
	
	where:
	
	B = terms[0] = background value
	A = terms[1] = amplitude, or peak height of gaussian over background.
	X = terms[2] = x coordinate of centroid of gaussian
	Y = terms[3] = y coordinate of centroid of gaussian
	R = terms[4] = "one-sigma" radius of gaussian.
	
	Note that in this function, X, Y and R are taken as inputs and assumed to be
	constant; hence the gaussian becomes a linear function in terms of A and B.
	
*************************************************************************************/

void GaussianSurface ( short x, short y, double terms[] )
{
	double dx = x - terms[2];
	double dy = y - terms[3];
	double r2 = terms[4] * terms[4];
	
	terms[0] = 1.0;
	terms[1] = exp ( - ( dx * dx + dy * dy ) / r2 );
}

/***************************  FitImageModel  ************************************/

int FitImageModel ( PIXEL **matrix, ImageRegionPtr region, PIXEL min, PIXEL max,
ImageModelFunctionPtr model, short numParams, double params[], double errors[],
double tolerance, short maxIterations )
{
	int		result = FALSE;
	long	iteration;
	double	oldResidual, residual = 0.0, lambda = -1.0;
	
	for ( iteration = 0; iteration < maxIterations; iteration++ )
	{
		oldResidual = residual;
		
		if ( ImproveImageModelFit ( matrix, region, model, params, errors, numParams, &lambda, &residual ) == FALSE )
			break;

		if ( ( fabs ( residual - oldResidual ) / residual ) < tolerance )
		{
			result = TRUE;
			break;
		}
	}

	lambda = 0.0;
	ImproveImageModelFit ( region, matrix, model, params, errors, numParams, &lambda, &residual );
	
	return ( result );
}

/**********************************  improveFit  ************************************

	Improves a fit of a surface to an image.  The image data used to fit the surface
	is defined by (box); pixels above (max) and below (min) are ignored.  The
	surface that gets fit to the image is defined by (function) and the array
	(parameters).  Given integer (x,y) coordinates and the array of parameters,
	(function) must return the value of the function at those coordinates AND its
	first derivative w/ resp. to each to parameter in an array.  (function) must have
	(numParams) parameters and derivative evaluations.
		This function uses an adaptation of the Levenberg-Marqhardt method given in
	"Numerical Recipes in C."  The value in (lambda) is used to keep parameter
	improvements under control- the larger (lambda) is, the smaller the parameter
	improvements will be.  The first time you call improveFit(), give your initial
	guess at the parameters in (parameters) and set (lambda) < 0.  A set of trial
	parameter improvements and the sum-squared residual will be calculated and
	returned in (improvements) and (residual); (lambda) is set to 1000.0.
		On subsequent iterations, the function generates a new set of trial parameters
	and trial improvements by adding the (improvements) to the (parameters).
	The residual for the new trial parameters is calculated, and compared to that
	given in (residual).  If the trial parameters produce a lower residual than the
	given one, (parameters) are replaced with the trial parameters, (residual)
	is replaced with the trial residual, and (lambda) divided by 10.  If the fractional
	decrease in (residual) is less than (tolerance), convergence is assumed and
	(lambda) is set to zero so errors will be calculated properly.  Otherwise,
	(lambda) is multiplied by 10, and the parameters and residual are not updated.
	A new set of improvements are calculated and returned in (improvements).
	
		Stop calling improveFit() when the residual decreases by less than a tolerance
	amount.	 The function returns FALSE if it cannot calculate the trial improvements;
	otherwise it returns TRUE.
	
**************************************************************************************/

int ImproveImageModelFit ( PIXEL **matrix, ImageRegionPtr region, ImageModelFunctionPtr model,
double parameters[], double errors[], short numParams, double *lambda, double *residual )
{
	short	i, j, row, col, left, right, result;
	double	trialResidual, diff;

	static double	**covariances;
	static double	**improvements;
	static double	*trialParameters;
	static double	*derivatives;

	/*** Initialization ***/
	
	if ( *lambda < 0.0 )
	{	
		covariances = NMatrix ( double, numParams, numParams );
		if ( covariances == NULL )
		{
			return ( FALSE );
		}
		
		improvements = NMatrix ( double, numParams, 1 );
		if ( improvements == NULL )
		{
			NDestroyMatrix ( covariances );
			return ( FALSE );
		}
	
		derivatives = NVector ( double, numParams );
		if ( derivatives == NULL )
		{
			NDestroyMatrix ( improvements );
			NDestroyMatrix ( covariances );
			return ( FALSE );
		}
	
		trialParameters = NVector ( double, numParams );
		if ( trialParameters == NULL )
		{
			NDestroyVector ( derivatives );
			NDestroyMatrix ( improvements );
			NDestroyMatrix ( covariances );
			return ( FALSE );
		}
	}
	
	for ( i = 0; i < numParams; i++ )
		trialParameters[i] = parameters[i] + improvements[i][0];

	trialResidual = 0.0;
	row = -1;
	
	while ( GetImageRegionSegment ( region, &row, &left, &right ) )
	{
		for ( col = left; col < right; col++ )
		{
			diff = (*model) ( col, row, trialParameters, derivatives );
			diff = matrix[row][col] - diff;
			trialResidual += diff * diff;
			
			for ( i = 0; i < numParams; i++ )
			{
				for ( j = 0; j < numParams; j++ )
					covariances[i][j] += derivatives[i] * derivatives[j];
					
				improvements[i][0] += diff * derivatives[i];
			}
		}
	}	

	if ( *lambda < 0.0 )
	{
		*lambda = 1000.0;
		*residual = trialResidual;
	}
	else
	{
		if ( trialResidual <= *residual )
		{
			for ( i = 0; i < numParams; i++ )
				parameters[i] = trialParameters[i];
				
			*lambda /= 10.0;
			*residual = trialResidual;
		}
		
		if ( trialResidual > *residual )
		{
			*lambda *= 10.0;
		}
	}
	
	for ( i = 0; i < numParams; i++ )
		covariances[i][i] *= 1.0 + *lambda;
	
	result = NGaussJordanSolveMatrixEqn ( covariances, numParams, improvements, 1 );
	if ( result )	
	{
		for ( i = 0; i < numParams; i++ )
			errors[i] = sqrt ( covariances[i][i] );
	}

	if ( *lambda == 0.0 )
	{	
		NDestroyMatrix ( covariances );
		NDestroyMatrix ( improvements );
		NDestroyVector ( trialParameters );
		NDestroyVector ( derivatives );
	}
	
	return ( result );
}

/***********************************  singleGaussian  ********************************

	Returns the value of a 2D gaussian at (x,y) defined in (parameters); returns
	the value of its first derivatives w/ respect each parameter in (derivatives).
	
**************************************************************************************/
	
double GaussianStarModel ( short x, short y, double parameters[5], double derivatives[5] )
{
	double dx1, dy1, d12, r2, exp1, temp1;
	
	dx1 = x - parameters[2];
	dy1 = y - parameters[3];
	d12 = dx1 * dx1 + dy1 * dy1;
	r2 = parameters[4] * parameters[4];
	exp1 = exp ( - d12 / ( 2.0 * r2 ) );
	temp1 = parameters[1] * exp1 / r2;
	
	derivatives[0] = 1.0;
	derivatives[1] = exp1;
	derivatives[2] = temp1 * dx1;
	derivatives[3] = temp1 * dy1;
	derivatives[4] = d12 * temp1 / parameters[4];
	
	return ( parameters[1] * exp1 + parameters[0] );
}

