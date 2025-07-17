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
#ifndef _CDBBLKS_H
#define _CDBBLKS_H

/*
******************************************************************
* Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "../common.h"	// Required for string_struct

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct coordinate_struct
{
	int32_t X;
	int32_t Y;
} coordinate_struct;

typedef struct color_struct
{
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
	uint8_t Key;
} color_struct;

/*
******************************************************************
* Global Functions
******************************************************************
*/
int parseCdbblks(char*, uint32_t);
void initCdbblks(void);

#endif //_CDBBLKS_H