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
#ifndef _BLKATL_H
#define _BLKATL_H

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
#define PATH_BLKATL "blkatl.v"

/*
******************************************************************
* Global Variables
******************************************************************
*/
// Blkatl
extern key_struct* Blkatl_BNetFlg;
extern key_struct* Blkatl_BPin2Nets;
extern key_struct* Blkatl_BPin2Prps;
extern key_struct* Blkatl_BPinTyp;
extern key_struct* Blkatl_BPinUID;
extern key_struct* Blkatl_BPins2Pins;
extern key_struct* Blkatl_BSym2BPins;
extern key_struct* Blkatl_BSym2CMatrix;
extern key_struct* Blkatl_BSym2CMatrixes;
extern key_struct* Blkatl_BSym2Prps;
extern key_struct* Blkatl_BSym2Sub;
extern key_struct* Blkatl_BSym2Xtrs;
extern key_struct* Blkatl_BSymArrayed;
extern key_struct* Blkatl_BSymFlg;
extern key_struct* Blkatl_BSymForwardPCB;
extern key_struct* Blkatl_BSymRef;
extern key_struct* Blkatl_BSymUID;
extern key_struct* Blkatl_BlkPrps;
extern key_struct* Blkatl_Bus2Nets;
extern key_struct* Blkatl_Bus2Prps;
extern key_struct* Blkatl_BusNam;
extern key_struct* Blkatl_BusParent;
extern key_struct* Blkatl_BusType;
extern key_struct* Blkatl_BusUID;
extern key_struct* Blkatl_CMatrixName;
extern key_struct* Blkatl_CMatrixPairs;
extern key_struct* Blkatl_CMatrixUID;
extern key_struct* Blkatl_Connection2CMatrix;
extern key_struct* Blkatl_ConnectionBit1;
extern key_struct* Blkatl_ConnectionBit2;
extern key_struct* Blkatl_ConnectionPin1;
extern key_struct* Blkatl_ConnectionPin2;
extern key_struct* Blkatl_DiffPrs;
extern key_struct* Blkatl_Fixes;
extern key_struct* Blkatl_GrpPrp2Prp;
extern key_struct* Blkatl_Net2BNet;
extern key_struct* Blkatl_Net2Prps;
extern key_struct* Blkatl_NetNam;
extern key_struct* Blkatl_PNetUID;
extern key_struct* Blkatl_Prp2AssocObj;
extern key_struct* Blkatl_PrpAttr;
extern key_struct* Blkatl_PrpBit;
extern key_struct* Blkatl_PrpId;
extern key_struct* Blkatl_PrpNam;
extern key_struct* Blkatl_PrpOrder;
extern key_struct* Blkatl_PrpStr;
extern key_struct* Blkatl_PrpUID;
extern key_struct* Blkatl_Sym2Cmp;
extern key_struct* Blkatl_Version;
extern key_struct* Blkatl_Xtr2BPin;
extern key_struct* Blkatl_Xtr2Prps;
extern key_struct* Blkatl_XtrBPin;
extern key_struct* Blkatl_XtrNam;
extern key_struct* Blkatl_XtrTyp;
extern key_struct* Blkatl_XtrUID;

/*
******************************************************************
* Global Functions
******************************************************************
*/
extern void ProcessKeyBlkatl(FILE*, char*, unsigned int);
extern void InitBlkatl(void);

#endif //_BLKATL_H