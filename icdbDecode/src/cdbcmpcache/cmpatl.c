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

/*
******************************************************************
* Includes
******************************************************************
*/
#include "cmpatl.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <string.h>					// Required for strcmp
#include "../common.h"				// Required for parseFile

/*
******************************************************************
* Global Variables
******************************************************************
*/
key_struct* Cmpatl_CmpPins = NULL;
key_struct* Cmpatl_CmpPrps = NULL;
key_struct* Cmpatl_Fixes = NULL;
key_struct* Cmpatl_Pin2Prps = NULL;
key_struct* Cmpatl_PinArrayWidth = NULL;
key_struct* Cmpatl_PinLibUID = NULL;
key_struct* Cmpatl_PinNam = NULL;
key_struct* Cmpatl_PinTyp = NULL;
key_struct* Cmpatl_PinUID = NULL;
key_struct* Cmpatl_PrpAttr = NULL;
key_struct* Cmpatl_PrpBit = NULL;
key_struct* Cmpatl_PrpId = NULL;
key_struct* Cmpatl_PrpNam = NULL;
key_struct* Cmpatl_PrpOrder = NULL;
key_struct* Cmpatl_PrpStr = NULL;
key_struct* Cmpatl_PrpUID = NULL;
key_struct* Cmpatl_Version = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessKeyCmpatl()
*
* - description: 	Check for cmpatl keys
*
* - parameter: 		file pointer; Key to check; length of key
*
* - return value: 	-
******************************************************************
*/
void ProcessKeyCmpatl(FILE * sourceFile, char* Key, unsigned int KeyLen)
{
	if (strcmp(Key, "CmpPins") == 0)
	{
		Cmpatl_CmpPins = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "CmpPrps") == 0)
	{
		Cmpatl_CmpPrps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Fixes") == 0)
	{
		Cmpatl_Fixes = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Pin2Prps") == 0)
	{
		Cmpatl_Pin2Prps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinArrayWidth") == 0)
	{
		Cmpatl_PinArrayWidth = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinLibUID") == 0)
	{
		Cmpatl_PinLibUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinNam") == 0)
	{
		Cmpatl_PinNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinTyp") == 0)
	{
		Cmpatl_PinTyp = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PinUID") == 0)
	{
		Cmpatl_PinUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpAttr") == 0)
	{
		Cmpatl_PrpAttr = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpBit") == 0)
	{
		Cmpatl_PrpBit = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpId") == 0)
	{
		Cmpatl_PrpId = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpNam") == 0)
	{
		Cmpatl_PrpNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpOrder") == 0)
	{
		Cmpatl_PrpOrder = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpStr") == 0)
	{
		Cmpatl_PrpStr = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpUID") == 0)
	{
		Cmpatl_PrpUID = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Version") == 0)
	{
		Cmpatl_Version = ParseKey(sourceFile);
	}
	else 
	{
		myPrint("Unknown Key in cmpatl [%s]\n", Key);
	}
}

/*
******************************************************************
* - function name:	InitCmpatl()
*
* - description: 	Resets all cmpatl data
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitCmpatl(void)
{
	InitKey(&Cmpatl_CmpPins);
	InitKey(&Cmpatl_CmpPrps);
	InitKey(&Cmpatl_Fixes);
	InitKey(&Cmpatl_Pin2Prps);
	InitKey(&Cmpatl_PinArrayWidth);
	InitKey(&Cmpatl_PinLibUID);
	InitKey(&Cmpatl_PinNam);
	InitKey(&Cmpatl_PinTyp);
	InitKey(&Cmpatl_PinUID);
	InitKey(&Cmpatl_PrpAttr);
	InitKey(&Cmpatl_PrpBit);
	InitKey(&Cmpatl_PrpId);
	InitKey(&Cmpatl_PrpNam);
	InitKey(&Cmpatl_PrpOrder);
	InitKey(&Cmpatl_PrpStr);
	InitKey(&Cmpatl_PrpUID);
	InitKey(&Cmpatl_Version);
}