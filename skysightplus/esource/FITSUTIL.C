/*
 				fitsutil.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	sextractorII
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	functions for handling of FITS data.
*
*	Last modify:	21/06/94
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<assert.h>

#include	"define.h"
#include	"globals.h"


/******************************* fitsfind ***********************************/
/*
Search for a FITS keyword in a FITS header.
*/
int	fitsfind(char *fitsbuf, char *keyword)

  {
   char	*ptr;
   int	i, len;

  len = strlen(keyword);
  for (i=0; strncmp(ptr=&fitsbuf[80*i], "END     ", 8); i++)
    if (!strncmp(ptr, keyword, len))
      return i;

  if (strncmp(keyword, "END     ", 8))
    return RETURN_ERROR;
  else
    return i;
  }


/********************************* fitsread **********************************/
/*
read a FITS keyword in a fits header.
*/
int	fitsread(char *fitsbuf, char *keyword, void *ptr, h_type type,
		int t_type)

  {
   int		pos;
   static char	s[80];
   char		*str, *st;

  if ((pos = fitsfind(fitsbuf, keyword)) < 0)
    return RETURN_ERROR;

  str = &fitsbuf[80*pos+10];

  switch(type)
    {
    case H_INT:		if (t_type == T_SHORT)
			  sscanf(str, "    %hd", (short *)ptr);
			else
			  sscanf(str, "    %d", (LONG *)ptr);
			break;

    case H_FLOAT:
    case H_EXPO:	if (t_type == T_DOUBLE)
			  sscanf(str, "    %lf", (double *)ptr);
			else
			  sscanf(str, "    %hf", (float *)ptr);
			break;

    case H_BOOL:	sscanf(str, "%1s", s);
			*(int *)ptr = ((int)s[0] == 'T') ? 1 : 0;
			break;

    case H_STRING:	sscanf(str, "'%s'", (char *)ptr);
			if (st=strchr((char *)ptr,'\''))
			  *st = (char)'\0';
			break;

    case H_COMMENT:	sscanf(str, "%s", (char *)ptr);
			break;

    default:		error(EXIT_FAILURE,
				"*Internal Error*: Unknown FITS type in ",
				"fitsread()");
			break;
    }

  return RETURN_OK;
  }


/********************************* fitswrite *********************************/
/*
Write a FITS keyword in a fits header.
*/
int	fitswrite(char *fitsbuf, char *keyword, void *ptr, h_type type,
		int t_type)

  {
   int		i, pos;
   char		str[70];

  if ((pos = fitsfind(fitsbuf, keyword)) < 0)
    return RETURN_ERROR;

  switch(type)
    {
    case H_INT:	sprintf(str, "%20d", (t_type==T_SHORT)?
				*(short *)ptr: *(int *)ptr);
			break;

    case H_FLOAT:	sprintf(str, "        %12.4f", (t_type==T_DOUBLE)?
				*(double *)ptr: *(float *)ptr);
			break;

    case H_EXPO:	sprintf(str, "    %16.9e", (t_type==T_DOUBLE)?
				*(double *)ptr: *(float *)ptr);
			break;

    case H_BOOL:	if (*(int *)ptr)
			  sprintf(str, "                   T");
			else
			  sprintf(str, "                   F");
			break;

    case H_STRING:	sprintf(str, "'%-8.18s'", (char *)ptr);
			break;

    case H_COMMENT:	sprintf(str, "%-69s", (char *)ptr);
			break;

    default:		error(EXIT_FAILURE,
				"*FATAL ERROR*: Unknown FITS type in ",
				"fitswrite()");
			break;
    }

  fitsbuf += 80*pos+10;
  for (i=0; str[i]; i++)
    fitsbuf[i] = str[i];

  return RETURN_OK;
  }


/********************************** fitsadd **********************************/
/*
Write a FITS keyword in a fits header.
*/
int	fitsadd(char *fitsbuf, char *keyword, char *comment)

  {
   char    *key_ptr, str[81];
   int     i, headpos;

  if ((headpos = fitsfind(fitsbuf, keyword)) < 0)
    {
    headpos = fitsfind(fitsbuf, "END     ");
    key_ptr = &fitsbuf[80*headpos];
    for (i=0; i<80; i++)
      key_ptr[i+80] = key_ptr[i];

    sprintf(str, "%-8.8s=                      / %-47.47s",
	keyword, comment?comment:" ");

    for (i=0; i<80; i++)
      key_ptr[i] = str[i];
    }

  return headpos;
  }

