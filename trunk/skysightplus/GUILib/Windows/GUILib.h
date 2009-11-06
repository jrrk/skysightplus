/*** Copyright Notice and Public Source License *********************************

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

/***************************  Target.h  ************************************

	This header file is required by GUILib but must be provided by you.
	It describes the target you wish to compile by #defining one or more
	of the following symbols.

	#define GMAC	- Macintosh System 7.x, Mac OS 8 - 9
	#define GWIN16  - Microsoft Windows, 16-bit (3.1)
	#define GWIN32	- Microsoft Windows, 32-bit (Win32s, 95, 98, Me, NT, 2K)

	Note: if you #define GWIN16 or GWIN32, this header file automatically
	#defines GWIN as well; thus you can use #ifdef GWIN to test for any
	version of Microsoft Windows (16-bit or 32-bit).

	You may also optionally #define the following symbols if your target
	platform is Microsoft Windows:

	#define GWINCTL3D	- Use Microsoft Windows 3D control library.

	If you #define GWINCTL3D, GUILib.h then #includes the Microsoft Windows
	3D control header file <ctl3d.h>.  This requires you to link with the
	appropriate 3D control library (CTL3D.LIB, CTL3DV2.LIB, or CTL3D32.LIB),
	and ensure that the corresponding CTL3D.DLL file is properly installed
	on the target system at runtime.

    #define GWINCOM32S  - Use 16-bit Windows communications for Win32s.

    If you #define GWINCOM32S, GUILib.h then #includes the Com32s header
    file "Com32s.h" for accessing the 16-bit Windows communications
    functions from within a 32-bit Windows program.  This should only be
    necessary if you are planning to run your program under Win32s; if you
    run it under a true 32-bit Windows OS (95, 98, or NT) you should not
    #define GWINCOM32S so that GUILib uses the real 32-bit Windows
    communications API.  If you #define this symbol, you must also link with
    COM32S32.LIB, and make sure that both COM32S32.DLL and COM32S16.DLL are
    present at runtime.

	#define GJPEG - Use Independent JPEG Group's JPEG file I/O library.
	
	If you #define GJPEG, GUILib.h then #includes the IJG's "jpeglib.h" header
	file, and code for reading and writing JPEG-compressed image files is
	conditionally complied.  Make sure you are including the JPEG library
	source files and/or libraries as well; otherwise your application will not
	link.  If this symbol is undefined (by default), the GUILib functions
	related to reading and writing JPEG image files will not be defined.
	See GReadJPEGFile(), below.
	
	The latest version of the IJG JPEG library can be downloaded from
	http://www.ijg.org/  This release of GUILib assumes version 6b of the
	JPEG library is present.  Also, please note that the JPEG library
	requires a platform-specific file called "jconfig.h".  Several
	templates for such a file are included with the library, called (e.g.)
	"jconfig.mac" for Macintosh, "jconfig.vc" for Microsoft Visual C++,
	etc.  We suggest that you rename the appropriate file for your
	platfrom to "jconfig.h" and place it in the same directory as the
	GUILib "Target.h" file -- that way, multiple conflicting versions
	of "jpeglib.h" will not be present in the JPEG library source
	directory.

	#define GTIFF - Use Sam Leffer's TIFF file I/O library.
	
	If you #define GTIFF, GUILib.h then #includes the IJG's "tiffio.h" header
	file, and code for reading and writing TIFF-compressed image files is
	conditionally complied.  Make sure you are including the TIFF library
	source files and/or libraries as well; otherwise your application will not
	link.  If this symbol is undefined (by default), the GUILib functions
	related to reading and writing TIFF image files will not be defined.
	See GReadTIFFImageFile(), below.
	
	The latest version of the TIFF library can be downloaded from
	http://www.libtiff.org/  This release of GUILib assumes version 3.5.5 of
	the TIFF library is present.  Please note that the TIFF library is missing
	two of the source files required to link everything together.  These source
	files (tif_fax3sm.c and version.c) must be built using programs included
	with the TIFF library distribution.  We suggest that once you do this,
	you keep a copy of these files handy so that you never have to deal with
	that problem again.  An easier way around this problem may be to simply
	rename the files fax3sm_winnt.c (in the TIFF library's libtiff subdirectory)
	and version.c (in the contrib/winnt subdirectory) to tif_fax3sm.c and
	version.c; this seems to work on both Macintosh and Windows.

	#define GGIF
	
	If you #define GGIF, GUILib.h then #includes the public-domain "gif_lib.h"
	header file, and code for reading and writing GIF-compressed image files is
	conditionally complied.  Make sure you are including the GIF library
	source files and/or libraries as well; otherwise your application will not
	link.  If this symbol is undefined (by default), the GUILib functions
	related to reading and writing GIF image files will not be defined.
	See GReadGIFImageFile(), below.

	The latest version of the TIFF library can be downloaded from:
	
	http://prtr-13.ucsc.edu/~badger/software/libungif/giflib.shtml
	
	This release of GUILib assumes version 4.1 of the GIF library is present.
	Please note that several modifications, and one bug-fix, are required to
	be made to the library before it will link and run successfully.  These
	modifications are detailed in gif_changes.txt.
	
*******************************************************************************/

#ifndef GUILIB_H
#define GUILIB_H

#include "Target.h"

/*** Mac headers ***/

#ifdef GMAC
#include "Toolbox Extensions.h"
#endif

/*** Windows headers ***/

#ifdef GWIN16
#define	GWIN
#endif

#ifdef GWIN32
#define	GWIN
#endif

#ifdef GWIN

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>

#ifdef GWIN16
#include <dos.h>
#endif

#ifdef GWIN32
#include <commctrl.h>
#endif

#ifdef GWINCTL3D
#include <ctl3d.h>
#endif

#ifdef GWINCOM32S
#include "com32s.h"
#endif
#endif

/*** ANSI headers ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#ifdef __MWERKS__
#include <unix.h>
#endif

/*** Define all version of true and false ***/

#ifndef false
#define false 0
#endif
#ifndef true
#define true (!false)
#endif

#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif

/*** JPEG headers ***/

#ifdef GJPEG
#include "jpeglib.h"
#include <setjmp.h>
#endif

/*** TIFF headers ***/

#ifdef GTIFF
#include "tiffio.h"
#endif

/*** GIF headers ***/

#ifdef GGIF
#include "gif_lib.h"
#endif

/****************************  Native data types  ********************************

	Most of these data types are pointers or handles (pointers-to-pointers) to
	data structures which are defined in the native operating system's header
	files.  Hence, to find the internal format of a GWindowPtr, you would need
	to look in the Apple Universal Headers of Microsoft Windows header files.
	Because the internal formats of these objects are defined differently on
	different platforms, you should not write any code which accesses data
	structure members directly.  Instead, use the appropriate GUILib function
	to obtain or modify the data.

**********************************************************************************/

#ifdef GMAC

typedef WindowPtr		GWindowPtr;
typedef Handle			GMenuBarPtr;
typedef MenuHandle		GMenuPtr;
typedef ControlHandle	GControlPtr;

typedef EventRecord		GEvent, *GEventPtr;

typedef Ptr				GPtr;
typedef Handle			GHandle;
typedef Handle			GInstancePtr;

typedef FSSpecPtr		GPathPtr;
typedef short			GFilePtr;

typedef GrafPtr			GGraphicPtr;
typedef PixMapHandle	GBitmapPtr;
typedef PixMapHandle	GImagePtr;
typedef PicHandle		GPicturePtr;

typedef RGBColor		GColor, *GColorPtr;
typedef Rect			GRect, *GRectPtr;
typedef RgnHandle		GRegionPtr;
typedef PolyHandle		GPolygonPtr;
typedef Handle			GIconPtr;
typedef CursHandle		GCursorPtr;

typedef struct GPortRec	*GPortPtr;

#endif

#ifdef GWIN

typedef HWND			GWindowPtr;
typedef HMENU			GMenuBarPtr;
typedef HMENU			GMenuPtr;
typedef HWND			GControlPtr;

typedef MSG				GEvent, *GEventPtr;

typedef LPSTR			GPtr;
typedef HGLOBAL			GHandle;

typedef LPSTR			GPathPtr;
typedef HFILE			GFilePtr;

typedef HDC				GGraphicPtr;
typedef HBITMAP			GBitmapPtr;
typedef struct GImage	*GImagePtr;
typedef LPMETAFILEPICT	GPicturePtr;

typedef COLORREF		GColor, *GColorPtr;
typedef RECT			GRect, *GRectPtr;
typedef HRGN			GRegionPtr;
typedef LPPOINT			GPolygonPtr;
typedef HICON			GIconPtr;
typedef HCURSOR			GCursorPtr;

typedef HANDLE			GPortPtr;
typedef HINSTANCE		GInstancePtr;

#endif

/****************************  Event types  **********************************

	When an event (such as a mouse click) ocurrs, GUILib informs your
	application by calling an event-handling procedure you have written.
	One of the arguments GUILib passes to your procedure is an integer code
	describing what kind of event has happend.
	
	Many events are associated with a particular window; if so, GUILib tells
	you which window by passing your procedure a pointer to the window.   You
	can in fact assign a separate event-handling procedure to each window
	you create, so that all events for that window are redirected to its own
	particular procedure.  Other events are associated with the application
	on a more general level, and are not associated with any particular window.

	In addition to the event code, GUILib may pass your event-handling procedure
	some other information related to the event.  This information is passed
	in two additional arguments to your event-handling procedure.
	
	When your procedure has finished handling the event, it should tell GUILib
	whether or not to take any additional action in response to the event.  Your
	procedure does this by returning a simple TRUE or FALSE (i.e., yes or no)
	response to GUILib.  See GEnterEventLoop() for a more detailed description
	of how GUILib's event-handling mechanism works.
	
	The following section describes all the event types GUILib currently
	supports, whether or not each event is associated with a particular window,
	what (if any) additional information is provided along with the event code,
	and how GUILib responds to the event if you choose to let it.
	
******************************************************************************/

/****************************  G_NULL_EVENT  *******************************

	This is the event your application receives when nothing else happens.
	
	This event is never associated with any window, so the (window)
	parameter to your event-handling function is always NULL.  The other
	parameters, (param1) and (param2), contain zero.

	After your application enters its main event-handling loop, GUILib
	continuously checks for events and dispatches them to your application
	when they occur.  If your application has no events pending, GUILib
	sends it a null event instead.  Thus, while your application is simply
	running undisturbed, it may expect a rapid stream of null events.
	Although the exact interval between null events depends on the amount
	of time taken by background processes, you may normally expect up to
	several dozen null events per second.  Thus, your application might use
	this event to perform any periodic mantainance tasks it requires.  
	
****************************************************************************/

#define G_NULL_EVENT	0

/**************************  G_ENTRY_EVENT  **********************************

	Your application receives this event immediately before it enters its
	main event-handling loop.
	
	This event is never associated with any window, so the (window)
	parameter to your event-handling function is always NULL.  The other
	parameters, (param1) and (param2), contain zero.

	If your application uses GUILib's portable main() function, this event
	is the initial entry point to your application; thus you may use this
	event to perform your application-specific initializations.  If you
	are writing your own main() function instead, you might choose to ignore
	this event.
	
******************************************************************************/

#define G_ENTRY_EVENT	1

/************************** G_KEY_DOWN_EVENT  ********************************

	GUILib generates this event whenever the user presses a key on the
	keyboard.  Additionally, in Windows, for keystrokes which correspond to
	characters (e.g. the 'A' key, as opposed to the "Shift" key), GUILib
	generates a second key-down event indicating the ASCII character code
	corresponding to the keystroke.

	On both Mac and in Windows, this event is always sent to the application's
	active window, whose pointer is passed in the (window) argument to its event-
	handling function.  If no windows are active, the event is sent to the
	application's main event-handling function, in which case the (window)
	parameter contains NULL.

	What happens next is platform-dependent.  On the Mac, (param1) contains
	the integer key code corresponding to the key that was pressed, and
	(param2) contains the corresponding ASCII character code.
	
	In Windows, this information is sent to your application as two separate
	key-down events.  When your application's event-handling function receives
	the first key-down event, (param1) contains an integer code corresponding
	to the key that was pressed, and (param2) contains 0.  If the keystroke
	generates a character, your application receives a second key-down event;
	this time, (param1) contains 0 and (param2) contains the ASCII character
	code corresponding to the keystroke.

	For example, pressing the "Shift" key would generate a key-down event
	in which (param1) contains the "shift" key's key-code.  Pressing the "a"
	key would generate a key-down whose (param1) indicates the "A" key's code,
	then a second key-down event whose (param2) contains the ASCII value of
	the character 'a'.  Pressing the "A" key while the "Shift" key is already
	down would produce similar results, except that (param2) in the second
	key-down event would contain the ASCII code for a capital 'A' rather than
	a lower-case 'a'.

	Although handling key-downs in this platform-dependent way is somewhat
	awkward, it was (after much thought) the best mechanism I could come up
	with which accurately reflected the native behavior of the underlying OSes.
	On the Mac, the native event-generation mechanism puts both the key code
	and character code (if any) into the same event record.  Windows sends
	them separately, and since there's no guarantee that a Windows character
	message will follow a key-down message, it's impossible to try to
	combine them and send them together, Mac-style.
	
	In any event, doing it this way ensures that your application receives
	the proper interpretation of keystroke events, since it uses the native OS
	to translate raw keystrokes into characters.  Considering the vast assortment
	of different US keyboard layouts out there (not to mention international
	keyboards!), this is far better than trying to do it all yourself.
	
******************************************************************************/

#define G_KEY_DOWN_EVENT    2

/*** Key codes: the folloing is by no means a comprehensive list!  Also,
	 please note that the numerical values for these are platform-dependent.
	 The ASCII character codes, however, are not. ***/

#ifdef GMAC

#define G_LEFT_ARROW_KEY	0x7B
#define G_RIGHT_ARROW_KEY	0x7C
#define G_UP_ARROW_KEY		0x7E
#define G_DOWN_ARROW_KEY	0x7D
#define G_PAGE_UP_KEY		0x74
#define G_PAGE_DOWN_KEY		0x79
#define G_HOME_KEY			0x73
#define G_END_KEY			0x77
#define G_CLEAR_KEY			0x47
#define G_ESCAPE_KEY		0x35
#define G_ENTER_KEY			0x4C
#define G_RETURN_KEY		0x24
#define G_FORWARD_DEL_KEY	0x75
#define G_DELETE_KEY        0x33
#define G_HELP_KEY			0x72
#define G_SHIFT_KEY			0x38
#define G_CAPS_LOCK_KEY		0x39
#define G_CONTROL_KEY		0x3B
#define G_OPTION_KEY		0x3A
#define G_ALT_KEY			0x3A
#define G_COMMAND_KEY		0x37
#define G_SPACE_KEY			0x31
#define G_TAB_KEY			0x30
#define G_F1_KEY			0x7A
#define G_F2_KEY			0x78
#define G_F3_KEY			0x63
#define G_F4_KEY			0x76
#define G_F5_KEY			0x60
#define G_F6_KEY			0x61
#define G_F7_KEY			0x62
#define G_F8_KEY			0x64
#define G_F9_KEY			0x65
#define G_F10_KEY			0x6D
#define G_F11_KEY			0x67
#define G_F12_KEY			0x6F
#define G_F13_KEY			0x69
#define G_F14_KEY			0x6B
#define G_F15_KEY			0x71

#endif

#ifdef GWIN

#define G_LEFT_ARROW_KEY	VK_LEFT
#define G_RIGHT_ARROW_KEY	VK_RIGHT
#define G_UP_ARROW_KEY		VK_UP
#define G_DOWN_ARROW_KEY	VK_DOWN
#define G_PAGE_UP_KEY		VK_PRIOR
#define G_PAGE_DOWN_KEY		VK_NEXT
#define G_HOME_KEY			VK_HOME
#define G_END_KEY			VK_NEXT
#define G_CLEAR_KEY			VK_CLEAR
#define G_ESCAPE_KEY		VK_ESCAPE
#define G_ENTER_KEY			VK_RETURN
#define G_RETURN_KEY		VK_RETURN
#define G_FORWARD_DEL_KEY	VK_DELETE
#define G_DELETE_KEY        VK_BACK
#define G_HELP_KEY			VK_HELP
#define G_SHIFT_KEY			VK_SHIFT
#define G_CAPS_LOCK_KEY		VK_CAPITAL
#define G_CONTROL_KEY		VK_CONTROL
#define G_OPTION_KEY		VK_MENU
#define G_ALT_KEY			VK_MENU
#define G_COMMAND_KEY		VK_CONTROL
#define G_SPACE_KEY			VK_SPACE
#define G_TAB_KEY			VK_TAB
#define G_F1_KEY			VK_F1
#define G_F2_KEY			VK_F2
#define G_F3_KEY			VK_F3
#define G_F4_KEY			VK_F4
#define G_F5_KEY			VK_F5
#define G_F6_KEY			VK_F6
#define G_F7_KEY			VK_F7
#define G_F8_KEY			VK_F8
#define G_F9_KEY			VK_F9
#define G_F10_KEY			VK_F10
#define G_F11_KEY			VK_F11
#define G_F12_KEY			VK_F12
#define G_F13_KEY			VK_F13
#define G_F14_KEY			VK_F14
#define G_F15_KEY			VK_F15

#endif

/*** ASCII character codes... note that the C language supports built-in
     character codes for most of those that are not defined here (e.g.
     'a', 'A', ' ', '@', '^', '6', etc.) ***/

#define NUL	     0	/* Null */
#define SOH		 1	/* Start Of Heading */
#define STX		 2	/* Start Of Text */
#define ETX		 3	/* End Of Text */
#define EOT		 4  /* End Of Transmission */
#define BEL		 7  /* Bell */
#define BS		 8	/* Backspace */
#define TAB		 9	/* Tab */
#define LF		10	/* Line Feed */
#define VT		11	/* Vertical Tab */
#define FF		12	/* Form Feed */
#define CR		13	/* Carriage Return */
#define ESC		27	/* Escape */
#define FS		28	/* File Separator */
#define GS		29	/* Group Separator */
#define RS		30	/* Record Separator */
#define US		31	/* Unit Separator */

/*************************  G_KEY_UP_EVENT  **********************************

	GUILib sends your application a key-up event when the user releases a
	key that was formerly pressed.  The key-up event it sent to your
	application's window, and your event-handler receives that window's
	pointer in its (window) argument.  If your application has no active
	windows open, the key-up event is sent to its main event-handler, and the
	(window) parameter contains NULL.  The key code of the key that was released
    is passed to your event-handler in (param1).  

******************************************************************************/

#define G_KEY_UP_EVENT      3

/****************************  G_SUSPEND_EVENT  ******************************

	GUILib sends this event to your application when the user switches to
	another application, immediately before your application is switched
	to the background.
	
******************************************************************************/

#define G_SUSPEND_EVENT		4

/****************************  G_RESUME_EVENT  ******************************
	
******************************************************************************/

#define G_RESUME_EVENT		5

/****************************  G_MENU_EVENT  ******************************

	GUILib sends this event to your application when the user selects a
	command from one of the program's main menus.
	
	This event may be sent to your application's main event-handling
	function, or to a particular window's event-handling function.
	
	If the application has a window open which is qualified to recieve
	menu events, the event is sent to that window's event-handling
	function, and the function's (window) parameter recieves a pointer
	to that window.
	
	If the application has no windows open, the event is sent to the
	application's main event-handling function, and the function's
	(window) parameter recieves NULL.

	The argument (param1) contains the menu ID number of the menu from
	which the user selected the command.  The argument (param2) contains
	the command's number within the menu, i.e. 1 = first command, 2 =
	second command, etc.
	
	GUILib does nothing in response to the event.  However, if the event
	was directed to a particular window's event handling function, and the
	function returns TRUE, the event is then passed on to the application's
	main event-handling function.  This architecture allows you to keep
	the majority of your application's event-handling code in one place
	(the application's main event-handling function), yet still allow
	individual windows to recieve and if necessary override the application's
	default response to menu events.

	You can obtain a pointer to the window which should receives menu
	events using the function GGetMenuEventWindow().  You may also force
	menu events to be redirected to a particular window by calling the
	function GSetMenuEventWindow().  Please refer to the documentation
	for those functions for a more complete explanation of the qualifications
	a window must meet in order to recieve menu events.
		
******************************************************************************/

#define G_MENU_EVENT		6

/**************************  AppleEvents  ***********************************/

#define G_OPEN_FILE_EVENT	8
#define G_PRINT_FILE_EVENT	9

/***************************  G_EXIT_EVENT  ******************************

	This event happens when the user wants to quit your application.
	
	Because an exit event is not associated with any particular window,
	the (window) parameter to your event-handling procedure will contain
	NULL.  The other parameters (param1) and (param2) will contain zeros.
	
	An exit event can happen for any of several reasons.  On the Macintosh,
	the user may select the Finder's "Shut Down" command, which triggers a
	"Quit Application" AppleEvent for every running application.  In
	Windows, the user may close the application's main window, or select
	the Program Manager's "Exit Windows" command.
	
	Your application will also recieve a quit event when you call the
	function GExitMainLoop(), or in Windows when close your application's
	main window with the function GCloseWindow().  Your application should
	use this event to perform whatever actions it needs to before exiting
	(e.g. asking the user to save any open documents).
	
	If your application's event-handling procedure returns TRUE in response
	to this event, GUILib performs the appropriate sequence of actions
	which causes your program to drop out of its main event-handling loop.
	If your procedure returns FALSE, the program will continue to execute
	in its main event loop.

**************************************************************************/

#define G_EXIT_EVENT	10

/**************************  G_INITIAL_EVENT  ****************************

	This event indicates that a window has been created.
	
	The parameter (window) contains a pointer to the window;
	the arguments (param1) and (param2) contain zero.
	
	If you have assigned the window its own event-handling function,
	GUILib passes the window's initial event to that function, rather
	than to your application's main event-handling function.  This
	is the first event your window's event-handling function will
	receive, and the only time it will ever recieve this event.  You
	may use this event to perform any special window initializations.
	
	At the time your function receives this event, the window (and, if
	it is a dialog window) all of its associated controls have been
	created.  If the window is initially visible, it will be displayed
	on screen before it receieves this event.
		
***************************************************************************/

#define G_INITIAL_EVENT			11

/***************************  G_SIZE_EVENT  *******************************

	This event occurs whenever the user changes a window's size.  It also
	ocurrs when a window is first created, after the window's initial event.
	
	The argument (window) always contains a pointer to the window being
	resized.  The arguments (param1) and (param2) contain, respectively,
	the window's new horizontal and vertical dimensions, in pixels.
	
	SHOULD THIS EVENT HAPPEN BEFORE OR AFTER THE WINDOW GETS RESIZED?  I.E.,
	SHOULD THE APPLICATION BE ABLE TO PREVENT THE NATIVE OS FROM ACTUALLY
	RESETTING THE WINDOW'S SIZE TO THE DIMENSIONS THE USER PICKED?
	 
	In Windows, your window will be completely erased after it is resized,
	and will receive an update event requiring your application code to
	redraw the window.  On the Mac, this happens only if you tell GUILib
	to take the default response to the event.  If you do not, nothing gets
	erased, and the window will receive an update event only if the user
	makes it larger (thus creating an area which needs to be filled), not
	smaller.
	
***************************************************************************/

#define G_SIZE_EVENT		12

/****************************  G_ACTIVATE_EVENT ****************************

	This event ocurrs whenever a new window becomes the active window.  The
	active window is the one the user is currently interacting with, and its
	active status is usually indicated by a highlited title bar.  On the Mac,
	the active window is always the frontmost window.  (This is usually true
	in Windows as well, except in the case of modeless dialog windows.)
	
	Currently, GUILib notifies your application of this event after:
	
	-a new window is created
	-an existing window becomes active, either because the user has selected
	 it, or because the previously active window has been hidden, deleted, etc.
	-the user switches back to your application after switching to another.
	 
	The (window) argument to your event-handling procedure contains a pointer
	to the window being activated.  No other additional information is passed.
	
	The native window system automatically hilites a window's title bar,
	frame, etc., when it becomes active.  GUILib automatically redraws the
	window's size box if it has been enabled to do so for the window; see
	GShowSizeBox().

	If the window being activated is a document window, then that window will
	receive subsequent menu events; see GGetMenuEventWindow().  If GUILib is
	maintaining a window menu for your application, the window menu item
	corresponding to the window that is being activated will receive a check
	mark; see GSetWindowMenu() for more information.
	
********************************************************************************/

#define G_ACTIVATE_EVENT	13

/******************************  G_DEACTIVATE_EVENT ****************************

	This event ocurrs whenever a window loses its status as the active window.
	It is always immediately followed by an activate event, as the window
	replaces the currently-active window becomes active.
	
	IF YOU HIDE A WINDOW, IS IT DEACTIVATED?
	
	The (window) argument to your event-handling procedure contains a pointer to
	the window being deactivated.  No other additional information is passed.
	
	The native window system automatically un-hilites a window's title bar,
	frame, etc. when it is deactivated.  GUILib automatically redraws the
	window's size box if it has been enabled to do so for the window; see
	GShowSizeBox().

*******************************************************************************/

#define G_DEACTIVATE_EVENT	14

/*************************** G_UPDATE_EVENT ***********************************

	Your application receives this event code when part or all of a window's
	content area has become "invalid" and needs to be redrawn.  This happens when:
	
	-a window is first displayed on screen;
	-the user enlarges or shrinks a window;
	-the user uncovers a previously-hidden part of the window.
	
	You can also manually invalidate part or all of a window by calling
	GInvalidateWindow().  Note that update events are -not- generated when
	a part of the window outside of its content region (e.g. its title bar,
	frame, etc.) needs to be redrawn; this is handled automatically by the
	native window system.
	
	The (window) argument contains a pointer to the window requiring update.
	The arguments (param1) and (param2) contain zeros.
	
	When responding to update events, the operating system first sets the window's
	clipping region to the invalid region so that subsequent drawing will be
	restricted to that region only.  

	If automatic update region erasure has been enabled for the window, the
	window's invalid region is erased -before- your application's event-handling
	function is called.  Automatic update region erasure is enabled by default,
	and can be turned on or off with GEraseOnUpdates().
	
	GUILib then send an update event to your application by calling the window's
	event-handling procedure with the parameters described above.
	
	When your procedure has finished updating the window's content area, the
	window's controls are redrawn.  If its size box is visible, it is also redrawn.
	The size box is automatically visible for resizable windows on the Mac, and
	can be made visible or hidden for all windows with GShowSizeBox().

	Finally, the clipping region is removed so that drawing is no longer
	restricted to the invalid region, and the invalid region is cleared
	to prevent further generation of update events.
	
*******************************************************************************/

#define G_UPDATE_EVENT		15

/*****************************  G_MOUSE_DOWN_EVENT  *******************************

	This event happens when the user presses the mouse button in the content area
	of one of your application's windows.
	
	This event is always associated with a particular window, i.e. the (window)
	parameter is never NULL.  The other parameters describe where the mouse button
	was clicked; (param1) contains the X-coordinate and (param2) contains the Y-
	coordinate.  Both coordinates are given relative to the top left corner of the
	window specified in the (window) parameter.

	Mouse clicks outside a window's content area (e.g. in its title bar or size box)
	do -not- generate mouse-down events.  Mouse clicks inside enabled controls of the
	active window produce control events rather than mouse-down events (see below).

	In Windows, if a mouse button is pressed in an inactive window, Windows activates
	the window before sending a mouse-down event to it.  On the Macintosh, if the
	mouse is pressed in an inactive window, GUILib activates the window, but does
	-not- send a mouse-down event to it.  This reflects standard behavior on the Mac.

	Unlike key-down events, your application does not receive a continuous stream
	of mouse-down events if the user keeps the mouse button pressed.  Use the
    GIsMouseButtonDown() function to determine if the mouse button is still down.

	As part of your application's response to this event, you may choose to redirect
	subsequent mouse events to the window with the function GSetMouseEventWindow().
	If you do so, the window will recieve mouse-drag events (see below) whenever the
	mouse is moved while the mouse button is down.

************************************************************************************/

#define G_MOUSE_DOWN_EVENT	16

/******************************  G_MOUSE_UP_EVENT  *******************************

	This event happens when the user releases the mouse button in the content area
	of one of your application's windows.

	This event is always associated with a particular window, i.e. the (window)
	parameter is never NULL.  The other parameters describe where the mouse button
	was released; (param1) contains the X-coordinate and (param2) contains the Y-
	coordinate.  Both coordinates are given relative to the top left corner of the
	window specified in the (window) parameter.

	In Windows, a mouse-up event is always preceded by a mouse-down event.  On
	the Mac, a mouse-up event may occur without a preceding mouse-down, because
	GUILib does not send mouse-down events to inactive windows there.

	Normally, a mouse-up event is dispatched to whatever window the mouse was
	released within.  This means that your application may not see a mouse-up
	if the user releases the mouse outside all of its windows.  However, if you
	chose to redirect mouse events to the window where the initial mouse-down
	occurred (by calling GSetMouseEventWindow() as part of your response to it),
	a mouse-up event is guaranteed to go to the same window where the preceding
	mouse-down event occurred.  Additionally, this would probably be a good time
	to stop redirecting mouse events to that window (by calling the function
	GSetMouseEventWindow() with a NULL window parameter).

***********************************************************************************/

#define G_MOUSE_UP_EVENT	17

/******************************  G_DOUBLE_CLICK_EVENT  ****************************

	An application receives this event when the user double-clicks a mouse
	button in a window's content area.

	As with mouse-down and mouse-up events, the (window) argument contains a
	pointer to the window being clicked, and the (param1) and (param2) arguments
	describe the location of the point the user double-clicked on.  For
	double clicks which ocurr inside controls or outside a window's content
	area, the same comments as for mouse-down events apply.

	This event is normally preceded by a mouse-down event and a mouse-up event.
	It may be followed by a mouse-up event if the user releases the mouse within
	some window's content area.

************************************************************************************/

#define G_DOUBLE_CLICK_EVENT	18

/**************************  G_MOUSE_MOVED_EVENT  *******************************

	This event occurs when the mouse moves within the content area of an
	active, hilited window, and no mouse button is depressed.  (If any mouse
	button is depressed, the window recieves a mouse-drag event instead of a
	mouse-moved event.)
	
	The (window) parameter to your event-handling function contains a pointer
	to the window within whose content area the mouse is moving.  If the window
	is not hilited, or the mouse is moving outside its content area, the window
	will not recieve a mouse-moved event, unless you have specifically chosen
	to redirect all mouse input to that window.  See GSetMouseEventWindow().
	
	The other parameters to your event-handling function indicate the mouse's
	position in the window's local coordinate frame: (param1) contains the
	mouse's horizontal position and (param2) contains the window's vertical
	position.
	
	In Windows, portions of the window occupied by controls do not belong to the
	window; hence your application will not recieve mouse-moved events when the
	mouse moves over a control inside a window.
	
	GUILib does nothing in response to mouse-moved events; hence the value
	returned by your event-handling procedure is ignored.
		
*********************************************************************************/

#define G_MOUSE_MOVED_EVENT		19

/****************************  G_MOUSE_DRAG_EVENT  ******************************

	An active, hilited window receives this event when the mouse moves within
	the window's content area while a mouse button is depressed.

	The (window) parameter to your event-handling function contains a pointer
	to the window within whose content area the mouse is moving.  If the window
	is not hilited, or the mouse is moving outside its content area, the window
	will not recieve a mouse-moved event, unless you have specifically chosen
	to redirect all mouse input to that window.  See GSetMouseEventWindow().
	
	This event is always associated with a particular window: the (window)
	parameter to your event-handling function is never NULL.  The other parameters
	for this event describe the mouse's new position: (param1) contains the mouse's
	X-coordinate, and (param2) contains its Y-coordinate.  Both coordinates are
	given relative to the top left corner of the window given in the (window)
	parameter.

	In Windows, portions of the window occupied by controls do not belong to the
	window; hence your application will not recieve mouse-drag events when the
	mouse moves over a control inside a window.

	GUILib does nothing in response to mouse-moved events; hence the value
	returned by your event-handling procedure is ignored.

**********************************************************************************/

#define G_MOUSE_DRAG_EVENT		20

/****************************  G_CLOSE_EVENT  **********************************

	Your application receives this event code when the user closes a window.
	This can happen in any of the following ways:
	
	The (window) argument to your event-handling procedure contains a pointer
	to the window that the user wants closed.  The other arguments contain zero.
	
	When GUILib sends this event to your application, the window has not yet
	been hidden or destroyed; the user has merely signalled that he wants the
	window closed.  This happens when the user clicks the mouse in the window's
	close box (on the Mac) or selects "Close" from or double-clicks its system
	menu (in Windows).
	
	Your application can also cause this event to be sent to a window by calling
	the function GCloseWindow().  This function will call the window's event-
	handling procedure, and then perform the default response if the procedure
	returns TRUE.
	
	GUILib's default response to this event is to delete the window.  To prevent
	this response, your event-handling procedure should return FALSE rather than
	TRUE.  Your window's event-handling procedure is then responsible for
	destroying or hiding the window.
	
	In Microsoft Windows, the window being closed will not recieve a close event
	if it is the application's main window.  Instead, it will receive a quit
	event, which signals that the user wants to quit the application.  This is
	because a Windows program must have at least one window open to run, and
	deleting that window would cause the application to exit.
		 
********************************************************************************/

#define G_CLOSE_EVENT			21

/***************************  G_MAXIMIZE_EVENT  **********************************

	Your application receives this event code when the user maximizes a window.
	This happens when the user clicks and releases the mouse within a window's
	zoom out box (Macintosh) or Maximize button (Windows).
	
	The (window) argument to your event-handling procedure contains a pointer
	to the window that the user wants maximized.  The other arguments contain zero.
	
	If your application's event-handling procedure returns TRUE in response
	to this event, GUILib will perform the default response, which is to
	maximize the window.  You can prevent this by returning FALSE in response
	to this event.
	
	If your event-handling procedure returns TRUE, your window will recieve
	a following resize event as a result of the window's size being changed.
	A subsequent click in the maximize button or zoom box will generate a
	restore event, rather than another maximize event.
	
	Note that your application can also maximize a window by calling the
	function GMaximizeWindow().  If it does so, that window will recieve
	this event during the call to GMaximizeWindow().  Thus, you should never
	call GMaximizeWindow() while processing this event, or an infinite loop
	will occurr. 
			 
********************************************************************************/

#define G_MAXIMIZE_EVENT			22

/***************************  G_MINIMIZE_EVENT  **********************************

	Your application receives this event code when the user minimizes a window.
	This happens when the user clicks and releases the mouse within a window's
	minimize button (in Microsoft Windows); or clicks in a window's collapse box,
	or double-clicks in its title bar (on the Macintosh).
	
	The (window) argument to your event-handling procedure contains a pointer
	to the window that the user wants minimized.  The other arguments contain
	zero.
	
	If your application's event-handling procedure returns TRUE in response
	to this event, GUILib will perform the default response, which is to
	minimize or iconify the window.  You can prevent this by returning FALSE
	in response to this event.
	
	If your event-handling procedure returns TRUE, your window may recieve
	a subsequent resize event as a result of the window's size being changed.
	A subsequent click in the minimize button or zoom box will generate a
	restore event, rather than another minimize event.

	Note that your application can also minimize a window by calling the
	function GMinimizeWindow().  If it does so, that window will recieve
	this event during the call to GMinimizeWindow().  Thus, you should never
	call GMinimizeWindow() while processing this event, or an infinite loop
	will occurr. 
			 
********************************************************************************/

#define G_MINIMIZE_EVENT			23

/**************************  G_RESTORE_EVENT  **********************************

	Your application receives this event code when the user restores a window.
	This happens when the user clicks and releases the mouse within a window's
	zoom in (Macintosh) or restore button (Windows).
	
	The (window) argument to your event-handling procedure contains a pointer
	to the window that the user wants minimized.  The other arguments contain
	zero.
	
	If your application's event-handling procedure returns TRUE in response
	to this event, GUILib will perform the default response, which is to
	restore the window to its size before the preceding maximize or minimize
	operation.  You can prevent this by returning FALSE in response to this
	event.
	
	If your event-handling procedure returns TRUE, your window may recieve
	a subsequent resize event as a result of the window's size being changed.
	A subsequent click in the restore button or zoom box will generate a
	maximie/minimize event, rather than another restore event.

	Note that your application can also restore a window by calling the
	function GRestoreWindow().  If it does so, that window will recieve
	this event during the call to GRestoreWindow().  Thus, you should never
	call GRestoreWindow() while processing this event, or an infinite loop
	will occurr. 
			 
********************************************************************************/

#define G_RESTORE_EVENT			24

/*****************************  G_CONTROL_EVENT  *****************************

	Applications receive this event code when the user has clicked in one of
	a window's controls.  Also, certain keystrokes may be translated and sent
	to your application as control events.  For instance, GUILib always
	interprets the a press of the return or enter key in a modal dialog as
	a click the dialog's "OK" button.

	The (window) argument is a pointer to the window which contains the control
	that was clicked.  The (param1) argument contains the control's ID number.
	(You set each control's ID number when you create the control, or in the
	case of dialog window controls, in the dialog resource template.)
	
	For simple controls which have only one part (such as push buttons) the
	argument (param2) contains the value 1.  For multi-part controls, such as
	scrollbars, the (param2) argument contains a part code indicating which
	part of the control was clicked.  Part codes for the standard controls
	supported by GUILib are defined below.
	
	The native windowing system automatically hilites the control when the
	user clicks in it; GUILib does nothing in response to this event, except
	to inform you when it has happened.  For different controls, this happens
	somewhat differently:
	
	-for push buttons, check boxes, and radio buttons, GUILib sends the control
	 events to your application only after the user releases the mouse button
	 within the control.
	 
	-for scroll bars, GUILib sends your application control events continuously
	 if the mouse button is held down in the line-up, line-down, page-up, or
	 page-down parts of the scroll bar.  This lets your application do continuous
	 actions while the user presses the mouse-button there.  If the user clicks
	 in the thumb of the scroll bar, GUILib does not inform your application
	 until (s)he has released the mouse button, as with check boxes.
	
*******************************************************************************/

#define G_CONTROL_EVENT		30

/*** Scroll bar part codes ***/
	 
#define G_SCROLL_THUMB		1
#define G_SCROLL_PAGE_UP	2
#define G_SCROLL_PAGE_DOWN  3
#define G_SCROLL_LINE_UP	4
#define G_SCROLL_LINE_DOWN	5

#define G_TRACK_THUMB		6

/***************************  GMain  ****************************************

	This function is the main entry point to your application.

	int GMain ( short event, GWindowPtr window, long param1, long param2 );

	(event): event code, as #defined above
	(window): pointer to window assocaited with the event, if any.
	(param1): additional information associated with the event.
	(param2): additional information associated with the event.
	
	The GMain() function is not part of GUILib; rather, your application
	provides it.  When GUILib detects an event affecting your application,
	it calls your GMain() function to allow it to perform application-
	specific event handling for it.  Thus, unlike the main() of a traditional
	C program, your GMain() function may be called multiple times.

*****************************************************************************/

int GMain ( short, GWindowPtr, long, long );

/*****************************  GUILIB functions  ****************************

	Use these functions to interact with the graphical-interface code.
	The function protoypes here define the protocol which you should use
	to send commands and information to, and retrieve data from, the
	graphical interface elements.
	
	Some functions may in turn call your code.  For additional information
	regarding any particular function, refer to the source file in which
	that function is located.

*****************************************************************************/

/*** functions in Events.c ***/

/*****************************  GEnterMainLoop  ****************************

	This function begins your program's main event-handling loop.

	int GEnterMainLoop ( GEventProcPtr proc )

	(proc): address of application-defined event-handling procedure.

	This function lets your application begin an interactive session with
	the user by continuously checking for events and dispatching them to
	it when they occur.
	
	When a user causes an event, such as a mouse-click, GUILib informs your
	application by calling its event-handling procedure.  This is the procedure
	whose address you have passed to this function as (proc).  Your procedure
	is responsible for performing whatever application-specific actions should
	take place when the event occurs, and telling GUILib whether or not to take
	any additional default actions in response.  Your event-handling procedure
	must have the following form:

	int EventProc ( short event, GWindowPtr window, long param1, long param2 )

	(event): an integer code indicating what kind of event has ocurred.
	(window): pointer to the window (if any) which the event is related to.
	(param1): additional data related to event; meaning depends on event type.
	(param2): even more additional event-related data.

	The value your event-handling procedure returns should be TRUE or FALSE,
	depending on whether or not you want GUILib to take to appropriate default
    action in reponse to the event.

	To end the interactive session with the user, use the function GQuitEventLoop().

	Here's an example: a program which displays a window, then quits when the
	window is closed.  This very simple example illustrates the same fundamental
	structure that any such program must have:

	int GMain ( void )
	{
		GCreateWindow ( 1, "NullApp", -1, -1, -1, -1, TRUE, G_APPLICATION_WINDOW, 0, NULL );
		return ( GEnterMainLoop ( DoEvent ) );
	}

	The first line of GMain() creates a window for the program.  The call to
	GEnterEventLoop() passes control back to GUILib, which informs us when events
	happen by calling our event-handling procedure, DoEvent():

	int DoEvent ( short event, GWindowPtr window, long param1, long param2 )
	{
		int respond = TRUE;
		
		switch ( event )
		{
			case G_CLOSE_EVENT:
				GExitMainLoop ( 0 );
				break;
		}
		
		return ( respond );
	}

	Although GUILib calls our function every time any event related to our
	application occurs, we pay special attention to only one kind of event:
	the G_CLOSE_EVENT.  This event happens when the user clicks in a window's
	close box.  When this happens, we call GExitMainLoop() to tell GUILib
	that the interactive session should end.
	
	Note that the program not explicitly destroy the window before exiting the
	program.  GUILib destroys the window automatically as part of its default
	response to the window-closing event (see the documentation for event types).
	For the program to override GUILib's default response, its event-handling
	procedure should return FALSE.  The application would then have to delete
	the window explicitly when the window is closed:
	
	case G_CLOSE_EVENT:
		GDeleteWindow ( window );
		GExitMainLoop ( 0 );
		respond = FALSE;
	
	This mechanism allows the application to perform its own sequence of
	actions in addition to or instead of GUILib's default response when
	events occur.
	
	Typically, your application will have only one main event loop.  When
	it exits the event loop, the program should terminate.  Also, you should
	not nest calls to GEnterMainLoop() (i.e., call it while your program is
	already executing inside an event-handling loop.)
	
*****************************************************************************/

typedef int (*GEventProcPtr) ( short, GWindowPtr, long, long );

int GEnterMainLoop ( GEventProcPtr );

/*********************  GExitMainLoop  *************************************

	This function sends a quit event to your application and causes program
	execution to drop out of its main event loop.
	
	void GExitMainLoop ( int result )

	(result): integer result code to return to caller of GEnterMainLoop().

	When your application calls GExitMainLoop(), GUILib causes your program
	to exit its main event-handling loop.  Before this happens, your
	application's event-handling procedure recieves a quit event, which will
	allow it to "change its mind" by returning FALSE to prevent event-loop
	execution from terminating; see the quit event documentation.

	The value you pass to GExitMainLoop() will be returned by GEnterMainLoop()
	to its caller.  See GEnterMainLoop() for an example of usage.
	
	Because this function causes a quit event to be sent to your application,
	you should never call it in response to a quit event.  Doing so would
	cause an infinite recursive loop!
	
	On the Macintosh, calling GExitMainLoop() simply sets in internal flag
	which indicates that event loop execution should end.
	
	In Microsoft Windows, GUILib exits your application's main event loop by
	deleting its main window.  This means that the main window will no longer
	exist when program execution returns from GMainWindow().
	
*****************************************************************************/

void GExitMainLoop ( int );

/******************************  GUILibInitialize  **************************

	Performs all necessary GUILib initializations.

	int GUILibInitialize ( GHandle instance )
	
	(intance): Windows program instance handle as given to WinMain().
	
	The function returns TRUE if successful or FALSE on failure.  You should
	check for failure and exit if it occurs.
	
	Note that you only need to call this function if you are writing your
	own main() or WinMain() function.  If you are using GUILib's private
	main function, you should not call GUILibInitialize() because GUILib
	automatically does so before calling your GMain().
	
	On the Mac, the (instance) argument is ignored.
	
*****************************************************************************/

int GUILibInitialize ( GHandle );

/******************************  GUILibCleanUp  **************************

	Performs all necessary GUILib cleanup procedures.

	void GUILibCleanUp ( void )

	If you are writing your own main() or WinMain() function, this is the
	last routine your application should call before it exits.  If you are
	using GUILib's private main function, you should not call GUILibCleanUp()
	because GUILib automatically does so before exiting.

*****************************************************************************/

void GUILibCleanUp ( void );

/**************************  GGetInstance  **********************************

	Returns a copy of the application's instance handle.

	GInstancePtr	GGetInstance ( void )

	The application instance handle is required by many Microsoft Windows
	API calls.  It has no equivalent on the Macintosh, so this function
	always returns NULL on that platform.

*****************************************************************************/

GInstancePtr GGetInstance ( void );

/***  GGetMenuEventWindow  *************************************************

	Returns a pointer to the window to which menu events are currently
	being directed.

	GWindowPtr GGetMenuEventWindow ( void )

	If an application has no windows open or active, menu events are
	sent to the application's main event-handling function, and this
	function will return NULL.
	
	If an application has a qualifying active window open, menu events
	are sent to that window's event-handling function, and this function
	returns a pointer to that window.
	
	On the Macintosh, the menu event window is the frontmost visible window
	in the application's document-window layer, unless there is a modal
	dialog window open; if so, the menu event window is the modal dialog
	window.  The menu event window may be a modless dialog window in the
	application's document window layer.  Floating windows never recieve
	menu events.
	
	In Windows, the menu event window is the frontmost document window.
	Floating windows, modal dialogs, and modeless dialogs are never the
	menu event window.
	
	You can override the natural menu event window selection and force
	menu events to be redirected to a particular window by calling
	GSetMenuEventWindow().  Please note, however, that if you do so,
	the menu event window will be change back to the one described by the
	rules above as soon as the user selects, opens, or closes another window.
	
******************************************************************************/

GWindowPtr GGetMenuEventWindow ( void );

/***  GSetMenuEventWindow  ***************************************************

	Sets the window to which all menu events should be directed.

	void GSetMenuEventWindow ( GWindowPtr window )

	(window): window to receive menu events, or NULL.

	If you pass NULL as the argument to this function, menu events are
	sent to the application's main event-handling function rather than to
	any particular window.  If you pass a valid window pointer to this
	function, menu events are sent to that window's event-handling function,
	rather than to the application's event-handling function.
	
	Please note, however, that if soon as the user opens, closes, or selects
	another window, GUILib may switch the menu event window to that window
	automatically.  See GGetMenuEventWindow() for details.

******************************************************************************/

void GSetMenuEventWindow ( GWindowPtr );

/*************************  GGetMouseEventWindow  ***************************

	Determines which window (if any) mouse events will be redirected to.

	GWindowPtr GGetMouseEventWindow ( void )

	If mouse events are being redirected to a particular window, this
	function returns a pointer to that window.  If mouse events are
	being distributed normally (i.e. to the window in which they ocurr),
    the function returns NULL.

******************************************************************************/

GWindowPtr GGetMouseEventWindow ( void );

/**************************  GSetMouseEventWindow  **************************

	Sets the window to which all mouse events are directed.

	void GSetMouseEventWindow ( GWindowPtr window )

	(window): window to receive mouse events, or NULL.

	Normally, mouse events are sent to the window in which they occurred.
	You can redirect all mouse events to a particular window by passing
	a pointer to that window to this function.  To contine distributing
	mouse events normally, pass NULL.

******************************************************************************/

void GSetMouseEventWindow ( GWindowPtr );

/*************************  GGetMouseEventWindow  ***************************

	Determines which window (if any) mouse events will be redirected to.

	GWindowPtr GGetMouseEventWindow ( void )

	If mouse events are being redirected to a particular window, this
	function returns a pointer to that window.  If mouse events are
	being distributed normally (i.e. to the window in which they ocurr),
    the function returns NULL.

******************************************************************************/

GWindowPtr GGetMouseEventWindow ( void );

/**************************  GSetKeyEventWindow  **************************

	Sets the window to which all keyboard events are directed.

	void GSetKeyEventWindow ( GWindowPtr window )

	(window): window to receive keyboard events, or NULL.

	Ordinarily, the window which recieves keyboard events is the active,
	hilited window selected by the user.  You can use this function to direct
	keyboard events to a different window.

	Alternatively, keyboard events may be captured by a particular control
	within the active window (e.g. an editable-text control); in that case,
	the window will recieve control events (rather than key up/key down events)
	when the keyboard is clicked.  You can restore keyboard events to the
	window itself by calling this function.  See GGetKeyEventControl() or
	GSetKeyEventControl().

	You can prevent keyboard events from being sent to any window or control
	by calling the function with NULL as the (window) parameter.
	
	To obtain a pointer to the window which is currently receiving keyboard
	input, use the function GGetKeyEventWindow().
	
******************************************************************************/

void GSetKeyEventWindow ( GWindowPtr );

/*************************  GGetKeyEventWindow  ***************************

	Determines which window (if any) keyboard events are being directed to.

	GWindowPtr GGetKeyEventWindow ( void )

	If mouse events are being directed to a particular window, this
	function returns a pointer to that window.
	
	If keyboard events are being directed to a control within a particular
	window, this function returns a pointer to the window containing that
	control.

	If keyboard events are not being directed to any window, this function
	return NULL.
	
******************************************************************************/

GWindowPtr GGetKeyEventWindow ( void );

/***  GSetKeyEventControl  *************************************************

	Specifies which control in a particular window will recieve keyboard
	events.
	
	void GSetKeyEventControl ( GControlPtr control )
	
	(control): pointer to control which should recieve keyboard events.
	
	Only one control may recieve keyboard input at a time.  You can use this
	function to specify which one.  When a control has keyboard input, the
	window containing the control recieves control events rather than key-down
	events when the keyboard is pressed.
	
	After you call this function, GGetKeyEventWindow() will return a pointer
	to the window containing the control.
	
	The control parameter (control) must be a valid pointer; it may not be
	NULL.  If (control) is NULL, this function will have no effect.
	
	If you wish to direct keyboard input to a window containing a particular
	control, rather than to the control itself, pass a pointer to the window
	to the function GSetKeyEventWindow().
	
	Some controls (e.g. push-buttons on the Mac) may not support keyboard input.
	In that case, calling this function will direct keyboard input to the
	window containing the control, but the window will recieve key-up/key
	down events rather than control events when the keyboard is pressed.

**********************************************************************************/

void GSetKeyEventControl ( GControlPtr );

/**************************  GGetKeyEventControl  *********************************

	Returns a pointer to the control which is currently recieving keyboard input.
	
	GControlPtr GGetKeyEventControl ( void )
	
	If no control is currently recieving keyboard input, the function returns NULL.
	
***********************************************************************************/

GControlPtr GGetKeyEventControl ( void );

/**************************  GGetTickCount  *********************************

	Returns the number of ticks of the system clock since system startup.

	long GGetTickCount ( void )
	
	The system clock's tick count is returned as a long integer; to convert
	the result from clock ticks to seconds, divide it by the constant
	G_TICKS_PER_SECOND, which is #defined below.  Note that the tick count
	may overflow the limits of a long integer and reset to zero after the
	system has been running conmtinuously for a very long period of time
	(usually weeks).
	
****************************************************************************/

long GGetTickCount ( void );

/**************************  GGetTickCount  *********************************

	Causes the system to wait for a specified number of clock ticks.

	void GWait ( long ticks )
	
	(ticks): duration of pause, in system clock ticks.

	This function causes a "multitasking-unfriendly" pause on cooperatively-
	multitasked systems.  Not only does it cause the calling application to
	stop executing for the specified number of clock ticks, but it also
	prevents the operating system and any other background applications
	from operating while the application is paused.

	To specify the duration of the wait in seconds rather than clock ticks,
	multiply the number of seconds by the constant G_TICKS_PER_SECOND, which
	is #defined below.
		
****************************************************************************/

void GWait ( long );

#ifdef GMAC
#define G_TICKS_PER_SECOND		60
#endif

#ifdef GWIN
#define G_TICKS_PER_SECOND		1000
#endif

/***************************  GDoBackgroundTask  ****************************

	Allows background processes to execute while your application is
	performing a time-consuming task, and lets the user switch to
	another application.
	
	int GDoBackgroundTask ( long interval )
	
	(interval): number of clock ticks between background-task-handling.
	
	This function returns TRUE.
	
	Your application should call this function periodically whenever it
	performs time-consuming operations that would monopolize CPU usage
	or otherwise prevent the user from doing work in another application,
	such as when reading an extremely long file or performing a lengthy
	series of computations.  This function will allow background processes
	time to execute, and in addition will let the user switch to another
	application (and, of course, back to your own).  If your application
	continues calling this function while it is executing in the background,
	the user will also have time to work in the foreground application.

	To ensure smooth operation, your application should call this function
	at least several times a second while its performing its time-consuming
	operation.  However, because background processes may themselves consume
	significant amounta of time, the total amount of time required for your
	application to complete its operation may be significantly increased if
	you call this function too often.  To prevent this, the (interval)
	parameter is provided in order to let you specify it minimum number of
	clock ticks that may elapse between subseqent calls of this function
	before other processes are in fact allowed to execute.  A value for this
	parameter of G_TICKS_PER_SECOND / 5 should be adequate under most
	circumstances.
	
	Because this function lets the user switch to another application,
	your application may recieve suspend or resume events.  If so, this
	function calls your application's event-handling routine in order to
	process them correctly.  Hence, your suspend/resume event handling
	procedure should never call this function.
	
	Similarly, while the user is working in another application, your
	application may need to have one of its window updated (the user may
	move a foreground application window, uncovering part of yours.)
	If so, GDoBackgroundTask() calls your application's event handler
	in order to update the window.  Therefore, your application's window
	update event handling procedure should never call GDoBackgroundTask(),
	or you may encounter an infinitely recursing loop.  
	
	On the Macintosh, this function beeps at the user if it detects a
	mouse-down event in any of your application's windows.  However, it
	does let the user perform a menu selection if the user clicks in the
	menu bar (in order to switch to another application, via the Apple or
	application menus).  Your application should therefore disable its
	menus before beginning its time-consuming process, and restore them
	afterwards.  You can easily accomplish this by bracketing your
	procedure with calls to GShowModalDialog() and GHideModalDialog().
	
	Finally, on the Macintosh, this function also animates the watch cursor
	while your application is in the foreground.  If the user switches to
	another application, it restores the standard arrow cursor.

*****************************************************************************/

int GDoBackgroundTask ( long );

/******************************  GNewPtr  ***********************************

	Allocates a block of fixed memory.

	GPtr GNewPtr ( long size )

	(size): size of memory block to allocate, in bytes.

	The function returns a pointer to the memory, or NULL on failure.
	You may have to cast the pointer to the appropriate type to use it:

	struct foo *fooPtr;

	fooPtr = (struct foo *) GNewPtr ( sizeof ( struct foo ) );

	When you are done with the memory, release it using GFreePtr():

    GFreePtr ( (Ptr) fooPtr );

	Implementation notes:

	On the Macintosh, this function simply calls NewPtr().  It allocates
	memory from the application's heap zone, and may therefore allocate a
	fixed, contiguous block of memory of any size up to the application's
	memory partition size.

	In Windows, this function calls GlobalAllocPtr().  It allocate memory from
	Windows' global heap.  Under 16-bit windows, you may not allocate more
	than 64K of memory in one contiguous block.  This limit is removed under
	32-bit Windows.  (Huge ptrs?)

****************************************************************************/

GPtr GNewPtr ( long );

/******************************  GNewHandle  ********************************

	Allocates a block of moveable memory.

	GHandle GNewHandle ( long size )

	(size): size of memory block to allocate, in bytes.

	The function returns a handle to the memory, or NULL on failure.

	Handles are a bit more complicated than pointers, because the operating
	system may move the memory that the handle addresses.  Do not attempt
	to access the memory directly.  Instead, use GLockHandle() to fix the
	memory block and return a pointer to it.  When you are done, use
	GUnlockHandle() to let the system move the memory block again:

	struct foo *fooPtr;
	GHandle fooHandle;

	fooHandle = GNewHandle ( sizeof ( struct foo ) );
	fooPtr = (struct foo *) GLockHandle ( fooHandle );

	...do something with the pointer to the struct foo...

	GUnlockHandle ( fooHandle );
	GFreeHandle ( fooHandle );

	With all this extra overhead, why use handles at all?  The reason is that,
	unlike fixed memory blocks, the system can shuffle moveable blocks around
	when it needs more memory.  For example, if an application has 4 MB
	available, but there is just a 1K fixed block stuck in the middle of it,
	then the largest memory block the system can allocate is only 2 MB.  On the
	other hand, if the memory is moveable, the system can move that 1K block out
	of the way if the application asks for more memory.  In general, the rule
	of thumb to follow is this: if you need to use only one block of memory
	over the lifespan of your program, or if you only one block at a time, use
	a pointer.  If you need multiple blocks of memory whose lifetimes will
	overlap, use handles.

	Implementation notes:

	On the Macintosh, this function simply calls NewHandle().  The handle
	returned is a pointer to a "Master Pointer" which points directly to the
	memorory block allocated.  Thus a pointer to the memory can be obtained
	by simply dereferencing the handle: *fooHandle points directly to the
	struct foo, in the above example.  However, doing this is bad because
	1) it makes your code unportable to windows, and 2) it doesn't lock the
	memory in place, which will cause havoc if the system tries to move it
	while you are using it.

	In Windows, this function calls GlobalAlloc() and allocates moveable memory
	from Windows' global heap.  The same size restrictions apply under 16-bit
	Windows as with GNewPtr().

****************************************************************************/

GHandle	GNewHandle ( long );

/***************************  GLockHandle  *********************************

	Locks a block of moveable memory in place and returns a pointer to it.

	GPtr GLockHandle ( GHandle handle )

	(handle): handle to block of moveable memory.

	The function returns NULL on failure.

	Use this function before attempting to use a block of moveable memory.
	You may need to cast the pointer it returns to the appropriate type;
	see GNewHandle() for an example of usage.  Always call GUnlockHandle()
    to unlock the memory when you are done with it.

******************************************************************************/

GPtr GLockHandle ( GHandle );

/***************************  GUnlockHandle  *********************************

	Unlocks a block of moveable memory.

	void GUnlockHandle ( GHandle handle )

	(handle): handle to block of moveable memory.

	Use this when you are done using a block of moveable memory.  The system
	may move the memory after you call this function, which will invalidate
	any pointer to the memory block you may have obtained with GLockHandle().

	Never call GUnlockHandle() without having first locked the memory by
	calling GLockHandle().

******************************************************************************/

void GUnlockHandle ( GHandle );

/***************************  GFreeHandle  *********************************

	Deallocates and destroys a block of moveable memory.

	void GUnlockHandle ( GHandle handle )

	(handle): handle to block of moveable memory.

	After you call this function, the contents of the memory block will be
	destroyed.  Do not call this function with anything other than a handle
    to a moveable memory block allocated with GNewHandle()!!!

******************************************************************************/

void GFreeHandle ( GHandle );

/*******************************  GFreePtr  *********************************

	Deallocates and destroys a block of fixed memory.

	void GFreePtr ( GPtr ptr )

	(handle): handle to block of moveable memory.

	After you call this function, the contents of the memory block will be
	destroyed.  You may need to cast the pointer to your memory block to
	the generic pointer type (GPtr); see GNewPtr() for an example of usage.

	Never use this function with anything other than a pointer to a block of
	fixed memory allocated with GNewPtr().  Do not use it to free memory
	on the stack, or a pointer to a block of moveable memory!!!

******************************************************************************/

void GFreePtr ( void * );

/****************************  GNewMatrix  ********************************

	Allocates memory for a 2D array.

	void **GNewMatrix ( long size, long rows, long cols )

	(size): size of individual elements in array, in bytes
	(rows): number of rows in array
	(cols): number of columns in array

	This function is an attempt to address one of the major shortcomings of
	the C language: the inability to dyamically allocate memory for 2D arrays.
	Although this is a higher-level function than any of the above memory-
	allocation routines, I placed it in GUILib rather than, say, an independent
	matrix library for two reasons: 1) here it can take advantage of native
	memory-allocation routines without compromising code portability, and
	2) it's damn useful to have around.

	The function allocates an array of pointers to the matrix rows.  For each
	row pointer, it allocates enough memory to hold one row of objects of the
	size you have given in the argument (size).  It returns the array of pointers
	to the matrix rows (or NULL on failure.)

	You will have to cast the result to the appropriate data type to address
	individual elements correctly.  For example, if you want a 10-row-by-7-
	column matrix of short integers, you would write:

	short **matrix;

	matrix = (short **) GNewMatrix ( sizeof ( short ), 10, 7 );

	To access any individual element in the matrix, you would use the notation

	element = matrix[i][j];

	where (i,j) are the row and column of the element you're after.

*****************************************************************************/

void **GNewMatrix ( long, long, long );

/****************************  GFreeMatrix  ********************************

	Frees memory for a 2D array allocated with GNewMatrix().

	void GFreeMatrix ( void **matrix )

	(matrix): array of pointers to matrix rows.

	This function is guaranteed to crash your machine hard and heavily if
	you use it with anything other than a matrix created by GNewMatrix().

*****************************************************************************/

void GFreeMatrix ( void ** );

/**************************  GGetFreeMemory  ********************************

	Returns the number of bytes of free physical memory.
	
	long GGetFreeMemory ( void )
	
	On the Macintosh, this function returns the number of free bytes
	in the application's heap zone.
	
	In Windows, this function returns the number of free bytes of physical
	memory (not virtual memory) in the global heap.

*****************************************************************************/

long GGetFreeMemory ( void );

/************************  FUNCTIONS IN GEOMETRY.C **************************/

/*******************************  GRectangle  *******************************

	Returns a pointer to a rectangle containing user-defined coordinates.
	
	GRectPtr GRectangle ( short left, short top, short right, short bottom )
	
	(left): coordinate of left side of rectangle
	(top): coordinate of top side of rectangle
	(right): coordinate of right side of rectangle
	(bottom): coordinate of bottom side of rectangle
	
	The function returns a pointer to a static GRect structure; its contents
	are overwritten every time you call this function.  You may wish to make
	a local copy, i.e.
	
	GRect rect = *GRectangle ( 10, 20, 30, 40 );
	
	You can also use the GSetRect() function to store a set of coordinates
	into a rectangle structure.
		
******************************************************************************/

GRectPtr GRectangle ( short, short, short, short );

/*******************************  GSetRect  ***********************************

	Stores coordinates into a rectangle structure.
	 
	GRectPtr GSetRect ( GRectPtr rect, short left, short top, short right,
	         short bottom )
	
	(rect): pointer to rectangle structure to receive coordinates.
	(left): position of left size of rectangle.
	(top): position of right side of rectangle.
	(right): position of right side of rectangle.
	(bottom): position of bottom side of rectangle.
	
	The function returns a pointer to the given rectangle.
	
	Both Mac and Windows define a rectangle data type, which this header file
	typedefs as GRect.  However, to maintain portability, you should not access
	that data type's fields directly; instead, use the accessor/modifier functions
	provided here.  Here's an example of how NOT do initialize a rectangle
	structure:
		
	GRect rect = { 0, 0, 400, 240 };
	
	This will generate a diferent rectangle on Windows than the Mac, because
	the ordering of its (left,top,right,bottom) fields is different there.
	Instead, the correct way to do it is:
	
	GSetRect ( &rect, 0, 0, 400, 240 );
	
	This ensures that the same rectangle will be generated on both platforms
	(as well as any future platforms that GUILib might be ported to).  Then,
	to access the fields of the rectangle you should use the accessor macros
	provided below.

	A note on rectangle coordinates: A rectangle's interior is considered
	to extend up to but not include its right and bottom sides.  For example,
	a rectangle whose (left,top,right,bottom) side coordinates are (4,3,8,6)
	would appear as follows:
	
	123456789
	2
	3  XXXX
	4  X  X
	5  XXXX
	6
	7
	
	and thus the rectangle's width (right minus left) is 4 units, and its
	height (bottom minus top) is 3 units.
	
	This is undeniably counter-intuitive, but it's also the way both the Mac
	and Windows treat their own rectangle definitions.  Given that, it is
	better to understand and work with the existing definitions than to
	try to fight them.
	
******************************************************************************/
	
GRectPtr GSetRect ( GRectPtr, short, short, short, short );

#define GLeft(rect)		((rect)->left)
#define GTop(rect)		((rect)->top)
#define GRight(rect)	((rect)->right)
#define GBottom(rect)	((rect)->bottom)
#define GWidth(rect)	((rect)->right - (rect)->left)
#define GHeight(rect)	((rect)->bottom - (rect)->top)

/***********************  GMoveRect  ****************************************

	Moves a rectangle without changing its size.
	
	GRectPtr GMoveRect ( GRectPtr rect, short right, short down )
	
	(rect): pointer to rectangle to move.
	(right): number of units by which to move rectangle right.
	(down): number of units by which to move rectangle down.
	
	The function returns a pointer to the given rectangle.  On return,
	the rectangle's top and bottom coordinates will be increaed by (down),
	and its left and right coordinates will be increased by (right).  To
	move a rectangle up or left, pass negative values for (down) and
	(right), respectively.
	
*****************************************************************************/

GRectPtr GMoveRect ( GRectPtr, short, short );

/***********************  GSizeRect  ****************************************

	Resizes a rectangle about its top left corner.
	
	GRectPtr GSizeRect ( GRectPtr rect, short width, short height )
	
	(rect): pointer to rectangle to move.
	(width): desired with for rectangle.
	(height): desired height for rectangle.
	
	The function returns a pointer to the given rectangle.  On return,
	the rectangle's top left coordinates will be unchanged.  Its bottom
	coordinate will equal its top coordinate plus (height); its right
	coordinate will equal its left coordinate plus (width).
	
*****************************************************************************/

GRectPtr GSizeRect ( GRectPtr, short, short );

/***********************  GClipRect  ****************************************

	Clips one rectangle to the boundaries of another..
	
	GRectPtr GClipRect ( GRectPtr rect, GRectPtr bounds )
	
	(rect): pointer to rectangle to clip.
	(bounds): pointer to rectangle containing boundaries to clip to.
	
	The function returns a pointer to the given rectangle (rect).  On return,
	the rectangle's left, top, right, and bottom coordinates will be clipped
	to those of the boundary rectangle if they exceeded it on input.
	
*****************************************************************************/

GRectPtr GClipRect ( GRectPtr, GRectPtr );

/***********************  GInsetRect  ****************************************

	Shrinks or expands a rectangle about its center by a specified amount.
	
	GRectPtr GInsetRect ( GRectPtr rect, short width, short height )
	
	(rect): pointer to rectangle to shrink or expand.
	(width): amount by which to shrink/expand vertical sides of the rectangle.
	(height): amount by which to string/expand horizontal sides of the rectangle.
	
	The function returns a pointer to the given rectangle.  On return, the
	rectangle's top and bottom coordinates will be decreased and increased,
	respectively, by (height), and its left and right coordinates will be
	decreased and increased, respectively, by (width).  To expand the
	rectangle rather than shrink it, give negative values for (width)
	and (height).
	
******************************************************************************/

GRectPtr GInsetRect ( GRectPtr, short, short );

/***********************  GCenterRect  **************************************

	Centers one rectangle symmetrically inside another.
	
	GRectPtr GCenterRect ( GRectPtr rect, GRectPtr frame )
	
	(rect): pointer to first rectangle to center
	(frame): pointer to second rectangle in which to center first rectangle.
	
	The function returns a pointer to the first rectangle.  On return,
	the top, left, bottom, and right sides of the first rectangle will be
	adjusted so that it is centered symmetrically inside the second
	rectangle (frome).  The first rectangle's overall width and height
	will remain unchanged.
	
*****************************************************************************/

GRectPtr GCenterRect ( GRectPtr, GRectPtr );

/***************************  GIsPointInRect  ******************************

	Determines whether or not a point is inside a rectangle.
	
	int GIsPointInRect ( short x, short y, GRectPtr rect )

	The function return TRUE if the point is inside the rectangle and FALSE
	otherwise.  (If the point is "on the line" it is considered to be inside.)
	 
*****************************************************************************/

int GIsPointInRect ( short, short, GRectPtr );

/**************************  FUNCTIONS IN WINDOW.C *************************/

/***  GCreateWindow  ********************************************************

	This function creates a new application window.

	GWindowPtr GCreateWindow ( short class, char *title, short left, short top,
			   short width, short height, int visible, char type, long data, GEventProcPtr proc );

	(class): number for window's class and icon/cursor/menu resources.
	(title): C-style string containing title for window.
	(left): desired coordinate for left side of window.
	(top): desired coordinate for top side of window.
	(width): desired width for window.
	(height): desired height for window.
	(visible): if TRUE, window will be initially visible; if FALSE, hidden.
	(type): window type code, as #defined below.
	(data): application-defined 32-bit data value to store in window.
	(proc): application-defined event-handling procedure for window.

	The function returns a pointer to the newly-created window, or NULL on
	failure.  Several aspects of its behavior are platform-dependent.
	
	In Windows, your application may create only one window unless you have
	enabled multiple-window support by creating a frame window beforehand
	(see GCreateFrameWindow(), below.)
	
	The class number you provide for the window (class) does not have to be
	unique, but it should be non-zero.  It can be obtained later with
	GGetWindowClass().

	In Windows, the class number is used for the window's class name.  Also, any
	ICON and CURSOR resources with that ID number will be used as the window's
	custom icon and cursor.
	
	On the Mac, the window's position, size, and other properties are
	obtained from a 'WIND' resource with the specified class number; the other
	parameters you give are ignored if the specified 'WIND' resource is not found.
	
	On the Mac, the coordinates for the window's top left corner are always
	relative to the top left corner of the screen.  The coordinates and
	dimensions you give refer to the window's content area rather than
	its frame; the window frame may be above, to the left of, and slightly
	larger than the values you give.
	
	In Windows, the coordinates for the window's top left corner are relative
	to the top left corner of the screen unless multiple-window support has been
	enabled, in which case they are relative to the top left corner of the
	application's frame window's content area.  The coordinates and dimensions
	you give refer to the window's frame; its content area may be below, to the
	right of, and slightly smaller than the values you give.
	
	The (visible) parameter indicates whether or not the window should be
	made initially visible.  If it is TRUE, the window will be visible as
	soon as it is created; if FALSE, the window will be hidden until you
	show it explicitly by calling GShowWindow().
	
	The argument (type) defines the window's appearance and properties, e.g.
	whether it has a title bar, is maximizable, etc, and its initial visibility
	state.  The window type codes recognized by GUILib are #defined below.
	You may also combine type codes with a bitwise OR.  For example, the
	combined type code
	
	G_CLOSEABLE_WINDOW | G_MAXIMIZABLE_WINDOW
	
	would produce a window with a title bar, close box (or system menu), and
	zoom box (or maximize box), but no minimize box or size box (or thick frame).

	On the Macintosh, a window's resize box is considered part of its content
	area, rather than its frame, and thus is NOT automatically drawn by the Mac's
	window wanager.  However, if you create a resizable Mac window, GUILib
	will maintain the responsibility of drawing its size box so your application
	doesn't have to.  You may suppress this behavior; see GShowSizeBox().
	
	The argument (data) is intended for you to store whatever other information
	you might want along with the window.  You might very well want to store a
	pointer or handle (which are both just long integers) to some other
	information here.  If you do this, watch out for memory leaks: since the
	native operating system doesn't know or care what value you store here, it
	will not automatically release the memory when the window is deleted.  You
	can obtain and modify a window's application-defined data value with the
	functions GGetWindowData() and GSetWindowData().

	The argument (proc) is a pointer to your own event-handling procedure for
	the window.  If you pass a valid pointer here, GUILib calls this procedure
	when events related to the window occur, rather than calling the application's
	general event-handling procedure.  If you don't want to define an specific
	event-handling procedure for the window, and want to use the application's
	event-handling procedure instead, pass NULL for this argument.

	If your application is maintaining a window menu (see GSetWindowMenu()), and
	the window you are creating is a document (i.e. non-dialog, non-floating)
	window, then a new menu item with the window's title will be added to the window
	menu.  If the window is being created invisibily, however, this menu item will
	not be added until you explicity show the window (see GShowWindow().)
	
************************************************************************************/

GWindowPtr GCreateWindow ( short, char *, short, short, short, short, int, char, long, GEventProcPtr );

/*** Window properties ***/

#define G_MOVEABLE				0x01	/* title bar */
#define G_CLOSEABLE				0x02	/* close box (Mac) or system menu (Win) */
#define G_SIZEABLE				0x04	/* size box (Max) or thick frame (Win) */
#define G_MAXIMIZABLE			0x08	/* zoom box (Mac) or maximize box (Win) */
#define G_MINIMIZABLE			0x10	/* minimize box (Win) */
#define G_FLOATING				0x20	/* floating window (Mac) or topmost (Win) */

/*****************************  Window types  **********************************/

#define G_FIXED_WINDOW			0x00	/* no title bar; not resizable */
#define G_MOVEABLE_WINDOW		0x01	/* title bar */
#define G_CLOSEABLE_WINDOW		0x03	/* title bar and close box or system menu */
#define G_SIZEABLE_WINDOW		0x05	/* title bar and size box or thick frame */
#define G_MAXIMIZABLE_WINDOW	0x09	/* title bar and zoom box (Mac) or maximize box (Win) */
#define G_MINIMIZABLE_WINDOW	0x19	/* title bar and mimimize box (Win) */
#define G_APPLICATION_WINDOW	0x1F	/* title bar and close box, size box, zoom box (Mac) or
										   title bar and system menu, thick frame, maximize box, minimize box (Win) */

/***************************  GCreateFrameWindow  ***************************

	Creates an application's frame window and initializes support for the
	Microsoft Windows Multiple Document Interface.

	GWindowPtr GCreateFrameWindow ( short id, char *title, short left, short top
	short width, short height, int visible )

	(id): resource ID number for window's icon, cursor, menu, and class name.
	(title): C-style string containing title for window.
	(left): desired coordinate for left side of window.
	(top): desired coordinate for top side of window.
	(width): desired width for window.
	(height): desired height for window.
	(visible): if TRUE, window will be initially visible; if FALSE, hidden.
	
	The function returns a pointer to the application's frame window, or
	NULL on failure.
	
	In order to run a multiple-window application under Microsoft Windows,
	you need to enable Multiple Document Interface (MDI) support by creating
	a frame window for your application with this function before creating
	any other windows.

	Most parameters to this function are the same as for GCreateWindow().
	However, the window's type is always G_APPLICATION_WINDOW (as described
	above), and the window is initially displayed in a maximized state.
	
	GUILib completely handles the frame window's behavior.  Your
	application will never receive events related to it, and thus you may
	not associate any custom data or event-handling procedure with it.

	Macintosh applications do not have frame windows, so on that platform
	this function does nothing, and returns NULL.

*****************************************************************************/

GWindowPtr GCreateFrameWindow ( short, char *, short, short, short, short, int );
	
/***  GDeleteWindow  *********************************************************

	Destroys a window and frees associated memory.

	void GDeleteWindow ( GWindowPtr window )

	(window): pointer to the window to be destroyed.

	The function also destroys all of the window's child controls.

	You may have "piggybacked" some application-defined data onto a
	window by storing a pointer to the data in the window.  If so,
	and if the window contains the only pointer to that data, be
	sure to destroy that data before destroying the window: otherwise,
	you will have a memory leak.  See GCreateWindow(), GGetWindowData()
	and GSetWindowData().

	If the window being deleted is a document window, and your application is
	maining a window menu, then the menu item corresponding to the specified
	window (window) may be removed from the window menu.

*****************************************************************************/

void GDeleteWindow ( GWindowPtr );

/***  GGetMainWindow  **********************************************************

	Returns a pointer to the application's main window.

	GWindowPtr	GGetMainWindow ( void )

	In Microsoft Windows, GUILib assumes that the first window your application
	creates will be its main window.  This is the window in which the program's
	menu bar will be displayed, and will cause your application to quit when
	closed.  If your application will support multiple windows, its main window
	should be an MDI frame window (see GCreateFrameWindow().)

	A Macintosh application can run without having any windows open, so this
	function always returns NULL on that platform.

********************************************************************************/

GWindowPtr GGetMainWindow ( void );

/*** GGetWindowClass **********************************************************

	Obtains a window's application-defined class number.
	
	short GGetWindowID ( GWindowPtr window )
	
	(window): pointer to window whose class number you wish to determine.
	
	The function returns the window's application-defined class number, or
	zero on failure.  You should give all of your windows non-zero class numbers
	for this reason!  See GCreateWindow().
	
******************************************************************************/

short GGetWindowClass ( GWindowPtr );

/*** GSetWindowCursor ********************************************************

	Sets the cursor that will be displayed when the mouse is over the window's
    active content area.

	void	GSetWindowCursor ( GWindowPtr window, GCursorPtr cursor )

    (window): pointer to window.
    (cursor): pointer to cursor.

    The function returns nothing.

	The cursor that you pass to this function will be displayed whenever the
    mouse is moved over the window's content area.  If the mouse is in another
    area of the window (e.g. the title bar, resize box, etc.) or over a control,
    the cursor may be changed to a different, operating-system-defined cursor.

    If you pass NULL as the cursor pointer, the standard arrow cursor will be
    displayed when the mouse is located over the window.

    GUILib's cursor maintainence takes place only while an event is being
    processed, i.e. during a call to GProcessEvent().  Your application can
    switch the cursor to some other shape (e.g. a watch or hourglass cursor,
    while some lengthly process is taking place), and the cursor will stay
    in that state until another event is processed.  As soon as GProcessEvent()
    gets called again, GUILib will change the cursor back to whatever cursor
    is defined for the window over which the mouse is located, or to some
    other operating-system-defined cursor (if the mouse is not located in any
    active window's content area.)

    On the Macintosh, the window must be hilited for the window cursor to be
    displayed when the mouse is located over its content area.  GUILib will
    set the cursor to the standard arrow cursor when the mouse is located over

    inactive application windows.

    In Windows, this function is implemented by storing the cursor handle
    as a class long; i.e. GSetWindowCursor ( window, cursor ) is implemented
    essentially as SetWindowLong ( window, GCL_HCURSOR, cursor ).  As a result,
    the specified cursor will be displayed whenever the mouse is located over
    ANY window which shares the same window class as (window.)

******************************************************************************/

void GSetWindowCursor ( GWindowPtr, GCursorPtr );

/*** GGetWindowCursor *********************************************************

	Returns a pointer to the cursor that is displayed whenever the mouse is
    located over a particular window.

    GCursorPtr GGetWindowCursor ( GWindowPtr window )

    (window): pointer to window.

    The function may return NULL if no cursor has been associated with the
    window.  See GSetWindowCursor() for more information.
    
******************************************************************************/

GCursorPtr GGetWindowCursor ( GWindowPtr );

/******************************  GGetWindowTitle  ****************************

	Copies a window's title into a C-style string.

	char *GGetWindowTitle ( GWindowPtr window, char *title )

	(window): pointer to window
	(title): receives title as a C-style string

	Returns a pointer to the given string.

	GGetWindowTitle() assumes that the buffer (title) can hold up to 255
	characters.

*****************************************************************************/

char *GGetWindowTitle ( GWindowPtr, char * );

/******************************  GSetWindowTitle  ****************************

	Copies a C-style string into a window's title.

	void GSetWindowTitle ( GWindowPtr window, char *title )

	(window): pointer to window
	(title): contains title as a C-style string

*****************************************************************************/

void GSetWindowTitle ( GWindowPtr, char * );

/************************  GGetWindowTitleBarHeight  ************************

	Returns the height of window title bars, in pixels.

	short GGetWindowTitleBarHeight ( void )

*****************************************************************************/

short GGetWindowTitleBarHeight ( void );

/***  GGetWindowContentRect  ************************************************

	Obtains a window's content rectangle in local window coordinates.

	GRectPtr GGetWindowContentRect ( GWindowPtr window, GRectPtr rect )

	(window): pointer to the window whose content rectangle you want.
	(rect): rectangle structure to receive dimensions of content area.

	The function returns a pointer to the given rectangle.  On return, the
	rectangle contains the position and size of the window's content area,
	which does not include its title bar, resize handles, etc., expressed in
	pixels RELATIVE TO THE TOP LEFT CORNER OF THE CONTENT AREA (thus the top
	and left coordinate of the rectangle are always zero.)
	
	Use the GGetWindowFrameRect() function to determine the bounds of the window's
	entire frame area, including title bar, resize handles, etc., in screen
	coordinates.

*******************************************************************************/

GRectPtr GGetWindowContentRect ( GWindowPtr, GRectPtr );

/***  GGetWindowFrameRect  **************************************************

	Obtains a window's frame rectangle in global screen or workspace
	coordinates.

	GRectPtr GGetWindowFrameRect ( GWindowPtr window, GRectPtr frame )
	
	(window): pointer to the window whose frame rectangle you want.
	(rect): rectangle structure to receive dimensions of frame area.
	
	The function returns a pointer to the given rectangle.  On return,
	the rectangle contains the position and size of the window's entire
	frame area, including title bar, resize handles, etc.  On the Mac,
	this is always expressed in units of pixels relative to the top left
	corner of the screen.  This is also true in Windows, unless the window
	in question is a MDI child document window.  In that case, the window's
	coordinates will be given relative to the top left corner of the MDI
	frame window's content area, i.e. the application's workspace.
	
	Use the GGetWindowContentRect() function to determine the dimensions
	of the window's content area (the area you have control over, not
	including title bar, resize handles, etc.)
	
*****************************************************************************/

GRectPtr GGetWindowFrameRect ( GWindowPtr, GRectPtr );

/***  GSetWindowContentRect  ************************************************

	Modifies a window's content rectangle in local window coordinates.

	void GSetWindowContentRect ( GWindowPtr window, GRectPtr frame )
	
	(window): pointer to the window whose frame rectangle you want.
	(rect): rectangle structure to receive dimensions of frame area.
	
	The function returns nothing.  On return, the window is moved and/or
	resized so that its content area (which does not include its title bar,
	resize handles, etc.) is as specified by the rectangle you provide,
	expressed in pixels RELATIVE TO THE CURRENT TOP LEFT CORNER OF ITS
	CONTENT AREA.
	
	Use the GSetWindowFrameRect() function to specify the position and dimensions
	of the window's entire frame area, including title bar, resize handles, etc.
	
*****************************************************************************/

void GSetWindowContentRect ( GWindowPtr, GRectPtr );

/***  GSetWindowFrameRect  *************************************************

	Modifies a window's frame rectangle in global screen or workspace
	coordinates.

	void GSetWindowFrameRect ( GWindowPtr window, GRectPtr frame )
	
	(window): pointer to the window whose frame rectangle you want.
	(rect): rectangle structure to receive dimensions of frame area.
	
	The function returns nothing.  On return, the window is moved and/or
	resized so that its entire frame area (which includes its title bar,
	resize handles, etc.) is as specified by the rectangle you provide.
	On the Mac, this rectangle is always expressed in units of pixels
	relative to the top left corner of the screen.  In Windows, this is
	also true, unless the window in question is an MDI child document
	window.  In that case, the window's coordinates will be set relative
	to the top left corner of the MDI frame window's content area (i.e.
	the application's workspace area.)
	
	Use the GSetWindowContentRect() function to specify the dimensions of
	the window's content area (the area you have control over, not including
	title bar, resize handles, etc.)
	
*****************************************************************************/

void GSetWindowFrameRect ( GWindowPtr, GRectPtr );

int GIsMoveableWindow ( GWindowPtr );
int GIsResizableWindow ( GWindowPtr );
int GIsCloseableWindow ( GWindowPtr );
int GIsMaximizableWindow ( GWindowPtr );
int GIsMinimizableWindow ( GWindowPtr );
int GIsFloatingWindow ( GWindowPtr );

/*** GIsDocumentWindow *******************************************************

	Determines whether or not a window is a document window.

	int GIsDocumentWindow ( GWindowPtr window )
	
	(window): pointer to a window.
	
	The function returns TRUE if the specified window is a document window,
	and FALSE otherwise.
	
	On the Macintosh, document windows are all application windows which are
	not dialog windows or floating windows.  (See GIsDialogWindow() and
	GIsFloatingWindow().)  This also holds true for Windows, with the
	following clarifications:
	
	In a single-window Microsoft Windows application, the application's
	main window (as returned by GGetMainWindow()) is a document window.
	
	In a multiple window Microsoft Windows (Multiple Document Interface or MDI)
	application, document windows are child windows of the application's MDI
	client window.  (The MDI client window is an invisible child window
	of the application's main window, and is not accessible via any public
	GUILib API.)

	Note that in order to receive menu events, a window must be a document
	window.  See GSetMenuEventWindow().
		
******************************************************************************/

int GIsDocumentWindow ( GWindowPtr );

/*************************  GCenterWindow  ***********************************

	Centers a window on the screen.

	void GCenterWindow ( GWindowPtr window )

	(window): pointer to a window.

	This function repositions a window so that it is centered on the screen.
	It does not change the window's vertical position relative to to other
	windows or resize the window.

******************************************************************************/

void GCenterWindow ( GWindowPtr );

/*************************  GGetWindowData   *********************************

	Returns a window's application-defined long-integer data.

	long GGetWindowData ( GWindowPtr window )

	(window): pointer to the GWindow.

	If the window has no application-defined long-integer data, returns 0.

*******************************************************************************/

long GGetWindowData ( GWindowPtr );

/*************************  GSetWindowData   *********************************

	Stores application-defined long integer data in a window.

	char GSetWindowData ( GWindowPtr window, long data )

	(window): pointer to the GWindow.
	(data): long-integer value to be stored in window.

	The function returns nothing.

*******************************************************************************/

void GSetWindowData ( GWindowPtr, long );

/************************  GSetActiveWindow  *********************************

	Activates a window.

	void GSetActiveWindow ( GWindowPtr window )

	If a different window is currently active, calling this function will
	automatically deactivate it.  Otherwise, it will have no effect, i.e.
	you are activating the window which is already active.

	Activating a window will always make it frontmost on the Mac.  This
	is usually true in Windows also, except if a dialog window is already
	open.  Since dialogs, in Windows, always appear on top of all other
	windows, the dialog may still be on top even though you've activated
	a different window.

	If the window being activated is a document window, then that window
	will recieve subsequent menu events.  See GGetMenuEventWindow().  If
	GUILib is maintaining a window menu for your application, then the
	corresponding window menu item will receive a check mark.  See function
	GSetWindowMenu().

*****************************************************************************/

void GSetActiveWindow ( GWindowPtr );

/*************************  GGetActiveWindow  ********************************

	Returns a pointer to the application's currently-active window.
	
	GWindowPtr GGetActiveWindow ( void )

	The active window is that which the user is currently interacting with.
	Both Mac and Windows indicate this by highliting the title bar and/or
	frame of the active window.  Also, the active window is usually one and
	the same as the frontmost window.  (On the Mac, it is -always- the frontmost
	window.)  The exception, as usual, is in Microsoft Windows.  There, dialog
	windows (even modeless dialogs) will always appear on top of all other
	application windows  In addition, dialog windows are -not- clipped to the
	client area of the application's frame window, as normal MDI application
	windows are.

	If no window is open, the function returns NULL.  If your application is
	running under Microsoft Windows and has enabled Multiple Document Interface
	support by creating a frame window for the application, this function returns
	NULL if there are no MDI child windows open.  It will never return a pointer
	to the application's frame window.

	WHATE DOES IT RETURN UNDER IN MICROSOFT WINDOWS WHEN THE ACTIVE WINDOW IS A
	MODELESS OR MODAL DIALOG?
	
******************************************************************************/

GWindowPtr GGetActiveWindow ( void );

/***  GShowWindow  **************************************************************

	Displays a window if it is hidden.

	void GShowWindow ( GWindowPtr window )

	This function merely toggles the window's visible/hidden flag in the
	native OS.  If a the window is completely covered up by other windows
	on the screen, it may still be invisible even after this function is
	called.  To move it in front of the other windows, you need to activate
	it using GActivateWindow().  (And even then, in MS Windows, it may still
	be covered up, since dialog windows always appear on top of other windows
    whether they are active or not.)

	If the window is a document window that was previously hidden, and your
	application is maining a window menu, then a menu item corresponding
	to the specified window (window) may be added to the window menu.
	
*********************************************************************************/

void GShowWindow ( GWindowPtr );

/************************  GHideWindow  *************************************

	Hides a window.

	void GHideWindow ( GWindowPtr window )

	This function toggles a window's visible/hidden flag to hide the window.
	If you hide the currently-active window, a different one will be activated.
	NOTE: this does not seem to be true in a Microsoft Windows MDI application;
	hiding an MDI document window does not seem to make the next window active.
	
	If the window is a document window that was previously hidden, and your
	application is maining a window menu, then the menu item corresponding
	to the specified window (window) may be removed from the window menu.

*****************************************************************************/

void GHideWindow ( GWindowPtr );

/***********************  GIsWindowVisible  ******************************

	Determines whether or not a window is visible.

	char GIsWindowVisible ( GWindowPtr window )

	(window): pointer to the window.

	Returns TRUE or FALSE, depending on whether the window is visible.
	A window can be visible even if completely covered up; this routine
	simply checks the window's visiblity flag in the native OS.

**************************************************************************/

int GIsWindowVisible ( GWindowPtr );

/************************  GMaximizeWindow  *********************************

	Maximizes a window.

	void GMaximizeWindow ( GWindowPtr window )

	(window): pointer to the window to be maximized.

	On the Macintosh, this has the same effect as clicking in a window's
	"Zoom Out" box.  If the window has no zoom box, this does nothing.

	In Microsoft Windows, this maximizes the window.
	
	If the window is hidden, this function will show it; similarly, if the
	window is minimized or collapsed, this function will restore/un-collapse
	it.
	
	This function calls your application's event-handling procedure to send
	a maximize event to the window.  Thus, your application should never
	call this function in response to a maximize event, or it will enter
	an endless recursive loop.
	
*****************************************************************************/

void GMaximizeWindow ( GWindowPtr );

/************************  GMinimizeWindow  *********************************

	Minimizes a window (i.e. turns it into an icon).

	void GMinimizeWindow ( GWindowPtr window )

	(window): pointer to window to be minimized.

	On the Macintosh, this function collapses a window.  If the window is
	not collapsable, the function does nothing.
	
	In Microsoft Windows, this function iconifies a window.  If the window
	is not minimizable, this function does nothing.

	This function calls your application's event-handling procedure to send
	a minimize event to the window.  Thus, your application should never
	call this function in response to a minimize event, or it will enter
	an endless recursive loop.

*****************************************************************************/

void GMinimizeWindow ( GWindowPtr );

/************************  GRestoreWindow  *********************************

	Restores a maximized or minimized window to its normal size and position

	void GRestoreWindow ( GWindowPtr window )

	On the Macintosh, this has the same effect as clicking in the window's
	"Zoom In" box.  Additionally, if the window is collapsed, this function
	will un-collapse it.  If the window has no zoom in box and is already
	not collapsed, this function does nothing.

	In Microsoft Windows, this function restores the window to its
	dimensions before the preceding maximize or minimize event.
	
	This function calls your application's event-handling procedure to send
	a restore event to the window.  Thus, your application should never
	call this function in response to a restore event, or it will enter
	an endless recursive loop.
	
*****************************************************************************/

void GRestoreWindow ( GWindowPtr );

/**************************  GIsWindowMaximized  *****************************

	Determines whether or not a window is in its maximized or zoomed state.

    int GIsWindowMaximized ( GWindowPtr window )

    (window): pointer to window whose state you wish to determine.

    The function returns TRUE if the window is in its maximized state,
    and FALSE otherwise.

******************************************************************************/

int GIsWindowMaximized ( GWindowPtr );

/**************************  GIsWindowMinimized  *****************************

	Determines whether or not a window is in its minimized or iconic state.

    int GIsWindowMinimized ( GWindowPtr window )

    (window): pointer to window whose state you wish to determine.

    The function returns TRUE if the window is in its minimized state,
    and FALSE otherwise.  On the Macintosh, this returns TRUE if the window
    is collapsed, and FALSE if the window is not collapsed.

******************************************************************************/

int GIsWindowMinimized ( GWindowPtr );

/**************************  GCloseWindow  ***********************************

	Send a close event to a window.

	void GCloseWindow ( GWindowPtr window )

	(window): pointer to window to close.

	This function returns nothing.
	
	Note that this function does not actually close, hide, or delete the
	window.  Instead, it calls the window's event-handling function and
	passes it a close event.  The window's event-handler then performs
	whatever actions should take place when the window is to be closed.
	This way, the correct behavior should be exhibited no matter what
	window is being closed.
	
	GUILib calls this function when the user clicks in a window's close
	box or selects "Close" from its system menu; you should call it whenever
	you want the equivalent behavior to occur (for example, when the user
	selects a "Close Window" menu item).
	
******************************************************************************/

void GCloseWindow ( GWindowPtr );

/**************************  GInvalidateWindow  ******************************

	Marks part or all of a window for update.
	
	void GInvalidateWindow ( GWindowPtr window, GRectPtr rect )
	
	(window): pointer to window.
	(rect): pointer to rectangle describing area of window to mark for update.
	
	This function adds the specifid rectangle to the portion of the window
	requiring update.  If you give NULL for (rect), the window's entire content
	rectangle is marked for update.
	
	If no already-pending update events for the window, this function will
	generate one; otherwise, it simplay adds the specified area to the region
	already marked for update.  Note that this function does not actually
	update the window or call your application's window-update-event-handling
	procedure.  It simply modifies the region already requring update.  
	
	Your application should never call this function when it is responding
	to an update event; otherwise, an endless stream of update events would
	be generated!

******************************************************************************/

void GInvalidateWindow ( GWindowPtr, GRectPtr );

/**************************  GValidateWindow  ********************************

	Removes part or all of a window from the area marked for update.
	
	void GValidateWindow ( GWindowPtr window, GRectPtr rect )
	
	(window): pointer to window.
	(rect): pointer to rectangle describing area of window to mark for update.
	
	You can use this function to avoid redundant screen drawing by telling
	the native window maganer that part of a window does not need to be
	redrawn.  You can pass NULL for (rect) to remove the window's entire
	centent area from its update region. 
	
******************************************************************************/

void GValidateWindow ( GWindowPtr, GRectPtr );

/**************************  GUpdateWindow  ********************************

	Completely redraws a window's invalid region.
	
	void GUpdateWindow ( GWindowPtr window )
	
	(window): pointer to window to redraw.
	
	This function completely redraws any invalid portion of a window.
	It also clears the window's invalid region to prevents further
	update events from being generated for the window.

	This function will send an update event to your window's event-handling
	procedure.  Therefore, your event-handling procedure MUST NEVER CALL THIS
	FUNCTION as part of its response to update events; otherwise, you would
	generate an infinite recursive loop, and instant program failure.
	
****************************************************************************/

void GUpdateWindow ( GWindowPtr );

/****************************  GEraseOnUpdates  ****************************

	Enables or disables automatic erasing of a window's update region.
	
	void GEraseOnUpdates ( GWindowPtr window, int erase )
	
	(window): pointer to window in which to enable automatic erasing.
	(erase): if TRUE, automatic erasing will be enabled; if FALSE, disabled.
	
	When a window needs to be updated, GUILib normally erases the window
	before calling your application's event handler to update the window.
	For some applications, this may cause redundant drawing or erasing,
	producing a performance hit and/or unpleasant screen flicker.  This
	function lets you disable (by passing FALSE) or re-enable (by passing
	TRUE) this behavior for the window.
	
	Note that this function does not actually erase anything!  It simply
	enabled or disables automatic erasing the next time a window update
	event occurs.  To generate an update event for the window, call the
	function GInvalidateWindow().
	
****************************************************************************/

void GEraseOnUpdates ( GWindowPtr, int );
	
/***************************  GIsErasedOnUpdates  ***************************

	Determines whether a window's automatic update region erasure is enabled.
	
	int GIsErasedOnUpdates ( GWindowPtr window )
	
	(window): pointer to window to determine erasure enabling of.

	The function returns TRUE if the window's automatic update region
	erasure is enabled, and FALSE otherwise.
	
****************************************************************************/

int GIsErasedOnUpdates ( GWindowPtr );

/***********************  GShowSizeBox  ************************************

	Enables or supresses automatic display of the resize box in a window's
	lower right-hand corner.
	
	void GShowSizeBox ( GWindowPtr window )
	void GHideSizeBox ( GWindowPtr window )

	(window): pointer to window in which size box should appear.

	On the Macintosh, a window's resize box (or "grow icon") normally
	appears in its lower right hand corner, between the scroll bars.
	Because it this area is part of the window's content area, rather
	than its frame, the size box is -not- automatically drawn by the
	Mac's window manager when a window is updated.
	
	However, rather than being forced to have your application draw the
	grow icon every time the window receives an update, activate, or
	deactivate event, you can have GUILib handle this responsibility by
	calling GShowSizeBox().  Note that when you create resizable Macintosh
	windows using the GCreateWindow() function, GUILib calls this function
	by default.
	
	Similarly, you can prevent GUILib from drawing the window's grow icon
	by calling GHideSizeBox().  This will erase the size box and invalidate
	its rectangle, generating an update event for its containing window.
	
	In Windows, the equivalent of the Mac's size box is a "size box"
	scroll bar control.  This appears as a small gray rectangle in the
	window's lower right corner, and allows the window to be resized
	when clicked in.  GUILib automatically creates a size box control
	in every window your application creates with the GCreateWindow()
	function; these routines simply hide or show that control.  Note
	that in Windows (unlike the Mac) the size box is -not- displayed
	by default when you create a resizable window!

*************************************************************************/

void GShowSizeBox ( GWindowPtr );
void GHideSizeBox ( GWindowPtr );

/***********************  GIsSizeBoxVisible  ****************************

	Determines a window's size box is visible.

	int GIsSizeBoxVisible ( GWindowPtr window )

	(window): poiner to window whose size box visibility you wish to
	          determine.
	          
	The function returns TRUE if the window's size box is visible and
	FALSE otherwise.
	
*************************************************************************/

int GIsSizeBoxVisible ( GWindowPtr );

/****************************  GDrawSizeBox  ********************************

	Draws a window's size box.
	
	void GDrawSizeBox ( GWindowPtr window )
	
	(window): pointer to window in which to draw size box.
	
	This function is called by GUILib whenever a window's size box needs
	to be drawn.  Your application should never have to call it directly.
	
*****************************************************************************/

void GDrawSizeBox ( GWindowPtr );

/************************  GTileWindows  ************************************

	Tiles Microsoft Windows Multiple Document Interface windows.

	void GTileWindows ( void )

	Although the Macintosh has no equivalent toolbox routine, GUILib
	emulates the behavior of the Windows Tile function on that platform.

*****************************************************************************/

void GTileWindows ( void );

/************************  GCascadeWindows  ************************************

	Cascades Microsoft Windows Multiple Document Interface windows.

	void GCascadeWindows ( void )

	Although the Macintosh has no equivalent toolbox routine, GUILib
	emulates this behavior of Windows on that platform.

*****************************************************************************/

void GCascadeWindows ( void );

/************************  GArrangeWindowIcons  ******************************

	Arranges Microsoft Windows Multiple Document Interface window icons.

	void GCascadeWindows ( void )

	This function does nothing on the Macintosh, since the Mac OS has
	no equivalent for iconized windows.

*****************************************************************************/

void GArrangeWindowIcons ( void );

/***  GSetWindowMenu  *******************************************************

	Appends window menu items to the end of an existing menu.

	void GSetWindowMenu ( GMenuPtr menu )

	(menu): pointer to menu to which window items should be added.

	In Windows, this function uses the Multiple Document Interface to append
	a list of windows to the specified menu.  Although the Macintosh has no
	equivalent toolbox routine, GUILib emulates Windows' behavior there
	inasmuch as is appropriate to the Mac environment.  That behavior is
	as follows:

	New windows will have their titles appended to the menu whenever you
	create them, and removed when you delete them.  A window's menu item
	is automatically updated if you rename the window, and a check mark is
	placed next to the item corresponding to the currently-active window.
	Selecting an item from menu's window list activates the window.  Note
	that this is the same window which will receive menu events; see
	GGetMenuEventWindow().
	
    Only document windows created with the GCreateWindow() function will
    appear in the window list.  Dialog windows and floating windows are -not-
    added to the list; see GIsDocumentWindow().

*****************************************************************************/

void GSetWindowMenu ( GMenuPtr menu );

/****************************  GWindowToScreen  ******************************

	Converts a point from local window coordinates to screen coordinates.

	void GWindowToScreen ( GWindowPtr window, short *x, short *y )

	(window): pointer to window whose coordinate frame you are converting from
	(x): contains window X-coordinate on input; receives screen X on output.
	(y): contains window Y-coordinate on input; receives screen Y on output.

	Returns nothing.

******************************************************************************/

void GWindowToScreen ( GWindowPtr, short *, short * );

/****************************  GScreenToWindow  ******************************

	Converts a point from screen coordinates to local window coordinates.

	void GScreenToWindow ( GWindowPtr window, short *x, short *y )

	(window): pointer to window whose coordinate frame you are converting to.
	(x): contains screen X-coordinate on input; receives window X on output.
	(y): contains screen Y-coordinate on input; receives window Y on output.

	Returns nothing.

******************************************************************************/

void GScreenToWindow ( GWindowPtr window, short *x, short *y );

/****************************  GWindowToScreenRect  ******************************

	Converts a rectangle from local window coordinates to screen coordinates.

	GRectPtr GWindowToScreen ( GWindowPtr window, GRectPtr rect )

	(window): pointer to window whose coordinate frame you are converting from
	(rect): pointer to rectangle to convert from window to screen coordinates.

	The function returns a pointer to the given rectangle.  On return, the
	rectangle's coordinates will be converted from the window's coordinate
    frame to the screen's.

******************************************************************************/

GRectPtr GWindowToScreenRect ( GWindowPtr, GRectPtr );

/****************************  GScreenToWindowRect  **************************

	Converts a rectangle from screen coordinates to local window coordinates.

	GRectPtr GScreenToWindowRect ( GWindowPtr window, GRectPtr rect )

	(window): pointer to window whose coordinate frame you are converting to
	(rect): pointer to rectangle to convert from screen to window coordinates.

	The function returns a pointer to the given rectangle.  On return, the
	rectangle's coordinates will be converted from the screen's coordinate
    frame to the window's.

******************************************************************************/

GRectPtr GScreenToWindowRect ( GWindowPtr, GRectPtr );

/************************  GSetWindowScrollRects  ***************************

	Given a pointer to a window, computes rectangles for its scroll bars.
	
	void GSetWindowScrollRects ( GWindowPtr window, GRectPtr vertScrollRect,
	GRectPtr horiScrollRect, GRectPtr windowRect )
	
	(window): pointer to window whose scroll bar rectangles you want.
	(vertScrollRect): if non-NULL, receives vertical scroll bar coordinates.
	(horiScrollRect): if non-NULL, receives horizontal scroll bar coordinates.
	(windowRect): in non-NULL, receives window coordinates.
	
	This function returns nothing.  On return, the rectangles whose pointers
	you pass as (vertScrollRect), (horiScrollRect), and (windowRect) will
	contain the coordinates of a standard vertical scroll bar, horizontal
	scroll bar, and remaining area of the window.
	
	You may pass NULL for any of these rectangles if you do not wish to
	compute it.  However, since the presence of one scroll bar can affect
	the dimensions of the other, you should at least pass valid rectangle
	pointers for (vertScrollRect) and ( horiScrollRect) if your window has
	both vertical and horizontal scroll bars.
	
	On the Mac, this function takes the presence of the window's size box
	into account if the size box is automatically being drawn; see function
	GShowSizeBox() for more information.
	
****************************************************************************/

void GSetWindowScrollRects ( GWindowPtr, GRectPtr, GRectPtr, GRectPtr );

/****************************  GFindWindow  *********************************

	Determines which window, if any, contains a given point on the screen.

	GWindowPtr GFindWindow ( short x, short y )

	(x,y): point to test, in pixels relative to top left corner of the screen

	The function returns a pointer to the window containing the specified
	point, or NULL if the point lies outside all windows.

******************************************************************************/

GWindowPtr GFindWindow ( short, short );

/**************************  GScrollWindow  **********************************

	Scrolls a rectangular portion of a window by a specified amount.
	
	void GScrollWindow ( GWindowPtr window, short right, short down,
	GRectPtr scrollRect, GRectPtr clipRect, int update )

	(window):     pointer to window whose contents you wish to scroll.
	(right):      number of pixels right by which to scroll contents.
	(down):       number of pixels down by which to scroll contents.
	(scrollRect): pointer to rectangle describing area you wish to scroll.
	(clipRect):   pointer to rectangle describing area to clip drawing to.
	(update):     if TRUE, updte the region vacated by scrolling.

	This function scrolls a rectangular portion of a window, shifting its
	contents by a specified number of pixels horizontally and/or vertically.
	
	The number of pixels by which to scroll the window's contents are specified
	in the parameters (right) and (down).  To scroll the window's contents
	left or up, pass a negative value for the respective parameter.  Note
	that one (or both!) of these parameters may be zero.

	The area to scroll is described by the rectangle (scrollRect).  If the
	argument (clipRect) is a valid pointer to a rectangle, drawing will be
	restricted to the area described by the contents of (clipRect).  If you
	do not wish to restrict drawing, pass NULL for (clipRect).
	
	If you pass TRUE for (update), the window will receive an update event
	for the region vacated by the scrolling; if you pass FALSE, no update
	event will be generated.

******************************************************************************/

void GScrollWindow ( GWindowPtr, short, short, GRectPtr, GRectPtr, int );

/*** functions in Menu.c ***/

/***************************  Menus  *****************************************

	Mac and Windows have very different conventions for referring to menus,
	submenus, and menu items.  In order for GUILib to provide a consistent
	interface, you need to follow a few rules when designing your resources.

	On the Mac:
	
	Menu bars are defined by 'MBAR' resources, and menus are defined by
	'MENU' resources.  You give those resources any ID numbers you choose,
	but the first 'MENU' resource in any menu bar (usually the "Apple" menu)
	must have the same ID number as the 'MBAR' resource which contains it.
	
	The Mac OS requires heirarchal submenus to have resource ID numbers
	between 1 and 255.  Hence, you should probably number your first 'MBAR'
	resouce (and its first 'MENU' resource) 256 to leave the maximum
	possible number of submenu resource ID numbers available.
		
	In Windows:
	
	Menu bars are defined by "MENU" resources, and menus within them are
	defined as "POPUP" menu items.  Entire menus do not have ID numbers
	as on the Mac; instead, individual menu items have ID numbers.  GUILib
	uses the item ID number to generate a "fake menu ID" number to indicate
	which menu the item came from.  For this to work properly, you must use
	the following convention to number menu items: 
	
	itemID = 100 * menuID + item
	
	where (menuID) is the resource ID number of the corresponding Mac
	'MENU' resource, and (item) is the sequential number of the item within
	the menu, counting from 1.  Thus items in the Windows equivalent of a
	Mac 'MENU' resource with ID number 256 would be numbered 25601, 25602, etc.
	
	Another problem GUILib must resolve is the fact that in Mac applications,
	the first menu in the main menu bar is usually the "Apple" menu, which
	has no equivalent in Windows.  To preserve consistency across platforms,
	GUILib considers the "Apple" menu to be the "0th" menu in the menu bar,
	the "File" menu to be the 1st menu, the "Edit" menu to be the 2nd menu,
	etc.  Thus, on the Mac, the "Apple", "File", and "Edit" resources within
	'MBAR' resource number 256 would be numbered 256, 257, and 258.  Items
	in the corresponding Windows "MENU" resource would be numbered 25701,
	25702, 25703, ... in the "File" menu; 25801, 25802, 25803, ... in the
	"Edit" menu (there would be no items numbered 25601, 25602, 25603, ... ).
	See GGetMenuBar() and GGetSubMenu() for further discussion.
	 
******************************************************************************/

/*** Limits on resource ID numbers of heirarchal submenus ***/

#define G_MIN_SUB_MENU_ID		1
#define G_MAX_SUB_MENU_ID		255

/*** Positions of standard main menus in the menu bar ***/

#define G_APPLE_MENU			0
#define G_FILE_MENU				1
#define G_EDIT_MENU				2

/*** Positions of standard items in the "Edit" menu ***/

#define G_EDIT_CUT_ITEM			3
#define G_EDIT_COPY_ITEM		4
#define G_EDIT_PASTE_ITEM		5
#define G_EDIT_CLEAR_ITEM		6

/************************  GGetMenuBar  ***********************************

	Creates and displays the program's menu bar from a resource template.
	
	GMenuBarPtr GGetMenuBar ( short id )

	(id): menu bar's resource ID number.

	The function returns a pointer to the menu bar, or NULL on failure.
	You should never have to do anything with this pointer, other than
	check for a NULL value on return of this function.

	In Windows, GUILib attaches the menu bar to the first window your
	application creates (which should be its frame window, if your
	application uses multiple windows).  Therefore, you MUST create
	a window before calling this function; it will fail otherwise.

	On the Macintosh, the menu bar is not associated with any window, but
	is instead drawn at the top of the screen; you don't need to create a
	window first.  When the menu bar is created, GUILib appends standard
	Apple menu items (e.g. desk accessories) to its first menu (which should
	have the same resource ID number as the menu bar itself; see above).

******************************************************************************/

GMenuBarPtr GGetMenuBar ( short );

/******************************  GCreateMenu  *********************************

	Allocates memory for a new empty menu record.
	
	GMenuPtr ( short menuID )
	
	(menuID): ID number for the menu.
	
	The function returns a pointer to the new menu record, or NULL on failure.
	When the user selects items from the menu, the value you give in the
	(menuID) parameter will identify which menu the item came from.

	Initially, the menu will have no title or items; use GInsertMenuItem()
	to add items to it, and GSetSubMenu() to attach it to an existing menu.
	
*******************************************************************************/

GMenuPtr GCreateMenu ( short );

/******************************  GDeleteMenu  *********************************

	Frees memory associated with a menu record.
	
	void GDeleteMenu ( GMenuPtr menu )
	
	(menu): pointer to menu record to delete.
	
	This function return nothing.  After you call it, the menu pointer you
	give will be invalid.
	
*******************************************************************************/
	
void GDeleteMenu ( GMenuPtr );

/*****************************  GGetMainMenu  *******************************

	Returns a pointer to a main menu.

	GMenuPtr GGetMainMenu ( short item )

	(item): position of menu in the menu bar; see below
	
	Use this function to obtain pointers to your program's main menus, i.e.
	the top-level menus which are directly accessible from the main menu bar.
	(Use GGetSubMenu() to obtain pointers to heirarchal submenus.)

	The (item) parameter indicates the sequential number within the menu bar
	of the menu whose handle you wish to retrieve.  On the Mac, the number
	should be counted from zero; in Windows, it should be counted from one.
	Because the first menu in Mac menu bars (i.e. menu zero) is usually the
	"Apple" menu, which has no equivalent in Windows, subsequent menu items
	can be referred to by the same number on both platforms.
	
******************************************************************************/

GMenuPtr GGetMainMenu ( short );

/******************************  GGetSubMenu  *********************************

	Returns a submenu attached to an existing menu item.
	
	GMenuPtr GGetSubMenu ( GMenuPtr menu, short item )
	
	(menu): pointer to parent menu containing desired submenu.
	(item): item number of submenu within parent menu, counting from 1.
	
	The function returns a pointer to the submenu at the indicated position,
	or NULL if one does not exist there.

********************************************************************************/

GMenuPtr GGetSubMenu ( GMenuPtr, short );

/*****************************  GSetSubMenu  **********************************

	Attaches a menu as a submenu of an existing menu item.
	
	void GSetSubMenu ( GMenuPtr menu, short item, GMenuPtr subMenu )
	
	(menu): pointer to menu to attach the submenu to.
	(item): item in menu to attach the submenu to.
	(subMenu): pointer to submenu to attach.

	The (menu) parameter must point to an existing menu (either a main
	top-level menu, or an existing submenu).  The (item) parameter must
	specify an existing item in the menu (menu), counting from one: i.e.
	1 for the 1st item, for the 2nd, etc.

	On the Macintosh, submenus may only have ID numbers between 1 and 255.
		
*******************************************************************************/
 
void GSetSubMenu ( GMenuPtr, short, GMenuPtr );

/*************************  GInsertMenuItem  ********************************

	Inserts a new item into a menu.

	void GInsertMenuItem ( GMenuPtr menu, short menuID, short item, char *text )
	
	(menu): pointer to menu to insert new item into
	(menuID): ID number of menu to insert new item into.
	(item): desired position for new item, counting from 1.
	(text): null-terminated string containing new menu item text.

	This function returns nothing.
	
	The item can be inserted anywhere in the menu; passing 1 for the (item)
	parameter will insert the new item before all others; a number greater
	than the number of items already present will append the new item to the
	end of the menu.  Existing menu items below the one you insert will have
	their item numbers increased by one.
	
	In Windows, the menu ID parameter (menuID) specifies which menu ID number
	your application will receive when the user selects the new item.  It
	should be the same as the corresponding Mac 'MENU' resource ID number.
	
	Finally, the item text string you give (text) should not contain more than
	255 characters.

*****************************************************************************/

void GInsertMenuItem ( GMenuPtr, short, short, char * );

/*************************  GDeleteMenuItem  ********************************

	Removes an item from a menu.

	void GDeleteMenuItem ( GMenuPtr menu, short item )
	
	(menu): pointer to menu to remove item from.
	(item): item to remove, counting from 1.

	This function returns nothing.
	
	Existing items below the one you give (item) will have their item
	numbers decreased by one.

*****************************************************************************/

void GDeleteMenuItem ( GMenuPtr menu, short );

/*************************  GGetMenuItemText  *******************************

	Copies the text of a menu item.

	char *GGetMenuItemText ( GMenuPtr menu, short item, char *text )

	(menu): pointer to the menu
	(item): position of the desired item in the menu, starting from 1.
	(text): receives copy of menu item text as a C-style string.

	The function returns a pointer to the input text buffer (text).
	It assumes the buffer can hold up to 256 characters.

*****************************************************************************/

char *GGetMenuItemText ( GMenuPtr, short, char * );

/*************************  GSetMenuItemText  *******************************

	Sets the text of a menu item.

	void GSetMenuItemText ( GMenuPtr menu, short item, char *text )

	(menu): pointer to the menu
	(item): position of the desired item in the menu, starting from 1.
	(text): C-style string to copy into menu item.

	This function returns nothing.  It assumes the buffer can hold up to
	256 characters.

*****************************************************************************/

void GSetMenuItemText ( GMenuPtr, short, char * );

/************************  GSetMenuItemGraphic  ******************************

	Adds a graphic (bitmap in Windows, icon on Mac) to a menu item.
	
	void GSetMenuItemBitmap ( GMenuPtr menu, short item, short graphicID,
	     char iconType )
	
	(menu): pointer to menu cointaining item to add graphic to.
	(item): menu item number to add graphic to.
	(graphicID): resource ID number of bitmap or icon to replace menu item.
	(iconType): specifies small or recued icons (ignored in Windows).
	
	The behavior of this function is somewhat platform-dependent.  On the
	Mac, it adds the specified 'cicn' (color icon) resource to the left of
	the menu item text, or 'ICON' (B/W icon) resource if no 'cicn' is found.
	You can alternatively specify that a reduced 'ICON' or 'SICN' (small icon)
	resource be used, by passing either of the two constants #defined below
	as the (iconType) argument; any other values will be ignored.  However,
	this will prevent the menu item from having a keyboard command equivalent.
	
	In Windows, the graphic resource is a BITMAP rather than an icon, and it
	completely replaces the menu item.  The (iconType) argument is ignored.
	
	This function does not check whether the specified menu item, bitmap,
	and/or icon resources actually exist!
	
*****************************************************************************/

void GSetMenuItemGraphic ( GMenuPtr, short, short, char );

#define G_SMALL_MENU_ICON		0x1E
#define G_REDUCED_MENU_ICON		0x1D

/****************************  GEnableMainMenu  *******************************

	Enables or disables a menus in the main menu bar.
	
	void GEnableMainMenu ( short menu, int flag )
	
	(menu): position of menu in menu bar counting from 1 in Windows, 0 on Mac.
	(flag): if TRUE, enable menu; if FALSE, disable menu.
	
	Use this function to enable or disable your program's main menus; see
	GGetMainMenu() for an explanation of the main menu numbering scheme.

	Use GEnableMenuItem to enable/disable items within a menu.
	
*******************************************************************************/

void GEnableMainMenu ( short, int );
	
/*************************  GIsMainMenuEnabled  *******************************

	Determines whether or not a menu in the main menu bar is enabled.
	
	int GIsMainMenuEnabled ( short menu )
	
	(menu): position of menu in menu bar counting from 1 in Windows, 0 on Mac.
	
	This function returns TRUE if the specified main menu is enabled and FALSE
	if it is not.  See GGetMainMenu() for an explanation of the main menu
	numbering scheme.

	Use GIsMenuItemEnabled to determine whether individual items within a menu
	are enabled or disabled.
	
*******************************************************************************/

int GIsMainMenuEnabled ( short );
	
/*****************************  GEnableMenuItem  ******************************

	Enables or disables a menu item.

	void GEnableMenuItem ( GMenuPtr menu, short item, int flag )

	(menu): pointer to menu containing item to enable/disable.
	(item): number of menu item to enable or disable.
	(flag): if TRUE, item will be enabled; if FALSE, it will be disabled.
	
	This functions returns.  If you disable a menu item, its text will be
	grayed and selecting it will have no effect. 

*******************************************************************************/

void GEnableMenuItem ( GMenuPtr, short, int );

/*****************************  GIsMenuItemEnabled  ***************************

	Determines whether or not a menu item is enabled.

	int GIsMenuItemEnabled ( GMenuPtr menu, short item )

	(menu): pointer to menu containing item to determine status of.
	(item): number of menu item to determine enable status of.

	The function returns TRUE if the menu item is enabled and FALSE if not.
	
*******************************************************************************/

int GIsMenuItemEnabled ( GMenuPtr, short );

/*****************************  GCheckMenuItem  ****************************

	Checks or unchecks a menu item.
	
	void GCheckMenuItem ( GMenuPtr menu, short item, int flag )

	(menu): pointer to menu containing item to mark.
	(item): menu item to mark, counting from one.
	(flag): if TRUE, check the menu item; if FALSE, un-check it.

	This function will not check any menu item which leads to a submenu.

*******************************************************************************/

void GCheckMenuItem ( GMenuPtr, short, int );

/****************************  GIsMenuItemChecked  ****************************

	Determines if a menu item is checked.
	
	int GIsMenuItemChecked ( GMenuPtr menu, short item )

	(menu): pointer to menu containing desired item.
	(item): menu item, counting from one.

	This function returns TRUE or FALSE, depending on whether the specified
	menu item is checked.

*******************************************************************************/

int GIsMenuItemChecked ( GMenuPtr, short );

/*****************************  GSetCheckedMenuItem  **************************

	Checks one item in a group of menu items, and unchecks all the others.
	
	void GSetCheckedMenuItem ( GMenuPtr menu, short firstItem, short lastItem,
	short checkedItem )

	(menu): pointer to the menu containing items to check/uncheck.
	(firstItem): number of first menu item in group.
	(lastItem): number of last menu item in group.
	(checkedItem): number of item to check.
	
	On return, the menu item specified to be checked (checkedItem) will be
	checked, and all other menu items in the specified range will be unchecked.
	If (checkedItem) is outside the specified range, no menu items in that range
	will be checked.

*******************************************************************************/

void GSetCheckedMenuItem ( GMenuPtr, short, short, short );

/****************************  GGetCheckedMenuItem  ****************************

	Finds the checked item among a group of menu items (if any).
	
	short GGetCheckedMenuItem ( GMenuPtr menu, short firstItem, short lastItem )
	
	(menu): menu containing group of items from which to find checked item
	(firstItem): number of first item to examine for check
	(lastItem): number of last item to examine for check
	
	This function searches the specified range of items in the given menu
	for the first checked item.  It returns zero if no items within the range
	specified are checked.
	
*******************************************************************************/

short GGetCheckedMenuItem ( GMenuPtr, short, short );

/****************************  GGetMenuBarHeight  ****************************

	Returns the height of the menu bar, in pixels.

	short GGetMenuBarHeight ( void )

	In Windows, where a multi-line menu bar is possible, this function just
	returns the height of a single-line menu bar, regardless of how many
	lines the application's menu bar actually contains.

*****************************************************************************/

short GGetMenuBarHeight ( void );

/*****************************  GGetNumMenuItems  *******************************

	Returns the number of items in a menu.

	short GGetNumMenuItems ( GMenuPtr menu )

	(menu): pointer to menu.

******************************************************************************/

short GGetNumMenuItems ( GMenuPtr );

/***************************  GDoPopupMenu  **********************************

	Displays a popup menu and tracks the mouse while it is displayed.

	int GDoPopupMenu ( GMenuPtr menu, short item, short x, short y )
	
	(menu): pointer to menu to display.
	(item): number of item to hilite when menu is initially displayed.
	(x): horizontal screen coordinate at which to initially display menu.
	(y): vertical screen coordinate at which to initially display menu.

	The function returns TRUE if the user actually selected an item from
	the menu, or FALSE if the user doesn't select anything.
		
	Use this function to display and let the user select items from a
	menu anywhere on the screen.

	When the menu is initially displayed by this function, the top left corner
	of the initially hilited item (specified in the (item) parameter) will be
	positioned at pixel (x,y) relative to the top left corner of the screen.
	(Note: in Windows, the (item) parameter is ignored; the initially hilited
	item is always the first one in the menu.)
	
	This function tracks the motion of the mouse until the user dismisses the
	popup menu (by releasing the mouse button on the Mac, or by pressing a
	key or clicking the mouse in Windows).  If the user selects a menu item,
	your program's event-handing procedure will receive a menu event containing
	the menu ID and item number selected, and the GDoPopupMenu() will return
	TRUE.  If the user dismisses the menu without selecting an item, no menu
	event will be generated, and the function will return FALSE.

	IMPLEMENTATION NOTE: the Windows version of this function always returns
    TRUE, regardless of whether or not the user actually selected anything.
    This will be fixed in a future GUILib release.
    
	IMPORTANT NOTE: the native popup-menu-tracking functions on both Mac and
	Windows swallow the mouse-up event that occurrs when the mouse is released.
	Hence, if you call this function in response to a mouse-down event, neither
	GUILib nor your application will receive a mouse-up event when the user
	releases the mouse button.

******************************************************************************/

int GDoPopupMenu ( GMenuPtr, short, short, short );

/******************************  GDoFontMenuItem  ********************************

	Implements standard Macintosh "Font", "Size", and "Style" menu behavior.
	
	void GDoFontMenuItem ( short menu, short item, char *font, short *size,
	short *style )
	
	(menu): menu ID number from which item was selected.
	(item): item number of selected item within menu.
	(font): receives name of selected font as a null-terminated string.
	(size): receives size of selected font.
	(style): receives style of selected font.
	
	The Macintosh has no built-in, standard equivalent of Windows's "Choose Font"
	dialog.  However, Apple's Human Interface Guidelines suggest a standard
	mechanism for letting the user select a text font, size, and style from
	a set of menus.  GUILib provides those menus and implements that mechanism
	with this function.
	
	GUILib provides three 'MENU' resources with the ID numbers #defined below as
	the "Font", "Size", and "Style" menus.  These are automatically loaded and
	built when you call GGetMenuBar().  Whenever your application receives a
	menu event whose menu ID parameter corresponds to one of these numbers, you
	should call GDoFontMenuItem() to check/uncheck the item selected by the user,
	and receive the corresponding font name, number, size, and/or style.
	
	Since the user can only select one item from one menu at a time, the values
	which correspond to items in the other two menus will be unaffected.  For
	example, if you call this function as a result of the user having selected
	an item from the "Size" menu, the only function parameter whose value will
	be changed is (size); the contents of the (font) and (style) parameters
	will be unaffected.
	
	When the function returns, the selected menu's items will be checked to
	reflect the user's selection.  In the "Font" and "Size" menus, the selected
	item will be checked, and all others will be unchecked.  In the "Style" menu,
	the item selected by the user will have its check mark toggled, with one
	exception: if the "Plain" item is selected, it will be checked, and all other
	items unchecked.  Additionally, if all items other than "Plain" are unchecked,
	"Plain" will be checked.
	
	This function is not implemented in the Windows version of GUILib; however,
	there you can use Windows' built-in "Choose Font" dialog; see GDoFontDialog().
	
**********************************************************************************/

void GDoFontMenuItem ( short, short, char *, short *, short * );

#define G_TEXT_FONT_MENU_ID		1
#define G_TEXT_SIZE_MENU_ID		2
#define G_TEXT_STYLE_MENU_ID	3

/******************************  GCheckFontMenus  ********************************

	Updates check marks in the "Font", "Size", and "Style" menus to reflect a
	specified text font, size, and style.
	
	void GCheckFontMenus ( char *font, short size, short style )
	
	(font): name of selected font as a null-terminated string.
	(size): size of selected font.
	(style): style of selected font.
	
	You can use this function to update the check marks in the standard "Font",
	"Size", and "Style" menus provided by GUILib to indicate any combination of
	text font, size, and style you desire.  See GDoFontMenuItem() for more info.
	
	This function is not implemented in the Windows version of GUILib.
	
**********************************************************************************/

void GCheckFontMenuItems ( char *, short, short );

/*************************** functions in Control.c **************************

	MAC IMPLEMENTATION NOTE:
	
	The Macintosh control record does not contain any field indicating the
	control ID number.  GUILib gets around this problem by storing the ID
	number in the ControlRecord's contrlRefCon field.  This means that you
	cannot use any control's reference constant for anything else: if you call
	SetCRefCon() directly from within your application, GUILib will almost 
	certainly die.
	
	All of the Mac controls created by GUILib have the "useWFont" variant
	code.  If you want the control's text to be displayed in some non-
	standard font, you should set the control's owning window's font to that
	font -before- creating the control; otherwise there will be some ugliness
	when the control first appears (and resizes itself to fit the new font).
	
******************************************************************************/

/****************************  GCreatePushButton  ****************************

	Creates a new push button control.
	
	GControlPtr GCreatePushButton ( GWindowPtr window, short id, char *text,
	            GRectPtr rect, int visible )
	
	(window): pointer to window in which the control should reside.
	(id): application-defined ID number for control.
	(text): null-terminated string containing control's title.
	(rect): pointer to rectangle defining control's size and position.
	(visible): if TRUE, the control should be initially visible.
	
	The function returns a pointer to the control, or NULL on failure.

	The (window) parameter defines which window the control should
	reside in.  When the window is moved, the control moves with it;
	when the window is deleted, the control is deleted as well.
	You can obtain a pointer to a control's owning window with the
	function GGetControlWindow().
	
	The (id) parameter, which identifies the control within the window,
	may take any value, but you should not use zero, and you should make
	sure that every control within the same window has a unique ID number.
	You can obtain a control's ID number via GGetControlID(), but after
	the control is created, its ID number may not be modified.
	
	The control's initial title (text) may contain no more than 255
	characters; any more than that will be ignored.  You can obtain
	a copy of the control's title with GGetControlText() and modify
	it with GSetControlText().
	
	The control's initial size and position (rect) are given relative
	to the top left corner of the window in which it resides (window).
	On the Mac, they are given in units of pixels; in Windows, they
	may be given in "dialog base units" if the control is being created
	in a dialog window.
	
	If you pass TRUE for the (visible) parameter, the control will be
	displayed immediately after it is created.  If you give FALSE,
	the control will not be made visible until you call GShowControl().
	
****************************************************************************/
	
GControlPtr GCreatePushButton ( GWindowPtr, short, char *, GRectPtr, int );

/****************************  GCreateCheckBox  ****************************

	Creates a new check box control.
	
	GControlPtr GCreatePushButton ( GWindowPtr window, short id, char *text,
	            GRectPtr rect, int visible, short value )
	
	(window): pointer to window in which the control should reside.
	(id): application-defined ID number for control.
	(text): null-terminated string containing control's title.
	(rect): pointer to rectangle defining control's size and position.
	(visible): if TRUE, the control should be initially visible.
	(value): control's initial state: 1 = checked; 0 = unchecked.
	 
	The function returns a pointer to the control, or NULL on failure.
	
	See GCreatePushButton() for a discussion of the parameters (window),
	(id), (text), (rect), and (visible).
	
*********************************************************************************/

GControlPtr GCreateCheckBox ( GWindowPtr, short, char *, GRectPtr, int, short );

/****************************  GCreateRadioButton  ****************************

	Creates a new radio button control.
	
	GControlPtr GCreateRadioButton ( GWindowPtr window, short id, char *text,
	            GRectPtr rect, int visible, short value )
	
	(window): pointer to window in which the control should reside.
	(id): application-defined ID number for control.
	(text): null-terminated string containing control's title.
	(rect): pointer to rectangle defining control's size and position.
	(visible): if TRUE, the control should be initially visible.
	(value): control's initial state: 1 = checked; 0 = unchecked.
	 
	The function returns a pointer to the control, or NULL on failure.

	See GCreatePushButton() for a discussion of the parameters (window),
	(id), (text), (rect), and (visible).

*********************************************************************************/

GControlPtr GCreateRadioButton ( GWindowPtr, short, char *, GRectPtr, int, short );

/****************************  GCreateScrollBar  ****************************

	Creates a new scroll bar control.
	
	GControlPtr GCreateScrollBar ( GWindowPtr window, short id, char *text,
	            GRectPtr rect, int visible, short value, short min, short max )
	
	(window): pointer to window in which the control should reside.
	(id): application-defined ID number for control.
	(text): null-terminated string containing control's title.
	(rect): pointer to rectangle defining control's size and position.
	(visible): if TRUE, the control should be initially visible.
	(value): control's initial value.
	(min): control's minimum value.
	(max): control's maximum value.
	 
	The function returns a pointer to the control, or NULL on failure.

	See GCreatePushButton() for a discussion of the parameters (window),
	(id), (text), (rect), and (visible).

	You can use the function GGetScrollValue() and GSetScrollValue() to obtain
	and modify a scroll bar's current value.  You can obtain and change its
	minimum and maximum values with GSetScrollMinMax() and GGetScrollMinMax().
	
*********************************************************************************/

GControlPtr GCreateScrollBar ( GWindowPtr, short, char *, GRectPtr, int, short, short, short );

/****************************  GCreatePopupMenu  ****************************

	Creates a new Macintosh popup menu control, or Windows combo-box control.
	
	GControlPtr GCreatePopupMenu ( GWindowPtr window, short id, char *text,
	            GRectPtr rect, int visible, short menuID, short width )
	
	(window): pointer to window in which the control should reside.
	(id): application-defined ID number for control.
	(text): C-style string containing control's title.
	(rect): pointer to rectangle defining control's size & position.
	(visible): if TRUE, control should be initially visible; if FALSE, hidden.
	(menuID): popup menu's Macintosh resource ID number.
	(width): width of area in which to display popup menu's title, in pixels.
			 This area will be hilited when the user clicks in the control.

	The function returns a pointer to the control, or NULL on failure.

	See GCreatePushButton() for a discussion of the parameters (window),
	(id), (text), (rect), and (visible).

	You may create a popup menu control from a Mac 'CNTL' resource as well
	as creating one directly; see GCreateCustomControl().  If you put a
	control resource into a dialog template (Mac resource types 'DLOG' and
	'DITL') the control will be created automatically along with the dialog.

	Before you define a popup menu control in a Mac 'CNTL' resource, you need
	to define the menu it contains in a 'MENU' resource.  Then, when you define
	the 'CNTL' resource, use 1008 for the procID field to indicate a popup menu.
	Use the popup menu control's 'MENU' resource ID number for the control's
	minimum, the title width as its maximum, and the title justification code
	for the control's value (0 = left-justifified; see Apple's documentation for
	other justification codes.)

	Although Windows has no direct equivalent of the Mac popup menu control,
	a functionally identical "Combo-Box" control can be used instead.  In
	Windows, that's what this function creates.  The main differences are that
	
	1) The combo box will not have any title displayed to its left.
	
	2) The items in the combo box's drop-down list box are defined in a
	   "STRINGTABLE" resource rather than a 'MENU' resouce.  When creating the
	   combo box, GUILib looks for strings in the "STRINGTABLE" with consecutive
	   ID numbers starting at 100 * (menuID).  For example, if you wanted a
	   combo-box containing the three strings whose "STRINGTABLE" ID numbers
	   were 7000, 7001, and 7002, you'd pass 70 for the menuID, and make sure
	   there is nothing with an ID of 7003 in the "STRINGTABLE".   (And on the
	   Mac side, you'd make a 'MENU' resouce with the same ID number, containing
	   the same items.)
	
	When you place a combo-box in a Windows dialog template (resource type
	"DIALOG", Windows automatically creates the combo-box along with the
	dialog.  However, the combo-box's drop-down list-box is initially empty.
	You must fill it yourself, in your code, by using GSetPopupMenuMenu().
	(You'd probably want to do this while handling the dialog window's
	initial event, before the dialog is displayed on screen.)

	To return a handle to a popup menu's menu, use GGetPopupMenuMenu().  To
	store a different menu in the popup menu, use GSetPopupMenuMenu(); access
	individual items with GGetPopupMenuItem() and GSetPopupMenuItem().

******************************************************************************/

GControlPtr GCreatePopupMenu ( GWindowPtr, short, char *, GRectPtr, int, short, short );

/*** GCreateEditText ***********************************************************

	Creates an editable-text control.

	GControlPtr GCreateEditText ( GWindowPtr window, short id, char *text,
	            GRectPtr rect, int visible, int vertScroll, int horiScroll );
	
	(window): pointer to window in which the control should reside.
	(id):     application-defined ID number for control.
	(text):   C-style string containing control's initial text.
	(rect):   pointer to rectangle defining control's size & position.
	(visible): if TRUE, control should be initially visible; if FALSE, hidden.
	(vertScroll): if TRUE, control should contain a vertical scroll bar.
	(horiScroll): if TRUE, control should contain a horizontal scroll bar.
	
	See GCreatePushButton() for a discussion of the parameters (window),
	(id), (text), (rect), and (visible).
	
********************************************************************************/

GControlPtr GCreateEditText ( GWindowPtr, short, char *, GRectPtr, int, int, int );

/****  GCreateProgressBar  ***************************************************

	Creates a new progress bar control.
	
	GControlPtr GCreateProgressBar ( GWindowPtr window, short id, char *text,
	            GRectPtr rect, int visible, short min, short max, short value )
	
	(window):  pointer to window in which the control should reside.
	(id):      application-defined ID number for control.
	(text):    null-terminated string containing control's title.
	(rect):    pointer to rectangle defining control's size and position.
	(visible): if TRUE, the control should be initially visible.
	(min):     minimum value for progress bar.
	(max):     maximum value for progress bar.
	(value):   initial value for progres bar.
	
	The function returns a pointer to the control, or NULL on failure.

	A progress bar control appears as a "thermometer" which fills
	from left to right to indicate the status of a lengthy task.
	Unlike other controls, progress bars recieve no user input;
	they merely display their current value (which normally represents
	the status of a task in progress.)
	
	The (window) parameter defines which window the control should
	reside in.  When the window is moved, the control moves with it;
	when the window is deleted, the control is deleted as well.
	You can obtain a pointer to a control's owning window with the
	function GGetControlWindow().
	
	The (id) parameter, which identifies the control within the window,
	may take any value, but you should not use zero, and you should make
	sure that every control within the same window has a unique ID number.
	You can obtain a control's ID number via GGetControlID(), but after
	the control is created, its ID number may not be modified.
	
	The control's initial title (text) may contain no more than 255
	characters; any more than that will be ignored.  You can obtain
	a copy of the control's title with GGetControlText() and modify
	it with GSetControlText().
	
	The control's initial size and position (rect) are given relative
	to the top left corner of the window in which it resides (window).
	On the Mac, they are given in units of pixels; in Windows, they
	may be given in "dialog base units" if the control is being created
	in a dialog window.
	
	If you pass TRUE for the (visible) parameter, the control will be
	displayed immediately after it is created.  If you give FALSE,
	the control will not be made visible until you call GShowControl().
	
	The parameters (min,max,value) define the minimum, maximum, and
	initial values displayed in the control.  You can change these values
	using the functions GSetControlMin(), GSetControlMax(), and
	GSetControlValue(), respectively.
	
****************************************************************************/
	
GControlPtr GCreateProgressBar ( GWindowPtr, short, char *, GRectPtr, int,
short, short, short );

/****  GCreateGroupBox  ****************************************************

	Creates a new group box control.
	
	GControlPtr GCreateGroupBox ( GWindowPtr window, short id, char *text,
	            GRectPtr rect, int visible )
	
	(window):  pointer to window in which the control should reside.
	(id):      application-defined ID number for control.
	(text):    null-terminated string containing control's title.
	(rect):    pointer to rectangle defining control's size and position.
	(visible): if TRUE, the control should be initially visible.
	
	The function returns a pointer to the control, or NULL on failure.

	A group box control appears as a rectangular divider with a text title
	at its upper left.  It is primarily used to create visual separation
	between other controls inside a dialog box.  Group box controls recieve
	no user input and display no current value.
	
	On the Macintosh, the OS 8 Appearance Manager provides Group Box controls
	which may display a checkbox or a popup menu instead of a plain-text title.
	Because this has no equivalent in the Windows API, it is not supported by
	GUILib.  However, you may create Macintosh dialogs which contain such
	controls in their resource forks.  You can obtain the checkbox state and/or
	current popup menu item displayed in this kind of Group Box control by
	using GGetControlValue()/GSetControlValue().  The only way do the equivalent
	in Windows is to create two separate superimposed controls (e.g. a checkbox
	sitting on top of a group box control with an empty title), give the
	checkbox/popup menu the same ID number as the corresponding Macintosh 
	group box control dialog item, and give the Windows group box an ID number
	which has no equivalent on the corresponding Mac side.
	
	The (window) parameter defines which window the control should reside in.
	When the window is moved, the control moves with it; when the window is deleted,
	the control is deleted as well.  You can obtain a pointer to a control's owning
	window with the function GGetControlWindow().
	
	The (id) parameter, which identifies the control within the window,
	may take any value, but you should not use zero, and you should make
	sure that every control within the same window has a unique ID number.
	You can obtain a control's ID number via GGetControlID(), but after
	the control is created, its ID number may not be modified.
	
	The control's initial title (text) may contain no more than 255 characters;
	any more than that will be ignored.  You can obtain a copy of the control's
	title with GGetControlText() and modify it with GSetControlText().
	
	You can create a group box with no title by passing a zero-length (but not
	NULL) string for the (text) parameter.
	
	The control's initial size and position (rect) are given relative
	to the top left corner of the window in which it resides (window).
	On the Mac, they are given in units of pixels; in Windows, they
	may be given in "dialog base units" if the control is being created
	in a dialog window.
	
	If you pass TRUE for the (visible) parameter, the control will be
	displayed immediately after it is created.  If you give FALSE,
	the control will not be made visible until you call GShowControl().
	
****************************************************************************/
	
GControlPtr GCreateGroupBox ( GWindowPtr, short, char *, GRectPtr, int );

/************************  GDeleteControl  ***********************************

	Frees all memory associated with a control and removes it from its window.
	
	void GDeleteControl ( GControlPtr control )

	(control): pointer to control to delete.

	Use this function to destroy any control type.  Note that when a window
	is deleted, all of its controls are also deleted automatically.
	
******************************************************************************/

void GDeleteControl ( GControlPtr control );

/************************  GGetControl  **************************************

	Obtains a handle to one of a window's controls.

	GControlPtr GGetControl ( GWindowPtr window, short id )

	(window): pointer to the window where the control lives.
	(id): ID number of the desired control.

	Returns a pointer to the control, or NULL on failure.

******************************************************************************/

GControlPtr GGetControl ( GWindowPtr, short );

/************************  GGetControlWindow  ********************************

	Returns a pointer to the window in which a control lives.

	GWindowPtr GGetControlWindow ( GControlPtr control )

	(control): pointer to the control whose parent window you want.

	Returns a pointer to the control's parent window, or NULL on failure.

******************************************************************************/

GWindowPtr GGetControlWindow ( GControlPtr );

/*************************  GGetControlID  ************************************

	Obtains a control's application-defined ID number.
	
	short GGetControlID ( GControlPtr control )
	
	(control): pointer to the control.
	
	Returns the control's application-defined ID number, or 0 on failure.
	
*******************************************************************************/

short GGetControlID ( GControlPtr );

/***************************  GGetControlTitle  *****************************

	Obtains a copy of a control's title.
	
	void GGetControlTitle ( GControlPtr control, char *title )
	
	(control): pointer to a control.
	(title): pointer to buffer to receive control title.
	
	On return, the buffer will contain up to 255 characters of the control's
	title as a null-terminated string.  Note: for editable-text controls,
	you should use the function GGetEditTextText() instead of this function
	to obtain a copy of the control's text.  To obtain the text of Mac
	dialog text items, you should use the function GGetDialogItemText().
	
*****************************************************************************/

void GGetControlTitle ( GControlPtr, char * );

/***************************  GSetControlTitle  *****************************

	Modifies a control's title.
	
	void GSetControlTitle ( GControlPtr control, char *title )
	
	(control): pointer to a control.
	(title): pointer to buffer containing desired control title.
	
	The buffer may contain a null-terminated string of up to 255 characters
	to use as the control's title.  Note: for editable-text controls,
	you should use the function GSetEditTextText() instead of this function
	to set the control's text.  To set the text of Mac dialog text items,
	you should use the function GSetDialogItemText().
	
*****************************************************************************/

void GSetControlTitle ( GControlPtr, char * );

/***************************  GGetControlRect  ******************************

	Obtains a control's position and size in its parent window.

	GRectPtr GGetControlRect ( GControlPtr control, GRectPtr rect )

	(control): pointer to the control.
	(rect): pointer to GRect to receive coordinates.

	On return, (rect) will contain the control's coordinates in the
	frame of reference of its parent window.

*****************************************************************************/

void GGetControlRect ( GControlPtr, GRectPtr );

/***************************  GSetControlRect  ******************************

	Moves and/or resizes a control.

	void GSetControlRect ( GControlPtr control, GRectPtr rect )

	(control): pointer to the control.
	(rect): pointer to GRect containing desired coordinates for control.

	The coordinates in (rect) should be in the frame of reference of the
	control's parent window.  If visible, the control is redrawn automatically
    when this function is called.

*****************************************************************************/

void GSetControlRect ( GControlPtr, GRectPtr );

/************************  GGetControlValue  ********************************

	Obtains a control's current value.

	short GGetControlValue ( GControlPtr control )
	
	(control): pointer to a control.
	
	The meaning of a control's value depends on the control type:

	-for push buttons, the control's minimum, maximum, and current value
	 is always zero.
	
	-for check boxes and radio buttons, the value is zero if the control is
	 unchecked, and one if the control is checked.  The minimum and maximum
	 values are always zero and one, respectively.
	 
	-for scroll bars, the value can take any number between (and including)
	 a user-specifiable minimum and maximum value.
	 
	-for popup menus and combo-boxes, the control value is the number of the
	 currently-displayed item.  The minimum value is one if the control's
	 popup menu (or drop-down list box) contains any items; it is only zero
	 if the control is empty.  The maximum value is the number of items in
	 in the control's popup menu/drop-down list box.

	-for progress bars, the control value is the current value of the
	 progress indicator.  The minimum and maximum values are the minimum
	 and maximum values of the progress indicator.  Please note that in
	 Windows, the minimum and maximum values of a progress bar cannot be
	 negative!
	 
******************************************************************************/

short GGetControlValue ( GControlPtr );

/************************  GSetControlValue  ***********************************

	Sets a control's value.
	
	void GSetControlValue ( GControlPtr control, short value )
	
	(control): pointer to a control.
	(value): desired value for control.
	
	You cannot set a control's value beyond its minimum or maximum; see
	GGetControlValue() for more information.  
	
********************************************************************************/

void GSetControlValue ( GControlPtr, short );

/************************  GGetControlXXX  *********************************

	Obtains a control's minimum, maximum, and current values.

	short GGetControlMin ( GControlPtr control )
	short GGetControlMax ( GControlPtr control )
	
	(control): pointer to control.
	
	These functions return the minimum, maximum, and current values
	of the specified control.
	
	The meaning of a control's value depends on the control type:

	-for push buttons, the control's minimum, maximum, and current value
	 is always zero.
	
	-for check boxes and radio buttons, the value is zero if the control is
	 unchecked, and one if the control is checked.  The minimum and maximum
	 are always zero and one, respectively.
	 
	-for scroll bars, the value can take any number between (and including)
	 the user-specifiable minimum and maximum.
	 
	-for popup menus and combo-boxes, the value is the number of the
	 currently-displayed item.  The minimum value is one if the control's
	 popup menu (or drop-down list box) contains any items; it is only zero
	 if the control is empty.  The maximum value is the number of items in
	 in the control's popup menu/drop-down list box.

	-for progress bars, the control value is the current value of the
	 progress indicator.  The minimum and maximum values are the minimum
	 and maximum values of the progress indicator.  Please note that in
	 Windows, the minimum and maximum values of a progress bar cannot be
	 negative!

******************************************************************************/

short GGetControlMin ( GControlPtr );
short GGetControlMax ( GControlPtr );

/************************  GSetControlMin  ***********************************

	Sets a control's minimum or maximum values.
	
	void GSetControlMin ( GControlPtr control, short min )
	void GSetControlMax ( GControlPtr control, short max )
	
	(control): pointer to control.
	(min):     new minimum value for control.
	(max):     new maximum value for control.
	
	These functions return nothing.  You cannot set a control's value
	to a number beyond its current minimum or maximum.  Also note that
	certain values you give may be ignored for certain types of controls,
	e.g. you cannot set a check box or radio button's maximum to a
	number greater than one.
	
	Control values have different meanings for different types of controls;
	see GGetControlValue().
	 
***************************************************************************/

void GSetControlMin ( GControlPtr, short );
void GSetControlMax ( GControlPtr, short );

/************************  GHideControl  *************************************

	Makes a visible control hidden.
	
	void GHideControl ( GControlPtr control )

	(control): pointer a control.

	After the control is hidden, it will not receive any input from the
	mouse or keyboard, and your application will not receive control events
	related to it.  On the Macintosh, the control's rectangle is invalidated,
	generating an update event for the window which contains it.
	
	Use GIsControlVisible() to determine if a control is currently visible.
	
*****************************************************************************/

void GHideControl ( GControlPtr );

/************************  GShowControl  *************************************

	Makes a hidden control visible.
	
	void GShowControl ( GControlPtr control )

	(control): pointer to a control.
	
	If the control is already visible, this function has no effect.  Use
	GIsControlVisible() to determine in a control is currently visible.
	
*****************************************************************************/

void GShowControl ( GControlPtr );

/*****************************  GToggleControl  *****************************

	Toggles a dialog check box control.

	void GToggleControl ( GControlPtr control )

	(control): pointer to control.

	This function sets the control's value to zero if it is non-zero and
	vice-versa, then redraws the control.

******************************************************************************/

void GToggleControl ( GControlPtr );

/***************************  GEnableControl  ******************************

	Enables or disables a control.

	void GEnableControl ( GControlPtr control, int enable )

	(control): pointer to the control.
	(enable): TRUE/FALSE flag indicating whether control should be
	          enabled or disabled.

	This function enables or disables a control.  If the control is disabled,
	it is redrawn in gray indicating that it is no longer active, and it will
	no longer generate control events when the user clicks in it.  Use the
	function GEnableControl() to determine if a control is currently enabled.

*****************************************************************************/

void GEnableControl ( GControlPtr, int );

/************************  GDrawControl  *************************************

	void GDrawControl ( GControlPtr control )

	(control): pointer to control to draw.

*****************************************************************************/

void GDrawControl ( GControlPtr );

/***********************  GIsControlVisible  ********************************

	Determines whether or not a control is currently visible.
	
	int GIsControlVisible ( GControlPtr control )
	
	(control): pointer to a control.
	
	The function returns TRUE if the specified control is visible, and
	FALSE if it is not.  You can change a control's visibility with
	GShowControl() and GHideControl().
	
*****************************************************************************/

int GIsControlVisible ( GControlPtr );

/***********************  GIsControlEnabled  ********************************

	Determines whether or not a control is currently enabled.
	
	int GIsControlEnabled ( GControlPtr control )
	
	(control): pointer to control.
	
	The function returns TRUE if the specified control is enabled,
	and FALSE if it is not.  You can enable or disable a control
	with GEnableControl().
	
*****************************************************************************/

int GIsControlEnabled ( GControlPtr );

/************************  GGetScrollBarWidth  ******************************

	Returns the width and height of standard scroll bars, in pixels.
	 
	short GGetScrollBarWidth ( void )
	short GGetScrollBarHeight ( void )
	
	GGetScrollBarWIdth() returns the width of a standard vertical scroll bar;
	GGetScrollBarHeight returns the height of a standard horizontal scroll bar.
	
*****************************************************************************/

short GGetScrollBarWidth ( void );
short GGetScrollBarHeight ( void );

/***********************  GInsertPopupMenuItem  ******************************

	Inserts a new item into a popup menu or combo-box control.
	
	void GInsertPopupMenuItem ( GControlPtr control, short item, char *text )

	(control): pointer to popup menu control.
	(item): desired item number for new item, counting from 1.
	(text): desired item text as a null-terminated string.
	
	This function returns nothing.  On the Mac, the new item is inserted
	into the menu of a popup-menu control; in Windows, the item is inserted
	into the drop-down list-box of a combo-box control.  The item can
	be inserted anywhere in the control.  Note that existing items with
	numbers higher than the one you give (item) will have their item numbers
	increased by one.  If the item number you give is greater than the number
	of items present, the new item will be appended to the end of the menu or
	list-box.  Finally, the item text string you give (text) should hold
	no more than 255 characters.
	
*******************************************************************************/

void GInsertPopupMenuItem ( GControlPtr, short, char * );

/***********************  GDeletePopupMenuItem  ******************************

	Removes an item from a popup menu or combo-box control.
	
	void GDeletePopupMenuItem ( GControlPtr control, short item )

	(control): pointer to popup menu control.
	(item): item number of item to remove, counting from 1.
	
	This function returns nothing.  Note that existing items with numbers higher
	than the one you give (item) will have their item numbers decreased by one.
	
*******************************************************************************/

void GDeletePopupMenuItem ( GControlPtr, short );

/***********************  GGetPopupMenuItemText  ******************************

	Obtains the text of an existing item in a popup menu or combo-box control.
	
	char *GGetPopupMenuItemText ( GControlPtr control, short item, char *text )

	(control): pointer to popup menu control.
	(item): number of the item whose text you wish to obtain.
	(text): buffer which receives text of specified item as a C-string.
	
	The function returns the pointer to the buffer.  It assumes that the buffer
	can hold at least 255 characters.  If the specified item does not exist,
	the buffer's contents are unchanged.
	
*******************************************************************************/

char *GGetPopupMenuItemText ( GControlPtr, short, char * );

/***********************  GSetPopupMenuItemText  ******************************

	Modifies the text of an existing item in a popup menu or combo-box control.
	
	char *GSetPopupMenuItemText ( GControlPtr control, short item, char *text )

	(control): pointer to popup menu control.
	(item): number of the item whose text you wish to modify.
	(text): buffer which contains new text for specified item as a C-string.
	
	The function returns nothing.  If the specified item does not exist,
	the popup menu's or combo-box's contents are unchanged.
	
*******************************************************************************/

void GSetPopupMenuItemText ( GControlPtr, short, char * );

/**************************  GInsertPopupMenuStrings  *************************

	Inserts a series of string resources into a popup menu or combo box control.
	
	void GInsertPopupMenuStrings ( GControlPtr control, short firstItem,
	     short firstStringID, short lastStringID )
	
	(control): pointer to popup menu control.
	(firstItem): position of first menu/list box item to insert string into.
	(firstStringID): ID number of first string resource to insert.
	(lastStringID): ID number of last string resource to insert.

	On return, the combo box or popup menu will have have the text of the
	specified string resources inserted sequentially into its drop-down list
	box or menu, starting at the position specified by the (firstItem)
	parameter.  This parameter should be counted from one; i.e. pass 1 if you
	wish to start inserting strings at the 1st position in the menu/list box.
	
*******************************************************************************/

void GInsertPopupMenuStrings ( GControlPtr, short, short, short );

/**************************  GEnablePopupMenuItem  *************************

	Enables an item in a popup menu or combo box control.
	
	void GEnablePopupMenuItem ( GControlPtr control, short item )
	
	(control): pointer to popup menu control.
	(item): item number of item to enable, counting from 1.

	On return, the specified item in the control will be enabled.  If the
	item is already enabled, or if (item) specifies an item number greater
	than the number of items in the popup menu control, this function does
	nothing.
		
*******************************************************************************/

void GEnablePopupMenuItem ( GControlPtr, short );

/**************************  GDisablePopupMenuItem  *************************

	Disables an item in a popup menu or combo box control.
	
	void GDisablePopupMenuItem ( GControlPtr control, short item )
	
	(control): Ipointer to popup-menu or combo-box control.
	(item): item number of item to disable, counting from 1.

	On return, the specified item in the control will be disabled.  If the
	item is already disabled, or if (item) specifies an item number greater
	than the number of items in the popup menu control, this function does
	nothing.
		
*******************************************************************************/

void GDisablePopupMenuItem ( GControlPtr control, short );

/***************************  GGetEditTextFont  ****************************

	Determines the face name, character set, point size, and style of text
	displayed in an editable-text control.

	void GGetEditTextFont ( GControlPtr control, char *font, short *size,
	short *style )

	(control): pointer to edit-text control inside window.
	(font): buffer to recieve font name of text as null-terminated string.
	(size): receives size of text
	(style): recieves text style bit flags.

	See GSetPenFont() for more information regarding the return values of
    this function.

****************************************************************************/

void GGetEditTextFont ( GControlPtr, char *, short *, short * );

/***************************  GSetEditTextFont  ****************************

	Sets the font, character set, size, and style of text displayed in an
	editable-text control.

	void GSetEditTextFont ( GControlPtr control, char *font, short size,
	short style )

	(control): pointer to edit-text control inside window.
	(font): null-terminated string containing name of desired font for text.
	(size): desired size for text.
	(style): desired style bit flags.

	For more information, see GSetPenFont().

****************************************************************************/

void GSetEditTextFont ( GControlPtr, char *, short, short );

/***************************  GSetEditTextText  ********************************

	Copies text into an editable-text control.
	
	void GSetEditText ( GControlPtr control, char *text )
	
	(control): pointer to an editable-text control
	(text): pointer to buffer containing text to display inside control.
	
	The buffer (text) must contain a null-terminated string of no more
	than 32K characters.
	
********************************************************************************/

void GSetEditTextText ( GControlPtr, char * );

/***************************  GGetEditTextText  ********************************

	Obtains a copy of the text in an editable-text control.
	
	void GGetEditTextText ( GControlPtr control, char *text )
	
	(control): pointer to an editable-text control
	(text): pointer to buffer containing text to display inside control.
	
	On return, the buffer (text) will be filled with a copy of up to 32K of
	the control's text.  You may use the GGetEditTextLength() function to
	determine the actual number of characters in the control's text.

*********************************************************************************/

void GGetEditTextText ( GControlPtr, char * );

/***************************  GGetEditTextLength  ********************************

	Returns the number of characters in an editable-text control.
	
	long GGetEditTextLength ( GControlPtr control )
	
	(control): pointer to an editable-text control

	The value returned does not include the terminating null character.
	
*********************************************************************************/

long GGetEditTextLength ( GControlPtr );

/*** GSetEditTextWrap *********************************************************

	Turns word-wrapping in an editable-text control on or off.
	
	void GSetEditTextWrap ( GControlPtr control, int wrap )
	
	(control): pointer to an editable-text control.
	(wrap): if TRUE, turns on word-wrapping; if FALSE, turns it off.
	
	This function returns nothing.
	
	If the (wrap) parameter is non-zero, lines of text will be wrapped
	to the boundaries of the editable-text control.  If (wrap) is zero,
	text will only wrap at end-of-line characters (e.g. carriage return
	on the Mac, CR-LF on Windows.)
	
*******************************************************************************/

void GSetEditTextWrap ( GControlPtr, int );

/*** GGetEditTextWrap *********************************************************

	Determines whether word-wrapping in an editable-text control is on or off.
	
	int GGetEditTextWrap ( GControlPtr control )
	
	(control): pointer to an editable-text control.

	This function returns TRUE if word wrapping is on, and FALSE otherwise.
	See GSetEditTextWrap() for more information.
	
*******************************************************************************/

int GGetEditTextWrap ( GControlPtr );

/*** GGetEditTextNumLines ******************************************************

	Determines the number of lines of text contained in an editable-text
	control.
	
	long GGetEditTextNumLines ( GControlPtr control )
	
	(control): pointer to an editable-text control.

	This function returns the total number of lines of text contained in
	the control.  If word-wrap is turned on, the number of lines may exceed
	the number of end-of-line characters, since a single line of text may
	wrap around as several lines in the control.
	
*******************************************************************************/

long GGetEditTextNumLines ( GControlPtr );

/*** GGetEditTextLineLength ***************************************************

	Determines the length of a single line of text contained in an editable-
	text control.
	
	long GGetEditTextLineLength ( GControlPtr control, long line )
	
	(control): pointer to an editable-text control.
	(line):    line number.

	This function returns the number of characters of text contained in a
	single line of the editable-text control.
	
	The (line) parameter specifies the line number whose length you wish to
	determine; it ranges from zero (for the first line) to the total number
	of lines in the control minus one.  (Use GGetEditTextNumLines() to find
	out the latter value.)	The value returned by this function will be
	negative if the (line) parameter is invalid.
	
	If the line ends at an end-of-line character (e.g. CR or CR-LF), the value
	returned does not include those characters.
	
*******************************************************************************/

long GGetEditTextLineLength ( GControlPtr, long );

/*** GGetEditTextLine *********************************************************

	Obtains a copy of a single line of text contained in an editable-text
	control.
	
	long GGetEditTextLine ( GControlPtr control, long line, char *text,
	     long length )
	
	(control): pointer to an editable-text control.
	(line):    line number.
	(text):    points to buffer to copy line into.
	(length):  maximum number of characters to copy into the buffer.
	
	This function copies a single line of text contained in an editable-text
	control.  The value returned by the function is the number of characters
	copied.
	
	The (line) parameter specifies the line you wish to copy; it ranges from
	zero (for the first line) to the total number of lines in the control minus
	one.  (Use GGetEditTextNumLines() to find out the latter value.)
	
	The value returned by this function will be negative if the (line) parameter
	is invalid.

	The (text) parameter is a pointer to the buffer into which characters
	should be copied.  The (length) parameter specifies the maximum number
	of characters which can be copied into the (text) buffer.  Note that the
	buffer must be large enough to hold at least one more character than the
	(length) value, since an ASCII NUL string terminator is appended to the
	line.
	
	If the line ends with CR or CR-LF (i.e. end-of-line marker) characters,
	those characters are NOT copied into the (text) buffer.
	
*******************************************************************************/

long GGetEditTextLine ( GControlPtr, long, char *, long );

/***************************  GGetEditTextSelection  ************************

	Obtains the current selection range in an edit-text control.
	
	void GGetEditTextSelection ( GControlPtr control, long *start, long *end )
	
	(control): pointer to edit-text control window.
	(start): receives offset of first selected character.
	(end): receives offset of last selected character.
	
	When an editable-text control is active, the selected range of characters
	in it is hilited.  This function obtains the offsets of the first and last
	selected characters from the beginning of the text displayed in the control.
	Both offsets are zero-based; i.e. the offset of the 1st character is zero.
	
*****************************************************************************/

void GGetEditTextSelection ( GControlPtr, long *, long * );

/***************************  GSetEditTextSelection  ************************

	Sets the current selection range in an edit-text control.
	
	void GSetEditTextSelection ( GControlPtr control, long start, long end )
	
	(control): pointer to editable-text control.
	(start): offset of first character to select.
	(end): offset of last character to select.
	
	Both offsets are zero-based; use a selection range of 0 to 32767 to
	select the entire text of the control.

*****************************************************************************/

void GSetEditTextSelection ( GControlPtr, long, long );

/***************************  GCutEditTextSelection  ************************

	Cuts the current selection range in an edit-text control to the clipboard.
	
	void GCutEditTextSelection ( GControlPtr control )
	
	(control): pointer to editable-text control.

	This function deletes the currently-selected range of text in an
	editable-text control but puts a copy of it into the clipboard.
	
	You should always make sure that you specify the currently-active edit-
	text control (the one currently receiving keyboard input) to prevent user
	confusion; see GGetKeyEventControl().
	
	In Mac dialogs with standard edit-text items, this function always cuts
	the text of the active edit-text item, regardless of the control ID number
	you give (id).

*****************************************************************************/

void GCutEditTextSelection ( GControlPtr );

/***************************  GCopyEditTextSelection  ************************

	Copies the current selection range in an edit-text control to the clipboard.
	
	void GCopyEditTextSelection ( GControlPtr control, short id )
	
	(control): pointer to editable-text control.

	This function puts a copy of the currently-selected range of text in an
	editable-text control into the clipboard.
	
	You should always make sure that you specify the currently-active edit-
	text control (the one currently receiving keyboard input) to prevent user
	confusion; see GGetKeyEventControl().
	
	In Mac dialogs with standard edit-text items, this function always copies
	the text of the active edit-text item, regardless of the control ID number
	you give (id).

*****************************************************************************/

void GCopyEditTextSelection ( GControlPtr );

/***************************  GPasteEditTextSelection  ************************

	Pastes into the current selection range in an edit-text control from
	the clipboard.
	
	void GPasteEditTextSelection ( GControlPtr control )
	
	(control): pointer to editable-text control.

	This function replaces the currently-selected range of text in an
	editable-text control with text from the clipboard.
	
	You should always make sure that you specify the currently-active edit-
	text control (the one currently receiving keyboard input) to prevent user
	confusion; see GGetKeyEventControl().
	
	In Mac dialogs with standard edit-text items, this function always pastes
	into the active edit-text item, regardless of the control ID number
	you give (id).

*****************************************************************************/

void GPasteEditTextSelection ( GControlPtr );

/***************************  GDeleteEditTextSelection  ************************

	Deletes the current selection range in an edit-text control.
	
	void GDeleteEditTextSelection ( GControlPtr control )
	
	(control): pointer to editable-text control.

	This function deletes the currently-selected range of text in an
	editable-text control.
	
	You should always make sure that you specify the currently-active edit-
	text control (the one currently receiving keyboard input) to prevent user
	confusion; see GGetKeyEventControl().
	
	In Mac dialogs with standard edit-text items, this function always deletes
	from the active edit-text item, regardless of the control ID number
	you give (id).

*****************************************************************************/

void GDeleteEditTextSelection ( GControlPtr );

/******************************  GScrollEditText  ***************************

	Scrolls the text in an edit-text control by a specified number of lines.
	
	void GScrollEditText ( GControlPtr control, long numLines )

	(control): pointer to edit-text control containing text to scroll.	
	(numLines): number of lines to scroll the text; positive = up.

	This function also updates the associated vertical scroll bar.
	
*****************************************************************************/

void GScrollEditText ( GControlPtr, long );

/*** GDrawButton *************************************************************

	Draws a simulated rectangular push-button in a window.

    void GDrawButton ( GWindowPtr window, GRectPtr rect, char *text,
         GIconPtr icon, int pressed )

    (window):  pointer to window in which to draw button.
    (rect):    pointer to rectangle describing button's coordinates.
    (text):    text to draw as button title, or NULL if none.
    (icon):    icon to draw within button, or NULL if none.
    (pressed): if TRUE, draw button in its "pressed" state;
               if FALSE, draw button in its normal state.

	This function returns nothing.

    Use this function to simulate the appearance of a push-button control
    without actually creating such a control.  An example of when you might
    wish to do this would be to create a "button" which displays a pop-up
    menu when pressed.

    You do not need surround this function with calls to GStartDrawingWindow()
    and GEndDrawingWindow(), although doing so will cause no harm.

    On the Macintosh, the button's text title (if any) will be drawn using
    the window's current font, size, and style.

    In Windows, the button's title text will be drawn using the current GUI
    item font (i.e. as shown in the "Display" control panel).

    The icon-drawing functionality is not yet implemented.

******************************************************************************/

void GDrawButton ( GWindowPtr, GRectPtr, char *, GIconPtr, int );

/*** functions in Dialog.c ***/

/****************************  GDoAlert  *******************************

	These functions display an alert dialog box with a user-definable
	message and an icon.

	int GDoAlert ( short icon, short buttons, char *text )

	(icon):    specifies which icon (if any) to display in the dialog.
	(buttons): specifies buttons to display in the dialog.
	(text):    string containing text of message to display.
	
	The function returns the number of the button the user clicked to
	exit the dialog.  This will be one of G_OK_BUTTON, G_CANCEL_BUTTON,
	G_YES_BUTTON, or G_NO_BUTTON, also #defined below.  On failure, the
	function returns zero.
	
	The alert icon specification (icon) determines which icon (if any)
	will be displayed in the dialog box.  G_INFO_ALERT displays an
	"information" icon, G_WARNING_ALERT displays an "exclamation point"
	icon, and G_ERROR_ALERT displays a "stop sign" icon.  No other values
	will display an icon; note that the exact appearance and position
	of these icons is platform-dependent.

	The alert button specification (buttons) must be one of the values
	#defined below; if you give any other value, the dialog will not be
	displayed.  G_OK_ALERT will display a dialog with an "OK" button
	only; G_OK_CANCEL_ALERT will display a dialog with "OK" and "Cancel"
	buttons; and G_YES_NO_CANCEL_ALERT will display a dialog with "Yes",
	"No", and "Cancel" buttons.
	
	The G_SAVE_CANCEL_ALERT is a special variant of the "Yes-No-Cancel"
	alert which conforms to Apple's Interface Guidelines for the standard
	"Save Changes?" dialog.  If the user hits the "Save" button, the
	function returns G_SAVE_BUTTON; if the user hits the "Don't Save" button,
	the function returns G_DONT_SAVE_BUTTON; if the user hits the "Cancel"
	button, the function returns G_CANCEL_BUTTON.  In MS Windows, this
	alert brings up the standard 3-button "Yes-No-Cancel" message dialog
	box, where the functionality of the "Yes", "No", and "Cancel" buttons
	are equivalent to the "Save", "Don't Save", and "Cancel" buttons on the
	Mac.

	The parameter (text) is an ASCII NUL-terminated string containing the
	text you wish to display in the dialog.  It should be limited to 255
	characters in length (plus ther terminating NUL character.)
	
******************************************************************************/

int GDoAlert ( short, short, char * );

/*** Alert/MessageBox icon specifications ***/

#define G_INFO_ALERT			1
#define G_WARNING_ALERT			2
#define G_ERROR_ALERT			3

/*** Alert/MessageBox button specifications.  ***/

#define G_OK_ALERT				1
#define G_OK_CANCEL_ALERT		2
#define G_YES_NO_CANCEL_ALERT	3

#ifdef GMAC
#define G_SAVE_CANCEL_ALERT		6
#else
#define G_SAVE_CANCEL_ALERT		3
#endif

/*** Button return codes. ***/

#define G_OK_BUTTON				1
#define G_YES_BUTTON			1
#define G_SAVE_BUTTON			1
#define G_CANCEL_BUTTON			2
#define G_NO_BUTTON				3
#define G_DONT_SAVE_BUTTON		3

/***************************  GDoDialog  ***********************************

	Displays a dialog box with a prompt and allows the user to enter an
	answer.

	int GDoDialog ( char *prompt, char *text )

	(prompt): null-terminated string containing prompt to display in dialog.
	(text): null-terminated string containing text to display in edit field.

	The function returns TRUE if the user hit the dialog's "OK" button exit,
	or FALSE if the user hit the "Cancel" button or other failure occured.

	If the user hit the "OK" button and exited successfully, the text entered
	into the dialog's editable-text field by the user will be returned in
	the string (text).  Otherwise, the contents of (text) will be unchanged.

*****************************************************************************/

int GDoDialog ( char *, char * );

/*************************  GCreateDialog  ***************************

	Creates a modeless dialog window from a resource template.

	GWindowPtr GCreateDialog ( short id, long data, GEventProcPtr proc )

	(id): ID number of resource template to create dialog from.
	(data): application-defined data value to place in dialog.
	(proc): application-defined event-handling procedure for the dialog.

	The function returns a pointer to the dialog window, or NULL on failure.

	Unlike a modal dialog, the user can interact with any of the program's other
	windows while a modeless dialog is visible; it does not have to be closed
	first.  Additionally, a modeless dialog window receives its events from the
	application's main event loop; the program does not enter a separate loop
	to handle dialog events.  The event-handling function for your dialog window
	should look just like the event-handling function for any other window.

	There are some interface differences between the Mac's and Windows'
	implementation of the modeless dialog.  On the Mac, modeless dialog windows
	may be covered up by other windows.  In Windows, modeless dialog window(s)
	are always frontmost as long as the application is active.  (The status of
	the dialog- active or inactive- is indicated by the presence or lack of
	hilight in its title bar and window frame.)  Furthermore, a modeless
	dialog in a Windows application is not clipped to the application's frame
	window, unlike the application's other MDI child windows.  Finally, dialog
    windows are not added to the window menu (if there is one) on either platform.

	In Windows, you should make sure that the dialog has the style DS_MODALFRAME
	(which is customary for all Windows dialogs, modal and modeless.)

	If the dialog's resource template indicates that the dialog should be
	initially visible, it will be displayed on the screen before it receives
	its initial event (Mac) or right after (Windows).  If the resource
	template indicates that the dialog should not be initially visible, it
	will not appear until you explicitly show it by calling GShowWindow().
	
	There is no "GDeleteModelessDialog" function.  Use the GDeleteWindow()
	function to free memory for the dialog when you are done with it. 
	
******************************************************************************/

GWindowPtr GCreateDialog ( short, long, GEventProcPtr );

/***************************  GIsDialogWindow  ******************************

	Determines whether or not a window is a dialog window.

	int GIsDialogWindow ( GWindowPtr window )

	(window): pointer to a window.

	This function returns TRUE or FALSE, depending on whether or not the
	given window is a dialog window.  You may safely pass a NULL pointer
	to this function; in that case, it will simply return FALSE.
	
*****************************************************************************/

int GIsDialogWindow ( GWindowPtr );

/****************************  GShowModalDialog  **************************

	Displays and activates a modal dialog window.

	void GShowModalDialog ( GWindowPtr dialog )

	(dialog): pointer to modal dialog window to display; or NULL.

	This function displays and activates a modal dialog window, disabling
	the program's interface to prevent the user from interacting with
	any other part of the program while the modal dialog window is active.
	You should never have to call this function unless you want to
	implement all of the parts of a modal dialog loop manually; see
	GCreateDialog(), GEnterModalDialogLoop(), and GEnterModalDialog()
	for more info.

	After calling this function, the dialog window (dialog) will
	be frontmost and the rest of the application's interface (windows,
	menus) will be disabled.  You can then retrieve a pointer to the
	dialog window by calling GGetModalDialog().  Use the function
	GHideModalDialog() to hide the modal dialog window and re-enable the
	application's interface.

	Note that calls to GShowModalDialog() MAY be nested.  In other words,
	you may display one modal dialog window, then display another one
	on top of it.  In that case, GGetModalDialog() will return a pointer
	to the modal dialog window that was most recently displayed using
	this this function.  Make sure that each call to GShowModalDialog()
	is balanced by a subsequent call to GHideModalDialog(), though!
	
	You may pass NULL for the (dialog) parameter.  In this case, the
	function merely disables the application's interface without displaying
	or activating the dialog window.  If you do pass

	On the Macintosh, this function deactivates the program's active
	non-floating window, deactivates all of its floating windows, then
	displays and activates the modal dialog window.  It also disables
	the menus in the main menu bar.

	In Microsoft Windows, this function disables the application's main
	window (see GGetMainWindow()) and any application-owned windows in front
	of it.  It then displays and activates the modal dialog window.

*************************************************************************/

void GShowModalDialog ( GWindowPtr dialog );

/****************************  GHideModalDialog  **************************

	Hides a modal dialog window and re-enables the application's interface.

	void GHideModalDialog ( GWindowPtr dialog )

	(dialog): pointer to modal dialog window to display, or NULL.

	This function hides a modal dialog window and re-enables the program's
	interface, reversing the effects of the function GShowModalDialog().
	You should never have to call this function unless you want to
	implement all of the parts of a modal dialog loop manually; see
	GCreateDialog(), GEnterModalDialogLoop(), and GEnterModalDialog()
	for more info.

	You may pass NULL for the (dialog) parameter.  In this case, the
	function merely enables the application's interface without hiding
	or deactivating the dialog window.

	On the Macintosh, this function also re-activates the application's
	currently-visible floating windows.  It restores the menus in the main
	menu bar to their status before the preceding call to GShowModalDialog().

	In Microsoft Windows, this function enables the application's main
	window and any other application-owned windows in front of it.

*************************************************************************/

void GHideModalDialog ( GWindowPtr dialog );

/************************  GEnterModalDialogLoop  **************************

	Displays a modal dialog window and executes an event-handling loop.

	int GEnterModalDialogLoop ( GWindowPtr dialog )

	(dialog): pointer to dialog window to display before entering loop.

	The value returned by this function is the value your application passes
	to GExitModalDialogLoop() from within the dialog's event-handling procedure.

	This function implements a modal dialog loop.  It first displays and
	activates the specified dialog window (dialog) and disables the rest
	of your application's interface.  It then enters an event-handling
	loop, using the dialog window's event-handling procedure to process
	events related to it when they occurr.  Your application can cause the
	modal dialog loop to terminate by calling GExitModalDialogLoop(); when
	this happens, the modal dialog window is hidden and the rest of your
	application's interface is re-enabled.  This function then returns the
	value your application passed tp GExitModalDialogLoop().

***************************************************************************/

int GEnterModalDialogLoop ( GWindowPtr dialog );

/*************************  GExitModalDialogLoop  ***************************

	Quits a modal dialog loop.

	void GExitModalDialogLoop ( int result )

	(result): application-defined value to return from GEnterModalDialogLoop().

	This function causes the currently-executing modal dialog loop to
	terminate.  Do not call it unless you have entered a modal dialog
	loop by calling GEnterModalDialog() or GEnterModalDialogLoop().  The value
	you pass to this function will be the value returned by GEnterModalDialog()
    or GEnterModalDialogLoop().  

	Note that this function does not actually remove the dialog window
	from the screen or free any associated memory.  You can accomplish this
	by calling GHideModalDialog() and GDeleteWindow(), which is done auto-
	matically by both GEnterModalDialogLoop() and GEnterModalDialog().

*****************************************************************************/

void GExitModalDialogLoop ( int );

/****************************  GEnterModalDialog  ****************************

	Creates a modal dialog window, enters an event loop, and destroys the
    modal dialog window after the application exits the modal dialog loop.

	int GEnterModalDialog ( short id, long data, GEventProcPtr proc )

	(id): resource ID number of the dialog resource template.
	(data): application-defined custom data value to store in the dialog.
	(proc): application-defined event-handling procedure for dialog.

	The function returns the value your application passes to GExitModal-
	DialogLoop().  If for some reason it cannot create the dialog window,
	it will return zero before entering its event-handling loop.

	The arguments to this function are identical to the arguments to the
	GCreateDialog() function, and are used to create the modal dialog window.
	If successful, the function enters a modal-dialog-event-handling loop.
	After the application exits the loop, this function destroys the dialog
	window and returns the value your application passed to GExitModalDialog-
	Loop(),

****************************************************************************/

int GEnterModalDialog ( short, long, GEventProcPtr );

/***  GGetModalDialogWindow  ***********************************************

	Returns a pointer to the frontmost modal dialog window visible on 
	screen.
	
	GWindowPtr GGetModalDialogWindow ( void )
	
	This function returns a pointer to the modal dialog window that was
	most recently displayed using the function GShowModalDialog().  If
	no modal dialog window is present, the function returns NULL.

****************************************************************************/

GWindowPtr GGetModalDialogWindow ( void );

/*************************  GSetDialogItemText  ****************************

	Copies text into a control or text item in a dialog window.
	
	void GSetDialogItemText ( GWindowPtr dialog, short item, char *text )
	
	(dialog): dialog window containing control to copy text into.
	(item): item number of control or text item within dialog
	(text): buffer containing text to copy into the control.
	
	This function returns nothing.  The text buffer must contain a null-
	terminated string of no more than 255 characters.
	
****************************************************************************/
	
void GSetDialogItemText ( GWindowPtr, short, char * );

/************************  GGetDialogItemText  ******************************

	Obtains a copy of the text displayed in a control.
	
	void GGetDialogItemText ( GWindowPtr dialog, short item, char *text )
	
	(dialog): dialog window containing control displaying desired text.
	(item):item number of desired control or text item within dialog.
	(text): buffer to receieve copy of control's text.
	
	On return the buffer provided (text) will contain a copy of up to 255
	characters of the specified control's title or item's text.
	
*****************************************************************************/

void GGetDialogItemText ( GWindowPtr, short, char * );

/************************  GGetDialogItemRect  ******************************

	Obtains a dialog item's position and size in its parent window.

	void GGetDialogItemRect ( GWindowPtr dialog, short item, GRectPtr rect )

	(dialog): pointer to the dialog window.
	(item): number of desired item within dialog window.
	(rect): pointer to rectangle to receive coordinates.

	On return, (rect) will contain the item's coordinates in the frame of
	reference of its parent window.
	
*****************************************************************************/

void GGetDialogItemRect ( GWindowPtr, short, GRectPtr );

/*************************  GSetDialogItemRect  *****************************

	Moves and/or resizes a dialog item.

	void GSetDialogItemRect ( GWindowPtr dialog, short item, GRectPtr rect )

	(dialog): pointer to the dialog window.
	(item): number of desired item within the dialog window.
	(rect): pointer to rectangle containing desired coordinates for control.

	The coordinates in (rect) should be in the frame of reference of the
	control's parent window.

*****************************************************************************/

void GSetDialogItemRect ( GWindowPtr, short, GRectPtr );

/***********************  GSetDialogControlValue  **************************

	Modifies the value of a control in a dialog window.

	void GSetDialogControlValue ( GWindowPtr dialog, short item, short value )
	
	(dialog): pointer to a dialog window.
	(item): dialog item number of control item.
	(value): value for control.
	
	This function is equivalent to:
	
	GSetControlValue ( GGetControl ( dialog, item ), value );
	
****************************************************************************/

void GSetDialogControlValue ( GWindowPtr, short, short );

/***********************  GSetDialogControlValue  **************************

	Obtains the value of a control in a dialog window.

	void GSetDialogControlValue ( GWindowPtr dialog, short item, short value )
	
	(dialog): pointer to a dialog window.
	(item): dialog item number of control item.
	(value): value for control.
	
	This function is equivalent to:
	
	GGetControlValue ( GGetControl ( dialog, item ), value );
	
****************************************************************************/

short GGetDialogControlValue ( GWindowPtr, short );

/***********************  GSetSelectedDialogButton  ************************

	This function selects one button in a set of dialog items.
	
	void GSetSelectedDialogButton ( GWindowPtr dialog, short first,
	short last, short select )
	
	(dialog): pointer to dialog window containing buttons.
	(first): dialog item or control ID number of first button in set.
	(last): dialog item or control ID number of last button in set.
	(select): dialog item or control ID number of button to select.
	
	Use this function to implement typical radio-button behavior, where one
	button in a set of radio buttons is turned on and all the others are
	turned off.  If the item number of the button to turn on (select) is
	outside the specified range (first-last), all buttons in that range
	will be turned off.

****************************************************************************/

void GSetSelectedDialogButton ( GWindowPtr, short, short, short );

/**********************  GGetSelectedDialogButton  *************************

	Determines which button in a set of dialog items is selected.
	
	short GGetSelectedDialogButton ( GWindowPtr dialog, short first, short last )
	
	(dialog): pointer to dialog window containing buttons.
	(first): dialog item or control ID number of first button in set.
	(last): dialog item or control ID number of last button in set.
	
	The function returns the value of the first button in the specified
	range of items with a non-zero control value.  If no buttons in that
	range are selected, the function returns zero.
	
****************************************************************************/

short GGetSelectedDialogButton ( GWindowPtr, short, short );

/*************************  GShowDialogItem  *******************************

	Makesa hidden dialog item visible.
	
	void GShowDialogItem ( GWindowPtr dialog, short item )

	(dialog): pointer to a dialog window
	(item): dialog item or control ID number or item to make visible.
	
	If the specified dialog item is already visible, this function has
	no effect.
	
***************************************************************************/

void GShowDialogItem ( GWindowPtr, short );

/*************************  GHideDialogItem  *******************************

	Makesa visible dialog item hidden.
	
	void GHideDialogItem ( GWindowPtr dialog, short item )

	(dialog): pointer to a dialog window
	(item): dialog item or control ID number or item to hide.
	
	If the specified dialog item is already hidden, this function has
	no effect.
	
****************************************************************************/

void GHideDialogItem ( GWindowPtr, short );

/*************************  GSelectDialogItem  *****************************

	This function generates a control event for a button or edit-text field
	in a dialog window.
	
	void GSelectDialogItem ( GWindowPtr dialog, short item )
	
	(dialog): pointer to dialog window containing item.
	(item): item number of button or edit-text field within the dialog.
	
	Use this function to simulate the actions produced selecting a button
	or editable-text field inside a dialog window.  If the specified
	dialog item is a push button, check box, or radio button, GUILib briefly
	hilites the button.  If the item is an editable text field, GUILib selects
	the item's entire text.  In both cases, it then calls your application's
	event-handling function and passes it a control event indicating that the
	item has been clicked.  Note that you should never call this function
	in response to a control event for that item; otherwise your application
	would enter an endless recursive loop!
	
****************************************************************************/

void GSelectDialogItem ( GWindowPtr, short );

/*************************  GEnableDialogItem  *******************************

	Enables or disables a dialog item.
	
	void GEnableDialogItem ( GWindowPtr dialog, short item, int enable )

	(dialog): pointer to a dialog window.
	(item): dialog item or control ID number or item to hide.
	(enable): TRUE to enable item; FALSE to disable.
	
	This function returns nothing.
	
****************************************************************************/

void GEnableDialogItem ( GWindowPtr, short, int );

/*** functions in Screen.c ***/

/***  GGetScreenRect  ******************************************************

	Determines the coordinates of the rectangle enclosing the screen.
	
	GRectPtr GGetScreenRect ( GRectPtr rect )
	
	(rect): pointer to rectangle to receive screen's size and position.
	
	The function returns a pointer to the given rectangle.

	On a Macintosh with multiple monitors attached, this function returns
	the rectangle of the main monitor (i.e. that which contains the menu
	bar.)  In Windows, this function returns the rectangle enclosing the
	desktop area.
	
****************************************************************************/

GRectPtr GGetScreenRect ( GRectPtr );

/***  GGetDesktopRect  *******************************************************

	Determines the position and size of the destop area of the screen.
	
	GRectPtr GGetDesktopRect ( GRectPtr rect )
	
	(rect): pointer to rectangle to receive desktop size and position.
	
	The function returns a pointer to the given rectangle.  On return,
	the rectangle contains the position and dimensions of the desktop
	area of the main screen.  On the Macintosh, this is the main monitor's
	rectangle minus the menu bar at the top.  In Windows 3.1, this is the
	screen's entire rectangle.  In Windows 95/98/NT, this is the screen's
	entire rectangle minus the area occupied by the taskbar.
	
******************************************************************************/

GRectPtr GGetDesktopRect ( GRectPtr );

/***  GGetWorkspaceRect  *******************************************************

	Determines the position and size of the application's workspace area.
	
	GRectPtr GGetWorkspaceRect ( GRectPtr rect )
	
	(rect): pointer to rectangle to receive desktop size and position.
	
	The function returns a pointer to the given rectangle.  On return,
	the rectangle contains the position and dimensions of the application's
	workspace area.  The application's workspace is considered to be the
	total area where windows can be displayed.  On the Macintosh, this is
	the same as the desktop area.  In Windows, it is the same as the
	desktop area for single-window applications.  For multiple-window
	applications in Windows, the workspace is the rectangle occupied
	by the MDI frame window's content area.
	
******************************************************************************/

GRectPtr GGetWorkspaceRect ( GRectPtr );

/**************************  GGetScreenXXX ***********************************

	Returns the width and height of the main screen in pixels.

	short GGetScreenWidth ( void )
	short GGetScreenHeight ( void )
	
******************************************************************************/

short GGetScreenWidth ( void );
short GGetScreenHeight ( void );

/***  GGetScreenDepth  ******************************************************

	Returns the current bit-depth of the the main screen.

	short GGetScreenDepth ( void )
	
	This function returns the number of bits per pixel currently displayed
	on the main screen (the one containing the menu bar):
	
	1  for black & white screens
	2  for 4-color screens
	4  for 16-color screens
	8  for 256-color screens
	16 for thousands-of-color screens
	32 for millions-of-color screens
	
******************************************************************************/

short GGetScreenDepth ( void );

/**************************  GSetScreenDepth  *******************************

	Changes the number of bits per pixel on the main monitor.

	int GSetScreenDepth ( short depth )
	
	(depth): new bit-depth for main monitor, legal values are 1, 2, 4, 8,
	         16, or 32; see above
	         
	The function returns TRUE or FALSE depending on whether or not it was
	able to reset the main monitor's bit-depth to the requested value.  In
	Windows, this function always return FALSE.
		
	On the Macintosh, the main monitor is the one the menu bar is attached to.

******************************************************************************/

int GSetScreenDepth ( short );

/*** Functions in Graphics.c ***/

/***************************  GSetColor  **************************************

	Describes a color using the native operating system's representation.
	
	GColorPtr GSetColor ( GColorPtr color, unsigned char red, unsigned char green,
	unsigned char blue )
	
	(color): pointer to a color representation record.
	(red): red component (0 to 255)
	(green): green component (0 to 255)
	(blue): blue component (0 to 255)
	
	All visual colors can be represented by some combination of the "pure"
	colors: red, green, and blue.  Use this function to convert an RGB triplet
	to the native operating system's color representation.  Each component may
	take any value in the range 0 to 255.
	
********************************************************************************/
	
GColorPtr GSetColor ( GColorPtr, unsigned char, unsigned char, unsigned char );

/*******************************  GGetRed  *************************************

	Obtains the red, green, and blue components of a color in the native
	operating system's representation.
	
	unsigned char GGetRed ( GColorPtr color )
	unsigned char GGetGreen ( GColorPtr color )
	unsigned char GGetBlue ( GColorPtr color )
	
	(color): pointer to a color representation record.
	
	These functions return the red, green, and blue components of a color
	recorded in the native operating system's representation.
	
*********************************************************************************/
	 
unsigned char GGetRed ( GColorPtr );
unsigned char GGetGreen ( GColorPtr );
unsigned char GGetBlue ( GColorPtr );

/*******************************  Color constants *******************************

	These functions return pointers to records representing the "pure" colors
	in the native operating system's representation.
	
	GColorPtr GRedColor ( void );
	GColorPtr GGreenColor ( void );
	GColorPtr GBlueColor ( void );
	GColorPtr GCyanColor ( void );
	GColorPtr GYellowColor ( void );
	GColorPtr GMagentaColor ( void );
	GColorPtr GBlackColor ( void );
	GColorPtr GWhiteColor ( void );

	All of these functions return pointers to static color records.
	
*********************************************************************************/

GColorPtr GRedColor ( void );
GColorPtr GGreenColor ( void );
GColorPtr GBlueColor ( void );
GColorPtr GCyanColor ( void );
GColorPtr GYellowColor ( void );
GColorPtr GMagentaColor ( void );
GColorPtr GBlackColor ( void );
GColorPtr GWhiteColor ( void );

/****** Tim, Document these functions **********/

/***************************  GSetGraphicPort  ***************************/

void GSetGraphicPort ( GGraphicPtr hDC );

/***************************  GGetGraphicPort  ***************************/

GGraphicPtr GGetGraphicPort ( void );

/**************************  GStartDrawing ***********************************

	Initializes a window for subsequent drawing operations.
	
	void GStartDrawing ( GWindowPtr window )
	
	(window): pointer to window to start drawing into.
	
	This function returns nothing.  (Maybe it should return a handle to
	the window's device context (Windows) or GrafPort (Mac)?)
	
	You must call this function before you perform any graphical operations
	on a window.  When you are done drawing in the window, call GEndDrawing()
	to indicate that you are done drawing in the window.  Any graphics calls
	(e.g., GMovePenTo(), GDrawLineTo(), GSetClipRect(), etc.) will send their
	output to the window whose pointer you passed to GStartDrawing().
	
	If you wish to start drawing in another window, you must call GEndDrawing()
	for the window you are currently drawing in first.  Similarly, you may not
	call any of the other GStartDrawingXXX() functions (GStartDrawingPicture(),
	GStartDrawingPage(), etc.) until you have first called GEndDrawingWindow() to
	indicate that you are done drawing in the window.
	
******************************************************************************/

void GStartDrawing ( GWindowPtr );

/**************************  GEndDrawing ***********************************

	Ends drawing operations to a window.
	
	void GEndDrawing ( GWindowPtr window )
	
	(window): pointer to window to end drawing into.
	
	This function returns nothing.
	
	Call this function when you are done performing graphical operations on
	a window.  Do not call this function without first calling GStartDrawing(),
	and MAKE SURE THAT THE WINDOW POINTER YOU PASS IS THE SAME ONE YOU PASSED
	TO GStartDrawing()!!!
	
******************************************************************************/

void GEndDrawing ( GWindowPtr );

/**************************  GStartDrawingBitmap  ****************************

	Selects a bitmap as the device to receive graphical output.
	
	void GStartDrawingBitmap ( GWindowPtr window, GBitmapPtr bitmap )
	  
	(window): pointer to window to create offscreen bitmap for.
	(bitmap): pointer to bitmap which will receive drawing output.
	
	The function returns nothing.  

	After you make this call, all subsequent drawing operations (e.g.
	GDrawLine(), GDrawRectangle(), etc.) will output results to the
	contents of the bitmap object passed as the (bitmap) parameter.
	The bitmap will continue to serve as the graphical output device
	until you call GEndDrawingBitmap(); see below.
	
	The window parameter (window) will serve as storage for the associated
	drawing parameters (e.g. the color of the current pen, the current text
	font, etc.) while drawing is taking place.  Note that nothing will
	actually be drawn in the window; graphic output will all be drawn into
	the image.
	
	This function is very similar to GStartDrawingImage(); see below.
	
	IMPLEMENTATION NOTE:  Although I've typedef'd a GBitmapPtr to be a BitMap *
	on the Macintosh, this is only true on Macs that lack Color QuickDraw.
	On color Macs, the bitmap pointer is actually a handle which leads to an
	object of type PixMap (i.e., a PixMapHandle).  This is a highly platform-
	specific issue which your application code should never have to worry about...
	but now you know, just in case.
	
******************************************************************************/

void GStartDrawingBitmap ( GWindowPtr, GBitmapPtr );

/**************************  GEndDrawingBitmap  ******************************

	Ends drawing an offscreen bitmap.
	
	void GEndDrawingBitmap ( GWindowPtr window )
	
	(window): pointer to the on-screen window the bitmap was created from.
	
	This function returns nothing.
	
******************************************************************************/

void GEndDrawingBitmap ( GWindowPtr, GBitmapPtr );

/**************************  GStartDrawingPicture  **************************

	Creates a picture record and initializes it for drawing operations.
	
	GPicturePtr GStartDrawingPicture ( GWindowPtr window, GRectPtr rect )
	
	(window): pointer to any color window (needed as a placeholder)
	(rect): pointer to rectangle defining picture frame; will be used
	        to scale picture during playback.
	
	The function returns a pointer to an empty picture record, if
	successful, or NULL on failure.
	
	After you call this function, subsequent graphics calls will not
	produce any output in the specified window, but will instead be
	recorded into the new picture record.  When you are done recording
	the picture, call the function GEndDrawingPicture().  Note: if
	GStartDrawingPicture() returns NULL, you should not perform any
	drawing or call GEndDrawingPicture().
	
	Once you have finished recording the picture, you may play it back
	by calling GDrawPicture().  You can draw a picture into any graphic
	output device, such as a window, a bitmap, a printer page, or even
	another picture.  However, you cannot draw the picture until you have
	finished recording it by calling GEndDrawingPicture().
	
*****************************************************************************/

GPicturePtr GStartDrawingPicture ( GWindowPtr, GRectPtr );

/**************************  GEndDrawingPicture  **************************

	Ends recording drawing calls into a picture record.
	
	void GEndDrawingPicture ( GWindowPtr window, GPicturePtr picture )
	
	(window): pointer to window that was passed to GStartDrawingPicture().
	(picture): pointer to picture record returned by GStartDrawingPicture().
	
	Do not call this function if the preceding call to GStartDrawingPicture()
	returned an unsuccessful (i.e. NULL) result!
	
*****************************************************************************/

void GEndDrawingPicture ( GWindowPtr, GPicturePtr );

/***************************  GGetPen  ***************************************

	Obtains the attributes of the current pen.  
	
	void GGetPen ( GColorPtr color, short *size, short *fill )
	
	(color): receives pen's current color.
	(size): receieves pen's current width, in pixels.
	(fill): receives TRUE if currently drawing filled shapes, or FALSE if
	        currently drawing hollow shapes.

	This function returns nothing.  If you pass NULL for any item, its value
	will not be returned.
	
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!! 

******************************************************************************/

void GGetPen ( GColorPtr, short *, short * );

/***************************  GSetPen  ***************************************

	Sets the attributes of the current pen for subsequent drawing operations.  
	
	void GSetPen ( GColorPtr color, short size, short fill )
	
	(color): pointer to color record describing desired color for pen.
	(size): desired pen size, in pixels.
	(fill): desired pen fill pattern.
	
	This function returns nothing.  You can also use  GSetPenColor(),
	GSetPenSize(), etc. to change individual pen attributes, but if you
	need to change more than one pen attribute at once, this function is
	more efficient.

	The pen fill mode (fill) determines the effect of subsequent drawing
	operations.  If TRUE, subsequent drawing calls will draw filled shapes;
	if FALSE, they will draw hollow shapes.
	
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!! 
	
******************************************************************************/
 
void GSetPen ( GColorPtr, short, short );

/***************************  GSetPenColor  ***********************************

	Sets the current pen's color.
	
	void GSetPenColor ( GColorPtr color )
	
	(color): pointer to color record describing desired color for pen.
	
	Returns nothing.  The actual color that appears when you make subsequent
	drawing calls may not precisely match the color you pass; the results are
	both patform-, hardware-, and call-dependent.
	
	On the Mac, this function sets the pen color to the closest available RGB
	color value in the color table of display hardware the program is running
	on. On a black-and-white display, any color other than white appears as
	black.
	
	In Windows, outline shapes will be drawn in the closest available RGB
	color value.  For filled shapes, Windows will create an 8x8 dithered
	pattern which approximates the color you provided.  On a black-and-white
	display, any color darker than 50% gray appears black, and any color
	lighter than 50% gray appears white.

	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!! 

*******************************************************************************/
	
void GSetPenColor ( GColorPtr );

/****************************  GSetPenSize  ***********************************

	Sets the pen's size for future drawing operations.

	void GSetPenSize ( short size )
	
	(size): desired size for pen, in pixels.
	
	Returns nothing.  The pen size applies to lines and outline shapes.
	Setting the pen size to zero will hide the pen and prevent lines and
	outline shapes from being drawn.
	
	"Fat" pens get drawn somewhat differently between Mac and Windows.
	Windows uses a circular region with radius equal to the pen size, while
	the Mac uses a rectangular pen.  Therefore, a "Fat" line will have a
	round end in Windows and a square end on the Mac.
		
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!! 

*******************************************************************************/

void GSetPenSize ( short );

/*****************************  GSetPenFill ***********************************

	Determines whether shapes will be drawn filled or in outline only.
	
	void GSetPenFill ( short fill )
	
	(short): if TRUE, subsequent shapes will be draw filled;
	         if FALSE, they will be drawn in outline only.
	        
	Returns nothing.
		
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!! 

******************************************************************************/
	        
void GSetPenFill ( short );

/*****************************  GGetPenMode  *********************************

	Obtains the current pen transfer mode.
	
	short GGetPenMode ( void )
	
	Returns the current pen-transfer mode; see GSetPenMode() for more info.
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!! 
	
******************************************************************************/

short GGetPenMode ( void );

/*****************************  GSetPenMode  **********************************

	Sets the pen transfer mode for subsequent drawing operations.
	
	void GSetPenMode ( short mode )
	
	(mode): integer code describing desired pen mode.
	
	Returns nothing.  The pen transfer mode defines how the pen interacts with
	pixels already on the display.  GUILib #defines several pen transfer codes;
	the actual results can be quite complicated and dependent on both the platform
	and the color characteristics of the display.
	
	Consider the simplest example: a black & white display, in which all pixel
	values can be represented by either a zero or a one.  The pen color (either
	black or white) can also be represented by a zero or one.  The pen's
	intraction with background pixels can thus be represented by a boolean
	logical operation; -which- operation is defined by the transfer mode:
	
	G_COPY_PEN		G_COPY_NOT_PEN	G_OR_PEN		G_OR_NOT_PEN	G_XOR_PEN	
	
	Pen     0 1		Pen     0 1		Pen     0 1		Pen     0 1		Pen     0 1
	        - -		        - -		        - -		        - -		        - -
	Pix 0 | 0 1		Pix 0 | 1 0		Pix 0 | 0 1		Pix 0 | 1 0		Pix 0 | 0 1
	    1 | 0 1		    1 | 1 0		    1 | 1 1		    1 | 1 1		    1 | 1 0

	The G_COPY_PEN mode is fairly straightforward: if the pen is 0, the back-
	ground pixel gets set to 0, and if the pen is 1, the background pixel gets
	set to one.  G_NOT_COPY_PEN is also fairly straightforward; it's just the
	opposite of G_COPY_PEN: it puts a 1 where the pen is 0, and a 0 where the
	pen is 1.  G_OR_PEN puts a 1 everywhere, except where the pen and background
	are both zero.  G_OR_NOT_PEN is the equivalent of G_OR_PEN with the inverse
	of the pen.  G_XOR_PEN always inverts the background where the pen is 1, and
	leaves it unchanged there the pen is zero.
	
	This fairly straightforward.  The first complication comes when we try to
	map logical 0s and 1s with real black/white color values: on the Mac,
	a "0" pixel is white and a "1" pixel is black, while in Windows, a "1"
	pixel is white and a "0" pixel is black.  Thus, drawing with a black pen
	in XOR mode will invert the background (on Mac) but do nothing in Windows;
	to invert the background there, you'd have to draw with a white pen in
	XOR mode.
	
	It gets even more complicated when you have a color display in which pixels
	are not just 0s and 1s, but whole collections of bits (and worse yet, in
	Windows, you can have displays which are organized into separate color
	"planes" with multiple bits per pixel.)  Given these difficulties, you
	should stick to simple transfer modes if you want conistent results on
	both platforms.  G_COPY_PEN always draws the pen color; G_COPY_NOT_PEN
	always draws the opposite of the pen color.  GUILib also #defines a pen
	mode, G_INVERTING_PEN, which always inverts the background regardless of
	the pen's color (this is very useful: to "undraw" a shape you've drawn in
	inverting mode, just draw it again!)
		
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!! 

*******************************************************************************/
	
void GSetPenMode ( short );

/*** Drawing modes ***/

#ifdef GMAC

#define G_COPY_PEN		patCopy
#define G_OR_PEN		patOr
#define G_XOR_PEN		patXor
#define G_NOT_COPY_PEN	notPatCopy
#define G_NOT_OR_PEN	notPatOr
#define G_NOT_XOR_PEN	notPatXor

#define G_INVERTING_PEN	patXor

#endif

#ifdef GWIN

#define G_COPY_PEN		R2_COPYPEN
#define G_OR_PEN		R2_MERGEPEN
#define G_XOR_PEN		R2_XORPEN
#define G_NOT_COPY_PEN	R2_NOTCOPYPEN
#define G_NOT_OR_PEN	R2_NOTMERGEPEN
#define G_NOT_XOR_PEN	R2_NOTXORPEN

#define G_INVERTING_PEN	R2_NOT

#endif

/****************************  GSetPenFont  **********************************

	Sets the font typeface, font size, and font style, to use in
	subsquent text-drawing.

	void GSetPenFont ( char *name, short size, short style )

	(name): name of desired font
	(size): desired font height
	(style): desired font style bit flags.

	The function returns nothing.  Never call this function without first
	calling one of the GStartDrawingXXX() routines to initialize graphic
	output!

	Fonts are one of the more confusing and inconsistent aspects of both the
	Mac and Windows APIs, and consequently this function has a fair amount of
	platform-dependent behavior.  Read carefully!

	Specify the desired typeface (e.g. "Arial", "Helvetica", "Symbol") by
	passing its name in the argument (name) as a null-terminated string.

	The font-size argument (size) indicates the desired font size.  Again, this
	is somewhat platform-dependent.  On the Mac, it indicates the font height
	in "points"; in Windows, it indicates the font height in pixels.  Theortically,
	this should produce the same result, since a typographical point is 1/72",
	and the VGA video display resolution is typically 72 pixels/inch.  However,
	in real life these numbers may differ from machine to machine, especially
	in Windows.  The only definitive way to obtain the font size is to use
	the GGetStringSize() function.

	The font-style argument (style) consists of a series of bit-flags
	indicating the style (or combination thereof) in which you want the
	font to appear.  The only font styles which both Mac and Windows supports
	are plain, bold, italic, and underline.  Windows does not support the
	Mac's outline, shadow, condensed, or extended font styles, and the Mac
	does not support Windows' strikeout style.  If you use a font style which
	is not supported on one or the other platform, your font will simply not
	appear in that style on that platform.

	Finally, it may just be that the font you want is simply not installed
	on the machine you're working with.  In that case, both Mac and Windows
	try to substitute as closely a matching font as possible, but as you might
	expect, the result of this situation is highly platform-dependent.  The
	only fonts which are standard on both machines are the system font (Chicago
	on Mac) and the true-type fonts Helvetica (Arial in Windows), Courier
	(Courier New), Times (Times New Roman), Symbol (Symbol), and Mobile
	(Wingdings).  Other than this, the only reliable option is to let the user
	specifically pick the typeface, chracter set, font size, and style.
	In Windows, the built in "choose font" dialog does this; see GDoFontDialog().
	The Mac has no standard font-selection dialog; Mac programs usually use
	a "Font" menu (and "Size" and "Style" menus, which may or may not be
	separate.)  This is one area where your program should and in fact will
	have to work differently on different platforms.

	If you only want to change a single aspect of the pen font (i.e. just
	name/character set or just style) you can use one of the functions below.

******************************************************************************/

void GSetPenFont ( char *, short, short );
void GSetPenFontName ( char * );
void GSetPenFontSize ( short );
void GSetPenFontStyle ( short );

/*** Font style codes.  These values are identical to those defined in the
	 Apple Universal Headers (except strikeout); many are unsupported in
	 Windows.  You can create combined font style codes (e.g. bold + italic)
	 by combining the appropriate flags with a bitwise OR. ***/

#define G_PLAIN_STYLE		0x0000
#define G_BOLD_STYLE		0x0001
#define G_ITALIC_STYLE		0x0002
#define G_UNDERLINE_STYLE	0x0004
#define G_OUTLINE_STYLE		0x0008	/* unsupported in Windows */
#define G_SHADOW_STYLE		0x0010  /* unsupported in Windows */
#define G_CONDENSED_STYLE	0x0020  /* unsupported in Windows */
#define G_EXTENDED_STYLE	0x0040  /* unsupported in Windows */
#define G_STRIKEOUT_STYLE	0x0080  /* unsupported in Mac */

/***********************  GGetPenFont  *****************************************

	Obtains the current text-drawing attributes (face name, size, style).

	void GGetPenFont ( char *name, short *size, short *style )

	(name): receives name of current typeface as a C-style string.
	(size): receives current font size.
	(style): receives integer code indicating current font style bit flags.

	Returns nothing.  Never call this function without first calling one
	of the GStartDrawingXXX() routines to initialize graphic output!

	See GSetPenFont() for more information.

*****************************************************************************/

void GGetPenFont ( char *, short *, short * );

/***************************  GSetClipRect  ********************************

	Clips subsequent drawing in the current graphic port to a specified
	rectangle.

	void GSetClipRect ( GRectPtr rect )

	(rect): pointer to rectangle defining desired clipping region,
	        in units of pixels in the local coordinate frame.

	To end clipping, call GSetNullClipRegion().

	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!

******************************************************************************/

void GSetClipRect ( GRectPtr );

/***************************  GSetClipRegion  ********************************

	Clips subsequent drawing in the current window to the specified region.

	void GSetClipRegion ( GRegionPtr region )

	(region): pointer to the desired clipping region.

	To end clipping, call GSetNullClipRegion().

	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!

******************************************************************************/

void GSetClipRegion ( GRegionPtr );

/*************************  GSetNullClipRegion  *******************************

	Prevents subsequent drawing from being clipped.

	void GSetNullClipRegion ( void )

	Call this function when you wish to remove a clipping region you have
	previously set with GSetClipRect() or GSetClipRegion().

*******************************************************************************/

void GSetNullClipRegion ( void );

/************************  GGetPenPosition  ***********************************

	Moves the pen to a specified position.
	
	void GGetPenPosition ( short *x, short *y )

	(x): receives pen's horizontal coordinate, in pixels.
	(y): receives pen's vertical coordinate, in pixels.
	
	This function determines the current position of the pen, which is the
	point that LineTo() calls will start drawing lines from.  The parameters
	(x) or (y) can be NULL if you wish to ignore the horizontal or vertical
	coordinate.
	
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!	
	
********************************************************************************/

void GGetPenPosition ( short *, short * );

/***************************   GMovePenTo  **************************************

	Moves the pen to a specified location.
	
	void GMovePenTo ( short x, short y )

	(x,y): coordinates of point to move pen to, in pixels.
	
	The location you move the pen to specifies the starting point for lines
	drawn with subsequent DrawLineTo() calls.  (Text, too?)
	
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!	
	
********************************************************************************/

void GMovePenTo ( short, short );

/***************************   GDrawLineTo  **************************************

	Draws a line from the pen's current position to specified coordinates.
	
	void GDrawLineTo ( short x, short y )

	(x,y): coordinates of endpoint for line, in pixels.
	
	This function changes the pen's position, so after calling it, the pen is
	moved to (x,y).  You can set the pen's position without drawing a line by
	calling GMovePenTo().  You can determine the pen's current position using
	the function GGetPenPosition().
	
	On the Macintosh, the line drawn by this function will include the pixel
	whose coordinates are given in (x,y).  In Windows, the line will be drawn
	up to but not including the final pixel (x,y).  This is due to the algorithm
	used by the underlying Windows GDI calls used by GUILib.  To maintain
	consistent line-drawing across both platforms, you may wish to use the
	function GDrawLine() instead.


	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!	
	
********************************************************************************/

void GDrawLineTo ( short, short );

/***************************  GDrawLine  ***************************************

	Draws a line between two points.
	
	void GDrawLine ( short x0, short y0, short x1, short y1 )
	
	(x0,y0): coordinates of starting point for line.
	(x1,y1): coordinates of endpoint for line.
	
	This function is an alternative to GMovePenTo()/GDrawLineTo() for drawing
	individual line segments.  The line is drawn width the current pen color,
	size, and mode.  After calling this routine, the pen moves to (x1,y1).

	In Microsoft Windows, this function moves the endpoint coordinate one
	pixel away from the starting point before calling the native Windows GDI
	line-drawing routine if the endpoint's horizontal or vertical coordinates
	are different from the starting point's.  This is to compensate for the fact
	that Windows draws lines up to but not including the ending pixel, while the
	Macintosh draws lines which include the final pixel.  Using this funtion will
	ensure a consistent appearance on both platforms.  If you do not wish to have
	GUILib modify the coordinates of the endpoint you pass, you should cal
	GMovePenTo()/GDrawLineTo() instead of this function.
	
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!	
	
********************************************************************************/

void GDrawLine ( short, short, short, short );
	
/****************************  GDrawRectangle  **********************************

	Draws a rectangle.
	
	void GDrawRectangle ( short left, short top, short right, short bottom )

	(left): coordinate of left side of rectangle.
	(top): coordinate of top side of rectangle.
	(right): coordinate of right side of rectangle.
	(bottom): coordinate of bottom side of rectangle.
	
	The function returns nothing.
	
	On both platforms, the rectangle will be drawn up to but not including the
	(right,bottom) coordinates; see GRectangle() for more explanation.
	
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!
	
********************************************************************************/

void GDrawRectangle ( short, short, short, short );

/***************************  GDrawRect  ***************************************

	Draws a rectangle defined by a rect structure.
	
	void GDrawRect ( GRectPtr rect )
	
	(rect): pointer to a rectangle structure.
	
	This function works just like GDrawRectangle(), except that the coordinates
	defining the rectangle are contained inside the rectangle structure (rect)
	rather than specified individually.  See GDrawRectangle() for more info.
	
*********************************************************************************/

void GDrawRect ( GRectPtr );

/***************************   GDrawEllipse  ************************************

	Draws an ellipse.
	
	void GDrawEllipse ( short left, short top, short right, short bottom )

	(left): coordinate of left side of rectangle enclosing ellipse.
	(top): coordinate of top side of rectangle enclosing ellipse.
	(right): coordinate of right side of rectangle encloseing ellipse.
	(bottom): coordinate of bottom side of rectangle enclosing ellipse.

	On both platforms, the rectangle will be drawn up to but not including the
	(right,bottom) coordinates; see GDrawRectangle().
	
	On the Mac, the rectangle's right-side coordinate must be greater than its
	left, and its bottom-side must be greater than its top, or it will not be
	drawn.  Settings its values with GSetRect() will ensure this.
	
	In Windows, the width of the ellipse must be greater than one, or nothing
	will be drawn.  To draw a single-pixel dot, call the function GDrawDot().
	  
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!

********************************************************************************/

void GDrawEllipse ( short, short, short, short );

/******************************  GDrawDot  ************************************

	Draws a single-pixel dot.
	
	void GDrawDot ( short col, short row )

	(col): horizontal coordinate of dot.
	(row): vertical; coordinate of dot.

	This function draws a circular dot one pixel across.
		
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!

********************************************************************************/

void GDrawDot ( short, short );

/****************************  GDrawArc  **************************************

	Draws an elliptical arc or pie-shaped wedge.

	void GDrawArc ( GRectPtr rect, short x1, short y1, short x2, short y2 )

	(rect): contains coordinates of rectangle bounding the ellipse.
	(x1,y1): endpoint of ray from (rect)'s center defining start of arc.
	(x2,y1): endpoint of ray from (rect)'s center defining end of arc.

	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!

*******************************************************************************/

void GDrawArc ( GRectPtr, short, short, short, short );

/**************************  GCreatePolygon  *********************************

	Allocates memory for and initializes a new polygon record.

	GPolygonPtr GCreatePolygon ( short points, short x[], short y[] )

	(points): number of points (vertices) in the polygon.
	(x): array containing X coordinates of polygon's vertices.
	(y): array containing Y coordinates of polygon's vertices.

	The function returns a pointer to the initialized polygon record, if
	successful, or NULL on failure.  On return, the polygon record will
	contain the coordinates of the points you provided.  When the polygon
	is drawn (GDrawPolygon()) the points will be connected in the order
	you gave them in the array.

	Although this function does not actually draw anything, you must still
	make sure you first call one of the GStartDrawingXXX() functions to
	initialize graphical operations!!!

*****************************************************************************/

GPolygonPtr GCreatePolygon ( short, short [], short [] );

/**************************  GDeletePolygon  *********************************

	Destroys a polygon record and frees all associated memory.

	void GDeletePolygon ( GPolygonPtr polygon )

	(polygon): pointer to the polygon record you wish to destroy.

	On return, the pointer you pass will be invalid.

******************************************************************************/

void GDeletePolygon ( GPolygonPtr );

/*************************  GDrawPolygon  ************************************

	Draws a polygon.

	void GDrawPolygon ( GPolygonPtr polygon )

	(polygon): pointer to polygon record describing the polygon you wish to draw.

	The polygon will be drawn in the pen's current color, size, transfer mode,
	and fill mode.  If you are just drawing an outline polygon, it may be more
	efficient to use a series of GMovePenTo()/GDrawLineTo() calls.

	You can define a new polygon record with GCreatePolygon(); when you are
	done with it, use GDeletePolygon() to destroy it.

	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!

******************************************************************************/

void GDrawPolygon ( GPolygonPtr );

/**************************  GDrawIcon  ************************************/

void GDrawIcon ( GIconPtr, short, short );

/**************************  GDrawString  **********************************

	Draws a string using the current font.
	
	void GDrawString ( char *string, short length, short left, short top )
	
	(string): buffer containing null-terminated string to draw.
	(length): number of characters to draw; 0 = whole string.
	(left): horizontal coordinate to begin drawing string at.
	(top): vertical coordinate to begin drawing string at.
	
	The string is drawn with the current text font/size/style attributes;
	you can obtain these with GGetFont() and change them with GSetFont().

	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!

*******************************************************************************/

void GDrawString ( char *, short, short, short );

/**************************  GGetStringSize  **********************************

	Determines the width and height of a string drawn in the current font.

	void GGetStringSize ( char *string, short length, short *width, short *height )

	(string): pointer to buffer containing null-terminated string.
	(length): number of characters to use; 0 = entire string.
	(width): receives width of specified characters, in pixels.
	(height): receives height of specified characters, in pixels.

	The width and height are computed using the current text font/size/style
	attributes; you can obtain these with GGetFont() and change them with
	GSetFont().
	
	On the Mac, the height returned by this function is the number of pixels
	between lines of text drawn in the current font, alhough no character in
	the string may actually be this tall.

	If you do not wish to compute (width) or (height), you may pass NULL for
	either one (or both!)
	
	NEVER call this function without first calling one of the GStartDrawingXXX()
	functions to initialize graphical operations!!!

*******************************************************************************/

void GGetStringSize ( char *, short, short *, short * );

/******************************  GCreateBitmap  *******************************

	Creates a bitmap and allocates memory for its image data array.
	
	GBitmapPtr GCreateBitmap ( GRectPtr rect )
	
	(rect): pointer to rectangle containing desired dimensions for bitmap.
	
	The function returns a pointer to the bitmap, if successful, or NULL
	otherwise.
	
	If successful, the bitmap returned will have the same bit-depth and
	color organization as the main monitor.  It can be drawn into any
	window, picture, or even into another bitmap using the function
	GDrawBitmap().  To delete the bitmap, call GDeleteBitmap().
	
*******************************************************************************/

GBitmapPtr GCreateBitmap ( GRectPtr );

/**************************  GDeleteBitmap  **********************************

	Destroys a bitmap and frees all associated memory.
	
	void GDeleteBitmap ( GBitmapPtr bitmap )
	
	(bitmap): pointer to the bitmap you wish to destroy.
	
	This function returns nothing.  After you call it, the pointer you
	passed will be invalid.
	
******************************************************************************/

void GDeleteBitmap ( GBitmapPtr );

/************************  GGetBitmapRect  ********************************

	Obtains a bitmap's size and dimensions.
	
	GRectPtr GGetBitampRect ( GBitmapPtr bitmap, GRectPtr rect )

	(bitmap): pointer to a bitmap
	(rect): pointer to rectangle to recieve bitmap's size and position.
	
	The function returns a pointer to the given rectangle.
		
***************************************************************************/

GRectPtr GGetBitmapRect ( GBitmapPtr, GRectPtr );

GPtr GGetBitmapData ( GBitmapPtr, short *, short * );
void GSetBitmapData ( GBitmapPtr, short, short, GPtr );

/***************************  GDrawBitmap  ***********************************

	Draws a bitmap (or a rectangular section thereof), optionally scaling it.
	
	voidf GDrawBitmap ( GBitmapPtr bitmap, GRectPtr source, GRectPtr dest )
	
	(bitmap): pointer to the bitmap
	(source): pointer to rectangle defining portion of bitmap to draw;
	          a NULL pointer will use the bitmap's entire rectangle.
	(dest): pointer to a rectangle defining the area to draw the bitmap
	        into; a NULL pointer will use the same rectangle as (source).
	
	This function returns nothing.
	
	The source rectangle defines the portion of the bitmap to draw; for
	instance, a source rect of { 10, 20, 110, 220 } includes pixels in
	columns 10-110 and rows 20-220.
	
	The destination rectangle defines the area of your current graphic
	output device (window, printer page, etc.) that you wish to draw the
	bitmap into.  By giving a destination rectangle of different dimensions
	than the source rectangle, you can shrink or enlarge the bitmap by
	arbitrary amounts in both dimensions.  For example, using a destination
	rectangle of { 50, 60, 250, 160 } with the source rectangle described
	above would stretch the bitmap by a factor of 2 horizontally and shrink
	it by a factor of 2 vertically.  See GStartDrawingBitmap() for an example
	of usage.
	
******************************************************************************/
	   
void GDrawBitmap ( GBitmapPtr, GRectPtr, GRectPtr );

/******************************  GDrawPicture  ******************************

	Draws a picture record by "playing back" its contents, with optional
	scaling.
	
	void GDrawPicture ( GPicturePtr picture, GRectPtr rect )
	
	(picture): pointer to the picture record.
	(rect): pointer to rectangle describing area to draw the picture into;
	        passing NULL will use the picture's internal frame.
	        
	This function returns nothing.  It draws a picture by "playing back" the
	record of graphics calls it stored during picture definition.  You can
	draw a picture into any graphic output device (e.g. a window on screen,
	a printer page, an offscreen bitmap, or even another picture record);
	make sure that your call to GDrawPicture() falls between the appropriate
	GStartDrawingXXX() and GEndDrawingXXX() functions!
	
	The rectangle you pass to GDrawPicture() defines the area of the graphic
	output object which the picture will be drawin into.  Passing NULL will
	use the same rectangle as the picture's own internal frame.  You can
	arbitrarily reposition and scale the picture by passing a different
	rectangle to GDrawPicture().  Unlike scaling a bitmap, scaling a picture
	does not result in any loss of resolution: lines, circles, etc. expand
	and shrink smoothly because the native OS is reproducing the actual
	line/circle/etc. drawing calls at different coordinates.
	
	When you are done with a picture, call GDeletePicture() to destroy it.
			
*****************************************************************************/

void GDrawPicture ( GPicturePtr, GRectPtr );

/******************************  GDeletePicture  ****************************

	Destroys a picture record and frees all associated memory.
	
	void GDeletePicture ( GPicturePtr picture )
	
	(picture): pointer to the picture record to destroy.
	
	After you have called this function, the pointer you passed to it
	will be invalid.

*****************************************************************************/

void GDeletePicture ( GPicturePtr );

/*************************  GPictureToBitmap  ****************************

	Creates a bitmap and draws a picture into it.
	
	GBitmapPtr GPictureToBitmap ( GPicturePtr picture )
	
	(picture): pointer to picture to draw into the bitmap.
	
	The function returns a pointer to the newly-created bitmap, if
	successful, or NULL on failure.
	
	Use this function to convert a graphic stored as a picture into
	the equivalent bitmap.  On return, the bitmap will have the same
	dimensions as the picture's default rectangle.  It can be drawn,
	deleted, etc. using the same bitmap function (GDrawBitmap(),
	GDeleteBitmap(), etc.) as for any other bitmap.
	
****************************************************************************/

GBitmapPtr GPictureToBitmap ( GPicturePtr );

/***************************  GPictureToBitmap  ****************************

	Creates a picture and draws a bitmap into it.
	
	GPicturePtr GBitmapToPicture ( GBitmapPtr bitmap )
	
	(bitmap): pointer to bitmap to draw into the picture.
	
	The function returns a pointer to the newly-created picture, if
	successful, or NULL on failure.
	
	Use this function to convert a graphic stored as a bitmap into
	the equivalent picture.  On return, the picture will have the same
	dimensions as the bitmap's rectangle.  It can be drawn, deleted, etc.
	using the same functions (GDrawPicture(), GDeletePicture(), etc.) as
	for any other picture.
	
****************************************************************************/

GPicturePtr GBitmapToPicture ( GBitmapPtr );
 
/****************************  GDoFontDialog  *******************************

	Displays the standard Windows font-selection dialog.

	int GDoFontDialog ( char *font, short *size, short *style, GColor *color )

	(font): contains font name, as a null-terminated string.
	(size): contains font size.
	(style): contains bit-flags indicating desired font style.
	(color): contains desired text color; or NULL if none.

	The function returns TRUE if the user hits the dialog's "OK" button
	and exits successfully.  If the user hits the "Cancel" button,
	the function returns FALSE and none of the input parameters are
	modified.

	On input, the values stored in the above parameters are used to
	initialize the fields of the dialog.  On return, if the user hit
	the "OK" button, they recieve the values selected by the user when
	he/she exited the dialog.

	If you pass NULL for the (color) argument, GUILib will initialize the
	text color displayed in the dialog to black, and ignore the color
	chosen by the user.

	This function is not implemented on the Macintosh because the Mac has
	no stadard, built in font-selection dialog.  However, GUILib provides
	support for standard Macintosh "Font", "Size", and "Style" menu behavior;
	see GDoFontMenuItem().	
	
*****************************************************************************/

int GDoFontDialog ( char *, short *, short *, GColorPtr );

/**************************  GDoColorDialog  ********************************

	Displays the standard color picker dialog, which lets the user select an
	RGB color value.
	
	int GDoColorDialog ( char *prompt, GColor *color )
	
	(prompt): null-terminated string containing prompt to display in dialog.
	(color): on input, contains initial color to be displayed in dialog;
	         on output, receives color selected by user.
	         
	The function returns TRUE if the user hit the dialog's "OK" button to
	exit; it returns FALSE if the user hit cancel or other failure occurred.
	
*****************************************************************************/

int GDoColorDialog ( char *, GColor * );

long GGetSystemColors ( GColor[], long, long );
long GSetSystemColors ( GColor[], long, long, int );

/*** functions in Files.c ***/

/*****************************  GCreatePath  *********************************

	Allocates an initializes a new path specification record.
	
	GPathPtr GCreatePath ( GPathPtr path )
	
	(path): if non-NULL, pointer to an existing path specification to duplicate.
	
	The function returns a pointer to the new path specification record if
	successful, or NULL on failure.  If the (path) parameter was NULL, the
	new path specification record will be zero-initialized; otherwise, it
	will contain a copy of the path specification given in (path).
	
******************************************************************************/

GPathPtr GCreatePath ( GPathPtr path );

/*****************************  GDeletePath  *********************************

	Frees memory for a path specification record.
	
	void GDeletePath ( GPathPtr path )
	
	(path): pointer to path specification record to delete.
	
	Only use this function to free memory for path specification records
	that have been allocated by the GDoOpenFileDialog(), GDoSaveFileDialog(),
	and GCreatePath() functions!

	Note that this function does not actually delete any files from disk;
	it only flushes a path specification record from memory.
		
******************************************************************************/

void GDeletePath ( GPathPtr );

/*****************************  GCopyPath  **********************************

	Copies one path specification into another.
	
	void GCopyPath ( GPathPtr dstPath, GPathPtr srcPath )
	
	(dstPath): pointer to destination path specification record.
	(srcPath): pointer to source path specification record.
	
	On return, the destination path specification record will be
	overwritten with the source path specification record.
		
******************************************************************************/

void GCopyPath ( GPathPtr, GPathPtr );

/*****************************  GComparePaths  *******************************

	Compares one path specification with another.
	
	void GComparePaths ( GPathPtr path1, GPathPtr path2 )
	
	(path1): pointer to first path specification record.
	(path2): pointer to second path specification record.
	
	The function returns TRUE if the path specification records refer to
	the same object, and FALSE otherwise.
		
******************************************************************************/

int GComparePaths ( GPathPtr, GPathPtr );

/*****************************  GPathExists  **********************************

	Determines whether or not an object described by a path specification
	actually exists.
	
	int GPathExists ( GPathPtr path )
	
	(path): pointer to path specification record.
	
	The function returns TRUE if the path specification provided describes
	a file or directory which is actually present, and returns FALSE if not.
		
******************************************************************************/

int GPathExists ( GPathPtr );

/*****************************  GFileExists  **********************************

	Determines whether or not a file described by path specification record
	actually exists.
	
	int GFileExists ( GPathPtr path )
	
	(path): pointer to path specification record.
	
	The function returns TRUE if the path specification provided describes
	a file which is actually present.  It returns FALSE if no object exists
	at the specified path, or if the object is a directory rather than a file.
		
******************************************************************************/

int GFileExists ( GPathPtr );

/**************************  GDirectoryExists  *******************************

	Determines whether or not a directory described by path specification
	record actually exists.
	
	int GDirectoryExists ( GPathPtr path )
	
	(path): pointer to path specification record.
	
	The function returns TRUE if the path specification provided describes
	a directory which is actually present.  It returns FALSE if no object
	exists at the specified path, or if the object is a file rather than a
	directory.
		
******************************************************************************/

int GDirectoryExists ( GPathPtr );

/**************************  GPathIsReadOnly  *******************************

	Determines whether or not a file or directory described by path
	specification record is read-only.
	
	int GPathIsReadOnly ( GPathPtr path )
	
	(path): pointer to path specification record.
	
	The function returns TRUE if the path specification provided describes
	a file or directory which is read-only.  It returns FALSE if no object
	exists at the specified path, or if the object is not read-only.
	
	On the Macintosh, this function uses the "locked" bit of the file's
	Finder flags to determine whether or not it is read-only, i.e. if the
	file is locked, it is considered to be read-only.  Additionally, the
	function checks to see if the volume on which the file resides is locked,
	and if so, it returns TRUE even if the file itself is not locked.  This
	covers cases such as an unlocked file on a CD-ROM or a remotely mounted
	volume without write permission (which really are read-only files!)
		
******************************************************************************/

int GPathIsReadOnly ( GPathPtr );

/*************************  GDoOpenFileDialog  *******************************

	Displays the standard "Open file..." dialog and prompts the user for the
	name of a file to open.

	GPathPtr GDoOpenFileDialog ( char *prompt, char *filter, short *format )

	(prompt): C-style string comtaning prompt to display in dialog
	(filter): buffer containing file type filters
	(format): indicates which file type filter should be used.

	If the user hits the dialog's "OK" button, the function allocates a new
	path specification record, copies the selected file's path specification
	into it, and returns a pointer to it.  If the user cancels or an error
	occurs, the function returns NULL.

	In the Windows version of GUILib, the strings in the (filter) buffer must
	come in pairs and be separated by the character '|'.  The first string in
	each pair is a description; the second is the filter itself.  (Implementation
	note: GUILib converts the '|' character to an ASCII null character internally;
	this ensures that the pairs of strings are stored contiguously in the buffer.)
	For example, to store the following set of file filters:

	"Text Files"	"*.txt"
	"Bitmap Files"	"*.bmp"

	the contents of the filter buffer would look like this:

	char *filter = "Text Files|*.txt|Bitmap Files|*.bmp|";

	On the Mac, the Mac file types you want to display in the dialog are stored
	in the (filter) string as 4-character codes.  The first string in each pair
	is the description that will appear in the dialog's file type popup menu;
	the second is the file type itself.  To display files of any Mac file type,
	use '****' as the file type.  For example, the following filter would allow
	the "Open File" dialog to display files of type 'TEXT' and 'PICT' as
	"Text files" and "Picture files": 
	
	char *filter = "Text files|TEXT|Picture files|PICT|All files|****|";

	You can display the "Open" dialog without any file type popup menu
	by passing an empty (but not NULL) string for (filter).
	
	To keep your code platform-independent, you should probably store the
	filter string in a resource, which can then be different on Mac and
	Windows.
	
	The value in the (format) parameter indicates which of the items in the
	(filter) string will be initially used to display files.  On a successful
	return, the value placed into (format) will be the number of the item in
	this menu which was actually selected by the user.  This number is counted
	from 1 (for the first item) to the total number of items contained in the
	(filter) string.

	On the Macintosh, this function uses the Navigation Services "Open"
	dialog if it is available.  Navigation Services are available on Mac OS
	>= 8.5.  If otherwise, GUILib uses the Standard File package.  In that
	case, the "Open" dialog displayed by this function has no file type menu;
	instead, the dialog will display files of any of the file types contained
	in the filter string (as described above), and the value returned in the
	(format) parameter is unchanged from the value passed on input.

	To open the file for reading using the standard C I/O library functions,
	use GOpenFile().  You can use the GDeletePath() function to free memory
	for the path specification record when it is no longer needed.
	
******************************************************************************/

GPathPtr GDoOpenFileDialog ( char *, char *, short * );

/*************************  GDoSaveFileDialog  *******************************

	Displays the standard "Save..." dialog and prompts the user for the
	name of a file to save.

	GPathPtr GDoSaveFileDialog ( char *prompt, char *filter, char *filename,
	         short *format )

	(prompt):   null-terminated string comtaning prompt to display in dialog.
	(filter):   buffer containing file format strings.
	(filename): null-terminated string containing default file name.
	(format):   indicates which format file should be saved in.
	
	If the user hits the dialog's "OK" button, the function allocates a new
	path specification record, copies the selected file's path specification
	into it, and returns a pointer to it.  If the user cancels or an error
	occurs, the function returns NULL.

	When the "Save..." dialog is first displayed, the contents of (filename)
	are copied into the dialog's file name editable-text field.

	The format of the (filter) string is as described in GDoOpenFileDialog().

	The value in the (format) parameter indicates which of the items in the
	(filter) will be initially displayed in the dialog's "Format" or "Type"
	popup menu.  On a successful return, the value placed into (format) will
	be the number of the item in this menu which was actually selected by the
	user.  This number is counted from 1 (for the first item) to the total
	number of items contained in the (filter) string.
	
	On the Macintosh, this function uses the Navigation Services "Save"
	dialog if it is available; otherwise, it uses the Standard File package.
	
	To open the file for writing using the standard C I/O library functions,
	use GOpenFile().  You can use the GDeletePath() function to free memory
	for the path specification record when it is no longer needed.

******************************************************************************/

GPathPtr GDoSaveFileDialog ( char *, char *, char *, short * );

/*** GSetFileFilterPopupMenuItems *********************************************

	Copies file format strings into a popup menu control.
	
	short GSetFileFilterPopupMenuItems ( GControlPtr control, char *filter )
	
	(control): pointer to a popup menu control.
	(filter):  pointer to file format filter string.
	
	The function returns the number of items added to the popup menu control.
	Any existing items in the control are overwritten.  The filter string must
	take the same format as the filter string passed to GDoOpenFileDialog()
	and GDoSaveFileDialog().

*******************************************************************************/

short GSetFileFilterPopupMenuItems ( GControlPtr, char * );

/*****************************  GCreate  **************************************

	Creates and opens a new file.
	
	int GCreate ( GPathPtr path, GFilePtr *file )
	
	(path): pointer to path specification record describing file to create.
	(file): pointer to a variable to receive reference number of opened file.
	
	The function returns TRUE if successful, or FALSE on failure.
	If successful, the value returned in (file) can be used for subsequent
	file reading/writing.  On failure, the value should not be used.
	If the file specified by the (path) argument already exists, it is
	overwritten.
	
*******************************************************************************/

int GCreate ( GPathPtr, GFilePtr * );

/*****************************  GOpen  ****************************************

	Opens an existing file for reading and/or writing.
	
	int GOpen ( GPathPtr path, GFilePtr *file )
	
	(path): pointer to path specification record describing file to open.
	(file): pointer to a variable to receive reference number of opened file.

	The function returns TRUE, if successful, or FALSE on failure.
	If successful, the value returned in (file) can be used for subsequent
	file reading/writing.  On failure, the value should not be used.
	
*******************************************************************************/

int GOpen ( GPathPtr, GFilePtr * );

/*****************************  GClose  ***************************************

	Closes an opened file.
	
	void GClose ( GFilePtr file )
	
	(file): pointer to an opened file.
	
	This function returns TRUE if successful or FALSE on failure.
	
*******************************************************************************/

int GClose ( GFilePtr );

/******************************  GRead  ***************************************

	Reads a number of bytes from an open file into a buffer.
	
	long GRead ( GFilePtr file, void *buffer, long bytes )
	
	(file): pointer to an open file
	(buffer): pointer to buffer into which data from file should be read.
	(bytes): number of bytes to read from the file.
	
	The function returns the number of bytes actually read from the file if
	successful, or a negative number on failure.
	
*******************************************************************************/

long GRead ( GFilePtr, void *, long );

/******************************  GWrite  ***************************************

	Writes a number of bytes from a buffer in memory to a file.
	
	long GWrite ( GFilePtr file, void *buffer, long bytes )
	
	(file): pointer to an open file
	(buffer): pointer to buffer into which data from file should be read.
	(bytes): number of bytes to write to the file.
	
	The function returns the number of bytes actually written to the file if
	successful, or a negative number on failure.
	
*******************************************************************************/

long GWrite ( GFilePtr, void *, long );

/******************************  GSeek  ***************************************

	Moves a file's read/write mark.
	
	long GSeek ( GFilePtr file, int mode, long bytes )
	
	(file): pointer to an open file
	(mode): origin from which the file's read-write mark should be moved.
	(bytes): number of bytes by which to move the file mark.
	
	The function returns the new offset of the file mark from the start
	of the file, if successful, or a negative number on failure.
	
	The (mode) argument specifies the origin relative to which the file mark
	should be moved.  It can take one of three possible values:
	
	If the value is G_SEEK_FROM_START, the file mark will be moved to the
	specified number of bytes from the start of the file.
	
	If the value is G_SEEK_FROM_MARK, the mark will be moved from its current
	position by the specified number of bytes.
	
	If the value is G_SEEK_FROM_END, the mark will be moved from the end
	of the file by the specified number of bytes.
	
	Any other value will generate an error value.
	
*******************************************************************************/

long GSeek ( GFilePtr, int, long );

#ifdef G_MAC
#define G_SEEK_FROM_START	fsFromStart
#define G_SEEK_FROM_MARK	fsFromMark
#define G_SEEK_FROM_END		fsFromEnd
#endif

#ifdef G_WIN
#define G_SEEK_FROM_START	SEEK_SET
#define G_SEEK_FROM_MARK	SEEK_CUR
#define G_SEEK_FROM_END		SEEK_END
#endif

/****************************  GExecute  ************************************

	Executes an application given a path to its exectuable file.

	int GExecute ( GPathPtr path )

	(path): executable file's complete path specification.

	The function return TRUE if successful, or FALSE on failure.
	
	If successful, the specified application is launched immediately in the
	foreground.  The application calling this function will continue execution
	in the background.
	
******************************************************************************/

int GExecute ( GPathPtr );

/**************************  GGetPathName  ************************************

	Obtains the name of a file or directory from its complete path
	specification.

	void GGetPathName ( GPathPtr path, char *name )

	(path): complete path specification of file or directory.
	(name): receives name of file or directory as a C-style string.

	On return, the name of the file or directory whose complete path
	specification has been given in (path) will be copied into (name)
	as a null-terminated ASCII string.  In Windows, if the path specifies
	a file rather than a directory, the file's name includes the extension.
	
******************************************************************************/

void GGetPathName ( GPathPtr, char * );

/*************************  GSetPathName  ************************************

	Modifies the name of a file or directory in a complete path specification.

	void GSetPathName ( GPathPtr path, char *name )

	(path): complete path specification of the file or directory.
	(name): new name for file or directory, as a C-style string.

	Returns TRUE if successful, or FALSE on failure.  Failure may occurr
	if the new file or directory name contains illegal characters, or if
	the path specification is invalid.
	
	Note: THIS FUNCTION DOES NOT ACTUALLY MODIFY THE NAME OF THE FILE OR
	DIRECTORY THAT THE PATH SPECIFICATION REFERS TO.  It only modifies the
	file or directory name within the path specification.  Thus, after you
	change the file/directory name, the path specification could refer to a
	file or directory which does not actually exist.  This is not an error
	case, so GSetFileName() will return TRUE.

******************************************************************************/

int GSetPathName ( GPathPtr, char * );

/*************************  GAppendPathName  *********************************

	Appends the name of a file or directory to a directory's complete path
	specification.

	int GAppendPathName ( GPathPtr path, char *name )

	(path): complete path specification of directory to append name to.
	(name): name of file or directory to append, as a C-style string.

	The function returns TRUE if successful, or FALSE on failure.  Failure
	may occur if the input path specification is invalid, or the file or
	directory name contains illegal characters.
	
	On input, the path specification must refer to a directory (not a file)
	which currently exists.  If the object specified by the (path) argument
	is nonexistent, or is not a directory, this function will fail.
	
	On successful return, the name of the file or directory provided in the
	(name) parameter will be appended to the path.  You may append the name
	of a file or directory which does not yet exist; in that case, this
	function will still return TRUE (i.e. success) as long as the file or
	directory name is valid.  (Note that if you do so, a new directory or
	file will NOT be created by this function!  See GOpenFile() and
	GCreateDirectory() for more information.)

******************************************************************************/

int GAppendPathName ( GPathPtr, char * );

/*************************  GCreateDirectory  *******************************

	Creates a directory at a specified path.

	int GCreateDirectory ( GPathPtr path )

	(path): complete path specification of directory to create.

	The function returns TRUE if successful, or FALSE on failure.  Failure
	may occur if the input path specification is invalid, or if the disk
	is full, etc.
	
******************************************************************************/

int GCreateDirectory ( GPathPtr );

/*************************  GDeleteDirectory  *******************************

	Deletes a directory at a specified path.

	int GCreateDirectory ( GPathPtr path )

	(path): complete path specification of directory to delete.

	The function returns TRUE if successful, or FALSE on failure.  The
	specified directory must be empty before it can be deleted.  It must
	not also be the current working directory, or the root directory of
	the volume or drive on which it resides.
	
******************************************************************************/

int GDeleteDirectory ( GPathPtr );

/*************************  GDeleteFile  *******************************

	Deletes a file at a specified path.

	int GDeleteFile ( GPathPtr path )

	(path): complete path specification of file to delete.

	The function returns TRUE if successful, or FALSE on failure.  The
	specified file must be closed before it can be deleted, and must not
	be read-only.  If either of these conditions are not met, or if the
	path specification is invalid or refers to a file which does not exist,
	this function will return FALSE.
	
******************************************************************************/

int GDeleteFile ( GPathPtr );

/**************************  GGetFileType  ***********************************

	Obtains a file's Mac file type code or DOS file extension.
	
	char *GGetFileType ( GPathPtr path, char *type )
	
	(path): pointer to path object containing file's complete path specification.
	(type): pointer to character buffer to receive file type/extension.
	
	The function returns a pointer to the buffer, which should be able to
	contain at least 5 bytes of character data.
	
	On the Mac, this function copies the specified file's 4-byte type code
	code as a null-terminated string (e.g. "TEXT", "PICT", etc.).  In Windows,
	it returns the DOS file name extension as a null-terminated string (e.g.,
	".TXT", ".BMP", etc.)

******************************************************************************/

char *GGetFileType ( GPathPtr, char * );

/**************************  GOpenFile  **************************************

	Opens a file for reading/writing using the standard C library I/o functions.

	FILE *GOpenFile ( GPathPtr path, char *mode, char *type, char *creator )

	(path): pointer to complete file's complete path specification.
	(mode): string indicating desired file I/o mode, as with fopen().
	(type): string containing Mac file type code for file.
	(creator): string containing Mac file creator code for file.

	The function returns a pointer to a standard C-library FILE object, if
	successful, or NULL on failure.

	(path) must point to an object of type GPath, as returned by
	GDoOpenFileDialog() or GSaveFileDialog().  Indicate the desired mode
	for file I/O using the same convention as with the standard C library
	function fopen() (e.g. "r" means reading in text mode, "wb" means writing
	in binary mode, etc.).

	You may also specify a Mac file type and creator code (e.g. "TEXT",
	"ttxt").  These arguments are ignored in Windows, and on the Mac when
	you are only opening a file for reading.

	Programming Windows warns that a standard C file pointer is only valid
	during one Windows message cycle.  Thus, you should only use the FILE *
	returned by this function in response to a single GUILib event.  (The
	file path specification should stay valid, however, as long as someone
	doesn't physically move the file it refers to!)

	When you are done reading/writing, you can close the file using the
    standard C library fclose() call.

*****************************************************************************/

FILE *GOpenFile ( GPathPtr, char *, char *, char * );

/***************************  GReadLine  ************************************

	Reads one line from a text file in either DOS, Mac, or UNIX format.

	long GReadLine ( FILE *file, char *buffer, long size, char *eol )
	
	(file): pointer to file, which must be open for reading in BINARY mode.
	(buffer): pointer to buffer to recieve characters from file.
	(size): maximum number of characters to read; i.e. size of buffer.
	(eol): optional buffer to recieve end-of-line character(s).
	
	The function returns the number of bytes read from the file,
	if successful, or zero on failure.
	
	On return, the buffer will contain up to (size - 1) characters from
	the text file.  The buffer will be null-terminated at the end-of-line
	mark (if one was encountered) or at the end of the buffer (if no end-
	of-line mark was encountered).
	
	The optional buffer (eol) receives the end-of-line character(s) that
	were encountered in the file: "\r\n" for DOS text files, "\r" for
	Macintosh text files, "\n" for UNIX text files, or "" if no end-of-
	line character was encountered.  The (eol) buffer will also be null-
	terminated, and must be big enough to contain at least 3 characters.
	If you wish to ignore the end-of-line characters, pass NULL for (eol).

*****************************************************************************/

long GReadLine ( FILE *, char *, long, char * );

/***************************  GWriteLine  ************************************

	Writes one line to a text file in either DOS, Mac, or UNIX format.

	long GWriteLine ( FILE *file, char *buffer, long size, char *eol )
	
	(file): pointer to file, which must be open for writing in BINARY mode.
	(buffer): pointer to buffer containing characters to write to file.
	(size): maximum number of characters to write; i.e. size of buffer.
	(eol): optional buffer containing end-of-line character(s).
	
	The function returns the number of bytes written to the file,
	if successful, or zero on failure.
	
	The function writes the contents of the input text buffer to the file,
	up to the first ASCII NUL character, or up to the size of the buffer.
	The text is followed by the end-of-line character(s) you specified, or,
	if you provide NULL for the (eol) string, by platform-specific end-of-line
	character(s):

	"\r\n" for DOS/Windows text files;
	"\r" for Macintosh text files;
	"\n" for UNIX text files;

	Note that if you provide your own (eol) string, it must be ASCII NUL
	('\0') terminated.
	
*****************************************************************************/

long GWriteLine ( FILE *, char *, long, char * );

/**************************  GReadTextFile  *********************************

	Reads a Macintosh, DOS, or UNIX text file into a buffer and converts
	it into the platform-native text format.
	
	long GReadTextFile ( FILE *file, char *text, long size )
	
	(file): pointer to the file, opened for reading in BINARY mode.
	(text): pointer to buffer to receive text.
	(size): maximum number of bytes to read from file.
	
	The function returns the number of bytes actually read from the file,
	or zero on failure.
	
	The argument (file) is a standard C I/O library file pointer, e.g. as
	returned by fopen() or GOpenFile().  Note that the file must be opened
	for reading in binary (not text) mode.
	
	The buffer which receives the file (text) must be big enough to contain
	at least (size) bytes, plus a terminating null character.  The text
	returned in the buffer will be null-terminated.
	
*****************************************************************************/

long GReadTextFile ( FILE *, long, char * );

/**************************  GWriteTextFile  *********************************

	Writes characters from a buffer in memory to a text file.
	
	long GReadTextFile ( FILE *file, long size, char *text, char *eol )
	
	(file): pointer to the file, opened for writing in BINARY mode.
	(size): maximum number of bytes to write to from file.
	(text): pointer to buffer containing text to write to the file.
	(eol): pointer to string contaning end-of-line character(s).
	
	The function returns the number of bytes actually written to the file,
	or zero on failure.
	
	The argument (file) is a standard C I/O library file pointer, e.g. as
	returned by fopen() or GOpenFile().  Note that the file must be opened
	for writing in binary mode (not text).
	
	The function writes text from the buffer to the file until it has copied
	(size) bytes, or until it reaches the first ASCII NUL ('\0') character
	in the buffer.
	
	You may specify the end-of-line character(s) to be written to the file
	by passing them as a null-terminated string in the (eol) parameter.
	If you do this, all occurrences of following characters in the text
	buffer will be replaced by the end-of-line string in the text file:
	
	'\r' on the Macintosh;
	'\r\n' in Windows;
	'\n' in UNIX.
	
	If you specify NULL for the (eol) string, the text in the buffer
	will be written to the file without any modifications.

********************************************************************************/

long GWriteTextFile ( FILE *, long, char *, char * );

GBitmapPtr GReadBitmapFile ( GPathPtr );
GPicturePtr GReadPictureFile ( GPathPtr );
long GWriteBitmapFile ( GPathPtr, GBitmapPtr );
long GWritePictureFile ( GPathPtr, GPicturePtr );

int GCopyDataToClipboard ( long, void * );
void GCopyTextToClipboard ( char * );
void GCopyBitmapToClipboard ( GBitmapPtr );
void GCopyPictureToClipboard ( GPicturePtr );

/************************  GDoPrintSetupDialog  ******************************

	Displays the standard "Page Setup..." or "Print Setup..." dialog.
	
	int GDoPrintSetupDialog ( void )

	
	If the user hits the dialog's "OK" button, the function returns TRUE.
	If the user hits the "Cancel" button or other failure occurs, the
	function returns FALSE.
	
	GUILib maintains all of the user's currently-selected printer settings
	internally; you do not need to worry about them.
	
******************************************************************************/

int GDoPrintSetupDialog ( void );

/*************************  GDoPrintJobDialog  *******************************

	Displayes the standard "Print..." dialog and receives the number of
	pages and copies the user wishes to print.
	
	int GDoPrintJobDialog ( short *first, short *last, short *copies )
	
	(first): receives first page in document to print
	(last): receives last page of document ot print.
	(copies): receievs number of copies to print.
	
	If the user hits the dialog's "OK" button, this function returns TRUE;
	otherwise, it returns FALSE.  If it returns TRUE, the user wants to
	print something, which you should do with GStartPrintJob(), etc.
	
	In Windows, the dialog initially displays the values in you pass to the
	function in (first), (last), and (copies) for the first page, last page
	number of copies to print.  The Mac's print manager does not display
	them initially, by default.
	
	If the user hits the "OK" button and the dialog exits successfully, these
	variables receive the first page, last page, and number of copies which
	the user entered in the dialog.  If the user indicated that he wants
	to print the entire document by selecting the dialog's "All" radio button,
	(first) and (last) will receive values of 1 and 9999, respectively.
	
	You may then begin a new print job with GStartPrintJob().  For each page
	of the document, call GStartDrawingPage(), make the appropriate graphics
	calls, then call GEndDrawingPage().  When you are done printing, call
	GEndPrintJob() to finish.  Here is an example:
	
	short page, first = 1, last = 1, copy, copies = 1;
	
	if ( GDoPrintJobDialog ( &first, &last, &copies ) )
	{
		for ( copy = 0; copy < copies; copy++ )
		{
			if ( GStartPrintJob ( "Document" ) )
			{
				for ( page = first; page <= last; page++ )
				{
					GStartDrawingPage();
					... make some graphics calls...
					GEndDrawingPage();
				}
			}
			GEndPrintJob();
		}
	}
		
******************************************************************************/	
	
int GDoPrintJobDialog ( short *, short *, short * );

/*************************  GStartPrintJob  **********************************
	
	Initializes printer driver to begin a new print job.
	
	int GStartPrintJob ( char *title )
	
	(title): C-style string containing document title.
	WINDOWS AND MAC BOTH IGNORE THIS FOR NOW.
	
	If successful, this function returns TRUE; otherwise, it returns FALSE.
	NOTE: For every call to GStartPrintJob(), you MUST make a balancing call
	to GEndPrintJob() before calling GStartPrintJob() again, even if your
	initial call to GStartPrintJob() indicates failure.
	
	Draw individual pages between calls to GStartDrawingPage() and
	GEndDrawingPage(); see GDoPrintJobDialog() for an example.
	
******************************************************************************/

int GStartPrintJob ( char * );

/*************************  GEndPrintJob  **********************************
	
	Finishes a print job and releases printer driver.
	
	int GStartPrintJob ( void )
	
	(title): C-style string containing document title.
	WINDOWS AND MAC BOTH IGNORE THIS FOR NOW.
	
	If the print job ended successfully, this function returns TRUE; if it
	was cancelled by the user or otherwise ended in failure, it returns FALSE.
	Do not call this function before calling GStartPrintJob()!
	
	Draw individual pages between calls to GStartDrawingPage() and
	GEndDrawingPage(); see GDoPrintJobDialog() for an example.
	
******************************************************************************/

int GEndPrintJob ( void );

/**************************  GStartDrawingPage  *******************************

	Starts drawing a new printer page.
	
	int GStartDrawingPage ( void )
	
	If successful, the function returns TRUE; otherwise it returns FALSE.
	
	You must call GStartPrintJob() before calling this function!  Also,
	You MUST balance every call to GStartDrawingPage() with a call to
	GEndDrawingPage(), regardless of whether or not the initial call to
	GStartDrawingPage() was successful.  You may not call GStartDrawingPage()
	again before doing so.  
	
	If GStartDrawingPage() returns successfully, subsequent graphics calls
	(e.g. GDrawLineTo(), GMovePenTo(), etc.) will send output to the printer
	rather than to the screen.  GEndDrawingPage() ends printer output.
	
*******************************************************************************/

int GStartDrawingPage ( void );

/*****************************  GEndDrawingPage  *******************************

	Finishes drawing a printer page and ejects the page.
	
	int GEndDrawingPage ( void )
	
	If successful, the function returns TRUE; otherwise it returns FALSE.
	
	Do not call this function without calling GStartDrawingPage() first!!!
	See GDoPrintJobDialog() for an example of usage.
	
*******************************************************************************/

int GEndDrawingPage ( void );

/****************************  GGetPageRect  **********************************

	Obtains the dimensions of a printer page, in pixels.
	
	GRectPtr GGetPageRect ( GRectPtr rect )
	
	(rect): pointer to a rectangle to receive page dimensions.
	
	The function returns a pointer to the given rectangle.  The rectangle's
	top and left will be zero; its bottom and right will contain the height
	and width of a printer page, in pixels.
	
********************************************************************************/
 
GRectPtr GGetPageRect ( GRectPtr );

/**************************  GString  **************************************

	Returns a pointer to a copy of a string resource.
	
	char *GString ( short id )
	
	(id): resource ID number of the desired string.
	
	This function returns a pointer to a static character buffer.  The
	contents of the buffer are overwritten every time you call the function.
	
****************************************************************************/

char *GString ( short );

/**************************  GGetString  ***********************************

	Loads a string resource and copies it into a character buffer.
	
	char *GGetString ( short id, char *string )

	(id): resource ID number of the desired string.
	(string): character buffer to receive string.
	
	The function returns a pointer to the given string buffer.  If the
	specified resource does not exist, the buffer's contents are unchanged.
	The function assumes that the buffer can hold 256 characters.
	
****************************************************************************/

char *GGetString ( short, char * );
GIconPtr GGetIcon ( short );
GBitmapPtr GGetBitmap ( short );
GPicturePtr GGetPicture ( short );

/*******************************************************************************
*
* FUNCTIONS IN Hardware.c
*  
********************************************************************************/
 
long GGetCPUType ( void );
long GGetFPUType ( void );
long GGetOSVersion ( void );
int GCanDrawColor ( void );

/***********************  GGetCursor  *************************************

	Obtains a handle to a cursor resource.

	GCursorPtr GGetCursor ( short id )

	(id): resource ID number of desired cursor resource.

	If successful, this function returns a handle to the specified
	cursor resource, or NULL on failure.  
	
***************************************************************************/

GCursorPtr GGetCursor ( short );

/***********************  GGetXXXCursor ***********************************

	These functions obtain handles to system-defined cursors.

	GCursorPtr GGetArrowCursor ( void )
	GCursorPtr GGetWaitCursor ( void )
	GCursorPtr GGetCrossCursor ( void )
	GCursorPtr GGetTextCursor ( void )

	These functions return handles to the standard system-defined cursors,
	or NULL on failure.  You can display them by passing the handle returned
	to GSetCursor().  
	
	Note that the on the Macintosh, the standard arrow cursor is not a
	System resource, so GGetArrowCursor() always returns NULL on the
	Macintosh.  However, you may still display the standard arrow cursor
	on either platform by passing NULL to GSetCursor().
	
	The appearance and typical usage of these cursors is as follows:
	
	The arrow cursor is the standard left-leaning arrow used to indicate
	that the program is awaiting input.
	
	The wait cursor is a watch (Mac) or hourglass (Windows), used to
	indicate that the program is temporarily occupied.
	
	The text cursor is an I-beam shape used to place the text insertion
	point in an editable text entry field.
	
	The cross cursor is a thin cross, typically used in graphics editing. 
	
***************************************************************************/

GCursorPtr GGetArrowCursor ( void );
GCursorPtr GGetWaitCursor ( void );
GCursorPtr GGetTextCursor ( void );
GCursorPtr GGetCrossCursor ( void );

/***********************  GSetCursor  *************************************

	Displays a given cursor.

	void GSetCursor ( GCursorPtr cursor )

	(cursor): handle to cursor to display, or NULL to display arrow cursor.

	This function returns nothing.  If the given cursor handle specifies
	a valid cursor resource (e.g., as returned by GGetCursor()), that
	cursor will be displayed.  If you pass NULL instead of a valid cursor
	handle, this function will display the standard arrow cursor.

	You can use this function to temporarily switch the cursor between calls
    to GProcessEvent().  As soon as an event is processed, GUILib will change
    the cursor back to whatever cursor was defined for the window in which
    the mouse is located, or to another operating-system-defined cursor,
    if the mouse is located outside any of your application's active windows'
    content area.  See GSetWindowCursor() for more information.

	If the cursor has been hidden by a call to GHideCursor(), this function
	will not display it; you must call GShowCursor() to make it visible.

***************************************************************************/

void GSetCursor ( GCursorPtr );

/***********************  GSetXXXCursor  **********************************

	These functions display system-defined cursors.

	void GSetArrowCursor ( void )
	void GSetWaitCursor ( void )
	void GSetTextCursor ( void )
	void GSetCrossCursor ( void )

	These function return nothing.  A call to GSetArrowCursor() is
	functionally equivalent to GSetCursor ( GGetArrowCursor() ); see
	above.
	
***************************************************************************/

void GSetArrowCursor ( void );
void GSetWaitCursor ( void );
void GSetTextCursor ( void );
void GSetCrossCursor ( void );

/********************  GHideCursor and GShowCursor  *************************

	These functions conceal and re-display the cursor.
	
	void GHideCursor ( void )
	void GShowCursor ( void )
	
	These functions return nothing.  GHideCursor hides the cursor from the
	screen, while GShowCursor() makes it visible once again.
	
*****************************************************************************/

void GHideCursor ( void );
void GShowCursor ( void );

/**************************  GGetMousePosition  *****************************

	Obtains the mouse's position relative to a window or the screen.
	
	void GGetMousePosition ( GWindowPtr window, short *col, short *row )
	
	(window): pointer to window to get mouse position in; NULL = screen.
	(col): receives mouse's horizontal coordinate in window/screen
	(row): receives mouse's vertical coordinate in window/screen
	
	The function returns nothing.
	
	Note that this function returns the mouse position instantaneously, i.e.
	at the time of the function call, not at the time of the last event.
	
******************************************************************************/

void GGetMousePosition ( GWindowPtr, short *, short * );

/************************  GGetDoubleClickTime  *****************************

	Obtains the maximum time interval between mouse clicks in order for
	them to be considered double-clicks.
	
	long GGetDoubleClickTime ( void )
	
	The function returns the mouse double-click time interval in system
	clock ticks.  To convert this value to seconds, divide it by the
	constant G_TICKS_PER_SECOND.

	The user can change this value using the operating system's "Mouse"
	control panel.  Currently, there is no GUILib API function for modifying
	this value.
		
******************************************************************************/

long GGetDoubleClickTime ( void );

/**************************  GButtonIsDown  ************************************

	Determines which (if any) mouse button(s) are currently down.
	
	int GButtonIsDown ( int button )
	
	(button): number(s) of mouse button you are interested in; see below.
	
	The function returns TRUE if any of the specified mouse buttons are down,
	and FALSE otherwise.  Specify multiple mouse buttons by combining their
	numbers with a bitwise OR; for instance,
	
	GButtonIsDown ( 1 | 2 );
	
	will return TRUE if either mouse button 1 or 2 is down.
	
	Note that this function obtains the instantaneous mouse-button state, i.e.
	at the time of the function call, not at the time of the last event.
	
**********************************************************************************/

int GMouseButtonIsDown ( unsigned char );

/****************************  GKeyIsDown  ***************************************

	Determines whether or not a particular key is down.
	
	int GKeyIsDown ( unsigned char key )
	
	(key): key code of key whose state you wish to examine; as #defined above.
	
	The function returns TRUE if the specified key is down and FALSE otherwise.
	
**********************************************************************************/

int GKeyIsDown ( unsigned char );

/***********************  Functions in GSound.c  ******************************/

/****************************  GBeep  *****************************************

	Plays the standard warning beep or alert sound.
	
	void GBeep ( void )
	
*******************************************************************************/

void GBeep ( void );

/**************************  In-Progress APIs  ********************************

	The following APIs are still under development and are liable to change.
	Use them at your own risk!!!

******************************************************************************/

int		GGetNextEvent ( GEventPtr );
void	GProcessEvent ( GEventPtr );
int		GEventPending ( void );
int		GSendEvent ( short, GWindowPtr, long, long );

/*** Dialog icon API ***/

void	GSetDialogIcon ( GWindowPtr, short, short );

/*** Extended control API ***/

/*** Progress dialog API ***/

GWindowPtr GShowProgressDialog ( long, long, long, long, char *, char *, int );
int		GUpdateProgressDialog ( long );
void	GHideProgressDialog ( void );

void GClipLineToRect ( short *, short *, short *, short *, GRectPtr );

/*** File path manipulation API ***/

void GGetApplicationFilePath ( GPathPtr );
int GGetWorkingDirectory ( GPathPtr );
int GSetWorkingDirectory ( GPathPtr );
int GGetParentDirectory ( GPathPtr );

/*** ListBox control API ***/

GControlPtr	GCreateListBox ( GWindowPtr, short, char *, GRectPtr, int, int );
void		GInsertListBoxItem ( GControlPtr, short, char * );
void		GDeleteListBoxItem ( GControlPtr, short );
void		GGetListBoxItemText ( GControlPtr, short, char * );
void		GSetListBoxItemText ( GControlPtr, short, char * );

/*** TabGroup control API ***/

GControlPtr	GCreateTabGroup ( GWindowPtr, short, GRectPtr, int, short );
void		GGetTabGroupItemText ( GControlPtr, short, char * );
void		GSetTabGroupItemText ( GControlPtr, short, char * );
void		GInsertTabGroupItem ( GControlPtr, short, char * );
void		GDeleteTabGroupItem ( GControlPtr, short );
void    	GGetTabGroupContentRect ( GControlPtr, GRectPtr );

/*** Window control API ***/

/****** Tim, Document these functions **********/

GWindowPtr GGetFirstWindow ( void );
GWindowPtr GGetNextWindow ( GWindowPtr );

/*** Serial communications API ***/

/***  GGetNumPorts  *******************************************************

	Returns the number of serial/parallel communication ports that are
	installed on computer on which the applciation is running.

	short GGetNumPorts ( void )
	
***************************************************************************/

short GGetNumPorts ( void );

/***  GGetPortName  *******************************************************

	Obtains the name of a particular serial or parallel communications
	port.

	void GGetPortName ( short portNum, char *portname )

	(portNum): port number
	(portName): recieves name of port as an ASCII NUL-terminated string.
	
	The input port number must range from 1 to the number of ports returned
	by GGetNumPorts().  The output buffer receives the name of the port,
	and must be large enough to hold 255 characters plus the terminating
	ASCII NUL character.
	
	Use this function to generate a list of "friendly names" for the
	serial ports to allow the user to select a desired port from the list.
	
	On the Macintosh, the output buffer recieves names such as
	"Printer Port", "Modem Port", "Printer/Modem Port", etc.
	
	In Windows, the ports are named "COM1", "COM2", etc.
		
***************************************************************************/

void GGetPortName ( short, char * );

/***  GOpenPort  **********************************************************

	Opens a serial port for communcation.

	GPortPtr GOpenPort ( short portNum, long inBufSize, long outBufSize )

	(portNum):    port number
	(inBufSize):  input serial buffer size.
	(outBufSize): output serial buffer size.
	
	If successful, this function returns a pointer to a communication port
	descriptor record.  On failure, the function returns the value #defined
	below as G_OPEN_PORT_FAILED.  Failure will ocurr if the port is in use
	by the operating system or another application, or if memory allocation
	failure occurrs.
	
	The input port number must range from 1 to the number of ports returned
	by GGetNumPorts().  The input and output buffer sizes specify the amount
	of memory to be allocated for input and output communcation buffers,
	in bytes; a value of 256 is suggested for most purposes.
	
	If the serial port can be opened successfully, its communication
	parameters will be set to 9600 baud, no parity, 8 data bits, 1 stop bit.
	You can use the function GSetPortState() to change these values later.

***************************************************************************/

GPortPtr GOpenPort ( short, long, long );

#ifdef GMAC
#define G_OPEN_PORT_FAILED	NULL
#endif

#ifdef GWIN
#ifdef GWIN32
#define G_OPEN_PORT_FAILED	INVALID_HANDLE_VALUE
#else
#define G_OPEN_PORT_FAILED	-1
#endif
#endif

/***  GClosePort  **********************************************************

	Closes an opened communication port.

	int GClosePort ( GPortPtr port )

	(port): pointer to an opened communcation port descriptor record.
	
	This function returns TRUE if it is able to close the port successfully,
	and FALSE otherwise.
	
	The input parameter to this function (port) must be a pointer to an
	opened communcation port specification record, as returned by
	GOpenPort().
		
***************************************************************************/

int GClosePort ( GPortPtr );

/***  GWritePort  **********************************************************

	Writes data to an opened communication port.

	int GWritePort ( GPortPtr port, long size, void *data )

	(port): pointer to an opened communcation port descriptor record.
	(size): number of bytes of data to write to the port.
	(data): pointer to buffer containing data to write to the port.
	
	This function returns TRUE if it is able to write data to the port
	successfully, and FALSE otherwise.
	
***************************************************************************/

int GWritePort ( GPortPtr, long, void * );

/***  GReadPort  **********************************************************

	Reads data from an opened communication port.

	int GReadPort ( GPortPtr port, long size, void *data )

	(port): pointer to an opened communcation port descriptor record.
	(size): maximum number of bytes of data to read from the port.
	(data): pointer to buffer to receive data.
	
	This function returns TRUE if it is able to read data from the port
	successfully, and FALSE otherwise.
	
	To determine the number of data bytes that are currently in the input
	serial buffer, use the function GGetPortDataLength().
	
***************************************************************************/

int GReadPort ( GPortPtr, long, void * );

/***  GSetPortState  *******************************************************

	Sets communication parameters for an opened communication port.

	int GSetPortState ( GPortPtr port, long baud, long parity,
	    long dataBits, long stopBits )

	(port):     pointer to an opened communcation port descriptor record.
	(baud):     baud rate specifier, as #defined below.
	(parity):   parity specifier, as #defined below.
	(dataBits): number of data bits,
	(stopBits): number of stop bits,
	
	This function returns TRUE if it is able to set the port's communication
	state successfully, and FALSE otherwise.
	
******************************************************************************/

int GSetPortState ( GPortPtr, long, long, long, long );

#ifdef GMAC

#define G_300_BAUD			baud300
#define G_1200_BAUD			baud1200
#define G_2400_BAUD			baud2400
#define G_9600_BAUD			baud9600
#define G_14400_BAUD		baud14400
#define G_19200_BAUD		baud19200
#define G_38400_BAUD		baud38400
#define G_57600_BAUD		baud57600

#define G_EVEN_PARITY		evenParity
#define G_ODD_PARITY		oddParity
#define G_NO_PARITY			noParity

#define G_5_DATA_BITS		data5
#define G_6_DATA_BITS		data6
#define G_7_DATA_BITS		data7
#define G_8_DATA_BITS		data8

#define G_1_STOP_BITS		stop10
#define G_1_5_STOP_BITS		stop15
#define G_2_STOP_BITS		stop20

#endif

#ifdef GWIN

#define G_300_BAUD			300
#define G_1200_BAUD			1200
#define G_2400_BAUD			2400
#define G_9600_BAUD			9600
#define G_14400_BAUD		14400
#define G_19200_BAUD		19200
#define G_38400_BAUD		38400
#define G_57600_BAUD		57600
#define G_115200_BAUD		115200

#define G_EVEN_PARITY		EVENPARITY
#define G_ODD_PARITY		ODDPARITY
#define G_NO_PARITY			NOPARITY

#define G_5_DATA_BITS		5
#define G_6_DATA_BITS		6
#define G_7_DATA_BITS		7
#define G_8_DATA_BITS		8

#define	G_1_STOP_BITS		ONESTOPBIT
#define G_1_5_STOP_BITS		ONE5STOPBITS
#define G_2_STOP_BITS		TWOSTOPBITS

#endif

/***  GGetPortState  *********************************************************

	Obtains an opened communication port's current communication parameters.

	int GGetPortState ( GPortPtr port, long baud, long parity,
	    long dataBits, long stopBits )

	(port):     pointer to an opened communcation port descriptor record.
	(baud):     receives baud rate specifier, as #defined above.
	(parity):   recieves parity specifier, as #defined above.
	(dataBits): recieves number of data bits.
	(stopBits): receievs number of stop bits.
	
	This function returns TRUE if it is able to get the port's communication
	state successfully, and FALSE otherwise.
	
******************************************************************************/

int GGetPortState ( GPortPtr, long *, long *, long *, long * );

/***  GGetPortDataLength  *****************************************************

	Obtains the number of bytes of data that are currently in the input
	and output serial buffers.

	int GGetPortDataLength ( GPortPtr port, long *inBytes, long *outBytes )

	(port):     pointer to an opened communcation port descriptor record.
	(inBytes):  receievs number of bytes in the input serial buffer.
	(outBytes): receievs number of bytes in the output serial buffer.
	
	This function returns TRUE if it is able to determine the amount of data
	in the input and output serial buffers, and FALSE otherwise.
	
	You may pass NULL for (inBytes) and/or (outBytes) if you do not wish
	to determine the amount of data in one or the other buffer.  To actually
	read data from the input serial buffer, call GReadPort().
	
******************************************************************************/

int GGetPortDataLength ( GPortPtr, long *, long * );

/*** Image API ***/

/***  GCreateImage  ********************************************************

	Creates a new image record.

	int GCreateImage ( short width, short height, short depth )

	(width):  number of pixel across image horizontally.
	(height): number of pixels down image vertically.
	(depth):  number of bits per pixel in image data.
	
	If successful, this function returns a pointer to a new image record.
	On failure, the function returns NULL.
	
	Valid values for the image pixel depth are 1, 2, 4, 8, 16, 24, and 32.
	
	When you are finished using and wish to dispose of the image record
	returned by this function, you can release the memory it occupies
	by calling GDeleteImage().
	
	An image, like a bitmap, is a rectangular array of pixel data which 
	represents an image that can be drawn on screen.  However, in a bitmap,
	the physical arrangement of the image data is private and platform-
	dependent.  The bitmap's pixel depth and/or color table are controlled
	by hardware, and intended for buffering offscreen drawing operations.
	In an image, by contrast, you can control the bit depth and/or color
	table, and can directly manipulate the pixel values in the image.  You
	cannot, however, perform any drawing on the image.
	
	There are two kinds of images: indexed and direct.  The type of image
	is defined by the image's pixel depth (i.e. number of bits per pixel.)
	Direct-color images are those with bit depths greater than 8.  In a
	direct image, the RGB color components of each pixel are stored in the
	pixels themselves, i.e. each pixel represnts a specific RGB color value.

	Indexed-color images are those with bit depths of 8 bits/pixel or fewer.
	In an indexed image, pixels do not represent actual RGB color values,
	but rather indices into a color table (stored in the image) which contains
	all of the RGB color values present in the image.  The number of entries
	in the color table depends on the bit-depth of the image.  For a one-bit
	image, there are just two colors.  For a four-bit image, there are 16
	colors; for an eight-bit image, there are 256 color table entries.  You
	can determine the RGB color components that correspond to each color
	table entry with the functions GGetImageColorTableEntry(), and modify
	them with GSetImageColorTableEntry().
	
	If the image created by this function is a direct-color image, the image
	will contain no color table.  If the image is an indexed-color image, the
	color table will be uninitialized.  In both cases, the image data array
	will also be uninitialized.
	
	To obtain pointers to the actual image data rows, which you can then
	use to modify and manipulate image pixel data values directly, use the
	function GGetImageDataRow().
	
******************************************************************************/

GImagePtr GCreateImage ( short, short, short );

/***************************  GDeleteImage  ********************************

	Destroys and releases memory occupied by an image record.

	void GDeleteImage ( GImagePtr image )

	(image): pointer to an image record, as returned by GCreateImage().
	
****************************************************************************/
	
void GDeleteImage ( GImagePtr );

/****  GGetImageWidth  *****************************************************

	Obtains the number of pixels across an image horizontally.

	short GGetImageWidth ( GImagePtr image )

	(image): pointer to an image record, as returned by GCreateImage().
	
	This function returns the width of the image, in pixels.
	
****************************************************************************/

short GGetImageWidth ( GImagePtr );

/***  GGetImageHeight  ******************************************************

	Obtains the number of pixels across an image vertically.

	short GGetImageHeight ( GImagePtr image )

	(image): pointer to an image record, as returned by GCreateImage().
	
	This function returns the height of the image, in pixels.
	
****************************************************************************/

short GGetImageHeight ( GImagePtr );

/***  GGetImageDepth  ******************************************************

	Obtains the bit-depth of an image.

	short GGetImageDepth ( GImagePtr image )

	(image): pointer to an image record, as returned by GCreateImage().
	
	This function returns the number of bits per pixel in an image.
	
****************************************************************************/

short GGetImageDepth ( GImagePtr );

/***  GSetImageColorTableEntry  ********************************************

	Sets the R-G-B color components of an entry in the color table of an
	indexed-color image.

	void GSetImageColorTableEntry ( GImagePtr image, unsigned char index,
	     unsigned char red, unsigned char green, unsigned char blue )

	(image): pointer to an image record, as returned by GCreateImage().
	(index): index of color table entry to modify.
	(red):   red color component, from 0-255.
	(green): green color compoenent, from 0-255.
	(blue):  blue color component, from 0-255.
	
	This function returns nothing.
	
	Valid values for the color table entry index are from zero to the
	total number of color table entries minus one, which is
	
	2 ^ (image bit depth) - 1.
	
	If the image is a direct-color image, this function does nothing.
	
****************************************************************************/

void GSetImageColorTableEntry ( GImagePtr, unsigned char, unsigned char, unsigned char, unsigned char );

/***  GGetImageColorTableEntry  ********************************************

	Obtains the R-G-B color components of an entry in the color table of an
	indexed-color image.

	void GGetImageColorTableEntry ( GImagePtr image, unsigned char index,
	     unsigned char *red, unsigned char *green, unsigned char *blue )

	(image): pointer to an image record, as returned by GCreateImage().
	(index): index of color table entry to obtain.
	(red):   receives red color component, from 0-255.
	(green): receives green color compoenent, from 0-255.
	(blue):  receives blue color component, from 0-255.
	
	This function returns nothing.
	
	Valid values for the color table entry index are from zero to the
	total number of color table entries minus one, which is
	
	2 ^ (image bit depth) - 1.
	
	If the image is a direct-color image, this function does nothing.
	
****************************************************************************/

void GGetImageColorTableEntry ( GImagePtr, unsigned char, unsigned char *, unsigned char *, unsigned char * );

/***  GGetImageDataRow  *****************************************************

	Returns a pointer to the start of a row of an image's pixel data.
	
	unsigned char *GGetImageDataRow ( GImagePtr image, short row )
	
	(image): pointer to an image record, as returned by GCreateImage().
	(row):   row number.
	
	The function returns a pointer to the start of the image data row.
	
	The row number (row) must range from zero (for the top row of the image)
	to the totla number of rows in the image minus one (for the bottom row);
	see GGetImageHeight() to determine the number of rows in the image.
	
*****************************************************************************/

unsigned char *GGetImageDataRow ( GImagePtr, short );

/***  GGetImageDataValue  ***************************************************

	Returns the value of a specific pixel in an image.
	
	unsigned long GGetImageDataValue ( GImagePtr image, unsigned char *row, short col )
	
	(image): pointer to an image record, as returned by GCreateImage().
	(row):   pointer to start of image data row containing pixel.
	(col):   position of desired pixel within the row.
	
	The function returns the pixel's value.
	
	You can obtain a pointer to the start of any image data row with the
	function GGetImageDataRow().
	
	For an indexed-color image (i.e. one whose bit depth is less than or
	equal to 8 bits per pixel), the value returned represents an index into
	the image's color table.  Use the function GGetImageColorTableEntry()
	to determine the actual RGB color value corresponding to this index.
	
	For a direct-color image (one whose bit depth is greater than 8 bits per
	pixel), the value returned represents the actual RGB color values.  Use
	the function GGetImageDataValueColor() to parse the R, G, and B color
	components from this value.
	
	The row number (row) must range from zero (for the top row of the image)
	to the totla number of rows in the image minus one (for the bottom row);
	see GGetImageHeight() to determine the number of rows in the image.
	
*****************************************************************************/

unsigned long GGetImageDataValue ( GImagePtr, unsigned char *, short );

/***  GGetImageDataValue  ***************************************************

	Returns the RGB color components of a pixel value in a direct-color
	image.
	
	void GGetImageDataValueColor ( GImagePtr image, unsigned long value,
	     unsigned char *red, unsigned char *green, unsigned char *blue )
	
	(image): pointer to an image record, as returned by GCreateImage().
	(value): value of a particular pixel in the image.
	(red):   recieves red component of the pixel value.
	(green): recieves green component of the pixel value.
	(blue):  recieves blue component of the pixel value.
	
	The function returns nothing.  The RGB color components of the pixel
	value are returned in (red), (green), and (blue), respectively.
	
	You can obtain the value of any individual pixel in an image by calling
	the function GGetImageDataValue().
	
	For an indexed-color image (i.e. one whose bit depth is less than or
	equal to 8 bits per pixel), a pixel value represents an index into
	the image's color table.  This function simply looks up the RGB color
	components corresponding to this index in the image's color table.
	
	For a direct-color image (one whose bit depth is greater than 8 bits per
	pixel), a pixel value represents an actual RGB color value.  This function
	parss the R, G, and B color components from this value.
	
*****************************************************************************/

void GGetImageDataValueColor ( GImagePtr, unsigned long,
unsigned char *, unsigned char *, unsigned char * );

/*** GStartDrawingImage *****************************************************

	Selects an image as the device to receive graphical output.

	void GStartDrawingImage ( GWindowPtr window, GImagePtr image )

	(window): pointer to window to create offscreen bitmap for.
	(image):  pointer to image which will receive drawing operations.
	
	The function returns nothing.  

	After you make this call, all subsequent drawing operations (e.g.
	GDrawLine(), GDrawRectangle(), etc.) will output results to the
	contents of the image object passed as the (image) parameter.
	The image will continue to serve as the graphical output device
	until you call GEndDrawingImage(); see below.
	
	The window parameter (window) will serve as storage for the associated
	drawing parameters (e.g. the color of the current pen, the current text
	font, etc.) while drawing is taking place.  Note that nothing will
	actually be drawn in the window; graphic output will all be drawn into
	the image.

	This function is very similar to the GStartDrawingBitmap() function.
	The main difference is that unlike a bitmap, you can directly manipulate
	the pixel values stored in the image, in addition to drawing on it.

	On the Macintosh, GBitmaps and GImages are implemented using exactly
	the same underlying data structure (a color PixMap), so there is no
	difference between drawing into a GImage and drawing into a GBitmap.
	
	On Windows, the difference is that GImages are implemented using Device-
	Independent Bitmaps (DIBs), while GBitmaps are implemented using Device-
	Dependent Bitmaps (DDBs).  Device-dependent bitmaps have the same
	underlying pixel format as the display hardware, so there is no guarantee
	that the pixel format will be the same from one display device to the
	next -- hence bitmap pixel values cannot be manipulated directly.  With
	a Device-Independent Bitmap, the pixel format is the same from one 
	display device to the next, so direct pixel manipulation can be done
	reliably.  The disadvantage is that the DIB pixel format may not be
	optimized for the actual graphical output display hardware -- and so
	drawing (and drawing into) a GImage may be significantly slower than
	drawing a GBitmap.

	NOTE: I am currently unable to draw into an 8-bit image on Windows.
	No matter what I draw, the image always ends up being blank.  This
	may be related to color table problems, but at the moment I'm stumped.
	
*****************************************************************************/

void GStartDrawingImage ( GWindowPtr, GImagePtr );

/*** GEndDrawingImage *******************************************************

	Ends drawing operations into an image.

	void GEndDrawingImage ( GWindowPtr window, GImagePtr image )

	(window): pointer to window used during graphical operations.
	(image):  pointer to image recieving graphical output.

	The function returns nothing.

	The pointers to the window and the image must be the same as those
	passed in the preceding call to GStartDrawingImage().  After this
	call, no further graphical output to the image will be allowed.

*****************************************************************************/

void GEndDrawingImage ( GWindowPtr, GImagePtr );

/***  GDrawImage  ***********************************************************

	Draws an image into the current window, picture, printer page,
	offscreen bitmap, or other image that is being used for graphic output.
	
	unsigned char *GGetImageDataRow ( GImagePtr image, GRectPtr srcRect,
	               GRectPtr dstRect )
	
	(image):   pointer to an image record, as returned by GCreateImage().
	(srcRect): source rectangle.
	(dstRect): destination rectangle.
	
	This function returns nothing.
	
	The source rectangle defines the coordinates of the portion of the
	image you wish to draw.  Pixels from the left side of the rectangle
	up to (but not including) the rectangle, and from the top side up to
	(but not including) the bottom, will be included in the part of the
	image that gets drawn.  If the source rectangle (srcRect) is NULL,
	the entire image will be drawn.
	
	The destination rectangle specifies the portion of the current
	graphic output device into which the image will be drawn.  By making
	the dimesions of the output rectangle larger or smaller than the
	source rectangle, you can stretch or compress the image.  If the
	destination rectangle is NULL, the image will be drawn into a rectangle
	with the same position and dimensions as the source rectangle.
	
	Make sure you call one of the GStartDrawingXXX() functions before
	calling this function!
	
*****************************************************************************/

void GDrawImage ( GImagePtr, GRectPtr, GRectPtr );

/***  GReadImageFile  *******************************************************

	Reads an image from a file on disk in the platform's native image file
	format.
	
	GImagePtr GReadImageFile ( FILE *file, short depth )
	
	(file):  pointer to file, opened for reading in binary mode.
	(depth): desired bit-depth for resulting image.
	
	The function returns a pointer to the image, if successful, or NULL on
	failure.
	
	The resulting image will have the width and height specified by the
	file parameters.  You specify its bit depth (i.e. number of bits per
	pixel) in the (depth) parameter; see GCreateImage() for legal values
	on this parameter.
	
	The file must be in the native image file format for the respective
	platform.  On the Macintosh, this must be PICT format; in Windows,
	it is BMP.
	
*****************************************************************************/

GImagePtr GReadImageFile ( FILE *, short );

/*** GWriteImageFile *******************************************************

	Writes an image to a file on disk in the platform's native image file
	format.
	
	int GWriteImageFile ( GImagePtr image, FILE *file )
	
	(image): pointer to image.
	(file):  pointer to file, opened for writing in binary mode.
	
	The function returns TRUE, if successful, or FALSE on failure.
	
	The file will be written in the native image file format for the
	respective platform.  On the Macintosh, this is PICT format;
	in Windows, it is BMP.
	
*****************************************************************************/

int GWriteImageFile ( GImagePtr, FILE * );

#ifdef GJPEG

/*** jpeg_std_error_jump ********************************************************

	Initializes the JPEG error handler object, and sets the place to which execution
	will jump if a fatal JPEG library error occurs.
	
	struct jpeg_err_mgr *jpeg_std_error_jump ( struct jpeg_err_mgr *err, jmp_buf *err_jmp_buf )
	
	(err):         pointer to a JPEG error manager object.
	(err_jmp_buf): pointer to a jmp_buf object.
	
	The function returns a pointer to the JPEG error manager object that was provided.
	You should store this pointer in the JPEG compression or decompression object
	used by the rest of your code.
	 
	The JPEG library's default error handling function will cause the program to
	exit if a fatal error occurs.  This function will override the default
	error-exit function with one that jumps back to a specific place in your
	code.
	
	You should call this function after calling jpeg_std_error(); you should place
	your exception-handling code in a call to setjmp() immediately afterwards.  You
	may need to include <setjmp.h>, as well.  For example:
	
	struct	jpeg_decompress_struct	cinfo;
	struct	jpeg_error_mgr			err;
	jmp_buf							err_jmp_buf;
	
	cinfo.err = jpeg_std_error_jump ( &err, &err_jmp_buf );
	if ( setjmp ( err_jmp_buf ) )
	{
    	execution will jump to this point if a fatal JPEG library error occurs.
	}
	
	NOTE: This function will only be defined if your "Target.h" file #defines
	the symbol GJPEG.  See the notes at the top of this header file.
	
*************************************************************************************/

struct jpeg_error_mgr *jpeg_std_error_jump ( struct jpeg_error_mgr *, jmp_buf * );

/*** GReadJPEGImageFile *****************************************************

	Creates a new image and reads imaeg data from a JPEG file into it.
	
	GImagePtr GReadJPEGImageFile ( FILE *file )
	
	(file): pointer to the file from which the JPEG image should be read.
	
	The function returns a pointer to the image that was read from the file,
	if successful, or NULL on failure.
	
	The file must be opened for reading in binary mode.  See GOpenFile().
	
	If successful, the function returns a pointer to an initialized image
	structure containing the data read from the JPEG file.  If the JPEG file
	contains grayscale data (i.e. one color channel per pixel), the image
	returned by this function will be an 8-bit indexed-color bitmap with a
	simple grayscale color table (i.e. color index 0 corresponds to black;
	color index 255 corresponds to white).  If the JPEG file contains color
	data (i.e. 3 color components per pixel), the image returned by this
	function will be a 32-bit direct-color bitmap containing RGB color data.
	
	NOTE: This function will only be defined if your "Target.h" file #defines
	the symbol GJPEG.  See the notes at the top of this header file.

*****************************************************************************/

GImagePtr GReadJPEGImageFile ( FILE * );

/*** GWriteJPEGImageFile *****************************************************

	Writes data from an image in memory to a JPEG image file.
	
	int GWriteJPEGImageFile ( GImagePtr image, short quality, int grayscale,
	    FILE *file )
	
	(image):   pointer to image which should be written to the file.
	(quality): quality factor, indicating degree of image compression.
	(grayscale): flag indicating whether to write grayscale or RGB color data.
	(file):    pointer to the file to which the JPEG image should be written.
	
	The function returns TRUE if successful, or FALSE on failure.
	
	The file must be opened for writing in binary mode.  See GOpenFile().
	
	The quality factor may vary from 0 (maximum compression) to 100
	(no compression).
	
	If the (grayscale) flag is set to be TRUE, the output JPEG file will
	contain only one 8-bit color channel per pixel.  Pixel values in the
	input image (image) are written verbatim to the output JPEG file; any
	color table values present in the input image are ignored.  You should
	not set the (grayscale) flag to TRUE if the input image is a direct-
	color image (i.e. has more than 8 bits per pixel -- see GGetImageDepth()).
	
	If the (grayscale) flag is FALSE, then the output JPEG file will always
	contain RGB color data, with three 8-bit color channels per pixel.  If
	the input image is an indexed-color image, RGB color values for each pixel
	are taken from the image's color table; otherwise, they are taken from
	the image's pixel value directly.
	
	NOTE: This function will only be defined if your "Target.h" file #defines
	the symbol GJPEG.  See the notes at the top of this header file.

*****************************************************************************/

int GWriteJPEGImageFile ( GImagePtr, short, int, FILE * );

#endif /* GJPEG */

#ifdef GTIFF

/*** GReadTIFFImageFile *****************************************************

	Creates a new image and reads imaeg data from a TIFF file into it.
	
	GImagePtr GReadTIFFImageFile ( FILE *file )
	
	(file): pointer to the file from which the TIFF image should be read.
	
	The function returns a pointer to the image that was read from the file,
	if successful, or NULL on failure.
	
	The file must be opened for reading in binary mode.  See GOpenFile().
	
	NOTE: This function will only be defined if your "Target.h" file
	#defines the symbol GTIFF.  See the notes at the top of this header.

*****************************************************************************/

GImagePtr GReadTIFFImageFile ( FILE * );

/*** GWriteTIFFImageFile *****************************************************

	Writes data from an image in memory to a TIFF image file.
	
	int GWriteTIFFImageFile ( GImagePtr image, FILE *file )
	
	(image): pointer to image which should be written to the file.
	(file):  pointer to the file to which the TIFF image should be written.
	
	The function returns TRUE if successful, or FALSE on failure.
	
	The file must be opened for writing in binary mode.  See GOpenFile().
	
	NOTE: This function will only be defined if your "Target.h" file
	#defines the symbol GTIFF.  See the notes at the top of this header file.

*****************************************************************************/

int GWriteTIFFImageFile ( GImagePtr, char *, FILE * );

/*** TIFFClientXXX ***********************************************************

	These are support functions needed to use the standard C file I/O library
	functions (fread(), fwrite(), etc.) with the TIFF library.  They are
	publicly defined here so you can use them yourself.  This allows you
	to pass an open FILE pointer to the TIFFClientOpen() function as follows:

	FILE *file;
	TIFF *tiff;
	
	file = fopen ( filename, mode );
	if ( file != NULL )
	{
		tiff = TIFFClientOpen ( filename, mode, file,
		       TIFFClientRead, TIFFClientWrite, TIFFClientSeek, TIFFClientClose,
		       TIFFClientSize, TIFFClientMap, TIFFClientUnmap );
		if ( tiff != NULL )
		{
			... do stuff with TIFF record ...
		}
		
		TIFFClose ( tiff );
	}
	
	fclose ( file );

******************************************************************************/

tsize_t TIFFClientRead ( thandle_t, tdata_t, tsize_t );
tsize_t TIFFClientWrite ( thandle_t, tdata_t, tsize_t );
toff_t TIFFClientSeek ( thandle_t, toff_t, int );
toff_t TIFFClientSize ( thandle_t );
int TIFFClientClose ( thandle_t );
int TIFFClientMap ( thandle_t, tdata_t *, toff_t * );
void TIFFClientUnmap ( thandle_t, tdata_t, toff_t );

/*** TIFFErrorSupressor ****************************************************

	This function supresses error and warning messages generated by the TIFF
	file I/O library.

	void TIFFErrorSupressor ( const char *module, const char *fmt, va_list ap )
	
	(module): string containing module that generated the error message
	
	Currently, this function does nothing; it simply swallows the error/warning
	message generated by the TIFF library.  To install this function, call
	TIFFSetErrorHandler() and TIFFSetWarningHandler() at some point during
	your program's initialization phase.
	
********************************************************************************/

void TIFFErrorSupressor ( const char *, const char *, va_list );

#endif

#ifdef GGIF

/*** GGIFInputFunction ******************************************************

	These functions are utility functions which enable the GIF library to
	perform I/O using the standard C library file I/O functions fread(),
	fwrite(), etc.  Your application should not need to call them directly.
	
*****************************************************************************/

int GGIFInputFunction ( GifFileType *, GifByteType * , int );
int GGIFOutputFunction ( GifFileType *, GifByteType * , int );

/*** GReadGIFImageFile *****************************************************

	Creates a new image and reads imaeg data from a GIF file into it.
	
	GImagePtr GReadGIFImageFile ( FILE *file )
	
	(file): pointer to the file from which the GIF image should be read.
	
	The function returns a pointer to the image that was read from the file,
	if successful, or NULL on failure.
	
	The file must be opened for reading in binary mode.  See GOpenFile().
	
	The image returned by this function will always be an 8-bit, indexed-
	color image, regardless of the bit depth of the GIF file.  (The GIF
	library returns data in an 8-bits-per-pixel buffer, which makes it
	difficult to create an image with any other bit depth.)  However,
	pixel values and color table values stored in the image will be
	exactly the same as stored in the GIF file.
	
	In a multi-image GIF file (e.g. an animated GIF), only the first image
	in the file will be returned.  This function ignores all GIF extensions,
	including transparency codes, etc.
	
	NOTE: This function will only be defined if your "Target.h" file
	#defines the symbol GGIF.  See the notes at the top of this header file.

*****************************************************************************/

GImagePtr GReadGIFImageFile ( FILE * );

/*** GWriteGIFImageFile *****************************************************

	Writes data from an image in memory to a GIF image file.
	
	int GWriteGIFImageFile ( GImagePtr image, FILE *file )
	
	(image): pointer to image which should be written to the file.
	(file):  pointer to the file to which the GIF image should be written.
	
	The function returns TRUE if successful, or FALSE on failure.
	
	The file must be opened for writing in binary mode.  See GOpenFile().
	
	This function can only write GIF files from indexed-color images; thus
	the function will fail if the image presented to it has a bit depth of
	more than eight bits per pixel.
	
	GIF files written by this function will contain only a single GIF image,
	and no extensions, in GIF87a format (e.g. the simplest possible GIF
	format).
	
	NOTE: This function will only be defined if your "Target.h" file
	#defines the symbol GGIF.  See the notes at the top of this header file.

*****************************************************************************/

int GWriteGIFImageFile ( GImagePtr, FILE * );

/*** GWriteGIFHeader *********************************************************

	Writes a header for a GIF87a or GIF89a GIF image file.

	int GWriteGIFHeader ( GImagePtr image, FILE *file, char *version )

	(image): pointer to a image which defines dimensions of GIF file.
	(file):  pointer to file to which the GIF header should be written.
	(version): GIF file version stamp.

	The function returns a pointer to a GifFileType record, if successful,
	or NULL on failure.

	The image provided (image) defines the size and color table for the
	file.  If you intending to write multiple images to the GIF file (e.g.
	for an animated GIF89 file), they should all be of the same size and
	contain the same color table as the image provided here.
	
	This function can only write GIF files from indexed-color images; thus
	the function will fail if the image presented to it has a bit depth of
	more than eight bits per pixel.

	The GIF file (file) must be opened for writing in binary mode.
	See GOpenFile().

	The version stamp (version) must be either GIF87_STAMP or GIF89_STAMP,
	as #defined in the GIF library header file "gif_lib.h".  Any other value
	will cause invalid GIF files to be written.  GIF87a files are the simplest
	and most widely-readable version; however, you are required to use the GIF89a
	version if you wish to use effects such as transparency or animation.

	After this function has returned, you may call GWriteGIFImage() to write
	individual images to the file; then call the GIF library function
	EGifCloseFile() to terminate the GIF file.

******************************************************************************/

GifFileType *GWriteGIFHeader ( GImagePtr, FILE *, char * );

/*** GWriteGIFImage *********************************************************

	Writes an image to a GIF87a or GIF89a GIF image file.

	int GWriteGIFImage ( GImagePtr image, GifFileType *gif,
					     int transparent, unsigned char transparentColor,
                         int userInput, unsigned short delay, int disposalMethod )

	(image): pointer to a image which should be written to GIF file.
	(gif):   pointer to GIF file to which the image should be written.

	The function returns a TRUE, if successful, or FALSE on failure.

	The GIF file descriptor (gif) must be the object returned by the
	preceding call to GWriteGIFHeader().

	The image provided (image) will be written to the GIF file.  If you are
	intending to write multiple images to the GIF file (e.g. an animated GIF
	file), the image should be of the same size and contain the same color
	table as the image passed to GWriteGIFHeader().  This function will not
	write local image color tables.
	
	This function can only write GIF files from indexed-color images; thus
	the function will fail if the image presented to it has a bit depth of
	more than eight bits per pixel.

	If the GIF file version is GIF87a, then the image is simply written to
	the file, without any graphic control block extension.  The parameters
	following the GIF file descriptor (gif) are ignored.
	
	If the GIF file version is GIF89a, then a graphic control extension is
	written to the file, preceding the image itself.  The values contained
	in the extension are taken from the function parameters as follows:

	(transparent): transparency flag; if TRUE, the color table index given
	               in the next parameter will be the transparent color;

	(transparentColor): color table index to use as the transparent color;

	(userInput):   if TRUE, user input should be required before displaying
	               the next image;

	(delay):       delay before next image in the GIF file should be presented,
	               in hundredths of a second;

	(disposalMethod): desired transition to next image in the GIF file.  

	After this function has returned, you may call GWriteGIFImage() to write
	additional images to the file; then call the GIF library function
	EGifCloseFile() to terminate the GIF file.

******************************************************************************/

int GWriteGIFImage ( GImagePtr, GifFileType *, int, unsigned char, int, unsigned short, int );

#endif

/*** GSetSubDialog ***********************************************************

	Creates dialog items contained within another dialog.
	
	GWindowPtr GSetSubDialog ( GWindowPtr dialog, short item, short id )
	
	(dialog): pointer to parent dialog window which should contain sub-dialog.
	(item):   item in parent dialog which should contain sub-dialog.
	(id):     dialog resource describing items in sub-dialog.
	
	The function returns a pointer to the sub-dialog window created, or NULL
	on failure.
	
	NEED TO DOCUMENT THIS BETTER, AND EXPLAIN MAC IMPLEMENTATION.
	
	ALSO NEED TO EXPLAIN MODIFIED G_CREATE_EVENTs!
	
******************************************************************************/

GWindowPtr GSetSubDialog ( GWindowPtr, short, short );

/*** GGetSubDialog ***********************************************************

	Returns a pointer to the sub-dialog of a dialog window (if any)

	GWindowPtr GGetSubDialog ( GWindowPtr dialog, short item )
	
	(dialog): pointer to parent dialog window containing sub-dialog.
	(item):   item in parent dialog containing sub-dialog.
	
	The function returns a pointer to the sub-dialog window contained
	within the specified item in the parent dialog window.
	
	On the Macintosh, this function always returns a pointer to the parent
	dialog window, since the sub-dialog is actually just a set of items
	within it.
	
******************************************************************************/

GWindowPtr GGetSubDialog ( GWindowPtr, short );

/*** GSetDialogDefaultButton *************************************************

	Sets the item in a dialog window which will become the default push-
	button control.

	void GSetDialogDefaultButton ( GWindowPtr dialog, short item )
	
	(dialog): pointer to a dialog window.
	(item):   item number of desired default button control.
	
	OBVIOUSLY, WE NEED TO FINISH DOCUMENTING THIS.
		
******************************************************************************/

void GSetDialogDefaultButton ( GWindowPtr, short );

/*** GSetDialogDefaultButton *************************************************

	Returns the item number of the default push-button control in a dialog
	window.

	short GGetDialogDefaultButton ( GWindowPtr dialog )
	
	(dialog): pointer to a dialog window.

	The function returns the dialog window's default push button's item
	number.
		
	OBVIOUSLY, WE NEED TO FINISH DOCUMENTING THIS.
		
******************************************************************************/

short GGetDialogDefaultButton ( GWindowPtr );

GBitmapPtr		GCreateBitmapFromImage ( GImagePtr );
GImagePtr		GCreateImageFromBitmap ( GBitmapPtr );
void			GBitmapToImageDataRow ( GBitmapPtr, short, GImagePtr, short );
void			GImageToBitmapDataRow ( GImagePtr, short, GBitmapPtr, short );

#endif
