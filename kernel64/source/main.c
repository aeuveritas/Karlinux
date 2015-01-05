#include "types.h"
#include "printString.h"
#include "keyboard.h"
#include "descriptor.h"
#include "PIC.h"

// Main function
void main(void)
{
	unsigned char vcTemp[2] = { 0, };
	BYTE bFlags;
	BYTE bTemp;
	int i = 0;
	KEYDATA stData;
	
	kPrintString( 45, 7, "Pass" );
	kPrintString( 0, 8, "IA-32e C Language Kernel Start..............[Pass]" );

	kPrintString( 0, 9, "GDT Initialize And Switch For IA-32e Mode...[    ]" );
	kInitializeGDTTableAndTSS();
	kLoadGDTR( GDTR_STARTADDRESS );
	kPrintString( 45, 9, "Pass" );
	
	kPrintString( 0, 10, "TSS Segment Load............................[    ]" );
	kLoadTR( GDT_TSSSEGMENT );
	kPrintString( 45, 10, "Pass" );
	
	kPrintString( 0, 11, "IDT Initialize..............................[    ]" );
	kInitializeIDTTables();
	kLoadIDTR( IDTR_STARTADDRESS );
	kPrintString( 45, 11, "Pass" );
	
	kPrintString( 0, 12, "Keyboard Activate...........................[    ]" );
	// Enable keyboard
	if ( kInitializeKeyboard() == TRUE )
	{
		kPrintString( 45, 12, "Pass" );
		kChangeKeyboardLED( FALSE, FALSE, FALSE );
	}
	else
	{
		kPrintString( 45, 12, "Fail" );
		while ( 1 )
		{
			;
		}
	}
	
	kPrintString( 0, 13, "PIC Controller And Interrupt Initialize.....[    ]" );
	// Initiate PIC Controller and Enable All interrupt
	kInitializePIC();
	kMaskPICInterrupt( 0 );
	kEnableInterrupt();
	kPrintString(45, 13, "Pass" );
	
	// Simple Shell
	while ( 1 )
	{
		// If data is in key queue, process
		if ( kGetKeyFromKeyQueue( &stData ) == TRUE )
		{
			// If key is down, print ASCII code of the key
			if ( stData.bFlags & KEY_FLAGS_DOWN )
			{
				// Store ASCII code of key 
				vcTemp[0] = stData.bASCIICode;
				kPrintString( i++, 14, vcTemp );
					
				// Sample test code
				if ( vcTemp[0] == '0' )
				{
					bTemp = bTemp / 0;
				}
			}
		}
	}
}