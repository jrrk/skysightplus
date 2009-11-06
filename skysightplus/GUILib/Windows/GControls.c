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

/****************************  GCreatePushButton  ***************************/

HWND GCreatePushButton ( HWND hWnd, short idCtl, char *lpszTitle, LPRECT lpRect, int fVisible )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
	DWORD	dwStyle = WS_CHILD | BS_PUSHBUTTON;
	HWND	hCtl = NULL;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

	hCtl = CreateWindow ( "BUTTON", lpszTitle, dwStyle, nLeft, nTop,
	       nWidth, nHeight, hWnd, (HMENU) idCtl, GGetInstance(), NULL );

	return ( hCtl );
}

/****************************  GCreateCheckBox  ****************************/

HWND GCreateCheckBox ( HWND hWnd, short idCtl, char *lpszTitle, LPRECT lpRect,
int fVisible, short wValue )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
	HWND	hCtl = NULL;
	DWORD	dwStyle = WS_CHILD | BS_CHECKBOX;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;
		
	hCtl = CreateWindow ( "BUTTON", lpszTitle, dwStyle, nLeft, nTop, nWidth,
	       nHeight, hWnd, (HMENU) idCtl, GGetInstance(), NULL );

	if ( hCtl )
		SendMessage ( hCtl, BM_SETCHECK, wValue, 0 );

	return ( hCtl );
}

/****************************  GCreateRadioButton  *************************/

HWND GCreateRadioButton ( HWND hWnd, short idCtl, char *lpszTitle, LPRECT lpRect,
int fVisible, short wValue )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
	HWND	hCtl = NULL;
	DWORD	dwStyle = WS_CHILD | BS_RADIOBUTTON;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

	hCtl = CreateWindow ( "BUTTON", lpszTitle, dwStyle, nLeft, nTop, nWidth,
	       nHeight, hWnd, (HMENU) idCtl, GGetInstance(), NULL );

	if ( hCtl )
		SendMessage ( hCtl, BM_SETCHECK, wValue, 0 );

	return ( hCtl );
}

/***************************  GCreateScrollBar  ******************************/

HWND GCreateScrollBar ( HWND hWnd, short idCtl, char *lpszTitle, LPRECT lpRect,
int fVisible, short wValue, short wMin, short wMax )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
	HWND	hCtl = NULL;
	DWORD	dwStyle = WS_CHILD;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

	if ( nWidth > nHeight )
		dwStyle = dwStyle | SBS_HORZ;
	else
		dwStyle = dwStyle | SBS_VERT;

	hCtl = CreateWindow ( "SCROLLBAR", lpszTitle, dwStyle, nLeft, nTop, nWidth,
	       nHeight, hWnd, (HMENU) idCtl, GGetInstance(), NULL );

	if ( hCtl )
	{
		SetScrollRange ( hCtl, SB_CTL, wMin, wMax, FALSE );
		SetScrollPos ( hCtl, SB_CTL, wValue, TRUE );
	}

	return ( hCtl );
}

/***********************  GCreatePopupMenu  ************************/

HWND GCreatePopupMenu ( HWND hWnd, short idCtl, char *lpszTitle,
LPRECT lpRect, int fVisible, short idMenu, short wTitleWidth )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
	HWND	hCtl = NULL;
	DWORD	dwStyle = WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

	hCtl = CreateWindow ( "COMBOBOX", lpszTitle, dwStyle,
	nLeft, nTop, nWidth, nHeight, hWnd, (HMENU) idCtl, GGetInstance(), NULL );

	return ( hCtl );
}

/************************  GCreateEditText  **************************/

HWND GCreateEditText ( HWND hWnd, short idCtl, char *lpszText, LPRECT lpRect,
int fVisible, int fVertScroll, int fHoriScroll )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
	HWND	hCtl = NULL;
	DWORD	dwStyle = WS_CHILD | ES_MULTILINE | ES_LEFT
	                | ES_AUTOHSCROLL | ES_AUTOVSCROLL;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

	if ( fVertScroll )
		dwStyle |= WS_VSCROLL;

	if ( fHoriScroll )
		dwStyle |= WS_HSCROLL;

    if ( GIsDialogWindow ( hWnd ) )
    	dwStyle |= WS_BORDER;
        
	hCtl = CreateWindow ( "EDIT", lpszText, dwStyle, nLeft, nTop, nWidth, nHeight,
		   hWnd, (HMENU) idCtl, GGetInstance(), NULL );

	return ( hCtl );
}

/*** GCreateProgressBar *************************************************/

GControlPtr GCreateProgressBar ( GWindowPtr hWnd, short idCtl, char *lpszTitle,
LPRECT lpRect, int fVisible, short wMin, short wMax, short wValue )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
    DWORD	dwStyle = WS_CHILD;
	HWND	hCtl = NULL;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

    hCtl = CreateWindow ( PROGRESS_CLASS, lpszTitle, dwStyle, nLeft, nTop, nWidth, nHeight,
           hWnd, (HMENU) idCtl, GGetInstance(), NULL );

    if ( hCtl != NULL )
    {
		SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( wMin, wMax ) );
		SendMessage ( hCtl, PBM_SETPOS, wValue, 0 );
	}
    
    return ( hCtl );
}

/*** GCreateTabGroup *************************************************/

GControlPtr GCreateTabGroup ( GWindowPtr hWnd, short idCtl, LPRECT lpRect,
int fVisible, short wTabStrIDs )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
    DWORD	dwStyle = WS_CHILD;
	HWND	hCtl = NULL;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

    hCtl = CreateWindow ( WC_TABCONTROL, "", dwStyle, nLeft, nTop, nWidth, nHeight,
           hWnd, (HMENU) idCtl, GGetInstance(), NULL );

    return ( hCtl );
}

/*** GCreateGroupBox *************************************************/

// THIS FUNCTION HAS NOT YET BEEN TESTED.

GControlPtr GCreateGroupBox ( GWindowPtr hWnd, short idCtl, LPSTR lpszTitle,
LPRECT lpRect, int fVisible )
{
	int		nLeft = lpRect->left;
    int		nTop = lpRect->top;
	int		nWidth = lpRect->right - lpRect->left;
	int		nHeight = lpRect->bottom - lpRect->top;
    DWORD	dwStyle = WS_CHILD | BS_GROUPBOX;
	HWND	hCtl = NULL;

	if ( fVisible )
		dwStyle |= WS_VISIBLE;

    hCtl = CreateWindow ( "BUTTON", lpszTitle, dwStyle,
           nLeft, nTop, nWidth, nHeight,
           hWnd, (HMENU) idCtl, GGetInstance(), NULL );

    return ( hCtl );
}

/***************************  GGetControl  ******************************/

HWND GGetControl ( HWND hWnd, short idCtl )
{
	return ( GetDlgItem ( hWnd, idCtl ) );
}

/***********************  GGetControlWindow  ****************************/

HWND GGetControlWindow ( HWND hCtl )
{
	return ( GetParent ( hCtl ) );
}

/************************  GGetControlID  *******************************/

short GGetControlID ( HWND hCtl )
{
	return ( GetWindowID ( hCtl ) );
}

/*************************  GGetControlTitle  ***************************/

void GGetControlTitle ( HWND hCtl, LPSTR lpszText )
{
	GetWindowText ( hCtl, lpszText, 255 );
}

/*************************  GSetControlTitle  ***************************/

void GSetControlTitle ( HWND hCtl, LPSTR lpszText )
{
	SetWindowText ( hCtl, lpszText );
}

/*************************  GGetControlRect  ***************************/

void GGetControlRect ( HWND hCtl, LPRECT lpRect )
{
	GetWindowRect ( hCtl, lpRect );
	ScreenToClient ( GetParent ( hCtl ), (LPPOINT) &lpRect->left );
	ScreenToClient ( GetParent ( hCtl ), (LPPOINT) &lpRect->right );
}

/************************  GSetControlRect  ****************************/

void GSetControlRect ( HWND hCtl, LPRECT lpRect )
{
	MoveWindow ( hCtl, lpRect->left, lpRect->top,
	lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, TRUE );
}

/*** GSetKeyEventControl *****************************************************/

void GSetKeyEventControl ( GControlPtr hCtl )
{
	SetFocus ( hCtl );
}

/*** GGetKeyEventControl *****************************************************/

GControlPtr GGetKeyEventControl ( void )
{
    HWND	hCtl;

	hCtl = GetFocus();

    return ( hCtl );
}

/***********************  GGetControlValue  ******************************/

short GGetControlValue ( HWND hCtl )
{
	short	wValue = 0;
	char	szClassName[256];
	DWORD	dwStyle;

	GetClassName ( hCtl, szClassName, 255 );

	if ( lstrcmpi ( szClassName, "BUTTON" ) == 0 )
	{
		dwStyle = GetWindowLong ( hCtl, GWL_STYLE );
		if ( ( dwStyle & BS_PUSHBUTTON ) || ( dwStyle & BS_DEFPUSHBUTTON ) )
			wValue = SendMessage ( hCtl, BM_GETSTATE, 0, 0 ) & 0x0004 ? 1 : 0;
		else
			wValue = SendMessage ( hCtl, BM_GETCHECK, 0, 0 );
	}

	if ( lstrcmpi ( szClassName, "SCROLLBAR" ) == 0 )
		wValue = GetScrollPos ( hCtl, SB_CTL );

	if ( lstrcmpi ( szClassName, "COMBOBOX" ) == 0 )
		wValue = SendMessage ( hCtl, CB_GETCURSEL, 0, 0 ) + 1;

    if ( lstrcmpi ( szClassName, PROGRESS_CLASS ) == 0 )
    	wValue = SendMessage ( hCtl, PBM_DELTAPOS, 0, 0 );
        
    if ( lstrcmpi ( szClassName, WC_TABCONTROL ) == 0 )
    	wValue = SendMessage ( hCtl, TCM_GETCURSEL, 0, 0 ) + 1;

    if ( lstrcmpi ( szClassName, TRACKBAR_CLASS ) == 0 )
    	wValue = SendMessage ( hCtl, TBM_GETPOS, 0, 0 );

	return ( wValue );
}

/************************  GGetControlMin  ****************************/

short GGetControlMin ( HWND hCtl )
{
	int		nMin = 0, nMax;
    long	nRange;
	char	szClassName[256];

	GetClassName ( hCtl, szClassName, 255 );

	if ( lstrcmpi ( szClassName, "BUTTON" ) == 0 )
		nMin = 0;

	if ( lstrcmpi ( szClassName, "SCROLLBAR" ) == 0 )
		GetScrollRange ( hCtl, SB_CTL, &nMin, &nMax );

	if ( lstrcmpi ( szClassName, "COMBOBOX" ) == 0 )
	{
		if ( SendMessage ( hCtl, CB_GETCOUNT, 0, 0 ) > 0 )
			nMin = 1;
		else
			nMin = 0;
	}

    if ( lstrcmpi ( szClassName, PROGRESS_CLASS ) == 0 )
    {
		nRange = SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( 0, 100 ) );
        nMin = LOWORD ( nRange );
        nMax = HIWORD ( nRange );
        nRange = SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( nMin, nMax ) );
    }

    if ( lstrcmpi ( szClassName, WC_TABCONTROL ) == 0 )
	{
		if ( SendMessage ( hCtl, TCM_GETITEMCOUNT, 0, 0 ) > 0 )
			nMin = 1;
		else
			nMin = 0;
	}

    if ( lstrcmpi ( szClassName, TRACKBAR_CLASS ) == 0 )
    	nMin = SendMessage ( hCtl, TBM_GETRANGEMIN, 0, 0 );
    
	return ( nMin );
}

/************************  GGetControlMax  ****************************/

short GGetControlMax ( HWND hCtl )
{
	int		nMin, nMax = 0;
    long	nRange;
	char	szClassName[256];

	GetClassName ( hCtl, szClassName, 255 );
	
	if ( lstrcmpi ( szClassName, "BUTTON" ) == 0 )
		nMax = 1;

	if ( strcmp ( szClassName, "SCROLLBAR" ) == 0 )
		GetScrollRange ( hCtl, SB_CTL, &nMin, &nMax );

	if ( strcmp ( szClassName, "COMBOBOX" ) == 0 )
		nMax = SendMessage ( hCtl, CB_GETCOUNT, 0, 0 );

    if ( lstrcmpi ( szClassName, PROGRESS_CLASS ) == 0 )
    {
		nRange = SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( 0, 100 ) );
        nMin = LOWORD ( nRange );
        nMax = HIWORD ( nRange );
        nRange = SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( nMin, nMax ) );
    }

    if ( lstrcmpi ( szClassName, WC_TABCONTROL ) == 0 )
    	nMax = SendMessage ( hCtl, TCM_GETITEMCOUNT, 0, 0 );

    if ( lstrcmpi ( szClassName, TRACKBAR_CLASS ) == 0 )
    	nMax = SendMessage ( hCtl, TBM_GETRANGEMAX, 0, 0 );
        
	return ( nMax );
}

/************************  GSetControlValue  *****************************/

void GSetControlValue ( HWND hCtl, short wValue )
{
	char	szClassName[256];
	DWORD	dwStyle;

	GetClassName ( hCtl, szClassName, 255 );

	if ( lstrcmpi ( szClassName, "BUTTON" ) == 0 )
	{
		dwStyle = GetWindowLong ( hCtl, GWL_STYLE );
		if ( ( dwStyle & BS_PUSHBUTTON ) || ( dwStyle & BS_DEFPUSHBUTTON ) )
			SendMessage ( hCtl, BM_SETSTATE, wValue, 0 );
		else
			SendMessage ( hCtl, BM_SETCHECK, wValue, 0 );
   }

	if ( lstrcmpi ( szClassName, "SCROLLBAR" ) == 0 )
		SetScrollPos ( hCtl, SB_CTL, wValue, TRUE );

	if ( lstrcmpi ( szClassName, "COMBOBOX" ) == 0 )
		SendMessage ( hCtl, CB_SETCURSEL, wValue - 1, 0 );

    if ( lstrcmpi ( szClassName, PROGRESS_CLASS ) == 0 )
		SendMessage ( hCtl, PBM_SETPOS, wValue, 0 );

    if ( lstrcmpi ( szClassName, WC_TABCONTROL ) == 0 )
    	SendMessage ( hCtl, TCM_SETCURSEL, wValue - 1, 0 );

    if ( lstrcmpi ( szClassName, TRACKBAR_CLASS ) == 0 )
    	SendMessage ( hCtl, TBM_SETPOS, TRUE, wValue );
}

/************************  GSetControlMin  ****************************/

void GSetControlMin ( HWND hCtl, short wMin )
{
	int		nMin, nMax;
    long	nRange;
	char	szClassName[256];

	GetClassName ( hCtl, szClassName, 255 );
	
	if ( lstrcmpi ( szClassName, "SCROLLBAR" ) == 0 )
	{
		GetScrollRange ( hCtl, SB_CTL, &nMin, &nMax );
		SetScrollRange ( hCtl, SB_CTL, wMin, nMax, TRUE );
	}

    if ( lstrcmpi ( szClassName, PROGRESS_CLASS ) == 0 )
    {
		nRange = SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( 0, 100 ) );
        nMin = LOWORD ( nRange );
        nMax = HIWORD ( nRange );
        nRange = SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( wMin, nMax ) );
    }

    if ( lstrcmpi ( szClassName, TRACKBAR_CLASS ) == 0 )
    {
		SendMessage ( hCtl, TBM_SETRANGEMIN, TRUE, wMin );
    }
}

/************************  GSetControlMax  ****************************/

void GSetControlMax ( HWND hCtl, short wMax )
{
	int		nMin, nMax;
    long	nRange;
	char	szClassName[256];

	GetClassName ( hCtl, szClassName, 255 );
	
	if ( lstrcmpi ( szClassName, "SCROLLBAR" ) == 0 )
	{
		GetScrollRange ( hCtl, SB_CTL, &nMin, &nMax );
		SetScrollRange ( hCtl, SB_CTL, nMin, wMax, TRUE );
	}

    if ( lstrcmpi ( szClassName, PROGRESS_CLASS ) == 0 )
    {
		nRange = SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( 0, 100 ) );
        nMin = LOWORD ( nRange );
        nMax = HIWORD ( nRange );
        nRange = SendMessage ( hCtl, PBM_SETRANGE, 0, MAKELPARAM ( nMin, wMax ) );
    }

    if ( lstrcmpi ( szClassName, TRACKBAR_CLASS ) == 0 )
    {
    	SendMessage ( hCtl, TBM_SETRANGEMAX, TRUE, wMax );
    }
}

/******************************  GShowControl  ****************************/

void GShowControl ( HWND hCtl )
{
	ShowWindow ( hCtl, SW_SHOWNA );
}

/******************************  GHideControl  ****************************/

void GHideControl ( HWND hCtl )
{
	ShowWindow ( hCtl, SW_HIDE );
}

/****************************  GIsControlVisible  *************************/

int GIsControlVisible ( HWND hCtl )
{
	return ( IsWindowVisible ( hCtl ) );
}

/***********************  GToggleControl  *****************************/

void GToggleControl ( GControlPtr pCtrl )
{
	SendMessage ( pCtrl, BM_SETCHECK,
	! SendMessage ( pCtrl, BM_GETCHECK, 0, 0 ), 0 );
}

/**********************  GEnableControl  **********************************/

void GEnableControl ( GControlPtr pCtrl, int fEnable )
{
	EnableWindow ( pCtrl, fEnable );
}

/****************************  GIsControlEnabled  *************************/

int GIsControlEnabled ( HWND hCtl )
{
	return ( IsWindowEnabled ( hCtl ) );
}

/***************************  GDrawControl  *******************************/

void GDrawControl ( HWND hCtl )
{
	if ( hCtl )
		RedrawWindow ( hCtl, NULL, NULL,
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_VALIDATE );
}

/*** GGetScrollBarWidth  ******************************************************/

short GGetScrollBarWidth ( void )
{
	return ( GetSystemMetrics ( SM_CXVSCROLL ) );
}

/*** GGetScrollBarHeight  ******************************************************/

short GGetScrollBarHeight ( void )
{
	return ( GetSystemMetrics ( SM_CYHSCROLL ) );
}

/***************************  GInsertPopupMenuItem  *************************/

void GInsertPopupMenuItem ( GControlPtr pControl, short wItem, char *lpszText )
{
	SendMessage ( pControl, CB_INSERTSTRING, wItem - 1, (LPARAM) lpszText );
}

/***************************  GDeletePopupMenuItem  *************************/

void GDeletePopupMenuItem ( GControlPtr pControl, short wItem )
{
	SendMessage ( pControl, CB_DELETESTRING, wItem - 1, 0 );
}

/***************************  GGetPopupMenuItemText  ************************/

char *GGetPopupMenuItemText ( GControlPtr pControl, short wItem, char *lpszText )
{
	SendMessage ( pControl, CB_GETLBTEXT, wItem - 1, (LPARAM) lpszText );
	return ( lpszText );
}

/***************************  GSetPopupMenuItemText  ************************/

void GSetPopupMenuItemText ( GControlPtr pControl, short wItem, char *lpszText )
{
	SendMessage ( pControl, CB_DELETESTRING, wItem - 1, 0 );
	SendMessage ( pControl, CB_INSERTSTRING, wItem - 1, (LPARAM) lpszText );
}

/************************  GInsertPopupMenuStrings  ************************/

void GInsertPopupMenuStrings ( GControlPtr pControl, short wFirstItem,
short wFirstStringID, short wLastStringID )
{
	short	wStringID, wItem = wFirstItem;
	char	szString[256];

	for ( wStringID = wFirstStringID; wStringID <= wLastStringID; wStringID++ )
	{
		LoadString ( GGetInstance(), wStringID, szString, 255 );
		SendMessage ( pControl, CB_INSERTSTRING, wItem - 1, (LPARAM) szString );
		wItem++;
	}
}

/*** GInsertTabGroupItem *****************************************************/

void GInsertTabGroupItem ( GControlPtr hCtl, short wItem, char *lpszText )
{
    TC_ITEM		tab;

    tab.mask = TCIF_TEXT;
    tab.pszText = lpszText;
    tab.cchTextMax = 255;

	SendMessage ( hCtl, TCM_INSERTITEM, wItem - 1, (LPARAM) &tab );
}

/*** GDeleteTabGroupItem *****************************************************/

void GDeleteTabGroupItem ( GControlPtr hCtl, short wItem )
{
	SendMessage ( hCtl, TCM_DELETEITEM, wItem - 1, 0 );
}

/*** GGetTabGroupItemText ****************************************************/

void GGetTabGroupItemText ( GControlPtr hCtl, short wItem, char *lpszText )
{
    TC_ITEM		tab;

    tab.mask = TCIF_TEXT;
    tab.pszText = lpszText;
    tab.cchTextMax = 255;

	SendMessage ( hCtl, TCM_GETITEM, wItem - 1, (LPARAM) &tab );
}

/*** GSetTabGroupItemText ****************************************************/

void GSetTabGroupItemText ( GControlPtr hCtl, short wItem, char *lpszText )
{
    TC_ITEM		tab;

    tab.mask = TCIF_TEXT;
    tab.pszText = lpszText;

	SendMessage ( hCtl, TCM_SETITEM, wItem - 1, (LPARAM) &tab );
}

/*** GGetTabGroupContentRect **************************************************/

void GGetTabGroupContentRect ( GControlPtr hCtl, GRectPtr lpRect )
{
	HWND hWnd = GetParent ( hCtl );

	GetWindowRect ( hCtl, lpRect );
	ScreenToClient ( hWnd, (LPPOINT) &lpRect->left );
	ScreenToClient ( hWnd, (LPPOINT) &lpRect->right );
	SendMessage ( hCtl, TCM_ADJUSTRECT, FALSE, (LPARAM) lpRect );
}

/**************************  GSetEditTextFont  ******************************/

void GSetEditTextFont ( HWND hCtl, char *lpszFont, short wSize, short wStyle )
{
	HFONT	hFont;
	LOGFONT	lf;

	GSetLogFont ( &lf, lpszFont, wSize, wStyle );
	if ( ( hFont = CreateFontIndirect ( &lf ) ) != NULL )
	{
		SendMessage ( hCtl, WM_SETFONT, (WPARAM) hFont, MAKELPARAM ( TRUE, 0 ) );
	}
}

/**************************  GGetEditTextFont  ******************************/

void GGetEditTextFont ( HWND hCtl, char *lpszFont, short *lpwSize, short *lpwStyle )
{
	HFONT	hFont;
	LOGFONT	lf;

	if ( ( hFont = (HFONT) SendMessage ( hCtl, WM_GETFONT, 0, 0 ) ) != NULL )
		GetObject ( hFont, sizeof ( LOGFONT ), &lf );
	else
		GetObject ( GetStockObject ( SYSTEM_FONT ), sizeof ( LOGFONT ), &lf );

	GGetLogFont ( &lf, lpszFont, lpwSize, lpwStyle );
}

/*************************  GSetEditTextText  ******************************/

void GSetEditTextText ( HWND hCtl, char *lpszText )
{
	SetWindowText ( hCtl, lpszText );
}

/*************************  GSetEditTextText  ******************************/

void GGetEditTextText ( HWND hCtl, char *lpszText )
{
	GetWindowText ( hCtl, lpszText, 32767 );
}

/***********************  GGetControlTextLength  ***************************/

long GGetEditTextLength ( HWND hCtl )
{
	return ( GetWindowTextLength ( hCtl ) );
}

/*** GGetEditTextNumLines *************************************************/

long GGetEditTextNumLines ( GControlPtr hCtl )
{
    long lNumLines;

	lNumLines = SendMessage ( hCtl, EM_GETLINECOUNT, 0, 0 );

    if ( GGetEditTextLineLength ( hCtl, lNumLines - 1 ) == 0 )
        lNumLines = lNumLines - 1;
        
    return ( lNumLines );
}

/*** GGetEditTextLineLength ***********************************************/

long GGetEditTextLineLength ( GControlPtr hCtl, long lLine )
{
    WORD	wIndex;
    
    wIndex = SendMessage ( hCtl, EM_LINEINDEX, lLine, 0 );
    if ( wIndex < 0 )
    	return ( -1 );

	return ( SendMessage ( hCtl, EM_LINELENGTH, wIndex, 0 ) );
}

/*** GGetEditTextLine ***********************************************/

long GGetEditTextLine ( GControlPtr hCtl, long lLineNum, char *pText, long lMaxLength )
{
    WORD	wLength;
	LPWORD	lpMaxLength = (LPWORD) pText;

    *lpMaxLength = lMaxLength;

    wLength = SendMessage ( hCtl, EM_GETLINE, lLineNum, (LPARAM) pText );
    if ( wLength == 0 )
    	return ( -1 );

    pText[ wLength ] = '\0';

	return ( wLength );
}

/*** GGetEditTextWrap ********************************************************/

int GGetEditTextWrap ( GControlPtr hCtl )
{
	return ( FALSE );
}

/*** GSetEditTextWrap ********************************************************/

void GSetEditTextWrap ( GControlPtr hCtl, int bWrap )
{
	return;
}

/**************************  GGetEditTextSelection  ****************************/

void GGetEditTextSelection ( HWND hCtl, long *lpStart, long *lpEnd )
{
	long lSelection;

#ifdef GWIN32

	lSelection = SendMessage ( hCtl, EM_GETSEL, (WPARAM) lpStart, (LPARAM) lpEnd );
    *lpEnd = *lpEnd - 1;

#else

	lSelection = SendMessage ( hCtl, EM_GETSEL, 0, 0 );
	*lpStart = LOWORD ( lSelection );
	*lpEnd = HIWORD ( lSelection ) - 1;

#endif
}

/**************************  GSetEditTextSelection  **************************/

void GSetEditTextSelection ( HWND hCtl, long lStart, long lEnd )
{
#ifdef GWIN32
	SendMessage ( hCtl, EM_SETSEL, lStart, lEnd );
#else
	SendMessage ( hCtl, EM_SETSEL, 0, MAKELPARAM ( lStart, lEnd ) );
#endif
}

/*************************  GCutEditTextSelection  ****************************/

void GCutEditTextSelection ( HWND hCtl )
{
	SendMessage ( hCtl, WM_CUT, 0, 0 );
}

/************************  GCopyEditTextSelection  ***************************/

void GCopyEditTextSelection ( HWND hCtl )
{
	SendMessage ( hCtl, WM_COPY, 0, 0 );
}

/***********************  GPasteEditTextSelection  ***************************/

void GPasteEditTextSelection ( HWND hCtl )
{
	SendMessage ( hCtl, WM_PASTE, 0, 0 );
}

/***********************  GDeleteEditTextSelection  **************************/

void GDeleteEditTextSelection ( HWND hCtl )
{
	SendMessage ( hCtl, WM_CLEAR, 0, 0 );
}

void GSetListBoxItemText ( HWND hCtl, short wItem, char *lpszText )
{

}

void GGetListBoxItemText ( HWND hCtl, short wItem, char *lpszText )
{

}

/*** GDrawButton *************************************************************/

void GDrawButton ( GWindowPtr hWnd, GRectPtr lpRect, char *lpszText, GIconPtr hIcon, int bPressed )
{
    HDC		hDC;
    HFONT	hFont, hOldFont;
    HBRUSH	hBrush, hOldBrush;
    HPEN	hPen, hOldPen;
    DWORD	dwColor;
    SIZE	size;
    int		cbText;
    int		nOldBkMode;
    
    hDC = GetDC ( hWnd );
    if ( hDC == NULL )
    	return;

    if ( lpszText != NULL )
    {
	    nOldBkMode = GetBkMode ( hDC );
		hOldFont = SelectObject ( hDC, GetStockObject ( DEFAULT_GUI_FONT ) );
        cbText = lstrlen ( lpszText );
    	GetTextExtentPoint ( hDC, lpszText, cbText, &size );
    }

    if ( bPressed )
    {
        dwColor = GetSysColor ( COLOR_BTNFACE );
        hBrush = CreateSolidBrush ( dwColor );
        hOldBrush = SelectObject ( hDC, hBrush );
        hOldPen = SelectObject ( hDC, GetStockObject ( NULL_PEN ) );
        Rectangle ( hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom );
        SelectObject ( hDC, hOldBrush );
        DeleteObject ( hBrush );

        dwColor = 0x00000000;
        hPen = CreatePen ( PS_SOLID, 1, dwColor );
        SelectObject ( hDC, hPen );
        MoveToEx ( hDC, lpRect->left, lpRect->bottom - 1, NULL );
        LineTo ( hDC, lpRect->left, lpRect->top );
        LineTo ( hDC, lpRect->right - 1, lpRect->top );
        SelectObject ( hDC, hOldPen );
        DeleteObject ( hPen );

        dwColor = GetSysColor ( COLOR_BTNSHADOW );
        hPen = CreatePen ( PS_SOLID, 1, dwColor );
        SelectObject ( hDC, hPen );
        MoveToEx ( hDC, lpRect->left + 1, lpRect->bottom - 2, NULL );
        LineTo ( hDC, lpRect->left + 1, lpRect->top + 1 );
        LineTo ( hDC, lpRect->right - 2, lpRect->top + 1 );
        SelectObject ( hDC, hOldPen );
        DeleteObject ( hPen );

        dwColor = GetSysColor ( COLOR_BTNHILIGHT );
        hPen = CreatePen ( PS_SOLID, 1, dwColor );
        SelectObject ( hDC, hPen );
        MoveToEx ( hDC, lpRect->left + 1, lpRect->bottom - 2, NULL );
        LineTo ( hDC, lpRect->right - 2, lpRect->bottom - 2 );
        LineTo ( hDC, lpRect->right - 2, lpRect->top + 1 );
        SelectObject ( hDC, hOldPen );
        DeleteObject ( hPen );

        dwColor = 0x00FFFFFFFF;
        hPen = CreatePen ( PS_SOLID, 1, dwColor );
        SelectObject ( hDC, hPen );
        MoveToEx ( hDC, lpRect->left, lpRect->bottom - 1, NULL );
        LineTo ( hDC, lpRect->right - 1, lpRect->bottom - 1 );
        LineTo ( hDC, lpRect->right - 1, lpRect->top );
        SelectObject ( hDC, hOldPen );
        DeleteObject ( hPen );

        if ( lpszText )
        {
            SetBkMode ( hDC, TRANSPARENT );
			TextOut ( hDC,
            lpRect->left + ( lpRect->right - lpRect->left - size.cx ) / 2,
            lpRect->top + ( lpRect->bottom - lpRect->top - size.cy ) / 2,
            lpszText, cbText );
        }
    }
    else
    {
        dwColor = GetSysColor ( COLOR_BTNFACE );
        hBrush = CreateSolidBrush ( dwColor );
        hOldBrush = SelectObject ( hDC, hBrush );
        hOldPen = SelectObject ( hDC, GetStockObject ( NULL_PEN ) );
        Rectangle ( hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom );
        SelectObject ( hDC, hOldBrush );
        DeleteObject ( hBrush );

        hPen = CreatePen ( PS_SOLID, 1, 0x00FFFFFF );
        SelectObject ( hDC, hPen );
        MoveToEx ( hDC, lpRect->left, lpRect->bottom - 1, NULL );
        LineTo ( hDC, lpRect->left, lpRect->top );
        LineTo ( hDC, lpRect->right - 1, lpRect->top );
        SelectObject ( hDC, hOldPen );
        DeleteObject ( hPen );

        dwColor = GetSysColor ( COLOR_BTNHILIGHT );
        hPen = CreatePen ( PS_SOLID, 1, dwColor );
        SelectObject ( hDC, hPen );
        MoveToEx ( hDC, lpRect->left + 1, lpRect->bottom - 2, NULL );
        LineTo ( hDC, lpRect->left + 1, lpRect->top + 1 );
        LineTo ( hDC, lpRect->right - 2, lpRect->top + 1 );
        SelectObject ( hDC, hOldPen );
        DeleteObject ( hPen );

        dwColor = GetSysColor ( COLOR_BTNSHADOW );
        hPen = CreatePen ( PS_SOLID, 1, dwColor );
        SelectObject ( hDC, hPen );
        MoveToEx ( hDC, lpRect->left + 1, lpRect->bottom - 2, NULL );
        LineTo ( hDC, lpRect->right - 2, lpRect->bottom - 2 );
        LineTo ( hDC, lpRect->right - 2, lpRect->top + 1 );
        SelectObject ( hDC, hOldPen );
        DeleteObject ( hPen );

        dwColor = 0x00000000;
        hPen = CreatePen ( PS_SOLID, 1, dwColor );
        SelectObject ( hDC, hPen );
        MoveToEx ( hDC, lpRect->left, lpRect->bottom - 1, NULL );
        LineTo ( hDC, lpRect->right - 1, lpRect->bottom - 1 );
        LineTo ( hDC, lpRect->right - 1, lpRect->top );
        SelectObject ( hDC, hOldPen );
        DeleteObject ( hPen );

		if ( lpszText )
        {
			SetBkMode ( hDC, TRANSPARENT );
        	TextOut ( hDC,
            lpRect->left + ( lpRect->right - lpRect->left - size.cx ) / 2,
            lpRect->top + ( lpRect->bottom - lpRect->top - size.cy ) / 2,
            lpszText, cbText );
        }

    }

    if ( lpszText )
    {
    	SetBkMode ( hDC, nOldBkMode );
        SelectObject ( hDC, hOldFont );
    }
    
    ReleaseDC ( hWnd, hDC );
}

