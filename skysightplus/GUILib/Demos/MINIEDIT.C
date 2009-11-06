#include "GUILib.h"

#define FILE_MENU					1
#define EDIT_MENU					2
#define WINDOW_MENU					3

#define FILE_MENU_ID				257
#define FILE_NEW_ITEM				1
#define FILE_OPEN_ITEM				2
#define FILE_CLOSE_ITEM				3
#define FILE_SAVE_ITEM				5
#define FILE_SAVE_AS_ITEM			6
#define FILE_PAGE_SETUP_ITEM		8
#define FILE_PRINT_ITEM				9
#define FILE_QUIT_ITEM				11

#define EDIT_MENU_ID				258
#define EDIT_UNDO_ITEM				1
#define EDIT_CUT_ITEM				3
#define EDIT_COPY_ITEM				4
#define EDIT_PASTE_ITEM				5
#define EDIT_CLEAR_ITEM				6
#define EDIT_SELECT_ALL_ITEM		8
#define EDIT_TEXT_FONT_ITEM			10

#define WINDOW_MENU_ID				259
#define WINDOW_TILE_ITEM			1
#define WINDOW_CASCADE_ITEM			2
#define WINDOW_ARRANGE_ITEM			3

#define EDIT_WINDOW_TEXT_ID			1

int 		DoFileMenuItem ( long );
int 		DoEditMenuItem ( long );
int			DoWindowMenuItem ( long );
int			DoFontMenu ( long, long );
int			DoEditWindowEvent ( short, GWindowPtr, long, long );
GWindowPtr	NewEditWindow ( GPathPtr );
void		DeleteEditWindow ( GWindowPtr );
void		SaveEditWindow ( GWindowPtr, GPathPtr );
void		UpdateMenus ( GWindowPtr );

typedef struct EditWindowData
{
	GPathPtr	path;
}
EditWindowData, *EditWindowDataPtr;

#define GetEditWindowData(window)	((EditWindowDataPtr)GGetWindowData(window))

/******************************  GMain  ********************************/

int GMain ( short event, GWindowPtr window, long param1, long param2 )
{
	int respond = TRUE;
	
	switch ( event )
	{
		case G_EVENT_LOOP_ENTRY:
			GCreateFrameWindow ( 256, "MiniEdit", -1, -1, -1, -1 );
			GSetWindowMenu ( GGetMainMenu ( WINDOW_MENU ) );
			UpdateMenus ( NULL );
			break;

		case G_MENU_EVENT:
			switch ( param1 )
			{
				case FILE_MENU_ID:
					respond = DoFileMenuItem ( param2 );
					break;
					
				case EDIT_MENU_ID:
					respond = DoEditMenuItem ( param2 );
					break;
					
				case WINDOW_MENU_ID:
					respond = DoWindowMenuItem ( param2 );
					break;
					
				case G_TEXT_FONT_MENU_ID:
				case G_TEXT_SIZE_MENU_ID:
				case G_TEXT_STYLE_MENU_ID:
					DoFontMenu ( param1, param2 );
					break;
			}
			break;
	}
	
	return ( respond );
}

/***********************************  DoFileMenuItem  **************************************/

int DoFileMenuItem ( long item )
{
	int			respond = TRUE;
	char		filename[256] = { 0 };
	char		*filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*||";
	char		*types[] = { "TEXT", "PICT" };
	GWindowPtr	window;
	GPathPtr	path;
	
	switch ( item )
	{
		case FILE_NEW_ITEM:
			NewEditWindow ( NULL );
			break;
			
		case FILE_OPEN_ITEM:
			if ( path = GDoOpenFileDialog ( "Open a text file...", filter, types, 1 ) )
				NewEditWindow ( path );
			break;

		case FILE_CLOSE_ITEM:
			if ( window = GGetActiveWindow() )
				GCloseWindow ( window );
			break;
			
		case FILE_SAVE_ITEM:
			if ( window = GGetActiveWindow() )
				SaveEditWindow ( window, NULL );
			break;
			
		case FILE_SAVE_AS_ITEM:
			if ( window = GGetActiveWindow() )
			{
				GGetWindowTitle ( window, filename );
				if ( path = GDoSaveFileDialog ( "Save text file as...", filter, filename ) )
					SaveEditWindow ( window, path );
			}
			break;
			
		case FILE_PAGE_SETUP_ITEM:
			GDoPrintSetupDialog();
			break;
			
		case FILE_QUIT_ITEM:
			GExitEventLoop ( 0 );
			break;
	}
	
	return ( respond );
}

/***********************************  DoEditMenuItem  ***************************************/

int DoEditMenuItem ( long item )
{
	int			respond = TRUE;
	char		font[256];
    short		charset;
	short		size;
	short		style;
	GWindowPtr	window;
	
	switch ( item )
	{
		case EDIT_SELECT_ALL_ITEM:
			if ( window = GGetActiveWindow() )
				GSetEditTextSelection ( window, EDIT_WINDOW_TEXT_ID, 0, 32767 );
			break;

		case EDIT_TEXT_FONT_ITEM:
			if ( window = GGetActiveWindow() )
			{
				GGetEditTextFont ( window, EDIT_WINDOW_TEXT_ID, font, &charset, &size, &style );
				if ( GDoFontDialog ( font, &charset, &size, &style, NULL ) )
                	GSetEditTextFont ( window, EDIT_WINDOW_TEXT_ID, font, charset, size, style );
			}
            break;
	}

	return ( respond );
}

/********************************  DoWindowMenuItem  ****************************************/

int DoWindowMenuItem ( long item )
{
	int respond = TRUE;
	
	switch ( item )
	{
		case WINDOW_TILE_ITEM:
			GTileWindows();
			break;
			
		case WINDOW_CASCADE_ITEM:
			GCascadeWindows();
			break;
						
		case WINDOW_ARRANGE_ITEM:
			GArrangeWindowIcons();
			break;
	}
	
	return ( respond );
}

/********************************  DoFontMenu  ************************************/

int DoFontMenu ( long menu, long item )
{
	char		font[256];
	short		charset, size, style;
	GWindowPtr	window;
	
	if ( window = GGetActiveWindow() )
	{
		GGetEditTextFont ( window, EDIT_WINDOW_TEXT_ID, font, &charset, &size, &style );
		GDoFontMenuItem ( menu, item, font, &charset, &size, &style );
		GSetEditTextFont ( window, EDIT_WINDOW_TEXT_ID, font, charset, size, style );
	}
		
	return ( TRUE );
}

/******************************  DoEditWindowEvent  *********************************/

int DoEditWindowEvent ( short event, GWindowPtr window, long param1, long param2 )
{
	int					respond = TRUE;
	GRect				rect;
	EditWindowDataPtr	data;
	
	switch ( event )
	{
		/*** When the window is activated, send keyboard events to the window's
		     edit-text control, and update the menus. ***/
		     
		case G_ACTIVATE_EVENT:
			GSetKeyEventControl ( window, EDIT_WINDOW_TEXT_ID );
			UpdateMenus ( window );
			break;
		
		/*** When the window is resized, resize its edit-text control to fit the window. ***/
		
		case G_SIZE_EVENT:
			GGetWindowContentRect ( window, &rect );
			GInsetRect ( &rect, -1, -1 );
			GSetControlRect ( window, EDIT_WINDOW_TEXT_ID, &rect );
			break;
			
		/*** When the window is closed, delete its path specification record and
		     data record, then delete the window itself.  If there are no more
		     windows left, update the menus. ***/
		     
		case G_CLOSE_EVENT:
			DeleteEditWindow ( window );
			if ( GGetActiveWindow() == NULL )
				UpdateMenus ( NULL );
			break;
	}
	
	return ( respond );
}

/*****************************  NewEditWindow  **********************************/

GWindowPtr NewEditWindow ( GPathPtr path )
{
	static int			number = 1;
	long				size;
	char				*buffer, title[64];
	FILE				*file;
	GRect				rect;
	GWindowPtr			window;
	EditWindowDataPtr	data;
	
	/*** First allocate a private data record for the window.  If we are creating
	     the window from a file on disk, store a pointer to the file's path
	     specification in the data record and use the file's name as the window's
	     title.  Otherwise, give the window an "Untitled" caption.  Finally, create
	     the window and store a pointer to its private data record in it. ***/
	     
	data = (EditWindowDataPtr) GNewPtr ( sizeof ( EditWindowData ) );

	if ( data->path = path )
		GGetFileName ( path, title );
	else
		sprintf ( title, "Untitled-%d", number++ );
	
	window = GCreateWindow ( 2, title, -1, -1, -1, -1, FALSE, G_APPLICATION_WINDOW, (long) data, DoEditWindowEvent );
	GGetWindowContentRect ( window, &rect );
	GInsetRect ( &rect, -1, -1 );
	GCreateEditText ( window, EDIT_WINDOW_TEXT_ID, "", &rect, TRUE, TRUE, TRUE );
	GEraseOnUpdates ( window, FALSE );
	
	/*** If we creating the window from a file, open the file and determine its size.
	     Allocate a buffer big enough to hold the file's contents, and read them into
	     the buffer (adding a null character to terminate it!)  Copy the text in the
	     the buffer into the window's edit-text control, then free the buffer and close
	     the file. ***/
	
	if ( path )
	{
		file = GOpenFile ( path, "rb", "", "" );
		fseek ( file, 0, SEEK_END );
		size = ftell ( file );
		fseek ( file, 0, SEEK_SET );
		buffer = GNewPtr ( size + 1 );
		fread ( buffer, size, 1, file );
		buffer[size] = '\0';
		GSetControlText ( window, EDIT_WINDOW_TEXT_ID, buffer );
		free ( buffer );
		fclose ( file );
	}
	
	/*** Display and return a pointer to the newly-created window. ***/
	
	GSetActiveWindow ( window );
	return ( window );
}

/*****************************  DeleteEditWindow  *************************************/

void DeleteEditWindow ( GWindowPtr window )
{
	EditWindowDataPtr	data = GetEditWindowData ( window );
	
	if ( data->path )
		GDeletePath ( data->path );
		
	GFreePtr ( data );
	GDeleteWindow ( window );
}

/******************************  SaveEditWindow  **************************************/

void SaveEditWindow ( GWindowPtr window, GPathPtr path )
{
	char				title[64], *buffer;
	long				size;
	FILE				*file;
	EditWindowDataPtr	data = GetEditWindowData ( window );
	
	if ( path )
	{
		if ( data->path )
			GDeletePath ( data->path );
		data->path = path;
	}
	else
	{
		if ( data->path )
			path = data->path;
		else
		{
			DoFileMenuItem ( FILE_SAVE_AS_ITEM );
			return;
		}		
	}
		
	GGetFileName ( path, title );
	GSetWindowTitle ( window, title );
		
	/*** Determine the length of the text in the window's edit-text control.  Allocate
	     a temporary buffer to store it, then copy it into the buffer.  Open a file
	     on disk, write the buffer into it, and close the file. ***/
	
	size = GGetControlTextLength ( window, EDIT_WINDOW_TEXT_ID );
	buffer = GNewPtr ( size + 1 );
	GGetControlText ( window, EDIT_WINDOW_TEXT_ID, buffer );
	
	file = GOpenFile ( data->path, "wb", "TEXT", "ttxt" );
	fwrite ( buffer, size, 1, file );
	fclose ( file );
	free ( buffer );
}

/*****************************  UpdateMenus  *******************************************/

void UpdateMenus ( GWindowPtr window )
{
	char		font[256];
	short		charset, size, style;
	GMenuPtr	fileMenu = GGetMainMenu ( FILE_MENU );
	GMenuPtr	editMenu = GGetMainMenu ( EDIT_MENU );
	GMenuPtr	windowMenu = GGetMainMenu ( WINDOW_MENU );

	if ( window == NULL )
	{
		GEnableMenuItem ( fileMenu, FILE_CLOSE_ITEM, FALSE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_ITEM, FALSE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_AS_ITEM, FALSE );
		GEnableMenuItem ( fileMenu, FILE_PRINT_ITEM, FALSE );
		
		GEnableMainMenu ( EDIT_MENU, FALSE );
        GEnableMainMenu ( WINDOW_MENU, FALSE );
	}
	else
	{
		GEnableMenuItem ( fileMenu, FILE_CLOSE_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_SAVE_AS_ITEM, TRUE );
		GEnableMenuItem ( fileMenu, FILE_PRINT_ITEM, TRUE );

		GEnableMainMenu ( EDIT_MENU, TRUE );
        GEnableMainMenu ( WINDOW_MENU, TRUE );

		GGetEditTextFont ( window, EDIT_WINDOW_TEXT_ID, font, &charset, &size, &style );
		GCheckFontMenuItems ( font, charset, size, style );
	}
}
