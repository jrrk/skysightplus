/*** COPYRIGHT NOTICE AND PUBLIC SOURCE LICENSE ***************************************

	Portions Copyright (c) 1992-2001 Southern Stars Systems.  All Rights Reserved.

	This file contains Original Code and/or Modifications of Original Code as
	defined in and that are subject to the Southern Stars Systems Public Source
	License Version 1.0 (the 'License').  You may not use this file except in
	compliance with the License.  Please obtain a copy of the License at

	http://www.southernstars.com/opensource/

	and read it before using this file.

	The Original Code and all software distributed under the License are distributed
	on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, AND
	SOUTHERN STARS SYSTEMS HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT
	LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
	QUIET ENJOYMENT, OR NON-INFRINGEMENT.  Please see the License for the specific
	language governing rights and limitations under the License.

	CONTRIBUTORS:

	TCD - Tim DeBenedictis (timmyd@southernstars.com)

	MODIFICATION HISTORY:

	1.0.0 - 09 Apr 2001 - TCD - Original Code.

****************************************************************************************/

/*** Header files ***/

#include "GUILib.h"
#include "AstroLib.h"
#include "CameraInterface.h"

/*** Window resource #defines ***/

#define MAIN_WINDOW					256
#define IMAGE_WINDOW				257
#define HEADER_WINDOW				258
#define HISTOGRAM_WINDOW			259

/*** Menu resource #defines ***/

#define APPLE_MENU					0
#define FILE_MENU					1
#define EDIT_MENU					2
#define DISPLAY_MENU				3
#define PROCESS_MENU				4
#define ANALYZE_MENU				5
#define WINDOW_MENU					6
#define HELP_MENU					7

#define COLOR_TABLE_MENU_ID			128
#define COLOR_TABLE_GRAYSCALE		1
#define COLOR_TABLE_RAINBOW			2
#define COLOR_TABLE_FLAME			3
#define COLOR_TABLE_WINTER			4
#define COLOR_TABLE_SPRING			5
#define COLOR_TABLE_SUMMER			6
#define COLOR_TABLE_FALL			7
#define COLOR_TABLE_RED				8		
#define COLOR_TABLE_GREEN			9
#define COLOR_TABLE_BLUE			10

#define MAGNIFICATION_MENU_ID		129
#define MAGNIFICATION_16X_ITEM		1
#define MAGNIFICATION_8X_ITEM		2
#define MAGNIFICATION_4X_ITEM		3
#define MAGNIFICATION_2X_ITEM		4
#define MAGNIFICATION_1X_ITEM		5
#define MAGNIFICATION_1_2X_ITEM		6
#define MAGNIFICATION_1_4X_ITEM		7
#define MAGNIFICATION_1_8X_ITEM		8
#define MAGNIFICATION_1_16X_ITEM	9

#define ADD_MENU_ID					130
#define SUBTRACT_MENU_ID			131
#define MULTIPLY_MENU_ID			132
#define DIVIDE_MENU_ID				133
#define ARITHMETIC_CONSTANT_ITEM	1
#define ARITHMETIC_IMAGE_SEPARATOR	2

#define ROTATE_MENU_ID				134
#define ROTATE_90_DEGREES			1
#define ROTATE_180_DEGREES			2
#define ROTATE_270_DEGREES			3
#define ROTATE_OTHER				4

#define CONVOLVE_MENU_ID			135
#define CONVOLVE_SMOOTH_SOFT		1
#define CONVOLVE_SMOOTH_HARD		2
#define CONVOLVE_SHARPEN_SOFT		3
#define CONVOLVE_SHARPEN_HARD		4
#define CONVOLVE_GRADIENT			5

#define COLOR_FRAME_MENU_ID			136
#define COLOR_FRAME_ALL				1
#define COLOR_FRAME_RED				2
#define COLOR_FRAME_GREEN			3
#define COLOR_FRAME_BLUE			4

#define MOUSE_COORDINATES_MENU_ID	137
#define MOUSE_COORDINATES_NONE		1
#define MOUSE_COORDINATES_COL_ROW	2
#define MOUSE_COORDINATES_X_Y		3
#define MOUSE_COORDINATES_RA_DEC	4

#define EDIT_COLOR_FRAME_MENU_ID	138

#define ALIGN_MENU_ID				139

#define MOSAIC_MENU_ID				140

/*** items for this menu same as "Color Frame" menu. ***/

#define APPLE_MENU_ID				256
#define APPLE_ABOUT_ITEM			1

#define FILE_MENU_ID				257
#define FILE_NEW_ITEM				1
#define FILE_OPEN_ITEM				2
#define FILE_CLOSE_ITEM				3
#define FILE_SAVE_ITEM				5
#define FILE_SAVE_AS_ITEM			6
#define FILE_PAGE_SETUP_ITEM		8
#define FILE_PRINT_ITEM				9
#define FILE_QUIT_ITEM				11

#define EDIT_MENU_ID				258
#define EDIT_UNDO_ITEM				1
#define EDIT_CUT_ITEM				3
#define EDIT_COPY_ITEM				4
#define EDIT_PASTE_ITEM				5
#define EDIT_CLEAR_ITEM				6
#define EDIT_DELETE_ITEM			6
#define EDIT_SELECT_ALL_ITEM		8
#define EDIT_SELECT_NONE_ITEM		9
#define EDIT_COLOR_FRAME_ITEM		11
#define EDIT_SELECT_TOOL			13
#define EDIT_DRAG_TOOL				14
#define EDIT_ZOOM_IN_TOOL			15
#define EDIT_ZOOM_OUT_TOOL			16
#define EDIT_SHIFT_TOOL				17
#define EDIT_MARK_TOOL				18

#define DISPLAY_MENU_ID					259
#define DISPLAY_MAGNIFICATION_ITEM		1
#define DISPLAY_ZOOM_IN_ITEM			2
#define DISPLAY_ZOOM_OUT_ITEM			3
#define DISPLAY_CROP_ITEM				4
#define DISPLAY_COLOR_TABLE_ITEM		6
#define DISPLAY_COLOR_FRAME_ITEM		7
#define DISPLAY_MOUSE_COORDINATES_ITEM	9
#define DISPLAY_OBJECTS_ITEM			11
#define DISPLAY_OBJECT_CENTROIDS_ITEM	12
#define DISPLAY_OBJECT_NAMES_ITEM		13
#define DISPLAY_FITS_HEADER_ITEM		15
#define DISPLAY_HISTOGRAM_ITEM			16

#define PROCESS_MENU_ID					260
#define PROCESS_ADD_ITEM				1
#define PROCESS_SUBTRACT_ITEM			2
#define PROCESS_MULTIPLY_ITEM			3
#define PROCESS_DIVIDE_ITEM				4
#define PROCESS_FLIP_HORIZONTALLY_ITEM	6
#define PROCESS_FLIP_VERTICALLY_ITEM	7
#define PROCESS_TRIM_ITEM				8
#define PROCESS_SHIFT_ITEM				9
#define PROCESS_SCALE_ITEM				10
#define PROCESS_ROTATE_ITEM				11
#define PROCESS_CONVOLVE_ITEM			12
#define PROCESS_INVERT_ITEM				13
#define PROCESS_CLIP_ITEM				14
#define PROCESS_ALIGN_ITEM				16
#define PROCESS_MOSAIC_ITEM				17
#define PROCESS_RGB_COMBINE_ITEM		19
#define PROCESS_RGB_SEPARATE_ITEM		20
#define PROCESS_RGB_BALANCE_ITEM		21

#define ANALYZE_MENU_ID					261
#define ANALYZE_DEFINE_OBJECT_PSF		1
#define ANALYZE_FIND_OBJECTS			2
#define ANALYZE_DELETE_OBJECTS			3

#ifdef GMAC

#define WINDOW_MENU_ID				262
#define WINDOW_TILE_ITEM			1
#define WINDOW_CASCADE_ITEM			2
#define WINDOW_ARRANGE_ITEM			0
#define WINDOW_CONTROL_ITEM			4
#define WINDOW_WINDOW_ITEM			6

#define HELP_MENU_ID				263

#else

#define WINDOW_MENU_ID				262
#define WINDOW_TILE_ITEM			1
#define WINDOW_CASCADE_ITEM			2
#define WINDOW_ARRANGE_ITEM			3
#define WINDOW_CONTROL_ITEM			5
#define WINDOW_WINDOW_ITEM			7

#define HELP_MENU_ID				263
#define HELP_ABOUT_ITEM				1

#endif

/*** Dialog resource #defines ***/

#define ABOUT_DIALOG						256
#define DISPLAY_DIALOG						257
#define RGB_COMBINATION_DIALOG				259
#define RGB_BALANCE_DIALOG					260
#define TRIM_DIALOG							261
#define SCALE_DIALOG						262
#define SHIFT_DIALOG						263
#define CLIP_DIALOG							264

#define IMAGE_DISPLAY_DIALOG				300
#define CAMERA_CONTROL_DIALOG				200
#define CAMERA_SETUP_PAGE_DIALOG			201
#define CAMERA_TEMPERATURE_PAGE_DIALOG		202
#define CAMERA_EXPOSURE_PAGE_DIALOG			203
#define CAMERA_DISPLAY_PAGE_DIALOG			204
#define CAMERA_ANALYSIS_PAGE_DIALOG			205

#define MULTIPLE_EXPOSURE_DIALOG			211

/*** String resource #defines ***/

#define APPLICATION_TITLE_STRING	256
#define WINDOW_TITLE_STRING			257

#define CANT_ALLOCATE_MEMORY_STRING	300
#define CANT_OPEN_FILE_STRING		301
#define CANT_READ_FILE_STRING		302
#define CANT_WRITE_FILE_STRING		303

#define CCD_DRIVER_ERROR_STRING		310
#define OPEN_SHUTTER_STRING			311
#define CLOSE_SHUTTER_STRING		312

#define SAVE_CHANGES_STRING			350

#define OPEN_FILE_PROMPT_STRING		400
#define OPEN_FILE_FILTER_STRING		401

#define SAVE_IMAGE_PROMPT_STRING		500
#define SAVE_IMAGE_FILTER_STRING		501

#define SAVE_HISTOGRAM_PROMPT_STRING	510
#define SAVE_HISTOGRAM_FILTER_STRING	511

/*** File type identifier #defines ***/

#define FILE_TYPE_UNKNOWN				0
#define FILE_TYPE_FITS					1
#define FILE_TYPE_GIF					2
#define FILE_TYPE_JPEG					3
#define FILE_TYPE_TIFF					4
#define FILE_TYPE_TIFF_16_BIT			5
#define FILE_TYPE_PICT					6
#define FILE_TYPE_BMP					7
#define FILE_TYPE_TEXT					8

/*** Camera #defines ***/

#define EXPOSURE_MODE_FOCUS				1
#define EXPOSURE_MODE_IMAGE				2
#define EXPOSURE_MODE_COLOR_IMAGE		3
#define EXPOSURE_MODE_MULTIPLE_IMAGE	4
#define EXPOSURE_MODE_COMBINED_IMAGE	5

#define DARK_FRAME_NEVER				1
#define DARK_FRAME_ONLY					2
#define DARK_FRAME_ALWAYS				3
#define DARK_FRAME_AUTOMATIC			4

#define CAMERA_STATUS_WAITING			0
#define CAMERA_STATUS_EXPOSING			1
#define CAMERA_STATUS_DOWNLOADING		2

/*** Image #defines ***/

#define IMAGE_TYPE_MONOCHROME			1
#define IMAGE_TYPE_RGB_COLOR			2
#define IMAGE_TYPE_COMPLEX				3

/*** Image frame #defines ***/

#define ALL_FRAMES						-1
#define RED_FRAME						0
#define GREEN_FRAME						1
#define BLUE_FRAME						2

/*** Image region #defines ***/

#define IMAGE_REGION_MAX_PARAMS			16

#define IMAGE_REGION_TYPE_LINEAR		1
#define IMAGE_REGION_TYPE_RECTANGULAR	2
#define IMAGE_REGION_TYPE_ELLIPTICAL	3

#define IMAGE_REGION_PARAM_N			1
#define IMAGE_REGION_PARAM_SUM			2
#define IMAGE_REGION_PARAM_MIN			3
#define IMAGE_REGION_PARAM_MIN_COL		4
#define IMAGE_REGION_PARAM_MIN_ROW		5
#define IMAGE_REGION_PARAM_MAX			6
#define IMAGE_REGION_PARAM_MAX_COL		7
#define IMAGE_REGION_PARAM_MAX_ROW		8
#define IMAGE_REGION_PARAM_MEAN			9
#define IMAGE_REGION_PARAM_BACKGROUND	10
#define IMAGE_REGION_PARAM_AMPLITUDE	11
#define IMAGE_REGION_PARAM_CENTROID_X	12
#define IMAGE_REGION_PARAM_CENTROID_Y	13
#define IMAGE_REGION_PARAM_RADIUS		14

#define GetImageRegionNumPixels(r)		(long) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_N )
#define GetImageRegionSum(r)			(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_SUM )
#define GetImageRegionMin(r)			(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_MIN )
#define GetImageRegionMinCol(r)			(short) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_MIN_COL )
#define GetImageRegionMinRow(r)			(short) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_MIN_ROW )
#define GetImageRegionMax(r)			(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_MAX )
#define GetImageRegionMaxCol(r)			(short) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_MAX_COL )
#define GetImageRegionMaxRow(r)			(short) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_MAX_ROW )
#define GetImageRegionMean(r)			(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_MEAN )
#define GetImageRegionStdDev(r)			(double) GetImageRegionError ( r, IMAGE_REGION_PARAM_MEAN )
#define GetImageObjectBackground(r)		(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_BACKGROUND )
#define GetImageObjectAmplitude(r)		(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_AMPLITUDE )
#define GetImageObjectCentroidX(r)		(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_CENTROID_X )
#define GetImageObjectCentroidY(r)		(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_CENTROID_Y )
#define GetImageObjectRadius(r)			(double) GetImageRegionParameter ( r, IMAGE_REGION_PARAM_RADIUS )

#define SetImageRegionNumPixels(r,x)	SetImageRegionParameter ( r, IMAGE_REGION_PARAM_N, x )
#define SetImageRegionSum(r,x)			SetImageRegionParameter ( r, IMAGE_REGION_PARAM_SUM, x )
#define SetImageRegionMin(r,x)			SetImageRegionParameter ( r, IMAGE_REGION_PARAM_MIN, x )
#define SetImageRegionMinCol(r,x)		SetImageRegionParameter ( r, IMAGE_REGION_PARAM_MIN_COL, x )
#define SetImageRegionMinRow(r,x)		SetImageRegionParameter ( r, IMAGE_REGION_PARAM_MIN_ROW, x )
#define SetImageRegionMax(r,x)			SetImageRegionParameter ( r, IMAGE_REGION_PARAM_MAX, x )
#define SetImageRegionMaxCol(r,x)		SetImageRegionParameter ( r, IMAGE_REGION_PARAM_MAX_COL, x )
#define SetImageRegionMaxRow(r,x)		SetImageRegionParameter ( r, IMAGE_REGION_PARAM_MAX_ROW, x )
#define SetImageRegionMean(r,x)			SetImageRegionParameter ( r, IMAGE_REGION_PARAM_MEAN, x )
#define SetImageRegionStdDev(r,x)		SetImageRegionError ( r, IMAGE_REGION_PARAM_MEAN, x )
#define SetImageObjectBackground(r,x)	SetImageRegionParameter ( r, IMAGE_REGION_PARAM_BACKGROUND, x )
#define SetImageObjectAmplitude(r,x)	SetImageRegionParameter ( r, IMAGE_REGION_PARAM_AMPLITUDE, x )
#define SetImageObjectCentroidX(r,x)	SetImageRegionParameter ( r, IMAGE_REGION_PARAM_CENTROID_X, x )
#define SetImageObjectCentroidY(r,x)	SetImageRegionParameter ( r, IMAGE_REGION_PARAM_CENTROID_Y, x )
#define SetImageObjectRadius(r,x)		SetImageRegionParameter ( r, IMAGE_REGION_PARAM_RADIUS, x )

/*** Image window object display #defines ***/

#define IMAGE_WINDOW_DISPLAY_OBJECTS			0x0001
#define IMAGE_WINDOW_DISPLAY_OBJECT_CENTROIDS	0x0002
#define IMAGE_WINDOW_DISPLAY_OBJECT_RADII		0x0004
#define IMAGE_WINDOW_DISPLAY_OBJECT_NAMES		0x0008
#define IMAGE_WINDOW_DISPLAY_OBJECT_MAGNITUDES	0x0010

#define GetImageWindowDisplayObjects(w)				GetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECTS )
#define GetImageWindowDisplayObjectCentroids(w)		GetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECT_CENTROIDS )
#define GetImageWindowDisplayObjectRadii(w)			GetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECT_RADII )
#define GetImageWindowDisplayObjectNames(w)			GetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECT_NAMES )
#define GetImageWindowDisplayObjectMagnitudes(w)	GetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECT_MAGNITUDES )

#define SetImageWindowDisplayObjects(w,d)			SetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECTS, d )
#define SetImageWindowDisplayObjectCentroids(w,d)	SetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECT_CENTROIDS, d )
#define SetImageWindowDisplayObjectRadii(w,d)		SetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECT_RADII, d )
#define SetImageWindowDisplayObjectNames(w,d)		SetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECT_NAMES, d )
#define SetImageWindowDisplayObjectMagnitudes(w,d)	SetImageWindowObjectDisplayFlag ( w, IMAGE_WINDOW_DISPLAY_OBJECT_MAGNITUDES, d )

/*** Miscellaneous #defines ***/

#define BRIGHTNESS_CONTRAST_CONTROL		0

#define DISPLAY_MODE_MANUAL				1
#define DISPLAY_MODE_AUTOMATIC			0

#define SKYSIGHT_FILE_CREATOR			"VIST"

/*** Data types ***/

typedef struct FITSImage		*FITSImagePtr;
typedef struct Image			Image, *ImagePtr;
typedef struct ImageRegion		ImageRegion, ImageObject, *ImageRegionPtr, *ImageObjectPtr;
typedef struct ImageHistogram	ImageHistogram, *ImageHistogramPtr;
typedef struct Exposure			Exposure, *ExposurePtr, *ExposureList;
typedef struct Camera			Camera, *CameraPtr;

typedef void	(*ImageSurfaceFunctionPtr)	( short, short, double * );
typedef double	(*ImageModelFunctionPtr)	( short, short, double *, double * );

/*** Functions in Utilities.c ***/

void		EnableMessages ( int );
int			MessagesEnabled ( void );

int			InfoMessage ( short, short, ... );
int			WarningMessage ( short, short, ... );
int			ErrorMessage ( short, short, ... );

GWindowPtr	ShowProgressDialog ( int, long );
void 		StartProgressDialogTask ( char *, long, long );
short		UpdateProgressDialogTask ( long );
void		HideProgressDialog ( void );

/*** Functions in Main.c ***/

/*** Functions in Menus.c ***/

void	DoMenuItem ( long, long );
void	UpdateMenus ( void );
void	UpdateColorTableMenu ( GWindowPtr );
void	UpdateColorFrameMenu ( GWindowPtr );
void	BuildImageMenus ( void );
void	AddImageMenuItems ( GMenuPtr, short, short );
void	BuildImagePopupMenu ( GControlPtr, GWindowPtr ); 

/*** Functions in AppleMenu.c ***/

int			DoAppleMenuItem ( short );
int			DoAboutDialogEvent ( short, GWindowPtr, long, long );
GWindowPtr	CreateSplashScreen ( void );
void 		DeleteSplashScreen ( GWindowPtr );
void		ShowSplashScreen ( long );

/*** Functions in FileMenu.c ***/

void	DoFileMenuItem ( long );
void	DoNew ( void );
void	DoOpenDialog ( void );
void	DoClose ( void );
void	DoSave ( GWindowPtr );
void	DoSaveAsDialog ( GWindowPtr );
void	DoSaveImageDialog ( GWindowPtr );
void	DoSaveHistogramDialog ( GWindowPtr );
void	DoPageSetupDialog ( void );
void	DoPrintDialog ( GWindowPtr );
void	DoQuit ( void );

/*** Functions in EditMenu.c ***/

void	DoEditMenuItem ( long );
void	DoEditColorFrameMenuItem ( long );
void	SetImageTool ( short );
short	GetImageTool ( void );

GWindowPtr	GetUndoWindow ( void );
int			PrepareUndo ( GWindowPtr );
void		DoUndo ( void );
void		DoCopy ( GWindowPtr );

/*** Functions in DisplayMenu.c ***/

void	DoDisplayMenuItem ( long );
void	DoMagnificationMenuItem ( long );
void	DoColorTableMenuItem ( long );
void	DoColorFrameMenuItem ( long );
void	DoMouseCoordinatesMenuItem ( long );
void	ZoomImageWindow ( long );

/*** Functions in ProcessMenu.c ***/

void	DoProcessMenuItem ( long );
void	DoImageArithmeticMenuItem ( long, long );
void	DoRotateMenuItem ( long );
void	DoConvolveMenuItem ( long );
void	DoAlignMenuItem ( long );
void	DoMosaicMenuItem ( long );
void	DoRGBCombination ( void );
void	DoRGBSeparation ( GWindowPtr );
void	DoRGBBalance ( GWindowPtr );
void	RGBBalanceImageWindow ( GWindowPtr, double, double, double );
void	FlipImageWindow ( GWindowPtr, short );
void	TrimImageWindow ( GWindowPtr );
void	ShiftImageWindow ( GWindowPtr, double, double, int );
void	ScaleImageWindow ( GWindowPtr );
void	RotateImageWindow ( GWindowPtr, double );
void	ConvolveImageWindow ( GWindowPtr, float **, short, short );
float	**GetConvolutionFilter ( short, short *, short * );
void	DeleteConvolutionFilter ( float ** );
void	AlignImageWindow ( GWindowPtr, GWindowPtr );
void	MosaicImageWindow ( GWindowPtr, GWindowPtr );
void	DoTrim ( GWindowPtr );
void	DoClip ( GWindowPtr );
void	ClipImageData ( FITSImagePtr, FITSImagePtr, PIXEL, PIXEL, PIXEL, PIXEL );

/*** Functions in AnalyzeMenu.c ***/

void	DoAnalyzeMenuItem ( long );
void	DefineImageWindowImageObjectPSF ( GWindowPtr );
void	FindImageWindowImageObjects ( GWindowPtr, PIXEL, PIXEL, double );
void	DeleteImageWindowImageObjects ( GWindowPtr );
void	FindLocalMaximumPixel ( PIXEL **, short, short, short *, short *, short );

typedef	unsigned char	BYTE;			/* a byte */
typedef	float		PIXTYPE;		/* pixel type */
typedef	unsigned short	USHORT;			/* 0 to 65535 integers */

/*** Image constants and macros ***/

#define IMAGE_HISTOGRAM_BINS	100

struct Image
{
	char			imageTitle[256];
	GPathPtr		imagePath;
	GWindowPtr		imageWindow;
	GWindowPtr		imageHeaderWindow;
	GWindowPtr		imageHistogramWindow;
	FITSImagePtr	imageFITSImage;
	float			imageTemperature;
	float			imageExposureLength;
	short			imageType;
	short			imageFileFormat;
	long			imageHistogram[IMAGE_HISTOGRAM_BINS];
	PIXEL			imageMinimum;
	PIXEL			imageMaximum;
	double			imageMean;
	double			imageStdDev;
	ImageObjectPtr	imageObjectList;
	long			imageObjectCount;
	ImagePtr		imagePreviousImage;
};

/*** Functions in WindowMenu.c ***/

void			DoWindowMenuItem ( long );

/*** Functions in HelpMenu.c ***/

void			DoHelpMenuItem ( long );

/*** Functions in Files.c ***/

void			OpenImageFile ( GPathPtr );
void			SaveImageWindow ( GWindowPtr, GPathPtr );
short			FindFileType ( GPathPtr );
int				IsFITSImageFile ( GPathPtr );
int				IsTIFFImageFile ( GPathPtr );
int				IsJPEGImageFile ( GPathPtr );
int				IsPICTImageFile ( GPathPtr );
int				IsBMPImageFile ( GPathPtr );
int				IsGIFImageFile ( GPathPtr );

/*** Functions in HeaderWindow.c ***/

GWindowPtr		CreateHeaderWindow ( GWindowPtr );
void			DeleteHeaderWindow ( GWindowPtr );

void			SetHeaderWindowTitle ( GWindowPtr );
void			SetHeaderWindowImage ( GWindowPtr, ImagePtr );
ImagePtr		GetHeaderWindowImage ( GWindowPtr );
void			UpdateHeaderWindowImageHeader ( GWindowPtr );

/*** Functions in HistogramWindow.c ***/

GWindowPtr			CreateHistogramWindow ( GWindowPtr );

void				SetHistogramWindowTitle ( GWindowPtr );
void				SetHistogramWindowImage ( GWindowPtr, ImagePtr );
ImagePtr			GetHistogramWindowImage ( GWindowPtr );
ImageHistogramPtr	GetHistogramWindowHistogram ( GWindowPtr );

GPicturePtr			CreateHistogramWindowPicture ( GWindowPtr );

void				ComputeHistogramWindowHistogram ( GWindowPtr );
int					SaveHistogramWindow ( GWindowPtr, GPathPtr, short );
int					PrintHistogramWindow ( GWindowPtr, GRectPtr );
int					CopyHistogramWindow ( GWindowPtr );

/*** Functions in Image.c ***/

ImagePtr		NewImage ( char *, short, short, short, short );
void			DeleteImage ( ImagePtr );
void			UpdateImage ( ImagePtr );

ImagePtr		NewImageFromBitmap ( char *, GImagePtr );
ImagePtr		ReadFITSImageFile ( GPathPtr );
ImagePtr		ReadJPEGImageFile ( GPathPtr );
ImagePtr		ReadTIFFImageFile ( GPathPtr );
ImagePtr		ReadPICTImageFile ( GPathPtr );
ImagePtr		ReadBMPImageFile ( GPathPtr );
ImagePtr		ReadGIFImageFile ( GPathPtr );

ImagePtr		ReadDeepTIFFImageFile ( GPathPtr );
int				WriteDeepTIFFImageFile ( ImagePtr, FILE *, char *, unsigned short );

short			GetImageType ( ImagePtr );
short			GetImageFrames ( ImagePtr );
short			GetImageColumns ( ImagePtr );
short 			GetImageRows ( ImagePtr );
PIXEL 			**GetImageDataFrame ( ImagePtr, short );
PIXEL 			*GetImageDataRow ( ImagePtr, short, short );
PIXEL			GetImageDataValue ( ImagePtr, short, short, short );
void			SetImageDataValue ( ImagePtr, short, short, short, PIXEL );

short			GetImageFileFormat ( ImagePtr );
void 			SetImageFileFormat ( ImagePtr, short );

FITSImagePtr	GetImageFITSImage ( ImagePtr );
void			SetImageFITSImage ( ImagePtr, FITSImagePtr );

char			*GetImageTitle ( ImagePtr );
void			SetImageTitle ( ImagePtr, char * );

GPathPtr		GetImagePath ( ImagePtr );
void			SetImagePath ( ImagePtr, GPathPtr );

GWindowPtr		GetImageWindow ( ImagePtr );
void			SetImageWindow ( ImagePtr, GWindowPtr );

GWindowPtr		GetImageHeaderWindow ( ImagePtr );
void			SetImageHeaderWindow ( ImagePtr, GWindowPtr );

GWindowPtr		GetImageHistogramWindow ( ImagePtr );
void			SetImageHistogramWindow ( ImagePtr, GWindowPtr );

void			ComputeImageStatistics ( ImagePtr );
PIXEL			GetImageMinimumValue ( ImagePtr );
PIXEL			GetImageMaximumValue ( ImagePtr );
double			GetImageMeanValue ( ImagePtr );
double			GetImageStandardDeviation ( ImagePtr );
long			*GetImageHistogram ( ImagePtr, long * );
PIXEL			GetImagePercentile ( ImagePtr, double );

float			GetImageTemperature ( ImagePtr );
void			SetImageTemperature ( ImagePtr, float );
float			GetImageExposureLength ( ImagePtr );
void			SetImageExposureLength ( ImagePtr, float );

ImageObjectPtr	GetImageObjectList ( ImagePtr );
long			GetImageObjectCount ( ImagePtr );
void			AddImageObject ( ImagePtr, ImageObjectPtr );
void			RemoveImageObject ( ImagePtr, ImageObjectPtr );
void			DeleteImageObjectList ( ImagePtr );

ImagePtr		GetImagePreviousImage ( ImagePtr );
ImagePtr		PrepareImagePreviousImage ( ImagePtr );
void			DeleteImagePreviousImage ( ImagePtr );
int				RestoreImagePreviousImage ( ImagePtr );

/*** Functions in ImageWindow.c ***/

GWindowPtr		NewImageWindow ( ImagePtr );
void			DeleteImageWindow ( GWindowPtr );
int				DoImageWindowEvent ( short, GWindowPtr, long, long );

int				IsImageWindow ( GWindowPtr );

GWindowPtr		GetFirstImageWindow ( void );
GWindowPtr		GetNextImageWindow ( GWindowPtr );
void			SetNextImageWindow ( GWindowPtr, GWindowPtr );
GWindowPtr		GetNthImageWindow ( long );
GWindowPtr		GetActiveImageWindow ( void );

ImagePtr		GetImageWindowImage ( GWindowPtr );
void			SetImageWindowImage ( GWindowPtr, ImagePtr );

FITSImagePtr	ResizeImageWindowImage ( GWindowPtr, short, short, int );

int				GetImageWindowNeedsSave ( GWindowPtr );
void			SetImageWindowNeedsSave ( GWindowPtr, int );

GImagePtr		GetImageWindowBitmap ( GWindowPtr );
GImagePtr		NewImageWindowBitmap ( GWindowPtr );

GControlPtr		GetImageWindowVerticalScrollBar ( GWindowPtr );
GControlPtr		GetImageWindowHorizontalScrollBar ( GWindowPtr );

void			SetImageWindowColorTable ( GWindowPtr, short );
short			GetImageWindowColorTable ( GWindowPtr );

void			SetImageWindowColorFrame ( GWindowPtr, short, int );
short			GetImageWindowColorFrame ( GWindowPtr );

short			GetImageWindowDisplayMode ( GWindowPtr );
void			SetImageWindowDisplayMode ( GWindowPtr, short );

PIXEL			GetImageWindowDisplayMin ( GWindowPtr );
void			SetImageWindowDisplayMin ( GWindowPtr, PIXEL );

PIXEL			GetImageWindowDisplayMax ( GWindowPtr );
void			SetImageWindowDisplayMax ( GWindowPtr, PIXEL );

short			GetImageWindowDisplayZoom ( GWindowPtr );
void			SetImageWindowDisplayZoom ( GWindowPtr, short );

short			GetImageWindowDisplayMagnification ( GWindowPtr );
short			GetImageWindowDisplayMouseCoordinates ( GWindowPtr );
void			SetImageWindowDisplayMouseCoordinates ( GWindowPtr, short );

short			GetImageWindowDisplayCenterRow ( GWindowPtr );
void			SetImageWindowDisplayCenterRow ( GWindowPtr, short );

short			GetImageWindowDisplayCenterCol ( GWindowPtr );
void			SetImageWindowDisplayCenterCol ( GWindowPtr, short );

void			GetImageWindowDisplayRect ( GWindowPtr, GRectPtr );

short			GetImageWindowObjectDisplayFlag ( GWindowPtr, short );
void			SetImageWindowObjectDisplayFlag ( GWindowPtr, short, short );

ImageRegionPtr	GetImageWindowSelectedRegion ( GWindowPtr );
void			SetImageWindowSelectedRegion ( GWindowPtr, ImageRegionPtr );

short			GetImageWindowSelectedFrame ( GWindowPtr, short *, short * );
void			SetImageWindowSelectedFrame ( GWindowPtr, short );

CameraPtr		GetImageWindowCamera ( GWindowPtr );
void			SetImageWindowCamera ( GWindowPtr, CameraPtr );

int				GetImageWindowCroppedContentRect ( GWindowPtr, GRectPtr );
void			CropImageWindow ( GWindowPtr );
void			SizeImageWindow ( GWindowPtr );

void			DrawImageWindowMouseCoordinates ( GWindowPtr, short, short );

/*** Functions in ImageDisplay.c ***/

void			ImageToWindow ( GWindowPtr, short *, short * );
void			WindowToImage ( GWindowPtr, short *, short * );

void			ImageToWindowRect ( GWindowPtr, GRectPtr );
void			WindowToImageRect ( GWindowPtr, GRectPtr );

void			ImageXYToWindow ( GWindowPtr, float, float, short *, short * );
void			WindowToImageXY ( GWindowPtr, short, short, float *, float * );

void			DrawImageWindowBitmap ( GWindowPtr );
void			ComputeImageDisplayRange ( GWindowPtr );

void			SetRGBBalance ( double, double, double );
void			GetRGBBalance ( double *, double *, double * );

void			SetBitmapColorTableGrayscale ( GImagePtr );
void			SetBitmapColorTableRainbow ( GImagePtr );
void			SetBitmapColorTableFlame ( GImagePtr );
void			SetBitmapColorTableRed ( GImagePtr );
void			SetBitmapColorTableGreen ( GImagePtr );
void			SetBitmapColorTableBlue ( GImagePtr );
void			SetBitmapColorTableWinter ( GImagePtr );
void			SetBitmapColorTableSpring ( GImagePtr );
void			SetBitmapColorTableSummer ( GImagePtr );
void			SetBitmapColorTableFall ( GImagePtr );

/*** Functions in ImageRegion.c ***/

#define DeleteImageObject(o)	DeleteImageRegion ( o )
#define GetNextImageObject(o)	GetNextImageRegion ( o )
#define SetNextImageObject(o,n)	SetNextImageRegion ( o, n )

ImageRegionPtr	NewImageRegion ( short, short, short, short, short );
void			DeleteImageRegion ( ImageRegionPtr );
short			GetImageRegionType ( ImageRegionPtr );
short			GetImageRegionLeft ( ImageRegionPtr );
short			GetImageRegionTop ( ImageRegionPtr );
short			GetImageRegionRight ( ImageRegionPtr );
short			GetImageRegionBottom ( ImageRegionPtr );
short			GetImageRegionWidth ( ImageRegionPtr );
short			GetImageRegionHeight ( ImageRegionPtr );
double			GetImageRegionParameter ( ImageRegionPtr, short );
double			GetImageRegionError ( ImageRegionPtr, short );
ImageRegionPtr	GetNextImageRegion ( ImageRegionPtr );
char			*GetImageRegionName ( ImageRegionPtr );
int				GetImageRegionFlag ( ImageRegionPtr, long );
void			SetNextImageRegion ( ImageRegionPtr, ImageRegionPtr );
void			SetImageRegionName ( ImageRegionPtr, char * );
void			SetImageRegionFlag ( ImageRegionPtr, long, int );
void			SetImageRegionParameter ( ImageRegionPtr, short, double );
void			SetImageRegionError ( ImageRegionPtr, short, double );

int				GetImageRegionSegment ( ImageRegionPtr, short *, short *, short * );
int				IsPixelInImageRegion ( ImageRegionPtr, short, short );

void			DrawImageRegion ( GWindowPtr, ImageRegionPtr );
void			SelectImageRegion ( GWindowPtr, short, short );
void			SelectAllOfImage ( GWindowPtr );
void			SelectNoneOfImage ( GWindowPtr );
void			ComputeImageRegionStatistics ( GWindowPtr, ImageRegionPtr );
void			ComputeImageObjectCentroid ( ImageRegionPtr, PIXEL ** );

/*** Functions in ImageObject.c ***/

ImageObjectPtr	CreateImageObject ( ImagePtr, double, double, double );
void			DrawImageObject ( ImageObjectPtr, GWindowPtr );
void			GetImageObjectWindowRect ( ImageObjectPtr, GWindowPtr, GRectPtr );

int				FitImageSurface ( PIXEL **, ImageRegionPtr, PIXEL, PIXEL, ImageSurfaceFunctionPtr, short, double *, double * );
int				FitImageModel ( PIXEL **, ImageRegionPtr, PIXEL, PIXEL, ImageModelFunctionPtr, short, double *, double *, double, short );
int				ImproveImageModelFit ( PIXEL **, ImageRegionPtr, ImageModelFunctionPtr, double *, double *, short, double *, double * );

void			GaussianSurface ( short, short, double * );
double			GaussianStarModel ( short, short, double *, double * );

/*** Functions in ImageHistogram.c ***/

ImageHistogramPtr	CreateImageHistogram ( PIXEL, PIXEL, PIXEL, long );
void				DeleteImageHistogram ( ImageHistogramPtr );

PIXEL				GetImageHistogramMinimumValue ( ImageHistogramPtr );
PIXEL				GetImageHistogramMaximumValue ( ImageHistogramPtr );
PIXEL				GetImageHistogramResolution ( ImageHistogramPtr );

long				GetImageHistogramNumBins ( ImageHistogramPtr );
long				GetImageHistogramBinCount ( ImageHistogramPtr, long );
PIXEL				GetImageHistogramBinValue ( ImageHistogramPtr, long );
long				GetImageHistogramBinIndex ( ImageHistogramPtr, PIXEL );
long				GetImageHistogramPeakBinCount ( ImageHistogramPtr );

void				SetHistogramDisplayRect ( ImageHistogramPtr, GRectPtr );
GRectPtr			GetHistogramDisplayRect ( ImageHistogramPtr );

PIXEL				GetHistogramDisplayMinimumValue ( ImageHistogramPtr );
void				SetHistogramDisplayMinimumValue ( ImageHistogramPtr, PIXEL );
PIXEL				GetHistogramDisplayMaximumValue ( ImageHistogramPtr );
void				SetHistogramDisplayMaximumValue ( ImageHistogramPtr, PIXEL );

long				GetHistogramDisplayMaximumCount ( ImageHistogramPtr );
void				SetHistogramDisplayMaximumCount ( ImageHistogramPtr, long );
long				GetHistogramDisplayMinimumCount ( ImageHistogramPtr );
void				SetHistogramDisplayMinimumCount ( ImageHistogramPtr, long );

void				HistogramColRowToValueCount ( ImageHistogramPtr, short, short, PIXEL *, long * );
void				HistogramValueCountToColRow ( ImageHistogramPtr, PIXEL, long, short *, short * );

void				ComputeImageRegionHistogram ( ImagePtr, ImageRegionPtr, ImageHistogramPtr );
void				DrawImageHistogram ( ImageHistogramPtr );

/*** Functions in ImageProcessing.c ***/

void			DuplicateImage ( PIXEL **, PIXEL **, ImageRegionPtr );
void			FlipImageRows ( PIXEL **, PIXEL **, ImageRegionPtr );
void			FlipImageCols ( PIXEL **, PIXEL **, ImageRegionPtr );
void			InvertImage ( PIXEL **, PIXEL **, ImageRegionPtr );
void			AddImage ( PIXEL **, PIXEL **, PIXEL, ImageRegionPtr );
void			SubtractImage ( PIXEL **, PIXEL **, PIXEL, ImageRegionPtr );
void			MultiplyImage ( PIXEL **, PIXEL **, PIXEL, ImageRegionPtr );
void			DivideImage ( PIXEL **, PIXEL **, PIXEL, ImageRegionPtr );
void			LogarithmicScaleImage ( PIXEL **, double, ImageRegionPtr );
void			ExponentialScaleImage ( PIXEL **, double, ImageRegionPtr );
void			PowerScaleImage ( PIXEL **, double, ImageRegionPtr );

PIXEL			InterpolatePixelValue ( PIXEL **, short, short, double, double );

#define IMAGE_ADDITION			1
#define IMAGE_SUBTRACTION		2
#define IMAGE_MULTIPLICATION	3
#define IMAGE_DIVISION			4

void			DoImageWindowArithmetic ( short, GWindowPtr, GWindowPtr, PIXEL );

/*** Functions in CameraDialog.c ***/

int				DoCameraControlDialogEvent ( short, GWindowPtr, long, long );
short			GetCameraSubDialog ( GWindowPtr * );
GWindowPtr		GetCameraControlDialog ( void );
GWindowPtr		CreateCameraControlDialog ( void );

/*** Functions in CameraSetupDialog.c ***/

int				DoCameraSetupDialogEvent ( short, GWindowPtr, long, long );
int				DisconnectCamera ( void );

/*** Functions in CameraTemperatureDialog.c ***/

int				DoCameraTemperatureDialogEvent ( short, GWindowPtr, long, long );
int				UpdateCameraTemperatureDialog ( long );

/*** Functions in CameraExposureDialog.c ***/

int				DoCameraExposureDialogEvent ( short, GWindowPtr, long, long );
int				UpdateCameraExposureDialog ( long );
short			MoveFilter ( CameraPtr, short );
void			StartExposure ( CameraPtr );
void			CompleteExposure ( CameraPtr );
void			CancelExposure ( CameraPtr );

/*** Functions in MultipleExposureDialog ***/

int				DoMultipleExposureDialogEvent ( short, GWindowPtr, long, long );

/*** Functions in ImageDisplayDialog.c ***/

int				DoImageDisplayDialogEvent ( short, GWindowPtr, long, long );
void			SetImageDisplayDialogItems ( GWindowPtr );

/*** Functions in ImageAnalysisDialog.c ***/

int				DoImageAnalysisDialogEvent ( short, GWindowPtr, long, long );
void			ShowPixelValues ( GWindowPtr, short, short );
void			ShowImageRegionStatistics ( ImageRegionPtr );

/*** Functions in CameraInterface.c ***/

short			GetNumCameraTypes ( void );
short			GetNthCameraType ( short, char *, CameraInterfaceProcPtr * );

short			GetNumFilterTypes ( void );
short			GetNthFilterType ( short, char * );

short			DemoCameraInterface ( short, CameraInterfaceDataPtr );
short			SXUSBCameraInterface ( short, CameraInterfaceDataPtr );

/*** Functions in Camera.c ***/

CameraPtr		NewCameraConnection ( short, short, short );
void			DeleteCameraConnection ( CameraPtr );

CameraPtr		GetActiveCamera ( void );
void			SetActiveCamera ( CameraPtr );

short			GetCameraType ( CameraPtr );
short			GetCameraPort ( CameraPtr );

short			CameraHasCooling ( CameraPtr );
short			CameraHasShutter ( CameraPtr );

short			SetCameraTemperatureRegulation ( CameraPtr, short, float );
short			GetCameraTemperatureRegulation ( CameraPtr, short *, float * );
short			GetCameraTemperatureStatus ( CameraPtr, float *, float *, float * );

FITSImagePtr	NewCameraDarkFrame ( CameraPtr, short, short );
FITSImagePtr	GetCameraDarkFrame ( CameraPtr );
short			GetCameraDarkFrameMode ( CameraPtr );
void			SetCameraDarkFrameMode ( CameraPtr, short );

void			SetCameraExposureMode ( CameraPtr, short );
short			GetCameraExposureMode ( CameraPtr );
void			SetCameraExposureShutter ( CameraPtr, short );
short			GetCameraExposureShutter ( CameraPtr );
void			SetCameraExposureFilter ( CameraPtr, short );
short			GetCameraExposureFilter ( CameraPtr );
void			SetCameraExposureLength ( CameraPtr, float );
float			GetCameraExposureLength ( CameraPtr );
void			SetCameraExposureWindow ( CameraPtr, GWindowPtr );
GWindowPtr		GetCameraExposureWindow ( CameraPtr );

ExposureList	GetCameraExposureList ( CameraPtr );
void			SetCameraExposureList ( CameraPtr, ExposureList );
ExposurePtr 	GetCameraExposure ( CameraPtr );
void			SetCameraExposure ( CameraPtr, ExposurePtr );

GPathPtr		GetCameraExposurePath ( CameraPtr );
void			SetCameraExposurePath ( CameraPtr, GPathPtr );

char			*GetCameraExposureName ( CameraPtr );
void			SetCameraExposureName ( CameraPtr, char * );

short			GetCameraExposureFormat ( CameraPtr );
void			SetCameraExposureFormat ( CameraPtr, short );

short			GetCameraExposureNumber ( CameraPtr );
void			SetCameraExposureNumber ( CameraPtr, short );

short			GetCameraExposureTotal ( CameraPtr );
void			SetCameraExposureTotal ( CameraPtr, short );

short			GetCameraFilterInfo ( CameraPtr, short, short *, char * );
short			SetCameraFilter ( CameraPtr, short, short );
short			GetCameraFilter ( CameraPtr, short * );

short			GetCameraAntibloomingInfo ( CameraPtr, short, char * );
short			GetCameraAntiblooming ( CameraPtr, short * );
short			SetCameraAntiblooming ( CameraPtr, short );

short			GetCameraReadoutModeInfo ( CameraPtr, short, short *, short *, char * );
short			GetCameraReadoutMode ( CameraPtr, short * );
short			SetCameraReadoutMode ( CameraPtr, short );

short			GetCameraStatus ( CameraPtr, float *, short * );

short			StartCameraExposure ( CameraPtr, short );
short			StartCameraDownload ( CameraPtr );
short			EndCameraExposure ( CameraPtr );
short			EndCameraDownload ( CameraPtr );
short			DiscardCameraImageRows ( CameraPtr, short );
short			DownloadCameraImageRow ( CameraPtr, short, short, PIXEL * );

/*** Functions in Exposure.c ***/

ExposurePtr		NewExposure ( ExposureList *, short, short, short, float );
void			DeleteExposure ( ExposurePtr );
void			DeleteExposureList ( ExposureList );

short			GetExposureBinning ( ExposurePtr );
short			GetExposureShutter ( ExposurePtr );
short			GetExposureFilter ( ExposurePtr );
float			GetExposureLength ( ExposurePtr );
ExposurePtr		GetNextExposure ( ExposurePtr );

/*** Functions in GIF.c ***/

int				WriteGIFImageFile ( FILE *, GImagePtr );
