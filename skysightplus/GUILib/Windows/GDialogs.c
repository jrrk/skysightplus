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
#define GLOBAL extern
#include "GUIPriv.h"
#undef GLOBAL

/*** local data types ***/

typedef struct ModalDialogSettings
{
	HWND							hOldModalDialog;
	struct ModalDialogSettings		*lpOldSettings;
}
ModalDialogSettings;

/*** local function prototypes ***/

static BOOL CALLBACK sAskDialogProc ( HWND, UINT, WPARAM, LPARAM );
static BOOL CALLBACK sProgressDialogProc ( HWND, UINT, WPARAM, LPARAM );

static BOOL CALLBACK	sDlgProc ( HWND, UINT, WPARAM, LPARAM );
static LRESULT CALLBACK	sDlgWndProc ( HWND, UINT, WPARAM, LPARAM );

/*** local data ***/

static ModalDialogSettings			*sModalDialogSettingsList = NULL;

static HWND	sModalDialogWindow = NULL;
static int	sModalDialogLoopFlag = FALSE;
static int	sModalDialogLoopResult = 0;

/****************************  sDlgProc  *****************************/

BOOL CALLBACK sDlgProc ( HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if ( GDoMessage ( hDlg, nMsg, wParam, lParam ) )
		return ( FALSE );
	else
		return ( TRUE );
}

/****************************  sDlgWndProc  ********************************/

LRESULT CALLBACK sDlgWndProc ( HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if ( GDoMessage ( hDlg, nMsg, wParam, lParam ) )
		return ( DefDlgProc ( hDlg, nMsg, wParam, lParam ) );
	else
		return ( 0 );
}

/*****************************  GCreateDialog  *******************************/

HWND GCreateDialog ( short wDlgID, long lData, GEventProcPtr lpfnEventProc )
{
	HWND			hDlg, hWndOwner;
    HINSTANCE		hInstance;
	FARPROC			lpfnDlgProc;
	LPSTR			lpszDlgID;
	GWindowDataPtr	lpDlgData;

	hInstance = GGetInstance();
	lpszDlgID = MAKEINTRESOURCE ( wDlgID );

	/*** Use the application's main window as the dialog's owning window,
	     unless the main window is hidden.  (Strange things happen when we
		 try to center the dialog window when its owner is hidden.  Fuck
		 Microsoft for making me waste a day to figure out this bullshit.) ***/

	hWndOwner = GGetMainWindow();
//	if ( ! IsWindowVisible ( hWndOwner ) )
//		hWndOwner = NULL;

	/*** Create a GUILib private window data record; return on failure. ***/

	lpDlgData = GCreateGWindowData ( wDlgID, lpfnEventProc, lData );
	if ( lpDlgData == NULL )
		return ( NULL );

	/*** Make a thunk to GUILib's dialog procedure.
		 Create the dialog using the dialog template resource ID provided.  ***/

	lpfnDlgProc = MakeProcInstance ( sDlgProc, hInstance );
	hDlg = CreateDialog ( hInstance, lpszDlgID, hWndOwner, lpfnDlgProc );

	/*** If we failed, free the dialog procedure instance handle.
		 Then register a window class with the same name as the
		 dialog template resource ID, and try creating the dialog
		 again. ***/

	if ( hDlg == NULL )
	{
		FreeProcInstance ( lpfnDlgProc );
		GRegisterWindowClass ( lpszDlgID, DLGWINDOWEXTRA, sDlgWndProc, COLOR_WINDOW );

		hDlg = CreateDialog ( hInstance, lpszDlgID, hWndOwner, NULL );
	}

	/*** If we still can't create the dialog window, give up. ***/

	if ( hDlg == NULL )
	{
		GDeleteGWindowData ( lpDlgData );
		return ( NULL );
	}

	/*** Attach the GUILib private window data record to the dialog window.
		 Send the window its initial event, then return a pointer to the
		 dialog window. ***/

    GCenterWindow ( hDlg );
	GSetGWindowData ( hDlg, lpDlgData );
	GEraseOnUpdates ( hDlg, TRUE );
	GSendEvent ( G_INITIAL_EVENT, hDlg, wDlgID, lData );

	return ( hDlg );
}

/*** GSetSubDialog ******************************************************/

GWindowPtr GSetSubDialog ( GWindowPtr hDlgParent, short wBaseCtlID, short wDlgID )
{
	HWND			hDlg, hWndOwner;
    HINSTANCE		hInstance;
	FARPROC			lpfnDlgProc;
	LPSTR			lpszDlgID;
	GWindowDataPtr	lpDlgData;

	hInstance = GGetInstance();
	lpszDlgID = MAKEINTRESOURCE ( wDlgID );
	hWndOwner = GetDlgItem ( hDlgParent, wBaseCtlID );

    /*** First see if the owner control already has a child dialog ***/

    hDlg = GetWindow ( hWndOwner, GW_CHILD );
    if ( hDlg != NULL )
    	GDeleteWindow ( hDlg );
        
	/*** Create a GUILib private window data record with the same event-
	     handling procedure and application-defined data as the parent
	     dialog window; return on failure. ***/

	lpDlgData = GGetGWindowData ( hDlgParent );
	lpDlgData = GCreateGWindowData ( wDlgID, lpDlgData->pfnProc, lpDlgData->lData );
	if ( lpDlgData == NULL )
		return ( NULL );

	/*** Make a thunk to GUILib's dialog procedure.  Then create the sub-
	     dialog as a child window of the root control of the parent dialog.
	     Use the dialog template resource ID provided.  On failure, delete
	     the private window data record and return.  ***/

	lpfnDlgProc = MakeProcInstance ( sDlgProc, hInstance );
	hDlg = CreateDialog ( hInstance, lpszDlgID, hWndOwner, lpfnDlgProc );
	if ( hDlg == NULL )
	{
		GDeleteGWindowData ( lpDlgData );
		return ( NULL );
	}

	/*** Position the child dialog window so its upper left corner
	     matches the upper left corner of the base control. ***/
	     
    SetWindowPos ( hDlg, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE );

	/*** Attach the GUILib private window data record to the dialog window.
		 Send the window its initial event, then display the window and return
		 a pointer to it. ***/

	GSetGWindowData ( hDlg, lpDlgData );
	GEraseOnUpdates ( hDlg, TRUE );
	GSendEvent ( G_INITIAL_EVENT, hDlg, wDlgID, lpDlgData->lData );
	ShowWindow ( hDlg, SW_SHOWNA );

	return ( hDlg );
}

/*** GGetSubDialog ******************************************************/

GWindowPtr GGetSubDialog ( GWindowPtr hDlg, short wCtlID )
{
	return ( GetWindow ( GetDlgItem ( hDlg, wCtlID ), GW_CHILD ) );
}

/**************************  GIsDialogWindow  ***************************/

int GIsDialogWindow ( HWND hWnd )
{
	char		szClassName[256] = { '\0' };
	WNDCLASS	wndclass;

	if ( GetClassWord ( hWnd, GCW_ATOM ) == (WORD) WC_DIALOG )
	{
		return ( TRUE );
	}
	else
	{
		GetClassName ( hWnd, szClassName, 255 );
		GetClassInfo ( GGetInstance(), szClassName, &wndclass );

		if ( wndclass.cbWndExtra == DLGWINDOWEXTRA )
			return ( TRUE );
		else
			return ( FALSE );
	}
}

/*************************  GShowModalDialog  ***************************/

void GShowModalDialog ( HWND hDlg )
{
	HWND					hWnd;
	ModalDialogSettings		*lpSettings;

	/*** First, create a modal dialog settings structure to save
	     the current values of the modal dialog variables into. ***/
	     
	lpSettings = (ModalDialogSettings *) GNewPtr ( sizeof ( ModalDialogSettings ) );
	
	if ( lpSettings != NULL )
	{
		/*** If successful, add it to the head of the linked list
		     of modal dialog settings variables, and save the current
		     modal dialog settings into it. **/

		lpSettings->hOldModalDialog = sModalDialogWindow;
		lpSettings->lpOldSettings = sModalDialogSettingsList;
		sModalDialogSettingsList = lpSettings;

		/*** Now display the new modal dialog window in front of
             all of the application's other windows. ***/

		ShowWindow ( hDlg, SW_SHOW );
		EnableWindow ( hDlg, TRUE );
        
    	/*** If there is already a modal dialog window visible, disable it.
             If not, disable the application's main window and all windows
             in front of it, up to the new modal dialog window. ***/

    	if ( sModalDialogWindow != NULL )
    		EnableWindow ( sModalDialogWindow, FALSE );
    	else
			for ( hWnd = GGetMainWindow(); hWnd != hDlg; hWnd = GetWindow ( hWnd, GW_HWNDPREV ) )
				EnableWindow ( hWnd, FALSE );

        /*** Save a pointer to the new modal dialog window. ***/

		sModalDialogWindow = hDlg;
	}
}

/**************************  GHideModalDialog  **************************/

void GHideModalDialog ( HWND hDlg )
{
	HWND					hWnd;
	ModalDialogSettings		*lpSettings = sModalDialogSettingsList;

	/*** First, get a pointer to the current modal dialog settings. ***/
	     
	if ( lpSettings != NULL && hDlg == sModalDialogWindow )
	{
		/*** Restore the previous values of the modal dialog settings
		     variable(s).  Then remove the settings record from the start
		     of the settings list and destroy it. ***/

		sModalDialogWindow = lpSettings->hOldModalDialog;
		sModalDialogSettingsList = lpSettings->lpOldSettings;
		GFreePtr ( lpSettings );

    	/*** If there was a modal dialog previously visible, re-enable it.
	         Otherwise, re-enable the main window and all windows in front
	         of it, up to our dialog window.  ***/

		if ( sModalDialogWindow != NULL )
			EnableWindow ( sModalDialogWindow, TRUE );
	    else
 			for ( hWnd = GGetMainWindow(); hWnd != hDlg; hWnd = GetWindow ( hWnd, GW_HWNDPREV ) )
				EnableWindow ( hWnd, TRUE );

		/*** Now hide our (previously active modal) dialog window. ***/
		
		ShowWindow ( hDlg, SW_HIDE );
	}
}

/***********************  GEnterModalDialogLoop  *************************/

int GEnterModalDialogLoop ( GWindowPtr hDlg )
{
	int 	oldModalDialogLoopFlag;
	MSG		message;

	/*** Display the modal dialog window and disable all others. ***/
	
	GShowModalDialog ( hDlg );
	
    /*** Save the status of the previous modal dialog loop,
         then set the modal dialog loop flag to indicate the
         start of a new dialog loop.***/

	oldModalDialogLoopFlag = sModalDialogLoopFlag;
	sModalDialogLoopFlag = TRUE;

    /*** Now go into loop, retrieving and dispatching messages until
         the application sets the loop status flag to FALSE in response
         to a message. ***/
         
	while ( sModalDialogLoopFlag == TRUE )
	{
		GetMessage ( &message, NULL, 0, 0 );
		if ( ! IsDialogMessage ( hDlg, &message ) )
		{
			TranslateMessage ( &message );
			DispatchMessage ( &message );
		}
	}

    /*** Restore the value of the previous modal dialog loop flag. ***/

	sModalDialogLoopFlag = oldModalDialogLoopFlag;

	/*** Hide the modal dialog window and re-enable the others.
	     Return the modal dialog result code.***/
	
	GHideModalDialog ( hDlg );
    return ( sModalDialogLoopResult );
}

/************************  GExitModalDialogLoop  ***************************/

void GExitModalDialogLoop ( int nResult )
{
	sModalDialogLoopFlag = FALSE;
	sModalDialogLoopResult = nResult;
}

/*****************************  GModalDialog  *******************************/

int GEnterModalDialog ( short idDialog, long lData, GEventProcPtr lpfnEventProc )
{
    int				nResult = 0;
	HWND			hDlg;

	if ( ( hDlg = GCreateDialog ( idDialog, lData, lpfnEventProc ) ) != NULL )
	{
		nResult = GEnterModalDialogLoop ( hDlg );
		DestroyWindow ( hDlg ); 
	}

    return ( nResult );
}

/************************  GGetModalDialogWindow  ***********************/

GWindowPtr GGetModalDialogWindow ( void )
{
	return ( sModalDialogWindow );
}

/************************  GGetDialogItemText  **************************/

void GGetDialogItemText ( HWND hDlg, short idItem, char *lpszText )
{
	GetWindowText ( GetDlgItem ( hDlg, idItem ), lpszText, 255 );
}

/************************  GSetDialogItemText  ***************************/

void GSetDialogItemText ( HWND hDlg, short idItem, char *lpszText )
{
    HWND	hCtl = GetDlgItem ( hDlg, idItem );

    /*** Setting the control's text will cause it to generate a
         WM_COMMAND message with an EN_CHANGE code to the application.
         Since we only want the application to handle events when the
         user has changed the control's text, we'll supress the
         command message rather than passing it on. ***/

    GSupressCommandMessage ( hCtl );
	SetWindowText ( hCtl, lpszText );
    GSupressCommandMessage ( NULL );
}

/************************  GGetDialogItemRect  ***************************/

void GGetDialogItemRect ( HWND hDlg, short idItem, LPRECT lpRect )
{
	GetWindowRect ( GetDlgItem ( hDlg, idItem ), lpRect );
	ScreenToClient ( hDlg, (LPPOINT) &lpRect->left );
	ScreenToClient ( hDlg, (LPPOINT) &lpRect->right );
}

/***********************  GSetDialogItemRect  ****************************/

void GSetDialogItemRect ( HWND hDlg, short idItem, LPRECT lpRect )
{
	MoveWindow ( GetDlgItem ( hDlg, idItem ), lpRect->left, lpRect->top,
	lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, TRUE );
}

/*************************  GSetDialogIcon  *****************************/

void GSetDialogIcon ( HWND hDlg, short idItem, short idIcon )
{
	HWND	hCtl = GetDlgItem ( hDlg, idItem );
	HICON	hIcon = LoadIcon ( GGetInstance(), MAKEINTRESOURCE ( idIcon ) );

	SendMessage ( hCtl, STM_SETICON, (WPARAM) hIcon, 0 );
}

/*************************  GShowDialogItem  ****************************/

void GShowDialogItem ( HWND hDlg, short idItem )
{
	ShowWindow ( GetDlgItem ( hDlg, idItem ), SW_SHOWNA );
}

/*************************  GHideDialogItem  ****************************/

void GHideDialogItem ( HWND hDlg, short idItem )
{
	ShowWindow ( GetDlgItem ( hDlg, idItem ), SW_HIDE );
}

/*********************  EnableDialogItem  ********************************/

void GEnableDialogItem ( GWindowPtr pDlg, short idItem, int fEnable )
{
	EnableWindow ( GetDlgItem ( pDlg, idItem ), fEnable );
}

/*********************  GGetDialogControlValue  **************************/

short GGetDialogControlValue ( HWND hDlg, short idItem )
{
	return ( GGetControlValue ( GetDlgItem ( hDlg, idItem ) ) );
}

/*********************  GSetDialogControlValue  ****************************/

void GSetDialogControlValue ( HWND hDlg, short idItem, short wValue )
{
	GSetControlValue ( GetDlgItem ( hDlg, idItem ), wValue );
}

/***********************  GSetSelectedDialogButton  **********************/

void GSetSelectedDialogButton ( HWND hDlg, short idFirstItem, short idLastitem,
short idCheckedItem )
{
	CheckRadioButton ( hDlg, idFirstItem, idLastitem, idCheckedItem );
}

/**********************  GGetSelectedDialogButton  *************************/

short GGetSelectedDialogButton ( HWND hDlg, short idFirstItem, short idLastItem )
{
	int   idItem;

	for ( idItem = idFirstItem; idItem <= idLastItem; idItem++ )
		if ( IsDlgButtonChecked ( hDlg, idItem ) )
			return ( idItem );

	return ( 0 );
}

/*** GSetDialogDefaultButton *************************************************/

void GSetDialogDefaultButton ( HWND hDlg, short wItem ) 
{
	HWND	hCtl = GetDlgItem ( hDlg, wItem );
	DWORD	dwStyle = GetWindowLong ( hCtl, GWL_STYLE );
	char	szClass[256];

	if ( GetClassName ( hCtl, szClass, sizeof ( szClass ) ) )
		if ( lstrcmpi ( szClass, "BUTTON" ) == 0 )
			SetWindowLong ( hCtl, GWL_STYLE, dwStyle | BS_DEFPUSHBUTTON );
}

/*** GGetDialogDefaultButton **************************************************/

short GGetDialogDefaultButton ( HWND hDlg )
{
	HWND	hCtl;
	char	szClass[256];

	for ( hCtl = GetWindow ( hDlg, GW_CHILD ); hCtl != NULL; hCtl = GetWindow ( hCtl, GW_HWNDNEXT ) )
	{
		if ( GetClassName ( hCtl, szClass, sizeof ( szClass ) ) )
			if ( lstrcmpi ( szClass, "BUTTON" ) == 0 )
				if ( GetWindowLong ( hCtl, GWL_STYLE ) & BS_DEFPUSHBUTTON )
					return ( GetWindowLong ( hCtl, GWL_ID ) );
	}
}

/************************  GSelectDialogItem  *****************************/

void GSelectDialogItem ( HWND hDlg, short idItem )
{
	HWND	hCtl = GetDlgItem ( hDlg, idItem );
	char	szClassName[32] = { 0 };
	long	lTicks;
	
	GetClassName ( hCtl, szClassName, 31 );
	
	if ( lstrcmpi ( szClassName, "BUTTON" ) == 0 )
	{
		SendMessage ( hCtl, BM_SETSTATE, 1, 0 );
		for ( lTicks = GetTickCount() + 125; GetTickCount() < lTicks; )
			;
		SendMessage ( hCtl, BM_SETSTATE, 0, 0 );
		FORWARD_WM_COMMAND ( hDlg, idItem, hCtl, BN_CLICKED, SendMessage );
	}
	
	if ( lstrcmpi ( szClassName, "EDIT" ) == 0 )
	{
		SetFocus ( hCtl );
	}
}

/*****************************  GDoAlert  ********************************/

int GDoAlert ( short wIcon, short wButtons, char *lpszText )
{
	char	szTitle[64];
    int		nResult;
    UINT    fuStyle = 0;
 
	/*** Compute the appropriate style flags to MessageBox from the
		 number of buttons. ***/

	if ( wIcon == G_INFO_ALERT )
		fuStyle |= MB_ICONINFORMATION;

	if ( wIcon == G_WARNING_ALERT )
		fuStyle |= MB_ICONEXCLAMATION;

	if ( wIcon == G_ERROR_ALERT )
		fuStyle |= MB_ICONSTOP;

	if ( wButtons == G_OK_ALERT )
		fuStyle |= MB_OK;

	if ( wButtons == G_OK_CANCEL_ALERT )
		fuStyle |= MB_OKCANCEL;

	if ( wButtons == G_YES_NO_CANCEL_ALERT )
		fuStyle |= MB_YESNOCANCEL;

	/*** Display the message box with the appropriate icon. ***/

	GetWindowText ( GGetMainWindow(), szTitle, 63 );
	nResult = MessageBox ( GetActiveWindow(), lpszText, szTitle, fuStyle );
    
	/*** Return the item number of the button selected by the user
	     to exit the message box, or 0 on failure. ***/

	if ( nResult == IDOK )
		nResult = G_OK_BUTTON;

	if ( nResult == IDCANCEL )
		nResult = G_CANCEL_BUTTON;

	if ( nResult == IDYES )
		nResult = G_YES_BUTTON;

	if ( nResult == IDNO )
    	nResult = G_NO_BUTTON;

	return ( nResult );
}

/****************************  GDoDialog  *******************************/

#define ASK_OK_BUTTON_ID		1
#define ASK_CANCEL_BUTTON_ID	2
#define ASK_QUESTION_TEXT_ID	3
#define ASK_ANSWER_TEXT_ID		4

static char *sPrompt, *sText;
static BOOL CALLBACK sAskDialogProc ( HWND, UINT, WPARAM, LPARAM );

int GDoDialog ( char *lpszPrompt, char *lpszText )
{
	int		nResult;
	FARPROC	lpfnAskDlgProc;

	sPrompt = lpszPrompt;
	sText = lpszText;

 	lpfnAskDlgProc = MakeProcInstance ( sAskDialogProc, GGetInstance() );
	nResult = DialogBox ( GGetInstance(), "ASKBOX", GetActiveWindow(), lpfnAskDlgProc );
	FreeProcInstance ( lpfnAskDlgProc );

	return ( nResult );
}

BOOL CALLBACK sAskDialogProc ( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	short	id;
	char	szTitle[256];

	switch ( message )
	{
		case WM_INITDIALOG:
			GCenterWindow ( hDlg );
			GetWindowText ( GGetMainWindow(), szTitle, 255 );
			SetWindowText ( hDlg, szTitle );
			SetWindowText ( GetDlgItem ( hDlg, ASK_QUESTION_TEXT_ID ), sPrompt );
			SetWindowText ( GetDlgItem ( hDlg, ASK_ANSWER_TEXT_ID ), sText );
			return ( TRUE );

		case WM_COMMAND:
			switch ( id = LOWORD ( wParam ) )
			{
				case ASK_OK_BUTTON_ID:
					GetWindowText ( GetDlgItem ( hDlg, ASK_ANSWER_TEXT_ID ), sText, 255 );
					EndDialog ( hDlg, TRUE );
					return ( TRUE );

				case ASK_CANCEL_BUTTON_ID:
					EndDialog ( hDlg, FALSE );
					return ( TRUE );
			}
			break;
	}

	return ( FALSE );
}

/**************************  GShowProgressDialog  ******************************/

#define G_PROGRESS_TASK_TEXT		1
#define G_PROGRESS_CANCEL_BUTTON	2
#define G_PROGRESS_RECT				3

static HWND	sProgressDialog;
static long	sNumTasks, sTaskNum, sTaskMin, sTaskMax;

GWindowPtr GShowProgressDialog ( long lNumTasks, long lTaskNum, long lTaskMin, long lTaskMax,
char *lpszTask, char *lpszTitle, int fShowCancel )
{
	FARPROC			lpfnDlgProc;

	/*** If we haven't already created the progress dialog, do so. ***/

	if ( sProgressDialog == NULL )
	{
		lpfnDlgProc = MakeProcInstance ( sProgressDialogProc, GGetInstance() );
		sProgressDialog = CreateDialog ( GGetInstance(), "PROGRESSBOX", GGetMainWindow(), lpfnDlgProc );
	}

	/*** If the progress dialog was not created, return NULL. ***/

	if ( sProgressDialog == NULL )
		return ( NULL );

	/*** If specified, set the progress dialog window's title and task strings. ***/

	if ( lpszTitle )
		SetWindowText ( sProgressDialog, lpszTitle );

	if ( lpszTask )
    	SetWindowText ( GetDlgItem ( sProgressDialog, G_PROGRESS_TASK_TEXT ), lpszTask );

	/*** Show or hide the dialog's "Cancel" button, as specified. ***/

	if ( fShowCancel )
		ShowWindow ( GetDlgItem ( sProgressDialog, G_CANCEL_BUTTON ), SW_SHOW );
	else
    	ShowWindow ( GetDlgItem ( sProgressDialog, G_CANCEL_BUTTON ), SW_HIDE );

	/*** Record the total number of tasks to perform, and the number,
		 minimum, and maximum value of the current task. ***/

	sNumTasks = lNumTasks;
	sTaskNum = lTaskNum;
	sTaskMin = lTaskMin;
	sTaskMax = lTaskMax;

	/*** Display and activate the dialog window, disable the application's other
		 windows, and draw the dialog's progress bar.  Return a pointer to the
		 progress dialog window. ***/

	GShowModalDialog ( sProgressDialog );
	GUpdateProgressDialog ( 0 );

    return ( sProgressDialog );
}

/***********************  GUpdateProgressDialog  ************************************/

int GUpdateProgressDialog ( long lValue )
{
	short		width;
	long		lCurrentTick;
	static long	lNextUpdateTick = 0;
	MSG			message;
	RECT		rect;
	HDC			hDC;
	HBRUSH		hBrush;

	lCurrentTick = GetTickCount();
	if ( lCurrentTick < lNextUpdateTick )
		return ( TRUE );
	else
		lNextUpdateTick = lCurrentTick + 100;

	GetWindowRect ( GetDlgItem ( sProgressDialog, G_PROGRESS_RECT ), &rect );
	ScreenToClient ( sProgressDialog, (LPPOINT) &rect.left );
	ScreenToClient ( sProgressDialog, (LPPOINT) &rect.right );

	hDC = GetDC ( sProgressDialog );
	hBrush = SelectObject ( hDC, GetStockObject ( NULL_BRUSH ) );
	Rectangle ( hDC, rect.left, rect.top, rect.right, rect.bottom );
	SelectObject ( hDC, hBrush );

	width = ( rect.right - rect.left ) / sNumTasks;
	width = width * ( lValue - sTaskMin ) / ( sTaskMax - sTaskMin ) + width * ( sTaskNum - 1 );
	rect.right = rect.left + width;

    hBrush = SelectObject ( hDC, GetStockObject ( BLACK_BRUSH ) );
	Rectangle ( hDC, rect.left, rect.top, rect.right, rect.bottom );
    SelectObject ( hDC, hBrush );
    ReleaseDC ( sProgressDialog, hDC );

	while ( PeekMessage ( &message, NULL, 0, 0, PM_REMOVE ) )
	{
		if ( ! IsDialogMessage ( sProgressDialog, &message ) )
		{
			TranslateMessage ( &message );
			DispatchMessage ( &message );
		}
	}

	return ( TRUE );
}

/**************************  GHideProgressDialog  *******************************/

void GHideProgressDialog ( void )
{
	GHideModalDialog ( sProgressDialog );
}

BOOL CALLBACK sProgressDialogProc ( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	short	wControlID;

	switch ( message )
	{
		case WM_COMMAND:
			switch ( wControlID = LOWORD ( wParam ) )
			{
				case G_CANCEL_BUTTON:
					return ( TRUE );
			}
			break;
	}

	return ( FALSE );
}

