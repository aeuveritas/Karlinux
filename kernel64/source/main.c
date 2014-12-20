#include "types.h"

void kPrintString( int ix, int iY, const char* pcString );

// Main function
void main(void)
{
	kPrintString( 45, 7, "Pass" );
	kPrintString( 0, 8, "IA-32e C Language Kernel Start..............[Pass]" );
}

// Print string
void kPrintString( int iX, int iY, const char* pcString )
{
	CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
	int i;

	pstScreen += (iY * 80 ) + iX;
	
	for ( i = 0 ; pcString[i] != 0 ; i++ )
	{
		pstScreen[i].bCharactor = pcString[i];
	}
}
