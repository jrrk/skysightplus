/*** header files: some of these need symbols #defined before #including. ***/

#include <windows.h>

#define W32SUT_32
#include <w32sut.h>

#include "ParLib.h"
#include "Cookbook.h"

/*** local functions ***/

static BOOL RegisterDLL ( HINSTANCE );
static BOOL UnRegisterDLL ( HINSTANCE );

/*** local data ***/

static UT32PROC lpThunk = NULL;

/***  DllEntryPoint  *********************************************************

	This function is the required entry point to a 32-bit Windows DLL.
    
******************************************************************************/

int APIENTRY DllEntryPoint ( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
    int		nResult = TRUE;

	switch ( dwReason )
	{
        /*** When the DLL is attached at binding/program startup time,
             create the Universal Thunk to the 16-bit DLL. ***/
             
		case DLL_PROCESS_ATTACH:
			nResult = RegisterDLL ( hInstance );
            break;

        /*** When the DLL is unloaded at program exit, destroy the
             Universal Thunk. ***/

		case DLL_PROCESS_DETACH:
        	nResult = UnRegisterDLL ( hInstance );
			break;
	}

	return ( nResult );
}

/***  RegisterDLL  ***********************************************************

	This function creates a Universal Thunk which allows a 32-bit application
    to call a 16-bit DLL.  It is called when the 32-bit DLL is loaded and bound
    to the application at application startup time.

******************************************************************************/

BOOL RegisterDLL ( HINSTANCE hInstance )
{
    char			szErr[256];
	BOOL			bResult;

	bResult = UTRegister ( hInstance, SZDLL16, SZDLL16INITPROC,
				SZDLL16PROC, &lpThunk, NULL, NULL );

	if ( bResult == FALSE )
	{
		wsprintf ( szErr, "UTRegister() failed; error code was %ld!", GetLastError() );
		MessageBox ( NULL, szErr, SZDLL32, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK );
	}

	return ( bResult );
}

/***  UnRegisterDLL  **********************************************************

	This function destroys the Universal Thunk created by the DLL when it was
    loaded.  It should be called when the DLL is unloaded.
    
******************************************************************************/

BOOL UnRegisterDLL ( HINSTANCE hInstance )
{
	UTUnRegister ( hInstance );

	return ( TRUE );
}

/*** GetParallelPortAddress ***************************************************

	This function calls a function in the 16-bit DLL, using the Universal
    Thunk which the 32-bit DLL created when it got loaded.

    The service which the 16-bit DLL ought to perform is specified by
    a long-integer constant passed as the second argument to the UT.

    Data is passed between the 16- and 32-bit DLLs in a common data
    structure whose address is passed as the first argument to the UT.
    If no data needs to be exchanged while performing the specified
    service, this argument can be NULL.

    Any elements of the data structure which are pointers to memory in
    the address space of the 32-bit application need to be translated into
    16-bit address space before the 16-bit DLL can use them.  The addresses
    of such elements must be stored in an array which is passed as the third
    argument to the UT.  The final element of the array must be NULL.
    If no addresses need to be translated, the entire array can be NULL.

*****************************************************************************/

unsigned short _export GetParallelPortAddress ( short wLPTNum )
{
    GETLPTADDRESSDATA	data;

    data.wLPTNum = wLPTNum;

	(*lpThunk) ( (LPVOID) &data, SERVICE_GET_LPT_ADDRESS, NULL );

    return ( data.wLPTAddress );
}

/*** OpenCookbookDriver **************************************************/

int _export OpenCookbookDriver ( WORD wLPTAddress )
{
	OPENDRIVERDATA	data;

    data.wLPTAddress = wLPTAddress;

	(*lpThunk) ( (LPVOID) &data, SERVICE_OPEN_DRIVER, NULL );

	return ( data.bResult );
}

/*** CloseCookbookDriver *************************************************/

int _export CloseCookbookDriver ( void )
{
	(*lpThunk) ( NULL, SERVICE_CLOSE_DRIVER, NULL );

	return ( TRUE );
}

/*** SetCookbookReadoutMode **************************************************/

int SetCookbookReadoutMode ( short wModeIndex )
{
	SETREADOUTMODEDATA	data;

    data.wModeIndex = wModeIndex;

    (*lpThunk ) ( (LPVOID) &data, SERVICE_SET_READOUT_MODE, NULL );

    return ( data.bResult );
}

/*** GetCookbookReadoutMode **************************************************/

int GetCookbookReadoutMode ( short *lpwWidth, short *lpwHeight,
short *lpwHorizontalBinning, short *lpwVerticalBinning )
{
	GETREADOUTMODEDATA	data;

    (*lpThunk) ( (LPVOID) &data, SERVICE_GET_READOUT_MODE, NULL );

    *lpwWidth = data.wWidth;
    *lpwHeight = data.wHeight;
    *lpwHorizontalBinning = data.wHorizontalBinning;
    *lpwVerticalBinning = data.wVerticalBinning;
    
    return ( data.bResult );
}

/*** StartCookbookExposure ***************************************************/

int StartCookbookExposure ( int bLight )
{
	STARTEXPOSUREDATA	data;

    data.wLight = bLight;
    
    (*lpThunk) ( (LPVOID) &data, SERVICE_START_EXPOSURE, NULL );

    return ( data.bResult );
}

/*** EndCookbookExposure ***************************************************/

int EndCookbookExposure ( void )
{
    ENDEXPOSUREDATA		data;
    
    (*lpThunk) ( (LPVOID) &data, SERVICE_END_EXPOSURE, NULL );

    return ( data.bResult );
}

/*** GetCookbookImageDataRow ***************************************************/

int GetCookbookImageDataRow ( unsigned short *lpwData )
{
    GETIMAGEROWDATA		data;
    LPVOID				lpvTranslatedAddresses[2];
    
    data.lpwData = lpwData;

    /*** The lpwData argument is a pointer to memory in the 32-bit application's
         address space.  It must be translated into 16-bit address space before
         the 16-bit DLL can use it. ***/

    lpvTranslatedAddresses[0] = &(data.lpwData);
	lpvTranslatedAddresses[1] = NULL;

    (*lpThunk) ( (LPVOID) &data, SERVICE_GET_IMAGE_ROW, lpvTranslatedAddresses );

    return ( data.bResult );
}




