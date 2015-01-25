#include "consoleShell.h"

// Define command table
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
	{ "help", "Show Help", kHelp },
	{ "cls", "Clear Screen", kCls },
	{ "totalram", "Show Total RAM Size", kShowTotalRAMSize },
	{ "strtod", "String To Decimal/Hex Convert", kStringToDecimalHexTest },
	{ "shutdown", "Shutdown And Reboot OS", kShutdown },
	{ "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)", kSetTimer },
	{ "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT },
	{ "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter },
	{ "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed },
	{ "date", "Show Date And Time", kShowDateAndTime },
	{ "createtask", "Create Task, ex)createtask 1(type) 10(count)", kCreateTestTask }
};

//=============================================================================
// Code for shell 
//=============================================================================
// Main loop
void kStartConsoleShell( void )
{
	char vcCommandBuffer[CONSOLESHELL_MAXCOMMANDBUFFERCOUNT];
	int iCommandBufferIndex = 0;
	BYTE bKey;
	int iCursorX, iCursorY;
	
	// Print prompt
	kPrintf( CONSOLESHELL_PROMPTMESSAGE );
	
	while( 1 )
	{
		// Wait for key 
		bKey = kGetCh();
		// If backspace
		if ( bKey == KEY_BACKSPACE )
		{
			if ( iCommandBufferIndex > 0 )
			{
				// Get current cursor point, move back on point
				// print white space, 
				// delete last character in command buffer
				kGetCursor( &iCursorX, &iCursorY );
				kPrintStringXY( iCursorX - 1, iCursorY, " " );
				kSetCursor( iCursorX - 1, iCursorY );
				iCommandBufferIndex--;
			}
		}
		// Process enter key 
		else if ( bKey == KEY_ENTER )
		{
			kPrintf( "\n" );
			
			if ( iCommandBufferIndex > 0 )
			{
				// Execute command in buffer
				vcCommandBuffer[iCommandBufferIndex] = '\0';
				kExecuteCommand( vcCommandBuffer );
			}
			
			// Print prompt and Initialize command buffer
			kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );
			kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
			iCommandBufferIndex = 0;
		}
		// Ignore Shift key, Caps lock, Num lock, Scroll lock
		else if ( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
			( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
			( bKey == KEY_SCROLLLOCK ) )
		{
			;
		}
		else
		{
			// Switch TAB to white space
			if ( bKey == KEY_TAB )
			{
				bKey = ' ';
			}
			
			// If buffer have room
			if ( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
			{
				vcCommandBuffer[iCommandBufferIndex++] = bKey;
				kPrintf( "%c", bKey );
			}
		}
	}
}

// Execute command if the command is one of the predefined
void kExecuteCommand( const char * pcCommandBuffer )
{
	int i, iSpaceIndex;
	int iCommandBufferLength, iCommandLength;
	int iCount;
	
	// Extract command
	iCommandBufferLength = kStrLen( pcCommandBuffer );
	for ( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
	{
		if( pcCommandBuffer[iSpaceIndex] == ' ' )
		{
			break;
		}
	}
	
	// Search the command in table
	iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
	for ( i = 0 ; i < iCount ; i++ )
	{
		iCommandLength = kStrLen( gs_vstCommandTable[i].pcCommand );
		// Compare length and command
		if ( ( iCommandLength == iSpaceIndex ) &&
			( kMemCmp( gs_vstCommandTable[i].pcCommand, pcCommandBuffer,
				   iSpaceIndex ) == 0 ) )
		{
			gs_vstCommandTable[i].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
			break;
		}
	}
	
	// If there is not same command
	if ( i >= iCount )
	{
		kPrintf( "'%s' is not found.\n", pcCommandBuffer );
	}
}

// Initialize parameter structure
void kInitializeParameter( PARAMETERLIST * pstList, const char * pcParameter )
{
	pstList->pcBuffer = pcParameter;
	pstList->iLength = kStrLen( pcParameter );
	pstList->iCurrentPosition = 0;
}

// Return parameter and its length
int kGetNextParameter( PARAMETERLIST * pstList, char * pcParameter )
{
	int i;
	int iLength;
	
	// If there is no parameter, exit
	if( pstList->iLength <= pstList->iCurrentPosition )
	{
		return 0;
	}
	
	// Search white space
	for ( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
	{
		if ( pstList->pcBuffer[i] == ' ' )
		{
			break;
		}
	}
	
	// Copy parameter and return length
	kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
	iLength = i - pstList->iCurrentPosition;
	pcParameter[iLength] = '\0';
	
	// Update parameter position 
	pstList->iCurrentPosition += iLength + 1;
	return iLength;
}

//=============================================================================
// Process command
//=============================================================================
// Help
void kHelp( const char * pcCommandBuffer )
{
	int i;
	int iCount;
	int iCursorX, iCursorY;
	int iLength, iMaxCommandLength = 0;
	
	kPrintf( "======================================================================\n" );
	kPrintf( "                          KARLINUX SHELL HELP                         \n" );
	kPrintf( "======================================================================\n" );
	
	iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
	
	// Calculate length of longest command
	for ( i = 0 ; i < iCount ; i++ )
	{
		iLength = kStrLen( gs_vstCommandTable[i].pcCommand );
		if ( iLength > iMaxCommandLength )
		{
			iMaxCommandLength = iLength;
		}
	}
	
	// Print help
	for ( i = 0 ; i < iCount ; i++ )
	{
		kPrintf( "%s", gs_vstCommandTable[i].pcCommand );
		kGetCursor( &iCursorX, &iCursorY );
		kSetCursor( iMaxCommandLength, iCursorY );
		kPrintf( " - %s\n", gs_vstCommandTable[i].pcHelp );
	}
}

// Clear Screen
void kCls( const char * pcParameterBuffer )
{
	// First line is debugging
	// So, second is shell start point
	kClearScreen();
	kSetCursor( 0, 1 );
}

// Print Total RAM Size
void kShowTotalRAMSize( const char * pcParameterBuffer )
{
	kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

// Translate number string to number
void kStringToDecimalHexTest( const char * pcParameterBuffer )
{
	char vcParameter[100];
	int iLength;
	PARAMETERLIST stList;
	int iCount = 0;
	long lValue;
	
	// Initialize parameter
	kInitializeParameter( &stList, pcParameterBuffer );
	
	while( 1 )
	{
		// Get next parameter
		iLength = kGetNextParameter( &stList, vcParameter );
		if( iLength == 0 )
		{
			break;
		}
		
		// Print information of parameter
		// Detect 16 or 10 digit
		kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1,
			 vcParameter, iLength );
		
		// If first is 0x, 16 digit
		// Other 10 digit
		if ( kMemCmp( vcParameter, "0x", 2 ) == 0 )
		{
			lValue = kAToI( vcParameter + 2, 16 );
			kPrintf( "HEX Value = %q\n", lValue );
		}
		else
		{
			lValue = kAToI( vcParameter, 10 );
			kPrintf( "Decimal Value = %d\n", lValue );
		}
		
		iCount++;
	}
}

// Restart PC
void kShutdown( const char* pcParameterBuffer )
{
	kPrintf( "System Shutdown Start..\n" );
	
	// PC restart by keyboard controller
	kPrintf( "Press Any Key To Reboot PC..." );
	kGetCh();
	kReboot();
}

// Set 0 to PIT Controller counter
void kSetTimer( const char * pcParameterBuffer )
{
	char vcParameter[100];
	PARAMETERLIST stList;
	long lValue;
	BOOL bPeriodic;
	
	// Initialize parameter
	kInitializeParameter( &stList, pcParameterBuffer );
	
	// Extract milisecond
	if ( kGetNextParameter( &stList, vcParameter ) == 0 )
	{
		kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
		return;
	}
	lValue = kAToI( vcParameter, 10 );
	
	// Extract periodic
	if ( kGetNextParameter( &stList, vcParameter ) == 0 )
	{
		kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
		return;
	}
	bPeriodic = kAToI( vcParameter, 10 );
	
	kInitializePIT( MSTOCOUNT( lValue ), bPeriodic );
	kPrintf( "Time= %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic );
}

// Wait for ms by PIT controller
void kWaitUsingPIT( const char * pcParameterBuffer )
{
	char vcParameter[100];
	int iLength;
	PARAMETERLIST stList;
	long lMillisecond;
	int i;
	
	// Initialize parameter
	kInitializeParameter( &stList, pcParameterBuffer );
	if (kGetNextParameter( &stList, vcParameter ) == 0 )
	{
		kPrintf( "ex) wait 100(ms)\n" );
		return;
	}

	lMillisecond = kAToI( pcParameterBuffer, 10 );
	kPrintf( "%d ms Sleep Start...\n", lMillisecond );
	
	// Disable interrupt and measure the time by PIT controller
	kDisableInterrupt();
	for ( i = 0 ; i < lMillisecond / 30 ; i++ )
	{
		kWaitUsingDirectPIT( MSTOCOUNT( 30 ) );
	}
	kWaitUsingDirectPIT( MSTOCOUNT( lMillisecond % 30 ) );
	kEnableInterrupt();
	kPrintf( "%d ms Sleep Complete\n", lMillisecond );
	
	// Restore kSetTimer
	kInitializePIT( MSTOCOUNT( 1 ), TRUE );
}

// Read Time stamp count
void kReadTimeStampCounter( const char * pcParameterBuffer )
{
	QWORD qwTSC;
	
	qwTSC = kReadTSC();
	kPrintf( "Time Stamp Counter = %q\n", qwTSC );
}

// Measure the speed of processor
void kMeasureProcessorSpeed( const char * pcParameterBuffer )
{
	int i;
	QWORD qwLastTSC, qwTotalTSC = 0;
	
	kPrintf( "Now Measuring." );
	
	// For 10 seconds, calculate processor speed by measuring the change of time stamp
	kDisableInterrupt();
	for ( i = 0 ; i < 200 ; i++ )
	{
		qwLastTSC = kReadTSC();
		kWaitUsingDirectPIT( MSTOCOUNT( 50 ) );
		qwTotalTSC += kReadTSC() - qwLastTSC;
		
		kPrintf( "." );
	}

	// Restore timer 
	kInitializePIT( MSTOCOUNT( 1 ), TRUE );
	kEnableInterrupt();
	
	kPrintf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

// Print data and time in RTC controller
void kShowDateAndTime( const char * pcParameterBuffer )
{
	BYTE bSecond, bMinute, bHour;
	BYTE bDayOfWeek, bDayOfMonth, bMonth;
	WORD wYear;
	
	// Read date and time from RTC controller
	kReadRTCTime( &bHour, &bMinute, &bSecond );
	kReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );

	kPrintf( "Data: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth, kConvertDayOfWeekToString( bDayOfWeek ) );
	kPrintf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

// Define TCB structure and Stack
static TCB gs_vstTask[2] = {0, };
static QWORD gs_vstStack[1024] = {0, };

// Test for context switching
void kTestTask(void)
{
	int i = 0;

	while (1)
	{
		// Print message and wait for key input
		kPrintf("[%d] This message is from kTestTask. Press any key to switch " "kConsolShell!!\n", i++);
		kGetCh();

		// Context switching
		kSwitchContext(&(gs_vstTask[1].stContext), &(gs_vstTask[0].stContext));
	}
}
// Task 1
// 		Print character around monitor
void kTestTask1(void)
{
	BYTE bData;
	int i = 0, iX = 0, iY = 0, iMargin;
	CHARACTER * pstScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;
	TCB * pstRunningTask;
	
	// Get ID and use is as offset of monitor
	pstRunningTask = kGetRunningTask();
	iMargin = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) % 10;
	
	while (1)
	{
		switch (i)
		{
		case 0:
			iX++;
			if ( iX >= (CONSOLE_WIDTH - iMargin) )
			{
				i = 1;
			}
			break;
		case 1:
			iY++;
			if ( iY >= (CONSOLE_HEIGHT - iMargin) )
			{
				i = 2;
			}
			break;
		case 2:
			iX--;
			if ( iX < iMargin )
			{
				i = 3;
			}
			break;
		case 3:
			iY--;
			if ( iY < iMargin )
			{
				i = 0;
			}
			break;
		}
		
		// Set characture and color
		pstScreen[iY * CONSOLE_WIDTH + iX].bCharactor = bData;
		pstScreen[iY * CONSOLE_WIDTH + iX].bAttribute = bData & 0x0F;
		bData++;
		
		// Switch task
		kSchedule();
	}
}

// Task 2
// 		Along ID, print rotating character
void kTestTask2(void)
{
	int i = 0;
	int iOffset;
	CHARACTER * pstScreen = (CHARACTER *) CONSOLE_VIDEOMEMORYADDRESS;
	TCB * pstRunningTask;
	char vcData[4] = {'-', '\\', '|', '/'};
	
	// Get ID and use is as offset of monitor
	pstRunningTask = kGetRunningTask();
	iOffset = (pstRunningTask->stLink.qwID & 0xFFFFFFFF) * 2;
	iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - (iOffset % (CONSOLE_WIDTH * CONSOLE_HEIGHT));
	
	while (1)
	{
		// Set rotating character
		pstScreen[iOffset].bCharactor = vcData[i % 4];
		// Set Color
		pstScreen[iOffset].bAttribute = (iOffset % 15) + 1;
		i++;
		
		// Switch task
		kSchedule();
	}
}

// Create a task and excute it
void kCreateTestTask(const char * pcParameterBuffer)
{
	PARAMETERLIST stList;
	char vcType[30];
	char vcCount[30];
	int i = 0;

	// Get parameter
	kInitializeParameter(&stList, pcParameterBuffer);
	kGetNextParameter(&stList, vcType);
	kGetNextParameter(&stList, vcCount);

	switch (kAToI(vcType, 10))
	{
	case 1:
		for ( i = 0 ; i < kAToI(vcCount, 10) ; i++ )
		{
			if ( kCreateTask(0, (QWORD)kTestTask1) == NULL )
			{
				break;
			}
		}
		
		kPrintf("Task1 %d Created\n", i);
		break;
	case 2:
	default:
		for ( i = 0 ; i < kAToI(vcCount, 10) ; i++ )
		{
			if ( kCreateTask(0, (QWORD)kTestTask2) == NULL )
			{
				break;
			}
		}
		
		kPrintf("Task2 %d Created\n", i);
		break;
	}
}

