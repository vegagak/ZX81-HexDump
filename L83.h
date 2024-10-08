#pragma once
#include "zx81s.h"

zx81vars L83vars[] = {
	1,"VERSN    ",0,
	2,"NXTLIN   ",0,
	2,"PROGRAM  ",16396,
	2,"DF_CC    ",0,
	2,"VARS     ",0,
	2,"DEST     ",16402,
	2,"E_LINE   ",0,
	2,"CH_ADD   ",0,
	2,"X_PTR    ",16408,
	2,"STKBOT   ",0,
	2,"STKEND   ",0,
	1,"BERG     ",16414,
	2,"MEM      ",0,
	1,"MUNIT    ",16417,
	1,"DF_SZ    ",16418,
	2,"S_TOP    ",16419,
	2,"LAST_K   ",16421,
	1,"DEBOUNCE ",16423,
	1,"MARGIN   ",0,
	2,"E_PPC    ",16425,
	2,"OLDPPC   ",16427,
	1,"FLAGX    ",16429,
	2,"STRLEN   ",16430,
	2,"T_ADDR   ",16432,
	2,"SEED     ",16434,
	2,"FRAMES   ",16436,
	1,"COORDS   ",16438,
	1,"         ",16439,
	1,"PR_CC    ",16440,
	1,"S_POSN   ",16441,
	1,"         ",16442,
	1,"CDFLAG   ",0,
   33,"PRBUFF   ",16444,
   25,"MEMBOT   ",16477,
	5,"MEM_5    ",16502,
	2,"BLINK    ",16507,
  255,0,0
};

char L83toAscii[][11] = {
" ",
"�",//\241", //"\xA1"
"�",//\242", //"\xA2"
"�",//\246", //"\xA6"
"�",//\244", //"\xA4"
"�",//\254", //"\xAC"
"�",//\262", //"\xB2"
"�",//\271", //"\xB9" //08-0F Lamba differs from ZX81
"�",//\272", //"\xBA"
"�",//\276", //"\xBE"
"�",//\275", //"\xBD"
"\"",
"�",
"$",
":",
"?",                  // 0F end of BLOCK 1 
"(",
")",
">",
"<",
"=",
"+",
"-",
"*",
"/",
";",
",",
".",
"0",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"A",
"B",
"C",
"D",
"E",
"F",
"G",
"H",
"I",
"J",
"K",
"L",
"M",
"N",
"O",
"P",
"Q",
"R",
"S",
"T",
"U",
"V",
"W",
"X",
"Y",
"Z",
" THEN ",                //40 Lamba differs from ZX81
" TO ",                  //41 Lamba differs from ZX81
" STEP ",                //42 Lamba differs from ZX81
"RND", //"\x43",         //43 Lamba differs from ZX81
"INKEY$", //"\x44",      //44 Lamba differs from ZX81  
"PI", //"\x45",          //45 Lamba differs from ZX81
" INK ", //"\x46",       //46 Lamba differs from ZX81
" PAPER ", //"\x47",     //47 Lamba differs from ZX81
" BORDER ", //"\x48",    //48 Lamba differs from ZX81
"\111", //"\x49",
"\112", //"\x4A",
"\113", //"\x4B",
"\114", //"\x4C",
"\115", //"\x4D",
"\116", //"\x4E",
"\117", //"\x4F",
"\120", //"\x50",
"\121", //"\x51",
"\122", //"\x52",
"\123", //"\x53",
"\124", //"\x54",
"\125", //"\x55",
"\126", //"\x56",
"\127", //"\x57",
"\130", //"\x58",
"\131", //"\x59",
"\132", //"\x5A",
"\133", //"\x5B",
"\134", //"\x5C",
"\135", //"\x5D",
"\136", //"\x5E",
"\137", //"\x5F",
"\140", //"\x60",
"\141", //"\x61",
"\142", //"\x62",
"\143", //"\x63",
"\144", //"\x64",
"\145", //"\x65",
"\146", //"\x66",
"\147", //"\x67",
"\150", //"\x68",
"\151", //"\x69",
"\152", //"\x6A",
"\153", //"\x6B",
"\154", //"\x6C",
"\155", //"\x6D",
"\156", //"\x6E",
"\157", //"\x6F",
"\160", //"\x70",
"\161", //"\x71",
"\162", //"\x72",
"\163", //"\x73",
"\164", //"\x74",
"\165", //"\x75",
"",//\r
"\167", //"\x77",
"\170", //"\x78",
"\171", //"\x79",
"\172", //"\x7A",
"\173", //"\x7B",
"\174", //"\x7C",
"\175", //"\x7D",
"\176", //"\x7E",
"\177", //"\x7F",
"�",//\261", //"\xB1",
"�",//\265", //"\xB5",
"�",//\266", //"\xB6",
"�",//\252", //"\xAA",
"�",//\270", //"\xB8",
"�",//\257", //"\xAF",
"�",//\263", //"\xB3",
"�",//\270", //"\xB8",
"�",//\274", //"\xBC",
"�",//\301", //"\xC1",
"�",//\300", //"\xC0",
"\302", //"\xC2", //"
"\303", //"\xC3", //�",
"\305", //"\xC5", //$",
"\306", //"\xC6", //:",
"\307", //"\xC7", //?",
"\310", //"\xC8", //(",
"\311", //"\xC9", //)",
"\312", //"\xCA", //>",
"\313", //"\xCB", //<",
"\314", //"\xCC", //=",
"\315", //"\xCD", //+",
"\316", //"\xCE", //-",
"\317", //"\xCF", //*",
"\320", //"\xD0", //",
"\321", //"\xD1", //;",
"\322", //"\xD2", //,",
"\323", //"\xD3", //.",
"\324", //"\xD4", //0",
"\325", //"\xD5", //1",
"\327", //"\xD7", //2",
"\330", //"\xD8", //3",
"\331", //"\xD9", //4",
"\332", //"\xDA", //5",
"\333", //"\xDB", //6",
"\335", //"\xDD", //7",
"\336", //"\xDE)", //8",
"\340", //"\xE0", //9",
"\341", //"\xE1", //"a",
"\342", //"\xE2", //"b",
"\343", //"\xE3", //"c",
"\345", //"\xE5", //"d",
"\346", //"\xE6", //"e",
"\347", //"\xE7", //"f",
"\350", //"\xE8", //"g",
"\351", //"\xE9", //"h",
"\352", //"\xEA", //"i",
"\353", //"\xEB", //"j",
"\354", //"\xEC", //"k",
"\355", //"\xED", //"l",
"\356", //"\xEE", //"m",
"\357", //"\xEF", //"n",
"\360", //"\xF0", //"o",
"\361", //"\xF1", //"p",
"\362", //"\xF2", //"q",
"\363", //"\xF3", //"r",
"\364", //"\xF4", //"s",
"\365", //"\xF5", //"t",
"\367", //"\xF7", //"u",
"\370", //"\xF8", //"v",
"\371", //"\xF9", //"w",
"\372", //"\xFA", //"x",
"\373", //"\xFB", //"y",
"\375", //"\xFD", //"z",
"CODE ",              //C0 Lamba differs from ZX81
"VAL ",               //C1 Lamba differs from ZX81
"LEN ",               //C2 Lamba differs from ZX81
"SIN ",
"COS ",
"TAN ",
"ASN ",
"ACS ",
"ATN ",
"LN ",
"EXP ",
"INT ",
"SQR ",
"SGN ",
"ABS ",
"PEEK ",
"USR ",
"STR$ ",
"CHR$ ",
"NOT ",
"AT ",
"TAB ",
"**",
"OR ",
"AND ",
"<=",
">=",
"<>",
"TEMPO ",
"MUSIC ",
"SOUND ",
"BEEP ",
"NOBEEP ",           //BLOCK 3 END
"LPRINT ",
"LLIST ",
"STOP ",
"SLOW ",
"FAST ",
"NEW ",
"SCROLL ",
"CONT ",
"DIM ",
"REM ",
"FOR ",
"GOTO ",
"GOSUB ",
"INPUT ",
"LOAD ",
"LIST ",
"LET ",
"PAUSE ",
"NEXT ",
"POKE ",
"PRINT ",
"PLOT ",
"RUN ",
"SAVE ",
"RAND ",
"IF ",
"CLS ",
"UNPLOT ",
"CLEAR ",
"RETURN ",
"COPY "};

charcodes L83charcodes[]{
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
2, "ZX_QUOTE",//$0B
2, "%",//12 Pound character
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
1, "ZX_ASTER",//$17 ASTERISK
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
2, "L83_THEN",//$40
2, "L83_TO",//$41
2, "L83_STEP",
2, "L83_RND",
2, "L83_INKEY",
2, "L83_PI",
2, "L8_INK",
2, "L83_PAPER",
2, "L83_BORDER",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
2, "ZX_NEWLINE",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
2, "ZX_NUMBER",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
0, "",
2, "L83_CODE",//$C0 192
2, "L83_VAL",
2, "L83_LEN",
2, "L83_SIN",
2, "L83_COS",
2, "L83_TAN",
2, "L83_ASN",
2, "L83_ACS",
2, "L83_ATN",
2, "L83_LOG",
2, "L83_EXP",
2, "L83_INT",
2, "L83_SQR",
2, "L83_SGN",
2, "L83_ABS",
2, "L83_PEEK",
2, "L83_USR",
2, "L83_STR",
2, "L83_CHR",
2, "L83_NOT",
2, "L83_AT",
2, "L83_TAB",
2, "L83_POWER",
2, "L83_OR",
2, "L83_AND",//$DD 216
2, "L83_LTE",
2, "L83_GTE",
2, "L83_NOTEQU",
2, "L83_TEMPO",
2, "L83_MUSIC",
2, "L83_SOUND",
2, "L83_BEEP",
2, "L83_NOBEEP",
0, "ZX_LPRINT",
0, "ZX_LLIST",
0, "ZX_STOP",
0, "ZX_SLOW",
0, "ZX_FAST",
0, "ZX_NEW",
0, "ZX_SCROLL",
0, "ZX_CONT",
0, "ZX_DIM",
0, "ZX_REM",//$EA
0, "ZX_FOR",
0, "ZX_GOTO",
0, "ZX_GOSUB",
0, "ZX_INPUT",
0, "ZX_LOAD",
0, "ZX_LIST",
0, "ZX_LET",
0, "ZX_PAUSE",
0, "ZX_NEXT",
0, "ZX_POKE",
0, "ZX_PRINT",
0, "ZX_PLOT",
0, "ZX_RUN",
0, "ZX_SAVE",
0, "ZX_RAND",
0, "ZX_IF",
0, "ZX_CLS",
0, "ZX_UNPLOT",
0, "ZX_CLEAR",
0, "ZX_RETURN",
0, "ZX_COPY"};