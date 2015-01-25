#include "task.h"

///////////////////////////////////////////////////////////////////////////////
// Task Pool and Task
///////////////////////////////////////////////////////////////////////////////
// Initialize Task poll
void kInitializeTCBPool(void)
{
	int i;
	
	kMemSet(&(gs_stTCBPoolManager ), 0, sizeof(gs_stTCBPoolManager));
	
	// Initialize address of task pool
	gs_stTCBPoolManager.pstStartAddress = (TCB *)TASK_TCBPOOLADDRESS;
	kMemSet(TASK_TCBPOOLADDRESS, 0, sizeof(TCB) * TASK_MAXCOUNT);

	// Assign ID to TCB 
	for ( i = 0 ; i < TASK_MAXCOUNT ; i++ )
	{
		gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID = i;
	}
	
	// Initilize the number of TCBs
	gs_stTCBPoolManager.iMaxCount = TASK_MAXCOUNT;
	gs_stTCBPoolManager.iAllocatedCount = 1;
}

// Allocate TCB
TCB * kAllocateTCB(void)
{
	TCB * pstEmptyTCB;
	int i;
	
	if ( gs_stTCBPoolManager.iUseCount == gs_stTCBPoolManager.iMaxCount )
	{
		return NULL;
	}

	for ( i = 0 ; i < gs_stTCBPoolManager.iMaxCount ; i++ )
	{
		// If upper 32 bit of ID is 0, it is not allocated TCB
		if ( ( gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID >> 32 ) == 0 )
		{
			pstEmptyTCB = &(gs_stTCBPoolManager.pstStartAddress[i]);
			break;
		}
	}
	
	// Give non zero value to upper 32 bit to allocate TCB
	pstEmptyTCB->stLink.qwID = ((QWORD)gs_stTCBPoolManager.iAllocatedCount << 32 ) | i;
	gs_stTCBPoolManager.iUseCount++;
	gs_stTCBPoolManager.iAllocatedCount++;
	
	if ( gs_stTCBPoolManager.iAllocatedCount == 0 )
	{
		gs_stTCBPoolManager.iAllocatedCount = 1;
	}
	
	return pstEmptyTCB;
}

// Deallocate TCB
void kFreeTCB(QWORD qwID)
{
	int i;
	
	// Lower 32 bit is index of Task ID
	i = qwID & 0xFFFFFFFF;
	
	// Initialize TCB and Set ID
	kMemSet(&(gs_stTCBPoolManager.pstStartAddress[i].stContext), 0, sizeof(CONTEXT));
	gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID = i;
	
	gs_stTCBPoolManager.iUseCount--;
}

// Create Task
// 		Along the Task ID, assign stack automatically in stack pool
TCB * kCreateTask(QWORD qwFlags, QWORD qwEntryPointAddress)
{
	TCB * pstTask;
	void * pvStackAddress;
	
	pstTask = kAllocateTCB();
	if ( pstTask == NULL )
	{
		return NULL;
	}
	
	// Calculate stack address from Task ID
	// Lower 32 bit is offset of stack pool
	pvStackAddress = (void *)(TASK_STACKPOOLADDRESS + (TASK_STACKSIZE * (pstTask->stLink.qwID & 0xFFFFFFFF)));
	
	// Set TCB and insert it into ready list to be scheduled
	kSetUpTask(pstTask, qwFlags, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);
	kAddTaskToReadyList(pstTask);
	
	return pstTask;
}

// Set TCB with parameters
void kSetUpTask(TCB * pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void * pvStackAddress, QWORD qwStackSize)
{
	// Initialize context
	kMemSet(pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));

	// Set RSP, RBP register related to stack
	pstTCB->stContext.vqRegister[TASK_RSPOFFSET] = (QWORD)pvStackAddress + qwStackSize;
	pstTCB->stContext.vqRegister[TASK_RBPOFFSET] = (QWORD)pvStackAddress + qwStackSize;

	// Set Segment selector
	pstTCB->stContext.vqRegister[TASK_CSOFFSET] = GDT_KERNELCODESEGMENT;
	pstTCB->stContext.vqRegister[TASK_DSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_ESOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_FSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_GSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_SSOFFSET] = GDT_KERNELDATASEGMENT;

	// Set RIP register and Interrupt flag
	pstTCB->stContext.vqRegister[TASK_RIPOFFSET] = qwEntryPointAddress;

	// Set IF bit in RFLAGS register as 1 to enable interrupt
	pstTCB->stContext.vqRegister[TASK_RFLAGSOFFSET] |= 0x0200;

	// Store ID & Stack & Flag
	pstTCB->pvStackAddress = pvStackAddress;
	pstTCB->qwStackSize = qwStackSize;
	pstTCB->qwFlags = qwFlags;
}

///////////////////////////////////////////////////////////////////////////////
// Scheduler
///////////////////////////////////////////////////////////////////////////////
// Initialize Scheduler
//		Initialize with TCB Pool and init task
void kInitializeScheduler(void)
{
	// Initialize Task pool
	kInitializeTCBPool();
	
	// Initialize Ready list
	kInitializeList(&(gs_stScheduler.stReadyList));
	
	// Allocate TCB of current task which is a booting task
	gs_stScheduler.pstRunningTask = kAllocateTCB();

}

// Set curent task
void kSetRunningTask(TCB * pstTask)
{
	gs_stScheduler.pstRunningTask = pstTask;
}

// Return current task
TCB * kGetRunningTask(void)
{
	return gs_stScheduler.pstRunningTask;
}

// Get next task from task list
TCB * kGetNextTaskToRun(void)
{
	if ( kGetListCount(&(gs_stScheduler.stReadyList)) == 0 )
	{
		return NULL;
	}
	
	return (TCB *) kRemoveListFromHeader(&(gs_stScheduler.stReadyList));
}

// Insert task to ready list of scheduler
void kAddTaskToReadyList(TCB * pstTask)
{
	kAddListToTail(&(gs_stScheduler.stReadyList), pstTask);
}

// Find other task and switch
//		It should not be called when interrupt happens
void kSchedule(void)
{
	TCB * pstRunningTask;
	TCB * pstNextTask;
	BOOL bPreviousFlag;
	
	if ( kGetListCount(&(gs_stScheduler.stReadyList)) == 0 )
	{
		return;
	}
	
	// Disable interrupt
	bPreviousFlag = kSetInterruptFlag(FALSE);
	// Get next task
	pstNextTask = kGetNextTaskToRun();
	if ( pstNextTask == NULL )
	{
		kSetInterruptFlag( bPreviousFlag );
		return;
	}
	
	pstRunningTask = gs_stScheduler.pstRunningTask;
	kAddTaskToReadyList(pstRunningTask);
	
	// Context switch
	gs_stScheduler.pstRunningTask = pstNextTask;
	kSwitchContext(&(pstRunningTask->stContext), &(pstNextTask->stContext));
	
	// Update Used time
	gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;
	
	kSetInterruptFlag(bPreviousFlag);
}

// When interrupt happens, switch other task
// 		After interrupt, it should be called
unsigned char kScheduleInInterrupt(void)
{
	TCB * pstRunningTask;
	TCB * pstNextTask;
	char * pcContextAddress;
	
	pstNextTask = kGetNextTaskToRun();
	if ( pstNextTask == NULL )
	{
		return FALSE;
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Switching Tasks 
	// 		Switch other context and context which is stored by interrupt handler
	///////////////////////////////////////////////////////////////////////////
	pcContextAddress = (char *) IST_STARTADDRESS + IST_SIZE - sizeof(CONTEXT);
	
	// Get current task and copy context in IST, then move current task to ready list
	pstRunningTask = gs_stScheduler.pstRunningTask;
	kMemCpy(&(pstRunningTask->stContext), pcContextAddress, sizeof(CONTEXT));
	kAddTaskToReadyList(pstRunningTask);
	
	// Set next task as running task, and copy context to IST to automatically change
	gs_stScheduler.pstRunningTask = pstNextTask;
	kMemCpy(pcContextAddress, &(pstNextTask->stContext), sizeof(CONTEXT));
	
	// Update Used time
	gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;
	return TRUE;
}

// Reduce Used time by 1
void kDecreaseProcessorTime(void)
{
	if ( gs_stScheduler.iProcessorTime > 0 )
	{
		gs_stScheduler.iProcessorTime--;
	}
}

// Check exceed max time
BOOL kIsProcessorTimeExpired(void)
{
	if ( gs_stScheduler.iProcessorTime <= 0 )
	{
		return TRUE;
	}
	
	return FALSE;
}
