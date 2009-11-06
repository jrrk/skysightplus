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

#ifndef _PARALLEL_
#define _PARALLEL_

//*************************************************************
//
// Intelligently figure out what platform we're compiling for
// by examining the predefined symbols in the header files of
// the most commonly used platforms. Once a platform has been
// figured, #define identifiers that can be used throughout
// subsequent code. Also, adjust for any cross platform issues.
//
// PARALLEL_MAC   = Macintosh and Metrowerks compiler.
//
// PARALLEL_WIN16 = PC with 16 bit Windows and Borland or
//					Microsoft compiler.
//
// PARALLEL_WIN32 = PC with 32 bit Windows/Win95/Win98 and
//					Borland, Microsoft or Metrowerks compiler.
//
//*************************************************************

#if defined( __MWERKS__ ) && macintosh
	#define PARALLEL_MAC
#elif defined( _Windows ) || defined( _WINDOWS ) || defined( _WINDOWS_ )
	#if defined( _WIN32 ) || defined( __WIN32__ )
		#define PARALLEL_WIN32
	#else
		#define PARALLEL_WIN16
	#endif
#endif

#if defined( PARALLEL_WIN32 )
	#include <windows.h>
	#if !defined( DLLEXPORT )
		#if defined( __BORLANDC__ )
			#define DLLEXPORT _export
		#else
			#define DLLEXPORT __declspec(dllexport)
		#endif
	#endif
#endif

#if defined( PARALLEL_WIN16 )
	#include <windows.h>
	#include <bios.h>
	#include <dos.h>
	#define DLLEXPORT _export
#endif

#if defined( _MSC_VER )
	#define asm	__asm
#endif

#if defined( PARALLEL_MAC )
	#include <Types.h>
	typedef unsigned short WORD, *LPWORD;
	typedef unsigned long DWORD, *LPDWORD;
	#define DLLEXPORT
	#define FAR
	#define BOOL Boolean
#endif


//*************************************************************
//
// Define all versions of true and false in a cross platform,
// and compiler neutral fashion.
//
//*************************************************************

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


//*************************************************************
//
// Parallel register bit definitions.
//
// The classic PC parallel port bit definitions has historically
// been defined as a confusing set of negative and positive logic
// signals names. To add to the confusion, many signals are
// physically inverted from their electrical state on the wire.
// To further compound the confusion, subsequent protocols gave
// the same bits new names and correspondingly new negative or
// positive logic senses.
//
// To eliminate this confusion, use the definitions here with the
// "XXXRegPortRead" and "XXXRegPortWrite" macros. This presents
// all bit I/O operations to the programmer in positive logic
// terms, while the signal wire will always be set to its
// correspondingly electrical correct state. Read the second
// section below for mare detail.
//
// The macros have the following effective function prototypes
// and parameter definitions.
//
// port        = The register I/O address. Pass the appropriate
//				 address for the desired operation.
//
// controlBits = Pointer to a shadow copy of the current state
//				 of the bits in the Control register, since on
//				 some parallel ports this register is write
//				 only. Define this variable as a global for
//				 the entire module that will use these macros.
//				 Make sure to initialize this variable, and the
//				 Control register, at the beginning of your
//				 module. Do this by setting the variable to zero
//				 and calling this macro with the default Control
//				 register settings.
//
// setBits     = Pass in the or'd set of bit enums for the signals
//				 that you want to be set or "asserted". If no
//				 signals are to be "asserted", pass in a zero.
//
// resetBits   = Pass in the or'd set of bit enums for the signals
//				 that you want to be reset or "deasserted". If no
//				 signals are to be "deasserted", pass in a zero.
//
// unsigned char
// XXXRegPortRead( PortAddress port );
//
// void
// XXXRegPortWrite( PortAddress port,
//					unsigned char * controlBits,
//					unsigned char setBits,
//					unsigned char resetBits );
//
//*************************************************************
//
// These bit definitions consist of a naming convention that
// gives the reader a better sense as to how a bit is used, and
// will appear on the wire. Each define starts with a three letter
// acronym that indicates which protocol the bit belong in. Next
// a four letter abbreviation indicates which register the bit is
// contained in. Lastly, a signal names is given. The name is
// proceeded by a "n" to indicate negative logic signals on the
// wire. Otherwise the signal is positive logic.
//
// All signal names conform to IEEE 1284 names with the exception
// of the SPP Busy signal. As classically used, this name attempts
// to hide the fact that this signal is really a negative logic
// signal, but is also physically inverted from its wire state;
// and hence that name presents the signal as being positive logic.
// Definitions in this file only defines a signals negative or
// positive sense on the wire and ignores whether a signal is
// physically inverted or not.
//
// To compensate for physically inverted bits, the bit masks
// "Stat_Phys_Inverted" and "Cntl_Phys_Inverted" are used by the
// the "XXXRegPortRead" and "XXXRegPortWrite" macros to physically
// invert bit I/O transparently. In this way, a programmer may
// ignore the physically inversion register bits.
//
//*************************************************************

//*************************************************************
//
// Compatibility unidirectional mode register bit definitions.
//
//*************************************************************

typedef enum {
	SPP_Stat_nBusy		 = 0x80,	// Status Reg, physically inverted, active low.
	SPP_Stat_nAck		 = 0x40,	// Status Reg, unchanged input, active low.
	SPP_Stat_PaperOut	 = 0x20,	// Status Reg, unchanged input, active high.
	SPP_Stat_Select		 = 0x10,	// Status Reg, unchanged input, active high.
	SPP_Stat_nError		 = 0x08,	// Status Reg, unchanged input, active low.
	SPP_Stat_Bit2Unused	 = 0x04,
	SPP_Stat_Bit1Unused	 = 0x20,
	SPP_Stat_Bit0Unused	 = 0x01,
	
	Stat_Phys_Inverted	= SPP_Stat_nBusy,
	
	SPP_Stat_Active_Low	= ( SPP_Stat_nError |
							SPP_Stat_nAck |
							SPP_Stat_nBusy ),
	
	SPP_Cntl_Bit7Unused	 = 0x80,
	SPP_Cntl_Bit6Unused	 = 0x40,
	SPP_Cntl_Bit5Unused	 = 0x20,
	SPP_Cntl_IntEnable	 = 0x10,	// Control Reg, set to pass IRQ from nAck, active high.
	SPP_Cntl_nSelectIn	 = 0x08,	// Control Reg, physically inverted, active low.
	SPP_Cntl_nInit		 = 0x04,	// Control Reg, unchanged output, active low.
	SPP_Cntl_nAutoLF	 = 0x02,	// Control Reg, physically inverted, active low.
	SPP_Cntl_nStrobe	 = 0x01,	// Control Reg, physically inverted, active low.
	
	Cntl_Phys_Inverted	= ( SPP_Cntl_nStrobe |
							SPP_Cntl_nAutoLF |
							SPP_Cntl_nSelectIn ),
	
	SPP_Cntl_Active_Low	= ( SPP_Cntl_nStrobe |
							SPP_Cntl_nAutoLF |
							SPP_Cntl_nInit |
							SPP_Cntl_nSelectIn ),
							
	SPP_Cntl_All_Clear =  ( SPP_Cntl_nStrobe |
							SPP_Cntl_nAutoLF |
							SPP_Cntl_nInit |
							SPP_Cntl_nSelectIn |
							SPP_Cntl_IntEnable )
	} SPP_REG_BITS;

#define SPPRegPortRead( port ) \
	( ( inportb( port ) ^ Stat_Phys_Inverted ) ^ SPP_Stat_Active_Low )

#define SPPRegPortWrite( port, controlBits, setBits, resetBits ) \
	outportb( port, ( Cntl_Phys_Inverted ^					/* Output the bits while adjusting the physically inverted bits. */ \
		( *( controlBits ) =								/* Save the new control bits. */ \
		  ( \
			( ( ( setBits ) & ~SPP_Cntl_Active_Low ) |		/* Get all of the bits to be physically set. */ \
			  ( ( resetBits ) & SPP_Cntl_Active_Low ) \
			) |												/* Combine all of the bits togather. */ \
			( *( controlBits ) &							/* Clear the reset bits from the current control bits. */ \
			  ~( ( ( resetBits ) & ~SPP_Cntl_Active_Low ) |	/* Get all of the bits to be physically reset. */ \
				 ( ( setBits ) & SPP_Cntl_Active_Low ) \
			  ) \
			) \
		  ) \
		) \
	  ) \
	)
	
//*************************************************************
//
// Nibble bidirectional mode register bit definitions.
//
//*************************************************************

#define LOW_NIBBLE_TBL	"\x00\x01\x02\x03\x04\x05\x06\x07\x00\x00\x00\x00\x00\x00\x00\x00\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
#define HIGH_NIBBLE_TBL	"\x00\x10\x20\x30\x40\x50\x60\x70\x00\x00\x00\x00\x00\x00\x00\x00\x80\x90\xA0\xB0\xC0\xD0\xE0\xF0"

typedef enum {
	NIB_Stat_PtrBusy	 = 0x80,	// Status Reg, physically inverted, active high.
	NIB_Stat_PtrClk		 = 0x40,	// Status Reg, unchanged input, active high.
	NIB_Stat_AckDataReg	 = 0x20,	// Status Reg, unchanged input, active high.
	NIB_Stat_XFlag		 = 0x10,	// Status Reg, unchanged input, active high.
	NIB_Stat_nDataAvail	 = 0x08,	// Status Reg, unchanged input, active low.
	NIB_Stat_Bit2Unused	 = 0x04,
	NIB_Stat_Bit1Unused	 = 0x20,
	NIB_Stat_Bit0Unused	 = 0x01,
	
	NIB_Stat_Active_Low	= NIB_Stat_nDataAvail,
	
	NIB_Cntl_Bit7Unused	 = 0x80,
	NIB_Cntl_Bit6Unused	 = 0x40,
	NIB_Cntl_Bit5Unused	 = 0x20,
	NIB_Cntl_IntEnable	 = 0x10,	// Control Reg, set to pass IRQ from nAck, active high.
	NIB_Cntl_1284Active	 = 0x08,	// Control Reg, physically inverted, active high.
	NIB_Cntl_nInit		 = 0x04,	// Control Reg, unchanged output, active low.
	NIB_Cntl_HostBusy	 = 0x02,	// Control Reg, physically inverted, active high.
	NIB_Cntl_HostClk	 = 0x01,	// Control Reg, physically inverted, active high.
	
	NIB_Cntl_Active_Low	= NIB_Cntl_nInit
	} NIB_REG_BITS;

#define NIBRegPortRead( port ) \
	( ( inportb( port ) ^ Stat_Phys_Inverted ) ^ NIB_Stat_Active_Low )

#define NIBRegPortWrite( port, controlBits, setBits, resetBits ) \
	outportb( port, ( Cntl_Phys_Inverted ^					/* Output the bits while adjusting the physically inverted bits. */ \
		( *( controlBits ) =								/* Save the new control bits. */ \
		  ( \
			( ( ( setBits ) & ~NIB_Cntl_Active_Low ) |		/* Get all of the bits to be physically set. */ \
			  ( ( resetBits ) & NIB_Cntl_Active_Low ) \
			) |												/* Combine all of the bits togather. */ \
			( *( controlBits ) &							/* Clear the reset bits from the current control bits. */ \
			  ~( ( ( resetBits ) & ~NIB_Cntl_Active_Low ) |	/* Get all of the bits to be physically reset. */ \
				 ( ( setBits ) & NIB_Cntl_Active_Low ) \
			  ) \
			) \
		  ) \
		) \
	  ) \
	)


//*************************************************************
//
// Byte bidirectional mode register bit definitions.
//
//*************************************************************

typedef enum {
	BYT_Stat_PtrBusy	 = 0x80,	// Status Reg, physically inverted, active high. Also data bits 3, 7.
	BYT_Stat_PtrClk		 = 0x40,	// Status Reg, unchanged input, active high.
	BYT_Stat_AckDataReg	 = 0x20,	// Status Reg, unchanged input, active high. Also data bits 2, 6.
	BYT_Stat_XFlag		 = 0x10,	// Status Reg, unchanged input, active high. Also data bits 1, 5.
	BYT_Stat_nDataAvail	 = 0x08,	// Status Reg, unchanged input, active low. Also data bits 0, 4.
	BYT_Stat_Bit2Unused	 = 0x04,
	BYT_Stat_Bit1Unused	 = 0x20,
	BYT_Stat_Bit0Unused	 = 0x01,
	
	BYT_Stat_Active_Low	 = BYT_Stat_nDataAvail,
	
	BYT_Cntl_Bit7Unused	 = 0x80,
	BYT_Cntl_Bit6Unused	 = 0x40,
	BYT_Cntl_ReadMode	 = 0x20,	// Control Reg, set to enable Data reg read mode, active high.
	BYT_Cntl_IntEnable	 = 0x10,	// Control Reg, high to pass IRQ from nAck, active high.
	BYT_Cntl_1284Active	 = 0x08,	// Control Reg, physically inverted, active high.
	BYT_Cntl_nInit		 = 0x04,	// Control Reg, unchanged output, active low.
	BYT_Cntl_HostBusy	 = 0x02,	// Control Reg, physically inverted, active high.
	BYT_Cntl_HostClk	 = 0x01,	// Control Reg, physically inverted, active high.
	
	BYT_Cntl_Active_Low	= BYT_Cntl_nInit
	} BYT_REG_BITS;

#define BYTRegPortRead( port ) \
	( ( inportb( port ) ^ Stat_Phys_Inverted ) ^ BYT_Stat_Active_Low )

#define BYTRegPortWrite( port, controlBits, setBits, resetBits ) \
	outportb( port, ( Cntl_Phys_Inverted ^					/* Output the bits while adjusting the physically inverted bits. */ \
		( *( controlBits ) =								/* Save the new control bits. */ \
		  ( \
			( ( ( setBits ) & ~BYT_Cntl_Active_Low ) |		/* Get all of the bits to be physically set. */ \
			  ( ( resetBits ) & BYT_Cntl_Active_Low ) \
			) |												/* Combine all of the bits togather. */ \
			( *( controlBits ) &							/* Clear the reset bits from the current control bits. */ \
			  ~( ( ( resetBits ) & ~BYT_Cntl_Active_Low ) |	/* Get all of the bits to be physically reset. */ \
				 ( ( setBits ) & BYT_Cntl_Active_Low ) \
			  ) \
			) \
		  ) \
		) \
	  ) \
	)


//*************************************************************
//
// Enhanced bidirectional mode register bit definitions.
//
//*************************************************************

typedef enum {
	EPP_Stat_nWait		  = 0x80,	// Status Reg, physically inverted, active low.
	EPP_Stat_Intr		  = 0x40,	// Status Reg, unchanged input, active high.
	EPP_Stat_UserDefined1 = 0x20,	// Status Reg, unchanged input, active high.
	EPP_Stat_XFlag		  = 0x10,	// Status Reg, unchanged input, active high. Also UserDefined3.
	EPP_Stat_UserDefined2 = 0x08,	// Status Reg, unchanged input, active low.
	EPP_Stat_Bit2Unused   = 0x04,
	EPP_Stat_Bit1Unused   = 0x20,
	EPP_Stat_Timeout      = 0x01,
	
	EPP_Stat_Active_Low   = EPP_Stat_nWait,
	
	EPP_Cntl_Bit7Unused   = 0x80,
	EPP_Cntl_Bit6Unused   = 0x40,
	EPP_Cntl_ReadMode	  = 0x20,	// Control Reg, set to enable Data reg read mode, active high.
	EPP_Cntl_IntEnable	  = 0x10,	// Control Reg, high to pass IRQ from nAck, active high.
	EPP_Cntl_nAStrb		  = 0x08,	// Control Reg, physically inverted, active low. Also 1284Active.
	EPP_Cntl_nInit		  = 0x04,	// Control Reg, unchanged output, active low.
	EPP_Cntl_nDStrb		  = 0x02,	// Control Reg, physically inverted, active low.
	EPP_Cntl_nWrite		  = 0x01,	// Control Reg, physically inverted, active low.
	
	EPP_Cntl_Active_Low	= ( EPP_Cntl_nWrite |
							EPP_Cntl_nDStrb |
							EPP_Cntl_nInit |
							EPP_Cntl_nAStrb )
	} EPP_REG_BITS;

#define EPPRegPortRead( port ) \
	( ( inportb( port ) ^ Stat_Phys_Inverted ) ^ EPP_Stat_Active_Low )

#define EPPRegPortWrite( port, controlBits, setBits, resetBits ) \
	outportb( port, ( Cntl_Phys_Inverted ^					/* Output the bits while adjusting the physically inverted bits. */ \
		( *( controlBits ) =								/* Save the new control bits. */ \
		  ( \
			( ( ( setBits ) & ~EPP_Cntl_Active_Low ) |		/* Get all of the bits to be physically set. */ \
			  ( ( resetBits ) & EPP_Cntl_Active_Low ) \
			) |												/* Combine all of the bits togather. */ \
			( *( controlBits ) &							/* Clear the reset bits from the current control bits. */ \
			  ~( ( ( resetBits ) & ~EPP_Cntl_Active_Low ) |	/* Get all of the bits to be physically reset. */ \
				 ( ( setBits ) & EPP_Cntl_Active_Low ) \
			  ) \
			) \
		  ) \
		) \
	  ) \
	)


//*************************************************************
//
// Extended bidirectional mode register bit definitions.
//
//*************************************************************

typedef enum {
	ECP_Stat_PeripAck	 = 0x80,	// Status Reg, physically inverted, active high.
	ECP_Stat_PeripClk	 = 0x40,	// Status Reg, unchanged input, active high.
	ECP_Stat_nAckReverse = 0x20,	// Status Reg, unchanged input, active low.
	ECP_Stat_XFlag		 = 0x10,	// Status Reg, unchanged input, active high.
	ECP_Stat_nPeriphReq	 = 0x08,	// Status Reg, unchanged input, active low.
	ECP_Stat_Bit2Unused	 = 0x04,
	ECP_Stat_Bit1Unused	 = 0x20,
	ECP_Stat_Bit0Unused	 = 0x01,
	
	ECP_Stat_Active_Low	 = (ECP_Stat_nPeriphReq | ECP_Stat_nAckReverse),
	
	ECP_Cntl_Bit7Unused	 = 0x80,
	ECP_Cntl_Bit6Unused	 = 0x40,
	ECP_Cntl_ReadMode	 = 0x20,	// Control Reg, set to enable Data reg read mode, active high.
	ECP_Cntl_IntEnable	 = 0x10,	// Control Reg, high to pass IRQ from nAck, active high.
	ECP_Cntl_1284Active	 = 0x08,	// Control Reg, physically inverted, active high.
	ECP_Cntl_nReverseReq = 0x04,	// Control Reg, unchanged output, active low.
	ECP_Cntl_HostBusy	 = 0x02,	// Control Reg, physically inverted, active high.
	ECP_Cntl_HostClk	 = 0x01,	// Control Reg, physically inverted, active high.
	
	ECP_Cntl_Active_Low	 = ECP_Cntl_nReverseReq
	} ECP_REG_BITS;

#define ECPRegPortRead( port ) \
	( ( inportb( port ) ^ Stat_Phys_Inverted ) ^ ECP_Stat_Active_Low )

#define ECPRegPortWrite( port, controlBits, setBits, resetBits ) \
	outportb( port, ( Cntl_Phys_Inverted ^					/* Output the bits while adjusting the physically inverted bits. */ \
		( *( controlBits ) =								/* Save the new control bits. */ \
		  ( \
			( ( ( setBits ) & ~ECP_Cntl_Active_Low ) |		/* Get all of the bits to be physically set. */ \
			  ( ( resetBits ) & ECP_Cntl_Active_Low ) \
			) |												/* Combine all of the bits togather. */ \
			( *( controlBits ) &							/* Clear the reset bits from the current control bits. */ \
			  ~( ( ( resetBits ) & ~ECP_Cntl_Active_Low ) |	/* Get all of the bits to be physically reset. */ \
				 ( ( setBits ) & ECP_Cntl_Active_Low ) \
			  ) \
			) \
		  ) \
		) \
	  ) \
	)


//*************************************************************
//
// Define the parallel port register map structure.
//
// This allows a cross platform representation of a parallel
// port's register I/O address.
//
// The enums are offsets for classical PC address offsets.
//
// The structure contains an I/O address for each register.
// For parallel ports that don't actually a register at a
// offset, the structure I/O address is NULL.
//
//*************************************************************
	
typedef enum {
	PORT_DATA			= 0,
	PORT_STATUS			= 1,
	PORT_CONTROL		= 2,
	PORT_EPP_ADDRESS	= 3,
	PORT_EPP_DATA		= 4,
	PORT_EPP_DATA1		= 5,
	PORT_EPP_DATA2		= 6,
	PORT_EPP_DATA3		= 7,
	PORT_PC_ECP_OFFSET	= 0x400,
	PORT_DFIFO			= 0,
	PORT_CFGB			= 1,
	PORT_ECR			= 2
	} ECP_ADDRESS_OFFSET;

#if defined( PARALLEL_MAC )
	typedef unsigned char * PortAddress;
#else
	typedef unsigned short PortAddress;
#endif

typedef struct {
	PortAddress	PORT_DATA;
	PortAddress	PORT_STATUS;
	PortAddress	PORT_CONTROL;
	PortAddress	PORT_EPP_ADDRESS;
	PortAddress	PORT_EPP_DATA;
	PortAddress	PORT_EPP_DATA1;
	PortAddress	PORT_EPP_DATA2;
	PortAddress	PORT_EPP_DATA3;
	PortAddress	PORT_DFIFO;
	PortAddress	PORT_CFGB;
	PortAddress	PORT_ECR;
	} ParallelPortMap;


//*************************************************************
//
// Define the parallel port protocol modes.
//
// Setting one of these modes in the MacParallel driver
// defines which parallel protocols the driver will use to
// communicate with a device.
//
//*************************************************************
//
// Setting kAuto_Protocol_Mode only causes the driver to auto
// negotiate with the device for the highest speed protocol
// that device and parallel port supports. The protocol mode
// that was negotiated is returned.
//
// Setting kRenegotiate_Protocol_Mode causes the driver to
// renegotiate with a device for the highest speed protocol
// on each I/O request. After each I/O the driver reverts
// back SPP mode. Not setting this mode causes the driver
// to keep the protocol mode the same between each I/O, and
// eliminates negotiation on all subsequent I/O requests.
// This condition persists until explicitly changed, or the
// driver is closed.
//
// Setting one of the other modes causes the driver to enter
// that protocol mode directly without negotiating with the
// device. If the parallel port, or the device, doesn't
// support the specified protocol mode, an error is returned
//
// Setting both kAuto_Protocol_Mode, and a set of the other
// modes, causes the driver to auto negotiate for the highest
// speed mode specified in the set. If the device refuses all
// protocol modes in the set, or the parallel port doesn't
// support one of the modes in the set, an error is returned.
//
// These constants are also used to get a bit mask of which
// protocol modes a device or parallel port supports.
//
//*************************************************************
	
typedef enum
	{
	kAuto_Protocol_Mode        = 0x80,
	kRenegotiate_Protocol_Mode = 0x40,
	kSPP_Protocol_Mode         = 0x01,
	kNIB_Protocol_Mode         = 0x02,
	kBYT_Protocol_Mode         = 0x04,
	kEPP_Protocol_Mode         = 0x08,
	kECP_Protocol_Mode         = 0x10,
	kECP_RLE_Protocol_Mode     = 0x20
	} Parallel_Protocol_Modes;


//*************************************************************
//
// MacParallel driver Status codes.
//
//*************************************************************

//*************************************************************
//
// Get the address map for parallel port registers.
//
// Use this Status call to fill in a ParallelPortMap struct
// with the register addresses of the specified parallel
// port. This is for software that needs direct control of
// the parallel port.
//
// Each register that the port doesn't support will have its
// map address returned as NULL. If the EPP register addresses
// are NULL, the port doesn't support EPP mode. If the DFIFO
// and CFGB are NULL, the port doesn't support ECP mode. The
// ECR register may have a map address for any mode. This is
// because this register is used for setting with protocol
// mode the port uses (see Parallel_Protocol_Modes defines
// above). All parallel ports will have (and support) the
// DATA, STATUS and CONTROL register maps filled in.
//
//*************************************************************
//
// Example:
//
//  {
//  OSStatus			errorCode;
//  CntrlParam			cntrlBlock;
//  ParallelPortMap		addressMap;
//  unsigned short		portRefnum;
//
//  cntrlBlock.ioCompletion = (IOCompletionUPP)nil;
//  cntrlBlock.csCode       = kGetPortAddressMap;
//  cntrlBlock.ioCRefNum    = portRefnum;
//
//  *(ParallelPortMap **)cntrlBlock.csParam = &addressMap;
//
//  errorCode = PBStatusSync( (ParmBlkPtr)&cntrlBlock );
//  }
//
//*************************************************************

#define kPARGetPortAddressMap	0x4241	// "BA"

//*************************************************************
//
// Get the supported protocol modes for a parallel port.
//
// Use this Status call to get a bit mask of the supported
// protocol modes for a parallel port.
//
// See Parallel_Protocol_Modes defines above.
//
//*************************************************************
//
// Example:
//
//  {
//  OSStatus			errorCode;
//  CntrlParam			cntrlBlock;
//  unsigned char		portProtcolSupport;
//  unsigned short		portRefnum;
//
//  cntrlBlock.ioCompletion = (IOCompletionUPP)nil;
//  cntrlBlock.csCode       = kPARGetPortProtcolSupport;
//  cntrlBlock.ioCRefNum    = portRefnum;
//
//  *(unsigned char **)cntrlBlock.csParam = &portProtcolSupport;
//
//  errorCode = PBStatusSync( (ParmBlkPtr)&cntrlBlock );
//  }
//
//*************************************************************

#define kPARGetPortProtcolSupport	0x4D53	// "MS"

//*************************************************************
//
// MacParallel driver Control codes.
//
//*************************************************************


//*************************************************************
//
// Function prototypes for ParLib. Adjust for cross platform
// issues.
//
//*************************************************************

#ifdef __cplusplus
extern "C" {
#endif

unsigned short FAR DLLEXPORT
CountParallelPorts( void );

unsigned short FAR DLLEXPORT
GetParallelPortAddress( short portNum );

short FAR DLLEXPORT
OpenParallelPort( unsigned short	baseAddress,
				  ParallelPortMap *	addressMap );

short FAR DLLEXPORT
CloseParallelPort( unsigned short	baseAddress );

#if defined( PARALLEL_WIN32 )

	#define delay( milliseconds ) Sleep( milliseconds )

#else
	
	void FAR DLLEXPORT
	delay( unsigned int milliseconds );

#endif

#if defined ( PARALLEL_MAC )

	void
	delay_nanoseconds( unsigned long	nanoseconds );

	#define delay_microseconds( microseconds ) ( delay_nanoseconds( microseconds * 1000 ) )

#else

	void
	delay_microseconds ( unsigned long microseconds );

#endif

#if ! defined( PARALLEL_WIN16 )

	unsigned long FAR DLLEXPORT
	GetBIOSTickCount( void );

	void FAR DLLEXPORT
	enable( void );

	void FAR DLLEXPORT
	disable( void );

	void FAR DLLEXPORT
	outportb( PortAddress		port,
			  unsigned char		data );

	void FAR DLLEXPORT
	outport( PortAddress	port,
             unsigned short	data );

	unsigned char FAR DLLEXPORT
	inportb( PortAddress	port );

	unsigned short FAR DLLEXPORT
	inport( PortAddress	port );
	#define inp(port)			inportb(port)
    #define inpw(port)			inport(port)

    #define outp(port,data)		outportb(port,data)
	#define outpw(port,data)	outport(port,data)

#endif

#ifdef __cplusplus
}
#endif

#endif	// _PARALLEL_
