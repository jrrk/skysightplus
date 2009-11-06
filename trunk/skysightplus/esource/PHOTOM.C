 /*
 				photom.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO
*
*	Contents:	Compute magnitudes and other photometrical parameters.
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

#define		APEROVERSAMP	5	/* oversampling in each dimension */

/***************************** computeapermag********************************/
/*
Compute magnitude within a circular aperture.
*/
void  computeapermag(objstruct *obj)

  {
   double		bkg, sigtv, tv, xm,ym, ngamma,
			scale,scale2, offsetx,offsety,
			rx,ry, dx,dy, r,r2;
   int			area, x,y, xmin,xmax,ymin,ymax, sx,sy;
   PIXTYPE		pix;

/*Integrate flux through a circular aperture */
  xm = obj->mx;
  ym = obj->my;
  r = prefs.apert/2.0;
  r2 = r*r;
  bkg = (double)obj->dbkg;
  tv = 0.0;
  area = 0;
  scale = 1.0/APEROVERSAMP;
  scale2 = scale*scale;
  offsetx = 0.5*(scale-1.0) - xm;
  offsety = 0.5*(scale-1.0) - ym;

  xmin = RINT(xm-r);
  xmax = RINT(xm+r);
  ymin = RINT(ym-r);
  ymax = RINT(ym+r);

  if (xmin < 0)
    {
    xmin = 0;
    obj->flag |= OBJ_APERT_PB;
    }
  if (xmax >= field.width)
    {
    xmax = field.width - 1;
    obj->flag |= OBJ_APERT_PB;
    }
  if (ymin < field.ymin)
    {
    ymin = field.ymin;
    obj->flag |= OBJ_APERT_PB;
    }
  if (ymax >= field.ymax)
    {
    ymax = field.ymax - 1;
    obj->flag |= OBJ_APERT_PB;
    }

  if (prefs.detect_type!=PHOTO)
    {
    for (y = ymin; y <= ymax; y++)
      {
      ry = y + offsety;
      for (x = xmin; x <= xmax; x++)
        {
        rx = x + offsetx;
        for (sy=0; sy<APEROVERSAMP; sy++)
          {
          dy = ry + sy*scale;
          for (sx=0; sx<APEROVERSAMP; sx++)
            {
            dx = rx + sx*scale;
            if (dx*dx+dy*dy<r2)
              {
              area++;
              tv += PIX(x, y);
              }
            }
          }
        }
      }


    sigtv = area*obj->sigbkg*obj->sigbkg*scale2;
    tv -= area*bkg;
    tv *= scale2;
    if (prefs.gain > 0.0 && tv>0.0)
      sigtv += tv/prefs.gain;
    }
  else
    {
    ngamma = field.ngamma;
    sigtv = 0.0;
    for (y = ymin; y <= ymax; y++)
      {
      ry = y + offsety;
      for (x = xmin; x <= xmax; x++)
        {
        rx = x + offsetx;
        for (sy=0; sy<APEROVERSAMP; sy++)
          {
          dy = ry + sy*scale;
          for (sx=0; sx<APEROVERSAMP; sx++)
            {
            dx = rx + sx*scale;
            if (dx*dx+dy*dy<r2)
              {
              area++;
              pix = exp(PIX(x, y)/ngamma);
              tv += pix;
              sigtv += pix*pix;
              }
            }
          }
        }
      }
    tv -= area*exp(bkg/ngamma);
    tv *= ngamma*scale2;
    sigtv *= obj->sigbkg*obj->sigbkg*scale2;
    }

  if (tv>0.0)
    {
    obj->apermag = -2.5*log10(tv) + prefs.mag_zeropoint;
    obj->sigmaapermag = 1.086 * sqrt(sigtv)/tv;
    }
  else
    {
    obj->apermag = 99.0;
    obj->sigmaapermag = 99.0;
    }

  return;
  }


/***************************** computeautomag********************************/
/*
Compute magnitude within an automatic elliptical aperture.
*/
void  computeautomag(objstruct *obj)

  {
   double		bkg, di,
			sigtv, rv, tv,
			xb,yb, xm,ym, cx2,cy2,cxy,
			dx,dy, r1, dt, ngamma;
   int			area, x,y, xmin,xmax,ymin,ymax;
   PIXTYPE		pix;


/*Integrate flux through an "autoscaled" ellips */
  ngamma = field.ngamma;
  bkg = (double)obj->dbkg;
  xb = obj->mx;
  yb = obj->my;
/*Compute ellips boundaries in x and y */
  cx2 = obj->cxx/(dt=prefs.kron_nsig*prefs.kron_nsig);
  cy2 = obj->cyy/dt;
  cxy = obj->cxy/dt;
  xm = cx2 - cxy*cxy/(4.0*cy2);
  xm = xm>0.0 ? 1.0/sqrt(xm) : 0.0;
  ym = cy2 - cxy*cxy/(4.0*cx2);
  ym = ym > 0.0 ? 1.0/sqrt(ym) : 0.0;
  xmin = RINT(xb-xm);
  xmax = RINT(xb+xm);
  ymin = RINT(yb-ym);
  ymax = RINT(yb+ym);
  if (ymin < field.ymin || ymax >= field.ymax ||
		xmin < 0 || xmax >= field.width)
    obj->flag |= OBJ_APERT_PB;
  r1 = 0.0;
  rv = 0.0;
  for (y=ymin; y<=ymax; y++)
    if (y>=field.ymin && y<field.ymax)
      for (x=xmin; x<=xmax; x++)
        if (x>=0 && x<field.width)
          if (cx2*(x-xb)*(x-xb) + cy2*(y-yb)*(y-yb)
			+ cxy*(x-xb)*(y-yb) <= 1.0)
            {
            pix = PIX(x, y);
            di = pix - bkg;
            rv += di;
            dx = (double)x - obj->mx;
            dy = (double)y - obj->my;
            r1 += sqrt(dx*dx+dy*dy) * di;
            }

  if (rv>0.0 && r1>0.0)
    {
    obj->kronfactor = prefs.kron_fact*r1/(rv*sqrt(obj->a*obj->b));
    if (obj->kronfactor < prefs.kron_minsig)
      obj->kronfactor = prefs.kron_minsig;
    }
  else
    obj->kronfactor = prefs.kron_minsig;

/*Compute ellips boundaries in x and y (bis) */
  cx2 = obj->cxx/(dt=obj->kronfactor*obj->kronfactor);
  cy2 = obj->cyy/dt;
  cxy = obj->cxy/dt;
  xm = cx2 - cxy*cxy/(4.0*cy2);
  xm = xm>0.0 ? 1.0/sqrt(xm) : 0.0;
  ym = cy2 - cxy*cxy/(4.0*cx2);
  ym = ym > 0.0 ? 1.0/sqrt(ym) : 0.0;
  area = 0;
  tv = 0.0;
  xmin = RINT(xb-xm);
  xmax = RINT(xb+xm);
  ymin = RINT(yb-ym);
  ymax = RINT(yb+ym);
  if (prefs.detect_type!=PHOTO)
    {
    for (y=ymin; y<=ymax; y++)
      if (y>=0 && y<field.height)
        for (x=xmin; x<=xmax; x++)
          if (x>=0 && x<field.width)
            if (cx2*(x-xb)*(x-xb) + cy2*(y-yb)*(y-yb)
			+ cxy*(x-xb)*(y-yb) <= 1.0)
              {
              area++;
              tv += PIX(x, y);
              }
    tv -= area*bkg;
    sigtv = area*obj->sigbkg*obj->sigbkg;
    if (prefs.gain > 0.0 && tv>0.0)
      sigtv += tv/prefs.gain;
    }
  else
    {
    sigtv = 0.0;
    for (y=ymin; y<=ymax; y++)
      if (y>=0 && y<field.height)
        for (x=xmin; x<=xmax; x++)
          if (x>=0 && x<field.width)
            if (cx2*(x-xb)*(x-xb) + cy2*(y-yb)*(y-yb)
			+ cxy*(x-xb)*(y-yb) <= 1.0)
              {
              area++;
              pix = exp(PIX(x, y)/ngamma);
              tv += pix;
              sigtv += pix*pix;
              }
    tv -= area*exp(bkg/ngamma);
    tv *= ngamma;
    sigtv *= obj->sigbkg*obj->sigbkg;
    }

  if (tv>0.0)
    {
    obj->automag = -2.5*log10(tv) + prefs.mag_zeropoint;
    obj->sigmaautomag = 1.086 * sqrt(sigtv)/tv;
    }
  else
    {
    obj->automag = 99.0;
    obj->sigmaautomag = 99.0;
    obj->kronfactor = 0.0;
    }

  return;
  }


/***************************** computeisomag ********************************/
/*
compute some isophotal parameters.
*/
void  computeisomag(objstruct *obj)

  {
  double	ati,dati, rv,tv, sigtv;

  rv = obj->rawvalue;
  tv = obj->totalvalue;
  sigtv = obj->totalvar;
  if (tv>0.0)
    {
    outobj2.isomag = -2.5*log10(tv) + prefs.mag_zeropoint;
    sigtv /= tv*tv;
    outobj2.sigmaisomag = 1.086 * sqrt(sigtv);

    if (FLAG(obj2.isocormag))
      {
      ati =  obj->pixnb*obj->thresh / rv;
      if (ati>1.0)
        ati = 1.0;
      else if (ati<0.0)
        ati = 0.0;
      outobj2.isocormag = outobj2.isomag+2.5*log10(1.0-0.196099*ati-0.751208*ati*ati);
      if (FLAG(obj2.sigmaisocormag))
        {
        dati = ati*sqrt(sigtv+1.0/obj->pixnb);
        dati = 0.196099*dati + 0.751208*2*ati*dati;
        outobj2.sigmaisocormag = 1.086*sqrt(sigtv+dati*dati);
        }
      }
    }
  else
    {
    outobj2.isomag = outobj2.isocormag = 99.0;
    outobj2.sigmaisomag = outobj2.sigmaisocormag = 99.0;
    }


/*What about the peak of the profile? */

  if (obj->maxflux+obj->bkg >= prefs.satur_level)
    obj->flag |= OBJ_SATUR;

  if (FLAG(obj2.maxmu))
    outobj2.maxmu = obj->maxflux > 0.0 ? -2.5*log10((obj->maxflux)
		/ (field.pixscale * field.pixscale)) + prefs.mag_zeropoint
		: 99.0;

  if (FLAG(obj2.threshmu))
    outobj2.threshmu = obj->thresh > 0.0 ? -2.5*log10((obj->thresh)
		/ (field.pixscale * field.pixscale)) + prefs.mag_zeropoint
		: 99.0;

  return;
  }
