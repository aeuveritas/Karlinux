#ifndef __MODESWITCH_H__
#define __MODESWITCH_H__

#include "types.h"

void kReadCPUID( DWORD dwEAX, DWORD * pdwEAX, DWORD * pdwEBX, DWORD * pdwECX, DWORD * pdwEDX );
void kSwitchExecute64bitKernel( void );

#endif

