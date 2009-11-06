/////////////////////////////////
//  cb245.c

/******************************************************************************
(C) Copyright 1997 by Intricate Micro Systems, All Rights Reserved.

This Camera Class Library is the Copyrighted intellectual property of
Intricate Micro Systems.  It may be used as is or modified for private use,
but any Commercial use is strictly forbidden without the express written
consent of Intricate Micro Systems.
******************************************************************************/

#include "ParLib.h"
#include "Cookbook.h"


short			UnbinnedXRes;
short			ShutterState;
short			DummyPixels;
short			StoragePlusSerialRows;
short			StorageRows;
short			SerialRegisterColumns;
short			SensorRows;
short			SensorColumns;
short			PixelColumns;
float			HPixelSize;
float			VPixelSize;
IMAGE			sIMAGEs[ NUM_READOUT_MODES ];
unsigned short	ReferenceVoltage;
unsigned short	ResetVoltage;
unsigned short	rawLine[ 1024 ];


/******************************************************************************
	CookBook definitions
******************************************************************************/
/*
		Data Port Pin assignments
			xxxx xxxx
			|||| |||\_ srg1
			|||| ||\__ srg2
			|||| |\___ srg3
			|||| \____ trg
			|||\______ mux1 (inverted)
			||\_______ mux2 (inverted)
			|\________ sag
			\_________ iag
*/

#define All_Low 0x00
#define srg1 	0x01     // Shift register gate 1
#define srg2 	0x02     // Shift register gate 2
#define srg3 	0x04     // Shift register gate 3
#define trg  	0x08     // Transfer register gate
#define mux1 	0x10     // Multiplexer 1
#define mux2 	0x20     // Multiplexer 2
#define sag  	0x40     // Image area gate
#define iag  	0x80     // Storage area gate

#define high_nibble		mux1
#define middle_nibble	mux2
#define low_nibble		mux1 | mux2


/*
		Status Port Pin assignments
			xxxx xxxx
			|||| |||\_ Not used
			|||| ||\__ Not used
			|||| |\___ Not used
			|||| \____ Not used
			|||\______ ADC data bit 0 (not inverted)
			||\_______ ADC data bit 1 (not inverted)
			|\________ ADC data bit 2 (not inverted)
			\_________ ADC data bit 3 (inverted)


		Control Port Pin assignments
			xxxx xxxx
			|||| |||\_ ADC
			|||| ||\__ LDC
			|||| |\___ Shutter
			|||\______ Not used
			||\_______ Not used
			|\________ Not used
			\_________ Not used
*/

#define ADCready   		0x00
#define ADCconvert 		0x01
#define LDCHigh    		0x00
#define LDCLow     		0x02
#define ShutterOpen		0x04
#define ShutterClosed	0x00

#define kPreSampleTime	12
	
// Set number of times to clear the array before beginning an integration

#define kArrayClears	5
	
// Time in milliseconds to delay after sending Close Shutter pulse to
// Start of array clearing

#define kShutterDelay	10
	
// Set the OpAmp settling delay

#define kOpAmp_delay	4
	
// Set the Analog to Digital converter delay

#define kADC_delay		7


//*************************************************************
//
// FUNCTION : CookBookCameraInitize
//
//*************************************************************

void
CookBookCameraInitize( void )
	{
	
	HPixelSize            = 8.5;
	VPixelSize            = 19.75;
	DummyPixels           = 11;
	SensorColumns         = 252;
	SensorRows            = 242;
	StorageRows           = 245;
	SerialRegisterColumns = 273;
	StoragePlusSerialRows = 247;
	PixelColumns          = SensorColumns * 3;
	ShutterState          = ShutterClosed;

	InitImageData( sIMAGEs );

	}


//*************************************************************
//
// FUNCTION : CameraParameters
//
//*************************************************************

void
CameraParameters( ParallelPortMap *	port )
	{

	unsigned short	high, middle, low;
	short			x;
	
	
	LDCOff( port );
	
	storage_area_clear( port );
	
	serial_register_clear( port );
	
	image_area_clear( port );
	
	ShiftDownOneImageLine( port );
	
	serial_register_clear( port );
	
	outportb( port->PORT_DATA, All_Low);
	outportb( port->PORT_DATA, srg1);
	
	outportb( port->PORT_DATA, All_Low);
	outportb( port->PORT_DATA, srg2);
	
	outportb( port->PORT_DATA, All_Low);
	outportb( port->PORT_DATA, srg3);
	
	outportb( port->PORT_DATA, All_Low);
	outportb( port->PORT_DATA, srg1);
	
	outportb( port->PORT_DATA, All_Low);
	outportb( port->PORT_DATA, srg2);
	outportb( port->PORT_DATA, All_Low);
 
	outportb( port->PORT_DATA, srg3 );
	delay_microseconds( kPreSampleTime );
	
	outportb( port->PORT_CONTROL, ADCready | LDCLow | ShutterState );
	delay_microseconds( kOpAmp_delay );

	outportb( port->PORT_CONTROL, ADCconvert | LDCLow | ShutterState );

	outportb( port->PORT_DATA, srg3 );
	delay_microseconds( kADC_delay );
  
	outportb( port->PORT_DATA, All_Low);
	outportb( port->PORT_DATA, middle_nibble | srg1 );
	middle = inportb( port->PORT_STATUS );
	
	outportb( port->PORT_DATA, All_Low);
	outportb( port->PORT_DATA, high_nibble | srg2 );
	high = inportb( port->PORT_STATUS );
	
	outportb( port->PORT_DATA, All_Low);
	outportb( port->PORT_DATA, low_nibble | srg3 );
	low = inportb( port->PORT_STATUS );
	
	ResetVoltage = ( ( ( high & 0x00F0 ) << 4 ) |
				   ( middle & 0x00F0 ) |
				   ( ( low & 0x00F0 ) >> 4 ) ) ^ 0x888;
	
	outportb( port->PORT_DATA, All_Low );
	outportb( port->PORT_DATA, srg1 );
	
	outportb( port->PORT_DATA, All_Low );
	outportb( port->PORT_DATA, srg2 );
	outportb( port->PORT_DATA, All_Low );
	
	for( x = 0; x < DummyPixels + 1; x++ )
		outportb( port->PORT_DATA, srg1 | srg3 );

	outportb( port->PORT_CONTROL, ADCready | LDCLow | ShutterState );
	delay_microseconds( kOpAmp_delay );

	outportb( port->PORT_CONTROL, ADCconvert | LDCLow | ShutterState );
	
	outportb( port->PORT_DATA, srg1 | srg3 );
	delay_microseconds( kADC_delay );
	
	outportb( port->PORT_DATA, All_Low );
	outportb( port->PORT_DATA, middle_nibble | srg1 );
	middle = inportb( port->PORT_STATUS );
	
	outportb( port->PORT_DATA, All_Low );
	outportb( port->PORT_DATA, high_nibble | srg2 );
	high = inportb( port->PORT_STATUS );
	
	outportb( port->PORT_DATA, low_nibble | srg3 );
	low = inportb( port->PORT_STATUS );
	
	 ReferenceVoltage = ( ( ( high & 0x00F0 ) << 4 ) |
	 					  ( middle & 0x00F0 ) |
	 					  ( ( low & 0x00F0 ) >> 4 ) ) ^ 0x888;

	}


//*************************************************************
//
// FUNCTION : BeginExposure
//
//*************************************************************

void
BeginExposure( ParallelPortMap *	port,
			   BOOL				TakingDark,
			   BOOL				LDCControl )
	{
	
	short i;
	
	
	storage_area_clear( port );
	
	serial_register_clear( port );
	
	image_area_clear( port );
	
	OpenCloseShutter( port, kShutterDelay, TakingDark );
		
	for( i = 0; i < kArrayClears; i++ )
		{
		
		storage_area_clear( port );
		
		serial_register_clear( port );
		
		image_area_clear( port );
		
		if( LDCControl )
			LDCOn( port );
			
		}
		
	}


//*************************************************************
//
// FUNCTION : ReadNextExposureLine
//
//*************************************************************

void
ReadNextExposureLine( ParallelPortMap *	port,
					  BOOL				SubReset,
					  BOOL				Dbl_smpl,
					  short				readoutMode,
					  short				y,
					  unsigned short *	lineData )
	{
	
	short			x, tmpy, p;
	short			binIndex;
	unsigned long	binned;
	IMAGE *			image = &sIMAGEs[ readoutMode ];
	unsigned short	tempResetVoltage;


//	tempResetVoltage = SubReset ? ( ResetVoltage + 100 ) : 0;
	tempResetVoltage = SubReset ? ResetVoltage : 0;
	
	switch( readoutMode )
		{

		case READOUT_HIRES:
		case READOUT_MEDRES:
		case READOUT_LOWRES1:
			{
			
			read_whole_line_not_binned( port, rawLine, Dbl_smpl );

			p = 0;
			
			for ( x = 0; x < UnbinnedXRes; x += image->hbin )
				{
				
				binned = 0;
				
				for( binIndex = 0; binIndex < image->hbin; binIndex++ )
					binned += ( rawLine[ x + binIndex ] - tempResetVoltage );
					
				binned /= image->hbin;  // bring back to 12 bit
				
				if( binned > image->dynamic_range )
					binned = image->dynamic_range;
					
				lineData[ p ] = (unsigned short)binned;
				
				p++;
				
				}

			}
		break;

		case READOUT_LOWRES2:
			{
			
			shift_down_three_lines_and_bin( port );
			
			read_internal_binned_line( port, rawLine, Dbl_smpl );
			
			for( x = 0; x < image->x_res; x++ )
				{
				
				lineData[ x ] = rawLine[ x ] - tempResetVoltage;
				
				}
				
			}
		break;

		case READOUT_FOCUSHIGH:
		case READOUT_FOCUSMEDIUM:
		case READOUT_FOCUSLOW:
			{
			
			// Is this is the first line to be read out?
			
			if( y == 0 )
				{
					
				for( tmpy = 0; tmpy < ( 121 - image->y_res / 2 ); tmpy++ )
					shift_down_three_lines_and_bin( port );
				
				}
				
			shift_down_three_lines_and_bin( port );
			
			read_internal_binned_line( port, rawLine, FALSE );
			
			for( x = ( 126 - image->x_res / 2 ); x < ( 126 - image->x_res / 2 ) + image->x_res; x++ )
				{
				
				lineData[ x - ( 126 - image->x_res / 2 ) ] = ( rawLine[ x ] - tempResetVoltage );
					
				}
					
			}
		break;
		
		// This mode returns the center quarter image (121v x 126h) out of
		// the low res image area (242v x 252h) starting at (60v x 63h).
		
		case READOUT_FOCUS:
			{
			
			// Is this is the first line to be read out?
			
			if( y == 0 )
				{
				
				// Move the center quarter strip down to the serial register.
				
				for( tmpy = 0; tmpy < 60; tmpy++ )
					shift_down_three_lines_and_bin( port );
			
				}
			
			// Bin the next three lines.
			
			shift_down_three_lines_and_bin( port );
			
			read_internal_binned_line( port, rawLine, FALSE );
			
			// Return the data from the center quarter strip.
			
			for( x = 63; x < 189; x++ )
				{
				
				lineData[ x - 63 ] = ( rawLine[ x ] - tempResetVoltage );
				
				}
				
			}
		break;

		case READOUT_FIND:
			{

			shift_find_line_binned( port );

			read_internal_binned_line( port, rawLine, FALSE );

			p = 0;

			for( x = 0; x < SensorColumns; x = x + 2 )
				{

				lineData[ p ] = ( rawLine[ x ] - tempResetVoltage ) + ( rawLine[ x + 1 ] - tempResetVoltage );

				lineData[ p ] /= 2; /////// bring back to 12 bit

				if( lineData[ p ] > image->dynamic_range )
					lineData[ p ] = image->dynamic_range;

				p++;

				}

			}
		break;

		}

	}


//*************************************************************
//
// FUNCTION : InitImageData
//
//*************************************************************

void
InitImageData( IMAGE *	image )
	{
	
	image[ READOUT_HIRES ].x_res               = PixelColumns - 1;
	image[ READOUT_HIRES ].y_res               = SensorRows;
	image[ READOUT_HIRES ].hbin                = 1;
	image[ READOUT_HIRES ].vbin                = 1;
	image[ READOUT_HIRES ].dynamic_range       = Resolution_12bit;
	
	image[ READOUT_MEDRES ].x_res              = PixelColumns / 2;
	image[ READOUT_MEDRES ].y_res              = SensorRows;
	image[ READOUT_MEDRES ].hbin               = 2;
	image[ READOUT_MEDRES ].vbin               = 1;
	image[ READOUT_MEDRES ].dynamic_range      = Resolution_12bit;
	
	image[ READOUT_LOWRES1 ].x_res             = PixelColumns / 3;
	image[ READOUT_LOWRES1 ].y_res             = SensorRows;
	image[ READOUT_LOWRES1 ].hbin              = 3;
	image[ READOUT_LOWRES1 ].vbin              = 1;
	image[ READOUT_LOWRES1 ].dynamic_range     = Resolution_12bit;
	
	image[ READOUT_LOWRES2 ].x_res             = SensorColumns;
	image[ READOUT_LOWRES2 ].y_res             = SensorRows;
	image[ READOUT_LOWRES2 ].hbin              = 3;
	image[ READOUT_LOWRES2 ].vbin              = 1;
	image[ READOUT_LOWRES2 ].dynamic_range     = Resolution_12bit;
	
	image[ READOUT_FIND ].x_res                = SensorColumns / 2;
	image[ READOUT_FIND ].y_res                = SensorRows / 2;
	image[ READOUT_FIND ].hbin                 = 6;
	image[ READOUT_FIND ].vbin                 = 2;
	image[ READOUT_FIND ].dynamic_range        = Resolution_13bit;
	
	image[ READOUT_FOCUS ].x_res               = SensorColumns / 2;
	image[ READOUT_FOCUS ].y_res               = SensorRows / 2;
	image[ READOUT_FOCUS ].hbin                = 3;
	image[ READOUT_FOCUS ].vbin                = 1;
	image[ READOUT_FOCUS ].dynamic_range       = Resolution_12bit;
	
	image[ READOUT_FOCUSLOW ].x_res            = SensorColumns / 2;
	image[ READOUT_FOCUSLOW ].y_res            = SensorRows / 2;
	image[ READOUT_FOCUSLOW ].hbin             = 3;
	image[ READOUT_FOCUSLOW ].vbin             = 1;
	image[ READOUT_FOCUSLOW ].dynamic_range    = Resolution_12bit;
	
	image[ READOUT_FOCUSMEDIUM ].x_res         = SensorColumns / 4;
	image[ READOUT_FOCUSMEDIUM ].y_res         = SensorRows /  4;
	image[ READOUT_FOCUSMEDIUM ].hbin          = 3;
	image[ READOUT_FOCUSMEDIUM ].vbin          = 1;
	image[ READOUT_FOCUSMEDIUM ].dynamic_range = Resolution_12bit;
	
	image[ READOUT_FOCUSHIGH ].x_res           = SensorColumns / 8;
	image[ READOUT_FOCUSHIGH ].y_res           = SensorRows / 8;
	image[ READOUT_FOCUSHIGH ].hbin            = 3;
	image[ READOUT_FOCUSHIGH ].vbin            = 1;
	image[ READOUT_FOCUSHIGH ].dynamic_range   = Resolution_12bit;
	
	UnbinnedXRes = PixelColumns - 1;

	}


//*************************************************************
//
// FUNCTION : read_whole_line_not_binned
//
//*************************************************************

void
read_whole_line_not_binned( ParallelPortMap *	port,
							unsigned short *	lineBuffer,
							BOOL				Dbl_smpl )
	{
	
	short			nline;
	unsigned char	high, middle, low;
	short			a;
	unsigned short	temp;
	
	
	LDCOff( port );
	
	outportb( port->PORT_DATA, sag );
	outportb( port->PORT_DATA, All_Low );
	
	for( nline = 0; nline < 3; nline++ )
		{
			
		outportb( port->PORT_DATA, trg );
		outportb( port->PORT_DATA, All_Low );
		
		outportb( port->PORT_DATA, srg1 | srg2 | srg3 );
		outportb( port->PORT_DATA, All_Low );

		for( a = 0; a < DummyPixels + 1; a++ )
			{
					
			outportb( port->PORT_DATA, All_Low );
			outportb( port->PORT_DATA, srg1 );
			
			outportb( port->PORT_DATA, All_Low );
			outportb( port->PORT_DATA, srg2 );
			
			outportb( port->PORT_DATA, All_Low );
			outportb( port->PORT_DATA, srg3 );
			
			}
			
		for( a = 0; a < SensorColumns + 1; a++ )
			{
					
			outportb( port->PORT_DATA, All_Low );
			outportb( port->PORT_DATA, srg1 );
			
			outportb( port->PORT_DATA, All_Low );
			outportb( port->PORT_DATA, srg2 );
			
			outportb( port->PORT_DATA, All_Low );
			outportb( port->PORT_DATA, srg3 );
				
			outportb( port->PORT_CONTROL, ADCconvert | LDCLow | ShutterState );
			delay_microseconds( kOpAmp_delay );
		  
			outportb( port->PORT_CONTROL, ADCready | LDCLow | ShutterState );
			delay_microseconds( kADC_delay );
		  
			outportb( port->PORT_CONTROL, ADCconvert | LDCLow | ShutterState );
			
			outportb( port->PORT_DATA, high_nibble );
			high = inportb( port->PORT_STATUS );
			
			outportb( port->PORT_DATA, middle_nibble );
			middle = inportb( port->PORT_STATUS );
			
			outportb( port->PORT_DATA, low_nibble );
			low = inportb( port->PORT_STATUS );
	
			lineBuffer[ ( a * 3 ) + nline ] = ( ( ( high & 0x00F0 ) << 4 ) |
												( middle & 0x00F0 ) |
												( ( low & 0x00F0 ) >> 4 ) ) ^ 0x888;
	
			if( Dbl_smpl )
				{
					
				outportb( port->PORT_DATA, high_nibble );
				high = inportb( port->PORT_STATUS );
				
				outportb( port->PORT_DATA, middle_nibble );
				middle = inportb( port->PORT_STATUS );
				
				outportb( port->PORT_DATA, low_nibble );
				low = inportb( port->PORT_STATUS );
				
				temp = ( ( ( high & 0x00F0 ) << 4 ) |
						 ( middle & 0x00F0 ) |
						 ( ( low & 0x00F0 ) >> 4 ) ) ^ 0x888;
										
				lineBuffer[ ( a * 3 ) + nline ] = ( lineBuffer[ ( a * 3 ) + nline ] + temp ) / 2;
	
				}
	
			}
	
		}
	
	}


//*************************************************************
//
// FUNCTION : read_internal_binned_line
//
//*************************************************************

void
read_internal_binned_line( ParallelPortMap *	port,
						   unsigned short *	lineBuffer,
						   BOOL				Dbl_smpl )
	{
	
	unsigned short	high, middle, low;
	short			a;
	unsigned short	temp;
		
	
	LDCOff( port );
	
	for( a = 0; a < DummyPixels + 1; a++ )
		{
		
		outportb( port->PORT_DATA, All_Low );
		outportb( port->PORT_DATA, srg1 );
		
		outportb( port->PORT_DATA, All_Low );
		outportb( port->PORT_DATA, srg2 );
		
		outportb( port->PORT_DATA, All_Low );
		outportb( port->PORT_DATA, srg3 );
		
		}
	
	for( a = 0; a < SensorColumns + 1; a++ )
		{
		
		outportb( port->PORT_DATA, All_Low );
		outportb( port->PORT_DATA, srg1 );
		
		outportb( port->PORT_DATA, All_Low );
		outportb( port->PORT_DATA, srg2 );
		
		outportb( port->PORT_DATA, All_Low );
		outportb( port->PORT_DATA, srg3 );
		
		outportb( port->PORT_CONTROL, ADCconvert | LDCLow | ShutterState );
		delay_microseconds( kOpAmp_delay );
		
		outportb( port->PORT_CONTROL, ADCready | LDCLow | ShutterState );
		delay_microseconds( kADC_delay );
		 
		outportb( port->PORT_CONTROL, ADCconvert | LDCLow | ShutterState );

		outportb( port->PORT_DATA, high_nibble );
		high = inportb( port->PORT_STATUS );
		
		outportb( port->PORT_DATA, middle_nibble );
		middle = inportb( port->PORT_STATUS );
		
		outportb( port->PORT_DATA, low_nibble );
		low = inportb( port->PORT_STATUS );
		
		lineBuffer[ a ] = ( ( ( high & 0x00F0 ) << 4 ) |
	  						( middle & 0x00F0 ) |
	  						( ( low & 0x00F0 ) >> 4 ) ) ^ 0x888;
	  
		if( Dbl_smpl )
			{
			
			outportb( port->PORT_DATA, high_nibble);
			high = inportb( port->PORT_STATUS );
			
			outportb( port->PORT_DATA, middle_nibble);
			middle = inportb( port->PORT_STATUS );
			
			outportb( port->PORT_DATA, low_nibble);
			low = inportb( port->PORT_STATUS );
			
			temp = ( ( ( high & 0x00F0 ) << 4 ) |
					 ( middle & 0x00F0 ) |
					 ( ( low & 0x00F0 ) >> 4 ) ) ^ 0x888;

			lineBuffer[ a ] = ( lineBuffer[ a ] + temp ) / 2;

			}

		}

	}


//*************************************************************
//
// FUNCTION : ClearStorageAndShiftImage
//
//*************************************************************

void
ClearStorageAndShiftImage( ParallelPortMap *	port )
	{
	
	LDCOff( port );
	
	storage_area_clear( port );
	
	shift_image_to_storage( port );
	
	}


//*************************************************************
//
// FUNCTION : ShiftDownOneImageLine
//
//*************************************************************

void
ShiftDownOneImageLine( ParallelPortMap *	port )
	{
	
	LDCOff( port );
	
	outportb( port->PORT_DATA, trg );
	outportb( port->PORT_DATA, All_Low );
	
	outportb( port->PORT_DATA, srg1 | srg2 | srg3 );
	outportb( port->PORT_DATA, All_Low );
	
	}


//*************************************************************
//
// FUNCTION : shift_down_three_lines_and_bin
//
//*************************************************************

void
shift_down_three_lines_and_bin( ParallelPortMap *	port )
	{
	
	LDCOff( port );
	
	outportb( port->PORT_DATA, trg );
	outportb( port->PORT_DATA, All_Low );
	
	outportb( port->PORT_DATA, trg );
	outportb( port->PORT_DATA, All_Low );
	
	outportb( port->PORT_DATA, trg );
	outportb( port->PORT_DATA, srg1 | srg2 | srg3 );
	outportb( port->PORT_DATA, sag );
	outportb( port->PORT_DATA, All_Low );
	
	}


//*************************************************************
//
// FUNCTION : shift_find_line_binned
//
//*************************************************************

void
shift_find_line_binned( ParallelPortMap *	port )
	{
	
	LDCOff( port );
	
	outportb( port->PORT_DATA, trg );
	outportb( port->PORT_DATA, All_Low );
	
	outportb( port->PORT_DATA, trg );
	outportb( port->PORT_DATA, All_Low );
	
	outportb( port->PORT_DATA, trg );
	outportb( port->PORT_DATA, srg1 | srg2 | srg3 );
	outportb( port->PORT_DATA, sag );
	outportb( port->PORT_DATA, All_Low );
	
	outportb( port->PORT_DATA, sag );
	outportb( port->PORT_DATA, All_Low );
	
	}


//*************************************************************
//
// FUNCTION : storage_area_clear
//
//*************************************************************

void
storage_area_clear( ParallelPortMap *	port )
	{
	
	short x;
	
	
	LDCOff( port );
	
	outportb( port->PORT_DATA, All_Low );
	
	for( x = 0; x < StoragePlusSerialRows + 1; x++ )
		{
		
		outportb( port->PORT_DATA, trg );
		outportb( port->PORT_DATA, sag | trg );
		outportb( port->PORT_DATA, sag | srg1 | srg2 | srg3 );
		outportb( port->PORT_DATA, srg1 | srg2 | srg3 );
		
		}
		
	outportb( port->PORT_DATA, All_Low );
	
	}


//*************************************************************
//
// FUNCTION : image_area_clear
//
//*************************************************************

void
image_area_clear( ParallelPortMap *	port )
	{
	
	short x;
	
	
	LDCOff( port );
	
	outportb( port->PORT_DATA, All_Low );
	
	for( x = 0; x < SensorRows; x++ )
		{
		
		outportb( port->PORT_DATA, trg );
		outportb( port->PORT_DATA, sag | trg );
		outportb( port->PORT_DATA, iag | sag | srg1 | srg2 | srg3 );
		outportb( port->PORT_DATA, iag | srg1 | srg2 | srg3 );
		
		}
		
	outportb( port->PORT_DATA, All_Low );
	
	}


//*************************************************************
//
// FUNCTION : serial_register_clear
//
//*************************************************************

void
serial_register_clear( ParallelPortMap *	port )
	{
	
	short x;

	
	LDCOff( port );
	
	outportb( port->PORT_DATA, All_Low );
	
	for ( x = 0; x < SerialRegisterColumns + 1; x++ )
		{
		
		outportb( port->PORT_DATA, srg1 );
		outportb( port->PORT_DATA, srg2 );
		outportb( port->PORT_DATA, srg3 );
		
		}
		
	outportb( port->PORT_DATA, All_Low );
	
	}


//*************************************************************
//
// FUNCTION : shift_image_to_storage
//
//*************************************************************

void
shift_image_to_storage( ParallelPortMap *	port )
	{
	
	short x;

	
	LDCOff( port );
	
	for( x = 0; x < StorageRows + 1; x++ )
		{
		
		outportb( port->PORT_DATA, trg);
		outportb( port->PORT_DATA, sag | trg);
		outportb( port->PORT_DATA, iag | sag | srg1 | srg2 | srg3);
		outportb( port->PORT_DATA, iag | srg1 | srg2 | srg3);
		
		}
		
	}


//*************************************************************
//
// FUNCTION : OpenCloseShutter
//
//*************************************************************

void
OpenCloseShutter( ParallelPortMap *	port,
				  short				ms,
				  BOOL				openShutter )
	{
	
	ShutterState = openShutter ? ShutterOpen : ShutterClosed;

	outportb( port->PORT_CONTROL, LDCLow | ShutterState );
	
	delay( ms );
	
	}


//*************************************************************
//
// FUNCTION : LDCOn
//
//*************************************************************

void
LDCOn( ParallelPortMap *	port )
	{
	
	outportb( port->PORT_CONTROL, LDCHigh | ShutterState );
		
	}


//*************************************************************
//
// FUNCTION : LDCOff
//
//*************************************************************

void
LDCOff( ParallelPortMap *	port )
	{
	
	outportb( port->PORT_CONTROL, LDCLow | ShutterState );
		
	}
