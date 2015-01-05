#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "types.h"

// Structure
// Align 1 byte
#pragma pakc( push, 1 )

// Structure for Queue 
typedef struct kQueueManagerStruct
{
	int iDataSize;
	int iMaxDataCount;
	
	void * pvQueueArray;
	int iPutIndex;
	int iGetIndex;
	
	// Last operation is put?
	BOOL bLastOperationPut;
} QUEUE;

#pragma pack( pop )

// Functions
void kInitializeQueue( QUEUE * pstQueue, void * pvQueueBuffer, int iMaxDataCount,
		       int iDataSize );
BOOL kIsQueueFull( const QUEUE * pstQueue );
BOOL kIsQueueEmpty( const QUEUE * pstQueue );
BOOL kPutQueue( QUEUE * pstQueue, const void * pvData );
BOOL kGetQueue( QUEUE * pstQueue, void * pvData );

#endif /* __QUEUE_H__ */
