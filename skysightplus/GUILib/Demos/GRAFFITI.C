#include "GUILib.h"

#define DRAW_MENU_ID				128
#define DRAW_RECTANGLE				1
#define DRAW_LINE					2
#define DRAW_ELLIPSE				3

#define GRAFFITI_MENU_BAR_ID		256
#define GRAFFITI_MENU				1
#define COLOR_SUBMENU				6
#define SIZE_SUBMENU				7

#define GRAFFITI_MENU_ID			257
#define GRAFFITI_LINE_ITEM			1
#define GRAFFITI_RECTANGLE_ITEM		2
#define GRAFFITI_ELLIPSE_ITEM		3
#define GRAFFITI_RANDOM_ITEM		4
#define GRAFFITI_FILL_ITEM			8
#define GRAFFITI_QUIT_ITEM			10

#define COLOR_MENU_ID				6
#define COLOR_BLACK_ITEM			1
#define COLOR_WHITE_ITEM			2
#define COLOR_RED_ITEM				3
#define COLOR_YELLOW_ITEM			4
#define COLOR_GREEN_ITEM			5
#define COLOR_CYAN_ITEM				6
#define COLOR_BLUE_ITEM				7
#define COLOR_MAGENTA_ITEM			8
#define COLOR_RANDOM_ITEM			9

#define SIZE_MENU_ID				7
#define SIZE_1_PT_ITEM				1
#define SIZE_2_PT_ITEM				2
#define SIZE_4_PT_ITEM				3
#define SIZE_6_PT_ITEM				4
#define SIZE_8_PT_ITEM				5
#define SIZE_10_PT_ITEM				6

int			DoMainEvent ( short, GWindowPtr, long, long );
int			DoGraffitiMenuItem ( long );
int			DoColorMenuItem ( long );
int			DoSizeMenuItem ( long );
void		DoGraffitiPopupMenu ( GWindowPtr, short, short );
void		DrawGraffiti ( GWindowPtr, short );

short		gSizeItem = SIZE_1_PT_ITEM;
short		gColorItem = COLOR_RANDOM_ITEM;

GMenuPtr	gGraffitiMenu = NULL;
GMenuPtr	gColorMenu = NULL;
GMenuPtr	gSizeMenu = NULL;

short		gPenSize;
int			gPenFill;
GColor		gPenColor;

int GMain ( short event, GWindowPtr window, long param1, long param2 )
{
	int respond = TRUE;

	switch ( event )
	{
		case G_ENTRY_EVENT:
			GCreateWindow ( 1, "Graffiti", -1, -1, -1, -1, TRUE, G_APPLICATION_WINDOW, 0, NULL );
			GGetMenuBar ( GRAFFITI_MENU_BAR_ID );

			gGraffitiMenu = GGetMainMenu ( GRAFFITI_MENU );
			gColorMenu = GGetSubMenu ( gGraffitiMenu, COLOR_SUBMENU );
			gSizeMenu = GGetSubMenu ( gGraffitiMenu, SIZE_SUBMENU );

			gPenColor = *GBlackColor();
			gPenSize = 1;
			gPenFill = TRUE;

			GCheckMenuItem ( gGraffitiMenu, GRAFFITI_FILL_ITEM, gPenFill );
			GSetCheckedMenuItem ( gColorMenu, COLOR_BLACK_ITEM, COLOR_RANDOM_ITEM, gColorItem );
			GSetCheckedMenuItem ( gSizeMenu, SIZE_1_PT_ITEM, SIZE_10_PT_ITEM, gSizeItem );
			break;

		case G_MENU_EVENT:
			switch ( param1 )
			{
				case GRAFFITI_MENU_ID:
					DoGraffitiMenuItem ( param2 );
					break;
					
				case COLOR_MENU_ID:
					DoColorMenuItem ( param2 );
					break;
					
				case SIZE_MENU_ID:
					DoSizeMenuItem ( param2 );
					break;
			}
			break;

		case G_MOUSE_DOWN_EVENT:
			DoGraffitiPopupMenu ( window, param1, param2 );
			break;

		case G_CLOSE_EVENT:
			GExitMainLoop ( 0 );
			break;
	}
	
	return ( respond );
}

/**********************************  DoGraffitiMenuItem  ************************************/

int DoGraffitiMenuItem ( long item )
{
	switch ( item )
	{
		case GRAFFITI_LINE_ITEM:
		case GRAFFITI_RECTANGLE_ITEM:
		case GRAFFITI_ELLIPSE_ITEM:
		case GRAFFITI_RANDOM_ITEM:
			DrawGraffiti ( GGetActiveWindow(), item );
			break;
		
		case GRAFFITI_FILL_ITEM:
			gPenFill = ! gPenFill;
			GCheckMenuItem ( gGraffitiMenu, item, ! GIsMenuItemChecked ( gGraffitiMenu, item ) );
			break;
				
		case GRAFFITI_QUIT_ITEM:
			GExitMainLoop ( 0 );
			break;
	}

	return ( TRUE );
}

/*************************************  DoColorMenuItem  ****************************************/

int DoColorMenuItem ( long item )
{
	switch ( item )
	{
		case COLOR_RED_ITEM:
			gPenColor = *GRedColor();
			break;
			
		case COLOR_YELLOW_ITEM:
			gPenColor = *GYellowColor();
			break;
			
		case COLOR_GREEN_ITEM:
			gPenColor = *GGreenColor();
			break;
			
		case COLOR_CYAN_ITEM:
			gPenColor = *GCyanColor();
			break;
			
		case COLOR_BLUE_ITEM:
			gPenColor = *GBlueColor();
			break;
			
		case COLOR_MAGENTA_ITEM:
			gPenColor = *GMagentaColor();
			break;
			
		case COLOR_BLACK_ITEM:
			gPenColor = *GBlackColor();
			break;
			
		case COLOR_WHITE_ITEM:
			gPenColor = *GWhiteColor();
			break;
	}

    gColorItem = item;
	GSetCheckedMenuItem ( gColorMenu, COLOR_BLACK_ITEM, COLOR_RANDOM_ITEM, item );
	return ( TRUE );
}

/*********************************  DoSizeMenuItem  ******************************************/

int DoSizeMenuItem ( long item )
{
	char text[256];
	
	GSetCheckedMenuItem ( gSizeMenu, SIZE_1_PT_ITEM, SIZE_10_PT_ITEM, item );
	GGetMenuItemText ( gSizeMenu, item, text );
	sscanf ( text, "%hd", &gPenSize );
	
	return ( TRUE );
}

/********************************  DoGraffitiPopupMenu  ***************************************/

void DoGraffitiPopupMenu ( GWindowPtr window, short x, short y )
{
	GMenuPtr	menu;
	
	/*** To use the main "Graffiti" menu as a popup menu instead of generating our own,
	     comment out these lines... ***/
	     
	menu = GCreateMenu ( GRAFFITI_MENU_ID );
	GInsertMenuItem ( menu, GRAFFITI_MENU_ID, GRAFFITI_LINE_ITEM, "Draw Line" );
	GInsertMenuItem ( menu, GRAFFITI_MENU_ID, GRAFFITI_RECTANGLE_ITEM, "Draw Rectangle" );
	GInsertMenuItem ( menu, GRAFFITI_MENU_ID, GRAFFITI_ELLIPSE_ITEM, "Draw Ellipse" );
	GInsertMenuItem ( menu, GRAFFITI_MENU_ID, GRAFFITI_RANDOM_ITEM, "Draw Random Shape" );
	GWindowToScreen ( window, &x, &y );
	GDoPopupMenu ( menu, x, y, GRAFFITI_LINE_ITEM );
	GDeleteMenu ( menu );
	
	/*** ... and un-comment these
	
	GWindowToScreen ( window, &x, &y );
	GDoPopupMenu ( gGraffitiMenu, x, y, GRAFFITI_LINE_ITEM );

	***/
}

/**************************************  DrawGraffiti  *****************************************/

#define RANDOM(x,y) ( x + rand() % ( y - x + 1 ) )
#define MIN(x,y) ( x < y ? x : y )
#define MAX(x,y) ( x > y ? x : y )

void DrawGraffiti ( GWindowPtr window, short item )
{
	char		text[256];
	short		x1, y1, x2, y2;
	GRect		rect;
		
	GGetWindowContentRect ( window, &rect );
	
	x1 = RANDOM ( rect.left, rect.right );
	x2 = RANDOM ( rect.left, rect.right );
	y1 = RANDOM ( rect.top, rect.bottom );
	y2 = RANDOM ( rect.top, rect.bottom );

	if ( item == GRAFFITI_RANDOM_ITEM )
		item = RANDOM ( GRAFFITI_LINE_ITEM, GRAFFITI_ELLIPSE_ITEM );
	
	if ( gColorItem == COLOR_RANDOM_ITEM )
		GSetColor ( &gPenColor, RANDOM ( 0, 255 ), RANDOM ( 0, 255 ), RANDOM ( 0, 255 ) );

 	GStartDrawing ( window );
	GSetPen ( &gPenColor, gPenSize, gPenFill );

	switch ( item )
	{
		case GRAFFITI_LINE_ITEM:
			GMovePenTo ( x1, y1 );
			GDrawLineTo ( x2, y2 );
			break;
			
		case GRAFFITI_RECTANGLE_ITEM:
			GDrawRectangle ( MIN ( x1, x2 ), MIN ( y1, y2 ), MAX ( x1, x2 ), MAX ( y1, y2 ) );
			break;
			
		case GRAFFITI_ELLIPSE_ITEM:
			GDrawEllipse ( MIN ( x1, x2 ), MIN ( y1, y2 ), MAX ( x1, x2 ), MAX ( y1, y2 ) );
			break;
	}
	
	GEndDrawing ( window );
}
