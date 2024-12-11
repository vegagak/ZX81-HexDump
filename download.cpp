#include "stdafx.h"

DWORD retFileSize3(HANDLE h_File);
int FileExistCheckEx(HWND hWnd,WCHAR* szFileName);
DWORD DownloadFileToBufferSafe(WCHAR* szFileName,char* szBuffer, DWORD* dwBufferSize);

DWORD DownloadFileToBufferSafe(WCHAR* szFileName,char* szBuffer, DWORD* dwBufferSize)
{	// szFileName: file name to open, return contents in szBuffer
	// return value: bytes read, 0 for failure
	_TCHAR myErrString[300];
	DWORD dwBytesRead = 0;
	SetLastError(ERROR_SUCCESS); // for reading 0 bytes
	//check to make sure file already exists
	DWORD dwExists = GetFileAttributes(szFileName);
	if(dwExists == 0xFFFFFFFF)
	{
		_stprintf_s(myErrString,TEXT("ERROR_FILE_NOT_FOUND:\n\n%s"),szFileName);
		SetLastError(ERROR_FILE_NOT_FOUND);
		return 0;
	}
	//file exists, now get file size
	HANDLE hFile_htm = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile_htm == INVALID_HANDLE_VALUE)
	{
		_sntprintf_s(myErrString,(sizeof(myErrString)/sizeof(_TCHAR)) - 1,TEXT("DownloadFileToBufferSafe INVALID_HANDLE_VALUE: %s"), szFileName);
		return 0;
	}
	DWORD dwFS_low = retFileSize3(hFile_htm);
	if( dwFS_low == 0xFFFFFFFF)
	{
		CloseHandle(hFile_htm);
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		*dwBufferSize = 0;
		return 0;
	}
	if(	dwFS_low > *dwBufferSize )
	{
		CloseHandle(hFile_htm);
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		*dwBufferSize = dwFS_low;
		return 0;
	}
	char* szBufferA = new char[dwFS_low+1];
	if(!szBufferA)
	{
		//MessageBox(NULL, TEXT("ERROR_NOT_ENOUGH_MEMORY"),TEXT("DownloadFileToBufferSafe"),MB_OK);
		_sntprintf_s(myErrString,(sizeof(myErrString)/sizeof(_TCHAR)) - 1, TEXT("ERROR_NOT_ENOUGH_MEMORY - DownloadFileToBufferSafe\r\n"));
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return 0;
	}
	BOOL bSuccess;
	DWORD dwTotal = 0;
	dwBytesRead = 1;
	bSuccess= ReadFile (hFile_htm, szBufferA, 
		dwFS_low, &dwBytesRead, NULL);
	dwTotal += dwBytesRead;
	CloseHandle(hFile_htm);
	dwBytesRead = dwTotal;
	if (bSuccess && (dwBytesRead>0))
	{
		//success
		memcpy(szBuffer,szBufferA,dwBytesRead);
	}
	else if ((dwBytesRead != 0))
	{
		//failure
		_sntprintf_s(myErrString,(sizeof(myErrString)/sizeof(_TCHAR)) - 1, TEXT("Error %d reading file: %s"), GetLastError(), szFileName);
		//MessageBox(NULL, myErrString,TEXT("DownloadFileToBufferSafe"),MB_OK);
		OutputDebugString(TEXT("*** failed to read local File\n"));
		dwBytesRead = 0;
	}
	else
	{
		// 0 bytes read from file. OK, file is 0 bytes in size
	}
	delete [] szBufferA;
	return dwBytesRead;
}
DWORD retFileSize3(HANDLE h_File)
{	//return: bytes, or 0xFFFFFFFF = failure
	if(!h_File)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return 0xFFFFFFFF;
	}
	DWORD dwHigh = 0;
	LPDWORD lpFileSizeHigh = &dwHigh;
	DWORD dwLow = GetFileSize(h_File,lpFileSizeHigh);
	if( (dwLow == 0xFFFFFFFF) && (GetLastError() != NO_ERROR) )
	{   //technically this means the file size is 0xFFFFFFFF bytes
							//  we don't handle files that big
		return 0xFFFFFFFF;
	}
	if(*lpFileSizeHigh) // > 4GB, we don't handle files that big
	{
		return 0xFFFFFFFF;
	}
	return dwLow;
}
int FileExistCheckEx(HWND hWnd,WCHAR* szFileName)
{	// return IDNO if file should not be overwritten
	// return IDYES if file is ready to write
	// return IDCANCEL to disable prompt
	int x = IDYES;
	DWORD dw = GetFileAttributes(szFileName);
	if(dw != INVALID_FILE_ATTRIBUTES)
	{	
		WCHAR sz[200];
		_stprintf_s(sz,L"File already exists\n\n%s\n\nOverwrite?",szFileName);
		x = MessageBox(hWnd,sz,L"ZX81 HexDump",MB_YESNOCANCEL|MB_ICONEXCLAMATION);
	}
	return x;
}