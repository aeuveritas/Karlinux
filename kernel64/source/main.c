#include "types.h"
#include "keyboard.h"
#include "descriptor.h"
#include "PIC.h"
#include "console.h"
#include "consoleShell.h"
#include "task.h"
#include "PIT.h"

// Main function
void main(void)
{
	int iCursorX, iCursorY;
	
	kInitializeConsole( 0, 8 );
	
	kPrintStringXY( 45, 7, "Pass" );
	kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
	kPrintf( "Initialize Console..........................[Pass]\n" );
	
	kGetCursor( &iCursorX, &iCursorY );
	kPrintf( "GDT Initialize And Switch For IA-32e Mode...[    ]" );
	kInitializeGDTTableAndTSS();
	kLoadGDTR( GDTR_STARTADDRESS );
	kSetCursor( 45, iCursorY++ );
	kPrintf( "Pass\n" );
	
	kPrintf( "TSS Segment Load............................[    ]" );
	kLoadTR( GDT_TSSSEGMENT );
	kSetCursor( 45, iCursorY++ );
	kPrintf( "Pass\n" );
	
	kPrintf( "IDT Initialize..............................[    ]" );
	kInitializeIDTTables();
	kLoadIDTR( IDTR_STARTADDRESS );
	kSetCursor( 45, iCursorY++ );
	kPrintf( "Pass\n" );
	
	kPrintf( "Total RAM Size Check........................[    ]" );
	kCheckTotalRAMSize();
	kSetCursor( 45, iCursorY++ );
	kPrintf( "Pass], Size = %d MB\n", kGetTotalRAMSize() );
	
	kPrintf( "TCB Pool And Scheduler Initialize...........[PASS]\n" );
	iCursorY++;
	kInitializeScheduler();
	// For every 1ms, Occurs interrupt
	kInitializePIT(MSTOCOUNT(1), 1);
	
	kPrintf( "Keyboard Activate...........................[    ]" );
	// Enable keyboard
	if ( kInitializeKeyboard() == TRUE )
	{
		kSetCursor( 45, iCursorY++ );
		kPrintf( "Pass\n" );
		kChangeKeyboardLED( FALSE, FALSE, FALSE );
	}
	else
	{
		kSetCursor( 45, iCursorY++ );
		kPrintf( "Fail\n" );
	
		while ( 1 )
		{
			;
		}
	}
	
	kPrintf( "PIC Controller And Interrupt Initialize.....[    ]" );
	// Initiate PIC Controller and Enable All interrupt
	kInitializePIC();
	kMaskPICInterrupt( 0 );
	kEnableInterrupt();
	kSetCursor( 45, iCursorY++ );
	kPrintf( "Pass\n" );

	// Start shell 
	kStartConsoleShell();
}