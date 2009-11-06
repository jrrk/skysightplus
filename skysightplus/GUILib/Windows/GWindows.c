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

/*** header files ***/

#include "GUILib.h"
#define GLOBAL extern
#include "GUIPriv.h"
#undef GLOBAL

/*** private functions ***/

static LRESULT CALLBACK sWindowProc ( HWND, UINT, WPARAM, LPARAM );
static LRESULT CALLBACK sFrameWndProc ( HWND, UINT, WPARAM, LPARAM );

/*** private data ***/

static HWND	     sMainWindow = NULL;

/***  GCreateWindow  ******************************************************/

GWindowPtr GCreateWindow ( short wClass, char *lpszTitle,
short wLeft, short wTop, short wWidth, short wHeight, int fVisible,
char fType, long lData, GEventProcPtr lpfnEventProc )
{
	int				nLeft = wLeft > 0 ? wLeft : CW_USEDEFAULT;
	int				nTop = wTop > 0 ? wTop : CW_USEDEFAULT;
	int				nWidth = wWidth > 0 ? wWidth : CW_USEDEFAULT;
	int				nHeight = wHeight > 0 ? wHeight : CW_USEDEFAULT;
	DWORD			dwStyle = WS_BORDER, dwExStyle = 0;
	HWND		    hWnd = NULL;
    HINSTANCE		hInstance = GGetInstance();
	LPSTR			lpszClass = MAKEINTRESOURCE ( wClass );
	GWindowDataPtr  lpWindowData;

	/*** Compute Windows style bit-flags from type bit-flags. ***/

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

	if ( fType & G_MOVEABLE )
		dwStyle |= WS_CAPTION;

	if ( fType & G_SIZEABLE )
		dwStyle |= WS_THICKFRAME;

	if ( fType & G_MINIMIZABLE )
		dwStyle |= WS_MINIMIZEBOX;

	if ( fType & G_MAXIMIZABLE )
		dwStyle |= WS_MAXIMIZEBOX;

	if ( fType & G_CLOSEABLE )
		dwStyle |= WS_SYSMENU;

	if ( fType & G_FLOATING )
		dwExStyle = WS_EX_TOOLWINDOW;

	/*** Create a new private window data record; return NULL on failure. ***/

	lpWindowData = GCreateGWindowData ( wClass, lpfnEventProc, lData );
	if ( lpWindowData == NULL )
		return ( NULL );

	/*** Now create the window.  If we have not yet created the application's
		 main window, we do so now; otherwise, the window we create will be
		 a floating window owned by, or an MDI child window of, the main window. ***/

	if ( sMainWindow == NULL )
	{
		GRegisterWindowClass ( lpszClass, sizeof ( long ), sWindowProc, COLOR_WINDOW );

		hWnd = CreateWindowEx ( dwExStyle, lpszClass, lpszTitle, dwStyle,
			   nLeft, nTop, nWidth, nHeight, NULL, NULL, hInstance, 0 );

		sMainWindow = hWnd;
	}
    else
	{
		/*** If the style specifies a floating window, create the window with
			 the application's main window as its parent. ***/

		if ( fType & G_FLOATING )
		{
			GRegisterWindowClass ( lpszClass, sizeof ( long ), sWindowProc, COLOR_WINDOW );

			hWnd = CreateWindowEx ( dwExStyle, lpszClass, lpszTitle, dwStyle | WS_CHILD,
				   nLeft, nTop, nWidth, nHeight, sMainWindow, NULL, hInstance, 0 );
		}
		else
		{
			/*** If the application has enabled MDI support by creating an MDI client
				 window, we create an MDI child window by telling the MDI client
				 window to create one. ***/

			if ( gClientWindow )
			{
				GRegisterWindowClass ( lpszClass, sizeof ( long ), sWindowProc, COLOR_WINDOW );

				hWnd = CreateWindowEx ( dwExStyle | WS_EX_MDICHILD, lpszClass, lpszTitle, dwStyle | WS_CHILD,
					   nLeft, nTop, nWidth, nHeight, gClientWindow, (HMENU) 1, hInstance, 0 );
			}
		}
	}

	/*** If we failed to create the window, delete GUILib's private window
		 data record for that window, and return a NULL pointer. ***/

	if ( hWnd == NULL )
	{
		GDeleteGWindowData ( lpWindowData );
		return ( NULL );
	}

	/*** Attach GUILib's private window data record to the window, enable
		 automatic erasing, create the size boz, and send the window its
		 initial event. ***/

	GSetGWindowData ( hWnd, lpWindowData );
	GEraseOnUpdates ( hWnd, TRUE );
	GCreateSizeBox ( hWnd, FALSE );
	GSendEvent ( G_INITIAL_EVENT, hWnd, 0, 0 );

	return ( hWnd );
}

/***  GCreateFrameWindow  *****************************************************/

GWindowPtr GCreateFrameWindow ( short wClass, char *lpszTitle,
short wLeft, short wTop, short wWidth, short wHeight, int bVisible )
{
	int     nLeft = wLeft > 0 ? wLeft : CW_USEDEFAULT;
	int     nTop = wTop > 0 ? wTop : CW_USEDEFAULT;
	int     nWidth = wWidth > 0 ? wWidth : CW_USEDEFAULT;
	int     nHeight = wHeight > 0 ? wHeight : CW_USEDEFAULT;
	LPSTR	lpszClass = MAKEINTRESOURCE ( wClass );
	DWORD	dwStyle;


	/*** If the application has already created a window, return NULL to
		 indicate failure. ***/

	if ( GGetMainWindow() )
		return ( NULL );

	/*** Register the frame window class. ***/

	GRegisterWindowClass ( lpszClass, 0, sFrameWndProc, COLOR_APPWORKSPACE );

	/*** Create the application's main frame window based on the type,
		 title, and rectangle provided. ***/

	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	sMainWindow = CreateWindow ( lpszClass, lpszTitle, dwStyle,
	                             nLeft, nTop, nWidth, nHeight,
	                             NULL, NULL, GGetInstance(), NULL );

	/*** If successful, load the frame window's menu bar and accelerator
		 table.  Then attach the menu bar to the window, and display the
		 frame window in its maximized state. ***/

	if ( sMainWindow )
	{
		gMenuBar = LoadMenu ( GGetInstance(), lpszClass );
		gAccelerators = LoadAccelerators ( GGetInstance(), lpszClass );

		SetMenu ( sMainWindow, gMenuBar );



		if ( bVisible )

		{
			ShowWindow ( sMainWindow, SW_SHOWNORMAL );
			UpdateWindow ( sMainWindow );
		}
	}


    return ( sMainWindow );
}

/***************************  GDeleteWindow  ********************************/

void GDeleteWindow ( GWindowPtr hWnd )
{
	if ( ( GetParent ( hWnd ) == gClientWindow ) && ( gClientWindow != NULL ) )
		SendMessage ( gClientWindow, WM_MDIDESTROY, (WPARAM) hWnd, 0 );
	else
		DestroyWindow ( hWnd );
}

/**************************  GGetMainWindow  ******************************/

HWND GGetMainWindow ( void )
{
	return ( sMainWindow );
}

/**************************  GGetMDIClientWindow  *************************/

HWND GGetMDIClientWindow ( void )
{
	return ( gClientWindow );
}

/*** GGetWindowClass *******************************************************/

short GGetWindowClass ( GWindowPtr hWnd )
{
	GWindowDataPtr	data = GGetGWindowData ( hWnd );
	
	if ( data != NULL )
		return ( data->wClass );
	else
		return ( 0 );	
}

/*** GGetWindowCursor ******************************************************/

GCursorPtr GGetWindowCursor ( GWindowPtr hWnd )
{
	HCURSOR	hCursor;

    hCursor = (HCURSOR) GetClassLong ( hWnd, GCL_HCURSOR );

    return ( hCursor );
}

/*** GSetWindowCursor ******************************************************/

void GSetWindowCursor ( GWindowPtr hWnd, GCursorPtr hCursor )
{
    if ( hCursor == NULL )
    	hCursor = LoadCursor ( NULL, IDC_ARROW );
        
	SetClassLong ( hWnd, GCL_HCURSOR, (LONG) hCursor );
}

/***  GGetWindowTitle  *****************************************************/

char *GGetWindowTitle ( GWindowPtr hwnd, char *lpszTitle )
{
	GetWindowText ( hwnd, lpszTitle, 255 );
	return ( lpszTitle );
}

/***  GSetWindowTitle  *****************************************************/

void GSetWindowTitle ( GWindowPtr hwnd, char *lpszTitle )
{
	SetWindowText ( hwnd, lpszTitle );
}

/***  GGetWindowContentRect  ***********************************************/

GRectPtr GGetWindowContentRect ( GWindowPtr hwnd, GRectPtr lpRect )
{
	GetClientRect ( hwnd, lpRect );
	return ( lpRect );
}

/***  GSetWindowContentRect  ***********************************************/

void GSetWindowContentRect ( GWindowPtr hWnd, GRectPtr lpRect )
{
	HWND	hWndParent;
	RECT	rectWindow, rectClient;
	int		nLeft, nTop, nWidth, nHeight;

	/*** Determine the window's current client area rectangle
	     and frame rectangle.  Note that the frame rectangle's
	     coordinates are relative to the top left corner of the
	     screen. ***/

	GetClientRect ( hWnd, &rectClient );
	GetWindowRect ( hWnd, &rectWindow );

	/*** Because MoveWindow() always moves a window relative to
	     the client area of its parent window, we need to convert
		 them from screen coordinates to client coordinates of the
		 parent window (if there is one!) ***/

	hWndParent = GetParent ( hWnd );
	if ( hWndParent != NULL )
	{
		ScreenToClient ( hWndParent, (LPPOINT) &rectWindow.left );
		ScreenToClient ( hWndParent, (LPPOINT) &rectWindow.right );
	}

	/*** Now compute a new top left corner coordinates, width, and
	     height for the window's frame rectangle, such that the window's
		 content rectangle will change to match the rectangle passed
	     by the caller. ***/

	nLeft   = rectWindow.left
			+ ( lpRect->left - rectClient.left );

	nTop    = rectWindow.top
			+ ( lpRect->top - rectClient.top );

	nWidth  = ( rectWindow.right - rectWindow.left )
			+ ( lpRect->right - lpRect->left )
			- ( rectClient.right - rectClient.left );

	nHeight = ( rectWindow.bottom - rectWindow.top )
			+ ( lpRect->bottom - lpRect->top )
			- ( rectClient.bottom - rectClient.top );

	/*** Move and/or resize the window's frame rectangle;
	     and tell Windows to repaint the window. ***/

	MoveWindow ( hWnd, nLeft, nTop, nWidth, nHeight, TRUE );
}

/***  GGetWindowFrameRect  *************************************************/

GRectPtr GGetWindowFrameRect ( GWindowPtr hWnd, GRectPtr lpRect )
{
	GetWindowRect ( hWnd, lpRect );

	/*** If the window is an MDI child window, convert its frame
	     rectangle from screen coordinates to workspace coordinates. ***/

	hWnd = GetParent ( hWnd );
	if ( hWnd == GGetMDIClientWindow() )
	{
		ScreenToClient ( hWnd, (LPPOINT) &lpRect->left );
		ScreenToClient ( hWnd, (LPPOINT) &lpRect->right );
	}

	return ( lpRect );
}

/***  GSetWindowFrameRect  *************************************************/

void GSetWindowFrameRect ( GWindowPtr hwnd, GRectPtr lpRect )
{
	MoveWindow ( hwnd, lpRect->left, lpRect->top,
	lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, TRUE );
}

/***  GGetActiveWindow  ****************************************************/

GWindowPtr GGetActiveWindow ( void )
{
	HWND    hWnd = GetActiveWindow();

	/*** If the application's main window is active, and MDI support is
		 enabled, we determine which MDI child window is active by sending
		 the WM_MDIGETACTIVE message to the MDI client window. ***/

	if ( hWnd == GGetMainWindow() )
	{
		if ( gClientWindow )
		{
			hWnd = SendMessage ( gClientWindow, WM_MDIGETACTIVE, 0, 0 );

			/*** Why is this here?  We'll comment it out for now, but if
			     this causes problems, maybe something to come back to...

			if ( IsWindow ( hWnd ) )
				return ( hWnd );
			else
				return ( NULL );  ***/
		}
	}

	return ( hWnd );
}

/***  GSetActiveWindow  ****************************************************/

void GSetActiveWindow ( GWindowPtr hWnd )
{
	HWND hWndMDIClient = GGetMDIClientWindow();

	if ( ! IsWindowVisible ( hWnd ) )
		ShowWindow ( hWnd, SW_SHOWNA );

	if ( hWndMDIClient != NULL && GetParent ( hWnd ) == hWndMDIClient )
	{
		SendMessage ( hWndMDIClient, WM_MDIACTIVATE, (WPARAM) hWnd, 0 );
		SendMessage ( hWndMDIClient, WM_MDIREFRESHMENU, 0, 0 );
	}
	else
	{
		SetActiveWindow ( hWnd );
	}
}

/***  GSetWindowData  *******************************************************/

void GSetWindowData ( GWindowPtr hwnd, long lData )
{
	GWindowDataPtr  lpWindowData = GGetGWindowData ( hwnd );

	if ( lpWindowData != NULL )
		lpWindowData->lData = lData;
}

/***  GGetWindowData   ******************************************************/

long GGetWindowData ( GWindowPtr hwnd )
{
	GWindowDataPtr  lpWindowData = GGetGWindowData ( hwnd );

	if ( lpWindowData != NULL )
		return ( lpWindowData->lData );
	else
		return ( 0 );
}

/***  GShowWindow  **********************************************************/

void GShowWindow ( GWindowPtr hWnd )
{
	HWND hWndMDIClient = GGetMDIClientWindow();

	ShowWindow ( hWnd, SW_SHOWNA );

	if ( hWndMDIClient != NULL && GetParent ( hWnd ) == hWndMDIClient )
		SendMessage ( hWndMDIClient, WM_MDIREFRESHMENU, 0, 0 );
}

/***  GHideWindow  **********************************************************/

void GHideWindow ( GWindowPtr hWnd )
{
	HWND hWndMDIClient = GGetMDIClientWindow();

	ShowWindow ( hWnd, SW_HIDE );

	if ( hWndMDIClient != NULL && GetParent ( hWnd ) == hWndMDIClient )
		SendMessage ( hWndMDIClient, WM_MDIREFRESHMENU, 0, 0 );
}

/***************************  GMaximizeWindow  ******************************/

void GMaximizeWindow ( HWND hWnd )
{
	if ( gClientWindow )
		SendMessage ( gClientWindow, WM_MDIMAXIMIZE, (WPARAM) hWnd, 0 );
	else
		ShowWindow ( hWnd, SW_SHOWMAXIMIZED );
}

/***************************  GMinimizeWindow  ******************************/

void GMinimizeWindow ( GWindowPtr window )
{
	if ( window )
		ShowWindow ( window, SW_SHOWMINIMIZED );
}

/***************************  GRestoreWindow  *******************************/

void GRestoreWindow ( GWindowPtr window )
{
	if ( window )
	{
		if ( gClientWindow )
			SendMessage ( gClientWindow, WM_MDIRESTORE, (WPARAM) window, 0 );
		else
			ShowWindow ( window, SW_RESTORE );
	}
}

/**************************  GIsWindowMaximized  ****************************/

int GIsWindowMaximized ( HWND hWnd )
{
	return ( IsZoomed ( hWnd ) );
}

/**************************  GIsWindowMaximized  ****************************/

int GIsWindowMinimized ( HWND hWnd )
{
	return ( IsIconic ( hWnd ) );
}

/*****************************  GIsMoveableWindow  ***************************/

int GIsMoveableWindow ( HWND hWnd )
{
	return ( GetWindowLong ( hWnd, GWL_STYLE ) & WS_CAPTION ? TRUE : FALSE );
}

/*****************************  GIsCloseableWindow  ***************************/

int GIsCloseableWindow ( HWND hWnd )
{
	return ( GetWindowLong ( hWnd, GWL_STYLE ) & WS_SYSMENU ? TRUE : FALSE );
}

/***************************  GIsSizeableWindow  ***************************/

int GIsSizeableWindow ( HWND hWnd )
{
	return ( GetWindowLong ( hWnd, GWL_STYLE ) & WS_THICKFRAME ? TRUE : FALSE );
}

/***************************  GIsMazimizableWindow  **************************/

int GIsMaximizableWindow ( HWND hWnd )
{
	return ( GetWindowLong ( hWnd, GWL_STYLE ) & WS_MAXIMIZEBOX ? TRUE : FALSE );
}

/***************************  GIsIconizableWindow  ***************************/

int GIsMinimizableWindow ( HWND hWnd )
{
	return ( GetWindowLong ( hWnd, GWL_STYLE ) & WS_MINIMIZEBOX ? TRUE : FALSE );
}

/***  GIsFloatingWindow  *****************************************************/

int GIsFloatingWindow ( HWND hWnd )
{
	return ( GetWindowLong ( hWnd, GWL_STYLE ) & WS_MINICAPTION ? TRUE : FALSE );
}

/*** GIsDocumentWindow *******************************************************/

int GIsDocumentWindow ( GWindowPtr hWnd )
{
	int		nResult;
	HWND	hWndMDIClient = GGetMDIClientWindow();

	if ( hWndMDIClient )
	{
		if ( GetParent ( hWnd ) == hWndMDIClient )
			nResult = TRUE;
	}
	else
	{
		if ( hWnd == GGetMainWindow() )
			nResult = TRUE;
	}

	return ( nResult );
}

/***************************  GIsWindowVisible  ******************************/

int GIsWindowVisible ( GWindowPtr window )
{
	return ( IsWindowVisible ( window ) );
}

/************************  GGetFirstWindow  ******************************/

GWindowPtr GGetFirstWindow ( void )
{
	if ( gGWindowDataList )
		return ( gGWindowDataList->hWnd );
	else
		return ( NULL );
}

/**************************  GCenterWindow  *****************************/

void GCenterWindow ( HWND hDlg )
{
	int		cxScreen, cyScreen, cxDlg, cyDlg, nLeft, nTop;
	HWND	hWnd;
	RECT	rect;

	GetWindowRect ( hDlg, &rect );

	cxDlg = rect.right - rect.left;
	cyDlg = rect.bottom - rect.top;

	cxScreen = GetSystemMetrics ( SM_CXSCREEN );
	cyScreen = GetSystemMetrics ( SM_CYSCREEN );

	nLeft = ( cxScreen - cxDlg ) / 2;
	nTop = ( cyScreen - cyDlg ) / 2;
	hWnd = GetParent ( hDlg );

    /*** NOTE: If the dialog's owning window is hidden when this
         function is called, the results seem to be random.  I haven't
         found any way around this problem. ***/

	MoveWindow ( hDlg, nLeft, nTop, cxDlg, cyDlg, TRUE );

/*  The following is how I used to do this, but it doesn't seem to work
    any better than the one-line solution above.

	if ( hWnd == NULL )
	{
		MoveWindow ( hDlg, nLeft, nTop, cxDlg, cyDlg, TRUE );
	}
	else
	{
		GetClientRect ( hWnd, &rect );
		ClientToScreen ( hWnd, (LPPOINT) &rect.left );

		rect.left = nLeft - rect.left;
		rect.top = nTop - rect.top;

		MoveWindow ( hDlg, rect.left, rect.top, cxDlg, cyDlg, TRUE );
	}
*/
}

/**************************  GGetNextWindow  *********************************/

GWindowPtr GGetNextWindow ( GWindowPtr hWnd )
{
	GWindowDataPtr data = gGWindowDataList;

	while ( data )
	{
		if ( data->hWnd == hWnd && data->pNext )
			return ( data->pNext->hWnd );
		data = data->pNext;
	}

	return ( NULL );
}

/***********************  GGetWindowTitleBarHeight  **************************/

short GGetWindowTitleBarHeight ( void )
{
	return ( GetSystemMetrics ( SM_CYCAPTION ) - GetSystemMetrics ( SM_CYBORDER ) );
}

/***************************  GTileWindows  *********************************/

void GTileWindows ( void )
{
	if ( gClientWindow )
		SendMessage ( gClientWindow, WM_MDITILE, 0, 0 );
}

/***************************  GCascadeWindows  ******************************/

void GCascadeWindows ( void )
{
	if ( gClientWindow )
		SendMessage ( gClientWindow, WM_MDICASCADE, 0, 0 );
}

/***************************  GArrangeWindowIcons  **************************/

void GArrangeWindowIcons ( void )
{
	if ( gClientWindow )
		SendMessage ( gClientWindow, WM_MDIICONARRANGE, 0, 0 );
}

/****************************  GCloseWindow  *******************************/

void GCloseWindow ( GWindowPtr hwnd )
{
	SendMessage ( hwnd, WM_CLOSE, 0, 0 );
}

/***********************  GSetWindowMenu  ***********************************/

void GSetWindowMenu ( GMenuPtr hMenuWindow )
{
	int	nResult;

#ifdef GWIN32
	nResult = SendMessage ( gClientWindow, WM_MDISETMENU, (WPARAM) NULL, (LPARAM) hMenuWindow );
#else
	nResult = SendMessage ( gClientWindow, WM_MDISETMENU, FALSE, MAKELPARAM ( NULL, hMenuWindow ) );
#endif

	DrawMenuBar ( GGetMainWindow() );
}

/************************  GSetWindowScrollRects  ************************/

void GSetWindowScrollRects ( GWindowPtr hwnd, GRectPtr lpVertScrollRect,
GRectPtr lpHoriScrollRect, GRectPtr lpWindowRect )
{
	RECT    rect;
	
	GetClientRect ( hwnd, &rect );
	
	if ( lpVertScrollRect )
		rect.right = rect.right - GetSystemMetrics ( SM_CXVSCROLL ) + 1;

	if ( lpHoriScrollRect )
		rect.bottom = rect.bottom - GetSystemMetrics ( SM_CYHSCROLL ) + 1;

	if ( lpVertScrollRect )
	{
		lpVertScrollRect->left = rect.right;
		lpVertScrollRect->top = rect.top - 1;
		lpVertScrollRect->right = rect.right + GetSystemMetrics ( SM_CXVSCROLL ) - 1;
		lpVertScrollRect->bottom = rect.bottom; // + 1;
	}

	if ( lpHoriScrollRect )
	{
		lpHoriScrollRect->left = rect.left - 1;
		lpHoriScrollRect->top = rect.bottom;
		lpHoriScrollRect->right = rect.right; // + 1;
		lpHoriScrollRect->bottom = rect.bottom + GetSystemMetrics ( SM_CYHSCROLL ) - 1;
	}

	if ( lpWindowRect )
		*lpWindowRect = rect;
}

/**************************  GFindWindow  *******************************/

GWindowPtr GFindWindow ( short x, short y )
{
	POINT   point;

	point.x = x;
	point.y = y;

    return ( WindowFromPoint ( point ) );
}

/***************************  GInvalidateWindow  ************************/

void GInvalidateWindow ( GWindowPtr hwnd, GRectPtr lpRect )
{
	InvalidateRect ( hwnd, lpRect, TRUE );
}

/***************************  GValidateWindow  **************************/

void GValidateWindow ( GWindowPtr hwnd, GRectPtr lpRect )
{
	ValidateRect ( hwnd, lpRect );
}

/**************************  GUpdateWindow  *****************************/

void GUpdateWindow ( GWindowPtr hwnd )
{
	UpdateWindow ( hwnd );
}

/**************************  GEraseOnUpdates  ****************************/

void GEraseOnUpdates ( GWindowPtr hwnd, int fErase )
{
	GWindowDataPtr  windowData;

	if ( ( windowData = GGetGWindowData ( hwnd ) ) != NULL )
	{
		if ( fErase )
			windowData->lFlags |=  G_ERASE_BACKGROUND;
		else
			windowData->lFlags &= ~G_ERASE_BACKGROUND;
	}
}

/************************  GIsErasedOnUpdates  ****************************/

int GIsErasedOnUpdates ( GWindowPtr hwnd )
{
	GWindowDataPtr lpWindowData;

	if ( ( lpWindowData = GGetGWindowData ( hwnd ) ) != NULL )
		if ( lpWindowData->lFlags & G_ERASE_BACKGROUND )
			return ( TRUE );

	return ( FALSE );
}

/****************************  GCreateSizeBox  *****************************/

HWND GCreateSizeBox ( HWND hWnd, BOOL fVisible )
{
	int		nLeft, nTop, nWidth, nHeight;
    RECT	rect;
	DWORD	dwStyle = WS_CHILD | SBS_SIZEBOX;
	HWND	hSizeBox;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

#if ( WINVER >= 0x0400 )
	dwStyle |= SBS_SIZEGRIP;
#endif

	GetClientRect ( hWnd, &rect );

	nWidth = GetSystemMetrics ( SM_CXVSCROLL );
	nHeight = GetSystemMetrics ( SM_CYHSCROLL );
	nLeft = rect.right - nWidth;
	nTop = rect.bottom - nHeight;

	hSizeBox = CreateWindow ( "SCROLLBAR", "", dwStyle, nLeft, nTop, nWidth,
	nHeight, hWnd, (HMENU) ID_SIZEBOX, GGetInstance(), NULL );

    return ( hSizeBox );
}

/***  GMoveSizeBox  ********************************************************/

void GMoveSizeBox ( HWND hWnd )
{
	int		nLeft, nTop, nWidth, nHeight;
    RECT	rect;
	HWND	hSizeBox = GetDlgItem ( hWnd, ID_SIZEBOX );

	if ( hSizeBox )
	{
		GetClientRect ( hWnd, &rect );

		nWidth = GetSystemMetrics ( SM_CXVSCROLL );
		nHeight = GetSystemMetrics ( SM_CYHSCROLL );
		nLeft = rect.right - nWidth;
		nTop = rect.bottom - nHeight;

		MoveWindow ( hSizeBox, nLeft, nTop, nWidth, nHeight, TRUE );
	}
}

/***  GShowSizeBox  ********************************************************/

void GShowSizeBox ( HWND hWnd )
{
	HWND	hSizeBox = GetDlgItem ( hWnd, ID_SIZEBOX );

	if ( hSizeBox )
		ShowScrollBar ( hSizeBox, SB_CTL, TRUE );
}

/***  GHideSizeBox  ********************************************************/

void GHideSizeBox ( HWND hWnd )
{
	HWND	hSizeBox = GetDlgItem ( hWnd, ID_SIZEBOX );

	if ( hSizeBox )
		ShowScrollBar ( hSizeBox, SB_CTL, FALSE );
}

/***  GIsSizeBoxVisible  ***************************************************/

int GIsSizeBoxVisible ( HWND hWnd )
{
	HWND	hSizeBox = GetDlgItem ( hWnd, ID_SIZEBOX );

	if ( hSizeBox )
		return ( IsWindowVisible ( hSizeBox ) );

	return ( FALSE );
}

/*** GScrollWindow *********************************************************/

void GScrollWindow ( GWindowPtr hWnd, short wRight, short wDown, GRectPtr lpScrollRect,
GRectPtr lpClipRect, int fUpdate )
{
    int		nResult;

	/*** Scroll the Device Context of the current window, then (if desired)
	     invalidate the region to be updated. ***/

    if ( fUpdate )
		nResult = ScrollWindowEx ( hWnd, wRight, wDown, lpScrollRect, lpClipRect, NULL, NULL, SW_INVALIDATE );
	else
		nResult = ScrollWindowEx ( hWnd, wRight, wDown, lpScrollRect, lpClipRect, NULL, NULL, 0 );
}

/***  PRIVATE FUNCTIONS  *************************************************/

/***  GCreateGWindowData  ************************************************/

GWindowDataPtr GCreateGWindowData ( short wClass, GEventProcPtr pfnProc, long lData )
{
	GWindowDataPtr  pWindowData = (GWindowDataPtr) GNewPtr ( sizeof ( GWindowData ) );

	if ( pWindowData != NULL )
	{
		pWindowData->pNext = gGWindowDataList;
		pWindowData->hWnd = NULL;
		pWindowData->pfnProc = pfnProc;
		pWindowData->lData = lData;
		pWindowData->lFlags = 0;
		pWindowData->wClass = wClass;
		
		gGWindowDataList = pWindowData;
	}

	return ( pWindowData );
}

/***  GDeleteGWindowData  ************************************************/

void GDeleteGWindowData ( GWindowDataPtr windowDataToFree )
{
	GWindowDataPtr *windowData;
	
	for ( windowData = &gGWindowDataList; *windowData; windowData = &(*windowData)->pNext )
	{
		if ( *windowData == windowDataToFree )
		{
			(*windowData) = windowDataToFree->pNext;
			GFreePtr ( windowDataToFree );
			break;
		}
	}
}

/***  GSetGWindowData  ****************************************************/

void GSetGWindowData ( HWND hWnd, GWindowDataPtr lpWindowData )
{
	lpWindowData->hWnd = hWnd;

	if ( GIsDialogWindow ( hWnd ) )
		SetWindowLong ( hWnd, DWL_USER, (long) lpWindowData );
	else
		SetWindowLong ( hWnd, 0, (long) lpWindowData );
}

/***  GGetGWindowData  ****************************************************/

GWindowDataPtr GGetGWindowData ( HWND hWnd )
{
	if ( GIsDialogWindow ( hWnd ) )
		return ( (GWindowDataPtr) GetWindowLong ( hWnd, DWL_USER ) );
	else
		return ( (GWindowDataPtr) GetWindowLong ( hWnd, 0 ) );
}

/***  GRegisterWindowClass  ***********************************************/

BOOL GRegisterWindowClass ( LPSTR lpszClassName, int cbWndExtra, WNDPROC lpfnWndProc, int nBrush )
{
	HINSTANCE	hInstance = GGetInstance();
	HICON		hIcon = LoadIcon ( hInstance, lpszClassName );
	HCURSOR		hCursor = LoadCursor ( hInstance, lpszClassName );
	WNDCLASS	wndclass;

	/*** If the class has already been registered, return TRUE
	     to indicate this without registering the class again. ***/

	if ( GetClassInfo ( hInstance, lpszClassName, &wndclass ) )
		return ( TRUE );

	/*** Fill the window class record structure.  If we did not find
		 icon and cursor resources with the same names as the window
		 class, use the standard ones instead.  If we are registering
		 a dialog window class, provide enough class extra bytes for
		 a dialog; otherwise, just provide enough extra class storage
		 for a single long integer. ***/

	wndclass.style =  CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
	wndclass.lpfnWndProc = lpfnWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = cbWndExtra;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = hIcon ? hIcon : LoadIcon ( NULL, IDI_APPLICATION );
	wndclass.hCursor = hCursor ? hCursor : LoadCursor ( NULL, IDC_ARROW );
	wndclass.hbrBackground = (HBRUSH) ( nBrush + 1 );
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = lpszClassName;

	/*** Now register the window class; return TRUE if successful
		 or FALSE on failure. ***/

	if ( RegisterClass ( &wndclass ) == 0 )
		return ( FALSE );
	else
    	return ( TRUE );
}

/**************************  sFrameWndProc  *******************************

	This is the message-handling procedure for an MDI frame window.

***************************************************************************/

LRESULT CALLBACK sFrameWndProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	short					wCode, idItem;
	HWND					hwndChild;
	CLIENTCREATESTRUCT      clientcreate;

	switch ( message )
	{
		case WM_NULL:
			GSendEvent ( G_NULL_EVENT, NULL, 0, 0 );
			break;

		/*** When the MDI frame window is created, we need to create an MDI
			 client which manages the MDI child windows.  We need to specify
			 which menu will be the MDI window menu, and the command ID of
			 the MDI menu item for the first MDI child window. ***/

		case WM_CREATE:
			clientcreate.hWindowMenu = NULL; // gWindowMenu;
			clientcreate.idFirstChild = 1;
			gClientWindow = CreateWindow ( "MDICLIENT", NULL,
			WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | MDIS_ALLCHILDSTYLES,
			0, 0, 0, 0, hwnd, (HMENU) 1, GGetInstance(), (LPSTR) &clientcreate );
			break;

		/*** When the main frame window is destroyed, we end the application
		     by quitting the main message loop. ***/

		case WM_DESTROY:
			PostQuitMessage ( 0 );
			break;

		/*** If the user closes the frame window, send a quit event to the
			 application.  If it tells us not to take the default response,
			 return zero to prevent Windows from destroying the window (and
			 hence quitting the program). ***/

		case WM_CLOSE:
			if ( GSendEvent ( G_EXIT_EVENT, NULL, 0, 0 ) == FALSE )
				return ( 0 );
			else
				break;

		/*** If the message is a command, get the handle to the child window
			 or control which sent the command and process it accordingly.
			 If the command was not sent from a child control, it was sent from
			 the frame window's menu.  Find the menu command and do it.  If
			 the command is not in the frame window's menu, it may be in a
			 child window's menu; get the active child window and pass the
		     command on to the child. ***/

		case WM_COMMAND:
#ifdef GWIN32
			idItem = LOWORD ( wParam );
			wCode = HIWORD ( wParam );
			hwndChild = (HWND) lParam;
#else
			idItem = wParam;
			wCode = HIWORD ( lParam );
			hwndChild = (HWND) LOWORD ( lParam );
#endif
			if ( hwndChild == NULL && idItem > 100 )
				GSendEvent ( G_MENU_EVENT, NULL, idItem / 100, idItem % 100 );
			break;
	}

	/*** Pass the message on to the default frame-window-message-handling
		 procedure, and return the result. ***/

	return ( DefFrameProc ( hwnd, gClientWindow, message, wParam, lParam ) );
}

/****************************  sWindowProc  ********************************

	This is the message-handling procedure for an MDI child window.

****************************************************************************/

LRESULT CALLBACK sWindowProc ( HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if ( GDoMessage ( hWnd, nMsg, wParam, lParam ) )
	{
		if ( ( GetParent ( hWnd ) == gClientWindow ) && ( gClientWindow != NULL ) )
		{
			return ( DefMDIChildProc ( hWnd, nMsg, wParam, lParam ) );
		}
		else
		{
			return ( DefWindowProc ( hWnd, nMsg, wParam, lParam ) );
		}
	}

	return ( 0 );
}

