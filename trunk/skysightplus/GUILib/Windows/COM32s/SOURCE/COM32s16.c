/*** Header files ***/

#define W32SUT_16
#include <windows.h>
#include <w32sut.h>
#include "com32s.h"

static HINSTANCE hInst;

int FAR PASCAL _export LibMain ( HINSTANCE hInstance, WORD wDataSeg, WORD cbHeap, LPSTR ignore )
{
	if ( cbHeap )
		UnlockData ( 0 );

	hInst = hInstance;
	return ( TRUE );
}

int FAR PASCAL WEP ( int bSysexit )
{
}

DWORD FAR PASCAL _export Com32s16Init ( UT16CBPROC pfnCB, LPVOID data )
{
	return TRUE;
}


DWORD FAR PASCAL _export Com32s16Proc ( LPVOID lpData, DWORD dwServiceID )
{
	LPOPENCOMMPARAMS			lpOpenCommParams;
	LPCLOSECOMMPARAMS			lpCloseCommParams;
	LPGETCOMMERRORPARAMS		lpGetCommErrorParams;
	LPREADWRITECOMMPARAMS		lpReadWriteCommParams;
	LPGETSETCOMMSTATEPARAMS		lpGetSetCommStateParams;

	switch ( dwServiceID )
	{
		case SERVICE_OPEN_COMM:
			lpOpenCommParams = lpData;
			lpOpenCommParams->hCom = OpenComm ( lpOpenCommParams->szPort,
												lpOpenCommParams->lInBufferSize,
												lpOpenCommParams->lOutBufferSize );
			break;

		case SERVICE_CLOSE_COMM:
			lpCloseCommParams = lpData;
			lpCloseCommParams->wResult = CloseComm ( lpCloseCommParams->hCom );
			break;

		case SERVICE_GET_COMM_ERROR:
			lpGetCommErrorParams = lpData;
			lpGetCommErrorParams->wResult = GetCommError ( lpGetCommErrorParams->hCom,
														   lpGetCommErrorParams->lpComstat );
			break;

		case SERVICE_READ_COMM:
			lpReadWriteCommParams = lpData;
			lpReadWriteCommParams->wResult = ReadComm ( lpReadWriteCommParams->hCom,
														lpReadWriteCommParams->lpvBuffer,
													    lpReadWriteCommParams->lNumBytes );
			break;

		case SERVICE_WRITE_COMM:
			lpReadWriteCommParams = lpData;
			lpReadWriteCommParams->wResult = WriteComm ( lpReadWriteCommParams->hCom,
														 lpReadWriteCommParams->lpvBuffer,
														 lpReadWriteCommParams->lNumBytes );
			break;

		case SERVICE_GET_COMM_STATE:
			lpGetSetCommStateParams = lpData;
			lpGetSetCommStateParams->wResult = GetCommState ( lpGetSetCommStateParams->hCom,
															  lpGetSetCommStateParams->lpDCB );
			break;

		case SERVICE_SET_COMM_STATE:
			lpGetSetCommStateParams = lpData;
			lpGetSetCommStateParams->wResult = SetCommState ( lpGetSetCommStateParams->lpDCB );
			break;
	}

	return ( TRUE );
}
