#include "descriptor.h"
#include "utility.h"

//=============================================================================
// GDT & TSS
//=============================================================================

// Initialize GDT Table
void kInitializeGDTTableAndTSS( void )
{
	GDTR * pstGDTR;
	GDTENTRY8 * pstEntry;
	TSSSEGMENT * pstTSS;
	int i;
	
	// GDTR setting
	pstGDTR = ( GDTR * ) GDTR_STARTADDRESS;
	pstEntry = ( GDTENTRY8 * ) ( GDTR_STARTADDRESS + sizeof( GDTR ) );
	pstGDTR->wLimit = GDT_TABLESIZE - 1;
	pstGDTR->qwBaseAddress = ( QWORD ) pstEntry;
	// TSS setting
	pstTSS = ( TSSSEGMENT * ) ( ( QWORD ) pstEntry + GDT_TABLESIZE );
	
	// Create 4 segment for NULL, 64 bit Code/Data, TSS
	kSetGDTEntry8( &( pstEntry[0] ), 0, 0, 0, 0, 0 );
	kSetGDTEntry8( &( pstEntry[1] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_CODE ,
		       GDT_FLAGS_LOWER_KERNELCODE, GDT_TYPE_CODE );
	kSetGDTEntry8( &( pstEntry[2] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_DATA,
		       GDT_FLAGS_LOWER_KERNELDATA, GDT_TYPE_DATA );
	kSetGDTEntry16( ( GDTENTRY16 * ) &( pstEntry[3] ), ( QWORD ) pstTSS,
			sizeof( TSSSEGMENT ) - 1, GDT_FLAGS_UPPER_TSS,
			GDT_FLAGS_LOWER_TSS , GDT_TYPE_TSS );
	
	// Initialize TSS below GDT
	kInitializeTSSSegment( pstTSS );
}

// 8 byte GDT Entry set
// To be use for setting code & data segment descriptor
void kSetGDTEntry8( GDTENTRY8 * pstEntry, DWORD dwBaseAddress, DWORD dwLimit,
		    BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType )
{
	pstEntry->wLowerLimit = dwLimit & 0xFFFF;
	pstEntry->wLowerBaseAddress = dwBaseAddress & 0xFFFF;
	pstEntry->bUpperBaseAddress1 = ( dwBaseAddress >> 16 ) & 0xFF;
	pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
	pstEntry->bUpperLimitAndUpperFlag = ( ( dwLimit >> 16 ) & 0xFF ) |
					bUpperFlags;
	pstEntry->bUpperBaseAddress2 = ( dwBaseAddress >> 24 ) & 0xFF;
}

// 16 byte GDT Entry set
// To be used for setting for TSS segment descriptor
void kSetGDTEntry16( GDTENTRY16 * pstEntry, QWORD qwBaseAddress, DWORD dwLimit,
		     BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType )
{
	pstEntry->wLowerLimit = dwLimit & 0xFFFF;
	pstEntry->wLowerBaseAddress = qwBaseAddress & 0xFFFF;
	pstEntry->bMiddleBaseAddress1 = ( qwBaseAddress >> 16 ) & 0xFF;
	pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
	pstEntry->bUpperLimitAndUpperFlag = ( ( dwLimit >> 16) & 0xFF ) |
					bUpperFlags;
	pstEntry->bMiddleBaseAddress2 = ( qwBaseAddress >> 24 ) & 0xFF;
	pstEntry->dwUpperBaseAddress = qwBaseAddress >> 32;
	pstEntry->dwReserved = 0;
}

// Initialize TSS Segment information
void kInitializeTSSSegment( TSSSEGMENT * pstTSS )
{
	kMemSet( pstTSS, 0, sizeof( TSSSEGMENT ) );
	pstTSS->qwIST[0] = IST_STARTADDRESS + IST_SIZE;
	// Set larger IO than TSS limit not to user IO Map
	pstTSS->wIOMapBaseAddress = 0xFFFF;
}

//=============================================================================
// IDT
//=============================================================================
// Initialize IDT table
void kInitializeIDTTables( void )
{
	IDTR * pstIDTR;
	IDTENTRY * pstEntry;
	int i;
	
	// Start address of IDTR
	pstIDTR = ( IDTR * ) IDTR_STARTADDRESS;
	// Create IDT table information
	pstEntry = ( IDTENTRY * ) ( IDTR_STARTADDRESS + sizeof( IDTR ) );
	pstIDTR->qwBaseAddress = ( QWORD ) pstEntry;
	pstIDTR->wLimit = IDT_TABLESIZE - 1;

	// Connect DummyHandler to 0 ~ 99 vector
	for ( i = 0 ; i < IDT_MAXENTRYCOUNT ; i++ )
	{
		kSetIDTEntry( &( pstEntry[i] ), kDummyHandler, 0x08, IDT_FLAGS_IST1,
			      IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
	}
}

// Set IDT Gate Descriptor
void kSetIDTEntry( IDTENTRY * pstEntry, void * pvHandler, WORD wSelector,
		   BYTE bIST, BYTE bFlags, BYTE bType )
{
	pstEntry->wLowerBaseAddress = ( QWORD )  pvHandler & 0xFFFF;
	pstEntry->wSegmentSelector = wSelector;
	pstEntry->bIST = bIST & 0x03;
	pstEntry->bTypeAndFlags = bType | bFlags;
	pstEntry->wMiddleBaseAddress = ( ( QWORD ) pvHandler >> 16 ) & 0xFFFF;
	pstEntry->dwUpperBaseAddress = ( QWORD ) pvHandler >> 32;
	pstEntry->dwReserved = 0;
}

// Interrupt or Exception Handler 
void kDummyHandler( void )
{
	kPrintString( 0, 0, "======================================================================" );
	kPrintString( 0, 1, "                Dummy Interrupt Handler Execute~!!!                   " );
	kPrintString( 0, 2, "                  Interrupt or Exception Occur~!!!                    " );
	kPrintString( 0, 3, "======================================================================" );

	while( 1 )
	{

	}
}
