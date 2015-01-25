#ifndef __TASK_H__
#define __TASK_H__

#include "types.h"
#include "list.h"
#include "descriptor.h"

// MACRO
// 19 registers for SS, RSP, RFLAGS, CS, RIP + ISR
#define TASK_REGISTERCOUNT	(5+19)
#define TASK_REGISTERSIZE	8

// Register offset for context structure
#define TASK_GSOFFSET		0
#define TASK_FSOFFSET		1
#define TASK_ESOFFSET		2
#define TASK_DSOFFSET		3
#define TASK_R15OFFSET		4
#define TASK_R14OFFSET		5
#define TASK_R13OFFSET		6
#define TASK_R12OFFSET		7
#define TASK_R11OFFSET		8
#define TASK_R9OFFSET		10
#define TASK_R8OFFSET		11
#define TASK_RSIOFFSET		12
#define TASK_RDIOFFSET		13
#define TASK_RDXOFFSET		14
#define TASK_RCXOFFSET		15
#define TASK_RBXOFFSET		16
#define TASK_RAXOFFSET		17
#define TASK_RBPOFFSET		18
#define TASK_RIPOFFSET		19
#define TASK_CSOFFSET		20
#define TASK_RFLAGSOFFSET	21
#define TASK_RSPOFFSET		22
#define TASK_SSOFFSET		23

// Address of Task pool
#define TASK_TCBPOOLADDRESS 0x800000
#define TASK_MAXCOUNT		1024

// Size of Stack piil and Stack
#define TASK_STACKPOOLADDRESS (TASK_TCBPOOLADDRESS + sizeof(TCB) * TASK_MAXCOUNT)
#define TASK_STACKSIZE		8192

// Invalid task ID
#define TASK_INVALIDID		0xFFFFFFFFFFFFFFFF

// Max time for Task
#define TASK_PROCESSORTIME	5

// Structure
// Align 1 Byte
#pragma pack(push, 1)

// Structure for context
typedef struct kContextStruct
{
	QWORD vqRegister[TASK_REGISTERCOUNT];
} CONTEXT;

// Structure for task state
typedef struct kTaskControlBlockStruct
{
	// Location and ID for next data
	LISTLINK stLink;
	
	// Flag
	QWORD qwFlags;
	
	// Context
	CONTEXT stContext;

	// Address & Size for Stack
	void * pvStackAddress;
	QWORD qwStackSize;
} TCB;

// Structure for manager TCB pool
typedef struct kTCBPoolManagerStruct
{
	// Information for Task pool
	TCB * pstStartAddress;
	int iMaxCount;
	int iUseCount;
	
	// The number of TCB
	int iAllocatedCount;
} TCBPOOLMANAGER;

// Structure for Scheuler state
typedef struct kSchedulerStruct
{
	// Current Task
	TCB * pstRunningTask;
	
	// Used time
	int iProcessorTime;
	
	// List for ready Task
	LIST stReadyList;
} SCHEDULER;

// Structure for scheduler
static SCHEDULER gs_stScheduler;
static TCBPOOLMANAGER gs_stTCBPoolManager;

#pragma pack(pop)

// Functions
///////////////////////////////////////////////////////////////////////////////
// Task Pool and Task
///////////////////////////////////////////////////////////////////////////////
void kInitializeTCBPool(void);
TCB * kAllocateTCB(void);
void kFreeTCB(QWORD qwID);
TCB * kCreateTask(QWORD qwFlags, QWORD qwEntryPointAddress);
void kSetUpTask(TCB * pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void * pvStackAddress, QWORD qwStackSize);

///////////////////////////////////////////////////////////////////////////////
// Scheduler
///////////////////////////////////////////////////////////////////////////////
void kInitializeScheduler(void);
void kSetRunningTask(TCB * pstTask);
TCB * kGetRunningTask(void);
TCB * kGetNextTaskToRun(void);
void kAddTaskToReadyList(TCB * pstTask);
void kSchedule(void);
BOOL kScheduleInInterrupt(void);
void kScheduleProcessorTime(void);
BOOL kIsProcessorTimeExpired(void);

#endif /* __TASK_H__ */

