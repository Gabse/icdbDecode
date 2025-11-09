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
#ifndef _GRPATL_H
#define _GRPATL_H

/*
******************************************************************
* Global Includes
******************************************************************
*/
#include <stdio.h>		// Required for FILE
#include "../common.h"	// Required for key_struct

/*
******************************************************************
* Global Defines
******************************************************************
*/
#define PATH_GRPATL "grpatl.v"

/*
******************************************************************
* Global Variables
******************************************************************
*/
// Grpatl
extern key_struct* Grpatl_Fixes;
extern key_struct* Grpatl_Group2Prps;
extern key_struct* Grpatl_GroupContentsType;
extern key_struct* Grpatl_GroupObjects;
extern key_struct* Grpatl_GroupSubGroups;
extern key_struct* Grpatl_GrpVersion;
extern key_struct* Grpatl_PrpAttr;
extern key_struct* Grpatl_PrpBit;
extern key_struct* Grpatl_PrpId;
extern key_struct* Grpatl_PrpNam;
extern key_struct* Grpatl_PrpOrder;
extern key_struct* Grpatl_PrpStr;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessKeyGrpatl(FILE*, char*, unsigned int);
extern void InitGrpatl(void);

#endif //_GRPATL_H