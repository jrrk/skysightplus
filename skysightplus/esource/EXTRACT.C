  /*
 				extract.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO
*
*	Contents:	functions for extraction of connected pixels from
*			a bitmap.
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

/*------------------------- Static buffers for lutz() -----------------------*/

static infostruct	*info, *store;
static char		*marker;
static status		*psstack;
static USHORT		*start, *end;


/******************************* lutzalloc ***********************************/
/*
Allocate once for all memory space for buffers used by lutz().
*/
void	lutzalloc()
  {
   int	stacksize;

  stacksize = field.width+1;
  QMALLOC(info, infostruct, stacksize);
  QMALLOC(store, infostruct, stacksize);
  QMALLOC(marker, char, stacksize);
  QMALLOC(psstack, status, stacksize);
  QMALLOC(start, USHORT, stacksize);
  QMALLOC(end, USHORT, stacksize);

  return;
  }


/******************************* lutzfree ************************************/
/*
Free once for all memory space for buffers used by lutz().
*/
void	lutzfree()
  {
  myfree(info);
  myfree(store);
  myfree(marker);
  myfree(psstack);
  myfree(start);
  myfree(end);

  return;
  }


/********************************** lutz *************************************/
/*
C implementation of R.K LUTZ' algorithm for the extraction of 8-connected pi-
xels in an image
*/
int	lutz(int stx, int enx, int sty, int eny, int minarea,
	     objliststruct *objlist)

  {
   static infostruct	curpixinfo,initinfo;
   objstruct		*obj;
   pliststruct		*pixel;

   char			newmarker;
   int			cn, co, luflag, objnb, pstop, xl,xl2,yl,
			out, pos,
			nobjm = NOBJ,
			width = field.width,
			height = field.height,
			bmpw = field.subwidth,
			bmph = field.subheight,
			xoff = field.subx,
			yoff = field.suby;
   short		trunflag;
   PIXTYPE		cthresh, newsymbol, mnewsymbol, *scan,*mscan;
   status		cs, ps;

  out = RETURN_OK;

  cthresh = objlist->cthresh;
  initinfo.pixnb = 0;
  initinfo.flag = 0;
  initinfo.firstpix = initinfo.lastpix = -1;
  enx++;
  eny++;
  cn = 0;

/*------Allocate memory to store object data */

  QFREE(objlist->obj);
  if (!(obj = objlist->obj =(objstruct *)myalloc(nobjm * sizeof(objstruct))))
    {
    out = RETURN_FATAL_ERROR;
    goto exit_lutz;
    }

/*------Allocate memory for the pixel list */

  QFREE(objlist->plist);
  if (!(pixel = objlist->plist
	= (pliststruct *)myalloc((eny-sty)*(enx-stx)*sizeof(pliststruct))))
    {
    out = RETURN_FATAL_ERROR;
    goto exit_lutz;
    }

/*----------------------------------------*/

  for (xl=stx; xl<=enx; xl++)
    marker[xl] = 0 ;

  objnb = objlist->nobj = 0;
  co = pstop = 0;
  curpixinfo.pixnb = 1;

  for (yl=sty; yl<=eny; yl++)
    {
    ps = COMPLETE;
    cs = NONOBJECT;
    pos = bmpw * ((yl-yoff)%bmph);
    if (yl==eny)
      scan = mscan = dumscan;
    else
      {
      scan = &field.sub[pos];
      mscan = prefs.conv_flag? &field.csub[pos]:scan;
      }

    trunflag =  (yl==0 || yl==height-1) ? OBJ_TRUNC : 0;

    for (xl=stx; xl<=enx; xl++)
      {
      mnewsymbol = (xl!=enx)?mscan[xl-xoff]:-BIG;
      newsymbol = (xl!=enx)?scan[xl-xoff]:-BIG;
      newmarker = marker[xl];
      marker[xl] = 0;
      curpixinfo.flag = trunflag;
      luflag = mnewsymbol > cthresh?1:0;

      if (luflag)
        {
        if (xl==0 || xl==width-1)
          curpixinfo.flag |= OBJ_TRUNC;
        pixel[cn].x = xl;
        pixel[cn].y = yl;
        pixel[cn].value = newsymbol;
        pixel[cn].cvalue = mnewsymbol;
        pixel[cn].nextpix = -1;
        curpixinfo.lastpix = curpixinfo.firstpix = (LONG)(cn++);
       if (cs != OBJECT)
/*------------------------------- Start Segment -----------------------------*/
          {
          cs = OBJECT;
          if (ps == OBJECT)
              {
              if (start[co] == UNKNOWN)
                {
                marker[xl] = 'S';
              start[co] = xl;
                }
              else  marker[xl] = 's';
              }
          else
            {
            psstack[pstop++] = ps;
            marker[xl] = 'S';
            start[++co] = xl;
            ps = COMPLETE;
            info[co] = initinfo;
            }
          }
        }
/*---------------------------------------------------------------------------*/
      if (newmarker)
/*---------------------------- Process New Marker ---------------------------*/

        {
        if (newmarker == 'S')
          {
          psstack[pstop++] = ps;
          if (cs == NONOBJECT)
            {
            psstack[pstop++] = COMPLETE;
            info[++co] = store[xl];
            start[co] = UNKNOWN;
          }
          else
            update (&info[co],&store[xl], pixel);
          ps = OBJECT;
          }
        else if (newmarker == 's')
          {
          if ((cs == OBJECT) && (ps == COMPLETE))
            {
            pstop--;
            xl2 = start[co];
            update (&info[co-1],&info[co], pixel);
          if (start[--co] == UNKNOWN)
              start[co] = xl2;
            else
              marker[xl2] = 's';
            }
          ps = OBJECT;
          }
        else if (newmarker == 'f')
          ps = INCOMPLETE;
        else if (newmarker == 'F')
          {
          ps = psstack[--pstop];
          if ((cs == NONOBJECT) && (ps == COMPLETE))
            {
          if (start[co] == UNKNOWN)
              {
              if ((int)info[co].pixnb >= minarea)
                {
                if (objlist->nobj>=nobjm)
                  if (!(obj = objlist->obj = (objstruct *)
  			myrealloc(obj, (nobjm+=nobjm/2)* sizeof(objstruct))))
                    {
                    out = RETURN_FATAL_ERROR;
                    goto exit_lutz;
                    }
                lutzsort(&info[co], objlist);
                }
              }
            else
            {
              marker[end[co]] = 'F';
              store[start[co]] = info[co];
              }
            co--;
            ps = psstack[--pstop];
            }
          }
        }
  
/*---------------------------------------------------------------------------*/

      if (luflag)
        update (&info[co],&curpixinfo, pixel);
    else
        {
        if (cs == OBJECT)
/*-------------------------------- End Segment ------------------------------*/
          {
          cs = NONOBJECT;
          if (ps != COMPLETE)
            {
            marker[xl] = 'f';
            end[co] = xl;
            }
          else
          {
            ps = psstack[--pstop];
            marker[xl] = 'F';
            store[start[co]] = info[co];
            co--;
            }
          }
        }
/*---------------------------------------------------------------------------*/
      }
    }

exit_lutz:

   if (objlist->nobj && out == RETURN_OK)
    {
    if (!(objlist->obj=(objstruct *)myrealloc(obj,
		objlist->nobj*sizeof(objstruct))))
      error(EXIT_FAILURE,"problem with mem. myrealloc. in lutz()","");
    }
  else
    {
    QFREE(obj);
    objlist->obj = NULL;
    }

  if (cn && out == RETURN_OK)
    {
    if (!(objlist->plist=(pliststruct *)myrealloc(pixel,cn*sizeof(pliststruct))))
      error(EXIT_FAILURE,"problem with mem. myrealloc. in lutz()","");
    }
  else
    {
    QFREE(pixel);
    objlist->plist = NULL;
    }

  return  out;
  }


/********************************* lutzsort ***********************************/
/*
build the object structure.
*/
void  lutzsort(infostruct *info, objliststruct *objlist)

  {
  objstruct  *obj = &objlist->obj[objlist->nobj];
  pliststruct  *pixel = objlist->plist;

  obj->firstpix = info->firstpix;
  obj->lastpix = info->lastpix;
  obj->flag = info->flag;
  objlist->npix += info->pixnb;

  preanalyse(objlist->nobj, objlist, ANALYSE_FAST);

  objlist->nobj++;

  return;
  }

