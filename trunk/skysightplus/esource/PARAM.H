/*
 				param.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN, IAP/ESO.
*
*	Contents:	parameter list for catalog data.
*
*	Last modify:	30/03/95
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

objstruct	outobj;
obj2struct	outobj2;

paramstruct	param[] = {
	{"NUMBER", T_LONG, &outobj.number, "%5d "},
	{"MAG_ISO", T_FLOAT, &outobj2.isomag, "%8.4f "},
	{"MAGERR_ISO", T_FLOAT, &outobj2.sigmaisomag, "%8.4f "},
	{"MAG_ISOCOR", T_FLOAT, &outobj2.isocormag, "%8.4f "},
	{"MAGERR_ISOCOR", T_FLOAT, &outobj2.sigmaisocormag, "%8.4f "},
	{"MAG_APER", T_FLOAT, &outobj.apermag, "%8.4f "},
	{"MAGERR_APER", T_FLOAT, &outobj.sigmaapermag, "%8.4f "},
	{"MAG_AUTO", T_FLOAT, &outobj.automag, "%8.4f "},
	{"MAGERR_AUTO", T_FLOAT, &outobj.sigmaautomag, "%8.4f "},
	{"MAG_BEST", T_FLOAT, &outobj2.mag, "%8.4f "},
	{"MAGERR_BEST", T_FLOAT, &outobj2.sigmag, "%8.4f "},
	{"KRON_RADIUS", T_FLOAT, &outobj.kronfactor, "%5.2f "},
	{"BACKGROUND", T_FLOAT, &outobj.bkg, "%-13g "},
	{"THRESHOLD", T_FLOAT, &outobj.thresh, "%-13g "},
	{"FLUX_MAX", T_FLOAT, &outobj.maxflux, "%-13g "},
	{"ISOAREA_IMAGE", T_LONG, &outobj.scannb, "%9d "},
	{"X_IMAGE", T_FLOAT, &outobj.mx, "%8.2f "},
	{"Y_IMAGE", T_FLOAT, &outobj.my, "%8.2f "},
	{"A_IMAGE", T_FLOAT, &outobj.a, "%8.2f "},
	{"B_IMAGE", T_FLOAT, &outobj.b, "%8.2f "},
	{"THETA_IMAGE", T_FLOAT, &outobj.theta, "%5.1f "},
	{"MU_THRESHOLD", T_FLOAT, &outobj2.threshmu, "%8.4f "},
	{"MU_MAX", T_FLOAT, &outobj2.maxmu, "%8.4f "},
	{"ISOAREA_WORLD", T_FLOAT, &outobj2.scannbw, "%-13g "},
	{"X_WORLD", T_DOUBLE, &outobj2.mxw, "%-15e "},
	{"Y_WORLD", T_DOUBLE, &outobj2.myw, "%-15e "},
	{"A_WORLD", T_FLOAT, &outobj2.aw, "%-13g "},
	{"B_WORLD", T_FLOAT, &outobj2.bw, "%-13g "},
	{"THETA_WORLD", T_FLOAT, &outobj2.thetaw, "%5.1f "},
	{"ISO0", T_LONG, &outobj.iso[0], "%8d "},
	{"ISO1", T_LONG, &outobj.iso[1], "%8d "},
	{"ISO2", T_LONG, &outobj.iso[2], "%8d "},
	{"ISO3", T_LONG, &outobj.iso[3], "%8d "},
	{"ISO4", T_LONG, &outobj.iso[4], "%8d "},
	{"ISO5", T_LONG, &outobj.iso[5], "%8d "},
	{"ISO6", T_LONG, &outobj.iso[6], "%8d "},
	{"ISO7", T_LONG, &outobj.iso[7], "%8d "},
	{"FLAGS", T_SHORT, &outobj.flag, "%03d "},
	{"MAMA_ALPHA", T_DOUBLE, &outobj2.alpha, "%9.5f "},
	{"MAMA_DELTA", T_DOUBLE, &outobj2.delta, "%9.5f "},
	{"FWHM_IMAGE", T_FLOAT, &outobj.fwhm, "%8.2f "},
	{"FWHM_WORLD", T_FLOAT, &outobj2.fwhmw, "%-13g "},
	{"ELONGATION", T_FLOAT, &outobj2.e, "%8.3f "},
	{"CLASS_STAR", T_FLOAT, &outobj2.sprob, "%5.2f "},
	{""}
	};

paramstruct	hparam[] = {
	{"OBJECT  ", 0, field.rident, "%-18s", H_STRING},
	{"EPOCH   ", T_FLOAT, &field.epoch, "%7.2f", H_FLOAT},
	{"CRVAL1  ", T_DOUBLE, &field.crvalx, "%-15g", H_EXPO},
	{"CRVAL2  ", T_DOUBLE, &field.crvaly, "%-15g", H_EXPO},
	{"CRPIX1  ", T_LONG, &field.crpixx, "%5d", H_INT},
	{"CRPIX2  ", T_LONG, &field.crpixy, "%5d", H_INT},
	{"CDELT1  ", T_DOUBLE, &field.cdeltx, "%-15g", H_EXPO},
	{"CDELT2  ", T_DOUBLE, &field.cdelty, "%-15g", H_EXPO},
	{"CROTA1  ", T_DOUBLE, &field.crotax, "%-15g", H_EXPO},
	{"CROTA2  ", T_DOUBLE, &field.crotay, "%-15g", H_EXPO},
	{"SEXIMASX", T_LONG, &field.width, "%5d", H_INT},
	{"SEXIMASY", T_LONG, &field.height,"%5d", H_INT},
	{"SEXSTRSY", T_LONG, &field.stripheight, "%5d", H_INT},
	{"SEXIMABP", T_LONG, &field.bitpix, "%3d", H_INT},
	{"SEXPIXS ", T_DOUBLE, &field.pixscale, "%-15g", H_EXPO},
	{"SEXSFWHM", T_DOUBLE, &prefs.seeing_fwhm, "%-13g", H_EXPO},
	{"SEXNNWF ", 0, prefs.nnw_rname, "%-18s", H_STRING},
	{"SEXGAIN ", T_DOUBLE, &prefs.gain, "%6.2f", H_EXPO},
	{"SEXBKGND", T_FLOAT, &field.backmean, "%-13g", H_EXPO},
	{"SEXBKDEV", T_FLOAT, &field.backsig, "%-13g", H_EXPO},
	{"SEXBKTHD", T_DOUBLE, &field.thresh, "%-15g", H_EXPO},
	{"SEXCONFF", 0, prefs.prefs_rname, "%-18s", H_STRING},
	{"SEXDETT ", 0, &prefs.detect_type, "%-18s", H_KEY},
	{"SEXTHLDT", 0, &prefs.threshold_type, "-18s", H_KEY},
	{"SEXTHLD ", T_DOUBLE, &prefs.threshold, "%-13g", H_EXPO},
	{"SEXMINAR", T_LONG, &prefs.ext_minarea, "%5d", H_INT},
	{"SEXCONV ", T_LONG, &prefs.conv_flag, "%1s", H_BOOL},
	{"SEXCONVN", T_LONG, &prefs.convnorm_flag, "%1s", H_BOOL},
	{"SEXCONVF", 0, prefs.conv_rname, "%-18s", H_STRING},
	{"SEXDBLDN", T_LONG, &prefs.deblend_nthresh, "%3d", H_INT},
	{"SEXDBLDC", T_DOUBLE, &prefs.deblend_mincont, "%8f", H_FLOAT},
	{"SEXCLN  ", T_LONG, &prefs.clean_flag, "%1s", H_BOOL},
	{"SEXCLNPA", T_DOUBLE, &prefs.clean_param, "%5.2f", H_FLOAT},
	{"SEXCLNST", T_LONG, &prefs.clean_stacksize, "%6d", H_INT},
	{"SEXAPERD", T_LONG, &prefs.apert, "%5d", H_INT},
	{"SEXAPEK1", T_DOUBLE, &prefs.kron_fact, "%4.1f", H_FLOAT},
	{"SEXAPEK2", T_DOUBLE, &prefs.kron_nsig, "%4.1f", H_FLOAT},
	{"SEXAPEK3", T_DOUBLE, &prefs.kron_minsig, "%4.1f", H_FLOAT},
	{"SEXSATLV", T_DOUBLE, &prefs.satur_level, "%-13g", H_EXPO},
	{"SEXMGZPT", T_DOUBLE, &prefs.mag_zeropoint, "%8.4f", H_FLOAT},
	{"SEXMGGAM", T_DOUBLE, &prefs.mag_gamma, "%4.2f", H_FLOAT},
	{"SEXBKGSX", T_LONG, &field.backw, "%5d", H_INT},
	{"SEXBKGSY", T_LONG, &field.backh, "%5d", H_INT},
	{"SEXBKGFX", T_LONG, &field.nbackfx, "%3d", H_INT},
	{"SEXBKGFY", T_LONG, &field.nbackfy, "%3d", H_INT},
	{"SEXPBKGT", 0, &prefs.pback_type, "-18s", H_KEY},
	{"SEXPBKGS", T_LONG, &prefs.pback_size, "%3d", H_INT},
	{"SEXPIXSK", T_LONG, &prefs.mem_pixstack, "%8d", H_INT},
	{"SEXFBUFS", T_LONG, &prefs.mem_bufsize, "%5d", H_INT},
	{"SEXISAPR", T_DOUBLE, &prefs.scan_isoapratio, "%4.2f", H_FLOAT},
	{"SEXNDET ", T_LONG, &field.ndetect, "%9d", H_INT},
	{"SEXNFIN ", T_LONG, &field.nfinal, "%9d", H_INT},
	{"SEXNPARA", T_LONG, &cat.nparam, "%3d", H_INT},
	{""}
	};

char    paramlist[sizeof(param)/sizeof(paramstruct)][16];
int	t_size[] = {1, 2, 4, 4, 8};
char	t_form[][4] = {"1B", "1I", "1J", "1E", "1D"};
