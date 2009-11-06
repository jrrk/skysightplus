/***********************  functions in Matrix.c  **************************/

/****************************  NCreateMatrix  *********************************

	Allocates memory for a new matrix of arbitrary dimensions.
	
	void **NewMatrix ( size_t size, long m, long n )
	
	(size): size of an individual matrix element in bytes.
	   (m): desired number of rows for matrix.
	   (n): desired number of columns for matrix.
	
	The function returns an array of pointer to the matrix rows, or NULL
	on failure.  Memory for each matrix row is allocated separately, and
	all elements of each row are initialized to zero.

	Because this function can create a matrix with elements of any size,
	you will need to cast the matrix row pointer array it returns to the
	proper type.
	
	For example, the following code creates a 5 x 10 matrix with short
	integer elements, and another with double-precision floating-point
	elements:
	
	short **matrix1 = (short **) NewMatrix ( sizeof ( short ), 5, 10 );
	double **matrix2 = (double **) NewMatrix ( sizeof ( double ), 5, 10 );
	
	To refer to an individual element in the ith row and jth column, use
	the notation matrix[i][j].  Both the row and column number should be
	counted from zero, following standard C array notation.

	Reference: Numerical Recipes in C, pp. 16-20.
	
***************************************************************************/

void  *NCreateVector ( size_t, long );
void **NCreateMatrix ( size_t, long, long );

#define NVector(type,n)		(type *)  NCreateVector ( sizeof ( type ), n )
#define NMatrix(type,m,n)	(type **) NCreateMatrix ( sizeof ( type ), m, n )

/**************************  NDestroyMatrix  *******************************

	Frees memory for a matrix allocated with NewMatrix().
	
	void NDestroyMatrix ( void **matrix )

	(matrix): array of pointers to rows of matrix to free.
	
	This function returns nothing.  After this funtion is called, the
	matrix row pointer array given (matrix) will be invalid.  You should
	only use this function to free matrices created with NewMatrix()!!!

	Reference: Numerical Recipes in C, pp. 16-20.

*****************************************************************************/

void NDestroyVector ( void * );
void NDestroyMatrix ( void ** );

void NCopyVector ( double *, double *, long );
void NCopyMatrix ( double **, double **, long, long );

void NInitializeMatrix ( double **, long, long, double );
void NIdentityMatrix ( double **, long, long );

void NTransposeMatrix ( double **, double **, long, long );
void NMultiplyMatrix ( double **, double **, double **, long, long, long );

/***************************  NCreateNormalEqns  *******************************/

int NCreateNormalEqns ( long, long, double ***, double *** );
void NAugmentNormalEqns ( long, long, double *, double *, double **, double ** );

/**********************  GaussJordanSolveMatrixEqn  ***********************

	Linear equation solution by Gauss-Jordan elimination.
	
	void GaussJordanSolveMatrixEqn ( double **a, long m, double **b, long n )
	
	(a): input matrix of (m) by (m) elemnts.
	(b): input matrix of (m) rows and (n) columns.
	
	The function returns TRUE if it is successfully able to solve the
	matrix equation, and FALSE otherwise.
	
	On output, (a) is replaced by its inverse, and (b) is replaced by the
	corresponding set of solution vectors.
	
	Reference: Numerical Recipes in C, pp. 32-36
	
****************************************************************************/

int NGaussJordanSolveMatrixEqn ( double **, long, double **, long );

