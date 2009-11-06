#include "GUILib.h"

int GMain ( short event, GWindowPtr window, long param1, long param2 )
{
	switch ( event )
	{
		case G_ENTRY_EVENT:
			GCreateWindow ( 1, "NullApp", -1, -1, -1, -1, TRUE, G_APPLICATION_WINDOW, 0, NULL );
			break;

		case G_CLOSE_EVENT:
			GExitMainLoop ( 0 );
			break;
	}
}
