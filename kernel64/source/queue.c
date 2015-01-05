#include "queue.h"

// Initialize Queue
void kInitializeQueue( QUEUE * pstQueue, void * pvQueueBuffer, int iMaxDataCount,
		       int iDataSize )
{
	pstQueue->iMaxDataCount = iMaxDataCount;
	pstQueue->iDataSize = iDataSize;
	pstQueue->pvQueueArray = pvQueueBuffer;
	
	// Initialize put/get index
	pstQueue->iPutIndex = 0;
	pstQueue->iGetIndex = 0;
	
	pstQueue->bLastOperationPut = FALSE;
}

// Check queue is full
BOOL kIsQueueFull( const QUEUE * pstQueue )
{
	// If put & get index are the same,
	// and last operation is put,
	// then queue is full
	if ( ( pstQueue->iGetIndex == pstQueue->iPutIndex ) &&
		( pstQueue->bLastOperationPut == TRUE ) )
	{
		return TRUE;
	}
	
	return FALSE;
}

// Check queue is empty
BOOL kIsQueueEmpty( const QUEUE * pstQueue )
{
	// If put & get index are the same,
	// and last operation is get,
	// then queue is empty
	if ( ( pstQueue->iGetIndex == pstQueue->iPutIndex ) &&
		( pstQueue->bLastOperationPut == FALSE ) )
	{
		return TRUE;
	}
	
	return FALSE;
}

// Put data into Queue
BOOL kPutQueue( QUEUE * pstQueue, const void * pvData )
{
	// If queue is full, we cannot put data
	if ( kIsQueueFull( pstQueue ) == TRUE )
	{
		return FALSE;
	}
	
	// Copy data at the put index
	kMemCpy( ( char * ) pstQueue->pvQueueArray + ( pstQueue->iDataSize *
		pstQueue->iPutIndex ), pvData, pstQueue->iDataSize );
	
	// Change put index and bLastOperationPut
	pstQueue->iPutIndex = ( pstQueue->iPutIndex + 1 ) % pstQueue->iMaxDataCount;
	pstQueue->bLastOperationPut = TRUE;
	
	return TRUE;
}

// Get data from Queue
BOOL kGetQueue( QUEUE * pstQueue, void * pvData )
{
	// If the queue is empty, we cannot get data
	if ( kIsQueueEmpty( pstQueue ) == TRUE )
	{
		return FALSE;
	}
	
	// Copy data from the get index
	kMemCpy( pvData, ( char * ) pstQueue->pvQueueArray + ( pstQueue->iDataSize *
		pstQueue->iGetIndex ), pstQueue->iDataSize );
	
	// Change get index and bLastOperationPut
	pstQueue->iGetIndex = ( pstQueue->iGetIndex + 1 ) % pstQueue->iMaxDataCount;
	pstQueue->bLastOperationPut = FALSE;
	
	return TRUE;
}
