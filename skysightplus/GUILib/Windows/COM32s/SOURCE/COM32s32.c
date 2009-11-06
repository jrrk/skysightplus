#define W32SUT_32
#include <windows.h>
#include <w32sut.h>
#include "COM32s.h"

static UT32PROC lpThunk = NULL;
static BOOL		bWin32s = FALSE;

/***  DllEntryPoint  *********************************************************/

int APIENTRY DllEntryPoint ( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
    int		nResult = TRUE;

	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
			nResult = RegisterCom32s ( hInstance);
            break;
            
		case DLL_PROCESS_DETACH:
        	nResult = UnRegisterCom32s ( hInstance );
			break;
	}

	return ( nResult );
}

/***  RegisterCom32s  *********************************************************/

BOOL RegisterCom32s ( HINSTANCE hInstance )
{
    char			szErr[256];
	BOOL			bResult;
	OSVERSIONINFO	info;

	/*** First, determine whether we are running under Win32s.
		 If not, return a successful result code. ***/

	info.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );

	if ( GetVersionEx ( &info ) == TRUE )
		if ( info.dwPlatformId == VER_PLATFORM_WIN32s )
			bWin32s = TRUE;

	/*** If we are running under Win32s, create a Universal Thunk
	     to our 16-bit DLL. ***/

	if ( bWin32s )
	{
		bResult = UTRegister ( hInstance, "COM32s16.dll", "Com32s16Init",
				"Com32s16Proc", &lpThunk, NULL, NULL );

		if ( bResult == FALSE )
		{
			wsprintf ( szErr, "UTRegister() failed; error code was %ld!", GetLastError() );
			MessageBox(NULL, szErr, SZDLL32, MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK );
		}
	}

	return ( bResult );
}

/***  UnRegisterCom32s  *******************************************************/

BOOL UnRegisterCom32s ( HINSTANCE hInstance )
{
	if ( bWin32s )
		UTUnRegister ( hInstance );

	return ( TRUE );
}

/***  Win32s  *****************************************************************/

BOOL _export Win32s ( void )
{
	return ( bWin32s );
}

/***  OpenComm32s  ************************************************************/

SHORT _export OpenComm32s ( LPCSTR szPort, USHORT lInBufferSize, USHORT lOutBufferSize )
{
	if ( bWin32s )
	{
		OPENCOMMPARAMS	params;

		strcpy ( params.szPort, szPort );
		params.lInBufferSize = lInBufferSize;
		params.lOutBufferSize = lOutBufferSize;

		(*lpThunk) ( (LPVOID) &params, SERVICE_OPEN_COMM, NULL );

		return ( params.hCom );
	}
	else
	{
		return ( -1 );
	}
}

/***  CloseComm32s  ***********************************************************/

SHORT _export CloseComm32s ( SHORT hCom )
{
	if ( bWin32s )
	{
		CLOSECOMMPARAMS 	params;

		params.hCom = (SHORT) hCom;

		(*lpThunk) ( (LPVOID) &params, SERVICE_CLOSE_COMM, NULL );

		return ( params.wResult );
	}
	else
	{
		return ( -1 );
	}
}

/***  ReadComm32s  *************************************************************/

SHORT _export ReadComm32s ( SHORT hCom, LPVOID lpvBuffer, LONG lNumBytes )
{
	if ( bWin32s )
	{
		READWRITECOMMPARAMS	params;
		LPVOID				trans[2];

		params.hCom = (SHORT) hCom;
		params.lpvBuffer = lpvBuffer;
		params.lNumBytes = lNumBytes;

		trans[0] = &(params.lpvBuffer);
		trans[1] = NULL;

		(*lpThunk) ( (LPVOID) &params, SERVICE_READ_COMM, trans );

		return ( params.wResult );
	}
	else
	{
		return ( -1 );
    }
}

/***  WriteComm32s  **********************************************************/

SHORT _export WriteComm32s ( SHORT hCom, LPVOID lpvBuffer, LONG lNumBytes )
{
	if ( bWin32s )
	{
		READWRITECOMMPARAMS	params;
		LPVOID				trans[2];

		params.hCom = (SHORT) hCom;
		params.lpvBuffer = lpvBuffer;
		params.lNumBytes = lNumBytes;

		trans[0] = &(params.lpvBuffer);
		trans[1] = NULL;

		(*lpThunk) ( (LPVOID) &params, SERVICE_WRITE_COMM, trans );

		return ( params.wResult );

	}
	else
	{
		return ( -1 );
	}
}

/***  GetCommState32s  *******************************************************/

SHORT _export GetCommState32s ( SHORT hCom, LPDCB16 lpDCB )
{
	if ( bWin32s )
	{
		GETSETCOMMSTATEPARAMS	params;
		LPVOID					trans[2];

		params.hCom = (SHORT) hCom;
		params.lpDCB = lpDCB;

		trans[0] = &(params.lpDCB);
		trans[1] = NULL;

		(*lpThunk) ( (LPVOID) &params, SERVICE_GET_COMM_STATE, trans );

		return ( params.wResult );
	}
	else
	{
		return ( -1 );
	}
}

/***  SetCommState32s  *******************************************************/

SHORT _export SetCommState32s ( LPDCB16 lpDCB )
{
	if ( bWin32s )
	{
		GETSETCOMMSTATEPARAMS	params;
		LPVOID					trans[2];
		
		params.lpDCB = lpDCB;

		trans[0] = &(params.lpDCB);
		trans[1] = NULL;

		(*lpThunk) ( (LPVOID) &params, SERVICE_SET_COMM_STATE, trans );
		
		return ( params.wResult );
	}
	else
	{
		return ( -1 );
	}
}

/***  GetCommError32s  ********************************************************/

SHORT _export GetCommError32s ( SHORT hCom, LPCOMSTAT16 lpComstat )
{
	if ( bWin32s )
	{
		GETCOMMERRORPARAMS	params;
		LPVOID				trans[2];

		params.hCom = (SHORT) hCom;
		params.lpComstat = lpComstat;

		trans[0] = &(params.lpComstat);
		trans[1] = NULL;

		(*lpThunk) ( (LPVOID) &params, SERVICE_GET_COMM_ERROR, trans );

		return ( params.wResult );
	}
	else
	{
    	return ( -1 );
	}
}
