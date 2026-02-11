/**
 * Types.h
 * Created by Matheus Leme Da Silva
 */
#ifndef TYPES_H
#define TYPES_H

typedef unsigned char Uint8;
typedef char Int8;
typedef unsigned short Uint16;
typedef short Int16;
typedef unsigned long Uint32;
typedef long Int32;

#define NULL ((void *)0)

#define ALIGN_UP(x,n) \
	(((x)+((n)-1)) & ~((n)-1))

#define offsetof(type,el) \
	((Uint16)&((((type)*)0)->el))

typedef Uint8 * VaList;

#define VaStart(ap,last) \
	((ap) = ((VaList)&(last) + ALIGN_UP(sizeof(last), 2)))

#define VaArg(ap,type) \
	((ap) += ALIGN_UP(sizeof(type), 2),*(type*)((ap) - ALIGN_UP(sizeof(type), 2)))

#define VaEnd(ap) \
	((ap) = NULL)

#endif // TYPES_H
