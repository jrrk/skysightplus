 /*
 				astrom.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP
*
*	Contents:	Astrometrical computations.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
#include    <stdio.h>
#include	<math.h>
#include    <assert.h>

#include	"define.h"
#include	"globals.h"

/****************************** initastrom ***********************************/
/*
initialize astrometrical matrix according to FITS info.
*/
void	initastrom()

  {
  lmapstruct	*lmap;
  double	det, zxx,zxy,zyx,zyy;

  lmap = &field.lmap;

  zxx = cos(field.crotax*PI/180.0);
  zxy = -sin(field.crotay*PI/180.0);
  zyx = sin(field.crotax*PI/180.0);
  zyy = cos(field.crotay*PI/180.0);

  det = zxx*zyy-zxy*zyx;
  if (det != 0.0)
    {
    lmap->axx = zyy/det;
    lmap->axy = -zxy/det;
    lmap->ayx = -zyx/det;
    lmap->ayy = zxx/det;
    }
  else
    {
    warning ("Improper FITS-image coordinate-system:\n",
	"           WORLD-parameters may be uncorrect");
    lmap->axx = lmap->ayy = 1.0;
    lmap->axy = lmap->ayx = 0.0;
    }

  lmap->cx = field.crvalx;
  lmap->cy = field.crvaly;
  lmap->px = (double)field.crpixx;
  lmap->py = (double)field.crpixy;
  lmap->fx = field.cdeltx;
#ifdef MAMA
  lmap->fx /= cos(PI/180.0*lmap->cy);	/* MAMAspecific */
#endif
  lmap->fy = field.cdelty;

  if (prefs.pixel_scale == 0.0)
    {
    field.pixscale = sqrt(fabs(lmap->fx*lmap->fy / (3600.0*3600.0)
			* cos(PI/180.0*(field.crotax-field.crotay))));
    }

  return;
  }

/**************************** computeastrom *********************************/
/*
Compute real WORLD coordinates and dimensions according to FITS info.
*/
void	computeastrom(objstruct *obj)

  {
  lmapstruct	*lmap;
  double	dx,dy, dxe,dye;

  lmap = &field.lmap;

  if (FLAG(obj2.alpha))
    {
/*- The new astrometric format (01/06/93) implemented in sex EB 02/02/95 */
/*- MAMAspecific: swap x<->y */

     mapstruct	*map = &field.map;
     double	posx, posy;

    posx = map->x + (obj->my+0.5) * map->pdx;
    posy = map->y + (obj->mx+0.5) * map->pdy;
    xytoadII(map, posx, posy, map->epoch, &outobj2.alpha, &outobj2.delta);
    }

  obj->mx += 1.0;	/* That's standard FITS */
  obj->my += 1.0;

  dx = obj->mx - lmap->px;
  dy = obj->my - lmap->py;


  if (FLAG(obj2.mxw))
    outobj2.mxw = lmap->cx + (lmap->axx*dx + lmap->axy*dy)*lmap->fx;
  if (FLAG(obj2.myw))
    outobj2.myw = lmap->cy + (lmap->ayx*dx + lmap->ayy*dy)*lmap->fy;

  if (FLAG(obj2.aw) || FLAG(obj2.thetaw))
    {
    dx = obj->a*cos(PI/180.0*obj->theta);
    dy = obj->a*sin(PI/180.0*obj->theta);
    dxe = (lmap->axx*dx + lmap->axy*dy)*lmap->fx;
    dye = (lmap->ayx*dx + lmap->ayy*dy)*lmap->fy;
    outobj2.aw = sqrt(dxe*dxe+dye*dye);
    if (FLAG(obj2.thetaw))
      if (dxe != 0.0)
        outobj2.thetaw = 180.0/PI*atan(dye/dxe);
      else
        outobj2.thetaw = 90.0;
    }

  if (FLAG(obj2.bw))
    {
    dx = obj->b*sin(PI/180.0*obj->theta);
    dy = -obj->b*cos(PI/180.0*obj->theta);
    dxe = (lmap->axx*dx + lmap->axy*dy)*lmap->fx;
    dye = (lmap->ayx*dx + lmap->ayy*dy)*lmap->fy;
    outobj2.bw = sqrt(dxe*dxe+dye*dye);
    }

  if (FLAG(obj2.scannbw))
    outobj2.scannbw = obj->scannb*field.pixscale*field.pixscale;

  if (FLAG(obj2.fwhmw))
    outobj2.fwhmw = obj->fwhm*field.pixscale;

  return;
  }

/*--------------------------- MAMAspecific EB 02/02/95 ----------------------*/

/********************************* xytoadII **********************************/
/*
convert x,y MAMA coordinates to alpha, delta. (Astrometric Std version II).
*/
void	xytoadII(mapstruct *map, double x, double y, double year,
		double *alpha, double *delta)

  {
   double	alpha0 = map->alphacent * PI/180.0,
		delta0 = map->deltacent * PI/180.0,
		f = map->foc,
		ama0, cd0, sd0;

  xytoxyII(map, &x, &y);
  cd0 = cos(delta0);
  sd0 = sin(delta0);
  ama0 = atan(x / (cd0 - y*sd0));
  *alpha = ama0 + alpha0;
  *delta = atan( cos(ama0)*(sd0+y*cd0)/(cd0-y*sd0));
  *alpha *= 180.0/PI;
  *delta *= 180.0/PI;
  if (fabs(map->equinox-year) > 0.01)
    precess(map->equinox, *alpha, *delta, year, alpha, delta);

  return;
  }


/********************************* xytoxyII **********************************/
/*
perform a 2-D mapping of x,y coordinates. (Astrometric Std version II).
*/
void	xytoxyII(mapstruct *map, double *xin, double *yin)

  {
   double	*dx = map->dx,
		*dy = map->dy,
                x, y;
   int		i;

  x = (*xin - map->xcent)/map->foc;
  y = (*yin - map->ycent)/map->foc;
  *xin = dx[0] + dx[1]*x + dx[2]*y + dx[3]*x*x + dx[4]*y*y + dx[5]*x*y
	+ dx[6]*x*x*x + dx[7]*y*y*y + dx[8]*x*x*y + dx[9]*x*y*y;
  *yin = dy[0] + dy[1]*x + dy[2]*y + dy[3]*x*x + dy[4]*y*y + dy[5]*x*y
	+ dy[6]*x*x*x + dy[7]*y*y*y + dy[8]*x*x*y + dy[9]*x*y*y;

  return;
  }


/********************************* precess ***********************************/
/*
precess equatorial coordinates according to the equinox (from Ephemerides du
Bureau des Longitudes 1992). Epoch for coordinates should be J2000
(FK5 system).
*/
void	precess(double yearin, double alphain, double deltain,
		double yearout, double *alphaout, double *deltaout)

  {
   double	dzeta,theta,z, t1,t2, cddsadz, cddcadz, cdd, sdd, adz;

  alphain *= PI/180.0;
  deltain *= PI/180.0;

  t1 = (yearin - 2000.0)/1000.0;
  t2 = (yearout - yearin)/1000.0;
  theta = (97171.735e-06 - 413.691e-06*t1 - 1.052e-06 * t1*t1) * t2
	+ (-206.846e-06 - 1.052e-06*t1) * t2*t2 - 202.812e-06 * t2*t2*t2;
  dzeta = (111808.609e-06 + 677.071e-06*t1 - 0.674e-06 * t1*t1) * t2
	+ (146.356e-06 - 1.673e-06*t1) * t2*t2 + 87.257e-06 * t2*t2*t2;
  z = (111808.609e-06 +677.071e-06*t1 - 0.674e-06 * t1*t1) * t2
	+ (530.716e-06 + 0.320e-06*t1) * t2*t2 + 88.251e-06 * t2*t2*t2;
  cddsadz = cos(deltain) * sin(alphain+dzeta);
  cddcadz = -sin(deltain)*sin(theta)+cos(deltain)*cos(theta)*cos(alphain+dzeta);
  sdd = sin(deltain)*cos(theta) + cos(deltain)*sin(theta)*cos(alphain+dzeta);
  cdd = cos(*deltaout = asin(sdd));
  adz = asin(cddsadz/cdd);
  if (cddcadz<0.0)
    adz = PI - adz;
  if (adz<0.0)
    adz += 2.0*PI;
  adz += z;
  *alphaout = 180.0/PI*adz;
  *deltaout *= 180.0/PI;

  return;
  }

/*-------------------------------------------------------------------------*/

