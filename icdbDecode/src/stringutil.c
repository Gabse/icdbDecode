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
* - function name : assemblePath()
*
* - description:  Assemble file path
*
* - parameter:  pointer to string pointer of both texts united, string pointer to first part; lenth of first part; string pointer to second part; lenth of second part; sepeartor character
*
* -  return value: complete path lenth
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
* - function name : createPath()
*
* - description:  Assemble file path & create folder structure
*
* - parameter:  pointer to string pointer of both texts united, string pointer to first part; lenth of first part; string pointer to second part; lenth of second part; sepeartor character
*
* -  return value: complete path lenth
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
* - function name : removeFilenameExtension()
*
* - description:  removes the file filename extension if pressent
*
* - parameter:  pointer to string, pointer to lenth of string
*
* -  return value: -
******************************************************************
*/
void removeFilenameExtension(char* string, uint32_t* lenth)
{
	if (string != 0)
	{
		// Remove filename extension from database path if present
		for (uint32_t i = *lenth; i > 0; i--)
		{
			if (string[i] == '.')
			{
				*lenth = i + 1; // String lenth
				string[i] = '\0'; // Zero terminate string
				break;
			}
			// File has no filename extension
			else if (string[i] == DIR_SEPERATOR_WINDOWS || (string[i] == DIR_SEPERATOR_UNIX))
			{
				break;
			}
		}
	}
}

/*
******************************************************************
* - function name : removeFilePath()
*
* - description:  seperates the filename from the filepath
*
* - parameter:  pointer to string, pointer to lenth of string, pointer to destination
*
* -  return value: lenth of filename
******************************************************************
*/
unsigned int removeFilePath(char* input, unsigned int lenth, char** output)
{
	unsigned int nameLen = lenth;
	*output = input;
	for (unsigned int i = 0; i < lenth; i++)
	{
		if (input[lenth - i] == DIR_SEPERATOR_WINDOWS || input[lenth - i] == DIR_SEPERATOR_UNIX)
		{
			nameLen = i - 1;
			*output = &(input[lenth - nameLen]);
			break;
		}
	}
	return nameLen;
}

/*
******************************************************************
* - function name : addStrings()
*
* - description:  adds two strings together
*
* - parameter:  pointer to string pointer of both texts united, string pointer of first part; lenth of first part; string pointer of second part; lenth of second part; sepeartor character
*
* -  return value: complete path lenth
******************************************************************
*/
uint32_t addStrings(char** destination, char* leftstring, uint32_t leftstringlength, char* rightstring, uint32_t rightstringlength, char seperator)
{
	// Handle if one of the parameter is zero
	uint32_t completePathLength;
	if ((leftstring == NULL || leftstringlength) <= 0 && (rightstring == NULL || rightstringlength <= 0))
	{
		*destination = NULL;
		return 0;
	}
	else if (leftstring == NULL || leftstringlength <= 0)
	{
		completePathLength = rightstringlength;
		*destination = (char*)calloc(completePathLength + 1, sizeof(char));
		if (*destination == 0)
		{
			return 0;
		}
		memcpy((char*)*destination, rightstring, completePathLength);
		(*destination)[completePathLength] = '\0'; // Zero terminate string
	}
	else if (rightstring == NULL || rightstringlength <= 0)
	{
		completePathLength = leftstringlength;
		*destination = (char*)calloc(completePathLength + 1, sizeof(char));
		if (*destination == 0)
		{
			return 0;
		}
		memcpy((char*)*destination, leftstring, completePathLength);
		(*destination)[completePathLength] = '\0'; // Zero terminate string
	}
	else
	{
		if (leftstring[leftstringlength - 1] == '\0')
		{
			leftstringlength -= 1; // Remove zero termination	
		}
		if (rightstring[rightstringlength - 1] != '\0')
		{
			rightstringlength++; // Add space for zero termination if not already pressent
		}
		// Buffer for complete path
		completePathLength = rightstringlength + leftstringlength;
		if (seperator != '\0')
		{
			completePathLength++; // add space for seperator
		}
		*destination = (char*)calloc(completePathLength, sizeof(char));
		if (*destination != 0)
		{
			// Unite the path
			memcpy((char*)*destination, leftstring, leftstringlength);
			if (seperator != '\0')
			{
				leftstringlength++; // add seperator
				(*destination)[leftstringlength - 1] = seperator;
			}
			memcpy((char*)*destination + leftstringlength, rightstring, rightstringlength);
			completePathLength--; // Remove zero termination
			(*destination)[completePathLength] = '\0'; // Zero terminate string
		}
	}
	return completePathLength;
}

/*
******************************************************************
* - function name : stringSmall()
*
* - description:  make first character lower case
*
* - parameter:  string pointer text; lenth of text
*
* -  return value: char pointer
******************************************************************
*/
char* stringSmall(char* name, unsigned int nameLen)
{
	// make first character lower case
	char* nameSmall = malloc(nameLen);
	if (nameSmall == NULL)
	{
		return NULL;
	}
	memcpy(nameSmall, name, nameLen);
	if (nameSmall[0] >= 'A' && nameSmall[0] <= 'Z')
	{
		nameSmall[0] = nameSmall[0] + 'a' - 'A';
	}
	return nameSmall;
}

/*
******************************************************************
* - function name : stringBig()
*
* - description:  make first character upper case
*
* - parameter:  string pointer text; lenth of text
*
* -  return value: char pointer
******************************************************************
*/
char* stringBig(char* name, unsigned int nameLen)
{
	char* nameBig = malloc(nameLen);
	if (nameBig == NULL)
	{
		return NULL;
	}
	memcpy(nameBig, name, nameLen);
	if (nameBig[0] >= 'a' && nameBig[0] <= 'z')
	{
		nameBig[0] = nameBig[0] + 'A' - 'a';
	}
	return nameBig;
}