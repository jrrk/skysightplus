 /*
 				prefs.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO
*
*	Contents:	Functions to handle the configuration file.
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
#include	"prefs.h"


/********************************* readprefs ********************************/
/*
Read configuration file.
*/
void	readprefs(char *filename, char **argkey, char **argval, int narg)

  {
  FILE		*infile;
  char		*cp, *sstr, str[MAXCHAR], keyword[MAXCHAR], value[MAXCHAR];
  int		i, ival, nkey, warn, argi, flag;
  double	dval;


  if ((infile = fopen(filename,"r")) == NULL)
    error(EXIT_FAILURE,"*ERROR*: can't read ", filename);

/*Build the keyword-list from keystruct-array */

  for (i=0; key[i].name[0]; i++)
    strcpy(keylist[i], key[i].name);
  keylist[i][0] = '\0';


/*Scan the configuration file*/

  argi=0;
  flag = 1;
  for (warn=0;;)
    {
    if (flag>0)
      {
      if (!fgets(str, MAXCHAR, infile))
        flag=0;
      }

    if (flag==0)
      {
      if (argi<narg)
        {
        sprintf(str, "%s %s", argkey[argi], argval[argi]);
        argi++;
        }
      else
        break;
      }

    sstr = &str[(int)strspn(str," \t")];
    if (sstr[0]!=(char)'#' && sstr[0]!=(char)'\n')
      {
      if (warn>=20)
        error(EXIT_FAILURE, "*Error*: No valid keyword found in ", filename);
      sscanf(sstr, "%s %[^\n#\011]", keyword, value);
	  while (isspace(value[strlen(value)-1]))
			value[strlen(value)-1] = 0;
      nkey = findkey(keyword, keylist);
      if (nkey!=RETURN_ERROR)
        {
        switch(key[nkey].type)
          {
          case P_FLOAT:  dval = atof(value);
                         if (dval>=key[nkey].dmin && dval<=key[nkey].dmax)
                           *(double *)(key[nkey].ptr) = dval;
                         else
                           error(EXIT_FAILURE, keyword," keyword out of range"
						" in configuration file");
                         break;

          case P_INT:    ival = atoi(value);
                         if (ival>=key[nkey].imin && ival<=key[nkey].imax)
                           *(int *)(key[nkey].ptr) = ival;
                         else
                           error(EXIT_FAILURE, keyword," keyword out of range"
						" in configuration file");
                       break;

          case P_STRING: if (value[0])
                           strcpy((char *)key[nkey].ptr, value);
                         else
                           error(EXIT_FAILURE, keyword, " keyword empty"
						" in configuration file");
                       break;

          case P_BOOL:   if (cp = strchr("yYnN", (int)value[0]))
                           *(int *)(key[nkey].ptr)
				= (tolower((int)*cp)=='y')?1:0;
                         else
                           error(EXIT_FAILURE, keyword, " value must be"
					" Y or N in configuration file");
                       break;

          case P_KEY:    if ((ival = findkey(value, key[nkey].keylist))
				!= RETURN_ERROR)
                         *(int *)(key[nkey].ptr) = ival;
                       else
                         error(EXIT_FAILURE, keyword, " set to an unknown"
					" keyword in configuration file");
                       break;

          default:     error(EXIT_FAILURE, "*Internal ERROR*: Type Unknown",
                                 	" in readprefs()");
                       break;
          }
        key[nkey].flag = 1;
        }
      else
	{
        warning(keyword, " keyword unknown");
        warn++;
        }
      }
    }

  for (i=0; key[i].name[0]; i++)
    if (!key[i].flag)
#if 1
      fprintf(stderr, 
	"keyword %s (%s) missing in configuration file\n", key[i].name, key[i].keylist[0]);
#else
	fprintf(logfile,
	"%s %s\n", key[i].name, key[i].keylist[0]);
#endif

  fclose(infile);
  if (key[i].name[0]) sexit(1);

  return;
  }


/********************************* findkey **********************************/
/*
find an item within a list of keywords.
*/
int	findkey(char *str, char keyw[][16])

  {
  int i;

  for (i=0; keyw[i][0]; i++)
    if (!cistrcmp(str, keyw[i]))
      return i;

  return RETURN_ERROR;
  }


/********************************* useprefs *********************************/
/*
Update various structures according to the prefs.
*/
void	useprefs()

  {
   char		*str;


/*Photometry */
  field.ngamma = prefs.mag_gamma/log(10.0);
  if (prefs.pixel_scale>0.0)
    field.pixscale = prefs.pixel_scale;

/*Astrometry */

  if (prefs.pixel_scale != 0.0)
    field.pixscale = prefs.pixel_scale;	/* override astrom.definition. */

/*Background */

  field.backw = prefs.backx<field.width ? prefs.backx : field.width;
  field.backh = prefs.backy<field.height ? prefs.backy : field.height;
  field.nbackp = field.backw * field.backh;
  if ((field.nbackx = (field.width-1)/field.backw + 1) < 1)
    field.nbackx = 1;
  if ((field.nbacky = (field.height-1)/field.backh + 1) < 1)
    field.nbacky = 1;
  field.nback = field.nbackx * field.nbacky;
  field.nbackfx = field.nbackx>1 ? prefs.backfx : 1;
  field.nbackfy = field.nbacky>1 ? prefs.backfy : 1;

/*Memory and Scanning */

  field.stripheight = prefs.mem_bufsize;
  if (field.stripheight>field.height)
    field.stripheight = field.height;
  field.stripmargin = (int)(field.stripheight*(1-prefs.scan_isoapratio)/2.0);

/*Catalogue */

  if (!strcmp(prefs.cat_name, "STDOUT"))
    prefs.pipe_flag = 1;

  if (str=strrchr(field.ident, '/'))
    strcpy(field.rident,str);
  else
    strcpy(field.rident,field.ident);

  if (str=strrchr(prefs.conv_name, '/'))
    strcpy(prefs.conv_rname, str);
  else
    strcpy(prefs.conv_rname,prefs.conv_name);

  if (str=strrchr(prefs.prefs_name, '/'))
    strcpy(prefs.prefs_rname, str);
  else
    strcpy(prefs.prefs_rname, prefs.prefs_name);

  if (str=strrchr(prefs.nnw_name, '/'))
    strcpy(prefs.nnw_rname, str);
  else
    strcpy(prefs.nnw_rname,prefs.nnw_name);

/*Check-image */
  field.check_name = prefs.check_name;

  return;
  }

