#ifndef __TYPES_H__
#define __TYPES_H__

#define BYTE 	unsinged char
#define WORD	unsinged short
#define DWORD 	unsinged int
#define QWORD 	unsinged long
#define BOOL 	unsinged char

#define TRUE	1
#define FALSE	0
#define NULL	0

#pragma pack( push, 1)

// Text mode for video mode
typedef struct kCharactorStruct
{
	BYTE bCharactor;
	BYTE bAttribute;
} CHARACTER;

#pragma pack ( pop )
#endif /* __TYPE_H__ */

