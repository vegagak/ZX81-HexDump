#pragma once

extern DWORD DownloadFileToBufferSafe(WCHAR* szFileName, char* szBuffer, DWORD* dwBufferSize);
extern DWORD retFileSize3(HANDLE h_File);
extern int FileExistCheckEx(HWND hWnd, WCHAR* szFileName);
