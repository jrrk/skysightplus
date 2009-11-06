 /*
 				prefs.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	Keywords for the configuration file.
*
*	Last modify:	07/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

keystruct key[] =
 {
  {"CATALOG_NAME", P_STRING, prefs.cat_name},
  {"CATALOG_TYPE", P_KEY, &prefs.cat_type, 0,0, 0.0,0.0,
   {"ASCII","FITS",""}},
  {"PARAMETERS_NAME", P_STRING, prefs.param_name},
  {"THRESHOLD_TYPE", P_KEY, &prefs.threshold_type, 0,0, 0.0,0.0,
   {"SIGMA", "MAGNITUDE",""}},
  {"THRESHOLD", P_FLOAT, &prefs.threshold, 0,0, -100.0, 100.0},
  {"DETECTION_TYPE", P_KEY, &prefs.detect_type, 0,0, 0.0,0.0,
   {"CCD","PHOTO",""}},
  {"CONVOLVE", P_BOOL, &prefs.conv_flag},
  {"CONVOLVE_NAME", P_STRING, prefs.conv_name},
  {"CONVOLVE_NORM", P_BOOL, &prefs.convnorm_flag},
  {"DEBLEND_NTHRESH", P_INT, &prefs.deblend_nthresh, 1,64},
  {"DEBLEND_MINCONT", P_FLOAT, &prefs.deblend_mincont, 0,0, 0.0,1.0},
  {"EXTRACT_MINAREA", P_INT, &prefs.ext_minarea, 1,1000000},
  {"CLEAN", P_BOOL, &prefs.clean_flag},
  {"CLEAN_PARAM", P_FLOAT, &prefs.clean_param, 0,0, 0.0,100.0},
  {"CLEAN_OBJSTACK", P_INT, &prefs.clean_stacksize, 16,65536},
  {"PHOTOM_APERTURE", P_INT, &prefs.apert, 1,1000000},
  {"PHOTOM_KPAR", P_FLOAT, &prefs.kron_fact, 0,0, 0.0,10.0},
  {"PHOTOM_KSIG", P_FLOAT, &prefs.kron_nsig, 0,0, 1.0,16.0},
  {"PHOTOM_KMINSIG", P_FLOAT, &prefs.kron_minsig, 0,0, 1.0,16.0},
  {"BACK_XSIZE", P_INT, &prefs.backx, 8,1024},
  {"BACK_YSIZE", P_INT, &prefs.backy, 8,1024},
  {"BACK_FLTRXSIZE", P_INT, &prefs.backfx, 1,7},
  {"BACK_FLTRYSIZE", P_INT, &prefs.backfy, 1,7},
  {"BACKPHOTO_TYPE", P_KEY, &prefs.pback_type, 0,0, 0.0,0.0,
   {"GLOBAL","LOCAL",""}},
  {"BACKPHOTO_THICK", P_INT, &prefs.pback_size, 1, 256},
  {"MEMORY_PIXSTACK", P_INT, &prefs.mem_pixstack, 1000, 10000000},
  {"MEMORY_BUFSIZE", P_INT, &prefs.mem_bufsize, 8, 65534},
  {"SCAN_ISOAPRATIO", P_FLOAT, &prefs.scan_isoapratio, 0,0, 0.0,1.0},
  {"VERBOSE_TYPE", P_KEY, &prefs.verbose_type, 0,0, 0.0,0.0,
   {"QUIET","NORMAL","FULL",""}},
  {"MAG_ZEROPOINT", P_FLOAT, &prefs.mag_zeropoint, 0,0, -100.0, 100.0},
  {"SATUR_LEVEL", P_FLOAT, &prefs.satur_level, 0,0, -1e+30, 1e+30},
  {"MAG_GAMMA", P_FLOAT, &prefs.mag_gamma, 0,0, 1e-10,1e+30},
  {"CHECKIMAGE_TYPE", P_KEY, &prefs.check_type, 0,0, 0.0,0.0,
   {"NONE", "BACKGROUND", "-BACKGROUND", "CONVOLVED", "OBJECTS",
	"APERTURES", "SEGMENTATION", ""}},
  {"CHECKIMAGE_NAME", P_STRING, prefs.check_name},
  {"GAIN", P_FLOAT, &prefs.gain, 0,0, 0.0, 1e+30},
  {"PIXEL_SCALE", P_FLOAT, &prefs.pixel_scale, 0,0, 1e-10, 1e+10},
  {"SEEING_FWHM", P_FLOAT, &prefs.seeing_fwhm, 0,0, 1e-10, 1e+10},
  {"STARNNW_NAME", P_STRING, prefs.nnw_name},
  {""}
 };

char		keylist[sizeof(key)/sizeof(keystruct)][16];
