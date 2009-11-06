/*** Header files ***/

#include <windows.h>

#define W32SUT_16
#include <w32sut.h>

#include "ParLib.h"
#include "Cookbook.h"

static HINSTANCE hInst;

int FAR PASCAL _export LibMain ( HINSTANCE hInstance, WORD wDataSeg, WORD cbHeap, LPSTR lpszIgnored )
{
	if ( cbHeap )
		UnlockData ( 0 );

	hInst = hInstance;
	return ( TRUE );
}

int FAR PASCAL WEP ( int bSysexit )
{
}

/*** DLL16Init ****************************************************************

	This function is called by the Universal Thunk when it is first created.
    It should perform any initializations needed by the 16-bit DLL.  It should
    return TRUE if successful or FALSE on failure.
     
*******************************************************************************/

DWORD FAR PASCAL _export DLL16Init ( UT16CBPROC pfnCB, LPVOID lpvData )
{
	return ( TRUE );
}

/*** DLL16Proc ***************************************************************

	This function is the "recieving end" of the Universal Thunk.  Once the
    UT has been successfully created and initialized, it calls this function
    every time the 32-bit DLL wants our 16-bit DLL to perform some service.

    The service is specified by an integer code passed as the second argument
    to this function.

    Data is passed in from, and returned to, the 32-bit DLL in a data structure
    whose address is given as the first argument to this function.
    
******************************************************************************/

DWORD FAR PASCAL _export DLL16Proc ( LPVOID lpData, DWORD dwService )
{
	LPGETLPTADDRESSDATA		lpGetLPTAddressData = (LPGETLPTADDRESSDATA) lpData;
    LPOPENDRIVERDATA		lpOpenDriverData = (LPOPENDRIVERDATA) lpData;
    LPSETREADOUTMODEDATA	lpSetReadoutModeData = (LPSETREADOUTMODEDATA) lpData;
    LPGETREADOUTMODEDATA	lpGetReadoutModeData = (LPGETREADOUTMODEDATA) lpData;
    LPSTARTEXPOSUREDATA		lpStartExposureData = (LPSTARTEXPOSUREDATA) lpData;
    LPENDEXPOSUREDATA		lpEndExposureData = (LPENDEXPOSUREDATA) lpData;
    LPGETIMAGEROWDATA		lpGetImageRowData = (LPGETIMAGEROWDATA) lpData;
    
	switch ( dwService )
    {
        case SERVICE_GET_LPT_ADDRESS:
        	lpGetLPTAddressData->wLPTAddress = GetParallelPortAddress ( lpGetLPTAddressData->wLPTNum );
            break;

        case SERVICE_OPEN_DRIVER:
        	lpOpenDriverData->bResult = OpenCookbookDriver ( lpOpenDriverData->wLPTAddress );
            break;

        case SERVICE_CLOSE_DRIVER:
        	CloseCookbookDriver();
            break;

        case SERVICE_SET_READOUT_MODE:
        	lpSetReadoutModeData->bResult = SetCookbookReadoutMode ( lpSetReadoutModeData->wModeIndex );
        	break;

        case SERVICE_GET_READOUT_MODE:
        	lpGetReadoutModeData->bResult = GetCookbookReadoutMode (
            &lpGetReadoutModeData->wWidth,
            &lpGetReadoutModeData->wHeight,
            &lpGetReadoutModeData->wHorizontalBinning,
            &lpGetReadoutModeData->wVerticalBinning );
            break;

        case SERVICE_START_EXPOSURE:
        	lpStartExposureData->bResult = StartCookbookExposure (
            lpStartExposureData->wLight );
        	break;

        case SERVICE_END_EXPOSURE:
        	lpEndExposureData->bResult = EndCookbookExposure();
        	break;

        case SERVICE_GET_IMAGE_ROW:
        	lpGetImageRowData->bResult = GetCookbookImageDataRow (
            lpGetImageRowData->lpwData );
        	break;
    }

	return ( TRUE );
}
