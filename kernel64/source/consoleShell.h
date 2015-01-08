#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "types.h"
#include "console.h"
#include "keyboard.h"
#include "utility.h"
#include "PIT.h"
#include "RTC.h"
#include "assemblyUtility.h"

// Macro
#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT 	300
#define CONSOLESHELL_PROMPTMESSAGE		"KARLINUX>"

// Define function pointer type to receive character pointer as a parameter
typedef void ( * CommandFunction ) ( const char * pcParameter );

// Structure
// Align 1 byte
#pragma pack( push, 1 )

// Structure for shell command 
typedef struct kShellCommnadEntryStruct
{
	// Command string
	char * pcCommand;
	// Command Help
	char * pcHelp;
	// Function pointer for executing command
	CommandFunction pfFunction;
} SHELLCOMMANDENTRY;

// Structure for information to process parameter
typedef struct kParameterListStruct
{
	// Address for parameter buffer
	const char * pcBuffer;
	// parameter length
	int iLength;
	// Start point for current parameter
	int iCurrentPosition;
} PARAMETERLIST;

#pragma pack( pop )

// Function
// Code for shell
void kStartConsoleShell( void );
void kExecuteCommand( const char * pcCommandBuffer );
void kInitializeParameter( PARAMETERLIST * pstList, const char * pcParameter );
int kGetNextParameter( PARAMETERLIST * pstList, char * pcParameter );

// Function for command
void kHelp( const char * pcParameterBuffer );
void kCls( const char * pcParameterBuffer );
void kShowTotalRAMSize( const char * pcParameterBuffer );
void kStringToDecimalHexTest( const char * pcParameterBuffer );
void kShutdown( const char * pcParameterBuffer );
void kSetTimer( const char * pcParameterBuffer );
void kWaitUsingPIT( const char * pcParameterBuffer );
void kReadTimeStampCounter( const char * pcParameterBuffer );
void kMeasureProcessorSpeed( const char * pcParameterBuffer );
void kShowDateAndTime( const char * pcParameterBuffer );

#endif /* __CONSOLESHELL_H__ */
