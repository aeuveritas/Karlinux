#include "PIC.h"

// Initiate PIC
void kInitializePIC( void )
{
	// Initiate master PIC 
	// ICW1( port 0x20 ), IC4 bit( bit 0 ) = 1
	kOutPortByte( PIC_MASTER_PORT1, 0x11 );
	
	// ICW2( posrt 0x21 ), Interrupt vector ( 0x20 )
	kOutPortByte( PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR );
	
	// ICW3( port 0x21 ), location for slave PIC controller
	// second pin is for master PIC, so 0x04( bit 2 )
	kOutPortByte( PIC_MASTER_PORT2, 0x04 );
	
	// ICW4( port 0x21 ), uPM bit(bit 0) = 1
	kOutPortByte( PIC_MASTER_PORT2, 0x01 );
	
	// Initiate slave PIC 
	// ICW1( port 0xA0 ), IC4 bit( bit 0 ) = 1
	kOutPortByte( PIC_SLAVE_PORT1, 0x11 );
	
	// ICW2( port 0xA1 ), Interrupt vector( 0x20 + 8 )
	kOutPortByte( PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8 );
	
	// ICW3( port 0xA1 ), location for master PIC controller
	// second pin is for master PIC, so 0x02
	kOutPortByte( PIC_SLAVE_PORT2, 0x02 );
	
	// ICW4( port 0xA1 ), uPM bit( bit 0 ) = 1
	kOutPortByte( PIC_SLAVE_PORT2, 0x01 );
}

// To forbid some interrupt, set interrupt mastk
void kMaskPICInterrupt( WORD wIRQBitmask )
{
	// Set IMR in master PIC controller
	// OCW1( port 0x21 ), IRQ 0 - IRQ 7
	kOutPortByte( PIC_MASTER_PORT2, ( BYTE ) wIRQBitmask );
	
	// Set IMR in slave PIC controller
	// OCW1( port 0xA1 ), IRQ 8 - IRQ 15
	kOutPortByte( PIC_SLAVE_PORT2, ( BYTE ) ( wIRQBitmask >> 8 ) );
}

// Send acknowledge for finishing interrupt handling
// For master PIC controller, Send EOI to only master
// For slave PIC controller, send EOI to master and slave
void kSendEOIToPIC(int iIRQNumber)
{
	// Send EOI to master 
	// OCW2( port 0x20 ), EOI bit( bit 5 ) = 1
	kOutPortByte( PIC_MASTER_PORT1, 0x20 );
	
	// If interrupt was for slave, send EOI to slave too
	if ( iIRQNumber >= 8 )
	{
		// OCW2 ( port 0xA0 ), EOI bit ( bit 5 ) = 1
		kOutPortByte( PIC_SLAVE_PORT1, 0x20 );
	}
}
