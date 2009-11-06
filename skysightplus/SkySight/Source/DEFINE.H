 /*
 				define.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	global definitions.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

extern FILE *logfile;

/*------------------------ what, who, when and where ------------------------*/

#define		BANNER		"SExtractor"
#define		VERSION		"1.0a (May 31 1995)"
#define		COPYRIGHT	"Emmanuel BERTIN (bertin@iap.fr)"
#define		INSTITUTE	"IAP/ESO"


/*----------------------------- Internal constants --------------------------*/

#define	QUANTIF_NSIGMA		5		/* histogram limits */
#define	QUANTIF_NMAXLEVELS	4096		/* max nb of quantif. levels */
#define	QUANTIF_NMINLEVELS (1+2*QUANTIF_NSIGMA)	/* min nb of quantif. levels */
#define	QUANTIF_AMIN		4		/* min nb of "mode pixels" */

#define	POLLUTE_CONTRAST	0.1		/* above that fraction of */
						/* flux a neighbour pollutes*/
#define	POLLUTE_RADIUS		3.5		/* action "radius" of a  */
						/* pollutor */

#define	NOMANSLAND_NSIGMA	5.0		/* inner size of LOCALBACK */
						/* annulus */

#define	OUTPUT			logfile		/* where all msgs are sent */
#define	NISO			8		/* number of isophotes */
#define	DATA_BUFSIZE		262144		/* data buffer size */
#define	BACK_BUFSIZE		1048576		/* bkgnd buffer */
#define	MAXCHAR			256		/* max. number of characters */
#define	MAXPICSIZE		65534		/* max. image size */
#ifndef PI
#define	PI			3.14159265359	/* never met before? */
#endif

/* NOTES:

We must have:	0 < QUANTIF_NSIGMA <= 10
		QUANTIF_NMAXLEVELS > QUANTIF_NMINLEVELS
		QUANTIF_AMIN > 0
		POLLUTE_CONSTRAST >= 0
		POLLUTE_RADIUS >= 0
		NOMANSLAND_NSIGMA >= 0.0 (preferably >= 3.0)
		NISO = 8 (otherwise need to change prefs.h)
		DATA_BUFSIZE >= 8 with DATA_BUFSIZE%8 = 0
		BACK_BUFSIZE >= MAXPICSIZE
		MAXCHAR >= 16
		MAXPICSIZE > size of images!!
*/

/*--------------------- in case of missing constants ------------------------*/

#ifdef	NOSMALLHUGE
#define	BIG			1e+30	/* a huge number */
#else
#define	BIG			HUGE_VAL
#endif

#ifndef		SEEK_SET
#define		SEEK_SET	0
#endif
#ifndef		SEEK_CUR
#define		SEEK_CUR	1
#endif

#ifndef	EXIT_SUCCESS
#define	EXIT_SUCCESS		0
#endif
#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE		-1
#endif

/*------------------------ extraction definitions --------------------------*/

#define	NOBJ			256		/* starting number of obj. */
#define	UNKNOWN			65535		/* flag for LUTZ */

/*------------------- a few definitions to read FITS parameters ------------*/

#define	FBSIZE	2880L	/* size (in bytes) of one FITS block */

#define	FITSTOF(k, def) \
			(st[0]=0,((point = fitsnfind(buf, k, n))? \
				 atof(strncat(st, &point[10], 70)) \
				:(def)))

#define	FITSTOI(k, def) \
			(st[0]=0,(point = fitsnfind(buf, k, n))? \
				 atoi(strncat(st, &point[10], 70)) \
				:(def))

#define	FITSTOS(k, str, def) \
			{ if (point = fitsnfind(buf, k, n)) \
				{ \
				for (i=0,point+=11; (*point)!='\'';) \
					str[i++] = *(point++); \
				str[i] = '\0'; \
				} \
			  else\
				strcpy(str, def); \
			}


/*------------------------------- Other Macros -----------------------------*/

#define QFREAD(ptr, size, file, fname) \
		if (fread(ptr, (size_t)(size), (size_t)1, file)!=1) \
		  error(EXIT_FAILURE, "*Error* while reading ", fname)

#define QFWRITE(ptr, size, file, fname) \
		if (fwrite(ptr, (size_t)(size), (size_t)1, file)!=1) \
		  error(EXIT_FAILURE, "*Error* while writing ", fname)

#define	QFSEEK(file, offset, pos, fname) \
		if (fseek(file, (offset), pos)) \
		  error(EXIT_FAILURE,"*Error*: file positioning failed in ", \
			fname)

#define	QFTELL(pos, file, fname) \
		if ((pos=ftell(file))==-1) \
		  error(EXIT_FAILURE,"*Error*: file position unknown in ", \
			fname)


#define	QFREE(x)	if (x)	free(x)

#define	QCALLOC(ptr, typ, nel) \
		if (!(ptr = (typ *)calloc((size_t)(nel),sizeof(typ)))) \
		  error(EXIT_FAILURE, "Not enough memory for ", \
			#ptr " (" #nel " elements) !")

#define	QMALLOC(ptr, typ, nel) \
		if (!(ptr = (typ *)malloc((size_t)(nel)*sizeof(typ)))) \
		  error(EXIT_FAILURE, "Not enough memory for ", \
			#ptr " (" #nel " elements) !")

#define	RINT(x)	(int)(floor(x+0.5))

#define	PIX(x, y)	field.strip[(((int)y)%field.stripheight) \
				*field.width +(int)x]

#define	NPRINTF		if (prefs.verbose_type == NORM) fprintf

#define	NFPRINTF(w,x)	{if (prefs.verbose_type == NORM) \
				fprintf(w, "\33[1M> %s\n\33[1A",x); \
			else if (prefs.verbose_type == FULL) \
				fprintf(w, "%s.\n", x);}

#define	FPRINTF		if (prefs.verbose_type == FULL)	fprintf

#define FLAG(x)		(*((char *)&flag##x))
