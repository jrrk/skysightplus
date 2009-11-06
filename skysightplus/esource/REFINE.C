 /*
 				refine.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO
*
*	Contents:	functions to refine extraction of objects.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include    <stdio.h>
#include	<math.h>
#include	<stdlib.h>
#include	<assert.h>

#include	"define.h"
#include	"globals.h"

#ifndef	RAND_MAX
#define	RAND_MAX	2147483647
#endif
#define MINSUBAREA		3	/* max. area for detection */
#define	NSONMAX			1024	/* max. number per level */
#define	NBRANCH			16	/* starting number per branch */

static objliststruct	*objlist = NULL;
static short		*son = NULL, *ok = NULL;

/******************************** parcelout **********************************/
/*
Divide merged objects into several parts.
*/
int	parcelout(objliststruct *objlistin, objliststruct *objlistout)

  {
   objstruct		*obj;
   objliststruct	tobjlist, tobjlist2;
   double		dthresh, value0;
   int			h,i,j,k,l,m,
			minsubarea,
			xn,
			nbm = NBRANCH,
			out;


  out = RETURN_OK;

  xn = prefs.deblend_nthresh;

/*----- set the area limit for sub-detection */

  minsubarea = prefs.ext_minarea<MINSUBAREA ? prefs.ext_minarea:MINSUBAREA;

/*----- allocate some memory */

  if (!son)
    if (!(son = (short *)malloc(xn*NSONMAX*nbm*sizeof(short))))
      error(EXIT_FAILURE, "Not enough memory in ", "parcelout()");
  if (!ok)
    if (!(ok = (short *)malloc(xn*NSONMAX*sizeof(short))))
      error(EXIT_FAILURE, "Not enough memory in ", "parcelout()");

  if (!objlist)
    if (!(objlist = (objliststruct *)malloc(xn*sizeof(objliststruct))))
      error(EXIT_FAILURE, "Not enough memory in ", "parcelout()");

/* ---- initialize lists of objects */

  tobjlist.obj = tobjlist2.obj =  NULL;
  tobjlist.plist = tobjlist2.plist = NULL;
  tobjlist.nobj = tobjlist2.nobj = 0;
  tobjlist.npix = tobjlist2.npix = 0;
  objlistout->cthresh = tobjlist2.cthresh = objlistin->cthresh;
  for (k=0; k<xn; k++)
    {
    objlist[k].obj = NULL;
    objlist[k].plist = NULL;
    objlist[k].nobj = objlist[k].npix = 0;
    }

  for (l=0; l<objlistin->nobj && out==RETURN_OK; l++)
      {
      if ((out = addobj(l, objlistin, &objlist[0])) == RETURN_FATAL_ERROR)
        goto exit_parcelout;
      if ((out = addobj(l, objlistin, &tobjlist2)) == RETURN_FATAL_ERROR)
        goto exit_parcelout;
      value0 = objlist[0].obj[0].rawvalue*prefs.deblend_mincont;
      ok[0] = (short)1;
      for (k=1; k<xn; k++)
        {
/*--------- Calculate threshold */
        dthresh = objlistin->obj[l].maxflux; 
        if (dthresh>0.0)
          {
          if (prefs.detect_type == PHOTO)
            tobjlist.cthresh = objlistin->cthresh+(dthresh-objlistin->cthresh)
			* (double)k/xn;
          else
            tobjlist.cthresh = objlistin->cthresh
		* pow(dthresh/objlistin->cthresh, (double)k/xn);
          }
        else
          tobjlist.cthresh = objlistin->cthresh;

/*--------- Build tree (bottom->up) */
        if (objlist[k-1].nobj>=NSONMAX)
          {
          out = RETURN_FATAL_ERROR;
          goto exit_parcelout;
          }

        for (i=0; i<objlist[k-1].nobj; i++)
          {
          if ((out=lutz((int)objlist[k-1].obj[i].xmin,
			(int)objlist[k-1].obj[i].xmax,
			(int)objlist[k-1].obj[i].ymin,
			(int)objlist[k-1].obj[i].ymax,
			minsubarea, &tobjlist)) == RETURN_FATAL_ERROR)
            goto exit_parcelout;

          for (j=h=0; j<tobjlist.nobj; j++)
            if (belong(j, &tobjlist, i, &objlist[k-1]))
              {
              tobjlist.obj[j].thresh = tobjlist.cthresh;
              m = addobj(j, &tobjlist, &objlist[k]);
              if (m==RETURN_FATAL_ERROR || m>=NSONMAX)
                {
                out = RETURN_FATAL_ERROR;
                goto exit_parcelout;
                }
              if (h>=nbm-1)
                if (!(son = (short *)realloc(son,
			xn*NSONMAX*(nbm+=16)*sizeof(short))))
                  {
                  out = RETURN_FATAL_ERROR;
                  goto exit_parcelout;
                  }
              son[k-1+xn*(i+NSONMAX*(h++))] = (short)m;
              ok[k+xn*m] = (short)1;
              }
          son[k-1+xn*(i+NSONMAX*h)] = (short)-1;
          }
        }

/*------- cut the right branches (top->down) */

      for (k = xn-2; k>=0; k--)
        {
        obj = objlist[k+1].obj;
        for (i=0; i<objlist[k].nobj; i++)
          {
          for (m=h=0; (j=(int)son[k+xn*(i+NSONMAX*h)])!=-1; h++)
            {
            if (obj[j].rawvalue - obj[j].thresh*obj[j].pixnb > value0)
              m++;
            ok[k+xn*i] &= ok[k+1+xn*j];
            }
          if (m>1)	
            {
            for (h=0; (j=(int)son[k+xn*(i+NSONMAX*h)])!=-1; h++)
              if (ok[k+1+xn*j] && obj[j].rawvalue-obj[j].thresh*obj[j].pixnb
			> value0)
                {
                objlist[k+1].obj[j].flag |= OBJ_MERGED	/* Merge flag on */
			| ((OBJ_ISO_PB|OBJ_APERT_PB|OBJ_OVERFLOW)
			&tobjlist2.obj[0].flag);
                if ((out = addobj(j, &objlist[k+1], &tobjlist2))
			== RETURN_FATAL_ERROR)
                  goto exit_parcelout;
                }
            ok[k+xn*i] = (short)0;
            }
          }
        }

      if (ok[0])
        out = addobj(0, &tobjlist2, objlistout);
      else
        out = gatherup(&tobjlist2, objlistout);

exit_parcelout:

      QFREE(tobjlist2.obj);
      QFREE(tobjlist2.plist);
      tobjlist2.obj = NULL;
      tobjlist2.plist = NULL;
      tobjlist2.nobj = 0;
      tobjlist2.npix = 0;

      for (k=0; k<xn; k++)
        {
        QFREE(objlist[k].obj);
        QFREE(objlist[k].plist);
        objlist[k].obj = NULL;
        objlist[k].plist = NULL;
        objlist[k].nobj = 0;
        objlist[k].npix = 0;
        }
      }

  QFREE(tobjlist.obj);
  QFREE(tobjlist.plist);

  return out;
  }

/******************************* freeparcelout *******************************/
/*
free the memory allocated by global pointers in refine.c
*/
void	freeparcelout(void)
  {
  QFREE(son);
  QFREE(ok);
  QFREE(objlist);
  return;
  }

/********************************* gatherup **********************************/
/*
collect faint remaining pixels and allocate them to their most probable
progenitor.
*/
int	gatherup(objliststruct *objlistin, objliststruct *objlistout)

  {
   char		*bmp;
   double	*amp, *p,
		dx,dy, drand, dist,distmin;
   objstruct	*objin = objlistin->obj,
		*objout;

   pliststruct	*pixelin = objlistin->plist,
		*pixelout;

   int		i,j,k, *n, iclst,
		npix, bmpix, bmwidth,
		nobj = objlistin->nobj,
		xs,xe, ys,ye, x,y,
		out;

   size_t	bnb;

  out = RETURN_OK;

  bnb = (size_t)nobj*sizeof(double);
  amp = (double *)malloc(bnb);
  p = (double *)malloc(bnb);
  n = (int *)malloc(nobj*sizeof(int));

  if (!(amp && p && n))
    error(EXIT_FAILURE, "Not enough memory in ", "gatherup()");

  for (i=1; i<nobj; i++)
    preanalyse(i, objlistin, ANALYSE_FULL|ANALYSE_NOTHRESH);

  xs = objin[0].xmin;
  xe = objin[0].xmax;
  ys = objin[0].ymin;
  ye = objin[0].ymax;
  p[0] = 0.0;
  bmwidth = xe-xs + 1;
  npix = bmwidth * (ye-ys+1);
  if (!(bmp = (char *)calloc(1, npix*sizeof(char))))
    {
    bmp = 0;
    out = RETURN_FATAL_ERROR;
    goto exit_gatherup;
    }

  for (i=1; i<nobj; i++)
    {

/* ------------	flag pixels which are already allocated */

    for (j=objin[i].firstpix; j!=-1; j=pixelin[j].nextpix)
      {
      x = (int)pixelin[j].x;
      y = (int)pixelin[j].y;
      bmpix = (x-xs) + (y-ys)*bmwidth;
      if (bmpix<0 || bmpix>=npix)
        error(EXIT_FAILURE, "pixel out of bounds in ", "gatherup()");
      bmp[bmpix] = '\1';
      }			

    if ((n[i] = addobj(i, objlistin, objlistout)) == RETURN_FATAL_ERROR)
      {
      out = RETURN_FATAL_ERROR;
      goto exit_gatherup;
      }
    amp[i] = objin[i].thresh*exp(objin[i].scannb/
		(2*PI*objin[i].abcor*objin[i].a*objin[i].b));

/* ------------ limitate expansion ! */

    if (amp[i]>4.0*objin[i].maxflux)
      amp[i] = 4.0*objin[i].maxflux;
    }

  objout = objlistout->obj;		/* DO NOT MOVE !!! */

  if (!(pixelout=(pliststruct *)realloc(objlistout->plist,
	(objlistout->npix + npix) * sizeof(pliststruct))))
    {
    out = RETURN_FATAL_ERROR;
    goto exit_gatherup;
    }

  objlistout->plist = pixelout;
  k = objlistout->npix;
  for (j=objin[0].firstpix; j!=-1; j=pixelin[j].nextpix)
    {
    x = (int)pixelin[j].x;
    y = (int)pixelin[j].y;
    bmpix = (x-xs) + (y-ys)*bmwidth;
    if (bmpix<0 || bmpix>=npix)
      error(EXIT_FAILURE, "pixel out of bounds in ", "gatherup()");
    if (!bmp[bmpix])
      {
      pixelout[k].x = (USHORT)x;
      pixelout[k].y = (USHORT)y;
      pixelout[k].nextpix = -1;
      pixelout[k].value = pixelin[j].value;
      pixelout[k].cvalue = pixelin[j].cvalue;
      distmin = 1e+64;
      for (i=1; i<nobj; i++)
        {
        dx = (double)x - objin[i].mx;
        dy = (double)y - objin[i].my;
        dist = objin[i].cxx*dx*dx+objin[i].cyy*dy*dy+objin[i].cxy*dx*dy;
        p[i] = p[i-1] + amp[i]*exp(-0.5*dist/objin[i].abcor);
        if (dist<distmin)
          {
          distmin = dist;
          iclst = i;
          }
        }			
      if (p[nobj-1] > 1.0e-100)
        {
        drand = p[nobj-1]*rand()/RAND_MAX;
        for (i=1; p[i]<drand; i++);
	}
      else
        i = iclst;
      objout[n[i]].lastpix = pixelout[objout[n[i]].lastpix].nextpix = k++;
      }
    }

  objlistout->npix = k;
  if (!(objlistout->plist = (pliststruct *)realloc(pixelout,
	(objlistout->npix) * sizeof(pliststruct))))
    error (-1, "Not enough memory to update pixel list in ", "gatherup()");


  objlistout->cthresh = objlistin->cthresh;

exit_gatherup:

  QFREE(bmp);
  free(amp);
  free(p);
  free(n);

  return out;
  }

