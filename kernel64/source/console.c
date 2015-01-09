#include "console.h"

// Structure for console managin
CONSOLEMANAGER gs_stConsoleManager = { 0, };

// Initialize console
void kInitializeConsole( int iX, int iY )
{
	// Set 0
	kMemSet( &gs_stConsoleManager, 0, sizeof( gs_stConsoleManager ) );
	
	// Set cursor position
	kSetCursor( iX, iY );
}

// Set cursor position
void kSetCursor( int iX, int iY )
{
	int iLinearValue;
	
	// Calculate cursor position
	iLinearValue = iY * CONSOLE_WIDTH + iX;
	
	// Send CRTC control address register(post 0x3D4) to 0x0E
	// To select upper cursor position register
	kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR );
	// Out upper byte of cursor to CRTC control data register(port 0x3D5)
	kOutPortByte( VGA_PORT_DATA, iLinearValue >> 8 );
	
	// Send CRTC control address register(post 0x3D4) to 0x0F
	// To select lower cursor position register
	kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR );
	// Out upper byte of cursor to CRTC control data register(port 0x3D5)
	kOutPortByte( VGA_PORT_DATA, iLinearValue & 0xFF );
	
	// Update position
	gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}

// Return current cursor position
void kGetCursor( int * piX, int * piY )
{
	*piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
	*piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}

// Internal implementation of printf
void kPrintf( const char * pcFormatString, ... )
{
	va_list ap;
	char vcBuffer[1024];
	int iNextPrintOffset;
	
	// Process vsprintf() with variable list 
	va_start( ap, pcFormatString );
	kVSPrintf( vcBuffer, pcFormatString, ap );
	va_end( ap );
	
	// Printf format string 
	iNextPrintOffset = kConsolePrintString( vcBuffer );
	
	// Update cursor position
	kSetCursor( iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH );
}

// Return new cursor position after \n or \t
int kConsolePrintString( const char * pcBuffer )
{
	CHARACTER * pstScreen = ( CHARACTER * ) CONSOLE_VIDEOMEMORYADDRESS;
	int i, j;
	int iLength;
	int iPrintOffset;

	// Save print position
	iPrintOffset = gs_stConsoleManager.iCurrentPrintOffset;
	
	// Print string as it's length 
	iLength = kStrLen( pcBuffer );
	for ( i = 0 ; i < iLength ; i++ )
	{
		// Next line 
		if ( pcBuffer[i] == '\n' )
		{
			iPrintOffset += ( CONSOLE_WIDTH - ( iPrintOffset % CONSOLE_WIDTH ) );
		}
		// Tab
		else if ( pcBuffer[i] == '\t' )
		{
			iPrintOffset += ( 8 - ( iPrintOffset % 8 ) );
		}
		// Normal character
		else
		{
			pstScreen[iPrintOffset].bCharactor = pcBuffer[i];
			pstScreen[iPrintOffset].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
			iPrintOffset++;
		}
		
		// If cursor is out of range
		if ( iPrintOffset >= ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) )
		{
			// Except first line 
			kMemCpy( CONSOLE_VIDEOMEMORYADDRESS,
				 CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof( CHARACTER ),
				 ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH * sizeof( CHARACTER ) );
			
			// Except last line
			for ( j = ( CONSOLE_HEIGHT - 1 ) * ( CONSOLE_WIDTH ) ; j < ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) ; j++ )
			{
				// Print white space
				pstScreen[j].bCharactor = ' ';
				pstScreen[j].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
			}
			
			// Set cursor to last line
			iPrintOffset = ( CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;
		}
	}
	
	return iPrintOffset;
}

// Delete all screen 
void kClearScreen( void )
{
	CHARACTER * pstScreen = ( CHARACTER * ) CONSOLE_VIDEOMEMORYADDRESS;
	int i;
	
	// Set all 0
	for ( i = 0 ; i < CONSOLE_WIDTH * CONSOLE_HEIGHT ; i++ )
	{
		pstScreen[i].bCharactor = ' ';
		pstScreen[i].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
	}
	
	// Ser cursor to the first line
	kSetCursor( 0, 0 );
}

// Implementation of getch()
BYTE kGetCh( void )
{
	KEYDATA stData;

	// Wait key 
	while ( 1 )
	{
		// Wait key queue
		while ( kGetKeyFromKeyQueue( &stData ) == FALSE )
		{
			;
		}
		
		if ( stData.bFlags & KEY_FLAGS_DOWN )
		{
			return stData.bASCIICode;
		}
	}
}

// Print character at X, Y
void kPrintStringXY( int iX, int iY, const char* pcString )
{
	CHARACTER * pstScreen = ( CHARACTER * ) CONSOLE_VIDEOMEMORYADDRESS;
	int i;
	
	// Calculate the position
	pstScreen += ( iY * 80 ) + iX;
	// Set character properties
	for ( i = 0 ; pcString[i] != 0 ; i++ )
	{
		pstScreen[i].bCharactor = pcString[i];
		pstScreen[i].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
	}
}
