#include "utility.h"

// The number of PIT controller
volatile QWORD g_qwTickCount = 0;

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

// Return length of string
int kStrLen( const char* pcBuffer )
{
	int i;
	
	for ( i = 0 ; ; i++ )
	{
		if ( pcBuffer[i] == '\0' )
		{
			break;
		}
	}
	
	return i;
}

// Check RAM size above 64MB
//	At the first booting, it is only called once
void kCheckTotalRAMSize( void )
{
	DWORD* pdwCurrentAddress;
	DWORD dwPreviousValue;
	
	// From 64MB(0x4000000), Check each 4MB
	pdwCurrentAddress = ( DWORD * ) 0x4000000;
	/*
	while ( 1 )
	{
		// Store previous data temporary
		dwPreviousValue = *pdwCurrentAddress;
		
		// Check by writing and reading value
		*pdwCurrentAddress = 0x12345678;
		if ( *pdwCurrentAddress != 0x12345678 )
		{
			break;
		}
		
		// Restore pervious data
		*pdwCurrentAddress = dwPreviousValue;
		
		// Move to next 4MB
		pdwCurrentAddress += ( 0x400000 / 4 );
	}
	*/
	// Calculate How many 1MB is here
	gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}

// Return RAM size
QWORD kGetTotalRAMSize( void )
{
	return gs_qwTotalRAMMBSize;
}

// Internal implementation of atoi()
long kAToI( const char * pcBuffer, int iRadix )
{
	long lReturn;
	
	switch ( iRadix )
	{
	case 16:
		lReturn = kHexStringToQword( pcBuffer );
		break;
	case 10:
	default:
		lReturn = kDecimalStringToLong( pcBuffer );
		break;
	}
	
	return lReturn;
}

// Translate 16 digit to QWORD
QWORD kHexStringToQword( const char * pcBuffer )
{
	QWORD qwValue = 0;
	int i;
	
	// One by One
	for ( i = 0 ; pcBuffer[i] != '\0' ; i++ )
	{
		qwValue *= 16;
		
		if ( ( 'A' <= pcBuffer[i] ) && ( pcBuffer[i] <= 'Z' ) )
		{
			qwValue += ( pcBuffer[i] - 'A' ) + 10;
		}
		else if ( ( 'a' <= pcBuffer[i] ) && ( pcBuffer[i] <= 'z' ) )
		{
			qwValue += ( pcBuffer[i] - 'a' ) + 10;
		}
		else{
			qwValue += pcBuffer[i] - '0';
		}
	}
	
	return qwValue;
}

// Translate 10 digit to long
long kDecimalStringToLong( const char * pcBuffer )
{
	long lValue = 0;
	int i;
	
	// Except minus, translate to long
	if ( pcBuffer[0] == '-' )
	{
		i = 1;
	}
	else
	{
		i = 0;
	}
	
	// One by One
	for ( ; pcBuffer[i] != '\0' ; i++ )
	{
		lValue *= 10;
		lValue += pcBuffer[i] - '0';
	}
	
	// If it is minus
	if ( pcBuffer[0] == '-' )
	{
		lValue = -lValue;
	}
	
	return lValue;
}

// Internal implementation of itoa()
int kIToA( long lValue, char * pcBuffer, int iRadix )
{
	int iReturn;
	
	switch ( iRadix )
	{
	case 16:
		iReturn = kHexToString( lValue, pcBuffer );
		break;
	case 10:
	default:
		iReturn = kDecimalToString( lValue, pcBuffer );
		break;
	}
	
	return iReturn;
}

// Translate 16 digit to string
int kHexToString( QWORD qwValue, char * pcBuffer )
{
	QWORD i;
	QWORD qwCurrentValue;
	
	// If input is 0, process
	if ( qwValue == 0 )
	{
		pcBuffer[0] = '0';
		pcBuffer[1] = '\0';
		return 1;
	}
	
	// Get data and put buffer from right most position
	for ( i = 0 ; qwValue > 0 ; i++ )
	{
		qwCurrentValue = qwValue % 16;
		
		if ( qwCurrentValue >= 10 )
		{
			pcBuffer[i] = 'A' + ( qwCurrentValue - 10 );
		}
		else
		{
			pcBuffer[i] = '0' + qwCurrentValue;
		}
		
		qwValue = qwValue / 16;
	}
	
	pcBuffer[i] = '\0';
	
	// Reverse position
	kReverseString( pcBuffer );
	
	return i;
}

// Translate 10 digit to string
int kDecimalToString( long lValue, char * pcBuffer )
{
	long i;
	
	// If input is 0, process
	if ( lValue == 0 )
	{
		pcBuffer[0] = '0';
		pcBuffer[1] = '\0';
		return 1;
	}
	
	// If input is minus, insert '-'
	if ( lValue < 0 )
	{
		i = 1;
		pcBuffer[0] = '-';
		lValue = -lValue;
	}
	else
	{
		i = 0;
	}
	
	// Get data and put data from right most position
	for ( ; lValue > 0 ; i++ )
	{
		pcBuffer[i] = '0' + lValue % 10;
		lValue = lValue / 10;
	}
	pcBuffer[i] = '\0';
	
	// Reverse
	if ( pcBuffer[0] == '-' )
	{
		// If it is minus, reverse string without minus
		kReverseString( &( pcBuffer[1] ) );
	}
	else
	{
		kReverseString( pcBuffer );
	}
	
	return i;
}
	
// Revers string
void kReverseString( char * pcBuffer )
{
	int iLength;
	int i;
	char cTemp;
	
	// Center fixed
	iLength = kStrLen( pcBuffer );
	for ( i = 0 ; i < iLength / 2 ; i++ )
	{
		cTemp = pcBuffer[i];
		pcBuffer[i] = pcBuffer[iLength - 1 - i];
		pcBuffer[iLength - 1 - i] = cTemp;
	}
}

// Internal implementation of sprintf
int kSPrintf( char * pcBuffer, const char * pcFormatString, ... )
{
	va_list ap;
	int iReturn;
	
	// Get variable and give it to vsprintf()
	va_start( ap, pcFormatString );
	iReturn = kVSPrintf( pcBuffer, pcFormatString, ap );
	va_end( ap );
	
	return iReturn;
}

// Internal implementation of vsprintf()
//	Copy data to buffer along format string
int kVSPrintf( char * pcBuffer, const char * pcFormatString, va_list ap)
{
	QWORD i, j;
	int iBufferIndex = 0;
	int iFormatLength, iCopyLength;
	char * pcCopytString;
	QWORD qwValue;
	int iValue;

	// Read length of format string and send data to output buffer as the length
	iFormatLength = kStrLen( pcFormatString );
	for ( i = 0 ; i < iFormatLength ; i++ )
	{
		// If first is '%', it is data type
		if ( pcFormatString[i] == '%' )
		{
			// move to next
			i++;
			
			switch ( pcFormatString[i] )
			{
			// String
			case 's':
				// Translate parameter to string
				pcCopytString = ( char * ) ( va_arg( ap, char * ) ) ;
				iCopyLength = kStrLen( pcCopytString );
				// As the length of string,
				// Copy the string and send it to output buffer
				// Move index of buffer
				kMemCpy( pcBuffer + iBufferIndex, pcCopytString, iCopyLength );
				iBufferIndex += iCopyLength;
				break;
			// Character
			case 'c':
				// Translate parameter to character
				// Copy it to output buffer
				// Move buffer index
				pcBuffer[iBufferIndex] = ( char ) ( va_arg( ap, int ) );
				iBufferIndex++;
				break;
				
			// Integer
			case 'd':
			case 'i':
				// Translate parameter to int 
				// Copy it to output buffer
				// Move buffer index
				iValue = ( int ) ( va_arg( ap, int ) );
				iBufferIndex += kIToA( iValue, pcBuffer + iBufferIndex, 10 );
				break;
				
			// 4 byte HEX 
			case 'x':
			case 'X':
				// Translate parameter to DWORD
				// Copy it to output buffer
				// Move buffer index
				qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
				iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
				break;
				
			// 8 byte HEX 
			case 'q':
			case 'Q':
			case 'p':
				// Translate parameter to QWORD
				// Copyt it to output buffer
				// Move buffer index
				qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
				iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
				break;
				
			// ETC
			default:
				pcBuffer[iBufferIndex] = pcFormatString[i];
				iBufferIndex++;
				break;
			}
		}
		// Normal string
		else
		{
			// Print itself and move index
			pcBuffer[iBufferIndex] = pcFormatString[i];
			iBufferIndex++;
		}
	}
	
	// Insert NULL at the end of string
	pcBuffer[iBufferIndex] = '\0';
	
	return iBufferIndex;
}

// Return Tick Count 
QWORD kGetTickCount(void)
{
	return g_qwTickCount;
}
