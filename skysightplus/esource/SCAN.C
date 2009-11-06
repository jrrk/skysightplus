  /*
 				scan.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	functions for extraction of connected pixels from
*			a bitmap.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<assert.h>

#include	"define.h"
#include	"globals.h"

#define		BSAWP 1

/****************************** scanimage ************************************/
/*
Scan of the large bitmap. Main loop.
*/
void	scanimage()

{
  static infostruct	curpixinfo, *info, *store,
			initinfo, freeinfo, *victim;
  objliststruct		objlist;
  pliststruct		*pixel;

  char			*marker, newmarker;
  int			co, i,j, flag, luflag,pstop, xl,xl2,yl, cn,npo,
			stacksize;
  short			trunflag;
  LONG			maxpixnb;
  PIXTYPE		cthresh,
			mnewsymbol, newsymbol, *mscan, *scan;
  status		cs, ps, *psstack;
  USHORT		*start, *end;

/*Allocate memory for buffers */

  stacksize = field.width+1;
  QMALLOC(info, infostruct, stacksize);
  QMALLOC(store, infostruct, stacksize);
  QMALLOC(marker, char, stacksize);
  QMALLOC(dumscan, PIXTYPE, stacksize);
  QMALLOC(psstack, status, stacksize);
  QMALLOC(start, USHORT, stacksize);
  QMALLOC(end, USHORT, stacksize);
  QMALLOC(ghisto, LONG, QUANTIF_NMAXLEVELS);
  lutzalloc();

/*some initializations */

  field.nfinal = field.ndetect = 0;
  cthresh = (PIXTYPE)(objlist.cthresh = field.thresh);
  initinfo.pixnb = 0;
  initinfo.flag = 0;
  initinfo.firstpix = initinfo.lastpix = -1;

  for (i=0; i<37; i++)
    {
    ctg[i] = cos(i*PI/18);
    stg[i] = sin(i*PI/18);
    }

  for (xl=0; xl<=field.width; xl++)
    {
    marker[xl]  = 0 ;
    dumscan[xl] = -BIG ;
    }

  co = pstop = 0;
  objlist.nobj = 1;
  curpixinfo.pixnb = 1;

  if (prefs.conv_flag)
    QMALLOC(mscan, PIXTYPE, stacksize);

/*---------------- init cleaning procedure */

  if (prefs.clean_flag)
    initclean();

/*----- Allocate memory for the image buffer */

  if (!(field.strip=(PIXTYPE *)malloc(field.stripheight*field.width
	*sizeof(PIXTYPE))))
    error(EXIT_FAILURE,"Not enough memory for the image buffer in ",
	"scanimage()");

/*----- Allocate memory for the pixel list */

  npo = prefs.mem_pixstack;

  if (!(pixel=objlist.plist=(pliststruct *)malloc(npo*sizeof(pliststruct))))
    error(EXIT_FAILURE, "Not enough memory to store the pixel list in ",
	"scanimage()");

/*----- at the beginning, the "free" object fills the whole pixel list */

  freeinfo.firstpix = 0;
  freeinfo.lastpix = (LONG)npo-1;
  for (i=0; i<npo-1; i++)
    pixel[i].nextpix = (LONG)i+1;
  pixel[npo-1].nextpix = -1;

/*----- Beginning of the main loop: Initialisations  */

  field.y = field.stripy = 0;
  field.ymin = field.stripylim = 0;
  field.stripysclim = 0;

/*----- Here we go */
  for (yl=0; yl<=field.height; yl++)
    {
    ps = COMPLETE;
    cs = NONOBJECT;
    field.stripy = (field.y=yl)%field.stripheight;

    if (yl==field.height)
      scan = dumscan;
    else if (field.stripy==field.stripysclim)
      scan = loadstrip();
    else
      scan = &field.strip[field.stripy*field.width];

    if (prefs.check_type==CONVOLVED && yl)
      {
#     ifdef BSWAP
        swapbytes(mscan, sizeof(PIXTYPE), field.width);
#     endif
      QFWRITE(mscan, field.width*sizeof(PIXTYPE), field.check_file,
		field.check_name);
      }

    if (prefs.conv_flag && yl!=field.height)
      convolve(mscan);
    else
      mscan = scan;

    trunflag = (yl==0 || yl==field.height-1)? OBJ_TRUNC:0;

    for (xl=0; xl<=field.width; xl++)
      {
      if (xl == field.width)
        mnewsymbol = newsymbol = -BIG;
      else
        {
        newsymbol = scan[xl];
        mnewsymbol = mscan[xl];
        }

      newmarker = marker[xl];
      marker[xl] = 0;

      curpixinfo.flag = trunflag;
      luflag = mnewsymbol > cthresh?1:0;

      if (luflag)
        {
        cn = freeinfo.firstpix;
        if (xl==0 || xl==field.width-1)
          curpixinfo.flag |= OBJ_TRUNC;
        freeinfo.firstpix = pixel[cn].nextpix;

/*------- Running out of pixels, the largest object becomes a "victim" ------*/

        if (freeinfo.firstpix==freeinfo.lastpix)
          {
          maxpixnb = 0;
          for (i=0; i<=field.width; i++)
            if (store[i].pixnb>maxpixnb)
              if (marker[i]=='S' || (newmarker=='S' && i==xl))
                {
                flag = 0;
                if (i<xl)
                  for (j=0; j<=co; j++)
                    flag |= (start[j]==i);
                if (!flag)
                  maxpixnb = (victim = &store[i])->pixnb;
              }
          for (j=1; j<=co; j++)
            if (info[j].pixnb>maxpixnb)
              maxpixnb = (victim = &info[j])->pixnb;

          if (!maxpixnb)
            error(EXIT_FAILURE, "*Fatal Error*: something is badly bugged in ",
		"scanimage()!");
          if (maxpixnb <= 1)
            error(EXIT_FAILURE, "Pixel stack overflow in ", "scanimage()");
          freeinfo.firstpix = pixel[victim->firstpix].nextpix;
          pixel[victim->lastpix].nextpix = freeinfo.lastpix;
          pixel[victim->lastpix = victim->firstpix].nextpix = -1;
          victim->pixnb = 1;
          victim->flag |= OBJ_OVERFLOW;
          }

/*---------------------------------------------------------------------------*/

        pixel[cn].x = xl;
        pixel[cn].y = yl;
        pixel[cn].cvalue = mnewsymbol;
        pixel[cn].value = newsymbol;
        pixel[cn].nextpix = -1;
        curpixinfo.lastpix = curpixinfo.firstpix = (LONG)cn;

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
            else
              marker[xl] = 's';
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

/*---------------------------------------------------------------------------*/
        }

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
              if ((int)info[co].pixnb >= prefs.ext_minarea)
                sortit(&info[co], &objlist);
/* ------------------------------------ free the chain-list */

              pixel[info[co].lastpix].nextpix = freeinfo.firstpix;
              freeinfo.firstpix = info[co].firstpix;
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
    if (!((yl+1)%16))
      NPRINTF(OUTPUT, "\33[1M> Line:%5d  "
		"Objects: %8d detected / %8d sextracted\n\33[1A",
	yl+1, field.ndetect, field.nfinal);
    }

  if (prefs.clean_flag)
    {
     int	n;


    if (n=field.cleanobjlist->nobj)
      {
      for (i=0; i<n; i++)
        endobject(i, field.cleanobjlist);
      }
    endclean();
    }

/*Free memory */

  free(field.strip);
  freeparcelout();
  QFREE(pixel);

  if (prefs.conv_flag)
    free(mscan);

  lutzfree();
  free(ghisto);
  free(info);
  free(store);
  free(marker);
  /* free(dumscan);	*/
  free(psstack);
  free(start);
  free(end);

  return;
  }

/********************************* update ************************************/
/*
update object's properties each time one of its pixels is scanned by lutz()
*/
void  update(infostruct *infoptr1, infostruct *infoptr2, pliststruct *pixel)

  {
  infoptr1->pixnb += infoptr2->pixnb;
  infoptr1->flag |= infoptr2->flag;
  if (infoptr1->firstpix == -1)
    {
    infoptr1->firstpix = infoptr2->firstpix;
    infoptr1->lastpix = infoptr2->lastpix;
    }
  else if (infoptr2->lastpix != -1)
    {
    pixel[infoptr1->lastpix].nextpix = infoptr2->firstpix;
    infoptr1->lastpix = infoptr2->lastpix;
    }

  return;
  }

/********************************* sortit ************************************/
/*
build the object structure.
*/
void  sortit(infostruct *info, objliststruct *objlist)

  {
   objliststruct	objlistout, *objlist2;
   static objstruct	obj;
   pliststruct		*pixel;
   int 			i,j, retflag;

  retflag = RETURN_OK;
  pixel = objlist->plist;
  objlistout.obj = NULL;
  objlistout.plist = NULL;
  objlistout.nobj = objlistout.npix = 0;

/*------Allocate memory to store object data */

  objlist->obj = &obj;
  objlist->nobj = 1;

  objlist->npix = info->pixnb;
  obj.firstpix = info->firstpix;
  obj.lastpix = info->lastpix;
  obj.flag = info->flag;

  preanalyse(0, objlist, ANALYSE_FAST);

/*----- Check if the current strip contains the lower isophote... */
  if (obj.ymin < field.ymin)
    obj.flag |= OBJ_ISO_PB;

  if (!(obj.flag & OBJ_ISO_PB) && !prefs.conv_flag)
    {
/*- ...if yes use the strip bitmap itself to perform deblending... */
    field.sub = field.strip;
    field.subwidth = field.width;
    field.subheight= field.stripheight;
    field.subx = 0;
    field.suby = (obj.ymin/field.stripheight)*field.stripheight;
    field.subflag = 0;
    }
  else
/*- ...otherwise create a new bitmap from the pixel list */
    retflag = createbmp(0, objlist);

  if (!(obj.flag & OBJ_OVERFLOW) && obj.scannb > obj.pixnb/2
	&& retflag == RETURN_OK)
    {
    if (parcelout(objlist, &objlistout) == RETURN_OK)
      objlist2 = &objlistout;
    else
      {
      objlist2 = objlist;
      for (i=0; i<objlist2->nobj; i++)
        objlist2->obj[i].flag |= OBJ_DOVERFLOW;
      }
    }
  else
    objlist2 = objlist;

  for (i=0; i<objlist2->nobj; i++)
    {
    preanalyse(i, objlist2, ANALYSE_FULL);
    analyse(i, objlist2);
    if (prefs.clean_flag)
      {
       objliststruct	*cleanobjlist;
       objstruct	*cleanobj;
       int		n, ymin, victim;


      cleanobjlist = field.cleanobjlist;
      if ((n=cleanobjlist->nobj)>= prefs.clean_stacksize)
        {
        ymin = 2*field.height;
        cleanobj = cleanobjlist->obj;
        for (j=0; j<n; j++)
          if (cleanobj[j].ylim < ymin)
            {
            victim = j;
            ymin = cleanobj[j].ylim;
            }

        endobject(victim, cleanobjlist);
        subcleanobj(victim, cleanobjlist);
        }

      clean(i, objlist2);
      }
    else
      endobject(i, objlist2);
    }

  if (field.subflag)
    {
    QFREE(field.sub);
    QFREE(field.csub);
    }

  QFREE(objlistout.plist);
  QFREE(objlistout.obj);
  return;
  }

/******************************** createbmp **********************************/
/*
build a bitmap from a chained-list of pixels.
*/
int	createbmp(int no, objliststruct *objlist)

  {
   objstruct	*obj;
   pliststruct	*pixel;
   int		i, x,y, pos;
   PIXTYPE	*bmp, *cbmp;

  obj = &objlist->obj[no];
  pixel = objlist->plist;

  field.subwidth = obj->xmax - obj->xmin + 1;
  field.subheight = obj->ymax - obj->ymin + 1;
  field.subx = obj->xmin;
  field.suby = obj->ymin;
  field.subflag = 1;

  if (!(bmp = field.sub =
	(PIXTYPE *)calloc(field.subwidth*field.subheight,sizeof(PIXTYPE))))
    return RETURN_FATAL_ERROR;
  if (prefs.conv_flag)
    {
    if (!(cbmp = field.csub =
	(PIXTYPE *)calloc(field.subwidth*field.subheight,sizeof(PIXTYPE))))
    return RETURN_FATAL_ERROR;
    }
  else
    cbmp = field.csub = NULL;

  for (i=obj->firstpix; i != -1; i = pixel[i].nextpix)
    {
    x = pixel[i].x - obj->xmin;
    y = pixel[i].y - obj->ymin;
    pos = x + y*field.subwidth;
    bmp[pos] = pixel[i].value;
    if (prefs.conv_flag)
      cbmp[pos] = pixel[i].cvalue;
    }

  return RETURN_OK;
  }

/******************************** preanalyse *********************************/
/*
compute useful parameters for each object, from the pixel-list.
*/
void  preanalyse(int no, objliststruct *objlist, int analyse_type)

  {
   objstruct	*obj = &objlist->obj[no];
   pliststruct	*pixel = objlist->plist;
   PIXTYPE	maxflux;
   double	dthresh,thresh2, t1t2,darea,
		val,
		mx,my, mx2,my2,mxy, tv,
		xm,ym, xm2,ym2,xym,
		temp, theta, ctheta,stheta,ct2,st2,cst,
		pmx2,pmy2, a,b;
   int		j, x, y, xmin,xmax, ymin,ymax,area2,
		pixnb, scannb;
  
  if (!(analyse_type & ANALYSE_NOTHRESH))
    obj->thresh = objlist->cthresh;

  dthresh = obj->thresh;

/*-----  initialize stacks and bounds */
  pixnb = scannb = 0;
  tv = 0.0;
  mx = my = 0.0;
  mx2 = my2 = mxy = 0.0;
  maxflux = -BIG;
  ymin = xmin = 2*MAXPICSIZE;    /* to be really sure!! */
  ymax = xmax = 0;

/*-----  integrate results */
  for (j=obj->firstpix; j != -1; j = pixel[j].nextpix)
    {
    x = (int)pixel[j].x;
    y = (int)pixel[j].y;
    val =  pixel[j].value;
    pixnb++;
    if (val>dthresh)
      scannb++;
    tv += val;
    mx += val * x;
    my += val * y;
    mx2 += val * x*x;
    my2 += val * y*y;
    mxy += val * x*y;
    if (maxflux < val)
      maxflux = val;
    if (xmin > x)
      xmin = x;
    if (xmax < x)
      xmax = x;
    if (ymin > y)
      ymin = y;
    if (ymax < y)
      ymax = y;
    }    

/*----- compute object's properties */
  xm = mx / tv;			/* mean x */
  ym = my / tv;			/* mean y */


  if (analyse_type & ANALYSE_FULL)
    {
    xm2 = mx2 / tv - xm * xm;	/* variance of x */
    ym2 = my2 / tv - ym * ym;	/* variance of y */
    if (xm2 < 0.0)  xm2 = 0.0;
    if (ym2 < 0.0)  ym2 = 0.0;
    xym = mxy / tv - xm * ym;	/* covariance */

    if (fabs(temp=xm2-ym2) > 0.0)
      theta = atan(2.0 * xym / temp) / 2.0;
    else
      theta = PI/4.0;
    stheta = sin(theta);
    ctheta = cos(theta);
    ct2 = ctheta*ctheta;
    st2 = stheta*stheta;
    cst = 2*ctheta*stheta;
    pmx2 = xm2*ct2 + ym2*st2 + xym*cst;
    pmy2 = xm2*st2 + ym2*ct2 - xym*cst;

    if (pmx2<0.0841)
      pmx2 = 0.0841;

    if (pmy2<0.0841)
      pmy2 = 0.0841;

    if (pmx2 > pmy2)
      {
      a = sqrt(pmx2);
      b = sqrt(pmy2);
      }
    else
      {
      a = sqrt(pmy2);
      b = sqrt(pmx2);
      theta += PI/2.0;
      }

    obj->a = a;
    obj->b = b;
    obj->theta = theta*180.0/PI;
    if (obj->theta>90.0)
      obj->theta -= 180.0;
    obj->cxx = (float)(ct2/pmx2+st2/pmy2);
    obj->cyy = (float)(st2/pmx2+ct2/pmy2);
    obj->cxy = (float)(cst*(1/pmx2-1/pmy2));

    thresh2 = (dthresh + maxflux)/2.0;
    area2 = 0;
    for (j=obj->firstpix; j != -1; j = pixel[j].nextpix)
      if (pixel[j].value > thresh2)
        area2++;
    darea = (double)area2 - scannb;
    t1t2 = dthresh/thresh2;
    if (t1t2>0.0)
      obj->abcor = (darea<0.0?darea:-1.0)/(2*PI*log(t1t2<1.0?t1t2:0.99)*a*b);
    else
      obj->abcor = 1.0;
    }

/* copy some data to "obj" structure */

  obj->pixnb = (LONG)pixnb;
  if (!(obj->flag & OBJ_OVERFLOW))
    obj->scannb = (LONG)scannb;
  obj->rawvalue = tv;
  obj->mx = xm;
  obj->my = ym;
  obj->maxflux = maxflux;
  obj->xmin = (USHORT)xmin;
  obj->xmax = (USHORT)xmax;
  obj->ymin = (USHORT)ymin;
  obj->ymax = (USHORT)ymax;
  obj->height = (USHORT)(ymax - ymin + 1);

  return;
  }

