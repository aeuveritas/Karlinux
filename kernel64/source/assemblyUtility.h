#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "types.h"
#include "task.h"

// Functions
BYTE kInPortByte(WORD wPort);
void kOutPortByte(WORD wPort, BYTE bData);
void kLoadGDTR(QWORD qwGDTRAddress);
void kLoadTR(WORD wTSSSegmentOffset);
void kLoadIDTR(QWORD qwIDTRAddress);
void kEnableInterrupt(void);
void kDisableInterrupt(void);
QWORD kReadRFLAGS(void);
QWORD kReadTSC(void);
void kSwitchContext(CONTEXT * pstCurrentContext, CONTEXT * pstNextContext);

#endif /* __ASSEMBLYUTILITY_H__ */
