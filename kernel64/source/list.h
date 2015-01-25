#ifndef __LIST_H__
#define __LIST_H__

#include "types.h"

// Structure
// Align 1 byte
#pragma pack(push, 1)

// Structure for data link
// It should be at the first position in any data
typedef struct kListLinkStruct
{
	// Identifier
	void * pvNext;
	QWORD qwID;
} LISTLINK;

/* 
// Example
// LISTLINK should be the first
struct kListItemExampleStruct
{
	// Struct for list link
	LISTLINK stLink;
	
	// Data
	int iData1;
	char cData2;
};
*/

// Structure for list management
typedef struct kListManagerStruct
{
	// The number of count
	int iItemCount;
	
	// First and last elements
	void * pvHeader;
	void * pvTail;
} LIST;

#pragma pack(pop)

// Functions
void kInitializeList(LIST* pstList);
int kGetListCount(const LIST * pstList);
void kAddListToTail(LIST * pstList, void * pvItem);
void * kRemoveList(LIST * pstList, QWORD qwID);
void * kRemoveListFromHeader(LIST * pstList);
void * kRemoveListFromTail(LIST * pstList);
void * kFindList(const LIST * pstList, QWORD qwID);
void * kGetHeaderFromList(const LIST * pstList);
void * kGetTailFromList(const LIST * pstList);
void * kGetNExtFromList(const LIST * pstList, void * pstCurrent);

#endif /* __LIST_H__ */
