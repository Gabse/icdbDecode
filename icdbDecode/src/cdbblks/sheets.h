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
#ifndef _SHEETS_H
#define _SHEETS_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>			// Required for int32_t, uint32_t, ...
#include "cdbblks.h" 		// Required for coordinate struct

/*
******************************************************************
* Enums
******************************************************************
*/
#if C23
typedef enum sheetsize_type : int32_t
{
#else
typedef enum sheetsize_type
{
	sheetsize_hack = (int32_t)(-2147483647), // Hack to force size to 32bit signed
#endif
	sheetsize_A				= 0,
	sheetsize_B				= 1,
	sheetsize_C				= 2,
	sheetsize_D				= 3,
	sheetsize_E				= 4,
	sheetsize_A4			= 5,
	sheetsize_A3			= 6,
	sheetsize_A2			= 7,
	sheetsize_A1			= 8,
	sheetsize_A0			= 9,
	sheetsize_Custom		= 10,
	sheetsize_F				= 11, // F is actually after Custom and not between E and A4
}sheetsize_type;

typedef enum sheetorientation_type
{
	sheetorientation_Landscape	= 0,
	sheetorientation_Portrait	= 1,
	sheetorientation_Custom		= 2,
}sheetorientation_type;

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct sheet_struct
{
	coordinate_struct Size;
	sheetsize_type SizeCode;
	sheetorientation_type Orientation;
	int Group;
} sheet_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessSheets(void);
extern void InitSheets(void);
extern unsigned int GetNumSheets(void);
extern sheet_struct GetSheet(int);

#endif //_SHEETS_H