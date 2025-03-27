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
#ifndef _CATLGATL_H
#define _CATLGATL_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "common.h"		// Required for text_struct

/*
******************************************************************
* Structures
******************************************************************
*/
typedef struct uid_struct
{
	uint8_t UID[8];
} uid_struct;

/*
******************************************************************
* Global Variables
******************************************************************
*/
extern int32_t NumBlkUID;
extern uid_struct* BlkUID;

extern int32_t NumGroupOwnerUID;
extern uid_struct* GroupOwnerUID;

extern int32_t NumMdlNam;
extern text_struct* MdlNam;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern int parseCatlgatl(char*, uint32_t);

#endif //_CATLGATL_H