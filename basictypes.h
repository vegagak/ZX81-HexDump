#pragma once

extern HWND g_hWnd;
extern HWND g_hwndStartAddr;
extern HWND g_hwndColumns;

extern char* hexdump(BYTE* inputstring, size_t ccin, size_t* lpccout, char* szFileName);

typedef struct zx81vars
{
	BYTE bytes;
	char name[10];
	WORD addr;
}zx81vars;

struct charcodes {
	int number;
	char text[11];
};
