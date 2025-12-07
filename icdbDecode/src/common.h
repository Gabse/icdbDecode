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
* No static or dynamic code analysis of any proprietary executable files was used to gain information about the file format.
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
#include <stdlib.h>		// Required for min/max

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define DIR_SEPARATOR_WINDOWS '\\'
#define DIR_SEPARATOR_UNIX '/'

#ifdef WIN32 // Building for Windows
	#define  DIR_SEPARATOR DIR_SEPARATOR_WINDOWS
#else // Building for Unix
	#define  DIR_SEPARATOR DIR_SEPARATOR_UNIX
#endif

#if __x86_64__
	#define B64Bit
# endif

#ifndef max
#define max(a,b) \
	({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a > _b ? _a : _b; })
#endif

#ifndef min
#define min(a,b) \
	({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a < _b ? _a : _b; })
#endif

/*
******************************************************************
* Unions
******************************************************************
*/
typedef union CharData
{
	uint8_t u8[1];
	int8_t i8[1];
} CharData;

typedef union WordData
{
	uint8_t u8[2];
	int8_t i8[2];
	uint16_t u16[1];
	int16_t i16[1];
} WordData;

typedef union IntData
{
	uint8_t u8[4];
	int8_t i8[4];
	uint16_t u16[2];
	int16_t i16[2];
	uint32_t u32[1];
	int32_t i32[1];
} IntData;

typedef union LongData
{
	uint8_t u8[8];
	int8_t i8[8];
	uint16_t u16[4];
	int16_t i16[4];
	uint32_t u32[2];
	int32_t i32[2];
#ifdef B64Bit
	uint64_t u64[1];
	int64_t i64[1];
#endif
} LongData;

typedef union TimeData
{
	uint8_t u8[12];
	int8_t i8[12];
	uint16_t u16[6];
	int16_t i16[6];
	uint32_t u32[3];
	int32_t i32[3];
} TimeData;

typedef union LongLongData
{
	uint8_t u8[16];
	int8_t i8[16];
	uint16_t u16[8];
	int16_t i16[8];
	uint32_t u32[4];
	int32_t i32[4];
#ifdef B64Bit
	uint64_t u64[2];
	int64_t i64[2];
#endif
} LongLongData;

/*
******************************************************************
* Enums
******************************************************************
*/
typedef enum typecode_enum
{
	typecode_String = 0x01,	// uint length + char array
	typecode_IntArray = 0x02, // uint length + (u)int array
	typecode_UID = 0x03,	// Long (64bits)
	typecode_SUID = 0x04,	// LongLong (128bits)
	typecode_Int = 0x05, // Int (32Bit)
	typecode_Long = 0x06, // Long (64bits)
	typecode_Time = 0x07, // Longint (96bits)
	typecode_LongLong = 0x08, // LongLong (128bits)
	typecode_GUID = 0x0D, // Unknown
}typecode_enum;

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct num_struct
{
	int32_t Integ;
	uint32_t Frac;
} num_struct;

typedef struct string_struct
{
	uint32_t Length;
	char* Text;
} string_struct;

typedef struct int_array_struct
{
	uint32_t Length;
	IntData* Data;
} int_array_struct;

typedef struct key_struct
{
	typecode_enum Typecode;
	int Length;
	int LengthCalc;
	void* Data;
} key_struct;

typedef struct element_struct
{
	uint32_t Length;
	void* Data;
} element_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern FILE* myfopen(char*, char*, uint32_t, char*, uint32_t, char);
extern void myPrint(const char*, ...);
extern char swpnib(char);
extern int parseFile(char*, uint32_t, char*, uint32_t, void(*CheckKey)(FILE*, char*,  unsigned int));
extern void SkipBlock(FILE*, uint32_t);
extern num_struct numProcess(int32_t, int32_t, int32_t);
extern void InitString(int32_t, string_struct**);
extern void InitRegular(int32_t, void**);
extern key_struct* ParseKey(FILE*);
extern void InitKey(key_struct**);
string_struct CopyString(string_struct);


#endif //_COMMON_H