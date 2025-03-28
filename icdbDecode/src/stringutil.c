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
#include "stringutil.h"
#include <stdlib.h>		// Required for calloc to work properly
#include <string.h>		// Required for memcpy
#include <stdint.h>		// Required for int32_t, uint32_t, ...
#include "common.h"		// Required for DIR_SEPERATOR

/*
******************************************************************
* - function: assemblePath()
*
* - desc: Assemble file path
*
* - par: pointer to string pointer of both texts united, string pointer to first part; lenth of first part; string pointer to second part; lenth of second part; sepeartor character
*
* - ret: complete path lenth
******************************************************************
*/
uint32_t assemblePath(char** destination, char* leftstring, uint32_t leftstringlength, char* rightstring, uint32_t rightstringlength, char seperator)
{
	uint32_t completePathLength = addStrings(destination, leftstring, leftstringlength, rightstring, rightstringlength, seperator);
	// replace slash/backslash
	for (uint32_t i = 0; i < completePathLength; i++)
	{
		if ((*destination)[i] == DIR_SEPERATOR_WINDOWS || (*destination)[i] == DIR_SEPERATOR_UNIX)
		{
			(*destination)[i] = DIR_SEPERATOR; // Replace Slash / Backslash
		}
	}
	return completePathLength;
}

/*
******************************************************************
* - function: createPath()
*
* - desc: Assemble file path & create folder structure
*
* - par: pointer to string pointer of both texts united, string pointer to first part; lenth of first part; string pointer to second part; lenth of second part; sepeartor character
*
* - ret: complete path lenth
******************************************************************
*/
uint32_t createPath(char** destination, char* leftstring, uint32_t leftstringlength, char* rightstring, uint32_t rightstringlength, char seperator)
{
	uint32_t completePathLength = addStrings(destination, leftstring, leftstringlength, rightstring, rightstringlength, seperator);
	// replace slash/backslash
	for (uint32_t i = 0; i < completePathLength; i++)
	{
		if ((*destination)[i] == DIR_SEPERATOR_WINDOWS || (*destination)[i] == DIR_SEPERATOR_UNIX)
		{
			(*destination)[i] = '\0'; // Zero termintate string for mkdir command
			#ifdef WIN32 // Building for Windows
				mkdir(*destination);
			#else // Building for Unix
				mkdir(*destination, 0777);
			#endif
			(*destination)[i] = DIR_SEPERATOR; // Replace Slash / Backslash
		}
	}	
	return completePathLength;
}

/*
******************************************************************
* - function: removeFileEnding()
*
* - desc: removes the file ending if pressent
*
* - par: pointer to string, pointer to lenth of string
*
* - ret: -
******************************************************************
*/
void removeFileEnding(char* string, uint32_t* lenth)
{
	if (string != 0)
	{
		// Remove .dat ending from database path if present
		for (uint32_t i = 0; i < lenth; i++)
		{
			if (string[i] == '.')
			{
				*lenth = i; // String lenth
				string[i] = '\0'; // Zero terminate string
				break;
			}
		}
	}
}

/*
******************************************************************
* - function: addStrings()
*
* - desc: adds two strings together
*
* - par: pointer to string pointer of both texts united, string pointer of first part; lenth of first part; string pointer of second part; lenth of second part; sepeartor character
*
* - ret: complete path lenth
******************************************************************
*/
uint32_t addStrings(char** destination, char* leftstring, uint32_t leftstringlength, char* rightstring, uint32_t rightstringlength, char seperator)
{
	if(leftstring[leftstringlength - 1] == '\0')
	{
		leftstringlength -= 1; // Remove zero termination	
	}
	if(rightstring[rightstringlength - 1] != '\0')
	{
		rightstringlength++; // Add space for zero termination if not already pressent
	}
	// Buffer for complete path
	uint32_t completePathLength = rightstringlength + leftstringlength;
	if(seperator != '\0')
	{
		completePathLength++; // add space for seperator
	}
	*destination = (char*)calloc(completePathLength, sizeof(char));
	if (*destination != 0)
	{
		// Unite the path
		memcpy((char*)*destination, leftstring, leftstringlength);
		if(seperator != '\0')
		{
			leftstringlength++; // add seperator
			(*destination)[leftstringlength - 1] = seperator;
		}
		memcpy((char*)*destination + leftstringlength, rightstring, rightstringlength);
		completePathLength--; // Remove zero termination
		(*destination)[completePathLength] = '\0'; // Zero terminate string
	}
	return completePathLength;
}