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

#include	"define.h"
#include	"globals.h"

#define		SYNTAX \
"sex <image_name> [-c <configuration_file>] [-<keyword> <value>]"

/********************************** main ************************************/

FILE *logfile;

int	main(int argc, char *argv[])

  {
   int		a, narg;
   char		**argkey, **argval;

  if (argc<1)
    {
    fprintf(OUTPUT, "\n		%s  Version %s\n", BANNER, VERSION);
    fprintf(OUTPUT, "\nFor information, please contact: %s\n", COPYRIGHT);
    error(EXIT_SUCCESS, "SYNTAX: ", SYNTAX);
    }

  QMALLOC(argkey, char *, argc);
  QMALLOC(argval, char *, argc);

/*default parameters */

  logfile = fopen("logfile.txt", "wt");

  prefs.pipe_flag = 0;
  strcpy(prefs.image_name, "M32.fit");
  strcpy(prefs.prefs_name, "default.sex");
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
          default:	error(EXIT_SUCCESS,"SYNTAX: ", SYNTAX);
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

  makeit();

  NFPRINTF(OUTPUT, "All done");
  NPRINTF(OUTPUT, "\n");

  fclose(logfile);
  return(EXIT_SUCCESS);
  }
