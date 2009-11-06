/*** COPYRIGHT NOTICE AND PUBLIC SOURCE LICENSE *********************************

Portions Copyright (c) 1992-2001 Southern Stars Systems.  All Rights Reserved.

This file contains Original Code and/or Modifications of Original Code as defined
in and that are subject to the Southern Stars Systems Public Source License
Version 1.0 (the 'License').  You may not use this file except in compliance with
the License.  Please obtain a copy of the License at

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

*********************************************************************************/

/*****************************  header files  ********************************/

#include "GUILib.h"
#include "GUIPriv.h"

/*** functions ***/

static BOOL CALLBACK sAbortProc ( HDC, short );
static BOOL CALLBACK sCancelDialogProc ( HWND, UINT, WPARAM, LPARAM );
static void sInitializePrintDialogRecord ( PRINTDLG * );

/*** static global data ***/

static int			sSuccess, sCancel, sPrintDialogRecordInitialized = FALSE;
static HWND			sCancelDialog;
static PRINTDLG		sPrintDialogRecord;
static FARPROC		sCancelDialogProcPtr;
static ABORTPROC	sAbortProcPtr;

/**************************  GDoPrintJobDialog  ******************************/

int GDoPrintJobDialog ( short *lpwFirstPage, short *lpwLastPage, short *lpwNumCopies )
{
	int		nResult = FALSE;
	DWORD	dwError;
	
	/*** Initialize the Print Dialog data structure ***/

	if ( ! sPrintDialogRecordInitialized )
		sInitializePrintDialogRecord ( &sPrintDialogRecord );

	sPrintDialogRecord.hwndOwner = GGetMainWindow();
	sPrintDialogRecord.Flags = PD_ALLPAGES | PD_RETURNDC;
	sPrintDialogRecord.nFromPage = *lpwFirstPage;
	sPrintDialogRecord.nToPage = *lpwLastPage;

	/*** Display the "Print..." dialog.  If the user cancels or other error
		 ocurrs, return FALSE to indicate failure. ***/

	if ( ( nResult = PrintDlg ( &sPrintDialogRecord ) ) != FALSE )
	{
		/*** Determine if the user wants the entire document printed or just
			 certain pages.  Also save the page range and number of copies. ***/

		if ( sPrintDialogRecord.Flags & PD_ALLPAGES )
		{
			*lpwFirstPage = 1;
			*lpwLastPage = 9999;
		}
		else
		{
			*lpwFirstPage = sPrintDialogRecord.nFromPage;
			*lpwLastPage = sPrintDialogRecord.nToPage;
		}

		*lpwNumCopies = sPrintDialogRecord.nCopies;
	}
	else
	{
		dwError = CommDlgExtendedError();
	}

	return ( nResult );
}

/**************************  GStartPrintJob  ********************************/

#if 0

int GStartPrintJob ( char *lpszTitle )
{
	/*** Disable the application's main window so the user can't do something
		 stupid like select "Print..." again while the application is printing. ***/

	EnableWindow ( GGetMainWindow(), FALSE );

	sSuccess = TRUE;
	sCancel = FALSE;

	/*** Create our "Cancel Printing" dialog and its message-handling procedure. ***/

	sCancelDialogProcPtr = MakeProcInstance ( (FARPROC) sCancelDialogProc, GGetInstance() );
	sCancelDialog = CreateDialog ( GGetInstance(), "PRINTBOX", GGetMainWindow(), sCancelDialogProcPtr );

	/*** Create a pointer to our abort procedure.  The print manager calls this
		 procedure repeatedly while the application is busy printing.  We will
		 use it to dipatch messages to other applications while ours is busy. ***/

	sAbortProcPtr = MakeProcInstance ( (FARPROC) sAbortProc, GGetInstance() );
	Escape ( sPrintDialogRecord.hDC, SETABORTPROC, 0, (LPSTR) sAbortProcPtr, NULL );

	/*** Now start the print job. ***/

	if ( Escape ( sPrintDialogRecord.hDC, STARTDOC, strlen ( lpszTitle ), lpszTitle, NULL ) > 0 )
		sSuccess = TRUE;
	else
		sSuccess = FALSE;

	return ( sSuccess );
}

/************************** GEndPrintJob ***********************************/

int GEndPrintJob ( void )
{
	/*** If we successfully started the print job, end it ***/

	if ( sSuccess )
		Escape ( sPrintDialogRecord.hDC, ENDDOC, 0, NULL, NULL );

	/*** If the user did not cancel the print job, re-enable the application's
		 main window, and destroy the "Cancel printing" dialog. ***/

	if ( ! sCancel )
	{
		EnableWindow ( GGetMainWindow(), TRUE );
		DestroyWindow ( sCancelDialog );
	}

	/*** Dispose of the printer's device context, and the "Cancel" dialog/
		 abort procedure pointer; set them to NULL to indicate that we have
		 finished printing. ***/

	DeleteDC ( sPrintDialogRecord.hDC );
	FreeProcInstance ( sCancelDialogProcPtr );
	FreeProcInstance ( sAbortProcPtr );

	sCancelDialogProcPtr = NULL;
	sAbortProcPtr = NULL;

	return ( sSuccess );
}

/***********************  GStartDrawingPage  ********************************/

int GStartDrawingPage ( void )
{
	long	lXPixPerInch;
	long	lYPixPerInch;
	int		nXExtent;
    int		nYExtent;
	HDC		hDC;

	if ( hDC = GetDC ( NULL ) )
	{
		lXPixPerInch = GetDeviceCaps ( hDC, LOGPIXELSX );
		lYPixPerInch = GetDeviceCaps ( hDC, LOGPIXELSY );
        ReleaseDC ( NULL, hDC );
	}

	if ( hDC = sPrintDialogRecord.hDC )
	{
		GSetGraphicPort ( hDC );

		nXExtent = lXPixPerInch * GetDeviceCaps ( hDC, HORZRES ) / GetDeviceCaps ( hDC, LOGPIXELSX );
		nYExtent = lYPixPerInch * GetDeviceCaps ( hDC, VERTRES ) / GetDeviceCaps ( hDC, LOGPIXELSY );

		SetMapMode ( hDC, MM_ANISOTROPIC );
		SetWindowExtEx ( hDC, nXExtent, nYExtent, NULL );
		SetViewportExtEx ( hDC, GetDeviceCaps ( hDC, HORZRES ), GetDeviceCaps ( hDC, VERTRES ), NULL );
		SetViewportOrgEx ( hDC, 0, 0, NULL );
	}

	return ( TRUE );
}

/***********************  GEndDrawingPage  *********************************/

int GEndDrawingPage ( void )
{
	if ( sSuccess )
	{
		if ( Escape ( sPrintDialogRecord.hDC, NEWFRAME, 0, NULL, NULL ) < 0 )
		{
			sSuccess = FALSE;
			return ( FALSE );
		}
		else
		{
			GSetGraphicPort ( NULL );
			return ( TRUE );
		}
	}
}

#endif

int GStartPrintJob ( char *lpszTitle )
{
	int			nResult;
	DOCINFO		docinfo;

	/*** Disable the application's main window so the user can't do something
		 stupid like select "Print..." again while the application is printing. ***/

	EnableWindow ( GGetMainWindow(), FALSE );

	sSuccess = TRUE;
	sCancel = FALSE;

	/*** Create our "Cancel Printing" dialog and its message-handling procedure. ***/

	sCancelDialogProcPtr = MakeProcInstance ( (FARPROC) sCancelDialogProc, GGetInstance() );
	sCancelDialog = CreateDialog ( GGetInstance(), "PRINTBOX", GGetMainWindow(), sCancelDialogProcPtr );

	/*** Create a pointer to our abort procedure.  The print manager calls this
		 procedure repeatedly while the application is busy printing.  We will
		 use it to dipatch messages to other applications while ours is busy. ***/

	sAbortProcPtr = MakeProcInstance ( (FARPROC) sAbortProc, GGetInstance() );
	nResult = SetAbortProc ( sPrintDialogRecord.hDC, sAbortProcPtr );

    if ( nResult <= 0 )
    	sSuccess = FALSE;

	/*** Now start the print job. ***/

    docinfo.cbSize = sizeof ( DOCINFO );
    docinfo.lpszDocName = lpszTitle;
    docinfo.lpszOutput = NULL;
#ifdef GWIN32
    docinfo.lpszDatatype = NULL;
    docinfo.fwType = 0;
#endif

    nResult = StartDoc ( sPrintDialogRecord.hDC, &docinfo );

	if ( nResult <= 0 )
		sSuccess = FALSE;

	return ( sSuccess );
}

/**************************  GEndPrintJob  ***********************************/

int GEndPrintJob ( void )
{
    int	nResult;

	/*** If we successfully started the print job, end it ***/

	if ( sSuccess )
		nResult = EndDoc ( sPrintDialogRecord.hDC );

	/*** If the user did not cancel the print job, re-enable the application's
		 main window, and destroy the "Cancel printing" dialog. ***/

	if ( ! sCancel )
	{
		EnableWindow ( GGetMainWindow(), TRUE );
		DestroyWindow ( sCancelDialog );
	}

	/*** Dispose of the printer's device context, and the "Cancel" dialog/
		 abort procedure pointer; set them to NULL to indicate that we have
		 finished printing. ***/

	DeleteDC ( sPrintDialogRecord.hDC );
	FreeProcInstance ( sCancelDialogProcPtr );
	FreeProcInstance ( sAbortProcPtr );

	sCancelDialogProcPtr = NULL;
	sAbortProcPtr = NULL;

	return ( sSuccess );
}

/***********************  GStartDrawingPage  ********************************/

int GStartDrawingPage ( void )
{
	long	lXPixPerInch;
	long	lYPixPerInch;
	int		nXExtent;
    int		nYExtent;
    int		nResult;
    int		nTechnology;
	HDC		hDC;

    /*** Get the screen's device context, and determine the physical resolution
         (pixels per inch) of the screen. ***/
         
    hDC = GetDC ( NULL );
	if ( hDC != NULL )
	{
		lXPixPerInch = GetDeviceCaps ( hDC, LOGPIXELSX );
		lYPixPerInch = GetDeviceCaps ( hDC, LOGPIXELSY );
        ReleaseDC ( NULL, hDC );
	}

    hDC = sPrintDialogRecord.hDC;
	if ( hDC != NULL )
	{
		/*** Let's see what's going on here... ***/

		nTechnology = GetDeviceCaps ( hDC, TECHNOLOGY );

		GSetGraphicPort ( hDC );

		nResult = StartPage ( hDC );
		if ( nResult <= 0 )
			return ( FALSE );

		nXExtent = lXPixPerInch * GetDeviceCaps ( hDC, HORZRES ) / GetDeviceCaps ( hDC, LOGPIXELSX );
		nYExtent = lYPixPerInch * GetDeviceCaps ( hDC, VERTRES ) / GetDeviceCaps ( hDC, LOGPIXELSY );

		SetMapMode ( hDC, MM_ANISOTROPIC );
		SetWindowExtEx ( hDC, nXExtent, nYExtent, NULL );
		SetViewportExtEx ( hDC, GetDeviceCaps ( hDC, HORZRES ), GetDeviceCaps ( hDC, VERTRES ), NULL );
		SetViewportOrgEx ( hDC, 0, 0, NULL );
	}

	return ( TRUE );
}

/***********************  GEndDrawingPage  *********************************/

int GEndDrawingPage ( void )
{
	int		nResult;

	if ( sSuccess )
	{
        nResult = EndPage ( sPrintDialogRecord.hDC );
        
		if ( nResult < 0 )
		{
			sSuccess = FALSE;
			return ( FALSE );
		}
		else
		{
			GSetGraphicPort ( NULL );
			return ( TRUE );
		}
	}
}

/*****************************  GGetPageRect  *******************************/

LPRECT GGetPageRect ( LPRECT lpRect )
{
	SIZE	size;

	GetWindowExtEx ( sPrintDialogRecord.hDC, &size );

	lpRect->left = 0;
	lpRect->top = 0;
	lpRect->right = size.cx;
	lpRect->bottom = size.cy;

    return ( lpRect );
}

/**************************  GDoPrintSetupDialog  ***************************/
  
int GDoPrintSetupDialog ( void )
{
	int		nResult = FALSE;

	/*** Initialize the Print Dialog data structure ***/

	if ( ! sPrintDialogRecordInitialized )
		sInitializePrintDialogRecord ( &sPrintDialogRecord );

	sPrintDialogRecord.hwndOwner = GetActiveWindow();
	sPrintDialogRecord.Flags = PD_PRINTSETUP;

	/*** Display the "Print Setup..." dialog.  If the user cancels or other
		 error ocurrs, return FALSE to indicate failure. ***/

	nResult = PrintDlg ( &sPrintDialogRecord );

    return ( nResult );
}

/*****************************  sAbortProc  ********************************/

static BOOL CALLBACK sAbortProc ( HDC printerDC, short code )
{
	MSG	message;

	while ( ! sCancel && PeekMessage ( &message, NULL, 0, 0, PM_REMOVE ) )
	{
		if ( ! sCancelDialog || ! IsDialogMessage ( sCancelDialog, &message ) )
		{
			TranslateMessage ( &message );
			DispatchMessage ( &message );
		}
	}

    return ( ! sCancel );
}

/***************************  sCancelDialogProc  ***************************/

static BOOL CALLBACK sCancelDialogProc ( HWND dialog, UINT message, WPARAM wParam, LPARAM lParam )
{
	/*** If the user hits the dialog's "Cancel" button, re-enable the
		 application's main window, destroy the dialog and set its handle
		 to NULL to indicate that it has been obliterated. ***/

	switch ( message )
	{
		case WM_COMMAND:
			sCancel = TRUE;
			EnableWindow ( GGetMainWindow(), TRUE );
			DestroyWindow ( dialog );
			sCancelDialog = NULL;
			return ( TRUE );
	}

    return ( FALSE );
}

/**********************  sInitializePrintRecord  *****************************/

static void sInitializePrintDialogRecord ( PRINTDLG *pd )
{
	pd->lStructSize = sizeof ( PRINTDLG );
	pd->hwndOwner = GGetMainWindow();
	pd->hDevMode = NULL;
	pd->hDevNames = NULL;
	pd->hDC = NULL;
	pd->Flags = PD_ALLPAGES | PD_RETURNDC;
	pd->nFromPage = 0;
	pd->nToPage = 0;
	pd->nMinPage = 0;
	pd->nMaxPage = USHRT_MAX;
	pd->nCopies = 1;
	pd->hInstance = NULL;
	pd->lCustData = 0;
	pd->lpfnPrintHook = NULL;
	pd->lpfnSetupHook = NULL;
	pd->lpPrintTemplateName = NULL;
	pd->lpSetupTemplateName = NULL;
	pd->hPrintTemplate = NULL;
	pd->hSetupTemplate = NULL;

    sPrintDialogRecordInitialized = TRUE;
}

