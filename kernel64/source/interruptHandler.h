#ifndef __INTERRUPTHANDLER_H__
#define __INTERRUPTHANDLER_H__

#include "types.h"
#include "PIC.h"
#include "keyboard.h"
#include "console.h"
#include "utility.h"
#include "task.h"
#include "descriptor.h"

// Functions
void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode);
void kCommonInterruptHandler(int iVectorNumber);
void kKeyboardHandler(int iVectorNumber);
void kTimerHandler(int iVectorNumber);

#endif /* __INTERRUTHANDLER_H__ */
