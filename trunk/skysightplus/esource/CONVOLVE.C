 /*
 				convolve.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO
*
*	Contents:	functions dealing with convolution.
*
*	Last modify:	11/05/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"define.h"
#include	"globals.h"


/******************************** convolve ***********************************/
/*
convolve a scan line with an array.
*/
void	convolve(PIXTYPE *mscan)

  {
   int		mw,mw2,mn,mx,my,my0, sw,stl,snmin,snmax,sx,sy,sy0, x;
   double	pixel, *m;
   PIXTYPE	*s;

  sw = field.width;
  snmin = field.ymin*sw;
  snmax = field.ymax*sw;
  mn = field.nconv;
  mw = field.convw;
  mw2 = mw/2;
  m = field.conv;
  s = field.strip;
  stl = field.stripheight*sw;
  sy0 = (field.y - (field.convh/2))*sw;
  my0 = 0;
  if (sy0 < snmin)
    {
    my0 = mw*(snmin - sy0)/sw;
    sy0 = snmin;
    }

  for (x=0; x<sw; x++)
    {
    pixel = 0.0;
    for(my=my0, sy=sy0; my<mn && sy<snmax; my += mw, sy += sw)
      for (mx=0, sx=x-mw2; mx<mw; mx++, sx++)
        if (sx>=0 && sx<sw)
          pixel += m[my+mx]*s[sy%stl+sx];
    mscan[x] = (PIXTYPE)pixel;
    }

  return;
  }


/********************************* getconv **********************************/
/*
Read the convolution mask from a file.
*/
void	getconv()

  {
#define	MAXMASK	400

  FILE		*file;
  char		str[MAXCHAR], *sstr, *null = NULL;
  double	d[MAXMASK], sum;
  int		i,j, n;


/*Open the file containing the convolution mask */

  if (!(file = fopen(prefs.conv_name, "r")))
    error(EXIT_FAILURE, "*Error*: cannot open ", prefs.conv_name);

  fgets(str,MAXCHAR,file);
  if (strncmp(str,"CONV",4))
    error(EXIT_FAILURE, "*Error*: No convolution-mask data in ",
	prefs.conv_name);

  for (i=0, n=0; fgets(str,MAXCHAR,file);)
    {
    j=0;
    sstr = strtok(str, " \t\n");
    if (sstr && sstr[0]!=(char)'#')
      do
        {
        j++;
        d[i++] = atof(sstr);
        if (i>MAXMASK)
          error(EXIT_FAILURE, "*Error*: Convolution mask too large in ",
		prefs.conv_name);
        }	while (sstr = strtok(null, " \t\n"));
    if (j>n)
      n = j;
    }

  fclose(file);

  if ((field.convw = n)<1)
    error(EXIT_FAILURE, "*Error*: unappropriate convolution mask width in ",
	prefs.conv_name);
  if (i%n)
    error(EXIT_FAILURE, "*Error*: unappropriate convolution mask line in ",
	prefs.conv_name);

  field.convh = i/n;
  field.nconv = i;
  if (!(field.conv = (double *)myalloc(i*sizeof(double))))
    error(EXIT_FAILURE, "Not enough memory in ", "getconv()");
  for (j=0, sum=0.0; j<i; j++)
    sum += fabs(field.conv[j] = d[j]);

  if (!sum)
    error(EXIT_FAILURE, "Null convolution mask in ", prefs.conv_name);

  if (prefs.convnorm_flag)
    for (j=0; j<i; j++)
      field.conv[j] /= sum;

  return;
  }

