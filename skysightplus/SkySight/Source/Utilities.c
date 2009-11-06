/*** COPYRIGHT NOTICE AND PUBLIC SOURCE LICENSE ***************************************

	Portions Copyright (c) 1992-2001 Southern Stars Systems.  All Rights Reserved.

	This file contains Original Code and/or Modifications of Original Code as
	defined in and that are subject to the Southern Stars Systems Public Source
	License Version 1.0 (the 'License').  You may not use this file except in
	compliance with the License.  Please obtain a copy of the License at

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

****************************************************************************************/

#include "SkySight.h"

/*** local functions ***/

static int		DoProgressDialogEvent ( short, GWindowPtr, long, long );

/*** local variables ***/

static int		sMessagesEnabled = TRUE;

/***  EnableMessages  *******************************************************

	Allows or prevents info/warning/error messages from being displayed.

	void EnableMessages ( int enable )
	
	(enable): TRUE to enable error messages; FALSE to disable them.
	
	If you pass FALSE to this function, the functions InfoMessage(),
	WarningMessage(), and ErrorMessage() will not display anything
	when called.  Pass TRUE to this function to re-enable error message
	display.
	
	Use the function MessagesEnabled() to determine whether or not
	error messages are enables.
	
*****************************************************************************/

void EnableMessages ( int enable )
{
	if ( enable )
		sMessagesEnabled = TRUE;
	else
		sMessagesEnabled = FALSE;
}

/***  MessagesEnabled  ******************************************************

	Determines whether info/warning/error messages are enabled.

	int MessagesEnabled ( void )

	The function returns TRUE if messages are enabled, and FALSE otherwise.
	When messages are disabled, the functions InfoMessage(), WarningMessage(),
	and ErrorMessage() will not display anything when called.
	
	Use the function EnableMessages() to enable/disable error messages.
	
*****************************************************************************/

int MessagesEnabled ( void )
{
	return ( sMessagesEnabled );
}

/***  InfoMessage  **********************************************************

	Displays an information message in an alert box.

	int InfoMessage ( short alertID, short stringID, ... )
	int WarningMessage ( short alertID, short stringID, ... )
	int ErrorMessage ( short alertID, short stringID, ... )

	(alertID):  flag indicating which alert box to use
	(stringID): ID number of string resource containing message to display
	(...):      additional printf()-type arguments to insert into message.
	
	If successful, the function displays an alert dialog box containing the
	specified message string.  It returns the number of the button the user
	pressed to exit the alert box; this will be G_OK_BUTTON, G_CANCEL_BUTTON,
	G_YES_BUTTON, G_NO_BUTTON, G_SAVE_BUTTON, or G_DONT_SAVE_BUTTON, depending
	on which alert was specified.  On failure, or if messages were disabled,
	the function returns zero.
	
	The functions InfoMessage(), WarningMessage, and ErrorMessage() differ
	only in the icon which is displayed in the message box.  InfoMessage()
	displays the "information" icon, and should be used for informational
	messages and user queries.  WarningMessage() displays the "warning" icon,
	and should be used to inform the user of non-fatal errors.  ErrorMessage()
	displays the "stop" icon, and should only be used to inform the user when
	a fatal error has occurred.
	
	The parameter (alertID) determines which alert will be displayed:
	
	G_OK_ALERT:
	displays an alert containing only a single "OK" button;
	
	G_OK_CANCEL_ALERT:
	displays an alert containing "OK" and "Cancel" buttons;
	
	G_YES_NO_CANCEL_ALERT:
	displays an alert containing "Yes", "No", and "Cancel"
	buttons;
	
	G_SAVE_DONT_SAVE_CANCEL_ALERT:
	displays an alert containing "Save", "Don't Save", and "Cancel" buttons
	(note that this is only implemented on the Macintosh; in Windows, this
	will display the "Yes"-"No"-"Cancel" alert).
	
	The parameter (stringID) is the resource ID number of a string resource
	containing the text of the message to display.  This message may contain
	printf()-style format specifiers (e.g. %d for integers, %f for floats);
	any additional arguments following (stringID) will be inserted into the
	string.
	
	When messages are disabled, the functions InfoMessage(), WarningMessage(),
	and ErrorMessage() will not display anything when called.  Use the function
	EnableMessages() to enable/disable messages.
	
*****************************************************************************/

int InfoMessage ( short alertID, short stringID, ... )
{
	int			result = 0;
	char		*string, message[256];
	va_list		ap;
	
	if ( sMessagesEnabled )
	{
		string = GString ( stringID );
		if ( string != NULL )
		{
			va_start ( ap, stringID );
			vsprintf ( message, string, ap );
			va_end ( ap );
	
			result = GDoAlert ( G_INFO_ALERT, alertID, message );
		}
	}
	
	return ( result );
}

/***  WarningMessage  ********************************************************/

int WarningMessage ( short alertID, short stringID, ... )
{
	int		result = 0;
	char	*string, message[256];
	va_list	ap;
	
	if ( sMessagesEnabled )
	{
		string = GString ( stringID );
		if ( string != NULL )
		{
			va_start ( ap, stringID );
			vsprintf ( message, string, ap );
			va_end ( ap );
	
			result = GDoAlert ( G_INFO_ALERT, alertID, message );
		}
	}

	return ( result );
}

/***  ErrorMessage  ********************************************************/

int ErrorMessage ( short alertID, short stringID, ... )
{
	int			result = 0;
	char		*string, message[256];
	va_list		ap;
	
	if ( sMessagesEnabled )
	{
		string = GString ( stringID );
		if ( string != NULL )
		{
			va_start ( ap, stringID );
			vsprintf ( message, string, ap );
			va_end ( ap );
	
			result = GDoAlert ( G_INFO_ALERT, alertID, message );
		}
	}

	return ( result );
}

/***  ShowProgressDialog  ***********************************************

	Creates and displays the "Progress" dialog box.

	void ShowProgressDialog ( int hasCancel, long numTasks )
	
	(hasCancel): indicates whether the dialog's cancel button
	             should be visible.
	(numTasks):  number of tasks to be performed while the dialog
	             is visible.
	
	The function returns a pointer to the progress dialog window,
	if successful, or NULL on failure.
	
	Use the "Progress" dialog to indicate the status of a long task
	(or number of tasks).  After calling this function, you may call
	StarProgressDialogTask() to begin individual tasks, and Update-
	ProgressDialogTask() to update the status of each task as work
	toward its completion progresses.  Finally, call HideProgressDialog()
	to hide the working dialog when all tasks have been completed.
	
**************************************************************************/

#define PROGRESS_DIALOG					275
#define PROGRESS_TASK_TEXT				1
#define PROGRESS_CANCEL_BUTTON			2
#define PROGRESS_INDICATOR_CONTROL		3
	
static int DoProgressDialogEvent ( short, GWindowPtr, long, long );

GWindowPtr			sProgressDialog = NULL;
static short		sProgressDialogResult = TRUE;
static long			sProgressDialogNumTasks = 1;
static long			sProgressDialogTaskNum = 0;
static long			sProgressDialogTaskMin = 0;
static long			sProgressDialogTaskMax = 1;
static long			sProgressDialogMax = 1;

static short		sProgressControlValue = 0;
static short		sProgressControlMin = 0;
static short		sProgressControlMax = 100;

GWindowPtr ShowProgressDialog ( int hasCancel, long numTasks )
{
	GRect			rect;
	GControlPtr		control;
	
	/*** If we haven't yet done so, create the dialog.
	     On failure, return NULL. ***/
	     
	if ( sProgressDialog == NULL )
		sProgressDialog = GCreateDialog ( PROGRESS_DIALOG, 0, DoProgressDialogEvent );

	if ( sProgressDialog == NULL )
		return ( NULL );
	
	/*** Initialize the task counter(s) and result code. ***/
	
	sProgressDialogNumTasks = numTasks;
	sProgressDialogTaskNum = -1;
	sProgressDialogResult = TRUE;
	
	/*** Set the progress bar control's minimum and maximum values
	     equal to zero and the width of the control (since the control
	     can't display progress information with resolution greater
	     than a single pixel.) ***/
	
	control = GGetControl ( sProgressDialog, PROGRESS_INDICATOR_CONTROL );
	GGetControlRect ( control, &rect );
	
	sProgressControlMin   = 0;
	sProgressControlMax   = rect.right - rect.left;
	sProgressControlValue = 0;
	
	GSetControlMin   ( control, sProgressControlMin );
	GSetControlMax   ( control, sProgressControlMax );
	GSetControlValue ( control, sProgressControlValue );
	
	/*** Show or hide the dialog's "Cancel" button, as appropriate. ***/
	
	if ( hasCancel )
		GShowDialogItem ( sProgressDialog, PROGRESS_CANCEL_BUTTON );
	else
		GHideDialogItem ( sProgressDialog, PROGRESS_CANCEL_BUTTON );
		
	GShowModalDialog ( sProgressDialog );
	return ( sProgressDialog );
}

/***  StartProgressDialogTask  **********************************************

	Displays the name of a new task to begin in the "Progress" dialog box.
	
	void StartProgressDialogTask ( char *taskName, long taskMin, long taskMax )
	
	(taskName): ASCII NUL-terminated string containing name of task.
	(taskMin):  Progress value indicating start of task.
	(taskMax):  Progress value indicating end of task.
	
	Use this function when you want to begin a new task whose progress
	you wish to display in the "Progress" dialog box.  Make sure that
	you've already displayed the Progress dialog box by calling
	ShowProgressDialog() before calling this function!
	
	The parameter (taskName) is a pointer to a string which contains the
	name of the task (e.g. "Copying file...")
	
	The parameters (taskMin) and (taskMax) are arbitary values which you
	can use to indicate the start and end of the task (e.g. 0 and 100 to
	indicate percentage of file copied.)  As work on the task progresses,
	you should call UpdateProgressDialogTask() with a value between this
	minimum and maximum to show the amount of work completed.
	
*****************************************************************************/

void StartProgressDialogTask ( char *taskName, long taskMin, long taskMax )
{
	if ( sProgressDialog != NULL )
	{
		sProgressDialogTaskNum++;
		sProgressDialogTaskMin = taskMin;
		sProgressDialogTaskMax = taskMax;
		sProgressDialogMax = sProgressDialogNumTasks * ( taskMax - taskMin );
		
		if ( sProgressDialogMax == 0 )
			sProgressDialogMax = 1;
			
		GSetDialogItemText ( sProgressDialog, PROGRESS_TASK_TEXT, taskName );
		UpdateProgressDialogTask ( taskMin );
	}
}

/***  UpdateProgressDialogTask  *********************************************

	Updates the status of the task currently displayed in the "Progress"
	dialog box.
	
	short UpdateProgressDialogTask ( long status )
	
	(status): current progress dialog box task status.
	
	The function returns FALSE if the user has pressed the dialog's
	"Cancel" button, or TRUE otherwise.  Note that if the user presses
	the "Cancel" button, this function does NOT hide the dialog or
	interrupt the task in progress; it is up to the calling function
	to decide how that situation should be handled.  This function
	merely informs the caller that the user wants to cancel the
	operation.
	
	The status value you pass should be between the task minimum and
	maximum which were given in the preceding call to
	StartProgressDialogTask().
	
	In addition to updating the value of the dialog's progress indicator
	control to reflect the status of the current task, this function also
	periodically handles background events.  This will allow some user
	interaction while work on the task is progressing.  Specifically,
	the user will be able to move the dialog box on the screen, and/or
	click in the dialog's "Cancel" button (or, equivalently, hit the
	"escape" or "command-period" keys).  Since the progress dialog box
	is modal, the user cannot interact with the menus or with any other
	window while the dialog is present on the screen.  The user can,
	however, switch to another application, and work on the task should
	then continue in the background.  The periodic event-handling performed
	by this function should allow background applications to recieve CPU
	time and continue their work on non-preemptively-multitasking operating
	systems.
	
*****************************************************************************/

short UpdateProgressDialogTask ( long status )
{
	short			value;
	static long		lastEventTick = 0;
	GEvent			event;
	GControlPtr		control;
	
	/*** First, update the progress indicator control value. ***/
	
	if ( sProgressDialog != NULL )
	{
		status = sProgressDialogTaskNum * ( sProgressDialogTaskMax - sProgressDialogTaskMin )
		       + status - sProgressDialogTaskMin;
		
		control = GGetControl ( sProgressDialog, PROGRESS_INDICATOR_CONTROL );
		value   = status * ( sProgressControlMax - sProgressControlMin ) / sProgressDialogMax;
		
		if ( value != sProgressControlValue )
		{
			GSetControlValue ( control, sProgressControlValue );
			sProgressControlValue = value;
		}
	}

	/*** Now perform the background event handling.  We want to handle user
	     events frequently enough to prevent the program from feeling "sluggish",
	     but not so frequently that the overhead of handling the events significantly
	     increases the time needed to complete the task.  After much trial and error,
	     the "right" event-handling rate seems to be about 10 events/second. ***/
	
	if ( ( GGetTickCount() - lastEventTick ) > G_TICKS_PER_SECOND / 10 )
	{
		lastEventTick = GGetTickCount();

		while ( TRUE )
		{
			GGetNextEvent ( &event );
			GProcessEvent ( &event );

            /*** If there are no more events waiting to be processed,
                 we can stop now. ***/
                 
            if ( GEventPending() == FALSE )
            	break;

            /*** If the user has pressed the dialog's "cancel" button,
                 we can also stop. ***/
                 
            if ( sProgressDialogResult == FALSE )
				break;

			/*** If we've been processing events for more than a tenth of
                 a second, we should take a break and let the application
                 go back to whatever else it was doing; we'll get to the
                 rest of the events next time around.  The Windows version
                 of the program occasionally locks up if this code is missing;
                 for some reason, Windows sometimes thinks the app always has
                 messages pending. ***/

            if ( GGetTickCount() - lastEventTick > G_TICKS_PER_SECOND / 10 )
            	break;
		}

		lastEventTick = GGetTickCount();
	}

	return ( sProgressDialogResult );
}

/***  HideProgressDialog  **********************************************

	Hides the "Progress" dialog box.
	
	void HideProgressDialog ( void )

	Call this function when all of the tasks displayed in the progress
	dialog box have been completed, or if the user has cancelled the
	operation in progress.
	
************************************************************************/

void HideProgressDialog ( void )
{
	if ( sProgressDialog != NULL )
		GHideModalDialog ( sProgressDialog );
}

/***  DoProgressDialogEvent  ***********************************************

	This is the standard event-handling procedure for the "Progress"
	dialog box.
	
****************************************************************************/

int DoProgressDialogEvent ( short event, GWindowPtr dialog, long param1, long param2 )
{
	int respond = TRUE;
	
	switch ( event )
	{
		case G_CONTROL_EVENT:
			switch ( param1 )
			{
				case G_CANCEL_BUTTON:
					sProgressDialogResult = FALSE;
					break;
			}
			break;
	}
	
	return ( respond );
}
