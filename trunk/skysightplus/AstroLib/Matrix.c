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

/****************************  NCreateVector  *********************************/

void *NCreateVector ( size_t size, long n )
{
	void *vector;
	
	vector = (void *) malloc ( size * n );
	if ( vector == NULL )
		return ( NULL );
		
	memset ( vector, 0, size * n );
	return ( vector );
}

/****************************  NCreateMatrix  *********************************/

void **NCreateMatrix ( size_t size, long m, long n )
{
	long	i;
	void	**matrix;
	
	if ( m == 0 || n == 0 )
		return ( NULL );
	
	matrix = (void **) NCreateVector ( sizeof ( void * ), m + 1 );
	if ( matrix == NULL )
		return ( NULL );
			
	for ( i = 0; i < m; i++ )
	{
		matrix[i] = NCreateVector ( size, n );
		if ( matrix[i] == NULL )
		{
			NDestroyMatrix ( matrix );
			return ( NULL );
		}
	}
	
	return ( matrix );
}

/****************************  NDestroyVector  *******************************/

void NDestroyVector ( void *vector )
{
	free ( vector );
}

/***************************  NDestroyMatrix  *********************************/

void NDestroyMatrix ( void **matrix )
{
	long i;
	
	for ( i = 0; matrix[i]; i++ )
		free ( matrix[i] );
		
	free ( matrix );
}

/*****************************  NCopyVector  ********************************/

void NCopyVector ( double *b, double *a, long n )
{
	long i;
	
	for ( i = 0; i < n; i++ )
		b[i] = a[i];
}

/*****************************  NCopyMatrix  ********************************/

void NCopyMatrix ( double **b, double **a, long m, long n )
{
	long i, j;
	
	for ( i = 0; i < m; i++ )
		for ( j = 0; j < n; j++ )
			b[i][j] = a[i][j];
}

/**************************  NInitializeMatrix  ******************************/

void NInitializeMatrix ( double **a, long m, long n, double x )
{
	long i, j;
	
	for ( i = 0; i < m; i++ )
		for ( j = 0; j < n; j++ )
			a[i][j] = x;
}

/**************************  NTransposeMatrix  *******************************/

void NTransposeMatrix ( double **b, double **a, long m, long n )
{
	long i, j;
	
	for ( i = 0; i < m; i++ )
		for ( j = 0; j < n; j++ )
			b[j][i] = a[i][j];
}

/****************************  NMultiplyMatrix  *****************************/

void NMultiplyMatrix ( double **c, double **a, double **b, long m, long n, long p )
{
	long i, j, k;
	
	for ( i = 0; i < m; i++ )
		for ( k = 0; k < p; k++ )
			for ( c[i][k] = 0.0, j = 0; j < n; j++ )
				c[i][k] += a[i][j] * b[j][k];
}

/****************************  NIdentityMatrix  ******************************/

void NIdentityMatrix ( double **a, long m, long n )
{
	long i, j;
	
	for ( i = 0; i < m; i++ )
		for ( j = 0; j < n; j++ )
			if ( i == j )
				a[i][j] = 1.0;
			else
				a[i][j] = 0.0;
}

/****************************  NCreateNormalEqns  ******************************

	Allocates and initializes normal equation matrices for a least-squares
	solution to an overdetermined linear system of equations.
	
	int NCreateNormalEqns ( long m, long n, double ***a, double ***b )
	
	(m): number of independent or left-hand-side (LHS) variables.
	(n): number of dependent or right-hand-side (RHS) variables.
	(a): recieves pointer to LHS normal equation matrix.
	(b): recieves pointer to RHS normal equation matrix.
	
	The function returns TRUE if successful and FALSE on failure.
	
	On a successful return, the variable (a) will point to a matrix of
	(m) rows by (m) columns, and (b) will point to a matrix of (m) rows
	by (n) columns.  Both matrices will be initialized to zero.
	
	The values required for the parameters (m) and (n) depend on the
	the system of equations whose least-squares solution you wish to
	determine.  The variable (m) represents the number of independent
	(LHS) variables in the equation, and (n) represents the number of
	dependent (RHS) variables.

	In general, a linear system of equations with M independent variables
	and N dependent variables takes the following form:

	y1 = a11 * x1 + a12 * x2 + ... + a1M * xM
	
	y2 = a21 * x2 + a22 * x2 + ... + a2M * xM
	
	.        .          .      .         .
	.        .          .       .        .
	.        .          .        .       .
	
	yN = aN1 * xN + aN2 * x2 + ... + aNM * xM

	Here, x1...xM are the independent variables, y1...yN are the dependent
	variables, and a11...aNM are the unknown coefficients whose values will
	be determined by least-squares solution.

	We can write this in vector-matrix form as y = x * A, where:
	
	y = | y1  y2  ... yN  | is the vector of dependent variables;
	
	x = | x1  x2  ... xM  | is the vector of independent variables;
	
	    | a11 a21 ... aN1 |  
	    |                 |
	    | a12 a22 ... aN2 |
	    |                 |
	A = |  .   .  .    .  | is the matrix of unknown coefficients.
	    |  .   .   .   .  |
	    |  .   .    .  .  |
	    |                 |
	    | a1M a2M ... aNM |

	In order to solve for the matrix of unknown coefficients, you must have
	a set of "data points" for which the values of both the dependent and
	independent variables are known.  The system of equations with the
	values of the data points "plugged in" can then be written as:
	
	Y = X * A

	Here, each row of matrix X contains the values of the dependent variables
	x1...xM at a particular data point, and the corresponding row of matrix Y
	contains the values of the dependent variables y1...yN at that same point.
	If the number of known data points is P, then matrix X will contain P rows
	and M columns, while matrix Y will contain P rows and N columns.
	
	In order to solve for the matrix A, the number of data points (P) must be
	equal to or greater than the number of rows of the coefficient matrix (M),
	so that the number of known values equals or exceeds the number of unknown
	coefficients.
	
	 T       T
	X * Y = X * X * A

	  T    -1    T
	(X * X)  * (X * Y) = A
	
****************************************************************************/

int NCreateNormalEqns ( long m, long n, double ***a, double ***b )
{
	long i, j;
	
	*a = NMatrix ( double, m, m );
	if ( *a == NULL )
		return ( FALSE );
		
	*b = NMatrix ( double, m, n );
	if ( *b == NULL )
	{
		NDestroyMatrix ( *a );
		return ( FALSE );
	}
		
	for ( i = 0; i < m; i++ )
	{
		for ( j = 0; j < m; j++ )
			(*a)[i][j] = 0.0;
		
		for ( j = 0; j < n; j++ )
			(*b)[i][j] = 0.0;
	}
	
	return ( TRUE );	
}

/**************************  NAugmentNormalEqns  ***************************

	Adds the values of a known data point to the normal equations.
	
	void NAugmentNormalEnqs ( long m, long n, double *x, double *y, double **a, double **b )
	
	(m): number of independent or right-hand-side (RHS) variables.
	(n): number of dependent or left-hand-side (LHS) variables.
	(x): values of the independent variables at a known data point.
	(y): values of the dependent variables at a known data point.
	(a): pointer to RHS normal equation matrix.
	(b): pointer to LHS normal equation matrix.
	
	This function returns nothing.  It assumes that (a) points to a matrix
	of (m) rows by (m) columns, and that (b) points to a matrix of (m) rows
	by (n) columns.  Both matrices should be zero-initialized before calling
	this function.  You can ensure that this has been done correctly by
	using the function NewNormalEqns() to create both matrices.  After you
	have done so, you should call AugmentNormalEqns() once for each known
	data point you wish to include in the least-squares solution.
	
	After you have finished calling AugmentNormalEqns() for all of the
	known data points, the normal equation matrices will contain the sums
	of the products of the values of the dependent and independent variables
	at each of the known data points.
	
	The RHS normal equation matrix (a) contains the sums:
	
	sum(x1*x1) sum(x1*x2) ... sum(x1*xM)
	
	sum(x2*x1) sum(x2*x2) ... sum(x2*xM)
	
	    .          .      .       .
	    .          .       .      .
	    .          .        .     .
	
	sum(xM*x1) sum(xM*x2)     sum(xM*xM)
	
	The LHS normal equation matrix (b) contains the sums:
	
	sum(x1*y1) sum(x1*y2) ... sum(x1*yN)
	
	sum(x2*y1) sum(x2*y2) ... sum(x2*yN)
	
	    .          .      .       .
	    .          .       .      .
	    .          .        .     .
	
	sum(xM*y1) sum(xM*y2) ... sum(xM*yN)

	Here, x1 ... xM are the independent variables, y1 ... yN are the
	dependent variables, and sum(...) means "sum over all of the known
	data points".
	
	At this point the original least-squares problem can be rewritten
	as
	
	Y = X * A
	
	where X is the RHS normal equation matrix (a), Y is the LHS normal
	equation matrix (b).
	
	To solve the normal equations and obtain a least-squares solution,
	you may call the routine 
	
***************************************************************************/

void NAugmentNormalEqns ( long m, long n, double *x, double *y, double **a, double **b )
{
	long i, j;
	
	for ( i = 0; i < m; i++ )
	{
		for ( j = 0; j < m; j++ )
			a[i][j] += x[i] * x[j];
		
		for ( j = 0; j < n; j++ )
			b[i][j] += x[i] * y[j];
	}
}

/***********************  GaussJordanSolveMatrixEqn  **********************/
	
int NGaussJordanSolveMatrixEqn ( double **a, long m, double **b, long n )
{
	long	*indxc, *indxr, *ipiv;
	long	i, j, k, l, ll, irow, icol;
	double	big, temp, pivinv;
	
	/*** these arrays are used for bookkeeping on the pivoting ***/
		
	if ( ( indxr = NVector ( long, m ) ) == NULL )
		return ( FALSE );
		
	if ( ( indxc = NVector ( long, m ) ) == NULL )
	{
		free ( indxr );
		return ( FALSE );
	}
	
	if ( ( ipiv = NVector ( long, m ) ) == NULL )
	{
		free ( indxr );
		free ( indxc );
		return ( FALSE );
	}
	
	for ( j = 0; j < m; j++ )
		ipiv[j] = 0;

	/*** this is the main loop over the columns to be reduced. ***/
	
	for ( i = 0; i < m; i++ )
	{	
		big = 0.0;
		
		/*** this is the outer loop of the search for a pivot element ***/
		
		for ( j = 0; j < m; j++ )
		{
			if ( ipiv[j] != 1 )
			{
				for ( k = 0; k < m; k++ )
				{
					if ( ipiv[k] == 0 )
					{
						if ( ( temp = fabs ( a[j][k] ) >= big ) != 0.0 )
						{
							big = temp;
							irow = j;
							icol = k;
						}
					}
					else
					{
						if ( ipiv[k] > 1 )
						{
							free ( indxc );
							free ( indxr );
							free ( ipiv );
							return ( FALSE );
						}
					}
				}
			}
		}
		
		++( ipiv[icol] );
		
		/*** We now have the pivot element, so we interchange rows, if needed, to put
			the pivot element on the diagonal.  The columns are not physically inter-
			changed, only relabeled: indxc[i], the column of the ith pivot element, is
			the ith column that is reduced, while indxr[i] is the row in which that
			pivot element was originally located.  If indxr[i] ­ indxc[i] there is an
			implied column interchange.  With this form of bookkeeping, the solution
			matrix will end up the correct order, and the inverse matrix will be
			scrambled by columns. ***/
			
		if ( irow != icol )
		{
			for ( l = 0; l < m; l++ )
			{
				temp = a[irow][l];
				a[irow][l] = a[icol][l];
				a[icol][l] = temp;
			}
			
			for ( l = 0; l < n; l++ )
			{
				temp = b[irow][l];
				b[irow][l] = b[icol][l];
				b[icol][l] = temp;
			}
		}
			
		/*** We are now ready to divide the pivot row by the pivot element, located
			at irow and icol ***/
		
		indxr[i] = irow;
		indxc[i] = icol;
		
		if ( ( temp = a[icol][icol] ) == 0.0 )
		{
			free ( indxc );
			free ( indxr );
			free ( ipiv );
			return ( FALSE );
		}
		
		pivinv = 1.0 / temp;
		a[icol][icol] = 1.0;
		
		for ( l = 0; l < m; l++ )
			a[icol][l] *= pivinv;
			
		for ( l = 0; l < n; l++ )
			b[icol][l] *= pivinv;
			
		/*** next, we reduce the rows, except for the pivot one... ***/
		
		for ( ll = 0; ll < m; ll++ )
		{
			if ( ll != icol )
			{
				temp = a[ll][icol];
				a[ll][icol] = 0.0;
				
				for ( l = 0; l < m; l++ )
					a[ll][l] -= a[icol][l] * temp;
					
				for ( l = 0; l < n; l++ )
					b[ll][l] -= b[icol][l] * temp;
			}
		}
	}
	
	/*** This is the end of the main loop over columns of the reduction.  It only remains
		to unscrable the solution in view of the column interchanges.  We do this by
		interchanging pairs of columns in the reverse order that the permutation was
		built up. ***/
		
	for ( l = m - 1; l >= 0; l-- )
	{
		if ( indxr[l] != indxc[l] )
			for ( k = 1; k < m; k++ )
			{
				temp = a[k][indxr[l]];
				a[k][indxr[l]] = a[k][indxc[l]];
				a[k][indxc[l]] = temp;
			}
	}
	
	/*** And we are done. ***/
	
	free ( indxr );
	free ( indxc );
	free ( ipiv );
	
	return ( TRUE );
}

