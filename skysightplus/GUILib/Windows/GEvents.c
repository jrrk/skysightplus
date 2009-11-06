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

#include "GUILib.h"
#define GGLOBAL
#include "GUIPriv.h"
#undef GGLOBAL

static int 		sDoMenuCommand ( HWND, HMENU, UINT );
static void		DoPushButtonClick ( HWND, HWND, WORD, int );
static void		GDoActivateMessage ( HWND, BOOL );

static HINSTANCE		sInstance = NULL;
static GEventProcPtr	sEventProc = NULL;
static HWND				sMenuEventWindow = NULL;

/**********************  GUILibInitialize  *******************************/

int GUILibInitialize ( GHandle hInstance )
{
	/*** Initialize GUILib private data ***/

	sInstance = hInstance;

	gClientWindow = NULL;
	gMenuBar = NULL;
	gAccelerators = NULL;
	sEventProc = NULL;

	/*** Initialize GUILib's window list ***/

    gGWindowDataList = NULL;

    /*** Initialize the common control library. ***/

#ifdef GWIN32
    InitCommonControls();
#endif
    
	/*** Initialize 3D controls ***/

#ifdef GWINCTL3D
	Ctl3dRegister ( hInstance );
	Ctl3dAutoSubclass ( hInstance );
#endif

#ifdef GWINCOM32S
	RegisterCom32s ( hInstance );
#endif

	return ( TRUE );
}

/*****************************  GUILibCleanUp  *********************************/

void GUILibCleanUp ( void )
{
#ifdef GWINCTL3D
	Ctl3dUnregister ( sInstance );
#endif

#ifdef GWINCOM32S
	UnRegisterCom32s ( sInstance );
#endif
}

/****************************  GEnterMainLoop  *************************************/

int GEnterMainLoop ( GEventProcPtr proc )
{
	MSG				message;

	/*** Store a global pointer to the event-handling prcedure.  Inform
		 the application that we are about to enter the event loop, and
		 return if it tells us not to. ***/

	sEventProc = proc;

	GSendEvent ( G_ENTRY_EVENT, NULL, 0, 0 );

	while ( GGetNextEvent ( &message ) )
		GProcessEvent ( &message );

	/*** Return the value passed to PostQuitMessage(), which appears here
		 in the wParam field of the "Quit" message. ***/

	return ( message.wParam );
}

/***********************  GExitMainLoop  *********************************/

void GExitMainLoop ( int result )
{
	/*** If we our application is running inside a window, we don't quit the
		 message loop directly.  Instead, we tell the application's window
		 to close, which causes it to destroy itself, which then posts the
		 quit message.  Posting the quit message before closing the window
		 seems to cause some ugliness, which is why we do it this convoluted way. ***/

	if ( GGetMainWindow() )
		SendMessage ( GGetMainWindow(), WM_CLOSE, 0, 0 );
	else
		PostQuitMessage ( result );
}

/*************************  GGetNextEvent  ********************************/

int GGetNextEvent ( GEventPtr lpMsg )
{
	if ( PeekMessage ( lpMsg, NULL, 0, 0, PM_REMOVE ) == FALSE )
	{
		lpMsg->message = WM_NULL;
		lpMsg->hwnd = NULL;
        lpMsg->wParam = 0;
        lpMsg->lParam = 0;
	}

	if ( lpMsg->message == WM_QUIT )
		return ( FALSE );
	else
    	return ( TRUE );
}

/*************************  GProcessEvent  ********************************/

void GProcessEvent ( GEventPtr lpMsg )
{
	HWND	hWnd;

	/*** If we have retrieved a NULL event, and no modal dialog window
	     is present, call the application's main event-handling function
	     directly. ***/

	if ( lpMsg->message == WM_NULL )
	{
		if ( GGetModalDialogWindow() == NULL )
			GSendEvent ( G_NULL_EVENT, NULL, 0, 0 );
		return;
	}

	/*** First check all of the dialog windows owned by the main window
		 to see if the the message is a dialog message intended for any
		 of them.  If so, we don't need to translate or dispatch it because
		 IsDialogMessage() does that for us. ***/

	for ( hWnd = GGetMainWindow(); hWnd; hWnd = GetWindow ( hWnd, GW_HWNDPREV ) )
		if ( GIsDialogWindow ( hWnd ) && IsDialogMessage ( hWnd, lpMsg ) )
			return;

	/*** If MDI support has been enabled by creating an MDI client window,
		 check to see if the message is an MDI system accelerator keystroke.
		 If so, we don't need to translate or dispatch it any further. ***/

	if ( gClientWindow && TranslateMDISysAccel ( gClientWindow, lpMsg ) )
		return;

	/*** If not, see if the message corresponds to an application-defined
		 keyboard accelerator.  If not, translate and dispatch it. ***/

	if ( TranslateAccelerator ( GGetMainWindow(), gAccelerators, lpMsg ) )
		return;

	TranslateMessage ( lpMsg );
	DispatchMessage ( lpMsg );
}

/***************************  GEventPending  *********************************/

int GEventPending ( void )
{
	DWORD dwResult = GetQueueStatus ( QS_ALLEVENTS );

    if ( HIWORD ( dwResult ) > 0 )
    	return ( TRUE );
    else
    	return ( FALSE );
}

/***************************  GGetInstance  *******************************/

HINSTANCE GGetInstance ( void )
{
	return ( sInstance );
}

/***************************  GSendEvent  *********************************/

int GSendEvent ( short wEvent, GWindowPtr hWnd, long lParam1, long lParam2 )
{
	GWindowDataPtr lpWindowData;

	/*** If the event is associated with a window, search the list of
	     windows for the one associated with the event.  If we find it,
	     and that window has its own event-handling procedure, call it. ***/

	if ( hWnd )
	{
		lpWindowData = GGetGWindowData ( hWnd );
		if ( lpWindowData != NULL )
			if ( lpWindowData->hWnd == hWnd && lpWindowData->pfnProc )
				return ( (*lpWindowData->pfnProc) ( wEvent, hWnd, lParam1, lParam2 ) );
	}

	/*** Call the application's general event-handling procedure by default. ***/
	
	if ( sEventProc )
		return ( (*sEventProc) ( wEvent, hWnd, lParam1, lParam2 ) );
	else
		return ( TRUE );
}

/*** GGetMenuEventWindow *****************************************************/

GWindowPtr GGetMenuEventWindow ( void )
{
	return ( sMenuEventWindow );
}

/*** GSetMenuEventWindow *****************************************************/

void GSetMenuEventWindow ( GWindowPtr hWnd )
{
	sMenuEventWindow = hWnd;
}

/*************************  GSetMouseEventWindow  ****************************/

void GSetMouseEventWindow ( GWindowPtr hwnd )
{
	if ( hwnd )
		SetCapture ( hwnd );
	else
		ReleaseCapture();
}

/**************************  GGetMouseEventWindow  ****************************/

GWindowPtr GGetMouseEventWindow ( void )
{
	return ( GetCapture() );
}

/***************************  GGetTickCount  *****************************/
 
long GGetTickCount ( void )
{
	return ( GetTickCount() );
}

/******************************  GWait  **********************************/
 
void GWait ( long lNumTicks )
{
	long lCurrentTick, lStartTick = GetTickCount();
	
	do
	{
		lCurrentTick = GetTickCount();
	}
	while ( ( lCurrentTick - lStartTick ) < lNumTicks );
}

/*************************  GDoBackgroundTask  ***************************/

int GDoBackgroundTask ( long lMessageInterval )
{
	long			lCurrentTick;
	static long		lLastMessageTick = 0;
	MSG				msg;

	lCurrentTick = GetTickCount();

	if ( lCurrentTick - lLastMessageTick >= lMessageInterval )
	{
		if ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage ( &msg );
			DispatchMessage ( &msg );
		}

		lLastMessageTick = lCurrentTick;
	}

	return ( TRUE );
}

/****************************  GDoMessage  *******************************/

int GDoMessage ( HWND hWnd, UINT nMsg, UINT wParam, LONG lParam )
{
	int				fRespond = TRUE;
	short			wX, wY, wEvent;
	static HWND		hWndShow;
	PAINTSTRUCT		ps;
    LPNMHDR			lpNMHdr;
	GWindowDataPtr	lpWindowData;

	switch ( nMsg )
	{
		case WM_CREATE:
			break;

		/*** If the window is visible, we return FALSE to let Windows set
			 the initial keyboard focus.  Otherwise, we return TRUE to prevent
			 Windows from setting focus to the (invisible) dialog window
			 and causing the currently-active window to lose focus. ***/

		case WM_INITDIALOG:
#ifdef GWINCTL3D
			Ctl3dSubclassDlgEx ( hWnd, CTL3D_ALL );
#endif

			if ( GetWindowStyle ( hWnd ) & WS_VISIBLE )
				fRespond = FALSE;
			else
				fRespond = TRUE;

			break;

		/*** When we destroy a window, we also need to remove GUILib's
			 corresponding private window data record.  If we are
			 destroying the application's window, exit the event loop.
			 If we are destroying the window which should receive menu
			 events, set the menu event window to NULL.  (The next window
			 to receive menu events will do so when it receives its
			 WM_SETFOCUS.) ***/

		case WM_DESTROY:
			GDeleteGWindowData ( GGetGWindowData ( hWnd ) );
			
			if ( hWnd == GGetMenuEventWindow() )
				GSetMenuEventWindow ( NULL );

			if ( hWnd == GGetMainWindow() )
				PostQuitMessage ( 0 );
			break;

		/*** If the user tries to close the application's main window,
			 we send a quit event, rather than a close event, because
			 Windows will have nothing to send messages to.  Otherwise,
			 we just send a normal close event.  If the application tells
			 us not to respond, we return zero to prevent Windows from
			 destroying the window. ***/

		case WM_CLOSE:
			if ( hWnd == GGetMainWindow() )
				fRespond = GSendEvent ( G_EXIT_EVENT, NULL, 0, 0 );
			else
				fRespond = GSendEvent ( G_CLOSE_EVENT, hWnd, 0, 0 );
            break;

		case WM_SYSCOMMAND:
			if ( ( wParam & 0xFFF0 ) == SC_MINIMIZE )
				fRespond = GSendEvent ( G_MINIMIZE_EVENT, hWnd, 0, 0 );

			if ( ( wParam & 0xFFF0 ) == SC_MAXIMIZE )
				fRespond = GSendEvent ( G_MAXIMIZE_EVENT, hWnd, 0, 0 );

			if ( ( wParam & 0xFFF0 ) == SC_RESTORE )
				fRespond = GSendEvent ( G_RESTORE_EVENT, hWnd, 0, 0 );

			break;

		/*** If a window is about to be made visible, save its handle
			 to let us avoid sending the WM_SIZE message which follows
			 to the application.

		case WM_SHOWWINDOW:
			if ( wParam )
				hWndShow = hWnd;
			break;

            ***/

		/*** When the window is resized, first move its size box control
			 (if any) to the proper position.  If the window is maximized,
			 disable the size box control; re-enable it when then window
			 is restored.  For other resize events, only send a size events
			 to the application if the window is visible, and the window is
			 not being minimized. (If the window is being minimized, the
			 window's dimensions end up becoming 0,0, which can cause problems.) ***/

		case WM_SIZE:
			wX = LOWORD ( lParam );
			wY = HIWORD ( lParam );

           	GMoveSizeBox ( hWnd );
			
			if ( wParam == SIZE_MAXIMIZED )
			{
				EnableWindow ( GetDlgItem ( hWnd, ID_SIZEBOX ), FALSE );
 				GSendEvent ( G_SIZE_EVENT, hWnd, wX, wY );
			}

			else if ( wParam != SIZE_MINIMIZED )
			{
				if ( IsWindowVisible ( hWnd ) )
	 				GSendEvent ( G_SIZE_EVENT, hWnd, wX, wY );
			}

//			hWndShow = NULL;
			break;

		/*** When windows are activated or deactivated, we get
			 WM_ACTIVATE or WM_MDIACTIVATE messages.  First extract
			 the handles of the window that is being activated and
			 the window that is being deactivated, then process the
			 message. ***/

		case WM_ACTIVATE:

			if ( LOWORD ( wParam ) == WA_INACTIVE )
				GDoActivateMessage ( hWnd, FALSE );
			else
				GDoActivateMessage ( hWnd, TRUE );
			
			break;

		case WM_MDIACTIVATE:

			if ( hWnd == (HWND) wParam )
				GDoActivateMessage ( (HWND) wParam, FALSE );

			if ( hWnd == (HWND) lParam )
				GDoActivateMessage ( (HWND) lParam, TRUE );

			break;

		/*** If our application is being deactivated or activated,
			 we send a suspend or resume event to the application's
			 main event handling procedure. ***/

		case WM_ACTIVATEAPP:
			if ( wParam )
				GSendEvent ( G_RESUME_EVENT, NULL, 0, 0 );
			else
				GSendEvent ( G_SUSPEND_EVENT, NULL, 0, 0 );
			break;

		/*** When Windows wants to erase our window's background,
			 first check GUILib's private window data record to
			 see if the automatic-background-erase flag is set.
			 If not, return a non-zero value to prevent Windows
			 from erasing the window background. ***/

		case WM_ERASEBKGND:
			if ( ( lpWindowData = GGetGWindowData ( hWnd ) ) != NULL )
				if ( lpWindowData->lFlags & G_ERASE_BACKGROUND )
					fRespond = TRUE;
				else
                	fRespond = FALSE;
			break;

		case WM_PAINT:
			BeginPaint ( hWnd, &ps );
			GSendEvent ( G_UPDATE_EVENT, hWnd, 0, 0 );
			EndPaint ( hWnd, &ps );
			return ( 0 );

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			wX = LOWORD ( lParam );
			wY = HIWORD ( lParam );
			GSendEvent ( G_MOUSE_DOWN_EVENT, hWnd, wX, wY );
			break;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			wX = LOWORD ( lParam );
			wY = HIWORD ( lParam );
			GSendEvent ( G_MOUSE_UP_EVENT, hWnd, wX, wY );
			break;

		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			wX = LOWORD ( lParam );
			wY = HIWORD ( lParam );
			GSendEvent ( G_DOUBLE_CLICK_EVENT, hWnd, wX, wY );
			break;

		/*** When the mouse moves, we may send a mouse-moved or mouse-drag
			 event.  First obtain the mouse's position. ***/

		case WM_MOUSEMOVE:

			wX = LOWORD ( lParam );
			wY = HIWORD ( lParam );

			/*** If the mouse button is down, send a mouse-drag event to
                 the application.  Otherwise, send a mouse-moved event. ***/

			if ( IsLButtonDown() || IsMButtonDown() || IsRButtonDown() )
				wEvent = G_MOUSE_DRAG_EVENT;
            else
            	wEvent = G_MOUSE_MOVED_EVENT;

            /*** We only pass the event on if the window has been designated
                 to capture all mouse input, or if it is the active window. ***/

            if ( hWnd == GetCapture() || hWnd == GGetActiveWindow() )
				GSendEvent ( wEvent, hWnd, wX, wY );

			break;

		/*** When the user presses a key, Windows generates a key-down
			 message whose wParam is the virtual-key code of the key that
			 was pressed.  We pass this to the application as param1.
			 WE ARE NOT HANDLING TYPEMATIC KEYSTROKES YET!!! ***/

		case WM_KEYDOWN:
			GSendEvent ( G_KEY_DOWN_EVENT, hWnd, wParam, 0 );
			break;

		/*** If the keystroke actually generates a character (e.g. the 'A'
			 key as opposed to the CTRL key), Windows generates a WM_CHAR
			 message whose wParam is the ASCII code of the character that
			 was generated.  We pass this on to the application as param2,
			 to distinguish it from the key code. ***/

		case WM_CHAR:
			GSendEvent ( G_KEY_DOWN_EVENT, hWnd, 0, wParam );
			break;

		/*** When the user releases the key, Windows generates a key-up
			 message with the virtual-key-code in wParam.  We pass this
			 to the application in param1. ***/

		case WM_KEYUP:
			GSendEvent ( G_KEY_UP_EVENT, hWnd, wParam, 0 );
			break;

		case WM_VSCROLL:
		case WM_HSCROLL:
			HANDLE_WM_VSCROLL ( hWnd, wParam, lParam, GDoScrollMessage );
            break;

		/*** A window may receive WM_COMMAND messages from either its menu bar
		     or one of its child window controls.  To see which, extract the
		     child window handle and command ID from the other message parameters. ***/

		case WM_COMMAND:
        	HANDLE_WM_COMMAND ( hWnd, wParam, lParam, GDoCommandMessage );
			break;

        case WM_NOTIFY:
        	lpNMHdr = (LPNMHDR) lParam;
        	GDoCommandMessage ( hWnd, lpNMHdr->idFrom, lpNMHdr->hwndFrom, lpNMHdr->code );
        	break;

#ifdef GWINCTL3D
		case WM_SYSCOLORCHANGE:
			Ctl3dColorChange();
			break;
#endif
	}

    return ( fRespond );
}

/*** GDoActivateMessage ******************************************************/

void GDoActivateMessage ( HWND hWnd, BOOL fActivate )
{
	HWND	hWndMDIClient;

	/*** If the window is being activated, set keyboard
		 focus to the window.  Then see if the window should
		 receive menu events, and if so, designate it as the
		 menu event window.  Finally, send it an activate
		 event. ***/ 

	if ( fActivate == TRUE )
	{
		SetFocus ( hWnd );

		hWndMDIClient = GGetMDIClientWindow();
		if ( hWndMDIClient != NULL && GetParent ( hWnd ) == hWndMDIClient )
			GSetMenuEventWindow ( hWnd );
		else if ( hWnd == GGetMainWindow() )
			GSetMenuEventWindow ( hWnd );

		GSendEvent ( G_ACTIVATE_EVENT, hWnd, 0, 0 );
	}
	else
	{
		/*** If the window is being deactivated, send the deactivate
			 event now. ***/

		GSendEvent ( G_DEACTIVATE_EVENT, hWnd, 0, 0 );
	}
}

/******************************  GDoScrollMessage  ****************************/

void GDoScrollMessage ( HWND hWnd, HWND hCtl, UINT nCode, int nPos )
{
	/*** When a scroll-bar control is clicked, Windows generates scroll messages
		 rather than command messages.  We get around this inconsistency by
		 translating them to control events before sending them to the application.
		 Here we extract the scroll bar control's handle, ID, and the code
		 indicating the part of the scroll bar that was clicked before sending them
		 to the application. ***/

	switch ( nCode )
	{
        /*** When the scroll bar's "thumb" is dragged or repositioned, we set
             the scroll position to the value at which the thumb is currently
             located before sending the control event along to the application.
             We do this to maintiain consistency with the Mac version of GUILib;
             that's the only way in which both versions can obtain the scroll bar's
             current control value while the thumb is being dragged. ***/

        case SB_THUMBTRACK:
        	SetScrollPos ( hCtl, SB_CTL, nPos, FALSE );
            GSendEvent ( G_CONTROL_EVENT, hWnd, GetWindowID ( hCtl ), G_TRACK_THUMB );
        	break;

		case SB_THUMBPOSITION:
			SetScrollPos ( hCtl, SB_CTL, nPos, TRUE );
			GSendEvent ( G_CONTROL_EVENT, hWnd, GetWindowID ( hCtl ), G_SCROLL_THUMB );
			break;

		case SB_PAGEUP:
			GSendEvent ( G_CONTROL_EVENT, hWnd, GetWindowID ( hCtl ), G_SCROLL_PAGE_UP );
			break;

		case SB_PAGEDOWN:
			GSendEvent ( G_CONTROL_EVENT, hWnd, GetWindowID ( hCtl ), G_SCROLL_PAGE_DOWN );
			break;

		case SB_LINEUP:
			GSendEvent ( G_CONTROL_EVENT, hWnd, GetWindowID ( hCtl ), G_SCROLL_LINE_UP );
			break;

		case SB_LINEDOWN:
			GSendEvent ( G_CONTROL_EVENT, hWnd, GetWindowID ( hCtl ), G_SCROLL_LINE_DOWN );
			break;
	}
}

/*** GSupressCommandMessage ***************************************************

	Causes the next WM_COMMAND message for a particular control to be
    supressed, rather than passed on to the GUILib application's event-
    handling procedure.

	void GSupressCommandMessage ( HWND hCtl )

    (hCtl): handle to control whose next WM_COMMAND message you want supressed.

    The function returns nothing.
    
******************************************************************************/

static HWND	hCtlCommandSupressed = NULL;

void GSupressCommandMessage ( HWND hCtl )
{
	hCtlCommandSupressed = hCtl;
}

/*************************  GDoCommandMessage  **********************************/

void GDoCommandMessage ( HWND hWnd, int id, HWND hCtl, UINT nCode )
{
	char szClassName[32];

	/*** If the WM_COMMAND message came from the menu bar, rather than from
         a child window control, the child window handle passed in the parameters
         with the message is NULL.  Determine which window the menu event should
	     be sent to, then dispatch it.  If the window tells us to take the default
		 response, pass the event on to the application's main event handler. ***/

	if ( hCtl == NULL )
	{
		HWND	hWnd = GGetMenuEventWindow();
		long	lMenu = id / 100;
		long	lItem = id % 100;
		int		nResult;
		
		nResult = GSendEvent ( G_MENU_EVENT, hWnd, lMenu, lItem );

		if ( hWnd != NULL && nResult )
			GSendEvent ( G_MENU_EVENT, NULL, lMenu, lItem );
	}

    /*** If there is a valid child window handle, the message came from a
		 child window control.  Unless we are supressing messages for that
         control, inform the application that the control was clicked. ***/
         
	else
	{
        if ( hCtl != hCtlCommandSupressed )
        {
			GetClassName ( hCtl, szClassName, 31 );

			if ( lstrcmpi ( szClassName, "BUTTON" ) == 0 )
			{
        		if ( nCode == BN_CLICKED )
            		DoPushButtonClick ( hWnd, hCtl, id, nCode );

            	if ( nCode == BN_DOUBLECLICKED )
					GSendEvent ( G_CONTROL_EVENT, hWnd, id, nCode );
			}

			if ( lstrcmpi ( szClassName, "COMBOBOX" ) == 0 )
				if ( nCode == CBN_SELCHANGE )
					GSendEvent ( G_CONTROL_EVENT, hWnd, id, nCode );

			if ( lstrcmpi ( szClassName, "EDIT" ) == 0 )
				if ( nCode == EN_CHANGE )
					GSendEvent ( G_CONTROL_EVENT, hWnd, id, nCode );

			if ( lstrcmpi ( szClassName, WC_TABCONTROL ) == 0 )
        		if ( nCode == TCN_SELCHANGE )
            		GSendEvent ( G_CONTROL_EVENT, hWnd, id, nCode );
		}
    }
}

/***  DoPushButtonClick  ****************************************************

	For reasons that I cannot fathom, Windows 95 seems to occasionally
    get into this mode where push buttons send two BN_CLICKED notifications
    to their parent window even if they have only been clicked once.
    This routine detects that behavior and prevents the second click from
    being passed on to the the application's event-handling function.

*****************************************************************************/

void DoPushButtonClick ( HWND hWnd, HWND hButton, WORD idButton, int nCode )
{
	DWORD			dwTick;
	static HWND		hLastButtonClicked = NULL;
	static DWORD	dwLastClickTick = 0;

    /*** First get the time at which the mouse-button-click message was sent. ***/
    
    dwTick = GetMessageTime();
//    dwTick = GetTickCount();

    /*** If the button being clicked now is the same as the last button that
         was clicked, and the time difference between the clicks is less than
	     the double-click time interval, we DO NOT pass the event on to the
	     application's event handling function.  (Besides, shouldn't this be
         a BN_DOUBLECLICKED notification???!!!) ***/

	if ( hButton != hLastButtonClicked || dwTick - dwLastClickTick > GetDoubleClickTime() )
    	GSendEvent ( G_CONTROL_EVENT, hWnd, idButton, nCode );
        
    /*** Save the button control handle and the current system clock value
         so we can compare them the next time this function gets called. ***/

    hLastButtonClicked = hButton;
    dwLastClickTick = dwTick;
}
