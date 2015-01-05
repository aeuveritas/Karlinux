#include "utility.h"
#include "assemblyUtility.h"

// Fill the memory with specific value
void kMemSet( void * pvDestination, BYTE bData, int iSize )
{
	int i;
	
	for ( i = 0 ; i < iSize ; i++ )
	{
		( ( char * ) pvDestination )[i] = bData;
	}
}

// Copy memory
int kMemCpy( void * pvDestination, const void * pvSource, int iSize )
{
	int i;
	
	for ( i = 0 ; i < iSize ; i++ )
	{
		( ( char * ) pvDestination )[i] = ( ( char * ) pvSource )[i];
	}
	
	return iSize;
}

// Compare memory
int kMemCmp( const void * pvDestination, const void * pvSource, int iSize )
{
	int i;
	char cTemp;
	
	for ( i = 0 ; i < iSize ; i++ )
	{
		cTemp = ( ( char * ) pvDestination )[i] - ( ( char * ) pvSource )[i];
		
		if ( cTemp != 0 )
		{
			return ( int ) cTemp;
		}
	}
	
	return 0;
}

// Swith interrupt flag in RFLAGS register,
// and return previous interrupt flag state
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
	QWORD qwRFLAGS;
	
	// Read previous RFLAGS register value,
	// and Enable/Disable interrupt
	qwRFLAGS = kReadRFLAGS();
	if ( bEnableInterrupt == TRUE )
	{
		kEnableInterrupt();
	}
	else
	{
		kDisableInterrupt();
	}
	
	// Check IF bit (bit 9) in previous RFLAGS register,
	// and return previous interrupt flag state
	if ( qwRFLAGS & 0x0200 )
	{
		return TRUE;
	}
	
	return FALSE;
}