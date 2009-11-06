 /*
 				back.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	functions dealing with background computation.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include    <assert.h>

#include	"define.h"
#include	"globals.h"


/******************************** makeback ***********************************/
/*
A background map is established from the frame itself; though we need to make
at least one first pass through the data.
*/
void	makeback()

  {
   backstruct	*backmesh;
   PIXTYPE	*buf;
   int		i,j,k,m,n, bin, step,
		bufsize, bufsize2, nlines, bufshift, jumpsize, size, meshsize,
		fcurpos, fcurpos2,
		w,bw, nx,ny;
   float	*sig;

  w = field.width;
  bw = field.backw;
  nx = field.nbackx;
  ny = field.nbacky;

/*save current position in file */

  QFTELL(fcurpos, field.file, field.filename);

/*allocate a correct amount of memory to store pixels */

  bufsize = w*field.backh;
  meshsize = bufsize;
  nlines = 0;
  if (bufsize > BACK_BUFSIZE)
    {
    nlines = BACK_BUFSIZE/w;
    step = (field.backh-1)/nlines+1;
    bufsize = (nlines = field.backh/step)*w;
    bufshift = (step/2)*w*sizeof(PIXTYPE);
    jumpsize = (step-1)*w*sizeof(PIXTYPE);
    }

/*allocate some memory */

  for (i=0; i<nx; i++)
    QMALLOC(backmesh, backstruct, nx);		/* background information */
  QMALLOC(buf, PIXTYPE, bufsize);		/* pixel buffer */
  QMALLOC(field.back, float, field.nback);	/* background map */
  QMALLOC(sig, float, field.nback);		/* temporary sigma map */

/*loop over the data packets */

  for (j=0; j<ny; j++)
    {
    if (!nlines)
      {
      if (j == field.nbacky-1 && field.npix%bufsize)
        bufsize = field.npix%bufsize;
      readdata(buf, bufsize);
      backstat(backmesh,buf,bufsize, field.nbackx,field.width,field.backw);
      for (m=0; m<nx; m++)
        QCALLOC(backmesh[m].histo, LONG, backmesh[m].nlevels);
      for (i=0; i<bufsize; i++)
        {
        m = (i%w)/bw;
        bin = (int)((buf[i]-backmesh[m].qzero)/backmesh[m].qscale + 0.5);
        if (bin>=0 && bin<backmesh[m].nlevels)
        backmesh[m].histo[bin]++;
        }
      }
    else
      {
      QFTELL(fcurpos2, field.file, field.filename);

      if (j == field.nbacky-1 && (n=field.height%field.backh))
        {
        meshsize = n*w;
        nlines = BACK_BUFSIZE/w;
        step = (n-1)/nlines+1;
        bufsize = (nlines = n/step)*w;
        bufshift = (step/2)*w*sizeof(PIXTYPE);
        jumpsize = (step-1)*w*sizeof(PIXTYPE);
        free(buf);
        QMALLOC(buf, PIXTYPE, bufsize);		/* pixel buffer */
        }

      QFSEEK(field.file, bufshift, SEEK_CUR, field.filename);

      for (i=0; i<nlines; i++)
        {
        readdata(&buf[w*i], w);
        if (i!=nlines-1)
          QFSEEK(field.file, jumpsize, SEEK_CUR, field.filename);
        }
      backstat(backmesh,buf,bufsize, field.nbackx,field.width,field.backw);
      for (m=0; m<nx; m++)
        QCALLOC(backmesh[m].histo, LONG, backmesh[m].nlevels);
      QFSEEK(field.file, fcurpos2, SEEK_SET, field.filename);
      
      for(size=meshsize, bufsize2=bufsize; size>0; size -= bufsize2)
        {
        if (bufsize2>size)
          bufsize2 = size;
        readdata(buf, bufsize2);
        for (i=0; i<bufsize2; i++)
          {
          m = (i%w)/bw;
          bin = (int)((buf[i]-backmesh[m].qzero)/backmesh[m].qscale + 0.49999);
          if (bin>=0 && bin<backmesh[m].nlevels)
          backmesh[m].histo[bin]++;
          }
        }
      }
      
    for (m=0; m<nx; m++)
      {
      k = m+nx*j;
      backguess(&backmesh[m], &field.back[k], &sig[k]);
      free(backmesh[m].histo);
      }
    }

  field.backsig = hmedian(sig, field.nback);
  if (field.backsig<=0.0)
    {
    warning("Image contains mainly constant data; ",
	"I'll try to cope with that...");
    for (i=0; i<field.nback && sig[i]<=0.0; i++);
    if (i<field.nback)
      field.backsig = hmedian(&sig[i], field.nback-i);
    else
      error(EXIT_SUCCESS, "Constant image - ", "no source found.");
    }

/*free memory */

  free(sig);
  free(buf);
  free(backmesh);

/*go back to the original position */

  QFSEEK(field.file, fcurpos, SEEK_SET, field.filename);
  
  return;
  }


/******************************** backstat **********************************/
/*
compute robust statistical estimators in multiple background meshes.
*/
void	backstat(backstruct *backmesh, PIXTYPE *buf, int bufsize,
			int n, int w, int bw)

  {
   int		i, m;
   double	pix, sig;

  for (m = 0; m < n; m++)
    {
    backmesh[m].mean = backmesh[m].sigma = 0.0;
    backmesh[m].npix = 0;
    }

  for (i = 0; i<bufsize; i++)
    {
    m = (i%w)/bw;
    pix = buf[i];
    backmesh[m].mean += pix;
    backmesh[m].sigma += pix*pix;
    backmesh[m].npix++;
    }

  for (m = 0; m < n; m++)
    {
    backmesh[m].mean /= backmesh[m].npix;
    sig = backmesh[m].sigma/backmesh[m].npix
		- backmesh[m].mean*backmesh[m].mean;
    backmesh[m].sigma = sig>0.0 ? sqrt(sig):0.0;;
    backmesh[m].lcut = backmesh[m].mean-2.0*backmesh[m].sigma;
    backmesh[m].hcut = backmesh[m].mean+2.0*backmesh[m].sigma;
    backmesh[m].mean = backmesh[m].sigma = 0.0;
    backmesh[m].npix = 0;
    }

  for (i = 0; i<bufsize; i++)
    {
    m = (i%w)/bw;
    pix = buf[i];
    if (pix>=backmesh[m].lcut && pix<=backmesh[m].hcut)
      {
      backmesh[m].mean += pix;
      backmesh[m].sigma += pix*pix;
      backmesh[m].npix++;
      }
    }

  pix = sqrt(2/PI)*QUANTIF_NSIGMA/QUANTIF_AMIN;

  for (m = 0; m < n; m++)
    {
    backmesh[m].mean /= backmesh[m].npix;
    sig = backmesh[m].sigma/backmesh[m].npix
		- backmesh[m].mean*backmesh[m].mean;
    backmesh[m].sigma = sig>0.0 ? sqrt(sig):0.0;;
    backmesh[m].nlevels = (int)(pix*backmesh[m].npix+1);
    if (backmesh[m].nlevels>QUANTIF_NMAXLEVELS)
      backmesh[m].nlevels = QUANTIF_NMAXLEVELS;
    backmesh[m].qscale =  backmesh[m].sigma>0.0 ?
		 2*QUANTIF_NSIGMA*backmesh[m].sigma/backmesh[m].nlevels
		:1.0;
    backmesh[m].qzero = backmesh[m].mean - QUANTIF_NSIGMA*backmesh[m].sigma;
    }

  return;
  }


/******************************* filterback *********************************/
/*
Median filtering of the background map to remove the contribution from bright
sources.
*/
void	filterback()

  {
   int		i,p, px,py, np, nx,npx,npy, dpx,dpy, x,y;
   float	*back, *mask, *all;

  nx = field.nbackx;
  np = field.nback;
  npx = field.nbackfx/2;
  npy = nx*(field.nbackfy/2);
  back = field.back;

  if (!(mask = (float *)malloc(field.nbackfx*field.nbackfy*sizeof(float))))
    error (EXIT_FAILURE, "*Error*: not enough memory in ", "filterback()");

  if (!(all = (float *)malloc(np*sizeof(float))))
    error (EXIT_FAILURE, "*Error*: not enough memory in ", "filterback()");

  for (py=0; py<np; py+=nx)
    for (px=0; px<nx; px++)
      {
      i=0;
      for (dpy = -npy; dpy<= npy; dpy+=nx)
        for (dpx = -npx; dpx <= npx; dpx++)
          {
          y = py+dpy;
          x = px+dpx;
          if (y>=0 && y<np && x>=0 && x<nx)
            mask[i++] = back[x+y];
          }
      back[p=px+py] = hmedian(mask, i);
      all[p] = back[p];
      }

  field.backmean = hmedian(all, np);

  free(mask);
  free(all);

  return;
  }


/******************************* backguess **********************************/
/*
Estimate the background from a histogram;
*/
float	backguess(backstruct *bkg, float *mean, float *sigma)

#define	EPS	(1e-4)	/* a small number */

  {
   LONG		*histo;
   double	ftemp, pix, mea, sig, sig1, sum, lowsum, highsum, med;
   int		i, n, lcut,hcut, nlevelsm1, ilow,ihigh;

  histo = bkg->histo;
  hcut = nlevelsm1 = bkg->nlevels-1;
  lcut = 0;

  sig = 10.0*nlevelsm1;
  sig1 = 1.0;
  for (n=0; (n<100) && (sig>=0.1) && (fabs(sig/sig1-1.0)>EPS); n++)
    {
    sig1 = sig;
    sum = mea = sig = lowsum = highsum = 0.0;
    ilow = lcut;
    ihigh = hcut;
    for (i=lcut; i<=hcut; i++)
      {
      if (lowsum<highsum)
        lowsum += histo[ilow++];
      else 
        highsum += histo[ihigh--];
      pix = (double)histo[i];
      sum += pix;
      mea += pix * i;
      sig += pix * i*i;
      }

    med = ihigh+0.5+(highsum-lowsum)/(2.0*(histo[ilow]>histo[ihigh]?
					histo[ilow]:histo[ihigh]));

    if (sum)
      mea /= sum;
    sig = sig/sum - mea*mea;
    sig = sig>0.0?sqrt(sig):0.0;
    lcut = (ftemp=med-3.0*sig)>0.0 ?(int)(ftemp>0.0?ftemp+0.5:ftemp-0.5):0;
    hcut = (ftemp=med+3.0*sig)<nlevelsm1 ?(int)(ftemp>0.0?ftemp+0.5:ftemp-0.5)
								: nlevelsm1;
    }
 *mean = fabs(sig)>0.0? (fabs(bkg->sigma/sig-1) < 0.2 ?
			    bkg->qzero+mea*bkg->qscale
			    :(fabs((mea-med)/sig)< 0.3 ?
			      bkg->qzero+(2.5*med-1.5*mea)*bkg->qscale
			     :bkg->qzero+med*bkg->qscale))
                       :bkg->qzero+mea*bkg->qscale;

  *sigma = sig*bkg->qscale;


  return *mean;
  }


/******************************** localback *********************************/
/*
Compute Local background if possible.
*/
float	localback(objstruct *obj)

  {
   static backstruct	backmesh;
   int			bxmin,bxmax, bymin,bymax, ixmin,ixmax, iymin,iymax,
			bxnml,bynml, oxsize,oysize, npix,
			i, x,y, yw, bin;
   float		bkg;
   PIXTYPE		*backpix, *strip;

  strip = field.strip;

/*estimate background in a 'rectangular annulus' around the object */
  oxsize = obj->xmax - obj->xmin + 1;
  oysize = obj->ymax - obj->ymin + 1;

  if (oxsize<field.width/2)
    bxnml = oxsize/4;
  else
    bxnml = (field.width-oxsize)/4;

  if (oysize<field.height/2)
    bynml = oysize/4;
  else
    bynml = (field.height-oysize)/4;

  ixmin = obj->xmin - bxnml;
  ixmax = obj->xmax+1 + bxnml;
  iymin = obj->ymin - bynml;
  iymax = obj->ymax+1 + bynml;

  bxmin = ixmin - prefs.pback_size;
  bxmax = ixmax + prefs.pback_size;
  bymin = iymin - prefs.pback_size;
  bymax = iymax + prefs.pback_size;

  if (bymin>=field.ymin && bymax<field.ymax
	&& bxmin>=0 && bxmax<field.width)
    {
    npix = (bxmax-bxmin)*(bymax-bymin) - (ixmax-ixmin)*(iymax-iymin);

    QMALLOC(backpix, PIXTYPE, npix);

    i=0;
/*--store all the pixels*/
    for (y=bymin; y<bymax; y++)
      {
      yw = y%field.stripheight * field.width;
      for (x=bxmin; x<ixmin; x++)
        backpix[i++] = strip[yw+x];
      for (x=ixmax; x<bxmax; x++)
        backpix[i++] = strip[yw+x];
      }

    for (y=bymin; y<iymin; y++)
      {
      yw = y%field.stripheight * field.width;
      for (x=ixmin; x<ixmax; x++)
        backpix[i++] = strip[yw+x];
      }
    for (y=iymax; y<bymax; y++)
      {
      yw = y%field.stripheight * field.width;
      for (x=ixmin; x<ixmax; x++)
        backpix[i++] = strip[yw+x];
      }

    backstat(&backmesh, backpix, npix, 1, 1, 1);
    QCALLOC(backmesh.histo, LONG, backmesh.nlevels);
    for (i=0; i<npix; i++)
      {
      bin = (int)((backpix[i]-backmesh.qzero)/backmesh.qscale + 0.5);
      if (bin>=0 && bin<backmesh.nlevels)
      backmesh.histo[bin]++;
      }
    free(backpix);
    backguess(&backmesh, &bkg, &obj->sigbkg);
    obj->bkg += (obj->dbkg = bkg);
    free(backmesh.histo);
    }
  else
    {
    obj->dbkg = bkg = 0.0;
    obj->sigbkg = field.backsig;
    }

  return bkg;
  }

/************************************ back ***********************************/
/*
return background at position x,y (linear interpolation between background
map vertices).
*/
PIXTYPE	back(int x, int y)

  {
   int		nx,ny, xl,yl, pos;
   double	dx,dy, cdx;
   float	*b, b0,b1,b2,b3;

  b = field.back;
  nx = field.nbackx;
  ny = field.nbacky;

  dx = (double)x/field.backw - 0.5;
  dy = (double)y/field.backh - 0.5;
  dx -= (xl = (int)dx);
  dy -= (yl = (int)dy);

  if (xl<0)
    {
    xl = 0;
    dx -= 1.0;
    }
  else if (xl>=nx-1)
    {
    xl = nx<2 ? 0 : nx-2;
    dx += 1.0;
    }

  if (yl<0)
    {
    yl = 0;
    dy -= 1.0;
    }
  else if (yl>=ny-1)
    {
    yl = ny<2 ? 0 : ny-2;
    dy += 1.0;
    }

  pos = yl*nx + xl;
  cdx = 1 - dx;

  b0 = b[pos];			/* consider when nbackx or nbacky = 1 */
  b1 = nx<2? b0:b[++pos];
  b2 = ny<2? b[pos]:b[pos+=nx];
  b3 = nx<2? b[pos]:b[--pos];

  return (PIXTYPE)((1-dy)*(cdx*b0 + dx*b1) + dy*(dx*b2 + cdx*b3));
  }

