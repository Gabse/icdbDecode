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
#ifndef _CMPATL_H
#define _CMPATL_H

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
#define PATH_CMPATL "cmpatl.v"

/*
******************************************************************
* Global Variables
******************************************************************
*/
// Cmpatl
extern key_struct* Cmpatl_CmpPins;
extern key_struct* Cmpatl_CmpPrps;
extern key_struct* Cmpatl_Fixes;
extern key_struct* Cmpatl_Pin2Prps;
extern key_struct* Cmpatl_PinArrayWidth;
extern key_struct* Cmpatl_PinLibUID;
extern key_struct* Cmpatl_PinNam;
extern key_struct* Cmpatl_PinTyp;
extern key_struct* Cmpatl_PinUID;
extern key_struct* Cmpatl_PrpAttr;
extern key_struct* Cmpatl_PrpBit;
extern key_struct* Cmpatl_PrpId;
extern key_struct* Cmpatl_PrpNam;
extern key_struct* Cmpatl_PrpOrder;
extern key_struct* Cmpatl_PrpStr;
extern key_struct* Cmpatl_PrpUID;
extern key_struct* Cmpatl_Version;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessKeyCmpatl(FILE*, char*, unsigned int);
extern void InitCmpatl(void);

#endif //_CMPATL_H