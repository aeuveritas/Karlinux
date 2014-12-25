#include "types.h"
#include "keyboard.h"

void kPrintString( int ix, int iY, const char* pcString );

// Main function
void main(void)
{
	char vcTemp[2] = { 0, };
	BYTE bFlags;
	BYTE bTemp;
	int i = 0;
	
	kPrintString( 45, 7, "Pass" );
	kPrintString( 0, 8, "IA-32e C Language Kernel Start..............[Pass]" );
	
	kPrintString( 0, 9, "Keyboard Activate...........................[    ]" );
	
	// Enable keyboard
	if ( kActivateKeyboard() == TRUE )
	{
		kPrintString( 45, 9, "Pass" );
		kChangeKeyboardLED( FALSE, FALSE, FALSE );
	}
	else
	{
		kPrintString( 45, 9, "Fail" );
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
					kPrintString( i++, 10, vcTemp );
				}
			}
		}
	}
}

// Print string
void kPrintString( int iX, int iY, const char* pcString )
{
	CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
	int i;

	pstScreen += (iY * 80 ) + iX;
	
	for ( i = 0 ; pcString[i] != 0 ; i++ )
	{
		pstScreen[i].bCharactor = pcString[i];
	}
}
