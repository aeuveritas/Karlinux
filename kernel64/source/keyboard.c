#include "types.h"
#include "assemblyUtility.h"
#include "keyboard.h"

/* 
 * Input is from processor to devices
 * Output is from devices to processor
 */

///////////////////////////////////////////////////////////////////////////////
// Functions for keyboard controller & keyboard managerment
///////////////////////////////////////////////////////////////////////////////
// Return TRUE when there is received data.
BOOL kIsOutputBufferFull( void )
{
	// If the output buffer bit(bit 0) in the control/state register is 1,
	// there is received data in the output buffer
	// Output buffer is 0x60, its control/state register buffer ix 0x64 
	if ( kInPortByte( 0x64 ) & 0x01 )
	{
		return TRUE;
	}
	
	return FALSE;
}

// Return TRUE when there is remained data to use
BOOL kIsInputBufferFull( void )
{
	// If the input buffer bit(bit 1) in the control/state register is 1,
	// there is remianed data in the output buffer	
	// Output buffer is 0x60, its control/state register buffer ix 0x64
	if ( kInPortByte( 0x64 ) & 0x02 )
	{
		return TRUE;
	}
	
	return FALSE;
}

// Wait for ACK
//	If input is not ACK, translate it and put it to queue
unsigned char kWaitForACKAndPutOtherScanCode( void )
{
	int i, j;
	BYTE bData;
	BOOL bResult = FALSE;
	
	// Wait for ACK
	// Check ACK in received 100 data,
	// Because ACK can be aleady arrived at output buffer
	for ( j = 0 ; j < 100 ; j++ )
	{
		// 0xFFFF is enough time for emptying the input buffer.
		// Even though input buffer is not empty after 0xFFFF,
		// Ignore the remained data
		for ( i = 0 ; i < 0xFFFF ; i++ )
		{
			// If the output buffer is full, we cannot read data
			if ( kIsOutputBufferFull() == TRUE )
			{
				break;
			}
		}
		
		// If the read data is ACK, return TRUE
		bData = kInPortByte( 0x60 );
		if ( bData == 0xFA )
		{
			bResult = TRUE;
			break;
		}
		// If the read data is not ACL,
		// Translate it to ASCII code and put it to queue
		else
		{
			kConvertScanCodeAndPutQueue( bData );
		}
	}
	
	return bResult;
}


// Enable Keyboard
BOOL kActivateKeyboard( void )
{
	int i, j;
	BOOL bPreviousInterrupt;
	BOOL bResult;
	
	// Disable interrupt 
	bPreviousInterrupt = kSetInterruptFlag( FALSE );
	
	// Send enable command(0xAE) to control/state register(port 0x64)
	kOutPortByte( 0x64, 0xAE );
	
	// Til emptying the input buffer(port 0x60),
	// then send enable command(0xF4) to input buffer(port 0x60)
	// 0xFFFF is enough time for emptying the input buffer.
	// Even though input buffer is not empty after 0xFFFF,
	// Ignore the remained data
	for ( i = 0 ;  i < 0xFFFF ; i++ )
	{
		if ( kIsInputBufferFull() == FALSE )
		{
			break;
		}
	}
	
	// Send enable command(0xF4) to control/state register(port 0x64)
	kOutPortByte( 0x60, 0xF4 );

	// Wait for ACK 
	bResult = kWaitForACKAndPutOtherScanCode();
	
	// Restore previous interrupt
	kSetInterruptFlag( bPreviousInterrupt );
	
	return bResult;
}

// Read data from output buffer
BYTE kGetKeyboardScanCode( void )
{
	// Wait til there is data in output buffer(port 0x60)
	while ( kIsOutputBufferFull() == FALSE )
	{
		;
	}
	
	return kInPortByte( 0x60 );
}

// Keyboard LED On/Off
BOOL kChangeKeyboardLED(BOOL bCpasLockOn, BOOL bNumLockOn, BOOL bScrollLockOn)
{
	int i, j;
	BOOL bPreviousInterrupt;
	BOOL bResult;
	BYTE bData;
	
	// Disable interrupt
	bPreviousInterrupt = kSetInterruptFlag( FALSE );
	
	// Before sending LED change command,
	// Check output buffer is empty
	for ( i = 0 ; i < 0xFFFF ; i++ )
	{
		// If input buffer is empty, available to send command
		if ( kIsInputBufferFull() == FALSE )
		{
			break;
		}
	}
	
	// Send LED change command(0xED) to output buffer(port 0x60) 
	kOutPortByte( 0x60, 0xED );
	for ( i = 0 ; i < 0xFFFF ; i++ )
	{
		// If input buffer is empty,
		// sended command is taken by keyboard
		if ( kIsInputBufferFull() == FALSE )
		{
			break;
		}
	}
	
	// Wait for ACK
	bResult = kWaitForACKAndPutOtherScanCode();
	
	if ( bResult = FALSE )
	{
		// Restore previous interrupt
		kSetInterruptFlag( bPreviousInterrupt );
		return FALSE;
	}
	
	
	// Send changing value to keyboard and wait
	kOutPortByte( 0x60, ( bCpasLockOn << 2 ) | ( bNumLockOn << 1 ) | bScrollLockOn );
	for ( i = 0 ; i < 0xFFFF ; i++ )
	{
		// If input buffer is empty,
		// sended command is taken by keyboard
		if ( kIsInputBufferFull() == FALSE )
		{
			break;
		}
	}
	
	// Wait for ACK
	bResult = kWaitForACKAndPutOtherScanCode();
	
	// Restore previous interrupt
	kSetInterruptFlag( bPreviousInterrupt );
	return bResult;
	
}

// Enable A20 gate
void kEnableA20Gate( void )
{
	BYTE bOutputPortData;
	int i;
	
	// To read data of control/state register's output port
	// Send read command(0xD0) to control/state register
	kOutPortByte( 0x64, 0xD0 );
	
	// Read output port data
	for ( i = 0 ; i < 0xFFFF ; i++ )
	{
		// If output buffer is full, we can read data
		if ( kIsOutputBufferFull() == TRUE )
		{
			break;
		}
	}
	// Read output buffer data
	bOutputPortData = kInPortByte( 0x60 );
	
	// Set A20 gate bit
	bOutputPortData |= 0x01;
	
	// If input buffer(port 0x60) is empty,
	// Enable A20 gate
	for ( i = 0 ; i < 0xFFFF ; i++ )
	{
		// If input buffer is full, we can send data
		if ( kIsInputBufferFull() == FALSE )
		{
			break;
		}
	}
	
	// Send setting command(0xD1) to control/state register
	kOutPortByte( 0x64, 0xD1 );
	
	// Send A20 enalbe value to input buffer
	kOutPortByte( 0x60, bOutputPortData );
}

// Reset processor
void kReboot( void )
{
	int i;
	
	// If input buffer(port 0x60) is empty,
	// Enable A20 gate
	for ( i = 0 ; i < 0xFFFF ; i++ )
	{
		// If input buffer is full, we can send data
		if ( kIsInputBufferFull() == FALSE )
		{
			break;
		}
	}
	
	// Send setting command(0xD1) to control/state register
	kOutPortByte( 0x64, 0xD1 );
	
	// Send 0 to input buffer to reset processor
	kOutPortByte( 0x60, 0x00 );
	
	while ( 1 ) 
	{
		;
	}
}

// Check scan code is in alphabet range
BOOL kIsAlphabetScanCode( BYTE bScanCode )
{
	if ( ( 'a' <= gs_vstKeyMappingTable[bScanCode].bNormalCode ) &&
		( gs_vstKeyMappingTable[bScanCode].bNormalCode <= 'z' ) )
	{
		return TRUE;
	}
	
	return FALSE;
}

// Check scan code is in number or symbol range
BOOL kIsNumberOrSymbolScanCode( BYTE bScanCode )
{
	// In 2 ~ 53, except for alphabet, the key is number or symbol
	if ( ( 2 <= bScanCode ) && ( bScanCode <= 53 ) &&
		(kIsAlphabetScanCode( bScanCode ) == FALSE ) )
	{
		return TRUE;
	}
	
	return FALSE;
}

// Check scan code is number pad
BOOL kIsNumberPadScanCode( BYTE bScanCode )
{
	// number pad is in 71 ~ 83
	if ( ( 71 <= bScanCode ) && ( bScanCode <= 83 ) )
	{
		return TRUE;
	}
	
	return FALSE;
}

// Check scan code is combined 
BOOL kIsUseCombinedCode( BYTE bScanCode )
{
	BYTE bDownScanCode;
	BOOL bUseCombinedKey = FALSE;
	
	bDownScanCode = bScanCode & 0x7F;
	
	// If it is alphabet key,
	// Shift and Capslock key can affect
	if ( kIsAlphabetScanCode( bDownScanCode ) == TRUE )
	{
		// One of both work
		if ( gs_stKeyboardManager.bShiftDown ^ gs_stKeyboardManager.bCapsLockOn )
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}
	// If it is number or symbol key,
	// Shift key can affect
	else if ( kIsNumberOrSymbolScanCode( bDownScanCode ) == TRUE )
	{
		// Check shift key 
		if ( gs_stKeyboardManager.bShiftDown == TRUE )
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}
	// If it is number pas key,
	// Num Lock key can affect
	// If extended key code is not set,
	// combinedkey are the same
	else if ( ( kIsNumberPadScanCode( bDownScanCode ) == TRUE ) &&
		( gs_stKeyboardManager.bExtendedCodeIn == FALSE ) )
	{
		// Check Num Lock
		if ( gs_stKeyboardManager.bNumLockOn == TRUE )
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}
	
	return bUseCombinedKey;
}

// Update combination key and Sync LED 
void updateCombinationKeyStatusAndLED( BYTE bScanCode )
{
	BOOL bDown;
	BYTE bDownScanCode;
	BOOL bLEDStatusChanged = FALSE;
	
	// Check any key is down
	// The highest bit it 1, no key is down
	if ( bScanCode & 0x80 )
	{
		bDown = FALSE;
		bDownScanCode = bScanCode & 0x7F;
	}
	else
	{
		bDown = TRUE;
		bDownScanCode = bScanCode;
	}
	
	// Search combination key 
	// Shift
	if ( ( bDownScanCode == 42 ) || ( bDownScanCode == 54 ) )
	{
		gs_stKeyboardManager.bShiftDown = bDown;
	}
	// Caps Lock
	else if ( ( bDownScanCode == 58 ) && ( bDown == TRUE ) )
	{
		gs_stKeyboardManager.bCapsLockOn ^= TRUE;
		// Update LED 
		bLEDStatusChanged = TRUE;
	}
	// Num Lock
	else if ( ( bDownScanCode == 69 ) && ( bDown = TRUE ) )
	{
		gs_stKeyboardManager.bNumLockOn ^= TRUE;
		// Update LED 
		bLEDStatusChanged = TRUE;
	}
	// Scroll Lock
	else if ( ( bDownScanCode == 70 ) && ( bDown = TRUE ) )
	{
		gs_stKeyboardManager.bScrollLockOn ^= TRUE;
		// Update LED 
		bLEDStatusChanged = TRUE;
	}
	
	// Change LED State
	if ( bLEDStatusChanged == TRUE )
	{
		kChangeKeyboardLED(gs_stKeyboardManager.bCapsLockOn, 
			gs_stKeyboardManager.bNumLockOn, gs_stKeyboardManager.bScrollLockOn );
	}
}

// Transform scan code to ASCII cdoe
BOOL kConvertScanCodeToASCIICode( BYTE bScanCode, BYTE * pbASCIICode, BOOL * pbFlags )
{
	BOOL bUseCombinedKey;

	// If previous key was PAUSE, 
	// Ignore remain scan code of PAUSE
	if ( gs_stKeyboardManager.iSkipCountForPause > 0 )
	{
		gs_stKeyboardManager.iSkipCountForPause--;
		return FALSE;
	}

	// Special handler for PAUSE
	if ( bScanCode == 0xE1 )
	{
		*pbASCIICode = KEY_PAUSE;
		*pbFlags = KEY_FLAGS_DOWN;
		gs_stKeyboardManager.iSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;
		return TRUE;
	}
	// For extended key, set flag 
	else if ( bScanCode == 0xE0 )
	{
		gs_stKeyboardManager.bExtendedCodeIn = TRUE;
		return FALSE;
	}
	
	// Return combination key?
	bUseCombinedKey = kIsUseCombinedCode( bScanCode );
	
	// Set key value
	if ( bUseCombinedKey == TRUE ) 
	{
		*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bCombinedCode;
	}
	else
	{
		*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bNormalCode;
	}
	
	// Set extended key 
	if ( gs_stKeyboardManager.bExtendedCodeIn == TRUE )
	{
		*pbFlags = KEY_FLAGS_EXTENDEDKEY;
		gs_stKeyboardManager.bExtendedCodeIn = FALSE;
	}
	else
	{
		*pbFlags = 0;
	}
	
	// Down or Up
	if ( ( bScanCode & 0x80 ) == 0 )
	{
		*pbFlags |= KEY_FLAGS_DOWN;
	}
	
	// Update combination key down or up
	updateCombinationKeyStatusAndLED( bScanCode );
	
	// Skip single shift key
	if ( gs_stKeyboardManager.bShiftDown == TRUE &&
		( ( bScanCode & 0x7F ) == 42 || ( bScanCode & 0x7F ) == 54) )
		return FALSE;
	// Skip single Caps Lock key
	else if ( ( bScanCode & 0x7F ) == 58 )
		return FALSE;
	// Skip single Number Lock key 
	else if ( ( bScanCode & 0x7F ) == 69 )
		return FALSE;
	// Skip Scroll Lock key 
	else if ( ( bScanCode & 0x7F ) == 70 )
		return FALSE;
	
	return TRUE;
}

unsigned char kInitializeKeyboard( void )
{
	// Initialize Queue
	kInitializeQueue( &gs_stKeyQueue, gs_vstKeyQueueBuffer, KEY_MAXQUEUECOUNT,
				sizeof( KEYDATA ) );
	
	// Activate Keyboard
	return kActivateKeyboard();
}

// Translate scan code to internal key and put it to queue
unsigned char kConvertScanCodeAndPutQueue( BYTE bScanCode )
{
	KEYDATA stData;
	BOOL bResult = FALSE;
	BOOL bPreviousInterrupt;
	
	// Insert scan code to key data
	stData.bScanCode = bScanCode;
	
	// Translate scan code to ASCII code and put it to queue
	if ( kConvertScanCodeToASCIICode( bScanCode, &( stData.bASCIICode ),
		&( stData.bFlags ) ) == TRUE )
	{
		// Disable interrupt
		bPreviousInterrupt = kSetInterruptFlag( FALSE );
		
		// Put key to queue
		bResult = kPutQueue( &gs_stKeyQueue, &stData );
		
		// Restore previous interrupt
		kSetInterruptFlag( bPreviousInterrupt );
	}
	
	return bResult;
}

// Get key data from queue
BOOL kGetKeyFromKeyQueue( KEYDATA * pstData )
{
	BOOL bResult;
	BOOL bPreviousInterrupt;
	
	// If the queue is empty, we cannot get data
	if ( kIsQueueEmpty( &gs_stKeyQueue ) == TRUE )
	{
		return FALSE;
	}
	
	// Disable interrupt
	bPreviousInterrupt = kSetInterruptFlag( FALSE );
	
	// Get key data from key queue
	bResult = kGetQueue( &gs_stKeyQueue, pstData );
	
	// Restore previouse interrupt
	kSetInterruptFlag( bPreviousInterrupt );
	
	return bResult;
}
