#ifndef COOKBOOK_H
#define COOKBOOK_H

/*** Cookbook public API *****************************************************

	These function prototypes define the C-language interface to the cookbook
    camera driver.  This high-level interface is identical in 16-bit Windows,
    32-bit Windows, and Mac OS.  The actual compiled functions themselves are
    contained in the 16- and 32-bit Windows cookbook driver DLLs, and in the
    Mac PowerPC CFM library.

    16-bit Windows applications need to link with Cookbk16.lib, and ensure
    that Cookbk16.dll is in the application's home directory or in the
    Windows directory.

    32-bit Windows applications need to link with Cookbk32.lib, and ensure
    that both Cookbk16.dll and Cookbk32.dll are in the application's home
    directory or in the Windows directory.

    Note that the 32-bit DLL is simply a stub which allows 32-bit applications
    to call the 16-bit driver code inside Cookbk16.dll.  That is why 32-bit
    Windows applications require both DLLs.  Also note that this will only
    work under Windows 95, Windows 98, or Windows 3.1 with Win32s; it will
    not work under Windows NT.

    IMPORTANT: For a 32-bit .EXE to use Cookbk32.lib, it must be marked as a
    Windows Subsystem version 3.10 executable (NOT a version 4.0 executable).
    If it's marked as a version 4.0 .exe, UTRegister() will fail.  Check your
    compiler's linker options for information on how to mark the executable's
    Windows subsystem version number.

    Mac applications simply need to link with CookbookDriverLib, either
    statically or dynamically.  If the latter, make sure that a copy of
    CookbookDriverLib is present in either the application's folder
    or in the System Extensions folder.

******************************************************************************/

/*** Turn off C++ name-mangling so that the Cookbook API functions can be
     called from C. ***/
     
#ifdef __cplusplus
extern "C" {
#endif

/*** OpenCookbookDriver *******************************************************

	Initializes the cookbook camera driver.

	int OpenCookbookDriver ( short wLPTAddress )

    (wLPTAddress): I/O address of LPT port to which camera is attached.

    The function returns TRUE if successful or FALSE on failure.  Please
    note that this function should execute successfully even if there is
    no camera attached to the parallel port!

    You should call this function before calling any of the other cookbook
    driver functions which follow.

******************************************************************************/

int FAR DLLEXPORT OpenCookbookDriver ( unsigned short );

/*** CloseCookbookDriver *******************************************************

	Closes the cookbook camera driver and performs associated cleanup.

	int CloseCookbookDriver ( void )

    The function returns TRUE if successful or FALSE on failure.

    Call this function after you are done performing any cookbook functions.

******************************************************************************/

int FAR DLLEXPORT CloseCookbookDriver ( void );

/*** SetCookbookReadoutMode ***************************************************

	Sets the readout (i.e. binning) mode that will be used when downloading
    images from the camera.

	int SetCookbookReadoutMode ( short binningMode )

    The function returns TRUE if successful or FALSE on failure.

******************************************************************************/

int FAR DLLEXPORT SetCookbookReadoutMode ( short binningMode );

/*** GetCookbookReadoutMode **************************************************

	Obtains the current cookbook readout mode, and the associated dimensions
    and binning factors of an image taken in that mode.

    int GetCookbookReadoutMode ( short *lpwMode, short *lpwWidth,
	short *lpwHeight, short *lpwHorizontalBinning, short *lpwVerticalBinning )

    (lpwMode): receives current readout mode index number.
    (lpwWidth): receives image width, in pixels.
    (lpwHeight): receives image height, in pixels.
    (lpwHorizontalBinning): receives horizontal binning factor.
    (lpwVerticalBinning): receives vertical binning factor.

    The function returns TRUE if successful or FALSE on failure.

    The readout mode index will be one of the constants #defined above.
    
    The binning factors returned are dimensionless numbers which correspond
    to the number of physical pixels on the CCD per pixels in the image
    read out from the camera.  For example, a binning factor of 2 means
    that each pixel in the downloaded image is actually made up of 2 pixels
    on the CCD.
    
******************************************************************************/

int FAR DLLEXPORT GetCookbookReadoutMode ( short *, short *, short *, short * );

/*** StartCookbookExposure ***************************************************

	Starts a cookbook camera exposure.
    
	int StartCookbookExposure ( int bLight )

    (wLight): if TRUE, open shutter during exposure; if FALSE, close shutter.

	The function returns TRUE if successful, and FALSE on failure.

******************************************************************************/

int FAR DLLEXPORT StartCookbookExposure ( int );

/*** EndCookbookExposure ***************************************************

	Ends a cookbook camera exposure and reads out the image.
    
	int EndCookbookExposure ( void )

	The function returns TRUE if successful, and FALSE on failure.

    After calling this function, the image will be read out from the camera
    and stored in a memory buffer allocated by the driver.  To view the
    image data that has been downloded, call GetCookbookImageDataRow()
    after calling this function.
    
******************************************************************************/

int FAR DLLEXPORT EndCookbookExposure ( void );

/*** GetCookbookImageDataRow **************************************************

	Copies a row of downloaded image data from the driver.
    
	int GetCookbookImageDataRow ( short *lpwData )

    (wRow): desired image data row number.
    (lpwData): pointer to buffer to receieve image data row.

	The function returns TRUE if successful, and FALSE on failure.

    After calling EndCookbookExposure (which reads out the image from the
    camera), you may call this function to obtain the image data which has
    been downloaded to the camera driver buffer.

    The row number specifies which row of the image you wish to copy.  It is
    counted from zero (for the topmost row) to the height of the image in the
    current readout mode minus one (for the bottom row.)  To obtain the
    width and height of the image, see GetCookbookReadoutMode().

    The image data row pixel values are copied as short integers into the
    buffer which begins at the address specified by the paramater (lpwData).
    You must make sure that this buffer is big enough to hold an entire
    row of image data (e.g. sizeof ( short ) * image width).

******************************************************************************/

int FAR DLLEXPORT GetCookbookImageDataRow ( unsigned short * );

#ifdef __cplusplus
}
#endif

/*** Cookbook private API ****************************************************

	All of the code below this point exists solely to allow the 16-bit and
    32-bit Windows DLL versions of the cookbook driver to communicate with
    each other.  High-level applications do not need and should not try to
    make use of any of the constants, macros, and functions defined in this
    section.

    When compiling the 16-bit and 32-bit DLLs, make sure that both DLLs have
    the same structure byte alignment, and also watch out for typical 16/32
    bit incompatibilities like the size of the "int" type, etc.

******************************************************************************/

/*** readout mode indices ***/

enum ReadoutMode
	{
	READOUT_LOWRES2 = 0,
	READOUT_LOWRES1,
	READOUT_MEDRES,
	READOUT_HIRES,
	READOUT_FIND,
	READOUT_FOCUS,
	READOUT_FOCUSLOW,
	READOUT_FOCUSMEDIUM,
	READOUT_FOCUSHIGH,
	//Insert new Readout Modes here
	NUM_READOUT_MODES
	};

#define Resolution_12bit 4095
#define Resolution_13bit 8190
#define Resolution_14bit 16383
#define Resolution_16bit 65535L

struct IMAGE
	{
	short x_res;
	short y_res;
	float dynamic_range;
	short hbin;
	short vbin;
	};
typedef struct IMAGE IMAGE;


void
CookBookCameraInitize( void );

void OpenCloseShutter( ParallelPortMap * port, short ms, BOOL openShutter );
void CameraParameters( ParallelPortMap * port );
void BeginExposure( ParallelPortMap * port, BOOL TakingDark, BOOL LDCControl );
void ReadNextExposureLine( ParallelPortMap * port, BOOL SubReset, BOOL Dbl_smpl, short readoutMode, short y, unsigned short * lineData );
void ClearStorageAndShiftImage( ParallelPortMap * port );
void InitImageData(IMAGE *);
void image_area_clear( ParallelPortMap * port );
void ShiftDownOneImageLine( ParallelPortMap * port );
void shift_find_line_binned( ParallelPortMap * port );
void shift_down_three_lines_and_bin( ParallelPortMap * port );
void read_whole_line_not_binned( ParallelPortMap * port, unsigned short * line, BOOL Dbl_smpl );
void read_internal_binned_line( ParallelPortMap * port, unsigned short * line, BOOL Dbl_smpl );
void serial_register_clear( ParallelPortMap * port );
void storage_area_clear( ParallelPortMap * port );
void shift_image_to_storage( ParallelPortMap * port );
void LDCOn( ParallelPortMap * port );
void LDCOff( ParallelPortMap * port );

#if defined ( PARALLEL_WIN16 ) || defined ( PARALLEL_WIN32 )

#define SZDLL32						"Cookbk32.dll"
#define SZDLL16						"Cookbk16.dll"
#define SZDLL16PROC					"DLL16Proc"
#define SZDLL16INITPROC				"DLL16Init"

/*** service codes: indicate the functions that the 32-bit DLL wants the 16-bit
     DLL to perform. ***/
     
#define SERVICE_GET_LPT_ADDRESS		1
#define SERVICE_OPEN_DRIVER			2
#define SERVICE_CLOSE_DRIVER		3
#define SERVICE_SET_READOUT_MODE	4
#define SERVICE_GET_READOUT_MODE	5
#define SERVICE_START_EXPOSURE		6
#define SERVICE_END_EXPOSURE		7
#define SERVICE_GET_IMAGE_ROW		8

/*** data structures: hold the data passed by the 32-bit DLL to the 16-bit DLL,
     and the results returned by the 16-bit DLL to the 32-bit DLL. ***/

typedef struct tagGETLPTADDRESSDATA
{
	WORD	wLPTNum;
    WORD	wLPTAddress;
}
GETLPTADDRESSDATA, FAR *LPGETLPTADDRESSDATA;

typedef struct tagOPENDRIVERDATA
{
	WORD	wLPTAddress;
    WORD	bResult;
}
OPENDRIVERDATA, FAR *LPOPENDRIVERDATA;

typedef struct tagSETREADOUTMODEDATA
{
	WORD	wModeIndex;
    WORD	bResult;
}
SETREADOUTMODEDATA, FAR *LPSETREADOUTMODEDATA;

typedef struct tagGETREADOUTMODEDATA
{
	WORD	wModeIndex;
    WORD	wWidth;
    WORD	wHeight;
    WORD	wHorizontalBinning;
    WORD	wVerticalBinning;
    WORD	bResult;
}
GETREADOUTMODEDATA, FAR *LPGETREADOUTMODEDATA;

typedef struct tagSTARTEXPOSUREDATA
{
	WORD	wLight;
    DWORD	dwLength;
    WORD	wWait;
    BOOL	bResult;
}
STARTEXPOSUREDATA, FAR *LPSTARTEXPOSUREDATA;

typedef struct tagENDEXPOSUREDATA
{
	WORD		bResult;
}
ENDEXPOSUREDATA, FAR *LPENDEXPOSUREDATA;

typedef struct tagGETIMAGEROWDATA
{
	WORD		wRow;
    LPWORD		lpwData;
    WORD		bResult;
}
GETIMAGEROWDATA, FAR *LPGETIMAGEROWDATA;

#endif /* PARALLEL_WIN16, PARALLEL_WIN32 */

#endif /* COOKBOOK_H */

