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

static HDC		sDC = NULL;			/* Current device context handle */
static HDC		sDCAux = NULL;		/* Auxiliary device context handle */

static HPEN		sPen = NULL;		/* Current pen handle */
static HBRUSH	sBrush = NULL;		/* Current brush handle */
static HFONT	sFont = NULL;		/* Current font handle */

static LOGPEN	sLogPen;			/* Current pen attributes */
static LOGBRUSH	sLogBrush;			/* Current brush attributes */
static LOGFONT	sLogFont;			/* Current font attributes */

static HPEN		sOldPen = NULL;		/* Saved pen attributes */
static HBRUSH	sOldBrush = NULL;	/* Saved brush attributes */
static HFONT	sOldFont = NULL;	/* Saved font attributes */
static int		sOldROP2;			/* Saved pattern background mode */
static int		sOldBkMode;			/* Saved pen transfer mode */

void sSelectPenAndBrush ( void );
void sSelectFont ( void );

/***************************  GSetGraphicPort  ***************************/

void GSetGraphicPort ( HDC hDC )
{
	if ( hDC )
	{
		sOldPen = SelectObject ( hDC, GetStockObject ( NULL_PEN ) );
		sOldBrush = SelectObject ( hDC, GetStockObject ( NULL_BRUSH ) );
		sOldFont = SelectObject ( hDC, GetStockObject ( SYSTEM_FONT ) );
		sOldROP2 = GetROP2 ( hDC );
		sOldBkMode = GetBkMode ( hDC );

		GetObject ( sOldPen, sizeof ( LOGPEN ), &sLogPen );
		GetObject ( sOldBrush, sizeof ( LOGBRUSH ), &sLogBrush );
		GetObject ( sOldFont, sizeof ( LOGFONT ), &sLogFont );

		if ( sLogPen.lopnWidth.x == 0 )
			sLogPen.lopnWidth.x = 1;

		sPen = CreatePenIndirect ( &sLogPen );
		sBrush = CreateBrushIndirect ( &sLogBrush );
		sFont = CreateFontIndirect ( &sLogFont );

		SelectObject ( hDC, sPen );
		SelectObject ( hDC, sBrush );
		SelectObject ( hDC, sFont );
		SetBkMode ( hDC, TRANSPARENT );
	}
	else
	{
		SelectObject ( sDC, sOldPen );
		SelectObject ( sDC, sOldBrush );
		SelectObject ( sDC, sOldFont );
		SetROP2 ( sDC, sOldROP2 );
		SetBkMode ( sDC, sOldBkMode );

		DeleteObject ( sPen );
		DeleteObject ( sBrush );
		DeleteObject ( sFont );

		sPen = NULL;
		sBrush = NULL;
		sFont = NULL;
	}

	sDC = hDC;
}

/***************************  GGetGraphicPort  ***************************/

HDC GGetGraphicPort ( void )
{
	return ( sDC );
}

/*****************************  GStartDrawing  ****************************/

void GStartDrawing ( HWND hWnd )
{
	HDC	hDC;

	if ( ( hDC = GetDC ( hWnd ) ) != NULL )
	{
		GSetGraphicPort ( hDC );
	}
}

/*******************************  GEndDrawing  ****************************/

void GEndDrawing ( HWND hWnd )
{
	HDC	hDC;

	if ( ( hDC = GGetGraphicPort() ) != NULL )
	{
		GSetGraphicPort ( NULL );
		ReleaseDC ( hWnd, hDC );
	}
}

/****************************  GStartDrawingBitmap  ************************/

void GStartDrawingBitmap ( HWND hWnd, HBITMAP hBmp )
{
	HDC	hDCWnd, hDCBmp;

	if ( ( hDCWnd = GetDC ( hWnd ) ) != NULL )
	{
		if ( ( hDCBmp = CreateCompatibleDC ( hDCWnd ) ) != NULL )
		{
			SelectObject ( hDCBmp, hBmp );
			GSetGraphicPort ( hDCBmp );
		}

		ReleaseDC ( hWnd, hDCWnd );
	}
}

/*************************  GEndDrawingBitmap  ******************************/

void GEndDrawingBitmap ( HWND hWnd, HBITMAP hBmp )
{
	HDC	hDC;

	if ( ( hDC = GGetGraphicPort() ) != NULL )
	{
		GSetGraphicPort ( NULL );
		DeleteDC ( hDC );
	}
}

/****************************  GStartDrawingPicture  *************************/

LPMETAFILEPICT GStartDrawingPicture ( HWND hWnd, LPRECT lpRect )
{
	LPMETAFILEPICT	lpPict;

	if ( ( lpPict = GCreatePicture ( lpRect ) ) != NULL )
	{
		sDCAux = GetDC ( hWnd );

		sOldPen = SelectObject ( sDCAux, GetStockObject ( NULL_PEN ) );
		sOldBrush = SelectObject ( sDCAux, GetStockObject ( NULL_BRUSH ) );
		sOldFont = SelectObject ( sDCAux, GetStockObject ( SYSTEM_FONT ) );
		sOldROP2 = GetROP2 ( sDCAux );

		GetObject ( sOldPen, sizeof ( LOGPEN ), &sLogPen );
		GetObject ( sOldBrush, sizeof ( LOGBRUSH ), &sLogBrush );
		GetObject ( sOldFont, sizeof ( LOGFONT ), &sLogFont );

		if ( sLogPen.lopnWidth.x == 0 )
			sLogPen.lopnWidth.x = 1;

		sPen = CreatePenIndirect ( &sLogPen );
		sBrush = CreateBrushIndirect ( &sLogBrush );
		sFont = CreateFontIndirect ( &sLogFont );

		SelectObject ( sDCAux, sPen );
		SelectObject ( sDCAux, sBrush );
		SelectObject ( sDCAux, sFont );

		sDC = CreateMetaFile ( NULL );

		SelectObject ( sDC, sPen );
		SelectObject ( sDC, sBrush );
		SelectObject ( sDC, sFont );
		SetROP2 ( sDC, sOldROP2 );
		SetBkMode ( sDC, TRANSPARENT );

		SetMapMode ( sDC, MM_ANISOTROPIC );
		SetWindowExtEx ( sDC, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, NULL );
		SetWindowOrgEx ( sDC, lpRect->left, lpRect->top, NULL );
	}

	return ( lpPict );
}

/***************************  GEndDrawingPicture  ***************************/

void GEndDrawingPicture ( HWND hWnd, LPMETAFILEPICT lpPict )
{
	SelectObject ( sDCAux, sOldPen );
	SelectObject ( sDCAux, sOldBrush );
	SelectObject ( sDCAux, sOldFont );

	DeleteObject ( sPen );
	DeleteObject ( sBrush );
	DeleteObject ( sFont );

	sPen = NULL;
	sBrush = NULL;
	sFont = NULL;

	lpPict->hMF = CloseMetaFile ( sDC );
	ReleaseDC ( hWnd, sDCAux );

	sDCAux = NULL;
	sDC = NULL;
}

/*************************  GSetColor  ********************************/

COLORREF *GSetColor ( COLORREF *lpColor, unsigned char bRed, unsigned char bGreen,
unsigned char bBlue )
{
	*lpColor = RGB ( bRed, bGreen, bBlue );
    return ( lpColor );
}

/*************************  GGetRed  **********************************/

unsigned char GGetRed ( COLORREF *lpColor )
{
    return ( GetRValue ( *lpColor ) );
}

/*************************  GGetGreen  ********************************/

unsigned char GGetGreen ( COLORREF *lpColor )
{
	return ( GetGValue ( *lpColor ) );
}

/*************************  GGetBlue  *********************************/

unsigned char GGetBlue ( COLORREF *lpColor )
{
    return ( GetBValue ( *lpColor ) );
}

/*************************  GRedColor  ********************************/

COLORREF *GRedColor ( void )
{
	static COLORREF color = RGB ( 255, 0, 0 );

    return ( &color );
}

/*************************  GGreenColor  ********************************/

COLORREF *GGreenColor ( void )
{
	static COLORREF color = RGB ( 0, 255, 0 );

    return ( &color );
}

/*************************  GBlueColor  ********************************/

COLORREF *GBlueColor ( void )
{
	static COLORREF color = RGB ( 0, 0, 255 );

    return ( &color );
}

/*************************  GCyanColor  ********************************/

COLORREF *GCyanColor ( void )
{
	static COLORREF color = RGB ( 0, 255, 255 );

    return ( &color );
}

/*************************  GYellowColor  ********************************/

COLORREF *GYellowColor ( void )
{
	static COLORREF color = RGB ( 255, 255, 0 );

    return ( &color );
}

/*************************  GMagentaColor  ********************************/

COLORREF *GMagentaColor ( void )
{
	static COLORREF color = RGB ( 255, 0, 255 );

    return ( &color );
}

/*************************  GBlackColor  ********************************/

COLORREF *GBlackColor ( void )
{
	static COLORREF color = RGB ( 0, 0, 0 );

    return ( &color );
}

/*************************  GWhiteColor  ********************************/

COLORREF *GWhiteColor ( void )
{
	static COLORREF color = RGB ( 255, 255, 255 );

    return ( &color );
}

/********************************  GSetPen  **********************************/

void GSetPen ( COLORREF *lpColor, short wSize, short fFill )
{
	sLogPen.lopnStyle = wSize ? PS_SOLID : PS_NULL;
	sLogPen.lopnWidth.x = wSize;
	sLogPen.lopnColor = *lpColor;

	sLogBrush.lbStyle = fFill ? BS_SOLID : BS_NULL;
	sLogBrush.lbColor = *lpColor;

	SetTextColor ( sDC, *lpColor );

	sSelectPenAndBrush();
}

/*****************************  GGetPen  *********************************/

void GGetPen ( COLORREF *lpColor, short *lpwSize, short *lpfFill )
{
	if ( lpColor )
		*lpColor = sLogPen.lopnColor;

	if ( lpwSize )
		*lpwSize = sLogPen.lopnWidth.x;

	if ( lpfFill )
		*lpfFill = sLogBrush.lbStyle == BS_SOLID ? TRUE : FALSE;
}

/*************************  GSetPenColor  *********************************/

void GSetPenColor ( COLORREF *lpColor )
{
	SetTextColor ( sDC, *lpColor );

	sLogPen.lopnColor = *lpColor;
	sLogBrush.lbColor = *lpColor;

	sSelectPenAndBrush();
}

/****************************  GSetPenSize  ********************************/

void GSetPenSize ( short wSize )
{
	sLogPen.lopnStyle = wSize ? PS_SOLID : PS_NULL;
	sLogPen.lopnWidth.x = wSize;

	sSelectPenAndBrush();
}

/*******************************  GSetPenFill  ******************************/

void GSetPenFill ( short fFill )
{
	sLogBrush.lbStyle = fFill ? BS_SOLID : BS_NULL;

	sSelectPenAndBrush();
}

/*****************************  GGetPenMode  *******************************/

short GGetPenMode ( void )
{
	if ( sDCAux )
		return ( GetROP2 ( sDCAux ) );
	else
		return ( GetROP2 ( sDC ) );
}

/*****************************  GSetPenMode  *******************************/

void GSetPenMode ( short wMode )
{
	if ( sDC )
		SetROP2 ( sDC, wMode );

	if ( sDCAux )
    	SetROP2 ( sDCAux, wMode );
}

/**************************  GGetPenFont  ***********************************/

void GGetPenFont ( char *lpszName, short *lpwSize, short *lpwStyle )
{
	HFONT	hFont;
    
	/*** Select the stock system font object into the current device
         context (or the auxiliary device context, if we're using one.)
         This will return a handle to the font which was previously
         selected. ***/

	if ( sDCAux )
    	hFont = SelectObject ( sDCAux, GetStockObject ( SYSTEM_FONT ) );
	else if ( sDC )
    	hFont = SelectObject ( sDC, GetStockObject ( SYSTEM_FONT ) );

    /*** If we got a font handle, convert it into a logical font structure.
         Then re-select the font handle into the current (or auxiliary)
         device context.  We do it this way because there's apparently no
         other way to get a handle to the currently-selected front than
         to select a new one.  Gotta love the Windows API. ***/

    if ( hFont )
	{
		GetObject ( hFont, sizeof ( LOGFONT ), &sLogFont );

    	if ( sDCAux )
        	SelectObject ( sDCAux, hFont );
		else if ( sDC )
        	SelectObject ( sDC, hFont );
	}

    /*** Finally, obtain the font name, size, and style flags from the
         logical font record. ***/
         
	GGetLogFont ( &sLogFont, lpszName, lpwSize, lpwStyle );
}

/**************************  GSetPenFont  ***********************************/

void GSetPenFont ( char *lpszName, short wSize, short wStyle )
{
	GSetLogFont ( &sLogFont, lpszName, wSize, wStyle );
	sSelectFont();
}

/**************************  GSetPenFontName  ***********************************/

void GSetPenFontName ( char *lpszFaceName )
{
	char	szFaceName[ LF_FACESIZE ];
	short	wSize, wStyle;

	GGetLogFont ( &sLogFont, szFaceName, &wSize, &wStyle );
	GSetLogFont ( &sLogFont, lpszFaceName, wSize, wStyle );

	sSelectFont();
}

/**************************  GSetPenFontSize  ***********************************/

void GSetPenFontSize ( short wSize )
{
	sLogFont.lfHeight = wSize;
	sSelectFont();
}

/**************************  GSetPenFontStyle  ***********************************/

void GSetPenFontStyle ( short wStyle )
{
	sLogFont.lfWeight = ( wStyle & G_BOLD_STYLE ) ? FW_BOLD : FW_NORMAL;
	sLogFont.lfItalic = ( wStyle & G_ITALIC_STYLE ) ? TRUE : FALSE;
	sLogFont.lfUnderline = ( wStyle & G_UNDERLINE_STYLE ) ? TRUE : FALSE;
	sLogFont.lfStrikeOut = ( wStyle & G_STRIKEOUT_STYLE ) ? TRUE : FALSE;

	sSelectFont();
}

/****************************  GSetClipRect  *********************************/

void GSetClipRect ( LPRECT lpRect )
{
	POINT	points[2];
	HRGN	hRgn;

	/*** Convert the rectangle's coordinates from logical to device coordinates
		 because the clip region must be defined in device coordinates. ***/

	points[0].x = lpRect->left;
	points[0].y = lpRect->top;

	points[1].x = lpRect->right;
	points[1].y = lpRect->bottom;

	LPtoDP ( sDC, points, 2 );

	/*** Create the a rectangular region with the coordinates computed above,
		 and if successful, select it into the current device context. ***/

	if ( ( hRgn = CreateRectRgn ( points[0].x, points[0].y, points[1].x, points[1].y ) ) != NULL )
	{
		SelectClipRgn ( sDC, hRgn );
		DeleteObject ( hRgn );
	}
}

/****************************  GSetClipRegion ********************************/

void GSetClipRegion ( HRGN hRgn )
{
	/*** WE NEED TO CONVERT THE CLIP REGION FROM LOGICAL TO DEVICE COORDINATES:
		 HOW???!!! ***/

	SelectClipRgn ( sDC, hRgn );
}

/**************************  GSetNullClipRegion  *****************************/

void GSetNullClipRegion ( void )
{
	SelectClipRgn ( sDC, NULL );
}

/******************************  GDrawDot  *********************************/

void GDrawDot ( short wCol, short wRow )
{
	MoveToEx ( sDC, wCol, wRow, NULL );
    LineTo ( sDC, wCol + 1, wRow );
}

/******************************  GDrawRect  ********************************/

void GDrawRect ( LPRECT lpRect )
{
	Rectangle ( sDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom );
}

/***************************  GDrawRectangle  ******************************/

void GDrawRectangle ( short wLeft, short wTop, short wRight, short wBottom )
{
	Rectangle ( sDC, wLeft, wTop, wRight, wBottom );
}

/****************************  GDrawEllipse  *******************************/

void GDrawEllipse ( short wLeft, short wTop, short wRight, short wBottom )
{
	Ellipse ( sDC, wLeft, wTop, wRight, wBottom );
}

/*************************  GCreatePolygon  ********************************/

GPolygonPtr GCreatePolygon ( short num, short x[], short y[] )
{
	short			i;
	GPolygonPtr		polygon = NULL;

	if ( ( polygon = (LPPOINT) GlobalAllocPtr ( GHND, ( num + 1 ) * sizeof ( POINT ) ) ) != NULL )
	{
		polygon[0].x = num;
		for ( i = 0; i < num; i++ )
		{
			polygon[i + 1].x = x[i];
			polygon[i + 1].y = y[i];
		}
	}

	return ( polygon );
}

/***************************  GDeletePolygon  ********************************/

void GDeletePolygon ( GPolygonPtr polygon )
{
	GlobalFreePtr ( (LPSTR) polygon );
}

/***************************  GDrawPolygon  **********************************/

void GDrawPolygon ( GPolygonPtr polygon )
{
	Polygon ( sDC, &polygon[1], polygon[0].x );
}

/***************************  GDrawArc  **************************************/

void GDrawArc ( LPRECT lpRect, short x1, short y1, short x2, short y2 )
{
	if ( sLogBrush.lbStyle == BS_NULL )
		Arc ( sDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, x1, y1, x2, y2 );
	else
		Pie ( sDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, x1, y1, x2, y2 );
}

/******************************  GDrawLineTo  ********************************/

void GDrawLineTo ( short wX, short wY )
{
	LineTo ( sDC, wX, wY );
}

/*****************************  GMovePenTo  **********************************/

void GMovePenTo ( short wX, short wY )
{
	MoveToEx ( sDC, wX, wY, NULL );
}

/****************************  GGetPenPosition  *****************************/

void GGetPenPosition ( short *lpwX, short *lpwY )
{
	POINT	point;

    GetCurrentPositionEx ( sDC, &point );

    *lpwX = point.x;
    *lpwY = point.y;
}

/******************************  GDrawLine  ********************************/

void GDrawLine ( short xStart, short yStart, short xEnd, short yEnd )
{
	if ( xEnd > xStart )
    	xEnd++;

    if ( xEnd < xStart )
    	xEnd--;

    if ( yEnd > yStart )
    	yEnd++;

    if ( yEnd < yStart )
    	yEnd--;
        
	MoveToEx ( sDC, xStart, yStart, NULL );
	LineTo ( sDC, xEnd, yEnd );
}

/******************************  GGetIcon  **********************************/

GIconPtr GGetIcon ( short id )
{
	return ( LoadIcon ( GGetInstance(), MAKEINTRESOURCE ( id ) ) );
}

/******************************  GDrawIcon  *********************************/

void GDrawIcon ( GIconPtr hIcon, short nLeft, short nTop )
{
	DrawIcon ( sDC, nLeft, nTop, hIcon );
}

/******************************  GDrawString  *******************************/

void GDrawString ( char *string, short length, short x, short y )
{
	if ( length == 0 )
		length = strlen ( string );
		
	TextOut ( sDC, x, y, string, length );
}

/*****************************  GGetString  *********************************/

char *GGetString ( short id, char *string )
{
	if ( LoadString ( GGetInstance(), id, string, 255 ) > 0 )
    	return ( string );
    else
    	return ( NULL );
}

/******************************  GString  ***********************************/

char *GString ( short id )
{
	static char string[256];
	
	if ( LoadString ( GGetInstance(), id, string, 255 ) > 0 )
    	return ( string );
    else
    	return ( NULL );
}

/****************************  GGetStringSize  *******************************/

void GGetStringSize ( char *lpszString, short cbString, short *lpcxString, short *lpcyString )
{
	SIZE	size;

	if ( cbString == 0 )
		cbString = strlen ( lpszString );

	if ( sDCAux )
		GetTextExtentPoint ( sDCAux, lpszString, cbString, &size );
	else
   		GetTextExtentPoint ( sDC, lpszString, cbString, &size );

	if ( lpcxString )
		*lpcxString = size.cx;
	
	if ( lpcyString )
		*lpcyString = size.cy;
}

/**************************  GCreatePicture  *******************************/

LPMETAFILEPICT GCreatePicture ( LPRECT lpRect )
{
	LPMETAFILEPICT	lpPict;

	if ( ( lpPict = (LPMETAFILEPICT) GlobalAllocPtr ( GPTR, sizeof ( METAFILEPICT ) ) ) != NULL )
	{
		lpPict->mm = MM_TEXT;
		lpPict->xExt = lpRect->right - lpRect->left;
		lpPict->yExt = lpRect->bottom - lpRect->top;
		lpPict->hMF = NULL;
	}

    return ( lpPict );
}

/**************************  GDeletePicture  ********************************/

void GDeletePicture ( LPMETAFILEPICT lpPict )
{
	DeleteMetaFile ( lpPict->hMF );
    GlobalFreePtr ( lpPict );
}

/************************  GGetPicture  **************************************/

LPMETAFILEPICT GGetPicture ( short id )
{
	return ( NULL );
}

/**************************  GDrawPicture  **********************************/

void GDrawPicture ( LPMETAFILEPICT lpPict, LPRECT lpRect )
{
	/*** First save the device context's current attributes, since the
		 metafile may change them when we play it back. ***/

	SaveDC ( sDC );

	/*** Now set the graphic device's coordinate mapping mode to]
		 anisotropic, so the metafile may be scaled arbitrarily.
		 Set the device's coordinate frame origin and extent as
		 described in the given rectangle so that the metafile gets
		 scales as the user wants it.  NOTE: in order for this to
		 work, the metafile must contain calls to SetWindowOrg() and
		 SetWindowExt(), which describe its own logical coordinate frame;
		 Windows needs both to scale from the metafile's logical
	     coordinates to the graphic device's coordinates. ***/

	SetMapMode ( sDC, MM_ANISOTROPIC );
	SetViewportOrgEx ( sDC, lpRect->left, lpRect->top, NULL );
	SetViewportExtEx ( sDC, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, NULL );


	/*** Finally, play the metafile into the graphic device.  When we
		 are done, restore the graphic device's old attributes. ***/

	PlayMetaFile ( sDC, lpPict->hMF );
	RestoreDC ( sDC, -1 );
}

/**************************  GCreateBitmap  *******************************/

HBITMAP GCreateBitmap ( LPRECT lpRect )
{
	int		nWidth, nHeight;
	HDC		hDCBmp;
	HBITMAP	hBmp;

	nWidth = lpRect->right - lpRect->left;
	nHeight = lpRect->bottom - lpRect->top;

	hDCBmp = GetDC ( NULL );
	hBmp = CreateCompatibleBitmap ( hDCBmp, nWidth, nHeight );
	ReleaseDC ( NULL, hDCBmp );

    return ( hBmp );
}

/*** GBitmapToImageDataRow **************************************************/

void GBitmapToImageDataRow ( GBitmapPtr hBitmap, short wBitmapRow, GImagePtr pImage, short wImageRow )
{
	int		nResult;
	HDC		hDC;
	
	hDC = GetDC ( NULL );
	nResult = GetDIBits ( hDC, hBitmap, wBitmapRow, 1, GGetImageDataRow ( pImage, wImageRow ), pImage, DIB_RGB_COLORS );
	ReleaseDC ( NULL, hDC );
}

/*** GImageToBitmapDataRow **************************************************/

void GImageToBitmapDataRow ( GImagePtr pImage, short wImageRow, GBitmapPtr hBitmap, short wBitmapRow )
{
	int		nResult;
	HDC		hDC;

	hDC = GetDC ( NULL );
	nResult = SetDIBits ( hDC, hBitmap, wBitmapRow, 1, GGetImageDataRow ( pImage, wImageRow ), pImage, DIB_RGB_COLORS );
	ReleaseDC ( NULL, hDC );
}

/****************************  GDeleteBitmap  ********************************/

void GDeleteBitmap ( HBITMAP hBmp )
{
	DeleteObject ( hBmp );
}

/***********************  GGetBitmap  ****************************************/

GBitmapPtr GGetBitmap ( short id )
{
	return ( LoadBitmap ( GGetInstance(), MAKEINTRESOURCE ( id ) ) );
}

/****************************  GGetBitmapRect  *****************************/

GRectPtr GGetBitmapRect ( GBitmapPtr hBitmap, GRectPtr lpRect )
{
	BITMAP	bm;

	GetObject ( hBitmap, sizeof ( bm ), &bm );

	lpRect->left = 0;
	lpRect->top = 0;
	lpRect->right = bm.bmWidth;
    lpRect->bottom = bm.bmHeight;

	return ( lpRect );
}

/**************************  GGetBitmapData  *******************************/

GPtr GGetBitmapData ( GBitmapPtr bitmap, short *bitpix, short *rowbytes )
{
	long		width, height, colors, size;
	HDC			hdc;
	BITMAP		bm;
	BITMAPINFOHEADER *infoPtr;
	LPSTR		dataPtr = NULL;
	RGBQUAD		*palette;

	/*** Obtain a handle to the screen's device context.  If successful,
		 and the bitmap handle is valid, convert it to DIB format ***/

	hdc = GetDC ( NULL );
	if ( hdc && bitmap )
	{
		/*** Determine width, height, and bits/pixel of original bitmap ***/

		GetObject ( bitmap, sizeof ( BITMAP ), &bm );

		width = bm.bmWidth;
        height = bm.bmHeight;
		*bitpix = bm.bmPlanes * bm.bmBitsPixel;

		/*** Determine number of colors needed for color table (a 24-bit/pixel
			 bitmap needs no color table since the image data array will contain
			 the image's RGB color values written out explicitly) ***/

		if ( *bitpix < 24 )
			colors = ((long) 1 ) << *bitpix;
		else
			colors = 0;

		/*** Now allocate a buffer to hold the bitmap info header, plus enough
			 bytes to hold the color table ***/

		size = sizeof ( BITMAPINFOHEADER ) + colors * sizeof ( RGBQUAD );
		if ( ( infoPtr = (BITMAPINFOHEADER *) GlobalAllocPtr ( GMEM_MOVEABLE, size ) ) != NULL )
		{
			infoPtr->biSize = sizeof ( BITMAPINFOHEADER );
			infoPtr->biWidth = width;
			infoPtr->biHeight = height;
			infoPtr->biPlanes = 1;
			infoPtr->biBitCount = *bitpix;
			infoPtr->biCompression = 0;
			infoPtr->biSizeImage = 0;
			infoPtr->biXPelsPerMeter = 0;
			infoPtr->biYPelsPerMeter = 0;
			infoPtr->biClrUsed = 0;
			infoPtr->biClrImportant = 0;

			/*** Now call GetDIBits() with a NULL pointer to the image data
				 buffer, so it calculates the size needed for the buffer.
				 If Windows computed the image size, we compute the bytes-per-
				 image-row from it.  If windows did not compute an image size
				 for us, we have to do it ourselves.  How pathetic. ***/

			GetDIBits ( hdc, bitmap, 0, height, NULL, (BITMAPINFO *) infoPtr, DIB_RGB_COLORS );
			if ( infoPtr->biSizeImage )
			{
				*rowbytes = infoPtr->biSizeImage / height;
			}
			else
			{
				*rowbytes = G_BITMAP_ROW_BYTES ( *bitpix, width );
				infoPtr->biSizeImage = (long) *rowbytes * height;
            }

			palette = (RGBQUAD *) ( infoPtr + 1 );

			/*** Then allocate a buffer to hold the actual image data, and
				 if successful copy the image data into the buffer in
				 device-independent format. ***/

			if ( ( dataPtr = GlobalAllocPtr ( GMEM_MOVEABLE, infoPtr->biSizeImage ) ) != NULL )
				GetDIBits ( hdc, bitmap, 0, height, dataPtr, (BITMAPINFO *) infoPtr, DIB_RGB_COLORS );

			GlobalFreePtr ( infoPtr );
		}

		ReleaseDC ( NULL, hdc );
	}

	/*** Return the handle to the device-independent bitmap ***/

	return ( dataPtr );
}

/****************************  GSetBitmapData  *******************************/

void GSetBitmapData ( GBitmapPtr bitmap, short bitpix, short rowbytes, GPtr data )
{
	long		width, height, colors, size;
	HDC			hdc;
	BITMAP		bm;
	BITMAPINFOHEADER *infoPtr;
	RGBQUAD		*palette;

	/*** Obtain a handle to the screen's device context.  If successful,
		 and the bitmap handle is valid, convert it to DIB format ***/

	hdc = GetDC ( NULL );
	if ( hdc && bitmap )
	{
		/*** Determine width, height, and bits/pixel of original bitmap ***/

		GetObject ( bitmap, sizeof ( BITMAP ), &bm );

		width = bm.bmWidth;
        height = bm.bmHeight;
		if ( bm.bmPlanes * bm.bmBitsPixel == bitpix )
		{
			if ( bitpix < 24 )
				colors = ((long) 1 ) << bitpix;
			else
				colors = 0;

			size = sizeof ( BITMAPINFOHEADER ) + colors * sizeof ( RGBQUAD );
			if ( ( infoPtr = (BITMAPINFOHEADER *) GlobalAllocPtr ( GMEM_MOVEABLE, size ) ) != NULL )
			{
				infoPtr->biSize = sizeof ( BITMAPINFOHEADER );
				infoPtr->biWidth = width;
				infoPtr->biHeight = height;
				infoPtr->biPlanes = 1;
				infoPtr->biBitCount = bitpix;
				infoPtr->biCompression = 0;
				infoPtr->biSizeImage = 0;
				infoPtr->biXPelsPerMeter = 0;
				infoPtr->biYPelsPerMeter = 0;
				infoPtr->biClrUsed = 0;
				infoPtr->biClrImportant = 0;

				GetDIBits ( hdc, bitmap, 0, height, NULL, (BITMAPINFO *) infoPtr, DIB_RGB_COLORS );
				if ( infoPtr->biSizeImage == 0 )
					infoPtr->biSizeImage = (long) rowbytes * height;

				palette = (RGBQUAD *) ( infoPtr + 1 );

				SetDIBits ( hdc, bitmap, 0, height, data, (BITMAPINFO *) infoPtr, DIB_RGB_COLORS );

				GlobalFreePtr ( infoPtr );
			}
		}

		ReleaseDC ( NULL, hdc );
	}
    GlobalFreePtr ( data );
}

/***************************  GDrawBitmap  **************************/

void GDrawBitmap ( HBITMAP hBmp, GRectPtr lpRectSrc, GRectPtr lpRectDst )
{
	int		nLeftSrc, nTopSrc, nWidthSrc, nHeightSrc;
	int		nLeftDst, nTopDst, nWidthDst, nHeightDst;
	BITMAP	bm;
	HDC		hDCBmp;

	/*** If we have been given a source rectangle, use its boundaries to determine
		  the top left corner and dimensions of the source area to copy from the
		  bitmap.  Otherwise, use the entire bitmap. ***/

	if ( lpRectSrc )
	{
		nLeftSrc = lpRectSrc->left;
		nTopSrc = lpRectSrc->top;
		nWidthSrc = lpRectSrc->right - lpRectSrc->left;
		nHeightSrc = lpRectSrc->bottom - lpRectSrc->top;
	}
	else
	{
		GetObject ( hBmp, sizeof ( BITMAP ), (LPSTR) &bm );
		nLeftSrc = 0;
		nTopSrc = 0;
		nWidthSrc = bm.bmWidth;
		nHeightSrc = bm.bmHeight;
	}

	/*** If we have been given a destination rectangle, use it;
		 otherwise, use the same destination coordinates as the source. ***/

	if ( lpRectDst )
	{
		nLeftDst = lpRectDst->left;
		nTopDst = lpRectDst->top;
		nWidthDst = lpRectDst->right - lpRectDst->left;
		nHeightDst = lpRectDst->bottom - lpRectDst->top;
	}
	else
	{
		nLeftDst = nLeftSrc;
		nTopDst = nTopSrc;
		nWidthDst = nWidthSrc;
		nHeightDst = nHeightSrc;
	}

	/*** Create a memory device context with the same color organization as
		 the screen's.  If successful, select the bitmap as the display surface
		 of that device context.  Then copy the bitmap from the memory device
		 context to the device context we are currently drawing into.  Finally,
		 delete the memory device context. ***/

	if ( ( hDCBmp = CreateCompatibleDC ( NULL ) ) != NULL )
	{
		SelectObject ( hDCBmp, hBmp );
		if ( nWidthSrc == nWidthDst && nHeightSrc == nHeightDst )
		{
			BitBlt ( sDC, nLeftDst, nTopDst, nWidthDst, nHeightDst,
			hDCBmp, nLeftSrc, nTopSrc, SRCCOPY );
		}
		else
		{
			StretchBlt ( sDC, nLeftDst, nTopDst, nWidthDst, nHeightDst,
			hDCBmp, nLeftSrc, nTopSrc, nWidthSrc, nHeightSrc, SRCCOPY );
		}

		DeleteDC ( hDCBmp );
	}
}

/*****************************  GDoColorDialog  ******************************/

int GDoColorDialog ( char *prompt, GColor *color )
{
	int					nResult;
	CHOOSECOLOR			cc;
	static COLORREF		colors[16] = { 0 };

	cc.lStructSize = sizeof ( CHOOSECOLOR );
	cc.hwndOwner = GetActiveWindow();
	cc.hInstance = GGetInstance();
	cc.rgbResult = *color;
	cc.lpCustColors = colors;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.lCustData = 0;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	if ( ( nResult = ChooseColor ( &cc ) ) != FALSE )
	{
		*color = cc.rgbResult;
	}

	return ( nResult );
}

/*************************  GDoFontDialog  **********************************/

int GDoFontDialog ( char *lpszFont, short *lpwSize, short *lpwStyle, COLORREF *lpColor )
{
	int			nResult = FALSE;
	LOGFONT		lf;
	CHOOSEFONT	cf;

	/*** Initialize the LOGFONT structure ***/

	GSetLogFont ( &lf, lpszFont, *lpwSize, *lpwStyle );

	/*** Initialize the CHOOSEFONT stucture. ***/

	cf.lStructSize = sizeof ( CHOOSEFONT );
	cf.hwndOwner = GetActiveWindow();
	cf.hDC = NULL;
	cf.lpLogFont = &lf;
	cf.iPointSize = 0;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;

	if ( lpColor )
		cf.rgbColors = *lpColor;
	else
		cf.rgbColors = 0;

	cf.lCustData = 0;
	cf.lpfnHook = NULL;
	cf.lpTemplateName = NULL;
	cf.hInstance = NULL;
	cf.lpszStyle = NULL;
	cf.nFontType = 0;
	cf.nSizeMin = 0;
	cf.nSizeMax = 0;

	/*** Display the standard "Choose Font..." dialog.  If the user exits
		 by hitting the "OK" button, extract the font size, style, and
		 face name from the CHOOSEFONT structure, and return TRUE.  If
		 the user hits "Cancel" or other failure occurs, return FALSE
		 without modifying the input font name, size, style, etc. ***/

	if ( ( nResult = ChooseFont ( &cf ) ) != FALSE )
	{
		GGetLogFont ( &lf, lpszFont, lpwSize, lpwStyle );
		if ( lpColor )
			*lpColor = cf.rgbColors;
	}

	return ( nResult );
}

/****************************  GSetLogFont  *********************************/

void GSetLogFont ( LOGFONT *lpLogFont, char *lpszName, short wSize, short wStyle )
{
	lpLogFont->lfHeight = wSize;
	lpLogFont->lfWidth = 0;
	lpLogFont->lfEscapement = 0;
	lpLogFont->lfOrientation = 0;

	lpLogFont->lfWeight = wStyle & G_BOLD_STYLE ? FW_BOLD : FW_NORMAL;
	lpLogFont->lfItalic = wStyle & G_ITALIC_STYLE ? TRUE : FALSE;
	lpLogFont->lfUnderline = wStyle & G_UNDERLINE_STYLE ? TRUE : FALSE;
	lpLogFont->lfStrikeOut = wStyle & G_STRIKEOUT_STYLE ? TRUE : FALSE;

	lpLogFont->lfCharSet = DEFAULT_CHARSET;
	lpLogFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lpLogFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lpLogFont->lfQuality = DEFAULT_QUALITY;
	lpLogFont->lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	lstrcpyn ( lpLogFont->lfFaceName, lpszName, LF_FACESIZE );
}

/*****************************  GGetLogFont  *********************************/

void GGetLogFont ( LOGFONT *lpLogFont, char *lpszName, short *lpwSize, short *lpwStyle )
{
	strcpy ( lpszName, lpLogFont->lfFaceName );

	*lpwSize = lpLogFont->lfHeight;
	*lpwStyle = 0;

	if ( lpLogFont->lfWeight == FW_BOLD )
		*lpwStyle |= G_BOLD_STYLE;

	if ( lpLogFont->lfItalic )
		*lpwStyle |= G_ITALIC_STYLE;

	if ( lpLogFont->lfUnderline )
		*lpwStyle |= G_UNDERLINE_STYLE;

	if ( lpLogFont->lfStrikeOut )
		*lpwStyle |= G_STRIKEOUT_STYLE;
}

/***************************  sSelectPenAndBrush  *************************/

void sSelectPenAndBrush ( void )
{
	HPEN	hPen;
	HBRUSH	hBrush;

	if ( ( hPen = CreatePenIndirect ( &sLogPen ) ) != NULL )
	{
		if ( sDC )
			SelectObject ( sDC, hPen );

		if ( sDCAux )
			SelectObject ( sDCAux, hPen );

		if ( sPen )
			DeleteObject ( sPen );

		sPen = hPen;
	}

	if ( ( hBrush = CreateBrushIndirect ( &sLogBrush ) ) != NULL )
	{
		if ( sDC )
			SelectObject ( sDC, hBrush );

		if ( sDCAux )
			SelectObject ( sDCAux, hBrush );

		if ( sBrush )
			DeleteObject ( sBrush );

		sBrush = hBrush;
	}
}

/****************************  sSelectFont  *********************************/

void sSelectFont ( void )
{
	HFONT	hFont;

	if ( ( hFont = CreateFontIndirect ( &sLogFont ) ) != NULL )
	{
		if ( sDC )
			SelectObject ( sDC, hFont );

		if ( sDCAux )
			SelectObject ( sDCAux, hFont );

		if ( sFont )
			DeleteObject ( sFont );

		sFont = hFont;
	}
}
