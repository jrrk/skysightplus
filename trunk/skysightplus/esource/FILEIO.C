/*
 				fileio.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, Institut d'Astrophysique de Paris.
*
*	Contents:	functions for input/output of image data.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<assert.h>

#include	"define.h"
#include	"globals.h"

#define     BSWAP 1

/******************************** fitsnfind **********************************/
/*
search for a FITS keyword in a fits header of nblock blocks.
*/
char	*fitsnfind(char *fitsbuf, char *str, int nblock)
  {
   int	i;

  for (i=0;i<36*nblock;i++)
    if (!strncmp(&fitsbuf[80*i], str, strlen(str)))
      return &fitsbuf[80*i];

  return	(char *)NULL;
  }


/******************************* loadstrip ***********************************/
/*
Load a new strip of pixel data into the buffer.
*/
PIXTYPE	*loadstrip()

  {
   int	i, w;
   PIXTYPE	*fs;

  w = field.width;

  if (!field.y)

    {
/*- First strip */

     int	nbpix;

    fs = field.strip;
    nbpix = w*field.stripheight;

    readdata(fs, nbpix);

/*----------------------------------Check-image-----------------------------*/
    if (prefs.check_type!=CNONE)
      {
      PIXTYPE	backt, subt, *fcbmp;
      switch (prefs.check_type) 
        {
        case BACKGROUND:	for (i=0; i<nbpix; i++)
				  {
				  backt = (PIXTYPE)back(i%w, i/w);
				  *(fs++) -= (PIXTYPE)backt;
#				  ifdef BSWAP
				    swapbytes(&backt, sizeof(float), 1);
#				  endif
				  QFWRITE(&backt, sizeof(float),
					field.check_file, field.check_name);
				  }
				break;

        case SUBSTRACTED:	for (i=0; i<nbpix; i++)
				  {
				  subt = (*(fs++)
					-= (PIXTYPE)back(i%w, i/w));
#				  ifdef BSWAP
				    swapbytes(&subt, sizeof(float), 1);
#				  endif
				  QFWRITE(&subt, sizeof(PIXTYPE),
					field.check_file, field.check_name);
				  }
				break;

	case APERTURES:		fcbmp = (PIXTYPE *)field.check_bmp;
				for (i=0; i<nbpix; i++)
				  *(fcbmp++) += (*(fs++)
					-= (PIXTYPE)back(i%w, i/w));
				break;
	default:		for (i=0; i<nbpix; i++)
				  *(fs++) -= (PIXTYPE)back(i%w, i/w);
				break;
        }

      }
/*--------------------------------------------------------------------------*/
    else
      for (i=0; i<nbpix; i++)
        *(fs++) -= (PIXTYPE)back(i%w, i/w);

    field.ymax = field.stripheight;
    if (field.ymax < field.height)
      field.stripysclim = field.stripheight - field.stripmargin;
    }

  else

    {
/*- other strips */
    fs = &field.strip[field.stripylim*field.width];

    readdata(fs, w);

/*----------------------------------Check-image-----------------------------*/
    if (prefs.check_type!=CNONE)
      {
      PIXTYPE	backt, subt, *fcbmp;
      switch (prefs.check_type) 
        {
        case BACKGROUND:	for (i=0; i<w; i++)
				  {
				  backt = (PIXTYPE)back(i,field.ymax);
				  *(fs++) -= (PIXTYPE)backt;
#				  ifdef BSWAP
				    swapbytes(&backt, sizeof(float), 1);
#				  endif
				  QFWRITE(&backt, sizeof(float),
					field.check_file, field.check_name);
				  }
				break;

        case SUBSTRACTED:	for (i=0; i<w; i++)
				  {
				  subt = (*(fs++)
				    -= (PIXTYPE)back(i, field.ymax));
#				  ifdef BSWAP
				    swapbytes(&subt, sizeof(float), 1);
#				  endif
				  QFWRITE(&subt, sizeof(PIXTYPE),
					field.check_file, field.check_name);
				  }
				break;
	case APERTURES:		fcbmp =
				&((PIXTYPE *)field.check_bmp)[field.ymax*w];
				for (i=0; i<w; i++)
				  *(fcbmp++) += (*(fs++)
				      -= (PIXTYPE)back(i,field.ymax));
				break;
	default:		for (i=0; i<w; i++)
			          *(fs++)-=(PIXTYPE)back(i,field.ymax);
				break;
        }

      }
/*--------------------------------------------------------------------------*/
    else
      for (i=0; i<w; i++)
        *(fs++) -= (PIXTYPE)back(i, field.ymax);

    field.stripylim = (++field.ymin)%field.stripheight;
    if ((++field.ymax)<field.height)
      field.stripysclim = (++field.stripysclim)%field.stripheight;
    }

  return &field.strip[field.stripy*field.width];
  }

/******************************** readdata **********************************/
/*
read and convert input data stream in PIXTYPE (float) format.
*/
void	readdata(PIXTYPE *ptr, int size)
  {
  int		i, bowl, spoonful;


  bowl = DATA_BUFSIZE/field.bytepix;
  spoonful = size<bowl?size:bowl;
  for(; size>0; size -= spoonful, ptr += spoonful)
    {
    if (spoonful>size)
      spoonful = size;
    QFREAD(bufdata, spoonful*field.bytepix, field.file, field.filename);
    switch(field.bitpix)
      {
      case BP_BYTE:	for (i=0; i<spoonful; i++)
			  ptr[i] = bufdata[i]*field.bscale+field.bzero;
			break;

      case BP_SHORT:
#			ifdef BSWAP
			  swapbytes(bufdata, 2, spoonful);
#			endif

			for (i=0; i<spoonful; i++)
			  ptr[i] = ((short *)bufdata)[i]*field.bscale
					+field.bzero;
			break;

      case BP_LONG:
#			ifdef BSWAP
			  swapbytes(bufdata, 4, spoonful);
#			endif

			for (i=0; i<spoonful; i++)
			  ptr[i] = ((int *)bufdata)[i]*field.bscale
					+field.bzero;
			break;

      case BP_FLOAT:
#			ifdef BSWAP
			  swapbytes(bufdata, 4, spoonful);
#			endif
			for (i=0; i<spoonful; i++)
			  ptr[i] = ((float *)bufdata)[i]*field.bscale
					+field.bzero;
			break;
      case BP_DOUBLE:
#			ifdef BSWAP
			  swapbytes(bufdata, 8, spoonful);
#			endif
			for (i=0; i<spoonful; i++)
			  ptr[i] = ((double *)bufdata)[i]*field.bscale
					+field.bzero;
			break;

      default:		error(EXIT_FAILURE,
				"*FATAL ERROR*: unknown BITPIX type in ",
				"readdata()");
			break;
      }
    }

  return;
  }


/****************************** readimagehead *******************************/
/*
extract some data from the FITS-file header
*/
void	readimagehead()
  {
   int		i, n;
   char		*buf, st[80], *point;

/*Open the file */
 
  if (!(field.file = fopen(field.filename, "rb")))
    error(EXIT_FAILURE,"*Error*: cannot open ", field.filename);

  buf = readfitshead(field.file, field.filename, &n);
  if(FITSTOI("NAXIS   ", 0) < 2)
    error(EXIT_FAILURE, field.filename, " does NOT contain 2D-data!");

  field.bitpix = FITSTOI("BITPIX  ", 0);
  if (field.bitpix != BP_BYTE
	&& field.bitpix != BP_SHORT
	&& field.bitpix != BP_LONG
	&& field.bitpix != BP_FLOAT
	&& field.bitpix != BP_DOUBLE)
    error(EXIT_FAILURE, "Sorry, I don't know that kind of data.", "");

  field.bytepix = (field.bitpix>0?field.bitpix:-field.bitpix)>>3;
  field.width = FITSTOI("NAXIS1  ", 0);
  field.height = FITSTOI("NAXIS2  ", 0);
  field.npix = field.width*field.height;

#ifdef MAMA
  field.bscale = FITSTOF("SCALE   ", 1.0);	/* MAMAspecific */
#else
  field.bscale = FITSTOF("BSCALE  ", 1.0);
#endif

  field.bzero = FITSTOF("BZERO   ", 0.0);

  field.epoch = FITSTOF("EPOCH   ", 2000.0);

  FITSTOS("OBJECT  ", field.ident, "Unnamed");

  field.crpixx = FITSTOI("CRPIX1  ", field.width/2+1);
  field.crpixy = FITSTOI("CRPIX2  ", field.height/2+1);
  field.crvalx = FITSTOF("CRVAL1  ", 0.0);
  field.crvaly = FITSTOF("CRVAL2  ", 0.0);
  field.cdeltx = FITSTOF("CDELT1  ", 1.0);
  field.cdelty = FITSTOF("CDELT2  ", 1.0);
  field.crotax = FITSTOF("CROTA1  ", 0.0);
  field.crotay = FITSTOF("CROTA2  ", 0.0);
#ifdef MAMA
  field.crotax = field.crotay;			/* MAMAspecific */
#endif

/*------------------ MAMAspecific EB 02/02/95 ---------------------------------*/

#ifdef MAMA
  if (point = fitsnfind(buf,
	"COMMENT   'MAMA ASTROMETRIC COEFFICIENTS FOR XY->AD  II'", n))
    {
     mapstruct	*map = &field.map;

    map->epoch = field.epoch;
    map->crpixx = field.crpixx-1;
    map->crpixy = field.crpixy-1;
    map->x = field.crvalx;
    map->y = field.crvaly;
    map->pdx = field.cdeltx;
    map->pdy = field.cdelty;

    sscanf(&point[80], "HISTORY OBX = %lf OBY = %lf", &map->x, &map->y);
    sscanf(&point[80*2], "HISTORY PDX = %lf PDY = %lf", &map->pdx, &map->pdy);
    sscanf(&point[80*3], "HISTORY ALC = %lf DEC = %lf",
	&map->alphacent, &map->deltacent);
    sscanf(&point[80*4], "HISTORY FOC = %lf EQU = %lf",
	&map->foc, &map->equinox);
    sscanf(&point[80*5], "HISTORY XZC = %lf YZC = %lf",
	&map->xcent, &map->ycent);

    for (i=0; i<10; i+=2)
      {
      sprintf(st, "HISTORY DX%1d = %%lf DX%1d = %%lf",i,i+1);
      sscanf(&point[80*(6+i/2)], st, &map->dx[i], &map->dx[i+1]);
      }
    for (i=0; i<10; i+=2)
      {
      sprintf(st, "HISTORY DY%1d = %%lf DY%1d = %%lf",i,i+1);
      sscanf(&point[80*(11+i/2)], st, &map->dy[i], &map->dy[i+1]);
      }
    }
  else
    warning("No appropriate Astrometric info found in ", field.filename);
#endif

/*-----------------------------------------------------------------------------*/

  if (prefs.check_type==CNONE)
    myfree(buf);
  else
    {
    field.fitshead = buf;
    field.fitsheadsize = n*FBSIZE;
    }

  return;
  }

/****************************** readfitshead ********************************/
/*
read data from the FITS-file header
*/
char    *readfitshead(FILE *file, char *filename, int *nblock)

  {
   int     n;
   char    *buf;

  if (!(buf=(char *)myalloc((size_t)FBSIZE)))
    error(EXIT_FAILURE, "*Error*: Not enough memory in readfitshead()");

/*Find the number of FITS blocks of the header while reading it */
  QFREAD(buf, FBSIZE, file, filename);

  if (strncmp(buf, "SIMPLE  ", 8))
    error(EXIT_FAILURE, "Error %s is NOT a FITS file!", filename);

  for (n=1; !fitsnfind(buf,"END     ", n); n++)
    {
    if (!(buf=(char *)myrealloc(buf, (size_t)(FBSIZE*(n+1)))))
      error(EXIT_FAILURE, "*Error*: Not enough memory in readfitshead()");
    QFREAD(&buf[FBSIZE*n], FBSIZE, file, filename);
    }

  *nblock = n;
  return  buf;
  }


/******************************* initcheck **********************************/
/*
initialize check-image.
*/
void	initcheck()

  {
   int		ival;
   double	dval;
   USHORT	*ptri;
   PIXTYPE	*ptrf;

  if (!(field.check_file = fopen(field.check_name, "wb")))
    error(EXIT_FAILURE, "*Error*: Cannot open for output ",field.check_name);

  dval = 1.0;fitswrite(field.fitshead, "BSCALE  ", &dval, H_FLOAT, T_DOUBLE);
  dval = 0.0;fitswrite(field.fitshead, "BZERO   ", &dval, H_FLOAT, T_DOUBLE);
  fitswrite(field.fitshead, "ORIGIN  ", BANNER, H_STRING, 0);

  switch(prefs.check_type)
    {
    case BACKGROUND:
    case CONVOLVED:
    case SUBSTRACTED:	ival = -32;
			fitswrite(field.fitshead, "BITPIX  ", &ival, H_INT,
				T_LONG);
			break;
    case OBJECTS:
    case APERTURES:	ival = -32;
			fitswrite(field.fitshead, "BITPIX  ", &ival, H_INT,
				T_LONG);
			QCALLOC(ptrf, PIXTYPE, field.npix);
			field.check_bmp = (void *)ptrf;
			break;
    case SEGMENTATION:	ival = 16;
			fitswrite(field.fitshead, "BITPIX  ", &ival, H_INT,
				T_LONG);
			QCALLOC(ptri, USHORT, field.npix);
			field.check_bmp = (void *)ptri;
			break;
    default:		error(EXIT_FAILURE, "*Internal Error* in ",
				"initcheck()!");
    }

  QFWRITE(field.fitshead, field.fitsheadsize, field.check_file,
	field.check_name);

  myfree(field.fitshead);

  return;
  }


/******************************* closecheck *********************************/
/*
close check-image.
*/
void	closecheck()
  {
   char	buf[FBSIZE];
   int	padsize;

  memset(buf, 0, FBSIZE*sizeof(char));
  switch(prefs.check_type)
    {
    case BACKGROUND:
    case CONVOLVED:
    case SUBSTRACTED:	padsize = (FBSIZE
			-((field.npix*sizeof(PIXTYPE))%FBSIZE))% FBSIZE;

			break;
    case OBJECTS:
    case APERTURES:	padsize = (FBSIZE
			-((field.npix*sizeof(PIXTYPE))%FBSIZE))% FBSIZE;
#			ifdef BSWAP
			  swapbytes(field.check_bmp, sizeof(PIXTYPE),
				field.npix);
#			endif
			QFWRITE(field.check_bmp, field.npix*sizeof(PIXTYPE),
				field.check_file, field.check_name);
			myfree(field.check_bmp);
			break;

    case SEGMENTATION:	padsize = (FBSIZE
			-((field.npix*sizeof(USHORT))%FBSIZE))% FBSIZE;
#			ifdef BSWAP
			  swapbytes(field.check_bmp, sizeof(USHORT),
				field.npix);
#			endif
			QFWRITE(field.check_bmp, field.npix*sizeof(USHORT),
				field.check_file, field.check_name);
			myfree(field.check_bmp);
			break;
    default:		error(EXIT_FAILURE, "*Internal Error* in ",
				"closecheck()!");
    }

  if (padsize)
    QFWRITE (buf, padsize, field.check_file, prefs.check_name);
  fclose(field.check_file);

  return;
  }

