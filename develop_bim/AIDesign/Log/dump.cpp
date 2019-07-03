#include "dump.h"
#include <DbgHelp.h>
#include<stdio.h>
#include "../../ComUtil.h"

#pragma comment(lib,"Dbghelp.lib")

void CreateDumpFile( struct _EXCEPTION_POINTERS* pExceptionPointers )
{
	char szPath[MAX_PATH] = { 0 };
	char szFileName[MAX_PATH] = { 0 };

	DWORD dwBufferSize = MAX_PATH;
	HANDLE hDumpFile = NULL;

	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION ExParam;
	GetLocalTime(&stLocalTime);

	//GetCurrentDirectory(MAX_PATH,szPath );

	sprintf(szFileName, "%s/%04d_%02d_%02d_%02d_%02d_%02d_%03d.dmp", 
		ComUtil::dump_path.c_str(),
		stLocalTime.wYear, 
		stLocalTime.wMonth, 
		stLocalTime.wDay, 
		stLocalTime.wHour, 
		stLocalTime.wMinute, 
		stLocalTime.wSecond, 
		stLocalTime.wMilliseconds);

	hDumpFile = ::CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);

	ExParam.ThreadId = ::GetCurrentThreadId();
	ExParam.ExceptionPointers = pExceptionPointers;
	ExParam.ClientPointers = TRUE;

	MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hDumpFile, 
		(MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs/* | MiniDumpWithFullMemory*/ | MiniDumpWithThreadInfo), &ExParam, NULL, NULL);
	CloseHandle(hDumpFile);

	return;
}
LONG WINAPI DumpExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionPointers)
{
	CreateDumpFile(pExceptionPointers);
	return EXCEPTION_EXECUTE_HANDLER;
}