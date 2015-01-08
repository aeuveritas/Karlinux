#include "PIT.h"

// Initialize PIT
void kInitializePIT( WORD wCount, BOOL bPeriodic )
{
	// Initiate PIT controller register(port 0x43) 
	// and stop it, then set binary counter as mode 0
	kOutPortByte( PIT_PORT_CONTROL, PIT_COUNTER0_ONCE );
	
	// If it is periodic, set mode 2
	if ( bPeriodic == TRUE )
	{
		// Set binary counter as mode 2 in PIT controller register(port 0x43)
		kOutPortByte( PIT_PORT_CONTROL, PIT_COUNTER0_PERIODIC );
	}
	
	// Set initial value from LSB to MSB in counter0(port 0x40)
	kOutPortByte( PIT_PORT_COUNTER0, wCount );
	kOutPortByte( PIT_PORT_COUNTER0, wCount >> 8 );
}

// Return value of counter0
WORD kReadCounter0( void )
{
	BYTE bHighByte, bLowByte;
	WORD wTemp = 0;
	
	// Send latch command to PIT control register(port 0x43) to read value of counter0
	kOutPortByte( PIT_PORT_CONTROL, PIT_COUNTER0_LATCH);
	
	// Read from LSB to MSB in counter0
	bLowByte = kInPortByte( PIT_PORT_COUNTER0);
	bHighByte = kInPortByte( PIT_PORT_COUNTER0 );
	
	// Return 16 bit
	wTemp = bHighByte;
	wTemp = ( wTemp << 8 ) | bLowByte;
	return wTemp;
}

// Wait certain time by setting counter0
//	After function call, PIT control setting is changed.
//	So set PIT control again.
//	Before measuring, disable interrupt is necessary
//	Acurate: about 50ms
void kWaitUsingDirectPIT( WORD wCount )
{
	WORD wLastCounter0;
	WORD wCurrentCounter0;
		
	// From 0 to 0xFFFF, count again and again
	kInitializePIT( 0, TRUE );
	
	// Wait count
	wLastCounter0 = kReadCounter0();
	while ( 1 )
	{
		// return current value
		wCurrentCounter0 = kReadCounter0();
		if ( ( ( wLastCounter0 - wCurrentCounter0 ) & 0xFFFF ) <= wCount )
		{
			break;
		}
	}
}