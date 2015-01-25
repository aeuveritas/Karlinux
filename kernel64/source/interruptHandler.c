#include "interruptHandler.h"
//#include "PIC.h"
//#include "keyboard.h"
//#include "console.h"

// Common exception handler 
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
	char vcBuffer[3] = { 0, };

	// Print interrupt vector as 2 digit integer at right upper of monitor
	vcBuffer[0] = '0' + iVectorNumber / 10;
	vcBuffer[1] = '0' + iVectorNumber % 10;
	
	kPrintStringXY( 0, 0, "======================================================================" );
	kPrintStringXY( 0, 1, "                          Exception Occur!!                           " );
	kPrintStringXY( 0, 2, "                             Vector :                                 " );
	kPrintStringXY( 38, 2, vcBuffer );
	kPrintStringXY( 0, 3, "======================================================================" );
	
	while( 1 ) ;
}

// Common interrupt handler
void kCommonInterruptHandler(int iVectorNumber)
{
	char vcBuffer[] = "[INT:   ,  ]";
	static int g_iCommonInterruptCount = 0;
	
	//=====================================================================
	// Print to notice the interrupt occur
	// Print interrupt vector as 2 digit integer at right upper of monitor
	vcBuffer[6] = '0' + iVectorNumber / 10;
	vcBuffer[7] = '0' + iVectorNumber % 10;
	// Print the number of interrupt
	vcBuffer[10] = '0' + g_iCommonInterruptCount;
	g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
	kPrintStringXY( 67, 0, vcBuffer );
	// ====================================================================
	
	// Send EOI
	kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

// Handler for keyboard
void kKeyboardHandler( int iVectorNumber )
{
	char vcBuffer[] = "[INT:   ,  ]";
	static int g_iCommonInterruptCount = 0;
	BYTE bTemp;
	
	//=========================================================================
	// Print to notice the interrupt occur
	// Print interrupt vector as 2 digit integer at left upper of monitor
	vcBuffer[6] = '0' + iVectorNumber / 10;
	vcBuffer[7] = '0' + iVectorNumber % 10;
	// Print the number of interrupt
	vcBuffer[10] = '0' + g_iCommonInterruptCount;
	g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
	kPrintStringXY( 0, 0, vcBuffer );
	//=========================================================================
	
	// Read data from key controller and translate the data to ASCII code and put it ti queue
	if ( kIsOutputBufferFull() == TRUE )
	{
		bTemp = kGetKeyboardScanCode();
		kConvertScanCodeAndPutQueue( bTemp );
	}
	
	// Send EOI
	kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );	
}

// Handler for Timer
void kTimerHandler(int iVectorNumber)
{
	char vcBuffer[] = "[INT:   ,  ]";
	static int g_iTimerInterruptCount = 0;
	
	//=========================================================================
	// Print to notice the interrupt occur
	// Print interupt vector as 2 degit integer at right upper of monitor
	vcBuffer[6] = '0' + iVectorNumber / 10;
	vcBuffer[7] = '0' + iVectorNumber % 10;
	// Print the number of interrupt
	vcBuffer[10] = '0' + g_iTimerInterruptCount;
	g_iTimerInterruptCount = (g_iTimerInterruptCount + 1) % 10;
	kPrintStringXY(68, 0, vcBuffer);
	//=========================================================================
	
	// Send EOI 
	kSendEOIToPIC(iVectorNumber - PIC_IRQSTARTVECTOR);
	
	// Add 1 to Timer counter
	g_qwTickCount++;
	
	// Reduce Used time 
	kDecreaseProcessorTime();
	// If the Used time exceeds max, switch task
	if ( kIsProcessorTimeExpired() == TRUE )
	{
		kScheduleInInterrupt();
	}
}
