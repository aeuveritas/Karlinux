#include "types.h"
#include "page.h"
#include "modeSwitch.h"

void kPrintString( int ix, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );
void kCopyKernel64ImageTo2Mbyte( void );

// Main function
void main(void)
{
	DWORD i;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char vcVendorString[13] = { 0, };
	
	kPrintString( 0, 0, "                                                          " );
	kPrintString( 0, 0, "Protected Mode C Language Kernel Start......[Pass]" );

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
	
	// Generate page table for IA-32e kernel mode 
	kPrintString( 0, 3, "IA-32e Page Tables Initialize...............[    ]" );
	kInitializePageTables();
	kPrintString( 45, 3, "Pass" );
	
	// Read information of processor's vendor
	kReadCPUID( 0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX );
	*( DWORD * ) vcVendorString = dwEBX;
	*( ( DWORD * ) vcVendorString + 1 ) = dwEDX;
	*( ( DWORD * ) vcVendorString + 2 ) = dwECX;
	kPrintString( 0, 4, "Processor Vendor Recognize..................[            ]" );
	kPrintString( 45, 4, vcVendorString);
	
	// Check 64 bit available
	kReadCPUID( 0x80000001, &dwEAX, & dwEBX, &dwECX, &dwEDX );
	kPrintString( 0, 5, "64bit Mode Support Check....................[    ]" );
	
	if ( dwEDX & ( 1 << 29 ) )
	{
		kPrintString( 45, 5, "Pass");
	}
	else
	{
		kPrintString( 45, 5, "Fail");
		kPrintString( 0, 6, "This processor doest not support 64bit mode!!" );
		while ( 1 );
	}
	
	// Move IA-32e mode kernel to 0x2000000 (2MB)
	kPrintString( 0, 6, "Copy UA-32e Kernel To 2M Address............[    ]" );
	kCopyKernel64ImageTo2Mbyte();
	kPrintString( 45, 6, "Pass" );
	
	// Change to IA-32e mode
	kPrintString( 0, 7, "Switch To IA-32e Mode.......................[    ]" );
	kSwitchAndExecute64bitKernel();
	
	while ( 1 );
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

// Move IA-32e mode kernel to 0x2000000 (2MB)
void kCopyKernel64ImageTo2Mbyte( void )
{
	WORD wKernel32SectorCount;
	WORD wTotalKernelSectorCount;
	DWORD * pdwSourceAddress;
	DWORD * pdwDestinationAddress;
	int i;
	
	// The number of total sector: 0x7C05
	// The number of proteced mode sector: 0x7C07
	wTotalKernelSectorCount = *( ( WORD * ) 0x7C05 );
	wKernel32SectorCount = *( ( WORD * ) 0x7C07 );
	
	pdwSourceAddress = ( DWORD * ) ( 0x10000 + ( wKernel32SectorCount * 512 ) );
	pdwDestinationAddress = ( DWORD * ) 0x200000;
	
	// Copy IA-32e mode kernel
	for ( i = 0 ; i < 512 * ( wTotalKernelSectorCount - wKernel32SectorCount ) / 4; i++ )
	{
		*pdwDestinationAddress = *pdwSourceAddress;
		pdwDestinationAddress++;
		pdwSourceAddress++;
	}
}
