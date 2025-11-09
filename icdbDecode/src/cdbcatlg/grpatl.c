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
#include "grpatl.h"
#include <stdint.h>					// Required for int32_t, uint32_t, ...
#include <string.h>					// Required for strcmp
#include "../common.h"				// Required for parseFile

/*
******************************************************************
* Global Variables
******************************************************************
*/
key_struct* Grpatl_Fixes = NULL;
key_struct* Grpatl_Group2Prps = NULL;
key_struct* Grpatl_GroupContentsType = NULL;
key_struct* Grpatl_GroupObjects = NULL;
key_struct* Grpatl_GroupSubGroups = NULL;
key_struct* Grpatl_GrpVersion = NULL;
key_struct* Grpatl_PrpAttr = NULL;
key_struct* Grpatl_PrpBit = NULL;
key_struct* Grpatl_PrpId = NULL;
key_struct* Grpatl_PrpNam = NULL;
key_struct* Grpatl_PrpOrder = NULL;
key_struct* Grpatl_PrpStr = NULL;

/*
******************************************************************
* Global Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	ProcessKeyGrpatl()
*
* - description: 	Check for grpatl keys
*
* - parameter: 		file pointer; Key to check; length of key
*
* - return value: 	-
******************************************************************
*/
void ProcessKeyGrpatl(FILE * sourceFile, char* Key, unsigned int KeyLen)
{
	if (strcmp(Key, "Fixes") == 0)
	{
		Grpatl_Fixes = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "Group2Prps") == 0)
	{
		Grpatl_Group2Prps = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupContentsType") == 0)
	{
		Grpatl_GroupContentsType = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupObjects") == 0)
	{
		Grpatl_GroupObjects = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GroupSubGroups") == 0)
	{
		Grpatl_GroupSubGroups = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "GrpVersion") == 0)
	{
		Grpatl_GrpVersion = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpAttr") == 0)
	{
		Grpatl_PrpAttr = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpBit") == 0)
	{
		Grpatl_PrpBit = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpId") == 0)
	{
		Grpatl_PrpId = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpNam") == 0)
	{
		Grpatl_PrpNam = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpOrder") == 0)
	{
		Grpatl_PrpOrder = ParseKey(sourceFile);
	}
	else if (strcmp(Key, "PrpStr") == 0)
	{
		Grpatl_PrpStr = ParseKey(sourceFile);
	}
	else 
	{
		myPrint("Unknown Key in grpatl [%s]\n", Key);
	}
}

/*
******************************************************************
* - function name:	InitGrpatl()
*
* - description: 	Resets all grpatl data
*
* - parameter: 		-
*
* - return value: 	-
******************************************************************
*/
void InitGrpatl(void)
{
	InitKey(&Grpatl_Fixes);
	InitKey(&Grpatl_Group2Prps);
	InitKey(&Grpatl_GroupContentsType);
	InitKey(&Grpatl_GroupObjects);
	InitKey(&Grpatl_GroupSubGroups);
	InitKey(&Grpatl_GrpVersion);
	InitKey(&Grpatl_PrpAttr);
	InitKey(&Grpatl_PrpBit);
	InitKey(&Grpatl_PrpId);
	InitKey(&Grpatl_PrpNam);
	InitKey(&Grpatl_PrpOrder);
	InitKey(&Grpatl_PrpStr);
}