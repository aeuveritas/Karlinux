#include "types.h"

void kPrintString( int ix, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );

// Main function
void main(void)
{
	DWORD i;
	
	kPrintString( 0, 0, "                                                          " );
	kPrintString( 0, 0, "C Language Kernel Start.....................[Pass]" );

	// Check minimum memory size
	kPrintString( 0, 1, "Minimum Memory Size Check...................[    ]" );
	if (kIsMemoryEnough() == FALSE )
	{
		kPrintString( 45, 1, "Fail" );
		kPrintString( 0, 2, "Not Enough Memory!! Karlinux OS Requires Over "
				"64MB Memory!!" );
		while ( 1 );
	}
	else
	{
		kPrintString( 45, 1, "Pass" );
	}

	// Initialize kernel area for IA-32e mode
	kPrintString( 0, 2, "IA-32e kernel Area Initialize...............[    ]" );
	if ( kInitializeKernel64Area() == FALSE )
	{
		kPrintString( 45, 2, "Fail" );
		kPrintString( 0, 3, "Kernel Area Initialization Fail!!" );

		while ( 1 );
	}
	kPrintString( 45, 2, "Pass" );

	while( 1 );
}

// Print string
void kPrintString( int iX, int iY, const char* pcString )
{
	CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
	int i;

	pstScreen += (iY * 80 ) + iX;
	
	for( i = 0 ; pcString[i] != 0 ; i++ )
	{
		pstScreen[i].bCharactor = pcString[i];
	}
}

// Initialize kernel area for IA-32e
BOOL kInitializeKernel64Area( void )
{
	DWORD* pdwCurrentAddress;

	// Initial start point is 0x10000 (1MB)
	pdwCurrentAddress = ( DWORD* ) 0x100000;

	// Initial end point is 0x60000 (6MB)
	while ( ( DWORD ) pdwCurrentAddress < 0x600000 )
	{
		*pdwCurrentAddress = 0x00;

		// After write 0x00 at pdwCurrentAddress,
		// If the value is not 0, there is a problem
		if (*pdwCurrentAddress != 0 )
		{
			return FALSE;
		}
		
		// Go to next address
		pdwCurrentAddress++;
	}

	return TRUE;
}

// Check miminum memory size
BOOL kIsMemoryEnough( void )
{
	DWORD* pdwCurrentAddress;

	// Start form 0x100000 (1MB)
	pdwCurrentAddress = ( DWORD* ) 0x100000;

	// Til 0x4000000 (64MB)
	while ( ( DWORD ) pdwCurrentAddress < 0x4000000 )
	{
		*pdwCurrentAddress = 0x12345678;

		// After write 0x12345678 at pdwCurrentAddress,
		// If the value is not 0x12345678, there is a problem
		if ( *pdwCurrentAddress != 0x12345678 )
		{
			return FALSE;
		}

		// Go to next 1MB
		pdwCurrentAddress += ( 0x100000 / 4 ); // 4 byte = 32 bit
	}

	return TRUE;
}
