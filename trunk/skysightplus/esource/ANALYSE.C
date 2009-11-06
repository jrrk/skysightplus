 /*
 				analyse.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO
*
*	Contents:	analyse() and endobject(): high level processing.
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


/********************************* analyse ***********************************/
void  analyse(int objnb, objliststruct *objlist)
  {
   objstruct		* obj = &objlist->obj[objnb];

  obj->number = ++field.ndetect;
  obj->bkg = (float)back((int)(obj->mx+0.5), (int)(obj->my+0.5));
  obj->dbkg = 0.0;

  if (prefs.pback_type == LOCAL)
    localback(obj);
  else
    obj->sigbkg = field.backsig;

  examineiso(obj, objlist->plist);

  if (FLAG(obj.apermag))
    computeapermag(obj);
  if (FLAG(obj.automag))
    computeautomag(obj);

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/* Put here your calls to PIXEL-related custom functions. Ex:

compute_myparams(obj); 

*/

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

  return;
  }


/***************************** examineiso ********************************/
/*
compute some isophotal parameters.
*/
void  examineiso(objstruct *obj, pliststruct *pixel)

  {
   int			i,j;
   double		rv, tv,sigtv;
   PIXTYPE		pix;
   static PIXTYPE	thresh[NISO];


  memset(obj->iso, 0, NISO*sizeof(int));

/*initialize isophotal thresholds so as to sample optimally the full profile*/

  if (prefs.detect_type == PHOTO)
    for (i=0; i<NISO; i++)
      thresh[i] = obj->thresh + (obj->maxflux-obj->thresh)*i/NISO;
  else
    {
    if (obj->maxflux>0.0)
      for (i=0; i<NISO; i++)
        thresh[i] = obj->thresh*pow(obj->maxflux/obj->thresh, (double)i/NISO);
    else
      for (i=0; i<NISO; i++)
        thresh[i] = 0.0;
    }

/*put objects in "segmentation check-image" */

  if (prefs.check_type == SEGMENTATION)
    for (j=obj->firstpix; j!=-1; j = pixel[j].nextpix)
      ((USHORT *)field.check_bmp)[field.width*pixel[j].y+pixel[j].x]
	= (USHORT)((obj->number*2000)%32000+obj->number%2000);

  else if (prefs.check_type == OBJECTS)
    for (j=obj->firstpix; j!=-1; j = pixel[j].nextpix)
      ((PIXTYPE *)field.check_bmp)[field.width*pixel[j].y+pixel[j].x]
	= pixel[j].value;

/*scan profile */

  if (prefs.detect_type!=PHOTO)
    {
    tv = -obj->pixnb*obj->dbkg;
    for (j=obj->firstpix; j!=-1; j = pixel[j].nextpix)
      {
      pix = pixel[j].value;
      tv += pix;
      for (i=0; pix>thresh[i] && i<NISO; i++)
        obj->iso[i]++;
      }
    sigtv = obj->sigbkg*obj->sigbkg*obj->pixnb;
    rv = tv;
    if (prefs.gain > 0.0 && tv>0.0)
      sigtv += tv/prefs.gain;
    }
  else
    {
     double	ngamma = field.ngamma;
    sigtv = 0.0;
    rv = 0.0;
    tv = -obj->pixnb*ngamma*exp(obj->dbkg/ngamma);
    for (j=obj->firstpix; j!=-1; j = pixel[j].nextpix)
      {
      pix = pixel[j].value;
      for (i=0; pix>thresh[i] && i<NISO; i++)
        obj->iso[i]++;
      rv += pix;
      pix = exp(pix/ngamma);
      tv += pix;
      sigtv += pix*pix;
      }
    tv *= ngamma;
    sigtv *= obj->sigbkg*obj->sigbkg;
    }

  obj->rawvalue = rv;
  obj->totalvalue = tv;
  obj->totalvar = sigtv;

  obj->thresh -= obj->dbkg;
  obj->maxflux -= obj->dbkg;


/*Compute the FWHM of the object */
  if (FLAG(obj.fwhm))
    {
     PIXTYPE	thresh0;

    thresh0 = obj->maxflux/5.0;
    if (thresh0<obj->thresh)
      thresh0 = obj->thresh;
    if (thresh0>0.0)
      {
       double	mx,my, s,sx,sy,sxx,sxy, dx,dy,d2, lpix,pix, b, inverr2, sat,
		dbkg, d, bmax;

      mx = (double)obj->mx;
      my = (double)obj->my;
      dbkg = obj->dbkg;
      sat = (double)(prefs.satur_level - obj->bkg);
      s = sx = sy = sxx = sxy = 0.0;
      for (j=obj->firstpix; j!=-1; j = pixel[j].nextpix)
        {
        pix = pixel[j].value-dbkg;
        if (pix>thresh0 && pix<sat)
          {
          lpix = log(pix);
          inverr2 = pix*pix;
          s += inverr2;
          dx = pixel[j].x - mx;
          dy = pixel[j].y - my;
          d2 = dx*dx+dy*dy;
          sx += d2*inverr2;
          sxx += d2*d2*inverr2;
          sy += lpix*inverr2;
          sxy += lpix*d2*inverr2;
          }        
        }
      d = s*sxx-sx*sx;
      if (fabs(d)>0.0)
        {
        b = -(s*sxy-sx*sy)/d;
        if (b<(bmax = 1/(13*obj->a*obj->b)))	/* to have FHWM # 6 sigma */
          b = bmax;
        obj->fwhm = (float)(1.6651/sqrt(b));
/*----- correction for undersampling effects (established from simulations) */
        if (obj->fwhm>0.0)
          obj->fwhm -= 1/(4*obj->fwhm);
        }
      else
        obj->fwhm = 0.0;
      }
    else
      obj->fwhm = 0.0;
    }

  return;
  }


/******************************* endobject **********************************/
/*
Final processing of object data, just before saving it to the catalog.
*/
void	endobject(int n, objliststruct *objlist)
  {
   double	rv;
   int		i;
   objstruct	*obj;


  obj = &objlist->obj[n];
  computeisomag(obj);

/* ---- choose the best magnitude according to the local crowding */

  if (prefs.clean_flag)
    flagcleancrowded(n, objlist);
  else
    if (obj->flag&OBJ_MERGED)
      {
      for (rv=0.0, i=0; i<objlist->nobj; i++)
        rv += objlist->obj[i].rawvalue;
      if (rv > obj->rawvalue*POLLUTE_CONTRAST)
        obj->flag |= OBJ_CROWDED;
      }

  if (FLAG(obj2.mag))
    {
    if (obj->flag&OBJ_CROWDED)
      {
      outobj2.mag = (-1.0)*(outobj2.isocormag);
      outobj2.sigmag = outobj2.sigmaisocormag;
      }
    else
      {
      outobj2.mag = (-1.0)*(obj->automag);
      outobj2.sigmag = obj->sigmaautomag;
      }
    }

/* ---- Check-image APERTURES option */

  if (prefs.check_type==APERTURES && FLAG(obj.apermag))
    sexcircle(field.check_bmp, field.width, field.height,
	obj->mx, obj->my, prefs.apert/2.0, 30*field.backsig);
  if (prefs.check_type==APERTURES && FLAG(obj.automag))
    sexellips(field.check_bmp, field.width, field.height,
	obj->mx, obj->my, obj->a*obj->kronfactor,
	obj->b*obj->kronfactor, obj->theta,
	30*field.backsig, obj->flag&OBJ_CROWDED);

/* ---- Astrometry */

  computeastrom(obj);		/* Warning! From now on, obj->m(x,y) += 1 */


/* ---- Aspect ratio */

  if (FLAG(obj2.e))
    {
    outobj2.e = obj->a/obj->b;
    }

/* ---- Star/Galaxy classification */

  if (FLAG(obj2.sprob))
    {
     int	j;
     double	fac2, input[10], output, fwhm;

    fwhm = prefs.seeing_fwhm;

    fac2 = fwhm/prefs.pixel_scale;
    fac2 *= fac2;
    input[j=0] = log10(obj->iso[0]? obj->iso[0]/fac2: 0.01);
    input[++j] = obj->thresh>0.0?
		  log10(obj->maxflux>0.0? obj->maxflux/obj->thresh: 0.1)
		 :-1.0;
    for (i=1; i<NISO; i++)
      input[++j] = log10(obj->iso[i]? obj->iso[i]/fac2: 0.01);
    input[++j] = log10(fwhm);
    neurresp(input, &output);
    outobj2.sprob = (float)output;
    }

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/* Put here your calls to "BLIND" custom functions. Ex:
*/

compute_myotherparams(obj, outobj2.isomag); 

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


  obj->number = ++field.nfinal;

  FPRINTF(OUTPUT, "%8d %6.1f %6.1f %5.1f %5.1f %7.3f "
			"%c%c%c%c%c%c%c%c\n",
	obj->number, obj->mx, obj->my,
	obj->a, obj->b,
	outobj2.isomag,
	obj->flag&OBJ_CROWDED?'C':'_',
	obj->flag&OBJ_MERGED?'M':'_',
	obj->flag&OBJ_SATUR?'S':'_',
	obj->flag&OBJ_TRUNC?'T':'_',
	obj->flag&OBJ_APERT_PB?'A':'_',
	obj->flag&OBJ_ISO_PB?'I':'_',
	obj->flag&OBJ_DOVERFLOW?'D':'_',
	obj->flag&OBJ_OVERFLOW?'O':'_');

  writecat(n, objlist);

  return;
  }

