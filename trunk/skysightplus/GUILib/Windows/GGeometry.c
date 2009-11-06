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

/****************************  header files  **********************************/

#include "GUILib.h"
#include "GUIPriv.h"

/*****************************  GRectangle  *******************************/

GRectPtr GRectangle ( short wLeft, short wTop, short wRight, short wBottom )
{
	static RECT		rect;
	
	rect.left   = wLeft;
	rect.top    = wTop;
	rect.right  = wRight;
	rect.bottom = wBottom;
	
	return ( &rect );
}

/*****************************  GSetRect  ********************************/

GRectPtr GSetRect ( GRectPtr lpRect, short wLeft, short wTop, short wRight, short wBottom )
{
	lpRect->left = wLeft;
	lpRect->top = wTop;
	lpRect->right = wRight;
	lpRect->bottom = wBottom;
	
	return ( lpRect );
}

/****************************  GMoveRect  ******************************/

GRectPtr GMoveRect ( GRectPtr lpRect, short wRight, short wDown )
{
	lpRect->left += wRight;
	lpRect->top += wDown;
	lpRect->right += wRight;
    lpRect->bottom += wDown;

	return ( lpRect );
}

/****************************  GSizeRect  ******************************/

GRectPtr GSizeRect ( GRectPtr lpRect, short wWidth, short wHeight )
{
	lpRect->right = lpRect->left + wWidth;
	lpRect->bottom = lpRect->top + wHeight;

	return ( lpRect );
}

/****************************  GClipRect  **************************************/

GRectPtr GClipRect ( GRectPtr rect, GRectPtr bounds )
{
	rect->left = ( rect->left < bounds->left ) ? bounds->left : rect->left;
	rect->top = ( rect->top < bounds->top ) ? bounds->top : rect->top;
	rect->right = ( rect->right > bounds->right ) ? bounds->right : rect->right;
	rect->bottom = ( rect->bottom > bounds->bottom ) ? bounds->bottom : rect->bottom;
	
	return ( rect );
}

/****************************  GInsetRect  *****************************/

LPRECT GInsetRect ( LPRECT lpRect, short wWidth, short wHeight )
{
	lpRect->left += wWidth;
	lpRect->top += wHeight;
	lpRect->right -= wWidth;
	lpRect->bottom -= wHeight;

    return ( lpRect );
}

/****************************  GCenterRect  ****************************/

LPRECT GCenterRect ( LPRECT lpRect, LPRECT lpFrame )
{
	short	wWidth = lpRect->right - lpRect->left;
	short	wHeight = lpRect->bottom - lpRect->top;
	
	lpRect->left = lpFrame->left + ( ( lpFrame->right - lpFrame->left ) - wWidth ) / 2;
	lpRect->top = lpFrame->top + ( ( lpFrame->bottom - lpFrame->top ) - wHeight ) / 2;
	lpRect->right = lpRect->left + wWidth;
	lpRect->bottom = lpRect->top + wHeight;
	
	return ( lpRect );
}

/**************************  GIsPointInRect  ***************************/

int GIsPointInRect ( short wX, short wY, LPRECT lpRect )
{
	if ( wX >= lpRect->left && wX < lpRect->right && wY >= lpRect->top && wY < lpRect->bottom )
		return ( TRUE );
	else
		return ( FALSE );
}

/**************************  GClipLineToRect  ***************************/

void GClipLineToRect ( short *x1, short *y1, short *x2, short *y2, LPRECT rect )
{
	long	dx = *x2 - *x1;
	long	dy = *y2 - *y1;

	if ( ( *x2 < rect->left || *x2 > rect->right ) && dx )
	{
		if ( *x2 < rect->left )
			*x2 = rect->left;

		if ( *x2 > rect->right )
			*x2 = rect->right;

		*y2 = dy * ( *x2 - *x1 ) / dx + *y1;
	}

	if ( ( *y2 < rect->top || *y2 > rect->bottom ) && dy )
	{
		if ( *y2 < rect->top )
			*y2 = rect->top;

		if ( *y2 > rect->bottom )
			*y2 = rect->bottom;

		*x2 = dx * ( *y2 - *y1 ) / dy + *x1;
	}
}

