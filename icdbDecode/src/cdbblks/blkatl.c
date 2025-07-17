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

/*
******************************************************************
* Includes
******************************************************************
*/
#include "blkatl.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <string.h>					// Required for strcmp
#include "../common.h"				// Required for parseFile

/*
******************************************************************
* Global Variables
******************************************************************
*/
key_struct* Blkatl_BNetFlg = NULL;
key_struct* Blkatl_BPin2Nets = NULL;
key_struct* Blkatl_BPin2Prps = NULL;
key_struct* Blkatl_BPinTyp = NULL;
key_struct* Blkatl_BPinUID = NULL;
key_struct* Blkatl_BPins2Pins = NULL;
key_struct* Blkatl_BSym2BPins = NULL;
key_struct* Blkatl_BSym2CMatrix = NULL;
key_struct* Blkatl_BSym2CMatrixes = NULL;
key_struct* Blkatl_BSym2Prps = NULL;
key_struct* Blkatl_BSym2Sub = NULL;
key_struct* Blkatl_BSym2Xtrs = NULL;
key_struct* Blkatl_BSymArrayed = NULL;
key_struct* Blkatl_BSymFlg = NULL;
key_struct* Blkatl_BSymForwardPCB = NULL;
key_struct* Blkatl_BSymRef = NULL;
key_struct* Blkatl_BSymUID = NULL;
key_struct* Blkatl_BlkPrps = NULL;
key_struct* Blkatl_Bus2Nets = NULL;
key_struct* Blkatl_Bus2Prps = NULL;
key_struct* Blkatl_BusNam = NULL;
key_struct* Blkatl_BusParent = NULL;
key_struct* Blkatl_BusType = NULL;
key_struct* Blkatl_BusUID = NULL;
key_struct* Blkatl_CMatrixName = NULL;
key_struct* Blkatl_CMatrixPairs = NULL;
key_struct* Blkatl_CMatrixUID = NULL;
key_struct* Blkatl_Connection2CMatrix = NULL;
key_struct* Blkatl_ConnectionBit1 = NULL;
key_struct* Blkatl_ConnectionBit2 = NULL;
key_struct* Blkatl_ConnectionPin1 = NULL;
key_struct* Blkatl_ConnectionPin2 = NULL;
key_struct* Blkatl_DiffPrs = NULL;
key_struct* Blkatl_Fixes = NULL;
key_struct* Blkatl_GrpPrp2Prp = NULL;
key_struct* Blkatl_Net2BNet = NULL;
key_struct* Blkatl_Net2Prps = NULL;
key_struct* Blkatl_NetNam = NULL;
key_struct* Blkatl_PNetUID = NULL;
key_struct* Blkatl_Prp2AssocObj = NULL;
key_struct* Blkatl_PrpAttr = NULL;
key_struct* Blkatl_PrpBit = NULL;
key_struct* Blkatl_PrpId = NULL;
key_struct* Blkatl_PrpNam = NULL;
key_struct* Blkatl_PrpOrder = NULL;
key_struct* Blkatl_PrpStr = NULL;
key_struct* Blkatl_PrpUID = NULL;
key_struct* Blkatl_Sym2Cmp = NULL;
key_struct* Blkatl_Version = NULL;
key_struct* Blkatl_Xtr2BPin = NULL;
key_struct* Blkatl_Xtr2Prps = NULL;
key_struct* Blkatl_XtrBPin = NULL;
key_struct* Blkatl_XtrNam = NULL;
key_struct* Blkatl_XtrTyp = NULL;
key_struct* Blkatl_XtrUID = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessKeyBlkatl()
*
* - description: 	Check for blkatl keys
*
* - parameter: 		file pointer; Key to check; length of key
*
* - return value: 	-
******************************************************************
*/
void ProcessKeyBlkatl(FILE * sourceFile, char* Key, unsigned int KeyLen)
{
	if (strcmp(Key, "BNetFlg") == 0)
	{
		Blkatl_BNetFlg = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BPin2Nets") == 0)
	{
		Blkatl_BPin2Nets = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BPin2Prps") == 0)
	{
		Blkatl_BPin2Prps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BPinTyp") == 0)
	{
		Blkatl_BPinTyp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BPinUID") == 0)
	{
		Blkatl_BPinUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BPins2Pins") == 0)
	{
		Blkatl_BPins2Pins = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSym2BPins") == 0)
	{
		Blkatl_BSym2BPins = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSym2CMatrix") == 0)
	{
		Blkatl_BSym2CMatrix = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSym2CMatrixes") == 0)
	{
		Blkatl_BSym2CMatrixes = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSym2Prps") == 0)
	{
		Blkatl_BSym2Prps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSym2Sub") == 0)
	{
		Blkatl_BSym2Sub = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSym2Xtrs") == 0)
	{
		Blkatl_BSym2Xtrs = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSymArrayed") == 0)
	{
		Blkatl_BSymArrayed = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSymFlg") == 0)
	{
		Blkatl_BSymFlg = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSymForwardPCB") == 0)
	{
		Blkatl_BSymForwardPCB = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSymRef") == 0)
	{
		Blkatl_BSymRef = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BSymUID") == 0)
	{
		Blkatl_BSymUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BlkPrps") == 0)
	{
		Blkatl_BlkPrps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Bus2Nets") == 0)
	{
		Blkatl_Bus2Nets = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Bus2Prps") == 0)
	{
		Blkatl_Bus2Prps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BusNam") == 0)
	{
		Blkatl_BusNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BusParent") == 0)
	{
		Blkatl_BusParent = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BusType") == 0)
	{
		Blkatl_BusType = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "BusUID") == 0)
	{
		Blkatl_BusUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CMatrixName") == 0)
	{
		Blkatl_CMatrixName = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CMatrixPairs") == 0)
	{
		Blkatl_CMatrixPairs = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CMatrixUID") == 0)
	{
		Blkatl_CMatrixUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Connection2CMatrix") == 0)
	{
		Blkatl_Connection2CMatrix = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ConnectionBit1") == 0)
	{
		Blkatl_ConnectionBit1 = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ConnectionBit2") == 0)
	{
		Blkatl_ConnectionBit2 = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ConnectionPin1") == 0)
	{
		Blkatl_ConnectionPin1 = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "ConnectionPin2") == 0)
	{
		Blkatl_ConnectionPin2 = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "DiffPrs") == 0)
	{
		Blkatl_DiffPrs = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Fixes") == 0)
	{
		Blkatl_Fixes = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GrpPrp2Prp") == 0)
	{
		Blkatl_GrpPrp2Prp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Net2BNet") == 0)
	{
		Blkatl_Net2BNet = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Net2Prps") == 0)
	{
		Blkatl_Net2Prps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "NetNam") == 0)
	{
		Blkatl_NetNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PNetUID") == 0)
	{
		Blkatl_PNetUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Prp2AssocObj") == 0)
	{
		Blkatl_Prp2AssocObj = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpAttr") == 0)
	{
		Blkatl_PrpAttr = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpBit") == 0)
	{
		Blkatl_PrpBit = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpId") == 0)
	{
		Blkatl_PrpId = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpNam") == 0)
	{
		Blkatl_PrpNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpOrder") == 0)
	{
		Blkatl_PrpOrder = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpStr") == 0)
	{
		Blkatl_PrpStr = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpUID") == 0)
	{
		Blkatl_PrpUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Sym2Cmp") == 0)
	{
		Blkatl_Sym2Cmp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Version") == 0)
	{
		Blkatl_Version = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Xtr2BPin") == 0)
	{
		Blkatl_Xtr2BPin = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Xtr2Prps") == 0)
	{
		Blkatl_Xtr2Prps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "XtrBPin") == 0)
	{
		Blkatl_XtrBPin = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "XtrNam") == 0)
	{
		Blkatl_XtrNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "XtrTyp") == 0)
	{
		Blkatl_XtrTyp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "XtrUID") == 0)
	{
		Blkatl_XtrUID = ParseKey(sourceFile);
	}
	else 
	{
		myPrint("Unknown Key in blkatl [%s]\n", Key);
	}
}

/*
******************************************************************
* - function name:	InitBlkatl()
*
* - description: 	Resets all blkatl data
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitBlkatl(void)
{
	InitKey(&Blkatl_BNetFlg);
	InitKey(&Blkatl_BPin2Nets);
	InitKey(&Blkatl_BPin2Prps);
	InitKey(&Blkatl_BPinTyp);
	InitKey(&Blkatl_BPinUID);
	InitKey(&Blkatl_BPins2Pins);
	InitKey(&Blkatl_BSym2BPins);
	InitKey(&Blkatl_BSym2CMatrix);
	InitKey(&Blkatl_BSym2CMatrixes);
	InitKey(&Blkatl_BSym2Prps);
	InitKey(&Blkatl_BSym2Sub);
	InitKey(&Blkatl_BSym2Xtrs);
	InitKey(&Blkatl_BSymArrayed);
	InitKey(&Blkatl_BSymFlg);
	InitKey(&Blkatl_BSymForwardPCB);
	InitKey(&Blkatl_BSymRef);
	InitKey(&Blkatl_BSymUID);
	InitKey(&Blkatl_BlkPrps);
	InitKey(&Blkatl_Bus2Nets);
	InitKey(&Blkatl_Bus2Prps);
	InitKey(&Blkatl_BusNam);
	InitKey(&Blkatl_BusParent);
	InitKey(&Blkatl_BusType);
	InitKey(&Blkatl_BusUID);
	InitKey(&Blkatl_CMatrixName);
	InitKey(&Blkatl_CMatrixPairs);
	InitKey(&Blkatl_CMatrixUID);
	InitKey(&Blkatl_Connection2CMatrix);
	InitKey(&Blkatl_ConnectionBit1);
	InitKey(&Blkatl_ConnectionBit2);
	InitKey(&Blkatl_ConnectionPin1);
	InitKey(&Blkatl_ConnectionPin2);
	InitKey(&Blkatl_DiffPrs);
	InitKey(&Blkatl_Fixes);
	InitKey(&Blkatl_GrpPrp2Prp);
	InitKey(&Blkatl_Net2BNet);
	InitKey(&Blkatl_Net2Prps);
	InitKey(&Blkatl_NetNam);
	InitKey(&Blkatl_PNetUID);
	InitKey(&Blkatl_Prp2AssocObj);
	InitKey(&Blkatl_PrpAttr);
	InitKey(&Blkatl_PrpBit);
	InitKey(&Blkatl_PrpId);
	InitKey(&Blkatl_PrpNam);
	InitKey(&Blkatl_PrpOrder);
	InitKey(&Blkatl_PrpStr);
	InitKey(&Blkatl_PrpUID);
	InitKey(&Blkatl_Sym2Cmp);
	InitKey(&Blkatl_Version);
	InitKey(&Blkatl_Xtr2BPin);
	InitKey(&Blkatl_Xtr2Prps);
	InitKey(&Blkatl_XtrBPin);
	InitKey(&Blkatl_XtrNam);
	InitKey(&Blkatl_XtrTyp);
	InitKey(&Blkatl_XtrUID);
}