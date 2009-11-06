 /*
 				main.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	parsing.
*
*	Last modify:	25/02/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include	<ctype.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<assert.h>
#include    <windows.h>
#include	"define.h"
#include	"globals.h"

#define		SYNTAX \
"sex <image_name> [-c <configuration_file>] [-<keyword> <value>]"

/********************************** main ************************************/

FILE *logfile;

int	unprotected_sextract_main(int argc, char *argv[])
  {
   int		a, narg;
   char		**argkey, **argval;
   char *ptr1, p_name[ MAX_PATH + 1 ];
   strcpy(p_name, argv[0]);
   ptr1 = strrchr(p_name, '\\');
   if (ptr1) strcpy(ptr1, "\\default.sex");  

  QMALLOC(argkey, char *, argc);
  QMALLOC(argval, char *, argc);

/*default parameters */

  logfile = fopen("logfile.txt", "wt");

  prefs.pipe_flag = 0;
  strcpy(prefs.image_name, argv[1]);
  strcpy(prefs.prefs_name, p_name);
  narg = 0;

  for (a=1; a<argc; a++)
    {
    if (argv[a][0] == '-' && a<(argc-1))
      {
      if (strlen(argv[a])<3)
        switch((int)tolower((int)argv[a][1]))
          {
          case 'c':	strcpy(prefs.prefs_name, argv[++a]);
			break;
          default:	error(EXIT_SUCCESS,"SYNTAX: %s", SYNTAX);
          }
      else
        {
        argkey[narg] = &argv[a][1];
        argval[narg++] = argv[++a];
        }       
      }
    else
      strcpy(prefs.image_name, argv[a]);
    }

  readprefs(prefs.prefs_name, argkey, argval, narg);
  QFREE(argkey);
  QFREE(argval);

  if (ptr1)
	{
	  sprintf(ptr1, "\\%s", prefs.param_name);
	  strcpy(prefs.param_name, p_name);
	  sprintf(ptr1, "\\%s", prefs.conv_name);
	  strcpy(prefs.conv_name, p_name);
  }
  makeit();

  NFPRINTF(OUTPUT, "All done");
  NPRINTF(OUTPUT, "\n");

  fclose(logfile);
  return(EXIT_SUCCESS);
}