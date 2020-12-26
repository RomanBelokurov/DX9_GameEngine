
#ifndef SPELIB_H
#define SPELIB_H

#ifdef _WINDOWS
	#ifdef MAKE_DLL
	#define SPE_API __declspec(dllexport)
	#else
	#define SPE_API __declspec(dllimport)
	#endif
#else
	#define SPE_API
#endif

typedef unsigned char				    BYTE;
typedef unsigned short      			WORD;
typedef unsigned long      				DWORD;
typedef void*	SPEHANDLE;
typedef void* (*SPECallBackFunc)(void*);

#define SPE_POSITIVE_INFINITE 1.0e30f
#define SPE_NEGATIVE_INFINITE -1.0e30f

enum SPERESULT
{
	SPE_FAILED = 0,
	SPE_OK = 1
};

#endif

