/*
 				catout.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	functions for output of catalog data.
*
*	Last modify:	31/10/94
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"define.h"
#include	"globals.h"
#include	"param.h"

#define     BSAWP 1

#define		NBHEAD	2	/* size (in FITS blocks) of header */


extern	keystruct	key[];
extern	char		keylist[][16];

char	*fbuf, *fmtstr;
LONG	catpos;

/******************************* readcatparams *******************************/
/*
Read the catalog config file
*/
void	readcatparams(char *filename)
  {
   FILE	*infile;
   char str[MAXCHAR], keyword[MAXCHAR], *sstr;
   int	i, nkey;

  if ((infile = fopen(filename,"r")) == NULL)
    error(EXIT_FAILURE, "*ERROR*: can't read ", filename);

/*Build the keyword-list from paramstruct-array */

  for (i=0; param[i].name[0]; i++)
    strcpy(paramlist[i], param[i].name);
  paramlist[i][0] = '\0';

/*Scan the catalog config file*/

  QMALLOC(cat.paramnb, int, sizeof(param)/sizeof(paramstruct));
  QMALLOC(cat.paramsize, int, sizeof(param)/sizeof(paramstruct));

  for (i=0; fgets(str, MAXCHAR, infile);)
    {
    sstr = &str[(int)strspn(str," \t")];
    if (sstr[0]!=(char)'#' && sstr[0]!=(char)'\n')
      {
      sscanf(sstr, "%s", keyword);
      nkey = findkey(keyword, paramlist);
      if (nkey!=RETURN_ERROR)
        {
        cat.paramnb[i] = nkey;
        cat.paramsize[i++] = t_size[param[nkey].t_type];
        *((char *)param[nkey].ptr) = (char)'\1';
        }
      else
        warning(keyword, " catalog parameter unknown");
      }
    }

  fclose(infile);

  cat.nparam = i;

/*Now we copy the flags to the proper structures */

  flagobj = outobj;
  flagobj2 = outobj2;
  updateparamflags();

  return;
  }


/***************************** updateparamflags ******************************/
/*
Update parameter flags according to their mutual dependancies.
*/
void	updateparamflags()

  {
  FLAG(obj2.mag) |= FLAG(obj2.sigmag);
  FLAG(obj.automag) |= FLAG(obj2.mag);	/*if MAG_BEST used, we need MAG_AUTO*/
  FLAG(obj2.isocormag) |= FLAG(obj2.mag);		/* and MAG_ISOCOR */
  FLAG(obj2.isocormag) |= FLAG(obj2.sigmaisocormag);
  FLAG(obj2.sigmaisocormag) |= FLAG(obj2.isocormag);
  FLAG(obj.automag) |= FLAG(obj.sigmaautomag);
  FLAG(obj.automag) |= FLAG(obj.kronfactor);
  FLAG(obj.apermag) |= FLAG(obj.sigmaapermag);
  FLAG(obj.fwhm) |= FLAG(obj2.fwhmw);

  FLAG(obj2.alpha) |= FLAG(obj2.delta);		/* MAMAspecific 02/02/95 */

  return; 
  }


/********************************** initcat **********************************/
/*
Initialize the catalog header
*/
void	initcat()
  {

/*Open the file */
  if (prefs.pipe_flag)
    cat.outfile = stdout;
  else
    {
    if (prefs.cat_type == FITS)
      {
      if (!(cat.outfile = fopen(prefs.cat_name, "w+")))
        error(EXIT_FAILURE,"*Error*: cannot open ", prefs.cat_name);
      }
    else
      {
      if (!(cat.outfile = fopen(prefs.cat_name, "w+")))
        error(EXIT_FAILURE,"*Error*: cannot open ", prefs.cat_name);
      }
    }

  if (prefs.cat_type == FITS)
    {
    QMALLOC(fbuf, char, 8*cat.nparam);
    initfitscat();
    }
  else
    QMALLOC(fmtstr, char, 20*cat.nparam);

  return;
  }


/******************************** initfitscat ********************************/
/*
Initialize the FITS catalog header
*/
void	initfitscat()
  {
   static char	bufmodel[NBHEAD*36][80] = {
	"SIMPLE  =                    T / THIS IS REGULAR FITS",
	"BITPIX  =                    8 /",
	"NAXIS   =                    0 /",
	"EXTEND  =                    T / A BINARY TABLE FOLLOWS",
	"EPOCH   =                  0.0 /",
	"OBJECT  = 'Unknown'            /",
	"ORIGIN  = 'SExtractor'         /",
	"CRVAL1  =                    0 / WORLD X COORD. OF REFERENCE PIXEL",
	"CRVAL2  =                    0 / WORLD Y COORD. OF REFERENCE PIXEL",
	"CRPIX1  =                    0 / IMAGE X COORD. OF REFERENCE PIXEL",
	"CRPIX2  =                    0 / IMAGE Y COORD. OF REFERENCE PIXEL",
	"CDELT1  =                    0 / WORLD PIXEL STEP ALONG X",
	"CDELT2  =                    0 / WORLD PIXEL STEP ALONG Y",
	"CROTA1  =                    0 / CCW ANGLE FROM X-IMAGE TO X-WORLD",
	"CROTA2  =                    0 / CCW ANGLE FROM Y-IMAGE TO Y-WORLD",
	"SEXIMASX=                    0 / IMAGE WIDTH (PIXELS)",
	"SEXIMASY=                    0 / IMAGE HEIGHT (PIXELS)",
	"SEXSTRSY=                    0 / STRIP HEIGHT (LINES)",
	"SEXIMABP=                    0 / FITS IMAGE BITPIX",
	"SEXPIXS =                  1.0 / PIXEL SCALE (ARCSEC)",
	"SEXSFWHM=                  1.0 / SEEING FWHM (ARCSEC)",
	"SEXNNWF = 'Default.nnw'        / CLASSIFICATION NNW FILENAME",
	"SEXGAIN =                  0.0 / GAIN (IN E- PER ADU)",
	"SEXBKGND=                  0.0 / MEDIAN BACKGROUND (ADU)",
	"SEXBKDEV=                  0.0 / MEDIAN RMS (ADU)",
	"SEXBKTHD=                  0.0 / EXTRACTION THRESHOLD (ADU)",
	"SEXCONFF= 'Default.sex'        / CONFIGURATION FILENAME",
	"SEXDETT = 'CCD'                / DETECTION TYPE",
	"SEXTHLDT= 'SIGMA'              / THRESHOLD TYPE",
        "SEXTHLD =                  0.0 / THRESHOLD",
        "SEXMINAR=                    0 / EXTRACTION MINIMUM AREA (PIXELS)",
	"SEXCONV =                    F / CONVOLUTION FLAG",
	"SEXCONVN=                    F / CONVOLUTION NORM. FLAG",
	"SEXCONVF= 'Default.conv'       / CONVOLUTION FILENAME",
        "SEXDBLDN=                    0 / NUMBER OF SUB-THRESHOLDS",
        "SEXDBLDC=                  0.0 / CONTRAST PARAMETER",
	"SEXCLN  =                    F / CLEANING FLAG",
	"SEXCLNPA=                  0.0 / CLEANING PARAMETER",
	"SEXCLNST=                    0 / CLEANING OBJECT-STACK",
        "SEXAPERD=                    0 / APERTURE DIAMETER (PIXELS)",
        "SEXAPEK1=                  0.0 / KRON PARAMETER",
        "SEXAPEK2=                  0.0 / KRON ANALYSIS RADIUS",
        "SEXAPEK3=                  0.0 / KRON MINIMUM RADIUS",
        "SEXSATLV=                  0.0 / SATURATION LEVEL (ADU)",
        "SEXMGZPT=                  0.0 / MAGNITUDE ZERO-POINT",
        "SEXMGGAM=                  0.0 / MAGNITUDE GAMMA",
	"SEXBKGSX=                    0 / BACKGROUND MESH WIDTH (PIXELS)",
	"SEXBKGSY=                    0 / BACKGROUND MESH HEIGHT (PIXELS)",
	"SEXBKGFX=                    0 / BACKGROUND FILTER WIDTH",
	"SEXBKGFY=                    0 / BACKGROUND FILTER HEIGHT",
	"SEXPBKGT= 'GLOBAL'             / PHOTOM BACKGROUND TYPE",
	"SEXPBKGS=                    0 / LOCAL AREA THICKNESS (PIXELS)",
	"SEXPIXSK=                    0 / PIXEL STACKSIZE (PIXELS)",
	"SEXFBUFS=                    0 / FRAME-BUFFER SIZE (LINES)",
	"SEXISAPR=                    0 / ISO-APER RATIO",
	"SEXNDET =                    0 / NB OF DETECTIONS",
	"SEXNFIN =                    0 / NB OF FINAL EXTRACTED OBJECTS",
	"SEXNPARA=                    0 / NB OF PARAMETERS PER OBJECT",
	"END                             "};

   static char	bufxtndmodel[36][80] = {
	"XTENSION= 'BINTABLE'           / THIS IS A BINARY TABLE",
	"BITPIX  =                    8 /",
	"NAXIS   =                    2 /",
	"NAXIS1  =                 1024 /",
	"NAXIS2  =                    0 / NUMBER OF CATALOG ENTRIES",
	"PCOUNT  =                    0 /",
	"GCOUNT  =                    1 /",
	"TFIELDS =                    0 /",
	"END                             "};


  int	i,j,  bufsize, pos, nbytes;
  char	*buf, *bufxtnd, *str, strk[80], strc[80];

  QMALLOC(buf, char, sizeof(bufmodel));
  str = (char *)bufmodel;
  for (i=0; i< sizeof(bufmodel); i++)
    if (str[i])
      buf[i] = str[i];
    else
      buf[i] = ' ';

  nbytes = FBSIZE*(2+cat.nparam/18);	/* 2 lines per parameter */
  QMALLOC(bufxtnd, char, nbytes);
  for (i=0; i<nbytes; i++)
    bufxtnd[i] = ' ';
  str = (char *)bufxtndmodel;
  for (i=0; i< sizeof(bufxtndmodel); i++)
    if (str[i])
      bufxtnd[i] = str[i];


  for (i=0; hparam[i].name[0]; i++)
    {
    if (hparam[i].headtype != H_KEY)
      fitswrite(buf, hparam[i].name, hparam[i].ptr, hparam[i].headtype,
		hparam[i].t_type);
    else
      {
      for (j=0; key[j].ptr != hparam[i].ptr && key[j].name[0]; j++);
      if (key[j].name[0])
        fitswrite(buf, hparam[i].name, key[j].keylist[*(int *)key[j].ptr],
		H_STRING, 0);
      else
        error (EXIT_FAILURE, "*Internal Error*: Unknown Keyword in ",
		"initfitscat()");
      }
    }

  catpos = 0;
  pos = 0;
  for (i=0; i<cat.nparam; i++)
    {
    j = cat.paramnb[i];
    sprintf(strk, "TTYPE%-3d", i+1);
    sprintf(strc, "CATALOG PARAMETER #%d", i+1);
    fitsadd(bufxtnd, strk, strc);
    fitswrite(bufxtnd, strk, param[j].name, H_STRING,0);
    pos += t_size[param[j].t_type];

    sprintf(strk, "TFORM%-3d", i+1);
    sprintf(strc, "PARAMETER TYPE");
    fitsadd(bufxtnd, strk, strc);
    fitswrite(bufxtnd, strk, t_form[param[j].t_type], H_STRING, 0);
    }

  fitswrite(bufxtnd, "TFIELDS ", &cat.nparam, H_INT, BP_LONG);
  fitswrite(bufxtnd, "NAXIS1  ", &pos, H_INT, BP_LONG);
  QFWRITE(buf, sizeof(bufmodel), cat.outfile, prefs.cat_name);
  bufsize = FBSIZE*(1+fitsfind(bufxtnd, "END     ")/36);
  QFWRITE(bufxtnd, bufsize, cat.outfile, prefs.cat_name);

  free(buf);
  free(bufxtnd);

  return;
  }

/********************************* writecat **********************************/
/*
Write out in the catalog each one object.
*/
void	writecat(int n, objliststruct *objlist)
  {
   char		str[20];
   int		i,j, nb, pos, size;

  outobj = objlist->obj[n];

  if (prefs.cat_type == FITS)
    {
    *(LONG *)fbuf = ++catpos;
#   ifdef BSWAP
      swapbytes(fbuf, 4, 1);
#   endif

    pos = 0;
    for (i=0; i<cat.nparam; i++)
      {
      nb = cat.paramnb[i];
      size = cat.paramsize[i];
      for (j=0; j<size; j++)
        fbuf[pos++] = ((char *)param[nb].ptr)[j];
#     ifdef BSWAP
        swapbytes(&fbuf[pos-size], size, 1);
#     endif
      }

    QFWRITE(fbuf, pos, cat.outfile, prefs.cat_name);
    }
  else
    {
    fmtstr[0] = '\0';
    for (i=0; i<cat.nparam; i++)
      {
      nb = cat.paramnb[i];
      switch(param[nb].t_type)
        {
        case T_BYTE:	sprintf(str, param[nb].format,
				*(char *)param[nb].ptr);
			break;
        case T_SHORT:	sprintf(str, param[nb].format,
				*(short *)param[nb].ptr);
			break;
        case T_LONG:	sprintf(str, param[nb].format,
				*(LONG *)param[nb].ptr);
			break;
        case T_FLOAT:	sprintf(str, param[nb].format,
				*(float *)param[nb].ptr);
			break;
        case T_DOUBLE:	sprintf(str, param[nb].format,
				*(double *)param[nb].ptr);
			break;
        default:	error (EXIT_FAILURE,
				"*Fatal Error*: unknown BITPIX type in ",
				"writeout()");
        }
      strcat(fmtstr, str);
      }
    fmtstr[strlen(fmtstr)-1] = '\n';
    if (fputs(fmtstr, cat.outfile) == EOF)
      error (EXIT_FAILURE, "*Error*: Cannot write ", prefs.cat_name);
    }

  return;
  }


/********************************* closecat **********************************/
/*
Close the output catalog.
*/
void	closecat()
  {

  if (prefs.cat_type == FITS)
    {
     char	*buf;
     int	i, catsize, padsize, nbytes;

    cat.ntotal = field.nfinal;
    nbytes = (NBHEAD+1)*FBSIZE;
    QCALLOC(buf, char, nbytes+80);
    sprintf(&buf[nbytes], "END     ");
    catsize = 0;
    for (i=0; i<cat.nparam; i++)
      catsize += cat.paramsize[i];
    catsize *= cat.ntotal;
    padsize = (FBSIZE-(catsize%FBSIZE)) % FBSIZE;
    if (padsize)
      QFWRITE (buf, padsize, cat.outfile, prefs.cat_name);

    if (!prefs.pipe_flag)
      {
      
      QFSEEK(cat.outfile, 0, SEEK_SET, prefs.cat_name);
      QFREAD (buf, nbytes, cat.outfile, prefs.cat_name);
      fitswrite(buf, "SEXNDET ", &field.ndetect, H_INT, BP_LONG);
      fitswrite(buf, "SEXNFIN ", &field.nfinal, H_INT, BP_LONG);
      fitswrite(&buf[NBHEAD*FBSIZE], "NAXIS2  ", &cat.ntotal, H_INT, BP_LONG);
      QFSEEK(cat.outfile, 0, SEEK_SET, prefs.cat_name);
      QFWRITE (buf, nbytes, cat.outfile, prefs.cat_name);
      }

    free(buf);
    QFREE(fbuf);
    }
  else
    QFREE(fmtstr);

  QFREE(cat.paramnb);
  QFREE(cat.paramsize);

  if (!prefs.pipe_flag)
    fclose(cat.outfile);

  return;
  }

