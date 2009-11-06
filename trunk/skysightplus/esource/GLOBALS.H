 /*
 				globals.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	global declarations.
*
*	Last modify:	07/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include    <stdarg.h>
#include	"types.h"

/*----------------------- miscellaneous variables ---------------------------*/

catstruct		cat;
picstruct		field;
prefstruct		prefs;
objstruct		flagobj;
obj2struct		flagobj2;
extern obj2struct	outobj2;
PIXTYPE			*dumscan;
double			ctg[37], stg[37];
LONG			*ghisto;
char			*bufdata;


/*------------------------------- functions ---------------------------------*/

int	unprotected_sextract_main(int argc, char *argv[]);
int protected_sextract_main(char *path);
/*
  malloc(size_t n)
  Returns a pointer to a newly allocated chunk of at least n bytes, or
  null if no space is available, in which case errno is set to ENOMEM
  on ANSI C systems.

  If n is zero, malloc returns a minimum-sized chunk. (The minimum
  size is 16 bytes on most 32bit systems, and 32 bytes on 64bit
  systems.)  Note that size_t is an unsigned type, so calls with
  arguments that would be negative if signed are interpreted as
  requests for huge amounts of space, which will often fail. The
  maximum supported value of n differs across systems, but is in all
  cases less than the maximum representable value of a size_t.
*/
void* myalloc(size_t);

/*
  myfree(void* p)
  Releases the chunk of memory pointed to by p, that had been previously
  allocated using malloc or a related routine such as realloc.
  It has no effect if p is null. If p was not malloced or already
  freed, free(p) will by default cause the current program to abort.
*/
void  myfree(void*);

/*
  mycalloc(size_t n_elements, size_t element_size);
  Returns a pointer to n_elements * element_size bytes, with all locations
  set to zero.
*/
void* mycalloc(size_t, size_t);

/*
  myrealloc(void* p, size_t n)
  Returns a pointer to a chunk of size n that contains the same data
  as does chunk p up to the minimum of (n, p's size) bytes, or null
  if no space is available.

  The returned pointer may or may not be the same as p. The algorithm
  prefers extending p in most cases when possible, otherwise it
  employs the equivalent of a malloc-copy-free sequence.

  If p is null, realloc is equivalent to malloc.

  If space is not available, realloc returns null, errno is set (if on
  ANSI) and p is NOT freed.

  if n is for fewer bytes than already held by p, the newly unused
  space is lopped off and freed if possible.  realloc with a size
  argument of zero (re)allocates a minimum-sized chunk.

  The old unix realloc convention of allowing the last-free'd chunk
  to be used as an argument to realloc is not supported.
*/

void* myrealloc(void*, size_t);

/*
  memalign(size_t alignment, size_t n);
  Returns a pointer to a newly allocated chunk of n bytes, aligned
  in accord with the alignment argument.

  The alignment argument should be a power of two. If the argument is
  not a power of two, the nearest greater power is used.
  8-byte alignment is guaranteed by normal malloc calls, so don't
  bother calling memalign with an argument of 8 or less.

  Overreliance on memalign is a sure way to fragment space.
*/
void heapwalk(void);
void sexit(int);

extern void	addcleanobj(int, objliststruct *, objliststruct *),
		analyse(int, objliststruct *),
		backstat(backstruct *, PIXTYPE *, int, int, int, int),
		clean(int, objliststruct *),
                closecat(void),
                closecheck(void),
		computeastrom(objstruct *),
		computeapermag(objstruct *),
		computeautomag(objstruct *),
		computeisomag(objstruct *),
		convolve(PIXTYPE *),
		endclean(void),
		endobject(int, objliststruct *),
		error(int, char *, ...),
		examineiso(objstruct *, pliststruct *),
		filterback(void),
		flagcleancrowded(int, objliststruct *),
		freeparcelout(void),
		getconv(void),
		getnnw(void),
		initastrom(void),
		initcat(void),
		initcheck(void),
		initclean(void),
		initfitscat(void),
		lutzalloc(void),
		lutzfree(void),
		lutzsort(infostruct *, objliststruct *),
		makeback(void),
		makeit(void),
		mergeobject(objstruct *, objstruct *),
		neurinit(void),
		neurclose(void),
		neurresp(double *, double *),
		preanalyse(int, objliststruct *, int),
/*MAMAspecific*/precess(double, double, double, double, double *, double *),
		readcatparams(char *),
		readdata(PIXTYPE *, int),
		readimagehead(void),
		readprefs(char *, char **, char **, int),
		scanimage(void),
		sexcircle(PIXTYPE *bmp, int, int, double, double, double,
			PIXTYPE),
		sexdraw(PIXTYPE *bmp, int, int, double, double, PIXTYPE),
		sexellips(PIXTYPE *bmp, int, int, double, double, double,
			double, double, PIXTYPE, int),
		sexmove(double, double),
		sortit(infostruct *, objliststruct *),
		subcleanobj(int, objliststruct *),
		swapbytes(void *, int, int),
		update(infostruct *, infostruct *, pliststruct *),
		updateparamflags(void),
		useprefs(void),
		warning(char *, char *),
		writecat(int, objliststruct *),
/*MAMAspecific*/xytoadII(mapstruct *, double, double, double,
			double *, double *),
/*MAMAspecific*/xytoxyII(mapstruct *, double *, double *);

extern float	backguess(backstruct *, float *, float *),
		hmedian(float *, int),
		localback(objstruct *);

extern int	addobj(int, objliststruct *, objliststruct *),
		belong(int, objliststruct *, int, objliststruct *),
		cistrcmp(char *, char *),
		createbmp(int, objliststruct *),
		findkey(char *, char key[][16]),
		fitsadd(char *, char *, char *),
		fitsfind(char *, char *),
		fitswrite(char *, char *, void *, h_type, int),
		gatherup(objliststruct *, objliststruct *),
		lutz(int, int, int, int, int, objliststruct *),
		parcelout(objliststruct *, objliststruct *);

extern PIXTYPE	back(int, int),
		*loadstrip(void);

extern char	*fitsnfind(char *, char *, int),
		*readfitshead(FILE *, char *, int *);
