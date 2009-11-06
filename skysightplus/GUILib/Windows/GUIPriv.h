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

/*** GUILib private window styles ***/

#define WS_MINICAPTION		0x4000L
	
/*** GUILib private #defines ***/

#ifndef MAX_PATH
#define MAX_PATH	256
#endif

#define	G_BITMAP_ROW_BYTES(nBitsPix,nWidth) \
( ( ( (long) nBitsPix * nWidth + 31 ) / 32 ) * 4 )

/*** GUILib private data types ***/

typedef struct GWindowData GWindowData, *GWindowDataPtr;

struct GWindowData
{
	GWindowDataPtr	pNext;
	GWindowPtr		hWnd;
	GEventProcPtr	pfnProc;
	long			lData;
	long			lFlags;
	short			wClass;
};

#define G_ERASE_BACKGROUND		0x00000002
#define G_IS_FLOATING_WINDOW	0x00000004

#define ID_SIZEBOX				-3

/*** functions in Events.c ***/

void	GDoScrollMessage ( HWND, HWND, UINT, int );
void	GDoCommandMessage ( HWND, int, HWND, UINT );
void	GSupressCommandMessage ( HWND );
int		GSendEvent ( short, GWindowPtr, long, long );
int		GDoMessage ( HWND, UINT, WPARAM, LPARAM );
int		GDoWMCOMMAND ( HWND, UINT, WPARAM, LPARAM );

/*** Functions in Windows.c ***/

GWindowDataPtr	GCreateGWindowData ( short, GEventProcPtr, long );
void			GDeleteGWindowData ( GWindowDataPtr );
GWindowDataPtr	GGetGWindowData ( GWindowPtr );
void			GSetGWindowData ( GWindowPtr, GWindowDataPtr );
GWindowDataPtr	GGetGWindowDataList ( void );

BOOL			GRegisterWindowClass ( LPSTR, int, WNDPROC, int );
HWND			GCreateSizeBox ( HWND, BOOL );
void			GMoveSizeBox ( HWND );

HWND			GGetMDIClientWindow ( void );


/*** functions in Files.c ***/

HGLOBAL	GBitmapToDIB ( HBITMAP, long * );

/*** functions in Dialogs.c ***/

void	GDoDialogControlClick ( HWND, short );

/*** functions in Graphics.c ***/

GPicturePtr GCreatePicture ( GRectPtr );
void	GSetLogFont ( LOGFONT *, char *, short, short );
void	GGetLogFont ( LOGFONT *, char *, short *, short * );

/*** GUILib private data ***/

#ifndef GGLOBAL
#define GGLOBAL extern
#endif

GGLOBAL GWindowDataPtr	gGWindowDataList;	/* Linked list of window data records */

GGLOBAL HWND			gClientWindow;		/* MDI client support; NULL if non-MDI */
GGLOBAL HMENU			gWindowMenu;		/* MDI document list menu */

GGLOBAL HMENU			gMenuBar;			/* Handle to main menu bar */
GGLOBAL HACCEL			gAccelerators;		/* Handle to menu accelerator table */

#undef GLOBAL

