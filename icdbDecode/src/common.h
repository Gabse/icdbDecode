/*
******************************************************************
* Info
******************************************************************
* iCDBdecode
*
* This tool can be used to analyze and decompress Siemens EDA (former Mentor Graphics) icdb.dat files.
* It's intend is to gain understanding of the file format, in order to allow interoperability with other EDA packages.
*
* The tool is based on initial research done by Patrick Yeon (https://github.com/patrickyeon/icdb2fs) in 2011.
* The research was performed by analyzing various icdb.dat files (basically staring at the hex editor for hours),
* No static or dynamic code analysis of any proprietary executable files was used to gain information’s about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/
#ifndef _COMMON_H
#define _COMMON_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdio.h>		// Required for file type

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define DIR_SEPERATOR_WINDOWS '\\'
#define DIR_SEPERATOR_UNIX '/'

#ifdef WIN32 // Building for Windows
	#define  DIR_SEPERATOR DIR_SEPERATOR_WINDOWS
#else // Building for Unix
	#define  DIR_SEPERATOR DIR_SEPERATOR_UNIX
#endif

/*
******************************************************************
* Global Structures
******************************************************************
*/
typedef struct num_struct
{
	int32_t Integ;
	uint32_t Frac;
} num_struct;

typedef struct text_struct
{
	uint32_t Lenth;
	char* Text;
} text_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern FILE* myfopen(char*, char*, uint32_t, char*, uint32_t, char);
extern void myPrint(const char*, ...);
extern int parseFile(char*, uint32_t, char*, uint32_t, void(*CheckKey)(FILE*, char*));
extern void SkipBlock(FILE*, uint32_t);
extern void* Parse(FILE*, int32_t*, uint32_t, uint32_t, char*);
extern text_struct* ParseString(FILE*, int32_t*, char*);
extern num_struct numProcess(int32_t, int32_t, int32_t);
extern void PrintKey(FILE*, char*);
extern void InitString(int32_t, text_struct**);
extern void InitRegular(int32_t, void**);
extern uint8_t IsInsideFile(FILE*);

#endif //_COMMON_H