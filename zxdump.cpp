/*
 changes:

 ZX81 HexDump 6.5a (2024 11)
 * Add toolbar button for "Scan for alternate display files" to match the existing menu item
 * Added RETURN key accelerator for Columns & Starting Address (these setting now auto-refresh on enter)
 * Validates entries for Columns & Starting Address
 * Columns limited to 2-36 with + and - buttons added
 * Added "Reset Address" menu with ESCAPE key accelerator
 * Added the HexDump checkbox as a styling element
 * Added tooltip to Re-process button

 ZX81 HexDump 6.5 (2024 10)
 * coded in Win32
 * added variable column view (2-33 columns)
 * added starting address for display
 * added Visualize: Search for alternative display files (supports ZX81 & Lambda 8300 files)

 HexDump (1994)
 * written in Visual Basic
 * used ZX81 character set
*/

#include "stdafx.h"
#include "zxdump.h"
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <windowsx.h>
#include <Commdlg.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#define darkmode_
#ifdef darkmode_
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define OFFSET 16393 //0x4009
#define ADDRESS_BASE L"4009"
WORD address_start = OFFSET;

extern BOOL validatePfile(BYTE* inputstring,size_t ccin,char* outstring, size_t ccout);
extern DWORD retFileSize3(HANDLE h_File);
extern int DetectVersion(BYTE* bytestream);

void convert(BYTE* inputstring, size_t ccin, HWND hWnd, char* szFileName);
BOOL GetFileName(WCHAR* szFileName);
void ScanForDisplayFiles();
char* ShowAlternateDisplayFiles(char* is, int ccis);
void validateMinus();
void validatePlus();

HWND g_hWndZxdump;
WCHAR szWindowTitle[MAX_PATH];

#define FONT_SML 0
#define FONT_MED 1
#define FONT_LRG 2
UINT FONT_SIZE_ID = IDM_MENU_MED;
WCHAR g_szFont[20];
long g_MED_lfHeight;
long g_MED_lfWidth;
int status_y;
HBRUSH hbrBkgnd = NULL;
HBRUSH hbrBkgnd2 = NULL;
HBRUSH hbrBkgnd3 = NULL;
HBRUSH hBrushLabel;
COLORREF clrLabelText;
COLORREF clrLabelBkGnd;
HWND hWndOpen;
HWND hwndTipGo;
HWND hwndTipCols;
HWND g_WndStatus;
HWND g_hwndStartAddr;
HWND g_hwndColumns;
HWND g_hwndOPTIONstatic;
HWND g_hWndExportOpen;
HMENU g_hMenu;
HWND g_hwndCHKhexDump;
HWND g_hWndScanADF;
HWND g_hWndMinus;
HWND g_hWndPlus;

#define BTN_OPEN 68
#define BTN_PROCESS 6
#define BTN_EXPORT 7
#define BTN_EXPORT_OPEN 8
#define BTN_COMPARE 9
#define BTN_WINDIFF 10
#define BTN_MINUS 11
#define BTN_PLUS 12
#define BTN_SCANADF 13
#define BTN_FC 90
#define MAX_LOADSTRING 100
#define MAX_WEBPAGE_SIZE 1024*1024

// Global Variables:
BOOL g_ZxFontAvailable;
HWND hWndExportName;
HWND hWndPB;
WCHAR g_szFileName[MAX_PATH]{ 0 };
HWND hWndButtonProcess;
HWND hWndButtonExport;
HINSTANCE hInst;
HWND hDlg;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void OnButtonExport();
int FilePathExistQz(IN OUT _TCHAR* szPath, const size_t sizePath, HWND hWnd);
void OnProcess();
void OnOpen();
DWORD WINAPI ThreadProcess(LPVOID lpParameter);
int FileExistCheck(WCHAR* szFileName);
void OpenMyFile(WCHAR* szFileName);
DWORD DownloadFileToBufferSafe(WCHAR* szFileName,char* szBuffer, DWORD* dwBufferSize);
void OnExportOpen();
void OnResetAddress();
BOOL GetSaveName(WCHAR* szFileName, WCHAR* lpstrTitle, WCHAR* lpstrDefExt, int Flags, int nFilterIndex);
BOOL convertToStr(WCHAR* szWide, char* szANSI);

extern char* hexdump (BYTE* inputstring,size_t ccin, size_t* lpccout,char* szFileName);

BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID aFontCount) 
{ 
	g_ZxFontAvailable = FALSE;
  	if(0 == wcscmp(lplf->lfFaceName,L"SinclairZX"))
	{
		g_ZxFontAvailable = TRUE;
		return FALSE;
	}
	UNREFERENCED_PARAMETER( aFontCount ); 
	UNREFERENCED_PARAMETER( FontType ); 
    UNREFERENCED_PARAMETER( lplf ); 
    UNREFERENCED_PARAMETER( lpntm ); 
	return TRUE;
}
void CheckZxFont()
{
	int aFontCount[] = { 0, 0, 0 }; 
    EnumFontFamilies(GetDC(g_hWndZxdump), (LPCTSTR) NULL,(FONTENUMPROC) EnumFamCallBack, (LPARAM) aFontCount);
}
void args()
{
	 LPWSTR *szArgList; 
	 int argCount; 
	 WCHAR szArg[MAX_PATH]; *szArg= 0;
	 //UpdateWindow(hWnd);

     szArgList = CommandLineToArgvW(GetCommandLine(), &argCount); 
     
     if (argCount > 1)
		wcscpy_s(szArg,szArgList[1]);
  
     LocalFree(szArgList); 
	 
	 if(*szArg != 0)
		OpenMyFile(szArg);
}
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ZXDUMP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZXDUMP));
	SetFocus(g_WndStatus);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if(msg.hwnd == g_WndStatus)
		{
			if(msg.message == WM_CHAR)
				if(msg.wParam == VK_TAB)
					SetFocus(hWndOpen);
		}
		if(msg.wParam == VK_TAB)
		{
			if(!IsDialogMessage(g_hWndZxdump,&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if (!TranslateAccelerator(g_hWndZxdump, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}
void ResetFields()
{
		SetWindowText(g_WndStatus,L"");
		SetWindowText(hWndExportName,L"");
		EnableWindow(hWndButtonExport,FALSE);
		EnableWindow(g_hWndExportOpen,FALSE);
		EnableMenuItem(GetMenu(g_hWndZxdump),IDM_MENU_EXPORT,MF_GRAYED);
}
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZXDUMP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ZXDUMP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
#ifdef darkmode_
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	int nWidth = 640;
	int nHeight = 480;
	nWidth = 1400;
	nHeight = 1080;
	g_hWndZxdump = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, nWidth, nHeight, NULL, NULL, hInstance, NULL);
	
#ifdef darkmode_
	BOOL value = TRUE;
	::DwmSetWindowAttribute(g_hWndZxdump, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
#endif

	*szWindowTitle = 0;

	if (!g_hWndZxdump)
	{
		return FALSE;
	}
	ShowWindow(g_hWndZxdump, nCmdShow);
	UpdateWindow(g_hWndZxdump);
	hWndPB = NULL;
	hWndPB = NULL;
	return TRUE;
}
void ChangeFont(UINT ID)
{
	int cHeight = 0;
	if(ID == IDM_MENU_SML) cHeight = 12;
	if(ID == IDM_MENU_MED) cHeight = 0;
	if(ID == IDM_MENU_LRG) cHeight = 28;
	HFONT hFont=CreateFont(cHeight,0,0,0,0,0,0,0,0,0,0,0,0,g_szFont);
	SendMessage(g_WndStatus,WM_SETFONT,(WPARAM)hFont,0);
	SendMessage(g_WndStatus,WM_SIZE,(WPARAM)hFont,0);
	
	HMENU hMenu = GetMenu(g_hWndZxdump);
	CheckMenuItem(hMenu,IDM_MENU_SML,MF_UNCHECKED);
	CheckMenuItem(hMenu,IDM_MENU_MED,MF_UNCHECKED);
	CheckMenuItem(hMenu,IDM_MENU_LRG,MF_UNCHECKED);
	CheckMenuItem(hMenu,ID,MF_CHECKED);

	FONT_SIZE_ID = ID;		
}
char* getBS(WCHAR* szFileName,int* cb)
{
	HANDLE hFile_htm = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
	OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile_htm == INVALID_HANDLE_VALUE)
		return FALSE;
	DWORD dwFS_low = retFileSize3(hFile_htm);
	CloseHandle(hFile_htm);
	if( dwFS_low == 0xFFFFFFFF)
		return FALSE;
	
	char* bs1 = new char[dwFS_low+1];

	int n = DownloadFileToBufferSafe(szFileName,bs1,&dwFS_low);
	if (n == 0)
	{
		delete bs1;
		bs1 = 0;
	}
	*cb = n;
	return bs1;
}
void OnCheckLineLength()
{
	// open file
	// 407D PROGRAM
	// follow line trail
	WCHAR szFileName[MAX_PATH];
	BOOL b = GetFileName(szFileName);
	if (b)
	{
		DWORD dw = 64 * 1024;
		char* inputstring = new char[dw];

		int n = DownloadFileToBufferSafe(szFileName, inputstring, &dw);
		if (n)
		{
			MessageBox(g_hWndZxdump, L"Success", L"OnCheckLineLength", MB_OK);
		}
		else
		{
			MessageBox(g_hWndZxdump, L"Failure", L"OnCheckLineLength", MB_OK);
		}
	}
}
void OnTest()
{
	ScanForDisplayFiles();
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#define ROW_HEIGHT 24
#define ROW_SPACING 6
#define HORIZONTAL_SPACING 10
#define COLUMN_1 -60
#define COLUMN_2 COLUMN_1+80
#define COLUMN_3 640
	int ctl_width = 10;
	LRESULT lr;
	BOOL bstrict = FALSE;
	TOOLINFO toolInfo = { 0 };
	HWND hwndTip;
	BOOL b;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HFONT hFont;
	HBITMAP hBmp;
	HBITMAP hBmpRefresh;
	int y=-10;
	int x=0;
	RECT rect;
	HMENU hMenu;
	switch (message)
	{
	case WM_PARENTNOTIFY: 
		if (LOWORD(wParam) == WM_LBUTTONDOWN)
			x = 1;
	    break;
	case WM_LBUTTONDOWN:
		if (HWND(lParam) == g_WndStatus)
		{
			x = GET_X_LPARAM(lParam);			
		}
		break;
	case WM_CTLCOLORSTATIC:
		if (HWND(lParam) == g_WndStatus)
		{
			HDC hdc = reinterpret_cast<HDC>(wParam);
			SetTextColor(hdc, clrLabelText);
			SetBkColor(hdc, clrLabelBkGnd);
			if (!hBrushLabel) hBrushLabel = CreateSolidBrush(clrLabelBkGnd);
			return reinterpret_cast<LRESULT>(hBrushLabel);
		}
		if (HWND(lParam) == g_hwndOPTIONstatic )
		{
			if (hbrBkgnd == NULL)
			{
				hbrBkgnd = GetSysColorBrush(COLOR_WINDOW);
			}
			return (INT_PTR)hbrBkgnd;
		}
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		x = rect.right;
		y = rect.bottom;
		MoveWindow(g_WndStatus, rect.left + 20, status_y, rect.right - 20 /* 800 width */, y - status_y /* height */, FALSE);
		UpdateWindow(hWnd);
		break;
	case WM_CREATE:
		x = COLUMN_1;
		y = -20;
		y += ROW_HEIGHT + ROW_SPACING;
		x = COLUMN_2;

		//ROW ONE
		ctl_width = 30;
		hWndOpen = CreateWindow(L"BUTTON", L"Open",
			BS_BITMAP | WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
			x, y, ctl_width, ROW_HEIGHT, /* x,y,w,h */
			hWnd, (HMENU)BTN_OPEN, hInst, NULL);
		hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDI_OPEN));
		SendMessage(hWndOpen, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp);
		hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_ALWAYSTIP ,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hWndOpen, NULL,
			hInst, NULL);
		toolInfo.cbSize = sizeof(toolInfo)-4; //for XP
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.hwnd = hWndOpen;
		toolInfo.uId = (UINT_PTR)hWndOpen;
		GetClientRect(hWndOpen, &toolInfo.rect);
		toolInfo.hinst = hInst;
		toolInfo.lpszText = L"Open File";
		toolInfo.lParam = NULL;
		lr = SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

		x += ctl_width + HORIZONTAL_SPACING;
		ctl_width = 75;
		hWndButtonExport = CreateWindow(L"BUTTON", (LPCWSTR)L"Export",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
			x, y, ctl_width, ROW_HEIGHT, hWnd, (HMENU)BTN_EXPORT, /* x,y,w,h */
			(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
		x += ctl_width + HORIZONTAL_SPACING;
		ctl_width = 110;
		g_hWndExportOpen = CreateWindow(L"BUTTON", (LPCWSTR)L"Open Export",
			WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
			x, y, ctl_width, ROW_HEIGHT, /* x,y,w,h */
			hWnd, (HMENU)BTN_EXPORT_OPEN, hInst, NULL);
		x += ctl_width + 126 + HORIZONTAL_SPACING;
		ctl_width = 210;
		g_hWndScanADF = CreateWindow(L"BUTTON", (LPCWSTR)L"Scan for alternate display files",
			WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
			x, y, ctl_width, ROW_HEIGHT, /* x,y,w,h */
			hWnd, (HMENU)BTN_SCANADF, hInst, NULL);

		//ROW TWO
		x += HORIZONTAL_SPACING;
		ctl_width = 85;
					x = COLUMN_2;
			y += ROW_HEIGHT + ROW_SPACING; 
			CreateWindow(L"STATIC", (LPCWSTR)L"",
				WS_CHILD | SS_RIGHT | SS_BLACKFRAME | WS_VISIBLE,
				x , y-3 , 581, ROW_HEIGHT+6, /* x,y,w,h */
				hWnd, NULL, hInst, NULL);
			
			ctl_width = 133;
			g_hwndOPTIONstatic = CreateWindow(L"STATIC", (LPCWSTR)L"options",
				WS_CHILD | SS_RIGHT | SS_WHITEFRAME | WS_VISIBLE,
				x + 5, y + 4, 50, ROW_HEIGHT, /* x,y,w,h */
				hWnd, NULL, hInst, NULL);
			x += 50 + HORIZONTAL_SPACING;
			
			ctl_width = 100; //sized for 125% fonts
			x += HORIZONTAL_SPACING;
			g_hwndCHKhexDump = CreateWindow(L"BUTTON", (LPCWSTR)L"HexDump",
				WS_TABSTOP | WS_CHILD | BS_CHECKBOX | WS_VISIBLE,
				x, y, ctl_width, ROW_HEIGHT, hWnd, NULL, /* x,y,w,h */
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			Button_SetCheck(g_hwndCHKhexDump, BST_CHECKED);
			
			x += ctl_width + 116 + HORIZONTAL_SPACING;;
			ctl_width = 45;
			CreateWindow(L"STATIC", (LPCWSTR)L"Start: $",
				WS_CHILD | SS_RIGHT | SS_WHITEFRAME | WS_VISIBLE,
				x, y+1, ctl_width, ROW_HEIGHT, /* x,y,w,h */
				hWnd, NULL, hInst, NULL);
			x += ctl_width + 5;
			ctl_width = 54;
			g_hwndStartAddr = CreateWindow(L"EDIT", (LPCWSTR)ADDRESS_BASE,
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_WANTRETURN,
				x, y, ctl_width, ROW_HEIGHT, hWnd, NULL, /* x,y,w,h */
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			x += ctl_width + 5;
			ctl_width = 60;
			CreateWindow(L"STATIC", (LPCWSTR)L"Columns:",
				WS_CHILD | SS_RIGHT | SS_WHITEFRAME | WS_VISIBLE,
				x, y +1, ctl_width, ROW_HEIGHT, /* x,y,w,h */
				hWnd, NULL, hInst, NULL); 
			x += ctl_width + HORIZONTAL_SPACING;
			ctl_width = 18;
			g_hWndMinus = CreateWindow(L"BUTTON", (LPCWSTR)L"-",
				WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
				x, y, ctl_width, ROW_HEIGHT, /* x,y,w,h */
				hWnd, (HMENU)BTN_MINUS, hInst, NULL);
			x += ctl_width;
			ctl_width = 36;
			g_hwndColumns = CreateWindow(L"EDIT", (LPCWSTR)L"33",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_WANTRETURN,
				x, y, ctl_width, ROW_HEIGHT, hWnd, NULL, /* x,y,w,h */
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			x += ctl_width;
			ctl_width = 18;
			g_hWndPlus = CreateWindow(L"BUTTON", (LPCWSTR)L"+",
				WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
				x, y, ctl_width, ROW_HEIGHT, /* x,y,w,h */
				hWnd, (HMENU)BTN_PLUS, hInst, NULL);
			hwndTipCols = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
				WS_POPUP | TTS_ALWAYSTIP,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				g_hwndColumns, NULL,
				hInst, NULL);
			toolInfo.hwnd = g_hwndColumns;
			toolInfo.uId = (UINT_PTR)g_hwndColumns;
			GetClientRect(g_hwndColumns, &toolInfo.rect);
			toolInfo.lpszText = L"2-33";
			lr = SendMessage(hwndTipCols, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
			
			x = 566;
			ctl_width = 30;
			hWndButtonProcess = CreateWindow(L"BUTTON", (LPCWSTR)L"Re-process",
				BS_BITMAP | WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
				x, y, ctl_width, ROW_HEIGHT, hWnd, (HMENU)BTN_PROCESS,
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			
			hwndTipGo = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
				WS_POPUP | TTS_ALWAYSTIP,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				hWndButtonProcess, NULL,
				hInst, NULL);
			toolInfo.hwnd = hWndButtonProcess;
			toolInfo.uId = (UINT_PTR)hWndButtonProcess;
			GetClientRect(hWndButtonProcess, &toolInfo.rect);
			toolInfo.lpszText = L"Re-process";
			lr = SendMessage(hwndTipGo, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

			hBmpRefresh = LoadBitmap(hInst, MAKEINTRESOURCE(IDI_REFRESH));
			lr = SendMessage(hWndButtonProcess, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmpRefresh);

			
			y += ROW_HEIGHT + ROW_SPACING;
			hWndExportName = CreateWindow(L"EDIT",(LPCWSTR)L"", 
				WS_CHILD | /*WS_VISIBLE |*/ WS_BORDER | SS_LEFT | WS_DISABLED,
				x, y, 540+HORIZONTAL_SPACING, ROW_HEIGHT, /* x,y,w,h */
				hWnd, NULL, hInst, NULL); 
			
			//y += ROW_HEIGHT + ROW_SPACING*2;
			y += ROW_SPACING;
			status_y = y;
			g_WndStatus = CreateWindow(L"EDIT", (LPCWSTR)L"",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_WANTRETURN | WS_HSCROLL | WS_VSCROLL,
				COLUMN_2,y, 0 /* nWidth */,0 /* nHeight set by WM_SIZE */,hWnd, NULL, /* x,y,w,h */
				(HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE),NULL);
			//y += 200 + ROW_SPACING;
			hBrushLabel = NULL;
			clrLabelText = GetSysColor(COLOR_WINDOWTEXT);
			clrLabelBkGnd = GetSysColor(COLOR_WINDOW);

			CheckZxFont();
			if(g_ZxFontAvailable)
			{
				CheckMenuItem(GetMenu(hWnd),IDM_MENU_ZX,MF_CHECKED);
				wcscpy_s(g_szFont,20,TEXT("SinclairZX"));
			}
			else
			{
				CheckMenuItem(GetMenu(hWnd),IDM_MENU_CN,MF_CHECKED);
				wcscpy_s(g_szFont,20,TEXT("Courier New"));
			}
			hFont=CreateFont(0,0,0,0,0,0,0,0,0,0,0,0,0,g_szFont);
			SendMessage(g_WndStatus,WM_SETFONT,(WPARAM)hFont,0);
			g_hMenu = GetMenu(hWnd);
			
			EnableWindow(hWndButtonProcess,FALSE);
			EnableMenuItem(GetMenu(g_hWndZxdump),IDM_MENU_REPROCESS,MF_GRAYED);
			EnableWindow(hWndButtonExport,FALSE);
			EnableWindow(g_hWndExportOpen,FALSE);

			b = EnableMenuItem(GetMenu(hWnd),IDM_MENU_REPROCESS,MF_GRAYED);

			hdc  = GetDC(g_WndStatus);
			g_MED_lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
			g_MED_lfWidth = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSX), 72);
		    ReleaseDC(NULL, hdc);
			args();
			break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmEvent)
		{
		case BN_CLICKED:
				break;
		}
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_MENU_OPEN:
		case BTN_OPEN:
			OnOpen();
			break;
		case ID_MODIFY_TEST:
			OnTest();
			break;
		case IDM_MENU_RESET:
			OnResetAddress();
			break;
		case IDM_MENU_REPROCESS:
		case BTN_PROCESS:			
			OnProcess();
			break;
		case BTN_SCANADF:
			ScanForDisplayFiles();
			break;
		case BTN_EXPORT_OPEN:
			OnExportOpen();
			break;
		case BTN_PLUS:
			validatePlus();
			break;
		case BTN_MINUS:
			validateMinus();
			break;
		case IDM_MENU_EXPORT:
		case BTN_EXPORT:
			OnButtonExport();
			break;
		case IDM_MENU_SML:
			ChangeFont(IDM_MENU_SML);
			break;
		case IDM_MENU_MED:
			ChangeFont(IDM_MENU_MED);
			break;
		case IDM_MENU_LRG:
			ChangeFont(IDM_MENU_LRG);
			break;
		case IDM_MENU_CN:
			wcscpy_s(g_szFont,20,TEXT("Courier New"));
			hFont=CreateFont(0,0,0,0,0,0,0,0,0,0,0,0,0,g_szFont);
			SendMessage(g_WndStatus,WM_SETFONT,(WPARAM)hFont,0);
			SendMessage(g_WndStatus,WM_SIZE,(WPARAM)hFont,0);
			hMenu = GetMenu(g_hWndZxdump);
			CheckMenuItem(hMenu,IDM_MENU_CN,MF_CHECKED);
			CheckMenuItem(hMenu,IDM_MENU_ZX,MF_UNCHECKED);
			ChangeFont(FONT_SIZE_ID);
			break;
		case IDM_MENU_ZX:
			wcscpy_s(g_szFont,20,TEXT("SinclairZX"));
			hFont=CreateFont(0,0,0,0,0,0,0,0,0,0,0,0,0,g_szFont);
			SendMessage(g_WndStatus,WM_SETFONT,(WPARAM)hFont,0);
			SendMessage(g_WndStatus,WM_SIZE,(WPARAM)hFont,0);
			hMenu = GetMenu(g_hWndZxdump);
			CheckMenuItem(hMenu,IDM_MENU_ZX,MF_CHECKED);
			CheckMenuItem(hMenu,IDM_MENU_CN,MF_UNCHECKED);
			ChangeFont(FONT_SIZE_ID);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlgAbout, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlgAbout, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
BOOL convertToStr(WCHAR* szWide, char* szANSI)
{
	if (szANSI == NULL)
	{
		*szWide = NULL;
		return false;
	}
	size_t tRet;
	size_t count;

	count = wcslen(szWide) + 1;
	size_t lenW = count;
	size_t lenA = count;
	tRet = wcstombs_s(&lenA,szANSI,lenW,szWide, count);
	//tRet = wcstombs(szANSI, szWide, count);
	if (tRet >= 0)
	{
		if (tRet == count)
			szANSI[count] = 0;
		return true;
	}
	if (tRet == -1)
	{	//un-translatable character
		*szANSI = NULL;
		return false;
	}
	return false;
}
BOOL GetSaveName(WCHAR* szFileName, WCHAR* lpstrTitle, WCHAR* lpstrDefExt, int Flags, int nFilterIndex)
{
	OPENFILENAME ofn;       // common dialog box structure
	WCHAR szFile[MAX_PATH];

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWndZxdump;
	ofn.lpstrFile = szFileName;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not use the contents of szFile to initialize itself.
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Text\0*.txt\0Assembler files\0*.asm\0BASIC files\0*.bas\0All\0*.*\0";
	ofn.nFilterIndex = nFilterIndex;
	ofn.lpstrFileTitle = szFile;
	ofn.nMaxFileTitle = sizeof(szFile);
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = lpstrTitle;
	ofn.Flags = Flags;
	ofn.lpstrDefExt = lpstrDefExt;

	// Display the Open dialog box. 
	BOOL b = GetSaveFileName(&ofn);
	WCHAR szPath[MAX_PATH];
	wcscpy_s(szPath, ofn.lpstrFile);
	szPath[ofn.nFileOffset] = 0;
	return b;
}
BOOL GetFileName(WCHAR* szFileName)
{
	OPENFILENAME ofn;       // common dialog box structure
	WCHAR szFile[MAX_PATH];

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize		= sizeof(ofn);
	ofn.hwndOwner		= g_hWndZxdump;
	ofn.lpstrFile		= szFileName;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not use the contents of szFile to initialize itself.
	ofn.lpstrFile[0]	= '\0';
	ofn.nMaxFile		= sizeof(szFile);
	ofn.lpstrFilter		= L"P files, 81 files\0*.p;*.81\0All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex	= 1;
	ofn.lpstrFileTitle  = szFile;
	ofn.nMaxFileTitle	= sizeof(szFile);
	ofn.lpstrInitialDir = NULL;//C:\\Zx81";
	ofn.lpstrTitle      = L"Select a file";
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	BOOL b = GetOpenFileName(&ofn);
	WCHAR szPath[MAX_PATH];
	wcscpy_s(szPath,ofn.lpstrFile);
	szPath[ofn.nFileOffset]=0;
	return b;
}
void OpenMyFile(WCHAR* szFileName)	
{	
	wcscpy_s(g_szFileName, MAX_PATH, szFileName);

	LPWSTR  lpc = PathFindFileName(szFileName);

	LoadString(hInst, IDS_APP_TITLE, szWindowTitle,MAX_PATH);
	WCHAR szNewWindowName[MAX_PATH];
	_stprintf_s(szNewWindowName, MAX_PATH, TEXT("%s - %s"), lpc, szWindowTitle);
	SendMessage(g_hWndZxdump, WM_SETTEXT, 0, (LPARAM)szNewWindowName);

	ResetFields();
	
	DWORD dwThreadID = 0;
	CreateThread(NULL,0,ThreadProcess,0,0,&dwThreadID);
}
DWORD WINAPI ThreadOpenFile(LPVOID lpParameter)
{
	UNREFERENCED_PARAMETER(lpParameter);
	
	WCHAR szFileName[MAX_PATH];
	BOOL b = GetFileName(szFileName);
	if(b)
	{
		OpenMyFile(szFileName);
	}
	return 0;
}
DWORD WINAPI ThreadProcess(LPVOID lpParameter)
{
	UNREFERENCED_PARAMETER(lpParameter);
	if(*g_szFileName == 0)
	{
		SetWindowText(g_WndStatus,L"Open a file first");
		return 1;
	}
	DWORD dw = 64*1024;
	char* inputstring = new char[dw];

	int n = DownloadFileToBufferSafe(g_szFileName,inputstring,&dw);
	if (n)
	{
		char sFileName[MAX_PATH];	
		convertToStr(g_szFileName,sFileName);

		
#ifdef _DEBUG
		convert((BYTE*)inputstring, n, g_WndStatus, sFileName);
#else
		__try
		{
			convert((BYTE*)inputstring,n,g_WndStatus,sFileName);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			OutputDebugStringA("zxdump - exception\r\n");
			sprintf_s(inputstring,dw,"\r\n\r\nzxdump - exception");
			SetWindowTextA(g_WndStatus,inputstring);
		}		
#endif
		SetWindowText(hWndExportName,L"");
		EnableWindow(g_hWndExportOpen,FALSE);
		EnableWindow(hWndButtonProcess,TRUE);
		EnableMenuItem(GetMenu(g_hWndZxdump),IDM_MENU_REPROCESS,MF_ENABLED);
		EnableWindow(hWndButtonExport,TRUE);
		EnableMenuItem(GetMenu(g_hWndZxdump),IDM_MENU_EXPORT,MF_ENABLED);
	}
	else
	{
		SetWindowText(g_WndStatus,L"File is invalid");
		return 1;
	}
	SetFocus(g_WndStatus);
	SendMessage(g_WndStatus, EM_SETSEL, -1, 0);
	delete [] inputstring;
	return 0;
}
void OnOpen()
{
	DWORD dwThreadID = 0;
	CreateThread(NULL,0,ThreadOpenFile,0,0,&dwThreadID);
}
void OnProcess()
{
	DWORD selStart;
	DWORD selEnd;
	LRESULT result = SendMessage(g_WndStatus, EM_GETSEL, (WPARAM) & selStart, (LPARAM) & selEnd);
	DWORD dwThreadID = 0;
	CreateThread(NULL,0,ThreadProcess,0,0,&dwThreadID);
}
void OnExportOpen()
{
	WCHAR szFileName[MAX_PATH];
	GetWindowText(hWndExportName,szFileName,MAX_PATH);
	if(*szFileName == 0)
		return;
	ShellExecute(g_hWndZxdump,L"open",szFileName,NULL,NULL,SW_SHOWDEFAULT);
}
void OnButtonExport()
{
	WCHAR szFileName[MAX_PATH]; *szFileName = 0;
	WCHAR szFilePath[MAX_PATH]; 
	GetWindowText(hWndExportName, szFilePath, MAX_PATH);
	if (*szFilePath == 0)
	{
		wcscpy_s(szFilePath, MAX_PATH, g_szFileName);
	}
	LPCWSTR lpe = PathFindExtension(szFilePath);
	szFilePath[lpe - szFilePath] = 0;

	LPWSTR  lpc = PathFindFileName(szFilePath);
	wcscpy_s(szFileName, lpc);
	
	GetSaveName(szFileName, L"Save file As", L"txt", OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, 1);

	WCHAR wOutname[MAX_PATH];
	wcscpy_s(wOutname, szFileName);

	int i = GetWindowTextLength(g_WndStatus);
	if (i) 
	{
		i++;
		WCHAR* s = new WCHAR[i]; *s = 0;
		GetWindowText(g_WndStatus,s,i);
		int cc_s = i;
		char* sz = new char[i]; *sz = 0;
		convertToStr(s,sz);
		delete [] s;

			DWORD dwBytesWritten = 0;
			HANDLE hFile_Img = CreateFile(wOutname,GENERIC_WRITE,0,NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile_Img != INVALID_HANDLE_VALUE)
			{	//write bytes to a file
				int bResult = 0;
				if (cc_s > 0)
					bResult = WriteFile (hFile_Img, sz, cc_s,&dwBytesWritten, NULL);
				assert(bResult != 0);
			}
			CloseHandle(hFile_Img);
	
		delete [] sz;
		SetWindowText(hWndExportName,wOutname);
		EnableWindow(g_hWndExportOpen,TRUE);
	}
}
int FilePathExistQz(IN OUT _TCHAR* szPath, const size_t sizePath, HWND hWnd)
{
	int i=0;
	DWORD d2,d;

	d = GetFileAttributes (szPath);
	if( d == -1 ) 
	{
		LPVOID lpMsgBuf;

		d = GetLastError();
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			d,
			0, // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL );
		_TCHAR* s = new _TCHAR[MAX_PATH];
		if(d)
			_stprintf_s(s,MAX_PATH,
				TEXT("Log file Path Directory '%s' does not exist: %s(Error code: %d)\n\nPlease enter a valid Log file path."),
				szPath,(wchar_t*)lpMsgBuf,d);
		else
			_stprintf_s(s,MAX_PATH,
				TEXT("Log file Path Directory '%s' does not exist.\n(Error code: %d)\n\nPlease enter a valid Log file path."),
				szPath,d);
		i = MessageBox(hWnd,s,TEXT("Failure"),MB_OKCANCEL|MB_ICONEXCLAMATION);
		delete [] s;
		LocalFree( lpMsgBuf );
	}
	else
	{
		if( (!(d && FILE_ATTRIBUTE_DIRECTORY)) || (_tcslen(szPath)==0) ) 
		{
			_TCHAR* sz = new _TCHAR[MAX_PATH];
			_stprintf_s(sz,MAX_PATH,TEXT("GetFileAttributes return code: %d"),d);
			i = MessageBox(hWnd,sz,TEXT("Failure"),MB_OKCANCEL|MB_ICONEXCLAMATION);
			delete [] sz;
		}
	}
	d2 = _tcslen(szPath);
	if(d2>0)
	{
		if(szPath[d2-1] != '\\')
			_tcscat_s(szPath, sizePath, TEXT("\\"));
	}
	return i;
}
int FileExistCheck(WCHAR* szFileName)
{	// return IDNO if file should not be overwritten
	// return IDYES if file is ready to write
	int x = IDYES;
	DWORD dw = GetFileAttributes(szFileName);
	if(dw != INVALID_FILE_ATTRIBUTES)
	{	
		WCHAR sz[200];
		_stprintf_s(sz,L"File already exists\n\n%s\n\nOverwrite?",szFileName);
		x = MessageBox(g_hWndZxdump,sz,L"ZX81 HexDump",MB_YESNOCANCEL|MB_ICONEXCLAMATION);
		if(x == IDNO)
		{	
			_stprintf_s(sz,L"rename or move\n\n'%s'\n\nthen try again.",szFileName);
			MessageBox(g_hWndZxdump,sz,L"ZX81 HexDump",MB_OK);
		}
	}
	return x;
}

extern BYTE VERSN;
void convert(BYTE* bs, size_t cbbs, HWND hWnd, char* szFileName)
{
	size_t ccout = 0;
	char* outstring = hexdump(bs,cbbs,&ccout,szFileName);
	size_t length = strlen(outstring);
	//do not show last CrLf
	if (outstring[length - 2] == '\r')
	{
		outstring[length - 2] = 0;
	}
	SetWindowTextA(hWnd,outstring);
	delete [] outstring;
	SetFocus(g_WndStatus);
}
void ScanForDisplayFiles()
{
	DWORD dwExists = GetFileAttributes(g_szFileName);
	if (dwExists == 0xFFFFFFFF) {
		MessageBox(g_hWndZxdump, L"File not found.", g_szFileName, MB_OK);
		return;
	}
	HANDLE hFile_htm = CreateFile(g_szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile_htm == INVALID_HANDLE_VALUE) {
		MessageBox(g_hWndZxdump, L"INVALID_HANDLE_VALUE", L"CreateFile", MB_OK);
		return;
	}
	DWORD dwFS_low = retFileSize3(hFile_htm);
	CloseHandle(hFile_htm);
	if (dwFS_low == 0xFFFFFFFF) {
		MessageBox(g_hWndZxdump, L"ERROR_INSUFFICIENT_BUFFER", L"disableAutorun()", MB_OK);
		return;
	}

	char* inputstring = new char[dwFS_low + 1];
	char* s = 0;

	int n = DownloadFileToBufferSafe(g_szFileName, inputstring, &dwFS_low);
	if (n)
	{
		size_t cctemp = 120;	// size of SYSTEM VARS + 4
		char* working = new char[cctemp]; *working = 0;
		BOOL b = validatePfile((BYTE*)inputstring, dwFS_low, working, cctemp);
		delete working;
		if (b)
		{
			VERSN = DetectVersion((BYTE*)inputstring);
			s = ShowAlternateDisplayFiles(inputstring,n);
			SetWindowTextA(g_WndStatus, s);
			if (s)
				delete s;
		}
		else {
			SetWindowTextA(g_WndStatus, inputstring);
		}
	}
	delete inputstring;
}
void validatePlus()
{
	char sColumns[5];
	int ccs = 5;
	GetWindowTextA(g_hwndColumns, sColumns, ccs);
	int cols = atoi(sColumns);
	if (cols < 36)
	{
		cols++;
		sprintf_s(sColumns, ccs, "%i", cols);
		SetWindowTextA(g_hwndColumns, sColumns);
		OnProcess();
	}
}
void validateMinus()
{
	char sColumns[5];
	int ccs = 5;
	GetWindowTextA(g_hwndColumns, sColumns, ccs);
	int cols = atoi(sColumns);
	if (cols > 2)
	{
		cols--;
		sprintf_s(sColumns, ccs, "%i", cols);
		SetWindowTextA(g_hwndColumns, sColumns);
		OnProcess();
	}
}
void OnResetAddress()
{
	char s[7];
	int ccs = 7;
	sprintf_s(s, ccs, "%04X", address_start);
	SetWindowTextA(g_hwndStartAddr, s);
	OnProcess();
}