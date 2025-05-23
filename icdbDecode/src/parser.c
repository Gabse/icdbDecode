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
* No static or dynamic code analysis of any proprietary executable files was used to gain informationís about the file format.
*
* This project uses the Zlib library (https://www.zlib.net/) for decompression.
*/

/*
******************************************************************
* Includes
******************************************************************
*/
#include "parser.h"
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include <stdlib.h>		// Required for calloc to work properly
#include "stringutil.h" // Required for assemblePath
#include "catlgatl.h"	// Required for parseCatlgatl
#include "kicad.h"		// Required for StoreAsKicadFile

/*
******************************************************************
* Defines
******************************************************************
*/
#define Exportpath "Export" // Path for exported files

/*
******************************************************************
* Function Prototypes
******************************************************************
*/
int parseSessionFolder(char*, uint32_t);
void makePathFromUID(char* output, uid_struct* input);


/*
******************************************************************
* Global Functions
******************************************************************
*/

/*
******************************************************************
* - function name:	ParseIcdb()
*
* - description: 	Icdb parser entry point
*
* - parameter: 		source path string, string length
*
* - return value: 	errorcode
******************************************************************
*/
int ParseIcdb(char* path, uint32_t pathlength)
{
	char* Path = NULL;
	uint32_t PathLen = 0;
	uint32_t error = 0;

	// do only s1 for now
	PathLen = assemblePath(&Path, path, pathlength, "s1", sizeof("s1"), DIR_SEPERATOR);
	error = parseSessionFolder(Path, PathLen);
	free(Path);

	return error;
}

/*
******************************************************************
* Local Functions
******************************************************************
*/
/*
******************************************************************
* - function name:	parseSessionFolder()
*
* - description: 	Parser for session folders
*
* - parameter: 		source path string, string length
*
* - return value: 	errorcode
******************************************************************
*/
int parseSessionFolder(char* path, uint32_t pathlength)
{
	char* Path = NULL;
	uint32_t PathLen = 0;
	uint32_t error = 0;
	char UIDpath[17];
	char* FullUIDpath = NULL;
	char* SubPath = NULL;
	uint32_t SubPathLen = 0;

	// Parse catlgatl
	PathLen = assemblePath(&Path, path, pathlength, "cdbcatlg", sizeof("cdbcatlg"), DIR_SEPERATOR);
	error = parseCatlgatl(Path, PathLen);
	free(Path);
	// Change to cdbblks
	PathLen = assemblePath(&Path, path, pathlength, "cdbblks", sizeof("cdbblks"), DIR_SEPERATOR);
	if (NumBlkUID == NumMdlNam && NumBlkUID >= 0 && BlkUID != NULL && MdlNam != NULL)
	{
		for (uint32_t i = 0; i < NumBlkUID; i++)
		{
			// Get filepath for block folder
			makePathFromUID((char*)&UIDpath, BlkUID + i);
			uint32_t FullUIDpathLength = addStrings(&FullUIDpath, UIDpath, 16, ".blk", 4, '\0');

			// Assemble filepath for block folder
			myPrint("Sheet [%s] is stored in [%s]\n", MdlNam[i].Text, FullUIDpath);
			SubPathLen = assemblePath(&SubPath, Path, PathLen, FullUIDpath, FullUIDpathLength, DIR_SEPERATOR);

			// Parse cdbblks
			error += parseCdbblks(SubPath, SubPathLen);
			error += StoreAsKicadFile(Exportpath, sizeof(Exportpath), MdlNam[i].Text, MdlNam[i].Lenth);
			free(SubPath);
		}
	}
	else
	{
		myPrint("Missmatch!, got NumBlkUID %d and NumMdlNam %d\n", NumBlkUID, NumMdlNam);
	}
	free(Path);
	return error;
}

/*
******************************************************************
* - function name:	makePathFromUID()
*
* - description: 	Transforms a UID into a string
*
* - parameter: 		destintion path, uid to convert
*
* - return value: 	-
******************************************************************
*/
void makePathFromUID(char* output, uid_struct* input)
{
	uint8_t temp = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		// Right part
		temp = (input->UID[i] & 0x0F);
		if (temp < 0x0A)
		{
			temp += '0'; // To ASCII
		}
		else
		{
			temp += 'a' - 0x0A; // To ASCII
		}
		output[i << 1] = temp;
		// Left part
		temp = ((input->UID[i] & 0xF0) >> 4);
		if (temp < 0x0A)
		{
			temp += '0'; // To ASCII
		}
		else
		{
			temp += 'a' - 0x0A; // To ASCII
		}
		output[(i << 1) + 1] = temp;
	}
	output[16] = '\0'; // Zero terminate string
}