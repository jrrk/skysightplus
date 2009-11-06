 /*
 				clean.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO
*
*	Contents:	functions that remove spurious detections from the
*			catalog
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include    <stdio.h>
#include	<math.h>
#include	<stdlib.h>

#include	"define.h"
#include	"globals.h"

static LONG	*cleanvictim;

#define		CLEAN_ZONE		10.0	/* zone (in sigma) to */
						/* consider for processing */

/******************************* initclean **********************************/
/*
Initialize things for CLEANing.
*/
void	initclean()
  {
  QMALLOC(cleanvictim, LONG, prefs.clean_stacksize);
  QMALLOC(field.cleanobjlist, objliststruct, 1);
  field.cleanobjlist->obj = NULL;
  field.cleanobjlist->plist = NULL;
  field.cleanobjlist->nobj = field.cleanobjlist->npix = 0;
  return;
  }


/******************************** endclean **********************************/
/*
Terminate things for CLEANing.
*/
void	endclean()
  {
  myfree(cleanvictim);
  myfree(field.cleanobjlist);
  return;
  }


/********************************** clean ***********************************/
void	clean(int objnb, objliststruct *objlistin)
  {
   objliststruct	*cleanobjlist;
   objstruct		*objin, *obj, *cleanobj;
   int			i,j,k;
   double		amp,ampin, cp, dx,dy, r,rlim;

  cleanobjlist = field.cleanobjlist;
  cleanobj = cleanobjlist->obj;
  objin = &objlistin->obj[objnb];
  ampin = objin->rawvalue/(2*PI*objin->a*objin->b*objin->abcor);
  cp = 0.5/(prefs.clean_param*prefs.clean_param);
  j=0;
  for (i=0; i<cleanobjlist->nobj; i++)
    {
    obj = &cleanobj[i];
    dx = objin->mx - obj->mx;
    dy = objin->my - obj->my;
    r = dx*dx+dy*dy;
    rlim = objin->a+obj->a;
    rlim *= rlim;
    if (r<rlim*CLEAN_ZONE*CLEAN_ZONE)
      {
      if (obj->rawvalue < objin->rawvalue)
        {
        r = cp*(objin->cxx*dx*dx+objin->cyy*dy*dy+objin->cxy*dx*dy)/objin->abcor;
        if (obj->rawvalue/obj->pixnb - ampin*exp(-r) < obj->thresh)
/*------- newcomer puts that object in his menu! */
          cleanvictim[j++] = i;
        }
      else
        {
        r = cp*(obj->cxx*dx*dx+obj->cyy*dy*dy+obj->cxy*dx*dy)/obj->abcor;
        amp = obj->rawvalue/(2*PI*obj->a*obj->b);
        if (objin->rawvalue/objin->pixnb - amp*exp(-r) < objin->thresh)
          {
/*------- newcomer is eaten!! */
          mergeobject(objin, obj);

          return;
          }
        }
      }
    }

/*Newcomer eats the content of the menu */
  for (i=j-1; i>=0; i--)
    {
    k = cleanvictim[i];
    obj = &cleanobj[k];
    mergeobject(obj, objin);
    subcleanobj(k, cleanobjlist);
    }

  addcleanobj(objnb, objlistin, cleanobjlist);

  return;
  }


/******************************* addcleanobj ********************************/
/*
Add an object to a "cleanobjlist".
*/
void	addcleanobj(int objnb, objliststruct *objlistin,
		objliststruct *cleanobjlist)

  {

/*Update the object list */
  if (cleanobjlist->nobj)
    {
    if (!(cleanobjlist->obj = (objstruct *)myrealloc(cleanobjlist->obj,
		    (++cleanobjlist->nobj) * sizeof(objstruct))))
      error(EXIT_FAILURE, "Not enough memory for ", "CLEANing");
    }
  else
    {
    if (!(cleanobjlist->obj = (objstruct *)myalloc((++cleanobjlist->nobj)
		    * sizeof(objstruct))))
      error(EXIT_FAILURE, "Not enough memory for ", "CLEANing");
    }

  objlistin->obj[objnb].ylim = (USHORT)((int)objlistin->obj[objnb].ymax
				+objlistin->obj[objnb].height);
  cleanobjlist->obj[cleanobjlist->nobj-1] = objlistin->obj[objnb];

  return;
  }


/******************************** mergeobject *******************************/
/*
Merge twos objects from "objlist".
*/
void	mergeobject(objstruct *objslave,objstruct *objmaster)

  {
  if (prefs.check_type == SEGMENTATION)
    {
     USHORT	*bmp = field.check_bmp;
     USHORT	colorslave = (USHORT)((objslave->number*2000)%32000
			+objslave->number%2000),
		colormaster = (USHORT)((objmaster->number*2000)%32000
			+objmaster->number%2000);
     int	w = field.width, pos, x,y;

    for (y=objslave->ymin; y<=objslave->ymax; y++)
      for (x=objslave->xmin; x<=objslave->xmax; x++)
        if (bmp[pos=x+y*w]==colorslave)
          bmp[pos] = colormaster;
    }

  objmaster->pixnb += objslave->pixnb;
  objmaster->scannb += objslave->scannb;
  objmaster->rawvalue += objslave->rawvalue;
  objmaster->totalvalue += objslave->totalvalue;
  objmaster->totalvar += objslave->totalvar;
  if (objslave->maxflux>objmaster->maxflux)
    objmaster->maxflux = objslave->maxflux;

  if (objslave->xmin<objmaster->xmin)
    objmaster->xmin = objslave->xmin;
  if (objslave->xmax>objmaster->xmax)
    objmaster->xmax = objslave->xmax;

  if (objslave->ymin<objmaster->ymin)
    objmaster->ymin = objslave->ymin;
  if (objslave->ymax>objmaster->ymax)
    objmaster->ymax = objslave->ymax;

  objmaster->height = (USHORT)(objmaster->ymax-objmaster->ymin+1);

  objmaster->flag |= (objslave->flag & (~(OBJ_MERGED|OBJ_CROWDED)));

  return;
  }

/******************************* subcleanobj ********************************/
/*
remove an object from a "cleanobjlist".
*/
void	subcleanobj(int objnb, objliststruct *cleanobjlist)

  {

/*Update the object list */

  if (objnb>=cleanobjlist->nobj)
    error(EXIT_FAILURE, "*Internal Error*: no CLEAN object to remove ",
	"in subcleanobj()");

  cleanobjlist->obj[objnb] = cleanobjlist->obj[--cleanobjlist->nobj];

  if (cleanobjlist->nobj)
    {
    if (!(cleanobjlist->obj = (objstruct *)myrealloc(cleanobjlist->obj,
		    cleanobjlist->nobj * sizeof(objstruct))))
      error(EXIT_FAILURE, "Not enough memory for ", "CLEANing");
    }
  else
    myfree(cleanobjlist->obj);

  return;
  }

/***************************** flagcleancrowded *****************************/
/*
Flag a "victim" if its flux is significantly contaminated by neighbours.
*/
void	flagcleancrowded(int n, objliststruct *cleanobjlist)

  {
    double	dx,dy,dxe,dye, r,rlim, rv, ct,st,a,b,x[36],y[36], k2;
    int		i,j;
    objstruct	*obj, *objin;

  rv = 0.0;
  objin = &cleanobjlist->obj[n];
  ct = cos(PI*objin->theta/180);
  st = sin(PI*objin->theta/180);
  a = objin->kronfactor*objin->a;
  b = objin->kronfactor*objin->b;
  k2 = objin->kronfactor*objin->kronfactor;
  for (j=0; j<36; j++)
    {
    x[j] = a*ctg[j]*ct - b*stg[j]*st;
    y[j] = b*ctg[j]*st + b*stg[j]*ct;
    }

  for (i=0; i<cleanobjlist->nobj; i++)
    if (i!=n)
      {
      obj = &cleanobjlist->obj[i];
      dx = objin->mx - obj->mx;
      dy = objin->my - obj->my;
      r = dx*dx+dy*dy;
      rlim = objin->a+obj->a;
      rlim *= rlim;
      if (r<rlim*CLEAN_ZONE*CLEAN_ZONE)
        {
        if (objin->cxx*dx*dx+objin->cyy*dy*dy+objin->cxy*dx*dy < k2)
          rv += obj->rawvalue;
        else
          for (j=0; j<36; j++)
            {
            dxe = dx + x[j];
            dye = dy + y[j];
            if (obj->cxx*dxe*dxe+obj->cyy*dye*dye+obj->cxy*dxe*dye
		< POLLUTE_RADIUS*POLLUTE_RADIUS)
              {
              rv += obj->rawvalue;
              break;
              }
            }
        }
      }

  if (rv > POLLUTE_CONTRAST*objin->rawvalue)
    objin->flag |= OBJ_CROWDED;

  return;
  }

