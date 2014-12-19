#include "page.h"

// Generate page table for IA-32e
void kInitializePageTables( void )
{
	PML4TENTRY* pstPML4TEntry;
	PDPTENTRY* pstPDPTEntry;
	PDENTRY* pstPDEntry;
	DWORD dwMappingAddress;
	int i;

	// Generate PML4 Table
	// Except for first, set 0x00
	pstPML4TEntry = ( PML4TENTRY* ) 0x100000;
	kSetPageEntryData( &( pstPML4TEntry[0] ), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0 );
	for ( i = 1 ; i < PAGE_MAXENTRYCOUNT ; i++ )
	{
		// Don't use this section
		// Initiate 0
		kSetPageEntryData( &( pstPML4TEntry[i] ), 0, 0 ,0 ,0 );
	}

	// Generate Page Directory Pointer Table 
	// One PDPT can handle fully 512GB
	// We set 64 entries for 64GB
	pstPDPTEntry = ( PDPTENTRY* ) 0x101000;
	for ( i = 0 ; i < 64 ; i++ )
	{
		kSetPageEntryData( &( pstPDPTEntry[i]), 
			0, 0x102000 + ( i * PAGE_TABLESIZE ), PAGE_FLAGS_DEFAULT, 0 );
	}
	for ( i = 64 ; i < PAGE_MAXENTRYCOUNT ; i++ )
	{
		// Don't use this section
		// Initiate 0
		kSetPageEntryData( &( pstPDPTEntry[i] ), 0, 0, 0, 0);
	}
	
	// Generate Page Directory
	// One page directory can handle 1GB
	// We set 64 entries for 64GB
	pstPDEntry = ( PDENTRY* ) 0x102000;
	dwMappingAddress = 0;
	for ( i = 0 ; i < PAGE_MAXENTRYCOUNT * 64 ; i++ )
	{	
		// Here is Proteced mode(32 bit)
		// To access 64 bit address, 
		// access unit is MB ( >> 20 )
		// For under 4GB, dwUpperBaseAddress is 0
		kSetPageEntryData( &( pstPDEntry[i] ), 
			( i * ( PAGE_DEFAULTSIZE >> 20 ) ) >> 12, dwMappingAddress, 
			PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0 );
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}
}

// Set flag and address
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
		DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags )
{
	pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB = ( dwUpperBaseAddress & 0xFF ) | dwUpperFlags;
}

