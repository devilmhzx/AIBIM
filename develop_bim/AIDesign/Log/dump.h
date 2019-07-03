#pragma once
#include <Windows.h>
#include "../CommonSettings.h"
LONG WINAPI DumpExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionPointers);
inline LONG WINAPI ExceptionFilterNull(struct _EXCEPTION_POINTERS* pExceptionPointers){return EXCEPTION_EXECUTE_HANDLER;}
#if 1
#define TryDump( cppcode ) 	_try{ cppcode } __except( DumpExceptionFilter(GetExceptionInformation()) ){ return 0; }
#else
#define TryDump( cppcode ) cppcode
#endif

#define TryNull( cppcode ) _try{ cppcode } __except( ExceptionFilterNull(GetExceptionInformation()) ){}