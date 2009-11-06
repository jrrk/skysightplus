#include "GUILib.h"

#define APPLE_MENU					0
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

#define WINDOW_MENU_ID				259
#define WINDOW_TILE_ITEM			1
#define WINDOW_CASCADE_ITEM			2
#define WINDOW_ARRANGE_ITEM			3

int		DoFileMenuItem ( long );
int		DoEditMenuItem ( long );
int		DoWindowMenuItem ( long );
void	UpdateMenus ( GWindowPtr );

/**************************  DoEvent  *************************************/

int GMain ( short event, GWindowPtr window, long param1, long param2 )
{
	int respond = TRUE;
	
	switch ( event )
	{
		case G_EVENT_LOOP_ENTRY:
			GCreateFrameWindow ( 256, "Multiple Window Demo", -1, -1, -1, -1 );
			GSetWindowMenu ( GGetMainMenu ( WINDOW_MENU ) );
            UpdateMenus ( GGetActiveWindow() );
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
			}
			break;

		case G_ACTIVATE_EVENT:
			UpdateMenus ( GGetActiveWindow() );
			break;

		case G_CLOSE_EVENT:
			GDeleteWindow ( window );
			window = GGetActiveWindow();
			if ( window == NULL )
				UpdateMenus ( NULL );
			respond = FALSE;
			break;
	}
	
	return ( respond );
}

/***********************************  DoFileMenuItem  **************************************/

int DoFileMenuItem ( long item )
{
	int			respond = TRUE;
	char		title[256];
	static int	number = 1;
	GWindowPtr	window;
		
	switch ( item )
	{
		case FILE_NEW_ITEM:
			sprintf ( title, "Untitled-%d", number++ );
			GCreateWindow ( 2, title, -1, -1, -1, -1, TRUE, G_APPLICATION_WINDOW, 0, NULL );
			break;
			
		case FILE_CLOSE_ITEM:
			if ( window = GGetActiveWindow() )
				GCloseWindow ( window );
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
	int respond = TRUE;
	
	switch ( item )
	{
	
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

/*******************************  UpdateMenus  *********************************************/

void UpdateMenus ( GWindowPtr window )
{
	GMenuPtr	fileMenu = GGetMainMenu ( FILE_MENU );
	GMenuPtr	editMenu = GGetMainMenu ( EDIT_MENU );
	GMenuPtr	windowMenu = GGetMainMenu ( WINDOW_MENU );

	if ( window == NULL )
	{
		GEnableMenuItem ( fileMenu, FILE_CLOSE_ITEM, FALSE );
        GEnableMainMenu ( WINDOW_MENU, FALSE );
	}
	else
	{
		GEnableMenuItem ( fileMenu, FILE_CLOSE_ITEM, TRUE );
        GEnableMainMenu ( WINDOW_MENU, TRUE );
	}
}

