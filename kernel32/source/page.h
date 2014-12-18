#ifndef __PAGE_H__
#define __PAGE_H__

#include "types.h"

// MACRO
#define PAGE_FLAGS_P		0x00000001	// Present
#define PAGE_FLAGS_RW		0x00000002	// Read/Write
#define PAGE_FLAGS_US		0x00000004	// User/Supervisor - If set, User level
#define PAGE_FLAGS_PWT		0x00000008	// Page Level Write-through
#define PAGE_FLAGS_PCD		0x00000010	// Page Level Cache Disable
#define PAGE_FLAGS_A		0x00000020	// Accessed
#define PAGE_FLAGS_D		0x00000040	// Dirty
#define PAGE_FLAGS_PS		0x00000080	// Page Size
#define PAGE_FLAGS_G		0x00000100	// Global
#define PAGE_FLAGS_PAT		0x00001000	// Page Attribute Table Index
#define PAGE_FALGS_EXB		0x80000000	// Execute Disable Bit
#define PAGE_FLAGS_DEFAULT	( PAGE_FLAGS_P | PAGE_FLAGS_RW )
#define PAGE_TABLESIZE		0x1000
#define PAGE_MAXENTRYCOUNT	512
#define PAGE_DEFAULTSIZE	0x200000

// Structures
#pragma pack ( push, 1 )

// Structure for page entry
typedef struct kPageTableStruct
{
	// For PML4T & PDPTE
	// P, RW, US, PWT, PCD, A, Reserved(3), Avail(3),
	// Base Address(20)
	// For PDE
	// P, RW, US, PWT, PCD, A, D, PS1, G, Avail(3), PAT, Reserved(8),
	// Base Address(11)
	DWORD dwAttributeAndLowerBaseAddress;
	// Upper Base Address(8), Reserved(12), Avail(11), EXB
	DWORD dwUpperBaseAddressAndEXB;
} PML4TENTRY, PDPTENTRY, PDENTRY, PTENTRY;
#pragma pack ( pop )

// Functions
void kInitializePageTables( void );
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
		DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);

#endif /* __PAGE_H_ */
