#include "types.h"
#include "printString.h"
#include "keyboard.h"
#include "descriptor.h"

// Main function
void main(void)
{
	unsigned char vcTemp[2] = { 0, };
	BYTE bFlags;
	BYTE bTemp;
	int i = 0;
	
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
	if ( kActivateKeyboard() == TRUE )
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
	
	// Simple Shell
	while ( 1 )
	{
		// If output buffer is full, read scan code
		if ( kIsOutputBufferFull() == TRUE )
		{
			// Read data from output buffer
			bTemp = kGetKeyboardScanCode();

			// Transform scan code to ASCII code
			if ( kConvertScanCodeToASCIICode( bTemp, &( vcTemp[0] ), &bFlags ) == TRUE )
			{
				// Printf the key 
				if ( bFlags & KEY_FLAGS_DOWN )
				{
					kPrintString( i++, 13, vcTemp );
					
					// Sample test code
					if ( vcTemp[0] == '0' )
					{
						bTemp = bTemp / 0;
					}
				}
			}
		}
	}
}