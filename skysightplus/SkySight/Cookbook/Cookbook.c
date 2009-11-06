/*** header files: we need to define the bool type, true, and false before
	 #including these. ***/

#include "ParLib.h"
#include "cookbook.h"

/*** private data ***/

short			currentReadoutLine;
short			sCameraReadoutMode;
unsigned short	systemPortRefNum;

extern IMAGE	sIMAGEs[ NUM_READOUT_MODES ];


// Struct that contains an address map of the parallel port register set.

ParallelPortMap	portBaseAddress;


/*** OpenCookbookDriver ******************************************************/

int OpenCookbookDriver ( unsigned short port )
	{
	
	// Remember which port driver we're opening in case we get an error.
	
	systemPortRefNum = port;
	
	//We have to set the, Camera Base Port and select a Sensor
	//before we call the Camera Class constructor.
	//The other items can be set at any time before we take an image or read
	//the Camera Parameters
	
	 //Set the Camera port base address.
	 
	 if( OpenParallelPort( port, &portBaseAddress ) != 0 )
		return false;
	
	// Call the Camera Constructor and Create the Camera Link

	CookBookCameraInitize();
	
	return ( TRUE );
		
	}

/*** CloseCookbookDriver *****************************************************/

int CloseCookbookDriver ( void )
{
	CloseParallelPort( systemPortRefNum );
	
	return ( TRUE );
}

/*** SetCookbookReadoutMode ***************************************************
	
	int SetCookbookReadoutMode ( short mode )
		
*******************************************************************************/

// This array will translate the Exposure Mode and Binning into
// a CookBook 245 exposure mode.


short CookBook245ExposureModeMap[ 8 ] =
	{ READOUT_HIRES, READOUT_MEDRES,   READOUT_LOWRES1,     READOUT_LOWRES2,
	  READOUT_FOCUS, READOUT_FOCUSLOW, READOUT_FOCUSMEDIUM, READOUT_FOCUSHIGH
	};

int SetCookbookReadoutMode ( short mode )
	{

	int result = FALSE;

	if( ( mode >= 1 ) && ( mode <= 8 ) )
		{
		
		sCameraReadoutMode = CookBook245ExposureModeMap[ mode - 1 ];
		
		result = TRUE;
		
		}
		
    return ( result );
	}

/*** GetCookbookReadoutMode ***************************************************/

int GetCookbookReadoutMode ( short *width, short *height, short *hbinning, short *vbinning )
{
	*width    = sIMAGEs[ sCameraReadoutMode ].x_res;
	*height   = sIMAGEs[ sCameraReadoutMode ].y_res;
	*hbinning = sIMAGEs[ sCameraReadoutMode ].hbin;
	*vbinning = sIMAGEs[ sCameraReadoutMode ].vbin;

	return ( TRUE );
}

/*** StartCookbookExposure ****************************************************/

int StartCookbookExposure( int light )
	{
	
	short x;
	
	
	// If you're going to use the Cookbook Subtract Reset feature you must
	// call CameraParameters a couple of times before begining the integration
	// to read the current Reset voltage.
			
	for( x = 0; x < 3; x++ )
		CameraParameters( &portBaseAddress );

	// If you're taking a darkframe, set TakingDark = true
	// for a light frame set TakingDark = false

	// If you're using LDC Control, set LDCControl.

	BeginExposure( &portBaseAddress, !light, true );

    return ( TRUE );

	}

/*** EndCookbookExposure *****************************************************/

int EndCookbookExposure ( void )
	{
	
	ClearStorageAndShiftImage( &portBaseAddress );
	
	currentReadoutLine = 0;

    return ( TRUE );

	}

/*** GetCookbookImageDataRow *************************************************/

int GetCookbookImageDataRow ( unsigned short *data )
	{

	// Do readout with subtracting the Reset voltage from the image as it is being read,
	// but don't use Double Sampling.

	ReadNextExposureLine( &portBaseAddress, true, false, sCameraReadoutMode, currentReadoutLine++, data );

    return ( TRUE );
    
	}
