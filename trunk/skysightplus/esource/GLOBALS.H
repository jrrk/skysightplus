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
		error(int, char *, char *),
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
