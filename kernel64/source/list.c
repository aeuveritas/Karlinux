#include "list.h"

// Initialize list
void kInitializeList(LIST * pstList)
{
	pstList->iItemCount = 0;
	pstList->pvHeader = NULL;
	pstList->pvTail = NULL;
}

// Return the number of items
int kGetListCount(const LIST * pstList)
{
	return pstList->iItemCount;
}

// Add new data ot list
void kAddListToTail(LIST * pstList, void * pvItem)
{
	LISTLINK * pstLink;
	
	// Set the address of next data as NULL
	pstLink = (LISTLINK *)pvItem;
	pstLink->pvNext = NULL;
	
	// If list is empty, set header and tail as the new item
	if (pstList->pvHeader == NULL)
	{
		pstList->pvHeader = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;
		
		return;
	}
	
	// Add the new item
	pstLink = (LISTLINK *)pstList->pvTail;
	pstLink->pvNext = pvItem;
	
	// Set the last item of the list as the new item
	pstList->pvTail = pvItem;
	pstList->iItemCount++;
}

// Insert data into head of list
void kAddListToHeader(LIST * pstList, void * pvItem)
{
	LISTLINK * pstLink;
	
	pstLink = (LISTLINK *)pvItem;
	pstLink->pvNext = pstList->pvHeader;
	
	// If list is empty, set header and tail as the new item
	if (pstList->pvHeader == NULL)
	{
		pstList->pvHeader = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;
		
		return;
	}
	
	// Set first data of list as the the new item
	pstList->pvHeader = pvItem;
	pstList->iItemCount++;
}

// Return point of deleted data after delete it in list
void * kRemoveList(LIST * pstList, QWORD qwID)
{
	LISTLINK * pstLink;
	LISTLINK * pstPreviousLink;
	
	pstPreviousLink = (LISTLINK *)pstList->pvHeader;
	for (pstLink = pstPreviousLink ; pstLink != NULL ; pstLink = pstLink->pvNext)
	{
		// If ID is the same
		if (pstLink->qwID == qwID)
		{
			// If the ID is the last, initialize list
			if ( (pstLink == pstList->pvHeader ) &&
				(pstLink == pstList->pvTail ) )
			{
				pstList->pvHeader = NULL;
				pstList->pvTail = NULL;
			}
			// If the ID is the first, set the second as header
			else if (pstLink == pstList->pvHeader)
			{
				pstList->pvHeader = pstLink->pvNext;
			}
			// If the ID is the last, set the last second as tail
			else if (pstLink == pstList->pvTail)
			{
				pstList->pvTail = pstPreviousLink;
			}
			else
			{
				pstPreviousLink->pvNext = pstLink->pvNext;
			}
			
			pstList->iItemCount--;
			
			return pstLink;
		}
		
		pstPreviousLink = pstLink;
	}
	
	return NULL;
}

// Return pointer of list whose first is deleted
void * kRemoveListFromHeader(LIST * pstList)
{
	LISTLINK * pstLink;
	
	if (pstList->iItemCount == 0)
	{
		return NULL;
	}
	
	// Remove header and return changed list
	pstLink = (LISTLINK *)pstList->pvHeader;
	return kRemoveList(pstList, pstLink->qwID);
}

// Retrun pointer of list whose last is deleted
void * kRemoveListFromTail(LIST * pstList)
{
	LISTLINK * pstLink;
	
	if (pstList->iItemCount == 0)
	{
		return NULL;
	}
	
	// Remove tail and return changed list
	pstLink = (LISTLINK *)pstList->pvHeader;
	return kRemoveList(pstList, pstLink->qwID);
}

// Find item in list
void * kFindList(const LIST * pstList, QWORD qwID)
{
	LISTLINK * pstLink;
	
	for (pstLink = (LISTLINK *)pstList->pvHeader ; pstLink != NULL ;
		pstLink = pstLink->pvNext)
	{
		if (pstLink->qwID == qwID)
		{
			return pstLink;
		}
	}
	
	return NULL;
}

// Return header
void * kGetHeaderFromList(const LIST * pstList)
{
	return pstList->pvHeader;
}

// Return tail
void * kGetTailFromList(const LIST * pstList)
{
	return pstList->pvTail;
}

// Return next item
void * kGetNextFromList(const LIST * pstList, void * pstCurrent)
{
	LISTLINK * pstLink;
	
	pstLink = (LISTLINK *) pstCurrent;
	
	return pstLink->pvNext;
}
