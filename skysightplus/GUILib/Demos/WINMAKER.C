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
#define EDIT_SHOW_EVENTS_ITEM		10

#define WINDOW_MENU_ID				259
#define WINDOW_TILE_ITEM			1
#define WINDOW_CASCADE_ITEM			2
#define WINDOW_ARRANGE_ITEM			3

#define NEW_DIALOG_ID				128
#define NEW_TITLE_TEXT				4
#define NEW_DOCUMENT_RADIO			6
#define NEW_FLOATING_RADIO			7
#define NEW_MOVEABLE_BOX			8
#define NEW_CLOSEABLE_BOX			9
#define NEW_SIZEABLE_BOX			10
#define NEW_MAXIMIZABLE_BOX			11
#define NEW_MINIMIZABLE_BOX			12

#define EVENT_DIALOG_ID				129
#define EVENT_CLOSE_BUTTON			1
#define EVENT_HISTORY_TEXT			2
#define EVENT_RECORD_NULL_BOX		3
#define EVENT_RECORD_DRAG_BOX		4

#define	MAX_EVENT_HISTORY_LENGTH	10

int			DoWindowEvent ( short, GWindowPtr, long, long );
int 		DoNewDialogEvent ( short, GWindowPtr, long, long );
int			DoEventDialogEvent ( short, GWindowPtr, long, long );
int 		DoFileMenuItem ( long );
int			DoEditMenuItem ( long );
int			DoWindowMenuItem ( long );
void		DoNewWindowDialog ( void );
void		RecordEvent ( short, GWindowPtr, long, long );

char		gNewWindowTitle[256] = { 0 };
char		gNewWindowType = G_APPLICATION_WINDOW;

char		gEventHistory[MAX_EVENT_HISTORY_LENGTH][256];
int			gEventHistoryLength = 0;
GWindowPtr	gEventHistoryDialog = NULL;
int			gRecordNullEvents = FALSE;
int			gRecordMouseMoveDragEvents = TRUE;

/******************************  GMain  ************************************/

int GMain ( short event, GWindowPtr window, long param1, long param2 )
{
	int respond = TRUE;

	RecordEvent ( event, window, param1, param2 );
			
	switch ( event )
	{
		case G_ENTRY_EVENT:
			strcpy ( gNewWindowTitle, "Untitled" );
			GCreateFrameWindow ( 256, "Window Maker", -1, -1, -1, -1 );

			GSetWindowMenu ( GGetMainMenu ( WINDOW_MENU ) );

			gEventHistoryDialog = GCreateDialog ( EVENT_DIALOG_ID, 0, DoEventDialogEvent );
			GSetDialogControlValue ( gEventHistoryDialog, EVENT_RECORD_NULL_BOX, gRecordNullEvents );
			GSetDialogControlValue ( gEventHistoryDialog, EVENT_RECORD_DRAG_BOX, gRecordMouseMoveDragEvents );
			break;

		case G_MENU_EVENT:
			switch ( param1 )
			{
				case FILE_MENU_ID:
					DoFileMenuItem ( param2 );
					break;
					
				case EDIT_MENU_ID:
					DoEditMenuItem ( param2 );
					break;

				case WINDOW_MENU_ID:
					DoWindowMenuItem ( param2 );
                    break;
			}
			break;
	}
	
	return ( respond );
}

/***********************************  DoFileMenuItem  **************************************/

int DoFileMenuItem ( long item )
{
	int result, respond = TRUE;
	GWindowPtr window, dialog;
		
	switch ( item )
	{
		case FILE_NEW_ITEM:
			if ( GEnterModalDialog ( NEW_DIALOG_ID, 0, DoNewDialogEvent ) == G_OK_BUTTON )
				GCreateWindow ( 2, gNewWindowTitle, -1, -1, -1, -1, TRUE, gNewWindowType, 0, DoWindowEvent );
			break;
			
		case FILE_CLOSE_ITEM:
			if ( window = GGetActiveWindow() )
				GCloseWindow ( window );
			break;
			
		case FILE_PAGE_SETUP_ITEM:
			GDoPrintSetupDialog();
			break;
			
		case FILE_QUIT_ITEM:
			GExitMainLoop ( 0 );
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
		case EDIT_SHOW_EVENTS_ITEM:
			GShowWindow ( gEventHistoryDialog );
			GSetActiveWindow ( gEventHistoryDialog );
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

/********************************  DoWindowEvent  ************************************/

int DoWindowEvent ( short event, GWindowPtr window, long param1, long param2 )
{
	int respond = TRUE;
	
	RecordEvent ( event, window, param1, param2 );

	switch ( event )
	{
		case G_MOUSE_DOWN_EVENT:
			GSetMouseEventWindow ( window );
			break;
			
		case G_MOUSE_UP_EVENT:
			GSetMouseEventWindow ( NULL );
			break;
			
		case G_CLOSE_EVENT:
			GDeleteWindow ( window );
			break;
	}
	
	return ( respond );
}

/********************************  DoNewWindowDialogEvent  ***************************/

int DoNewDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int respond = TRUE;
	
	RecordEvent ( event, dialog, param1, param2 );
	
	switch ( event )
	{
		case G_INITIAL_EVENT:
			GSetDialogItemText ( dialog, NEW_TITLE_TEXT, gNewWindowTitle );

			GSetDialogControlValue ( dialog, NEW_DOCUMENT_RADIO, ! ( gNewWindowType & G_FLOATING ) );
			GSetDialogControlValue ( dialog, NEW_FLOATING_RADIO, gNewWindowType & G_FLOATING );

			GSetDialogControlValue ( dialog, NEW_MOVEABLE_BOX, gNewWindowType & G_MOVEABLE );
			GSetDialogControlValue ( dialog, NEW_CLOSEABLE_BOX, gNewWindowType & G_CLOSEABLE );
			GSetDialogControlValue ( dialog, NEW_SIZEABLE_BOX, gNewWindowType & G_SIZEABLE );
			GSetDialogControlValue ( dialog, NEW_MAXIMIZABLE_BOX, gNewWindowType & G_MAXIMIZABLE );
			GSetDialogControlValue ( dialog, NEW_MINIMIZABLE_BOX, gNewWindowType & G_MINIMIZABLE );
			break;

		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_OK_BUTTON:
					GGetDialogItemText ( dialog, NEW_TITLE_TEXT, gNewWindowTitle );

					gNewWindowType = G_FIXED_WINDOW;
					
					if ( GGetDialogControlValue ( dialog, NEW_FLOATING_RADIO ) )
						gNewWindowType |= G_FLOATING;

					if ( GGetDialogControlValue ( dialog, NEW_MOVEABLE_BOX ) )
						gNewWindowType |= G_MOVEABLE;

					if ( GGetDialogControlValue ( dialog, NEW_CLOSEABLE_BOX ) )
						gNewWindowType |= G_CLOSEABLE;

					if ( GGetDialogControlValue ( dialog, NEW_SIZEABLE_BOX ) )
						gNewWindowType |= G_SIZEABLE;
						
					if ( GGetDialogControlValue ( dialog, NEW_MAXIMIZABLE_BOX ) )
						gNewWindowType |= G_MAXIMIZABLE;

					if ( GGetDialogControlValue ( dialog, NEW_MINIMIZABLE_BOX ) )
						gNewWindowType |= G_MINIMIZABLE;

					GExitModalDialogLoop ( G_OK_BUTTON );
					break;

				case G_CANCEL_BUTTON:
					GExitModalDialogLoop ( G_CANCEL_BUTTON );
					break;
					
				case NEW_DOCUMENT_RADIO:
				case NEW_FLOATING_RADIO:
					GSetSelectedDialogButton ( dialog, NEW_DOCUMENT_RADIO, NEW_FLOATING_RADIO, param1 );
					break;
				
				case NEW_MOVEABLE_BOX:
				case NEW_CLOSEABLE_BOX:
				case NEW_SIZEABLE_BOX:
				case NEW_MAXIMIZABLE_BOX:
				case NEW_MINIMIZABLE_BOX:
					GToggleControl ( dialog, param1 );
					break;
			}
			break;
	}
	
	return ( respond );
}

/**************************  DoEventDialogEvent  ***********************************/

int DoEventDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int		respond = TRUE;
	
	RecordEvent ( event, dialog, param1, param2 );
	
	switch ( event )
	{
		case G_CLOSE_EVENT:
			GHideWindow ( dialog );
			break;
				
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case EVENT_CLOSE_BUTTON:
					GCloseWindow ( dialog );
					break;
					
				case EVENT_RECORD_NULL_BOX:
					gRecordNullEvents = ! gRecordNullEvents;
					GSetDialogControlValue ( dialog, param1, gRecordNullEvents );
					break;

				case EVENT_RECORD_DRAG_BOX:
					gRecordMouseMoveDragEvents = ! gRecordMouseMoveDragEvents;
					GSetDialogControlValue ( dialog, param1, gRecordMouseMoveDragEvents );
                    break;
			}
	}

	return ( respond );
}

/**************************  RecordEvent  ***************************************/

void RecordEvent ( short event, GWindowPtr window, long param1, long param2 )
{
	int			i;
	char		title[256], text[ 256 * MAX_EVENT_HISTORY_LENGTH ] = { 0 };
	static char	*events[] =
	{
		"G_NULL_EVENT",
		"G_EVENT_LOOP_ENTRY",
		"G_KEY_DOWN_EVENT",
		"G_KEY_UP_EVENT",
		"G_SUSPEND_EVENT",
		"G_RESUME_EVENT",
		"G_MENU_EVENT",
		"G_OPEN_EVENT",
		"G_OPEN_FILE_EVENT",
		"G_PRINT_FILE_EVENT",
		"G_QUIT_EVENT",
		"G_INITIAL_EVENT",
		"G_SIZE_EVENT",
		"G_ACTIVATE_EVENT",
		"G_DEACTIVATE_EVENT",
		"G_UPDATE_EVENT",
		"G_MOUSE_DOWN_EVENT",
		"G_MOUSE_UP_EVENT",
		"G_DOUBLE_CLICK_EVENT",
		"G_MOUSE_MOVED_EVENT",
		"G_MOUSE_DRAG_EVENT",
		"G_CLOSE_EVENT",
		"G_CONTROL_EVENT",
	};
	
	if ( event == G_NULL_EVENT && gRecordNullEvents == FALSE )
		return;

	if ( ( event == G_MOUSE_DRAG_EVENT || event == G_MOUSE_MOVED_EVENT ) && gRecordMouseMoveDragEvents == FALSE )
		return;

	if ( gEventHistoryLength < MAX_EVENT_HISTORY_LENGTH )
		gEventHistoryLength++;

	for ( i = gEventHistoryLength - 1; i > 0; i-- )
		strcpy ( gEventHistory[i], gEventHistory[i - 1] );
		
	if ( window )
		GGetWindowTitle ( window, title );
	else
		strcpy ( title, "NULL" );
	
	sprintf ( gEventHistory[0], "%s (%d), %s, %ld, %ld\r",
	events[ event ], event, title, param1, param2 );

	if ( gEventHistoryDialog )
	{
		for ( i = 0; i < gEventHistoryLength; i++ )
			strcat ( text, gEventHistory[i] );
	
		GSetDialogItemText ( gEventHistoryDialog, EVENT_HISTORY_TEXT, text );
	}
}
