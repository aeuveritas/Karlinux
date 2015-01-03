#include "interruptHandler.h"
#include "PIC.h"

// Common exception handler 
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
	char vcBuffer[3] = { 0, };

	// Print interrupt vector as 2 digit integer at right upper of monitor
	vcBuffer[0] = '0' + iVectorNumber / 10;
	vcBuffer[1] = '0' + iVectorNumber % 10;
	
	kPrintString( 0, 0, "======================================================================" );
	kPrintString( 0, 1, "                          Exception Occur!!                           " );
	kPrintString( 0, 2, "                             Vector :                                 " );
	kPrintString( 38, 2, vcBuffer );
	kPrintString( 0, 3, "======================================================================" );
	
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
	kPrintString( 67, 0, vcBuffer );
	// ====================================================================
	
	// Send EOI
	kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

// Handler for keyboard
void kKeyboardHandler( int iVectorNumber )
{
	char vcBuffer[] = "[INT:   ,  ] ";
	static int g_iCommonInterruptCount = 0;
	
	//=====================================================================
	// Print to notice the interrupt occur
	// Print interrupt vector as 2 digit integer at left upper of monitor
	vcBuffer[6] = '0' + iVectorNumber / 10;
	vcBuffer[7] = '0' + iVectorNumber % 10;
	// Print the number of interrupt
	vcBuffer[10] = '0' + g_iCommonInterruptCount;
	g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
	kPrintString( 0, 0, vcBuffer );
	// ====================================================================
	
	// Send EOI
	kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );	
}
