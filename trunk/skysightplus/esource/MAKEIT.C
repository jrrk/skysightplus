/*
 				makeit.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	main program.
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

/******************************** makeit *************************************/
/*
Manage the whole stuff.
*/
void	makeit()

  {
  field.filename = prefs.image_name;
  NFPRINTF(OUTPUT, "Reading image header");
  readimagehead();		/* read the image FITS header */
  initastrom();			/* initialize astrom. parameters */
  useprefs();			/* update things accor. to prefs parameters */

  if (prefs.verbose_type != QUIET)
    fprintf(OUTPUT, "Frame: \"%.20s\" / %d x %d / %d bits %s data\n",
	field.ident,
	field.width, field.height, field.bytepix*8,
	field.bitpix>0? "INTEGER":"FLOATING POINT");

  NFPRINTF(OUTPUT, "Setting catalog parameters");
  readcatparams(prefs.param_name);

  if (prefs.conv_flag)
    {
    NFPRINTF(OUTPUT, "Reading convolution mask");
    getconv();			/* get the convolution mask if asked for */
    }
  else
    field.conv = NULL;

  if (FLAG(obj2.sprob))
    {
    NFPRINTF(OUTPUT, "Initializing Neural Network");
    neurinit();
    NFPRINTF(OUTPUT, "Reading Neural Network Weights");
    getnnw(); 
    }

  QMALLOC(bufdata, char, DATA_BUFSIZE);

  NFPRINTF(OUTPUT, "Setting up background map");
  makeback();			/* make the background map from the image */

  NFPRINTF(OUTPUT, "Filtering background map");
  filterback();			/* apply a median filtering to the image */

  if (prefs.threshold_type == T_MAGNITUDE)	/* compute the threshold */
    field.thresh = field.pixscale*field.pixscale
		*pow(10.0, -0.4*(prefs.threshold-prefs.mag_zeropoint));
  else
    field.thresh = prefs.threshold*field.backsig;

  if (prefs.verbose_type != QUIET)
    fprintf(OUTPUT, "Background: %-10g RMS: %-10g / Threshold: %-10g \n",
	field.backmean, field.backsig, field.thresh);

/*Let's test a few things to avoid problems later */

  if (field.thresh<=0.0)
    error(EXIT_FAILURE,
	"*Error*: I cannot deal with zero or negative thresholds!", "");

  if (prefs.detect_type == PHOTO
	&& field.backmean+3*field.backsig > 50*field.ngamma)
    error(EXIT_FAILURE,
	"*Error*: The density range of this image is too large for ",
	"PHOTO mode");

  NFPRINTF(OUTPUT, "Initializing catalog");
  initcat();

  if (prefs.check_type != CNONE)/* open the check-imagefile */
   {
   NFPRINTF(OUTPUT, "Initializing check-image");
   initcheck();
   }

  NFPRINTF(OUTPUT, "Scanning image");
  scanimage();			/* extraction pipeline */

  NFPRINTF(OUTPUT, "Closing files");
  if (prefs.check_type != CNONE)
    closecheck();
  fclose(field.file);
  closecat();

  free(bufdata);		/* free some memory */
  QFREE(field.conv);
  QFREE(field.back);
  /* QFREE(field.strip); */

  if (FLAG(obj2.sprob))
    neurclose();

  if (prefs.verbose_type != QUIET)
    fprintf(OUTPUT, "Objects: detected %-8d / sextracted %-8d\n",
	field.ndetect, field.nfinal);
  return;
  }

