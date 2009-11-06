/***  Introduction  ***********************************************************

	COM32s is a set of Dynamically Linked Libraries which will allow a 32-bit
	Windows program running under Win32s to access the 16-bit serial and parallel
	communications routines built into Windows 3.1.  COM32s was created because
	Microsoft did not implement the true 32-bit Windows communications API in
    Win32s.

	In order for your 32-bit application to use COM32s, you must #include this
	header file, and the COM32s32.lib library file in your application.  You
	must also have the dynamically-linked library files (COM32s32.dll and
	COM32s16.dll) present in either your application's working directory or
	the active WINDOWS\SYSTEM directory.

	Please note that the functions contained in COM32s.dll will only work in
	Win32s; they will fail if your application is running in Windows 95, NT,
	or any other true 32-bit Windows operating system.  You can use the Win32
	API call GetVersionEx() to find this out, as well as the function Win32s()
    provided in COM32s32.dll.

	The source files COM32s16.c and COM32s32.c will compile to generate the
    dynamically-linked libraries COM32s16.dll and COM32s32.dll, respectively.

    Note: you can compile COM32s32.c directly into your application, thereby
    avoiding the need for a separate COM32s32.dll.  If you do so, make sure
    you call RegisterCom32s() when your application starts up, and
    UnRegisterCom32s() when it shuts down.  The HINSTANCE passed to these
    functions should be your application's instance handle.

******************************************************************************/

/*** prevent multiple #includes ***/

#ifndef COM32S_H
#define COM32S_H

#include <windows.h>

#define SZDLL32			"COM32s32.dll"
#define SZDLL16			"COM32s16.dll"

#ifndef USHORT
#define USHORT	unsigned short
#endif

#ifndef SHORT
#define SHORT	signed short
#endif

#define SERVICE_OPEN_COMM      1
#define SERVICE_CLOSE_COMM     2
#define SERVICE_READ_COMM      3
#define SERVICE_WRITE_COMM     4
#define SERVICE_GET_COMM_STATE 5
#define SERVICE_SET_COMM_STATE 6
#define SERVICE_GET_COMM_ERROR 7

/*** The following structures must be compiled with 1-byte alignment. ***/

#if defined ( __BORLANDC__ )
#pragma option -a1
#endif

typedef struct tagDCB16
{
	 BYTE		Id;
	 USHORT		BaudRate;
	 BYTE		ByteSize;
	 BYTE		Parity;
	 BYTE		StopBits;
	 USHORT		RlsTimeout;
	 USHORT		CtsTimeout;
	 USHORT		DsrTimeout;

	 USHORT		fBinary;
	 USHORT		fRtsDisable;
	 USHORT		fParity;
	 USHORT		fOutxCtsFlow;
	 USHORT		fOutxDsrFlow;
	 USHORT		fDummy;
	 USHORT		fDtrDisable;

	 USHORT		fOutX;
	 USHORT		fInX;
	 USHORT		fPeChar;
	 USHORT		fNull;
	 USHORT		fChEvt;

	 USHORT		fDtrflow;
	 USHORT		fRtsflow;
	 USHORT		fDummy2;

	 char		XonChar;
	 char		XoffChar;
	 USHORT		XonLim;
	 USHORT		XoffLim;
	 char		PeChar;
	 char		EofChar;
	 char		EvtChar;
	 USHORT		TxDelay;
}
DCB16, FAR *LPDCB16;

typedef struct tagCOMSTAT16 {
	 BYTE		status;
	 USHORT		cbInQue;
	 USHORT		cbOutQue;
}
COMSTAT16, FAR *LPCOMSTAT16;

typedef struct tagOPENCOMMPARAMS
{
	char      szPort[16];
	USHORT    lInBufferSize;
	USHORT    lOutBufferSize;
	SHORT     hCom;
}
OPENCOMMPARAMS, FAR *LPOPENCOMMPARAMS;

typedef struct tagCLOSECOMMPARAMS
{
	SHORT  hCom;
	SHORT  wResult;
}
CLOSECOMMPARAMS, FAR *LPCLOSECOMMPARAMS;

typedef struct tagREADWRITECOMMPARAMS
{
	SHORT	hCom;
	LPVOID	lpvBuffer;
	SHORT	lNumBytes;
	SHORT	wResult;
}
READWRITECOMMPARAMS, FAR *LPREADWRITECOMMPARAMS;

typedef struct tagGETCOMMERRORPARAMS
{
	SHORT    	hCom;
	LPCOMSTAT16	lpComstat;
	SHORT    	wResult;
}
GETCOMMERRORPARAMS, FAR *LPGETCOMMERRORPARAMS;

typedef struct tagGETSETCOMMSTATEPARAMS
{
	SHORT		hCom;
	LPDCB16		lpDCB;
	SHORT		wResult;
}
GETSETCOMMSTATEPARAMS, FAR *LPGETSETCOMMSTATEPARAMS;

/*** Revert to the previous structure byte-alignment option ***/

#if defined ( __BORLANDC__ )
#pragma option -a.
#endif

/*** COM32s API functions *****************************************************

	All of the COM32s API functions should perform identically to their
	16-bit Windows API counterparts; for instance, OpenComm32s() takes the
	same arguments and returns the same result codes as OpenComm(), etc.

	Several routines require 16-bit versions of the structures #defined in
    the API; these structures are defined above (DCB16 and COMSTAT16).

******************************************************************************/

BOOL RegisterCom32s ( HINSTANCE );
BOOL UnRegisterCom32s ( HINSTANCE );

BOOL _export Win32s ( VOID );

SHORT _export OpenComm32s ( LPCSTR, USHORT, USHORT );
SHORT _export CloseComm32s ( SHORT );
SHORT _export ReadComm32s ( SHORT, LPVOID, LONG );
SHORT _export WriteComm32s ( SHORT, LPVOID, LONG );
SHORT _export GetCommError32s ( SHORT, LPCOMSTAT16 );
SHORT _export GetCommState32s ( SHORT, LPDCB16 );
SHORT _export SetCommState32s ( LPDCB16 );

#endif

