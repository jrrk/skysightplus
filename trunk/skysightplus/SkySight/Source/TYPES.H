 /*
 				types.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	global type definitions.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include	<stdio.h>

/*-------------------------------- flags ------------------------------------*/

#define		OBJ_CROWDED	0x0001
#define		OBJ_MERGED	0x0002
#define		OBJ_SATUR	0x0004
#define		OBJ_TRUNC	0x0008
#define		OBJ_APERT_PB	0x0010
#define		OBJ_ISO_PB	0x0020
#define		OBJ_DOVERFLOW	0x0040
#define		OBJ_OVERFLOW	0x0080

/*---------------------------- preanalyse flags -----------------------------*/

#define		ANALYSE_FAST		0
#define		ANALYSE_FULL		1
#define		ANALYSE_NOTHRESH	2

/*--------------------------- FITS BitPix coding ----------------------------*/

#define		BP_BYTE		8
#define		BP_SHORT	16
#define		BP_LONG		32
#define		BP_FLOAT	(-32)
#define		BP_DOUBLE	(-64)

/*---------------------------- return messages ------------------------------*/

#define		RETURN_OK		0
#define		RETURN_ERROR		(-1)
#define		RETURN_FATAL_ERROR	(-2)


/*--------------------------------- typedefs --------------------------------*/
typedef	unsigned char	BYTE;			/* a byte */
typedef	float		PIXTYPE;		/* pixel type */
typedef	unsigned short	USHORT;			/* 0 to 65535 integers */

#if defined(__ALPHA) || defined(__ALPHA_AXP)
typedef int LONG;
#else
typedef long LONG;
#endif

typedef enum            {H_INT, H_FLOAT, H_EXPO, H_BOOL, H_STRING, H_COMMENT,
			H_KEY}	h_type;		/* type of FITS-header data */

typedef enum		{T_BYTE, T_SHORT, T_LONG, T_FLOAT, T_DOUBLE}
				t_type;		/* Type of data */

typedef	enum		{COMPLETE, INCOMPLETE, NONOBJECT, OBJECT}
				status;		/* extraction status */


/*------------------- temporary object parameters during extraction ---------*/
typedef struct
  {
  LONG		pixnb;
  LONG		firstpix;
  LONG		lastpix;
  short		flag;
  }       infostruct;


/*---------------------------- background infos -----------------------------*/
typedef struct
  {
  double	mode, mean, sigma;	/* Background mode, mean and sigma */
  LONG		*histo;			/* pointer to a histogram */
  int		nlevels;		/* nb of histogram bins */
  double	qzero, qscale;		/* position of histogram */
  double	lcut, hcut;		/* histogram cuts */
  int		npix;			/* number of pixels involved */
  }	backstruct;


/*--------------------------- linear mapping infos --------------------------*/
typedef struct
  {
  double	px, py;			/* Pixel coordinates of origin */
  double	cx, cy;			/* World coordinates of origin */
  double	fx, fy;			/* World coord. step per pixel */
  double	axx,axy,ayx,ayy;	/* Element of the shear/rot.. matrix */
  }	lmapstruct;


/*---------------------------- MAMA mapping infos ---------------------------*/
typedef struct  {
                double          alphacent, deltacent;
                double          xcent,ycent;
                double          equinox;
                double          foc;
                double          x, y;
                double          crpixx, crpixy;
                double          pdx, pdy;
                double          epoch;
                double          ax[10], bx[10], cx[10]; /* parameters for I */
                double          dx[10], dy[10];         /* parameters for II*/
                }       mapstruct;


/*---------------------------- pixel list elements --------------------------*/
typedef struct
  {
  USHORT	x, y;
  PIXTYPE	value, cvalue;
  LONG		nextpix;
  }	pliststruct;


/*--------------------------------- objects ---------------------------------*/
/* I: "PIXEL" parameters */

typedef struct
  {
/* ---- basic parameters */
  LONG		number;				/* ID */
  LONG		pixnb;				/* nb of extracted pix */
  LONG		scannb;				/* nb of pix above thresh */
  double	rawvalue;			/* integrated raw flux */
  double	totalvalue;			/* integrated isoph. flux */
  double	totalvar;			/* integrated variance */
  PIXTYPE	maxflux;			/* peak intensity (ADU) */
/* ---- photometric data */
  float		apermag;			/* APER mag */
  float		sigmaapermag;			/* APER mag uncertainty */
  float		automag;			/* AUTO mag */
  float		sigmaautomag;			/* AUTO mag uncertainty */
  float		kronfactor;			/* kron parameter */
/* ---- astrometric data */
  float		mx, my;				/* barycenter */
/* ---- morphological data */			
  USHORT	xmin,xmax, ymin,ymax, height, ylim;	/* x,y limits */
  float		a, b, theta, abcor;		/* moments and angle */
  float		cxx,cyy,cxy;			/* ellipse parameters */
  LONG		firstpix;			/* ptr to first pixel */
  LONG		lastpix;			/* ptr to last pixel */
  float		bkg, dbkg, sigbkg;		/* Background stats (ADU) */
  float		thresh;				/* extract. threshold (ADU) */
  LONG		iso[NISO];			/* isophotal areas */
  short		flag;				/* extraction flags */
  float		fwhm;				/* IMAGE FWHM */
  }	objstruct;

/* II: "BLIND" parameters */
typedef struct
  {
/* ---- photometric data */
  float		mag;				/* BEST mag */
  float		sigmag;				/* BEST mag uncertainty */
  float		isomag;				/* ISO mag */
  float		sigmaisomag;			/* ISO mag uncertainty */
  float		isocormag;			/* ISOCOR mag */
  float		sigmaisocormag;			/* ISOCOR mag uncertainty */
/* ---- astrometric data */
  double	mxw, myw;			/* WORLD barycenters */
  float		aw, bw;				/* WORLD ellipse size */
  float		thetaw;				/* WORLD positionn angle */
  float		scannbw;			/* WORLD isophotal area */
  float		threshmu;			/* det. surface brightnees */
  float		maxmu;				/* max. surface brightnees */
  float		e;				/* ellipticity */
  float		sprob;				/* Stellarity index */
  float		fwhmw;				/* WORLD FWHM */
  double	alpha, delta;			/* WORLD alpha, delta */
  }	obj2struct;

/*----------------------------- lists of objects ----------------------------*/
typedef struct
  {
  int		nobj;
  objstruct	*obj;
  int		npix;
  pliststruct	*plist;
  double	cthresh;
  }	objliststruct;


/*----------------------------- image parameters ----------------------------*/
typedef struct
  {
  char		*filename;		/* pointer to the image filename */
  char		ident[MAXCHAR];		/* field identifier (read from FITS)*/
  char		rident[MAXCHAR];	/* field identifier (relative) */
  FILE		*file;			/* pointer the image file structure */
  char		*fitshead;		/* pointer to the FITS header */
  int		fitsheadsize;		/* FITS header size */
/* ---- main image parameters */
  int		bitpix, bytepix;	/* nb of bits and bytes per pixel */
  int		width, height;		/* x,y size of the field */
  int		npix;			/* total number of pixels */
  double	bscale, bzero;		/* FITS scale and offset */
  double	ngamma;			/* normalized photo gamma */
  int		nlevels;		/* nb of quantification levels */
  float		pixmin, pixmax;		/* min and max values in frame */
  int		y;			/* y current position in field */
  int		ymin;			/* y limit (lowest accessible) */
  int		ymax;			/* y limit (highest accessible+1) */
  PIXTYPE	*strip;			/* pointer to the image buffer */
  int		stripheight;		/* height  of a strip (in lines) */
  int		stripmargin;		/* number of lines in margin */
  int		stripstep;		/* number of lines at each read */
  int		stripy;			/* y position in buffer */
  int		stripylim;		/* y limit in buffer */
  int		stripysclim;		/* y scroll limit in buffer */
/* ---- clean-list ----*/
  objliststruct	*cleanobjlist;		/* ptr to the "clean-objlist" */
/* ---- sub image parameters */
  PIXTYPE	*sub, *csub;		/* pointer to the sub bitmaps */
  int		subwidth, subheight;	/* size of subimage */
  int		subx, suby;		/* offset to subimage coordinates */
  int		subflag;		/* !=0 if new bitmap */
/* ---- convolution mask parameters */
  double	*conv;			/* pointer to the convolution mask */
  int		convw, convh;		/* x,y size of mask */
  int		nconv;			/* convw*convh */
/* ---- basic astrometric parameters */
  double	epoch;			/* epoch for coordinates */
  double	pixscale;		/* pixel size in arcsec.pix-1 */
/* ---- background parameters */
  float		*back;			/* ptr to the background map in mem */
  int		backw, backh;		/* x,y size of a bkgnd mesh */
  int		nbackp;			/* total nb of pixels per bkgnd mesh */
  int		nbackx, nbacky;		/* x,y number of bkgnd meshes */
  int		nback;			/* total number of bkgnd meshes */
  int		nbackfx, nbackfy;	/* x,y size of bkgnd filtering mask */
  float		backmean;		/* median bkgnd value in image */
  float		backsig;		/* median bkgnd rms in image */
  double	thresh;			/* extraction threshold */
/* ---- astrometric parameters */
  int		crpixx,crpixy;		/* FITS CRPIXn */
  double	crvalx,crvaly;		/* FITS CRVALn */
  double	cdeltx,cdelty;		/* FITS CDELTn */
  double	crotax,crotay;		/* FITS CROTAn */
  lmapstruct	lmap;			/* linear mapping data */
  mapstruct	map;			/* MAMA mapping data */
/* ---- output parameters */
  int		ndetect;		/* nb of detections */
  int		nfinal;			/* nb of final objects */
  int		flag;
/* ---- Check-image parameters */
  char		*check_name;		/* ptr to check-image filename */
  FILE		*check_file;		/* ptr to check-image file structure */
  void		*check_bmp;		/* ptr to check-image bitmap */
  }	picstruct;


/*---------------------------- preference keywords --------------------------*/
typedef struct
  {
  char		name[16];
  enum	{P_FLOAT, P_INT, P_STRING, P_BOOL, P_KEY}	type;
  void		*ptr;
  int		imin, imax;
  double	dmin, dmax;
  char		keylist[8][16];
  int		flag;
  }	keystruct;

/*---------------------------- parameters keywords --------------------------*/
typedef struct
  {
  char		name[16];
  int		t_type;
  void		*ptr;
  char		format[16];
  h_type	headtype;
  }	paramstruct;

/*------------------------------- preferences -------------------------------*/
typedef struct
  {
  char		prefs_name[MAXCHAR];			/* prefs filename*/
  char		prefs_rname[MAXCHAR];			/* prefs filename*/
  char		image_name[MAXCHAR];			/* image filename*/
  char		cat_name[MAXCHAR];			/* catalog filename*/
/*----- thresholding */
  enum	{T_SIGMA, T_MAGNITUDE}		threshold_type;	/* threshold unit */
  double	threshold;				/* thresh.in sigma */
/*----- extraction */
  int		ext_minarea;				/* min area in pix. */
  int		conv_flag;				/* smoothing on/off */
  int		convnorm_flag;				/* normalize on/off */
  char		conv_name[MAXCHAR];			/* mask filename */
  char		conv_rname[MAXCHAR];			/* relative name */
  int		deblend_nthresh;			/* threshold number */
  double	deblend_mincont;			/* minimum contrast */
  double	satur_level;				/* saturation level */
  enum	{CCD, PHOTO}			detect_type;	/* detection type */
/*----- cleaning */
  int		clean_flag;				/* allow cleaning ? */
  double	clean_param;				/* cleaning effic. */
  int		clean_stacksize;			/* size of buffer */
/*----- photometry */
  enum	{ASCII, FITS}			cat_type;	/* type of catalog */
  enum	{PNONE, TFIXED, AUTO}		apert_type;	/* type of aperture */
  int		apert;					/* apert size (pix) */
  double	kron_fact;				/* Kron factor */
  double	kron_nsig;				/* Kron search rad. */
  double	kron_minsig;				/* minimum radius */
  double	mag_zeropoint;				/* magnitude offset */
  double	mag_gamma;				/* for emulsions */
  double	gain;					/* only for CCD */
/*----- S/G separation */
  double	pixel_scale;				/* in arcsec */
  double	seeing_fwhm;				/* in arcsec */
  char		nnw_name[MAXCHAR];			/* nnw filename */
  char		nnw_rname[MAXCHAR];			/* relative name */
/*----- background */
  enum	{IMAGE, AFILE}			back_origin;	/* origin of bkgnd */
  char		back_name[MAXCHAR];			/* bkgnd filename */
  int		backx, backy;				/* bkgnd mesh size */
  int		backfx, backfy;				/* bkgnd filt. size */
  enum	{GLOBAL, LOCAL}			pback_type;	/* phot. bkgnd type */
  int		pback_size;				/* rect. ann. width */
/*----- memory */
  int		mem_pixstack;				/* pixel stack size */
  int		mem_bufsize;				/* strip height */
/*----- scanning */
  double	scan_isoapratio;			/* iso/apert ratio */
/*----- catalog output */
  char		param_name[MAXCHAR];			/* param. filename */
/*----- miscellaneous */
  int		pipe_flag;				/* allow piping ? */
  enum	{QUIET, NORM, FULL}		verbose_type;	/* display type */
  enum	{CNONE, BACKGROUND, SUBSTRACTED,
	CONVOLVED, OBJECTS, APERTURES,
	SEGMENTATION}
					check_type;	/* check-image type */
  char		check_name[MAXCHAR];			/* check-image name */
	}	prefstruct;


/*------------------------------- catalog  ---------------------------------*/

typedef struct
  {
  FILE		*outfile;				/* FILE ptr */
  char		*fitshead;				/* FITS header ptr*/
  int		fitsheadsize;				/* FITS header size */
  int		ntotal;					/* total object nb */
  int		datablocksize;				/* datablocksize */
  int		nparam;					/* nb of parameters */
  int		*paramnb;				/* parameters order */
  int		*paramsize;				/* parameters size */
  }		catstruct;

void compute_myotherparams(objstruct *obj, float isomag);
