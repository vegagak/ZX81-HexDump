#include "stdafx.h"
#include "zx81s.h"
#include "L83.h"

//host app will supply these GUI items
extern HWND hWndExportName; // text window for status updates
int FileExistCheckEx(HWND hWnd,WCHAR* szFileName);

char (*code2ascii)[11];
//charcodes *CHRcodes;
WORD VARS =0;
BYTE VERSN = 0;

const char* GetFileTitle(char* szFileName)
{
	char* p = strrchr(szFileName,'\\');
	p++;
	return p;
}
BOOL convertToWide2(size_t* lenW, WCHAR* szWide,char* szANSI)
{
	if(szWide == NULL)
	{
		*szANSI = NULL;
		return false;
	}
	size_t tRet;
	size_t count;
	count = strlen(szANSI);
	size_t ccConverted = 0;
	tRet = mbstowcs_s(&ccConverted,szWide,*lenW,szANSI,count);
	if(tRet >= 0)
	{	
		if(tRet == count)
		{   //not null-terminated
			szWide[count] = 0;
		}
		return true;
	}
	if(tRet == -1)
	{  //un-translatable character
		*szWide = NULL;
		return false;
	}
	return false;
}
void GetFileNameOut(char* szFileName,char* outname,WCHAR* wOutname)
{
	char FilePath[MAX_PATH]; *FilePath=0;
	if(szFileName)
	{
		char* p = strrchr(szFileName,'\\');
		if(p != 0)
		{
			int x = p - szFileName;
			strncpy_s(FilePath,MAX_PATH,szFileName,x+1);
			FilePath[x+1]=0;
		}
	}
	strcat_s(FilePath,outname);
	size_t size = MAX_PATH;
	convertToWide2(&size,wOutname,FilePath);
}
BYTE GetSysByte(WORD _offset, BYTE* p_stream)
{
	return p_stream[_offset];
}
WORD GetAddress(WORD _offset, BYTE* p_stream)
{
	BYTE d = p_stream[_offset];
	BYTE e = p_stream[_offset + 1];
	WORD address = d + e * 256;
	return address;
}
int DetectVersion(BYTE* bytestream)
{
	//Double-check the reported version in the P file, 
	// return actual version (ZX81 or Lambda 8300 or ???
	int DetectedVersion;
	WORD D_FILE;
	BYTE dd;
	int versn = GetSysByte(0, bytestream);
	DetectedVersion = versn;
	if (versn == 0x00) {
		//check for Original Lambda file format. Example: DEV_DEMO.P is marked 0x00, but is actually a Lambda format image
		D_FILE = GetAddress(16396 - offset, bytestream);
		dd = GetSysByte(D_FILE - offset, bytestream);
		if (dd != 0x76)
		{
			DetectedVersion = 0xFF; //VERSN Lambda type FF
		}
	}
	else if (versn == 0xFF) {
		//verify Lambda file format. Example: StoreDemo_original.p is actually a ZX81 file
		D_FILE = GetAddress(16396 - offset, bytestream);
		dd = GetSysByte(D_FILE - offset, bytestream);
		if (dd == 0x76)
		{
			DetectedVersion = 0x00; //VERSN ZX81 type 00
		}
	}
	else {
		char s[4]; *s = 0;
		sprintf_s(s, "%d", versn);
		MessageBoxA(g_hWnd, s, "Version", MB_OK | MB_ICONEXCLAMATION);
	}
	return DetectedVersion;
}
BOOL validatePfile(BYTE* inputstring, size_t ccin, char* outstring, size_t ccout)
{
	//basic checks to determine if it is a valid P file, 
	//and not simply a renamed file, or a damaged/truncated P file
	if (ccin < 1)
	{
		strcpy_s(outstring, ccout, "file is empty");
		return FALSE;
	}
	if (ccin < 142)
	{
		strcpy_s(outstring, ccout, "file too short to be valid");
		return FALSE;
	}
	if (VERSN == 0xFF) {
	}
	else if (VERSN != 0)
	{
		sprintf_s(outstring, ccout, "file may not be not a valid ZX81 BASIC file:\r\n\r\nVERSN is 0x%02x", VERSN);
		//return FALSE;
	}
	WORD D_FILE = GetAddress(16396 - offset, (BYTE*)inputstring);
	VARS = GetAddress(16400 - offset, (BYTE*)inputstring);
	WORD STKEND = GetAddress(16412 - offset, (BYTE*)inputstring);
	sprintf_s(outstring, ccout, "file does not appear to be a valid P file:\r\n\r\nVARS is 0x%04x", VARS);
	if ((VARS - offset > (WORD)ccin) || (VARS < PROGRAM) || (VARS > 32767))
		return FALSE;
	sprintf_s(outstring, ccout, "file does not appear to be a valid P file:\r\n\r\nD_FILE is 0x%04X\nvars is 0x%04X", D_FILE, VARS);
	if ((D_FILE - offset > (WORD)ccin) || (D_FILE < PROGRAM) || (D_FILE > 32767) || (D_FILE >= VARS))
		return FALSE;
	sprintf_s(outstring, ccout, "file does not appear to be a valid ZX81 file:\r\n\r\nSTKEND is 0x%04X", STKEND);
	if ((STKEND < PROGRAM) || (STKEND > 32767) || (STKEND <= VARS))
		return FALSE;
	*outstring = 0;
	return TRUE;
}

size_t optionSizeHD(size_t rem_size)
{
	size_t size_needed = 0;
	//16 bytes in = 76 bytes out
	//1  byte  in = 68 bytes out
	int remainder = rem_size % 16;
	int full = rem_size / 16;
	if (full)
	{	// 76 per line
		//     // 439B  76809C9C 9C9C9C9C 80808080 80808080  .±ÔÔÔÔÔÔ±±±±±±±± v...............
		size_needed = full * 16; // header for notiscode (3 less for code)
		size_needed += full * 16 * 5; // ~5 chars per byte
		size_needed += full * 2; // carriage return
		size_needed = size_needed - 2; //no need for last last CR
	}
	if (remainder)
	{
		size_needed += 16; // fixed line
		size_needed += 52; // fixed hex display
		size_needed += remainder; //variable char display
		size_needed += 2;  //line feed + carriage return
	}
	size_needed += 16000; //mcline
	return size_needed;
}
char* HexHeader2(WORD address, int columns)
{
	//e.g. for 33 columns: 
	//"ADDR   0 1 2 3  4 5 6 7  8 9 A B  C D E F  0 1 2 3  4 5 6 7  8 9 A B  C D E F  0 ZX81                              ASCII\r\n"
	char hexd[]="0123456789ABCDEF";
	//int cc = 68;
	int dividers = columns / 4;
	int cc = strlen("ADDR  ") + 2 * columns +strlen(" ZX81                               ASCII\r\n") + dividers + 1;
	char* hh = new char[cc];
	
	strcpy_s(hh,cc,"ADDR  ");
	int m = address % columns;
	int z = strlen(hh);
	for(int f=0,m1=1;f< columns;f++,m1++)
	{
		hh[z]=' ';
		z++;
		if(m > 15)
			m = 0;
		hh[z]=hexd[m];
		if((m1 % 4) == 0)
		{
			z++;
			hh[z] =' ';
		}
		m++;
		z++;
	}
	hh[z]=0;
	strcat_s(hh,cc," ZX81");
	for (int f = 0; f < columns-3; f++)
	{
		strcat_s(hh, cc, " ");
	}
	strcat_s(hh,cc,"ASCII\r\n");
	return hh;
}
void doHexDump(char* zxrem,size_t cczxrem,BYTE* in,int ccin,int columns, WORD address)
{	
	int cb = ccin;
	size_t cchexdump = optionSizeHD(ccin) + (columns/16+1)*HEADERSIZE_HEXDUMP;
	char* hexdump = new char[cchexdump]; *hexdump = 0;

	strcat_s(hexdump, cchexdump, mcIndent[0]);
	strcat_s(hexdump, cchexdump, "Length: ");
	char s[10];
	sprintf_s(s,"%d",cb);
	strcat_s(hexdump, cchexdump,s);
	strcat_s(hexdump, cchexdump," bytes");
	strcat_s(hexdump, cchexdump," (" );
	sprintf_s(s,"%04X",cb);
	strcat_s(hexdump, cchexdump,s);
	strcat_s(hexdump, cchexdump,"h bytes) Location:");
	char s1[10];
	sprintf_s(s1,"%u",address + offset);
	strcat_s(hexdump, cchexdump,s1);
	strcat_s(hexdump, cchexdump,"\r\n");
	
	char* hh = HexHeader2(address + offset, columns);
	strcat_s(hexdump, cchexdump, hh);
	delete hh;
	
	int fx = strlen(hexdump); 
	assert((int)strlen(hexdump) <= (columns / 16 + 1) * HEADERSIZE_HEXDUMP);
	BOOL bShowStatus = TRUE;
	char* ss = zxhexdumpX(in, cb, mcIndent[0],columns,address + offset,bShowStatus);
	assert(strlen(ss)<cchexdump);
	strcat_s(hexdump, cchexdump, ss);
	delete ss;
	
	strcat_s(zxrem, cczxrem, hexdump);
	delete hexdump;
}
BOOL IsZXstandard(BYTE byte)
{
	if ((byte < 64) || (((byte >= 128) && (byte <= 191))))
		return TRUE;
	return FALSE;
}
char* zxhexdumpX(BYTE* bs, int cbbs, char* spacer, int columns, WORD address, BOOL bShowStatus)
{
	size_t cczxhexdump = 20 + optionSizeHD(cbbs);
	char* szHexDump = new char[cczxhexdump]; *szHexDump = 0;
	char chex[10]; *chex = 0;
	char hx_line[4 * (8 + 7 + 16 + 4)]; *hx_line = 0;
	char a[33 + 2];
	char ts[33 + 2];
	char hx[3];
	BYTE d;
	int addcounter = 0;
	while (addcounter < cbbs)
	{
		WCHAR sz[67];
		if (bShowStatus)
		{
			_stprintf_s(sz, L"Hex Dump: %d bytes of %d", addcounter, cbbs);
			if (hWndExportName)
				SetWindowText(hWndExportName, sz);
		}
#ifdef _DEBUG
		int tx = wcslen(sz);
		tx = tx;
#endif
		* hx_line = 0;
		strcpy_s(a, " ");
		strcpy_s(ts, " ");
		sprintf_s(chex, "%04X  ", address);
		strcat_s(hx_line, spacer);
		strcat_s(hx_line, chex);
		int f;
		int m;
		for (f = 0, m = 1; f < columns; f++, m++)
		{
			//'get digit-hex for next byte  - put in hx$
			d = bs[addcounter];
			sprintf_s(hx, "%02X", d);
			strcat_s(hx_line, hx);

#			ifdef _DEBUG
			size_t st = strlen(hx_line);
#endif

			if ((m % 4) == 0)
				strcat_s(hx_line, " ");

			if (IsZXstandard(d))
				strcat_s(ts, code2ascii[d]);
			else
				strcat_s(ts, ".");

			//'get char from ASCII Char Set
			if (((d > 31) && (d < 0x7f)) || (d >= 0xA0))
			{
				char as[2];
				as[0] = d;
				as[1] = 0;
				strcat_s(a, as);
			}
			else
				strcat_s(a, ".");

			addcounter = addcounter + 1;
			address = address + 1;
			if (addcounter == cbbs)
				break;
		}
		strcat_s(szHexDump, cczxhexdump, hx_line);
		int remaining = columns - 1 - f;
		if (remaining)
		{
			int xxx = remaining / 4 + 1;
			int xSpaces = (2 * remaining) + xxx;
			for (int f = 0; f < xSpaces; f++) {
				strcat_s(szHexDump, cczxhexdump, " ");
			}
			strcat_s(szHexDump, cczxhexdump, ts);
			xSpaces = remaining;
			for (int f = 0; f < xSpaces; f++) {
				strcat_s(szHexDump, cczxhexdump, " ");
			}
		}
		else
			strcat_s(szHexDump, cczxhexdump, ts);
		strcat_s(szHexDump, cczxhexdump, a);
		strcat_s(szHexDump, cczxhexdump, "\r\n");
	}
	//int fw = strlen(szHexDump);
	assert(strlen(szHexDump) <= cczxhexdump);
	return szHexDump;
}
char* hexdump(BYTE* inputstring, size_t ccin, size_t* lpccout, char* szFileName)
{
	int columns = 16;
	size_t ccw = optionSizeHD(ccin) + (columns / 16 + 1) * HEADERSIZE_HEXDUMP;

	char* working = new char[ccw]; *working = 0;
	strcat_s(working, ccw, GetFileTitle(szFileName));
	strcat_s(working, ccw, "\r\n");
	WORD address = 0;// 16393;
	int ccsa = 5;
	char sStartAddr[5];
	GetWindowTextA(g_hwndStartAddr, sStartAddr, ccsa);
	unsigned long addr = strtoul(sStartAddr, NULL, 16);
	if ((addr > 65536) || (addr < 0))
		addr = 16393;
	address = (WORD)addr - offset;
	code2ascii = zx2ascii;
	*mcIndent[0] = 0;
	int cols = 16;
	int ccs = 3;
	char sColumns[3];
	GetWindowTextA(g_hwndColumns, sColumns, ccs);
	cols = atoi(sColumns);
	if ((cols <1) || (cols >33))
		cols = 16;
	if (address > ccin)
		address = 0;
	doHexDump(working, ccw, inputstring+address, ccin- address, cols, address);
	
	return working;
}

int findScreenSize(BYTE* bs, int ccbs)
{
	for (int columns = 33; columns > 8; columns--) //minimum width we look for is 8 columns, say a 1K program
	{
		for (int f = 0; f < ccbs; f++)
		{
			int a = bs[f];
			int b = bs[f + columns];
			int c = bs[f + columns * 2];
			int d = bs[f + columns * 3];
			if ((a == 0x76) && (b == 0x76) && (c == 0x76) && (d == 0x76)) //minimum rows we look for is 4
			{
				return columns;
			}
		}
	}
	return 33; //default, let the caller look for a default DFILE
}
void doDBZXdisplayfile(char* s, size_t ccs, BYTE* in, int ccin, int columns, int rows, WORD address)
{
	char sl[] = "L4023:";
	size_t slcc = strlen(sl)+1;
	char zxs[11]; *zxs = 0;
	size_t zxscc = 11;
	int d;
	int z = 1;
	for (int b = 0; b < rows; b++)
	{
		if(b>1)
			strcat_s(s, ccs, mcIndent[1]);
		else {
			sprintf_s(sl, slcc, "L%04X:", address+offset+b*columns);
			strcat_s(s, ccs, sl);
		}
		strcat_s(s, ccs, "dbzx ");
		strcat_s(s, ccs, "'");
		for (int c = 0; c < columns; c++)
		{
			d = in[z++];
			strcpy_s(zxs, zxscc, zx2ascii[d]);
			if (strlen(zxs) == 1)
				strcat_s(s, ccs, zxs);
			else if (d==0x76)
				strcat_s(s, ccs, "',13");
			else
				strcat_s(s, ccs, "^"); //marker not mapped to ZXDB for non-printable character
		}
		if(d!=0x76)
			strcat_s(s, ccs, "'");
		strcat_s(s, ccs, "\r\n");
	}
}
char* ShowAlternateDisplayFiles(char* is, int ccis)
{
	int ccs = 73900;
	char* s = new char[ccs]; *s = 0;

	BYTE* bs = (BYTE*)is;
	WORD D_FILE = GetAddress(16396 - offset, bs);
	WORD DFILE_ADDR = GetAddress(16396 - offset, bs);
	if (VERSN == 0xFF) {
		D_FILE = 16509 - offset;
		DFILE_ADDR = 16509;
	}
	WORD PROGRAM_ = 16509;
	if (VERSN == 0xFF)
		PROGRAM_ = 17302;

	int  lBound = PROGRAM_ - offset;
	int uBound = ccis;
	if (VERSN == 0x00)
		uBound = DFILE_ADDR - offset;

	int columns = findScreenSize(bs + lBound, uBound - lBound);
	int size_mydfile = columns * 33 + 1;

	for (int f = lBound; f < uBound; f++)
	{
		int a = is[f];
		int b = is[f + columns];
		int c = is[f + columns * 2];
		if ((a == 0x76) && (b == 0x76) && (c == 0x76))
		{
			if (*s)
				strcat_s(s, ccs, "\r\n");
			char ss[] = "      // 439B  76 possible display file found\r\n";
			int ccss = strlen(ss) + 1;
			sprintf_s(ss, ccss, "%s%04X  76 possible display file found\r\n", mcIndent, f + offset);
			strcat_s(s, ccs, ss);

			int myrows = 24;
			for (int rows = 0; rows < 24; rows++) //find rows
			{
				int d = is[f + columns * rows];
				if (d != 0x76)
				{
					size_mydfile = columns * rows + 1;
					myrows = rows;
					break;
				}
			}
			doDBZXdisplayfile(s, ccs, bs + f - columns, size_mydfile + 1 * columns + 1, columns, myrows + 1, f - columns + 1);
			WORD myadd = f + offset;
			f += 24 * columns;// size_mydfile;
			myadd = f + offset;
			f = f;
		}
	}
	if (*s == 0)
		strcpy_s(s, ccs, "no alternate display files found");
	return s;
}
