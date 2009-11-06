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

/*

	ParLib.c

	These are the Windows versions of the low level functions for parallel-port
    I/O and timing.

*/

#include "ParLib.h"

/*** Static local functions **************************************************/

static unsigned short GetTimer2( void );
static void LocalDelay( void );

/*

	GetParallelPortAddress:

	Given a parallel port LPT number from 1 to 4, returns the base I/O address
    of the parallel port.  If the specified port does not exist, the function
    returns zero.  This function only exists in 16-bit Windows!

*/

#ifdef PARALLEL_WIN16

unsigned short GetParallelPortAddress ( short nPort )
{
	unsigned short	wPortAddress;

    if ( nPort == 1 )
    	wPortAddress = peek ( 0x0040, 0x0008 );
    else if ( nPort == 2 )
    	wPortAddress = peek ( 0x0040, 0x000A );
    else if ( nPort == 3 )
    	wPortAddress = peek ( 0x0040, 0x000C );
    else if ( nPort == 4 )
    	wPortAddress = peek ( 0x0040, 0x000E );
    else
    	wPortAddress = 0;
        
    return ( wPortAddress );
}

#else

unsigned short GetParallelPortAddress ( short nPort )
{
	if ( nPort == 1 )
		return ( 0x0378 );
	else if ( nPort == 2 )
		return ( 0x0278 );
	else if ( nPort == 3 )
		return ( 0x03BC );
	else
		return ( 0 );
}

#endif


/*

	OpenParallelPort:

	Given the parallel port base address, fills in a parallel port address map
    with the I/O addresses of the other parallel lines.  For compatibility with
    the Mac version.

*/

short
OpenParallelPort( unsigned short	baseAddress,
				  ParallelPortMap *	addressMap )
	{

	// Record the base address

	addressMap->PORT_DATA    = baseAddress;
	addressMap->PORT_STATUS  = addressMap->PORT_DATA + PORT_STATUS;
	addressMap->PORT_CONTROL = addressMap->PORT_DATA + PORT_CONTROL;
	addressMap->PORT_DFIFO   = addressMap->PORT_DATA + PORT_DFIFO + PORT_PC_ECP_OFFSET;
	addressMap->PORT_CFGB    = addressMap->PORT_DATA + PORT_CFGB + PORT_PC_ECP_OFFSET;
	addressMap->PORT_ECR     = addressMap->PORT_DATA + PORT_ECR + PORT_PC_ECP_OFFSET;

	return 0;

	}

/*

	CloseParallelPort:

	Does nothing at present.  For compatibility with the Mac version.

*/

short
CloseParallelPort( unsigned short	port )
	{

	return 0;

	}

#if defined ( PARALLEL_WIN32 )

/*

	inportb:

    Inputs an 8-bit byte from an I/O port.  This function is only compiled in
    32-bit Windows, and exists only for compatibility with the DOS function
    that is declared in <dos.h> for 16-bit Windows programs.

*/

unsigned char
inportb( unsigned short	port )
	{
	unsigned char data;

	asm {
		push edx
		mov dx, port
		in al, dx
		mov data, al
		pop edx
		}

	return data;
	}

/*

	inport:

    Inputs a 16-bit word from an I/O port.  This function is only compiled in
    32-bit Windows, and exists only for compatibility with the DOS function
    that is declared in <dos.h> for 16-bit Windows programs.

*/

unsigned short
inport(unsigned short port)
	{
	unsigned short data;

    asm {
		push edx
		mov dx, port
  		in ax, dx
		mov data, ax
  		pop edx
 		}

	return data;
	}
/*

	outportb:

    Output an 8-bit byte to an I/O port.  This function is only compiled in
    32-bit Windows, and exists only for compatibility with the DOS function
    that is declared in <dos.h> for 16-bit Windows programs.

*/

void
outportb( unsigned short	port,
		  unsigned char		val )
	{

	asm {
	    push edx
	    mov dx, port
	    mov al, val
	    out dx, al
	    pop edx
		}
	}

/*

	outport:

    Output a 16-bit word to an I/O port.  This function is only compiled in
    32-bit Windows, and exists only for compatibility with the DOS function
    that is declared in <dos.h> for 16-bit Windows programs.

*/

void
outport( unsigned short port,
         unsigned short data )
	{

	asm {
	    push edx
	    mov dx, port
		mov ax, data
		out dx, ax
	    pop edx
		}
	}
/*

	enable:

    Disable NMI interrupts.  This function is only compiled in 32-bit Windows,
    and exists for compatibility with the DOS function that is declared in
    <dos.h> for 16-bit Windows programs.

*/

void
enable( void )
	{

	asm {
	    sti
		}

	}

/*

	disable:

    Disable NMI interrupts.  This function is only compiled in 32-bit Windows,
    and exists for compatibility with the DOS function that is declared in
    <dos.h> for 16-bit Windows programs.

*/

void
disable( void )
	{

	asm {
	    cli
		}
	}

/*

	GetBIOSTickCount:

	Emulates the DOS BIOS timing function, which counts the number of ticks
    elapsed since midnight at the rate of 18.2 ticks per second.  Note that
    this function actually counts the number of ticks elapsed since Windows
    started (rather than from midnight), and will not actually roll over to
    zero at midnight.  Also, it can only get the time, not set it!

*/

unsigned long GetBIOSTickCount( void )
{
	return GetTickCount() / 55;
}

/*

	delay:

	Executes a delay for a specified number of milliseconds.

*/

#endif

#if defined ( PARALLEL_WIN16 )

void delay( unsigned int milliseconds )
{

    DWORD dwTickCount = GetTickCount();

    while ( GetTickCount() - dwTickCount < milliseconds )
    	;
}

#endif

/*

	delay_microseconds:

	Executes a delay for a specified number of microseconds.

*/

void delay_microseconds(unsigned long delay)
{
	unsigned th, tl, lastTl;
	unsigned long t, t0;

	delay +=  198 * (delay >> 10);
	delay *= 2;
	outportb(0x43, 0xB6);	// program timer 2 for mode 3
	outportb(0x42, 0xFF);
	outportb(0x42, 0xFF);
	outportb(0x61, (inportb(0x61) & ~3) | 1);	// raise gate, speaker off

	t0 = lastTl = GetTimer2();
	th = 0;
	do {
		tl = GetTimer2();
		if ( tl < lastTl )
			th++;
		lastTl = tl;
		t = ((unsigned long)th << 16) + tl;
        LocalDelay();
	} while ( t - t0 < delay );
	outportb(0x61, inportb(0x61) & ~3);         // lower gate
}

/*

	GetTimer2:

	Read the timer 2 channel for high accuracy
	timing.
	
	Got rid of the assem version of GetTimer2 since the 
	"NT" (because of the inportb function) will really
	for all version.

*/

static unsigned short GetTimer2(void)
{
	unsigned short res;
    unsigned char tl, th, tl2;

    disable();				/* disable ints while read timer */
    tl = inportb(0x42);		/* read timer once */
    th = inportb(0x42);
    tl2 = inportb(0x42);
    if ( tl2 > tl )			/* see if timer overflowed while reading */
    	th = inportb(0x42);		/* if so re-read high byte for use */
    else
    	tl2 = inportb(0x42);    /* if not must re-read and discard high byte */
    enable();
    res = tl + ((unsigned short)th << 8);
    return ~res;				/* timer counts down, we want to count up */
}

/*

	TimerDelay:

	Delay the passed period of time in microseconds.
	Each unadjusted count represents 0.83809 microseconds
	but this routine adjusts to microseconds by
	multiplying the delay by 1/0.83809

	Note that when running Timer 2 the
	the timer counts at 2 times the rate
	shown above

*/

static void LocalDelay(void)
{
    int i, j = 0;

    for ( i = 0; i < 100; i++ )
       	j++;
}


